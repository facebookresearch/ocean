/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameFilterScharr.h"
#include "ocean/test/testcv/TestFrameFilter.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterScharr.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameFilterScharr::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width >= 3u && height >= 3u);

	Log::info() << "---   Scharr filter test with frame size " << width << "x" << height << ":   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	Log::info() << " ";

	allSucceeded = testHorizontalVerticalFilter8BitPerChannel<int8_t>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testHorizontalVerticalFilter8BitPerChannel<int16_t>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDiagonalFilter8BitPerChannel<int8_t>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testDiagonalFilter8BitPerChannel<int16_t>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFilter8BitPerChannel<int8_t>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testFilter8BitPerChannel<int16_t>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHorizontalVerticalMaximumAbsoluteFilter8BitPerChannel<uint8_t>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testHorizontalVerticalMaximumAbsoluteFilter8BitPerChannel<uint16_t>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMaximumAbsoluteFilter8BitPerChannel<uint8_t>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testMaximumAbsoluteFilter8BitPerChannel<uint16_t>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Scharr filter test succeeded.";
	}
	else
	{
		Log::info() << "Scharr filter test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameFilterScharr, HorizontalVerticalFilter8BitPerChannel_1920x1080_int8)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterScharr::testHorizontalVerticalFilter8BitPerChannel<int8_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterScharr, HorizontalVerticalFilter8BitPerChannel_1920x1080_int16)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterScharr::testHorizontalVerticalFilter8BitPerChannel<int16_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameFilterScharr, DiagonalFilter8BitPerChannel_1920x1080_int8)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterScharr::testDiagonalFilter8BitPerChannel<int8_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterScharr, DiagonalFilter8BitPerChannel_1920x1080_int16)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterScharr::testDiagonalFilter8BitPerChannel<int16_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameFilterScharr, Filter8BitPerChannel_1920x1080_int8)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterScharr::testFilter8BitPerChannel<int8_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterScharr, Filter8BitPerChannel_1920x1080_int16)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterScharr::testFilter8BitPerChannel<int16_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameFilterScharr, HorizontalVerticalMaximumAbsoluteFilter8BitPerChannel_1920x1080_uint8)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterScharr::testHorizontalVerticalMaximumAbsoluteFilter8BitPerChannel<uint8_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterScharr, HorizontalVerticalMaximumAbsoluteFilter8BitPerChannel_1920x1080_uint16)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterScharr::testHorizontalVerticalMaximumAbsoluteFilter8BitPerChannel<uint16_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameFilterScharr, MaximumAbsoluteFilter8BitPerChannel_1920x1080_uint8)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterScharr::testMaximumAbsoluteFilter8BitPerChannel<uint8_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterScharr, MaximumAbsoluteFilter8BitPerChannel_1920x1080_uint16)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterScharr::testMaximumAbsoluteFilter8BitPerChannel<uint16_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

#endif // OCEAN_USE_GTEST

template <typename TTarget>
bool TestFrameFilterScharr::testHorizontalVerticalFilter8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert((std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value), "Invalid data type!");

	ocean_assert(width >= 3u && height >= 3u);

	if (std::is_same<TTarget, int8_t>::value)
	{
		Log::info() << "Testing 8 bit horizontal and vertical Scharr filter, with response range [-128, 127]:";
	}
	else
	{
		Log::info() << "Testing 8 bit horizontal and vertical Scharr filter, with response range [-32768, 32767]:";
	}

	bool allSucceeded = true;

	for (unsigned int nChannels = 1u; nChannels <= 4u; ++nChannels)
	{
		Log::info() << " ";
		Log::info() << "... for " << nChannels << " channels";

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		const unsigned int maxWorkerIterations = worker ? 2u : 1u;

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			const Timestamp startTimestamp(true);

			do
			{
				for (const bool performanceIteration : {true, false})
				{
					const unsigned int testWidth = performanceIteration ? width : RandomI::random(3u, width);
					const unsigned int testHeight = performanceIteration ? height : RandomI::random(3u, height);

					const unsigned int sourcePaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);
					const unsigned int targetPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

					Frame source(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(nChannels), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
					Frame target(FrameType(source, FrameType::genericPixelFormat<TTarget>(2u * nChannels)), targetPaddingElements);

					CV::CVUtilities::randomizeFrame(source, false);
					CV::CVUtilities::randomizeFrame(target, false);

					const Frame targetCopy(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.startIf(performanceIteration);

					if (!CV::FrameFilterScharr::Comfort::filterHorizontalVertical(source, target, useWorker))
					{
						ocean_assert(false && "This should never happen!");
						allSucceeded = false;
					}

					performance.stopIf(performanceIteration);

					if (!CV::CVUtilities::isPaddingMemoryIdentical(target, targetCopy))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					if (!validateHorizontalVerticalFilter8BitPerChannel(source, target))
					{
						allSucceeded = false;
					}
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 2u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 2u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 2u) << "x";
		}
	}

	Log::info() << " ";

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

template <typename TTarget>
bool TestFrameFilterScharr::testDiagonalFilter8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert((std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value), "Invalid data type!");

	ocean_assert(width >= 3u && height >= 3u);

	if (std::is_same<TTarget, int8_t>::value)
	{
		Log::info() << "Testing 8 bit diagonal Scharr filter, with response range [-128, 127]:";
	}
	else
	{
		Log::info() << "Testing 8 bit diagonal Scharr filter, with response range [-32768, 32767]:";
	}

	bool allSucceeded = true;

	for (unsigned int nChannels = 1u; nChannels <= 4u; ++nChannels)
	{
		Log::info() << " ";
		Log::info() << "... for " << nChannels << " channels";

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		const unsigned int maxWorkerIterations = worker ? 2u : 1u;

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			const Timestamp startTimestamp(true);

			do
			{
				for (const bool performanceIteration : {true, false})
				{
					const unsigned int testWidth = performanceIteration ? width : RandomI::random(3u, width);
					const unsigned int testHeight = performanceIteration ? height : RandomI::random(3u, height);

					const unsigned int sourcePaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);
					const unsigned int targetPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

					Frame source(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(nChannels), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
					Frame target(FrameType(source, FrameType::genericPixelFormat<TTarget>(2u * nChannels)), targetPaddingElements);

					CV::CVUtilities::randomizeFrame(source, false);
					CV::CVUtilities::randomizeFrame(target, false);

					const Frame targetCopy(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.startIf(performanceIteration);

					if (!CV::FrameFilterScharr::Comfort::filterDiagonal(source, target, useWorker))
					{
						ocean_assert(false && "This should never happen!");
						allSucceeded = false;
					}

					performance.stopIf(performanceIteration);

					if (!CV::CVUtilities::isPaddingMemoryIdentical(target, targetCopy))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					if (!validateDiagonalFilter8BitPerChannel(source, target))
					{
						allSucceeded = false;
					}
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 2u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 2u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 2u) << "x";
		}
	}

	Log::info() << " ";

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

template <typename TTarget>
bool TestFrameFilterScharr::testFilter8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert((std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value), "Invalid data type!");

	ocean_assert(width >= 3u && height >= 3u);

	if (std::is_same<TTarget, int8_t>::value)
	{
		Log::info() << "Testing 8 bit horizontal, vertical, and diagonal Scharr filter, with response range [-128, 127]:";
	}
	else
	{
		Log::info() << "Testing 8 bit horizontal, vertical, and diagonal Scharr filter, with response range [-32768, 32767]:";
	}

	bool allSucceeded = true;

	for (unsigned int nChannels = 1u; nChannels <= 4u; ++nChannels)
	{
		Log::info() << " ";
		Log::info() << "... for " << nChannels << " channels";

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		const unsigned int maxWorkerIterations = worker ? 2u : 1u;

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			const Timestamp startTimestamp(true);

			do
			{
				for (const bool performanceIteration : {true, false})
				{
					const unsigned int testWidth = performanceIteration ? width : RandomI::random(3u, width);
					const unsigned int testHeight = performanceIteration ? height : RandomI::random(3u, height);

					const unsigned int sourcePaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);
					const unsigned int targetPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

					Frame source(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(nChannels), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
					Frame target(FrameType(source, FrameType::genericPixelFormat<TTarget>(4u * nChannels)), targetPaddingElements);

					CV::CVUtilities::randomizeFrame(source, false);
					CV::CVUtilities::randomizeFrame(target, false);

					const Frame targetCopy(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.startIf(performanceIteration);

					if (!CV::FrameFilterScharr::Comfort::filter(source, target, useWorker))
					{
						ocean_assert(false && "This should never happen!");
						allSucceeded = false;
					}

					performance.stopIf(performanceIteration);

					if (!CV::CVUtilities::isPaddingMemoryIdentical(target, targetCopy))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					if (!validateFilter8BitPerChannel(source, target))
					{
						allSucceeded = false;
					}
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 2u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 2u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 2u) << "x";
		}
	}

	Log::info() << " ";

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

template <typename TTarget>
bool TestFrameFilterScharr::testHorizontalVerticalMaximumAbsoluteFilter8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert((std::is_same<TTarget, uint8_t>::value || std::is_same<TTarget, uint16_t>::value), "Invalid data type!");

	ocean_assert(width >= 3u && height >= 3u);

	if (std::is_same<TTarget, uint8_t>::value)
	{
		Log::info() << "Testing 8 bit horizontal and vertical maximum absolute Scharr filter, with response range [0, 255]:";
	}
	else
	{
		Log::info() << "Testing 8 bit horizontal and vertical maximum absolute Scharr filter, with response range [0, 65535]:";
	}

	bool allSucceeded = true;

	for (unsigned int nChannels = 1u; nChannels <= 4u; ++nChannels)
	{
		Log::info() << " ";
		Log::info() << "... for " << nChannels << " channels";

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		const unsigned int maxWorkerIterations = worker ? 2u : 1u;

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			const Timestamp startTimestamp(true);

			do
			{
				for (const bool performanceIteration : {true, false})
				{
					const unsigned int testWidth = performanceIteration ? width : RandomI::random(3u, width);
					const unsigned int testHeight = performanceIteration ? height : RandomI::random(3u, height);

					const unsigned int sourcePaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);
					const unsigned int targetPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

					Frame source(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(nChannels), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
					Frame target(FrameType(source, FrameType::genericPixelFormat<TTarget>(nChannels)), targetPaddingElements);

					CV::CVUtilities::randomizeFrame(source, false);
					CV::CVUtilities::randomizeFrame(target, false);

					const Frame targetCopy(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.startIf(performanceIteration);

					if (!CV::FrameFilterScharr::Comfort::filterHorizontalVerticalMaximumAbsolute(source, target, useWorker))
					{
						ocean_assert(false && "This should never happen!");
						allSucceeded = false;
					}

					performance.stopIf(performanceIteration);

					if (!CV::CVUtilities::isPaddingMemoryIdentical(target, targetCopy))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					if (!validateHorizontalVerticalMaximumAbsoluteFilter8BitPerChannel(source, target))
					{
						allSucceeded = false;
					}
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 2u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 2u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 2u) << "x";
		}
	}

	Log::info() << " ";

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

template <typename TTarget>
bool TestFrameFilterScharr::testMaximumAbsoluteFilter8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert((std::is_same<TTarget, uint8_t>::value || std::is_same<TTarget, uint16_t>::value), "Invalid data type!");

	ocean_assert(width >= 3u && height >= 3u);

	if (std::is_same<TTarget, uint8_t>::value)
	{
		Log::info() << "Testing 8 bit horizontal, vertical, and diagonal maximum absolute Scharr filter, with response range [0, 255]:";
	}
	else
	{
		Log::info() << "Testing 8 bit horizontal, vertical, and diagonal maximum absolute Scharr filter, with response range [0, 65535]:";
	}

	bool allSucceeded = true;

	for (unsigned int nChannels = 1u; nChannels <= 4u; ++nChannels)
	{
		Log::info() << " ";
		Log::info() << "... for " << nChannels << " channels";

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		const unsigned int maxWorkerIterations = worker ? 2u : 1u;

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			const Timestamp startTimestamp(true);

			do
			{
				for (const bool performanceIteration : {true, false})
				{
					const unsigned int testWidth = performanceIteration ? width : RandomI::random(3u, width);
					const unsigned int testHeight = performanceIteration ? height : RandomI::random(3u, height);

					const unsigned int sourcePaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);
					const unsigned int targetPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

					Frame source(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(nChannels), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
					Frame target(FrameType(source, FrameType::genericPixelFormat<TTarget>(nChannels)), targetPaddingElements);

					CV::CVUtilities::randomizeFrame(source, false);
					CV::CVUtilities::randomizeFrame(target, false);

					const Frame targetCopy(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.startIf(performanceIteration);

					if (!CV::FrameFilterScharr::Comfort::filterMaximumAbsolute(source, target, useWorker))
					{
						ocean_assert(false && "This should never happen!");
						allSucceeded = false;
					}

					performance.stopIf(performanceIteration);

					if (!CV::CVUtilities::isPaddingMemoryIdentical(target, targetCopy))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					if (!validateMaximumAbsoluteFilter8BitPerChannel(source, target))
					{
						allSucceeded = false;
					}
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 2u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 2u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 2u) << "x";
		}
	}

	Log::info() << " ";

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

bool TestFrameFilterScharr::validateHorizontalVerticalFilter8BitPerChannel(const Frame& frame, const Frame& response)
{
	ocean_assert(frame.width() == response.width());
	ocean_assert(frame.height() == response.height());

	ocean_assert(response.isPixelFormatCompatible(FrameType::genericPixelFormat<int8_t>(2u * frame.channels())) || response.isPixelFormatCompatible(FrameType::genericPixelFormat<int16_t>(2u * frame.channels())));

	const bool responseIsInt8 = response.dataType() == FrameType::DT_SIGNED_INTEGER_8;

	const unsigned int channels = frame.channels();

	for (unsigned int y = 0u; y < frame.height(); y++)
	{
		for (unsigned int x = 0u; x < frame.width(); x++)
		{
			for (unsigned int channelIndex = 0u; channelIndex < channels; ++channelIndex)
			{
				const int32_t response0 = filterResponse<0u>(frame, x, y, channelIndex);
				const int32_t response90 = filterResponse<90u>(frame, x, y, channelIndex);

				if (responseIsInt8)
				{
					const int8_t normalizedResponse0 = int8_t(response0 / 32);
					const int8_t normalizedResponse90 = int8_t(response90 / 32);

					const int8_t* const responsePixel = response.constpixel<int8_t>(x, y) + 2u * channelIndex;

					if (normalizedResponse0 != responsePixel[0] || normalizedResponse90 != responsePixel[1])
					{
						return false;
					}
				}
				else
				{
					const int16_t* const responsePixel = response.constpixel<int16_t>(x, y) + 2u * channelIndex;

					if (response0 != int32_t(responsePixel[0]) || response90 != int32_t(responsePixel[1]))
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

bool TestFrameFilterScharr::validateDiagonalFilter8BitPerChannel(const Frame& frame, const Frame& response)
{
	ocean_assert(frame.width() == response.width());
	ocean_assert(frame.height() == response.height());

	ocean_assert(response.isPixelFormatCompatible(FrameType::genericPixelFormat<int8_t>(2u * frame.channels())) || response.isPixelFormatCompatible(FrameType::genericPixelFormat<int16_t>(2u * frame.channels())));

	const bool responseIsInt8 = response.dataType() == FrameType::DT_SIGNED_INTEGER_8;

	const unsigned int channels = frame.channels();

	for (unsigned int y = 0u; y < frame.height(); y++)
	{
		for (unsigned int x = 0u; x < frame.width(); x++)
		{
			for (unsigned int channelIndex = 0u; channelIndex < channels; ++channelIndex)
			{
				const int32_t response45 = filterResponse<45u>(frame, x, y, channelIndex);
				const int32_t response135 = filterResponse<135u>(frame, x, y, channelIndex);

				if (responseIsInt8)
				{
					const int8_t normalizedResponse45 = int8_t(response45 / 32);
					const int8_t normalizedResponse135 = int8_t(response135 / 32);

					const int8_t* const responsePixel = response.constpixel<int8_t>(x, y) + 2u * channelIndex;

					if (normalizedResponse45 != responsePixel[0] || normalizedResponse135 != responsePixel[1])
					{
						return false;
					}
				}
				else
				{
					const int16_t* const responsePixel = response.constpixel<int16_t>(x, y) + 2u * channelIndex;

					if (response45 != int32_t(responsePixel[0]) || response135 != int32_t(responsePixel[1]))
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

bool TestFrameFilterScharr::validateFilter8BitPerChannel(const Frame& frame, const Frame& response)
{
	ocean_assert(frame.width() == response.width());
	ocean_assert(frame.height() == response.height());

	ocean_assert(response.isPixelFormatCompatible(FrameType::genericPixelFormat<int8_t>(4u * frame.channels())) || response.isPixelFormatCompatible(FrameType::genericPixelFormat<int16_t>(4u * frame.channels())));

	const bool responseIsInt8 = response.dataType() == FrameType::DT_SIGNED_INTEGER_8;

	const unsigned int channels = frame.channels();

	for (unsigned int y = 0u; y < frame.height(); y++)
	{
		for (unsigned int x = 0u; x < frame.width(); x++)
		{
			for (unsigned int channelIndex = 0u; channelIndex < channels; ++channelIndex)
			{
				const int32_t response0 = filterResponse<0u>(frame, x, y, channelIndex);
				const int32_t response90 = filterResponse<90u>(frame, x, y, channelIndex);
				const int32_t response45 = filterResponse<45u>(frame, x, y, channelIndex);
				const int32_t response135 = filterResponse<135u>(frame, x, y, channelIndex);

				if (responseIsInt8)
				{
					const int8_t normalizedResponse0 = int8_t(response0 / 32);
					const int8_t normalizedResponse90 = int8_t(response90 / 32);
					const int8_t normalizedResponse45 = int8_t(response45 / 32);
					const int8_t normalizedResponse135 = int8_t(response135 / 32);

					const int8_t* const responsePixel = response.constpixel<int8_t>(x, y) + 4u * channelIndex;

					if (normalizedResponse0 != responsePixel[0] || normalizedResponse90 != responsePixel[1] || normalizedResponse45 != responsePixel[2] || normalizedResponse135 != responsePixel[3])
					{
						return false;
					}
				}
				else
				{
					const int16_t* const responsePixel = response.constpixel<int16_t>(x, y) + 4u * channelIndex;

					if (response0 != int32_t(responsePixel[0]) || response90 != int32_t(responsePixel[1]) || response45 != int32_t(responsePixel[2]) || response135 != int32_t(responsePixel[3]))
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

bool TestFrameFilterScharr::validateHorizontalVerticalMaximumAbsoluteFilter8BitPerChannel(const Frame& frame, const Frame& response)
{
	ocean_assert(frame.width() == response.width());
	ocean_assert(frame.height() == response.height());

	ocean_assert(response.isPixelFormatCompatible(FrameType::genericPixelFormat<uint8_t>(frame.channels())) || response.isPixelFormatCompatible(FrameType::genericPixelFormat<uint16_t>(frame.channels())));

	const bool responseIsUInt8 = response.dataType() == FrameType::DT_UNSIGNED_INTEGER_8;

	const unsigned int channels = frame.channels();

	for (unsigned int y = 0u; y < frame.height(); y++)
	{
		for (unsigned int x = 0u; x < frame.width(); x++)
		{
			for (unsigned int channelIndex = 0u; channelIndex < channels; ++channelIndex)
			{
				const int32_t response0 = filterResponse<0u>(frame, x, y, channelIndex);
				const int32_t response90 = filterResponse<90u>(frame, x, y, channelIndex);

				if (responseIsUInt8)
				{
					const uint8_t normalizedResponse = uint8_t(max((abs(response0) + 8) / 16, (abs(response90) + 8) / 16));

					const uint8_t responsePixel = response.constpixel<uint8_t>(x, y)[channelIndex];

					if (normalizedResponse != responsePixel)
					{
						return false;
					}
				}
				else
				{
					const int32_t normalizedResponse = max(abs(response0), abs(response90));

					const uint16_t responsePixel = response.constpixel<uint16_t>(x, y)[channelIndex];

					if (normalizedResponse != int32_t(responsePixel))
					{
						return false;
					}
				}
			}
		}
	}

	if (!CV::CVUtilities::isBorderZero(response))
	{
		return false;
	}

	return true;
}

bool TestFrameFilterScharr::validateMaximumAbsoluteFilter8BitPerChannel(const Frame& frame, const Frame& response)
{
	ocean_assert(frame.width() == response.width());
	ocean_assert(frame.height() == response.height());

	ocean_assert(response.isPixelFormatCompatible(FrameType::genericPixelFormat<uint8_t>(frame.channels())) || response.isPixelFormatCompatible(FrameType::genericPixelFormat<uint16_t>(frame.channels())));

	const bool responseIsUInt8 = response.dataType() == FrameType::DT_UNSIGNED_INTEGER_8;

	const unsigned int channels = frame.channels();

	for (unsigned int y = 1u; y < frame.height() - 1u; y++)
	{
		for (unsigned int x = 1u; x < frame.width() - 1u; x++)
		{
			for (unsigned int channelIndex = 0u; channelIndex < channels; ++channelIndex)
			{
				const int32_t response0 = filterResponse<0u>(frame, x, y, channelIndex);
				const int32_t response90 = filterResponse<90u>(frame, x, y, channelIndex);
				const int32_t response45 = filterResponse<45u>(frame, x, y, channelIndex);
				const int32_t response135 = filterResponse<135u>(frame, x, y, channelIndex);

				if (responseIsUInt8)
				{
					const uint8_t normalizedResponse = uint8_t(max(max((abs(response0) + 8) / 16, (abs(response90) + 8) / 16), max((abs(response45) + 8) / 16, (abs(response135) + 8) / 16)));

					const uint8_t responsePixel = response.constpixel<uint8_t>(x, y)[channelIndex];

					if (normalizedResponse != responsePixel)
					{
						return false;
					}
				}
				else
				{
					const int32_t normalizedResponse = max(max(abs(response0), abs(response90)), max(abs(response45), abs(response135)));

					const uint16_t responsePixel = response.constpixel<uint16_t>(x, y)[channelIndex];

					if (normalizedResponse != int32_t(responsePixel))
					{
						return false;
					}
				}
			}
		}
	}

	if (!CV::CVUtilities::isBorderZero(response))
	{
		return false;
	}

	return true;
}

}

}

}
