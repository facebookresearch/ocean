/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/NonLinearOptimizationHomography.h"
#include "ocean/geometry/Error.h"
#include "ocean/geometry/Homography.h"
#include "ocean/geometry/Jacobian.h"
#include "ocean/geometry/NonLinearUniversalOptimizationDense.h"
#include "ocean/geometry/NonLinearUniversalOptimizationSparse.h"

namespace Ocean
{

namespace Geometry
{

#ifdef OCEAN_USE_SLOWER_IMPLEMENTATION

// we keep the following implementation of a non-linear optimization of a Homography to show the usability of the (dense) universal model

/**
 * This class implements a data object allowing the optimization of a Homography using 9 parameters to represent the Homography.
 */
class NonLinearOptimizationHomography::HomographyData
{
	public:

		/**
		 * Creates a new data object.
		 * @param imagePointsLeft The left image points, must be valid
		 * @param imagePointsRight The right image points, one point for each left point, must be valid
		 */
		HomographyData(const ImagePoint* imagePointsLeft, const ImagePoint* imagePointsRight) :
			imagePointsLeft_(imagePointsLeft),
			imagePointsRight_(imagePointsRight)
		{
			// nothing to do here
		}

		/**
		 * Determines a specific transformed image point.
		 * @param externalModel The model of the homography to be used for the transformation
		 * @param index The index of the image point for which the transformed point will be determined
		 * @param result The resulting transformed image point
		 */
		void value(const StaticBuffer<Scalar, 9>& externalModel, const size_t index, StaticBuffer<Scalar, 2>& result)
		{
			const SquareMatrix3& homography = *(SquareMatrix3*)externalModel.data();
			const Vector2 rightImagePoint(homography * imagePointsLeft_[index]);

			result[0] = rightImagePoint[0];
			result[1] = rightImagePoint[1];
		}

		/**
		 * Determines the error between a specific transformed image point and the corresponding measurement.
		 * @param externalModel The model of the homography to be used for the transformation
		 * @param index The index of the image point for which the error will be determined
		 * @param result The resulting error
		 */
		bool error(const StaticBuffer<Scalar, 9>& externalModel, const size_t index, StaticBuffer<Scalar, 2>& result)
		{
			const SquareMatrix3& homography = *(SquareMatrix3*)externalModel.data();
			const Vector2 rightImagePoint(homography * imagePointsLeft_[index]);

			const Vector2& measurementImagePoint = imagePointsRight_[index];
			const Vector2 error(rightImagePoint - measurementImagePoint);

			result[0] = error[0];
			result[1] = error[1];
			return true;
		}

		/**
		 * Transformed the internal model to the external model.
		 * However, as in our case both models are identical we simply normalize the homography and copy the result.
		 * @param internalModel The internal model of the homography
		 * @param externalModel The resulting external model of the homography
		 */
		void transformModel(StaticBuffer<Scalar, 9>& internalModel, StaticBuffer<Scalar, 9>& externalModel)
		{
			SquareMatrix3& internalHomography = *(SquareMatrix3*)internalModel.data();
			Homography::normalizeHomography(internalHomography);

			externalModel = internalModel;
		}

	protected:

		/// The left image points.
		const ImagePoint* imagePointsLeft_ = nullptr;

		/// The right image points, one for each left point.
		const ImagePoint* imagePointsRight_ = nullptr;
};

bool NonLinearOptimizationHomography::optimizeHomography(const SquareMatrix3& homography, const Vector2* imagePointsLeft, const Vector2* imagePointsRight, const size_t correspondences, const unsigned int modelParameters, SquareMatrix3& optimizedHomography, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, const Matrix* invertedCovariances)
{
	ocean_assert(imagePointsLeft && imagePointsRight);
	ocean_assert(modelParameters == 9u);

	typedef NonLinearUniversalOptimizationDense<9, 2, 9> UniversalOptimization;

	HomographyData data(imagePointsLeft, imagePointsRight);

	UniversalOptimization::Model model(homography.data());
	UniversalOptimization::Model optimizedModel;

	if (!UniversalOptimization::optimizeUniversalModel(model, correspondences, UniversalOptimization::ValueCallback::create(data, &HomographyData::value), UniversalOptimization::ErrorCallback::create(data, &HomographyData::error), UniversalOptimization::ModelTransformationCallback::create(data, &HomographyData::transformModel), UniversalOptimization::ModelAdjustmentCallback(), optimizedModel, iterations, estimator, lambda, lambdaFactor, initialError, finalError))
	{
		return false;
	}

	optimizedHomography = *(SquareMatrix3*)optimizedModel.data();

	return true;
}

#endif

/**
 * This provider allows the optimization of a Homography either using 8 or 9 parameters to represent the Homography.
 */
class NonLinearOptimizationHomography::HomographyOptimizationProvider : public NonLinearOptimization::OptimizationProvider
{
	public:

		/**
		 * Creates a new optimization provider object.
		 * @param imagePointsLeft Definition of a pair holding to sets of corresponding image points.
		 * @param imagePointsRight Definition of a pair holding to sets of corresponding image points.
		 * @param correspondences Number of point correspondences, with range [4, infinity)
		 * @param homographyMatrix The initial homography which will be optimized, must be normalized
		 * @param modelParameters The number of parameters defining the model (of the homography) to be optimize, with range [8, 9]
		 */
		inline HomographyOptimizationProvider(const ImagePoint* imagePointsLeft, const ImagePoint* imagePointsRight, const size_t correspondences, SquareMatrix3& homographyMatrix, const unsigned int modelParameters) :
			imagePointsLeft_(imagePointsLeft),
			imagePointsRight_(imagePointsRight),
			homographyMatrix_(homographyMatrix),
			candidateHomographyMatrix_(homographyMatrix),
			modelParameters_(modelParameters),
			correspondences_(correspondences)
		{
			ocean_assert(correspondences_ >= 4);

			ocean_assert(Numeric::isEqual(homographyMatrix_(2, 2), 1));
			ocean_assert(Numeric::isEqual(candidateHomographyMatrix_(2, 2), 1));

			ocean_assert(modelParameters_ >= 8u && modelParameters_ <= 9u);
		};

		/**
		 * Determines the jacobian matrix for the current homography.
		 * @param jacobian Jacobian matrix
		 */
		inline void determineJacobian(Matrix& jacobian) const
		{
			if (modelParameters_ == 9u)
			{
				jacobian.resize(2 * correspondences_, 9);

				for (size_t n = 0; n < correspondences_; ++n)
				{
					Geometry::Jacobian::calculateHomographyJacobian2x9(jacobian[2 * n + 0], jacobian[2 * n + 1], imagePointsLeft_[n].x(), imagePointsLeft_[n].y(), homographyMatrix_);
				}
			}
			else
			{
				ocean_assert(modelParameters_ == 8u);

				jacobian.resize(2 * correspondences_, 8);

				for (size_t n = 0; n < correspondences_; ++n)
				{
					Geometry::Jacobian::calculateHomographyJacobian2x8(jacobian[2 * n + 0], jacobian[2 * n + 1], imagePointsLeft_[n].x(), imagePointsLeft_[n].y(), homographyMatrix_);
				}
			}
		}

		/**
		 * Applies the matrix correction and stores the new homography matrix as candidate
		 * @param deltas Optimization deltas that define the correction
		 */
		inline void applyCorrection(const Matrix& deltas)
		{
			ocean_assert(deltas.rows() == modelParameters_ && deltas.columns() == 1);

			/*
			 * H_{i+1} = H_{i} - Delta_{i}
			 */
			for (unsigned int n = 0u; n < modelParameters_; ++n)
			{
				candidateHomographyMatrix_[n] = homographyMatrix_[n] - deltas(n, 0);
			}

			if (modelParameters_ == 9u)
			{
				Homography::normalizeHomography(candidateHomographyMatrix_);
			}
			else
			{
				ocean_assert(Numeric::isEqual(candidateHomographyMatrix_(2, 2), 1));
			}
		}

		/**
		 * Determines the robust error of the current candidate homography matrix.
		 * @param weightedErrorVector Resulting (weighted - if using a robust estimator) error vector
		 * @param weightVector Vector holding the weights that have already been applied to the error vector
		 * @param invertedCovariances Optional 2x2 inverted covariance matrices
		 */
		template <Estimator::EstimatorType tEstimator>
		Scalar determineRobustError(Matrix& weightedErrorVector, Matrix& weightVector, const Matrix* invertedCovariances) const
		{
			if (!candidateHomographyMatrix_.isHomography())
			{
				return Numeric::maxValue();
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
				Scalar sqrError = Error::determineHomographyError<ConstTemplateArrayAccessor<Vector2>, true, false>(candidateHomographyMatrix_, ConstTemplateArrayAccessor<Vector2>(imagePointsLeft_, correspondences_), ConstTemplateArrayAccessor<Vector2>(imagePointsRight_, correspondences_), weightedErrors);

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
				Error::determineHomographyError<ConstTemplateArrayAccessor<Vector2>, true, true>(candidateHomographyMatrix_, ConstTemplateArrayAccessor<Vector2>(imagePointsLeft_, correspondences_), ConstTemplateArrayAccessor<Vector2>(imagePointsRight_, correspondences_), weightedErrors, sqrErrors.data());

				return sqrErrors2robustErrors2<tEstimator>(sqrErrors, modelParameters_, weightedErrors, (Vector2*)weightVector.data(), transposedInvertedCovariances);
			}
		}

		/**
		 * Accepts the current homography candidate as better matrix.
		 */
		inline void acceptCorrection()
		{
			homographyMatrix_ = candidateHomographyMatrix_;
		}

	protected:

		/// The image points in the left/first camera frame.
		const ImagePoint* imagePointsLeft_;

		/// The image points in the right/second camera frame, one for each left point.
		const ImagePoint* imagePointsRight_;

		/// The initial or (currently best) optimized homography.
		SquareMatrix3& homographyMatrix_;

		/// Definition of a candidate object for an optimized homography.
		SquareMatrix3 candidateHomographyMatrix_;

		/// The number of parameters that represent the optimization model of the homography, with range [8, 9].
		const unsigned int modelParameters_;

		/// Number of correspondences between left and right, with range [8, infinity)
		const size_t correspondences_;
};

bool NonLinearOptimizationHomography::optimizeHomography(const SquareMatrix3& homography, const Vector2* imagePointsLeft, const Vector2* imagePointsRight, const size_t correspondences, const unsigned int modelParameters, SquareMatrix3& optimizedHomography, const unsigned int iterations, const Estimator::EstimatorType estimator, const Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, const Matrix* invertedCovariances, Scalars* intermediates)
{
	ocean_assert(imagePointsLeft && imagePointsRight);
	ocean_assert(!homography.isNull());
	ocean_assert(correspondences >= 4);
	ocean_assert(modelParameters == 8u || modelParameters == 9u);

	SquareMatrix3 homographyMatrix(Homography::normalizedHomography(homography));

	HomographyOptimizationProvider provider(imagePointsLeft, imagePointsRight, correspondences, homographyMatrix, modelParameters);
	if(!denseOptimization<HomographyOptimizationProvider>(provider, iterations, estimator, lambda, lambdaFactor, initialError, finalError, invertedCovariances, intermediates))
	{
		return false;
	}

	ocean_assert(Numeric::isEqual(homographyMatrix(2, 2), 1));
	optimizedHomography = homographyMatrix;

	return true;
}

template <Estimator::EstimatorType tEstimator>
bool NonLinearOptimizationHomography::optimizeHomography(const SquareMatrix3& homography, const Vector2* imagePointsLeft, const Vector2* imagePointsRight, const size_t correspondences, const unsigned int modelParameters, SquareMatrix3& optimizedHomography, const unsigned int iterations, const Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, const Matrix* invertedCovariances, Scalars* intermediates)
{
	ocean_assert(imagePointsLeft && imagePointsRight);
	ocean_assert(!homography.isNull());
	ocean_assert(correspondences >= 4);
	ocean_assert(modelParameters == 8u || modelParameters == 9u);

	SquareMatrix3 homographyMatrix(Homography::normalizedHomography(homography));

	HomographyOptimizationProvider provider(imagePointsLeft, imagePointsRight, correspondences, homographyMatrix, modelParameters);
	if(!denseOptimization<HomographyOptimizationProvider, tEstimator>(provider, iterations, lambda, lambdaFactor, initialError, finalError, invertedCovariances, intermediates))
	{
		return false;
	}

	ocean_assert(Numeric::isEqual(homographyMatrix(2, 2), 1));
	optimizedHomography = homographyMatrix;

	return true;
}

// We force the compilation of the following template-based functions to ensure that they exist when needed/linked
template OCEAN_GEOMETRY_EXPORT bool NonLinearOptimizationHomography::optimizeHomography<Estimator::ET_SQUARE>(const SquareMatrix3&, const Vector2*, const Vector2*, const size_t, const unsigned int, SquareMatrix3&, const unsigned int, const Scalar, const Scalar, Scalar*, Scalar*, const Matrix*, Scalars*);
template OCEAN_GEOMETRY_EXPORT bool NonLinearOptimizationHomography::optimizeHomography<Estimator::ET_LINEAR>(const SquareMatrix3&, const Vector2*, const Vector2*, const size_t, const unsigned int, SquareMatrix3&, const unsigned int, const Scalar, const Scalar, Scalar*, Scalar*, const Matrix*, Scalars*);
template OCEAN_GEOMETRY_EXPORT bool NonLinearOptimizationHomography::optimizeHomography<Estimator::ET_HUBER>(const SquareMatrix3&, const Vector2*, const Vector2*, const size_t, const unsigned int, SquareMatrix3&, const unsigned int, const Scalar, const Scalar, Scalar*, Scalar*, const Matrix*, Scalars*);
template OCEAN_GEOMETRY_EXPORT bool NonLinearOptimizationHomography::optimizeHomography<Estimator::ET_TUKEY>(const SquareMatrix3&, const Vector2*, const Vector2*, const size_t, const unsigned int, SquareMatrix3&, const unsigned int, const Scalar, const Scalar, Scalar*, Scalar*, const Matrix*, Scalars*);
template OCEAN_GEOMETRY_EXPORT bool NonLinearOptimizationHomography::optimizeHomography<Estimator::ET_CAUCHY>(const SquareMatrix3&, const Vector2*, const Vector2*, const size_t, const unsigned int, SquareMatrix3&, const unsigned int, const Scalar, const Scalar, Scalar*, Scalar*, const Matrix*, Scalars*);

/**
 * This provider allows the optimization of a similarity transformation.
 */
class NonLinearOptimizationHomography::SimilarityOptimizationProvider : public NonLinearOptimization::OptimizationProvider
{
	public:

		/**
		 * Creates a new optimization provider object.
		 * @param imagePointsLeft Definition of a pair holding to sets of corresponding image points.
		 * @param imagePointsRight Definition of a pair holding to sets of corresponding image points.
		 * @param correspondences Number of point correspondences, with range [4, infinity)
		 * @param similarityMatrix The initial similarity transformation which will be optimized, must be normalized
		 */
		inline SimilarityOptimizationProvider(const ImagePoint* imagePointsLeft, const ImagePoint* imagePointsRight, const size_t correspondences, SquareMatrix3& similarityMatrix) :
			imagePointsLeft_(imagePointsLeft),
			imagePointsRight_(imagePointsRight),
			similarityMatrix_(similarityMatrix),
			candidateSimilarityMatrix_(similarityMatrix),
			correspondences_(correspondences)
		{
			ocean_assert(correspondences_ >= 2);

			ocean_assert(Numeric::isEqual(similarityMatrix_(2, 0), 0));
			ocean_assert(Numeric::isEqual(similarityMatrix_(2, 1), 0));
			ocean_assert(Numeric::isEqual(similarityMatrix_(2, 2), 1));
		};

		/**
		 * Determines the jacobian matrix for the current homography.
		 * @param jacobian Jacobian matrix
		 */
		inline void determineJacobian(Matrix& jacobian) const
		{
			jacobian.resize(2 * correspondences_, 4);

			for (size_t n = 0; n < correspondences_; ++n)
			{
				Geometry::Jacobian::calculateSimilarityJacobian2x4(jacobian[2 * n + 0], jacobian[2 * n + 1], imagePointsLeft_[n].x(), imagePointsLeft_[n].y(), similarityMatrix_);
			}
		}

		/**
		 * Returns whether the provider comes with an own solver for the linear equation.
		 * @return True, if so
		 */
		inline bool hasSolver() const
		{
			return true;
		}

		/**
		 * Solves the linear equation JTJ * deltas = jErrors.
		 * @param JTJ The Hesse matrix (transposed jacobian multiplied by the jacobian)
		 * @param jErrors The individual error values
		 * @param deltas The resulting individual delta values
		 * @return True, if the equation could be solved
		 */
		inline bool solve(const Matrix& JTJ, const Matrix& jErrors, Matrix& deltas) const
		{
			ocean_assert(JTJ.rows() == 4 && JTJ.columns() == 4);
			SquareMatrix4 matrix(JTJ.data(), true);

			if (matrix.invert() == false)
			{
				return false;
			}

			const Vector4 result = matrix * Vector4(jErrors.data());

			deltas = Matrix(4, 1, result.data());

			return true;
		}

		/**
		 * Applies the matrix correction and stores the new homography matrix as candidate
		 * @param deltas Optimization deltas that define the correction
		 */
		inline void applyCorrection(const Matrix& deltas)
		{
			ocean_assert(deltas.rows() == 4 && deltas.columns() == 1);

			/*
			 * S_{i+1} = S_{i} - Delta_{i}
			 *
			 * d0  -d1   d2
			 * d1   d0   d3
			 *  0    0    1
			 */

			candidateSimilarityMatrix_[0] = similarityMatrix_[0] - deltas(0, 0);
			candidateSimilarityMatrix_[1] = similarityMatrix_[1] - deltas(1, 0);

			candidateSimilarityMatrix_[3] = -candidateSimilarityMatrix_[1];
			candidateSimilarityMatrix_[4] = candidateSimilarityMatrix_[0];

			candidateSimilarityMatrix_[6] = similarityMatrix_[6] - deltas(2, 0);
			candidateSimilarityMatrix_[7] = similarityMatrix_[7] - deltas(3, 0);
		}

		/**
		 * Determines the robust error of the current candidate homography matrix.
		 * @param weightedErrorVector Resulting (weighted - if using a robust estimator) error vector
		 * @param weightVector Vector holding the weights that have already been applied to the error vector
		 * @param invertedCovariances Optional 2x2 inverted covariance matrices
		 */
		template <Estimator::EstimatorType tEstimator>
		Scalar determineRobustError(Matrix& weightedErrorVector, Matrix& weightVector, const Matrix* invertedCovariances) const
		{
			if (candidateSimilarityMatrix_.isSingular())
			{
				return Numeric::maxValue();
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
				Scalar sqrError = Error::determineHomographyError<ConstTemplateArrayAccessor<Vector2>, true, false>(candidateSimilarityMatrix_, ConstTemplateArrayAccessor<Vector2>(imagePointsLeft_, correspondences_), ConstTemplateArrayAccessor<Vector2>(imagePointsRight_, correspondences_), weightedErrors);

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
				Error::determineHomographyError<ConstTemplateArrayAccessor<Vector2>, true, true>(candidateSimilarityMatrix_, ConstTemplateArrayAccessor<Vector2>(imagePointsLeft_, correspondences_), ConstTemplateArrayAccessor<Vector2>(imagePointsRight_, correspondences_), weightedErrors, sqrErrors.data());

				return sqrErrors2robustErrors2<tEstimator>(sqrErrors, 4, weightedErrors, (Vector2*)weightVector.data(), transposedInvertedCovariances);
			}
		}

		/**
		 * Accepts the current homography candidate as better matrix.
		 */
		inline void acceptCorrection()
		{
			similarityMatrix_ = candidateSimilarityMatrix_;
		}

	protected:

		/// The image points in the left/first camera frame.
		const ImagePoint* imagePointsLeft_;

		/// The image points in the right/second camera frame, one for each left point.
		const ImagePoint* imagePointsRight_;

		/// The initial or (currently best) optimized similarity.
		SquareMatrix3& similarityMatrix_;

		/// Definition of a candidate object for an optimized similarity.
		SquareMatrix3 candidateSimilarityMatrix_;

		/// Number of correspondences between left and right, with range [8, infinity)
		const size_t correspondences_;
};

bool NonLinearOptimizationHomography::optimizeSimilarity(const SquareMatrix3& similarity, const Vector2* imagePointsLeft, const Vector2* imagePointsRight, const size_t correspondences, SquareMatrix3& optimizedSimilarity, const unsigned int iterations, const Estimator::EstimatorType estimator, const Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, const Matrix* invertedCovariances, Scalars* intermediates)
{
	ocean_assert(imagePointsLeft && imagePointsRight);
	ocean_assert(correspondences >= 2);

	ocean_assert_accuracy(Numeric::isWeakEqual(similarity(0, 0), similarity(1, 1)));
	ocean_assert_accuracy(Numeric::isWeakEqual(similarity(1, 0), -similarity(0, 1)));
	ocean_assert(Numeric::isEqual(similarity(2, 0), 0));
	ocean_assert(Numeric::isEqual(similarity(2, 0), 0));
	ocean_assert(Numeric::isEqual(similarity(2, 2), 1));

	SquareMatrix3 similarityMatrix(similarity);

	SimilarityOptimizationProvider provider(imagePointsLeft, imagePointsRight, correspondences, similarityMatrix);
	if(!denseOptimization<SimilarityOptimizationProvider>(provider, iterations, estimator, lambda, lambdaFactor, initialError, finalError, invertedCovariances, intermediates))
	{
		return false;
	}

	ocean_assert(Numeric::isEqual(similarityMatrix(0, 0), similarityMatrix(1, 1)));
	ocean_assert(Numeric::isEqual(similarityMatrix(1, 0), -similarityMatrix(0, 1)));
	ocean_assert(Numeric::isEqual(similarityMatrix(2, 0), 0));
	ocean_assert(Numeric::isEqual(similarityMatrix(2, 0), 0));
	ocean_assert(Numeric::isEqual(similarityMatrix(2, 2), 1));

	optimizedSimilarity = similarityMatrix;

	return true;
}

template <Estimator::EstimatorType tEstimator>
bool NonLinearOptimizationHomography::optimizeSimilarity(const SquareMatrix3& similarity, const Vector2* imagePointsLeft, const Vector2* imagePointsRight, const size_t correspondences, SquareMatrix3& optimizedSimilarity, const unsigned int iterations, const Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, const Matrix* invertedCovariances, Scalars* intermediates)
{
	ocean_assert(imagePointsLeft && imagePointsRight);
	ocean_assert(correspondences >= 2);

	ocean_assert_accuracy(Numeric::isWeakEqual(similarity(0, 0), similarity(1, 1)));
	ocean_assert_accuracy(Numeric::isWeakEqual(similarity(1, 0), -similarity(0, 1)));
	ocean_assert(Numeric::isEqual(similarity(2, 0), 0));
	ocean_assert(Numeric::isEqual(similarity(2, 0), 0));
	ocean_assert(Numeric::isEqual(similarity(2, 2), 1));

	SquareMatrix3 similarityMatrix(similarity);

	SimilarityOptimizationProvider provider(imagePointsLeft, imagePointsRight, correspondences, similarityMatrix);
	if(!denseOptimization<SimilarityOptimizationProvider, tEstimator>(provider, iterations, lambda, lambdaFactor, initialError, finalError, invertedCovariances, intermediates))
	{
		return false;
	}

	ocean_assert(Numeric::isEqual(similarityMatrix(0, 0), similarityMatrix(1, 1)));
	ocean_assert(Numeric::isEqual(similarityMatrix(1, 0), -similarityMatrix(0, 1)));
	ocean_assert(Numeric::isEqual(similarityMatrix(2, 0), 0));
	ocean_assert(Numeric::isEqual(similarityMatrix(2, 0), 0));
	ocean_assert(Numeric::isEqual(similarityMatrix(2, 2), 1));

	optimizedSimilarity = similarityMatrix;

	return true;
}

// We force the compilation of the following template-based functions to ensure that they exist when needed/linked
template OCEAN_GEOMETRY_EXPORT bool NonLinearOptimizationHomography::optimizeSimilarity<Estimator::ET_SQUARE>(const SquareMatrix3&, const Vector2*, const Vector2*, const size_t, SquareMatrix3&, const unsigned int, const Scalar, const Scalar, Scalar*, Scalar*, const Matrix*, Scalars*);
template OCEAN_GEOMETRY_EXPORT bool NonLinearOptimizationHomography::optimizeSimilarity<Estimator::ET_LINEAR>(const SquareMatrix3&, const Vector2*, const Vector2*, const size_t, SquareMatrix3&, const unsigned int, const Scalar, const Scalar, Scalar*, Scalar*, const Matrix*, Scalars*);
template OCEAN_GEOMETRY_EXPORT bool NonLinearOptimizationHomography::optimizeSimilarity<Estimator::ET_HUBER>(const SquareMatrix3&, const Vector2*, const Vector2*, const size_t, SquareMatrix3&, const unsigned int, const Scalar, const Scalar, Scalar*, Scalar*, const Matrix*, Scalars*);
template OCEAN_GEOMETRY_EXPORT bool NonLinearOptimizationHomography::optimizeSimilarity<Estimator::ET_TUKEY>(const SquareMatrix3&, const Vector2*, const Vector2*, const size_t, SquareMatrix3&, const unsigned int, const Scalar, const Scalar, Scalar*, Scalar*, const Matrix*, Scalars*);
template OCEAN_GEOMETRY_EXPORT bool NonLinearOptimizationHomography::optimizeSimilarity<Estimator::ET_CAUCHY>(const SquareMatrix3&, const Vector2*, const Vector2*, const size_t, SquareMatrix3&, const unsigned int, const Scalar, const Scalar, Scalar*, Scalar*, const Matrix*, Scalars*);

/**
 * This class implements a data object allowing the optimization of a Homography in combination with a camera model holding distortion parameters.
 */
class NonLinearOptimizationHomography::NormalizedHomographyData
{
	public:

		/**
		 * Creates a new data object.
		 * @param pinholeCamera The pinhole camera model holding the distortion models
		 * @param imagePointsLeft The left image points, must be valid
		 * @param imagePointsRight The right image points, one point for each left point, must be valid
		 */
		NormalizedHomographyData(const PinholeCamera& pinholeCamera, const ImagePoint* imagePointsLeft, const ImagePoint* imagePointsRight) :
			camera_(pinholeCamera),
			imagePointsLeft_(imagePointsLeft),
			imagePointsRight_(imagePointsRight)
		{
			// nothing to do here
		}

		/**
		 * Determines a specific transformed image point.
		 * @param externalModel The model of the homography to be used for the transformation
		 * @param index The index of the image point for which the transformed point will be determined
		 * @param result The resulting transformed image point
		 */
		void value(const StaticBuffer<Scalar, 9>& externalModel, const size_t index, StaticBuffer<Scalar, 2>& result)
		{
			const Vector2 distortedRight(transform(externalModel, index));

			result[0] = distortedRight[0];
			result[1] = distortedRight[1];
		}

		/**
		 * Determines the error between a specific transformed image point and the corresponding measurement.
		 * @param externalModel The model of the homography to be used for the transformation
		 * @param index The index of the image point for which the error will be determined
		 * @param result The resulting error
		 */
		bool error(const StaticBuffer<Scalar, 9>& externalModel, const size_t index, StaticBuffer<Scalar, 2>& result)
		{
			const Vector2 distortedRight(transform(externalModel, index));

			const Vector2& measurementImagePoint = imagePointsRight_[index];
			const Vector2 error(distortedRight - measurementImagePoint);

			result[0] = error[0];
			result[1] = error[1];
			return true;
		}

		/**
		 * Transformed the internal model to the external model.
		 * @param internalModel The internal model of the homography
		 * @param externalModel The resulting external model of the homography
		 */
		void transformModel(StaticBuffer<Scalar, 8>& internalModel, StaticBuffer<Scalar, 9>& externalModel)
		{
			SquareMatrix3 internalHomography;
			memcpy(internalHomography.data(), internalModel.data(), sizeof(Scalar) * 8);
			internalHomography[8] = 1;

			Homography::normalizeHomography(internalHomography);

			memcpy(internalModel.data(), internalHomography.data(), sizeof(Scalar) * 8);
			memcpy(externalModel.data(), internalHomography.data(), sizeof(Scalar) * 9);
		}

	protected:

		/**
		 * Transforms an image point using the current homography model and using the distortion parameters of the camera.
		 * @param externalModel The model of the homography to be used for the transformation
		 * @param index The index of the image point to be used
		 * @eturn The resulting transformed image point
		 */
		Vector2 transform(const StaticBuffer<Scalar, 9>& externalModel, const size_t index)
		{
			const SquareMatrix3& homography = *(SquareMatrix3*)(externalModel.data());
			ocean_assert(Numeric::isEqual(homography[8], 1));

			const Vector2 undistortedLeft(camera_.undistort<true>(imagePointsLeft_[index]));
			const Vector2 normalizedLeft(camera_.invertedIntrinsic() * undistortedLeft);

			const Vector2 normalizedTransformed(homography * normalizedLeft);
			return camera_.normalizedImagePoint2imagePoint<true>(normalizedTransformed, camera_.hasDistortionParameters());
		}

	protected:

		/// The camera profile holding the distortion parameters.
		const PinholeCamera& camera_;

		/// The left image points.
		const ImagePoint* imagePointsLeft_ = nullptr;

		/// The right image points, one for each left point.
		const ImagePoint* imagePointsRight_ = nullptr;
};

bool NonLinearOptimizationHomography::optimizeHomography(const PinholeCamera& pinholeCamera, const SquareMatrix3& homography, const Vector2* imagePointsLeft, const Vector2* imagePointsRight, const size_t correspondences, SquareMatrix3& optimizedHomography, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError)
{
	ocean_assert(imagePointsLeft && imagePointsRight);

	typedef NonLinearUniversalOptimizationDense<8, 2, 9> UniversalOptimization;

	NormalizedHomographyData data(pinholeCamera, imagePointsLeft, imagePointsRight);

	const SquareMatrix3 cameraFreeHomography(Homography::normalizedHomography(pinholeCamera.invertedIntrinsic() * homography * pinholeCamera.intrinsic()));

	UniversalOptimization::Model model;
	UniversalOptimization::Model optimizedModel;

	pinholeCamera.copyElements(model.data());

	ocean_assert(Numeric::isEqual(cameraFreeHomography[8], 1));
	memcpy(model.data(), cameraFreeHomography.data(), sizeof(Scalar) * 8);

	if (!UniversalOptimization::optimizeUniversalModel(model, correspondences,
			UniversalOptimization::ValueCallback::create(data, &NormalizedHomographyData::value),
			UniversalOptimization::ErrorCallback::create(data, &NormalizedHomographyData::error),
			UniversalOptimization::ModelTransformationCallback::create(data, &NormalizedHomographyData::transformModel),
			UniversalOptimization::ModelAdjustmentCallback(),
			optimizedModel, iterations, estimator, lambda, lambdaFactor, initialError, finalError))
	{
		return false;
	}

	SquareMatrix3 optimizedCameraFreeHomography;
	memcpy(optimizedCameraFreeHomography.data(), optimizedModel.data(), sizeof(Scalar) * 8);
	optimizedCameraFreeHomography[8] = 1;

	optimizedHomography = Homography::normalizedHomography(pinholeCamera.intrinsic() * optimizedCameraFreeHomography * pinholeCamera.invertedIntrinsic());

	return true;
}

/**
 * This class implements a data object allowing the optimization of a Homography and the camera profile concurrently.
 */
class NonLinearOptimizationHomography::HomographyCameraData
{
	public:

		/**
		 * Creates a new data object.
		 * @param cameraWidth The width of the camera frame in pixel, with range  (0, infinity)
		 * @param cameraHeight The height of the camera frame in pixel, with range (0, infinity)
		 * @param imagePointsLeft The left image points, must be valid
		 * @param imagePointsRight The right image points, one point for each left point, must be valid
		 */
		HomographyCameraData(const unsigned int cameraWidth, const unsigned int cameraHeight, const ImagePoint* imagePointsLeft, const ImagePoint* imagePointsRight) :
			cameraWidth_(cameraWidth),
			cameraHeight_(cameraHeight),
			imagePointsLeft_(imagePointsLeft),
			imagePointsRight_(imagePointsRight)
		{
			// nothing to do here
		}

		/**
		 * Determines a specific transformed image point.
		 * @param externalModel The model of the homography and the camera profiles to be used for the transformation
		 * @param index The index of the image point for which the transformed point will be determined
		 * @param result The resulting transformed image point
		 */
		void value(const StaticBuffer<Scalar, 17>& externalModel, const size_t index, StaticBuffer<Scalar, 2>& result)
		{
			const Vector2 distortedRight(transform(externalModel, index));

			result[0] = distortedRight[0];
			result[1] = distortedRight[1];
		}

		/**
		 * Determines the error between a specific transformed image point and the corresponding measurement.
		 * @param externalModel The model of the homography and the camera profile to be used for the transformation
		 * @param index The index of the image point for which the error will be determined
		 * @param result The resulting error
		 */
		bool error(const StaticBuffer<Scalar, 17>& externalModel, const size_t index, StaticBuffer<Scalar, 2>& result)
		{
			const Vector2 distortedRight(transform(externalModel, index));

			const Vector2& measurementImagePoint = imagePointsRight_[index];
			const Vector2 error(distortedRight - measurementImagePoint);

			result[0] = error[0];
			result[1] = error[1];
			return true;
		}

		/**
		 * Transformed the internal model to the external model.
		 * @param internalModel The internal model of the homography
		 * @param externalModel The resulting externa model of the homography
		 */
		void transformModel(StaticBuffer<Scalar, 16>& internalModel, StaticBuffer<Scalar, 17>& externalModel)
		{
			SquareMatrix3 internalHomography;
			memcpy(internalHomography.data(), internalModel.data() + 8, sizeof(Scalar) * 8);
			internalHomography[8] = 1;

			Homography::normalizeHomography(internalHomography);
			memcpy(internalModel.data() + 8, internalHomography.data(), sizeof(Scalar) * 8);

			memcpy(externalModel.data(), internalModel.data(), sizeof(Scalar) * 8);
			memcpy(externalModel.data() + 8, internalHomography.data(), sizeof(Scalar) * 9);
		}

	protected:

		/**
		 * Transforms an image point using the current homography model and camera profile model.
		 * @param externalModel The model of the homography and camera profile to be used for the transformation
		 * @param index The index of the image point to be used
		 * @eturn The resulting transformed image point
		 */
		Vector2 transform(const StaticBuffer<Scalar, 17>& externalModel, const size_t index)
		{
			const PinholeCamera pinholeCamera(cameraWidth_, cameraHeight_, externalModel.data());
			const SquareMatrix3& homography = *(SquareMatrix3*)(externalModel.data() + 8);
			ocean_assert(Numeric::isEqual(homography[8], 1));

			const Vector2 undistortedLeft(pinholeCamera.undistort<true>(imagePointsLeft_[index]));
			const Vector2 normalizedLeft(pinholeCamera.invertedIntrinsic() * undistortedLeft);

			const Vector2 normalizedTransformed(homography * normalizedLeft);
			return pinholeCamera.normalizedImagePoint2imagePoint<true>(normalizedTransformed, pinholeCamera.hasDistortionParameters());
		}

	protected:

		/// The width of the camera frame in pixel, with range (0, infinity)
		const unsigned int cameraWidth_ = 0u;

		/// The height of the camera frame in pixel, with range (0, infinity)
		const unsigned int cameraHeight_ = 0u;

		/// The left image points.
		const ImagePoint* imagePointsLeft_ = nullptr;

		/// The right image points, one for each left point.
		const ImagePoint* imagePointsRight_ = nullptr;
};

bool NonLinearOptimizationHomography::optimizeCameraHomography(const PinholeCamera& pinholeCamera, const SquareMatrix3& homography, const Vector2* imagePointsLeft, const Vector2* imagePointsRight, const size_t correspondences, PinholeCamera& optimizedCamera, SquareMatrix3& optimizedHomography, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError)
{
	ocean_assert(imagePointsLeft && imagePointsRight);

	typedef NonLinearUniversalOptimizationDense<16, 2, 17> UniversalOptimization;

	HomographyCameraData data(pinholeCamera.width(), pinholeCamera.height(), imagePointsLeft, imagePointsRight);

	const SquareMatrix3 cameraFreeHomography(Homography::normalizedHomography(pinholeCamera.invertedIntrinsic() * homography * pinholeCamera.intrinsic()));

	UniversalOptimization::Model model;
	UniversalOptimization::Model optimizedModel;

	pinholeCamera.copyElements(model.data());

	ocean_assert(Numeric::isEqual(cameraFreeHomography[8], 1));
	memcpy(model.data() + 8, cameraFreeHomography.data(), sizeof(Scalar) * 8);

	if (!UniversalOptimization::optimizeUniversalModel(model, correspondences, UniversalOptimization::ValueCallback::create(data, &HomographyCameraData::value), UniversalOptimization::ErrorCallback::create(data, &HomographyCameraData::error), UniversalOptimization::ModelTransformationCallback::create(data, &HomographyCameraData::transformModel), UniversalOptimization::ModelAdjustmentCallback(), optimizedModel, iterations, estimator, lambda, lambdaFactor, initialError, finalError))
	{
		return false;
	}

	optimizedCamera = PinholeCamera(pinholeCamera.width(), pinholeCamera.height(), optimizedModel.data());

	SquareMatrix3 optimizedCameraFreeHomography;
	memcpy(optimizedCameraFreeHomography.data(), optimizedModel.data() + 8, sizeof(Scalar) * 8);
	optimizedCameraFreeHomography[8] = 1;

	optimizedHomography = Homography::normalizedHomography(optimizedCamera.intrinsic() * optimizedCameraFreeHomography * optimizedCamera.invertedIntrinsic());

	return true;
}

class NonLinearOptimizationHomography::CameraHomographiesData
{
	public:

		CameraHomographiesData(const unsigned int cameraWidth, const unsigned int cameraHeight, const ImagePointsPairs& imagePointsPairs) :
			cameraWidth_(cameraWidth),
			cameraHeight_(cameraHeight),
			imagePointsPairs_(imagePointsPairs)
		{
			// nothing to do here
		}

		void value(const StaticBuffer<Scalar, 8>& externalSharedModel, const StaticBuffer<Scalar, 9>& externalIndividualModel, const size_t individualModelIndex, const size_t elementIndex, StaticBuffer<Scalar, 2>& result)
		{
			const Vector2& leftPoint = imagePointsPairs_[individualModelIndex].first[elementIndex];

			const Vector2 transformedRightPoint(transformPoint(externalSharedModel, externalIndividualModel, leftPoint));

			result[0] = transformedRightPoint[0];
			result[1] = transformedRightPoint[1];
		}

		bool error(const StaticBuffer<Scalar, 8>& externalSharedModel, const StaticBuffer<Scalar, 9>& externalIndividualModel, const size_t individualModelIndex, const size_t elementIndex, StaticBuffer<Scalar, 2>& result)
		{
			const Vector2& leftPoint = imagePointsPairs_[individualModelIndex].first[elementIndex];
			const Vector2& rightPoint = imagePointsPairs_[individualModelIndex].second[elementIndex];

			const Vector2 transformedRightPoint(transformPoint(externalSharedModel, externalIndividualModel, leftPoint));

			const Vector2 error(transformedRightPoint - rightPoint);

			result[0] = Scalar(int(error[0] * Scalar(5))) * Scalar(0.2); // **TODO**
			result[1] = Scalar(int(error[1] * Scalar(5))) * Scalar(0.2);
			return true;
		}

		void transformSharedModel(StaticBuffer<Scalar, 8>& internalModel, StaticBuffer<Scalar, 8>& externalModel)
		{
			externalModel = internalModel;
		}

		/**
		 * Transforms the internal individual model to an external individual model.
		 * @param internalModel Internal model that will be transformed
		 * @param externalModel Resulting external model
		 */
		void transformIndividualModel(StaticBuffer<Scalar, 8>& internalModel, StaticBuffer<Scalar, 9>& externalModel)
		{
			SquareMatrix3 homography;
			memcpy(homography.data(), internalModel.data(), 8 * sizeof(Scalar));
			homography[8] = Scalar(1);

			Homography::normalizeHomography(homography);
			memcpy(internalModel.data(), homography.data(), 8 * sizeof(Scalar));

			memcpy(externalModel.data(), homography.data(), 9 * sizeof(Scalar));
		}

	protected:

		inline Vector2 transformPoint(const StaticBuffer<Scalar, 8>& externalSharedModel, const StaticBuffer<Scalar, 9>& externalIndividualModel, const Vector2& leftImagePoint)
		{
			const PinholeCamera pinholeCamera(cameraWidth_, cameraHeight_, externalSharedModel.data());
			const SquareMatrix3& homography = *(SquareMatrix3*)externalIndividualModel.data();
			ocean_assert(Numeric::isEqual(homography[8], 1));

			const Vector2 undistortedLeftImagePoint(pinholeCamera.undistort<true>(leftImagePoint));
			const Vector2 normalizedLeftImagePoint(pinholeCamera.invertedIntrinsic() * undistortedLeftImagePoint);

			const Vector2 normalizedTransformed(homography * normalizedLeftImagePoint);
			return pinholeCamera.normalizedImagePoint2imagePoint<true>(normalizedTransformed, pinholeCamera.hasDistortionParameters());
		}

	protected:

		/// The width of the camera frame in pixel, with range [1, infinity).
		const unsigned int cameraWidth_ = 0u;

		/// The height of the camera frame in pixel, with range [1, infinity).
		const unsigned int cameraHeight_ = 0u;

		/// The pairs of image points.
		const ImagePointsPairs& imagePointsPairs_;
};

class NonLinearOptimizationHomography::DistortionCameraHomographiesData
{
	public:

		DistortionCameraHomographiesData(const PinholeCamera& pinholeCamera, const ImagePointsPairs& imagePointsPairs) :
			camera_(pinholeCamera),
			imagePointsPairs_(imagePointsPairs)
		{
			// nothing to do here
		}

		void value(const StaticBuffer<Scalar, 4>& externalSharedModel, const StaticBuffer<Scalar, 9>& externalIndividualModel, const size_t individualModelIndex, const size_t elementIndex, StaticBuffer<Scalar, 2>& result)
		{
			const Vector2& leftPoint = imagePointsPairs_[individualModelIndex].first[elementIndex];

			const Vector2 transformedRightPoint(transformPoint(externalSharedModel, externalIndividualModel, leftPoint));

			result[0] = transformedRightPoint[0];
			result[1] = transformedRightPoint[1];
		}

		bool error(const StaticBuffer<Scalar, 4>& externalSharedModel, const StaticBuffer<Scalar, 9>& externalIndividualModel, const size_t individualModelIndex, const size_t elementIndex, StaticBuffer<Scalar, 2>& result)
		{
			const Vector2& leftPoint = imagePointsPairs_[individualModelIndex].first[elementIndex];
			const Vector2& rightPoint = imagePointsPairs_[individualModelIndex].second[elementIndex];

			const Vector2 transformedRightPoint(transformPoint(externalSharedModel, externalIndividualModel, leftPoint));

			const Vector2 error(transformedRightPoint - rightPoint);

			result[0] = Scalar(int(error[0] * Scalar(5))) * Scalar(0.2); // **TODO**
			result[1] = Scalar(int(error[1] * Scalar(5))) * Scalar(0.2);
			return true;
		}

		void transformSharedModel(StaticBuffer<Scalar, 4>& internalModel, StaticBuffer<Scalar, 4>& externalModel)
		{
			externalModel = internalModel;
		}

		/**
		 * Transforms the internal individual model to an external individual model.
		 * @param internalModel Internal model that will be transformed
		 * @param externalModel Resulting external model
		 */
		void transformIndividualModel(StaticBuffer<Scalar, 8>& internalModel, StaticBuffer<Scalar, 9>& externalModel)
		{
			SquareMatrix3 homography;
			memcpy(homography.data(), internalModel.data(), 8 * sizeof(Scalar));
			homography[8] = Scalar(1);

			Homography::normalizeHomography(homography);
			memcpy(internalModel.data(), homography.data(), 8 * sizeof(Scalar));

			memcpy(externalModel.data(), homography.data(), 9 * sizeof(Scalar));
		}

	protected:

		inline Vector2 transformPoint(const StaticBuffer<Scalar, 4>& externalSharedModel, const StaticBuffer<Scalar, 9>& externalIndividualModel, const Vector2& leftImagePoint)
		{
			PinholeCamera pinholeCamera(camera_);
			pinholeCamera.setRadialDistortion(PinholeCamera::DistortionPair(externalSharedModel[0], externalSharedModel[1]));
			pinholeCamera.setTangentialDistortion(PinholeCamera::DistortionPair(externalSharedModel[2], externalSharedModel[3]));

			const SquareMatrix3& homography = *(SquareMatrix3*)externalIndividualModel.data();
			ocean_assert(Numeric::isEqual(homography[8], 1));

			const Vector2 undistortedLeftImagePoint(pinholeCamera.undistort<true>(leftImagePoint));
			const Vector2 normalizedLeftImagePoint(pinholeCamera.invertedIntrinsic() * undistortedLeftImagePoint);

			const Vector2 normalizedTransformed(homography * normalizedLeftImagePoint);
			return pinholeCamera.normalizedImagePoint2imagePoint<true>(normalizedTransformed, pinholeCamera.hasDistortionParameters());
		}

	protected:

		/// The initial camera profile that will be optimized.
		const PinholeCamera& camera_;

		/// The image points pair correspondences, individual for each homography.
		const ImagePointsPairs& imagePointsPairs_;
};

bool NonLinearOptimizationHomography::optimizeCameraHomographies(const PinholeCamera& pinholeCamera, const SquareMatrices3& homographies, const ImagePointsPairs& imagePointsPairs, PinholeCamera& optimizedCamera, SquareMatrices3& optimizedHomographies, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(homographies.size() == imagePointsPairs.size());

	typedef NonLinearUniversalOptimizationSparse::SharedModelIndividualModels<8u, 8u, 2u, 8u, 9u> UniversalOptimization;

	UniversalOptimization::SharedModel sharedModel, optimizedSharedModel;
	pinholeCamera.copyElements(sharedModel.data());

	UniversalOptimization::IndividualModels individualModels, optimizedIndividualModels;
	for (SquareMatrices3::const_iterator i = homographies.begin(); i != homographies.end(); ++i)
	{
		const SquareMatrix3 normalizedHomography(Homography::normalizedHomography(pinholeCamera.invertedIntrinsic() * *i * pinholeCamera.intrinsic()));

		const UniversalOptimization::IndividualModel indiviudalModel(normalizedHomography.data());
		individualModels.push_back(indiviudalModel);
	}

	std::vector<size_t> numberElementsPerIndividualModel;
	numberElementsPerIndividualModel.reserve(imagePointsPairs.size());
	for (ImagePointsPairs::const_iterator i = imagePointsPairs.begin(); i != imagePointsPairs.end(); ++i)
	{
		ocean_assert(i->first.size() == i->second.size());
		numberElementsPerIndividualModel.push_back(i->first.size());
	}

	CameraHomographiesData data(pinholeCamera.width(), pinholeCamera.height(), imagePointsPairs);

	if (!UniversalOptimization::optimizeUniversalModel(sharedModel, individualModels, numberElementsPerIndividualModel.data(),
					UniversalOptimization::ValueCallback::create(data, &CameraHomographiesData::value),
					UniversalOptimization::ErrorCallback::create(data, &CameraHomographiesData::error),
					UniversalOptimization::SharedModelIsValidCallback(),
					UniversalOptimization::SharedModelTransformationCallback::create(data, &CameraHomographiesData::transformSharedModel),
					UniversalOptimization::IndividualModelTransformationCallback::create(data, &CameraHomographiesData::transformIndividualModel),
					UniversalOptimization::ModelAcceptedCallback(),
					optimizedSharedModel, optimizedIndividualModels, iterations, estimator, lambda, lambdaFactor, initialError, finalError))
	{
		return false;
	}


	optimizedCamera = PinholeCamera(pinholeCamera.width(), pinholeCamera.height(), optimizedSharedModel.data());

	optimizedHomographies.clear();
	optimizedHomographies.reserve(homographies.size());

	for (UniversalOptimization::IndividualModels::const_iterator i = optimizedIndividualModels.begin(); i != optimizedIndividualModels.end(); ++i)
	{
		SquareMatrix3 homography;
		memcpy(homography.data(), i->data(), sizeof(Scalar) * 8);
		homography[8] = 1;

		optimizedHomographies.push_back(Homography::normalizedHomography(optimizedCamera.intrinsic() * homography * optimizedCamera.invertedIntrinsic()));
	}

	return true;
}

bool NonLinearOptimizationHomography::optimizeDistortionCameraHomographies(const PinholeCamera& pinholeCamera, const SquareMatrices3& homographies, const ImagePointsPairs& imagePointsPairs, PinholeCamera& optimizedCamera, SquareMatrices3& optimizedHomographies, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(homographies.size() == imagePointsPairs.size());

	typedef NonLinearUniversalOptimizationSparse::SharedModelIndividualModels<4u, 8u, 2u, 4u, 9u> UniversalOptimization;

	UniversalOptimization::SharedModel sharedModel, optimizedSharedModel;
	sharedModel[0] = pinholeCamera.radialDistortion().first;
	sharedModel[1] = pinholeCamera.radialDistortion().second;
	sharedModel[2] = pinholeCamera.tangentialDistortion().first;
	sharedModel[3] = pinholeCamera.tangentialDistortion().second;

	UniversalOptimization::IndividualModels individualModels, optimizedIndividualModels;
	for (SquareMatrices3::const_iterator i = homographies.begin(); i != homographies.end(); ++i)
	{
		const SquareMatrix3 normalizedHomography(Homography::normalizedHomography(pinholeCamera.invertedIntrinsic() * *i * pinholeCamera.intrinsic()));

		const UniversalOptimization::IndividualModel indiviudalModel(normalizedHomography.data());
		individualModels.push_back(indiviudalModel);
	}

	std::vector<size_t> numberElementsPerIndividualModel;
	numberElementsPerIndividualModel.reserve(imagePointsPairs.size());
	for (ImagePointsPairs::const_iterator i = imagePointsPairs.begin(); i != imagePointsPairs.end(); ++i)
	{
		ocean_assert(i->first.size() == i->second.size());
		numberElementsPerIndividualModel.push_back(i->first.size());
	}

	DistortionCameraHomographiesData data(pinholeCamera, imagePointsPairs);

	if (!UniversalOptimization::optimizeUniversalModel(sharedModel, individualModels, numberElementsPerIndividualModel.data(),
					UniversalOptimization::ValueCallback::create(data, &DistortionCameraHomographiesData::value),
					UniversalOptimization::ErrorCallback::create(data, &DistortionCameraHomographiesData::error),
					UniversalOptimization::SharedModelIsValidCallback(),
					UniversalOptimization::SharedModelTransformationCallback::create(data, &DistortionCameraHomographiesData::transformSharedModel),
					UniversalOptimization::IndividualModelTransformationCallback::create(data, &DistortionCameraHomographiesData::transformIndividualModel),
					UniversalOptimization::ModelAcceptedCallback(),
					optimizedSharedModel, optimizedIndividualModels, iterations, estimator, lambda, lambdaFactor, initialError, finalError))
	{
		return false;
	}

	optimizedCamera = pinholeCamera;
	optimizedCamera.setRadialDistortion(PinholeCamera::DistortionPair(optimizedSharedModel[0], optimizedSharedModel[1]));
	optimizedCamera.setTangentialDistortion(PinholeCamera::DistortionPair(optimizedSharedModel[2], optimizedSharedModel[3]));

	optimizedHomographies.clear();
	optimizedHomographies.reserve(homographies.size());

	for (UniversalOptimization::IndividualModels::const_iterator i = optimizedIndividualModels.begin(); i != optimizedIndividualModels.end(); ++i)
	{
		SquareMatrix3 homography;
		memcpy(homography.data(), i->data(), sizeof(Scalar) * 8);
		homography[8] = 1;

		optimizedHomographies.push_back(Homography::normalizedHomography(optimizedCamera.intrinsic() * homography * optimizedCamera.invertedIntrinsic()));
	}

	return true;
}

}

}
