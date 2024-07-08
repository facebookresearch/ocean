/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameShrinker.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Utilities.h"

#include "ocean/cv/CVUtilities.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameShrinker::test(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Test Frame Shrinker:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testRowDownsamplingByTwoThreeRows8Bit121(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFrameDownsamplingByTwo8Bit11(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testFrameDownsamplingByTwo8Bit11ExtremeResolutions(worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDownsampleBinayMaskByTwo11(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testDownsampleBinayMaskByTwo11ExtremeResolutions(worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFrameDownsamplingByTwo8Bit14641(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testFrameDownsamplingByTwo8Bit14641ExtremeResolutions(worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPyramidByTwo11(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "FrameShrinker test succeeded.";
	}
	else
	{
		Log::info() << "FrameShrinker test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameShrinker, RowDownsamplingByTwoThreeRows8Bit121)
{
	EXPECT_TRUE(TestFrameShrinker::testRowDownsamplingByTwoThreeRows8Bit121(GTEST_TEST_DURATION));
}


// downsamplingByTwo11

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit11_640x480_320x240_1)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit11(640u, 480u, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit11_640x481_320x240_1)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit11(640u, 481u, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit11_641x480_320x240_1)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit11(641u, 480u, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit11_641x481_320x240_1)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit11(641u, 481u, 1u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit11_640x480_320x240_2)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit11(640u, 480u, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit11_640x481_320x240_2)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit11(640u, 481u, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit11_641x480_320x240_2)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit11(641u, 480u, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit11_641x481_321x241_2)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit11(641u, 481u, 2u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit11_640x480_320x240_3)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit11(640u, 480u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit11_640x481_320x240_3)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit11(640u, 481u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit11_641x480_320x240_3)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit11(641u, 480u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit11_641x481_321x241_3)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit11(641u, 481u, 3u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit11_640x480_320x240_4)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit11(640u, 480u, 4u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit11_640x481_320x240_4)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit11(640u, 481u, 4u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit11_641x480_320x240_4)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit11(641u, 480u, 4u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit11_641x481_321x241_4)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit11(641u, 481u, 4u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit11_640x480_320x240_5)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit11(640u, 480u, 5u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit11_640x481_320x240_5)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit11(640u, 481u, 5u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit11_641x480_320x240_5)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit11(641u, 480u, 5u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit11_641x481_321x241_5)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit11(641u, 481u, 5u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit11ExtremeResolutions)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit11ExtremeResolutions(worker));
}


// downsampleBinayMaskByTwo11

TEST(TestFrameShrinker, DownsampleBinayMaskByTwo11_640x480)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testDownsampleBinayMaskByTwo11(640u, 480u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, DownsampleBinayMaskByTwo11_640x481)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testDownsampleBinayMaskByTwo11(640u, 481u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, DownsampleBinayMaskByTwo11_641x480)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testDownsampleBinayMaskByTwo11(641u, 480u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, DownsampleBinayMaskByTwo11_641x481)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testDownsampleBinayMaskByTwo11(641u, 481u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameShrinker, DownsampleBinayMaskByTwo11ExtremeResolutions)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testDownsampleBinayMaskByTwo11ExtremeResolutions(worker));
}


// downsamplingByTwo14641

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit14641_640x480_320x240_1)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit14641(640u, 480u, 320u, 240u, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit14641_641x481_320x240_1)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit14641(641u, 481u, 320u, 240u, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit14641_641x481_321x241_1)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit14641(641u, 481u, 321u, 241u, 1u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit14641_640x480_320x240_2)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit14641(640u, 480u, 320u, 240u, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit14641_641x481_320x240_2)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit14641(641u, 481u, 320u, 240u, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit14641_641x481_321x241_2)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit14641(641u, 481u, 321u, 241u, 2u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit14641_640x480_320x240_3)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit14641(640u, 480u, 320u, 240u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit14641_641x481_320x240_3)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit14641(641u, 481u, 320u, 240u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit14641_641x481_321x241_3)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit14641(641u, 481u, 321u, 241u, 3u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit14641_640x480_320x240_4)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit14641(640u, 480u, 320u, 240u, 4u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit14641_641x481_320x240_4)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit14641(641u, 481u, 320u, 240u, 4u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit14641_641x481_321x241_4)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit14641(641u, 481u, 321u, 241u, 4u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit14641_640x480_320x240_5)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit14641(640u, 480u, 320u, 240u, 5u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit14641_641x481_320x240_5)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit14641(641u, 481u, 320u, 240u, 5u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit14641_641x481_321x241_5)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit14641(641u, 481u, 321u, 241u, 5u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameShrinker, FrameDownsamplingByTwo8Bit14641ExtremeResolutions)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testFrameDownsamplingByTwo8Bit14641ExtremeResolutions(worker));
}


TEST(TestFrameShrinker, PyramidByTwo11)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinker::testPyramidByTwo11(GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameShrinker::testRowDownsamplingByTwoThreeRows8Bit121(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing downsampling three rows (by two) with 121 filtering:";

	bool allSucceeded = true;

	Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 1000u);
		const unsigned int width_2 = std::max(1u, width / 2u);

		const unsigned int channels = RandomI::random(1u, 5u);

		const Frame sourceRows = CV::CVUtilities::randomizedFrame(FrameType(width, 3u, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT));
		Frame targetRow = CV::CVUtilities::randomizedFrame(FrameType(width_2, 1u, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT));

		const Frame copyTargetRow(targetRow, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		CV::FrameShrinker::downsampleByTwoThreeRows8BitPerChannel121(sourceRows.constdata<uint8_t>(), targetRow.data<uint8_t>(), sourceRows.width(), sourceRows.channels(), sourceRows.strideElements());

		if (!CV::CVUtilities::isPaddingMemoryIdentical(targetRow, copyTargetRow))
		{
			ocean_assert(false && "Invalid padding memory!");
			return false;
		}

		for (unsigned int xTarget = 0u; xTarget < targetRow.width(); ++xTarget)
		{
			const unsigned int xSource = xTarget * 2u;

			const bool applyDownsampling3x3 = xTarget + 1u == targetRow.width() && sourceRows.width() % 2u != 0u; // potentially special case for last target pixel

			const uint8_t* target = targetRow.constpixel<uint8_t>(xTarget, 0u);

			for (unsigned int channelIndex = 0u; channelIndex < channels; ++channelIndex)
			{
				uint32_t result = uint32_t(-1);

				if (width == 1u)
				{
					// 1
					// 2
					// 1

					const uint32_t    topRow = uint32_t(sourceRows.constpixel<uint8_t>(xSource + 0u, 0u)[channelIndex]) * 1u;
					const uint32_t centerRow = uint32_t(sourceRows.constpixel<uint8_t>(xSource + 0u, 1u)[channelIndex]) * 2u;
					const uint32_t bottomRow = uint32_t(sourceRows.constpixel<uint8_t>(xSource + 0u, 2u)[channelIndex]) * 1u;

					result = (topRow + centerRow + bottomRow + 2u) / 4u;
				}
				else if (applyDownsampling3x3)
				{
					// 1 2 1
					// 2 4 2
					// 1 2 1

					const uint32_t    topRow = uint32_t(sourceRows.constpixel<uint8_t>(xSource + 0u, 0u)[channelIndex]) * 1u + uint32_t(sourceRows.constpixel<uint8_t>(xSource + 1u, 0u)[channelIndex]) * 2u + uint32_t(sourceRows.constpixel<uint8_t>(xSource + 2u, 0u)[channelIndex]) * 1u;
					const uint32_t centerRow = uint32_t(sourceRows.constpixel<uint8_t>(xSource + 0u, 1u)[channelIndex]) * 2u + uint32_t(sourceRows.constpixel<uint8_t>(xSource + 1u, 1u)[channelIndex]) * 4u + uint32_t(sourceRows.constpixel<uint8_t>(xSource + 2u, 1u)[channelIndex]) * 2u;
					const uint32_t bottomRow = uint32_t(sourceRows.constpixel<uint8_t>(xSource + 0u, 2u)[channelIndex]) * 1u + uint32_t(sourceRows.constpixel<uint8_t>(xSource + 1u, 2u)[channelIndex]) * 2u + uint32_t(sourceRows.constpixel<uint8_t>(xSource + 2u, 2u)[channelIndex]) * 1u;

					result = (topRow + centerRow + bottomRow + 8u) / 16u;
				}
				else
				{
					// 1 1
					// 2 2
					// 1 1

					const uint32_t    topRow = uint32_t(sourceRows.constpixel<uint8_t>(xSource + 0u, 0u)[channelIndex]) * 1u + uint32_t(sourceRows.constpixel<uint8_t>(xSource + 1u, 0u)[channelIndex]) * 1u;
					const uint32_t centerRow = uint32_t(sourceRows.constpixel<uint8_t>(xSource + 0u, 1u)[channelIndex]) * 2u + uint32_t(sourceRows.constpixel<uint8_t>(xSource + 1u, 1u)[channelIndex]) * 2u;
					const uint32_t bottomRow = uint32_t(sourceRows.constpixel<uint8_t>(xSource + 0u, 2u)[channelIndex]) * 1u + uint32_t(sourceRows.constpixel<uint8_t>(xSource + 1u, 2u)[channelIndex]) * 1u;

					result = (topRow + centerRow + bottomRow + 4u) / 8u;
				}

				if (result != uint32_t(target[channelIndex]))
				{
					allSucceeded = false;
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Verification: succeeded.";
	}
	else
	{
		Log::info() << "Verification: FAILED!";
	}

	return allSucceeded;
}

bool TestFrameShrinker::testFrameDownsamplingByTwo8Bit11(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing downsampling (by two) with 11 filtering:";
	Log::info() << " ";

	bool allSucceeded = true;

	const unsigned int sourceWidths[] = {640u, 641u, 640u, 641u, 800u, 1024u, 1920u, 3840u};
	const unsigned int sourceHeights[] = {480u, 480u, 481u, 481u, 640u, 512u, 1080u, 2160u};

	for (unsigned int n = 0u; n < sizeof(sourceWidths) / sizeof(sourceWidths[0]); ++n)
	{
		const unsigned int sourceWidth = sourceWidths[n];
		const unsigned int sourceHeight = sourceHeights[n];

		const unsigned int targetWidth = sourceWidth / 2u;
		const unsigned int targetHeight = sourceHeight / 2u;

		Log::info() << "Testing 8 bit frame with size " << sourceWidth << "x" << sourceHeight << " -> " << targetWidth << "x" << targetHeight << ":";

		for (unsigned int channels = 1u; channels <= 4u; ++channels)
		{
			Log::info() << " ";

			allSucceeded = testFrameDownsamplingByTwo8Bit11(sourceWidth, sourceHeight, channels, testDuration, worker) && allSucceeded;
		}

		Log::info() << " ";
	}

	if (allSucceeded)
	{
		Log::info() << "Downsampling test with 11 filtering succeeded.";
	}
	else
	{
		Log::info() << "Downsampling test with 11 filtering FAILED!";
	}

	return allSucceeded;
}

bool TestFrameShrinker::testFrameDownsamplingByTwo8Bit11ExtremeResolutions(Worker& worker)
{
	Log::info() << "Testing downsampling (by two) with 11 filtering for extreme frame resolutions:";

	bool allSucceeded = true;

	for (unsigned int channels = 1u; channels <= 4u; ++channels)
	{
		for (unsigned int sourceWidth = 2u; sourceWidth < 64u; ++sourceWidth)
		{
			for (unsigned int sourceHeight = 2u; sourceHeight < 64u; ++sourceHeight)
			{
				allSucceeded = testFrameDownsamplingByTwo8Bit11(sourceWidth, sourceHeight, channels, NumericD::eps(), worker) && allSucceeded;
			}
		}

		const unsigned int sourceWidth = RandomI::random(2u, 1920u);
		const unsigned int sourceHeight = RandomI::random(2u, 1080u);

		allSucceeded = testFrameDownsamplingByTwo8Bit11(sourceWidth, sourceHeight, channels, NumericD::eps(), worker) && allSucceeded;
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

bool TestFrameShrinker::testFrameDownsamplingByTwo8Bit14641(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing downsampling (by two) with 14641 filtering:";
	Log::info() << " ";

	bool allSucceeded = true;

	const unsigned int sourceWidths[] = {640u, 641u, 640u, 641u, 800u, 1024u, 1920u, 3840u};
	const unsigned int sourceHeights[] = {480u, 480u, 481u, 481u, 640u, 512u, 1080u, 2160u};

	const unsigned int targetWidths[] = {320u, 321u, 320u, 321u, 400u, 512u, 960u, 1920u};
	const unsigned int targetHeights[] = {240u, 240u, 241u, 241u, 320u, 256u, 540u, 1080u};

	for (unsigned int n = 0u; n < sizeof(sourceWidths) / sizeof(sourceWidths[0]); ++n)
	{
		const unsigned int sourceWidth = sourceWidths[n];
		const unsigned int sourceHeight = sourceHeights[n];

		const unsigned int targetWidth = targetWidths[n];
		const unsigned int targetHeight = targetHeights[n];

		Log::info().newLine(n != 0u);
		Log::info() << "Testing 8 bit frame with size " << sourceWidth << "x" << sourceHeight << " -> " << targetWidth << "x" << targetHeight << ":";

		for (unsigned int channels = 1u; channels <= 4u; ++channels)
		{
			Log::info() << " ";

			allSucceeded = testFrameDownsamplingByTwo8Bit14641(sourceWidth, sourceHeight, targetWidth, targetHeight, channels, testDuration, worker) && allSucceeded;
		}

		Log::info() << " ";
	}

	if (allSucceeded)
	{
		Log::info() << "Downsampling test with 14641 filtering succeeded.";
	}
	else
	{
		Log::info() << "Downsampling test with 14641 filtering FAILED!";
	}

	return allSucceeded;
}

bool TestFrameShrinker::testFrameDownsamplingByTwo8Bit14641ExtremeResolutions(Worker& worker)
{
	Log::info() << "Testing downsampling (by two) with 14641 filtering for extreme frame resolutions:";

	bool allSucceeded = true;

	for (unsigned int channels = 1u; channels <= 4u; ++channels)
	{
		for (unsigned int sourceWidth = 2u; sourceWidth < 64u; ++sourceWidth)
		{
			for (unsigned int sourceHeight = 2u; sourceHeight < 64u; ++sourceHeight)
			{
				for (unsigned int oX = 0u; oX < 2u; ++oX)
				{
					const unsigned int targetWidthStrict = (sourceWidth) / 2u;
					const unsigned int targetWidth = (sourceWidth + oX) / 2u;

					if (oX != 0u && targetWidthStrict == targetWidth)
					{
						// we have tested this resolution already
						continue;
					}

					for (unsigned int oY = 0u; oY < 2u; ++oY)
					{
						const unsigned int targetHeightStrict = (sourceHeight) / 2u;
						const unsigned int targetHeight = (sourceHeight + oY) / 2u;

						if (oY != 0u && targetHeightStrict == targetHeight)
						{
							// we have tested this resolution already
							continue;
						}

						allSucceeded = testFrameDownsamplingByTwo8Bit14641(sourceWidth, sourceHeight, targetWidth, targetHeight, channels, NumericD::eps(), worker) && allSucceeded;
					}
				}
			}
		}

		const unsigned int sourceWidth = RandomI::random(2u, 1920u);
		const unsigned int sourceHeight = RandomI::random(2u, 1080u);

		const unsigned int targetWidth = (sourceWidth + RandomI::random(1u)) / 2u;
		const unsigned int targetHeight = (sourceHeight + RandomI::random(1u)) / 2u;

		allSucceeded = testFrameDownsamplingByTwo8Bit14641(sourceWidth, sourceHeight, targetWidth, targetHeight, channels, NumericD::eps(), worker) && allSucceeded;
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

bool TestFrameShrinker::testDownsampleBinayMaskByTwo11(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing downsampling binary mask (by two) with 11 filtering:";
	Log::info() << " ";

	bool allSucceeded = true;

	const unsigned int sourceWidths[] = {640u, 641u, 640u, 641u, 800u, 1024u, 1920u, 3840u};
	const unsigned int sourceHeights[] = {480u, 480u, 481u, 481u, 640u, 512u, 1080u, 2160u};

	for (unsigned int n = 0u; n < sizeof(sourceWidths) / sizeof(sourceWidths[0]); ++n)
	{
		const unsigned int sourceWidth = sourceWidths[n];
		const unsigned int sourceHeight = sourceHeights[n];

		const unsigned int targetWidth = sourceWidth / 2u;
		const unsigned int targetHeight = sourceHeight / 2u;

		Log::info().newLine(n != 0u);
		Log::info() << "Testing binary frame with size " << sourceWidth << "x" << sourceHeight << " -> " << targetWidth << "x" << targetHeight << ":";
		Log::info() << " ";

		allSucceeded = testDownsampleBinayMaskByTwo11(sourceWidth, sourceHeight, testDuration, worker) && allSucceeded;

		Log::info() << " ";
	}

	if (allSucceeded)
	{
		Log::info() << "Downsampling test with 11 filtering succeeded.";
	}
	else
	{
		Log::info() << "Downsampling test with 11 filtering FAILED!";
	}

	return allSucceeded;
}

bool TestFrameShrinker::testDownsampleBinayMaskByTwo11ExtremeResolutions(Worker& worker)
{
	Log::info() << "Testing binary downsampling (by two) with 11 filtering for extreme frame resolutions:";

	bool allSucceeded = true;

	for (unsigned int sourceWidth = 2u; sourceWidth < 64u; ++sourceWidth)
	{
		for (unsigned int sourceHeight = 2u; sourceHeight < 64u; ++sourceHeight)
		{
			allSucceeded = testDownsampleBinayMaskByTwo11(sourceWidth, sourceHeight, NumericD::eps(), worker) && allSucceeded;
		}
	}

	const unsigned int sourceWidth = RandomI::random(2u, 1920u);
	const unsigned int sourceHeight = RandomI::random(2u, 1080u);

	allSucceeded = testDownsampleBinayMaskByTwo11(sourceWidth, sourceHeight, NumericD::eps(), worker) && allSucceeded;

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

bool TestFrameShrinker::testPyramidByTwo11(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing pyramid downsampling with 11 filtering:";
	Log::info() << " ";

	constexpr unsigned int frameWidth = 1920u;
	constexpr unsigned int frameHeight = 1080u;

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (const unsigned int channels : {1u, 2u, 3u, 4u})
	{
		Log::info() << frameWidth << "x" << frameHeight << ", " << channels << " channels:";

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			Timestamp startTimestamp(true);

			do
			{
				for (const bool performanceIteration : {true, false})
				{
					const unsigned int width = performanceIteration ? frameWidth : RandomI::random(randomGenerator, 1u, 2000u);
					const unsigned int height = performanceIteration ? frameHeight : RandomI::random(randomGenerator, 1u, 2000u);

					unsigned int maximalLayers = (unsigned int)(-1);

					if (!performanceIteration && RandomI::random(randomGenerator, 1u) == 0u)
					{
						maximalLayers = RandomI::random(randomGenerator, 1u, 10u);
					}

					const bool copyFirstLayer = performanceIteration || RandomI::random(randomGenerator, 1u) == 0u;

					unsigned int pyramidPixels = 0u;

					unsigned int layerWidth = width;
					unsigned int layerHeight = height;
					unsigned int layers = 0u;

					while (layerWidth != 0u && layerHeight != 0u && layers < maximalLayers)
					{
						++layers;
						pyramidPixels += layerWidth * layerHeight;

						layerWidth /= 2u;
						layerHeight /= 2u;
					}

					if (!copyFirstLayer)
					{
						// we need to remove the memory for the finest layer again

						ocean_assert(layerWidth * layerHeight <= pyramidPixels);
						pyramidPixels -= width * height;
					}

					ocean_assert(layers >= 1u);
					ocean_assert(pyramidPixels <= width * height * 134u / 100u); // should not exceed 133%

					const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
					Frame pyramidMemory = CV::CVUtilities::randomizedFrame(FrameType(frame, pyramidPixels, 1u), &randomGenerator);

					const Frame pyramidMemoryCopy(pyramidMemory, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					ocean_assert(!performanceIteration || maximalLayers == (unsigned int)(-1));
					ocean_assert(!performanceIteration || copyFirstLayer);

					performance.startIf(performanceIteration);
						const bool result = CV::FrameShrinker::pyramidByTwo11(frame, pyramidMemory.data<uint8_t>(), pyramidMemory.size(), maximalLayers, copyFirstLayer, useWorker);
					performance.stopIf(performanceIteration);

					if (layers != 1u || copyFirstLayer)
					{
						if (!CV::CVUtilities::isPaddingMemoryIdentical(pyramidMemory, pyramidMemoryCopy))
						{
							ocean_assert(false && "This must never happen!");
							return false;
						}
					}
					else
					{
						ocean_assert(!pyramidMemory.isValid());
					}

					if (!result)
					{
						allSucceeded = false;
					}

					if (copyFirstLayer)
					{
						// verify first layer

						for (unsigned int y = 0u; y < frame.height(); ++y)
						{
							if (memcmp(frame.constrow<uint8_t>(y), pyramidMemory.constdata<uint8_t>() + y * frame.planeWidthBytes(0u), frame.planeWidthBytes(0u)) != 0)
							{
								allSucceeded = false;
							}
						}

						// verify the following layers

						layerWidth = width;
						layerHeight = height;

						const uint8_t* finerLayer = pyramidMemory.constdata<uint8_t>();

						for (unsigned int layerIndex = 1u; layerIndex < layers; ++layerIndex)
						{
							ocean_assert(layerWidth >= 2u || layerHeight >= 2u);

							const uint8_t* const coarserLayer = finerLayer + layerWidth * layerHeight * channels;

							constexpr unsigned int finerLayerPaddingElements = 0u;
							constexpr unsigned int coarserLayerPaddingElements = 0u;

							const unsigned int coarserLayerWidth = layerWidth / 2u;
							const unsigned int coarserLayerHeight = layerHeight / 2u;

							const Frame sourceLayer(FrameType(frame, layerWidth, layerHeight), finerLayer, Frame::CM_USE_KEEP_LAYOUT, finerLayerPaddingElements);
							const Frame targetLayer(FrameType(frame, coarserLayerWidth, coarserLayerHeight), coarserLayer, Frame::CM_USE_KEEP_LAYOUT, coarserLayerPaddingElements);

							double averageAbsError = NumericD::maxValue();
							double maximalAbsError = NumericD::maxValue();
							if (!validateDownsamplingByTwo8Bit11(sourceLayer, targetLayer, &averageAbsError, &maximalAbsError))
							{
								allSucceeded = false;
							}

							if (averageAbsError > 5.0 || maximalAbsError > 5.0)
							{
								allSucceeded = false;
							}

							layerWidth = coarserLayerWidth;
							layerHeight = coarserLayerHeight;

							finerLayer = coarserLayer;
						}
					}
					else
					{
						layerWidth = width;
						layerHeight = height;

						const uint8_t* finerLayer = frame.constdata<uint8_t>();
						unsigned int finerLayerPaddingElements = frame.paddingElements();

						const uint8_t* coarserLayer = pyramidMemory.constdata<uint8_t>();

						for (unsigned int layerIndex = 1u; layerIndex < layers; ++layerIndex)
						{
							ocean_assert(layerWidth >= 2u || layerHeight >= 2u);

							constexpr unsigned int coarserLayerPaddingElements = 0u;

							const unsigned int coarserLayerWidth = layerWidth / 2u;
							const unsigned int coarserLayerHeight = layerHeight / 2u;

							const Frame sourceLayer(FrameType(frame, layerWidth, layerHeight), finerLayer, Frame::CM_USE_KEEP_LAYOUT, finerLayerPaddingElements);
							const Frame targetLayer(FrameType(frame, coarserLayerWidth, coarserLayerHeight), coarserLayer, Frame::CM_USE_KEEP_LAYOUT, coarserLayerPaddingElements);

							double averageAbsError = NumericD::maxValue();
							double maximalAbsError = NumericD::maxValue();
							if (!validateDownsamplingByTwo8Bit11(sourceLayer, targetLayer, &averageAbsError, &maximalAbsError))
							{
								allSucceeded = false;
							}

							if (averageAbsError > 5.0 || maximalAbsError > 5.0)
							{
								allSucceeded = false;
							}

							layerWidth = coarserLayerWidth;
							layerHeight = coarserLayerHeight;

							finerLayer = coarserLayer;
							coarserLayer += layerWidth * layerHeight * channels;

							finerLayerPaddingElements = 0u; // all remaining layers don't have padding elements
						}
					}
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		Log::info() << "Singlecore performance: Best: " << performanceSinglecore.bestMseconds() << "ms, worst: " << performanceSinglecore.worstMseconds() << "ms, average: " << performanceSinglecore.averageMseconds() << "ms, median: " << performanceSinglecore.medianMseconds() << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms, median: " << performanceMulticore.medianMseconds() << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x, average: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 1u) << "x";
		}

		Log::info() << " ";
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrameShrinker::testFrameDownsamplingByTwo8Bit11(const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(sourceWidth >= 2u && sourceHeight >= 2u);
	ocean_assert(channels >= 1u);
	ocean_assert(testDuration > 0.0);

	const unsigned int targetWidth = sourceWidth / 2u;
	const unsigned int targetHeight = sourceHeight / 2u;

	const bool textOutput = sourceWidth >= 64u && testDuration > NumericD::eps();

	if (textOutput)
	{
		Log::info() << ".... with " << channels << " channels:";
	}

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	double sumAverageError = 0.0;
	double maximalError = 0u;

	uint64_t measurements = 0ull;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		Timestamp startTimestamp(true);

		do
		{
			const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceWidth, sourceHeight, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels), FrameType::ORIGIN_UPPER_LEFT));
			Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceFrame, targetWidth, targetHeight));

			const Frame targetFrameCopy(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			performance.start();
				CV::FrameShrinker::downsampleByTwo8BitPerChannel11(sourceFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), channels, sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
			performance.stop();

			double averageAbsError = NumericD::maxValue();
			double maximalAbsError = NumericD::maxValue();
			validateDownsamplingByTwo8Bit11(sourceFrame, targetFrame, &averageAbsError, &maximalAbsError);

			if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, targetFrameCopy))
			{
				ocean_assert(false && "This must never happen!");
				maximalAbsError = NumericD::maxValue();
			}

			sumAverageError += averageAbsError;
			maximalError = max(maximalError, maximalAbsError);
			measurements++;
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	if (textOutput)
	{
		Log::info() << "Singlecore performance: Best: " << performanceSinglecore.bestMseconds() << "ms, worst: " << performanceSinglecore.worstMseconds() << "ms, average: " << performanceSinglecore.averageMseconds() << "ms, median: " << performanceSinglecore.medianMseconds() << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms, median: " << performanceMulticore.medianMseconds() << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x, average: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 1u) << "x";
		}
	}

	const double averageErrorThreshold = textOutput ? 0.5 : 1.0;
	const double maximalErrorThreshold = 1.0;

	ocean_assert(measurements != 0ull);
	const double averageAbsError = sumAverageError / double(measurements);

	const bool allSucceeded = averageAbsError <= averageErrorThreshold && maximalError <= maximalErrorThreshold;

	if (textOutput)
	{
		Log::info() << "Validation: average error: " << String::toAString(averageAbsError, 2u) << ", maximal error: " << String::toAString(maximalError, 2u);

		if (!allSucceeded)
		{
			Log::info() << "Validation: FAILED!";
		}
	}

	return allSucceeded;
}

bool TestFrameShrinker::testDownsampleBinayMaskByTwo11(const unsigned int sourceWidth, const unsigned int sourceHeight, const double testDuration, Worker& worker)
{
	ocean_assert(sourceWidth >= 2u && sourceHeight >= 2u && testDuration > 0.0);

	const unsigned int targetWidth = sourceWidth / 2u;
	const unsigned int targetHeight = sourceHeight / 2u;

	const bool textOutput = sourceWidth >= 64u && testDuration > NumericD::eps();

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
			Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceWidth, sourceHeight, FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
			Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceFrame, targetWidth, targetHeight), &randomGenerator);

			const Frame targetFrameCopy(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			for (unsigned int y = 0u; y < sourceFrame.height(); ++y)
			{
				for (unsigned int x = 0u; x < sourceFrame.width(); ++x)
				{
					sourceFrame.pixel<uint8_t>(x, y)[0] = uint8_t(255u * RandomI::random(randomGenerator, 1u));
				}
			}

			performance.startIf(textOutput);
				CV::FrameShrinker::downsampleBinayMaskByTwo8BitPerChannel11(sourceFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), 766u, useWorker);
			performance.stopIf(textOutput);

			if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, targetFrameCopy))
			{
				ocean_assert(false && "Invalid padding memory!");
				return false;
			}

			allSucceeded = validateDownsampleBinayMaskByTwo11(sourceFrame.constdata<uint8_t>(), targetFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), 766u) && allSucceeded;
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	if (textOutput)
	{
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
	}

	return allSucceeded;
}

bool TestFrameShrinker::testFrameDownsamplingByTwo8Bit14641(const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(sourceWidth >= 2u && sourceHeight >= 2u);

	ocean_assert(sourceWidth / 2u == targetWidth || (sourceWidth + 1u) / 2u == targetWidth);
	ocean_assert(sourceHeight / 2u == targetHeight || (sourceHeight + 1u) / 2u == targetHeight);

	ocean_assert(channels >= 1u);
	ocean_assert(testDuration > 0.0);

	const bool textOutput = sourceWidth >= 64u && testDuration > NumericD::eps();

	if (textOutput)
	{
		Log::info() << ".... with " << channels << " channels:";
	}

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	double sumAverageError = 0.0;
	double maximalError = 0u;

	uint64_t measurements = 0ull;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		Timestamp startTimestamp(true);

		do
		{
			const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceWidth, sourceHeight, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels), FrameType::ORIGIN_UPPER_LEFT));
			Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceFrame, targetWidth, targetHeight));

			const Frame targetFrameCopy(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			performance.start();
				CV::FrameShrinker::downsampleByTwo8BitPerChannel14641(sourceFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), channels, sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
			performance.stop();

			double averageAbsError = NumericD::maxValue();
			double maximalAbsError = NumericD::maxValue();
			validateDownsamplingByTwo8Bit14641(sourceFrame.constdata<uint8_t>(), targetFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), channels, sourceFrame.paddingElements(), targetFrame.paddingElements(), &averageAbsError, &maximalAbsError);

			if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, targetFrameCopy))
			{
				ocean_assert(false && "This must never happen!");
				maximalAbsError = NumericD::maxValue();
			}

			sumAverageError += averageAbsError;
			maximalError = max(maximalError, maximalAbsError);
			measurements++;
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	if (textOutput)
	{
		Log::info() << "Singlecore performance: Best: " << performanceSinglecore.bestMseconds() << "ms, worst: " << performanceSinglecore.worstMseconds() << "ms, average: " << performanceSinglecore.averageMseconds() << "ms, median: " << performanceSinglecore.medianMseconds() << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms, median: " << performanceMulticore.medianMseconds() << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x, average: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 1u) << "x";
		}
	}

	const double averageErrorThreshold = 0.1;
	const double maximalErrorThreshold = 1.0;

	ocean_assert(measurements != 0ull);
	const double averageAbsError = sumAverageError / double(measurements);

	const bool allSucceeded = averageAbsError <= averageErrorThreshold && maximalError <= maximalErrorThreshold;

	if (textOutput)
	{
		Log::info() << "Validation: average error: " << String::toAString(averageAbsError, 2u) << ", maximal error: " << String::toAString(maximalError, 2u);

		if (!allSucceeded)
		{
			Log::info() << "Validation: FAILED!";
		}
	}

	return allSucceeded;
}

bool TestFrameShrinker::validateDownsamplingByTwo8Bit11(const Frame& source, const Frame& target, double* averageAbsError, double* maximalAbsError, uint8_t* groundTruth, const unsigned int groundTruthPaddingElements)
{
	ocean_assert(source.isValid() && target.isValid());
	ocean_assert(source.width() >= 2u && source.height() >= 2u);
	ocean_assert(source.numberPlanes() == 1u && source.channels() >= 1u && source.dataType() == Frame::DT_UNSIGNED_INTEGER_8);
	ocean_assert(source.isPixelFormatCompatible(target.pixelFormat()));

	if (!source.isValid() || source.numberPlanes() != 1u || source.dataType() != Frame::DT_UNSIGNED_INTEGER_8)
	{
		return false;
	}

	const unsigned int targetWidth = source.width() / 2u;
	const unsigned int targetHeight = source.height() / 2u;
	ocean_assert(targetWidth >= 1u && targetHeight >= 1u);

	if (targetWidth != target.width() || targetHeight != target.height())
	{
		return false;
	}

	const unsigned int channels = source.channels();

	const unsigned int groundTruthStrideElements = targetWidth * channels + groundTruthPaddingElements;

	const unsigned int targetCoreWidth = targetWidth - (source.width() & 0x01u);
	const unsigned int targetCoreHeight = targetHeight - (source.height() & 0x01u);

	std::vector<uint8_t> resultValues(channels);

	if (averageAbsError)
	{
		*averageAbsError = NumericD::maxValue();
	}

	if (maximalAbsError)
	{
		*maximalAbsError = NumericD::maxValue();
	}

	double sumAbsError = 0.0;
	double maxAbsError = 0.0;
	uint64_t measurements = 0ull;

	for (unsigned int tY = 0u, sY = 0u; tY < targetHeight; ++tY, sY += 2u)
	{
		for (unsigned int tX = 0u, sX = 0u; tX < targetWidth; ++tX, sX += 2u)
		{
			const uint8_t* sourceTopLeft = source.constpixel<uint8_t>(sX, sY + 0u);
			const uint8_t* sourceBottomLeft = source.constpixel<uint8_t>(sX, sY + 1u);

			if (tX < targetCoreWidth && tY < targetCoreHeight)
			{
				// center pixels
				// | 1 1 |
				// | 1 1 | / 4

				for (unsigned int c = 0u; c < channels; ++c)
				{
					const unsigned int value = (sourceTopLeft[c] + sourceTopLeft[channels + c] + sourceBottomLeft[c] + sourceBottomLeft[channels + c] + 2u) / 4u;
					ocean_assert(value <= 255u);

					resultValues[c] = (uint8_t)value;
				}
			}
			else if (tX == targetCoreWidth && tY < targetCoreHeight)
			{
				// right border pixel
				// | 1 2 1 |
				// | 1 2 1 | / 8

				for (unsigned int c = 0u; c < channels; ++c)
				{
					const unsigned int valueTop = sourceTopLeft[c] + 2u * sourceTopLeft[channels + c] + sourceTopLeft[2u * channels + c];
					const unsigned int valueBottom = sourceBottomLeft[c] + 2u * sourceBottomLeft[channels + c] + sourceBottomLeft[2u * channels + c];

					const unsigned int value = (valueTop + valueBottom + 4u) / 8u;
					ocean_assert(value <= 255u);

					resultValues[c] = (uint8_t)value;
				}
			}
			else if (tX < targetCoreWidth && tY == targetCoreHeight)
			{
				// bottom border
				// | 1 1 |
				// | 2 2 | / 8
				// | 1 1 |

				const uint8_t* sourceBottomExtra = source.constpixel<uint8_t>(sX, sY + 2u);

				for (unsigned int c = 0u; c < channels; ++c)
				{
					const unsigned int valueTop = sourceTopLeft[c] + sourceTopLeft[channels + c];
					const unsigned int valueMiddle = (sourceBottomLeft[c] + sourceBottomLeft[channels + c]) * 2u;
					const unsigned int valueBottom = sourceBottomExtra[c] + sourceBottomExtra[channels + c];

					const unsigned int value = (valueTop + valueMiddle + valueBottom + 4u) / 8u;
					ocean_assert(value <= 255u);

					resultValues[c] = (uint8_t)value;
				}
			}
			else
			{
				ocean_assert(tX == targetCoreWidth && tY == targetCoreHeight);

				// bottom right corner
				// | 1 2 1 |
				// | 2 4 2 | / 16
				// | 1 2 1 |

				const uint8_t* sourceBottomExtra = source.constpixel<uint8_t>(sX, sY + 2u);

				for (unsigned int c = 0u; c < channels; ++c)
				{
					const unsigned int valueTop = sourceTopLeft[c] + sourceTopLeft[channels + c] * 2u + sourceTopLeft[2u * channels + c];
					const unsigned int valueMiddle = (sourceBottomLeft[c] + sourceBottomLeft[channels + c] * 2u + sourceBottomLeft[2u * channels + c]) * 2u;
					const unsigned int valueBottom = sourceBottomExtra[c] + sourceBottomExtra[channels + c] * 2u + sourceBottomExtra[2u * channels + c];

					const unsigned int value = (valueTop + valueMiddle + valueBottom + 8u) / 16u;
					ocean_assert(value <= 255u);

					resultValues[c] = (uint8_t)value;
				}
			}

			const uint8_t* targetResult = target.constpixel<uint8_t>(tX, tY);

			for (unsigned int n = 0u; n < channels; ++n)
			{
				const double absError = NumericD::abs(double(targetResult[n]) - double(resultValues[n]));

				sumAbsError += absError;
				maxAbsError = max(maxAbsError, absError);

				measurements++;
			}

			if (groundTruth)
			{
				memcpy(groundTruth + tY * groundTruthStrideElements + tX * channels, resultValues.data(), channels * sizeof(uint8_t));
			}
		}
	}

	if (averageAbsError)
	{
		ocean_assert(measurements != 0ull);
		*averageAbsError = sumAbsError / double(measurements);
	}

	if (maximalAbsError)
	{
		*maximalAbsError = maxAbsError;
	}

	return true;
}

bool TestFrameShrinker::validateDownsampleBinayMaskByTwo11(const uint8_t* source, const uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int threshold)
{
	ocean_assert(source && target);
	ocean_assert(sourceWidth >= 2u && sourceHeight >= 2u);

	const unsigned int targetWidth = sourceWidth / 2u;
	const unsigned int targetHeight = sourceHeight / 2u;

	const unsigned int sourceStrideElements = sourceWidth + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth + targetPaddingElements;

	ocean_assert(targetWidth >= 1u && targetHeight >= 1u);

	unsigned int targetCoreWidth = targetWidth;
	unsigned int targetCoreHeight = targetHeight;

	if (sourceWidth % 2u != 0u)
	{
		--targetCoreWidth;
	}

	if (sourceHeight % 2u != 0u)
	{
		--targetCoreHeight;
	}

	// | 1 1 |
	// | 1 1 | / 4
	for (unsigned int tY = 0u, sY = 0u; tY < targetCoreHeight; ++tY, sY += 2u)
	{
		for (unsigned int tX = 0u, sX = 0u; tX < targetCoreWidth; ++tX, sX += 2u)
		{
			const unsigned int value = (source[sY * sourceStrideElements + sX] + source[sY * sourceStrideElements + (sX + 1u)] + source[(sY + 1u) * sourceStrideElements + sX] + source[(sY + 1u) * sourceStrideElements + (sX + 1u)]);

			const uint8_t pixel = value >= threshold ? 0xFFu : 0x00u;

			if (target[tY * targetStrideElements + tX] != pixel)
			{
				Log::info() << "A: " << value << " " << threshold;
				return false;
			}
		}
	}

	// right border
	// | 1 2 1 |
	// | 1 2 1 | / 8
	if (targetCoreWidth < targetWidth)
	{
		const unsigned int sX = sourceWidth - 3u;
		const unsigned int tX = targetWidth - 1u;

		for (unsigned int tY = 0u, sY = 0u; tY < targetCoreHeight; ++tY, sY += 2u)
		{
			const unsigned int value = (source[sY * sourceStrideElements + sX + 0u] + 2u * source[sY * sourceStrideElements + (sX + 1u) + 0u] + source[sY * sourceStrideElements + (sX + 2u) + 0u] + source[(sY + 1u) * sourceStrideElements + sX + 0u] + 2u * source[(sY + 1u) * sourceStrideElements + (sX + 1u) + 0u] + source[(sY + 1u) * sourceStrideElements + (sX + 2) + 0u]);

			const uint8_t pixel = value >= 2u * threshold ? 0xFFu : 0x00u;

			if (target[tY * targetStrideElements + tX] != pixel)
			{
				Log::info() << "B";
				return false;
			}
		}
	}

	// bottom border
	//  | 1 1 |
	//  | 2 2 | / 8
	//  | 1 1 |
	if (targetCoreHeight < targetHeight)
	{
		const unsigned int sY = sourceHeight - 3u;
		const unsigned int tY = targetHeight - 1u;

		for (unsigned int tX = 0u, sX = 0u; tX < targetCoreWidth; ++tX, sX += 2)
		{
			const unsigned int value = (source[sY * sourceStrideElements + sX + 0u] + source[sY * sourceStrideElements + (sX + 1u) + 0u] + 2u * source[(sY + 1u) * sourceStrideElements + sX + 0u] + 2u * source[(sY + 1u) * sourceStrideElements + (sX + 1u) + 0u] + source[(sY + 2u) * sourceStrideElements + sX + 0u] + source[(sY + 2u) * sourceStrideElements + (sX + 1u) + 0u]);

			const uint8_t pixel = value >= 2u * threshold ? 0xFFu : 0x00u;

			if (target[tY * targetStrideElements + tX] != pixel)
			{
				Log::info() << "C";
				return false;
			}
		}
	}

	// bottom right corner
	//  | 1 2 1 |
	//  | 2 4 2 | / 16
	//  | 1 2 1 |
	if (targetCoreWidth < targetWidth && targetCoreHeight < targetHeight)
	{
		const unsigned int sX = sourceWidth - 3u;
		const unsigned int sY = sourceHeight - 3u;

		const unsigned int tX = targetWidth - 1u;
		const unsigned int tY = targetHeight - 1u;

		const unsigned int value = (source[sY * sourceStrideElements + sX + 0u] + 2u * source[sY * sourceStrideElements + (sX + 1u) + 0u] + source[sY * sourceStrideElements + (sX + 2) + 0u] + 2u * source[(sY + 1u) * sourceStrideElements + sX + 0u] + 4u * source[(sY + 1u) * sourceStrideElements + (sX + 1u) + 0u] + 2u * source[(sY + 1u) * sourceStrideElements + (sX + 2) + 0u] + source[(sY + 2u) * sourceStrideElements + sX + 0u] + 2u * source[(sY + 2u) * sourceStrideElements + (sX + 1u) + 0u] + source[(sY + 2u) * sourceStrideElements + (sX + 2) + 0u]);

		const uint8_t pixel = value >= 4u * threshold ? 0xFFu : 0x00u;

		if (target[tY * targetStrideElements + tX] != pixel)
		{
			Log::info() << "D";
			return false;
		}
	}

	return true;
}

void TestFrameShrinker::validateDownsamplingByTwo8Bit14641(const uint8_t* source, const uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, double* averageAbsError, double* maximalAbsError, uint8_t* groundTruth, const unsigned int groundTruthPaddingElements)
{
	ocean_assert(source != nullptr);
	ocean_assert(target != nullptr);

	ocean_assert(sourceWidth >= 2u && sourceHeight >= 2u);
	ocean_assert((sourceWidth + 0u) / 2u == targetWidth || (sourceWidth + 1u) / 2u == targetWidth);
	ocean_assert((sourceHeight + 0u) / 2u == targetHeight || (sourceHeight + 1u) / 2u == targetHeight);

	ocean_assert(channels >= 1u);

	const unsigned int sourceStrideElements = sourceWidth * channels + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * channels + targetPaddingElements;
	const unsigned int groundTruthStrideElements = targetWidth * channels + groundTruthPaddingElements;

	const unsigned int filterFactors[5] = {1u, 4u, 6u, 4u, 1u};

	std::vector<unsigned int> intermediateValues(channels);
	std::vector<uint8_t> resultValues(channels);

	if (averageAbsError)
	{
		*averageAbsError = NumericD::maxValue();
	}

	if (maximalAbsError)
	{
		*maximalAbsError = NumericD::maxValue();
	}

	double sumAbsError = 0.0;
	double maxAbsError = 0.0;

	uint64_t measurements = 0ull;

	for (unsigned int yTarget = 0u; yTarget < targetHeight; ++yTarget)
	{
		const unsigned int ySource = yTarget * 2u;
		ocean_assert(ySource < sourceHeight);

		for (unsigned int xTarget = 0u; xTarget < targetWidth; ++xTarget)
		{
			const unsigned int xSource = xTarget * 2u;
			ocean_assert(xSource < sourceWidth);

			for (unsigned int n = 0u; n < channels; ++n)
			{
				intermediateValues[n] = 0u;
			}

			for (int fy = -2; fy <= 2; ++fy)
			{
				const unsigned int sourceFy = mirrorValue(int(ySource) + fy, sourceHeight);
				ocean_assert(sourceFy < sourceHeight);

				const unsigned int filterFactorY = filterFactors[fy + 2];

				for (int fx = -2; fx <= 2; ++fx)
				{
					const unsigned int sourceFx = mirrorValue(int(xSource) + fx, sourceWidth);
					ocean_assert(sourceFx < sourceWidth);

					const unsigned int filterFactorX = filterFactors[fx + 2];

					for (unsigned int n = 0u; n < channels; ++n)
					{
						intermediateValues[n] += source[sourceFy * sourceStrideElements + sourceFx * channels + n] * filterFactorY * filterFactorX;
					}
				}
			}

			for (unsigned int n = 0u; n < channels; ++n)
			{
				resultValues[n] = uint8_t((intermediateValues[n] + 128u) / 256u);
			}

			const uint8_t* targetResult = target + yTarget * targetStrideElements + xTarget * channels;

			for (unsigned int n = 0u; n < channels; ++n)
			{
				const double absError = NumericD::abs(double(targetResult[n]) - double(resultValues[n]));

				sumAbsError += absError;
				maxAbsError = max(maxAbsError, absError);

				measurements++;
			}

			if (groundTruth)
			{
				memcpy(groundTruth + yTarget * groundTruthStrideElements + xTarget * channels, resultValues.data(), channels * sizeof(uint8_t));
			}
		}
	}

	if (averageAbsError)
	{
		ocean_assert(measurements != 0ull);
		*averageAbsError = sumAbsError / double(measurements);
	}

	if (maximalAbsError)
	{
		*maximalAbsError = maxAbsError;
	}
}

}

}

}
