/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testsegmentation/TestMaskCreator.h"

#include "ocean/base/Frame.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/CVUtilities.h"

#include "ocean/cv/segmentation/MaskCreator.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestSegmentation
{

bool TestMaskCreator::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 32u && height >= 32u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Mask creator test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testJoinMasks(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Mask creator test succeeded.";
	}
	else
	{
		Log::info() << "Mask creator test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestMaskCreator, JoinMasks_1920x1080)
{
	Worker worker;
	EXPECT_TRUE(TestMaskCreator::testJoinMasks(1920u, 1080u, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestMaskCreator::testJoinMasks(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Join masks test for " << width << "x" << height << ":";

	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

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
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 1u, width);
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 1u, height);

				const Frame maskFrame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
				Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

				const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				const uint8_t maskValue = uint8_t(RandomI::random(randomGenerator, 255u));

				performance.startIf(performanceIteration);
					CV::Segmentation::MaskCreator::joinMasks(maskFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), maskFrame.width(), maskFrame.height(), maskFrame.paddingElements(), targetFrame.paddingElements(), maskValue, useWorker);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				for (unsigned int y = 0u; y < maskFrame.height(); ++y)
				{
					for (unsigned int x = 0u; x < maskFrame.width(); ++x)
					{
						const uint8_t maskPixel = maskFrame.constpixel<uint8_t>(x, y)[0];
						const uint8_t targetPixel = targetFrame.constpixel<uint8_t>(x, y)[0];

						if (maskPixel == maskValue || targetPixel == maskValue)
						{
							if (targetPixel != maskValue)
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

	Log::info() << "Performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms";
		Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 2u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 2u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 2u) << "x";
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED";
	}

	return allSucceeded;
}

}

}

}

}
