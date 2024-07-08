/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/offline/PlanarRectangleTracker.h"
#include "ocean/tracking/offline/Frame2FrameTracker.h"

#include "ocean/base/Subset.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/MaskAnalyzer.h"

#include "ocean/cv/advanced/AdvancedMotion.h"

#include "ocean/geometry/Error.h"
#include "ocean/geometry/Homography.h"
#include "ocean/geometry/NonLinearOptimizationPose.h"
#include "ocean/geometry/NonLinearUniversalOptimizationSparse.h"
#include "ocean/geometry/Utilities.h"
#include "ocean/geometry/VanishingProjection.h"

#include "ocean/math/Pose.h"
#include "ocean/math/SuccessionSubset.h"

namespace Ocean
{

namespace Tracking
{

namespace Offline
{

/**
 * This class implements a data object allowing to optimize the camera profile and several camera poses concurrently.
 * All parameters of the camera profile will be updated by application of this data object.<br>
 * @see Geometry::NonLinearUniversalOptimizationSparse.
 */
class PlanarRectangleTracker::CameraPosesData
{
	public:

		/**
		 * Creates a new optimization data object.
		 * @param cameraWidth The width of the camera frame in pixel, with range [1, infinity)
		 * @param cameraHeight The height of the camera frame in pixel, with range [1, infinity)
		 * @param initialPose The initial pose of the camera of the first/initial tracking frame
		 * @param rectangleCorners The four corners of the rectangle that define the projected planar 3D rectangle
		 * @param imagePointsPairs A set of image point correspondences, each set of correspondences define one camera frame
		 */
		CameraPosesData(const unsigned int cameraWidth, const unsigned int cameraHeight, const HomogenousMatrix4& initialPose, const Vector2* rectangleCorners, const Geometry::NonLinearOptimizationPlane::ImagePointsPairs& imagePointsPairs) :
			cameraWidth_(cameraWidth),
			cameraHeight_(cameraHeight),
			initialPose_(initialPose),
			rectangleCorners_(rectangleCorners),
			imagePointsPairs_(imagePointsPairs)
		{
			// nothing to do here
		}

		/**
		 * Value calculation function.
		 * @param externalSharedModel External shared model that is applied
		 * @param externalIndividualModel External individual model that is applied
		 * @param individualModelIndex Index of the individual model data set
		 * @param elementIndex Index of the element within the individual model data set
		 * @param result Resulting value
		 */
		void value(const StaticBuffer<Scalar, 12>& externalSharedModel, const StaticBuffer<Scalar, 16>& externalIndividualModel, const size_t individualModelIndex, const size_t elementIndex, StaticBuffer<Scalar, 2>& result)
		{
			const Vector2 projectedPoint(projectObjectPoint(externalSharedModel, externalIndividualModel, individualModelIndex, elementIndex));

			result[0] = projectedPoint[0];
			result[1] = projectedPoint[1];
		}

		/**
		 * Error calculation function.
		 * @param externalSharedModel External shared model that is applied
		 * @param externalIndividualModel External individual model that is applied
		 * @param individualModelIndex Index of the individual model data set
		 * @param elementIndex Index of the element within the individual model data set
		 * @param result Resulting value
		 * @return True, if the measurement is valid
		 */
		bool error(const StaticBuffer<Scalar, 12>& externalSharedModel, const StaticBuffer<Scalar, 16>& externalIndividualModel, const size_t individualModelIndex, const size_t elementIndex, StaticBuffer<Scalar, 2>& result)
		{
			const Vector2 projectedPoint(projectObjectPoint(externalSharedModel, externalIndividualModel, individualModelIndex, elementIndex));
			const ImagePoint& measurementImagePoint = imagePointsPairs_[individualModelIndex].second[elementIndex];
			const Vector2 error(projectedPoint - measurementImagePoint);

			result[0] = error[0];
			result[1] = error[1];
			return true;
		}

		/**
		 * Transforms the shared internal model to a shared external model.
		 * @param internalModel Internal model that will be transformed
		 * @param externalModel Resulting external model
		 */
		void transformSharedModel(StaticBuffer<Scalar, 8>& internalModel, StaticBuffer<Scalar, 12>& externalModel)
		{
			const PinholeCamera pinholeCamera(cameraWidth_, cameraHeight_, internalModel.data());
			ocean_assert(pinholeCamera.isValid());

			Plane3 plane;
			const bool result = PlanarRectangleTracker::determinePlane(pinholeCamera, rectangleCorners_, plane);
			ocean_assert_and_suppress_unused(result, result);

			memcpy(externalModel.data(), internalModel.data(), sizeof(Scalar) * 8);

			ocean_assert(sizeof(plane) == sizeof(Scalar) * 4);
			memcpy(externalModel.data() + 8, &plane, sizeof(plane));
		}

		/**
		 * Transforms the internal individual model to an external individual model.
		 * @param internalModel Internal model that will be transformed
		 * @param externalModel Resulting external model
		 */
		void transformIndividualModel(StaticBuffer<Scalar, 6>& internalModel, StaticBuffer<Scalar, 16>& externalModel)
		{
			// internal model: 6 pose parameters
			// external model: 16 pose parameters

			const HomogenousMatrix4 transformationIF((*(Pose*)internalModel.data()).transformation());
			memcpy(externalModel.data(), transformationIF.data(), sizeof(transformationIF));
		}

	protected:

		/**
		 * Projects a specific object point by application of the shared model and the individual mode.
		 * @param externalSharedModel The external shared model that is applied
		 * @param externalIndividualModel The external individual mode that is applied
		 * @param individualModelIndex The index of the individual model
		 * @param elementIndex The index of the element wihtin the individual model
		 * @return The resulting projected object point
		 */
		inline Vector2 projectObjectPoint(const StaticBuffer<Scalar, 12>& externalSharedModel, const StaticBuffer<Scalar, 16>& externalIndividualModel, const size_t individualModelIndex, const size_t elementIndex)
		{
			const PinholeCamera pinholeCamera(cameraWidth_, cameraHeight_, externalSharedModel.data());
			const Plane3& plane = *((Plane3*)(externalSharedModel.data() + 8));
			const HomogenousMatrix4& poseIF = *((HomogenousMatrix4*)(externalIndividualModel.data()));
			ocean_assert(pinholeCamera.isValid() && plane.isValid() && poseIF.isValid());

			const ImagePoint& initialImagePoint = imagePointsPairs_[individualModelIndex].first[elementIndex];

			const ObjectPoint objectPoint(Geometry::Utilities::backProjectImagePoint(pinholeCamera, initialPose_, plane, initialImagePoint, pinholeCamera.hasDistortionParameters()));

			return pinholeCamera.projectToImageIF<true>(poseIF, objectPoint, pinholeCamera.hasDistortionParameters());
		}

	protected:

		/// The width of the current camera profile in pixel.
		const unsigned int cameraWidth_;

		/// The height of the current camera profile in pixel.
		const unsigned int cameraHeight_;

		/// The pose of the initial frame (from which the tracking starts).
		const HomogenousMatrix4 initialPose_;

		/// The projected 3D rectangle that defines a plane.
		const Vector2* rectangleCorners_;

		/// The set of image point correspondences.
		const Geometry::NonLinearOptimizationPlane::ImagePointsPairs& imagePointsPairs_;
};


class PlanarRectangleTracker::DistortionCameraPosesData
{
	public:

		DistortionCameraPosesData(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& initialPose, const Vector2* rectangleCorners, const Geometry::NonLinearOptimizationPlane::ImagePointsPairs& imagePointsPairs) :
			camera_(pinholeCamera),
			initialPose_(initialPose),
			rectangleCorners_(rectangleCorners),
			imagePointsPairs_(imagePointsPairs)
		{
			// nothing to do here
		}

		void value(const StaticBuffer<Scalar, 8>& externalSharedModel, const StaticBuffer<Scalar, 16>& externalIndividualModel, const size_t individualModelIndex, const size_t elementIndex, StaticBuffer<Scalar, 2>& result)
		{
			const Vector2 projectedPoint(projectObjectPoint(externalSharedModel, externalIndividualModel, individualModelIndex, elementIndex));

			result[0] = projectedPoint[0];
			result[1] = projectedPoint[1];
		}

		/**
		 * Error calculation function.
		 * @param externalSharedModel External shared model that is applied
		 * @param externalIndividualModel External individual model that is applied
		 * @param individualModelIndex Index of the individual model data set
		 * @param elementIndex Index of the element within the individual model data set
		 * @param result Resulting value
		 * @return True, if the measurement is valid
		 */
		bool error(const StaticBuffer<Scalar, 8>& externalSharedModel, const StaticBuffer<Scalar, 16>& externalIndividualModel, const size_t individualModelIndex, const size_t elementIndex, StaticBuffer<Scalar, 2>& result)
		{
			const Vector2 projectedPoint(projectObjectPoint(externalSharedModel, externalIndividualModel, individualModelIndex, elementIndex));
			const ImagePoint& measurementImagePoint = imagePointsPairs_[individualModelIndex].second[elementIndex];
			const Vector2 error(projectedPoint - measurementImagePoint);

			result[0] = error[0];
			result[1] = error[1];
			return true;
		}

		/**
		 * @param internalModel Internal model that will be transformed
		 * @param externalModel Resulting external model
		 */
		void transformSharedModel(StaticBuffer<Scalar, 4>& internalModel, StaticBuffer<Scalar, 8>& externalModel)
		{
			PinholeCamera pinholeCamera(camera_);
			pinholeCamera.setRadialDistortion(PinholeCamera::DistortionPair(internalModel[0], internalModel[1]));
			pinholeCamera.setTangentialDistortion(PinholeCamera::DistortionPair(internalModel[2], internalModel[3]));

			ocean_assert(pinholeCamera.isValid());

			Plane3 plane;
			const bool result = PlanarRectangleTracker::determinePlane(pinholeCamera, rectangleCorners_, plane);
			ocean_assert_and_suppress_unused(result, result);

			memcpy(externalModel.data(), internalModel.data(), sizeof(Scalar) * 4);

			ocean_assert(sizeof(plane) == sizeof(Scalar) * 4);
			memcpy(externalModel.data() + 4, &plane, sizeof(plane));
		}

		/**
		 * Transforms the internal individual model to an external individual model.
		 * @param internalModel Internal model that will be transformed
		 * @param externalModel Resulting external model
		 */
		void transformIndividualModel(StaticBuffer<Scalar, 6>& internalModel, StaticBuffer<Scalar, 16>& externalModel)
		{
			// internal model: 6 pose parameters
			// external model: 16 pose parameters

			const HomogenousMatrix4 transformationIF((*(Pose*)internalModel.data()).transformation());
			memcpy(externalModel.data(), transformationIF.data(), sizeof(transformationIF));
		}

	protected:

		inline Vector2 projectObjectPoint(const StaticBuffer<Scalar, 8>& externalSharedModel, const StaticBuffer<Scalar, 16>& externalIndividualModel, const size_t individualModelIndex, const size_t elementIndex)
		{
			PinholeCamera pinholeCamera(camera_);
			pinholeCamera.setRadialDistortion(PinholeCamera::DistortionPair(externalSharedModel[0], externalSharedModel[1]));
			pinholeCamera.setTangentialDistortion(PinholeCamera::DistortionPair(externalSharedModel[2], externalSharedModel[3]));

			const Plane3& plane = *((Plane3*)(externalSharedModel.data() + 4));
			const HomogenousMatrix4& poseIF = *((HomogenousMatrix4*)(externalIndividualModel.data()));
			ocean_assert(pinholeCamera.isValid() && plane.isValid() && poseIF.isValid());

			const ImagePoint& initialImagePoint = imagePointsPairs_[individualModelIndex].first[elementIndex];

			const ObjectPoint objectPoint(Geometry::Utilities::backProjectImagePoint(pinholeCamera, initialPose_, plane, initialImagePoint, pinholeCamera.hasDistortionParameters()));

			return pinholeCamera.projectToImageIF<true>(poseIF, objectPoint, pinholeCamera.hasDistortionParameters());
		}

	protected:

		const PinholeCamera& camera_;

		const HomogenousMatrix4 initialPose_;

		const Vector2* rectangleCorners_;

		const Geometry::NonLinearOptimizationPlane::ImagePointsPairs& imagePointsPairs_;
};

PlanarRectangleTracker::PlaneTrackerComponent::PlaneTrackerComponent(PlanarRectangleTracker& parent, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& initialPose, const Plane3& plane, OfflinePoses& resultingPoses, PinholeCamera* resultingOptimizedCamera) :
	FramePyramidTrackerComponent(parent),
	parent_(parent),
	camera_(pinholeCamera),
	initialPose_(initialPose),
	plane_(plane),
	resultingPoses_(resultingPoses),
	initialRectifiedPose_(false),
	resultingOptimizedCamera_(resultingOptimizedCamera)
{
	ocean_assert(camera_.isValid() && initialPose_.isValid() && plane_.isValid());
}

bool PlanarRectangleTracker::PlaneTrackerComponent::onStart(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex)
{
	if (!FramePyramidTrackerComponent::onStart(lowerFrameIndex, initialFrameIndex, upperFrameIndex))
	{
		return false;
	}

	const size_t frames = upperFrameIndex - lowerFrameIndex + 1;

	poses_ = OfflinePoses(lowerFrameIndex, frames, OfflinePose());
	imagePointsPairs_ = ImagePointsPairs(lowerFrameIndex, frames, Geometry::NonLinearOptimizationPlane::ImagePointsPair());

	return true;
}

PlanarRectangleTracker::FramePyramidTrackerComponent::IterationResult PlanarRectangleTracker::PlaneTrackerComponent::onFrame(const unsigned int previousIndex, const unsigned int currentIndex, const unsigned int iteration, const unsigned int maximalIterations)
{
	const WorkerPool::ScopedWorker scopedWorker(WorkerPool::get().scopedWorker());

	if (!initialFramePyramid_)
	{
		ocean_assert(!previousFramePyramid_);

		ocean_assert(previousIndex == (unsigned int)(-1));

		ocean_assert(poses_.isValidIndex(currentIndex));
		poses_[currentIndex] = OfflinePose(currentIndex, initialPose_);

		initialFramePyramid_ = CV::FramePyramid(currentFramePyramid_, true /*copyData*/);

		ocean_assert(!initialRectifiedFramePyramid_.isValid() && !initialRectifiedCamera_.isValid() && initialRectifiedPose_.isNull());

		// determined the camera and pose that observes the selected plane area from the top, centered at the rectangle's center
		if (!lookAtTransformation(camera_, initialPose_, parent_.initialRectangleCorners_, plane_, Scalar(0.1), initialRectifiedCamera_, initialRectifiedPose_))
		{
			return IR_FAILED;
		}

		const Frame& currentFrame = currentFramePyramid_.finestLayer();
		Frame initialRectifiedFrame(FrameType(currentFrame, initialRectifiedCamera_.width(), initialRectifiedCamera_.height()));

		// the homography transforming points in the rectified frame into points in the current camera frame
		const SquareMatrix3 homography(Geometry::Homography::homographyMatrix(initialRectifiedPose_, initialPose_, initialRectifiedCamera_, camera_, plane_));

		// create the rectified frame of the tracking rectangle
		ocean_assert(!initialRectifiedCamera_.hasDistortionParameters());
		if (!CV::FrameInterpolatorBilinear::Comfort::homographyWithCamera(camera_, initialRectifiedCamera_, currentFrame, initialRectifiedFrame, homography, camera_.hasDistortionParameters(), nullptr, scopedWorker()))
		{
			return IR_FAILED;
		}

		// create a frame pyramid of the tracking rectangle
		if (!initialRectifiedFramePyramid_.replace8BitPerChannel11(initialRectifiedFrame, 5u, true /*copyFirstLayer*/, scopedWorker()))
		{
			return IR_FAILED;
		}
	}
	else if (previousFramePyramid_)
	{
		ocean_assert(poses_.isValidIndex(previousIndex));

		const HomogenousMatrix4& previousPose = poses_[previousIndex].transformation();
		ocean_assert(previousPose.isValid());

		ocean_assert(camera_.isValid());
		const SquareMatrix3 previousHomography(Geometry::Homography::homographyMatrix(initialPose_, previousPose, camera_, camera_, plane_));

		// adjust the sub-region from the initial frame so that it matches with the previous frame
		const CV::SubRegion previousSubRegion(parent_.initialRectangleSubRegion_ * previousHomography);

		// determine the pose between the previous and the current frame
		HomogenousMatrix4 currentPose;
		if (frame2framePose(CV::FramePyramid(previousFramePyramid_, 0u, 4u, false), CV::FramePyramid(currentFramePyramid_, 0u, 4u, false), previousSubRegion, previousPose, currentPose, Scalar(0.9 * 0.9), 20u, 20u, 30u, scopedWorker(), 3u) < 25)
		{
			if (frame2framePose(CV::FramePyramid(previousFramePyramid_, 0u, 4u, false), CV::FramePyramid(currentFramePyramid_, 0u, 4u, false), previousSubRegion, previousPose, currentPose, Scalar(1.1 * 1.1), 30u, 30u, 10u, scopedWorker(), 3u) < 20)
			{
				if (frame2framePose(previousFramePyramid_, currentFramePyramid_, previousSubRegion, previousPose, currentPose, Scalar(1.9 * 1.9), 0u, 0u, 5u, scopedWorker(), 5u) < 25)
				{
					// the tracking region could not been tracked from the previous frame to the current frame
					// thus, we expect the tracking region to be invisible from now on and we can finish here

					ocean_assert(!poses_[currentIndex].isValid());

					return IR_FINISHED;
				}
			}
		}

		// check whether the two poses are almost identical (we expect a pose-orientation-offset of 8 degrees at too much)
		if (!Geometry::Error::posesAlmostEqual(previousPose, currentPose, Numeric::deg2rad(8)))
		{
			return IR_FINISHED;
		}

		ImagePoints initialImagePoints, currentImagePoints;
		const unsigned int poseOptimizationIterations = 2u;
		bool imagePointsReliable = true;

		for (unsigned int n = 0u; n < poseOptimizationIterations; ++n)
		{
			HomogenousMatrix4 optimizedPose;
			if (!optimizePose(currentFramePyramid_.finestLayer(), currentPose, optimizedPose, scopedWorker(), n + 1u == poseOptimizationIterations ? &initialImagePoints : nullptr, n + 1u == poseOptimizationIterations ? &currentImagePoints : nullptr, &imagePointsReliable))
			{
				break;
			}

			currentPose = optimizedPose;
		}

		ocean_assert(!poses_[currentIndex].isValid());

		if (initialImagePoints.size() < 25)
		{
			// the tracking region could not been tracked from the previous frame to the current frame
			// thus, we expect the tracking region to be invisible from now on and we can finish here

			ocean_assert(!poses_[currentIndex].isValid());

			return IR_FINISHED;
		}

		// if the point correspondences are not reliable, then we do not use them to optimized the camera profile
		if (!imagePointsReliable)
		{
			initialImagePoints.clear();
			currentImagePoints.clear();
		}

		poses_[currentIndex] = OfflinePose(currentIndex, currentPose);
		poses_[currentIndex].setQuality(Scalar(initialImagePoints.size()));
		imagePointsPairs_[currentIndex] = std::make_pair(std::move(initialImagePoints), std::move(currentImagePoints));
	}

	parent_.eventCallbacks_(TrackerPoseStateEvent(parent_.id(), poses_[currentIndex]));
	parent_.eventCallbacks_(TrackingProgressEvent(parent_.id(), (iteration + 1u) * 100u / maximalIterations));

	return IR_SUCCEEDED;
}

bool PlanarRectangleTracker::PlaneTrackerComponent::onStop(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex)
{
	if (!FramePyramidTrackerComponent::onStop(lowerFrameIndex, initialFrameIndex, upperFrameIndex))
	{
		return false;
	}

	resultingPoses_ = poses_;

	if (resultingOptimizedCamera_ && !parent_.shouldThreadStop() && !optimizeCamera(*resultingOptimizedCamera_, 20u))
	{
		// the optimization of the camera profile failed, however, than we use the normal profile
		*resultingOptimizedCamera_ = camera_;
	}

	return true;
}

bool PlanarRectangleTracker::PlaneTrackerComponent::optimizePose(const Frame& currentFrame, const HomogenousMatrix4& currentPose, HomogenousMatrix4& optimizedPose, Worker* worker, ImagePoints* validInitialImagePoints, ImagePoints* validCurrentImagePoints, bool* reliableImagePoints)
{
	ocean_assert(currentFrame && currentPose.isValid());

	ocean_assert(initialRectifiedPose_.isValid());
	ocean_assert(initialRectifiedCamera_.isValid());

	// the homography that transforms the rectified frame of the current frame into the current frame
	const SquareMatrix3 currentHomography(Geometry::Homography::homographyMatrix(initialRectifiedPose_, currentPose, initialRectifiedCamera_, camera_, plane_));

	ocean_assert(!initialRectifiedCamera_.hasDistortionParameters());

	// this frame (as well as componentInitialRectifiedFramePyramid) will receive the undistorted rectified frame (as the componentInitialRectifiedCamera object holds no distortion parameters)
	if (!intermediateRectifiedFrame_.set(FrameType(currentFrame, initialRectifiedCamera_.width(), initialRectifiedCamera_.height()), true, true))
	{
		return false;
	}

	intermediateRectifiedFrame_.setValue(0x00);
	Frame rectifiedMask(FrameType(intermediateRectifiedFrame_, FrameType::FORMAT_Y8));

	ocean_assert(!initialRectifiedCamera_.hasDistortionParameters());
	if (!CV::FrameInterpolatorBilinear::Comfort::homographyWithCameraMask(AnyCameraPinhole(camera_), AnyCameraPinhole(initialRectifiedCamera_), currentFrame, intermediateRectifiedFrame_, rectifiedMask, currentHomography, worker, 0xFF))
	{
		return false;
	}

	// determine the bounding box of all visible pixels of the tracking pattern in the current frame
	const CV::PixelBoundingBox maskBoundingBox(CV::MaskAnalyzer::detectBoundingBox(rectifiedMask.constdata<uint8_t>(), rectifiedMask.width(), rectifiedMask.height(), 0x00u, rectifiedMask.paddingElements()));

	// we define a sub-region that shrinks the tracking area and avoids to use tracking points near the frame borders
	Box2 maskTrackingArea = maskBoundingBox.isValid() ? Box2(Scalar(maskBoundingBox.left()), Scalar(maskBoundingBox.top()), Scalar(maskBoundingBox.right()), Scalar(maskBoundingBox.bottom())) : Box2();

	// the tracking sub-region
	const CV::SubRegion subRegion(maskTrackingArea);

	if (!intermediateRectifiedFramePyramid_.replace8BitPerChannel11(intermediateRectifiedFrame_, 5u, true /*copyFirstLayer*/, worker))
	{
		return false;
	}

	Vectors2 rectifiedInitialPoints, rectifiedCurrentPoints;
	if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackArbitraryPointsBidirectionalSubPixelMirroredBorder<15u>(initialRectifiedFramePyramid_, intermediateRectifiedFramePyramid_, 2u, rectifiedInitialPoints, rectifiedCurrentPoints, Scalar(0.9 * 0.9), subRegion, 20u, 20u, 30u, worker, 3u) || rectifiedInitialPoints.size() <= 45)
	{
		rectifiedInitialPoints.clear();
		rectifiedCurrentPoints.clear();

		// as the first tracking call failed, the quality cannot be very good, thus we should not use the image points
		if (reliableImagePoints)
		{
			*reliableImagePoints = false;
		}

		if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackArbitraryPointsBidirectionalSubPixelMirroredBorder<15u>(initialRectifiedFramePyramid_, intermediateRectifiedFramePyramid_, 2u, rectifiedInitialPoints, rectifiedCurrentPoints, Scalar(1.1 * 1.1), subRegion, 30u, 30u, 10u, worker, 3u) || rectifiedInitialPoints.size() < 25)
		{
			rectifiedInitialPoints.clear();
			rectifiedCurrentPoints.clear();

			if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackArbitraryPointsBidirectionalSubPixelMirroredBorder<15u>(initialRectifiedFramePyramid_, intermediateRectifiedFramePyramid_, 2u, rectifiedInitialPoints, rectifiedCurrentPoints, Scalar(1.9 * 1.9), subRegion, 0u, 0u, 5u, worker, 3u) || rectifiedInitialPoints.size() < 25)
			{
				return false;
			}
		}
	}

	// determine 3D object points by back projection, we do not apply camera distortion parameters as the image points are determine with undistorted frames
	const Vectors3 intermediateObjectPoints(Geometry::Utilities::backProjectImagePoints(initialRectifiedCamera_, initialRectifiedPose_, plane_, rectifiedInitialPoints.data(), rectifiedInitialPoints.size(), false));

	// determine the 2D image points as would been seen in the current frame
	Vectors2 cameraCurrentPoints;
	cameraCurrentPoints.reserve(intermediateObjectPoints.size());

	Vectors3 objectPoints;
	objectPoints.reserve(intermediateObjectPoints.size());

	for (size_t n = 0; n < rectifiedCurrentPoints.size(); n++)
	{
		// we apply the homography and an distortion as the current frame is distorted
		const Vector2 currentPoint(camera_.distort<true>(currentHomography * rectifiedCurrentPoints[n]));

		// check whether the point is inside the camera frame, otherwise the point-pair is unused
		if (camera_.isInside(currentPoint))
		{
			cameraCurrentPoints.push_back(currentPoint);
			objectPoints.push_back(intermediateObjectPoints[n]);
		}
	}

	ocean_assert(objectPoints.size() == cameraCurrentPoints.size());

	// we optimize the camera pose, and we apply the camera distortion parameters, as the camera image points are distorted
	if (!Geometry::NonLinearOptimizationPose::optimizePose(camera_, currentPose, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(cameraCurrentPoints), camera_.hasDistortionParameters(), optimizedPose, 20u, Geometry::Estimator::ET_HUBER, Scalar(0.001), Scalar(5)))
	{
		return false;
	}

	if (validInitialImagePoints && validCurrentImagePoints)
	{
		// determine the valid subset of image points correspondences that has been used to optimized the pose

		// the homography that transforms the initial rectified frame into the initial frame
		const SquareMatrix3 initialHomography(Geometry::Homography::homographyMatrix(initialRectifiedPose_, initialPose_, initialRectifiedCamera_, camera_, plane_));

		Scalars sqrErrors(objectPoints.size());
		Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, false, true>(optimizedPose, camera_, ConstTemplateArrayAccessor<Vector3>(objectPoints), ConstTemplateArrayAccessor<Vector2>(cameraCurrentPoints), camera_.hasDistortionParameters(), Scalar(1), nullptr, sqrErrors.data());

		validInitialImagePoints->clear();
		validCurrentImagePoints->clear();

		for (size_t n = 0u; n < objectPoints.size(); ++n)
		{
			if (sqrErrors[n] <= Scalar(5 * 5)) // we accept a projection error of 5 pixels
			{
				validInitialImagePoints->push_back(camera_.distort<true>(initialHomography * rectifiedInitialPoints[n]));
				validCurrentImagePoints->push_back(cameraCurrentPoints[n]);
			}
		}
	}

	return true;
}

bool PlanarRectangleTracker::PlaneTrackerComponent::optimizeCamera(PinholeCamera& optimizedCamera, const unsigned int numberFrames)
{
	ocean_assert(numberFrames >= 10u);
	ocean_assert(camera_.isValid());

	Indices32 poseMapping;
	poseMapping.reserve(poses_.size());

	// find the threshold for feature point correspondences
	Indices32 pointCorrespondences;
	pointCorrespondences.reserve(poses_.size());

	for (OfflinePoses::Index i = poses_.firstIndex(); i < poses_.endIndex(); ++i)
	{
		if (poses_[i].isValid())
		{
			pointCorrespondences.push_back((unsigned int)imagePointsPairs_[i].first.size());
		}
	}

	const unsigned int qualityThreshold = max(10u, Median::median(pointCorrespondences.data(), pointCorrespondences.size()));

	// find a subset of all poses best representing the entire tracking sequence
	Poses poses;
	poses.reserve(poses_.size());

	for (OfflinePoses::Index i = poses_.firstIndex(); i < poses_.endIndex(); ++i)
	{
		if (poses_[i].isValid() && imagePointsPairs_[i].first.size() >= qualityThreshold)
		{
			// poses is not a shift vector so we have to adjust the index
			poses.push_back(Pose(poses_[i].transformation()));
			poseMapping.push_back((unsigned int)i);
		}
	}

	ocean_assert(sizeof(SuccessionSubset<Scalar, 6>::Object) == sizeof(Pose));
	SuccessionSubset<Scalar, 6> successionSubset((SuccessionSubset<Scalar, 6>::Object*)poses.data(), poses.size());

	HomogenousMatrices4 bestPosesIF;
	Geometry::NonLinearOptimizationPlane::ImagePointsPairs bestImagePointsPairs;

	bestPosesIF.reserve(numberFrames);
	bestImagePointsPairs.reserve(numberFrames);

	while (bestPosesIF.size() < numberFrames)
	{
		// find the best next pose
		const size_t indexZeroBased = successionSubset.incrementSubset();
		if (indexZeroBased == size_t(-1))
		{
			break;
		}

		// the subset index might not be identical with the frame index of the tracking sequence
		const unsigned int frameIndex = poseMapping[indexZeroBased];

		ocean_assert(poses_[frameIndex].isValid());

		bestPosesIF.push_back(PinholeCamera::standard2InvertedFlipped(poses_[frameIndex].transformation()));
		bestImagePointsPairs.push_back(imagePointsPairs_[frameIndex]);
	}

	ocean_assert(bestPosesIF.size() == bestImagePointsPairs.size());

	if (bestPosesIF.size() < 10)
	{
		return false;
	}

	typedef std::vector< StaticBuffer<Scalar, 6> > IndividualModels;

	IndividualModels individualModels(bestPosesIF.size());

	std::vector<size_t> numberElementsPerIndividualModel;
	numberElementsPerIndividualModel.reserve(bestPosesIF.size());

	// the individual models start with the second pose
	for (size_t n = 0; n < bestPosesIF.size(); ++n)
	{
		const Pose poseIF(bestPosesIF[n]);
		individualModels[n] = *((StaticBuffer<Scalar, 6>*)poseIF.data());

		ocean_assert(bestImagePointsPairs[n].first.size() == bestImagePointsPairs[n].second.size());
		numberElementsPerIndividualModel.push_back(bestImagePointsPairs[n].first.size());
	}

	// the individual optimized models will not be used after optimization
	IndividualModels optimizedIndividualModels;

	// first we try to optimized the entire camera model including, focal length, principal point and camera distortion
	{
		typedef Geometry::NonLinearUniversalOptimizationSparse::SharedModelIndividualModels<8, 6, 2, 12, 16> UniversalOptimization;

		UniversalOptimization::SharedModel sharedModel;
		camera_.copyElements(sharedModel.data());

		UniversalOptimization::SharedModel optimizedSharedModel;

		CameraPosesData data(camera_.width(), camera_.height(), initialPose_, parent_.initialRectangleCorners_, bestImagePointsPairs);

		if (!UniversalOptimization::optimizeUniversalModel(sharedModel, individualModels, numberElementsPerIndividualModel.data(),
						UniversalOptimization::ValueCallback::create(data, &CameraPosesData::value),
						UniversalOptimization::ErrorCallback::create(data, &CameraPosesData::error),
						UniversalOptimization::SharedModelIsValidCallback(),
						UniversalOptimization::SharedModelTransformationCallback::create(data, &CameraPosesData::transformSharedModel),
						UniversalOptimization::IndividualModelTransformationCallback::create(data, &CameraPosesData::transformIndividualModel),
						UniversalOptimization::ModelAcceptedCallback(),
						optimizedSharedModel, optimizedIndividualModels, 20u, Geometry::Estimator::ET_HUBER, Scalar(0.001), Scalar(5)))
		{
			return false;
		}

		const PinholeCamera cameraCandidate(camera_.width(), camera_.height(), optimizedSharedModel.data());

		if (isPlausibleCamera(cameraCandidate, CM_MEDIUM_QUALITY))
		{
			optimizedCamera = cameraCandidate;
			return true;
		}
	}

	// now we try to optimize the distortion parameters only
	{
		typedef Geometry::NonLinearUniversalOptimizationSparse::SharedModelIndividualModels<4, 6, 2, 8, 16> UniversalOptimization;

		UniversalOptimization::SharedModel sharedModel;
		sharedModel[0] = camera_.radialDistortion().first;
		sharedModel[1] = camera_.radialDistortion().second;
		sharedModel[2] = camera_.tangentialDistortion().first;
		sharedModel[3] = camera_.tangentialDistortion().second;

		UniversalOptimization::SharedModel optimizedSharedModel;

		DistortionCameraPosesData data(camera_, initialPose_, parent_.initialRectangleCorners_, bestImagePointsPairs);

		if (!UniversalOptimization::optimizeUniversalModel(sharedModel, individualModels, numberElementsPerIndividualModel.data(),
						UniversalOptimization::ValueCallback::create(data, &DistortionCameraPosesData::value),
						UniversalOptimization::ErrorCallback::create(data, &DistortionCameraPosesData::error),
						UniversalOptimization::SharedModelIsValidCallback(),
						UniversalOptimization::SharedModelTransformationCallback::create(data, &DistortionCameraPosesData::transformSharedModel),
						UniversalOptimization::IndividualModelTransformationCallback::create(data, &DistortionCameraPosesData::transformIndividualModel),
						UniversalOptimization::ModelAcceptedCallback(),
						optimizedSharedModel, optimizedIndividualModels, 20u, Geometry::Estimator::ET_HUBER, Scalar(0.001), Scalar(5)))
		{
			return false;
		}

		PinholeCamera cameraCandidate(camera_);
		cameraCandidate.setRadialDistortion(PinholeCamera::DistortionPair(optimizedSharedModel[0], optimizedSharedModel[1]));
		cameraCandidate.setTangentialDistortion(PinholeCamera::DistortionPair(optimizedSharedModel[2], optimizedSharedModel[3]));

		if (isPlausibleCamera(cameraCandidate, CM_MEDIUM_QUALITY))
		{
			optimizedCamera = cameraCandidate;
			return true;
		}
	}

	return false;
}

PlanarRectangleTracker::PlanarRectangleTracker() :
	initialPose_(Vector3(0, 0, 0))
{
	initialRectangleCorners_[0] = Vector2(0, 0);
	initialRectangleCorners_[1] = Vector2(0, 0);
	initialRectangleCorners_[2] = Vector2(0, 0);
	initialRectangleCorners_[3] = Vector2(0, 0);
}

PlanarRectangleTracker::~PlanarRectangleTracker()
{
	shouldStop_ = true;
	stopThread();
	stopThreadExplicitly();
}

bool PlanarRectangleTracker::setInitialRectangle(const Vector2 rectangleCorners[4])
{
	if (isThreadActive() || isThreadInvokedToStart())
	{
		return false;
	}

	for (unsigned int n = 0u; n < 4u; ++n)
	{
		initialRectangleCorners_[n] = rectangleCorners[n];
	}

	Triangles2 triangles(2);
	triangles[0] = Triangle2(initialRectangleCorners_[0], initialRectangleCorners_[1], initialRectangleCorners_[2]);
	triangles[1] = Triangle2(initialRectangleCorners_[0], initialRectangleCorners_[2], initialRectangleCorners_[3]);

	initialRectangleSubRegion_ = CV::SubRegion(triangles);

	return true;
}

bool PlanarRectangleTracker::applyFrameTracking(const FrameType& frameType)
{
	const unsigned int optimizationIterations = 3u;

	// check whether the initial camera object matches with the current frame type
	if (camera_.isValid() && (camera_.width() != frameType.width() || camera_.height() != frameType.height()))
	{
		Log::warning() << "This given camera object does not match the frame type!";
		camera_ = PinholeCamera();
	}

	// use a default camera object when no camera object is given
	if (!camera_.isValid())
	{
		ocean_assert(optimizeCamera_);
		camera_ = PinholeCamera(frameType.width(), frameType.height(), Numeric::deg2rad(65));
	}

	// at this position a valid camera object must be defined, invoke the camera event
	ocean_assert(camera_);
	eventCallbacks_(CameraCalibrationStateEvent(id_, camera_));

	const unsigned int startFrameIndex = startFrameIndex_ == (unsigned int)(-1) ? lowerFrameIndex_ : startFrameIndex_;

	// determine the initial plane from the user-defined corners of the planar rectangle
	Plane3 initialPlane;
	if (!determinePlane(camera_, initialRectangleCorners_, initialPlane))
	{
		return false;
	}

	updatePlane(initialPlane);

	bool needFinalPlaneTracking = false;

	ScopedEventStackLayer scopedEventLayer(*this, Scalar(0), Scalar(1) - Scalar(1) / Scalar(optimizationIterations + 1u));

	for (unsigned int n = 0u; n < optimizationIterations && !shouldThreadStop(); ++n)
	{
		ocean_assert(isPlausibleCamera(camera_, OfflineTracker::CM_MEDIUM_QUALITY));

		ocean_assert(optimizationIterations != 0u);
		const ScopedEventStackLayer scopedEventLayerIteration(*this, Scalar(n) / Scalar(optimizationIterations), Scalar(n + 1u) / Scalar(optimizationIterations));

		OfflinePoses poses;
		PinholeCamera optimizedCamera;
		if (!trackPlane(camera_, plane_, lowerFrameIndex_, startFrameIndex, upperFrameIndex_, true, poses, &optimizedCamera) || shouldThreadStop())
		{
			return false;
		}

		// the resulting poses have been determined with the old camera profile - so they can be accepted anyway - further an event is invoked
		updatePoses(poses);

		ocean_assert(isPlausibleCamera(optimizedCamera, OfflineTracker::CM_MEDIUM_QUALITY));

		// if the camera has not been optimized, we can break here
		if (optimizedCamera == camera_)
		{
			break;
		}

		// determine the new plane corresponding with the new camera profile
		Plane3 optimizedPlane;
		if (!determinePlane(optimizedCamera, initialRectangleCorners_, optimizedPlane))
		{
			return false;
		}

		const Scalar previousAngle = parallelogramAngle(camera_, plane_);
		const Scalar newAngle = parallelogramAngle(optimizedCamera, optimizedPlane);

		// check whether the parallelogram angle corresponding with the new camera and plane is better or almost equal than/to the previous one
		if (newAngle <= previousAngle + Numeric::deg2rad(1))
		{
			updateCamera(optimizedCamera);
			updatePlane(optimizedPlane);
		}
		else
		{
			// we will not used the new camera profile and the corresponding plane so we can stop here
			break;
		}

		// check whether we have to invoke a final tracking iteration
		if (n + 1u == optimizationIterations)
		{
			needFinalPlaneTracking = true;
		}
	}

	scopedEventLayer.modify(Scalar(1) - Scalar(1) / Scalar(optimizationIterations + 1u), Scalar(1));

	if (needFinalPlaneTracking)
	{
		OfflinePoses poses;

		if (!trackPlane(camera_, plane_, lowerFrameIndex_, startFrameIndex, upperFrameIndex_, true, poses, nullptr) || shouldThreadStop())
		{
			return false;
		}

		// update the poses and provide an event
		updatePoses(poses);
	}

	removeIrregularPoses(Scalar(5));
	extrapolatePoses(15u, 25u);
	removeIrregularPoses(Scalar(5));

	// update the poses and provide an event
	updatePoses(offlinePoses_);

	return true;
}

bool PlanarRectangleTracker::trackPlane(const PinholeCamera& pinholeCamera, const Plane3& plane, const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex, const bool /*createStateEvents*/, OfflinePoses& poses, PinholeCamera* optimizedCamera)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(lowerFrameIndex <= initialFrameIndex && initialFrameIndex <= upperFrameIndex);

	const TrackingComponentEvent startEvent(id(), ComponentEvent::CS_STARTED);
	TrackingComponentEvent breakEvent(id(), ComponentEvent::CS_FAILED);
	TrackingComponentEvent finishedEvent(id(), ComponentEvent::CS_FINISHED);

	return PlaneTrackerComponent(*this, pinholeCamera, initialPose_, plane, poses, optimizedCamera).invoke(startEvent, breakEvent, finishedEvent, lowerFrameIndex, initialFrameIndex, upperFrameIndex);
}

void PlanarRectangleTracker::updatePlane(const Plane3& plane)
{
	const ScopedLock scopedLock(lock_);
	PlaneTracker::updatePlane(plane);

	// update the object transformation as the camera has been changed
	updateObjectTransformation();
}

void PlanarRectangleTracker::updateCamera(const PinholeCamera& pinholeCamera)
{
	const ScopedLock scopedLock(lock_);
	FrameTracker::updateCamera(pinholeCamera);

	// update the object transformation as the camera has been changed
	updateObjectTransformation();
}

bool PlanarRectangleTracker::updateObjectTransformation()
{
	ocean_assert(initialPose_.isValid());

	if (!camera_.isValid() || !plane_.isValid() || !initialPose_.isValid())
	{
		return false;
	}

	const Vectors3 objectPoints(Geometry::Utilities::backProjectImagePoints(camera_, initialPose_, plane_, initialRectangleCorners_, 4, camera_.hasDistortionParameters()));

	ocean_assert(objectPoints.size() == 4);
	const Vector3 centerPosition = (objectPoints[0] + objectPoints[1] + objectPoints[2] + objectPoints[3]) * Scalar(0.25);

	// the normal of the plane will be the z-axis of the local coordinate system
	// now we need to find the y-axis

	const Vector3& zAxis = plane_.normal();
	ocean_assert(Numeric::isEqual(zAxis.length(), 1));

	// now we need to find the x-axis (which can be each of the edges of the rectangle)

	// we start to find the most north corner
	unsigned int north = 0u;
	for (unsigned int n = 1u; n < 4u; ++n)
	{
		if (initialRectangleCorners_[n].y() < initialRectangleCorners_[north].y())
		{
			north = n;
		}
	}

	// now we need to find the horizontal direction
	const unsigned int northMinus = modulo(int(north) - 1, 4);
	const unsigned int northPlus = modulo(int(north) + 1, 4);

	Vector2 xProjectedCandidate0(initialRectangleCorners_[northMinus] - initialRectangleCorners_[north]);
	Vector2 xProjectedCandidate1(initialRectangleCorners_[northPlus] - initialRectangleCorners_[north]);
	if (!xProjectedCandidate0.normalize() || !xProjectedCandidate1.normalize())
	{
		return false;
	}

	Vector3 xAxis(0, 0, 0);

	// now we take the most horizontal edge as x-axis
	if (Numeric::abs(Vector2(1, 0) * xProjectedCandidate0) > Numeric::abs(Vector2(1, 0) * xProjectedCandidate1))
	{
		// now we need to find the right direction
		if (Vector2(1, 0) * xProjectedCandidate0 >= 0)
		{
			xAxis = objectPoints[northMinus] - objectPoints[north];
		}
		else
		{
			xAxis = objectPoints[north] - objectPoints[northMinus];
		}
	}
	else
	{
		// now we need to find the right direction
		if (Vector2(1, 0) * xProjectedCandidate1 >= 0)
		{
			xAxis = objectPoints[northPlus] - objectPoints[north];
		}
		else
		{
			xAxis = objectPoints[north] - objectPoints[northPlus];
		}
	}

	if (!xAxis.normalize())
	{
		return false;
	}

	const Vector3 yAxis(zAxis.cross(xAxis));
	ocean_assert(Numeric::isEqual(yAxis.length(), 1));

	objectTransformation_ = HomogenousMatrix4(xAxis, yAxis, zAxis, centerPosition);

	eventCallbacks_(TrackerTransformationStateEvent(id_, objectTransformation_));
	return true;
}

Scalar PlanarRectangleTracker::parallelogramAngle(const PinholeCamera& pinholeCamera, const Plane3& plane)
{
	ocean_assert(pinholeCamera.isValid() && plane.isValid());

	const ObjectPoints objectPoints(Geometry::Utilities::backProjectImagePoints(pinholeCamera, initialPose_, plane, initialRectangleCorners_, 4, pinholeCamera.hasDistortionParameters()));
	const Scalar angle1_0_3 = (objectPoints[1] - objectPoints[0]).angle(objectPoints[3] - objectPoints[0]);

	return Numeric::abs(Numeric::pi_2() - angle1_0_3) * Scalar(0.5);
}

bool PlanarRectangleTracker::determinePlane(const PinholeCamera& pinholeCamera, const Vector2 rectangleCorners[4], Plane3& plane)
{
	ocean_assert(pinholeCamera.isValid());

	if (!pinholeCamera.isValid())
	{
		return false;
	}

	for (unsigned int n = 0u; n < 4u; ++n)
	{
		if (rectangleCorners[n].x() <= 0 || rectangleCorners[n].y() <= 0 || rectangleCorners[n].x() > Scalar(pinholeCamera.width() - 1u) || rectangleCorners[n].y() > Scalar(pinholeCamera.height() - 1u))
		{
			return false;
		}
	}

	Vector3 normal;
	if (!Geometry::VanishingProjection::planeNormal(pinholeCamera, rectangleCorners, pinholeCamera.hasDistortionParameters(), normal))
	{
		return false;
	}

	plane = Plane3(normal, -1);

	return true;
}

bool PlanarRectangleTracker::lookAtTransformation(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& initialPose, const Vector2 rectangleCorners[4], const Plane3& plane, const Scalar extraBorderPercent, PinholeCamera& lookAtCamera, HomogenousMatrix4& lookAtPose)
{
	ocean_assert(pinholeCamera.isValid() && plane.isValid());

	const Vectors3 objectPoints(Geometry::Utilities::backProjectImagePoints(pinholeCamera, initialPose, plane, rectangleCorners, 4u, pinholeCamera.hasDistortionParameters()));
	ocean_assert(objectPoints.size() == 4);

	const Scalar maximalPixelLength = max(max((rectangleCorners[1] - rectangleCorners[0]).length(),
											(rectangleCorners[2] - rectangleCorners[1]).length()),
											max((rectangleCorners[3] - rectangleCorners[2]).length(),
											(rectangleCorners[0] - rectangleCorners[3]).length()));

	const Scalar diagonal02 = (objectPoints[0] - objectPoints[2]).length();
	const Scalar diagonal13 = (objectPoints[1] - objectPoints[3]).length();

	Vector3 correctedObjectPoints[4];

	if (diagonal02 >= diagonal13)
	{
		const Scalar angle1_0_3 = (objectPoints[1] - objectPoints[0]).angle(objectPoints[3] - objectPoints[0]);
		ocean_assert(angle1_0_3 <= Numeric::pi_2());

		Vector3 direction01 = objectPoints[1] - objectPoints[0];
		if (!direction01.normalize())
		{
			return false;
		}

		const Vector3 correctedDirection01 = Rotation(plane.normal(), -(Numeric::pi_2() - angle1_0_3) * Scalar(0.5)) * direction01;
		const Scalar correctedLength01 = correctedDirection01 * (objectPoints[2] - objectPoints[0]);
		correctedObjectPoints[1] = objectPoints[0] + correctedDirection01 * correctedLength01;

		const Vector3 objectCenter = (objectPoints[2] + objectPoints[0]) * Scalar(0.5);
		correctedObjectPoints[3] = objectCenter * 2 - correctedObjectPoints[1];

		correctedObjectPoints[0] = objectPoints[0];
		correctedObjectPoints[2] = objectPoints[2];
	}
	else
	{
		const Scalar angle0_1_2 = (objectPoints[0] - objectPoints[1]).angle(objectPoints[2] - objectPoints[1]);
		ocean_assert(angle0_1_2 <= Numeric::pi_2());

		Vector3 direction10 = objectPoints[0] - objectPoints[1];
		if (!direction10.normalize())
		{
			return false;
		}

		const Vector3 correctedDirection10 = Rotation(plane.normal(), (Numeric::pi_2() - angle0_1_2) * Scalar(0.5)) * direction10;
		const Scalar correctedLength10 = correctedDirection10 * (objectPoints[3] - objectPoints[1]);
		correctedObjectPoints[0] = objectPoints[1] + correctedDirection10 * correctedLength10;

		const Vector3 objectCenter = (objectPoints[3] + objectPoints[1]) * Scalar(0.5);
		correctedObjectPoints[2] = objectCenter * 2 - correctedObjectPoints[0];

		correctedObjectPoints[1] = objectPoints[1];
		correctedObjectPoints[3] = objectPoints[3];
	}

	const Vector3 horizontalDirection = correctedObjectPoints[3] - correctedObjectPoints[0];
	const Vector3 verticalDirection = correctedObjectPoints[1] - correctedObjectPoints[0];

	ocean_assert(Numeric::isEqualEps(horizontalDirection * verticalDirection));
	ocean_assert(correctedObjectPoints[0] + horizontalDirection + verticalDirection == correctedObjectPoints[2]);

	const Scalar lengthHorizontal = horizontalDirection.length();
	const Scalar lengthVertical = verticalDirection.length();

	if (Numeric::isEqualEps(lengthHorizontal) || Numeric::isEqualEps(lengthVertical))
	{
		return false;
	}

	unsigned int cameraWidth, cameraHeight;
	if (lengthHorizontal > lengthVertical)
	{
		cameraWidth = (unsigned int)(maximalPixelLength * (Scalar(1) + extraBorderPercent) + Scalar(0.5));
		cameraHeight = (unsigned int)(Scalar(cameraWidth) * lengthVertical / lengthHorizontal + Scalar(0.5));
	}
	else
	{
		cameraHeight = (unsigned int)(maximalPixelLength * (Scalar(1) + extraBorderPercent) + Scalar(0.5));
		cameraWidth = (unsigned int)(Scalar(cameraHeight) * lengthHorizontal / lengthVertical + Scalar(0.5));
	}

	const Scalar poseDistance = Scalar(lengthHorizontal * (Scalar(1) + extraBorderPercent)) * Scalar(0.5) / Numeric::tan(Numeric::pi_4() * Scalar(0.5));

	lookAtCamera = PinholeCamera(cameraWidth, cameraHeight, Numeric::pi_4());

	const Vector3 xAxis(horizontalDirection / lengthHorizontal);
	const Vector3& zAxis(plane.normal());
	const Vector3 yAxis(zAxis.cross(xAxis));
	ocean_assert(Numeric::isEqual(yAxis.length(), 1));

	lookAtPose = HomogenousMatrix4(xAxis, yAxis, zAxis, (correctedObjectPoints[0] + correctedObjectPoints[2]) * Scalar(0.5) + plane.normal() * poseDistance);
	ocean_assert(lookAtPose.rotation().isValid());

	return true;
}

}

}

}
