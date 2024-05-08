/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameInverter.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameInverter.h"


namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameInverter::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width > 0u && height > 0u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Frame Inverter test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	for (unsigned int channels = 1u; channels <= 4u; ++channels)
	{
		Log::info().newLine(channels != 1u);
		allSucceeded = testInvert8BitPerChannel(width, height, channels, testDuration, worker) && allSucceeded;
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Frame Inverter test succeeded.";
	}
	else
	{
		Log::info() << "Frame Inverter test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameInverter, Invert8BitPerChannel_1920x1080_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInverter::testInvert8BitPerChannel(1920u, 1080u, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInverter, Invert8BitPerChannel_1920x1080_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInverter::testInvert8BitPerChannel(1920u, 1080u, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInverter, Invert8BitPerChannel_1920x1080_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInverter::testInvert8BitPerChannel(1920u, 1080u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInverter, Invert8BitPerChannel_1920x1080_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInverter::testInvert8BitPerChannel(1920u, 1080u, 4u, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameInverter::testInvert8BitPerChannel(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 3u && height >= 3u);
	ocean_assert(testDuration >= 0.0);

	Log::info() << "Inverting uint8 " << width << "x" << height << ", " << channels << " channels test:";

	const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<uint8_t>(channels);

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

				Frame sourceFrame(FrameType(testWidth, testHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
				Frame targetFrame(sourceFrame.frameType(), targetPaddingElements);

				CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);
				CV::CVUtilities::randomizeFrame(targetFrame, false, &randomGenerator);

				const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				performance.startIf(performanceIteration);
					CV::FrameInverter::invert8BitPerChannel(sourceFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.channels(), sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				for (unsigned int y = 0u; y < sourceFrame.height(); ++y)
				{
					const uint8_t* sourceRow = sourceFrame.constrow<uint8_t>(y);
					const uint8_t* targetRow = targetFrame.constrow<uint8_t>(y);

					for (unsigned int n = 0u; n < sourceFrame.planeWidthElements(0u); ++n)
					{
						if (sourceRow[n] != 0xFF - targetRow[n])
						{
							allSucceeded = false;
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
		Log::info() << "Validation succeeded.";
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
