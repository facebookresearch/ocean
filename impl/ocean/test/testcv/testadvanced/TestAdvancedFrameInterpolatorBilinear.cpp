/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testadvanced/TestAdvancedFrameInterpolatorBilinear.h"

#include "ocean/base/Frame.h"
#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/cv/advanced/AdvancedFrameInterpolatorBilinear.h"
#include "ocean/cv/advanced/AdvancedFrameInterpolatorBilinearNEON.h"
#include "ocean/cv/advanced/AdvancedFrameInterpolatorBilinearSSE.h"

#include "ocean/geometry/Homography.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestAdvanced
{

bool TestAdvancedFrameInterpolatorBilinear::test(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Advanced bilinear interpolation test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testInterpolatePixelWithMask8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testInterpolateSquare(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testInterpolateSquareMirroredBorder(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHomographyFilterMask(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Advanced bilinear interpolation test succeeded.";
	}
	else
	{
		Log::info() << "Advanced bilinear interpolation test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePixelWithMask8BitPerChannel_float_TopLeft)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePixelWithMask8BitPerChannel<float>(CV::PC_TOP_LEFT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePixelWithMask8BitPerChannel_float_Center)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePixelWithMask8BitPerChannel<float>(CV::PC_CENTER, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePixelWithMask8BitPerChannel_double_TopLeft)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePixelWithMask8BitPerChannel<double>(CV::PC_TOP_LEFT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePixelWithMask8BitPerChannel_double_Center)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePixelWithMask8BitPerChannel<double>(CV::PC_CENTER, GTEST_TEST_DURATION)));
}


TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareChannels1PatchSize1)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<1u, 1u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<1u, 1u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareChannels2PatchSize1)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<2u, 1u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<2u, 1u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareChannels3PatchSize1)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<3u, 1u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<3u, 1u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareChannels4PatchSize1)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<4u, 1u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<4u, 1u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareChannels1PatchSize3)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<1u, 3u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<1u, 3u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareChannels2PatchSize3)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<2u, 3u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<2u, 3u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareChannels3PatchSize3)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<3u, 3u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<3u, 3u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareChannels4PatchSize3)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<4u, 3u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<4u, 3u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareChannels1PatchSize5)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<1u, 5u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<1u, 5u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareChannels2PatchSize5)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<2u, 5u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<2u, 5u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareChannels3PatchSize5)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<3u, 5u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<3u, 5u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareChannels4PatchSize5)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<4u, 5u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<4u, 5u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareChannels1PatchSize7)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<1u, 7u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<1u, 7u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareChannels2PatchSize7)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<2u, 7u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<2u, 7u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareChannels3PatchSize7)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<3u, 7u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<3u, 7u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareChannels4PatchSize7)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<4u, 7u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<4u, 7u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareChannels1PatchSize15)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<1u, 15u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<1u, 15u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareChannels2PatchSize15)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<2u, 15u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<2u, 15u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareChannels3PatchSize15)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<3u, 15u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<3u, 15u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareChannels4PatchSize15)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<4u, 15u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<4u, 15u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareChannels1PatchSize31)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<1u, 31u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<1u, 31u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareChannels2PatchSize31)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<2u, 31u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<2u, 31u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareChannels3PatchSize31)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<3u, 31u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<3u, 31u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareChannels4PatchSize31)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<4u, 31u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare<4u, 31u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}


TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePatchWithMaskChannels1PatchSize1)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<1u, 1u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<1u, 1u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePatchWithMaskChannels2PatchSize1)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<2u, 1u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<2u, 1u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePatchWithMaskChannels3PatchSize1)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<3u, 1u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<3u, 1u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePatchWithMaskChannels4PatchSize1)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<4u, 1u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<4u, 1u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePatchWithMaskChannels1PatchSize3)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<1u, 3u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<1u, 3u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePatchWithMaskChannels2PatchSize3)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<2u, 3u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<2u, 3u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePatchWithMaskChannels3PatchSize3)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<3u, 3u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<3u, 3u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePatchWithMaskChannels4PatchSize3)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<4u, 3u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<4u, 3u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePatchWithMaskChannels1PatchSize5)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<1u, 5u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<1u, 5u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePatchWithMaskChannels2PatchSize5)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<2u, 5u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<2u, 5u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePatchWithMaskChannels3PatchSize5)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<3u, 5u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<3u, 5u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePatchWithMaskChannels4PatchSize5)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<4u, 5u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<4u, 5u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePatchWithMaskChannels1PatchSize7)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<1u, 7u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<1u, 7u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePatchWithMaskChannels2PatchSize7)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<2u, 7u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<2u, 7u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePatchWithMaskChannels3PatchSize7)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<3u, 7u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<3u, 7u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePatchWithMaskChannels4PatchSize7)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<4u, 7u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<4u, 7u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePatchWithMaskChannels1PatchSize15)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<1u, 15u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<1u, 15u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePatchWithMaskChannels2PatchSize15)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<2u, 15u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<2u, 15u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePatchWithMaskChannels3PatchSize15)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<3u, 15u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<3u, 15u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePatchWithMaskChannels4PatchSize15)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<4u, 15u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<4u, 15u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePatchWithMaskChannels1PatchSize31)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<1u, 31u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<1u, 31u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePatchWithMaskChannels2PatchSize31)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<2u, 31u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<2u, 31u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePatchWithMaskChannels3PatchSize31)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<3u, 31u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<3u, 31u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolatePatchWithMaskChannels4PatchSize31)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<4u, 31u, CV::PC_TOP_LEFT>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask<4u, 31u, CV::PC_CENTER>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}


TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareMirroredBorderChannels1PatchSize1)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquareMirroredBorder<1u, 1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareMirroredBorderChannels2PatchSize1)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquareMirroredBorder<2u, 1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareMirroredBorderChannels3PatchSize1)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquareMirroredBorder<3u, 1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareMirroredBorderChannels4PatchSize1)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquareMirroredBorder<4u, 1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareMirroredBorderChannels1PatchSize3)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquareMirroredBorder<1u, 3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareMirroredBorderChannels2PatchSize3)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquareMirroredBorder<2u, 3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareMirroredBorderChannels3PatchSize3)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquareMirroredBorder<3u, 3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareMirroredBorderChannels4PatchSize3)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquareMirroredBorder<4u, 3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareMirroredBorderChannels1PatchSize5)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquareMirroredBorder<1u, 5u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareMirroredBorderChannels2PatchSize5)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquareMirroredBorder<2u, 5u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareMirroredBorderChannels3PatchSize5)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquareMirroredBorder<3u, 5u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareMirroredBorderChannels4PatchSize5)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquareMirroredBorder<4u, 5u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareMirroredBorderChannels1PatchSize7)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquareMirroredBorder<1u, 7u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareMirroredBorderChannels2PatchSize7)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquareMirroredBorder<2u, 7u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareMirroredBorderChannels3PatchSize7)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquareMirroredBorder<3u, 7u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareMirroredBorderChannels4PatchSize7)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquareMirroredBorder<4u, 7u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareMirroredBorderChannels1PatchSize15)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquareMirroredBorder<1u, 15u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareMirroredBorderChannels2PatchSize15)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquareMirroredBorder<2u, 15u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareMirroredBorderChannels3PatchSize15)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquareMirroredBorder<3u, 15u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareMirroredBorderChannels4PatchSize15)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquareMirroredBorder<4u, 15u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareMirroredBorderChannels1PatchSize31)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquareMirroredBorder<1u, 31u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareMirroredBorderChannels2PatchSize31)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquareMirroredBorder<2u, 31u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareMirroredBorderChannels3PatchSize31)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquareMirroredBorder<3u, 31u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestAdvancedFrameInterpolatorBilinear, InterpolateSquareMirroredBorderChannels4PatchSize31)
{
	EXPECT_TRUE((TestAdvancedFrameInterpolatorBilinear::testInterpolateSquareMirroredBorder<4u, 31u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}


TEST(TestAdvancedFrameInterpolatorBilinear, HomographyFilterMask_1920x1080_1channel)
{
	Worker worker;
	EXPECT_TRUE(TestAdvancedFrameInterpolatorBilinear::testHomographyFilterMask(1920u, 1080u, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestAdvancedFrameInterpolatorBilinear, HomographyFilterMask_1920x1080_2channel)
{
	Worker worker;
	EXPECT_TRUE(TestAdvancedFrameInterpolatorBilinear::testHomographyFilterMask(1920u, 1080u, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestAdvancedFrameInterpolatorBilinear, HomographyFilterMask_1920x1080_3channel)
{
	Worker worker;
	EXPECT_TRUE(TestAdvancedFrameInterpolatorBilinear::testHomographyFilterMask(1920u, 1080u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestAdvancedFrameInterpolatorBilinear, HomographyFilterMask_1920x1080_4channel)
{
	Worker worker;
	EXPECT_TRUE(TestAdvancedFrameInterpolatorBilinear::testHomographyFilterMask(1920u, 1080u, 4u, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestAdvancedFrameInterpolatorBilinear::testInterpolatePixelWithMask8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Pixel interpolation with mask test using 7bit precision:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testInterpolatePixelWithMask8BitPerChannel<float>(CV::PC_TOP_LEFT, testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testInterpolatePixelWithMask8BitPerChannel<float>(CV::PC_CENTER, testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testInterpolatePixelWithMask8BitPerChannel<double>(CV::PC_TOP_LEFT, testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testInterpolatePixelWithMask8BitPerChannel<double>(CV::PC_CENTER, testDuration) && allSucceeded;
	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Pixel interpolation with mask test succeeded.";
	}
	else
	{
		Log::info() << "Pixel interpolation with mask test FAILED!";
	}

	return allSucceeded;
}

template <typename TScalar>
bool TestAdvancedFrameInterpolatorBilinear::testInterpolatePixelWithMask8BitPerChannel(const CV::PixelCenter pixelCenter, const double testDuration)
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

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);
		const unsigned int channels = RandomI::random(randomGenerator, 1u, 5u);

		const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		const uint8_t invalidPixelValue = uint8_t(RandomI::random(randomGenerator, 255u));
		const uint8_t validPixelValue = 0xFFu - invalidPixelValue;

		const Frame mask = CV::CVUtilities::randomizedBinaryMask(width, height, invalidPixelValue, &randomGenerator);

		std::vector<uint8_t> interpolationResult(channels + 1u);
		uint8_t maskResult;

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const VectorT2<TScalar> position = RandomT<TScalar>::vector2(randomGenerator, TScalar(-100), TScalar(width + 100u), TScalar(-100), TScalar(height + 100u));

			for (uint8_t& value : interpolationResult)
			{
				value = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			const uint8_t lastValue = interpolationResult.back();

			if (!CV::Advanced::AdvancedFrameInterpolatorBilinear::Comfort::interpolatePixelWithMask8BitPerChannel(frame.constdata<uint8_t>(), mask.constdata<uint8_t>(), frame.channels(), frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), pixelCenter, position, interpolationResult.data(), maskResult, validPixelValue))
			{
				ocean_assert(false && "This should never happen!");
				allSucceeded = false;
			}

			if (lastValue != interpolationResult.back())
			{
				ocean_assert(false && "Invalid padding memory!");
				return false;
			}

			if (!validateInterpolatePixel8BitPerChannel<TScalar>(frame, mask, position, pixelCenter, validPixelValue, interpolationResult.data(), maskResult, threshold))
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

bool TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Square patch interpolation test:";
	Log::info() << " ";

	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;

	bool allSucceeded = true;

	allSucceeded = testInterpolateSquare<1u, 1u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolateSquare<1u, 1u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquare<2u, 1u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolateSquare<2u, 1u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquare<3u, 1u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolateSquare<3u, 1u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquare<4u, 1u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolateSquare<4u, 1u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testInterpolateSquare<1u, 3u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolateSquare<1u, 3u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquare<2u, 3u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolateSquare<2u, 3u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquare<3u, 3u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolateSquare<3u, 3u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquare<4u, 3u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolateSquare<4u, 3u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testInterpolateSquare<1u, 5u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolateSquare<1u, 5u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquare<2u, 5u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolateSquare<2u, 5u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquare<3u, 5u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolateSquare<3u, 5u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquare<4u, 5u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolateSquare<4u, 5u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testInterpolateSquare<1u, 7u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolateSquare<1u, 7u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquare<2u, 7u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolateSquare<2u, 7u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquare<3u, 7u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolateSquare<3u, 7u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquare<4u, 7u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolateSquare<4u, 7u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testInterpolateSquare<1u, 15u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolateSquare<1u, 15u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquare<2u, 15u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolateSquare<2u, 15u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquare<3u, 15u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolateSquare<3u, 15u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquare<4u, 15u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolateSquare<4u, 15u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testInterpolateSquare<1u, 31u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolateSquare<1u, 31u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquare<2u, 31u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolateSquare<2u, 31u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquare<3u, 31u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolateSquare<3u, 31u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquare<4u, 31u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolateSquare<4u, 31u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	return allSucceeded;
}

bool TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Patch with mask interpolation test:";
	Log::info() << " ";

	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;

	bool allSucceeded = true;

	allSucceeded = testInterpolatePatchWithMask<1u, 1u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<1u, 1u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<2u, 1u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<2u, 1u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<3u, 1u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<3u, 1u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<4u, 1u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<4u, 1u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<1u, 3u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<1u, 3u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<2u, 3u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<2u, 3u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<3u, 3u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<3u, 3u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<4u, 3u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<4u, 3u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<1u, 5u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<1u, 5u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<2u, 5u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<2u, 5u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<3u, 5u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<3u, 5u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<4u, 5u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<4u, 5u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<1u, 7u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<1u, 7u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<2u, 7u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<2u, 7u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<3u, 7u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<3u, 7u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<4u, 7u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<4u, 7u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<1u, 8u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<1u, 8u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<2u, 8u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<2u, 8u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<3u, 8u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<3u, 8u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<4u, 8u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<4u, 8u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<1u, 15u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<1u, 15u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<2u, 15u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<2u, 15u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<3u, 15u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<3u, 15u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<4u, 15u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<4u, 15u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<1u, 31u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<1u, 31u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<2u, 31u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<2u, 31u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<3u, 31u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<3u, 31u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolatePatchWithMask<4u, 31u, CV::PC_TOP_LEFT>(width, height, testDuration) && allSucceeded;
	allSucceeded = testInterpolatePatchWithMask<4u, 31u, CV::PC_CENTER>(width, height, testDuration) && allSucceeded;

	return allSucceeded;
}

bool TestAdvancedFrameInterpolatorBilinear::testInterpolateSquareMirroredBorder(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Square patch interpolation (mirrored border) test:";
	Log::info() << " ";

	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;

	bool allSucceeded = true;

	allSucceeded = testInterpolateSquareMirroredBorder<1u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquareMirroredBorder<2u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquareMirroredBorder<3u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquareMirroredBorder<4u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testInterpolateSquareMirroredBorder<1u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquareMirroredBorder<2u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquareMirroredBorder<3u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquareMirroredBorder<4u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testInterpolateSquareMirroredBorder<1u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquareMirroredBorder<2u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquareMirroredBorder<3u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquareMirroredBorder<4u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testInterpolateSquareMirroredBorder<1u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquareMirroredBorder<2u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquareMirroredBorder<3u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquareMirroredBorder<4u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testInterpolateSquareMirroredBorder<1u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquareMirroredBorder<2u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquareMirroredBorder<3u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquareMirroredBorder<4u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testInterpolateSquareMirroredBorder<1u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquareMirroredBorder<2u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquareMirroredBorder<3u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateSquareMirroredBorder<4u, 31u>(width, height, testDuration) && allSucceeded;

	return allSucceeded;
}

template <unsigned int tChannels, unsigned int tPatchSize, CV::PixelCenter tPixelCenter>
bool TestAdvancedFrameInterpolatorBilinear::testInterpolateSquare(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize % 2u == 1u, "Invalid patch size!");

	ocean_assert(width >= tPatchSize + 1u);
	ocean_assert(height >= tPatchSize + 1u);

	ocean_assert(testDuration > 0.0);

	Log::info() << "... with " << tChannels << " channels, pixel center at " << (tPixelCenter == CV::PC_TOP_LEFT ? "0.0" : "0.5") << ", and " << tPatchSize << "x" << tPatchSize << " patch test:";

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	constexpr unsigned int locations = 10000u;

	Vectors2 positions(locations);

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performanceTemplate;
	HighPerformanceStatistic performanceSSE;
	HighPerformanceStatistic performanceNEON;
	HighPerformanceStatistic performanceDefault;

	uint64_t iterations = 0u;
	uint64_t succeeded = 0u;

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool performanceIteration : {true, false})
		{
			const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, tPatchSize + 1u, 1920u);
			const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, tPatchSize + 1u, 1080u);

			const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			const Scalar offset = tPixelCenter == CV::PC_TOP_LEFT ? Scalar(0) : Scalar(0.5);

			const Scalar rangeLeft = Scalar(tPatchSize_2) + offset;
			const Scalar rangeRight = Scalar(testWidth - tPatchSize_2 - 1u) + offset - Numeric::weakEps();

			const Scalar rangeTop = Scalar(tPatchSize_2) + offset;
			const Scalar rangeBottom = Scalar(testHeight - tPatchSize_2 - 1u) + offset - Numeric::weakEps();

			for (Vector2& position : positions)
			{
				position = Random::vector2(randomGenerator, rangeLeft, rangeRight, rangeTop, rangeBottom);
			}

			const uint8_t* const frameData = frame.constdata<uint8_t>();

			for (const ImplementationType implementationType : {IT_NAIVE, IT_TEMPLATE, IT_SSE, IT_NEON, IT_DEFAULT})
			{
				Frame buffer = CV::CVUtilities::randomizedFrame(FrameType(frame, tPatchSize * tPatchSize, locations), &randomGenerator);

				const Frame copyBuffer(buffer, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				const unsigned int bufferStrideElements = buffer.strideElements();
				uint8_t* const bufferData = buffer.data<uint8_t>();

				switch (implementationType)
				{
					case IT_NAIVE:
					{
						performanceNaive.startIf(performanceIteration);

						for (size_t n = 0; n < locations; ++n)
						{
							CV::Advanced::AdvancedFrameInterpolatorBilinear::interpolatePatch8BitPerChannel<tChannels, tPixelCenter>(frameData, frame.width(), frame.paddingElements(), bufferData + n * bufferStrideElements, positions[n], tPatchSize, tPatchSize);
						}

						performanceNaive.stopIf(performanceIteration);

						break;
					}

					case IT_TEMPLATE:
					{
						performanceTemplate.startIf(performanceIteration);

						for (size_t n = 0; n < locations; ++n)
						{
							CV::Advanced::AdvancedFrameInterpolatorBilinearBase::interpolateSquarePatch8BitPerChannelTemplate<tChannels, tPatchSize, tPixelCenter>(frameData, frame.width(), frame.paddingElements(), bufferData + n * bufferStrideElements, positions[n]);
						}

						performanceTemplate.stopIf(performanceIteration);

						break;
					}

					case IT_SSE:
					{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
						if constexpr ((tChannels == 1u || tChannels == 3u) && tPatchSize >= 5u)
						{
							performanceSSE.startIf(performanceIteration);

							for (size_t n = 0; n < locations; ++n)
							{
								CV::Advanced::AdvancedFrameInterpolatorBilinearSSE::interpolateSquarePatch8BitPerChannel<tChannels, tPatchSize, tPixelCenter>(frameData, frame.width(), frame.paddingElements(), bufferData + n * bufferStrideElements, positions[n]);
							}

							performanceSSE.stopIf(performanceIteration);
						}
						else
#endif // OCEAN_HARDWARE_SSE_VERSION >= 41
						{
							// not implemented
							continue;
						}

						break;
					}

					case IT_NEON:
					{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
						if constexpr (tChannels == 1u && tPatchSize >= 5u)
						{
							performanceNEON.startIf(performanceIteration);

							for (size_t n = 0; n < locations; ++n)
							{
								CV::Advanced::AdvancedFrameInterpolatorBilinearNEON::interpolateSquarePatch8BitPerChannel<tChannels, tPatchSize, tPixelCenter>(frameData, frame.width(), frame.paddingElements(), bufferData + n * bufferStrideElements, positions[n]);
							}

							performanceNEON.stopIf(performanceIteration);
						}
						else
#endif // OCEAN_HARDWARE_NEON_VERSION >= 10
						{
							// not implemented
							continue;
						}

						break;
					}

					case IT_DEFAULT:
					{
						performanceDefault.startIf(performanceIteration);

						for (size_t n = 0; n < locations; ++n)
						{
							CV::Advanced::AdvancedFrameInterpolatorBilinear::interpolateSquarePatch8BitPerChannel<tChannels, tPatchSize, tPixelCenter>(frameData, frame.width(), frame.paddingElements(), bufferData + n * bufferStrideElements, positions[n]);
						}

						performanceDefault.stopIf(performanceIteration);

						break;
					}

					default:
						ocean_assert(false && "Invalid implementation type!");
						return false;
				}

				if (!CV::CVUtilities::isPaddingMemoryIdentical(buffer, copyBuffer))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				uint8_t testBuffer[tChannels * tPatchSize * tPatchSize];

				for (unsigned int n = 0u; n < locations; ++n)
				{
					if (interpolatePatch8BitPerChannel(frame, tPatchSize, tPatchSize, positions[n], tPixelCenter, testBuffer))
					{
						if (memcmp(buffer.constrow<void>(n), testBuffer, size_t(tChannels * tPatchSize * tPatchSize) * sizeof(uint8_t)) == 0)
						{
							++succeeded;
						}
					}

					++iterations;
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	static_assert(locations != 0u, "Invalid number of locations!");

	ocean_assert(iterations != 0ull);

	const double percent = double(succeeded) / double(iterations);

	if (performanceNaive.measurements() != 0u)
	{
		Log::info() << "   Naive: [" << performanceNaive.bestMseconds() << ", " << performanceNaive.medianMseconds() << ", " << performanceNaive.worstMseconds() << "] ms";
	}

	if (performanceTemplate.measurements() != 0u)
	{
		Log::info() << "Template: [" << performanceTemplate.bestMseconds() << ", " << performanceTemplate.medianMseconds() << ", " << performanceTemplate.worstMseconds() << "] ms";
	}

	if (performanceSSE.measurements() != 0u)
	{
		Log::info() << "     SSE: [" << performanceSSE.bestMseconds() << ", " << performanceSSE.medianMseconds() << ", " << performanceSSE.worstMseconds() << "] ms";
	}

	if (performanceNEON.measurements() != 0u)
	{
		Log::info() << "    NEON: [" << performanceNEON.bestMseconds() << ", " << performanceNEON.medianMseconds() << ", " << performanceNEON.worstMseconds() << "] ms";
	}

	ocean_assert(performanceDefault.measurements() != 0u);
	Log::info() << " Default: [" << performanceDefault.bestMseconds() << ", " << performanceDefault.medianMseconds() << ", " << performanceDefault.worstMseconds() << "] ms";

	const bool allSucceeded = percent >= 0.995;

	if (allSucceeded)
	{
		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

template <unsigned int tChannels, unsigned int tPatchSize, CV::PixelCenter tPixelCenter>
bool TestAdvancedFrameInterpolatorBilinear::testInterpolatePatchWithMask(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize >= 1u, "Invalid patch size!");

	ocean_assert(width >= tPatchSize + 1u);
	ocean_assert(height >= tPatchSize + 1u);

	ocean_assert(testDuration > 0.0);

	Log::info() << "... with " << tChannels << " channels, pixel center at " << (tPixelCenter == CV::PC_TOP_LEFT ? "0.0" : "0.5") << ", and " << tPatchSize << "x" << tPatchSize << " patch test:";

	constexpr unsigned int locations = 10000u;

	Vectors2 positions(locations);

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performanceTemplate;
	HighPerformanceStatistic performanceSSE;
	HighPerformanceStatistic performanceNEON;
	HighPerformanceStatistic performanceDefault;

	uint64_t iterations = 0u;
	uint64_t succeeded = 0u;

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool performanceIteration : {true, false})
		{
			const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 1u, 1920u);
			const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 1u, 1080u);

			const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			constexpr uint8_t validMaskValue = 0xFFu;

			const Frame mask = CV::CVUtilities::randomizedBinaryMask(testWidth, testHeight, 0xFFu - validMaskValue, &randomGenerator);

			for (Vector2& position : positions)
			{
				position = Random::vector2(randomGenerator, Scalar(-10), Scalar(testWidth + 10u), Scalar(-10), Scalar(testHeight + 10u));
			}

			const uint8_t* const frameData = frame.constdata<uint8_t>();
			const uint8_t* const maskData = mask.constdata<uint8_t>();

			for (const ImplementationType implementationType : {IT_NAIVE, /*IT_TEMPLATE, IT_SSE, IT_NEON,*/ IT_DEFAULT})
			{
				Frame patchBuffer = CV::CVUtilities::randomizedFrame(FrameType(frame, tPatchSize, locations * tPatchSize), &randomGenerator);
				Frame patchMaskBuffer = CV::CVUtilities::randomizedFrame(FrameType(patchBuffer, FrameType::FORMAT_Y8), &randomGenerator);

				const unsigned int patchBufferPaddingElements = patchBuffer.paddingElements();
				const unsigned int patchMaskBufferPaddingElements = patchMaskBuffer.paddingElements();

				const unsigned int patchBufferStrideElements = patchBuffer.strideElements();
				const unsigned int patchMaskBufferStrideElements = patchMaskBuffer.strideElements();

				const Frame copyPatchBuffer(patchBuffer, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
				const Frame copyPatchMaskBuffer(patchMaskBuffer, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				uint8_t* const patchBufferData = patchBuffer.data<uint8_t>();
				uint8_t* const patchMaskBufferData = patchMaskBuffer.data<uint8_t>();

				switch (implementationType)
				{
					case IT_NAIVE:
					{
						performanceNaive.startIf(performanceIteration);

						for (size_t n = 0; n < locations; ++n)
						{
							CV::Advanced::AdvancedFrameInterpolatorBilinear::interpolatePatchWithMask8BitPerChannel<tChannels, tPixelCenter>(frameData, maskData, frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), positions[n], patchBufferData + n * tPatchSize * patchBufferStrideElements, patchMaskBufferData + n * tPatchSize * patchMaskBufferStrideElements, tPatchSize, tPatchSize, patchBufferPaddingElements, patchMaskBufferPaddingElements, validMaskValue);
						}

						performanceNaive.stopIf(performanceIteration);

						break;
					}

					case IT_TEMPLATE:
					{
						/*performanceTemplate.startIf(performanceIteration);

						for (size_t n = 0; n < locations; ++n)
						{
							... not yet implemented
						}

						performanceTemplate.stopIf(performanceIteration);*/

						break;
					}

					case IT_SSE:
					{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
						/*if ((tChannels == 1u || tChannels == 3u) && tPatchSize >= 5u)
						{
							performanceSSE.startIf(performanceIteration);

							for (size_t n = 0; n < locations; ++n)
							{
								... not yet implemented
							}

							performanceSSE.stopIf(performanceIteration);
						}
						else*/
#endif // OCEAN_HARDWARE_SSE_VERSION >= 41
						{
							// not implemented
							continue;
						}

						break;
					}

					case IT_NEON:
					{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
						/*if constexpr (tChannels == 1u && tPatchSize >= 5u)
						{
							performanceNEON.startIf(performanceIteration);

							for (size_t n = 0; n < locations; ++n)
							{
								... not yet implemented
							}

							performanceNEON.stopIf(performanceIteration);
						}
						else*/
#endif // OCEAN_HARDWARE_NEON_VERSION >= 10
						{
							// not implemented
							continue;
						}

						break;
					}

					case IT_DEFAULT:
					{
						performanceDefault.startIf(performanceIteration);

						for (size_t n = 0; n < locations; ++n)
						{
							CV::Advanced::AdvancedFrameInterpolatorBilinear::interpolatePatchWithMask8BitPerChannel<tChannels, tPixelCenter>(frameData, maskData, frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), positions[n], patchBufferData + n * tPatchSize * patchBufferStrideElements, patchMaskBufferData + n * tPatchSize * patchMaskBufferStrideElements, tPatchSize, tPatchSize, patchBufferPaddingElements, patchMaskBufferPaddingElements, validMaskValue);
						}

						performanceDefault.stopIf(performanceIteration);

						break;
					}

					default:
						ocean_assert(false && "Invalid implementation type!");
						return false;
				}

				if (!CV::CVUtilities::isPaddingMemoryIdentical(patchBuffer, copyPatchBuffer))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (!CV::CVUtilities::isPaddingMemoryIdentical(patchMaskBuffer, copyPatchMaskBuffer))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				uint8_t testPatchBuffer[tChannels * tPatchSize * tPatchSize];
				uint8_t testPatchMaskBuffer[tPatchSize * tPatchSize];

				for (unsigned int n = 0u; n < locations; ++n)
				{
					if (interpolatePatchWithMask8BitPerChannel(frame, mask, tPatchSize, tPatchSize, positions[n], tPixelCenter, testPatchBuffer, testPatchMaskBuffer, validMaskValue))
					{
						bool patchValid = true;

						for (unsigned int y = 0u; y < tPatchSize; ++y)
						{
							if (memcmp(patchMaskBuffer.constrow<void>(n * tPatchSize + y), testPatchMaskBuffer + y * tPatchSize, size_t(tPatchSize) * sizeof(uint8_t)) != 0)
							{
								patchValid = false;
								break;
							}
						}

						if (patchValid)
						{
							for (unsigned int y = 0u; y < tPatchSize; ++y)
							{
								for (unsigned int x = 0u; x < tPatchSize; ++x)
								{
									if (testPatchMaskBuffer[y * tPatchSize + x] == validMaskValue)
									{
										if (memcmp((void*)(patchBuffer.constrow<uint8_t>(n * tPatchSize + y) + x * tChannels), (void*)(testPatchBuffer + (y * tPatchSize + x) * tChannels), size_t(tChannels) * sizeof(uint8_t)) != 0)
										{
											patchValid = false;
											break;
										}
									}
								}
							}

							if (patchValid)
							{
								++succeeded;
							}
						}
					}

					++iterations;
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	static_assert(locations != 0u, "Invalid number of locations!");

	ocean_assert(iterations != 0ull);

	const double percent = double(succeeded) / double(iterations);

	if (performanceNaive.measurements() != 0u)
	{
		Log::info() << "   Naive: [" << performanceNaive.bestMseconds() << ", " << performanceNaive.medianMseconds() << ", " << performanceNaive.worstMseconds() << "] ms";
	}

	if (performanceTemplate.measurements() != 0u)
	{
		Log::info() << "Template: [" << performanceTemplate.bestMseconds() << ", " << performanceTemplate.medianMseconds() << ", " << performanceTemplate.worstMseconds() << "] ms";
	}

	if (performanceSSE.measurements() != 0u)
	{
		Log::info() << "     SSE: [" << performanceSSE.bestMseconds() << ", " << performanceSSE.medianMseconds() << ", " << performanceSSE.worstMseconds() << "] ms";
	}

	if (performanceNEON.measurements() != 0u)
	{
		Log::info() << "    NEON: [" << performanceNEON.bestMseconds() << ", " << performanceNEON.medianMseconds() << ", " << performanceNEON.worstMseconds() << "] ms";
	}

	ocean_assert(performanceDefault.measurements() != 0u);
	Log::info() << " Default: [" << performanceDefault.bestMseconds() << ", " << performanceDefault.medianMseconds() << ", " << performanceDefault.worstMseconds() << "] ms";

	const bool allSucceeded = percent >= 0.995;

	if (allSucceeded)
	{
		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

template <unsigned int tChannels, unsigned int tPatchSize>
bool TestAdvancedFrameInterpolatorBilinear::testInterpolateSquareMirroredBorder(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize % 2u == 1u, "Invalid patch size!");

	ocean_assert(width >= tPatchSize + 1u);
	ocean_assert(height >= tPatchSize + 1u);

	ocean_assert(testDuration > 0.0);

	Log::info() << "... with " << tChannels << " channels and " << tPatchSize << "x" << tPatchSize << " patch test:";

	constexpr unsigned int locations = 10000u;

	Vectors2 positions(locations);

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performanceTemplate;
	HighPerformanceStatistic performanceSSE;
	HighPerformanceStatistic performanceNEON;
	HighPerformanceStatistic performanceDefault;

	uint64_t iterations = 0u;
	uint64_t succeeded = 0u;

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool performanceIteration : {true, false})
		{
			const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, tPatchSize + 2u, 1920u);
			const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, tPatchSize + 2u, 1080u);

			const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			for (Vector2& position : positions)
			{
				position.x() = Random::scalar(randomGenerator, Scalar(0), Scalar(frame.width()) - Numeric::weakEps());
				position.y() = Random::scalar(randomGenerator, Scalar(0), Scalar(frame.height()) - Numeric::weakEps());
			}

			const uint8_t* const frameData = frame.constdata<uint8_t>();
			const unsigned int framePaddingElements = frame.paddingElements();

			for (const ImplementationType implementationType : {IT_NAIVE, IT_TEMPLATE, IT_DEFAULT})
			{
				Frame buffer = CV::CVUtilities::randomizedFrame(FrameType(frame, tPatchSize * tPatchSize, locations), &randomGenerator);

				const Frame copyBuffer(buffer, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				uint8_t* const bufferData = buffer.data<uint8_t>();
				const unsigned int bufferStrideElements = buffer.strideElements();

				switch (implementationType)
				{
					case IT_NAIVE:
					{
						performanceNaive.startIf(performanceIteration);

						for (size_t n = 0; n < locations; ++n)
						{
							CV::Advanced::AdvancedFrameInterpolatorBilinearBase::interpolateSquareMirroredBorder8BitPerChannel<tChannels>(frameData, frame.width(), frame.height(), framePaddingElements, bufferData + n * bufferStrideElements, positions[n], tPatchSize);
						}

						performanceNaive.stopIf(performanceIteration);

						break;
					}

					case IT_TEMPLATE:
					{
						performanceTemplate.startIf(performanceIteration);

						for (size_t n = 0; n < locations; ++n)
						{
							CV::Advanced::AdvancedFrameInterpolatorBilinearBase::interpolateSquareMirroredBorderTemplate8BitPerChannel<tChannels, tPatchSize>(frameData, frame.width(), frame.height(), framePaddingElements, bufferData + n * bufferStrideElements, positions[n]);
						}

						performanceTemplate.stopIf(performanceIteration);

						break;
					}

					case IT_SSE:
					{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
						/*if constexpr (tChannels == 1u || tChannels == 3u) // not yet implemented
						{
							performanceSSE.startIf(performanceIteration);

							for (size_t n = 0; n < locations; ++n)
							{
								...
							}

							performanceSSE.stopIf(performanceIteration);
						}*/
#endif // OCEAN_HARDWARE_SSE_VERSION >= 41

						break;
					}

					case IT_NEON:
					{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
						/*if constexpr (tChannels == 1u) // not yet implemented
						{
							performanceNEON.startIf(performanceIteration);

							for (size_t n = 0; n < locations; ++n)
							{
								...
							}

							performanceNEON.stopIf(performanceIteration);
						}*/
#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

						break;
					}

					case IT_DEFAULT:
					{
						performanceDefault.startIf(performanceIteration);

						for (size_t n = 0; n < locations; ++n)
						{
							CV::Advanced::AdvancedFrameInterpolatorBilinear::interpolateSquareMirroredBorder8BitPerChannel<tChannels, tPatchSize>(frameData, frame.width(), frame.height(), framePaddingElements, bufferData + n * bufferStrideElements, positions[n]);
						}

						performanceDefault.stopIf(performanceIteration);

						break;
					}

					default:
						ocean_assert(false && "Invalid implementation type!");
						return false;
				}

				if (!CV::CVUtilities::isPaddingMemoryIdentical(buffer, copyBuffer))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				uint8_t testBuffer[tChannels * tPatchSize * tPatchSize];

				for (unsigned int n = 0u; n < locations; ++n)
				{
					interpolateSquarePatchMirroredBorder8BitPerChannel(frame, tPatchSize, positions[n], testBuffer);

					if (memcmp(buffer.constrow<void>(n), testBuffer, size_t(tChannels * tPatchSize * tPatchSize) * sizeof(uint8_t)) == 0)
					{
						++succeeded;
					}

					++iterations;
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	static_assert(locations != 0u, "Invalid number of locations!");

	ocean_assert(iterations != 0u);

	const double percent = double(succeeded) / double(iterations);

	if (performanceNaive.measurements() != 0u)
	{
		Log::info() << "   Naive: [" << performanceNaive.bestMseconds() << ", " << performanceNaive.medianMseconds() << ", " << performanceNaive.worstMseconds() << "] ms";
	}

	if (performanceTemplate.measurements() != 0u)
	{
		Log::info() << "Template: [" << performanceTemplate.bestMseconds() << ", " << performanceTemplate.medianMseconds() << ", " << performanceTemplate.worstMseconds() << "] ms";
	}

	if (performanceSSE.measurements() != 0u)
	{
		Log::info() << "     SSE: [" << performanceSSE.bestMseconds() << ", " << performanceSSE.medianMseconds() << ", " << performanceSSE.worstMseconds() << "] ms";
	}

	if (performanceNEON.measurements() != 0u)
	{
		Log::info() << "    NEON: [" << performanceNEON.bestMseconds() << ", " << performanceNEON.medianMseconds() << ", " << performanceNEON.worstMseconds() << "] ms";
	}

	ocean_assert(performanceDefault.measurements() != 0u);
	Log::info() << " Default: [" << performanceDefault.bestMseconds() << ", " << performanceDefault.medianMseconds() << ", " << performanceDefault.worstMseconds() << "] ms";

	const bool allSucceeded = percent >= 0.995;

	if (allSucceeded)
	{
		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestAdvancedFrameInterpolatorBilinear::interpolatePatch8BitPerChannel(const Frame& frame, const unsigned int patchWidth, const unsigned int patchHeight, const Vector2& position, const CV::PixelCenter pixelCenter, uint8_t* buffer)
{
	ocean_assert(frame.isValid() && buffer != nullptr);
	ocean_assert(frame.numberPlanes() == 1u && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	ocean_assert(patchWidth % 2u == 1u);
	ocean_assert(patchHeight % 2u == 1u);

	ocean_assert(patchWidth + 1u <= frame.width() && patchHeight + 1u <= frame.height());

	const unsigned int patchWidth_2 = patchWidth / 2u;
	const unsigned int patchHeight_2 = patchHeight / 2u;

	for (unsigned int y = 0u; y < patchHeight; ++y)
	{
		const Scalar yPosition = position.y() + Scalar(y) - Scalar(patchHeight_2);

		for (unsigned int x = 0u; x < patchWidth; ++x)
		{
			const Scalar xPosition = position.x() + Scalar(x) - Scalar(patchWidth_2);

			if (!CV::FrameInterpolatorBilinear::Comfort::interpolatePixel8BitPerChannel(frame.constdata<uint8_t>(), frame.channels(), frame.width(), frame.height(), frame.paddingElements(), pixelCenter, Vector2(xPosition, yPosition), buffer))
			{
				return false;
			}

			buffer += frame.channels();
		}
	}

	return true;
}

bool TestAdvancedFrameInterpolatorBilinear::interpolatePatchWithMask8BitPerChannel(const Frame& frame, const Frame& mask, const unsigned int patchWidth, const unsigned int patchHeight, const Vector2& position, const CV::PixelCenter pixelCenter, uint8_t* patchBuffer, uint8_t* patchMaskBuffer, const uint8_t validMaskValue)
{
	ocean_assert(frame.isValid() && mask.isValid());
	ocean_assert(frame.numberPlanes() == 1u && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);
	ocean_assert(mask.isPixelFormatCompatible(FrameType::FORMAT_Y8));

	ocean_assert(patchBuffer != nullptr && patchMaskBuffer != nullptr);

	ocean_assert(patchWidth >= 1u);
	ocean_assert(patchHeight >= 1u);

	const Scalar left = position.x() - Scalar(patchWidth - 1u) * Scalar(0.5);
	const Scalar top = position.y() - Scalar(patchHeight - 1u) * Scalar(0.5);

	for (unsigned int y = 0u; y < patchHeight; ++y)
	{
		const Scalar yPosition = top + Scalar(y);

		for (unsigned int x = 0u; x < patchWidth; ++x)
		{
			const Scalar xPosition = left + Scalar(x);

			if (!CV::Advanced::AdvancedFrameInterpolatorBilinear::Comfort::interpolatePixelWithMask8BitPerChannel(frame.constdata<uint8_t>(), mask.constdata<uint8_t>(), frame.channels(), frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), pixelCenter, Vector2(xPosition, yPosition), patchBuffer, *patchMaskBuffer, validMaskValue))
			{
				return false;
			}

			patchBuffer += frame.channels();
			++patchMaskBuffer;
		}
	}

	return true;
}

void TestAdvancedFrameInterpolatorBilinear::interpolateSquarePatchMirroredBorder8BitPerChannel(const Frame& frame, const unsigned int patchSize, const Vector2& position, uint8_t* buffer)
{
	ocean_assert(frame.isValid() && buffer != nullptr);
	ocean_assert(frame.numberPlanes() == 1u && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	ocean_assert(patchSize % 2u == 1u);
	ocean_assert(patchSize + 1u <= frame.width() && patchSize + 1u <= frame.height());

	const unsigned int patchSize_2 = patchSize / 2u;

	ocean_assert(position.x() >= Scalar(0) && position.x() < Scalar(frame.width()));
	ocean_assert(position.y() >= Scalar(0) && position.y() < Scalar(frame.height()));

	const unsigned int tx = (unsigned int)(128u * (position.x() - Scalar(int(position.x()))) + Scalar(0.5));
	const unsigned int ty = (unsigned int)(128u * (position.y() - Scalar(int(position.y()))) + Scalar(0.5));
	const unsigned int tx_ = 128u - tx;
	const unsigned int ty_ = 128u - ty;

	const Scalar squareLeftFloat = position.x() - Scalar(patchSize_2);
	const Scalar squareTopFloat = position.y() - Scalar(patchSize_2);

	int squareLeft = int(squareLeftFloat);
	int squareTop = int(squareTopFloat);

	if (squareLeftFloat < 0 && squareLeftFloat != Scalar(int(squareLeftFloat)))
	{
		--squareLeft;
	}

	if (squareTopFloat < 0 && squareTopFloat != Scalar(int(squareTopFloat)))
	{
		--squareTop;
	}

	const int width = int(frame.width());
	const int height = int(frame.height());

	for (int yy = 0; yy < int(patchSize); ++yy)
	{
		for (int xx = 0; xx < int(patchSize); ++xx)
		{
			int left = squareLeft + xx;
			int top = squareTop + yy;

			int right = left + 1;
			int bottom = top + 1;

			if (left < 0)
			{
				left = -left - 1;
			}
			else if (left >= width)
			{
				left = width - (left - width) - 1;
			}

			if (top < 0)
			{
				top = -top - 1;
			}
			else if (top >= height)
			{
				top = height - (top - height) - 1;
			}

			if (right < 0)
			{
				right = -right - 1;
			}
			else if (right >= width)
			{
				right = width - (right - width) - 1;
			}

			if (bottom < 0)
			{
				bottom = -bottom - 1;
			}
			else if (bottom >= height)
			{
				bottom = height - (bottom - height) - 1;
			}

			ocean_assert(left >= 0 && right < width);
			ocean_assert(top >= 0 && bottom < height);

			const uint8_t* pixelTopLeft = frame.constpixel<uint8_t>(left, top);
			const uint8_t* pixelTopRight = frame.constpixel<uint8_t>(right, top);
			const uint8_t* pixelBottomLeft = frame.constpixel<uint8_t>(left, bottom);
			const uint8_t* pixelBottomRight = frame.constpixel<uint8_t>(right, bottom);

			for (unsigned int n = 0u; n < frame.channels(); ++n)
			{
				const unsigned int valueTopLeft = pixelTopLeft[n];
				const unsigned int valueTopRight = pixelTopRight[n];

				const unsigned int valueBottomLeft = pixelBottomLeft[n];
				const unsigned int valueBottomRight = pixelBottomRight[n];

				const unsigned int valueTop = valueTopLeft * tx_ + valueTopRight * tx;
				const unsigned int valueBottom = valueBottomLeft * tx_ + valueBottomRight * tx;

				const unsigned int value = valueTop * ty_ + valueBottom * ty;

				ocean_assert(value >= 0u && value <= 255u * 16384u);

				const unsigned int result = (value + 8192u) / 16384u;
				ocean_assert(result <= 255u);

				*buffer++ = uint8_t(result);
			}
		}
	}
}

bool TestAdvancedFrameInterpolatorBilinear::testHomographyFilterMask(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	const IndexPairs32 dimensions =
	{
		{640u, 480u},
		{1280u, 720u},
		{1920u, 1080u},
		{3840u, 2160u}
	};

	Log::info() << "Homography interpolation test (with binary filter mask):";
	Log::info() << " ";

	bool allSucceeded = true;

	for (const IndexPair32& dimension : dimensions)
	{
		for (unsigned int channel = 1u; channel <= 4u; ++channel)
		{
			allSucceeded = testHomographyFilterMask(dimension.first, dimension.second, channel, testDuration, worker) && allSucceeded;
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

bool TestAdvancedFrameInterpolatorBilinear::testHomographyFilterMask(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
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

			const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			ocean_assert(sourceFrame.width() > 10u);
			const unsigned int targetWidth = RandomI::random(randomGenerator, sourceFrame.width() - 10u, sourceFrame.width() + 10u);
			const unsigned int targetHeight = RandomI::random(randomGenerator, sourceFrame.height() - 10u, sourceFrame.height() + 10u);

			Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceFrame, targetWidth, targetHeight), &randomGenerator);

			constexpr uint8_t maskValue = 0xFF;

			const Frame targetFilterMask = CV::CVUtilities::randomizedBinaryMask(targetWidth, targetHeight, maskValue, &randomGenerator);

			const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			CV::PixelBoundingBox boundingBox;

			if (RandomI::random(randomGenerator, 1u) == 0u)
			{
				const unsigned int left = RandomI::random(randomGenerator, targetWidth - 1u);
				const unsigned int right = RandomI::random(randomGenerator, left, targetWidth - 1u);

				const unsigned int top = RandomI::random(randomGenerator, targetHeight - 1u);
				const unsigned int bottom = RandomI::random(randomGenerator, top, targetHeight - 1u);

				boundingBox = CV::PixelBoundingBox(left, top, right, bottom);
			}

			performance.start();
				if (!CV::Advanced::AdvancedFrameInterpolatorBilinear::Comfort::homographyFilterMask(sourceFrame, targetFilterMask, targetFrame, input_H_output, boundingBox, useWorker))
				{
					allSucceeded = false;
				}
			performance.stop();

			if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
			{
				ocean_assert(false && "Invalid padding memory!");
			}

			if (!validateHomographyFilterMask8BitPerChannel(sourceFrame, targetFilterMask, copyTargetFrame, targetFrame, input_H_output, boundingBox))
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

	return allSucceeded;
}

template <typename TScalar>
bool TestAdvancedFrameInterpolatorBilinear::validateInterpolatePixel8BitPerChannel(const Frame& frame, const Frame& mask, const VectorT2<TScalar>& position, const CV::PixelCenter pixelCenter, const uint8_t maskValue, const uint8_t* const interpolationResult, const uint8_t maskResult, const TScalar threshold)
{
	ocean_assert(frame.isValid());
	ocean_assert(mask.isValid() && mask.isPixelFormatCompatible(FrameType::FORMAT_Y8));
	ocean_assert(threshold >= 0 && threshold < 255);

	VectorT2<TScalar> shiftedPosition = position;

	if (pixelCenter == CV::PC_CENTER)
	{
		shiftedPosition -= VectorT2<TScalar>(TScalar(0.5), TScalar(0.5));
	}

	const int pixelLeft = int(NumericT<TScalar>::floor(shiftedPosition.x()));
	const int pixelTop = int(NumericT<TScalar>::floor(shiftedPosition.y()));

	const TScalar sFactorRight = shiftedPosition.x() - TScalar(pixelLeft);
	const TScalar sFactorBottom = shiftedPosition.y() - TScalar(pixelTop);

	const unsigned int factorRight = (unsigned int)(sFactorRight * TScalar(128) + TScalar(0.5));
	const unsigned int factorBottom = (unsigned int)(sFactorBottom * TScalar(128) + TScalar(0.5));

	const unsigned int factorLeft = 128u - factorRight;
	const unsigned int factorTop = 128u - factorBottom;

	const unsigned int factorTopLeft = factorTop * factorLeft;
	const unsigned int factorTopRight = factorTop * factorRight;
	const unsigned int factorBottomLeft = factorBottom * factorLeft;
	const unsigned int factorBottomRight = factorBottom * factorRight;

	const int pixelRight = pixelLeft + 1;
	const int pixelBottom = pixelTop + 1;

	unsigned int factorSum = 0u;

	const bool leftValid = pixelLeft >= 0 && pixelLeft < int(frame.width());
	const bool rightValid = pixelRight >= 0 && pixelRight < int(frame.width());
	const bool topValid = pixelTop >= 0 && pixelTop < int(frame.height());
	const bool bottomValid = pixelBottom >= 0 && pixelBottom < int(frame.height());

	std::vector<unsigned int> result(frame.channels(), 0u);

	if (topValid)
	{
		if (leftValid && mask.constpixel<uint8_t>((unsigned int)(pixelLeft), (unsigned int)(pixelTop))[0] == maskValue)
		{
			const uint8_t* const pixel = frame.constpixel<uint8_t>((unsigned int)(pixelLeft), (unsigned int)(pixelTop));

			for (unsigned int n = 0u; n < frame.channels(); ++n)
			{
				result[n] += pixel[n] * factorTopLeft;
			}

			factorSum += factorTopLeft;
		}

		if (rightValid && mask.constpixel<uint8_t>((unsigned int)(pixelRight), (unsigned int)(pixelTop))[0] == maskValue)
		{
			const uint8_t* const pixel = frame.constpixel<uint8_t>((unsigned int)(pixelRight), (unsigned int)(pixelTop));

			for (unsigned int n = 0u; n < frame.channels(); ++n)
			{
				result[n] += pixel[n] * factorTopRight;
			}

			factorSum += factorTopRight;
		}
	}

	if (bottomValid)
	{
		if (leftValid && mask.constpixel<uint8_t>((unsigned int)(pixelLeft), (unsigned int)(pixelBottom))[0] == maskValue)
		{
			const uint8_t* const pixel = frame.constpixel<uint8_t>((unsigned int)(pixelLeft), (unsigned int)(pixelBottom));

			for (unsigned int n = 0u; n < frame.channels(); ++n)
			{
				result[n] += pixel[n] * factorBottomLeft;
			}

			factorSum += factorBottomLeft;
		}

		if (rightValid && mask.constpixel<uint8_t>((unsigned int)(pixelRight), (unsigned int)(pixelBottom))[0] == maskValue)
		{
			const uint8_t* const pixel = frame.constpixel<uint8_t>((unsigned int)(pixelRight), (unsigned int)(pixelBottom));

			for (unsigned int n = 0u; n < frame.channels(); ++n)
			{
				result[n] += pixel[n] * factorBottomRight;
			}

			factorSum += factorBottomRight;
		}
	}

	if (factorSum == 128u * 128u)
	{
		// all pixels are valid, so the interpolation result must be identical to the non-mask version from 'FrameInterpolatorBilinear'

		if (leftValid && rightValid && topValid && bottomValid)
		{
			std::vector<uint8_t> testResult(frame.channels());

			if (!CV::FrameInterpolatorBilinear::Comfort::interpolatePixel8BitPerChannel<TScalar>(frame.constdata<uint8_t>(), frame.channels(), frame.width(), frame.height(), frame.paddingElements(), pixelCenter, position, testResult.data()))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			for (unsigned int n = 0u; n < frame.channels(); ++n)
			{
				if (interpolationResult[n] != testResult[n])
				{
					return false;
				}
			}
		}
	}

	if (factorSum == 0u)
	{
		if (maskResult != 0xFFu - maskValue)
		{
			return false;
		}
	}
	else
	{
		ocean_assert(factorSum <= 128u * 128u);

		for (unsigned int n = 0u; n < frame.channels(); ++n)
		{
			const TScalar normalizedResult = TScalar((result[n] + factorSum / 2u) / factorSum);

			if (NumericT<TScalar>::isNotEqual(TScalar(interpolationResult[n]), normalizedResult, threshold))
			{
				Log::info() << TScalar(interpolationResult[n]) << " vs " << result[n];
				return false;
			}
		}

		if (maskResult != maskValue)
		{
			return false;
		}
	}

	return true;
}

bool TestAdvancedFrameInterpolatorBilinear::validateHomographyFilterMask8BitPerChannel(const Frame& inputFrame, const Frame& outputFilterMask, const Frame& outputFrame, const Frame& interpolatedOutputFrame, const SquareMatrix3& input_H_output, const CV::PixelBoundingBox& boundingBox)
{
	ocean_assert(inputFrame && outputFrame);

	ocean_assert(inputFrame.pixelFormat() == outputFrame.pixelFormat() && inputFrame.pixelOrigin() == outputFrame.pixelOrigin());
	ocean_assert(inputFrame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	ocean_assert(outputFrame.width() == outputFilterMask.width() && outputFrame.height() == outputFilterMask.height());
	ocean_assert(outputFrame.pixelOrigin() == outputFilterMask.pixelOrigin());
	ocean_assert(FrameType::formatIsGeneric(outputFilterMask.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));

	ocean_assert(!input_H_output.isSingular());

	const unsigned int channels = inputFrame.channels();
	ocean_assert(channels <= 4u);

	unsigned int left = 0u;
	unsigned int top = 0u;
	unsigned int rightEnd = outputFrame.width();
	unsigned int bottomEnd = outputFrame.height();

	if (boundingBox.isValid())
	{
		left = boundingBox.left();
		top = boundingBox.top();
		rightEnd = boundingBox.rightEnd();
		bottomEnd = boundingBox.bottomEnd();
	}

	unsigned int invalidValues = 0u;

	std::vector<uint8_t> pixelResult(channels);

	for (unsigned int y = top; y < bottomEnd; ++y)
	{
		for (unsigned int x = left; x < rightEnd; ++x)
		{
			const uint8_t* const outputFramePixel = outputFrame.constpixel<uint8_t>(x, y);
			const uint8_t* const interpolatedOutputFramePixel = interpolatedOutputFrame.constpixel<uint8_t>(x, y);

			const uint8_t outputFilterMaskPixel = outputFilterMask.constpixel<uint8_t>(x, y)[0];

			if (outputFilterMaskPixel != 0xFFu)
			{
				const Vector2 inputPosition = input_H_output * Vector2(Scalar(x), Scalar(y));

				const Vector2 clampedInputPosition(minmax<Scalar>(0, inputPosition.x(), Scalar(inputFrame.width() - 1u)), minmax<Scalar>(0, inputPosition.y(), Scalar(inputFrame.height() - 1u)));

				switch (channels)
				{
					case 1u:
						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<1u, CV::PC_TOP_LEFT>(inputFrame.constdata<uint8_t>(), inputFrame.width(), inputFrame.height(), inputFrame.paddingElements(), clampedInputPosition, pixelResult.data());
						break;

					case 2u:
						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<2u, CV::PC_TOP_LEFT>(inputFrame.constdata<uint8_t>(), inputFrame.width(), inputFrame.height(), inputFrame.paddingElements(), clampedInputPosition, pixelResult.data());
						break;

					case 3u:
						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<3u, CV::PC_TOP_LEFT>(inputFrame.constdata<uint8_t>(), inputFrame.width(), inputFrame.height(), inputFrame.paddingElements(), clampedInputPosition, pixelResult.data());
						break;

					case 4u:
						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<4u, CV::PC_TOP_LEFT>(inputFrame.constdata<uint8_t>(), inputFrame.width(), inputFrame.height(), inputFrame.paddingElements(), clampedInputPosition, pixelResult.data());
						break;

					default:
						ocean_assert(false && "Invalid frame number!");
						return false;
				}

				if (memcmp(interpolatedOutputFramePixel, pixelResult.data(), sizeof(uint8_t) * channels) != 0)
				{
					return false;
				}
			}
			else
			{
				if (memcmp(outputFramePixel, interpolatedOutputFramePixel, sizeof(uint8_t) * channels) != 0)
				{
					return false;
				}
			}
		}
	}

	ocean_assert(invalidValues <= interpolatedOutputFrame.size()); // size - as we determine invalid values per pixel and channel
	const double percent = double(interpolatedOutputFrame.size() - invalidValues) / double(interpolatedOutputFrame.size());

	return percent >= 0.995;
}

}

}

}

}
