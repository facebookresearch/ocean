/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/NonLinearOptimizationCamera.h"
#include "ocean/geometry/NonLinearOptimizationObjectPoint.h"
#include "ocean/geometry/NonLinearUniversalOptimizationSparse.h"

#include "ocean/base/HashMap.h"

#include "ocean/geometry/Error.h"
#include "ocean/geometry/Jacobian.h"

#include "ocean/math/ExponentialMap.h"
#include "ocean/math/Pose.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements the base data class allowing to optimize a camera profile for rotational camera motion based on observations of individual object points in individual camera frames.
 * The individual model provides the orientations of the cameras by three scalar parameters (as exponential map), the external model stores the orientations by a 3x3 rotation matrix.
 */
class NonLinearOptimizationCamera::CameraOrientationsBaseData
{
	protected:

		/**
		 * Definition of a hash map used as lookup table to translate ids of object points to indices of object points.
		 */
		typedef HashMap<Index32, Index32> IndexLookupTable;

	public:

		/**
		 * Transforms the internal individual model to an external individual model.
		 * @param internalModel Internal model that will be transformed, which is the camera's orientation given as exponential map
		 * @param externalModel Resulting external model, which is the inverted and flipped camera pose given as 4x4 homogenous matrix
		 */
		void transformIndividualModel(StaticBuffer<Scalar, 3>& internalModel, StaticBuffer<Scalar, 16>& externalModel)
		{
			const ExponentialMap exponentialMap(internalModel[0], internalModel[1], internalModel[2]);
			*((HomogenousMatrix4*)externalModel.data()) = PinholeCamera::standard2InvertedFlipped(HomogenousMatrix4(exponentialMap.rotation()));
		}

	protected:

		/**
		 * Creates a new data object by groups of correspondences between object point ids and image points.
		 * Each groups represents one camera frame thus the number of given groups must match the number of individual models (the orientation parameters of the camera frames).
		 * Each group must provide correspondences between object point ids (which are visible in several of the given camera frames concurrently) and image points (the observations of the object points).
		 * The number of correspondences and the individual object point ids may vary between the individual groups.
		 * However, each group must provide at least one correspondence.
		 * @param correspondenceGroups The groups of correspondences between object point ids and image points
		 */
		CameraOrientationsBaseData(const PoseGroupsAccessor& correspondenceGroups) :
			correspondenceGroups_(correspondenceGroups),
			objectPointIndexLookup_(512)
		{
			// the ids of the object points may be arbitrary ids so that they may not be defined as simple indices with values between [0, number object points)
			// so we need to determine a lookup table allowing to translate ids to indices

			ocean_assert(correspondenceGroups.groups() != 0);

			// we first determine the number of object points
			for (size_t g = 0; g < correspondenceGroups.groups(); ++g)
			{
				ocean_assert(correspondenceGroups.groupElements(g) != 0);

				for (size_t e = 0; e < correspondenceGroups.groupElements(g); ++e)
				{
					const Index32 objectPointId = correspondenceGroups.firstElement(g, e);

					objectPointIndexLookup_.insert(objectPointId, Index32(objectPointIndexLookup_.size()), true, true);
				}
			}
		}

		/**
		 * Determines the averaged object points for the individual image points and camera orientations.
		 * The averaged position of an object point is determined only due the image points which all belong to the same object point.
		 * The image points are back projected by respecting the orientations of the individual camera frames.
		 * @param pinholeCamera The current camera profile to be applied
		 * @param orientations The individual orientations of the camera frames
		 */
		void determineAveragedObjectPoints(const PinholeCamera& pinholeCamera, const SquareMatrices3& orientations)
		{
			ocean_assert(pinholeCamera.isValid());
			ocean_assert(orientations.size() == correspondenceGroups_.groups());

			averagedObjectPoints_.clear();
			averagedObjectPoints_.resize(objectPointIndexLookup_.size(), Vector3(0, 0, 0));

			Indices32 denominators(objectPointIndexLookup_.size(), 0u);

			Index32 objectPointId;
			Vector2 imagePoint;

			for (size_t g = 0; g < correspondenceGroups_.groups(); ++g)
			{
				const SquareMatrix3& orientation = orientations[g];

				for (size_t e = 0; e < correspondenceGroups_.groupElements(g); ++e)
				{
					correspondenceGroups_.element(g, e, objectPointId, imagePoint);

					const Vector2 undistortedImagePoint(pinholeCamera.hasDistortionParameters() ? pinholeCamera.undistort<true>(imagePoint) : imagePoint);
					const Vector3 objectPoint = orientation * pinholeCamera.vector(undistortedImagePoint);

					const Index32 objectPointIndex = objectPointIndexLookup_.element(objectPointId);

					ocean_assert(objectPointIndex < averagedObjectPoints_.size());
					averagedObjectPoints_[objectPointIndex] += objectPoint;
					denominators[objectPointIndex]++;
				}
			}

			for (size_t n = 0; n < averagedObjectPoints_.size(); ++n)
			{
				ocean_assert(denominators[n] != 0u);
				averagedObjectPoints_[n] /= Scalar(denominators[n]);
			}
		}

	protected:

		/// The groups of correspondences between object point ids and image points.
		const PoseGroupsAccessor& correspondenceGroups_;

		/// The averaged object point locations sorted by their index and not by their id.
		Vectors3 averagedObjectPoints_;

		/// The lookup table allowing to translate ids of object points to indices.
		IndexLookupTable objectPointIndexLookup_;
};

/**
 * This class implements a base data object allowing to optimize a camera profile.
 * @tparam tParameters The number of camera parameters to optimized
 */
template <unsigned int tParameters>
class NonLinearOptimizationCamera::CameraProfileBaseData
{
	public:

		/**
		 * Creates a new data object.
		 * @param pinholeCamera The initial camera profile which will be improved
		 */
		explicit CameraProfileBaseData(const PinholeCamera& pinholeCamera) :
			cameraWidth_(pinholeCamera.width()),
			cameraHeight_(pinholeCamera.height()),
			cameraPrincipalPointX_(pinholeCamera.principalPointX()),
			cameraPrincipalPointY_(pinholeCamera.principalPointY())
		{
			// nothing to do here
		}

		/**
		 * Creates a camera profile from a model.
		 * @param model The model from which a camera profile will be determined
		 * @return The resulting camera profile
		 */
		PinholeCamera model2profile(const StaticBuffer<Scalar, tParameters>& model) const
		{
			static_assert(tParameters == 1u || tParameters == 2u || tParameters == 4u || tParameters == 6u || tParameters == 7u || tParameters == 8u, "Invalid parameter");

			switch (tParameters)
			{
				case 1u:
					return PinholeCamera(cameraWidth_, cameraHeight_, model[0], model[0], cameraPrincipalPointX_, cameraPrincipalPointY_);

				case 2u:
					return PinholeCamera(cameraWidth_, cameraHeight_, model[0], model[1], cameraPrincipalPointX_, cameraPrincipalPointY_);

				case 4u:
					return PinholeCamera(cameraWidth_, cameraHeight_, model.data(), false, false);

				case 6u:
					return PinholeCamera(cameraWidth_, cameraHeight_, model[0], model[1], cameraPrincipalPointX_, cameraPrincipalPointY_,
										PinholeCamera::DistortionPair(model[2], model[3]), PinholeCamera::DistortionPair(model[4], model[5]));

				case 7u:
					return PinholeCamera(cameraWidth_, cameraHeight_, model[0], model[0], model[1], model[2],
										PinholeCamera::DistortionPair(model[3], model[4]), PinholeCamera::DistortionPair(model[5], model[6]));

				case 8u:
					return PinholeCamera(cameraWidth_, cameraHeight_, model.data(), true, true);
			}

			ocean_assert(false && "Invalid parameter");
			return PinholeCamera();
		}

		/**
		 * Determines whether a shared model is valid or not.
		 * @param externalSharedModel The shared model defining the camera profile
		 * @return True, if the shared model (the camera distortion) is valid
		 */
		bool sharedModelIsValid(const StaticBuffer<Scalar, tParameters>& externalSharedModel)
		{
			const PinholeCamera pinholeCamera(CameraProfileBaseData<tParameters>::model2profile(externalSharedModel));
			return pinholeCamera.isDistortionPlausible(plausibleCameraSymmetricFocalLengthRatio_, plausibleCameraModelAccuracy_, plausibleCameraSymmetricDistortionRatio_);
		}

		/**
		 * Creates a model from a given camera profile.
		 * @param pinholeCamera The camera profile for which the model will be returned
		 * @return The resulting model
		 */
		static StaticBuffer<Scalar, tParameters> profile2model(const PinholeCamera& pinholeCamera)
		{
			static_assert(tParameters == 1u || tParameters == 2u || tParameters == 4u || tParameters == 6u || tParameters == 7u || tParameters == 8u, "Invalid parameter");

			StaticBuffer<Scalar, tParameters> sharedModel;

			switch (tParameters)
			{
				case 1u:
					ocean_assert(Numeric::isEqual(pinholeCamera.focalLengthX(), pinholeCamera.focalLengthY()));
					sharedModel[0] = (pinholeCamera.focalLengthX() + pinholeCamera.focalLengthY()) * Scalar(0.5);
					break;

				case 2u:
					sharedModel[0] = pinholeCamera.focalLengthX();
					sharedModel[1] = pinholeCamera.focalLengthY();
					break;

				case 4u:
					pinholeCamera.copyElements(sharedModel.data(), false, false);
					break;

				case 6u:
					sharedModel[0] = pinholeCamera.focalLengthX();
					sharedModel[1] = pinholeCamera.focalLengthY();
					sharedModel[2] = pinholeCamera.radialDistortion().first;
					sharedModel[3] = pinholeCamera.radialDistortion().second;
					sharedModel[4] = pinholeCamera.tangentialDistortion().first;
					sharedModel[5] = pinholeCamera.tangentialDistortion().second;
					break;

				case 7u:
					ocean_assert(Numeric::isWeakEqual(pinholeCamera.focalLengthX(), pinholeCamera.focalLengthY()));
					sharedModel[0] = (pinholeCamera.focalLengthX() + pinholeCamera.focalLengthY()) * Scalar(0.5);
					sharedModel[1] = pinholeCamera.principalPointX();
					sharedModel[2] = pinholeCamera.principalPointY();
					sharedModel[3] = pinholeCamera.radialDistortion().first;
					sharedModel[4] = pinholeCamera.radialDistortion().second;
					sharedModel[5] = pinholeCamera.tangentialDistortion().first;
					sharedModel[6] = pinholeCamera.tangentialDistortion().second;
					break;

				case 8u:
					pinholeCamera.copyElements(sharedModel.data(), true, true);
					break;

				default:
					ocean_assert(false && "Invalid parameter");
					break;
			}

			return sharedModel;
		}

	protected:

		/// The width of the camera in pixel.
		const unsigned int cameraWidth_;

		/// The height of the camera in pixel.
		const unsigned int cameraHeight_;

		/// The horizontal principal point.
		const Scalar cameraPrincipalPointX_;

		/// The vertical principal point.
		const Scalar cameraPrincipalPointY_;

		/// The symmetric focal length ratio of a plausible camera profile.
		const Scalar plausibleCameraSymmetricFocalLengthRatio_ = Scalar(1.1);

		/// The model accuracy of a plausible camera profile.
		const Scalar plausibleCameraModelAccuracy_ = Scalar(0.001);

		/// The symmetric distortion ratio of a plausible camera profile.
		const Scalar plausibleCameraSymmetricDistortionRatio_ = Scalar(1.1);
};

/**
 * This class implements a data object allowing to optimize the field of view for individual camera poses with only rotational movements.
 */
class NonLinearOptimizationCamera::CameraOrientationsFovData : public CameraOrientationsBaseData
{
	public:

		/**
		 * Creates a new data object.
		 * @param pinholeCamera The initial camera profile for which an improved field of view will be determined
		 * @param correspondenceGroups The groups of correspondences between object point ids and image points
		 * @param onlyFrontObjectPoints True, to avoid that 3D position lies behind any camera in which the object point is visible
		 */
		CameraOrientationsFovData(const PinholeCamera& pinholeCamera, const PoseGroupsAccessor& correspondenceGroups, const bool onlyFrontObjectPoints) :
			CameraOrientationsBaseData(correspondenceGroups),
			camera_(pinholeCamera),
			onlyFrontObjectPoints_(onlyFrontObjectPoints)
		{
			// nothing to do here
		}

		/**
		 * Determines the value for a specified camera frame and specified element for a given model.
		 * @param externalSharedModel The current shared model to be applied (the focal length parameter)
		 * @param externalIndividualModel The current individual model to be applied (the frame's orientation)
		 * @param individualModelIndex The index of the individual model (the index of the camera frame)
		 * @param elementIndex The index of the correspondence between object point and image point visible in the frame
		 * @param result The projected object point
		 */
		void value(const StaticBuffer<Scalar, 1>& externalSharedModel, const StaticBuffer<Scalar, 16>& externalIndividualModel, const size_t individualModelIndex, const size_t elementIndex, StaticBuffer<Scalar, 2>& result)
		{
			const PinholeCamera pinholeCamera(camera_.width(), camera_.height(), externalSharedModel[0], externalSharedModel[0], camera_.principalPointX(), camera_.principalPointY(), camera_.radialDistortion(), camera_.tangentialDistortion());
			const HomogenousMatrix4& poseIF = *((HomogenousMatrix4*)externalIndividualModel.data());

			const Index32 objectPointId = correspondenceGroups_.firstElement(individualModelIndex, elementIndex);
			const Index32 objectPointIndex = objectPointIndexLookup_.element(objectPointId);

			const Vector2 point = pinholeCamera.projectToImageIF<true>(poseIF, averagedObjectPoints_[objectPointIndex], pinholeCamera.hasDistortionParameters());

			result[0] = point[0];
			result[1] = point[1];
		}

		/**
		 * Determines the error between a projected object point and the expected point for a given model.
		 * @param externalSharedModel The current shared model to be applied (the focal length parameter)
		 * @param externalIndividualModel The current individual model to be applied (the frame's orientation)
		 * @param individualModelIndex The index of the individual model (the index of the camera frame)
		 * @param elementIndex The index of the image point in the frame
		 * @param result The resulting error (offset from the expected point to the observed point)
		 * @return True, if succeeded
		 */
		bool error(const StaticBuffer<Scalar, 1>& externalSharedModel, const StaticBuffer<Scalar, 16>& externalIndividualModel, const size_t individualModelIndex, const size_t elementIndex, StaticBuffer<Scalar, 2>& result)
		{
			const PinholeCamera pinholeCamera(camera_.width(), camera_.height(), externalSharedModel[0], externalSharedModel[0], camera_.principalPointX(), camera_.principalPointY(), camera_.radialDistortion(), camera_.tangentialDistortion());
			const HomogenousMatrix4& poseIF = *((HomogenousMatrix4*)externalIndividualModel.data());

			Index32 objectPointId;
			Vector2 imagePoint;

			correspondenceGroups_.element(individualModelIndex, elementIndex, objectPointId, imagePoint);
			const Index32 objectPointIndex = objectPointIndexLookup_.element(objectPointId);

			const Vector3& objectPoint = averagedObjectPoints_[objectPointIndex];

			// ensure that the object point lies in front of the camera
			if (onlyFrontObjectPoints_ && (poseIF * objectPoint).z() <= Numeric::eps())
			{
				return false;
			}

			const Vector2 point = pinholeCamera.projectToImageIF<true>(poseIF, objectPoint, pinholeCamera.hasDistortionParameters());

			const Vector2 error(point - imagePoint);

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
			const PinholeCamera pinholeCamera(camera_.width(), camera_.height(), externalSharedModel[0], externalSharedModel[0], camera_.principalPointX(), camera_.principalPointY(), camera_.radialDistortion(), camera_.tangentialDistortion());

			SquareMatrices3 orientations(externalIndividualModels.size());
			for (size_t n = 0; n < externalIndividualModels.size(); ++n)
			{
				const ExponentialMap exponentialMap(externalIndividualModels[n][0], externalIndividualModels[n][1], externalIndividualModels[n][2]);
				orientations[n] = SquareMatrix3(exponentialMap.rotation());
			}

			determineAveragedObjectPoints(pinholeCamera, orientations);
		}

	protected:

		/// The camera profile to optimize.
		const PinholeCamera& camera_;

		/// True, to avoid that a 3D position lies behind any camera in which the object point is visible.
		const bool onlyFrontObjectPoints_;
};

bool NonLinearOptimizationCamera::findInitialFieldOfView(const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<SquareMatrix3>& orientations, const PoseGroupsAccessor& correspondenceGroups, PinholeCamera& optimizedCamera, NonconstIndexedAccessor<SquareMatrix3>* optimizedOrientations, const Scalar lowerFovX, const Scalar upperFovX, const unsigned int steps, const unsigned int recursiveIterations, const bool onlyFrontObjectPoints, bool* significantResult, Scalar* finalError, Worker* worker, bool* abort)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(orientations.size() == correspondenceGroups.groups());
	ocean_assert(orientations.size() >= 1);

	ocean_assert(optimizedOrientations == nullptr || optimizedOrientations->size() == orientations.size());

	ocean_assert(steps >= 4u);
	ocean_assert(recursiveIterations >= 1u);

	ocean_assert(lowerFovX > 0 && upperFovX < Numeric::pi());
	ocean_assert(lowerFovX <= upperFovX);

	Scalar globalLowerFovX = lowerFovX;
	Scalar globalUpperFovX = upperFovX;

	SquareMatrices3 localOptimizedOrientations;

	Scalars allErrors;
	allErrors.reserve(steps * recursiveIterations);

	Scalar bestError = Numeric::maxValue();
	Scalar bestFovX = -1;

	for (unsigned int iRec = 0u; (!abort || !*abort) && iRec < recursiveIterations; ++iRec)
	{
		const bool finestLayer = iRec + 1u == recursiveIterations;

		bestError = Numeric::maxValue();

		const Scalar recLowerFovX = globalLowerFovX;
		const Scalar recUpperFovX = globalUpperFovX;

		optimizedCamera = pinholeCamera;

		if (worker)
		{
			Lock lock;
			worker->executeFunction(Worker::Function::createStatic(&findInitialFieldOfViewSubset, &pinholeCamera, &orientations, &correspondenceGroups, &optimizedCamera, (optimizedOrientations && finestLayer) ? &localOptimizedOrientations : nullptr, recLowerFovX, recUpperFovX, steps, onlyFrontObjectPoints, &bestError, &allErrors, &lock, abort, 0u, 0u), 0u, steps);
		}
		else
		{
			findInitialFieldOfViewSubset(&pinholeCamera, &orientations, &correspondenceGroups, &optimizedCamera, (optimizedOrientations && finestLayer) ? &localOptimizedOrientations : nullptr, recLowerFovX, recUpperFovX, steps, onlyFrontObjectPoints, &bestError, &allErrors, nullptr, abort, 0u, steps);
		}

		bestFovX = optimizedCamera.fovX();

		// we set the boundary for the next recursive iteration to the next left and next right step
		globalLowerFovX = max(lowerFovX, bestFovX - (recUpperFovX - recLowerFovX) / Scalar(steps - 1u));
		globalUpperFovX = min(bestFovX + (recUpperFovX - recLowerFovX) / Scalar(steps - 1u), upperFovX);
	}

	if (abort && *abort)
	{
		return false;
	}

	ocean_assert(bestFovX != -1);

	if (finalError)
	{
		*finalError = bestError;
	}

	if (optimizedOrientations)
	{
		ocean_assert(optimizedOrientations->size() == localOptimizedOrientations.size());
		for (size_t i = 0; i < localOptimizedOrientations.size(); ++i)
		{
			(*optimizedOrientations)[i] = localOptimizedOrientations[i];
		}
	}

	if (significantResult)
	{
		// we decide whether the resulting fov has a significant impact due to the individual error results from the coarsest iteration
		// if the resulting field of view is identical to the given angle range, we can expect that we have a result which is not suitable

		if (allErrors.size() < 3 || Numeric::isEqual(bestFovX, lowerFovX) || Numeric::isEqual(bestFovX, upperFovX))
		{
			*significantResult = false;
		}
		else
		{
			std::sort(allErrors.begin(), allErrors.end());

			// we do not investigate the two worst results due to optimization failure issues etc.
			const Scalar badError = allErrors[allErrors.size() - 3];

			// we think that the resulting fov has a significant impact on the result if the 'bad' error is 1.5 times larger than the best error
			*significantResult = bestError * Scalar(1.5 * 1.5) < badError;
		}
	}

	return (!abort || !*abort) && bestError != Numeric::maxValue();
}

bool NonLinearOptimizationCamera::findInitialFieldOfView(const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<HomogenousMatrix4>& poses, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, PinholeCamera& optimizedCamera, NonconstIndexedAccessor<HomogenousMatrix4>* optimizedPoses, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const Scalar lowerFovX, const Scalar upperFovX, const unsigned int steps, const unsigned int recursiveIterations, const bool onlyFrontObjectPoints, bool* significantResult, Scalar* finalError, Worker* worker, bool* abort)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(poses.size() >= 1);
	ocean_assert(objectPoints.size() == correspondenceGroups.groups());
	ocean_assert(objectPoints.size() >= 1);

	ocean_assert(optimizedPoses == nullptr || optimizedPoses->size() == poses.size());
	ocean_assert(optimizedObjectPoints == nullptr || optimizedObjectPoints->size() == objectPoints.size());

	ocean_assert(steps >= 4u);
	ocean_assert(recursiveIterations >= 1u);

	ocean_assert(lowerFovX > 0 && upperFovX < Numeric::pi());
	ocean_assert(lowerFovX <= upperFovX);

	Scalar globalLowerFovX = lowerFovX;
	Scalar globalUpperFovX = upperFovX;

	Scalars allErrors;
	allErrors.reserve(steps * recursiveIterations);

	Scalar bestError = Numeric::maxValue();
	Scalar bestFovX = -1;

	HomogenousMatrices4 localOptimizedPoses;
	Vectors3 localOptimizedObjectPoints;

	for (unsigned int iRec = 0u; (!abort || !*abort) && iRec < recursiveIterations; ++iRec)
	{
		const bool finestLayer = iRec + 1u == recursiveIterations;

		bestError = Numeric::maxValue();

		const Scalar recLowerFovX = globalLowerFovX;
		const Scalar recUpperFovX = globalUpperFovX;

		optimizedCamera = pinholeCamera;

		if (worker)
		{
			Lock lock;
			worker->executeFunction(Worker::Function::createStatic(&findInitialFieldOfViewSubset, &pinholeCamera, &poses, &objectPoints, &correspondenceGroups, &optimizedCamera, (optimizedPoses && finestLayer) ? &localOptimizedPoses : nullptr, (optimizedObjectPoints && finestLayer) ? &localOptimizedObjectPoints : nullptr, recLowerFovX, recUpperFovX, steps, onlyFrontObjectPoints, &bestError, &allErrors, &lock, abort, 0u, 0u), 0u, steps);
		}
		else
		{
			findInitialFieldOfViewSubset(&pinholeCamera, &poses, &objectPoints, &correspondenceGroups, &optimizedCamera, (optimizedPoses && finestLayer) ? &localOptimizedPoses : nullptr, (optimizedObjectPoints && finestLayer) ? &localOptimizedObjectPoints : nullptr, recLowerFovX, recUpperFovX, steps, onlyFrontObjectPoints, &bestError, &allErrors, nullptr, abort, 0u, steps);
		}

		bestFovX = optimizedCamera.fovX();

		// we set the boundary for the next recursive iteration to the nect left and next right step
		globalLowerFovX = max(lowerFovX, bestFovX - (recUpperFovX - recLowerFovX) / Scalar(steps - 1u));
		globalUpperFovX = min(bestFovX + (recUpperFovX - recLowerFovX) / Scalar(steps - 1u), upperFovX);
	}

	if (abort && *abort)
	{
		return false;
	}

	ocean_assert(bestFovX != -1);

	if (finalError)
	{
		*finalError = bestError;
	}

	if (optimizedPoses)
	{
		ocean_assert(optimizedPoses->size() == localOptimizedPoses.size());
		for (size_t n = 0; n < localOptimizedPoses.size(); ++n)
		{
			(*optimizedPoses)[n] = localOptimizedPoses[n];
		}
	}

	if (optimizedObjectPoints)
	{
		ocean_assert(optimizedObjectPoints->size() == localOptimizedObjectPoints.size());
		for (size_t n = 0; n < localOptimizedObjectPoints.size(); ++n)
		{
			(*optimizedObjectPoints)[n] = localOptimizedObjectPoints[n];
		}
	}

	if (significantResult)
	{
		// we decide whether the resulting fov has a significant impact due to the individual error results from the coarsest iteration
		// if the resulting field of view is identical to the given angle range, we can expect that we have a result which is not suitable

		if (allErrors.size() < 3 || Numeric::isEqual(bestFovX, lowerFovX) || Numeric::isEqual(bestFovX, upperFovX))
		{
			*significantResult = false;
		}
		else
		{
			std::sort(allErrors.begin(), allErrors.end());

			// we do not investigate the two worst results due to optimization failure issues etc.
			const Scalar badError = allErrors[allErrors.size() - 3];

			// we think that the resulting fov has a significant impact on the result if the 'bad' error is 1.5 times larger than the best error
			*significantResult = bestError * Scalar(1.5 * 1.5) < badError;
		}
	}

	return (!abort || !*abort) && bestError != Numeric::maxValue();
}

/**
 * This class implements an optimization provider for a camera object.
 * Only the (radial and tangential) distortion parameters are optimized.<br>
 * The optimization needs several point correspondences between 3D object points (normalized 2D projected object points) and 2D image points.<br>
 * The provider optimizes the camera parameters by minimizing the projection error between 3D object and 2D image points.
 */
template <PinholeCamera::OptimizationStrategy tOptimizationStrategy>
class NonLinearOptimizationCamera::CameraProvider : public NonLinearOptimization::OptimizationProvider
{
	public:

		/**
		 * Creates a new optimization provider object.
		 * @param pinholeCamera The camera object to be optimized
		 * @param normalizedObjectPoints Normalized and projected 2D object points
		 * @param imagePoints 2D observation image points, each point corresponds to one projected object point
		 * @param numberObservations Number of points observations, with range [1, infinity)
		 */
		inline CameraProvider(PinholeCamera& pinholeCamera, const ImagePoint* normalizedObjectPoints, const ImagePoint* imagePoints, const size_t numberObservations) :
			camera_(pinholeCamera),
			candidateCamera_(pinholeCamera),
			normalizedObjectPoints_(normalizedObjectPoints),
			imagePoints_(imagePoints),
			numberObservations_(numberObservations)
		{
			ocean_assert(numberObservations_ >= 1);
		};

		/**
		 * Determines the jacobian matrix for the current camera.
		 * @param jacobian Jacobian matrix
		 */
		inline void determineJacobian(Matrix& jacobian) const;

		/**
		 * Applies the camera correction and stores the new camera as candidate
		 * @param deltas Optimization deltas that define the correction
		 */
		inline void applyCorrection(const Matrix& deltas);

		/**
		 * Determines the robust error of the current candidate pose.
		 * @param weightedErrorVector Resulting (weighted - if using a robust estimator) error vector
		 * @param weightVector Vector holding the weights that have already been applied to the error vector
		 * @param invertedCovariances Optional 2x2 inverted covariance matrices
		 * @return The resulting robust error
		 */
		template <Estimator::EstimatorType tEstimator>
		Scalar determineRobustError(Matrix& weightedErrorVector, Matrix& weightVector, const Matrix* invertedCovariances) const
		{
			// set the correct size of the resulting error vector
			weightedErrorVector.resize(2 * numberObservations_, 1);

			Vector2* const weightedErrors = (Vector2*)weightedErrorVector.data();
			const SquareMatrix2* transposedInvertedCovariances = invertedCovariances ? (SquareMatrix2*)invertedCovariances->data() : nullptr;

			// check whether the standard estimator is used
			if constexpr (Estimator::isStandardEstimator<tEstimator>())
			{
				// the weight vector should be and should stay invalid
				ocean_assert(!weightVector);

				// return the averaged square error
				Scalar sqrError = Error::determineCameraError(candidateCamera_, normalizedObjectPoints_, imagePoints_, numberObservations_, true, weightedErrors);

				if (transposedInvertedCovariances)
				{
					sqrError = 0;
					for (size_t n = 0; n < numberObservations_; ++n)
					{
						sqrError += (transposedInvertedCovariances[n].transposed() * weightedErrors[n]).sqr();
					}

					sqrError /= Scalar(numberObservations_);
				}

				return sqrError;
			}
			else
			{
				// now we need the weight vector
				weightVector.resize(2 * numberObservations_, 1);

				Scalars sqrErrors(numberObservations_);
				Error::determineCameraError(candidateCamera_, normalizedObjectPoints_, imagePoints_, numberObservations_, true, (Vector2*)weightedErrorVector.data(), sqrErrors.data());

				ocean_assert(tOptimizationStrategy != PinholeCamera::OS_DISTORTION || (tOptimizationStrategy & 0xFF) == 4);
				ocean_assert(tOptimizationStrategy != PinholeCamera::OS_INTRINSIC_PARAMETERS_RADIAL_DISTORTION || (tOptimizationStrategy & 0xFF) == 6);
				ocean_assert(tOptimizationStrategy != PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS || (tOptimizationStrategy & 0xFF) == 8);
				return sqrErrors2robustErrors2<tEstimator>(sqrErrors, (tOptimizationStrategy & 0xFF), weightedErrors, (Vector2*)weightVector.data(), transposedInvertedCovariances);
			}
		}

		/**
		 * Accepts the current pose candidate as better model.
		 */
		inline void acceptCorrection()
		{
			camera_ = candidateCamera_;
		}

	protected:

		/// The camera object that will be optimized.
		PinholeCamera& camera_;

		/// The camera object that stores the most recent optimization result as candidate.
		PinholeCamera candidateCamera_;

		/// The normalized and projected 2D object points.
		const ImagePoint* normalizedObjectPoints_;

		/// The observed 2D image points.
		const ImagePoint* imagePoints_;

		/// Number of point observations.
		const size_t numberObservations_;
};

template <>
inline void NonLinearOptimizationCamera::CameraProvider<PinholeCamera::OS_DISTORTION>::determineJacobian(Matrix& jacobian) const
{
	jacobian.resize(2 * numberObservations_, 4);

	for (unsigned int n = 0; n < numberObservations_; ++n)
	{
		Jacobian::calculateCameraDistortionJacobian2x4(jacobian[2 * n + 0], jacobian[2 * n + 1], camera_, normalizedObjectPoints_[n]);
	}
}

template <>
inline void NonLinearOptimizationCamera::CameraProvider<PinholeCamera::OS_INTRINSIC_PARAMETERS_RADIAL_DISTORTION>::determineJacobian(Matrix& jacobian) const
{
	jacobian.resize(2 * numberObservations_, 6);

	for (unsigned int n = 0; n < numberObservations_; ++n)
	{
		Jacobian::calculateCameraJacobian2x6(jacobian[2 * n + 0], jacobian[2 * n + 1], camera_, normalizedObjectPoints_[n]);
	}
}

template <>
inline void NonLinearOptimizationCamera::CameraProvider<PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS>::determineJacobian(Matrix& jacobian) const
{
	jacobian.resize(2 * numberObservations_, 8);

	for (unsigned int n = 0; n < numberObservations_; ++n)
	{
		Jacobian::calculateCameraJacobian2x8(jacobian[2 * n + 0], jacobian[2 * n + 1], camera_, normalizedObjectPoints_[n]);
	}
}

template <>
inline void NonLinearOptimizationCamera::CameraProvider<PinholeCamera::OS_DISTORTION>::applyCorrection(const Matrix& deltas)
{
	ocean_assert(deltas.rows() == 4u && deltas.columns() == 1u);

	const Scalar& deltaK1 = deltas(0);
	const Scalar& deltaK2 = deltas(1);
	const Scalar& deltaP1 = deltas(2);
	const Scalar& deltaP2 = deltas(3);

	const Scalar newK1 = camera_.radialDistortion().first - deltaK1;
	const Scalar newK2 = camera_.radialDistortion().second - deltaK2;
	const Scalar newP1 = camera_.tangentialDistortion().first - deltaP1;
	const Scalar newP2 = camera_.tangentialDistortion().second - deltaP2;

	candidateCamera_ = PinholeCamera(SquareMatrix3(camera_.focalLengthX(), 0, 0, 0, camera_.focalLengthY(), 0, camera_.principalPointX(), camera_.principalPointY(), 1), camera_.width(), camera_.height(), PinholeCamera::DistortionPair(newK1, newK2), PinholeCamera::DistortionPair(newP1, newP2));
}

template <>
inline void NonLinearOptimizationCamera::CameraProvider<PinholeCamera::OS_INTRINSIC_PARAMETERS_RADIAL_DISTORTION>::applyCorrection(const Matrix& deltas)
{
	ocean_assert(deltas.rows() == 6u && deltas.columns() == 1u);

	const Scalar& deltaK1 = deltas(0);
	const Scalar& deltaK2 = deltas(1);
	const Scalar& deltaFx = deltas(2);
	const Scalar& deltaFy = deltas(3);
	const Scalar& deltaMx = deltas(4);
	const Scalar& deltaMy = deltas(5);

	const Scalar newK1 = camera_.radialDistortion().first - deltaK1;
	const Scalar newK2 = camera_.radialDistortion().second - deltaK2;
	const Scalar newFx = camera_.focalLengthX() - deltaFx;
	const Scalar newFy = camera_.focalLengthY() - deltaFy;
	const Scalar newMx = camera_.principalPointX() - deltaMx;
	const Scalar newMy = camera_.principalPointY() - deltaMy;

	candidateCamera_ = PinholeCamera(SquareMatrix3(newFx, 0, 0, 0, newFy, 0, newMx, newMy, 1), camera_.width(), camera_.height(), PinholeCamera::DistortionPair(newK1, newK2), PinholeCamera::DistortionPair());
}

template <>
inline void NonLinearOptimizationCamera::CameraProvider<PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS>::applyCorrection(const Matrix& deltas)
{
	ocean_assert(deltas.rows() == 8u && deltas.columns() == 1u);

	const Scalar& deltaK1 = deltas(0);
	const Scalar& deltaK2 = deltas(1);
	const Scalar& deltaP1 = deltas(2);
	const Scalar& deltaP2 = deltas(3);
	const Scalar& deltaFx = deltas(4);
	const Scalar& deltaFy = deltas(5);
	const Scalar& deltaMx = deltas(6);
	const Scalar& deltaMy = deltas(7);

	const Scalar newK1 = camera_.radialDistortion().first - deltaK1;
	const Scalar newK2 = camera_.radialDistortion().second - deltaK2;
	const Scalar newP1 = camera_.tangentialDistortion().first - deltaP1;
	const Scalar newP2 = camera_.tangentialDistortion().second - deltaP2;
	const Scalar newFx = camera_.focalLengthX() - deltaFx;
	const Scalar newFy = camera_.focalLengthY() - deltaFy;
	const Scalar newMx = camera_.principalPointX() - deltaMx;
	const Scalar newMy = camera_.principalPointY() - deltaMy;

	candidateCamera_ = PinholeCamera(SquareMatrix3(newFx, 0, 0, 0, newFy, 0, newMx, newMy, 1), camera_.width(), camera_.height(), PinholeCamera::DistortionPair(newK1, newK2), PinholeCamera::DistortionPair(newP1, newP2));
}

bool NonLinearOptimizationCamera::optimizeCamera(const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<Vector2>& normalizedObjectPoints, const ConstIndexedAccessor<Vector2>& imagePoints, const PinholeCamera::OptimizationStrategy optimizationStrategy, PinholeCamera& optimizedCamera, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError)
{
	ocean_assert(&pinholeCamera != &optimizedCamera);
	optimizedCamera = pinholeCamera;

	ocean_assert(normalizedObjectPoints.size() == imagePoints.size());
	if (normalizedObjectPoints.size() != imagePoints.size())
	{
		return false;
	}

	const ScopedConstMemoryAccessor<Vector2> scopedNormalizedObjectPointMemoryAccessor(normalizedObjectPoints);
	const ScopedConstMemoryAccessor<Vector2> scopedImagePointMemoryAccessor(imagePoints);

	switch (optimizationStrategy)
	{
		case PinholeCamera::OS_DISTORTION:
		{
			CameraProvider<PinholeCamera::OS_DISTORTION> provider(optimizedCamera, scopedNormalizedObjectPointMemoryAccessor.data(), scopedImagePointMemoryAccessor.data(), scopedNormalizedObjectPointMemoryAccessor.size());
			return denseOptimization< CameraProvider<PinholeCamera::OS_DISTORTION> >(provider, iterations, estimator, lambda, lambdaFactor, initialError, finalError);
		}

		case PinholeCamera::OS_INTRINSIC_PARAMETERS_RADIAL_DISTORTION:
		{
			CameraProvider<PinholeCamera::OS_INTRINSIC_PARAMETERS_RADIAL_DISTORTION> provider(optimizedCamera, scopedNormalizedObjectPointMemoryAccessor.data(), scopedImagePointMemoryAccessor.data(), scopedNormalizedObjectPointMemoryAccessor.size());
			return denseOptimization< CameraProvider<PinholeCamera::OS_INTRINSIC_PARAMETERS_RADIAL_DISTORTION> >(provider, iterations, estimator, lambda, lambdaFactor, initialError, finalError);
		}

		case PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS:
		{
			CameraProvider<PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS> provider(optimizedCamera, scopedNormalizedObjectPointMemoryAccessor.data(), scopedImagePointMemoryAccessor.data(), scopedNormalizedObjectPointMemoryAccessor.size());
			return denseOptimization< CameraProvider<PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS> >(provider, iterations, estimator, lambda, lambdaFactor, initialError, finalError);
		}

		default:
			break;
	}

	ocean_assert(false && "Invalid optimization strategy!");
	return false;
}

/**
 * This class implements a data object allowing to optimize the all intrinsic (and distortion) camera parameters for individual camera poses with only rotational movements.
 * @tparam tParameters The number of camera parameters to optimized
 */
template <unsigned int tParameters>
class NonLinearOptimizationCamera::CameraOrientationsData :
	public CameraOrientationsBaseData,
	public CameraProfileBaseData<tParameters>
{
	public:

		/**
		 * Creates a new data object.
		 * @param pinholeCamera The initial camera profile which will be improved
		 * @param correspondenceGroups The groups of correspondences between object point ids and image points
		 * @param onlyFrontObjectPoints True, to avoid that an optimized 3D position lies behind any camera in which the object point is visible
		 */
		CameraOrientationsData(const PinholeCamera& pinholeCamera, const PoseGroupsAccessor& correspondenceGroups, const bool onlyFrontObjectPoints) :
			CameraOrientationsBaseData(correspondenceGroups),
			CameraProfileBaseData<tParameters>(pinholeCamera),
			onlyFrontObjectPoints_(onlyFrontObjectPoints)
		{
			// nothing to do here
		}

		/**
		 * Determines the value for a specified camera frame and specified element for a given model.
		 * @param externalSharedModel The current shared model to be applied (the focal length parameter)
		 * @param externalIndividualModel The current individual model to be applied (the frame's orientation)
		 * @param individualModelIndex The index of the individual model (the index of the camera frame)
		 * @param elementIndex The index of the correspondence between object point and image point visible in the frame
		 * @param result The projected object point
		 */
		void value(const StaticBuffer<Scalar, tParameters>& externalSharedModel, const StaticBuffer<Scalar, 16>& externalIndividualModel, const size_t individualModelIndex, const size_t elementIndex, StaticBuffer<Scalar, 2>& result)
		{
			const PinholeCamera pinholeCamera(CameraProfileBaseData<tParameters>::model2profile(externalSharedModel));
			const HomogenousMatrix4& poseIF = *((HomogenousMatrix4*)externalIndividualModel.data());

			const Index32 objectPointId = correspondenceGroups_.firstElement(individualModelIndex, elementIndex);
			const Index32 objectPointIndex = objectPointIndexLookup_.element(objectPointId);

			const Vector2 point = pinholeCamera.projectToImageIF<true>(poseIF, averagedObjectPoints_[objectPointIndex], pinholeCamera.hasDistortionParameters());

			result[0] = point[0];
			result[1] = point[1];
		}

		/**
		 * Determines the error between a projected object point and the expected point for a given model.
		 * @param externalSharedModel The current shared model to be applied (the focal length parameter)
		 * @param externalIndividualModel The current individual model to be applied (the frame's orientation)
		 * @param individualModelIndex The index of the individual model (the index of the camera frame)
		 * @param elementIndex The index of the image point in the frame
		 * @param result The resulting error (offset from the expected point to the observed point)
		 * @return True, if succeeded
		 */
		bool error(const StaticBuffer<Scalar, tParameters>& externalSharedModel, const StaticBuffer<Scalar, 16>& externalIndividualModel, const size_t individualModelIndex, const size_t elementIndex, StaticBuffer<Scalar, 2>& result)
		{
			const PinholeCamera pinholeCamera(CameraProfileBaseData<tParameters>::model2profile(externalSharedModel));
			const HomogenousMatrix4& poseIF = *((HomogenousMatrix4*)externalIndividualModel.data());

			Index32 objectPointId;
			Vector2 imagePoint;

			correspondenceGroups_.element(individualModelIndex, elementIndex, objectPointId, imagePoint);
			const Index32 objectPointIndex = objectPointIndexLookup_.element(objectPointId);

			const Vector3& objectPoint = averagedObjectPoints_[objectPointIndex];

			// ensure that the object point lies in front of the camera
			if (onlyFrontObjectPoints_ && (poseIF * objectPoint).z() <= Numeric::eps())
			{
				return false;
			}

			const Vector2 point = pinholeCamera.projectToImageIF<true>(poseIF, objectPoint, pinholeCamera.hasDistortionParameters());

			const Vector2 error(point - imagePoint);

			result[0] = error[0];
			result[1] = error[1];
			return true;
		}

		/**
		 * Transforms the internal model to the external model (which is the same in our case).
		 * @param internalModel The internal model
		 * @param externalModel The external model
		 */
		void transformSharedModel(StaticBuffer<Scalar, tParameters>& internalModel, StaticBuffer<Scalar, tParameters>& externalModel)
		{
			externalModel = internalModel;
		}

		/**
		 * Accepts a new model that has been verified as a better model (or the first model).
		 * @param externalSharedModel The new external shared model
		 * @param externalIndividualModels The new external individual models
		 */
		void acceptModel(const StaticBuffer<Scalar, tParameters>& externalSharedModel, const std::vector< StaticBuffer<Scalar, 3> >& externalIndividualModels)
		{
			const PinholeCamera pinholeCamera(CameraProfileBaseData<tParameters>::model2profile(externalSharedModel));

			SquareMatrices3 orientations(externalIndividualModels.size());
			for (size_t n = 0; n < externalIndividualModels.size(); ++n)
			{
				const ExponentialMap exponentialMap(externalIndividualModels[n][0], externalIndividualModels[n][1], externalIndividualModels[n][2]);
				orientations[n] = SquareMatrix3(exponentialMap.rotation());
			}

			determineAveragedObjectPoints(pinholeCamera, orientations);
		}

	protected:

		/// True, to avoid that an optimized 3D position lies behind any camera in which the object point is visible.
		const bool onlyFrontObjectPoints_;
};

bool NonLinearOptimizationCamera::optimizeCameraOrientations(const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<SquareMatrix3>& orientations, const PoseGroupsAccessor& correspondenceGroups, const PinholeCamera::OptimizationStrategy optimizationStrategy, PinholeCamera& optimizedCamera, NonconstIndexedAccessor<SquareMatrix3>* optimizedOrientations, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator, const Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(orientations.size() == correspondenceGroups.groups());
	ocean_assert(orientations.size() >= 1);

	ocean_assert(optimizedOrientations == nullptr || optimizedOrientations->size() == orientations.size());

	// shared model: camera profile with 4, 6 or 8 scalar parameters
	// individual model: camera orientation with 3 scalar parameters for each orientation

	typedef StaticBuffer<Scalar, 3> IndividualModel;
	typedef std::vector<IndividualModel> IndividualModels;

	IndividualModels individualModels, optimizedIndividualModels;
	for (size_t n = 0; n < orientations.size(); ++n)
	{
		const ExponentialMap exponentialMap(orientations[n]);
		individualModels.push_back(IndividualModel(exponentialMap.data()));
	}

	std::vector<size_t> numberElementsPerIndividualModel;
	numberElementsPerIndividualModel.reserve(correspondenceGroups.groups());

	for (size_t i = 0; i < correspondenceGroups.groups(); ++i)
	{
		ocean_assert(correspondenceGroups.groupElements(i) != 0);
		numberElementsPerIndividualModel.push_back(correspondenceGroups.groupElements(i));
	}

	switch (optimizationStrategy)
	{
		case PinholeCamera::OS_FOCAL_LENGTH:
		{
			// we have shared model with 4 parameters

			typedef NonLinearUniversalOptimizationSparse::SharedModelIndividualModels<PinholeCamera::OS_FOCAL_LENGTH, 3u, 2u, PinholeCamera::OS_FOCAL_LENGTH, 16u> UniversalOptimization;

			UniversalOptimization::SharedModel sharedModel = CameraOrientationsData<PinholeCamera::OS_FOCAL_LENGTH>::profile2model(pinholeCamera);
			UniversalOptimization::SharedModel optimizedSharedModel;

			CameraOrientationsData<PinholeCamera::OS_FOCAL_LENGTH> data(pinholeCamera, correspondenceGroups, onlyFrontObjectPoints);

			if (!UniversalOptimization::optimizeUniversalModel(sharedModel, individualModels, numberElementsPerIndividualModel.data(),
							UniversalOptimization::ValueCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_FOCAL_LENGTH>::value),
							UniversalOptimization::ErrorCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_FOCAL_LENGTH>::error),
							UniversalOptimization::SharedModelIsValidCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_FOCAL_LENGTH>::sharedModelIsValid),
							UniversalOptimization::SharedModelTransformationCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_FOCAL_LENGTH>::transformSharedModel),
							UniversalOptimization::IndividualModelTransformationCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_FOCAL_LENGTH>::transformIndividualModel),
							UniversalOptimization::ModelAcceptedCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_FOCAL_LENGTH>::acceptModel),
							optimizedSharedModel, optimizedIndividualModels, iterations, estimator, lambda, lambdaFactor, initialError, finalError, intermediateErrors))
				return false;

			optimizedCamera = data.model2profile(optimizedSharedModel);
			break;
		}

		case PinholeCamera::OS_FOCAL_LENGTHS:
		{
			// we have shared model with 4 parameters

			typedef NonLinearUniversalOptimizationSparse::SharedModelIndividualModels<PinholeCamera::OS_FOCAL_LENGTHS, 3u, 2u, PinholeCamera::OS_FOCAL_LENGTHS, 16u> UniversalOptimization;

			UniversalOptimization::SharedModel sharedModel = CameraOrientationsData<PinholeCamera::OS_FOCAL_LENGTHS>::profile2model(pinholeCamera);
			UniversalOptimization::SharedModel optimizedSharedModel;

			CameraOrientationsData<PinholeCamera::OS_FOCAL_LENGTHS> data(pinholeCamera, correspondenceGroups, onlyFrontObjectPoints);

			if (!UniversalOptimization::optimizeUniversalModel(sharedModel, individualModels, numberElementsPerIndividualModel.data(),
							UniversalOptimization::ValueCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_FOCAL_LENGTHS>::value),
							UniversalOptimization::ErrorCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_FOCAL_LENGTHS>::error),
							UniversalOptimization::SharedModelIsValidCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_FOCAL_LENGTHS>::sharedModelIsValid),
							UniversalOptimization::SharedModelTransformationCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_FOCAL_LENGTHS>::transformSharedModel),
							UniversalOptimization::IndividualModelTransformationCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_FOCAL_LENGTHS>::transformIndividualModel),
							UniversalOptimization::ModelAcceptedCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_FOCAL_LENGTHS>::acceptModel),
							optimizedSharedModel, optimizedIndividualModels, iterations, estimator, lambda, lambdaFactor, initialError, finalError, intermediateErrors))
				return false;

			optimizedCamera = data.model2profile(optimizedSharedModel);
			break;
		}

		case PinholeCamera::OS_INTRINSIC_PARAMETERS:
		{
			// we have shared model with 4 parameters

			typedef NonLinearUniversalOptimizationSparse::SharedModelIndividualModels<PinholeCamera::OS_INTRINSIC_PARAMETERS, 3u, 2u, PinholeCamera::OS_INTRINSIC_PARAMETERS, 16u> UniversalOptimization;

			UniversalOptimization::SharedModel sharedModel = CameraOrientationsData<PinholeCamera::OS_INTRINSIC_PARAMETERS>::profile2model(pinholeCamera);
			UniversalOptimization::SharedModel optimizedSharedModel;

			CameraOrientationsData<PinholeCamera::OS_INTRINSIC_PARAMETERS> data(pinholeCamera, correspondenceGroups, onlyFrontObjectPoints);

			if (!UniversalOptimization::optimizeUniversalModel(sharedModel, individualModels, numberElementsPerIndividualModel.data(),
							UniversalOptimization::ValueCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_INTRINSIC_PARAMETERS>::value),
							UniversalOptimization::ErrorCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_INTRINSIC_PARAMETERS>::error),
							UniversalOptimization::SharedModelIsValidCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_INTRINSIC_PARAMETERS>::sharedModelIsValid),
							UniversalOptimization::SharedModelTransformationCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_INTRINSIC_PARAMETERS>::transformSharedModel),
							UniversalOptimization::IndividualModelTransformationCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_INTRINSIC_PARAMETERS>::transformIndividualModel),
							UniversalOptimization::ModelAcceptedCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_INTRINSIC_PARAMETERS>::acceptModel),
							optimizedSharedModel, optimizedIndividualModels, iterations, estimator, lambda, lambdaFactor, initialError, finalError, intermediateErrors))
				return false;

			optimizedCamera = data.model2profile(optimizedSharedModel);
			break;
		}

		case PinholeCamera::OS_FOCAL_LENGTHS_DISTORTION:
		{
			// we have shared model with 6 parameters

			typedef NonLinearUniversalOptimizationSparse::SharedModelIndividualModels<PinholeCamera::OS_FOCAL_LENGTHS_DISTORTION, 3u, 2u, PinholeCamera::OS_FOCAL_LENGTHS_DISTORTION, 16u> UniversalOptimization;

			UniversalOptimization::SharedModel sharedModel = CameraOrientationsData<PinholeCamera::OS_FOCAL_LENGTHS_DISTORTION>::profile2model(pinholeCamera);
			UniversalOptimization::SharedModel optimizedSharedModel;

			CameraOrientationsData<PinholeCamera::OS_FOCAL_LENGTHS_DISTORTION> data(pinholeCamera, correspondenceGroups, onlyFrontObjectPoints);

			if (!UniversalOptimization::optimizeUniversalModel(sharedModel, individualModels, numberElementsPerIndividualModel.data(),
							UniversalOptimization::ValueCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_FOCAL_LENGTHS_DISTORTION>::value),
							UniversalOptimization::ErrorCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_FOCAL_LENGTHS_DISTORTION>::error),
							UniversalOptimization::SharedModelIsValidCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_FOCAL_LENGTHS_DISTORTION>::sharedModelIsValid),
							UniversalOptimization::SharedModelTransformationCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_FOCAL_LENGTHS_DISTORTION>::transformSharedModel),
							UniversalOptimization::IndividualModelTransformationCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_FOCAL_LENGTHS_DISTORTION>::transformIndividualModel),
							UniversalOptimization::ModelAcceptedCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_FOCAL_LENGTHS_DISTORTION>::acceptModel),
							optimizedSharedModel, optimizedIndividualModels, iterations, estimator, lambda, lambdaFactor, initialError, finalError, intermediateErrors))
				return false;

			optimizedCamera = data.model2profile(optimizedSharedModel);
			break;
		}

		case PinholeCamera::OS_SYMMETRIC_INTRINSIC_PARAMETERS_DISTORTIONS:
		{
			// we have shared model with 7 parameters

			typedef NonLinearUniversalOptimizationSparse::SharedModelIndividualModels<PinholeCamera::OS_SYMMETRIC_INTRINSIC_PARAMETERS_DISTORTIONS, 3u, 2u, PinholeCamera::OS_SYMMETRIC_INTRINSIC_PARAMETERS_DISTORTIONS, 16u> UniversalOptimization;

			UniversalOptimization::SharedModel sharedModel = CameraOrientationsData<PinholeCamera::OS_SYMMETRIC_INTRINSIC_PARAMETERS_DISTORTIONS>::profile2model(pinholeCamera);
			UniversalOptimization::SharedModel optimizedSharedModel;

			CameraOrientationsData<PinholeCamera::OS_SYMMETRIC_INTRINSIC_PARAMETERS_DISTORTIONS> data(pinholeCamera, correspondenceGroups, onlyFrontObjectPoints);

			if (!UniversalOptimization::optimizeUniversalModel(sharedModel, individualModels, numberElementsPerIndividualModel.data(),
							UniversalOptimization::ValueCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_SYMMETRIC_INTRINSIC_PARAMETERS_DISTORTIONS>::value),
							UniversalOptimization::ErrorCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_SYMMETRIC_INTRINSIC_PARAMETERS_DISTORTIONS>::error),
							UniversalOptimization::SharedModelIsValidCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_SYMMETRIC_INTRINSIC_PARAMETERS_DISTORTIONS>::sharedModelIsValid),
							UniversalOptimization::SharedModelTransformationCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_SYMMETRIC_INTRINSIC_PARAMETERS_DISTORTIONS>::transformSharedModel),
							UniversalOptimization::IndividualModelTransformationCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_SYMMETRIC_INTRINSIC_PARAMETERS_DISTORTIONS>::transformIndividualModel),
							UniversalOptimization::ModelAcceptedCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_SYMMETRIC_INTRINSIC_PARAMETERS_DISTORTIONS>::acceptModel),
							optimizedSharedModel, optimizedIndividualModels, iterations, estimator, lambda, lambdaFactor, initialError, finalError, intermediateErrors))
				return false;

			optimizedCamera = data.model2profile(optimizedSharedModel);
			break;
		}

		case PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS:
		{
			// we have shared model with 8 parameters

			typedef NonLinearUniversalOptimizationSparse::SharedModelIndividualModels<PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS, 3u, 2u, PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS, 16u> UniversalOptimization;

			UniversalOptimization::SharedModel sharedModel = CameraOrientationsData<PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS>::profile2model(pinholeCamera);
			UniversalOptimization::SharedModel optimizedSharedModel;

			CameraOrientationsData<PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS> data(pinholeCamera, correspondenceGroups, onlyFrontObjectPoints);

			if (!UniversalOptimization::optimizeUniversalModel(sharedModel, individualModels, numberElementsPerIndividualModel.data(),
							UniversalOptimization::ValueCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS>::value),
							UniversalOptimization::ErrorCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS>::error),
							UniversalOptimization::SharedModelIsValidCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS>::sharedModelIsValid),
							UniversalOptimization::SharedModelTransformationCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS>::transformSharedModel),
							UniversalOptimization::IndividualModelTransformationCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS>::transformIndividualModel),
							UniversalOptimization::ModelAcceptedCallback::create(data, &CameraOrientationsData<PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS>::acceptModel),
							optimizedSharedModel, optimizedIndividualModels, iterations, estimator, lambda, lambdaFactor, initialError, finalError, intermediateErrors))
				return false;

			optimizedCamera = data.model2profile(optimizedSharedModel);
			break;
		}

		default:
			ocean_assert(false && "Invalid optimization strategy!");
			return false;
	}

	if (optimizedOrientations)
	{
		ocean_assert(optimizedOrientations->size() == orientations.size());

		for (size_t i = 0; i < optimizedIndividualModels.size(); ++i)
		{
			const IndividualModel& individualModel = optimizedIndividualModels[i];

			const ExponentialMap exponentialMap(individualModel[0], individualModel[1], individualModel[2]);
			(*optimizedOrientations)[i] = SquareMatrix3(exponentialMap.rotation());
		}
	}

	return true;
}

/**
 * This class implements an optimization provider for a camera and several 6DOF poses.
 * The optimization needs several point correspondences between 3D object points and 2D image points in several individual camera frames.<br>
 * The provider optimizes the camera and pose parameters by minimizing the projection error between 3D object and 2D image points.<br>
 */
class NonLinearOptimizationCamera::CameraPosesOptimizationProvider : public NonLinearOptimization::OptimizationProvider
{
	public:

		/**
		 * Creates a new optimization provider object.
		 * @param pinholeCamera The camera object to be optimized
		 * @param posesIF Inverted and flipped extrinsic camera matrix
		 * @param objectPointGroups Groups of 3D object points
		 * @param imagePointGroups Groups of 2D observation image points
		 * @param onlyFrontObjectPoints True, to allow only object points in front of the camera
		 */
		inline CameraPosesOptimizationProvider(PinholeCamera& pinholeCamera, NonconstTemplateArrayAccessor<HomogenousMatrix4>& posesIF, const ConstIndexedAccessor<Vectors3>& objectPointGroups, const ConstIndexedAccessor<Vectors2>& imagePointGroups, const bool onlyFrontObjectPoints) :
			camera_(pinholeCamera),
			candidateCamera_(pinholeCamera),
			flippedCamera_T_world_(posesIF),
			objectPointGroups_(objectPointGroups),
			candidateFlippedCamera_T_world_(Accessor::accessor2elements(posesIF)),
			imagePointGroups_(imagePointGroups),
			onlyFrontObjectPoints_(onlyFrontObjectPoints),
			observations_(0)
		{
			ocean_assert(flippedCamera_T_world_.size() == objectPointGroups_.size());
			ocean_assert(objectPointGroups_.size() == imagePointGroups_.size());

			for (size_t n = 0; n < objectPointGroups_.size(); ++n)
			{
				ocean_assert(objectPointGroups_[n].size() == imagePointGroups_[n].size());
				observations_ += objectPointGroups_[n].size();
			}
		};

		/**
		 * Returns whether this provider comes with an own equation solver.
		 * @return True, as this provider has an own solver
		 */
		inline bool hasSolver() const
		{
			// **NOTE** we do not implement our own solver based on the Schur complement as the performance does not seem to be better (e.g., because of the overhead due to the creation for the sub-matrices)
			// however, the performance should be improved further by application of AdvancedSparseOptimizationProvider instead of the default Provider

			return false;
		}

		/**
		 * Determines the jacobian matrix for the current camera.
		 * @param jacobian Jacobian matrix
		 */
		void determineJacobian(SparseMatrix& jacobian) const
		{
			ocean_assert(observations_ != 0);

			SparseMatrix::Entries jacobianEntries;
			jacobianEntries.reserve(observations_ * 2 * 14); // in each row are at most 14 non-zero elements

			Scalar jacobianX[14], jacobianY[14];
			size_t row = 0;

			for (size_t p = 0; p < flippedCamera_T_world_.size(); ++p)
			{
				const HomogenousMatrix4& transformationPoseIF = flippedCamera_T_world_[p];
				const Vectors3& objectPoints = objectPointGroups_[p];

				const Pose poseIF(transformationPoseIF);

				SquareMatrix3 Rwx, Rwy, Rwz;
				Jacobian::calculateRotationRodriguesDerivative(ExponentialMap(Vector3(poseIF.rx(), poseIF.ry(), poseIF.rz())), Rwx, Rwy, Rwz);

				for (size_t i = 0; i < objectPoints.size(); ++i)
				{
					Jacobian::calculateJacobianCameraPoseRodrigues2x14(jacobianX, jacobianY, camera_, transformationPoseIF, poseIF, objectPoints[i], Rwx, Rwy, Rwz);

					for (size_t e = 0u; e < 8u; ++e)
					{
						// .insert(row + 0, 0, jacobianX, 8);
						jacobianEntries.push_back(SparseMatrix::Entry(row + 0, e, jacobianX[e]));
						// .insert(row + 1, 0, jacobianY, 8);
						jacobianEntries.push_back(SparseMatrix::Entry(row + 1, e, jacobianY[e]));
					}

					for (size_t e = 0u; e < 6; ++e)
					{
						// .insert(row + 0, 8 + p * 6, jacobianX + 8, 6);
						jacobianEntries.push_back(SparseMatrix::Entry(row + 0, 8 + p * 6 + e, jacobianX[8 + e]));
						// .insert(row + 1, 8 + p * 6, jacobianY + 8, 6);
						jacobianEntries.push_back(SparseMatrix::Entry(row + 1, 8 + p * 6 + e, jacobianY[8 + e]));
					}

					row += 2;
				}
			}

			ocean_assert(row == observations_ * 2);

			jacobian = SparseMatrix(2 * observations_, 8 + flippedCamera_T_world_.size() * 6, jacobianEntries);
			ocean_assert(SparseMatrix::Entry::hasOneEntry(jacobian.rows(), jacobian.columns(), jacobianEntries));
		}

		/**
		 * Applies the camera correction and stores the new camera as candidate
		 * @param deltas Optimization deltas that define the correction
		 */
		inline void applyCorrection(const Matrix& deltas)
		{
			const Scalar& deltaK1 = deltas(0);
			const Scalar& deltaK2 = deltas(1);
			const Scalar& deltaP1 = deltas(2);
			const Scalar& deltaP2 = deltas(3);
			const Scalar& deltaFx = deltas(4);
			const Scalar& deltaFy = deltas(5);
			const Scalar& deltaMx = deltas(6);
			const Scalar& deltaMy = deltas(7);

			const Scalar newK1 = camera_.radialDistortion().first - deltaK1;
			const Scalar newK2 = camera_.radialDistortion().second - deltaK2;
			const Scalar newP1 = camera_.tangentialDistortion().first - deltaP1;
			const Scalar newP2 = camera_.tangentialDistortion().second - deltaP2;
			const Scalar newFx = camera_.focalLengthX() - deltaFx;
			const Scalar newFy = camera_.focalLengthY() - deltaFy;
			const Scalar newMx = camera_.principalPointX() - deltaMx;
			const Scalar newMy = camera_.principalPointY() - deltaMy;

			candidateCamera_ = PinholeCamera(SquareMatrix3(newFx, 0, 0, 0, newFy, 0, newMx, newMy, 1), camera_.width(), camera_.height(), PinholeCamera::DistortionPair(newK1, newK2), PinholeCamera::DistortionPair(newP1, newP2));

			for (size_t n = 0; n < flippedCamera_T_world_.size(); ++n)
			{
				const Pose pose(flippedCamera_T_world_[n]);

				const Pose deltaPose(deltas(8 + n * 6 + 3), deltas(8 + n * 6 + 4), deltas(8 + n * 6 + 5), deltas(8 + n * 6 + 0), deltas(8 + n * 6 + 1), deltas(8 + n * 6 + 2));
				const Pose newPose(pose - deltaPose);

				candidateFlippedCamera_T_world_[n] = newPose.transformation();
			}
		}

		/**
		 * Determines the robust error of the current candidate pose.
		 * @param weightedErrorVector Resulting (weighted - if using a robust estimator) error vector
		 * @param weightVector Vector holding the weights that have already been applied to the error vector
		 * @param invertedCovariances Not supported
		 * @return The robust error
		 */
		template <Estimator::EstimatorType tEstimator>
		Scalar determineRobustError(Matrix& weightedErrorVector, Matrix& weightVector, const Matrix* invertedCovariances) const
		{
			ocean_assert(invertedCovariances == nullptr);
			ocean_assert(observations_ != 0);

			// set the correct size of the resulting error vector
			weightedErrorVector.resize(observations_ * 2u, 1u);

			Vector2* const weightedErrors = (Vector2*)weightedErrorVector.data();
			const SquareMatrix2* transposedInvertedCovariances = invertedCovariances ? (SquareMatrix2*)invertedCovariances->data() : nullptr;

			Scalars sqrErrors;
			if constexpr (!Estimator::isStandardEstimator<tEstimator>())
			{
				sqrErrors.reserve(observations_);
			}

			size_t row = 0;
			Scalar sqrError = 0;

			for (size_t p = 0; p < flippedCamera_T_world_.size(); ++p)
			{
				const HomogenousMatrix4& poseIF = candidateFlippedCamera_T_world_[p];
				const Vectors3& objectPoints = objectPointGroups_[p];
				const Vectors2& imagePoints = imagePointGroups_[p];

				for (size_t i = 0; i < objectPoints.size(); ++i)
				{
					const Vector3& objectPoint = objectPoints[i];

					if (onlyFrontObjectPoints_ && !PinholeCamera::isObjectPointInFrontIF(poseIF, objectPoint))
					{
						return Numeric::maxValue();
					}

					const Vector2 error = Error::determinePoseErrorIF(poseIF, candidateCamera_, objectPoint, imagePoints[i], true);
					weightedErrors[row++] = error;

					if constexpr (Estimator::isStandardEstimator<tEstimator>())
					{
						sqrError += error.sqr();
					}
					else
					{
						ocean_assert(!Estimator::isStandardEstimator<tEstimator>());
						sqrErrors.emplace_back(error.sqr());
					}
				}
			}

			ocean_assert(row * 2u == weightedErrorVector.rows());

			// check whether the standard estimator is used
			if constexpr (Estimator::isStandardEstimator<tEstimator>())
			{
				// the weight vector should be and should stay invalid
				ocean_assert(!weightVector);

				return sqrError / Scalar(observations_);
			}
			else
			{
				// now we need the weight vector
				weightVector.resize(observations_ * 2, 1);

				ocean_assert(sqrErrors.size() == observations_);

				// the model size is the number of columns in the jacobian row as so many parameters will be modified
				return sqrErrors2robustErrors2<tEstimator>(sqrErrors, 8 + flippedCamera_T_world_.size() * 6, weightedErrors, (Vector2*)(weightVector.data()), transposedInvertedCovariances);
			}
		}

		/**
		 * Accepts the current pose candidate as better model.
		 */
		inline void acceptCorrection()
		{
			camera_ = candidateCamera_;

			ocean_assert(flippedCamera_T_world_.size() == candidateFlippedCamera_T_world_.size());
			for (size_t n = 0; n < flippedCamera_T_world_.size(); ++n)
			{
				flippedCamera_T_world_[n] = candidateFlippedCamera_T_world_[n];
			}
		}

	protected:

		/// The camera object that will be optimized.
		PinholeCamera& camera_;

		/// The camera object that stores the most recent optimization result as candidate.
		PinholeCamera candidateCamera_;

		/// The accessor for all camera poses.
		NonconstTemplateArrayAccessor<HomogenousMatrix4>& flippedCamera_T_world_;

		/// The accessor for all object point groups.
		const ConstIndexedAccessor<Vectors3>& objectPointGroups_;

		/// The candidates of new camera poses.
		HomogenousMatrices4 candidateFlippedCamera_T_world_;

		/// The accessor for all image point groups.
		const ConstIndexedAccessor<Vectors2>& imagePointGroups_;

		/// True, forces the object point to stay in front of the camera.s
		const bool onlyFrontObjectPoints_;

		// The entire number of observations.
		size_t observations_;
};

bool NonLinearOptimizationCamera::optimizeCameraPoses(const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<HomogenousMatrix4>& poses, const ConstIndexedAccessor<Vectors3>& objectPointGroups, const ConstIndexedAccessor<Vectors2>& imagePointGroups, PinholeCamera& optimizedCamera, NonconstIndexedAccessor<HomogenousMatrix4>* optimizedPoses, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
	const ScopedConstMemoryAccessor<HomogenousMatrix4> scopedPoses(poses);

	HomogenousMatrices4 posesIF(scopedPoses.size());
	for (size_t n = 0; n < posesIF.size(); ++n)
	{
		posesIF[n] = PinholeCamera::standard2InvertedFlipped(scopedPoses[n]);
	}

	HomogenousMatrices4 optimizedPosesIF;
	NonconstArrayAccessor<HomogenousMatrix4> optimizedPosesAccessorIF(optimizedPosesIF, optimizedPoses ? poses.size() : 0);

	if (!optimizeCameraPosesIF(pinholeCamera, ConstArrayAccessor<HomogenousMatrix4>(posesIF), objectPointGroups, imagePointGroups, optimizedCamera, optimizedPosesAccessorIF.pointer(), iterations, estimator, lambda, lambdaFactor, onlyFrontObjectPoints, initialError, finalError, intermediateErrors))
	{
		return false;
	}

	if (optimizedPoses)
	{
		for (size_t n = 0; n < optimizedPosesIF.size(); ++n)
		{
			(*optimizedPoses)[n] = PinholeCamera::invertedFlipped2Standard(optimizedPosesIF[n]);
		}
	}

	return true;
}

bool NonLinearOptimizationCamera::optimizeCameraPosesIF(const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<HomogenousMatrix4>& posesIF, const ConstIndexedAccessor<Vectors3>& objectPointGroups, const ConstIndexedAccessor<Vectors2>& imagePointGroups, PinholeCamera& optimizedCamera, NonconstIndexedAccessor<HomogenousMatrix4>* optimizedPosesIF, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
	ocean_assert(&pinholeCamera != &optimizedCamera);
	ocean_assert(objectPointGroups.size() == imagePointGroups.size());

	optimizedCamera = pinholeCamera;

	// we need enough buffer for the optimized poses, we take them from the provided parameter or create them temporary in this scope
	ScopedNonconstMemoryAccessor<HomogenousMatrix4> scopedOptimizedPosesIF(optimizedPosesIF, posesIF.size());
	ocean_assert(scopedOptimizedPosesIF.size() == posesIF.size());

	const ScopedConstMemoryAccessor<HomogenousMatrix4> scopedPosesIF(posesIF);
	for (size_t n = 0; n < posesIF.size(); ++n)
	{
		scopedOptimizedPosesIF[n] = scopedPosesIF[n];
	}

	NonconstTemplateArrayAccessor<HomogenousMatrix4> posesIFAccessor(scopedOptimizedPosesIF.data(), scopedOptimizedPosesIF.size());

	CameraPosesOptimizationProvider provider(optimizedCamera, posesIFAccessor, objectPointGroups, imagePointGroups, onlyFrontObjectPoints);
	return sparseOptimization<CameraPosesOptimizationProvider>(provider, iterations, estimator, lambda, lambdaFactor, initialError, finalError, nullptr, intermediateErrors);
}

/**
 * This class implements a data object allowing to optimize the camera profile, 3D object points locations and 6-DOF camera poses concurrently.
 * This data object allows to optimized 3D object points observed in individual camera pose (while each camera pose may observe an arbitrary number of object points).<br>
 */
template <unsigned int tParameters>
class NonLinearOptimizationCamera::CameraObjectPointsPosesData : public CameraProfileBaseData<tParameters>
{
	public:

		/**
		 * Creates a new data object.
		 * @param pinholeCamera The camera profile defining the projection
		 * @param correspondenceGroups The groups of correspondences between pose indices and image points, one group for each object point
		 * @param onlyFrontObjectPoints True, to avoid that an optimized 3D position lies behind any camera in which the object point is visible
		 */
		CameraObjectPointsPosesData(const PinholeCamera& pinholeCamera, const ObjectPointGroupsAccessor& correspondenceGroups, const bool onlyFrontObjectPoints) :
			CameraProfileBaseData<tParameters>(pinholeCamera),
			correspondenceGroups_(correspondenceGroups),
			onlyFrontObjectPoints_(onlyFrontObjectPoints)
		{
#ifdef OCEAN_DEBUG
			ocean_assert(correspondenceGroups.groups() != 0);
			for (size_t g = 0; g < correspondenceGroups.groups(); ++g)
			{
				ocean_assert(correspondenceGroups.groupElements(g) != 0);
			}
#endif
		}

		/**
		 * Determines the value for a specified 3D object point and specified element (pair of camera pose and image point location).
		 * @param externalSharedModel The shared model defining the camera profile
		 * @param externalFirstModels The current first models to be applied (the poses of the individual camera frames)
		 * @param externalSecondModels The current second model to be applied (the locations of the individual 3D object points)
		 * @param secondModelIndex The index of the second model (the index of the 3D object point)
		 * @param elementIndex The index of the correspondence between pose id and image point visible in that pose
		 * @param result The resulting value which is the projected 3D object points
		 * @return The index of the pose for the specified element index
		 */
		size_t value(const StaticBuffer<Scalar, tParameters>& externalSharedModel, const std::vector< StaticBuffer<Scalar, 16> >& externalFirstModels, const std::vector< StaticBuffer<Scalar, 3> >& externalSecondModels, const size_t secondModelIndex, const size_t elementIndex, StaticBuffer<Scalar, 2>& result)
		{
			const PinholeCamera pinholeCamera(CameraProfileBaseData<tParameters>::model2profile(externalSharedModel));

			const Index32 poseIndex = correspondenceGroups_.firstElement(secondModelIndex, elementIndex);
			ocean_assert(poseIndex < externalFirstModels.size());

			const HomogenousMatrix4& poseIF = *((HomogenousMatrix4*)(externalFirstModels[poseIndex].data()));
			const Vector3& objectPoint = *((Vector3*)(externalSecondModels[secondModelIndex].data()));

			const Vector2 point = pinholeCamera.projectToImageIF<true>(poseIF, objectPoint, pinholeCamera.hasDistortionParameters());

			result[0] = point[0];
			result[1] = point[1];

			return size_t(poseIndex);
		}

		/**
		 * Determines the error between a projected 3D object point and the expected point for the given models.
		 * @param externalSharedModel The shared model defining the camera profile
		 * @param externalFirstModels The current first models to be applied (the poses of the individual camera frames)
		 * @param externalSecondModels The current second model to be applied (the locations of the individual 3D object points)
		 * @param secondModelIndex The index of the second model (the index of the 3D object point)
		 * @param elementIndex The index of the correspondence between pose id and image point visible in that pose
		 * @param result The resulting projection error
		 * @return True, if succeeded
		 */
		bool error(const StaticBuffer<Scalar, tParameters>& externalSharedModel, const std::vector< StaticBuffer<Scalar, 16> >& externalFirstModels, const std::vector< StaticBuffer<Scalar, 3> >& externalSecondModels, const size_t secondModelIndex, const size_t elementIndex, StaticBuffer<Scalar, 2>& result)
		{
			const PinholeCamera pinholeCamera(CameraProfileBaseData<tParameters>::model2profile(externalSharedModel));

			Index32 poseIndex;
			Vector2 imagePoint;

			correspondenceGroups_.element(secondModelIndex, elementIndex, poseIndex, imagePoint);
			ocean_assert(poseIndex < externalFirstModels.size());

			const HomogenousMatrix4& poseIF = *((HomogenousMatrix4*)(externalFirstModels[poseIndex].data()));
			const Vector3& objectPoint = *((Vector3*)(externalSecondModels[secondModelIndex].data()));

			// ensure that the object point lies in front of the camera
			if (onlyFrontObjectPoints_ && !PinholeCamera::isObjectPointInFrontIF(poseIF, objectPoint))
			{
				return false;
			}

			const Vector2 point = pinholeCamera.projectToImageIF<true>(poseIF, objectPoint, pinholeCamera.hasDistortionParameters());
			const Vector2 error(point - imagePoint);

			result[0] = error[0];
			result[1] = error[1];

			return true;
		}

		/**
		 * Transforms the internal shared model to the external shared model.
		 * @param internalModel The internal model
		 * @param externalModel The external model
		 */
		void transformSharedModel(StaticBuffer<Scalar, tParameters>& internalModel, StaticBuffer<Scalar, tParameters>& externalModel)
		{
			externalModel = internalModel;
		}

		/**
		 * Transforms an internal first model to the external model.
		 * @param internalModel The internal model, which is the 6-DOF pose represented by 6 scalar parameters (translation and exponential map)
		 * @param externalModel The external model, which is the 6-DOF inverted and flipped pose represented by 16 scalar parameters (4x4 transformation matrix)
		 */
		void transformFirstIndividualModel(StaticBuffer<Scalar, 6>& internalModel, StaticBuffer<Scalar, 16>& externalModel)
		{
			*((HomogenousMatrix4*)externalModel.data()) = PinholeCamera::standard2InvertedFlipped(Pose(internalModel.data()).transformation());
		}

		/**
		 * Transforms an internal second model to the external model.
		 * @param internalModel The internal model
		 * @param externalModel The external model
		 */
		void transformSecondIndividualModel(StaticBuffer<Scalar, 3>& internalModel, StaticBuffer<Scalar, 3>& externalModel)
		{
			externalModel = internalModel;
		}

	protected:

		/// The groups of correspondences between pose indices and image points, one group for each object point.
		const ObjectPointGroupsAccessor& correspondenceGroups_;

		/// True, to avoid that an optimized 3D position lies behind any camera in which the object point is visible.
		const bool onlyFrontObjectPoints_;
};

bool NonLinearOptimizationCamera::optimizeCameraObjectPointsPoses(const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<HomogenousMatrix4>& poses, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, const PinholeCamera::OptimizationStrategy optimizationStrategy, PinholeCamera& optimizedCamera, NonconstIndexedAccessor<HomogenousMatrix4>* optimizedPoses, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator, const Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(objectPoints.size() == correspondenceGroups.groups());
	ocean_assert(objectPoints.size() >= 1);

	ocean_assert(optimizedPoses == nullptr || optimizedPoses->size() == poses.size());
	ocean_assert(optimizedObjectPoints == nullptr || optimizedObjectPoints->size() == objectPoints.size());

	// shared model: camera profile with 4, 6 or 8 scalar parameters
	// individual model: camera orientation with 3 scalar parameters for each orientation

	typedef StaticBuffer<Scalar, 6> FirstIndividualModel;
	typedef std::vector<FirstIndividualModel> FirstIndividualModels;

	FirstIndividualModels firstIndividualModels, optimizedFirstIndividualModels;
	for (size_t n = 0; n < poses.size(); ++n)
	{
		const Pose pose(poses[n]);
		firstIndividualModels.push_back(FirstIndividualModel(pose.data()));
	}

	typedef StaticBuffer<Scalar, 3> SecondIndividualModel;
	typedef std::vector<SecondIndividualModel> SecondIndividualModels;

	SecondIndividualModels secondIndividualModels, optimizedSecondIndividualModels;
	for (size_t n = 0; n < objectPoints.size(); ++n)
	{
		secondIndividualModels.push_back(SecondIndividualModel(objectPoints[n].data()));
	}

	std::vector<size_t> numberElementsPerIndividualModel;
	numberElementsPerIndividualModel.reserve(correspondenceGroups.groups());

	for (size_t i = 0; i < correspondenceGroups.groups(); ++i)
	{
		ocean_assert(correspondenceGroups.groupElements(i) != 0);
		numberElementsPerIndividualModel.push_back(correspondenceGroups.groupElements(i));
	}

	switch (optimizationStrategy)
	{
		case PinholeCamera::OS_FOCAL_LENGTH:
		{
			// we have shared model with 1 parameters

			typedef NonLinearUniversalOptimizationSparse::SharedModelIndividualModelsIndividualModels<PinholeCamera::OS_FOCAL_LENGTH, 6u, 3u, 2u, PinholeCamera::OS_FOCAL_LENGTH, 16u, 3u> UniversalOptimization;

			UniversalOptimization::SharedModel sharedModel = CameraObjectPointsPosesData<PinholeCamera::OS_FOCAL_LENGTH>::profile2model(pinholeCamera);
			UniversalOptimization::SharedModel optimizedSharedModel;

			CameraObjectPointsPosesData<PinholeCamera::OS_FOCAL_LENGTH> data(pinholeCamera, correspondenceGroups, onlyFrontObjectPoints);

			if (!UniversalOptimization::optimizeUniversalModel(sharedModel, firstIndividualModels, secondIndividualModels, numberElementsPerIndividualModel.data(),
							UniversalOptimization::ValueCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_FOCAL_LENGTH>::value),
							UniversalOptimization::ErrorCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_FOCAL_LENGTH>::error),
							UniversalOptimization::SharedModelIsValidCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_FOCAL_LENGTH>::sharedModelIsValid),
							UniversalOptimization::SharedModelTransformationCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_FOCAL_LENGTH>::transformSharedModel),
							UniversalOptimization::FirstIndividualModelTransformationCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_FOCAL_LENGTH>::transformFirstIndividualModel),
							UniversalOptimization::SecondIndividualModelTransformationCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_FOCAL_LENGTH>::transformSecondIndividualModel),
							UniversalOptimization::ModelAcceptedCallback(),
							optimizedSharedModel, optimizedFirstIndividualModels, optimizedSecondIndividualModels, iterations, estimator, lambda, lambdaFactor, initialError, finalError, intermediateErrors))
				return false;

			optimizedCamera = data.model2profile(optimizedSharedModel);
			break;
		}

		case PinholeCamera::OS_FOCAL_LENGTHS:
		{
			// we have shared model with 2 parameters

			typedef NonLinearUniversalOptimizationSparse::SharedModelIndividualModelsIndividualModels<PinholeCamera::OS_FOCAL_LENGTHS, 6u, 3u, 2u, PinholeCamera::OS_FOCAL_LENGTHS, 16u, 3u> UniversalOptimization;

			UniversalOptimization::SharedModel sharedModel = CameraObjectPointsPosesData<PinholeCamera::OS_FOCAL_LENGTHS>::profile2model(pinholeCamera);
			UniversalOptimization::SharedModel optimizedSharedModel;

			CameraObjectPointsPosesData<PinholeCamera::OS_FOCAL_LENGTHS> data(pinholeCamera, correspondenceGroups, onlyFrontObjectPoints);

			if (!UniversalOptimization::optimizeUniversalModel(sharedModel, firstIndividualModels, secondIndividualModels, numberElementsPerIndividualModel.data(),
							UniversalOptimization::ValueCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_FOCAL_LENGTHS>::value),
							UniversalOptimization::ErrorCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_FOCAL_LENGTHS>::error),
							UniversalOptimization::SharedModelIsValidCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_FOCAL_LENGTHS>::sharedModelIsValid),
							UniversalOptimization::SharedModelTransformationCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_FOCAL_LENGTHS>::transformSharedModel),
							UniversalOptimization::FirstIndividualModelTransformationCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_FOCAL_LENGTHS>::transformFirstIndividualModel),
							UniversalOptimization::SecondIndividualModelTransformationCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_FOCAL_LENGTHS>::transformSecondIndividualModel),
							UniversalOptimization::ModelAcceptedCallback(),
							optimizedSharedModel, optimizedFirstIndividualModels, optimizedSecondIndividualModels, iterations, estimator, lambda, lambdaFactor, initialError, finalError, intermediateErrors))
				return false;

			optimizedCamera = data.model2profile(optimizedSharedModel);
			break;
		}

		case PinholeCamera::OS_INTRINSIC_PARAMETERS:
		{
			// we have shared model with 4 parameters

			typedef NonLinearUniversalOptimizationSparse::SharedModelIndividualModelsIndividualModels<PinholeCamera::OS_INTRINSIC_PARAMETERS, 6u, 3u, 2u, PinholeCamera::OS_INTRINSIC_PARAMETERS, 16u, 3u> UniversalOptimization;

			UniversalOptimization::SharedModel sharedModel = CameraObjectPointsPosesData<PinholeCamera::OS_INTRINSIC_PARAMETERS>::profile2model(pinholeCamera);
			UniversalOptimization::SharedModel optimizedSharedModel;

			CameraObjectPointsPosesData<PinholeCamera::OS_INTRINSIC_PARAMETERS> data(pinholeCamera, correspondenceGroups, onlyFrontObjectPoints);

			if (!UniversalOptimization::optimizeUniversalModel(sharedModel, firstIndividualModels, secondIndividualModels, numberElementsPerIndividualModel.data(),
							UniversalOptimization::ValueCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_INTRINSIC_PARAMETERS>::value),
							UniversalOptimization::ErrorCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_INTRINSIC_PARAMETERS>::error),
							UniversalOptimization::SharedModelIsValidCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_INTRINSIC_PARAMETERS>::sharedModelIsValid),
							UniversalOptimization::SharedModelTransformationCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_INTRINSIC_PARAMETERS>::transformSharedModel),
							UniversalOptimization::FirstIndividualModelTransformationCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_INTRINSIC_PARAMETERS>::transformFirstIndividualModel),
							UniversalOptimization::SecondIndividualModelTransformationCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_INTRINSIC_PARAMETERS>::transformSecondIndividualModel),
							UniversalOptimization::ModelAcceptedCallback(),
							optimizedSharedModel, optimizedFirstIndividualModels, optimizedSecondIndividualModels, iterations, estimator, lambda, lambdaFactor, initialError, finalError, intermediateErrors))
				return false;

			optimizedCamera = data.model2profile(optimizedSharedModel);
			break;
		}

		case PinholeCamera::OS_FOCAL_LENGTHS_DISTORTION:
		{
			// we have shared model with 6 parameters

			typedef NonLinearUniversalOptimizationSparse::SharedModelIndividualModelsIndividualModels<PinholeCamera::OS_FOCAL_LENGTHS_DISTORTION, 6u, 3u, 2u, PinholeCamera::OS_FOCAL_LENGTHS_DISTORTION, 16u, 3u> UniversalOptimization;

			UniversalOptimization::SharedModel sharedModel = CameraObjectPointsPosesData<PinholeCamera::OS_FOCAL_LENGTHS_DISTORTION>::profile2model(pinholeCamera);
			UniversalOptimization::SharedModel optimizedSharedModel;

			CameraObjectPointsPosesData<PinholeCamera::OS_FOCAL_LENGTHS_DISTORTION> data(pinholeCamera, correspondenceGroups, onlyFrontObjectPoints);

			if (!UniversalOptimization::optimizeUniversalModel(sharedModel, firstIndividualModels, secondIndividualModels, numberElementsPerIndividualModel.data(),
							UniversalOptimization::ValueCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_FOCAL_LENGTHS_DISTORTION>::value),
							UniversalOptimization::ErrorCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_FOCAL_LENGTHS_DISTORTION>::error),
							UniversalOptimization::SharedModelIsValidCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_FOCAL_LENGTHS_DISTORTION>::sharedModelIsValid),
							UniversalOptimization::SharedModelTransformationCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_FOCAL_LENGTHS_DISTORTION>::transformSharedModel),
							UniversalOptimization::FirstIndividualModelTransformationCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_FOCAL_LENGTHS_DISTORTION>::transformFirstIndividualModel),
							UniversalOptimization::SecondIndividualModelTransformationCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_FOCAL_LENGTHS_DISTORTION>::transformSecondIndividualModel),
							UniversalOptimization::ModelAcceptedCallback(),
							optimizedSharedModel, optimizedFirstIndividualModels, optimizedSecondIndividualModels, iterations, estimator, lambda, lambdaFactor, initialError, finalError, intermediateErrors))
				return false;

			optimizedCamera = data.model2profile(optimizedSharedModel);
			break;
		}

		case PinholeCamera::OS_SYMMETRIC_INTRINSIC_PARAMETERS_DISTORTIONS:
		{
			// we have shared model with 6 parameters

			typedef NonLinearUniversalOptimizationSparse::SharedModelIndividualModelsIndividualModels<PinholeCamera::OS_SYMMETRIC_INTRINSIC_PARAMETERS_DISTORTIONS, 6u, 3u, 2u, PinholeCamera::OS_SYMMETRIC_INTRINSIC_PARAMETERS_DISTORTIONS, 16u, 3u> UniversalOptimization;

			UniversalOptimization::SharedModel sharedModel = CameraObjectPointsPosesData<PinholeCamera::OS_SYMMETRIC_INTRINSIC_PARAMETERS_DISTORTIONS>::profile2model(pinholeCamera);
			UniversalOptimization::SharedModel optimizedSharedModel;

			CameraObjectPointsPosesData<PinholeCamera::OS_SYMMETRIC_INTRINSIC_PARAMETERS_DISTORTIONS> data(pinholeCamera, correspondenceGroups, onlyFrontObjectPoints);

			if (!UniversalOptimization::optimizeUniversalModel(sharedModel, firstIndividualModels, secondIndividualModels, numberElementsPerIndividualModel.data(),
							UniversalOptimization::ValueCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_SYMMETRIC_INTRINSIC_PARAMETERS_DISTORTIONS>::value),
							UniversalOptimization::ErrorCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_SYMMETRIC_INTRINSIC_PARAMETERS_DISTORTIONS>::error),
							UniversalOptimization::SharedModelIsValidCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_SYMMETRIC_INTRINSIC_PARAMETERS_DISTORTIONS>::sharedModelIsValid),
							UniversalOptimization::SharedModelTransformationCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_SYMMETRIC_INTRINSIC_PARAMETERS_DISTORTIONS>::transformSharedModel),
							UniversalOptimization::FirstIndividualModelTransformationCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_SYMMETRIC_INTRINSIC_PARAMETERS_DISTORTIONS>::transformFirstIndividualModel),
							UniversalOptimization::SecondIndividualModelTransformationCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_SYMMETRIC_INTRINSIC_PARAMETERS_DISTORTIONS>::transformSecondIndividualModel),
							UniversalOptimization::ModelAcceptedCallback(),
							optimizedSharedModel, optimizedFirstIndividualModels, optimizedSecondIndividualModels, iterations, estimator, lambda, lambdaFactor, initialError, finalError, intermediateErrors))
				return false;

			optimizedCamera = data.model2profile(optimizedSharedModel);
			break;
		}

		case PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS:
		{
			// we have shared model with 8 parameters

			typedef NonLinearUniversalOptimizationSparse::SharedModelIndividualModelsIndividualModels<PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS, 6u, 3u, 2u, PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS, 16u, 3u> UniversalOptimization;

			UniversalOptimization::SharedModel sharedModel = CameraObjectPointsPosesData<PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS>::profile2model(pinholeCamera);
			UniversalOptimization::SharedModel optimizedSharedModel;

			CameraObjectPointsPosesData<PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS> data(pinholeCamera, correspondenceGroups, onlyFrontObjectPoints);

			if (!UniversalOptimization::optimizeUniversalModel(sharedModel, firstIndividualModels, secondIndividualModels, numberElementsPerIndividualModel.data(),
							UniversalOptimization::ValueCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS>::value),
							UniversalOptimization::ErrorCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS>::error),
							UniversalOptimization::SharedModelIsValidCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS>::sharedModelIsValid),
							UniversalOptimization::SharedModelTransformationCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS>::transformSharedModel),
							UniversalOptimization::FirstIndividualModelTransformationCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS>::transformFirstIndividualModel),
							UniversalOptimization::SecondIndividualModelTransformationCallback::create(data, &CameraObjectPointsPosesData<PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS>::transformSecondIndividualModel),
							UniversalOptimization::ModelAcceptedCallback(),
							optimizedSharedModel, optimizedFirstIndividualModels, optimizedSecondIndividualModels, iterations, estimator, lambda, lambdaFactor, initialError, finalError, intermediateErrors))
				return false;

			optimizedCamera = data.model2profile(optimizedSharedModel);
			break;
		}

		default:
			ocean_assert(false && "Invalid optimization strategy!");
			return false;
	}

	if (optimizedPoses)
	{
		ocean_assert(optimizedPoses->size() == poses.size());

		for (size_t i = 0; i < optimizedFirstIndividualModels.size(); ++i)
		{
			const FirstIndividualModel& firstIndividualModel = optimizedFirstIndividualModels[i];

			const Pose pose(firstIndividualModel.data());
			(*optimizedPoses)[i] = pose.transformation();
		}
	}

	if (optimizedObjectPoints)
	{
		ocean_assert(optimizedObjectPoints->size() == objectPoints.size());

		for (size_t i = 0; i < optimizedSecondIndividualModels.size(); ++i)
		{
			const SecondIndividualModel& secondIndividualModel = optimizedSecondIndividualModels[i];
			(*optimizedObjectPoints)[i] = Vector3(secondIndividualModel.data());
		}
	}

	return true;
}

void NonLinearOptimizationCamera::findInitialFieldOfViewSubset(const PinholeCamera* pinholeCamera, const ConstIndexedAccessor<SquareMatrix3>* orientations, const PoseGroupsAccessor* correspondenceGroups, PinholeCamera* optimizedCamera, SquareMatrices3* optimizedOrientations, const Scalar lowerFovX, const Scalar upperFovX, const unsigned int overallSteps, const bool onlyFrontObjectPoints, Scalar* bestError, Scalars* allErrors, Lock* lock, bool* abort, const unsigned int firstStep, const unsigned int steps)
{
	ocean_assert(pinholeCamera && pinholeCamera->isValid());
	ocean_assert(orientations);
	ocean_assert(correspondenceGroups);
	ocean_assert(optimizedCamera && bestError);
	ocean_assert(lowerFovX <= upperFovX);

	ocean_assert(firstStep + steps <= overallSteps);

	// shared model: camera profile with 1 scalar parameters
	// individual model: camera orientation with 3 scalar parameters for each orientation

	typedef NonLinearUniversalOptimizationSparse::SharedModelIndividualModels<1u, 3u, 2u, 1u, 16u> UniversalOptimization;

	UniversalOptimization::SharedModel sharedModel, optimizedSharedModel;
	UniversalOptimization::IndividualModels individualModels, optimizedIndividualModels;

	Scalar localError = Numeric::maxValue();
	PinholeCamera localCamera;
	SquareMatrices3 localOrientations;
	Scalars localAllErrors;

	for (unsigned int n = firstStep; (!abort || !*abort) && n < firstStep + steps; ++n)
	{
		const Scalar fovX = lowerFovX + Scalar(n) * (upperFovX - lowerFovX) / Scalar(overallSteps - 1u);

		ocean_assert(n != 0u || fovX == lowerFovX);
		ocean_assert(n != (overallSteps - 1u) || fovX == upperFovX);

		sharedModel[0] = PinholeCamera(pinholeCamera->width(), pinholeCamera->height(), fovX).focalLengthX();

		individualModels.clear();
		optimizedIndividualModels.clear();

		for (size_t i = 0; i < orientations->size(); ++i)
		{
			const ExponentialMap exponentialMap((*orientations)[i]);

			const UniversalOptimization::IndividualModel indiviudalModel(exponentialMap.data());
			individualModels.push_back(indiviudalModel);
		}

		std::vector<size_t> numberElementsPerIndividualModel;
		numberElementsPerIndividualModel.reserve(correspondenceGroups->groups());

		for (size_t i = 0; i < correspondenceGroups->groups(); ++i)
		{
			ocean_assert(correspondenceGroups->groupElements(i) != 0);
			numberElementsPerIndividualModel.push_back(correspondenceGroups->groupElements(i));
		}

		CameraOrientationsFovData data(*pinholeCamera, *correspondenceGroups, onlyFrontObjectPoints);

		Scalar iterationFinalError = Numeric::maxValue();
		if (UniversalOptimization::optimizeUniversalModel(sharedModel, individualModels, numberElementsPerIndividualModel.data(),
					UniversalOptimization::ValueCallback::create(data, &CameraOrientationsFovData::value),
					UniversalOptimization::ErrorCallback::create(data, &CameraOrientationsFovData::error),
					UniversalOptimization::SharedModelIsValidCallback(),
					UniversalOptimization::SharedModelTransformationCallback::create(data, &CameraOrientationsFovData::transformSharedModel),
					UniversalOptimization::IndividualModelTransformationCallback::create(data, &CameraOrientationsFovData::transformIndividualModel),
					UniversalOptimization::ModelAcceptedCallback::create(data, &CameraOrientationsFovData::acceptModel),
					optimizedSharedModel, optimizedIndividualModels, 5u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), nullptr, &iterationFinalError))
		{
			localAllErrors.push_back(iterationFinalError);

			if (iterationFinalError < localError)
			{
				localError = iterationFinalError;
				localCamera = PinholeCamera(pinholeCamera->width(), pinholeCamera->height(), optimizedSharedModel[0], optimizedSharedModel[0], pinholeCamera->principalPointX(), pinholeCamera->principalPointY(), pinholeCamera->radialDistortion(), pinholeCamera->tangentialDistortion());

				if (optimizedOrientations)
				{
					localOrientations.clear();

					for (size_t i = 0; i < optimizedIndividualModels.size(); ++i)
					{
						const UniversalOptimization::IndividualModel& individualModel = optimizedIndividualModels[i];

						const ExponentialMap exponentialMap(individualModel[0], individualModel[1], individualModel[2]);
						localOrientations.push_back(SquareMatrix3(exponentialMap.rotation()));
					}
				}
			}
		}
	}

	if (!abort || !*abort)
	{
		const OptionalScopedLock scopedLock(lock);

		if (allErrors)
		{
			allErrors->insert(allErrors->end(), localAllErrors.begin(), localAllErrors.end());
		}

		if (localError < *bestError)
		{
			*bestError = localError;
			*optimizedCamera = localCamera;

			if (optimizedOrientations)
			{
				*optimizedOrientations = std::move(localOrientations);
			}
		}
	}
}

void NonLinearOptimizationCamera::findInitialFieldOfViewSubset(const PinholeCamera* pinholeCamera, const ConstIndexedAccessor<HomogenousMatrix4>* poses, const ConstIndexedAccessor<Vector3>* objectPoints, const ObjectPointGroupsAccessor* correspondenceGroups, PinholeCamera* optimizedCamera, HomogenousMatrices4* optimizedPoses, Vectors3* optimizedObjectPoints, const Scalar lowerFovX, const Scalar upperFovX, const unsigned int overallSteps, const bool onlyFrontObjectPoints, Scalar* bestError, Scalars* allErrors, Lock* lock, bool* abort, const unsigned int firstStep, const unsigned int steps)
{
	ocean_assert(pinholeCamera && pinholeCamera->isValid());
	ocean_assert(poses && objectPoints);
	ocean_assert(correspondenceGroups);
	ocean_assert(optimizedCamera && bestError);
	ocean_assert(lowerFovX <= upperFovX);

	ocean_assert(firstStep + steps <= overallSteps);

	Scalar localBestError = Numeric::maxValue();
	PinholeCamera localBestCamera;

	HomogenousMatrices4 localBestOptimizedPoses, localIterationOptimizedPoses;
	Vectors3 localBestOptimizedObjectPoints, localIterationOptimizedObjectPoints;

	NonconstArrayAccessor<HomogenousMatrix4> localOptimizedPoseAccessor(localIterationOptimizedPoses, optimizedPoses ? poses->size() : 0);
	NonconstArrayAccessor<Vector3> localOptimizedObjectPointAccessor(localIterationOptimizedObjectPoints, optimizedObjectPoints ? objectPoints->size() : 0);

	Scalars localAllErrors;

	for (unsigned int n = firstStep; (!abort || !*abort) && n < firstStep + steps; ++n)
	{
		const Scalar fovX = lowerFovX + Scalar(n) * (upperFovX - lowerFovX) / Scalar(overallSteps - 1u);

		ocean_assert(n != 0u || Numeric::isEqual(fovX, lowerFovX));
		ocean_assert(n != (overallSteps - 1u) || Numeric::isEqual(fovX, upperFovX));

		PinholeCamera fovCamera(pinholeCamera->width(), pinholeCamera->height(), fovX, pinholeCamera->principalPointX(), pinholeCamera->principalPointY());
		fovCamera.setRadialDistortion(pinholeCamera->radialDistortion());
		fovCamera.setTangentialDistortion(pinholeCamera->tangentialDistortion());

		Scalar iterationFinalError = Numeric::maxValue();
		if (NonLinearOptimizationObjectPoint::optimizeObjectPointsAndPoses(AnyCameraPinhole(fovCamera), *poses, *objectPoints, *correspondenceGroups, localOptimizedPoseAccessor.pointer(), localOptimizedObjectPointAccessor.pointer(), 50u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), onlyFrontObjectPoints, nullptr, &iterationFinalError))
		{
			localAllErrors.push_back(iterationFinalError);

			if (iterationFinalError < localBestError)
			{
				localBestError = iterationFinalError;
				localBestCamera = fovCamera;

				if (optimizedPoses)
				{
					localBestOptimizedPoses = localIterationOptimizedPoses;
				}

				if (optimizedObjectPoints)
				{
					localBestOptimizedObjectPoints = localIterationOptimizedObjectPoints;
				}
			}
		}
	}

	if (!abort || !*abort)
	{
		const OptionalScopedLock scopedLock(lock);

		if (allErrors)
		{
			allErrors->insert(allErrors->end(), localAllErrors.begin(), localAllErrors.end());
		}

		if (localBestError < *bestError)
		{
			*bestError = localBestError;
			*optimizedCamera = localBestCamera;

			if (optimizedPoses)
			{
				*optimizedPoses = std::move(localBestOptimizedPoses);
			}

			if (optimizedObjectPoints)
			{
				*optimizedObjectPoints = std::move(localBestOptimizedObjectPoints);
			}
		}
	}
}

}

}
