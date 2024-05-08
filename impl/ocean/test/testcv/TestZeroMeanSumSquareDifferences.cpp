/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestZeroMeanSumSquareDifferences.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/ZeroMeanSumSquareDifferences.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestZeroMeanSumSquareDifferences::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Test zero mean sum of square differences:   ---";
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

	allSucceeded = testPatchMirroredBorder8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Zero mean sum of square differences test succeeded.";
	}
	else
	{
		Log::info() << "Zero mean sum of square differences test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestZeroMeanSumSquareDifferences, Patch8BitPerChannel)
{
	EXPECT_TRUE(TestZeroMeanSumSquareDifferences::testPatch8BitPerChannel(GTEST_TEST_DURATION));
}

TEST(TestZeroMeanSumSquareDifferences, Buffer8BitPerChannel)
{
	EXPECT_TRUE(TestZeroMeanSumSquareDifferences::testBuffer8BitPerChannel(GTEST_TEST_DURATION));
}

TEST(TestZeroMeanSumSquareDifferences, PatchBuffer8BitPerChannel)
{
	EXPECT_TRUE(TestZeroMeanSumSquareDifferences::testPatchBuffer8BitPerChannel(GTEST_TEST_DURATION));
}

TEST(TestZeroMeanSumSquareDifferences, PatchMirroredBorder8BitPerChannel)
{
	EXPECT_TRUE(TestZeroMeanSumSquareDifferences::testPatchMirroredBorder8BitPerChannel(GTEST_TEST_DURATION));
}

#endif

bool TestZeroMeanSumSquareDifferences::testPatch8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "ZMSSD between two patches:";
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

bool TestZeroMeanSumSquareDifferences::testBuffer8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "ZMSSD between two buffers:";
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

bool TestZeroMeanSumSquareDifferences::testPatchBuffer8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "ZMSSD between a patch and a buffer:";
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

bool TestZeroMeanSumSquareDifferences::testPatchMirroredBorder8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "ZMSSD between two patches with mirrored border:";
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
bool TestZeroMeanSumSquareDifferences::testPatch8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration)
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
						resultsNaive[n] = CV::ZeroMeanSumSquareDifferencesBase::patch8BitPerChannel<tChannels>(data0, data1, tPatchSize, width0, width1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
					}

					break;
				}

				case IT_TEMPLATE:
				{
					resultsTemplate.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceTemplate);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsTemplate[n] = CV::ZeroMeanSumSquareDifferencesBase::patch8BitPerChannelTemplate<tChannels, tPatchSize>(data0, data1, width0, width1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
					}

					break;
				}

				case IT_SSE:
				{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
					if constexpr (tChannels == 1u || tChannels == 3u)
					{
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

								resultsSSE[n] = CV::ZeroMeanSumSquareDifferencesSSE::patch8BitPerChannel<tChannels, tPatchSize>(patch0, patch1, patch0StrideElements, patch1StrideElements);
							}
						}
					}
#endif // defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

					break;
				}

				case IT_NEON:
				{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
					if constexpr (tChannels == 1u || tChannels == 3u)
					{
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

								resultsNEON[n] = CV::ZeroMeanSumSquareDifferencesNEON::patch8BitPerChannel<tChannels, tPatchSize>(patch0, patch1, patch0StrideElements, patch1StrideElements);
							}
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
						resultsDefault[n] = CV::ZeroMeanSumSquareDifferences::patch8BitPerChannel<tChannels, tPatchSize>(data0, data1, width0, width1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
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
			uint32_t zmssdTest = 0u;

			Indices32 mean0(tChannels, 0u);
			Indices32 mean1(tChannels, 0u);

			unsigned int y0 = centersY0[n];
			for (unsigned int y1 = centersY1[n]; y1 < centersY1[n] + tPatchSize; ++y1)
			{
				const uint8_t* pixel0 = frame0.constpixel<uint8_t>(centersX0[n] - tPatchSize_2, y0 - tPatchSize_2);
				const uint8_t* pixel1 = frame1.constpixel<uint8_t>(centersX1[n] - tPatchSize_2, y1 - tPatchSize_2);

				for (unsigned int x = 0u; x < tPatchSize; ++x)
				{
					for (unsigned int c = 0u; c < tChannels; ++c)
					{
						mean0[c] += pixel0[x * tChannels + c];
						mean1[c] += pixel1[x * tChannels + c];
					}
				}

				++y0;
			}

			for (unsigned int i = 0u; i < tChannels; ++i)
			{
				mean0[i] = (mean0[i] + tPatchSize * tPatchSize / 2u) / (tPatchSize * tPatchSize);
				mean1[i] = (mean1[i] + tPatchSize * tPatchSize / 2u) / (tPatchSize * tPatchSize);
			}

			y0 = centersY0[n];
			for (unsigned int y1 = centersY1[n]; y1 < centersY1[n] + tPatchSize; ++y1)
			{
				const uint8_t* pixel0 = frame0.constpixel<uint8_t>(centersX0[n] - tPatchSize_2, y0 - tPatchSize_2);
				const uint8_t* pixel1 = frame1.constpixel<uint8_t>(centersX1[n] - tPatchSize_2, y1 - tPatchSize_2);

				for (unsigned int x = 0u; x < tPatchSize; ++x)
				{
					for (unsigned int c = 0u; c < tChannels; ++c)
					{
						const int32_t difference = (int32_t(pixel0[x * tChannels + c]) - int32_t(mean0[c])) - (int32_t(pixel1[x * tChannels + c]) - int32_t(mean1[c]));

						zmssdTest += uint32_t(difference * difference);
					}
				}

				++y0;
			}

			if (!resultsNaive.empty() && resultsNaive[n] != zmssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsTemplate.empty() && resultsTemplate[n] != zmssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsSSE.empty() && resultsSSE[n] != zmssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsNEON.empty() && resultsNEON[n] != zmssdTest)
			{
				allSucceeded = false;
			}

			if (resultsDefault[n] != zmssdTest)
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
bool TestZeroMeanSumSquareDifferences::testBuffer8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPixels >= 1u, "Invalid number of pixels");

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
						resultsNaive[n] = CV::ZeroMeanSumSquareDifferencesBase::buffer8BitPerChannel<tChannels>(data0 + offsets0[n], data1 + offsets1[n], tPixels);
					}

					break;
				}

				case IT_TEMPLATE:
				{
					resultsTemplate.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceTemplate);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsTemplate[n] = CV::ZeroMeanSumSquareDifferencesBase::buffer8BitPerChannelTemplate<tChannels, tPixels>(data0 + offsets0[n], data1 + offsets1[n]);
					}

					break;
				}

				case IT_SSE:
				{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
					if constexpr (tChannels == 1u || tChannels == 3u)
					{
						if constexpr (tPixels >= 8u)
						{
							resultsSSE.resize(locations);

							const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceSSE);

							for (size_t n = 0; n < locations; ++n)
							{
								resultsSSE[n] = CV::ZeroMeanSumSquareDifferencesSSE::buffer8BitPerChannel<tChannels, tPixels>(data0 + offsets0[n], data1 + offsets1[n]);
							}
						}
					}
#endif // defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

					break;
				}

				case IT_NEON:
				{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
					if constexpr (tChannels == 1u || tChannels == 3u)
					{
						if constexpr (tPixels >= 8u)
						{
							resultsNEON.resize(locations);

							const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceNEON);

							for (size_t n = 0; n < locations; ++n)
							{
								resultsNEON[n] = CV::ZeroMeanSumSquareDifferencesNEON::buffer8BitPerChannel<tChannels, tPixels>(data0 + offsets0[n], data1 + offsets1[n]);
							}
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
						resultsDefault[n] = CV::ZeroMeanSumSquareDifferences::buffer8BitPerChannel<tChannels, tPixels>(data0 + offsets0[n], data1 + offsets1[n]);
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
			uint32_t zmssdTest = 0u;

			Indices32 mean0(tChannels, 0u);
			Indices32 mean1(tChannels, 0u);

			const uint8_t* const pixel0 = data0 + offsets0[n];
			const uint8_t* const pixel1 = data1 + offsets1[n];

			for (unsigned int i = 0u; i < tPixels; ++i)
			{
				for (unsigned int c = 0u; c < tChannels; ++c)
				{
					mean0[c] += pixel0[i * tChannels + c];
					mean1[c] += pixel1[i * tChannels + c];
				}
			}

			for (unsigned int c = 0u; c < tChannels; ++c)
			{
				mean0[c] = (mean0[c] + tPixels / 2u) / tPixels;
				mean1[c] = (mean1[c] + tPixels / 2u) / tPixels;
			}

			for (unsigned int i = 0u; i < tPixels; ++i)
			{
				for (unsigned int c = 0u; c < tChannels; ++c)
				{
					const int32_t difference = (int32_t(pixel0[i * tChannels + c]) - int32_t(mean0[c])) - (int32_t(pixel1[i * tChannels + c]) - int32_t(mean1[c]));

					zmssdTest += uint32_t(difference * difference);
				}
			}

			if (!resultsNaive.empty() && resultsNaive[n] != zmssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsTemplate.empty() && resultsTemplate[n] != zmssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsSSE.empty() && resultsSSE[n] != zmssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsNEON.empty() && resultsNEON[n] != zmssdTest)
			{
				allSucceeded = false;
			}

			if (resultsDefault[n] != zmssdTest)
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
bool TestZeroMeanSumSquareDifferences::testPatchBuffer8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration)
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
		offsets1[0] = 0;

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
						resultsNaive[n] = CV::ZeroMeanSumSquareDifferencesBase::patchBuffer8BitPerChannel<tChannels>(data0, tPatchSize, width0, centersX0[n], centersY0[n], paddingElements0, data1 + offsets1[n]);
					}

					break;
				}

				case IT_TEMPLATE:
				{
					resultsTemplate.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceTemplate);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsTemplate[n] = CV::ZeroMeanSumSquareDifferencesBase::patchBuffer8BitPerChannelTemplate<tChannels, tPatchSize>(data0, width0, centersX0[n], centersY0[n], paddingElements0, data1 + offsets1[n]);
					}

					break;
				}

				case IT_SSE:
				{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
					if constexpr (tChannels == 1u || tChannels == 3u)
					{
						if constexpr (tPatchSize >= 5u)
						{
							resultsSSE.resize(locations);

							const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceSSE);

							const unsigned int patch0StrideElements = frame0.strideElements();

							for (size_t n = 0; n < locations; ++n)
							{
								const uint8_t* const patch0 = data0 + (centersY0[n] - tPatchSize_2) * patch0StrideElements + (centersX0[n] - tPatchSize_2) * tChannels;
								const uint8_t* const buffer1 = data1 + offsets1[n];

								resultsSSE[n] = CV::ZeroMeanSumSquareDifferencesSSE::patchBuffer8BitPerChannel<tChannels, tPatchSize>(patch0, buffer1, patch0StrideElements);
							}
						}
					}
#endif // defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

					break;
				}

				case IT_NEON:
				{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
					if constexpr (tChannels == 1u || tChannels == 3u)
					{
						if constexpr (tPatchSize >= 5u)
						{
							resultsNEON.resize(locations);

							const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceNEON);

							const unsigned int patch0StrideElements = frame0.strideElements();

							for (size_t n = 0; n < locations; ++n)
							{
								const uint8_t* const patch0 = data0 + (centersY0[n] - tPatchSize_2) * patch0StrideElements + (centersX0[n] - tPatchSize_2) * tChannels;
								const uint8_t* const buffer1 = data1 + offsets1[n];

								resultsNEON[n] = CV::ZeroMeanSumSquareDifferencesNEON::patchBuffer8BitPerChannel<tChannels, tPatchSize>(patch0, buffer1, patch0StrideElements);
							}
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
						resultsDefault[n] = CV::ZeroMeanSumSquareDifferences::patchBuffer8BitPerChannel<tChannels, tPatchSize>(data0, width0, centersX0[n], centersY0[n], paddingElements0, data1 + offsets1[n]);
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
			uint32_t zmssdTest = 0u;

			Indices32 mean0(tChannels, 0u);
			Indices32 mean1(tChannels, 0u);

			const uint8_t* pixel1 = frame1.constdata<uint8_t>() + offsets1[n];

			for (unsigned int y0 = centersY0[n]; y0 < centersY0[n] + tPatchSize; ++y0)
			{
				const uint8_t* pixel0 = frame0.constpixel<uint8_t>(centersX0[n] - tPatchSize_2, y0 - tPatchSize_2);

				for (unsigned int x = 0u; x < tPatchSize; ++x)
				{
					for (unsigned int c = 0u; c < tChannels; ++c)
					{
						mean0[c] += pixel0[x * tChannels + c];
						mean1[c] += *pixel1++;
					}
				}
			}

			for (unsigned int i = 0u; i < tChannels; ++i)
			{
				mean0[i] = (mean0[i] + tPatchSize * tPatchSize / 2u) / (tPatchSize * tPatchSize);
				mean1[i] = (mean1[i] + tPatchSize * tPatchSize / 2u) / (tPatchSize * tPatchSize);
			}

			pixel1 = frame1.constdata<uint8_t>() + offsets1[n];

			for (unsigned int y0 = centersY0[n]; y0 < centersY0[n] + tPatchSize; ++y0)
			{
				const uint8_t* pixel0 = frame0.constpixel<uint8_t>(centersX0[n] - tPatchSize_2, y0 - tPatchSize_2);

				for (unsigned int x = 0u; x < tPatchSize; ++x)
				{
					for (unsigned int c = 0u; c < tChannels; ++c)
					{
						const int32_t difference = (int32_t(pixel0[x * tChannels + c]) - int32_t(mean0[c])) - (int32_t(*pixel1++) - int32_t(mean1[c]));

						zmssdTest += uint32_t(difference * difference);
					}
				}
			}

			if (!resultsNaive.empty() && resultsNaive[n] != zmssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsTemplate.empty() && resultsTemplate[n] != zmssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsSSE.empty() && resultsSSE[n] != zmssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsNEON.empty() && resultsNEON[n] != zmssdTest)
			{
				allSucceeded = false;
			}

			if (resultsDefault[n] != zmssdTest)
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
bool TestZeroMeanSumSquareDifferences::testPatchMirroredBorder8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration)
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
	HighPerformanceStatistic performanceNEON;
	HighPerformanceStatistic performanceDefault;

	constexpr size_t locations = 10000;

	Indices32 centersX0(locations);
	Indices32 centersY0(locations);
	Indices32 centersX1(locations);
	Indices32 centersY1(locations);

	Indices32 resultsNaive;
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

		for (const ImplementationType implementationType : {IT_NAIVE, IT_NEON, IT_DEFAULT})
		{
			switch (implementationType)
			{
				case IT_NAIVE:
				{
					resultsNaive.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceNaive);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsNaive[n] = CV::ZeroMeanSumSquareDifferencesBase::patchMirroredBorder8BitPerChannel<tChannels>(data0, data1, tPatchSize, width0, height0, width1, height1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
					}

					break;
				}

				case IT_NEON:
				{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
					if constexpr (tChannels == 1u)
					{
						if constexpr (tPatchSize >= 5u)
						{
							resultsNEON.resize(locations);

							const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceNEON);

							for (size_t n = 0; n < locations; ++n)
							{
								resultsNEON[n] = CV::ZeroMeanSumSquareDifferencesNEON::patchMirroredBorder8BitPerChannel<tChannels, tPatchSize>(data0, data1, width0, height0, width1, height1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
							}
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
						resultsDefault[n] = CV::ZeroMeanSumSquareDifferences::patchMirroredBorder8BitPerChannel<tChannels, tPatchSize>(data0, data1, width0, height0, width1, height1, centersX0[n], centersY0[n], centersX1[n], centersY1[n], paddingElements0, paddingElements1);
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
			uint32_t zmssdTest = 0u;

			Indices32 mean0(tChannels, 0u);
			Indices32 mean1(tChannels, 0u);

			int y1 = int(centersY1[n]) - int(tPatchSize_2);
			for (int y0 = int(centersY0[n]) - int(tPatchSize_2); y0 <= int(centersY0[n]) + int(tPatchSize_2); ++y0)
			{
				const unsigned int yMirror0 = y0 + CV::CVUtilities::mirrorOffset(y0, height0);
				const unsigned int yMirror1 = y1 + CV::CVUtilities::mirrorOffset(y1, height1);

				int x1 = int(centersX1[n]) - int(tPatchSize_2);
				for (int x0 = int(centersX0[n]) - int(tPatchSize_2); x0 <= int(centersX0[n]) + int(tPatchSize_2); ++x0)
				{
					const unsigned int xMirror0 = x0 + CV::CVUtilities::mirrorOffset(x0, width0);
					const unsigned int xMirror1 = x1 + CV::CVUtilities::mirrorOffset(x1, width1);

					const uint8_t* const pixel0 = frame0.constpixel<uint8_t>(xMirror0, yMirror0);
					const uint8_t* const pixel1 = frame1.constpixel<uint8_t>(xMirror1, yMirror1);

					for (unsigned int c = 0u; c < tChannels; ++c)
					{
						mean0[c] += pixel0[c];
						mean1[c] += pixel1[c];
					}

					++x1;
				}

				++y1;
			}

			for (unsigned int i = 0u; i < tChannels; ++i)
			{
				mean0[i] = (mean0[i] + tPatchSize * tPatchSize / 2u) / (tPatchSize * tPatchSize);
				mean1[i] = (mean1[i] + tPatchSize * tPatchSize / 2u) / (tPatchSize * tPatchSize);
			}

			y1 = int(centersY1[n]) - int(tPatchSize_2);
			for (int y0 = int(centersY0[n]) - int(tPatchSize_2); y0 <= int(centersY0[n]) + int(tPatchSize_2); ++y0)
			{
				const unsigned int yMirror0 = y0 + CV::CVUtilities::mirrorOffset(y0, height0);
				const unsigned int yMirror1 = y1 + CV::CVUtilities::mirrorOffset(y1, height1);

				int x1 = int(centersX1[n]) - int(tPatchSize_2);
				for (int x0 = int(centersX0[n]) - int(tPatchSize_2); x0 <= int(centersX0[n]) + int(tPatchSize_2); ++x0)
				{
					const unsigned int xMirror0 = x0 + CV::CVUtilities::mirrorOffset(x0, width0);
					const unsigned int xMirror1 = x1 + CV::CVUtilities::mirrorOffset(x1, width1);

					const uint8_t* const pixel0 = frame0.constpixel<uint8_t>(xMirror0, yMirror0);
					const uint8_t* const pixel1 = frame1.constpixel<uint8_t>(xMirror1, yMirror1);

					for (unsigned int c = 0u; c < tChannels; ++c)
					{
						const int32_t difference = (int32_t(pixel0[c]) - int32_t(mean0[c])) - (int32_t(pixel1[c]) - int32_t(mean1[c]));

						zmssdTest += uint32_t(difference * difference);
					}

					++x1;
				}

				++y1;
			}

			if (!resultsNaive.empty() && resultsNaive[n] != zmssdTest)
			{
				allSucceeded = false;
			}

			if (!resultsNEON.empty() && resultsNEON[n] != zmssdTest)
			{
				allSucceeded = false;
			}

			if (resultsDefault[n] != zmssdTest)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	static_assert(locations != 0, "Invalid number of locations!");

	if (performanceNaive.measurements() != 0u)
	{
		Log::info() << "  Naive: [" << performanceNaive.bestMseconds() << ", " << performanceNaive.medianMseconds() << ", " << performanceNaive.worstMseconds() << "] ms";
	}

	if (performanceNEON.measurements() != 0u)
	{
		Log::info() << "   NEON: [" << performanceNEON.bestMseconds() << ", " << performanceNEON.medianMseconds() << ", " << performanceNEON.worstMseconds() << "] ms";
	}

	ocean_assert(performanceDefault.measurements() != 0u);
	Log::info() << "Default: [" << performanceDefault.bestMseconds() << ", " << performanceDefault.medianMseconds() << ", " << performanceDefault.worstMseconds() << "] ms";

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

}

}

}
