/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestSumSquareDifferences.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/CVUtilities.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestSumSquareDifferences::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Test sum of square differences:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testPatch8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Sum of square differences test succeeded.";
	}
	else
	{
		Log::info() << "Sum of square differences test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestSumSquareDifferences, Patch8BitPerChannel)
{
	EXPECT_TRUE(TestSumSquareDifferences::testPatch8BitPerChannel(GTEST_TEST_DURATION));
}

TEST(TestSumSquareDifferences, Buffer8BitPerChannel)
{
	EXPECT_TRUE(TestSumSquareDifferences::testBuffer8BitPerChannel(GTEST_TEST_DURATION));
}

TEST(TestSumSquareDifferences, PatchBuffer8BitPerChannel)
{
	EXPECT_TRUE(TestSumSquareDifferences::testPatchBuffer8BitPerChannel(GTEST_TEST_DURATION));
}

TEST(TestSumSquareDifferences, PatchAtBorder8BitPerChannel)
{
	EXPECT_TRUE(TestSumSquareDifferences::testPatchAtBorder8BitPerChannel(GTEST_TEST_DURATION));
}

TEST(TestSumSquareDifferences, PatchMirroredBorder8BitPerChannel)
{
	EXPECT_TRUE(TestSumSquareDifferences::testPatchMirroredBorder8BitPerChannel(GTEST_TEST_DURATION));
}

#endif

bool TestSumSquareDifferences::testPatch8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SSD between two patches:";
	Log::info() << " ";

	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;

	bool allSucceeded = true;

	allSucceeded = testPatch8BitPerChannel<1u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<2u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<3u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<4u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<1u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<2u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<3u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<4u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<1u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<2u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<3u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<4u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<1u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<2u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<3u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<4u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<1u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<2u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<3u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<4u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<1u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<2u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<3u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<4u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<1u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<2u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<3u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatch8BitPerChannel<4u, 31u>(width, height, testDuration) && allSucceeded;

	return allSucceeded;
}

bool TestSumSquareDifferences::testBuffer8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SSD between two buffers:";
	Log::info() << " ";

	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;

	bool allSucceeded = true;

	allSucceeded = testBuffer8BitPerChannel<1u, 1u * 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<2u, 1u * 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<3u, 1u * 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<4u, 1u * 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<1u, 3u * 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<2u, 3u * 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<3u, 3u * 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<4u, 3u * 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<1u, 5u * 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<2u, 5u * 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<3u, 5u * 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<4u, 5u * 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<1u, 7u * 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<2u, 7u * 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<3u, 7u * 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<4u, 7u * 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<1u, 9u * 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<2u, 9u * 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<3u, 9u * 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<4u, 9u * 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<1u, 15u * 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<2u, 15u * 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<3u, 15u * 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<4u, 15u * 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<1u, 31u * 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<2u, 31u * 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<3u, 31u * 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<4u, 31u * 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<1u, 63u * 63u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<2u, 63u * 63u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<3u, 63u * 63u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testBuffer8BitPerChannel<4u, 63u * 63u>(width, height, testDuration) && allSucceeded;

	return allSucceeded;
}

bool TestSumSquareDifferences::testPatchBuffer8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SSD between a patch and a buffer:";
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

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<1u, 63u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<2u, 63u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<3u, 63u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchBuffer8BitPerChannel<4u, 63u>(width, height, testDuration) && allSucceeded;

	return allSucceeded;
}

bool TestSumSquareDifferences::testPatchAtBorder8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing two pixel accurate patches which can be partially outside of the image:";
	Log::info() << " ";

	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;

	bool allSucceeded = true;

	allSucceeded = testPatchAtBorder8BitPerChannel<1u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<2u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<3u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<4u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<1u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<2u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<3u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<4u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<1u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<2u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<3u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<4u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<1u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<2u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<3u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<4u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<1u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<2u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<3u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<4u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<1u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<2u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<3u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<4u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<1u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<2u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<3u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchAtBorder8BitPerChannel<4u, 31u>(width, height, testDuration) && allSucceeded;

	return allSucceeded;
}

bool TestSumSquareDifferences::testPatchMirroredBorder8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SSD between two patches with mirrored border:";
	Log::info() << " ";

	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;

	bool allSucceeded = true;

	allSucceeded = testPatchMirroredBorder8BitPerChannel<1u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<2u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<3u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<4u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<1u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<2u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<3u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<4u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<1u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<2u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<3u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<4u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<1u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<2u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<3u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<4u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<1u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<2u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<3u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<4u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<1u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<2u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<3u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<4u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<1u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<2u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<3u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchMirroredBorder8BitPerChannel<4u, 31u>(width, height, testDuration) && allSucceeded;

	return allSucceeded;
}

template <unsigned int tChannels, unsigned int tPatchSize>
bool TestSumSquareDifferences::testPatch8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize % 2u == 1u, "Invalid size");

	ocean_assert(width >= tPatchSize && height >= tPatchSize);
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	Log::info() << "... with " << tChannels << " channels and " << tPatchSize * tPatchSize << " pixels (" << tPatchSize << "x" << tPatchSize << "):";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performanceTemplate;
	HighPerformanceStatistic performanceSSE;
	HighPerformanceStatistic performanceNEON;
	HighPerformanceStatistic performanceDefault;

	constexpr size_t locations = 10000;

	Indices32 centersX0(locations);
	Indices32 centersY0(locations);
	Indices32 centersX1(locations);
	Indices32 centersY1(locations);

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

		// Add valid locations nearest to buffer boundaries to test for memory access violation bugs
		centersX0[0] = tPatchSize_2;
		centersY0[0] = tPatchSize_2;
		centersX1[0] = tPatchSize_2;
		centersY1[0] = tPatchSize_2;

		centersX0[1] = width0 - tPatchSize_2 - 1u;
		centersY0[1] = height0 - tPatchSize_2 - 1u;
		centersX1[1] = width1 - tPatchSize_2 - 1u;
		centersY1[1] = height1 - tPatchSize_2 - 1u;

		for (unsigned int n = 2u; n < locations; ++n)
		{
			centersX0[n] = RandomI::random(randomGenerator, tPatchSize_2, width0 - tPatchSize_2 - 1u);
			centersY0[n] = RandomI::random(randomGenerator, tPatchSize_2, height0 - tPatchSize_2 - 1u);

			centersX1[n] = RandomI::random(randomGenerator, tPatchSize_2, width1 - tPatchSize_2 - 1u);
			centersY1[n] = RandomI::random(randomGenerator, tPatchSize_2, height1 - tPatchSize_2 - 1u);
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
						resultsNaive[n] = CV::SumSquareDifferencesBase::patch8BitPerChannel(data0, data1, tChannels, tPatchSize, width0, width1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
					}

					break;
				}

				case IT_TEMPLATE:
				{
					resultsTemplate.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceTemplate);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsTemplate[n] = CV::SumSquareDifferencesBase::patch8BitPerChannelTemplate<tChannels, tPatchSize>(data0, data1, width0, width1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
					}

					break;
				}

				case IT_SSE:
				{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
					if constexpr (tPatchSize >= 5u)
					{
						resultsSSE.resize(locations);

						const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceSSE);

						const unsigned int patch0StrideElements = frame0.strideElements();
						const unsigned int patch1StrideElements = frame1.strideElements();

						for (size_t n = 0; n < locations; ++n)
						{
							const uint8_t* const patch0 = data0 + (centersY0[n] - tPatchSize_2) * patch0StrideElements + (centersX0[n] - tPatchSize_2) * tChannels;
							const uint8_t* const patch1 = data1 + (centersY1[n] - tPatchSize_2) * patch1StrideElements + (centersX1[n] - tPatchSize_2) * tChannels;

							resultsSSE[n] = CV::SumSquareDifferencesSSE::patch8BitPerChannel<tChannels, tPatchSize>(patch0, patch1, patch0StrideElements, patch1StrideElements);
						}
					}
#endif // defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

					break;
				}

				case IT_NEON:
				{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
					if constexpr (tPatchSize >= 5u)
					{
						resultsNEON.resize(locations);

						const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceNEON);

						const unsigned int patch0StrideElements = frame0.strideElements();
						const unsigned int patch1StrideElements = frame1.strideElements();

						for (size_t n = 0; n < locations; ++n)
						{
							const uint8_t* const patch0 = data0 + (centersY0[n] - tPatchSize_2) * patch0StrideElements + (centersX0[n] - tPatchSize_2) * tChannels;
							const uint8_t* const patch1 = data1 + (centersY1[n] - tPatchSize_2) * patch1StrideElements + (centersX1[n] - tPatchSize_2) * tChannels;

							resultsNEON[n] = CV::SumSquareDifferencesNEON::patch8BitPerChannel<tChannels, tPatchSize>(patch0, patch1, patch0StrideElements, patch1StrideElements);
						}
					}
#endif // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

					break;
				}

				case IT_DEFAULT:
				{
					resultsDefault.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceDefault);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsDefault[n] = CV::SumSquareDifferences::patch8BitPerChannel<tChannels, tPatchSize>(data0, data1, width0, width1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
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
			uint32_t ssdTest = 0u;

			const uint8_t* pixel0 = frame0.constpixel<uint8_t>(centersX0[n] - tPatchSize_2, centersY0[n] - tPatchSize_2);
			const uint8_t* pixel1 = frame1.constpixel<uint8_t>(centersX1[n] - tPatchSize_2, centersY1[n] - tPatchSize_2);

			for (unsigned int y = 0u; y < tPatchSize; ++y)
			{
				for (unsigned int i = 0u; i < tChannels * tPatchSize; ++i)
				{
					const int32_t value = int32_t(pixel0[i]) - int32_t(pixel1[i]);
					ssdTest += uint32_t(value * value);
				}

				pixel0 += frame0.strideElements();
				pixel1 += frame1.strideElements();
			}

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

template <unsigned int tChannels, unsigned int tPixels>
bool TestSumSquareDifferences::testBuffer8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPixels >= 1u, "Invalid size");

	ocean_assert(width * height >= tPixels);
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int tBufferSize = tChannels * tPixels;

	Log::info() << "... with " << tChannels << " channels and " << tPixels << " pixels:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performanceTemplate;
	HighPerformanceStatistic performanceSSE;
	HighPerformanceStatistic performanceNEON;
	HighPerformanceStatistic performanceDefault;

	constexpr size_t locations = 10000;

	Indices32 offsets0(locations);
	Indices32 offsets1(locations);

	Indices32 resultsNaive;
	Indices32 resultsTemplate;
	Indices32 resultsSSE;
	Indices32 resultsNEON;
	Indices32 resultsDefault;

	const Timestamp startTimestamp(true);

	do
	{
		Frame frame0(FrameType(width, height, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT));
		Frame frame1(FrameType(width, height, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT));

		CV::CVUtilities::randomizeFrame(frame0, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(frame1, false, &randomGenerator);

		// Add valid locations nearest to buffer boundaries to test for memory access violation bugs
		offsets0[0] = 0u;
		offsets1[0] = 0u;
		
		offsets0[1] = height * frame0.strideElements() - tBufferSize;
		offsets1[1] = height * frame1.strideElements() - tBufferSize;

		for (unsigned int n = 2u; n < locations; ++n)
		{
			offsets0[n] = RandomI::random(randomGenerator, height * frame0.strideElements() - tBufferSize);
			offsets1[n] = RandomI::random(randomGenerator, height * frame1.strideElements() - tBufferSize);
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
						resultsNaive[n] = CV::SumSquareDifferencesBase::buffer8BitPerChannel(data0 + offsets0[n], data1 + offsets1[n], tBufferSize);
					}

					break;
				}

				case IT_TEMPLATE:
				{
					resultsTemplate.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceTemplate);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsTemplate[n] = CV::SumSquareDifferencesBase::buffer8BitPerChannelTemplate<tBufferSize>(data0 + offsets0[n], data1 + offsets1[n]);
					}

					break;
				}

				case IT_SSE:
				{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
					if constexpr (tPixels >= 8u)
					{
						resultsSSE.resize(locations);

						const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceSSE);

						for (size_t n = 0; n < locations; ++n)
						{
							resultsSSE[n] = CV::SumSquareDifferencesSSE::buffer8BitPerChannel<tBufferSize>(data0 + offsets0[n], data1 + offsets1[n]);
						}
					}
#endif // defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

					break;
				}

				case IT_NEON:
				{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
					if constexpr (tPixels >= 8u)
					{
						resultsNEON.resize(locations);

						const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceNEON);

						for (size_t n = 0; n < locations; ++n)
						{
							resultsNEON[n] = CV::SumSquareDifferencesNEON::buffer8BitPerChannel<tBufferSize>(data0 + offsets0[n], data1 + offsets1[n]);
						}
					}
#endif // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

					break;
				}

				case IT_DEFAULT:
				{
					resultsDefault.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceDefault);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsDefault[n] = CV::SumSquareDifferences::buffer8BitPerChannel<tChannels, tPixels>(data0 + offsets0[n], data1 + offsets1[n]);
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
			uint32_t ssdTest = 0u;

			const uint8_t* const pixel0 = data0 + offsets0[n];
			const uint8_t* const pixel1 = data1 + offsets1[n];

			for (unsigned int i = 0u; i < tBufferSize; ++i)
			{
				const int32_t value = int32_t(pixel0[i]) - int32_t(pixel1[i]);
				ssdTest += uint32_t(value * value);
			}

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
bool TestSumSquareDifferences::testPatchBuffer8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration)
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

	Indices32 centersX0(locations);
	Indices32 centersY0(locations);
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

		// Add valid locations nearest to buffer boundaries to test for memory access violation bugs
		centersX0[0] = tPatchSize_2;
		centersY0[0] = tPatchSize_2;
		offsets1[0] = 0u;

		centersX0[1] = width0 - tPatchSize_2 - 1u;
		centersY0[1] = height0 - tPatchSize_2 - 1u;
		offsets1[1] = height1 * frame1.strideElements() - tBufferSize;

		for (unsigned int n = 2u; n < locations; ++n)
		{
			centersX0[n] = RandomI::random(randomGenerator, tPatchSize_2, width0 - tPatchSize_2 - 1u);
			centersY0[n] = RandomI::random(randomGenerator, tPatchSize_2, height0 - tPatchSize_2 - 1u);

			offsets1[n] = RandomI::random(randomGenerator, height1 * frame1.strideElements() - tBufferSize);
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
						resultsNaive[n] = CV::SumSquareDifferencesBase::patchBuffer8BitPerChannel(data0, tChannels, tPatchSize, width0, centersX0[n], centersY0[n], paddingElements0, data1 + offsets1[n]);
					}

					break;
				}

				case IT_TEMPLATE:
				{
					resultsTemplate.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceTemplate);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsTemplate[n] = CV::SumSquareDifferencesBase::patchBuffer8BitPerChannelTemplate<tChannels, tPatchSize>(data0, width0, centersX0[n], centersY0[n], paddingElements0, data1 + offsets1[n]);
					}

					break;
				}

				case IT_SSE:
				{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
					if constexpr (tPatchSize >= 5u)
					{
						resultsSSE.resize(locations);

						const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceSSE);

						const unsigned int patch0StrideElements = frame0.strideElements();

						for (size_t n = 0; n < locations; ++n)
						{
							const uint8_t* const patch0 = data0 + (centersY0[n] - tPatchSize_2) * patch0StrideElements + (centersX0[n] - tPatchSize_2) * tChannels;
							const uint8_t* const buffer1 = data1 + offsets1[n];

							resultsSSE[n] = CV::SumSquareDifferencesSSE::patchBuffer8BitPerChannel<tChannels, tPatchSize>(patch0, buffer1, patch0StrideElements);
						}
					}
#endif // defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

					break;
				}

				case IT_NEON:
				{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
					if constexpr (tPatchSize >= 5u)
					{
						resultsNEON.resize(locations);

						const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceNEON);

						const unsigned int patch0StrideElements = frame0.strideElements();

						for (size_t n = 0; n < locations; ++n)
						{
							const uint8_t* const patch0 = data0 + (centersY0[n] - tPatchSize_2) * patch0StrideElements + (centersX0[n] - tPatchSize_2) * tChannels;
							const uint8_t* const buffer1 = data1 + offsets1[n];

							resultsNEON[n] = CV::SumSquareDifferencesNEON::patchBuffer8BitPerChannel<tChannels, tPatchSize>(patch0, buffer1, patch0StrideElements);
						}
					}
#endif // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

					break;
				}

				case IT_DEFAULT:
				{
					resultsDefault.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceDefault);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsDefault[n] = CV::SumSquareDifferences::patchBuffer8BitPerChannel<tChannels, tPatchSize>(data0, width0, centersX0[n], centersY0[n], paddingElements0, data1 + offsets1[n]);
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
			uint32_t ssdTest = 0u;

			const uint8_t* pixel0 = frame0.constpixel<uint8_t>(centersX0[n] - tPatchSize_2, centersY0[n] - tPatchSize_2);
			const uint8_t* pixel1 = frame1.constdata<uint8_t>() + offsets1[n];

			for (unsigned int y = 0u; y < tPatchSize; ++y)
			{
				for (unsigned int i = 0u; i < tChannels * tPatchSize; ++i)
				{
					const int32_t value = int32_t(pixel0[i]) - int32_t(pixel1[i]);
					ssdTest += uint32_t(value * value);
				}

				pixel0 += frame0.strideElements();
				pixel1 += tChannels * tPatchSize;
			}

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
bool TestSumSquareDifferences::testPatchAtBorder8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration)
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

		const unsigned int paddingElements0 = frame0.paddingElements();
		const unsigned int paddingElements1 = frame1.paddingElements();

		// Add valid locations nearest to buffer boundaries to test for memory access violation bugs
		centersX0[0] = 0u;
		centersY0[0] = 0u;
		centersX1[0] = 0u;
		centersY1[0] = 0u;

		centersX0[1] = width0 - 1u;
		centersY0[1] = height0 - 1u;
		centersX1[1] = width1 - 1u;
		centersY1[1] = height1 - 1u;

		for (unsigned int n = 2u; n < locations; ++n)
		{
			centersX0[n] = RandomI::random(randomGenerator, 0u, width0 - 1u);
			centersY0[n] = RandomI::random(randomGenerator, 0u, height0 - 1u);

			centersX1[n] = RandomI::random(randomGenerator, 0u, width1 - 1u);
			centersY1[n] = RandomI::random(randomGenerator, 0u, height1 - 1u);
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
						resultsNaive[n] = CV::SumSquareDifferencesBase::patchAtBorder8BitPerChannel(data0, data1, tChannels, tPatchSize, width0, height0, width1, height1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
					}

					break;
				}

				case IT_TEMPLATE:
				{
					resultsTemplate.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceTemplate);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsTemplate[n] = CV::SumSquareDifferencesBase::patchAtBorder8BitPerChannelTemplate<tChannels, tPatchSize>(data0, data1, width0, height0, width1, height1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
					}

					break;
				}

				case IT_SSE:
				{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
					/*if constexpr (tPatchSize == 5u) // not yet implemented
					{
						resultsSSE.resize(locations);

						const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceSSE);

						for (size_t n = 0; n < locations; ++n)
						{
							...
						}
					}*/
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
						resultsDefault[n] = CV::SumSquareDifferences::patchAtBorder8BitPerChannel<tChannels, tPatchSize>(data0, data1, width0, height0, width1, height1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
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
			const IndexPair32 ssdTest = calculateAtBorder8BitPerChannel(frame0, frame1, CV::PixelPosition(centersX0[n], centersY0[n]), CV::PixelPosition(centersX1[n], centersY1[n]), tPatchSize);

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
bool TestSumSquareDifferences::testPatchMirroredBorder8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize % 2u == 1u, "Invalid size");

	ocean_assert(width >= tPatchSize && height >= tPatchSize);
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	Log::info() << "... with " << tChannels << " channels and " << tPatchSize * tPatchSize << " pixels (" << tPatchSize << "x" << tPatchSize << "):";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceTemplate;
	HighPerformanceStatistic performanceNEON;
	HighPerformanceStatistic performanceDefault;

	constexpr size_t locations = 10000;

	Indices32 centersX0(locations);
	Indices32 centersY0(locations);
	Indices32 centersX1(locations);
	Indices32 centersY1(locations);

	Indices32 resultsTemplate;
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

		// Add valid locations nearest to buffer boundaries to test for memory access violation bugs
		centersX0[0] = 0u;
		centersY0[0] = 0u;
		centersX1[0] = 0u;
		centersY1[0] = 0u;

		centersX0[1] = width0 - 1u;
		centersY0[1] = height0 - 1u;
		centersX1[1] = width1 - 1u;
		centersY1[1] = height1 - 1u;

		for (unsigned int n = 2u; n < locations; ++n)
		{
			centersX0[n] = RandomI::random(randomGenerator, 0u, width0 - 1u);
			centersY0[n] = RandomI::random(randomGenerator, 0u, height0 - 1u);

			centersX1[n] = RandomI::random(randomGenerator, 0u, width1 - 1u);
			centersY1[n] = RandomI::random(randomGenerator, 0u, height1 - 1u);
		}

		const uint8_t* const data0 = frame0.constdata<uint8_t>();
		const uint8_t* const data1 = frame1.constdata<uint8_t>();

		for (const ImplementationType implementationType : {IT_TEMPLATE, IT_NEON, IT_DEFAULT})
		{
			switch (implementationType)
			{
				case IT_TEMPLATE:
				{
					resultsTemplate.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceTemplate);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsTemplate[n] = CV::SumSquareDifferencesBase::patchMirroredBorder8BitPerChannelTemplate<tChannels>(data0, data1, tPatchSize, width0, height0, width1, height1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
					}

					break;
				}

				case IT_NEON:
				{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
					if constexpr (tPatchSize >= 5u)
					{
						resultsNEON.resize(locations);

						const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceNEON);

						for (size_t n = 0; n < locations; ++n)
						{
							resultsNEON[n] = CV::SumSquareDifferencesNEON::patchMirroredBorder8BitPerChannel<tChannels, tPatchSize>(data0, data1, width0, height0, width1, height1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
						}
					}
#endif // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

					break;
				}

				case IT_DEFAULT:
				{
					resultsDefault.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceDefault);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsDefault[n] = CV::SumSquareDifferences::patchMirroredBorder8BitPerChannel<tChannels, tPatchSize>(data0, data1, width0, height0, width1, height1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
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
			uint32_t ssdTest = 0u;

			int y1 = int(centersY1[n]) - int(tPatchSize_2);
			for (int y0 = int(centersY0[n]) - int(tPatchSize_2); y0 <= int(centersY0[n]) + int(tPatchSize_2); ++y0)
			{
				const unsigned int yMirror0 = y0 + CV::CVUtilities::mirrorOffset(y0, frame0.height());
				const unsigned int yMirror1 = y1 + CV::CVUtilities::mirrorOffset(y1, frame1.height());

				int x1 = int(centersX1[n]) - int(tPatchSize_2);
				for (int x0 = int(centersX0[n]) - int(tPatchSize_2); x0 <= int(centersX0[n]) + int(tPatchSize_2); ++x0)
				{
					const unsigned int xMirror0 = x0 + CV::CVUtilities::mirrorOffset(x0, frame0.width());
					const unsigned int xMirror1 = x1 + CV::CVUtilities::mirrorOffset(x1, frame1.width());

					const uint8_t* const pixel0 = frame0.constpixel<uint8_t>(xMirror0, yMirror0);
					const uint8_t* const pixel1 = frame1.constpixel<uint8_t>(xMirror1, yMirror1);

					for (unsigned int c = 0u; c < tChannels; ++c)
					{
						const int32_t value = int32_t(pixel0[c]) - int32_t(pixel1[c]);
						ssdTest += uint32_t(value * value);
					}

					++x1;
				}

				++y1;
			}

			if (!resultsTemplate.empty() && resultsTemplate[n] != ssdTest)
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

	if (performanceTemplate.measurements() != 0u)
	{
		Log::info() << "Template: [" << performanceTemplate.bestMseconds() << ", " << performanceTemplate.medianMseconds() << ", " << performanceTemplate.worstMseconds() << "] ms";
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

IndexPair32 TestSumSquareDifferences::calculateAtBorder8BitPerChannel(const Frame& frame0, const Frame& frame1, const CV::PixelPosition& center0, const CV::PixelPosition& center1, const unsigned int patchSize)
{
	ocean_assert(frame0.isValid() && frame1.isValid());

	if (!frame0.isValid() || !frame1.isValid() || !frame0.isPixelFormatCompatible(frame1.pixelFormat()))
	{
		ocean_assert(false && "This should never happen!");
		return IndexPair32(uint32_t(-1), uint32_t(-1));
	}

	ocean_assert(patchSize >= 1 && patchSize % 2u == 1u);
	const unsigned int patchSize_2 = patchSize / 2u;

	if (center0.x() >= frame0.width() || center0.y() >= frame0.height())
	{
		ocean_assert(false && "Invalid input!");
		return IndexPair32(uint32_t(-1), uint32_t(-1));
	}

	if (center1.x() >= frame1.width() || center1.y() >= frame1.height())
	{
		ocean_assert(false && "Invalid input!");
		return IndexPair32(uint32_t(-1), uint32_t(-1));
	}

	unsigned int ssd = 0u;
	unsigned int numberPixels = 0u;

	const unsigned int channels = frame0.channels();

	for (int yy = -int(patchSize_2); yy <= int(patchSize_2); ++yy)
	{
		const int y0 = int(center0.y()) + yy;
		const int y1 = int(center1.y()) + yy;

		for (int xx = -int(patchSize_2); xx <= int(patchSize_2); ++xx)
		{
			const int x0 = int(center0.x()) + xx;
			const int x1 = int(center1.x()) + xx;

			if (x0 >= 0 && x0 < int(frame0.width()) && y0 >= 0 && y0 < int(frame0.height()))
			{
				if (x1 >= 0 && x1 < int(frame1.width()) && y1 >= 0 && y1 < int(frame1.height()))
				{
					const uint8_t* pixel0 = frame0.constpixel<uint8_t>((unsigned int)(x0), (unsigned int)(y0));
					const uint8_t* pixel1 = frame1.constpixel<uint8_t>((unsigned int)(x1), (unsigned int)(y1));

					for (unsigned int n = 0u; n < channels; ++n)
					{
						int value = int(pixel0[n]) - int(pixel1[n]);
						ssd += value * value;
					}

					++numberPixels;
				}
				else
				{
					return IndexPair32(uint32_t(-1), 0u);
				}
			}
		}
	}

	return IndexPair32(ssd, numberPixels);
}

}

}

}
