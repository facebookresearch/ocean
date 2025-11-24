/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverterB_G_R24.h"

#include "ocean/cv/FrameConverterB_G_R24.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameConverterB_G_R24::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	TestResult testResult("B_G_R24 converter test");

	Log::info() << " ";

	if (selector.shouldRun("B_G_R24ToBGR24"))
	{
		Log::info() << "Testing B_G_R24 to BGR24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testB_G_R24ToBGR24(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("B_G_R24ToRGB24"))
	{
		Log::info() << "Testing B_G_R24 to RGB24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testB_G_R24ToRGB24(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterB_G_R24, B_G_R24ToBGR24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterB_G_R24::testB_G_R24ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterB_G_R24, B_G_R24ToBGR24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterB_G_R24::testB_G_R24ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterB_G_R24, B_G_R24ToBGR24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterB_G_R24::testB_G_R24ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterB_G_R24, B_G_R24ToBGR24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterB_G_R24::testB_G_R24ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterB_G_R24, B_G_R24ToRGB24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterB_G_R24::testB_G_R24ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterB_G_R24, B_G_R24ToRGB24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterB_G_R24::testB_G_R24ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterB_G_R24, B_G_R24ToRGB24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterB_G_R24::testB_G_R24ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterB_G_R24, B_G_R24ToRGB24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterB_G_R24::testB_G_R24ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterB_G_R24::testB_G_R24ToBGR24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix(3, 3, true);

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_B_G_R24, FrameType::FORMAT_BGR24, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterB_G_R24::convertB_G_R24ToBGR24), conversionFlag, pixelFunctionB_G_R24ForBGR24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterB_G_R24::testB_G_R24ToRGB24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix(3, 3, true);

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_B_G_R24, FrameType::FORMAT_BGR24, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterB_G_R24::convertB_G_R24ToRGB24), conversionFlag, pixelFunctionB_G_R24ForRGB24, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

MatrixD TestFrameConverterB_G_R24::pixelFunctionB_G_R24ForBGR24(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
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

MatrixD TestFrameConverterB_G_R24::pixelFunctionB_G_R24ForRGB24(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
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
