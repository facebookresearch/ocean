/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverterY32.h"

#include "ocean/cv/FrameConverterY32.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameConverterY32::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	TestResult testResult("Y32 converter test");
	Log::info() << " ";

	if (selector.shouldRun("Y32ToY8"))
	{
		Log::info() << "Testing Y32 to Y8 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testY32ToY8(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("Y32ToY16"))
	{
		Log::info() << "Testing Y32 to Y16 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testY32ToY16(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterY32, Y32ToY8Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY32::testY32ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY32, Y32ToY8Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY32::testY32ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY32, Y32ToY8Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY32::testY32ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY32, Y32ToY8FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY32::testY32ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY32, Y32ToY16Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY32::testY32ToY16(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY32, Y32ToY16Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY32::testY32ToY16(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY32, Y32ToY16Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY32::testY32ToY16(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY32, Y32ToY16FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY32::testY32ToY16(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterY32::testY32ToY8(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y8 | = | 1 / 2^24 | * | Y32 |

	MatrixD transformationMatrix(1, 1, false);
	transformationMatrix(0, 0) = 1.0 / 16810048.1213308; // (2^32 - 1) / x < 255.5, x = 16,810,048.12133072407045

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y32, FrameType::FORMAT_Y8, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY32::convertY32ToY8), flag, FrameConverterTestUtilities::functionGenericPixel, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY32::testY32ToY16(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y16 | = | 1 / 2^16 | * | Y32 |

	MatrixD transformationMatrix(1, 1, false);
	transformationMatrix(0, 0) = 1.0 / 65536.5; // (2^32 - 1) / x < 65,535.5, x = 65,536.499988555820891

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_Y32, FrameType::FORMAT_Y16, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterY32::convertY32ToY16), flag, FrameConverterTestUtilities::functionGenericPixel, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 65535.0, testDuration, worker);
}

}

}

}
