/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameEnlarger.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameEnlarger.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameEnlarger::test(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Frame enlarger test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testAddBorder<uint8_t>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testAddBorder<float>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAddBorderNearestPixel<uint8_t>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testAddBorderNearestPixel<float>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAddBorderMirrored<uint8_t>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testAddBorderMirrored<float>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFrameMultiplyByTwo(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAddTransparentBorder(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Frame enlarger test succeeded.";
	}
	else
	{
		Log::info() << "Frame enlarger test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameEnlarger, AddBorder_uint8)
{
	EXPECT_TRUE((TestFrameEnlarger::testAddBorder<uint8_t>(GTEST_TEST_DURATION)));
}

TEST(TestFrameEnlarger, AddBorder_float)
{
	EXPECT_TRUE((TestFrameEnlarger::testAddBorder<float>(GTEST_TEST_DURATION)));
}


TEST(TestFrameEnlarger, AddBorderNearestPixel_uint8)
{
	EXPECT_TRUE(TestFrameEnlarger::testAddBorderNearestPixel<uint8_t>(GTEST_TEST_DURATION));
}

TEST(TestFrameEnlarger, AddBorderNearestPixel_float)
{
	EXPECT_TRUE(TestFrameEnlarger::testAddBorderNearestPixel<float>(GTEST_TEST_DURATION));
}


TEST(TestFrameEnlarger, AddBorderMirrored_uint8)
{
	EXPECT_TRUE((TestFrameEnlarger::testAddBorderMirrored<uint8_t>(GTEST_TEST_DURATION)));
}

TEST(TestFrameEnlarger, AddBorderMirrored_float)
{
	EXPECT_TRUE((TestFrameEnlarger::testAddBorderMirrored<float>(GTEST_TEST_DURATION)));
}


TEST(TestFrameEnlarger, FrameMultiplyByTwo_640x480_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameEnlarger::testFrameMultiplyByTwo(640u, 480u, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameEnlarger, FrameMultiplyByTwo_640x480_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestFrameEnlarger::testFrameMultiplyByTwo(640u, 480u, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameEnlarger, FrameMultiplyByTwo_640x480_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestFrameEnlarger::testFrameMultiplyByTwo(640u, 480u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameEnlarger, FrameMultiplyByTwo_640x480_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestFrameEnlarger::testFrameMultiplyByTwo(640u, 480u, 4u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameEnlarger, FrameMultiplyByTwo_641x480_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameEnlarger::testFrameMultiplyByTwo(641u, 480u, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameEnlarger, FrameMultiplyByTwo_641x480_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestFrameEnlarger::testFrameMultiplyByTwo(641u, 480u, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameEnlarger, FrameMultiplyByTwo_641x480_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestFrameEnlarger::testFrameMultiplyByTwo(641u, 480u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameEnlarger, FrameMultiplyByTwo_641x480_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestFrameEnlarger::testFrameMultiplyByTwo(641u, 480u, 4u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameEnlarger, FrameMultiplyByTwo_640x481_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameEnlarger::testFrameMultiplyByTwo(640u, 481u, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameEnlarger, FrameMultiplyByTwo_640x481_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestFrameEnlarger::testFrameMultiplyByTwo(640u, 481u, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameEnlarger, FrameMultiplyByTwo_640x481_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestFrameEnlarger::testFrameMultiplyByTwo(640u, 481u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameEnlarger, FrameMultiplyByTwo_640x481_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestFrameEnlarger::testFrameMultiplyByTwo(640u, 481u, 4u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameEnlarger, FrameMultiplyByTwo_641x481_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameEnlarger::testFrameMultiplyByTwo(641u, 481u, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameEnlarger, FrameMultiplyByTwo_641x481_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestFrameEnlarger::testFrameMultiplyByTwo(641u, 481u, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameEnlarger, FrameMultiplyByTwo_641x481_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestFrameEnlarger::testFrameMultiplyByTwo(641u, 481u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameEnlarger, FrameMultiplyByTwo_641x481_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestFrameEnlarger::testFrameMultiplyByTwo(641u, 481u, 4u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameEnlarger, FrameMultiplyByTwo_1920x1080_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameEnlarger::testFrameMultiplyByTwo(1920u, 1080u, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameEnlarger, FrameMultiplyByTwo_1920x1080_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestFrameEnlarger::testFrameMultiplyByTwo(1920u, 1080u, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameEnlarger, FrameMultiplyByTwo_1920x1080_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestFrameEnlarger::testFrameMultiplyByTwo(1920u, 1080u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameEnlarger, FrameMultiplyByTwo_1920x1080_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestFrameEnlarger::testFrameMultiplyByTwo(1920u, 1080u, 4u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameEnlarger, AddTransparentBorder)
{
	EXPECT_TRUE((TestFrameEnlarger::testAddTransparentBorder(GTEST_TEST_DURATION)));
}

#endif // OCEAN_USE_GTEST

template <typename T>
bool TestFrameEnlarger::testAddBorder(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Add border with fixed color '" << TypeNamer::name<T>() << "' test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);
		const unsigned int channels = RandomI::random(randomGenerator, 1u, 4u);

		const unsigned int borderSizeLeft = RandomI::random(randomGenerator, 20u);
		const unsigned int borderSizeTop = RandomI::random(randomGenerator, 20u);
		const unsigned int borderSizeRight = RandomI::random(randomGenerator, 20u);
		const unsigned int borderSizeBottom = RandomI::random(randomGenerator, 20u);

		const unsigned int framePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int enlargedFramePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		Frame frame(FrameType(width, height, FrameType::genericPixelFormat<T>(channels), FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);
		Frame enlargedFrame(FrameType(frame, frame.width() + borderSizeLeft + borderSizeRight, frame.height() + borderSizeTop + borderSizeBottom), enlargedFramePaddingElements);

		CV::CVUtilities::randomizeFrame(frame, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(enlargedFrame, false, &randomGenerator);

		const Frame copyEnlargedFrame(enlargedFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		std::vector<T> color(channels);

		for (T& value : color)
		{
			value = T(RandomI::random(randomGenerator, 0, 255));
		}

		CV::FrameEnlarger::Comfort::addBorder(frame, enlargedFrame, borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, color.data());

		if (!CV::CVUtilities::isPaddingMemoryIdentical(enlargedFrame, copyEnlargedFrame))
		{
			ocean_assert(false && "Invalid padding memory!");
			return false;
		}

		if (!validateAddBorder<T>(frame, enlargedFrame, borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, color.data()))
		{
			allSucceeded = false;
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

template <typename T>
bool TestFrameEnlarger::testAddBorderNearestPixel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Add border with nearest pixel '" << TypeNamer::name<T>() << "' test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);
		const unsigned int channels = RandomI::random(randomGenerator, 1u, 4u);

		const unsigned int borderSizeLeft = RandomI::random(randomGenerator, 20u);
		const unsigned int borderSizeTop = RandomI::random(randomGenerator, 20u);
		const unsigned int borderSizeRight = RandomI::random(randomGenerator, 20u);
		const unsigned int borderSizeBottom = RandomI::random(randomGenerator, 20u);

		const unsigned int framePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int enlargedFramePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		Frame frame(FrameType(width, height, FrameType::genericPixelFormat<T>(channels), FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);
		Frame enlargedFrame(FrameType(frame, frame.width() + borderSizeLeft + borderSizeRight, frame.height() + borderSizeTop + borderSizeBottom), enlargedFramePaddingElements);

		CV::CVUtilities::randomizeFrame(frame, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(enlargedFrame, false, &randomGenerator);

		const Frame copyEnlargedFrame(enlargedFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		CV::FrameEnlarger::Comfort::addBorderNearestPixel(frame, enlargedFrame, borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom);

		if (!CV::CVUtilities::isPaddingMemoryIdentical(enlargedFrame, copyEnlargedFrame))
		{
			ocean_assert(false && "Invalid padding memory!");
			return false;
		}

		if (!validateAddBorderNearestPixel<T>(frame, enlargedFrame, borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom))
		{
			allSucceeded = false;
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

template <typename T>
bool TestFrameEnlarger::testAddBorderMirrored(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Add border with mirroring pixel values test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int borderSizeLeft = RandomI::random(randomGenerator, 20u);
		const unsigned int borderSizeTop = RandomI::random(randomGenerator, 20u);
		const unsigned int borderSizeRight = RandomI::random(randomGenerator, 20u);
		const unsigned int borderSizeBottom = RandomI::random(randomGenerator, 20u);

		const unsigned int width = RandomI::random(randomGenerator, std::max(borderSizeLeft, borderSizeRight), 1920u);
		const unsigned int height = RandomI::random(randomGenerator, std::max(borderSizeTop, borderSizeBottom), 1080u);
		const unsigned int channels = RandomI::random(randomGenerator, 1u, 4u);

		const unsigned int framePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int enlargedFramePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		Frame frame(FrameType(width, height, FrameType::genericPixelFormat<T>(channels), FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);
		Frame enlargedFrame(FrameType(frame, frame.width() + borderSizeLeft + borderSizeRight, frame.height() + borderSizeTop + borderSizeBottom), enlargedFramePaddingElements);

		CV::CVUtilities::randomizeFrame(frame, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(enlargedFrame, false, &randomGenerator);

		const Frame copyEnlargedFrame(enlargedFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		CV::FrameEnlarger::Comfort::addBorderMirrored(frame, enlargedFrame, borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom);

		if (!CV::CVUtilities::isPaddingMemoryIdentical(enlargedFrame, copyEnlargedFrame))
		{
			ocean_assert(false && "Invalid padding memory!");
			return false;
		}

		if (!validateAddBorderMirrored<T>(frame, enlargedFrame, borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom))
		{
			allSucceeded = false;
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

bool TestFrameEnlarger::testFrameMultiplyByTwo(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test multiplication by two ... ";
	Log::info() << " ";

	bool allSucceeded = true;

	const IndexPairs32 frameSizes =
	{
		{640u, 480u},
		{641u, 480u},
		{640u, 481u},
		{641u, 481u},
		{1920u, 1080u},
	};

	for (const IndexPair32& frameSize : frameSizes)
	{
		for (unsigned int channels = 1u; channels <= 4u; ++channels)
		{
			allSucceeded = testFrameMultiplyByTwo(frameSize.first, frameSize.second, channels, testDuration, worker) && allSucceeded;

			Log::info() << " ";
		}

		Log::info() << " ";
	}

	if (allSucceeded)
	{
		Log::info() << "Test: succeeded.";
	}
	else
	{
		Log::info() << "Test: FAILED!";
	}

	return allSucceeded;
}

bool TestFrameEnlarger::testFrameMultiplyByTwo(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(channels >= 1u && channels <= 4u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test multiplication by two for image size " << width << "x" << height << " and " << channels << " channel(s):";

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	RandomGenerator randomGenerator;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	bool allSucceeded = true;

	for (const bool performanceIteration : {true, false})
	{
		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			const Timestamp startTimestamp(true);

			do
			{
				const unsigned int useWidth = performanceIteration ? width : RandomI::random(randomGenerator, 1u, 2048u);
				const unsigned int useHeight = performanceIteration ? height : RandomI::random(randomGenerator, 1u, 2048u);

				const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 1u, 256u) * RandomI::random(randomGenerator, 1u);

				Frame source(FrameType(useWidth, useHeight, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
				Frame target;

				CV::CVUtilities::randomizeFrame(source);

				performance.startIf(performanceIteration);
				CV::FrameEnlarger::Comfort::multiplyByTwo(source, target, useWorker);
				performance.stopIf(performanceIteration);

				allSucceeded = validationMultiplyByTwo(source, target) && allSucceeded;
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

bool TestFrameEnlarger::testAddTransparentBorder(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Add transparent border test:";

	bool allSucceeded = true;

	constexpr size_t numberPixelFormats = 10;
	constexpr FrameType::PixelFormat pixelFormats8BitsPerChannel[numberPixelFormats] =
	{
		FrameType::FORMAT_ABGR32,
		FrameType::FORMAT_ARGB32,
		FrameType::FORMAT_BGR24,
		FrameType::FORMAT_BGRA32,
		FrameType::FORMAT_RGB24,
		FrameType::FORMAT_RGBA32,
		FrameType::FORMAT_Y8,
		FrameType::FORMAT_YA16,
		FrameType::FORMAT_YUV24,
		FrameType::FORMAT_YUVA32,
	};

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);

		// No border in 10% of the cases.
		const bool zeroBorder = RandomI::random(randomGenerator, 9u) == 0u;
		const unsigned int borderSizeLeft = zeroBorder ? 0u : RandomI::random(randomGenerator, 0u, 20u);
		const unsigned int borderSizeTop = zeroBorder ? 0u : RandomI::random(randomGenerator, 0u, 20u);
		const unsigned int borderSizeRight = zeroBorder ? 0u : RandomI::random(randomGenerator, 0u, 20u);
		const unsigned int borderSizeBottom = zeroBorder ? 0u : RandomI::random(randomGenerator, 0u, 20u);

		const unsigned int framePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		const FrameType::PixelFormat pixelFormat = pixelFormats8BitsPerChannel[RandomI::random(randomGenerator, (unsigned int)(numberPixelFormats - 1))];

		Frame frame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);
		Frame enlargedFrame;

		CV::CVUtilities::randomizeFrame(frame, false, &randomGenerator);

		const bool transparentIs0xFF = RandomI::random(randomGenerator, 1u) == 0u;

		if (transparentIs0xFF)
		{
			CV::FrameEnlarger::Comfort::addTransparentBorder<true>(frame, enlargedFrame, borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom);
		}
		else
		{
			CV::FrameEnlarger::Comfort::addTransparentBorder<false>(frame, enlargedFrame, borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom);
		}

		if (!validateAddTransparentBorder(frame, enlargedFrame, borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, transparentIs0xFF))
		{
			allSucceeded = false;
		}
	} while (startTimestamp + testDuration > Timestamp(true));

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
bool TestFrameEnlarger::validateAddBorder(const Frame& original, const Frame& enlarged, const unsigned int borderSizeLeft, const unsigned int borderSizeTop, const unsigned int borderSizeRight, const unsigned int borderSizeBottom, const T* color)
{
	ocean_assert_and_suppress_unused(enlarged.width() > borderSizeLeft + borderSizeRight, borderSizeRight);
	ocean_assert_and_suppress_unused(enlarged.height() > borderSizeTop + borderSizeBottom, borderSizeBottom);

	ocean_assert(original.width() + borderSizeLeft + borderSizeRight == enlarged.width());
	ocean_assert(original.height() + borderSizeTop + borderSizeBottom == enlarged.height());

	ocean_assert(original.pixelFormat() == enlarged.pixelFormat());
	ocean_assert(original.pixelOrigin() == enlarged.pixelOrigin());

	ocean_assert(original.numberPlanes() == 1u);
	ocean_assert(original.dataType() == FrameType::dataType<T>());

	for (unsigned int yEnlarged = 0u; yEnlarged < enlarged.height(); ++yEnlarged)
	{
		const int yOriginal = int(yEnlarged) - int(borderSizeTop);

		for (unsigned int xEnlarged = 0u; xEnlarged < enlarged.width(); ++xEnlarged)
		{
			const int xOriginal = int(xEnlarged) - int(borderSizeLeft);

			if (xOriginal >= 0 && xOriginal < int(original.width()) && yOriginal >= 0 && yOriginal < int(original.height()))
			{
				if (memcmp(enlarged.constpixel<T>(xEnlarged, yEnlarged), original.constpixel<T>(xOriginal, yOriginal), sizeof(T) * original.channels()) != 0)
				{
					return false;
				}
			}
			else
			{
				if (memcmp(enlarged.constpixel<T>(xEnlarged, yEnlarged), color, sizeof(T) * original.channels()) != 0)
				{
					return false;
				}
			}
		}
	}

	return true;
}

template <typename T>
bool TestFrameEnlarger::validateAddBorderNearestPixel(const Frame& original, const Frame& enlarged, const unsigned int borderSizeLeft, const unsigned int borderSizeTop, const unsigned int borderSizeRight, const unsigned int borderSizeBottom)
{
	ocean_assert_and_suppress_unused(enlarged.width() > borderSizeLeft + borderSizeRight, borderSizeRight);
	ocean_assert_and_suppress_unused(enlarged.height() > borderSizeTop + borderSizeBottom, borderSizeBottom);

	ocean_assert(original.width() + borderSizeLeft + borderSizeRight == enlarged.width());
	ocean_assert(original.height() + borderSizeTop + borderSizeBottom == enlarged.height());

	ocean_assert(original.pixelFormat() == enlarged.pixelFormat());
	ocean_assert(original.pixelOrigin() == enlarged.pixelOrigin());

	ocean_assert(original.numberPlanes() == 1u);
	ocean_assert(original.dataType() == FrameType::dataType<T>());

	for (unsigned int yEnlarged = 0u; yEnlarged < enlarged.height(); ++yEnlarged)
	{
		const int yOriginal = minmax<int>(0, int(yEnlarged) - int(borderSizeTop), int(original.height() - 1u));

		for (unsigned int xEnlarged = 0u; xEnlarged < enlarged.width(); ++xEnlarged)
		{
			const int xOriginal = minmax<int>(0, int(xEnlarged) - int(borderSizeLeft), int(original.width() - 1u));

			if (memcmp(enlarged.constpixel<T>(xEnlarged, yEnlarged), original.constpixel<T>(xOriginal, yOriginal), sizeof(T) * original.channels()) != 0)
			{
				return false;
			}
		}
	}

	return true;
}

template <typename T>
bool TestFrameEnlarger::validateAddBorderMirrored(const Frame& original, const Frame& enlarged, const unsigned int borderSizeLeft, const unsigned int borderSizeTop, const unsigned int borderSizeRight, const unsigned int borderSizeBottom)
{
	ocean_assert_and_suppress_unused(enlarged.width() >= borderSizeLeft && enlarged.height() >= borderSizeRight, borderSizeRight);
	ocean_assert_and_suppress_unused(enlarged.height() >= borderSizeTop && enlarged.height() >= borderSizeBottom, borderSizeBottom);

	ocean_assert(original.width() + borderSizeLeft + borderSizeRight == enlarged.width());
	ocean_assert(original.height() + borderSizeTop + borderSizeBottom == enlarged.height());

	ocean_assert(original.pixelFormat() == enlarged.pixelFormat());
	ocean_assert(original.pixelOrigin() == enlarged.pixelOrigin());

	ocean_assert(original.numberPlanes() == 1u);
	ocean_assert(original.dataType() == FrameType::dataType<T>());

	for (unsigned int yEnlarged = 0u; yEnlarged < enlarged.height(); ++yEnlarged)
	{
		// -2 -1 0 1 2 3 4 5 W ...
		//  1  0 0 1 2 3 4 5 5 4 3 2 ...

		const int yOriginalNormal = int(yEnlarged) - int(borderSizeTop);

		unsigned int yOriginal = (unsigned int)(yOriginalNormal);

		if (yOriginalNormal < 0)
		{
			yOriginal = (unsigned int)(abs(yOriginalNormal) - 1);
		}
		else if (yOriginalNormal >= int(original.height()))
		{
			yOriginal = (unsigned int)(int(original.height() * 2u) - yOriginalNormal - 1); // sourceHeight - (syNormal - sourceHeight) - 1
		}

		for (unsigned int xEnlarged = 0u; xEnlarged < enlarged.width(); ++xEnlarged)
		{
			const int xOriginalNormal = int(xEnlarged) - int(borderSizeLeft);

			unsigned int xOriginal = (unsigned int)(xOriginalNormal);

			if (xOriginalNormal < 0)
			{
				xOriginal = (unsigned int)(abs(xOriginalNormal) - 1);
			}
			else if (xOriginalNormal >= int(original.width()))
			{
				xOriginal = (unsigned int)(int(original.width() * 2u) - xOriginalNormal - 1); // sourceWidth - (sxNormal - sourceWidth) - 1
			}

			if (memcmp(enlarged.constpixel<T>(xEnlarged, yEnlarged), original.constpixel<T>(xOriginal, yOriginal), sizeof(T) * original.channels()) != 0)
			{
				return false;
			}
		}
	}

	return true;
}

bool TestFrameEnlarger::validationMultiplyByTwo(const Frame& source, const Frame& target)
{
	ocean_assert(source && target);

	if (source.pixelFormat() != target.pixelFormat())
	{
		return false;
	}

	if (source.pixelOrigin() != target.pixelOrigin())
	{
		return false;
	}

	if (target.width() / 2u != source.width())
	{
		return false;
	}

	if (target.height() / 2u != source.height())
	{
		return false;
	}

	for (unsigned int y = 0u; y < target.height(); ++y)
	{
		for (unsigned int x = 0u; x < target.width(); ++x)
		{
			const unsigned int xSource = min(x / 2u, source.width() - 1u);
			const unsigned int ySource = min(y / 2u, source.height() - 1u);

			const uint8_t* sourcePixel = source.constpixel<uint8_t>(xSource, ySource);
			const uint8_t* targetPixel = target.constpixel<uint8_t>(x, y);

			for (unsigned int n = 0u; n < source.channels(); ++n)
			{
				if (targetPixel[n] != sourcePixel[n])
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool TestFrameEnlarger::validateAddTransparentBorder(const Frame& original, const Frame& enlarged, const unsigned int borderSizeLeft, const unsigned int borderSizeTop, const unsigned int borderSizeRight, const unsigned int borderSizeBottom, const bool transparentIs0xFF)
{
	ocean_assert(enlarged.width() > borderSizeLeft + borderSizeRight);
	ocean_assert(enlarged.height() > borderSizeTop + borderSizeBottom);

	ocean_assert(original.width() + borderSizeLeft + borderSizeRight == enlarged.width());
	ocean_assert(original.height() + borderSizeTop + borderSizeBottom == enlarged.height());

	ocean_assert(enlarged.pixelFormat() == original.pixelFormat() || enlarged.pixelFormat() == FrameType::formatAddAlphaChannel(original.pixelFormat(), /* last channel */ true) || enlarged.pixelFormat() == FrameType::formatAddAlphaChannel(original.pixelFormat(), /* last channel */ false));
	ocean_assert(enlarged.pixelOrigin() == original.pixelOrigin());

	ocean_assert(original.numberPlanes() == 1u);
	ocean_assert(original.dataType() == FrameType::dataType<uint8_t>());

	if (borderSizeTop == 0u && borderSizeBottom == 0u && borderSizeLeft == 0u && borderSizeRight == 0u)
	{
		// No border added: frame type and data should be identical
		if (enlarged.frameType() != original.frameType())
		{
			return false;
		}

		for (unsigned int y = 0u; y < original.height(); ++y)
		{
			const uint8_t* enlargedPtr = enlarged.constrow<uint8_t>(y);
			const uint8_t* originalPtr = original.constrow<uint8_t>(y);

			if (memcmp(enlargedPtr, originalPtr, original.width() * original.channels()) != 0)
			{
				return false;
			}
		}
	}
	else
	{
		bool alphaAtBack = false;
		if (!FrameType::formatHasAlphaChannel(enlarged.pixelFormat(), &alphaAtBack))
		{
			ocean_assert(false && "This should never happen - if a border has been added, an alpha channel must exist!");
			return false;
		}

		const bool originalHasAlphaChannel = FrameType::formatHasAlphaChannel(original.pixelFormat());

		const uint8_t fullTransparency = transparentIs0xFF ? 0xFFu : 0x00u;
		const uint8_t fullOpacity = 0xFFu - fullTransparency;

		const size_t channelsWithAlpha = enlarged.channels();
		const size_t channelsWithoutAlpha = channelsWithAlpha - 1u;
		ocean_assert(channelsWithoutAlpha != 0u && channelsWithoutAlpha <= original.channels());

		const size_t channelSizeWithoutAlpha = size_t(channelsWithoutAlpha);

		const size_t alphaOffset = alphaAtBack ? 0 : 1;
		const size_t alphaChannel = size_t(alphaAtBack ? enlarged.channels() - 1u : 0u);

		for (unsigned int yEnlarged = 0u; yEnlarged < enlarged.height(); ++yEnlarged)
		{
			const unsigned int yOriginal = (unsigned int)minmax<int>(0, int(yEnlarged) - int(borderSizeTop), int(original.height() - 1u));

			for (unsigned int xEnlarged = 0u; xEnlarged < enlarged.width(); ++xEnlarged)
			{
				const unsigned int xOriginal = (unsigned int)minmax<int>(0, int(xEnlarged) - int(borderSizeLeft), int(original.width() - 1u));

				const uint8_t* enlargedPtr = enlarged.constpixel<uint8_t>(xEnlarged, yEnlarged);
				const uint8_t* originalPtr = original.constpixel<uint8_t>(xOriginal, yOriginal);

				uint8_t expectedAlphaValue;

				if (yEnlarged >= borderSizeTop && yEnlarged < borderSizeTop + original.height() && xEnlarged >= borderSizeLeft && xEnlarged < borderSizeLeft + original.width())
				{
					expectedAlphaValue = originalHasAlphaChannel ? originalPtr[alphaChannel] : fullOpacity;
				}
				else
				{
					expectedAlphaValue = fullTransparency;
				}

				if (memcmp(enlargedPtr + alphaOffset, originalPtr + alphaOffset, channelSizeWithoutAlpha) != 0 || enlargedPtr[alphaChannel] != expectedAlphaValue)
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
