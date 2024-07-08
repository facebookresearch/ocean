/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_ORIENTATION_H
#define META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_ORIENTATION_H

#include "ocean/geometry/Geometry.h"
#include "ocean/geometry/NonLinearOptimization.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/Matrix.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements least square or robust optimization algorithms for orientations.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT NonLinearOptimizationOrientation : protected NonLinearOptimization
{
	protected:

		/**
		 * Forward declaration of a provider object allowing to optimize an orientation.
		 */
		class OrientationOptimizationProvider;

		/**
		 * Forward declaration of a provider object allowing to optimize an orientation and a camera profile.
		 */
		class CameraOrientationOptimizationProvider;

	public:

		/**
		 * Minimizes the projection error of a given 3DOF orientation.
		 * The given 3DOF orientation is the rotational part of a standard extrinsic camera matrix.<br>
		 * @param camera  The camera profile defining the projection, must be valid
		 * @param world_R_camera 3DOF pose to minimize the projection error for
		 * @param objectPoints The accessor providing the 3D object points to be projected into the camera plane
		 * @param imagePoints The accessor providing the 2D image points corresponding to the object points, the image points may be distorted or undistorted depending on the usage of the distortImagePoints state
		 * @param world_R_optimizedCamera Resulting optimized 3DOF orientation
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged robust pixel error for the given initial parameters depending on the selected estimator
		 * @param finalError Optional resulting averaged robust pixel error for the final optimized parameters depending on the selected estimator
		 * @param invertedCovariances Optional set of 2x2 inverted covariance matrices that represent the uncertainties of the image points (a 2*n x 2 matrix)
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if the optimization succeeded
		 * @see optimizeOrientationIF().
		 */
		static inline bool optimizeOrientation(const AnyCamera& camera, const SquareMatrix3& world_R_camera, const ConstIndexedAccessor<ObjectPoint>& objectPoints, const ConstIndexedAccessor<ImagePoint>& imagePoints, SquareMatrix3& world_R_optimizedCamera, const unsigned int iterations, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, const Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr, const Matrix* invertedCovariances = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Minimizes the projection error of a given inverted and flipped 3DOF orientation.
		 * Beware: The given inverted and flipped 3DOF orientation is not equivalent to the rotational part of a standard extrinsic camera matrix.<br>
		 * @param camera  The camera profile defining the projection, must be valid
		 * @param flippedCamera_R_world 3DOF orientation to minimize the projection error for (inverted and flipped)
		 * @param objectPoints The accessor providing the 3D object points to be projected into the camera plane
		 * @param imagePoints The accessor providing the 2D image points corresponding to the object points, the image points may be distorted or undistorted depending on the usage of the distortImagePoints state
		 * @param optimizedFlippedCamera_R_world Resulting optimized 3DOF orientation (inverted and flipped)
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged robust pixel error for the given initial parameters depending on the selected estimator
		 * @param finalError Optional resulting averaged robust pixel error for the final optimized parameters depending on the selected estimator
		 * @param invertedCovariances Optional set of 2x2 inverted covariance matrices that represent the uncertainties of the image points (a 2*n x 2 matrix)
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if succeeded
		 * @see optimizeOrientation().
		 */
		static bool optimizeOrientationIF(const AnyCamera& camera, const SquareMatrix3& flippedCamera_R_world, const ConstIndexedAccessor<ObjectPoint>& objectPoints, const ConstIndexedAccessor<ImagePoint>& imagePoints, SquareMatrix3& optimizedFlippedCamera_R_world, const unsigned int iterations, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, const Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr, const Matrix* invertedCovariances = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Minimizes the projection error of a given 3DOF orientation and the entire camera parameters (intrinsic and distortion).
		 * The given 3DOF orientation is the rotational part of a standard extrinsic camera matrix.<br>
		 * @param pinholeCamera  The pinhole camera object defining the projection between 3D object points and 2D image points
		 * @param world_R_camera 3DOF orientation to minimize the projection error for
		 * @param objectPoints The accessor providing the 3D object points to be projected into the camera plane
		 * @param imagePoints The accessor providing the 2D image points corresponding to the object points, the image points may be distorted or undistorted depending on the usage of the distortImagePoints state
		 * @param distortImagePoints True, to force the usage of the distortion parameters of the given camera object to distort the projected 2D image points before error determination
		 * @param world_R_optimizedCamera Resulting optimized 3DOF orientation
		 * @param optimizedCamera The resulting optimized camera with modified intrinsic camera parameters and distortion parameters
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged robust pixel error for the given initial parameters depending on the selected estimator
		 * @param finalError Optional resulting averaged robust pixel error for the final optimized parameters depending on the selected estimator
		 * @param invertedCovariances Optional set of 2x2 inverted covariance matrices that represent the uncertainties of the image points (a 2*n x 2 matrix)
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if the optimization succeeded
		 * @see optimizeOrientationIF().
		 */
		static inline bool optimizeCameraOrientation(const PinholeCamera& pinholeCamera, const SquareMatrix3& world_R_camera, const ConstIndexedAccessor<ObjectPoint>& objectPoints, const ConstIndexedAccessor<ImagePoint>& imagePoints, const bool distortImagePoints, SquareMatrix3& world_R_optimizedCamera, PinholeCamera& optimizedCamera, const unsigned int iterations, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, const Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr, const Matrix* invertedCovariances = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Minimizes the projection error of a given inverted and flipped 3DOF orientation and the entire camera parameters (intrinsic and distortion).
		 * Beware: The given inverted and flipped 3DOF orientation is not equivalent to the rotational part of a standard extrinsic camera matrix.<br>
		 * @param pinholeCamera  The pinhole camera object defining the projection between 3D object points and 2D image points
		 * @param flippedCamera_R_world 3DOF orientation to minimize the projection error for (inverted and flipped)
		 * @param objectPoints The accessor providing the 3D object points to be projected into the camera plane
		 * @param imagePoints The accessor providing the 2D image points corresponding to the object points, the image points may be distorted or undistorted depending on the usage of the distortImagePoints state
		 * @param distortImagePoints True, to force the usage of the distortion parameters of the given camera object to distort the projected 2D image points before error determination
		 * @param optimizedFlippedCamera_R_world Resulting optimized 3DOF orientation (inverted and flipped)
		 * @param optimizedCamera The resulting optimized camera with modified intrinsic camera parameters and distortion parameters
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged robust pixel error for the given initial parameters depending on the selected estimator
		 * @param finalError Optional resulting averaged robust pixel error for the final optimized parameters depending on the selected estimator
		 * @param invertedCovariances Optional set of 2x2 inverted covariance matrices that represent the uncertainties of the image points (a 2*n x 2 matrix)
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if the optimization succeeded
		 * @see optimizeOrientationIF().
		 */
		static bool optimizeCameraOrientationIF(const PinholeCamera& pinholeCamera, const SquareMatrix3& flippedCamera_R_world, const ConstIndexedAccessor<ObjectPoint>& objectPoints, const ConstIndexedAccessor<ImagePoint>& imagePoints, const bool distortImagePoints, SquareMatrix3& optimizedFlippedCamera_R_world, PinholeCamera& optimizedCamera, const unsigned int iterations, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, const Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr, const Matrix* invertedCovariances = nullptr, Scalars* intermediateErrors = nullptr);
};

inline bool NonLinearOptimizationOrientation::optimizeOrientation(const AnyCamera& camera, const SquareMatrix3& world_R_camera, const ConstIndexedAccessor<ObjectPoint>& objectPoints, const ConstIndexedAccessor<ImagePoint>& imagePoints, SquareMatrix3& world_R_optimizedCamera, const unsigned int iterations, const Estimator::EstimatorType estimator, const Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, const Matrix* invertedCovariances, Scalars* intermediateErrors)
{
	ocean_assert(objectPoints.size() >= 3u && objectPoints.size() >= imagePoints.size());

	const SquareMatrix3 flippedCamera_R_world(PinholeCamera::standard2InvertedFlipped(world_R_camera));

	SquareMatrix3 optimizedFlippedCamera_R_world;
	if (!optimizeOrientationIF(camera, flippedCamera_R_world, objectPoints, imagePoints, optimizedFlippedCamera_R_world, iterations, estimator, lambda, lambdaFactor, initialError, finalError, invertedCovariances, intermediateErrors))
	{
		return false;
	}

	world_R_optimizedCamera = PinholeCamera::invertedFlipped2Standard(optimizedFlippedCamera_R_world);

	return true;
}

inline bool NonLinearOptimizationOrientation::optimizeCameraOrientation(const PinholeCamera& pinholeCamera, const SquareMatrix3& world_R_camera, const ConstIndexedAccessor<ObjectPoint>& objectPoints, const ConstIndexedAccessor<ImagePoint>& imagePoints, const bool distortImagePoints, SquareMatrix3& world_R_optimizedCamera, PinholeCamera& optimizedCamera, const unsigned int iterations, const Estimator::EstimatorType estimator, const Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, const Matrix* invertedCovariances, Scalars* intermediateErrors)
{
	ocean_assert(objectPoints.size() >= 3u && objectPoints.size() >= imagePoints.size());

	const SquareMatrix3 flippedCamera_R_world(PinholeCamera::standard2InvertedFlipped(world_R_camera));

	SquareMatrix3 optimizedFlippedCamera_R_world;
	if (!optimizeCameraOrientationIF(pinholeCamera, flippedCamera_R_world, objectPoints, imagePoints, distortImagePoints, optimizedFlippedCamera_R_world, optimizedCamera, iterations, estimator, lambda, lambdaFactor, initialError, finalError, invertedCovariances, intermediateErrors))
	{
		return false;
	}

	world_R_optimizedCamera = PinholeCamera::invertedFlipped2Standard(optimizedFlippedCamera_R_world);
	return true;
}

}

}

#endif // META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_ORIENTATION_H
