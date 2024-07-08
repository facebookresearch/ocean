/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_UTILITIES_H
#define META_OCEAN_BASE_UTILITIES_H

#include "ocean/base/Base.h"

#include <climits>
#include <cmath>
#include <limits>

namespace Ocean
{

/**
 * This class implements basic utility functions.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT Utilities
{
	public:

		/**
		 * Separates a set of given values.
		 * @param values Set of values to be separated
		 * @param delimiter The delimiter separating the individual values
		 * @param removeQuotes State determining whether the beginning and ending quotes are removed for each value
		 * @param trim State determining whether prefix and postfix blanks are removed for each value
		 * @return Resulting separated values
		 */
		static std::vector<std::string> separateValues(const std::string& values, const char delimiter = ',', const bool removeQuotes = true, const bool trim = true);

		/**
		 * Sorts two values so that the lowest value will finally be the first value.
		 * The sorting is not stable.<br>
		 * @param value0 First value
		 * @param value1 Second value
		 * @tparam T Data type of the values to be sorted
		 */
		template <typename T>
		static void sortLowestToFront2(T& value0, T& value1);

		/**
		 * Sorts three values so that the lowest value will finally be the first value.
		 * The sorting is not stable.<br>
		 * @param value0 First value
		 * @param value1 Second value
		 * @param value2 Third value
		 * @tparam T Data type of the values to be sorted
		 */
		template <typename T>
		static void sortLowestToFront3(T& value0, T& value1, T& value2);

		/**
		 * Sorts two values so that the lowest value will finally be the first value.
		 * Each value has a connected data value that will be sorted accordingly.<br>
		 * The sorting is not stable.<br>
		 * @param value0 First value
		 * @param value1 Second value
		 * @param data0 First data value
		 * @param data1 Second data value
		 * @tparam T Data type of the values to be sorted
		 */
		template <typename T, typename TData>
		static void sortLowestToFront2(T& value0, T& value1, TData& data0, TData& data1);

		/**
		 * Sorts three values so that the lowest value will finally be the first value.
		 * Each value has a connected data value that will be sorted accordingly.<br>
		 * The sorting is not stable.<br>
		 * @param value0 First value
		 * @param value1 Second value
		 * @param value2 Third value
		 * @param data0 First data value
		 * @param data1 Second data value
		 * @param data2 Third data value
		 * @tparam T Data type of the values to be sorted
		 */
		template <typename T, typename TData>
		static void sortLowestToFront3(T& value0, T& value1, T& value2, TData& data0, TData& data1, TData& data2);

		/**
		 * Sorts two values so that the highest value will finally be the first value.
		 * The sorting is not stable.<br>
		 * @param value0 First value
		 * @param value1 Second value
		 * @tparam T Data type of the values to be sorted
		 */
		template <typename T>
		static void sortHighestToFront2(T& value0, T& value1);

		/**
		 * Sorts three values so that the highest value will finally be the first value.
		 * The sorting is not stable.<br>
		 * @param value0 First value
		 * @param value1 Second value
		 * @param value2 Third value
		 * @tparam T Data type of the values to be sorted
		 */
		template <typename T>
		static void sortHighestToFront3(T& value0, T& value1, T& value2);

		/**
		 * Sorts two values so that the highest value will finally be the first value.
		 * Each value has a connected payload data value that will be sorted accordingly.<br>
		 * The sorting is not stable.<br>
		 * @param value0 First value
		 * @param value1 Second value
		 * @param data0 First data value
		 * @param data1 Second data value
		 * @tparam T Data type of the values to be sorted
		 * @tparam TData Data type of the payload data value
		 */
		template <typename T, typename TData>
		static void sortHighestToFront2(T& value0, T& value1, TData& data0, TData& data1);

		/**
		 * Sorts three values so that the highest value will finally be the first value.
		 * Each value has a connected payload data value that will be sorted accordingly.<br>
		 * The sorting is not stable.<br>
		 * @param value0 First value
		 * @param value1 Second value
		 * @param value2 Third value
		 * @param data0 First data value
		 * @param data1 Second data value
		 * @param data2 Third data value
		 * @tparam T Data type of the values to be sorted
		 * @tparam TData Data type of the payload data value
		 */
		template <typename T, typename TData>
		static void sortHighestToFront3(T& value0, T& value1, T& value2, TData& data0, TData& data1, TData& data2);

		/**
		 * Returns the index of the lowest value, if two or more values have the lowest value the smallest index of the values will be returned.
		 * @param value0 The first value
		 * @param value1 The second value
		 * @param value2 The third value
		 * @return The resulting index with range [0, 2]
		 */
		template <typename T>
		static unsigned int lowestValue(const T& value0, const T& value1, const T& value2);

		/**
		 * Returns the index of the highest value, if two or more values have the highest value the smallest index of the values will be returned.
		 * @param value0 The first value
		 * @param value1 The second value
		 * @param value2 The third value
		 * @return The resulting index with range [0, 2]
		 */
		template <typename T>
		static unsigned int highestValue(const T& value0, const T& value1, const T& value2);

		/**
		 * Compares two pair objects and returns whether the first value of the first pair is lesser than the first value of the second pair.
		 * @param firstPair First pair object
		 * @param secondPair Second pair object
		 * @return True, if so
		 * @tparam T1 The data type of the first value of the pair objects
		 * @tparam T2 The data type of the second value of the pair objects
		 */
		template <typename T1, typename T2>
		static inline bool sortPairFirst(const std::pair<T1, T2>& firstPair, const std::pair<T1, T2>& secondPair);

		/**
		 * Compares two pair objects and returns whether the first value of the first pair is lesser than the first value of the second pair.
		 * @param firstPair First pair object
		 * @param secondPair Second pair object
		 * @return True, if so
		 * @tparam T The data type of the std::pair object
		 */
		template <typename T>
		static inline bool sortPairFirst(const T& firstPair, const T& secondPair);

		/**
		 * Compares two pair objects and returns whether the second value of the first pair is lesser than the second value of the second pair.
		 * @param firstPair First pair object
		 * @param secondPair Second pair object
		 * @return True, if so
		 * @tparam T1 The data type of the first value of the pair objects
		 * @tparam T2 The data type of the second value of the pair objects
		 */
		template <typename T1, typename T2>
		static inline bool sortPairSecond(const std::pair<T1, T2>& firstPair, const std::pair<T1, T2>& secondPair);

		/**
		 * Compares two pair objects and returns whether the second value of the first pair is lesser than the second value of the second pair.
		 * @param firstPair First pair object
		 * @param secondPair Second pair object
		 * @return True, if so
		 * @tparam T The data type of the std::pair object
		 */
		template <typename T>
		static inline bool sortPairSecond(const T& firstPair, const T& secondPair);

		/**
		 * This function divides a given value by two.
		 * Integer values are divided by two (bit shifting is no applied), floating point values are multiplied by 0.5.
		 * @param value The value to be divided
		 * @tparam T The data type of the value
		 * @return The resulting value
		 */
		template <typename T>
		static inline T divideBy2(const T& value);

		/**
		 * This function applies a fast division by 255 for unsigned integer values.
		 * @param value The value that will be divided by 255, with range [0, 65534]
		 * @return Divided unsigned integer value
		 */
		static inline unsigned char divideBy255(const unsigned int value);

		/**
		 * Returns whether a given value is a power of two.
		 * Zero is not a power of two value.
		 * @param value The value to be checked, with range [0, infinity)
		 * @return True, if so
		 */
		static inline bool isPowerOfTwo(const unsigned int value);

		/**
		 * Returns the smallest power of two value that is equal or larger than a given value.
		 * @param value The value for that the smallest power of two value is determined, with range [0, 0x80000000]
		 * @return Smallest power of two value that is equal or larger
		 */
		static inline unsigned int smallestPowerOfTwo(const unsigned int value);

		/**
		 * Multiplies the specified 64 bit integers and stores the 128 bit integer result in resultHigh (64 most significant bits) and resultLow (64 least significant bits).
		 * @param left The left operand
		 * @param right The right operand
		 * @param resultHigh Receives the 64 most significant bits of the 128 bit result
		 * @param resultLow Receives the 64 least significant bits of the 128 bit result
		 */
		static void multiply64(const uint64_t left, const uint64_t right, uint64_t& resultHigh, uint64_t& resultLow);

		/**
		 * Compares the product specified by the first two factors with the product specified by the last two factors.
		 * @param leftFactor1 First factor of left operand
		 * @param leftFactor2 Second factor of left operand
		 * @param rightFactor1 First factor of right operand
		 * @param rightFactor2 Second factor of right operand
		 * @return 1 if the left product is greater than the right product, -1 if it is less and 0 if both are equal.
		 */
		static inline int compareProducts(const uint64_t leftFactor1, const uint64_t leftFactor2, const uint64_t rightFactor1, const uint64_t rightFactor2);
};

/**
 * This function fits a given parameter into a specified value range.
 * If the given parameter is lesser the lower boundary, the lower boundary value is returned.
 * If the given parameter is higher the upper boundary, the upper boundary value is returned.
 * @param lowerBoundary Lower boundary value, with range (-infinity, upperBoundary]
 * @param value The value to be fitted
 * @param upperBoundary Upper boundary value, with range [lowerBoundary, infinity)
 * @return Value fitting into the given range
 * @tparam T Data type of the values
 * @ingroup base
 */
template <typename T>
inline T minmax(const T& lowerBoundary, const T& value, const T& upperBoundary);

/**
 * Returns the modulo value of a given parameter within a ring allowing positive and negative parameters.
 * The function provides a modulo result for a ring size of 5 as below:<br>
 * <pre>
 * Original: -7 -6  | -5 -4 -3 -2 -1  |  0  1  2  3  4  |  5  6  7  9 ...
 *   Result:  3  4  |  0  1  2  3  4  |  0  1  2  3  4  |  0  1  2  3 ...
 * </pre>
 * @param value Parameter to return the module value for
 * @param ring The ring size, must be positive with range [1, infinity)
 * @return Resulting modulo value
 * @tparam T Data type of the values
 * @ingroup base
 */
template <typename T>
inline T modulo(const T& value, const T& ring);

/**
 * Returns the distance between two values inside defined ring.
 * @param value0 First value inside the ring, with range [0, ring)
 * @param value1 Second value inside the ring, with range [0, ring)
 * @param ring The ring size, must be positive
 * @return Resulting distance
 * @tparam T Data type of the values
 * @ingroup base
 */
template <typename T>
inline T ringDistance(const T& value0, const T& value1, const T& ring);

/**
 * Returns the square value of a given value.
 * @param value The value to be squared
 * @return Squared value
 * @ingroup base
 */
inline unsigned int sqr(const char value);

/**
 * Returns the square value of a given value.
 * @param value The value to be squared
 * @return Squared value
 * @ingroup base
 */
inline unsigned int sqr(const unsigned char value);

/**
 * Returns the square value of a given value.
 * @param value The value to be squared
 * @return Squared value
 * @ingroup base
 */
inline unsigned int sqr(const short value);

/**
 * Returns the square value of a given value.
 * @param value The value to be squared
 * @return Squared value
 * @ingroup base
 */
inline unsigned int sqr(const unsigned short value);

/**
 * Returns the square value of a given value.
 * @param value The value to be squared
 * @return Squared value
 * @ingroup base
 */
inline unsigned int sqr(const int value);

/**
 * Returns the square value of a given value.
 * @param value The value to be squared
 * @return Squared value
 * @ingroup base
 */
inline unsigned int sqr(const unsigned int value);

/**
 * Returns the square value of a given value.
 * @param value The value to be squared
 * @return Squared value
 * @ingroup base
 */
inline unsigned long sqr(const long value);

/**
 * Returns the square value of a given value.
 * @param value The value to be squared
 * @return Squared value
 * @ingroup base
 */
inline unsigned long sqr(const unsigned long value);

/**
 * Returns the square value of a given value.
 * @param value The value to be squared
 * @return Squared value
 * @ingroup base
 */
inline unsigned long long sqr(const long long value);

/**
 * Returns the square value of a given value.
 * @param value The value to be squared
 * @return Squared value
 * @ingroup base
 */
inline unsigned long long sqr(const unsigned long long value);

/**
 * Returns the square value of a given value.
 * @param value The value to be squared
 * @return Squared value
 * @ingroup base
 */
inline float sqr(const float value);

/**
 * Returns the square value of a given value.
 * @param value The value to be squared
 * @return Squared value
 * @ingroup base
 */
inline double sqr(const double value);

/**
 * Returns the square distance between two values.
 * @param first The first value
 * @param second The second value
 * @return Square distance
 * @ingroup base
 */
inline unsigned int sqrDistance(const char first, const char second);

/**
 * Returns the square distance between two values.
 * @param first The first value
 * @param second The second value
 * @return Square distance
 * @ingroup base
 */
inline unsigned int sqrDistance(const unsigned char first, const unsigned char second);

/**
 * Returns the square distance between two values.
 * @param first The first value
 * @param second The second value
 * @return Square distance
 * @ingroup base
 */
inline unsigned int sqrDistance(const short first, const short second);

/**
 * Returns the square distance between two values.
 * @param first The first value
 * @param second The second value
 * @return Square distance
 * @ingroup base
 */
inline unsigned int sqrDistance(const unsigned short first, const unsigned short second);

/**
 * Returns the square distance between two values.
 * @param first The first value
 * @param second The second value
 * @return Square distance
 * @ingroup base
 */
inline unsigned int sqrDistance(const int first, const int second);

/**
 * Returns the square distance between two values.
 * @param first The first value
 * @param second The second value
 * @return Square distance
 * @ingroup base
 */
inline unsigned int sqrDistance(const unsigned int first, const unsigned int second);

/**
 * Returns the square distance between two values.
 * @param first The first value
 * @param second The second value
 * @return Square distance
 * @ingroup base
 */
inline unsigned long long sqrDistance(const long long first, const long long second);

/**
 * Returns the square distance between two values.
 * @param first The first value
 * @param second The second value
 * @return Square distance
 * @ingroup base
 */
inline unsigned long long sqrDistance(const unsigned long long first, const unsigned long long second);

/**
 * Returns the square distance between two values.
 * @param first The first value
 * @param second The second value
 * @return Square distance
 * @ingroup base
 */
inline float sqrDistance(const float first, const float second);

/**
 * Returns the square distance between two values.
 * @param first The first value
 * @param second The second value
 * @return Square distance
 * @ingroup base
 */
inline double sqrDistance(const double first, const double second);

/**
 * Returns the absolute distance between two values.
 * @param first The first value
 * @param second The second value
 * @return Square distance
 * @ingroup base
 */
inline unsigned char absDistance(const char first, const char second);

/**
 * Returns the absolute distance between two values.
 * @param first The first value
 * @param second The second value
 * @return Square distance
 * @ingroup base
 */
inline unsigned char absDistance(const unsigned char first, const unsigned char second);

/**
 * Returns the absolute distance between two values.
 * @param first The first value
 * @param second The second value
 * @return Square distance
 * @ingroup base
 */
inline unsigned short absDistance(const short first, const short second);

/**
 * Returns the absolute distance between two values.
 * @param first The first value
 * @param second The second value
 * @return Square distance
 * @ingroup base
 */
inline unsigned short absDistance(const unsigned short first, const unsigned short second);

/**
 * Returns the absolute distance between two values.
 * @param first The first value
 * @param second The second value
 * @return Square distance
 * @ingroup base
 */
inline unsigned int absDistance(const int first, const int second);

/**
 * Returns the absolute distance between two values.
 * @param first The first value
 * @param second The second value
 * @return Square distance
 * @ingroup base
 */
inline unsigned int absDistance(const unsigned int first, const unsigned int second);

/**
 * Returns the absolute distance between two values.
 * @param first The first value
 * @param second The second value
 * @return Square distance
 * @ingroup base
 */
inline unsigned long long absDistance(const long long first, const long long second);

/**
 * Returns the absolute distance between two values.
 * @param first The first value
 * @param second The second value
 * @return Square distance
 * @ingroup base
 */
inline unsigned long long absDistance(const unsigned long long first, const unsigned long long second);

/**
 * Mirrors a given value if necessary.
 * The values is mirrored according to a given size parameter.<br>
 * The function provides a result as below:<br>
 * <pre>
 * Original: -3 -2 -1 |  0  1  2  3  4  5  6 ... s-2  s-1 |   s  s+1  s+2
 *   Result:  2  1  0 |  0  1  2  3  4  5  6 ... s-2  s-1 | s-1  s-2  s-3
 * </pre>
 * @param value The value to be mirrored, with range [-size, 2*size]
 * @param size Specified size defining the upper mirror border, with range [1, 2147483647]
 * @return Mirrored value
 * @ingroup base
 */
static inline unsigned int mirrorValue(const int value, const unsigned int size);

/**
 * Determines the entire number of elements that are stored in a vector holding a vector of elements.
 * @param vectors Set of given vector
 * @return Resulting entire number of elements
 * @tparam T Data type of the vector elements
 * @ingroup base
 */
template <typename T>
static inline size_t countElements(const std::vector<std::vector<T>>& vectors);

/**
 * Returns whether a vector holds a specified element.
 * @param elements Vector to be checked
 * @param element The element that has to be found
 * @return True, if so
 */
template <typename T>
static inline bool hasElement(const std::vector<T>& elements, const T& element);

/**
 * Concatenates a given set of vectors to one vector.
 * @param vectors Set of given vectors
 * @return Resulting unique vector
 * @tparam T Data type of the vector elements
 * @ingroup base
 */
template <typename T>
static inline std::vector<T> concatenate(const std::vector<std::vector<T>>& vectors);

/**
 * Fills a vector with increasing index values.
 * @param numberIndices Number of indices that will be created, with range [1, infinity)
 * @param startIndex Start index value, successive indices will be incremented, with range [0, infinity)
 * @param indices The vector receiving the indices
 * @tparam T Data type of the index elements
 * @ingroup base
 */
template <typename T>
static inline void createIndices(const size_t numberIndices, const T& startIndex, std::vector<T>& indices);

/**
 * Returns a vector with increasing index values.
 * @param numberIndices Number of indices that will be created, with range [1, infinity)
 * @param startIndex Start index value, successive indices will be incremented, with range [0, infinity)
 * @return The resulting index vector
 * @tparam T Data type of the index elements
 * @ingroup base
 */
template <typename T>
[[nodiscard]] static inline std::vector<T> createIndices(const size_t numberIndices, const T& startIndex);

template <typename T>
void Utilities::sortLowestToFront2(T& value0, T& value1)
{
	if (!(value0 < value1))
	{
		std::swap(value0, value1);
	}

	ocean_assert(value0 <= value1);
}

template <typename T>
void Utilities::sortLowestToFront3(T& value0, T& value1, T& value2)
{
	if (!(value0 < value1))
	{
		std::swap(value0, value1);
	}

	if (!(value0 < value2))
	{
		std::swap(value0, value2);
	}

	if (!(value1 < value2))
	{
		std::swap(value1, value2);
	}

	ocean_assert(value0 <= value1);
	ocean_assert(value1 <= value2);
}

template <typename T, typename TData>
void Utilities::sortLowestToFront2(T& value0, T& value1, TData& data0, TData& data1)
{
	if (!(value0 < value1))
	{
		std::swap(value0, value1);
		std::swap(data0, data1);
	}

	ocean_assert(value0 <= value1);
}

template <typename T, typename TData>
void Utilities::sortLowestToFront3(T& value0, T& value1, T& value2, TData& data0, TData& data1, TData& data2)
{
	if (!(value0 < value1))
	{
		std::swap(value0, value1);
		std::swap(data0, data1);
	}

	if (!(value0 < value2))
	{
		std::swap(value0, value2);
		std::swap(data0, data2);
	}

	if (!(value1 < value2))
	{
		std::swap(value1, value2);
		std::swap(data1, data2);
	}

	ocean_assert(value0 <= value1);
	ocean_assert(value1 <= value2);
}

template <typename T>
void Utilities::sortHighestToFront2(T& value0, T& value1)
{
	if (value0 < value1)
	{
		std::swap(value0, value1);
	}

	ocean_assert(value0 >= value1);
}

template <typename T>
void Utilities::sortHighestToFront3(T& value0, T& value1, T& value2)
{
	if (value0 < value1)
	{
		std::swap(value0, value1);
	}

	if (value0 < value2)
	{
		std::swap(value0, value2);
	}

	if (value1 < value2)
	{
		std::swap(value1, value2);
	}

	ocean_assert(value0 >= value1);
	ocean_assert(value1 >= value2);
}

template <typename T, typename TData>
void Utilities::sortHighestToFront2(T& value0, T& value1, TData& data0, TData& data1)
{
	if (value0 < value1)
	{
		std::swap(value0, value1);
		std::swap(data0, data1);
	}

	ocean_assert(value0 >= value1);
}

template <typename T, typename TData>
void Utilities::sortHighestToFront3(T& value0, T& value1, T& value2, TData& data0, TData& data1, TData& data2)
{
	if (value0 < value1)
	{
		std::swap(value0, value1);
		std::swap(data0, data1);
	}

	if (value0 < value2)
	{
		std::swap(value0, value2);
		std::swap(data0, data2);
	}

	if (value1 < value2)
	{
		std::swap(value1, value2);
		std::swap(data1, data2);
	}

	ocean_assert(value0 >= value1);
	ocean_assert(value1 >= value2);
}

template <typename T>
unsigned int Utilities::lowestValue(const T& value0, const T& value1, const T& value2)
{
	if (value0 <= value1)
	{
		if (value0 <= value2)
		{
			return 0u;
		}
		else
		{
			return 2u;
		}
	}
	else
	{
		if (value1 <= value2)
		{
			return 1u;
		}
		else
		{
			return 2u;
		}
	}
}

template <typename T>
unsigned int Utilities::highestValue(const T& value0, const T& value1, const T& value2)
{
	if (value0 >= value1)
	{
		if (value0 >= value2)
		{
			return 0u;
		}
		else
		{
			return 2u;
		}
	}
	else
	{
		if (value1 >= value2)
		{
			return 1u;
		}
		else
		{
			return 2u;
		}
	}
}

template <typename T1, typename T2>
inline bool Utilities::sortPairFirst(const std::pair<T1, T2>& firstPair, const std::pair<T1, T2>& secondPair)
{
	return firstPair.first < secondPair.first;
}

template <typename T>
inline bool Utilities::sortPairFirst(const T& firstPair, const T& secondPair)
{
	return firstPair.first < secondPair.first;
}

template <typename T1, typename T2>
inline bool Utilities::sortPairSecond(const std::pair<T1, T2>& firstPair, const std::pair<T1, T2>& secondPair)
{
	return firstPair.second < secondPair.second;
}

template <typename T>
inline bool Utilities::sortPairSecond(const T& firstPair, const T& secondPair)
{
	return firstPair.second < secondPair.second;
}

template <typename T>
inline T Utilities::divideBy2(const T& value)
{
	return value / T(2);
}

template <>
inline float Utilities::divideBy2(const float& value)
{
	return value * 0.5f;
}

template <>
inline double Utilities::divideBy2(const double& value)
{
	return value * 0.5;
}

inline unsigned char Utilities::divideBy255(const unsigned int value)
{
	ocean_assert(value < 65535u);
	ocean_assert(value / 255u == (value + 1u + (value >> 8u)) >> 8u);

	return (unsigned char)((value + 1u + (value >> 8u)) >> 8u);
}

inline bool Utilities::isPowerOfTwo(const unsigned int value)
{
	// we determine/extract the least significant bit and check whether the value is still equal:
	//              value: 11010011000
	//            value-1: 11010010111
	//         ~(value-1): 00101101000
	// value & ~(value-1): 00000001000

	// return value != 0u && (value == (value & ~(value - 1u)));

	// an even faster way is to check (for all values larger than zero):
	// (value & (value - 1u)) == 0

	ocean_assert((value == 1u || value == 2u || value == 4u || value == 8u || value == 16u || value == 32u || value == 64u
					|| value == 128u || value == 256u || value == 512u || value == 1024u || value == 2048u
					|| value == 4096u || value == 8192 || value == 16384u || value == 32768u || value == 65536u
					|| value == 131072 || value == 262144u || value == 524288u || value == 1048576u || value == 2097152u
					|| value == 4194304u || value == 8388608u || value == 16777216u || value == 33554432u || value == 67108864u
					|| value == 134217728u || value == 268435456u || value == 536870912u || value == 1073741824u || value == 2147483648u) == (value != 0u && (value & (value - 1u)) == 0u));

	return value != 0u && (value & (value - 1u)) == 0u;
}

inline unsigned int Utilities::smallestPowerOfTwo(const unsigned int value)
{
	ocean_assert(value <= 0x80000000u);

	unsigned int powerOfTwoValue = 0x80000000u;

	while (powerOfTwoValue != 0u && (powerOfTwoValue >> 1) >= value)
	{
		powerOfTwoValue >>= 1u;
	}

	return powerOfTwoValue;
}

#ifdef __GNUC__

// The GCC compiler provides faster code with this minmax determination
template <typename T>
inline T minmax(const T& lowerBoundary, const T& value, const T& upperBoundary)
{
	ocean_assert(lowerBoundary <= upperBoundary);

	ocean_assert((value < lowerBoundary ? lowerBoundary : (value > upperBoundary ? upperBoundary : value)) == (max(lowerBoundary, min(value, upperBoundary))));
	return value < lowerBoundary ? lowerBoundary : (value > upperBoundary ? upperBoundary : value);
}

#else

// The Microsoft VC compiler provides faster code with this minmax determination
template <typename T>
inline T minmax(const T& lowerBoundary, const T& value, const T& upperBoundary)
{
	ocean_assert(lowerBoundary <= upperBoundary);
	return max(lowerBoundary, min(upperBoundary, value));
}

#endif // __GNUC__

template <typename T>
inline T modulo(const T& value, const T& ring)
{
	ocean_assert(ring > 0);

	if (value >= 0)
	{
		if (value < ring)
		{
			return value;
		}

		ocean_assert(value % ring < ring);
		return value % ring;
	}
	else
	{
		const T result = (ring - 1) - (-(value + 1) % ring);
		ocean_assert(result >= 0 && result < ring);

		return result;
	}
}

template <>
inline unsigned char modulo(const unsigned char& value, const unsigned char& ring)
{
	return value % ring;
}

template <>
inline unsigned short modulo(const unsigned short& value, const unsigned short& ring)
{
	return value % ring;
}

template <>
inline unsigned int modulo(const unsigned int& value, const unsigned int& ring)
{
	return value % ring;
}

template <>
inline unsigned long long modulo(const unsigned long long& value, const unsigned long long& ring)
{
	return value % ring;
}

template <>
inline double modulo(const double& value, const double& ring)
{
	ocean_assert(ring > 0);

	if (value >= 0)
	{
		if (value < ring)
		{
			return value;
		}

		return fmod(value, ring);
	}
	else
	{
		const double intermediate = ring - (fmod(-value, ring));
		ocean_assert(intermediate >= 0.0 && intermediate <= ring);

		return intermediate == ring ? 0.0 : intermediate;
	}
}

template <>
inline float modulo(const float& value, const float& ring)
{
	ocean_assert(ring > 0);

	if (value >= 0)
	{
		if (value < ring)
		{
			return value;
		}

		return fmod(value, ring);
	}
	else
	{
		const float intermediate = ring - (fmod(-value, ring));
		ocean_assert(intermediate >= 0.0f && intermediate <= ring);

		return intermediate == ring ? 0.0f : intermediate;
	}
}

template <typename T>
inline T ringDistance(const T& value0, const T& value1, const T& ring)
{
	ocean_assert(value0 >= 0 && value0 < ring);
	ocean_assert(value1 >= 0 && value1 < ring);

	const T low = min(value0, value1);
	const T high = max(value0, value1);

	return min(high - low, low + ring - high);
}

inline unsigned int sqr(const char value)
{
	return value * value;
}

inline unsigned int sqr(const unsigned char value)
{
	return value * value;
}

inline unsigned int sqr(const short value)
{
	return value * value;
}

inline unsigned int sqr(const unsigned short value)
{
	return value * value;
}

inline unsigned int sqr(const int value)
{
	return value * value;
}

inline unsigned int sqr(const unsigned int value)
{
	return value * value;
}

inline unsigned long sqr(const long value)
{
	return value * value;
}

inline unsigned long sqr(const unsigned long value)
{
	return value * value;
}

inline unsigned long long sqr(const long long value)
{
	return value * value;
}

inline unsigned long long sqr(const unsigned long long value)
{
	return value * value;
}

inline float sqr(const float value)
{
	return value * value;
}

inline double sqr(const double value)
{
	return value * value;
}

inline unsigned int sqrDistance(const char first, const char second)
{
	const int distance = int(first) - int(second);
	return distance * distance;
}

inline unsigned int sqrDistance(const unsigned char first, const unsigned char second)
{
	const int distance = int(first) - int(second);
	return distance * distance;
}

inline unsigned int sqrDistance(const short first, const short second)
{
	const int distance = int(first) - int(second);
	return distance * distance;
}

inline unsigned int sqrDistance(const unsigned short first, const unsigned short second)
{
	const int distance = int(first) - int(second);
	return distance * distance;
}

inline unsigned int sqrDistance(const int first, const int second)
{
	const int distance = first - second;
	return distance * distance;
}

inline unsigned int sqrDistance(const unsigned int first, const unsigned int second)
{
	ocean_assert(first <= (unsigned int)INT_MAX);
	ocean_assert(second <= (unsigned int)INT_MAX);

	const int distance = int(first) - int(second);
	return distance * distance;
}

inline unsigned long long sqrDistance(const long long first, const long long second)
{
	const long long distance = first - second;
	return distance * distance;
}

inline unsigned long long sqrDistance(const unsigned long long first, const unsigned long long second)
{
	ocean_assert(first <= (unsigned long long)LLONG_MAX);
	ocean_assert(second <= (unsigned long long)LLONG_MAX);

	const long long distance = (long long)first - (long long)second;
	return distance * distance;
}

inline float sqrDistance(const float first, const float second)
{
	const float distance = first - second;
	return distance * distance;
}

inline double sqrDistance(const double first, const double second)
{
	const double distance = first - second;
	return distance * distance;
}

inline unsigned char absDistance(const char first, const char second)
{
	return (unsigned char)(::abs(int(first) - int(second)));
}

inline unsigned char absDistance(const unsigned char first, const unsigned char second)
{
	return (unsigned char)(::abs(int(first - second)));
}

inline unsigned short absDistance(const short first, const short second)
{
	return (unsigned short)(::abs(int(first) - int(second)));
}

inline unsigned short absDistance(const unsigned short first, const unsigned short second)
{
	return (unsigned short)(::abs(int(first - second)));
}

inline unsigned int absDistance(const int first, const int second)
{
	return (unsigned int)(::abs(first - second));
}

inline unsigned int absDistance(const unsigned int first, const unsigned int second)
{
	return (unsigned int)(::abs(int(first - second)));
}

inline unsigned long long absDistance(const long long first, const long long second)
{
#if defined(__clang__)
	return (first > second) ? (first - second) : (second - first);
#elif (defined(OCEAN_COMPILER_MSC) && OCEAN_MSC_VERSION <= 1500)
	return (first > second) ? (first - second) : (second - first);
#else
	return ::abs(first - second);
#endif
}

inline unsigned long long absDistance(const unsigned long long first, const unsigned long long second)
{
#if defined(__clang__)
	return (first > second) ? (first - second) : (second - first);
#elif (defined(OCEAN_COMPILER_MSC) && OCEAN_MSC_VERSION <= 1500)
	return (first > second) ? (first - second) : (second - first);
#else
	return ::abs((long long)(first - second));
#endif
}

inline unsigned int mirrorValue(const int value, const unsigned int size)
{
	ocean_assert(size != 0u);
	ocean_assert(size <= 2147483647);
	ocean_assert(value >= -int(size) && value <= int(size * 2));

#ifdef OCEAN_DEBUG
	const int result = (value < 0) ? (-value - 1) : (value >= int(size) ? (int(size + size) - value - 1) : value);
	ocean_assert(result >= 0 && result < int(size));
#endif // OCEAN_DEBUG

	return (value < 0) ? (-value - 1) : (value >= int(size) ? (int(size + size) - value - 1) : value);
}

template <typename T>
inline size_t countElements(const std::vector<std::vector<T>>& vectors)
{
	size_t size = 0;

	for (size_t n = 0; n < vectors.size(); ++n)
	{
		size += vectors[n].size();
	}

	return size;
}

template <typename T>
inline bool hasElement(const std::vector<T>& elements, const T& element)
{
	for (const T& value : elements)
	{
		if (value == element)
		{
			return true;
		}
	}

	return false;
}

template <typename T>
inline std::vector<T> concatenate(const std::vector<std::vector<T>>& vectors)
{
	const size_t size = countElements(vectors);

	std::vector<T> result;
	result.reserve(size);

	for (size_t n = 0; n < vectors.size(); ++n)
	{
		result.insert(result.end(), vectors[n].begin(), vectors[n].end());
	}

	return result;
}

template <typename T>
inline void createIndices(const size_t numberIndices, const T& startIndex, std::vector<T>& indices)
{
	ocean_assert(numberIndices > 0);

	indices.reserve(numberIndices);
	indices.clear();

	T index = startIndex;

	for (size_t n = 0; n < numberIndices; ++n)
	{
		indices.emplace_back(index++);
	}
}

template <typename T>
inline std::vector<T> createIndices(const size_t numberIndices, const T& startIndex)
{
	std::vector<T> result;
	createIndices(numberIndices, startIndex, result);

	return result;
}

int Utilities::compareProducts(const uint64_t leftFactor1, const uint64_t leftFactor2, const uint64_t rightFactor1, const uint64_t rightFactor2)
{
	uint64_t leftProductHigh, leftProductLow;
	multiply64(leftFactor1, leftFactor2, leftProductHigh, leftProductLow);

	uint64_t rightProductHigh, rightProductLow;
	multiply64(rightFactor1, rightFactor2, rightProductHigh, rightProductLow);

	return leftProductHigh == rightProductHigh ? (leftProductLow > rightProductLow) - (leftProductLow < rightProductLow) : (leftProductHigh > rightProductHigh) - (leftProductHigh < rightProductHigh);
}


}

#endif // META_OCEAN_BASE_UTILITIES_H
