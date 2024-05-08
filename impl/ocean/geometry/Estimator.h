/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_ESTIMATOR_H
#define META_OCEAN_GEOMETRY_ESTIMATOR_H

#include "ocean/geometry/Geometry.h"

#include "ocean/base/Median.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements robust estimator functions.
 * See 'Parameter Estimation Techniques: A Tutorial with Application to Conic Fitting', Zhengyou Zhang, 1997 for detailed information.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT Estimator
{
	public:

		/**
		 * Definition of individual robust estimator types.
		 */
		enum EstimatorType : uint32_t
		{
			/**
			 * An invalid estimator type.
			 */
			ET_INVALID = 0u,

			/**
			 * The standard square error estimator (L2).
			 * The estimation function is defined by:
			 * <pre>
			 * p(x) = x^2 / 2
			 * </pre>
			 *
			 * The weighting function is given by:
			 * <pre>
			 * w(x) = 1
			 * </pre>
			 */
			ET_SQUARE,

			/**
			 * The linear estimator (L1).
			 * The estimation function is defined by:
			 * <pre>
			 * p(x) = |x|
			 * </pre>
			 *
			 * The weighting function is given by:
			 * <pre>
			 * w(x) = 1 / |x|
			 * </pre>
			 */
			ET_LINEAR,

			/**
			 * The Huber estimator type.
			 * The estimation function is defined by:
			 * <pre>
			 * p(x, s) = x^2 / 2,            if |x| <= s
			 *         = s * (|x| - s / 2),  else
			 * </pre>
			 *
			 * The weighting function is given by:
			 * <pre>
			 * w(x, s) = 1,        if |x| <= s
			 *         = s / |x|,  else
			 * </pre>
			 *
			 * The sigma tuning constant is given as: 1.345
			 */
			ET_HUBER,

			/**
			 * The Tukey estimator.
			 * The estimation function is defined by:
			 * <pre>
			 * p(x, s) = s^2 / 6 * (1 - (1 - (x / s)^2)^3),  if |x| <= s
			 *         = s^2 / 6,                            else
			 * </pre>
			 *
			 * The weighting function is given by:
			 * <pre>
			 * w(x, s) = (1 - (x / s)^2)^2,  if |x| <= s
			 *         = 0,                  else
			 * </pre>
			 *
			 * The sigma tuning constant is given as: 4.6851
			 */
			ET_TUKEY,

			/**
			 * The Cauchy estimator.
			 * The estimation function is defined by:
			 * <pre>
			 * p(x, s) = s^2 / 2 * log(1 + (x / s)^2)
			 * </pre>
			 *
			 * The weighting function is defined by:
			 * <pre>
			 * w(x, s) = 1 / (1 + (x / s)^2)
			 * </pre>
			 *
			 * The sigma tuning constant is given as: 2.3849
			 */
			ET_CAUCHY
		};

		/**
		 * Definition of a vector holding estimator types.
		 */
		using EstimatorTypes = std::vector<EstimatorType>;

	public:

		/**
		 * Returns whether an estimator needs a standard deviation for computation.
		 * @tparam tEstimator Estimator for that the dependency is requested
		 * @return True, if so
		 */
		template <EstimatorType tEstimator>
		static constexpr bool needSigma();

		/**
		 * Returns whether an estimator is the standard square error estimator.
		 * @return True, if so
		 * @tparam tEstimator Estimator to check
		 */
		template <EstimatorType tEstimator>
		static constexpr bool isStandardEstimator();

		/**
		 * Returns whether an estimator is the standard square error estimator.
		 * @param estimator The estimator to check
		 * @return True, if so
		 */
		static inline bool isStandardEstimator(const EstimatorType estimator);

		/**
		 * Returns whether a given estimator needs a standard deviation for computation.
		 * @param estimator The estimator for that the dependency is requested
		 * @return True, if so
		 */
		static inline bool needSigma(const EstimatorType estimator);

		/**
		 * Returns the robust error of a residual error for a specified estimator.
		 * @param value Residual error to return the robust error for, with range (-infinity, infinity)
		 * @param sigma Standard deviation of the expected residual error, with range (0, infinity) if 'needSigma<tEstimator>() == false', otherwise 0
		 * @return The resulting robust error, with range [0, infinity)
		 * @tparam tEstimator Type of the estimator to use
		 * @see needSigma().
		 */
		template <EstimatorType tEstimator>
		static inline Scalar robustError(const Scalar value, const Scalar sigma = 0);

		/**
		 * Returns the robust error of a given residual error for a specified estimator.
		 * @param value Residual error to return the robust error for, with range (-infinity, infinity)
		 * @param sigma Standard deviation of the expected residual error, with range (0, infinity) if 'needSigma(estimator) == false', otherwise 0
		 * @param estimator Type of the estimator to use
		 * @return The resulting robust error, with range [0, infinity)
		 * @see needSigma().
		 */
		static inline Scalar robustError(const Scalar value, const Scalar sigma, const EstimatorType estimator);

		/**
		 * Returns the robust error of a squared residual error for a specified estimator.
		 * @param sqrValue Squared residual error to return the robust error for, with range [0, infinity)
		 * @param sqrSigma Standard deviation of the expected residual error, with range (0, infinity) if 'needSigma<tEstimator>() == false', otherwise 0
		 * @return The resulting robust error, with range [0, infinity)
		 * @tparam tEstimator Type of the estimator to use
		 * @see needSigma().
		 */
		template <EstimatorType tEstimator>
		static inline Scalar robustErrorSquare(const Scalar sqrValue, const Scalar sqrSigma = 0);

		/**
		 * Returns the robust error of a given squared residual error for a specified estimator.
		 * @param sqrValue Residual error to return the robust error for, with range [0, infinity)
		 * @param sqrSigma Standard deviation of the expected residual error, with range (0, infinity) if 'needSigma(estimator) == false', otherwise 0
		 * @param estimator Type of the estimator to use
		 * @return The resulting robust error, with range [0, infinity)
		 * @see needSigma().
		 */
		static inline Scalar robustErrorSquare(const Scalar sqrValue, const Scalar sqrSigma, const EstimatorType estimator);

		/**
		 * Returns the weight in relation to a error for a given residual error and a specified estimator.
		 * @param value Residual error to return the weight for
		 * @param sigma Standard deviation of the expected residual error, beware: provide a valid standard deviation if necessary with range (0, infinity)
		 * @return Resulting weight
		 * @tparam tEstimator Type of the estimator to use
		 */
		template <EstimatorType tEstimator>
		static inline Scalar robustWeight(const Scalar value, const Scalar sigma = 0);

		/**
		 * Returns the weight in relation to a error for a given residual error and a specified estimator.
		 * @param value Residual error to return the weight for
		 * @param sigma Standard deviation of the expected residual error
		 * @param estimator Type of the estimator to use
		 * @return Resulting weight
		 */
		static inline Scalar robustWeight(const Scalar value, const Scalar sigma, const EstimatorType estimator);

		/**
		 * Returns the weight in relation to a squared error for a given residual error and a specified estimator.
		 * @param sqrValue Squared residual error to return the weight for
		 * @param sqrSigma Squared standard deviation of the expected residual error, beware: provide a valid standard deviation if necessary with range (0, infinity)
		 * @return Resulting weight (not the squared result), with range [0, infinity)
		 * @tparam tEstimator Type of the estimator to use
		 */
		template <EstimatorType tEstimator>
		static inline Scalar robustWeightSquare(const Scalar sqrValue, const Scalar sqrSigma = 0);

		/**
		 * Returns the weight in relation to a squared error for a given residual error and a specified estimator.
		 * @param sqrValue Squared residual error to return the weight for
		 * @param sqrSigma Squared standard deviation of the expected residual error
		 * @param estimator Type of the estimator to use
		 * @return Resulting weight (not the squared result), with range [0, infinity)
		 */
		static inline Scalar robustWeightSquare(const Scalar sqrValue, const Scalar sqrSigma, const EstimatorType estimator);

		/**
		 * Determines the sigma for a specific set of residual errors and a specified estimator.
		 * @param errors Residual errors for that the corresponding sigma has to be determined
		 * @param number The number of provided residual errors, with range [1, infinity)
		 * @param modelParameters Number of the parameter that define the model
		 * @return Resulting sigma
		 * @tparam tEstimator Type of the estimator to use
		 */
		template <EstimatorType tEstimator>
		static inline Scalar determineSigma(const Scalar* errors, const size_t number, const size_t modelParameters);

		/**
		 * Determines the sigma for a specific set of residual errors and a specified estimator.
		 * @param errors Residual errors for that the corresponding sigma has to be determined
		 * @param number The number of provided residual errors, with range [1, infinity)
		 * @param modelParameters Number of the parameter that define the model
		 * @param estimator Type of the estimator to use
		 * @return Resulting sigma
		 */
		static inline Scalar determineSigma(const Scalar* errors, const size_t number, const size_t modelParameters, const EstimatorType estimator);

		/**
		 * Determines the sigma for a specific subset of residual errors and a specified estimator.
		 * @param errors Residual errors for that the corresponding sigma has to be determined
		 * @param indices Indices of the subset of the residual errors, beware: no range check is applied
		 * @param numberIndices Number of provided indices, with range [1, infinity)
		 * @param modelParameters Number of the parameter that define the model, with range [1, infinity)
		 * @return Resulting sigma
		 * @tparam tEstimator Type of the estimator to use
		 */
		template <EstimatorType tEstimator>
		static inline Scalar determineSigma(const Scalar* errors, const unsigned int* indices, const size_t numberIndices, const size_t modelParameters);

		/**
		 * Determines the sigma for a specific subset of residual errors and a specified estimator.
		 * @param errors Residual errors for that the corresponding sigma has to be determined
		 * @param indices Indices of the subset of the residual errors, beware: no range check is applied
		 * @param numberIndices Number of provided indices, with range [1, infinity)
		 * @param modelParameters Number of the parameter that define the model
		 * @param estimator Type of the estimator to use
		 * @return Resulting sigma
		 */
		static inline Scalar determineSigma(const Scalar* errors, const unsigned int* indices, const size_t numberIndices, const size_t modelParameters, const EstimatorType estimator);

		/**
		 * Determines the squared sigma for a specific set of squared residual errors and a specified estimator.
		 * @param sqrErrors Squared residual errors for that the corresponding sigma has to be determined
		 * @param number The number of provided residual errors, with range [1, infinity)
		 * @param modelParameters Number of the parameter that define the model
		 * @return Resulting sigma (not the squared sigma)
		 * @tparam tEstimator Type of the estimator to use
		 */
		template <EstimatorType tEstimator>
		static inline Scalar determineSigmaSquare(const Scalar* sqrErrors, const size_t number, const size_t modelParameters);

		/**
		 * Determines the squared sigma for a specific set of squared residual errors and a specified estimator.
		 * @param sqrErrors Squared residual errors for that the corresponding sigma has to be determined
		 * @param number The number of provided residual errors, with range [1, infinity)
		 * @param modelParameters Number of the parameter that define the model
		 * @param estimator Type of the estimator to use
		 * @return Resulting sigma (not the squared sigma)
		 */
		static inline Scalar determineSigmaSquare(const Scalar* sqrErrors, const size_t number, const size_t modelParameters, const EstimatorType estimator);

		/**
		 * Determines the squared sigma for a specific set of squared residual errors and a specified estimator.
		 * @param sqrErrors Squared residual errors for that the corresponding sigma has to be determined
		 * @param indices Indices of the subset of the residual errors, beware: no range check is applied
		 * @param numberIndices Number of provided indices, with range [1, infinity)
		 * @param modelParameters Number of the parameter that define the model
		 * @return Resulting sigma (not the squared sigma)
		 * @tparam tEstimator Type of the estimator to use
		 */
		template <EstimatorType tEstimator>
		static inline Scalar determineSigmaSquare(const Scalar* sqrErrors, const unsigned int* indices, const size_t numberIndices, const size_t modelParameters);

		/**
		 * Determines the squared sigma for a specific set of squared residual errors and a specified estimator.
		 * @param sqrErrors Squared residual errors for that the corresponding sigma has to be determined
		 * @param indices Indices of the subset of the residual errors, beware: no range check is applied
		 * @param numberIndices Number of provided indices, with range [1, infinity)
		 * @param modelParameters Number of the parameter that define the model
		 * @param estimator Type of the estimator to use
		 * @return Resulting sigma (not the squared sigma)
		 */
		static inline Scalar determineSigmaSquare(const Scalar* sqrErrors, const unsigned int* indices, const size_t numberIndices, const size_t modelParameters, const EstimatorType estimator);

		/**
		 * Determines the overall robust error for set of given squared errors, a specified estimator and the dimension of the model.
		 * @param sqrErrors The squared error values for which the overall robust error will be determined
		 * @param number The number of provided squared error values, with range [1, infinity)
		 * @param modelParameters Number of parameters that define the model that has to be optimized (the dimension of the model), with range [1, infinity)
		 * @return Resulting overall robust error for the given set of squared errors
		 * @tparam tEstimator Robust error estimator to be used
		 */
		template <Estimator::EstimatorType tEstimator>
		static inline Scalar determineRobustError(const Scalar* sqrErrors, const size_t number, const size_t modelParameters);

		/**
		 * Returns the maximal weight for any estimator which is used to clamp extremely high weights (for tiny errors).
		 * @return The maximal weight to be applied
		 */
		static constexpr Scalar maximalWeight();

		/**
		 * Returns the inverse maximal weight for any estimator which is used to clamp extremely high weights (for tiny errors).
		 * @return Returns 1 / maximalWeight()
		 */
		static constexpr Scalar invMaximalWeight();

		/**
		 * Translates a given estimator type into a readable string.
		 * @param estimatorType The type of the estimator to translate
		 * @return The readable string, 'Invalid' if unknown
		 */
		static std::string translateEstimatorType(const EstimatorType estimatorType);

		/**
		 * Translates a readable name of an estimator type to it's value.
		 * @param estimatorType The name of the estimator type for which the value will be returned
		 * @return The estimator type, ET_INVALID if invalid
		 */
		static EstimatorType translateEstimatorType(const std::string& estimatorType);

		/**
		 * Returns all existing valid estimator types.
		 * @return The valid estimator types
		 */
		static const EstimatorTypes& estimatorTypes();

	protected:

		/**
		 * Returns the tuning constant allowing to determine a 95 percent efficiency on the standard normal distribution for individual estimators.
		 * @return Tuning constant
		 */
		template <EstimatorType tEstimator>
		static inline Scalar sigmaTuningContstant();
};

template <Estimator::EstimatorType tEstimator>
constexpr bool Estimator::needSigma()
{
	return tEstimator == Estimator::ET_HUBER || tEstimator == Estimator::ET_TUKEY || tEstimator == Estimator::ET_CAUCHY;
}

inline bool Estimator::needSigma(const EstimatorType estimator)
{
	switch (estimator)
	{
		case ET_SQUARE:
			return needSigma<ET_SQUARE>();

		case ET_LINEAR:
			return needSigma<ET_LINEAR>();

		case ET_HUBER:
			return needSigma<ET_HUBER>();

		case ET_TUKEY:
			return needSigma<ET_TUKEY>();

		case ET_CAUCHY:
			return needSigma<ET_CAUCHY>();

		case ET_INVALID:
			break;
	}

	ocean_assert(false && "Invalid estimator!");
	return false;
}

template <Estimator::EstimatorType tEstimator>
constexpr bool Estimator::isStandardEstimator()
{
	static_assert(tEstimator != Estimator::ET_INVALID, "Invalid estimator!");

	return tEstimator == Estimator::ET_SQUARE;
}

inline bool Estimator::isStandardEstimator(const EstimatorType estimator)
{
	ocean_assert(estimator != ET_INVALID);

	return estimator == ET_SQUARE;
}

template <Estimator::EstimatorType tEstimator>
inline Scalar Estimator::robustError(const Scalar value, const Scalar /*sigma*/)
{
	ocean_assert(false && "Invalid estimator type!");

	return value;
}

template <>
inline Scalar Estimator::robustError<Estimator::ET_SQUARE>(const Scalar value, const Scalar sigma)
{
	ocean_assert_and_suppress_unused(sigma == Scalar(0), sigma);

	// value^2 / 2

	return value * value * Scalar(0.5);
}

template <>
inline Scalar Estimator::robustError<Estimator::ET_LINEAR>(const Scalar value, const Scalar sigma)
{
	ocean_assert_and_suppress_unused(sigma == Scalar(0), sigma);

	// |value|

	return Numeric::abs(value);
}

template <>
inline Scalar Estimator::robustError<Estimator::ET_HUBER>(const Scalar value, const Scalar sigma)
{
	// |value| <= sigma : value * value / 2
	//             else : sigma * (|value| - sigma / 2)

	ocean_assert(sigma > 0);

	const Scalar absValue = Numeric::abs(value);

	if (absValue <= sigma)
	{
		return value * value * Scalar(0.5);
	}

	return sigma * (absValue - sigma * Scalar(0.5));
}

template <>
inline Scalar Estimator::robustError<Estimator::ET_TUKEY>(const Scalar value, const Scalar sigma)
{
	// |value| <= sigma : sigma^2 / 6 * (1 - (1 - (value / sigma)^2 )^3 )
	//             else : sigma^2 / 6

	ocean_assert(sigma > 0);

	const Scalar sqrSigma6 = sigma * sigma * Scalar(1.0 / 6.0);

	if (Numeric::abs(value) <= sigma)
	{
		const Scalar tmp(Scalar(1) - Numeric::sqr(value / sigma));

		return sqrSigma6 * (Scalar(1) - tmp * tmp * tmp);
	}

	return sqrSigma6;
}

template <>
inline Scalar Estimator::robustError<Estimator::ET_CAUCHY>(const Scalar value, const Scalar sigma)
{
	// sigma^2 / 2 * log(1 + (value / sigma)^2)

	ocean_assert(sigma > 0);

	return Numeric::log(Scalar(1) + Numeric::sqr(value / sigma)) * Numeric::sqr(sigma) * Scalar(0.5);
}

inline Scalar Estimator::robustError(const Scalar value, const Scalar sigma, const EstimatorType estimator)
{
	switch (estimator)
	{
		case ET_SQUARE:
			return robustError<ET_SQUARE>(value);

		case ET_LINEAR:
			return robustError<ET_LINEAR>(value);

		case ET_HUBER:
			return robustError<ET_HUBER>(value, sigma);

		case ET_TUKEY:
			return robustError<ET_TUKEY>(value, sigma);

		case ET_CAUCHY:
			return robustError<ET_CAUCHY>(value, sigma);

		case ET_INVALID:
			break;
	}

	ocean_assert(false && "Invalid estimator!");
	return robustError<ET_SQUARE>(value);
}

template <Estimator::EstimatorType tEstimator>
inline Scalar Estimator::robustErrorSquare(const Scalar sqrValue, const Scalar /*sqrSigma*/)
{
	ocean_assert(false && "Invalid estimator type!");

	return sqrValue;
}

template <>
inline Scalar Estimator::robustErrorSquare<Estimator::ET_SQUARE>(const Scalar sqrValue, const Scalar sqrSigma)
{
	ocean_assert_and_suppress_unused(sqrSigma == Scalar(0), sqrSigma);

	// value^2 / 2

	return sqrValue * Scalar(0.5);
}

template <>
inline Scalar Estimator::robustErrorSquare<Estimator::ET_LINEAR>(const Scalar sqrValue, const Scalar sqrSigma)
{
	ocean_assert_and_suppress_unused(sqrSigma == Scalar(0), sqrSigma);

	// |value|

	return Numeric::sqrt(sqrValue);
}

template <>
inline Scalar Estimator::robustErrorSquare<Estimator::ET_HUBER>(const Scalar sqrValue, const Scalar sqrSigma)
{
	// |value| <= sigma : value * value / 2
	//             else : sigma * (|value| - sigma / 2) = sigma * |value| - sigma^2 / 2

	ocean_assert(sqrSigma > 0);

	if (sqrValue <= sqrSigma)
	{
		return sqrValue * Scalar(0.5);
	}

	return Numeric::sqrt(sqrValue) * Numeric::sqrt(sqrSigma) - sqrSigma * Scalar(0.5);
}

template <>
inline Scalar Estimator::robustErrorSquare<Estimator::ET_TUKEY>(const Scalar sqrValue, const Scalar sqrSigma)
{
	// |value| <= sigma : sigma^2 / 6 * (1 - (1 - (value / sigma)^2 )^3 )
	//             else : sigma^2 / 6

	ocean_assert(sqrSigma > 0);

	if (sqrValue <= sqrSigma)
	{
		const Scalar tmp(Scalar(1) - sqrValue / sqrSigma);

		return sqrSigma * Scalar(1.0 / 6.0) * (Scalar(1) - tmp * tmp * tmp);
	}

	return sqrSigma * Scalar(1.0 / 6.0);
}

template <>
inline Scalar Estimator::robustErrorSquare<Estimator::ET_CAUCHY>(const Scalar sqrValue, const Scalar sqrSigma)
{
	// sigma^2 / 2 * log(1 + (value / sigma)^2)

	ocean_assert(sqrSigma > 0);

	return Numeric::log(Scalar(1) + sqrValue / sqrSigma) * sqrSigma * Scalar(0.5);
}

inline Scalar Estimator::robustErrorSquare(const Scalar sqrValue, const Scalar sqrSigma, const EstimatorType estimator)
{
	switch (estimator)
	{
		case ET_SQUARE:
			return robustErrorSquare<ET_SQUARE>(sqrValue);

		case ET_LINEAR:
			return robustErrorSquare<ET_LINEAR>(sqrValue);

		case ET_HUBER:
			return robustErrorSquare<ET_HUBER>(sqrValue, sqrSigma);

		case ET_TUKEY:
			return robustErrorSquare<ET_TUKEY>(sqrValue, sqrSigma);

		case ET_CAUCHY:
			return robustErrorSquare<ET_CAUCHY>(sqrValue, sqrSigma);

		case ET_INVALID:
			break;
	}

	ocean_assert(false && "Invalid estimator!");
	return robustErrorSquare<ET_SQUARE>(sqrValue);
}

template <Estimator::EstimatorType tEstimator>
inline Scalar Estimator::robustWeight(const Scalar value, const Scalar /*sigma*/)
{
	ocean_assert(false && "Invalid estimator!");

	return value;
}

template <>
inline Scalar Estimator::robustWeight<Estimator::ET_SQUARE>(const Scalar /*value*/, const Scalar sigma)
{
	ocean_assert_and_suppress_unused(sigma == Scalar(0), sigma);

	return Scalar(1);
}

template <>
inline Scalar Estimator::robustWeight<Estimator::ET_LINEAR>(const Scalar value, const Scalar sigma)
{
	ocean_assert_and_suppress_unused(sigma == Scalar(0), sigma);

	const Scalar absValue = Numeric::abs(value);

	return std::min(Scalar(1) / absValue, maximalWeight());
}

template <>
inline Scalar Estimator::robustWeight<Estimator::ET_HUBER>(const Scalar value, const Scalar sigma)
{
	ocean_assert(sigma > 0);

	const Scalar absValue = Numeric::abs(value);

	if (absValue <= sigma)
	{
		return Scalar(1);
	}

	return std::min(sigma / absValue, maximalWeight());
}

template <>
inline Scalar Estimator::robustWeight<Estimator::ET_TUKEY>(const Scalar value, const Scalar sigma)
{
	ocean_assert(sigma > 0);

	const Scalar absValue = Numeric::abs(value);

	if (absValue > sigma)
	{
		return Scalar(0);
	}

	return std::min(Numeric::sqr(Scalar(1) - Numeric::sqr(value / sigma)), maximalWeight());
}

template <>
inline Scalar Estimator::robustWeight<Estimator::ET_CAUCHY>(const Scalar value, const Scalar sigma)
{
	ocean_assert(sigma > 0);

	return Scalar(1) / (Scalar(1) + Numeric::sqr(value / sigma));
}

inline Scalar Estimator::robustWeight(const Scalar value, const Scalar sigma, const EstimatorType estimator)
{
	switch (estimator)
	{
		case ET_SQUARE:
			return robustWeight<ET_SQUARE>(value);

		case ET_LINEAR:
			return robustWeight<ET_LINEAR>(value);

		case ET_HUBER:
			return robustWeight<ET_HUBER>(value, sigma);

		case ET_TUKEY:
			return robustWeight<ET_TUKEY>(value, sigma);

		case ET_CAUCHY:
			return robustWeight<ET_CAUCHY>(value, sigma);

		case ET_INVALID:
			break;
	}

	ocean_assert(false && "Invalid estimator!");
	return robustWeight<ET_SQUARE>(value);
}

template <Estimator::EstimatorType tEstimator>
inline Scalar Estimator::robustWeightSquare(const Scalar sqrValue, const Scalar /*sqrSigma*/)
{
	ocean_assert(false && "Invalid estimator!");
	return sqrValue;
}

template <>
inline Scalar Estimator::robustWeightSquare<Estimator::ET_SQUARE>(const Scalar /*sqrValue*/, const Scalar sqrSigma)
{
	ocean_assert_and_suppress_unused(sqrSigma == Scalar(0), sqrSigma);

	return Scalar(1);
}

template <>
inline Scalar Estimator::robustWeightSquare<Estimator::ET_LINEAR>(const Scalar sqrValue, const Scalar sqrSigma)
{
	ocean_assert(sqrValue >= 0);
	ocean_assert_and_suppress_unused(sqrSigma == Scalar(0), sqrSigma);

	if (sqrValue < Numeric::sqr(Numeric::weakEps()))
	{
		return Scalar(1) / Numeric::weakEps();
	}

	return Scalar(1) / Numeric::sqrt(sqrValue);
}

template <>
inline Scalar Estimator::robustWeightSquare<Estimator::ET_HUBER>(const Scalar sqrValue, const Scalar sqrSigma)
{
	ocean_assert(sqrValue >= 0 && sqrSigma > 0);

	if (sqrValue <= sqrSigma)
	{
		return Scalar(1);
	}

	return std::min(Numeric::sqrt(sqrSigma / sqrValue), maximalWeight());
}

template <>
inline Scalar Estimator::robustWeightSquare<Estimator::ET_TUKEY>(const Scalar sqrValue, const Scalar sqrSigma)
{
	ocean_assert(sqrSigma > 0);

	if (sqrValue > sqrSigma)
	{
		return Scalar(0);
	}

	return std::min(Numeric::sqr(Scalar(1) - sqrValue / sqrSigma), maximalWeight());
}

template <>
inline Scalar Estimator::robustWeightSquare<Estimator::ET_CAUCHY>(const Scalar sqrValue, const Scalar sqrSigma)
{
	ocean_assert(sqrSigma > 0);

	return Scalar(1) / Scalar(1 + sqrValue / sqrSigma);
}

inline Scalar Estimator::robustWeightSquare(const Scalar sqrValue, const Scalar sqrSigma, const EstimatorType estimator)
{
	switch (estimator)
	{
		case ET_SQUARE:
			return robustWeightSquare<ET_SQUARE>(sqrValue);

		case ET_LINEAR:
			return robustWeightSquare<ET_LINEAR>(sqrValue);

		case ET_HUBER:
			return robustWeightSquare<ET_HUBER>(sqrValue, sqrSigma);

		case ET_TUKEY:
			return robustWeightSquare<ET_TUKEY>(sqrValue, sqrSigma);

		case ET_CAUCHY:
			return robustWeightSquare<ET_CAUCHY>(sqrValue, sqrSigma);

		case ET_INVALID:
			break;
	}

	ocean_assert(false && "Invalid estimator!");
	return robustWeightSquare<ET_SQUARE>(sqrValue);
}

template <Estimator::EstimatorType tEstimator>
inline Scalar Estimator::determineSigma(const Scalar* errors, const size_t number, const size_t modelParameters)
{
	ocean_assert(number > 0);

	const Scalar median = Median::constMedian(errors, number);

	if (number <= modelParameters)
	{
		return max(Numeric::eps(), sigmaTuningContstant<tEstimator>() * Scalar(1.4826) * median);
	}

	return max(Numeric::eps(), sigmaTuningContstant<tEstimator>() * Scalar(1.4826) * (1 + Scalar(5) / Scalar(number - modelParameters)) * median);
}

inline Scalar Estimator::determineSigma(const Scalar* errors, const size_t number, const size_t modelParameters, const EstimatorType estimator)
{
	switch (estimator)
	{
		case ET_SQUARE:
		case ET_LINEAR:
			break;

		case ET_HUBER:
			return determineSigma<ET_HUBER>(errors, number, modelParameters);

		case ET_TUKEY:
			return determineSigma<ET_TUKEY>(errors, number, modelParameters);

		case ET_CAUCHY:
			return determineSigma<ET_CAUCHY>(errors, number, modelParameters);

		case ET_INVALID:
			break;
	}

	ocean_assert(false && "Invalid estimator!");
	return Scalar(1);
}

template <Estimator::EstimatorType tEstimator>
inline Scalar Estimator::determineSigma(const Scalar* errors, const unsigned int* indices, const size_t numberIndices, const size_t modelParameters)
{
	ocean_assert(errors != nullptr);
	ocean_assert(indices != nullptr);
	ocean_assert(numberIndices > 0);
	ocean_assert(modelParameters >= 1);

	// extract the subset of the errors
	Scalars subsetErrors(numberIndices);
	for (size_t n = 0; n < numberIndices; ++n)
	{
		subsetErrors[n] = errors[indices[n]];
	}

	const Scalar median = Median::median(subsetErrors.data(), numberIndices);

	if (numberIndices <= modelParameters)
	{
		return max(Numeric::eps(), sigmaTuningContstant<tEstimator>() * Scalar(1.4826) * median);
	}

	return max(Numeric::eps(), sigmaTuningContstant<tEstimator>() * Scalar(1.4826) * (1 + Scalar(5) / Scalar(numberIndices - modelParameters)) * median);
}

inline Scalar Estimator::determineSigma(const Scalar* errors, const unsigned int* indices, const size_t numberIndices, const size_t modelParameters, const EstimatorType estimator)
{
	switch (estimator)
	{
		case ET_SQUARE:
		case ET_LINEAR:
			break;

		case ET_HUBER:
			return determineSigma<ET_HUBER>(errors, indices, numberIndices, modelParameters);

		case ET_TUKEY:
			return determineSigma<ET_TUKEY>(errors, indices, numberIndices, modelParameters);

		case ET_CAUCHY:
			return determineSigma<ET_CAUCHY>(errors, indices, numberIndices, modelParameters);

		case ET_INVALID:
			break;
	}

	ocean_assert(false && "Invalid estimator!");
	return Scalar(1);
}

template <Estimator::EstimatorType tEstimator>
inline Scalar Estimator::determineSigmaSquare(const Scalar* sqrErrors, const size_t number, const size_t modelParameters)
{
	ocean_assert(sqrErrors != nullptr);
	ocean_assert(number > 0);

	const Scalar sqrMedian = Median::constMedian(sqrErrors, number);

	if (number <= modelParameters)
	{
		return max(Numeric::eps(), sigmaTuningContstant<tEstimator>() * Scalar(1.4826) * Numeric::sqrt(sqrMedian));
	}

	return max(Numeric::eps(), sigmaTuningContstant<tEstimator>() * Scalar(1.4826) * (1 + Scalar(5) / Scalar(number - modelParameters)) * Numeric::sqrt(sqrMedian));
}

inline Scalar Estimator::determineSigmaSquare(const Scalar* sqrErrors, const size_t number, const size_t modelParameters, const EstimatorType estimator)
{
	switch (estimator)
	{
		case ET_SQUARE:
		case ET_LINEAR:
			break;

		case ET_HUBER:
			return determineSigmaSquare<ET_HUBER>(sqrErrors, number, modelParameters);

		case ET_TUKEY:
			return determineSigmaSquare<ET_TUKEY>(sqrErrors, number, modelParameters);

		case ET_CAUCHY:
			return determineSigmaSquare<ET_CAUCHY>(sqrErrors, number, modelParameters);

		case ET_INVALID:
			break;
	}

	ocean_assert(false && "Invalid estimator!");
	return Scalar(1);
}

template <Estimator::EstimatorType tEstimator>
inline Scalar Estimator::determineSigmaSquare(const Scalar* sqrErrors, const unsigned int* indices, const size_t numberIndices, const size_t modelParameters)
{
	ocean_assert(sqrErrors != nullptr);
	ocean_assert(indices != nullptr);
	ocean_assert(numberIndices > 0);

	// extract the subset of the errors
	Scalars subsetSqrErrors(numberIndices);
	for (size_t n = 0; n < numberIndices; ++n)
	{
		subsetSqrErrors[n] = sqrErrors[indices[n]];
	}

	const Scalar sqrMedian = Median::median(subsetSqrErrors.data(), numberIndices);

	if (numberIndices <= modelParameters)
	{
		return max(Numeric::eps(), sigmaTuningContstant<tEstimator>() * Scalar(1.4826) * Numeric::sqrt(sqrMedian));
	}

	return max(Numeric::eps(), sigmaTuningContstant<tEstimator>() * Scalar(1.4826) * (1 + Scalar(5) / Scalar(numberIndices - modelParameters)) * Numeric::sqrt(sqrMedian));
}

inline Scalar Estimator::determineSigmaSquare(const Scalar* sqrErrors, const unsigned int* indices, const size_t numberIndices, const size_t modelParameters, const EstimatorType estimator)
{
	switch (estimator)
	{
		case ET_SQUARE:
		case ET_LINEAR:
			break;

		case ET_HUBER:
			return determineSigmaSquare<ET_HUBER>(sqrErrors, indices, numberIndices, modelParameters);

		case ET_TUKEY:
			return determineSigmaSquare<ET_TUKEY>(sqrErrors, indices, numberIndices, modelParameters);

		case ET_CAUCHY:
			return determineSigmaSquare<ET_CAUCHY>(sqrErrors, indices, numberIndices, modelParameters);

		case ET_INVALID:
			break;
	}

	ocean_assert(false && "Invalid estimator!");
	return Scalar(1);
}

template <Estimator::EstimatorType tEstimator>
inline Scalar Estimator::determineRobustError(const Scalar* sqrErrors, const size_t number, const size_t modelParameters)
{
	ocean_assert(sqrErrors != nullptr && number >= 1);
	ocean_assert(modelParameters >= 1);

	// determine the sigma ideal for the square errors
	const Scalar sqrSigma = needSigma<tEstimator>() ? Numeric::sqr(determineSigmaSquare<tEstimator>(sqrErrors, number, modelParameters)) : 0;

	Scalar robustError = 0;

	for (size_t n = 0; n < number; ++n)
	{
		robustError += sqrErrors[n] * robustWeightSquare<tEstimator>(sqrErrors[n], sqrSigma);
	}

	// return the averaged robust error
	return robustError / Scalar(number);
}

constexpr Scalar Estimator::maximalWeight()
{
	return Scalar(10) / Numeric::weakEps();
}

constexpr Scalar Estimator::invMaximalWeight()
{
	return Scalar(1) / maximalWeight();
}

template <Estimator::EstimatorType tEstimator>
inline Scalar Estimator::sigmaTuningContstant()
{
	ocean_assert(false && "Invalid estimator type!");
	return Scalar(1);
}

template <>
inline Scalar Estimator::sigmaTuningContstant<Estimator::ET_HUBER>()
{
	return Scalar(1.345);
}

template <>
inline Scalar Estimator::sigmaTuningContstant<Estimator::ET_TUKEY>()
{
	return Scalar(4.6851);
}

template <>
inline Scalar Estimator::sigmaTuningContstant<Estimator::ET_CAUCHY>()
{
	return Scalar(2.3849);
}

}

}

#endif // META_OCEAN_GEOMETRY_ESTIMATOR_2_H
