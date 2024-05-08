/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverterRGGB10_Packed.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/FrameConverterRGGB10_Packed.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameConverterRGGB10_Packed::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	Log::info() << "---   RGGB10_PACKED converter test:   ---";
	Log::info() << " ";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	{
		Log::info() << "Testing RGGB10_PACKED to BGR24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGGB10_PackedToBGR24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing RGGB10_PACKED to RGB24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testRGGB10_PackedToRGB24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing RGGB10_PACKED to RGB24 conversion with black-level subtraction, white balancing, and gamma encoding at resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testConvertRGGB10_PackedToRGB24BlacklevelWhiteBalanceGammaLUT(randomGenerator, width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "RGGB10_PACKED converter tests succeeded.";
	}
	else
	{
		Log::info() << "RGGB10_PACKED converter tests FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterRGGB10_Packed, RGGB10_PackedToBGR24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGGB10_Packed::testRGGB10_PackedToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGGB10_Packed, RGGB10_PackedToBGR24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGGB10_Packed::testRGGB10_PackedToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGGB10_Packed, RGGB10_PackedToBGR24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGGB10_Packed::testRGGB10_PackedToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGGB10_Packed, RGGB10_PackedToBGR24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGGB10_Packed::testRGGB10_PackedToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGGB10_Packed, RGGB10_PackedToRGB24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGGB10_Packed::testRGGB10_PackedToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGGB10_Packed, RGGB10_PackedToRGB24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGGB10_Packed::testRGGB10_PackedToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGGB10_Packed, RGGB10_PackedToRGB24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGGB10_Packed::testRGGB10_PackedToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGGB10_Packed, RGGB10_PackedToRGB24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGGB10_Packed::testRGGB10_PackedToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGGB10_Packed, RGGB10_PackedToRGB24BlackLevelWhiteBalanceGammaLUTNormal)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGGB10_Packed::testConvertRGGB10_PackedToRGB24BlacklevelWhiteBalanceGammaLUT(randomGenerator, GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGGB10_Packed, RGGB10_PackedToRGB24BlackLevelWhiteBalanceGammaLUTFlipped)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGGB10_Packed::testConvertRGGB10_PackedToRGB24BlacklevelWhiteBalanceGammaLUT(randomGenerator, GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGGB10_Packed, RGGB10_PackedToRGB24BlackLevelWhiteBalanceGammaLUTMirrored)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGGB10_Packed::testConvertRGGB10_PackedToRGB24BlacklevelWhiteBalanceGammaLUT(randomGenerator, GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGGB10_Packed, RGGB10_PackedToRGB24BlackLevelWhiteBalanceGammaLUTFlippedMirrored)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGGB10_Packed::testConvertRGGB10_PackedToRGB24BlacklevelWhiteBalanceGammaLUT(randomGenerator, GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterRGGB10_Packed::testRGGB10_PackedToBGR24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | B8 |   |   0.0        0.0      1 / 2^2 |   | R10 |
	// | G8 | = |   0.0      1 / 2^2      0.0   | * | G10 |
	// | R8 |   | 1 / 2^2      0.0        0.0   |   | B10 |

	MatrixD transformationMatrix(3, 3, false);
	transformationMatrix(0, 2) = 1.0 / 4.003913895; // (2^10 - 1) / x < 255.5, x = 4.0039138943248532289628180039139
	transformationMatrix(1, 1) = 1.0 / 4.003913895;
	transformationMatrix(2, 0) = 1.0 / 4.003913895;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGGB10_PACKED, FrameType::FORMAT_BGR24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGGB10_Packed::convertRGGB10_PackedToBGR24), flag, TestFrameConverterRGGB10_Packed::PixelFunctorRGGB10_Packed::pixelFunctionRGGB10_Packed, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGGB10_Packed::testRGGB10_PackedToRGB24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | R8 |   | 1 / 2^2      0.0        0.0   |   | R10 |
	// | G8 | = |   0.0      1 / 2^2      0.0   | * | G10 |
	// | B8 |   |   0.0        0.0      1 / 2^2 |   | B10 |

	MatrixD transformationMatrix(3, 3, false);
	transformationMatrix(0, 0) = 1.0 / 4.003913895; // (2^10 - 1) / x < 255.5, x = 4.0039138943248532289628180039139
	transformationMatrix(1, 1) = 1.0 / 4.003913895;
	transformationMatrix(2, 2) = 1.0 / 4.003913895;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGGB10_PACKED, FrameType::FORMAT_RGB24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGGB10_Packed::convertRGGB10_PackedToRGB24), flag, TestFrameConverterRGGB10_Packed::PixelFunctorRGGB10_Packed::pixelFunctionRGGB10_Packed, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterRGGB10_Packed::testConvertRGGB10_PackedToRGB24BlacklevelWhiteBalanceGammaLUT(RandomGenerator& randomGenerator, const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// Random parameters
	const uint16_t blackLevel = uint16_t(RandomI::random(randomGenerator, 1023u));
	const double whiteBalance[3] =
	{
	 	RandomD::scalar(randomGenerator, 0.1, 1.9),
		RandomD::scalar(randomGenerator, 0.1, 1.9),
		RandomD::scalar(randomGenerator, 0.1, 1.9),
	};

	// Gamma value, note: increasing the size of this range beyond the current setting will cause thresholdMaximalErrorToInteger to be exceeded occasionally (but clearly). Inaccuracies of the LUT-approximation?
	const double gamma = RandomD::scalar(randomGenerator, 0.5, 1.5);

	// Functor for the validation
	PixelFunctorRGGB10_Packed pixelFunctorRGGB10_Packed(blackLevel, whiteBalance, gamma);

	// These parameters will be passed to the actual conversion function
	const float options[5] =
	{
		float(blackLevel),
		float(whiteBalance[0]),
		float(whiteBalance[1]),
		float(whiteBalance[2]),
	 	float(gamma),
	 };

	// Transformation matrix for convert 10-bit values into 8-bit values.
	//
	// | R8 |   | 1 / 2^2      0.0        0.0   |   | R10 |
	// | G8 | = |   0.0      1 / 2^2      0.0   | * | G10 |
	// | B8 |   |   0.0        0.0      1 / 2^2 |   | B10 |

	MatrixD transformationMatrix(3, 3, false);
	transformationMatrix(0, 0) = 1.0 / 4.003913895; // (2^10 - 1) / x < 255.5, x = 4.0039138943248532289628180039139
	transformationMatrix(1, 1) = 1.0 / 4.003913895;
	transformationMatrix(2, 2) = 1.0 / 4.003913895;

	constexpr unsigned int thresholdMaximalErrorToInteger = 5u;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGGB10_PACKED, FrameType::FORMAT_RGB24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGGB10_Packed::convertRGGB10_PackedToRGB24BlacklevelWhiteBalanceGammaLUT), flag, pixelFunctorRGGB10_Packed, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker, thresholdMaximalErrorToInteger, options);
}

MatrixD TestFrameConverterRGGB10_Packed::PixelFunctorRGGB10_Packed::pixelFunctionRGGB10_PackedWithBlackLevelWhiteBalanceGamma(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag, const uint16_t blackLevel, const double* whiteBalance, const double gamma)
{
	ocean_assert(frame.isValid());
	ocean_assert(x < frame.width() && y < frame.height());
	ocean_assert_and_suppress_unused(conversionFlag == CV::FrameConverter::CONVERT_NORMAL, conversionFlag); // we expect the target frame to have a conversion flag

	ocean_assert(frame.pixelFormat() == FrameType::FORMAT_RGGB10_PACKED);
	ocean_assert(frame.planeChannels(0u) == 1u);
	ocean_assert(frame.width() >= 4u && frame.width() % 4u == 0u);
	ocean_assert(frame.height() >= 2u && frame.height() % 2u == 0u);

	ocean_assert(blackLevel < 1024u);
	ocean_assert(whiteBalance != nullptr);
	ocean_assert(gamma >= 0);

	/**
	 * The packed pixel format representing a Bayer mosaic pattern for images with red, green, and blue channels with order RGGB for a 2x2 pixel block.
	 * The format has the byte order R G for the upper two pixels, and G B for the lower two pixels in a 2x2 pixel block.<br>
	 * Images with this pixel format have a resolution which is a multiple of 4x2 pixels.<br>
	 * The Pixel format stores 10 bits per pixel (and channel), packed so that four consecutive pixels fit into five bytes.<br>
	 * The higher 8 bits of each pixel are stored in the first four bytes, the lower 2 bits of all four pixels are stored in the fifth byte.<br>
	 * Here is the memory layout:
	 * <pre>
	 *   Pixel:  0        1        2        3        0 1 2 3    4        5        6        7        4 5 6 7
	 *    Byte:  0        1        2        3        4          5        6        7        8        9
	 *     Bit:  01234567 89ABCDEF 01234567 89ABCDEF 01234567   01234567 89ABCDEF 01234567 89ABCDEF 01234567
	 * Channel:  0        1        2        3        0 1 2 3    4        5        6        7        4 5 6 7
	 *   Color:  RRRRRRRR GGGGGGGG RRRRRRRR GGGGGGGG RRGGRRGG   RRRRRRRR GGGGGGGG RRRRRRRR GGGGGGGG RRGGRRGG ........
	 *   Color:  GGGGGGGG BBBBBBBB GGGGGGGG BBBBBBBB GGBBGGBB   GGGGGGGG BBBBBBBB GGGGGGGG BBBBBBBB GGBBGGBB ........
	 * </pre>
	 */

	// row 0:   R G R G X   R G R G X   R G . .
	// row 1:   G B G B X   G B G B X   G B . .
	// row 2:   R G R G X   R G R G X   R G . .
	// row 3:   G B G B X   G B G B X   G B . .
	// row 4:   R G R G X   R G R G X   R G . .
	// row 5:   . . .

	double redValue = -1.0;
	double greenValue = -1.0;
	double blueValue = -1.0;

	if (y % 2u == 0u)
	{
		// we have a R G R G ... row

		if (x % 2u == 0u)
		{
			// we are exactly on a red channel pixel
			redValue = elementValue(frame, x, int(y), blackLevel, whiteBalance[0]);

			if (x == 0u)
			{
				// left border handling

				greenValue = (elementValue(frame, x, int(y) - 1, blackLevel, whiteBalance[1]) + elementValue(frame, x, int(y + 1u), blackLevel, whiteBalance[1])) * 0.5;

				blueValue = (elementValue(frame, x + 1u, int(y) - 1, blackLevel, whiteBalance[2]) + elementValue(frame, x + 1u, int(y + 1u), blackLevel, whiteBalance[2])) * 0.5;
			}
			else
			{
				ocean_assert(x < frame.width() - 1u);

				// we are surrounded by 4 green channels (top, left, bottom, right)
				greenValue = (elementValue(frame, x, int(y) - 1, blackLevel, whiteBalance[1]) + elementValue(frame, x - 1u, int(y), blackLevel, whiteBalance[1]) + elementValue(frame, x + 1u, int(y), blackLevel, whiteBalance[1]) + elementValue(frame, x, int(y + 1u), blackLevel, whiteBalance[1])) * 0.25;

				// we are surrounded by 4 blue channels (top-left, top-right, bottom-left, bottom-right)
				blueValue = (elementValue(frame, x - 1u, int(y) - 1, blackLevel, whiteBalance[2]) + elementValue(frame, x + 1u, int(y) - 1, blackLevel, whiteBalance[2]) + elementValue(frame, x - 1u, int(y + 1u), blackLevel, whiteBalance[2]) + elementValue(frame, x + 1u, int(y + 1u), blackLevel, whiteBalance[2])) * 0.25;
			}
		}
		else
		{
			if (x == frame.width() - 1u)
			{
				// special border handling

				redValue = elementValue(frame, x - 1u, int(y), blackLevel, whiteBalance[0]);
			}
			else
			{
				ocean_assert(x >= 1u);

				// we are inbetween two red channel pixels (left, right)
				redValue = (elementValue(frame, x - 1u, int(y), blackLevel, whiteBalance[0]) + elementValue(frame, x + 1u, int(y), blackLevel, whiteBalance[0])) * 0.5;
			}

			// we are exactly on a green channel pixel
			greenValue = elementValue(frame, x, int(y), blackLevel, whiteBalance[1]);

			// we are inbetween to blue channels (top, bottom)
			blueValue = (elementValue(frame, x, int(y) - 1, blackLevel, whiteBalance[2]) + elementValue(frame, x, int(y + 1u), blackLevel, whiteBalance[2])) * 0.5;
		}
	}
	else
	{
		// we have a G B G B ... row

		if (x % 2u == 0u)
		{
			// we are inbetween two red channel pixels (top, bottom)
			redValue = (elementValue(frame, x, int(y) - 1, blackLevel, whiteBalance[0]) + elementValue(frame, x, int(y + 1u), blackLevel, whiteBalance[0])) * 0.5;

			// we are exactly on a green channel pixel
			greenValue = elementValue(frame, x, int(y), blackLevel, whiteBalance[1]);

			if (x == 0u)
			{
				// special border handling

				blueValue = elementValue(frame, x + 1u, int(y), blackLevel, whiteBalance[2]);
			}
			else
			{
				ocean_assert(x >= 1u);

				// we are inbetween two blue channel pixels (left, right)
				blueValue = (elementValue(frame, x - 1u, int(y), blackLevel, whiteBalance[2]) + elementValue(frame, x + 1u, int(y), blackLevel, whiteBalance[2])) * 0.5;
			}
		}
		else
		{
			if (x == frame.width() - 1u)
			{
				// special border handling

				redValue = (elementValue(frame, x - 1u, int(y) - 1, blackLevel, whiteBalance[0]) + elementValue(frame, x - 1u, int(y + 1u), blackLevel, whiteBalance[0])) * 0.5;

				greenValue = (elementValue(frame, x, int(y) - 1, blackLevel, whiteBalance[1]) + elementValue(frame, x, int(y + 1u), blackLevel, whiteBalance[1])) * 0.5;
			}
			else
			{
				// we are surrounded by 4 red channels (top-left, top-right, bottom-left, bottom-right)
				redValue = (elementValue(frame, x - 1u, int(y) - 1, blackLevel, whiteBalance[0]) + elementValue(frame, x + 1u, int(y) - 1, blackLevel, whiteBalance[0]) + elementValue(frame, x - 1u, int(y + 1u), blackLevel, whiteBalance[0]) + elementValue(frame, x + 1u, int(y + 1u), blackLevel, whiteBalance[0])) * 0.25;

				// we are surrounded by 4 green channels (top, left, bottom, right)
				greenValue = (elementValue(frame, x, int(y) - 1, blackLevel, whiteBalance[1]) + elementValue(frame, x - 1u, int(y), blackLevel, whiteBalance[1]) + elementValue(frame, x + 1u, int(y), blackLevel, whiteBalance[1]) + elementValue(frame, x, int(y + 1u), blackLevel, whiteBalance[1])) * 0.25;
			}

			// we are exactly on a blue channel pixel
			blueValue = elementValue(frame, x, int(y), blackLevel, whiteBalance[2]);
		}
	}

	ocean_assert(redValue >= 0.0 && greenValue >= 0.0 && blueValue >= 0.0);

	if (gamma >= 0.0 && gamma != 1.0)
	{
		redValue = minmax<double>(0.0, 1023.0 * NumericD::pow(redValue / 1023.0, gamma), 1023.0);
		greenValue = minmax<double>(0.0, 1023.0 * NumericD::pow(greenValue / 1023.0, gamma), 1023.0);
		blueValue = minmax<double>(0.0, 1023.0 * NumericD::pow(blueValue / 1023.0, gamma), 1023.0);
	}

	ocean_assert(redValue < 1023.5 && greenValue < 1023.5 && blueValue < 1023.5);

	MatrixD colorVector(3, 1);

	colorVector(0, 0) = redValue;
	colorVector(1, 0) = greenValue;
	colorVector(2, 0) = blueValue;

	return colorVector;
}

double TestFrameConverterRGGB10_Packed::PixelFunctorRGGB10_Packed::elementValue(const Frame& frame, const unsigned int x, const int y, const uint16_t blackLevel, const double whiteBalance)
{
	ocean_assert(frame.isValid());
	ocean_assert(x < frame.width() && y >= -1 && y <= int(frame.height()));
	ocean_assert(blackLevel < 1024u);
	ocean_assert(whiteBalance >= 0);

	const unsigned int blockIndex = x / 4u;
	const unsigned int pixelIndex = x % 4u;

	unsigned int mirroredY = (unsigned int)(y);

	if (y < 0)
	{
		mirroredY = 1u; // mirrored to the first G B G B ... row
	}
	else if (y >= int(frame.height()))
	{
		mirroredY = frame.height() - 2u; // mirrored to last R G R G ... row
	}

	const uint8_t* block = frame.constrow<uint8_t>(mirroredY) + blockIndex * 5u;

	// the 8 high bits
	const unsigned int highBits = block[pixelIndex];

	const unsigned int lowBitsPattern = 0b11 << (pixelIndex * 2u);

	// the 2 low bits
	const unsigned int lowBits = (block[4] & lowBitsPattern) >> (pixelIndex * 2u);
	ocean_assert(lowBits < 4u);

	const unsigned int allBits = (highBits << 2u) | lowBits;
	ocean_assert(allBits < 1024u);

	// Apply black level subtraction, white balance, and clamping
	const uint16_t blackLevelledValue = allBits > blackLevel ? uint16_t(allBits - blackLevel) : 0u;
	const double whiteBalancedValue = double(blackLevelledValue) * whiteBalance;

	return minmax<double>(0.0, whiteBalancedValue, 1023.0);
}

}

}

}
