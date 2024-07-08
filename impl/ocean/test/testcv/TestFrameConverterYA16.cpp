/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverterYA16.h"

#include "ocean/cv/FrameConverterYA16.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameConverterYA16::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	Log::info() << "---   YA16 converter test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	{
		Log::info() << "Testing YA16 to RGBA32 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testYA16ToRGBA32(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing YA16 to BGRA32 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testYA16ToBGRA32(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing YA16 to Y8 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testYA16ToY8(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing YA16 to YA16 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testYA16ToYA16(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "YA16 converter tests succeeded.";
	}
	else
	{
		Log::info() << "YA16 converter tests FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterYA16, YA16ToRGBA32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYA16::testYA16ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYA16, YA16ToRGBA32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYA16::testYA16ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYA16, YA16ToRGBA32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYA16::testYA16ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYA16, YA16ToRGBA32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYA16::testYA16ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterYA16, YA16ToBGRA32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYA16::testYA16ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYA16, YA16ToBGRA32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYA16::testYA16ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYA16, YA16ToBGRA32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYA16::testYA16ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYA16, YA16ToBGRA32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYA16::testYA16ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterYA16, YA16ToY8Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYA16::testYA16ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYA16, YA16ToY8Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYA16::testYA16ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYA16, YA16ToY8Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYA16::testYA16ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYA16, YA16ToY8FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYA16::testYA16ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterYA16, YA16ToYA16Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYA16::testYA16ToYA16(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYA16, YA16ToYA16Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYA16::testYA16ToYA16(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYA16, YA16ToYA16Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYA16::testYA16ToYA16(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYA16, YA16ToYA16FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYA16::testYA16ToYA16(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterYA16::testYA16ToRGBA32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | R |   | 1 0 |
	// | G | = | 1 0 | * | Y |
	// | B |   | 1 0 |   | A |
	// | A |   | 0 1 |

	MatrixD transformationMatrix(4, 2, false);
	transformationMatrix(0, 0) = 1;
	transformationMatrix(1, 0) = 1;
	transformationMatrix(2, 0) = 1;
	transformationMatrix(3, 1) = 1;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_YA16, FrameType::FORMAT_RGBA32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterYA16::convertYA16ToRGBA32), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterYA16::testYA16ToBGRA32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | B |   | 1 0 |
	// | G | = | 1 0 | * | Y |
	// | R |   | 1 0 |   | A |
	// | A |   | 0 1 |

	MatrixD transformationMatrix(4, 2, false);
	transformationMatrix(0, 0) = 1;
	transformationMatrix(1, 0) = 1;
	transformationMatrix(2, 0) = 1;
	transformationMatrix(3, 1) = 1;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_YA16, FrameType::FORMAT_BGRA32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterYA16::convertYA16ToBGRA32), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterYA16::testYA16ToY8(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y | = | 1 0 | * | Y |
	//                   | A |

	MatrixD transformationMatrix(1, 2, false);
	transformationMatrix(0, 0) = 1;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_YA16, FrameType::FORMAT_Y8, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterYA16::convertYA16ToY8), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterYA16::testYA16ToYA16(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y |   | 1 0 | * | Y |
	// | A | = | 0 1 |   | A |

	const MatrixD transformationMatrix(2, 2, true);

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_YA16, FrameType::FORMAT_YA16, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterYA16::convertYA16ToYA16), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

}

}

}
