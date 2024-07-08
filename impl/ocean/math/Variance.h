/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_VARIANCE_H
#define META_OCEAN_MATH_VARIANCE_H

#include "ocean/math/Math.h"
#include "ocean/math/Numeric.h"

namespace Ocean
{

// Forward declaration.
template <typename T> class VarianceT;

/**
 * Definition of a variance object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION either with single or double precision float data type.
 * @see VarianceT
 * @ingroup math
 */
typedef VarianceT<Scalar> Variance;

/**
 * Definition of a variance object with double values.
 * @see VarianceT
 * @ingroup math
 */
typedef VarianceT<double> VarianceD;

/**
 * Definition of a variance object with float values.
 * @see VarianceT
 * @ingroup math
 */
typedef VarianceT<float> VarianceF;

/**
 * Definition of a vector holding variance objects.
 * @see Variance
 * @ingroup math
 */
typedef std::vector<Variance> Variances;

/**
 * This class allows to determine the variance in a given data set.
 * @tparam T Data type of the variance data
 * @see Variance, VarianceF, VarianceD.
 * @ingroup math
 */
template <typename T>
class VarianceT
{
	public:

		/**
		 * Creates a new variance object.
		 */
		VarianceT() = default;

		/**
		 * Creates a new variance object with several values.
		 * @param values The values to be used, must be valid if size > 0
		 * @param size The number of values to be used, with range [0, infinity)
		 */
		inline VarianceT(const T* values, const size_t size);

		/**
		 * Adds a new value.
		 * @param value The value to be added
		 */
		inline void add(const T& value);

		/**
		 * Adds the same value several times.
		 * @param size The number of value to add, with range [1, infinity)
		 * @param value The value to be added
		 */
		inline void add(const size_t size, const T& value);

		/**
		 * Adds several values.
		 * @param values The values to be added, must be valid if size > 0
		 * @param size The number of values to be added, with range [0, infinity)
		 */
		inline void add(const T* values, const size_t size);

		/**
		 * Removes a previously added value.
		 * @param value The value to be removed, must have been added before
		 * @see add().
		 */
		inline void remove(const T& value);

		/**
		 * Returns the variance of the data set.
		 * @return Data set variance
		 */
		inline T variance() const;

		/**
		 * Returns the deviation of the data set.
		 * The deviation is determined by the square root of the variance.
		 * @return Data set deviation
		 */
		inline T deviation() const;

		/**
		 * Returns the average of the data set.
		 * @return Data set average
		 */
		inline T average() const;

		/**
		 * Returns the size of the data set.
		 * @return Data set size
		 */
		inline size_t size() const;

		/**
		 * Returns whether the data set holds at least one value.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	private:

		/// Stores the data sum.
		T sum_ = T(0);

		/// Stores the squared data sum.
		T squaredSum_ = T(0);

		/// Size of the data set.
		size_t size_ = 0;
};

template <typename T>
inline VarianceT<T>::VarianceT(const T* values, const size_t size)
{
	add(values, size);
}

template <typename T>
inline void VarianceT<T>::add(const T& value)
{
	// check that enough space is left
	ocean_assert(NumericT<T>::maxValue() - sum_ >= value);
	ocean_assert(NumericT<T>::maxValue() - squaredSum_ >= value * value);

	sum_ += value;
	squaredSum_ += value * value;

	++size_;
}

template <typename T>
inline void VarianceT<T>::add(const size_t size, const T& value)
{
	// check that enough space is left
	ocean_assert(NumericT<T>::maxValue() - sum_ >= value * T(size));
	ocean_assert(NumericT<T>::maxValue() - squaredSum_ >= value * value * T(size));

	ocean_assert(size >= 1);

	sum_ += value * T(size);
	squaredSum_ += value * value * T(size);

	size_ += size;
}

template <typename T>
inline void VarianceT<T>::add(const T* values, const size_t size)
{
	for (size_t n = 0; n < size; ++n)
	{
		add(values[n]);
	}
}

template <typename T>
inline void VarianceT<T>::remove(const T& value)
{
	ocean_assert(size_ >= 1);

	sum_ -= value;

	ocean_assert(!(std::is_same<T, float>::value) || squaredSum_ + T(0.1) >= value * value); // e.g., in case of several add/move calls
	ocean_assert(!(std::is_same<T, double>::value) || squaredSum_ + T(0.0001) >= value * value);

	squaredSum_ -= value * value;

	--size_;
}

template <typename T>
inline T VarianceT<T>::variance() const
{
	ocean_assert(size_ > 0);

	/**
	 * E[X^2] - (E[x])^2
	 * = mean(X^2) - mean(X)^2
	 * = sum(X^2) / size - (sum(X) / size)^2
	 * = sum(X^2) * size / size^2 - sum(X)^2 / size^2
	 * = (sum(X^2) * size - sum(X)^2) / size^2
	 */

	const T varianceSquaredSize = T(size_) * T(size_);
	const T varianceSquaredSize_2 = varianceSquaredSize / 2;

	// varianceSquaredSize_2 the correction is mainly for integer data types
	return (squaredSum_ * T(size_) - sum_ * sum_ + varianceSquaredSize_2) / varianceSquaredSize;
}

/**
 * Specialization for double elements.
 * @return Resulting variance
 */
template <>
inline double VarianceT<double>::variance() const
{
	ocean_assert(size_ > 0);

	/**
	 * E[X^2] - (E[x])^2
	 */

	return squaredSum_ / double(size_) - (sum_ * sum_) / (double(size_) * double(size_));
}

/**
 * Specialization for double elements.
 * @return Resulting variance
 */
template <>
inline float VarianceT<float>::variance() const
{
	ocean_assert(size_ > 0);

	/**
	 * E[X^2] - (E[x])^2
	 */

	return squaredSum_ / float(size_) - (sum_ * sum_) / (float(size_) * float(size_));
}

template <typename T>
inline T VarianceT<T>::deviation() const
{
	const T tempVariance = variance();

	ocean_assert(!(std::is_same<T, float>::value) || tempVariance >= T(-0.1)); // e.g., in case of several add/move calls
	ocean_assert(!(std::is_same<T, double>::value) || tempVariance >= T(-0.0001)); // e.g., in case of several add/move calls

	if (tempVariance <= NumericT<T>::eps())
	{
		return T(0);
	}

	return NumericT<T>::sqrt(tempVariance);
}

template <typename T>
inline T VarianceT<T>::average() const
{
	ocean_assert(size_ > 0);

	return sum_ / T(size_);
}

template <typename T>
inline size_t VarianceT<T>::size() const
{
	return size_;
}

template <typename T>
inline VarianceT<T>::operator bool() const
{
	return size_ != 0;
}

}

#endif // META_OCEAN_MATH_VARIANCE_H
