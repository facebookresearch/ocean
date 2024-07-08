/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_PLANE_H
#define META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_PLANE_H

#include "ocean/geometry/Geometry.h"
#include "ocean/geometry/NonLinearOptimization.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Plane3.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements least square or robust optimization algorithms for 3D planes.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT NonLinearOptimizationPlane : protected NonLinearOptimization
{
	public:

		/**
		 * Definition of a pair holding to sets of corresponding image points.
		 */
		typedef std::pair<Vectors2, Vectors2> ImagePointsPair;

		/**
		 * Definition of a vector holding pairs of corresponding image points.
		 */
		typedef std::vector<ImagePointsPair> ImagePointsPairs;

	protected:

		/**
		 * Forward declaration of a data object allowing to optimize a 3D plane.
		 */
		class PlaneData;

		/**
		 * Forward declaration of a data object allowing to optimize a 3D plane and one 6DOF camera pose concurrently.
		 */
		class OnePoseOnePlaneData;

		/**
		 * Forward declaration of a data object allowing to optimize a 3D plane and several 6DOF camera poses concurrently.
		 */
		class PosesPlaneData;

		/**
		 * Forward declaration of a data object allowing to optimize a 3D plane and several 6DOF camera poses concurrently.
		 */
		class GeneralizedPosesPlaneData;

	public:

		/**
		 * Optimizes a 3D plane by reducing the distance between 3D object points and their projected plane point.
		 * @param plane The plane in 3D space that as to be optimized
		 * @param pointAccessor The accessor providing the 3D object points that define the 3D plane, at least three points
		 * @param optimizedPlane The resulting optimized 3D plane with reduced (optimized) distance between object points and projected plane points
		 * @param iterations The number of iterations to be applied at most, if no convergence can be reached in the meantime, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, w.r.t. the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, w.r.t. the defined estimator
		 * @return True, if succeeded
		 */
		static bool optimizePlane(const Plane3& plane, const ConstIndexedAccessor<Vector3>& pointAccessor, Plane3& optimizedPlane, const unsigned int iterations = 20u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr);

		/**
		 * Optimizes the orientation of a plane in 3D spaces and the pose of one camera pose.
		 * The projected 2D image points of several 3D plane object points are observed in two views.<br>
		 * The second pose will be optimized while the first pose defines the static reference system.<br>
		 * The given poses are equivalent to extrinsic camera matrices and thus define a transformation from the camera coordinate system into the world coordinate system.<br>
		 * @param pinholeCamera The pinhole camera object that defines the point projection
		 * @param world_T_cameraFirst First camera pose that corresponds to the first image points
		 * @param world_T_cameraSecond Second camera pose that corresponds to the second image points
		 * @param plane Initial plane on that all object points are located
		 * @param imagePointsFirst The accessor providing the projected plane object points that are visible in the first camera frame
		 * @param imagePointsSecond The accessor providing the projected plane object points that are visible in the second camera frame, one image point for each image point as defined for the first camera frame
		 * @param distortImagePoints True, to apply the distortion parameters of the camera
		 * @param world_T_optimizedCameraSecond Resulting optimized second camera pose providing minimal projection errors
		 * @param optimizedPlane Optimized plane that minimizes the projection errors
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @return Result of the optimization
		 */
		static inline bool optimizeOnePoseOnePlane(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_cameraFirst, const HomogenousMatrix4& world_T_cameraSecond, const Plane3& plane, const ConstIndexedAccessor<Vector2>& imagePointsFirst, const ConstIndexedAccessor<Vector2>& imagePointsSecond, const bool distortImagePoints, HomogenousMatrix4& world_T_optimizedCameraSecond, Plane3& optimizedPlane, const unsigned int iterations = 20u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoints = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr);

		/**
		 * Optimizes the orientation of a plane in 3D spaces and the pose of one camera pose.
		 * The projected 2D image points of several 3D plane object points are observed in two views.<br>
		 * The second pose will be optimized while the first pose defines the static reference system.<br>
		 * The given poses must be inverted and flipped around the new x axis by 180 degree.<br>
		 * @see optimizeOnePoseOnePlane().
		 */
		static bool optimizeOnePoseOnePlaneIF(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCameraFirst_T_world, const HomogenousMatrix4& flippedCameraSecond_T_world, const Plane3& plane, const ConstIndexedAccessor<Vector2>& imagePointsFirst, const ConstIndexedAccessor<Vector2>& imagePointsSecond, const bool distortImagePoints, HomogenousMatrix4& optimizedFlippedCameraSecond_T_world, Plane3& optimizedPlane, const unsigned int iterations = 20u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoints = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr);

		/**
		 * Optimizes the orientation of a plane in 3D spaces and several camera poses concurrently.
		 * 3D plane object points are observed in several individual camera frames (with individual camera poses).<br>
		 * The first pose defines the static reference system while the remaining poses will be optimized accordingly.<br>
		 * The given poses are equivalent to extrinsic camera matrices and thus define a transformation from the camera coordinate system into the world coordinate system.
		 * @param pinholeCamera The pinhole camera profile defining the projection
		 * @param world_T_camera First camera pose of the first frame (corresponding with the first set of image points)
		 * @param imagePointsFirst The first set of observations of the plane object points (visible in the first camera frame)
		 * @param world_T_cameras Further camera poses, at least 1
		 * @param plane Initial plane on that all object points are located
		 * @param imagePointGroups The group of observations of the plane object points, one group for each further pose
		 * @param distortImagePoints True, to apply the distortion parameters of the camera
		 * @param world_T_optimizedCameras Resulting optimized camera poses providing minimal projection errors
		 * @param optimizedPlane Optimized plane that minimizes the projection errors
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @return True, if succeeded
		 */
		static inline bool optimizePosesPlane(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_camera, const Vectors2& imagePointsFirst, const HomogenousMatrices4& world_T_cameras, const Plane3& plane, const ImagePointGroups& imagePointGroups, const bool distortImagePoints, HomogenousMatrices4& world_T_optimizedCameras, Plane3& optimizedPlane, const unsigned int iterations = 20u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoints = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr);

		/**
		 * Optimizes the orientation of a plane in 3D spaces and several (inverted and flipped) camera poses concurrently.
		 * 3D plane object points are observed in several individual camera frames (with individual camera poses).<br>
		 * The first pose defines the static reference system while the remaining poses will be optimized accordingly.<br>
		 * The given poses must be inverted and flipped around the new x axis by 180 degree.
		 * @see minimizePosesPlane().
		 */
		static bool optimizePosesPlaneIF(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_T_world, const Vectors2& imagePointsFirst, const HomogenousMatrices4& flippedCameras_T_world, const Plane3& plane, const ImagePointGroups& imagePointGroups, const bool distortImagePoints, HomogenousMatrices4& optimizedFlippedCameras_T_world, Plane3& optimizedPlane, const unsigned int iterations = 20u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoints = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr);

		/**
		 * Optimizes the orientation of a plane in 3D spaces and several camera poses concurrently.
		 * 3D plane object points are observed in several individual camera frames (with individual camera poses).<br>
		 * The first pose defines the static reference system while the remaining poses will be optimized accordingly.<br>
		 * The given poses are equivalent to extrinsic camera matrices and thus define a transformation from the camera coordinate system into the world coordinate system.
		 * @param pinholeCamera The pinhole camera profile defining the projection
		 * @param world_T_camera First camera pose of the first frame (corresponding with the first set of image points)
		 * @param world_T_cameras Further camera poses, at least 1
		 * @param imagePointPairGroups Groups of image pairs, one group for each further camera pose, each pair combines the observation of an plane object point in the first camera frame with the observation of the same object point in the further camera frame
		 * @param plane Initial plane on that all object points are located
		 * @param distortImagePoints True, to apply the distortion parameters of the camera
		 * @param world_T_optimizedCameras Resulting optimized camera poses providing minimal projection errors
		 * @param optimizedPlane Optimized plane that minimizes the projection errors
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @return True, if succeeded
		 */
		static inline bool optimizePosesPlane(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_camera, const HomogenousMatrices4& world_T_cameras, const ImagePointsPairs& imagePointPairGroups, const Plane3& plane, const bool distortImagePoints, HomogenousMatrices4& world_T_optimizedCameras, Plane3& optimizedPlane, const unsigned int iterations = 20u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoints = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr);

		/**
		 * Optimizes the orientation of a plane in 3D spaces and several (inverted and flipped) camera poses concurrently.
		 * 3D plane object points are observed in several individual camera frames (with individual camera poses).<br>
		 * The first pose defines the static reference system while the remaining poses will be optimized accordingly.<br>
		 * The given poses must be inverted and flipped around the new x axis by 180 degree.
		 * @see optimizePosesPlaneIF().
		 */
		static bool optimizePosesPlaneIF(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_T_world, const HomogenousMatrices4& flippedCameras_T_world, const ImagePointsPairs& imagePointPairGroups, const Plane3& plane, const bool distortImagePoints, HomogenousMatrices4& optimizedFlippedCameras_T_world, Plane3& optimizedPlane, const unsigned int iterations = 20u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoints = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr);
};

inline bool NonLinearOptimizationPlane::optimizeOnePoseOnePlane(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_cameraFirst, const HomogenousMatrix4& world_T_cameraSecond, const Plane3& plane, const ConstIndexedAccessor<Vector2>& imagePointsFirst, const ConstIndexedAccessor<Vector2>& imagePointsSecond, const bool distortImagePoints, HomogenousMatrix4& world_T_optimizedCameraSecond, Plane3& optimizedPlane, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialError, Scalar* finalError)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(world_T_cameraFirst.isValid() && world_T_cameraSecond.isValid());
	ocean_assert(imagePointsFirst.size() == imagePointsSecond.size());

	const HomogenousMatrix4 flippedCameraFirst_T_world(PinholeCamera::standard2InvertedFlipped(world_T_cameraFirst));
	const HomogenousMatrix4 flippedCameraSecond_T_world(PinholeCamera::standard2InvertedFlipped(world_T_cameraSecond));

	if (!optimizeOnePoseOnePlaneIF(pinholeCamera, flippedCameraFirst_T_world, flippedCameraSecond_T_world, plane, imagePointsFirst, imagePointsSecond, distortImagePoints, world_T_optimizedCameraSecond, optimizedPlane, iterations, estimator, lambda, lambdaFactor, onlyFrontObjectPoints, initialError, finalError))
	{
		return false;
	}

	world_T_optimizedCameraSecond = PinholeCamera::invertedFlipped2Standard(world_T_optimizedCameraSecond);
	return true;
}

inline bool NonLinearOptimizationPlane::optimizePosesPlane(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_cameraFirst, const Vectors2& imagePointsFirst, const HomogenousMatrices4& world_T_cameras, const Plane3& plane, const ImagePointGroups& imagePointGroups, const bool distortImagePoints, HomogenousMatrices4& world_T_optimizedCameras, Plane3& optimizedPlane, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialError, Scalar* finalError)
{
	const HomogenousMatrix4 flippedCameraFirst_T_world(PinholeCamera::standard2InvertedFlipped(world_T_cameraFirst));
	const HomogenousMatrices4 flippedCameras_T_world(PinholeCamera::standard2InvertedFlipped(world_T_cameras));

	if (!optimizePosesPlaneIF(pinholeCamera, flippedCameraFirst_T_world, imagePointsFirst, flippedCameras_T_world, plane, imagePointGroups, distortImagePoints, world_T_optimizedCameras, optimizedPlane, iterations, estimator, lambda, lambdaFactor, onlyFrontObjectPoints, initialError, finalError))
	{
		return false;
	}

	world_T_optimizedCameras = PinholeCamera::invertedFlipped2Standard(world_T_optimizedCameras);
	return true;
}

inline bool NonLinearOptimizationPlane::optimizePosesPlane(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& firstPose, const HomogenousMatrices4& poses, const ImagePointsPairs& imagePointPairGroups, const Plane3& plane, const bool distortImagePoints, HomogenousMatrices4& world_T_optimizedCameras, Plane3& optimizedPlane, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialError, Scalar* finalError)
{
	const HomogenousMatrices4 posesIF(PinholeCamera::standard2InvertedFlipped(poses));

	if (!optimizePosesPlaneIF(pinholeCamera, PinholeCamera::standard2InvertedFlipped(firstPose), posesIF, imagePointPairGroups, plane, distortImagePoints, world_T_optimizedCameras, optimizedPlane, iterations, estimator, lambda, lambdaFactor, onlyFrontObjectPoints, initialError, finalError))
	{
		return false;
	}

	world_T_optimizedCameras = PinholeCamera::invertedFlipped2Standard(world_T_optimizedCameras);
	return true;
}

}

}

#endif // META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_PLANE_H
