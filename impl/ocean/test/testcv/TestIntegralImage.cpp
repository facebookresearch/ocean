/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestIntegralImage.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"
#include "ocean/test/Validation.h"
#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestIntegralImage::test(const unsigned int width, const unsigned int height, const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Integral image test");

	if (selector.shouldRun("integralimage"))
	{
		testResult = testIntegralImage(width, height, testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("linedintegralimage"))
	{
		testResult = testLinedIntegralImage(width, height, testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("linedintegralimagecomfort"))
	{
		testResult = testLinedIntegralImageComfort(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("linedintegralimagesquared"))
	{
		testResult = testLinedIntegralImageSquared(width, height, testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("linedintegralimagesandsquaredjoined"))
	{
		testResult = testLinedIntegralImageAndSquaredJoined(width, height, testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("linedintegralimagesandsquaredseparate"))
	{
		testResult = testLinedIntegralImageAndSquaredSeparate(width, height, testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("borderedintegralimage"))
	{
		testResult = testBorderedIntegralImage(width, height, testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("borderedintegralimagecomfort"))
	{
		testResult = testBorderedIntegralImageComfort(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("borderedintegralimagesquared"))
	{
		testResult = testBorderedIntegralImageSquared(width, height, testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("borderedintegralimagemirror"))
	{
		testResult = testBorderedIntegralImageMirror(width, height, testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("borderedintegralimagesquaredmirror"))
	{
		testResult = testBorderedIntegralImageSquaredMirror(width, height, testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("variancecalculation"))
	{
		testResult = testVarianceCalculation(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("variancecalculationtworegions"))
	{
		testResult = testVarianceCalculationTwoRegions(testDuration);
	}

	Log::info() << " ";

	Log::info() << testResult;

	return testResult.succeeded();

}

#ifdef OCEAN_USE_GTEST

// testIntegralImage()

TEST(TestIntegralImage, IntegralImage_uint8_uint32_1Channel)
{
	EXPECT_TRUE((TestIntegralImage::testIntegralImage<uint8_t, uint32_t, 1u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, IntegralImage_uint8_uint32_2Channels)
{
	EXPECT_TRUE((TestIntegralImage::testIntegralImage<uint8_t, uint32_t, 2u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, IntegralImage_uint8_uint32_3Channels)
{
	EXPECT_TRUE((TestIntegralImage::testIntegralImage<uint8_t, uint32_t, 3u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, IntegralImage_uint8_uint32_4Channels)
{
	EXPECT_TRUE((TestIntegralImage::testIntegralImage<uint8_t, uint32_t, 4u>(1920u, 1080u, GTEST_TEST_DURATION)));
}


TEST(TestIntegralImage, IntegralImage_int16_int64_1Channel)
{
	EXPECT_TRUE((TestIntegralImage::testIntegralImage<int16_t, int64_t, 1u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, IntegralImage_int16_int64_2Channels)
{
	EXPECT_TRUE((TestIntegralImage::testIntegralImage<int16_t, int64_t, 2u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, IntegralImage_int16_int64_3Channels)
{
	EXPECT_TRUE((TestIntegralImage::testIntegralImage<int16_t, int64_t, 3u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, IntegralImage_int16_int64_4Channels)
{
	EXPECT_TRUE((TestIntegralImage::testIntegralImage<int16_t, int64_t, 4u>(1920u, 1080u, GTEST_TEST_DURATION)));
}


// testLinedIntegralImage()

TEST(TestIntegralImage, LinedIntegralImage_uint8_uint32_1Channel)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImage<uint8_t, uint32_t, 1u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, LinedIntegralImage_uint8_uint32_2Channels)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImage<uint8_t, uint32_t, 2u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, LinedIntegralImage_uint8_uint32_3Channels)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImage<uint8_t, uint32_t, 3u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, LinedIntegralImage_uint8_uint32_4Channels)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImage<uint8_t, uint32_t, 4u>(1920u, 1080u, GTEST_TEST_DURATION)));
}


TEST(TestIntegralImage, LinedIntegralImage_int16_int64_1Channel)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImage<int16_t, int64_t, 1u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, LinedIntegralImage_int16_int64_2Channels)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImage<int16_t, int64_t, 2u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, LinedIntegralImage_int16_int64_3Channels)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImage<int16_t, int64_t, 3u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, LinedIntegralImage_int16_int64_4Channels)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImage<int16_t, int64_t, 4u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, LinedIntegralImageComfort)
{
	EXPECT_TRUE(TestIntegralImage::testLinedIntegralImageComfort(GTEST_TEST_DURATION));
}



// testLinedIntegralImageSquared()

TEST(TestIntegralImage, LinedIntegralImageSquared_uint8_uint64_1Channel)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImageSquared<uint8_t, uint64_t, 1u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, LinedIntegralImageSquared_uint8_uint64_2Channels)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImageSquared<uint8_t, uint64_t, 2u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, LinedIntegralImageSquared_uint8_uint64_3Channels)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImageSquared<uint8_t, uint64_t, 3u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, LinedIntegralImageSquared_uint8_uint64_4Channels)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImageSquared<uint8_t, uint64_t, 4u>(1920u, 1080u, GTEST_TEST_DURATION)));
}


TEST(TestIntegralImage, LinedIntegralImageSquared_int16_int64_1Channel)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImageSquared<int16_t, int64_t, 1u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, LinedIntegralImageSquared_int16_int64_2Channels)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImageSquared<int16_t, int64_t, 2u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, LinedIntegralImageSquared_int16_int64_3Channels)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImageSquared<int16_t, int64_t, 3u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, LinedIntegralImageSquared_int16_int64_4Channels)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImageSquared<int16_t, int64_t, 4u>(1920u, 1080u, GTEST_TEST_DURATION)));
}


// testLinedIntegralImageAndSquaredJoined()

TEST(TestIntegralImage, LinedIntegralImageAndSquaredJoined_uint8_uint32_1Channel)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImageAndSquaredJoined<uint8_t, uint32_t, 1u>(300u, 200u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, LinedIntegralImageAndSquaredJoined_uint8_uint32_2Channels)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImageAndSquaredJoined<uint8_t, uint32_t, 2u>(300u, 200u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, LinedIntegralImageAndSquaredJoined_uint8_uint32_3Channels)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImageAndSquaredJoined<uint8_t, uint32_t, 3u>(300u, 200u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, LinedIntegralImageAndSquaredJoined_uint8_uint32_4Channels)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImageAndSquaredJoined<uint8_t, uint32_t, 4u>(300u, 200u, GTEST_TEST_DURATION)));
}


TEST(TestIntegralImage, LinedIntegralImageAndSquaredJoined_double_double_1Channel)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImageAndSquaredJoined<double, double, 1u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, LinedIntegralImageAndSquaredJoined_double_double_2Channels)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImageAndSquaredJoined<double, double, 2u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, LinedIntegralImageAndSquaredJoined_double_double_3Channels)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImageAndSquaredJoined<double, double, 3u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, LinedIntegralImageAndSquaredJoined_double_double_4Channels)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImageAndSquaredJoined<double, double, 4u>(1920u, 1080u, GTEST_TEST_DURATION)));
}


// testLinedIntegralImageAndSquaredSeparate()

TEST(TestIntegralImage, LinedIntegralImageAndSquaredSeparate_uint8_uint32_uint64_1Channel)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImageAndSquaredSeparate<uint8_t, uint32_t, uint64_t, 1u>(300u, 200u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, LinedIntegralImageAndSquaredSeparate_uint8_uint32_uint64_2Channels)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImageAndSquaredSeparate<uint8_t, uint32_t, uint64_t, 2u>(300u, 200u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, LinedIntegralImageAndSquaredSeparate_uint8_uint32_uint64_3Channels)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImageAndSquaredSeparate<uint8_t, uint32_t, uint64_t, 3u>(300u, 200u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, LinedIntegralImageAndSquaredSeparate_uint8_uint32_uint64_4Channels)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImageAndSquaredSeparate<uint8_t, uint32_t, uint64_t, 4u>(300u, 200u, GTEST_TEST_DURATION)));
}


TEST(TestIntegralImage, LinedIntegralImageAndSquaredSeparate_double_double_double_1Channel)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImageAndSquaredSeparate<double, double, double, 1u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, LinedIntegralImageAndSquaredSeparate_double_double_double_2Channels)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImageAndSquaredSeparate<double, double, double, 2u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, LinedIntegralImageAndSquaredSeparate_double_double_double_3Channels)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImageAndSquaredSeparate<double, double, double, 3u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, LinedIntegralImageAndSquaredSeparate_double_double_double_4Channels)
{
	EXPECT_TRUE((TestIntegralImage::testLinedIntegralImageAndSquaredSeparate<double, double, double, 4u>(1920u, 1080u, GTEST_TEST_DURATION)));
}


// testBorderedIntegralImage()

TEST(TestIntegralImage, BorderedIntegralImage_uint8_uint32_1Channel)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImage<uint8_t, uint32_t, 1u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, BorderedIntegralImage_uint8_uint32_2Channels)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImage<uint8_t, uint32_t, 2u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, BorderedIntegralImage_uint8_uint32_3Channels)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImage<uint8_t, uint32_t, 3u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, BorderedIntegralImage_uint8_uint32_4Channels)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImage<uint8_t, uint32_t, 4u>(1920u, 1080u, GTEST_TEST_DURATION)));
}


TEST(TestIntegralImage, BorderedIntegralImage_int16_int64_1Channel)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImage<int16_t, int64_t, 1u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, BorderedIntegralImage_int16_int64_2Channels)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImage<int16_t, int64_t, 2u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, BorderedIntegralImage_int16_int64_3Channels)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImage<int16_t, int64_t, 3u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, BorderedIntegralImage_int16_int64_4Channels)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImage<int16_t, int64_t, 4u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, BorderedIntegralImageComfort)
{
	EXPECT_TRUE(TestIntegralImage::testBorderedIntegralImageComfort(GTEST_TEST_DURATION));
}


// testBorderedIntegralImageSquared()

TEST(TestIntegralImage, BorderedIntegralImageSquared_uint8_uint64_1Channel)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImageSquared<uint8_t, uint64_t, 1u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, BorderedIntegralImageSquared_uint8_uint64_2Channels)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImageSquared<uint8_t, uint64_t, 2u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, BorderedIntegralImageSquared_uint8_uint64_3Channels)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImageSquared<uint8_t, uint64_t, 3u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, BorderedIntegralImageSquared_uint8_uint64_4Channels)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImageSquared<uint8_t, uint64_t, 4u>(1920u, 1080u, GTEST_TEST_DURATION)));
}


TEST(TestIntegralImage, BorderedIntegralImageSquared_int8_uint64_1Channel)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImageSquared<int8_t, uint64_t, 1u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, BorderedIntegralImageSquared_int8_uint64_2Channels)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImageSquared<int8_t, uint64_t, 2u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, BorderedIntegralImageSquared_int8_uint64_3Channels)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImageSquared<int8_t, uint64_t, 3u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, BorderedIntegralImageSquared_int8_uint64_4Channels)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImageSquared<int8_t, uint64_t, 4u>(1920u, 1080u, GTEST_TEST_DURATION)));
}


// testBorderedIntegralImageMirror()

TEST(TestIntegralImage, BorderedIntegralImageMirror_uint8_uint64_1Channel)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImageMirror<uint8_t, uint64_t, 1u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, BorderedIntegralImageMirror_uint8_uint64_2Channels)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImageMirror<uint8_t, uint64_t, 2u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, BorderedIntegralImageMirror_uint8_uint64_3Channels)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImageMirror<uint8_t, uint64_t, 3u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, BorderedIntegralImageMirror_uint8_uint64_4Channels)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImageMirror<uint8_t, uint64_t, 4u>(1920u, 1080u, GTEST_TEST_DURATION)));
}


TEST(TestIntegralImage, BorderedIntegralImageMirror_int8_uint64_1Channel)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImageMirror<int8_t, int64_t, 1u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, BorderedIntegralImageMirror_int8_uint64_2Channels)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImageMirror<int8_t, int64_t, 2u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, BorderedIntegralImageMirror_int8_uint64_3Channels)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImageMirror<int8_t, int64_t, 3u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, BorderedIntegralImageMirror_int8_uint64_4Channels)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImageMirror<int8_t, int64_t, 4u>(1920u, 1080u, GTEST_TEST_DURATION)));
}


// testBorderedIntegralImageSquaredMirror()

TEST(TestIntegralImage, BorderedIntegralImageSquaredMirror_uint8_uint64_1Channel)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImageSquaredMirror<uint8_t, uint64_t, 1u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, BorderedIntegralImageSquaredMirror_uint8_uint64_2Channels)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImageSquaredMirror<uint8_t, uint64_t, 2u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, BorderedIntegralImageSquaredMirror_uint8_uint64_3Channels)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImageSquaredMirror<uint8_t, uint64_t, 3u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, BorderedIntegralImageSquaredMirror_uint8_uint64_4Channels)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImageSquaredMirror<uint8_t, uint64_t, 4u>(1920u, 1080u, GTEST_TEST_DURATION)));
}


TEST(TestIntegralImage, BorderedIntegralImageSquaredMirror_int8_uint64_1Channel)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImageSquaredMirror<int8_t, uint64_t, 1u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, BorderedIntegralImageSquaredMirror_int8_uint64_2Channels)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImageSquaredMirror<int8_t, uint64_t, 2u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, BorderedIntegralImageSquaredMirror_int8_uint64_3Channels)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImageSquaredMirror<int8_t, uint64_t, 3u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, BorderedIntegralImageSquaredMirror_int8_uint64_4Channels)
{
	EXPECT_TRUE((TestIntegralImage::testBorderedIntegralImageSquaredMirror<int8_t, uint64_t, 4u>(1920u, 1080u, GTEST_TEST_DURATION)));
}


// variance calculation

TEST(TestIntegralImage, VarianceCalculation_uint8_uint32_uint64_float)
{
	EXPECT_TRUE((TestIntegralImage::testVarianceCalculation<uint8_t, uint32_t, uint64_t, float>(GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, VarianceCalculation_int8_int32_uint64_float)
{
	EXPECT_TRUE((TestIntegralImage::testVarianceCalculation<int8_t, int32_t, uint64_t, float>(GTEST_TEST_DURATION)));
}


TEST(TestIntegralImage, VarianceCalculation_uint8_uint32_uint64_double)
{
	EXPECT_TRUE((TestIntegralImage::testVarianceCalculation<uint8_t, uint32_t, uint64_t, double>(GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, VarianceCalculation_int8_int32_uint64_double)
{
	EXPECT_TRUE((TestIntegralImage::testVarianceCalculation<int8_t, int32_t, uint64_t, double>(GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, VarianceCalculation_float_float_float_float)
{
	EXPECT_TRUE((TestIntegralImage::testVarianceCalculation<float, float, float, float>(GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, VarianceCalculation_double_double_double_double)
{
	EXPECT_TRUE((TestIntegralImage::testVarianceCalculation<double, double, double, double>(GTEST_TEST_DURATION)));
}


// variance calculation two regions

TEST(TestIntegralImage, VarianceCalculationTwoRegions_uint8_uint32_uint64_float)
{
	EXPECT_TRUE((TestIntegralImage::testVarianceCalculationTwoRegions<uint8_t, uint32_t, uint64_t, float>(GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, VarianceCalculationTwoRegions_int8_int32_uint64_float)
{
	EXPECT_TRUE((TestIntegralImage::testVarianceCalculationTwoRegions<int8_t, int32_t, uint64_t, float>(GTEST_TEST_DURATION)));
}


TEST(TestIntegralImage, VarianceCalculationTwoRegions_uint8_uint32_uint64_double)
{
	EXPECT_TRUE((TestIntegralImage::testVarianceCalculationTwoRegions<uint8_t, uint32_t, uint64_t, double>(GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, VarianceCalculationTwoRegions_int8_int32_uint64_double)
{
	EXPECT_TRUE((TestIntegralImage::testVarianceCalculationTwoRegions<int8_t, int32_t, uint64_t, double>(GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, VarianceCalculationTwoRegions_float_float_float_float)
{
	EXPECT_TRUE((TestIntegralImage::testVarianceCalculationTwoRegions<float, float, float, float>(GTEST_TEST_DURATION)));
}

TEST(TestIntegralImage, VarianceCalculationTwoRegions_double_double_double_double)
{
	EXPECT_TRUE((TestIntegralImage::testVarianceCalculationTwoRegions<double, double, double, double>(GTEST_TEST_DURATION)));
}

#endif // OCEAN_USE_GTEST

bool TestIntegralImage::testIntegralImage(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(width >= 1u && height >= 1u && testDuration > 0.0);

	Log::info() << "Testing integral image " << width << "x" << height << " without border:";
	Log::info() << " ";

	TestResult testResult;

	testResult = testIntegralImage<uint8_t, uint32_t, 1u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testIntegralImage<uint8_t, uint32_t, 2u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testIntegralImage<uint8_t, uint32_t, 3u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testIntegralImage<uint8_t, uint32_t, 4u>(width, height, testDuration);

	Log::info() << " ";
	Log::info() << " ";

	testResult = testIntegralImage<int16_t, int64_t, 1u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testIntegralImage<int16_t, int64_t, 2u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testIntegralImage<int16_t, int64_t, 3u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testIntegralImage<int16_t, int64_t, 4u>(width, height, testDuration);

	return testResult.succeeded();
}

template <typename T, typename TIntegral, unsigned int tChannels>
bool TestIntegralImage::testIntegralImage(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	ocean_assert(width >= 1u && height >= 1u);

	Log::info() << "... for " << tChannels << " channels with '" << TypeNamer::name<T>() << "' elements:";

	const FrameType::PixelFormat sourcePixelFormat = FrameType::genericPixelFormat<T, tChannels>();
	const FrameType::PixelFormat integralPixelFormat = FrameType::genericPixelFormat<TIntegral, tChannels>();

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool benchmark : {true, false})
		{
			const unsigned int testWidth = benchmark ? width : RandomI::random(randomGenerator, 1u, width);
			const unsigned int testHeight = benchmark ? height : RandomI::random(randomGenerator, 1u, height);

			const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, sourcePixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
			Frame integralFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceFrame, integralPixelFormat), &randomGenerator);

			const Frame copyIntegralFrame(integralFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			performance.startIf(benchmark);
				CV::IntegralImage::createImage<T, TIntegral, tChannels>(sourceFrame.constdata<T>(), integralFrame.data<TIntegral>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), integralFrame.paddingElements());
			performance.stopIf(benchmark);

			if (!CV::CVUtilities::isPaddingMemoryIdentical(integralFrame, copyIntegralFrame))
			{
				ocean_assert(false && "Invalid padding elements!");
				OCEAN_SET_FAILED(validation);
				break;
			}

			OCEAN_EXPECT_TRUE(validation, validateIntegralImage<T, TIntegral>(sourceFrame.constdata<T>(), integralFrame.constdata<TIntegral>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.channels(), sourceFrame.paddingElements(), integralFrame.paddingElements(), 20u));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance: " << performance;

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestIntegralImage::testLinedIntegralImage(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(width >= 1u && height >= 1u && testDuration > 0.0);

	Log::info() << "Testing lined integral image " << width << "x" << height << " image:";
	Log::info() << " ";

	TestResult testResult;

	testResult = testLinedIntegralImage<uint8_t, uint32_t, 1u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImage<uint8_t, uint32_t, 2u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImage<uint8_t, uint32_t, 3u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImage<uint8_t, uint32_t, 4u>(width, height, testDuration);

	Log::info() << " ";
	Log::info() << " ";

	testResult = testLinedIntegralImage<int16_t, int64_t, 1u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImage<int16_t, int64_t, 2u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImage<int16_t, int64_t, 3u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImage<int16_t, int64_t, 4u>(width, height, testDuration);

	return testResult.succeeded();
}

bool TestIntegralImage::testLinedIntegralImageComfort(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing lined integral image comfort function:";
	Log::info() << " ";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		// uint8_t -> uint32_t
		for (unsigned int channels = 1u; channels <= 4u; ++channels)
		{
			const unsigned int width = RandomI::random(randomGenerator, 1u, 1024u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 1024u);

			const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			const Frame integralFrame = CV::IntegralImage::Comfort::createLinedImage(frame);

			if (integralFrame.isValid() && integralFrame.isPixelFormatCompatible(FrameType::genericPixelFormat<uint32_t>(channels)))
			{
				constexpr unsigned int border = 0u;
				OCEAN_EXPECT_TRUE(validation, validateBorderedIntegralImage<uint8_t, uint32_t>(frame.constdata<uint8_t>(), integralFrame.constdata<uint32_t>(), frame.width(), frame.height(), channels, border, frame.paddingElements(), integralFrame.paddingElements()));
			}
			else
			{
				OCEAN_SET_FAILED(validation);
			}
		}

		// int8_t -> int32_t
		for (unsigned int channels = 1u; channels <= 4u; ++channels)
		{
			const unsigned int width = RandomI::random(randomGenerator, 1u, 1024u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 1024u);

			const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<int8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			const Frame integralFrame = CV::IntegralImage::Comfort::createLinedImage(frame);

			if (integralFrame.isValid() && integralFrame.isPixelFormatCompatible(FrameType::genericPixelFormat<int32_t>(channels)))
			{
				constexpr unsigned int border = 0u;
				OCEAN_EXPECT_TRUE(validation, validateBorderedIntegralImage<int8_t, int32_t>(frame.constdata<int8_t>(), integralFrame.constdata<int32_t>(), frame.width(), frame.height(), channels, border, frame.paddingElements(), integralFrame.paddingElements()));
			}
			else
			{
				OCEAN_SET_FAILED(validation);
			}
		}

		// double -> double
		for (unsigned int channels = 1u; channels <= 4u; ++channels)
		{
			const unsigned int width = RandomI::random(randomGenerator, 1u, 1024u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 1024u);

			const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<double>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			const Frame integralFrame = CV::IntegralImage::Comfort::createLinedImage(frame);

			if (integralFrame.isValid() && integralFrame.isPixelFormatCompatible(FrameType::genericPixelFormat<double>(channels)))
			{
				constexpr unsigned int border = 0u;
				OCEAN_EXPECT_TRUE(validation, validateBorderedIntegralImage<double, double>(frame.constdata<double>(), integralFrame.constdata<double>(), frame.width(), frame.height(), channels, border, frame.paddingElements(), integralFrame.paddingElements()));
			}
			else
			{
				OCEAN_SET_FAILED(validation);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T, typename TIntegral, unsigned int tChannels>
bool TestIntegralImage::testLinedIntegralImage(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	ocean_assert(width >= 1u && height >= 1u);

	Log::info() << "... for " << tChannels << " channels with '" << TypeNamer::name<T>() << "' elements:";

	const FrameType::PixelFormat sourcePixelFormat = FrameType::genericPixelFormat<T, tChannels>();
	const FrameType::PixelFormat integralPixelFormat = FrameType::genericPixelFormat<TIntegral, tChannels>();

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool benchmark : {true, false})
		{
			const unsigned int testWidth = benchmark ? width : RandomI::random(randomGenerator, 1u, width);
			const unsigned int testHeight = benchmark ? height : RandomI::random(randomGenerator, 1u, height);

			const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, sourcePixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
			Frame integralFrame = CV::CVUtilities::randomizedFrame(FrameType(testWidth + 1u, testHeight + 1u, integralPixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			const Frame copyIntegralFrame(integralFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			performance.startIf(benchmark);
				CV::IntegralImage::createLinedImage<T, TIntegral, tChannels>(sourceFrame.constdata<T>(), integralFrame.data<TIntegral>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), integralFrame.paddingElements());
			performance.stopIf(benchmark);

			if (!CV::CVUtilities::isPaddingMemoryIdentical(integralFrame, copyIntegralFrame))
			{
				ocean_assert(false && "Invalid padding elements!");
				OCEAN_SET_FAILED(validation);
				break;
			}

			constexpr unsigned int border = 0u;
			OCEAN_EXPECT_TRUE(validation, (validateBorderedIntegralImage<T, TIntegral, tChannels>(sourceFrame.constdata<T>(), integralFrame.constdata<TIntegral>(), sourceFrame.width(), sourceFrame.height(), border, sourceFrame.paddingElements(), integralFrame.paddingElements())));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance: " << performance;

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestIntegralImage::testLinedIntegralImageSquared(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(width >= 1u && height >= 1u && testDuration > 0.0);

	Log::info() << "Testing lined squared integral image " << width << "x" << height << " image:";
	Log::info() << " ";

	TestResult testResult;

	testResult = testLinedIntegralImageSquared<uint8_t, uint64_t, 1u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImageSquared<uint8_t, uint64_t, 2u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImageSquared<uint8_t, uint64_t, 3u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImageSquared<uint8_t, uint64_t, 4u>(width, height, testDuration);

	Log::info() << " ";
	Log::info() << " ";

	testResult = testLinedIntegralImageSquared<int16_t, int64_t, 1u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImageSquared<int16_t, int64_t, 2u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImageSquared<int16_t, int64_t, 3u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImageSquared<int16_t, int64_t, 4u>(width, height, testDuration);

	return testResult.succeeded();
}

template <typename T, typename TIntegral, unsigned int tChannels>
bool TestIntegralImage::testLinedIntegralImageSquared(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	ocean_assert(width >= 1u && height >= 1u);

	Log::info() << "... for " << tChannels << " channels with '" << TypeNamer::name<T>() << "' elements:";

	const FrameType::PixelFormat sourcePixelFormat = FrameType::genericPixelFormat<T, tChannels>();
	const FrameType::PixelFormat integralPixelFormat = FrameType::genericPixelFormat<TIntegral, tChannels>();

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool benchmark : {true, false})
		{
			const unsigned int testWidth = benchmark ? width : RandomI::random(randomGenerator, 1u, width);
			const unsigned int testHeight = benchmark ? height : RandomI::random(randomGenerator, 1u, height);

			const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, sourcePixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
			Frame integralFrame = CV::CVUtilities::randomizedFrame(FrameType(testWidth + 1u, testHeight + 1u, integralPixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			const Frame copyIntegralFrame(integralFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			performance.startIf(benchmark);
				CV::IntegralImage::createLinedImageSquared<T, TIntegral, tChannels>(sourceFrame.constdata<T>(), integralFrame.data<TIntegral>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), integralFrame.paddingElements());
			performance.stopIf(benchmark);

			if (!CV::CVUtilities::isPaddingMemoryIdentical(integralFrame, copyIntegralFrame))
			{
				ocean_assert(false && "Invalid padding elements!");
				OCEAN_SET_FAILED(validation);
				break;
			}

			constexpr unsigned int border = 0u;
			OCEAN_EXPECT_TRUE(validation, (validateBorderedIntegralImageSquared<T, TIntegral, tChannels>(sourceFrame.constdata<T>(), integralFrame.constdata<TIntegral>(), sourceFrame.width(), sourceFrame.height(), border, sourceFrame.paddingElements(), integralFrame.paddingElements())));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance: " << performance;

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestIntegralImage::testLinedIntegralImageAndSquaredJoined(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(width >= 1u && height >= 1u && testDuration > 0.0);

	Log::info() << "Testing joined lined integral and squared integral image " << width << "x" << height << " image, and (300x200):";
	Log::info() << " ";

	TestResult testResult;

	// for uint32_t, we need to apply an image resolution <= 2^16

	testResult = testLinedIntegralImageAndSquaredJoined<uint8_t, uint32_t, 1u>(300u, 200u, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImageAndSquaredJoined<uint8_t, uint32_t, 2u>(300u, 200u, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImageAndSquaredJoined<uint8_t, uint32_t, 3u>(300u, 200u, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImageAndSquaredJoined<uint8_t, uint32_t, 4u>(300u, 200u, testDuration);

	Log::info() << " ";
	Log::info() << " ";

	testResult = testLinedIntegralImageAndSquaredJoined<uint8_t, uint64_t, 1u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImageAndSquaredJoined<uint8_t, uint64_t, 2u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImageAndSquaredJoined<uint8_t, uint64_t, 3u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImageAndSquaredJoined<uint8_t, uint64_t, 4u>(width, height, testDuration);

	Log::info() << " ";
	Log::info() << " ";

	testResult = testLinedIntegralImageAndSquaredJoined<double, double, 1u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImageAndSquaredJoined<double, double, 2u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImageAndSquaredJoined<double, double, 3u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImageAndSquaredJoined<double, double, 4u>(width, height, testDuration);

	return testResult.succeeded();
}

template <typename T, typename TIntegralAndSquared, unsigned int tChannels>
bool TestIntegralImage::testLinedIntegralImageAndSquaredJoined(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	ocean_assert(width >= 1u && height >= 1u);

	Log::info() << "... for " << tChannels << " channels with '" << TypeNamer::name<T>() << "' elements:";

	const FrameType::PixelFormat sourcePixelFormat = FrameType::genericPixelFormat<T, tChannels>();
	const FrameType::PixelFormat integralPixelFormat = FrameType::genericPixelFormat<TIntegralAndSquared, tChannels>();

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool benchmark : {true, false})
		{
			const unsigned int testWidth = benchmark ? width : RandomI::random(randomGenerator, 1u, width);
			const unsigned int testHeight = benchmark ? height : RandomI::random(randomGenerator, 1u, height);

			const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, sourcePixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
			Frame integralAndSquaredFrame = CV::CVUtilities::randomizedFrame(FrameType((testWidth + 1u) * 2u, testHeight + 1u, integralPixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			const Frame copyIntegralAndSquaredFrame(integralAndSquaredFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			performance.startIf(benchmark);
				CV::IntegralImage::createLinedImageAndSquared<T, TIntegralAndSquared, tChannels>(sourceFrame.constdata<T>(), integralAndSquaredFrame.data<TIntegralAndSquared>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), integralAndSquaredFrame.paddingElements());
			performance.stopIf(benchmark);

			if (!CV::CVUtilities::isPaddingMemoryIdentical(integralAndSquaredFrame, copyIntegralAndSquaredFrame))
			{
				ocean_assert(false && "Invalid padding elements!");
				OCEAN_SET_FAILED(validation);
				break;
			}

			constexpr unsigned int border = 0u;
			OCEAN_EXPECT_TRUE(validation, (validateBorderedIntegralImageAndSquaredJoined<T, TIntegralAndSquared, tChannels>(sourceFrame.constdata<T>(), integralAndSquaredFrame.constdata<TIntegralAndSquared>(), sourceFrame.width(), sourceFrame.height(), border, sourceFrame.paddingElements(), integralAndSquaredFrame.paddingElements())));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance: " << performance;

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestIntegralImage::testLinedIntegralImageAndSquaredSeparate(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(width >= 1u && height >= 1u && testDuration > 0.0);

	Log::info() << "Testing separate lined integral and squared integral image " << width << "x" << height << " image, and (300x200):";
	Log::info() << " ";

	TestResult testResult;

	testResult = testLinedIntegralImageAndSquaredSeparate<uint8_t, uint32_t, uint64_t, 1u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImageAndSquaredSeparate<uint8_t, uint32_t, uint64_t, 2u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImageAndSquaredSeparate<uint8_t, uint32_t, uint64_t, 3u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImageAndSquaredSeparate<uint8_t, uint32_t, uint64_t, 4u>(width, height, testDuration);

	Log::info() << " ";
	Log::info() << " ";

	testResult = testLinedIntegralImageAndSquaredSeparate<uint8_t, uint64_t, uint64_t, 1u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImageAndSquaredSeparate<uint8_t, uint64_t, uint64_t, 2u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImageAndSquaredSeparate<uint8_t, uint64_t, uint64_t, 3u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImageAndSquaredSeparate<uint8_t, uint64_t, uint64_t, 4u>(width, height, testDuration);

	Log::info() << " ";
	Log::info() << " ";

	testResult = testLinedIntegralImageAndSquaredSeparate<double, double, double, 1u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImageAndSquaredSeparate<double, double, double, 2u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImageAndSquaredSeparate<double, double, double, 3u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testLinedIntegralImageAndSquaredSeparate<double, double, double, 4u>(width, height, testDuration);

	return testResult.succeeded();
}

template <typename T, typename TIntegral, typename TIntegralSquared, unsigned int tChannels>
bool TestIntegralImage::testLinedIntegralImageAndSquaredSeparate(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	ocean_assert(width >= 1u && height >= 1u);

	Log::info() << "... for " << tChannels << " channels with '" << TypeNamer::name<T>() << "' elements:";

	const FrameType::PixelFormat sourcePixelFormat = FrameType::genericPixelFormat<T, tChannels>();
	const FrameType::PixelFormat integralPixelFormat = FrameType::genericPixelFormat<TIntegral, tChannels>();
	const FrameType::PixelFormat integralSquaredPixelFormat = FrameType::genericPixelFormat<TIntegralSquared, tChannels>();

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool benchmark : {true, false})
		{
			const unsigned int testWidth = benchmark ? width : RandomI::random(randomGenerator, 1u, width);
			const unsigned int testHeight = benchmark ? height : RandomI::random(randomGenerator, 1u, height);

			const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, sourcePixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
			Frame integralFrame = CV::CVUtilities::randomizedFrame(FrameType(testWidth + 1u, testHeight + 1u, integralPixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
			Frame integralSquaredFrame = CV::CVUtilities::randomizedFrame(FrameType(testWidth + 1u, testHeight + 1u, integralSquaredPixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			const Frame copyIntegralFrame(integralFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
			const Frame copyIntegralSquaredFrame(integralSquaredFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			performance.startIf(benchmark);
				CV::IntegralImage::createLinedImageAndSquared<T, TIntegral, TIntegralSquared, tChannels>(sourceFrame.constdata<T>(), integralFrame.data<TIntegral>(), integralSquaredFrame.data<TIntegralSquared>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), integralFrame.paddingElements(), integralSquaredFrame.paddingElements());
			performance.stopIf(benchmark);

			if (!CV::CVUtilities::isPaddingMemoryIdentical(integralFrame, copyIntegralFrame))
			{
				ocean_assert(false && "Invalid padding elements!");
				OCEAN_SET_FAILED(validation);
				break;
			}

			if (!CV::CVUtilities::isPaddingMemoryIdentical(integralSquaredFrame, copyIntegralSquaredFrame))
			{
				ocean_assert(false && "Invalid padding elements!");
				OCEAN_SET_FAILED(validation);
				break;
			}

			constexpr unsigned int border = 0u;
			OCEAN_EXPECT_TRUE(validation, (validateBorderedIntegralImageAndSquaredSeparate<T, TIntegral, TIntegralSquared, tChannels>(sourceFrame.constdata<T>(), integralFrame.constdata<TIntegral>(), integralSquaredFrame.constdata<TIntegralSquared>(), sourceFrame.width(), sourceFrame.height(), border, sourceFrame.paddingElements(), integralFrame.paddingElements(), integralSquaredFrame.paddingElements())));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance: " << performance;

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestIntegralImage::testBorderedIntegralImage(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(width >= 1u && height >= 1u && testDuration > 0.0);

	Log::info() << "Testing bordered integral image " << width << "x" << height << " image:";
	Log::info() << " ";

	TestResult testResult;

	testResult = testBorderedIntegralImage<uint8_t, uint32_t, 1u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testBorderedIntegralImage<uint8_t, uint32_t, 2u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testBorderedIntegralImage<uint8_t, uint32_t, 3u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testBorderedIntegralImage<uint8_t, uint32_t, 4u>(width, height, testDuration);

	Log::info() << " ";
	Log::info() << " ";

	testResult = testBorderedIntegralImage<int16_t, int64_t, 1u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testBorderedIntegralImage<int16_t, int64_t, 2u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testBorderedIntegralImage<int16_t, int64_t, 3u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testBorderedIntegralImage<int16_t, int64_t, 4u>(width, height, testDuration);

	return testResult.succeeded();
}

bool TestIntegralImage::testBorderedIntegralImageComfort(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing bordered integral image comfort function:";
	Log::info() << " ";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		// uint8_t -> uint32_t
		for (unsigned int channels = 1u; channels <= 4u; ++channels)
		{
			const unsigned int width = RandomI::random(randomGenerator, 1u, 1024u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 1024u);

			Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			const unsigned int border = RandomI::random(randomGenerator, 1u, 100u);

			const Frame integralFrame = CV::IntegralImage::Comfort::createBorderedImage(frame, border);

			if (integralFrame.isValid() && integralFrame.isPixelFormatCompatible(FrameType::genericPixelFormat<uint32_t>(channels)))
			{
				OCEAN_EXPECT_TRUE(validation, validateBorderedIntegralImage<uint8_t, uint32_t>(frame.constdata<uint8_t>(), integralFrame.constdata<uint32_t>(), frame.width(), frame.height(), channels, border, frame.paddingElements(), integralFrame.paddingElements()));
			}
			else
			{
				OCEAN_SET_FAILED(validation);
			}
		}

		// int8_t -> int32_t
		for (unsigned int channels = 1u; channels <= 4u; ++channels)
		{
			const unsigned int width = RandomI::random(randomGenerator, 1u, 1024u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 1024u);

			const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<int8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			const unsigned int border = RandomI::random(randomGenerator, 1u, 100u);

			const Frame integralFrame = CV::IntegralImage::Comfort::createBorderedImage(frame, border);

			if (integralFrame.isValid() && integralFrame.isPixelFormatCompatible(FrameType::genericPixelFormat<int32_t>(channels)))
			{
				OCEAN_EXPECT_TRUE(validation, validateBorderedIntegralImage<int8_t, int32_t>(frame.constdata<int8_t>(), integralFrame.constdata<int32_t>(), frame.width(), frame.height(), channels, border, frame.paddingElements(), integralFrame.paddingElements()));
			}
			else
			{
				OCEAN_SET_FAILED(validation);
			}
		}

		// double -> double
		for (unsigned int channels = 1u; channels <= 4u; ++channels)
		{
			const unsigned int width = RandomI::random(randomGenerator, 1u, 1024u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 1024u);

			const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<double>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			const unsigned int border = RandomI::random(randomGenerator, 1u, 100u);

			const Frame integralFrame = CV::IntegralImage::Comfort::createBorderedImage(frame, border);

			if (integralFrame.isValid() && integralFrame.isPixelFormatCompatible(FrameType::genericPixelFormat<double>(channels)))
			{
				OCEAN_EXPECT_TRUE(validation, validateBorderedIntegralImage<double, double>(frame.constdata<double>(), integralFrame.constdata<double>(), frame.width(), frame.height(), channels, border, frame.paddingElements(), integralFrame.paddingElements()));
			}
			else
			{
				OCEAN_SET_FAILED(validation);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T, typename TIntegral, unsigned int tChannels>
bool TestIntegralImage::testBorderedIntegralImage(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	ocean_assert(width >= 1u && height >= 1u);

	Log::info() << "... for " << tChannels << " channels with '" << TypeNamer::name<T>() << "' elements:";

	const FrameType::PixelFormat sourcePixelFormat = FrameType::genericPixelFormat<T, tChannels>();
	const FrameType::PixelFormat integralPixelFormat = FrameType::genericPixelFormat<TIntegral, tChannels>();

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool benchmark : {true, false})
		{
			const unsigned int testWidth = benchmark ? width : RandomI::random(randomGenerator, 1u, width);
			const unsigned int testHeight = benchmark ? height : RandomI::random(randomGenerator, 1u, height);
			const unsigned int testBorder = benchmark ? 10u : RandomI::random(randomGenerator, 1u, 50u);

			const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, sourcePixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
			Frame integralFrame = CV::CVUtilities::randomizedFrame(FrameType(testWidth + testBorder * 2u + 1u, testHeight + testBorder * 2u + 1u, integralPixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			const Frame copyIntegralFrame(integralFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			performance.startIf(benchmark);
				CV::IntegralImage::createBorderedImage<T, TIntegral, tChannels>(sourceFrame.constdata<T>(), integralFrame.data<TIntegral>(), sourceFrame.width(), sourceFrame.height(), testBorder, sourceFrame.paddingElements(), integralFrame.paddingElements());
			performance.stopIf(benchmark);

			if (!CV::CVUtilities::isPaddingMemoryIdentical(integralFrame, copyIntegralFrame))
			{
				ocean_assert(false && "Invalid padding elements!");
				OCEAN_SET_FAILED(validation);
				break;
			}

			OCEAN_EXPECT_TRUE(validation, (validateBorderedIntegralImage<T, TIntegral, tChannels>(sourceFrame.constdata<T>(), integralFrame.constdata<TIntegral>(), sourceFrame.width(), sourceFrame.height(), testBorder, sourceFrame.paddingElements(), integralFrame.paddingElements())));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance: " << performance;

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestIntegralImage::testBorderedIntegralImageSquared(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(width >= 1u && height >= 1u && testDuration > 0.0);

	Log::info() << "Testing bordered squared integral image " << width << "x" << height << " image:";
	Log::info() << " ";

	TestResult testResult;

	testResult = testBorderedIntegralImageSquared<uint8_t, uint64_t, 1u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testBorderedIntegralImageSquared<uint8_t, uint64_t, 2u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testBorderedIntegralImageSquared<uint8_t, uint64_t, 3u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testBorderedIntegralImageSquared<uint8_t, uint64_t, 4u>(width, height, testDuration);

	Log::info() << " ";
	Log::info() << " ";

	testResult = testBorderedIntegralImageSquared<int8_t, uint64_t, 1u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testBorderedIntegralImageSquared<int8_t, uint64_t, 2u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testBorderedIntegralImageSquared<int8_t, uint64_t, 3u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testBorderedIntegralImageSquared<int8_t, uint64_t, 4u>(width, height, testDuration);

	return testResult.succeeded();
}

template <typename T, typename TIntegral, unsigned int tChannels>
bool TestIntegralImage::testBorderedIntegralImageSquared(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	ocean_assert(width >= 1u && height >= 1u);

	Log::info() << "... for " << tChannels << " channels with '" << TypeNamer::name<T>() << "' elements:";

	const FrameType::PixelFormat sourcePixelFormat = FrameType::genericPixelFormat<T, tChannels>();
	const FrameType::PixelFormat integralPixelFormat = FrameType::genericPixelFormat<TIntegral, tChannels>();

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool benchmark : {true, false})
		{
			const unsigned int testWidth = benchmark ? width : RandomI::random(randomGenerator, 1u, width);
			const unsigned int testHeight = benchmark ? height : RandomI::random(randomGenerator, 1u, height);
			const unsigned int testBorder = benchmark ? 10u : RandomI::random(randomGenerator, 1u, 50u);

			const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, sourcePixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
			Frame integralFrame = CV::CVUtilities::randomizedFrame(FrameType(testWidth + testBorder * 2u + 1u, testHeight + testBorder * 2u + 1u, integralPixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			const Frame copyIntegralFrame(integralFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			performance.startIf(benchmark);
				CV::IntegralImage::createBorderedImageSquared<T, TIntegral, tChannels>(sourceFrame.constdata<T>(), integralFrame.data<TIntegral>(), sourceFrame.width(), sourceFrame.height(), testBorder, sourceFrame.paddingElements(), integralFrame.paddingElements());
			performance.stopIf(benchmark);

			if (!CV::CVUtilities::isPaddingMemoryIdentical(integralFrame, copyIntegralFrame))
			{
				ocean_assert(false && "Invalid padding elements!");
				OCEAN_SET_FAILED(validation);
				break;
			}

			OCEAN_EXPECT_TRUE(validation, (validateBorderedIntegralImageSquared<T, TIntegral, tChannels>(sourceFrame.constdata<T>(), integralFrame.constdata<TIntegral>(), sourceFrame.width(), sourceFrame.height(), testBorder, sourceFrame.paddingElements(), integralFrame.paddingElements())));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance: " << performance;

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestIntegralImage::testBorderedIntegralImageMirror(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(width >= 1u && height >= 1u && testDuration > 0.0);

	Log::info() << "Testing bordered mirrored integral image " << width << "x" << height << " image:";
	Log::info() << " ";

	TestResult testResult;

	testResult = testBorderedIntegralImageMirror<uint8_t, uint64_t, 1u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testBorderedIntegralImageMirror<uint8_t, uint64_t, 2u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testBorderedIntegralImageMirror<uint8_t, uint64_t, 3u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testBorderedIntegralImageMirror<uint8_t, uint64_t, 4u>(width, height, testDuration);

	Log::info() << " ";
	Log::info() << " ";

	testResult = testBorderedIntegralImageMirror<int8_t, int64_t, 1u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testBorderedIntegralImageMirror<int8_t, int64_t, 2u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testBorderedIntegralImageMirror<int8_t, int64_t, 3u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testBorderedIntegralImageMirror<int8_t, int64_t, 4u>(width, height, testDuration);

	return testResult.succeeded();
}

template <typename T, typename TIntegral, unsigned int tChannels>
bool TestIntegralImage::testBorderedIntegralImageMirror(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	ocean_assert(width >= 1u && height >= 1u);

	Log::info() << "... for " << tChannels << " channels with '" << TypeNamer::name<T>() << "' elements:";

	const FrameType::PixelFormat sourcePixelFormat = FrameType::genericPixelFormat<T, tChannels>();
	const FrameType::PixelFormat integralPixelFormat = FrameType::genericPixelFormat<TIntegral, tChannels>();

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool benchmark : {true, false})
		{
			const unsigned int testWidth = benchmark ? width : RandomI::random(randomGenerator, 1u, width);
			const unsigned int testHeight = benchmark ? height : RandomI::random(randomGenerator, 1u, height);
			const unsigned int testBorder = benchmark ? 10u : RandomI::random(randomGenerator, 1u, std::min(50u, std::min(testWidth, testHeight)));

			const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, sourcePixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
			Frame integralFrame = CV::CVUtilities::randomizedFrame(FrameType(testWidth + testBorder * 2u + 1u, testHeight + testBorder * 2u + 1u, integralPixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			const Frame copyIntegralFrame(integralFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			performance.startIf(benchmark);
				CV::IntegralImage::createBorderedImageMirror<T, TIntegral, tChannels>(sourceFrame.constdata<T>(), integralFrame.data<TIntegral>(), sourceFrame.width(), sourceFrame.height(), testBorder, sourceFrame.paddingElements(), integralFrame.paddingElements());
			performance.stopIf(benchmark);

			if (!CV::CVUtilities::isPaddingMemoryIdentical(integralFrame, copyIntegralFrame))
			{
				ocean_assert(false && "Invalid padding elements!");
				OCEAN_SET_FAILED(validation);
				break;
			}

			OCEAN_EXPECT_TRUE(validation, (validateBorderedIntegralImageMirror<T, TIntegral, tChannels>(sourceFrame.constdata<T>(), integralFrame.constdata<TIntegral>(), sourceFrame.width(), sourceFrame.height(), testBorder, sourceFrame.paddingElements(), integralFrame.paddingElements())));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance: " << performance;

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestIntegralImage::testBorderedIntegralImageSquaredMirror(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(width >= 1u && height >= 1u && testDuration > 0.0);

	Log::info() << "Testing bordered squared mirrored integral image " << width << "x" << height << " image:";
	Log::info() << " ";

	TestResult testResult;

	testResult = testBorderedIntegralImageSquaredMirror<uint8_t, uint64_t, 1u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testBorderedIntegralImageSquaredMirror<uint8_t, uint64_t, 2u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testBorderedIntegralImageSquaredMirror<uint8_t, uint64_t, 3u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testBorderedIntegralImageSquaredMirror<uint8_t, uint64_t, 4u>(width, height, testDuration);

	Log::info() << " ";
	Log::info() << " ";

	testResult = testBorderedIntegralImageSquaredMirror<int8_t, uint64_t, 1u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testBorderedIntegralImageSquaredMirror<int8_t, uint64_t, 2u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testBorderedIntegralImageSquaredMirror<int8_t, uint64_t, 3u>(width, height, testDuration);
	Log::info() << " ";
	testResult = testBorderedIntegralImageSquaredMirror<int8_t, uint64_t, 4u>(width, height, testDuration);

	return testResult.succeeded();
}

template <typename T, typename TIntegral, unsigned int tChannels>
bool TestIntegralImage::testBorderedIntegralImageSquaredMirror(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	ocean_assert(width >= 1u && height >= 1u);

	Log::info() << "... for " << tChannels << " channels with '" << TypeNamer::name<T>() << "' elements:";

	const FrameType::PixelFormat sourcePixelFormat = FrameType::genericPixelFormat<T, tChannels>();
	const FrameType::PixelFormat integralPixelFormat = FrameType::genericPixelFormat<TIntegral, tChannels>();

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool benchmark : {true, false})
		{
			const unsigned int testWidth = benchmark ? width : RandomI::random(randomGenerator, 1u, width);
			const unsigned int testHeight = benchmark ? height : RandomI::random(randomGenerator, 1u, height);
			const unsigned int testBorder = benchmark ? 10u : RandomI::random(randomGenerator, 1u, std::min(50u, std::min(testWidth, testHeight)));

			const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, sourcePixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
			Frame integralFrame = CV::CVUtilities::randomizedFrame(FrameType(testWidth + testBorder * 2u + 1u, testHeight + testBorder * 2u + 1u, integralPixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			const Frame copyIntegralFrame(integralFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			performance.startIf(benchmark);
				CV::IntegralImage::createBorderedImageSquaredMirror<T, TIntegral, tChannels>(sourceFrame.constdata<T>(), integralFrame.data<TIntegral>(), sourceFrame.width(), sourceFrame.height(), testBorder, sourceFrame.paddingElements(), integralFrame.paddingElements());
			performance.stopIf(benchmark);

			if (!CV::CVUtilities::isPaddingMemoryIdentical(integralFrame, copyIntegralFrame))
			{
				ocean_assert(false && "Invalid padding elements!");
				OCEAN_SET_FAILED(validation);
				break;
			}

			OCEAN_EXPECT_TRUE(validation, (validateBorderedIntegralImageSquaredMirror<T, TIntegral, tChannels>(sourceFrame.constdata<T>(), integralFrame.constdata<TIntegral>(), sourceFrame.width(), sourceFrame.height(), testBorder, sourceFrame.paddingElements(), integralFrame.paddingElements())));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance: " << performance;

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestIntegralImage::testVarianceCalculation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing calculation of variance:";
	Log::info() << " ";

	TestResult testResult;

	testResult = testVarianceCalculation<uint8_t, uint32_t, uint64_t, float>(testDuration);
	Log::info() << " ";
	testResult = testVarianceCalculation<int8_t, int32_t, uint64_t, float>(testDuration);
	Log::info() << " ";

	Log::info() << " ";
	Log::info() << " ";

	testResult = testVarianceCalculation<uint8_t, uint32_t, uint64_t, double>(testDuration);
	Log::info() << " ";
	testResult = testVarianceCalculation<int8_t, int32_t, uint64_t, double>(testDuration);

	Log::info() << " ";
	Log::info() << " ";

	testResult = testVarianceCalculation<float, float, float, float>(testDuration);
	Log::info() << " ";
	testResult = testVarianceCalculation<double, double, double, double>(testDuration);

	return testResult.succeeded();
}

template <typename T, typename TIntegral, typename TIntegralSquared, typename TVariance>
bool TestIntegralImage::testVarianceCalculation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "for data types " <<  TypeNamer::name<T>() << ", " << TypeNamer::name<TIntegral>() << ", " << TypeNamer::name<TIntegralSquared>() << ", " << TypeNamer::name<TVariance>() << ":";

	constexpr double threshold = std::is_same<T, float>::value ? 0.95 : 0.985;

	RandomGenerator randomGenerator;
	ValidationPrecision validation(threshold, randomGenerator);

	const unsigned int frameWidth = RandomI::random(randomGenerator, 1u, 1920u);
	const unsigned int frameHeight = RandomI::random(randomGenerator, 1u, 1080u);

	const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(frameWidth, frameHeight, FrameType::genericPixelFormat<T, 1u>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
	Frame linedIntegralFrame = CV::CVUtilities::randomizedFrame(FrameType(frameWidth + 1u, frameHeight + 1u, FrameType::genericPixelFormat<TIntegral, 1u>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
	Frame linedIntegralSquaredFrame = CV::CVUtilities::randomizedFrame(FrameType(frameWidth + 1u, frameHeight + 1u, FrameType::genericPixelFormat<TIntegralSquared, 1u>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

	CV::IntegralImage::createLinedImage<T, TIntegral, 1u>(frame.constdata<T>(), linedIntegralFrame.data<TIntegral>(), frameWidth, frameHeight, frame.paddingElements(), linedIntegralFrame.paddingElements());
	CV::IntegralImage::createLinedImageSquared<T, TIntegralSquared, 1u>(frame.constdata<T>(), linedIntegralSquaredFrame.data<TIntegralSquared>(), frameWidth, frameHeight, frame.paddingElements(), linedIntegralSquaredFrame.paddingElements());

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		const unsigned int windowLeft = RandomI::random(randomGenerator, 0u, frameWidth - 1u);
		const unsigned int windowTop = RandomI::random(randomGenerator, 0u, frameHeight - 1u);
		const unsigned int windowWidth = RandomI::random(randomGenerator, 1u, frameWidth - windowLeft);
		const unsigned int windowHeight = RandomI::random(randomGenerator, 1u, frameHeight - windowTop);

		const TVariance variance = CV::IntegralImage::linedIntegralVariance<TIntegral, TIntegralSquared, TVariance>(linedIntegralFrame.constdata<TIntegral>(), linedIntegralSquaredFrame.constdata<TIntegralSquared>(), linedIntegralFrame.strideElements(), linedIntegralSquaredFrame.strideElements(), windowLeft, windowTop, windowWidth, windowHeight);

		// determine mean value

		double sum = 0.0;

		for (unsigned int y = windowTop; y < windowTop + windowHeight; ++y)
		{
			for (unsigned int x = windowLeft; x < windowLeft + windowWidth; ++x)
			{
				sum += double(frame.constpixel<T>(x, y)[0]);
			}
		}

		ocean_assert(windowWidth * windowHeight != 0u);
		const double mean = sum / double(windowWidth * windowHeight);

		double sumSquareDifferences = 0.0;

		for (unsigned int y = windowTop; y < windowTop + windowHeight; ++y)
		{
			for (unsigned int x = windowLeft; x < windowLeft + windowWidth; ++x)
			{
				sumSquareDifferences += NumericD::sqr(double(frame.constpixel<T>(x, y)[0]) - mean);
			}
		}

		const double testVariance = sumSquareDifferences / double(windowWidth * windowHeight);
		const double testDeviation = NumericD::sqrt(testVariance);

		const double deviation = NumericD::sqrt(double(variance));

		if (!NumericD::isEqual(deviation, testDeviation, testDeviation * 0.01))
		{
			scopedIteration.setInaccurate();
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestIntegralImage::testVarianceCalculationTwoRegions(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing calculation of variance for two regions:";
	Log::info() << " ";

	TestResult testResult;

	testResult = testVarianceCalculationTwoRegions<uint8_t, uint32_t, uint64_t, float>(testDuration);
	Log::info() << " ";
	testResult = testVarianceCalculationTwoRegions<int8_t, int32_t, uint64_t, float>(testDuration);
	Log::info() << " ";
	testResult = testVarianceCalculationTwoRegions<uint8_t, uint32_t, uint64_t, double>(testDuration);
	Log::info() << " ";
	testResult = testVarianceCalculationTwoRegions<int8_t, int32_t, uint64_t, double>(testDuration);
	Log::info() << " ";
	testResult = testVarianceCalculationTwoRegions<float, float, float, float>(testDuration);
	Log::info() << " ";
	testResult = testVarianceCalculationTwoRegions<double, double, double, double>(testDuration);

	return testResult.succeeded();
}

template <typename T, typename TIntegral, typename TIntegralSquared, typename TVariance>
bool TestIntegralImage::testVarianceCalculationTwoRegions(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "for data types " <<  TypeNamer::name<T>() << ", " << TypeNamer::name<TIntegral>() << ", " << TypeNamer::name<TIntegralSquared>() << ", " << TypeNamer::name<TVariance>() << ":";

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.985, randomGenerator);

	const unsigned int frameWidth = RandomI::random(randomGenerator, 1u, 1920u);
	const unsigned int frameHeight = RandomI::random(randomGenerator, 1u, 1080u);

	const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(frameWidth, frameHeight, FrameType::genericPixelFormat<T, 1u>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
	Frame linedIntegralFrame = CV::CVUtilities::randomizedFrame(FrameType(frameWidth + 1u, frameHeight + 1u, FrameType::genericPixelFormat<TIntegral, 1u>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
	Frame linedIntegralSquaredFrame = CV::CVUtilities::randomizedFrame(FrameType(frameWidth + 1u, frameHeight + 1u, FrameType::genericPixelFormat<TIntegralSquared, 1u>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

	CV::IntegralImage::createLinedImage<T, TIntegral, 1u>(frame.constdata<T>(), linedIntegralFrame.data<TIntegral>(), frameWidth, frameHeight, frame.paddingElements(), linedIntegralFrame.paddingElements());
	CV::IntegralImage::createLinedImageSquared<T, TIntegralSquared, 1u>(frame.constdata<T>(), linedIntegralSquaredFrame.data<TIntegralSquared>(), frameWidth, frameHeight, frame.paddingElements(), linedIntegralSquaredFrame.paddingElements());

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		const unsigned int windowALeft = RandomI::random(randomGenerator, 0u, frameWidth - 1u);
		const unsigned int windowATop = RandomI::random(randomGenerator, 0u, frameHeight - 1u);
		const unsigned int windowAWidth = RandomI::random(randomGenerator, 1u, frameWidth - windowALeft);
		const unsigned int windowAHeight = RandomI::random(randomGenerator, 1u, frameHeight - windowATop);

		const unsigned int windowBLeft = RandomI::random(randomGenerator, 0u, frameWidth - 1u);
		const unsigned int windowBTop = RandomI::random(randomGenerator, 0u, frameHeight - 1u);
		const unsigned int windowBWidth = RandomI::random(randomGenerator, 1u, frameWidth - windowBLeft);
		const unsigned int windowBHeight = RandomI::random(randomGenerator, 1u, frameHeight - windowBTop);

		const TVariance variance = CV::IntegralImage::linedIntegralVariance<TIntegral, TIntegralSquared, TVariance>(linedIntegralFrame.constdata<TIntegral>(), linedIntegralSquaredFrame.constdata<TIntegralSquared>(), linedIntegralFrame.strideElements(), linedIntegralSquaredFrame.strideElements(), windowALeft, windowATop, windowAWidth, windowAHeight, windowBLeft, windowBTop, windowBWidth, windowBHeight);

		// determine mean value

		double sum = 0.0;

		for (unsigned int y = windowATop; y < windowATop + windowAHeight; ++y)
		{
			for (unsigned int x = windowALeft; x < windowALeft + windowAWidth; ++x)
			{
				sum += double(frame.constpixel<T>(x, y)[0]);
			}
		}

		for (unsigned int y = windowBTop; y < windowBTop + windowBHeight; ++y)
		{
			for (unsigned int x = windowBLeft; x < windowBLeft + windowBWidth; ++x)
			{
				sum += double(frame.constpixel<T>(x, y)[0]);
			}
		}

		ocean_assert(windowAWidth * windowAHeight != 0u);
		ocean_assert(windowBWidth * windowBHeight != 0u);

		const double pixels = double(windowAWidth * windowAHeight + windowBWidth * windowBHeight);

		const double mean = sum / pixels;

		double sumSquareDifferences = 0.0;

		for (unsigned int y = windowATop; y < windowATop + windowAHeight; ++y)
		{
			for (unsigned int x = windowALeft; x < windowALeft + windowAWidth; ++x)
			{
				sumSquareDifferences += NumericD::sqr(double(frame.constpixel<T>(x, y)[0]) - mean);
			}
		}

		for (unsigned int y = windowBTop; y < windowBTop + windowBHeight; ++y)
		{
			for (unsigned int x = windowBLeft; x < windowBLeft + windowBWidth; ++x)
			{
				sumSquareDifferences += NumericD::sqr(double(frame.constpixel<T>(x, y)[0]) - mean);
			}
		}

		const double testVariance = sumSquareDifferences / pixels;
		const double testDeviation = NumericD::sqrt(testVariance);

		const double deviation = NumericD::sqrt(double(variance));

		if (!NumericD::isEqual(deviation, testDeviation, testDeviation * 0.01))
		{
			scopedIteration.setInaccurate();
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T, typename TIntegral>
bool TestIntegralImage::validateIntegralImage(const T* source, const TIntegral* integral, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements, const unsigned int validationChecks)
{
	ocean_assert(source != nullptr && integral != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(channels >= 1u);
	ocean_assert(validationChecks >= 1u);

	const unsigned int sourceStrideElements = width * channels + sourcePaddingElements;
	const unsigned int integralStrideElements = width * channels + integralPaddingElements;

	unsigned int checks = 0u;

	do
	{
		const unsigned int xLocation = RandomI::random(0u, width - 1u);
		const unsigned int yLocation = RandomI::random(0u, height - 1u);

		std::vector<double> sums(channels, 0.0);

		for (unsigned int y = 0u; y <= yLocation; ++y)
		{
			const T* sourceRow = source + y * sourceStrideElements;

			for (unsigned int x = 0u; x <= xLocation; ++x)
			{
				for (unsigned int n = 0u; n < channels; ++n)
				{
					sums[n] += double(sourceRow[x * channels + n]);
				}
			}
		}

		for (unsigned int n = 0u; n < channels; ++n)
		{
			if (NumericD::isNotWeakEqual(double(integral[yLocation * integralStrideElements + xLocation * channels + n]), sums[n]))
			{
				return false;
			}
		}

		checks++;
	}
	while (checks < validationChecks);

	return true;
}

template <typename T, typename TIntegral>
bool TestIntegralImage::validateBorderedIntegralImage(const T* source, const TIntegral* integral, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int border, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements)
{
	switch (channels)
	{
		case 1u:
			return validateBorderedIntegralImage<T, TIntegral, 1u>(source, integral, width, height, border, sourcePaddingElements, integralPaddingElements);

		case 2u:
			return validateBorderedIntegralImage<T, TIntegral, 2u>(source, integral, width, height, border, sourcePaddingElements, integralPaddingElements);

		case 3u:
			return validateBorderedIntegralImage<T, TIntegral, 3u>(source, integral, width, height, border, sourcePaddingElements, integralPaddingElements);

		case 4u:
			return validateBorderedIntegralImage<T, TIntegral, 4u>(source, integral, width, height, border, sourcePaddingElements, integralPaddingElements);

		case 5u:
			return validateBorderedIntegralImage<T, TIntegral, 5u>(source, integral, width, height, border, sourcePaddingElements, integralPaddingElements);

		default:
			break;
	}

	ocean_assert(false && "This should never happen!");
	return false;
}

template <typename T, typename TIntegral, unsigned int tChannels>
bool TestIntegralImage::validateBorderedIntegralImage(const T* source, const TIntegral* integral, const unsigned int width, const unsigned int height, const unsigned int border, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && integral != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int integralStrideElements = (width + 1u + border * 2u) * tChannels + integralPaddingElements;

	const unsigned int leftBorder = border + 1u;
	const unsigned int topBorder = border + 1u;
	const unsigned int rightBorder = border;
	const unsigned int bottomBorder = border;

	// checking the core integral

	Frame coreIntegralImage(FrameType(width, height, FrameType::genericPixelFormat<TIntegral, tChannels>(), FrameType::ORIGIN_UPPER_LEFT));
	CV::IntegralImage::createImage<T, TIntegral, tChannels>(source, coreIntegralImage.data<TIntegral>(), width, height, sourcePaddingElements, coreIntegralImage.paddingElements());

	for (unsigned int y = 0u; y < height; y++)
	{
		const TIntegral* integralRow = integral + (y + topBorder) * integralStrideElements;
		const TIntegral* coreIntegralRow = coreIntegralImage.constrow<TIntegral>(y);

		for (unsigned int x = 0u; x < width; x++)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				const TIntegral integralValue = integralRow[(leftBorder + x) * tChannels + n];
				const TIntegral coreIntegralValue = coreIntegralRow[x * tChannels + n];

				if (integralValue != coreIntegralValue)
				{
					return false;
				}
			}
		}
	}

	// checking the top border

	for (unsigned int y = 0u; y < topBorder; ++y)
	{
		const TIntegral* integralRow = integral + y * integralStrideElements;

		for (unsigned int x = 0u; x < width + leftBorder + rightBorder; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				if (integralRow[x * tChannels + n] != TIntegral(0))
				{
					return false;
				}
			}
		}
	}

	// checking the left and right border

	for (unsigned int y = 0u; y < height + topBorder + bottomBorder; ++y)
	{
		const TIntegral* integralRow = integral + y * integralStrideElements;

		for (unsigned int x = 0u; x < leftBorder; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				if (integralRow[x * tChannels + n] != TIntegral(0))
				{
					return false;
				}
			}
		}

		for (unsigned int x = leftBorder + width; x < leftBorder + width + rightBorder; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				if (integralRow[x * tChannels + n] != integralRow[(leftBorder + width - 1u) * tChannels + n])
				{
					return false;
				}
			}
		}
	}

	// checking the bottom border

	for (unsigned int y = topBorder + height; y < topBorder + height + bottomBorder; ++y)
	{
		const TIntegral* lastValueIntegralRow = integral + (topBorder + height - 1u) * integralStrideElements;
		const TIntegral* integralRow = integral + y * integralStrideElements;

		for (unsigned int x = 0u; x < leftBorder; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				if (integralRow[x * tChannels + n] != lastValueIntegralRow[x * tChannels + n])
				{
					return false;
				}
			}
		}
	}

	return true;
}

template <typename T, typename TIntegral, unsigned int tChannels>
bool TestIntegralImage::validateBorderedIntegralImageSquared(const T* source, const TIntegral* integral, const unsigned int width, const unsigned int height, const unsigned int border, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements, const unsigned int validationChecks)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && integral != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;
	const unsigned int integralStrideElements = (width + 1u + border * 2u) * tChannels + integralPaddingElements;

	const unsigned int leftBorder = border + 1u;
	const unsigned int topBorder = border + 1u;
	const unsigned int rightBorder = border;
	const unsigned int bottomBorder = border;

	// checking the core integral

	unsigned int checks = 0u;

	do
	{
		const unsigned int xLocation = RandomI::random(0u, width - 1u);
		const unsigned int yLocation = RandomI::random(0u, height - 1u);

		std::vector<double> squaredSums(tChannels, 0.0);

		for (unsigned int y = 0u; y <= yLocation; ++y)
		{
			const T* sourceRow = source + y * sourceStrideElements;

			for (unsigned int x = 0u; x <= xLocation; ++x)
			{
				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					const double value = double(sourceRow[x * tChannels + n]);

					squaredSums[n] += value * value;
				}
			}
		}

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			const unsigned int xLocationBordered = xLocation + leftBorder;
			const unsigned int yLocationBordered = yLocation + topBorder;

			constexpr double epsilon = std::is_floating_point<TIntegral>::value ? 1.0 : NumericD::weakEps();

			if (NumericD::isNotEqual(double(integral[yLocationBordered * integralStrideElements + xLocationBordered * tChannels + n]), squaredSums[n], epsilon))
			{
				return false;
			}
		}

		checks++;
	}
	while (checks < validationChecks);

	// checking the top border

	for (unsigned int y = 0u; y < topBorder; ++y)
	{
		const TIntegral* integralRow = integral + y * integralStrideElements;

		for (unsigned int x = 0u; x < width + leftBorder + rightBorder; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				if (integralRow[x * tChannels + n] != TIntegral(0))
				{
					return false;
				}
			}
		}
	}

	// checking the left and right border

	for (unsigned int y = 0u; y < height + topBorder + bottomBorder; ++y)
	{
		const TIntegral* integralRow = integral + y * integralStrideElements;

		for (unsigned int x = 0u; x < leftBorder; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				if (integralRow[x * tChannels + n] != TIntegral(0))
				{
					return false;
				}
			}
		}

		for (unsigned int x = leftBorder + width; x < leftBorder + width + rightBorder; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				if (integralRow[x * tChannels + n] != integralRow[(leftBorder + width - 1u) * tChannels + n])
				{
					return false;
				}
			}
		}
	}

	// checking the bottom border

	for (unsigned int y = topBorder + height; y < topBorder + height + bottomBorder; ++y)
	{
		const TIntegral* lastValueIntegralRow = integral + (topBorder + height - 1u) * integralStrideElements;
		const TIntegral* integralRow = integral + y * integralStrideElements;

		for (unsigned int x = 0u; x < leftBorder; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				if (integralRow[x * tChannels + n] != lastValueIntegralRow[x * tChannels + n])
				{
					return false;
				}
			}
		}
	}

	return true;
}

template <typename T, typename TIntegralAndSquared, unsigned int tChannels>
bool TestIntegralImage::validateBorderedIntegralImageAndSquaredJoined(const T* source, const TIntegralAndSquared* integralAndSquared, const unsigned int width, const unsigned int height, const unsigned int border, const unsigned int sourcePaddingElements, const unsigned int integralAndSquaredPaddingElements, const unsigned int validationChecks)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && integralAndSquared != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;
	const unsigned int integralAndSquaredStrideElements = (width + 1u + border * 2u) * 2u * tChannels + integralAndSquaredPaddingElements;

	const unsigned int leftBorder = border + 1u;
	const unsigned int topBorder = border + 1u;
	const unsigned int rightBorder = border;
	const unsigned int bottomBorder = border;

	// checking the core integral

	unsigned int checks = 0u;

	do
	{
		const unsigned int xLocation = RandomI::random(0u, width - 1u);
		const unsigned int yLocation = RandomI::random(0u, height - 1u);

		std::vector<double> sums(tChannels, 0.0);
		std::vector<double> squaredSums(tChannels, 0.0);

		for (unsigned int y = 0u; y <= yLocation; ++y)
		{
			const T* sourceRow = source + y * sourceStrideElements;

			for (unsigned int x = 0u; x <= xLocation; ++x)
			{
				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					const double value = double(sourceRow[x * tChannels + n]);

					sums[n] += value;
					squaredSums[n] += value * value;
				}
			}
		}

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			const unsigned int xLocationBordered = xLocation + leftBorder;
			const unsigned int yLocationBordered = yLocation + topBorder;

			constexpr double epsilon = std::is_floating_point<TIntegralAndSquared>::value ? 1.0 : NumericD::weakEps();

			if (NumericD::isNotEqual(double(integralAndSquared[yLocationBordered * integralAndSquaredStrideElements + xLocationBordered * 2u * tChannels + tChannels * 0u + n]), sums[n], epsilon))
			{
				return false;
			}

			if (NumericD::isNotEqual(double(integralAndSquared[yLocationBordered * integralAndSquaredStrideElements + xLocationBordered * 2u * tChannels + tChannels * 1u + n]), squaredSums[n], epsilon))
			{
				return false;
			}
		}

		checks++;
	}
	while (checks < validationChecks);

	// checking the top border

	for (unsigned int y = 0u; y < topBorder; ++y)
	{
		const TIntegralAndSquared* integralAndSquaredRow = integralAndSquared + y * integralAndSquaredStrideElements;

		for (unsigned int x = 0u; x < width + leftBorder + rightBorder; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				if (integralAndSquaredRow[x * 2u * tChannels + tChannels * 0u + n] != TIntegralAndSquared(0))
				{
					return false;
				}

				if (integralAndSquaredRow[x * 2u * tChannels + tChannels * 1u + n] != TIntegralAndSquared(0))
				{
					return false;
				}
			}
		}
	}

	// checking the left and right border

	for (unsigned int y = 0u; y < height + topBorder + bottomBorder; ++y)
	{
		const TIntegralAndSquared* integralAndSquaredRow = integralAndSquared + y * integralAndSquaredStrideElements;

		for (unsigned int x = 0u; x < leftBorder; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				if (integralAndSquaredRow[x * 2u * tChannels + tChannels * 0u + n] != TIntegralAndSquared(0))
				{
					return false;
				}

				if (integralAndSquaredRow[x * 2u * tChannels + tChannels * 1u + n] != TIntegralAndSquared(0))
				{
					return false;
				}
			}
		}

		for (unsigned int x = leftBorder + width; x < leftBorder + width + rightBorder; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				if (integralAndSquaredRow[x * 2u * tChannels + tChannels * 0u + n] != integralAndSquaredRow[(leftBorder + width - 1u) * 2u * tChannels + tChannels * 0u + n])
				{
					return false;
				}

				if (integralAndSquaredRow[x * 2u * tChannels + tChannels * 1u + n] != integralAndSquaredRow[(leftBorder + width - 1u) * 2u * tChannels + tChannels * 1u + n])
				{
					return false;
				}
			}
		}
	}

	// checking the bottom border

	for (unsigned int y = topBorder + height; y < topBorder + height + bottomBorder; ++y)
	{
		const TIntegralAndSquared* lastValueIntegralAndSquaredRow = integralAndSquared + (topBorder + height - 1u) * integralAndSquaredStrideElements;
		const TIntegralAndSquared* integralAndSquaredRow = integralAndSquared + y * integralAndSquaredStrideElements;

		for (unsigned int x = 0u; x < leftBorder; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				if (integralAndSquaredRow[x * 2u * tChannels + tChannels * 0u + n] != lastValueIntegralAndSquaredRow[x * 2u * tChannels + tChannels * 0u + n])
				{
					return false;
				}

				if (integralAndSquaredRow[x * 2u * tChannels + tChannels * 1u + n] != lastValueIntegralAndSquaredRow[x * 2u * tChannels + tChannels * 1u + n])
				{
					return false;
				}
			}
		}
	}

	return true;
}

template <typename T, typename TIntegral, typename TIntegralSquared, unsigned int tChannels>
bool TestIntegralImage::validateBorderedIntegralImageAndSquaredSeparate(const T* source, const TIntegral* integral, const TIntegralSquared* integralSquared, const unsigned int width, const unsigned int height, const unsigned int border, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements, const unsigned int integralSquaredPaddingElements, const unsigned int validationChecks)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && integral != nullptr && integralSquared != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;
	const unsigned int integralStrideElements = (width + 1u + border * 2u) * tChannels + integralPaddingElements;
	const unsigned int integralSquaredStrideElements = (width + 1u + border * 2u) * tChannels + integralSquaredPaddingElements;

	const unsigned int leftBorder = border + 1u;
	const unsigned int topBorder = border + 1u;
	const unsigned int rightBorder = border;
	const unsigned int bottomBorder = border;

	// checking the core integral

	unsigned int checks = 0u;

	do
	{
		const unsigned int xLocation = RandomI::random(0u, width - 1u);
		const unsigned int yLocation = RandomI::random(0u, height - 1u);

		std::vector<double> sums(tChannels, 0.0);
		std::vector<double> squaredSums(tChannels, 0.0);

		for (unsigned int y = 0u; y <= yLocation; ++y)
		{
			const T* sourceRow = source + y * sourceStrideElements;

			for (unsigned int x = 0u; x <= xLocation; ++x)
			{
				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					const double value = double(sourceRow[x * tChannels + n]);

					sums[n] += value;
					squaredSums[n] += value * value;
				}
			}
		}

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			const unsigned int xLocationBordered = xLocation + leftBorder;
			const unsigned int yLocationBordered = yLocation + topBorder;

			constexpr double epsilonIntegral = std::is_floating_point<TIntegral>::value ? 1.0 : NumericD::weakEps();
			constexpr double epsilonIntegralSquared = std::is_floating_point<TIntegralSquared>::value ? 1.0 : NumericD::weakEps();

			if (NumericD::isNotEqual(double(integral[yLocationBordered * integralStrideElements + xLocationBordered * tChannels + n]), sums[n], epsilonIntegral))
			{
				return false;
			}

			if (NumericD::isNotEqual(double(integralSquared[yLocationBordered * integralSquaredStrideElements + xLocationBordered * tChannels + n]), squaredSums[n], epsilonIntegralSquared))
			{
				return false;
			}
		}

		checks++;
	}
	while (checks < validationChecks);

	// checking the top border

	for (unsigned int y = 0u; y < topBorder; ++y)
	{
		const TIntegral* integralRow = integral + y * integralStrideElements;
		const TIntegralSquared* integralSquaredRow = integralSquared + y * integralSquaredStrideElements;

		for (unsigned int x = 0u; x < width + leftBorder + rightBorder; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				if (integralRow[x * tChannels + n] != TIntegral(0))
				{
					return false;
				}

				if (integralSquaredRow[x * tChannels + n] != TIntegralSquared(0))
				{
					return false;
				}
			}
		}
	}

	// checking the left and right border

	for (unsigned int y = 0u; y < height + topBorder + bottomBorder; ++y)
	{
		const TIntegral* integralRow = integral + y * integralStrideElements;
		const TIntegralSquared* integralSquaredRow = integralSquared + y * integralSquaredStrideElements;

		for (unsigned int x = 0u; x < leftBorder; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				if (integralRow[x * tChannels + n] != TIntegral(0))
				{
					return false;
				}

				if (integralSquaredRow[x * tChannels + n] != TIntegralSquared(0))
				{
					return false;
				}
			}
		}

		for (unsigned int x = leftBorder + width; x < leftBorder + width + rightBorder; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				if (integralRow[x * tChannels + n] != integralRow[(leftBorder + width - 1u) * tChannels + n])
				{
					return false;
				}

				if (integralSquaredRow[x * tChannels + n] != integralSquaredRow[(leftBorder + width - 1u) * tChannels + n])
				{
					return false;
				}
			}
		}
	}

	// checking the bottom border

	for (unsigned int y = topBorder + height; y < topBorder + height + bottomBorder; ++y)
	{
		const TIntegral* lastValueIntegralRow = integral + (topBorder + height - 1u) * integralStrideElements;
		const TIntegralSquared* lastValueIntegralSquaredRow = integralSquared + (topBorder + height - 1u) * integralSquaredStrideElements;

		const TIntegral* integralRow = integral + y * integralStrideElements;
		const TIntegralSquared* integralSquaredRow = integralSquared + y * integralSquaredStrideElements;

		for (unsigned int x = 0u; x < leftBorder; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				if (integralRow[x * tChannels + n] != lastValueIntegralRow[x * tChannels + n])
				{
					return false;
				}

				if (integralSquaredRow[x * tChannels + n] != lastValueIntegralSquaredRow[x * tChannels + n])
				{
					return false;
				}
			}
		}
	}

	return true;
}

template <typename T, typename TIntegral, unsigned int tChannels>
bool TestIntegralImage::validateBorderedIntegralImageMirror(const T* source, const TIntegral* integral, const unsigned int width, const unsigned int height, const unsigned int border, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements, const unsigned int validationChecks)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && integral != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(border >= 1u);

	unsigned int borderedWidth = 2u * border + width + 1u;
	unsigned int borderedHeight = 2u * border + height + 1u;

	const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;
	const unsigned int integralStrideElements = borderedWidth * tChannels + integralPaddingElements;

	// check top row of the integral image
	for (unsigned int x = 0; x < borderedWidth * tChannels; ++x)
	{
		if (integral[x] != TIntegral(0))
		{
			return false;
		}
	}

	// check left column of the integral image
	for (unsigned int y = 0; y < borderedHeight; ++y)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			if (integral[y * integralStrideElements + n] != TIntegral(0))
			{
				return false;
			}
		}
	}

	unsigned int checks = 0u;

	do
	{
		const int xLocation = RandomI::random(-int(border) - 1, int(width + border) - 1);
		const int yLocation = RandomI::random(-int(border) - 1, int(height + border) - 1);

		const unsigned int iX = (unsigned int)(xLocation + int(border + 1u));
		const unsigned int iY = (unsigned int)(yLocation + int(border + 1u));

		ocean_assert(iX < borderedWidth);
		ocean_assert(iY < borderedHeight);

		double value[tChannels] = {TIntegral(0)};

		for (int yy = -int(border); yy <= yLocation; ++yy)
		{
			for (int xx = -int(border); xx <= xLocation; ++xx)
			{
				int oYY = yy;
				int oXX = xx;

				if (oYY < 0)
				{
					oYY = abs(oYY + 1);
				}
				else if (oYY >= int(height))
				{
					oYY = (height - oYY) + height - 1;
				}

				if (oXX < 0)
				{
					oXX = abs(oXX + 1);
				}
				else if (oXX >= int(width))
				{
					oXX = (width - oXX) + width - 1;
				}

				ocean_assert(oXX >= 0 && oXX < int(width));
				ocean_assert(oYY >= 0 && oYY < int(height));

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					value[n] += double(source[oYY * sourceStrideElements + oXX * tChannels + n]);
				}
			}
		}

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			if (double(integral[iY * integralStrideElements + iX * tChannels + n]) != value[n])
			{
				return false;
			}
		}

		checks++;
	}
	while (checks < validationChecks);

	return true;
}

template <typename T, typename TIntegral, unsigned int tChannels>
bool TestIntegralImage::validateBorderedIntegralImageSquaredMirror(const T* source, const TIntegral* integral, const unsigned int width, const unsigned int height, const unsigned int border, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements, const unsigned int validationChecks)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && integral != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(border >= 1u);

	unsigned int borderedWidth = 2u * border + width + 1u;
	unsigned int borderedHeight = 2u * border + height + 1u;

	const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;
	const unsigned int integralStrideElements = borderedWidth * tChannels + integralPaddingElements;

	// check top row of the integral image
	for (unsigned int x = 0; x < borderedWidth * tChannels; ++x)
	{
		if (integral[x] != TIntegral(0))
		{
			return false;
		}
	}

	// check left column of the integral image
	for (unsigned int y = 0; y < borderedHeight; ++y)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			if (integral[y * integralStrideElements + n] != TIntegral(0))
			{
				return false;
			}
		}
	}

	unsigned int checks = 0u;

	do
	{
		const int xLocation = RandomI::random(-int(border) - 1, int(width + border) - 1);
		const int yLocation = RandomI::random(-int(border) - 1, int(height + border) - 1);

		const unsigned int iX = (unsigned int)(xLocation + int(border + 1u));
		const unsigned int iY = (unsigned int)(yLocation + int(border + 1u));

		ocean_assert(iX < borderedWidth);
		ocean_assert(iY < borderedHeight);

		double value[tChannels] = {TIntegral(0)};

		for (int yy = -int(border); yy <= yLocation; ++yy)
		{
			for (int xx = -int(border); xx <= xLocation; ++xx)
			{
				int oYY = yy;
				int oXX = xx;

				if (oYY < 0)
				{
					oYY = abs(oYY + 1);
				}
				else if (oYY >= int(height))
				{
					oYY = (height - oYY) + height - 1;
				}

				if (oXX < 0)
				{
					oXX = abs(oXX + 1);
				}
				else if (oXX >= int(width))
				{
					oXX = (width - oXX) + width - 1;
				}

				ocean_assert(oXX >= 0 && oXX < int(width));
				ocean_assert(oYY >= 0 && oYY < int(height));

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					value[n] += NumericD::sqr(double(source[oYY * sourceStrideElements + oXX * tChannels + n]));
				}
			}
		}

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			if (double(integral[iY * integralStrideElements + iX * tChannels + n]) != value[n])
			{
				return false;
			}
		}

		checks++;
	}
	while (checks < validationChecks);

	return true;
}

}

}

}
