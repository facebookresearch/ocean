/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverterYVU24.h"

#include "ocean/cv/FrameConverterYVU24.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameConverterYVU24::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	Log::info() << "---   YVU24 converter test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	{
		Log::info() << "Testing YVU24 to BGR24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testYVU24ToBGR24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing YVU24 to RGB24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testYVU24ToRGB24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing YVU24 to Y8 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testYVU24ToY8(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing YVU24 to YUV24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testYVU24ToYUV24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing YVU24 to YVU24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testYVU24ToYVU24(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing YVU24 to Y_V_U12 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			allSucceeded = testYVU24ToY_V_U12(width, height, flag, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "YVU24 converter tests succeeded.";
	}
	else
	{
		Log::info() << "YVU24 converter tests FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterYVU24, YVU24ToBGR24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYVU24::testYVU24ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYVU24, YVU24ToBGR24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYVU24::testYVU24ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYVU24, YVU24ToBGR24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYVU24::testYVU24ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYVU24, YVU24ToBGR24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYVU24::testYVU24ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterYVU24, YVU24ToRGB24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYVU24::testYVU24ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYVU24, YVU24ToRGB24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYVU24::testYVU24ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYVU24, YVU24ToRGB24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYVU24::testYVU24ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYVU24, YVU24ToRGB24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYVU24::testYVU24ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterYVU24, YVU24ToY8Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYVU24::testYVU24ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYVU24, YVU24ToY8Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYVU24::testYVU24ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYVU24, YVU24ToY8Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYVU24::testYVU24ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYVU24, YVU24ToY8FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYVU24::testYVU24ToY8(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterYVU24, YVU24ToYUV24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYVU24::testYVU24ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYVU24, YVU24ToYUV24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYVU24::testYVU24ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYVU24, YVU24ToYUV24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYVU24::testYVU24ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYVU24, YVU24ToYUV24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYVU24::testYVU24ToYUV24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterYVU24, YVU24ToYVU24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYVU24::testYVU24ToYVU24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYVU24, YVU24ToYVU24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYVU24::testYVU24ToYVU24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYVU24, YVU24ToYVU24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYVU24::testYVU24ToYVU24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYVU24, YVU24ToYVU24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYVU24::testYVU24ToYVU24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterYVU24, YVU24ToY_V_U12Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYVU24::testYVU24ToY_V_U12(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYVU24, YVU24ToY_V_U12Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYVU24::testYVU24ToY_V_U12(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYVU24, YVU24ToY_V_U12Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYVU24::testYVU24ToY_V_U12(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterYVU24, YVU24ToY_V_U12FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterYVU24::testYVU24ToY_V_U12(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterYVU24::testYVU24ToBGR24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_LimitedRangeYVU24_To_FullRangeBGR24_BT601();

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_YVU24, FrameType::FORMAT_BGR24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterYVU24::convertYVU24ToBGR24), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterYVU24::testYVU24ToRGB24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	const MatrixD transformationMatrix = CV::FrameConverter::transformationMatrix_LimitedRangeYVU24_To_FullRangeRGB24_BT601();

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_YVU24, FrameType::FORMAT_RGB24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterYVU24::convertYVU24ToRGB24), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterYVU24::testYVU24ToY8(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	//                     | Y |
	// | Y | = | 1 0 0 | * | V |
	//                     | U |

	MatrixD transformationMatrix(1, 3, false);
	transformationMatrix(0, 0) = 1.0;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_YVU24, FrameType::FORMAT_Y8, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterYVU24::convertYVU24ToY8), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterYVU24::testYVU24ToYUV24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y |   | 1 0 0 |   | Y |
	// | U | = | 0 0 1 | * | V |
	// | V |   | 0 1 0 |   | U |

	MatrixD transformationMatrix(3, 3, false);
	transformationMatrix(0, 0) = 1.0;
	transformationMatrix(1, 2) = 1.0;
	transformationMatrix(2, 1) = 1.0;

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_YVU24, FrameType::FORMAT_YUV24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterYVU24::convertYVU24ToYUV24), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterYVU24::testYVU24ToYVU24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y |   | 1 0 0 |   | Y |
	// | V | = | 0 1 0 | * | V |
	// | U |   | 0 0 1 |   | U |

	const MatrixD transformationMatrix(3, 3, true);

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_YVU24, FrameType::FORMAT_YVU24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterYVU24::convertYVU24ToYVU24), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterYVU24::testYVU24ToY_V_U12(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | Y |   | 1 0 0 |   | Y |
	// | V | = | 0 1 0 | * | V |
	// | U |   | 0 0 1 |   | U |

	const MatrixD transformationMatrix(3, 3, true);

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_YVU24, FrameType::FORMAT_Y_V_U12, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterYVU24::convertYVU24ToY_V_U12), conversionFlag, pixelFunctionYVU24ForY_V_U12, pixelFunctionY_V_U12, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

MatrixD TestFrameConverterYVU24::pixelFunctionYVU24ForY_V_U12(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
{
	ocean_assert(frame.isValid());
	ocean_assert(x < frame.width() && y < frame.height());
	ocean_assert_and_suppress_unused(conversionFlag == CV::FrameConverter::CONVERT_NORMAL, conversionFlag); // we expect the target frame to have a conversion flag

	MatrixD colorVector(3, 1);

	colorVector(0, 0) = double(frame.constpixel<uint8_t>(x, y, 0u)[0]);

	const unsigned int xLeft = x & 0xFFFFFFFEu; // make value even
	const unsigned int yTop = y & 0xFFFFFFFEu;

	//         012 345
	// top:    YVU YVU
	// bottom: YVU YVU
	colorVector(1, 0) = double((frame.constpixel<uint8_t>(xLeft, yTop)[1u] + frame.constpixel<uint8_t>(xLeft + 1u, yTop)[1u] + frame.constpixel<uint8_t>(xLeft, yTop + 1u)[1u] + frame.constpixel<uint8_t>(xLeft + 1u, yTop + 1u)[1u] + 2u) / 4u);
	colorVector(2, 0) = double((frame.constpixel<uint8_t>(xLeft, yTop)[2u] + frame.constpixel<uint8_t>(xLeft + 1u, yTop)[2u] + frame.constpixel<uint8_t>(xLeft, yTop + 1u)[2u] + frame.constpixel<uint8_t>(xLeft + 1u, yTop + 1u)[2u] + 2u) / 4u);

	return colorVector;
}

MatrixD TestFrameConverterYVU24::pixelFunctionY_V_U12(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
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

	colorVector(0, 0) = double(*frame.constpixel<uint8_t>(xAdjusted, yAdjusted, 0u));
	colorVector(1, 0) = double(*frame.constpixel<uint8_t>(xAdjusted_2, yAdjusted_2, 1u));
	colorVector(2, 0) = double(*frame.constpixel<uint8_t>(xAdjusted_2, yAdjusted_2, 2u));

	return colorVector;
}

}

}

}
