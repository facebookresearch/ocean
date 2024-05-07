// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/test/testmath/TestHSVAColor.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/Numeric.h"
#include "ocean/math/Random.h"
#include "ocean/math/HSVAColor.h"

#include <array>

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
		const float hue = RandomF::scalar(0.0f, 1.0f);
		const float saturation = RandomF::scalar(0.0f, 1.0f);
		const float value = RandomF::scalar(0.0f, 1.0f);
		const float alpha = RandomF::scalar(0.0f, 1.0f);

		const HSVAColor color(hue, saturation, value, alpha);

		const float eps = RandomF::scalar(0.0f, 1.0f - NumericF::weakEps());

		{
			// hue

			const float otherHue = RandomF::scalar(0.0f, 1.0f);

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
		const float hue = RandomF::scalar(0.0f, 1.0f);
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

}

}

}
