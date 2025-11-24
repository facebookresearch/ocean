/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverterBGRA32.h"

#include "ocean/cv/FrameConverterBGRA32.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameConverterBGRA32::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	TestResult testResult("BGRA32 converter test");

	Log::info() << " ";

	if (selector.shouldRun("BGRA32ToARGB32"))
	{
		Log::info() << "Testing BGRA32 to ARGB32 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testBGRA32ToARGB32( width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("BGRA32ToBGR24"))
	{
		Log::info() << "Testing BGRA32 to BGR24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testBGRA32ToBGR24( width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("BGRA32ToBGRA32"))
	{
		Log::info() << "Testing BGRA32 to BGRA32 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testBGRA32ToBGRA32( width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("BGRA32ToRGB24"))
	{
		Log::info() << "Testing BGRA32 to RGB24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testBGRA32ToRGB24(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("BGRA32ToRGBA32"))
	{
		Log::info() << "Testing BGRA32 to RGBA32 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testBGRA32ToRGBA32(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("BGRA32ToY8FullRange"))
	{
		Log::info() << "Testing BGRA32 (full range) to Y8 full range conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testBGRA32ToY8FullRange(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("BGRA32ToYA16"))
	{
		Log::info() << "Testing BGRA32 to YA16 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testBGRA32ToYA16(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("BGRA32ToYUV24"))
	{
		Log::info() << "Testing BGRA32 to YUV24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testBGRA32ToYUV24(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterBGRA32, BGRA32ToARGB32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToARGB32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGRA32, BGRA32ToARGB32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToARGB32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGRA32, BGRA32ToARGB32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToARGB32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGRA32, BGRA32ToARGB32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToARGB32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterBGRA32, BGRA32ToBGR24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGRA32, BGRA32ToBGR24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGRA32, BGRA32ToBGR24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGRA32, BGRA32ToBGR24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterBGRA32, BGRA32ToBGRA32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGRA32, BGRA32ToBGRA32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGRA32, BGRA32ToBGRA32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGRA32, BGRA32ToBGRA32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterBGRA32, BGRA32ToRGB24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGRA32, BGRA32ToRGB24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGRA32, BGRA32ToRGB24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGRA32, BGRA32ToRGB24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterBGRA32, BGRA32ToRGBA32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGRA32, BGRA32ToRGBA32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGRA32, BGRA32ToRGBA32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGRA32, BGRA32ToRGBA32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterBGRA32, BGRA32ToY8FullRangeNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToY8FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGRA32, BGRA32ToY8FullRangeFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToY8FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGRA32, BGRA32ToY8FullRangeMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToY8FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGRA32, BGRA32ToY8FullRangeFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToY8FullRange(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterBGRA32, BGRA32ToYA16Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToYA16(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGRA32, BGRA32ToYA16Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToYA16(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGRA32, BGRA32ToYA16Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToYA16(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGRA32, BGRA32ToYA16FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToYA16(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterBGRA32, BGRA32ToYUV24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGRA32, BGRA32ToYUV24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGRA32, BGRA32ToYUV24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterBGRA32, BGRA32ToYUV24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterBGRA32::testBGRA32ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterBGRA32::testBGRA32ToARGB32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | A |   | 0 0 0 1 |   | B |
	// | R | = | 0 0 1 0 | * | G |
	// | G |   | 0 1 0 0 |   | R |
	// | B |   | 1 0 0 0 |   | A |

	MatrixD transformationMatrix(4, 4, false);
	transformationMatrix(0, 3) = 1.0;
	transformationMatrix(1, 2) = 1.0;
	transformationMatrix(2, 1) = 1.0;
	transformationMatrix(3, 0) = 1.0;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_BGRA32, FrameType::FORMAT_ARGB32, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterBGRA32::convertBGRA32ToARGB32), flag, FrameConverterTestUtilities::functionGenericPixel, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterBGRA32::testBGRA32ToBGR24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | B |   | 1 0 0 0 |   | B |
	// | G | = | 0 1 0 0 | * | G |
	// | R |   | 0 0 1 0 |   | R |
	//                       | A |

	MatrixD transformationMatrix(3, 4, false);
	transformationMatrix(0, 0) = 1.0;
	transformationMatrix(1, 1) = 1.0;
	transformationMatrix(2, 2) = 1.0;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_BGRA32, FrameType::FORMAT_BGR24, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterBGRA32::convertBGRA32ToBGR24), flag, FrameConverterTestUtilities::functionGenericPixel, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterBGRA32::testBGRA32ToBGRA32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | B |   | 1 0 0 0 |   | B |
	// | G | = | 0 1 0 0 | * | G |
	// | R |   | 0 0 1 0 |   | R |
	// | A |   | 0 0 0 1 |   | A |

	MatrixD transformationMatrix(4, 4, false);
	transformationMatrix(0, 0) = 1.0;
	transformationMatrix(1, 1) = 1.0;
	transformationMatrix(2, 2) = 1.0;
	transformationMatrix(3, 3) = 1.0;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_BGRA32, FrameType::FORMAT_BGRA32, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterBGRA32::convertBGRA32ToBGRA32), flag, FrameConverterTestUtilities::functionGenericPixel, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterBGRA32::testBGRA32ToRGB24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | R |   | 0 0 1 0 |   | B |
	// | G | = | 0 1 0 0 | * | G |
	// | B |   | 1 0 0 0 |   | R |
	//                       | A |

	MatrixD transformationMatrix(3, 4, false);
	transformationMatrix(0, 2) = 1.0;
	transformationMatrix(1, 1) = 1.0;
	transformationMatrix(2, 0) = 1.0;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_BGRA32, FrameType::FORMAT_RGB24, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterBGRA32::convertBGRA32ToRGB24), flag, FrameConverterTestUtilities::functionGenericPixel, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterBGRA32::testBGRA32ToRGBA32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | R |   | 0 0 1 0 |   | B |
	// | G | = | 0 1 0 0 | * | G |
	// | B |   | 1 0 0 0 |   | R |
	// | A |   | 0 0 0 1 |   | A |

	MatrixD transformationMatrix(4, 4, false);
	transformationMatrix(0, 2) = 1.0;
	transformationMatrix(1, 1) = 1.0;
	transformationMatrix(2, 0) = 1.0;
	transformationMatrix(3, 3) = 1.0;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_BGRA32, FrameType::FORMAT_RGBA32, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterBGRA32::convertBGRA32ToRGBA32), flag, FrameConverterTestUtilities::functionGenericPixel, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterBGRA32::testBGRA32ToY8FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	//                                     | B |
	// | Y | = | 0.114 0.587 0.299 0.0 | * | G |
	//                                     | R |
	//                                     | A |

	MatrixD transformationMatrix(1, 4, false);
	transformationMatrix(0, 0) = 0.114;
	transformationMatrix(0, 1) = 0.587;
	transformationMatrix(0, 2) = 0.299;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_BGRA32, FrameType::FORMAT_Y8, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterBGRA32::convertBGRA32ToY8FullRange), flag, FrameConverterTestUtilities::functionGenericPixel, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterBGRA32::testBGRA32ToYA16(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	//                                     | B |
	// | Y | = | 0.114 0.587 0.299 0.0 | * | G |
	// | A |   | 0.0   0.0   0.0   1.0 |   | R |
	//                                     | A |

	MatrixD transformationMatrix(2, 4, false);
	transformationMatrix(0, 0) = 0.114;
	transformationMatrix(0, 1) = 0.587;
	transformationMatrix(0, 2) = 0.299;
	transformationMatrix(1, 3) = 1.0;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_BGRA32, FrameType::FORMAT_YA16, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterBGRA32::convertBGRA32ToYA16), flag, FrameConverterTestUtilities::functionGenericPixel, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterBGRA32::testBGRA32ToYUV24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// BT.601

	// | Y |   |  0.09765625   0.5039063   0.2578125  0.0    16.0 |   | B |
	// | U | = |  0.4375      -0.2890625  -0.1484375  0.0   128.0 | * | G |
	// | V |   | -0.0703125   -0.3671875   0.4375     0.0   128.0 |   | R |
	//                                                                | A |
	//                                                                | 1 |

	MatrixD transformationMatrix(3, 5, false);

	transformationMatrix(0, 0) = 0.09765625;
	transformationMatrix(1, 0) = 0.4375;
	transformationMatrix(2, 0) = -0.0703125;

	transformationMatrix(0, 1) = 0.5039063;
	transformationMatrix(1, 1) = -0.2890625;
	transformationMatrix(2, 1) = -0.3671875;

	transformationMatrix(0, 2) = 0.2578125;
	transformationMatrix(1, 2) = -0.1484375;
	transformationMatrix(2, 2) = 0.4375;

	transformationMatrix(0, 4) = 16.0;
	transformationMatrix(1, 4) = 128.0;
	transformationMatrix(2, 4) = 128.0;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_BGRA32, FrameType::FORMAT_YUV24, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterBGRA32::convertBGRA32ToYUV24), flag, FrameConverterTestUtilities::functionGenericPixel, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

}

}

}
