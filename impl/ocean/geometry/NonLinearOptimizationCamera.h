/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_CAMERA_H
#define META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_CAMERA_H

#include "ocean/geometry/Geometry.h"
#include "ocean/geometry/NonLinearOptimization.h"

#include "ocean/math/PinholeCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements least square or robust optimization algorithms for camera profiles.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT NonLinearOptimizationCamera : protected NonLinearOptimization
{
	protected:

		/**
		 * Forward declaration of a base data class necessary to optimize rotational camera motion.
		 */
		class CameraOrientationsBaseData;

		/**
		 * Forward declaration of a base class allowing to optimized the camera profile.
		 * @tparam tParameters The number of parameters to optimized
		 */
		template <unsigned int tParameters>
		class CameraProfileBaseData;

		/**
		 * Forward declaration of the data class allowing to find an initial camera field of view for rotational camera motion.
		 */
		class CameraOrientationsFovData;

		/**
		 * Forward declaration of an optimization provider allowing to optimize the individual parameters of a camera profile.
		 * @tparam tOptimizationStrategy The optimization strategy to be used
		 */
		template <PinholeCamera::OptimizationStrategy tOptimizationStrategy>
		class CameraProvider;

		/**
		 * Forward declaration of the data class allowing to optimized the camera parameters for rotational camera motion.
		 * @tparam tParameters The number of parameters to optimized
		 */
		template <unsigned int tParameters>
		class CameraOrientationsData;

		/**
		 * Forward declaration of an optimization provider allowing to optimize a camera profile and camera poses concurrently.
		 */
		class CameraPosesOptimizationProvider;

		/**
		 * Forward declaration of the data class allowing to optimized the camera parameters for unconstrained (translational and rotational) camera motion.
		 * @tparam tParameters The number of parameters to optimized
		 */
		template <unsigned int tParameters>
		class CameraObjectPointsPosesData;

	public:

		/**
		 * Determines the initial field of view for a set of camera frames with known orientations and groups of correspondences of ids of 3D object points and 2D image point locations from the individual frames.
		 * The number of correspondences may vary between the individual frames (groups).<br>
		 * Each group may address individual object points, however the larger the intersection of sets between the individual 3D object points in the individual frames the better the optimization result.<br>
		 * @param pinholeCamera The pinhole camera profile for which the better field of view will be determined, must be valid
		 * @param orientations The accessor for the known orientations of the individual camera frames
		 * @param correspondenceGroups The accessor for the individual groups of point correspondences, one group for each orientation
		 * @param optimizedCamera The resulting camera profile with best matching field of view
		 * @param optimizedOrientations Optional accessor for the optimized camera orientations matching with the new camera profile
		 * @param lowerFovX The lower bound of the possible horizontal field of view in radian, with range (0, upperFovX]
		 * @param upperFovX The upper bound of the possible horizontal field of view in radian, with range [lowerFovX, PI)
		 * @param steps The number of steps in which the defined angle range is subdivided, with range [4, infinity)
		 * @param recursiveIterations The number of recursive iterations that will be applied to improve the accuracy, start with the second iteration the search for the best fov is centered at the position of the previous iteration, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param significantResult Optional resulting whether the resulting field of view has an significant impact on the error; True, of so
		 * @param finalError Optional resulting averaged square pixel error for the camera profile with the best matching field of view
		 * @param worker Optional worker object to distribute the computation
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if succeeded and the execution has not been aborted
		 */
		static bool findInitialFieldOfView(const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<SquareMatrix3>& orientations, const PoseGroupsAccessor& correspondenceGroups, PinholeCamera& optimizedCamera, NonconstIndexedAccessor<SquareMatrix3>* optimizedOrientations, const Scalar lowerFovX = Numeric::deg2rad(40), const Scalar upperFovX = Numeric::deg2rad(90), const unsigned int steps = 10u, const unsigned int recursiveIterations = 3u, const bool onlyFrontObjectPoints = true, bool* significantResult = nullptr, Scalar* finalError = nullptr, Worker* worker = nullptr, bool* abort = nullptr);

		/**
		 * Determines the initial field of view for a set of camera frames with known poses and groups of correspondences between pose indices and 2D image points locations within the pose frames while also the provided object points are optimized.
		 * The number of correspondences may vary between the individual frames (groups).<br>
		 * Each group may address individual object points, however the larger the intersection of sets between the individual 3D object points in the individual frames the better the optimization result.<br>
		 * @param pinholeCamera The pinhole camera profile for which the better field of view will be determined, must be valid
		 * @param poses The accessor for the known poses of the individual camera frames
		 * @param objectPoints The accessor for the individual 3D object points
		 * @param correspondenceGroups The accessor for the individual groups of correspondences between pose ids and image point location, one group for each object point
		 * @param optimizedCamera The resulting camera profile with best matching field of view
		 * @param optimizedPoses Optional accessor for the resulting optimized camera poses matching with the new camera profile
		 * @param optimizedObjectPoints Optional accessor for the resulting optimized object point locations
		 * @param lowerFovX The lower bound of the possible horizontal field of view in radian, with range (0, upperFovX]
		 * @param upperFovX The upper bound of the possible horizontal field of view in radian, with range [lowerFovX, PI)
		 * @param steps The number of steps in which the defined angle range is subdivided, with range [4, infinity)
		 * @param recursiveIterations The number of recursive iterations that will be applied to improve the accuracy, start with the second iteration the search for the best fov is centered at the position of the previous iteration, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param significantResult Optional resulting whether the resulting field of view has an significant impact on the error; True, of so
		 * @param finalError Optional resulting averaged square pixel error for the camera profile with the best matching field of view
		 * @param worker Optional worker object to distribute the computation
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if succeeded and the execution has not been aborted
		 */
		static bool findInitialFieldOfView(const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<HomogenousMatrix4>& poses, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, PinholeCamera& optimizedCamera, NonconstIndexedAccessor<HomogenousMatrix4>* optimizedPoses, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const Scalar lowerFovX = Numeric::deg2rad(40), const Scalar upperFovX = Numeric::deg2rad(90), const unsigned int steps = 10u, const unsigned int recursiveIterations = 3u, const bool onlyFrontObjectPoints = true, bool* significantResult = nullptr, Scalar* finalError = nullptr, Worker* worker = nullptr, bool* abort = nullptr);

		/**
		 * Optimizes the individual parameters of a camera profile by minimizing the pixel error between normalized image points (projected 3D object points) and their corresponding 2D image point observations.
		 * Which parameter of the camera profile will be optimized depends on the specified optimization strategy.
		 * @param pinholeCamera The pinhole camera profile with initial distortion parameters to optimize
		 * @param normalizedObjectPoints The accessor for the projected 3D object points (the normalized image/object points)
		 * @param imagePoints imagePoints The accessor for the observations of the projected 3D object points, defined in the pixel coordinate system, each point corresponds to one normalized object point
		 * @param optimizationStrategy The optimization strategy to be used
		 * @param optimizedCamera Resulting optimized camera object holding optimized intrinsic and distortion parameters
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @return True, if succeeded
		 */
		static bool optimizeCamera(const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<Vector2>& normalizedObjectPoints, const ConstIndexedAccessor<Vector2>& imagePoints, const PinholeCamera::OptimizationStrategy optimizationStrategy, PinholeCamera& optimizedCamera, const unsigned int iterations = 20u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr);

		/**
		 * Optimizes the camera parameters of a given camera profile for a set of camera frames with known orientation and groups of 2D/3D point correspondences from individual frames.
		 * The number of points correspondences may vary between the individual frames (groups).<br>
		 * Each group may address individual object points.<br>
		 * @param pinholeCamera The pinhole camera profile to optimized
		 * @param orientations The accessor for the known orientations of the individual camera frames
		 * @param correspondenceGroups The accessor for the individual groups of point correspondences, one group for each orientation
		 * @param optimizationStrategy The optimization strategy
		 * @param optimizedCamera The resulting camera profile with ideal field of view
		 * @param optimizedOrientations Optional accessor for the optimized camera orientations matching with the new camera profile
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param intermediateErrors Optional resulting intermediate averaged pixel errors for the individual optimization steps, in relation to the defined estimator
		 * @return True, if succeeded
		 */
		static bool optimizeCameraOrientations(const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<SquareMatrix3>& orientations, const PoseGroupsAccessor& correspondenceGroups, const PinholeCamera::OptimizationStrategy optimizationStrategy, PinholeCamera& optimizedCamera, NonconstIndexedAccessor<SquareMatrix3>* optimizedOrientations, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoints = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Minimizes the projection error between the projections of static 3D object points and their corresponding image points in several 6DOF camera poses.
		 * The camera profile as well as the camera poses are optimized concurrently.
		 * The given poses are equivalent to extrinsic camera matrices and thus define a transformation from the camera coordinate system into the world coordinate system.<br>
		 * @param pinholeCamera The pinhole camera holding intrinsic and distortion parameters to minimize the projection error for
		 * @param poses The individual camera poses, one pose for each pair of groups of object points and image points
		 * @param objectPointGroups The accessor for the individual groups of 3D object points, one group for each camera pose with at least one object point
		 * @param imagePointGroups The accessor for the individual groups of 2D image points, one group for each camera pose and one image point for each corresponding object point
		 * @param optimizedCamera The resulting optimized camera profile
		 * @param optimizedPoses Optional accessor for the resulting optimized camera poses, matching with the new camera profile
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param intermediateErrors Optional resulting intermediate averaged pixel errors for the individual optimization steps, in relation to the defined estimator
		 * @return True, if succeeded
		 * @see optimizeCameraPoseIF().
		 */
		static bool optimizeCameraPoses(const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<HomogenousMatrix4>& poses, const ConstIndexedAccessor<Vectors3>& objectPointGroups, const ConstIndexedAccessor<Vectors2>& imagePointGroups, PinholeCamera& optimizedCamera, NonconstIndexedAccessor<HomogenousMatrix4>* optimizedPoses, const unsigned int iterations, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoints = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Minimizes the projection error between the projections of static 3D object points and their corresponding image points in several 6DOF camera poses.
		 * Beware: The given poses are not equivalent to extrinsic camera matrices.<br>
		 * The given poses must be inverted and flipped around the new x axis by 180 degree.<br>
		 * @see optimizeCameraPose().
		 */
		static bool optimizeCameraPosesIF(const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<HomogenousMatrix4>& posesIF, const ConstIndexedAccessor<Vectors3>& objectPointGroups, const ConstIndexedAccessor<Vectors2>& imagePointGroups, PinholeCamera& optimizedCamera, NonconstIndexedAccessor<HomogenousMatrix4>* optimizedPosesIF, const unsigned int iterations, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoints = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Optimizes the camera parameters of a given camera profile for a set of given camera poses and a set of given 3D object points by minimizing the projection error between the 3D object points and the corresponding 2D image points.
		 * This function also optimized the camera poses and the locations of the 3D object point while the camera profile is optimized.<br>
		 * The number of 2D/3D point correspondences may vary between the individual frames (groups).<br>
		 * Each group may address individual object points, however the larger the intersection of sets between the individual 3D object points in the individual frames the better the optimization result.<br>
		 * @param pinholeCamera The pinhole camera profile to optimized
		 * @param poses The accessor for the known poses of the individual camera frames
		 * @param objectPoints The accessor for the known 3D object points locations
		 * @param correspondenceGroups The accessor for the individual groups of correspondences between pose indices and image points, one group for each object point
		 * @param optimizationStrategy The optimization strategy for the camera profile
		 * @param optimizedCamera The resulting optimized camera profile
		 * @param optimizedPoses Optional accessor for the resulting optimized camera poses, matching with the new camera profile
		 * @param optimizedObjectPoints Optional accessor for the resulting optimized 3D object point locations, matching with the new camera profile
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param intermediateErrors Optional resulting intermediate averaged pixel errors for the individual optimization steps, in relation to the defined estimator
		 * @return True, if succeeded
		 */
		static bool optimizeCameraObjectPointsPoses(const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<HomogenousMatrix4>& poses, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, const PinholeCamera::OptimizationStrategy optimizationStrategy, PinholeCamera& optimizedCamera, NonconstIndexedAccessor<HomogenousMatrix4>* optimizedPoses, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), const bool onlyFrontObjectPoints = true, Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);

	protected:

		/**
		 * Determines the initial field of view for a set of camera frames with known orientations and groups of correspondences of ids of 3D object points and 2D image point locations from the individual frames.
		 * The number of correspondences may vary between the individual frames (groups).<br>
		 * Each group may address individual object points, however the larger the intersection of sets between the individual 3D object points in the individual frames the better the optimization result.
		 * @param pinholeCamera The pinhole camera profile for which the better field of view will be determined, must be valid
		 * @param orientations The accessor for the known orientations of the individual camera frames
		 * @param correspondenceGroups The accessor for the individual groups of point correspondences, one group for each orientation
		 * @param optimizedCamera The resulting camera profile with best matching field of view
		 * @param optimizedOrientations Optional accessor for the optimized camera orientations matching with the new camera profile
		 * @param lowerFovX The lower bound of the possible horizontal field of view in radian, with range (0, upperFovX]
		 * @param upperFovX The upper bound of the possible horizontal field of view in radian, with range [lowerFovX, PI)
		 * @param overallSteps The overall number of steps in which the defined angle range is subdivided, with range [4, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param bestError Resulting averaged square pixel error for the camera profile with the best matching field of view
		 * @param allErrors Optional vector of all errors that have been determined (can be used to decide whether a resulting best camera profile is significantly different from all other camera profiles)
		 * @param lock Lock object, must be defined if this function is executed in parallel on individual threads, nullptr otherwise
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @param firstStep The first step to be handled, with range [0, overallSteps)
		 * @param steps The number of steps which will be handled (from the entire number of overall steps), with range [1, overallSteps]
		 */
		static void findInitialFieldOfViewSubset(const PinholeCamera* pinholeCamera, const ConstIndexedAccessor<SquareMatrix3>* orientations, const PoseGroupsAccessor* correspondenceGroups, PinholeCamera* optimizedCamera, SquareMatrices3* optimizedOrientations, const Scalar lowerFovX, const Scalar upperFovX, const unsigned int overallSteps, const bool onlyFrontObjectPoints, Scalar* bestError, Scalars* allErrors, Lock* lock, bool* abort, const unsigned int firstStep, const unsigned int steps);

		/**
		 * Determines the initial field of view for a set of camera frames with known poses and groups of correspondences between pose indices and 2D image points locations within the pose frames.
		 * The number of correspondences may vary between the individual frames (groups).<br>
		 * Each group may address individual object points, however the larger the intersection of sets between the individual 3D object points in the individual frames the better the optimization result.<br>
		 * @param pinholeCamera The pinhole camera profile for which the better field of view will be determined, must be valid
		 * @param poses The accessor for the known poses of the individual camera frames
		 * @param objectPoints The accessor for the individual 3D object points
		 * @param correspondenceGroups The accessor for the individual groups of correspondences between pose ids and image point location, one group for each object point
		 * @param optimizedCamera The resulting camera profile with best matching field of view
		 * @param optimizedPoses Optional accessor for the resulting optimized camera poses matching with the new camera profile
		 * @param optimizedObjectPoints Optional accessor for the resulting optimized object point locations
		 * @param lowerFovX The lower bound of the possible horizontal field of view in radian, with range (0, upperFovX]
		 * @param upperFovX The upper bound of the possible horizontal field of view in radian, with range [lowerFovX, PI)
		 * @param overallSteps The overall number of steps in which the defined angle range is subdivided, with range [4, infinity)
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 * @param bestError Resulting averaged square pixel error for the camera profile with the best matching field of view
		 * @param allErrors Optional vector of all errors that have been determined (can be used to decide whether a resulting best camera profile is significantly different from all other camera profiles)
		 * @param lock Lock object, must be defined if this function is executed in parallel on individual threads, nullptr otherwise
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @param firstStep The first step to be handled, with range [0, overallSteps)
		 * @param steps The number of steps which will be handled (from the entire number of overall steps), with range [1, overallSteps]
		 */
		static void findInitialFieldOfViewSubset(const PinholeCamera* pinholeCamera, const ConstIndexedAccessor<HomogenousMatrix4>* poses, const ConstIndexedAccessor<Vector3>* objectPoints, const ObjectPointGroupsAccessor* correspondenceGroups, PinholeCamera* optimizedCamera, HomogenousMatrices4* optimizedPoses, Vectors3* optimizedObjectPoints, const Scalar lowerFovX, const Scalar upperFovX, const unsigned int overallSteps, const bool onlyFrontObjectPoints, Scalar* bestError, Scalars* allErrors, Lock* lock, bool* abort, const unsigned int firstStep, const unsigned int steps);
};

}

}

#endif // META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_CAMERA_H
