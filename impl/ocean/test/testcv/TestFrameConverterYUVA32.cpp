/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverterYUVA32.h"

#include "ocean/cv/FrameConverterYUVA32.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameConverterYUVA32::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	Log::info() << "---   YUVA32 converter test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	{
		Log::info() << "Testing YUVA32 to Y8 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testYUVA32ToY8(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "YUVA32 converter tests succeeded.";
	}
	else
	{
		Log::info() << "YUVA32 converter tests FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterYUVA32, YUVA32ToY8Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUVA32::testYUVA32ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUVA32, YUVA32ToY8Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUVA32::testYUVA32ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUVA32, YUVA32ToY8Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUVA32::testYUVA32ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYUVA32, YUVA32ToY8FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYUVA32::testYUVA32ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterYUVA32::testYUVA32ToY8(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	//                       | Y |
	// | Y | = | 1 0 0 0 | * | U |
	//                       | V |
	//                       | A |

	MatrixD transformationMatrix(1, 4, false);
	transformationMatrix(0, 0) = 1.0;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_YUVA32, FrameType::FORMAT_Y8, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterYUVA32::convertYUVA32ToY8), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

}

}

}
