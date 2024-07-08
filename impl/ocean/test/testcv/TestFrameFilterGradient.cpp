/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameFilterGradient.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterGradient.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameFilterGradient::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 3u && height >= 3u && testDuration > 0.0);

	Log::info() << "---   Gradient filter test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testHorizontalVertical(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHorizontalVerticalSubFrame(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHorizontalVerticalMagnitudeSquared(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFilterHorizontal1x2LinedIntegralImage<uint8_t, int32_t>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFilterVertical2x1LinedIntegralImage<uint8_t, int32_t>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Gradient filter test succeeded.";
	}
	else
	{
		Log::info() << "Gradient filter test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameFilterGradient, HorizontalVertical_uint8_int8_1channel_factor1)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterGradient::testHorizontalVertical<uint8_t, int8_t, true>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, 1u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterGradient, HorizontalVertical_uint16_int16_1channel_factor1)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterGradient::testHorizontalVertical<uint16_t, int16_t, true>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, 1u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterGradient, HorizontalVertical_uint32_int32_1channel_factor1)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterGradient::testHorizontalVertical<uint32_t, int32_t, true>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, 1u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterGradient, HorizontalVertical_uint8_float32_4channel_factor2)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterGradient::testHorizontalVertical<uint8_t, float, true>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 4u, double(2), GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameFilterGradient, HorizontalVerticalSubFrame_uint8_int8_1channel_factor1)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterGradient::testHorizontalVerticalSubFrame<uint8_t, int8_t, true>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, 1u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterGradient, HorizontalVerticalSubFrame_uint16_int16_1channel_factor1)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterGradient::testHorizontalVerticalSubFrame<uint16_t, int16_t, true>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, 1u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterGradient, HorizontalVerticalSubFrame_uint32_int32_1channel_factor1)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterGradient::testHorizontalVerticalSubFrame<uint32_t, int32_t, true>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, 1u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterGradient, HorizontalVerticalSubFrame_uint8_float32_4channel_factor2)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterGradient::testHorizontalVerticalSubFrame<uint8_t, float, true>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 4u, double(2), GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameFilterGradient, FilterHorizontal1x2LinedIntegralImage_uint8_int32)
{
	EXPECT_TRUE((TestFrameFilterGradient::testFilterHorizontal1x2LinedIntegralImage<uint8_t, int32_t>(GTEST_TEST_DURATION)));
}

TEST(TestFrameFilterGradient, FilterVertical2x1LinedIntegralImage_uint8_int32)
{
	EXPECT_TRUE((TestFrameFilterGradient::testFilterVertical2x1LinedIntegralImage<uint8_t, int32_t>(GTEST_TEST_DURATION)));
}

#endif // OCEAN_USE_GTEST

bool TestFrameFilterGradient::testHorizontalVertical(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	bool allSucceeded = true;

	for (unsigned int channels = 1u; channels <= 4u; ++channels)
	{
		Log::info().newLine(channels != 1u);
		Log::info().newLine(channels != 1u);

		for (unsigned int factor = 1u; factor <= 2u; ++factor)
		{
			if (factor == 1u)
			{
				// the target data type does not allow a factor of 2

				allSucceeded = testHorizontalVertical<uint8_t, int8_t, true>(width, height, channels, int8_t(factor), testDuration, worker) && allSucceeded;
				Log::info() << " ";
				allSucceeded = testHorizontalVertical<uint16_t, int16_t, true>(width, height, channels, int16_t(factor), testDuration, worker) && allSucceeded;
				Log::info() << " ";
				allSucceeded = testHorizontalVertical<uint32_t, int32_t, true>(width, height, channels, int32_t(factor), testDuration, worker) && allSucceeded;
			}

			Log::info() << " ";
			allSucceeded = testHorizontalVertical<uint8_t, float, true>(width, height, channels, float(factor), testDuration, worker) && allSucceeded;
			Log::info() << " ";
			allSucceeded = testHorizontalVertical<uint8_t, double, true>(width, height, channels, double(factor), testDuration, worker) && allSucceeded;
			Log::info() << " ";
			allSucceeded = testHorizontalVertical<uint8_t, int16_t, true>(width, height, channels, int16_t(factor), testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	for (unsigned int channels = 1u; channels <= 4u; ++channels)
	{
		Log::info().newLine(channels != 1u);
		Log::info().newLine(channels != 1u);

		for (unsigned int factor = 1u; factor <= 2u; ++factor)
		{
			allSucceeded = testHorizontalVertical<uint8_t, int16_t, false>(width, height, channels, int16_t(factor), testDuration, worker) && allSucceeded;
			Log::info() << " ";
			allSucceeded = testHorizontalVertical<uint16_t, int32_t, false>(width, height, channels, factor, testDuration, worker) && allSucceeded;
			Log::info() << " ";
			allSucceeded = testHorizontalVertical<uint8_t, float, false>(width, height, channels, float(factor), testDuration, worker) && allSucceeded;
			Log::info() << " ";
			allSucceeded = testHorizontalVertical<uint8_t, double, false>(width, height, channels, double(factor), testDuration, worker) && allSucceeded;
		}
	}

	return allSucceeded;
}

bool TestFrameFilterGradient::testHorizontalVerticalSubFrame(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	bool allSucceeded = true;

	for (unsigned int channels = 1u; channels <= 4u; ++channels)
	{
		Log::info().newLine(channels != 1u);
		Log::info().newLine(channels != 1u);

		for (unsigned int factor = 1u; factor <= 2u; ++factor)
		{
			if (factor == 1u)
			{
				// the target data type does not allow a factor of 2

				allSucceeded = testHorizontalVerticalSubFrame<uint8_t, int8_t, true>(width, height, channels, int8_t(factor), testDuration, worker) && allSucceeded;
				Log::info() << " ";
				allSucceeded = testHorizontalVerticalSubFrame<uint16_t, int16_t, true>(width, height, channels, int16_t(factor), testDuration, worker) && allSucceeded;
				Log::info() << " ";
				allSucceeded = testHorizontalVerticalSubFrame<uint32_t, int32_t, true>(width, height, channels, factor, testDuration, worker) && allSucceeded;
			}

			Log::info() << " ";
			allSucceeded = testHorizontalVerticalSubFrame<uint8_t, float, true>(width, height, channels, float(factor), testDuration, worker) && allSucceeded;
			Log::info() << " ";
			allSucceeded = testHorizontalVerticalSubFrame<uint8_t, double, true>(width, height, channels, double(factor), testDuration, worker) && allSucceeded;
			Log::info() << " ";
			allSucceeded = testHorizontalVerticalSubFrame<uint8_t, int16_t, true>(width, height, channels, int16_t(factor), testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	for (unsigned int channels = 1u; channels <= 4u; ++channels)
	{
		Log::info().newLine(channels != 1u);
		Log::info().newLine(channels != 1u);

		for (unsigned int factor = 1u; factor <= 2u; ++factor)
		{
			allSucceeded = testHorizontalVerticalSubFrame<uint8_t, int16_t, false>(width, height, channels, int16_t(factor), testDuration, worker) && allSucceeded;
			Log::info() << " ";
			allSucceeded = testHorizontalVerticalSubFrame<uint16_t, int32_t, false>(width, height, channels, factor, testDuration, worker) && allSucceeded;
			Log::info() << " ";
			allSucceeded = testHorizontalVerticalSubFrame<uint8_t, float, false>(width, height, channels, float(factor), testDuration, worker) && allSucceeded;
			Log::info() << " ";
			allSucceeded = testHorizontalVerticalSubFrame<uint8_t, double, false>(width, height, channels, double(factor), testDuration, worker) && allSucceeded;
		}
	}

	return allSucceeded;
}

bool TestFrameFilterGradient::testHorizontalVerticalMagnitudeSquared(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	bool allSucceeded = true;

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		Log::info().newLine(n != 1u);
		Log::info().newLine(n != 1u);

		for (unsigned int i = 1u; i <= 2; ++i)
		{
			allSucceeded = testHorizontalVerticalMagnitudeSquared<uint8_t, uint32_t, true>(width, height, n, i, testDuration, worker) && allSucceeded;
			Log::info() << " ";
			allSucceeded = testHorizontalVerticalMagnitudeSquared<uint8_t, float, true>(width, height, n, float(i), testDuration, worker) && allSucceeded;
			Log::info() << " ";
			allSucceeded = testHorizontalVerticalMagnitudeSquared<uint8_t, double, true>(width, height, n, double(i), testDuration, worker) && allSucceeded;
			Log::info() << " ";
			allSucceeded = testHorizontalVerticalMagnitudeSquared<uint8_t, int32_t, true>(width, height, n, i, testDuration, worker) && allSucceeded;
			Log::info() << " ";
			allSucceeded = testHorizontalVerticalMagnitudeSquared<float, float, true>(width, height, n, float(i), testDuration, worker) && allSucceeded;
			Log::info() << " ";
			allSucceeded = testHorizontalVerticalMagnitudeSquared<double, double, true>(width, height, n, double(i), testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		Log::info().newLine(n != 1u);
		Log::info().newLine(n != 1u);

		for (unsigned int i = 1u; i <= 2; ++i)
		{
			allSucceeded = testHorizontalVerticalMagnitudeSquared<uint8_t, uint32_t, false>(width, height, n, i, testDuration, worker) && allSucceeded;
			Log::info() << " ";
			allSucceeded = testHorizontalVerticalMagnitudeSquared<uint8_t, float, false>(width, height, n, float(i), testDuration, worker) && allSucceeded;
			Log::info() << " ";
			allSucceeded = testHorizontalVerticalMagnitudeSquared<uint8_t, double, false>(width, height, n, double(i), testDuration, worker) && allSucceeded;
			Log::info() << " ";
			allSucceeded = testHorizontalVerticalMagnitudeSquared<uint8_t, int32_t, false>(width, height, n, i, testDuration, worker) && allSucceeded;
			Log::info() << " ";
			allSucceeded = testHorizontalVerticalMagnitudeSquared<float, float, false>(width, height, n, float(i), testDuration, worker) && allSucceeded;
			Log::info() << " ";
			allSucceeded = testHorizontalVerticalMagnitudeSquared<double, double, false>(width, height, n, double(i), testDuration, worker) && allSucceeded;
		}
	}

	return allSucceeded;
}

template <typename TSource, typename TTarget, bool tNormalizeByTwo>
bool TestFrameFilterGradient::testHorizontalVertical(const unsigned int width, const unsigned int height, const unsigned int channels, const TTarget multiplicationFactor, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 3u && height >= 3u && testDuration > 0.0);
	ocean_assert(channels >= 1u && channels <= 4u);

	Log::info() << "Testing " << (tNormalizeByTwo ? "" : "non-") << "normalized horizontal and vertical gradient filter for " << channels << " channels with data types \"" << TypeNamer::name<TSource>() << "\" -> \"" << TypeNamer::name<TTarget>() << "\"" << (multiplicationFactor == 1 ? "" : " with additional multiplication factor") << ":";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		Timestamp startTimestamp(true);

		do
		{
			for (unsigned int benchmarkIteration = 0u; benchmarkIteration < 2u; ++benchmarkIteration)
			{
				const bool benchmark = benchmarkIteration == 0u;

				const unsigned int testWidth = benchmark ? width : RandomI::random(randomGenerator, 3u, 1920u);
				const unsigned int testHeight = benchmark ? height : RandomI::random(randomGenerator, 3u, 1080u);

				const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
				const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

				Frame sourceFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<TSource>(channels), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
				Frame targetFrame(FrameType(sourceFrame, FrameType::genericPixelFormat<TTarget>(channels * 2u)), targetPaddingElements);

				CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);
				CV::CVUtilities::randomizeFrame(targetFrame, false, &randomGenerator);

				const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				performance.startIf(benchmarkIteration);

				switch (channels)
				{
					case 1u:
						CV::FrameFilterGradient::filterHorizontalVertical<TSource, TTarget, 1u, tNormalizeByTwo>(sourceFrame.constdata<TSource>(), targetFrame.data<TTarget>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), multiplicationFactor, useWorker);
						break;

					case 2u:
						CV::FrameFilterGradient::filterHorizontalVertical<TSource, TTarget, 2u, tNormalizeByTwo>(sourceFrame.constdata<TSource>(), targetFrame.data<TTarget>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), multiplicationFactor, useWorker);
						break;

					case 3u:
						CV::FrameFilterGradient::filterHorizontalVertical<TSource, TTarget, 3u, tNormalizeByTwo>(sourceFrame.constdata<TSource>(), targetFrame.data<TTarget>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), multiplicationFactor, useWorker);
						break;

					case 4u:
						CV::FrameFilterGradient::filterHorizontalVertical<TSource, TTarget, 4u, tNormalizeByTwo>(sourceFrame.constdata<TSource>(), targetFrame.data<TTarget>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), multiplicationFactor, useWorker);
						break;

					default:
						ocean_assert(false && "Invalid channel number!");
						allSucceeded = false;
				}

				performance.stopIf(benchmarkIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (!validationHorizontalVertical<TSource, TTarget, tNormalizeByTwo>(sourceFrame, targetFrame, multiplicationFactor))
				{
					allSucceeded = false;
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Singlecore performance: Best: " << performanceSinglecore.bestMseconds() << "ms, worst: " << performanceSinglecore.worstMseconds() << "ms, average: " << performanceSinglecore.averageMseconds() << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore performance: Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms";
		Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
	}

	return allSucceeded;
}

template <typename TSource, typename TTarget, bool tNormalizeByTwo>
bool TestFrameFilterGradient::testHorizontalVerticalMagnitudeSquared(const unsigned int width, const unsigned int height, const unsigned int channels, const TTarget multiplicationFactor, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 3u && height >= 3u && testDuration > 0.0);
	ocean_assert(channels >= 1u && channels <= 4u);

	Log::info() << "Testing " << (tNormalizeByTwo ? "" : "non-") << "normalized squared magnitude gradient filter for " << channels << " channels with data types \"" << TypeNamer::name<TSource>() << "\" -> \"" << TypeNamer::name<TTarget>() << "\"" << (multiplicationFactor == 1 ? "" : " with additional multiplication factor") << ":";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		Timestamp startTimestamp(true);

		do
		{
			for (unsigned int benchmarkIteration = 0u; benchmarkIteration < 2u; ++benchmarkIteration)
			{
				const bool benchmark = benchmarkIteration == 0u;

				const unsigned int testWidth = benchmark ? width : RandomI::random(3u, 1920u);
				const unsigned int testHeight = benchmark ? height : RandomI::random(3u, 1080u);

				constexpr unsigned int sourcePaddingElements = 0u; // not yet supported
				constexpr unsigned int targetPaddingElements = 0u;

				Frame sourceFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<TSource>(channels), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
				Frame targetFrame(FrameType(sourceFrame, FrameType::genericPixelFormat<TTarget>(channels)), targetPaddingElements);

				CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);
				CV::CVUtilities::randomizeFrame(targetFrame, false, &randomGenerator);

				performance.startIf(benchmarkIteration);

				switch (channels)
				{
					case 1u:
						CV::FrameFilterGradient::filterHorizontalVerticalMagnitudeSquared<TSource, TTarget, 1u, tNormalizeByTwo>(sourceFrame.constdata<TSource>(), targetFrame.data<TTarget>(), sourceFrame.width(), sourceFrame.height(), multiplicationFactor, useWorker);
						break;

					case 2u:
						CV::FrameFilterGradient::filterHorizontalVerticalMagnitudeSquared<TSource, TTarget, 2u, tNormalizeByTwo>(sourceFrame.constdata<TSource>(), targetFrame.data<TTarget>(), sourceFrame.width(), sourceFrame.height(), multiplicationFactor, useWorker);
						break;

					case 3u:
						CV::FrameFilterGradient::filterHorizontalVerticalMagnitudeSquared<TSource, TTarget, 3u, tNormalizeByTwo>(sourceFrame.constdata<TSource>(), targetFrame.data<TTarget>(), sourceFrame.width(), sourceFrame.height(), multiplicationFactor, useWorker);
						break;

					case 4u:
						CV::FrameFilterGradient::filterHorizontalVerticalMagnitudeSquared<TSource, TTarget, 4u, tNormalizeByTwo>(sourceFrame.constdata<TSource>(), targetFrame.data<TTarget>(), sourceFrame.width(), sourceFrame.height(), multiplicationFactor, useWorker);
						break;

					default:
						ocean_assert(false && "Invalid channel number!");
						allSucceeded = false;
				}

				performance.stopIf(benchmarkIteration);

				if (!validationHorizontalVerticalMagnitudeSquared<TSource, TTarget, tNormalizeByTwo>(sourceFrame, targetFrame, multiplicationFactor))
				{
					allSucceeded = false;
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Singlecore Best: " << performanceSinglecore.bestMseconds() << "ms, worst: " << performanceSinglecore.worstMseconds() << "ms, average: " << performanceSinglecore.averageMseconds() << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms";
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

template <typename TSource, typename TTarget, bool tNormalizeByTwo>
bool TestFrameFilterGradient::testHorizontalVerticalSubFrame(const unsigned int width, const unsigned int height, const unsigned int channels, const TTarget multiplicationFactor, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 3u && height >= 3u && testDuration > 0.0);

	Log::info() << "Testing " << (tNormalizeByTwo ? "" : "non-") << "normalized sub-frame horizontal and vertical gradient filter for " << channels << " channels with data types \"" << TypeNamer::name<TSource>() << "\" -> \"" << TypeNamer::name<TTarget>() << "\"" << (multiplicationFactor == 1 ? "" : " with additional multiplication factor") << ":";

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<TSource>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height,  FrameType::genericPixelFormat<TTarget>(channels * 2u), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		switch (channels)
		{
			case 1u:
				CV::FrameFilterGradient::filterHorizontalVertical<TSource, TTarget, 1u, tNormalizeByTwo>(sourceFrame.constdata<TSource>(), targetFrame.data<TTarget>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), multiplicationFactor);
				break;

			case 2u:
				CV::FrameFilterGradient::filterHorizontalVertical<TSource, TTarget, 2u, tNormalizeByTwo>(sourceFrame.constdata<TSource>(), targetFrame.data<TTarget>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), multiplicationFactor);
				break;

			case 3u:
				CV::FrameFilterGradient::filterHorizontalVertical<TSource, TTarget, 3u, tNormalizeByTwo>(sourceFrame.constdata<TSource>(), targetFrame.data<TTarget>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), multiplicationFactor);
				break;

			case 4u:
				CV::FrameFilterGradient::filterHorizontalVertical<TSource, TTarget, 4u, tNormalizeByTwo>(sourceFrame.constdata<TSource>(), targetFrame.data<TTarget>(),sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), multiplicationFactor);
				break;

			default:
				ocean_assert(false && "Invalid channel number!");
				allSucceeded = false;
		}

		const unsigned int subFrameLeft = RandomI::random(width - 3u);
		const unsigned int subFrameTop = RandomI::random(height - 3u);

		const unsigned int subFrameWidth = RandomI::random(3u, width - subFrameLeft);
		const unsigned int subFrameHeight = RandomI::random(3u, height - subFrameTop);

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;

			Frame targetSubFrame = CV::CVUtilities::randomizedFrame(FrameType(targetFrame, subFrameWidth, subFrameHeight), &randomGenerator);

			switch (channels)
			{
				case 1u:
					CV::FrameFilterGradient::filterHorizontalVerticalSubFrame<TSource, TTarget, 1u, tNormalizeByTwo>(sourceFrame.constdata<TSource>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), subFrameLeft, subFrameTop, targetSubFrame.data<TTarget>(), targetSubFrame.width(), targetSubFrame.height(), targetSubFrame.paddingElements(), multiplicationFactor, useWorker);
					break;

				case 2u:
					CV::FrameFilterGradient::filterHorizontalVerticalSubFrame<TSource, TTarget, 2u, tNormalizeByTwo>(sourceFrame.constdata<TSource>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), subFrameLeft, subFrameTop, targetSubFrame.data<TTarget>(), targetSubFrame.width(), targetSubFrame.height(), targetSubFrame.paddingElements(), multiplicationFactor, useWorker);
					break;

				case 3u:
					CV::FrameFilterGradient::filterHorizontalVerticalSubFrame<TSource, TTarget, 3u, tNormalizeByTwo>(sourceFrame.constdata<TSource>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), subFrameLeft, subFrameTop, targetSubFrame.data<TTarget>(), targetSubFrame.width(), targetSubFrame.height(), targetSubFrame.paddingElements(), multiplicationFactor, useWorker);
					break;

				case 4u:
					CV::FrameFilterGradient::filterHorizontalVerticalSubFrame<TSource, TTarget, 4u, tNormalizeByTwo>(sourceFrame.constdata<TSource>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), subFrameLeft, subFrameTop, targetSubFrame.data<TTarget>(), targetSubFrame.width(), targetSubFrame.height(), targetSubFrame.paddingElements(), multiplicationFactor, useWorker);
					break;

				default:
					ocean_assert(false && "Invalid channel number!");
					allSucceeded = false;
			}

			for (unsigned int y = 0u; y < subFrameHeight; ++y)
			{
				for (unsigned int x = 0u; x < subFrameWidth; ++x)
				{
					const TTarget* targetPixel = targetFrame.constpixel<TTarget>(x + subFrameLeft, y + subFrameTop);
					const TTarget* subFrameTargetPixel = targetSubFrame.constpixel<TTarget>(x, y);

					for (unsigned int n = 0u; n < channels * 2u; ++n)
					{
						if (targetPixel[n] != subFrameTargetPixel[n])
						{
							allSucceeded = false;
						}
					}
				}
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

template <typename T, typename TIntegral>
bool TestFrameFilterGradient::testFilterHorizontal1x2LinedIntegralImage(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing lined-based horizontal 1x2 gradient filter '" << TypeNamer::name<T>() << "' and '" << TypeNamer::name<TIntegral>() << "':";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int sourceWidth = RandomI::random(randomGenerator, 2u, 1920u);
		const unsigned int sourceHeight = RandomI::random(randomGenerator, 1u, 1080u);
		const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 0u, 1u);

		Frame sourceFrame(FrameType(sourceWidth, sourceHeight, FrameType::genericPixelFormat<T, 1u>(), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
		CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);

		const unsigned int linedIntegralWidth = sourceWidth; // +1 for lined integral, -1 for gradient filter
		const unsigned int linedIntegralHeight = sourceHeight + 1u;

		const unsigned int linedIntegralPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 0u, 1u);

		Frame linedIntegralFrame(FrameType(linedIntegralWidth, linedIntegralHeight, FrameType::genericPixelFormat<TIntegral, 1u>(), FrameType::ORIGIN_UPPER_LEFT), linedIntegralPaddingElements);
		CV::CVUtilities::randomizeFrame(linedIntegralFrame, false, &randomGenerator);

		const Frame copyLinedIntegralFrame(linedIntegralFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		const bool absoluteGradient = RandomI::random(randomGenerator, 0u, 1u) == 1u;

		if (absoluteGradient)
		{
			CV::FrameFilterGradient::filterHorizontal1x2LinedIntegralImage<T, TIntegral, true>(sourceFrame.constdata<T>(), sourceWidth, sourceHeight, linedIntegralFrame.data<TIntegral>(), sourcePaddingElements, linedIntegralPaddingElements);
		}
		else
		{
			CV::FrameFilterGradient::filterHorizontal1x2LinedIntegralImage<T, TIntegral, false>(sourceFrame.constdata<T>(), sourceWidth, sourceHeight, linedIntegralFrame.data<TIntegral>(), sourcePaddingElements, linedIntegralPaddingElements);
		}

		if (!CV::CVUtilities::isPaddingMemoryIdentical(linedIntegralFrame, copyLinedIntegralFrame))
		{
			ocean_assert(false && "Invalid padding data!");
			allSucceeded = false;
			break;
		}

		// top line must be zero
		for (unsigned int x = 0u; x < linedIntegralWidth; ++x)
		{
			if (double(*linedIntegralFrame.constpixel<TIntegral>(x, 0u)) != 0.0)
			{
				allSucceeded = false;
			}
		}

		// left column must be zero
		for (unsigned int y = 0u; y < linedIntegralHeight; ++y)
		{
			if (double(*linedIntegralFrame.constpixel<TIntegral>(0u, y)) != 0.0)
			{
				allSucceeded = false;
			}
		}

		for (unsigned int n = 0u; n < 10u; ++n)
		{
			const unsigned int xPosition = RandomI::random(randomGenerator, 0u, sourceWidth - 2u); // no filter responses for the last column in the source frame
			const unsigned int yPosition = RandomI::random(randomGenerator, 0u, sourceHeight - 1u);

			double sumGradient = 0.0;

			for (unsigned int y = 0u; y <= yPosition; ++y)
			{
				for (unsigned int x = 0u; x <= xPosition; ++x)
				{
					double localGradient = double(*sourceFrame.constpixel<T>(x + 1u, y)) - double(*sourceFrame.constpixel<T>(x, y));

					if (absoluteGradient)
					{
						localGradient = NumericD::abs(localGradient);
					}

					sumGradient += localGradient;
				}
			}

			const double integralValue = double(*linedIntegralFrame.constpixel<TIntegral>(xPosition + 1u, yPosition + 1u));

			if (NumericD::isNotEqual(sumGradient, integralValue, 0.1))
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

template <typename T, typename TIntegral>
bool TestFrameFilterGradient::testFilterVertical2x1LinedIntegralImage(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing lined-based vertical 2x1 gradient filter '" << TypeNamer::name<T>() << "' and '" << TypeNamer::name<TIntegral>() << "':";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int sourceWidth = RandomI::random(randomGenerator, 2u, 1920u);
		const unsigned int sourceHeight = RandomI::random(randomGenerator, 1u, 1080u);
		const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 0u, 1u);

		Frame sourceFrame(FrameType(sourceWidth, sourceHeight, FrameType::genericPixelFormat<T, 1u>(), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
		CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);

		const unsigned int linedIntegralWidth = sourceWidth + 1u;
		const unsigned int linedIntegralHeight = sourceHeight; // +1 for lined integral, -1 for gradient filter

		const unsigned int linedIntegralPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 0u, 1u);

		Frame linedIntegralFrame(FrameType(linedIntegralWidth, linedIntegralHeight, FrameType::genericPixelFormat<TIntegral, 1u>(), FrameType::ORIGIN_UPPER_LEFT), linedIntegralPaddingElements);
		CV::CVUtilities::randomizeFrame(linedIntegralFrame, false, &randomGenerator);

		const Frame copyLinedIntegralFrame(linedIntegralFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		const bool absoluteGradient = RandomI::random(randomGenerator, 0u, 1u) == 1u;

		if (absoluteGradient)
		{
			CV::FrameFilterGradient::filterVertical2x1LinedIntegralImage<T, TIntegral, true>(sourceFrame.constdata<T>(), sourceWidth, sourceHeight, linedIntegralFrame.data<TIntegral>(), sourcePaddingElements, linedIntegralPaddingElements);
		}
		else
		{
			CV::FrameFilterGradient::filterVertical2x1LinedIntegralImage<T, TIntegral, false>(sourceFrame.constdata<T>(), sourceWidth, sourceHeight, linedIntegralFrame.data<TIntegral>(), sourcePaddingElements, linedIntegralPaddingElements);
		}

		if (!CV::CVUtilities::isPaddingMemoryIdentical(linedIntegralFrame, copyLinedIntegralFrame))
		{
			ocean_assert(false && "Invalid padding data!");
			allSucceeded = false;
			break;
		}

		// top line must be zero
		for (unsigned int x = 0u; x < linedIntegralWidth; ++x)
		{
			if (double(*linedIntegralFrame.constpixel<TIntegral>(x, 0u)) != 0.0)
			{
				allSucceeded = false;
			}
		}

		// left column must be zero
		for (unsigned int y = 0u; y < linedIntegralHeight; ++y)
		{
			if (double(*linedIntegralFrame.constpixel<TIntegral>(0u, y)) != 0.0)
			{
				allSucceeded = false;
			}
		}

		for (unsigned int n = 0u; n < 10u; ++n)
		{
			const unsigned int xPosition = RandomI::random(randomGenerator, 0u, sourceWidth - 1u);
			const unsigned int yPosition = RandomI::random(randomGenerator, 0u, sourceHeight - 2u); // no filter responses for the last column in the source frame

			double sumGradient = 0.0;

			for (unsigned int y = 0u; y <= yPosition; ++y)
			{
				for (unsigned int x = 0u; x <= xPosition; ++x)
				{
					double localGradient = double(*sourceFrame.constpixel<T>(x, y + 1u)) - double(*sourceFrame.constpixel<T>(x, y));

					if (absoluteGradient)
					{
						localGradient = NumericD::abs(localGradient);
					}

					sumGradient += localGradient;
				}
			}

			const double integralValue = double(*linedIntegralFrame.constpixel<TIntegral>(xPosition + 1u, yPosition + 1u));

			if (NumericD::isNotEqual(sumGradient, integralValue, 0.1))
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

template <typename TSource, typename TTarget, bool tNormalizeByTwo>
bool TestFrameFilterGradient::validationHorizontalVertical(const Frame& source, const Frame& target, const TTarget multiplicationFactor)
{
	ocean_assert(source.isValid() && target.isValid());

	ocean_assert(source.dataType() == FrameType::dataType<TSource>() && target.dataType() == FrameType::dataType<TTarget>());
	ocean_assert(source.numberPlanes() == 1u && target.numberPlanes() == 1u);
	ocean_assert(source.channels() * 2u == target.channels());

	if (!source.isValid())
	{
		return false;
	}

	if (source.channels() * 2u != target.channels())
	{
		return false;
	}

	const double epsilon = (std::is_floating_point<TSource>::value && std::is_floating_point<TTarget>::value) ? Numeric::weakEps() : double(1); // for integer we allow an error of 1 due to rounding

	typedef typename NextLargerTyper<TTarget>::TypePerformance TTargetLarger;

	// top & bottom row

	for (unsigned int n = 0u; n < target.planeWidthElements(0u); ++n)
	{
		if (target.constrow<TTarget>(0u)[n] != TTarget(0))
		{
			return false;
		}

		if (target.constrow<TTarget>(target.height() - 1u)[n] != TTarget(0))
		{
			return false;
		}
	}

	// left & right column

	for (unsigned int y = 0u; y < target.height(); ++y)
	{
		const TTarget* leftPixel = target.constpixel<TTarget>(0u, y);
		const TTarget* rightPixel = target.constpixel<TTarget>(target.width() - 1u, y);

		for (unsigned int n = 0u; n < target.channels(); ++n)
		{
			if (leftPixel[n] != TTarget(0))
			{
				return false;
			}

			if (rightPixel[n] != TTarget(0))
			{
				return false;
			}
		}
	}

	// inner frame

	for (unsigned int y = 1u; y < source.height() - 1u; ++y)
	{
		for (unsigned int x = 1u; x < source.width() - 1u; ++x)
		{
			for (unsigned int n = 0u; n < source.channels(); ++n)
			{
				// [-1 0 1]
				const TTargetLarger horizontalResponse = TTargetLarger(source.constpixel<TSource>(x + 1u, y)[n] - source.constpixel<TSource>(x - 1u, y)[n]);

				// [-1]
				// [ 0]
				// [ 1]
				const TTargetLarger verticalResponse = TTargetLarger(source.constpixel<TSource>(x, y + 1u)[n] - source.constpixel<TSource>(x, y - 1u)[n]);

				const TTarget horizontalResult = target.constpixel<TTarget>(x, y)[2u * n + 0u];
				const TTarget verticalResult = target.constpixel<TTarget>(x, y)[2u * n + 1u];

				double dHorizontal = NumericD::minValue();
				double dVertical = NumericD::minValue();

				if constexpr (tNormalizeByTwo)
				{
					dHorizontal = double(Utilities::divideBy2<TTargetLarger>(horizontalResponse)) * double(multiplicationFactor);
					dVertical = double(Utilities::divideBy2<TTargetLarger>(verticalResponse)) * double(multiplicationFactor);
				}
				else
				{
					dHorizontal = double(horizontalResponse) * double(multiplicationFactor);
					dVertical = double(verticalResponse) * double(multiplicationFactor);
				}

				if (NumericD::isNotEqual(double(dHorizontal), double(horizontalResult), epsilon))
				{
					return false;
				}

				if (NumericD::isNotEqual(double(dVertical), double(verticalResult), epsilon))
				{
					return false;
				}
			}
		}
	}

	return true;
}

template <typename TSource, typename TTarget, bool tNormalizeByTwo>
bool TestFrameFilterGradient::validationHorizontalVerticalMagnitudeSquared(const Frame& source, const Frame& target, const TTarget multiplicationFactor)
{
	ocean_assert(source.isValid() && target.isValid());

	ocean_assert(source.dataType() == FrameType::dataType<TSource>() && target.dataType() == FrameType::dataType<TTarget>());
	ocean_assert(source.numberPlanes() == 1u && target.numberPlanes() == 1u);
	ocean_assert(source.channels() == target.channels());

	if (!source.isValid())
	{
		return false;
	}

	if (source.channels() != target.channels())
	{
		return false;
	}

	typedef typename NextLargerTyper<TSource>::TypePerformance TSourceLarger;
	typedef typename SignedTyper<TSourceLarger>::Type TSourceLargerSigned;

	// top & bottom row

	for (unsigned int n = 0u; n < source.planeWidthElements(0u); ++n)
	{
		if (target.constrow<TTarget>(0u)[n] != TTarget(0))
		{
			return false;
		}

		if (target.constrow<TTarget>(target.height() - 1u)[n] != TTarget(0))
		{
			return false;
		}
	}

	// left & right column

	for (unsigned int y = 0u; y < target.height(); ++y)
	{
		const TTarget* leftPixel = target.constpixel<TTarget>(0u, y);
		const TTarget* rightPixel = target.constpixel<TTarget>(target.width() - 1u, y);

		for (unsigned int n = 0u; n < target.channels(); ++n)
		{
			if (leftPixel[n] != TTarget(0))
			{
				return false;
			}

			if (rightPixel[n] != TTarget(0))
			{
				return false;
			}
		}
	}

	// inner frame

	for (unsigned int y = 1u; y < source.height() - 1u; ++y)
	{
		for (unsigned int x = 1u; x < source.width() - 1u; ++x)
		{
			for (unsigned int n = 0u; n < source.channels(); ++n)
			{
				// [-1 0 1]
				const TSourceLargerSigned horizontalResponse = TSourceLargerSigned(source.constpixel<TSource>(x + 1u, y)[n] - source.constpixel<TSource>(x - 1u, y)[n]);

				// [-1]
				// [ 0]
				// [ 1]
				const TSourceLargerSigned verticalResponse = TSourceLargerSigned(source.constpixel<TSource>(x, y + 1u)[n] - source.constpixel<TSource>(x, y - 1u)[n]);

				const TTarget result = target.constpixel<TTarget>(x, y)[n];

				double dHorizontal = NumericD::minValue();
				double dVertical = NumericD::minValue();

				if constexpr (tNormalizeByTwo)
				{
					dHorizontal = double(Utilities::divideBy2<TSourceLargerSigned>(horizontalResponse));
					dVertical = double(Utilities::divideBy2<TSourceLargerSigned>(verticalResponse));
				}
				else
				{
					dHorizontal = double(horizontalResponse);
					dVertical = double(verticalResponse);
				}

				const double sqrMagnitude = (NumericD::sqr(dHorizontal) + NumericD::sqr(dVertical)) * double(multiplicationFactor);

				if (NumericD::isNotEqual(sqrMagnitude, double(result), 0.1))
				{
					return false;
				}
			}
		}
	}

	return true;
}

}

}

}
