/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverterARGB32.h"

#include "ocean/cv/FrameConverterARGB32.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameConverterARGB32::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	Log::info() << "---   RGBA32 converter test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	{
		Log::info() << "Testing ARGB32 to ARGB32 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testARGB32ToARGB32( width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing ARGB32 to BGRA32 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testARGB32ToBGRA32( width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing ARGB32 to RGB24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testARGB32ToRGB24( width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing ARGB32 to RGBA32 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testARGB32ToRGBA32( width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "ARGB32 converter tests succeeded.";
	}
	else
	{
		Log::info() << "ARGB32 converter tests FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterARGB32, ARGB32ToARGB32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterARGB32::testARGB32ToARGB32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterARGB32, ARGB32ToARGB32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterARGB32::testARGB32ToARGB32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterARGB32, ARGB32ToARGB32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterARGB32::testARGB32ToARGB32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterARGB32, ARGB32ToARGB32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterARGB32::testARGB32ToARGB32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterARGB32, ARGB32ToBGRA32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterARGB32::testARGB32ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterARGB32, ARGB32ToBGRA32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterARGB32::testARGB32ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterARGB32, ARGB32ToBGRA32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterARGB32::testARGB32ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterARGB32, ARGB32ToBGRA3232FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterARGB32::testARGB32ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterARGB32, ARGB32ToRGB24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterARGB32::testARGB32ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterARGB32, ARGB32ToRGB24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterARGB32::testARGB32ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterARGB32, ARGB32ToRGB24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterARGB32::testARGB32ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterARGB32, ARGB32ToRGB24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterARGB32::testARGB32ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterARGB32, ARGB32ToRGBA32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterARGB32::testARGB32ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterARGB32, ARGB32ToRGBA32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterARGB32::testARGB32ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterARGB32, ARGB32ToRGBA32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterARGB32::testARGB32ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterARGB32, ARGB32ToRGBA32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterARGB32::testARGB32ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterARGB32::testARGB32ToARGB32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | A |   | 1 0 0 0 |   | A |
	// | R | = | 0 1 0 0 | * | R |
	// | G |   | 0 0 1 0 |   | G |
	// | B |   | 0 0 0 1 |   | B |

	const MatrixD transformationMatrix(4, 4, true);

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_ARGB32, FrameType::FORMAT_ARGB32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterARGB32::convertARGB32ToARGB32), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterARGB32::testARGB32ToBGRA32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | B |   | 0 0 0 1 |   | A |
	// | G | = | 0 0 1 0 | * | R |
	// | R |   | 0 1 0 0 |   | G |
	// | A |   | 1 0 0 0 |   | B |

	MatrixD transformationMatrix(4, 4, false);
	transformationMatrix(0, 3) = 1.0;
	transformationMatrix(1, 2) = 1.0;
	transformationMatrix(2, 1) = 1.0;
	transformationMatrix(3, 0) = 1.0;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_ARGB32, FrameType::FORMAT_BGRA32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterARGB32::convertARGB32ToBGRA32), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterARGB32::testARGB32ToRGB24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | R |   | 0 1 0 0 |   | A |
	// | G | = | 0 0 1 0 | * | R |
	// | B |   | 0 0 0 1 |   | G |
	//                       | B |

	MatrixD transformationMatrix(3, 4, false);
	transformationMatrix(0, 1) = 1.0;
	transformationMatrix(1, 2) = 1.0;
	transformationMatrix(2, 3) = 1.0;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_ARGB32, FrameType::FORMAT_RGB24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterARGB32::convertARGB32ToRGB24), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterARGB32::testARGB32ToRGBA32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | R |   | 0 1 0 0 |   | A |
	// | G | = | 0 0 1 0 | * | R |
	// | B |   | 0 0 0 1 |   | G |
	// | A |   | 1 0 0 0 |   | B |

	MatrixD transformationMatrix(4, 4, false);
	transformationMatrix(0, 1) = 1.0;
	transformationMatrix(1, 2) = 1.0;
	transformationMatrix(2, 3) = 1.0;
	transformationMatrix(3, 0) = 1.0;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_ARGB32, FrameType::FORMAT_RGBA32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterARGB32::convertARGB32ToRGBA32), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

}

}

}
