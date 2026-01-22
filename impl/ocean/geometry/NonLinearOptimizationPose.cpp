/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/NonLinearOptimizationPose.h"
#include "ocean/geometry/Error.h"
#include "ocean/geometry/Jacobian.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements an optimization provider for a 6DOF pose.
 * The optimization needs several point correspondences between 3D object points and 2D image points.<br>
 * The provider optimizes the camera pose by minimizing the projection error between 3D object and 2D image points.
 * @see NonLinearOptimizationPose::AdvancedPoseOptimizationProvider.
 */
class NonLinearOptimizationPose::PoseOptimizationProvider : public NonLinearOptimization::OptimizationProvider
{
	public:

		/**
		 * Creates a new optimization provider object.
		 * @param pinholeCamera The pinhole camera object to be used
		 * @param flippedCamera_P_world Initial inverted and flipped pose that has to be optimized
		 * @param objectPoints 3D object points that are projected into the camera frame
		 * @param imagePoints 2D observation image points, each point corresponds to one object point
		 * @param correspondences Number of points correspondences
		 * @param gravityConstraints Optional gravity constraints to force the optimization to create a camera pose aligned with gravity, nullptr to avoid any gravity alignment
		 */
		inline PoseOptimizationProvider(const AnyCamera& camera, Pose& flippedCamera_P_world, const Vector3* objectPoints, const Vector2* imagePoints, const size_t correspondences, const GravityConstraints* gravityConstraints) :
			camera_(camera),
			flippedCamera_P_world_(flippedCamera_P_world),
			candidateFlippedCamera_P_world_(flippedCamera_P_world),
			objectPoints_(objectPoints),
			imagePoints_(imagePoints),
			correspondences_(correspondences),
			gravityConstraints_(gravityConstraints)
		{
			ocean_assert(correspondences_ >= 3);

			if (gravityConstraints_ != nullptr)
			{
				constexpr Scalar fixedFactor = Scalar(1000); // fixed factor to balance projection error and gravity error

				gravityWeight_ = Numeric::sqrt(Scalar(correspondences_)) * fixedFactor * gravityConstraints_->weightFactor();
			}
		};

		/**
		 * Determines the jacobian matrix for the current pose.
		 * @param jacobian Jacobian matrix
		 */
		inline void determineJacobian(Matrix& jacobian) const
		{
			/**
			 * Function with Translation, Rotation, Projection and de-Homogenization:
			 * fDPRT(X, p) = x
			 * with X as 3D object position, p as 6D pose information (3D for exponential map rotation 3D for translation) and x as 2D image position
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
			 * Jacobian matrix of the projection function evaluated at current pose, dimension (2n + 3*k) x 6
			 * where k is 1 if gravity constraints are present, otherwise 0
			 * Image point: ip
			 * [ ip1_x / dwx, ip1_x / dwy, ip1_x / dwz, ip1_x / dx, ip1_x / dy, ip1_x / dz ]
			 * [ ip1_y / dwx, ip1_y / dwy, ip1_y / dwz, ip1_y / dx, ip1_y / dy, ip1_y / dz ]
			 * [ ..........,                                                   ........... ]
			 * [ ..........,                                                   ........... ]
			 * [ ipn_x / dwx, ipn_x / dwy, ipn_x / dwz, ipn_x / dx, ipn_x / dy, ipn_x / dz ]
			 * [ ipn_y / dwx, ipn_y / dwy, ipn_y / dwz, ipn_y / dx, ipn_y / dy, ipn_y / dz ]
			 *
			 * Optional gravity constraints:
			 * [ g_x / dwx,   g_x / dwy,   g_x / dwz,   0,          0,          0          ]
			 * [ g_y / dwx,   g_y / dwy,   g_y / dwz,   0,          0,          0          ]
			 * [ g_z / dwx,   g_z / dwy,   g_z / dwz,   0,          0,          0          ]
			 */

			const size_t gravityRows = (gravityConstraints_ != nullptr) ? 3u : 0u;
			jacobian.resize(correspondences_ * 2 + gravityRows, 6);

			Jacobian::calculatePoseJacobianRodrigues2nx6IF(jacobian.data(), camera_, flippedCamera_P_world_, objectPoints_, correspondences_);

			if (gravityConstraints_ != nullptr)
			{
				const ExponentialMap exponentialMap(Vector3(flippedCamera_P_world_.rx(), flippedCamera_P_world_.ry(), flippedCamera_P_world_.rz()));

				SquareMatrix3 Rwx, Rwy, Rwz;
				Jacobian::calculateRotationRodriguesDerivative(exponentialMap, Rwx, Rwy, Rwz);

				// the gravity error Jacobian with respect to rotation is:
				// d(R * g_world) / dwi = Rwi * g_world
				const Vector3 dRotGravity_dwx = Rwx * gravityConstraints_->worldGravityInWorld();
				const Vector3 dRotGravity_dwy = Rwy * gravityConstraints_->worldGravityInWorld();
				const Vector3 dRotGravity_dwz = Rwz * gravityConstraints_->worldGravityInWorld();

				const size_t gravityRowOffset = correspondences_ * 2;

				jacobian(gravityRowOffset + 0, 0) = dRotGravity_dwx.x() * gravityWeight_;
				jacobian(gravityRowOffset + 0, 1) = dRotGravity_dwy.x() * gravityWeight_;
				jacobian(gravityRowOffset + 0, 2) = dRotGravity_dwz.x() * gravityWeight_;
				jacobian(gravityRowOffset + 0, 3) = 0;
				jacobian(gravityRowOffset + 0, 4) = 0;
				jacobian(gravityRowOffset + 0, 5) = 0;

				jacobian(gravityRowOffset + 1, 0) = dRotGravity_dwx.y() * gravityWeight_;
				jacobian(gravityRowOffset + 1, 1) = dRotGravity_dwy.y() * gravityWeight_;
				jacobian(gravityRowOffset + 1, 2) = dRotGravity_dwz.y() * gravityWeight_;
				jacobian(gravityRowOffset + 1, 3) = 0;
				jacobian(gravityRowOffset + 1, 4) = 0;
				jacobian(gravityRowOffset + 1, 5) = 0;

				jacobian(gravityRowOffset + 2, 0) = dRotGravity_dwx.z() * gravityWeight_;
				jacobian(gravityRowOffset + 2, 1) = dRotGravity_dwy.z() * gravityWeight_;
				jacobian(gravityRowOffset + 2, 2) = dRotGravity_dwz.z() * gravityWeight_;
				jacobian(gravityRowOffset + 2, 3) = 0;
				jacobian(gravityRowOffset + 2, 4) = 0;
				jacobian(gravityRowOffset + 2, 5) = 0;
			}
		}

		/**
		 * Applies the pose correction and stores the new pose as candidate
		 * @param deltas Optimization deltas that define the correction
		 */
		inline void applyCorrection(const Matrix& deltas)
		{
			ocean_assert(deltas.rows() == 6u && deltas.columns() == 1u);

			const Pose deltaPose(deltas(3), deltas(4), deltas(5), deltas(0), deltas(1), deltas(2));
			const Pose newFlippedCamera_P_world(flippedCamera_P_world_ - deltaPose);

			candidateFlippedCamera_P_world_ = newFlippedCamera_P_world;
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
			const HomogenousMatrix4 candidateFlippedCamera_T_world(candidateFlippedCamera_P_world_.transformation());

			const size_t gravityRows = (gravityConstraints_ != nullptr) ? 3u : 0u;

			// set the correct size of the resulting error vector
			weightedErrorVector.resize(2 * correspondences_ + gravityRows, 1u);

			Vector2* const weightedErrors = (Vector2*)(weightedErrorVector.data());
			const SquareMatrix2* transposedInvertedCovariances = invertedCovariances ? (SquareMatrix2*)invertedCovariances->data() : nullptr;

			Scalar averageRobustError = Numeric::maxValue();

			// check whether the standard estimator is used
			if constexpr (Estimator::isStandardEstimator<tEstimator>())
			{
				// the weight vector should be and should stay invalid
				ocean_assert(!weightVector);

				// determine the averaged square error
				averageRobustError = Error::determinePoseErrorIF<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, false>(candidateFlippedCamera_T_world, camera_, ConstTemplateArrayAccessor<Vector3>(objectPoints_, correspondences_), ConstTemplateArrayAccessor<Vector2>(imagePoints_, correspondences_), weightedErrors);

				if (transposedInvertedCovariances != nullptr)
				{
					averageRobustError = 0;
					for (size_t n = 0; n < correspondences_; ++n)
					{
						averageRobustError += (transposedInvertedCovariances[n].transposed() * weightedErrors[n]).sqr();
					}

					averageRobustError /= Scalar(correspondences_);
				}
			}
			else
			{
				// now we need the weight vector
				weightVector.resize(2 * correspondences_ + gravityRows, 1u);

				Scalars sqrErrors(correspondences_);
				Error::determinePoseErrorIF<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, true>(candidateFlippedCamera_T_world, camera_, ConstTemplateArrayAccessor<Vector3>(objectPoints_, correspondences_), ConstTemplateArrayAccessor<Vector2>(imagePoints_, correspondences_), weightedErrors, sqrErrors.data());

				averageRobustError = sqrErrors2robustErrors2<tEstimator>(sqrErrors, 6, weightedErrors, (Vector2*)(weightVector.data()), transposedInvertedCovariances);
			}

			if (gravityConstraints_ != nullptr)
			{
				const Vector3 worldGravityInFlippedCamera = gravityConstraints_->worldGravityInFlippedCameraIF(candidateFlippedCamera_T_world);
				const Vector3 cameraGravityInFlippedCamera = gravityConstraints_->cameraGravityInFlippedCamera();

				const Vector3 gravityError = worldGravityInFlippedCamera - cameraGravityInFlippedCamera;

				const size_t gravityOffset = 2 * correspondences_;

				weightedErrorVector(gravityOffset + 0) = gravityError.x() * gravityWeight_;
				weightedErrorVector(gravityOffset + 1) = gravityError.y() * gravityWeight_;
				weightedErrorVector(gravityOffset + 2) = gravityError.z() * gravityWeight_;

				if constexpr (!Estimator::isStandardEstimator<tEstimator>())
				{
					ocean_assert(weightVector);
					ocean_assert(gravityRows == 3);
					ocean_assert(weightVector.rows() == correspondences_ * 2 + gravityRows);

					weightVector(gravityOffset + 0) = 1;
					weightVector(gravityOffset + 1) = 1;
					weightVector(gravityOffset + 2) = 1;
				}

				const Scalar gravitySqrError = Numeric::sqr(gravityError.length() * gravityWeight_);

				// averageRobustError is already normalized wrt to number of correspondences (it's the average sqr pixel error for a square estimator), so we first have to de-normalize this error
				const Scalar sumRobustError = averageRobustError * Scalar(correspondences_);

				// we do not apply a robust error estimation for the gravity error, the weight factor is supposed to apply some kind of robustness

				averageRobustError = (sumRobustError + gravitySqrError) / Scalar(correspondences_ + 1);
			}

			return averageRobustError;
		}

		/**
		 * Accepts the current pose candidate as better model.
		 */
		inline void acceptCorrection()
		{
			flippedCamera_P_world_ = candidateFlippedCamera_P_world_;
		}

	protected:

		/// The camera object.
		const AnyCamera& camera_;

		/// Inverted and flipped pose that will be optimized.
		Pose& flippedCamera_P_world_;

		/// Intermediate inverted and flipped pose that stores the most recent optimization result as candidate.
		Pose candidateFlippedCamera_P_world_;

		/// The 3D object points that are used for the optimization.
		const Vector3* objectPoints_;

		/// The observed 2D image points.
		const Vector2* imagePoints_;

		/// Number of points correspondences.
		const size_t correspondences_;

		/// The optional gravity constraints.
		const GravityConstraints* gravityConstraints_ = nullptr;

		/// The constant weight factor for the gravity constraints.
		Scalar gravityWeight_ = Scalar(-1);
};

/**
 * This class implements an advanced optimization provider for a 6DOF pose and any camera.
 * This advanced optimized determine the Hessian and (transposed-)jacobian-error vector on it's own using not by multiplying the transposed Jacobian with the Jacobian but by accumulating the Hessian for the individual Jacobian rows directly.<br>
 * However, this advanced optimizer currently does not support individual weight or covariance values.<br>
 * The optimization needs several point correspondences between 3D object points and 2D image points.<br>
 * The provider optimizes the camera pose by minimizing the projection error between 3D object and 2D image points.
 * @see NonLinearOptimizationPose::PoseOptimizationProvider
 */
class NonLinearOptimizationPose::AdvancedAnyCameraPoseOptimizationProvider : public NonLinearOptimization::AdvancedDenseOptimizationProvider
{
	public:

		/**
		 * Creates a new advanced optimization provider object.
		 * @param anyCamera The camera profile defining the projection, must be valid
		 * @param flippedCamera_P_world Initial inverted and flipped pose that has to be optimized
		 * @param objectPoints 3D object points that are projected into the camera frame
		 * @param imagePoints 2D observation image points, each point corresponds to one object point
		 * @param correspondences Number of points correspondences
		 * @param estimator The estimator type to be used as error measure
		 * @param gravityConstraints Optional gravity constraints to force the optimization to create a camera pose aligned with gravity, nullptr to avoid any gravity alignment
		 */
		inline AdvancedAnyCameraPoseOptimizationProvider(const AnyCamera& anyCamera, Pose& flippedCamera_T_world, const Vector3* objectPoints, const Vector2* imagePoints, const size_t correspondences, const Estimator::EstimatorType estimator, const GravityConstraints* gravityConstraints) :
			anyCamera_(anyCamera),
			flippedCamera_P_world_(flippedCamera_T_world),
			candidateFlippedCamera_P_world_(flippedCamera_T_world),
			objectPoints_(objectPoints),
			imagePoints_(imagePoints),
			correspondences_(correspondences),
			estimator_(estimator),
			gravityConstraints_(gravityConstraints)
		{
			ocean_assert(correspondences_ >= 3);

			if (gravityConstraints_ != nullptr)
			{
				constexpr Scalar fixedFactor = Scalar(1000); // fixed factor to balance projection error and gravity error

				gravityWeight_ = Numeric::sqrt(Scalar(correspondences_)) * fixedFactor * gravityConstraints_->weightFactor();
			}
		};

		/**
		 * Determines the error for the current model candidate (not the actual model).
		 * @see AdvancedDenseOptimizationProvider::determineError().
		 */
		inline Scalar determineError()
		{
			// now we determine the current error for the candidate pose
			// and we further store the individual errors and weights for the next Hessian (and Jacobian-error) determination during the next call of determineHessianAndErrorJacobian()

			const HomogenousMatrix4 candidateFlippedCamera_T_world(candidateFlippedCamera_P_world_.transformation());

			// set the correct size of the resulting error vector
			weightedErrors_.resize(correspondences_);

			Scalar averageRobustError = Numeric::maxValue();

			// check whether the standard estimator is used
			if (estimator_ == Estimator::ET_SQUARE)
			{
				// the weight vector should be and should stay invalid
				ocean_assert(weights_.empty());

				// determine the averaged square error
				averageRobustError = Error::determinePoseErrorIF<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, false>(candidateFlippedCamera_T_world, anyCamera_, ConstTemplateArrayAccessor<Vector3>(objectPoints_, correspondences_), ConstTemplateArrayAccessor<Vector2>(imagePoints_, correspondences_), weightedErrors_.data());
			}
			else
			{
				// now we need the weight vector
				weights_.resize(correspondences_);

				Scalars sqrErrors(correspondences_);
				Error::determinePoseErrorIF<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, true>(candidateFlippedCamera_T_world, anyCamera_, ConstTemplateArrayAccessor<Vector3>(objectPoints_, correspondences_), ConstTemplateArrayAccessor<Vector2>(imagePoints_, correspondences_), weightedErrors_.data(), sqrErrors.data());

				averageRobustError = NonLinearOptimization::sqrErrors2robustErrors2(estimator_, sqrErrors, 6, weightedErrors_.data(), weights_.data(), nullptr);
			}

			if (gravityConstraints_ != nullptr)
			{
				const Vector3 worldGravityInFlippedCamera = gravityConstraints_->worldGravityInFlippedCameraIF(candidateFlippedCamera_T_world);
				const Vector3 cameraGravityInFlippedCamera = gravityConstraints_->cameraGravityInFlippedCamera();

				const Vector3 gravityError = worldGravityInFlippedCamera - cameraGravityInFlippedCamera;

				weightedErrorGravity_ = gravityError * gravityWeight_;

				const Scalar gravitySqrError = Numeric::sqr(gravityError.length() * gravityWeight_);

				// averageRobustError is already normalized wrt to number of correspondences (it's the average sqr pixel error for a square estimator), so we first have to de-normalize this error
				const Scalar sumRobustError = averageRobustError * Scalar(correspondences_);

				// we do not apply a robust error estimation for the gravity error, the weight factor is supposed to apply some kind of robustness

				averageRobustError = (sumRobustError + gravitySqrError) / Scalar(correspondences_ + 1);
			}

			return averageRobustError;
		}

		/**
		 * Determines the Hessian matrix and the Error-Jacobian vector based on the actual/current model (the transposed jacobian multiplied with the individual errors).
		 * @see AdvancedDenseOptimizationProvider::determineHessianAndError().
		 */
		inline bool determineHessianAndErrorJacobian(Matrix& hessian, Matrix& jacobianError)
		{
			hessian.resize(6, 6);
			jacobianError = Matrix(6, 1, false);

			SquareMatrix3 Rwx, Rwy, Rwz;
			Jacobian::calculateRotationRodriguesDerivative(ExponentialMap(Vector3(flippedCamera_P_world_.rx(), flippedCamera_P_world_.ry(), flippedCamera_P_world_.rz())), Rwx, Rwy, Rwz);

			ocean_assert(weightedErrors_.size() == correspondences_);
			ocean_assert(estimator_ == Estimator::ET_SQUARE || weights_.size() == correspondences_);

			const HomogenousMatrix4 flippedCamera_T_world(flippedCamera_P_world_.transformation());

			// see Jacobian::calculatePoseJacobianRodrigues2nx6() provided more detailed about the equations

			/**
				* Determination of the symmetric matrix:
				*
				* | A |                     | AA BA CA DA EA FA |
				* | B |                     | AB BB CB DB EB FB |
				* | C |                     | AC BC CC DC EC FC |
				* | D | * | A B C D E F | = | AD BD CD DD ED FD |
				* | E |                     | AE BE CE DE EE FE |
				* | F |                     | AF BF CF DF EF FF |
				*
				* We will determine the upper right triangle only, and store the results in a separate array:
				*
				* | AA BA CA DA EA FA |     |  0  1  2  3  4  5 |
				* | .. BB CB DB EB FB |     | ..  6  7  8  9 10 |
				* | .. .. CC DC EC FC |     | .. .. 11 12 13 14 |
				* | .. .. .. DD ED FD | ... | .. .. .. 15 16 17 |
				* | .. .. .. .. EE FE |     | .. .. .. .. 18 19 |
				* | .. .. .. .. .. FF |     | .. .. .. .. .. 20 |
				*
				* We determine the jacobian error by:
				*
				* | A |
				* | B |
				* | C |
				* | D | * | error |
				* | E |
				* | F |
				*/

			Scalar xJacobian[6];
			Scalar yJacobian[6];

			Scalar hessianValues[21] = {Scalar(0)};

			if (weights_.empty())
			{
				 // we use ET_SQUARE as estimator

				ocean_assert(weights_.empty());
				ocean_assert(weightedErrors_.size() == correspondences_);

				for (size_t n = 0; n < correspondences_; ++n)
				{
					Jacobian::calculatePoseJacobianRodrigues2x6IF(anyCamera_, flippedCamera_T_world, objectPoints_[n], Rwx, Rwy, Rwz, xJacobian, yJacobian);

					const Vector2& weightedError = weightedErrors_[n];

					hessianValues[ 0] += xJacobian[0] * xJacobian[0] + yJacobian[0] * yJacobian[0];
					hessianValues[ 1] += xJacobian[0] * xJacobian[1] + yJacobian[0] * yJacobian[1];
					hessianValues[ 2] += xJacobian[0] * xJacobian[2] + yJacobian[0] * yJacobian[2];
					hessianValues[ 3] += xJacobian[0] * xJacobian[3] + yJacobian[0] * yJacobian[3];
					hessianValues[ 4] += xJacobian[0] * xJacobian[4] + yJacobian[0] * yJacobian[4];
					hessianValues[ 5] += xJacobian[0] * xJacobian[5] + yJacobian[0] * yJacobian[5];

					hessianValues[ 6] += xJacobian[1] * xJacobian[1] + yJacobian[1] * yJacobian[1];
					hessianValues[ 7] += xJacobian[1] * xJacobian[2] + yJacobian[1] * yJacobian[2];
					hessianValues[ 8] += xJacobian[1] * xJacobian[3] + yJacobian[1] * yJacobian[3];
					hessianValues[ 9] += xJacobian[1] * xJacobian[4] + yJacobian[1] * yJacobian[4];
					hessianValues[10] += xJacobian[1] * xJacobian[5] + yJacobian[1] * yJacobian[5];

					hessianValues[11] += xJacobian[2] * xJacobian[2] + yJacobian[2] * yJacobian[2];
					hessianValues[12] += xJacobian[2] * xJacobian[3] + yJacobian[2] * yJacobian[3];
					hessianValues[13] += xJacobian[2] * xJacobian[4] + yJacobian[2] * yJacobian[4];
					hessianValues[14] += xJacobian[2] * xJacobian[5] + yJacobian[2] * yJacobian[5];

					hessianValues[15] += xJacobian[3] * xJacobian[3] + yJacobian[3] * yJacobian[3];
					hessianValues[16] += xJacobian[3] * xJacobian[4] + yJacobian[3] * yJacobian[4];
					hessianValues[17] += xJacobian[3] * xJacobian[5] + yJacobian[3] * yJacobian[5];

					hessianValues[18] += xJacobian[4] * xJacobian[4] + yJacobian[4] * yJacobian[4];
					hessianValues[19] += xJacobian[4] * xJacobian[5] + yJacobian[4] * yJacobian[5];

					hessianValues[20] += xJacobian[5] * xJacobian[5] + yJacobian[5] * yJacobian[5];

					jacobianError(0) += xJacobian[0] * weightedError.x() + yJacobian[0] * weightedError.y();
					jacobianError(1) += xJacobian[1] * weightedError.x() + yJacobian[1] * weightedError.y();
					jacobianError(2) += xJacobian[2] * weightedError.x() + yJacobian[2] * weightedError.y();
					jacobianError(3) += xJacobian[3] * weightedError.x() + yJacobian[3] * weightedError.y();
					jacobianError(4) += xJacobian[4] * weightedError.x() + yJacobian[4] * weightedError.y();
					jacobianError(5) += xJacobian[5] * weightedError.x() + yJacobian[5] * weightedError.y();
				}
			}
			else
			{
				// we use a non-ET_SQUARE estimator

				ocean_assert(weights_.size() == correspondences_);
				ocean_assert(weightedErrors_.size() == correspondences_);

				for (size_t n = 0; n < correspondences_; ++n)
				{
					Jacobian::calculatePoseJacobianRodrigues2x6IF(anyCamera_, flippedCamera_T_world, objectPoints_[n], Rwx, Rwy, Rwz, xJacobian, yJacobian);

					const Vector2 weight = weights_[n];
					const Vector2& weightedError = weightedErrors_[n];

					hessianValues[ 0] += xJacobian[0] * xJacobian[0] * weight.x() + yJacobian[0] * yJacobian[0] * weight.y();
					hessianValues[ 1] += xJacobian[0] * xJacobian[1] * weight.x() + yJacobian[0] * yJacobian[1] * weight.y();
					hessianValues[ 2] += xJacobian[0] * xJacobian[2] * weight.x() + yJacobian[0] * yJacobian[2] * weight.y();
					hessianValues[ 3] += xJacobian[0] * xJacobian[3] * weight.x() + yJacobian[0] * yJacobian[3] * weight.y();
					hessianValues[ 4] += xJacobian[0] * xJacobian[4] * weight.x() + yJacobian[0] * yJacobian[4] * weight.y();
					hessianValues[ 5] += xJacobian[0] * xJacobian[5] * weight.x() + yJacobian[0] * yJacobian[5] * weight.y();

					hessianValues[ 6] += xJacobian[1] * xJacobian[1] * weight.x() + yJacobian[1] * yJacobian[1] * weight.y();
					hessianValues[ 7] += xJacobian[1] * xJacobian[2] * weight.x() + yJacobian[1] * yJacobian[2] * weight.y();
					hessianValues[ 8] += xJacobian[1] * xJacobian[3] * weight.x() + yJacobian[1] * yJacobian[3] * weight.y();
					hessianValues[ 9] += xJacobian[1] * xJacobian[4] * weight.x() + yJacobian[1] * yJacobian[4] * weight.y();
					hessianValues[10] += xJacobian[1] * xJacobian[5] * weight.x() + yJacobian[1] * yJacobian[5] * weight.y();

					hessianValues[11] += xJacobian[2] * xJacobian[2] * weight.x() + yJacobian[2] * yJacobian[2] * weight.y();
					hessianValues[12] += xJacobian[2] * xJacobian[3] * weight.x() + yJacobian[2] * yJacobian[3] * weight.y();
					hessianValues[13] += xJacobian[2] * xJacobian[4] * weight.x() + yJacobian[2] * yJacobian[4] * weight.y();
					hessianValues[14] += xJacobian[2] * xJacobian[5] * weight.x() + yJacobian[2] * yJacobian[5] * weight.y();

					hessianValues[15] += xJacobian[3] * xJacobian[3] * weight.x() + yJacobian[3] * yJacobian[3] * weight.y();
					hessianValues[16] += xJacobian[3] * xJacobian[4] * weight.x() + yJacobian[3] * yJacobian[4] * weight.y();
					hessianValues[17] += xJacobian[3] * xJacobian[5] * weight.x() + yJacobian[3] * yJacobian[5] * weight.y();

					hessianValues[18] += xJacobian[4] * xJacobian[4] * weight.x() + yJacobian[4] * yJacobian[4] * weight.y();
					hessianValues[19] += xJacobian[4] * xJacobian[5] * weight.x() + yJacobian[4] * yJacobian[5] * weight.y();

					hessianValues[20] += xJacobian[5] * xJacobian[5] * weight.x() + yJacobian[5] * yJacobian[5] * weight.y();

					jacobianError(0) += xJacobian[0] * weightedError.x() + yJacobian[0] * weightedError.y();
					jacobianError(1) += xJacobian[1] * weightedError.x() + yJacobian[1] * weightedError.y();
					jacobianError(2) += xJacobian[2] * weightedError.x() + yJacobian[2] * weightedError.y();
					jacobianError(3) += xJacobian[3] * weightedError.x() + yJacobian[3] * weightedError.y();
					jacobianError(4) += xJacobian[4] * weightedError.x() + yJacobian[4] * weightedError.y();
					jacobianError(5) += xJacobian[5] * weightedError.x() + yJacobian[5] * weightedError.y();
				}
			}

			if (gravityConstraints_ != nullptr)
			{
				// the gravity error Jacobian with respect to rotation is:
				// d(R * g_world) / dwi = Rwi * g_world
				const Vector3 dRotGravity_dwx = Rwx * gravityConstraints_->worldGravityInWorld();
				const Vector3 dRotGravity_dwy = Rwy * gravityConstraints_->worldGravityInWorld();
				const Vector3 dRotGravity_dwz = Rwz * gravityConstraints_->worldGravityInWorld();

				const Vector3 jacobianX = dRotGravity_dwx * gravityWeight_;
				const Vector3 jacobianY = dRotGravity_dwy * gravityWeight_;
				const Vector3 jacobianZ = dRotGravity_dwz * gravityWeight_;

				// Gravity constraints are not weighted by the robust estimator (weight is always 1)
				// so we compute: J^T * J and J^T * error with weighted J but weight = 1 for the multiplication

				hessianValues[ 0] += jacobianX[0] * jacobianX[0] + jacobianX[1] * jacobianX[1] + jacobianX[2] * jacobianX[2];
				hessianValues[ 1] += jacobianX[0] * jacobianY[0] + jacobianX[1] * jacobianY[1] + jacobianX[2] * jacobianY[2];
				hessianValues[ 2] += jacobianX[0] * jacobianZ[0] + jacobianX[1] * jacobianZ[1] + jacobianX[2] * jacobianZ[2];
				// hessianValues[ 3] += 0
				// hessianValues[ 4] += 0
				// hessianValues[ 5] += 0

				hessianValues[ 6] += jacobianY[0] * jacobianY[0] + jacobianY[1] * jacobianY[1] + jacobianY[2] * jacobianY[2];
				hessianValues[ 7] += jacobianY[0] * jacobianZ[0] + jacobianY[1] * jacobianZ[1] + jacobianY[2] * jacobianZ[2];
				// hessianValues[ 8] += 0
				// hessianValues[ 9] += 0
				// hessianValues[10] += 0

				hessianValues[11] += jacobianZ[0] * jacobianZ[0] + jacobianZ[1] * jacobianZ[1] + jacobianZ[2] * jacobianZ[2];
				// hessianValues[12] += 0
				// hessianValues[13] += 0
				// hessianValues[14] += 0

				// hessianValues[15] += 0
				// hessianValues[16] += 0
				// hessianValues[17] += 0

				// hessianValues[18] += 0
				// hessianValues[19] += 0

				// hessianValues[20] += 0

				jacobianError(0) += jacobianX[0] * weightedErrorGravity_[0] + jacobianX[1] * weightedErrorGravity_[1] + jacobianX[2] * weightedErrorGravity_[2];
				jacobianError(1) += jacobianY[0] * weightedErrorGravity_[0] + jacobianY[1] * weightedErrorGravity_[1] + jacobianY[2] * weightedErrorGravity_[2];
				jacobianError(2) += jacobianZ[0] * weightedErrorGravity_[0] + jacobianZ[1] * weightedErrorGravity_[1] + jacobianZ[2] * weightedErrorGravity_[2];
			}

			/**
			 *
			 * |  0  1  2  3  4  5 |
			 * |  1  6  7  8  9 10 |
			 * |  2  7 11 12 13 14 |
			 * |  3  8 12 15 16 17 |
			 * |  4  9 13 16 18 19 |
			 * |  5 10 14 17 19 20 |
			 *
			 */

			hessian( 0) = hessianValues[ 0];
			hessian( 1) = hessianValues[ 1];
			hessian( 2) = hessianValues[ 2];
			hessian( 3) = hessianValues[ 3];
			hessian( 4) = hessianValues[ 4];
			hessian( 5) = hessianValues[ 5];

			hessian( 6) = hessianValues[ 1];
			hessian( 7) = hessianValues[ 6];
			hessian( 8) = hessianValues[ 7];
			hessian( 9) = hessianValues[ 8];
			hessian(10) = hessianValues[ 9];
			hessian(11) = hessianValues[10];

			hessian(12) = hessianValues[ 2];
			hessian(13) = hessianValues[ 7];
			hessian(14) = hessianValues[11];
			hessian(15) = hessianValues[12];
			hessian(16) = hessianValues[13];
			hessian(17) = hessianValues[14];

			hessian(18) = hessianValues[ 3];
			hessian(19) = hessianValues[ 8];
			hessian(20) = hessianValues[12];
			hessian(21) = hessianValues[15];
			hessian(22) = hessianValues[16];
			hessian(23) = hessianValues[17];

			hessian(24) = hessianValues[ 4];
			hessian(25) = hessianValues[ 9];
			hessian(26) = hessianValues[13];
			hessian(27) = hessianValues[16];
			hessian(28) = hessianValues[18];
			hessian(29) = hessianValues[19];

			hessian(30) = hessianValues[ 5];
			hessian(31) = hessianValues[10];
			hessian(32) = hessianValues[14];
			hessian(33) = hessianValues[17];
			hessian(34) = hessianValues[19];
			hessian(35) = hessianValues[20];

#ifdef OCEAN_INTENSIVE_DEBUG
			if (gravityConstraints_ == nullptr)
			{
				// we need to ensure that the currently stored weighted errors and weights match with the current transformation

				Vectors2 debugWeightedErrors(correspondences_);
				Vectors2 debugWeights(correspondences_);

				Scalars debugSqrErrors(correspondences_);
				Error::determinePoseErrorIF<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, true>(flippedCamera_T_world, anyCamera_, ConstTemplateArrayAccessor<Vector3>(objectPoints_, correspondences_), ConstTemplateArrayAccessor<Vector2>(imagePoints_, correspondences_), debugWeightedErrors.data(), debugSqrErrors.data());

				NonLinearOptimization::sqrErrors2robustErrors2(estimator_, debugSqrErrors, 6, debugWeightedErrors.data(), debugWeights.data(), nullptr);

				ocean_assert(debugWeightedErrors == weightedErrors_);
				if (estimator_ == Estimator::ET_SQUARE)
				{
					for (size_t n = 0; n < debugWeights.size(); ++n)
					{
						ocean_assert(debugWeights[n] == Vector2(1, 1));
					}
				}
				else
				{
					ocean_assert(debugWeights == weights_);
				}

				if (std::is_same<Scalar, double>::value)
				{
					Matrix debugJacobian(correspondences_ * 2, 6);

					for (unsigned int n = 0u; n < correspondences_; ++n)
					{
						Jacobian::calculatePoseJacobianRodrigues2x6(debugJacobian[n * 2u + 0u], debugJacobian[n * 2u + 1u], anyCamera_, flippedCamera_T_world, objectPoints_[n], Rwx, Rwy, Rwz);
					}

					Matrix debugWeightMatrix;

					if (weights_.empty())
					{
						debugWeightMatrix = Matrix(correspondences_ * 2, correspondences_ * 2, true);
					}
					else
					{
						debugWeightMatrix = Matrix(correspondences_ * 2, correspondences_ * 2, Matrix(correspondences_ * 2, 1, (Scalar*)(weights_.data())));
					}

					const Matrix debugJTJ(debugJacobian.transposed() * debugWeightMatrix * debugJacobian);
					ocean_assert(debugJTJ.isEqual(hessian, Numeric::weakEps()));

					const Matrix debugErrorJacobian(debugJacobian.transposed() * Matrix(correspondences_ * 2, 1, (Scalar*)debugWeightedErrors.data()));
					ocean_assert(debugErrorJacobian.isEqual(jacobianError, Numeric::weakEps()));
				}
			}
#endif

			return true;
		}

		/**
		 * Creates a new model candidate by adjusting the current model with delta values.
		 * @see AdvancedDenseOptimizationProvider::applyCorrection().
		 */
		inline void applyCorrection(const Matrix& deltas)
		{
			ocean_assert(deltas.rows() == 6u && deltas.columns() == 1u);

			const Pose deltaPose(deltas(3), deltas(4), deltas(5), deltas(0), deltas(1), deltas(2));
			const Pose newFlippedCamera_P_world(flippedCamera_P_world_ - deltaPose);

			candidateFlippedCamera_P_world_ = newFlippedCamera_P_world;
		}

		/**
		 * Accepts the current model candidate a new (better) model than the previous one.
		 * @see AdvancedDenseOptimizationProvider::acceptCorrection().
		 */
		inline void acceptCorrection()
		{
			flippedCamera_P_world_ = candidateFlippedCamera_P_world_;
		}

		/**
		 * Returns whether the optimization process should stop e.g., due to an external event.
		 * @see AdvancedDenseOptimizationProvider::shouldStop().
		 */
		inline bool shouldStop()
		{
			return false;
		}

		/**
		 * Returns whether the provider comes with an own solver for the linear equation.
		 * @see AdvancedDenseOptimizationProvider::hasSolver().
		 */
		inline bool hasSolver() const
		{
			return false;
		}

	protected:

		/// The camera object.
		const AnyCamera& anyCamera_;

		/// Inverted and flipped pose that will be optimized.
		Pose& flippedCamera_P_world_;

		/// Intermediate inverted and flipped pose that stores the most recent optimization result as candidate.
		Pose candidateFlippedCamera_P_world_;

		/// The 3D object points that are used for the optimization.
		const Vector3* objectPoints_;

		/// The observed 2D image points.
		const Vector2* imagePoints_;

		/// Number of points correspondences.
		const size_t correspondences_;

		/// The estimator to be used as error measure.
		const Estimator::EstimatorType estimator_;

		/// The individual (weighted) errors, one for each correspondence.
		Vectors2 weightedErrors_;

		/// The individual weights, one for each correspondence, used for non-square estimators.
		Vectors2 weights_;

		/// The optional gravity constraints.
		const GravityConstraints* gravityConstraints_ = nullptr;

		/// The three weights for the gravity error, in case gravity constraints are provided.
		Vector3 weightedErrorGravity_;

		/// The constant weight factor for the gravity constraints.
		Scalar gravityWeight_ = Scalar(-1);
};

bool NonLinearOptimizationPose::optimizePoseIF(const AnyCamera& anyCamera, const HomogenousMatrix4& flippedCamera_T_world, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& imagePoints, HomogenousMatrix4& optimizedFlippedCamera_T_world, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, Scalars* intermediateRobustErrors, const GravityConstraints* gravityConstraints)
{
	ocean_assert(anyCamera.isValid() && flippedCamera_T_world.isValid());
	ocean_assert(objectPoints.size() >= 3u);
	ocean_assert(objectPoints.size() == imagePoints.size());
	ocean_assert(&flippedCamera_T_world != &optimizedFlippedCamera_T_world);

	if (gravityConstraints != nullptr)
	{
		ocean_assert(gravityConstraints->isValid());
		ocean_assert(gravityConstraints->numberCameras() == 1);

		optimizedFlippedCamera_T_world = gravityConstraints->alignCameraWithGravityIF(flippedCamera_T_world);
	}
	else
	{
		optimizedFlippedCamera_T_world = flippedCamera_T_world;
	}

	Pose flippedCamera_P_world(optimizedFlippedCamera_T_world);

	const ScopedConstMemoryAccessor<Vector3> scopedObjectPointMemoryAccessor(objectPoints);
	const ScopedConstMemoryAccessor<Vector2> scopedImagePointMemoryAccessor(imagePoints);

	AdvancedAnyCameraPoseOptimizationProvider provider(anyCamera, flippedCamera_P_world, scopedObjectPointMemoryAccessor.data(), scopedImagePointMemoryAccessor.data(), scopedObjectPointMemoryAccessor.size(), estimator, gravityConstraints);
	if (!advancedDenseOptimization<AdvancedAnyCameraPoseOptimizationProvider>(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateRobustErrors))
	{
		return false;
	}

	optimizedFlippedCamera_T_world = flippedCamera_P_world.transformation();
	return true;
}

bool NonLinearOptimizationPose::optimizePoseIF(const AnyCamera& camera, const HomogenousMatrix4& flippedCamera_T_world, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& imagePoints, HomogenousMatrix4& optimizedFlippedCamera_T_world, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, Scalars* intermediateRobustErrors, const Matrix* invertedCovariances, const GravityConstraints* gravityConstraints)
{
	ocean_assert(camera.isValid());
	ocean_assert(flippedCamera_T_world.isValid());
	ocean_assert(objectPoints.size() >= 3u);
	ocean_assert(objectPoints.size() == imagePoints.size());

	ocean_assert(&flippedCamera_T_world != &optimizedFlippedCamera_T_world);

	if (gravityConstraints != nullptr)
	{
		ocean_assert(gravityConstraints->isValid());
		ocean_assert(gravityConstraints->numberCameras() == 1);

		optimizedFlippedCamera_T_world = gravityConstraints->alignCameraWithGravityIF(flippedCamera_T_world);
	}
	else
	{
		optimizedFlippedCamera_T_world = flippedCamera_T_world;
	}

	Pose flippedCamera_P_world(optimizedFlippedCamera_T_world);

	const ScopedConstMemoryAccessor<Vector3> scopedObjectPointMemoryAccessor(objectPoints);
	const ScopedConstMemoryAccessor<Vector2> scopedImagePointMemoryAccessor(imagePoints);

	if (invertedCovariances == nullptr)
	{
		AdvancedAnyCameraPoseOptimizationProvider provider(camera, flippedCamera_P_world, scopedObjectPointMemoryAccessor.data(), scopedImagePointMemoryAccessor.data(), scopedObjectPointMemoryAccessor.size(), estimator, gravityConstraints);
		if (!advancedDenseOptimization<AdvancedAnyCameraPoseOptimizationProvider>(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateRobustErrors))
		{
			return false;
		}
	}
	else
	{
		if (gravityConstraints != nullptr && invertedCovariances != nullptr)
		{
			Log::debug() << "The usage of inverted covariances and gravity constraints is not supported, skipping gravity constraints";
			gravityConstraints = nullptr;
		}

		PoseOptimizationProvider provider(camera, flippedCamera_P_world, scopedObjectPointMemoryAccessor.data(), scopedImagePointMemoryAccessor.data(), scopedObjectPointMemoryAccessor.size(), gravityConstraints);
		if (!denseOptimization<PoseOptimizationProvider>(provider, iterations, estimator, lambda, lambdaFactor, initialError, finalError, invertedCovariances, intermediateRobustErrors))
		{
			return false;
		}
	}

	optimizedFlippedCamera_T_world = flippedCamera_P_world.transformation();
	return true;
}

bool NonLinearOptimizationPose::optimizePoseIF(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_T_world, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& imagePoints, const bool distortImagePoints, HomogenousMatrix4& optimizedFlippedCamera_T_world, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, const Matrix* invertedCovariances)
{
	const AnyCameraPinhole anyCamera(PinholeCamera(pinholeCamera, distortImagePoints));

	constexpr Scalars* intermediateRobustErrors = nullptr;
	constexpr const GravityConstraints* gravityConstraints = nullptr;

	return optimizePoseIF(anyCamera, flippedCamera_T_world, objectPoints, imagePoints, optimizedFlippedCamera_T_world, iterations, estimator, lambda, lambdaFactor, initialError, finalError, intermediateRobustErrors, invertedCovariances, gravityConstraints);
}

/**
 * This class implements an optimization provider for a 6DOF pose with flexible scalar zoom factor.
 * The optimization needs several point correspondences between 3D object points and 2D image points.<br>
 * The provider optimizes the camera pose by minimizing the projection error between 3D object and 2D image points.<br>
 */
class NonLinearOptimizationPose::PoseZoomOptimizationProvider : public NonLinearOptimization::OptimizationProvider
{
	public:

		/**
		 * Creates a new optimization provider object.
		 * @param pinholeCamera The pinhole camera object to be used
		 * @param flippedCamera_P_world Initial inverted and flipped pose that has to be optimized
		 * @param zoom Initial zoom factor to be optimized
		 * @param objectPoints 3D object points that are projected into the camera frame
		 * @param imagePoints 2D observation image points, each point corresponds to one object point
		 * @param correspondences Number of points correspondences
		 * @param distortImagePoints True, to apply the distortion parameters of the camera
		 */
		inline PoseZoomOptimizationProvider(const PinholeCamera& camera, Pose& flippedCamera_P_world, Scalar& zoom, const Vector3* objectPoints, const Vector2* imagePoints, const size_t correspondences, const bool distortImagePoints) :
			camera_(camera),
			flippedCamera_P_world_(flippedCamera_P_world),
			candidateFlippedCamera_P_world_(flippedCamera_P_world),
			zoom_(zoom),
			candidateZoom_(zoom),
			objectPoints_(objectPoints),
			imagePoints_(imagePoints),
			correspondences_(correspondences),
			distortImagePoints_(distortImagePoints)
		{
			ocean_assert(correspondences_ >= 3);
		};

		/**
		 * Determines the jacobian matrix for the current pose.
		 * @param jacobian Jacobian matrix
		 */
		inline void determineJacobian(Matrix& jacobian) const
		{
			/**
			 * Function with Translation, Rotation, Projection and de-Homogenization:
			 * fDPRT(X, p) = x
			 * with X as 3D object position, p as 6D pose information (3D for exponential map rotation 3D for translation) and x as 2D image position
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
			 * [ ip1_x / dwx, ip1_x / dwy, ip1_x / dwz, ip1_x / dx, ip1_x / dy, ip1_x / dz, ip1_x / ds ]
			 * [ ip1_y / dwx, ip1_y / dwy, ip1_y / dwz, ip1_y / dx, ip1_y / dy, ip1_y / dz, ip1_x / ds ]
			 * [ ..........,                                                               ........... ]
			 * [ ..........,                                                               ........... ]
			 * [ ipn_x / dwx, ipn_x / dwy, ipn_x / dwz, ipn_x / dx, ipn_x / dy, ipn_x / dz, ip1_x / ds ]
			 * [ ipn_y / dwx, ipn_y / dwy, ipn_y / dwz, ipn_y / dx, ipn_y / dy, ipn_y / dz, ip1_x / ds ]
			 */

			jacobian.resize(correspondences_ * 2, 7);

			Jacobian::calculatePoseZoomJacobianRodrigues2nx7(jacobian.data(), camera_, Pose(flippedCamera_P_world_), zoom_, objectPoints_, correspondences_, distortImagePoints_);
		}

		/**
		 * Applies the pose correction and stores the new pose as candidate
		 * @param deltas Optimization deltas that define the correction
		 */
		inline void applyCorrection(const Matrix& deltas)
		{
			ocean_assert(deltas.rows() == 7u && deltas.columns() == 1u);

			const Pose deltaPose(deltas(3), deltas(4), deltas(5), deltas(0), deltas(1), deltas(2));
			const Pose newPose(flippedCamera_P_world_ - deltaPose);

			candidateFlippedCamera_P_world_ = newPose;

			candidateZoom_ = zoom_ - deltas(6);
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
			if (candidateZoom_ <= Numeric::eps() || candidateZoom_ > 100)
			{
				return Numeric::maxValue();
			}

			// we ensure that all pose values are valid (this check is especially for 32 bit floating point values)
			for (unsigned int n = 0u; n < 6u; ++n)
			{
				if (Numeric::isNan(candidateFlippedCamera_P_world_[n]))
				{
					return Numeric::maxValue();
				}
			}

			const HomogenousMatrix4 candidateFlippedCamera_T_world(candidateFlippedCamera_P_world_.transformation());

			// we ensure that all 3D object points are located in front of the camera
			for (size_t n = 0; n < correspondences_; ++n)
			{
				if (!PinholeCamera::isObjectPointInFrontIF(candidateFlippedCamera_T_world, objectPoints_[n]))
				{
					return Numeric::maxValue();
				}
			}

			// set the correct size of the resulting error vector
			weightedErrorVector.resize(2 * correspondences_, 1u);

			Vector2* const weightedErrors = (Vector2*)weightedErrorVector.data();
			const SquareMatrix2* transposedInvertedCovariances = invertedCovariances ? (SquareMatrix2*)invertedCovariances->data() : nullptr;

			// check whether the standard estimator is used
			if constexpr (Estimator::isStandardEstimator<tEstimator>())
			{
				// the weight vector should be and should stay invalid
				ocean_assert(!weightVector);

				// determine the averaged square error
				Scalar sqrError = Error::determinePoseErrorIF<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, true, false>(candidateFlippedCamera_T_world, camera_, ConstTemplateArrayAccessor<Vector3>(objectPoints_, correspondences_), ConstTemplateArrayAccessor<Vector2>(imagePoints_, correspondences_), distortImagePoints_, candidateZoom_, weightedErrors);

				if (transposedInvertedCovariances != nullptr)
				{
					sqrError = 0;
					for (size_t n = 0; n < correspondences_; ++n)
					{
						sqrError += (transposedInvertedCovariances[n].transposed() * weightedErrors[n]).sqr();
					}

					sqrError /= Scalar(correspondences_);
				}

				return sqrError;
			}
			else
			{
				// now we need the weight vector
				weightVector.resize(2 * correspondences_, 1u);

				Scalars sqrErrors(correspondences_);
				Error::determinePoseErrorIF<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, true, true>(candidateFlippedCamera_T_world, camera_, ConstTemplateArrayAccessor<Vector3>(objectPoints_, correspondences_), ConstTemplateArrayAccessor<Vector2>(imagePoints_, correspondences_), distortImagePoints_, candidateZoom_, weightedErrors, sqrErrors.data());
				return sqrErrors2robustErrors2<tEstimator>(sqrErrors, 7, weightedErrors, (Vector2*)weightVector.data(), transposedInvertedCovariances);
			}
		}

		/**
		 * Accepts the current pose candidate as better model.
		 */
		inline void acceptCorrection()
		{
			ocean_assert(candidateZoom_ > Numeric::eps());

			flippedCamera_P_world_ = candidateFlippedCamera_P_world_;
			zoom_ = candidateZoom_;
		}

	protected:

		/// The camera object.
		const PinholeCamera& camera_;

		/// Inverted and flipped pose that will be optimized.
		Pose& flippedCamera_P_world_;

		/// Intermediate inverted and flipped pose that stores the most recent optimization result as candidate.
		Pose candidateFlippedCamera_P_world_;

		/// Zoom factor that will be optimized.
		Scalar& zoom_;

		/// Intermediate zoom factor representing the most recent optimization result as candidate.
		Scalar candidateZoom_;

		/// The 3D object points that are used for the optimization.
		const Vector3* objectPoints_;

		/// The observed 2D image points.
		const Vector2* imagePoints_;

		/// Number of points correspondences.
		const size_t correspondences_;

		/// True, to use the camera distortion parameters.
		const bool distortImagePoints_;
};

bool NonLinearOptimizationPose::optimizePoseZoomIF(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_T_world, const Scalar zoom, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& imagePoints, const bool distortImagePoints, HomogenousMatrix4& optimizedFlippedCamera_T_world, Scalar& optimizedZoom, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, const Matrix* invertedCovariances)
{
	ocean_assert(flippedCamera_T_world.isValid());
	ocean_assert(objectPoints.size() >= 3u);
	ocean_assert(objectPoints.size() == imagePoints.size());

	ocean_assert(pinholeCamera.isValid() && flippedCamera_T_world.isValid());
	ocean_assert(&flippedCamera_T_world != &optimizedFlippedCamera_T_world);
	ocean_assert(&zoom != &optimizedZoom);

	optimizedFlippedCamera_T_world = flippedCamera_T_world;
	Pose optimizedInvertedFlipped(flippedCamera_T_world);

	optimizedZoom = zoom;

	const ScopedConstMemoryAccessor<Vector3> scopedObjectPointMemoryAccessor(objectPoints);
	const ScopedConstMemoryAccessor<Vector2> scopedImagePointMemoryAccessor(imagePoints);

	PoseZoomOptimizationProvider provider(pinholeCamera, optimizedInvertedFlipped, optimizedZoom, scopedObjectPointMemoryAccessor.data(), scopedImagePointMemoryAccessor.data(), scopedObjectPointMemoryAccessor.size(), distortImagePoints);
	if (!denseOptimization<PoseZoomOptimizationProvider>(provider, iterations, estimator, lambda, lambdaFactor, initialError, finalError, invertedCovariances))
	{
		return false;
	}

	optimizedFlippedCamera_T_world = optimizedInvertedFlipped.transformation();

	return true;
}

}

}
