/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameMean.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameMean.h"

#include <limits>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameMean::test(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Frame Mean test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testMeanValue(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " - ";
	Log::info() << " ";

	allSucceeded = testAddToFrameIndividually(1920u, 1080u, 1u, testDuration, worker);

	Log::info() << " ";

	allSucceeded = testAddToFrameIndividually(1920u, 1080u, 2u, testDuration, worker);

	Log::info() << " ";

	allSucceeded = testAddToFrameIndividually(1920u, 1080u, 3u, testDuration, worker);

	Log::info() << " ";

	allSucceeded = testAddToFrameIndividually(1920u, 1080u, 4u, testDuration, worker);

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Frame Mean test succeeded.";
	}
	else
	{
		Log::info() << "Frame Mean test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameMean, AddToFrameIndividually_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameMean::testAddToFrameIndividually(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, GTEST_TEST_DURATION, worker));
}
TEST(TestFrameMean, AddToFrameIndividually_2Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameMean::testAddToFrameIndividually(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 2u, GTEST_TEST_DURATION, worker));
}
TEST(TestFrameMean, AddToFrameIndividually_3Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameMean::testAddToFrameIndividually(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, GTEST_TEST_DURATION, worker));
}
TEST(TestFrameMean, AddToFrameIndividually_4Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameMean::testAddToFrameIndividually(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 4u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameMean, MeanValue_1920x1080_uint8_uint8_uint32_1Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameMean::testMeanValue<uint8_t, uint8_t, uint32_t>(1920u, 1080u, 1u, GTEST_TEST_DURATION, worker)));
}
TEST(TestFrameMean, MeanValue_1920x1080_uint8_int32_int32_2Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameMean::testMeanValue<uint8_t, int32_t, int32_t>(1920u, 1080u, 2u, GTEST_TEST_DURATION, worker)));
}
TEST(TestFrameMean, MeanValue_1920x1080_uint8_int64_int64_3Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameMean::testMeanValue<uint8_t, int64_t, int64_t>(1920u, 1080u, 3u, GTEST_TEST_DURATION, worker)));
}
TEST(TestFrameMean, MeanValue_1920x1080_uint8_float_double_4Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameMean::testMeanValue<uint8_t, float, double>(1920u, 1080u, 4u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameMean, MeanValue_1920x1080_int16_int16_int32_1Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameMean::testMeanValue<int16_t, int16_t, int32_t>(1920u, 1080u, 1u, GTEST_TEST_DURATION, worker)));
}
TEST(TestFrameMean, MeanValue_1920x1080_int16_int32_int32_2Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameMean::testMeanValue<int16_t, int32_t, int32_t>(1920u, 1080u, 2u, GTEST_TEST_DURATION, worker)));
}
TEST(TestFrameMean, MeanValue_1920x1080_int16_int64_int64_3Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameMean::testMeanValue<int16_t, int64_t, int64_t>(1920u, 1080u, 3u, GTEST_TEST_DURATION, worker)));
}
TEST(TestFrameMean, MeanValue_1920x1080_int16_double_double_4Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameMean::testMeanValue<int16_t, double, double>(1920u, 1080u, 4u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameMean, MeanValue_1920x1080_int32_int32_int64_1Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameMean::testMeanValue<int32_t, int32_t, int64_t>(1920u, 1080u, 1u, GTEST_TEST_DURATION, worker)));
}
TEST(TestFrameMean, MeanValue_1920x1080_int32_int64_int64_2Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameMean::testMeanValue<int32_t, int64_t, int64_t>(1920u, 1080u, 2u, GTEST_TEST_DURATION, worker)));
}
TEST(TestFrameMean, MeanValue_1920x1080_int32_int64_int64_3Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameMean::testMeanValue<int32_t, int64_t, int64_t>(1920u, 1080u, 3u, GTEST_TEST_DURATION, worker)));
}
TEST(TestFrameMean, MeanValue_1920x1080_int32_float_double_4Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameMean::testMeanValue<int32_t, float, double>(1920u, 1080u, 4u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameMean, MeanValue_1920x1080_float_float_double_1Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameMean::testMeanValue<float, float, double>(1920u, 1080u, 1u, GTEST_TEST_DURATION, worker)));
}
TEST(TestFrameMean, MeanValue_1920x1080_float_double_double_2Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameMean::testMeanValue<float, double, double>(1920u, 1080u, 2u, GTEST_TEST_DURATION, worker)));
}
TEST(TestFrameMean, MeanValue_1920x1080_double_float_double_3Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameMean::testMeanValue<double, float, double>(1920u, 1080u, 3u, GTEST_TEST_DURATION, worker)));
}
TEST(TestFrameMean, MeanValue_1920x1080_double_double_double_4Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameMean::testMeanValue<double, double, double>(1920u, 1080u, 4u, GTEST_TEST_DURATION, worker)));
}

#endif // OCEAN_USE_GTEST

bool TestFrameMean::testAddToFrameIndividually(const unsigned int performanceWidth, const unsigned int performanceHeight, const unsigned numberChannels, const double testDuration, Worker& worker)
{
	ocean_assert(performanceWidth != 0u && performanceHeight != 0u && numberChannels != 0u && numberChannels <= 4u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing Adding individual pixel values of a given source frame to a testTarget frame with " << numberChannels << "-channel images of size " << performanceWidth << " x " << performanceHeight << " pixels:";

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (const bool performanceIteration : {true, false})
	{
		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			const Timestamp startTimestamp(true);

			do
			{
				const unsigned int width = performanceIteration ? performanceWidth : RandomI::random(randomGenerator, 1u, 2048u);
				const unsigned int height = performanceIteration ? performanceHeight : RandomI::random(randomGenerator, 1u, 2048u);

				const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 1u, 256u) * RandomI::random(randomGenerator, 1u);
				const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 1u, 256u) * RandomI::random(randomGenerator, 1u);
				const unsigned int maskPaddingElements = RandomI::random(randomGenerator, 1u, 256u) * RandomI::random(randomGenerator, 1u);
				const unsigned int denominatorsPaddingElements = RandomI::random(randomGenerator, 1u, 256u) * RandomI::random(randomGenerator, 1u);

				const uint8_t maskValue = uint8_t(RandomI::random(randomGenerator, 255u));
				const uint8_t nonMaskValue = 0xFFu - maskValue;

				Frame source(FrameType(width, height, FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8>(numberChannels), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
				Frame mask(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), maskPaddingElements);

				Frame target(FrameType(width, height, FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_32>(numberChannels), FrameType::ORIGIN_UPPER_LEFT), targetPaddingElements);
				Frame denominators(FrameType(width, height, FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_32>(1u), FrameType::ORIGIN_UPPER_LEFT), denominatorsPaddingElements);

				CV::CVUtilities::randomizeFrame(source, false, &randomGenerator);
				CV::CVUtilities::randomizeFrame(mask, false, &randomGenerator);

				CV::CVUtilities::randomizeFrame(target, false, &randomGenerator);
				CV::CVUtilities::randomizeFrame(denominators, false, &randomGenerator);

				// The tested function adds to the current, so resetting everything to 0.
				target.setValue(0u);
				denominators.setValue(0u);

				const Frame clonedTarget(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
				const Frame clonedDenominators(denominators, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				performance.startIf(performanceIteration);
				allSucceeded = CV::FrameMean::addToFrameIndividually(source, mask, target, denominators, nonMaskValue, useWorker) && allSucceeded;
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(target, clonedTarget) || !CV::CVUtilities::isPaddingMemoryIdentical(denominators, clonedDenominators))
				{
					Log::error() << "Padding area has been changed - potential memory access violation. Aborting immediately!";
					ocean_assert(false);

					return false;
				}

				if (!validateAddToFrameIndividually(source, mask, target, denominators, nonMaskValue))
				{
					allSucceeded = false;
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}
	}

	Log::info() << "Single-core performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multi-core performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms";
		Log::info() << "Multi-core boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
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
bool TestFrameMean::testMeanValue(const double testDuration, Worker& worker)
{
	Log::info() << "Mean value test:";
	Log::info() << " ";

	constexpr unsigned int width = 1920u;
	constexpr unsigned int height = 1080u;

	bool allSucceeded = true;

	allSucceeded = testMeanValue<uint8_t, uint8_t, uint32_t>(width, height, 1u, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testMeanValue<uint8_t, int32_t, int32_t>(width, height, 2u, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testMeanValue<uint8_t, int64_t, int64_t>(width, height, 3u, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testMeanValue<uint8_t, float, double>(width, height, 4u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testMeanValue<int16_t, int16_t, int32_t>(width, height, 1u, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testMeanValue<int16_t, int32_t, int32_t>(width, height, 2u, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testMeanValue<int16_t, int64_t, int64_t>(width, height, 3u, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testMeanValue<int16_t, double, double>(width, height, 4u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testMeanValue<int32_t, int32_t, int64_t>(width, height, 1u, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testMeanValue<int32_t, int64_t, int64_t>(width, height, 2u, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testMeanValue<int32_t, int64_t, int64_t>(width, height, 3u, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testMeanValue<int32_t, float, double>(width, height, 4u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testMeanValue<float, float, double>(width, height, 1u, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testMeanValue<float, double, double>(width, height, 2u, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testMeanValue<double, float, double>(width, height, 3u, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testMeanValue<double, double, double>(width, height, 4u, testDuration, worker) && allSucceeded;

	if (allSucceeded)
	{
		Log::info() << "Mean value test succeeded.";
	}
	else
	{
		Log::info() << "Mean value test FAILED!";
	}

	return allSucceeded;
}

template <typename T, typename TMean, typename TIntermediate>
bool TestFrameMean::testMeanValue(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(width > 0u && height > 0u && channels > 0u);

	Log::info() << "... for '" << TypeNamer::name<T>() << "' -> '" << TypeNamer::name<TMean>() << "' with " << channels << " channels:";

	bool allSucceeded = true;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		unsigned int iterations = 0u;

		Timestamp startTimestamp(true);

		do
		{
			const bool performanceIteration = iterations % 2u == 0u;

			const unsigned int testWidth = performanceIteration ? width : RandomI::random(1u, width);
			const unsigned int testHeight = performanceIteration ? height : RandomI::random(1u, height);

			const unsigned int paddingElements = RandomI::random(0u, 100u);

			Frame frame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<T>(channels), FrameType::ORIGIN_UPPER_LEFT), paddingElements);
			CV::CVUtilities::randomizeFrame(frame, false);

			std::vector<TMean> meanValues(channels, TMean(0));

			if (performanceIteration)
			{
				performance.start();
			}

			switch (channels)
			{
				case 1u:
					CV::FrameMean::meanValue<T, TMean, 1u>(frame.constdata<T>(), frame.width(), frame.height(), meanValues.data(), frame.paddingElements(), useWorker);
					break;

				case 2u:
					CV::FrameMean::meanValue<T, TMean, 2u>(frame.constdata<T>(), frame.width(), frame.height(), meanValues.data(), frame.paddingElements(), useWorker);
					break;

				case 3u:
					CV::FrameMean::meanValue<T, TMean, 3u>(frame.constdata<T>(), frame.width(), frame.height(), meanValues.data(), frame.paddingElements(), useWorker);
					break;

				case 4u:
					CV::FrameMean::meanValue<T, TMean, 4u>(frame.constdata<T>(), frame.width(), frame.height(), meanValues.data(), frame.paddingElements(), useWorker);
					break;
			}

			if (performanceIteration)
			{
				performance.stop();
			}

			if (!validateMeanValue<T, TMean, TIntermediate>(frame.constdata<T>(), frame.width(), frame.height(), channels, meanValues.data(), frame.paddingElements()))
			{
				allSucceeded = false;
			}

			++iterations;
		}
		while (iterations < 2u || startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Singlecore performance: Best: " << performanceSinglecore.bestMseconds() << "ms, worst: " << performanceSinglecore.worstMseconds() << "ms, average: " << performanceSinglecore.averageMseconds() << "ms, median: " << performanceSinglecore.medianMseconds() << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore performance: Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms, median: " << performanceMulticore.medianMseconds() << "ms";
		Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x, average: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 1u) << "x";
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

bool TestFrameMean::validateAddToFrameIndividually(const Frame& source, const Frame& mask, const Frame& testTarget, const Frame& testDenominators, const uint8_t nonMaskValue)
{
	ocean_assert(source.isValid() && mask.isValid() && testTarget.isValid() && testDenominators.isValid());
	ocean_assert(source.numberPlanes() == 1u && source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && source.channels() <= 4u);
	ocean_assert(FrameType::areFrameTypesCompatible(FrameType(source, FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_32>(source.channels())),  testTarget, false));
	ocean_assert(FrameType::areFrameTypesCompatible(FrameType(source, FrameType::FORMAT_Y32), testDenominators, false));

	for (unsigned int y = 0u; y < source.height(); ++y)
	{
		for (unsigned int x = 0u; x < source.width(); ++x)
		{
			const uint8_t* sourcePixel = source.constpixel<uint8_t>(x, y);
			const uint8_t* maskPixel = mask.constpixel<uint8_t>(x, y);

			const uint32_t* testTargetPixel = testTarget.constpixel<uint32_t>(x, y);
			const uint32_t* testDenominatorPixel = testDenominators.constpixel<uint32_t>(x, y);

			if (*maskPixel != nonMaskValue)
			{
				for (unsigned int c = 0u; c < source.channels(); ++c)
				{
					if (testTargetPixel[c] != sourcePixel[c])
					{
						return false;
					}
				}

				if (*testDenominatorPixel != 1u)
				{
					return false;
				}
			}
			else
			{
				for (unsigned int c = 0u; c < source.channels(); ++c)
				{
					if (testTargetPixel[c] != 0u)
					{
						return false;
					}
				}

				if (*testDenominatorPixel != 0u)
				{
					return false;
				}
			}
		}
	}

	return true;
}

template <typename T, typename TMean, typename TIntermediate>
bool TestFrameMean::validateMeanValue(const T* frame, const unsigned int width, const unsigned int height, const unsigned int channels, const TMean* testMeanValues, const unsigned int framePaddingElements)
{
	ocean_assert(frame != nullptr && testMeanValues != nullptr);
	ocean_assert(width > 0u && height > 0u && channels > 0u);

	std::vector<TIntermediate> sumValues(channels, TIntermediate(0));

	const unsigned int frameStrideElements = width * channels + framePaddingElements;

	for (unsigned int y = 0u; y < height; ++y)
	{
		const T* const row = frame + y * frameStrideElements;

		for (unsigned int x = 0u; x < width; ++x)
		{
			const T* const pixel = row + x * channels;

			for (unsigned int c = 0u; c < channels; ++c)
			{
				ocean_assert(double(sumValues[c]) + double(pixel[c]) <= NumericD::maxValue());
				ocean_assert(double(sumValues[c]) + double(pixel[c]) >= NumericD::minValue());

				sumValues[c] += TIntermediate(pixel[c]);
			}
		}
	}

	const unsigned int pixels = width * height;

	for (unsigned int c = 0u; c < channels; ++c)
	{
		const TMean value = TMean(meanValue<TIntermediate>(sumValues[c], size_t(pixels)));

		if (NumericT<TMean>::isNotEqual(value, testMeanValues[c], TMean(0.1)))
		{
			return false;
		}
	}

	return true;
}

}

}

}
