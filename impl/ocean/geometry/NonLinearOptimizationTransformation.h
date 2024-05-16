/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_TRANSFORMATION_H
#define META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_TRANSFORMATION_H

#include "ocean/geometry/Geometry.h"
#include "ocean/geometry/NonLinearOptimization.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements non linear optimization algorithms for coordinate transformations.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT NonLinearOptimizationTransformation : protected NonLinearOptimization
{
	protected:

		/**
		 * Forward declaration of a class implementing a provider allowing to optimize a 6-DOF transformation for any camera.
		 */
		class AdvancedObjectTransformationOptimizationProvider;

		/**
		 * Forward declaration of a class implementing a provider allowing to optimize a 6-DOF transformation for any stereo camera.
		 */
		class AdvancedObjectTransformationStereoOptimizationProvider;

	public:

		/**
		 * Minimizes the projection error for several 3D object points projected into several camera images via a 6-DOF object transformation (to be optimized).
		 * The individual camera poses and the camera profile will not be adjusted.<br>
		 * The entire projection pipeline has the following equation:
		 * <pre>
		 * q = K * (world_T_camera)^-1 * world_T_object * X
		 *
		 * With:
		 * q               Projected 2D image point
		 * K               Static camera projection matrix
		 * world_T_camera  Static 6-DOF camera pose transforming points from world coordinate system to camera coordinate system
		 * world_T_object  6-DOF object transformation for which the projection error will be minimized, transforming points in the object coordinate system to points in the world coordinate system
		 * X               3D object point, defined in the object coordinate system
		 * </pre>
		 * @param camera The camera profile defining the projection between 3D object points and 2D image points, must be valid
		 * @param world_T_cameras Several 6-DOF camera pose which will not be adjusted, transforming points from the world coordinate system to points in the camera coordinate systems, (world_T_camera), one pose for each group of 2D image points
		 * @param world_T_object The 6-DOF object transformation to be optimized, with orthonormal rotation matrix, must be valid
		 * @param objectPointGroups The groups of 3D object points to be projected into the camera image, one group for each camera pose, one 3D object point for each 2D image point visible in each camera frame
		 * @param imagePointGroups The groups of 2D image points, one group for each camera pose, each image point has a corresponding 3D object point
		 * @param optimized_world_T_object The resulting optimized 6-DOF object point transformation
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator The robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param intermediateErrors Optional resulting averaged pixel errors for each intermediate optimization step, in relation to the defined estimator
		 * @return True, if the optimization succeeded
		 * @see optimizeObjectTransformationIF(), optimizeObjectTransformationStereo().
		 */
		static inline bool optimizeObjectTransformation(const AnyCamera& camera, const HomogenousMatrices4& world_T_cameras, const HomogenousMatrix4& world_T_object, const ObjectPointGroups& objectPointGroups, const ImagePointGroups& imagePointGroups, HomogenousMatrix4& optimized_world_T_object, const unsigned int iterations = 20u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Minimizes the projection error for several 3D object points projected into several camera images via a 6-DOF object transformation (to be optimized).
		 * This function applies the same optimization as optimizeObjectTransformation() while uses 6-DOF camera poses with inverted and flipped coordinate system.
		 * @see optimizeObjectTransformation(), optimizeObjectTransformationStereoIF().
		 */
		static bool optimizeObjectTransformationIF(const AnyCamera& camera, const HomogenousMatrices4& flippedCameras_T_world, const HomogenousMatrix4& world_T_object, const ObjectPointGroups& objectPointGroups, const ImagePointGroups& imagePointGroups, HomogenousMatrix4& optimized_world_T_object, const unsigned int iterations = 20u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Minimizes the projection error for several 3D object points projected into several stereo camera images via a 6-DOF object transformation (to be optimized).
		 * The individual camera poses and the camera profile will not be adjusted.<br>
		 * @param cameraA First camera profile defining the project for the first stereo camera, must be valid
		 * @param cameraB Second camera profile defining the project for the second stereo camera, must be valid
		 * @param world_T_camerasA Several 6-DOF camera pose connected with the first camera profile which will not be adjusted, transforming points from the world coordinate system to points in the camera coordinate systems, (world_T_camera), one pose for each group of 2D image points
		 * @param world_T_camerasB Several 6-DOF camera pose connected with the second camera profile which will not be adjusted, transforming points from the world coordinate system to points in the camera coordinate systems, (world_T_camera), one pose for each group of 2D image points
		 * @param world_T_object The 6-DOF object transformation to be optimized, with orthonormal rotation matrix, must be valid
		 * @param objectPointGroupsA The groups of 3D object points to be projected into the camera image with first camera profile, one group for each camera pose, one 3D object point for each 2D image point visible in each camera frame
		 * @param objectPointGroupsB The groups of 3D object points to be projected into the camera image with second camera profile, one group for each camera pose, one 3D object point for each 2D image point visible in each camera frame
		 * @param imagePointGroupsA The groups of 2D image points, one group for each camera pose with first camera profile, each image point has a corresponding 3D object point
		 * @param imagePointGroupsB The groups of 2D image points, one group for each camera pose with second camera profile, each image point has a corresponding 3D object point
		 * @param optimized_world_T_object The resulting optimized 6-DOF object point transformation
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator The robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param intermediateErrors Optional resulting averaged pixel errors for each intermediate optimization step, in relation to the defined estimator
		 * @return True, if the optimization succeeded
		 * @see optimizeObjectTransformationStereoIF(), optimizeObjectTransformation().
		 */
		static inline bool optimizeObjectTransformationStereo(const AnyCamera& cameraA, const AnyCamera& cameraB, const HomogenousMatrices4& world_T_camerasA, const HomogenousMatrices4& world_T_camerasB, const HomogenousMatrix4& world_T_object, const ObjectPointGroups& objectPointGroupsA, const ObjectPointGroups& objectPointGroupsB, const ImagePointGroups& imagePointGroupsA, const ImagePointGroups& imagePointGroupsB, HomogenousMatrix4& optimized_world_T_object, const unsigned int iterations = 20u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Minimizes the projection error for several 3D object points projected into several stereo camera images via a 6-DOF object transformation (to be optimized).
		 * This function applies the same optimization as optimizeObjectTransformation() while uses 6-DOF camera poses with inverted and flipped coordinate system.
		 * @see optimizeObjectTransformation().
		 */
		static bool optimizeObjectTransformationStereoIF(const AnyCamera& cameraA, const AnyCamera& cameraB, const HomogenousMatrices4& flippedCamerasA_T_world, const HomogenousMatrices4& flippedCamerasB_T_world, const HomogenousMatrix4& world_T_object, const ObjectPointGroups& objectPointGroupsA, const ObjectPointGroups& objectPointGroupsB, const ImagePointGroups& imagePointGroupsA, const ImagePointGroups& imagePointGroupsB, HomogenousMatrix4& optimized_world_T_object, const unsigned int iterations = 20u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);
};

inline bool NonLinearOptimizationTransformation::optimizeObjectTransformation(const AnyCamera& camera, const HomogenousMatrices4& world_T_cameras, const HomogenousMatrix4& world_T_object, const ObjectPointGroups& objectPointGroups, const ImagePointGroups& imagePointGroups, HomogenousMatrix4& optimized_world_T_object, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
	return optimizeObjectTransformationIF(camera, PinholeCamera::standard2InvertedFlipped(world_T_cameras), world_T_object, objectPointGroups, imagePointGroups, optimized_world_T_object, iterations, estimator, lambda, lambdaFactor, initialError, finalError, intermediateErrors);
}

inline bool NonLinearOptimizationTransformation::optimizeObjectTransformationStereo(const AnyCamera& cameraA, const AnyCamera& cameraB, const HomogenousMatrices4& world_T_camerasA, const HomogenousMatrices4& world_T_camerasB, const HomogenousMatrix4& world_T_object, const ObjectPointGroups& objectPointGroupsA, const ObjectPointGroups& objectPointGroupsB, const ImagePointGroups& imagePointGroupsA, const ImagePointGroups& imagePointGroupsB, HomogenousMatrix4& optimized_world_T_object, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
	return optimizeObjectTransformationStereoIF(cameraA, cameraB, PinholeCamera::standard2InvertedFlipped(world_T_camerasA), PinholeCamera::standard2InvertedFlipped(world_T_camerasB), world_T_object, objectPointGroupsA, objectPointGroupsB, imagePointGroupsA, imagePointGroupsB, optimized_world_T_object, iterations, estimator, lambda, lambdaFactor, initialError, finalError, intermediateErrors);
}

}

}

#endif // META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_TRANSFORMATION_H
