/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverterY_U_V12.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/FrameConverterY_U_V12.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"
#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameConverterY_U_V12::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	TestResult testResult("Y_U_V12 converter test");

	if (selector.shouldRun("y_u_v12toy_u_v12"))
	{
		Log::info() << "Testing Y_U_V12 to Y_U_V12 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testY_U_V12ToY_U_V12(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("y_u_v12limitedrangetobgr24fullrange"))
	{
		Log::info() << "Testing Y_U_V12_LIMITED_RANGE to BGR24 (full range) conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testY_U_V12LimitedRangeToBGR24FullRange(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("y_u_v12fullrangetobgr24fullrangeprecision6bit"))
	{
		Log::info() << "Testing Y_U_V12_LIMITED_RANGE to BGR24 (full range) conversion with resolution " << width << "x" << height << " (6 bit):";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testY_U_V12FullRangeToBGR24FullRangePrecision6Bit(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("y_u_v12limitedrangetobgra32fullrangeprecision6bit"))
	{
		Log::info() << "Testing Y_U_V12_LIMITED_RANGE to BGRA32 (full range) conversion with resolution " << width << "x" << height << " (6 bit):";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testY_U_V12LimitedRangeToBGRA32FullRangePrecision6Bit(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("y_u_v12limitedrangetorgb24fullrange"))
	{
		Log::info() << "Testing Y_U_V12_LIMITED_RANGE to RGB24 (full range) conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testY_U_V12LimitedRangeToRGB24FullRange(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("y_u_v12limitedrangetorgb24fullrangeprecision6bit"))
	{
		Log::info() << "Testing Y_U_V12_LIMITED_RANGE to RGB24 (full range) conversion with resolution " << width << "x" << height << " (6 bit):";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testY_U_V12LimitedRangeToRGB24FullRangePrecision6Bit(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("y_u_v12fullrangetorgb24fullrangeprecision6bit"))
	{
		Log::info() << "Testing Y_U_V12_FULL_RANGE to RGB24 (full range) conversion with resolution " << width << "x" << height << " (6 bit):";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testY_U_V12FullRangeToRGB24FullRangePrecision6Bit(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("y_u_v12limitedrangetorgba32fullrangeprecision6bit"))
	{
		Log::info() << "Testing Y_U_V12_LIMITED_RANGE to RGBA32 (full range) conversion with resolution " << width << "x" << height << " (6 bit):";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testY_U_V12LimitedRangeToRGBA32FullRangePrecision6Bit(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("y_u_v12toyuv24"))
	{
		Log::info() << "Testing Y_U_V12 to YUV24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testY_U_V12ToYUV24(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("y_u_v12toyvu24"))
	{
		Log::info() << "Testing Y_U_V12 to YVU24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testY_U_V12ToYVU24(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("y_u_v12limitedrangetoy8limitedrange"))
	{
		Log::info() << "Testing Y_U_V12_LIMITED_RANGE to Y8_LIMITED_RANGE conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testY_U_V12LimitedRangeToY8LimitedRange(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("y_u_v12fullrangetoy8fullrange"))
	{
		Log::info() << "Testing Y_U_V12_FULL_RANGE to Y8_FULL_RANGE conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testY_U_V12FullRangeToY8FullRange(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("y_u_v12limitedrangetoy8fullrange"))
	{
		Log::info() << "Testing Y_U_V12_LIMITED_RANGE to Y8_FULL_RANGE conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testY_U_V12LimitedRangeToY8FullRange(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("y_u_v12fullrangetoy8limitedrange"))
	{
		Log::info() << "Testing Y_U_V12_FULL_RANGE to Y8_LIMITED_RANGE conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testY_U_V12FullRangeToY8LimitedRange(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("y_u_v12toy_uv12"))
	{
		Log::info() << "Testing Y_U_V12 to Y_UV12 conversion with resolution " << width << "x" << height << ":";
		Log::info() << " ";

		testResult = testY_U_V12ToY_UV12(width, height, testDuration, worker);

		Log::info() << " ";
	}

	return testResult.succeeded();
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


TEST(TestFrameConverterY_U_V12, Y_U_V12LimitedRangeToY8LimitedRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToY8LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12LimitedRangeToY8LimitedRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToY8LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12LimitedRangeToY8LimitedRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToY8LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12LimitedRangeToY8LimitedRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToY8LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_U_V12, Y_U_V12FullRangeToY8FullRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12FullRangeToY8FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12FullRangeToY8FullRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12FullRangeToY8FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12FullRangeToY8FullRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12FullRangeToY8FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12FullRangeToY8FullRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12FullRangeToY8FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_U_V12, Y_U_V12LimitedRangeToY8FullRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToY8FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12LimitedRangeToY8FullRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToY8FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12LimitedRangeToY8FullRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToY8FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12LimitedRangeToY8FullRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToY8FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_U_V12, Y_U_V12FullRangeToY8LimitedRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12FullRangeToY8LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12FullRangeToY8LimitedRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12FullRangeToY8LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12FullRangeToY8LimitedRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12FullRangeToY8LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12FullRangeToY8LimitedRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12FullRangeToY8LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V12, Y_U_V12ToY_UV12)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V12::testY_U_V12ToY_UV12(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterY_U_V12::testY_U_V12ToY_U_V12(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width >= 2u && height >= 2u);

	const MatrixD transformationMatrix(3, 3, true);

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_U_V12, FrameType::FORMAT_Y_U_V12, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_U_V12::convertY_U_V12ToY_U_V12), conversionFlag, pixelFunctionY_U_V12ForYUV24, pixelFunctionY_U_V12ForYUV24, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToBGR24FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_LimitedRangeYUV24_To_FullRangeBGR24_BT601();

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_BGR24, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_U_V12::convertY_U_V12LimitedRangeToBGR24FullRange), conversionFlag, pixelFunctionY_U_V12ForYUV24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY_U_V12::testY_U_V12FullRangeToBGR24FullRangePrecision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_FullRangeYUV24_To_FullRangeBGR24_BT601();

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_U_V12_FULL_RANGE, FrameType::FORMAT_BGR24, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_U_V12::convertY_U_V12FullRangeToBGR24FullRangePrecision6Bit), conversionFlag, pixelFunctionY_U_V12ForYUV24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
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
	transformationMatrix(3, 3) = double(FrameConverterTestUtilities::ValueProvider::get().alphaValue());

	constexpr unsigned int thresholdMaximalErrorToInteger = 6u;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_BGRA32, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_U_V12::convertY_U_V12LimitedRangeToBGRA32FullRangePrecision6Bit), conversionFlag, pixelFunctionY_U_V12ForYUV24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

bool TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToRGB24FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_LimitedRangeYUV24_To_FullRangeRGB24_BT601();

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_RGB24, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_U_V12::convertY_U_V12LimitedRangeToRGB24FullRange), conversionFlag, pixelFunctionY_U_V12ForYUV24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToRGB24FullRangePrecision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_LimitedRangeYUV24_To_FullRangeRGB24_BT601();

	constexpr unsigned int thresholdMaximalErrorToInteger = 6u;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_RGB24, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_U_V12::convertY_U_V12LimitedRangeToRGB24FullRangePrecision6Bit), conversionFlag, pixelFunctionY_U_V12ForYUV24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

bool TestFrameConverterY_U_V12::testY_U_V12FullRangeToRGB24FullRangePrecision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_FullRangeYUV24_To_FullRangeRGB24_BT601();

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_U_V12_FULL_RANGE, FrameType::FORMAT_RGB24, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_U_V12::convertY_U_V12FullRangeToRGB24FullRangePrecision6Bit), conversionFlag, pixelFunctionY_U_V12ForYUV24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
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
	transformationMatrix(3, 3) = double(FrameConverterTestUtilities::ValueProvider::get().alphaValue());

	constexpr unsigned int thresholdMaximalErrorToInteger = 6u;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_RGBA32, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_U_V12::convertY_U_V12LimitedRangeToRGBA32FullRangePrecision6Bit), conversionFlag, pixelFunctionY_U_V12ForYUV24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
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

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_YUV24, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_U_V12::convertY_U_V12ToYUV24), conversionFlag, pixelFunctionY_U_V12ForYUV24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
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

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_YVU24, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_U_V12::convertY_U_V12ToYVU24), conversionFlag, pixelFunctionY_U_V12ForYUV24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

bool TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToY8LimitedRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	//                     | Y |
	// | Y | = | 1 0 0 | * | U |
	//                     | V |

	MatrixD transformationMatrix(1, 3, false);
	transformationMatrix(0, 0) = 1.0;

	const unsigned int thresholdMaximalErrorToInteger = 0u;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_Y8_LIMITED_RANGE, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_U_V12::convertY_U_V12LimitedRangeToY8LimitedRange), conversionFlag, pixelFunctionY_U_V12ForYUV24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

bool TestFrameConverterY_U_V12::testY_U_V12FullRangeToY8FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	//                     | Y |
	// | Y | = | 1 0 0 | * | U |
	//                     | V |

	MatrixD transformationMatrix(1, 3, false);
	transformationMatrix(0, 0) = 1.0;

	const unsigned int thresholdMaximalErrorToInteger = 0u;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_U_V12_FULL_RANGE, FrameType::FORMAT_Y8_FULL_RANGE, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_U_V12::convertY_U_V12FullRangeToY8FullRange), conversionFlag, pixelFunctionY_U_V12ForYUV24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

bool TestFrameConverterY_U_V12::testY_U_V12LimitedRangeToY8FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// Y_full = (Y_limited - 16) * 255 / 219

	// | Y_full | = | 255/219  0  0  -16*255/219 | * | Y |
	//                                               | U |
	//                                               | V |
	//                                               | 1 |

	MatrixD transformationMatrix(1, 4, false);
	transformationMatrix(0, 0) = 255.0 / 219.0;
	transformationMatrix(0, 3) = -16.0 * 255.0 / 219.0;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_Y8_FULL_RANGE, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_U_V12::convertY_U_V12LimitedRangeToY8FullRange), conversionFlag, pixelFunctionY_U_V12ForYUV24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY_U_V12::testY_U_V12FullRangeToY8LimitedRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// Y_limited = Y_full * 219 / 255 + 16

	// | Y_limited | = | 219/255  0  0  16 | * | Y |
	//                                         | U |
	//                                         | V |
	//                                         | 1 |

	MatrixD transformationMatrix(1, 4, false);
	transformationMatrix(0, 0) = 219.0 / 255.0;
	transformationMatrix(0, 3) = 16.0;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y_U_V12_FULL_RANGE, FrameType::FORMAT_Y8_LIMITED_RANGE, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY_U_V12::convertY_U_V12FullRangeToY8LimitedRange), conversionFlag, pixelFunctionY_U_V12ForYUV24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY_U_V12::testY_U_V12ToY_UV12(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 2u && height >= 2u);
	ocean_assert(width % 2u == 0u && height % 2u == 0u);
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool useWorker : {false, true})
		{
			const unsigned int testWidth = RandomI::random(randomGenerator, 1u, width / 2u) * 2u;
			const unsigned int testHeight = RandomI::random(randomGenerator, 1u, height / 2u) * 2u;

			const unsigned int testWidth_2 = testWidth / 2u;
			const unsigned int testHeight_2 = testHeight / 2u;

			// a pixel stride larger than 1 describes an interleaved source plane, as delivered by e.g. a semi-planar camera buffer
			const unsigned int ySourcePixelStride = RandomI::random(randomGenerator, 1u, 4u);
			const unsigned int uSourcePixelStride = RandomI::random(randomGenerator, 1u, 4u);
			const unsigned int vSourcePixelStride = RandomI::random(randomGenerator, 1u, 4u);

			// the converter determines the source row stride as 'width + padding', so the elements skipped between two
			// pixels of an interleaved plane are part of that plane's padding
			const unsigned int ySourcePaddingElements = testWidth * (ySourcePixelStride - 1u) + RandomI::random(randomGenerator, 0u, 8u);
			const unsigned int uSourcePaddingElements = testWidth_2 * (uSourcePixelStride - 1u) + RandomI::random(randomGenerator, 0u, 8u);
			const unsigned int vSourcePaddingElements = testWidth_2 * (vSourcePixelStride - 1u) + RandomI::random(randomGenerator, 0u, 8u);

			const unsigned int yTargetPaddingElements = RandomI::random(randomGenerator, 0u, 8u);
			const unsigned int uvTargetPaddingElements = RandomI::random(randomGenerator, 0u, 8u);

			std::vector<uint8_t> ySource((testWidth + ySourcePaddingElements) * testHeight);
			std::vector<uint8_t> uSource((testWidth_2 + uSourcePaddingElements) * testHeight_2);
			std::vector<uint8_t> vSource((testWidth_2 + vSourcePaddingElements) * testHeight_2);

			std::vector<uint8_t> yTarget((testWidth + yTargetPaddingElements) * testHeight);
			std::vector<uint8_t> uvTarget((testWidth + uvTargetPaddingElements) * testHeight_2);

			for (std::vector<uint8_t>* buffer : {&ySource, &uSource, &vSource, &yTarget, &uvTarget})
			{
				for (uint8_t& element : *buffer)
				{
					element = uint8_t(RandomI::random(randomGenerator, 255u));
				}
			}

			const std::vector<uint8_t> copyYTarget(yTarget);
			const std::vector<uint8_t> copyUVTarget(uvTarget);

			CV::FrameConverterY_U_V12::convertY_U_V12ToY_UV12(ySource.data(), uSource.data(), vSource.data(), yTarget.data(), uvTarget.data(), testWidth, testHeight, ySourcePaddingElements, uSourcePaddingElements, vSourcePaddingElements, yTargetPaddingElements, uvTargetPaddingElements, ySourcePixelStride, uSourcePixelStride, vSourcePixelStride, useWorker ? &worker : nullptr);

			OCEAN_EXPECT_TRUE(validation, validateY_U_V12ToY_UV12(ySource.data(), uSource.data(), vSource.data(), yTarget.data(), uvTarget.data(), testWidth, testHeight, ySourcePaddingElements, uSourcePaddingElements, vSourcePaddingElements, yTargetPaddingElements, uvTargetPaddingElements, ySourcePixelStride, uSourcePixelStride, vSourcePixelStride));

			// the converter must not touch the padding of the target planes

			for (unsigned int y = 0u; y < testHeight; ++y)
			{
				const unsigned int rowOffset = y * (testWidth + yTargetPaddingElements) + testWidth;

				OCEAN_EXPECT_TRUE(validation, memcmp(yTarget.data() + rowOffset, copyYTarget.data() + rowOffset, yTargetPaddingElements) == 0);
			}

			for (unsigned int y = 0u; y < testHeight_2; ++y)
			{
				const unsigned int rowOffset = y * (testWidth + uvTargetPaddingElements) + testWidth;

				OCEAN_EXPECT_TRUE(validation, memcmp(uvTarget.data() + rowOffset, copyUVTarget.data() + rowOffset, uvTargetPaddingElements) == 0);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestFrameConverterY_U_V12::validateY_U_V12ToY_UV12(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, const uint8_t* yTarget, const uint8_t* uvTarget, const unsigned int width, const unsigned int height, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int uvTargetPaddingElements, const unsigned int ySourcePixelStride, const unsigned int uSourcePixelStride, const unsigned int vSourcePixelStride)
{
	ocean_assert(ySource != nullptr && uSource != nullptr && vSource != nullptr);
	ocean_assert(yTarget != nullptr && uvTarget != nullptr);

	ocean_assert(width >= 2u && height >= 2u);
	ocean_assert(width % 2u == 0u && height % 2u == 0u);

	ocean_assert(ySourcePixelStride >= 1u && uSourcePixelStride >= 1u && vSourcePixelStride >= 1u);

	const unsigned int width_2 = width / 2u;
	const unsigned int height_2 = height / 2u;

	const unsigned int ySourceStrideElements = width + ySourcePaddingElements;
	const unsigned int uSourceStrideElements = width_2 + uSourcePaddingElements;
	const unsigned int vSourceStrideElements = width_2 + vSourcePaddingElements;

	const unsigned int yTargetStrideElements = width + yTargetPaddingElements;
	const unsigned int uvTargetStrideElements = width + uvTargetPaddingElements;

	// the y plane is copied 1:1, the u and v planes are zipped into one uv plane holding one row per two y rows

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			if (yTarget[y * yTargetStrideElements + x] != ySource[y * ySourceStrideElements + x * ySourcePixelStride])
			{
				return false;
			}
		}
	}

	for (unsigned int y = 0u; y < height_2; ++y)
	{
		for (unsigned int x = 0u; x < width_2; ++x)
		{
			if (uvTarget[y * uvTargetStrideElements + x * 2u + 0u] != uSource[y * uSourceStrideElements + x * uSourcePixelStride])
			{
				return false;
			}

			if (uvTarget[y * uvTargetStrideElements + x * 2u + 1u] != vSource[y * vSourceStrideElements + x * vSourcePixelStride])
			{
				return false;
			}
		}
	}

	return true;
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
