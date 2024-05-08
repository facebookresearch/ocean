/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/math/RGBAColor.h"
#include "ocean/math/HSVAColor.h"
#include "ocean/math/Numeric.h"

#include "ocean/base/Utilities.h"

namespace Ocean
{

RGBAColor::RGBAColor()
{
	values_[0] = 1.0f;
	values_[1] = 1.0f;
	values_[2] = 1.0f;
	values_[3] = 1.0f;

	ocean_assert(isValid());
}

RGBAColor::RGBAColor(const bool valid)
{
	if (valid)
	{
		values_[0] = 1.0f;
		values_[1] = 1.0f;
		values_[2] = 1.0f;
		values_[3] = 1.0f;
	}
	else
	{
		values_[0] = -1.0f;
		values_[1] = -1.0f;
		values_[2] = -1.0f;
		values_[3] = -1.0f;
	}

	ocean_assert(valid == isValid());
}

RGBAColor::RGBAColor(const float red, const float green, const float blue)
{
	values_[0] = red;
	values_[1] = green;
	values_[2] = blue;
	values_[3] = 1.0f;

	ocean_assert(isValid());
}

RGBAColor::RGBAColor(const float red, const float green, const float blue, const float alpha)
{
	values_[0] = red;
	values_[1] = green;
	values_[2] = blue;
	values_[3] = alpha;

	ocean_assert(isValid());
}

RGBAColor::RGBAColor(const float colorTemperature)
{
	ocean_assert(colorTemperature >= 1000.0f && colorTemperature <= 40000.0f);

	// https://tannerhelland.com/2012/09/18/convert-temperature-rgb-algorithm-code.html

	const float colorTemperature_100 = minmax(10.0f, colorTemperature * 0.01f, 400.0f);

	float red = 1.0f;

	if (colorTemperature_100 > 66.0f)
	{
		red = minmax(0.0f, 1.2929361860627f * NumericF::pow(colorTemperature_100 - 60.0f, -0.1332047592f), 1.0f);
	}

	float green = -1.0f;

	if (colorTemperature_100 <= 66.f)
	{
		green = minmax(0.0f, 0.390081578769f * NumericF::log(colorTemperature_100) - 0.6318414437886f, 1.0f);
	}
	else
	{
		green = minmax(0.0f, 1.1298908608952f * NumericF::pow(colorTemperature_100 - 60.0f, -0.0755148492f), 1.0f);
	}

	float blue = 1.0f;

	if (colorTemperature_100 <= 19.0f)
	{
		blue = 0.0f;
	}
	else if (colorTemperature_100 < 66.0f)
	{
		blue = minmax(0.0f, 0.5432067891101f * NumericF::log(colorTemperature_100 - 10.0f) - 1.19625408914f, 1.0f);
	}

	values_[0] = red;
	values_[1] = green;
	values_[2] = blue;
	values_[3] = 1.0f;

	ocean_assert(isValid());
}

RGBAColor::RGBAColor(const float* arrayValue)
{
	ocean_assert(arrayValue);

	memcpy(values_, arrayValue, sizeof(float) * 4);

	ocean_assert(isValid());
}

RGBAColor::RGBAColor(const HSVAColor& color)
{
	ocean_assert(color.isValid());

	// h = deg / 60 degrees
	const int h = int(color.hue() * 0.95492965855137201461330258023509f);
	const float f = color.hue() * 0.95492965855137201461330258023509f - float(h);

	switch (h)
	{
		case 0:
		case 6:
			values_[0] = color.value();
			values_[1] = color.value() * (1.0f - color.saturation() * (1.0f - f));
			values_[2] = color.value() * (1.0f - color.saturation());
			break;

		case 1:
			values_[0] = color.value() * (1.0f - color.saturation() * f);
			values_[1] = color.value();
			values_[2] = color.value() * (1.0f - color.saturation());
			break;

		case 2:
			values_[0] = color.value() * (1.0f - color.saturation());
			values_[1] = color.value();
			values_[2] = color.value() * (1.0f - color.saturation() * (1.0f - f));
			break;

		case 3:
			values_[0] = color.value() * (1.0f - color.saturation());
			values_[1] = color.value() * (1.0f - color.saturation() * f);
			values_[2] = color.value();
			break;

		case 4:
			values_[0] = color.value() * (1.0f - color.saturation() * (1.0f - f));
			values_[1] = color.value() * (1.0f - color.saturation());
			values_[2] = color.value();
			break;

		case 5:
			values_[0] = color.value();
			values_[1] = color.value() * (1.0f - color.saturation());
			values_[2] = color.value() * (1.0f - color.saturation() * f);
			break;
	};

	values_[3] = color.alpha();

	ocean_assert(isNormalized());
}

bool RGBAColor::setRed(const float red)
{
	ocean_assert(isValid());

	if (red < 0.0f)
	{
		return false;
	}

	values_[0] = red;
	return true;
}

bool RGBAColor::setGreen(const float green)
{
	ocean_assert(isValid());

	if (green < 0.0f)
	{
		return false;
	}

	values_[1] = green;
	return true;
}

bool RGBAColor::setBlue(const float blue)
{
	ocean_assert(isValid());

	if (blue < 0.0f)
	{
		return false;
	}

	values_[2] = blue;
	return true;
}

bool RGBAColor::setAlpha(const float alpha)
{
	ocean_assert(isValid());

	if (alpha < 0.0f || alpha > 1.0f)
	{
		return false;
	}

	values_[3] = alpha;
	return true;
}

RGBAColor RGBAColor::combined(const RGBAColor& color) const
{
	ocean_assert(isValid());
	ocean_assert(color.isValid());

	return RGBAColor(values_[0] + color.values_[0], values_[1] + color.values_[1], values_[2] + color.values_[2], (values_[3] + color.values_[3]) * 0.5f);
}

void RGBAColor::combine(const RGBAColor& color)
{
	ocean_assert(isValid());
	ocean_assert(color.isValid());

	values_[0] += color.values_[0];
	values_[1] += color.values_[1];
	values_[2] += color.values_[2];
	values_[3] = (values_[3] + color.values_[3]) * 0.5f;
}

bool RGBAColor::isValid() const
{
	return values_[0] >= 0.0f && values_[1] >= 0.0f && values_[2] >= 0.0f && values_[3] >= 0.0f && values_[3] <= 1.0f;
}

bool RGBAColor::isNormalized() const
{
	return NumericF::isInsideRange(0.0f, values_[0], 1.0f) && NumericF::isInsideRange(0.0f, values_[1], 1.0f)
			&& NumericF::isInsideRange(0.0f, values_[2], 1.0f) && NumericF::isInsideRange(0.0f, values_[3], 1.0f);
}

RGBAColor RGBAColor::normalized() const
{
	ocean_assert(isValid());

	const float highest = max(values_[0], max(values_[1], values_[2]));

	if (highest > 1.0f)
	{
		const float factor = 1.0f / highest;
		return RGBAColor(values_[0] * factor, values_[1] * factor, values_[2] * factor, values_[3]);
	}

	return *this;
}

void RGBAColor::normalize()
{
	ocean_assert(isValid());

	const float highest = max(values_[0], max(values_[1], values_[2]));

	if (highest > 1.0f)
	{
		const float factor = 1.0f / highest;
		values_[0] *= factor;
		values_[1] *= factor;
		values_[2] *= factor;
	}
}

RGBAColor RGBAColor::clamped() const
{
	ocean_assert(isValid());

	return RGBAColor(max(0.0f, min(1.0f, values_[0])), max(0.0f, min(1.0f, values_[1])), max(0.0f, min(1.0f, values_[2])), max(0.0f, min(1.0f, values_[3])));
}

void RGBAColor::clamp()
{
	ocean_assert(isValid());

	values_[0] =
	values_[1] = max(0.0f, min(1.0f, values_[1]));
	values_[2] = max(0.0f, min(1.0f, values_[2]));
	values_[3] = max(0.0f, min(1.0f, values_[3]));
}

bool RGBAColor::operator==(const RGBAColor& right) const
{
	ocean_assert(isValid() && right.isValid());

	return NumericF::isEqual(values_[0], right.values_[0]) && NumericF::isEqual(values_[1], right.values_[1])
			&& NumericF::isEqual(values_[2], right.values_[2]) && NumericF::isEqual(values_[3], right.values_[3]);
}

RGBAColor RGBAColor::operator-() const
{
	ocean_assert(isNormalized());

	return RGBAColor(1.0f - values_[0], 1.0f - values_[1], 1.0f - values_[2], values_[3]);
}

#if 0 // temporary disabled

RGBAColor RGBAColor::operator*(const float factor) const
{
	ocean_assert(isValid());
	ocean_assert(factor >= 0);

	return RGBAColor(values_[0] * factor, values_[1] * factor, values_[2] * factor, values_[3], true);
}

RGBAColor& RGBAColor::operator*=(const float factor)
{
	ocean_assert(isValid());
	ocean_assert(factor >= 0);

	values_[0] *= factor;
	values_[1] *= factor;
	values_[2] *= factor;

	ocean_assert(isValid());

	return *this;
}

#endif

}
