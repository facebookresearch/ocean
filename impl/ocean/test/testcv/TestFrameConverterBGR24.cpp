/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverterBGR24.h"

#include "ocean/cv/FrameConverterBGR24.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameConverterBGR24::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	Log::info() << "---   BGR24 converter test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	{
		Log::info() << "Testing BGR24 to BGR24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testBGR24ToBGR24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing BGR24 to BGRA32 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testBGR24ToBGRA32(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing BGR24 to RGB24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testBGR24ToRGB24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing BGR24 to RGBA32 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testBGR24ToRGBA32(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing BGR24 to Y8 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testBGR24ToY8(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing BGR24 to YUV24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testBGR24ToYUV24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing full range BGR24 to limited range Y_U_V12 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testBGR24FullRangeToY_U_V12LimitedRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing full range BGR24 to limited range Y_V_U12 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testBGR24FullRangeToY_V_U12LimitedRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing full range BGR24 to full range Y_U_V12 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testBGR24FullRangeToY_U_V12FullRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing full range BGR24 to full range Y_V_U12 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testBGR24FullRangeToY_V_U12FullRange(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "BGR24 converter test succeeded.";
	}
	else
	{
		Log::info() << "BGR24 converter test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterBGR24, BGR24ToBGR24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24ToBGR24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24ToBGR24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24ToBGR24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterBGR24, BGR24ToBGRA32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24ToBGRA32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24ToBGRA32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24ToBGRA32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterBGR24, BGR24ToRGB24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24ToRGB24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24ToRGB24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24ToRGB24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterBGR24, BGR24ToRGBA32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24ToRGBA32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24ToRGBA32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24ToRGBA32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterBGR24, BGR24ToY8Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24ToY8Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24ToY8Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24ToY8FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterBGR24, BGR24ToYUV24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24ToYUV24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24ToYUV24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24ToYUV4FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterBGR24, BGR24FullRangeToY_U_V12LimitedRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24FullRangeToY_U_V12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24FullRangeToY_U_V12LimitedRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24FullRangeToY_U_V12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24FullRangeToY_U_V12LimitedRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24FullRangeToY_U_V12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24FullRangeToY_U_V12LimitedRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24FullRangeToY_U_V12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterBGR24, BGR24FullRangeToY_V_U12LimitedRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24FullRangeToY_V_U12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24FullRangeToY_V_U12LimitedRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24FullRangeToY_V_U12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24FullRangeToY_V_U12LimitedRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24FullRangeToY_V_U12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24FullRangeToY_V_U12LimitedRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24FullRangeToY_V_U12LimitedRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterBGR24, BGR24FullRangeToY_U_V12FullRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24FullRangeToY_U_V12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24FullRangeToY_U_V12FullRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24FullRangeToY_U_V12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24FullRangeToY_U_V12FullRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24FullRangeToY_U_V12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24FullRangeToY_U_V12FullRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24FullRangeToY_U_V12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterBGR24, BGR24FullRangeToY_V_U12FullRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24FullRangeToY_V_U12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24FullRangeToY_V_U12FullRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24FullRangeToY_V_U12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24FullRangeToY_V_U12FullRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24FullRangeToY_V_U12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGR24, BGR24FullRangeToY_V_U12FullRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGR24::testBGR24FullRangeToY_V_U12FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterBGR24::testBGR24ToBGR24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | B |   | 1 0 0 |   | B |
	// | G | = | 0 1 0 | * | G |
	// | R |   | 0 0 1 |   | R |

	MatrixD transformationMatrix(3, 3, true);

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_BGR24, FrameType::FORMAT_BGR24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterBGR24::convertBGR24ToBGR24), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterBGR24::testBGR24ToBGRA32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | G |   | 1 0 0 0 |   | B |
	// | G |   | 0 1 0 0 | * | G |
	// | R | = | 0 0 1 0 |   | R |
	// | A |   | 0 0 0 A |   | 1 |

	MatrixD transformationMatrix(4, 4, true);
	transformationMatrix(3, 3) = double(TestFrameConverter::ValueProvider::get().alphaValue());

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_BGR24, FrameType::FORMAT_BGRA32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterBGR24::convertBGR24ToBGRA32), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterBGR24::testBGR24ToRGB24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | R |   | 0 0 1 |   | B |
	// | G | = | 0 1 0 | * | G |
	// | B |   | 1 0 0 |   | R |

	MatrixD transformationMatrix(3, 3, false);
	transformationMatrix(0, 2) = 1;
	transformationMatrix(1, 1) = 1;
	transformationMatrix(2, 0) = 1;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_BGR24, FrameType::FORMAT_RGB24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterBGR24::convertBGR24ToRGB24), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterBGR24::testBGR24ToRGBA32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | R |   | 0 0 1 0 |   | B |
	// | G | = | 0 1 0 0 | * | G |
	// | B |   | 1 0 0 0 |   | R |
	// | A |   | 0 0 0 A |   | 1 |

	MatrixD transformationMatrix(4, 4, false);
	transformationMatrix(0, 2) = 1;
	transformationMatrix(1, 1) = 1;
	transformationMatrix(2, 0) = 1;
	transformationMatrix(3, 3) = double(TestFrameConverter::ValueProvider::get().alphaValue());

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_BGR24, FrameType::FORMAT_RGBA32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterBGR24::convertBGR24ToRGBA32), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterBGR24::testBGR24ToY8(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	//                                 | B |
	// | Y | = | 0.114 0.587 0.299 | * | G |
	//                                 | R |

	MatrixD transformationMatrix(1, 3, false);
	transformationMatrix(0, 0) = 0.114;
	transformationMatrix(0, 1) = 0.587;
	transformationMatrix(0, 2) = 0.299;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_BGR24, FrameType::FORMAT_Y8, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterBGR24::convertBGR24ToY8), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterBGR24::testBGR24ToYUV24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_FullRangeBGR24_To_LimitedRangeYUV24_BT601();

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_BGR24, FrameType::FORMAT_YUV24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterBGR24::convertBGR24ToYUV24), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterBGR24::testBGR24FullRangeToY_U_V12LimitedRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_FullRangeBGR24_To_LimitedRangeYUV24_BT601();

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_BGR24, FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterBGR24::convertBGR24FullRangeToY_U_V12LimitedRange), flag, TestFrameConverterBGR24::pixelFunctionBGRForY_UV12, TestFrameConverterBGR24::pixelFunctionY_U_V12ForYUV24, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterBGR24::testBGR24FullRangeToY_V_U12LimitedRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y |   | 1       |   | Y |
	// | V | = |       1 | * | U |
	// | U |   |   1     |   | V |

	MatrixD yvu_T_yuv(3, 3, false);

	yvu_T_yuv(0, 0) = 1.0;
	yvu_T_yuv(1, 2) = 1.0;
	yvu_T_yuv(2, 1) = 1.0;
	const MatrixD transformationMatrix = yvu_T_yuv * CV::FrameConverter::transformationMatrix_FullRangeBGR24_To_LimitedRangeYUV24_BT601();

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_BGR24, FrameType::FORMAT_Y_V_U12_LIMITED_RANGE, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterBGR24::convertBGR24FullRangeToY_V_U12LimitedRange), flag, TestFrameConverterBGR24::pixelFunctionBGRForY_UV12, TestFrameConverterBGR24::pixelFunctionY_U_V12ForYUV24, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterBGR24::testBGR24FullRangeToY_U_V12FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_FullRangeBGR24_To_FullRangeYUV24_BT601();

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_BGR24, FrameType::FORMAT_Y_U_V12_FULL_RANGE, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterBGR24::convertBGR24FullRangeToY_U_V12FullRange), flag, TestFrameConverterBGR24::pixelFunctionBGRForY_UV12, TestFrameConverterBGR24::pixelFunctionY_U_V12ForYUV24, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterBGR24::testBGR24FullRangeToY_V_U12FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_FullRangeBGR24_To_FullRangeYVU24_BT601();

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_BGR24, FrameType::FORMAT_Y_V_U12_FULL_RANGE, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterBGR24::convertBGR24FullRangeToY_V_U12FullRange), flag, TestFrameConverterBGR24::pixelFunctionBGRForY_UV12, TestFrameConverterBGR24::pixelFunctionY_U_V12ForYUV24, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

MatrixD TestFrameConverterBGR24::pixelFunctionBGRForY_UV12(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
{
	ocean_assert(frame.isValid());
	ocean_assert(x < frame.width() && y < frame.height());
	ocean_assert_and_suppress_unused(conversionFlag == CV::FrameConverter::CONVERT_NORMAL, conversionFlag);

	const unsigned int xTopLeft = (x / 2u) * 2u;
	const unsigned int yTopLeft = (y / 2u) * 2u;

	// first column is for the Y-channel, second and third column is for U and V

	MatrixD colorMatrix(3, 3);

	colorMatrix(0, 0) = double(frame.constpixel<uint8_t>(x, y)[0]);
	colorMatrix(1, 0) = double(frame.constpixel<uint8_t>(x, y)[1]);
	colorMatrix(2, 0) = double(frame.constpixel<uint8_t>(x, y)[2]);

	for (unsigned int nChannel = 0u; nChannel < 3u; ++nChannel)
	{
		// averaging every 2x2 block

		const double averageColorValue = double(frame.constpixel<uint8_t>(xTopLeft + 0u, yTopLeft + 0u)[nChannel]
												+ frame.constpixel<uint8_t>(xTopLeft + 1u, yTopLeft + 0u)[nChannel]
												+ frame.constpixel<uint8_t>(xTopLeft + 0u, yTopLeft + 1u)[nChannel]
												+ frame.constpixel<uint8_t>(xTopLeft + 1u, yTopLeft + 1u)[nChannel]) * 0.25;

		colorMatrix(nChannel, 1) = averageColorValue;
		colorMatrix(nChannel, 2) = averageColorValue;
	}

	return colorMatrix;
}

MatrixD TestFrameConverterBGR24::pixelFunctionY_U_V12ForYUV24(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
{
	ocean_assert(frame.isValid());
	ocean_assert(x < frame.width() && y < frame.height());

	unsigned int xAdjusted = x;
	unsigned int yAdjusted = y;

	switch (conversionFlag)
	{
		case CV::FrameConverter::CONVERT_NORMAL:
			break;

		case CV::FrameConverter::CONVERT_FLIPPED:
			yAdjusted = frame.height() - y - 1u;
			break;

		case CV::FrameConverter::CONVERT_MIRRORED:
			xAdjusted = frame.width() - x - 1u;
			break;

		case CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED:
			xAdjusted = frame.width() - x - 1u;
			yAdjusted = frame.height() - y - 1u;
			break;

		default:
			ocean_assert(false && "Not supported conversion flag.");
	}

	const unsigned int xAdjusted_2 = xAdjusted / 2u;
	const unsigned int yAdjusted_2 = yAdjusted / 2u;

	MatrixD colorVector(3, 1);

	colorVector(0, 0) = double(frame.constpixel<uint8_t>(xAdjusted, yAdjusted, 0u)[0]);
	colorVector(1, 0) = double(frame.constpixel<uint8_t>(xAdjusted_2, yAdjusted_2, 1u)[0]);
	colorVector(2, 0) = double(frame.constpixel<uint8_t>(xAdjusted_2, yAdjusted_2, 2u)[0]);

	return colorVector;
}

}

}

}
