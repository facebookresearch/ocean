/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testadvanced/TestAdvancedFrameShrinker.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/CVUtilities.h"

#include "ocean/cv/advanced/AdvancedFrameShrinker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestAdvanced
{

bool TestAdvancedFrameShrinker::test(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Advanced Frame Shrinker Test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testDivideByTwo(testDuration, worker) && allSucceeded;

	Log::info() << " ";


	if (allSucceeded)
	{
		Log::info() << "Advanced Frame Shrinker test succeeded.";
	}
	else
	{
		Log::info() << "Advanced Frame Shrinker test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestAdvancedFrameShrinker, DivideByTwo_1Channel_SkipMask)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameShrinker::testDivideByTwo<1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, false, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedFrameShrinker, DivideByTwo_2Channel_SkipMask)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameShrinker::testDivideByTwo<2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, false, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedFrameShrinker, DivideByTwo_3Channel_SkipMask)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameShrinker::testDivideByTwo<3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, false, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedFrameShrinker, DivideByTwo_4Channel_SkipMask)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameShrinker::testDivideByTwo<4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, false, GTEST_TEST_DURATION, worker)));
}


TEST(TestAdvancedFrameShrinker, DivideByTwo_1Channel_HandleMask)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameShrinker::testDivideByTwo<1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, true, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedFrameShrinker, DivideByTwo_2Channel_HandleMask)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameShrinker::testDivideByTwo<2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, true, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedFrameShrinker, DivideByTwo_3Channel_HandleMask)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameShrinker::testDivideByTwo<3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, true, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedFrameShrinker, DivideByTwo_4Channel_HandleMask)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameShrinker::testDivideByTwo<4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, true, GTEST_TEST_DURATION, worker)));
}

#endif // OCEAN_USE_GTEST

bool TestAdvancedFrameShrinker::testDivideByTwo(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing divide by two...";
	Log::info() << " ";

	constexpr unsigned int width = 1920u;
	constexpr unsigned int height = 1080u;

	bool allSucceeded = true;

	allSucceeded = testDivideByTwo<1u>(width, height, false, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testDivideByTwo<1u>(width, height, true, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testDivideByTwo<2u>(width, height, false, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testDivideByTwo<2u>(width, height, true, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testDivideByTwo<3u>(width, height, false, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testDivideByTwo<3u>(width, height, true, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testDivideByTwo<4u>(width, height, false, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testDivideByTwo<4u>(width, height, true, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	return allSucceeded;
}

template <unsigned int tChannels>
bool TestAdvancedFrameShrinker::testDivideByTwo(const unsigned int width, const unsigned int height, const bool handleMask, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	if (handleMask)
	{
		Log::info() << "... for " << width << "x" << height << ", and " << tChannels << " channels (handling mask):";
	}
	else
	{
		Log::info() << "... for " << width << "x" << height << ", and " << tChannels << " channels:";
	}

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
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 2u, 1920u);
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 2u, 1080u);

				const unsigned int sourceFramePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
				const unsigned int targetFramePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

				Frame sourceFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), sourceFramePaddingElements);
				Frame targetFrame(FrameType(sourceFrame, sourceFrame.width() / 2u, sourceFrame.height() / 2u), targetFramePaddingElements);

				CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);
				CV::CVUtilities::randomizeFrame(targetFrame, false, &randomGenerator);

				const unsigned int sourceMaskPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
				const unsigned int targetMaskPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

				Frame sourceMask(FrameType(sourceFrame, FrameType::FORMAT_Y8), sourceMaskPaddingElements);
				Frame targetMask(FrameType(targetFrame, FrameType::FORMAT_Y8), targetMaskPaddingElements);

				constexpr uint8_t maskValue = 0x00;
				constexpr uint8_t nonMaskValue = 0xFF;

				sourceMask.setValue(nonMaskValue);

				for (unsigned int n = 0u; n < 10u; ++n)
				{
					const unsigned int xPosition = RandomI::random(randomGenerator, sourceMask.width() - 1u);
					const unsigned int yPosition = RandomI::random(randomGenerator, sourceMask.height() - 1u);

					const unsigned int xSize = RandomI::random(randomGenerator, 1u, 40u) * 2u + 1u;
					const unsigned int ySize = RandomI::random(randomGenerator, 1u, 40u) * 2u + 1u;

					CV::Canvas::ellipse8BitPerChannel<1u>(sourceMask.data<uint8_t>(), sourceMask.width(), sourceMask.height(), CV::PixelPosition(xPosition, yPosition), xSize, ySize, &maskValue);
				}

				for (unsigned int n = 0u; n < 100u; ++n)
				{
					const unsigned int xPosition = RandomI::random(randomGenerator, sourceMask.width() - 1u);
					const unsigned int yPosition = RandomI::random(randomGenerator, sourceMask.height() - 1u);

					const uint8_t color = RandomI::random(randomGenerator, 1u) == 0u ? maskValue : nonMaskValue;

					sourceMask.pixel<uint8_t>(xPosition, yPosition)[0] = color;
				}

				CV::CVUtilities::randomizeFrame(targetMask, false, &randomGenerator);

				const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
				const Frame copyTargetMask(targetMask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				performance.startIf(performanceIteration);
					CV::Advanced::AdvancedFrameShrinker::divideByTwo8BitPerChannel<tChannels>(sourceFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), sourceMask.constdata<uint8_t>(), targetMask.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), sourceMask.paddingElements(), targetMask.paddingElements(), handleMask, nullptr, useWorker);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (!CV::CVUtilities::isPaddingMemoryIdentical(targetMask, copyTargetMask))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (!validateDivideByTwo(sourceFrame, sourceMask, targetFrame, targetMask, handleMask, maskValue, nonMaskValue))
				{
					allSucceeded = false;
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

bool TestAdvancedFrameShrinker::validateDivideByTwo(const Frame& sourceFrame, const Frame& sourceMask, const Frame& targetFrame, const Frame& targetMask, const bool handleMaskPixels, const uint8_t maskValue, const uint8_t nonMaskValue)
{
	ocean_assert(sourceFrame.isValid() && targetFrame.isValid());
	ocean_assert(sourceMask.isValid() && targetMask.isValid());

	ocean_assert(maskValue != nonMaskValue);

	if (!sourceFrame.isValid())
	{
		return false;
	}

	if (!sourceFrame.isPixelFormatCompatible(targetFrame.pixelFormat()) || !targetFrame.isFrameTypeCompatible(FrameType(sourceFrame, sourceFrame.width() / 2u, sourceFrame.height() / 2u), false))
	{
		return false;
	}

	if (!sourceMask.isPixelFormatCompatible(FrameType::FORMAT_Y8) || !targetMask.isPixelFormatCompatible(FrameType::FORMAT_Y8))
	{
		return false;
	}

	const unsigned int channels = sourceFrame.channels();

	const bool threeEndingColumns = sourceFrame.width() % 2u != 0u;
	const bool threeEndingRows = sourceFrame.height() % 2u != 0u;

	for (unsigned int ty = 0u; ty < targetFrame.height(); ++ty)
	{
		for (unsigned int tx = 0u; tx < targetFrame.width(); ++tx)
		{
			const unsigned int sx = tx * 2u;
			const unsigned int sy = ty * 2u;

			unsigned int xPatch = 2u;
			unsigned int yPatch = 2u;

			if (tx + 1u == targetFrame.width() && threeEndingColumns)
			{
				xPatch = 3u;
			}

			if (ty + 1u == targetFrame.height() && threeEndingRows)
			{
				yPatch = 3u;
			}

			Indices32 pixelValues(channels, 0u);

			unsigned int handledPixels = 0u;

			for (unsigned int xx = sx; xx < sx + xPatch; ++xx)
			{
				for (unsigned int yy = sy; yy < sy + yPatch; ++yy)
				{
					if (sourceMask.constpixel<uint8_t>(xx, yy)[0] == nonMaskValue)
					{
						for (unsigned int channelIndex = 0u; channelIndex < channels; ++channelIndex)
						{
							pixelValues[channelIndex] += sourceFrame.constpixel<uint8_t>(xx, yy)[channelIndex];
						}

						++handledPixels;
					}
				}
			}

			if (handledPixels == 0u)
			{
				if (targetMask.constpixel<uint8_t>(tx, ty)[0] != maskValue)
				{
					return false;
				}

				if (handleMaskPixels)
				{
					ocean_assert(handledPixels == 0u);

					for (unsigned int xx = sx; xx < sx + xPatch; ++xx)
					{
						for (unsigned int yy = sy; yy < sy + yPatch; ++yy)
						{
							ocean_assert(sourceMask.constpixel<uint8_t>(xx, yy)[0] != 0xFFu);

							for (unsigned int channelIndex = 0u; channelIndex < channels; ++channelIndex)
							{
								pixelValues[channelIndex] += sourceFrame.constpixel<uint8_t>(xx, yy)[channelIndex];
							}
						}
					}

					handledPixels = xPatch * yPatch;
				}
			}
			else
			{
				if (targetMask.constpixel<uint8_t>(tx, ty)[0] != nonMaskValue)
				{
					return false;
				}
			}

			if (handledPixels != 0u)
			{
				for (unsigned int channelIndex = 0u; channelIndex < channels; ++channelIndex)
				{
					const unsigned int value = (pixelValues[channelIndex] + handledPixels / 2u) / handledPixels;
					ocean_assert(value <= 255u);

					if (targetFrame.constpixel<uint8_t>(tx, ty)[channelIndex] != uint8_t(value))
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

}

}

}

}
