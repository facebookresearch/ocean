/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverterY8.h"

#include "ocean/cv/FrameConverterY8.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{


bool TestFrameConverterY8::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	Log::info() << "---   Y8 converter test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	{
		Log::info() << "Testing Y8 to BGR24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY8ToBGR24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y8 to RGB24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY8ToRGB24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y8 to RGBA32 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY8ToRGBA32(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y8 to Y8 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY8ToY8(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y8 to Y8 conversion with LUT gamma correction (x^" << String::toAString(TestFrameConverter::ValueProvider::get().gammaValue(), 1u) << ") with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY8ToY8GammaLUT(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Y8 converter tests succeeded.";
	}
	else
	{
		Log::info() << "Y8 converter tests FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterY8, Y8ToBGR24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY8::testY8ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY8, Y8ToBGR24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY8::testY8ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY8, Y8ToBGR24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY8::testY8ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY8, Y8ToBGR24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY8::testY8ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY8, Y8ToRGB24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY8::testY8ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY8, Y8ToRGB24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY8::testY8ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY8, Y8ToRGB24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY8::testY8ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY8, Y8ToRGB24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY8::testY8ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY8, Y8ToRGBA32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY8::testY8ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY8, Y8ToRGBA32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY8::testY8ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY8, Y8ToRGBA32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY8::testY8ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY8, Y8ToRGBA32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY8::testY8ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY8, Y8ToY8Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY8::testY8ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY8, Y8ToY8Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY8::testY8ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY8, Y8ToY8Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY8::testY8ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY8, Y8ToY8FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY8::testY8ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY8, Y8ToY8GammaLUTNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY8::testY8ToY8GammaLUT(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY8, Y8ToY8GammaLUTFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY8::testY8ToY8GammaLUT(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY8, Y8ToY8GammaLUTMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY8::testY8ToY8GammaLUT(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY8, Y8ToY8GammaLUTFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY8::testY8ToY8GammaLUT(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterY8::testY8ToBGR24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | B |   | 1 |
	// | G | = | 1 | * | Y |
	// | R |   | 1 |

	MatrixD transformationMatrix(3, 1, false);
	transformationMatrix(0, 0) = 1;
	transformationMatrix(1, 0) = 1;
	transformationMatrix(2, 0) = 1;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y8, FrameType::FORMAT_BGR24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY8::convertY8ToBGR24), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY8::testY8ToRGB24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | R |   | 1 |
	// | G | = | 1 | * | Y |
	// | B |   | 1 |

	MatrixD transformationMatrix(3, 1, false);
	transformationMatrix(0, 0) = 1;
	transformationMatrix(1, 0) = 1;
	transformationMatrix(2, 0) = 1;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y8, FrameType::FORMAT_RGB24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY8::convertY8ToRGB24), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY8::testY8ToRGBA32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | R |   | 1 0 |
	// | G | = | 1 0 | * | Y |
	// | B |   | 1 0 |   | 1 |
	// | A |   | 0 A |

	MatrixD transformationMatrix(4, 2, false);
	transformationMatrix(0, 0) = 1;
	transformationMatrix(1, 0) = 1;
	transformationMatrix(2, 0) = 1;
	transformationMatrix(3, 1) = double(TestFrameConverter::ValueProvider::get().alphaValue());

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y8, FrameType::FORMAT_RGBA32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY8::convertY8ToRGBA32), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY8::testY8ToY8(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y | = | 1 | * | Y |

	const MatrixD transformationMatrix(1, 1, true);

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y8, FrameType::FORMAT_Y8, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY8::convertY8ToY8), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY8::testY8ToY8GammaLUT(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y8 | = | 1 | * | 255 * (Y10 / 255) ^ gamma |

	const MatrixD transformationMatrix(1, 1, true);

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y8, FrameType::FORMAT_Y8, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY8::convertY8ToY8GammaLUT), flag, TestFrameConverterY8::pixelFunctionY8Gamma, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

MatrixD TestFrameConverterY8::pixelFunctionY8Gamma(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
{
	ocean_assert(frame.isValid());
	ocean_assert(x < frame.width() && y < frame.height());
	ocean_assert_and_suppress_unused(conversionFlag == CV::FrameConverter::CONVERT_NORMAL, conversionFlag); // we expect the target frame to have a conversion flag

	ocean_assert(frame.pixelFormat() == FrameType::FORMAT_Y8);
	ocean_assert(frame.planeChannels(0u) == 1u);
	ocean_assert(frame.width() >= 1u);

	const uint8_t pixelValue = frame.constpixel<uint8_t>(x, y)[0];

	MatrixD colorVector(1, 1);

	colorVector(0, 0) = NumericD::pow(double(pixelValue) / 255.0, double(TestFrameConverter::ValueProvider::get().gammaValue())) * 255.0;

	return colorVector;
}

}

}

}
