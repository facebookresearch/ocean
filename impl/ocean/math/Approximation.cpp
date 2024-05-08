/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/math/Approximation.h"
#include "ocean/math/Numeric.h"

namespace Ocean
{

uint8_t Approximation::sqrt(const uint8_t value)
{
	const static uint8_t* lookupTable = sqrtLookup8();

	return lookupTable[value];
}

uint8_t Approximation::sqrt(const uint16_t value)
{
	const static uint8_t* lookupTable = sqrtLookup16();

	ocean_assert(min((uint32_t)(Numeric::sqrt(Scalar(value)) + Scalar(0.5)), 255u) == lookupTable[value]);

	return lookupTable[value];
}

uint16_t Approximation::sqrt(const uint32_t value)
{
	ocean_assert(value <= 131071u);

	const static uint16_t* lookupTable = sqrtLookup17();

	return lookupTable[value >> 1u];
}

template <typename T>
T Approximation::atan2(const T y, const T x)
{
	ocean_assert(NumericT<T>::isNotEqualEps(y) || NumericT<T>::isNotEqualEps(x));

	const T coeff_1 = NumericT<T>::pi_4();
	const T absY = NumericT<T>::abs(y) + NumericT<T>::eps();

	if (x >= 0)
	{
		ocean_assert(x + absY != 0);

		const T r = (x - absY) / (x + absY);

		return y < 0 ? -(coeff_1 + (T(0.1963) * r * r - T(0.9817f)) * r) : (coeff_1 + (T(0.1963) * r * r - T(0.9817f)) * r);
	}

	const T coeff_2 = 3 * coeff_1;

	ocean_assert(absY - x != 0);
	const T r = (x + absY) / (absY - x);

	if (y < 0)
	{
		return -(coeff_2 + (T(0.1963) * r * r - T(0.9817f)) * r);
	}
	else
	{
		return (coeff_2 + (T(0.1963) * r * r - T(0.9817f)) * r);
	}
}

// Explicit instantiations
template float OCEAN_MATH_EXPORT Approximation::atan2<float>(const float, const float);
template double OCEAN_MATH_EXPORT Approximation::atan2<double>(const double, const double);

const uint8_t* Approximation::sqrtLookup8()
{
	static uint8_t lookupTable[256];
	static bool createLookup = true;

	if (createLookup)
	{
		createLookup = false;

		for (unsigned int n = 0u; n < 256u; ++n)
		{
			lookupTable[n] = uint8_t(NumericF::sqrt(float(n)) + 0.5f);
		}
	}

	return lookupTable;
}

const unsigned char* Approximation::sqrtLookup16()
{
	static uint8_t lookupTable[65536];
	static bool createLookup = true;

	if (createLookup)
	{
		createLookup = false;

		for (unsigned int n = 0u; n < 65536u; ++n)
		{
			// we will have an accurate sqrt result for values within the range [0, 65280]
			lookupTable[n] = uint8_t(min(int32_t(NumericF::sqrt(float(n)) + 0.5f), 255));
		}
	}

	return lookupTable;
}

const uint16_t* Approximation::sqrtLookup17()
{
	static uint16_t lookupTable[65536];
	static bool createLookup = true;

	if (createLookup)
	{
		createLookup = false;

		for (unsigned int n = 0u; n < 65536u; ++n)
		{
			lookupTable[n] = uint16_t(NumericF::sqrt(float(n << 1u)) + 0.5f);
		}
	}

	return lookupTable;
}

}
