/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/NonLinearOptimizationOrientation.h"
#include "ocean/geometry/Error.h"
#include "ocean/geometry/Jacobian.h"
#include "ocean/geometry/NonLinearOptimization.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements an optimization provider for a 3DOF orientation of a 6DOF pose.
 * The optimization needs several point correspondences between 3D object points and 2D image points.<br>
 * The provider optimizes the camera pose by minimizing the projection error between 3D object and 2D image points.<br>
 */
class NonLinearOptimizationOrientation::OrientationOptimizationProvider : public NonLinearOptimization::OptimizationProvider
{
	public:

		/**
		 * Creates a new optimization provider object.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param flippedCamera_R_world Initial inverted and flipped orientation that has to be optimized
		 * @param objectPoints 3D object points that are projected into the camera frame
		 * @param imagePoints 2D observation image points, each point corresponds to one object point
		 */
		inline OrientationOptimizationProvider(const AnyCamera& camera, SquareMatrix3& flippedCamera_R_world, const ConstIndexedAccessor<ObjectPoint>& objectPoints, const ConstIndexedAccessor<ImagePoint>& imagePoints) :
			camera_(camera),
			flippedCamera_R_world_(flippedCamera_R_world),
			candidateFlippedCamera_R_world_(flippedCamera_R_world),
			objectPoints_(objectPoints),
			imagePoints_(imagePoints)
		{
			ocean_assert(objectPoints_.size() >= 3);
			ocean_assert(objectPoints_.size() == imagePoints_.size());
		};

		/**
		 * Determines the jacobian matrix for the current pose.
		 * @param jacobian Jacobian matrix
		 */
		inline void determineJacobian(Matrix& jacobian) const
		{
			/**
			 * Function with Rotation, Projection and de-Homogenization:
			 * fDPRT(X, p) = x
			 * with X as 3D object position, p as 3D pose information (3D for exponential map rotation) and x as 2D image position
			 *
			 * Update iterations:
			 * p_i+1 = p_i + delta_i
			 * with p_i the initial pose, p_i+1 the updated pose and delta_i the correction offset
			 *
			 * Determination of delta_i:
			 * delta_i = -J^+ * error_i
			 * with J^+ the pseudo inverse of J, [where J^+ = (J^T * J)^-1 * J^T ] and error_i the projection error
			 *
			 * Determination of error_i:
			 * error_i = fDPRT(X, p_i) - x'
			 * with fDPRT(X, p_i) is the projected 2D image point using the current pose and x' the measurement value
			 */

			/**
			 * Jacobian matrix of the projection function evaluated at current pose, dimension 2n x 6
			 * Image point: ip
			 * [ ip1_x / dwx, ip1_x / dwy, ip1_x / dwz ]
			 * [ ip1_y / dwx, ip1_y / dwy, ip1_y / dwz ]
			 * [ ..........,               ........... ]
			 * [ ..........,               ........... ]
			 * [ ipn_x / dwx, ipn_x / dwy, ipn_x / dwz ]
			 * [ ipn_y / dwx, ipn_y / dwy, ipn_y / dwz ]
			 */

			jacobian.resize(objectPoints_.size() * 2, 3u);

			Jacobian::calculateOrientationJacobianRodrigues2nx3IF(jacobian.data(), camera_, ExponentialMap(flippedCamera_R_world_), objectPoints_);
		}

		/**
		 * Applies the pose correction and stores the new pose as candidate
		 * @param deltas Optimization deltas that define the correction
		 */
		inline void applyCorrection(const Matrix& deltas)
		{
			ocean_assert(deltas.rows() == 3 && deltas.columns() == 1);

			const Pose pose = Pose(Rotation(flippedCamera_R_world_));

			const Pose deltaPose(0, 0, 0, deltas(0), deltas(1), deltas(2));
			const Pose newPose(pose - deltaPose);

			candidateFlippedCamera_R_world_ = newPose.transformation().rotationMatrix();
		}

		/**
		 * Determines the robust error of the current candidate pose.
		 * @param weightedErrorVector Resulting (weighted - if using a robust estimator) error vector
		 * @param weightVector Vector holding the weights that have already been applied to the error vector
		 * @param invertedCovariances Optional 2x2 inverted covariance matrices
		 */
		template <Estimator::EstimatorType tEstimator>
		Scalar determineRobustError(Matrix& weightedErrorVector, Matrix& weightVector, const Matrix* invertedCovariances) const
		{
			// set the correct size of the resulting error vector
			weightedErrorVector.resize(2 * objectPoints_.size(), 1);

			Vector2* const weightedErrors = (Vector2*)weightedErrorVector.data();
			const SquareMatrix2* transposedInvertedCovariances = invertedCovariances ? (SquareMatrix2*)invertedCovariances->data() : nullptr;

			// check whether the standard estimator is used
			if constexpr (Estimator::isStandardEstimator<tEstimator>())
			{
				// the weight vector should be and should stay invalid
				ocean_assert(!weightVector);

				// return the averaged square error
				Scalar sqrError = Error::determinePoseErrorIF<ConstIndexedAccessor<ObjectPoint>, ConstIndexedAccessor<ImagePoint>, true, false>(HomogenousMatrix4(candidateFlippedCamera_R_world_), camera_, objectPoints_, imagePoints_, weightedErrors);

				if (transposedInvertedCovariances != nullptr)
				{
					sqrError = 0;
					for (size_t n = 0; n < objectPoints_.size(); ++n)
					{
						sqrError += (transposedInvertedCovariances[n].transposed() * weightedErrors[n]).sqr();
					}

					sqrError /= Scalar(objectPoints_.size());
				}

				return sqrError;
			}
			else
			{
				// now we need the weight vector
				weightVector.resize(2 * objectPoints_.size(), 1);

				Scalars sqrErrors(objectPoints_.size());
				Error::determinePoseErrorIF<ConstIndexedAccessor<ObjectPoint>, ConstIndexedAccessor<ImagePoint>, true, true>(HomogenousMatrix4(candidateFlippedCamera_R_world_), camera_, objectPoints_, imagePoints_, weightedErrors, sqrErrors.data());
				return sqrErrors2robustErrors2<tEstimator>(sqrErrors, 3, weightedErrors, (Vector2*)weightVector.data(), transposedInvertedCovariances);
			}
		}

		/**
		 * Accepts the current pose candidate as better model.
		 */
		inline void acceptCorrection()
		{
			flippedCamera_R_world_ = candidateFlippedCamera_R_world_;
		}

	protected:

		/// The camera object.
		const AnyCamera& camera_;

		/// Inverted and flipped pose that will be optimized.
		SquareMatrix3& flippedCamera_R_world_;

		/// Intermediate inverted and flipped pose that stores the most recent optimization result as candidate.
		SquareMatrix3 candidateFlippedCamera_R_world_;

		/// The 3D object points that are used for the optimization.
		const ConstIndexedAccessor<ObjectPoint>& objectPoints_;

		/// The observed 2D image points.
		const ConstIndexedAccessor<ImagePoint>& imagePoints_;
};

bool NonLinearOptimizationOrientation::optimizeOrientationIF(const AnyCamera& camera, const SquareMatrix3& flippedCamera_R_world, const ConstIndexedAccessor<ObjectPoint>& objectPoints, const ConstIndexedAccessor<ImagePoint>& imagePoints, SquareMatrix3& optimizedFlippedCamera_R_world, const unsigned int iterations, const Estimator::EstimatorType estimator, const Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, const Matrix* invertedCovariances, Scalars* intermediateErrors)
{
	ocean_assert(camera.isValid());
	ocean_assert(!flippedCamera_R_world.isSingular());
	ocean_assert(&flippedCamera_R_world != &optimizedFlippedCamera_R_world);

	optimizedFlippedCamera_R_world = flippedCamera_R_world;

	OrientationOptimizationProvider provider(camera, optimizedFlippedCamera_R_world, objectPoints, imagePoints);

	return denseOptimization<OrientationOptimizationProvider>(provider, iterations, estimator, lambda, lambdaFactor, initialError, finalError, invertedCovariances, intermediateErrors);
}

/**
 * This class implements an optimization provider for a 3DOF orientation (of a 6DOF pose) and camera parameters (intrinsic and distortion).
 * The optimization needs several point correspondences between 3D object points and 2D image points.<br>
 * The provider optimizes the camera pose by minimizing the projection error between 3D object and 2D image points.<br>
 */
class NonLinearOptimizationOrientation::CameraOrientationOptimizationProvider : public NonLinearOptimization::OptimizationProvider
{
	public:

		/**
		 * Creates a new optimization provider object.
		 * @param pinholeCamera The pinhole camera object to be used
		 * @param flippedCamera_R_world Initial inverted and flipped orientation that has to be optimized
		 * @param objectPoints 3D object points that are projected into the camera frame
		 * @param imagePoints 2D observation image points, each point corresponds to one object point
		 * @param distortImagePoints True, to apply the distortion parameters of the camera
		 */
		inline CameraOrientationOptimizationProvider(PinholeCamera& pinholeCamera, SquareMatrix3& flippedCamera_R_world, const ConstIndexedAccessor<ObjectPoint>& objectPoints, const ConstIndexedAccessor<ImagePoint>& imagePoints, const bool distortImagePoints) :
			camera_(pinholeCamera),
			candidateCamera_(pinholeCamera),
			flippedCamera_R_world_(flippedCamera_R_world),
			candidateFlippedCamera_R_world_(flippedCamera_R_world),
			objectPoints_(objectPoints),
			imagePoints_(imagePoints),
			distortImagePoints_(distortImagePoints)
		{
			ocean_assert(objectPoints_.size() >= 3);
			ocean_assert(objectPoints_.size() == imagePoints_.size());
		};

		/**
		 * Determines the jacobian matrix for the current pose.
		 * @param jacobian Jacobian matrix
		 */
		inline void determineJacobian(Matrix& jacobian) const
		{
			jacobian.resize(objectPoints_.size() * 2, 11);

			Jacobian::calculateOrientationCameraJacobianRodrigues2nx11(jacobian.data(), camera_, Pose(Rotation(flippedCamera_R_world_)), objectPoints_);
		}

		/**
		 * Applies the pose correction and stores the new pose as candidate
		 * @param deltas Optimization deltas that define the correction
		 */
		inline void applyCorrection(const Matrix& deltas)
		{
			ocean_assert(deltas.rows() == 4 + 7 && deltas.columns() == 1);

			const Pose pose = Pose(Rotation(flippedCamera_R_world_));

			PinholeCamera pinholeCamera(camera_);
			SquareMatrix3 intrinsic(pinholeCamera.intrinsic());
			intrinsic(0, 0) -= deltas(7);
			intrinsic(1, 1) -= deltas(8);
			intrinsic(0, 2) -= deltas(9);
			intrinsic(1, 2) -= deltas(10);
			pinholeCamera.setIntrinsic(intrinsic);

			pinholeCamera.setRadialDistortion(std::make_pair(pinholeCamera.radialDistortion().first - deltas(3), pinholeCamera.radialDistortion().second - deltas(4)));
			pinholeCamera.setTangentialDistortion(std::make_pair(pinholeCamera.tangentialDistortion().first - deltas(5), pinholeCamera.tangentialDistortion().second - deltas(6)));

			ocean_assert(candidateCamera_.principalPointX() >= 0 && candidateCamera_.principalPointY() >= 0);
			ocean_assert(candidateCamera_.principalPointX() < Scalar(candidateCamera_.width()) && candidateCamera_.principalPointY() < Scalar(candidateCamera_.height()));
			ocean_assert(candidateCamera_.focalLengthX() > 0 && candidateCamera_.focalLengthY() > 0);

			const Pose deltaPose(0, 0, 0, deltas(0), deltas(1), deltas(2));
			const Pose newPose(pose - deltaPose);

			candidateFlippedCamera_R_world_ = newPose.transformation().rotationMatrix();
			candidateCamera_ = pinholeCamera;
		}

		/**
		 * Determines the robust error of the current candidate pose.
		 * @param weightedErrorVector Resulting (weighted - if using a robust estimator) error vector
		 * @param weightVector Vector holding the weights that have already been applied to the error vector
		 * @param invertedCovariances Optional 2x2 inverted covariance matrices
		 */
		template <Estimator::EstimatorType tEstimator>
		Scalar determineRobustError(Matrix& weightedErrorVector, Matrix& weightVector, const Matrix* invertedCovariances) const
		{
			if (candidateCamera_.principalPointX() < 0 || candidateCamera_.principalPointY() < 0
					|| candidateCamera_.principalPointX() >= Scalar(candidateCamera_.width()) || candidateCamera_.principalPointY() >= Scalar(candidateCamera_.height())
					|| candidateCamera_.focalLengthX() <= 0 || candidateCamera_.focalLengthY() <= 0)
			{
				return Numeric::maxValue();
			}

			// set the correct size of the resulting error vector
			weightedErrorVector.resize(2 * objectPoints_.size(), 1);

			Vector2* const weightedErrors = (Vector2*)weightedErrorVector.data();
			const SquareMatrix2* transposedInvertedCovariances = invertedCovariances ? (SquareMatrix2*)invertedCovariances->data() : nullptr;

			// check whether the standard estimator is used
			if constexpr (Estimator::isStandardEstimator<tEstimator>())
			{
				// the weight vector should be and should stay invalid
				ocean_assert(!weightVector);

				// return the averaged square error
				Scalar sqrError = Error::determinePoseErrorIF<ConstIndexedAccessor<ObjectPoint>, ConstIndexedAccessor<ImagePoint>, true, true, false>(HomogenousMatrix4(candidateFlippedCamera_R_world_), candidateCamera_, objectPoints_, imagePoints_, distortImagePoints_, Scalar(1), weightedErrors);

				if (transposedInvertedCovariances != nullptr)
				{
					sqrError = 0;
					for (size_t n = 0; n < objectPoints_.size(); ++n)
					{
						sqrError += (transposedInvertedCovariances[n].transposed() * weightedErrors[n]).sqr();
					}

					sqrError /= Scalar(objectPoints_.size());
				}

				return sqrError;
			}
			else
			{
				// now we need the weight vector
				weightVector.resize(2 * objectPoints_.size(), 1);

				Scalars sqrErrors(objectPoints_.size());
				Error::determinePoseErrorIF<ConstIndexedAccessor<ObjectPoint>, ConstIndexedAccessor<ImagePoint>, true, true, true>(HomogenousMatrix4(candidateFlippedCamera_R_world_), candidateCamera_, objectPoints_, imagePoints_, distortImagePoints_, Scalar(1), weightedErrors, sqrErrors.data());
				return sqrErrors2robustErrors2<tEstimator>(sqrErrors, 11, weightedErrors, (Vector2*)weightVector.data(), transposedInvertedCovariances);
			}
		}

		/**
		 * Accepts the current pose candidate as better model.
		 */
		inline void acceptCorrection()
		{
			flippedCamera_R_world_ = candidateFlippedCamera_R_world_;
			camera_ = candidateCamera_;
		}

	protected:

		/// The camera object.
		PinholeCamera& camera_;

		/// Intermediate camera profile that stores the most recent optimization result as candidate.
		PinholeCamera candidateCamera_;

		/// Inverted and flipped orientation that will be optimized.
		SquareMatrix3& flippedCamera_R_world_;

		/// Intermediate inverted and flipped orientation that stores the most recent optimization result as candidate.
		SquareMatrix3 candidateFlippedCamera_R_world_;

		/// The 3D object points that are used for the optimization.
		const ConstIndexedAccessor<ObjectPoint>& objectPoints_;

		/// The observed 2D image points.
		const ConstIndexedAccessor<ImagePoint>& imagePoints_;

		/// True, to use the camera distortion parameters.
		const bool distortImagePoints_;
};

bool NonLinearOptimizationOrientation::optimizeCameraOrientationIF(const PinholeCamera& pinholeCamera, const SquareMatrix3& flippedCamera_R_world, const ConstIndexedAccessor<ObjectPoint>& objectPoints, const ConstIndexedAccessor<ImagePoint>& imagePoints, const bool distortImagePoints, SquareMatrix3& optimizedFlippedCamera_R_world, PinholeCamera& optimizedCamera, const unsigned int iterations, const Estimator::EstimatorType estimator, const Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, const Matrix* invertedCovariances, Scalars* intermediateErrors)
{
	ocean_assert(pinholeCamera.isValid() && !flippedCamera_R_world.isSingular());
	ocean_assert(&flippedCamera_R_world != &optimizedFlippedCamera_R_world);

	optimizedFlippedCamera_R_world = flippedCamera_R_world;
	optimizedCamera = pinholeCamera;

	CameraOrientationOptimizationProvider provider(optimizedCamera, optimizedFlippedCamera_R_world, objectPoints, imagePoints, distortImagePoints);
	return denseOptimization<CameraOrientationOptimizationProvider>(provider, iterations, estimator, lambda, lambdaFactor, initialError, finalError, invertedCovariances, intermediateErrors);
}

}

}
