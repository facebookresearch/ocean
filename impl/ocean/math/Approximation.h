/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_APPROXIMATION_H
#define META_OCEAN_MATH_APPROXIMATION_H

#include "ocean/math/Math.h"

namespace Ocean
{

/**
 * This class implements several numeric function with approximated results but with fast performances.
 * @ingroup math
 */
class OCEAN_MATH_EXPORT Approximation
{
	public:

		/**
		 * Returns the square root for values inside range [0, 2^8).
		 * A lookup table with 2^8 = 256 bytes is used, which will be created before the first usage.
		 * @param value The value to return the square root for, with range [0, 255]
		 * @return Rounded square root
		 */
		static uint8_t sqrt(const uint8_t value);

		/**
		 * Returns the square root for values inside range [0, 2^16).
		 * A lookup table with 2^16 = 65.536 bytes is used, which will be created before the first usage.
		 * @param value The value to return the square root for, with range [0, 65535]
		 * @return Rounded square root
		 */
		static uint8_t sqrt(const uint16_t value);

		/**
		 * Returns the square root for values inside range [0, 2^17).
		 * Beware: No range check will be done for the given value.
		 * A lookup table with 2^16 * 2 = 131.072 bytes is used, which will be created before the first usage.
		 * Therefore, the resulting value does not have the best accuracy due to the reduces lookup table dimension.
		 * @param value The value to return the square root for, with range [0, 131071]
		 * @return Rounded square root
		 */
		static uint16_t sqrt(const uint32_t value);

		/**
		 * Returns the approximated arc tangent with an error below one degree.
		 * @param y Y coordinate value, with range (-infinity, infinity), must not be zero if x is zero
		 * @param x X coordinate value, with range (-infinity, infinity), must not be zero if y is zero
		 * @return Approximated arc tangent in radian, with range [-PI, PI]
		 * @tparam T The data type of the scalar, either 'float' or 'double'
		 */
		template <typename T>
		static T atan2(const T y, const T x);

		/**
		 * Returns the approximated exponential function exp(x) = e^x.
		 * This function provides two accuracy modes.<br>
		 * The accuracy is as follows for an x86 CPU, within the value range [-1, 1]:
		 * <pre>
		 * 32 bit floating point values:<br>
		 *  Lower accuracy: Median (p50) error: 0.00041, p99 error: 0.00498
		 * Higher accuracy: Median (p50) error: 0.00008, p99 error: 0.00096
		 *
		 * 64 bit floating point values:<br>
		 *  Lower accuracy: Median (p50) error: 0.00041, p99 error: 0.00498
		 * Higher accuracy: Median (p50) error: 0.00003, p99 error: 0.00031
		 * </pre>
		 * @param x The value for which exp(x) will be approximated, with range (-infinity, infinity)
		 * @return The approximated exponential value
		 * @tparam T Data type of the provided value, either 'float' or 'double'
		 * @tparam tHigherAccuracy True, to approximate exp with higher accuracy; False, to get the lower accuracy
		 */
		template <typename T, bool tHigherAccuracy>
		static inline T exp(T x);

	protected:

		/**
		 * Creates a lookup table for the sqrt function with range [0, 255].
		 * @return Pointer to the static lookup table
		 */
		static const uint8_t* sqrtLookup8();

		/**
		 * Creates a lookup table for the sqrt function with range [0, 65,535].
		 * @return Pointer to the static lookup table
		 */
		static const uint8_t* sqrtLookup16();

		/**
		 * Creates a lookup table for the sqrt function with range [0, 131,071].
		 * @return Pointer to the static lookup table
		 */
		static const uint16_t* sqrtLookup17();
};

template <typename T, bool tHigherAccuracy>
inline T Approximation::exp(T x)
{
	// e^x = lim (n->inf) (1 + x/n)^n

	if constexpr (tHigherAccuracy)
	{
		x = T(1) + x * T(0.000244140625); // 1/4096

		x *= x;
		x *= x;
		x *= x;
		x *= x;

		x *= x;
		x *= x;
		x *= x;
		x *= x;

		x *= x;
		x *= x;
		x *= x;
		x *= x;

		return x;
	}
	else
	{
		x = T(1) + x * T(0.00390625); // 1/256

		x *= x;
		x *= x;
		x *= x;
		x *= x;

		x *= x;
		x *= x;
		x *= x;
		x *= x;

		return x;
	}
}

}

#endif // META_OCEAN_MATH_APPROXIMATION_H
