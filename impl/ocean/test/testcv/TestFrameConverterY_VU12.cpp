/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverterY_VU12.h"

#include "ocean/cv/FrameConverterY_VU12.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameConverterY_VU12::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	Log::info() << "---   Y_VU12 converter test:   ---";
	Log::info() << " ";

	const CV::FrameConverter::ConversionFlags flags = CV::FrameConverter::conversionFlags();

	bool allSucceeded = true;

	{
		Log::info() << "Testing Y_VU12_LIMITED_RANGE to BGR24 (full range) conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_VU12LimitedRangeToBGR24FullRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_VU12_FULL_RANGE to BGRA32 (full range), Android specific, conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_VU12FullRangeToBGRA32FullRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_VU12_LIMITED_RANGE to RGB24 (full range) conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_VU12LimitedRangeToRGB24FullRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_VU12_FULL_RANGE to RGB24 (full range) conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_VU12FullRangeToRGB24FullRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_VU12 to YUV24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_VU12ToYUV24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_VU12 to YVU24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_VU12ToYVU24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_VU12_LIMITED_RANGE to Y8_LIMITED_RANGE conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_VU12LimitedRangeToY8LimitedRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_VU12_LIMITED_RANGE to Y8_FULL_RANGE conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_VU12LimitedRangeToY8FullRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_VU12_FULL_RANGE to Y8_FULL_RANGE conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_VU12FullRangeToY8FullRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_VU12_FULL_RANGE to Y8_LIMITED_RANGE conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_VU12FullRangeToY8LimitedRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_VU12_LIMITED_RANGE to Y_UV12_LIMITED_RANGE conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_VU12LimitedRangeToY_UV12LimitedRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_VU12_FULL_RANGE to Y_UV12_FULL_RANGE conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_VU12FullRangeToY_UV12FullRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_VU12_LIMITED_RANGE to Y_U_V12_LIMITED_RANGE conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_VU12LimitedRangeToY_U_V12LimitedRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_VU12_FULL_RANGE to Y_U_V12_FULL_RANGE conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_VU12FullRangeToY_U_V12FullRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Y_VU12 converter tests succeeded.";
	}
	else
	{
		Log::info() << "Y_VU12 converter tests FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterY_VU12, Y_VU12LimitedRangeToBGR24FullRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12LimitedRangeToBGR24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12LimitedRangeToBGR24FullRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12LimitedRangeToBGR24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12LimitedRangeToBGR24FullRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12LimitedRangeToBGR24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12LimitedRangeToBGR24FullRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12LimitedRangeToBGR24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_VU12, Y_VU12FullRangeToBGRA32FullRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12FullRangeToBGRA32FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12FullRangeToBGRA32FullRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12FullRangeToBGRA32FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12FullRangeToBGRA32FullRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12FullRangeToBGRA32FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12FullRangeToBGRA32FullRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12FullRangeToBGRA32FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_VU12, Y_VU12LimitedRangeToRGB24FullRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12LimitedRangeToRGB24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12LimitedRangeToRGB24FullRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12LimitedRangeToRGB24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12LimitedRangeToRGB24FullRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12LimitedRangeToRGB24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12LimitedRangeToRGB24FullRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12LimitedRangeToRGB24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_VU12, Y_VU12FullRangeToRGB24FullRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12FullRangeToRGB24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12FullRangeToRGB24FullRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12FullRangeToRGB24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12FullRangeToRGB24FullRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12FullRangeToRGB24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12FullRangeToRGB24FullRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12FullRangeToRGB24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_VU12, Y_VU12ToYUV24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12ToYUV24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12ToYUV24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12ToYUV24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_VU12, Y_VU12ToYVU24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12ToYVU24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12ToYVU24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12ToYVU24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12ToYVU24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12ToYVU24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12ToYVU24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12ToYVU24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_VU12, Y_VU12LimitedRangeToY8LimitedRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12LimitedRangeToY8LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12LimitedRangeToY8LimitedRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12LimitedRangeToY8LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12LimitedRangeToY8LimitedRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12LimitedRangeToY8LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12LimitedRangeToY8LimitedRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12LimitedRangeToY8LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_VU12, Y_VU12LimitedRangeToY8FullRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12LimitedRangeToY8FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12LimitedRangeToY8FullRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12LimitedRangeToY8FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12LimitedRangeToY8FullRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12LimitedRangeToY8FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12LimitedRangeToY8FullRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12LimitedRangeToY8FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_VU12, Y_VU12FullRangeToY8FullRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12FullRangeToY8FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12FullRangeToY8FullRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12FullRangeToY8FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12FullRangeToY8FullRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12FullRangeToY8FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12FullRangeToY8FullRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12FullRangeToY8FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_VU12, Y_VU12FullRangeToY8LimitedRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12FullRangeToY8LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12FullRangeToY8LimitedRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12FullRangeToY8LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12FullRangeToY8LimitedRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12FullRangeToY8LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12FullRangeToY8LimitedRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12FullRangeToY8LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_VU12, Y_VU12LimitedRangeToY_UV12LimitedRange_Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12LimitedRangeToY_UV12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12LimitedRangeToY_UV12LimitedRange_Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12LimitedRangeToY_UV12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12LimitedRangeToY_UV12LimitedRange_Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12LimitedRangeToY_UV12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12LimitedRangeToY_UV12LimitedRange_FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12LimitedRangeToY_UV12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_VU12, Y_VU12FullRangeToY_UV12FullRange_Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12FullRangeToY_UV12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12FullRangeToY_UV12FullRange_Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12FullRangeToY_UV12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12FullRangeToY_UV12FullRange_Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12FullRangeToY_UV12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12FullRangeToY_UV12FullRange_FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12FullRangeToY_UV12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_VU12, Y_VU12LimitedRangeToY_U_V12LimitedRange_Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12LimitedRangeToY_U_V12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12LimitedRangeToY_U_V12LimitedRange_Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12LimitedRangeToY_U_V12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12LimitedRangeToY_U_V12LimitedRange_Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12LimitedRangeToY_U_V12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12LimitedRangeToY_U_V12LimitedRange_FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12LimitedRangeToY_U_V12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_VU12, Y_VU12FullRangeToY_U_V12FullRange_Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12FullRangeToY_U_V12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12FullRangeToY_U_V12FullRange_Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12FullRangeToY_U_V12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12FullRangeToY_U_V12FullRange_Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12FullRangeToY_U_V12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_VU12, Y_VU12FullRangeToY_U_V12FullRange_FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_VU12::testY_VU12FullRangeToY_U_V12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterY_VU12::testY_VU12LimitedRangeToBGR24FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_LimitedRangeYVU24_To_FullRangeBGR24_BT601();

	constexpr unsigned int thresholdMaximalErrorToInteger = 5u;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_VU12_LIMITED_RANGE, FrameType::FORMAT_BGR24, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_VU12::convertY_VU12LimitedRangeToBGR24FullRange), conversionFlag, pixelFunctionY_VU12ForYVU24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

bool TestFrameConverterY_VU12::testY_VU12FullRangeToBGRA32FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	MatrixD transformationMatrix(4, 4, CV::FrameConverter::transformationMatrix_FullRangeYVU24_To_FullRangeBGR24_Android(), 0, 0);
	transformationMatrix(3, 3) = double(FrameConverterTestUtilities::ValueProvider::get().alphaValue());

	constexpr unsigned int thresholdMaximalErrorToInteger = 5u;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_VU12_FULL_RANGE, FrameType::FORMAT_BGRA32, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_VU12::convertY_VU12FullRangeToBGRA32FullRangeAndroid), conversionFlag, pixelFunctionY_VU12ForYVU24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

bool TestFrameConverterY_VU12::testY_VU12LimitedRangeToRGB24FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_LimitedRangeYVU24_To_FullRangeRGB24_BT601();

	constexpr unsigned int thresholdMaximalErrorToInteger = 5u;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_VU12_LIMITED_RANGE, FrameType::FORMAT_RGB24, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_VU12::convertY_VU12LimitedRangeToRGB24FullRange), conversionFlag, pixelFunctionY_VU12ForYVU24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

bool TestFrameConverterY_VU12::testY_VU12FullRangeToRGB24FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_FullRangeYVU24_To_FullRangeRGB24_BT601();

	constexpr unsigned int thresholdMaximalErrorToInteger = 5u;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_VU12_FULL_RANGE, FrameType::FORMAT_RGB24, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_VU12::convertY_VU12FullRangeToRGB24FullRangePrecision6Bit), conversionFlag, pixelFunctionY_VU12ForYVU24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

bool TestFrameConverterY_VU12::testY_VU12ToYUV24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y |   | 1 0 0 |   | Y |
	// | U | = | 0 0 1 | * | V |
	// | V |   | 0 1 0 |   | U |

	MatrixD transformationMatrix(3, 3, false);
	transformationMatrix(0, 0) = 1.0;
	transformationMatrix(1, 2) = 1.0;
	transformationMatrix(2, 1) = 1.0;

	constexpr unsigned int thresholdMaximalErrorToInteger = 0u;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_VU12_LIMITED_RANGE, FrameType::FORMAT_YUV24, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_VU12::convertY_VU12ToYUV24), conversionFlag, pixelFunctionY_VU12ForYVU24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

bool TestFrameConverterY_VU12::testY_VU12ToYVU24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y |   | 1 0 0 |   | Y |
	// | V | = | 0 1 0 | * | V |
	// | U |   | 0 0 1 |   | U |

	const MatrixD transformationMatrix(3, 3, true);

	constexpr unsigned int thresholdMaximalErrorToInteger = 0u;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_VU12_LIMITED_RANGE, FrameType::FORMAT_YVU24, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_VU12::convertY_VU12ToYVU24), conversionFlag, pixelFunctionY_VU12ForYVU24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

bool TestFrameConverterY_VU12::testY_VU12LimitedRangeToY8LimitedRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	//                     | Y |
	// | Y | = | 1 0 0 | * | V |
	//                     | U |

	MatrixD transformationMatrix(1, 3, false);
	transformationMatrix(0, 0) = 1.0;

	constexpr unsigned int thresholdMaximalErrorToInteger = 0u;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_VU12_LIMITED_RANGE, FrameType::FORMAT_Y8_LIMITED_RANGE, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_VU12::convertY_VU12LimitedRangeToY8LimitedRange), conversionFlag, pixelFunctionY_VU12ForYVU24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

bool TestFrameConverterY_VU12::testY_VU12LimitedRangeToY8FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// Y_full = (Y_limited - 16) * 255 / 219

	// | Y_full | = | 255/219  0  0  -16*255/219 | * | Y |
	//                                               | V |
	//                                               | U |
	//                                               | 1 |

	MatrixD transformationMatrix(1, 4, false);
	transformationMatrix(0, 0) = 255.0 / 219.0;
	transformationMatrix(0, 3) = -16.0 * 255.0 / 219.0;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_VU12_LIMITED_RANGE, FrameType::FORMAT_Y8_FULL_RANGE, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_VU12::convertY_VU12LimitedRangeToY8FullRange), conversionFlag, pixelFunctionY_VU12ForYVU24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY_VU12::testY_VU12FullRangeToY8FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	//                     | Y |
	// | Y | = | 1 0 0 | * | V |
	//                     | U |

	MatrixD transformationMatrix(1, 3, false);
	transformationMatrix(0, 0) = 1.0;

	constexpr unsigned int thresholdMaximalErrorToInteger = 0u;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_VU12_FULL_RANGE, FrameType::FORMAT_Y8_FULL_RANGE, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_VU12::convertY_VU12FullRangeToY8FullRange), conversionFlag, pixelFunctionY_VU12ForYVU24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

bool TestFrameConverterY_VU12::testY_VU12FullRangeToY8LimitedRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// Y_limited = Y_full * 219 / 255 + 16

	// | Y_limited | = | 219/255  0  0  16 | * | Y |
	//                                         | V |
	//                                         | U |
	//                                         | 1 |

	MatrixD transformationMatrix(1, 4, false);
	transformationMatrix(0, 0) = 219.0 / 255.0;
	transformationMatrix(0, 3) = 16.0;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_VU12_FULL_RANGE, FrameType::FORMAT_Y8_LIMITED_RANGE, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_VU12::convertY_VU12FullRangeToY8LimitedRange), conversionFlag, pixelFunctionY_VU12ForYVU24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY_VU12::testY_VU12LimitedRangeToY_UV12LimitedRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y |   | 1 0 0 |   | Y |
	// | U | = | 0 0 1 | * | V |
	// | V |   | 0 1 0 |   | U |

	MatrixD transformationMatrix(3, 3, false);
	transformationMatrix(0, 0) = 1.0;
	transformationMatrix(1, 2) = 1.0;
	transformationMatrix(2, 1) = 1.0;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_VU12_LIMITED_RANGE, FrameType::FORMAT_Y_UV12_LIMITED_RANGE, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_VU12::convertY_VU12ToY_UV12), conversionFlag, pixelFunctionY_VU12ForYVU24, pixelFunctionY_UV12ForYUV24, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY_VU12::testY_VU12FullRangeToY_UV12FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y |   | 1 0 0 |   | Y |
	// | U | = | 0 0 1 | * | V |
	// | V |   | 0 1 0 |   | U |

	MatrixD transformationMatrix(3, 3, false);
	transformationMatrix(0, 0) = 1.0;
	transformationMatrix(1, 2) = 1.0;
	transformationMatrix(2, 1) = 1.0;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_VU12_FULL_RANGE, FrameType::FORMAT_Y_UV12_FULL_RANGE, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_VU12::convertY_VU12ToY_UV12), conversionFlag, pixelFunctionY_VU12ForYVU24, pixelFunctionY_UV12ForYUV24, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY_VU12::testY_VU12LimitedRangeToY_U_V12LimitedRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y |   | 1 0 0 |   | Y |
	// | U | = | 0 0 1 | * | V |
	// | V |   | 0 1 0 |   | U |

	MatrixD transformationMatrix(3, 3, false);
	transformationMatrix(0, 0) = 1.0;
	transformationMatrix(1, 2) = 1.0;
	transformationMatrix(2, 1) = 1.0;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_VU12_LIMITED_RANGE, FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_VU12::convertY_VU12ToY_U_V12), conversionFlag, pixelFunctionY_VU12ForYVU24, pixelFunctionY_U_V12ForYUV24, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY_VU12::testY_VU12FullRangeToY_U_V12FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y |   | 1 0 0 |   | Y |
	// | U | = | 0 0 1 | * | V |
	// | V |   | 0 1 0 |   | U |

	MatrixD transformationMatrix(3, 3, false);
	transformationMatrix(0, 0) = 1.0;
	transformationMatrix(1, 2) = 1.0;
	transformationMatrix(2, 1) = 1.0;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_VU12_FULL_RANGE, FrameType::FORMAT_Y_U_V12_FULL_RANGE, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_VU12::convertY_VU12ToY_U_V12), conversionFlag, pixelFunctionY_VU12ForYVU24, pixelFunctionY_U_V12ForYUV24, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

MatrixD TestFrameConverterY_VU12::pixelFunctionY_VU12ForYVU24(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
{
	ocean_assert(frame.isValid());
	ocean_assert(x < frame.width() && y < frame.height());
	ocean_assert_and_suppress_unused(conversionFlag == CV::FrameConverter::CONVERT_NORMAL, conversionFlag); // we expect the target frame to have a conversion flag

	const unsigned int x_2 = x / 2u;
	const unsigned int y_2 = y / 2u;

	MatrixD colorVector(3, 1);

	colorVector(0, 0) = double(frame.constpixel<uint8_t>(x, y, 0u)[0]);
	colorVector(1, 0) = double(frame.constpixel<uint8_t>(x_2, y_2, 1u)[0]);
	colorVector(2, 0) = double(frame.constpixel<uint8_t>(x_2, y_2, 1u)[1]);

	return colorVector;
}

MatrixD TestFrameConverterY_VU12::pixelFunctionY_UV12ForYUV24(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
{
	ocean_assert(frame.isValid());
	ocean_assert(x < frame.width() && y < frame.height());

	unsigned int xAdjusted = x;
	unsigned int yAdjusted = y;

	switch (conversionFlag)
	{
		case CV::FrameConverter::CONVERT_NORMAL:
			break;

		case CV::FrameConverter::CONVERT_FLIPPED:
			yAdjusted = frame.height() - y - 1u;
			break;

		case CV::FrameConverter::CONVERT_MIRRORED:
			xAdjusted = frame.width() - x - 1u;
			break;

		case CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED:
			xAdjusted = frame.width() - x - 1u;
			yAdjusted = frame.height() - y - 1u;
			break;

		default:
			ocean_assert(false && "Not supported conversion flag.");
	}

	const unsigned int xAdjusted_2 = xAdjusted / 2u;
	const unsigned int yAdjusted_2 = yAdjusted / 2u;

	MatrixD colorVector(3, 1);

	colorVector(0, 0) = double(frame.constpixel<uint8_t>(xAdjusted, yAdjusted, 0u)[0]);
	colorVector(1, 0) = double(frame.constpixel<uint8_t>(xAdjusted_2, yAdjusted_2, 1u)[0]);
	colorVector(2, 0) = double(frame.constpixel<uint8_t>(xAdjusted_2, yAdjusted_2, 1u)[1]);

	return colorVector;
}

MatrixD TestFrameConverterY_VU12::pixelFunctionY_U_V12ForYUV24(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
{
	ocean_assert(frame.isValid());
	ocean_assert(x < frame.width() && y < frame.height());

	unsigned int xAdjusted = x;
	unsigned int yAdjusted = y;

	switch (conversionFlag)
	{
		case CV::FrameConverter::CONVERT_NORMAL:
			break;

		case CV::FrameConverter::CONVERT_FLIPPED:
			yAdjusted = frame.height() - y - 1u;
			break;

		case CV::FrameConverter::CONVERT_MIRRORED:
			xAdjusted = frame.width() - x - 1u;
			break;

		case CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED:
			xAdjusted = frame.width() - x - 1u;
			yAdjusted = frame.height() - y - 1u;
			break;

		default:
			ocean_assert(false && "Not supported conversion flag.");
	}

	const unsigned int xAdjusted_2 = xAdjusted / 2u;
	const unsigned int yAdjusted_2 = yAdjusted / 2u;

	MatrixD colorVector(3, 1);

	colorVector(0, 0) = double(frame.constpixel<uint8_t>(xAdjusted, yAdjusted, 0u)[0]);
	colorVector(1, 0) = double(frame.constpixel<uint8_t>(xAdjusted_2, yAdjusted_2, 1u)[0]);
	colorVector(2, 0) = double(frame.constpixel<uint8_t>(xAdjusted_2, yAdjusted_2, 2u)[0]);

	return colorVector;
}

}

}

}
