/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/NonLinearOptimizationPlane.h"
#include "ocean/geometry/Error.h"
#include "ocean/geometry/NonLinearUniversalOptimizationDense.h"
#include "ocean/geometry/NonLinearUniversalOptimizationSparse.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements a data object necessary to optimize a 3D plane.
 * The internal model for the optimization is composed of two angular values defining the orientation of the plane's normal and one scalar value defining the distance of the plane to the origin.<br>
 * An euler rotation is used to model the direction of the normal (using yaw- and pitch-angle).<br>
 * The internal model has the following three scalar values: yaw-angle, pitch-angle, distance.<br>
 * The external model has the following four scalar values: normal-x, normal-y, normal-z, distance.
 */
class NonLinearOptimizationPlane::PlaneData
{
	public:

		/**
		 * Creates a new plane data object.
		 * @param objectPoints The accessor for the object points
		 */
		explicit PlaneData(const ConstIndexedAccessor<ObjectPoint>& objectPoints) :
			objectPoints_(objectPoints)
		{
			// nothing to do here
		}

		/**
		 * Determines the value for a given model and measurement.
		 * @param externalModel The external model for which a specific projected object point will be determined
		 * @param index The index of the object point for which the projected plane point will be determined
		 * @param result The resulting projected 3D object point lying on the plane
		 */
		void value(const StaticBuffer<Scalar, 4>& externalModel, const size_t index, StaticBuffer<Scalar, 3>& result)
		{
			const Vector3 projectedObjectPoint(projectOnPlane(externalModel, index));

			result[0] = projectedObjectPoint[0];
			result[1] = projectedObjectPoint[1];
			result[2] = projectedObjectPoint[2];
		}

		/**
		 * Determines the error for a given model and measurement.
		 * @param externalModel The external model for which a specific error will be determined
		 * @param index The index of the object point for which the error will be determined
		 * @param result The resulting error for each axis
		 */
		bool error(const StaticBuffer<Scalar, 4>& externalModel, const size_t index, StaticBuffer<Scalar, 3>& result)
		{
			const Vector3 projectedObjectPoint(projectOnPlane(externalModel, index));

			const Vector3& measurementObjectPoint = objectPoints_[index];
			const Vector3 error(projectedObjectPoint - measurementObjectPoint);

			result[0] = error[0];
			result[1] = error[1];
			result[2] = error[2];
			return true;
		}

		/**
		 * Transforms the internal model to a corresponding external model.
		 * @param internalModel The internal model which will be converted
		 * @param externalModel The resulting external model
		 */
		void transformModel(StaticBuffer<Scalar, 3>& internalModel, StaticBuffer<Scalar, 4>& externalModel)
		{
			const Plane3 plane(internalModel[0], internalModel[1], internalModel[2]);
			ocean_assert(plane.isValid());

			externalModel[0] = plane.normal()[0];
			externalModel[1] = plane.normal()[1];
			externalModel[2] = plane.normal()[2];
			externalModel[3] = plane.distance();
		}

	protected:

		/**
		 * Projects a 3D object point onto the 3D plane defined by the external model.
		 * @param externalModel The external model defining the plane
		 * @param index The index of the 3D object point which will be projected
		 * @return The resulting projected 3D object point
		 */
		Vector3 projectOnPlane(const StaticBuffer<Scalar, 4>& externalModel, const size_t index)
		{
			const Plane3 plane(Vector3(externalModel[0], externalModel[1], externalModel[2]), externalModel[3]);
			ocean_assert(plane.isValid());

			return plane.projectOnPlane(objectPoints_[index]);
		}

	protected:

		/// The 3D object points defining the 3D plane.
		const ConstIndexedAccessor<ObjectPoint>& objectPoints_;
};

bool NonLinearOptimizationPlane::optimizePlane(const Plane3& plane, const ConstIndexedAccessor<ObjectPoint>& pointAccessor, Plane3& optimizedPlane, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError)
{
	ocean_assert(plane.isValid() && pointAccessor.size() >= 3);

	typedef NonLinearUniversalOptimizationDense<3, 3, 4> UniversalOptimization;

	PlaneData planeData(pointAccessor);

	UniversalOptimization::Model model;
	UniversalOptimization::Model optimizedModel;

	plane.decomposeNormal(model[0], model[1]);
	model[2] = plane.distance();

	if (!UniversalOptimization::optimizeUniversalModel(model, pointAccessor.size(),
			UniversalOptimization::ValueCallback::create(planeData, &PlaneData::value),
			UniversalOptimization::ErrorCallback::create(planeData, &PlaneData::error),
			UniversalOptimization::ModelTransformationCallback::create(planeData, &PlaneData::transformModel),
			UniversalOptimization::ModelAdjustmentCallback(),
			optimizedModel, iterations, estimator, lambda, lambdaFactor, initialError, finalError))
	{
		return false;
	}

	optimizedPlane = Plane3(optimizedModel[0], optimizedModel[1], optimizedModel[2]);
	ocean_assert(optimizedPlane.isValid());

	return true;
}

/**
 * Defines a data object for the universal pose-plane optimization.
 */
class NonLinearOptimizationPlane::OnePoseOnePlaneData
{
	public:

		/**
		 * Creates a new data object.
		 * @param camera The camera profile defining the projection
		 * @param planeDistance The distance between plane and the origin, with range (-infinity, infinity).
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 */
		OnePoseOnePlaneData(const PinholeCamera& camera, const Line3* raysFirst, const Scalar planeDistance, const ImagePoint* imagePointsSecond, const size_t numberImagePoints, const bool distortImagePoints, const bool onlyFrontObjectPoints) :
			camera_(camera),
			raysFirst_(raysFirst),
			planeDistance_(planeDistance),
			imagePointsSecond_(imagePointsSecond),
			numberImagePoints_(numberImagePoints),
			distortImagePoints_(distortImagePoints),
			onlyFrontObjectPoints_(onlyFrontObjectPoints)
		{
			// nothing to do here
		}

		/**
		 * Value calculation function.
		 * @param externalModel External model that is applied to determine the value
		 * @param index The index of the data element
		 * @param result Resulting value
		 */
		void value(const StaticBuffer<Scalar, 20>& externalModel, const size_t index, StaticBuffer<Scalar, 2>& result)
		{
			const Vector2 projectedPoint(projectObjectPoint(externalModel, index));

			result[0] = projectedPoint[0];
			result[1] = projectedPoint[1];
		}

		/**
		 * Error calculation function.
		 * @param externalModel External model that is applied to determine the error
		 * @param index The index of the data element
		 * @param result Resulting value
		 */
		bool error(const StaticBuffer<Scalar, 20>& externalModel, const size_t index, StaticBuffer<Scalar, 2>& result)
		{
			bool frontObjectPoint;
			const Vector2 projectedPoint(projectObjectPoint(externalModel, index, &frontObjectPoint));

			if (onlyFrontObjectPoints_ && !frontObjectPoint)
				return false;

			const Vector2& measurementImagePoint = imagePointsSecond_[index];
			const Vector2 error(projectedPoint - measurementImagePoint);

			result[0] = error[0];
			result[1] = error[1];
			return true;
		}

		/**
		 * Transforms the internal model to an external model and normalized the internal model.
		 * @param internalModel Internal model that will be transformed
		 * @param externalModel Resulting external model
		 */
		void transformModel(StaticBuffer<Scalar, 8>& internalModel, StaticBuffer<Scalar, 20>& externalModel)
		{
			// internal model: 2 plane parameters, 6 pose parameters
			// external model: 4 plane parameters, 16 pose parameters

			const HomogenousMatrix4 transformationIF((*(Pose*)(internalModel.data() + 2)).transformation());
			memcpy(externalModel.data() + 4, transformationIF.data(), sizeof(transformationIF));

			const Plane3 externalPlane = Plane3(internalModel[0], internalModel[1], planeDistance_);
			memcpy(externalModel.data() + 0, &externalPlane, sizeof(externalPlane));
		}

	protected:

		/**
		 * Determines the object points of a given point correspondences and projected the object point into the second frame.
		 * @param externalModel The external model that is applied
		 * @param index The index of the image point correspondence
		 * @param frontObjectPoint Optional resulting information whether the projected object point lies in front of the cameras.
		 * @return Resulting projected object point
		 */
		inline Vector2 projectObjectPoint(const StaticBuffer<Scalar, 20>& externalModel, const size_t index, bool* frontObjectPoint = nullptr)
		{
			const HomogenousMatrix4& poseSecondIF = *((HomogenousMatrix4*)(externalModel.data() + 4));
			const Plane3& plane = *(Plane3*)(externalModel.data() + 0);
			ocean_assert(plane.isValid());

			ocean_assert(index < numberImagePoints_);

			const Line3& rayFirst = raysFirst_[index];

			Vector3 objectPoint;
			if (!plane.intersection(rayFirst, objectPoint))
			{
				ocean_assert(false && "No valid intersection!");
			}

			if (frontObjectPoint)
			{
				// determine whether the 3D object point lies in front of both cameras
				*frontObjectPoint = rayFirst.direction() * (objectPoint - rayFirst.point()) > 0 && (poseSecondIF * objectPoint).z() > 0;
			}

			return camera_.projectToImageIF<true>(poseSecondIF, objectPoint, distortImagePoints_);
		}

	protected:

		/// The camera object that is used for optimization.
		const PinholeCamera& camera_;

		/// 3D rays from the first frame.
		const Line3* raysFirst_;

		/// The distance between plane and the origin, with range (-infinity, infinity).
		const Scalar planeDistance_;

		/// Image points in the second frame, each point corresponds to one ray in the first frame (by index).
		const ImagePoint* imagePointsSecond_;

		/// Number of given image points.
		const size_t numberImagePoints_;

		/// True, to apply camera distortion parameters.
		const bool distortImagePoints_;

		/// True, to avoid that an object point 3D position lies behind any camera
		const bool onlyFrontObjectPoints_;
};

bool NonLinearOptimizationPlane::optimizeOnePoseOnePlaneIF(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCameraFirst_T_world, const HomogenousMatrix4& flippedCameraSecond_T_world, const Plane3& plane, const ConstIndexedAccessor<Vector2>& imagePointsFirst, const ConstIndexedAccessor<Vector2>& imagePointsSecond, const bool distortImagePoints, HomogenousMatrix4& optimizedInvertedFlippedPoseSecond, Plane3& optimizedPlane, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialError, Scalar* finalError)
{
	typedef NonLinearUniversalOptimizationDense<8, 2, 20> UniversalOptimization;

	ocean_assert(pinholeCamera.isValid());
	ocean_assert(flippedCameraFirst_T_world.isValid());
	ocean_assert(flippedCameraSecond_T_world.isValid());
	ocean_assert(imagePointsFirst.size() == imagePointsSecond.size());

	// we have to avoid that the first camera pose is identical to the second pose as this case provides mathematical uncertainties
	ocean_assert(!Geometry::Error::posesAlmostEqual(flippedCameraFirst_T_world, flippedCameraSecond_T_world, Vector3(Scalar(0.001), Scalar(0.001), Scalar(0.001)), Scalar(Numeric::deg2rad(1))));

	const HomogenousMatrix4 poseFirst(PinholeCamera::invertedFlipped2Standard(flippedCameraFirst_T_world));

	// create 3D rays beginning at the camera center of the first pose and intersecting the corresponding image points
	Lines3 raysFirst;
	raysFirst.reserve(imagePointsFirst.size());

	for (size_t n = 0; n < imagePointsFirst.size(); ++n)
	{
		if (distortImagePoints)
		{
			const Vector2 undistortedImagePoint(pinholeCamera.undistort<true>(imagePointsFirst[n]));
			raysFirst.push_back(pinholeCamera.ray(undistortedImagePoint, poseFirst));
		}
		else
		{
			raysFirst.push_back(pinholeCamera.ray(imagePointsFirst[n], poseFirst));
		}
	}

	const ScopedConstMemoryAccessor<Vector2> scopedImagePointsSecondMemory(imagePointsSecond);
	OnePoseOnePlaneData data(pinholeCamera, raysFirst.data(), plane.distance(), scopedImagePointsSecondMemory.data(), scopedImagePointsSecondMemory.size(), distortImagePoints, onlyFrontObjectPoints);

	// Model layout: yaw, pitch | tx, ty, tz, rx, ry, rz

	const Pose poseSecondIF(flippedCameraSecond_T_world);
	StaticBuffer<Scalar, 8> model, optimizedModel;
	plane.decomposeNormal(model[0], model[1]);
	memcpy(model.data() + 2, &poseSecondIF, sizeof(poseSecondIF));

	if (!UniversalOptimization::optimizeUniversalModel(model, scopedImagePointsSecondMemory.size(), UniversalOptimization::ValueCallback::create(data, &OnePoseOnePlaneData::value), UniversalOptimization::ErrorCallback::create(data, &OnePoseOnePlaneData::error), UniversalOptimization::ModelTransformationCallback::create(data, &OnePoseOnePlaneData::transformModel), UniversalOptimization::ModelAdjustmentCallback(), optimizedModel, iterations, estimator, lambda, lambdaFactor, initialError, finalError))
	{
		return false;
	}

	const Pose& optimizedPoseSecondIF = *((Pose*)(optimizedModel.data() + 2));
	optimizedInvertedFlippedPoseSecond = optimizedPoseSecondIF.transformation();
	optimizedPlane = Plane3(optimizedModel[0], optimizedModel[1], plane.distance());

	ocean_assert(optimizedInvertedFlippedPoseSecond.isValid());
	ocean_assert(optimizedPlane.isValid());

	return true;
}

/**
 * Defines a data object for the universal optimization one shared 3D plane and several individual poses.
 */
class NonLinearOptimizationPlane::PosesPlaneData
{
	public:

		/**
		 * Creates a new data object.
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 */
		PosesPlaneData(const PinholeCamera& pinholeCamera, const Lines3& raysFirst, const Scalar planeDistance, const ImagePointGroups& imagePointGroups, const bool distortImagePoints, const bool onlyFrontObjectPoints) :
			camera_(pinholeCamera),
			raysFirst_(raysFirst),
			planeDistance_(planeDistance),
			imagePointGroups_(imagePointGroups),
			distortImagePoints_(distortImagePoints),
			onlyFrontObjectPoints_(onlyFrontObjectPoints)
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
		void value(const StaticBuffer<Scalar, 4>& externalSharedModel, const StaticBuffer<Scalar, 16>& externalIndividualModel, const size_t individualModelIndex, const size_t elementIndex, StaticBuffer<Scalar, 2>& result)
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
		bool error(const StaticBuffer<Scalar, 4>& externalSharedModel, const StaticBuffer<Scalar, 16>& externalIndividualModel, const size_t individualModelIndex, const size_t elementIndex, StaticBuffer<Scalar, 2>& result)
		{
			bool frontObjectPoint;
			const Vector2 projectedPoint(projectObjectPoint(externalSharedModel, externalIndividualModel, individualModelIndex, elementIndex, &frontObjectPoint));

			if (onlyFrontObjectPoints_ && !frontObjectPoint)
			{
				return false;
			}

			const Vector2& measurementImagePoint = imagePointGroups_[individualModelIndex][elementIndex];
			const Vector2 error(projectedPoint - measurementImagePoint);

			result[0] = error[0];
			result[1] = error[1];
			return true;
		}

		/**
		 * Transforms the internal shared model to an external shared model and normalized the internal model.
		 * @param internalModel Internal model that will be transformed
		 * @param externalModel Resulting external model
		 */
		void transformSharedModel(StaticBuffer<Scalar, 2>& internalModel, StaticBuffer<Scalar, 4>& externalModel)
		{
			// internal model: 2 plane parameters
			// external model: 4 plane parameters

			const Plane3 externalPlane(internalModel[0], internalModel[1], planeDistance_);
			memcpy(externalModel.data(), &externalPlane, sizeof(externalPlane));
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
		 * Determines the object points of a given point correspondences and projected the object point into the second frame.
		 * @param externalSharedModel External shared model that is applied
		 * @param externalIndividualModel External individual model that is applied
		 * @param individualModelIndex Index of the individual model data set
		 * @param elementIndex Index of the element within the individual model data set
		 * @param frontObjectPoint Optional resulting information whether the projected object point lies in front of the cameras.
		 * @return Resulting projected object point
		 */
		inline Vector2 projectObjectPoint(const StaticBuffer<Scalar, 4>& externalSharedModel, const StaticBuffer<Scalar, 16>& externalIndividualModel, const size_t individualModelIndex, const size_t elementIndex, bool* frontObjectPoint = nullptr)
		{
			const HomogenousMatrix4& poseSecondIF = *((HomogenousMatrix4*)(externalIndividualModel.data()));
			const Plane3& plane = *((Plane3*)(externalSharedModel.data()));
			ocean_assert(plane.isValid());

			ocean_assert_and_suppress_unused(individualModelIndex < imagePointGroups_.size(), individualModelIndex);

			const Line3& rayFirst = raysFirst_[elementIndex];

			Vector3 objectPoint;
			if (!plane.intersection(rayFirst, objectPoint))
			{
				ocean_assert(false && "No valid intersection!");
			}

			if (frontObjectPoint)
			{
				// determine whether the 3D object point lies in front of both cameras
				*frontObjectPoint = rayFirst.direction() * (objectPoint - rayFirst.point()) > 0 && (poseSecondIF * objectPoint).z() > 0;
			}

			return camera_.projectToImageIF<true>(poseSecondIF, objectPoint, distortImagePoints_);
		}

	protected:

		/// The camera object that is used for optimization.
		const PinholeCamera& camera_;

		/// 3D rays from the first frame.
		const Lines3& raysFirst_;

		/// The distance between plane and the origin, with range (-infinity, infinity).
		const Scalar planeDistance_;

		/// The groups of image points.
		const ImagePointGroups& imagePointGroups_;

		/// True, to apply camera distortion parameters.
		const bool distortImagePoints_;

		/// True, to avoid that an object point 3D position lies behind any camera
		const bool onlyFrontObjectPoints_;
};

bool NonLinearOptimizationPlane::optimizePosesPlaneIF(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCameraFirst_T_world, const ImagePoints& imagePointsFirst, const HomogenousMatrices4& invertedFlippedPoses, const Plane3& plane, const ImagePointGroups& imagePointGroups, const bool distortImagePoints, HomogenousMatrices4& optimizedInvertedFlippedPoses, Plane3& optimizedPlane, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialError, Scalar* finalError)
{
	typedef NonLinearUniversalOptimizationSparse::SharedModelIndividualModels<2, 6, 2, 4, 16> UniversalOptimization;

	ocean_assert(pinholeCamera.isValid());
	ocean_assert(invertedFlippedPoses.size() >= 1);
	ocean_assert(imagePointsFirst.size() > 0);

	ocean_assert(invertedFlippedPoses.size() == imagePointGroups.size());

#ifdef OCEAN_DEBUG

	// we have to avoid that the first camera pose is identical to the remaining poses as this case provides mathematical uncertainties
	for (HomogenousMatrices4::const_iterator i = invertedFlippedPoses.begin(); i != invertedFlippedPoses.end(); ++i)
	{
		ocean_assert(!Geometry::Error::posesAlmostEqual(flippedCameraFirst_T_world, *i, Vector3(Scalar(0.001), Scalar(0.001), Scalar(0.001)), Scalar(Numeric::deg2rad(1))));
	}

#endif // OCEAN_DEBUG

	if (invertedFlippedPoses.empty() || invertedFlippedPoses.size() != imagePointGroups.size())
	{
		return false;
	}

	const HomogenousMatrix4 transformation(PinholeCamera::invertedFlipped2Standard(flippedCameraFirst_T_world));
	const Vector3 translation(transformation.translation());
	const Quaternion quaternion(transformation.rotation());

	Lines3 raysFirst;
	raysFirst.reserve(imagePointsFirst.size());

	for (Vectors2::const_iterator i = imagePointsFirst.begin(); i != imagePointsFirst.end(); ++i)
	{
		if (distortImagePoints)
		{
			const Vector2 undistortedImagePoint(pinholeCamera.undistort<true>(*i));
			raysFirst.emplace_back(pinholeCamera.ray(undistortedImagePoint, translation, quaternion));
		}
		else
		{
			raysFirst.emplace_back(pinholeCamera.ray(*i, translation, quaternion));
		}
	}

	UniversalOptimization::SharedModel sharedModel;
	UniversalOptimization::IndividualModels individualModels(invertedFlippedPoses.size());

	std::vector<size_t> numberElementsPerIndividualModel;
	numberElementsPerIndividualModel.reserve(invertedFlippedPoses.size());

	plane.decomposeNormal(sharedModel[0], sharedModel[1]);

	for (size_t n = 0; n < invertedFlippedPoses.size(); ++n)
	{
		const Pose pose(invertedFlippedPoses[n]);
		individualModels[n] = *((StaticBuffer<Scalar, 6>*)&pose);

		ocean_assert(imagePointsFirst.size() == imagePointGroups[n].size());
		if (imagePointsFirst.size() != imagePointGroups[n].size())
		{
			return false;
		}

		numberElementsPerIndividualModel.push_back(imagePointsFirst.size());
	}

	PosesPlaneData data(pinholeCamera, raysFirst, plane.distance(), imagePointGroups, distortImagePoints, onlyFrontObjectPoints);

	UniversalOptimization::SharedModel optimizedSharedModel;
	UniversalOptimization::IndividualModels optimizedIndividualModels;

	if (!UniversalOptimization::optimizeUniversalModel(sharedModel, individualModels, numberElementsPerIndividualModel.data(), UniversalOptimization::ValueCallback::create(data, &PosesPlaneData::value), UniversalOptimization::ErrorCallback::create(data, &PosesPlaneData::error), UniversalOptimization::SharedModelIsValidCallback(), UniversalOptimization::SharedModelTransformationCallback::create(data, &PosesPlaneData::transformSharedModel), UniversalOptimization::IndividualModelTransformationCallback::create(data, &PosesPlaneData::transformIndividualModel), UniversalOptimization::ModelAcceptedCallback(), optimizedSharedModel, optimizedIndividualModels, iterations, estimator, lambda, lambdaFactor, initialError, finalError))
	{
		return false;
	}

	optimizedPlane = Plane3(optimizedSharedModel[0], optimizedSharedModel[1], plane.distance());

	ocean_assert(optimizedPlane.isValid());

	optimizedInvertedFlippedPoses.resize(invertedFlippedPoses.size());
	for (size_t n = 0; n < invertedFlippedPoses.size(); ++n)
	{
		optimizedInvertedFlippedPoses[n] = ((Pose*)optimizedIndividualModels[n].data())->transformation();
		ocean_assert(optimizedInvertedFlippedPoses[n].isValid());
	}

	return true;
}

/**
 * Defines a data object for the universal optimization one shared 3D plane and several individual poses.
 */
class NonLinearOptimizationPlane::GeneralizedPosesPlaneData
{
	public:

		GeneralizedPosesPlaneData(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_cameraFirst, const NonLinearOptimizationPlane::ImagePointsPairs& imagePointPairGroups, const Scalar planeDistance, const bool distortImagePoints, const bool onlyFrontObjectPoints) :
			camera_(pinholeCamera),
			world_T_cameraFirst_(world_T_cameraFirst),
			imagePointPairGroups_(imagePointPairGroups),
			planeDistance_(planeDistance),
			distortImagePoints_(distortImagePoints),
			onlyFrontObjectPoints_(onlyFrontObjectPoints)
		{
			// nothing to do here
		}

		void value(const StaticBuffer<Scalar, 4>& externalSharedModel, const StaticBuffer<Scalar, 16>& externalIndividualModel, const size_t individualModelIndex, const size_t elementIndex, StaticBuffer<Scalar, 2>& result)
		{
			const Vector2 projectedPoint(projectObjectPoint(externalSharedModel, externalIndividualModel, imagePointPairGroups_[individualModelIndex].first[elementIndex]));

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
		bool error(const StaticBuffer<Scalar, 4>& externalSharedModel, const StaticBuffer<Scalar, 16>& externalIndividualModel, const size_t individualModelIndex, const size_t elementIndex, StaticBuffer<Scalar, 2>& result)
		{
			bool frontObjectPoint;
			const Vector2 projectedPoint(projectObjectPoint(externalSharedModel, externalIndividualModel, imagePointPairGroups_[individualModelIndex].first[elementIndex], &frontObjectPoint));

			if (onlyFrontObjectPoints_ && !frontObjectPoint)
			{
				Log::info() << "FAIL:" << individualModelIndex << ", " << elementIndex;
				return false;
			}

			const Vector2& measurementImagePoint = imagePointPairGroups_[individualModelIndex].second[elementIndex];
			const Vector2 error(projectedPoint - measurementImagePoint);

			result[0] = error[0];
			result[1] = error[1];
			return true;
		}

		/**
		 * Transforms the internal shared model to an external shared model and normalized the internal model.
		 * @param internalModel Internal model that will be transformed
		 * @param externalModel Resulting external model
		 */
		void transformSharedModel(StaticBuffer<Scalar, 2>& internalModel, StaticBuffer<Scalar, 4>& externalModel)
		{
			const Plane3 externalPlane(internalModel[0], internalModel[1], planeDistance_);
			memcpy(externalModel.data(), &externalPlane, sizeof(externalPlane));
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

		inline Vector2 projectObjectPoint(const StaticBuffer<Scalar, 4>& externalSharedModel, const StaticBuffer<Scalar, 16>& externalIndividualModel, const Vector2& imagePointFirst, bool* frontObjectPoint = nullptr)
		{
			const HomogenousMatrix4& poseSecondIF = *((HomogenousMatrix4*)(externalIndividualModel.data()));
			const Plane3& plane = *((Plane3*)(externalSharedModel.data()));
			ocean_assert(plane.isValid());

			const bool hasDistortionParameters = distortImagePoints_ && camera_.hasDistortionParameters();

			Line3 rayFirst;
			if (hasDistortionParameters)
			{
				const Vector2 undistortedImagePoint(camera_.undistort<true>(imagePointFirst));
				rayFirst = camera_.ray(undistortedImagePoint, world_T_cameraFirst_);
			}
			else
			{
				rayFirst = camera_.ray(imagePointFirst, world_T_cameraFirst_);
			}

			Vector3 objectPoint;
			if (!plane.intersection(rayFirst, objectPoint))
			{
				ocean_assert(false && "No valid intersection!");
			}

			if (frontObjectPoint)
			{
				// determine whether the 3D object point lies in front of both cameras
				*frontObjectPoint = rayFirst.direction() * (objectPoint - rayFirst.point()) > 0 && (poseSecondIF * objectPoint).z() > 0;
			}

			return camera_.projectToImageIF<true>(poseSecondIF, objectPoint, hasDistortionParameters);
		}

	protected:

		/// The camera profile defining the projection.
		const PinholeCamera& camera_;

		/// The camera pose of the first (static camera frame).
		const HomogenousMatrix4 world_T_cameraFirst_;

		/// The group of image point pairs, one group for each further camera pose, each pair combines the observation of one object point in the first camera frame and in a further camera frame.
		const NonLinearOptimizationPlane::ImagePointsPairs& imagePointPairGroups_;

		/// Distance parameter of the plane.
		const Scalar planeDistance_;

		/// True, to apply camera distortion parameters.
		const bool distortImagePoints_;

		/// True, to avoid that an object point 3D position lies behind any camera
		const bool onlyFrontObjectPoints_;
};

bool NonLinearOptimizationPlane::optimizePosesPlaneIF(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_T_world, const HomogenousMatrices4& flippedCameras_T_world, const ImagePointsPairs& imagePointPairGroups, const Plane3& plane, const bool distortImagePoints, HomogenousMatrices4& optimizedInvertedFlippedPoses, Plane3& optimizedPlane, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialError, Scalar* finalError)
{
	typedef NonLinearUniversalOptimizationSparse::SharedModelIndividualModels<2, 6, 2, 4, 16> UniversalOptimization;

	ocean_assert(pinholeCamera.isValid());
	ocean_assert(flippedCameras_T_world.size() >= 1);
	ocean_assert(flippedCameras_T_world.size() == imagePointPairGroups.size());

	if (flippedCameras_T_world.size() < 2 || flippedCameras_T_world.size() != imagePointPairGroups.size())
	{
		return false;
	}

#ifdef OCEAN_DEBUG

	// we have to avoid that the first camera pose is identical to the remaining poses as this case provides mathematical uncertainties
	for (size_t n = 1; n < flippedCameras_T_world.size(); ++n)
	{
		ocean_assert(!Geometry::Error::posesAlmostEqual(flippedCameras_T_world[0], flippedCameras_T_world[n], Vector3(Scalar(0.001), Scalar(0.001), Scalar(0.001)), Scalar(Numeric::deg2rad(1))));
	}

#endif // OCEAN_DEBUG

	UniversalOptimization::SharedModel sharedModel;
	UniversalOptimization::IndividualModels individualModels(flippedCameras_T_world.size());

	std::vector<size_t> numberElementsPerIndividualModel;
	numberElementsPerIndividualModel.reserve(flippedCameras_T_world.size());

	plane.decomposeNormal(sharedModel[0], sharedModel[1]);

	// the individual models start with the second pose
	for (size_t n = 0; n < flippedCameras_T_world.size(); ++n)
	{
		const Pose localFlippedCamera_T_world(flippedCameras_T_world[n]);
		individualModels[n] = *((const StaticBuffer<Scalar, 6>*)(localFlippedCamera_T_world.data()));

		ocean_assert(imagePointPairGroups[n].first.size() == imagePointPairGroups[n].second.size());
		numberElementsPerIndividualModel.emplace_back(imagePointPairGroups[n].first.size());
	}

	GeneralizedPosesPlaneData data(pinholeCamera, PinholeCamera::invertedFlipped2Standard(flippedCamera_T_world), imagePointPairGroups, plane.distance(), distortImagePoints, onlyFrontObjectPoints);

	UniversalOptimization::SharedModel optimizedSharedModel;
	UniversalOptimization::IndividualModels optimizedIndividualModels;

	if (!UniversalOptimization::optimizeUniversalModel(sharedModel, individualModels, numberElementsPerIndividualModel.data(), UniversalOptimization::ValueCallback::create(data, &GeneralizedPosesPlaneData::value), UniversalOptimization::ErrorCallback::create(data, &GeneralizedPosesPlaneData::error), UniversalOptimization::SharedModelIsValidCallback(), UniversalOptimization::SharedModelTransformationCallback::create(data, &GeneralizedPosesPlaneData::transformSharedModel), UniversalOptimization::IndividualModelTransformationCallback::create(data, &GeneralizedPosesPlaneData::transformIndividualModel), UniversalOptimization::ModelAcceptedCallback(), optimizedSharedModel, optimizedIndividualModels, iterations, estimator, lambda, lambdaFactor, initialError, finalError))
	{
		return false;
	}

	optimizedPlane = Plane3(optimizedSharedModel[0], optimizedSharedModel[1], plane.distance());
	ocean_assert(optimizedPlane.isValid());

	optimizedInvertedFlippedPoses.resize(flippedCameras_T_world.size());
	for (size_t n = 0; n < flippedCameras_T_world.size(); ++n)
	{
		optimizedInvertedFlippedPoses[n] = ((Pose*)optimizedIndividualModels[n].data())->transformation();
		ocean_assert(optimizedInvertedFlippedPoses[n].isValid());
	}

	return true;
}

}

}
