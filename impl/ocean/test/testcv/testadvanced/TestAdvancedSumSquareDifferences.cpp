/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testadvanced/TestAdvancedSumSquareDifferences.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"

#include "ocean/cv/advanced/AdvancedFrameInterpolatorBilinear.h"
#include "ocean/cv/advanced/AdvancedSumSquareDifferences.h"
#include "ocean/cv/advanced/AdvancedSumSquareDifferencesSSE.h"

#include "ocean/math/Random.h"
#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestAdvanced
{

bool TestAdvancedSumSquareDifferences::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Advanced sum square differences test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testTwoSubPixelPatch8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Advanced sum square differences test succeeded.";
	}
	else
	{
		Log::info() << "Advanced sum square differences test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestAdvancedSumSquareDifferences, TwoSubPixelPatch8BitPerChannel)
{
	EXPECT_TRUE(TestAdvancedSumSquareDifferences::testTwoSubPixelPatch8BitPerChannel(GTEST_TEST_DURATION));
}

TEST(TestAdvancedSumSquareDifferences, OneSubPixelPatch8BitPerChannel)
{
	EXPECT_TRUE(TestAdvancedSumSquareDifferences::testOneSubPixelPatch8BitPerChannel(GTEST_TEST_DURATION));
}

TEST(TestAdvancedSumSquareDifferences, PatchBuffer8BitPerChannel)
{
	EXPECT_TRUE(TestAdvancedSumSquareDifferences::testPatchBuffer8BitPerChannel(GTEST_TEST_DURATION));
}

TEST(TestAdvancedSumSquareDifferences, PatchMirroredBorderBuffer8BitPerChannel)
{
	EXPECT_TRUE(TestAdvancedSumSquareDifferences::testPatchMirroredBorderBuffer8BitPerChannel(GTEST_TEST_DURATION));
}

TEST(TestAdvancedSumSquareDifferences, TwoPixelPatchWithMask8BitPerChannel)
{
	EXPECT_TRUE(TestAdvancedSumSquareDifferences::testTwoPixelPatchWithMask8BitPerChannel(GTEST_TEST_DURATION));
}

TEST(TestAdvancedSumSquareDifferences, TwoSubPixelPatchWithMask8BitPerChannel)
{
	EXPECT_TRUE(TestAdvancedSumSquareDifferences::testTwoSubPixelPatchWithMask8BitPerChannel(GTEST_TEST_DURATION));
}

TEST(TestAdvancedSumSquareDifferences, TwoPixelPatchWithRejectingMask8BitPerChannel)
{
	EXPECT_TRUE(TestAdvancedSumSquareDifferences::testTwoPixelPatchWithRejectingMask8BitPerChannel(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestAdvancedSumSquareDifferences::testTwoSubPixelPatch8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing two sub-pixel SSD between two patches:";
	Log::info() << " ";

	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;

	bool allSucceeded = true;

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<1u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<2u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<3u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<4u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<1u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<2u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<3u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<4u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<1u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<2u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<3u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<4u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<1u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<2u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<3u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<4u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<1u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<2u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<3u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<4u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<1u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<2u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<3u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<4u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<1u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<2u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<3u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatch8BitPerChannel<4u, 31u>(width, height, testDuration) && allSucceeded;

	return allSucceeded;
}

bool TestAdvancedSumSquareDifferences::testOneSubPixelPatch8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing one sub-pixel accurate and one pixel accurate SSD between two patches:";
	Log::info() << " ";

	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;

	bool allSucceeded = true;

	allSucceeded = testOneSubPixelPatch8BitPerChannel<1u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<2u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<3u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<4u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<1u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<2u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<3u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<4u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<1u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<2u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<3u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<4u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<1u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<2u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<3u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<4u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<1u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<2u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<3u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<4u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<1u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<2u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<3u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<4u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<1u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<2u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<3u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testOneSubPixelPatch8BitPerChannel<4u, 31u>(width, height, testDuration) && allSucceeded;

	return allSucceeded;
}

bool TestAdvancedSumSquareDifferences::testPatchBuffer8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SSD between a sub-pixel patch and a buffer:";
	Log::info() << " ";

	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;

	bool allSucceeded = true;

	allSucceeded = testPatchBuffer8BitPerChannel<1u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<2u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<3u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<4u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<1u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<2u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<3u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<4u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<1u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<2u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<3u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<4u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<1u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<2u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<3u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<4u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<1u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<2u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<3u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<4u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<1u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<2u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<3u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<4u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<1u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<2u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<3u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<4u, 31u>(width, height, testDuration) && allSucceeded;

	return allSucceeded;
}

bool TestAdvancedSumSquareDifferences::testPatchMirroredBorderBuffer8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SSD between a sub-pixel patch (mirrored at the image border) and a buffer:";
	Log::info() << " ";

	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;

	bool allSucceeded = true;

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<1u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<2u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<3u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<4u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<1u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<2u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<3u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<4u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<1u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<2u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<3u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<4u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<1u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<2u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<3u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<4u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<1u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<2u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<3u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<4u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<1u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<2u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<3u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<4u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<1u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<2u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<3u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorderBuffer8BitPerChannel<4u, 31u>(width, height, testDuration) && allSucceeded;

	return allSucceeded;
}

bool TestAdvancedSumSquareDifferences::testTwoPixelPatchWithMask8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing two pixel accurate SSD between two patches combined with a mask:";
	Log::info() << " ";

	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;

	bool allSucceeded = true;

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<1u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<2u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<3u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<4u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<1u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<2u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<3u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<4u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<1u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<2u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<3u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<4u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<1u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<2u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<3u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<4u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<1u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<2u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<3u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<4u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<1u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<2u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<3u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<4u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<1u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<2u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<3u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithMask8BitPerChannel<4u, 31u>(width, height, testDuration) && allSucceeded;

	return allSucceeded;
}

bool TestAdvancedSumSquareDifferences::testTwoSubPixelPatchWithMask8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing two sub-pixel accurate SSD between two patches combined with a mask:";
	Log::info() << " ";

	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;

	bool allSucceeded = true;

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<1u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<2u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<3u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<4u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<1u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<2u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<3u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<4u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<1u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<2u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<3u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<4u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<1u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<2u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<3u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<4u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<1u, 8u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<2u, 8u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<3u, 8u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<4u, 8u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<1u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<2u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<3u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<4u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<1u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<2u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<3u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<4u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<1u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<2u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<3u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoSubPixelPatchWithMask8BitPerChannel<4u, 31u>(width, height, testDuration) && allSucceeded;

	return allSucceeded;
}

bool TestAdvancedSumSquareDifferences::testTwoPixelPatchWithRejectingMask8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing two pixel accurate SSD between two patches combined with a rejecting mask:";
	Log::info() << " ";

	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;

	bool allSucceeded = true;

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<1u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<2u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<3u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<4u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<1u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<2u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<3u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<4u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<1u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<2u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<3u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<4u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<1u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<2u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<3u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<4u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<1u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<2u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<3u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<4u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<1u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<2u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<3u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<4u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<1u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<2u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<3u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchWithRejectingMask8BitPerChannel<4u, 31u>(width, height, testDuration) && allSucceeded;

	return allSucceeded;
}

template <unsigned int tChannels, unsigned int tPatchSize>
bool TestAdvancedSumSquareDifferences::testTwoSubPixelPatch8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize % 2u == 1u, "Invalid size");

	ocean_assert(width >= tPatchSize && height >= tPatchSize);
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	constexpr size_t locations = 10000;

	Log::info() << "... with " << tChannels << " channels and " << tPatchSize * tPatchSize << " pixels (" << tPatchSize << "x" << tPatchSize << ") at " << locations << " locations:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performanceTemplate;
	HighPerformanceStatistic performanceSSE;
	HighPerformanceStatistic performanceNEON;
	HighPerformanceStatistic performanceDefault;

	Scalars centersX0(locations);
	Scalars centersY0(locations);
	Scalars centersX1(locations);
	Scalars centersY1(locations);

	Indices32 resultsNaive;
	Indices32 resultsTemplate;
	Indices32 resultsSSE;
	Indices32 resultsNEON;
	Indices32 resultsDefault;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width0 = RandomI::random(randomGenerator, width - 1u, width + 1u);
		const unsigned int height0 = RandomI::random(randomGenerator, height - 1u, height + 1u);

		const unsigned int width1 = RandomI::random(randomGenerator, width - 1u, width + 1u);
		const unsigned int height1 = RandomI::random(randomGenerator, height - 1u, height + 1u);

		const unsigned int paddingElements0 = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int paddingElements1 = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		Frame frame0(FrameType(width0, height0, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), paddingElements0);
		Frame frame1(FrameType(width1, height1, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), paddingElements1);

		CV::CVUtilities::randomizeFrame(frame0, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(frame1, false, &randomGenerator);

		for (unsigned int n = 0u; n < locations; ++n)
		{
			centersX0[n] = Random::scalar(randomGenerator, Scalar(tPatchSize_2), Scalar(width0 - tPatchSize_2 - 1u) - Numeric::weakEps());
			centersY0[n] = Random::scalar(randomGenerator, Scalar(tPatchSize_2), Scalar(height0 - tPatchSize_2 - 1u) - Numeric::weakEps());

			centersX1[n] = Random::scalar(randomGenerator, Scalar(tPatchSize_2), Scalar(width1 - tPatchSize_2 - 1u) - Numeric::weakEps());
			centersY1[n] = Random::scalar(randomGenerator, Scalar(tPatchSize_2), Scalar(height1 - tPatchSize_2 - 1u) - Numeric::weakEps());
		}

		const uint8_t* const data0 = frame0.constdata<uint8_t>();
		const uint8_t* const data1 = frame1.constdata<uint8_t>();

		for (const ImplementationType implementationType : {IT_NAIVE, IT_TEMPLATE, IT_SSE, IT_NEON, IT_DEFAULT})
		{
			switch (implementationType)
			{
				case IT_NAIVE:
				{
					resultsNaive.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceNaive);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsNaive[n] = CV::Advanced::AdvancedSumSquareDifferencesBase::patch8BitPerChannel(data0, data1, tChannels, tPatchSize, width0, width1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
					}

					break;
				}

				case IT_TEMPLATE:
				{
					resultsTemplate.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceTemplate);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsTemplate[n] = CV::Advanced::AdvancedSumSquareDifferencesBase::patch8BitPerChannelTemplate<tChannels, tPatchSize>(data0, data1, width0, width1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
					}

					break;
				}

				case IT_SSE:
				{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
					if constexpr (tPatchSize == 5u)
					{
						resultsSSE.resize(locations);

						const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceSSE);

						for (size_t n = 0; n < locations; ++n)
						{
							resultsSSE[n] = CV::Advanced::AdvancedSumSquareDifferencesSSE::patch8BitPerChannel<tChannels, tPatchSize>(data0, data1, width0, width1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
						}
					}
#endif // defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

					break;
				}

				case IT_NEON:
				{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
					/*if constexpr (tPatchSize >= 5u) // not yet implemented
					{
						resultsNEON.resize(locations);

						const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceNEON);

						const unsigned int patch0StrideElements = frame0.strideElements();
						const unsigned int patch1StrideElements = frame1.strideElements();

						for (size_t n = 0; n < locations; ++n)
						{
							...
						}
					}*/
#endif // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

					break;
				}

				case IT_DEFAULT:
				{
					resultsDefault.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceDefault);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsDefault[n] = CV::Advanced::AdvancedSumSquareDifferences::patch8BitPerChannel<tChannels, tPatchSize>(data0, data1, width0, width1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
					}

					break;
				}

				default:
					ocean_assert(false && "Invalid implementation type!");
					break;
			}
		}

		for (size_t n = 0; n < locations; ++n)
		{
			const uint32_t ssdTest = calculate8BitPerChannel(frame0, frame1, Vector2(centersX0[n], centersY0[n]), Vector2(centersX1[n], centersY1[n]), tPatchSize);

			if (!resultsNaive.empty() && resultsNaive[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsTemplate.empty() && resultsTemplate[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsSSE.empty() && resultsSSE[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsNEON.empty() && resultsNEON[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (resultsDefault[n] != ssdTest)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	static_assert(locations != 0, "Invalid number of locations!");

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

template <unsigned int tChannels, unsigned int tPatchSize>
bool TestAdvancedSumSquareDifferences::testOneSubPixelPatch8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize % 2u == 1u, "Invalid size");

	ocean_assert(width >= tPatchSize && height >= tPatchSize);
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	constexpr size_t locations = 10000;

	Log::info() << "... with " << tChannels << " channels and " << tPatchSize * tPatchSize << " pixels (" << tPatchSize << "x" << tPatchSize << ") at " << locations << " locations:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performanceTemplate;
	HighPerformanceStatistic performanceSSE;
	HighPerformanceStatistic performanceNEON;
	HighPerformanceStatistic performanceDefault;

	Indices32 centersX0(locations);
	Indices32 centersY0(locations);
	Scalars centersX1(locations);
	Scalars centersY1(locations);

	Indices32 resultsNaive;
	Indices32 resultsTemplate;
	Indices32 resultsSSE;
	Indices32 resultsNEON;
	Indices32 resultsDefault;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width0 = RandomI::random(randomGenerator, width - 1u, width + 1u);
		const unsigned int height0 = RandomI::random(randomGenerator, height - 1u, height + 1u);

		const unsigned int width1 = RandomI::random(randomGenerator, width - 1u, width + 1u);
		const unsigned int height1 = RandomI::random(randomGenerator, height - 1u, height + 1u);

		const unsigned int paddingElements0 = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int paddingElements1 = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		Frame frame0(FrameType(width0, height0, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), paddingElements0);
		Frame frame1(FrameType(width1, height1, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), paddingElements1);

		CV::CVUtilities::randomizeFrame(frame0, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(frame1, false, &randomGenerator);

		for (unsigned int n = 0u; n < locations; ++n)
		{
			centersX0[n] = RandomI::random(randomGenerator, tPatchSize_2, width0 - tPatchSize_2 - 1u);
			centersY0[n] = RandomI::random(randomGenerator, tPatchSize_2, height0 - tPatchSize_2 - 1u);

			centersX1[n] = Random::scalar(randomGenerator, Scalar(tPatchSize_2), Scalar(width1 - tPatchSize_2 - 1u) - Numeric::weakEps());
			centersY1[n] = Random::scalar(randomGenerator, Scalar(tPatchSize_2), Scalar(height1 - tPatchSize_2 - 1u) - Numeric::weakEps());
		}

		const uint8_t* const data0 = frame0.constdata<uint8_t>();
		const uint8_t* const data1 = frame1.constdata<uint8_t>();

		for (const ImplementationType implementationType : {IT_NAIVE, IT_TEMPLATE, IT_SSE, IT_NEON, IT_DEFAULT})
		{
			switch (implementationType)
			{
				case IT_NAIVE:
				{
					resultsNaive.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceNaive);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsNaive[n] = CV::Advanced::AdvancedSumSquareDifferencesBase::patch8BitPerChannel(data0, data1, tChannels, tPatchSize, width0, width1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
					}

					break;
				}

				case IT_TEMPLATE:
				{
					resultsTemplate.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceTemplate);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsTemplate[n] = CV::Advanced::AdvancedSumSquareDifferencesBase::patch8BitPerChannelTemplate<tChannels, tPatchSize>(data0, data1, width0, width1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
					}

					break;
				}

				case IT_SSE:
				{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
					if constexpr (tPatchSize == 5u)
					{
						resultsSSE.resize(locations);

						const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceSSE);

						for (size_t n = 0; n < locations; ++n)
						{
							resultsSSE[n] = CV::Advanced::AdvancedSumSquareDifferencesSSE::patch8BitPerChannel<tChannels, tPatchSize>(data0, data1, width0, width1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
						}
					}
#endif // defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

					break;
				}

				case IT_NEON:
				{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
					/*if constexpr (tPatchSize >= 5u) // not yet implemented
					{
						resultsNEON.resize(locations);

						const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceNEON);

						for (size_t n = 0; n < locations; ++n)
						{
							...
						}
					}*/
#endif // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

					break;
				}

				case IT_DEFAULT:
				{
					resultsDefault.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceDefault);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsDefault[n] = CV::Advanced::AdvancedSumSquareDifferences::patch8BitPerChannel<tChannels, tPatchSize>(data0, data1, width0, width1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
					}

					break;
				}

				default:
					ocean_assert(false && "Invalid implementation type!");
					break;
			}
		}

		for (size_t n = 0; n < locations; ++n)
		{
			const uint32_t ssdTest = calculate8BitPerChannel(frame0, frame1, CV::PixelPosition(centersX0[n], centersY0[n]), Vector2(centersX1[n], centersY1[n]), tPatchSize);

			if (!resultsNaive.empty() && resultsNaive[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsTemplate.empty() && resultsTemplate[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsSSE.empty() && resultsSSE[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsNEON.empty() && resultsNEON[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (resultsDefault[n] != ssdTest)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	static_assert(locations != 0, "Invalid number of locations!");

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

template <unsigned int tChannels, unsigned int tPatchSize>
bool TestAdvancedSumSquareDifferences::testPatchBuffer8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize % 2u == 1u, "Invalid size");

	ocean_assert(width >= tPatchSize && height >= tPatchSize);
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;
	constexpr unsigned int tBufferSize = tChannels * tPatchSize * tPatchSize;

	Log::info() << "... with " << tChannels << " channels and " << tPatchSize * tPatchSize << " pixels (" << tPatchSize << "x" << tPatchSize << "):";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performanceTemplate;
	HighPerformanceStatistic performanceSSE;
	HighPerformanceStatistic performanceNEON;
	HighPerformanceStatistic performanceDefault;

	constexpr size_t locations = 10000;

	Scalars centersX0(locations);
	Scalars centersY0(locations);
	Indices32 offsets1(locations);

	Indices32 resultsNaive;
	Indices32 resultsTemplate;
	Indices32 resultsSSE;
	Indices32 resultsNEON;
	Indices32 resultsDefault;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width0 = RandomI::random(randomGenerator, width - 1u, width + 1u);
		const unsigned int height0 = RandomI::random(randomGenerator, height - 1u, height + 1u);

		const unsigned int width1 = RandomI::random(randomGenerator, width - 1u, width + 1u);
		const unsigned int height1 = RandomI::random(randomGenerator, height - 1u, height + 1u);

		const unsigned int paddingElements0 = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int paddingElements1 = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		Frame frame0(FrameType(width0, height0, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), paddingElements0);
		Frame frame1(FrameType(width1, height1, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), paddingElements1);

		CV::CVUtilities::randomizeFrame(frame0, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(frame1, false, &randomGenerator);

		for (unsigned int n = 0u; n < locations; ++n)
		{
			centersX0[n] = Random::scalar(randomGenerator, Scalar(tPatchSize_2), Scalar(width0 - tPatchSize_2 - 2u) - Numeric::weakEps());
			centersY0[n] = Random::scalar(randomGenerator, Scalar(tPatchSize_2), Scalar(height0 - tPatchSize_2 - 2u) - Numeric::weakEps());

			offsets1[n] = RandomI::random(randomGenerator, height1 * frame1.strideElements() - tBufferSize - 1u);
		}

		const uint8_t* const data0 = frame0.constdata<uint8_t>();
		const uint8_t* const data1 = frame1.constdata<uint8_t>();

		for (const ImplementationType implementationType : {IT_NAIVE, IT_TEMPLATE, IT_SSE, IT_NEON, IT_DEFAULT})
		{
			switch (implementationType)
			{
				case IT_NAIVE:
				{
					resultsNaive.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceNaive);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsNaive[n] = CV::Advanced::AdvancedSumSquareDifferencesBase::patchBuffer8BitPerChannel(data0, tChannels, tPatchSize, width0, centersX0[n], centersY0[n], paddingElements0, data1 + offsets1[n]);
					}

					break;
				}

				case IT_TEMPLATE:
				{
					resultsTemplate.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceTemplate);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsTemplate[n] = CV::Advanced::AdvancedSumSquareDifferencesBase::patchBuffer8BitPerChannelTemplate<tChannels, tPatchSize>(data0, width0, centersX0[n], centersY0[n], paddingElements0, data1 + offsets1[n]);
					}

					break;
				}

				case IT_SSE:
				{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
					if constexpr (tPatchSize >= 5u)
					{
						/*resultsSSE.resize(locations);

						const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceSSE);

						const unsigned int patch0StrideElements = frame0.strideElements();

						for (size_t n = 0; n < locations; ++n)
						{
							...
						}*/
					}
#endif // defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

					break;
				}

				case IT_NEON:
				{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
					/*if constexpr (tPatchSize >= 5u) not yet implemented
					{
						resultsNEON.resize(locations);

						const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceNEON);

						const unsigned int patch0StrideElements = frame0.strideElements();

						for (size_t n = 0; n < locations; ++n)
						{
							...
						}
					}*/
#endif // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

					break;
				}

				case IT_DEFAULT:
				{
					resultsDefault.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceDefault);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsDefault[n] = CV::Advanced::AdvancedSumSquareDifferences::patchBuffer8BitPerChannel<tChannels, tPatchSize>(data0, width0, centersX0[n], centersY0[n], paddingElements0, data1 + offsets1[n]);
					}

					break;
				}

				default:
					ocean_assert(false && "Invalid implementation type!");
					break;
			}
		}

		for (size_t n = 0; n < locations; ++n)
		{
			const uint32_t ssdTest = calculate8BitPerChannel(frame0, Vector2(centersX0[n], centersY0[n]), data1 + offsets1[n], tPatchSize);

			if (!resultsNaive.empty() && resultsNaive[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsTemplate.empty() && resultsTemplate[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsSSE.empty() && resultsSSE[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsNEON.empty() && resultsNEON[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (resultsDefault[n] != ssdTest)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	static_assert(locations != 0, "Invalid number of locations!");

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

template <unsigned int tChannels, unsigned int tPatchSize>
bool TestAdvancedSumSquareDifferences::testPatchMirroredBorderBuffer8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize % 2u == 1u, "Invalid size");

	ocean_assert(width >= tPatchSize && height >= tPatchSize);
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int tBufferSize = tChannels * tPatchSize * tPatchSize;

	Log::info() << "... with " << tChannels << " channels and " << tPatchSize * tPatchSize << " pixels (" << tPatchSize << "x" << tPatchSize << "):";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performanceTemplate;
	HighPerformanceStatistic performanceSSE;
	HighPerformanceStatistic performanceNEON;
	HighPerformanceStatistic performanceDefault;

	constexpr size_t locations = 10000;

	Scalars centersX0(locations);
	Scalars centersY0(locations);
	Indices32 offsets1(locations);

	Indices32 resultsNaive;
	Indices32 resultsTemplate;
	Indices32 resultsSSE;
	Indices32 resultsNEON;
	Indices32 resultsDefault;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width0 = RandomI::random(randomGenerator, width - 1u, width + 1u);
		const unsigned int height0 = RandomI::random(randomGenerator, height - 1u, height + 1u);

		const unsigned int width1 = RandomI::random(randomGenerator, width - 1u, width + 1u);
		const unsigned int height1 = RandomI::random(randomGenerator, height - 1u, height + 1u);

		const unsigned int paddingElements0 = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int paddingElements1 = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		Frame frame0(FrameType(width0, height0, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), paddingElements0);
		Frame frame1(FrameType(width1, height1, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), paddingElements1);

		CV::CVUtilities::randomizeFrame(frame0, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(frame1, false, &randomGenerator);

		for (unsigned int n = 0u; n < locations; ++n)
		{
			centersX0[n] = Random::scalar(randomGenerator, Scalar(0), Scalar(width0) - Numeric::weakEps());
			centersY0[n] = Random::scalar(randomGenerator, Scalar(0), Scalar(height0) - Numeric::weakEps());

			offsets1[n] = RandomI::random(randomGenerator, height1 * frame1.strideElements() - tBufferSize - 1u);
		}

		const uint8_t* const data0 = frame0.constdata<uint8_t>();
		const uint8_t* const data1 = frame1.constdata<uint8_t>();

		for (const ImplementationType implementationType : {IT_NAIVE, IT_TEMPLATE, IT_SSE, IT_NEON, IT_DEFAULT})
		{
			switch (implementationType)
			{
				case IT_NAIVE:
				{
					/*resultsNaive.resize(locations); // not yet implemented

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceNaive);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsNaive[n] = CV::Advanced::AdvancedSumSquareDifferencesBase::patchMirroredBorderBuffer8BitPerChannel(data0, tChannels, tPatchSize, width0, centersX0[n], centersY0[n], paddingElements0, data1 + offsets1[n]);
					}*/

					break;
				}

				case IT_TEMPLATE:
				{
					/*resultsTemplate.resize(locations); // not yet implemented

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceTemplate);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsTemplate[n] = CV::Advanced::AdvancedSumSquareDifferencesBase::patchMirroredBorderBuffer8BitPerChannelTemplate<tChannels, tPatchSize>(data0, data1 + offsets1[n], width0, centersX0[n], centersY0[n]);
					}*/

					break;
				}

				case IT_SSE:
				{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
					if constexpr (tPatchSize >= 5u)
					{
						/*resultsSSE.resize(locations);

						const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceSSE);

						const unsigned int patch0StrideElements = frame0.strideElements();

						for (size_t n = 0; n < locations; ++n)
						{
							...
						}*/
					}
#endif // defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

					break;
				}

				case IT_NEON:
				{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
					/*if constexpr (tPatchSize >= 5u) not yet implemented
					{
						resultsNEON.resize(locations);

						const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceNEON);

						const unsigned int patch0StrideElements = frame0.strideElements();

						for (size_t n = 0; n < locations; ++n)
						{
							...
						}
					}*/
#endif // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

					break;
				}

				case IT_DEFAULT:
				{
					resultsDefault.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceDefault);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsDefault[n] = CV::Advanced::AdvancedSumSquareDifferences::patchMirroredBorderBuffer8BitPerChannel<tChannels, tPatchSize>(data0, width0, height0, centersX0[n], centersY0[n], paddingElements0, data1 + offsets1[n]);
					}

					break;
				}

				default:
					ocean_assert(false && "Invalid implementation type!");
					break;
			}
		}

		for (size_t n = 0; n < locations; ++n)
		{
			const uint32_t ssdTest = calculateMirroredBorder8BitPerChannel(frame0, Vector2(centersX0[n], centersY0[n]), data1 + offsets1[n], tPatchSize);

			if (!resultsNaive.empty() && resultsNaive[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsTemplate.empty() && resultsTemplate[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsSSE.empty() && resultsSSE[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsNEON.empty() && resultsNEON[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (resultsDefault[n] != ssdTest)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	static_assert(locations != 0, "Invalid number of locations!");

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

template <unsigned int tChannels, unsigned int tPatchSize>
bool TestAdvancedSumSquareDifferences::testTwoPixelPatchWithMask8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize % 2u == 1u, "Invalid size");

	ocean_assert(width >= tPatchSize && height >= tPatchSize);
	ocean_assert(testDuration > 0.0);

	constexpr size_t locations = 10000;

	Log::info() << "... with " << tChannels << " channels and " << tPatchSize * tPatchSize << " pixels (" << tPatchSize << "x" << tPatchSize << ") at " << locations << " locations:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performanceTemplate;
	HighPerformanceStatistic performanceSSE;
	HighPerformanceStatistic performanceNEON;
	HighPerformanceStatistic performanceDefault;

	Indices32 centersX0(locations);
	Indices32 centersY0(locations);
	Indices32 centersX1(locations);
	Indices32 centersY1(locations);

	IndexPairs32 resultsNaive;
	IndexPairs32 resultsTemplate;
	IndexPairs32 resultsSSE;
	IndexPairs32 resultsNEON;
	IndexPairs32 resultsDefault;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width0 = RandomI::random(randomGenerator, width - 1u, width + 1u);
		const unsigned int height0 = RandomI::random(randomGenerator, height - 1u, height + 1u);

		const unsigned int width1 = RandomI::random(randomGenerator, width - 1u, width + 1u);
		const unsigned int height1 = RandomI::random(randomGenerator, height - 1u, height + 1u);

		const Frame frame0 = CV::CVUtilities::randomizedFrame(FrameType(width0, height0, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
		const Frame frame1 = CV::CVUtilities::randomizedFrame(FrameType(width1, height1, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		const unsigned int framePaddingElements0 = frame0.paddingElements();
		const unsigned int framePaddingElements1 = frame1.paddingElements();

		const uint8_t maskValue = uint8_t(Random::random(randomGenerator, 255u));

		const Frame mask0 = CV::CVUtilities::randomizedBinaryMask(width0, height0, maskValue, &randomGenerator);
		const Frame mask1 = CV::CVUtilities::randomizedBinaryMask(width1, height1, maskValue, &randomGenerator);

		const unsigned int maskPaddingElements0 = mask0.paddingElements();
		const unsigned int maskPaddingElements1 = mask1.paddingElements();

		for (unsigned int n = 0u; n < locations; ++n)
		{
			centersX0[n] = RandomI::random(randomGenerator, width0 - 1u);
			centersY0[n] = RandomI::random(randomGenerator, height0 - 1u);

			centersX1[n] = RandomI::random(randomGenerator, width1 - 1u);
			centersY1[n] = RandomI::random(randomGenerator, height1 - 1u);
		}

		const uint8_t* const frameData0 = frame0.constdata<uint8_t>();
		const uint8_t* const frameData1 = frame1.constdata<uint8_t>();

		const uint8_t* const maskData0 = mask0.constdata<uint8_t>();
		const uint8_t* const maskData1 = mask1.constdata<uint8_t>();

		for (const ImplementationType implementationType : {IT_NAIVE, IT_TEMPLATE, IT_SSE, IT_NEON, IT_DEFAULT})
		{
			switch (implementationType)
			{
				case IT_NAIVE:
				{
					resultsNaive.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceNaive);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsNaive[n] = CV::Advanced::AdvancedSumSquareDifferencesBase::patchWithMask8BitPerChannel(frameData0, frameData1, maskData0, maskData1, tChannels, tPatchSize, width0, height0, width1, height1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], framePaddingElements0, framePaddingElements1, maskPaddingElements0, maskPaddingElements1, maskValue);
					}

					break;
				}

				case IT_TEMPLATE:
				{
					resultsTemplate.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceTemplate);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsTemplate[n] = CV::Advanced::AdvancedSumSquareDifferencesBase::patchWithMask8BitPerChannelTemplate<tChannels, tPatchSize>(frameData0, frameData1, maskData0, maskData1, width0, height0, width1, height1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], framePaddingElements0, framePaddingElements1, maskPaddingElements0, maskPaddingElements1, maskValue);
					}

					break;
				}

				case IT_SSE:
				{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
					// not yet implemented
#endif // defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

					break;
				}

				case IT_NEON:
				{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
					// not yet implemented
#endif // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

					break;
				}

				case IT_DEFAULT:
				{
					resultsDefault.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceDefault);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsDefault[n] = CV::Advanced::AdvancedSumSquareDifferences::patchWithMask8BitPerChannel<tChannels, tPatchSize>(frameData0, frameData1, maskData0, maskData1, width0, height0, width1, height1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], framePaddingElements0, framePaddingElements1, maskPaddingElements0, maskPaddingElements1, maskValue);
					}

					break;
				}

				default:
					ocean_assert(false && "Invalid implementation type!");
					break;
			}
		}

		for (size_t n = 0; n < locations; ++n)
		{
			const IndexPair32 ssdTest = calculateWithMask8BitPerChannel(frame0, frame1, mask0, mask1, CV::PixelPosition(centersX0[n], centersY0[n]), CV::PixelPosition(centersX1[n], centersY1[n]), tPatchSize, maskValue);

			if (!resultsNaive.empty() && resultsNaive[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsTemplate.empty() && resultsTemplate[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsSSE.empty() && resultsSSE[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsNEON.empty() && resultsNEON[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (resultsDefault[n] != ssdTest)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	static_assert(locations != 0, "Invalid number of locations!");

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

template <unsigned int tChannels, unsigned int tPatchSize>
bool TestAdvancedSumSquareDifferences::testTwoSubPixelPatchWithMask8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize >= 1u, "Invalid size");

	ocean_assert(width >= tPatchSize && height >= tPatchSize);
	ocean_assert(testDuration > 0.0);

	constexpr size_t locations = 10000;

	Log::info() << "... with " << tChannels << " channels and " << tPatchSize * tPatchSize << " pixels (" << tPatchSize << "x" << tPatchSize << ") at " << locations << " locations:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performanceTemplate;
	HighPerformanceStatistic performanceSSE;
	HighPerformanceStatistic performanceNEON;
	HighPerformanceStatistic performanceDefault;

	Scalars centersX0(locations);
	Scalars centersY0(locations);
	Scalars centersX1(locations);
	Scalars centersY1(locations);

	IndexPairs32 resultsNaive;
	IndexPairs32 resultsTemplate;
	IndexPairs32 resultsSSE;
	IndexPairs32 resultsNEON;
	IndexPairs32 resultsDefault;

	const Timestamp startTimestamp(true);

	constexpr CV::PixelCenter pixelCenter = CV::PC_CENTER; // not yet implemented

	do
	{
		const unsigned int width0 = RandomI::random(randomGenerator, width - 1u, width + 1u);
		const unsigned int height0 = RandomI::random(randomGenerator, height - 1u, height + 1u);

		const unsigned int width1 = RandomI::random(randomGenerator, width - 1u, width + 1u);
		const unsigned int height1 = RandomI::random(randomGenerator, height - 1u, height + 1u);

		const Frame frame0 = CV::CVUtilities::randomizedFrame(FrameType(width0, height0, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
		const Frame frame1 = CV::CVUtilities::randomizedFrame(FrameType(width1, height1, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		const unsigned int framePaddingElements0 = frame0.paddingElements();
		const unsigned int framePaddingElements1 = frame1.paddingElements();

		constexpr uint8_t validMaskValue = 0xFFu;
		constexpr uint8_t invalidMaskValue = 0xFFu - validMaskValue;

		const Frame mask0 = CV::CVUtilities::randomizedBinaryMask(width0, height0, invalidMaskValue, &randomGenerator);
		const Frame mask1 = CV::CVUtilities::randomizedBinaryMask(width1, height1, invalidMaskValue, &randomGenerator);

		const unsigned int maskPaddingElements0 = mask0.paddingElements();
		const unsigned int maskPaddingElements1 = mask1.paddingElements();

		for (unsigned int n = 0u; n < locations; ++n)
		{
			centersX0[n] = Random::scalar(randomGenerator, Scalar(-10), Scalar(width0 + 10u));
			centersY0[n] = Random::scalar(randomGenerator, Scalar(-10), Scalar(height0 + 10u));

			centersX1[n] = Random::scalar(randomGenerator, Scalar(-10), Scalar(width1 + 10u));
			centersY1[n] = Random::scalar(randomGenerator, Scalar(-10), Scalar(height1 + 10u));
		}

		const uint8_t* const frameData0 = frame0.constdata<uint8_t>();
		const uint8_t* const frameData1 = frame1.constdata<uint8_t>();

		const uint8_t* const maskData0 = mask0.constdata<uint8_t>();
		const uint8_t* const maskData1 = mask1.constdata<uint8_t>();

		for (const ImplementationType implementationType : {IT_NAIVE, IT_TEMPLATE, IT_SSE, IT_NEON, IT_DEFAULT})
		{
			switch (implementationType)
			{
				case IT_NAIVE:
				{
					// not yet implemented
					/*resultsNaive.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceNaive);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsNaive[n] = ...
					}*/

					break;
				}

				case IT_TEMPLATE:
				{
					resultsTemplate.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceTemplate);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsTemplate[n] = CV::Advanced::AdvancedSumSquareDifferencesBase::patchWithMask8BitPerChannelTemplate<tChannels, tPatchSize>(frameData0, frameData1, maskData0, maskData1, width0, height0, width1, height1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], framePaddingElements0, framePaddingElements1, maskPaddingElements0, maskPaddingElements1, invalidMaskValue);
					}

					break;
				}

				case IT_SSE:
				{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
					// not yet implemented
#endif // defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

					break;
				}

				case IT_NEON:
				{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
					// not yet implemented
#endif // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

					break;
				}

				case IT_DEFAULT:
				{
					resultsDefault.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceDefault);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsDefault[n] = CV::Advanced::AdvancedSumSquareDifferencesBase::patchWithMask8BitPerChannelTemplate<tChannels, tPatchSize>(frameData0, frameData1, maskData0, maskData1, width0, height0, width1, height1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], framePaddingElements0, framePaddingElements1, maskPaddingElements0, maskPaddingElements1, invalidMaskValue);
					}

					break;
				}

				default:
					ocean_assert(false && "Invalid implementation type!");
					break;
			}
		}

		for (size_t n = 0; n < locations; ++n)
		{
			const IndexPair32 ssdTest = calculateWithMask8BitPerChannel(frame0, frame1, mask0, mask1, Vector2(centersX0[n], centersY0[n]), Vector2(centersX1[n], centersY1[n]), pixelCenter, tPatchSize, validMaskValue);

			if (!resultsNaive.empty() && resultsNaive[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsTemplate.empty() && resultsTemplate[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsSSE.empty() && resultsSSE[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsNEON.empty() && resultsNEON[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (resultsDefault[n] != ssdTest)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	static_assert(locations != 0, "Invalid number of locations!");

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

template <unsigned int tChannels, unsigned int tPatchSize>
bool TestAdvancedSumSquareDifferences::testTwoPixelPatchWithRejectingMask8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize % 2u == 1u, "Invalid size");

	ocean_assert(width >= tPatchSize && height >= tPatchSize);
	ocean_assert(testDuration > 0.0);

	constexpr size_t locations = 10000;

	Log::info() << "... with " << tChannels << " channels and " << tPatchSize * tPatchSize << " pixels (" << tPatchSize << "x" << tPatchSize << ") at " << locations << " locations:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performanceTemplate;
	HighPerformanceStatistic performanceSSE;
	HighPerformanceStatistic performanceNEON;
	HighPerformanceStatistic performanceDefault;

	Indices32 centersX0(locations);
	Indices32 centersY0(locations);
	Indices32 centersX1(locations);
	Indices32 centersY1(locations);

	IndexPairs32 resultsNaive;
	IndexPairs32 resultsTemplate;
	IndexPairs32 resultsSSE;
	IndexPairs32 resultsNEON;
	IndexPairs32 resultsDefault;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width0 = RandomI::random(randomGenerator, width - 1u, width + 1u);
		const unsigned int height0 = RandomI::random(randomGenerator, height - 1u, height + 1u);

		const unsigned int width1 = RandomI::random(randomGenerator, width - 1u, width + 1u);
		const unsigned int height1 = RandomI::random(randomGenerator, height - 1u, height + 1u);

		const Frame frame0 = CV::CVUtilities::randomizedFrame(FrameType(width0, height0, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
		const Frame frame1 = CV::CVUtilities::randomizedFrame(FrameType(width1, height1, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		const unsigned int framePaddingElements0 = frame0.paddingElements();
		const unsigned int framePaddingElements1 = frame1.paddingElements();

		const uint8_t maskValue = uint8_t(Random::random(randomGenerator, 255u));

		const Frame mask0 = CV::CVUtilities::randomizedBinaryMask(width0, height0, maskValue, &randomGenerator);
		const Frame mask1 = CV::CVUtilities::randomizedBinaryMask(width1, height1, maskValue, &randomGenerator);

		const unsigned int maskPaddingElements0 = mask0.paddingElements();
		const unsigned int maskPaddingElements1 = mask1.paddingElements();

		for (unsigned int n = 0u; n < locations; ++n)
		{
			centersX0[n] = RandomI::random(randomGenerator, width0 - 1u);
			centersY0[n] = RandomI::random(randomGenerator, height0 - 1u);

			centersX1[n] = Random::random(randomGenerator, width1 - 1u);
			centersY1[n] = Random::random(randomGenerator, height1 - 1u);
		}

		const uint8_t* const frameData0 = frame0.constdata<uint8_t>();
		const uint8_t* const frameData1 = frame1.constdata<uint8_t>();

		const uint8_t* const maskData0 = mask0.constdata<uint8_t>();
		const uint8_t* const maskData1 = mask1.constdata<uint8_t>();

		for (const ImplementationType implementationType : {IT_NAIVE, IT_TEMPLATE, IT_SSE, IT_NEON, IT_DEFAULT})
		{
			switch (implementationType)
			{
				case IT_NAIVE:
				{
					resultsNaive.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceNaive);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsNaive[n] = CV::Advanced::AdvancedSumSquareDifferencesBase::patchWithRejectingMask8BitPerChannel(frameData0, frameData1, maskData0, maskData1, tChannels, tPatchSize, width0, height0, width1, height1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], framePaddingElements0, framePaddingElements1, maskPaddingElements0, maskPaddingElements1, maskValue);
					}

					break;
				}

				case IT_TEMPLATE:
				{
					// not yet implemented

					break;
				}

				case IT_SSE:
				{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
					// not yet implemented
#endif // defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

					break;
				}

				case IT_NEON:
				{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
					// not yet implemented
#endif // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

					break;
				}

				case IT_DEFAULT:
				{
					resultsDefault.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceDefault);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsDefault[n] = CV::Advanced::AdvancedSumSquareDifferences::patchWithRejectingMask8BitPerChannel<tChannels>(frameData0, frameData1, maskData0, maskData1, tPatchSize, width0, height0, width1, height1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], framePaddingElements0, framePaddingElements1, maskPaddingElements0, maskPaddingElements1, maskValue);
					}

					break;
				}

				default:
					ocean_assert(false && "Invalid implementation type!");
					break;
			}
		}

		for (size_t n = 0; n < locations; ++n)
		{
			const IndexPair32 ssdTest = calculateWithRejectingMask8BitPerChannel(frame0, frame1, mask0, mask1, CV::PixelPosition(centersX0[n], centersY0[n]), CV::PixelPosition(centersX1[n], centersY1[n]), tPatchSize, maskValue);

			if (!resultsNaive.empty() && resultsNaive[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsTemplate.empty() && resultsTemplate[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsSSE.empty() && resultsSSE[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsNEON.empty() && resultsNEON[n] != ssdTest)
			{
				allSucceeded = false;
			}

			if (resultsDefault[n] != ssdTest)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	static_assert(locations != 0, "Invalid number of locations!");

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

uint32_t TestAdvancedSumSquareDifferences::calculate8BitPerChannel(const Frame& frame0, const Frame& frame1, const Vector2& center0, const Vector2& center1, const unsigned int patchSize)
{
	ocean_assert(frame0.isValid() && frame1.isValid());

	if (!frame0.isValid() || !frame1.isValid() || !frame0.isPixelFormatCompatible(frame1.pixelFormat()))
	{
		ocean_assert(false && "This should never happen!");
		return uint32_t(-1);
	}

	const unsigned int channels = frame0.channels();

	ocean_assert(patchSize % 2u == 1u);

	if (patchSize == 0u || patchSize % 2u != 1u)
	{
		return uint32_t(-1);
	}

	const unsigned int patchSize_2 = patchSize / 2u;

	ocean_assert(center0.x() >= Scalar(patchSize_2) && center0.x() < Scalar(frame0.width() - patchSize_2 - 1u));
	ocean_assert(center0.y() >= Scalar(patchSize_2) && center0.y() < Scalar(frame0.height() - patchSize_2 - 1u));

	ocean_assert(center1.x() >= Scalar(patchSize_2) && center1.x() < Scalar(frame1.width() - patchSize_2 - 1u));
	ocean_assert(center1.y() >= Scalar(patchSize_2) && center1.y() < Scalar(frame1.height() - patchSize_2 - 1u));

	uint32_t ssdTest = 0u;

	const unsigned int firstTx = (unsigned int)((center0.x() - Scalar((unsigned int)center0.x())) * Scalar(128) + Scalar(0.5));
	const unsigned int firstTy = (unsigned int)((center0.y() - Scalar((unsigned int)center0.y())) * Scalar(128) + Scalar(0.5));

	const unsigned int secondTx = (unsigned int)((center1.x() - Scalar((unsigned int)center1.x())) * Scalar(128) + Scalar(0.5));
	const unsigned int secondTy = (unsigned int)((center1.y() - Scalar((unsigned int)center1.y())) * Scalar(128) + Scalar(0.5));

	const int patchLeft0 = int(center0.x() - Scalar(patchSize_2));
	const int patchTop0 = int(center0.y() - Scalar(patchSize_2));

	const int patchLeft1 = int(center1.x() - Scalar(patchSize_2));
	const int patchTop1 = int(center1.y() - Scalar(patchSize_2));

	for (int y = 0; y < int(patchSize); ++y)
	{
		for (int x = 0; x < int(patchSize); ++x)
		{
			std::vector<uint8_t> first(channels, 0u);
			std::vector<uint8_t> second(channels, 0u);

			{
				const unsigned int left = (unsigned int)(patchLeft0 + x);
				const unsigned int top = (unsigned int)(patchTop0 + y);
				const unsigned int right = (unsigned int)(left + 1);
				const unsigned int bottom = (unsigned int)(top + 1);

				ocean_assert(right < frame0.width());
				ocean_assert(bottom < frame0.height());

				const unsigned int tx = firstTx;
				const unsigned int ty = firstTy;
				const unsigned int tx_ = 128u - tx;
				const unsigned int ty_ = 128u - ty;

				ocean_assert(tx >= 0u && tx <= 128u);
				ocean_assert(ty >= 0u && ty <= 128u);

				for (unsigned int n = 0u; n < channels; ++n)
				{
					const unsigned int topLeft = frame0.constpixel<uint8_t>(left, top)[n];
					const unsigned int topRight = frame0.constpixel<uint8_t>(right, top)[n];

					const unsigned int bottomLeft = frame0.constpixel<uint8_t>(left, bottom)[n];
					const unsigned int bottomRight = frame0.constpixel<uint8_t>(right, bottom)[n];

					const unsigned int topPosition = topLeft * tx_ + topRight * tx;
					const unsigned int bottomPosition = bottomLeft * tx_ + bottomRight * tx;

					const unsigned int value = (topPosition * ty_ + bottomPosition * ty + 8192u) / 16384u;

					ocean_assert(value >= 0u && value <= 255u);
					first[n] = uint8_t(value);
				}
			}

			{
				const unsigned int left = (unsigned int)(patchLeft1 + x);
				const unsigned int top = (unsigned int)(patchTop1 + y);
				const unsigned int right = (unsigned int)(left + 1);
				const unsigned int bottom = (unsigned int)(top + 1);

				ocean_assert(right < frame1.width());
				ocean_assert(bottom < frame1.height());

				const unsigned int tx = secondTx;
				const unsigned int ty = secondTy;
				const unsigned int tx_ = 128u - tx;
				const unsigned int ty_ = 128u - ty;

				ocean_assert(tx >= 0u && tx <= 128u);
				ocean_assert(ty >= 0u && ty <= 128u);

				for (unsigned int n = 0u; n < channels; ++n)
				{
					const unsigned int topLeft = frame1.constpixel<uint8_t>(left, top)[n];
					const unsigned int topRight = frame1.constpixel<uint8_t>(right, top)[n];

					const unsigned int bottomLeft = frame1.constpixel<uint8_t>(left, bottom)[n];
					const unsigned int bottomRight = frame1.constpixel<uint8_t>(right, bottom)[n];

					const unsigned int topPosition = topLeft * tx_ + topRight * tx;
					const unsigned int bottomPosition = bottomLeft * tx_ + bottomRight * tx;

					const unsigned int value = (topPosition * ty_ + bottomPosition * ty + 8192u) / 16384u;

					ocean_assert(value >= 0u && value <= 255u);

					second[n] = uint8_t(value);
				}
			}

			for (unsigned int n = 0u; n < channels; ++n)
			{
				const int diff = int(first[n]) - int(second[n]);

				ssdTest += uint32_t(diff * diff);
			}
		}
	}

	return ssdTest;
}

uint32_t TestAdvancedSumSquareDifferences::calculate8BitPerChannel(const Frame& frame0, const Frame& frame1, const CV::PixelPosition& center0, const Vector2& center1, const unsigned int patchSize)
{
	ocean_assert(frame0.isValid() && frame1.isValid());

	if (!frame0.isValid() || !frame1.isValid() || !frame0.isPixelFormatCompatible(frame1.pixelFormat()))
	{
		ocean_assert(false && "This should never happen!");
		return uint32_t(-1);
	}

	const unsigned int channels = frame0.channels();

	ocean_assert(patchSize % 2u == 1u);

	if (patchSize == 0u || patchSize % 2u != 1u)
	{
		return uint32_t(-1);
	}

	const unsigned int patchSize_2 = patchSize / 2u;

	ocean_assert(center0.x() >= patchSize_2 && center0.x() < frame0.width() - patchSize_2);
	ocean_assert(center0.y() >= patchSize_2 && center0.y() < frame0.height() - patchSize_2);

	ocean_assert(center1.x() >= Scalar(patchSize_2) && center1.x() < Scalar(frame1.width() - patchSize_2 - 1u));
	ocean_assert(center1.y() >= Scalar(patchSize_2) && center1.y() < Scalar(frame1.height() - patchSize_2 - 1u));

	uint32_t ssdTest = 0u;

	const unsigned int secondTx = (unsigned int)((center1.x() - Scalar((unsigned int)center1.x())) * Scalar(128) + Scalar(0.5));
	const unsigned int secondTy = (unsigned int)((center1.y() - Scalar((unsigned int)center1.y())) * Scalar(128) + Scalar(0.5));

	const int patchLeft0 = int(center0.x() - patchSize_2);
	const int patchTop0 = int(center0.y() - patchSize_2);

	const int patchLeft1 = int(center1.x() - Scalar(patchSize_2));
	const int patchTop1 = int(center1.y() - Scalar(patchSize_2));

	for (int y = 0; y < int(patchSize); ++y)
	{
		for (int x = 0; x < int(patchSize); ++x)
		{
			std::vector<uint8_t> first(channels, 0u);
			std::vector<uint8_t> second(channels, 0u);

			for (unsigned int n = 0u; n < channels; ++n)
			{
				first[n] = frame0.constpixel<uint8_t>((unsigned int)(patchLeft0 + x), (unsigned int)(patchTop0 + y))[n];
			}

			{
				const unsigned int left = (unsigned int)(patchLeft1 + x);
				const unsigned int top = (unsigned int)(patchTop1 + y);
				const unsigned int right = (unsigned int)(left + 1);
				const unsigned int bottom = (unsigned int)(top + 1);

				ocean_assert(right < frame1.width());
				ocean_assert(bottom < frame1.height());

				const unsigned int tx = secondTx;
				const unsigned int ty = secondTy;
				const unsigned int tx_ = 128u - tx;
				const unsigned int ty_ = 128u - ty;

				ocean_assert(tx >= 0u && tx <= 128u);
				ocean_assert(ty >= 0u && ty <= 128u);

				for (unsigned int n = 0u; n < channels; ++n)
				{
					const unsigned int topLeft = frame1.constpixel<uint8_t>(left, top)[n];
					const unsigned int topRight = frame1.constpixel<uint8_t>(right, top)[n];

					const unsigned int bottomLeft = frame1.constpixel<uint8_t>(left, bottom)[n];
					const unsigned int bottomRight = frame1.constpixel<uint8_t>(right, bottom)[n];

					const unsigned int topPosition = topLeft * tx_ + topRight * tx;
					const unsigned int bottomPosition = bottomLeft * tx_ + bottomRight * tx;

					const unsigned int value = (topPosition * ty_ + bottomPosition * ty + 8192u) / 16384u;

					ocean_assert(value >= 0u && value <= 255u);

					second[n] = (unsigned char)(value);
				}
			}

			for (unsigned int n = 0u; n < channels; ++n)
			{
				const int diff = int(first[n]) - int(second[n]);

				ssdTest += uint32_t(diff * diff);
			}
		}
	}

	return ssdTest;
}

uint32_t TestAdvancedSumSquareDifferences::calculate8BitPerChannel(const Frame& frame0, const Vector2& center0, const uint8_t* buffer1, const unsigned int patchSize)
{
	ocean_assert(frame0.isValid());
	ocean_assert(buffer1 != nullptr);

	if (!frame0.isValid() || buffer1 == nullptr)
	{
		ocean_assert(false && "This should never happen!");
		return uint32_t(-1);
	}

	const unsigned int channels = frame0.channels();

	ocean_assert(patchSize % 2u == 1u);

	if (patchSize == 0u || patchSize % 2u != 1u)
	{
		return uint32_t(-1);
	}

	const unsigned int patchSize_2 = patchSize / 2u;

	ocean_assert(center0.x() >= Scalar(patchSize_2) && center0.x() < Scalar(frame0.width() - patchSize_2 - 1u));
	ocean_assert(center0.y() >= Scalar(patchSize_2) && center0.y() < Scalar(frame0.height() - patchSize_2 - 1u));

	uint32_t ssdTest = 0u;

	const unsigned int firstTx = (unsigned int)((center0.x() - Scalar((unsigned int)center0.x())) * Scalar(128) + Scalar(0.5));
	const unsigned int firstTy = (unsigned int)((center0.y() - Scalar((unsigned int)center0.y())) * Scalar(128) + Scalar(0.5));

	const int patchLeft0 = int(center0.x() - Scalar(patchSize_2));
	const int patchTop0 = int(center0.y() - Scalar(patchSize_2));

	for (int y = 0; y < int(patchSize); ++y)
	{
		for (int x = 0; x < int(patchSize); ++x)
		{
			std::vector<uint8_t> pixel0(channels, 0u);

			{
				const unsigned int left = (unsigned int)(patchLeft0 + x);
				const unsigned int top = (unsigned int)(patchTop0 + y);
				const unsigned int right = (unsigned int)(left + 1);
				const unsigned int bottom = (unsigned int)(top + 1);

				ocean_assert(right < frame0.width());
				ocean_assert(bottom < frame0.height());

				const unsigned int tx = firstTx;
				const unsigned int ty = firstTy;
				const unsigned int tx_ = 128u - tx;
				const unsigned int ty_ = 128u - ty;

				ocean_assert(tx >= 0u && tx <= 128u);
				ocean_assert(ty >= 0u && ty <= 128u);

				for (unsigned int n = 0u; n < channels; ++n)
				{
					const unsigned int topLeft = frame0.constpixel<uint8_t>(left, top)[n];
					const unsigned int topRight = frame0.constpixel<uint8_t>(right, top)[n];

					const unsigned int bottomLeft = frame0.constpixel<uint8_t>(left, bottom)[n];
					const unsigned int bottomRight = frame0.constpixel<uint8_t>(right, bottom)[n];

					const unsigned int topPosition = topLeft * tx_ + topRight * tx;
					const unsigned int bottomPosition = bottomLeft * tx_ + bottomRight * tx;

					const unsigned int value = (topPosition * ty_ + bottomPosition * ty + 8192u) / 16384u;

					ocean_assert(value >= 0u && value <= 255u);
					pixel0[n] = uint8_t(value);
				}
			}

			for (unsigned int n = 0u; n < channels; ++n)
			{
				const int diff = int(pixel0[n]) - int(buffer1[n]);

				ssdTest += uint32_t(diff * diff);
			}

			buffer1 += channels;
		}
	}

	return ssdTest;
}

uint32_t TestAdvancedSumSquareDifferences::calculateMirroredBorder8BitPerChannel(const Frame& frame0, const Vector2& center0, const uint8_t* buffer1, const unsigned int patchSize)
{
	ocean_assert(frame0.isValid());
	ocean_assert(buffer1 != nullptr);

	if (!frame0.isValid() || buffer1 == nullptr)
	{
		ocean_assert(false && "This should never happen!");
		return uint32_t(-1);
	}

	const unsigned int channels = frame0.channels();

	ocean_assert(patchSize % 2u == 1u);

	if (patchSize == 0u || patchSize % 2u != 1u)
	{
		return uint32_t(-1);
	}

	const unsigned int patchSize_2 = patchSize / 2u;

	ocean_assert(center0.x() >= Scalar(0) && center0.x() < Scalar(frame0.width()));
	ocean_assert(center0.y() >= Scalar(0) && center0.y() < Scalar(frame0.height()));

	uint32_t ssdTest = 0u;

	const unsigned int firstTx = (unsigned int)((center0.x() - Scalar((unsigned int)center0.x())) * Scalar(128) + Scalar(0.5));
	const unsigned int firstTy = (unsigned int)((center0.y() - Scalar((unsigned int)center0.y())) * Scalar(128) + Scalar(0.5));

	const Scalar patchLeft0 = center0.x() - Scalar(patchSize_2);
	const Scalar patchTop0 = center0.y() - Scalar(patchSize_2);

	int intPatchLeft0 = int(patchLeft0);
	int intPatchTop0 = int(patchTop0);

	if (patchLeft0 < Scalar(0) && patchLeft0 != Scalar(int(patchLeft0)))
	{
		--intPatchLeft0;
	}

	if (patchTop0 < 0 && patchTop0 != Scalar(int(patchTop0)))
	{
		--intPatchTop0;
	}

	for (int y = 0; y < int(patchSize); ++y)
	{
		for (int x = 0; x < int(patchSize); ++x)
		{
			std::vector<uint8_t> pixel0(channels, 0u);

			{
				// **TODO** verify that the mirroring logic is the most optimal solution wrt performance and quality

				int left = intPatchLeft0 + x;
				int top = intPatchTop0 + y;
				int right = left + 1;
				int bottom = top + 1;

				if (left < 0)
				{
					left = -left - 1;
				}
				else if (left >= int(frame0.width()))
				{
					left = int(frame0.width()) - (left - int(frame0.width())) - 1;
				}

				if (top < 0)
				{
					top = -top - 1;
				}
				else if (top >= int(frame0.height()))
				{
					top = int(frame0.height()) - (top - int(frame0.height())) - 1;
				}

				if (right < 0)
				{
					right = -right - 1;
				}
				else if (right >= int(frame0.width()))
				{
					right = int(frame0.width()) - (right - int(frame0.width())) - 1;
				}

				if (bottom < 0)
				{
					bottom = -bottom - 1;
				}
				else if (bottom >= int(frame0.height()))
				{
					bottom = int(frame0.height()) - (bottom - int(frame0.height())) - 1;
				}

				ocean_assert(left >= 0 && right < int(frame0.width()));
				ocean_assert(top >= 0 && bottom < int(frame0.height()));

				const unsigned int tx = firstTx;
				const unsigned int ty = firstTy;
				const unsigned int tx_ = 128u - tx;
				const unsigned int ty_ = 128u - ty;

				ocean_assert(tx >= 0u && tx <= 128u);
				ocean_assert(ty >= 0u && ty <= 128u);

				for (unsigned int n = 0u; n < channels; ++n)
				{
					const unsigned int topLeft = frame0.constpixel<uint8_t>(left, top)[n];
					const unsigned int topRight = frame0.constpixel<uint8_t>(right, top)[n];

					const unsigned int bottomLeft = frame0.constpixel<uint8_t>(left, bottom)[n];
					const unsigned int bottomRight = frame0.constpixel<uint8_t>(right, bottom)[n];

					const unsigned int topPosition = topLeft * tx_ + topRight * tx;
					const unsigned int bottomPosition = bottomLeft * tx_ + bottomRight * tx;

					const unsigned int value = (topPosition * ty_ + bottomPosition * ty + 8192u) / 16384u;

					ocean_assert(value >= 0u && value <= 255u);
					pixel0[n] = uint8_t(value);
				}
			}

			for (unsigned int n = 0u; n < channels; ++n)
			{
				const int diff = int(pixel0[n]) - int(buffer1[n]);

				ssdTest += uint32_t(diff * diff);
			}

			buffer1 += channels;
		}
	}

	return ssdTest;
}

IndexPair32 TestAdvancedSumSquareDifferences::calculateWithMask8BitPerChannel(const Frame& frame0, const Frame& frame1, const Frame& mask0, const Frame& mask1, const CV::PixelPosition& center0, const CV::PixelPosition& center1, const unsigned int patchSize, const uint8_t maskValue)
{
	ocean_assert(frame0.isValid() && frame1.isValid());
	ocean_assert(mask0.isValid() && mask1.isValid());

	if (!frame0.isValid() || !frame1.isValid() || !frame0.isPixelFormatCompatible(frame1.pixelFormat()))
	{
		ocean_assert(false && "This should never happen!");
		return IndexPair32(uint32_t(-1), uint32_t(-1));
	}

	if (!frame0.isFrameTypeCompatible(FrameType(mask0, frame0.pixelFormat()), false) || !frame1.isFrameTypeCompatible(FrameType(mask1, frame1.pixelFormat()), false))
	{
		ocean_assert(false && "This should never happen!");
		return IndexPair32(uint32_t(-1), uint32_t(-1));
	}

	const unsigned int channels = frame0.channels();

	ocean_assert(patchSize % 2u == 1u);

	if (patchSize == 0u || patchSize % 2u != 1u)
	{
		return IndexPair32(uint32_t(-1), uint32_t(-1));
	}

	const unsigned int patchSize_2 = patchSize / 2u;

	uint32_t ssd = 0u;
	uint32_t validPixels = 0u;

	for (int yy = -int(patchSize_2); yy <= int(patchSize_2); ++yy)
	{
		const int y0 = int(center0.y()) + yy;
		const int y1 = int(center1.y()) + yy;

		if (y0 < 0 || y0 >= int(frame0.height()) || y1 < 0 || y1 >= int(frame1.height()))
		{
			continue;
		}

		for (int xx = -int(patchSize_2); xx <= int(patchSize_2); ++xx)
		{
			const int x0 = int(center0.x()) + xx;
			const int x1 = int(center1.x()) + xx;

			if (x0 < 0 || x0 >= int(frame0.width()) || x1 < 0 || x1 >= int(frame1.width()))
			{
				continue;
			}

			if (mask0.constpixel<uint8_t>(x0, y0)[0] != maskValue && mask1.constpixel<uint8_t>(x1, y1)[0] != maskValue)
			{
				for (unsigned int n = 0u; n < channels; ++n)
				{
					const uint8_t value0 = frame0.constpixel<uint8_t>((unsigned int)(x0), (unsigned int)(y0))[n];
					const uint8_t value1 = frame1.constpixel<uint8_t>((unsigned int)(x1), (unsigned int)(y1))[n];

					const int32_t difference = int32_t(value0) - int32_t(value1);

					ssd += uint32_t(difference * difference);
				}

				++validPixels;
			}
		}
	}

	return IndexPair32(ssd, validPixels);
}

IndexPair32 TestAdvancedSumSquareDifferences::calculateWithMask8BitPerChannel(const Frame& frame0, const Frame& frame1, const Frame& mask0, const Frame& mask1, const Vector2& center0, const Vector2& center1, const CV::PixelCenter pixelCenter, const unsigned int patchSize, const uint8_t validMaskValue)
{
	ocean_assert(frame0.isValid() && frame1.isValid());
	ocean_assert(mask0.isValid() && mask1.isValid());

	if (!frame0.isValid() || !frame1.isValid() || !frame0.isPixelFormatCompatible(frame1.pixelFormat()))
	{
		ocean_assert(false && "This should never happen!");
		return IndexPair32(uint32_t(-1), uint32_t(-1));
	}

	if (!frame0.isFrameTypeCompatible(FrameType(mask0, frame0.pixelFormat()), false) || !frame1.isFrameTypeCompatible(FrameType(mask1, frame1.pixelFormat()), false))
	{
		ocean_assert(false && "This should never happen!");
		return IndexPair32(uint32_t(-1), uint32_t(-1));
	}

	const unsigned int channels = frame0.channels();

	ocean_assert(patchSize >= 1u);

	if (patchSize == 0u)
	{
		return IndexPair32(uint32_t(-1), uint32_t(-1));
	}

	const Scalar topLeftOffset = Scalar(patchSize - 1u) / Scalar(2);

	const Vector2 topLeft0 = center0 - Vector2(topLeftOffset, topLeftOffset);
	const Vector2 topLeft1 = center1 - Vector2(topLeftOffset, topLeftOffset);

	const Scalar leftSize = center0.x() - topLeft0.x();
	const Scalar rightSize = topLeft0.x() + Scalar(patchSize - 1u) - center0.x();
	ocean_assert(Numeric::isWeakEqual(leftSize, rightSize));

	const Scalar topSize = center0.y() - topLeft0.y();
	const Scalar bottomSize = topLeft0.y() + Scalar(patchSize - 1u) - center0.y();
	ocean_assert(Numeric::isWeakEqual(topSize, bottomSize));

	if constexpr (std::is_same<double, Scalar>::value)
	{
		if (Numeric::isNotEqual(leftSize, rightSize) || Numeric::isNotEqual(topSize, bottomSize))
		{
			ocean_assert(false && "This should never happen!");

			return IndexPair32(uint32_t(-1), uint32_t(-1));
		}
	}

	uint32_t ssd = 0u;
	uint32_t validPixels = 0u;

	std::vector<uint8_t> resultPixel0(channels);
	std::vector<uint8_t> resultPixel1(channels);

	uint8_t resultMask0;
	uint8_t resultMask1;

	for (unsigned int yy = 0u; yy < patchSize; ++yy)
	{
		for (unsigned int xx = 0u; xx < patchSize; ++xx)
		{
			const Vector2 position0 = topLeft0 + Vector2(Scalar(xx), Scalar(yy));
			const Vector2 position1 = topLeft1 + Vector2(Scalar(xx), Scalar(yy));

			if (!CV::Advanced::AdvancedFrameInterpolatorBilinear::Comfort::interpolatePixelWithMask8BitPerChannel(frame0.constdata<uint8_t>(), mask0.constdata<uint8_t>(), frame0.channels(), frame0.width(), frame0.height(), frame0.paddingElements(), mask0.paddingElements(), pixelCenter, position0, resultPixel0.data(), resultMask0, validMaskValue))
			{
				ocean_assert(false && "This should never happen!");
				return IndexPair32(uint32_t(-1), uint32_t(-1));
			}

			if (!CV::Advanced::AdvancedFrameInterpolatorBilinear::Comfort::interpolatePixelWithMask8BitPerChannel(frame1.constdata<uint8_t>(), mask1.constdata<uint8_t>(), frame1.channels(), frame1.width(), frame1.height(), frame1.paddingElements(), mask1.paddingElements(), pixelCenter, position1, resultPixel1.data(), resultMask1, validMaskValue))
			{
				ocean_assert(false && "This should never happen!");
				return IndexPair32(uint32_t(-1), uint32_t(-1));
			}

			if (resultMask0 == validMaskValue && resultMask1 == validMaskValue)
			{
				for (unsigned int n = 0u; n < channels; ++n)
				{
					const int32_t difference = int32_t(resultPixel0[n]) - int32_t(resultPixel1[n]);

					ssd += uint32_t(difference * difference);
				}

				++validPixels;
			}
		}
	}

	return IndexPair32(ssd, validPixels);
}

IndexPair32 TestAdvancedSumSquareDifferences::calculateWithRejectingMask8BitPerChannel(const Frame& frame0, const Frame& frame1, const Frame& mask0, const Frame& mask1, const CV::PixelPosition& center0, const CV::PixelPosition& center1, const unsigned int patchSize, const uint8_t maskValue)
{
	ocean_assert(frame0.isValid() && frame1.isValid());
	ocean_assert(mask0.isValid() && mask1.isValid());

	if (!frame0.isValid() || !frame1.isValid() || !frame0.isPixelFormatCompatible(frame1.pixelFormat()))
	{
		ocean_assert(false && "This should never happen!");
		return IndexPair32(uint32_t(-1), uint32_t(-1));
	}

	if (!frame0.isFrameTypeCompatible(FrameType(mask0, frame0.pixelFormat()), false) || !frame1.isFrameTypeCompatible(FrameType(mask1, frame1.pixelFormat()), false))
	{
		ocean_assert(false && "This should never happen!");
		return IndexPair32(uint32_t(-1), uint32_t(-1));
	}

	const unsigned int channels = frame0.channels();

	ocean_assert(patchSize % 2u == 1u);

	if (patchSize == 0u || patchSize % 2u != 1u)
	{
		return IndexPair32(uint32_t(-1), uint32_t(-1));
	}

	const unsigned int patchSize_2 = patchSize / 2u;

	uint32_t ssd = 0u;
	uint32_t validPixels = 0u;

	for (int yy = -int(patchSize_2); yy <= int(patchSize_2); ++yy)
	{
		const int y0 = int(center0.y()) + yy;

		if (y0 < 0 || y0 >= int(frame0.height()))
		{
			continue;
		}

		for (int xx = -int(patchSize_2); xx <= int(patchSize_2); ++xx)
		{
			const int x0 = int(center0.x()) + xx;

			if (x0 < 0 || x0 >= int(frame0.width()))
			{
				continue;
			}

			if (mask0.constpixel<uint8_t>(x0, y0)[0] != maskValue)
			{
				const int y1 = int(center1.y()) + yy;
				const int x1 = int(center1.x()) + xx;

				if (y1 < 0 || y1 >= int(frame1.height()))
				{
					return IndexPair32(uint32_t(-1), 0u);
				}

				if (x1 < 0 || x1 >= int(frame1.width()))
				{
					return IndexPair32(uint32_t(-1), 0u);
				}

				if (mask1.constpixel<uint8_t>(x1, y1)[0] == maskValue)
				{
					return IndexPair32(uint32_t(-1), 0u);
				}

				for (unsigned int n = 0u; n < channels; ++n)
				{
					const uint8_t value0 = frame0.constpixel<uint8_t>((unsigned int)(x0), (unsigned int)(y0))[n];
					const uint8_t value1 = frame1.constpixel<uint8_t>((unsigned int)(x1), (unsigned int)(y1))[n];

					const int32_t difference = int32_t(value0) - int32_t(value1);

					ssd += uint32_t(difference * difference);
				}

				++validPixels;
			}
		}
	}

	return IndexPair32(ssd, validPixels);
}

}

}

}

}
