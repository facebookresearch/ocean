/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverterBGR32.h"

#include "ocean/cv/FrameConverterBGR32.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameConverterBGR32::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	Log::info() << "---   BGR32 converter test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	{
		Log::info() << "Testing BGR32 to RGB24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testBGR32ToRGB24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing BGR32 to RGBA32 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testBGR32ToRGBA32(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "BGR32 converter test succeeded.";
	}
	else
	{
		Log::info() << "BGR32 converter test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterBGR32, BGR32ToRGB24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR32::testBGR32ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR32, BGR32ToRGB24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR32::testBGR32ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR32, BGR32ToRGB24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR32::testBGR32ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR32, BGR32ToRGB24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR32::testBGR32ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterBGR32, BGR32ToRGBA32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR32::testBGR32ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR32, BGR32ToRGBA32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR32::testBGR32ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR32, BGR32ToRGBA32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR32::testBGR32ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR32, BGR32ToRGBA32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR32::testBGR32ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterBGR32::testBGR32ToRGB24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | R |   | 0 0 1 0|   | B |
	// | G | = | 0 1 0 0| * | G |
	// | B |   | 1 0 0 0|   | R |
	//                      | ? |

	MatrixD transformationMatrix(3, 4, false);
	transformationMatrix(0, 2) = 1;
	transformationMatrix(1, 1) = 1;
	transformationMatrix(2, 0) = 1;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_BGR32, FrameType::FORMAT_RGB24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterBGR32::convertBGR32ToRGB24), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}


bool TestFrameConverterBGR32::testBGR32ToRGBA32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | R |   | 0 0 1 0 0 |   | B |
	// | G | = | 0 1 0 0 0 | * | G |
	// | B |   | 1 0 0 0 0 |   | R |
	// | A |   | 0 0 0 0 A |   | ? |
	//                         | 1 |

	MatrixD transformationMatrix(4, 5, false);
	transformationMatrix(0, 2) = 1;
	transformationMatrix(1, 1) = 1;
	transformationMatrix(2, 0) = 1;
	transformationMatrix(3, 4) = double(TestFrameConverter::ValueProvider::get().alphaValue());

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_BGR32, FrameType::FORMAT_RGBA32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterBGR32::convertBGR32ToRGBA32), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

}

}

}
