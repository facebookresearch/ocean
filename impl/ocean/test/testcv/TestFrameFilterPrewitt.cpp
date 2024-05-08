/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameFilterPrewitt.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterPrewitt.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameFilterPrewitt::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	Log::info() << "---   Prewitt filter test with frame size " << width << "x" << height << ":   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testHorizontalVerticalFilter8BitPerChannel<int8_t>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testHorizontalVerticalFilter8BitPerChannel<int16_t>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Prewitt filter test succeeded.";
	}
	else
	{
		Log::info() << "Prewitt filter test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameFilterPrewitt, HorizontalVerticalFilter8BitPerChannel_1920x1080_int8)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterPrewitt::testHorizontalVerticalFilter8BitPerChannel<int8_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterPrewitt, HorizontalVerticalFilter8BitPerChannel_1920x1080_int16)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterPrewitt::testHorizontalVerticalFilter8BitPerChannel<int16_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

#endif // OCEAN_USE_GTEST

template <typename TTarget>
bool TestFrameFilterPrewitt::testHorizontalVerticalFilter8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert((std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value), "Invalid data type!");

	ocean_assert(width >= 3u && height >= 3u);

	if (std::is_same<TTarget, int8_t>::value)
	{
		Log::info() << "Testing 8 bit horizontal and vertical Prewitt filter, with response range [-128, 127]:";
	}
	else
	{
		Log::info() << "Testing 8 bit horizontal and vertical Prewitt filter, with response range [-32768, 32767]:";
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

					switch (source.channels())
					{
						case 1u:
							CV::FrameFilterPrewitt::filterHorizontalVertical8BitPerChannel<TTarget, 1u>(source.constdata<uint8_t>(), target.data<TTarget>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), useWorker);
							break;

						case 2u:
							CV::FrameFilterPrewitt::filterHorizontalVertical8BitPerChannel<TTarget, 2u>(source.constdata<uint8_t>(), target.data<TTarget>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), useWorker);
							break;

						case 3u:
							CV::FrameFilterPrewitt::filterHorizontalVertical8BitPerChannel<TTarget, 3u>(source.constdata<uint8_t>(), target.data<TTarget>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), useWorker);
							break;

						case 4u:
							CV::FrameFilterPrewitt::filterHorizontalVertical8BitPerChannel<TTarget, 4u>(source.constdata<uint8_t>(), target.data<TTarget>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), useWorker);
							break;

						default:
							ocean_assert(false && "Invalid channel number!");
							allSucceeded = false;
							break;
					}

					performance.stopIf(performanceIteration);

					if (!CV::CVUtilities::isPaddingMemoryIdentical(target, targetCopy))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					if (!validateHorizontalVerticalFilter8BitPerChannel<TTarget>(source, target))
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
bool TestFrameFilterPrewitt::validateHorizontalVerticalFilter8BitPerChannel(const Frame& frame, const Frame& response)
{
	ocean_assert(frame.width() == response.width());
	ocean_assert(frame.height() == response.height());

	ocean_assert(response.isPixelFormatCompatible(FrameType::genericPixelFormat<int8_t>(2u * frame.channels())) || response.isPixelFormatCompatible(FrameType::genericPixelFormat<int16_t>(2u * frame.channels())));
	ocean_assert(response.isPixelFormatCompatible(FrameType::genericPixelFormat<TTarget>(2u * frame.channels())));

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
					const int8_t normalizedResponse0 = int8_t(response0 / 8);
					const int8_t normalizedResponse90 = int8_t(response90 / 8);

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

}

}

}
