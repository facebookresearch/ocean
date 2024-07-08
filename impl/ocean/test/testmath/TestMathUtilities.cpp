/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestMathUtilities.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Random.h"
#include "ocean/math/MathUtilities.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestMathUtilities::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Utilities test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testEncodeFloatToUint8(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Utlities test succeeded.";
	}
	else
	{
		Log::info() << "Utilities test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestMathUtilities, EncodeFloatToUint8)
{
	EXPECT_TRUE(TestMathUtilities::testEncodeFloatToUint8(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestMathUtilities::testEncodeFloatToUint8(const double testDuration)
{
	Log::info() << "Encode float to uin8_t values test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	float maxError = 0.0f;
	double sumError = 0.0;
	size_t iterations = 0;

	do
	{
		const float value = float(double(RandomI::random32(randomGenerator)) / double(0xFFFFFFFFu));

		uint8_t decomposedValues[4];
		MathUtilities::encodeFloatToUint8(value, decomposedValues);

		const float decodedValue = MathUtilities::decodeFloatFromUint8(decomposedValues);

		const float error = decodedValue - value;

		maxError = std::max(maxError, NumericF::abs(error));

		sumError += double(NumericF::abs(error));
		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Maximal error: " << String::toAString(maxError, 10u) << ", average error: " << String::toAString(sumError / double(iterations), 10u);

	if (maxError > 0.0001f)
	{
		allSucceeded = false;
	}

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
