/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/SphericalEnvironment.h"

#include "ocean/base/Subset.h"

#include "ocean/cv/FrameMean.h"

#include "ocean/cv/advanced/AdvancedFrameInterpolatorBilinear.h"
#include "ocean/cv/advanced/AdvancedMotion.h"

#include "ocean/cv/detector/FeatureDetector.h"
#include "ocean/cv/detector/HarrisCornerDetector.h"

#include "ocean/cv/segmentation/MaskCreator.h"

#include "ocean/geometry/Delaunay.h"
#include "ocean/geometry/NonLinearOptimizationOrientation.h"
#include "ocean/geometry/NonLinearUniversalOptimizationDense.h"
#include "ocean/geometry/NonLinearUniversalOptimizationSparse.h"
#include "ocean/geometry/RANSAC.h"
#include "ocean/geometry/Utilities.h"

namespace Ocean
{

namespace Tracking
{

/**
 * This class implements the base class for all optimization classes.
 */
class SphericalEnvironment::CameraData
{
	public:

		/**
		 * Creates a new data object.
		 * @param pinholeCamera The initial pinhole camera profile to be optimized
		 * @param imagePoints A set of image points located in the individual camera frames
		 * @param orientationImagePointPairGroups The groups of pairs combining unique feature point indices with their observation index from the set of image points, individually for each camera frame
		 */
		CameraData(const PinholeCamera& pinholeCamera, const ImagePoints& imagePoints, const PoseImagePointPairGroups& orientationImagePointPairGroups) :
			cameraWidth_(pinholeCamera.width()),
			cameraHeight_(pinholeCamera.height()),
			imagePoints_(imagePoints),
			orientationImagePointPairGroups_(orientationImagePointPairGroups)
		{
			// nothing to do here
		}

		/**
		 * Transforms the internal individual model to an external individual model.
		 * @param internalModel Internal model that will be transformed
		 * @param externalModel Resulting external model
		 */
		void transformIndividualModel(StaticBuffer<Scalar, 3>& internalModel, StaticBuffer<Scalar, 9>& externalModel)
		{
			const Pose pose(0, 0, 0, internalModel[0], internalModel[1], internalModel[2]);
			*((SquareMatrix3*)externalModel.data()) = pose.transformation().rotationMatrix();
		}

	protected:

		/**
		 * Determines the averaged image points for the default camera profile.
		 * @param pinholeCamera The current pinhole camera profile to be applied
		 * @param orientations The individual orientations of the camera frames
		 */
		void determineAveragedImagePoints(const PinholeCamera& pinholeCamera, const SquareMatrices3& orientations)
		{
			for (size_t n = 0; n < averagedFeaturePointRays_.size(); ++n)
			{
				averagedFeaturePointRays_[n] = Vector3(0, 0, 0);
			}

			for (size_t orientationIndex = 0; orientationIndex < orientationImagePointPairGroups_.size(); ++orientationIndex)
			{
				const SquareMatrix3& orientation = orientations[orientationIndex];
				const IndexPairs32& imagePointPairs = orientationImagePointPairGroups_[orientationIndex];

				for (size_t n = 0; n < imagePointPairs.size(); ++n)
				{
					const unsigned int imagePointIndex = imagePointPairs[n].first;
					const unsigned int featurePointIndex = imagePointPairs[n].second;

					if (featurePointIndex >= averagedFeaturePointRays_.size())
					{
						averagedFeaturePointRays_.resize(featurePointIndex + 1, Vector3(0, 0, 0));
					}

					const Vector2 undistortedImagePoint = pinholeCamera.undistortDamped(imagePoints_[imagePointIndex]);

					averagedFeaturePointRays_[featurePointIndex] += orientation * pinholeCamera.vector(undistortedImagePoint);
				}
			}

			for (size_t n = 0; n < averagedFeaturePointRays_.size(); ++n)
			{
				averagedFeaturePointRays_[n].normalize();
				ocean_assert(Numeric::isEqual(averagedFeaturePointRays_[n].length(), 1));
			}
		}

	protected:

		/// The width of the camera in pixel.
		const unsigned int cameraWidth_;

		/// The height of the camera in pixel.
		const unsigned int cameraHeight_;

		/// A set of image points located in the individual camera frames.
		const ImagePoints& imagePoints_;

		/// The groups pairs combining unique feature point indices with their observation index from the set of image points, individually for each camera frame.
		const PoseImagePointPairGroups& orientationImagePointPairGroups_;

		/// The positions of the feature points.
		Vectors3 averagedFeaturePointRays_;
};

/**
 * This class implements a data object necessary to optimize a camera's field of view for a set of camera frames captured with a camera with individual orientations.
 */
class SphericalEnvironment::CameraFovData : public SphericalEnvironment::CameraData
{
	public:

		/**
		 * Creates a new data object.
		 * @param pinholeCamera The initial pinhole camera profile to be optimized
		 * @param imagePoints A set of image points located in the individual camera frames
		 * @param orientationImagePointPairGroups The groups of pairs combining unique feature point indices with their observation index from the set of image points, individually for each camera frame
		 */
		CameraFovData(const PinholeCamera& pinholeCamera, const ImagePoints& imagePoints, const PoseImagePointPairGroups& orientationImagePointPairGroups) :
			CameraData(pinholeCamera, imagePoints, orientationImagePointPairGroups)
		{
			// nothing to do here
		}

		/**
		 * Determines the value for a specified camera frame and specified point for a given model.
		 * @param externalSharedModel The current shared model to be applied (the focal length parameter)
		 * @param externalIndividualModel The current individual model to be applied (the frame's orientation)
		 * @param individualModelIndex The index of the individual model (the index of the camera frame)
		 * @param elementIndex The index of the image point in the frame
		 */
		void value(const StaticBuffer<Scalar, 1>& externalSharedModel, const StaticBuffer<Scalar, 9>& externalIndividualModel, const size_t individualModelIndex, const size_t elementIndex, StaticBuffer<Scalar, 2>& result)
		{
			const PinholeCamera pinholeCamera(cameraWidth_, cameraHeight_, externalSharedModel[0], externalSharedModel[0], Scalar(cameraWidth_) * Scalar(0.5), Scalar(cameraHeight_) * Scalar(0.5));

			const SquareMatrix3& orientation = *((SquareMatrix3*)externalIndividualModel.data());

			const unsigned int featurePointIndex = orientationImagePointPairGroups_[individualModelIndex][elementIndex].second;

			const Vector2 point = pinholeCamera.projectToImageDamped(HomogenousMatrix4(orientation), averagedFeaturePointRays_[featurePointIndex], true);

			result[0] = point[0];
			result[1] = point[1];
		}

		/**
		 * Determines the error between a transformed point and the expected point for a given model.
		 * @param externalSharedModel The current shared model to be applied (the focal length parameter)
		 * @param externalIndividualModel The current individual model to be applied (the frame's orientation)
		 * @param individualModelIndex The index of the individual model (the index of the camera frame)
		 * @param elementIndex The index of the image point in the frame
		 * @return The resulting error (offset from the expected point to the transformed point)
		 */
		bool error(const StaticBuffer<Scalar, 1>& externalSharedModel, const StaticBuffer<Scalar, 9>& externalIndividualModel, const size_t individualModelIndex, const size_t elementIndex, StaticBuffer<Scalar, 2>& result)
		{
			const PinholeCamera pinholeCamera(cameraWidth_, cameraHeight_, externalSharedModel[0], externalSharedModel[0], Scalar(cameraWidth_) * Scalar(0.5), Scalar(cameraHeight_) * Scalar(0.5));

			const SquareMatrix3& orientation = *((SquareMatrix3*)externalIndividualModel.data());

			const unsigned int imagePointIndex = orientationImagePointPairGroups_[individualModelIndex][elementIndex].first;
			const unsigned int featurePointIndex = orientationImagePointPairGroups_[individualModelIndex][elementIndex].second;

			ocean_assert(featurePointIndex < averagedFeaturePointRays_.size());

			Vector2 error(pinholeCamera.projectToImageDamped(HomogenousMatrix4(orientation), averagedFeaturePointRays_[featurePointIndex], true) - imagePoints_[imagePointIndex]);

			result[0] = error[0];
			result[1] = error[1];

			return true;
		}

		/**
		 * Transforms the internal model to the external model (which is the same in our case).
		 * @param internalModel The internal model
		 * @param externalModel The external model
		 */
		void transformSharedModel(StaticBuffer<Scalar, 1>& internalModel, StaticBuffer<Scalar, 1>& externalModel)
		{
			externalModel = internalModel;
		}

		/**
		 * Accepts a new model that has been verified as a better model (or the first model).
		 * @param externalSharedModel The new external shared model
		 * @param externalIndividualModels The new external individual models
		 */
		void acceptModel(const StaticBuffer<Scalar, 1>& externalSharedModel, const std::vector< StaticBuffer<Scalar, 3> >& externalIndividualModels)
		{
			const PinholeCamera pinholeCamera(cameraWidth_, cameraHeight_, externalSharedModel[0], externalSharedModel[0], Scalar(cameraWidth_) * Scalar(0.5), Scalar(cameraHeight_) * Scalar(0.5));

			SquareMatrices3 orientations(externalIndividualModels.size());
			for (size_t n = 0; n < externalIndividualModels.size(); ++n)
			{
				const Pose pose(0, 0, 0, externalIndividualModels[n][0], externalIndividualModels[n][1], externalIndividualModels[n][2]);
				orientations[n] = SquareMatrix3(pose.transformation().rotationMatrix());
			}

			determineAveragedImagePoints(pinholeCamera, orientations);
		}
};

/**
 * This class implements a data object necessary to optimized the camera parameters and the camera orientations for a set of camera frames with individual orientations.
 */
class SphericalEnvironment::CameraOrientationsData : public SphericalEnvironment::CameraData
{
	public:

		/**
		 * Creates a new data object.
		 * @param pinholeCamera The initial pinhole camera profile which will be optimized
		 * @param imagePoints A set of image points located in the individual camera frames
		 * @param orientationImagePointPairGroups The groups of pairs combining unique feature point indices with their observation index from the set of image points, individually for each camera frame
		 */
		CameraOrientationsData(const PinholeCamera& pinholeCamera, const ImagePoints& imagePoints, const PoseImagePointPairGroups& orientationImagePointPairGroups) :
			CameraData(pinholeCamera, imagePoints, orientationImagePointPairGroups),
			camera_(pinholeCamera)
		{
			// nothing to do here
		}

		/**
		 * Determines the value for a specified camera frame and specified point for a given model.
		 * @param externalSharedModel The current shared model to be applied (the camera profile)
		 * @param externalIndividualModel The current individual model to be applied (the frame's orientation)
		 * @param individualModelIndex The index of the individual model (the index of the camera frame)
		 * @param elementIndex The index of the image point in the frame
		 * @param result The resulting transformed point
		 */
		void value(const StaticBuffer<Scalar, 8>& externalSharedModel, const StaticBuffer<Scalar, 9>& externalIndividualModel, const size_t individualModelIndex, const size_t elementIndex, StaticBuffer<Scalar, 2>& result)
		{
			const PinholeCamera pinholeCamera(camera_.width(), camera_.height(), externalSharedModel.data(), true, true);
			const SquareMatrix3& orientation = *((SquareMatrix3*)externalIndividualModel.data());

			const unsigned int featurePointIndex = orientationImagePointPairGroups_[individualModelIndex][elementIndex].second;

			const Vector2 point = pinholeCamera.projectToImageDamped(HomogenousMatrix4(orientation), averagedFeaturePointRays_[featurePointIndex], true);

			result[0] = point[0];
			result[1] = point[1];
		}

		/**
		 * Determines the error between a transformed point and the expected point for a given model.
		 * @param externalSharedModel The current shared model to be applied (the camera profile)
		 * @param externalIndividualModel The current individual model to be applied (the frame's orientation)
		 * @param individualModelIndex The index of the individual model (the index of the camera frame)
		 * @param elementIndex The index of the image point in the frame
		 * @return The resulting error (offset from the expected point to the transformed point)
		 */
		bool error(const StaticBuffer<Scalar, 8>& externalSharedModel, const StaticBuffer<Scalar, 9>& externalIndividualModel, const size_t individualModelIndex, const size_t elementIndex, StaticBuffer<Scalar, 2>& result)
		{
			const PinholeCamera pinholeCamera(camera_.width(), camera_.height(), externalSharedModel.data(), true, true);
			const SquareMatrix3& orientation = *((SquareMatrix3*)externalIndividualModel.data());

			const unsigned int imagePointIndex = orientationImagePointPairGroups_[individualModelIndex][elementIndex].first;
			const unsigned int featurePointIndex = orientationImagePointPairGroups_[individualModelIndex][elementIndex].second;

			ocean_assert(featurePointIndex < averagedFeaturePointRays_.size());

			Vector2 error(pinholeCamera.projectToImageDamped(HomogenousMatrix4(orientation), averagedFeaturePointRays_[featurePointIndex], true) - imagePoints_[imagePointIndex]);

			result[0] = error[0];
			result[1] = error[1];

			return true;
		}

		/**
		 * Transforms the internal model to the external model (which is the same in our case).
		 * @param internalModel The internal model
		 * @param externalModel The external model
		 */
		void transformSharedModel(StaticBuffer<Scalar, 8>& internalModel, StaticBuffer<Scalar, 8>& externalModel)
		{
			externalModel = internalModel;
		}

		/**
		 * Accepts a new model that has been verified as a better model (or the first model).
		 * @param sharedModel The new external shared model
		 * @param individualModels The new external individual models
		 */
		void acceptModel(const StaticBuffer<Scalar, 8>& sharedModel, const std::vector< StaticBuffer<Scalar, 3> >& individualModels)
		{
			const PinholeCamera pinholeCamera(camera_.width(), camera_.height(), sharedModel.data(), true, true);

			SquareMatrices3 orientations(individualModels.size());
			for (size_t n = 0; n < individualModels.size(); ++n)
			{
				const Pose pose(0, 0, 0, individualModels[n][0], individualModels[n][1], individualModels[n][2]);
				orientations[n] = SquareMatrix3(pose.transformation().rotationMatrix());
			}

			determineAveragedImagePoints(pinholeCamera, orientations);
		}

	protected:

		/// The initial camera profile which will be optimized.
		const PinholeCamera& camera_;
};

/**
 * This class implements a data object necessary to optimize a position lookup table (fine adjustment table) with relative offsets.
 * The lookup table has four bins, two in each direction (2x2).<br>
 * We create a lookup table with corner values so that we have 3 values in each direction.<br>
 */
class SphericalEnvironment::TransformationTableData2x2
{
	public:

		/**
		 * Creates a new data object.
		 * @param width The width of the lookup table, with range [1, infinity)
		 * @param height The height of the lookup table, with range [1, infinity)
		 * @param points0 The points lying in the domain of the lookup table, with ranges [0, width)x[0, height), for these points an offset lookup table will be determine
		 * @param points1 The points which are the target points, each point has a corresponding point in 'points0', with ranges (-infinity, infinity)x(-infinity, infinity)
		 */
		explicit TransformationTableData2x2(const unsigned int width, const unsigned int height, const Vectors2& points0, const Vectors2& points1) :
			width_(width),
			height_(height),
			points0_(points0),
			points1_(points1)
		{
#ifdef OCEAN_DEBUG
			ocean_assert(points0.size() == points1.size());
			for (size_t n = 0; n < points0.size(); ++n)
			{
				ocean_assert(points0[n].x() >= 0 && points0[n].x() < Scalar(width) && points0[n].y() >= 0 && points1[n].y() < Scalar(height));
			}
#endif
		}

		/**
		 * Determines the value for a specified point for a given model.
		 * @param externalModel The current model to be applied
		 * @param index The index of the point to be applied
		 * @param result The resulting transformed point
		 */
		void value(const StaticBuffer<Scalar, 18u>& externalModel, const size_t index, StaticBuffer<Scalar, 2>& result)
		{
			const LookupCorner2<Vector2> lookup(width_, height_, 2u, 2u, (Vector2*)externalModel.data());

			const Vector2 point = points0_[index] + lookup.bilinearValue(points0_[index].x(), points0_[index].y());

			result[0] = point[0];
			result[1] = point[1];
		}

		/**
		 * Determines the error between a transformed point and the expected point for a given model.
		 * @param externalModel The current model to be applied
		 * @param index The index of the point for which the error is determined
		 * @param result The resulting error (offset from the expected point to the transformed point)
		 * @return True, if succeeded
		 */
		bool error(const StaticBuffer<Scalar, 18>& externalModel, const size_t index, StaticBuffer<Scalar, 2>& result)
		{
			const LookupCorner2<Vector2> lookup(width_, height_, 2u, 2u, (Vector2*)externalModel.data());

			const Vector2 point = points0_[index] + lookup.bilinearValue(points0_[index].x(), points0_[index].y());

			const Vector2& measurementImagePoint = points1_[index];
			const Vector2 error(point - measurementImagePoint);

			result[0] = error[0];
			result[1] = error[1];
			return true;
		}

		/**
		 * Transforms the internal model to the external model (which is the same in our case).
		 * @param internalModel The internal model
		 * @param externalModel The external model
		 */
		void transformModel(StaticBuffer<Scalar, 18>& internalModel, StaticBuffer<Scalar, 18>& externalModel)
		{
			// make a copy as internal and external model are identical
			externalModel = internalModel;
		}

	protected:

		/// The width of the lookup table.
		const unsigned int width_;

		/// The height of the lookup table.
		const unsigned int height_;

		/// The points lying in the domain of the lookup table (the points for which a more accurate position will be determined).
		const Vectors2& points0_;

		/// The points lying not in the domain of the lookup table (the points which are expected).
		const Vectors2& points1_;
};

bool SphericalEnvironment::extendEnvironment(const PinholeCamera& pinholeCamera, const Frame& frame, const unsigned int approximationBinSize, const Geometry::Estimator::EstimatorType fineAdjustmentEstimator, const bool optimizeCamera, Worker* worker, SquareMatrix3* orientation, PinholeCamera* optimizedCamera, const FrameCallback& frameCallback)
{
	ocean_assert(isValid());

	if (!previousCamera_.isValid())
	{
		previousCamera_ = pinholeCamera;
	}

	ocean_assert(pinholeCamera.isValid() && frame.isValid() && frame.numberPlanes() == 1u);

	if (!pinholeCamera.isValid() || !frame.isValid() || frame.numberPlanes() != 1u)
	{
		return false;
	}

	CV::FramePyramid currentFramePyramid(frame, CV::FramePyramid::AS_MANY_LAYERS_AS_POSSIBLE, true /*copyFirstLayer*/, worker);

	if (!previousFramePyramid_.isValid())
	{
		if (!reset(pinholeCamera, frame, initialOrientation_, approximationBinSize, worker))
		{
			return false;
		}

		if (orientation)
		{
			*orientation = initialOrientation_;
		}

		if (optimizedCamera)
		{
			*optimizedCamera = pinholeCamera;
		}

		previousOrientation_ = initialOrientation_;
	}
	else
	{
		// we copy the given camera profile as the optimization may be necessary
		PinholeCamera currentCamera(pinholeCamera);

		constexpr unsigned int featureStengthThreshold = 15u;

		// track 2D points from (successive) frame to frame
		Vectors2 previousImagePoints, currentImagePoints;
		if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackArbitraryPointsBidirectionalSubPixelMirroredBorder<15u>(CV::FramePyramid(previousFramePyramid_, 0u, 6u, false), CV::FramePyramid(currentFramePyramid, 0u, 6u, false), 2u, previousImagePoints, currentImagePoints, Scalar(0.9 * 0.9), CV::SubRegion(), 20u, 20u, featureStengthThreshold, worker) || previousImagePoints.size() < 10)
		{
			return false;
		}

		// create 3D object points with orientation of the previous frame
		ocean_assert(!previousOrientation_.isSingular());
		Vectors3 previousObjectPoints(Geometry::Utilities::createObjectPoints(previousCamera_, HomogenousMatrix4(previousOrientation_), ConstArrayAccessor<Vector2>(previousImagePoints), previousCamera_.hasDistortionParameters(), 10));

		SquareMatrix3 optimizedOrientation;

		// determine orientation for the current frame (and optional optimize the camera profile
		if (optimizeCamera)
		{
			PinholeCamera internalOptimizedCamera;
			if (!Geometry::NonLinearOptimizationOrientation::optimizeCameraOrientation(currentCamera, previousOrientation_, ConstArrayAccessor<Vector3>(previousObjectPoints), ConstArrayAccessor<Vector2>(currentImagePoints), true, optimizedOrientation, internalOptimizedCamera, 20u, Geometry::Estimator::ET_HUBER, Scalar(0.001), Scalar(10)))
			{
				return false;
			}

			currentCamera = internalOptimizedCamera;
		}
		else
		{
			if (!Geometry::NonLinearOptimizationOrientation::optimizeOrientation(AnyCameraPinhole(currentCamera), previousOrientation_, ConstArrayAccessor<Vector3>(previousObjectPoints), ConstArrayAccessor<Vector2>(currentImagePoints), optimizedOrientation, 20u, Geometry::Estimator::ET_HUBER, Scalar(0.001), Scalar(10)))
			{
				return false;
			}
		}

		// now we have a valid orientation for the current frame
		SquareMatrix3 currentOrientation = optimizedOrientation;

		Frame correspondingPanoramaFrame(frame.frameType());
		Frame correspondingPanoramaMask(FrameType(frame, FrameType::FORMAT_Y8));

		// extract the panorama frame matching with the current frame to make the orientation more accurate
		if (!extractFrame(currentCamera, currentOrientation, correspondingPanoramaFrame, correspondingPanoramaMask, approximationBinSize, worker))
		{
			return false;
		}

		const CV::FramePyramid correspondingFramePyramid(correspondingPanoramaFrame, 5u, true /*copyFirstLayer*/, worker);

		previousImagePoints.clear();
		currentImagePoints.clear();

		// again track 2D points from the extracted frame to the current frame (now previousImagePoints are defined in the extracted panorama frame matching with the current orientation)
		if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackArbitraryPointsBidirectionalSubPixelMirroredBorder<15u>(correspondingFramePyramid, currentFramePyramid, 2u, previousImagePoints, currentImagePoints, Scalar(0.9 * 0.9), CV::SubRegion(correspondingPanoramaMask, CV::PixelBoundingBox(), 0xFF), 20u, 20u, featureStengthThreshold, worker) || previousImagePoints.size() < 10)
		{
			return false;
		}

		// again create 3D object points with the current orientation
		ocean_assert(!currentOrientation.isSingular());
		previousObjectPoints = Geometry::Utilities::createObjectPoints(currentCamera, HomogenousMatrix4(currentOrientation), ConstArrayAccessor<Vector2>(previousImagePoints), currentCamera.hasDistortionParameters(), 10);

		// determine the final highly accurate orientation for the current frame (and optional optimize the camera profile)
		if (optimizeCamera)
		{
			PinholeCamera internalOptimizedCamera;
			if (!Geometry::NonLinearOptimizationOrientation::optimizeCameraOrientation(currentCamera, currentOrientation, ConstArrayAccessor<Vector3>(previousObjectPoints), ConstArrayAccessor<Vector2>(currentImagePoints), true, optimizedOrientation, internalOptimizedCamera, 50u, Geometry::Estimator::ET_HUBER, Scalar(0.001), Scalar(10)))
			{
				return false;
			}

			currentCamera = internalOptimizedCamera;
		}
		else
		{
			if (!Geometry::NonLinearOptimizationOrientation::optimizeOrientation(AnyCameraPinhole(currentCamera), currentOrientation, ConstArrayAccessor<Vector3>(previousObjectPoints), ConstArrayAccessor<Vector2>(currentImagePoints), optimizedOrientation, 50u, Geometry::Estimator::ET_HUBER, Scalar(0.001), Scalar(10)))
			{
				return false;
			}
		}

		// now we have the highly optimized frame orientation
		currentOrientation = optimizedOrientation;

		Frame currentMask;
		if (frameCallback && !frameCallback(previousFramePyramid_, currentFramePyramid, previousCamera_, currentCamera, previousOrientation_, currentOrientation, maskValue(), currentMask, worker))
		{
			return false;
		}

		LookupTable fineAdjustmentTransformation;
		if (fineAdjustmentEstimator != Geometry::Estimator::ET_INVALID)
		{
			// finally extract the panorama frame matching with the current frame again
			if (!extractFrame(currentCamera, currentOrientation, correspondingPanoramaFrame, correspondingPanoramaMask, approximationBinSize, worker))
			{
				return false;
			}

			previousImagePoints.clear();
			currentImagePoints.clear();

			if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackArbitraryPointsBidirectionalSubPixelMirroredBorder<15u>(correspondingFramePyramid, currentFramePyramid, 2u, previousImagePoints, currentImagePoints, Scalar(0.9 * 0.9), CV::SubRegion(correspondingPanoramaMask, CV::PixelBoundingBox(), 0xFF), 30u, 30u, featureStengthThreshold, worker) || previousImagePoints.size() < 10)
			{
				return false;
			}

			Vectors2 validCameraPoints, validPanoramaPoints;

			if (currentMask)
			{
				validCameraPoints.reserve(currentImagePoints.size());
				validPanoramaPoints.reserve(previousImagePoints.size());

				for (size_t i = 0; i < previousImagePoints.size(); ++i)
				{
					if (currentMask.data<uint8_t>()[CV::PixelPosition::vector2pixelPosition(currentImagePoints[i]).index(currentMask.strideBytes(0u))] == maskValue())
					{
						validPanoramaPoints.push_back(previousImagePoints[i]);
						validCameraPoints.push_back(currentImagePoints[i]);
					}
				}
			}
			else
			{
				validCameraPoints = std::move(currentImagePoints);
				validPanoramaPoints = std::move(previousImagePoints);
			}

			if (validPanoramaPoints.size() >= 5)
			{
				determineTransformationTable2x2(currentCamera.width(), currentCamera.height(), validPanoramaPoints, validCameraPoints, fineAdjustmentTransformation, 20u, fineAdjustmentEstimator);
			}
		}

		if (!update(currentCamera, frame, currentMask, currentOrientation, approximationBinSize, worker, fineAdjustmentTransformation.isEmpty() ? nullptr : &fineAdjustmentTransformation))
		{
			return false;
		}

		if (orientation != nullptr)
		{
			*orientation = currentOrientation;
		}

		if (optimizedCamera != nullptr)
		{
			*optimizedCamera = currentCamera;
		}

		previousOrientation_ = currentOrientation;
		previousCamera_ = currentCamera;
	}

	previousFramePyramid_ = std::move(currentFramePyramid);

	return true;
}

bool SphericalEnvironment::optimizeOrientation(const PinholeCamera& pinholeCamera, const SquareMatrix3& orientation, const Frame& frame, const Frame& mask, const Geometry::Estimator::EstimatorType estimator, SquareMatrix3& optimizedOrientation, PinholeCamera* optimizedCamera, LookupTable* fineAdjustment, const unsigned int approximationBinSize, Worker* worker)
{
	ocean_assert(isValid());
	ocean_assert(pinholeCamera.isValid() && !orientation.isSingular());
	ocean_assert(frame.isValid());
	ocean_assert(!mask.isValid() || FrameType::formatIsGeneric(mask.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));

	Frame referenceFrame, referenceMask;
	if (!extractFrame(pinholeCamera, orientation, referenceFrame, referenceMask, approximationBinSize, worker))
	{
		return false;
	}

	ocean_assert(!mask.isValid() || (mask.width() == referenceMask.width() && mask.height() == referenceMask.height() && mask.pixelOrigin() == referenceMask.pixelOrigin()));

	if (mask)
	{
		CV::Segmentation::MaskCreator::joinMasks(mask.constdata<uint8_t>(), referenceMask.data<uint8_t>(), referenceMask.width(), referenceMask.height(), mask.paddingElements(), referenceMask.paddingElements(), maskValue(), worker);
	}

	CV::FramePyramid framePyramid(frame, 6u, false /*copyFirstLayer*/, worker);
	CV::FramePyramid referenceFramePyramid(referenceFrame, 6u, false /*copyFirstLayer*/, worker);

	Vectors2 referenceImagePoints, frameImagePoints;
	if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackArbitraryPointsBidirectionalSubPixelMirroredBorder<15u>(referenceFramePyramid, framePyramid, 2u, referenceImagePoints, frameImagePoints, Scalar(0.9 * 0.9), CV::SubRegion(std::move(referenceMask), CV::PixelBoundingBox(), maskValue()), 30u, 30u, 30u, worker) || referenceImagePoints.size() < 10)
	{
		return false;
	}

	ocean_assert(!referenceMask.isValid());

	const Vectors3 referenceObjectPoints(Geometry::Utilities::createObjectPoints(pinholeCamera, HomogenousMatrix4(orientation), ConstArrayAccessor<Vector2>(referenceImagePoints), pinholeCamera.hasDistortionParameters(), 10));

	// determine orientation for the current frame (and optional optimize the camera profile
	if (optimizedCamera)
	{
		if (!Geometry::NonLinearOptimizationOrientation::optimizeCameraOrientation(pinholeCamera, orientation, ConstArrayAccessor<Vector3>(referenceObjectPoints), ConstArrayAccessor<Vector2>(frameImagePoints), true, optimizedOrientation, *optimizedCamera, 20u, estimator, Scalar(0.001), Scalar(10)))
		{
			return false;
		}
	}
	else
	{
		if (!Geometry::NonLinearOptimizationOrientation::optimizeOrientation(AnyCameraPinhole(pinholeCamera), orientation, ConstArrayAccessor<Vector3>(referenceObjectPoints), ConstArrayAccessor<Vector2>(frameImagePoints), optimizedOrientation, 20u, estimator, Scalar(0.001), Scalar(10)))
		{
			return false;
		}
	}

	// if a fine adjustment lookup table is requested
	if (fineAdjustment)
	{
		// we extract the panorama reference frame with the optimized orientation and optional optimized camera
		if (!extractFrame(optimizedCamera ? *optimizedCamera : pinholeCamera, optimizedOrientation, referenceFrame, referenceMask, approximationBinSize, worker))
		{
			return false;
		}

		ocean_assert(!mask.isValid() || (mask.width() == referenceMask.width() && mask.height() == referenceMask.height() && mask.pixelOrigin() == referenceMask.pixelOrigin()));

		if (mask.isValid())
		{
			CV::Segmentation::MaskCreator::joinMasks(mask.constdata<uint8_t>(), referenceMask.data<uint8_t>(), referenceMask.width(), referenceMask.height(), mask.paddingElements(), referenceMask.paddingElements(), maskValue(), worker);
		}

		framePyramid.reduceLayers(4);
		referenceFramePyramid.reduceLayers(4);

		referenceImagePoints.clear();
		frameImagePoints.clear();

		if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackArbitraryPointsBidirectionalSubPixelMirroredBorder<31>(referenceFramePyramid, framePyramid, 2u, referenceImagePoints, frameImagePoints, Scalar(0.9 * 0.9), CV::SubRegion(std::move(referenceMask), CV::PixelBoundingBox(), maskValue()), 30u, 30u, 0u, worker) || referenceImagePoints.size() < 10)
		{
			return false;
		}

		ocean_assert(!referenceMask.isValid());

		if (!determineTransformationTable2x2(pinholeCamera.width(), pinholeCamera.height(), frameImagePoints, referenceImagePoints, *fineAdjustment))
		{
			return false;
		}
	}

	return true;
}

void SphericalEnvironment::clear()
{
	PanoramaFrame::clear();

	previousOrientation_.toNull();

	previousCamera_ = PinholeCamera();

	previousFramePyramid_.clear();
}

bool SphericalEnvironment::determinePointCorrespondencesHomography(const Frame& sourceFrame, const Frame& targetFrame, const SquareMatrix3& homography, Vectors2& sourcePoints, Vectors2& targetPoints, const unsigned int patchSize, const unsigned int maximalDistance, const unsigned int coarsestLayerRadius, const CV::FramePyramid::DownsamplingMode downsamplingMode, Worker* worker)
{
	ocean_assert(sourceFrame && targetFrame);
	ocean_assert(sourceFrame.frameType() == targetFrame.frameType());
	ocean_assert(!homography.isSingular());

	ocean_assert(patchSize == 7u || patchSize == 15u || patchSize == 31u);
	ocean_assert(maximalDistance >= 1u && coarsestLayerRadius >= 2u);

	// pointTargetFrame = H * pointSourceFrame

	SquareMatrix3 invHomography;

	if (!homography.invert(invHomography))
	{
		return false;
	}

	// pointSourceFrame = (H^-1) * pointTargetFrame

	// we transform the targetFrame frame so that it matches (aligns) with the source frame (not including the translation)
	// however, we do not need the entire transformed second frame but the intersecting image content only

	Box2 transformedTargetBoundingBox;
	transformedTargetBoundingBox += invHomography * Vector2(0, 0);
	transformedTargetBoundingBox += invHomography * Vector2(Scalar(targetFrame.width()), 0);
	transformedTargetBoundingBox += invHomography * Vector2(0, Scalar(targetFrame.height()));
	transformedTargetBoundingBox += invHomography * Vector2(Scalar(targetFrame.width()), Scalar(targetFrame.height()));

	const unsigned int extraBorder = maximalDistance * 2u;

	const Box2 sourceBoundingBox(Vector2(0, 0), Vector2(Scalar(sourceFrame.width()), Scalar(sourceFrame.height())));
	const Box2 enlargedSourceBoundingBox(sourceBoundingBox.center(), sourceBoundingBox.width() + Scalar(extraBorder * 2u), sourceBoundingBox.height() + Scalar(extraBorder * 2u));
	const Box2 intersectionBoundingBox = enlargedSourceBoundingBox.intersection(transformedTargetBoundingBox);

	int subRegionLeft, subRegionTop;
	unsigned int subRegionWidth, subRegionHeight;

	if (!intersectionBoundingBox.box2integer(-int(extraBorder), -int(extraBorder), sourceFrame.width() + extraBorder, sourceFrame.height() + extraBorder, subRegionLeft, subRegionTop, subRegionWidth, subRegionHeight))
	{
		return false;
	}

	ocean_assert(subRegionWidth <= sourceFrame.width() + 2u * extraBorder + 2u);
	ocean_assert(subRegionHeight <= sourceFrame.height() + 2u * extraBorder + 2u);

	if (subRegionWidth <= 15u || subRegionHeight <= 15u)
	{
		return false;
	}

	// now we actually transform the target frame

	Frame transformedTarget(FrameType(targetFrame, subRegionWidth, subRegionHeight));
	transformedTarget.setValue(0x00u);

	// **NOTE** we should also investigate the resulting mask ensuring that we do not include the 'black border color' during tracking, this could improve the tracking quality at the border of the transformed target frame

	if (!CV::FrameInterpolatorBilinear::Comfort::homography(targetFrame, transformedTarget, homography, nullptr, worker, CV::PixelPositionI(subRegionLeft, subRegionTop)))
	{
		return false;
	}

	// now we determine strong feature points in the source frame (in the intersection area only)

	const unsigned int horizontalBins = (unsigned int)Numeric::round32(intersectionBoundingBox.width() / Scalar(5));
	const unsigned int verticalBins = (unsigned int)Numeric::round32(intersectionBoundingBox.height() / Scalar(5));

	Vectors2 sourcePointCandidates = CV::Detector::FeatureDetector::determineHarrisPoints(sourceFrame, CV::SubRegion(intersectionBoundingBox), horizontalBins, verticalBins, 35u, worker);

	// although the patch size has been defined from the caller we may need to reduce the patch size

	unsigned int adjustedPatchSize = patchSize;
	unsigned int pyramidLayers = 1u;

	while (true)
	{
		const unsigned int minimalLayerDimension = adjustedPatchSize * 2u;

		const unsigned int pyramidLayersSource = CV::FramePyramid::idealLayers(sourceFrame.width(), sourceFrame.height(), minimalLayerDimension, minimalLayerDimension, 2u, maximalDistance, coarsestLayerRadius);
		const unsigned int pyramidLayersTransformedTarget = CV::FramePyramid::idealLayers(transformedTarget.width(), transformedTarget.height(), minimalLayerDimension, minimalLayerDimension, 2u, maximalDistance, coarsestLayerRadius);

		pyramidLayers = std::max(1u, std::min(pyramidLayersSource, pyramidLayersTransformedTarget));

		// we stop if we have the smallest patch size already
		if (adjustedPatchSize <= 7u)
		{
			break;
		}

		// we stop if we can reach the maximal distance
		if ((1u << (pyramidLayers - 1u)) * coarsestLayerRadius >= maximalDistance)
		{
			break;
		}

		adjustedPatchSize /= 2u;
	}

	ocean_assert(pyramidLayers >= 1u);

	const CV::FramePyramid sourceFramePyramid(sourceFrame, downsamplingMode, pyramidLayers, false /*copyFirstLayer*/, worker);
	const CV::FramePyramid transformedTargetPyramid(transformedTarget, downsamplingMode, pyramidLayers, false /*copyFirstLayer*/, worker);

	// in general, now the source frame and the transformed target frame align so that source points and target points should be almost identical
	// however, as the transformed target frame has an translational offset we need to adjust the locations accordingly

	Vectors2 roughTransformedPointCandidates;
	roughTransformedPointCandidates.reserve(sourcePointCandidates.size());

	for (size_t n = 0; n < sourcePointCandidates.size(); ++n)
	{
		const Vector2 roughTransformedPoint = sourcePointCandidates[n] - Vector2(Scalar(subRegionLeft), Scalar(subRegionTop));

		ocean_assert(roughTransformedPoint.x() >= 0 && roughTransformedPoint.y() >= 0 && roughTransformedPoint.x() < Scalar(transformedTarget.width()) && roughTransformedPoint.y() < Scalar(transformedTarget.height()));
		roughTransformedPointCandidates.push_back(roughTransformedPoint);
	}

	Vectors2 transformedPoints;

	switch (adjustedPatchSize)
	{
		case 7u:
			if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsBidirectionalSubPixelMirroredBorderWithRoughLocations<7u>(sourceFramePyramid, transformedTargetPyramid, coarsestLayerRadius, sourcePointCandidates, roughTransformedPointCandidates, transformedPoints, Scalar(0.9 * 0.9), worker))
			{
				return false;
			}

			break;

		case 31u:
			if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsBidirectionalSubPixelMirroredBorderWithRoughLocations<31u>(sourceFramePyramid, transformedTargetPyramid, coarsestLayerRadius, sourcePointCandidates, roughTransformedPointCandidates, transformedPoints, Scalar(0.9 * 0.9), worker))
			{
				return false;
			}

			break;

		default:
			ocean_assert(patchSize == 15u);
			if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsBidirectionalSubPixelMirroredBorderWithRoughLocations<15u>(sourceFramePyramid, transformedTargetPyramid, coarsestLayerRadius, sourcePointCandidates, roughTransformedPointCandidates, transformedPoints, Scalar(0.9 * 0.9), worker))
			{
				return false;
			}

			break;
	}

	ocean_assert(sourcePointCandidates.size() == transformedPoints.size());

	// we need to translate the tracked locations in the transformed target frame to locations into the normal (not transformed) target frame
	// some of the tracked target points may be slightly out of the target frame so that we also filter/remove those correspondences

	sourcePoints.clear();
	sourcePoints.reserve(sourcePointCandidates.size());

	targetPoints.clear();
	targetPoints.reserve(sourcePointCandidates.size());

	for (size_t n = 0; n < transformedPoints.size(); ++n)
	{
		const Vector2 targetPoint = homography * (transformedPoints[n] + Vector2(Scalar(subRegionLeft), Scalar(subRegionTop)));

		if (targetPoint.x() >= 0 && targetPoint.y() >= 0 && targetPoint.x() < Scalar(targetFrame.width()) && targetPoint.y() < Scalar(targetFrame.height()))
		{
			sourcePoints.push_back(sourcePointCandidates[n]);
			targetPoints.push_back(targetPoint);
		}
	}

	ocean_assert(sourcePoints.size() == targetPoints.size());

	return true;
}

bool SphericalEnvironment::determinePointCorrespondencesHomography(const CV::FramePyramid& sourceFramePyramid, const Frame& targetFrame, const SquareMatrix3& homography, const Vectors2& sourcePointCandidates, Vectors2& validSourcePoints, Vectors2& validTargetPoints, Indices32& validSourcePointIndices, const unsigned int patchSize, const unsigned int maximalDistance, const unsigned int coarsestLayerRadius, const CV::FramePyramid::DownsamplingMode downsamplingMode, Worker* worker, size_t* usedPointCandidates)
{
	ocean_assert(sourceFramePyramid.layers() >= 1u && targetFrame);
	ocean_assert(sourceFramePyramid.frameType() == targetFrame.frameType());
	ocean_assert(!homography.isSingular());

	ocean_assert(sourcePointCandidates.size() >= 1);

	ocean_assert(patchSize == 7u || patchSize == 15u || patchSize == 31u);
	ocean_assert(maximalDistance >= 1u && coarsestLayerRadius >= 2u);

	// pointTargetFrame = H * pointSourceFrame

	SquareMatrix3 invHomography;
	if (!homography.invert(invHomography))
	{
		return false;
	}

	// pointSourceFrame = (H^-1) * pointTargetFrame

	// we transform the targetFrame frame so that it matches (aligns) with the source frame (not including the translation)
	// however, we do not need the entire transformed second frame but the intersecting image content only

	Box2 transformedTargetBoundingBox;
	transformedTargetBoundingBox += invHomography * Vector2(0, 0);
	transformedTargetBoundingBox += invHomography * Vector2(Scalar(targetFrame.width()), 0);
	transformedTargetBoundingBox += invHomography * Vector2(0, Scalar(targetFrame.height()));
	transformedTargetBoundingBox += invHomography * Vector2(Scalar(targetFrame.width()), Scalar(targetFrame.height()));

	const unsigned int extraBorder = maximalDistance * 2u;

	const Box2 sourceBoundingBox(Vector2(0, 0), Vector2(Scalar(sourceFramePyramid.finestWidth()), Scalar(sourceFramePyramid.finestHeight())));
	const Box2 enlargedSourceBoundingBox(sourceBoundingBox.center(), sourceBoundingBox.width() + Scalar(extraBorder * 2u), sourceBoundingBox.height() + Scalar(extraBorder * 2u));
	const Box2 intersectionBoundingBox = enlargedSourceBoundingBox.intersection(transformedTargetBoundingBox);

	int subRegionLeft, subRegionTop;
	unsigned int subRegionWidth, subRegionHeight;
	if (!intersectionBoundingBox.box2integer(-int(extraBorder), -int(extraBorder), sourceFramePyramid.finestWidth() + extraBorder, sourceFramePyramid.finestHeight() + extraBorder, subRegionLeft, subRegionTop, subRegionWidth, subRegionHeight))
	{
		return false;
	}

	ocean_assert(subRegionWidth <= sourceFramePyramid.finestWidth() + 2u * extraBorder + 2u);
	ocean_assert(subRegionHeight <= sourceFramePyramid.finestHeight() + 2u * extraBorder + 2u);

	if (subRegionWidth <= 15u || subRegionHeight <= 15u)
	{
		return false;
	}

	// now we actually transform the target frame

	Frame transformedTarget(FrameType(targetFrame, subRegionWidth, subRegionHeight));
	transformedTarget.setValue(0x00u);

	// **NOTE** we should also investigate the resulting mask ensuring that we do not include the 'black border color' during tracking, this could improve the tracking quality at the border of the transformed target frame

	if (!CV::FrameInterpolatorBilinear::Comfort::homography(targetFrame, transformedTarget, homography, nullptr, worker, CV::PixelPositionI(subRegionLeft, subRegionTop)))
	{
		return false;
	}

	// now we determine the feature points in the source frame (in the intersection area only)

	Vectors2 subRegionSourcePoints;
	subRegionSourcePoints.reserve(sourcePointCandidates.size());

	Indices32 subRegionValidIndices;
	subRegionValidIndices.reserve(sourcePointCandidates.size());

	for (size_t n = 0; n < sourcePointCandidates.size(); ++n)
	{
		if (intersectionBoundingBox.isInside(sourcePointCandidates[n]))
		{
			subRegionSourcePoints.push_back(sourcePointCandidates[n]);
			subRegionValidIndices.push_back(Index32(n));
		}
	}

	if (subRegionSourcePoints.size() < 5)
	{
		return false;
	}

	if (usedPointCandidates)
		*usedPointCandidates = subRegionSourcePoints.size();

	// although the patch size has been defined from the caller we may need to reduce the patch size

	unsigned int adjustedPatchSize = patchSize;
	unsigned int pyramidLayers = 1u;

	while (true)
	{
		const unsigned int minimalLayerDimension = adjustedPatchSize * 2u;

		const unsigned int pyramidLayersTransformedTarget = CV::FramePyramid::idealLayers(transformedTarget.width(), transformedTarget.height(), minimalLayerDimension, minimalLayerDimension, 2u, maximalDistance, coarsestLayerRadius);

		pyramidLayers = std::max(1u, std::min(sourceFramePyramid.layers(), pyramidLayersTransformedTarget));

		// we stop if we have the smallest patch size already
		if (adjustedPatchSize <= 7u)
		{
			break;
		}

		// we stop if we can reach the maximal distance
		if ((1u << (pyramidLayers - 1u)) * coarsestLayerRadius >= maximalDistance)
		{
			break;
		}

		adjustedPatchSize /= 2u;
	}

	ocean_assert(pyramidLayers >= 1u);

	const CV::FramePyramid transformedTargetPyramid(transformedTarget, downsamplingMode, pyramidLayers, false /*copyFirstLayer*/, worker);

	// in general, now the source frame and the transformed target frame align so that source points and target points should be almost identical
	// however, as the transformed target frame has an translational offset we need to adjust the locations accordingly

	Vectors2 roughTransformedPointCandidates;
	roughTransformedPointCandidates.reserve(subRegionSourcePoints.size());

	for (size_t n = 0; n < subRegionSourcePoints.size(); ++n)
	{
		const Vector2 roughTransformedPoint = subRegionSourcePoints[n] - Vector2(Scalar(subRegionLeft), Scalar(subRegionTop));

		ocean_assert(roughTransformedPoint.x() >= 0 && roughTransformedPoint.y() >= 0 && roughTransformedPoint.x() < Scalar(transformedTarget.width()) && roughTransformedPoint.y() < Scalar(transformedTarget.height()));
		roughTransformedPointCandidates.push_back(roughTransformedPoint);
	}

	Vectors2 transformedPoints;
	Indices32 trackedValidIndices;

	switch (adjustedPatchSize)
	{
		case 7u:
		{
			if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsBidirectionalSubPixelMirroredBorderWithRoughLocations<7u>(sourceFramePyramid, transformedTargetPyramid, coarsestLayerRadius, subRegionSourcePoints, roughTransformedPointCandidates, transformedPoints, Scalar(0.9 * 0.9), worker, &trackedValidIndices))
			{
				return false;
			}

			break;
		}

		case 31u:
		{
			if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsBidirectionalSubPixelMirroredBorderWithRoughLocations<31u>(sourceFramePyramid, transformedTargetPyramid, coarsestLayerRadius, subRegionSourcePoints, roughTransformedPointCandidates, transformedPoints, Scalar(0.9 * 0.9), worker, &trackedValidIndices))
			{
				return false;
			}

			break;
		}

		default:
		{
			ocean_assert(patchSize == 15u);
			if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsBidirectionalSubPixelMirroredBorderWithRoughLocations<15u>(sourceFramePyramid, transformedTargetPyramid, coarsestLayerRadius, subRegionSourcePoints, roughTransformedPointCandidates, transformedPoints, Scalar(0.9 * 0.9), worker, &trackedValidIndices))
			{
				return false;
			}

			break;
		}
	}

	ocean_assert(subRegionSourcePoints.size() == transformedPoints.size());

	// we need to translate the tracked locations in the transformed target frame to locations into the normal (not transformed) target frame
	// some of the tracked target points may be slightly out of the target frame so that we also filter/remove those correspondences

	validSourcePoints.clear();
	validSourcePoints.reserve(trackedValidIndices.size());

	validTargetPoints.clear();
	validTargetPoints.reserve(trackedValidIndices.size());

	validSourcePointIndices.clear();
	validSourcePointIndices.reserve(trackedValidIndices.size());

	for (size_t n = 0; n < trackedValidIndices.size(); ++n)
	{
		const Index32 validTrackedIndex = trackedValidIndices[n];

		ocean_assert(validTrackedIndex < transformedPoints.size());
		const Vector2& validTransformedPoint = transformedPoints[validTrackedIndex];

		const Vector2 targetPoint = homography * (validTransformedPoint + Vector2(Scalar(subRegionLeft), Scalar(subRegionTop)));

		if (targetPoint.x() >= 0 && targetPoint.y() >= 0 && targetPoint.x() < Scalar(targetFrame.width()) && targetPoint.y() < Scalar(targetFrame.height()))
		{
			ocean_assert(validTrackedIndex < subRegionSourcePoints.size());

			validSourcePoints.push_back(subRegionSourcePoints[validTrackedIndex]);
			validTargetPoints.push_back(targetPoint);

			ocean_assert(validTrackedIndex < subRegionValidIndices.size());

			const Index32 validIndex = subRegionValidIndices[validTrackedIndex];

			ocean_assert(validIndex < sourcePointCandidates.size());
			validSourcePointIndices.push_back(validIndex);
		}
	}

	ocean_assert(validSourcePoints.size() == validTargetPoints.size());

	return true;
}

bool SphericalEnvironment::nonHomographyMask(const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, const PinholeCamera& previousCamera, const PinholeCamera& currentCamera, const SquareMatrix3& previousOrientation, const SquareMatrix3& currentOrientation, const uint8_t maskValue, Frame& currnetMask, Worker* worker)
{
	Frame adjustedPreviousFrame;
	Frame adjustedPreviousMask;
	if (!CV::Advanced::PanoramaFrame::cameraFrame2cameraFrame(previousCamera, previousOrientation, previousFramePyramid.finestLayer(), Frame(), currentCamera, currentOrientation, adjustedPreviousFrame, adjustedPreviousMask, maskValue, 20u, worker))
	{
		return false;
	}

	const CV::FramePyramid adjustedPreviousFramePyramid(adjustedPreviousFrame, currentFramePyramid.layers(), false /*copyFirstLayer*/, worker);

	Vectors2 previousImagePoints, currentImagePoints;
	if (!CV::Advanced::AdvancedMotionSSD::trackArbitraryPointsBidirectionalSubPixelMirroredBorder<5u>(adjustedPreviousFramePyramid, currentFramePyramid, 2u, previousImagePoints, currentImagePoints, Scalar(1), CV::SubRegion(adjustedPreviousMask, CV::PixelBoundingBox(), maskValue), 30u, 30u, 25u, worker))
	{
		return false;
	}

	// create border points for the Delaunay triangulation
	for (unsigned int n = 0u; n <= 10u; ++n)
	{
		const Scalar x = Scalar((currentCamera.width() - 1u) * Scalar(n) * Scalar(0.1));
		const Scalar y = Scalar((currentCamera.height() - 1u) * Scalar(n) * Scalar(0.1));

		currentImagePoints.emplace_back(x, Scalar(0));
		previousImagePoints.emplace_back(x, Scalar(0));

		currentImagePoints.emplace_back(x, Scalar(currentCamera.height() - 1u));
		previousImagePoints.emplace_back(x, Scalar(currentCamera.height() - 1u));

		currentImagePoints.emplace_back(Scalar(0), y);
		previousImagePoints.emplace_back(Scalar(0), y);

		currentImagePoints.emplace_back(Scalar(currentCamera.width() - 1u), y);
		previousImagePoints.emplace_back(Scalar(currentCamera.width() - 1u), y);
	}

	const Geometry::Delaunay::IndexTriangles triangleIndices = Geometry::Delaunay::triangulation(currentImagePoints);

	if (!currnetMask.set(FrameType(currentFramePyramid.frameType(), FrameType::FORMAT_Y8), false /*forceOwner*/, true /*forceWritable*/))
	{
		return false;
	}

	currnetMask.setValue(maskValue);

	for (size_t n = 0; n < triangleIndices.size(); ++n)
	{
		for (unsigned int i = 0u; i < 3u; ++i)
		{
			const unsigned int index = triangleIndices[n][i];

			if (currentImagePoints[index].sqrDistance(previousImagePoints[index]) > 9)
			{
				CV::Segmentation::MaskCreator::triangle2inclusiveMask(currnetMask.data<uint8_t>(), currnetMask.width(), currnetMask.height(), currnetMask.paddingElements(), CV::Advanced::PixelTriangle(triangleIndices[n].triangle2(currentImagePoints.data()), currentCamera.width(), currentCamera.height()), 0xFF - maskValue);
				break;
			}
		}
	}

	return true;
}

std::vector<Indices64> SphericalEnvironment::determineFeaturePointsFromPointCloud(const FeaturePointMap& correspondences, unsigned int minSiblings)
{
	IndexMap frameSet;

	UnorderedIndexSet64 usedSet;
	UnorderedIndexSet64 siblings;

	std::vector<Indices64> result;

	FeaturePointMap::const_iterator i = correspondences.begin();
	while (i != correspondences.end())
	{
		frameSet.clear();
		siblings.clear();

		i = determineSiblings(correspondences, i, siblings, usedSet, frameSet);

		if (!siblings.empty())
		{
			bool doublePoint = false;

			for (IndexMap::const_iterator iF = frameSet.begin(); iF != frameSet.end(); ++iF)
			{
				if (iF->second > 1u)
				{
					doublePoint = true;
					break;
				}
			}

			if (!doublePoint)
			{
				ocean_assert(siblings.size() != 0);

				if (siblings.size() >= minSiblings)
				{
					result.emplace_back(siblings.begin(), siblings.end());
				}
			}
		}
	}

	return result;
}

SphericalEnvironment::FeaturePointMap::const_iterator SphericalEnvironment::determineSiblings(const FeaturePointMap& correspondences, const FeaturePointMap::const_iterator& iStart, UnorderedIndexSet64& siblings, UnorderedIndexSet64& usedSet, IndexMap& frameSet)
{
	ocean_assert(iStart != correspondences.end());

	FeaturePointMap::const_iterator i = iStart;
	if (usedSet.find(i->first) != usedSet.end())
	{
		return ++i;
	}

	usedSet.insert(i->first);
	siblings.insert(i->first);
	frameSet[frameIndex(i->first)]++;

	do
	{
		determineSiblings(correspondences, correspondences.find(i->second), siblings, usedSet, frameSet);
		++i;
	}
	while (i != correspondences.end() && i->first == iStart->first);

	return i;
}

bool SphericalEnvironment::optimizeCamera(const PinholeCamera& pinholeCamera, const Frames& frames, const SquareMatrices3& orientations, PinholeCamera& optimizedCamera, Worker* worker)
{
	ocean_assert(pinholeCamera);
	ocean_assert(frames.size() >= 2 && frames.size() == orientations.size());

	// determine strong feature points in all given image frames providing candidates for the final feature correspondences
	std::vector<Vectors2> cloudPoints;
	cloudPoints.reserve(frames.size());

	Buffers cloudBuffers;
	cloudBuffers.reserve(frames.size());

	std::vector<Geometry::SpatialDistribution::DistributionArray> cloudDistributions;
	cloudDistributions.reserve(frames.size());

	for (size_t n = 0; n < frames.size(); ++n)
	{
		ocean_assert(pinholeCamera.width() == frames[n].width() && pinholeCamera.height() == frames[n].height());

		CV::Detector::HarrisCorners corners;
		if (!CV::Detector::HarrisCornerDetector::detectCorners(frames[n], 25u, false, corners, true, worker))
		{
			return false;
		}

		Vectors2 points;
		points.reserve(corners.size());

		for (CV::Detector::HarrisCorners::const_iterator i = corners.begin(); i != corners.end(); ++i)
		{
			if (i->observation().x() >= Scalar(15u / 2u) && i->observation().y() >= Scalar(15u / 2u) && i->observation().x() < Scalar(pinholeCamera.width() - 15u / 2u - 1u) && i->observation().y() < Scalar(pinholeCamera.height() - 15u / 2u - 1u))
			{
				points.push_back(i->observation());
			}
		}

		cloudBuffers.emplace_back(interpolateSquarePatches<15>(frames[n], points, worker));
		cloudDistributions.emplace_back(Geometry::SpatialDistribution::distributeToArray(points.data(), points.size(), Scalar(0), Scalar(0), Scalar(pinholeCamera.width()), Scalar(pinholeCamera.height()), pinholeCamera.width() / 40u, pinholeCamera.height() / 40u));
		cloudPoints.emplace_back(std::move(points));
	}

	std::multimap<uint64_t, uint64_t> correspondenceMap;

	for (unsigned int a = 0u; a < frames.size() - 1; a++)
	{
		for (unsigned int b = a + 1u; b < frames.size(); ++b)
		{
			const IndexPairs32 bijectiveCorrespondences = findBijectiveCorrespondences<15u>(pinholeCamera, pinholeCamera, frames.front().pixelFormat(), orientations[a], orientations[b], cloudBuffers[a], cloudBuffers[b], cloudPoints[a], cloudPoints[b], cloudDistributions[a], cloudDistributions[b], worker);

			for (IndexPairs32::const_iterator i = bijectiveCorrespondences.begin(); i != bijectiveCorrespondences.end(); ++i)
			{
				correspondenceMap.insert(std::make_pair(uniqueFeaturePointId(a, i->first), uniqueFeaturePointId(b, i->second)));
				correspondenceMap.insert(std::make_pair(uniqueFeaturePointId(b, i->second), uniqueFeaturePointId(a, i->first)));
			}
		}
	}

	const std::vector<Indices64> siblingsSet = determineFeaturePointsFromPointCloud(correspondenceMap, 4u);

	PoseImagePointPairGroups orientationPointPairs(frames.size());

	Vectors2 imagePoints;
	imagePoints.reserve(countElements(siblingsSet));

	for (size_t n = 0; n < siblingsSet.size(); ++n)
	{
		const Indices64& featurePoint = siblingsSet[n];

		for (size_t i = 0; i < featurePoint.size(); ++i)
		{
			const uint32_t oi = frameIndex(featurePoint[i]);
			const uint32_t pi = pointIndex(featurePoint[i]);

			ocean_assert(oi < orientations.size());

			orientationPointPairs[oi].emplace_back(uint32_t(imagePoints.size()), uint32_t(n));
			imagePoints.emplace_back(cloudPoints[oi][pi]);
		}
	}

#if 0
	{
		SphericalEnvironment sphericalEnvironment(pinholeCamera.width() * 6u, pinholeCamera.width() * 3u, 0xFF, CV::Advanced::PanoramaFrame::UM_AVERAGE_GLOBAL);

		for (unsigned int n = 0u; n < frames.size(); ++n)
		{
			sphericalEnvironment.addFrame(pinholeCamera, orientations[n], frames[n], Frame(), 20u, worker);
		}

		Frame panoramaFrame(sphericalEnvironment.frame(), Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

		std::map<unsigned int, Vectors2> helperMap;
		for (size_t n = 0; n < orientationPointPairs.size(); ++n)
		{
			for (size_t i = 0; i < orientationPointPairs[n].size(); ++i)
			{
				const Vector2 panoramaPixel = sphericalEnvironment.cameraPixel2panoramaPixelStrict(pinholeCamera, orientations[n], imagePoints[orientationPointPairs[n][i].first]);

				helperMap[orientationPointPairs[n][i].second].push_back(Vector2(panoramaPixel.x() - Scalar(sphericalEnvironment.frameTopLeft().x()), panoramaPixel.y() - Scalar(sphericalEnvironment.frameTopLeft().y())));
			}
		}

		for (std::map<unsigned int, Vectors2>::const_iterator i = helperMap.begin(); i != helperMap.end(); ++i)
		{
			const Vectors2& points = i->second;

			const uint8_t green[3] = {0x00, 0xFF, 0x00};

			for (size_t a = 0; a < points.size() - 1; ++a)
			{
				for (size_t b = a + 1; b < points.size(); ++b)
				{
					Utilities::paintLine(panoramaFrame, points[a], points[b], green);
				}
			}
		}
	}
#endif


	PinholeCamera initialFovCamera;
	SquareMatrices3 initialFovOrientations;
	if (!findInitialFieldOfView(pinholeCamera.width(), pinholeCamera.height(), orientations, imagePoints, orientationPointPairs, initialFovCamera, initialFovOrientations))
	{
		return false;
	}

	SquareMatrices3 optimizedOrientations;
	Scalar initialError = 0, finalError = 0;
	if (!optimizeCamera(initialFovCamera, initialFovOrientations, imagePoints, orientationPointPairs, optimizedCamera, optimizedOrientations, 50u, Geometry::Estimator::ET_HUBER, Scalar(0.001), Scalar(5), &initialError, &finalError))
	{
		return false;
	}

#if 0
	{
		SphericalEnvironment sphericalEnvironment(pinholeCamera.width() * 6u, pinholeCamera.width() * 3u, 0xFF, CV::Advanced::PanoramaFrame::UM_AVERAGE_GLOBAL);

		for (unsigned int n = 0u; n < frames.size(); ++n)
		{
			sphericalEnvironment.addFrame(optimizedCamera, optimizedOrientations[n], frames[n], Frame(), 20u, worker);
		}

		Frame panoramaFrame(sphericalEnvironment.frame(), Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

		std::map<unsigned int, Vectors2> helperMap;
		for (size_t n = 0; n < orientationPointPairs.size(); ++n)
		{
			for (size_t i = 0; i < orientationPointPairs[n].size(); ++i)
			{
				const Vector2 panoramaPixel = sphericalEnvironment.cameraPixel2panoramaPixelStrict(optimizedCamera, optimizedOrientations[n], imagePoints[orientationPointPairs[n][i].first]);

				helperMap[orientationPointPairs[n][i].second].push_back(Vector2(panoramaPixel.x() - Scalar(sphericalEnvironment.frameTopLeft().x()), panoramaPixel.y() - Scalar(sphericalEnvironment.frameTopLeft().y())));
			}
		}

		for (std::map<unsigned int, Vectors2>::const_iterator i = helperMap.begin(); i != helperMap.end(); ++i)
		{
			const Vectors2& points = i->second;

			const uint8_t green[3] = {0x00, 0xFF, 0x00};

			for (size_t a = 0; a < points.size() - 1; ++a)
			{
				for (size_t b = a + 1; b < points.size(); ++b)
				{
					Utilities::paintLine(panoramaFrame, points[a], points[b], green);
				}
			}
		}
	}
#endif

	return true;
}

bool SphericalEnvironment::optimizeCamera(const PinholeCamera& pinholeCamera, const SquareMatrices3& orientations, const ImagePoints& imagePoints, const PoseImagePointPairGroups& orientationImagePointPairGroups, PinholeCamera& optimizedCamera, SquareMatrices3& optimizedOrientations, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(orientations.size() == orientationImagePointPairGroups.size());
	ocean_assert(orientations.size() >= 1);

#ifdef OCEAN_DEBUG

	// we ensure that each feature point index exists once

	IndexSet32 featurePointIndices;
	for (size_t o = 0; o < orientationImagePointPairGroups.size(); ++o)
	{
		for (size_t i = 0; i < orientationImagePointPairGroups[o].size(); ++i)
		{
			const unsigned int featurePointIndex = orientationImagePointPairGroups[o][i].second;

			featurePointIndices.insert(featurePointIndex);
		}
	}

	ocean_assert(!featurePointIndices.empty());
	ocean_assert(featurePointIndices.size() == *featurePointIndices.rbegin() + 1);

#endif

	// shared model: camera profile with 8 scalar parameters
	// individual model: camera orientation with 3 scalar parameters for each orientation

	typedef Geometry::NonLinearUniversalOptimizationSparse::SharedModelIndividualModels<8u, 3u, 2u, 8u, 9u> UniversalOptimization;

	UniversalOptimization::SharedModel sharedModel, optimizedSharedModel;
	pinholeCamera.copyElements(sharedModel.data());

	UniversalOptimization::IndividualModels individualModels, optimizedIndividualModels;
	for (SquareMatrices3::const_iterator i = orientations.begin(); i != orientations.end(); ++i)
	{
		const Pose orientation = Pose(Rotation(*i));

		UniversalOptimization::IndividualModel indiviudalModel(orientation.data() + 3);
		individualModels.emplace_back(indiviudalModel);
	}

	std::vector<size_t> numberElementsPerIndividualModel;
	numberElementsPerIndividualModel.reserve(orientationImagePointPairGroups.size());
	for (PoseImagePointPairGroups::const_iterator i = orientationImagePointPairGroups.begin(); i != orientationImagePointPairGroups.end(); ++i)
	{
		ocean_assert(i->size() != 0);
		numberElementsPerIndividualModel.push_back(i->size());
	}

	CameraOrientationsData data(pinholeCamera, imagePoints, orientationImagePointPairGroups);

	if (!UniversalOptimization::optimizeUniversalModel(sharedModel, individualModels, numberElementsPerIndividualModel.data(),
					UniversalOptimization::ValueCallback::create(data, &CameraOrientationsData::value),
					UniversalOptimization::ErrorCallback::create(data, &CameraOrientationsData::error),
					UniversalOptimization::SharedModelIsValidCallback(),
					UniversalOptimization::SharedModelTransformationCallback::create(data, &CameraOrientationsData::transformSharedModel),
					UniversalOptimization::IndividualModelTransformationCallback::create(data, &CameraOrientationsData::transformIndividualModel),
					UniversalOptimization::ModelAcceptedCallback::create(data, &CameraOrientationsData::acceptModel),
					optimizedSharedModel, optimizedIndividualModels, iterations, estimator, lambda, lambdaFactor, initialError, finalError, intermediateErrors))
	{
		return false;
	}


	optimizedCamera = PinholeCamera(pinholeCamera.width(), pinholeCamera.height(), optimizedSharedModel.data());

	optimizedOrientations.clear();
	optimizedOrientations.reserve(orientations.size());

	for (UniversalOptimization::IndividualModels::const_iterator i = optimizedIndividualModels.begin(); i != optimizedIndividualModels.end(); ++i)
	{
		const Pose pose(0, 0, 0, (*i)[0], (*i)[1], (*i)[2]);
		optimizedOrientations.emplace_back(pose.transformation().rotationMatrix());
	}

	return true;
}

bool SphericalEnvironment::findInitialFieldOfView(const unsigned int width, const unsigned int height, const SquareMatrices3& orientations, const ImagePoints& imagePoints, const PoseImagePointPairGroups& orientationImagePointPairGroups, PinholeCamera& optimizedCamera, SquareMatrices3& optimizedOrientations, const Scalar lowerFovX, const Scalar upperFovX, const unsigned int steps)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(orientations.size() == orientationImagePointPairGroups.size());
	ocean_assert(orientations.size() >= 1);

	ocean_assert(steps >= 2u);
	ocean_assert(lowerFovX <= upperFovX);

#ifdef OCEAN_DEBUG

	// we ensure that each feature point index exists once

	IndexSet32 featurePointIndices;
	for (size_t o = 0; o < orientationImagePointPairGroups.size(); ++o)
	{
		for (size_t i = 0; i < orientationImagePointPairGroups[o].size(); ++i)
		{
			const unsigned int featurePointIndex = orientationImagePointPairGroups[o][i].second;

			featurePointIndices.insert(featurePointIndex);
		}
	}

	ocean_assert(!featurePointIndices.empty());
	ocean_assert(featurePointIndices.size() == *featurePointIndices.rbegin() + 1);

#endif

	// shared model: camera profile with 1 scalar parameters
	// individual model: camera orientation with 3 scalar parameters for each orientation

	typedef Geometry::NonLinearUniversalOptimizationSparse::SharedModelIndividualModels<1u, 3u, 2u, 1u, 9u> UniversalOptimization;

	UniversalOptimization::SharedModel sharedModel, optimizedSharedModel;

	Scalar idealError = Numeric::maxValue();

	for (unsigned int n = 0u; n < steps; ++n)
	{
		const Scalar fovX = lowerFovX + Scalar(n) * (upperFovX - lowerFovX) / Scalar(steps - 1u);

		const PinholeCamera pinholeCamera(width, height, fovX);
		sharedModel[0] = pinholeCamera.focalLengthX();

		UniversalOptimization::IndividualModels individualModels, optimizedIndividualModels;
		for (SquareMatrices3::const_iterator i = orientations.begin(); i != orientations.end(); ++i)
		{
			const Pose orientation = Pose(Rotation(*i));

			UniversalOptimization::IndividualModel individualModel(orientation.data() + 3);
			individualModels.emplace_back(individualModel);
		}

		std::vector<size_t> numberElementsPerIndividualModel;
		numberElementsPerIndividualModel.reserve(orientationImagePointPairGroups.size());
		for (PoseImagePointPairGroups::const_iterator i = orientationImagePointPairGroups.begin(); i != orientationImagePointPairGroups.end(); ++i)
		{
			ocean_assert(i->size() != 0);
			numberElementsPerIndividualModel.push_back(i->size());
		}

		CameraFovData data(pinholeCamera, imagePoints, orientationImagePointPairGroups);

		Scalar finalError = Numeric::maxValue();
		if (UniversalOptimization::optimizeUniversalModel(sharedModel, individualModels, numberElementsPerIndividualModel.data(),
						UniversalOptimization::ValueCallback::create(data, &CameraFovData::value),
						UniversalOptimization::ErrorCallback::create(data, &CameraFovData::error),
						UniversalOptimization::SharedModelIsValidCallback(),
						UniversalOptimization::SharedModelTransformationCallback::create(data, &CameraFovData::transformSharedModel),
						UniversalOptimization::IndividualModelTransformationCallback::create(data, &CameraFovData::transformIndividualModel),
						UniversalOptimization::ModelAcceptedCallback::create(data, &CameraFovData::acceptModel),
						optimizedSharedModel, optimizedIndividualModels, 50u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), nullptr, &finalError))
		{
			if (finalError < idealError)
			{
				idealError = finalError;

				optimizedCamera = PinholeCamera(width, height, optimizedSharedModel[0], optimizedSharedModel[0], Scalar(width) * Scalar(0.5), Scalar(height) * Scalar(0.5));

				optimizedOrientations.clear();
				optimizedOrientations.reserve(orientations.size());

				for (UniversalOptimization::IndividualModels::const_iterator i = optimizedIndividualModels.begin(); i != optimizedIndividualModels.end(); ++i)
				{
					const Pose pose(0, 0, 0, (*i)[0], (*i)[1], (*i)[2]);
					optimizedOrientations.emplace_back(pose.transformation().rotationMatrix());
				}
			}
		}
	}

	return idealError != Numeric::maxValue();
}

bool SphericalEnvironment::determineTransformationTable2x2(const unsigned int width, const unsigned int height, const Vectors2& points0, const Vectors2& points1, LookupCorner2<Vector2>& transformation0to1, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError)
{
	ocean_assert(width >= 1u && height >= 1u);

#ifdef OCEAN_DEBUG
	ocean_assert(points0.size() == points1.size());
	for (size_t n = 0; n < points0.size(); ++n)
	{
		ocean_assert(points0[n].x() >= 0 && points0[n].x() < Scalar(width) && points0[n].y() >= 0 && points1[n].y() < Scalar(height));
	}
#endif

	typedef Geometry::NonLinearUniversalOptimizationDense<18, 2, 18> UniversalOptimization;

	TransformationTableData2x2 data(width, height, points0, points1);

	UniversalOptimization::Model model;
	UniversalOptimization::Model optimizedModel;

	// we set the model to zero-offsets
	memset(model.data(), 0, sizeof(Scalar) * 18);

	if (!UniversalOptimization::optimizeUniversalModel(model, points0.size(), UniversalOptimization::ValueCallback::create(data, &TransformationTableData2x2::value), UniversalOptimization::ErrorCallback::create(data, &TransformationTableData2x2::error), UniversalOptimization::ModelTransformationCallback::create(data, &TransformationTableData2x2::transformModel), UniversalOptimization::ModelAdjustmentCallback(), optimizedModel, iterations, estimator, lambda, lambdaFactor, initialError, finalError))
	{
		return false;
	}

	transformation0to1 = LookupCorner2<Vector2>(width, height, 2u, 2u, (Vector2*)optimizedModel.data());
	return true;
}

template <unsigned int tSize>
SphericalEnvironment::Buffer SphericalEnvironment::interpolateSquarePatches(const Frame& frame, const Vectors2& positions, Worker* worker)
{
	ocean_assert(frame.isValid() && positions.size() > 1);

	switch (frame.channels())
	{
		case 1u:
		{
			Buffer result(positions.size() * 1u * tSize * tSize);
			interpolateSquarePatches8BitPerChannel<1u, tSize>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), positions, result.data(), worker);
			return result;
		}

		case 2u:
		{
			Buffer result(positions.size() * 2u * tSize * tSize);
			interpolateSquarePatches8BitPerChannel<2u, tSize>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), positions, result.data(), worker);
			return result;
		}

		case 3u:
		{
			Buffer result(positions.size() * 3u * tSize * tSize);
			interpolateSquarePatches8BitPerChannel<3u, tSize>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), positions, result.data(), worker);
			return result;
		}

		case 4u:
		{
			Buffer result(positions.size() * 4u * tSize * tSize);
			interpolateSquarePatches8BitPerChannel<4u, tSize>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), positions, result.data(), worker);
			return result;
		}
	}

	ocean_assert(false && "Invalid pixel format!");
	return Buffer();
}

template <unsigned int tChannels, unsigned int tSize>
void SphericalEnvironment::interpolateSquarePatches8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const Vectors2& positions, uint8_t* result, Worker* worker)
{
	ocean_assert(frame != nullptr);
	ocean_assert(width >= tSize && height >= tSize);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(SphericalEnvironment::interpolateSquarePatches8BitPerChannelSubset<tChannels, tSize>, frame, width, height, framePaddingElements, positions.data(), result, 0u, 0u), 0u, (unsigned int)(positions.size()), 7u, 8u, 20u);
	}
	else
	{
		interpolateSquarePatches8BitPerChannelSubset<tChannels, tSize>(frame, width, height, framePaddingElements, positions.data(), result, 0u, (unsigned int)(positions.size()));
	}
}

template <unsigned int tChannels, unsigned int tSize>
void SphericalEnvironment::interpolateSquarePatches8BitPerChannelSubset(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const Vector2* positions, uint8_t* result, const unsigned int firstPosition, const unsigned int numberPositions)
{
	ocean_assert(frame && positions && result);
	ocean_assert_and_suppress_unused(width >= tSize && height >= tSize, height);

	uint8_t* buffer = result + firstPosition * tChannels * tSize * tSize;

	for (unsigned int n = firstPosition; n < firstPosition + numberPositions; ++n)
	{
		const Vector2& position = positions[n];
		ocean_assert(position.x() >= Scalar(tSize / 2) && position.y() >= Scalar(tSize / 2u) && position.x() < Scalar(width - tSize / 2u - 1u) && position.y() < Scalar(height - tSize / 2u - 1u));

		CV::Advanced::AdvancedFrameInterpolatorBilinear::interpolateSquarePatch8BitPerChannel<tChannels, tSize>(frame, width, framePaddingElements, buffer, position);

		buffer += tChannels * tSize * tSize;
	}
}

template < unsigned int tSize>
IndexPairs32 SphericalEnvironment::findBijectiveCorrespondences(const PinholeCamera& pinholeCamera0, const PinholeCamera& pinholeCamera1, const FrameType::PixelFormat pixelFormat, const SquareMatrix3& orientation0, const SquareMatrix3& orientation1, const Buffer& buffer0, const Buffer& buffer1, const Vectors2& points0, const Vectors2& points1, const Geometry::SpatialDistribution::DistributionArray& distribution0, const Geometry::SpatialDistribution::DistributionArray& distribution1, Worker* worker)
{
	ocean_assert(pinholeCamera0.isValid() && pinholeCamera1.isValid());
	ocean_assert(!orientation0.isSingular() && !orientation1.isSingular());

	switch (FrameType::channels(pixelFormat))
	{
		case 1u:
			return findBijectiveCorrespondences8BitPerChannel<1u, tSize>(pinholeCamera0, pinholeCamera1, orientation0, orientation1, buffer0.data(), buffer1.data(), points0, points1, distribution0, distribution1, worker);

		case 2u:
			return findBijectiveCorrespondences8BitPerChannel<2u, tSize>(pinholeCamera0, pinholeCamera1, orientation0, orientation1, buffer0.data(), buffer1.data(), points0, points1, distribution0, distribution1, worker);

		case 3u:
			return findBijectiveCorrespondences8BitPerChannel<3u, tSize>(pinholeCamera0, pinholeCamera1, orientation0, orientation1, buffer0.data(), buffer1.data(), points0, points1, distribution0, distribution1, worker);

		case 4u:
			return findBijectiveCorrespondences8BitPerChannel<4u, tSize>(pinholeCamera0, pinholeCamera1, orientation0, orientation1, buffer0.data(), buffer1.data(), points0, points1, distribution0, distribution1, worker);
	}

	ocean_assert(false && "Invalid pixel format!");
	return IndexPairs32();
}

template <unsigned int tChannels, unsigned int tSize>
IndexPairs32 SphericalEnvironment::findBijectiveCorrespondences8BitPerChannel(const PinholeCamera& pinholeCamera0, const PinholeCamera& pinholeCamera1, const SquareMatrix3& orientation0, const SquareMatrix3& orientation1, const uint8_t* datas0, const uint8_t* datas1, const Vectors2& points0, const Vectors2& points1, const Geometry::SpatialDistribution::DistributionArray& distribution0, const Geometry::SpatialDistribution::DistributionArray& distribution1, Worker* worker)
{
	ocean_assert(pinholeCamera0 && pinholeCamera1);
	ocean_assert(!orientation0.isSingular() && !orientation1.isSingular());
	ocean_assert(datas0 && datas1);

	ocean_assert(Geometry::SpatialDistribution::distributeToArray(points0.data(), points0.size(), Scalar(0), Scalar(0), Scalar(pinholeCamera0.width()), Scalar(pinholeCamera0.height()), pinholeCamera0.width() / 40u, pinholeCamera0.height() / 40u) == distribution0);
	ocean_assert(Geometry::SpatialDistribution::distributeToArray(points1.data(), points1.size(), Scalar(0), Scalar(0), Scalar(pinholeCamera1.width()), Scalar(pinholeCamera1.height()), pinholeCamera1.width() / 40u, pinholeCamera1.height() / 40u) == distribution1);

	// determine bidirectional correspondences
	const IndexPairs32 correspondences(findBidirectionalCorrespondences8BitPerChannel<tChannels, tSize>(pinholeCamera0, pinholeCamera1, orientation0, orientation1, datas0, datas1, points0, points1, distribution0, distribution1, worker));

#ifdef OCEAN_DEBUG
	// now the correspondences should also be bijective
	IndexSet32 indexSet0, indexSet1;
	for (IndexPairs32::const_iterator i = correspondences.begin(); i != correspondences.end(); ++i)
	{
		ocean_assert(indexSet0.find(i->first) == indexSet0.end());
		ocean_assert(indexSet1.find(i->second) == indexSet1.end());

		indexSet0.insert(i->first);
		indexSet1.insert(i->second);
	}
#endif

	return correspondences;
}

template <unsigned int tChannels, unsigned int tSize>
IndexPairs32 SphericalEnvironment::findBidirectionalCorrespondences8BitPerChannel(const PinholeCamera& pinholeCamera0, const PinholeCamera& pinholeCamera1, const SquareMatrix3& orientation0, const SquareMatrix3& orientation1, const uint8_t* datas0, const uint8_t* datas1, const Vectors2& points0, const Vectors2& points1, const Geometry::SpatialDistribution::DistributionArray& distribution0, const Geometry::SpatialDistribution::DistributionArray& distribution1, Worker* worker)
{
	ocean_assert(datas0 && datas1);

	IndexPairs32 results;

	if (worker)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::createStatic(findBidirectionalCorrespondences8BitPerChannelSubset<tChannels, tSize>, &pinholeCamera0, &pinholeCamera1, &orientation0, &orientation1, datas0, datas1, &points0, &points1, &distribution0, &distribution1, &lock, &results, 0u, 0u), 0u, (unsigned int)points0.size(), 12u, 13u);
	}
	else
		findBidirectionalCorrespondences8BitPerChannelSubset<tChannels, tSize>(&pinholeCamera0, &pinholeCamera1, &orientation0, &orientation1, datas0, datas1, &points0, &points1, &distribution0, &distribution1, nullptr, &results, 0u, (unsigned int)points0.size());

	return results;
}

template <unsigned int tChannels, unsigned int tSize>
void SphericalEnvironment::findBidirectionalCorrespondences8BitPerChannelSubset(const PinholeCamera* pinholeCamera0, const PinholeCamera* pinholeCamera1, const SquareMatrix3* orientation0, const SquareMatrix3* orientation1, const uint8_t* datas0, const uint8_t* datas1, const Vectors2* points0, const Vectors2* points1, const Geometry::SpatialDistribution::DistributionArray* distribution0, const Geometry::SpatialDistribution::DistributionArray* distribution1, Lock* lock, IndexPairs32* results, const unsigned int firstPoint, const unsigned int numberPoints)
{
	ocean_assert(datas0 && datas1);
	ocean_assert(points0 && points1);

	ocean_assert(firstPoint + numberPoints <= (unsigned int)points0->size());

	IndexPairs32 localResults;
	Indices32 candidates0, candidates1;

	for (unsigned int n = firstPoint; n < firstPoint + numberPoints; ++n)
	{
		const Vector2& point0 = (*points0)[n];
		const Vector2 expectedPoint1(cameraPixel2cameraPixel(*pinholeCamera0, *orientation0, point0, *pinholeCamera1, *orientation1));

		candidates1 = Geometry::SpatialDistribution::determineNeighbors(expectedPoint1, points1->data(), points1->size(), 40, *distribution1);

		if (candidates1.size() >= 1)
		{
			const unsigned int indexPoint1 = findCorrespondingPoint8BitPerChannel<tChannels, tSize>(datas0 + n * tChannels * tSize * tSize, datas1, candidates1);

			if (indexPoint1 != (unsigned int)(-1))
			{
				ocean_assert(indexPoint1 < points1->size());
				const Vector2& point1 = (*points1)[indexPoint1];

				const Vector2 expectedPoint0(cameraPixel2cameraPixel(*pinholeCamera1, *orientation1, point1, *pinholeCamera0, *orientation0));

				candidates0 = Geometry::SpatialDistribution::determineNeighbors(expectedPoint0, points0->data(), points0->size(), 40, *distribution0);

				if (candidates0.size() >= 1)
				{
					const unsigned int indexPoint0 = findCorrespondingPoint8BitPerChannel<tChannels, tSize>(datas1 + indexPoint1 * tChannels * tSize * tSize, datas0, candidates0);

					if (indexPoint0 == n)
					{
						ocean_assert(indexPoint1 < points1->size());
						localResults.emplace_back(n, indexPoint1);
					}
				}
			}
		}
	}

	const OptionalScopedLock scopedLock(lock);

	results->insert(results->end(), localResults.begin(), localResults.end());
}

template <unsigned int tChannels, unsigned int tSize>
unsigned int SphericalEnvironment::findCorrespondingPoint8BitPerChannel(const uint8_t* data0, const uint8_t* datas1, const Indices32& indices)
{
	ocean_assert(data0 && datas1);
	ocean_assert(indices.size() >= 1);

	unsigned int ssdBest = tChannels * tSize * tSize * 7u * 7u;
	unsigned int indexBest = (unsigned int)(-1);

	for (Indices32::const_iterator i = indices.begin(); i != indices.end(); ++i)
	{
		const unsigned int ssd = CV::ZeroMeanSumSquareDifferences::buffer8BitPerChannel<tChannels, tSize>(data0, datas1 + *i * tChannels * tSize * tSize);

		if (ssd < ssdBest)
		{
			ssdBest = ssd;
			indexBest = *i;
		}
	}

	return indexBest;
}

}

}
