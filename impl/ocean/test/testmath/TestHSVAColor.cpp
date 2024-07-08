/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestHSVAColor.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/Interpolation.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/Random.h"
#include "ocean/math/RGBAColor.h"
#include "ocean/math/HSVAColor.h"

#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestHSVAColor::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   HSVAColor test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testWriteToMessenger() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testIsEqual(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAccessors(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConversionRGBA(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testInterpolate(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "HSVAColor test succeeded.";
	}
	else
	{
		Log::info() << "HSVAColor test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestHSVAColor, WriteToMessenger)
{
	EXPECT_TRUE(TestHSVAColor::testWriteToMessenger());
}

TEST(TestHSVAColor, IsEqual)
{
	EXPECT_TRUE(TestHSVAColor::testIsEqual(GTEST_TEST_DURATION));
}

TEST(TestHSVAColor, Accessors)
{
	EXPECT_TRUE(TestHSVAColor::testAccessors(GTEST_TEST_DURATION));
}

TEST(TestHSVAColor, ConversionRGBA)
{
	EXPECT_TRUE(TestHSVAColor::testConversionRGBA(GTEST_TEST_DURATION));
}

TEST(TestHSVAColor, Interpolate)
{
	EXPECT_TRUE(TestHSVAColor::testInterpolate(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestHSVAColor::testWriteToMessenger()
{
	Log::info() << "Write to messenger test:";

	// this is mainly a check whether the code does not compile or crash

	Log::info() << " ";

	Log::info() << HSVAColor(0.0f, 0.0f, 0.0f);
	Log::info() << "Color: " << HSVAColor(1.0f, 0.0f, 0.0f, 0.5f);
	Log::info() << HSVAColor() << " <- Color";

	Log::info() << " ";
	Log::info() << "Validation succeeded.";

	return true;
}

bool TestHSVAColor::testIsEqual(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "IsEqual test:";

	bool allSucceeded = true;

	Timestamp startTimestamp(true);

	do
	{
		const float hue = RandomF::scalar(0.0f, NumericF::pi2() - NumericF::weakEps());
		const float saturation = RandomF::scalar(0.0f, 1.0f);
		const float value = RandomF::scalar(0.0f, 1.0f);
		const float alpha = RandomF::scalar(0.0f, 1.0f);

		const HSVAColor color(hue, saturation, value, alpha);

		const float eps = RandomF::scalar(0.0f, 1.0f - NumericF::weakEps());

		{
			// hue

			const float otherHue = RandomF::scalar(0.0f, NumericF::pi2() - NumericF::weakEps());

			const HSVAColor otherColor(otherHue, saturation, value, alpha);

			const bool isEqual = NumericF::abs(hue - otherHue) <= eps;

			for (const bool includeAlpha : {false, true})
			{
				if (isEqual != color.isEqual(otherColor, eps, includeAlpha))
				{
					allSucceeded = false;
				}
			}
		}

		{
			// saturation

			const float otherSaturation = RandomF::scalar(0.0f, 1.0f);

			const HSVAColor otherColor(hue, otherSaturation, value, alpha);

			const bool isEqual = NumericF::abs(saturation - otherSaturation) <= eps;

			for (const bool includeAlpha : {false, true})
			{
				if (isEqual != color.isEqual(otherColor, eps, includeAlpha))
				{
					allSucceeded = false;
				}
			}
		}

		{
			// value

			const float otherValue = RandomF::scalar(0.0f, 1.0f);

			const HSVAColor otherColor(hue, saturation, otherValue, alpha);

			const bool isEqual = NumericF::abs(value - otherValue) <= eps;

			for (const bool includeAlpha : {false, true})
			{
				if (isEqual != color.isEqual(otherColor, eps, includeAlpha))
				{
					allSucceeded = false;
				}
			}
		}

		{
			// alpha

			const float otherAlpha = RandomF::scalar(0.0f, 1.0f);

			const HSVAColor otherColor(hue, saturation, value, otherAlpha);

			const bool isEqual = NumericF::abs(alpha - otherAlpha) <= eps;

			if (isEqual != color.isEqual(otherColor, eps, true /*includeAlpha*/))
			{
				allSucceeded = false;
			}

			if (!color.isEqual(otherColor, eps, false /*includeAlpha*/))
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestHSVAColor::testAccessors(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Accessors test:";

	bool allSucceeded = true;

	Timestamp startTimestamp(true);

	do
	{
		const float hue = RandomF::scalar(0.0f, NumericF::pi2() - NumericF::weakEps());
		const float saturation = RandomF::scalar(0.0f, 1.0f);
		const float value = RandomF::scalar(0.0f, 1.0f);

		{
			// no alpha

			const HSVAColor color(hue, saturation, value);

			const float* values = color();

			if (color[0] != hue || values[0] != hue)
			{
				allSucceeded = false;
			}

			if (color[1] != saturation || values[1] != saturation)
			{
				allSucceeded = false;
			}

			if (color[2] != value || values[2] != value)
			{
				allSucceeded = false;
			}

			if (color[3] != 1.0f || values[3] != 1.0f)
			{
				allSucceeded = false;
			}
		}

		{
			// with alpha

			const float alpha = RandomF::scalar(0.0f, 1.0f);

			const HSVAColor color(hue, saturation, value, alpha);

			const float* values = color();

			if (color[0] != hue || values[0] != hue)
			{
				allSucceeded = false;
			}

			if (color[1] != saturation || values[1] != saturation)
			{
				allSucceeded = false;
			}

			if (color[2] != value || values[2] != value)
			{
				allSucceeded = false;
			}

			if (color[3] != alpha || values[3] != alpha)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestHSVAColor::testConversionRGBA(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Conversion with HSVA test:";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	{
		// testing hard-coded values

		// black
		OCEAN_EXPECT_EQUAL(validation, RGBAColor(HSVAColor(0.0f, 0.0f, 0.0f)), RGBAColor(0.0f, 0.0f, 0.0f));

		// white
		OCEAN_EXPECT_EQUAL(validation, RGBAColor(HSVAColor(0.0f, 0.0f, 1.0f)), RGBAColor(1.0f, 1.0f, 1.0f));

		// red
		OCEAN_EXPECT_EQUAL(validation, RGBAColor(HSVAColor(0.0f, 1.0f, 1.0f)), RGBAColor(1.0f, 0.0f, 0.0f));

		// green
		OCEAN_EXPECT_EQUAL(validation, RGBAColor(HSVAColor(NumericF::deg2rad(120), 1.0f, 1.0f)), RGBAColor(0.0f, 1.0f, 0.0f));

		// blue
		OCEAN_EXPECT_EQUAL(validation, RGBAColor(HSVAColor(NumericF::deg2rad(240), 1.0f, 1.0f)), RGBAColor(0.0f, 0.0f, 1.0f));

		// yellow
		OCEAN_EXPECT_EQUAL(validation, RGBAColor(HSVAColor(NumericF::deg2rad(60), 1.0f, 1.0f)), RGBAColor(1.0f, 1.0f, 0.0f));

		// cyan
		OCEAN_EXPECT_EQUAL(validation, RGBAColor(HSVAColor(NumericF::deg2rad(180), 1.0f, 1.0f)), RGBAColor(0.0f, 1.0f, 1.0f));

		// magenta
		OCEAN_EXPECT_EQUAL(validation, RGBAColor(HSVAColor(NumericF::deg2rad(300), 1.0f, 1.0f)), RGBAColor(1.0f, 0.0f, 1.0f));
	}

	Timestamp startTimestamp(true);

	do
	{
		const float hue = RandomF::scalar(randomGenerator, 0.0f, NumericF::pi2() - NumericF::weakEps());
		const float saturation = RandomF::scalar(randomGenerator, 0.0f, 1.0f);
		const float value = RandomF::scalar(randomGenerator, 0.0f, 1.0f);
		const float alpha = RandomF::scalar(randomGenerator, 0.0f, 1.0f);

		const HSVAColor inputColor(hue, saturation, value, alpha);

		const RGBAColor rgbaColor(inputColor);

		const HSVAColor outputColor(rgbaColor);

		if (NumericF::isEqualEps(inputColor.value()))
		{
			// black

			OCEAN_EXPECT_EQUAL(validation, outputColor.hue(), 0.0f);
			OCEAN_EXPECT_EQUAL(validation, outputColor.saturation(), 0.0f);
			OCEAN_EXPECT_EQUAL(validation, outputColor.value(), 0.0f);
		}
		else
		{
			constexpr float threshold = 0.01f;

			const bool correctHueA = NumericF::angleIsEqual(inputColor.hue(), outputColor.hue(), threshold);
			const bool correctHueB = NumericF::isEqual(inputColor.saturation(), 0.0f, threshold) && NumericF::isEqual(outputColor.hue(), 0.0f, threshold); // special case for gray
			const bool correctHueC = NumericF::isWeakEqualEps(inputColor.value()) && NumericF::isEqual(outputColor.hue(), 0.0f, threshold); // special case almost black

			OCEAN_EXPECT_TRUE(validation, correctHueA || correctHueB || correctHueC);
			OCEAN_EXPECT_TRUE(validation, NumericF::isEqual(inputColor.saturation(), outputColor.saturation(), threshold));
			OCEAN_EXPECT_TRUE(validation, NumericF::isEqual(inputColor.value(), outputColor.value(), threshold));
		}

		OCEAN_EXPECT_EQUAL(validation, inputColor.alpha(), outputColor.alpha());
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestHSVAColor::testInterpolate(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Interpolate test:";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	Timestamp startTimestamp(true);

	do
	{
		const float hueA = RandomF::scalar(randomGenerator, 0.0f, NumericF::pi2() - NumericF::weakEps());
		const float saturationA = RandomF::scalar(randomGenerator, 0.0f, 1.0f);
		const float valueA = RandomF::scalar(randomGenerator, 0.0f, 1.0f);
		const float alphaA = RandomF::scalar(randomGenerator, 0.0f, 1.0f);

		const float hueB = RandomF::scalar(randomGenerator, 0.0f, NumericF::pi2() - NumericF::weakEps());
		const float saturationB = RandomF::scalar(randomGenerator, 0.0f, 1.0f);
		const float valueB = RandomF::scalar(randomGenerator, 0.0f, 1.0f);
		const float alphaB = RandomF::scalar(randomGenerator, 0.0f, 1.0f);

		const HSVAColor colorA(hueA, saturationA, valueA, alphaA);
		const HSVAColor colorB(hueB, saturationB, valueB, alphaB);

		const float factor = RandomF::scalar(randomGenerator, 0.0f, 1.0f);

		const HSVAColor interpolatedColor = colorA.interpolate(colorB, factor);

		const float expectedSaturation = Interpolation::linear(saturationA, saturationB, factor);
		const float expectedValue = Interpolation::linear(valueA, valueB, factor);
		const float expectedAlpha = Interpolation::linear(alphaA, alphaB, factor);

		OCEAN_EXPECT_EQUAL(validation, interpolatedColor.saturation(), expectedSaturation);
		OCEAN_EXPECT_EQUAL(validation, interpolatedColor.value(), expectedValue);
		OCEAN_EXPECT_EQUAL(validation, interpolatedColor.alpha(), expectedAlpha);

		if (hueA <= hueB)
		{
			if (hueB - hueA <= NumericF::pi())
			{
				// straight forward

				const float expectedHue = Interpolation::linear(hueA, hueB, factor);

				OCEAN_EXPECT_EQUAL(validation, interpolatedColor.hue(), expectedHue);
			}
			else
			{
				float expectedHue = Interpolation::linear(hueA + NumericF::pi2(), hueB, factor);
				expectedHue = NumericF::angleAdjustPositive(expectedHue);

				OCEAN_EXPECT_TRUE(validation, NumericF::angleIsWeakEqual(interpolatedColor.hue(), expectedHue));
			}
		}
		else
		{
			if (hueA - hueB <= NumericF::pi())
			{
				// straight forward

				const float expectedHue = Interpolation::linear(hueA, hueB, factor);

				OCEAN_EXPECT_EQUAL(validation, interpolatedColor.hue(), expectedHue);
			}
			else
			{
				float expectedHue = Interpolation::linear(hueA, hueB + NumericF::pi2(), factor);
				expectedHue = NumericF::angleAdjustPositive(expectedHue);

				OCEAN_EXPECT_TRUE(validation, NumericF::angleIsWeakEqual(interpolatedColor.hue(), expectedHue));
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
