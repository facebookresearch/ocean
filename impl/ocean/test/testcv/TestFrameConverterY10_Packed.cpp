/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverterY10_Packed.h"

#include "ocean/cv/FrameConverterY10_Packed.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{


bool TestFrameConverterY10_Packed::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	Log::info() << "---   Y10_PACKED converter test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	{
		Log::info() << "Testing Y10_PACKED to BGR24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY10_PackedToBGR24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y10_PACKED to RGB24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY10_PackedToRGB24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing linear Y10_PACKED to Y8 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY10_PackedToY8Linear(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y10_PACKED to Y8 conversion with LUT gamma correction (x^" << String::toAString(TestFrameConverter::ValueProvider::get().gammaValue(), 1u) << ") with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY10_PackedToY8GammaLUT(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y10_PACKED to Y8 conversion with approximated gamma correction (x^" << String::toAString(TestFrameConverter::ValueProvider::get().gammaValue(), 1u) << ") with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY10_PackedToY8GammaApproximated(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing Y10_PACKED to Y10 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testY10_PackedToY10(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Y10_PACKED converter tests succeeded.";
	}
	else
	{
		Log::info() << "Y10_PACKED converter tests FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterY10_Packed, Y10_PackedToBGR24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY10_Packed::testY10_PackedToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY10_Packed, Y10_PackedToBGR24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY10_Packed::testY10_PackedToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY10_Packed, Y10_PackedToBGR24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY10_Packed::testY10_PackedToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY10_Packed, Y10_PackedToBGR24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY10_Packed::testY10_PackedToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY10_Packed, Y10_PackedToRGB24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY10_Packed::testY10_PackedToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY10_Packed, Y10_PackedToRGB24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY10_Packed::testY10_PackedToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY10_Packed, Y10_PackedToRGB24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY10_Packed::testY10_PackedToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY10_Packed, Y10_PackedToRGB24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY10_Packed::testY10_PackedToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY10_Packed, Y10_PackedToY8LinearNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY10_Packed::testY10_PackedToY8Linear(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY10_Packed, Y10_PackedToY8LinearFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY10_Packed::testY10_PackedToY8Linear(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY10_Packed, Y10_PackedToY8LinearMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY10_Packed::testY10_PackedToY8Linear(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY10_Packed, Y10_PackedToY8LinearFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY10_Packed::testY10_PackedToY8Linear(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY10_Packed, Y10_PackedToY8GammaLUTNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY10_Packed::testY10_PackedToY8GammaLUT(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY10_Packed, Y10_PackedToY8GammaLUTFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY10_Packed::testY10_PackedToY8GammaLUT(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY10_Packed, Y10_PackedToY8GammaLUTMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY10_Packed::testY10_PackedToY8GammaLUT(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY10_Packed, Y10_PackedToY8GammaLUTFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY10_Packed::testY10_PackedToY8GammaLUT(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY10_Packed, Y10_PackedToY8GammaApproximatedNormal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY10_Packed::testY10_PackedToY8GammaApproximated(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY10_Packed, Y10_PackedToY8GammaApproximatedFlipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY10_Packed::testY10_PackedToY8GammaApproximated(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY10_Packed, Y10_PackedToY8GammaApproximatedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY10_Packed::testY10_PackedToY8GammaApproximated(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY10_Packed, Y10_PackedToY8GammaApproximatedFlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY10_Packed::testY10_PackedToY8GammaApproximated(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterY10_Packed, Y10_PackedToY10Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY10_Packed::testY10_PackedToY10(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY10_Packed, Y10_PackedToY10Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY10_Packed::testY10_PackedToY10(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY10_Packed, Y10_PackedToY10Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY10_Packed::testY10_PackedToY10(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterY10_Packed, Y10_PackedToY10FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterY10_Packed::testY10_PackedToY10(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterY10_Packed::testY10_PackedToBGR24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | B8 |   | 1 / 2^2 |
	// | G8 | = | 1 / 2^2 | * | Y10 |
	// | R8 |   | 1 / 2^2 |

	MatrixD transformationMatrix(3, 1, false);
	transformationMatrix(0, 0) = 1.0 / 4.003913895; // (2^10 - 1) / x < 255.5, x = 4.0039138943248532289628180039139
	transformationMatrix(1, 0) = 1.0 / 4.003913895;
	transformationMatrix(2, 0) = 1.0 / 4.003913895;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y10_PACKED, FrameType::FORMAT_BGR24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY10_Packed::convertY10_PackedToBGR24), flag, TestFrameConverterY10_Packed::pixelFunctionY10_Packed<false>, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY10_Packed::testY10_PackedToRGB24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | R8 |   | 1 / 2^2 |
	// | G8 | = | 1 / 2^2 | * | Y10 |
	// | B8 |   | 1 / 2^2 |

	MatrixD transformationMatrix(3, 1, false);
	transformationMatrix(0, 0) = 1.0 / 4.003913895; // (2^10 - 1) / x < 255.5, x = 4.0039138943248532289628180039139
	transformationMatrix(1, 0) = 1.0 / 4.003913895;
	transformationMatrix(2, 0) = 1.0 / 4.003913895;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y10_PACKED, FrameType::FORMAT_RGB24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY10_Packed::convertY10_PackedToRGB24), flag, TestFrameConverterY10_Packed::pixelFunctionY10_Packed<false>, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY10_Packed::testY10_PackedToY8Linear(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y8 | = | 1 / 2^2 | * | Y10 |

	MatrixD transformationMatrix(1, 1, false);
	transformationMatrix(0, 0) = 1.0 / 4.003913895; // (2^10 - 1) / x < 255.5, x = 4.0039138943248532289628180039139

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y10_PACKED, FrameType::FORMAT_Y8, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY10_Packed::convertY10_PackedToY8Linear), flag, TestFrameConverterY10_Packed::pixelFunctionY10_Packed<false>, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY10_Packed::testY10_PackedToY8GammaLUT(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y8 | = | 1 | * | 255 * (Y10 / 1023) ^ gamma |

	const MatrixD transformationMatrix(1, 1, true);

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y10_PACKED, FrameType::FORMAT_Y8, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY10_Packed::convertY10_PackedToY8GammaLUT), flag, TestFrameConverterY10_Packed::pixelFunctionY10_Packed<true>, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterY10_Packed::testY10_PackedToY8GammaApproximated(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y8 | = | 1 | * | approximatedGamma(Y10) |

	const MatrixD transformationMatrix(1, 1, true);

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y10_PACKED, FrameType::FORMAT_Y8, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY10_Packed::convertY10_PackedToY8GammaApproximated), flag, TestFrameConverterY10_Packed::pixelFunctionY10_PackedApproximated, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, 5u);
}

bool TestFrameConverterY10_Packed::testY10_PackedToY10(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y10 | = | 1 | * | Y10 |

	MatrixD transformationMatrix(1, 1, true);

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_Y10_PACKED, FrameType::FORMAT_Y10, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterY10_Packed::convertY10_PackedToY10), flag, TestFrameConverterY10_Packed::pixelFunctionY10_Packed<false>, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 1023.0, testDuration, worker);
}

template <bool tApplyGamma>
MatrixD TestFrameConverterY10_Packed::pixelFunctionY10_Packed(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
{
	ocean_assert(frame.isValid());
	ocean_assert(x < frame.width() && y < frame.height());
	ocean_assert_and_suppress_unused(conversionFlag == CV::FrameConverter::CONVERT_NORMAL, conversionFlag); // we expect the target frame to have a conversion flag

	ocean_assert(frame.pixelFormat() == FrameType::FORMAT_Y10_PACKED);
	ocean_assert(frame.planeChannels(0u) == 1u);
	ocean_assert(frame.width() >= 4u && frame.width() % 4u == 0u);

	/*
	 * Pixel format with byte order Y and 10 bits per pixel, packed so that four consecutive pixels fit into five bytes.
	 * The higher 8 bits of each pixel are stored in the first four bytes, the lower 2 bits of all four pixels are stored in the fifth byte.
	 * Here is the memory layout:
	 * <pre>
	 *   Pixel:  0        1        2        3        0 1 2 3    4        5        6        7        4 5 6 7
	 *    Byte:  0        1        2        3        4          5        6        7        8        9
	 *     Bit:  01234567 89ABCDEF 01234567 89ABCDEF 01234567   01234567 89ABCDEF 01234567 89ABCDEF 01234567
	 * Channel:  0        1        2        3        0 1 2 3    4        5        6        7        4 5 6 7
	 *   Color:  YYYYYYYY YYYYYYYY YYYYYYYY YYYYYYYY YYYYYYYY   YYYYYYYY YYYYYYYY YYYYYYYY YYYYYYYY YYYYYYYYY ........
	 * </pre>
	 */

	const unsigned int blockIndex = x / 4u;
	const unsigned int pixelIndex = x % 4u;

	const uint8_t* block = frame.constrow<uint8_t>(y) + blockIndex * 5u;

	// the 8 high bits
	const unsigned int highBits = block[pixelIndex];

	const unsigned int lowBitsPattern = 0b11 << (pixelIndex * 2u);

	// the 2 low bits
	const unsigned int lowBits = (block[4] & lowBitsPattern) >> (pixelIndex * 2u);
	ocean_assert(lowBits < 4u);

	const unsigned int allBits = (highBits << 2u) | lowBits;
	ocean_assert(allBits < 1024u);

	MatrixD colorVector(1, 1);

	if constexpr (tApplyGamma)
	{
		colorVector(0, 0) = NumericD::pow(double(allBits) / 1023.0, double(TestFrameConverter::ValueProvider::get().gammaValue())) * 255.0;
	}
	else
	{
		colorVector(0, 0) = double(allBits);
	}

	return colorVector;
}

MatrixD TestFrameConverterY10_Packed::pixelFunctionY10_PackedApproximated(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
{
	MatrixD colorVector = pixelFunctionY10_Packed<false>(frame, x, y, conversionFlag);

	const double unpacked = colorVector(0, 0);
	ocean_assert(unpacked >= 0.0 && unpacked < 1024.0);

	const float gamma = float(TestFrameConverter::ValueProvider::get().gammaValue());

	constexpr float step01 = 40.0f;
	constexpr float step12 = 280.0f;

	if (unpacked <= step01)
	{
		const float m0 = 255.0f * NumericF::pow(step01 / 1023.0f, gamma) / step01;

		colorVector(0, 0) = double(unpacked * m0);
	}
	else if (unpacked > step12)
	{
		const float m2 = 255.0f * (NumericF::pow(1.0f, gamma) - NumericF::pow(step12 / 1023.0f, gamma)) / (1023.0f - step12);
		const float c2 = 255.0f - 1023.0f * m2;

		colorVector(0, 0) = double(unpacked * m2 + c2);
	}
	else
	{
		const float m1 = 255.0f * (NumericF::pow(step12 / 1023.0f, gamma) - NumericF::pow(step01 / 1023.0f, gamma)) / (step12 - step01);
		const float c1 = 255.0f * NumericF::pow(step12 / 1023.0f, gamma) - step12 * m1;

		colorVector(0, 0) = double(unpacked * m1 + c1);
	}

#ifdef OCEAN_DEBUG
	const double approximated = colorVector(0, 0);
	ocean_assert(approximated >= 0.0 && approximated < 256.0);

	const double precise = NumericD::pow(unpacked / 1023.0, double(gamma)) * 255.0;

	ocean_assert(NumericD::isEqual(approximated, precise, 255.0 * 0.1));
#endif

	return colorVector;
}

}

}

}
