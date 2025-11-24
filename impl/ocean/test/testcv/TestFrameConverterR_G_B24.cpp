/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverterR_G_B24.h"

#include "ocean/cv/FrameConverterR_G_B24.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameConverterR_G_B24::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	TestResult testResult("R_G_B24 converter test");

	Log::info() << " ";

	if (selector.shouldRun("R_G_B24ToRGB24"))
	{
		Log::info() << "Testing R_G_B24 to RGB24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testR_G_B24ToRGB24(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("R_G_B24ToBGR24"))
	{
		Log::info() << "Testing R_G_B24 to BGR24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testR_G_B24ToBGR24(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterR_G_B24, R_G_B24ToRGB24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterR_G_B24::testR_G_B24ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterR_G_B24, R_G_B24ToRGB24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterR_G_B24::testR_G_B24ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterR_G_B24, R_G_B24ToRGB24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterR_G_B24::testR_G_B24ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterR_G_B24, R_G_B24ToRGB24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterR_G_B24::testR_G_B24ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterR_G_B24, R_G_B24ToBGR24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterR_G_B24::testR_G_B24ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterR_G_B24, R_G_B24ToBGR24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterR_G_B24::testR_G_B24ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterR_G_B24, R_G_B24ToBGR24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterR_G_B24::testR_G_B24ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterR_G_B24, R_G_B24ToBGR24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterR_G_B24::testR_G_B24ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterR_G_B24::testR_G_B24ToRGB24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix(3, 3, true);

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_R_G_B24, FrameType::FORMAT_RGB24, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterR_G_B24::convertR_G_B24ToRGB24), conversionFlag, pixelFunctionR_G_B24ForRGB24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterR_G_B24::testR_G_B24ToBGR24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix(3, 3, true);

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_R_G_B24, FrameType::FORMAT_RGB24, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterR_G_B24::convertR_G_B24ToBGR24), conversionFlag, pixelFunctionR_G_B24ForBGR24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

MatrixD TestFrameConverterR_G_B24::pixelFunctionR_G_B24ForRGB24(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
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

MatrixD TestFrameConverterR_G_B24::pixelFunctionR_G_B24ForBGR24(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
{
	ocean_assert(frame.isValid());
	ocean_assert(x < frame.width() && y < frame.height());
	ocean_assert_and_suppress_unused(conversionFlag == CV::FrameConverter::CONVERT_NORMAL, conversionFlag); // we expect the target frame to have a conversion flag

	MatrixD colorVector(3, 1);

	colorVector(0, 0) = double(*frame.constpixel<uint8_t>(x, y, 2u));
	colorVector(1, 0) = double(*frame.constpixel<uint8_t>(x, y, 1u));
	colorVector(2, 0) = double(*frame.constpixel<uint8_t>(x, y, 0u));

	return colorVector;
}

}

}

}
