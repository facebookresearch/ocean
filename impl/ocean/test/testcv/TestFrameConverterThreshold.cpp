/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverterThreshold.h"

#include "ocean/base/Frame.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameConverterThreshold.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameConverterThreshold::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Frame converter threshold test:   ---";
	Log::info() << " ";

	allSucceeded = testConvertY8ToB8(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testInPlaceConvertY8ToB8(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Frame converter threshold test succeeded.";
	}
	else
	{
		Log::info() << "Frame converter threshold test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterThreshold, ConvertY8ToB8_1920x1080)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterThreshold::testConvertY8ToB8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterThreshold, InPlaceConvertY8ToB8_1920x1080)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterThreshold::testInPlaceConvertY8ToB8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterThreshold::testConvertY8ToB8(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing convert Y8 to binary " << width << "x" << height << " image:";
	Log::info() << " ";

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
			for (const bool performanceIteration : {true, false})
			{
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 1u, 1920u);
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 1u, 1080u);

				const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
				const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

				Frame sourceFrame(FrameType(testWidth, testHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
				Frame targetFrame(sourceFrame.frameType(), targetPaddingElements);

				CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);
				CV::CVUtilities::randomizeFrame(targetFrame, false, &randomGenerator);

				const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				const uint8_t threshold = uint8_t(RandomI::random(randomGenerator, 1u, 255u));

				performance.startIf(performanceIteration);
					CV::FrameConverterThreshold::convertY8ToB8(sourceFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), threshold, useWorker);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				for (unsigned int y = 0u; y < sourceFrame.height(); ++y)
				{
					const uint8_t* const sourceRow = sourceFrame.constrow<uint8_t>(y);
					const uint8_t* const targetRow = targetFrame.constrow<uint8_t>(y);

					for (unsigned int x = 0u; x < sourceFrame.width(); ++x)
					{
						if (sourceRow[x] < threshold)
						{
							if (targetRow[x] != 0x00)
							{
								allSucceeded = false;
							}
						}
						else
						{
							if (targetRow[x] != 0xFF)
							{
								allSucceeded = false;
							}
						}
					}
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
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

bool TestFrameConverterThreshold::testInPlaceConvertY8ToB8(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing in-place convert Y8 to binary " << width << "x" << height << " image:";
	Log::info() << " ";

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
			for (const bool performanceIteration : {true, false})
			{
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 1u, 1920u);
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 1u, 1080u);

				const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

				Frame frame(FrameType(testWidth, testHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), paddingElements);

				CV::CVUtilities::randomizeFrame(frame, false, &randomGenerator);

				const Frame copyFrame(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				const uint8_t threshold = uint8_t(RandomI::random(randomGenerator, 1u, 255u));

				performance.startIf(performanceIteration);
					CV::FrameConverterThreshold::convertY8ToB8(frame.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), threshold, useWorker);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, copyFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				for (unsigned int y = 0u; y < frame.height(); ++y)
				{
					const uint8_t* const frameRow = frame.constrow<uint8_t>(y);
					const uint8_t* const copyRow = copyFrame.constrow<uint8_t>(y);

					for (unsigned int x = 0u; x < frame.width(); ++x)
					{
						if (copyRow[x] < threshold)
						{
							if (frameRow[x] != 0x00)
							{
								allSucceeded = false;
							}
						}
						else
						{
							if (frameRow[x] != 0xFF)
							{
								allSucceeded = false;
							}
						}
					}
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
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

}

}

}
