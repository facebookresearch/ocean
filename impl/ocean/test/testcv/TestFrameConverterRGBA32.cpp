/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverterRGBA32.h"

#include "ocean/cv/FrameConverterRGBA32.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameConverterRGBA32::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	Log::info() << "---   RGBA32 converter test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	{
		Log::info() << "Testing RGBA32 to BGR24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGBA32ToBGR24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing RGBA32 to BGR32 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGBA32ToBGR32(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing RGBA32 to RGB24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGBA32ToRGB24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing RGBA32 to RGB32 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGBA32ToRGB32(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing RGBA32 to RGBA32 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGBA32ToRGBA32(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing RGBA32 to ABGR32 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGBA32ToABGR32(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing RGBA32 to ARGB32 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGBA32ToARGB32(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing RGBA32 to BGRA32 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGBA32ToBGRA32(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing RGBA32 to Y8 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGBA32ToY8(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing RGBA32 to YA16 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGBA32ToYA16(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing RGBA32 to YUV24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGBA32ToYUV24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "RGBA32 converter tests succeeded.";
	}
	else
	{
		Log::info() << "RGBA32 converter tests FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterRGBA32, RGBA32ToBGR24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToBGR24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToBGR24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToBGR24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGBA32, RGBA32ToBGR32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToBGR32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToBGR32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToBGR32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToBGR32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToBGR32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToBGR32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToBGR32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGBA32, RGBA32ToRGB24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToRGB24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToRGB24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToRGB24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGBA32, RGBA32ToRGB32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToRGB32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToRGB32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToRGB32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToRGB32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToRGB32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToRGB32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToRGB32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGBA32, RGBA32ToRGBA32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToRGBA32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToRGBA32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToRGBA32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGBA32, RGBA32ToABGR32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToABGR32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToABGR32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToABGR32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToABGR32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToABGR32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToABGR32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToABGR32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGBA32, RGBA32ToARGB32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToARGB32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToARGB32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToARGB32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToARGB32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToARGB32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToARGB32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToARGB32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGBA32, RGBA32ToBGRA32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToBGRA32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToBGRA32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToBGRA32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGBA32, RGBA32ToY8Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToY8Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToY8Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToY8FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGBA32, RGBA32ToYA16Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToYA16(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToYA16Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToYA16(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToYA16Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToYA16(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToYA16FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToYA16(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGBA32, RGBA32ToYUV24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToYUV24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToYUV24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGBA32, RGBA32ToYUV24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGBA32::testRGBA32ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterRGBA32::testRGBA32ToBGR24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | B |   | 0 0 1 0 |   | R |
	// | G | = | 0 1 0 0 | * | G |
	// | R |   | 1 0 0 0 |   | B |
	//                       | A |

	MatrixD transformationMatrix(3, 4, false);
	transformationMatrix(0, 2) = 1.0;
	transformationMatrix(1, 1) = 1.0;
	transformationMatrix(2, 0) = 1.0;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGBA32, FrameType::FORMAT_BGR24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGBA32::convertRGBA32ToBGR24), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGBA32::testRGBA32ToBGR32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | B |   | 0 0 1 0 |   | R |
	// | G | = | 0 1 0 0 | * | G |
	// | R |   | 1 0 0 0 |   | B |
	// | 0 |   | 0 0 0 0 |   | A |

	MatrixD transformationMatrix(4, 4, false);
	transformationMatrix(0, 2) = 1.0;
	transformationMatrix(1, 1) = 1.0;
	transformationMatrix(2, 0) = 1.0;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGBA32, FrameType::FORMAT_BGR32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGBA32::convertRGBA32ToBGR32), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGBA32::testRGBA32ToRGB24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | R |   | 1 0 0 0 |   | R |
	// | G | = | 0 1 0 0 | * | G |
	// | B |   | 0 0 1 0 |   | B |
	//                       | A |

	MatrixD transformationMatrix(3, 4, false);
	transformationMatrix(0, 0) = 1.0;
	transformationMatrix(1, 1) = 1.0;
	transformationMatrix(2, 2) = 1.0;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGBA32, FrameType::FORMAT_RGB24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGBA32::convertRGBA32ToRGB24), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGBA32::testRGBA32ToRGB32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | B |   | 1 0 0 0 |   | R |
	// | G | = | 0 1 0 0 | * | G |
	// | R |   | 0 0 1 0 |   | B |
	// | 0 |   | 0 0 0 0 |   | A |

	MatrixD transformationMatrix(4, 4, false);
	transformationMatrix(0, 0) = 1.0;
	transformationMatrix(1, 1) = 1.0;
	transformationMatrix(2, 2) = 1.0;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGBA32, FrameType::FORMAT_RGB32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGBA32::convertRGBA32ToRGB32), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGBA32::testRGBA32ToRGBA32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | R |   | 1 0 0 0 |   | R |
	// | G | = | 0 1 0 0 | * | G |
	// | B |   | 0 0 1 0 |   | B |
	// | A |   | 0 0 0 1 |   | A |

	const MatrixD transformationMatrix(4, 4, true);

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGBA32, FrameType::FORMAT_RGBA32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGBA32::convertRGBA32ToRGBA32), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGBA32::testRGBA32ToABGR32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | A |   | 0 0 0 1 |   | R |
	// | B | = | 0 0 1 0 | * | G |
	// | G |   | 0 1 0 0 |   | B |
	// | R |   | 1 0 0 0 |   | A |

	MatrixD transformationMatrix(4, 4, false);
	transformationMatrix(0, 3) = 1.0;
	transformationMatrix(1, 2) = 1.0;
	transformationMatrix(2, 1) = 1.0;
	transformationMatrix(3, 0) = 1.0;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGBA32, FrameType::FORMAT_ABGR32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGBA32::convertRGBA32ToABGR32), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGBA32::testRGBA32ToARGB32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | A |   | 0 0 0 1 |   | R |
	// | R | = | 1 0 0 0 | * | G |
	// | G |   | 0 1 0 0 |   | B |
	// | B |   | 0 0 1 0 |   | A |

	MatrixD transformationMatrix(4, 4, false);
	transformationMatrix(0, 3) = 1.0;
	transformationMatrix(1, 0) = 1.0;
	transformationMatrix(2, 1) = 1.0;
	transformationMatrix(3, 2) = 1.0;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGBA32, FrameType::FORMAT_ARGB32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGBA32::convertRGBA32ToARGB32), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGBA32::testRGBA32ToBGRA32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | B |   | 0 0 1 0 |   | R |
	// | G | = | 0 1 0 0 | * | G |
	// | R |   | 1 0 0 0 |   | B |
	// | A |   | 0 0 0 1 |   | A |

	MatrixD transformationMatrix(4, 4, false);
	transformationMatrix(0, 2) = 1.0;
	transformationMatrix(1, 1) = 1.0;
	transformationMatrix(2, 0) = 1.0;
	transformationMatrix(3, 3) = 1.0;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGBA32, FrameType::FORMAT_BGRA32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGBA32::convertRGBA32ToBGRA32), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGBA32::testRGBA32ToY8(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	//                                     | R |
	// | Y | = | 0.299 0.587 0.114 0.0 | * | G |
	//                                     | B |
	//                                     | A |

	MatrixD transformationMatrix(1, 4, false);
	transformationMatrix(0, 0) = 0.299;
	transformationMatrix(0, 1) = 0.587;
	transformationMatrix(0, 2) = 0.114;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGBA32, FrameType::FORMAT_Y8, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGBA32::convertRGBA32ToY8), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGBA32::testRGBA32ToYA16(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	//                                     | R |
	// | Y | = | 0.299 0.587 0.114 0.0 | * | G |
	// | A |   | 0.0   0.0   0.0   1.0 |   | B |
	//                                     | A |

	MatrixD transformationMatrix(2, 4, false);

	transformationMatrix(0, 0) = 0.299;
	transformationMatrix(0, 1) = 0.587;
	transformationMatrix(0, 2) = 0.114;
	transformationMatrix(1, 3) = 1.0;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGBA32, FrameType::FORMAT_YA16, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGBA32::convertRGBA32ToYA16), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGBA32::testRGBA32ToYUV24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// BT.601

	// | Y |   |  0.2578125   0.5039063   0.09765625  0.0    16.0 |   | R |
	// | U | = | -0.1484375  -0.2890625   0.4375      0.0   128.0 | * | G |
	// | V |   |  0.4375     -0.3671875  -0.0703125   0.0   128.0 |   | B |
	//                                                                | A |
	//                                                                | 1 |

	MatrixD transformationMatrix(3, 5, false);

	transformationMatrix(0, 0) = 0.2578125;
	transformationMatrix(1, 0) = -0.1484375;
	transformationMatrix(2, 0) = 0.4375;

	transformationMatrix(0, 1) = 0.5039063;
	transformationMatrix(1, 1) = -0.2890625;
	transformationMatrix(2, 1) = -0.3671875;

	transformationMatrix(0, 2) = 0.09765625;
	transformationMatrix(1, 2) = 0.4375;
	transformationMatrix(2, 2) = -0.0703125;

	transformationMatrix(0, 4) = 16.0;
	transformationMatrix(1, 4) = 128.0;
	transformationMatrix(2, 4) = 128.0;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_BGRA32, FrameType::FORMAT_YUV24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGBA32::convertRGBA32ToYUV24), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

}

}

}
