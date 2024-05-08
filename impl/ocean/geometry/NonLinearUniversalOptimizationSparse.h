/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_NON_LINEAR_UNIVERSAL_OPTIMIZATION_SPARSE_H
#define META_OCEAN_GEOMETRY_NON_LINEAR_UNIVERSAL_OPTIMIZATION_SPARSE_H

#include "ocean/geometry/Geometry.h"
#include "ocean/geometry/NonLinearOptimization.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements optimizations for universal sparse problems.
 * @ingroup geometry
 */
class NonLinearUniversalOptimizationSparse : protected NonLinearOptimization
{
	public:

		/**
		 * This class implements an optimization for universal sparse problems with one shared model (optimization problem) and concurrently several individual models (optimization problems).
		 * The implementation allows to optimize arbitrary (universal) problems with arbitrary dimensions.
		 * @tparam tSharedModelSize Size of the shared model, the number of model parameters
		 * @tparam tIndividualModelSize Size of the individual model, the number of model parameters
		 * @tparam tResultDimension Number of dimensions that result for each element (measurement) after the model has been applied
		 * @tparam tExternalSharedModelSize Size of the external shared model, the number of model parameters
		 * @tparam tExternalIndividualModelSize Size of the external individual model, the number of model parameters
		 */
		template <unsigned int tSharedModelSize, unsigned int tIndividualModelSize, unsigned int tResultDimension, unsigned int tExternalSharedModelSize = tSharedModelSize, unsigned int tExternalIndividualModelSize = tIndividualModelSize>
		class SharedModelIndividualModels
		{
			public:

				/**
				 * Definition of a shared model.
				 */
				typedef StaticBuffer<Scalar, tSharedModelSize> SharedModel;

				/**
				 * Definition of an external shared model.
				 */
				typedef StaticBuffer<Scalar, tExternalSharedModelSize> ExternalSharedModel;

				/**
				 * Definition of an individual model.
				 */
				typedef StaticBuffer<Scalar, tIndividualModelSize> IndividualModel;

				/**
				 * Definition of an external individual model.
				 */
				typedef StaticBuffer<Scalar, tExternalIndividualModelSize> ExternalIndividualModel;

				/**
				 * Definition of a model result.
				 */
				typedef StaticBuffer<Scalar, tResultDimension> Result;

				/**
				 * Definition of a vector holding individual models.
				 */
				typedef std::vector<IndividualModel> IndividualModels;

				/**
				 * Definition of a callback function for sparse value calculation.
				 * The first parameter provides the shared model that is applied to determine the value.<br>
				 * The second parameter provides the individual model that is applied to determine the value.<br>
				 * The third parameter provides the index of the individual models that is used to determine the value.<br>
				 * The fourth parameter provides the element index for the individual model.<br>
				 * The fifth parameter receives the determined value.
				 */
				typedef Callback<void, const ExternalSharedModel&, const ExternalIndividualModel&, const size_t, const size_t, Result&> ValueCallback;

				/**
				 * Definition of a callback function for sparse error calculation.
				 * The first parameter provides the shared model that is applied to determine the value.<br>
				 * The second parameter provides the individual model that is applied to determine the value.<br>
				 * The third parameter provides the index of the individual models that is used to determine the error.<br>
				 * The fourth parameter provides the element index for the individual model.<br>
				 * The fifth parameter receives the determined error.<br>
				 * The return value provides True if both models provide valid information for the measurement element.<br>
				 */
				typedef Callback<bool, const ExternalSharedModel&, const ExternalIndividualModel&, const size_t, const size_t, Result&> ErrorCallback;

				/**
				 * Definition of a callback function determining whether a shared model is valid.
				 * The first parameter provides the external shared model for which the decision has to be done
				 * True, if the shared model is valid.<br>
				 */
				typedef Callback<bool, const ExternalSharedModel&> SharedModelIsValidCallback;

				/**
				 * Definition of a shared model transformation function.
				 * The transformation function allows to use an external model function for value and error determination while the internal model is used for the internal optimization.<br>
				 * The first parameter provides the internal shared model.<br>
				 * The second parameter receives the external shared model.<br>
				 */
				typedef Callback<void, SharedModel&, ExternalSharedModel&> SharedModelTransformationCallback;

				/**
				 * Definition of an individual model transformation function.
				 * The transformation function allows to use an external model function for value and error determination while the internal model is used for the internal optimization.<br>
				 * The first parameter provides the internal shared model.<br>
				 * The second parameter receives the external shared model.<br>
				 */
				typedef Callback<void, IndividualModel&, ExternalIndividualModel&> IndividualModelTransformationCallback;

				/**
				 * Definition of a model accepted function.
				 * The first parameter provides the internal shared model that has been accepted as improved model.
				 * The second parameter provides the internal individual models that have been accepted as improved model.
				 */
				typedef Callback<void, const SharedModel&, const IndividualModels&> ModelAcceptedCallback;

			protected:

				/**
				 * Definition of a vector holding individual models.
				 */
				typedef std::vector<ExternalIndividualModel> ExternalIndividualModels;

				/**
				 * This class implements a sparse universal optimization provider for universal models and measurement/data values.
				 */
				class UniversalOptimizationProvider : public NonLinearOptimization::OptimizationProvider
				{
					public:

						/**
						 * Creates a new universal optimization object.
						 * @param sharedModel Shared model to be optimized
						 * @param individualModels Individual models to be optimized
						 * @param numberElementsPerIndividualModel Number of elements (measurements) that are used to determine the optimized model
						 * @param valueCallback Callback function that is used to determine the value for an individual element (measurement) by application of the model
						 * @param errorCallback Callback function that is used to determine the error for an individual element (measurement)
						 * @param sharedModelIsValidCallback Optional callback function that allows to decide whether a shared model is valid
						 * @param sharedModelTransformationCallback Callback function allowing to transform the internal shared model into an extern shared model
						 * @param individualModelTransformationCallback Callback function allowing to transform the internal individual model into an external individual model
						 * @param modelAcceptedCallback Optional callback function that allows to be informed whenever the internal models has been improved
						 */
						inline UniversalOptimizationProvider(SharedModel& sharedModel, IndividualModels& individualModels, const size_t* numberElementsPerIndividualModel, const ValueCallback& valueCallback, const ErrorCallback& errorCallback, const SharedModelIsValidCallback& sharedModelIsValidCallback, const SharedModelTransformationCallback& sharedModelTransformationCallback, const IndividualModelTransformationCallback& individualModelTransformationCallback, const ModelAcceptedCallback& modelAcceptedCallback = ModelAcceptedCallback());

						/**
						 * Determines the jacobian matrix for the current model.
						 * @param jacobian Jacobian matrix
						 */
						void determineJacobian(SparseMatrix& jacobian) const;

						/**
						 * Applies the model correction and stores the new model(s) as candidate.
						 * @param deltas Optimization deltas that define the correction
						 */
						inline void applyCorrection(const Matrix& deltas);

						/**
						 * Determines the robust error of the current candidate model(s).
						 * @param weightedErrorVector Resulting (weighted - if using a robust estimator) error vector
						 * @param weightVector Vector holding the weights that have already been applied to the error vector
						 * @param invertedCovariances Optional 2x2 inverted covariance matrices
						 * @return The resulting robust error
						 * @tparam tEstimator The type of the estimator that is applied for error determination
						 */
						template <Estimator::EstimatorType tEstimator>
						Scalar determineRobustError(Matrix& weightedErrorVector, Matrix& weightVector, const Matrix* invertedCovariances) const;

						/**
						 * Accepts the current model candidate as better model.
						 */
						inline void acceptCorrection();

					protected:

						/// Universal shared model that will be optimized.
						SharedModel& sharedModel_;

						/// Universal individual model that will be optimized.
						IndividualModels& individualModels_;

						/// Universal shared model that stores the most recent optimization result as candidate.
						mutable SharedModel candidateSharedModel_;

						/// Universal individual model that stores the most recent optimization result as candidate.
						mutable IndividualModels candidateIndividualModels_;

						/// The number of measurement elements that are used to optimize each individual model.
						const size_t* numberElementsPerIndividualModel_ = nullptr;

						/// The overall number of measurement elements that are used to optimize the models.
						size_t overallNumberElements_ = 0;

						/// The value calculation callback function.
						const ValueCallback valueCallback_;

						/// The error calculation callback function.
						const ErrorCallback errorCallback_;

						/// The callback function determining whether a shared model is valid.
						const SharedModelIsValidCallback sharedModelIsValidCallback_;

						/// The callback function allowing to transform the shared model into an external model before the value and error callback functions are invoked.
						const SharedModelTransformationCallback sharedModelTransformationCallback_;

						/// The callback function allowing to transform the individual model into an external model before the value and error callback functions are invoked.
						const IndividualModelTransformationCallback individualModelTransformationCallback_;

						/// Optional callback function allowing to be informed whenever the model has been improved.
						const ModelAcceptedCallback modelAcceptedCallback_;
				};

			public:

				/**
				 * Optimizes a universal model by minimizing the error the model produces.
				 * @param sharedModel Shared model that will be optimized
				 * @param individualModels Individual models that will be optimized
				 * @param numberElementsPerIndividualModel The numbers of measurement elements individually for each individual model
				 * @param valueCallback Callback function that is used to determine the value for an individual element (measurement) by application of the model
				 * @param errorCallback Callback function that is used to determine the error for an individual element (measurement)
				 * @param sharedModelIsValidCallback Optional callback function that allows to decide whether a shared model is valid
				 * @param sharedModelTransformationCallback Callback function allowing to transform the internal shared model into an extern model if intended
				 * @param individualModelTransformationCallback Callback function allowing to transform the internal individual model into an extern model if intended
				 * @param modelAcceptedCallback Optional callback function that allows to be informed whenever the internal models has been improved
				 * @param optimizedSharedModel Resulting optimized shared model
				 * @param optimizedIndividualModels Resulting optimized individual models
				 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
				 * @param estimator Robust error estimator to be used
				 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
				 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
				 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
				 * @param finalError Optional resulting averaged error for the final optimized parameters, in relation to the defined estimator
				 * @param intermediateErrors Optional resulting intermediate (improving) errors
				 * @return True, if the model could be optimized
				 */
				static bool optimizeUniversalModel(const SharedModel& sharedModel, const IndividualModels& individualModels, const size_t* numberElementsPerIndividualModel, const ValueCallback& valueCallback, const ErrorCallback& errorCallback, const SharedModelIsValidCallback& sharedModelIsValidCallback, const SharedModelTransformationCallback& sharedModelTransformationCallback, const IndividualModelTransformationCallback& individualModelTransformationCallback, const ModelAcceptedCallback& modelAcceptedCallback, SharedModel& optimizedSharedModel, IndividualModels& optimizedIndividualModels, const unsigned int iterations = 5u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);
		};

		/**
		 * This class implements an optimization for universal sparse problems with two types of individual models (optimization problems).
		 * The implementation allows to optimize arbitrary (universal) problems with arbitrary dimensions.<br>
		 * @tparam tFirstModelSize Size of the first model, the number of model parameters
		 * @tparam tSecondModelSize Size of the second model, the number of model parameters
		 * @tparam tResultDimension Number of dimensions that result for each element (measurement) after the model has been applied
		 * @tparam tExternalFirstModelSize Size of the external first model, the number of model parameters
		 * @tparam tExternalSecondModelSize Size of the external second model, the number of model parameters
		 */
		template <unsigned int tFirstModelSize, unsigned int tSecondModelSize, unsigned int tResultDimension, unsigned int tExternalFirstModelSize = tFirstModelSize, unsigned int tExternalSecondModelSize = tSecondModelSize>
		class IndividualModelsIndividualModels
		{
			public:

				/**
				 * Definition of the first model.
				 */
				typedef StaticBuffer<Scalar, tFirstModelSize> FirstModel;

				/**
				 * Definition of the external first model.
				 */
				typedef StaticBuffer<Scalar, tExternalFirstModelSize> ExternalFirstModel;

				/**
				 * Definition of the second model.
				 */
				typedef StaticBuffer<Scalar, tSecondModelSize> SecondModel;

				/**
				 * Definition of the external second model.
				 */
				typedef StaticBuffer<Scalar, tExternalSecondModelSize> ExternalSecondModel;

				/**
				 * Definition of a model result.
				 */
				typedef StaticBuffer<Scalar, tResultDimension> Result;

				/**
				 * Definition of a vector holding the first models.
				 */
				typedef std::vector<FirstModel> FirstModels;

				/**
				 * Definition of a vector holding the external first models.
				 */
				typedef std::vector<ExternalFirstModel> ExternalFirstModels;

				/**
				 * Definition of a vector holding the first models.
				 */
				typedef std::vector<SecondModel> SecondModels;

				/**
				 * Definition of a vector holding the external second models.
				 */
				typedef std::vector<ExternalSecondModel> ExternalSecondModels;

				/**
				 * Definition of a callback function for sparse value calculation.
				 * The first parameter provides the external first models that can be used to determine the value.<br>
				 * The second parameter provides the external second models that can be used to determined the value.<br>
				 * The third parameter provides the index of the second model for which the value needs to be determined.<br>
				 * The fourth parameter provides the element index for the second model for which the value needs to be determined.<br>
				 * The fifth parameter receives the determined value.<br>
				 * The return value provides the index of the corresponding first model.<br>
				 */
				typedef Callback<size_t, const ExternalFirstModels&, const ExternalSecondModels&, const size_t, const size_t, Result&> ValueCallback;

				/**
				 * Definition of a callback function for sparse error calculation.
				 * The first parameter provides the external first models that can be used to determine the error.<br>
				 * The second parameter provides the external second models that can be used to determined the error.<br>
				 * The third parameter provides the index of the second model for which the error needs to be determined.<br>
				 * The fourth parameter provides the element index for the second model for which the error needs to be determined.<br>
				 * The fifth parameter receives the determined error.<br>
				 */
				typedef Callback<bool, const ExternalFirstModels&, const ExternalSecondModels&, const size_t, const size_t, Result&> ErrorCallback;

				/**
				 * Definition of a first model transformation function.
				 * The transformation function allows to use an external model for value and error determination while the internal model is used for the internal optimization.<br>
				 * The first parameter provides the internal first model.<br>
				 * The second parameter receives the external first model.<br>
				 */
				typedef Callback<void, FirstModel&, ExternalFirstModel&> FirstModelTransformationCallback;

				/**
				 * Definition of a second model transformation function.
				 * The transformation function allows to use an external model for value and error determination while the internal model is used for the internal optimization.<br>
				 * The first parameter provides the internal second model.<br>
				 * The second parameter receives the external second model.<br>
				 */
				typedef Callback<void, SecondModel&, ExternalSecondModel&> SecondModelTransformationCallback;

				/**
				 * Definition of a model accepted function.
				 * The first parameter provides the internal first models that have been accepted as improved models.
				 * The second parameter provides the internal second models that have been accepted as improved model.
				 */
				typedef Callback<void, const FirstModels&, const SecondModels&> ModelAcceptedCallback;

			protected:

				/**
				 * This class implements a sparse universal optimization provider for universal models and measurement/data values.
				 */
				class UniversalOptimizationProvider : public NonLinearOptimization::OptimizationProvider
				{
					public:

						/**
						 * Creates a new universal optimization object.
						 * @param firstModels The first models to be optimized
						 * @param secondModels The second models to be optimized
						 * @param numberElementsPerSecondModel Number of elements (measurements) per second model that are used to determine the optimized models, one number for each second model, with range [1, infinity)
						 * @param valueCallback Callback function that is used to determine the value for an individual element (measurement) by application of the models
						 * @param errorCallback Callback function that is used to determine the error for an individual element (measurement)
						 * @param firstModelTransformationCallback Callback function allowing to transform the internal first model into an extern first model
						 * @param secondModelTransformationCallback Callback function allowing to transform the internal second model into an external second model
						 * @param modelAcceptedCallback Optional callback function that allows to be informed whenever the internal models has been improved
						 */
						inline UniversalOptimizationProvider(FirstModels& firstModels, SecondModels& secondModels, const size_t* numberElementsPerSecondModel, const ValueCallback& valueCallback, const ErrorCallback& errorCallback, const FirstModelTransformationCallback& firstModelTransformationCallback, const SecondModelTransformationCallback& secondModelTransformationCallback, const ModelAcceptedCallback& modelAcceptedCallback = ModelAcceptedCallback());

						/**
						 * Returns that this provider comes with an own equation solver.
						 * @return True, as this provider has an own solver
						 */
						inline bool hasSolver() const;

						/**
						 * Solves the equation JTJ * deltas = jErrors
						 * @param JTJ The JTJ matrix
						 * @param jErrors The jErrors vector
						 * @param deltas The deltas vector
						 * @return True, if succeeded
						 */
						inline bool solve(const SparseMatrix& JTJ, const Matrix& jErrors, Matrix& deltas) const;

						/**
						 * Determines the jacobian matrix for the current model.
						 * @param jacobian Jacobian matrix
						 */
						void determineJacobian(SparseMatrix& jacobian) const;

						/**
						 * Applies the model correction and stores the new model(s) as candidate.
						 * @param deltas Optimization deltas that define the correction
						 */
						inline void applyCorrection(const Matrix& deltas);

						/**
						 * Determines the robust error of the current candidate model(s).
						 * @param weightedErrorVector Resulting (weighted - if using a robust estimator) error vector
						 * @param weightVector Vector holding the weights that have already been applied to the error vector
						 * @param invertedCovariances Optional 2x2 inverted covariance matrices
						 * @return The resulting robust error
						 * @tparam tEstimator The type of the estimator that is applied for error determination
						 */
						template <Estimator::EstimatorType tEstimator>
						Scalar determineRobustError(Matrix& weightedErrorVector, Matrix& weightVector, const Matrix* invertedCovariances) const;

						/**
						 * Accepts the current model candidate as better model.
						 */
						inline void acceptCorrection();

					protected:

						/// The universal first models that will be optimized.
						FirstModels& firstModels_;

						/// The universal second models that will be optimized.
						SecondModels& secondModels_;

						/// The universal first models storing the most recent optimization results as candidates.
						mutable FirstModels candidateFirstModels_;

						/// The universal second models storing the most recent optimization results as candidates.
						mutable SecondModels candidateSecondModels_;

						/// The number of measurement elements for each second model.
						const size_t* numberElementsPerSecondModel_ = nullptr;

						/// The overall number of measurement elements that are used to optimize the models.
						size_t overallNumberElements_ = 0;

						/// The value calculation callback function.
						const ValueCallback valueCallback_;

						/// The error calculation callback function.
						const ErrorCallback errorCallback_;

						/// The callback function allowing to transform the first model into an external model before the value and error callback functions are invoked.
						const FirstModelTransformationCallback firstModelTransformationCallback_;

						/// The callback function allowing to transform the second model into an external model before the value and error callback functions are invoked.
						const SecondModelTransformationCallback secondModelTransformationCallback_;

						/// Optional callback function allowing to be informed whenever the model has been improved.
						const ModelAcceptedCallback modelAcceptedCallback_;
				};

			public:

				/**
				 * Optimizes a universal model by minimizing the error the model produces.
				 * @param firstModels The first models that will be optimized
				 * @param secondModels The second models that will be optimized
				 * @param numberElementsPerSecondModel Number of elements (measurements) per second model that are used to determine the optimized models, one number for each second model, with range [1, infinity)
				 * @param valueCallback Callback function that is used to determine the value for an individual element (measurement) by application of the model(s)
				 * @param errorCallback Callback function that is used to determine the error for an individual element (measurement)
				 * @param firstModelTransformationCallback Callback function allowing to transform the internal first model into an extern model if intended
				 * @param secondModelTransformationCallback Callback function allowing to transform the internal second model into an extern model if intended
				 * @param modelAcceptedCallback Optional callback function that allows to be informed whenever the internal models has been improved
				 * @param optimizedFirstModels Resulting optimized first models
				 * @param optimizedSecondModels Resulting optimized second models
				 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
				 * @param estimator Robust error estimator to be used
				 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
				 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
				 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
				 * @param finalError Optional resulting averaged error for the final optimized parameters, in relation to the defined estimator
				 * @param intermediateErrors Optional resulting intermediate (improving) errors
				 * @return True, if the model could be optimized
				 */
				static bool optimizeUniversalModel(const FirstModels& firstModels, const SecondModels& secondModels, const size_t* numberElementsPerSecondModel, const ValueCallback& valueCallback, const ErrorCallback& errorCallback, const FirstModelTransformationCallback& firstModelTransformationCallback, const SecondModelTransformationCallback& secondModelTransformationCallback, const ModelAcceptedCallback& modelAcceptedCallback, FirstModels& optimizedFirstModels, SecondModels& optimizedSecondModels, const unsigned int iterations = 5u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);
		};

		/**
		 * This class implements an optimization for universal sparse problems with one common shared model (optimization problem) and two types of individual models (optimization problems).
		 * The implementation allows to optimize arbitrary (universal) problems with arbitrary dimensions.<br>
		 * @tparam tSharedModelSize Size of the shared model, the number of model parameters
		 * @tparam tFirstIndividualModelSize Size of the first individual model, the number of model parameters
		 * @tparam tSecondIndividualModelSize Size of the second individual model, the number of model parameters
		 * @tparam tResultDimension Number of dimensions that result for each element (measurement) after the model has been applied
		 * @tparam tExternalSharedModelSize Size of the external shared model, the number of model parameters
		 * @tparam tExternalFirstIndividualModelSize Size of the external first individual model, the number of model parameters
		 * @tparam tExternalSecondIndividualModelSize Size of the external second individual model, the number of model parameters
		 */
		template <unsigned int tSharedModelSize, unsigned int tFirstIndividualModelSize, unsigned int tSecondIndividualModelSize, unsigned int tResultDimension, unsigned int tExternalSharedModelSize = tSharedModelSize, unsigned int tExternalFirstIndividualModelSize = tFirstIndividualModelSize, unsigned int tExternalSecondIndividualModelSize = tSecondIndividualModelSize>
		class SharedModelIndividualModelsIndividualModels
		{
			public:

				/**
				 * Definition of the shared model.
				 */
				typedef StaticBuffer<Scalar, tSharedModelSize> SharedModel;

				/**
				 * Definition of the external shared model.
				 */
				typedef StaticBuffer<Scalar, tExternalSharedModelSize> ExternalSharedModel;

				/**
				 * Definition of the first individual model.
				 */
				typedef StaticBuffer<Scalar, tFirstIndividualModelSize> FirstIndividualModel;

				/**
				 * Definition of the external first individual model.
				 */
				typedef StaticBuffer<Scalar, tExternalFirstIndividualModelSize> ExternalFirstIndividualModel;

				/**
				 * Definition of the second individual model.
				 */
				typedef StaticBuffer<Scalar, tSecondIndividualModelSize> SecondIndividualModel;

				/**
				 * Definition of the external second individual model.
				 */
				typedef StaticBuffer<Scalar, tExternalSecondIndividualModelSize> ExternalSecondIndividualModel;

				/**
				 * Definition of a model result.
				 */
				typedef StaticBuffer<Scalar, tResultDimension> Result;

				/**
				 * Definition of a vector holding the first individual models.
				 */
				typedef std::vector<FirstIndividualModel> FirstIndividualModels;

				/**
				 * Definition of a vector holding the external first individual models.
				 */
				typedef std::vector<ExternalFirstIndividualModel> ExternalFirstIndividualModels;

				/**
				 * Definition of a vector holding the first individual models.
				 */
				typedef std::vector<SecondIndividualModel> SecondIndividualModels;

				/**
				 * Definition of a vector holding the external second individual models.
				 */
				typedef std::vector<ExternalSecondIndividualModel> ExternalSecondIndividualModels;

				/**
				 * Definition of a callback function for sparse value calculation.
				 * The first parameter provides the external shared model that will be used to determined the value.<br>
				 * The second parameter provides the external first individual models that can be used to determine the value.<br>
				 * The third parameter provides the external second individual models that can be used to determined the value.<br>
				 * The fourth parameter provides the index of the second model for which the value needs to be determined.<br>
				 * The fifth parameter provides the element index for the second model for which the value needs to be determined.<br>
				 * The sixth parameter receives the determined value.<br>
				 * The return value provides the index of the corresponding first model.<br>
				 */
				typedef Callback<size_t, const ExternalSharedModel&, const ExternalFirstIndividualModels&, const ExternalSecondIndividualModels&, const size_t, const size_t, Result&> ValueCallback;

				/**
				 * Definition of a callback function for sparse error calculation.
				 * The first parameter provides the external shared model that will be used to determined the error.<br>
				 * The second parameter provides the external first individual models that can be used to determine the error.<br>
				 * The third parameter provides the external second individual models that can be used to determined the error.<br>
				 * The fourth parameter provides the index of the second model for which the error needs to be determined.<br>
				 * The fifth parameter provides the element index for the second model for which the error needs to be determined.<br>
				 * The sixth parameter receives the determined error.<br>
				 */
				typedef Callback<bool, const ExternalSharedModel&, const ExternalFirstIndividualModels&, const ExternalSecondIndividualModels&, const size_t, const size_t, Result&> ErrorCallback;

				/**
				 * Definition of a callback function determining whether a shared model is valid.
				 * The first parameter provides the external shared model for which the decision has to be done
				 * True, if the shared model is valid.<br>
				 */
				typedef Callback<bool, const ExternalSharedModel&> SharedModelIsValidCallback;

				/**
				 * Definition of a transformation function for the shared model.
				 * The transformation function allows to use an external model for value and error determination while the internal model is used for the internal optimization.<br>
				 * The first parameter provides the internal shared model.<br>
				 * The second parameter receives the external shared model.<br>
				 */
				typedef Callback<void, SharedModel&, ExternalSharedModel&> SharedModelTransformationCallback;

				/**
				 * Definition of a transformation function for the first individual models.
				 * The transformation function allows to use an external model for value and error determination while the internal model is used for the internal optimization.<br>
				 * The first parameter provides the internal first individual model.<br>
				 * The second parameter receives the external first individual model.<br>
				 */
				typedef Callback<void, FirstIndividualModel&, ExternalFirstIndividualModel&> FirstIndividualModelTransformationCallback;

				/**
				 * Definition of a transformation function for the second individual models.
				 * The transformation function allows to use an external model for value and error determination while the internal model is used for the internal optimization.<br>
				 * The first parameter provides the internal second individual model.<br>
				 * The second parameter receives the external second individual model.<br>
				 */
				typedef Callback<void, SecondIndividualModel&, ExternalSecondIndividualModel&> SecondIndividualModelTransformationCallback;

				/**
				 * Definition of a model accepted function.
				 * The first parameter provides the internal shared model that have been accepted as improved model.
				 * The second parameter provides the internal first individual models that have been accepted as improved models.
				 * The third parameter provides the internal second individual models that have been accepted as improved model.
				 */
				typedef Callback<void, const SharedModel&, const FirstIndividualModels&, const SecondIndividualModels&> ModelAcceptedCallback;

			protected:

				/**
				 * This class implements a sparse universal optimization provider for universal models and measurement/data values.
				 */
				class UniversalOptimizationProvider : public NonLinearOptimization::OptimizationProvider
				{
					public:

						/**
						 * Creates a new universal optimization object.
						 * @param sharedModel The shared model to be optimized
						 * @param firstIndividualModels The first individual models to be optimized
						 * @param secondIndividualModels The second individual models to be optimized
						 * @param numberElementsPerSecondModel Number of elements (measurements) per second model that are used to determine the optimized models, one number for each second model, with range [1, infinity)
						 * @param valueCallback Callback function that is used to determine the value for an individual element (measurement) by application of the models
						 * @param errorCallback Callback function that is used to determine the error for an individual element (measurement)
						 * @param sharedModelIsValidCallback Optional callback function that allows to decide whether a shared model is valid
						 * @param sharedModelTransformationCallback Callback function allowing to transform the internal shared model into an external shared model
						 * @param firstIndividualModelTransformationCallback Callback function allowing to transform the internal first model into an extern first model
						 * @param secondIndividualModelTransformationCallback Callback function allowing to transform the internal second model into an external second model
						 * @param modelAcceptedCallback Optional callback function that allows to be informed whenever the internal models has been improved
						 */
						inline UniversalOptimizationProvider(SharedModel& sharedModel, FirstIndividualModels& firstIndividualModels, SecondIndividualModels& secondIndividualModels, const size_t* numberElementsPerSecondModel, const ValueCallback& valueCallback, const ErrorCallback& errorCallback, const SharedModelIsValidCallback& sharedModelIsValidCallback, const SharedModelTransformationCallback& sharedModelTransformationCallback, const FirstIndividualModelTransformationCallback& firstIndividualModelTransformationCallback, const SecondIndividualModelTransformationCallback& secondIndividualModelTransformationCallback, const ModelAcceptedCallback& modelAcceptedCallback = ModelAcceptedCallback());

						/**
						 * Determines the jacobian matrix for the current model.
						 * @param jacobian Jacobian matrix
						 */
						void determineJacobian(SparseMatrix& jacobian) const;

						/**
						 * Applies the model correction and stores the new model(s) as candidate.
						 * @param deltas Optimization deltas that define the correction
						 */
						inline void applyCorrection(const Matrix& deltas);

						/**
						 * Determines the robust error of the current candidate model(s).
						 * @param weightedErrorVector Resulting (weighted - if using a robust estimator) error vector
						 * @param weightVector Vector holding the weights that have already been applied to the error vector
						 * @param invertedCovariances Optional 2x2 inverted covariance matrices
						 * @return The resulting robust error
						 * @tparam tEstimator The type of the estimator that is applied for error determination
						 */
						template <Estimator::EstimatorType tEstimator>
						Scalar determineRobustError(Matrix& weightedErrorVector, Matrix& weightVector, const Matrix* invertedCovariances) const;

						/**
						 * Accepts the current model candidate as better model.
						 */
						inline void acceptCorrection();

					protected:

						/// The universal shared model that will be optimized.
						SharedModel& sharedModel_;

						/// The universal first individual models that will be optimized.
						FirstIndividualModels& firstIndividualModels_;

						/// The universal second individual models that will be optimized.
						SecondIndividualModels& secondIndividualModels_;

						/// The universal shared model storing the most recent optimization result as candidate.
						mutable SharedModel candidateSharedModel_;

						/// The universal first individual models storing the most recent optimization results as candidates.
						mutable FirstIndividualModels candidateFirstIndividualModels_;

						/// The universal second individual models storing the most recent optimization results as candidates.
						mutable SecondIndividualModels candidateSecondIndividualModels_;

						/// The number of measurement elements for each second model.
						const size_t* numberElementsPerSecondModel_ = nullptr;

						/// The overall number of measurement elements that are used to optimize the models.
						size_t overallNumberElements_ = 0;

						/// The value calculation callback function.
						const ValueCallback valueCallback_;

						/// The error calculation callback function.
						const ErrorCallback errorCallback_;

						/// The callback function determining whether a shared model is valid.
						const SharedModelIsValidCallback sharedModelIsValidCallback_;

						/// The callback function allowing to transform the shared model into an external model before the value and error callback functions are invoked.
						const SharedModelTransformationCallback sharedModelTransformationCallback_;

						/// The callback function allowing to transform the first individual model into an external model before the value and error callback functions are invoked.
						const FirstIndividualModelTransformationCallback firstIndividualModelTransformationCallback_;

						/// The callback function allowing to transform the second model into an external model before the value and error callback functions are invoked.
						const SecondIndividualModelTransformationCallback secondIndividualModelTransformationCallback_;

						/// Optional callback function allowing to be informed whenever the model has been improved.
						const ModelAcceptedCallback modelAcceptedCallback_;
				};

			public:

				/**
				 * Optimizes a universal model by minimizing the error the model produces.
				 * @param sharedModel The shared model that will be optimized
				 * @param firstIndividualModels The first individual models that will be optimized
				 * @param secondIndividualModels The second individual models that will be optimized
				 * @param numberElementsPerSecondModel Number of elements (measurements) per second model that are used to determine the optimized models, one number for each second model, with range [1, infinity)
				 * @param valueCallback Callback function that is used to determine the value for an individual element (measurement) by application of the model(s)
				 * @param errorCallback Callback function that is used to determine the error for an individual element (measurement)
				 * @param sharedModelIsValidCallback Optional callback function that allows to decide whether a shared model is valid
				 * @param sharedModelTransformationCallback Callback function allowing to transform the internal shared model into an external model if intended
				 * @param firstIndividualModelTransformationCallback Callback function allowing to transform the internal first individual model into an external model if intended
				 * @param secondIndividualModelTransformationCallback Callback function allowing to transform the internal second individual model into an external model if intended
				 * @param modelAcceptedCallback Optional callback function that allows to be informed whenever the internal models has been improved
				 * @param optimizedSharedModel Resulting optimized shared model
				 * @param optimizedFirstIndividualModels Resulting optimized first individual models
				 * @param optimizedSecondIndividualModels Resulting optimized second individual models
				 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
				 * @param estimator Robust error estimator to be used
				 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
				 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
				 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
				 * @param finalError Optional resulting averaged error for the final optimized parameters, in relation to the defined estimator
				 * @param intermediateErrors Optional resulting intermediate (improving) errors
				 * @return True, if the model could be optimized
				 */
				static bool optimizeUniversalModel(const SharedModel& sharedModel, const FirstIndividualModels& firstIndividualModels, const SecondIndividualModels& secondIndividualModels, const size_t* numberElementsPerSecondModel, const ValueCallback& valueCallback, const ErrorCallback& errorCallback, const SharedModelIsValidCallback& sharedModelIsValidCallback, const SharedModelTransformationCallback& sharedModelTransformationCallback, const FirstIndividualModelTransformationCallback& firstIndividualModelTransformationCallback, const SecondIndividualModelTransformationCallback& secondIndividualModelTransformationCallback, const ModelAcceptedCallback& modelAcceptedCallback, SharedModel& optimizedSharedModel, FirstIndividualModels& optimizedFirstIndividualModels, SecondIndividualModels& optimizedSecondIndividualModels, const unsigned int iterations = 5u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);
		};
};

template <unsigned int tSharedModelSize, unsigned int tIndividualModelSize, unsigned int tResultDimension, unsigned int tExternalSharedModelSize, unsigned int tExternalIndividualModelSize>
inline NonLinearUniversalOptimizationSparse::SharedModelIndividualModels<tSharedModelSize, tIndividualModelSize, tResultDimension, tExternalSharedModelSize, tExternalIndividualModelSize>::UniversalOptimizationProvider::UniversalOptimizationProvider(SharedModel& sharedModel, IndividualModels& individualModels, const size_t* numberElementsPerIndividualModel, const ValueCallback& valueCallback, const ErrorCallback& errorCallback, const SharedModelIsValidCallback& sharedModelIsValidCallback, const SharedModelTransformationCallback& sharedModelTransformationCallback, const IndividualModelTransformationCallback& individualModelTransformationCallback, const ModelAcceptedCallback& modelAcceptedCallback) :
	sharedModel_(sharedModel),
	individualModels_(individualModels),
	numberElementsPerIndividualModel_(numberElementsPerIndividualModel),
	overallNumberElements_(0),
	valueCallback_(valueCallback),
	errorCallback_(errorCallback),
	sharedModelIsValidCallback_(sharedModelIsValidCallback),
	sharedModelTransformationCallback_(sharedModelTransformationCallback),
	individualModelTransformationCallback_(individualModelTransformationCallback),
	modelAcceptedCallback_(modelAcceptedCallback)
{
	ocean_assert(valueCallback_);
	ocean_assert(errorCallback_);
	ocean_assert(sharedModelTransformationCallback_);
	ocean_assert(individualModelTransformationCallback_);

	candidateSharedModel_ = sharedModel;
	candidateIndividualModels_ = individualModels;

	for (size_t n = 0; n < individualModels.size(); ++n)
	{
		overallNumberElements_ += numberElementsPerIndividualModel_[n];
	}
};

template <unsigned int tSharedModelSize, unsigned int tIndividualModelSize, unsigned int tResultDimension, unsigned int tExternalSharedModelSize, unsigned int tExternalIndividualModelSize>
void NonLinearUniversalOptimizationSparse::SharedModelIndividualModels<tSharedModelSize, tIndividualModelSize, tResultDimension, tExternalSharedModelSize, tExternalIndividualModelSize>::UniversalOptimizationProvider::determineJacobian(SparseMatrix& jacobian) const
{
	ocean_assert(valueCallback_);
	ocean_assert(sharedModelTransformationCallback_);
	ocean_assert(individualModelTransformationCallback_);

	ocean_assert(overallNumberElements_ != 0);

	SparseMatrix::Entries jacobianEntries;
	jacobianEntries.reserve(tResultDimension * overallNumberElements_ * (tSharedModelSize + tIndividualModelSize));

	const Scalar eps = Numeric::weakEps();
	const Scalar invEps = Scalar(1) / eps;

	// transform the internal shared to the external shared model
	ExternalSharedModel externalSharedModel;
	sharedModelTransformationCallback_(sharedModel_, externalSharedModel);

	// stores individual models, each model with one individual epsilon offset
	StaticBuffer<ExternalSharedModel, tSharedModelSize> externalEpsSharedModels;
	for (size_t a = 0; a < tSharedModelSize; ++a)
	{
		SharedModel internalModel = sharedModel_;
		internalModel[a] += eps;

		sharedModelTransformationCallback_(internalModel, externalEpsSharedModels[a]);
	}

	// transform the internal individual to the external individual models
	std::vector<ExternalIndividualModel> externalIndividualModels(individualModels_.size());
	std::vector< StaticBuffer<ExternalIndividualModel, tIndividualModelSize> > externalEpsIndividualModels(individualModels_.size());

	for (size_t i = 0; i < individualModels_.size(); ++i)
	{
		individualModelTransformationCallback_(individualModels_[i], externalIndividualModels[i]);

		for (size_t a = 0; a < tIndividualModelSize; ++a)
		{
			IndividualModel internalModel = individualModels_[i];
			internalModel[a] += eps;

			individualModelTransformationCallback_(internalModel, externalEpsIndividualModels[i][a]);
		}
	}

	Result result, epsResult;
	size_t row = 0;

	StaticBuffer<Scalar, tSharedModelSize * tResultDimension> sharedModelResults;
	StaticBuffer<Scalar, tIndividualModelSize * tResultDimension> individualModelResults;

	for (size_t i = 0; i < individualModels_.size(); ++i)
	{
		const size_t numberElements = numberElementsPerIndividualModel_[i];
		const size_t columnIndividual = tSharedModelSize + i * tIndividualModelSize;

		for (size_t n = 0; n < numberElements; ++n)
		{
			// calculate the value for the current model
			valueCallback_(externalSharedModel, externalIndividualModels[i], i, n, result);

			// shared model
			for (size_t m = 0; m < tSharedModelSize; ++m)
			{
				// calculate the value for the epsilon model
				valueCallback_(externalEpsSharedModels[m], externalIndividualModels[i], i, n, epsResult);

				// store the individual results
				for (size_t d = 0; d < tResultDimension; ++d)
				{
					sharedModelResults[d * tSharedModelSize + m] = (epsResult[d] - result[d]) * invEps;
				}
			}

			// individual model
			for (size_t m = 0; m < tIndividualModelSize; ++m)
			{
				// calculate the value for the epsilon model
				valueCallback_(externalSharedModel, externalEpsIndividualModels[i][m], i, n, epsResult);

				// store the individual results
				for (size_t d = 0; d < tResultDimension; ++d)
				{
					individualModelResults[d * tIndividualModelSize + m] = (epsResult[d] - result[d]) * invEps;
				}
			}

			for (size_t d = 0; d < tResultDimension; ++d)
			{
				// .insert(row, 0u, sharedModelResults.data() + d * tSharedModelSize, tSharedModelSize);
				for (size_t e = 0; e < tSharedModelSize; ++e)
				{
					jacobianEntries.emplace_back(row, e, sharedModelResults.data()[d * tSharedModelSize + e]);
				}

				// .insertBack(row, columnIndividual, individualModelResults.data() + d * tIndividualModelSize, tIndividualModelSize);
				for (size_t e = 0; e < tIndividualModelSize; ++e)
				{
					jacobianEntries.emplace_back(row, columnIndividual + e, individualModelResults.data()[d * tIndividualModelSize + e]);
				}

				row++;
			}
		}
	}

	jacobian = SparseMatrix(tResultDimension * overallNumberElements_, tSharedModelSize + tIndividualModelSize * individualModels_.size(), jacobianEntries);
	ocean_assert(SparseMatrix::Entry::hasOneEntry(jacobian.rows(), jacobian.columns(), jacobianEntries));
	ocean_assert(row == jacobian.rows());
}

template <unsigned int tSharedModelSize, unsigned int tIndividualModelSize, unsigned int tResultDimension, unsigned int tExternalSharedModelSize, unsigned int tExternalIndividualModelSize>
inline void NonLinearUniversalOptimizationSparse::SharedModelIndividualModels<tSharedModelSize, tIndividualModelSize, tResultDimension, tExternalSharedModelSize, tExternalIndividualModelSize>::UniversalOptimizationProvider::applyCorrection(const Matrix& deltas)
{
	ocean_assert(deltas.rows() == tSharedModelSize + tIndividualModelSize * individualModels_.size());

	// shared model
	for (size_t m = 0; m < tSharedModelSize; ++m)
	{
		const Scalar& delta = deltas(m);
		candidateSharedModel_[m] = sharedModel_[m] - delta;
	}

	// individual models
	for (size_t i = 0; i < individualModels_.size(); ++i)
	{
		for (size_t m = 0; m < tIndividualModelSize; ++m)
		{
			const Scalar& delta = deltas(tSharedModelSize + i * tIndividualModelSize + m);
			candidateIndividualModels_[i][m] = individualModels_[i][m] - delta;
		}
	}
}

template <unsigned int tSharedModelSize, unsigned int tIndividualModelSize, unsigned int tResultDimension, unsigned int tExternalSharedModelSize, unsigned int tExternalIndividualModelSize>
template <Estimator::EstimatorType tEstimator>
Scalar NonLinearUniversalOptimizationSparse::SharedModelIndividualModels<tSharedModelSize, tIndividualModelSize, tResultDimension, tExternalSharedModelSize, tExternalIndividualModelSize>::UniversalOptimizationProvider::determineRobustError(Matrix& weightedErrorVector, Matrix& weightVector, const Matrix* invertedCovariances) const
{
	ocean_assert(valueCallback_);
	ocean_assert(sharedModelTransformationCallback_);
	ocean_assert(individualModelTransformationCallback_);

	OCEAN_SUPPRESS_UNUSED_WARNING(invertedCovariances);
	ocean_assert(invertedCovariances == nullptr);
	ocean_assert(overallNumberElements_ != 0);

	ExternalSharedModel externalSharedModel;
	sharedModelTransformationCallback_(candidateSharedModel_, externalSharedModel);

	// check whether we can stop here as we do not have a valid shared model (and the provider supports to decide that)
	if (sharedModelIsValidCallback_ && !sharedModelIsValidCallback_(externalSharedModel))
	{
		return Numeric::maxValue();
	}

	// set the correct size of the resulting error vector
	weightedErrorVector.resize(overallNumberElements_ * tResultDimension, 1u);
	Result* const weightedErrors = (Result*)weightedErrorVector.data();

	ExternalIndividualModels externalIndividualModels(individualModels_.size());
	for (size_t i = 0; i < individualModels_.size(); ++i)
	{
		individualModelTransformationCallback_(candidateIndividualModels_[i], externalIndividualModels[i]);
	}

	size_t index = 0;
	Scalar sqrError = 0;
	Scalars sqrErrors;
	if constexpr (!Estimator::isStandardEstimator<tEstimator>())
	{
		sqrErrors.reserve(overallNumberElements_);
	}

	for (size_t i = 0; i < individualModels_.size(); ++i)
	{
		const size_t numberElements = numberElementsPerIndividualModel_[i];

		for (size_t n = 0; n < numberElements; ++n)
		{
			Result& weightedErrorPointer = *((Result*)weightedErrorVector.data() + index);

			if (!errorCallback_(externalSharedModel, externalIndividualModels[i], i, n, weightedErrorPointer))
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

			index++;
		}
	}

	ocean_assert(index == overallNumberElements_);
	ocean_assert(index * tResultDimension == weightedErrorVector.rows());

	// check whether the standard estimator is used
	if constexpr (Estimator::isStandardEstimator<tEstimator>())
	{
		// the weight vector should be and should stay invalid
		ocean_assert(!weightVector);

		ocean_assert((overallNumberElements_) > 0);
		return sqrError /= Scalar(overallNumberElements_);
	}
	else
	{
		// now we need the weight vector
		weightVector.resize(tResultDimension * overallNumberElements_, 1u);

		return sqrErrors2robustErrors<tEstimator, tResultDimension>(sqrErrors, tSharedModelSize + tIndividualModelSize * individualModels_.size(), weightedErrors, (StaticBuffer<Scalar, tResultDimension>*)weightVector.data(), nullptr);
	}
}

template <unsigned int tSharedModelSize, unsigned int tIndividualModelSize, unsigned int tResultDimension, unsigned int tExternalSharedModelSize, unsigned int tExternalIndividualModelSize>
inline void NonLinearUniversalOptimizationSparse::SharedModelIndividualModels<tSharedModelSize, tIndividualModelSize, tResultDimension, tExternalSharedModelSize, tExternalIndividualModelSize>::UniversalOptimizationProvider::acceptCorrection()
{
	sharedModel_ = candidateSharedModel_;
	individualModels_ = candidateIndividualModels_;

	if (modelAcceptedCallback_)
	{
		modelAcceptedCallback_(sharedModel_, individualModels_);
	}
}

template <unsigned int tSharedModelSize, unsigned int tIndividualModelSize, unsigned int tResultDimension, unsigned int tExternalSharedModelSize, unsigned int tExternalIndividualModelSize>
bool NonLinearUniversalOptimizationSparse::SharedModelIndividualModels<tSharedModelSize, tIndividualModelSize, tResultDimension, tExternalSharedModelSize, tExternalIndividualModelSize>::optimizeUniversalModel(const SharedModel& sharedModel, const IndividualModels& individualModels, const size_t* numberElementsPerIndividualModel, const ValueCallback& valueCallback, const ErrorCallback& errorCallback, const SharedModelIsValidCallback& sharedModelIsValidCallback, const SharedModelTransformationCallback& sharedModelTransformationCallback, const IndividualModelTransformationCallback& individualModelTransformationCallback, const ModelAcceptedCallback& modelAcceptedCallback, SharedModel& optimizedSharedModel, IndividualModels& optimizedIndividualModels, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
	ocean_assert(&sharedModel != &optimizedSharedModel);
	ocean_assert(&individualModels != &optimizedIndividualModels);

	optimizedSharedModel = sharedModel;
	optimizedIndividualModels = individualModels;

	if (modelAcceptedCallback)
	{
		modelAcceptedCallback(sharedModel, individualModels);
	}

	UniversalOptimizationProvider provider(optimizedSharedModel, optimizedIndividualModels, numberElementsPerIndividualModel, valueCallback, errorCallback, sharedModelIsValidCallback, sharedModelTransformationCallback, individualModelTransformationCallback, modelAcceptedCallback);
	return NonLinearOptimization::sparseOptimization<UniversalOptimizationProvider>(provider, iterations, estimator, lambda, lambdaFactor, initialError, finalError, nullptr, intermediateErrors);
}

template <unsigned int tFirstModelSize, unsigned int tSecondModelSize, unsigned int tResultDimension, unsigned int tExternalFirstModelSize, unsigned int tExternalSecondModelSize>
inline NonLinearUniversalOptimizationSparse::IndividualModelsIndividualModels<tFirstModelSize, tSecondModelSize, tResultDimension, tExternalFirstModelSize, tExternalSecondModelSize>::UniversalOptimizationProvider::UniversalOptimizationProvider(FirstModels& firstModels, SecondModels& secondModels, const size_t* numberElementsPerSecondModel, const ValueCallback& valueCallback, const ErrorCallback& errorCallback, const FirstModelTransformationCallback& firstModelTransformationCallback, const SecondModelTransformationCallback& secondModelTransformationCallback, const ModelAcceptedCallback& modelAcceptedCallback) :
	firstModels_(firstModels),
	secondModels_(secondModels),
	numberElementsPerSecondModel_(numberElementsPerSecondModel),
	overallNumberElements_(0),
	valueCallback_(valueCallback),
	errorCallback_(errorCallback),
	firstModelTransformationCallback_(firstModelTransformationCallback),
	secondModelTransformationCallback_(secondModelTransformationCallback),
	modelAcceptedCallback_(modelAcceptedCallback)
{
	ocean_assert(valueCallback_);
	ocean_assert(errorCallback_);
	ocean_assert(firstModelTransformationCallback_);
	ocean_assert(secondModelTransformationCallback_);

	candidateFirstModels_ = firstModels;
	candidateSecondModels_ = secondModels;

	// we need to know how many measurement values are provides as this number determines e.g., the size of the jacobian matrix etc.
	for (size_t n = 0; n < secondModels.size(); ++n)
	{
		overallNumberElements_ += numberElementsPerSecondModel_[n];
	}
};

template <unsigned int tFirstModelSize, unsigned int tSecondModelSize, unsigned int tResultDimension, unsigned int tExternalFirstModelSize, unsigned int tExternalSecondModelSize>
inline bool NonLinearUniversalOptimizationSparse::IndividualModelsIndividualModels<tFirstModelSize, tSecondModelSize, tResultDimension, tExternalFirstModelSize, tExternalSecondModelSize>::UniversalOptimizationProvider::hasSolver() const
{
	return true;
}

template <unsigned int tFirstModelSize, unsigned int tSecondModelSize, unsigned int tResultDimension, unsigned int tExternalFirstModelSize, unsigned int tExternalSecondModelSize>
inline bool NonLinearUniversalOptimizationSparse::IndividualModelsIndividualModels<tFirstModelSize, tSecondModelSize, tResultDimension, tExternalFirstModelSize, tExternalSecondModelSize>::UniversalOptimizationProvider::solve(const SparseMatrix& JTJ, const Matrix& jErrors, Matrix& deltas) const
{
	static_assert(tFirstModelSize >= 1u, "Invalid model size!");
	static_assert(tSecondModelSize >= 1u, "Invalid model size!");

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

	const size_t sizeA = firstModels_.size() * tFirstModelSize;
	const size_t sizeB = JTJ.rows() - sizeA;
	ocean_assert(sizeB % tSecondModelSize == 0);

	if (sizeA < sizeB)
	{
		SparseMatrix D(JTJ.submatrix(sizeA, sizeA, sizeB, sizeB));

		switch (tSecondModelSize)
		{
			case 1u:
				if (!D.invertDiagonal())
					return false;
				break;

			case 3u:
				if (!D.invertBlockDiagonal3())
					return false;
				break;

			default:
				if (!D.invertBlockDiagonal(tSecondModelSize))
					return false;
				break;
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

		switch (tFirstModelSize)
		{
			case 1u:
				if (!A.invertDiagonal())
					return false;
				break;

			case 3u:
				if (!A.invertBlockDiagonal3())
					return false;
				break;

			default:
				if (!A.invertBlockDiagonal(tFirstModelSize))
					return false;
				break;
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

#ifdef OCEAN_INTENSIVE_DEBUG
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

	// sometime event the average difference may not be weak-zero so that we do not check the value by default
	// ocean_assert(Numeric::isWeakEqualEps(averageDifference));
#endif

	return true;
}

template <unsigned int tFirstModelSize, unsigned int tSecondModelSize, unsigned int tResultDimension, unsigned int tExternalFirstModelSize, unsigned int tExternalSecondModelSize>
void NonLinearUniversalOptimizationSparse::IndividualModelsIndividualModels<tFirstModelSize, tSecondModelSize, tResultDimension, tExternalFirstModelSize, tExternalSecondModelSize>::UniversalOptimizationProvider::determineJacobian(SparseMatrix& jacobian) const
{
	ocean_assert(valueCallback_);
	ocean_assert(firstModelTransformationCallback_);
	ocean_assert(secondModelTransformationCallback_);

	ocean_assert(overallNumberElements_ != 0);

	SparseMatrix::Entries jacobianEntries;
	jacobianEntries.reserve(tResultDimension * overallNumberElements_ * (tFirstModelSize + tSecondModelSize));

	const Scalar eps = Numeric::weakEps();
	const Scalar invEps = Scalar(1) / eps;

	// for each model (we need to determine a slightly modified epsilon model) so that we can determine the jacobian matrix later
	// for each internal model:
	// - we determine the corresponding external model (without modifying the individual parameters)
	// - we modify each parameter and store the corresponding external sub-models (one external sub-model for each modified internal model parameter)

	// the external (first) models (without modified parameters)
	ExternalFirstModels externalFirstModels(firstModels_.size());
	// the external (first) models (with modified (internal) parameters)
	StaticBuffer<ExternalFirstModels, tFirstModelSize> externalEpsFirstModels(tFirstModelSize, ExternalFirstModels(firstModels_.size()));

	for (size_t i = 0; i < firstModels_.size(); ++i)
	{
		firstModelTransformationCallback_(firstModels_[i], externalFirstModels[i]);

		for (size_t a = 0; a < tFirstModelSize; ++a)
		{
			FirstModel firstModel = firstModels_[i];
			firstModel[a] += eps;

			firstModelTransformationCallback_(firstModel, externalEpsFirstModels[a][i]);
		}
	}

	// the external (second) models (without modified parameters)
	ExternalSecondModels externalSecondModels(secondModels_.size());
	// the external (second models (with modified (internal) parameters)
	StaticBuffer<ExternalSecondModels, tSecondModelSize> externalEpsSecondModels(tSecondModelSize, ExternalSecondModels(secondModels_.size()));

	for (size_t i = 0; i < secondModels_.size(); ++i)
	{
		secondModelTransformationCallback_(secondModels_[i], externalSecondModels[i]);

		for (size_t a = 0; a < tSecondModelSize; ++a)
		{
			SecondModel secondModel = secondModels_[i];
			secondModel[a] += eps;

			secondModelTransformationCallback_(secondModel, externalEpsSecondModels[a][i]);
		}
	}

	// now we apply the individual external models and their corresponding modified external models to determine the jacobian matrix

	Result result, epsResult;
	size_t row = 0;

	StaticBuffer<Scalar, tFirstModelSize * tResultDimension> firstModelResults;
	StaticBuffer<Scalar, tSecondModelSize * tResultDimension> secondModelResults;

	for (size_t i = 0; i < secondModels_.size(); ++i)
	{
		const size_t numberElements = numberElementsPerSecondModel_[i];
		const size_t columnSecond = tFirstModelSize * firstModels_.size() + i * tSecondModelSize;

		for (size_t n = 0; n < numberElements; ++n)
		{
			// calculate the value for the current model
			const size_t firstModelIndex = valueCallback_(externalFirstModels, externalSecondModels, i, n, result);
			ocean_assert(firstModelIndex <= firstModels_.size());

			// first model
			for (size_t m = 0; m < tFirstModelSize; ++m)
			{
				// calculate the value for the epsilon model
				const size_t checkModelIndex = valueCallback_(externalEpsFirstModels[m], externalSecondModels, i, n, epsResult);
				ocean_assert(checkModelIndex == firstModelIndex);
				OCEAN_SUPPRESS_UNUSED_WARNING(checkModelIndex);

				// store the individual results
				for (size_t d = 0; d < tResultDimension; ++d)
				{
					firstModelResults[d * tFirstModelSize + m] = (epsResult[d] - result[d]) * invEps;
				}
			}

			// second model
			for (size_t m = 0; m < tSecondModelSize; ++m)
			{
				// calculate the value for the epsilon model
				const size_t checkModelIndex = valueCallback_(externalFirstModels, externalEpsSecondModels[m], i, n, epsResult);
				ocean_assert(checkModelIndex == firstModelIndex);
				OCEAN_SUPPRESS_UNUSED_WARNING(checkModelIndex);

				// store the individual results
				for (size_t d = 0; d < tResultDimension; ++d)
				{
					secondModelResults[d * tSecondModelSize + m] = (epsResult[d] - result[d]) * invEps;
				}
			}

			const size_t columnFirst = firstModelIndex * tFirstModelSize;

			for (size_t d = 0; d < tResultDimension; ++d)
			{
				for (size_t e = 0; e < tFirstModelSize; ++e)
				{
					jacobianEntries.emplace_back(row, columnFirst + e, firstModelResults.data()[d * tFirstModelSize + e]);
				}

				for (size_t e = 0; e < tSecondModelSize; ++e)
				{
					jacobianEntries.emplace_back(row, columnSecond + e, secondModelResults.data()[d * tSecondModelSize + e]);
				}

				row++;
			}
		}
	}

	jacobian = SparseMatrix(tResultDimension * overallNumberElements_, tFirstModelSize * firstModels_.size() + tSecondModelSize * secondModels_.size(), jacobianEntries);
	ocean_assert(SparseMatrix::Entry::hasOneEntry(jacobian.rows(), jacobian.columns(), jacobianEntries));
	ocean_assert(row == jacobian.rows());
}

template <unsigned int tFirstModelSize, unsigned int tSecondModelSize, unsigned int tResultDimension, unsigned int tExternalFirstModelSize, unsigned int tExternalSecondModelSize>
inline void NonLinearUniversalOptimizationSparse::IndividualModelsIndividualModels<tFirstModelSize, tSecondModelSize, tResultDimension, tExternalFirstModelSize, tExternalSecondModelSize>::UniversalOptimizationProvider::applyCorrection(const Matrix& deltas)
{
	ocean_assert(deltas.rows() == tFirstModelSize * firstModels_.size() + tSecondModelSize * secondModels_.size());

	size_t index = 0;

	// first models
	for (size_t i = 0; i < firstModels_.size(); ++i)
	{
		for (size_t m = 0; m < tFirstModelSize; ++m)
		{
			const Scalar& delta = deltas(index++);
			candidateFirstModels_[i][m] = firstModels_[i][m] - delta;
		}
	}

	// second models
	for (size_t i = 0; i < secondModels_.size(); ++i)
	{
		for (size_t m = 0; m < tSecondModelSize; ++m)
		{
			const Scalar& delta = deltas(index++);
			candidateSecondModels_[i][m] = secondModels_[i][m] - delta;
		}
	}
}

template <unsigned int tFirstModelSize, unsigned int tSecondModelSize, unsigned int tResultDimension, unsigned int tExternalFirstModelSize, unsigned int tExternalSecondModelSize>
template <Estimator::EstimatorType tEstimator>
Scalar NonLinearUniversalOptimizationSparse::IndividualModelsIndividualModels<tFirstModelSize, tSecondModelSize, tResultDimension, tExternalFirstModelSize, tExternalSecondModelSize>::UniversalOptimizationProvider::determineRobustError(Matrix& weightedErrorVector, Matrix& weightVector, const Matrix* invertedCovariances) const
{
	ocean_assert(valueCallback_);
	ocean_assert(firstModelTransformationCallback_);
	ocean_assert(secondModelTransformationCallback_);

	OCEAN_SUPPRESS_UNUSED_WARNING(invertedCovariances);
	ocean_assert(invertedCovariances == nullptr);
	ocean_assert(overallNumberElements_ != 0);

	// set the correct size of the resulting error vector
	weightedErrorVector.resize(overallNumberElements_ * tResultDimension, 1u);
	Result* const weightedErrors = (Result*)weightedErrorVector.data();

	ExternalFirstModels externalFirstModels(firstModels_.size());
	for (size_t i = 0; i < firstModels_.size(); ++i)
	{
		firstModelTransformationCallback_(candidateFirstModels_[i], externalFirstModels[i]);
	}

	ExternalSecondModels externalSecondModels(secondModels_.size());
	for (size_t i = 0; i < secondModels_.size(); ++i)
	{
		secondModelTransformationCallback_(candidateSecondModels_[i], externalSecondModels[i]);
	}

	size_t index = 0;
	Scalar sqrError = 0;
	Scalars sqrErrors;
	if constexpr (!Estimator::isStandardEstimator<tEstimator>())
	{
		sqrErrors.reserve(overallNumberElements_);
	}

	for (size_t i = 0; i < secondModels_.size(); ++i)
	{
		const size_t numberElements = numberElementsPerSecondModel_[i];

		for (size_t n = 0; n < numberElements; ++n)
		{
			Result& weightedErrorPointer = *((Result*)weightedErrorVector.data() + index);

			if (!errorCallback_(externalFirstModels, externalSecondModels, i, n, weightedErrorPointer))
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

			index++;
		}
	}

	ocean_assert(index == overallNumberElements_);
	ocean_assert(index * 2 == weightedErrorVector.rows());

	// check whether the standard estimator is used
	if constexpr (Estimator::isStandardEstimator<tEstimator>())
	{
		// the weight vector should be and should stay invalid
		ocean_assert(!weightVector);

		ocean_assert((overallNumberElements_) > 0);
		return sqrError /= Scalar(overallNumberElements_);
	}
	else
	{
		// now we need the weight vector
		weightVector.resize(tResultDimension * overallNumberElements_, 1u);

		return sqrErrors2robustErrors<tEstimator, tResultDimension>(sqrErrors, tFirstModelSize * firstModels_.size() + tSecondModelSize * secondModels_.size(), weightedErrors, (StaticBuffer<Scalar, tResultDimension>*)weightVector.data(), nullptr);
	}
}

template <unsigned int tFirstModelSize, unsigned int tSecondModelSize, unsigned int tResultDimension, unsigned int tExternalFirstModelSize, unsigned int tExternalSecondModelSize>
inline void NonLinearUniversalOptimizationSparse::IndividualModelsIndividualModels<tFirstModelSize, tSecondModelSize, tResultDimension, tExternalFirstModelSize, tExternalSecondModelSize>::UniversalOptimizationProvider::acceptCorrection()
{
	firstModels_ = candidateFirstModels_;
	secondModels_ = candidateSecondModels_;

	if (modelAcceptedCallback_)
	{
		modelAcceptedCallback_(firstModels_, secondModels_);
	}
}

template <unsigned int tFirstModelSize, unsigned int tSecondModelSize, unsigned int tResultDimension, unsigned int tExternalFirstModelSize, unsigned int tExternalSecondModelSize>
bool NonLinearUniversalOptimizationSparse::IndividualModelsIndividualModels<tFirstModelSize, tSecondModelSize, tResultDimension, tExternalFirstModelSize, tExternalSecondModelSize>::optimizeUniversalModel(const FirstModels& firstModels, const SecondModels& secondModels, const size_t* numberElementsPerSecondModel, const ValueCallback& valueCallback, const ErrorCallback& errorCallback, const FirstModelTransformationCallback& firstModelTransformationCallback, const SecondModelTransformationCallback& secondModelTransformationCallback, const ModelAcceptedCallback& modelAcceptedCallback, FirstModels& optimizedFirstModels, SecondModels& optimizedSecondModels, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
	ocean_assert(&firstModels != &optimizedFirstModels);
	ocean_assert(&secondModels != &optimizedSecondModels);

	optimizedFirstModels = firstModels;
	optimizedSecondModels = secondModels;

	if (modelAcceptedCallback)
	{
		modelAcceptedCallback(firstModels, secondModels);
	}

	UniversalOptimizationProvider provider(optimizedFirstModels, optimizedSecondModels, numberElementsPerSecondModel, valueCallback, errorCallback, firstModelTransformationCallback, secondModelTransformationCallback, modelAcceptedCallback);
	return NonLinearOptimization::sparseOptimization<UniversalOptimizationProvider>(provider, iterations, estimator, lambda, lambdaFactor, initialError, finalError, nullptr, intermediateErrors);
}

template <unsigned int tSharedModelSize, unsigned int tFirstIndividualModelSize, unsigned int tSecondIndividualModelSize, unsigned int tResultDimension, unsigned int tExternalSharedModelSize, unsigned int tExternalFirstIndividualModelSize, unsigned int tExternalSecondIndividualModelSize>
inline NonLinearUniversalOptimizationSparse::SharedModelIndividualModelsIndividualModels<tSharedModelSize, tFirstIndividualModelSize, tSecondIndividualModelSize, tResultDimension, tExternalSharedModelSize, tExternalFirstIndividualModelSize, tExternalSecondIndividualModelSize>::UniversalOptimizationProvider::UniversalOptimizationProvider(SharedModel& sharedModel, FirstIndividualModels& firstIndividualModels, SecondIndividualModels& secondIndividualModels, const size_t* numberElementsPerSecondModel, const ValueCallback& valueCallback, const ErrorCallback& errorCallback, const SharedModelIsValidCallback& sharedModelIsValidCallback, const SharedModelTransformationCallback& sharedModelTransformationCallback, const FirstIndividualModelTransformationCallback& firstIndividualModelTransformationCallback, const SecondIndividualModelTransformationCallback& secondIndividualModelTransformationCallback, const ModelAcceptedCallback& modelAcceptedCallback) :
	sharedModel_(sharedModel),
	firstIndividualModels_(firstIndividualModels),
	secondIndividualModels_(secondIndividualModels),
	numberElementsPerSecondModel_(numberElementsPerSecondModel),
	overallNumberElements_(0),
	valueCallback_(valueCallback),
	errorCallback_(errorCallback),
	sharedModelIsValidCallback_(sharedModelIsValidCallback),
	sharedModelTransformationCallback_(sharedModelTransformationCallback),
	firstIndividualModelTransformationCallback_(firstIndividualModelTransformationCallback),
	secondIndividualModelTransformationCallback_(secondIndividualModelTransformationCallback),
	modelAcceptedCallback_(modelAcceptedCallback)
{
	ocean_assert(valueCallback_);
	ocean_assert(errorCallback_);
	ocean_assert(sharedModelTransformationCallback_);
	ocean_assert(firstIndividualModelTransformationCallback_);
	ocean_assert(secondIndividualModelTransformationCallback_);

	candidateSharedModel_ = sharedModel;
	candidateFirstIndividualModels_ = firstIndividualModels;
	candidateSecondIndividualModels_ = secondIndividualModels;

	// we need to know how many measurement values are provides as this number determines e.g., the size of the jacobian matrix etc.
	for (size_t n = 0; n < secondIndividualModels.size(); ++n)
	{
		overallNumberElements_ += numberElementsPerSecondModel_[n];
	}
};

template <unsigned int tSharedModelSize, unsigned int tFirstIndividualModelSize, unsigned int tSecondIndividualModelSize, unsigned int tResultDimension, unsigned int tExternalSharedModelSize, unsigned int tExternalFirstIndividualModelSize, unsigned int tExternalSecondIndividualModelSize>
void NonLinearUniversalOptimizationSparse::SharedModelIndividualModelsIndividualModels<tSharedModelSize, tFirstIndividualModelSize, tSecondIndividualModelSize, tResultDimension, tExternalSharedModelSize, tExternalFirstIndividualModelSize, tExternalSecondIndividualModelSize>::UniversalOptimizationProvider::determineJacobian(SparseMatrix& jacobian) const
{
	ocean_assert(valueCallback_);
	ocean_assert(sharedModelTransformationCallback_);
	ocean_assert(firstIndividualModelTransformationCallback_);
	ocean_assert(secondIndividualModelTransformationCallback_);

	ocean_assert(overallNumberElements_ != 0);

	SparseMatrix::Entries jacobianEntries;
	jacobianEntries.reserve(tResultDimension * overallNumberElements_ * (tSharedModelSize + tFirstIndividualModelSize + tSecondIndividualModelSize));

	const Scalar eps = Numeric::weakEps();
	const Scalar invEps = Scalar(1) / eps;

	// for each model (we need to determine a slightly modified epsilon model) so that we can determine the jacobian matrix later
	// for each internal model:
	// - we determine the corresponding external model (without modifying the individual parameters)
	// - we modify each parameter and store the corresponding external sub-models (one external sub-model for each modified internal model parameter)

	// the external shared model (without modified parameter)
	ExternalSharedModel externalSharedModel;
	// the external shared model (with modified (internal) parameters)
	StaticBuffer<ExternalSharedModel, tSharedModelSize> externalEpsSharedModels;

	sharedModelTransformationCallback_(sharedModel_, externalSharedModel);
	for (size_t a = 0; a < tSharedModelSize; ++a)
	{
		SharedModel internalModel = sharedModel_;
		internalModel[a] += eps;

		sharedModelTransformationCallback_(internalModel, externalEpsSharedModels[a]);
	}

	// the external (first) individual models (without modified parameters)
	ExternalFirstIndividualModels externalFirstIndividualModels(firstIndividualModels_.size());
	// the external (first) individual models (with modified (internal) parameters)
	StaticBuffer<ExternalFirstIndividualModels, tFirstIndividualModelSize> externalEpsFirstIndividualModels(tFirstIndividualModelSize, ExternalFirstIndividualModels(firstIndividualModels_.size()));

	for (size_t i = 0; i < firstIndividualModels_.size(); ++i)
	{
		firstIndividualModelTransformationCallback_(firstIndividualModels_[i], externalFirstIndividualModels[i]);

		for (size_t a = 0; a < tFirstIndividualModelSize; ++a)
		{
			FirstIndividualModel firstIndividualModel = firstIndividualModels_[i];
			firstIndividualModel[a] += eps;

			firstIndividualModelTransformationCallback_(firstIndividualModel, externalEpsFirstIndividualModels[a][i]);
		}
	}

	// the external (second) individual models (without modified parameters)
	ExternalSecondIndividualModels externalSecondIndividualModels(secondIndividualModels_.size());
	// the external (second) individual models (with modified (internal) parameters)
	StaticBuffer<ExternalSecondIndividualModels, tSecondIndividualModelSize> externalEpsSecondIndividualModels(tSecondIndividualModelSize, ExternalSecondIndividualModels(secondIndividualModels_.size()));

	for (size_t i = 0; i < secondIndividualModels_.size(); ++i)
	{
		secondIndividualModelTransformationCallback_(secondIndividualModels_[i], externalSecondIndividualModels[i]);

		for (size_t a = 0; a < tSecondIndividualModelSize; ++a)
		{
			SecondIndividualModel secondIndividualModel = secondIndividualModels_[i];
			secondIndividualModel[a] += eps;

			secondIndividualModelTransformationCallback_(secondIndividualModel, externalEpsSecondIndividualModels[a][i]);
		}
	}

	// now we apply the shared external model and the individual external models and their corresponding modified external models to determine the jacobian matrix

	Result result, epsResult;
	size_t row = 0;

	StaticBuffer<Scalar, tSharedModelSize * tResultDimension> sharedModelResults;
	StaticBuffer<Scalar, tFirstIndividualModelSize * tResultDimension> firstIndividualModelResults;
	StaticBuffer<Scalar, tSecondIndividualModelSize * tResultDimension> secondIndividualModelResults;

	for (size_t i = 0; i < secondIndividualModels_.size(); ++i)
	{
		const size_t numberElements = numberElementsPerSecondModel_[i];
		const size_t columnSecond = tSharedModelSize + tFirstIndividualModelSize * firstIndividualModels_.size() + i * tSecondIndividualModelSize;

		for (size_t n = 0; n < numberElements; ++n)
		{
			// calculate the value for the current model
			const size_t firstIndividualModelIndex = valueCallback_(externalSharedModel, externalFirstIndividualModels, externalSecondIndividualModels, i, n, result);
			ocean_assert(firstIndividualModelIndex <= firstIndividualModels_.size());

			// shared model
			for (size_t m = 0; m < tSharedModelSize; ++m)
			{
				// calculate the value for the epsilon model
				const size_t checkModelIndex = valueCallback_(externalEpsSharedModels[m], externalFirstIndividualModels, externalSecondIndividualModels, i, n, epsResult);
				ocean_assert(checkModelIndex == firstIndividualModelIndex);
				OCEAN_SUPPRESS_UNUSED_WARNING(checkModelIndex);

				// store the individual results
				for (size_t d = 0; d < tResultDimension; ++d)
				{
					sharedModelResults[d * tSharedModelSize + m] = (epsResult[d] - result[d]) * invEps;
				}
			}

			// first individual model
			for (size_t m = 0; m < tFirstIndividualModelSize; ++m)
			{
				// calculate the value for the epsilon model
				const size_t checkModelIndex = valueCallback_(externalSharedModel, externalEpsFirstIndividualModels[m], externalSecondIndividualModels, i, n, epsResult);
				ocean_assert(checkModelIndex == firstIndividualModelIndex);
				OCEAN_SUPPRESS_UNUSED_WARNING(checkModelIndex);

				// store the individual results
				for (size_t d = 0; d < tResultDimension; ++d)
				{
					firstIndividualModelResults[d * tFirstIndividualModelSize + m] = (epsResult[d] - result[d]) * invEps;
				}
			}

			// second individual model
			for (size_t m = 0; m < tSecondIndividualModelSize; ++m)
			{
				// calculate the value for the epsilon model
				const size_t checkModelIndex = valueCallback_(externalSharedModel, externalFirstIndividualModels, externalEpsSecondIndividualModels[m], i, n, epsResult);
				ocean_assert(checkModelIndex == firstIndividualModelIndex);
				OCEAN_SUPPRESS_UNUSED_WARNING(checkModelIndex);

				// store the individual results
				for (size_t d = 0; d < tResultDimension; ++d)
				{
					secondIndividualModelResults[d * tSecondIndividualModelSize + m] = (epsResult[d] - result[d]) * invEps;
				}
			}

			const size_t columnFirst = tSharedModelSize + firstIndividualModelIndex * tFirstIndividualModelSize;

			for (size_t d = 0; d < tResultDimension; ++d)
			{
				for (size_t e = 0; e < tSharedModelSize; ++e)
				{
					jacobianEntries.emplace_back(row, e, sharedModelResults.data()[d * tSharedModelSize + e]);
				}

				for (size_t e = 0; e < tFirstIndividualModelSize; ++e)
				{
					jacobianEntries.emplace_back(row, columnFirst + e, firstIndividualModelResults.data()[d * tFirstIndividualModelSize + e]);
				}

				for (size_t e = 0; e < tSecondIndividualModelSize; ++e)
				{
					jacobianEntries.emplace_back(row, columnSecond + e, secondIndividualModelResults.data()[d * tSecondIndividualModelSize + e]);
				}

				row++;
			}
		}
	}

	jacobian = SparseMatrix(tResultDimension * overallNumberElements_, tSharedModelSize + tFirstIndividualModelSize * firstIndividualModels_.size() + tSecondIndividualModelSize * secondIndividualModels_.size(), jacobianEntries);
	ocean_assert(SparseMatrix::Entry::hasOneEntry(jacobian.rows(), jacobian.columns(), jacobianEntries));
	ocean_assert(row == jacobian.rows());
}

template <unsigned int tSharedModelSize, unsigned int tFirstIndividualModelSize, unsigned int tSecondIndividualModelSize, unsigned int tResultDimension, unsigned int tExternalSharedModelSize, unsigned int tExternalFirstIndividualModelSize, unsigned int tExternalSecondIndividualModelSize>
inline void NonLinearUniversalOptimizationSparse::SharedModelIndividualModelsIndividualModels<tSharedModelSize, tFirstIndividualModelSize, tSecondIndividualModelSize, tResultDimension, tExternalSharedModelSize, tExternalFirstIndividualModelSize, tExternalSecondIndividualModelSize>::UniversalOptimizationProvider::applyCorrection(const Matrix& deltas)
{
	ocean_assert(deltas.rows() == tSharedModelSize + tFirstIndividualModelSize * firstIndividualModels_.size() + tSecondIndividualModelSize * secondIndividualModels_.size());

	size_t index = 0;

	// shared model
	for (size_t m = 0; m < tSharedModelSize; ++m)
	{
		const Scalar& delta = deltas(index++);
		candidateSharedModel_[m] = sharedModel_[m] - delta;
	}

	// first individual models
	for (size_t i = 0; i < firstIndividualModels_.size(); ++i)
	{
		for (size_t m = 0; m < tFirstIndividualModelSize; ++m)
		{
			const Scalar& delta = deltas(index++);
			candidateFirstIndividualModels_[i][m] = firstIndividualModels_[i][m] - delta;
		}
	}

	// second individual models
	for (size_t i = 0; i < secondIndividualModels_.size(); ++i)
	{
		for (size_t m = 0; m < tSecondIndividualModelSize; ++m)
		{
			const Scalar& delta = deltas(index++);
			candidateSecondIndividualModels_[i][m] = secondIndividualModels_[i][m] - delta;
		}
	}
}

template <unsigned int tSharedModelSize, unsigned int tFirstIndividualModelSize, unsigned int tSecondIndividualModelSize, unsigned int tResultDimension, unsigned int tExternalSharedModelSize, unsigned int tExternalFirstIndividualModelSize, unsigned int tExternalSecondIndividualModelSize>
template <Estimator::EstimatorType tEstimator>
Scalar NonLinearUniversalOptimizationSparse::SharedModelIndividualModelsIndividualModels<tSharedModelSize, tFirstIndividualModelSize, tSecondIndividualModelSize, tResultDimension, tExternalSharedModelSize, tExternalFirstIndividualModelSize, tExternalSecondIndividualModelSize>::UniversalOptimizationProvider::determineRobustError(Matrix& weightedErrorVector, Matrix& weightVector, const Matrix* invertedCovariances) const
{
	ocean_assert(valueCallback_);
	ocean_assert(sharedModelTransformationCallback_);
	ocean_assert(firstIndividualModelTransformationCallback_);
	ocean_assert(secondIndividualModelTransformationCallback_);

	OCEAN_SUPPRESS_UNUSED_WARNING(invertedCovariances);
	ocean_assert(invertedCovariances == nullptr);
	ocean_assert(overallNumberElements_ != 0);

	ExternalSharedModel externalSharedModel;
	sharedModelTransformationCallback_(candidateSharedModel_, externalSharedModel);

	// check whether we can stop here as we do not have a valid shared model (and the provider supports to decide that)
	if (sharedModelIsValidCallback_ && !sharedModelIsValidCallback_(externalSharedModel))
	{
		return Numeric::maxValue();
	}

	// set the correct size of the resulting error vector
	weightedErrorVector.resize(overallNumberElements_ * tResultDimension, 1u);
	Result* const weightedErrors = (Result*)weightedErrorVector.data();

	ExternalFirstIndividualModels externalFirstIndividualModels(firstIndividualModels_.size());
	for (size_t i = 0; i < firstIndividualModels_.size(); ++i)
	{
		firstIndividualModelTransformationCallback_(candidateFirstIndividualModels_[i], externalFirstIndividualModels[i]);
	}

	ExternalSecondIndividualModels externalSecondIndividualModels(secondIndividualModels_.size());
	for (size_t i = 0; i < secondIndividualModels_.size(); ++i)
	{
		secondIndividualModelTransformationCallback_(candidateSecondIndividualModels_[i], externalSecondIndividualModels[i]);
	}

	size_t index = 0;
	Scalar sqrError = 0;
	Scalars sqrErrors;
	if constexpr (!Estimator::isStandardEstimator<tEstimator>())
	{
		sqrErrors.reserve(overallNumberElements_);
	}

	for (size_t i = 0; i < secondIndividualModels_.size(); ++i)
	{
		const size_t numberElements = numberElementsPerSecondModel_[i];

		for (size_t n = 0; n < numberElements; ++n)
		{
			Result& weightedErrorPointer = *((Result*)weightedErrorVector.data() + index);

			if (!errorCallback_(externalSharedModel, externalFirstIndividualModels, externalSecondIndividualModels, i, n, weightedErrorPointer))
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

			index++;
		}
	}

	ocean_assert(index == overallNumberElements_);
	ocean_assert(index * 2 == weightedErrorVector.rows());

	// check whether the standard estimator is used
	if constexpr (Estimator::isStandardEstimator<tEstimator>())
	{
		// the weight vector should be and should stay invalid
		ocean_assert(!weightVector);

		ocean_assert((overallNumberElements_) > 0);
		return sqrError /= Scalar(overallNumberElements_);
	}
	else
	{
		// now we need the weight vector
		weightVector.resize(tResultDimension * overallNumberElements_, 1u);

		return sqrErrors2robustErrors<tEstimator, tResultDimension>(sqrErrors, tSharedModelSize + tFirstIndividualModelSize * firstIndividualModels_.size() + tSecondIndividualModelSize * secondIndividualModels_.size(), weightedErrors, (StaticBuffer<Scalar, tResultDimension>*)weightVector.data(), nullptr);
	}
}

template <unsigned int tSharedModelSize, unsigned int tFirstIndividualModelSize, unsigned int tSecondIndividualModelSize, unsigned int tResultDimension, unsigned int tExternalSharedModelSize, unsigned int tExternalFirstIndividualModelSize, unsigned int tExternalSecondIndividualModelSize>
inline void NonLinearUniversalOptimizationSparse::SharedModelIndividualModelsIndividualModels<tSharedModelSize, tFirstIndividualModelSize, tSecondIndividualModelSize, tResultDimension, tExternalSharedModelSize, tExternalFirstIndividualModelSize, tExternalSecondIndividualModelSize>::UniversalOptimizationProvider::acceptCorrection()
{
	sharedModel_ = candidateSharedModel_;
	firstIndividualModels_ = candidateFirstIndividualModels_;
	secondIndividualModels_ = candidateSecondIndividualModels_;

	if (modelAcceptedCallback_)
	{
		modelAcceptedCallback_(sharedModel_, firstIndividualModels_, secondIndividualModels_);
	}
}

template <unsigned int tSharedModelSize, unsigned int tFirstIndividualModelSize, unsigned int tSecondIndividualModelSize, unsigned int tResultDimension, unsigned int tExternalSharedModelSize, unsigned int tExternalFirstIndividualModelSize, unsigned int tExternalSecondIndividualModelSize>
bool NonLinearUniversalOptimizationSparse::SharedModelIndividualModelsIndividualModels<tSharedModelSize, tFirstIndividualModelSize, tSecondIndividualModelSize, tResultDimension, tExternalSharedModelSize, tExternalFirstIndividualModelSize, tExternalSecondIndividualModelSize>::optimizeUniversalModel(const SharedModel& sharedModel, const FirstIndividualModels& firstIndividualModels, const SecondIndividualModels& secondIndividualModels, const size_t* numberElementsPerSecondModel, const ValueCallback& valueCallback, const ErrorCallback& errorCallback, const SharedModelIsValidCallback& sharedModelIsValidCallback, const SharedModelTransformationCallback& sharedModelTransformationCallback, const FirstIndividualModelTransformationCallback& firstIndividualModelTransformationCallback, const SecondIndividualModelTransformationCallback& secondIndividualModelTransformationCallback, const ModelAcceptedCallback& modelAcceptedCallback, SharedModel& optimizedSharedModel, FirstIndividualModels& optimizedFirstIndividualModels, SecondIndividualModels& optimizedSecondIndividualModels, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
	ocean_assert(&sharedModel != &optimizedSharedModel);
	ocean_assert(&firstIndividualModels != &optimizedFirstIndividualModels);
	ocean_assert(&secondIndividualModels != &optimizedSecondIndividualModels);

	optimizedSharedModel = sharedModel;
	optimizedFirstIndividualModels = firstIndividualModels;
	optimizedSecondIndividualModels = secondIndividualModels;

	if (modelAcceptedCallback)
	{
		modelAcceptedCallback(sharedModel, firstIndividualModels, secondIndividualModels);
	}

	UniversalOptimizationProvider provider(optimizedSharedModel, optimizedFirstIndividualModels, optimizedSecondIndividualModels, numberElementsPerSecondModel, valueCallback, errorCallback, sharedModelIsValidCallback, sharedModelTransformationCallback, firstIndividualModelTransformationCallback, secondIndividualModelTransformationCallback, modelAcceptedCallback);
	return NonLinearOptimization::sparseOptimization<UniversalOptimizationProvider>(provider, iterations, estimator, lambda, lambdaFactor, initialError, finalError, nullptr, intermediateErrors);
}

}

}

#endif // META_OCEAN_GEOMETRY_NON_LINEAR_UNIVERSAL_OPTIMIZATION_SPARSE_H
