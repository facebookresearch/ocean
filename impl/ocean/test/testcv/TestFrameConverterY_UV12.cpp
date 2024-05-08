/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverterY_UV12.h"

#include "ocean/cv/FrameConverterY_UV12.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameConverterY_UV12::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	Log::info() << "---   Y_UV12 converter test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	{
		Log::info() << "Testing Y_UV12 (limited range) to BGR24 (full range) conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_UV12LimitedRangeToBGR24FullRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_UV12 (limited range) to RGB24 (full range) conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_UV12LimitedRangeToRGB24FullRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_UV12 (full range) to BGR24 (full range) conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_UV12FullRangeToBGR24FullRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_UV12 (full range) to RGB24 (full range) conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_UV12FullRangeToRGB24FullRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_UV12 to YUV24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_UV12ToYUV24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_UV12 to YVU24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_UV12ToYVU24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_UV12 to Y8 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_UV12ToY8(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y_UV12 to Y_U_V12 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY_UV12ToY_U_V12(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Y_UV12 converter tests succeeded.";
	}
	else
	{
		Log::info() << "Y_UV12 converter tests FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterY_UV12, Y_UV12LimitedRangeToBGR24FullRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12LimitedRangeToBGR24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_UV12, Y_UV12LimitedRangeToBGR24FullRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12LimitedRangeToBGR24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_UV12, Y_UV12LimitedRangeToBGR24FullRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12LimitedRangeToBGR24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_UV12,Y_UV12LimitedRangeToBGR24FullRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12LimitedRangeToBGR24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_UV12, Y_UV12LimitedRangeToRGB24FullRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12LimitedRangeToRGB24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_UV12, Y_UV12LimitedRangeToRGB24FullRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12LimitedRangeToRGB24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_UV12, Y_UV12LimitedRangeToRGB24FullRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12LimitedRangeToRGB24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_UV12, Y_UV12LimitedRangeToRGB24FullRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12LimitedRangeToRGB24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_UV12, Y_UV12FullRangeToBGR24FullRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12FullRangeToBGR24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_UV12, Y_UV12FullRangeToBGR24FullRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12FullRangeToBGR24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_UV12, Y_UV12FullRangeToBGR24FullRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12FullRangeToBGR24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_UV12, Y_UV12FullRangeToBGR24FullRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12FullRangeToBGR24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_UV12, Y_UV12FullRangeToRGB24FullRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12FullRangeToRGB24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_UV12, Y_UV12FullRangeToRGB24FullRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12FullRangeToRGB24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_UV12, Y_UV12FullRangeToRGB24FullRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12FullRangeToRGB24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_UV12, Y_UV12FullRangeToRGB24FullRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12FullRangeToRGB24FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_UV12, Y_UV12ToYUV24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_UV12, Y_UV12ToYUV24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_UV12, Y_UV12ToYUV24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_UV12, Y_UV12ToYUV24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_UV12, Y_UV12ToYVU24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12ToYVU24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_UV12, Y_UV12ToYVU24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12ToYVU24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_UV12, Y_UV12ToYVU24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12ToYVU24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_UV12, Y_UV12ToYVU24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12ToYVU24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_UV12, Y_UV12ToY8Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_UV12, Y_UV12ToY8Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_UV12, Y_UV12ToY8Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_UV12, Y_UV12ToY8FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY_UV12, Y_UV12ToY_U_V12Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12ToY_U_V12(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_UV12, Y_UV12ToY_U_V12Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12ToY_U_V12(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_UV12, Y_UV12ToY_U_V12Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12ToY_U_V12(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY_UV12, Y_UV12ToY_U_V12FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY_UV12::testY_UV12ToY_U_V12(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterY_UV12::testY_UV12LimitedRangeToBGR24FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_LimitedRangeYUV24_To_FullRangeBGR24_BT601();

	constexpr unsigned int thresholdMaximalErrorToInteger = 5u;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y_UV12_LIMITED_RANGE, FrameType::FORMAT_BGR24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY_UV12::convertY_UV12LimitedRangeToBGR24FullRange), conversionFlag, pixelFunctionY_UV12ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

bool TestFrameConverterY_UV12::testY_UV12LimitedRangeToRGB24FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_LimitedRangeYUV24_To_FullRangeRGB24_BT601();

	constexpr unsigned int thresholdMaximalErrorToInteger = 5u;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y_UV12_LIMITED_RANGE, FrameType::FORMAT_RGB24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY_UV12::convertY_UV12LimitedRangeToRGB24FullRange), conversionFlag, pixelFunctionY_UV12ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

bool TestFrameConverterY_UV12::testY_UV12FullRangeToBGR24FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_FullRangeYUV24_To_FullRangeBGR24_BT601();

	constexpr unsigned int thresholdMaximalErrorToInteger = 5u;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y_UV12_FULL_RANGE, FrameType::FORMAT_BGR24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY_UV12::convertY_UV12FullRangeToBGR24FullRange), conversionFlag, pixelFunctionY_UV12ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

bool TestFrameConverterY_UV12::testY_UV12FullRangeToRGB24FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_FullRangeYUV24_To_FullRangeRGB24_BT601();

	constexpr unsigned int thresholdMaximalErrorToInteger = 5u;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y_UV12_FULL_RANGE, FrameType::FORMAT_RGB24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY_UV12::convertY_UV12FullRangeToRGB24FullRange), conversionFlag, pixelFunctionY_UV12ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger);
}

bool TestFrameConverterY_UV12::testY_UV12ToYUV24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y |   | 1 0 0 |   | Y |
	// | U | = | 0 1 0 | * | U |
	// | V |   | 0 0 1 |   | V |

	const MatrixD transformationMatrix(3, 3, true);

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y_UV12, FrameType::FORMAT_YUV24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY_UV12::convertY_UV12ToYUV24), conversionFlag, pixelFunctionY_UV12ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY_UV12::testY_UV12ToYVU24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
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

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y_UV12, FrameType::FORMAT_YVU24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY_UV12::convertY_UV12ToYVU24), conversionFlag, pixelFunctionY_UV12ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY_UV12::testY_UV12ToY8(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	//                     | Y |
	// | Y | = | 1 0 0 | * | U |
	//                     | V |

	MatrixD transformationMatrix(1, 3, false);
	transformationMatrix(0, 0) = 1.0;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y_UV12, FrameType::FORMAT_Y8, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY_UV12::convertY_UV12ToY8), conversionFlag, pixelFunctionY_UV12ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY_UV12::testY_UV12ToY_U_V12(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y |   | 1 0 0 |   | Y |
	// | U | = | 0 1 0 | * | U |
	// | V |   | 0 0 1 |   | V |

	const MatrixD transformationMatrix(3, 3, true);

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y_UV12, FrameType::FORMAT_Y_U_V12, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY_UV12::convertY_UV12ToY_U_V12), conversionFlag, pixelFunctionY_UV12ForYUV24, pixelFunctionY_U_V12ForYUV24, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

MatrixD TestFrameConverterY_UV12::pixelFunctionY_UV12ForYUV24(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
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

MatrixD TestFrameConverterY_UV12::pixelFunctionY_U_V12ForYUV24(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
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
