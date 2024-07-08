/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameFilterTemplate.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterTemplate.h"

#include "ocean/math/Numeric.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameFilterTemplate::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 3u && height >= 3u && testDuration > 0.0);
	ocean_assert(testDuration > 0);

	Log::info() << "---   Frame filter template test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		Log::info().newLine(n != 1u);
		allSucceeded = testFilter8BitPerChannelTo8BitInteger(width, height, n, testDuration, worker) && allSucceeded;
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		Log::info().newLine(n != 1u);
		allSucceeded = testFilter8BitPerChannelTo32BitFloat(width, height, n, testDuration, worker) && allSucceeded;
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		Log::info().newLine(n != 1u);
		allSucceeded = testFilterWithFactor8BitPerChannelTo32BitFloat(width, height, n, testDuration, worker) && allSucceeded;
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		Log::info().newLine(n != 1u);
		allSucceeded = testPixel8BitPerChannel(width, height, n, testDuration) && allSucceeded;
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Frame filter template test succeeded.";
	}
	else
	{
		Log::info() << "frame filter template test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameFilterTemplate, Filter8BitPerChannelTo8BitInteger1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterTemplate::testFilter8BitPerChannelTo8BitInteger(1920u, 1080u, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterTemplate, Filter8BitPerChannelTo8BitInteger2Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterTemplate::testFilter8BitPerChannelTo8BitInteger(1920u, 1080u, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterTemplate, Filter8BitPerChannelTo8BitInteger3Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterTemplate::testFilter8BitPerChannelTo8BitInteger(1920u, 1080u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterTemplate, Filter8BitPerChannelTo8BitInteger4Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterTemplate::testFilter8BitPerChannelTo8BitInteger(1920u, 1080u, 4u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameFilterTemplate, Filter8BitPerChannelTo32BitFloat1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterTemplate::testFilter8BitPerChannelTo32BitFloat(1920u, 1080u, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterTemplate, Filter8BitPerChannelTo32BitFloat2Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterTemplate::testFilter8BitPerChannelTo32BitFloat(1920u, 1080u, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterTemplate, Filter8BitPerChannelTo32BitFloat3Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterTemplate::testFilter8BitPerChannelTo32BitFloat(1920u, 1080u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterTemplate, Filter8BitPerChannelTo32BitFloat4Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterTemplate::testFilter8BitPerChannelTo32BitFloat(1920u, 1080u, 4u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameFilterTemplate, FilterWithFactor8BitPerChannelTo32BitFloat1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterTemplate::testFilterWithFactor8BitPerChannelTo32BitFloat(1920u, 1080u, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterTemplate, FilterWithFactor8BitPerChannelTo32BitFloat2Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterTemplate::testFilterWithFactor8BitPerChannelTo32BitFloat(1920u, 1080u, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterTemplate, FilterWithFactor8BitPerChannelTo32BitFloat3Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterTemplate::testFilterWithFactor8BitPerChannelTo32BitFloat(1920u, 1080u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterTemplate, FilterWithFactor8BitPerChannelTo32BitFloat4Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterTemplate::testFilterWithFactor8BitPerChannelTo32BitFloat(1920u, 1080u, 4u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameFilterTemplate, Pixel8BitPerChannel1Channel)
{
	EXPECT_TRUE(TestFrameFilterTemplate::testPixel8BitPerChannel(1920u, 1080u, 1u, GTEST_TEST_DURATION));
}

TEST(TestFrameFilterTemplate, Pixel8BitPerChannel2Channel)
{
	EXPECT_TRUE(TestFrameFilterTemplate::testPixel8BitPerChannel(1920u, 1080u, 2u, GTEST_TEST_DURATION));
}

TEST(TestFrameFilterTemplate, Pixel8BitPerChannel3Channel)
{
	EXPECT_TRUE(TestFrameFilterTemplate::testPixel8BitPerChannel(1920u, 1080u, 3u, GTEST_TEST_DURATION));
}

TEST(TestFrameFilterTemplate, Pixel8BitPerChannel4Channel)
{
	EXPECT_TRUE(TestFrameFilterTemplate::testPixel8BitPerChannel(1920u, 1080u, 4u, GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestFrameFilterTemplate::testFilter8BitPerChannelTo8BitInteger(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 3u && height >= 3u);
	ocean_assert(channels >= 1u && channels <= 4u);
	ocean_assert(testDuration > 0);

	Log::info() << "Filter test for " << width << "x" << height << " with pattern: -1, 0, 1, -2, 0, 2, -1, 0, 1 for " << channels << " channels with 8 bit integer response:";
	Log::info() << " ";

	const int filterPattern[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};

	bool allSucceeded = true;

	for (unsigned int nOrientation = 0u; nOrientation < 8u; ++nOrientation)
	{
		bool localSucceeded = true;

		Log::info().newLine(nOrientation != 0u);
		Log::info() << "Orientation: " << directionStrings()[nOrientation];

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
				for (bool performanceIteration : {true, false})
				{
					const unsigned int testWidth = performanceIteration ? width : RandomI::random(3u, 500u);
					const unsigned int testHeight = performanceIteration ? height : RandomI::random(3u, 500u);

					const unsigned int framePaddingElements = RandomI::random(0u, 100u) * RandomI::random(1u);
					const unsigned int targetPaddingElements = RandomI::random(0u, 100u) * RandomI::random(1u);

					Frame frame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);
					Frame target(FrameType(frame, FrameType::genericPixelFormat<int8_t>(channels)), targetPaddingElements);

					CV::CVUtilities::randomizeFrame(frame, false);
					CV::CVUtilities::randomizeFrame(target, false);

					const Frame copyTarget(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.startIf(performanceIteration);
					CV::FrameFilterTemplate<int32_t, -1, 0, 1, -2, 0, 2, -1, 0, 1>::filter<uint8_t, int8_t, int32_t, 4, 0>(frame.constdata<uint8_t>(), target.data<int8_t>(), frame.width(), frame.height(), channels, directions()[nOrientation], frame.paddingElements(), target.paddingElements(), useWorker);
					performance.stopIf(performanceIteration);

					if (!CV::CVUtilities::isPaddingMemoryIdentical(target, copyTarget))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					if (!validateFilter8BitPerChannel<int8_t, int32_t>(frame.constdata<uint8_t>(), target.data<int8_t>(), frame.width(), frame.height(), channels, filterPattern, 4, 0, directions()[nOrientation], frame.paddingElements(), target.paddingElements()))
					{
						localSucceeded = false;
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

		if (localSucceeded)
		{
			Log::info() << "Validation: succeeded.";
		}
		else
		{
			allSucceeded = false;
			Log::info() << "Validation: FAILED!";
		}
	}

	return allSucceeded;
}

bool TestFrameFilterTemplate::testFilter8BitPerChannelTo32BitFloat(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 3u && height >= 3u);
	ocean_assert(channels >= 1u && channels <= 4u);
	ocean_assert(testDuration > 0);

	Log::info() << "Filter test for " << width << "x" << height << " with pattern: -1, 4, 3, -2, -4, 2, 1, 7, -6 for " << channels << " channels with 32 bit float response:";
	Log::info() << " ";

	const int filterPattern[] = {-1, 4, 3, -2, -4, 2, 1, 7, -6};

	bool allSucceeded = true;

	for (unsigned int nOrientation = 0u; nOrientation < 8u; ++nOrientation)
	{
		bool localSucceeded = true;

		Log::info().newLine(nOrientation != 0u);
		Log::info() << "Orientation: " << directionStrings()[nOrientation];

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
				for (bool performanceIteration : {true, false})
				{
					const unsigned int testWidth = performanceIteration ? width : RandomI::random(3u, 500u);
					const unsigned int testHeight = performanceIteration ? height : RandomI::random(3u, 500u);

					const unsigned int framePaddingElements = RandomI::random(0u, 100u) * RandomI::random(1u);
					const unsigned int targetPaddingElements = RandomI::random(0u, 100u) * RandomI::random(1u);

					Frame frame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);
					Frame target(FrameType(frame, FrameType::genericPixelFormat<float>(channels)), targetPaddingElements);

					CV::CVUtilities::randomizeFrame(frame, false);
					CV::CVUtilities::randomizeFrame(target, false);

					const Frame copyTarget(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.start();
					CV::FrameFilterTemplate<int32_t, -1, 4, 3, -2, -4, 2, 1, 7, -6>::filter<uint8_t, float, int32_t, 1, 0>(frame.constdata<uint8_t>(), target.data<float>(), frame.width(), frame.height(), channels, directions()[nOrientation], frame.paddingElements(), target.paddingElements(), useWorker);
					performance.stop();

					if (!CV::CVUtilities::isPaddingMemoryIdentical(target, copyTarget))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					if (!validateFilter8BitPerChannel<float, int>(frame.constdata<uint8_t>(), target.data<float>(), frame.width(), frame.height(), channels, filterPattern, 1, 0, directions()[nOrientation], frame.paddingElements(), target.paddingElements()))
					{
						localSucceeded = false;
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

		if (localSucceeded)
		{
			Log::info() << "Validation: succeeded.";
		}
		else
		{
			allSucceeded = false;
			Log::info() << "Validation: FAILED!";
		}
	}

	return allSucceeded;
}

bool TestFrameFilterTemplate::testFilterWithFactor8BitPerChannelTo32BitFloat(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 3u && height >= 3u);
	ocean_assert(channels >= 1u && channels <= 4u);
	ocean_assert(testDuration > 0);

	Log::info() << "Filter test for " << width << "x" << height << " with flexible factor and with pattern: -1, 4, 3, -2, -4, 2, 1, 7, -6 for " << channels << " channels with 32 bit float response:";
	Log::info() << " ";

	const int filterPattern[] = {-1, 4, 3, -2, -4, 2, 1, 7, -6};

	bool allSucceeded = true;

	for (unsigned int nOrientation = 0u; nOrientation < 8u; ++nOrientation)
	{
		bool localSucceeded = true;

		Log::info().newLine(nOrientation != 0u);
		Log::info() << "Orientation: " << directionStrings()[nOrientation];

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		const unsigned int maxWorkerIterations = worker ? 2u : 1u;

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			const Timestamp startTimestamp(true);

			float normalizationFactor = 0.5f;

			do
			{
				for (bool performanceIteration : {true, false})
				{
					const unsigned int testWidth = performanceIteration ? width : RandomI::random(3u, 500u);
					const unsigned int testHeight = performanceIteration ? height : RandomI::random(3u, 500u);

					const unsigned int framePaddingElements = RandomI::random(0u, 100u) * RandomI::random(1u);
					const unsigned int targetPaddingElements = RandomI::random(0u, 100u) * RandomI::random(1u);

					Frame frame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels), FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);
					Frame target(FrameType(frame, FrameType::genericPixelFormat(FrameType::DT_SIGNED_FLOAT_32, channels)), targetPaddingElements);

					CV::CVUtilities::randomizeFrame(frame, false);
					CV::CVUtilities::randomizeFrame(target, false);

					const Frame copyTarget(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					normalizationFactor = float(Random::scalar(Scalar(0.0001), 5));

					performance.start();
					CV::FrameFilterTemplate<int32_t, -1, 4, 3, -2, -4, 2, 1, 7, -6>::filterWithFactor<uint8_t, float, float>(frame.constdata<uint8_t>(), target.data<float>(), frame.width(), frame.height(), normalizationFactor, channels, directions()[nOrientation], frame.paddingElements(), target.paddingElements(), useWorker);
					performance.stop();

					if (!CV::CVUtilities::isPaddingMemoryIdentical(target, copyTarget))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					if (!validateFilterWithFactor8BitPerChannel<float, float>(frame.constdata<uint8_t>(), target.data<float>(), frame.width(), frame.height(), channels, filterPattern, normalizationFactor, directions()[nOrientation], frame.paddingElements(), target.paddingElements()))
					{
						localSucceeded = false;
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

		if (localSucceeded)
		{
			Log::info() << "Validation: succeeded.";
		}
		else
		{
			allSucceeded = false;
			Log::info() << "Validation: FAILED!";
		}
	}

	return allSucceeded;
}

bool TestFrameFilterTemplate::testPixel8BitPerChannel(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration)
{
	ocean_assert(width >= 3u && height >= 3u);
	ocean_assert(channels >= 1u && channels <= 4u);
	ocean_assert(testDuration > 0);

	Log::info() << "Pixel test for " << channels << " channels:";
	Log::info() << " ";

	Frame frame(FrameType(width, height, FrameType::genericPixelFormat(8u, channels), FrameType::ORIGIN_UPPER_LEFT));
	Frame target(frame.frameType());

	ocean_assert(frame.isContinuous());
	ocean_assert(target.isContinuous());

	typedef CV::FrameFilterTemplate<int32_t, -1, 0, 1, -2, 0, 2, -1, 0, 1> IntegerFilterTemplate;
	typedef CV::FrameFilterTemplate<int32_t, -1, 4, 3, -2, -4, 2, 1, 7, -6> FloatFilterTemplate;

	bool allSucceeded = true;

	for (unsigned int nOrientation = 0u; nOrientation < 8u; ++nOrientation)
	{
		Log::info() << "Orientation: " << directionStrings()[nOrientation];

		const Timestamp startTimestamp(true);

		CV::CVUtilities::randomizeFrame(frame);

		do
		{
			for (unsigned int n = 0u; n < 1000u; ++n)
			{
				const unsigned int x = RandomI::random(0u, width - 1u);
				const unsigned int y = RandomI::random(0u, height - 1u);

				if (!validateFilterPixel<IntegerFilterTemplate, int8_t, int32_t, 4, 0>(frame.constdata<uint8_t>(), frame.width(), frame.height(), x, y, channels, directions()[nOrientation]))
				{
					allSucceeded = false;
				}

				if (!validateFilterPixel<IntegerFilterTemplate, uint8_t, int32_t, 4, 0>(frame.constdata<uint8_t>(), frame.width(), frame.height(), x, y, channels, directions()[nOrientation]))
				{
					allSucceeded = false;
				}

				if (!validateFilterPixel<FloatFilterTemplate, float, int32_t, 1, 0>(frame.constdata<uint8_t>(), frame.width(), frame.height(), x, y, channels, directions()[nOrientation]))
				{
					allSucceeded = false;
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
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

template <typename TResponse, typename TNormalization>
bool TestFrameFilterTemplate::validateFilter8BitPerChannel(const uint8_t* frame, const TResponse* filtered, const unsigned int width, const unsigned int height, const unsigned int channels, const int filterPattern[9], const TNormalization normalization, const TNormalization bias, const CV::PixelDirection direction, const unsigned int framePaddingElements, const unsigned int targetPaddingElements)
{
	ocean_assert(frame && filtered);
	ocean_assert(width >= 3u && height >= 3u);

	ocean_assert(normalization != TNormalization(0));

	const unsigned int frameStrideElements = width * channels + framePaddingElements;
	const unsigned int filteredStrideElements = width * channels + targetPaddingElements;

	const int ringFactors[8] = {filterPattern[3], filterPattern[0], filterPattern[1], filterPattern[2], filterPattern[5], filterPattern[8], filterPattern[7], filterPattern[6]};
	const int centerFactor = filterPattern[4];

	unsigned int ringOffset = (unsigned int)(-1);

	switch (direction)
	{
		case CV::PD_NORTH:
			ringOffset = 0u;
			break;

		case CV::PD_NORTH_WEST:
			ringOffset = 7u;
			break;

		case CV::PD_WEST:
			ringOffset = 6u;
			break;

		case CV::PD_SOUTH_WEST:
			ringOffset = 5u;
			break;

		case CV::PD_SOUTH:
			ringOffset = 4u;
			break;

		case CV::PD_SOUTH_EAST:
			ringOffset = 3u;
			break;

		case CV::PD_EAST:
			ringOffset = 2u;
			break;

		case CV::PD_NORTH_EAST:
			ringOffset = 1u;
			break;

		default:
			ocean_assert(false && "Invalid orientation!");
	}

	for (unsigned int y = 1u; y < height - 1u; ++y)
	{
		const uint8_t* frameRowTop = frame + frameStrideElements * (y - 1u);
		const uint8_t* frameRowCenter = frame + frameStrideElements * (y + 0u);
		const uint8_t* frameRowBottom = frame + frameStrideElements * (y + 1u);

		const TResponse* filteredRow = filtered + filteredStrideElements * y;

		for (unsigned int x = 1u; x < width - 1u; ++x)
		{
			const uint8_t* framePtrTop = frameRowTop + x * channels;
			const uint8_t* framePtrCenter = frameRowCenter + x * channels;
			const uint8_t* framePtrBottom = frameRowBottom + x * channels;

			const TResponse* filteredPtr = filteredRow + x * channels;

			for (unsigned int n = 0u; n < channels; ++n)
			{
				const uint8_t index0 = *(framePtrTop + n);
				const uint8_t index1 = *(framePtrTop - channels + n);
				const uint8_t index2 = *(framePtrCenter - channels + n);
				const uint8_t index3 = *(framePtrBottom - channels + n);
				const uint8_t index4 = *(framePtrBottom + n);
				const uint8_t index5 = *(framePtrBottom + channels + n);
				const uint8_t index6 = *(framePtrCenter + channels + n);
				const uint8_t index7 = *(framePtrTop + channels + n);

				const uint8_t indexCenter = *(framePtrCenter + n);

				const TResponse result = TResponse(TNormalization((index0 * ringFactors[(0u + ringOffset) % 8u] + index1 * ringFactors[(1u + ringOffset) % 8u] + index2 * ringFactors[(2u + ringOffset) % 8u]
														+ index3 * ringFactors[(3u + ringOffset) % 8u] + index4 * ringFactors[(4u + ringOffset) % 8u] + index5 * ringFactors[(5u + ringOffset) % 8u]
														+ index6 * ringFactors[(6u + ringOffset) % 8u] + index7 * ringFactors[(7u + ringOffset) % 8u] + indexCenter * centerFactor) + bias) / normalization);

				if (filteredPtr[n] != result)
				{
					return false;
				}
			}
		}
	}

	// top and bottom row
	for (unsigned int x = 0u; x < width; ++x)
	{
		const TResponse* const topRow = filtered;
		const TResponse* const bottomRow = filtered + filteredStrideElements * (height - 1u);

		for (unsigned int n = 0u; n < channels; ++n)
		{
			if (topRow[x * channels + n] != 0 || bottomRow[x * channels + n] != 0)
			{
				return false;
			}
		}
	}

	// left and right columns
	for (unsigned int y = 0u; y < height; ++y)
	{
		const TResponse* const leftColumn = filtered + filteredStrideElements * y;
		const TResponse* const rightColumn = leftColumn + (width - 1u) * channels;

		for (unsigned int n = 0u; n < channels; ++n)
		{
			if (leftColumn[n] != 0 || rightColumn[n] != 0)
			{
				return false;
			}
		}
	}

	return true;
}

template <typename TResponse, typename TNormalizationFactor>
bool TestFrameFilterTemplate::validateFilterWithFactor8BitPerChannel(const uint8_t* frame, const TResponse* filtered, const unsigned int width, const unsigned int height, const unsigned int channels, const int filterPattern[9], const TNormalizationFactor factor, const CV::PixelDirection direction, const unsigned int framePaddingElements, const unsigned int targetPaddingElements)
{
	ocean_assert(frame && filtered);
	ocean_assert(width >= 3u && height >= 3u);

	const unsigned int frameStrideElements = width * channels + framePaddingElements;
	const unsigned int filteredStrideElements = width * channels + targetPaddingElements;

	const int ringFactors[8] = {filterPattern[3], filterPattern[0], filterPattern[1], filterPattern[2], filterPattern[5], filterPattern[8], filterPattern[7], filterPattern[6]};
	const int centerFactor = filterPattern[4];

	unsigned int ringOffset = (unsigned int)(-1);

	switch (direction)
	{
		case CV::PD_NORTH:
			ringOffset = 0u;
			break;

		case CV::PD_NORTH_WEST:
			ringOffset = 7u;
			break;

		case CV::PD_WEST:
			ringOffset = 6u;
			break;

		case CV::PD_SOUTH_WEST:
			ringOffset = 5u;
			break;

		case CV::PD_SOUTH:
			ringOffset = 4u;
			break;

		case CV::PD_SOUTH_EAST:
			ringOffset = 3u;
			break;

		case CV::PD_EAST:
			ringOffset = 2u;
			break;

		case CV::PD_NORTH_EAST:
			ringOffset = 1u;
			break;

		default:
			ocean_assert(false && "Invalid orientation!");
	}

	for (unsigned int y = 1u; y < height - 1u; ++y)
	{
		const uint8_t* frameRowTop = frame + frameStrideElements * (y - 1u);
		const uint8_t* frameRowCenter = frame + frameStrideElements * (y + 0u);
		const uint8_t* frameRowBottom = frame + frameStrideElements * (y + 1u);

		const TResponse* filteredRow = filtered + filteredStrideElements * y;

		for (unsigned int x = 1u; x < width - 1u; ++x)
		{
			const uint8_t* framePtrTop = frameRowTop + x * channels;
			const uint8_t* framePtrCenter = frameRowCenter + x * channels;
			const uint8_t* framePtrBottom = frameRowBottom + x * channels;

			const TResponse* filteredPtr = filteredRow + x * channels;

			for (unsigned int n = 0u; n < channels; ++n)
			{
				const uint8_t index0 = *(framePtrTop + n);
				const uint8_t index1 = *(framePtrTop - channels + n);
				const uint8_t index2 = *(framePtrCenter - channels + n);
				const uint8_t index3 = *(framePtrBottom - channels + n);
				const uint8_t index4 = *(framePtrBottom + n);
				const uint8_t index5 = *(framePtrBottom + channels + n);
				const uint8_t index6 = *(framePtrCenter + channels + n);
				const uint8_t index7 = *(framePtrTop + channels + n);

				const uint8_t indexCenter = *(framePtrCenter + n);

				const TResponse result = TResponse(TNormalizationFactor(index0 * ringFactors[(0u + ringOffset) % 8u] + index1 * ringFactors[(1u + ringOffset) % 8u] + index2 * ringFactors[(2u + ringOffset) % 8u]
														+ index3 * ringFactors[(3u + ringOffset) % 8u] + index4 * ringFactors[(4u + ringOffset) % 8u] + index5 * ringFactors[(5u + ringOffset) % 8u]
														+ index6 * ringFactors[(6u + ringOffset) % 8u] + index7 * ringFactors[(7u + ringOffset) % 8u] + indexCenter * centerFactor) * factor);

				if (NumericT<TResponse>::isNotEqual(filteredPtr[n], result))
				{
					return false;
				}
			}
		}
	}

	// top and bottom row
	for (unsigned int x = 0u; x < width; ++x)
	{
		const TResponse* const topRow = filtered;
		const TResponse* const bottomRow = filtered + filteredStrideElements * (height - 1u);

		for (unsigned int n = 0u; n < channels; ++n)
		{
			if (topRow[x * channels + n] != 0 || bottomRow[x * channels + n] != 0)
			{
				return false;
			}
		}
	}

	// left and right columns
	for (unsigned int y = 0u; y < height; ++y)
	{
		const TResponse* const leftColumn = filtered + filteredStrideElements * y;
		const TResponse* const rightColumn = leftColumn + (width - 1u) * channels;

		for (unsigned int n = 0u; n < channels; ++n)
		{
			if (leftColumn[n] != 0 || rightColumn[n] != 0)
			{
				return false;
			}
		}
	}

	return true;
}

template <typename TFilterTemplate, typename TResponse, typename TNormalization, TNormalization tNormalization, TNormalization tBias>
bool TestFrameFilterTemplate::validateFilterPixel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, const unsigned int channels, const CV::PixelDirection direction)
{
	ocean_assert(frame);
	ocean_assert(width >= 3u && height >= 3u);
	ocean_assert(x < width && y < height);

	switch (channels)
	{
		case 1u:
			return validateFilterPixel<TFilterTemplate, TResponse, TNormalization, tNormalization, tBias, 1u>(frame, width, height, x, y, direction);

		case 2u:
			return validateFilterPixel<TFilterTemplate, TResponse, TNormalization, tNormalization, tBias, 2u>(frame, width, height, x, y, direction);

		case 3u:
			return validateFilterPixel<TFilterTemplate, TResponse, TNormalization, tNormalization, tBias, 3u>(frame, width, height, x, y, direction);

		case 4u:
			return validateFilterPixel<TFilterTemplate, TResponse, TNormalization, tNormalization, tBias, 4u>(frame, width, height, x, y, direction);
	}

	ocean_assert(false && "Invalid channel number!");
	return false;
}

template <typename TFilterTemplate, typename TResponse, typename TNormalization, TNormalization tNormalization, TNormalization tBias, unsigned int tChannels>
bool TestFrameFilterTemplate::validateFilterPixel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, const CV::PixelDirection direction)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(frame);
	ocean_assert(width >= 3u && height >= 3u);
	ocean_assert(x < width && y < height);

	switch (direction)
	{
		case CV::PD_NORTH:
			return validateFilterPixel<TFilterTemplate, TResponse, TNormalization, tNormalization, tBias, tChannels, CV::PD_NORTH>(frame, width, height, x, y);

		case CV::PD_NORTH_WEST:
			return validateFilterPixel<TFilterTemplate, TResponse, TNormalization, tNormalization, tBias, tChannels, CV::PD_NORTH_WEST>(frame, width, height, x, y);

		case CV::PD_WEST:
			return validateFilterPixel<TFilterTemplate, TResponse, TNormalization, tNormalization, tBias, tChannels, CV::PD_WEST>(frame, width, height, x, y);

		case CV::PD_SOUTH_WEST:
			return validateFilterPixel<TFilterTemplate, TResponse, TNormalization, tNormalization, tBias, tChannels, CV::PD_SOUTH_WEST>(frame, width, height, x, y);

		case CV::PD_SOUTH:
			return validateFilterPixel<TFilterTemplate, TResponse, TNormalization, tNormalization, tBias, tChannels, CV::PD_SOUTH>(frame, width, height, x, y);

		case CV::PD_SOUTH_EAST:
			return validateFilterPixel<TFilterTemplate, TResponse, TNormalization, tNormalization, tBias, tChannels, CV::PD_SOUTH_EAST>(frame, width, height, x, y);

		case CV::PD_EAST:
			return validateFilterPixel<TFilterTemplate, TResponse, TNormalization, tNormalization, tBias, tChannels, CV::PD_EAST>(frame, width, height, x, y);

		case CV::PD_NORTH_EAST:
			return validateFilterPixel<TFilterTemplate, TResponse, TNormalization, tNormalization, tBias, tChannels, CV::PD_NORTH_EAST>(frame, width, height, x, y);

		default:
			break;
	}

	ocean_assert(false && "Invalid pixel direction!");
	return false;
}

template <typename TFilterTemplate, typename TResponse, typename TNormalization, TNormalization tNormalization, TNormalization tBias, unsigned int tChannels, CV::PixelDirection tDirection>
bool TestFrameFilterTemplate::validateFilterPixel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(frame);
	ocean_assert(width >= 3u && height >= 3u);
	ocean_assert(x < width && y < height);

	typename TFilterTemplate::Type filters[9];
	TFilterTemplate::copyFilterFactors(filters);

	TResponse testResponse[tChannels];
	determinePixelResponse<TResponse, typename TFilterTemplate::Type>(frame, width, height, x, y, tChannels, filters, tNormalization, tBias, tDirection, testResponse);

	switch (tChannels)
	{
		case 1u:
		{
			const TResponse response0 = TFilterTemplate::template filterPixel<uint8_t, TResponse, TNormalization, tNormalization, tBias, 0u, 1u, tDirection>(frame, width, height, x, y);

			if (response0 != testResponse[0])
				return false;

			break;
		}

		case 2u:
		{
			const TResponse response0 = TFilterTemplate::template filterPixel<uint8_t, TResponse, TNormalization, tNormalization, tBias, 0u, 2u, tDirection>(frame, width, height, x, y);
			const TResponse response1 = TFilterTemplate::template filterPixel<uint8_t, TResponse, TNormalization, tNormalization, tBias, 1u, 2u, tDirection>(frame, width, height, x, y);

			if (response0 != testResponse[0] || response1 != testResponse[1])
				return false;

			break;
		}

		case 3u:
		{
			const TResponse response0 = TFilterTemplate::template filterPixel<uint8_t, TResponse, TNormalization, tNormalization, tBias, 0u, 3u, tDirection>(frame, width, height, x, y);
			const TResponse response1 = TFilterTemplate::template filterPixel<uint8_t, TResponse, TNormalization, tNormalization, tBias, 1u, 3u, tDirection>(frame, width, height, x, y);
			const TResponse response2 = TFilterTemplate::template filterPixel<uint8_t, TResponse, TNormalization, tNormalization, tBias, 2u, 3u, tDirection>(frame, width, height, x, y);

			if (response0 != testResponse[0] || response1 != testResponse[1] || response2 != testResponse[2])
			{
				return false;
			}

			break;
		}

		case 4u:
		{
			const TResponse response0 = TFilterTemplate::template filterPixel<uint8_t, TResponse, TNormalization, tNormalization, tBias, 0u, 4u, tDirection>(frame, width, height, x, y);
			const TResponse response1 = TFilterTemplate::template filterPixel<uint8_t, TResponse, TNormalization, tNormalization, tBias, 1u, 4u, tDirection>(frame, width, height, x, y);
			const TResponse response2 = TFilterTemplate::template filterPixel<uint8_t, TResponse, TNormalization, tNormalization, tBias, 2u, 4u, tDirection>(frame, width, height, x, y);
			const TResponse response3 = TFilterTemplate::template filterPixel<uint8_t, TResponse, TNormalization, tNormalization, tBias, 3u, 4u, tDirection>(frame, width, height, x, y);

			if (response0 != testResponse[0] || response1 != testResponse[1] || response2 != testResponse[2] || response3 != testResponse[3])
			{
				return false;
			}

			break;
		}

		default:
			ocean_assert(false && "Invalid channel number!");
	}

	TResponse response[tChannels];
	TFilterTemplate::template filterPixel<uint8_t, TResponse, TNormalization, tNormalization, tBias, tChannels, tDirection>(frame, width, height, x, y, response);

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		if (response[n] != testResponse[n])
		{
			return false;
		}
	}

	if constexpr (!std::numeric_limits<TResponse>::is_signed || std::is_floating_point<TResponse>::value)
	{
		// we should use unsigned response types or floating point types only

		if (!std::is_floating_point<TResponse>::value)
		{
			// TResponse seems to be 'uint8_t' so we have to determine response for a signed data type larger than char

			int intTestResponse[tChannels];
			determinePixelResponse<int, typename TFilterTemplate::Type>(frame, width, height, x, y, tChannels, filters, tNormalization, tBias, tDirection, intTestResponse);

			unsigned int testMaxAbsResponse = 0;
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				testMaxAbsResponse = max(testMaxAbsResponse, (unsigned int)NumericT<int>::abs(intTestResponse[n]));
			}

			const TResponse maxAbsResponse = TFilterTemplate::template filterAbsoluteAs1ChannelPixel<uint8_t, TResponse, TNormalization, tNormalization, tBias, tChannels, tDirection>(frame, width, height, x, y);

			if ((unsigned int)maxAbsResponse != testMaxAbsResponse)
			{
				return false;
			}
		}
		else
		{
			TResponse testMaxAbsResponse = TResponse(0);
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				testMaxAbsResponse = max(testMaxAbsResponse, NumericT<TResponse>::abs(testResponse[n]));
			}

			const TResponse maxAbsResponse = TFilterTemplate::template filterAbsoluteAs1ChannelPixel<uint8_t, TResponse, TNormalization, tNormalization, tBias, tChannels, tDirection>(frame, width, height, x, y);

			if (maxAbsResponse != testMaxAbsResponse)
			{
				return false;
			}
		}
	}

	return true;
}

template <typename TResponse, typename TNormalization>
void TestFrameFilterTemplate::determinePixelResponse(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, const unsigned int channels, const int factors[9], const TNormalization normalization, const TNormalization bias, const CV::PixelDirection direction, TResponse* response)
{
	ocean_assert(frame);
	ocean_assert(width >= 3u && height >= 3u);

	ocean_assert(normalization != TNormalization(0));

	if (x == 0u || x + 1u == width || y == 0u || y + 1u == height)
	{
		for (unsigned int n = 0u; n < channels; ++n)
		{
			response[n] = TResponse(0);
		}

		return;
	}

	const int ringFactors[8] = {factors[3], factors[0], factors[1], factors[2], factors[5], factors[8], factors[7], factors[6]};
	const int centerFactor = factors[4];

	unsigned int ringOffset = (unsigned int)(-1);

	switch (direction)
	{
		case CV::PD_NORTH:
			ringOffset = 0u;
			break;

		case CV::PD_NORTH_WEST:
			ringOffset = 7u;
			break;

		case CV::PD_WEST:
			ringOffset = 6u;
			break;

		case CV::PD_SOUTH_WEST:
			ringOffset = 5u;
			break;

		case CV::PD_SOUTH:
			ringOffset = 4u;
			break;

		case CV::PD_SOUTH_EAST:
			ringOffset = 3u;
			break;

		case CV::PD_EAST:
			ringOffset = 2u;
			break;

		case CV::PD_NORTH_EAST:
			ringOffset = 1u;
			break;

		default:
			ocean_assert(false && "Invalid orientation!");
	}

	const uint8_t* framePtr = frame + (y * width + x) * channels;

	for (unsigned int n = 0u; n < channels; ++n)
	{
		const uint8_t index0 = *(framePtr - width * channels + n);
		const uint8_t index1 = *(framePtr - width * channels - channels + n);
		const uint8_t index2 = *(framePtr - channels + n);
		const uint8_t index3 = *(framePtr + width * channels - channels + n);
		const uint8_t index4 = *(framePtr + width * channels + n);
		const uint8_t index5 = *(framePtr + width * channels + channels + n);
		const uint8_t index6 = *(framePtr + channels + n);
		const uint8_t index7 = *(framePtr - width * channels + channels + n);

		const uint8_t indexCenter = *(framePtr + n);

		response[n] = TResponse((TNormalization(index0 * ringFactors[(0u + ringOffset) % 8u] + index1 * ringFactors[(1u + ringOffset) % 8u] + index2 * ringFactors[(2u + ringOffset) % 8u]
									+ index3 * ringFactors[(3u + ringOffset) % 8u] + index4 * ringFactors[(4u + ringOffset) % 8u] + index5 * ringFactors[(5u + ringOffset) % 8u]
									+ index6 * ringFactors[(6u + ringOffset) % 8u] + index7 * ringFactors[(7u + ringOffset) % 8u] + indexCenter * centerFactor) + bias) / normalization);
	}
}

const CV::PixelDirections& TestFrameFilterTemplate::directions()
{
	static const CV::PixelDirections result =
	{
		CV::PD_NORTH,
		CV::PD_NORTH_WEST,
		CV::PD_WEST,
		CV::PD_SOUTH_WEST,
		CV::PD_SOUTH,
		CV::PD_SOUTH_EAST,
		CV::PD_EAST,
		CV::PD_NORTH_EAST
	};

	return result;
}

const std::vector<std::string>& TestFrameFilterTemplate::directionStrings()
{
	static const std::vector<std::string> result =
	{
		"0 degree, North",
		"45 degree, North west",
		"90 degree, West",
		"135 degree, South west",
		"180 degree, South",
		"225 degree, South east",
		"270 degree, East",
		"315 degree, North east"
	};

	ocean_assert(result.size() == directions().size());

	return result;
}

}

}

}
