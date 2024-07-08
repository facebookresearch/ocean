/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverterYUYV16.h"

#include "ocean/cv/FrameConverterYUYV16.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameConverterYUYV16::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	Log::info() << "---   YUYV16 converter test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	{
		Log::info() << "Testing YUYV16 to BGR24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testYUYV16ToBGR24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing YUYV16 to RGB24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testYUYV16ToRGB24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing YUYV16 to Y8 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testYUYV16ToY8(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing YUYV16 to YUV24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testYUYV16ToYUV24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing YUYV16 to YVU24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testYUYV16ToYVU24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "YUYV16 converter tests succeeded.";
	}
	else
	{
		Log::info() << "YUYV16 converter tests FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterYUYV16, YUYV16ToBGR24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUYV16::testYUYV16ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUYV16, YUYV16ToBGR24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUYV16::testYUYV16ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUYV16, YUYV16ToBGR24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUYV16::testYUYV16ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUYV16, YUYV16ToBGR24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUYV16::testYUYV16ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterYUYV16, YUYV16ToRGB24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUYV16::testYUYV16ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUYV16, YUYV16ToRGB24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUYV16::testYUYV16ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUYV16, YUYV16ToRGB24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUYV16::testYUYV16ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUYV16, YUYV16ToRGB24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUYV16::testYUYV16ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterYUYV16, YUYV16ToY8Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUYV16::testYUYV16ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUYV16, YUYV16ToY8Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUYV16::testYUYV16ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUYV16, YUYV16ToY8Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUYV16::testYUYV16ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUYV16, YUYV16ToY8FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUYV16::testYUYV16ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterYUYV16, YUYV16ToYUV24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUYV16::testYUYV16ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUYV16, YUYV16ToYUV24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUYV16::testYUYV16ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUYV16, YUYV16ToYUV24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUYV16::testYUYV16ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUYV16, YUYV16ToYUV24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUYV16::testYUYV16ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterYUYV16, YUYV16ToYVU24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUYV16::testYUYV16ToYVU24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUYV16, YUYV16ToYVU24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUYV16::testYUYV16ToYVU24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUYV16, YUYV16ToYVU24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUYV16::testYUYV16ToYVU24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUYV16, YUYV16ToYVU24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUYV16::testYUYV16ToYVU24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterYUYV16::testYUYV16ToBGR24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_LimitedRangeYUV24_To_FullRangeBGR24_BT601();

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_YUYV16, FrameType::FORMAT_BGR24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterYUYV16::convertYUYV16ToBGR24), conversionFlag, pixelFunctionYUYV16ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterYUYV16::testYUYV16ToRGB24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_LimitedRangeYUV24_To_FullRangeRGB24_BT601();

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_YUYV16, FrameType::FORMAT_RGB24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterYUYV16::convertYUYV16ToRGB24), conversionFlag, pixelFunctionYUYV16ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterYUYV16::testYUYV16ToY8(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	//                     | Y |
	// | Y | = | 1 0 0 | * | V |
	//                     | U |

	MatrixD transformationMatrix(1, 3, false);
	transformationMatrix(0, 0) = 1.0;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_YUYV16, FrameType::FORMAT_Y8, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterYUYV16::convertYUYV16ToY8), conversionFlag, pixelFunctionYUYV16ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterYUYV16::testYUYV16ToYUV24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y |   | 1 0 0 |   | Y |
	// | V | = | 0 1 0 | * | V |
	// | U |   | 0 0 1 |   | U |

	const MatrixD transformationMatrix(3, 3, true);

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_YUYV16, FrameType::FORMAT_YUV24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterYUYV16::convertYUYV16ToYUV24), conversionFlag, pixelFunctionYUYV16ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterYUYV16::testYUYV16ToYVU24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
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

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_YUYV16, FrameType::FORMAT_YVU24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterYUYV16::convertYUYV16ToYVU24), conversionFlag, pixelFunctionYUYV16ForYUV24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

MatrixD TestFrameConverterYUYV16::pixelFunctionYUYV16ForYUV24(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
{
	ocean_assert(frame.isValid());
	ocean_assert(x < frame.width() && y < frame.height());
	ocean_assert_and_suppress_unused(conversionFlag == CV::FrameConverter::CONVERT_NORMAL, conversionFlag); // we expect the target frame to have a conversion flag

	ocean_assert(frame.planeChannels(0u) == 2u);

	const unsigned int xUV = x & 0xFFFFFFFEu; // even x values only

	MatrixD colorVector(3, 1);

	// 01 23
	// YU YV
	colorVector(0, 0) = double(frame.constpixel<uint8_t>(x, y)[0]);
	colorVector(1, 0) = double(frame.constpixel<uint8_t>(xUV, y)[1]);
	colorVector(2, 0) = double(frame.constpixel<uint8_t>(xUV + 1u, y)[1]);

	return colorVector;
}

}

}

}
