/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverterRGBA64.h"

#include "ocean/cv/FrameConverterRGBA64.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameConverterRGBA64::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	Log::info() << "---   RGBA64 converter test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	{
		Log::info() << "Testing RGBA64 to RGB24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGBA64ToRGB24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing RGBA64 to RGBA32 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGBA64ToRGBA32(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing RGBA64 to RGBA64 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGBA64ToRGBA64(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "RGBA64 converter tests succeeded.";
	}
	else
	{
		Log::info() << "RGBA64 converter tests FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterRGBA64, RGBA64ToRGB24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA64::testRGBA64ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA64, RGBA64ToRGB24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA64::testRGBA64ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA64, RGBA64ToRGB24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA64::testRGBA64ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA64, RGBA64ToRGB24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA64::testRGBA64ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGBA64, RGBA64ToRGBA32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA64::testRGBA64ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA64, RGBA64ToRGBA32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA64::testRGBA64ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA64, RGBA64ToRGBA32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA64::testRGBA64ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA64, RGBA64ToRGBA32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA64::testRGBA64ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGBA64, RGBA64ToRGBA64Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA64::testRGBA64ToRGBA64(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA64, RGBA64ToRGBA64Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA64::testRGBA64ToRGBA64(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA64, RGBA64ToRGBA64Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA64::testRGBA64ToRGBA64(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA64, RGBA64ToRGBA64FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA64::testRGBA64ToRGBA64(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterRGBA64::testRGBA64ToRGB24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | R8 | = | 1/2^8                      0 |   | R16 |
	// | G8 | = |          1/2^8             0 |   | G16 |
	// | B8 | = |                   1/2^8    0 | * | B16 |
	//                                             | A16 |


	MatrixD transformationMatrix(3, 4, false);
	transformationMatrix(0, 0) = 1.0 / 256.5; // (2^16 - 1) / x < 255.5, x = 256.49706457925636
	transformationMatrix(1, 1) = 1.0 / 256.5;
	transformationMatrix(2, 2) = 1.0 / 256.5;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGBA64, FrameType::FORMAT_RGB24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGBA64::convertRGBA64ToRGB24), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGBA64::testRGBA64ToRGBA32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | R8 | = | 1/2^8                             |   | R16 |
	// | G8 | = |          1/2^8                    |   | G16 |
	// | B8 | = |                   1/2^8           | * | B16 |
	// | A8 | = |                            1/2^8  |   | A16 |


	MatrixD transformationMatrix(4, 4, false);
	transformationMatrix(0, 0) = 1.0 / 256.5; // (2^16 - 1) / x < 255.5, x = 256.49706457925636
	transformationMatrix(1, 1) = 1.0 / 256.5;
	transformationMatrix(2, 2) = 1.0 / 256.5;
	transformationMatrix(3, 3) = 1.0 / 256.5;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGBA64, FrameType::FORMAT_RGBA32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGBA64::convertRGBA64ToRGBA32), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGBA64::testRGBA64ToRGBA64(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | R |   | 1 0 0 0 |   | R |
	// | G | = | 0 1 0 0 | * | G |
	// | B |   | 0 0 1 0 |   | B |
	// | A |   | 0 0 0 1 |   | A |

	const MatrixD transformationMatrix(4, 4, true);

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGBA64, FrameType::FORMAT_RGBA64, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGBA64::convertRGBA64ToRGBA64), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 65535.0, testDuration, worker);
}

}

}

}
