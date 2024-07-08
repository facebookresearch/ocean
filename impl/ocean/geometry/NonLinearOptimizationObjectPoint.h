/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_OBJECT_POINT_H
#define META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_OBJECT_POINT_H

#include "ocean/geometry/Geometry.h"
#include "ocean/geometry/NonLinearOptimization.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/StaticMatrix.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements least square or robust optimization algorithms optimizing the locations of 3D object points (sometimes in combination with e.g., 3-DOF camera orientations or 6-DOF camera poses).
 * Therefore, this class implements typical Bundle Adjustment tasks e.g., for 3D object points and 6-DOF camera poses.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT NonLinearOptimizationObjectPoint : protected NonLinearOptimization
{
	protected:

		/**
		 * Definition of a 3x3 matrix.
		 */
		typedef StaticMatrix<Scalar, 3, 3> StaticMatrix3x3;

		/**
		 * Definition of a 3x6 matrix.
		 */
		typedef StaticMatrix<Scalar, 3, 6> StaticMatrix3x6;

		/**
		 * Definition of a 6x3 matrix.
		 */
		typedef StaticMatrix<Scalar, 6, 3> StaticMatrix6x3;

		/**
		 * Definition of a 6x6 matrix.
		 */
		typedef StaticMatrix<Scalar, 6, 6> StaticMatrix6x6;

		/**
		 * Definition of a vector holding 3x3 matrices.
		 */
		typedef std::vector<StaticMatrix3x3> StaticMatrices3x3;

		/**
		 * Definition of a vector holding 6x3 matrices.
		 */
		typedef std::vector<StaticMatrix6x3> StaticMatrices6x3;

		/**
		 * Definition of a vector holding 6x6 matrices.
		 */
		typedef std::vector<StaticMatrix6x6> StaticMatrices6x6;

		/**
		 * Forward declaration of a provider object allowing to optimize one object point location for (6DOF) camera poses with any camera.
		 */
		class CameraObjectPointProvider;

		/**
		 * Forward declaration of a provider object allowing to optimize one object point location for (6DOF) camera poses with any camera.
		 */
		class CamerasObjectPointProvider;

		/**
		 * Forward declaration of a provider object allowing to optimize one object point location for (6DOF) camera poses with any stereo camera.
		 */
		class StereoCameraObjectPointProvider;

		/**
		 * Forward declaration of a provider object allowing to optimize one object point location for (3DOF) rotational camera poses.
		 */
		class SphericalObjectPointProvider;

		/**
		 * Forward declaration of a data object allowing to optimize object point locations and rotational camera poses.
		 */
		class ObjectPointsOrientationsData;

		/**
		 * Forward declaration of a highly optimized provider object allowing to optimize one camera pose and several 3D object point locations concurrently.
		 * @tparam tEstimator The robust estimator to be used as error measure
		 */
		template <Estimator::EstimatorType tEstimator>
		class ObjectPointsOnePoseProvider;

		/**
		 * Forward declaration of a provider allowing to optimize two camera poses and 3D object point locations concurrently.
		 */
		class ObjectPointsTwoPosesProvider;

		/**
		 * Forward declaration of a highly optimized provider object allowing to optimize several camera pose and several 3D object point locations concurrently.
		 * @tparam tEstimator The robust estimator to be used as error measure
		 * @see SlowObjectPointsPosesProvider.
		 */
		template <Estimator::EstimatorType tEstimator>
		class ObjectPointsPosesProvider;

		/**
		 * Forward declaration of a highly optimized provider object allowing to optimize the orientations of several camera pose and several 3D object point locations concurrently.
		 * @tparam tEstimator The robust estimator to be used as error measure
		 * @see SlowObjectPointsPosesProvider.
		 */
		template <Estimator::EstimatorType tEstimator>
		class ObjectPointsOrientationalPosesProvider;

		/**
		 * Forward declaration of a slow implementation of a provider object allowing to optimize several camera poses and 3D object point locations concurrently.
		 * The implementation is slower than the implementation of ObjectPointsPosesProvider and should not be used in practice.<br>
		 * The code remains for demonstration and evaluation purposes.
		 * @see ObjectPointsPosesProvider.
		 */
		class SlowObjectPointsPosesProvider;

	public:

		/**
		 * Minimizes the projection errors for one given 3D object point, visible in several individual (fixed) camera images, by minimizing the projection error between the 3D object point and the 2D image points.
		 * @param camera The camera profile defining the projection between 3D object points and 2D image points, must be valid
		 * @param world_T_cameras The accessor for 6-DOF camera poses of the camera frames, with default camera pointing towards the negative z-space with y-axis upwards, at least two
		 * @param worldObjectPoint The 3D object point to be projected into the camera plane and for that the projection error is minimized, defined in world
		 * @param imagePoints The accessor for the image points that are visible in individual camera frames (with world_T_cameras.size() == imagePoints.size())
		 * @param optimizedWorldObjectPoint Resulting optimized 3D object point, defined in world
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialRobustError Optional resulting averaged robust pixel error for the given initial parameters
		 * @param finalRobustError Optional resulting averaged robust pixel error for the final optimized parameters
		 * @param intermediateRobustErrors Optional resulting averaged robust pixel errors for intermediate optimization iterations
		 * @return True, if succeeded
		 * @see optimizeObjectPointForFixedPosesIF().
		 */
		static inline bool optimizeObjectPointForFixedPoses(const AnyCamera& camera, const ConstIndexedAccessor<HomogenousMatrix4>& world_T_cameras, const Vector3& worldObjectPoint, const ConstIndexedAccessor<Vector2>& imagePoints, Vector3& optimizedWorldObjectPoint, const unsigned int iterations = 5u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = 10, const bool onlyFrontObjectPoints = true, Scalar* initialRobustError = nullptr, Scalar* finalRobustError = nullptr, Scalars* intermediateRobustErrors = nullptr);

		/**
		 * Minimizes the projection errors for one given 3D object point, visible in several individual (fixed) camera images, by minimizing the projection error between the 3D object point and the 2D image points.
		 * Beware: The given inverted and flipped 6DOF poses are not equivalent to a standard extrinsic camera matrix.<br>
		 * @param camera The camera profile defining the projection between 3D object points and 2D image points, must be valid
		 * @param flippedCameras_T_world The accessor for (inverted and flipped) 6-DOF camera poses of the camera frames, with default flipped camera pointing towards the positive z-space with y-axis downwards, at least two
		 * @param worldObjectPoint The 3D object point to be projected into the camera plane and for that the projection error is minimized, defined in world
		 * @param imagePoints The accessor for the image points that are visible in individual camera frames (with flippedCameras_T_world.size() == imagePoints.size())
		 * @param optimizedWorldObjectPoint Resulting optimized 3D object point, defined in world
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialRobustError Optional resulting averaged robust pixel error for the given initial parameters
		 * @param finalRobustError Optional resulting averaged robust pixel error for the final optimized parameters
		 * @param intermediateRobustErrors Optional resulting averaged robust pixel errors for intermediate optimization iterations
		 * @return True, if succeeded
		 * @see optimizeObjectPointForFixedPoses().
		 */
		static bool optimizeObjectPointForFixedPosesIF(const AnyCamera& camera, const ConstIndexedAccessor<HomogenousMatrix4>& flippedCameras_T_world, const Vector3& worldObjectPoint, const ConstIndexedAccessor<Vector2>& imagePoints, Vector3& optimizedWorldObjectPoint, const unsigned int iterations = 5u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = 10, const bool onlyFrontObjectPoints = true, Scalar* initialRobustError = nullptr, Scalar* finalRobustError = nullptr, Scalars* intermediateRobustErrors = nullptr);

		/**
		 * Minimizes the projection errors for one given 3D object point, visible in several individual (fixed) camera images, by minimizing the projection error between the 3D object point and the 2D image points.
		 * @param cameras The camera profiles defining the projection between 3D object points and 2D image points, one individual provide for each observation, must be valid
		 * @param world_T_cameras The accessor for 6-DOF camera poses of the camera frames, with default camera pointing towards the negative z-space with y-axis upwards, at least two
		 * @param worldObjectPoint The 3D object point to be projected into the camera plane and for that the projection error is minimized, defined in world
		 * @param imagePoints The accessor for the image points that are visible in individual camera frames (with world_T_cameras.size() == imagePoints.size())
		 * @param optimizedWorldObjectPoint Resulting optimized 3D object point, defined in world
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialRobustError Optional resulting averaged robust pixel error for the given initial parameters
		 * @param finalRobustError Optional resulting averaged robust pixel error for the final optimized parameters
		 * @param intermediateRobustErrors Optional resulting averaged robust pixel errors for intermediate optimization iterations
		 * @return True, if succeeded
		 * @see optimizeObjectPointForFixedPosesIF().
		 */
		static inline bool optimizeObjectPointForFixedPoses(const ConstIndexedAccessor<const AnyCamera*>& cameras, const ConstIndexedAccessor<HomogenousMatrix4>& world_T_cameras, const Vector3& worldObjectPoint, const ConstIndexedAccessor<Vector2>& imagePoints, Vector3& optimizedWorldObjectPoint, const unsigned int iterations = 5u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = 10, const bool onlyFrontObjectPoints = true, Scalar* initialRobustError = nullptr, Scalar* finalRobustError = nullptr, Scalars* intermediateRobustErrors = nullptr);

		/**
		 * Minimizes the projection errors for one given 3D object point, visible in several individual (fixed) camera images, by minimizing the projection error between the 3D object point and the 2D image points.
		 * Beware: The given inverted and flipped 6DOF poses are not equivalent to a standard extrinsic camera matrix.<br>
		 * @param cameras The camera profiles defining the projection between 3D object points and 2D image points, one individual provide for each observation, must be valid
		 * @param flippedCameras_T_world The accessor for (inverted and flipped) 6-DOF camera poses of the camera frames, with default flipped camera pointing towards the positive z-space with y-axis downwards, at least two
		 * @param worldObjectPoint The 3D object point to be projected into the camera plane and for that the projection error is minimized, defined in world
		 * @param imagePoints The accessor for the image points that are visible in individual camera frames (with flippedCameras_T_world.size() == imagePoints.size())
		 * @param optimizedWorldObjectPoint Resulting optimized 3D object point, defined in world
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialRobustError Optional resulting averaged robust pixel error for the given initial parameters
		 * @param finalRobustError Optional resulting averaged robust pixel error for the final optimized parameters
		 * @param intermediateRobustErrors Optional resulting averaged robust pixel errors for intermediate optimization iterations
		 * @return True, if succeeded
		 * @see optimizeObjectPointForFixedPoses().
		 */
		static bool optimizeObjectPointForFixedPosesIF(const ConstIndexedAccessor<const AnyCamera*>& cameras, const ConstIndexedAccessor<HomogenousMatrix4>& flippedCameras_T_world, const Vector3& worldObjectPoint, const ConstIndexedAccessor<Vector2>& imagePoints, Vector3& optimizedWorldObjectPoint, const unsigned int iterations = 5u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = 10, const bool onlyFrontObjectPoints = true, Scalar* initialRobustError = nullptr, Scalar* finalRobustError = nullptr, Scalars* intermediateRobustErrors = nullptr);

		/**
		 * Minimizes the projection errors for one given 3D object point, visible in several individual (fixed) stereo fisheye camera frames, by minimizing the projection error between the 3D object point and the 2D image points.
		 * The number of provided observations from the first (e.g., left) stereo camera can be different from the number of observations from the second (e.g., right) stereo camera.
		 * @param anyCameraA The fisheye camera profile of first (e.g., left) stereo images defining the projection between 3D object points and 2D image points, must be valid
		 * @param anyCameraB The fisheye camera profile of second (e.g., right) stereo images defining the projection between 3D object points and 2D image points, must be valid
		 * @param world_T_camerasA The accessor for 6-DOF camera poses of the first stereo camera frames, with `poseAccessor_world_T_cameraA.size() + poseAccessor_world_T_cameraB.size() >= 2`
		 * @param world_T_camerasB The accessor for 6-DOF camera poses of the second stereo camera frames, with `poseAccessor_world_T_cameraA.size() + poseAccessor_world_T_cameraB.size() >= 2`
		 * @param worldObjectPoint The 3D object point to be optimized, defined in world
		 * @param imagePointAccessorA The accessor for the image points that are visible in individual first stereo camera frames (with poseAccessor_world_T_cameraA.size() == imagePointAccessorA.size())
		 * @param imagePointAccessorB The accessor for the image points that are visible in individual second stereo camera frames (with poseAccessor_world_T_cameraB.size() == imagePointAccessorB.size())
		 * @param optimizedWorldObjectPoint Resulting optimized 3D object point, defined in world
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialRobustError Optional resulting averaged robust pixel error for the given initial parameters
		 * @param finalRobustError Optional resulting averaged robust pixel error for the final optimized parameters
		 * @param intermediateRobustErrors Optional resulting averaged robust pixel errors for intermediate optimization iterations
		 * @return True, if succeeded
		 * @see optimizeObjectPointForFixedPosesIF().
		 */
		static inline bool optimizeObjectPointForFixedStereoPoses(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const ConstIndexedAccessor<HomogenousMatrix4>& world_T_camerasA, const ConstIndexedAccessor<HomogenousMatrix4>& world_T_camerasB, const Vector3& worldObjectPoint, const ConstIndexedAccessor<Vector2>& imagePointAccessorA, const ConstIndexedAccessor<Vector2>& imagePointAccessorB, Vector3& optimizedWorldObjectPoint, const unsigned int iterations = 5u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = 10, const bool onlyFrontObjectPoints = true, Scalar* initialRobustError = nullptr, Scalar* finalRobustError = nullptr, Scalars* intermediateRobustErrors = nullptr);

		/**
		 * Minimizes the projection errors for one given 3D object point, visible in several individual (fixed) stereo fisheye camera frames, by minimizing the projection error between the 3D object point and the 2D image points.
		 * The number of provided observations from the first (e.g., left) stereo camera can be different from the number of observations from the second (e.g., right) stereo camera.
		 * Beware: The given inverted and flipped 6DOF poses are not equivalent to a standard extrinsic camera matrix.<br>
		 * @param anyCameraA The fisheye camera profile of first (e.g., left) stereo images defining the projection between 3D object points and 2D image points, must be valid
		 * @param anyCameraB The fisheye camera profile of second (e.g., right) stereo images defining the projection between 3D object points and 2D image points, must be valid
		 * @param flippedCamerasA_T_world The accessor for (inverted and flipped) 6-DOF camera poses of the first stereo camera frames, with `poseAccessor_flippedCameraA_T_world.size() + poseAccessor_flippedCameraB_T_world.size() >= 2`
		 * @param flippedCamerasB_T_world The accessor for (inverted and flipped) 6-DOF camera poses of the second stereo camera frames, with `poseAccessor_flippedCameraA_T_world.size() + poseAccessor_flippedCameraB_T_world.size() >= 2`
		 * @param worldObjectPoint The 3D object point to be optimized, defined in world
		 * @param imagePointAccessorA The accessor for the image points that are visible in individual first stereo camera frames (with poseAccessor_flippedCameraA_T_world.size() == imagePointAccessorA.size())
		 * @param imagePointAccessorB The accessor for the image points that are visible in individual second stereo camera frames (with poseAccessor_flippedCameraB_T_world.size() == imagePointAccessorB.size())
		 * @param optimizedWorldObjectPoint Resulting optimized 3D object point, defined in world
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialRobustError Optional resulting averaged robust pixel error for the given initial parameters
		 * @param finalRobustError Optional resulting averaged robust pixel error for the final optimized parameters
		 * @param intermediateRobustErrors Optional resulting averaged robust pixel errors for intermediate optimization iterations
		 * @return True, if succeeded
		 * @see optimizeObjectPointForFixedPoses().
		 */
		static bool optimizeObjectPointForFixedStereoPosesIF(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const ConstIndexedAccessor<HomogenousMatrix4>& flippedCamerasA_T_world, const ConstIndexedAccessor<HomogenousMatrix4>& flippedCamerasB_T_world, const Vector3& worldObjectPoint, const ConstIndexedAccessor<Vector2>& imagePointAccessorA, const ConstIndexedAccessor<Vector2>& imagePointAccessorB, Vector3& optimizedWorldObjectPoint, const unsigned int iterations = 5u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = 10, const bool onlyFrontObjectPoints = true, Scalar* initialRobustError = nullptr, Scalar* finalRobustError = nullptr, Scalars* intermediateRobustErrors = nullptr);

		/**
		 * Minimizes the projection errors of given 3D object points, visible in several individual (fixed) camera frames, by minimizing the projection error between the 3D object point and the 2D image points.
		 * The given 6DOF poses are standard extrinsic camera matrix.<br>
		 * The number of correspondences can vary between the individual frames (groups).<br>
		 * Each group may address individual object points, however the larger the intersection of sets between the individual 3D object points in the individual frames the better the optimization result.
		 * @param pinholeCamera The pinhole camera object defining the projection between 3D object points and 2D image points
		 * @param poseAccessor The accessor for the known (and fixed) poses of the individual camera frames
		 * @param objectPoints The accessor for the known (but rough) locations of the 3D object points
		 * @param correspondenceGroups The accessor for the individual groups of correspondences between pose indices and image point location, one group for each object point
		 * @param distortImagePoints True, to force the usage of the distortion parameters of the given camera object to distort the projected 2D image points before error determination
		 * @param optimizedObjectPoints Accessor for the resulting optimized 3D object points locations
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @see optimizeObjectPointsForFixedPosesIF().
		 */
		static inline bool optimizeObjectPointsForFixedPoses(const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<HomogenousMatrix4>& poseAccessor, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, const bool distortImagePoints, NonconstIndexedAccessor<Vector3>& optimizedObjectPoints, const unsigned int iterations = 5u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = 10, const bool onlyFrontObjectPoints = true, Worker* worker = nullptr);

		/**
		 * Minimizes the projection errors of given 3D object points, visible in several individual (fixed) camera frames, by minimizing the projection error between the 3D object point and the 2D image points.
		 * Beware: The given inverted and flipped 6DOF poses are not equivalent to a standard extrinsic camera matrix.<br>
		 * The number of correspondences can vary between the individual frames (groups).<br>
		 * Each group may address individual object points, however the larger the intersection of sets between the individual 3D object points in the individual frames the better the optimization result.
		 * @param pinholeCamera The pinhole camera object defining the projection between 3D object points and 2D image points
		 * @param invertedFlippedPoses The accessor for the known (and fixed) poses of the individual camera frames
		 * @param objectPoints The accessor for the known (but rough) locations of the 3D object points
		 * @param correspondenceGroups The accessor for the individual groups of correspondences between pose indices and image point location, one group for each object point
		 * @param distortImagePoints True, to force the usage of the distortion parameters of the given camera object to distort the projected 2D image points before error determination
		 * @param optimizedObjectPoints Accessor for the resulting optimized 3D object points locations
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @see optimizeObjectPointsForFixedPoses().
		 */
		static bool optimizeObjectPointsForFixedPosesIF(const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<HomogenousMatrix4>& invertedFlippedPoses, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, const bool distortImagePoints, NonconstIndexedAccessor<Vector3>& optimizedObjectPoints, const unsigned int iterations = 5u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = 10, const bool onlyFrontObjectPoints = true, Worker* worker = nullptr);

		/**
		 * Optimizes the locations of one 3D object point visible in individual (fixed) camera poses all located at the origin with individual orientations by minimizing the projection error between the 3D object point and the 2D image points.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param world_R_cameras The accessor for the orientations of the individual cameras, with default camera pointing towards the negative z-space with y-axis upwards, at least two
		 * @param imagePoints The accessor for the image points which are projections of the 3D object points, one image point for each orientation
		 * @param objectPoint The rough object point location to optimize
		 * @param objectPointDistance The distance between the origin and the 3D object points, with range (0, infinity)
		 * @param optimizedObjectPoint The resulting optimized 3D object points locations
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoint True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if succeeded
		 * @see optimizeObjectPointForFixedOrientationsIF().
		 */
		static inline bool optimizeObjectPointForFixedOrientations(const AnyCamera& camera, const ConstIndexedAccessor<SquareMatrix3>& world_R_cameras, const ConstIndexedAccessor<Vector2>& imagePoints, const Vector3& objectPoint, const Scalar objectPointDistance, Vector3& optimizedObjectPoint, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoint = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Optimizes the locations of one 3D object point visible in individual (fixed) camera poses all located at the origin with individual orientations by minimizing the projection error between the 3D object point and the 2D image points.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param flippedCameras_R_world The accessor for the inverted and flipped orientations of the individual cameras, with default flipped camera pointing towards the positive z-space with y-axis downwards, at least two
		 * @param imagePoints The accessor for the image points which are projections of the 3D object points, one image point for each orientation
		 * @param objectPoint The rough object point location to optimize
		 * @param objectPointDistance The distance between the origin and the 3D object points, with range (0, infinity)
		 * @param optimizedObjectPoint The resulting optimized 3D object points locations
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoint True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if succeeded
		 * @see optimizeObjectPointForFixedOrientations().
		 */
		static bool optimizeObjectPointForFixedOrientationsIF(const AnyCamera& camera, const ConstIndexedAccessor<SquareMatrix3>& flippedCameras_R_world, const ConstIndexedAccessor<Vector2>& imagePoints, const Vector3& objectPoint, const Scalar objectPointDistance, Vector3& optimizedObjectPoint, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoint = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Optimizes the locations of 3D object points visible in individual camera poses all located at the origin with individual orientations by minimizing the projection error between the 3D object points and the 2D image points.
		 * Further, this function optimized the orientation of the individual camera poses.<br>
		 * The number of correspondences may vary between the individual frames (groups).<br>
		 * Each group may address individual object points, however the larger the intersection of sets between the individual 3D object points in the individual frames the better the optimization result.<br>
		 * @param pinholeCamera The pinhole camera profile defining the projection
		 * @param orientations The accessor for the known (but rough) orientations of the individual camera frames
		 * @param objectPoints The accessor for the known (but rough) locations of the 3D object points
		 * @param correspondenceGroups The accessor for the individual groups of correspondences between pose indices and image point location, one group for each object point
		 * @param objectPointDistance The distance between the origin and the 3D object points, with range (0, infinity)
		 * @param optimizedOrientations Optional accessor for the resulting optimized camera orientations, nullptr otherwise
		 * @param optimizedObjectPoints Optional accessor for the resulting optimized 3D object points locations, nullptr otherwise
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if succeeded
		 */
		static bool optimizeObjectPointsAndOrientations(const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<SquareMatrix3>& orientations, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, const Scalar objectPointDistance, NonconstIndexedAccessor<SquareMatrix3>* optimizedOrientations, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoints = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Optimizes the locations of 3D object points visible in two individual camera poses by minimizing the projection error between the 3D object points and the 2D image points.
		 * The first pose is static while the second pose and the 3D point positions are dynamic so that they will be optimized.<br>
		 * The object points are visible in both frames.
		 * @param pinholeCamera The pinhole camera profile defining the projection
		 * @param firstPose The first (static and precise) pose, must be valid
		 * @param secondPose The second (dynamic and rough) pose, must be valid
		 * @param objectPoints The accessor for the known (but rough) locations of the 3D object points
		 * @param firstImagePoints The accessor for the image points visible in the first frame, one image point for each object point (and with same order)
		 * @param secondImagePoints The accessor for the image points visible in the second frame, one image point for each object point (and with the same order)
		 * @param useDistortionParameters True, to force the usage of the distortion parameters of the given camera object to distort the projected 2D image points before error determination
		 * @param optimizedSecondPose Optional resulting optimized camera pose for the second frame, nullptr otherwise
		 * @param optimizedObjectPoints Optional accessor for the resulting optimized 3D object points locations, nullptr otherwise
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used to measure the pixel errors
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if succeeded
		 * @see optimizeObjectPointsAndOnePoseIF().
		 */
		static inline bool optimizeObjectPointsAndOnePose(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& firstPose, const HomogenousMatrix4& secondPose, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& firstImagePoints, const ConstIndexedAccessor<Vector2>& secondImagePoints, const bool useDistortionParameters, HomogenousMatrix4* optimizedSecondPose, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoints = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Optimizes the locations of 3D object points visible in two individual (inverted and flipped) camera poses by minimizing the projection error between the 3D object points and the 2D image points.
		 * The first pose is static while the second pose and the 3D point positions are dynamic so that they will be optimized.<br>
		 * The object points are visible in both frames.
		 * @param pinholeCamera The pinhole camera profile defining the projection
		 * @param firstPoseIF The first (static and precise) inverted and flipped pose, must be valid
		 * @param secondPoseIF The second (dynamic and rough) inverted and flipped pose, must be valid
		 * @param objectPoints The accessor for the known (but rough) locations of the 3D object points
		 * @param firstImagePoints The accessor for the image points visible in the first frame, one image point for each object point (and with same order)
		 * @param secondImagePoints The accessor for the image points visible in the second frame, one image point for each object point (and with the same order)
		 * @param useDistortionParameters True, to force the usage of the distortion parameters of the given camera object to distort the projected 2D image points before error determination
		 * @param optimizedSecondPoseIF Optional resulting optimized (inverted and flipped) camera pose for the second frame, nullptr otherwise
		 * @param optimizedObjectPoints Optional accessor for the resulting optimized 3D object points locations, nullptr otherwise
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used to measure the pixel errors
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if succeeded
		 * @see optimizeObjectPointsAndOnePose().
		 */
		static bool optimizeObjectPointsAndOnePoseIF(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& firstPoseIF, const HomogenousMatrix4& secondPoseIF, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& firstImagePoints, const ConstIndexedAccessor<Vector2>& secondImagePoints, const bool useDistortionParameters, HomogenousMatrix4* optimizedSecondPoseIF, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoints = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Optimizes the locations of 3D object points visible in two individual camera poses by minimizing the projection error between the 3D object points and the 2D image points.
		 * Both camera poses and the 3D point positions are dynamic.<br>
		 * The object points are visible in both frames.
		 * @param pinholeCamera The pinhole camera object defining the projection between 3D object points and 2D image points
		 * @param firstPose The first pose to optimize, must be valid
		 * @param secondPose The second pose to optimize, must be valid
		 * @param objectPoints The accessor for the known (but rough) locations of the 3D object points
		 * @param firstImagePoints The accessor for the image points visible in the first frame, one image point for each object point (and with same order)
		 * @param secondImagePoints The accessor for the image points visible in the second frame, one image point for each object point (and with the same order)
		 * @param useDistortionParameters True, to force the usage of the distortion parameters of the given camera object to distort the projected 2D image points before error determination
		 * @param optimizedFirstPose Optional resulting optimized camera pose for the first frame, nullptr otherwise
		 * @param optimizedSecondPose Optional resulting optimized camera pose for the second frame, nullptr otherwise
		 * @param optimizedObjectPoints Optional accessor for the resulting optimized 3D object points locations, nullptr otherwise
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used to measure the pixel errors
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param invertedFirstCovariances Optional set of 2x2 inverted covariance matrices that represent the uncertainties of the first image points (a 2*n x 2 matrix)
		 * @param invertedSecondCovariances Optional set of 2x2 inverted covariance matrices that represent the uncertainties of the second image points (a 2*n x 2 matrix)
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if succeeded
		 * @see optimizeObjectPointsAndTwoPosesIF().
		 */
		static inline bool optimizeObjectPointsAndTwoPoses(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& firstPose, const HomogenousMatrix4& secondPose, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& firstImagePoints, const ConstIndexedAccessor<Vector2>& secondImagePoints, const bool useDistortionParameters, HomogenousMatrix4* optimizedFirstPose, HomogenousMatrix4* optimizedSecondPose, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoints = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr, const Matrix* invertedFirstCovariances = nullptr, const Matrix* invertedSecondCovariances = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Optimizes the locations of 3D object points visible in two individual (inverted and flipped) camera poses by minimizing the projection error between the 3D object points and the 2D image points.
		 * Both camera poses and the 3D point positions are dynamic.<br>
		 * The object points are visible in both frames.
		 * @param pinholeCamera The pinhole camera object defining the projection between 3D object points and 2D image points
		 * @param firstPoseIF The first inverted and pose to optimize, must be valid
		 * @param secondPoseIF The second inverted and pose to optimize, must be valid
		 * @param objectPoints The accessor for the known (but rough) locations of the 3D object points
		 * @param firstImagePoints The accessor for the image points visible in the first frame, one image point for each object point (and with same order)
		 * @param secondImagePoints The accessor for the image points visible in the second frame, one image point for each object point (and with the same order)
		 * @param useDistortionParameters True, to force the usage of the distortion parameters of the given camera object to distort the projected 2D image points before error determination
		 * @param optimizedFirstPoseIF Optional resulting optimized inverted and camera pose for the first frame, nullptr otherwise
		 * @param optimizedSecondPoseIF Optional resulting optimized inverted and camera pose for the second frame, nullptr otherwise
		 * @param optimizedObjectPoints Optional accessor for the resulting optimized 3D object points locations, nullptr otherwise
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used to measure the pixel errors
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param invertedFirstCovariances Optional set of 2x2 inverted covariance matrices that represent the uncertainties of the first image points (a 2*n x 2 matrix)
		 * @param invertedSecondCovariances Optional set of 2x2 inverted covariance matrices that represent the uncertainties of the second image points (a 2*n x 2 matrix)
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if succeeded
		 * @see optimizeObjectPointsAndTwoPoses().
		 */
		static bool optimizeObjectPointsAndTwoPosesIF(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& firstPoseIF, const HomogenousMatrix4& secondPoseIF, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& firstImagePoints, const ConstIndexedAccessor<Vector2>& secondImagePoints, const bool useDistortionParameters, HomogenousMatrix4* optimizedFirstPoseIF, HomogenousMatrix4* optimizedSecondPoseIF, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoints = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr, const Matrix* invertedFirstCovariances = nullptr, const Matrix* invertedSecondCovariances = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Optimizes the locations of 3D object points visible in individual camera poses by minimizing the projection error between the 3D object points and the 2D image points.
		 * The number of correspondences may vary between the individual frames (groups).<br>
		 * Each group may address individual object points, however the larger the intersection of sets between the individual 3D object points in the individual frames the better the optimization result.<br>
		 * This function performs a classic Bundle Adjustment using a per-session camera profile (the same identical camera profile for all individual images).
		 * @param camera The camera profile defining the projection for all camera frames, must be valid
		 * @param world_T_cameras The accessor for the known (but rough) poses of the individual camera frames, transforming cameras to world, with default camera pointing towards the negative z-space with y-axis upwards, at least two
		 * @param objectPoints The accessor for the known (but rough) locations of the 3D object points, defined in world
		 * @param correspondenceGroups The accessor for the individual groups of correspondences between pose indices and image point location, one group for each object point
		 * @param world_T_optimizedCameras Optional accessor for the resulting optimized camera poses, nullptr otherwise
		 * @param optimizedObjectPoints Optional accessor for the resulting optimized 3D object points locations, nullptr otherwise
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if succeeded
		 * @see optimizeObjectPointsAndPosesIF(), NonLinearOptimization::ObjectPointToPoseIndexImagePointCorrespondenceAccessor.
		 */
		static inline bool optimizeObjectPointsAndPoses(const AnyCamera& camera, const ConstIndexedAccessor<HomogenousMatrix4>& world_T_cameras, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, NonconstIndexedAccessor<HomogenousMatrix4>* world_T_optimizedCameras, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoints = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Optimizes the locations of 3D object points visible in individual (inverted and flipped) camera poses by minimizing the projection error between the 3D object points and the 2D image points.
		 * The number of correspondences may vary between the individual frames (groups).<br>
		 * Each group may address individual object points, however the larger the intersection of sets between the individual 3D object points in the individual frames the better the optimization result.<br>
		 * This function performs a classic Bundle Adjustment using a per-session camera profile (the same identical camera profile for all individual images).
		 * @param camera The camera profile defining the projection for all camera frames, must be valid
		 * @param flippedCameras_T_world The accessor for the known (but rough) camera frames, with default camera pointing towards the positive z-space with y-axis downwards, at least two
		 * @param objectPoints The accessor for the known (but rough) locations of the 3D object points, defined in world
		 * @param correspondenceGroups The accessor for the individual groups of correspondences between pose indices and image point location, one group for each object point
		 * @param flippedOptimizedCameras_T_world Optional accessor for the resulting optimized camera poses, nullptr otherwise
		 * @param optimizedObjectPoints Optional accessor for the resulting optimized 3D object points locations, nullptr otherwise
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)s
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if succeeded
		 * @see optimizeObjectPointsAndPoses(), NonLinearOptimization::ObjectPointToPoseIndexImagePointCorrespondenceAccessor.
		 */
		static inline bool optimizeObjectPointsAndPosesIF(const AnyCamera& camera, const ConstIndexedAccessor<HomogenousMatrix4>& flippedCameras_T_world, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, NonconstIndexedAccessor<HomogenousMatrix4>* flippedOptimizedCameras_T_world, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoints = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Optimizes the locations of 3D object points visible in individual camera poses by minimizing the projection error between the 3D object points and the 2D image points.
		 * The number of correspondences may vary between the individual frames (groups).<br>
		 * Each group may address individual object points, however the larger the intersection of sets between the individual 3D object points in the individual frames the better the optimization result.<br>
		 * This function performs a classic Bundle Adjustment using a per-image camera profile (an individual camera profile for each individual image).
		 * @param cameras The camera profiles defining the projection, one for each camera frame, must be valid
		 * @param world_T_cameras The accessor for the known (but rough) poses of the individual camera frames, transforming cameras to world, with default camera pointing towards the negative z-space with y-axis upwards, at least two
		 * @param objectPoints The accessor for the known (but rough) locations of the 3D object points, defined in world
		 * @param correspondenceGroups The accessor for the individual groups of correspondences between pose indices and image point location, one group for each object point
		 * @param world_T_optimizedCameras Optional accessor for the resulting optimized camera poses, nullptr otherwise
		 * @param optimizedObjectPoints Optional accessor for the resulting optimized 3D object points locations, nullptr otherwise
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if succeeded
		 * @see optimizeObjectPointsAndPosesIF(), optimizeObjectPointsAndOrientationalPoses(), NonLinearOptimization::ObjectPointToPoseIndexImagePointCorrespondenceAccessor.
		 */
		static bool optimizeObjectPointsAndPoses(const ConstIndexedAccessor<const AnyCamera*>& cameras, const ConstIndexedAccessor<HomogenousMatrix4>& world_T_cameras, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, NonconstIndexedAccessor<HomogenousMatrix4>* world_T_optimizedCameras, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoints = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Optimizes the locations of 3D object points visible in individual (inverted and flipped) camera poses by minimizing the projection error between the 3D object points and the 2D image points.
		 * The number of correspondences may vary between the individual frames (groups).<br>
		 * Each group may address individual object points, however the larger the intersection of sets between the individual 3D object points in the individual frames the better the optimization result.
		 * This function performs a classic Bundle Adjustment using a per-image camera profile (an individual camera profile for each individual image).
		 * @param cameras The camera profiles defining the projection, one for each camera frame, must be valid
		 * @param flippedCameras_T_world The accessor for the known (but rough) camera frames, with default camera pointing towards the positive z-space with y-axis downwards, at least two
		 * @param objectPoints The accessor for the known (but rough) locations of the 3D object points, defined in world
		 * @param correspondenceGroups The accessor for the individual groups of correspondences between pose indices and image point location, one group for each object point
		 * @param flippedOptimizedCameras_T_world Optional accessor for the resulting optimized camera poses, nullptr otherwise
		 * @param optimizedObjectPoints Optional accessor for the resulting optimized 3D object points locations, nullptr otherwise
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)s
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if succeeded
		 * @see optimizeObjectPointsAndPoses(), optimizeObjectPointsAndOrientationalPosesIF(), NonLinearOptimization::ObjectPointToPoseIndexImagePointCorrespondenceAccessor.
		 */
		static bool optimizeObjectPointsAndPosesIF(const ConstIndexedAccessor<const AnyCamera*>& cameras, const ConstIndexedAccessor<HomogenousMatrix4>& flippedCameras_T_world, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, NonconstIndexedAccessor<HomogenousMatrix4>* flippedOptimizedCameras_T_world, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoints = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Optimizes the locations of 3D object points visible in individual camera poses by minimizing the projection error between the 3D object points and the 2D image points.
		 * The number of correspondences may vary between the individual frames (groups).<br>
		 * Each group may address individual object points, however the larger the intersection of sets between the individual 3D object points in the individual frames the better the optimization result.<br>
		 * This function performs a Bundle Adjustment but does not modify the translations of camera poses.<br>
		 * The function supports a per-image camera profile (an individual camera profile for each individual image).
		 * @param cameras The camera profiles defining the projection, one for each camera frame, must be valid
		 * @param world_T_cameras The accessor for the known (but rough) poses of the individual camera frames, transforming cameras to world, with default camera pointing towards the negative z-space with y-axis upwards, at least two
		 * @param objectPoints The accessor for the known (but rough) locations of the 3D object points, defined in world
		 * @param correspondenceGroups The accessor for the individual groups of correspondences between pose indices and image point location, one group for each object point
		 * @param world_T_optimizedCameras Optional accessor for the resulting optimized camera poses, nullptr otherwise
		 * @param optimizedObjectPoints Optional accessor for the resulting optimized 3D object points locations, nullptr otherwise
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if succeeded
		 * @see optimizeObjectPointsAndPoses(), NonLinearOptimization::ObjectPointToPoseIndexImagePointCorrespondenceAccessor.
		 */
		static bool optimizeObjectPointsAndOrientationalPoses(const ConstIndexedAccessor<const AnyCamera*>& cameras, const ConstIndexedAccessor<HomogenousMatrix4>& world_T_cameras, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, NonconstIndexedAccessor<HomogenousMatrix4>* world_T_optimizedCameras, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoints = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Optimizes the locations of 3D object points visible in individual (inverted and flipped) camera poses by minimizing the projection error between the 3D object points and the 2D image points.
		 * The number of correspondences may vary between the individual frames (groups).<br>
		 * Each group may address individual object points, however the larger the intersection of sets between the individual 3D object points in the individual frames the better the optimization result.
		 * This function performs a Bundle Adjustment but does not modify the translations of camera poses.<br>
		 * The function supports a per-image camera profile (an individual camera profile for each individual image).
		 * @param cameras The camera profiles defining the projection, one for each camera frame, must be valid
		 * @param flippedCameras_T_world The accessor for the known (but rough) camera frames, with default camera pointing towards the positive z-space with y-axis downwards, at least two
		 * @param objectPoints The accessor for the known (but rough) locations of the 3D object points, defined in world
		 * @param correspondenceGroups The accessor for the individual groups of correspondences between pose indices and image point location, one group for each object point
		 * @param flippedOptimizedCameras_T_world Optional accessor for the resulting optimized camera poses, nullptr otherwise
		 * @param optimizedObjectPoints Optional accessor for the resulting optimized 3D object points locations, nullptr otherwise
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)s
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if succeeded
		 * @see optimizeObjectPointsAndPosesIF(), NonLinearOptimization::ObjectPointToPoseIndexImagePointCorrespondenceAccessor.
		 */
		static bool optimizeObjectPointsAndOrientationalPosesIF(const ConstIndexedAccessor<const AnyCamera*>& cameras, const ConstIndexedAccessor<HomogenousMatrix4>& flippedCameras_T_world, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, NonconstIndexedAccessor<HomogenousMatrix4>* flippedOptimizedCameras_T_world, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoints = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);

	protected:

		/**
		 * This function is a slow implementation and has been replaced by a faster implementation.
		 * The code remains for demonstration and evaluation purposes.<br>
		 * Optimizes the locations of 3D object points visible in individual camera poses by minimizing the projection error between the 3D object points and the 2D image points.<br>
		 * The number of correspondences may vary between the individual frames (groups).<br>
		 * Each group may address individual object points, however the larger the intersection of sets between the individual 3D object points in the individual frames the better the optimization result.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param world_T_cameras The accessor for the known (but rough) poses of the individual camera frames, transforming cameras to world, with default camera pointing towards the negative z-space with y-axis upwards, at least two
		 * @param objectPoints The accessor for the known (but rough) locations of the 3D object points, defined in world
		 * @param correspondenceGroups The accessor for the individual groups of correspondences between pose indices and image point location, one group for each object point
		 * @param world_T_optimizedCameras Optional accessor for the resulting optimized camera poses, nullptr otherwise
		 * @param optimizedObjectPoints Optional accessor for the resulting optimized 3D object points locations, nullptr otherwise
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if succeeded
		 * @see optimizeObjectPointsAndPoses, optimizeObjectPointsAndPosesIF(), NonLinearOptimization::ObjectPointToPoseIndexImagePointCorrespondenceAccessor.
		 */
		static bool slowOptimizeObjectPointsAndPoses(const AnyCamera& camera, const ConstIndexedAccessor<HomogenousMatrix4>& world_T_cameras, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, NonconstIndexedAccessor<HomogenousMatrix4>* world_T_optimizedCameras, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoints = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * This function is a slow implementation and has been replaced by a faster implementation.
		 * The code remains for demonstration and evaluation purposes.<br>
		 * Optimizes the locations of 3D object points visible in individual (inverted and flipped) camera poses by minimizing the projection error between the 3D object points and the 2D image points.<br>
		 * The number of correspondences may vary between the individual frames (groups).<br>
		 * Each group may address individual object points, however the larger the intersection of sets between the individual 3D object points in the individual frames the better the optimization result.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param flippedCameras_T_world The accessor for the known (but rough) camera frames, with default camera pointing towards the positive z-space with y-axis downwards, at least two
		 * @param objectPoints The accessor for the known (but rough) locations of the 3D object points, defined in world
		 * @param correspondenceGroups The accessor for the individual groups of correspondences between pose indices and image point location, one group for each object point
		 * @param flippedOptimizedCameras_T_world Optional accessor for the resulting optimized camera poses, nullptr otherwise
		 * @param optimizedObjectPoints Optional accessor for the resulting optimized 3D object points locations, nullptr otherwise
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if succeeded
		 * @see optimizeObjectPointsAndPosesIF, slowOptimizeObjectPointsAndPoses(), NonLinearOptimization::ObjectPointToPoseIndexImagePointCorrespondenceAccessor.
		 */
		static bool slowOptimizeObjectPointsAndPosesIF(const AnyCamera& camera, const ConstIndexedAccessor<HomogenousMatrix4>& flippedCameras_T_world, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, NonconstIndexedAccessor<HomogenousMatrix4>* flippedOptimizedCameras_T_world, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoints = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Deprecated.
		 *
		 * This function is a slow implementation and has been replaced by a faster implementation.
		 * The code remains for demonstration and evaluation purposes.<br>
		 * Optimizes the locations of 3D object points visible in individual camera poses by minimizing the projection error between the 3D object points and the 2D image points.<br>
		 * The number of correspondences may vary between the individual frames (groups).<br>
		 * Each group may address individual object points, however the larger the intersection of sets between the individual 3D object points in the individual frames the better the optimization result.<br>
		 * @param pinholeCamera The pinhole camera profile defining the projection
		 * @param poses The accessor for the known (but rough) poses of the individual camera frames
		 * @param objectPoints The accessor for the known (but rough) locations of the 3D object points
		 * @param correspondenceGroups The accessor for the individual groups of correspondences between pose indices and image point location, one group for each object point
		 * @param useDistortionParameters True, to force the usage of the distortion parameters of the given camera object to distort the projected 2D image points before error determination
		 * @param optimizedPoses Optional accessor for the resulting optimized camera poses, nullptr otherwise
		 * @param optimizedObjectPoints Optional accessor for the resulting optimized 3D object points locations, nullptr otherwise
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if succeeded
		 * @see optimizeObjectPointsAndPoses, optimizeObjectPointsAndPosesIF(), NonLinearOptimization::ObjectPointToPoseIndexImagePointCorrespondenceAccessor.
		 */
		static inline bool slowOptimizeObjectPointsAndPoses(const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<HomogenousMatrix4>& poses, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, const bool useDistortionParameters, NonconstIndexedAccessor<HomogenousMatrix4>* optimizedPoses, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoints = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Deprecated.
		 *
		 * This function is a slow implementation and has been replaced by a faster implementation.
		 * The code remains for demonstration and evaluation purposes.<br>
		 * Optimizes the locations of 3D object points visible in individual (inverted and flipped) camera poses by minimizing the projection error between the 3D object points and the 2D image points.<br>
		 * The number of correspondences may vary between the individual frames (groups).<br>
		 * Each group may address individual object points, however the larger the intersection of sets between the individual 3D object points in the individual frames the better the optimization result.<br>
		 * @param pinholeCamera The pinhole camera profile defining the projection
		 * @param posesIF The accessor for the known (but rough) inverted and flipped poses of the individual camera frames
		 * @param objectPoints The accessor for the known (but rough) locations of the 3D object points
		 * @param correspondenceGroups The accessor for the individual groups of correspondences between pose indices and image point location, one group for each object point
		 * @param useDistortionParameters True, to force the usage of the distortion parameters of the given camera object to distort the projected 2D image points before error determination
		 * @param optimizedPosesIF Optional accessor for the resulting optimized camera poses, nullptr otherwise
		 * @param optimizedObjectPoints Optional accessor for the resulting optimized 3D object points locations, nullptr otherwise
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if succeeded
		 * @see optimizeObjectPointsAndPosesIF, slowOptimizeObjectPointsAndPoses(), NonLinearOptimization::ObjectPointToPoseIndexImagePointCorrespondenceAccessor.
		 */
		static inline bool slowOptimizeObjectPointsAndPosesIF(const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<HomogenousMatrix4>& posesIF, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, const bool useDistortionParameters, NonconstIndexedAccessor<HomogenousMatrix4>* optimizedPosesIF, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoints = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Minimizes the projection errors of a subset of given 3D object points, visible in several individual (fixed) camera frames, by minimizing the projection error between the 3D object point and the 2D image points.
		 * Beware: The given inverted and flipped 6DOF poses are not equivalent to a standard extrinsic camera matrix.<br>
		 * @param pinholeCamera The pinhole camera object defining the projection between 3D object points and 2D image points
		 * @param invertedFlippedPoses The accessor for the known (and fixed) poses of the individual camera frames
		 * @param objectPoints The accessor for the known (but rough) locations of the 3D object points
		 * @param correspondenceGroups The accessor for the individual groups of correspondences between pose indices and image point location, one group for each object point
		 * @param distortImagePoints True, to force the usage of the distortion parameters of the given camera object to distort the projected 2D image points before error determination
		 * @param optimizedObjectPoints Resulting optimized 3D object points, the caller must provide enough memory to store the optimized 3D points
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param firstObjectPoint The first object point to be handled
		 * @param numberObjectPoints The number of object points to be handled
		 * @see optimizeObjectPointsForFixedPosesIF().
		 */
		static void optimizeObjectPointsForFixedPosesIFSubset(const PinholeCamera* pinholeCamera, const ConstIndexedAccessor<HomogenousMatrix4>* invertedFlippedPoses, const ConstIndexedAccessor<Vector3>* objectPoints, const ObjectPointGroupsAccessor* correspondenceGroups, const bool distortImagePoints, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, const unsigned int firstObjectPoint, const unsigned int numberObjectPoints);
};

inline bool NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedPoses(const AnyCamera& anyCamera, const ConstIndexedAccessor<HomogenousMatrix4>& world_T_cameras, const Vector3& worldObjectPoint, const ConstIndexedAccessor<Vector2>& imagePoints, Vector3& optimizedWorldObjectPoint, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialRobustError, Scalar* finalRobustError, Scalars* intermediateRobustErrors)
{
	const HomogenousMatrices4 flippedCameras_T_world(PinholeCamera::standard2InvertedFlipped(Accessor::accessor2elements(world_T_cameras)));

	return optimizeObjectPointForFixedPosesIF(anyCamera, ConstArrayAccessor<HomogenousMatrix4>(flippedCameras_T_world), worldObjectPoint, imagePoints, optimizedWorldObjectPoint, iterations, estimator, lambda, lambdaFactor, onlyFrontObjectPoints, initialRobustError, finalRobustError, intermediateRobustErrors);
}

inline bool NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedPoses(const ConstIndexedAccessor<const AnyCamera*>& cameras, const ConstIndexedAccessor<HomogenousMatrix4>& world_T_cameras, const Vector3& worldObjectPoint, const ConstIndexedAccessor<Vector2>& imagePoints, Vector3& optimizedWorldObjectPoint, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialRobustError, Scalar* finalRobustError, Scalars* intermediateRobustErrors)
{
	const HomogenousMatrices4 flippedCameras_T_world(AnyCamera::standard2InvertedFlipped(Accessor::accessor2elements(world_T_cameras)));

	return optimizeObjectPointForFixedPosesIF(cameras, ConstArrayAccessor<HomogenousMatrix4>(flippedCameras_T_world), worldObjectPoint, imagePoints, optimizedWorldObjectPoint, iterations, estimator, lambda, lambdaFactor, onlyFrontObjectPoints, initialRobustError, finalRobustError, intermediateRobustErrors);
}

inline bool NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedStereoPoses(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const ConstIndexedAccessor<HomogenousMatrix4>& world_T_camerasA, const ConstIndexedAccessor<HomogenousMatrix4>& world_T_camerasB, const Vector3& worldObjectPoint, const ConstIndexedAccessor<Vector2>& imagePointAccessorA, const ConstIndexedAccessor<Vector2>& imagePointAccessorB, Vector3& optimizedWorldObjectPoint, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialRobustError, Scalar* finalRobustError, Scalars* intermediateRobustErrors)
{
	const HomogenousMatrices4 flippedCamerasA_T_world(PinholeCamera::standard2InvertedFlipped(Accessor::accessor2elements(world_T_camerasA)));
	const HomogenousMatrices4 flippedCamerasB_T_world(PinholeCamera::standard2InvertedFlipped(Accessor::accessor2elements(world_T_camerasB)));

	return optimizeObjectPointForFixedStereoPosesIF(anyCameraA, anyCameraB, ConstArrayAccessor<HomogenousMatrix4>(flippedCamerasA_T_world), ConstArrayAccessor<HomogenousMatrix4>(flippedCamerasB_T_world), worldObjectPoint, imagePointAccessorA, imagePointAccessorB, optimizedWorldObjectPoint, iterations, estimator, lambda, lambdaFactor, onlyFrontObjectPoints, initialRobustError, finalRobustError, intermediateRobustErrors);
}

inline bool NonLinearOptimizationObjectPoint::optimizeObjectPointsForFixedPoses(const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<HomogenousMatrix4>& poseAccessor, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, const bool distortImagePoints, NonconstIndexedAccessor<Vector3>& optimizedObjectPoints, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Worker* worker)
{
	ocean_assert(!poseAccessor.isEmpty() && !objectPoints.isEmpty());
	ocean_assert(objectPoints.size() == correspondenceGroups.groups());

	ocean_assert(lambda >= 0);
	ocean_assert(lambdaFactor >= 1);

	HomogenousMatrices4 invertedFlippedPoses;
	invertedFlippedPoses.reserve(poseAccessor.size());

	for (size_t n = 0; n < poseAccessor.size(); ++n)
	{
		invertedFlippedPoses.push_back(PinholeCamera::standard2InvertedFlipped(poseAccessor[n]));
	}

	return optimizeObjectPointsForFixedPosesIF(pinholeCamera, ConstArrayAccessor<HomogenousMatrix4>(invertedFlippedPoses), objectPoints, correspondenceGroups, distortImagePoints, optimizedObjectPoints, iterations, estimator, lambda, lambdaFactor, onlyFrontObjectPoints, worker);
}

inline bool NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedOrientations(const AnyCamera& camera, const ConstIndexedAccessor<SquareMatrix3>& world_R_cameras, const ConstIndexedAccessor<Vector2>& imagePoints, const Vector3& objectPoint, const Scalar objectPointDistance, Vector3& optimizedObjectPoint, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator, const Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoint, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
	SquareMatrices3 flippedCameras_R_world;
	flippedCameras_R_world.reserve(world_R_cameras.size());

	for (size_t n = 0; n < world_R_cameras.size(); ++n)
	{
		flippedCameras_R_world.emplace_back(AnyCamera::standard2InvertedFlipped(world_R_cameras[n]));
	}

	return optimizeObjectPointForFixedOrientationsIF(camera, ConstArrayAccessor<SquareMatrix3>(flippedCameras_R_world), imagePoints, objectPoint, objectPointDistance, optimizedObjectPoint, iterations, estimator, lambda, lambdaFactor, onlyFrontObjectPoint, initialError, finalError, intermediateErrors);
}

inline bool NonLinearOptimizationObjectPoint::optimizeObjectPointsAndOnePose(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& firstPose, const HomogenousMatrix4& secondPose, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& firstImagePoints, const ConstIndexedAccessor<Vector2>& secondImagePoints, const bool useDistortionParameters, HomogenousMatrix4* optimizedSecondPose, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator, const Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
	const HomogenousMatrix4 firstPoseIF(PinholeCamera::standard2InvertedFlipped(firstPose));
	const HomogenousMatrix4 secondPoseIF(PinholeCamera::standard2InvertedFlipped(secondPose));

	if (!optimizeObjectPointsAndOnePoseIF(pinholeCamera, firstPoseIF, secondPoseIF, objectPoints, firstImagePoints, secondImagePoints, useDistortionParameters, optimizedSecondPose, optimizedObjectPoints, iterations, estimator, lambda, lambdaFactor, onlyFrontObjectPoints, initialError, finalError, intermediateErrors))
	{
		return false;
	}

	if (optimizedSecondPose)
	{
		*optimizedSecondPose = PinholeCamera::invertedFlipped2Standard(*optimizedSecondPose);
	}

	return true;
}

inline bool NonLinearOptimizationObjectPoint::optimizeObjectPointsAndTwoPoses(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& firstPose, const HomogenousMatrix4& secondPose, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& firstImagePoints, const ConstIndexedAccessor<Vector2>& secondImagePoints, const bool useDistortionParameters, HomogenousMatrix4* optimizedFirstPose, HomogenousMatrix4* optimizedSecondPose, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator, const Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialError, Scalar* finalError, const Matrix* invertedFirstCovariances, const Matrix* invertedSecondCovariances, Scalars* intermediateErrors)
{
	const HomogenousMatrix4 firstPoseIF(PinholeCamera::standard2InvertedFlipped(firstPose));
	const HomogenousMatrix4 secondPoseIF(PinholeCamera::standard2InvertedFlipped(secondPose));

	if (!optimizeObjectPointsAndTwoPosesIF(pinholeCamera, firstPoseIF, secondPoseIF, objectPoints, firstImagePoints, secondImagePoints, useDistortionParameters, optimizedFirstPose, optimizedSecondPose, optimizedObjectPoints, iterations, estimator, lambda, lambdaFactor, onlyFrontObjectPoints, initialError, finalError, invertedFirstCovariances, invertedSecondCovariances, intermediateErrors))
	{
		return false;
	}

	if (optimizedFirstPose)
	{
		*optimizedFirstPose = PinholeCamera::invertedFlipped2Standard(*optimizedFirstPose);
	}

	if (optimizedSecondPose)
	{
		*optimizedSecondPose = PinholeCamera::invertedFlipped2Standard(*optimizedSecondPose);
	}

	return true;
}

inline bool NonLinearOptimizationObjectPoint::optimizeObjectPointsAndPoses(const AnyCamera& camera, const ConstIndexedAccessor<HomogenousMatrix4>& world_T_cameras, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, NonconstIndexedAccessor<HomogenousMatrix4>* world_T_optimizedCameras, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator, const Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
#if 1
	// creating local pointer to avoid Clang compiler bug
	const AnyCamera* cameraPointer = &camera;
	const ConstElementAccessor<const AnyCamera*> cameraAccessor(world_T_cameras.size(), cameraPointer);
#else
	const ConstElementAccessor<const AnyCamera*> cameraAccessor(world_T_cameras.size(), &camera);
#endif

	const HomogenousMatrices4 flippedCameras_T_world(AnyCamera::standard2InvertedFlipped(Accessor::accessor2elements(world_T_cameras)));

	HomogenousMatrices4 flippedOptimizedCameras_T_world;
	NonconstArrayAccessor<HomogenousMatrix4> accessor_flippedOptimizedCameras_T_world(flippedOptimizedCameras_T_world, world_T_optimizedCameras ? world_T_cameras.size() : 0);

	if (!optimizeObjectPointsAndPosesIF(cameraAccessor, ConstArrayAccessor<HomogenousMatrix4>(flippedCameras_T_world), objectPoints, correspondenceGroups, accessor_flippedOptimizedCameras_T_world.pointer(), optimizedObjectPoints, iterations, estimator, lambda, lambdaFactor, onlyFrontObjectPoints, initialError, finalError, intermediateErrors))
	{
		return false;
	}

	if (world_T_optimizedCameras != nullptr)
	{
		for (size_t n = 0; n < flippedOptimizedCameras_T_world.size(); ++n)
		{
			(*world_T_optimizedCameras)[n] = AnyCamera::invertedFlipped2Standard(flippedOptimizedCameras_T_world[n]);
		}
	}

	return true;
}

inline bool NonLinearOptimizationObjectPoint::optimizeObjectPointsAndPosesIF(const AnyCamera& camera, const ConstIndexedAccessor<HomogenousMatrix4>& flippedCameras_T_world, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, NonconstIndexedAccessor<HomogenousMatrix4>* flippedOptimizedCameras_T_world, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator, const Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
#if 1
	// creating local pointer to avoid Clang compiler bug
	const AnyCamera* cameraPointer = &camera;
	const ConstElementAccessor<const AnyCamera*> cameraAccessor(flippedCameras_T_world.size(), cameraPointer);
#else
	const ConstElementAccessor<const AnyCamera*> cameraAccessor(flippedCameras_T_world.size(), &camera);
#endif

	return optimizeObjectPointsAndPosesIF(cameraAccessor, flippedCameras_T_world, objectPoints, correspondenceGroups, flippedOptimizedCameras_T_world, optimizedObjectPoints, iterations, estimator, lambda, lambdaFactor, onlyFrontObjectPoints, initialError, finalError, intermediateErrors);
}

inline bool NonLinearOptimizationObjectPoint::slowOptimizeObjectPointsAndPoses(const PinholeCamera& camera, const ConstIndexedAccessor<HomogenousMatrix4>& poses, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, const bool useDistortionParameters, NonconstIndexedAccessor<HomogenousMatrix4>* optimizedPoses, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator, const Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
	const AnyCameraPinhole anyCameraPinhole(PinholeCamera(camera, useDistortionParameters));

	return slowOptimizeObjectPointsAndPoses(anyCameraPinhole, poses, objectPoints, correspondenceGroups, optimizedPoses, optimizedObjectPoints, iterations, estimator, lambda, lambdaFactor, onlyFrontObjectPoints, initialError, finalError, intermediateErrors);
}

inline bool NonLinearOptimizationObjectPoint::slowOptimizeObjectPointsAndPosesIF(const PinholeCamera& camera, const ConstIndexedAccessor<HomogenousMatrix4>& posesIF, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, const bool useDistortionParameters, NonconstIndexedAccessor<HomogenousMatrix4>* optimizedPosesIF, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator, const Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
	const AnyCameraPinhole anyCameraPinhole(PinholeCamera(camera, useDistortionParameters));

	return slowOptimizeObjectPointsAndPosesIF(anyCameraPinhole, posesIF, objectPoints, correspondenceGroups, optimizedPosesIF, optimizedObjectPoints, iterations, estimator, lambda, lambdaFactor, onlyFrontObjectPoints, initialError, finalError, intermediateErrors);
}


}

}

#endif // META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_OBJECT_POINT_H
