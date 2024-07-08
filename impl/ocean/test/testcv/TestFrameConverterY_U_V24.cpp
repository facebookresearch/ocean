/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverterY_U_V24.h"

#include "ocean/cv/FrameConverterY_U_V24.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameConverterY_U_V24::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	Log::info() << "---   Y_U_V24 converter test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	{
		Log::info() << "Testing Y_U_V24 to YUV24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_U_V24ToYUV24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing limited range Y_U_V24 to BGR24 (with 6 bit precision) conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_U_V24LimitedRangeToBGR24FullRangePrecision6Bit(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing limited range Y_U_V24 to RGB24 (with 6 bit precision) conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_U_V24LimitedRangeToRGB24FullRangePrecision6Bit(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing limited range Y_U_V24 to BGRA32 (with 6 bit precision) conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_U_V24LimitedRangeToBGRA32FullRangePrecision6Bit(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing limited range Y_U_V24 to RGBA32 (with 6 bit precision) conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_U_V24LimitedRangeToRGBA32FullRangePrecision6Bit(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing full range Y_U_V24 to BGRA32 (with 6 bit precision) Android-specific conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_U_V24FullRangeToBGRA32FullRangeAndroidPrecision6Bit(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Y_U_V24 converter tests succeeded.";
	}
	else
	{
		Log::info() << "Y_U_V24 converter tests FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterY_U_V24, Y_U_V24ToYUV24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V24::testY_U_V24ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V24, Y_U_V24ToYUV24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V24::testY_U_V24ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V24, Y_U_V24ToYUV24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V24::testY_U_V24ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V24, Y_U_V24ToYUV24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V24::testY_U_V24ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_U_V24, Y_U_V24LimitedRangeToBGR24FullRangePrecision6BitNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V24::testY_U_V24LimitedRangeToBGR24FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V24, Y_U_V24LimitedRangeToBGR24FullRangePrecision6BitFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V24::testY_U_V24LimitedRangeToBGR24FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V24, Y_U_V24LimitedRangeToBGR24FullRangePrecision6BitMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V24::testY_U_V24LimitedRangeToBGR24FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V24, Y_U_V24LimitedRangeToBGR24FullRangePrecision6BitFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V24::testY_U_V24LimitedRangeToBGR24FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_U_V24, Y_U_V24LimitedRangeToRGB24FullRangePrecision6BitNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V24::testY_U_V24LimitedRangeToRGB24FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V24, Y_U_V24LimitedRangeToRGB24FullRangePrecision6BitFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V24::testY_U_V24LimitedRangeToRGB24FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V24, Y_U_V24LimitedRangeToRGB24FullRangePrecision6BitMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V24::testY_U_V24LimitedRangeToRGB24FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V24, Y_U_V24LimitedRangeToRGB24FullRangePrecision6BitFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V24::testY_U_V24LimitedRangeToRGB24FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_U_V24, Y_U_V24LimitedRangeToBGRA32FullRangePrecision6BitNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V24::testY_U_V24LimitedRangeToBGRA32FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V24, Y_U_V24LimitedRangeToBGRA32FullRangePrecision6BitFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V24::testY_U_V24LimitedRangeToBGRA32FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V24, Y_U_V24LimitedRangeToBGRA32FullRangePrecision6BitMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V24::testY_U_V24LimitedRangeToBGRA32FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V24, Y_U_V24LimitedRangeToBGRA32FullRangePrecision6BitFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V24::testY_U_V24LimitedRangeToBGRA32FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_U_V24, Y_U_V24LimitedRangeToRGBA32FullRangePrecision6BitNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V24::testY_U_V24LimitedRangeToRGBA32FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V24, Y_U_V24LimitedRangeToRGBA32FullRangePrecision6BitFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V24::testY_U_V24LimitedRangeToRGBA32FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V24, Y_U_V24LimitedRangeToRGBA32FullRangePrecision6BitMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V24::testY_U_V24LimitedRangeToRGBA32FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V24, Y_U_V24LimitedRangeToRGBA32FullRangePrecision6BitFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V24::testY_U_V24LimitedRangeToRGBA32FullRangePrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_U_V24, Y_U_V24FullRangeToBGRA32FullRangeAndroidPrecision6BitNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V24::testY_U_V24FullRangeToBGRA32FullRangeAndroidPrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V24, Y_U_V24FullRangeToBGRA32FullRangeAndroidPrecision6BitFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V24::testY_U_V24FullRangeToBGRA32FullRangeAndroidPrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V24, Y_U_V24FullRangeToBGRA32FullRangeAndroidPrecision6BitMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V24::testY_U_V24FullRangeToBGRA32FullRangeAndroidPrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_U_V24, Y_U_V24FullRangeToBGRA32FullRangeAndroidPrecision6BitFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_U_V24::testY_U_V24FullRangeToBGRA32FullRangeAndroidPrecision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterY_U_V24::testY_U_V24ToYUV24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix(3, 3, true);

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y_U_V24, FrameType::FORMAT_YUV24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY_U_V24::convertY_U_V24ToYUV24), conversionFlag, pixelFunctionY_U_V24ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY_U_V24::testY_U_V24LimitedRangeToBGR24FullRangePrecision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix(CV::FrameConverter::transformationMatrix_LimitedRangeYUV24_To_FullRangeBGR24_BT601());

	constexpr unsigned int thresholdMaximalErrorToInteger = 6u;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y_U_V24, FrameType::FORMAT_BGR24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY_U_V24::convertY_U_V24LimitedRangeToBGR24), conversionFlag, pixelFunctionY_U_V24ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

bool TestFrameConverterY_U_V24::testY_U_V24LimitedRangeToRGB24FullRangePrecision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix(CV::FrameConverter::transformationMatrix_LimitedRangeYUV24_To_FullRangeRGB24_BT601());

	constexpr unsigned int thresholdMaximalErrorToInteger = 6u;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y_U_V24, FrameType::FORMAT_RGB24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY_U_V24::convertY_U_V24LimitedRangeToRGB24), conversionFlag, pixelFunctionY_U_V24ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

bool TestFrameConverterY_U_V24::testY_U_V24LimitedRangeToBGRA32FullRangePrecision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	MatrixD transformationMatrix(4, 4, CV::FrameConverter::transformationMatrix_LimitedRangeYUV24_To_FullRangeBGR24_BT601(), 0, 0);
	transformationMatrix(3, 3) = double(TestFrameConverter::ValueProvider::get().alphaValue());

	constexpr unsigned int thresholdMaximalErrorToInteger = 6u;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y_U_V24, FrameType::FORMAT_BGRA32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY_U_V24::convertY_U_V24LimitedRangeToBGRA32), conversionFlag, pixelFunctionY_U_V24ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

bool TestFrameConverterY_U_V24::testY_U_V24LimitedRangeToRGBA32FullRangePrecision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	MatrixD transformationMatrix(4, 4, CV::FrameConverter::transformationMatrix_LimitedRangeYUV24_To_FullRangeRGB24_BT601(), 0, 0);
	transformationMatrix(3, 3) = double(TestFrameConverter::ValueProvider::get().alphaValue());

	constexpr unsigned int thresholdMaximalErrorToInteger = 6u;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y_U_V24, FrameType::FORMAT_RGBA32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY_U_V24::convertY_U_V24LimitedRangeToRGBA32), conversionFlag, pixelFunctionY_U_V24ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

bool TestFrameConverterY_U_V24::testY_U_V24FullRangeToBGRA32FullRangeAndroidPrecision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	MatrixD transformationMatrix(4, 4, CV::FrameConverter::transformationMatrix_FullRangeYUV24_To_FullRangeBGR24_Android(), 0, 0);
	transformationMatrix(3, 3) = double(TestFrameConverter::ValueProvider::get().alphaValue());

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y_U_V24, FrameType::FORMAT_BGRA32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY_U_V24::convertY_U_V24FullRangeToBGRA32FullRangeAndroid), conversionFlag, pixelFunctionY_U_V24ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

MatrixD TestFrameConverterY_U_V24::pixelFunctionY_U_V24ForYUV24(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
{
	ocean_assert(frame.isValid());
	ocean_assert(x < frame.width() && y < frame.height());
	ocean_assert_and_suppress_unused(conversionFlag == CV::FrameConverter::CONVERT_NORMAL, conversionFlag); // we expect the target frame to have a conversion flag

	MatrixD colorVector(3, 1);

	colorVector(0, 0) = double(*frame.constpixel<uint8_t>(x, y, 0u));
	colorVector(1, 0) = double(*frame.constpixel<uint8_t>(x, y, 1u));
	colorVector(2, 0) = double(*frame.constpixel<uint8_t>(x, y, 2u));

	return colorVector;
}

}

}

}
