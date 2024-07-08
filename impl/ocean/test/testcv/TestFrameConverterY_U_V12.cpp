/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverterY_U_V12.h"

#include "ocean/cv/FrameConverterY_U_V12.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameConverterY_U_V12::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	Log::info() << "---   Y_U_V12 converter test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	{
		Log::info() << "Testing Y_U_V12 to Y_U_V12 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_U_V12ToY_U_V12(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_U_V12 (limited range) to BGR24 (full range) conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_U_V12LimitedRangeToBGR24FullRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_U_V12 (full range) to BGR24 (full range) conversion with resolution " << width << "x" << height << " (6 bit):";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_U_V12FullRangeToBGR24FullRangePrecision6Bit(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_U_V12 (limited range) to BGRA32 (full range) conversion with resolution " << width << "x" << height << " (6 bit):";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_U_V12LimitedRangeToBGRA32FullRangePrecision6Bit(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_U_V12 (limited range) to RGB24 (full range) conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_U_V12LimitedRangeToRGB24FullRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_U_V12 (limited range) to RGB24 (full range) conversion with resolution " << width << "x" << height << " (6 bit):";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_U_V12LimitedRangeToRGB24FullRangePrecision6Bit(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_U_V12 (full range) to RGB24 (full range) conversion with resolution " << width << "x" << height << " (6 bit):";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_U_V12FullRangeToRGB24FullRangePrecision6Bit(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_U_V12 (limited range) to RGBA32 (full range) conversion with resolution " << width << "x" << height << " (6 bit):";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_U_V12LimitedRangeToRGBA32FullRangePrecision6Bit(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_U_V12 to YUV24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_U_V12ToYUV24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_U_V12 to YVU24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_U_V12ToYVU24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_U_V12 to Y8 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_U_V12ToY8(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Y_U_V12 converter tests succeeded.";
	}
	else
	{
		Log::info() << "Y_U_V12 converter tests FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterY_U_V12, Y_U_V12ToY_U_V12Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12ToY_U_V12(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12ToY_U_V12Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12ToY_U_V12(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12ToY_U_V12Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12ToY_U_V12(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12ToY_U_V12FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12ToY_U_V12(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_U_V12, Y_U_V12LimitedRangeToBGR24FullRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToBGR24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12LimitedRangeToBGR24FullRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToBGR24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12LimitedRangeToBGR24FullRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToBGR24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12LimitedRangeToBGR24FullRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToBGR24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_U_V12, Y_U_V12FullRangeToBGR24FullRangePrecision6BitNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12FullRangeToBGR24FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12FullRangeToBGR24FullRangePrecision6BitFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12FullRangeToBGR24FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12FullRangeToBGR24FullRangePrecision6BitMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12FullRangeToBGR24FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12FullRangeToBGR24FullRangePrecision6BitFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12FullRangeToBGR24FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_U_V12, Y_U_V12LimitedRangeToBGRA32FullRangePrecision6BitNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToBGRA32FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12LimitedRangeToBGRA32FullRangePrecision6BitFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToBGRA32FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12LimitedRangeToBGRA32FullRangePrecision6BitMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToBGRA32FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12,Y_U_V12LimitedRangeToBGRA32FullRangePrecision6BitFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToBGRA32FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_U_V12, Y_U_V12LimitedRangeToRGB24FullRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToRGB24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12LimitedRangeToRGB24FullRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToRGB24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12LimitedRangeToRGB24FullRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToRGB24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12LimitedRangeToRGB24FullRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToRGB24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_U_V12, Y_U_V12LimitedRangeToRGB24FullRangePrecision6BitNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToRGB24FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12LimitedRangeToRGB24FullRangePrecision6BitFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToRGB24FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12LimitedRangeToRGB24FullRangePrecision6BitMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToRGB24FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12LimitedRangeToRGB24FullRangePrecision6BitFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToRGB24FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_U_V12, Y_U_V12FullRangeToRGB24FullRangePrecision6BitNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12FullRangeToRGB24FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12FullRangeToRGB24FullRangePrecision6BitFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12FullRangeToRGB24FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12FullRangeToRGB24FullRangePrecision6BitMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12FullRangeToRGB24FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12FullRangeToRGB24FullRangePrecision6BitFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12FullRangeToRGB24FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_U_V12, Y_U_V12LimitedRangeToRGBA32FullRangePrecision6BitNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToRGBA32FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12LimitedRangeToRGBA32FullRangePrecision6BitFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToRGBA32FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12LimitedRangeToRGBA32FullRangePrecision6BitMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToRGBA32FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12,Y_U_V12LimitedRangeToRGBA32FullRangePrecision6BitFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToRGBA32FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_U_V12, Y_U_V12ToYUV24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12ToYUV24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12ToYUV24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12ToYUV24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_U_V12, Y_U_V12ToYVU24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12ToYVU24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12ToYVU24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12ToYVU24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12ToYVU24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12ToYVU24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12ToYVU24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12ToYVU24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_U_V12, Y_U_V12ToY8Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12ToY8Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12ToY8Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12ToY8FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterY_U_V12::testY_U_V12ToY_U_V12(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width >= 2u && height >= 2u);

	const MatrixD transformationMatrix(3, 3, true);

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y_U_V12, FrameType::FORMAT_Y_U_V12, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY_U_V12::convertY_U_V12ToY_U_V12), conversionFlag, pixelFunctionY_U_V12ForYUV24, pixelFunctionY_U_V12ForYUV24, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToBGR24FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_LimitedRangeYUV24_To_FullRangeBGR24_BT601();

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_BGR24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY_U_V12::convertY_U_V12LimitedRangeToBGR24FullRange), conversionFlag, pixelFunctionY_U_V12ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY_U_V12::testY_U_V12FullRangeToBGR24FullRangePrecision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_FullRangeYUV24_To_FullRangeBGR24_BT601();

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y_U_V12_FULL_RANGE, FrameType::FORMAT_BGR24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY_U_V12::convertY_U_V12FullRangeToBGR24FullRangePrecision6Bit), conversionFlag, pixelFunctionY_U_V12ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToBGRA32FullRangePrecision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// BT.601

	// | B |     | 1.1639404296875   2.0179443359375  0.0             -276.919921875 |   | Y |
	// | G |  =  | 1.1639404296875  -0.3909912109375 -0.81298828125    135.486328125 | * | U |
	// | R |     | 1.1639404296875   0.0              1.595947265625  -222.904296875 |   | V |
	// | A |     | 0.0               0.0              0.0              alpha         |   | 1 |

	MatrixD transformationMatrix(4, 4, CV::FrameConverter::transformationMatrix_LimitedRangeYUV24_To_FullRangeBGR24_BT601(), 0, 0);
	transformationMatrix(3, 3) = double(TestFrameConverter::ValueProvider::get().alphaValue());

	constexpr unsigned int thresholdMaximalErrorToInteger = 6u;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_BGRA32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY_U_V12::convertY_U_V12LimitedRangeToBGRA32FullRangePrecision6Bit), conversionFlag, pixelFunctionY_U_V12ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

bool TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToRGB24FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_LimitedRangeYUV24_To_FullRangeRGB24_BT601();

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_RGB24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY_U_V12::convertY_U_V12LimitedRangeToRGB24FullRange), conversionFlag, pixelFunctionY_U_V12ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToRGB24FullRangePrecision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_LimitedRangeYUV24_To_FullRangeRGB24_BT601();

	constexpr unsigned int thresholdMaximalErrorToInteger = 6u;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_RGB24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY_U_V12::convertY_U_V12LimitedRangeToRGB24FullRangePrecision6Bit), conversionFlag, pixelFunctionY_U_V12ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

bool TestFrameConverterY_U_V12::testY_U_V12FullRangeToRGB24FullRangePrecision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_FullRangeYUV24_To_FullRangeRGB24_BT601();

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y_U_V12_FULL_RANGE, FrameType::FORMAT_RGB24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY_U_V12::convertY_U_V12FullRangeToRGB24FullRangePrecision6Bit), conversionFlag, pixelFunctionY_U_V12ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToRGBA32FullRangePrecision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// BT.601

	// | R |     | 1.1639404296875   0.0              1.595947265625  -222.904296875 |   | Y |
	// | G |  =  | 1.1639404296875  -0.3909912109375 -0.81298828125    135.486328125 | * | U |
	// | B |     | 1.1639404296875   2.0179443359375  0.0             -276.919921875 |   | V |
	// | A |     | 0.0               0.0              0.0              alpha         |   | 1 |

	MatrixD transformationMatrix(4, 4, CV::FrameConverter::transformationMatrix_LimitedRangeYUV24_To_FullRangeRGB24_BT601(), 0, 0);
	transformationMatrix(3, 3) = double(TestFrameConverter::ValueProvider::get().alphaValue());

	constexpr unsigned int thresholdMaximalErrorToInteger = 6u;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_RGBA32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY_U_V12::convertY_U_V12LimitedRangeToRGBA32FullRangePrecision6Bit), conversionFlag, pixelFunctionY_U_V12ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

bool TestFrameConverterY_U_V12::testY_U_V12ToYUV24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y |   | 1 0 0 |   | Y |
	// | U | = | 0 1 0 | * | U |
	// | V |   | 0 0 1 |   | V |

	const MatrixD transformationMatrix(3, 3, true);

	constexpr unsigned int thresholdMaximalErrorToInteger = 0u;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_YUV24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY_U_V12::convertY_U_V12ToYUV24), conversionFlag, pixelFunctionY_U_V12ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

bool TestFrameConverterY_U_V12::testY_U_V12ToYVU24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y |   | 1 0 0 |   | Y |
	// | V | = | 0 0 1 | * | U |
	// | U |   | 0 1 0 |   | V |

	MatrixD transformationMatrix(3, 3, false);
	transformationMatrix(0, 0) = 1.0;
	transformationMatrix(1, 2) = 1.0;
	transformationMatrix(2, 1) = 1.0;

	constexpr unsigned int thresholdMaximalErrorToInteger = 0u;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_YVU24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY_U_V12::convertY_U_V12ToYVU24), conversionFlag, pixelFunctionY_U_V12ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

bool TestFrameConverterY_U_V12::testY_U_V12ToY8(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	//                     | Y |
	// | Y | = | 1 0 0 | * | U |
	//                     | V |

	MatrixD transformationMatrix(1, 3, false);
	transformationMatrix(0, 0) = 1.0;

	const unsigned int thresholdMaximalErrorToInteger = 0u;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_Y8, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY_U_V12::convertY_U_V12ToY8), conversionFlag, pixelFunctionY_U_V12ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

MatrixD TestFrameConverterY_U_V12::pixelFunctionY_U_V12ForYUV24(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
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

	colorVector(0, 0) = double(*frame.constpixel<uint8_t>(xAdjusted, yAdjusted, 0u));
	colorVector(1, 0) = double(*frame.constpixel<uint8_t>(xAdjusted_2, yAdjusted_2, 1u));
	colorVector(2, 0) = double(*frame.constpixel<uint8_t>(xAdjusted_2, yAdjusted_2, 2u));

	return colorVector;
}

}

}

}
