/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/DataType.h"

namespace Ocean
{

Float16::operator float() const
{
	if (data_.ieee_.exponent_ == 0u)
	{
		if (data_.ieee_.fraction_ == 0u)
		{
			if (data_.ieee_.sign_)
			{
				return -0.0f;
			}

			return 0.0f;
		}
		else
		{
			const float mantissa = float(data_.ieee_.fraction_) / 1024.0f;

			constexpr float factor = float(1.0 / 16384.0);

			if (data_.ieee_.sign_)
			{
				return -mantissa * factor;
			}

			return mantissa * factor;
		}
	}
	else if (data_.ieee_.exponent_ == 31u)
	{
		static_assert(sizeof(uint32_t) == sizeof(float), "Invalid data type!");

		constexpr uint32_t integerValue = 0x7F800000u; // see NumericF::infinity()

		float floatValueInfinity;
		memcpy(&floatValueInfinity, &integerValue, sizeof(integerValue));

		if (data_.ieee_.sign_)
		{
			return -floatValueInfinity;
		}

		return floatValueInfinity;
	}
	else
	{
		const float mantissa = 1.0f + float(data_.ieee_.fraction_) / 1024.0f;

		const float factor = float(1u << data_.ieee_.exponent_) / 32768.0f;

		if (data_.ieee_.sign_)
		{
			return -mantissa * factor;
		}

		return mantissa * factor;
	}
}

}
