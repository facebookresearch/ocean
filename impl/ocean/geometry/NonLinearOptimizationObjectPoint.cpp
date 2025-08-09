/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/NonLinearOptimizationObjectPoint.h"
#include "ocean/geometry/Error.h"
#include "ocean/geometry/Jacobian.h"
#include "ocean/geometry/NonLinearUniversalOptimizationSparse.h"

#include "ocean/math/ExponentialMap.h"
#include "ocean/math/Pose.h"
#include "ocean/math/SphericalExponentialMap.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements an optimization provider for one 3D object point that is visible under several individual camera poses seen with a fisheye camera.
 * The provider optimizes the 3D position by minimizing the projection error between the 3D object point and the individual 2D observation positions.<br>
 */
class NonLinearOptimizationObjectPoint::CameraObjectPointProvider : public NonLinearOptimization::OptimizationProvider
{
	public:

		/**
		 * Creates a new optimization provider object.
		 * @param camera The camera profile to be used, must be valid
		 * @param flippedCameras_T_world Inverted and flipped poses in that the object point is visible
		 * @param objectPoint 3D object point for that the position has to be optimized
		 * @param imagePointAccessor 2D observation image points, each point corresponds to one camera pose
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 */
		inline CameraObjectPointProvider(const AnyCamera& camera, const ConstIndexedAccessor<HomogenousMatrix4>& flippedCameras_T_world, Vector3& objectPoint, const ConstIndexedAccessor<ImagePoint>& imagePoints, const bool onlyFrontObjectPoints) :
			camera_(camera),
			flippedCameras_T_world_(flippedCameras_T_world),
			objectPoint_(objectPoint),
			candidateObjectPoint_(objectPoint),
			imagePoints_(imagePoints),
			onlyFrontObjectPoints_(onlyFrontObjectPoints)
		{
			ocean_assert(flippedCameras_T_world.size() >= 2u);
			ocean_assert(flippedCameras_T_world.size() == imagePoints_.size());
		};

		/**
		 * Determines the jacobian matrix for the current pose.
		 * @param jacobian The resulting Jacobian matrix
		 */
		void determineJacobian(Matrix& jacobian) const
		{
			/**
			 * Jacobian matrix of the projection function evaluated at current pose, dimension 2n x 3
			 * Pose: fi, object point: p
			 * [ df1(p) / dp ]
			 * [ df2(p) / dp ]
			 * [ df3(p) / dp ]
			 * [ df4(p) / dp ]
			 * [     ...     ]
			 * [ dfn(p) / dp ]
			 *
			 */

			jacobian.resize(2 * flippedCameras_T_world_.size(), 3);

			for (size_t n = 0; n < flippedCameras_T_world_.size(); ++n)
			{
				Jacobian::calculatePointJacobian2x3IF(camera_, flippedCameras_T_world_[n], objectPoint_, jacobian[n * 2 + 0], jacobian[n * 2 + 1]);
			}
		}

		/**
		 * Applies the pose correction and stores the new pose as candidate
		 * @param deltas Optimization deltas that define the correction
		 */
		inline void applyCorrection(const Matrix& deltas)
		{
			ocean_assert(deltas.rows() == 3u && deltas.columns() == 1u);

			const Vector3 deltaObjectPoint(deltas(0), deltas(1), deltas(2));
			candidateObjectPoint_ = objectPoint_ - deltaObjectPoint;
		}

		/**
		 * Determines the robust error of the current candidate position.
		 * @param weightedErrorVector Resulting (weighted - if using a robust estimator) error vector
		 * @param weightVector Vector holding the weights that have already been applied to the error vector
		 * @param invertedCovariances Optional 2x2 inverted covariance matrices
		 * @return The robust error
		 */
		template <Estimator::EstimatorType tEstimator>
		Scalar determineRobustError(Matrix& weightedErrorVector, Matrix& weightVector, const Matrix* invertedCovariances) const
		{
			// currently covariances are not supported
			ocean_assert(invertedCovariances == nullptr);

			// set the correct size of the resulting error vector
			weightedErrorVector.resize(2 * flippedCameras_T_world_.size(), 1);

			Vector2* const weightedErrors = (Vector2*)(weightedErrorVector.data());
			const SquareMatrix2* transposedInvertedCovariances = invertedCovariances ? (SquareMatrix2*)invertedCovariances->data() : nullptr;

			// check whether at least for one camera the current object point candidate lies behind the camera
			if (onlyFrontObjectPoints_ && !checkFrontObjectPoint())
			{
				return Numeric::maxValue();
			}

			Scalar sqrErrorSum = 0;
			Scalars sqrErrors(flippedCameras_T_world_.size());

			// determine projective errors
			for (size_t n = 0u; n < flippedCameras_T_world_.size(); ++n)
			{
				const ImagePoint imagePoint(camera_.projectToImageIF(flippedCameras_T_world_[n], candidateObjectPoint_));
				const ImagePoint& realImagePoint = imagePoints_[n];

				const Vector2 difference(imagePoint - realImagePoint);

				weightedErrors[n] = difference;

				sqrErrorSum += difference.sqr();

				// if not the standard estimator is used we also need the individual square errors
				if constexpr (!Estimator::isStandardEstimator<tEstimator>())
				{
					sqrErrors[n] = difference.sqr();
				}
			}

			// check whether the standard estimator is used
			if constexpr (Estimator::isStandardEstimator<tEstimator>())
			{
				ocean_assert(!weightVector);

				// return the averaged square error
				return sqrErrorSum / Scalar(flippedCameras_T_world_.size());
			}
			else
			{
				// now we need the weight vector
				weightVector.resize(2 * flippedCameras_T_world_.size(), 1);

				return sqrErrors2robustErrors2<tEstimator>(sqrErrors, 3, weightedErrors, (Vector2*)weightVector.data(), transposedInvertedCovariances);
			}
		}

		/**
		 * Accepts the current pose candidate as better model.
		 */
		inline void acceptCorrection()
		{
			objectPoint_ = candidateObjectPoint_;
		}

	protected:

		/**
		 * Checks whether the object point lies in front of all candidate cameras.
		 * @return True, if so
		 */
		bool checkFrontObjectPoint() const
		{
			for (size_t n = 0; n < flippedCameras_T_world_.size(); ++n)
			{
				if (!AnyCamera::isObjectPointInFrontIF(flippedCameras_T_world_[n], candidateObjectPoint_))
				{
					return false;
				}
			}

			return true;
		}

	protected:

		/// The camera object defining the projection.
		const AnyCamera& camera_;

		/// Inverted and flipped poses of all cameras.
		const ConstIndexedAccessor<HomogenousMatrix4>& flippedCameras_T_world_;

		/// Object point that will be optimized.
		Vector3& objectPoint_;

		/// Intermediated object point that stores the most recent optimization result as candidate.
		Vector3 candidateObjectPoint_;

		/// The 2D observation positions in the cameras.
		const ConstIndexedAccessor<Vector2>& imagePoints_;

		/// True, forces the object point to stay in front of the cameras.
		const bool onlyFrontObjectPoints_;
};

bool NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedPosesIF(const AnyCamera& camera, const ConstIndexedAccessor<HomogenousMatrix4>& flippedCameras_T_world, const Vector3& worldObjectPoint, const ConstIndexedAccessor<Vector2>& imagePoints, Vector3& optimizedWorldObjectPoint, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialRobustError, Scalar* finalRobustError, Scalars* intermediateRobustErrors)
{
	ocean_assert(camera.isValid());
	ocean_assert(&worldObjectPoint != &optimizedWorldObjectPoint);
	ocean_assert(flippedCameras_T_world.size() >= 2);
	ocean_assert(flippedCameras_T_world.size() == imagePoints.size());

	optimizedWorldObjectPoint = worldObjectPoint;

	CameraObjectPointProvider provider(camera, flippedCameras_T_world, optimizedWorldObjectPoint, imagePoints, onlyFrontObjectPoints);
	return denseOptimization(provider, iterations, estimator, lambda, lambdaFactor, initialRobustError, finalRobustError, nullptr, intermediateRobustErrors);
}

/**
 * This class implements an optimization provider for one 3D object point that is visible under several individual camera poses seen with a fisheye camera.
 * The provider optimizes the 3D position by minimizing the projection error between the 3D object point and the individual 2D observation positions.<br>
 */
class NonLinearOptimizationObjectPoint::CamerasObjectPointProvider : public NonLinearOptimization::OptimizationProvider
{
	public:

		/**
		 * Creates a new optimization provider object.
		 * @param cameras The camera profiles to be used, must be valid
		 * @param flippedCameras_T_world Inverted and flipped poses in that the object point is visible
		 * @param objectPoint 3D object point for that the position has to be optimized
		 * @param imagePointAccessor 2D observation image points, each point corresponds to one camera pose
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 */
		inline CamerasObjectPointProvider(const ConstIndexedAccessor<const AnyCamera*>& cameras, const ConstIndexedAccessor<HomogenousMatrix4>& flippedCameras_T_world, Vector3& objectPoint, const ConstIndexedAccessor<ImagePoint>& imagePoints, const bool onlyFrontObjectPoints) :
			cameras_(cameras),
			flippedCameras_T_world_(flippedCameras_T_world),
			objectPoint_(objectPoint),
			candidateObjectPoint_(objectPoint),
			imagePoints_(imagePoints),
			onlyFrontObjectPoints_(onlyFrontObjectPoints)
		{
			ocean_assert(flippedCameras_T_world.size() >= 2u);
			ocean_assert(flippedCameras_T_world.size() == cameras_.size());
			ocean_assert(flippedCameras_T_world.size() == imagePoints_.size());
		};

		/**
		 * Determines the jacobian matrix for the current pose.
		 * @param jacobian The resulting Jacobian matrix
		 */
		void determineJacobian(Matrix& jacobian) const
		{
			/**
			 * Jacobian matrix of the projection function evaluated at current pose, dimension 2n x 3
			 * Pose: fi, object point: p
			 * [ df1(p) / dp ]
			 * [ df2(p) / dp ]
			 * [ df3(p) / dp ]
			 * [ df4(p) / dp ]
			 * [     ...     ]
			 * [ dfn(p) / dp ]
			 *
			 */

			jacobian.resize(2 * flippedCameras_T_world_.size(), 3);

			for (size_t n = 0; n < flippedCameras_T_world_.size(); ++n)
			{
				Jacobian::calculatePointJacobian2x3IF(*cameras_[n], flippedCameras_T_world_[n], objectPoint_, jacobian[n * 2 + 0], jacobian[n * 2 + 1]);
			}
		}

		/**
		 * Applies the pose correction and stores the new pose as candidate
		 * @param deltas Optimization deltas that define the correction
		 */
		inline void applyCorrection(const Matrix& deltas)
		{
			ocean_assert(deltas.rows() == 3u && deltas.columns() == 1u);

			const Vector3 deltaObjectPoint(deltas(0), deltas(1), deltas(2));
			candidateObjectPoint_ = objectPoint_ - deltaObjectPoint;
		}

		/**
		 * Determines the robust error of the current candidate position.
		 * @param weightedErrorVector Resulting (weighted - if using a robust estimator) error vector
		 * @param weightVector Vector holding the weights that have already been applied to the error vector
		 * @param invertedCovariances Optional 2x2 inverted covariance matrices
		 * @return The robust error
		 */
		template <Estimator::EstimatorType tEstimator>
		Scalar determineRobustError(Matrix& weightedErrorVector, Matrix& weightVector, const Matrix* invertedCovariances) const
		{
			// currently covariances are not supported
			ocean_assert(invertedCovariances == nullptr);

			// set the correct size of the resulting error vector
			weightedErrorVector.resize(2 * flippedCameras_T_world_.size(), 1);

			Vector2* const weightedErrors = (Vector2*)(weightedErrorVector.data());
			const SquareMatrix2* transposedInvertedCovariances = invertedCovariances ? (SquareMatrix2*)invertedCovariances->data() : nullptr;

			// check whether at least for one camera the current object point candidate lies behind the camera
			if (onlyFrontObjectPoints_ && !checkFrontObjectPoint())
			{
				return Numeric::maxValue();
			}

			Scalar sqrErrorSum = 0;
			Scalars sqrErrors(flippedCameras_T_world_.size());

			// determine projective errors
			for (size_t n = 0u; n < flippedCameras_T_world_.size(); ++n)
			{
				const ImagePoint imagePoint(cameras_[n]->projectToImageIF(flippedCameras_T_world_[n], candidateObjectPoint_));
				const ImagePoint& realImagePoint = imagePoints_[n];

				const Vector2 difference(imagePoint - realImagePoint);

				weightedErrors[n] = difference;

				sqrErrorSum += difference.sqr();

				// if not the standard estimator is used we also need the individual square errors
				if constexpr (!Estimator::isStandardEstimator<tEstimator>())
				{
					sqrErrors[n] = difference.sqr();
				}
			}

			// check whether the standard estimator is used
			if constexpr (Estimator::isStandardEstimator<tEstimator>())
			{
				ocean_assert(!weightVector);

				// return the averaged square error
				return sqrErrorSum / Scalar(flippedCameras_T_world_.size());
			}
			else
			{
				// now we need the weight vector
				weightVector.resize(2 * flippedCameras_T_world_.size(), 1);

				return sqrErrors2robustErrors2<tEstimator>(sqrErrors, 3, weightedErrors, (Vector2*)weightVector.data(), transposedInvertedCovariances);
			}
		}

		/**
		 * Accepts the current pose candidate as better model.
		 */
		inline void acceptCorrection()
		{
			objectPoint_ = candidateObjectPoint_;
		}

	protected:

		/**
		 * Checks whether the object point lies in front of all candidate cameras.
		 * @return True, if so
		 */
		bool checkFrontObjectPoint() const
		{
			for (size_t n = 0; n < flippedCameras_T_world_.size(); ++n)
			{
				if (!PinholeCamera::isObjectPointInFrontIF(flippedCameras_T_world_[n], candidateObjectPoint_))
				{
					return false;
				}
			}

			return true;
		}

	protected:

		/// The camera profiles defining the projection.
		const ConstIndexedAccessor<const AnyCamera*>& cameras_;

		/// Inverted and flipped poses of all cameras.
		const ConstIndexedAccessor<HomogenousMatrix4>& flippedCameras_T_world_;

		/// Object point that will be optimized.
		Vector3& objectPoint_;

		/// Intermediated object point that stores the most recent optimization result as candidate.
		Vector3 candidateObjectPoint_;

		/// The 2D observation positions in the cameras.
		const ConstIndexedAccessor<Vector2>& imagePoints_;

		/// True, forces the object point to stay in front of the cameras.
		const bool onlyFrontObjectPoints_;
};

bool NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedPosesIF(const ConstIndexedAccessor<const AnyCamera*>& cameras, const ConstIndexedAccessor<HomogenousMatrix4>& flippedCameras_T_world, const Vector3& worldObjectPoint, const ConstIndexedAccessor<Vector2>& imagePoints, Vector3& optimizedWorldObjectPoint, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialRobustError, Scalar* finalRobustError, Scalars* intermediateRobustErrors)
{
	ocean_assert(cameras.size() == flippedCameras_T_world.size());
	ocean_assert(&worldObjectPoint != &optimizedWorldObjectPoint);
	ocean_assert(flippedCameras_T_world.size() >= 2);
	ocean_assert(flippedCameras_T_world.size() == imagePoints.size());

	optimizedWorldObjectPoint = worldObjectPoint;

	CamerasObjectPointProvider provider(cameras, flippedCameras_T_world, optimizedWorldObjectPoint, imagePoints, onlyFrontObjectPoints);
	return denseOptimization(provider, iterations, estimator, lambda, lambdaFactor, initialRobustError, finalRobustError, nullptr, intermediateRobustErrors);
}

/**
 * This class implements an optimization provider for one 3D object point that is visible under several individual camera poses seen with any stereo camera.
 * The provider optimizes the 3D position by minimizing the projection error between the 3D object point and the individual 2D observation positions.
 */
class NonLinearOptimizationObjectPoint::StereoCameraObjectPointProvider : public NonLinearOptimization::OptimizationProvider
{
	public:

		/**
		 * Creates a new optimization provider object.
		 * @param anyCameraA First (e.g., left) camera profile to be used, must be valid
		 * @param anyCameraB Second (e.g., right) camera profile to be used, must be valid
		 * @param poseAccessor_world_T_cameraA Inverted and flipped poses for the first stereo camera frames
		 * @param poseAccessor_world_T_cameraB Inverted and flipped poses for the second stereo camera frames
		 * @param objectPoint 3D object point for that the position has to be optimized
		 * @param imagePointAccessorA 2D observation image points in the first stereo images, each point corresponds to one camera pose of the first (e.g., left) stereo camera
		 * @param imagePointAccessorB 2D observation image points in the second stereo images, each point corresponds to one camera pose of the second (e.g., right) stereo camera
		 * @param onlyFrontObjectPoints True, to avoid that the optimized 3D position lies behind any camera
		 */
		inline StereoCameraObjectPointProvider(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const ConstIndexedAccessor<HomogenousMatrix4>& flippedCamerasA_T_world, const ConstIndexedAccessor<HomogenousMatrix4>& flippedCamerasB_T_world, Vector3& objectPoint, const ConstIndexedAccessor<ImagePoint>& imagePointAccessorA, const ConstIndexedAccessor<ImagePoint>& imagePointAccessorB, const bool onlyFrontObjectPoints) :
			anyCameraA_(anyCameraA),
			anyCameraB_(anyCameraB),
			flippedCamerasA_T_world_(flippedCamerasA_T_world),
			flippedCamerasB_T_world_(flippedCamerasB_T_world),
			objectPoint_(objectPoint),
			candidateObjectPoint_(objectPoint),
			imagePointAccessorA_(imagePointAccessorA),
			imagePointAccessorB_(imagePointAccessorB),
			onlyFrontObjectPoints_(onlyFrontObjectPoints)
		{
			ocean_assert(flippedCamerasA_T_world.size() + flippedCamerasB_T_world.size() >= 2u);
			ocean_assert(flippedCamerasA_T_world.size() == imagePointAccessorA_.size());
			ocean_assert(flippedCamerasB_T_world.size() == imagePointAccessorB_.size());
		};

		/**
		 * Determines the jacobian matrix for the current pose.
		 * @param jacobian Jacobian matrix
		 */
		void determineJacobian(Matrix& jacobian) const
		{
			/**
			 * Jacobian matrix of the projection function evaluated at current pose, dimension 2n x 3
			 * Pose: fi, object point: p
			 * [ df1(p) / dp ]
			 * [ df2(p) / dp ]
			 * [ df3(p) / dp ]
			 * [ df4(p) / dp ]
			 * [     ...     ]
			 * [ dfn(p) / dp ]
			 *
			 */

			const size_t observations = flippedCamerasA_T_world_.size() + flippedCamerasB_T_world_.size();

			jacobian.resize(2 * observations, 3);

			size_t observationIndex = 0;

			for (size_t n = 0; n < flippedCamerasA_T_world_.size(); ++n)
			{
				Jacobian::calculatePointJacobian2x3IF(anyCameraA_, flippedCamerasA_T_world_[n], objectPoint_, jacobian[observationIndex * 2 + 0], jacobian[observationIndex * 2 + 1]);
				++observationIndex;
			}

			for (size_t n = 0; n < flippedCamerasB_T_world_.size(); ++n)
			{
				Jacobian::calculatePointJacobian2x3IF(anyCameraB_, flippedCamerasB_T_world_[n], objectPoint_, jacobian[observationIndex * 2 + 0], jacobian[observationIndex * 2 + 1]);
				++observationIndex;
			}

			ocean_assert(observationIndex == observations);
		}

		/**
		 * Applies the pose correction and stores the new pose as candidate
		 * @param deltas Optimization deltas that define the correction
		 */
		inline void applyCorrection(const Matrix& deltas)
		{
			ocean_assert(deltas.rows() == 3u && deltas.columns() == 1u);

			const Vector3 deltaObjectPoint(deltas(0), deltas(1), deltas(2));
			candidateObjectPoint_ = objectPoint_ - deltaObjectPoint;
		}

		/**
		 * Determines the robust error of the current candidate position.
		 * @param weightedErrorVector Resulting (weighted - if using a robust estimator) error vector
		 * @param weightVector Vector holding the weights that have already been applied to the error vector
		 * @param invertedCovariances Optional 2x2 inverted covariance matrices
		 */
		template <Estimator::EstimatorType tEstimator>
		Scalar determineRobustError(Matrix& weightedErrorVector, Matrix& weightVector, const Matrix* invertedCovariances) const
		{
			// currently covariances are not supported
			ocean_assert_and_suppress_unused(invertedCovariances == nullptr, invertedCovariances);

			const size_t observations = flippedCamerasA_T_world_.size() + flippedCamerasB_T_world_.size();

			// set the correct size of the resulting error vector
			weightedErrorVector.resize(2 * observations, 1);

			Vector2* const weightedErrors = (Vector2*)weightedErrorVector.data();
			constexpr SquareMatrix2* transposedInvertedCovariances = nullptr;

			// check whether at least for one camera the current object point candidate lies behind the camera
			if (onlyFrontObjectPoints_ && !checkFrontObjectPoint())
			{
				return Numeric::maxValue();
			}

			size_t observationIndex = 0;

			Scalar sqrErrorSum = 0;
			Scalars sqrErrors(observations);

			// determine projective errors for the first stereo cameras
			for (size_t n = 0u; n < flippedCamerasA_T_world_.size(); ++n)
			{
				const ImagePoint imagePoint(anyCameraA_.projectToImageIF(flippedCamerasA_T_world_[n], candidateObjectPoint_));
				const ImagePoint& realImagePoint = imagePointAccessorA_[n];

				const Vector2 difference(imagePoint - realImagePoint);

				weightedErrors[observationIndex] = difference;

				sqrErrorSum += difference.sqr();

				// if not the standard estimator is used we also need the individual sqr errors
				if constexpr (!Estimator::isStandardEstimator<tEstimator>())
				{
					sqrErrors[observationIndex] = difference.sqr();
				}

				++observationIndex;
			}

			// determine projective errors for the second stereo cameras
			for (size_t n = 0u; n < flippedCamerasB_T_world_.size(); ++n)
			{
				const ImagePoint imagePoint(anyCameraB_.projectToImageIF(flippedCamerasB_T_world_[n], candidateObjectPoint_));
				const ImagePoint& realImagePoint = imagePointAccessorB_[n];

				const Vector2 difference(imagePoint - realImagePoint);

				weightedErrors[observationIndex] = difference;

				sqrErrorSum += difference.sqr();

				// if not the standard estimator is used we also need the individual sqr errors
				if constexpr (!Estimator::isStandardEstimator<tEstimator>())
				{
					sqrErrors[observationIndex] = difference.sqr();
				}

				++observationIndex;
			}

			ocean_assert(observations == observationIndex);

			// check whether the standard estimator is used
			if constexpr (Estimator::isStandardEstimator<tEstimator>())
			{
				ocean_assert(!weightVector);

				// return the averaged square error
				return sqrErrorSum / Scalar(observations);
			}
			else
			{
				// now we need the weight vector
				weightVector.resize(2 * observations, 1);

				return sqrErrors2robustErrors2<tEstimator>(sqrErrors, 3, weightedErrors, (Vector2*)(weightVector.data()), transposedInvertedCovariances);
			}
		}

		/**
		 * Accepts the current pose candidate as better model.
		 */
		inline void acceptCorrection()
		{
			objectPoint_ = candidateObjectPoint_;
		}

	protected:

		/**
		 * Checks whether the object point lies in front of all candidate cameras.
		 * @return True, if so
		 */
		bool checkFrontObjectPoint() const
		{
			for (size_t n = 0; n < flippedCamerasA_T_world_.size(); ++n)
			{
				if (!PinholeCamera::isObjectPointInFrontIF(flippedCamerasA_T_world_[n], candidateObjectPoint_))
				{
					return false;
				}
			}

			for (size_t n = 0; n < flippedCamerasB_T_world_.size(); ++n)
			{
				if (!PinholeCamera::isObjectPointInFrontIF(flippedCamerasB_T_world_[n], candidateObjectPoint_))
				{
					return false;
				}
			}

			return true;
		}

	protected:

		/// The camera object of first stereo camera.
		const AnyCamera& anyCameraA_;

		/// The camera object of second stereo camera.
		const AnyCamera& anyCameraB_;

		/// Inverted and flipped poses of all first stereo cameras.
		const ConstIndexedAccessor<HomogenousMatrix4>& flippedCamerasA_T_world_;

		/// Inverted and flipped poses of all second stereo cameras.
		const ConstIndexedAccessor<HomogenousMatrix4>& flippedCamerasB_T_world_;

		/// Object point that will be optimized.
		Vector3& objectPoint_;

		/// Intermediated object point that stores the most recent optimization result as candidate.
		Vector3 candidateObjectPoint_;

		/// The 2D observation positions in the first stereo cameras.
		const ConstIndexedAccessor<Vector2>& imagePointAccessorA_;

		/// The 2D observation positions in the second stereo cameras.
		const ConstIndexedAccessor<Vector2>& imagePointAccessorB_;

		/// True, forces the object point to stay in front of the cameras.
		const bool onlyFrontObjectPoints_;
};

bool NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedStereoPosesIF(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const ConstIndexedAccessor<HomogenousMatrix4>& flippedCamerasA_T_world, const ConstIndexedAccessor<HomogenousMatrix4>& flippedCamerasB_T_world, const Vector3& worldObjectPoint, const ConstIndexedAccessor<Vector2>& imagePointAccessorA, const ConstIndexedAccessor<Vector2>& imagePointAccessorB, Vector3& optimizedWorldObjectPoint, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialRobustError, Scalar* finalRobustError, Scalars* intermediateRobustErrors)
{
	ocean_assert(&worldObjectPoint != &optimizedWorldObjectPoint);
	ocean_assert(flippedCamerasA_T_world.size() + flippedCamerasB_T_world.size() >= 2);
	ocean_assert(flippedCamerasA_T_world.size() == imagePointAccessorA.size());
	ocean_assert(flippedCamerasB_T_world.size() == imagePointAccessorB.size());

	optimizedWorldObjectPoint = worldObjectPoint;

	StereoCameraObjectPointProvider provider(anyCameraA, anyCameraB, flippedCamerasA_T_world, flippedCamerasB_T_world, optimizedWorldObjectPoint, imagePointAccessorA, imagePointAccessorB, onlyFrontObjectPoints);
	return denseOptimization(provider, iterations, estimator, lambda, lambdaFactor, initialRobustError, finalRobustError, nullptr, intermediateRobustErrors);
}

bool NonLinearOptimizationObjectPoint::optimizeObjectPointsForFixedPosesIF(const PinholeCamera& camera, const ConstIndexedAccessor<HomogenousMatrix4>& invertedFlippedPoses, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, const bool distortImagePoints, NonconstIndexedAccessor<Vector3>& optimizedObjectPoints, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Worker* worker)
{
	ocean_assert(!objectPoints.isEmpty() && !invertedFlippedPoses.isEmpty());
	ocean_assert(objectPoints.size() == correspondenceGroups.groups());

	ocean_assert(lambda >= 0);
	ocean_assert(lambdaFactor >= 1);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&NonLinearOptimizationObjectPoint::optimizeObjectPointsForFixedPosesIFSubset, &camera, &invertedFlippedPoses, &objectPoints, &correspondenceGroups, distortImagePoints, &optimizedObjectPoints, iterations, estimator, lambda, lambdaFactor, onlyFrontObjectPoints, 0u, 0u), 0u, (unsigned int)objectPoints.size(), (unsigned int)(-1), (unsigned int)(-1), 20u);
	}
	else
	{
		optimizeObjectPointsForFixedPosesIFSubset(&camera, &invertedFlippedPoses, &objectPoints, &correspondenceGroups, distortImagePoints, &optimizedObjectPoints, iterations, estimator, lambda, lambdaFactor, onlyFrontObjectPoints, 0u, (unsigned int)objectPoints.size());
	}

	return true;
}

/**
 * This class implements a provider allowing to optimize one 3D object point location for several camera poses (with rotational camera poses).
 * The position (translation) of the camera is expected to be the origin of the coordinate system in which the 3D object point location is defined.<br>
 */
class NonLinearOptimizationObjectPoint::SphericalObjectPointProvider : public NonLinearOptimization::OptimizationProvider
{
	public:

		/**
		 * Creates a new optimization provider object.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param flippedCameras_R_world The inverted and flipped poses (orientation) of the individual camera frames
		 * @param imagePoints The 2D observation image points, each point corresponds to one camera orientation
		 * @param objectPoint Spherical 3D object point for which the position has to be optimized
		 * @param objectPointDistance The distance (radius) between the origin of the camera poses and the 3D object point location
		 * @param distortImagePoints True, to apply the distortion parameters of the camera
		 * @param onlyFrontObjectPoint True, to avoid that the optimized 3D position lies behind any camera
		 */
		inline SphericalObjectPointProvider(const AnyCamera& camera, const ConstIndexedAccessor<SquareMatrix3>& flippedCameras_R_world, const ConstIndexedAccessor<ImagePoint>& imagePoints, ExponentialMap& objectPoint, const Scalar objectPointDistance, const bool onlyFrontObjectPoint) :
			camera_(camera),
			flippedCamera_R_world_(flippedCameras_R_world),
			providerImagePoints(imagePoints),
			objectPoint_(objectPoint),
			candidateObjectPoint_(objectPoint),
			objectPointDistance_(objectPointDistance),
			onlyFrontObjectPoint_(onlyFrontObjectPoint)
		{
			ocean_assert(flippedCamera_R_world_.size() > 1);
			ocean_assert(flippedCamera_R_world_.size() == providerImagePoints.size());
		};

		/**
		 * Determines the jacobian matrix for the current pose.
		 * @param jacobian Jacobian matrix
		 */
		void determineJacobian(Matrix& jacobian) const
		{
			jacobian.resize(2 * flippedCamera_R_world_.size(), 3);

			for (size_t n = 0; n < flippedCamera_R_world_.size(); ++n)
			{
				Jacobian::calculateSphericalObjectPointOrientationJacobian2x3IF(jacobian[n * 2 + 0], jacobian[n * 2 + 1], camera_, flippedCamera_R_world_[n], objectPoint_, objectPointDistance_);
			}
		}

		/**
		 * Applies the pose correction and stores the new pose as candidate
		 * @param deltas Optimization deltas that define the correction
		 */
		inline void applyCorrection(const Matrix& deltas)
		{
			ocean_assert(deltas.rows() == 3u && deltas.columns() == 1u);

			const ExponentialMap deltaObjectPoint(deltas(0), deltas(1), deltas(2));
			candidateObjectPoint_ = objectPoint_ - deltaObjectPoint;
		}

		/**
		 * Determines the robust error of the current candidate position.
		 * @param weightedErrorVector Resulting (weighted - if using a robust estimator) error vector
		 * @param weightVector Vector holding the weights that have already been applied to the error vector
		 * @param invertedCovariances Optional 2x2 inverted covariance matrices
		 * @return The robust error
		 */
		template <Estimator::EstimatorType tEstimator>
		Scalar determineRobustError(Matrix& weightedErrorVector, Matrix& weightVector, const Matrix* invertedCovariances) const
		{
			// currently covariances are not supported
			ocean_assert(invertedCovariances == nullptr);

			// set the correct size of the resulting error vector
			weightedErrorVector.resize(2 * flippedCamera_R_world_.size(), 1);

			Vector2* const weightedErrors = (Vector2*)(weightedErrorVector.data());
			const SquareMatrix2* transposedInvertedCovariances = invertedCovariances ? (SquareMatrix2*)invertedCovariances->data() : nullptr;

			// check whether at least for one camera the current object point candidate lies behind the camera
			if (onlyFrontObjectPoint_ && !checkFrontObjectPoint())
			{
				return Numeric::maxValue();
			}

			Scalar sqrErrorSum = 0;
			Scalars sqrErrors(flippedCamera_R_world_.size());

			const Vector3 candidateObjectPoint(candidateObjectPoint_.rotation() * Vector3(0, 0, -objectPointDistance_));

			// determine projective errors
			for (size_t n = 0u; n < flippedCamera_R_world_.size(); ++n)
			{
				const ImagePoint imagePoint(camera_.projectToImageIF(HomogenousMatrix4(flippedCamera_R_world_[n]), candidateObjectPoint));
				const ImagePoint& realImagePoint = providerImagePoints[n];

				const Vector2 difference(imagePoint - realImagePoint);

				weightedErrors[n] = difference;

				sqrErrorSum += difference.sqr();

				// if not the standard estimator is used we also need the individual sqr errors
				if constexpr (!Estimator::isStandardEstimator<tEstimator>())
				{
					sqrErrors[n] = difference.sqr();
				}
			}

			// check whether the standard estimator is used
			if constexpr (Estimator::isStandardEstimator<tEstimator>())
			{
				ocean_assert(!weightVector);

				// return the averaged square error
				return sqrErrorSum / Scalar(flippedCamera_R_world_.size());
			}
			else
			{
				// now we need the weight vector
				weightVector.resize(2 * flippedCamera_R_world_.size(), 1);

				return sqrErrors2robustErrors2<tEstimator>(sqrErrors, 2, weightedErrors, (Vector2*)(weightVector.data()), transposedInvertedCovariances);
			}
		}

		/**
		 * Accepts the current pose candidate as better model.
		 */
		inline void acceptCorrection()
		{
			objectPoint_ = candidateObjectPoint_;
		}

	protected:

		/**
		 * Checks whether the object point lies in front of all candidate cameras.
		 * @return True, if so
		 */
		bool checkFrontObjectPoint() const
		{
			const Vector3 candidateObjectPoint(candidateObjectPoint_.rotation() * Vector3(0, 0, -objectPointDistance_));

			for (size_t n = 0; n < flippedCamera_R_world_.size(); ++n)
			{
				if (!PinholeCamera::isObjectPointInFrontIF(flippedCamera_R_world_[n], candidateObjectPoint))
				{
					return false;
				}
			}

			return true;
		}

	protected:

		/// The camera profile.
		const AnyCamera& camera_;

		/// The rotations transforming world to flipped camera.
		const ConstIndexedAccessor<SquareMatrix3>& flippedCamera_R_world_;

		/// The 2D observation positions in the individual camera frames.
		const ConstIndexedAccessor<ImagePoint>& providerImagePoints;

		/// Object point that will be optimized.
		ExponentialMap& objectPoint_;

		/// Intermediated object point that stores the most recent optimization result as candidate.
		ExponentialMap candidateObjectPoint_;

		/// The distance (radius) between the origin of the camera poses and the 3D object point location.
		const Scalar objectPointDistance_;

		/// True, forces the object point to stay in front of the camera.s
		const bool onlyFrontObjectPoint_;
};

bool NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedOrientationsIF(const AnyCamera& camera, const ConstIndexedAccessor<SquareMatrix3>& flippedCameras_R_world, const ConstIndexedAccessor<Vector2>& imagePoints, const Vector3& objectPoint, const Scalar objectPointDistance, Vector3& optimizedObjectPoint, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator, const Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoint, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
	ocean_assert(camera.isValid());
	ocean_assert(flippedCameras_R_world.size() == imagePoints.size());
	ocean_assert(&objectPoint != &optimizedObjectPoint);
	ocean_assert(objectPointDistance > Numeric::eps());
	ocean_assert(!objectPoint.isNull());

	if (objectPoint.isNull())
	{
		return false;
	}

	optimizedObjectPoint = objectPoint;

	const Vector3 objectPointDirection(objectPoint.normalized());

#ifdef OCEAN_DEBUG
	for (size_t n = 0; n < flippedCameras_R_world.size(); ++n)
	{
		ocean_assert(AnyCamera::isObjectPointInFrontIF(flippedCameras_R_world[n], objectPoint));
	}
#endif

	ExponentialMap sphericalObjectPoint(Rotation(Vector3(0, 0, -1), objectPointDirection));
	ocean_assert((sphericalObjectPoint.rotation() * Vector3(0, 0, -1)).isEqual(objectPointDirection, Numeric::weakEps()));

	Scalar intermediateFinalError = Numeric::maxValue();

	SphericalObjectPointProvider provider(camera, flippedCameras_R_world, imagePoints, sphericalObjectPoint, objectPointDistance, onlyFrontObjectPoint);
	if (!denseOptimization<SphericalObjectPointProvider>(provider, iterations, estimator, lambda, lambdaFactor, initialError, &intermediateFinalError, nullptr, intermediateErrors))
	{
		return false;
	}

	if (finalError != nullptr)
	{
		*finalError = intermediateFinalError;
	}

	optimizedObjectPoint = sphericalObjectPoint.rotation() * Vector3(0, 0, -objectPointDistance);
	return true;
}

/**
 * This class implements a data object allowing to optimize 3D object point locations and 3-DOF camera orientations concurrently.
 * The position (translation) of the camera is expected to be the origin of the coordinate system in which the 3D object point locations are defined.<br>
 * This data object allows to optimize 3D object points observed in individual camera poses (while each camera pose may observe an arbitrary number of object points).<br>
 */
class NonLinearOptimizationObjectPoint::ObjectPointsOrientationsData
{
	public:

		/**
		 * Creates a new data object.
		 * @param pinholeCamera The pinhole camera profile defining the projection
		 * @param radius The radius between the origin of the camera poses and the 3D object point locations
		 * @param correspondenceGroups The groups of correspondences between pose indices and image points, one group for each object point
		 * @param onlyFrontObjectPoints True, to avoid that an optimized 3D position lies behind any camera in which the object point is visible
		 */
		ObjectPointsOrientationsData(const PinholeCamera& camera, const Scalar radius, const ObjectPointGroupsAccessor& correspondenceGroups, const bool onlyFrontObjectPoints) :
			camera_(camera),
			correspondenceGroups_(correspondenceGroups),
			radius_(radius),
			onlyFrontObjectPoints_(onlyFrontObjectPoints)
		{
			ocean_assert(Numeric::isNotEqualEps(radius_));

#ifdef OCEAN_DEBUG
			ocean_assert(correspondenceGroups.groups() != 0);
			for (size_t g = 0; g < correspondenceGroups.groups(); ++g)
			{
				ocean_assert(correspondenceGroups.groupElements(g) != 0);
			}
#endif
		}

		/**
		 * Determines the value for a specified 3D object point and specified element (pair of camera pose and image point location).
		 * @param externalFirstModels The current first models to be applied (the inverted and flipped poses of the individual camera frames)
		 * @param externalSecondModels The current second model to be applied (the locations of the individual 3D object points)
		 * @param secondModelIndex The index of the second model (the index of the 3D object point)
		 * @param elementIndex The index of the correspondence between pose id and image point visible in that pose
		 * @param result The resulting value which is the projected 3D object points
		 * @return The index of the pose for the specified element index
		 */
		size_t value(const std::vector< StaticBuffer<Scalar, 16> >& externalFirstModels, const std::vector< StaticBuffer<Scalar, 3> >& externalSecondModels, const size_t secondModelIndex, const size_t elementIndex, StaticBuffer<Scalar, 2>& result)
		{
			const Index32 poseIndex = correspondenceGroups_.firstElement(secondModelIndex, elementIndex);
			ocean_assert(poseIndex < externalFirstModels.size());

			const HomogenousMatrix4& poseIF = *((HomogenousMatrix4*)(externalFirstModels[poseIndex].data()));
			const Vector3& objectPoint = *((Vector3*)(externalSecondModels[secondModelIndex].data()));

			const Vector2 point = camera_.projectToImageIF<true>(poseIF, objectPoint, camera_.hasDistortionParameters());

			result[0] = point[0];
			result[1] = point[1];

			return size_t(poseIndex);
		}

		/**
		 * Determines the error between a projected 3D object point and the expected point for the given models.
		 * @param externalFirstModels The current first models to be applied (the inverted and flipped poses of the individual camera frames)
		 * @param externalSecondModels The current second model to be applied (the locations of the individual 3D object points)
		 * @param secondModelIndex The index of the second model (the index of the 3D object point)
		 * @param elementIndex The index of the correspondence between pose id and image point visible in that pose
		 * @param result The resulting projection error
		 * @return True, if succeeded
		 */
		bool error(const std::vector< StaticBuffer<Scalar, 16> >& externalFirstModels, const std::vector< StaticBuffer<Scalar, 3> >& externalSecondModels, const size_t secondModelIndex, const size_t elementIndex, StaticBuffer<Scalar, 2>& result)
		{
			Index32 poseIndex;
			Vector2 imagePoint;

			correspondenceGroups_.element(secondModelIndex, elementIndex, poseIndex, imagePoint);
			ocean_assert(poseIndex < externalFirstModels.size());

			const HomogenousMatrix4& poseIF = *((HomogenousMatrix4*)(externalFirstModels[poseIndex].data()));
			const Vector3& objectPoint = *((Vector3*)(externalSecondModels[secondModelIndex].data()));

			// ensure that the object point lies in front of the camera
			if (onlyFrontObjectPoints_ && !PinholeCamera::isObjectPointInFrontIF(poseIF, objectPoint))
			{
				return false;
			}

			const Vector2 point = camera_.projectToImageIF<true>(poseIF, objectPoint, camera_.hasDistortionParameters());
			const Vector2 error(point - imagePoint);

			result[0] = error[0];
			result[1] = error[1];

			return true;
		}

		/**
		 * Transforms an internal first model to the external model.
		 * @param internalModel The internal model, which is the 3-DOF camera orientation represented by 3 scalar parameters (exponential map)
		 * @param externalModel The external model, which is the 6-DOF inverted and flipped pose represented by 16 scalar parameters (4x4 transformation matrix)
		 */
		void transformFirstModel(StaticBuffer<Scalar, 3>& internalModel, StaticBuffer<Scalar, 16>& externalModel)
		{
			*((HomogenousMatrix4*)externalModel.data()) = PinholeCamera::standard2InvertedFlipped(HomogenousMatrix4(ExponentialMap(internalModel[0], internalModel[1], internalModel[2]).rotation()));
		}

		/**
		 * Transforms an internal second model to the external model.
		 * @param internalModel The internal model
		 * @param externalModel The external model
		 */
		void transformSecondModel(StaticBuffer<Scalar, 2>& internalModel, StaticBuffer<Scalar, 3>& externalModel)
		{
			*((Vector3*)externalModel.data()) = model2objectPoint(Vector2(internalModel[0], internalModel[1]), radius_);
		}

		/**
		 * Determines the model for a given 3D object point location.
		 * @param objectPoint The object point for which the model will be determined
		 * @return The resulting model
		 */
		static Vector2 objectPoint2model(const Vector3& objectPoint)
		{
			const Vector3 normalizedObjectPoint(objectPoint.normalized());

			const SphericalExponentialMap exponentialMap(Vector3(0, 0, -1), normalizedObjectPoint);
			const Vector2 value(exponentialMap.data());

			return value;
		}

		/**
		 * Determines the 3D object point location from a given model (representing the object point).
		 * @param model The model for which the corresponding 3D object point location will be determined
		 * @param radius The radius between origin and object points
		 * @return The resulting 3D object point location
		 */
		static Vector3 model2objectPoint(const Vector2& model, const Scalar radius)
		{
			const SphericalExponentialMap exponentialMap(model[0], model[1]);

			return exponentialMap.rotation() * Vector3(0, 0, -radius);
		}

	protected:

		/// The camera profile defining the projection.
		const PinholeCamera& camera_;

		/// The groups of correspondences between pose indices and image points, one group for each object point
		const ObjectPointGroupsAccessor& correspondenceGroups_;

		/// The radius between origin and object points.
		const Scalar radius_;

		/// True, to avoid that an optimized 3D position lies behind any camera in which the object point is visible.
		const bool onlyFrontObjectPoints_;
};

bool NonLinearOptimizationObjectPoint::optimizeObjectPointsAndOrientations(const PinholeCamera& camera, const ConstIndexedAccessor<SquareMatrix3>& orientations, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, const Scalar objectPointDistance, NonconstIndexedAccessor<SquareMatrix3>* optimizedOrientations, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator, const Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
	ocean_assert(camera.isValid());
	ocean_assert(objectPoints.size() == correspondenceGroups.groups());
	ocean_assert(orientations.size() >= 1);
	ocean_assert(objectPoints.size() >= 1);

	ocean_assert(optimizedOrientations == nullptr || optimizedOrientations->size() == orientations.size());
	ocean_assert(optimizedObjectPoints == nullptr || optimizedObjectPoints->size() == objectPoints.size());

	ocean_assert(objectPointDistance > Numeric::eps());

	// orientations
	typedef StaticBuffer<Scalar, 3> FirstModel;
	typedef std::vector<FirstModel> FirstModels;

	// object points
	typedef StaticBuffer<Scalar, 2> SecondModel;
	typedef std::vector<SecondModel> SecondModels;

	FirstModels firstModels;
	firstModels.reserve(orientations.size());
	for (size_t n = 0; n < orientations.size(); ++n)
	{
		const ExponentialMap exponentialMap(orientations[n]);
		firstModels.push_back(FirstModel(exponentialMap.data()));
	}

	SecondModels secondModels;
	secondModels.reserve(objectPoints.size());
	for (size_t n = 0; n < objectPoints.size(); ++n)
	{
		const Vector2 model(ObjectPointsOrientationsData::objectPoint2model(objectPoints[n]));
		secondModels.push_back(SecondModel(model.data()));
	}

	std::vector<size_t> numberElementsPerIndividualModel;
	numberElementsPerIndividualModel.reserve(correspondenceGroups.groups());

	for (size_t i = 0; i < correspondenceGroups.groups(); ++i)
	{
		ocean_assert(correspondenceGroups.groupElements(i) != 0);
		numberElementsPerIndividualModel.push_back(correspondenceGroups.groupElements(i));
	}

#ifdef OCEAN_DEBUG
	for (size_t g = 0; g < correspondenceGroups.groups(); ++g)
	{
		ocean_assert(correspondenceGroups.groupElements(g) >= 1);
		for (size_t e = 0; e < correspondenceGroups.groupElements(g); ++e)
		{
			Index32 poseIndex;
			Vector2 imagePoint;

			correspondenceGroups.element(g, e, poseIndex, imagePoint);

			const Vector3& objectPoint = objectPoints[g];
			const SquareMatrix3& orientation = orientations[poseIndex];

			const Vector2 projectedObjectPoint = camera.projectToImage<true>(HomogenousMatrix4(orientation), objectPoint, camera.hasDistortionParameters());
			const Scalar sqrDistance = projectedObjectPoint.sqrDistance(imagePoint);

			ocean_assert(sqrDistance <= 50 * 50);
		}
	}
#endif

	typedef Geometry::NonLinearUniversalOptimizationSparse::IndividualModelsIndividualModels<3u, 2u, 2u, 16u, 3u> UniversalOptimization;

	UniversalOptimization::FirstModels optimizedFirstModels;
	UniversalOptimization::SecondModels optimizedSecondModels;

	ObjectPointsOrientationsData data(camera, objectPointDistance, correspondenceGroups, onlyFrontObjectPoints);

	if (!UniversalOptimization::optimizeUniversalModel(firstModels, secondModels, numberElementsPerIndividualModel.data(),
					UniversalOptimization::ValueCallback::create(data, &ObjectPointsOrientationsData::value),
					UniversalOptimization::ErrorCallback::create(data, &ObjectPointsOrientationsData::error),
					UniversalOptimization::FirstModelTransformationCallback::create(data, &ObjectPointsOrientationsData::transformFirstModel),
					UniversalOptimization::SecondModelTransformationCallback::create(data, &ObjectPointsOrientationsData::transformSecondModel),
					UniversalOptimization::ModelAcceptedCallback(),
					optimizedFirstModels, optimizedSecondModels, iterations, estimator, lambda, lambdaFactor, initialError, finalError, intermediateErrors))
	{
		return false;
	}

	if (optimizedOrientations)
	{
		ocean_assert(optimizedOrientations->size() == orientations.size());

		for (size_t i = 0; i < optimizedFirstModels.size(); ++i)
		{
			const FirstModel& firstModel = optimizedFirstModels[i];

			const ExponentialMap exponentialMap(firstModel[0], firstModel[1], firstModel[2]);
			(*optimizedOrientations)[i] = SquareMatrix3(exponentialMap.rotation());
		}
	}

	if (optimizedObjectPoints)
	{
		ocean_assert(optimizedObjectPoints->size() == objectPoints.size());

		for (size_t i = 0; i < optimizedSecondModels.size(); ++i)
		{
			const SecondModel& secondModel = optimizedSecondModels[i];

			(*optimizedObjectPoints)[i] = ObjectPointsOrientationsData::model2objectPoint(Vector2(secondModel[0], secondModel[1]), objectPointDistance);
		}
	}

	return true;
}

/**
 * This class implements an optimization provider allowing to optimize a 6-DOF camera pose and 3-DOF object points concurrently (for a stereo vision problem while the first camera pose is static).
 * This provider is highly optimized not using sparse matrices but implements the individual sub-matrices of the sparse linear equation by small dense sub-matrices.
 * We divided the sparse Hessian matrix into four sub-matrices:
 *     | A B |   |  A  B |
 * H = | C D | = | B^T D |
 *
 * @tparam tEstimator The type of the estimator to be used
 */
template <Estimator::EstimatorType tEstimator>
class NonLinearOptimizationObjectPoint::ObjectPointsOnePoseProvider : public NonLinearOptimization::AdvancedSparseOptimizationProvider
{
	public:

		/**
		 * Creates a new provider object.
		 * @param pinholeCamera The pinhole camera profile defining the projection
		 * @param firstPoseIF The inverted and flipped camera pose of the first frame
		 * @param secondPoseIF The inverted and flipped camera pose of the second frame
		 * @param objectPoints The 3D object point locations which will be optimized
		 * @param firstImagePoints The (static) image points visible in the first camera frame
		 * @param secondImagePoints The (static) image points visible in the second camera frame
		 * @param correspondences The number of points correspondences, with range [5, infinity)
		 * @param useDistortionParameters True, to use the distortion parameters of the camera profile
		 * @param onlyFrontObjectPoints True, to ensure that all 3D object point locations will lie in front of both cameras
		 */
		inline ObjectPointsOnePoseProvider(const PinholeCamera& camera, const HomogenousMatrix4& firstPoseIF, HomogenousMatrix4& secondPoseIF, Vector3* objectPoints, const Vector2* firstImagePoints, const Vector2* secondImagePoints, const size_t correspondences, const bool useDistortionParameters, const bool onlyFrontObjectPoints) :
			camera_(camera),
			firstFlippedCamera_T_world_(firstPoseIF),
			secondFlippedCamera_T_world_(secondPoseIF),
			secondCandidateFlippedCamera_T_world_(secondPoseIF),
			objectPoints_(objectPoints),
			objectPointCandidates_(correspondences),
			firstImagePoints_(firstImagePoints),
			secondImagePoints_(secondImagePoints),
			correspondences_(correspondences),
			useDistortionParameters_(useDistortionParameters),
			onlyFrontObjectPoints_(onlyFrontObjectPoints),
			matrixB_(correspondences),
			matrixD_(correspondences),
			matrixInvertedD_(correspondences),
			jacobianErrorVector_(6 + correspondences * 3),
			diagonalMatrixD_(correspondences * 3)
		{
			ocean_assert(correspondences >= 5);
			memcpy(objectPointCandidates_.data(), objectPoints, sizeof(Vector3) * correspondences);
		}

		/**
		 * Determines the error for the current model candidate (not the actual/actual model).
		 * @see AdvancedSparseOptimizationProvider::determineError().
		 */
		inline Scalar determineError()
		{
			Scalar sqrError = 0;

			if constexpr (!Estimator::isStandardEstimator<tEstimator>())
			{
				intermediateSqrErrors_.resize(correspondences_ * 2u);
			}

			for (size_t n = 0; n < correspondences_; ++n)
			{
				const Vector3& objectPoint = objectPointCandidates_[n];

				if (onlyFrontObjectPoints_ && (!PinholeCamera::isObjectPointInFrontIF(firstFlippedCamera_T_world_, objectPoint) || !PinholeCamera::isObjectPointInFrontIF(secondCandidateFlippedCamera_T_world_, objectPoint)))
				{
					return Numeric::maxValue();
				}

				const Scalar firstSqrError = Error::determinePoseErrorIF(firstFlippedCamera_T_world_, camera_, objectPoint, firstImagePoints_[n], useDistortionParameters_).sqr();
				const Scalar secondSqrError = Error::determinePoseErrorIF(secondCandidateFlippedCamera_T_world_, camera_, objectPoint, secondImagePoints_[n], useDistortionParameters_).sqr();

				if constexpr (Estimator::isStandardEstimator<tEstimator>())
				{
					sqrError += firstSqrError + secondSqrError;
				}
				else
				{
					intermediateSqrErrors_[2 * n + 0] = firstSqrError;
					intermediateSqrErrors_[2 * n + 1] = secondSqrError;
				}
			}

			if constexpr (Estimator::isStandardEstimator<tEstimator>())
			{
				ocean_assert(correspondences_ != 0);
				return sqrError / Scalar(correspondences_ * 2);
			}
			else
			{
				ocean_assert(!intermediateSqrErrors_.empty());
				return Estimator::determineRobustError<tEstimator>(intermediateSqrErrors_.data(), intermediateSqrErrors_.size(), 6 + correspondences_ * 3);
			}
		}

		/**
		 * Determines any kind of (abstract) parameters based on the current/actual model (not the model candidate) e.g., the Jacobian parameters and/or a Hessian matrix.
		 * @see AdvancedSparseOptimizationProvider::determineParameters().
		 */
		inline bool determineParameters()
		{
			Scalar poseJacobianSecondPoseX[6], poseJacobianSecondPoseY[6];
			Scalar pointJacobianFirstPoseX[3], pointJacobianFirstPoseY[3];
			Scalar pointJacobianSecondPoseX[3], pointJacobianSecondPoseY[3];

			SquareMatrix3 dwx, dwy, dwz;

			const Pose pose(secondFlippedCamera_T_world_);
			Jacobian::calculateRotationRodriguesDerivative(ExponentialMap(pose.rx(), pose.ry(), pose.rz()), dwx, dwy, dwz);

			matrixA_.toNull();

			// as the first 6 error values are accumulated we set these values to zero only, the remaining error values will be set directly
			for (size_t n = 0; n < 6; ++n)
			{
				jacobianErrorVector_[n] = Scalar(0);
			}

			intermediateErrors_.resize(correspondences_ * 2);

			if constexpr (!Estimator::isStandardEstimator<tEstimator>())
			{
				intermediateSqrErrors_.resize(correspondences_ * 2u);
				intermediateWeights_.resize(correspondences_ * 2u);
			}

			for (size_t n = 0; n < correspondences_; ++n)
			{
				const Vector3& objectPoint = objectPoints_[n];

				intermediateErrors_[2 * n + 0] = Error::determinePoseErrorIF(firstFlippedCamera_T_world_, camera_, objectPoint, firstImagePoints_[n], useDistortionParameters_);
				intermediateErrors_[2 * n + 1] = Error::determinePoseErrorIF(secondFlippedCamera_T_world_, camera_, objectPoint, secondImagePoints_[n], useDistortionParameters_);

				if constexpr (!Estimator::isStandardEstimator<tEstimator>())
				{
					intermediateSqrErrors_[2 * n + 0] = intermediateErrors_[2 * n + 0].sqr();
					intermediateSqrErrors_[2 * n + 1] = intermediateErrors_[2 * n + 1].sqr();
				}
			}

			if constexpr (!Estimator::isStandardEstimator<tEstimator>())
			{
				const Scalar sqrSigma = Estimator::needSigma<tEstimator>() ? Numeric::sqr(Estimator::determineSigmaSquare<tEstimator>(intermediateSqrErrors_.data(), intermediateSqrErrors_.size(), 6 + correspondences_ * 3)) : 0;

				for (size_t n = 0; n < 2 * correspondences_; ++n)
				{
					// we determine the weights, however as e.g., the tukey estimator may return a weight of 0 we have to clamp the weight to ensure that we still can solve the equation
					// **NOTE** the much better way would be to remove the entry from the equation and to solve it
					intermediateWeights_[n] = max(Numeric::weakEps(), Estimator::robustWeightSquare<tEstimator>(intermediateSqrErrors_[n], sqrSigma));
				}
			}

			for (size_t n = 0; n < correspondences_; ++n)
			{
				const Vector3& objectPoint = objectPoints_[n];

				Jacobian::calculatePoseJacobianRodrigues2x6(poseJacobianSecondPoseX, poseJacobianSecondPoseY, camera_, secondFlippedCamera_T_world_, objectPoint, useDistortionParameters_, dwx, dwy, dwz);
				Jacobian::calculatePointJacobian2x3(pointJacobianFirstPoseX, pointJacobianFirstPoseY, camera_, firstFlippedCamera_T_world_, objectPoint, useDistortionParameters_);
				Jacobian::calculatePointJacobian2x3(pointJacobianSecondPoseX, pointJacobianSecondPoseY, camera_, secondFlippedCamera_T_world_, objectPoint, useDistortionParameters_);

				SquareMatrix3& subMatrixD = matrixD_[n];
				StaticMatrix6x3& subMatrixB = matrixB_[n];

				if constexpr (Estimator::isStandardEstimator<tEstimator>())
				{
					// we calculate the upper triangle of the matrix A (as A is symmetric)
					for (size_t r = 0; r < 6; ++r)
					{
						for (size_t c = r; c < 6; ++c)
						{
							matrixA_(r, c) += poseJacobianSecondPoseX[r] * poseJacobianSecondPoseX[c] + poseJacobianSecondPoseY[r] * poseJacobianSecondPoseY[c];
						}
					}

					// we calculate the upper triangle(s) of the matrix D (as D is symmetric)
					for (unsigned int r = 0u; r < 3u; ++r)
					{
						for (unsigned int c = r; c < 3u; ++c)
						{
							subMatrixD(r, c) = pointJacobianFirstPoseX[r] * pointJacobianFirstPoseX[c] + pointJacobianFirstPoseY[r] * pointJacobianFirstPoseY[c] + pointJacobianSecondPoseX[r] * pointJacobianSecondPoseX[c] + pointJacobianSecondPoseY[r] * pointJacobianSecondPoseY[c];
						}
					}

					// we calculate the matrix B
					for (size_t r = 0; r < 6; ++r)
					{
						for (size_t c = 0; c < 3; ++c)
						{
							subMatrixB(r, c) = poseJacobianSecondPoseX[r] * pointJacobianSecondPoseX[c] + poseJacobianSecondPoseY[r] * pointJacobianSecondPoseY[c];
						}
					}
				}
				else
				{
					const Scalar& firstRobustWeight = intermediateWeights_[2 * n + 0];
					const Scalar& secondRobustWeight = intermediateWeights_[2 * n + 1];

					// we calculate the upper triangle of the matrix A (as A is symmetric)
					for (size_t r = 0; r < 6; ++r)
					{
						for (size_t c = r; c < 6; ++c)
						{
							matrixA_(r, c) += (poseJacobianSecondPoseX[r] * poseJacobianSecondPoseX[c] + poseJacobianSecondPoseY[r] * poseJacobianSecondPoseY[c]) * secondRobustWeight;
						}
					}

					// we calculate the upper triangle(s) of the matrix D (as D is symmetric)
					for (unsigned int r = 0u; r < 3u; ++r)
					{
						for (unsigned int c = r; c < 3u; ++c)
						{
							subMatrixD(r, c) = (pointJacobianFirstPoseX[r] * pointJacobianFirstPoseX[c] + pointJacobianFirstPoseY[r] * pointJacobianFirstPoseY[c]) * firstRobustWeight
													+ (pointJacobianSecondPoseX[r] * pointJacobianSecondPoseX[c] + pointJacobianSecondPoseY[r] * pointJacobianSecondPoseY[c]) * secondRobustWeight;
						}
					}

					// we calculate the matrix B
					for (size_t r = 0; r < 6; ++r)
					{
						for (size_t c = 0; c < 3; ++c)
						{
							subMatrixB(r, c) = (poseJacobianSecondPoseX[r] * pointJacobianSecondPoseX[c] + poseJacobianSecondPoseY[r] * pointJacobianSecondPoseY[c]) * secondRobustWeight;
						}
					}
				}

				// we copy the lower triangle
				subMatrixD(1, 0) = subMatrixD(0, 1);
				subMatrixD(2, 0) = subMatrixD(0, 2);
				subMatrixD(2, 1) = subMatrixD(1, 2);

				ocean_assert(!subMatrixD.isNull());

				ocean_assert(intermediateErrors_[2 * n + 0] == Error::determinePoseErrorIF(firstFlippedCamera_T_world_, camera_, objectPoint, firstImagePoints_[n], useDistortionParameters_));
				ocean_assert(intermediateErrors_[2 * n + 1] == Error::determinePoseErrorIF(secondFlippedCamera_T_world_, camera_, objectPoint, secondImagePoints_[n], useDistortionParameters_));

				if constexpr (Estimator::isStandardEstimator<tEstimator>())
				{
					const Vector2& firstError = intermediateErrors_[2 * n + 0];
					const Vector2& secondError = intermediateErrors_[2 * n + 1];

					for (size_t i = 0; i < 6; ++i)
					{
						jacobianErrorVector_[i] += poseJacobianSecondPoseX[i] * secondError[0] + poseJacobianSecondPoseY[i] * secondError[1];
					}

					for (size_t i = 0; i < 3; ++i)
					{
						jacobianErrorVector_[6 + 3 * n + i] = pointJacobianFirstPoseX[i] * firstError[0] + pointJacobianFirstPoseY[i] * firstError[1] + pointJacobianSecondPoseX[i] * secondError[0] + pointJacobianSecondPoseY[i] * secondError[1];
					}
				}
				else
				{
					const Vector2 firstError(intermediateErrors_[2 * n + 0] * intermediateWeights_[2 * n + 0]);
					const Vector2 secondError(intermediateErrors_[2 * n + 1] * intermediateWeights_[2 * n + 1]);

					for (size_t i = 0; i < 6; ++i)
					{
						jacobianErrorVector_[i] += poseJacobianSecondPoseX[i] * secondError[0] + poseJacobianSecondPoseY[i] * secondError[1];
					}

					for (size_t i = 0; i < 3; ++i)
					{
						jacobianErrorVector_[6 + 3 * n + i] = pointJacobianFirstPoseX[i] * firstError[0] + pointJacobianFirstPoseY[i] * firstError[1] + pointJacobianSecondPoseX[i] * secondError[0] + pointJacobianSecondPoseY[i] * secondError[1];
					}
				}
			}

			// we copy the lower triangle from the upper triangle
			for (size_t r = 1; r < 6; ++r)
			{
				for (size_t c = 0; c < r; ++c)
				{
					matrixA_(r, c) = matrixA_(c, r);
				}
			}

			// we make a copy of the diagonal elements of matrix A so that we can apply a lambda later during the solve step
			for (size_t n = 0; n < 6; ++n)
			{
				diagonalMatrixA_[n] = matrixA_(n, n);
			}

#ifdef OCEAN_INTENSIVE_DEBUG

	#ifndef OCEAN_DEBUG
			#error Invalid debug state!
	#endif

			if (std::is_same<Scalar, double>::value)
			{
				Scalar pointJacobianBuffer[6];
				Scalar poseJacobianBuffer[12];

				SparseMatrix::Entries jacobianEntries;

				for (size_t n = 0; n < correspondences_; ++n)
				{
					const Vector3& objectPoint = objectPoints_[n];

					Jacobian::calculatePoseJacobianRodrigues2x6(poseJacobianBuffer, poseJacobianBuffer + 6, camera_, secondFlippedCamera_T_world_, objectPoint, useDistortionParameters_, dwx, dwy, dwz);

					for (size_t e = 0; e < 6; ++e)
					{
						jacobianEntries.push_back(SparseMatrix::Entry(4 * n + 2, e, poseJacobianBuffer[0 + e]));
						jacobianEntries.push_back(SparseMatrix::Entry(4 * n + 3, e, poseJacobianBuffer[6 + e]));
					}

					Jacobian::calculatePointJacobian2x3(pointJacobianBuffer, pointJacobianBuffer + 3, camera_, firstFlippedCamera_T_world_, objectPoint, useDistortionParameters_);

					for (size_t e = 0; e < 3; ++e)
					{
						jacobianEntries.push_back(SparseMatrix::Entry(4 * n + 0, 6 + 3 * n + e, pointJacobianBuffer[0 + e]));
						jacobianEntries.push_back(SparseMatrix::Entry(4 * n + 1, 6 + 3 * n + e, pointJacobianBuffer[3 + e]));
					}

					Jacobian::calculatePointJacobian2x3(pointJacobianBuffer, pointJacobianBuffer + 3, camera_, secondFlippedCamera_T_world_, objectPoint, useDistortionParameters_);

					for (size_t e = 0; e < 3; ++e)
					{
						jacobianEntries.push_back(SparseMatrix::Entry(4 * n + 2, 6 + 3 * n + e, pointJacobianBuffer[0 + e]));
						jacobianEntries.push_back(SparseMatrix::Entry(4 * n + 3, 6 + 3 * n + e, pointJacobianBuffer[3 + e]));
					}
				}

				debugJacobian_ = SparseMatrix(4 * correspondences_, 6 + 3 * correspondences_, jacobianEntries);

				const Scalar debugSqrSigma = Estimator::needSigma<tEstimator>() ? Numeric::sqr(Estimator::determineSigmaSquare<tEstimator>(intermediateSqrErrors_.data(), intermediateSqrErrors_.size(), 6 + correspondences_ * 3)) : 0;

				SparseMatrix::Entries weightEntries;
				for (size_t n = 0; n < 4 * correspondences_; ++n)
				{
					if constexpr (Estimator::isStandardEstimator<tEstimator>())
					{
						weightEntries.push_back(SparseMatrix::Entry(n, n, Scalar(1)));
					}
					else
					{
						const Scalar weight = max(Numeric::weakEps(), Estimator::robustWeightSquare<tEstimator>(intermediateSqrErrors_[n / 2], debugSqrSigma));
						weightEntries.push_back(SparseMatrix::Entry(n, n, weight));
					}
				}

				SparseMatrix debugWeight(4 * correspondences_, 4 * correspondences_, weightEntries);

				debugHessian_ = debugJacobian_.transposed() * debugWeight * debugJacobian_;

				const SparseMatrix subMatrixA(debugHessian_.submatrix(0, 0, 6, 6));
				const SparseMatrix subMatrixB(debugHessian_.submatrix(0, 6, 6, 3 * correspondences_));
				const SparseMatrix subMatrixC(debugHessian_.submatrix(6, 0, 3 * correspondences_, 6));
				const SparseMatrix subMatrixD(debugHessian_.submatrix(6, 6, 3 * correspondences_, 3 * correspondences_));

				for (size_t r = 0; r < 6; ++r)
				{
					for (size_t c = 0; c < 6; ++c)
					{
						const Scalar value0 = matrixA_(r, c);
						const Scalar value1 = subMatrixA(r, c);
						ocean_assert(Numeric::isWeakEqual(value0, value1));
					}
				}

				for (size_t n = 0; n < correspondences_; ++n)
				{
					for (unsigned int r = 0u; r < 6u; ++r)
					{
						for (unsigned int c = 0u; c < 3u; ++c)
						{
							const Scalar value0 = matrixB_[n](r, c);
							const Scalar value1 = subMatrixB(r, n * 3 + c);
							ocean_assert(Numeric::isWeakEqual(value0, value1));

							const Scalar value2 = subMatrixC(n * 3 + c, r);
							ocean_assert(Numeric::isWeakEqual(value0, value2));
						}
					}
				}

				for (size_t n = 0; n < correspondences_; ++n)
				{
					for (unsigned int r = 0u; r < 3u; ++r)
					{
						for (unsigned int c = 0u; c < 3u; ++c)
						{
							const Scalar value0 = matrixD_[n](r, c);
							const Scalar value1 = subMatrixD(n * 3 + r, n * 3 + c);
							ocean_assert(Numeric::isWeakEqual(value0, value1));
						}
					}
				}

				debugJacobianError_.resize(4 * correspondences_, 1);

				for (size_t n = 0; n < correspondences_; ++n)
				{
					const Vector3& objectPoint = objectPoints_[n];

					const Vector2 firstError = Error::determinePoseErrorIF(firstFlippedCamera_T_world_, camera_, objectPoint, firstImagePoints_[n], useDistortionParameters_);
					const Vector2 secondError = Error::determinePoseErrorIF(secondFlippedCamera_T_world_, camera_, objectPoint, secondImagePoints_[n], useDistortionParameters_);

					debugJacobianError_(4 * n + 0, 0) = firstError.x();
					debugJacobianError_(4 * n + 1, 0) = firstError.y();
					debugJacobianError_(4 * n + 2, 0) = secondError.x();
					debugJacobianError_(4 * n + 3, 0) = secondError.y();
				}

				debugJacobianError_ = debugJacobian_.transposed() * debugWeight * debugJacobianError_;
				ocean_assert(debugJacobianError_.rows() == jacobianErrorVector_.size());
				ocean_assert(debugJacobianError_.columns() == 1);

				for (size_t n = 0; n < jacobianErrorVector_.size(); ++n)
					ocean_assert(Numeric::isWeakEqual(jacobianErrorVector_[n], debugJacobianError_(n, 0)));
			}
#endif // OCEAN_INTENSIVE_DEBUG

			return true;
		}

		/**
		 * Creates a new model candidate by adjusting the current/actual model with delta values.
		 * @see AdvancedSparseOptimizationProvider::applyCorrection().
		 */
		inline void applyCorrection(const Matrix& deltas)
		{
			const Pose oldPose(secondFlippedCamera_T_world_);

			// p_i+1 = p_i + delta_i
			// p_i+1 = p_i - (-delta_i)
			const Pose deltaPose(deltas(3), deltas(4), deltas(5), deltas(0), deltas(1), deltas(2));
			const Pose newPose(oldPose - deltaPose);

			secondCandidateFlippedCamera_T_world_ = newPose.transformation();

			for (size_t n = 0; n < correspondences_; ++n)
			{
				objectPointCandidates_[n] = objectPoints_[n] - Vector3(deltas.data() + 6 + n * 3);
			}
		}

		/**
		 *  Accepts the current model candidate a new (better) model than the previous one.
		 * @see AdvancedSparseOptimizationProvider::acceptCorrection().
		 */
		inline void acceptCorrection()
		{
			secondFlippedCamera_T_world_ = secondCandidateFlippedCamera_T_world_;

			ocean_assert(objectPointCandidates_.size() == correspondences_);
			memcpy(objectPoints_, objectPointCandidates_.data(), sizeof(Vector3) * correspondences_);
		}

		/**
		 * Returns whether the optimization process should stop e.g., due to an external event.
		 * @see AdvancedSparseOptimizationProvider::shouldStop().
		 */
		inline bool shouldStop()
		{
			return false;
		}

		/**
		 * Solves the linear equation Hessian * deltas = -jacobianError based on the internal data.
		 * @see AdvancedSparseOptimizationProvider::solve().
		 */
		inline bool solve(Matrix& deltas, const Scalar lambda)
		{
			ocean_assert(lambda >= 0);

			// first we apply the lambda values to the diagonal of matrix A and D (if a lambda is defined)
			// afterwards, we invert the 3x3 block-diagonal matrix D by simply inverting the 3x3 blocks individually

			if (lambda > 0)
			{
				for (size_t n = 0; n < 6; ++n)
				{
					matrixA_(n, n) = diagonalMatrixA_[n] * (Scalar(1) + lambda);
				}

				for (size_t n = 0; n < correspondences_; ++n)
				{
					ocean_assert(matrixD_[n].isSymmetric());
					matrixInvertedD_[n] = matrixD_[n];

					for (unsigned int i = 0u; i < 3u; ++i)
					{
						matrixInvertedD_[n](i, i) *= Scalar(1) + lambda;
					}

					matrixInvertedD_[n].invert();
				}
			}
			else
			{
				for (size_t n = 0; n < correspondences_; ++n)
				{
					ocean_assert(matrixD_[n].isSymmetric());
					matrixInvertedD_[n] = matrixD_[n];

					matrixInvertedD_[n].invert();
				}
			}

			/**
			 * We solve the equation by applying the Schur complement for the linear equation:
			 * | A B | * |da| = |ra|
			 * | C D | * |db| = |rb|, while C = B^T
			 *
			 * We solve da by:
			 * (A - B D^-1 B^T) da = ra - B D^-1 rb
			 *
			 * Then we solve db by:
			 * db = D^-1 (rb - Cda)
			 */

			// first we calculate: ra - B D^-1 rb
			Scalar resultVector[6] = {jacobianErrorVector_[0], jacobianErrorVector_[1], jacobianErrorVector_[2], jacobianErrorVector_[3], jacobianErrorVector_[4], jacobianErrorVector_[5]};

			for (size_t n = 0; n < correspondences_; ++n)
			{
				const Vector3 intermediate(matrixInvertedD_[n] * Vector3(jacobianErrorVector_.data() + 6 + 3 * n));

				for (size_t i = 0; i < 6; ++i)
				{
					resultVector[i] -= matrixB_[n](i, 0) * intermediate[0] + matrixB_[n](i, 1) * intermediate[1] + matrixB_[n](i, 2) * intermediate[2];
				}
			}

			StaticMatrix6x6 left(matrixA_);
			StaticMatrix3x6 intermediate;

#ifdef OCEAN_DEBUG
			// we zero the lower triangle as we determine the upper triangle only
			for (size_t r = 1; r < 6; ++r)
			{
				for (size_t c = 0; c < r; ++c)
				{
					left(r, c) = 0;
				}
			}
#endif

			// now we calculate A - B D^-1 B^T

			for (size_t n = 0; n < correspondences_; ++n)
			{
				const SquareMatrix3& subMatrixInvertedD = matrixInvertedD_[n];
				const StaticMatrix6x3& subMatrixB = matrixB_[n];

				for (unsigned int r = 0u; r < 3u; ++r)
				{
					for (unsigned int c = 0u; c < 6u; ++c)
					{
						intermediate(r, c) = subMatrixInvertedD(r, 0) * subMatrixB(c, 0) + subMatrixInvertedD(r, 1) * subMatrixB(c, 1) + subMatrixInvertedD(r, 2) * subMatrixB(c, 2); // we interpret subMatrixB as transposed here
					}
				}

				// we determine the upper triangle only (as the result will be symmetric)
				for (unsigned int r = 0u; r < 6u; ++r)
				{
					for (unsigned int c = r; c < 6u; ++c)
					{
						left(r, c) -= subMatrixB(r, 0) * intermediate(0, c) + subMatrixB(r, 1) * intermediate(1, c) + subMatrixB(r, 2) * intermediate(2, c);
					}
				}
			}

			// we copy the upper triangle to the lower triangle
			for (size_t r = 1; r < 6; ++r)
			{
				for (size_t c = 0; c < r; ++c)
				{
					left(r, c) = left(c, r);
				}
			}

			// now we solve da:

			deltas.resize(6 + 3 * correspondences_, 1);
			if (!Matrix(6, 6, left.data()).solve<Matrix::MP_SYMMETRIC>(resultVector, deltas.data()))
			{
				return false;
			}

			// now we solve db:

			for (size_t n = 0; n < correspondences_; ++n)
			{
				const SquareMatrix3& subMatrixInvertedD = matrixInvertedD_[n];
				const StaticMatrix6x3& subMatrixB = matrixB_[n];

				Vector3 intermediateError(0, 0, 0);
				for (size_t i = 0; i < 6; ++i)
				{
					intermediateError[0] += subMatrixB(i, 0) * deltas.data()[i];
					intermediateError[1] += subMatrixB(i, 1) * deltas.data()[i];
					intermediateError[2] += subMatrixB(i, 2) * deltas.data()[i];
				}

				intermediateError = Vector3(jacobianErrorVector_.data() + 6 + 3 * n) - intermediateError;

				((Vector3*)(deltas.data() + 6))[n] = subMatrixInvertedD * intermediateError;
			}

#ifdef OCEAN_INTENSIVE_DEBUG
	#ifndef OCEAN_DEBUG
			#error Invalid debug state!
	#endif

			if (std::is_same<Scalar, double>::value)
			{
				ocean_assert(debugHessian_.rows() == debugHessian_.columns());
				SparseMatrix debugCopyHessian(debugHessian_);

				if (lambda > 0)
				{
					for (size_t n = 0; n < debugCopyHessian.rows(); ++n)
					{
						ocean_assert(!debugCopyHessian.isZero(n, n));
						debugCopyHessian(n, n) *= Scalar(1) + lambda;
					}
				}

				for (size_t r = 0; r < 6; ++r)
				{
					for (size_t c = 0; c < 6; ++c)
					{
						const Scalar value0 = debugCopyHessian(r, c);
						const Scalar value1 = matrixA_(r, c);
						ocean_assert(Numeric::isWeakEqual(value0, value1));
					}
				}

				const size_t sizeA = 6;
				const size_t sizeB = 3 * correspondences_;

				SparseMatrix D(debugCopyHessian.submatrix(sizeA, sizeA, sizeB, sizeB));

				if (!D.invertBlockDiagonal3())
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				const SparseMatrix A(debugCopyHessian.submatrix(0, 0, sizeA, sizeA));
				const SparseMatrix B(debugCopyHessian.submatrix(0, sizeA, sizeA, sizeB));
				const SparseMatrix C(debugCopyHessian.submatrix(sizeA, 0, sizeB, sizeA));

				const Matrix ea(sizeA, 1, debugJacobianError_.data());
				const Matrix eb(sizeB, 1, debugJacobianError_.data() + sizeA);

				const SparseMatrix debugLeft = A - B * (D * C);
				const Matrix debugRight = ea - B * (D * eb);

				for (size_t r = 0; r < 6; ++r)
				{
					for (size_t c = 0; c < 6; ++c)
					{
						const Scalar value0 = debugLeft(r, c);
						const Scalar value1 = left(r, c);
						ocean_assert(Numeric::isWeakEqual(value0, value1));
					}
				}

				for (size_t r = 0; r < 6; ++r)
				{
					const Scalar value0 = debugRight(r, 0);
					const Scalar value1 = resultVector[r];
					ocean_assert(Numeric::isWeakEqual(value0, value1));
				}

				Matrix da;
				if (!debugLeft.solve(debugRight, da))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				for (size_t r = 0; r < 6; ++r)
				{
					ocean_assert(Numeric::isWeakEqual(da(r, 0), deltas(r, 0)));
				}

				const Matrix db = D * (eb - C * da);

				for (size_t r = 0; r < 3 * correspondences_; ++r)
				{
					ocean_assert(Numeric::isWeakEqual(db(r, 0), deltas.data()[6 + r]));
				}
			}
#endif // OCEAN_INTENSIVE_DEBUG

			return true;
		}

	protected:

		/// The camera profile defining the projection.
		const PinholeCamera& camera_;

		/// The first (static) camera pose.
		const HomogenousMatrix4& firstFlippedCamera_T_world_;

		/// The second (dynamic) camera pose of the most recent succeeded optimization step.
		HomogenousMatrix4& secondFlippedCamera_T_world_;

		/// The candidate of the second camera pose, may be rejected if the error is larger than for the previous pose/model.
		HomogenousMatrix4 secondCandidateFlippedCamera_T_world_;

		/// The locations of the 3D object points of the most recent succeeded optimization step.
		Vector3* objectPoints_;

		/// The locations of the candidate object points.
		Vectors3 objectPointCandidates_;

		/// The image points visible in the first camera frame, one for each object point.
		const Vector2* firstImagePoints_;

		/// The image points visible in the second camera frame, one for each object point.
		const Vector2* secondImagePoints_;

		/// The number of point correspondences (e.g., the number of object points).
		const size_t correspondences_;

		/// True, if the distortion parameters of the camera model will be used.
		const bool useDistortionParameters_;

		/// True, if all 3D object points (before and after optimization) must lie in front of both cameras.
		const bool onlyFrontObjectPoints_;

		/// The upper left 6x6 sub-matrix of the Hessian matrix.
		StaticMatrix6x6 matrixA_;

		/// The upper right 6x3 sub-matrices of the Hessian matrix.
		StaticMatrices6x3 matrixB_;

		/// The lower right 3x3 sub-matrices of the Hessian matrix.
		SquareMatrices3 matrixD_;

		/// The inverted 3x3 sub-matrices of the Hessian matrix.
		SquareMatrices3 matrixInvertedD_;

		/// The error vector multiplied by the Jacobian matrix.
		Scalars jacobianErrorVector_;

		/// The copy of the diagonal of matrix A.
		Scalar diagonalMatrixA_[6];

		/// The copy of the diagonal of matrix D.
		Scalars diagonalMatrixD_;

		/// Intermediate square error values.
		Scalars intermediateSqrErrors_;

		/// Intermediate error values.
		Vectors2 intermediateErrors_;

		/// Intermediate weight values.
		Scalars intermediateWeights_;

#ifdef OCEAN_DEBUG
		/// The Jacobian matrix.
		SparseMatrix debugJacobian_;

		/// The Hessian matrix.
		SparseMatrix debugHessian_;

		/// The error vector multiplied by the Jacobian matrix.
		Matrix debugJacobianError_;
#endif
};

bool NonLinearOptimizationObjectPoint::optimizeObjectPointsAndOnePoseIF(const PinholeCamera& camera, const HomogenousMatrix4& firstPoseIF, const HomogenousMatrix4& secondPoseIF, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& firstImagePoints, const ConstIndexedAccessor<Vector2>& secondImagePoints, const bool useDistortionParameters, HomogenousMatrix4* optimizedSecondPoseIF, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator, const Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
	ocean_assert(camera.isValid() && firstPoseIF.isValid() && secondPoseIF.isValid());
	ocean_assert(objectPoints.size() == firstImagePoints.size());
	ocean_assert(objectPoints.size() == secondImagePoints.size());

	ocean_assert(optimizedObjectPoints == nullptr || optimizedObjectPoints->size() == objectPoints.size());

	HomogenousMatrix4 internalOptimizedSecondPoseIF(secondPoseIF);

	ScopedNonconstMemoryAccessor<Vector3> scopedOptimizedObjectPoints(optimizedObjectPoints, objectPoints.size());
	ocean_assert(scopedOptimizedObjectPoints.size() == objectPoints.size());

	const ScopedConstMemoryAccessor<Vector2> scopedFirstImagePoints(firstImagePoints);
	const ScopedConstMemoryAccessor<Vector2> scopedSecondImagePoints(secondImagePoints);

	for (size_t n = 0; n < objectPoints.size(); ++n)
		scopedOptimizedObjectPoints[n] = objectPoints[n];

	switch (estimator)
	{
		case Estimator::ET_LINEAR:
		{
			ObjectPointsOnePoseProvider<Estimator::ET_LINEAR> provider(camera, firstPoseIF, internalOptimizedSecondPoseIF, scopedOptimizedObjectPoints.data(), scopedFirstImagePoints.data(), scopedSecondImagePoints.data(), scopedOptimizedObjectPoints.size(), useDistortionParameters, onlyFrontObjectPoints);
			if (!advancedSparseOptimization(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors))
			{
				return false;
			}

			break;
		}

		case Estimator::ET_HUBER:
		{
			ObjectPointsOnePoseProvider<Estimator::ET_HUBER> provider(camera, firstPoseIF, internalOptimizedSecondPoseIF, scopedOptimizedObjectPoints.data(), scopedFirstImagePoints.data(), scopedSecondImagePoints.data(), scopedOptimizedObjectPoints.size(), useDistortionParameters, onlyFrontObjectPoints);
			if (!advancedSparseOptimization(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors))
			{
				return false;
			}

			break;
		}

		case Estimator::ET_TUKEY:
		{
			ObjectPointsOnePoseProvider<Estimator::ET_TUKEY> provider(camera, firstPoseIF, internalOptimizedSecondPoseIF, scopedOptimizedObjectPoints.data(), scopedFirstImagePoints.data(), scopedSecondImagePoints.data(), scopedOptimizedObjectPoints.size(), useDistortionParameters, onlyFrontObjectPoints);
			if (!advancedSparseOptimization(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors))
			{
				return false;
			}

			break;
		}

		case Estimator::ET_CAUCHY:
		{
			ObjectPointsOnePoseProvider<Estimator::ET_CAUCHY> provider(camera, firstPoseIF, internalOptimizedSecondPoseIF, scopedOptimizedObjectPoints.data(), scopedFirstImagePoints.data(), scopedSecondImagePoints.data(), scopedOptimizedObjectPoints.size(), useDistortionParameters, onlyFrontObjectPoints);
			if (!advancedSparseOptimization(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors))
			{
				return false;
			}

			break;
		}

		default:
		{
			ocean_assert(estimator == Estimator::ET_SQUARE);
			ObjectPointsOnePoseProvider<Estimator::ET_SQUARE> provider(camera, firstPoseIF, internalOptimizedSecondPoseIF, scopedOptimizedObjectPoints.data(), scopedFirstImagePoints.data(), scopedSecondImagePoints.data(), scopedOptimizedObjectPoints.size(), useDistortionParameters, onlyFrontObjectPoints);
			if (!advancedSparseOptimization(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors))
			{
				return false;
			}

			break;
		}
	}

	if (optimizedSecondPoseIF)
		*optimizedSecondPoseIF = internalOptimizedSecondPoseIF;

	return true;
}

/**
 * This class implements an optimization provider object for two camera poses and 3D object points.
 */
class NonLinearOptimizationObjectPoint::ObjectPointsTwoPosesProvider : public NonLinearOptimization::OptimizationProvider
{
	public:

		/**
		 * Creates a new provider object.
		 * @param pinholeCamera The pinhole camera profile defining the projection
		 * @param firstPoseIF The inverted and flipped camera pose of the first frame
		 * @param secondPoseIF The inverted and flipped camera pose of the second frame
		 * @param objectPoints The 3D object point locations which will be optimized
		 * @param firstImagePoints The (static) image points visible in the first camera frame
		 * @param secondImagePoints The (static) image points visible in the second camera frame
		 * @param correspondences The number of points correspondences, with range [5, infinity)
		 * @param useDistortionParameters True, to use the distortion parameters of the camera profile
		 * @param onlyFrontObjectPoints True, to ensure that all 3D object point locations will lie in front of both cameras
		 */
		inline ObjectPointsTwoPosesProvider(const PinholeCamera& camera, HomogenousMatrix4& firstPoseIF, HomogenousMatrix4& secondPoseIF, Vector3* objectPoints, const Vector2* firstImagePoints, const Vector2* secondImagePoints, const size_t correspondences, const bool useDistortionParameters, const bool onlyFrontObjectPoints) :
			camera_(camera),
			firstFlippedCamera_T_world_(firstPoseIF),
			providerFirstPoseCandidateIF(firstPoseIF),
			secondFlippedCamera_T_world_(secondPoseIF),
			secondCandidateFlippedCamera_T_world_(secondPoseIF),
			objectPoints_(objectPoints),
			objectPointCandidates_(correspondences),
			firstImagePoints_(firstImagePoints),
			secondImagePoints_(secondImagePoints),
			correspondences_(correspondences),
			useDistortionParameters_(useDistortionParameters),
			onlyFrontObjectPoints_(onlyFrontObjectPoints)
		{
			ocean_assert(correspondences >= 5);
			memcpy(objectPointCandidates_.data(), objectPoints, sizeof(Vector3) * correspondences);
		}

		/**
		 * Returns whether this provider comes with an own equation solver.
		 * @return True, as this provider has an own solver
		 */
		inline bool hasSolver() const
		{
			return true;
		}

		/**
		 * Solves the equation JTJ * deltas = jErrors
		 * @param JTJ The JTJ matrix
		 * @param jErrors The jErrors vector
		 * @param deltas The deltas vector
		 * @return True, if succeeded
		 */
		inline bool solve(const SparseMatrix& JTJ, const Matrix& jErrors, Matrix& deltas) const
		{
			ocean_assert(JTJ.rows() == JTJ.columns());
			ocean_assert(JTJ.rows() == jErrors.rows());

			ocean_assert(jErrors.columns() == 1);

			/**
			 * here we apply the Schur complement to improve the solve performance:
			 *
			 *  JTJ  * deltas = jErrors
			 * | A B |   | da |   | ea |
			 * | C D | * | db | = | eb |
			 *
			 * (A - B D^-1 C ) * da = ea - B D^-1 * eb  ->  (solve da)
			 * db = D^-1 (eb - C * da)
			 *
			 * or:
			 * (D - C A^-1 B) * db = eb - C A^-1 * ea -> (solve db)
			 * da = A^-1 (ea - B * db)
			 */

			// solving da:

			const size_t sizeA = 12;
			const size_t sizeB = JTJ.rows() - sizeA;
			ocean_assert(sizeB % 3 == 0);

			SparseMatrix D(JTJ.submatrix(sizeA, sizeA, sizeB, sizeB));

			if (!D.invertBlockDiagonal3())
			{
				return false;
			}

			const SparseMatrix A(JTJ.submatrix(0, 0, sizeA, sizeA));
			const SparseMatrix B(JTJ.submatrix(0, sizeA, sizeA, sizeB));
			const SparseMatrix C(JTJ.submatrix(sizeA, 0, sizeB, sizeA));

			const Matrix ea(sizeA, 1, jErrors.data());
			const Matrix eb(sizeB, 1, jErrors.data() + sizeA);

			Matrix da;
			if (!(A - B * (D * C)).solve(ea - B * (D * eb), da))
				return false;

			const Matrix db = D * (eb - C * da);

			deltas.resize(jErrors.rows(), 1);

			memcpy(deltas.data(), da.data(), sizeA * sizeof(Scalar));
			memcpy(deltas.data() + sizeA, db.data(), sizeB * sizeof(Scalar));

#ifdef OCEAN_INTENSIVE_DEBUG
	#ifndef OCEAN_DEBUG
			#error Invalid debug state!
	#endif

			const Matrix debugJErrors(JTJ * deltas);
			Scalars difference(jErrors.rows());

			bool allWeakEps = true;
			Scalar averageDifference = 0;
			for (unsigned int n = 0u; n < jErrors.rows(); ++n)
			{
				difference[n] = debugJErrors(n, 0) - jErrors(n, 0);

				averageDifference += Numeric::abs(difference[n]);
			}

			ocean_assert(jErrors.rows() != 0);
			averageDifference /= Scalar(jErrors.rows());

			ocean_assert(Numeric::isWeakEqualEps(averageDifference));
#endif // OCEAN_INTENSIVE_DEBUG

			return true;
		}

		/**
		 * Determines the Jacobian matrix of this providers.
		 * @param jacobian The resulting Jacobian matrix
		 */
		void determineJacobian(SparseMatrix& jacobian) const
		{
			SparseMatrix::Entries jacobianEntries;
			jacobianEntries.reserve(correspondences_ * 4 * 9); // in each row of the Jacobian are at most 9 non-zero elements

			Scalar pointJacobianBuffer[6];
			Scalar poseJacobianBuffer[12];

			SquareMatrix3 firstRodriguesDerivatives[3], secondRodriguesDerivatives[3];

			const Pose firstPose(firstFlippedCamera_T_world_);
			Jacobian::calculateRotationRodriguesDerivative(ExponentialMap(firstPose.rx(), firstPose.ry(), firstPose.rz()), firstRodriguesDerivatives[0], firstRodriguesDerivatives[1], firstRodriguesDerivatives[2]);

			const Pose secondPose(secondFlippedCamera_T_world_);
			Jacobian::calculateRotationRodriguesDerivative(ExponentialMap(secondPose.rx(), secondPose.ry(), secondPose.rz()), secondRodriguesDerivatives[0], secondRodriguesDerivatives[1], secondRodriguesDerivatives[2]);

			for (size_t n = 0; n < correspondences_; ++n)
			{
				const ObjectPoint& objectPoint = objectPoints_[n];

				Jacobian::calculatePoseJacobianRodrigues2x6(poseJacobianBuffer, poseJacobianBuffer + 6, camera_, firstFlippedCamera_T_world_, objectPoint, useDistortionParameters_, firstRodriguesDerivatives[0], firstRodriguesDerivatives[1], firstRodriguesDerivatives[2]);

				for (size_t e = 0; e < 6; ++e)
				{
					jacobianEntries.push_back(SparseMatrix::Entry(4 * n + 0, e, poseJacobianBuffer[0 + e]));
					jacobianEntries.push_back(SparseMatrix::Entry(4 * n + 1, e, poseJacobianBuffer[6 + e]));
				}

				Jacobian::calculatePointJacobian2x3(pointJacobianBuffer, pointJacobianBuffer + 3, camera_, firstFlippedCamera_T_world_, objectPoint, useDistortionParameters_);

				for (size_t e = 0; e < 3; ++e)
				{
					jacobianEntries.push_back(SparseMatrix::Entry(4 * n + 0, 12 + 3 * n + e, pointJacobianBuffer[0 + e]));
					jacobianEntries.push_back(SparseMatrix::Entry(4 * n + 1, 12 + 3 * n + e, pointJacobianBuffer[3 + e]));
				}

				Jacobian::calculatePoseJacobianRodrigues2x6(poseJacobianBuffer, poseJacobianBuffer + 6, camera_, secondFlippedCamera_T_world_, objectPoint, useDistortionParameters_, secondRodriguesDerivatives[0], secondRodriguesDerivatives[1], secondRodriguesDerivatives[2]);

				for (size_t e = 0; e < 6; ++e)
				{
					jacobianEntries.push_back(SparseMatrix::Entry(4 * n + 2, 6 + e, poseJacobianBuffer[0 + e]));
					jacobianEntries.push_back(SparseMatrix::Entry(4 * n + 3, 6 + e, poseJacobianBuffer[6 + e]));
				}

				Jacobian::calculatePointJacobian2x3(pointJacobianBuffer, pointJacobianBuffer + 3, camera_, secondFlippedCamera_T_world_, objectPoint, useDistortionParameters_);

				for (size_t e = 0; e < 3; ++e)
				{
					jacobianEntries.push_back(SparseMatrix::Entry(4 * n + 2, 12 + 3 * n + e, pointJacobianBuffer[0 + e]));
					jacobianEntries.push_back(SparseMatrix::Entry(4 * n + 3, 12 + 3 * n + e, pointJacobianBuffer[3 + e]));
				}
			}

			jacobian = SparseMatrix(4 * correspondences_, 12 + 3 * correspondences_, jacobianEntries);
			ocean_assert(SparseMatrix::Entry::hasOneEntry(jacobian.rows(), jacobian.columns(), jacobianEntries));
		}

		void applyCorrection(const Matrix& deltas)
		{
			const Pose oldFirstPose(firstFlippedCamera_T_world_);

			// p_i+1 = p_i + delta_i
			// p_i+1 = p_i - (-delta_i)
			const Pose deltaFirstPose(deltas(3), deltas(4), deltas(5), deltas(0), deltas(1), deltas(2));

			const Pose newFirstPose(oldFirstPose - deltaFirstPose);
			providerFirstPoseCandidateIF = newFirstPose.transformation();

			const Pose oldSecondPose(secondFlippedCamera_T_world_);

			// p_i+1 = p_i + delta_i
			// p_i+1 = p_i - (-delta_i)
			const Pose deltaSecondPose(deltas(6 + 3), deltas(6 + 4), deltas(6 + 5), deltas(6 + 0), deltas(6 + 1), deltas(6 + 2));

			const Pose newSecondPose(oldSecondPose - deltaSecondPose);
			secondCandidateFlippedCamera_T_world_ = newSecondPose.transformation();

			for (unsigned int n = 0u; n < correspondences_; ++n)
			{
				const Vector3 deltaObjectPoint(deltas(12 + 3 * n + 0), deltas(12 + 3 * n + 1), deltas(12 + 3 * n + 2));

				const Vector3 newObjectPoint(objectPoints_[n] - deltaObjectPoint);
				objectPointCandidates_[n] = newObjectPoint;
			}
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
			weightedErrorVector.resize(4 * correspondences_, 1);

			Vector2* const weightedErrors = (Vector2*)(weightedErrorVector.data());
			const SquareMatrix2* transposedInvertedCovariances = invertedCovariances ? (SquareMatrix2*)invertedCovariances->data() : nullptr;

			Scalar sqrError = 0;

			Scalars sqrErrors;
			if constexpr (!Estimator::isStandardEstimator<tEstimator>())
			{
				sqrErrors.reserve(correspondences_ * 2);
			}

			for (size_t n = 0; n < correspondences_; ++n)
			{
				const ObjectPoint& objectPoint = objectPointCandidates_[n];

				if (onlyFrontObjectPoints_ && (!PinholeCamera::isObjectPointInFrontIF(providerFirstPoseCandidateIF, objectPoint) || !PinholeCamera::isObjectPointInFrontIF(secondCandidateFlippedCamera_T_world_, objectPoint)))
				{
					return Numeric::maxValue();
				}

				const Vector2 firstError = Error::determinePoseErrorIF(providerFirstPoseCandidateIF, camera_, objectPoint, firstImagePoints_[n], useDistortionParameters_);
				const Vector2 secondError = Error::determinePoseErrorIF(secondCandidateFlippedCamera_T_world_, camera_, objectPoint, secondImagePoints_[n], useDistortionParameters_);

				weightedErrors[2 * n + 0] = firstError;
				weightedErrors[2 * n + 1] = secondError;

				if constexpr (Estimator::isStandardEstimator<tEstimator>())
				{
					if (transposedInvertedCovariances != nullptr)
					{
						sqrError += (transposedInvertedCovariances[2 * n + 0].transposed() * firstError).sqr();
						sqrError += (transposedInvertedCovariances[2 * n + 1].transposed() * secondError).sqr();
					}
					else
					{
						sqrError += firstError.sqr() + secondError.sqr();
					}
				}
				else
				{
					ocean_assert(!Estimator::isStandardEstimator<tEstimator>());

					sqrErrors.emplace_back(firstError.sqr());
					sqrErrors.emplace_back(secondError.sqr());
				}
			}

			// check whether the standard estimator is used
			if constexpr (Estimator::isStandardEstimator<tEstimator>())
			{
				// the weight vector should be and should stay invalid
				ocean_assert(!weightVector);

				return sqrError / Scalar(correspondences_ * 2);
			}
			else
			{
				// now we need the weight vector
				weightVector.resize(correspondences_ * 4, 1u);

				ocean_assert(sqrErrors.size() == correspondences_ * 2);

				// the model size is the number of columns in the jacobian row as so many parameters will be modified
				return sqrErrors2robustErrors2<tEstimator>(sqrErrors, 12 + correspondences_ * 3, weightedErrors, (Vector2*)(weightVector.data()), transposedInvertedCovariances);
			}
		}

		/**
		 * Accepts the current pose candidate as better model.
		 */
		inline void acceptCorrection()
		{
			firstFlippedCamera_T_world_ = providerFirstPoseCandidateIF;
			secondFlippedCamera_T_world_ = secondCandidateFlippedCamera_T_world_;

			memcpy(objectPoints_, objectPointCandidates_.data(), sizeof(Vector3) * correspondences_);
		}

	protected:

		/// The camera profile defining the projection.
		const PinholeCamera& camera_;

		/// The first camera pose.
		HomogenousMatrix4& firstFlippedCamera_T_world_;

		/// The candidate of the first camera pose, may be rejected if the error is larger than for the previous pose/model.
		HomogenousMatrix4 providerFirstPoseCandidateIF;

		/// The second camera pose of the most recent succeeded optimization step.
		HomogenousMatrix4& secondFlippedCamera_T_world_;

		/// The candidate of the second camera pose, may be rejected if the error is larger than for the previous pose/model.
		HomogenousMatrix4 secondCandidateFlippedCamera_T_world_;

		/// The locations of the 3D object points of the most recent succeeded optimization step.
		Vector3* objectPoints_;

		/// The locations of the candidate object points.
		Vectors3 objectPointCandidates_;

		/// The image points visible in the first camera frame, one for each object point.
		const Vector2* firstImagePoints_;

		/// The image points visible in the second camera frame, one for each object point.
		const Vector2* secondImagePoints_;

		/// The number of point correspondences (e.g., the number of object points).
		const size_t correspondences_;

		/// True, if the distortion parameters of the camera model will be used.
		const bool useDistortionParameters_;

		/// True, if all 3D object points (before and after optimization) must lie in front of both cameras.
		const bool onlyFrontObjectPoints_;
};

bool NonLinearOptimizationObjectPoint::optimizeObjectPointsAndTwoPosesIF(const PinholeCamera& camera, const HomogenousMatrix4& firstPoseIF, const HomogenousMatrix4& secondPoseIF, const ConstIndexedAccessor<Vector3>& objectPoints, const ConstIndexedAccessor<Vector2>& firstImagePoints, const ConstIndexedAccessor<Vector2>& secondImagePoints, const bool useDistortionParameters, HomogenousMatrix4* optimizedFirstPoseIF, HomogenousMatrix4* optimizedSecondPoseIF, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialError, Scalar* finalError, const Matrix* invertedFirstCovariances, const Matrix* invertedSecondCovariances, Scalars* intermediateErrors)
{
	ocean_assert(camera.isValid() && firstPoseIF.isValid() && secondPoseIF.isValid());
	ocean_assert(objectPoints.size() == firstImagePoints.size());
	ocean_assert(objectPoints.size() == secondImagePoints.size());

	ocean_assert(optimizedObjectPoints == nullptr || optimizedObjectPoints->size() == objectPoints.size());

	HomogenousMatrix4 internalOptimizedFirstPoseIF(firstPoseIF);
	HomogenousMatrix4 internalOptimizedSecondPoseIF(secondPoseIF);

	ScopedNonconstMemoryAccessor<Vector3> scopedOptimizedObjectPoints(optimizedObjectPoints, objectPoints.size());
	ocean_assert(scopedOptimizedObjectPoints.size() == objectPoints.size());

	const ScopedConstMemoryAccessor<Vector2> scopedFirstImagePoints(firstImagePoints);
	const ScopedConstMemoryAccessor<Vector2> scopedSecondImagePoints(secondImagePoints);

	for (size_t n = 0; n < objectPoints.size(); ++n)
	{
		scopedOptimizedObjectPoints[n] = objectPoints[n];
	}

	Matrix combinedInvertedCovariances;

	if (invertedFirstCovariances && invertedSecondCovariances)
	{
		ocean_assert(invertedFirstCovariances->rows() == objectPoints.size() * 2 && invertedFirstCovariances->columns() == 2);
		ocean_assert(invertedSecondCovariances->rows() == objectPoints.size() * 2 && invertedSecondCovariances->columns() == 2);

		if (invertedFirstCovariances->rows() != objectPoints.size() * 2 && invertedFirstCovariances->columns() == 2 && invertedSecondCovariances->rows() != objectPoints.size() * 2 && invertedSecondCovariances->columns() == 2)
		{
			return false;
		}

		combinedInvertedCovariances.resize(objectPoints.size() * 4, 2);

		// interleave the covariances
		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			*((SquareMatrix2*)combinedInvertedCovariances.data() + 2 * n + 0) = *((SquareMatrix2*)invertedFirstCovariances->data() + n);
			*((SquareMatrix2*)combinedInvertedCovariances.data() + 2 * n + 1) = *((SquareMatrix2*)invertedSecondCovariances->data() + n);
		}
	}

	ObjectPointsTwoPosesProvider provider(camera, internalOptimizedFirstPoseIF, internalOptimizedSecondPoseIF, scopedOptimizedObjectPoints.data(), scopedFirstImagePoints.data(), scopedSecondImagePoints.data(), scopedOptimizedObjectPoints.size(), useDistortionParameters, onlyFrontObjectPoints);
	if (!sparseOptimization(provider, iterations, estimator, lambda, lambdaFactor, initialError, finalError, combinedInvertedCovariances ? &combinedInvertedCovariances : nullptr, intermediateErrors))
	{
		return false;
	}

	if (optimizedFirstPoseIF)
	{
		*optimizedFirstPoseIF = internalOptimizedFirstPoseIF;
	}

	if (optimizedSecondPoseIF)
	{
		*optimizedSecondPoseIF = internalOptimizedSecondPoseIF;
	}

	return true;
}

/**
 * This class implements an optimization provider allowing to optimize several 6-DOF camera poses and 3-DOF object points concurrently.
 * This provider is highly optimized not using sparse matrices but implements the individual sub-matrices of the sparse linear equation by small dense sub-matrices.
 * The implementation uses the fact the Hessian matrix is symmetric, and further applies the Schur complement during solving.
 * We divide the sparse Hessian matrix into four sub-matrices:
 *     | A B |   |  A  B |
 * H = | C D | = | B^T D |, further A and D are symmetric
 *
 * @tparam tEstimator The type of the estimator to be used
 */
template <Estimator::EstimatorType tEstimator>
class NonLinearOptimizationObjectPoint::ObjectPointsPosesProvider : public NonLinearOptimization::AdvancedSparseOptimizationProvider
{
	public:

		/**
		 * Creates a new provider object.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param flippedCameras_T_world The inverted and flipped camera poses which will be optimized, with default camera pointing towards the positive z-space with y-axis downwards, at least two
		 * @param objectPointAccessor The 3D object point locations which will be optimized
		 * @param correspondenceGroups The accessor for the individual groups of correspondences between pose indices and image point location, one group for each object point
		 * @param onlyFrontObjectPoints True, to ensure that all 3D object point locations will lie in front of both cameras
		 */
		inline ObjectPointsPosesProvider(const ConstIndexedAccessor<const AnyCamera*>& cameras, NonconstTemplateArrayAccessor<HomogenousMatrix4>& flippedCameras_T_world, NonconstTemplateArrayAccessor<Vector3>& objectPointAccessor, const ObjectPointGroupsAccessor& correspondenceGroups, const bool onlyFrontObjectPoints) :
			cameras_(cameras),
			flippedCameras_T_world_(flippedCameras_T_world),
			candidateFlippedCameras_T_world_(Accessor::accessor2elements(flippedCameras_T_world)),
			objectPoints_(objectPointAccessor),
			objectPointCandidates_(Accessor::accessor2elements(objectPointAccessor)),
			correspondenceGroups_(correspondenceGroups),
			onlyFrontObjectPoints_(onlyFrontObjectPoints),
			matrixA_(flippedCameras_T_world.size()),
			matrixB_(correspondenceGroups.groups() * flippedCameras_T_world.size()),
			nonZeroMatrixB_(correspondenceGroups.groups() * flippedCameras_T_world.size()),
			matrixD_(correspondenceGroups.groups()),
			matrixInvertedD_(correspondenceGroups.groups()),
			jacobianErrorVector_(flippedCameras_T_world.size() * 6 + correspondenceGroups.groups() * 3),
			diagonalMatrixA_(flippedCameras_T_world.size() * 6),
			diagonalMatrixD_(correspondenceGroups.groups() * 3),
			jacobianRows_(0)
		{
			ocean_assert(correspondenceGroups.groups() >= 5);

			// each observation (combination of pose and object point) creates two rows
			for (size_t n = 0; n < correspondenceGroups_.groups(); ++n)
			{
				jacobianRows_ += correspondenceGroups_.groupElements(n) * 2;
			}
		}

		/**
		 * Determines the error for the current model candidate (not the actual/actual model).
		 * @see AdvancedSparseOptimizationProvider::determineError().
		 */
		inline Scalar determineError()
		{
			if constexpr (!Estimator::isStandardEstimator<tEstimator>())
			{
				intermediateSqrErrors_.resize(jacobianRows_ / 2);
			}

			Index32 poseId;
			Vector2 imagePoint;

			Scalar sqrError = 0;
			size_t row = 0;

			for (size_t o = 0; o < correspondenceGroups_.groups(); ++o)
			{
				const ObjectPoint& objectPoint = objectPointCandidates_[o];

				for (size_t p = 0; p < correspondenceGroups_.groupElements(o); ++p)
				{
					correspondenceGroups_.element(o, p, poseId, imagePoint);

					const HomogenousMatrix4& candidateFlippedCamera_T_world = candidateFlippedCameras_T_world_[poseId];
					const AnyCamera& camera = *cameras_[poseId];

					if (onlyFrontObjectPoints_ && !AnyCamera::isObjectPointInFrontIF(candidateFlippedCamera_T_world, objectPoint))
					{
						return Numeric::maxValue();
					}

					const Scalar localSqrError = Error::determinePoseErrorIF(candidateFlippedCamera_T_world, camera, objectPoint, imagePoint).sqr();

					if constexpr (Estimator::isStandardEstimator<tEstimator>())
					{
						sqrError += localSqrError;
					}
					else
					{
						intermediateSqrErrors_[row++] = localSqrError;
					}
				}
			}

			if constexpr (Estimator::isStandardEstimator<tEstimator>())
			{
				ocean_assert((jacobianRows_ / 2) != 0);
				return sqrError / Scalar(jacobianRows_ / 2);
			}
			else
			{
				ocean_assert(!intermediateSqrErrors_.empty() && row == intermediateSqrErrors_.size());
				return Estimator::determineRobustError<tEstimator>(intermediateSqrErrors_.data(), intermediateSqrErrors_.size(), candidateFlippedCameras_T_world_.size() * 6 + correspondenceGroups_.groups() * 3);
			}
		}

		/**
		 * Determines any kind of (abstract) parameters based on the current/actual model (not the model candidate) e.g., the Jacobian parameters and/or a Hessian matrix.
		 * @see AdvancedSparseOptimizationProvider::determineParameters().
		 */
		inline bool determineParameters()
		{
			memset(jacobianErrorVector_.data(), 0x00, sizeof(Scalar) * jacobianErrorVector_.size());

			intermediateErrors_.resize(jacobianRows_ / 2);

			if constexpr (!Estimator::isStandardEstimator<tEstimator>())
			{
				intermediateSqrErrors_.resize(jacobianRows_ / 2);
				intermediateWeights_.resize(jacobianRows_ / 2);
			}

			Index32 poseId;
			Vector2 imagePoint;
			size_t row = 0;

			for (size_t o = 0; o < correspondenceGroups_.groups(); ++o)
			{
				const ObjectPoint& objectPoint = objectPointCandidates_[o];

				for (size_t p = 0; p < correspondenceGroups_.groupElements(o); ++p)
				{
					correspondenceGroups_.element(o, p, poseId, imagePoint);

					const HomogenousMatrix4& candidateFlippedCamera_T_world = candidateFlippedCameras_T_world_[poseId];
					const AnyCamera& camera = *cameras_[poseId];

					intermediateErrors_[row] = Error::determinePoseErrorIF(candidateFlippedCamera_T_world, camera, objectPoint, imagePoint);

					if constexpr (!Estimator::isStandardEstimator<tEstimator>())
					{
						intermediateSqrErrors_[row] = intermediateErrors_[row].sqr();
					}

					row++;
				}
			}

			ocean_assert(row == jacobianRows_ / 2);

			if constexpr (!Estimator::isStandardEstimator<tEstimator>())
			{
				const Scalar sqrSigma = Estimator::needSigma<tEstimator>() ? Numeric::sqr(Estimator::determineSigmaSquare<tEstimator>(intermediateSqrErrors_.data(), intermediateSqrErrors_.size(), flippedCameras_T_world_.size() * 6 + correspondenceGroups_.groups() * 3)) : 0;

				for (size_t n = 0; n < intermediateWeights_.size(); ++n)
				{
					// we determine the weights, however as e.g., the tukey estimator may return a weight of 0 we have to clamp the weight to ensure that we still can solve the equation
					// **NOTE** the much better way would be to remove the entry from the equation and to solve it
					intermediateWeights_[n] = max(Numeric::weakEps(), Estimator::robustWeightSquare<tEstimator>(intermediateSqrErrors_[n], sqrSigma));
				}
			}

			SquareMatrices3 rotationRodriguesDerivatives(flippedCameras_T_world_.size() * 3);
			for (size_t n = 0; n < flippedCameras_T_world_.size(); ++n)
			{
				const Pose pose(flippedCameras_T_world_[n]);
				Jacobian::calculateRotationRodriguesDerivative(ExponentialMap(pose.rx(), pose.ry(), pose.rz()), rotationRodriguesDerivatives[n * 3 + 0], rotationRodriguesDerivatives[n * 3 + 1], rotationRodriguesDerivatives[n * 3 + 2]);

				matrixA_[n].toNull();
			}

#ifdef OCEAN_DEBUG
			memset(nonZeroMatrixB_.data(), 0x02u, sizeof(uint8_t) * nonZeroMatrixB_.size());
#endif

			Scalar poseJacobianX[6], poseJacobianY[6];
			Scalar pointJacobianX[3], pointJacobianY[3];

			row = 0;

			for (size_t o = 0; o < correspondenceGroups_.groups(); ++o)
			{
				const ObjectPoint& objectPoint = objectPointCandidates_[o];

				SquareMatrix3& subMatrixD = matrixD_[o];
				subMatrixD.toNull();

				// if not all poses 'see' the object point we have to 'zero' the corresponding matrix b
				if (correspondenceGroups_.groupElements(o) != candidateFlippedCameras_T_world_.size())
				{
					for (size_t p = 0; p < candidateFlippedCameras_T_world_.size(); ++p)
					{
						nonZeroMatrixB_[p * correspondenceGroups_.groups() + o] = 0u;

#ifdef OCEAN_DEBUG
						StaticMatrix6x3& subMatrixB = matrixB_[p * correspondenceGroups_.groups() + o];
						for (size_t i = 0; i < subMatrixB.elements(); ++i)
						{
							subMatrixB[i] = Numeric::minValue();
						}
#endif
					}
				}

				for (size_t p = 0; p < correspondenceGroups_.groupElements(o); ++p)
				{
					correspondenceGroups_.element(o, p, poseId, imagePoint);

					const HomogenousMatrix4& candidateFlippedCamera_T_world = candidateFlippedCameras_T_world_[poseId];
					const AnyCamera& camera = *cameras_[poseId];

					Jacobian::calculatePoseJacobianRodrigues2x6IF(camera, candidateFlippedCamera_T_world, objectPoint, rotationRodriguesDerivatives[poseId * 3 + 0], rotationRodriguesDerivatives[poseId * 3 + 1], rotationRodriguesDerivatives[poseId * 3 + 2], poseJacobianX, poseJacobianY);
					Jacobian::calculatePointJacobian2x3IF(camera, candidateFlippedCamera_T_world, objectPoint, pointJacobianX, pointJacobianY);

					StaticMatrix6x6& subMatrixA = matrixA_[poseId];
					StaticMatrix6x3& subMatrixB = matrixB_[poseId * correspondenceGroups_.groups() + o];
					nonZeroMatrixB_[poseId * correspondenceGroups_.groups() + o] = 1u;

					if constexpr (Estimator::isStandardEstimator<tEstimator>())
					{
						// we calculate the upper triangle of the matrix A (as A is symmetric)
						for (size_t r = 0; r < 6; ++r)
						{
							for (size_t c = r; c < 6; ++c)
							{
								subMatrixA(r, c) += poseJacobianX[r] * poseJacobianX[c] + poseJacobianY[r] * poseJacobianY[c];
							}
						}

						// we calculate the upper triangle(s) of the matrix D (as D is symmetric)
						for (unsigned int r = 0u; r < 3u; ++r)
						{
							for (unsigned int c = r; c < 3u; ++c)
							{
								subMatrixD(r, c) += pointJacobianX[r] * pointJacobianX[c] + pointJacobianY[r] * pointJacobianY[c];
							}
						}

						// we calculate the matrix B
						for (size_t r = 0; r < 6; ++r)
						{
							for (size_t c = 0; c < 3; ++c)
							{
								subMatrixB(r, c) = poseJacobianX[r] * pointJacobianX[c] + poseJacobianY[r] * pointJacobianY[c];
							}
						}
					}
					else
					{
						const Scalar& robustWeight = intermediateWeights_[row];

						// we calculate the upper triangle of the matrix A (as A is symmetric)
						for (size_t r = 0; r < 6; ++r)
						{
							for (size_t c = r; c < 6; ++c)
							{
								subMatrixA(r, c) += (poseJacobianX[r] * poseJacobianX[c] + poseJacobianY[r] * poseJacobianY[c]) * robustWeight;
							}
						}

						// we calculate the upper triangle(s) of the matrix D (as D is symmetric)
						for (unsigned int r = 0u; r < 3u; ++r)
						{
							for (unsigned int c = r; c < 3u; ++c)
							{
								subMatrixD(r, c) += (pointJacobianX[r] * pointJacobianX[c] + pointJacobianY[r] * pointJacobianY[c]) * robustWeight;
							}
						}

						// we calculate the matrix B
						for (size_t r = 0; r < 6; ++r)
						{
							for (size_t c = 0; c < 3; ++c)
							{
								subMatrixB(r, c) = (poseJacobianX[r] * pointJacobianX[c] + poseJacobianY[r] * pointJacobianY[c]) * robustWeight;
							}
						}
					}

					ocean_assert(intermediateErrors_[row] == Error::determinePoseErrorIF(candidateFlippedCamera_T_world, camera, objectPoint, imagePoint));

					if constexpr (Estimator::isStandardEstimator<tEstimator>())
					{
						const Vector2& error = intermediateErrors_[row];

						for (size_t i = 0; i < 6; ++i)
						{
							jacobianErrorVector_[poseId * 6 + i] += poseJacobianX[i] * error[0] + poseJacobianY[i] * error[1];
						}

						for (size_t i = 0; i < 3; ++i)
						{
							jacobianErrorVector_[6 * flippedCameras_T_world_.size() + 3 * o + i] += pointJacobianX[i] * error[0] + pointJacobianY[i] * error[1];
						}
					}
					else
					{
						const Vector2 error(intermediateErrors_[row] * intermediateWeights_[row]);

						for (size_t i = 0; i < 6; ++i)
						{
							jacobianErrorVector_[poseId * 6 + i] += poseJacobianX[i] * error[0] + poseJacobianY[i] * error[1];
						}

						for (size_t i = 0; i < 3; ++i)
						{
							jacobianErrorVector_[6 * flippedCameras_T_world_.size() + 3 * o + i] += pointJacobianX[i] * error[0] + pointJacobianY[i] * error[1];
						}
					}

					row++;
				}

				// we copy the lower triangle of the sub-matrix D
				subMatrixD(1, 0) = subMatrixD(0, 1);
				subMatrixD(2, 0) = subMatrixD(0, 2);
				subMatrixD(2, 1) = subMatrixD(1, 2);

				ocean_assert(!subMatrixD.isNull());
			}

#ifdef OCEAN_DEBUG
			for (size_t n = 0; n < nonZeroMatrixB_.size(); ++n)
			{
				ocean_assert(nonZeroMatrixB_[n] != 0x02u);
			}
#endif

			for (StaticMatrices6x6::iterator i = matrixA_.begin(); i != matrixA_.end(); ++i)
			{
				// we copy the lower triangle from the upper triangle
				for (size_t r = 1; r < 6; ++r)
				{
					for (size_t c = 0; c < r; ++c)
					{
						(*i)(r, c) = (*i)(c, r);
					}
				}
			}

			// we make a copy of the diagonal elements of matrix A so that we can apply a lambda later during the solve step
			for (size_t n = 0; n < flippedCameras_T_world_.size(); ++n)
			{
				const StaticMatrix6x6& subMatrixA = matrixA_[n];

				for (size_t i = 0; i < 6; ++i)
				{
					diagonalMatrixA_[6 * n + i] = subMatrixA(i, i);
				}
			}

#ifdef OCEAN_INTENSIVE_DEBUG
	#ifndef OCEAN_DEBUG
			#error Invalid debug state!
	#endif

			if (std::is_same<Scalar, double>::value)
			{
				SquareMatrices3 rotationRodriguesDerivatives(flippedCameras_T_world_.size() * 3);
				for (size_t n = 0; n < flippedCameras_T_world_.size(); ++n)
				{
					const Pose pose(flippedCameras_T_world_[n]);
					Jacobian::calculateRotationRodriguesDerivative(ExponentialMap(pose.rx(), pose.ry(), pose.rz()), rotationRodriguesDerivatives[n * 3 + 0], rotationRodriguesDerivatives[n * 3 + 1], rotationRodriguesDerivatives[n * 3 + 2]);
				}

				Scalar pointJacobianBuffer[6];
				Scalar poseJacobianBuffer[12];

				SparseMatrix::Entries jacobianEntries;

				size_t row = 0;

				for (size_t o = 0; o < correspondenceGroups_.groups(); ++o)
				{
					const ObjectPoint& objectPoint = objectPointCandidates_[o];

					for (size_t p = 0; p < correspondenceGroups_.groupElements(o); ++p)
					{
						correspondenceGroups_.element(o, p, poseId, imagePoint);

						const HomogenousMatrix4& poseIF = candidateFlippedCameras_T_world_[poseId];
						const AnyCamera& camera = *cameras_[poseId];

						Jacobian::calculatePoseJacobianRodrigues2x6IF(camera, poseIF, objectPoint, rotationRodriguesDerivatives[poseId * 3 + 0], rotationRodriguesDerivatives[poseId * 3 + 1], rotationRodriguesDerivatives[poseId * 3 + 2], poseJacobianBuffer, poseJacobianBuffer + 6);

						for (size_t e = 0; e < 6; ++e)
						{
							jacobianEntries.push_back(SparseMatrix::Entry(row + 0, poseId * 6 + e, poseJacobianBuffer[0 + e]));
							jacobianEntries.push_back(SparseMatrix::Entry(row + 1, poseId * 6 + e, poseJacobianBuffer[6 + e]));
						}

						Jacobian::calculatePointJacobian2x3IF(camera, poseIF, objectPoint, pointJacobianBuffer, pointJacobianBuffer + 3);

						for (size_t e = 0; e < 3; ++e)
						{
							jacobianEntries.push_back(SparseMatrix::Entry(row + 0, candidateFlippedCameras_T_world_.size() * 6 + 3 * o + e, pointJacobianBuffer[0 + e]));
							jacobianEntries.push_back(SparseMatrix::Entry(row + 1, candidateFlippedCameras_T_world_.size() * 6 + 3 * o + e, pointJacobianBuffer[3 + e]));
						}

						row += 2;
					}
				}

				ocean_assert(row == jacobianRows_);
				debugJacobian_ = SparseMatrix(jacobianRows_, candidateFlippedCameras_T_world_.size() * 6 + correspondenceGroups_.groups() * 3, jacobianEntries);

				const Scalar debugSqrSigma = Estimator::needSigma<tEstimator>() ? Numeric::sqr(Estimator::determineSigmaSquare<tEstimator>(intermediateSqrErrors_.data(), intermediateSqrErrors_.size(), candidateFlippedCameras_T_world_.size() * 6 + correspondenceGroups_.groups() * 3)) : 0;

				SparseMatrix::Entries weightEntries;
				for (size_t n = 0; n < jacobianRows_; ++n)
				{
					if constexpr (Estimator::isStandardEstimator<tEstimator>())
					{
						weightEntries.emplace_back(n, n, Scalar(1));
					}
					else
					{
						const Scalar weight = max(Numeric::weakEps(), Estimator::robustWeightSquare<tEstimator>(intermediateSqrErrors_[n / 2], debugSqrSigma));
						weightEntries.emplace_back(n, n, weight);
					}
				}

				SparseMatrix debugWeight(jacobianRows_, jacobianRows_, weightEntries);

				debugHessian_ = debugJacobian_.transposed() * debugWeight * debugJacobian_;

				const SparseMatrix subMatrixA(debugHessian_.submatrix(0, 0, candidateFlippedCameras_T_world_.size() * 6, candidateFlippedCameras_T_world_.size() * 6));
				const SparseMatrix subMatrixB(debugHessian_.submatrix(0, candidateFlippedCameras_T_world_.size() * 6, candidateFlippedCameras_T_world_.size() * 6, 3 * correspondenceGroups_.groups()));
				const SparseMatrix subMatrixC(debugHessian_.submatrix(candidateFlippedCameras_T_world_.size() * 6, 0, 3 * correspondenceGroups_.groups(), candidateFlippedCameras_T_world_.size() * 6));
				const SparseMatrix subMatrixD(debugHessian_.submatrix(candidateFlippedCameras_T_world_.size() * 6, candidateFlippedCameras_T_world_.size() * 6, 3 * correspondenceGroups_.groups(), 3 * correspondenceGroups_.groups()));

				for (size_t n = 0; n < candidateFlippedCameras_T_world_.size(); ++n)
				{
					for (size_t r = 0; r < 6; ++r)
					{
						for (size_t c = 0; c < 6; ++c)
						{
							const Scalar value0 = matrixA_[n](r, c);
							const Scalar value1 = subMatrixA(n * 6 + r, n * 6 + c);
							ocean_assert(Numeric::isEqual<-6>(value0, value1, Numeric::weakEps()));
						}
					}
				}

				for (size_t n = 0; n < candidateFlippedCameras_T_world_.size(); ++n)
				{
					for (size_t i = 0; i < correspondenceGroups_.groups(); ++i)
					{
						for (unsigned int r = 0u; r < 6u; ++r)
						{
							for (unsigned int c = 0u; c < 3u; ++c)
							{
								if (nonZeroMatrixB_[n * correspondenceGroups_.groups() + i])
								{
									const Scalar value0 = matrixB_[n * correspondenceGroups_.groups() + i](r, c);
									const Scalar value1 = subMatrixB(n * 6 + r, i * 3 + c);
									ocean_assert(Numeric::isEqual<-6>(value0, value1, Numeric::weakEps()));

									const Scalar value2 = subMatrixC(i * 3 + c, n * 6 + r);
									ocean_assert(Numeric::isEqual<-6>(value0, value2, Numeric::weakEps()));
								}
								else
								{
									const Scalar value0 = matrixB_[n * correspondenceGroups_.groups() + i](r, c);
									ocean_assert(value0 == Numeric::minValue());

									const Scalar value1 = subMatrixB(n * 6 + r, i * 3 + c);
									ocean_assert(value1 == 0);

									const Scalar value2 = subMatrixC(i * 3 + c, n * 6 + r);
									ocean_assert(value2 == 0);
								}
							}
						}
					}
				}

				for (size_t n = 0; n < correspondenceGroups_.groups(); ++n)
				{
					for (unsigned int r = 0u; r < 3u; ++r)
					{
						for (unsigned int c = 0u; c < 3u; ++c)
						{
							const Scalar value0 = matrixD_[n](r, c);
							const Scalar value1 = subMatrixD(n * 3 + r, n * 3 + c);
							ocean_assert(Numeric::isEqual<-6>(value0, value1, Numeric::weakEps()));
						}
					}
				}

				debugJacobianError_.resize(jacobianRows_, 1);

				row = 0;

				for (size_t o = 0; o < correspondenceGroups_.groups(); ++o)
				{
					const ObjectPoint& objectPoint = objectPointCandidates_[o];

					for (size_t p = 0; p < correspondenceGroups_.groupElements(o); ++p)
					{
						correspondenceGroups_.element(o, p, poseId, imagePoint);

						const HomogenousMatrix4& candidateFlippedCamera_T_world = candidateFlippedCameras_T_world_[poseId];
						const AnyCamera& camera = *cameras_[poseId];

						const Vector2 error = Error::determinePoseErrorIF(candidateFlippedCamera_T_world, camera, objectPoint, imagePoint);

						debugJacobianError_(row++, 0) = error[0];
						debugJacobianError_(row++, 0) = error[1];
					}
				}

				ocean_assert(row == jacobianRows_);

				debugJacobianError_ = debugJacobian_.transposed() * debugWeight * debugJacobianError_;
				ocean_assert(debugJacobianError_.rows() == jacobianErrorVector_.size());
				ocean_assert(debugJacobianError_.columns() == 1);

				for (size_t n = 0; n < jacobianErrorVector_.size(); ++n)
				{
					const Scalar value0 = jacobianErrorVector_[n];
					const Scalar value1 = debugJacobianError_(n, 0);

					ocean_assert(Numeric::isEqual<-6>(value0, value1, Numeric::weakEps()));
				}
			}
#endif // OCEAN_INTENSIVE_DEBUG

			return true;
		}

		/**
		 * Creates a new model candidate by adjusting the current/actual model with delta values.
		 * @see AdvancedSparseOptimizationProvider::applyCorrection().
		 */
		inline void applyCorrection(const Matrix& deltas)
		{
			for (size_t n = 0; n < flippedCameras_T_world_.size(); ++n)
			{
				const Pose oldPose(flippedCameras_T_world_[n]);

				// p_i+1 = p_i + delta_i
				// p_i+1 = p_i - (-delta_i)
				const Pose deltaPose(deltas(n * 6 + 3), deltas(n * 6 + 4), deltas(n * 6 + 5), deltas(n * 6 + 0), deltas(n * 6 + 1), deltas(n * 6 + 2));
				const Pose newPose(oldPose - deltaPose);

				candidateFlippedCameras_T_world_[n] = newPose.transformation();
			}

			for (size_t n = 0; n < correspondenceGroups_.groups(); ++n)
			{
				objectPointCandidates_[n] = objectPoints_[n] - Vector3(deltas.data() + 6 * flippedCameras_T_world_.size() + n * 3);
			}
		}

		/**
		 *  Accepts the current model candidate a new (better) model than the previous one.
		 * @see AdvancedSparseOptimizationProvider::acceptCorrection().
		 */
		inline void acceptCorrection()
		{
			ocean_assert(candidateFlippedCameras_T_world_.size() == flippedCameras_T_world_.size());
			memcpy(flippedCameras_T_world_.data(), candidateFlippedCameras_T_world_.data(), sizeof(HomogenousMatrix4) * flippedCameras_T_world_.size());

			ocean_assert(objectPointCandidates_.size() == correspondenceGroups_.groups());
			memcpy(objectPoints_.data(), objectPointCandidates_.data(), sizeof(Vector3) * correspondenceGroups_.groups());
		}

		/**
		 * Returns whether the optimization process should stop e.g., due to an external event.
		 * @see AdvancedSparseOptimizationProvider::shouldStop().
		 */
		inline bool shouldStop()
		{
			return false;
		}

		/**
		 * Solves the linear equation Hessian * deltas = -jacobianError based on the internal data.
		 * @see AdvancedSparseOptimizationProvider::solve().
		 */
		inline bool solve(Matrix& deltas, const Scalar lambda)
		{
			ocean_assert(lambda >= 0);

			// first we apply the lambda values to the diagonal of matrix A and D (if a lambda is defined)
			// afterwards, we invert the 3x3 block-diagonal matrix D by simply inverting the 3x3 blocks individually

			if (lambda > 0)
			{
				for (size_t n = 0; n < matrixA_.size(); ++n)
				{
					for (size_t i = 0; i < 6; ++i)
					{
						matrixA_[n](i, i) = diagonalMatrixA_[n * 6 + i] * (Scalar(1) + lambda);
					}
				}

				for (size_t n = 0; n < matrixD_.size(); ++n)
				{
					ocean_assert(matrixD_[n].isSymmetric());
					matrixInvertedD_[n] = matrixD_[n];

					for (unsigned int i = 0u; i < 3u; ++i)
					{
						matrixInvertedD_[n](i, i) *= Scalar(1) + lambda;
					}

					matrixInvertedD_[n].invert();
				}
			}
			else
			{
				for (size_t n = 0; n < matrixD_.size(); ++n)
				{
					ocean_assert(matrixD_[n].isSymmetric());
					matrixInvertedD_[n] = matrixD_[n];

					matrixInvertedD_[n].invert();
				}
			}

			/**
			 * We solve the equation by applying the Schur complement for the linear equation:
			 * | A B | * |da| = |ra|
			 * | C D | * |db| = |rb|, while C = B^T
			 *
			 * We solve da by:
			 * (A - B D^-1 B^T) da = ra - B D^-1 rb
			 *
			 * Then we solve db by:
			 * db = D^-1 (rb - C da)
			 */

			// first we calculate: ra - B D^-1 rb
			Scalars resultVector(matrixA_.size() * 6, Scalar(0));

			for (size_t n = 0; n < matrixInvertedD_.size(); ++n)
			{
				const Vector3 intermediate(matrixInvertedD_[n] * Vector3(jacobianErrorVector_.data() + 6 * matrixA_.size() + 3 * n));

				for (size_t i = 0; i < matrixA_.size(); ++i)
				{
					if (nonZeroMatrixB_[i * matrixInvertedD_.size() + n])
					{
						const StaticMatrix6x3& subMatrixB = matrixB_[i * matrixInvertedD_.size() + n];

						for (size_t t = 0; t < 6; ++t)
						{
							resultVector[i * 6 + t] += subMatrixB(t, 0) * intermediate[0] + subMatrixB(t, 1) * intermediate[1] + subMatrixB(t, 2) * intermediate[2];
						}
					}
				}
			}

			for (size_t n = 0; n < matrixA_.size() * 6; ++n)
			{
				resultVector[n] = jacobianErrorVector_[n] - resultVector[n];
			}

			// now we calculate A - B D^-1 B^T
			// as the result is a symmetric matrix we will calculate the lower left triangle only

			Matrix left(matrixA_.size() * 6, matrixA_.size() * 6, false);

			for (size_t i = 0; i < matrixA_.size(); ++i)
			{
				const StaticMatrix6x6& subMatrixA = matrixA_[i];

				for (unsigned int c = 0u; c < 6u; ++c)
				{
					for (unsigned int r = c; r < 6u; ++r)
					{
						left(i * 6 + r, i * 6 + c) = subMatrixA(r, c); // lower left triangle
					}
				}
			}

			StaticMatrix3x6 intermediate;

			for (size_t n = 0; n < matrixInvertedD_.size(); ++n)
			{
				const SquareMatrix3& subMatrixInvertedD = matrixInvertedD_[n];

				for (size_t i = 0; i < matrixA_.size(); ++i)
				{
					if (nonZeroMatrixB_[i * matrixInvertedD_.size() + n])
					{
						const StaticMatrix6x3& subMatrixB = matrixB_[i * matrixInvertedD_.size() + n];

						for (unsigned int r = 0u; r < 3u; ++r)
						{
							for (unsigned int c = 0u; c < 6u; ++c)
							{
								intermediate(r, c) = subMatrixInvertedD(r, 0) * subMatrixB(c, 0) + subMatrixInvertedD(r, 1) * subMatrixB(c, 1) + subMatrixInvertedD(r, 2) * subMatrixB(c, 2); // we interpret subMatrixB as transposed here
							}
						}

						for (size_t t = i; t < matrixA_.size(); ++t) // the lower left triangle only: therefore we start with t = i
						{
							if (nonZeroMatrixB_[t * matrixInvertedD_.size() + n])
							{
								const StaticMatrix6x3& subMatrixB2 = matrixB_[t * matrixInvertedD_.size() + n];

								for (unsigned int r = 0u; r < 6u; ++r)
								{
									for (unsigned int c = 0u; c < 6u; ++c)
									{
										left(t * 6 + r, i * 6 + c) -= subMatrixB2(r, 0) * intermediate(0, c) + subMatrixB2(r, 1) * intermediate(1, c) + subMatrixB2(r, 2) * intermediate(2, c);
									}
								}
							}
						}
					}
				}
			}

			// we copy the lower triangle from the upper triangle
			for (size_t c = 1; c < left.columns(); ++c)
			{
				for (size_t r = 0; r < c; ++r)
				{
					left(r, c) = left(c, r);
				}
			}

			ocean_assert(left.isSymmetric(Numeric::weakEps()));

			// now we solve da:

			deltas.resize(matrixA_.size() * 6 + matrixInvertedD_.size() * 3, 1);
			if (!left.solve<Matrix::MP_SYMMETRIC>(resultVector.data(), deltas.data()))
			{
				return false;
			}

			// now we solve db:

			for (size_t n = 0; n < matrixInvertedD_.size(); ++n)
			{
				Vector3 intermediateError(0, 0, 0);

				for (size_t i = 0; i < matrixA_.size(); ++i)
				{
					if (nonZeroMatrixB_[i * matrixInvertedD_.size() + n])
					{
						const StaticMatrix6x3& subMatrixB = matrixB_[i * matrixInvertedD_.size() + n];

						for (size_t t = 0; t < 6; ++t)
						{
							intermediateError[0] += subMatrixB(t, 0) * deltas.data()[i * 6 + t];
							intermediateError[1] += subMatrixB(t, 1) * deltas.data()[i * 6 + t];
							intermediateError[2] += subMatrixB(t, 2) * deltas.data()[i * 6 + t];
						}
					}
				}

				const SquareMatrix3& subMatrixInvertedD = matrixInvertedD_[n];

				intermediateError = Vector3(jacobianErrorVector_.data() + 6 * matrixA_.size() + 3 * n) - intermediateError;

				((Vector3*)(deltas.data() + 6 * matrixA_.size()))[n] = subMatrixInvertedD * intermediateError;
			}

#ifdef OCEAN_INTENSIVE_DEBUG
	#ifndef OCEAN_DEBUG
			#error Invalid debug state!
	#endif

			if (std::is_same<Scalar, double>::value)
			{
				ocean_assert(debugHessian_.rows() == debugHessian_.columns());
				SparseMatrix debugCopyHessian(debugHessian_);

				if (lambda > 0)
				{
					for (size_t n = 0; n < debugCopyHessian.rows(); ++n)
					{
						ocean_assert(!debugCopyHessian.isZero(n, n));
						debugCopyHessian(n, n) *= Scalar(1) + lambda;
					}
				}

				for (size_t n = 0; n < flippedCameras_T_world_.size(); ++n)
				{
					for (size_t r = 0; r < 6; ++r)
					{
						for (size_t c = 0; c < 6; ++c)
						{
							const Scalar value0 = debugCopyHessian(n * 6 + r, n * 6 + c);
							const Scalar value1 = matrixA_[n](r, c);
							ocean_assert(Numeric::isEqual<-6>(value0, value1, Numeric::weakEps()));
						}
					}
				}

				const size_t sizeA = flippedCameras_T_world_.size() * 6;
				const size_t sizeB = correspondenceGroups_.groups() * 3;

				SparseMatrix D(debugCopyHessian.submatrix(sizeA, sizeA, sizeB, sizeB));

				if (!D.invertBlockDiagonal3())
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				const SparseMatrix A(debugCopyHessian.submatrix(0, 0, sizeA, sizeA));
				const SparseMatrix B(debugCopyHessian.submatrix(0, sizeA, sizeA, sizeB));
				const SparseMatrix C(debugCopyHessian.submatrix(sizeA, 0, sizeB, sizeA));

				const Matrix ea(sizeA, 1, debugJacobianError_.data());
				const Matrix eb(sizeB, 1, debugJacobianError_.data() + sizeA);

				const SparseMatrix debugLeft = A - B * (D * C);
				const Matrix debugRight = ea - B * (D * eb);

				ocean_assert(debugLeft.rows() == left.rows() && debugLeft.columns() == left.columns());

				for (size_t r = 0; r < debugLeft.rows(); ++r)
				{
					for (size_t c = 0; c < debugLeft.columns(); ++c)
					{
						const Scalar value0 = debugLeft(r, c);
						const Scalar value1 = left(r, c);
						ocean_assert(Numeric::isEqual<-6>(value0, value1, Numeric::weakEps()));
					}
				}

				for (size_t r = 0; r < flippedCameras_T_world_.size() * 6; ++r)
				{
					const Scalar value0 = debugRight(r, 0);
					const Scalar value1 = resultVector[r];
					ocean_assert(Numeric::isEqual<-6>(value0, value1, Numeric::weakEps()));
				}

				Matrix da;
				if (!debugLeft.solve(debugRight, da))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				for (size_t r = 0; r < flippedCameras_T_world_.size() * 6; ++r)
				{
					const Scalar value0 = da(r, 0);
					const Scalar value1 = deltas(r, 0);
					ocean_assert(Numeric::isEqual<-6>(value0, value1, Numeric::weakEps() * 100));
				}

				const Matrix db = D * (eb - C * da);

				for (size_t r = 0; r < 3 * correspondenceGroups_.groups(); ++r)
				{
					const Scalar value0 = db(r, 0);
					const Scalar value1 = deltas.data()[flippedCameras_T_world_.size() * 6 + r];
					ocean_assert(Numeric::isEqual<-6>(value0, value1, Numeric::weakEps() * 100));
				}
			}
#endif // OCEAN_INTENSIVE_DEBUG

			return true;
		}

	protected:

		/// The camera profiles defining the projection for each individual camera frame.
		const ConstIndexedAccessor<const AnyCamera*>& cameras_;

		/// The accessor for all camera poses.
		NonconstTemplateArrayAccessor<HomogenousMatrix4>& flippedCameras_T_world_;

		/// The  candidate cameras poses.
		HomogenousMatrices4 candidateFlippedCameras_T_world_;

		/// The locations of the 3D object points of the most recent succeeded optimization step.
		NonconstTemplateArrayAccessor<Vector3>& objectPoints_;

		/// The locations of the candidate object points.
		Vectors3 objectPointCandidates_;

		/// The accessor for the individual groups of correspondences between pose indices and image point location, one group for each object point.
		const ObjectPointGroupsAccessor& correspondenceGroups_;

		/// True, if all 3D object points (before and after optimization) must lie in front of both cameras.
		const bool onlyFrontObjectPoints_;

		/// The upper left 6x6 sub-matrix of the Hessian matrix.
		StaticMatrices6x6 matrixA_;

		/// The upper right 6x3 sub-matrices of the Hessian matrix.
		StaticMatrices6x3 matrixB_;

		/// The individual states for all sub-matrices of b defining whether the matrix is zero or not.
		std::vector<uint8_t> nonZeroMatrixB_;

		/// The lower right 3x3 sub-matrices of the Hessian matrix.
		SquareMatrices3 matrixD_;

		/// The inverted 3x3 sub-matrices of the Hessian matrix.
		SquareMatrices3 matrixInvertedD_;

		/// The error vector multiplied by the Jacobian matrix.
		Scalars jacobianErrorVector_;

		/// The copy of the diagonal of matrix A.
		Scalars diagonalMatrixA_;

		/// The copy of the diagonal of matrix D.
		Scalars diagonalMatrixD_;

		/// Intermediate square error values.
		Scalars intermediateSqrErrors_;

		/// Intermediate error values.
		Vectors2 intermediateErrors_;

		/// Intermediate weight values.
		Scalars intermediateWeights_;

		/// The number of rows of the entire Jacobian.
		size_t jacobianRows_;

#ifdef OCEAN_DEBUG
		/// The Jacobian matrix.
		SparseMatrix debugJacobian_;

		/// The Hessian matrix.
		SparseMatrix debugHessian_;

		/// The error vector multiplied by the Jacobian matrix.
		Matrix debugJacobianError_;
#endif
};

bool NonLinearOptimizationObjectPoint::optimizeObjectPointsAndPoses(const ConstIndexedAccessor<const AnyCamera*>& cameras, const ConstIndexedAccessor<HomogenousMatrix4>& world_T_cameras, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, NonconstIndexedAccessor<HomogenousMatrix4>* world_T_optimizedCameras, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator, const Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
	ocean_assert(world_T_optimizedCameras == nullptr || world_T_cameras.size() == world_T_optimizedCameras->size());
	ocean_assert(optimizedObjectPoints == nullptr || objectPoints.size() == optimizedObjectPoints->size());

	ocean_assert(objectPoints.size() == correspondenceGroups.groups());
	if (objectPoints.size() != correspondenceGroups.groups())
	{
		return false;
	}

	const ScopedConstMemoryAccessor<HomogenousMatrix4> scoped_world_T_cameras(world_T_cameras);

	HomogenousMatrices4 flippedCamera_T_world(scoped_world_T_cameras.size());
	for (size_t n = 0; n < flippedCamera_T_world.size(); ++n)
	{
		flippedCamera_T_world[n] = AnyCamera::standard2InvertedFlipped(scoped_world_T_cameras[n]);
	}

	HomogenousMatrices4 flippedOptimizedCameras_T_world;
	NonconstArrayAccessor<HomogenousMatrix4> optimizedPosesAccessorIF(flippedOptimizedCameras_T_world, world_T_optimizedCameras != nullptr ? world_T_optimizedCameras->size() : 0);

	if (!optimizeObjectPointsAndPosesIF(cameras, ConstArrayAccessor<HomogenousMatrix4>(flippedCamera_T_world), objectPoints, correspondenceGroups, optimizedPosesAccessorIF.pointer(), optimizedObjectPoints, iterations, estimator, lambda, lambdaFactor, onlyFrontObjectPoints, initialError, finalError, intermediateErrors))
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

bool NonLinearOptimizationObjectPoint::optimizeObjectPointsAndPosesIF(const ConstIndexedAccessor<const AnyCamera*>& cameras, const ConstIndexedAccessor<HomogenousMatrix4>& flippedCameras_T_world, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, NonconstIndexedAccessor<HomogenousMatrix4>* flippedOptimizedCameras_T_world, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator, const Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
	ocean_assert(flippedOptimizedCameras_T_world == nullptr || flippedCameras_T_world.size() == flippedOptimizedCameras_T_world->size());
	ocean_assert(optimizedObjectPoints == nullptr || objectPoints.size() == optimizedObjectPoints->size());

	ocean_assert(objectPoints.size() == correspondenceGroups.groups());
	if (objectPoints.size() != correspondenceGroups.groups())
	{
		return false;
	}

	// we need enough buffer for the optimized poses, we take them from the provided parameter or create them temporary in this scope
	ScopedNonconstMemoryAccessor<HomogenousMatrix4> scoped_flippedOptimizedCameras_T_world(flippedOptimizedCameras_T_world, flippedCameras_T_world.size());
	ocean_assert(scoped_flippedOptimizedCameras_T_world.size() == flippedCameras_T_world.size());

	const ScopedConstMemoryAccessor<HomogenousMatrix4> scopedAccessor_flippedCameras_T_world(flippedCameras_T_world);
	for (size_t n = 0; n < flippedCameras_T_world.size(); ++n)
	{
		scoped_flippedOptimizedCameras_T_world[n] = scopedAccessor_flippedCameras_T_world[n];
	}

	// we need enough buffer for the optimized object points, we take them from the provided parameter or create them temporary in this scope
	ScopedNonconstMemoryAccessor<Vector3> scopedOptimizedObjectPoints(optimizedObjectPoints, objectPoints.size());
	ocean_assert(scopedOptimizedObjectPoints.size() == objectPoints.size());

	const ScopedConstMemoryAccessor<Vector3> scopedObjectPoints(objectPoints);
	for (size_t n = 0; n < objectPoints.size(); ++n)
	{
		scopedOptimizedObjectPoints[n] = scopedObjectPoints[n];
	}

#ifdef OCEAN_DEBUG
	for (size_t n = 0; n < correspondenceGroups.groups(); ++n)
	{
		const Vector3& objectPoint = scopedOptimizedObjectPoints[n];

		for (size_t i = 0; i < correspondenceGroups.groupElements(n); ++i)
		{
			Index32 poseIndex;
			Vector2 imagePoint;
			correspondenceGroups.element(n, i, poseIndex, imagePoint);

			ocean_assert(flippedCameras_T_world.canAccess(poseIndex));
			const HomogenousMatrix4& flippedOptimizedCamera_T_world = scoped_flippedOptimizedCameras_T_world[poseIndex];

			const Vector2 projectedObjectPoint(cameras[poseIndex]->projectToImageIF(flippedOptimizedCamera_T_world, objectPoint));
			const Scalar sqrDistance = projectedObjectPoint.sqrDistance(imagePoint);
			OCEAN_SUPPRESS_UNUSED_WARNING(sqrDistance);

			// we take a very large error due to outliers, however normally the error should be less then 5 * 5
			// however, the following test does not make sense if we e.g., try to adjust the camera poses and object points to a different camera profile, so we disable the assert
			//ocean_assert(sqrDistance < 20 * 20);
		}
	}
#endif

	NonconstTemplateArrayAccessor<HomogenousMatrix4> accessor_flippedOptimizedCameras_T_world(scoped_flippedOptimizedCameras_T_world.data(), scoped_flippedOptimizedCameras_T_world.size());
	NonconstTemplateArrayAccessor<Vector3> objectPointsAccessor(scopedOptimizedObjectPoints.data(), scopedOptimizedObjectPoints.size());

	switch (estimator)
	{
		case Estimator::ET_LINEAR:
		{
			ObjectPointsPosesProvider<Estimator::ET_LINEAR> provider(cameras, accessor_flippedOptimizedCameras_T_world, objectPointsAccessor, correspondenceGroups, onlyFrontObjectPoints);
			return advancedSparseOptimization(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors);
		}

		case Estimator::ET_HUBER:
		{
			ObjectPointsPosesProvider<Estimator::ET_HUBER> provider(cameras, accessor_flippedOptimizedCameras_T_world, objectPointsAccessor, correspondenceGroups, onlyFrontObjectPoints);
			return advancedSparseOptimization(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors);
		}

		case Estimator::ET_TUKEY:
		{
			ObjectPointsPosesProvider<Estimator::ET_TUKEY> provider(cameras, accessor_flippedOptimizedCameras_T_world, objectPointsAccessor, correspondenceGroups, onlyFrontObjectPoints);
			return advancedSparseOptimization(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors);
		}

		case Estimator::ET_CAUCHY:
		{
			ObjectPointsPosesProvider<Estimator::ET_CAUCHY> provider(cameras, accessor_flippedOptimizedCameras_T_world, objectPointsAccessor, correspondenceGroups, onlyFrontObjectPoints);
			return advancedSparseOptimization(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors);
		}

		case Estimator::ET_SQUARE:
		{
			ObjectPointsPosesProvider<Estimator::ET_SQUARE> provider(cameras, accessor_flippedOptimizedCameras_T_world, objectPointsAccessor, correspondenceGroups, onlyFrontObjectPoints);
			return advancedSparseOptimization(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors);
		}

		case Estimator::ET_INVALID:
		{
			break;
		}
	}

	ocean_assert(false && "This should never happen!");
	return false;
}

/**
 * This class implements an optimization provider allowing to optimize the 3-DOF orientations of several 6-DOF camera poses and 3-DOF object points concurrently.
 * This provider is highly optimized not using sparse matrices but implements the individual sub-matrices of the sparse linear equation by small dense sub-matrices.
 * The implementation uses the fact the Hessian matrix is symmetric, and further applies the Schur complement during solving.
 * We divide the sparse Hessian matrix into four sub-matrices:
 *     | A B |   |  A  B |
 * H = | C D | = | B^T D |, further A and D are symmetric
 *
 * @tparam tEstimator The type of the estimator to be used
 */
template <Estimator::EstimatorType tEstimator>
class NonLinearOptimizationObjectPoint::ObjectPointsOrientationalPosesProvider : public NonLinearOptimization::AdvancedSparseOptimizationProvider
{
	public:

		/**
		 * Creates a new provider object.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param flippedCameras_T_world The inverted and flipped camera poses which will be optimized, with default camera pointing towards the positive z-space with y-axis downwards, at least two
		 * @param objectPointAccessor The 3D object point locations which will be optimized
		 * @param correspondenceGroups The accessor for the individual groups of correspondences between pose indices and image point location, one group for each object point
		 * @param onlyFrontObjectPoints True, to ensure that all 3D object point locations will lie in front of both cameras
		 */
		inline ObjectPointsOrientationalPosesProvider(const ConstIndexedAccessor<const AnyCamera*>& cameras, NonconstTemplateArrayAccessor<HomogenousMatrix4>& flippedCameras_T_world, NonconstTemplateArrayAccessor<Vector3>& objectPointAccessor, const ObjectPointGroupsAccessor& correspondenceGroups, const bool onlyFrontObjectPoints) :
			cameras_(cameras),
			flippedCameras_T_world_(flippedCameras_T_world),
			candidateFlippedCameras_T_world_(Accessor::accessor2elements(flippedCameras_T_world)),
			objectPoints_(objectPointAccessor),
			objectPointCandidates_(Accessor::accessor2elements(objectPointAccessor)),
			correspondenceGroups_(correspondenceGroups),
			onlyFrontObjectPoints_(onlyFrontObjectPoints),
			matrixA_(flippedCameras_T_world.size()),
			matrixB_(correspondenceGroups.groups() * flippedCameras_T_world.size()),
			nonZeroMatrixB_(correspondenceGroups.groups() * flippedCameras_T_world.size()),
			matrixD_(correspondenceGroups.groups()),
			matrixInvertedD_(correspondenceGroups.groups()),
			jacobianErrorVector_(flippedCameras_T_world.size() * 3 + correspondenceGroups.groups() * 3),
			diagonalMatrixA_(flippedCameras_T_world.size() * 3),
			diagonalMatrixD_(correspondenceGroups.groups() * 3),
			jacobianRows_(0)
		{
			ocean_assert(correspondenceGroups.groups() >= 5);

			// each observation (combination of pose and object point) creates two rows
			for (size_t n = 0; n < correspondenceGroups_.groups(); ++n)
			{
				jacobianRows_ += correspondenceGroups_.groupElements(n) * 2;
			}

			/**
			 * camera_T_world
			 * | R | t |     | I | t |   | R | 0 |
			 * | 0 | 1 |  =  | 0 | 1 | * | 0 | 1 |   (first rotating world object point, then translating the point)
			 *
			 *               | R | 0 |   | I | R^-1 t |
			 *            =  | 0 | 1 | * | 0 |    1   |   (first translating the world object point, then rotating the point)
			 */

			translations_T_world_.reserve(candidateFlippedCameras_T_world_.size());

			for (size_t n = 0; n < candidateFlippedCameras_T_world_.size(); ++n)
			{
				translations_T_world_.emplace_back(candidateFlippedCameras_T_world_[n].rotationMatrix().inverted() * candidateFlippedCameras_T_world_[n].translation());

				ocean_assert(candidateFlippedCameras_T_world_[n].isEqual(HomogenousMatrix4(candidateFlippedCameras_T_world_[n].rotationMatrix()) * HomogenousMatrix4(translations_T_world_[n]), Numeric::weakEps()));
			}
		}

		/**
		 * Determines the error for the current model candidate (not the actual/actual model).
		 * @see AdvancedSparseOptimizationProvider::determineError().
		 */
		inline Scalar determineError()
		{
			if constexpr (!Estimator::isStandardEstimator<tEstimator>())
			{
				intermediateSqrErrors_.resize(jacobianRows_ / 2);
			}

			Index32 poseId;
			Vector2 imagePoint;

			Scalar sqrError = 0;
			size_t row = 0;

			for (size_t o = 0; o < correspondenceGroups_.groups(); ++o)
			{
				const ObjectPoint& objectPoint = objectPointCandidates_[o];

				for (size_t p = 0; p < correspondenceGroups_.groupElements(o); ++p)
				{
					correspondenceGroups_.element(o, p, poseId, imagePoint);

					const HomogenousMatrix4& candidateFlippedCamera_T_world = candidateFlippedCameras_T_world_[poseId];
					const AnyCamera& camera = *cameras_[poseId];

					if (onlyFrontObjectPoints_ && !AnyCamera::isObjectPointInFrontIF(candidateFlippedCamera_T_world, objectPoint))
					{
						return Numeric::maxValue();
					}

					const Scalar localSqrError = Error::determinePoseErrorIF(candidateFlippedCamera_T_world, camera, objectPoint, imagePoint).sqr();

					if constexpr (Estimator::isStandardEstimator<tEstimator>())
					{
						sqrError += localSqrError;
					}
					else
					{
						intermediateSqrErrors_[row++] = localSqrError;
					}
				}
			}

			if constexpr (Estimator::isStandardEstimator<tEstimator>())
			{
				ocean_assert((jacobianRows_ / 2) != 0);
				return sqrError / Scalar(jacobianRows_ / 2);
			}
			else
			{
				ocean_assert(!intermediateSqrErrors_.empty() && row == intermediateSqrErrors_.size());
				return Estimator::determineRobustError<tEstimator>(intermediateSqrErrors_.data(), intermediateSqrErrors_.size(), candidateFlippedCameras_T_world_.size() * 3 + correspondenceGroups_.groups() * 3);
			}
		}

		/**
		 * Determines any kind of (abstract) parameters based on the current/actual model (not the model candidate) e.g., the Jacobian parameters and/or a Hessian matrix.
		 * @see AdvancedSparseOptimizationProvider::determineParameters().
		 */
		inline bool determineParameters()
		{
			memset(jacobianErrorVector_.data(), 0x00, sizeof(Scalar) * jacobianErrorVector_.size());

			intermediateErrors_.resize(jacobianRows_ / 2);

			if constexpr (!Estimator::isStandardEstimator<tEstimator>())
			{
				intermediateSqrErrors_.resize(jacobianRows_ / 2);
				intermediateWeights_.resize(jacobianRows_ / 2);
			}

			Index32 poseId;
			Vector2 imagePoint;
			size_t row = 0;

			for (size_t o = 0; o < correspondenceGroups_.groups(); ++o)
			{
				const ObjectPoint& objectPoint = objectPointCandidates_[o];

				for (size_t p = 0; p < correspondenceGroups_.groupElements(o); ++p)
				{
					correspondenceGroups_.element(o, p, poseId, imagePoint);

					const HomogenousMatrix4& candidateFlippedCamera_T_world = candidateFlippedCameras_T_world_[poseId];
					const AnyCamera& camera = *cameras_[poseId];

					intermediateErrors_[row] = Error::determinePoseErrorIF(candidateFlippedCamera_T_world, camera, objectPoint, imagePoint);

					if constexpr (!Estimator::isStandardEstimator<tEstimator>())
					{
						intermediateSqrErrors_[row] = intermediateErrors_[row].sqr();
					}

					row++;
				}
			}

			ocean_assert(row == jacobianRows_ / 2);

			if constexpr (!Estimator::isStandardEstimator<tEstimator>())
			{
				const Scalar sqrSigma = Estimator::needSigma<tEstimator>() ? Numeric::sqr(Estimator::determineSigmaSquare<tEstimator>(intermediateSqrErrors_.data(), intermediateSqrErrors_.size(), candidateFlippedCameras_T_world_.size() * 3 + correspondenceGroups_.groups() * 3)) : 0;

				for (size_t n = 0; n < intermediateWeights_.size(); ++n)
				{
					// we determine the weights, however as e.g., the tukey estimator may return a weight of 0 we have to clamp the weight to ensure that we still can solve the equation
					// **NOTE** the much better way would be to remove the entry from the equation and to solve it
					intermediateWeights_[n] = max(Numeric::weakEps(), Estimator::robustWeightSquare<tEstimator>(intermediateSqrErrors_[n], sqrSigma));
				}
			}

			SquareMatrices3 rotationRodriguesDerivatives(candidateFlippedCameras_T_world_.size() * 3);
			for (size_t n = 0; n < candidateFlippedCameras_T_world_.size(); ++n)
			{
				const ExponentialMap rotation(candidateFlippedCameras_T_world_[n].rotation());

				Jacobian::calculateRotationRodriguesDerivative(rotation, rotationRodriguesDerivatives[n * 3 + 0], rotationRodriguesDerivatives[n * 3 + 1], rotationRodriguesDerivatives[n * 3 + 2]);

				matrixA_[n].toNull();
			}

#ifdef OCEAN_DEBUG
			memset(nonZeroMatrixB_.data(), 0x02u, sizeof(uint8_t) * nonZeroMatrixB_.size());
#endif

			Scalar orientationJacobianX[3], orientationJacobianY[3];
			Scalar pointJacobianX[3], pointJacobianY[3];

			row = 0;

			for (size_t o = 0; o < correspondenceGroups_.groups(); ++o)
			{
				const ObjectPoint& objectPoint = objectPointCandidates_[o];

				SquareMatrix3& subMatrixD = matrixD_[o];
				subMatrixD.toNull();

				// if not all poses 'see' the object point we have to 'zero' the corresponding matrix b
				if (correspondenceGroups_.groupElements(o) != candidateFlippedCameras_T_world_.size())
				{
					for (size_t p = 0; p < candidateFlippedCameras_T_world_.size(); ++p)
					{
						nonZeroMatrixB_[p * correspondenceGroups_.groups() + o] = 0u;

#ifdef OCEAN_DEBUG
						StaticMatrix3x3& subMatrixB = matrixB_[p * correspondenceGroups_.groups() + o];
						for (size_t i = 0; i < subMatrixB.elements(); ++i)
						{
							subMatrixB[i] = Numeric::minValue();
						}
#endif
					}
				}

				for (size_t p = 0; p < correspondenceGroups_.groupElements(o); ++p)
				{
					correspondenceGroups_.element(o, p, poseId, imagePoint);

					const HomogenousMatrix4 candidateFlippedCamera_T_world = candidateFlippedCameras_T_world_[poseId];
					const SquareMatrix3 candidateFlippedCamera_T_translation = candidateFlippedCamera_T_world.rotationMatrix();
					const Vector3& translation_T_world = translations_T_world_[poseId];

					const AnyCamera& camera = *cameras_[poseId];

					Jacobian::calculateOrientationalJacobianRodrigues2x3IF(camera, candidateFlippedCamera_T_translation, translation_T_world, objectPoint, rotationRodriguesDerivatives[poseId * 3 + 0], rotationRodriguesDerivatives[poseId * 3 + 1], rotationRodriguesDerivatives[poseId * 3 + 2], orientationJacobianX, orientationJacobianY);
					Jacobian::calculatePointJacobian2x3IF(camera, candidateFlippedCamera_T_world, objectPoint, pointJacobianX, pointJacobianY);

					StaticMatrix3x3& subMatrixA = matrixA_[poseId];
					StaticMatrix3x3& subMatrixB = matrixB_[poseId * correspondenceGroups_.groups() + o];
					nonZeroMatrixB_[poseId * correspondenceGroups_.groups() + o] = 1u;

					if constexpr (Estimator::isStandardEstimator<tEstimator>())
					{
						// we calculate the upper triangle of the matrix A (as A is symmetric)
						for (size_t r = 0; r < 3; ++r)
						{
							for (size_t c = r; c < 3; ++c)
							{
								subMatrixA(r, c) += orientationJacobianX[r] * orientationJacobianX[c] + orientationJacobianY[r] * orientationJacobianY[c];
							}
						}

						// we calculate the upper triangle(s) of the matrix D (as D is symmetric)
						for (unsigned int r = 0u; r < 3u; ++r)
						{
							for (unsigned int c = r; c < 3u; ++c)
							{
								subMatrixD(r, c) += pointJacobianX[r] * pointJacobianX[c] + pointJacobianY[r] * pointJacobianY[c];
							}
						}

						// we calculate the matrix B
						for (size_t r = 0; r < 3; ++r)
						{
							for (size_t c = 0; c < 3; ++c)
							{
								subMatrixB(r, c) = orientationJacobianX[r] * pointJacobianX[c] + orientationJacobianY[r] * pointJacobianY[c];
							}
						}
					}
					else
					{
						const Scalar& robustWeight = intermediateWeights_[row];

						// we calculate the upper triangle of the matrix A (as A is symmetric)
						for (size_t r = 0; r < 3; ++r)
						{
							for (size_t c = r; c < 3; ++c)
							{
								subMatrixA(r, c) += (orientationJacobianX[r] * orientationJacobianX[c] + orientationJacobianY[r] * orientationJacobianY[c]) * robustWeight;
							}
						}

						// we calculate the upper triangle(s) of the matrix D (as D is symmetric)
						for (unsigned int r = 0u; r < 3u; ++r)
						{
							for (unsigned int c = r; c < 3u; ++c)
							{
								subMatrixD(r, c) += (pointJacobianX[r] * pointJacobianX[c] + pointJacobianY[r] * pointJacobianY[c]) * robustWeight;
							}
						}

						// we calculate the matrix B
						for (size_t r = 0; r < 3; ++r)
						{
							for (size_t c = 0; c < 3; ++c)
							{
								subMatrixB(r, c) = (orientationJacobianX[r] * pointJacobianX[c] + orientationJacobianY[r] * pointJacobianY[c]) * robustWeight;
							}
						}
					}

					ocean_assert(intermediateErrors_[row] == Error::determinePoseErrorIF(candidateFlippedCamera_T_world, camera, objectPoint, imagePoint));

					if constexpr (Estimator::isStandardEstimator<tEstimator>())
					{
						const Vector2& error = intermediateErrors_[row];

						for (size_t i = 0; i < 3; ++i)
						{
							jacobianErrorVector_[poseId * 3 + i] += orientationJacobianX[i] * error[0] + orientationJacobianY[i] * error[1];
						}

						for (size_t i = 0; i < 3; ++i)
						{
							jacobianErrorVector_[3 * candidateFlippedCameras_T_world_.size() + 3 * o + i] += pointJacobianX[i] * error[0] + pointJacobianY[i] * error[1];
						}
					}
					else
					{
						const Vector2 error(intermediateErrors_[row] * intermediateWeights_[row]);

						for (size_t i = 0; i < 3; ++i)
						{
							jacobianErrorVector_[poseId * 3 + i] += orientationJacobianX[i] * error[0] + orientationJacobianY[i] * error[1];
						}

						for (size_t i = 0; i < 3; ++i)
						{
							jacobianErrorVector_[3 * candidateFlippedCameras_T_world_.size() + 3 * o + i] += pointJacobianX[i] * error[0] + pointJacobianY[i] * error[1];
						}
					}

					row++;
				}

				// we copy the lower triangle of the sub-matrix D
				subMatrixD(1, 0) = subMatrixD(0, 1);
				subMatrixD(2, 0) = subMatrixD(0, 2);
				subMatrixD(2, 1) = subMatrixD(1, 2);

				ocean_assert(!subMatrixD.isNull());
			}

#ifdef OCEAN_DEBUG
			for (size_t n = 0; n < nonZeroMatrixB_.size(); ++n)
			{
				ocean_assert(nonZeroMatrixB_[n] != 0x02u);
			}
#endif

			for (StaticMatrix3x3& matrix : matrixA_)
			{
				// we copy the lower triangle from the upper triangle
				for (size_t r = 1; r < 3; ++r)
				{
					for (size_t c = 0; c < r; ++c)
					{
						matrix(r, c) = matrix(c, r);
					}
				}
			}

			// we make a copy of the diagonal elements of matrix A so that we can apply a lambda later during the solve step
			for (size_t n = 0; n < candidateFlippedCameras_T_world_.size(); ++n)
			{
				const StaticMatrix3x3& subMatrixA = matrixA_[n];

				for (size_t i = 0; i < 3; ++i)
				{
					diagonalMatrixA_[3 * n + i] = subMatrixA(i, i);
				}
			}

#ifdef OCEAN_INTENSIVE_DEBUG
	#ifndef OCEAN_DEBUG
			#error Invalid debug state!
	#endif

			if constexpr (std::is_same<Scalar, double>::value)
			{
				SquareMatrices3 rotationRodriguesDerivatives(flippedCameras_T_world_.size() * 3);
				for (size_t n = 0; n < flippedCameras_T_world_.size(); ++n)
				{
					const Pose pose(flippedCameras_T_world_[n]);
					Jacobian::calculateRotationRodriguesDerivative(ExponentialMap(pose.rx(), pose.ry(), pose.rz()), rotationRodriguesDerivatives[n * 3 + 0], rotationRodriguesDerivatives[n * 3 + 1], rotationRodriguesDerivatives[n * 3 + 2]);
				}

				Scalar pointJacobianBuffer[6];
				Scalar poseJacobianBuffer[6];

				SparseMatrix::Entries jacobianEntries;
				jacobianEntries.reserve(jacobianRows_ * 12);

				size_t row = 0;

				for (size_t o = 0; o < correspondenceGroups_.groups(); ++o)
				{
					const ObjectPoint& objectPoint = objectPointCandidates_[o];

					for (size_t p = 0; p < correspondenceGroups_.groupElements(o); ++p)
					{
						correspondenceGroups_.element(o, p, poseId, imagePoint);

						const HomogenousMatrix4& candidateFlippedCamera_T_world = candidateFlippedCameras_T_world_[poseId];
						const AnyCamera& camera = *cameras_[poseId];

						Jacobian::calculateOrientationalJacobianRodrigues2x3IF(camera, candidateFlippedCamera_T_world.rotationMatrix(), translations_T_world_[poseId], objectPoint, rotationRodriguesDerivatives[poseId * 3 + 0], rotationRodriguesDerivatives[poseId * 3 + 1], rotationRodriguesDerivatives[poseId * 3 + 2], poseJacobianBuffer, poseJacobianBuffer + 3);

						for (size_t e = 0; e < 3; ++e)
						{
							jacobianEntries.emplace_back(row + 0, poseId * 3 + e, poseJacobianBuffer[0 + e]);
							jacobianEntries.emplace_back(row + 1, poseId * 3 + e, poseJacobianBuffer[3 + e]);
						}

						Jacobian::calculatePointJacobian2x3IF(camera, candidateFlippedCamera_T_world, objectPoint, pointJacobianBuffer, pointJacobianBuffer + 3);

						for (size_t e = 0; e < 3; ++e)
						{
							jacobianEntries.emplace_back(row + 0, candidateFlippedCameras_T_world_.size() * 3 + 3 * o + e, pointJacobianBuffer[0 + e]);
							jacobianEntries.emplace_back(row + 1, candidateFlippedCameras_T_world_.size() * 3 + 3 * o + e, pointJacobianBuffer[3 + e]);
						}

						row += 2;
					}
				}

				ocean_assert(row == jacobianRows_);
				debugJacobian_ = SparseMatrix(jacobianRows_, candidateFlippedCameras_T_world_.size() * 3 + correspondenceGroups_.groups() * 3, jacobianEntries);

				const Scalar debugSqrSigma = Estimator::needSigma<tEstimator>() ? Numeric::sqr(Estimator::determineSigmaSquare<tEstimator>(intermediateSqrErrors_.data(), intermediateSqrErrors_.size(), candidateFlippedCameras_T_world_.size() * 3 + correspondenceGroups_.groups() * 3)) : 0;

				SparseMatrix::Entries weightEntries;
				weightEntries.reserve(jacobianRows_);

				for (size_t n = 0; n < jacobianRows_; ++n)
				{
					if constexpr (Estimator::isStandardEstimator<tEstimator>())
					{
						weightEntries.emplace_back(n, n, Scalar(1));
					}
					else
					{
						const Scalar weight = max(Numeric::weakEps(), Estimator::robustWeightSquare<tEstimator>(intermediateSqrErrors_[n / 2], debugSqrSigma));

						weightEntries.emplace_back(n, n, weight);
					}
				}

				SparseMatrix debugWeight(jacobianRows_, jacobianRows_, weightEntries);

				debugHessian_ = debugJacobian_.transposed() * debugWeight * debugJacobian_;

				const SparseMatrix subMatrixA(debugHessian_.submatrix(0, 0, candidateFlippedCameras_T_world_.size() * 3, candidateFlippedCameras_T_world_.size() * 3));
				const SparseMatrix subMatrixB(debugHessian_.submatrix(0, candidateFlippedCameras_T_world_.size() * 3, candidateFlippedCameras_T_world_.size() * 3, 3 * correspondenceGroups_.groups()));
				const SparseMatrix subMatrixC(debugHessian_.submatrix(candidateFlippedCameras_T_world_.size() * 3, 0, 3 * correspondenceGroups_.groups(), candidateFlippedCameras_T_world_.size() * 3));
				const SparseMatrix subMatrixD(debugHessian_.submatrix(candidateFlippedCameras_T_world_.size() * 3, candidateFlippedCameras_T_world_.size() * 3, 3 * correspondenceGroups_.groups(), 3 * correspondenceGroups_.groups()));

				for (size_t n = 0; n < candidateFlippedCameras_T_world_.size(); ++n)
				{
					for (size_t r = 0; r < 3; ++r)
					{
						for (size_t c = 0; c < 3; ++c)
						{
							const Scalar value0 = matrixA_[n](r, c);
							const Scalar value1 = subMatrixA(n * 3 + r, n * 3 + c);
							ocean_assert(Numeric::isEqual<-6>(value0, value1, Numeric::weakEps()));
						}
					}
				}

				for (size_t n = 0; n < candidateFlippedCameras_T_world_.size(); ++n)
				{
					for (size_t i = 0; i < correspondenceGroups_.groups(); ++i)
					{
						for (unsigned int r = 0u; r < 3u; ++r)
						{
							for (unsigned int c = 0u; c < 3u; ++c)
							{
								if (nonZeroMatrixB_[n * correspondenceGroups_.groups() + i])
								{
									const Scalar value0 = matrixB_[n * correspondenceGroups_.groups() + i](r, c);
									const Scalar value1 = subMatrixB(n * 3 + r, i * 3 + c);
									ocean_assert(Numeric::isEqual<-6>(value0, value1, Numeric::weakEps()));

									const Scalar value2 = subMatrixC(i * 3 + c, n * 3 + r);
									ocean_assert(Numeric::isEqual<-6>(value0, value2, Numeric::weakEps()));
								}
								else
								{
									const Scalar value0 = matrixB_[n * correspondenceGroups_.groups() + i](r, c);
									ocean_assert(value0 == Numeric::minValue());

									const Scalar value1 = subMatrixB(n * 3 + r, i * 3 + c);
									ocean_assert(value1 == 0);

									const Scalar value2 = subMatrixC(i * 3 + c, n * 3 + r);
									ocean_assert(value2 == 0);
								}
							}
						}
					}
				}

				for (size_t n = 0; n < correspondenceGroups_.groups(); ++n)
				{
					for (unsigned int r = 0u; r < 3u; ++r)
					{
						for (unsigned int c = 0u; c < 3u; ++c)
						{
							const Scalar value0 = matrixD_[n](r, c);
							const Scalar value1 = subMatrixD(n * 3 + r, n * 3 + c);
							ocean_assert(Numeric::isEqual<-6>(value0, value1, Numeric::weakEps()));
						}
					}
				}

				debugJacobianError_.resize(jacobianRows_, 1);

				row = 0;

				for (size_t o = 0; o < correspondenceGroups_.groups(); ++o)
				{
					const ObjectPoint& objectPoint = objectPointCandidates_[o];

					for (size_t p = 0; p < correspondenceGroups_.groupElements(o); ++p)
					{
						correspondenceGroups_.element(o, p, poseId, imagePoint);

						const HomogenousMatrix4& candidateFlippedCamera_T_world = candidateFlippedCameras_T_world_[poseId];
						const AnyCamera& camera = *cameras_[poseId];

						const Vector2 error = Error::determinePoseErrorIF(candidateFlippedCamera_T_world, camera, objectPoint, imagePoint);

						debugJacobianError_(row++, 0) = error[0];
						debugJacobianError_(row++, 0) = error[1];
					}
				}

				ocean_assert(row == jacobianRows_);

				debugJacobianError_ = debugJacobian_.transposed() * debugWeight * debugJacobianError_;
				ocean_assert(debugJacobianError_.rows() == jacobianErrorVector_.size());
				ocean_assert(debugJacobianError_.columns() == 1);

				for (size_t n = 0; n < jacobianErrorVector_.size(); ++n)
				{
					const Scalar value0 = jacobianErrorVector_[n];
					const Scalar value1 = debugJacobianError_(n, 0);

					ocean_assert(Numeric::isEqual<-6>(value0, value1, Numeric::weakEps()));
				}
			}
#endif // OCEAN_INTENSIVE_DEBUG

			return true;
		}

		/**
		 * Creates a new model candidate by adjusting the current/actual model with delta values.
		 * @see AdvancedSparseOptimizationProvider::applyCorrection().
		 */
		inline void applyCorrection(const Matrix& deltas)
		{
			for (size_t n = 0; n < candidateFlippedCameras_T_world_.size(); ++n)
			{
				const ExponentialMap oldOrientation(candidateFlippedCameras_T_world_[n].rotation());

				// p_i+1 = p_i + delta_i
				// p_i+1 = p_i - (-delta_i)

				const ExponentialMap deltaOrientation(deltas(n * 3 + 0), deltas(n * 3 + 1), deltas(n * 3 + 2));
				const ExponentialMap newOrientation(oldOrientation - deltaOrientation);

				candidateFlippedCameras_T_world_[n] = HomogenousMatrix4(newOrientation.quaternion()) * HomogenousMatrix4(translations_T_world_[n]);
			}

			for (size_t n = 0; n < correspondenceGroups_.groups(); ++n)
			{
				objectPointCandidates_[n] = objectPoints_[n] - Vector3(deltas.data() + 3 * candidateFlippedCameras_T_world_.size() + n * 3);
			}
		}

		/**
		 * Accepts the current model candidate a new (better) model than the previous one.
		 * @see AdvancedSparseOptimizationProvider::acceptCorrection().
		 */
		inline void acceptCorrection()
		{
			ocean_assert(candidateFlippedCameras_T_world_.size() == flippedCameras_T_world_.size());
			memcpy(flippedCameras_T_world_.data(), candidateFlippedCameras_T_world_.data(), sizeof(HomogenousMatrix4) * flippedCameras_T_world_.size());

			ocean_assert(objectPointCandidates_.size() == correspondenceGroups_.groups());
			memcpy(objectPoints_.data(), objectPointCandidates_.data(), sizeof(Vector3) * correspondenceGroups_.groups());
		}

		/**
		 * Returns whether the optimization process should stop e.g., due to an external event.
		 * @see AdvancedSparseOptimizationProvider::shouldStop().
		 */
		inline bool shouldStop()
		{
			return false;
		}

		/**
		 * Solves the linear equation Hessian * deltas = -jacobianError based on the internal data.
		 * @see AdvancedSparseOptimizationProvider::solve().
		 */
		inline bool solve(Matrix& deltas, const Scalar lambda)
		{
			ocean_assert(lambda >= 0);

			// first we apply the lambda values to the diagonal of matrix A and D (if a lambda is defined)
			// afterwards, we invert the 3x3 block-diagonal matrix D by simply inverting the 3x3 blocks individually

			if (lambda > 0)
			{
				for (size_t n = 0; n < matrixA_.size(); ++n)
				{
					for (size_t i = 0; i < 3; ++i)
					{
						matrixA_[n](i, i) = diagonalMatrixA_[n * 3 + i] * (Scalar(1) + lambda);
					}
				}

				for (size_t n = 0; n < matrixD_.size(); ++n)
				{
					ocean_assert(matrixD_[n].isSymmetric());
					matrixInvertedD_[n] = matrixD_[n];

					for (unsigned int i = 0u; i < 3u; ++i)
					{
						matrixInvertedD_[n](i, i) *= Scalar(1) + lambda;
					}

					matrixInvertedD_[n].invert();
				}
			}
			else
			{
				for (size_t n = 0; n < matrixD_.size(); ++n)
				{
					ocean_assert(matrixD_[n].isSymmetric());
					matrixInvertedD_[n] = matrixD_[n];

					matrixInvertedD_[n].invert();
				}
			}

			/**
			 * We solve the equation by applying the Schur complement for the linear equation:
			 * | A B | * |da| = |ra|
			 * | C D | * |db| = |rb|, while C = B^T
			 *
			 * We solve da by:
			 * (A - B D^-1 B^T) da = ra - B D^-1 rb
			 *
			 * Then we solve db by:
			 * db = D^-1 (rb - C da)
			 */

			// first we calculate: ra - B D^-1 rb
			Scalars resultVector(matrixA_.size() * 3, Scalar(0));

			for (size_t n = 0; n < matrixInvertedD_.size(); ++n)
			{
				const Vector3 intermediate(matrixInvertedD_[n] * Vector3(jacobianErrorVector_.data() + 3 * matrixA_.size() + 3 * n));

				for (size_t i = 0; i < matrixA_.size(); ++i)
				{
					if (nonZeroMatrixB_[i * matrixInvertedD_.size() + n])
					{
						const StaticMatrix3x3& subMatrixB = matrixB_[i * matrixInvertedD_.size() + n];

						for (size_t t = 0; t < 3; ++t)
						{
							resultVector[i * 3 + t] += subMatrixB(t, 0) * intermediate[0] + subMatrixB(t, 1) * intermediate[1] + subMatrixB(t, 2) * intermediate[2];
						}
					}
				}
			}

			for (size_t n = 0; n < matrixA_.size() * 3; ++n)
			{
				resultVector[n] = jacobianErrorVector_[n] - resultVector[n];
			}

			// now we calculate A - B D^-1 B^T
			// as the result is a symmetric matrix we will calculate the lower left triangle only

			Matrix left(matrixA_.size() * 3, matrixA_.size() * 3, false);

			for (size_t i = 0; i < matrixA_.size(); ++i)
			{
				const StaticMatrix3x3& subMatrixA = matrixA_[i];

				for (unsigned int c = 0u; c < 3u; ++c)
				{
					for (unsigned int r = c; r < 3u; ++r)
					{
						left(i * 3 + r, i * 3 + c) = subMatrixA(r, c); // lower left triangle
					}
				}
			}

			StaticMatrix3x3 intermediate;

			for (size_t n = 0; n < matrixInvertedD_.size(); ++n)
			{
				const SquareMatrix3& subMatrixInvertedD = matrixInvertedD_[n];

				for (size_t i = 0; i < matrixA_.size(); ++i)
				{
					if (nonZeroMatrixB_[i * matrixInvertedD_.size() + n])
					{
						const StaticMatrix3x3& subMatrixB = matrixB_[i * matrixInvertedD_.size() + n];

						for (unsigned int r = 0u; r < 3u; ++r)
						{
							for (unsigned int c = 0u; c < 3u; ++c)
							{
								intermediate(r, c) = subMatrixInvertedD(r, 0) * subMatrixB(c, 0) + subMatrixInvertedD(r, 1) * subMatrixB(c, 1) + subMatrixInvertedD(r, 2) * subMatrixB(c, 2); // we interpret subMatrixB as transposed here
							}
						}

						for (size_t t = i; t < matrixA_.size(); ++t) // the lower left triangle only: therefore we start with t = i
						{
							if (nonZeroMatrixB_[t * matrixInvertedD_.size() + n])
							{
								const StaticMatrix3x3& subMatrixB2 = matrixB_[t * matrixInvertedD_.size() + n];

								for (unsigned int r = 0u; r < 3u; ++r)
								{
									for (unsigned int c = 0u; c < 3u; ++c)
									{
										left(t * 3 + r, i * 3 + c) -= subMatrixB2(r, 0) * intermediate(0, c) + subMatrixB2(r, 1) * intermediate(1, c) + subMatrixB2(r, 2) * intermediate(2, c);
									}
								}
							}
						}
					}
				}
			}

			// we copy the lower triangle from the upper triangle
			for (size_t c = 1; c < left.columns(); ++c)
			{
				for (size_t r = 0; r < c; ++r)
				{
					left(r, c) = left(c, r);
				}
			}

			ocean_assert(left.isSymmetric(Numeric::weakEps()));

			// now we solve da:

			deltas.resize(matrixA_.size() * 3 + matrixInvertedD_.size() * 3, 1);
			if (!left.solve<Matrix::MP_SYMMETRIC>(resultVector.data(), deltas.data()))
			{
				return false;
			}

			// now we solve db:

			for (size_t n = 0; n < matrixInvertedD_.size(); ++n)
			{
				Vector3 intermediateError(0, 0, 0);

				for (size_t i = 0; i < matrixA_.size(); ++i)
				{
					if (nonZeroMatrixB_[i * matrixInvertedD_.size() + n])
					{
						const StaticMatrix3x3& subMatrixB = matrixB_[i * matrixInvertedD_.size() + n];

						for (size_t t = 0; t < 3; ++t)
						{
							intermediateError[0] += subMatrixB(t, 0) * deltas.data()[i * 3 + t];
							intermediateError[1] += subMatrixB(t, 1) * deltas.data()[i * 3 + t];
							intermediateError[2] += subMatrixB(t, 2) * deltas.data()[i * 3 + t];
						}
					}
				}

				const SquareMatrix3& subMatrixInvertedD = matrixInvertedD_[n];

				intermediateError = Vector3(jacobianErrorVector_.data() + 3 * matrixA_.size() + 3 * n) - intermediateError;

				((Vector3*)(deltas.data() + 3 * matrixA_.size()))[n] = subMatrixInvertedD * intermediateError;
			}

#ifdef OCEAN_INTENSIVE_DEBUG
	#ifndef OCEAN_DEBUG
			#error Invalid debug state!
	#endif

			if (std::is_same<Scalar, double>::value)
			{
				ocean_assert(debugHessian_.rows() == debugHessian_.columns());
				SparseMatrix debugCopyHessian(debugHessian_);

				if (lambda > 0)
				{
					for (size_t n = 0; n < debugCopyHessian.rows(); ++n)
					{
						ocean_assert(!debugCopyHessian.isZero(n, n));
						debugCopyHessian(n, n) *= Scalar(1) + lambda;
					}
				}

				for (size_t n = 0; n < flippedCameras_T_world_.size(); ++n)
				{
					for (size_t r = 0; r < 3; ++r)
					{
						for (size_t c = 0; c < 3; ++c)
						{
							const Scalar value0 = debugCopyHessian(n * 3 + r, n * 3 + c);
							const Scalar value1 = matrixA_[n](r, c);
							ocean_assert(Numeric::isEqual<-6>(value0, value1, Numeric::weakEps()));
						}
					}
				}

				const size_t sizeA = flippedCameras_T_world_.size() * 3;
				const size_t sizeB = correspondenceGroups_.groups() * 3;

				SparseMatrix D(debugCopyHessian.submatrix(sizeA, sizeA, sizeB, sizeB));

				if (!D.invertBlockDiagonal3())
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				const SparseMatrix A(debugCopyHessian.submatrix(0, 0, sizeA, sizeA));
				const SparseMatrix B(debugCopyHessian.submatrix(0, sizeA, sizeA, sizeB));
				const SparseMatrix C(debugCopyHessian.submatrix(sizeA, 0, sizeB, sizeA));

				const Matrix ea(sizeA, 1, debugJacobianError_.data());
				const Matrix eb(sizeB, 1, debugJacobianError_.data() + sizeA);

				const SparseMatrix debugLeft = A - B * (D * C);
				const Matrix debugRight = ea - B * (D * eb);

				ocean_assert(debugLeft.rows() == left.rows() && debugLeft.columns() == left.columns());

				for (size_t r = 0; r < debugLeft.rows(); ++r)
				{
					for (size_t c = 0; c < debugLeft.columns(); ++c)
					{
						const Scalar value0 = debugLeft(r, c);
						const Scalar value1 = left(r, c);
						ocean_assert(Numeric::isEqual<-6>(value0, value1, Numeric::weakEps()));
					}
				}

				for (size_t r = 0; r < flippedCameras_T_world_.size() * 3; ++r)
				{
					const Scalar value0 = debugRight(r, 0);
					const Scalar value1 = resultVector[r];
					ocean_assert(Numeric::isEqual<-6>(value0, value1, Numeric::weakEps()));
				}

				Matrix da;
				if (!debugLeft.solve(debugRight, da))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				for (size_t r = 0; r < flippedCameras_T_world_.size() * 3; ++r)
				{
					const Scalar value0 = da(r, 0);
					const Scalar value1 = deltas(r, 0);
					ocean_assert(Numeric::isEqual<-6>(value0, value1, Numeric::weakEps() * 100));
				}

				const Matrix db = D * (eb - C * da);

				for (size_t r = 0; r < 3 * correspondenceGroups_.groups(); ++r)
				{
					const Scalar value0 = db(r, 0);
					const Scalar value1 = deltas.data()[flippedCameras_T_world_.size() * 3 + r];
					ocean_assert(Numeric::isEqual<-6>(value0, value1, Numeric::weakEps() * 100));
				}
			}
#endif // OCEAN_INTENSIVE_DEBUG

			return true;
		}

	protected:

		/// The camera profiles defining the projection for each individual camera frame.
		const ConstIndexedAccessor<const AnyCamera*>& cameras_;

		/// The accessor for all camera poses.
		NonconstTemplateArrayAccessor<HomogenousMatrix4>& flippedCameras_T_world_;

		/// The  candidate cameras poses.
		HomogenousMatrices4 candidateFlippedCameras_T_world_;

		/// The translations between world and translations.
		Vectors3 translations_T_world_;

		/// The locations of the 3D object points of the most recent succeeded optimization step.
		NonconstTemplateArrayAccessor<Vector3>& objectPoints_;

		/// The locations of the candidate object points.
		Vectors3 objectPointCandidates_;

		/// The accessor for the individual groups of correspondences between pose indices and image point location, one group for each object point.
		const ObjectPointGroupsAccessor& correspondenceGroups_;

		/// True, if all 3D object points (before and after optimization) must lie in front of both cameras.
		const bool onlyFrontObjectPoints_;

		/// The upper left 3x3 sub-matrix of the Hessian matrix.
		StaticMatrices3x3 matrixA_;

		/// The upper right 3x3 sub-matrices of the Hessian matrix.
		StaticMatrices3x3 matrixB_;

		/// The individual states for all sub-matrices of b defining whether the matrix is zero or not.
		std::vector<uint8_t> nonZeroMatrixB_;

		/// The lower right 3x3 sub-matrices of the Hessian matrix.
		SquareMatrices3 matrixD_;

		/// The inverted 3x3 sub-matrices of the Hessian matrix.
		SquareMatrices3 matrixInvertedD_;

		/// The error vector multiplied by the Jacobian matrix.
		Scalars jacobianErrorVector_;

		/// The copy of the diagonal of matrix A.
		Scalars diagonalMatrixA_;

		/// The copy of the diagonal of matrix D.
		Scalars diagonalMatrixD_;

		/// Intermediate square error values.
		Scalars intermediateSqrErrors_;

		/// Intermediate error values.
		Vectors2 intermediateErrors_;

		/// Intermediate weight values.
		Scalars intermediateWeights_;

		/// The number of rows of the entire Jacobian.
		size_t jacobianRows_;

#ifdef OCEAN_DEBUG
		/// The Jacobian matrix.
		SparseMatrix debugJacobian_;

		/// The Hessian matrix.
		SparseMatrix debugHessian_;

		/// The error vector multiplied by the Jacobian matrix.
		Matrix debugJacobianError_;
#endif
};

bool NonLinearOptimizationObjectPoint::optimizeObjectPointsAndOrientationalPoses(const ConstIndexedAccessor<const AnyCamera*>& cameras, const ConstIndexedAccessor<HomogenousMatrix4>& world_T_cameras, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, NonconstIndexedAccessor<HomogenousMatrix4>* world_T_optimizedCameras, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator, const Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
	ocean_assert(world_T_optimizedCameras == nullptr || world_T_cameras.size() == world_T_optimizedCameras->size());
	ocean_assert(optimizedObjectPoints == nullptr || objectPoints.size() == optimizedObjectPoints->size());

	ocean_assert(objectPoints.size() == correspondenceGroups.groups());
	if (objectPoints.size() != correspondenceGroups.groups())
	{
		return false;
	}

	const ScopedConstMemoryAccessor<HomogenousMatrix4> scoped_world_T_cameras(world_T_cameras);

	HomogenousMatrices4 flippedCamera_T_world(scoped_world_T_cameras.size());
	for (size_t n = 0; n < flippedCamera_T_world.size(); ++n)
	{
		flippedCamera_T_world[n] = AnyCamera::standard2InvertedFlipped(scoped_world_T_cameras[n]);
	}

	HomogenousMatrices4 flippedOptimizedCameras_T_world;
	NonconstArrayAccessor<HomogenousMatrix4> accessor_flippedOptimizedCameras_T_world(flippedOptimizedCameras_T_world, world_T_optimizedCameras != nullptr ? world_T_optimizedCameras->size() : 0);

	if (!optimizeObjectPointsAndOrientationalPosesIF(cameras, ConstArrayAccessor<HomogenousMatrix4>(flippedCamera_T_world), objectPoints, correspondenceGroups, accessor_flippedOptimizedCameras_T_world.pointer(), optimizedObjectPoints, iterations, estimator, lambda, lambdaFactor, onlyFrontObjectPoints, initialError, finalError, intermediateErrors))
	{
		return false;
	}

	if (world_T_optimizedCameras != nullptr)
	{
		for (size_t n = 0; n < flippedOptimizedCameras_T_world.size(); ++n)
		{
			(*world_T_optimizedCameras)[n] = AnyCamera::invertedFlipped2Standard(flippedOptimizedCameras_T_world[n]);

			ocean_assert(world_T_cameras[n].translation().isEqual((*world_T_optimizedCameras)[n].translation(), Numeric::weakEps()));
		}
	}

	return true;
}

bool NonLinearOptimizationObjectPoint::optimizeObjectPointsAndOrientationalPosesIF(const ConstIndexedAccessor<const AnyCamera*>& cameras, const ConstIndexedAccessor<HomogenousMatrix4>& flippedCameras_T_world, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, NonconstIndexedAccessor<HomogenousMatrix4>* flippedOptimizedCameras_T_world, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator, const Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
	ocean_assert(flippedOptimizedCameras_T_world == nullptr || flippedCameras_T_world.size() == flippedOptimizedCameras_T_world->size());
	ocean_assert(optimizedObjectPoints == nullptr || objectPoints.size() == optimizedObjectPoints->size());

	ocean_assert(objectPoints.size() == correspondenceGroups.groups());
	if (objectPoints.size() != correspondenceGroups.groups())
	{
		return false;
	}

	// we need enough buffer for the optimized poses, we take them from the provided parameter or create them temporary in this scope
	ScopedNonconstMemoryAccessor<HomogenousMatrix4> scoped_flippedOptimizedCameras_T_world(flippedOptimizedCameras_T_world, flippedCameras_T_world.size());
	ocean_assert(scoped_flippedOptimizedCameras_T_world.size() == flippedCameras_T_world.size());

	const ScopedConstMemoryAccessor<HomogenousMatrix4> scopedAccessor_flippedCameras_T_world(flippedCameras_T_world);
	for (size_t n = 0; n < flippedCameras_T_world.size(); ++n)
	{
		scoped_flippedOptimizedCameras_T_world[n] = scopedAccessor_flippedCameras_T_world[n];
	}

	// we need enough buffer for the optimized object points, we take them from the provided parameter or create them temporary in this scope
	ScopedNonconstMemoryAccessor<Vector3> scopedOptimizedObjectPoints(optimizedObjectPoints, objectPoints.size());
	ocean_assert(scopedOptimizedObjectPoints.size() == objectPoints.size());

	const ScopedConstMemoryAccessor<Vector3> scopedObjectPoints(objectPoints);
	for (size_t n = 0; n < objectPoints.size(); ++n)
	{
		scopedOptimizedObjectPoints[n] = scopedObjectPoints[n];
	}

#ifdef OCEAN_DEBUG
	for (size_t n = 0; n < correspondenceGroups.groups(); ++n)
	{
		const Vector3& objectPoint = scopedOptimizedObjectPoints[n];

		for (size_t i = 0; i < correspondenceGroups.groupElements(n); ++i)
		{
			Index32 poseIndex;
			Vector2 imagePoint;
			correspondenceGroups.element(n, i, poseIndex, imagePoint);

			ocean_assert(flippedCameras_T_world.canAccess(poseIndex));
			const HomogenousMatrix4& flippedOptimizedCamera_T_world = scoped_flippedOptimizedCameras_T_world[poseIndex];

			ocean_assert(!onlyFrontObjectPoints || AnyCamera::isObjectPointInFrontIF(flippedOptimizedCamera_T_world, objectPoint));

			const Vector2 projectedObjectPoint(cameras[poseIndex]->projectToImageIF(flippedOptimizedCamera_T_world, objectPoint));
			const Scalar sqrDistance = projectedObjectPoint.sqrDistance(imagePoint);
			OCEAN_SUPPRESS_UNUSED_WARNING(sqrDistance);

			// we take a very large error due to outliers, however normally the error should be less then 5 * 5
			// however, the following test does not make sense if we e.g., try to adjust the camera poses and object points to a different camera profile, so we disable the assert
			//ocean_assert(sqrDistance < 20 * 20);
		}
	}
#endif

	NonconstTemplateArrayAccessor<HomogenousMatrix4> accessor_flippedOptimizedCameras_T_world(scoped_flippedOptimizedCameras_T_world.data(), scoped_flippedOptimizedCameras_T_world.size());
	NonconstTemplateArrayAccessor<Vector3> objectPointsAccessor(scopedOptimizedObjectPoints.data(), scopedOptimizedObjectPoints.size());

	switch (estimator)
	{
		case Estimator::ET_LINEAR:
		{
			ObjectPointsOrientationalPosesProvider<Estimator::ET_LINEAR> provider(cameras, accessor_flippedOptimizedCameras_T_world, objectPointsAccessor, correspondenceGroups, onlyFrontObjectPoints);
			return advancedSparseOptimization(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors);
		}

		case Estimator::ET_HUBER:
		{
			ObjectPointsOrientationalPosesProvider<Estimator::ET_HUBER> provider(cameras, accessor_flippedOptimizedCameras_T_world, objectPointsAccessor, correspondenceGroups, onlyFrontObjectPoints);
			return advancedSparseOptimization(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors);
		}

		case Estimator::ET_TUKEY:
		{
			ObjectPointsOrientationalPosesProvider<Estimator::ET_TUKEY> provider(cameras, accessor_flippedOptimizedCameras_T_world, objectPointsAccessor, correspondenceGroups, onlyFrontObjectPoints);
			return advancedSparseOptimization(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors);
		}

		case Estimator::ET_CAUCHY:
		{
			ObjectPointsOrientationalPosesProvider<Estimator::ET_CAUCHY> provider(cameras, accessor_flippedOptimizedCameras_T_world, objectPointsAccessor, correspondenceGroups, onlyFrontObjectPoints);
			return advancedSparseOptimization(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors);
		}

		case Estimator::ET_SQUARE:
		{
			ObjectPointsOrientationalPosesProvider<Estimator::ET_SQUARE> provider(cameras, accessor_flippedOptimizedCameras_T_world, objectPointsAccessor, correspondenceGroups, onlyFrontObjectPoints);
			return advancedSparseOptimization(provider, iterations, lambda, lambdaFactor, initialError, finalError, intermediateErrors);
		}

		case Estimator::ET_INVALID:
		{
			break;
		}
	}

	ocean_assert(false && "This should never happen!");
	return false;
}

/**
 * This class implements an optimization provider allowing to optimize 6-DOF camera poses and 3-DOF object points concurrently.
 */
class NonLinearOptimizationObjectPoint::SlowObjectPointsPosesProvider : public NonLinearOptimization::OptimizationProvider
{
	public:

		/**
		 * Creates a new provider object.
		 * @param pinholeCamera The camera profile defining the projection, must be valid
		 * @param flippedCameras_T_world The inverted and flipped camera poses which will be optimized, with default flipped camera pointing towards the positive z-space with y-axis downwards, at least two
		 * @param objectPointAccessor The 3D object point locations which will be optimized
		 * @param correspondenceGroups The accessor for the individual groups of correspondences between pose indices and image point location, one group for each object point
		 * @param onlyFrontObjectPoints True, to ensure that all 3D object point locations will lie in front of both cameras
		 */
		inline SlowObjectPointsPosesProvider(const AnyCamera& camera, NonconstTemplateArrayAccessor<HomogenousMatrix4>& flippedCameras_T_world, NonconstTemplateArrayAccessor<Vector3>& objectPointAccessor, const NonLinearOptimization::ObjectPointGroupsAccessor& correspondenceGroups, const bool onlyFrontObjectPoints) :
			camera_(camera),
			flippedCameras_T_world_(flippedCameras_T_world),
			objectPointAccessor_(objectPointAccessor),
			candidateFlippedCameras_T_world_(Accessor::accessor2elements(flippedCameras_T_world)),
			candidateObjectPoints_(Accessor::accessor2elements(objectPointAccessor)),
			correspondenceGroups_(correspondenceGroups),
			onlyFrontObjectPoints_(onlyFrontObjectPoints),
			jacobianRows_(0),
			jacobianColumns_(0)
		{
			ocean_assert(objectPointAccessor_.size() == correspondenceGroups_.groups());

			// each observation (combination of pose and object point) creates two rows
			for (size_t n = 0; n < correspondenceGroups_.groups(); ++n)
			{
				jacobianRows_ += correspondenceGroups_.groupElements(n) * 2;
			}

			// each dynamic pose creates 6 columns and each dynamic object point creates 3 columns
			jacobianColumns_ = flippedCameras_T_world_.size() * 6 + objectPointAccessor_.size() * 3;
		}

		/**
		 * Returns whether this provider comes with an own equation solver.
		 * @return True, as this provider has an own solver
		 */
		inline bool hasSolver() const
		{
			return true;
		}

		/**
		 * Solves the equation JTJ * deltas = jErrors
		 * @param JTJ The JTJ matrix
		 * @param jErrors The jErrors vector
		 * @param deltas The deltas vector
		 * @return True, if succeeded
		 */
		inline bool solve(const SparseMatrix& JTJ, const Matrix& jErrors, Matrix& deltas) const
		{
			ocean_assert(JTJ.rows() == JTJ.columns());
			ocean_assert(JTJ.rows() == jErrors.rows());

			ocean_assert(jErrors.columns() == 1);

			/**
			 * here we apply the Schur complement to improve the solve performance:
			 *
			 *  JTJ  * deltas = jErrors
			 * | A B |   | da |   | ea |
			 * | C D | * | db | = | eb |
			 *
			 * (A - B D^-1 C ) * da = ea - B D^-1 * eb  ->  (solve da)
			 * db = D^-1 (eb - C * da)
			 *
			 * or:
			 * (D - C A^-1 B) * db = eb - C A^-1 * ea -> (solve db)
			 * da = A^-1 (ea - B * db)
			 */

			// solving da:

			const size_t sizeA = flippedCameras_T_world_.size() * 6;
			const size_t sizeB = JTJ.rows() - sizeA;
			ocean_assert(sizeB % 3 == 0);

			if (sizeA < sizeB)
			{
				SparseMatrix D(JTJ.submatrix(sizeA, sizeA, sizeB, sizeB));

				if (!D.invertBlockDiagonal3())
				{
					return false;
				}

				const SparseMatrix A(JTJ.submatrix(0, 0, sizeA, sizeA));
				const SparseMatrix B(JTJ.submatrix(0, sizeA, sizeA, sizeB));
				const SparseMatrix C(JTJ.submatrix(sizeA, 0, sizeB, sizeA));

				const Matrix ea(sizeA, 1, jErrors.data());
				const Matrix eb(sizeB, 1, jErrors.data() + sizeA);

				Matrix da;
				if (!(A - B * (D * C)).solve(ea - B * (D * eb), da))
				{
					return false;
				}

				const Matrix db = D * (eb - C * da);

				deltas.resize(jErrors.rows(), 1);

				memcpy(deltas.data(), da.data(), sizeA * sizeof(Scalar));
				memcpy(deltas.data() + sizeA, db.data(), sizeB * sizeof(Scalar));
			}
			else
			{
				SparseMatrix A(JTJ.submatrix(0, 0, sizeA, sizeA));

				if (!A.invertBlockDiagonal(6))
				{
					return false;
				}

				const SparseMatrix D(JTJ.submatrix(sizeA, sizeA, sizeB, sizeB));
				const SparseMatrix B(JTJ.submatrix(0, sizeA, sizeA, sizeB));
				const SparseMatrix C(JTJ.submatrix(sizeA, 0, sizeB, sizeA));

				const Matrix ea(sizeA, 1, jErrors.data());
				const Matrix eb(sizeB, 1, jErrors.data() + sizeA);

				Matrix db;
				if (!(D - C * (A * B)).solve(eb - C * (A * ea), db))
				{
					return false;
				}

				const Matrix da = A * (ea - B * db);

				deltas.resize(jErrors.rows(), 1);

				memcpy(deltas.data(), da.data(), sizeA * sizeof(Scalar));
				memcpy(deltas.data() + sizeA, db.data(), sizeB * sizeof(Scalar));
			}

			return true;
		}

		/**
		 * Determines the Jacobian matrix of this providers.
		 * @param jacobian The resulting Jacobian matrix
		 */
		void determineJacobian(SparseMatrix& jacobian) const
		{
			/**
			 * Jacobian matrix of the projection function evaluated at current pose, dimension (2 * n * m) x (6 * m + 3 * n)
			 * Image point: ip
			 * [ df1(p1) / dt1,        0     ,       0      , df1(p1) / dp1,        0     ,        0     , .............,        0      ]
			 * [      0       , df2(p1) / dt2,       0      , df2(p1) / dp1,        0     ,        0     , .............,        0      ]
			 * [      0       ,        0     , df3(p1) / dt3, df3(p1) / dp1,        0     ,        0     , .............,        0      ]
			 *
			 * [ df1(p2) / dt1,        0     ,        0     ,        0     , df1(p2) / dp2,        0     , .............,        0      ]
			 * [      0       ,        0     , df3(p2) / dt3,        0     , df3(p2) / dp2,        0     , .............,        0      ]
			 *
			 * [      0       , df2(p3) / dt2,        0     ,        0     ,        0     , df2(p3) / dp3, .............,        0      ]
			 * [      0       ,        0     , df3(p3) / dt3,        0     ,        0     , df3(p3) / dp3, .............,        0      ]
			 *
			 * [ ............., ............., ............., ............., ............., ............., .............,        0      ]
			 *
			 * [ df1(pn) / dt1,        0     ,        0     ,        0     ,        0     , ............., ............., df1(pn) / dpn ]
			 * [      0       , df2(pn) / dt2,        0     ,        0     ,        0     , ............., ............., df2(pn) / dpn ]
			 *
			 */

			SparseMatrix::Entries jacobianEntries;
			jacobianEntries.reserve(jacobianRows_ * 9); // in each row are at most 9 non-zero elements

			Scalar pointJacobianBuffer[6];
			Scalar poseJacobianBuffer[12];

			SquareMatrices3 rotationRodriguesDerivatives(flippedCameras_T_world_.size() * 3);
			for (size_t n = 0; n < flippedCameras_T_world_.size(); ++n)
			{
				const Pose pose(flippedCameras_T_world_[n]);
				Jacobian::calculateRotationRodriguesDerivative(ExponentialMap(pose.rx(), pose.ry(), pose.rz()), rotationRodriguesDerivatives[n * 3 + 0], rotationRodriguesDerivatives[n * 3 + 1], rotationRodriguesDerivatives[n * 3 + 2]);
			}

			size_t row = 0u;
			const size_t pointColumnStart = flippedCameras_T_world_.size() * 6;

			for (size_t o = 0; o < correspondenceGroups_.groups(); ++o)
			{
				const ObjectPoint& objectPoint = objectPointAccessor_[o];

				for (size_t i = 0; i < correspondenceGroups_.groupElements(o); ++i)
				{
					const Index32 poseId = correspondenceGroups_.firstElement(o, i);
					ocean_assert(poseId < flippedCameras_T_world_.size());

					Jacobian::calculatePoseJacobianRodrigues2x6IF(camera_, flippedCameras_T_world_[poseId], objectPoint, rotationRodriguesDerivatives[3u * poseId + 0u], rotationRodriguesDerivatives[3u * poseId + 1u], rotationRodriguesDerivatives[3u * poseId + 2u], poseJacobianBuffer, poseJacobianBuffer + 6);

					for (size_t e = 0; e < 6; ++e)
					{
						// .insert(row + 0, poseId * 6u, poseJacobianBuffer, 6u);
						jacobianEntries.push_back(SparseMatrix::Entry(row + 0, poseId * 6 + e, poseJacobianBuffer[0 + e]));

						// .insert(row + 1, poseId * 6u, poseJacobianBuffer + 6, 6u);
						jacobianEntries.push_back(SparseMatrix::Entry(row + 1, poseId * 6 + e, poseJacobianBuffer[6 + e]));
					}

					Jacobian::calculatePointJacobian2x3IF(camera_, flippedCameras_T_world_[poseId], objectPoint, pointJacobianBuffer, pointJacobianBuffer + 3);

					for (size_t e = 0; e < 3; ++e)
					{
						// .insert(row + 0, pointColumnStart + o * 3u, pointJacobianBuffer, 3u);
						jacobianEntries.push_back(SparseMatrix::Entry(row + 0, pointColumnStart + o * 3 + e, pointJacobianBuffer[0 + e]));

						// .insert(row + 1, pointColumnStart + o * 3u, pointJacobianBuffer + 3, 3u);
						jacobianEntries.push_back(SparseMatrix::Entry(row + 1, pointColumnStart + o * 3 + e, pointJacobianBuffer[3 + e]));
					}

					row += 2;
				}
			}

			jacobian = SparseMatrix(jacobianRows_, jacobianColumns_, jacobianEntries);
			ocean_assert(SparseMatrix::Entry::hasOneEntry(jacobian.rows(), jacobian.columns(), jacobianEntries));
			ocean_assert(row == jacobian.rows());
		}

		void applyCorrection(const Matrix& deltas)
		{
			for (unsigned int n = 0u; n < flippedCameras_T_world_.size(); ++n)
			{
				const Pose oldPose(flippedCameras_T_world_[n]);

				// p_i+1 = p_i + delta_i
				// p_i+1 = p_i - (-delta_i)
				const Pose deltaPose(deltas(6u * n + 3u), deltas(6u * n + 4u), deltas(6u * n + 5u), deltas(6u * n + 0u), deltas(6u * n + 1u), deltas(6u * n + 2u));

				const Pose newPose(oldPose - deltaPose);
				candidateFlippedCameras_T_world_[n] = newPose.transformation();
			}

			const unsigned int offset = (unsigned int)(flippedCameras_T_world_.size()) * 6u;

			for (unsigned int n = 0u; n < objectPointAccessor_.size(); ++n)
			{
				const Vector3 deltaObjectPoint(deltas(offset + 3u * n + 0u), deltas(offset + 3u * n + 1u), deltas(offset + 3u * n + 2u));

				const Vector3 newObjectPoint(objectPointAccessor_[n] - deltaObjectPoint);
				candidateObjectPoints_[n] = newObjectPoint;
			}
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
			if (onlyFrontObjectPoints_ && !checkFrontObjectPoint())
			{
				return Numeric::maxValue();
			}

			// set the correct size of the resulting error vector
			weightedErrorVector.resize(jacobianRows_, 1u);

			Vector2* const weightedErrors = (Vector2*)(weightedErrorVector.data());
			const SquareMatrix2* transposedInvertedCovariances = invertedCovariances ? (const SquareMatrix2*)(invertedCovariances->data()) : nullptr;

			unsigned int row = 0u;
			Scalar sqrError = 0;

			Scalars sqrErrors;
			if constexpr (!Estimator::isStandardEstimator<tEstimator>())
			{
				sqrErrors.reserve(jacobianRows_ / 2u);
			}

			for (size_t o = 0; o < correspondenceGroups_.groups(); ++o)
			{
				const ObjectPoint& objectPoint = candidateObjectPoints_[o];

				for (size_t p = 0; p < correspondenceGroups_.groupElements(o); ++p)
				{
					Index32 poseId;
					Vector2 imagePoint;
					correspondenceGroups_.element(o, p, poseId, imagePoint);

					const Vector2 error = Error::determinePoseErrorIF(candidateFlippedCameras_T_world_[poseId], camera_, objectPoint, imagePoint);

					weightedErrors[row] = error;

					if constexpr (Estimator::isStandardEstimator<tEstimator>())
					{
						if (transposedInvertedCovariances != nullptr)
						{
							sqrError += (transposedInvertedCovariances[row].transposed() * error).sqr();
						}
						else
						{
							sqrError += error.sqr();
						}
					}
					else
					{
						ocean_assert(!Estimator::isStandardEstimator<tEstimator>());
						sqrErrors.emplace_back(error.sqr());
					}

					row++;
				}
			}

			ocean_assert(row * 2u == weightedErrorVector.rows());

			// check whether the standard estimator is used
			if constexpr (Estimator::isStandardEstimator<tEstimator>())
			{
				// the weight vector should be and should stay invalid
				ocean_assert(!weightVector);

				return sqrError / Scalar(jacobianRows_ / 2u);
			}
			else
			{
				// now we need the weight vector
				weightVector.resize(jacobianRows_, 1u);

				ocean_assert(sqrErrors.size() == jacobianRows_ / 2u);

				// the model size is the number of columns in the jacobian row as so many parameters will be modified
				return sqrErrors2robustErrors2<tEstimator>(sqrErrors, jacobianColumns_, weightedErrors, (Vector2*)(weightVector.data()), transposedInvertedCovariances);
			}
		}

		/**
		 * Accepts the current pose candidate as better model.
		 */
		inline void acceptCorrection()
		{
			ocean_assert(objectPointAccessor_.size() == candidateObjectPoints_.size());
			for (size_t n = 0; n < objectPointAccessor_.size(); ++n)
			{
				objectPointAccessor_[n] = candidateObjectPoints_[n];
			}

			ocean_assert(flippedCameras_T_world_.size() == candidateFlippedCameras_T_world_.size());
			for (size_t n = 0; n < flippedCameras_T_world_.size(); ++n)
			{
				flippedCameras_T_world_[n] = candidateFlippedCameras_T_world_[n];
			}
		}

	protected:

		/**
		 * Checks whether all object points lie in front of the cameras.
		 * @return True, if so
		 */
		bool checkFrontObjectPoint() const
		{
			// the visibility check is applied without distinguishing between static or dynamic poses or object points

			for (unsigned int n = 0u; n < correspondenceGroups_.groups(); ++n)
			{
				const Vector3& objectPoint = candidateObjectPoints_[n];

				for (unsigned int i = 0u; i < correspondenceGroups_.groupElements(n); ++i)
				{
					const Index32 poseId = correspondenceGroups_.firstElement(n, i);

					if (!PinholeCamera::isObjectPointInFrontIF(candidateFlippedCameras_T_world_[poseId], objectPoint))
					{
						return false;
					}
				}
			}

			return true;
		}

	protected:

		/// The camera profile that is applied for optimization.
		const AnyCamera& camera_;

		/// The accessor for all camera poses.
		NonconstTemplateArrayAccessor<HomogenousMatrix4>& flippedCameras_T_world_;

		/// The accessor for all object points.
		NonconstTemplateArrayAccessor<Vector3>& objectPointAccessor_;

		/// The candidates of new camera poses.
		HomogenousMatrices4  candidateFlippedCameras_T_world_;

		/// The candidates of new object points.
		ObjectPoints candidateObjectPoints_;

		/// The groups of correspondences between pose indices and image points, one group for each object point
		const NonLinearOptimization::ObjectPointGroupsAccessor& correspondenceGroups_;

		/// True, forces the object point to stay in front of the camera.s
		const bool onlyFrontObjectPoints_;

		/// The number of jacobian rows.
		size_t jacobianRows_;

		/// The number of jacobian columns.
		size_t jacobianColumns_;
};

bool NonLinearOptimizationObjectPoint::slowOptimizeObjectPointsAndPoses(const AnyCamera& camera, const ConstIndexedAccessor<HomogenousMatrix4>& world_T_cameras, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, NonconstIndexedAccessor<HomogenousMatrix4>* flippedOptimizedCameras_T_world, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator, const Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
	ocean_assert(flippedOptimizedCameras_T_world == nullptr || world_T_cameras.size() == flippedOptimizedCameras_T_world->size());
	ocean_assert(optimizedObjectPoints == nullptr || objectPoints.size() == optimizedObjectPoints->size());

	ocean_assert(objectPoints.size() == correspondenceGroups.groups());
	if (objectPoints.size() != correspondenceGroups.groups())
	{
		return false;
	}

	const ScopedConstMemoryAccessor<HomogenousMatrix4> scopedPoses(world_T_cameras);

	HomogenousMatrices4 flippedCameras_T_world(scopedPoses.size());
	for (size_t n = 0; n < flippedCameras_T_world.size(); ++n)
	{
		flippedCameras_T_world[n] = PinholeCamera::standard2InvertedFlipped(scopedPoses[n]);
	}

	HomogenousMatrices4 optimizedPosesIF;
	NonconstArrayAccessor<HomogenousMatrix4> optimizedPosesAccessorIF(optimizedPosesIF, flippedOptimizedCameras_T_world ? world_T_cameras.size() : 0);

	if (!slowOptimizeObjectPointsAndPosesIF(camera, ConstArrayAccessor<HomogenousMatrix4>(flippedCameras_T_world), objectPoints, correspondenceGroups, optimizedPosesAccessorIF.pointer(), optimizedObjectPoints, iterations, estimator, lambda, lambdaFactor, onlyFrontObjectPoints, initialError, finalError, intermediateErrors))
	{
		return false;
	}

	if (flippedOptimizedCameras_T_world != nullptr)
	{
		for (size_t n = 0; n < optimizedPosesIF.size(); ++n)
		{
			(*flippedOptimizedCameras_T_world)[n] = PinholeCamera::invertedFlipped2Standard(optimizedPosesIF[n]);
		}
	}

	return true;
}

bool NonLinearOptimizationObjectPoint::slowOptimizeObjectPointsAndPosesIF(const AnyCamera& camera, const ConstIndexedAccessor<HomogenousMatrix4>& flippedCameras_T_world, const ConstIndexedAccessor<Vector3>& objectPoints, const ObjectPointGroupsAccessor& correspondenceGroups, NonconstIndexedAccessor<HomogenousMatrix4>* flippedOptimizedCameras_T_world, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator, const Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
	ocean_assert(flippedOptimizedCameras_T_world == nullptr || flippedCameras_T_world.size() == flippedOptimizedCameras_T_world->size());
	ocean_assert(optimizedObjectPoints == nullptr || objectPoints.size() == optimizedObjectPoints->size());

	ocean_assert(objectPoints.size() == correspondenceGroups.groups());
	if (objectPoints.size() != correspondenceGroups.groups())
	{
		return false;
	}

	// we need enough buffer for the optimized poses, we take them from the provided parameter or create them temporary in this scope
	ScopedNonconstMemoryAccessor<HomogenousMatrix4> scoped_flippedOptimizedCameras_T_world(flippedOptimizedCameras_T_world, flippedCameras_T_world.size());
	ocean_assert(scoped_flippedOptimizedCameras_T_world.size() == flippedCameras_T_world.size());

	const ScopedConstMemoryAccessor<HomogenousMatrix4> scopedPosesIF(flippedCameras_T_world);
	for (size_t n = 0; n < flippedCameras_T_world.size(); ++n)
	{
		scoped_flippedOptimizedCameras_T_world[n] = scopedPosesIF[n];
	}

	// we need enough buffer for the optimized object points, we take them from the provided parameter or create them temporary in this scope
	ScopedNonconstMemoryAccessor<Vector3> scopedOptimizedObjectPoints(optimizedObjectPoints, objectPoints.size());
	ocean_assert(scopedOptimizedObjectPoints.size() == objectPoints.size());

	const ScopedConstMemoryAccessor<Vector3> scopedObjectPoints(objectPoints);
	for (size_t n = 0; n < objectPoints.size(); ++n)
	{
		scopedOptimizedObjectPoints[n] = scopedObjectPoints[n];
	}

#ifdef OCEAN_DEBUG
	for (size_t n = 0; n < correspondenceGroups.groups(); ++n)
	{
		const Vector3& objectPoint = scopedObjectPoints[n];

		for (size_t i = 0; i < correspondenceGroups.groupElements(n); ++i)
		{
			Index32 poseIndex;
			Vector2 imagePoint;
			correspondenceGroups.element(n, i, poseIndex, imagePoint);

			ocean_assert(flippedCameras_T_world.canAccess(poseIndex));
			const HomogenousMatrix4& flippedCamera_T_world = scoped_flippedOptimizedCameras_T_world[poseIndex];

			const Vector2 projectedObjectPoint(camera.projectToImageIF(flippedCamera_T_world, objectPoint));
			const Scalar sqrDistance = projectedObjectPoint.sqrDistance(imagePoint);
			OCEAN_SUPPRESS_UNUSED_WARNING(sqrDistance);

			// we take a very large error due to outliers, however normally the error should be less then 5 * 5
			// however, the following test does not make sense if we e.g., try to adjust the camera poses and object points to a different camera profile, so we disable the assert
			//ocean_assert(sqrDistance < 20 * 20);
		}
	}
#endif

	NonconstTemplateArrayAccessor<HomogenousMatrix4> accessor_flippedOptimizedCameras_T_world(scoped_flippedOptimizedCameras_T_world.data(), scoped_flippedOptimizedCameras_T_world.size());
	NonconstTemplateArrayAccessor<Vector3> objectPointsAccessor(scopedOptimizedObjectPoints.data(), scopedOptimizedObjectPoints.size());

	SlowObjectPointsPosesProvider provider(camera, accessor_flippedOptimizedCameras_T_world, objectPointsAccessor, correspondenceGroups, onlyFrontObjectPoints);
	if (!sparseOptimization(provider, iterations, estimator, lambda, lambdaFactor, initialError, finalError, nullptr, intermediateErrors))
	{
		return false;
	}

	return true;
}

void NonLinearOptimizationObjectPoint::optimizeObjectPointsForFixedPosesIFSubset(const PinholeCamera* camera, const ConstIndexedAccessor<HomogenousMatrix4>* invertedFlippedPoses, const ConstIndexedAccessor<Vector3>* objectPoints, const ObjectPointGroupsAccessor* correspondenceGroups, const bool distortImagePoints, NonconstIndexedAccessor<Vector3>* optimizedObjectPoints, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, const bool onlyFrontObjectPoints, const unsigned int firstObjectPoint, const unsigned int numberObjectPoints)
{
	ocean_assert(camera && invertedFlippedPoses && objectPoints && correspondenceGroups && optimizedObjectPoints);

	ocean_assert(!invertedFlippedPoses->isEmpty() && !objectPoints->isEmpty());
	ocean_assert(objectPoints->size() == correspondenceGroups->groups());

	ocean_assert(lambda >= 0);
	ocean_assert(lambdaFactor >= 1);

	ocean_assert(firstObjectPoint + numberObjectPoints <= (unsigned int)objectPoints->size());

	unsigned int poseIndex;
	Vector2 imagePoint;

	Indices32 poseIndices;
	ImagePoints imagePoints;

	const AnyCameraPinhole anyCamera = AnyCameraPinhole(PinholeCamera(*camera, distortImagePoints));

	for (unsigned int n = firstObjectPoint; n < firstObjectPoint + numberObjectPoints; ++n)
	{
		poseIndices.clear();
		imagePoints.clear();

		const size_t elements = correspondenceGroups->groupElements(n);

		poseIndices.reserve(elements);
		imagePoints.reserve(elements);

		for (size_t i = 0; i < elements; ++i)
		{
			correspondenceGroups->element(n, i, poseIndex, imagePoint);

			poseIndices.push_back(poseIndex);
			imagePoints.push_back(imagePoint);
		}

		ObjectPoint optimizedObjectPoint;
		if (optimizeObjectPointForFixedPosesIF(anyCamera, ConstArrayAccessor<HomogenousMatrix4>(Accessor::accessor2subsetElements(*invertedFlippedPoses, poseIndices)), (*objectPoints)[n], ConstArrayAccessor<ImagePoint>(imagePoints), optimizedObjectPoint, iterations, estimator, lambda, lambdaFactor, onlyFrontObjectPoints))
		{
			(*optimizedObjectPoints)[n] = optimizedObjectPoint;
			continue;
		}

		ocean_assert(false && "Failed to optimize a given object point!");
		(*optimizedObjectPoints)[n] = (*objectPoints)[n];
	}
}

}

}
