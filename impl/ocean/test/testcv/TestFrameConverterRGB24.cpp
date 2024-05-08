/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverterRGB24.h"

#include "ocean/cv/FrameConverterRGB24.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameConverterRGB24::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	Log::info() << "---   RGB24 converter test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	{
		Log::info() << "Testing RGB24 to ARGB32 conversion with resolution " << width << "x" << height << ":";
		Log::info() << " ";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGB24ToARGB32(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing RGB24 to BGR24 conversion with resolution " << width << "x" << height << ":";
		Log::info() << " ";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGB24ToBGR24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing RGB24 to BGR32 conversion with resolution " << width << "x" << height << ":";
		Log::info() << " ";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGB24ToBGR32(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing RGB24 to BGRA32 conversion with resolution " << width << "x" << height << ":";
		Log::info() << " ";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGB24ToBGRA32(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing RGB24 to RGB24 conversion with resolution " << width << "x" << height << ":";
		Log::info() << " ";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGB24ToRGB24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing RGB24 to RGB32 conversion with resolution " << width << "x" << height << ":";
		Log::info() << " ";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGB24ToRGB32(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing RGB24 to RGBA32 conversion with resolution " << width << "x" << height << ":";
		Log::info() << " ";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGB24ToRGBA32(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing RGB24 to Y8 conversion with resolution " << width << "x" << height << ":";
		Log::info() << " ";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGB24ToY8(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing RGB24 to YUV24 conversion with resolution " << width << "x" << height << ":";
		Log::info() << " ";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGB24ToYUV24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing full range RGB24 to limited range Y_UV12 conversion with resolution " << width << "x" << height << ":";
		Log::info() << " ";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGB24FullRangeToY_UV12LimitedRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing full range RGB24 to limited range Y_VU12 conversion with resolution " << width << "x" << height << ":";
		Log::info() << " ";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGB24FullRangeToY_VU12LimitedRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing full range RGB24 to full range Y_UV12 conversion with resolution " << width << "x" << height << ":";
		Log::info() << " ";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGB24FullRangeToY_UV12FullRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing full range RGB24 to full range Y_VU12 conversion with resolution " << width << "x" << height << ":";
		Log::info() << " ";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGB24FullRangeToY_VU12FullRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing full range RGB24 to limited range Y_U_V12 conversion with resolution " << width << "x" << height << ":";
		Log::info() << " ";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGB24FullRangeToY_U_V12LimitedRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing full range RGB24 to limited range Y_V_U12 conversion with resolution " << width << "x" << height << ":";
		Log::info() << " ";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGB24FullRangeToY_V_U12LimitedRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing full range RGB24 to full range Y_U_V12 conversion with resolution " << width << "x" << height << ":";
		Log::info() << " ";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGB24FullRangeToY_U_V12FullRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing full range RGB24 to full range Y_V_U12 conversion with resolution " << width << "x" << height << ":";
		Log::info() << " ";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGB24FullRangeToY_V_U12FullRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "RGB24 converter test succeeded.";
	}
	else
	{
		Log::info() << "RGB24 converter test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterRGB24, RGB24ToARGB32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToARGB32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToARGB32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToARGB32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToARGB32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToARGB32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToARGB32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToARGB32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGB24, RGB24ToBGR24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToBGR24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToBGR24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToBGR24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGB24, RGB24ToBGR32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToBGR32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToBGR32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToBGR32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToBGR32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToBGR32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToBGR32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToBGR32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGB24, RGB24ToBGRA32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToBGRA32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToBGRA32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToBGRA32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGB24, RGB24ToRGB24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToRGB24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToRGB24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToRGB24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGB24, RGB24ToRGB32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToRGB32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToRGB32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToRGB32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToRGB32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToRGB32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToRGB32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToRGB32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGB24, RGB24ToRGBA32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToRGBA32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToRGBA32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToRGBA32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGB24, RGB24ToY8Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToY8Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToY8Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToY8FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGB24, RGB24ToYUV24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToYUV24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToYUV24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24ToYUV24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGB24, RGB24FullRangeToY_UV12LimitedRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_UV12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24FullRangeToY_UV12LimitedRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_UV12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24FullRangeToY_UV12LimitedRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_UV12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24FullRangeToY_UV12LimitedRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_UV12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGB24, RGB24FullRangeToY_VU12LimitedRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_VU12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24FullRangeToY_VU12LimitedRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_VU12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24FullRangeToY_VU12LimitedRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_VU12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24FullRangeToY_VU12LimitedRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_VU12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGB24, RGB24FullRangeToY_UV12FullRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_UV12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24FullRangeToY_UV12FullRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_UV12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24FullRangeToY_UV12FullRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_UV12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24FullRangeToY_UV12FullRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_UV12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGB24, RGB24FullRangeToY_VU12FullRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_VU12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24FullRangeToY_VU12FullRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_VU12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24FullRangeToY_VU12FullRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_VU12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24FullRangeToY_VU12FullRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_VU12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGB24, RGB24FullRangeToY_U_V12LimitedRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_U_V12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24FullRangeToY_U_V12LimitedRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_U_V12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24FullRangeToY_U_V12LimitedRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_U_V12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24FullRangeToY_U_V12LimitedRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_U_V12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGB24, RGB24FullRangeToY_V_U12LimitedRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_V_U12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24FullRangeToY_V_U12LimitedRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_V_U12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24FullRangeToY_V_U12LimitedRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_V_U12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24FullRangeToY_V_U12LimitedRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_V_U12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGB24, RGB24FullRangeToY_U_V12FullRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_U_V12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24FullRangeToY_U_V12FullRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_U_V12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24FullRangeToY_U_V12FullRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_U_V12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24FullRangeToY_U_V12FullRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_U_V12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGB24, RGB24FullRangeToY_V_U12FullRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_V_U12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24FullRangeToY_V_U12FullRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_V_U12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24FullRangeToY_V_U12FullRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_V_U12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB24, RGB24FullRangeToY_V_U12FullRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB24::testRGB24FullRangeToY_V_U12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterRGB24::testRGB24ToARGB32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | A |   | 0 0 0 A |   | R |
	// | R |   | 1 0 0 0 | * | G |
	// | G | = | 0 1 0 0 |   | B |
	// | B |   | 0 0 1 0 |   | 1 |

	MatrixD transformationMatrix(4, 4, false);
	transformationMatrix(0, 3) = double(TestFrameConverter::ValueProvider::get().alphaValue());
	transformationMatrix(1, 0) = 1;
	transformationMatrix(2, 1) = 1;
	transformationMatrix(3, 2) = 1;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGB24, FrameType::FORMAT_ARGB32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGB24::convertRGB24ToARGB32), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGB24::testRGB24ToBGR24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | B |   | 0 0 1 |   | R |
	// | G | = | 0 1 0 | * | G |
	// | R |   | 1 0 0 |   | B |

	MatrixD transformationMatrix(3, 3, false);
	transformationMatrix(0, 2) = 1;
	transformationMatrix(1, 1) = 1;
	transformationMatrix(2, 0) = 1;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGB24, FrameType::FORMAT_BGR24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGB24::convertRGB24ToBGR24), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGB24::testRGB24ToBGR32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | B |   | 0 0 1 |   | R |
	// | G | = | 0 1 0 | * | G |
	// | R |   | 1 0 0 |   | B |
	// | 0 |   | 0 0 0 |

	MatrixD transformationMatrix(4, 3, false);
	transformationMatrix(0, 2) = 1;
	transformationMatrix(1, 1) = 1;
	transformationMatrix(2, 0) = 1;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGB24, FrameType::FORMAT_BGR32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGB24::convertRGB24ToBGR32), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGB24::testRGB24ToBGRA32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | B |   | 0 0 1 0 |   | R |
	// | G | = | 0 1 0 0 | * | G |
	// | R |   | 1 0 0 0 |   | B |
	// | A |   | 0 0 0 A |   | 1 |

	MatrixD transformationMatrix(4, 4, false);
	transformationMatrix(0, 2) = 1;
	transformationMatrix(1, 1) = 1;
	transformationMatrix(2, 0) = 1;
	transformationMatrix(3, 3) = double(TestFrameConverter::ValueProvider::get().alphaValue());

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGB24, FrameType::FORMAT_BGRA32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGB24::convertRGB24ToBGRA32), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGB24::testRGB24ToRGB24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | R |   | 1 0 0 |   | R |
	// | G | = | 0 1 0 | * | G |
	// | B |   | 0 0 1 |   | B |

	const MatrixD transformationMatrix(3, 3, true);

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGB24, FrameType::FORMAT_RGB24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGB24::convertRGB24ToRGB24), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGB24::testRGB24ToRGB32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | B |   | 1 0 0 |   | R |
	// | G | = | 0 1 0 | * | G |
	// | R |   | 0 0 1 |   | B |
	// | 0 |   | 0 0 0 |

	MatrixD transformationMatrix(4, 3, true);

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGB24, FrameType::FORMAT_RGB32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGB24::convertRGB24ToRGB32), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGB24::testRGB24ToRGBA32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | R |   | 1 0 0 0 |   | R |
	// | G |   | 0 1 0 0 | * | G |
	// | B | = | 0 0 1 0 |   | B |
	// | A |   | 0 0 0 A |   | 1 |

	MatrixD transformationMatrix(4, 4, true);
	transformationMatrix(3, 3) = double(TestFrameConverter::ValueProvider::get().alphaValue());

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGB24, FrameType::FORMAT_RGBA32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGB24::convertRGB24ToRGBA32), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGB24::testRGB24ToY8(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	//                                 | R |
	// | Y | = | 0.299 0.587 0.114 | * | G |
	//                                 | B |

	MatrixD transformationMatrix(1, 3, false);
	transformationMatrix(0, 0) = 0.299;
	transformationMatrix(0, 1) = 0.587;
	transformationMatrix(0, 2) = 0.114;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGB24, FrameType::FORMAT_Y8, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGB24::convertRGB24ToY8), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGB24::testRGB24ToYUV24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_FullRangeRGB24_To_LimitedRangeYUV24_BT601();

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGB24, FrameType::FORMAT_YUV24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGB24::convertRGB24ToYUV24), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGB24::testRGB24FullRangeToY_UV12LimitedRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_FullRangeRGB24_To_LimitedRangeYUV24_BT601();

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGB24, FrameType::FORMAT_Y_UV12_LIMITED_RANGE, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGB24::convertRGB24FullRangeToY_UV12LimitedRange), flag, TestFrameConverterRGB24::pixelFunctionRGBForY_UV12, TestFrameConverterRGB24::pixelFunctionY_UV12ForYUV24, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGB24::testRGB24FullRangeToY_VU12LimitedRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_FullRangeRGB24_To_LimitedRangeYVU24_BT601();

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGB24, FrameType::FORMAT_Y_UV12_LIMITED_RANGE, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGB24::convertRGB24FullRangeToY_VU12LimitedRange), flag, TestFrameConverterRGB24::pixelFunctionRGBForY_UV12, TestFrameConverterRGB24::pixelFunctionY_UV12ForYUV24, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGB24::testRGB24FullRangeToY_UV12FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_FullRangeRGB24_To_FullRangeYUV24_BT601();

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGB24, FrameType::FORMAT_Y_UV12_FULL_RANGE, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGB24::convertRGB24FullRangeToY_UV12FullRange), flag, TestFrameConverterRGB24::pixelFunctionRGBForY_UV12, TestFrameConverterRGB24::pixelFunctionY_UV12ForYUV24, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGB24::testRGB24FullRangeToY_VU12FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_FullRangeRGB24_To_FullRangeYVU24_BT601();

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGB24, FrameType::FORMAT_Y_UV12_FULL_RANGE, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGB24::convertRGB24FullRangeToY_VU12FullRange), flag, TestFrameConverterRGB24::pixelFunctionRGBForY_UV12, TestFrameConverterRGB24::pixelFunctionY_UV12ForYUV24, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGB24::testRGB24FullRangeToY_U_V12LimitedRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_FullRangeRGB24_To_LimitedRangeYUV24_BT601();

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGB24, FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGB24::convertRGB24FullRangeToY_U_V12LimitedRange), flag, TestFrameConverterRGB24::pixelFunctionRGBForY_UV12, TestFrameConverterRGB24::pixelFunctionY_U_V12ForYUV24, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGB24::testRGB24FullRangeToY_V_U12LimitedRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_FullRangeRGB24_To_LimitedRangeYVU24_BT601();

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGB24, FrameType::FORMAT_Y_V_U12_LIMITED_RANGE, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGB24::convertRGB24FullRangeToY_V_U12LimitedRange), flag, TestFrameConverterRGB24::pixelFunctionRGBForY_UV12, TestFrameConverterRGB24::pixelFunctionY_U_V12ForYUV24, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGB24::testRGB24FullRangeToY_U_V12FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_FullRangeRGB24_To_FullRangeYUV24_BT601();

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGB24, FrameType::FORMAT_Y_U_V12_FULL_RANGE, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGB24::convertRGB24FullRangeToY_U_V12FullRange), flag, TestFrameConverterRGB24::pixelFunctionRGBForY_UV12, TestFrameConverterRGB24::pixelFunctionY_U_V12ForYUV24, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGB24::testRGB24FullRangeToY_V_U12FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_FullRangeRGB24_To_FullRangeYVU24_BT601();

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGB24, FrameType::FORMAT_Y_V_U12_FULL_RANGE, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGB24::convertRGB24FullRangeToY_V_U12FullRange), flag, TestFrameConverterRGB24::pixelFunctionRGBForY_UV12, TestFrameConverterRGB24::pixelFunctionY_U_V12ForYUV24, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

MatrixD TestFrameConverterRGB24::pixelFunctionRGBForY_UV12(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
{
	ocean_assert(frame.isValid());
	ocean_assert(x < frame.width() && y < frame.height());
	ocean_assert_and_suppress_unused(conversionFlag == CV::FrameConverter::CONVERT_NORMAL, conversionFlag);

	const unsigned int xTopLeft = (x / 2u) * 2u;
	const unsigned int yTopLeft = (y / 2u) * 2u;

	// first column is for the Y-channel, second and third column is for U and V

	MatrixD colorMatrix(3, 3);

	colorMatrix(0, 0) = double(frame.constpixel<uint8_t>(x, y)[0]);
	colorMatrix(1, 0) = double(frame.constpixel<uint8_t>(x, y)[1]);
	colorMatrix(2, 0) = double(frame.constpixel<uint8_t>(x, y)[2]);

	for (unsigned int nChannel = 0u; nChannel < 3u; ++nChannel)
	{
		// averaging every 2x2 block

		const double averageColorValue = double(frame.constpixel<uint8_t>(xTopLeft + 0u, yTopLeft + 0u)[nChannel]
												+ frame.constpixel<uint8_t>(xTopLeft + 1u, yTopLeft + 0u)[nChannel]
												+ frame.constpixel<uint8_t>(xTopLeft + 0u, yTopLeft + 1u)[nChannel]
												+ frame.constpixel<uint8_t>(xTopLeft + 1u, yTopLeft + 1u)[nChannel]) * 0.25;

		colorMatrix(nChannel, 1) = averageColorValue;
		colorMatrix(nChannel, 2) = averageColorValue;
	}

	return colorMatrix;
}

MatrixD TestFrameConverterRGB24::pixelFunctionY_UV12ForYUV24(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
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

MatrixD TestFrameConverterRGB24::pixelFunctionY_U_V12ForYUV24(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
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
