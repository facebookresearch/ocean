/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_NON_LINEAR_UNIVERSAL_OPTIMIZATION_DENSE_H
#define META_OCEAN_GEOMETRY_NON_LINEAR_UNIVERSAL_OPTIMIZATION_DENSE_H

#include "ocean/geometry/Geometry.h"
#include "ocean/geometry/NonLinearOptimization.h"

#include "ocean/base/StaticBuffer.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements an optimization for universal dense problems with one model (optimization problem).
 * The implementation allows to optimize arbitrary (universal) problems with arbitrary dimensions.
 * @tparam tModelSize Size of the model, the number of model parameters
 * @tparam tResultDimension Number of dimensions that result for each element (measurement) after the model has been applied
 * @tparam tExternalModelSize Size of the external model, the number of model parameters
 * @ingroup geometry
 */
template <unsigned int tModelSize, unsigned int tResultDimension, unsigned int tExternalModelSize = tModelSize>
class NonLinearUniversalOptimizationDense : protected NonLinearOptimization
{
	public:

		/**
		 * Definition of a model.
		 */
		typedef StaticBuffer<Scalar, tModelSize> Model;

		/**
		 * Definition of an external model.
		 */
		typedef StaticBuffer<Scalar, tExternalModelSize> ExternalModel;

		/**
		 * Definition of a model result.
		 */
		typedef StaticBuffer<Scalar, tResultDimension> Result;

		/**
		 * Definition of a callback function for dense value calculation.
		 * The first parameter provides the external model that is applied to determine the value.<br>
		 * The second parameter provides the index of the measurement elements that is used to determine the value.<br>
		 * The third parameter receives the determined value.
		 */
		typedef Callback<void, const ExternalModel&, const size_t, Result&> ValueCallback;

		/**
		 * Definition of a callback function for dense error calculation.
		 * The first parameter provides the external model that is applied to determine the error.<br>
		 * The second parameter provides the index of the measurement elements that is used to determine the error.<br>
		 * The third parameter receives the determined error.<br>
		 * The return value provides True if both models provide valid information for the measurement element.
		 */
		typedef Callback<bool, const ExternalModel&, const size_t, Result&> ErrorCallback;

		/**
		 * Definition of a dense model transformation function.
		 * The transformation function allows to use an external model function for value and error determination while the internal model is used for the internal optimization.<br>
		 * The first parameter provides the internal model.<br>
		 * The second parameter receives the external model.
		 */
		typedef Callback<void, Model&, ExternalModel&> ModelTransformationCallback;

		/**
		 * Definition of a dense model adjustment function.
		 * The adjustment function allows to modify the internal model (the modification should be tiny e.g., a normalization of a vector which has almost length 1) before the model will be accepted or rejected
		 * The first parameter provides the model to be adjusted
		 */
		typedef Callback<void, Model&> ModelAdjustmentCallback;

	protected:

		/**
		 * This class implements a dense universal optimization provider for universal models and measurement/data values.
		 */
		class UniversalOptimizationProvider : public NonLinearOptimization::OptimizationProvider
		{
			public:

				/**
				 * Creates a new universal optimization object.
				 * @param model The model to be optimized
				 * @param numberElements Number of elements (measurements) that are used to determine the optimized model
				 * @param valueCallback Callback function that is used to determine the value for an individual element (measurement) by application of the model
				 * @param errorCallback Callback function that is used to determine the error for an individual element (measurement)
				 * @param modelTransformationCallback Callback function allowing to transform the internal model into an extern model if intended
				 * @param modelAdjustmentCallback Callback function allowing to adjust the internal model before it will be accepted or rejected
				 */
				inline UniversalOptimizationProvider(Model& model, const size_t numberElements, const ValueCallback& valueCallback, const ErrorCallback& errorCallback, const ModelTransformationCallback& modelTransformationCallback, const ModelAdjustmentCallback& modelAdjustmentCallback);

				/**
				 * Determines the jacobian matrix for the current camera.
				 * @param jacobian Jacobian matrix
				 */
				void determineJacobian(Matrix& jacobian) const;

				/**
				 * Applies the model correction and stores the new model as candidate
				 * @param deltas Optimization deltas that define the correction
				 */
				inline void applyCorrection(const Matrix& deltas);

				/**
				 * Determines the robust error of the current candidate pose.
				 * @param weightedErrorVector Resulting (weighted - if using a robust estimator) error vector
				 * @param weightVector Vector holding the weights that have already been applied to the error vector
				 * @param invertedCovariances Optional 2x2 inverted covariance matrices
				 * @return The resulting robust error
				 * @tparam tEstimator The type of the estimator that is applied for error determination
				 */
				template <Estimator::EstimatorType tEstimator>
				Scalar determineRobustError(Matrix& weightedErrorVector, Matrix& weightVector, const Matrix* invertedCovariances) const;

				/**
				 * Accepts the current pose candidate as better model.
				 */
				inline void acceptCorrection();

			protected:

				/// Universal model that will be optimized.
				Model& model_;

				/// Universal model that stores the most recent optimization result as candidate.
				mutable Model candidateModel_;

				/// The number of measurement elements that are used to optimize the model.
				const size_t numberElements_;

				/// The value calculation callback function.
				const ValueCallback valueCallback_;

				/// The error calculation callback function.
				const ErrorCallback errorCallback_;

				/// The Callback function allowing to transform the model into an external model before the value and error callback functions are invoked.
				const ModelTransformationCallback modelTransformationCallback_;

				/// The optional callback function allowing to adjust a model before it is accepted or rejected
				const ModelAdjustmentCallback modelAdjustmentCallback_;
		};

	public:

		/**
		 * Optimizes a universal model by minimizing the error the model produces.
		 * @param model Universal model that will be optimized
		 * @param numberElements Number of elements (measurements) that are used to determine the optimized model
		 * @param valueCallback Callback function that is used to determine the value for an individual element (measurement) by application of the model
		 * @param errorCallback Callback function that is used to determine the error for an individual element (measurement)
		 * @param modelTransformationCallback Optional callback function allowing to transform the internal model into an extern model if intended
		 * @param modelAdjustmentCallback Optional callback function allowing to adjust the internal (already optimized) model before it will be checked on improvements
		 * @param optimizedModel Resulting optimized model
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if the model could be optimized
		 */
		static bool optimizeUniversalModel(const Model& model, const size_t numberElements, const ValueCallback& valueCallback, const ErrorCallback& errorCallback, const ModelTransformationCallback& modelTransformationCallback, const ModelAdjustmentCallback& modelAdjustmentCallback, Model& optimizedModel, const unsigned int iterations = 5u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);
};

template <unsigned int tModelSize, unsigned int tResultDimension, unsigned int tExternalModelSize>
NonLinearUniversalOptimizationDense<tModelSize, tResultDimension, tExternalModelSize>::UniversalOptimizationProvider::UniversalOptimizationProvider(Model& model, const size_t numberElements, const ValueCallback& valueCallback, const ErrorCallback& errorCallback, const ModelTransformationCallback& modelTransformationCallback, const ModelAdjustmentCallback& modelAdjustmentCallback) :
	model_(model),
	numberElements_(numberElements),
	valueCallback_(valueCallback),
	errorCallback_(errorCallback),
	modelTransformationCallback_(modelTransformationCallback),
	modelAdjustmentCallback_(modelAdjustmentCallback)
{
	ocean_assert(valueCallback_);
	ocean_assert(errorCallback_);
	ocean_assert(modelTransformationCallback_);

	candidateModel_ = model;
};

template <unsigned int tModelSize, unsigned int tResultDimension, unsigned int tExternalModelSize>
void NonLinearUniversalOptimizationDense<tModelSize, tResultDimension, tExternalModelSize>::UniversalOptimizationProvider::determineJacobian(Matrix& jacobian) const
{
	ocean_assert(valueCallback_);
	ocean_assert(modelTransformationCallback_);

	jacobian.resize(tResultDimension * numberElements_, tModelSize);

	const Scalar eps = Numeric::weakEps();
	const Scalar invEps = Scalar(1) / eps;

	// transform the internal to the external model
	ExternalModel externalModel;
	modelTransformationCallback_(model_, externalModel);

	// stores individual models, each model with one individual epsilon offset
	StaticBuffer<ExternalModel, tModelSize> externalEpsModels;
	for (size_t a = 0; a < tModelSize; ++a)
	{
		Model internalModel = model_;
		internalModel[a] += eps;

		modelTransformationCallback_(internalModel, externalEpsModels[a]);
	}

	Result result, epsResult;
	for (size_t n = 0; n < numberElements_; ++n)
	{
		// calculate the value for the current model
		valueCallback_(externalModel, n, result);

		for (size_t m = 0; m < tModelSize; ++m)
		{
			// calculate the value for the epsilon model
			valueCallback_(externalEpsModels[m], n, epsResult);

			// store the individual results
			for (size_t d = 0; d < tResultDimension; ++d)
			{
				jacobian[n * tResultDimension + d][m] = (epsResult[d] - result[d]) * invEps;
			}
		}
	}
}

template <unsigned int tModelSize, unsigned int tResultDimension, unsigned int tExternalModelSize>
inline void NonLinearUniversalOptimizationDense<tModelSize, tResultDimension, tExternalModelSize>::UniversalOptimizationProvider::applyCorrection(const Matrix& deltas)
{
	ocean_assert(deltas.rows() == tModelSize);

	for (size_t n = 0; n < tModelSize; ++n)
	{
		const Scalar& delta = deltas(n);
		candidateModel_[n] = model_[n] - delta;
	}

	if (modelAdjustmentCallback_)
	{
		modelAdjustmentCallback_(candidateModel_);
	}
}

template <unsigned int tModelSize, unsigned int tResultDimension, unsigned int tExternalModelSize>
template <Estimator::EstimatorType tEstimator>
Scalar NonLinearUniversalOptimizationDense<tModelSize, tResultDimension, tExternalModelSize>::UniversalOptimizationProvider::determineRobustError(Matrix& weightedErrorVector, Matrix& weightVector, const Matrix* invertedCovariances) const
{
	ocean_assert(errorCallback_);
	ocean_assert(modelTransformationCallback_);

	OCEAN_SUPPRESS_UNUSED_WARNING(invertedCovariances);
	ocean_assert(invertedCovariances == nullptr);

	// set the correct size of the resulting error vector
	weightedErrorVector.resize(tResultDimension * numberElements_, 1u);
	Result* const weightedErrors = (Result*)weightedErrorVector.data();

	ExternalModel externalModel;
	modelTransformationCallback_(candidateModel_, externalModel);

	Scalar sqrError = 0;
	Scalars sqrErrors;

	if constexpr (!Estimator::isStandardEstimator<tEstimator>())
	{
		sqrErrors.reserve(numberElements_);
	}

	for (size_t n = 0; n < numberElements_; ++n)
	{
		Result& weightedErrorPointer = *((Result*)weightedErrorVector.data() + n);
		if (!errorCallback_(externalModel, n, weightedErrorPointer))
		{
			return Numeric::maxValue();
		}

		if constexpr (Estimator::isStandardEstimator<tEstimator>())
		{
			sqrError += Numeric::summedSqr(weightedErrorPointer.data(), tResultDimension);
		}
		else
		{
			ocean_assert(!Estimator::isStandardEstimator<tEstimator>());
			sqrErrors.emplace_back(Numeric::summedSqr(weightedErrorPointer.data(), tResultDimension));
		}
	}

	// check whether the standard estimator is used
	if constexpr (Estimator::isStandardEstimator<tEstimator>())
	{
		// the weight vector should be and should stay invalid
		ocean_assert(!weightVector);

		ocean_assert(numberElements_ > 0);
		return sqrError /= Scalar(numberElements_);
	}
	else
	{
		// now we need the weight vector
		weightVector.resize(tResultDimension * numberElements_, 1u);

		ocean_assert(sqrErrors.size() == numberElements_);

		return sqrErrors2robustErrors<tEstimator, tResultDimension>(sqrErrors, tModelSize, weightedErrors, (StaticBuffer<Scalar, tResultDimension>*)(weightVector.data()), nullptr);
	}
}

template <unsigned int tModelSize, unsigned int tResultDimension, unsigned int tExternalModelSize>
inline void NonLinearUniversalOptimizationDense<tModelSize, tResultDimension, tExternalModelSize>::UniversalOptimizationProvider::acceptCorrection()
{
	model_ = candidateModel_;
}

template <unsigned int tModelSize, unsigned int tResultDimension, unsigned int tExternalModelSize>
bool NonLinearUniversalOptimizationDense<tModelSize, tResultDimension, tExternalModelSize>::optimizeUniversalModel(const Model& model, const size_t numberElements, const ValueCallback& valueCallback, const ErrorCallback& errorCallback, const ModelTransformationCallback& modelTransformationCallback, const ModelAdjustmentCallback& modelAdjustmentCallback, Model& optimizedModel, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
	ocean_assert(&model != &optimizedModel);
	optimizedModel = model;

	UniversalOptimizationProvider provider(optimizedModel, numberElements, valueCallback, errorCallback, modelTransformationCallback, modelAdjustmentCallback);
	return NonLinearOptimization::denseOptimization<UniversalOptimizationProvider>(provider, iterations, estimator, lambda, lambdaFactor, initialError, finalError, nullptr, intermediateErrors);
}

}

}

#endif // META_OCEAN_GEOMETRY_NON_LINEAR_UNIVERSAL_OPTIMIZATION_H
