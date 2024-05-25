/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/math/HSVAColor.h"
#include "ocean/math/RGBAColor.h"
#include "ocean/math/Numeric.h"

#include "ocean/base/Utilities.h"

namespace Ocean
{

HSVAColor::HSVAColor()
{
	values_[0] = 0.0f;
	values_[1] = 0.0f;
	values_[2] = 1.0f;
	values_[3] = 1.0f;

	ocean_assert(isValid());
}

HSVAColor::HSVAColor(const float hue, const float saturation, const float value)
{
	values_[0] = hue;
	values_[1] = saturation;
	values_[2] = value;
	values_[3] = 1.0f;

	ocean_assert(isValid());
}

HSVAColor::HSVAColor(const float hue, const float saturation, const float value, const float alpha)
{
	values_[0] = hue;
	values_[1] = saturation;
	values_[2] = value;
	values_[3] = alpha;

	ocean_assert(isValid());
}

HSVAColor::HSVAColor(const float* valueArray)
{
	ocean_assert(valueArray);

	memcpy(values_, valueArray, sizeof(float) * 4);
	ocean_assert(isValid());
}

HSVAColor::HSVAColor(const RGBAColor& color)
{
	ocean_assert(color.isValid());

	const float maxValue = max(color.red(), max(color.green(), color.blue()));
	const float minValue = min(color.red(), min(color.green(), color.blue()));
	const float diffValue = maxValue - minValue;

	///  red == green == blue
	if (NumericF::isEqualEps(diffValue))
	{
		values_[0] = 0.0f;
	}
	else
	{
		if (maxValue == color.red())
		{
			values_[0] = NumericF::pi_3() * (color.green() - color.blue()) / diffValue;
		}
		else if (maxValue == color.green())
		{
			values_[0] = NumericF::pi_3() * (2.0f + (color.blue() - color.red()) / diffValue);
		}
		else if (maxValue == color.blue())
		{
			values_[0] = NumericF::pi_3() * (4.0f + (color.red() - color.green()) / diffValue);
		}

		values_[0] = NumericF::fmod(values_[0], NumericF::pi2());

		if (values_[0] < 0)
		{
			values_[0] += NumericF::pi2();
		}
	}

	if (NumericF::isEqualEps(maxValue))
	{
		values_[1] = 0.0f;
	}
	else
	{
		values_[1] = (maxValue - minValue) / maxValue;
	}

	values_[2] = maxValue;
	values_[3] = color.alpha();

	ocean_assert(isValid());
}

bool HSVAColor::setHue(const float hue)
{
	if (hue < 0.0f || hue > 1.0f)
	{
		return false;
	}

	values_[0] = hue;
	return true;
}

bool HSVAColor::setSaturation(const float saturation)
{
	if (saturation < 0.0f || saturation > 1.0f)
	{
		return false;
	}

	values_[1] = saturation;
	return true;
}

bool HSVAColor::setValue(const float value)
{
	if (value < 0.0f || value > 1.0f)
	{
		return false;
	}

	values_[2] = value;
	return true;
}

bool HSVAColor::setAlpha(const float alpha)
{
	if (alpha < 0.0f || alpha > 1.0f)
	{
		return false;
	}

	values_[3] = alpha;
	return true;
}

HSVAColor HSVAColor::interpolate(const HSVAColor& color, const float factor) const
{
	ocean_assert(isValid() && color.isValid());

	ocean_assert(factor >= 0.0f && factor <= 1.0f);

	const float factor1 = 1.0f - factor;
	float interpolatedHue = -1.0f;

	if (values_[0] > color.values_[0])
	{
		// if the colors have to be interpolated over the zero angle
		if (values_[0] - color.values_[0] > NumericF::pi2() - values_[0] + color.values_[0])
		{
			ocean_assert(values_[0] - NumericF::pi2() < 0.0f);

			interpolatedHue = (values_[0] - NumericF::pi2()) * factor1 + color.values_[0] * factor;

			if (interpolatedHue < 0)
			{
				interpolatedHue += NumericF::pi2();
			}
		}
		else
		{
			interpolatedHue = values_[0] * factor1 + color.values_[0] * factor;
		}
	}
	else // values[0] <= color.values[0]
	{
		// if the colors have to be interpolated over the zero angle
		if (color.values_[0] - values_[0] > NumericF::pi2() - color.values_[0] + values_[0])
		{
			ocean_assert(color.values_[0] - NumericF::pi2() < 0.0f);

			interpolatedHue = (color.values_[0] - NumericF::pi2()) * factor + values_[0] * factor1;

			if (interpolatedHue < 0)
			{
				interpolatedHue += NumericF::pi2();
			}
		}
		else
		{
			interpolatedHue = values_[0] * factor1 + color.values_[0] * factor;
		}
	}

	return HSVAColor(NumericF::angleAdjustPositive(interpolatedHue),
						minmax<float>(0.0f, values_[1] * factor1 + color.values_[1] * factor, 1.0f),
						minmax<float>(0.0f, values_[2] * factor1 + color.values_[2] * factor, 1.0f),
						minmax<float>(0.0f, values_[3] * factor1 + color.values_[3] * factor, 1.0f));
}

bool HSVAColor::isValid() const
{
	return values_[0] >= 0.0f && values_[0] < NumericF::pi2()
		&& values_[1] >= 0.0f && values_[1] <= 1.0f
		&& values_[2] >= 0.0f && values_[2] <= 1.0f
		&& values_[3] >= 0.0f && values_[3] <= 1.0f;
}

bool HSVAColor::operator==(const HSVAColor& right) const
{
	ocean_assert(isValid() && right.isValid());

	return NumericF::isEqual(values_[0], right.values_[0]) && NumericF::isEqual(values_[1], right.values_[1])
			&& NumericF::isEqual(values_[2], right.values_[2]) && NumericF::isEqual(values_[3], right.values_[3]);
}

}
