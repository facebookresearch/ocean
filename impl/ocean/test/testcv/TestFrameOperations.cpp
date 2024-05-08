/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameOperations.h"

#include "ocean/base/DataType.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameOperations.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameOperations::test(const double testDuration, Ocean::Worker &worker)
{
	ocean_assert(testDuration > 0.0);

	bool result = true;

	Log::info() << "---   Frame operations test:   ---";
	Log::info() << " ";

	result = testSubtraction<uint8_t>(1920u, 1080u, 1u, testDuration, worker) && result;
	Log::info() << " ";
	result = testSubtraction<uint8_t>(1920u, 1080u, 2u, testDuration, worker) && result;
	Log::info() << " ";
	result = testSubtraction<uint8_t>(1920u, 1080u, 3u, testDuration, worker) && result;
	Log::info() << " ";
	result = testSubtraction<uint8_t>(1920u, 1080u, 4u, testDuration, worker) && result;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	result = testSubtraction<float>(1920u, 1080u, 1u, testDuration, worker) && result;
	Log::info() << " ";
	result = testSubtraction<float>(1920u, 1080u, 2u, testDuration, worker) && result;
	Log::info() << " ";
	result = testSubtraction<float>(1920u, 1080u, 3u, testDuration, worker) && result;
	Log::info() << " ";
	result = testSubtraction<float>(1920u, 1080u, 4u, testDuration, worker) && result;

	Log::info() << " ";

	if (result)
	{
		Log::info() << "Frame operations test succeeded.";
	}
	else
	{
		Log::info() << "Frame operations test FAILED!";
	}

	return result;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameOperations, Subtraction_1920x1080_1Channels_UnsignedChar)
{
	Worker worker;
	EXPECT_TRUE((TestFrameOperations::testSubtraction<uint8_t>(1920u, 1080u, 1u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameOperations, Subtraction_1920x1080_2Channels_UnsignedChar)
{
	Worker worker;
	EXPECT_TRUE((TestFrameOperations::testSubtraction<uint8_t>(1920u, 1080u, 2u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameOperations, Subtraction_1920x1080_3Channels_UnsignedChar)
{
	Worker worker;
	EXPECT_TRUE((TestFrameOperations::testSubtraction<uint8_t>(1920u, 1080u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameOperations, Subtraction_1920x1080_4Channels_UnsignedChar)
{
	Worker worker;
	EXPECT_TRUE((TestFrameOperations::testSubtraction<uint8_t>(1920u, 1080u, 4u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameOperations, Subtraction_1920x1080_1Channels_Float)
{
	Worker worker;
	EXPECT_TRUE((TestFrameOperations::testSubtraction<float>(1920u, 1080u, 1u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameOperations, Subtraction_1920x1080_2Channels_Float)
{
	Worker worker;
	EXPECT_TRUE((TestFrameOperations::testSubtraction<float>(1920u, 1080u, 2u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameOperations, Subtraction_1920x1080_3Channels_Float)
{
	Worker worker;
	EXPECT_TRUE((TestFrameOperations::testSubtraction<float>(1920u, 1080u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameOperations, Subtraction_1920x1080_4Channels_Float)
{
	Worker worker;
	EXPECT_TRUE((TestFrameOperations::testSubtraction<float>(1920u, 1080u, 4u, GTEST_TEST_DURATION, worker)));
}

#endif // OCEAN_USE_GTEST

template <typename T>
bool TestFrameOperations::testSubtraction(const unsigned int performanceWidth, const unsigned int performanceHeight, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(performanceWidth != 0u && performanceHeight != 0u);
	ocean_assert(channels >= 1u && channels <= 4u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Frame subtraction test for an " << performanceWidth << "x" << performanceHeight << " image with " << channels << " channels (" << TypeNamer::name<T>() << "):";

	bool allSucceeded = true;

	const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<T>(channels);

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	RandomGenerator randomGenerator;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (const bool performanceIteration : {true, false})
	{
		const unsigned int width = performanceIteration ? performanceWidth : RandomI::random(randomGenerator, 1u, 2048u);
		const unsigned int height = performanceIteration ? performanceHeight : RandomI::random(randomGenerator, 1u, 2048u);

		const unsigned int source0PaddingElements = RandomI::random(randomGenerator, 1u, 256u) * RandomI::random(randomGenerator, 1u);
		const unsigned int source1PaddingElements = RandomI::random(randomGenerator, 1u, 256u) * RandomI::random(randomGenerator, 1u);
		const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 1u, 256u) * RandomI::random(randomGenerator, 1u);

		Frame source0(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), source0PaddingElements);
		Frame source1(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), source1PaddingElements);

		Frame target(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), targetPaddingElements);

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			Timestamp startTimestamp(true);

			do
			{
				CV::CVUtilities::randomizeFrame(source0, /* skipPaddingArea */ false, &randomGenerator);
				CV::CVUtilities::randomizeFrame(source1, /* skipPaddingArea */ false, &randomGenerator);

				CV::CVUtilities::randomizeFrame(target, /* skipPaddingArea */ false, &randomGenerator);

				const Frame targetClone(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				performance.startIf(performanceIteration);
				CV::FrameOperations::subtract(source0, source1, target, useWorker);
				performance.stopIf(performanceIteration);

				if (!validateSubtraction<T>(source0, source1, target))
				{
					allSucceeded = false;
				}

				if (!CV::CVUtilities::isPaddingMemoryIdentical(target, targetClone))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}
	}

	Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms";
		Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
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

template <typename T>
bool TestFrameOperations::validateSubtraction(const Frame& source0, const Frame& source1, const Frame& target)
{
	ocean_assert(source0.isValid() && source0.numberPlanes() == 1u && FrameType::areFrameTypesCompatible(source0, source1, /* allowDifferentPixelOrigins */ false) && FrameType::areFrameTypesCompatible(source0, target, /* allowDifferentPixelOrigins */ false));

	const unsigned int width = source0.width();
	const unsigned int height = source0.height();

	const unsigned int channels = source0.channels();

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			const T source0Pixel = *source0.constpixel<T>(x, y);
			const T source1Pixel = *source1.constpixel<T>(x, y);
			const T targetPixel = *target.constpixel<T>(x, y);

			for (unsigned int n = 0u; n < channels; ++n)
			{
				if (NumericT<T>::isNotEqualEps((source0Pixel - source1Pixel) - targetPixel))
				{
					return false;
				}
			}
		}
	}

	return true;
}

} // namespace TestCV

} // namespace Test

} // namespace Ocean
