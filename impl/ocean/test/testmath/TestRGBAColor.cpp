/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestRGBAColor.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/HSVAColor.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/Random.h"
#include "ocean/math/RGBAColor.h"

#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestRGBAColor::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   RGBAColor test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testWriteToMessenger() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConstructorColorTemperature(testDuration) && allSucceeded;

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

	allSucceeded = testConversionHSVA(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "RGBAColor test succeeded.";
	}
	else
	{
		Log::info() << "RGBAColor test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestRGBAColor, WriteToMessenger)
{
	EXPECT_TRUE(TestRGBAColor::testWriteToMessenger());
}

TEST(TestRGBAColor, ConstructorColorTemperature)
{
	EXPECT_TRUE(TestRGBAColor::testConstructorColorTemperature(GTEST_TEST_DURATION));
}

TEST(TestRGBAColor, IsEqual)
{
	EXPECT_TRUE(TestRGBAColor::testIsEqual(GTEST_TEST_DURATION));
}

TEST(TestRGBAColor, Accessors)
{
	EXPECT_TRUE(TestRGBAColor::testAccessors(GTEST_TEST_DURATION));
}

TEST(TestRGBAColor, ConversionHSVA)
{
	EXPECT_TRUE(TestRGBAColor::testConversionHSVA(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestRGBAColor::testWriteToMessenger()
{
	Log::info() << "Write to messenger test:";

	// this is mainly a check whether the code does not compile or crash

	Log::info() << " ";

	Log::info() << RGBAColor(0.0f, 0.0f, 0.0f);
	Log::info() << "Color: " << RGBAColor(1.0f, 0.0f, 0.0f, 0.5f);
	Log::info() << RGBAColor() << " <- Color";

	Log::info() << " ";
	Log::info() << "Validation succeeded.";

	return true;
}

bool TestRGBAColor::testConstructorColorTemperature(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Color temperature constructor test:";

	bool allSucceeded = true;

	const std::vector<std::array<float, 4>> testColors =
	{
		{1000.0f, 255.0f, 67.0f, 0.0f},
		{2000.0f, 255.0f, 136.0f, 14.0f},
		{2500.0f, 255.0f, 159.0f, 70.0f},
		{3000.0f, 255.0f, 177.0f, 109.0f},
		{5400.0f, 255.0f, 235.0f, 220.0f},
		{7000.0f, 242.0f, 242.0f, 255.0f},
		{9000.0f, 209.0f, 223.0f, 255.0f},
		{14000.0f, 184.0f, 207.0f, 255.0f},
		{40000.0f, 151.0f, 185.0f, 255.0f},
	};

	for (const std::array<float, 4>& testColor : testColors)
	{
		const RGBAColor rgbaColor(testColor[0]);

		if (NumericF::isNotEqual(rgbaColor.red() * 255.0f, testColor[1], 1.0f))
		{
			allSucceeded = false;
		}

		if (NumericF::isNotEqual(rgbaColor.green() * 255.0f, testColor[2], 1.0f))
		{
			allSucceeded = false;
		}

		if (NumericF::isNotEqual(rgbaColor.blue() * 255.0f, testColor[3], 1.0f))
		{
			allSucceeded = false;
		}
	}

	Timestamp startTimestamp(true);

	do
	{
		const float colorTemperature = RandomF::scalar(1000.0f, 40000.f);

		const RGBAColor rgbaColor(colorTemperature);

		if (colorTemperature < 6600.0f && rgbaColor.red() != 1.0f)
		{
			allSucceeded = false;
		}

		if (colorTemperature < 1900.0f && rgbaColor.blue() != 0.0f)
		{
			allSucceeded = false;
		}

		if (colorTemperature > 6600.0f && rgbaColor.blue() != 1.0f)
		{
			allSucceeded = false;
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

bool TestRGBAColor::testIsEqual(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "IsEqual test:";

	bool allSucceeded = true;

	Timestamp startTimestamp(true);

	do
	{
		const float red = RandomF::scalar(0.0f, 1.0f);
		const float green = RandomF::scalar(0.0f, 1.0f);
		const float blue = RandomF::scalar(0.0f, 1.0f);
		const float alpha = RandomF::scalar(0.0f, 1.0f);

		const RGBAColor color(red, green, blue, alpha);

		const float eps = RandomF::scalar(0.0f, 1.0f - NumericF::weakEps());

		{
			// red

			const float otherRed = RandomF::scalar(0.0f, 1.0f);

			const RGBAColor otherColor(otherRed, green, blue, alpha);

			const bool isEqual = NumericF::abs(red - otherRed) <= eps;

			for (const bool includeAlpha : {false, true})
			{
				if (isEqual != color.isEqual(otherColor, eps, includeAlpha))
				{
					allSucceeded = false;
				}
			}
		}

		{
			// green

			const float otherGreen = RandomF::scalar(0.0f, 1.0f);

			const RGBAColor otherColor(red, otherGreen, blue, alpha);

			const bool isEqual = NumericF::abs(green - otherGreen) <= eps;

			for (const bool includeAlpha : {false, true})
			{
				if (isEqual != color.isEqual(otherColor, eps, includeAlpha))
				{
					allSucceeded = false;
				}
			}
		}

		{
			// blue

			const float otherBlue = RandomF::scalar(0.0f, 1.0f);

			const RGBAColor otherColor(red, green, otherBlue, alpha);

			const bool isEqual = NumericF::abs(blue - otherBlue) <= eps;

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

			const RGBAColor otherColor(red, green, blue, otherAlpha);

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

bool TestRGBAColor::testAccessors(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Accessors test:";

	bool allSucceeded = true;

	Timestamp startTimestamp(true);

	do
	{
		const float red = RandomF::scalar(0.0f, 1.0f);
		const float green = RandomF::scalar(0.0f, 1.0f);
		const float blue = RandomF::scalar(0.0f, 1.0f);

		{
			// no alpha

			const RGBAColor color(red, green, blue);

			const float* values = color();

			if (color[0] != red || values[0] != red)
			{
				allSucceeded = false;
			}

			if (color[1] != green || values[1] != green)
			{
				allSucceeded = false;
			}

			if (color[2] != blue || values[2] != blue)
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

			const RGBAColor color(red, green, blue, alpha);

			const float* values = color();

			if (color[0] != red || values[0] != red)
			{
				allSucceeded = false;
			}

			if (color[1] != green || values[1] != green)
			{
				allSucceeded = false;
			}

			if (color[2] != blue || values[2] != blue)
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

bool TestRGBAColor::testConversionHSVA(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Conversion with HSVA test:";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	{
		// testing hard-coded values

		// black
		OCEAN_EXPECT_EQUAL(validation, HSVAColor(RGBAColor(0.0f, 0.0f, 0.0f)), HSVAColor(0.0f, 0.0f, 0.0f));

		// white
		OCEAN_EXPECT_EQUAL(validation, HSVAColor(RGBAColor(1.0f, 1.0f, 1.0f)), HSVAColor(0.0f, 0.0f, 1.0f));

		// red
		OCEAN_EXPECT_EQUAL(validation, HSVAColor(RGBAColor(1.0f, 0.0f, 0.0f)), HSVAColor(0.0f, 1.0f, 1.0f));

		// green
		OCEAN_EXPECT_EQUAL(validation, HSVAColor(RGBAColor(0.0f, 1.0f, 0.0f)), HSVAColor(NumericF::deg2rad(120), 1.0f, 1.0f));

		// blue
		OCEAN_EXPECT_EQUAL(validation, HSVAColor(RGBAColor(0.0f, 0.0f, 1.0f)), HSVAColor(NumericF::deg2rad(240), 1.0f, 1.0f));

		// yellow
		OCEAN_EXPECT_EQUAL(validation, HSVAColor(RGBAColor(1.0f, 1.0f, 0.0f)), HSVAColor(NumericF::deg2rad(60), 1.0f, 1.0f));

		// cyan
		OCEAN_EXPECT_EQUAL(validation, HSVAColor(RGBAColor(0.0f, 1.0f, 1.0f)), HSVAColor(NumericF::deg2rad(180), 1.0f, 1.0f));

		// magenta
		OCEAN_EXPECT_EQUAL(validation, HSVAColor(RGBAColor(1.0f, 0.0f, 1.0f)), HSVAColor(NumericF::deg2rad(300), 1.0f, 1.0f));
	}

	Timestamp startTimestamp(true);

	do
	{
		const float red = RandomF::scalar(randomGenerator, 0.0f, 1.0f);
		const float green = RandomF::scalar(randomGenerator, 0.0f, 1.0f);
		const float blue = RandomF::scalar(randomGenerator, 0.0f, 1.0f);
		const float alpha = RandomF::scalar(randomGenerator, 0.0f, 1.0f);

		const RGBAColor inputColor(red, green, blue, alpha);

		const HSVAColor hsvaColor(inputColor);

		const RGBAColor outputColor(hsvaColor);

		constexpr float threshold = 0.01f;

		OCEAN_EXPECT_TRUE(validation, NumericF::isEqual(inputColor.red(), outputColor.red(), threshold));
		OCEAN_EXPECT_TRUE(validation, NumericF::isEqual(inputColor.green(), outputColor.green(), threshold));
		OCEAN_EXPECT_TRUE(validation, NumericF::isEqual(inputColor.blue(), outputColor.blue(), threshold));

		OCEAN_EXPECT_EQUAL(validation, inputColor.alpha(), outputColor.alpha());
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
