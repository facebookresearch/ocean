// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testcv/TestFrameConverterRGB32.h"

#include "ocean/cv/FrameConverterRGB32.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameConverterRGB32::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	Log::info() << "---   RGB32 converter test:   ---";
	Log::info() << " ";

	const auto flags = TestFrameConverter::conversionFlags();

	bool allSucceeded = true;

	{
		Log::info() << "Testing RGB32 to RGB24 conversion with resolution " << width << "x" << height << ":";
		Log::info() << " ";

		for (size_t n = 0; n < flags.size(); ++n)
		{
			Log::info().newLine(n != 0);
			allSucceeded = testRGB32ToRGB24(width, height, flags[n], testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	{
		Log::info() << "Testing RGB32 to RGBA32 conversion with resolution " << width << "x" << height << ":";
		Log::info() << " ";

		for (size_t n = 0; n < flags.size(); ++n)
		{
			Log::info().newLine(n != 0);
			allSucceeded = testRGB32ToRGBA32(width, height, flags[n], testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "RGB32 converter test succeeded.";
	}
	else
	{
		Log::info() << "RGB32 converter test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterRGB32, RGB32ToRGB24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB32::testRGB32ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB32, RGB32ToRGB24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB32::testRGB32ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB32, RGB32ToRGB24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB32::testRGB32ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB32, RGB32ToRGB24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB32::testRGB32ToRGB24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterRGB32, RGB32ToRGBA32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB32::testRGB32ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB32, RGB32ToRGBA32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB32::testRGB32ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB32, RGB32ToRGBA32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB32::testRGB32ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterRGB32, RGB32ToRGBA32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterRGB32::testRGB32ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterRGB32::testRGB32ToRGB24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | R |   | 1 0 0 0|   | R |
	// | G | = | 0 1 0 0| * | G |
	// | B |   | 0 0 1 0|   | B |
	//                      | ? |

	MatrixD transformationMatrix(3, 4, true);

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGB32, FrameType::FORMAT_RGB24, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGB32::convertRGB32ToRGB24), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}


bool TestFrameConverterRGB32::testRGB32ToRGBA32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | R |   | 1 0 0 0 0 |   | R |
	// | G | = | 0 1 0 0 0 | * | G |
	// | B |   | 0 0 1 0 0 |   | B |
	// | A |   | 0 0 0 0 A |   | ? |
	//                         | 1 |

	MatrixD transformationMatrix(4, 5, false);
	transformationMatrix(0, 0) = 1;
	transformationMatrix(1, 1) = 1;
	transformationMatrix(2, 2) = 1;
	transformationMatrix(3, 4) = double(TestFrameConverter::ValueProvider::get().alphaValue());

	return TestFrameConverter::testFrameConversion(FrameType::FORMAT_RGB32, FrameType::FORMAT_RGBA32, width, height, TestFrameConverter::FunctionWrapper(CV::FrameConverterRGB32::convertRGB32ToRGBA32), flag, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

}

}

}
