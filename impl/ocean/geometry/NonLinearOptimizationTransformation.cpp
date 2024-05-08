// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/geometry/NonLinearOptimizationTransformation.h"
#include "ocean/geometry/Error.h"
#include "ocean/geometry/Jacobian.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements an optimizer for 6-DOF object transformations.
 */
class NonLinearOptimizationTransformation::ObjectTransformationPinholeCameraOptimizationProvider : public NonLinearOptimization::OptimizationProvider
{
	public:

		/**
		 * Creates a new optimization provider object.
		 * @param pinholeCamera The pinhole camera profile to be used, must be valid
		 * @param flippedCameras_P_world The inverted and flipped camera poses, one for each group of image points
		 * @param world_P_object The 6-DOF object transformation to be optimized
		 * @param objectPoints The 3D object points used for the optimization
		 * @param imagePointGroups The groups of 2D image points which are observations of the 3D object points in individual camera frames
		 */
		inline ObjectTransformationPinholeCameraOptimizationProvider(const PinholeCamera& pinholeCamera, const HomogenousMatrices4& flippedCameras_P_world, Pose& world_P_object, const ObjectPoints& objectPoints, const ImagePointGroups& imagePointGroups) :
			camera_(pinholeCamera),
			flippedCameras_P_world_(flippedCameras_P_world),
			world_P_object_(world_P_object),
			world_P_candidateObject_(world_P_object),
			objectPoints_(objectPoints),
			imagePointGroups_(imagePointGroups),
			measurements_(imagePointGroups.size() * imagePointGroups.front().size())
		{
			ocean_assert(pinholeCamera.isValid());
			ocean_assert(flippedCameras_P_world.size() == imagePointGroups.size());

			for (size_t n = 0; n < flippedCameras_P_world.size(); ++n)
			{
				ocean_assert(flippedCameras_P_world[n].isValid());
				ocean_assert(objectPoints.size() == imagePointGroups[n].size());
			}
		};

		/**
		 * Determines the jacobian matrix for the current pose.
		 * @param jacobian Jacobian matrix
		 */
		inline void determineJacobian(Matrix& jacobian) const
		{
			jacobian.resize(measurements_ * 2, 6);

			for (size_t n = 0; n < flippedCameras_P_world_.size(); ++n)
			{
				Jacobian::calculateObjectTransformation2nx6(jacobian[n * 2 * objectPoints_.size()], camera_, flippedCameras_P_world_[n], world_P_object_, objectPoints_.data(), objectPoints_.size());
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
			const Pose newPose(world_P_object_ - deltaPose);

			world_P_candidateObject_ = newPose;
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
			weightedErrorVector.resize(2 * measurements_, 1u);

			Vector2* const weightedErrors = (Vector2*)weightedErrorVector.data();
			const SquareMatrix2* transposedInvertedCovariances = invertedCovariances ? (SquareMatrix2*)invertedCovariances->data() : nullptr;

			// check whether the standard estimator is used
			if constexpr (Estimator::isStandardEstimator<tEstimator>())
			{
				// the weight vector should be and should stay invalid
				ocean_assert(!weightVector);

				// determine the averaged square error

				Scalar sqrError = Scalar(0);

				for (size_t n = 0; n < flippedCameras_P_world_.size(); ++n)
				{
					const HomogenousMatrix4 candidateIF(flippedCameras_P_world_[n] * world_P_candidateObject_.transformation());

					Vector2* weightedPoseErrors = weightedErrors + objectPoints_.size() * n;

					// determine the averaged square error
					const Scalar averagePoseSqrError = Error::determinePoseErrorIF<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, true, false>(candidateIF, camera_, ConstTemplateArrayAccessor<Vector3>(objectPoints_), ConstTemplateArrayAccessor<Vector2>(imagePointGroups_[n]), false, Scalar(1), weightedPoseErrors);

					if (transposedInvertedCovariances)
					{
						Scalar localSqrError = 0;

						for (size_t i = 0; i < objectPoints_.size(); ++i)
						{
							localSqrError += ((*transposedInvertedCovariances).transposed() * (*weightedPoseErrors)).sqr();

							transposedInvertedCovariances++;
							weightedPoseErrors++;
						}

						sqrError += localSqrError;
					}
					else
					{
						// we will normalize the overall error at the end, we do not sum up averaged errors for individual poses
						sqrError += averagePoseSqrError * Scalar(objectPoints_.size());
					}
				}

				return sqrError / Scalar(measurements_);
			}
			else
			{
				// now we need the weight vector
				weightVector.resize(2 * measurements_, 1u);

				Scalars sqrErrors(measurements_);

				for (size_t n = 0; n < flippedCameras_P_world_.size(); ++n)
				{
					const HomogenousMatrix4 candidateIF(flippedCameras_P_world_[n] * world_P_candidateObject_.transformation());

					Vector2* const weightedPoseErrors = weightedErrors + objectPoints_.size() * n;
					Scalar* const sqrPoseErrors = sqrErrors.data() + objectPoints_.size() * n;

					Error::determinePoseErrorIF<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, true, true>(candidateIF, camera_, ConstTemplateArrayAccessor<Vector3>(objectPoints_), ConstTemplateArrayAccessor<Vector2>(imagePointGroups_[n]), false, Scalar(1), weightedPoseErrors, sqrPoseErrors);
				}

				return sqrErrors2robustErrors2<tEstimator>(sqrErrors, 6, weightedErrors, (Vector2*)weightVector.data(), transposedInvertedCovariances);
			}
		}

		/**
		 * Accepts the current pose candidate as better model.
		 */
		inline void acceptCorrection()
		{
			world_P_object_ = world_P_candidateObject_;
		}

	protected:

		/// The camera profile to be used.
		const PinholeCamera& camera_;

		/// The inverted and flipped camera poses, one for each group of image points.
		const HomogenousMatrices4& flippedCameras_P_world_;

		/// The 6-DOF object transformation to be optimized.
		Pose& world_P_object_;

		/// The candidate of an improved 6-DOF object transformation.
		Pose world_P_candidateObject_;

		/// The 3D object points used for the optimization.
		const ObjectPoints& objectPoints_;

		/// The groups of 2D image points which are observations of the 3D object points in individual camera frames.
		const ImagePointGroups& imagePointGroups_;

		/// The overall number of measurements (object points multiplied by number of cameras).
		const size_t measurements_;
};

bool NonLinearOptimizationTransformation::optimizeObjectTransformationIF(const PinholeCamera& pinholeCamera, const HomogenousMatrices4& flippedCameras_T_world, const HomogenousMatrix4& world_T_object, const Vectors3& objectPoints, const ImagePointGroups& imagePointGroups, HomogenousMatrix4& optimized_world_T_object, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{

#ifdef OCEAN_DEBUG

	ocean_assert(pinholeCamera.isValid());
	ocean_assert(flippedCameras_T_world.size() >= 1 && flippedCameras_T_world.size() == imagePointGroups.size());
	ocean_assert(world_T_object.isValid() && world_T_object.rotationMatrix().isOrthonormal(Numeric::weakEps()));
	ocean_assert(objectPoints.size() >= 1);

	for (size_t n = 0; n < flippedCameras_T_world.size(); ++n)
	{
		ocean_assert(flippedCameras_T_world[n].isValid());
		ocean_assert(objectPoints.size() == imagePointGroups[n].size());
	}

#endif

	Pose objectTransformationPose(world_T_object);

	ObjectTransformationPinholeCameraOptimizationProvider provider(pinholeCamera, flippedCameras_T_world, objectTransformationPose, objectPoints, imagePointGroups);
	if (!denseOptimization<ObjectTransformationPinholeCameraOptimizationProvider>(provider, iterations, estimator, lambda, lambdaFactor, initialError, finalError, nullptr, intermediateErrors))
	{
		return false;
	}

	optimized_world_T_object = objectTransformationPose.transformation();

	return true;
}

class NonLinearOptimizationTransformation::AdvancedObjectTransformationFisheyeCameraOptimizationProvider : public NonLinearOptimization::AdvancedDenseOptimizationProvider
{
	public:

		inline AdvancedObjectTransformationFisheyeCameraOptimizationProvider(const FisheyeCamera& fisheyeCamera, const HomogenousMatrices4& flippedCameras_P_world, Pose& world_P_object, const ObjectPointGroups& objectPointGroups, const ImagePointGroups& imagePointGroups, const Estimator::EstimatorType estimator) :
			fisheyeCamera_(fisheyeCamera),
			flippedCameras_P_world_(flippedCameras_P_world),
			world_P_object_(world_P_object),
			world_P_candidateObject_(world_P_object),
			objectPointGroups_(objectPointGroups),
			imagePointGroups_(imagePointGroups),
			estimator_(estimator),
			measurements_(0)
		{
			ocean_assert(fisheyeCamera.isValid());
			ocean_assert(flippedCameras_P_world.size() == objectPointGroups.size());
			ocean_assert(flippedCameras_P_world.size() == imagePointGroups.size());

			for (size_t n = 0; n < flippedCameras_P_world.size(); ++n)
			{
				ocean_assert(flippedCameras_P_world[n].isValid());
				ocean_assert(objectPointGroups[n].size() == imagePointGroups[n].size());

				measurements_ += objectPointGroups[n].size();
			}
		};

		/**
		 * Determines the error for the current model candidate (not the actual model).
		 * @see AdvancedDenseOptimizationProvider::determineError().
		 */
		inline Scalar determineError()
		{
			// set the correct size of the resulting error vector
			weightedErrors_.resize(measurements_);

			const HomogenousMatrix4 providerCandidateObjectTransformation = world_P_candidateObject_.transformation();

			// check whether the standard estimator is used
			if (estimator_ == Estimator::ET_SQUARE)
			{
				// the weight vector should be and should stay invalid
				ocean_assert(weights_.empty());

				// determine the averaged square error

				Scalar sqrError = Scalar(0);

				size_t measurements = 0;

				for (size_t n = 0; n < flippedCameras_P_world_.size(); ++n)
				{
					const HomogenousMatrix4 candidateIF(flippedCameras_P_world_[n] * providerCandidateObjectTransformation);

					Vector2* weightedPoseErrors = weightedErrors_.data() + measurements;

					// determine the averaged square error
					const Scalar averagePoseSqrError = Error::determinePoseErrorIF<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, true, false>(candidateIF, fisheyeCamera_, ConstTemplateArrayAccessor<Vector3>(objectPointGroups_[n]), ConstTemplateArrayAccessor<Vector2>(imagePointGroups_[n]), weightedPoseErrors);

					// we will normalize the overall error at the end, we do not sum up averaged errors for individual poses
					sqrError += averagePoseSqrError * Scalar(objectPointGroups_[n].size());

					measurements += objectPointGroups_[n].size();
				}

				return sqrError / Scalar(measurements_);
			}
			else
			{
				// now we need the weight vector
				weights_.resize(measurements_);

				Scalars sqrErrors(measurements_);

				size_t measurements = 0;

				for (size_t n = 0; n < flippedCameras_P_world_.size(); ++n)
				{
					const HomogenousMatrix4 candidateIF(flippedCameras_P_world_[n] * providerCandidateObjectTransformation);

					Vector2* const weightedPoseErrors = weightedErrors_.data() + measurements;
					Scalar* const sqrPoseErrors = sqrErrors.data() + measurements;

					Error::determinePoseErrorIF<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, true, true>(candidateIF, fisheyeCamera_, ConstTemplateArrayAccessor<Vector3>(objectPointGroups_[n]), ConstTemplateArrayAccessor<Vector2>(imagePointGroups_[n]), weightedPoseErrors, sqrPoseErrors);

					measurements += objectPointGroups_[n].size();
				}

				return NonLinearOptimization::sqrErrors2robustErrors2(estimator_, sqrErrors, 6, weightedErrors_.data(), weights_.data(), nullptr);
			}
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
			Jacobian::calculateRotationRodriguesDerivative(ExponentialMap(Vector3(world_P_object_.rx(), world_P_object_.ry(), world_P_object_.rz())), Rwx, Rwy, Rwz);

			const HomogenousMatrix4 world_T_object = world_P_object_.transformation();

			ocean_assert(weightedErrors_.size() == measurements_);
			ocean_assert(estimator_ == Estimator::ET_SQUARE || weights_.size() == measurements_);

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

			Scalar jDistX[2];
			Scalar jDistY[2];

			const Scalar& fx = fisheyeCamera_.focalLengthX();
			const Scalar& fy = fisheyeCamera_.focalLengthY();

			Scalar xJacobian[6];
			Scalar yJacobian[6];

			Scalar hessianValues[21] = {Scalar(0)};

			if (weights_.empty())
			{
				 // we use ET_SQUARE as estimator

				ocean_assert(weights_.empty());
				ocean_assert(weightedErrors_.size() == measurements_);

				size_t measurement = 0;

				for (size_t nPose = 0; nPose < flippedCameras_P_world_.size(); ++nPose)
				{
					const HomogenousMatrix4& flippedCamera_T_world = flippedCameras_P_world_[nPose];
					const HomogenousMatrix4 flippedCamera_T_object = flippedCamera_T_world * world_T_object;

					ocean_assert(objectPointGroups_[nPose].size() == imagePointGroups_[nPose].size());
					const Vectors3& objectPoints = objectPointGroups_[nPose];

					for (size_t nObject = 0; nObject < objectPoints.size(); ++nObject)
					{
						const Vector3& objectPoint = objectPoints[nObject];

						const Vector3 flippedCamera_objectPoint = flippedCamera_T_object * objectPoint;
						const Scalar& u = flippedCamera_objectPoint.x();
						const Scalar& v = flippedCamera_objectPoint.y();
						const Scalar& w = flippedCamera_objectPoint.z();

						ocean_assert(Numeric::isNotEqualEps(w));
						const Scalar invW = Scalar(1) / w;

						const Scalar u_invW = u * invW;
						const Scalar v_invW = v * invW;

						Jacobian::calculateFisheyeDistortNormalized2x2(jDistX, jDistY, u_invW, v_invW, fisheyeCamera_.radialDistortion(), fisheyeCamera_.tangentialDistortion());

						const Scalar fx_jDistXx_invW = fx * jDistX[0] * invW;
						const Scalar fy_jDistYx_invW = fy * jDistY[0] * invW;

						const Scalar fx_jDistXy_invW = fx * jDistX[1] * invW;
						const Scalar fy_jDistYy_invW = fy * jDistY[1] * invW;

						const Scalar u_fx_jDistXx__ = u_invW * fx_jDistXx_invW + v_invW * fx_jDistXy_invW;
						const Scalar u_fy_jDistYx__ = u_invW * fy_jDistYx_invW + v_invW * fy_jDistYy_invW;

						const Scalar jFocalPoseXx = fx_jDistXx_invW * flippedCamera_T_world[0] + fx_jDistXy_invW * flippedCamera_T_world[1] - u_fx_jDistXx__ * flippedCamera_T_world[2];
						const Scalar jFocalPoseXy = fx_jDistXx_invW * flippedCamera_T_world[4] + fx_jDistXy_invW * flippedCamera_T_world[5] - u_fx_jDistXx__ * flippedCamera_T_world[6];
						const Scalar jFocalPoseXz = fx_jDistXx_invW * flippedCamera_T_world[8] + fx_jDistXy_invW * flippedCamera_T_world[9] - u_fx_jDistXx__ * flippedCamera_T_world[10];

						const Scalar jFocalPoseYx = fy_jDistYx_invW * flippedCamera_T_world[0] + fy_jDistYy_invW * flippedCamera_T_world[1] - u_fy_jDistYx__ * flippedCamera_T_world[2];
						const Scalar jFocalPoseYy = fy_jDistYx_invW * flippedCamera_T_world[4] + fy_jDistYy_invW * flippedCamera_T_world[5] - u_fy_jDistYx__ * flippedCamera_T_world[6];
						const Scalar jFocalPoseYz = fy_jDistYx_invW * flippedCamera_T_world[8] + fy_jDistYy_invW * flippedCamera_T_world[9] - u_fy_jDistYx__ * flippedCamera_T_world[10];

						const Vector3 dwx(Rwx * objectPoint);
						const Vector3 dwy(Rwy * objectPoint);
						const Vector3 dwz(Rwz * objectPoint);

						xJacobian[0] = jFocalPoseXx * dwx.x() + jFocalPoseXy * dwx.y() + jFocalPoseXz * dwx.z();
						xJacobian[1] = jFocalPoseXx * dwy.x() + jFocalPoseXy * dwy.y() + jFocalPoseXz * dwy.z();
						xJacobian[2] = jFocalPoseXx * dwz.x() + jFocalPoseXy * dwz.y() + jFocalPoseXz * dwz.z();
						xJacobian[3] = jFocalPoseXx;
						xJacobian[4] = jFocalPoseXy;
						xJacobian[5] = jFocalPoseXz;

						yJacobian[0] = jFocalPoseYx * dwx.x() + jFocalPoseYy * dwx.y() + jFocalPoseYz * dwx.z();
						yJacobian[1] = jFocalPoseYx * dwy.x() + jFocalPoseYy * dwy.y() + jFocalPoseYz * dwy.z();
						yJacobian[2] = jFocalPoseYx * dwz.x() + jFocalPoseYy * dwz.y() + jFocalPoseYz * dwz.z();
						yJacobian[3] = jFocalPoseYx;
						yJacobian[4] = jFocalPoseYy;
						yJacobian[5] = jFocalPoseYz;

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

						const Vector2& weightedError = weightedErrors_[measurement++];

						jacobianError(0) += xJacobian[0] * weightedError.x() + yJacobian[0] * weightedError.y();
						jacobianError(1) += xJacobian[1] * weightedError.x() + yJacobian[1] * weightedError.y();
						jacobianError(2) += xJacobian[2] * weightedError.x() + yJacobian[2] * weightedError.y();
						jacobianError(3) += xJacobian[3] * weightedError.x() + yJacobian[3] * weightedError.y();
						jacobianError(4) += xJacobian[4] * weightedError.x() + yJacobian[4] * weightedError.y();
						jacobianError(5) += xJacobian[5] * weightedError.x() + yJacobian[5] * weightedError.y();
					}
				}
			}
			else
			{
				// we use a non-ET_SQUARE estimator

				ocean_assert(weights_.size() == measurements_);
				ocean_assert(weightedErrors_.size() == measurements_);

				size_t measurement = 0;

				for (size_t nPose = 0; nPose < flippedCameras_P_world_.size(); ++nPose)
				{
					const HomogenousMatrix4& flippedCamera_T_world = flippedCameras_P_world_[nPose];
					const HomogenousMatrix4 flippedCamera_T_object = flippedCamera_T_world * world_T_object;

					ocean_assert(objectPointGroups_[nPose].size() == imagePointGroups_[nPose].size());
					const Vectors3& objectPoints = objectPointGroups_[nPose];

					for (size_t nObject = 0; nObject < objectPoints.size(); ++nObject)
					{
						const Vector3& objectPoint = objectPoints[nObject];

						const Vector3 flippedCamera_objectPoint = flippedCamera_T_object * objectPoint;
						const Scalar& u = flippedCamera_objectPoint.x();
						const Scalar& v = flippedCamera_objectPoint.y();
						const Scalar& w = flippedCamera_objectPoint.z();

						ocean_assert(Numeric::isNotEqualEps(w));
						const Scalar invW = Scalar(1) / w;

						const Scalar u_invW = u * invW;
						const Scalar v_invW = v * invW;

						Jacobian::calculateFisheyeDistortNormalized2x2(jDistX, jDistY, u_invW, v_invW, fisheyeCamera_.radialDistortion(), fisheyeCamera_.tangentialDistortion());

						const Scalar fx_jDistXx_invW = fx * jDistX[0] * invW;
						const Scalar fy_jDistYx_invW = fy * jDistY[0] * invW;

						const Scalar fx_jDistXy_invW = fx * jDistX[1] * invW;
						const Scalar fy_jDistYy_invW = fy * jDistY[1] * invW;

						const Scalar u_fx_jDistXx__ = u_invW * fx_jDistXx_invW + v_invW * fx_jDistXy_invW;
						const Scalar u_fy_jDistYx__ = u_invW * fy_jDistYx_invW + v_invW * fy_jDistYy_invW;

						const Scalar jFocalPoseXx = fx_jDistXx_invW * flippedCamera_T_world[0] + fx_jDistXy_invW * flippedCamera_T_world[1] - u_fx_jDistXx__ * flippedCamera_T_world[2];
						const Scalar jFocalPoseXy = fx_jDistXx_invW * flippedCamera_T_world[4] + fx_jDistXy_invW * flippedCamera_T_world[5] - u_fx_jDistXx__ * flippedCamera_T_world[6];
						const Scalar jFocalPoseXz = fx_jDistXx_invW * flippedCamera_T_world[8] + fx_jDistXy_invW * flippedCamera_T_world[9] - u_fx_jDistXx__ * flippedCamera_T_world[10];

						const Scalar jFocalPoseYx = fy_jDistYx_invW * flippedCamera_T_world[0] + fy_jDistYy_invW * flippedCamera_T_world[1] - u_fy_jDistYx__ * flippedCamera_T_world[2];
						const Scalar jFocalPoseYy = fy_jDistYx_invW * flippedCamera_T_world[4] + fy_jDistYy_invW * flippedCamera_T_world[5] - u_fy_jDistYx__ * flippedCamera_T_world[6];
						const Scalar jFocalPoseYz = fy_jDistYx_invW * flippedCamera_T_world[8] + fy_jDistYy_invW * flippedCamera_T_world[9] - u_fy_jDistYx__ * flippedCamera_T_world[10];

						const Vector3 dwx(Rwx * objectPoint);
						const Vector3 dwy(Rwy * objectPoint);
						const Vector3 dwz(Rwz * objectPoint);

						xJacobian[0] = jFocalPoseXx * dwx.x() + jFocalPoseXy * dwx.y() + jFocalPoseXz * dwx.z();
						xJacobian[1] = jFocalPoseXx * dwy.x() + jFocalPoseXy * dwy.y() + jFocalPoseXz * dwy.z();
						xJacobian[2] = jFocalPoseXx * dwz.x() + jFocalPoseXy * dwz.y() + jFocalPoseXz * dwz.z();
						xJacobian[3] = jFocalPoseXx;
						xJacobian[4] = jFocalPoseXy;
						xJacobian[5] = jFocalPoseXz;

						yJacobian[0] = jFocalPoseYx * dwx.x() + jFocalPoseYy * dwx.y() + jFocalPoseYz * dwx.z();
						yJacobian[1] = jFocalPoseYx * dwy.x() + jFocalPoseYy * dwy.y() + jFocalPoseYz * dwy.z();
						yJacobian[2] = jFocalPoseYx * dwz.x() + jFocalPoseYy * dwz.y() + jFocalPoseYz * dwz.z();
						yJacobian[3] = jFocalPoseYx;
						yJacobian[4] = jFocalPoseYy;
						yJacobian[5] = jFocalPoseYz;

						const Vector2 weight = weights_[measurement];
						const Vector2& weightedError = weightedErrors_[measurement];
						++measurement;

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
			{
				// we need to ensure that the currently stored weighted errors and weights match with the current transformation

				Vectors2 debugWeightedErrors(measurements_);
				Vectors2 debugWeights(measurements_);

				Scalars debugSqrErrors(measurements_);

				size_t measurements = 0;

				for (size_t n = 0; n < flippedCameras_P_world_.size(); ++n)
				{
					const HomogenousMatrix4 candidateIF(flippedCameras_P_world_[n] * world_P_candidateObject_.transformation());

					Vector2* const weightedPoseErrors = debugWeightedErrors.data() + measurements;
					Scalar* const sqrPoseErrors = debugSqrErrors.data() + measurements;

					Error::determinePoseErrorIF<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, true, true>(candidateIF, fisheyeCamera_, ConstTemplateArrayAccessor<Vector3>(objectPointGroups_[n]), ConstTemplateArrayAccessor<Vector2>(imagePointGroups_[n]), weightedPoseErrors, sqrPoseErrors);

					measurements += objectPointGroups_[n].size();
				}

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
					Matrix debugJacobian(measurements_ * 2, 6);

					measurements = 0;

					for (size_t n = 0; n < flippedCameras_P_world_.size(); ++n)
					{
						const Vectors3& objectPoints = objectPointGroups_[n];

						Jacobian::calculateObjectTransformation2nx6(debugJacobian[measurements * 2], fisheyeCamera_, flippedCameras_P_world_[n], world_P_object_, objectPoints.data(), objectPoints.size());

						measurements += objectPoints.size();
					}

					Matrix debugWeightMatrix;

					if (weights_.empty())
					{
						debugWeightMatrix = Matrix(measurements_ * 2, measurements_ * 2, true);
					}
					else
					{
						debugWeightMatrix = Matrix(measurements_ * 2, measurements_ * 2, Matrix(measurements_ * 2, 1, (Scalar*)weights_.data()));
					}

					const Matrix debugJTJ(debugJacobian.transposed() * debugWeightMatrix * debugJacobian);
					ocean_assert(debugJTJ.isEqual(hessian, Numeric::weakEps()));

					const Matrix debugErrorJacobian(debugJacobian.transposed() * Matrix(measurements_ * 2, 1, (Scalar*)debugWeightedErrors.data()));
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
			const Pose newPose(world_P_object_ - deltaPose);

			world_P_candidateObject_ = newPose;
		}

		/**
		 * Accepts the current model candidate a new (better) model than the previous one.
		 * @see AdvancedDenseOptimizationProvider::acceptCorrection().
		 */
		inline void acceptCorrection()
		{
			world_P_object_ = world_P_candidateObject_;
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

		/// The camera profile to be used.
		const FisheyeCamera& fisheyeCamera_;

		/// The inverted and flipped camera poses, one for each group of image points.
		const HomogenousMatrices4& flippedCameras_P_world_;

		/// The 6-DOF object transformation to be optimized.
		Pose& world_P_object_;

		/// The candidate of an improved 6-DOF object transformation.
		Pose world_P_candidateObject_;

		/// The groups of 3D object points used for the optimization.
		const ObjectPointGroups& objectPointGroups_;

		/// The groups of 2D image points which are observations of the 3D object points in individual camera frames.
		const ImagePointGroups& imagePointGroups_;

		/// The estimator to be used as error measure.
		const Estimator::EstimatorType estimator_;

		/// The individual (weighted) errors, one for each correspondence.
		Vectors2 weightedErrors_;

		/// The individual weights, one for each correspondence, used for non-square estimators.
		Vectors2 weights_;

		/// The overall number of measurements (object points multiplied by number of cameras).
		size_t measurements_;
};

bool NonLinearOptimizationTransformation::optimizeObjectTransformationIF(const FisheyeCamera& fisheyeCamera, const HomogenousMatrices4& flippedCameras_T_world, const HomogenousMatrix4& world_T_object, const ObjectPointGroups& objectPointGroups, const ImagePointGroups& imagePointGroups, HomogenousMatrix4& optimized_world_T_object, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{

#ifdef OCEAN_DEBUG

	ocean_assert(fisheyeCamera.isValid());
	ocean_assert(flippedCameras_T_world.size() >= 1);
	ocean_assert(flippedCameras_T_world.size() == objectPointGroups.size());
	ocean_assert(flippedCameras_T_world.size() == imagePointGroups.size());
	ocean_assert(world_T_object.isValid() && world_T_object.rotationMatrix().isOrthonormal(Numeric::weakEps()));

	for (size_t n = 0; n < flippedCameras_T_world.size(); ++n)
	{
		ocean_assert(flippedCameras_T_world[n].isValid());
		ocean_assert(objectPointGroups[n].size() >= 1);
		ocean_assert(objectPointGroups[n].size() == imagePointGroups[n].size());
	}

#endif

	Pose objectTransformationPose(world_T_object);

	AdvancedObjectTransformationFisheyeCameraOptimizationProvider provider(fisheyeCamera, flippedCameras_T_world, objectTransformationPose, objectPointGroups, imagePointGroups, estimator);
	if (!advancedDenseOptimization<AdvancedObjectTransformationFisheyeCameraOptimizationProvider>(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors))
	{
		return false;
	}

	optimized_world_T_object = objectTransformationPose.transformation();

	return true;
}

/**
 * An advanced optimization provider for stereo fisheye cameras.
 */
class NonLinearOptimizationTransformation::AdvancedObjectTransformationStereoFisheyeCameraOptimizationProvider : public NonLinearOptimization::AdvancedDenseOptimizationProvider
{
	public:

		inline AdvancedObjectTransformationStereoFisheyeCameraOptimizationProvider(const FisheyeCamera& fisheyeCameraA, const FisheyeCamera& fisheyeCameraB, const HomogenousMatrices4& flippedCamerasA_T_world, const HomogenousMatrices4& flippedCamerasB_T_world, Pose& world_P_object, const ObjectPointGroups& objectPointGroupsA, const ObjectPointGroups& objectPointGroupsB, const ImagePointGroups& imagePointGroupsA, const ImagePointGroups& imagePointGroupsB, const Estimator::EstimatorType estimator) :
			fisheyeCameraA_(fisheyeCameraA),
			fisheyeCameraB_(fisheyeCameraB),
			flippedCamerasA_T_world_(flippedCamerasA_T_world),
			flippedCamerasB_T_world_(flippedCamerasB_T_world),
			world_P_object_(world_P_object),
			candidate_world_P_object_(world_P_object),
			objectPointGroupsA_(objectPointGroupsA),
			objectPointGroupsB_(objectPointGroupsB),
			imagePointGroupsA_(imagePointGroupsA),
			imagePointGroupsB_(imagePointGroupsB),
			estimator_(estimator),
			measurements_(0)
		{
			ocean_assert(fisheyeCameraA_.isValid());
			ocean_assert(fisheyeCameraB_.isValid());

			ocean_assert(flippedCamerasA_T_world_.size() == objectPointGroupsA_.size());
			ocean_assert(flippedCamerasA_T_world_.size() == imagePointGroupsA_.size());

			ocean_assert(flippedCamerasB_T_world_.size() == objectPointGroupsB_.size());
			ocean_assert(flippedCamerasB_T_world_.size() == imagePointGroupsB_.size());

			for (size_t n = 0; n < flippedCamerasA_T_world_.size(); ++n)
			{
				ocean_assert(flippedCamerasA_T_world_[n].isValid());
				ocean_assert(objectPointGroupsA_[n].size() == imagePointGroupsA_[n].size());

				measurements_ += objectPointGroupsA_[n].size();
			}

			for (size_t n = 0; n < flippedCamerasB_T_world_.size(); ++n)
			{
				ocean_assert(flippedCamerasB_T_world_[n].isValid());
				ocean_assert(objectPointGroupsB_[n].size() == imagePointGroupsB_[n].size());

				measurements_ += objectPointGroupsB_[n].size();
			}
		};

		/**
		 * Determines the error for the current model candidate (not the actual model).
		 * @see AdvancedDenseOptimizationProvider::determineError().
		 */
		inline Scalar determineError()
		{
			// set the correct size of the resulting error vector
			weightedErrors_.resize(measurements_);

			const HomogenousMatrix4 candidate_world_T_object = candidate_world_P_object_.transformation();

			// check whether the standard estimator is used
			if (estimator_ == Estimator::ET_SQUARE)
			{
				// the weight vector should be and should stay invalid
				ocean_assert(weights_.empty());

				// determine the averaged square error

				Scalar sqrError = Scalar(0);

				size_t measurements = 0;

				for (unsigned int nStereo = 0u; nStereo < 2u; ++nStereo)
				{
					const FisheyeCamera& fisheyeCamera = nStereo == 0u ? fisheyeCameraA_ : fisheyeCameraB_;
					const HomogenousMatrices4& flippedCameras_T_world = nStereo == 0u ? flippedCamerasA_T_world_ : flippedCamerasB_T_world_;
					const std::vector<Vectors3>& objectPointGroups = nStereo == 0u ? objectPointGroupsA_ : objectPointGroupsB_;
					const std::vector<Vectors2>& imagePointGroups = nStereo == 0u ? imagePointGroupsA_ : imagePointGroupsB_;

					for (size_t n = 0; n < flippedCameras_T_world.size(); ++n)
					{
						const HomogenousMatrix4 candidateIF(flippedCameras_T_world[n] * candidate_world_T_object);

						Vector2* weightedPoseErrors = weightedErrors_.data() + measurements;

						// determine the averaged square error
						const Scalar averagePoseSqrError = Error::determinePoseErrorIF<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, true, false>(candidateIF, fisheyeCamera, ConstTemplateArrayAccessor<Vector3>(objectPointGroups[n]), ConstTemplateArrayAccessor<Vector2>(imagePointGroups[n]), weightedPoseErrors);

						// we will normalize the overall error at the end, we do not sum up averaged errors for individual poses
						sqrError += averagePoseSqrError * Scalar(objectPointGroups[n].size());

						measurements += objectPointGroups[n].size();
					}
				}

				return sqrError / Scalar(measurements_);
			}
			else
			{
				// now we need the weight vector
				weights_.resize(measurements_);

				Scalars sqrErrors(measurements_);

				size_t measurements = 0;

				for (unsigned int nStereo = 0u; nStereo < 2u; ++nStereo)
				{
					const FisheyeCamera& fisheyeCamera = nStereo == 0u ? fisheyeCameraA_ : fisheyeCameraB_;
					const HomogenousMatrices4& flippedCameras_T_world = nStereo == 0u ? flippedCamerasA_T_world_ : flippedCamerasB_T_world_;
					const std::vector<Vectors3>& objectPointGroups = nStereo == 0u ? objectPointGroupsA_ : objectPointGroupsB_;
					const std::vector<Vectors2>& imagePointGroups = nStereo == 0u ? imagePointGroupsA_ : imagePointGroupsB_;

					for (size_t n = 0; n < flippedCameras_T_world.size(); ++n)
					{
						const HomogenousMatrix4 candidateIF(flippedCameras_T_world[n] * candidate_world_T_object);

						Vector2* const weightedPoseErrors = weightedErrors_.data() + measurements;
						Scalar* const sqrPoseErrors = sqrErrors.data() + measurements;

						Error::determinePoseErrorIF<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, true, true>(candidateIF, fisheyeCamera, ConstTemplateArrayAccessor<Vector3>(objectPointGroups[n]), ConstTemplateArrayAccessor<Vector2>(imagePointGroups[n]), weightedPoseErrors, sqrPoseErrors);

						measurements += objectPointGroups[n].size();
					}
				}

				return NonLinearOptimization::sqrErrors2robustErrors2(estimator_, sqrErrors, 6, weightedErrors_.data(), weights_.data(), nullptr);
			}
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
			Jacobian::calculateRotationRodriguesDerivative(ExponentialMap(Vector3(world_P_object_.rx(), world_P_object_.ry(), world_P_object_.rz())), Rwx, Rwy, Rwz);

			const HomogenousMatrix4 world_T_object = world_P_object_.transformation();

			ocean_assert(weightedErrors_.size() == measurements_);
			ocean_assert(estimator_ == Estimator::ET_SQUARE || weights_.size() == measurements_);

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

			Scalar jDistX[2];
			Scalar jDistY[2];

			Scalar xJacobian[6];
			Scalar yJacobian[6];

			Scalar hessianValues[21] = {Scalar(0)};

			if (weights_.empty())
			{
				 // we use ET_SQUARE as estimator

				ocean_assert(weights_.empty());
				ocean_assert(weightedErrors_.size() == measurements_);

				size_t measurement = 0;

				for (unsigned int nStereo = 0u; nStereo < 2u; ++nStereo)
				{
					const FisheyeCamera& fisheyeCamera = nStereo == 0u ? fisheyeCameraA_ : fisheyeCameraB_;
					const HomogenousMatrices4& flippedCameras_T_world = nStereo == 0u ? flippedCamerasA_T_world_ : flippedCamerasB_T_world_;
					const std::vector<Vectors3>& objectPointGroups = nStereo == 0u ? objectPointGroupsA_ : objectPointGroupsB_;
					const std::vector<Vectors2>& imagePointGroups = nStereo == 0u ? imagePointGroupsA_ : imagePointGroupsB_;

					const Scalar& fx = fisheyeCamera.focalLengthX();
					const Scalar& fy = fisheyeCamera.focalLengthY();

					for (size_t nPose = 0; nPose < flippedCameras_T_world.size(); ++nPose)
					{
						const HomogenousMatrix4& flippedCamera_T_world = flippedCameras_T_world[nPose];
						const HomogenousMatrix4 flippedCamera_T_object = flippedCamera_T_world * world_T_object;

						ocean_assert_and_suppress_unused(objectPointGroups[nPose].size() == imagePointGroups[nPose].size(), imagePointGroups);
						const Vectors3& objectPoints = objectPointGroups[nPose];

						for (size_t nObject = 0; nObject < objectPoints.size(); ++nObject)
						{
							const Vector3& objectPoint = objectPoints[nObject];

							const Vector3 flippedCamera_objectPoint = flippedCamera_T_object * objectPoint;
							const Scalar& u = flippedCamera_objectPoint.x();
							const Scalar& v = flippedCamera_objectPoint.y();
							const Scalar& w = flippedCamera_objectPoint.z();

							ocean_assert(Numeric::isNotEqualEps(w));
							const Scalar invW = Scalar(1) / w;

							const Scalar u_invW = u * invW;
							const Scalar v_invW = v * invW;

							Jacobian::calculateFisheyeDistortNormalized2x2(jDistX, jDistY, u_invW, v_invW, fisheyeCamera.radialDistortion(), fisheyeCamera.tangentialDistortion());

							const Scalar fx_jDistXx_invW = fx * jDistX[0] * invW;
							const Scalar fy_jDistYx_invW = fy * jDistY[0] * invW;

							const Scalar fx_jDistXy_invW = fx * jDistX[1] * invW;
							const Scalar fy_jDistYy_invW = fy * jDistY[1] * invW;

							const Scalar u_fx_jDistXx__ = u_invW * fx_jDistXx_invW + v_invW * fx_jDistXy_invW;
							const Scalar u_fy_jDistYx__ = u_invW * fy_jDistYx_invW + v_invW * fy_jDistYy_invW;

							const Scalar jFocalPoseXx = fx_jDistXx_invW * flippedCamera_T_world[0] + fx_jDistXy_invW * flippedCamera_T_world[1] - u_fx_jDistXx__ * flippedCamera_T_world[2];
							const Scalar jFocalPoseXy = fx_jDistXx_invW * flippedCamera_T_world[4] + fx_jDistXy_invW * flippedCamera_T_world[5] - u_fx_jDistXx__ * flippedCamera_T_world[6];
							const Scalar jFocalPoseXz = fx_jDistXx_invW * flippedCamera_T_world[8] + fx_jDistXy_invW * flippedCamera_T_world[9] - u_fx_jDistXx__ * flippedCamera_T_world[10];

							const Scalar jFocalPoseYx = fy_jDistYx_invW * flippedCamera_T_world[0] + fy_jDistYy_invW * flippedCamera_T_world[1] - u_fy_jDistYx__ * flippedCamera_T_world[2];
							const Scalar jFocalPoseYy = fy_jDistYx_invW * flippedCamera_T_world[4] + fy_jDistYy_invW * flippedCamera_T_world[5] - u_fy_jDistYx__ * flippedCamera_T_world[6];
							const Scalar jFocalPoseYz = fy_jDistYx_invW * flippedCamera_T_world[8] + fy_jDistYy_invW * flippedCamera_T_world[9] - u_fy_jDistYx__ * flippedCamera_T_world[10];

							const Vector3 dwx(Rwx * objectPoint);
							const Vector3 dwy(Rwy * objectPoint);
							const Vector3 dwz(Rwz * objectPoint);

							xJacobian[0] = jFocalPoseXx * dwx.x() + jFocalPoseXy * dwx.y() + jFocalPoseXz * dwx.z();
							xJacobian[1] = jFocalPoseXx * dwy.x() + jFocalPoseXy * dwy.y() + jFocalPoseXz * dwy.z();
							xJacobian[2] = jFocalPoseXx * dwz.x() + jFocalPoseXy * dwz.y() + jFocalPoseXz * dwz.z();
							xJacobian[3] = jFocalPoseXx;
							xJacobian[4] = jFocalPoseXy;
							xJacobian[5] = jFocalPoseXz;

							yJacobian[0] = jFocalPoseYx * dwx.x() + jFocalPoseYy * dwx.y() + jFocalPoseYz * dwx.z();
							yJacobian[1] = jFocalPoseYx * dwy.x() + jFocalPoseYy * dwy.y() + jFocalPoseYz * dwy.z();
							yJacobian[2] = jFocalPoseYx * dwz.x() + jFocalPoseYy * dwz.y() + jFocalPoseYz * dwz.z();
							yJacobian[3] = jFocalPoseYx;
							yJacobian[4] = jFocalPoseYy;
							yJacobian[5] = jFocalPoseYz;

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

							const Vector2& weightedError = weightedErrors_[measurement++];

							jacobianError(0) += xJacobian[0] * weightedError.x() + yJacobian[0] * weightedError.y();
							jacobianError(1) += xJacobian[1] * weightedError.x() + yJacobian[1] * weightedError.y();
							jacobianError(2) += xJacobian[2] * weightedError.x() + yJacobian[2] * weightedError.y();
							jacobianError(3) += xJacobian[3] * weightedError.x() + yJacobian[3] * weightedError.y();
							jacobianError(4) += xJacobian[4] * weightedError.x() + yJacobian[4] * weightedError.y();
							jacobianError(5) += xJacobian[5] * weightedError.x() + yJacobian[5] * weightedError.y();
						}
					}
				}
			}
			else
			{
				// we use a non-ET_SQUARE estimator

				ocean_assert(weights_.size() == measurements_);
				ocean_assert(weightedErrors_.size() == measurements_);

				size_t measurement = 0;

				for (unsigned int nStereo = 0u; nStereo < 2u; ++nStereo)
				{
					const FisheyeCamera& fisheyeCamera = nStereo == 0u ? fisheyeCameraA_ : fisheyeCameraB_;
					const HomogenousMatrices4& flippedCameras_T_world = nStereo == 0u ? flippedCamerasA_T_world_ : flippedCamerasB_T_world_;
					const std::vector<Vectors3>& objectPointGroups = nStereo == 0u ? objectPointGroupsA_ : objectPointGroupsB_;
					const std::vector<Vectors2>& imagePointGroups = nStereo == 0u ? imagePointGroupsA_ : imagePointGroupsB_;

					const Scalar& fx = fisheyeCamera.focalLengthX();
					const Scalar& fy = fisheyeCamera.focalLengthY();

					for (size_t nPose = 0; nPose < flippedCameras_T_world.size(); ++nPose)
					{
						const HomogenousMatrix4& flippedCamera_T_world = flippedCameras_T_world[nPose];
						const HomogenousMatrix4 flippedCamera_T_object = flippedCamera_T_world * world_T_object;

						ocean_assert_and_suppress_unused(objectPointGroups[nPose].size() == imagePointGroups[nPose].size(), imagePointGroups);
						const Vectors3& objectPoints = objectPointGroups[nPose];

						for (size_t nObject = 0; nObject < objectPoints.size(); ++nObject)
						{
							const Vector3& objectPoint = objectPoints[nObject];

							const Vector3 flippedCamera_objectPoint = flippedCamera_T_object * objectPoint;
							const Scalar& u = flippedCamera_objectPoint.x();
							const Scalar& v = flippedCamera_objectPoint.y();
							const Scalar& w = flippedCamera_objectPoint.z();

							ocean_assert(Numeric::isNotEqualEps(w));
							const Scalar invW = Scalar(1) / w;

							const Scalar u_invW = u * invW;
							const Scalar v_invW = v * invW;

							Jacobian::calculateFisheyeDistortNormalized2x2(jDistX, jDistY, u_invW, v_invW, fisheyeCamera.radialDistortion(), fisheyeCamera.tangentialDistortion());

							const Scalar fx_jDistXx_invW = fx * jDistX[0] * invW;
							const Scalar fy_jDistYx_invW = fy * jDistY[0] * invW;

							const Scalar fx_jDistXy_invW = fx * jDistX[1] * invW;
							const Scalar fy_jDistYy_invW = fy * jDistY[1] * invW;

							const Scalar u_fx_jDistXx__ = u_invW * fx_jDistXx_invW + v_invW * fx_jDistXy_invW;
							const Scalar u_fy_jDistYx__ = u_invW * fy_jDistYx_invW + v_invW * fy_jDistYy_invW;

							const Scalar jFocalPoseXx = fx_jDistXx_invW * flippedCamera_T_world[0] + fx_jDistXy_invW * flippedCamera_T_world[1] - u_fx_jDistXx__ * flippedCamera_T_world[2];
							const Scalar jFocalPoseXy = fx_jDistXx_invW * flippedCamera_T_world[4] + fx_jDistXy_invW * flippedCamera_T_world[5] - u_fx_jDistXx__ * flippedCamera_T_world[6];
							const Scalar jFocalPoseXz = fx_jDistXx_invW * flippedCamera_T_world[8] + fx_jDistXy_invW * flippedCamera_T_world[9] - u_fx_jDistXx__ * flippedCamera_T_world[10];

							const Scalar jFocalPoseYx = fy_jDistYx_invW * flippedCamera_T_world[0] + fy_jDistYy_invW * flippedCamera_T_world[1] - u_fy_jDistYx__ * flippedCamera_T_world[2];
							const Scalar jFocalPoseYy = fy_jDistYx_invW * flippedCamera_T_world[4] + fy_jDistYy_invW * flippedCamera_T_world[5] - u_fy_jDistYx__ * flippedCamera_T_world[6];
							const Scalar jFocalPoseYz = fy_jDistYx_invW * flippedCamera_T_world[8] + fy_jDistYy_invW * flippedCamera_T_world[9] - u_fy_jDistYx__ * flippedCamera_T_world[10];

							const Vector3 dwx(Rwx * objectPoint);
							const Vector3 dwy(Rwy * objectPoint);
							const Vector3 dwz(Rwz * objectPoint);

							xJacobian[0] = jFocalPoseXx * dwx.x() + jFocalPoseXy * dwx.y() + jFocalPoseXz * dwx.z();
							xJacobian[1] = jFocalPoseXx * dwy.x() + jFocalPoseXy * dwy.y() + jFocalPoseXz * dwy.z();
							xJacobian[2] = jFocalPoseXx * dwz.x() + jFocalPoseXy * dwz.y() + jFocalPoseXz * dwz.z();
							xJacobian[3] = jFocalPoseXx;
							xJacobian[4] = jFocalPoseXy;
							xJacobian[5] = jFocalPoseXz;

							yJacobian[0] = jFocalPoseYx * dwx.x() + jFocalPoseYy * dwx.y() + jFocalPoseYz * dwx.z();
							yJacobian[1] = jFocalPoseYx * dwy.x() + jFocalPoseYy * dwy.y() + jFocalPoseYz * dwy.z();
							yJacobian[2] = jFocalPoseYx * dwz.x() + jFocalPoseYy * dwz.y() + jFocalPoseYz * dwz.z();
							yJacobian[3] = jFocalPoseYx;
							yJacobian[4] = jFocalPoseYy;
							yJacobian[5] = jFocalPoseYz;

							const Vector2 weight = weights_[measurement];
							const Vector2& weightedError = weightedErrors_[measurement];
							++measurement;

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
				}
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
			{
				// we need to ensure that the currently stored weighted errors and weights match with the current transformation

				Vectors2 debugWeightedErrors(measurements_);
				Vectors2 debugWeights(measurements_);

				Scalars debugSqrErrors(measurements_);

				size_t measurements = 0;

				for (size_t n = 0; n < flippedCameras_P_world_.size(); ++n)
				{
					const HomogenousMatrix4 candidateIF(flippedCameras_P_world_[n] * world_P_candidateObject_.transformation());

					Vector2* const weightedPoseErrors = debugWeightedErrors.data() + measurements;
					Scalar* const sqrPoseErrors = debugSqrErrors.data() + measurements;

					Error::determinePoseErrorIF<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, true, true>(candidateIF, fisheyeCamera_, ConstTemplateArrayAccessor<Vector3>(objectPointGroups_[n]), ConstTemplateArrayAccessor<Vector2>(imagePointGroups_[n]), weightedPoseErrors, sqrPoseErrors);

					measurements += objectPointGroups_[n].size();
				}

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
					Matrix debugJacobian(measurements_ * 2, 6);

					measurements = 0;

					for (size_t n = 0; n < flippedCameras_P_world_.size(); ++n)
					{
						const Vectors3& objectPoints = objectPointGroups_[n];

						Jacobian::calculateObjectTransformation2nx6(debugJacobian[measurements * 2], fisheyeCamera_, flippedCameras_P_world_[n], world_P_object_, objectPoints.data(), objectPoints.size());

						measurements += objectPoints.size();
					}

					Matrix debugWeightMatrix;

					if (weights_.empty())
					{
						debugWeightMatrix = Matrix(measurements_ * 2, measurements_ * 2, true);
					}
					else
					{
						debugWeightMatrix = Matrix(measurements_ * 2, measurements_ * 2, Matrix(measurements_ * 2, 1, (Scalar*)weights_.data()));
					}

					const Matrix debugJTJ(debugJacobian.transposed() * debugWeightMatrix * debugJacobian);
					ocean_assert(debugJTJ.isEqual(hessian, Numeric::weakEps()));

					const Matrix debugErrorJacobian(debugJacobian.transposed() * Matrix(measurements_ * 2, 1, (Scalar*)debugWeightedErrors.data()));
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
			const Pose newPose(world_P_object_ - deltaPose);

			candidate_world_P_object_ = newPose;
		}

		/**
		 * Accepts the current model candidate a new (better) model than the previous one.
		 * @see AdvancedDenseOptimizationProvider::acceptCorrection().
		 */
		inline void acceptCorrection()
		{
			world_P_object_ = candidate_world_P_object_;
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

		/// The first stereo camera profile to be used.
		const FisheyeCamera& fisheyeCameraA_;

		/// The first stereo camera profile to be used.
		const FisheyeCamera& fisheyeCameraB_;

		/// The inverted and flipped camera poses for the first stereo camera, one for each group of image points.
		const HomogenousMatrices4& flippedCamerasA_T_world_;

		/// The inverted and flipped camera poses for the second stereo camera, one for each group of image points.
		const HomogenousMatrices4& flippedCamerasB_T_world_;

		/// The 6-DOF object transformation to be optimized.
		Pose& world_P_object_;

		/// The candidate of an improved 6-DOF object transformation.
		Pose candidate_world_P_object_;

		/// The groups of 3D object points for the first stereo camera.
		const ObjectPointGroups& objectPointGroupsA_;

		/// The groups of 3D object points for the second stereo camera.
		const ObjectPointGroups& objectPointGroupsB_;

		/// The groups of 2D image points which are observations of the 3D object points in individual first stereo camera frames.
		const ImagePointGroups& imagePointGroupsA_;

		/// The groups of 2D image points which are observations of the 3D object points in individual second stereo camera frames.
		const ImagePointGroups& imagePointGroupsB_;

		/// The estimator to be used as error measure.
		const Estimator::EstimatorType estimator_;

		/// The individual (weighted) errors, one for each correspondence.
		Vectors2 weightedErrors_;

		/// The individual weights, one for each correspondence, used for non-square estimators.
		Vectors2 weights_;

		/// The overall number of measurements (object points multiplied by number of cameras).
		size_t measurements_;
};

bool NonLinearOptimizationTransformation::optimizeObjectTransformationStereoIF(const FisheyeCamera& fisheyeCameraA, const FisheyeCamera& fisheyeCameraB, const HomogenousMatrices4& flippedCamerasA_T_world, const HomogenousMatrices4& flippedCamerasB_T_world, const HomogenousMatrix4& world_T_object, const ObjectPointGroups& objectPointGroupsA, const ObjectPointGroups& objectPointGroupsB, const ImagePointGroups& imagePointGroupsA, const ImagePointGroups& imagePointGroupsB, HomogenousMatrix4& optimized_world_T_object, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{

#ifdef OCEAN_DEBUG

	ocean_assert(fisheyeCameraA.isValid());
	ocean_assert(fisheyeCameraB.isValid());

	ocean_assert(flippedCamerasA_T_world.size() == objectPointGroupsA.size());
	ocean_assert(flippedCamerasA_T_world.size() == imagePointGroupsA.size());

	ocean_assert(flippedCamerasB_T_world.size() == objectPointGroupsB.size());
	ocean_assert(flippedCamerasB_T_world.size() == imagePointGroupsB.size());

	ocean_assert(world_T_object.isValid() && world_T_object.rotationMatrix().isOrthonormal(Numeric::weakEps()));

	for (size_t n = 0; n < flippedCamerasA_T_world.size(); ++n)
	{
		ocean_assert(flippedCamerasA_T_world[n].isValid());
		ocean_assert(objectPointGroupsA[n].size() == imagePointGroupsA[n].size());
	}

	for (size_t n = 0; n < flippedCamerasB_T_world.size(); ++n)
	{
		ocean_assert(flippedCamerasB_T_world[n].isValid());
		ocean_assert(objectPointGroupsB[n].size() == imagePointGroupsB[n].size());
	}

#endif

	Pose world_P_object(world_T_object);

	AdvancedObjectTransformationStereoFisheyeCameraOptimizationProvider provider(fisheyeCameraA, fisheyeCameraB, flippedCamerasA_T_world, flippedCamerasB_T_world, world_P_object, objectPointGroupsA, objectPointGroupsB, imagePointGroupsA, imagePointGroupsB, estimator);
	if (!advancedDenseOptimization<AdvancedObjectTransformationStereoFisheyeCameraOptimizationProvider>(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors))
	{
		return false;
	}

	optimized_world_T_object = world_P_object.transformation();

	return true;
}

class NonLinearOptimizationTransformation::AdvancedObjectTransformationAnyCameraOptimizationProvider : public NonLinearOptimization::AdvancedDenseOptimizationProvider
{
	public:

		inline AdvancedObjectTransformationAnyCameraOptimizationProvider(const AnyCamera& anyCamera, const HomogenousMatrices4& flippedCameras_P_world, Pose& world_P_object, const ObjectPointGroups& objectPointGroups, const ImagePointGroups& imagePointGroups, const Estimator::EstimatorType estimator) :
			anyCamera_(anyCamera),
			flippedCameras_P_world_(flippedCameras_P_world),
			world_P_object_(world_P_object),
			world_P_candidateObject_(world_P_object),
			objectPointGroups_(objectPointGroups),
			imagePointGroups_(imagePointGroups),
			estimator_(estimator),
			measurements_(0)
		{
			ocean_assert(anyCamera_.isValid());
			ocean_assert(flippedCameras_P_world.size() == objectPointGroups.size());
			ocean_assert(flippedCameras_P_world.size() == imagePointGroups.size());

			for (size_t n = 0; n < flippedCameras_P_world_.size(); ++n)
			{
				ocean_assert(flippedCameras_P_world_[n].isValid());
				ocean_assert(objectPointGroups[n].size() == imagePointGroups[n].size());

				measurements_ += objectPointGroups[n].size();
			}
		};

		/**
		 * Determines the error for the current model candidate (not the actual model).
		 * @see AdvancedDenseOptimizationProvider::determineError().
		 */
		inline Scalar determineError()
		{
			// set the correct size of the resulting error vector
			weightedErrors_.resize(measurements_);

			const HomogenousMatrix4 world_T_candidateObject = world_P_candidateObject_.transformation();

			// check whether the standard estimator is used
			if (estimator_ == Estimator::ET_SQUARE)
			{
				// the weight vector should be and should stay invalid
				ocean_assert(weights_.empty());

				// determine the averaged square error

				Scalar sqrError = Scalar(0);

				size_t measurements = 0;

				for (size_t n = 0; n < flippedCameras_P_world_.size(); ++n)
				{
					const HomogenousMatrix4 flippedCamera_T_candidateObject(flippedCameras_P_world_[n] * world_T_candidateObject);

					Vector2* weightedPoseErrors = weightedErrors_.data() + measurements;

					// determine the averaged square error
					const Scalar averagePoseSqrError = Error::determinePoseErrorIF<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, false>(flippedCamera_T_candidateObject, anyCamera_, ConstTemplateArrayAccessor<Vector3>(objectPointGroups_[n]), ConstTemplateArrayAccessor<Vector2>(imagePointGroups_[n]), weightedPoseErrors);

					// we will normalize the overall error at the end, we do not sum up averaged errors for individual poses
					sqrError += averagePoseSqrError * Scalar(objectPointGroups_[n].size());

					measurements += objectPointGroups_[n].size();
				}

				return sqrError / Scalar(measurements_);
			}
			else
			{
				// now we need the weight vector
				weights_.resize(measurements_);

				Scalars sqrErrors(measurements_);

				size_t measurements = 0;

				for (size_t n = 0; n < flippedCameras_P_world_.size(); ++n)
				{
					const HomogenousMatrix4 flippedCamera_T_candidateObject(flippedCameras_P_world_[n] * world_T_candidateObject);

					Vector2* const weightedPoseErrors = weightedErrors_.data() + measurements;
					Scalar* const sqrPoseErrors = sqrErrors.data() + measurements;

					Error::determinePoseErrorIF<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, true>(flippedCamera_T_candidateObject, anyCamera_, ConstTemplateArrayAccessor<Vector3>(objectPointGroups_[n]), ConstTemplateArrayAccessor<Vector2>(imagePointGroups_[n]), weightedPoseErrors, sqrPoseErrors);

					measurements += objectPointGroups_[n].size();
				}

				return NonLinearOptimization::sqrErrors2robustErrors2(estimator_, sqrErrors, 6, weightedErrors_.data(), weights_.data(), nullptr);
			}
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
			Jacobian::calculateRotationRodriguesDerivative(ExponentialMap(Vector3(world_P_object_.rx(), world_P_object_.ry(), world_P_object_.rz())), Rwx, Rwy, Rwz);

			const HomogenousMatrix4 world_T_object = world_P_object_.transformation();

			ocean_assert(weightedErrors_.size() == measurements_);
			ocean_assert(estimator_ == Estimator::ET_SQUARE || weights_.size() == measurements_);

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

			Scalar xPointJacobian[3];
			Scalar yPointJacobian[3];

			if (weights_.empty())
			{
				 // we use ET_SQUARE as estimator

				ocean_assert(weights_.empty());
				ocean_assert(weightedErrors_.size() == measurements_);

				size_t measurement = 0;

				for (size_t nPose = 0; nPose < flippedCameras_P_world_.size(); ++nPose)
				{
					const HomogenousMatrix4& flippedCamera_T_world = flippedCameras_P_world_[nPose];
					const HomogenousMatrix4 flippedCamera_T_object = flippedCamera_T_world * world_T_object;

					ocean_assert(objectPointGroups_[nPose].size() == imagePointGroups_[nPose].size());
					const Vectors3& objectPoints = objectPointGroups_[nPose];

					for (size_t nObject = 0; nObject < objectPoints.size(); ++nObject)
					{
						const Vector3& objectPoint = objectPoints[nObject];

						anyCamera_.pointJacobian2x3IF(flippedCamera_T_object * objectPoint, xPointJacobian, yPointJacobian);

						const Scalar jFocalPoseXx = xPointJacobian[0] * flippedCamera_T_world[0] + xPointJacobian[1] * flippedCamera_T_world[1] + xPointJacobian[2] * flippedCamera_T_world[2];
						const Scalar jFocalPoseXy = xPointJacobian[0] * flippedCamera_T_world[4] + xPointJacobian[1] * flippedCamera_T_world[5] + xPointJacobian[2] * flippedCamera_T_world[6];
						const Scalar jFocalPoseXz = xPointJacobian[0] * flippedCamera_T_world[8] + xPointJacobian[1] * flippedCamera_T_world[9] + xPointJacobian[2] * flippedCamera_T_world[10];

						const Scalar jFocalPoseYx = yPointJacobian[0] * flippedCamera_T_world[0] + yPointJacobian[1] * flippedCamera_T_world[1] + yPointJacobian[2] * flippedCamera_T_world[2];
						const Scalar jFocalPoseYy = yPointJacobian[0] * flippedCamera_T_world[4] + yPointJacobian[1] * flippedCamera_T_world[5] + yPointJacobian[2] * flippedCamera_T_world[6];
						const Scalar jFocalPoseYz = yPointJacobian[0] * flippedCamera_T_world[8] + yPointJacobian[1] * flippedCamera_T_world[9] + yPointJacobian[2] * flippedCamera_T_world[10];

						const Vector3 dwx(Rwx * objectPoint);
						const Vector3 dwy(Rwy * objectPoint);
						const Vector3 dwz(Rwz * objectPoint);

						xJacobian[0] = jFocalPoseXx * dwx.x() + jFocalPoseXy * dwx.y() + jFocalPoseXz * dwx.z();
						xJacobian[1] = jFocalPoseXx * dwy.x() + jFocalPoseXy * dwy.y() + jFocalPoseXz * dwy.z();
						xJacobian[2] = jFocalPoseXx * dwz.x() + jFocalPoseXy * dwz.y() + jFocalPoseXz * dwz.z();
						xJacobian[3] = jFocalPoseXx;
						xJacobian[4] = jFocalPoseXy;
						xJacobian[5] = jFocalPoseXz;

						yJacobian[0] = jFocalPoseYx * dwx.x() + jFocalPoseYy * dwx.y() + jFocalPoseYz * dwx.z();
						yJacobian[1] = jFocalPoseYx * dwy.x() + jFocalPoseYy * dwy.y() + jFocalPoseYz * dwy.z();
						yJacobian[2] = jFocalPoseYx * dwz.x() + jFocalPoseYy * dwz.y() + jFocalPoseYz * dwz.z();
						yJacobian[3] = jFocalPoseYx;
						yJacobian[4] = jFocalPoseYy;
						yJacobian[5] = jFocalPoseYz;

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

						const Vector2& weightedError = weightedErrors_[measurement++];

						jacobianError(0) += xJacobian[0] * weightedError.x() + yJacobian[0] * weightedError.y();
						jacobianError(1) += xJacobian[1] * weightedError.x() + yJacobian[1] * weightedError.y();
						jacobianError(2) += xJacobian[2] * weightedError.x() + yJacobian[2] * weightedError.y();
						jacobianError(3) += xJacobian[3] * weightedError.x() + yJacobian[3] * weightedError.y();
						jacobianError(4) += xJacobian[4] * weightedError.x() + yJacobian[4] * weightedError.y();
						jacobianError(5) += xJacobian[5] * weightedError.x() + yJacobian[5] * weightedError.y();
					}
				}
			}
			else
			{
				// we use a non-ET_SQUARE estimator

				ocean_assert(weights_.size() == measurements_);
				ocean_assert(weightedErrors_.size() == measurements_);

				size_t measurement = 0;

				for (size_t nPose = 0; nPose < flippedCameras_P_world_.size(); ++nPose)
				{
					const HomogenousMatrix4& flippedCamera_T_world = flippedCameras_P_world_[nPose];
					const HomogenousMatrix4 flippedCamera_T_object = flippedCamera_T_world * world_T_object;

					ocean_assert(objectPointGroups_[nPose].size() == imagePointGroups_[nPose].size());
					const Vectors3& objectPoints = objectPointGroups_[nPose];

					for (size_t nObject = 0; nObject < objectPoints.size(); ++nObject)
					{
						const Vector3& objectPoint = objectPoints[nObject];

						anyCamera_.pointJacobian2x3IF(flippedCamera_T_object * objectPoint, xPointJacobian, yPointJacobian);

						const Scalar jFocalPoseXx = xPointJacobian[0] * flippedCamera_T_world[0] + xPointJacobian[1] * flippedCamera_T_world[1] + xPointJacobian[2] * flippedCamera_T_world[2];
						const Scalar jFocalPoseXy = xPointJacobian[0] * flippedCamera_T_world[4] + xPointJacobian[1] * flippedCamera_T_world[5] + xPointJacobian[2] * flippedCamera_T_world[6];
						const Scalar jFocalPoseXz = xPointJacobian[0] * flippedCamera_T_world[8] + xPointJacobian[1] * flippedCamera_T_world[9] + xPointJacobian[2] * flippedCamera_T_world[10];

						const Scalar jFocalPoseYx = yPointJacobian[0] * flippedCamera_T_world[0] + yPointJacobian[1] * flippedCamera_T_world[1] + yPointJacobian[2] * flippedCamera_T_world[2];
						const Scalar jFocalPoseYy = yPointJacobian[0] * flippedCamera_T_world[4] + yPointJacobian[1] * flippedCamera_T_world[5] + yPointJacobian[2] * flippedCamera_T_world[6];
						const Scalar jFocalPoseYz = yPointJacobian[0] * flippedCamera_T_world[8] + yPointJacobian[1] * flippedCamera_T_world[9] + yPointJacobian[2] * flippedCamera_T_world[10];

						const Vector3 dwx(Rwx * objectPoint);
						const Vector3 dwy(Rwy * objectPoint);
						const Vector3 dwz(Rwz * objectPoint);

						xJacobian[0] = jFocalPoseXx * dwx.x() + jFocalPoseXy * dwx.y() + jFocalPoseXz * dwx.z();
						xJacobian[1] = jFocalPoseXx * dwy.x() + jFocalPoseXy * dwy.y() + jFocalPoseXz * dwy.z();
						xJacobian[2] = jFocalPoseXx * dwz.x() + jFocalPoseXy * dwz.y() + jFocalPoseXz * dwz.z();
						xJacobian[3] = jFocalPoseXx;
						xJacobian[4] = jFocalPoseXy;
						xJacobian[5] = jFocalPoseXz;

						yJacobian[0] = jFocalPoseYx * dwx.x() + jFocalPoseYy * dwx.y() + jFocalPoseYz * dwx.z();
						yJacobian[1] = jFocalPoseYx * dwy.x() + jFocalPoseYy * dwy.y() + jFocalPoseYz * dwy.z();
						yJacobian[2] = jFocalPoseYx * dwz.x() + jFocalPoseYy * dwz.y() + jFocalPoseYz * dwz.z();
						yJacobian[3] = jFocalPoseYx;
						yJacobian[4] = jFocalPoseYy;
						yJacobian[5] = jFocalPoseYz;

						const Vector2 weight = weights_[measurement];
						const Vector2& weightedError = weightedErrors_[measurement];
						++measurement;

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
			{
				// we need to ensure that the currently stored weighted errors and weights match with the current transformation

				Vectors2 debugWeightedErrors(measurements_);
				Vectors2 debugWeights(measurements_);

				Scalars debugSqrErrors(measurements_);

				size_t measurements = 0;

				for (size_t n = 0; n < flippedCameras_P_world_.size(); ++n)
				{
					const HomogenousMatrix4 candidateIF(flippedCameras_P_world_[n] * world_P_candidateObject_.transformation());

					Vector2* const weightedPoseErrors = debugWeightedErrors.data() + measurements;
					Scalar* const sqrPoseErrors = debugSqrErrors.data() + measurements;

					Error::determinePoseErrorIF<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, true>(candidateIF, anyCamera_, ConstTemplateArrayAccessor<Vector3>(objectPointGroups_[n]), ConstTemplateArrayAccessor<Vector2>(imagePointGroups_[n]), weightedPoseErrors, sqrPoseErrors);

					measurements += objectPointGroups_[n].size();
				}

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
					Matrix debugJacobian(measurements_ * 2, 6);

					measurements = 0;

					for (size_t n = 0; n < flippedCameras_P_world_.size(); ++n)
					{
						const Vectors3& objectPoints = objectPointGroups_[n];

						Jacobian::calculateObjectTransformation2nx6(debugJacobian[measurements * 2], anyCamera_, flippedCameras_P_world_[n], world_P_object_, objectPoints.data(), objectPoints.size());

						measurements += objectPoints.size();
					}

					Matrix debugWeightMatrix;

					if (weights_.empty())
					{
						debugWeightMatrix = Matrix(measurements_ * 2, measurements_ * 2, true);
					}
					else
					{
						debugWeightMatrix = Matrix(measurements_ * 2, measurements_ * 2, Matrix(measurements_ * 2, 1, (Scalar*)weights_.data()));
					}

					const Matrix debugJTJ(debugJacobian.transposed() * debugWeightMatrix * debugJacobian);
					ocean_assert(debugJTJ.isEqual(hessian, Numeric::weakEps()));

					const Matrix debugErrorJacobian(debugJacobian.transposed() * Matrix(measurements_ * 2, 1, (Scalar*)debugWeightedErrors.data()));
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
			const Pose newPose(world_P_object_ - deltaPose);

			world_P_candidateObject_ = newPose;
		}

		/**
		 * Accepts the current model candidate a new (better) model than the previous one.
		 * @see AdvancedDenseOptimizationProvider::acceptCorrection().
		 */
		inline void acceptCorrection()
		{
			world_P_object_ = world_P_candidateObject_;
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

		/// The camera profile to be used.
		const AnyCamera& anyCamera_;

		/// The inverted and flipped camera poses, one for each group of image points.
		const HomogenousMatrices4& flippedCameras_P_world_;

		/// The 6-DOF object transformation to be optimized.
		Pose& world_P_object_;

		/// The candidate of an improved 6-DOF object transformation.
		Pose world_P_candidateObject_;

		/// The groups of 3D object points used for the optimization.
		const ObjectPointGroups& objectPointGroups_;

		/// The groups of 2D image points which are observations of the 3D object points in individual camera frames.
		const ImagePointGroups& imagePointGroups_;

		/// The estimator to be used as error measure.
		const Estimator::EstimatorType estimator_;

		/// The individual (weighted) errors, one for each correspondence.
		Vectors2 weightedErrors_;

		/// The individual weights, one for each correspondence, used for non-square estimators.
		Vectors2 weights_;

		/// The overall number of measurements (object points multiplied by number of cameras).
		size_t measurements_;
};

bool NonLinearOptimizationTransformation::optimizeObjectTransformationIF(const AnyCamera& anyCamera, const HomogenousMatrices4& flippedCameras_T_world, const HomogenousMatrix4& world_T_object, const ObjectPointGroups& objectPointGroups, const ImagePointGroups& imagePointGroups, HomogenousMatrix4& optimized_world_T_object, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{

#ifdef OCEAN_DEBUG

	ocean_assert(anyCamera.isValid());
	ocean_assert(flippedCameras_T_world.size() >= 1);
	ocean_assert(flippedCameras_T_world.size() == objectPointGroups.size());
	ocean_assert(flippedCameras_T_world.size() == imagePointGroups.size());
	ocean_assert(world_T_object.isValid() && world_T_object.rotationMatrix().isOrthonormal(Numeric::weakEps()));

	for (size_t n = 0; n < flippedCameras_T_world.size(); ++n)
	{
		ocean_assert(flippedCameras_T_world[n].isValid());
		ocean_assert(objectPointGroups[n].size() >= 1);
		ocean_assert(objectPointGroups[n].size() == imagePointGroups[n].size());
	}

#endif

	Pose objectTransformationPose(world_T_object);

	AdvancedObjectTransformationAnyCameraOptimizationProvider provider(anyCamera, flippedCameras_T_world, objectTransformationPose, objectPointGroups, imagePointGroups, estimator);
	if (!advancedDenseOptimization<AdvancedObjectTransformationAnyCameraOptimizationProvider>(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors))
	{
		return false;
	}

	optimized_world_T_object = objectTransformationPose.transformation();

	return true;
}

/**
 * An advanced optimization provider for stereo fisheye cameras.
 */
class NonLinearOptimizationTransformation::AdvancedObjectTransformationStereoAnyCameraOptimizationProvider : public NonLinearOptimization::AdvancedDenseOptimizationProvider
{
	public:

		inline AdvancedObjectTransformationStereoAnyCameraOptimizationProvider(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const HomogenousMatrices4& flippedCamerasA_T_world, const HomogenousMatrices4& flippedCamerasB_T_world, Pose& world_P_object, const ObjectPointGroups& objectPointGroupsA, const ObjectPointGroups& objectPointGroupsB, const ImagePointGroups& imagePointGroupsA, const ImagePointGroups& imagePointGroupsB, const Estimator::EstimatorType estimator) :
			anyCameraA_(anyCameraA),
			anyCameraB_(anyCameraB),
			flippedCamerasA_T_world_(flippedCamerasA_T_world),
			flippedCamerasB_T_world_(flippedCamerasB_T_world),
			world_P_object_(world_P_object),
			candidate_world_P_object_(world_P_object),
			objectPointGroupsA_(objectPointGroupsA),
			objectPointGroupsB_(objectPointGroupsB),
			imagePointGroupsA_(imagePointGroupsA),
			imagePointGroupsB_(imagePointGroupsB),
			estimator_(estimator),
			measurements_(0)
		{
			ocean_assert(anyCameraA_.isValid());
			ocean_assert(anyCameraB_.isValid());

			ocean_assert(flippedCamerasA_T_world_.size() == objectPointGroupsA_.size());
			ocean_assert(flippedCamerasA_T_world_.size() == imagePointGroupsA_.size());

			ocean_assert(flippedCamerasB_T_world_.size() == objectPointGroupsB_.size());
			ocean_assert(flippedCamerasB_T_world_.size() == imagePointGroupsB_.size());

			size_t maximalObjectPoints = 0;

			for (size_t n = 0; n < flippedCamerasA_T_world_.size(); ++n)
			{
				ocean_assert(flippedCamerasA_T_world_[n].isValid());
				ocean_assert(objectPointGroupsA_[n].size() == imagePointGroupsA_[n].size());

				measurements_ += objectPointGroupsA_[n].size();

				maximalObjectPoints = std::max(maximalObjectPoints, objectPointGroupsA_[n].size());
			}

			for (size_t n = 0; n < flippedCamerasB_T_world_.size(); ++n)
			{
				ocean_assert(flippedCamerasB_T_world_[n].isValid());
				ocean_assert(objectPointGroupsB_[n].size() == imagePointGroupsB_[n].size());

				measurements_ += objectPointGroupsB_[n].size();

				maximalObjectPoints = std::max(maximalObjectPoints, objectPointGroupsB_[n].size());
			}

			pointJacobians_.resize(maximalObjectPoints * 6);
			flippedCameraObjectPoints_.resize(maximalObjectPoints);
		};

		/**
		 * Determines the error for the current model candidate (not the actual model).
		 * @see AdvancedDenseOptimizationProvider::determineError().
		 */
		inline Scalar determineError()
		{
			// set the correct size of the resulting error vector
			weightedErrors_.resize(measurements_);

			const HomogenousMatrix4 candidate_world_T_object = candidate_world_P_object_.transformation();

			// check whether the standard estimator is used
			if (estimator_ == Estimator::ET_SQUARE)
			{
				// the weight vector should be and should stay invalid
				ocean_assert(weights_.empty());

				// determine the averaged square error

				Scalar sqrError = Scalar(0);

				size_t measurements = 0;

				for (unsigned int nStereo = 0u; nStereo < 2u; ++nStereo)
				{
					const AnyCamera& anyCamera = nStereo == 0u ? anyCameraA_ : anyCameraB_;
					const HomogenousMatrices4& flippedCameras_T_world = nStereo == 0u ? flippedCamerasA_T_world_ : flippedCamerasB_T_world_;
					const std::vector<Vectors3>& objectPointGroups = nStereo == 0u ? objectPointGroupsA_ : objectPointGroupsB_;
					const std::vector<Vectors2>& imagePointGroups = nStereo == 0u ? imagePointGroupsA_ : imagePointGroupsB_;

					for (size_t n = 0; n < flippedCameras_T_world.size(); ++n)
					{
						const HomogenousMatrix4 candidateIF(flippedCameras_T_world[n] * candidate_world_T_object);

						Vector2* weightedPoseErrors = weightedErrors_.data() + measurements;

						// determine the averaged square error
						const Scalar averagePoseSqrError = Error::determinePoseErrorIF<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, false>(candidateIF, anyCamera, ConstTemplateArrayAccessor<Vector3>(objectPointGroups[n]), ConstTemplateArrayAccessor<Vector2>(imagePointGroups[n]), weightedPoseErrors);

						// we will normalize the overall error at the end, we do not sum up averaged errors for individual poses
						sqrError += averagePoseSqrError * Scalar(objectPointGroups[n].size());

						measurements += objectPointGroups[n].size();
					}
				}

				return sqrError / Scalar(measurements_);
			}
			else
			{
				// now we need the weight vector
				weights_.resize(measurements_);

				Scalars sqrErrors(measurements_);

				size_t measurements = 0;

				for (unsigned int nStereo = 0u; nStereo < 2u; ++nStereo)
				{
					const AnyCamera& anyCamera = nStereo == 0u ? anyCameraA_ : anyCameraB_;
					const HomogenousMatrices4& flippedCameras_T_world = nStereo == 0u ? flippedCamerasA_T_world_ : flippedCamerasB_T_world_;
					const std::vector<Vectors3>& objectPointGroups = nStereo == 0u ? objectPointGroupsA_ : objectPointGroupsB_;
					const std::vector<Vectors2>& imagePointGroups = nStereo == 0u ? imagePointGroupsA_ : imagePointGroupsB_;

					for (size_t n = 0; n < flippedCameras_T_world.size(); ++n)
					{
						const HomogenousMatrix4 candidateIF(flippedCameras_T_world[n] * candidate_world_T_object);

						Vector2* const weightedPoseErrors = weightedErrors_.data() + measurements;
						Scalar* const sqrPoseErrors = sqrErrors.data() + measurements;

						Error::determinePoseErrorIF<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, true>(candidateIF, anyCamera, ConstTemplateArrayAccessor<Vector3>(objectPointGroups[n]), ConstTemplateArrayAccessor<Vector2>(imagePointGroups[n]), weightedPoseErrors, sqrPoseErrors);

						measurements += objectPointGroups[n].size();
					}
				}

				return NonLinearOptimization::sqrErrors2robustErrors2(estimator_, sqrErrors, 6, weightedErrors_.data(), weights_.data(), nullptr);
			}
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
			Jacobian::calculateRotationRodriguesDerivative(ExponentialMap(Vector3(world_P_object_.rx(), world_P_object_.ry(), world_P_object_.rz())), Rwx, Rwy, Rwz);

			const HomogenousMatrix4 world_T_object = world_P_object_.transformation();

			ocean_assert(weightedErrors_.size() == measurements_);
			ocean_assert(estimator_ == Estimator::ET_SQUARE || weights_.size() == measurements_);

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

			//Scalar xPointJacobian[3];
			//Scalar yPointJacobian[3];

			Scalar hessianValues[21] = {Scalar(0)};

			if (weights_.empty())
			{
				 // we use ET_SQUARE as estimator

				ocean_assert(weights_.empty());
				ocean_assert(weightedErrors_.size() == measurements_);

				size_t measurement = 0;

				for (unsigned int nStereo = 0u; nStereo < 2u; ++nStereo)
				{
					const AnyCamera& anyCamera = nStereo == 0u ? anyCameraA_ : anyCameraB_;
					const HomogenousMatrices4& flippedCameras_T_world = nStereo == 0u ? flippedCamerasA_T_world_ : flippedCamerasB_T_world_;
					const std::vector<Vectors3>& objectPointGroups = nStereo == 0u ? objectPointGroupsA_ : objectPointGroupsB_;
					const std::vector<Vectors2>& imagePointGroups = nStereo == 0u ? imagePointGroupsA_ : imagePointGroupsB_;

					for (size_t nPose = 0; nPose < flippedCameras_T_world.size(); ++nPose)
					{
						const HomogenousMatrix4& flippedCamera_T_world = flippedCameras_T_world[nPose];
						const HomogenousMatrix4 flippedCamera_T_object = flippedCamera_T_world * world_T_object;

						ocean_assert_and_suppress_unused(objectPointGroups[nPose].size() == imagePointGroups[nPose].size(), imagePointGroups);
						const Vectors3& objectPoints = objectPointGroups[nPose];

						// first we calculate the Jacobians for all points to avoid too many virtual functions calls

						ocean_assert(objectPoints.size() <= flippedCameraObjectPoints_.size());
						for (size_t nObject = 0; nObject < objectPoints.size(); ++nObject)
						{
							const Vector3& objectPoint = objectPoints[nObject];

							flippedCameraObjectPoints_[nObject] = flippedCamera_T_object * objectPoint;
						}

						ocean_assert(objectPoints.size() * 6 <= pointJacobians_.size());
						anyCamera.pointJacobian2nx3IF(flippedCameraObjectPoints_.data(), objectPoints.size(), pointJacobians_.data());

						for (size_t nObject = 0; nObject < objectPoints.size(); ++nObject)
						{
							const Vector3& objectPoint = objectPoints[nObject];

							//anyCamera.jacobian2x3IF(flippedCamera_T_object * objectPoint, xPointJacobian, yPointJacobian);

							const Scalar* xPointJacobian = pointJacobians_.data() + nObject * 6 + 0;
							const Scalar* yPointJacobian = pointJacobians_.data() + nObject * 6 + 3;

							const Scalar jFocalPoseXx = xPointJacobian[0] * flippedCamera_T_world[0] + xPointJacobian[1] * flippedCamera_T_world[1] + xPointJacobian[2] * flippedCamera_T_world[2];
							const Scalar jFocalPoseXy = xPointJacobian[0] * flippedCamera_T_world[4] + xPointJacobian[1] * flippedCamera_T_world[5] + xPointJacobian[2] * flippedCamera_T_world[6];
							const Scalar jFocalPoseXz = xPointJacobian[0] * flippedCamera_T_world[8] + xPointJacobian[1] * flippedCamera_T_world[9] + xPointJacobian[2] * flippedCamera_T_world[10];

							const Scalar jFocalPoseYx = yPointJacobian[0] * flippedCamera_T_world[0] + yPointJacobian[1] * flippedCamera_T_world[1] + yPointJacobian[2] * flippedCamera_T_world[2];
							const Scalar jFocalPoseYy = yPointJacobian[0] * flippedCamera_T_world[4] + yPointJacobian[1] * flippedCamera_T_world[5] + yPointJacobian[2] * flippedCamera_T_world[6];
							const Scalar jFocalPoseYz = yPointJacobian[0] * flippedCamera_T_world[8] + yPointJacobian[1] * flippedCamera_T_world[9] + yPointJacobian[2] * flippedCamera_T_world[10];

							const Vector3 dwx(Rwx * objectPoint);
							const Vector3 dwy(Rwy * objectPoint);
							const Vector3 dwz(Rwz * objectPoint);

							xJacobian[0] = jFocalPoseXx * dwx.x() + jFocalPoseXy * dwx.y() + jFocalPoseXz * dwx.z();
							xJacobian[1] = jFocalPoseXx * dwy.x() + jFocalPoseXy * dwy.y() + jFocalPoseXz * dwy.z();
							xJacobian[2] = jFocalPoseXx * dwz.x() + jFocalPoseXy * dwz.y() + jFocalPoseXz * dwz.z();
							xJacobian[3] = jFocalPoseXx;
							xJacobian[4] = jFocalPoseXy;
							xJacobian[5] = jFocalPoseXz;

							yJacobian[0] = jFocalPoseYx * dwx.x() + jFocalPoseYy * dwx.y() + jFocalPoseYz * dwx.z();
							yJacobian[1] = jFocalPoseYx * dwy.x() + jFocalPoseYy * dwy.y() + jFocalPoseYz * dwy.z();
							yJacobian[2] = jFocalPoseYx * dwz.x() + jFocalPoseYy * dwz.y() + jFocalPoseYz * dwz.z();
							yJacobian[3] = jFocalPoseYx;
							yJacobian[4] = jFocalPoseYy;
							yJacobian[5] = jFocalPoseYz;

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

							const Vector2& weightedError = weightedErrors_[measurement++];

							jacobianError(0) += xJacobian[0] * weightedError.x() + yJacobian[0] * weightedError.y();
							jacobianError(1) += xJacobian[1] * weightedError.x() + yJacobian[1] * weightedError.y();
							jacobianError(2) += xJacobian[2] * weightedError.x() + yJacobian[2] * weightedError.y();
							jacobianError(3) += xJacobian[3] * weightedError.x() + yJacobian[3] * weightedError.y();
							jacobianError(4) += xJacobian[4] * weightedError.x() + yJacobian[4] * weightedError.y();
							jacobianError(5) += xJacobian[5] * weightedError.x() + yJacobian[5] * weightedError.y();
						}
					}
				}
			}
			else
			{
				// we use a non-ET_SQUARE estimator

				ocean_assert(weights_.size() == measurements_);
				ocean_assert(weightedErrors_.size() == measurements_);

				size_t measurement = 0;

				for (unsigned int nStereo = 0u; nStereo < 2u; ++nStereo)
				{
					const AnyCamera& anyCamera = nStereo == 0u ? anyCameraA_ : anyCameraB_;
					const HomogenousMatrices4& flippedCameras_T_world = nStereo == 0u ? flippedCamerasA_T_world_ : flippedCamerasB_T_world_;
					const std::vector<Vectors3>& objectPointGroups = nStereo == 0u ? objectPointGroupsA_ : objectPointGroupsB_;
					const std::vector<Vectors2>& imagePointGroups = nStereo == 0u ? imagePointGroupsA_ : imagePointGroupsB_;

					for (size_t nPose = 0; nPose < flippedCameras_T_world.size(); ++nPose)
					{
						const HomogenousMatrix4& flippedCamera_T_world = flippedCameras_T_world[nPose];
						const HomogenousMatrix4 flippedCamera_T_object = flippedCamera_T_world * world_T_object;

						ocean_assert_and_suppress_unused(objectPointGroups[nPose].size() == imagePointGroups[nPose].size(), imagePointGroups);
						const Vectors3& objectPoints = objectPointGroups[nPose];

						// first we calculate the Jacobians for all points to avoid too many virtual functions calls

						ocean_assert(objectPoints.size() <= flippedCameraObjectPoints_.size());
						for (size_t nObject = 0; nObject < objectPoints.size(); ++nObject)
						{
							const Vector3& objectPoint = objectPoints[nObject];

							flippedCameraObjectPoints_[nObject] = flippedCamera_T_object * objectPoint;
						}

						ocean_assert(objectPoints.size() * 6 <= pointJacobians_.size());
						anyCamera.pointJacobian2nx3IF(flippedCameraObjectPoints_.data(), objectPoints.size(), pointJacobians_.data());

						for (size_t nObject = 0; nObject < objectPoints.size(); ++nObject)
						{
							const Vector3& objectPoint = objectPoints[nObject];

							//anyCamera.jacobian2x3IF(flippedCamera_T_object * objectPoint, xPointJacobian, yPointJacobian);

							const Scalar* xPointJacobian = pointJacobians_.data() + nObject * 6 + 0;
							const Scalar* yPointJacobian = pointJacobians_.data() + nObject * 6 + 3;

							const Scalar jFocalPoseXx = xPointJacobian[0] * flippedCamera_T_world[0] + xPointJacobian[1] * flippedCamera_T_world[1] + xPointJacobian[2] * flippedCamera_T_world[2];
							const Scalar jFocalPoseXy = xPointJacobian[0] * flippedCamera_T_world[4] + xPointJacobian[1] * flippedCamera_T_world[5] + xPointJacobian[2] * flippedCamera_T_world[6];
							const Scalar jFocalPoseXz = xPointJacobian[0] * flippedCamera_T_world[8] + xPointJacobian[1] * flippedCamera_T_world[9] + xPointJacobian[2] * flippedCamera_T_world[10];

							const Scalar jFocalPoseYx = yPointJacobian[0] * flippedCamera_T_world[0] + yPointJacobian[1] * flippedCamera_T_world[1] + yPointJacobian[2] * flippedCamera_T_world[2];
							const Scalar jFocalPoseYy = yPointJacobian[0] * flippedCamera_T_world[4] + yPointJacobian[1] * flippedCamera_T_world[5] + yPointJacobian[2] * flippedCamera_T_world[6];
							const Scalar jFocalPoseYz = yPointJacobian[0] * flippedCamera_T_world[8] + yPointJacobian[1] * flippedCamera_T_world[9] + yPointJacobian[2] * flippedCamera_T_world[10];

							const Vector3 dwx(Rwx * objectPoint);
							const Vector3 dwy(Rwy * objectPoint);
							const Vector3 dwz(Rwz * objectPoint);

							xJacobian[0] = jFocalPoseXx * dwx.x() + jFocalPoseXy * dwx.y() + jFocalPoseXz * dwx.z();
							xJacobian[1] = jFocalPoseXx * dwy.x() + jFocalPoseXy * dwy.y() + jFocalPoseXz * dwy.z();
							xJacobian[2] = jFocalPoseXx * dwz.x() + jFocalPoseXy * dwz.y() + jFocalPoseXz * dwz.z();
							xJacobian[3] = jFocalPoseXx;
							xJacobian[4] = jFocalPoseXy;
							xJacobian[5] = jFocalPoseXz;

							yJacobian[0] = jFocalPoseYx * dwx.x() + jFocalPoseYy * dwx.y() + jFocalPoseYz * dwx.z();
							yJacobian[1] = jFocalPoseYx * dwy.x() + jFocalPoseYy * dwy.y() + jFocalPoseYz * dwy.z();
							yJacobian[2] = jFocalPoseYx * dwz.x() + jFocalPoseYy * dwz.y() + jFocalPoseYz * dwz.z();
							yJacobian[3] = jFocalPoseYx;
							yJacobian[4] = jFocalPoseYy;
							yJacobian[5] = jFocalPoseYz;

							const Vector2 weight = weights_[measurement];
							const Vector2& weightedError = weightedErrors_[measurement];
							++measurement;

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
				}
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
			const Pose newPose(world_P_object_ - deltaPose);

			candidate_world_P_object_ = newPose;
		}

		/**
		 * Accepts the current model candidate a new (better) model than the previous one.
		 * @see AdvancedDenseOptimizationProvider::acceptCorrection().
		 */
		inline void acceptCorrection()
		{
			world_P_object_ = candidate_world_P_object_;
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

		/// The first stereo camera profile to be used.
		const AnyCamera& anyCameraA_;

		/// The first stereo camera profile to be used.
		const AnyCamera& anyCameraB_;

		/// The inverted and flipped camera poses for the first stereo camera, one for each group of image points.
		const HomogenousMatrices4& flippedCamerasA_T_world_;

		/// The inverted and flipped camera poses for the second stereo camera, one for each group of image points.
		const HomogenousMatrices4& flippedCamerasB_T_world_;

		/// The 6-DOF object transformation to be optimized.
		Pose& world_P_object_;

		/// The candidate of an improved 6-DOF object transformation.
		Pose candidate_world_P_object_;

		/// The groups of 3D object points for the first stereo camera.
		const ObjectPointGroups& objectPointGroupsA_;

		/// The groups of 3D object points for the second stereo camera.
		const ObjectPointGroups& objectPointGroupsB_;

		/// The groups of 2D image points which are observations of the 3D object points in individual first stereo camera frames.
		const ImagePointGroups& imagePointGroupsA_;

		/// The groups of 2D image points which are observations of the 3D object points in individual second stereo camera frames.
		const ImagePointGroups& imagePointGroupsB_;

		/// The estimator to be used as error measure.
		const Estimator::EstimatorType estimator_;

		/// The individual (weighted) errors, one for each correspondence.
		Vectors2 weightedErrors_;

		/// The individual weights, one for each correspondence, used for non-square estimators.
		Vectors2 weights_;

		/// The overall number of measurements (object points multiplied by number of cameras).
		size_t measurements_;

		/// Intermediate buffer for jacobian calculations.
		std::vector<Scalar> pointJacobians_;

		/// Intermediate buffer for 3D object points.
		Vectors3 flippedCameraObjectPoints_;
};

bool NonLinearOptimizationTransformation::optimizeObjectTransformationStereoIF(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const HomogenousMatrices4& flippedCamerasA_T_world, const HomogenousMatrices4& flippedCamerasB_T_world, const HomogenousMatrix4& world_T_object, const ObjectPointGroups& objectPointGroupsA, const ObjectPointGroups& objectPointGroupsB, const ImagePointGroups& imagePointGroupsA, const ImagePointGroups& imagePointGroupsB, HomogenousMatrix4& optimized_world_T_object, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{

#ifdef OCEAN_DEBUG

	ocean_assert(anyCameraA.isValid());
	ocean_assert(anyCameraB.isValid());

	ocean_assert(flippedCamerasA_T_world.size() == objectPointGroupsA.size());
	ocean_assert(flippedCamerasA_T_world.size() == imagePointGroupsA.size());

	ocean_assert(flippedCamerasB_T_world.size() == objectPointGroupsB.size());
	ocean_assert(flippedCamerasB_T_world.size() == imagePointGroupsB.size());

	ocean_assert(world_T_object.isValid() && world_T_object.rotationMatrix().isOrthonormal(Numeric::weakEps()));

	for (size_t n = 0; n < flippedCamerasA_T_world.size(); ++n)
	{
		ocean_assert(flippedCamerasA_T_world[n].isValid());
		ocean_assert(objectPointGroupsA[n].size() == imagePointGroupsA[n].size());
		ocean_assert(objectPointGroupsA[n].size() >= 1);
	}

	for (size_t n = 0; n < flippedCamerasB_T_world.size(); ++n)
	{
		ocean_assert(flippedCamerasB_T_world[n].isValid());
		ocean_assert(objectPointGroupsB[n].size() == imagePointGroupsB[n].size());
		ocean_assert(objectPointGroupsB[n].size() >= 1);
	}

#endif

	Pose world_P_object(world_T_object);

	AdvancedObjectTransformationStereoAnyCameraOptimizationProvider provider(anyCameraA, anyCameraB, flippedCamerasA_T_world, flippedCamerasB_T_world, world_P_object, objectPointGroupsA, objectPointGroupsB, imagePointGroupsA, imagePointGroupsB, estimator);
	if (!advancedDenseOptimization<AdvancedObjectTransformationStereoAnyCameraOptimizationProvider>(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors))
	{
		return false;
	}

	optimized_world_T_object = world_P_object.transformation();

	return true;
}

}

}
