/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverterYUV24.h"
#include "ocean/test/testcv/TestFrameConverter.h"

#include "ocean/cv/FrameConverterYUV24.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameConverterYUV24::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	Log::info() << "---   YUV24 converter test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	{
		Log::info() << "Testing YUV24 to BGR24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testYUV24ToBGR24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing YUV24 to BGRA32 (with 6 bit precision) conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testYUV24ToBGRA32Precision6Bit(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing YUV24 to RGB24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testYUV24ToRGB24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing YUV24 to RGB24 (with 6 bit precision) conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testYUV24ToRGB24Precision6Bit(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing YUV24 to Y8 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testYUV24ToY8(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing YUV24 to YUV24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testYUV24ToYUV24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing YUV24 to YVU24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testYUV24ToYVU24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing YUV24 to Y_U_V12 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testYUV24ToY_U_V12(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "YUV24 converter tests succeeded.";
	}
	else
	{
		Log::info() << "YUV24 converter tests FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterYUV24, YUV24ToBGR24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUV24, YUV24ToBGR24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUV24, YUV24ToBGR24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUV24, YUV24ToBGR24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterYUV24, YUV24ToBGRA32Precision6BitNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToBGRA32Precision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUV24, YUV24ToBGRA32Precision6BitFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToBGRA32Precision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUV24, YUV24ToBGRA32Precision6BitMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToBGRA32Precision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUV24, YUV24ToBGRA32Precision6BitFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToBGRA32Precision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterYUV24, YUV24ToRGB24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUV24, YUV24ToRGB24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUV24, YUV24ToRGB24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUV24, YUV24ToRGB24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterYUV24, YUV24ToRGB24Precision6BitNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToRGB24Precision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUV24, YUV24ToRGB24Precision6BitFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToRGB24Precision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUV24, YUV24ToRGB24Precision6BitMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToRGB24Precision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUV24, YUV24ToRGB24Precision6BitFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToRGB24Precision6Bit(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterYUV24, YUV24ToY8Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUV24, YUV24ToY8Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUV24, YUV24ToY8Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUV24, YUV24ToY8FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterYUV24, YUV24ToYUV24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUV24, YUV24ToYUV24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUV24, YUV24ToYUV24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUV24, YUV24ToYUV24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterYUV24, YUV24ToYVU24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToYVU24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUV24, YUV24ToYVU24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToYVU24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUV24, YUV24ToYVU24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToYVU24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUV24, YUV24ToYVU24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToYVU24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterYUV24, YUV24ToY_U_V12Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToY_U_V12(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUV24, YUV24ToY_U_V12Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToY_U_V12(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUV24, YUV24ToY_U_V12Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToY_U_V12(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUV24, YUV24ToY_U_V12FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUV24::testYUV24ToY_U_V12(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterYUV24::testYUV24ToBGR24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_LimitedRangeYUV24_To_FullRangeBGR24_BT601();

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_YUV24, FrameType::FORMAT_BGR24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterYUV24::convertYUV24ToBGR24), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterYUV24::testYUV24ToBGRA32Precision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	MatrixD transformationMatrix(4, 4, CV::FrameConverter::transformationMatrix_FullRangeYUV24_To_FullRangeBGR24_Android(), 0, 0);
	transformationMatrix(3, 3) = double(TestFrameConverter::ValueProvider::get().alphaValue());

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_YUV24, FrameType::FORMAT_BGRA32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterYUV24::convertYUV24ToBGRA32Precision6Bit), conversionFlag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterYUV24::testYUV24ToRGB24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_LimitedRangeYUV24_To_FullRangeRGB24_BT601();

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_YUV24, FrameType::FORMAT_RGB24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterYUV24::convertYUV24ToRGB24), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterYUV24::testYUV24ToRGB24Precision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_FullRangeYUV24_To_FullRangeRGB24_Android();

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_YUV24, FrameType::FORMAT_RGB24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterYUV24::convertYUV24ToRGB24Precision6Bit), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterYUV24::testYUV24ToY8(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	//                     | Y |
	// | Y | = | 1 0 0 | * | U |
	//                     | V |

	MatrixD transformationMatrix(1, 3, false);
	transformationMatrix(0, 0) = 1.0;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_YUV24, FrameType::FORMAT_Y8, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterYUV24::convertYUV24ToY8), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterYUV24::testYUV24ToYUV24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y |   | 1 0 0 |   | Y |
	// | U | = | 0 1 0 | * | U |
	// | V |   | 0 0 1 |   | V |

	const MatrixD transformationMatrix(3, 3, true);

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_YUV24, FrameType::FORMAT_YUV24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterYUV24::convertYUV24ToYUV24), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterYUV24::testYUV24ToYVU24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
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

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_YUV24, FrameType::FORMAT_YVU24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterYUV24::convertYUV24ToYVU24), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterYUV24::testYUV24ToY_U_V12(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y |   | 1 0 0 |   | Y |
	// | U | = | 0 1 0 | * | U |
	// | V |   | 0 0 1 |   | V |

	const MatrixD transformationMatrix(3, 3, true);

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_YUV24, FrameType::FORMAT_Y_U_V12, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterYUV24::convertYUV24ToY_U_V12), conversionFlag, pixelFunctionYUV24ForY_U_V12, pixelFunctionY_U_V12, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

MatrixD TestFrameConverterYUV24::pixelFunctionYUV24ForY_U_V12(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
{
	ocean_assert(frame.isValid());
	ocean_assert(x < frame.width() && y < frame.height());
	ocean_assert_and_suppress_unused(conversionFlag == CV::FrameConverter::CONVERT_NORMAL, conversionFlag); // we expect the target frame to have a conversion flag

	MatrixD colorVector(3, 1);

	colorVector(0, 0) = double(frame.constpixel<uint8_t>(x, y)[0]);

	const unsigned int xLeft = x & 0xFFFFFFFEu; // make value even
	const unsigned int yTop = y & 0xFFFFFFFEu;

	//         012 345
	// top:    YUV YUV
	// bottom: YUV YUV
	colorVector(1, 0) = double((frame.constpixel<uint8_t>(xLeft, yTop)[1u] + frame.constpixel<uint8_t>(xLeft + 1u, yTop)[1u] + frame.constpixel<uint8_t>(xLeft, yTop + 1u)[1u] + frame.constpixel<uint8_t>(xLeft + 1u, yTop + 1u)[1u] + 2u) / 4u);
	colorVector(2, 0) = double((frame.constpixel<uint8_t>(xLeft, yTop)[2u] + frame.constpixel<uint8_t>(xLeft + 1u, yTop)[2u] + frame.constpixel<uint8_t>(xLeft, yTop + 1u)[2u] + frame.constpixel<uint8_t>(xLeft + 1u, yTop + 1u)[2u] + 2u) / 4u);

	return colorVector;
}

MatrixD TestFrameConverterYUV24::pixelFunctionY_U_V12(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
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
