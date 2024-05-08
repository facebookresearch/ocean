/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_NUMERIC_H
#define META_OCEAN_MATH_NUMERIC_H

#include "ocean/math/Math.h"

#include "ocean/base/DataType.h"

#include <cfloat>
#include <climits>
#include <cmath>
#include <complex>
#include <cstdlib>
#include <limits>

namespace Ocean
{

// Forward declaration.
template <typename T> class NumericT;

/**
 * Definition of a Numeric class.
 * @see NumericT
 * @ingroup math
 */
typedef NumericT<Scalar> Numeric;

/**
 * Definition of a Numeric class with double values.
 * @see NumericT
 * @ingroup math
 */
typedef NumericT<double> NumericD;

/**
 * Definition of a Numeric class with float values.
 * @see NumericT
 * @ingroup math
 */
typedef NumericT<float> NumericF;

/**
 * This class provides basic numeric functionalities.
 * @tparam T Data type of the numeric elements
 * @see Numeric, NumericF, NumericD.
 * @ingroup math
 */
template <typename T>
class NumericT
{
	public:

		/**
		 * Returns PI which is equivalent to 180 degree.
		 * @return PI
		 */
		static constexpr T pi();

		/**
		 * Returns 2*PI which is equivalent to 360 degree.
		 * @return 2*PI
		 */
		static constexpr T pi2();

		/**
		 * Returns PI/2 which is equivalent to 90 degree.
		 * @return PI/2
		 */
		static constexpr T pi_2();

		/**
		 * Returns PI/3 which is equivalent to 60 degree.
		 * @return PI/3
		 */
		static constexpr T pi_3();

		/**
		 * Returns PI/4 which is equivalent to 45 degree.
		 * @return PI/4
		 */
		static constexpr T pi_4();

		/**
		 * Returns the square of PI i.e., PI^2.
		 * @return PI^2
		 */
		static constexpr T squarePi();

		/**
		 * Returns the square of two PI i.e., (2*PI)^2.
		 * @return (2*PI)^2
		 */
		static constexpr T squarePi2();

		/**
		 * Returns the square of PI half i.e., (PI/2)^2.
		 * @return (PI/2)^2
		 */
		static constexpr T squarePi_2();

		/**
		 * Returns the euler's number.
		 * @return Euler's number
		 */
		static constexpr T e();

		/**
		 * Returns a small epsilon.
		 * @return Small epsilon
		 */
		static constexpr T eps();

		/**
		 * Returns a weak epsilon.
		 * @return Weak epsilon
		 */
		static constexpr T weakEps();

		/**
		 * Returns the absolute value of a given value.
		 * Beware some values do not have a corresponding absolute value e.g., -128 for (8bit signed chars), -32768 for (16bit signed chars), and so on.
		 * @param value The value for which the absolute value will be returned
		 * @return Absolute value
		 * @see secureAbs().
		 */
		static inline T abs(const T value);

		/**
		 * Returns the absolute value of a given value while the return value is guaranteed to be the absolute value of the input value.
		 * This function returns the unsigned data type T (if possible) and ensures that the absolute value of the minimal integer value is correct.
		 * @param value The value for which the absolute value will be returned
		 * @return Absolute value
		 * @see abs().
		 */
		static inline typename UnsignedTyper<T>::Type secureAbs(const T value);

		/**
		 * Returns the square of a given value.
		 * @param value The value to return the square from
		 * @return Square value
		 */
		static constexpr inline T sqr(const T value);

		/**
		 * Returns the sum of given values.
		 * @param values The values to return the sum from
		 * @param number The number of given values
		 * @return Sum of elements
		 */
		static inline T sum(const T* values, const size_t number);

		/**
		 * Returns the summed squares of a given values.
		 * @param values The values to return the summed squares from
		 * @param number The number of given values
		 * @return Summed squared result
		 */
		static inline T summedSqr(const T* values, const size_t number);

		/**
		 * Returns the square distance between two values.
		 * @param value0 First value
		 * @param value1 Second value
		 * @return Square distance
		 */
		static constexpr inline T sqrDistance(const T value0, const T value1);

		/**
		 * Returns the square root of a given value.
		 * @param value The value to return the square root from, with range [0, infinity)
		 * @return Square root
		 */
		static inline T sqrt(const T value);

		/**
		 * Returns the square root of a given value.
		 * @param value The value to return the square root from
		 * @return Square root
		 */
		static inline std::complex<T> sqrt(const std::complex<T>& value);

		/**
		 * Returns the sine of a given value.
		 * @param value The value to return the sine from, with range (-infinity, infinity)
		 * @return Sine value
		 */
		static inline T sin(const T value);

		/**
		 * Returns the cosine of a given value.
		 * @param value The value to return the cosine from, with range (-infinity, infinity)
		 * @return Cosine value
		 */
		static inline T cos(const T value);

		/**
		 * Returns the tangent of a given value.
		 * @param value The value to return the tangent from, with range (-infinity, infinity)
		 * @return Tangent value
		 */
		static inline T tan(const T value);

		/**
		 * Returns the arcsine of a given value.
		 * @param value The value to return the arc sine from, with range [-1, 1]
		 * @return Arcsine value, with range [-PI/2, PI/2]
		 */
		static inline T asin(const T value);

		/**
		 * Returns the arccosine of a given value.
		 * @param value The value to return the arc cosine from, with range [-1, 1]
		 * @return Arccosine value in radian, with range [0, PI]
		 */
		static inline T acos(const T value);

		/**
		 * Returns the arctangent of a given value.
		 * @param value The value to return the arctangent from
		 * @return Arctangent value, with range [-PI/2, PI/2]
		 */
		static inline T atan(const T value);

		/**
		 * Returns the arctangent of a given value in radian.
		 * @param y Opposite value, must not be zero if x is zero
		 * @param x Adjacent value, must not be zero if y is zero
		 * @return Arctangent value, with range [-PI, PI]
		 */
		static inline T atan2(const T y, const T x);

		/**
		 * Returns the base-e exponential function of a given value.
		 * @param value The value to determine the exponential value for, with range (-infinity, infinity)
		 * @return Exponential value
		 */
		static inline T exp(const T value);

		/**
		 * Returns the natural logarithm of a given value (the logarithm to the base e).
		 * @param value The value to return the logarithm for, with range (0, infinity)
		 * @return Natural logarithm
		 */
		static inline T log(const T value);

		/**
		 * Returns the natural logarithm of a given value plus 1 (the logarithm to the base e).
		 * This function has better precision than log(1 + value) when value is close to 0.
		 * @param value Input for which logarithm of (1 + value) will be returned, with range (-1, infinity)
		 * @return Natural logarithm
		 */
		static inline T log1p(const T value);

		/**
		 * Returns the logarithm to base 2 of a given value.
		 * @param value The value to return the logarithm for, with range (0, infinity)
		 * @return Logarithm to base 2
		 */
		static inline T log2(const T value);

		/**
		 * Returns the logarithm to base 10 of a given value.
		 * @param value The value to return the logarithm for, with range (0, infinity)
		 * @return Logarithm to base 10
		 */
		static inline T log10(const T value);

		/**
		 * Returns the floating-point remainder of a given value.
		 * @param valueA Value to return the floating-point remainder from
		 * @param valueB Divisor value
		 * @return Floating-point remainder
		 */
		static inline T fmod(const T valueA, const T valueB);

		/**
		 * Returns the dot product for two vectors.
		 * @param vectorA The first vector, can be nullptr if size is 0
		 * @param vectorB The second vector, can be nullptr if size is 0
		 * @param size The size/dimension of both vector (the number of elements each vector holds), with range [0, infinity)
		 * @return The resulting dot product
		 */
		static inline T dot(const T* vectorA, const T* vectorB, const size_t size);

		/**
		 * Adjusts an arbitrary angle into the range of [0.0, 2PI).
		 * @param angle The angle to be adjusted, defined in radian, with range (-infinity, infinity)
		 * @return Adjusted angle with range [0.0, 2PI)
		 */
		static inline T angleAdjustPositive(const T angle);

		/**
		 * Adjusts an arbitrary angle into the range of [0.0, PI).
		 * @param angle The angle to be adjusted, defined in radian, with range (-infinity, infinity)
		 * @return Adjusted angle with range [0.0, PI)
		 */
		static inline T angleAdjustPositiveHalf(const T angle);

		/**
		 * Adjusts an arbitrary angle into the range of (-PI, PI].
		 * @param angle The angle to be adjusted, defined in radian, with range (-infinity, infinity)
		 * @return Adjusted angle with range (-PI, PI]
		 */
		static inline T angleAdjustNull(const T angle);

		/**
		 * Returns whether two angles represent the same angle up to a small epsilon.
		 * @param angleA First angle specified in radian, with range (-infinity, infinity)
		 * @param angleB Second angle specified in radian, with range (-infinity, infinity)
		 * @return True, if so
		 */
		static inline bool angleIsEqual(const T angleA, const T angleB);

		/**
		 * Returns whether two angles represent the same angle up to a weak epsilon.
		 * @param angleA First angle specified in radian, with range (-infinity, infinity)
		 * @param angleB Second angle specified in radian, with range (-infinity, infinity)
		 * @return True, if so
		 */
		static inline bool angleIsWeakEqual(const T angleA, const T angleB);

		/**
		 * Returns whether two angles represent the same angle up to a given epsilon.
		 * @param angleA First angle specified in radian, with range (-infinity, infinity)
		 * @param angleB Second angle specified in radian, with range (-infinity, infinity)
		 * @param epsilon Accuracy epsilon in radian, with range [0, infinity)
		 * @return True, if so
		 */
		static inline bool angleIsEqual(const T angleA, const T angleB, const T epsilon);

		/**
		 * Returns whether the angular difference (distance) between two angles is smaller than a specified threshold.
		 * @param angleA First angle specified in radian, with range (-infinity, infinity)
		 * @param angleB Second angle specified in radian, with range (-infinity, infinity)
		 * @param threshold Positive angular threshold in radian with range [0, PI)
		 * @return True, if so
		 */
		static inline bool angleIsBelowThreshold(const T angleA, const T angleB, const T threshold);

		/**
		 * Returns the angular difference (distance between two angles).
		 * @param angleA First angle specified in radian, with range (-infinity, infinity)
		 * @param angleB Second angle specified in radian, with range (-infinity, infinity)
		 * @return Absolute distance between both angles, in radian
		 */
		static inline T angleDistance(const T angleA, const T angleB);

		/**
		 * Returns x raised to the power of y.
		 * @param x X value, with range (-infinity, infinity), must not be negative if y < 1
		 * @param y Y value, with range (-infinity, infinity)
		 * @return The power result
		 */
		static inline T pow(const T x, const T y);

		/**
		 * Returns x raised to the power of y.
		 * @param x X value
		 * @param y Y value
		 * @return The power result
		 */
		static inline std::complex<T> pow(const std::complex<T>& x, const T y);

		/**
		 * Returns x raised to the power of y while y is an integer.
		 * @param x X value, with range [0, infinity)
		 * @param y Y value, with range (-infinity, infinity)
		 * @return The power result
		 */
		static constexpr T integerPow(const T x, const unsigned int y);

		/**
		 * Returns the factorial for a given value.
		 * @param value The value for that the factorial will be determined, with range [0, infinity) for floating point types; with range [0, 12] for 32 bit integer types
		 * @return Resulting factorial, with range [0, infinity)
		 */
		static constexpr inline T factorial(const T& value);

		/**
		 * Returns the binomial coefficient for two binomial parameters.
		 * This function calculates: n! / (k! * (n - k)!).<br>
		 * @param n Upper binomial coefficient parameter, with range [0, infinity)
		 * @param k Lower binomial coefficient parameter, with range [0, n]
		 * @return Resulting binomial coefficient
		 */
		static constexpr inline T binomialCoefficient(const T& n, const T& k);

		/**
		 * Returns the smallest integer value that is not less than the given value.
		 * @param value The value to handle, with range (-infinity, infinity)
		 * @return Smallest integer value
		 */
		static inline T ceil(const T value);

		/**
		 * Returns the largest integer value that is not greater than the given value.
		 * @param value The value to handle, with range (-infinity, infinity)
		 * @return Largest integer value
		 */
		static inline T floor(const T value);

		/**
		 * Returns the rounded 32 bit integer value of a given value.
		 * The function applies a symmetric rounding:
		 * <pre>
		 * Input:     -1.50   -1.49  -0.50   -0.49   |   0.49   0.50    1.49   1.50
		 * Output:    -2      -1     -1       0      |   0      1       1      2
		 *
		 * So that for positive integer x:
		 * x  <-  [x - 0.5, x + 0.5)
		 *
		 * and for negative integer x:
		 * x  <-  (x - 0.5, x + 0.5]
		 * </pre>
		 * @param value The value to handle, with range (-infinity, infinity)
		 * @return Rounded value
		 */
		static constexpr inline int32_t round32(const T value);

		/**
		 * Returns the rounded 64 bit integer value of a given value.
		 * The function applies a symmetric rounding:
		 * <pre>
		 * Input:     -1.50   -1.49  -0.50   -0.49   |   0.49   0.50    1.49   1.50
		 * Output:    -2      -1     -1       0      |   0      1       1      2
		 *
		 * So that for positive integer x:
		 * x  <-  [x - 0.5, x + 0.5)
		 *
		 * and for negative integer x:
		 * x  <-  (x - 0.5, x + 0.5]
		 * </pre>
		 * @param value The value to handle, with range (-infinity, infinity)
		 * @return Rounded value
		 */
		static constexpr inline long long round64(const T value);

		/**
		 * Returns the ratio between two values if the denominator is not equal a small epsilon.
		 * @param nominator The nominator of the resulting ratio
		 * @param denominator The denominator of the resulting ratio
		 * @param fallback The fallback value if the denominator is equal eps()
		 * @return The ratio between both values, the 'fallback' if the denominator is equal eps()
		 */
		static constexpr inline T ratio(const T nominator, const T denominator, const T fallback = T(1));

		/**
		 * Returns whether a value is smaller than or equal to a small epsilon.
		 * @param value The value to check, with range (-infinity, infinity)
		 * @return True, if so
		 */
		static constexpr inline bool isEqualEps(const T value);

		/**
		 * Returns whether a complex value is smaller than or equal to a small epsilon.
		 * @param value Complex value to check
		 * @return True, if so
		 */
		static constexpr inline bool isEqualEps(const std::complex<T>& value);

		/**
		 * Returns whether a value is smaller than or equal to a weak epsilon.
		 * @param value The value to check, with range (-infinity, infinity)
		 * @return True, if so
		 */
		static constexpr inline bool isWeakEqualEps(const T value);

		/**
		 * Returns whether a complex value is smaller than or equal to a weak epsilon.
		 * @param value Complex value to check
		 * @return True, if so
		 */
		static constexpr inline bool isWeakEqualEps(const std::complex<T>& value);

		/**
		 * Returns whether a value is not smaller than or equal to a small epsilon.
		 * @param value The value to check, with range (-infinity, infinity)
		 * @return True, if so
		 */
		static constexpr inline bool isNotEqualEps(const T value);

		/**
		 * Returns whether a complex value is not smaller than or equal to a small epsilon.
		 * @param value Complex value to check
		 * @return True, if so
		 */
		static constexpr inline bool isNotEqualEps(const std::complex<T>& value);

		/**
		 * Returns whether a value is not smaller than or equal to a weak epsilon.
		 * @param value The value to check, with range (-infinity, infinity)
		 * @return True, if so
		 */
		static constexpr inline bool isNotWeakEqualEps(const T value);

		/**
		 * Returns whether a complex value is not smaller than or equal to a weak epsilon.
		 * @param value Complex value to check
		 * @return True, if so
		 */
		static constexpr inline bool isNotWeakEqualEps(const std::complex<T>& value);

		/**
		 * Returns whether two values are equal up to a small epsilon.
		 * @param first The first value to check, with range (-infinity, infinity)
		 * @param second The second value to check, with range (-infinity, infinity)
		 * @return True, if so
		 */
		static inline bool isEqual(const T first, const T second);

		/**
		 * Returns whether two complex values are equal up to a small epsilon.
		 * @param first The first value to check
		 * @param second The second value to check
		 * @return True, if so
		 */
		static inline bool isEqual(const std::complex<T>& first, const std::complex<T>& second);

		/**
		 * Returns whether two values a equal up to a weak epsilon.
		 * @param first The first value to check, with range (-infinity, infinity)
		 * @param second The second value to check, with range (-infinity, infinity)
		 * @return True, if so
		 */
		static inline bool isWeakEqual(const T first, const T second);

		/**
		 * Returns whether two complex values are equal up to a weak epsilon.
		 * @param first The first value to check
		 * @param second The second value to check
		 * @return True, if so
		 */
		static inline bool isWeakEqual(const std::complex<T>& first, const std::complex<T>& second);

		/**
		 * Returns whether two values are equal up to a given epsilon.
		 * @param first The first value to check, with range (-infinity, infinity)
		 * @param second The second value to check, with range (-infinity, infinity)
		 * @param epsilon Accuracy epsilon, with range [0, infinity)
		 * @return True, if so
		 */
		static inline bool isEqual(const T first, const T second, const T epsilon);

		/**
		 * Returns whether two complex values are equal up to a given epsilon.
		 * @param first The first value to check
		 * @param second The second value to check
		 * @param epsilon Accuracy epsilon, with range [0, infinity)
		 * @return True, if so
		 */
		static inline bool isEqual(const std::complex<T>& first, const std::complex<T>& second, const T epsilon);

		/**
		 * Returns whether two values are equal up to a dynamic epsilon which is adjusted due to the magnitude of both values and the reference magnitude.
		 * The reference magnitude (provided as template parameter) corresponds to the provided reference epsilon.<br>
		 * That means that if the magnitude of both values to compare is equal to the defined reference magnitude the given epsilon value is used for comparison.<br>
		 * Otherwise the given epsilon is increased by 10 times the difference between the magnitude of the values and the reference magnitude, the epsilon value will not be decreased if the actual magnitude is smaller than the reference magnitude.
		 * @param first The first value to check, with range (-infinity, infinity)
		 * @param second The second value to check, with range (-infinity, infinity)
		 * @param referenceEpsilon The accuracy epsilon for the specified magnitude, with range [0, infinity)
		 * @return True, if so
		 * @tparam tMagnitude The reference magnitude for which the specified epsilon must hold so that both values count as equal, with range [-20, 20]
		 */
		template <int tMagnitude>
		static inline bool isEqual(const T first, const T second, const T referenceEpsilon);

		/**
		 * Returns whether two values are not equal up to a small epsilon.
		 * @param first The first value to check, with range (-infinity, infinity)
		 * @param second The second value to check, with range (-infinity, infinity)
		 * @return True, if so
		 */
		static inline bool isNotEqual(const T first, const T second);

		/**
		 * Returns whether two complex values are not equal up to a small epsilon.
		 * @param first The first value to check
		 * @param second The second value to check
		 * @return True, if so
		 */
		static inline bool isNotEqual(const std::complex<T>& first, const std::complex<T>& second);

		/**
		 * Returns whether two values are not equal up to a weak epsilon.
		 * @param first The first value to check, with range (-infinity, infinity)
		 * @param second The second value to check, with range (-infinity, infinity)
		 * @return True, if so
		 */
		static inline bool isNotWeakEqual(const T first, const T second);

		/**
		 * Returns whether two complex values are not equal up to a weak epsilon.
		 * @param first The first value to check
		 * @param second The second value to check
		 * @return True, if so
		 */
		static inline bool isNotWeakEqual(const std::complex<T>& first, const std::complex<T>& second);

		/**
		 * Returns whether two values are not equal up to a given epsilon.
		 * @param first The first value to check, with range (-infinity, infinity)
		 * @param second The second value to check, with range (-infinity, infinity)
		 * @param epsilon Accuracy epsilon, with range [0, infinity)
		 * @return True, if so
		 */
		static inline bool isNotEqual(const T first, const T second, const T epsilon);

		/**
		 * Returns whether two complex values are not equal up to a given epsilon.
		 * @param first The first value to check
		 * @param second The second value to check
		 * @param epsilon Accuracy epsilon, with range [0, infinity)
		 * @return True, if so
		 */
		static inline bool isNotEqual(const std::complex<T>& first, const std::complex<T>& second, const T epsilon);

		/**
		 * Returns whether a value lies between a given range up to a provided epsilon border.
		 * @param lower The lower border, with range (-infinity, infinity)
		 * @param value The value to check, with range (-infinity, infinity)
		 * @param upper The upper border, with range [lower, infinity)
		 * @param epsilon Accuracy epsilon, with range [0, infinity)
		 * @return True, if so
		 */
		static constexpr bool isInsideRange(const T lower, const T value, const T upper, const T epsilon = NumericT<T>::eps());

		/**
		 * Returns whether a value lies between a given range up to a weak epsilon border.
		 * @param lower The lower border, with range (-infinity, infinity)
		 * @param value The value to check, with range (-infinity, infinity)
		 * @param upper The upper border, with range [lower, infinity)
		 * @return True, if so
		 */
		static constexpr bool isInsideWeakRange(const T lower, const T value, const T upper);

		/**
		 * Returns whether a parameter lies on or below a given border tolerating a small epsilon.
		 * @param value The value to check, with range (-infinity, infinity)
		 * @param upper The upper border, with range (-infinity, infinity)
		 * @param epsilon Accuracy epsilon, with range [0, infinity)
		 * @return True, if 'value <= upper + epsilon'
		 */
		static constexpr bool isBelow(const T value, const T upper, const T epsilon = NumericT<T>::eps());

		/**
		 * Returns whether a parameter lies on or above a given border tolerating a small epsilon.
		 * @param value The value to check, with range (-infinity, infinity)
		 * @param lower The lower border, with range (-infinity, infinity)
		 * @param epsilon Accuracy epsilon, with range [0, infinity)
		 * @return True, if 'lower - epsilon <= value'
		 */
		static constexpr bool isAbove(const T value, const T lower, const T epsilon = NumericT<T>::eps());

		/**
		 * Returns a value which is not a number (nan).
		 * @return The nan value
		 */
		static constexpr T nan();

		/**
		 * Returns whether a given value is not a number.
		 * @param value The value to check
		 * @return True, if so
		 */
		static inline bool isNan(const T value);

		/**
		 * Returns whether a given value is not a number.
		 * @param value The value to check
		 * @return True, if so
		 */
		static inline bool isNan(const std::complex<T>& value);

		/**
		 * Returns a value which is positive infinity.
		 * @return Positive infinity
		 */
		static constexpr T inf();

		/**
		 * Returns whether a given value is positive or negative infinity.
		 * Only floating point values can be infinity.
		 * @param value The value to check
		 * @return True, if so
		 */
		static inline bool isInf(const T value);

		/**
		 * Returns whether a given value is positive or negative infinity.
		 * Only floating point values can be infinity.
		 * @param value The value to check
		 * @return True, if so
		 */
		static inline bool isInf(const std::complex<T>& value);

		/**
		 * Returns whether a given value of an arbitrary data type fits into the value range of 'T'.
		 * Below the behavior for individual data types:
		 * <pre>
		 * T           TValue           return
		 * A           A                true
		 * float       float            true
		 * double      double           true
		 * uint8_t     int8_t           >= 0
		 * int8_t      uint8_t          <= 127
		 * uint8_t     uint16_t         true
		 * uint8_t     uint32_t         true
		 * ...
		 * float       Integral         false
		 * double      Integral         false
		 * Integral    float            false
		 * Integral    double           false
		 * </pre>
		 * @param value The value to check, with range (-infinity, infinity)
		 * @return True, if value fits into 'T', or 'T' is same as 'TValue'
		 * @tparam TValue The data type of the given value
		 */
		template <typename TValue>
		static inline bool isInsideValueRange(const TValue& value);

		/**
		 * Converts deg to rad.
		 * @param deg Angle in deg, with range (-infinity, infinity)
		 * @return Angle in radian
		 */
		static constexpr inline T deg2rad(const T deg);

		/**
		 * Converts rad to deg.
		 * @param rad Angle in radian, with range (-infinity, infinity)
		 * @return Angle in deg
		 */
		static constexpr inline T rad2deg(const T rad);

		/**
		 * Returns the max scalar value.
		 * @return Max value
		 */
		static constexpr inline T maxValue();

		/**
		 * Returns the min scalar value.
		 * @return Min value
		 */
		static constexpr inline T minValue();

		/**
		 * Returns the sign of a given value by either returning -1, 0, or +1.
		 * Negative values have return value -1, 0 has return value 0, and positive values have return value +1.
		 * @param value The value for which the sign will be returned
		 */
		static constexpr inline T sign(const T& value);

		/**
		 * Copies the sign of a given value to another one.
		 * @param signReceiver First value receiving the sign from the second value
		 * @param signProvider Second value providing the sign for the first one
		 * @return First value with the sign of the second one
		 */
		static constexpr inline T copySign(const T signReceiver, const T signProvider);

		/**
		 * Copies the inverted sign of a given value to another one.
		 * @param signReceiver First value receiving the inverted sign from the second value
		 * @param signProvider Second value providing the inverted sign for the first one
		 * @return First value with the inverted sign of the second one
		 */
		static constexpr inline T invertSign(const T signReceiver, const T signProvider);

		/**
		 * Returns the length of the hypotenuse of a given right-angled triangle.
		 * This function avoids the calculation of square values of a and b.
		 * @param a Length of the first leg
		 * @param b Length of the second leg
		 * @return (a^2 + b^2)^0.5
		 */
		static T pythagoras(const T a, const T b);

		/**
		 * Returns a value of the univariate Gaussian distribution centered around the origin.
		 * @param x Value to return the Gaussian distribution value for
		 * @param sigma The sigma parameter defining the standard deviation of the Gaussian distribution, with range (0, infinity)
		 * @return Function value
		 */
		static inline T gaussianDistribution(const T x, const T sigma);

		/**
		 * Returns a value of the univariate Gaussian distribution centered around a given position x0.
		 * @param x Value to return the Gaussian distribution value for
		 * @param x0 X value for the center of the Gaussian distribution
		 * @param sigma The sigma parameter defining the standard deviation of the Gaussian distribution, with range (0, infinity)
		 * @return Function value
		 */
		static inline T gaussianDistribution(const T x, const T x0, const T sigma);

		/**
		 * Returns a value of the normalized univariate Gaussian distribution centered around the origin.
		 * As the result is normalized, the resulting Gaussian distribution value is 1 at the origin.<br>
		 * @param x Value to return the Gaussian distribution value for
		 * @param sigma The sigma parameter defining the standard deviation of the Gaussian distribution, with range (0, infinity)
		 * @return Function value
		 */
		static inline T normalizedGaussianDistribution(const T x, const T sigma);

		/**
		 * Returns a value of the normalized univariate Gaussian distribution around a given position x0.
		 * As the result is normalized, the resulting Gaussian distribution value is 1 at the given position x0.<br>
		 * @param x Value to return the Gaussian distribution value for
		 * @param x0 X value for the center of the Gaussian distribution
		 * @param sigma The sigma parameter defining the standard deviation of the Gaussian distribution, with range (0, infinity)
		 * @return Function value
		 */
		static inline T normalizedGaussianDistribution(const T x, const T x0, const T sigma);

		/**
		 * Returns a value of the bivariate Gaussian distribution centered around the origin.
		 * @param x X value for the function
		 * @param y Y value for the function
		 * @param sigmaX Sigma parameter defining the standard deviation for the x-axis of the Gaussian distribution, with range (0, infinity)
		 * @param sigmaY Sigma parameter defining the standard deviation for the y-axis of the Gaussian distribution, with range (0, infinity)
		 * @return Function value
		 */
		static inline T gaussianDistribution2(const T x, const T y, const T sigmaX, const T sigmaY);

		/**
		 * Returns a value of the bivariate Gaussian distribution centered around a given position (x0, y0).
		 * @param x X value for the function
		 * @param y Y value for the function
		 * @param x0 X value for the center of the Gaussian distribution
		 * @param y0 Y value for the center of the Gaussian distribution
		 * @param sigmaX Sigma parameter defining the standard deviation for the x-axis of the Gaussian distribution, with range (0, infinity)
		 * @param sigmaY Sigma parameter defining the standard deviation for the y-axis of the Gaussian distribution, with range (0, infinity)
		 * @return Function value
		 */
		static inline T gaussianDistribution2(const T x, const T y, const T x0, const T y0, const T sigmaX, const T sigmaY);

		/**
		 * Returns a value of the normalized bivariate Gaussian distribution centered around the origin.
		 * As the result is normalized, the resulting Gaussian distribution value is 1 at the origin.<br>
		 * @param x X value for the function
		 * @param y Y value for the function
		 * @param sigmaX Sigma parameter defining the standard deviation for the x-axis of the Gaussian distribution, with range (0, infinity)
		 * @param sigmaY Sigma parameter defining the standard deviation for the y-axis of the Gaussian distribution, with range (0, infinity)
		 * @return Function value
		 */
		static inline T normalizedGaussianDistribution2(const T x, const T y, const T sigmaX, const T sigmaY);

		/**
		 * Returns a value of the normalized bivariate Gaussian distribution centered around a given position (x0, y0).
		 * As the result is normalized, the resulting Gaussian distribution value is 1 at the given position (x0, y0).<br>
		 * @param x X value for the function
		 * @param y Y value for the function
		 * @param x0 X value for the center of the Gaussian distribution
		 * @param y0 Y value for the center of the Gaussian distribution
		 * @param sigmaX Sigma parameter defining the standard deviation for the x-axis of the Gaussian distribution, with range (0, infinity)
		 * @param sigmaY Sigma parameter defining the standard deviation for the y-axis of the Gaussian distribution, with range (0, infinity)
		 * @return Function value
		 */
		static inline T normalizedGaussianDistribution2(const T x, const T y, const T x0, const T y0, const T sigmaX, const T sigmaY);

		/**
		 * Returns a value of the trivariate Gaussian distribution centered around the origin.
		 * @param x X value for the function
		 * @param y Y value for the function
		 * @param z Z value for the function
		 * @param sigmaX Sigma parameter defining the standard deviation for the x-axis of the Gaussian distribution, with range (0, infinity)
		 * @param sigmaY Sigma parameter defining the standard deviation for the y-axis of the Gaussian distribution, with range (0, infinity)
		 * @param sigmaZ Sigma parameter defining the standard deviation for the z-axis of the Gaussian distribution, with range (0, infinity)
		 * @return Function value
		 */
		static inline T gaussianDistribution3(const T x, const T y, const T z, const T sigmaX, const T sigmaY, const T sigmaZ);

		/**
		 * Returns a value of the trivariate Gaussian distribution centered around a given position (x0, y0, z0).
		 * @param x X value for the function
		 * @param y Y value for the function
		 * @param z Z value for the function
		 * @param x0 X value for the center of the Gaussian distribution
		 * @param y0 Y value for the center of the Gaussian distribution
		 * @param z0 Z value for the center of the Gaussian distribution
		 * @param sigmaX Sigma parameter defining the standard deviation for the x-axis of the Gaussian distribution, with range (0, infinity)
		 * @param sigmaY Sigma parameter defining the standard deviation for the y-axis of the Gaussian distribution, with range (0, infinity)
		 * @param sigmaZ Sigma parameter defining the standard deviation for the z-axis of the Gaussian distribution, with range (0, infinity)
		 * @return Function value
		 */
		static inline T gaussianDistribution3(const T x, const T y, const T z, const T x0, const T y0, const T z0, const T sigmaX, const T sigmaY, const T sigmaZ);

		/**
		 * Returns a value of the normalized trivariate Gaussian distribution centered around the origin.
		 * As the result is normalized, the resulting Gaussian distribution value is 1 at the origin.<br>
		 * @param x X value for the function
		 * @param y Y value for the function
		 * @param z Z value for the function
		 * @param sigmaX Sigma parameter defining the standard deviation for the x-axis of the Gaussian distribution, with range (0, infinity)
		 * @param sigmaY Sigma parameter defining the standard deviation for the y-axis of the Gaussian distribution, with range (0, infinity)
		 * @param sigmaZ Sigma parameter defining the standard deviation for the z-axis of the Gaussian distribution, with range (0, infinity)
		 * @return Function value
		 */
		static inline T normalizedGaussianDistribution3(const T x, const T y, const T z, const T sigmaX, const T sigmaY, const T sigmaZ);

		/**
		 * Returns a value of the normalized trivariate Gaussian distribution centered around a given position (x0, y0, z0).
		 * As the result is normalized, the resulting Gaussian distribution value is 1 at the given position (x0, y0, z0).<br>
		 * @param x X value for the function
		 * @param y Y value for the function
		 * @param z Z value for the function
		 * @param x0 X value for the center of the Gaussian distribution
		 * @param y0 Y value for the center of the Gaussian distribution
		 * @param z0 Z value for the center of the Gaussian distribution
		 * @param sigmaX Sigma parameter defining the standard deviation for the x-axis of the Gaussian distribution, with range (0, infinity)
		 * @param sigmaY Sigma parameter defining the standard deviation for the y-axis of the Gaussian distribution, with range (0, infinity)
		 * @param sigmaZ Sigma parameter defining the standard deviation for the z-axis of the Gaussian distribution, with range (0, infinity)
		 * @return Function value
		 */
		static inline T normalizedGaussianDistribution3(const T x, const T y, const T z, const T x0, const T y0, const T z0, const T sigmaX, const T sigmaY, const T sigmaZ);
};

template <typename T>
constexpr T NumericT<T>::pi()
{
	return T(3.1415926535897932384626433832795);
}

template <typename T>
constexpr T NumericT<T>::pi2()
{
	return T(6.283185307179586476925286766559);
}

template <typename T>
constexpr T NumericT<T>::pi_2()
{
	return T(1.5707963267948966192313216916395);
}

template <typename T>
constexpr T NumericT<T>::pi_3()
{
	return T(1.0471975511965977461542144610932);
}

template <typename T>
constexpr T NumericT<T>::pi_4()
{
	return T(0.78539816339744830961566084581988);
}

template <typename T>
constexpr T NumericT<T>::squarePi()
{
	return T(9.8696044010893586188344909998762);
}

template <typename T>
constexpr T NumericT<T>::squarePi2()
{
	return T(39.478417604357434475337963999505);
}

template <typename T>
constexpr T NumericT<T>::squarePi_2()
{
	return T(2.4674011002723396547086227499667);
}

template <typename T>
constexpr T NumericT<T>::e()
{
	return T(2.71828182845904523536);
}

/**
 * Specialization of NumericT::eps().
 * @see NumericT::eps().
 */
template <>
constexpr double NumericT<double>::eps()
{
	return 1e-12;
}

/**
 * Specialization of NumericT::eps().
 * @see NumericT::eps().
 */
template <>
constexpr float NumericT<float>::eps()
{
	return 1e-6f;
}

/**
 * Specialization of NumericT::eps().
 * @see NumericT::eps().
 */
template <>
constexpr signed char NumericT<signed char>::eps()
{
	return 0;
}

/**
 * Specialization of NumericT::eps().
 * @see NumericT::eps().
 */
template <>
constexpr unsigned char NumericT<unsigned char>::eps()
{
	return 0u;
}

/**
 * Specialization of NumericT::eps().
 * @see NumericT::eps().
 */
template <>
constexpr short NumericT<short>::eps()
{
	return 0;
}

/**
 * Specialization of NumericT::eps().
 * @see NumericT::eps().
 */
template <>
constexpr unsigned short NumericT<unsigned short>::eps()
{
	return 0u;
}

/**
 * Specialization of NumericT::eps().
 * @see NumericT::eps().
 */
template <>
constexpr int NumericT<int>::eps()
{
	return 0;
}

/**
 * Specialization of NumericT::eps().
 * @see NumericT::eps().
 */
template <>
constexpr unsigned int NumericT<unsigned int>::eps()
{
	return 0u;
}

/**
 * Specialization of NumericT::eps().
 * @see NumericT::eps().
 */
template <>
constexpr long NumericT<long>::eps()
{
	return 0l;
}

/**
 * Specialization of NumericT::eps().
 * @see NumericT::eps().
 */
template <>
constexpr unsigned long NumericT<unsigned long>::eps()
{
	return 0ul;
}

/**
 * Specialization of NumericT::eps().
 * @see NumericT::eps().
 */
template <>
constexpr long long NumericT<long long>::eps()
{
	return 0ll;
}

/**
 * Specialization of NumericT::eps().
 * @see NumericT::eps().
 */
template <>
constexpr unsigned long long NumericT<unsigned long long>::eps()
{
	return 0ull;
}

/**
 * Specialization of NumericT::weakEps().
 * @see NumericT::weakEps().
 */
template <>
constexpr double NumericT<double>::weakEps()
{
	return 1e-6;
}

/**
 * Specialization of NumericT::weakEps().
 * @see NumericT::weakEps().
 */
template <>
constexpr float NumericT<float>::weakEps()
{
	return 1e-3f;
}

/**
 * Specialization of NumericT::weakEps().
 * @see NumericT::weakEps().
 */
template <>
constexpr signed char NumericT<signed char>::weakEps()
{
	return 0;
}

/**
 * Specialization of NumericT::weakEps().
 * @see NumericT::weakEps().
 */
template <>
constexpr unsigned char NumericT<unsigned char>::weakEps()
{
	return 0u;
}

/**
 * Specialization of NumericT::weakEps().
 * @see NumericT::weakEps().
 */
template <>
constexpr short NumericT<short>::weakEps()
{
	return 0;
}

/**
 * Specialization of NumericT::weakEps().
 * @see NumericT::weakEps().
 */
template <>
constexpr unsigned short NumericT<unsigned short>::weakEps()
{
	return 0u;
}

/**
 * Specialization of NumericT::weakEps().
 * @see NumericT::weakEps().
 */
template <>
constexpr int NumericT<int>::weakEps()
{
	return 0;
}

/**
 * Specialization of NumericT::weakEps().
 * @see NumericT::weakEps().
 */
template <>
constexpr unsigned int NumericT<unsigned int>::weakEps()
{
	return 0u;
}

/**
 * Specialization of NumericT::weakEps().
 * @see NumericT::weakEps().
 */
template <>
constexpr long NumericT<long>::weakEps()
{
	return 0l;
}

/**
 * Specialization of NumericT::weakEps().
 * @see NumericT::weakEps().
 */
template <>
constexpr unsigned long NumericT<unsigned long>::weakEps()
{
	return 0ul;
}

/**
 * Specialization of NumericT::weakEps().
 * @see NumericT::weakEps().
 */
template <>
constexpr long long NumericT<long long>::weakEps()
{
	return 0ll;
}

/**
 * Specialization of NumericT::weakEps().
 * @see NumericT::weakEps().
 */
template <>
constexpr unsigned long long NumericT<unsigned long long>::weakEps()
{
	return 0ull;
}

template <typename T>
inline T NumericT<T>::abs(const T value)
{
	return ::abs(value);
}

/**
 * Specialization of NumericT::abs().
 * @see NumericT::abs().
 */
template <>
inline signed char NumericT<signed char>::abs(const signed char value)
{
	ocean_assert(value != std::numeric_limits<signed char>::lowest() && "The absolute value of -128 is 128 but then casted to -128");

	return (signed char)(::abs(value));
}

/**
 * Specialization of NumericT::abs().
 * @see NumericT::abs().
 */
template <>
inline unsigned char NumericT<unsigned char>::abs(const unsigned char value)
{
	return value;
}

/**
 * Specialization of NumericT::abs().
 * @see NumericT::abs().
 */
template <>
inline short NumericT<short>::abs(const short value)
{
	ocean_assert(value != std::numeric_limits<short>::lowest() && "The absolute value of -32768 is 32768 but then casted to -32768");

	return short(::abs(value));
}

/**
 * Specialization of NumericT::abs().
 * @see NumericT::abs().
 */
template <>
inline unsigned short NumericT<unsigned short>::abs(const unsigned short value)
{
	return value;
}

/**
 * Specialization of NumericT::abs().
 * @see NumericT::abs().
 */
template <>
inline int NumericT<int>::abs(const int value)
{
	ocean_assert(value != std::numeric_limits<int>::lowest());

	return ::abs(value);
}

/**
 * Specialization of NumericT::abs().
 * @see NumericT::abs().
 */
template <>
inline unsigned int NumericT<unsigned int>::abs(const unsigned int value)
{
	return value;
}

/**
 * Specialization of NumericT::abs().
 * @see NumericT::abs().
 */
template <>
inline long NumericT<long>::abs(const long value)
{
	ocean_assert(value != std::numeric_limits<long>::lowest());

	return ::labs(value);
}

/**
 * Specialization of NumericT::abs().
 * @see NumericT::abs().
 */
template <>
inline unsigned long NumericT<unsigned long>::abs(const unsigned long value)
{
	return value;
}

/**
 * Specialization of NumericT::abs().
 * @see NumericT::abs().
 */
template <>
inline long long NumericT<long long>::abs(const long long value)
{
	ocean_assert(value != std::numeric_limits<long long>::lowest());

	return ::llabs(value);
}

/**
 * Specialization of NumericT::abs().
 * @see NumericT::abs().
 */
template <>
inline unsigned long long NumericT<unsigned long long>::abs(const unsigned long long value)
{
	return value;
}

/**
 * Specialization of NumericT::abs().
 * @see NumericT::abs().
 */
template <>
inline double NumericT<double>::abs(const double value)
{
	return fabs(value);
}

/**
 * Specialization of NumericT::abs().
 * @see NumericT::abs().
 */
template <>
inline float NumericT<float>::abs(const float value)
{
	return fabsf(value);
}

template <typename T>
inline typename UnsignedTyper<T>::Type NumericT<T>::secureAbs(const T value)
{
	return typename UnsignedTyper<T>::Type(::abs(value));
}

/**
 * Specialization of NumericT::secureAbs().
 * @see NumericT::secureAbs().
 */
template <>
inline unsigned char NumericT<signed char>::secureAbs(const signed char value)
{
	return (unsigned char)(::abs(int(value)));
}

/**
 * Specialization of NumericT::secureAbs().
 * @see NumericT::secureAbs().
 */
template <>
inline unsigned char NumericT<unsigned char>::secureAbs(const unsigned char value)
{
	return value;
}

/**
 * Specialization of NumericT::secureAbs().
 * @see NumericT::secureAbs().
 */
template <>
inline unsigned short NumericT<short>::secureAbs(const short value)
{
	return (unsigned short)::abs(int(value));
}

/**
 * Specialization of NumericT::secureAbs().
 * @see NumericT::secureAbs().
 */
template <>
inline unsigned short NumericT<unsigned short>::secureAbs(const unsigned short value)
{
	return value;
}

/**
 * Specialization of NumericT::secureAbs().
 * @see NumericT::secureAbs().
 */
template <>
inline unsigned int NumericT<int>::secureAbs(const int value)
{
	if (value == std::numeric_limits<int>::lowest())
	{
		// -2147483648 -> +2147483648
		return (unsigned int)(std::numeric_limits<int>::max()) + 1u;
	}

	return (unsigned int)(::abs(value));
}

/**
 * Specialization of NumericT::secureAbs().
 * @see NumericT::secureAbs().
 */
template <>
inline unsigned int NumericT<unsigned int>::secureAbs(const unsigned int value)
{
	return value;
}

/**
 * Specialization of NumericT::secureAbs().
 * @see NumericT::secureAbs().
 */
template <>
inline unsigned long NumericT<long>::secureAbs(const long value)
{
	ocean_assert((unsigned long)::llabs(std::numeric_limits<long>::max()) == (unsigned long)(-(std::numeric_limits<long>::lowest() + 1l)));
	return (unsigned long)::llabs(value);
}

/**
 * Specialization of NumericT::secureAbs().
 * @see NumericT::secureAbs().
 */
template <>
inline unsigned long NumericT<unsigned long>::secureAbs(const unsigned long value)
{
	return value;
}

/**
 * Specialization of NumericT::secureAbs().
 * @see NumericT::secureAbs().
 */
template <>
inline unsigned long long NumericT<long long>::secureAbs(const long long value)
{
	if (value == std::numeric_limits<long long>::lowest())
	{
		// -9223372036854775808 -> +9223372036854775808
		return (unsigned long long)(std::numeric_limits<long long>::max()) + 1ull;
	}

	return (unsigned long long)(::llabs(value));
}

/**
 * Specialization of NumericT::secureAbs().
 * @see NumericT::secureAbs().
 */
template <>
inline unsigned long long NumericT<unsigned long long>::secureAbs(const unsigned long long value)
{
	return value;
}

/**
 * Specialization of NumericT::secureAbs().
 * @see NumericT::secureAbs().
 */
template <>
inline double NumericT<double>::secureAbs(const double value)
{
	return abs(value);
}

/**
 * Specialization of NumericT::secureAbs().
 * @see NumericT::secureAbs().
 */
template <>
inline float NumericT<float>::secureAbs(const float value)
{
	return abs(value);
}

template <typename T>
constexpr inline T NumericT<T>::sqr(const T value)
{
	return value * value;
}

template <typename T>
inline T NumericT<T>::sum(const T* values, const size_t number)
{
	T result = T(0);

	for (size_t n = 0; n < number; ++n)
	{
		result += values[n];
	}

	return result;
}

template <typename T>
inline T NumericT<T>::summedSqr(const T* values, const size_t number)
{
	T result = T(0);

	for (size_t n = 0; n < number; ++n)
	{
		result += sqr(values[n]);
	}

	return result;
}

template <typename T>
constexpr inline T NumericT<T>::sqrDistance(const T value0, const T value1)
{
	return sqr(value0 - value1);
}

template <typename T>
inline T NumericT<T>::sqrt(const T value)
{
	ocean_assert(value >= T(0));
	return T(::sqrt(value));
}

/**
 * Specialization of NumericT::sqrt().
 * @see NumericT::sqrt().
 */
template <>
inline float NumericT<float>::sqrt(const float value)
{
	ocean_assert(value >= 0.0f);
	return ::sqrtf(value);
}

/**
 * Specialization of NumericT::sqrt().
 * @see NumericT::sqrt().
 */
template <>
inline int NumericT<int>::sqrt(const int value)
{
	ocean_assert(value >= 0);
	return int(::sqrtf(float(value)));
}

template <typename T>
inline std::complex<T> NumericT<T>::sqrt(const std::complex<T>& value)
{
	return std::sqrt<T>(value);
}

template <typename T>
inline T NumericT<T>::sin(const T value)
{
	return ::sin(value);
}

/**
 * Specialization of NumericT::sin().
 * @see NumericT::sin().
 */
template <>
inline float NumericT<float>::sin(const float value)
{
	return sinf(value);
}

template <typename T>
inline T NumericT<T>::cos(const T value)
{
	return ::cos(value);
}

/**
 * Specialization of NumericT::cos().
 * @see NumericT::cos().
 */
template <>
inline float NumericT<float>::cos(const float value)
{
	return cosf(value);
}

template <typename T>
inline T NumericT<T>::tan(const T value)
{
	return ::tan(value);
}

/**
 * Specialization of NumericT::tan().
 * @see NumericT::tan().
 */
template <>
inline float NumericT<float>::tan(const float value)
{
	return tanf(value);
}

template <typename T>
inline T NumericT<T>::atan(const T value)
{
	return ::atan(value);
}

/**
 * Specialization of NumericT::atan().
 * @see NumericT::atan().
 */
template <>
inline float NumericT<float>::atan(const float value)
{
	return atanf(value);
}

template <typename T>
inline T NumericT<T>::atan2(const T y, const T x)
{
	ocean_assert(NumericT<T>::isNotEqualEps(y) || NumericT<T>::isNotEqualEps(x));

	const T angle = ::atan2(y, x);
	ocean_assert(angle >= -NumericT<T>::pi() && angle <= NumericT<T>::pi());

	return angle;
}

template <typename T>
inline T NumericT<T>::exp(const T value)
{
	return ::exp(value);
}

template <>
inline float NumericT<float>::exp(const float value)
{
	return ::expf(value);
}

template <typename T>
inline T NumericT<T>::log(const T value)
{
	ocean_assert(value > 0);
	return ::log(value);
}

template <>
inline float NumericT<float>::log(const float value)
{
	ocean_assert(value > 0);
	return ::logf(value);
}

template <typename T>
inline T NumericT<T>::log1p(const T value)
{
	ocean_assert(value > -1);
	return ::log1p(value);
}

template <>
inline float NumericT<float>::log1p(const float value)
{
	ocean_assert(value > -1);
	return ::log1pf(value);
}

template <typename T>
inline T NumericT<T>::log2(const T value)
{
	ocean_assert(value > 0);

#if (defined(OCEAN_COMPILER_MSC) && OCEAN_MSC_VERSION >= 1800) || (OCEAN_CLANG_VERSION >= 30300 && (!defined(__ANDROID__) || !__ANDROID__))
	return ::log2(value);
#else
	return ::log(value) * 1.44269504088896340736; // 1 / ln(2)
#endif
}

/**
 * Specialization of NumericT::log2().
 * @see NumericT::log2().
 */
template <>
inline float NumericT<float>::log2(const float value)
{
	ocean_assert(value > 0);

#if (defined(OCEAN_COMPILER_MSC) && OCEAN_MSC_VERSION >= 1800) || (OCEAN_CLANG_VERSION >= 30300 && (!defined(__ANDROID__) || !__ANDROID__))
	return ::log2f(value);
#else
	return ::logf(value) * 1.44269504088896340736f; // 1 / ln(2)
#endif
}

template <typename T>
inline T NumericT<T>::log10(const T value)
{
	ocean_assert(value > 0);
	return ::log10(value);
}

template <>
inline float NumericT<float>::log10(const float value)
{
	ocean_assert(value > 0);
	return ::log10f(value);
}

/**
 * Specialization of NumericT::atan2().
 * @see NumericT::atan2().
 */
template <>
inline float NumericT<float>::atan2(const float y, const float x)
{
	return atan2f(y, x);
}

template <typename T>
inline T NumericT<T>::fmod(const T valueA, const T valueB)
{
	return ::fmod(valueA, valueB);
}

/**
 * Specialization of NumericT::fmod().
 * @see NumericT::fmod().
 */
template <>
inline float NumericT<float>::fmod(const float valueA, const float valueB)
{
	return fmodf(valueA, valueB);
}

template <typename T>
inline T NumericT<T>::dot(const T* vectorA, const T* vectorB, const size_t size)
{
	T result = T(0);

	for (size_t n = 0; n < size; ++n)
	{
		result += vectorA[n] * vectorB[n];
	}

	return result;
}

template <typename T>
inline T NumericT<T>::angleAdjustPositive(const T angle)
{
	const T adjusted = fmod(angle, pi2());

	if (adjusted < 0)
	{
		ocean_assert((std::is_same<T, float>::value) || (pi2() + adjusted >= 0 && pi2() + adjusted < pi2()));
		return pi2() + adjusted;
	}

	ocean_assert(adjusted >= 0 && adjusted < pi2());
	return adjusted;
}

template <typename T>
inline T NumericT<T>::angleAdjustPositiveHalf(const T angle)
{
	const T adjusted = fmod(angle, pi());

	if (adjusted < 0)
	{
		ocean_assert(pi() + adjusted >= 0 && pi() + adjusted < pi());
		return pi() + adjusted;
	}

	ocean_assert(adjusted >= 0 && adjusted < pi());
	return adjusted;
}

template <typename T>
inline T NumericT<T>::angleAdjustNull(const T angle)
{
	if (angle > -pi() && angle <= pi())
	{
		return angle;
	}

	const T adjusted = fmod(angle, pi2());

	if (isAbove(adjusted, pi()))
	{
		ocean_assert(adjusted - pi2() > -pi() - eps() && adjusted - pi2() <= pi() + eps());
		return adjusted - pi2();
	}
	else if (isBelow(adjusted, -pi()))
	{
		ocean_assert(adjusted + pi2() > -pi() - eps() && adjusted + pi2() <= pi() + eps());
		return adjusted + pi2();
	}

	ocean_assert(adjusted > -pi() && adjusted <= pi());
	return adjusted;
}

template <typename T>
inline bool NumericT<T>::angleIsEqual(const T angleA, const T angleB)
{
	return isEqualEps(angleDistance(angleA, angleB));
}

template <typename T>
inline bool NumericT<T>::angleIsWeakEqual(const T angleA, const T angleB)
{
	return isWeakEqualEps(angleDistance(angleA, angleB));
}

template <typename T>
inline bool NumericT<T>::angleIsEqual(const T angleA, const T angleB, const T epsilon)
{
	ocean_assert(epsilon >= T(0));
	return isEqual(angleDistance(angleA, angleB), T(0), epsilon);
}

template <typename T>
inline bool NumericT<T>::angleIsBelowThreshold(const T angleA, const T angleB, const T threshold)
{
	ocean_assert(threshold < pi());

	return isBelow(abs(angleAdjustNull(angleA - angleB)), threshold);
}

template <typename T>
inline T NumericT<T>::angleDistance(const T angleA, const T angleB)
{
	const T distance = abs(angleAdjustPositive(angleA) - angleAdjustPositive(angleB));
	ocean_assert(distance < NumericT<T>::pi2());

	if (distance < NumericT<T>::pi())
	{
		return distance;
	}

	return NumericT<T>::pi2() - distance;
}

template <typename T>
inline T NumericT<T>::pow(const T x, const T y)
{
	return ::pow(x, y);
}

template <typename T>
inline std::complex<T> NumericT<T>::pow(const std::complex<T>& x, const T y)
{
	return std::pow<T>(x, y);
}

/**
 * Specialization of NumericT::pow().
 * @see NumericT::pow().
 */
template <>
inline float NumericT<float>::pow(const float x, const float y)
{
	return powf(x, y);
}

template <typename T>
constexpr T NumericT<T>::integerPow(const T x, const unsigned int y)
{
	return y == 0u ? T(1) : x * integerPow(x, y - 1u);
}

template <typename T>
constexpr inline T NumericT<T>::factorial(const T& value)
{
	ocean_assert(value >= 0);
	ocean_assert(T((long long)(value)) == value);

	if (value <= 1)
	{
		return T(1);
	}

	T result = T(2);

	for (unsigned int n = 3u; n <= (unsigned int)value; ++n)
	{
		ocean_assert(std::numeric_limits<T>::max() / T(n) >= result);
		result *= T(n);
	}

	return result;
}

template <>
constexpr inline unsigned int NumericT<unsigned int>::factorial(const unsigned int& value)
{
	switch (value)
	{
		case 0u:
		case 1u:
			return 1u;

		case 2u:
			return 2u;

		case 3u:
			return 6u;

		case 4u:
			return 24u;

		case 5u:
			return 120u;

		case 6u:
			return 720u;

		case 7u:
			return 5040u;

		case 8u:
			return 40320u;

		case 9u:
			return 362880u;

		case 10u:
			return 3628800u;

		case 11u:
			return 39916800u;

		case 12u:
			return 479001600u;
	}

	ocean_assert(false && "Invalid factorial value!");
	return 1u;
}

template <typename T>
constexpr inline T NumericT<T>::binomialCoefficient(const T& n, const T& k)
{
	ocean_assert(n >= 0 && k >= 0);
	ocean_assert(T((long long)(n)) == n);
	ocean_assert(T((long long)(k)) == k);

	if (n <= T(1) || k == T(0) || n == k)
	{
		return T(1);
	}

	ocean_assert(k < n);

	T result = n - k + T(1);

	for (unsigned int i = (unsigned int)(n - k) + 2u; i <= (unsigned int)n; ++i)
	{
		ocean_assert(std::numeric_limits<T>::max() / T(i) >= result);
		result *= T(i);
	}

	for (unsigned int i = 2u; i <= (unsigned int)k; ++i)
	{
		ocean_assert((result / T(i)) * T(i) == result);
		result /= T(i);
	}

	return result;
}

template <typename T>
inline T NumericT<T>::ceil(const T value)
{
	static_assert(std::is_floating_point<T>::value == false, "Data type must not be a floating point!");

	return value;
}

/**
 * Specialization of NumericT::ceil().
 * @see NumericT::ceil().
 */
template <>
inline float NumericT<float>::ceil(const float value)
{
	return ::ceilf(value);
}

/**
 * Specialization of NumericT::ceil().
 * @see NumericT::ceil().
 */
template <>
inline double NumericT<double>::ceil(const double value)
{
	return ::ceil(value);
}

/**
 * Specialization of NumericT::ceil().
 * @see NumericT::ceil().
 */
template <>
inline long double NumericT<long double>::ceil(const long double value)
{
	return ::ceill(value);
}

template <typename T>
inline T NumericT<T>::floor(const T value)
{
	static_assert(std::is_floating_point<T>::value == false, "Data type must not be a floating point!");

	return value;
}

/**
 * Specialization of NumericT::floor().
 * @see NumericT::floor().
 */
template <>
inline float NumericT<float>::floor(const float value)
{
	return ::floorf(value);
}

/**
 * Specialization of NumericT::floor().
 * @see NumericT::floor().
 */
template <>
inline double NumericT<double>::floor(const double value)
{
	return ::floor(value);
}

/**
 * Specialization of NumericT::floor().
 * @see NumericT::floor().
 */
template <>
inline long double NumericT<long double>::floor(const long double value)
{
	return ::floorl(value);
}

template <typename T>
constexpr inline int32_t NumericT<T>::round32(const T value)
{
	return int32_t(value + copySign(T(0.5), value));
}

template <typename T>
constexpr inline long long NumericT<T>::round64(const T value)
{
	return (long long)(value + copySign(T(0.5), value));
}

template <typename T>
constexpr inline T NumericT<T>::ratio(const T nominator, const T denominator, const T fallback)
{
	if (isEqualEps(denominator))
	{
		return fallback;
	}

	return nominator / denominator;
}

template <typename T>
constexpr inline bool NumericT<T>::isEqualEps(const T value)
{
	ocean_assert(isNan(value) == false);
	ocean_assert((fabs(value) <= eps()) == (-eps() <= value && value <= eps()));

	return -eps() <= value && value <= eps();
}

template <>
constexpr inline bool NumericT<signed char>::isEqualEps(const signed char value)
{
	return value == (signed char)(0);
}

template <>
constexpr inline bool NumericT<unsigned char>::isEqualEps(const unsigned char value)
{
	return value == (unsigned char)(0);
}

template <>
constexpr inline bool NumericT<short>::isEqualEps(const short value)
{
	return value == short(0);
}

template <>
constexpr inline bool NumericT<unsigned short>::isEqualEps(const unsigned short value)
{
	return value == (unsigned short)(0);
}

template <>
constexpr inline bool NumericT<int>::isEqualEps(const int value)
{
	return value == 0;
}

template <>
constexpr inline bool NumericT<unsigned int>::isEqualEps(const unsigned int value)
{
	return value == 0u;
}

template <>
constexpr inline bool NumericT<long>::isEqualEps(const long value)
{
	return value == 0l;
}

template <>
constexpr inline bool NumericT<unsigned long>::isEqualEps(const unsigned long value)
{
	return value == 0ul;
}

template <>
constexpr inline bool NumericT<long long>::isEqualEps(const long long value)
{
	return value == 0ll;
}

template <>
constexpr inline bool NumericT<unsigned long long>::isEqualEps(const unsigned long long value)
{
	return value == 0ull;
}

template <typename T>
constexpr inline bool NumericT<T>::isEqualEps(const std::complex<T>& value)
{
	return isEqualEps(value.real()) && isEqualEps(value.imag());
}

template <typename T>
constexpr inline bool NumericT<T>::isWeakEqualEps(const T value)
{
	ocean_assert(isNan(value) == false);
	ocean_assert((fabs(value) <= weakEps()) == (-weakEps() <= value && value <= weakEps()));

	return -weakEps() <= value && value <= weakEps();
}

template <>
constexpr inline bool NumericT<signed char>::isWeakEqualEps(const signed char value)
{
	return value == (signed char)(0);
}

template <>
constexpr inline bool NumericT<unsigned char>::isWeakEqualEps(const unsigned char value)
{
	return value == (unsigned char)(0);
}

template <>
constexpr inline bool NumericT<short>::isWeakEqualEps(const short value)
{
	return value == short(0);
}

template <>
constexpr inline bool NumericT<unsigned short>::isWeakEqualEps(const unsigned short value)
{
	return value == (unsigned short)(0);
}

template <>
constexpr inline bool NumericT<int>::isWeakEqualEps(const int value)
{
	return value == 0;
}

template <>
constexpr inline bool NumericT<unsigned int>::isWeakEqualEps(const unsigned int value)
{
	return value == 0u;
}

template <>
constexpr inline bool NumericT<long>::isWeakEqualEps(const long value)
{
	return value == 0l;
}

template <>
constexpr inline bool NumericT<unsigned long>::isWeakEqualEps(const unsigned long value)
{
	return value == 0ul;
}

template <>
constexpr inline bool NumericT<long long>::isWeakEqualEps(const long long value)
{
	return value == 0ll;
}

template <>
constexpr inline bool NumericT<unsigned long long>::isWeakEqualEps(const unsigned long long value)
{
	return value == 0ull;
}

template <typename T>
constexpr inline bool NumericT<T>::isWeakEqualEps(const std::complex<T>& value)
{
	return isWeakEqualEps(value.real()) && isWeakEqualEps(value.imag());
}

template <typename T>
constexpr inline bool NumericT<T>::isNotEqualEps(const T value)
{
	ocean_assert(isNan(value) == false);
	ocean_assert((fabs(value) > eps()) == (value > eps() || value < -eps()));

	return value > eps() || value < -eps();
}

template <>
constexpr inline bool NumericT<signed char>::isNotEqualEps(const signed char value)
{
	return value != (signed char)(0);
}

template <>
constexpr inline bool NumericT<unsigned char>::isNotEqualEps(const unsigned char value)
{
	return value != (unsigned char)(0);
}

template <>
constexpr inline bool NumericT<short>::isNotEqualEps(const short value)
{
	return value != short(0);
}

template <>
constexpr inline bool NumericT<unsigned short>::isNotEqualEps(const unsigned short value)
{
	return value != (unsigned short)(0);
}

template <>
constexpr inline bool NumericT<int>::isNotEqualEps(const int value)
{
	return value != 0;
}

template <>
constexpr inline bool NumericT<unsigned int>::isNotEqualEps(const unsigned int value)
{
	return value != 0u;
}

template <>
constexpr inline bool NumericT<long>::isNotEqualEps(const long value)
{
	return value != 0l;
}

template <>
constexpr inline bool NumericT<unsigned long>::isNotEqualEps(const unsigned long value)
{
	return value != 0ul;
}

template <>
constexpr inline bool NumericT<long long>::isNotEqualEps(const long long value)
{
	return value != 0ll;
}

template <>
constexpr inline bool NumericT<unsigned long long>::isNotEqualEps(const unsigned long long value)
{
	return value != 0ull;
}

template <typename T>
constexpr inline bool NumericT<T>::isNotEqualEps(const std::complex<T>& value)
{
	return value.real() > eps() || value.real() < -eps() || value.imag() > eps() || value.imag() < -eps();
}

template <typename T>
constexpr inline bool NumericT<T>::isNotWeakEqualEps(const T value)
{
	ocean_assert((fabs(value) > weakEps()) == (value > weakEps() || value < -weakEps()));

	return value > weakEps() || value < -weakEps();
}

template <>
constexpr inline bool NumericT<signed char>::isNotWeakEqualEps(const signed char value)
{
	return value != (signed char)(0);
}

template <>
constexpr inline bool NumericT<unsigned char>::isNotWeakEqualEps(const unsigned char value)
{
	return value != (unsigned char)(0);
}

template <>
constexpr inline bool NumericT<short>::isNotWeakEqualEps(const short value)
{
	return value != short(0);
}

template <>
constexpr inline bool NumericT<unsigned short>::isNotWeakEqualEps(const unsigned short value)
{
	return value != (unsigned short)(0);
}

template <>
constexpr inline bool NumericT<int>::isNotWeakEqualEps(const int value)
{
	return value != 0;
}

template <>
constexpr inline bool NumericT<unsigned int>::isNotWeakEqualEps(const unsigned int value)
{
	return value != 0u;
}

template <>
constexpr inline bool NumericT<long>::isNotWeakEqualEps(const long value)
{
	return value != 0l;
}

template <>
constexpr inline bool NumericT<unsigned long>::isNotWeakEqualEps(const unsigned long value)
{
	return value != 0ul;
}

template <>
constexpr inline bool NumericT<long long>::isNotWeakEqualEps(const long long value)
{
	return value != 0ll;
}

template <>
constexpr inline bool NumericT<unsigned long long>::isNotWeakEqualEps(const unsigned long long value)
{
	return value != 0ull;
}

template <typename T>
constexpr inline bool NumericT<T>::isNotWeakEqualEps(const std::complex<T>& value)
{
	return value.real() > weakEps() || value.real() < -weakEps() || value.imag() > weakEps() || value.imag() < -weakEps();
}

template <typename T>
inline bool NumericT<T>::isEqual(const T first, const T second)
{
	return fabs(first - second) <= eps();
}

template <>
inline bool NumericT<signed char>::isEqual(const signed char first, const signed char second)
{
	return first == second;
}

template <>
inline bool NumericT<unsigned char>::isEqual(const unsigned char first, const unsigned char second)
{
	return first == second;
}

template <>
inline bool NumericT<short>::isEqual(const short first, const short second)
{
	return first == second;
}

template <>
inline bool NumericT<unsigned short>::isEqual(const unsigned short first, const unsigned short second)
{
	return first == second;
}

template <>
inline bool NumericT<int>::isEqual(const int first, const int second)
{
	return first == second;
}

template <>
inline bool NumericT<unsigned int>::isEqual(const unsigned int first, const unsigned int second)
{
	return first == second;
}

template <>
inline bool NumericT<long>::isEqual(const long first, const long second)
{
	return first == second;
}

template <>
inline bool NumericT<unsigned long>::isEqual(const unsigned long first, const unsigned long second)
{
	return first == second;
}

template <>
inline bool NumericT<long long>::isEqual(const long long first, const long long second)
{
	return first == second;
}

template <>
inline bool NumericT<unsigned long long>::isEqual(const unsigned long long first, const unsigned long long second)
{
	return first == second;
}

template <typename T>
inline bool NumericT<T>::isEqual(const std::complex<T>& first, const std::complex<T>& second)
{
	return isEqual(first.real(), second.real()) && isEqual(first.imag(), second.imag());
}

template <typename T>
inline bool NumericT<T>::isEqual(const T first, const T second, const T epsilon)
{
	ocean_assert(epsilon >= T(0));
	return NumericT<T>::abs(first - second) <= epsilon;
}

template <>
inline bool NumericT<signed char>::isEqual(const signed char first, const signed char second, const signed char epsilon)
{
	ocean_assert(epsilon >= (signed char)(0));
	return std::abs(first - second) <= epsilon;
}

template <>
inline bool NumericT<short>::isEqual(const short first, const short second, const short epsilon)
{
	ocean_assert(epsilon >= short(0));
	return std::abs(first - second) <= epsilon;
}

template <>
inline bool NumericT<int>::isEqual(const int first, const int second, const int epsilon)
{
	ocean_assert(epsilon >= 0);
	return std::abs(first - second) <= epsilon;
}

template <>
inline bool NumericT<long>::isEqual(const long first, const long second, const long epsilon)
{
	ocean_assert(epsilon >= long(0));
	return std::abs(first - second) <= epsilon;
}

template <>
inline bool NumericT<long long>::isEqual(const long long first, const long long second, const long long epsilon)
{
	ocean_assert(epsilon >= 0ll);
	return std::abs(first - second) <= epsilon;
}

template <>
inline bool NumericT<unsigned char>::isEqual(const unsigned char first, const unsigned char second, const unsigned char epsilon)
{
	if (first > second)
	{
		return first - second <= epsilon;
	}
	else
	{
		return second - first <= epsilon;
	}
}

template <>
inline bool NumericT<unsigned short>::isEqual(const unsigned short first, const unsigned short second, const unsigned short epsilon)
{
	if (first > second)
	{
		return first - second <= epsilon;
	}
	else
	{
		return second - first <= epsilon;
	}
}

template <>
inline bool NumericT<unsigned int>::isEqual(const unsigned int first, const unsigned int second, const unsigned int epsilon)
{
	if (first > second)
	{
		return first - second <= epsilon;
	}
	else
	{
		return second - first <= epsilon;
	}
}

template <>
inline bool NumericT<unsigned long>::isEqual(const unsigned long first, const unsigned long second, const unsigned long epsilon)
{
	if (first > second)
	{
		return first - second <= epsilon;
	}
	else
	{
		return second - first <= epsilon;
	}
}

template <>
inline bool NumericT<unsigned long long>::isEqual(const unsigned long long first, const unsigned long long second, const unsigned long long epsilon)
{
	if (first > second)
	{
		return first - second <= epsilon;
	}
	else
	{
		return second - first <= epsilon;
	}
}

template <typename T>
inline bool NumericT<T>::isEqual(const std::complex<T>& first, const std::complex<T>& second, const T epsilon)
{
	ocean_assert(epsilon >= T(0));
	return isEqual(first.real(), second.real(), epsilon) && isEqual(first.imag(), second.imag(), epsilon);
}

template <typename T>
inline bool NumericT<T>::isWeakEqual(const T first, const T second)
{
	return NumericT<T>::abs(first - second) <= weakEps();
}

template <>
inline bool NumericT<int>::isWeakEqual(const int first, const int second)
{
	return first == second;
}

template <typename T>
inline bool NumericT<T>::isWeakEqual(const std::complex<T>& first, const std::complex<T>& second)
{
	return isWeakEqual(first.real(), second.real()) && isWeakEqual(first.imag(), second.imag());
}

template <typename T>
template <int tMagnitude>
inline bool NumericT<T>::isEqual(const T first, const T second, const T referenceEpsilon)
{
	static_assert(tMagnitude >= -20 && tMagnitude <= 20, "Invalid reference magnitude");
	ocean_assert(referenceEpsilon >= T(0));

	const T maximalAbsValue = max(NumericT<T>::abs(first), NumericT<T>::abs(second));

	// we ensure that the max value is not 0 (due to the following log)
	if (isEqualEps(maximalAbsValue))
	{
		return NumericT<T>::isEqual(first, second, referenceEpsilon);
	}

	const int offsetMagnitude = int(max(NumericT<float>::ceil(NumericT<float>::log10(float(maximalAbsValue))), float(tMagnitude))) - tMagnitude;
	ocean_assert(offsetMagnitude >= 0);

	const T adjustedEpsilon = referenceEpsilon * NumericT<T>::pow(10, T(offsetMagnitude));

	return isEqual(first, second, adjustedEpsilon);
}

template <typename T>
inline bool NumericT<T>::isNotEqual(const T first, const T second)
{
	return fabs(first - second) > eps();
}

template <>
inline bool NumericT<signed char>::isNotEqual(const signed char first, const signed char second)
{
	return first != second;
}

template <>
inline bool NumericT<unsigned char>::isNotEqual(const unsigned char first, const unsigned char second)
{
	return first != second;
}

template <>
inline bool NumericT<short>::isNotEqual(const short first, const short second)
{
	return first != second;
}

template <>
inline bool NumericT<unsigned short>::isNotEqual(const unsigned short first, const unsigned short second)
{
	return first != second;
}

template <>
inline bool NumericT<int>::isNotEqual(const int first, const int second)
{
	return first != second;
}

template <>
inline bool NumericT<unsigned int>::isNotEqual(const unsigned int first, const unsigned int second)
{
	return first != second;
}

template <>
inline bool NumericT<long>::isNotEqual(const long first, const long second)
{
	return first != second;
}

template <>
inline bool NumericT<unsigned long>::isNotEqual(const unsigned long first, const unsigned long second)
{
	return first != second;
}


template <>
inline bool NumericT<long long>::isNotEqual(const long long first, const long long second)
{
	return first != second;
}

template <>
inline bool NumericT<unsigned long long>::isNotEqual(const unsigned long long first, const unsigned long long second)
{
	return first != second;
}

template <typename T>
inline bool NumericT<T>::isNotEqual(const std::complex<T>& first, const std::complex<T>& second)
{
	return isNotEqual(first.real(), second.real()) || isNotEqual(first.imag(), second.imag());
}

template <typename T>
inline bool NumericT<T>::isNotWeakEqual(const T first, const T second)
{
	return fabs(first - second) > weakEps();
}

template <>
inline bool NumericT<signed char>::isNotWeakEqual(const signed char first, const signed char second)
{
	return first != second;
}

template <>
inline bool NumericT<unsigned char>::isNotWeakEqual(const unsigned char first, const unsigned char second)
{
	return first != second;
}

template <>
inline bool NumericT<short>::isNotWeakEqual(const short first, const short second)
{
	return first != second;
}

template <>
inline bool NumericT<unsigned short>::isNotWeakEqual(const unsigned short first, const unsigned short second)
{
	return first != second;
}

template <>
inline bool NumericT<int>::isNotWeakEqual(const int first, const int second)
{
	return first != second;
}

template <>
inline bool NumericT<unsigned int>::isNotWeakEqual(const unsigned int first, const unsigned int second)
{
	return first != second;
}

template <>
inline bool NumericT<long>::isNotWeakEqual(const long first, const long second)
{
	return first != second;
}

template <>
inline bool NumericT<unsigned long>::isNotWeakEqual(const unsigned long first, const unsigned long second)
{
	return first != second;
}

template <>
inline bool NumericT<long long>::isNotWeakEqual(const long long first, const long long second)
{
	return first != second;
}

template <>
inline bool NumericT<unsigned long long>::isNotWeakEqual(const unsigned long long first, const unsigned long long second)
{
	return first != second;
}

template <typename T>
inline bool NumericT<T>::isNotWeakEqual(const std::complex<T>& first, const std::complex<T>& second)
{
	return isNotWeakEqual(first.real(), second.real()) || isNotWeakEqual(first.imag(), second.imag());
}

template <typename T>
inline bool NumericT<T>::isNotEqual(const T first, const T second, const T epsilon)
{
	ocean_assert(epsilon >= 0);
	return fabs(first - second) > epsilon;
}

template <>
inline bool NumericT<signed char>::isNotEqual(const signed char first, const signed char second, const signed char epsilon)
{
	ocean_assert(epsilon >= (signed char)(0));
	return std::abs(first - second) > epsilon;
}

template <>
inline bool NumericT<short>::isNotEqual(const short first, const short second, const short epsilon)
{
	ocean_assert(epsilon >= short(0));
	return std::abs(first - second) > epsilon;
}

template <>
inline bool NumericT<int>::isNotEqual(const int first, const int second, const int epsilon)
{
	ocean_assert(epsilon >= 0);
	return std::abs(first - second) > epsilon;
}

template <>
inline bool NumericT<long>::isNotEqual(const long first, const long second, const long epsilon)
{
	ocean_assert(epsilon >= long(0));
	return std::abs(first - second) > epsilon;
}

template <>
inline bool NumericT<long long>::isNotEqual(const long long first, const long long second, const long long epsilon)
{
	ocean_assert(epsilon >= 0ll);
	return std::abs(first - second) > epsilon;
}

template <>
inline bool NumericT<unsigned char>::isNotEqual(const unsigned char first, const unsigned char second, const unsigned char epsilon)
{
	if (first > second)
	{
		return first - second > epsilon;
	}
	else
	{
		return second - first > epsilon;
	}
}

template <>
inline bool NumericT<unsigned short>::isNotEqual(const unsigned short first, const unsigned short second, const unsigned short epsilon)
{
	if (first > second)
	{
		return first - second > epsilon;
	}
	else
	{
		return second - first > epsilon;
	}
}

template <>
inline bool NumericT<unsigned int>::isNotEqual(const unsigned int first, const unsigned int second, const unsigned int epsilon)
{
	if (first > second)
	{
		return first - second > epsilon;
	}
	else
	{
		return second - first > epsilon;
	}
}

template <>
inline bool NumericT<unsigned long>::isNotEqual(const unsigned long first, const unsigned long second, const unsigned long epsilon)
{
	if (first > second)
	{
		return first - second > epsilon;
	}
	else
	{
		return second - first > epsilon;
	}
}

template <>
inline bool NumericT<unsigned long long>::isNotEqual(const unsigned long long first, const unsigned long long second, const unsigned long long epsilon)
{
	if (first > second)
	{
		return first - second > epsilon;
	}
	else
	{
		return second - first > epsilon;
	}
}

template <typename T>
inline bool NumericT<T>::isNotEqual(const std::complex<T>& first, const std::complex<T>& second, const T epsilon)
{
	ocean_assert(epsilon >= 0);
	return isNotEqual(first.real(), second.real(), epsilon) || isNotEqual(first.imag(), second.imag(), epsilon);
}

template <typename T>
constexpr bool NumericT<T>::isInsideRange(const T lower, const T value, const T upper, const T epsilon)
{
	ocean_assert(lower <= upper);
	ocean_assert(epsilon >= T(0));

	return value >= lower - epsilon && value <= upper + epsilon;
}

template <typename T>
constexpr bool NumericT<T>::isInsideWeakRange(const T lower, const T value, const T upper)
{
	return isInsideRange(lower, value, upper, weakEps());
}

template <typename T>
inline T NumericT<T>::asin(const T value)
{
	ocean_assert(NumericT<T>::isInsideWeakRange(T(-1), value, T(1)));

	return ::asin(max(T(-1), min(T(1), value)));
}

/**
 * Specialization of NumericT::asin().
 * @see NumericT::asin().
 */
template <>
inline float NumericT<float>::asin(const float value)
{
	ocean_assert(NumericT<float>::isInsideWeakRange(-1.0f, value, 1.0f));

	return asinf(max(-1.0f, min(1.0f, value)));
}

template <typename T>
inline T NumericT<T>::acos(const T value)
{
	ocean_assert(NumericT<T>::isInsideWeakRange(T(-1), value, T(1)));

	return ::acos(max(T(-1), min(T(1), value)));
}

/**
 * Specialization of NumericT::acos().
 * @see NumericT::acos().
 */
template <>
inline float NumericT<float>::acos(const float value)
{
	ocean_assert(NumericT<float>::isInsideWeakRange(-1.0f, value, 1.0f));

	return acosf(max(-1.0f, min(1.0f, value)));
}

template <typename T>
constexpr bool NumericT<T>::isBelow(const T value, const T upper, const T epsilon)
{
	static_assert(std::is_floating_point<T>::value, "Invalid data type!");

	ocean_assert(epsilon >= T(0));

	return value <= upper + epsilon;
}

template <typename T>
constexpr bool NumericT<T>::isAbove(const T value, const T lower, const T epsilon)
{
	static_assert(std::is_floating_point<T>::value, "Invalid data type!");

	ocean_assert(epsilon >= T(0));

	return lower - epsilon <= value;
}

template <typename T>
constexpr inline T NumericT<T>::nan()
{
	static_assert(std::is_floating_point<T>::value, "Data type must be a floating point data type");

	if constexpr (std::is_same<T, float>::value)
	{
		static_assert(sizeof(float) == sizeof(uint32_t), "Invalid data type!");

		constexpr uint32_t integerValue = 0x7FC00000u;

		float floatValue = 0.0f;
		memcpy(&floatValue, &integerValue, sizeof(integerValue));

		return T(floatValue);
	}
	else
	{
		static_assert(std::is_same<T, double>::value, "Invalid data type!");
		static_assert(sizeof(double) == sizeof(uint64_t), "Invalid data type!");

		constexpr uint64_t integerValue = 0x7FF8000000000000ull;

		double doubleValue = 0.0;
		memcpy(&doubleValue, &integerValue, sizeof(integerValue));

		return T(doubleValue);
	}
}

template <>
inline bool NumericT<float>::isNan(const float value)
{
	// with compiler optimizations, `std::isnan()` will not work anymore (the compiler simply skips the check),
	// thus implementing a custom check

	static_assert(sizeof(uint32_t) == sizeof(value), "Invalid value!");

	// IEEE 754: all exponents are encoded as 1, and the mantissa is not zero

	uint32_t tmpValue;
	memcpy(&tmpValue, &value, sizeof(tmpValue));

	constexpr uint32_t exponentBitsAllOne = 0x7F800000u;
	constexpr uint32_t mantissaBitsAllOne = 0x007FFFFFu;

	const bool allExponentBitsAreOne = (tmpValue & exponentBitsAllOne) == exponentBitsAllOne;
	const bool mantissaIsNotZero = (tmpValue & mantissaBitsAllOne) != 0u;

	return allExponentBitsAreOne && mantissaIsNotZero;
}

template <>
inline bool NumericT<double>::isNan(const double value)
{
	// with compiler optimizations, `std::isnan()` will not work anymore (the compiler simply skips the check),
	// thus implementing a custom check

	static_assert(sizeof(uint64_t) == sizeof(value), "Invalid value!");

	// IEEE 754: all exponents are encoded as 1, and the mantissa is not zero

	uint64_t tmpValue;
	memcpy(&tmpValue, &value, sizeof(tmpValue));

	constexpr uint64_t exponentBitsAllOne = 0x7FF0000000000000ull;
	constexpr uint64_t mantissaBitsAllOne = 0x000FFFFFFFFFFFFFull;

	const bool allExponentBitsAreOne = (tmpValue & exponentBitsAllOne) == exponentBitsAllOne;
	const bool mantissaIsNotZero = (tmpValue & mantissaBitsAllOne) != 0ull;

	return allExponentBitsAreOne && mantissaIsNotZero;
}

template <typename T>
inline bool NumericT<T>::isNan(const T /*value*/)
{
	// with compiler optimizations, `std::isnan()` will not work anymore (the compiler simply skips the check),
	// thus implementing a custom check

	ocean_assert(std::is_floating_point<T>::value == false);
	return false;
}

template <typename T>
inline bool NumericT<T>::isNan(const std::complex<T>& value)
{
	return NumericT<T>::isNan(value.real()) || NumericT<T>::isNan(value.imag());
}

template <typename T>
constexpr inline T NumericT<T>::inf()
{
	static_assert(std::is_floating_point<T>::value, "Data type must be a floating point data type");

	if constexpr (std::is_same<T, float>::value)
	{
		static_assert(sizeof(float) == sizeof(uint32_t), "Invalid data type!");

		constexpr uint32_t integerValue = 0x7F800000u;

		float floatValue = 0.0f;
		memcpy(&floatValue, &integerValue, sizeof(integerValue));

		return T(floatValue);
	}
	else
	{
		static_assert(std::is_same<T, double>::value, "Invalid data type!");
		static_assert(sizeof(double) == sizeof(uint64_t), "Invalid data type!");

		constexpr uint64_t integerValue = 0x7FF0000000000000ull;

		double doubleValue = 0.0;
		memcpy(&doubleValue, &integerValue, sizeof(integerValue));

		return T(doubleValue);
	}
}

template <>
inline bool NumericT<float>::isInf(const float value)
{
	// with compiler optimizations, `std::isInf()` will not work anymore (the compiler simply skips the check),
	// thus implementing a custom check

	static_assert(sizeof(uint32_t) == sizeof(value), "Invalid value!");

	// IEEE 754: all exponents are encoded as 1, and the mantissa is zero

	uint32_t tmpValue;
	memcpy(&tmpValue, &value, sizeof(tmpValue));

	constexpr uint32_t exponentBitsAllOne = 0x7F800000u;
	constexpr uint32_t mantissaBitsAllOne = 0x007FFFFFu;

	const bool allExponentBitsAreOne = (tmpValue & exponentBitsAllOne) == exponentBitsAllOne;
	const bool allMantissaBitsAreZero = (tmpValue & mantissaBitsAllOne) == 0u;

	return allExponentBitsAreOne && allMantissaBitsAreZero;
}

template <>
inline bool NumericT<double>::isInf(const double value)
{
	// with compiler optimizations, `std::isInf()` will not work anymore (the compiler simply skips the check),
	// thus implementing a custom check

	static_assert(sizeof(uint64_t) == sizeof(value), "Invalid value!");

	// IEEE 754: all exponents are encoded as 1, and the mantissa is not zero

	uint64_t tmpValue;
	memcpy(&tmpValue, &value, sizeof(tmpValue));

	constexpr uint64_t exponentBitsAllOne = 0x7FF0000000000000ull;
	constexpr uint64_t mantissaBitsAllOne = 0x000FFFFFFFFFFFFFull;

	const bool allExponentBitsAreOne = (tmpValue & exponentBitsAllOne) == exponentBitsAllOne;
	const bool allMantissaBitsAreZero = (tmpValue & mantissaBitsAllOne) == 0ull;

	return allExponentBitsAreOne && allMantissaBitsAreZero;
}

template <typename T>
inline bool NumericT<T>::isInf(const T /*value*/)
{
	// with compiler optimizations, `std::isinf()` will not work anymore (the compiler simply skips the check),
	// thus implementing a custom check

	ocean_assert(std::is_floating_point<T>::value == false);
	return false;
}

template <>
inline bool NumericT<long double>::isInf(const long double value)
{
	return std::isinf(value);
}

template <typename T>
inline bool NumericT<T>::isInf(const std::complex<T>& value)
{
	return NumericT<T>::isInf(value.real()) || NumericT<T>::isInf(value.imag());
}

template <typename T>
template <typename TValue>
inline bool NumericT<T>::isInsideValueRange(const TValue& value)
{
	if constexpr (std::is_same<T, TValue>::value)
	{
		// e.g., [int8_t, int8_t], [uint8_t, uint8_t], [float, float], ...

		return true;
	}

	if constexpr (std::is_floating_point<T>::value && std::is_floating_point<TValue>::value)
	{
		// either [float, double], or [double, float]

		// float/double values fit in each other (however, may be casted to inf)

		return true;
	}

	if constexpr (std::is_integral<T>::value && std::is_integral<TValue>::value)
	{
		// e.g., int8_t, uint8_t, int16_t, ...

		if constexpr (std::is_signed<T>::value == std::is_signed<TValue>::value)
		{
			// e.g., [int8_t, int16_t], [int16_t, int8_t], [int32_t, int64_t], ...

			if constexpr (sizeof(TValue) <= sizeof(T))
			{
				// e.g., [int16_t, int8_t], [uint64_t, uint8_t], ...

				return true;
			}
			else
			{
				ocean_assert(sizeof(T) < sizeof(TValue));

				// e.g., [int8_t, int16_t], [uint8_t, uint64_t], ...

				return TValue(NumericT<T>::minValue()) <= value && value <= TValue(NumericT<T>::maxValue());
			}
		}

		if constexpr (std::is_signed<T>::value)
		{
			ocean_assert(!std::is_signed<TValue>::value);

			// e.g., [int8_t, uint8_t], [int32_t, uint8_t], [int64_t, uint16_t], [int8_t, uint64_t], ...

			if constexpr (sizeof(TValue) < sizeof(T))
			{
				// e.g., [int16_t, uint8_t], [int64_t, uint16_t], ...

				return true;
			}

			// e.g., [int8_t, uint8_t], [int8_t, uint32_t], ...

			return value <= TValue(NumericT<T>::maxValue());
		}

		if constexpr (std::is_signed<TValue>::value)
		{
			ocean_assert(!std::is_signed<T>::value);

			// e.g., [uint8_t, int8_t], [uint32_t, int8_t], [uint64_t, int16_t], [uint8_t, int64_t], ...

			if (value < TValue(0))
			{
				return false;
			}

			if constexpr (sizeof(TValue) < sizeof(T))
			{
				// e.g., [uint16_t, int8_t], [uint_64_t, int16_t], ...

				return true;
			}
			else if constexpr (sizeof(TValue) == sizeof(T))
			{
				// e.g., [uint8_t, int8_t], [uint32_t, int32_t], [uint64_t, int64_t], ...

				ocean_assert(value >= TValue(0));

				return true;
			}

			// e.g.,  [uint8_t, int16_t], [uint32_t, int64_t], ...

			ocean_assert(value >= TValue(0));

			return value <= TValue(NumericT<T>::maxValue());
		}
	}

	return false;
}

template <typename T>
constexpr inline T NumericT<T>::deg2rad(const T deg)
{
	return deg * T(0.017453292519943295769236907684886);
}

template <typename T>
constexpr inline T NumericT<T>::rad2deg(const T rad)
{
	return rad * T(57.295779513082320876798154814105);
}

template <typename T>
constexpr inline T NumericT<T>::maxValue()
{
	return std::numeric_limits<T>::max();
}

template <typename T>
constexpr inline T NumericT<T>::minValue()
{
	return std::numeric_limits<T>::lowest();
}

template <typename T>
constexpr inline T NumericT<T>::sign(const T& value)
{
	static_assert(std::numeric_limits<T>::is_signed, "T must be a signed data type!");

	return T((T(0) < value) - (value < T(0)));
}

template <typename T>
constexpr inline T NumericT<T>::copySign(const T signReceiver, const T signProvider)
{
	if (signProvider >= 0)
	{
		return abs(signReceiver);
	}

	return -abs(signReceiver);
}

#if 0  // although the following code is a create optimization, we do not use it to avoid strict-aliasing compiler warnings

/**
 * Specialization of NumericT::copySign().
 * @see NumericT::copySign().
 */
template <>
constexpr inline double NumericT<double>::copySign(const double first, const double second)
{
	const unsigned long long value = ((*(unsigned long long*)&first) & 0x7FFFFFFFFFFFFFFFll)
							| ((*(unsigned long long*)&second) & 0x8000000000000000ll);

#ifdef OCEAN_DEBUG

	ocean_assert(sizeof(unsigned long long) == sizeof(double));

	const double result = *(double*)&value;

	double testValue = 0;

	if (second >= 0.0)
		testValue = NumericT<double>::abs(first);
	else
		testValue = -NumericT<double>::abs(first);

	ocean_assert(result == testValue || second == 0.0);

#endif

	return *(double*)&value;
}

/**
 * Specialization of NumericT::copySign().
 * @see NumericT::copySign().
 */
template <>
constexpr inline float NumericT<float>::copySign(const float first, const float second)
{
	const unsigned int value = ((*(unsigned int*)&first) & 0x7FFFFFFF) | ((*(unsigned int*)&second) & 0x80000000);

#ifdef OCEAN_DEBUG

	ocean_assert(sizeof(unsigned int) == sizeof(float));

	const float result = *(float*)&value;

	float testValue = 0;

	if (second >= 0.0f)
		testValue = NumericT<float>::abs(first);
	else
		testValue = -NumericT<float>::abs(first);

	ocean_assert(result == testValue || second == 0.0);

#endif

	return *(float*)&value;
}

#endif // #if 0

template <typename T>
constexpr inline T NumericT<T>::invertSign(const T signReceiver, const T signProvider)
{
	if (signProvider < 0)
	{
		return abs(signReceiver);
	}

	return -abs(signReceiver);
}

#if 0  // although the following code is a create optimization, we do not use it to avoid strict-aliasing compiler warnings

/**
 * Specialization of NumericT::invertSign().
 * @see NumericT::invertedSign().
 */
template <>
constexpr inline double NumericT<double>::invertSign(const double first, const double second)
{
	const unsigned long long value = ((*(unsigned long long*)&first) & 0x7FFFFFFFFFFFFFFFll)
							| ((~*(unsigned long long*)&second) & 0x8000000000000000ll);

#ifdef OCEAN_DEBUG

	ocean_assert(sizeof(unsigned long long) == sizeof(double));

	const double result = *(double*)&value;

	double testValue = 0;

	if (second < 0)
		testValue = NumericT<double>::abs(first);
	else
		testValue = -NumericT<double>::abs(first);

	ocean_assert(result == testValue);

#endif


	return *(double*)&value;
}

/**
 * Specialization of NumericT::invertSign().
 * @see NumericT::invertSign().
 */
template <>
constexpr inline float NumericT<float>::invertSign(const float first, const float second)
{
	const unsigned int value = ((*(unsigned int*)&first) & 0x7FFFFFFF) | ((~*(unsigned int*)&second) & 0x80000000);

#ifdef OCEAN_DEBUG

	ocean_assert(sizeof(unsigned int) == sizeof(float));

	const float result = *(float*)&value;

	double testValue = 0;

	if (second < 0)
		testValue = NumericT<float>::abs(first);
	else
		testValue = -NumericT<float>::abs(first);

	ocean_assert(result == testValue);

#endif

	return *(float*)&value;
}

#endif // #if 0

template <typename T>
T NumericT<T>::pythagoras(const T a, const T b)
{
	const T absA = NumericT<T>::abs(a);
	const T absB = NumericT<T>::abs(b);

	if (absA > absB)
	{
		const T factor = absB / absA;
		return absA * sqrt(1 + factor * factor);
	}
	else if (absB != 0)
	{
		const T factor = absA / absB;
		return absB * sqrt(1 + factor * factor);
	}

	return 0;
}

template <typename T>
inline T NumericT<T>::gaussianDistribution(const T x, const T sigma)
{
	ocean_assert(sigma > eps());

	const T inverseSigma = T(1) / sigma;

	return inverseSigma * T(0.3989422804014326779) * NumericT<T>::exp(T(-0.5) * x * x * inverseSigma * inverseSigma);
}

template <typename T>
inline T NumericT<T>::gaussianDistribution(const T x, const T x0, const T sigma)
{
	ocean_assert(sigma > eps());

	const T inverseSigma = T(1) / sigma;
	const T x_x0 = x - x0;

	return inverseSigma * T(0.3989422804014326779) * NumericT<T>::exp(T(-0.5) * x_x0 * x_x0 * inverseSigma * inverseSigma);
}

template <typename T>
inline T NumericT<T>::normalizedGaussianDistribution(const T x, const T sigma)
{
	ocean_assert(sigma > eps());

	const T inverseSigma = T(1) / sigma;

	return NumericT<T>::exp(T(-0.5) * x * x * inverseSigma * inverseSigma);
}

template <typename T>
inline T NumericT<T>::normalizedGaussianDistribution(const T x, const T x0, const T sigma)
{
	ocean_assert(sigma > eps());

	const T inverseSigma = T(1) / sigma;
	const T x_x0 = x - x0;

	return NumericT<T>::exp(T(-0.5) * x_x0 * x_x0 * inverseSigma * inverseSigma);
}

template <typename T>
inline T NumericT<T>::gaussianDistribution2(const T x, const T y, const T sigmaX, const T sigmaY)
{
	ocean_assert(sigmaX > eps() && sigmaY > eps());

	const T inverseSigmaX = T(1) / sigmaX;
	const T inverseSigmaY = T(1) / sigmaY;

	return inverseSigmaX * inverseSigmaY * T(0.15915494309189533576888) * NumericT<T>::exp(T(-0.5) * (x * x * inverseSigmaX * inverseSigmaX + y * y * inverseSigmaY * inverseSigmaY));
}

template <typename T>
inline T NumericT<T>::gaussianDistribution2(const T x, const T y, const T x0, const T y0, const T sigmaX, const T sigmaY)
{
	ocean_assert(sigmaX > eps() && sigmaY > eps());

	const T inverseSigmaX = T(1) / sigmaX;
	const T inverseSigmaY = T(1) / sigmaY;

	const T x_x0 = x - x0;
	const T y_y0 = y - y0;

	return inverseSigmaX * inverseSigmaY * T(0.15915494309189533576888) * NumericT<T>::exp(T(-0.5) * (x_x0 * x_x0 * inverseSigmaX * inverseSigmaX + y_y0 * y_y0 * inverseSigmaY * inverseSigmaY));
}

template <typename T>
inline T NumericT<T>::normalizedGaussianDistribution2(const T x, const T y, const T sigmaX, const T sigmaY)
{
	ocean_assert(sigmaX > eps() && sigmaY > eps());

	const T inverseSigmaX = T(1) / sigmaX;
	const T inverseSigmaY = T(1) / sigmaY;

	return NumericT<T>::exp(T(-0.5) * (x * x * inverseSigmaX * inverseSigmaX + y * y * inverseSigmaY * inverseSigmaY));
}

template <typename T>
inline T NumericT<T>::normalizedGaussianDistribution2(const T x, const T y, const T x0, const T y0, const T sigmaX, const T sigmaY)
{
	ocean_assert(sigmaX > eps() && sigmaY > eps());

	const T inverseSigmaX = T(1) / sigmaX;
	const T inverseSigmaY = T(1) / sigmaY;

	const T x_x0 = x - x0;
	const T y_y0 = y - y0;

	return NumericT<T>::exp(T(-0.5) * (x_x0 * x_x0 * inverseSigmaX * inverseSigmaX + y_y0 * y_y0 * inverseSigmaY * inverseSigmaY));
}

template <typename T>
inline T NumericT<T>::gaussianDistribution3(const T x, const T y, const T z, const T sigmaX, const T sigmaY, const T sigmaZ)
{
	ocean_assert(sigmaX > eps() && sigmaY > eps() && sigmaZ > eps());

	const T inverseSigmaX = T(1) / sigmaX;
	const T inverseSigmaY = T(1) / sigmaY;
	const T inverseSigmaZ = T(1) / sigmaZ;

	return inverseSigmaX * inverseSigmaY * inverseSigmaZ * T(0.06349363593424096978576330493464)
			* NumericT<T>::exp(T(-0.5) * (x * x * inverseSigmaX * inverseSigmaX + y * y * inverseSigmaY * inverseSigmaY + z * z * inverseSigmaZ * inverseSigmaZ));
}

template <typename T>
inline T NumericT<T>::gaussianDistribution3(const T x, const T y, const T z, const T x0, const T y0, const T z0, const T sigmaX, const T sigmaY, const T sigmaZ)
{
	ocean_assert(sigmaX > eps() && sigmaY > eps() && sigmaZ > eps());

	const T inverseSigmaX = T(1) / sigmaX;
	const T inverseSigmaY = T(1) / sigmaY;
	const T inverseSigmaZ = T(1) / sigmaZ;

	const T x_x0 = x - x0;
	const T y_y0 = y - y0;
	const T z_z0 = z - z0;

	return inverseSigmaX * inverseSigmaY * inverseSigmaZ * T(0.06349363593424096978576330493464)
			* NumericT<T>::exp(T(-0.5) * (x_x0 * x_x0 * inverseSigmaX * inverseSigmaX + y_y0 * y_y0 * inverseSigmaY * inverseSigmaY + z_z0 * z_z0 * inverseSigmaZ * inverseSigmaZ));
}

template <typename T>
inline T NumericT<T>::normalizedGaussianDistribution3(const T x, const T y, const T z, const T sigmaX, const T sigmaY, const T sigmaZ)
{
	ocean_assert(sigmaX > eps() && sigmaY > eps() && sigmaZ > eps());

	const T inverseSigmaX = T(1) / sigmaX;
	const T inverseSigmaY = T(1) / sigmaY;
	const T inverseSigmaZ = T(1) / sigmaZ;

	return NumericT<T>::exp(T(-0.5) * (x * x * inverseSigmaX * inverseSigmaX + y * y * inverseSigmaY * inverseSigmaY + z * z * inverseSigmaZ * inverseSigmaZ));
}

template <typename T>
inline T NumericT<T>::normalizedGaussianDistribution3(const T x, const T y, const T z, const T x0, const T y0, const T z0, const T sigmaX, const T sigmaY, const T sigmaZ)
{
	ocean_assert(sigmaX > eps() && sigmaY > eps() && sigmaZ > eps());

	const T inverseSigmaX = T(1) / sigmaX;
	const T inverseSigmaY = T(1) / sigmaY;
	const T inverseSigmaZ = T(1) / sigmaZ;

	const T x_x0 = x - x0;
	const T y_y0 = y - y0;
	const T z_z0 = z - z0;

	return NumericT<T>::exp(T(-0.5) * (x_x0 * x_x0 * inverseSigmaX * inverseSigmaX + y_y0 * y_y0 * inverseSigmaY * inverseSigmaY + z_z0 * z_z0 * inverseSigmaZ * inverseSigmaZ));
}

}

#endif // META_OCEAN_MATH_NUMERIC_H
