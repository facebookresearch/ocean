/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverterABGR32.h"

#include "ocean/cv/FrameConverterABGR32.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameConverterABGR32::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	TestResult testResult("ABGR32 converter test");

	Log::info() << " ";

	if (selector.shouldRun("ABGR32ToABGR32"))
	{
		Log::info() << "Testing ABGR32 to ABGR32 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testABGR32ToABGR32(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("ABGR32ToBGR24"))
	{
		Log::info() << "Testing ABGR32 to BGR24 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testABGR32ToBGR24(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("ABGR32ToBGRA32"))
	{
		Log::info() << "Testing ABGR32 to BGRA32 conversion with resolution " << width << "x" << height << ":";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testABGR32ToBGRA32(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("ABGR32ToRGBA32"))
	{
		Log::info() << "Testing ABGR32 to RGBA32 conversion with resolution " << width << "x" << height << ":";
		Log::info() << " ";

		for (const CV::FrameConverter::ConversionFlag flag : CV::FrameConverter::conversionFlags())
		{
			Log::info() << " ";
			testResult = testABGR32ToRGBA32(width, height, flag, testDuration, worker);
		}

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterABGR32, ABGR32ToABGR32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterABGR32::testABGR32ToABGR32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterABGR32, ABGR32ToABGR32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterABGR32::testABGR32ToABGR32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterABGR32, ABGR32ToABGR32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterABGR32::testABGR32ToABGR32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterABGR32, ABGR32ToABGR32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterABGR32::testABGR32ToABGR32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterABGR32, ABGR32ToBGR24Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterABGR32::testABGR32ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterABGR32, ABGR32ToBGR24Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterABGR32::testABGR32ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterABGR32, ABGR32ToBGR24Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterABGR32::testABGR32ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterABGR32, ABGR32ToBGR24FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterABGR32::testABGR32ToBGR24(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterABGR32, ABGR32ToBGRA32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterABGR32::testABGR32ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterABGR32, ABGR32ToBGRA32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterABGR32::testABGR32ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterABGR32, ABGR32ToBGRA32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterABGR32::testABGR32ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterABGR32, ABGR32ToBGRA32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterABGR32::testABGR32ToBGRA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameConverterABGR32, ABGR32ToRGBA32Normal)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterABGR32::testABGR32ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_NORMAL, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterABGR32, ABGR32ToRGBA32Flipped)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterABGR32::testABGR32ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterABGR32, ABGR32ToRGBA32Mirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterABGR32::testABGR32ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_MIRRORED, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameConverterABGR32, ABGR32ToRGBA32FlippedMirrored)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverterABGR32::testABGR32ToRGBA32(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterABGR32::testABGR32ToABGR32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | A |   | 1 0 0 0 |   | A |
	// | B | = | 0 1 0 0 | * | B |
	// | G |   | 0 0 1 0 |   | G |
	// | R |   | 0 0 0 1 |   | R |

	const MatrixD transformationMatrix(4, 4, true);

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_ABGR32, FrameType::FORMAT_ABGR32, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterABGR32::convertABGR32ToABGR32), flag, FrameConverterTestUtilities::functionGenericPixel, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterABGR32::testABGR32ToBGR24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | B |   | 0 1 0 0 |   | A |
	// | G | = | 0 0 1 0 | * | B |
	// | R |   | 0 0 0 1 |   | G |
	//                       | R |

	MatrixD transformationMatrix(3, 4, false);
	transformationMatrix(0, 1) = 1.0;
	transformationMatrix(1, 2) = 1.0;
	transformationMatrix(2, 3) = 1.0;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_ABGR32, FrameType::FORMAT_BGR24, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterABGR32::convertABGR32ToBGR24), flag, FrameConverterTestUtilities::functionGenericPixel, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterABGR32::testABGR32ToBGRA32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | B |   | 0 1 0 0 |   | A |
	// | G | = | 0 0 1 0 | * | B |
	// | R |   | 0 0 0 1 |   | G |
	// | A |   | 1 0 0 0 |   | R |

	MatrixD transformationMatrix(4, 4, false);
	transformationMatrix(0, 1) = 1.0;
	transformationMatrix(1, 2) = 1.0;
	transformationMatrix(2, 3) = 1.0;
	transformationMatrix(3, 0) = 1.0;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_ABGR32, FrameType::FORMAT_BGRA32, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterABGR32::convertABGR32ToBGRA32), flag, FrameConverterTestUtilities::functionGenericPixel, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

bool TestFrameConverterABGR32::testABGR32ToRGBA32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width != 0u && height != 0u);

	// | R |   | 0 0 0 1 |   | A |
	// | G | = | 0 0 1 0 | * | B |
	// | B |   | 0 1 0 0 |   | G |
	// | A |   | 1 0 0 0 |   | R |

	MatrixD transformationMatrix(4, 4, false);
	transformationMatrix(0, 3) = 1.0;
	transformationMatrix(1, 2) = 1.0;
	transformationMatrix(2, 1) = 1.0;
	transformationMatrix(3, 0) = 1.0;

	return FrameConverterTestUtilities::testFrameConversion(FrameType::FORMAT_ABGR32, FrameType::FORMAT_RGBA32, width, height, FrameConverterTestUtilities::FunctionWrapper(CV::FrameConverterABGR32::convertABGR32ToRGBA32), flag, FrameConverterTestUtilities::functionGenericPixel, FrameConverterTestUtilities::functionGenericPixel, transformationMatrix, 0.0, 255.0, testDuration, worker);
}

}

}

}
