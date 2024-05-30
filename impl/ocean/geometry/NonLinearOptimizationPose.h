/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_POSE_H
#define META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_POSE_H

#include "ocean/geometry/Geometry.h"
#include "ocean/geometry/NonLinearOptimization.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements non linear optimization algorithms for camera poses.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT NonLinearOptimizationPose : protected NonLinearOptimization
{
	protected:

		/**
		 * Forward declaration of a class implementing a provider allowing to optimize a camera pose.
		 */
		class PoseOptimizationProvider;

		/**
		 * Forward declaration of a class implementing an advanced provider allowing to optimize a pinhole camera pose.
		 */
		class AdvancedPinholeCameraPoseOptimizationProvider;

		/**
		 * Forward declaration of a class implementing an advanced provider allowing to optimize any camera pose.
		 */
		class AdvancedAnyCameraPoseOptimizationProvider;

		/**
		 * Forward declaration of a class implementing a provider allowing to optimize a camera pose together with a zoom factor.
		 */
		class PoseZoomOptimizationProvider;

	public:

		/**
		 * Deprecated.
		 *
		 * Minimizes the projection error of a given 6DOF pose holding orientation and translation parameters for a pinhole camera.
		 * The given 6DOF pose is a standard extrinsic camera matrix.<br>
		 * Beware: There is another optimizePose() function with almost identical functionality/parameter layout.<br>
		 * However, this function here does not support covariance parameters and thus creates a smaller binary footprint.<br>
		 * In case binary size matters, try to use this function only, and do not mix the usage of both options.
		 * @param pinholeCamera The pinhole camera object defining the projection between 3D object points and 2D image points, must be valid
		 * @param world_T_camera 6DOF pose to minimized the projection error for, must be valid
		 * @param objectPoints 3D object points to be projected into the camera plane
		 * @param imagePoints 2D image points corresponding to the object points, the image points may be distorted or undistorted depending on the usage of the distortImagePoints state
		 * @param distortImagePoints True, to force the usage of the distortion parameters of the given camera object to distort the projected 2D image points before error determination
		 * @param world_T_optimizedCamera Resulting optimized 6DOF pose
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @return True, if the optimization succeeded
		 * @see optimizePoseIF().
		 */
		static inline bool optimizePose(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_camera, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& imagePoints, const bool distortImagePoints, HomogenousMatrix4& world_T_optimizedCamera, const unsigned int iterations = 20u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = 10, Scalar* initialError = nullptr, Scalar* finalError = nullptr);

		/**
		 * Minimizes the projection error of a given 6DOF pose holding orientation and translation parameters for any camera.
		 * The given 6DOF pose is a standard extrinsic camera matrix.<br>
		 * @param anyCamera The camera profile defining the projection, must be valid
		 * @param world_T_camera 6DOF pose to minimized the projection error for, must be valid
		 * @param objectPoints 3D object points to be projected into the camera plane
		 * @param imagePoints 2D image points corresponding to the object points, the image points may be distorted or undistorted depending on the usage of the distortImagePoints state
		 * @param world_T_optimizedCamera Resulting optimized 6DOF pose
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @return True, if the optimization succeeded
		 * @see optimizePoseIF().
		 */
		static inline bool optimizePose(const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& imagePoints, HomogenousMatrix4& world_T_optimizedCamera, const unsigned int iterations = 20u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = 10, Scalar* initialError = nullptr, Scalar* finalError = nullptr);

		/**
		 * Minimizes the projection error of a given 6-DOF camera pose.
		 * Beware: There is another optimizePose() function with almost identical functionality/parameter layout.<br>
		 * However, this function here supports covariance parameters and thus creates a bigger binary footprint.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param world_T_camera The camera pose to optimized, transforming camera points to world points, with default camera pointing towards the negative z-space with y-axis upwards, must be valid
		 * @param objectPoints 3D object points to be projected into the camera plane
		 * @param imagePoints 2D image points corresponding to the object points, the image points may be distorted or undistorted depending on the usage of the distortImagePoints state
		 * @param world_T_optimizedCamera The resulting optimized 6-DOF camera pose, transforming camera points to world points, with default camera pointing towards the negative z-space with y-axis upwards, must be valid
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator, nullptr to avoid the usage of the return value
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator, nullptr to avoid the usage of the return value
		 * @param invertedCovariances Optional 2x2 inverted covariance matrices which represent the uncertainties of the image points, one for each image point (a (2*n)x2 matrix)
		 * @return True, if the optimization succeeded
		 * @see optimizePoseIF().
		 */
		static inline bool optimizePose(const AnyCamera& camera, const HomogenousMatrix4& world_T_camera, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& imagePoints, HomogenousMatrix4& world_T_optimizedCamera, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, const Matrix* invertedCovariances);

		/**
		 * Deprecated.
		 *
		 * Minimizes the projection error of a given 6DOF pose holding orientation and translation parameters.
		 * The given 6DOF pose is a standard extrinsic camera matrix.<br>
		 * Beware: There is another optimizePose() function with almost identical functionality/parameter layout.<br>
		 * However, this function here supports covariance parameters and thus creates a bigger binary footprint.
		 * @param pinholeCamera The pinhole camera object defining the projection between 3D object points and 2D image points
		 * @param world_T_camera 6DOF pose to minimized the projection error for, must be valid
		 * @param objectPoints 3D object points to be projected into the camera plane
		 * @param imagePoints 2D image points corresponding to the object points, the image points may be distorted or undistorted depending on the usage of the distortImagePoints state
		 * @param distortImagePoints True, to force the usage of the distortion parameters of the given camera object to distort the projected 2D image points before error determination
		 * @param world_T_optimizedCamera Resulting optimized 6DOF pose
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator, nullptr to avoid the usage of the return value
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator, nullptr to avoid the usage of the return value
		 * @param invertedCovariances Set of 2x2 inverted covariance matrices that represent the uncertainties of the image points (a 2*n x 2 matrix)
		 * @return True, if the optimization succeeded
		 * @see optimizePoseIF().
		 */
		static inline bool optimizePose(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_camera, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& imagePoints, const bool distortImagePoints, HomogenousMatrix4& world_T_optimizedCamera, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, const Matrix* invertedCovariances);

		/**
		 * Minimizes the projection error of a given 6DOF pose holding orientation and translation parameters together with a flexible zoom.
		 * The given 6DOF pose is a standard extrinsic camera matrix.<br>
		 * @param pinholeCamera The pinhole camera object defining the projection between 3D object points and 2D image points
		 * @param world_T_camera 6DOF pose to minimized the projection error for, must be valid
		 * @param zoom The scalar zoom factor matching to the given pose, with range (0, infinity)
		 * @param objectPoints 3D object points to be projected into the camera plane
		 * @param imagePoints 2D image points corresponding to the object points, the image points may be distorted or undistorted depending on the usage of the distortImagePoints state
		 * @param distortImagePoints True, to force the usage of the distortion parameters of the given camera object to distort the projected 2D image points before error determination
		 * @param world_T_optimizedCamera Resulting optimized 6DOF pose
		 * @param optimizedZoom Resulting optimized zoom factor matching to the resulting optimized pose, with range (0, infinity)
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param invertedCovariances Optional set of 2x2 inverted covariance matrices that represent the uncertainties of the image points (a 2*n x 2 matrix)
		 * @return True, if the optimization succeeded
		 * @see optimizePoseIF().
		 */
		static inline bool optimizePoseZoom(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_camera, const Scalar zoom, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& imagePoints, const bool distortImagePoints, HomogenousMatrix4& world_T_optimizedCamera, Scalar& optimizedZoom, const unsigned int iterations = 20u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = 10, Scalar* initialError = nullptr, Scalar* finalError = nullptr, const Matrix* invertedCovariances = nullptr);

		/**
		 * Deprecated.
		 *
		 * Minimizes the projection error of a given inverted and flipped 6DOF pose holding orientation and translation parameters for a pinhole camera.
		 * Beware: The given inverted and flipped 6DOF pose is not equivalent to a standard extrinsic camera matrix.<br>
		 * Beware: There is another optimizePoseIF() function with almost identical functionality/parameter layout.<br>
		 * However, this function here does not support covariance parameters and thus creates a smaller binary footprint.<br>
		 * In case binary size matters, try to use this function only, and do not mix the usage of both options.
		 * @param pinholeCamera The pinhole camera object defining the projection between 3D object points and 2D image points
		 * @param flippedCamera_T_world 6DOF pose to minimized the projection error for (inverted and flipped), must be valid
		 * @param objectPoints 3D object points to be projected into the camera plane
		 * @param imagePoints 2D image points corresponding to the object points, the image points may be distorted or undistorted depending on the usage of the distortImagePoints state
		 * @param distortImagePoints True, to force the usage of the distortion parameters of the given camera object to distort the projected 2D image points before error determination
		 * @param optimizedInvertedFlippedPose Resulting optimized 6DOF pose (inverted and flipped)
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @return True, if succeeded
		 * @see optimizePose().
		 */
		static bool optimizePoseIF(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_T_world, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& imagePoints, const bool distortImagePoints, HomogenousMatrix4& optimizedInvertedFlippedPose, const unsigned int iterations = 20u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = 10, Scalar* initialError = nullptr, Scalar* finalError = nullptr);

		/**
		 * Minimizes the projection error of a given inverted and flipped 6DOF pose holding orientation and translation parameters for any camera.
		 * Beware: The given inverted and flipped 6DOF pose is not equivalent to a standard extrinsic camera matrix.
		 * @param anyCamera The camera profile defining the projection, must be valid
		 * @param flippedCamera_T_world 6DOF pose to minimized the projection error for (inverted and flipped), must be valid
		 * @param objectPoints 3D object points to be projected into the camera plane
		 * @param imagePoints 2D image points corresponding to the object points, the image points may be distorted or undistorted depending on the usage of the distortImagePoints state
		 * @param optimizedPose_flippedCamera_T_world Resulting optimized 6DOF pose (inverted and flipped)
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @return True, if succeeded
		 * @see optimizePose().
		 */
		static bool optimizePoseIF(const AnyCamera& anyCamera, const HomogenousMatrix4& flippedCamera_T_world, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& imagePoints, HomogenousMatrix4& optimizedPose_flippedCamera_T_world, const unsigned int iterations = 20u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = 10, Scalar* initialError = nullptr, Scalar* finalError = nullptr);

		/**
		 * Minimizes the projection error of a given 6-DOF camera pose.
		 * Beware: There is another optimizePose() function with almost identical functionality/parameter layout.<br>
		 * However, this function here supports covariance parameters and thus creates a bigger binary footprint.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param flippedCamera_T_world The inverted and flipped camera pose to optimized, transforming world points to flipped camera points, with default camera pointing towards the positive z-space with y-axis downwards, must be valid
		 * @param objectPoints 3D object points to be projected into the camera plane
		 * @param imagePoints 2D image points corresponding to the object points, the image points may be distorted or undistorted depending on the usage of the distortImagePoints state
		 * @param optimizedFlippedCamera_T_world The resulting optimized inverted and flipped camera pose, transforming world points to flipped camera points, with default camera pointing towards the positive z-space with y-axis downwards, must be valid
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator, nullptr to avoid the usage of the return value
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator, nullptr to avoid the usage of the return value
		 * @param invertedCovariances Optional 2x2 inverted covariance matrices which represent the uncertainties of the image points, one for each image point (a (2*n)x2 matrix)
		 * @return True, if the optimization succeeded
		 * @see optimizePose().
		 */
		static bool optimizePoseIF(const AnyCamera& camera, const HomogenousMatrix4& flippedCamera_T_world, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& imagePoints, HomogenousMatrix4& optimizedFlippedCamera_T_world, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, const Matrix* invertedCovariances);


		/**
		 * Deprecated.
		 *
		 * Minimizes the projection error of a given inverted and flipped 6DOF pose holding orientation and translation parameters.
		 * Beware: The given inverted and flipped 6DOF pose is not equivalent to a standard extrinsic camera matrix.<br>
		 * Beware: There is another optimizePoseIF() function with almost identical functionality/parameter layout.<br>
		 * However, this function here supports covariance parameters and thus creates a bigger binary footprint.
		 * @param pinholeCamera The pinhole camera object defining the projection between 3D object points and 2D image points
		 * @param flippedCamera_T_world 6DOF pose to minimized the projection error for (inverted and flipped), must be valid
		 * @param objectPoints 3D object points to be projected into the camera plane
		 * @param imagePoints 2D image points corresponding to the object points, the image points may be distorted or undistorted depending on the usage of the distortImagePoints state
		 * @param distortImagePoints True, to force the usage of the distortion parameters of the given camera object to distort the projected 2D image points before error determination
		 * @param optimizedInvertedFlippedPose Resulting optimized 6DOF pose (inverted and flipped)
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator, nullptr to avoid the usage of the return value
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator, nullptr to avoid the usage of the return value
		 * @param invertedCovariances Optional set of 2x2 inverted covariance matrices that represent the uncertainties of the image points (a 2*n x 2 matrix)
		 * @return True, if succeeded
		 * @see optimizePose().
		 */
		static bool optimizePoseIF(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_T_world, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& imagePoints, const bool distortImagePoints, HomogenousMatrix4& optimizedInvertedFlippedPose, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, const Matrix* invertedCovariances);

		/**
		 * Minimizes the projection error of a given inverted and flipped 6DOF pose holding orientation and translation parameters together with a flexible zoom.
		 * Beware: The given inverted and flipped 6DOF pose is not equivalent to a standard extrinsic camera matrix.<br>
		 * @param pinholeCamera The pinhole camera object defining the projection between 3D object points and 2D image points
		 * @param flippedCamera_T_world 6DOF pose to minimized the projection error for (inverted and flipped), must be valid
		 * @param zoom The scalar zoom factor matching to the given pose, with range (0, infinity)
		 * @param objectPoints 3D object points to be projected into the camera plane
		 * @param imagePoints 2D image points corresponding to the object points, the image points may be distorted or undistorted depending on the usage of the distortImagePoints state
		 * @param distortImagePoints True, to force the usage of the distortion parameters of the given camera object to distort the projected 2D image points before error determination
		 * @param optimizedInvertedFlippedPose Resulting optimized 6DOF pose (inverted and flipped)
		 * @param optimizedZoom Resulting optimized zoom factor matching to the resulting optimized pose, with range (0, infinity)
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param invertedCovariances Optional set of 2x2 inverted covariance matrices that represent the uncertainties of the image points (a 2*n x 2 matrix)
		 * @return True, if succeeded
		 * @see optimizePose().
		 */
		static bool optimizePoseZoomIF(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_T_world, const Scalar zoom, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& imagePoints, const bool distortImagePoints, HomogenousMatrix4& optimizedInvertedFlippedPose, Scalar& optimizedZoom, const unsigned int iterations = 20u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = 10, Scalar* initialError = nullptr, Scalar* finalError = nullptr, const Matrix* invertedCovariances = nullptr);
};

inline bool NonLinearOptimizationPose::optimizePose(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_camera, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& imagePoints, const bool distortImagePoints, HomogenousMatrix4& world_T_optimizedCamera, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(world_T_camera.isValid());
	ocean_assert(objectPoints.size() >= 3);
	ocean_assert(objectPoints.size() == imagePoints.size());

	const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera));

	HomogenousMatrix4 optimizedInvertedFlippedPose;
	if (!optimizePoseIF(pinholeCamera, flippedCamera_T_world, objectPoints, imagePoints, distortImagePoints, optimizedInvertedFlippedPose, iterations, estimator, lambda, lambdaFactor, initialError, finalError))
	{
		return false;
	}

	world_T_optimizedCamera = PinholeCamera::invertedFlipped2Standard(optimizedInvertedFlippedPose);
	return true;
}

inline bool NonLinearOptimizationPose::optimizePose(const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& imagePoints, HomogenousMatrix4& world_T_optimizedCamera, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(world_T_camera.isValid());
	ocean_assert(objectPoints.size() >= 3);
	ocean_assert(objectPoints.size() == imagePoints.size());

	const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera));

	HomogenousMatrix4 optimizedFlippedCamera_T_world;
	if (!optimizePoseIF(anyCamera, flippedCamera_T_world, objectPoints, imagePoints, optimizedFlippedCamera_T_world, iterations, estimator, lambda, lambdaFactor, initialError, finalError))
	{
		return false;
	}

	world_T_optimizedCamera = PinholeCamera::invertedFlipped2Standard(optimizedFlippedCamera_T_world);
	return true;
}

inline bool NonLinearOptimizationPose::optimizePose(const AnyCamera& camera, const HomogenousMatrix4& world_T_camera, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& imagePoints, HomogenousMatrix4& world_T_optimizedCamera, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, const Matrix* invertedCovariances)
{
	ocean_assert(camera.isValid());
	ocean_assert(world_T_camera.isValid());

	ocean_assert(objectPoints.size() >= 3u);
	ocean_assert(objectPoints.size() == imagePoints.size());

	const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera));

	HomogenousMatrix4 optimizedFlippedCamera_T_world(false);
	if (!optimizePoseIF(camera, flippedCamera_T_world, objectPoints, imagePoints, optimizedFlippedCamera_T_world, iterations, estimator, lambda, lambdaFactor, initialError, finalError, invertedCovariances))
	{
		return false;
	}

	world_T_optimizedCamera = PinholeCamera::invertedFlipped2Standard(optimizedFlippedCamera_T_world);
	return true;
}

inline bool NonLinearOptimizationPose::optimizePose(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_camera, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& imagePoints, const bool distortImagePoints, HomogenousMatrix4& world_T_optimizedCamera, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, const Matrix* invertedCovariances)
{
	const AnyCameraPinhole anyCamera(PinholeCamera(pinholeCamera, distortImagePoints));

	return optimizePose(anyCamera, world_T_camera, objectPoints, imagePoints, world_T_optimizedCamera, iterations, estimator, lambda, lambdaFactor, initialError, finalError, invertedCovariances);
}

inline bool NonLinearOptimizationPose::optimizePoseZoom(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_camera, const Scalar zoom, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& imagePoints, const bool distortImagePoints, HomogenousMatrix4& world_T_optimizedCamera, Scalar& optimizedZoom, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, const Matrix* invertedCovariances)
{
	ocean_assert(world_T_camera.isValid() && zoom > Numeric::eps());
	ocean_assert(objectPoints.size() >= 3u);
	ocean_assert(objectPoints.size() == imagePoints.size());

	const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera));

	HomogenousMatrix4 optimizedInvertedFlippedPose;
	if (!optimizePoseZoomIF(pinholeCamera, flippedCamera_T_world, zoom, objectPoints, imagePoints, distortImagePoints, optimizedInvertedFlippedPose, optimizedZoom, iterations, estimator, lambda, lambdaFactor, initialError, finalError, invertedCovariances))
	{
		return false;
	}

	world_T_optimizedCamera = PinholeCamera::invertedFlipped2Standard(optimizedInvertedFlippedPose);
	return true;
}

}

}

#endif // META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_POSE_H
