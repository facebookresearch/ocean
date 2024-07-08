/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverterRGB565.h"

#include "ocean/cv/FrameConverterRGB565.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameConverterRGB565::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	Log::info() << "---   RGB565 converter test:   ---";
	Log::info() << " ";

	const auto flags = CV::FrameConverter::conversionFlags();

	bool allSucceeded = true;

	{
		Log::info() << "Testing RGB565 to RGB24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGB565ToRGB24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing RGB565 to Y8 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGB565ToY8(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "RGB565 converter test succeeded.";
	}
	else
	{
		Log::info() << "RGB565 converter test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterRGB565, RGB565ToRGB24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB565::testRGB565ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB565, RGB565ToRGB24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB565::testRGB565ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB565, RGB565ToRGB24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB565::testRGB565ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB565, RGB565ToRGB24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB565::testRGB565ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGB565, RGB565ToY8Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB565::testRGB565ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB565, RGB565ToY8Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB565::testRGB565ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB565, RGB565ToY8Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB565::testRGB565ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB565, RGB565ToY8FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB565::testRGB565ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterRGB565::testRGB565ToRGB24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | R |   | 1 0 0 |   | R |
	// | G | = | 0 1 0 | * | G |
	// | B |   | 0 0 1 |   | B |

	MatrixD transformationMatrix(3, 3, true);

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGB565, FrameType::FORMAT_RGB24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGB565::convertRGB565ToRGB24), conversionFlag, pixelFunctionRGB565ForRGB24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGB565::testRGB565ToY8(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
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

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGB565, FrameType::FORMAT_Y8, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGB565::convertRGB565ToY8), conversionFlag, pixelFunctionRGB565ForRGB24, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

MatrixD TestFrameConverterRGB565::pixelFunctionRGB565ForRGB24(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
{
	ocean_assert(frame.isValid());
	ocean_assert(x < frame.width() && y < frame.height());
	ocean_assert_and_suppress_unused(conversionFlag == CV::FrameConverter::CONVERT_NORMAL, conversionFlag); // we expect the target frame to have a conversion flag

	const uint16_t rgb565 = frame.constpixel<uint16_t>(x, y)[0];

	const unsigned int red5 = rgb565 & 0x1fu;
	const unsigned int green6 = (rgb565 >> 5u) & 0x3fu;
	const unsigned int blue5 = rgb565 >> 11u;

	// shift ...
	unsigned int red8 = red5 << 3u;
	unsigned int green8 = green6 << 2u;
	unsigned int blue8 = blue5 << 3u;

	// ... with insert
	red8 = red8 | (red5 & 0x07u);
	green8 = green8 | (green6 & 0x03u);
	blue8 = blue8 | (blue5 & 0x07u);

	ocean_assert(red8 < 256u && green8 < 256u && blue8 < 256u);

	MatrixD colorVector(3, 1);

	colorVector(0, 0) = double(red8);
	colorVector(1, 0) = double(green8);
	colorVector(2, 0) = double(blue8);

	return colorVector;
}

}

}

}
