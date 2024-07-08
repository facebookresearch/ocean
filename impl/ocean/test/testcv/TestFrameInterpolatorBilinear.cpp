/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameInterpolatorBilinear.h"

#include "ocean/base/DataType.h"
#include "ocean/base/Frame.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/geometry/Homography.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameInterpolatorBilinear::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width != 0u && height != 0u && testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Bilinear interpolation test:   ---";
	Log::info() << " ";

	allSucceeded = testInterpolatePixel8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testInterpolatePixel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAffine(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHomography<uint8_t>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHomography<int16_t>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHomography<int32_t>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHomography<float>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHomography<double>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHomographyMask(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testResizeExtremeResolutions(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testResize(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testResize<uint8_t>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testResize<int16_t>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testResize<int32_t>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testResize<float>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testResize<double>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSpecialCasesResize400x400To224x224_8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSpecialCasesResize400x400To256x256_8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testLookup(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testLookupMask(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRotateFrame(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPatchIntensitySum1Channel(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testResampleCameraImage(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Bilinear interpolation test succeeded.";
	}
	else
	{
		Log::info() << "Bilinear interpolation test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameInterpolatorBilinear, InterpolatePixel8BitPerChannel_float_TopLeft)
{
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testInterpolatePixel8BitPerChannel<float>(CV::PC_TOP_LEFT, GTEST_TEST_DURATION)));
}

TEST(TestFrameInterpolatorBilinear, InterpolatePixel8BitPerChannel_float_Center)
{
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testInterpolatePixel8BitPerChannel<float>(CV::PC_CENTER, GTEST_TEST_DURATION)));
}

TEST(TestFrameInterpolatorBilinear, InterpolatePixel8BitPerChannel_double_TopLeft)
{
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testInterpolatePixel8BitPerChannel<double>(CV::PC_TOP_LEFT, GTEST_TEST_DURATION)));
}

TEST(TestFrameInterpolatorBilinear, InterpolatePixel8BitPerChannel_double_Center)
{
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testInterpolatePixel8BitPerChannel<double>(CV::PC_CENTER, GTEST_TEST_DURATION)));
}


TEST(TestFrameInterpolatorBilinear, InterpolatePixel_uint8_uint8_float_TopLeft)
{
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testInterpolatePixel<uint8_t, uint8_t, float>(CV::PC_TOP_LEFT, GTEST_TEST_DURATION)));
}

TEST(TestFrameInterpolatorBilinear, InterpolatePixel_uint8_float_float_TopLeft)
{
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testInterpolatePixel<uint8_t, float, float>(CV::PC_TOP_LEFT, GTEST_TEST_DURATION)));
}

TEST(TestFrameInterpolatorBilinear, InterpolatePixel_float_float_float_TopLeft)
{
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testInterpolatePixel<float, float, float>(CV::PC_TOP_LEFT, GTEST_TEST_DURATION)));
}

TEST(TestFrameInterpolatorBilinear, InterpolatePixel_uint8_uint8_double_TopLeft)
{
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testInterpolatePixel<uint8_t, uint8_t, double>(CV::PC_TOP_LEFT, GTEST_TEST_DURATION)));
}

TEST(TestFrameInterpolatorBilinear, InterpolatePixel_uint8_double_double_TopLeft)
{
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testInterpolatePixel<uint8_t, double, double>(CV::PC_TOP_LEFT, GTEST_TEST_DURATION)));
}

TEST(TestFrameInterpolatorBilinear, InterpolatePixel_double_double_double_TopLeft)
{
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testInterpolatePixel<double, double, double>(CV::PC_TOP_LEFT, GTEST_TEST_DURATION)));
}


TEST(TestFrameInterpolatorBilinear, ResizeExtremeResolutions)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testResizeExtremeResolutions(GTEST_TEST_DURATION, worker));
}

// 1920x1080 -> 1800x900
TEST(TestFrameInterpolatorBilinear, Resize_1920x1080_to_1800x900_1channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testResize(1920u, 1080u, 1u, 1800u, 900u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorBilinear, Resize_1920x1080_to_1800x900_2channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testResize(1920u, 1080u, 2u, 1800u, 900u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorBilinear, Resize_1920x1080_to_1800x900_3channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testResize(1920u, 1080u, 3u, 1800u, 900u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorBilinear, Resize_1920x1080_to_1800x900_4channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testResize(1920u, 1080u, 4u, 1800u, 900u, GTEST_TEST_DURATION, worker));
}

// 1800x900 -> 1920x1080
TEST(TestFrameInterpolatorBilinear, Resize_1800x900_to_1920x1080_1channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testResize(1800u, 900u, 1u, 1920u, 1080u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorBilinear, Resize_1800x900_to_1920x1080_2channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testResize(1800u, 900u, 2u, 1920u, 1080u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorBilinear, Resize_1800x900_to_1920x1080_3channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testResize(1800u, 900u, 3u, 1920u, 1080u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorBilinear, Resize_1800x900_to_1920x1080_4channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testResize(1800u, 900u, 4u, 1920u, 1080u, GTEST_TEST_DURATION, worker));
}

// 1920x1080 -> 1920x855
TEST(TestFrameInterpolatorBilinear, Resize_1920x1080_to_1920x855_1channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testResize(1920u, 1080u, 1u, 1920u, 855u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorBilinear, Resize_1920x1080_to_1920x855_2channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testResize(1920u, 1080u, 2u, 1920u, 855u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorBilinear, Resize_1920x1080_to_1920x855_3channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testResize(1920u, 1080u, 3u, 1920u, 855u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorBilinear, Resize_1920x1080_to_1920x855_4channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testResize(1920u, 1080u, 4u, 1920u, 855u, GTEST_TEST_DURATION, worker));
}

// 1920x1080 -> 1803x1080
TEST(TestFrameInterpolatorBilinear, Resize_1920x1080_to_1803x1080_1channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testResize(1920u, 1080u, 1u, 1803u, 1080u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorBilinear, Resize_1920x1080_to_1803x1080_2channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testResize(1920u, 1080u, 2u, 1803u, 1080u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorBilinear, Resize_1920x1080_to_1803x1080_3channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testResize(1920u, 1080u, 3u, 1803u, 1080u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorBilinear, Resize_1920x1080_to_1803x1080_4channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testResize(1920u, 1080u, 4u, 1803u, 1080u, GTEST_TEST_DURATION, worker));
}


// Special case resize functions

TEST(TestFrameInterpolatorBilinear, SpecialCasesResize400x400To224x224_8BitPerChannel)
{
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testSpecialCasesResize400x400To224x224_8BitPerChannel(GTEST_TEST_DURATION));
}

TEST(TestFrameInterpolatorBilinear, SpecialCasesResize400x400To256x256_8BitPerChannel)
{
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testSpecialCasesResize400x400To256x256_8BitPerChannel(GTEST_TEST_DURATION));
}


// Homography interpolation test

TEST(TestFrameInterpolatorBilinear, Homography_1920x1080_uin8t_1channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testHomography<uint8_t>(1920u, 1080u, 1u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorBilinear, Homography_1920x1080_uin8t_2channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testHomography<uint8_t>(1920u, 1080u, 2u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorBilinear, Homography_1920x1080_uin8t_3channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testHomography<uint8_t>(1920u, 1080u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorBilinear, Homography_1920x1080_uin8t_4channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testHomography<uint8_t>(1920u, 1080u, 4u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameInterpolatorBilinear, Homography_1920x1080_float_1channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testHomography<float>(1920u, 1080u, 1u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorBilinear, Homography_1920x1080_float_2channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testHomography<float>(1920u, 1080u, 2u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorBilinear, Homography_1920x1080_float_3channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testHomography<float>(1920u, 1080u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorBilinear, Homography_1920x1080_float_4channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testHomography<float>(1920u, 1080u, 4u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameInterpolatorBilinear, HomographyMask_1920x1080_1channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testHomographyMask(1920u, 1080u, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorBilinear, HomographyMask_1920x1080_2channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testHomographyMask(1920u, 1080u, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorBilinear, HomographyMask_1920x1080_3channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testHomographyMask(1920u, 1080u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorBilinear, HomographyMask_1920x1080_4channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testHomographyMask(1920u, 1080u, 4u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameInterpolatorBilinear, Affine_1920x1080_1channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testAffine(1920u, 1080u, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorBilinear, Affine_1920x1080_2channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testAffine(1920u, 1080u, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorBilinear, Affine_1920x1080_3channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testAffine(1920u, 1080u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorBilinear, Affine_1920x1080_4channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testAffine(1920u, 1080u, 4u, GTEST_TEST_DURATION, worker));
}


// Lookup interpolation test

TEST(TestFrameInterpolatorBilinear, Lookup_1920x1080_1channel_uint8)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testLookup<uint8_t>(1920u, 1080u, 1u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorBilinear, Lookup_1920x1080_2channel_uint8)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testLookup<uint8_t>(1920u, 1080u, 2u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorBilinear, Lookup_1920x1080_3channel_uint8)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testLookup<uint8_t>(1920u, 1080u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorBilinear, Lookup_1920x1080_4channel_uint8)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testLookup<uint8_t>(1920u, 1080u, 4u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameInterpolatorBilinear, Lookup_1920x1080_1channel_float)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testLookup<float>(1920u, 1080u, 1u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorBilinear, Lookup_1920x1080_2channel_float)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testLookup<float>(1920u, 1080u, 2u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorBilinear, Lookup_1920x1080_3channel_float)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testLookup<float>(1920u, 1080u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorBilinear, Lookup_1920x1080_4channel_float)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testLookup<float>(1920u, 1080u, 4u, GTEST_TEST_DURATION, worker)));
}


// Lookup with mask interpolation test

TEST(TestFrameInterpolatorBilinear, LookupMask_1920x1080)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testLookupMask(1920u, 1080u, GTEST_TEST_DURATION, worker));
}


// Rotate test

TEST(TestFrameInterpolatorBilinear, RotateFrame_1920x1080)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testRotateFrame(1920u, 1080u, GTEST_TEST_DURATION, worker));
}



// Patch intensity

TEST(TestFrameInterpolatorBilinear, PatchIntensitySum1Channel)
{
	EXPECT_TRUE(TestFrameInterpolatorBilinear::testPatchIntensitySum1Channel(1920u, 1080u, GTEST_TEST_DURATION));
}


TEST(TestFrameInterpolatorBilinear, ResampleCameraImage_uint8)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testResampleCameraImage<uint8_t>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorBilinear, ResampleCameraImage_float)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorBilinear::testResampleCameraImage<float>(GTEST_TEST_DURATION, worker)));
}

#endif

bool TestFrameInterpolatorBilinear::testInterpolatePixel8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Pixel interpolation test with 7bit precision:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testInterpolatePixel8BitPerChannel<float>(CV::PC_TOP_LEFT, testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testInterpolatePixel8BitPerChannel<float>(CV::PC_CENTER, testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testInterpolatePixel8BitPerChannel<double>(CV::PC_TOP_LEFT, testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testInterpolatePixel8BitPerChannel<double>(CV::PC_CENTER, testDuration) && allSucceeded;
	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Pixel interpolation test succeeded.";
	}
	else
	{
		Log::info() << "Pixel interpolation test FAILED!";
	}

	return allSucceeded;
}

template <typename TScalar>
bool TestFrameInterpolatorBilinear::testInterpolatePixel8BitPerChannel(const CV::PixelCenter pixelCenter, const double testDuration)
{
	ocean_assert(pixelCenter == CV::PC_TOP_LEFT || pixelCenter == CV::PC_CENTER);

	ocean_assert(testDuration > 0.0);

	if (pixelCenter == CV::PC_TOP_LEFT)
	{
		Log::info() << "... with '" << TypeNamer::name<TScalar>() << "' and with pixel center at (0.0, 0.0):";
	}
	else
	{
		Log::info() << "... with '" << TypeNamer::name<TScalar>() << "' and with pixel center at (0.5, 0.5):";
	}

	bool allSucceeded = true;

	constexpr TScalar threshold = TScalar(2.5);

	RandomGenerator randomGenerator;

	{
		// testing interpolation of a 2x2 image into a larger image

		constexpr unsigned int channels = 3u;

		const unsigned int sourceFramePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		Frame sourceFrame(FrameType(2u, 2u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT), sourceFramePaddingElements);
		CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);

		const uint8_t* const sourceTopLeft = CV::Canvas::black(sourceFrame.pixelFormat());
		const uint8_t* const sourceTopRight = CV::Canvas::red(sourceFrame.pixelFormat());
		const uint8_t* const sourceBottomLeft = CV::Canvas::green(sourceFrame.pixelFormat());
		const uint8_t* const sourceBottomRight = CV::Canvas::blue(sourceFrame.pixelFormat());

		memcpy(sourceFrame.pixel<uint8_t>(0u, 0u), sourceTopLeft, sizeof(uint8_t) * channels);
		memcpy(sourceFrame.pixel<uint8_t>(1u, 0u), sourceTopRight, sizeof(uint8_t) * channels);
		memcpy(sourceFrame.pixel<uint8_t>(0u, 1u), sourceBottomLeft, sizeof(uint8_t) * channels);
		memcpy(sourceFrame.pixel<uint8_t>(1u, 1u), sourceBottomRight, sizeof(uint8_t) * channels);

		const unsigned int dimension = pixelCenter == CV::PC_TOP_LEFT ? 256u : 512u;

		Frame targetFrame(FrameType(dimension, dimension, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		CV::CVUtilities::randomizeFrame(targetFrame, false, &randomGenerator);

		if (pixelCenter == CV::PC_TOP_LEFT)
		{
			for (unsigned int y = 0u; y < targetFrame.height(); ++y)
			{
				const TScalar yPosition = TScalar(y) / TScalar(targetFrame.height() - 1u); // ensuring to cover full value range [0, 1] == [0, sourceFrame.width() - 1]

				ocean_assert(y != 0u || yPosition == TScalar(0));
				ocean_assert(y != targetFrame.height() - 1u || yPosition == TScalar(1));

				for (unsigned int x = 0u; x < targetFrame.width(); ++x)
				{
					const TScalar xPosition = TScalar(x) / TScalar(targetFrame.width() - 1u);

					ocean_assert(x != 0u || xPosition == TScalar(0));
					ocean_assert(x != targetFrame.width() - 1u || xPosition == TScalar(1));

					ocean_assert(xPosition >= TScalar(0) && xPosition <= TScalar(1));
					ocean_assert(yPosition >= TScalar(0) && yPosition <= TScalar(1));

					CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<channels, CV::PC_TOP_LEFT, TScalar>(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), VectorT2<TScalar>(xPosition, yPosition), targetFrame.pixel<uint8_t>(x, y));
				}
			}

			for (unsigned int y = 0u; y < targetFrame.height(); ++y)
			{
				const double bottomFactor = double(y) / double(targetFrame.height() - 1u);

				ocean_assert(y != 0u || bottomFactor == 0.0);
				ocean_assert(y != targetFrame.height() - 1u || bottomFactor == 1.0);

				const double topFactor = 1.0 - bottomFactor;

				for (unsigned int x = 0u; x < targetFrame.width(); ++x)
				{
					const double rightFactor = double(x) / double(targetFrame.width() - 1u);

					ocean_assert(x != 0u || rightFactor == 0.0);
					ocean_assert(x != targetFrame.width() - 1u || rightFactor == 1.0);

					const double leftFactor = 1.0 - rightFactor;

					const uint8_t* const targetPixel = targetFrame.constpixel<uint8_t>(x, y);

					for (unsigned int n = 0u; n < sourceFrame.channels(); ++n)
					{
						ocean_assert(leftFactor >= 0.0 && leftFactor <= 1.0);
						ocean_assert(topFactor >= 0.0 && topFactor <= 1.0);

						ocean_assert(NumericD::isEqual(leftFactor + rightFactor, 1.0));
						ocean_assert(NumericD::isEqual(topFactor + bottomFactor, 1.0));

						const double top = double(sourceTopLeft[n]) * double(leftFactor) + double(sourceTopRight[n]) * rightFactor;
						const double bottom = double(sourceBottomLeft[n]) * double(leftFactor) + double(sourceBottomRight[n]) * rightFactor;

						const double result = top * topFactor + bottom * bottomFactor;

						if (NumericD::isNotEqual(double(targetPixel[n]), result, double(threshold)))
						{
							allSucceeded = false;
						}
					}
				}
			}
		}
		else
		{
			ocean_assert(pixelCenter == CV::PC_CENTER);

			for (unsigned int y = 0u; y < targetFrame.height(); ++y)
			{
				const TScalar yPosition = TScalar(y * 2u) / TScalar(targetFrame.height() - 1u); // ensuring to cover full value range [0, 2] == [0, sourceFrame.width()]

				ocean_assert(y != 0u || yPosition == TScalar(0));
				ocean_assert(y != targetFrame.height() - 1u || yPosition == TScalar(2));

				for (unsigned int x = 0u; x < targetFrame.width(); ++x)
				{
					const TScalar xPosition = TScalar(x * 2u) / TScalar(targetFrame.width() - 1u);

					ocean_assert(x != 0u || xPosition == TScalar(0));
					ocean_assert(x != targetFrame.width() - 1u || xPosition == TScalar(2));

					ocean_assert(xPosition >= TScalar(0) && xPosition <= TScalar(2));
					ocean_assert(yPosition >= TScalar(0) && yPosition <= TScalar(2));

					CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<channels, CV::PC_CENTER, TScalar>(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), VectorT2(xPosition, yPosition), targetFrame.pixel<uint8_t>(x, y));
				}
			}

			for (unsigned int y = 0u; y < targetFrame.height(); ++y)
			{
				double bottomFactor = 1.0;

				if (y < 128u)
				{
					bottomFactor = 0.0;
				}
				else if (y < 128u + 256u)
				{
					bottomFactor = double(y - 128u) / double(255u);
				}

				const double topFactor = 1.0 - bottomFactor;

				for (unsigned int x = 0u; x < targetFrame.width(); ++x)
				{
					double rightFactor = 1.0;

					if (x < 128u)
					{
						rightFactor = 0.0;
					}
					else if (x < 128u + 256u)
					{
						rightFactor = double(x - 128u) / double(255u);
					}

					const double leftFactor = 1.0 - rightFactor;

					const uint8_t* const targetPixel = targetFrame.constpixel<uint8_t>(x, y);

					for (unsigned int n = 0u; n < sourceFrame.channels(); ++n)
					{
						ocean_assert(leftFactor >= 0.0 && leftFactor <= 1.0);
						ocean_assert(topFactor >= 0.0 && topFactor <= 1.0);

						ocean_assert(NumericD::isEqual(leftFactor + rightFactor, 1.0));
						ocean_assert(NumericD::isEqual(topFactor + bottomFactor, 1.0));

						const double top = double(sourceTopLeft[n]) * double(leftFactor) + double(sourceTopRight[n]) * rightFactor;
						const double bottom = double(sourceBottomLeft[n]) * double(leftFactor) + double(sourceBottomRight[n]) * rightFactor;

						const double result = top * topFactor + bottom * bottomFactor;

						if (NumericD::isNotEqual(double(targetPixel[n]), result, double(threshold)))
						{
							allSucceeded = false;
						}
					}
				}
			}
		}

		// extra check for corners

		if (memcmp(targetFrame.constpixel<uint8_t>(0u, 0u), sourceTopLeft, sizeof(uint8_t) * channels) != 0)
		{
			allSucceeded = false;
		}

		if (memcmp(targetFrame.constpixel<uint8_t>(targetFrame.width() - 1u, 0u), sourceTopRight, sizeof(uint8_t) * channels) != 0)
		{
			allSucceeded = false;
		}

		if (memcmp(targetFrame.constpixel<uint8_t>(0u, targetFrame.height() - 1u), sourceBottomLeft, sizeof(uint8_t) * channels) != 0)
		{
			allSucceeded = false;
		}

		if (memcmp(targetFrame.constpixel<uint8_t>(targetFrame.width() - 1u, targetFrame.height() - 1u), sourceBottomRight, sizeof(uint8_t) * channels) != 0)
		{
			allSucceeded = false;
		}
	}

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);
		const unsigned int channels = RandomI::random(randomGenerator, 1u, 8u);

		const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		std::vector<uint8_t> interpolationResult(channels + 1u);

		const TScalar xMax = pixelCenter == CV::PC_TOP_LEFT ? TScalar(width - 1u) : TScalar(width);
		const TScalar yMax = pixelCenter == CV::PC_TOP_LEFT ? TScalar(height - 1u) : TScalar(height);

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const VectorT2<TScalar> position(RandomT<TScalar>::scalar(randomGenerator, TScalar(0), xMax), RandomT<TScalar>::scalar(randomGenerator, TScalar(0), yMax));

			for (uint8_t& value : interpolationResult)
			{
				value = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			const uint8_t lastValue = interpolationResult.back();

			if (!CV::FrameInterpolatorBilinear::Comfort::interpolatePixel8BitPerChannel(frame.constdata<uint8_t>(), frame.channels(), frame.width(), frame.height(), frame.paddingElements(), pixelCenter, position, interpolationResult.data()))
			{
				ocean_assert(false && "This should never happen!");
				allSucceeded = false;
			}

			if (lastValue != interpolationResult.back())
			{
				ocean_assert(false && "Invalid padding memory!");
				return false;
			}

			if (!validateInterpolatePixel8BitPerChannel<TScalar>(frame, position, pixelCenter, interpolationResult.data(), threshold))
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrameInterpolatorBilinear::testInterpolatePixel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Pixel interpolation test with floating point precision:";
	Log::info() << " ";

	bool allSucceeded = true;

	for (const CV::PixelCenter pixelCenter : {CV::PC_TOP_LEFT, CV::PC_CENTER})
	{
		Log::info().newLine(pixelCenter != CV::PC_TOP_LEFT);

		allSucceeded = testInterpolatePixel<uint8_t, uint8_t, float>(pixelCenter, testDuration) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testInterpolatePixel<uint8_t, float, float>(pixelCenter, testDuration) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testInterpolatePixel<float, float, float>(pixelCenter, testDuration) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testInterpolatePixel<uint8_t, uint8_t, double>(pixelCenter, testDuration) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testInterpolatePixel<uint8_t, double, double>(pixelCenter, testDuration) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testInterpolatePixel<double, double, double>(pixelCenter, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	if (allSucceeded)
	{
		Log::info() << "Pixel interpolation test succeeded.";
	}
	else
	{
		Log::info() << "Pixel interpolation test FAILED!";
	}

	return allSucceeded;
}

template <typename TSource, typename TTarget, typename TScalar>
bool TestFrameInterpolatorBilinear::testInterpolatePixel(const CV::PixelCenter pixelCenter, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with source '" << TypeNamer::name<TSource>() << "', target '" << TypeNamer::name<TTarget>() << "', scalar type '" << TypeNamer::name<TScalar>() << "' and with pixel center at " << (pixelCenter == CV::PC_TOP_LEFT ? "(0.0, 0.0)" : "(0.5, 0.5)") << ":";

	bool allSucceeded = true;

	constexpr double floatThreshold = 0.1;
	constexpr double integerThreshold = 2.5;

	constexpr double threshold = std::is_floating_point<TTarget>::value ? floatThreshold : integerThreshold;

	RandomGenerator randomGenerator;

	{
		// testing interpolation of a 2x2 image into a larger image

		constexpr unsigned int channels = 3u;

		Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(2u, 2u, FrameType::genericPixelFormat<TSource, channels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		const TSource sourceTopLeft[channels] = {0, 0, 0};
		const TSource sourceTopRight[channels] = {255, 0, 0};
		const TSource sourceBottomLeft[channels] = {0, 255, 0};
		const TSource sourceBottomRight[channels] = {0, 0, 255};

		memcpy(sourceFrame.pixel<TSource>(0u, 0u), sourceTopLeft, sizeof(TSource) * channels);
		memcpy(sourceFrame.pixel<TSource>(1u, 0u), sourceTopRight, sizeof(TSource) * channels);
		memcpy(sourceFrame.pixel<TSource>(0u, 1u), sourceBottomLeft, sizeof(TSource) * channels);
		memcpy(sourceFrame.pixel<TSource>(1u, 1u), sourceBottomRight, sizeof(TSource) * channels);

		const unsigned int dimension = pixelCenter == CV::PC_TOP_LEFT ? 256u : 512u;

		Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(dimension, dimension, FrameType::genericPixelFormat<TTarget, channels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		if (pixelCenter == CV::PC_TOP_LEFT)
		{
			for (unsigned int y = 0u; y < targetFrame.height(); ++y)
			{
				const TScalar yPosition = TScalar(y) / TScalar(targetFrame.height() - 1u); // ensuring to cover full value range [0, 1] == [0, sourceFrame.width() - 1]

				ocean_assert(y != 0u || yPosition == TScalar(0));
				ocean_assert(y != targetFrame.height() - 1u || yPosition == TScalar(1));

				for (unsigned int x = 0u; x < targetFrame.width(); ++x)
				{
					const TScalar xPosition = TScalar(x) / TScalar(targetFrame.width() - 1u);

					ocean_assert(x != 0u || xPosition == TScalar(0));
					ocean_assert(x != targetFrame.width() - 1u || xPosition == TScalar(1));

					ocean_assert(xPosition >= TScalar(0) && xPosition <= TScalar(1));
					ocean_assert(yPosition >= TScalar(0) && yPosition <= TScalar(1));

					if (!CV::FrameInterpolatorBilinear::Comfort::interpolatePixel<TSource, TTarget, TScalar>(sourceFrame.constdata<TSource>(), sourceFrame.channels(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), pixelCenter, VectorT2<TScalar>(xPosition, yPosition), targetFrame.pixel<TTarget>(x, y)))
					{
						ocean_assert(false && "This should never happen!");
						allSucceeded = false;
					}
				}
			}

			for (unsigned int y = 0u; y < targetFrame.height(); ++y)
			{
				const double bottomFactor = double(y) / double(targetFrame.height() - 1u);

				ocean_assert(y != 0u || bottomFactor == 0.0);
				ocean_assert(y != targetFrame.height() - 1u || bottomFactor == 1.0);

				const double topFactor = 1.0 - bottomFactor;

				for (unsigned int x = 0u; x < targetFrame.width(); ++x)
				{
					const double rightFactor = double(x) / double(targetFrame.width() - 1u);

					ocean_assert(x != 0u || rightFactor == 0.0);
					ocean_assert(x != targetFrame.width() - 1u || rightFactor == 1.0);

					const double leftFactor = 1.0 - rightFactor;

					const TTarget* const targetPixel = targetFrame.constpixel<TTarget>(x, y);

					for (unsigned int n = 0u; n < sourceFrame.channels(); ++n)
					{
						ocean_assert(leftFactor >= 0.0 && leftFactor <= 1.0);
						ocean_assert(topFactor >= 0.0 && topFactor <= 1.0);

						ocean_assert(NumericD::isEqual(leftFactor + rightFactor, 1.0));
						ocean_assert(NumericD::isEqual(topFactor + bottomFactor, 1.0));

						const double top = double(sourceTopLeft[n]) * double(leftFactor) + double(sourceTopRight[n]) * rightFactor;
						const double bottom = double(sourceBottomLeft[n]) * double(leftFactor) + double(sourceBottomRight[n]) * rightFactor;

						const double result = top * topFactor + bottom * bottomFactor;

						if (NumericD::isNotEqual(double(targetPixel[n]), result, threshold))
						{
							allSucceeded = false;
						}
					}
				}
			}
		}
		else
		{
			ocean_assert(pixelCenter == CV::PC_CENTER);

			for (unsigned int y = 0u; y < targetFrame.height(); ++y)
			{
				const TScalar yPosition = TScalar(y * 2u) / TScalar(targetFrame.height() - 1u); // ensuring to cover full value range [0, 2] == [0, sourceFrame.width()]

				ocean_assert(y != 0u || yPosition == TScalar(0));
				ocean_assert(y != targetFrame.height() - 1u || yPosition == TScalar(2));

				for (unsigned int x = 0u; x < targetFrame.width(); ++x)
				{
					const TScalar xPosition = TScalar(x * 2u) / TScalar(targetFrame.width() - 1u);

					ocean_assert(x != 0u || xPosition == TScalar(0));
					ocean_assert(x != targetFrame.width() - 1u || xPosition == TScalar(2));

					ocean_assert(xPosition >= TScalar(0) && xPosition <= TScalar(2));
					ocean_assert(yPosition >= TScalar(0) && yPosition <= TScalar(2));

					if (!CV::FrameInterpolatorBilinear::Comfort::interpolatePixel<TSource, TTarget, TScalar>(sourceFrame.constdata<TSource>(), sourceFrame.channels(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), pixelCenter, VectorT2<TScalar>(xPosition, yPosition), targetFrame.pixel<TTarget>(x, y)))
					{
						ocean_assert(false && "This should never happen!");
						allSucceeded = false;
					}
				}
			}

			for (unsigned int y = 0u; y < targetFrame.height(); ++y)
			{
				const double bottomFactor = minmax(0.0, (double(y) / double(targetFrame.height() - 1u)) * 2.0 - 0.5, 1.0);
				const double topFactor = 1.0 - bottomFactor;

				ocean_assert(y != 0u || bottomFactor == 0.0);
				ocean_assert(y != targetFrame.height() - 1u || bottomFactor == 1.0);
				ocean_assert(bottomFactor >= 0.0 && bottomFactor <= 1.0);

				for (unsigned int x = 0u; x < targetFrame.width(); ++x)
				{
					const double rightFactor = minmax(0.0, (double(x) / double(targetFrame.width() - 1u)) * 2.0 - 0.5, 1.0);
					const double leftFactor = 1.0 - rightFactor;

					ocean_assert(x != 0u || rightFactor == 0.0);
					ocean_assert(x != targetFrame.width() - 1u || rightFactor == 1.0);
					ocean_assert(rightFactor >= 0.0 && rightFactor <= 1.0);

					const TTarget* const targetPixel = targetFrame.constpixel<TTarget>(x, y);

					for (unsigned int n = 0u; n < sourceFrame.channels(); ++n)
					{
						ocean_assert(leftFactor >= 0.0 && leftFactor <= 1.0);
						ocean_assert(topFactor >= 0.0 && topFactor <= 1.0);

						ocean_assert(NumericD::isEqual(leftFactor + rightFactor, 1.0));
						ocean_assert(NumericD::isEqual(topFactor + bottomFactor, 1.0));

						const double top = double(sourceTopLeft[n]) * double(leftFactor) + double(sourceTopRight[n]) * rightFactor;
						const double bottom = double(sourceBottomLeft[n]) * double(leftFactor) + double(sourceBottomRight[n]) * rightFactor;

						const double result = top * topFactor + bottom * bottomFactor;

						if (NumericD::isNotEqual(double(targetPixel[n]), result, threshold))
						{
							allSucceeded = false;
						}
					}
				}
			}
		}

		// extra check for corners

		for (unsigned int n = 0u; n < channels; ++n)
		{
			if (targetFrame.constpixel<TTarget>(0u, 0u)[n] != TTarget(sourceTopLeft[n]))
			{
				allSucceeded = false;
			}

			if (targetFrame.constpixel<TTarget>(targetFrame.width() - 1u, 0u)[n] != TTarget(sourceTopRight[n]))
			{
				allSucceeded = false;
			}

			if (targetFrame.constpixel<TTarget>(0u, targetFrame.height() - 1u)[n] != TTarget(sourceBottomLeft[n]))
			{
				allSucceeded = false;
			}

			if (targetFrame.constpixel<TTarget>(targetFrame.width() - 1u, targetFrame.height() - 1u)[n] != TTarget(sourceBottomRight[n]))
			{
				allSucceeded = false;
			}
		}
	}

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);
		const unsigned int channels = RandomI::random(randomGenerator, 1u, 8u);

		const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<TSource>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		std::vector<TTarget> interpolationResult(channels + 1u);

		const TScalar xMax = pixelCenter == CV::PC_TOP_LEFT ? TScalar(width - 1u) : TScalar(width);
		const TScalar yMax = pixelCenter == CV::PC_TOP_LEFT ? TScalar(height - 1u) : TScalar(height);

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const VectorT2<TScalar> position(RandomT<TScalar>::scalar(randomGenerator, TScalar(0), xMax), RandomT<TScalar>::scalar(randomGenerator, TScalar(0), yMax));

			for (TTarget& value : interpolationResult)
			{
				value = TTarget(RandomI::random(randomGenerator, 255u));
			}

			const TTarget lastValue = interpolationResult.back();

			if (!CV::FrameInterpolatorBilinear::Comfort::interpolatePixel<TSource, TTarget, TScalar>(frame.constdata<TSource>(), frame.channels(), frame.width(), frame.height(), frame.paddingElements(), pixelCenter, position, interpolationResult.data()))
			{
				ocean_assert(false && "This should never happen!");
				allSucceeded = false;
			}

			if (lastValue != interpolationResult.back())
			{
				ocean_assert(false && "Invalid padding memory!");
				return false;
			}

			if (!validateInterpolatePixel<TSource, TTarget, TScalar>(frame, position, pixelCenter, interpolationResult.data(), TScalar(threshold)))
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrameInterpolatorBilinear::testAffine(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	const IndexPairs32 dimensions =
	{
		{640u, 480u},
		{800u, 640u},
		{1280u, 720u},
		{1920u, 1080u},
		{3840u, 2160u}
	};

	Log::info() << "Interpolation test for affine transformations (with constant border color):";
	Log::info() << " ";

	bool allSucceeded = true;

	for (const IndexPair32& dimension : dimensions)
	{
		for (unsigned int channel = 1u; channel <= 4u; ++channel)
		{
			allSucceeded = testAffine(dimension.first, dimension.second, channel, testDuration, worker) && allSucceeded;
			Log::info() << " ";
			Log::info() << " ";
		}
	}

	if (allSucceeded)
	{
		Log::info() << "Affine transformations succeeded.";
	}
	else
	{
		Log::info() << "Affine transformations FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestFrameInterpolatorBilinear::testHomography(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	const IndexPairs32 dimensions =
	{
		{640u, 480u},
		{800u, 640u},
		{1280u, 720u},
		{1920u, 1080u}
	};

	Log::info() << "Homography interpolation test (with constant border color) for data type '" << TypeNamer::name<T>() << "':";
	Log::info() << " ";

	bool allSucceeded = true;

	for (const IndexPair32& dimension : dimensions)
	{
		for (unsigned int channel = 1u; channel <= 4u; ++channel)
		{
			allSucceeded = testHomography<T>(dimension.first, dimension.second, channel, testDuration, worker) && allSucceeded;
			Log::info() << " ";
			Log::info() << " ";
		}
	}

	if (allSucceeded)
	{
		Log::info() << "Homography interpolation validation succeeded.";
	}
	else
	{
		Log::info() << "Homography interpolation validation FAILED!";
	}

	return allSucceeded;
}

bool TestFrameInterpolatorBilinear::testHomographyMask(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	const IndexPairs32 dimensions =
	{
		{640u, 480u},
		{1280u, 720u},
		{1920u, 1080u},
		{3840u, 2160u}
	};

	Log::info() << "Homography interpolation test (with binary mask):";
	Log::info() << " ";

	bool allSucceeded = true;

	for (const IndexPair32& dimension : dimensions)
	{
		for (unsigned int channel = 1u; channel <= 4u; ++channel)
		{
			allSucceeded = testHomographyMask(dimension.first, dimension.second, channel, testDuration, worker) && allSucceeded;
			Log::info() << " ";
		}

		Log::info() << " ";
	}

	if (allSucceeded)
	{
		Log::info() << "Homography mask interpolation validation succeeded.";
	}
	else
	{
		Log::info() << "Homography mask interpolation validation FAILED!";
	}

	return allSucceeded;
}

bool TestFrameInterpolatorBilinear::testResizeExtremeResolutions(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Frame resizing for extreme resolutions test:";

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	const double averageErrorThreshold = 5.0;
	const unsigned int maximalErrorThreshold = 256u; // we simply ignore the maximal error
#else
	const double averageErrorThreshold = 1.0;
	const unsigned int maximalErrorThreshold = 2u;
#endif

	unsigned long long iterations = 0ull;

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int sourceWidth = RandomI::random(1u, 64u);
		const unsigned int sourceHeight = RandomI::random(1u, 64u);

		unsigned int targetWidth = RandomI::random(1u, 64u);
		unsigned int targetHeight = RandomI::random(1u, 64u);

		const unsigned int sourcePaddingElements = RandomI::random(0u, 100u);
		const unsigned int targetPaddingElements = RandomI::random(0u, 100u);

		for (unsigned int channels = 1u; channels <= 4u; ++channels)
		{
			Frame sourceFrame(FrameType(sourceWidth, sourceHeight, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
			Frame targetFrame(FrameType(sourceFrame, targetWidth, targetHeight), targetPaddingElements);

			CV::CVUtilities::randomizeFrame(sourceFrame, false);
			CV::CVUtilities::randomizeFrame(targetFrame, false);

			const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			switch (channels)
			{
				case 1u:
					CV::FrameInterpolatorBilinear::resize<uint8_t, 1u>(sourceFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), iterations % 2ull == 0ull ? nullptr : &worker);
					break;

				case 2u:
					CV::FrameInterpolatorBilinear::resize<uint8_t, 2u>(sourceFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), iterations % 2ull == 0ull ? nullptr : &worker);
					break;

				case 3u:
					CV::FrameInterpolatorBilinear::resize<uint8_t, 3u>(sourceFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), iterations % 2ull == 0ull ? nullptr : &worker);
					break;

				case 4u:
					CV::FrameInterpolatorBilinear::resize<uint8_t, 4u>(sourceFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), iterations % 2ull == 0ull ? nullptr : &worker);
					break;

				default:
					ocean_assert(false && "Invalid channel number!");
			}

			if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
			{
				ocean_assert(false && "Invalid padding memory!");
				allSucceeded = false;
				break;
			}

			const double xTargetToSource = double(sourceWidth) / double(targetWidth);
			const double yTargetToSource = double(sourceHeight) / double(targetHeight);

			double averageAbsErrorToInteger = NumericD::maxValue();
			unsigned int maximalAbsErrorToInteger = (unsigned int)(-1);
			validateScaleFrame(sourceFrame.constdata<uint8_t>(), sourceWidth, sourceHeight, channels, targetFrame.constdata<uint8_t>(), targetWidth, targetHeight, xTargetToSource, yTargetToSource, sourceFrame.paddingElements(), targetFrame.paddingElements(), &averageAbsErrorToInteger, &maximalAbsErrorToInteger);

			if (averageAbsErrorToInteger > averageErrorThreshold || maximalAbsErrorToInteger > maximalErrorThreshold)
			{
				allSucceeded = false;
			}
		}

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

bool TestFrameInterpolatorBilinear::testResize(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Frame resizing test (for 8 bit frames):";
	Log::info() << " ";

	bool allSucceeded = true;

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		allSucceeded = testResize(640u, 480u, n, 320u, 240u, testDuration, worker) && allSucceeded;
		Log::info() << " ";

		allSucceeded = testResize(320u, 240u, n, 640u, 480u, testDuration, worker) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		allSucceeded = testResize(641u, 480u, n, 321u, 240u, testDuration, worker) && allSucceeded;
		Log::info() << " ";

		allSucceeded = testResize(321u, 240u, n, 641u, 480u, testDuration, worker) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		allSucceeded = testResize(640u, 481u, n, 320u, 241u, testDuration, worker) && allSucceeded;
		Log::info() << " ";

		allSucceeded = testResize(320u, 241u, n, 640u, 481u, testDuration, worker) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		allSucceeded = testResize(641u, 481u, n, 321u, 241u, testDuration, worker) && allSucceeded;
		Log::info() << " ";

		allSucceeded = testResize(321u, 241u, n, 641u, 481u, testDuration, worker) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		allSucceeded = testResize(731u, 617u, n, 188u, 373u, testDuration, worker) && allSucceeded;
		Log::info() << " ";

		allSucceeded = testResize(188u, 373u, n, 731u, 617u, testDuration, worker) && allSucceeded;
		Log::info() << " ";
	}

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		allSucceeded = testResize(1280u, 720u, n, 1280u, 600u, testDuration, worker) && allSucceeded;
		Log::info() << " ";

		allSucceeded = testResize(1280u, 720u, n, 1000u, 720u, testDuration, worker) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		allSucceeded = testResize(1920u, 1080u, n, 1000u, 700u, testDuration, worker) && allSucceeded;
		Log::info() << " ";

		allSucceeded = testResize(1000u, 700u, n, 1920u, 1080u, testDuration, worker) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		allSucceeded = testResize(1920u, 1080u, n, 1280u, 720u, testDuration, worker) && allSucceeded;
		Log::info() << " ";

		allSucceeded = testResize(1280u, 720u, n, 1920u, 1080u, testDuration, worker) && allSucceeded;
		Log::info() << " ";
	}

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		allSucceeded = testResize(1920u, 1080u, n, 128u, 128u, testDuration, worker) && allSucceeded;
		Log::info() << " ";

		allSucceeded = testResize(128u, 128u, n, 1920u, 1080u, testDuration, worker) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Frame resizing test succeeded.";
	}
	else
	{
		Log::info() << "Frame resizing test FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestFrameInterpolatorBilinear::testResize(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Frame resizing test for data type '" << TypeNamer::name<T>() << "':";
	Log::info() << " ";

	bool allSucceeded = true;

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		allSucceeded = testResize<T>(640u, 480u, n, 320u, 240u, testDuration, worker) && allSucceeded;
		Log::info() << " ";

		allSucceeded = testResize<T>(320u, 240u, n, 640u, 480u, testDuration, worker) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		allSucceeded = testResize<T>(1920u, 1080u, n, 1000u, 700u, testDuration, worker) && allSucceeded;
		Log::info() << " ";

		allSucceeded = testResize<T>(1000u, 700u, n, 1920u, 1080u, testDuration, worker) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		allSucceeded = testResize<T>(1920u, 1080u, n, 1280u, 720u, testDuration, worker) && allSucceeded;
		Log::info() << " ";

		allSucceeded = testResize<T>(1280u, 720u, n, 1920u, 1080u, testDuration, worker) && allSucceeded;
		Log::info() << " ";
	}

	if (allSucceeded)
	{
		Log::info() << "Frame resizing test succeeded.";
	}
	else
	{
		Log::info() << "Frame resizing test FAILED!";
	}

	return allSucceeded;
}

bool TestFrameInterpolatorBilinear::testLookup(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Frame lookup transformation test:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testLookup<uint8_t>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testLookup<float>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Frame lookup transformation succeeded.";
	}
	else
	{
		Log::info() << "Frame lookup transformation FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestFrameInterpolatorBilinear::testLookup(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	const Indices32 widths = {640u, 800u, 1280u, 1920u};
	const Indices32 heights = {480u, 640u, 720u, 1080u};

	Log::info() << "With data type '" << TypeNamer::name<T>() << "':";
	Log::info() << " ";

	bool allSucceeded = true;

	for (unsigned int n = 0u; n < widths.size(); ++n)
	{
		Log::info().newLine(n != 0u);

		for (unsigned int channel = 1u; channel <= 4u; ++channel)
		{
			allSucceeded = testLookup<T>(widths[n], heights[n], channel, testDuration, worker) && allSucceeded;
			Log::info() << " ";
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

bool TestFrameInterpolatorBilinear::testAffine(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(width != 0u && height != 0u && channels != 0u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for a " << width << "x" << height << " frame with " << channels << " channels:";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	HighPerformanceStatistic performanceSingleCore;
	HighPerformanceStatistic performanceMultiCore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (const bool performanceIteration : {true, false})
	{
		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			const Timestamp startTimestamp(true);

			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMultiCore : performanceSingleCore;

			do
			{
				const unsigned int useWidth = performanceIteration ? width : RandomI::random(randomGenerator, 32u, 2048u);
				const unsigned int useHeight = performanceIteration ? height : RandomI::random(randomGenerator, 32u, 2048u);

				const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 128u) * RandomI::random(randomGenerator, 1u);
				const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 128u) * RandomI::random(randomGenerator, 1u);

				Frame source(FrameType(useWidth, useHeight, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
				Frame target(FrameType(useWidth, useHeight, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels), FrameType::ORIGIN_UPPER_LEFT), targetPaddingElements);

				CV::CVUtilities::randomizeFrame(source, /* skip padding */ false, &randomGenerator);
				CV::CVUtilities::randomizeFrame(target, /* skip padding */ false, &randomGenerator);

				// Clones for validation of the padding area
				const Frame clonedTarget(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				// Random affine transformation
				const Vector2 randomTranslation = Random::vector2(randomGenerator, -Scalar(useWidth) * Scalar(0.05), Scalar(useWidth) * Scalar(0.05), -Scalar(useHeight) * Scalar(0.05), Scalar(useHeight) * Scalar(0.05));
				const Vector2 randomScale = Random::vector2(randomGenerator, Scalar(0.90), Scalar(1.1), Scalar(0.90), Scalar(1.1));
				const Scalar randomRotation = Random::scalar(randomGenerator, Numeric::deg2rad(-10), Numeric::deg2rad(10));

				SquareMatrix3 source_A_target(Quaternion(Vector3(0, 0, 1), randomRotation));
				source_A_target(0, 0) *= randomScale.x();
				source_A_target(1, 0) *= randomScale.x();
				source_A_target(0, 1) *= randomScale.y();
				source_A_target(1, 1) *= randomScale.y();
				source_A_target(0, 2) = randomTranslation.x();
				source_A_target(1, 2) = randomTranslation.y();
				ocean_assert(!source_A_target.isSingular());

				std::vector<uint8_t> backgroundColor(channels);

				for (size_t n = 0; n < backgroundColor.size(); ++n)
				{
					backgroundColor[n] = (unsigned char)(RandomI::random(randomGenerator, 255u));
				}

				CV::PixelPositionI targetFrameOriginOffset(RandomI::random(randomGenerator, -5, -5), RandomI::random(randomGenerator, -5, 5));

				performance.start();
				CV::FrameInterpolatorBilinear::Comfort::affine(source, target, source_A_target, backgroundColor.data(), useWorker, targetFrameOriginOffset);
				performance.stop();

				if (!CV::CVUtilities::isPaddingMemoryIdentical(target, clonedTarget))
				{
					Log::error() << "Padding area has been changed - potential memory access violation. Aborting immediately!";
					return false;
				}

				allSucceeded = validateTransformation8BitPerChannel(source, target, source_A_target, backgroundColor.data(), targetFrameOriginOffset) && allSucceeded;
			}
			while (startTimestamp + testDuration > Timestamp(true) || performance.measurements() == 0u);
		}
	}

	Log::info() << "Median performance (single-core): " << String::toAString(performanceSingleCore.medianMseconds(), 3u) << "ms";
	Log::info() << "Median performance (multi-core): " << String::toAString(performanceMultiCore.medianMseconds(), 3u) << "ms";

	Log::info() << "Validation: " << (allSucceeded ? "successful" : "FAILED");

	return allSucceeded;
}

template <typename T>
bool TestFrameInterpolatorBilinear::testHomography(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(width >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for a " << width << "x" << height << " frame with " << channels << " channels:";

	std::vector<T> backgroundColor(channels);

	const Vectors2 outputPoints =
	{
		Vector2(Scalar(0), Scalar(0)),
		Vector2(Scalar(0), Scalar(height)),
		Vector2(Scalar(width), Scalar(height)),
		Vector2(Scalar(width), Scalar(0))
	};

	const Scalar maximalOffsetX = Scalar(width) * Scalar(0.075);
	const Scalar maximalOffsetY = Scalar(height) * Scalar(0.075);

	RandomGenerator randomGenerator;

	double sumAverageError = 0.0;
	double maximalError = 0.0;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		const Timestamp startTimestamp(true);

		do
		{
			Vectors2 inputPoints;

			for (size_t n = 0; n < outputPoints.size(); ++n)
			{
				inputPoints.emplace_back(outputPoints[n] + Random::vector2(randomGenerator, -maximalOffsetX, maximalOffsetX, -maximalOffsetY, maximalOffsetY));
			}

			SquareMatrix3 input_H_output(true);
			if (!Geometry::Homography::homographyMatrixLinear(outputPoints.data(), inputPoints.data(), inputPoints.size(), input_H_output, 10u))
			{
				ocean_assert(false && "This should never happen!");
				maximalError = NumericD::maxValue();
			}

			const unsigned int sourceFramePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			Frame sourceFrame(FrameType(width, height, FrameType::genericPixelFormat<T>(channels), FrameType::ORIGIN_UPPER_LEFT), sourceFramePaddingElements);
			CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator, true);

			const unsigned int targetFramePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			ocean_assert(sourceFrame.width() > 10u);
			Frame targetFrame(FrameType(sourceFrame, RandomI::random(sourceFrame.width() - 10u, sourceFrame.width() + 10u), RandomI::random(sourceFrame.height() - 10u, sourceFrame.height() + 10u)), targetFramePaddingElements);
			CV::CVUtilities::randomizeFrame(targetFrame, false, &randomGenerator, true);

			const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			for (size_t n = 0; n < backgroundColor.size(); ++n)
			{
				if (std::is_signed<T>::value)
				{
					backgroundColor[n] = T(RandomD::scalar(randomGenerator, -255.0, 255.0));
				}
				else
				{
					backgroundColor[n] = T(RandomI::random(randomGenerator, 0u, 255u));
				}
			}

			const CV::PixelPositionI targetFrameOriginOffset(RandomI::random(randomGenerator, -5, 5), RandomI::random(randomGenerator, -5, 5));

			performance.start();
			switch (channels)
			{
				case 1u:
					CV::FrameInterpolatorBilinear::homography<T, 1u>(sourceFrame.constdata<T>(), sourceFrame.width(), sourceFrame.height(), input_H_output, backgroundColor.data(), targetFrame.data<T>(), targetFrameOriginOffset, targetFrame.width(), targetFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
					break;

				case 2u:
					CV::FrameInterpolatorBilinear::homography<T, 2u>(sourceFrame.constdata<T>(), sourceFrame.width(), sourceFrame.height(), input_H_output, backgroundColor.data(), targetFrame.data<T>(), targetFrameOriginOffset, targetFrame.width(), targetFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
					break;

				case 3u:
					CV::FrameInterpolatorBilinear::homography<T, 3u>(sourceFrame.constdata<T>(), sourceFrame.width(), sourceFrame.height(), input_H_output, backgroundColor.data(), targetFrame.data<T>(), targetFrameOriginOffset, targetFrame.width(), targetFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
					break;

				case 4u:
					CV::FrameInterpolatorBilinear::homography<T, 4u>(sourceFrame.constdata<T>(), sourceFrame.width(), sourceFrame.height(), input_H_output, backgroundColor.data(), targetFrame.data<T>(), targetFrameOriginOffset, targetFrame.width(), targetFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
					break;

				default:
					ocean_assert(false && "Invalid channel number!");
			}
			performance.stop();

			if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
			{
				ocean_assert(false && "Invalid padding memory!");
				return false;
			}

			Frame groundTruth(targetFrame.frameType());

			double averageAbsError = NumericD::maxValue();
			double maximalAbsError = NumericD::maxValue();
			validateHomography<T>(sourceFrame, targetFrame, input_H_output, backgroundColor.data(), targetFrameOriginOffset, &averageAbsError, &maximalAbsError, &groundTruth);

			sumAverageError += averageAbsError;
			maximalError = max(maximalError, maximalAbsError);
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Single-core performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceSinglecore.medianMseconds(), 3u) << "ms";

	if (performanceMulticore.measurements() != 0)
	{
		Log::info() << "Multi-core performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceMulticore.medianMseconds(), 3u) << "ms";
		Log::info() << "Multi-core boost factor: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 2u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 2u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 2u) << "x, average: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 2u) << "x";
	}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	const double averageErrorThreshold = 2.0;
	const double maximalErrorThreshold = 10.0;
#else
	const double averageErrorThreshold = 2.0;
	const double maximalErrorThreshold = 5.0;
#endif

	ocean_assert(performanceSinglecore.measurements() != 0);
	const double averageAbsError = sumAverageError / double(performanceSinglecore.measurements());

	const bool allSucceeded = averageAbsError <= averageErrorThreshold && maximalError <= maximalErrorThreshold;

	if (width > 64u)
	{
		Log::info() << "Validation: average error: " << String::toAString(averageAbsError, 2u) << ", maximal error: " << String::toAString(maximalError, 2u);

		if (!allSucceeded)
		{
			Log::info() << "Validation: FAILED!";
		}
	}

	return allSucceeded;
}

bool TestFrameInterpolatorBilinear::testHomographyMask(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(width >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for a " << width << "x" << height << " frame with " << channels << " channels:";

	bool allSucceeded = true;

	const Vectors2 outputPoints =
	{
		Vector2(Scalar(0), Scalar(0)),
		Vector2(Scalar(0), Scalar(height)),
		Vector2(Scalar(width), Scalar(height)),
		Vector2(Scalar(width), Scalar(0))
	};

	const Scalar maximalOffsetX = Scalar(width) * Scalar(0.075);
	const Scalar maximalOffsetY = Scalar(height) * Scalar(0.075);

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		// we use the worker (multi-core execution in the second iteration)
		HighPerformanceStatistic& performance = workerIteration == 0u ? performanceSinglecore : performanceMulticore;
		Worker* useWorker = workerIteration == 0u ? nullptr : &worker;

		const Timestamp startTimestamp(true);

		do
		{
			Vectors2 inputPoints;

			for (size_t n = 0; n < outputPoints.size(); ++n)
			{
				inputPoints.emplace_back(outputPoints[n] + Random::vector2(randomGenerator, -maximalOffsetX, maximalOffsetX, -maximalOffsetY, maximalOffsetY));
			}

			SquareMatrix3 input_H_output(true);
			if (!Geometry::Homography::homographyMatrixLinear(outputPoints.data(), inputPoints.data(), inputPoints.size(), input_H_output, 10u))
			{
				ocean_assert(false && "This should never happen!");
				allSucceeded = false;
			}

			const unsigned int sourceFramePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			Frame sourceFrame(FrameType(width, height, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels), FrameType::ORIGIN_UPPER_LEFT), sourceFramePaddingElements);
			CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);

			ocean_assert(sourceFrame.width() > 10u);
			const unsigned int targetWidth = RandomI::random(randomGenerator, sourceFrame.width() - 10u, sourceFrame.width() + 10u);
			const unsigned int targetHeight = RandomI::random(randomGenerator, sourceFrame.height() - 10u, sourceFrame.height() + 10u);

			const unsigned int targetFramePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			Frame targetFrame(FrameType(sourceFrame, targetWidth, targetHeight), targetFramePaddingElements);
			CV::CVUtilities::randomizeFrame(targetFrame, false, &randomGenerator);

			const unsigned int targetMaskPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			Frame targetMask(FrameType(targetFrame, FrameType::FORMAT_Y8), targetMaskPaddingElements);
			CV::CVUtilities::randomizeFrame(targetMask, false, &randomGenerator);

			const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
			const Frame copyTargetMask(targetMask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			const CV::PixelPositionI targetFrameOriginOffset(RandomI::random(randomGenerator, -5, 5), RandomI::random(randomGenerator, -5, 5));

			constexpr uint8_t maskValue = 0xFF;

			performance.start();
				const bool localResult = CV::FrameInterpolatorBilinear::Comfort::homographyMask(sourceFrame, targetFrame, targetMask, input_H_output, useWorker, maskValue, targetFrameOriginOffset);
			performance.stop();

			if (!localResult)
			{
				allSucceeded = false;
			}

			if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
			{
				ocean_assert(false && "Invalid padding memory!");
			}

			if (!CV::CVUtilities::isPaddingMemoryIdentical(targetMask, copyTargetMask))
			{
				ocean_assert(false && "Invalid padding memory!");
			}

			if (!validateHomographyMask8BitPerChannel(sourceFrame, targetFrame, targetMask, input_H_output, targetFrameOriginOffset))
			{
				allSucceeded = false;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Median performance: " << performanceSinglecore.medianMseconds() << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Median performance (multicore): " << performanceMulticore.medianMseconds() << "ms";
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrameInterpolatorBilinear::testResize(const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourceChannels, const unsigned int targetWidth, const unsigned int targetHeight, const double testDuration, Worker& worker)
{
	ocean_assert(sourceWidth != 0u && sourceHeight != 0u);
	ocean_assert(targetWidth != 0u && targetHeight != 0u);
	ocean_assert(sourceChannels >= 1u && sourceChannels <= 4u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... resizing " << sourceWidth << "x" << sourceHeight << " -> " << targetWidth << "x" << targetHeight << ", " << sourceChannels << " channels:";

	bool allSucceeded = true;

	const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, sourceChannels);

	double sumAverageError = 0.0;
	unsigned int maximalError = 0u;
	unsigned long long measurements = 0ull;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		// we use the worker (multi-core execution in the second iteration)
		HighPerformanceStatistic& performance = workerIteration == 0u ? performanceSinglecore : performanceMulticore;
		Worker* useWorker = workerIteration == 0u ? nullptr : &worker;

		const Timestamp startTimestamp(true);

		do
		{
			const unsigned int sourcePaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);
			const unsigned int targetPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

			Frame sourceFrame(FrameType(sourceWidth, sourceHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
			Frame targetFrame(FrameType(targetWidth, targetHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), targetPaddingElements);

			CV::CVUtilities::randomizeFrame(sourceFrame, false);
			CV::CVUtilities::randomizeFrame(targetFrame, false);

			const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			performance.start();
			switch (sourceChannels)
			{
				case 1u:
					CV::FrameInterpolatorBilinear::resize<uint8_t, 1u>(sourceFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
					break;

				case 2u:
					CV::FrameInterpolatorBilinear::resize<uint8_t, 2u>(sourceFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
					break;

				case 3u:
					CV::FrameInterpolatorBilinear::resize<uint8_t, 3u>(sourceFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
					break;

				case 4u:
					CV::FrameInterpolatorBilinear::resize<uint8_t, 4u>(sourceFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
					break;

				default:
					ocean_assert(false && "Invalid channel number!");
			}
			performance.stop();

			if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
			{
				ocean_assert(false && "Invalid padding memory!");
				allSucceeded = false;
				break;
			}

			const double xTargetToSource = double(sourceWidth) / double(targetWidth);
			const double yTargetToSource = double(sourceHeight) / double(targetHeight);

			double averageAbsErrorToInteger = NumericD::maxValue();
			unsigned int maximalAbsErrorToInteger = (unsigned int)(-1);
			validateScaleFrame(sourceFrame.constdata<uint8_t>(), sourceWidth, sourceHeight, sourceChannels, targetFrame.constdata<uint8_t>(), targetWidth, targetHeight, xTargetToSource, yTargetToSource, sourceFrame.paddingElements(), targetFrame.paddingElements(), &averageAbsErrorToInteger, &maximalAbsErrorToInteger);

			sumAverageError += averageAbsErrorToInteger;
			maximalError = max(maximalError, maximalAbsErrorToInteger);
			measurements++;
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Single-core performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceSinglecore.medianMseconds(), 3u) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multi-core Performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceMulticore.medianMseconds(), 3u) << "ms";
		Log::info() << "Multi-core boost factor: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 2u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 2u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 2u) << "x, median: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 2u) << "x";
	}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	const double averageErrorThreshold = 1.5;
	const unsigned int maximalErrorThreshold = 10u;
#else
	const double averageErrorThreshold = 1.0;
	const unsigned int maximalErrorThreshold = 3u;
#endif

	ocean_assert(measurements != 0ull);
	const double averageAbsErrorToInteger = sumAverageError / double(measurements);

	allSucceeded = allSucceeded && averageAbsErrorToInteger <= averageErrorThreshold && maximalError <= maximalErrorThreshold;

	Log::info() << "Validation: average error: " << String::toAString(averageAbsErrorToInteger, 2u) << ", maximal error: " << maximalError;

	if (!allSucceeded)
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestFrameInterpolatorBilinear::testResize(const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourceChannels, const unsigned int targetWidth, const unsigned int targetHeight, const double testDuration, Worker& worker)
{
	ocean_assert(sourceWidth != 0u && sourceHeight != 0u);
	ocean_assert(targetWidth != 0u && targetHeight != 0u);
	ocean_assert(sourceChannels != 0u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... resizing " << sourceWidth << "x" << sourceHeight << " -> " << targetWidth << "x" << targetHeight << ", " << sourceChannels << " channels:";

	bool allSucceeded = true;

	const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<T>(sourceChannels);

	constexpr unsigned int maximalHorizontalPadding = 100u;

	RandomGenerator randomGenerator;

	double sumAverageError = 0.0;
	double maximalError = 0.0;
	unsigned long long measurements = 0ull;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		// we use the worker (multi-core execution in the second iteration)
		HighPerformanceStatistic& performance = workerIteration == 0u ? performanceSinglecore : performanceMulticore;
		Worker* useWorker = workerIteration == 0u ? nullptr : &worker;

		const Timestamp startTimestamp(true);

		do
		{
			for (unsigned int benchmarkIteration = 0u; benchmarkIteration < 2u; ++benchmarkIteration)
			{
				const bool benchmark = benchmarkIteration == 0u;

				const unsigned int sourcePaddingElements = RandomI::random(0u, maximalHorizontalPadding);
				const unsigned int targetPaddingElements = RandomI::random(0u, maximalHorizontalPadding);

				const unsigned int sourceTestWidth = benchmark ? sourceWidth : RandomI::random(1u, sourceWidth);
				const unsigned int sourceTestHeight = benchmark ? sourceHeight : RandomI::random(1u, sourceHeight);

				const unsigned int targetTestWidth = benchmark ? targetWidth : RandomI::random(1u, targetWidth);
				const unsigned int targetTestHeight = benchmark ? targetHeight : RandomI::random(1u, targetHeight);

				Frame sourceFrame(FrameType(sourceTestWidth, sourceTestHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
				Frame targetFrame(FrameType(targetTestWidth, targetTestHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), targetPaddingElements);

				T* sourceFrameData = sourceFrame.data<T>();
				for (size_t n = 0; n < sourceFrame.strideElements() * sourceFrame.height(); ++n)
				{
					sourceFrameData[n] = T(RandomI::random(-255, 255));
				}

				T* targetFrameData = targetFrame.data<T>();
				for (size_t n = 0; n < targetFrame.strideElements() * targetFrame.height(); ++n)
				{
					targetFrameData[n] = T(RandomI::random(-255, 255));
				}

				const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				performance.start();
				switch (sourceChannels)
				{
					case 1u:
						CV::FrameInterpolatorBilinear::resize<T, 1u>(sourceFrame.constdata<T>(), targetFrame.data<T>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
						break;

					case 2u:
						CV::FrameInterpolatorBilinear::resize<T, 2u>(sourceFrame.constdata<T>(), targetFrame.data<T>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
						break;

					case 3u:
						CV::FrameInterpolatorBilinear::resize<T, 3u>(sourceFrame.constdata<T>(), targetFrame.data<T>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
						break;

					case 4u:
						CV::FrameInterpolatorBilinear::resize<T, 4u>(sourceFrame.constdata<T>(), targetFrame.data<T>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
						break;

					default:
						ocean_assert(false && "Invalid channel number!");
				}
				performance.stop();

				if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					allSucceeded = false;
					break;
				}

				const double xTargetToSource = double(sourceFrame.width()) / double(targetFrame.width());
				const double yTargetToSource = double(sourceFrame.height()) / double(targetFrame.height());

				double averageAbsError = NumericD::maxValue();
				double maximalAbsError = NumericD::maxValue();
				validateScaleFrame<T>(sourceFrame.constdata<T>(), sourceFrame.width(), sourceFrame.height(), sourceChannels, targetFrame.constdata<T>(), targetFrame.width(), targetFrame.height(), xTargetToSource, yTargetToSource, sourceFrame.paddingElements(), targetFrame.paddingElements(), &averageAbsError, &maximalAbsError);

				sumAverageError += averageAbsError;
				maximalError = max(maximalError, maximalAbsError);
				measurements++;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Single-core performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceSinglecore.medianMseconds(), 3u) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multi-core Performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceMulticore.medianMseconds(), 3u) << "ms";
		Log::info() << "Multi-core boost factor: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 2u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 2u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 2u) << "x, median: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 2u) << "x";
	}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	const double averageErrorThreshold = 1.5;
	const double maximalErrorThreshold = 10.0;
#else
	const double averageErrorThreshold = 1.0;
	const double maximalErrorThreshold = 3.0;
#endif

	ocean_assert(measurements != 0ull);
	const double averageAbsError = sumAverageError / double(measurements);

	allSucceeded = allSucceeded && averageAbsError <= averageErrorThreshold && maximalError <= maximalErrorThreshold;

	Log::info() << "Validation: average error: " << String::toAString(averageAbsError, 2u) << ", maximal error: " << String::toAString(maximalError, 2u);

	if (!allSucceeded)
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrameInterpolatorBilinear::testSpecialCasesResize400x400To224x224_8BitPerChannel(const double testDuration)
{
	Log::info() << "Testing special case resize 400x400 to 224x224, FORMAT_Y8:";

	bool allSucceeded = true;

	unsigned int iterations = 0u;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceGeneral;
	HighPerformanceStatistic performanceSpecial;

	const Timestamp startTimestamp(true);

	const FrameType sourceFrameType(400u, 400u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT);
	const FrameType targetFrameType(sourceFrameType, 224u, 224u);

	do
	{
		const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		Frame sourceFrame(sourceFrameType, sourcePaddingElements);

		Frame targetFrameSpecial(targetFrameType, targetPaddingElements);
		Frame targetFrameGeneral(targetFrameType, targetPaddingElements);

		CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(targetFrameSpecial, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(targetFrameGeneral, false, &randomGenerator);

		const Frame copyTargetFrameSpecial(targetFrameSpecial, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		for (const unsigned int iteration : {0u, 1u})
		{
			if (iterations % 2u == iteration) // toggling the execution order
			{
				performanceSpecial.start();
				CV::FrameInterpolatorBilinear::SpecialCases::resize400x400To224x224_8BitPerChannel(sourceFrame.constdata<uint8_t>(), targetFrameSpecial.data<uint8_t>(), sourceFrame.paddingElements(), targetFrameSpecial.paddingElements());
				performanceSpecial.stop();
			}
			else
			{
				performanceGeneral.start();
				CV::FrameInterpolatorBilinear::resize<uint8_t, 1u>(sourceFrame.constdata<uint8_t>(), targetFrameGeneral.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrameGeneral.width(), targetFrameGeneral.height(), sourceFrame.paddingElements(), targetFrameGeneral.paddingElements());
				performanceGeneral.stop();
			}
		}

		if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrameSpecial, copyTargetFrameSpecial))
		{
			ocean_assert(false && "Invalid padding memory!");
			allSucceeded = false;
			break;
		}


		// ensuring bit-precise results between special case and general case

		for (unsigned int y = 0u; y < targetFrameSpecial.height(); ++y)
		{
			if (memcmp(targetFrameSpecial.constrow<void>(y), targetFrameGeneral.constrow<void>(y), targetFrameSpecial.planeWidthBytes(0u)) != 0)
			{
				allSucceeded = false;
			}
		}


		// verifying the 7-bit interpolation accuracy

		const double xSource_s_xTarget = double(sourceFrame.width()) / double(targetFrameSpecial.width());
		const double ySource_s_yTarget = double(sourceFrame.height()) / double(targetFrameSpecial.height());

		double averageAbsError = NumericD::maxValue();
		double maximalAbsError = NumericD::maxValue();
		validateScaleFramePrecision7Bit(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.channels(), targetFrameSpecial.constdata<uint8_t>(), targetFrameSpecial.width(), targetFrameSpecial.height(), xSource_s_xTarget, ySource_s_yTarget, sourceFrame.paddingElements(), targetFrameSpecial.paddingElements(), &averageAbsError, &maximalAbsError);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
		if (averageAbsError != 0.0 || maximalAbsError != 0.0)
		{
			allSucceeded = false;
		}
#else
		if (averageAbsError > 1.0 || maximalAbsError > 3.0)
		{
			allSucceeded = false;
		}
#endif

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "General performance: Best: " << String::toAString(performanceGeneral.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceGeneral.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceGeneral.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceGeneral.medianMseconds(), 3u) << "ms";
	Log::info() << "Special performance: Best: " << String::toAString(performanceSpecial.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSpecial.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSpecial.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceSpecial.medianMseconds(), 3u) << "ms";

	Log::info() << "Special boost: " << String::toAString(performanceGeneral.medianMseconds() / performanceSpecial.medianMseconds(), 2u) << "x";
	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrameInterpolatorBilinear::testSpecialCasesResize400x400To256x256_8BitPerChannel(const double testDuration)
{
	Log::info() << "Testing special case resize 400x400 to 256x256, FORMAT_Y8:";

	bool allSucceeded = true;

	unsigned int iterations = 0u;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceGeneral;
	HighPerformanceStatistic performanceSpecial;

	const Timestamp startTimestamp(true);

	const FrameType sourceFrameType(400u, 400u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT);
	const FrameType targetFrameType(sourceFrameType, 256u, 256u);

	do
	{
		const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		Frame sourceFrame(sourceFrameType, sourcePaddingElements);

		Frame targetFrameSpecial(targetFrameType, targetPaddingElements);
		Frame targetFrameGeneral(targetFrameType, targetPaddingElements);

		CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(targetFrameSpecial, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(targetFrameGeneral, false, &randomGenerator);

		const Frame copyTargetFrameSpecial(targetFrameSpecial, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		for (const unsigned int iteration : {0u, 1u})
		{
			if (iterations % 2u == iteration) // toggling the execution order
			{
				performanceSpecial.start();
				CV::FrameInterpolatorBilinear::SpecialCases::resize400x400To256x256_8BitPerChannel(sourceFrame.constdata<uint8_t>(), targetFrameSpecial.data<uint8_t>(), sourceFrame.paddingElements(), targetFrameSpecial.paddingElements());
				performanceSpecial.stop();
			}
			else
			{
				performanceGeneral.start();
				CV::FrameInterpolatorBilinear::resize<uint8_t, 1u>(sourceFrame.constdata<uint8_t>(), targetFrameGeneral.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrameGeneral.width(), targetFrameGeneral.height(), sourceFrame.paddingElements(), targetFrameGeneral.paddingElements());
				performanceGeneral.stop();
			}
		}

		if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrameSpecial, copyTargetFrameSpecial))
		{
			ocean_assert(false && "Invalid padding memory!");

			allSucceeded = false;
			break;
		}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
		// ensuring bit-precise results between special case and general case, x86's general interpolation is slightly different (more precise) so we cannot ensure bit-precise results

		for (unsigned int y = 0u; y < targetFrameSpecial.height(); ++y)
		{
			if (memcmp(targetFrameSpecial.constrow<void>(y), targetFrameGeneral.constrow<void>(y), targetFrameSpecial.planeWidthBytes(0u)) != 0)
			{
				allSucceeded = false;
			}
		}

		const double threshold = 0.0;
#else
		const double threshold = 1.0;
#endif

		// verifying the 7-bit interpolation accuracy

		const double xSource_s_xTarget = double(sourceFrame.width()) / double(targetFrameSpecial.width());
		const double ySource_s_yTarget = double(sourceFrame.height()) / double(targetFrameSpecial.height());

		double averageAbsError = NumericD::maxValue();
		double maximalAbsError = NumericD::maxValue();
		validateScaleFramePrecision7Bit(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.channels(), targetFrameSpecial.constdata<uint8_t>(), targetFrameSpecial.width(), targetFrameSpecial.height(), xSource_s_xTarget, ySource_s_yTarget, sourceFrame.paddingElements(), targetFrameSpecial.paddingElements(), &averageAbsError, &maximalAbsError);

		if (averageAbsError > threshold || maximalAbsError > threshold)
		{
			allSucceeded = false;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "General performance: Best: " << String::toAString(performanceGeneral.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceGeneral.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceGeneral.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceGeneral.medianMseconds(), 3u) << "ms";
	Log::info() << "Special performance: Best: " << String::toAString(performanceSpecial.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSpecial.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSpecial.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceSpecial.medianMseconds(), 3u) << "ms";

	Log::info() << "Special boost: " << String::toAString(performanceGeneral.medianMseconds() / performanceSpecial.medianMseconds(), 2u) << "x";
	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestFrameInterpolatorBilinear::testLookup(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width >= 20u && height >= 20u);
	ocean_assert(channels >= 1u);

	Log::info() << "... with a " << width << "x" << height << " frame and " << channels << " channels:";
	Log::info() << " ";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		// we use the worker (multi-core execution in the second iteration)
		HighPerformanceStatistic& performance = workerIteration == 0u ? performanceSinglecore : performanceMulticore;
		Worker* useWorker = workerIteration == 0u ? nullptr : &worker;

		const Timestamp startTimestamp(true);

		do
		{
			ocean_assert(width >= 4u && height >= 4u);

			const unsigned int testWidth = (unsigned int)(int(width) + RandomI::random(randomGenerator, -3, 3));
			const unsigned int testHeight = (unsigned int)(int(height) + RandomI::random(randomGenerator, -3, 3));

			CV::FrameInterpolatorBilinear::LookupTable lookupTable(testWidth, testHeight, 20u, 20u);

			const bool offset = RandomI::random(randomGenerator, 1u) == 0u;

			for (unsigned int yBin = 0u; yBin <= lookupTable.binsY(); ++yBin)
			{
				for (unsigned int xBin = 0u; xBin <= lookupTable.binsX(); ++xBin)
				{
					Vector2 value = Random::vector2(randomGenerator, -10, 10);

					if (!offset)
					{
						value += Vector2(lookupTable.binTopLeftCornerPosition(xBin, yBin));
					}

					lookupTable.setBinTopLeftCornerValue(xBin, yBin, value);
				}
			}

			std::vector<T> backgroundColor(channels);

			for (unsigned int n = 0u; n < channels; ++n)
			{
				backgroundColor[n] = T(RandomI::random(randomGenerator, 255u));
			}

			const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<T>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
			Frame targetFrame = CV::CVUtilities::randomizedFrame(sourceFrame.frameType(), &randomGenerator);

			const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			performance.start();
			switch (channels)
			{
				case 1u:
					CV::FrameInterpolatorBilinear::lookup<T, 1u>(sourceFrame.constdata<T>(), sourceFrame.width(), sourceFrame.height(), lookupTable, offset, backgroundColor.data(), targetFrame.data<T>(), sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
					break;

				case 2u:
					CV::FrameInterpolatorBilinear::lookup<T, 2u>(sourceFrame.constdata<T>(), sourceFrame.width(), sourceFrame.height(), lookupTable, offset, backgroundColor.data(), targetFrame.data<T>(), sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
					break;

				case 3u:
					CV::FrameInterpolatorBilinear::lookup<T, 3u>(sourceFrame.constdata<T>(), sourceFrame.width(), sourceFrame.height(), lookupTable, offset, backgroundColor.data(), targetFrame.data<T>(), sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
					break;

				case 4u:
					CV::FrameInterpolatorBilinear::lookup<T, 4u>(sourceFrame.constdata<T>(), sourceFrame.width(), sourceFrame.height(), lookupTable, offset, backgroundColor.data(), targetFrame.data<T>(), sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
					break;

				default:
					ocean_assert(false && "Invalid channel number!");
					allSucceeded = false;
					break;
			}
			performance.stop();

			if (CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame) == false)
			{
				ocean_assert(false && "This must never happen!");
				allSucceeded = false;
				break;
			}

			if (!validateLookup<T>(sourceFrame, targetFrame, lookupTable, offset, backgroundColor.data()))
			{
				allSucceeded = false;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Single-core performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceSinglecore.medianMseconds(), 3u) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multi-core Performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceMulticore.medianMseconds(), 3u) << "ms";
		Log::info() << "Multi-core boost factor: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 2u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 2u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 2u) << "x, median: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 2u) << "x";
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrameInterpolatorBilinear::testResampleCameraImage(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing Comfort::resampleCameraImage()";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testResampleCameraImage<uint8_t>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testResampleCameraImage<float>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Re-sample succeeded.";
	}
	else
	{
		Log::info() << "Re-sample FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestFrameInterpolatorBilinear::testResampleCameraImage(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with element type '" << TypeNamer::name<T>() << "':";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = workerIteration == 0u ? nullptr : &worker;

		const Timestamp startTimestamp(true);

		do
		{
			const unsigned int width = RandomI::random(randomGenerator, 200u, 1000u);
			const unsigned int height = RandomI::random(randomGenerator, 200u, 1000u);

			const unsigned int channels = RandomI::random(randomGenerator, 1u, 4u);

			const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<T>(channels);

			Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			if (!CV::FrameFilterGaussian::filter(sourceFrame, 7u, &worker))
			{
				ocean_assert(false && "This should never happen!");
				allSucceeded = false;
			}

			Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			const Scalar fovX = Numeric::deg2rad(70);

			const AnyCameraPinhole sourceCamera(PinholeCamera(width, height, fovX));

			const int xOffset = RandomI::random(randomGenerator, -100, 100);
			const int yOffset = RandomI::random(randomGenerator, -100, 100);

			const AnyCameraPinhole targetCamera(PinholeCamera(width, height, fovX, Scalar(width) * Scalar(0.5) + Scalar(xOffset), Scalar(height) * Scalar(0.5) + Scalar(yOffset)));

			const unsigned int binSize = RandomI::random(randomGenerator, 1u, 16u);

			if (CV::FrameInterpolatorBilinear::Comfort::resampleCameraImage(sourceFrame, sourceCamera, SquareMatrix3(true), targetCamera, targetFrame, nullptr, useWorker, binSize))
			{
				double sumDifference = 0.0;
				uint64_t pixels = 0ull;

				for (unsigned int yTarget = 0u; yTarget < targetFrame.height(); ++yTarget)
				{
					const int ySource = int(yTarget) - yOffset;

					if (ySource >= 0 && ySource < int(sourceFrame.height()))
					{
						for (unsigned int xTarget = 0u; xTarget < targetFrame.width(); ++xTarget)
						{
							const int xSource = int(xTarget) - xOffset;

							if (xSource >= 0 && xSource < int(sourceFrame.width()))
							{
								const T* sourcePixel = sourceFrame.constpixel<T>(xSource, ySource);
								const T* targetPixel = targetFrame.constpixel<T>(xTarget, yTarget);

								for (unsigned int n = 0u; n < channels; ++n)
								{
									sumDifference += std::abs(double(sourcePixel[n]) - double(targetPixel[n]));
								}

								++pixels;
							}
						}
					}
				}

				ocean_assert(pixels != 0ull);

				if (pixels != 0ull)
				{
					const double averageError = sumDifference / double(pixels * channels);
					ocean_assert(averageError >= 0.0);

					if (averageError > 2)
					{
						allSucceeded = false;
					}
				}
				else
				{
					ocean_assert(false && "This should never happen!");
					allSucceeded = false;
				}
			}
			else
			{
				ocean_assert(false && "This should never happen!");
				allSucceeded = false;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrameInterpolatorBilinear::testLookupMask(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Frame mask lookup transformation on a " << width << "x" << height << " frame:";
	Log::info() << " ";

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	bool allSucceeded = true;

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		Log::info() << "... with " << n << " channels:";

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			const Timestamp startTimestamp(true);

			do
			{
				const bool offset = RandomI::random(1u) == 1u;

				CV::FrameInterpolatorBilinear::LookupTable lookupTable(width, height, 20u, 20u);

				for (unsigned int yBin = 0u; yBin <= lookupTable.binsY(); ++yBin)
				{
					for (unsigned int xBin = 0u; xBin <= lookupTable.binsX(); ++xBin)
					{
						Vector2 value = Random::vector2(-10, 10);

						if (!offset)
						{
							value += Vector2(lookupTable.binTopLeftCornerPosition(xBin, yBin));
						}

						lookupTable.setBinTopLeftCornerValue(xBin, yBin, value);
					}
				}

				const unsigned int framePaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);
				const unsigned int targetPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);
				const unsigned int targetMaskPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

				Frame sourceFrame(FrameType(width, height, FrameType::findPixelFormat(n * 8), FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);
				Frame targetFrame(sourceFrame.frameType(), targetPaddingElements);
				Frame targetMask(FrameType(targetFrame, FrameType::FORMAT_Y8), targetMaskPaddingElements);

				CV::CVUtilities::randomizeFrame(sourceFrame, false);
				CV::CVUtilities::randomizeFrame(targetFrame, false);
				CV::CVUtilities::randomizeFrame(targetMask, false);

				const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
				const Frame copyTargetMask(targetMask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				performance.start();
					CV::FrameInterpolatorBilinear::Comfort::lookupMask(sourceFrame, targetFrame, targetMask, lookupTable, offset, useWorker);
				performance.stop();

				if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (!CV::CVUtilities::isPaddingMemoryIdentical(targetMask, copyTargetMask))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (!validateLookupMask(sourceFrame, targetFrame, targetMask, lookupTable, offset))
				{
					allSucceeded = false;
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		Log::info() << "Single-core performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceSinglecore.medianMseconds(), 3u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multi-core performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceMulticore.medianMseconds(), 3u) << "ms";
			Log::info() << "Multi-core boost factor: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 2u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 2u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 2u) << "x, average: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 2u) << "x";
		}

		Log::info() << " ";
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrameInterpolatorBilinear::testRotateFrame(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test rotation of frame with resolution " << width << "x" << height << ":";
	Log::info() << " ";

	bool allSucceeded = true;

	for (unsigned int channels = 1u; channels <= 4u; ++channels)
	{
		Log::info().newLine(channels != 0u);

		Log::info() << "... for " << channels << " channels:";

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		const unsigned int maxWorkerIterations = worker ? 2u : 1u;

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			const Timestamp startTimestamp(true);

			do
			{
				for (const bool performanceIteration : {true, false})
				{
					const unsigned int testWidth = performanceIteration ? width : RandomI::random(1u, 100u);
					const unsigned int testHeight = performanceIteration ? height : RandomI::random(1u, 100u);

					const unsigned int sourcePaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);
					const unsigned int targetPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

					Frame sourceFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
					Frame targetFrame(sourceFrame.frameType(), targetPaddingElements);

					CV::CVUtilities::randomizeFrame(sourceFrame, false);
					CV::CVUtilities::randomizeFrame(targetFrame, false);

					const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					const Scalar anchorX = Random::scalar(-20, Scalar(sourceFrame.width()) + 20);
					const Scalar anchorY = Random::scalar(-20, Scalar(sourceFrame.height()) + 20);

					const Scalar angle = Random::scalar(0, Numeric::pi2());

					performance.startIf(performanceIteration);
					if (!CV::FrameInterpolatorBilinear::Comfort::rotate(sourceFrame, targetFrame, anchorX, anchorY, angle, useWorker, nullptr))
					{
						allSucceeded = false;
					}
					performance.stopIf(performanceIteration);

					if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					if (!validateRotatedFrame(sourceFrame, targetFrame, anchorX, anchorY, angle))
					{
						allSucceeded = false;
					}
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		Log::info() << "Single-core performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceSinglecore.medianMseconds(), 3u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multi-core performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceMulticore.medianMseconds(), 3u) << "ms";
			Log::info() << "Multi-core boost factor: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 2u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 2u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 2u) << "x, average: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 2u) << "x";
		}
	}

	Log::info().newLine();

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrameInterpolatorBilinear::testPatchIntensitySum1Channel(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width >= 64u && height >= 64u);

	Log::info() << "Testing intensity sum of interpolated patch:";
	Log::info() << " ";

	const IndexPairs32 patchSizes =
	{
		{1u, 1u},
		{9u, 1u},
		{1u, 9u},
		{5u, 5u},
		{10u, 10u},
		{31u, 31u},
		{64u, 64u},
		{RandomI::random(1u, 64u), RandomI::random(1u, 64u)}
	};

	bool allSucceeded = true;

	for (const IndexPair32& patchSize : patchSizes)
	{
		allSucceeded = testPatchIntensitySum1Channel(width, height, patchSize.first, patchSize.second, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

void TestFrameInterpolatorBilinear::validateScaleFrame(const unsigned char* source, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int channels, const unsigned char* target, const unsigned int targetWidth, const unsigned int targetHeight, const double xTargetToSource, const double yTargetToSource, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, double* averageAbsErrorToInteger, unsigned int* maximalAbsErrorToInteger, unsigned char* groundTruth)
{
	ocean_assert(source && target);
	ocean_assert(sourceWidth != 0u && sourceHeight != 0u);
	ocean_assert(targetWidth != 0u && targetHeight != 0u);
	ocean_assert(channels >= 1u);
	ocean_assert(xTargetToSource > 0.0 && yTargetToSource > 0.0);

	const unsigned int sourceStrideElements = sourceWidth * channels + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * channels + targetPaddingElements;

	std::vector<unsigned char> result(channels, 0x00);

	if (averageAbsErrorToInteger)
	{
		*averageAbsErrorToInteger = NumericD::maxValue();
	}

	if (maximalAbsErrorToInteger)
	{
		*maximalAbsErrorToInteger = (unsigned int)(-1);
	}

	double sumAbsError = 0.0;
	unsigned int maxAbsError = 0u;

	for (unsigned int y = 0u; y < targetHeight; ++y)
	{
		for (unsigned int x = 0u; x < targetWidth; ++x)
		{
			const double sx = minmax(0.0, (double(x) + 0.5) * xTargetToSource - 0.5, double(sourceWidth - 1u));
			const double sy = minmax(0.0, (double(y) + 0.5) * yTargetToSource - 0.5, double(sourceHeight - 1u));

			const unsigned int leftPixel = (unsigned int)sx;
			const unsigned int rightPixel = min(leftPixel + 1u, sourceWidth - 1u);

			const unsigned int topPixel = (unsigned int)sy;
			const unsigned int bottomPixel = min(topPixel + 1u, sourceHeight - 1u);

			const double rightFactor = sx - double(leftPixel);
			const double bottomFactor = sy - double(topPixel);

			ocean_assert(rightFactor >= 0.0 && rightFactor <= 1.0);
			ocean_assert(bottomFactor >= 0.0 && bottomFactor <= 1.0);

			const double leftFactor = 1.0 - rightFactor;
			const double topFactor = 1.0 - bottomFactor;

			const unsigned char* sourceTopLeft = source + sourceStrideElements * topPixel + leftPixel * channels;
			const unsigned char* sourceTopRight = source + sourceStrideElements * topPixel + rightPixel * channels;

			const unsigned char* sourceBottomLeft = source + sourceStrideElements * bottomPixel + leftPixel * channels;
			const unsigned char* sourceBottomRight = source + sourceStrideElements * bottomPixel + rightPixel * channels;

			for (unsigned int n = 0u; n < channels; ++n)
			{
				const double top = double(sourceTopLeft[n]) * leftFactor + double(sourceTopRight[n]) * rightFactor;
				const double bottom = double(sourceBottomLeft[n]) * leftFactor + double(sourceBottomRight[n]) * rightFactor;

				const double interpolated = top * topFactor + bottom * bottomFactor;
				ocean_assert(interpolated >= 0.0 && interpolated < 255.5);

				result[n] = (unsigned char)(interpolated + 0.5);
			}

			const unsigned char* const targetValue = target + targetStrideElements * y + x * channels;

			for (unsigned int n = 0u; n < channels; ++n)
			{
				const unsigned int absError = (unsigned int)abs(int(result[n]) - int(targetValue[n]));

				sumAbsError += double(absError);
				maxAbsError = max(maxAbsError, absError);
			}

			if (groundTruth)
			{
				memcpy(groundTruth + (y * targetWidth + x) * channels, result.data(), sizeof(unsigned char) * channels);
			}
		}
	}

	if (averageAbsErrorToInteger)
	{
		*averageAbsErrorToInteger = sumAbsError / double(targetWidth * targetHeight * channels);
	}

	if (maximalAbsErrorToInteger)
	{
		*maximalAbsErrorToInteger = maxAbsError;
	}
}

bool TestFrameInterpolatorBilinear::testPatchIntensitySum1Channel(const unsigned int width, const unsigned int height, const unsigned int patchWidth, const unsigned int patchHeight, const double testDuration)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width > 0u && height > 0u);
	ocean_assert(patchWidth > 0u && patchHeight > 0u);

	constexpr size_t numberPoints = 1000;

	Log::info() << "... with patch size " << patchWidth << "x" << patchHeight << " and " << numberPoints << " points:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		for (const CV::PixelCenter pixelCenter : {CV::PC_TOP_LEFT, CV::PC_CENTER})
		{
			const Frame yFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			const Frame linedIntegralImage = CV::IntegralImage::Comfort::createLinedImage(yFrame);
			ocean_assert(linedIntegralImage.dataType() == FrameType::DT_UNSIGNED_INTEGER_32);

			Vectors2 points;
			points.reserve(numberPoints);

			const Scalar patchWidth_2 = Scalar(patchWidth) / Scalar(2);
			const Scalar patchHeight_2 = Scalar(patchHeight) / Scalar(2);

			const Scalar maxRangeOffset = pixelCenter == CV::PC_TOP_LEFT ? Scalar(1) : Numeric::weakEps();

			const Scalar xMaxRange = Scalar(width) - patchWidth_2 - maxRangeOffset;
			const Scalar yMaxRange = Scalar(height) - patchHeight_2 - maxRangeOffset;

			// add extreme positions
			points.emplace_back(patchWidth_2, patchHeight_2);
			points.emplace_back(patchWidth_2, yMaxRange);
			points.emplace_back(xMaxRange, yMaxRange);
			points.emplace_back(xMaxRange, patchHeight_2);

			points.emplace_back(patchWidth_2, Scalar(height) * Scalar(0.5));
			points.emplace_back(xMaxRange, Scalar(height) * Scalar(0.5));

			points.emplace_back(Scalar(width) * Scalar(0.5), patchHeight_2);
			points.emplace_back(Scalar(width) * Scalar(0.5), yMaxRange);

			while (points.size() < numberPoints)
			{
				points.emplace_back(Random::vector2(randomGenerator, patchWidth_2, xMaxRange, patchHeight_2, yMaxRange));
			}

			Scalars intensities(numberPoints);

			const uint32_t* linedIntegralImageData = linedIntegralImage.constdata<uint32_t>();
			const unsigned int linedIntegraleImagePaddingElements = linedIntegralImage.paddingElements();

			performance.start();

				for (size_t n = 0; n < numberPoints; ++n)
				{
					intensities[n] = CV::FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralImageData, width, height, linedIntegraleImagePaddingElements, points[n], pixelCenter, patchWidth, patchHeight);
				}

			performance.stop();

			for (size_t n = 0; n < numberPoints; ++n)
			{
				if (!validatePatchIntensitySum1Channel(yFrame, patchWidth, patchHeight, points[n], pixelCenter, intensities[n]))
				{
					allSucceeded = false;
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: Best: " << performance.bestMseconds() << "ms, worst: " << performance.worstMseconds() << "ms, average: " << performance.averageMseconds() << "ms, median: " << performance.medianMseconds() << "ms";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

template <typename TScalar>
bool TestFrameInterpolatorBilinear::validateInterpolatePixel8BitPerChannel(const Frame& frame, const VectorT2<TScalar>& position, const CV::PixelCenter pixelCenter, const uint8_t* const interpolationResult, const TScalar threshold)
{
	ocean_assert(frame.isValid());
	ocean_assert(threshold >= 0 && threshold < 255);

	unsigned int leftPixel = (unsigned int)(-1);
	unsigned int topPixel = (unsigned int)(-1);

	TScalar factorRight = NumericT<TScalar>::minValue();
	TScalar factorBottom = NumericT<TScalar>::minValue();

	if (pixelCenter == CV::PC_TOP_LEFT)
	{
		ocean_assert(position.x() >= TScalar(0) && position.x() <= TScalar(frame.width() - 1u));
		ocean_assert(position.y() >= TScalar(0) && position.y() <= TScalar(frame.height() - 1u));

		if (position.x() < TScalar(0) || position.x() > TScalar(frame.width() - 1u))
		{
			return false;
		}

		if (position.y() < TScalar(0) || position.y() > TScalar(frame.height() - 1u))
		{
			return false;
		}

		leftPixel = (unsigned int)(position.x());
		topPixel = (unsigned int)(position.y());

		factorRight = position.x() - TScalar(leftPixel);
		factorBottom = position.y() - TScalar(topPixel);
	}
	else
	{
		ocean_assert(pixelCenter == CV::PC_CENTER);

		ocean_assert(position.x() >= TScalar(0) && position.x() <= TScalar(frame.width()));
		ocean_assert(position.y() >= TScalar(0) && position.y() <= TScalar(frame.height()));

		if (position.x() < TScalar(0) || position.x() > TScalar(frame.width()))
		{
			return false;
		}

		if (position.y() < TScalar(0) || position.y() > TScalar(frame.height()))
		{
			return false;
		}

		const VectorT2<TScalar> shiftedPosition = position - VectorT2<TScalar>(TScalar(0.5), TScalar(0.5));
		const VectorT2<TScalar> clampedShiftedPosition(std::max(TScalar(0), shiftedPosition.x()), std::max(TScalar(0), shiftedPosition.y()));

		leftPixel = (unsigned int)(clampedShiftedPosition.x());
		topPixel = (unsigned int)(clampedShiftedPosition.y());

		factorRight = clampedShiftedPosition.x() - TScalar(leftPixel);
		factorBottom = clampedShiftedPosition.y() - TScalar(topPixel);
	}

	ocean_assert(leftPixel < frame.width());
	ocean_assert(topPixel < frame.height());

	const unsigned int rightPixel = min(leftPixel + 1u, frame.width() - 1u);
	const unsigned int bottomPixel = min(topPixel + 1u, frame.height() - 1u);

	ocean_assert(factorRight >= TScalar(0) && factorRight <= TScalar(1));
	ocean_assert(factorBottom >= TScalar(0) && factorBottom <= TScalar(1));

	const TScalar factorLeft = TScalar(1) - factorRight;
	const TScalar factorTop = TScalar(1) - factorBottom;

	const uint8_t* pixelTopLeft = frame.constpixel<uint8_t>(leftPixel, topPixel);
	const uint8_t* pixelTopRight = frame.constpixel<uint8_t>(rightPixel, topPixel);
	const uint8_t* pixelBottomLeft = frame.constpixel<uint8_t>(leftPixel, bottomPixel);
	const uint8_t* pixelBottomRight = frame.constpixel<uint8_t>(rightPixel, bottomPixel);

	for (unsigned int nChannel = 0u; nChannel < frame.channels(); ++nChannel)
	{
		const TScalar topResult = TScalar(pixelTopLeft[nChannel]) * factorLeft + TScalar(pixelTopRight[nChannel]) * factorRight;

		const TScalar bottomResult = TScalar(pixelBottomLeft[nChannel]) * factorLeft + TScalar(pixelBottomRight[nChannel]) * factorRight;

		const TScalar result = topResult * factorTop + bottomResult * factorBottom;

		if (NumericT<TScalar>::isNotEqual(TScalar(interpolationResult[nChannel]), result, threshold))
		{
			return false;
		}
	}

	return true;
}

template <typename TSource, typename TTarget, typename TScalar>
bool TestFrameInterpolatorBilinear::validateInterpolatePixel(const Frame& frame, const VectorT2<TScalar>& position, const CV::PixelCenter pixelCenter, const TTarget* const interpolationResult, const TScalar threshold)
{
	ocean_assert(frame.isValid());
	ocean_assert(frame.dataType() == FrameType::dataType<TSource>());
	ocean_assert(threshold >= 0 && threshold < 255);

	unsigned int leftPixel = (unsigned int)(-1);
	unsigned int topPixel = (unsigned int)(-1);

	TScalar factorRight = NumericT<TScalar>::minValue();
	TScalar factorBottom = NumericT<TScalar>::minValue();

	if (pixelCenter == CV::PC_TOP_LEFT)
	{
		ocean_assert(position.x() >= TScalar(0) && position.x() <= TScalar(frame.width() - 1u));
		ocean_assert(position.y() >= TScalar(0) && position.y() <= TScalar(frame.height() - 1u));

		if (position.x() < TScalar(0) || position.x() > TScalar(frame.width() - 1u))
		{
			return false;
		}

		if (position.y() < TScalar(0) || position.y() > TScalar(frame.height() - 1u))
		{
			return false;
		}

		leftPixel = (unsigned int)(position.x());
		topPixel = (unsigned int)(position.y());

		factorRight = position.x() - TScalar(leftPixel);
		factorBottom = position.y() - TScalar(topPixel);
	}
	else
	{
		ocean_assert(pixelCenter == CV::PC_CENTER);

		ocean_assert(position.x() >= TScalar(0) && position.x() <= TScalar(frame.width()));
		ocean_assert(position.y() >= TScalar(0) && position.y() <= TScalar(frame.height()));

		if (position.x() < TScalar(0) || position.x() > TScalar(frame.width()))
		{
			return false;
		}

		if (position.y() < TScalar(0) || position.y() > TScalar(frame.height()))
		{
			return false;
		}

		const VectorT2<TScalar> shiftedPosition = position - VectorT2<TScalar>(TScalar(0.5), TScalar(0.5));
		const VectorT2<TScalar> clampedShiftedPosition(std::max(TScalar(0), shiftedPosition.x()), std::max(TScalar(0), shiftedPosition.y()));

		leftPixel = (unsigned int)(clampedShiftedPosition.x());
		topPixel = (unsigned int)(clampedShiftedPosition.y());

		factorRight = clampedShiftedPosition.x() - TScalar(leftPixel);
		factorBottom = clampedShiftedPosition.y() - TScalar(topPixel);
	}

	ocean_assert(leftPixel < frame.width());
	ocean_assert(topPixel < frame.height());

	const unsigned int rightPixel = min(leftPixel + 1u, frame.width() - 1u);
	const unsigned int bottomPixel = min(topPixel + 1u, frame.height() - 1u);

	ocean_assert(factorRight >= TScalar(0) && factorRight <= TScalar(1));
	ocean_assert(factorBottom >= TScalar(0) && factorBottom <= TScalar(1));

	const TScalar factorLeft = TScalar(1) - factorRight;
	const TScalar factorTop = TScalar(1) - factorBottom;

	for (unsigned int nChannel = 0u; nChannel < frame.channels(); ++nChannel)
	{
		const TSource* pixelTopLeft = frame.constpixel<TSource>(leftPixel, topPixel);
		const TSource* pixelTopRight = frame.constpixel<TSource>(rightPixel, topPixel);
		const TSource* pixelBottomLeft = frame.constpixel<TSource>(leftPixel, bottomPixel);
		const TSource* pixelBottomRight = frame.constpixel<TSource>(rightPixel, bottomPixel);

		const TScalar topResult = TScalar(pixelTopLeft[nChannel]) * factorLeft + TScalar(pixelTopRight[nChannel]) * factorRight;

		const TScalar bottomResult = TScalar(pixelBottomLeft[nChannel]) * factorLeft + TScalar(pixelBottomRight[nChannel]) * factorRight;

		const TScalar result = topResult * factorTop + bottomResult * factorBottom;

		if (NumericT<TScalar>::isNotEqual(TScalar(interpolationResult[nChannel]), result, threshold))
		{
			return false;
		}
	}

	return true;
}

bool TestFrameInterpolatorBilinear::validateTransformation8BitPerChannel(const Frame& source, const Frame& validationTarget, const SquareMatrix3& source_H_target, const uint8_t* backgroundColor, const CV::PixelPositionI& validationTargetOrigin)
{
	ocean_assert(source && validationTarget);

	ocean_assert(source.pixelFormat() == validationTarget.pixelFormat() && source.pixelOrigin() == validationTarget.pixelOrigin());
	ocean_assert(source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	ocean_assert(!source_H_target.isSingular());
	ocean_assert(backgroundColor != nullptr);

	ocean_assert(validationTarget.channels() <= 4u);
	uint8_t pixelResults[4] = {0x00u, 0x00u, 0x00u, 0x00u};

	unsigned int invalidForegroundValues = 0u;
	unsigned int invalidBackgroundValues = 0u;

	const SquareMatrix3 source_H_adjustedTarget = source_H_target * SquareMatrix3(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(Scalar(validationTargetOrigin.x()), Scalar(validationTargetOrigin.y()), 1));

	for (unsigned int y = 0u; y < validationTarget.height(); ++y)
	{
		for (unsigned int x = 0u; x < validationTarget.width(); ++x)
		{
			const Vector2 validationTargetPosition = Vector2(Scalar(x), Scalar(y));
			const Vector2 sourcePosition = source_H_adjustedTarget * validationTargetPosition;

			const uint8_t* const validationTargetPixel = validationTarget.constpixel<uint8_t>(x, y);

			if (sourcePosition.x() >= 0 && sourcePosition.y() >= 0 && sourcePosition.x() <= Scalar(source.width() - 1u) && sourcePosition.y() <= Scalar(source.height() - 1u))
			{
				switch (source.channels())
				{
					case 1u:
						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<1u, CV::PC_TOP_LEFT>(source.constdata<uint8_t>(), source.width(), source.height(), source.paddingElements(), sourcePosition, pixelResults);
						break;

					case 2u:
						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<2u, CV::PC_TOP_LEFT>(source.constdata<uint8_t>(), source.width(), source.height(), source.paddingElements(), sourcePosition, pixelResults);
						break;

					case 3u:
						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<3u, CV::PC_TOP_LEFT>(source.constdata<uint8_t>(), source.width(), source.height(), source.paddingElements(), sourcePosition, pixelResults);
						break;

					case 4u:
						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<4u, CV::PC_TOP_LEFT>(source.constdata<uint8_t>(), source.width(), source.height(), source.paddingElements(), sourcePosition, pixelResults);
						break;

					default:
						ocean_assert(false && "Invalid channel number!");
						return false;
				}

				for (unsigned int n = 0u; n < validationTarget.channels(); ++n)
				{
					if (std::abs(int(validationTargetPixel[n]) - int(pixelResults[n])) > 1)
					{
						invalidForegroundValues++;
					}
				}
			}
			else
			{
				for (unsigned int n = 0u; n < validationTarget.channels(); ++n)
				{
					if (std::abs(int(validationTargetPixel[n]) - int(backgroundColor[n])) > 1)
					{
						invalidBackgroundValues++;
					}
				}
			}
		}
	}

	const unsigned int invalidValues = invalidForegroundValues + invalidBackgroundValues;

	ocean_assert(invalidValues <= validationTarget.size()); // size - as we determine invalid values per pixel and channel
	const double percent = double(validationTarget.size() - invalidValues) / double(validationTarget.size());

	if (percent >= 0.99)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool TestFrameInterpolatorBilinear::validateHomographyMask8BitPerChannel(const Frame& frame, const Frame& interpolatedFrame, const Frame& interpolatedMask, const SquareMatrix3& input_H_output, const CV::PixelPositionI& interpolatedFrameOrigin)
{
	ocean_assert(frame && interpolatedFrame);

	ocean_assert(frame.pixelFormat() == interpolatedFrame.pixelFormat() && frame.pixelOrigin() == interpolatedFrame.pixelOrigin());
	ocean_assert(frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	ocean_assert(interpolatedFrame.width() == interpolatedMask.width() && interpolatedFrame.height() == interpolatedMask.height());
	ocean_assert(interpolatedFrame.pixelOrigin() == interpolatedMask.pixelOrigin());
	ocean_assert(FrameType::formatIsGeneric(interpolatedMask.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));

	ocean_assert(!input_H_output.isSingular());

	const Scalar frameBorderEps = Scalar(0.5);

	ocean_assert(interpolatedFrame.channels() <= 4u);
	uint8_t pixelResults[4] = {0x00, 0x00, 0x00, 0x00};

	unsigned int invalidValues = 0u;
	unsigned int skippedPixels = 0u;

	for (unsigned int y = 0u; y < interpolatedFrame.height(); ++y)
	{
		for (unsigned int x = 0u; x < interpolatedFrame.width(); ++x)
		{
			const uint8_t* const interpolatedFramePixel = interpolatedFrame.constpixel<uint8_t>(x, y);
			const uint8_t interpolatedMaskPixel = interpolatedMask.constpixel<uint8_t>(x, y)[0];

			ocean_assert(interpolatedMaskPixel == 0x00 || interpolatedMaskPixel == 0xFF);

			const Vector2 outputPosition = Vector2(Scalar(x) + Scalar(interpolatedFrameOrigin.x()), Scalar(y) + Scalar(interpolatedFrameOrigin.y()));
			const Vector2 inputPosition = input_H_output * outputPosition;

			const bool isAtFrameBorder = Numeric::isNotEqual(inputPosition.x(), Scalar(0), frameBorderEps)
											&& Numeric::isNotEqual(inputPosition.x(), Scalar(frame.width() - 1u), frameBorderEps)
											&& Numeric::isNotEqual(inputPosition.y(), Scalar(0), frameBorderEps)
											&& Numeric::isNotEqual(inputPosition.y(), Scalar(frame.height() - 1u), frameBorderEps);

			if (inputPosition.x() >= Scalar(0) && inputPosition.y() >= Scalar(0) && inputPosition.x() <= Scalar(frame.width() - 1u) && inputPosition.y() <= Scalar(frame.height() - 1u))
			{
				switch (frame.channels())
				{
					case 1u:
						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<1u, CV::PC_TOP_LEFT>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), inputPosition, pixelResults);
						break;

					case 2u:
						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<2u, CV::PC_TOP_LEFT>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), inputPosition, pixelResults);
						break;

					case 3u:
						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<3u, CV::PC_TOP_LEFT>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), inputPosition, pixelResults);
						break;

					case 4u:
						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<4u, CV::PC_TOP_LEFT>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), inputPosition, pixelResults);
						break;

					default:
						ocean_assert(false && "Invalid frame number!");
						return false;
				}

				for (unsigned int n = 0u; n < interpolatedFrame.channels(); ++n)
				{
					if (interpolatedFramePixel[n] != pixelResults[n] || interpolatedMaskPixel != 0xFFu)
					{
						if (isAtFrameBorder)
						{
							++skippedPixels;
						}
						else
						{
							++invalidValues;
						}
					}
				}
			}
			else
			{
				if (interpolatedMaskPixel != 0x00)
				{
					if (isAtFrameBorder)
					{
						++skippedPixels;
					}
					else
					{
						++invalidValues;
					}
				}
			}
		}
	}

	ocean_assert(invalidValues <= interpolatedFrame.size()); // size - as we determine invalid values per pixel and channel
	const double percent = double(interpolatedFrame.size() - invalidValues) / double(interpolatedFrame.size());

	if (skippedPixels > frame.pixels() * 10u / 100u)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	return percent >= 0.995;
}

template <typename T>
bool TestFrameInterpolatorBilinear::validateLookup(const Frame& sourceFrame, const Frame& targetFrame, const LookupCorner2<Vector2>& lookupTable, const bool offset, const T* backgroundColor)
{
	ocean_assert(sourceFrame.isValid() && targetFrame.isValid());
	ocean_assert(backgroundColor != nullptr);

	ocean_assert(targetFrame.width() == (unsigned int)(lookupTable.sizeX()));
	ocean_assert(targetFrame.height() == (unsigned int)(lookupTable.sizeY()));
	ocean_assert(sourceFrame.pixelFormat() == targetFrame.pixelFormat());
	ocean_assert(sourceFrame.pixelOrigin() == targetFrame.pixelOrigin());

	const unsigned int channels = sourceFrame.channels();

	double sumAbsError = 0.0;
	double maxAbsError = 0.0;

	unsigned int measurements = 0u;

	constexpr Scalar borderDistance = Scalar(1.1);

	std::vector<T> pixelValue(channels);

	for (unsigned int ty = 0u; ty < targetFrame.height(); ++ty)
	{
		for (unsigned int tx = 0u; tx < targetFrame.width(); ++tx)
		{
			Vector2 sourcePosition = lookupTable.bilinearValue(Scalar(tx), Scalar(ty));

			if (offset)
			{
				sourcePosition += Vector2(Scalar(tx), Scalar(ty));
			}

			// due to rounding issues etc. we do not check the interpolation results at the boundary of the source frame
			const bool nearBoundary = Numeric::isEqual(sourcePosition.x(), Scalar(0), borderDistance) || Numeric::isEqual(sourcePosition.y(), Scalar(0), borderDistance) || Numeric::isEqual(sourcePosition.x(), Scalar(sourceFrame.width()), borderDistance) || Numeric::isEqual(sourcePosition.y(), Scalar(sourceFrame.height()), borderDistance);

			const T* testValue = backgroundColor;

			if (!nearBoundary)
			{
				if (sourcePosition.x() >= Scalar(0) && sourcePosition.y() >= Scalar(0) && sourcePosition.x() < Scalar(sourceFrame.width()) && sourcePosition.y() < Scalar(sourceFrame.height()))
				{
					switch (channels)
					{
						case 1u:
							CV::FrameInterpolatorBilinear::interpolatePixel<T, T, 1u, CV::PC_TOP_LEFT>(sourceFrame.constdata<T>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), sourcePosition, pixelValue.data());
							break;

						case 2u:
							CV::FrameInterpolatorBilinear::interpolatePixel<T, T, 2u, CV::PC_TOP_LEFT>(sourceFrame.constdata<T>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), sourcePosition, pixelValue.data());
							break;

						case 3u:
							CV::FrameInterpolatorBilinear::interpolatePixel<T, T, 3u, CV::PC_TOP_LEFT>(sourceFrame.constdata<T>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), sourcePosition, pixelValue.data());
							break;

						case 4u:
							CV::FrameInterpolatorBilinear::interpolatePixel<T, T, 4u, CV::PC_TOP_LEFT>(sourceFrame.constdata<T>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), sourcePosition, pixelValue.data());
							break;

						default:
							ocean_assert(false && "Invalid channels!");
							return false;
					}

					testValue = pixelValue.data();
				}

				for (unsigned int n = 0u; n < channels; ++n)
				{
					const double absError = std::abs(double(testValue[n]) - double(targetFrame.constpixel<T>(tx, ty)[n]));

					sumAbsError += absError;
					maxAbsError = std::max(maxAbsError, absError);

					measurements++;
				}
			}
		}
	}

	ocean_assert(measurements != 0u);
	if (measurements == 0u)
	{
		return false;
	}

	const double averageAbsError = sumAbsError / double(measurements);

	return maxAbsError <= 10.0 && averageAbsError < 1.0;
}

bool TestFrameInterpolatorBilinear::validateLookupMask(const Frame& sourceFrame, const Frame& targetFrame, const Frame& targetMask, const LookupCorner2<Vector2>& lookupTable, const bool offset)
{
	ocean_assert(sourceFrame.isValid() && targetFrame.isValid());
	ocean_assert(sourceFrame.width() > 50u && sourceFrame.height() > 50u);

	ocean_assert(targetFrame.width() == (unsigned int)(lookupTable.sizeX()));
	ocean_assert(targetFrame.height() == (unsigned int)(lookupTable.sizeY()));
	ocean_assert(sourceFrame.pixelFormat() == targetFrame.pixelFormat());
	ocean_assert(sourceFrame.pixelOrigin() == targetFrame.pixelOrigin());

	const unsigned int channels = sourceFrame.channels();

	unsigned int sumAbsError = 0u;
	unsigned int maxAbsError = 0u;
	unsigned int invalidMaskPixels = 0u;

	unsigned int nonMaskPixels = 0u;

	std::vector<uint8_t> pixelValue(channels);

	for (unsigned int ty = 0u; ty < targetFrame.height(); ++ty)
	{
		for (unsigned int tx = 0u; tx < targetFrame.width(); ++tx)
		{
			Vector2 sourcePosition = lookupTable.bilinearValue(Scalar(tx), Scalar(ty));

			if (offset)
			{
				sourcePosition += Vector2(Scalar(tx), Scalar(ty));
			}

			if (sourcePosition.x() >= Scalar(0) && sourcePosition.y() >= Scalar(0) && sourcePosition.x() < Scalar(sourceFrame.width() - 1u) && sourcePosition.y() < Scalar(sourceFrame.height()) - 1u)
			{
				switch (channels)
				{
					case 1u:
						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<1u, CV::PC_TOP_LEFT>(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), sourcePosition, pixelValue.data());
						break;

					case 2u:
						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<2u, CV::PC_TOP_LEFT>(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), sourcePosition, pixelValue.data());
						break;

					case 3u:
						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<3u, CV::PC_TOP_LEFT>(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), sourcePosition, pixelValue.data());
						break;

					case 4u:
						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<4u, CV::PC_TOP_LEFT>(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), sourcePosition, pixelValue.data());
						break;

					default:
						ocean_assert(false && "Invalid channels!");
						return false;
				}

				for (unsigned int n = 0u; n < channels; ++n)
				{
					const unsigned int absError = (unsigned int)std::abs(int(pixelValue[n]) - int(targetFrame.constpixel<uint8_t>(tx, ty)[n]));

					sumAbsError += absError;
					maxAbsError = std::max(maxAbsError, absError);
				}

				if (targetMask.constpixel<uint8_t>(tx, ty)[0] != 0xFF)
				{
					++invalidMaskPixels;
				}

				++nonMaskPixels;
			}
			else
			{
				if (targetMask.constpixel<uint8_t>(tx, ty)[0] != 0x00)
				{
					++invalidMaskPixels;
				}
			}
		}
	}

	ocean_assert(nonMaskPixels != 0u);
	if (nonMaskPixels == 0u)
	{
		return false;
	}

	const double averageAbsError = double(sumAbsError) / double(nonMaskPixels);

	const double ratioInvalidMaskPixels = double(invalidMaskPixels) / double(sourceFrame.pixels());

	return maxAbsError <= 10u && averageAbsError < 1.0 && ratioInvalidMaskPixels <= 0.05;
}

bool TestFrameInterpolatorBilinear::validateRotatedFrame(const Frame& source, const Frame& target, const Scalar anchorX, const Scalar anchorY, const Scalar angle)
{
	ocean_assert(source.isValid() && target.isValid());
	ocean_assert(source.frameType() == target.frameType());
	ocean_assert(source.numberPlanes() == 1u && source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	const Vector2 anchor(anchorX, anchorY);
	const Rotation rotationZ(0, 0, 1, angle);

	unsigned int validPixels = 0u;

	std::vector<uint8_t> interpolationResult(source.channels());

	for (unsigned int y = 0u; y < source.height(); ++y)
	{
		for (unsigned int x = 0u; x < source.width(); ++x)
		{
			/**
			 * we apply a rotation around the z-axis:
			 *
			 *      ^
			 *  Z .
			 *  .
			 * . . . . . >
			 * .      X
			 * .
			 * . Y
			 * v
			 *
			 */

			const Vector2 targetLocation = Vector2(Scalar(x), Scalar(y));
			const Vector2 shiftedTargetLocation = targetLocation - anchor;
			const Vector2 shiftedSourceLocation = (rotationZ * Vector3(shiftedTargetLocation, 0)).xy();
			const Vector2 sourceLocation = shiftedSourceLocation + anchor;

			if (sourceLocation.x() >= 0 && sourceLocation.x() <= Scalar(source.width() - 1u) && sourceLocation.y() >= 0 && sourceLocation.y() <= Scalar(source.height() - 1u))
			{
				switch (source.channels())
				{
					case 1u:
						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<1u, CV::PC_TOP_LEFT>(source.constdata<uint8_t>(), source.width(), source.height(), source.paddingElements(), sourceLocation, interpolationResult.data());
						break;

					case 2u:
						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<2u, CV::PC_TOP_LEFT>(source.constdata<uint8_t>(), source.width(), source.height(), source.paddingElements(), sourceLocation, interpolationResult.data());
						break;

					case 3u:
						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<3u, CV::PC_TOP_LEFT>(source.constdata<uint8_t>(), source.width(), source.height(), source.paddingElements(), sourceLocation, interpolationResult.data());
						break;

					case 4u:
						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<4u, CV::PC_TOP_LEFT>(source.constdata<uint8_t>(), source.width(), source.height(), source.paddingElements(), sourceLocation, interpolationResult.data());
						break;

					default:
						ocean_assert(false && "Invalid frame number!");
						return false;
				}
			}
			else
			{
				memset(interpolationResult.data(), 0x00, interpolationResult.size());
			}

			const uint8_t* targetData = target.constpixel<uint8_t>(x, y);

			unsigned int maximalError = 0u;
			for (unsigned int n = 0u; n < source.channels(); ++n)
			{
				maximalError = max(maximalError, (unsigned int)std::abs(int(targetData[n]) - int(interpolationResult[n])));
			}

			if (maximalError <= 2u)
			{
				validPixels++;
			}
		}
	}

	const double validPercentage = double(validPixels) / double(source.pixels());

	return validPercentage >= 0.99;
}

bool TestFrameInterpolatorBilinear::validatePatchIntensitySum1Channel(const Frame& yFrame, const unsigned int patchWidth, const unsigned int patchHeight, const Vector2& center, const CV::PixelCenter pixelCenter, const Scalar intensity)
{
	ocean_assert(yFrame.isValid() && yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8));

	if (!yFrame.isValid() || !yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8))
	{
		return false;
	}

	ocean_assert(patchWidth >= 1u && patchHeight >= 1u);

	Scalar sum = 0;

	for (unsigned int y = 0u; y < patchHeight; ++y)
	{
		const Scalar yOffset = Scalar(y) - Scalar(patchHeight - 1u) * Scalar(0.5);

		for (unsigned int x = 0u; x < patchWidth; ++x)
		{
			const Scalar xOffset = Scalar(x) - Scalar(patchWidth - 1u) * Scalar(0.5);

			Scalar result;
			if (!CV::FrameInterpolatorBilinear::Comfort::interpolatePixel<uint8_t, Scalar>(yFrame.constdata<uint8_t>(), 1u, yFrame.width(), yFrame.height(), yFrame.paddingElements(), pixelCenter, center + Vector2(xOffset, yOffset), &result))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			sum += result;
		}
	}

	double threshold = 0.1;

	if constexpr (std::is_same<float, Scalar>::value)
	{
		// the larger the patch the larger the threshold

		const unsigned int patchArea = patchWidth * patchHeight;

		threshold = std::max(threshold, double(patchArea) * 0.001);
	}

	if (NumericD::isNotEqual(double(intensity), double(sum), threshold))
	{
		return false;
	}

	return true;
}

template <uint32_t tChannels>
bool TestFrameInterpolatorBilinear::validatePatchFrame8BitPerChannel(const uint8_t* const source, const uint8_t* const validationTarget, const uint32_t sourceWidth, const uint32_t sourceHeight, const Scalar x, const Scalar y, const uint32_t validationTargetWidth, const uint32_t validationTargetHeight, const uint32_t sourcePaddingElements, const uint32_t validationTargetPaddingElements)
{
	static_assert(tChannels != 0u, "The minimum is 1 channel");
	ocean_assert(source != nullptr && validationTarget != nullptr);
	ocean_assert(validationTargetWidth != 0u && validationTargetHeight != 0u && validationTargetWidth <= sourceWidth && validationTargetHeight <= sourceHeight);
	ocean_assert(validationTargetWidth % 2u == 1u && validationTargetHeight % 2u == 1u);

	bool validationSuccessful = true;
	constexpr uint32_t maxInterpolationDifference = 3u;
	uint32_t maxInterpolationDifferenceFound = 0u;

	const uint32_t validationTargetStrideElements = validationTargetWidth * tChannels + validationTargetPaddingElements;

	const Scalar sourceLeft = x - Scalar(validationTargetWidth / 2u);
	const Scalar sourceTop = y - Scalar(validationTargetHeight / 2u);

	for (uint32_t targetY = 0u; targetY < validationTargetHeight; ++targetY)
	{
		const Scalar sourceY = sourceTop + Scalar(targetY);
		ocean_assert(sourceY >= 0 && sourceY < Scalar(sourceHeight));

		for (uint32_t targetX = 0u; targetX < validationTargetWidth; ++targetX)
		{
			const Scalar sourceX = sourceLeft + Scalar(targetX);
			ocean_assert(sourceX >= 0 && sourceX < Scalar(sourceWidth));

			uint8_t interpolationValue[tChannels];
			CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<tChannels, CV::PC_TOP_LEFT>(source, sourceWidth, sourceHeight, sourcePaddingElements, Vector2(sourceX, sourceY), interpolationValue);

			for (uint32_t channel = 0u; channel < tChannels; ++channel)
			{
				const uint32_t difference = std::abs(int32_t(validationTarget[targetY * validationTargetStrideElements + tChannels * targetX + channel]) - int32_t(interpolationValue[channel]));

				if (difference > maxInterpolationDifference)
				{
					validationSuccessful = false;
				}

				maxInterpolationDifferenceFound = std::max(difference, maxInterpolationDifferenceFound);
			}
		}
	}

	return validationSuccessful;
}

void TestFrameInterpolatorBilinear::validateScaleFramePrecision7Bit(const uint8_t* source, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int channels, const uint8_t* target, const unsigned int targetWidth, const unsigned int targetHeight, const double xSource_s_xTarget, const double ySource_s_yTarget, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, double* averageAbsError, double* maximalAbsError, uint8_t* groundTruth)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth != 0u && sourceHeight != 0u);
	ocean_assert(targetWidth != 0u && targetHeight != 0u);
	ocean_assert(channels >= 1u);
	ocean_assert(xSource_s_xTarget > 0.0 && ySource_s_yTarget > 0.0);

	const unsigned int sourceStrideElements = sourceWidth * channels + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * channels + targetPaddingElements;

	std::vector<uint8_t> intermediate(channels * sourceWidth, 0u);
	std::vector<uint8_t> result(channels, 0u);

	if (averageAbsError)
	{
		*averageAbsError = NumericD::maxValue();
	}

	if (maximalAbsError)
	{
		*maximalAbsError = NumericD::maxValue();
	}

	double sumAbsError = 0.0;
	double maxAbsError = 0.0;

	for (unsigned int y = 0u; y < targetHeight; ++y)
	{
		const double sy = minmax(0.0, (double(y) + 0.5) * ySource_s_yTarget - 0.5, double(sourceHeight - 1u));

		const unsigned int topRow = (unsigned int)(sy);
		const unsigned int bottomRow = min(topRow + 1u, sourceHeight - 1u);
		ocean_assert(topRow < sourceHeight && bottomRow < sourceHeight);

		const uint8_t* sourceTop = source + sourceStrideElements * topRow;
		const uint8_t* sourceBottom = source + sourceStrideElements * bottomRow;

		const double bottomFactor = sy - double(topRow);
		ocean_assert(bottomFactor >= 0.0 && bottomFactor <= 1.0);

		const unsigned int uBottomFactor = (unsigned int)(bottomFactor * 128.0 + 0.5);
		const unsigned int uTopFactor = 128u - uBottomFactor;

		for (unsigned int n = 0u; n < sourceWidth * channels; ++n)
		{
			intermediate[n] = uint8_t((sourceTop[n] * uTopFactor + sourceBottom[n] * uBottomFactor + 64u) / 128u);
		}

		for (unsigned int x = 0u; x < targetWidth; ++x)
		{
			const double sx = minmax(0.0, (double(x) + 0.5) * xSource_s_xTarget - 0.5, double(sourceWidth - 1u));

			const unsigned int leftPixel = (unsigned int)(sx);
			const unsigned int rightPixel = min(leftPixel + 1u, sourceWidth - 1u);
			ocean_assert(leftPixel < sourceWidth && rightPixel < sourceWidth);

			const double rightFactor = sx - double(leftPixel);
			ocean_assert(rightFactor >= 0.0 && rightFactor <= 1.0);

			const unsigned int uRightFactor = (unsigned int)(rightFactor * 128.0 + 0.5);
			const unsigned int uLeftFactor = 128u - uRightFactor;

			const uint8_t* intermediateLeft = intermediate.data() + leftPixel * channels;
			const uint8_t* intermediateRight = intermediate.data() + rightPixel * channels;

			for (unsigned int n = 0u; n < channels; ++n)
			{
				result[n] = uint8_t((intermediateLeft[n] * uLeftFactor + intermediateRight[n] * uRightFactor + 64u) / 128u);
			}

			const uint8_t* const targetValue = target + targetStrideElements * y + x * channels;

			for (unsigned int n = 0u; n < channels; ++n)
			{
				const double absError = NumericD::abs(double(result[n]) - double(targetValue[n]));

				sumAbsError += absError;
				maxAbsError = max(maxAbsError, absError);
			}

			if (groundTruth)
			{
				memcpy(groundTruth + (y * targetWidth + x) * channels, result.data(), sizeof(uint8_t) * channels);
			}
		}
	}

	if (averageAbsError)
	{
		*averageAbsError = sumAbsError / double(targetWidth * targetHeight * channels);
	}

	if (maximalAbsError)
	{
		*maximalAbsError = maxAbsError;
	}
}

} // namespace TestCV

} // namespace Test

} // namespace Ocean
