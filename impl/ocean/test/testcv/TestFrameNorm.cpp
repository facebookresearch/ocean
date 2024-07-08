/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameNorm.h"

#include "ocean/base/DataType.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameNorm.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameNorm::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Frame Norm test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testNormL2<float>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNormL2<double>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Frame Norm test succeeded.";
	}
	else
	{
		Log::info() << "Frame Norm test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameNorm, NormL2Float)
{
	EXPECT_TRUE(TestFrameNorm::testNormL2<float>(GTEST_TEST_DURATION));
}

TEST(TestFrameNorm, NormL2Double)
{
	EXPECT_TRUE(TestFrameNorm::testNormL2<double>(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

template <typename T>
bool TestFrameNorm::testNormL2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing L2 norm for " << TypeNamer::name<T>() << ":";

	uint64_t iterations = 0u;
	uint64_t validIterations = 0u;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 500u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 500u);

		const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 50u);

		Frame frame(FrameType(width, height, FrameType::genericPixelFormat<T, 1u>(), FrameType::ORIGIN_UPPER_LEFT), paddingElements);
		CV::CVUtilities::randomizeFrame(frame, false, &randomGenerator);

		const double dataNorm = CV::FrameNorm::normL2<T, double>(frame.constdata<T>(), width);
		const double testDataNorm = calculateNormL2<T>(frame.constdata<T>(), width, 1u, paddingElements);

		if (NumericD::isEqual(dataNorm, testDataNorm, 0.1))
		{
			validIterations++;
		}

		const double frameNorm = CV::FrameNorm::normL2<T, double>(frame.constdata<T>(), width, height, paddingElements);
		const double testFrameNorm = calculateNormL2<T>(frame.constdata<T>(), width, height, paddingElements);

		if (NumericD::isEqual(frameNorm, testFrameNorm, 0.1))
		{
			validIterations++;
		}

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

template <typename T>
double TestFrameNorm::calculateNormL2(const T* frame, const unsigned int width, const unsigned int height, const unsigned int horizontalPadding)
{
	ocean_assert(frame != nullptr);
	ocean_assert(width != 0u && height != 0u);

	double result = 0.0;

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			result += double(frame[x]) * double(frame[x]);
		}

		frame += width + horizontalPadding;
	}

	return NumericD::sqrt(result);
}

}

}

}
