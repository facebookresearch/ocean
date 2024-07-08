/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestHistogram.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameEnlarger.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/Histogram.h"

#include "ocean/math/Random.h"

#include <array>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestHistogram::test(const double testDuration, Worker& worker)
{
	assert(testDuration > 0.0);

	Log::info() << "---   Histogram test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testDetermineHistogram8BitPerChannel(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetermineHistogram8BitPerChannelSubFrame(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testContrastLimitedAdaptiveHistogramTileLookupTables(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testContrastLimitedAdaptiveBilinearInterpolationParameters(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testContrastLimitedHistogramEqualization(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Histogram test succeeded.";
	}
	else
	{
		Log::info() << "Histogram test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

// Test determineHistogram8BitPerChannel()

// One channel

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_127x127_1)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<1u>(GTEST_TEST_DURATION, 127u, 127u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_128x128_1)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<1u>(GTEST_TEST_DURATION, 128u, 128u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_255x255_1)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<1u>(GTEST_TEST_DURATION, 255u, 255u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_256x256_1)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<1u>(GTEST_TEST_DURATION, 256u, 256u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_511x511_1)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<1u>(GTEST_TEST_DURATION, 511u, 511u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_512x512_1)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<1u>(GTEST_TEST_DURATION, 512u, 512u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_639x479_1)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<1u>(GTEST_TEST_DURATION, 639u, 479u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_640x480_1)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<1u>(GTEST_TEST_DURATION, 640u, 480u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_1279x719_1)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<1u>(GTEST_TEST_DURATION, 1279u, 719u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_1280x720_1)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<1u>(GTEST_TEST_DURATION, 1280u, 720u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_1919x1079_1)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<1u>(GTEST_TEST_DURATION, 1919u, 1079u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_1920x1080_1)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<1u>(GTEST_TEST_DURATION, 1920u, 1080u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_0x0_1)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<1u>(GTEST_TEST_DURATION, 0u, 0u, randomGenerator, worker));
}

// Two channels

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_127x127_2)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<2u>(GTEST_TEST_DURATION, 127u, 127u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_128x128_2)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<2u>(GTEST_TEST_DURATION, 128u, 128u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_255x255_2)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<2u>(GTEST_TEST_DURATION, 255u, 255u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_256x256_2)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<2u>(GTEST_TEST_DURATION, 256u, 256u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_511x511_2)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<2u>(GTEST_TEST_DURATION, 511u, 511u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_512x512_2)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<2u>(GTEST_TEST_DURATION, 512u, 512u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_639x479_2)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<2u>(GTEST_TEST_DURATION, 639u, 479u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_640x480_2)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<2u>(GTEST_TEST_DURATION, 640u, 480u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_1279x719_2)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<2u>(GTEST_TEST_DURATION, 1279u, 719u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_1280x720_2)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<2u>(GTEST_TEST_DURATION, 1280u, 720u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_1919x1079_2)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<2u>(GTEST_TEST_DURATION, 1919u, 1079u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_1920x1080_2)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<2u>(GTEST_TEST_DURATION, 1920u, 1080u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_0x0_2)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<2u>(GTEST_TEST_DURATION, 0u, 0u, randomGenerator, worker));
}

// Three channels

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_127x127_3)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<3u>(GTEST_TEST_DURATION, 127u, 127u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_128x128_3)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<3u>(GTEST_TEST_DURATION, 128u, 128u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_255x255_3)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<3u>(GTEST_TEST_DURATION, 255u, 255u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_256x256_3)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<3u>(GTEST_TEST_DURATION, 256u, 256u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_511x511_3)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<3u>(GTEST_TEST_DURATION, 511u, 511u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_512x512_3)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<3u>(GTEST_TEST_DURATION, 512u, 512u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_639x479_3)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<3u>(GTEST_TEST_DURATION, 639u, 479u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_640x480_3)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<3u>(GTEST_TEST_DURATION, 640u, 480u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_1279x719_3)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<3u>(GTEST_TEST_DURATION, 1279u, 719u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_1280x720_3)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<3u>(GTEST_TEST_DURATION, 1280u, 720u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_1919x1079_3)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<3u>(GTEST_TEST_DURATION, 1919u, 1079u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_1920x1080_3)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<3u>(GTEST_TEST_DURATION, 1920u, 1080u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_0x0_3)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<3u>(GTEST_TEST_DURATION, 0u, 0u, randomGenerator, worker));
}

// Four channels

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_127x127_4)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<4u>(GTEST_TEST_DURATION, 127u, 127u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_128x128_4)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<4u>(GTEST_TEST_DURATION, 128u, 128u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_255x255_4)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<4u>(GTEST_TEST_DURATION, 255u, 255u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_256x256_4)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<4u>(GTEST_TEST_DURATION, 256u, 256u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_511x511_4)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<4u>(GTEST_TEST_DURATION, 511u, 511u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_512x512_4)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<4u>(GTEST_TEST_DURATION, 512u, 512u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_639x479_4)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<4u>(GTEST_TEST_DURATION, 639u, 479u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_640x480_4)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<4u>(GTEST_TEST_DURATION, 640u, 480u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_1279x719_4)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<4u>(GTEST_TEST_DURATION, 1279u, 719u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_1280x720_4)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<4u>(GTEST_TEST_DURATION, 1280u, 720u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_1919x1079_4)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<4u>(GTEST_TEST_DURATION, 1919u, 1079u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_1920x1080_4)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<4u>(GTEST_TEST_DURATION, 1920u, 1080u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannel_0x0_4)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannel<4u>(GTEST_TEST_DURATION, 0u, 0u, randomGenerator, worker));
}



// Test determineHistogram8BitPerChannelSubFrame()

// One channel

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_127x127_1)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<1u>(GTEST_TEST_DURATION, 127u, 127u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_128x128_1)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<1u>(GTEST_TEST_DURATION, 128u, 128u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_255x255_1)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<1u>(GTEST_TEST_DURATION, 255u, 255u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_256x256_1)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<1u>(GTEST_TEST_DURATION, 256u, 256u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_511x511_1)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<1u>(GTEST_TEST_DURATION, 511u, 511u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_512x512_1)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<1u>(GTEST_TEST_DURATION, 512u, 512u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_639x479_1)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<1u>(GTEST_TEST_DURATION, 639u, 479u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_640x480_1)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<1u>(GTEST_TEST_DURATION, 640u, 480u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_1279x719_1)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<1u>(GTEST_TEST_DURATION, 1279u, 719u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_1280x720_1)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<1u>(GTEST_TEST_DURATION, 1280u, 720u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_1919x1079_1)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<1u>(GTEST_TEST_DURATION, 1919u, 1079u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_1920x1080_1)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<1u>(GTEST_TEST_DURATION, 1920u, 1080u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_0x0_1)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<1u>(GTEST_TEST_DURATION, 0u, 0u, randomGenerator, worker));
}

// Two channels

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_127x127_2)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<2u>(GTEST_TEST_DURATION, 127u, 127u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_128x128_2)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<2u>(GTEST_TEST_DURATION, 128u, 128u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_255x255_2)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<2u>(GTEST_TEST_DURATION, 255u, 255u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_256x256_2)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<2u>(GTEST_TEST_DURATION, 256u, 256u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_511x511_2)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<2u>(GTEST_TEST_DURATION, 511u, 511u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_512x512_2)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<2u>(GTEST_TEST_DURATION, 512u, 512u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_639x479_2)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<2u>(GTEST_TEST_DURATION, 639u, 479u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_640x480_2)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<2u>(GTEST_TEST_DURATION, 640u, 480u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_1279x719_2)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<2u>(GTEST_TEST_DURATION, 1279u, 719u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_1280x720_2)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<2u>(GTEST_TEST_DURATION, 1280u, 720u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_1919x1079_2)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<2u>(GTEST_TEST_DURATION, 1919u, 1079u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_1920x1080_2)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<2u>(GTEST_TEST_DURATION, 1920u, 1080u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_0x0_2)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<2u>(GTEST_TEST_DURATION, 0u, 0u, randomGenerator, worker));
}

// Three channels

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_127x127_3)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<3u>(GTEST_TEST_DURATION, 127u, 127u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_128x128_3)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<3u>(GTEST_TEST_DURATION, 128u, 128u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_255x255_3)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<3u>(GTEST_TEST_DURATION, 255u, 255u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_256x256_3)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<3u>(GTEST_TEST_DURATION, 256u, 256u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_511x511_3)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<3u>(GTEST_TEST_DURATION, 511u, 511u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_512x512_3)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<3u>(GTEST_TEST_DURATION, 512u, 512u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_639x479_3)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<3u>(GTEST_TEST_DURATION, 639u, 479u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_640x480_3)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<3u>(GTEST_TEST_DURATION, 640u, 480u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_1279x719_3)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<3u>(GTEST_TEST_DURATION, 1279u, 719u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_1280x720_3)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<3u>(GTEST_TEST_DURATION, 1280u, 720u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_1919x1079_3)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<3u>(GTEST_TEST_DURATION, 1919u, 1079u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_1920x1080_3)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<3u>(GTEST_TEST_DURATION, 1920u, 1080u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_0x0_3)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<3u>(GTEST_TEST_DURATION, 0u, 0u, randomGenerator, worker));
}

// Four channels

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_127x127_4)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<4u>(GTEST_TEST_DURATION, 127u, 127u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_128x128_4)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<4u>(GTEST_TEST_DURATION, 128u, 128u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_255x255_4)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<4u>(GTEST_TEST_DURATION, 255u, 255u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_256x256_4)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<4u>(GTEST_TEST_DURATION, 256u, 256u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_511x511_4)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<4u>(GTEST_TEST_DURATION, 511u, 511u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_512x512_4)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<4u>(GTEST_TEST_DURATION, 512u, 512u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_639x479_4)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<4u>(GTEST_TEST_DURATION, 639u, 479u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_640x480_4)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<4u>(GTEST_TEST_DURATION, 640u, 480u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_1279x719_4)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<4u>(GTEST_TEST_DURATION, 1279u, 719u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_1280x720_4)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<4u>(GTEST_TEST_DURATION, 1280u, 720u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_1919x1079_4)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<4u>(GTEST_TEST_DURATION, 1919u, 1079u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_1920x1080_4)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<4u>(GTEST_TEST_DURATION, 1920u, 1080u, randomGenerator, worker));
}

TEST(TestHistogram, TestDetermineHistogram8BitPerChannelSubFrame_0x0_4)
{
	RandomGenerator randomGenerator;
	Worker worker;
	EXPECT_TRUE(TestHistogram::testDetermineHistogram8BitPerChannelSubFrame<4u>(GTEST_TEST_DURATION, 0u, 0u, randomGenerator, worker));
}


// testContrastLimitedAdaptiveHistogramTileLookupTables()

TEST(TestHistogram, TestCLAHETileLookupTables_127x127_clipLimit_40_tiles_8x8)
{
	Worker worker;
	EXPECT_TRUE(TestHistogram::testContrastLimitedAdaptiveHistogramTileLookupTables(127u, 127u, Scalar(40), 8u, 8u, GTEST_TEST_DURATION, worker));
}

TEST(TestHistogram, TestCLAHETileLookupTables_128x128_clipLimit_40_tiles_8x8)
{
	Worker worker;
	EXPECT_TRUE(TestHistogram::testContrastLimitedAdaptiveHistogramTileLookupTables(128u, 128u, Scalar(40), 8u, 8u, GTEST_TEST_DURATION, worker));
}

TEST(TestHistogram, TestCLAHETileLookupTables_255x255_clipLimit_40_tiles_8x8)
{
	Worker worker;
	EXPECT_TRUE(TestHistogram::testContrastLimitedAdaptiveHistogramTileLookupTables(255u, 255u, Scalar(40), 8u, 8u, GTEST_TEST_DURATION, worker));
}

TEST(TestHistogram, TestCLAHETileLookupTables_256x256_clipLimit_40_tiles_8x8)
{
	Worker worker;
	EXPECT_TRUE(TestHistogram::testContrastLimitedAdaptiveHistogramTileLookupTables(256u, 256u, Scalar(40), 8u, 8u, GTEST_TEST_DURATION, worker));
}

TEST(TestHistogram, TestCLAHETileLookupTables_511x511_clipLimit_40_tiles_8x8)
{
	Worker worker;
	EXPECT_TRUE(TestHistogram::testContrastLimitedAdaptiveHistogramTileLookupTables(511u, 511u, Scalar(40), 8u, 8u, GTEST_TEST_DURATION, worker));
}

TEST(TestHistogram, TestCLAHETileLookupTables_512x512_clipLimit_40_tiles_8x8)
{
	Worker worker;
	EXPECT_TRUE(TestHistogram::testContrastLimitedAdaptiveHistogramTileLookupTables(512u, 512u, Scalar(40), 8u, 8u, GTEST_TEST_DURATION, worker));
}

TEST(TestHistogram, TestCLAHETileLookupTables_639x479_clipLimit_40_tiles_8x8)
{
	Worker worker;
	EXPECT_TRUE(TestHistogram::testContrastLimitedAdaptiveHistogramTileLookupTables(639u, 479u, Scalar(40), 8u, 8u, GTEST_TEST_DURATION, worker));
}

TEST(TestHistogram, TestCLAHETileLookupTables_640x480_clipLimit_40_tiles_8x8)
{
	Worker worker;
	EXPECT_TRUE(TestHistogram::testContrastLimitedAdaptiveHistogramTileLookupTables(640u, 480u, Scalar(40), 8u, 8u, GTEST_TEST_DURATION, worker));
}

TEST(TestHistogram, TestCLAHETileLookupTables_1279x719_clipLimit_40_tiles_8x8)
{
	Worker worker;
	EXPECT_TRUE(TestHistogram::testContrastLimitedAdaptiveHistogramTileLookupTables(1279u, 719u, Scalar(40), 8u, 8u, GTEST_TEST_DURATION, worker));
}

TEST(TestHistogram, TestCLAHETileLookupTables_1280x720_clipLimit_40_tiles_8x8)
{
	Worker worker;
	EXPECT_TRUE(TestHistogram::testContrastLimitedAdaptiveHistogramTileLookupTables(1280u, 720u, Scalar(40), 8u, 8u, GTEST_TEST_DURATION, worker));
}

TEST(TestHistogram, TestCLAHETileLookupTables_1919x1079_clipLimit_40_tiles_8x8)
{
	Worker worker;
	EXPECT_TRUE(TestHistogram::testContrastLimitedAdaptiveHistogramTileLookupTables(1919u, 1079u, Scalar(40), 8u, 8u, GTEST_TEST_DURATION, worker));
}

TEST(TestHistogram, TestCLAHETileLookupTables_1920x1080_clipLimit_40_tiles_8x8)
{
	Worker worker;
	EXPECT_TRUE(TestHistogram::testContrastLimitedAdaptiveHistogramTileLookupTables(1920u, 1080u, Scalar(40), 8u, 8u, GTEST_TEST_DURATION, worker));
}

TEST(TestHistogram, TestCLAHETileLookupTables_Random)
{
	Worker worker;
	EXPECT_TRUE(TestHistogram::testContrastLimitedAdaptiveHistogramTileLookupTables(0u, 0u, Scalar(0), 0u, 0u, GTEST_TEST_DURATION, worker));
}

// testContrastLimitedAdaptiveBilinearInterpolationParameters()

TEST(TestHistogram, TestCLAHEBilinearInterpolationParameters)
{
	EXPECT_TRUE(TestHistogram::testContrastLimitedAdaptiveBilinearInterpolationParameters(GTEST_TEST_DURATION));
}

// testContrastLimitedHistogramEqualization()

TEST(TestHistogram, TestCLAHEEqualization_127x127_clipLimit_40_tiles_8x8)
{
	Worker worker;
	EXPECT_TRUE(TestHistogram::testContrastLimitedHistogramEqualization(127u, 127u, Scalar(40), 8u, 8u, GTEST_TEST_DURATION, worker));
}

TEST(TestHistogram, TestCLAHEEqualization_128x128_clipLimit_40_tiles_8x8)
{
	Worker worker;
	EXPECT_TRUE(TestHistogram::testContrastLimitedHistogramEqualization(128u, 128u, Scalar(40), 8u, 8u, GTEST_TEST_DURATION, worker));
}

TEST(TestHistogram, TestCLAHEEqualization_255x255_clipLimit_40_tiles_8x8)
{
	Worker worker;
	EXPECT_TRUE(TestHistogram::testContrastLimitedHistogramEqualization(255u, 255u, Scalar(40), 8u, 8u, GTEST_TEST_DURATION, worker));
}

TEST(TestHistogram, TestCLAHEEqualization_256x256_clipLimit_40_tiles_8x8)
{
	Worker worker;
	EXPECT_TRUE(TestHistogram::testContrastLimitedHistogramEqualization(256u, 256u, Scalar(40), 8u, 8u, GTEST_TEST_DURATION, worker));
}

TEST(TestHistogram, TestCLAHEEqualization_511x511_clipLimit_40_tiles_8x8)
{
	Worker worker;
	EXPECT_TRUE(TestHistogram::testContrastLimitedHistogramEqualization(511u, 511u, Scalar(40), 8u, 8u, GTEST_TEST_DURATION, worker));
}

TEST(TestHistogram, TestCLAHEEqualization_512x512_clipLimit_40_tiles_8x8)
{
	Worker worker;
	EXPECT_TRUE(TestHistogram::testContrastLimitedHistogramEqualization(512u, 512u, Scalar(40), 8u, 8u, GTEST_TEST_DURATION, worker));
}

TEST(TestHistogram, TestCLAHEEqualization_639x479_clipLimit_40_tiles_8x8)
{
	Worker worker;
	EXPECT_TRUE(TestHistogram::testContrastLimitedHistogramEqualization(639u, 479u, Scalar(40), 8u, 8u, GTEST_TEST_DURATION, worker));
}

TEST(TestHistogram, TestCLAHEEqualization_640x480_clipLimit_40_tiles_8x8)
{
	Worker worker;
	EXPECT_TRUE(TestHistogram::testContrastLimitedHistogramEqualization(640u, 480u, Scalar(40), 8u, 8u, GTEST_TEST_DURATION, worker));
}

TEST(TestHistogram, TestCLAHEEqualization_1279x719_clipLimit_40_tiles_8x8)
{
	Worker worker;
	EXPECT_TRUE(TestHistogram::testContrastLimitedHistogramEqualization(1279u, 719u, Scalar(40), 8u, 8u, GTEST_TEST_DURATION, worker));
}

TEST(TestHistogram, TestCLAHEEqualization_1280x720_clipLimit_40_tiles_8x8)
{
	Worker worker;
	EXPECT_TRUE(TestHistogram::testContrastLimitedHistogramEqualization(1280u, 720u, Scalar(40), 8u, 8u, GTEST_TEST_DURATION, worker));
}

TEST(TestHistogram, TestCLAHEEqualization_1919x1079_clipLimit_40_tiles_8x8)
{
	Worker worker;
	EXPECT_TRUE(TestHistogram::testContrastLimitedHistogramEqualization(1919u, 1079u, Scalar(40), 8u, 8u, GTEST_TEST_DURATION, worker));
}

TEST(TestHistogram, TestCLAHEEqualization_1920x1080_clipLimit_40_tiles_8x8)
{
	Worker worker;
	EXPECT_TRUE(TestHistogram::testContrastLimitedHistogramEqualization(1920u, 1080u, Scalar(40), 8u, 8u, GTEST_TEST_DURATION, worker));
}

TEST(TestHistogram, TestCLAHEEqualization_Random)
{
	Worker worker;
	EXPECT_TRUE(TestHistogram::testContrastLimitedHistogramEqualization(0u, 0u, Scalar(0), 0u, 0u, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST


bool TestHistogram::testDetermineHistogram8BitPerChannel(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test: determine histograms (8-bit):";
	Log::info() << " ";

	RandomGenerator randomGenerator;

	bool validationSuccessful = true;

	const std::vector<std::pair<unsigned int, unsigned int>> sourceImageSizes =
	{
		{ 127u, 127u },
		{ 128u, 128u },
		{ 255u, 255u },
		{ 256u, 256u },
		{ 511u, 511u },
		{ 512u, 512u },
		{ 639u, 479u },
		{ 640u, 480u },
		{ 1279u, 719u },
		{ 1280u, 720u },
		{ 1919u, 1079u },
		{ 1920u, 1080u },
		{ 0u, 0u },
	};

	for (const std::pair<unsigned int, unsigned int>& sourceImageSize : sourceImageSizes)
	{
		validationSuccessful = testDetermineHistogram8BitPerChannel<1u>(testDuration, sourceImageSize.first, sourceImageSize.second, randomGenerator, worker) && validationSuccessful;
		validationSuccessful = testDetermineHistogram8BitPerChannel<2u>(testDuration, sourceImageSize.first, sourceImageSize.second, randomGenerator, worker) && validationSuccessful;
		validationSuccessful = testDetermineHistogram8BitPerChannel<3u>(testDuration, sourceImageSize.first, sourceImageSize.second, randomGenerator, worker) && validationSuccessful;
		validationSuccessful = testDetermineHistogram8BitPerChannel<4u>(testDuration, sourceImageSize.first, sourceImageSize.second, randomGenerator, worker) && validationSuccessful;
	}

	Log::info() << " ";
	Log::info() << "Validation of test to determine histograms (8-bit): " << (validationSuccessful ? "successful" : "failed");

	return validationSuccessful;
}

bool TestHistogram::testDetermineHistogram8BitPerChannelSubFrame(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test: determine histograms for sub-frames (8-bit):";
	Log::info() << " ";

	RandomGenerator randomGenerator;

	bool validationSuccessful = true;

	const std::vector<std::pair<unsigned int, unsigned int>> sourceImageSizes =
	{
		{ 127u, 127u },
		{ 128u, 128u },
		{ 255u, 255u },
		{ 256u, 256u },
		{ 511u, 511u },
		{ 512u, 512u },
		{ 639u, 479u },
		{ 640u, 480u },
		{ 1279u, 719u },
		{ 1280u, 720u },
		{ 1919u, 1079u },
		{ 1920u, 1080u },
		{ 0u, 0u },
	};

	for (const std::pair<unsigned int, unsigned int>& sourceImageSize : sourceImageSizes)
	{
		validationSuccessful = testDetermineHistogram8BitPerChannelSubFrame<1u>(testDuration, sourceImageSize.first, sourceImageSize.second, randomGenerator, worker) && validationSuccessful;
		validationSuccessful = testDetermineHistogram8BitPerChannelSubFrame<2u>(testDuration, sourceImageSize.first, sourceImageSize.second, randomGenerator, worker) && validationSuccessful;
		validationSuccessful = testDetermineHistogram8BitPerChannelSubFrame<3u>(testDuration, sourceImageSize.first, sourceImageSize.second, randomGenerator, worker) && validationSuccessful;
		validationSuccessful = testDetermineHistogram8BitPerChannelSubFrame<4u>(testDuration, sourceImageSize.first, sourceImageSize.second, randomGenerator, worker) && validationSuccessful;
	}

	Log::info() << " ";
	Log::info() << "Validation of test to determine histograms (8-bit): " << (validationSuccessful ? "successful" : "failed");

	return validationSuccessful;
}

bool TestHistogram::testContrastLimitedAdaptiveHistogramTileLookupTables(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test for CLAHE: computation of tile lookup tables";
	Log::info() << " ";

	bool validationSuccessful = true;

	const std::vector<std::pair<unsigned int, unsigned int>> sourceImageSizes =
	{
		{ 127u, 127u },
		{ 128u, 128u },
		{ 255u, 255u },
		{ 256u, 256u },
		{ 511u, 511u },
		{ 512u, 512u },
		{ 639u, 479u },
		{ 640u, 480u },
		{ 1279u, 719u },
		{ 1280u, 720u },
		{ 1919u, 1079u },
		{ 1920u, 1080u },
		{ 0u, 0u },
	};

	for (const std::pair<unsigned int, unsigned int>& sourceImageSize : sourceImageSizes)
	{
		const unsigned int width = sourceImageSize.first;
		const unsigned int height = sourceImageSize.second;
		const bool randomize = width == 0u || height == 0u;

		const double clipLimit = randomize ? 0.0 : 40.0;
		const unsigned int horizontalTiles = randomize ? 0u : 8u;
		const unsigned int verticalTiles = randomize ? 0u : 8u;

		validationSuccessful = testContrastLimitedAdaptiveHistogramTileLookupTables(sourceImageSize.first, sourceImageSize.second, Scalar(clipLimit), horizontalTiles, verticalTiles, testDuration, worker) && validationSuccessful;
	}

	Log::info() << " ";
	Log::info() << "Validation: " << (validationSuccessful ? "successful" : "failed");

	return validationSuccessful;
}

bool TestHistogram::testContrastLimitedAdaptiveBilinearInterpolationParameters(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test for CLAHE: computation of bilinear interpolation parameters";
	Log::info() << " ";

	RandomGenerator randomGenerator;

	bool validationSuccessful = true;

	const Timestamp startTimeValidation(true);

	do
	{
		// Random number of tiles
		const unsigned int horizontalTiles = RandomI::random(randomGenerator, 2u, 16u);
		const unsigned int verticalTiles = RandomI::random(randomGenerator, 2u, 16u);
		ocean_assert(horizontalTiles >= 2u && verticalTiles >= 2u);

		// Random image size; make it a multiple of the tile size ...
		const unsigned int sourceImageWidth_ = RandomI::random(randomGenerator, horizontalTiles, 2000u);
		const unsigned int sourceImageHeight_ = RandomI::random(randomGenerator, verticalTiles, 2000u);
		const unsigned int sourceImageWidth = sourceImageWidth_ - (sourceImageWidth_ % horizontalTiles);
		const unsigned int sourceImageHeight = sourceImageHeight_ - (sourceImageHeight_ % verticalTiles);
		ocean_assert(sourceImageWidth != 0u && sourceImageHeight != 0u);
		ocean_assert(sourceImageWidth % horizontalTiles == 0u && sourceImageHeight % verticalTiles == 0u);

		// Generate random test images, value range: [0, 255] as in the original
		Frame sourceImage = Frame(FrameType(sourceImageWidth, sourceImageHeight, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, 1u), FrameType::ORIGIN_UPPER_LEFT));
		CV::CVUtilities::randomizeFrame(sourceImage);

		const CV::ContrastLimitedAdaptiveHistogram::TileLookupCenter2 tileLookupCenter2(sourceImage.width(), sourceImage.height(), horizontalTiles, verticalTiles);

		Indices32 leftBins(sourceImageWidth);
		std::vector<unsigned char> leftFactors_fixed7(sourceImageWidth);
		CV::ContrastLimitedAdaptiveHistogram::computeLowBilinearInterpolationFactors7BitPrecision(tileLookupCenter2, /* isHorizontal */ true, leftBins.data(), leftFactors_fixed7.data());

		validationSuccessful = validateBilinearInterpolationParameters(leftBins, leftFactors_fixed7, sourceImage.width(), (unsigned int)tileLookupCenter2.binsX()) && validationSuccessful;

		Indices32 topBins(sourceImageHeight);
		std::vector<unsigned char> topFactors_fixed7(sourceImageHeight);
		CV::ContrastLimitedAdaptiveHistogram::computeLowBilinearInterpolationFactors7BitPrecision(tileLookupCenter2, /* isHorizontal */ false, topBins.data(), topFactors_fixed7.data());

		validationSuccessful = validateBilinearInterpolationParameters(topBins, topFactors_fixed7, sourceImage.height(), (unsigned int)tileLookupCenter2.binsY()) && validationSuccessful;

		ocean_assert(validationSuccessful);
	}
	while (startTimeValidation + testDuration > Timestamp(true));

	Log::info() << "Validation: " << (validationSuccessful ? "successful" : "failed");

	return validationSuccessful;
}

bool TestHistogram::validateBilinearInterpolationParameters(const Indices32& lowBins, const std::vector<unsigned char>& lowFactors_fixed7, const unsigned int imageEdgeLength, const unsigned int tilesCount)
{
	ocean_assert(imageEdgeLength % tilesCount == 0u && "Image size must be a multiple of number of tiles");
	ocean_assert(imageEdgeLength != 0u);
	ocean_assert(tilesCount >= 2u);

	Indices32 highBins(lowBins.size());
	std::vector<unsigned char> highFactors_fixed7(lowFactors_fixed7.size());

	for (unsigned int i = 0u; i < lowFactors_fixed7.size(); ++i)
	{
		highBins[i] = lowBins[i] + 1u;
		ocean_assert(highBins[i] < tilesCount);

		highFactors_fixed7[i] = 128u - lowFactors_fixed7[i];
	}

	bool validationSuccessful = true;

	const CV::ContrastLimitedAdaptiveHistogram::TileLookupCenter2 lookupCenter2(imageEdgeLength, imageEdgeLength, tilesCount, tilesCount);

	for (unsigned int x = 0u; x < imageEdgeLength; ++x)
	{
		const unsigned int lowBin = lowBins[x];
		const unsigned int highBin = highBins[x];

		if (lowBin + 1u != highBin)
		{
			validationSuccessful = false;
		}

		const size_t bX = lookupCenter2.binX(Scalar(x));

		const float bCenterX = (float)lookupCenter2.binCenterPositionX(bX);

		const unsigned int xLowBin = (unsigned int)((float(x) >= bCenterX) ? bX : max(0, int(bX) - 1));
		const unsigned int xHighBin = std::min(tilesCount - 1u, (unsigned int)(xLowBin + 1));

		const float leftCenter = (float)lookupCenter2.binCenterPositionX(xLowBin);
		const float rightCenter = (float)lookupCenter2.binCenterPositionX(xHighBin);

		float xFactor = -1.0f;
		if ((float)x < leftCenter)
		{
			xFactor = 1.0f;
		}
		else if ((float)x >= rightCenter)
		{
			xFactor = 0.0f;
		}
		else
		{
			ocean_assert(leftCenter < rightCenter);
			xFactor = (rightCenter - float(x)) / (rightCenter - leftCenter);
			ocean_assert(xFactor >= 0.0f && xFactor <= 1.0f);
		}

		const float validationLeftFactor = xFactor;
		ocean_assert(validationLeftFactor >= 0.0f && validationLeftFactor <= 1.0f);

		const unsigned int validationLeftFactor_fixed7 = (unsigned int)(128.0f * validationLeftFactor + 0.5f);
		const unsigned int validationRightFactor_fixed7 = 128u - validationLeftFactor_fixed7;

		const unsigned int leftFactor_fixed7 = lowFactors_fixed7[x];
		const unsigned int rightFactor_fixed7 = highFactors_fixed7[x];

		const unsigned int errorLeftFactors = (unsigned int)std::abs((int)leftFactor_fixed7 - (int)validationLeftFactor_fixed7);
		const unsigned int errorRightFactors = (unsigned int)std::abs((int)rightFactor_fixed7 - (int)validationRightFactor_fixed7);

		if (errorLeftFactors > 2u || errorRightFactors > 2u)
		{
			Log::info() << "Validation failed: errorLeftFactors = " << errorLeftFactors << ", errorRightFactors = " << errorRightFactors;
			validationSuccessful = false;
		}
	}

	return validationSuccessful;
}

unsigned int TestHistogram::validateTileLookupTables(const unsigned char* source, const unsigned int width, const unsigned int height, const std::vector<unsigned char>& validationLookupTables, const unsigned int horizontalTiles, const unsigned int verticalTiles, const Scalar clipLimit, double& maxAbsError, const unsigned int sourcePaddingElements, const double maxAbsErrorThresholdMultiple, const double maxAbsErrorThreshold)
{
	bool validationSuccessful = true;

	const Frame sourceFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), source, Frame::CM_USE_KEEP_LAYOUT, sourcePaddingElements);
	Frame centeredFrame;

	const bool isMultipleWidth = width % horizontalTiles == 0u;
	const bool isMultipleHeight = height % verticalTiles == 0u;

	const Scalar maxErrorThreshold = (isMultipleWidth && isMultipleHeight) ? Scalar(maxAbsErrorThresholdMultiple) : Scalar(maxAbsErrorThreshold);

	if (isMultipleWidth == true && isMultipleHeight == true)
	{
		centeredFrame = Frame(sourceFrame, Frame::ACM_USE_KEEP_LAYOUT);
	}
	else
	{
		// Need a continuous version of the source frame for CV::FrameEnlarger::addBorderMirror()
		// This mimicks cv::copyMakeBorder()
		const Frame continuousSourceFrame(sourceFrame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
		ocean_assert(continuousSourceFrame.isContinuous());

		const unsigned int tileWidth = width / horizontalTiles + (isMultipleWidth ? 0u : 1u);
		const unsigned int tileHeight = height / verticalTiles + (isMultipleHeight ? 0u : 1u);

		const unsigned int centeredFrameWidth = tileWidth * horizontalTiles;
		const unsigned int centeredFrameHeight = tileHeight * verticalTiles;
		ocean_assert(centeredFrameWidth >= width && centeredFrameHeight >= height);

		const unsigned int extensionX = centeredFrameWidth - width;
		const unsigned int extensionY = centeredFrameHeight - height;

		const unsigned int borderLeft = extensionX / 2u;
		ocean_assert(borderLeft <= extensionX);
		const unsigned int borderRight = extensionX - borderLeft;

		const unsigned int borderTop = extensionY / 2u;
		ocean_assert(borderTop <= extensionY);
		const unsigned int borderBottom = extensionY - borderTop;

		// Expand source frame by the largest of the four borders above (b/c CV::FrameEnlarger::addBorderMirror() takes exactly
		// one parameter). Will be corrected in the next step.
		const unsigned int largestBorder = std::max(std::max(borderLeft, borderRight), std::max(borderTop, borderBottom));

		Frame centeredFrame0;
		CV::FrameEnlarger::Comfort::addBorderMirrored(continuousSourceFrame, centeredFrame0, largestBorder, largestBorder, largestBorder, largestBorder);

		// Correct for the border that was added in the previous step
		ocean_assert(largestBorder >= borderLeft && largestBorder >= borderRight);
		const unsigned int offsetX = largestBorder - borderLeft;
		const unsigned int offsetY = largestBorder - borderTop;

		centeredFrame = centeredFrame0.subFrame(offsetX, offsetY, centeredFrameWidth, centeredFrameHeight, Frame::CM_COPY_REMOVE_PADDING_LAYOUT);
	}

	ocean_assert(centeredFrame.isValid());
	ocean_assert(centeredFrame.width() % horizontalTiles == 0u && centeredFrame.height() % verticalTiles == 0u);

	// From cv::CLAHE_CalcLut_Body<T,histSize,shift>::operator()
	const Frame& src_ = centeredFrame;
	Frame lut_(FrameType(CV::ContrastLimitedAdaptiveHistogram::histogramSize, horizontalTiles * verticalTiles, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	const unsigned int rangeStart = 0u;
	const unsigned int rangeEnd = horizontalTiles * verticalTiles;
	const int tilesX_ = (int)horizontalTiles;
	const int tilesY_ = (int)verticalTiles;

	typedef uint8_t T;
	ocean_assert(src_.width() % horizontalTiles == 0u);
	ocean_assert(src_.height() % verticalTiles == 0u);
	const int tileWidth_ = (int)src_.width() / tilesX_;
	const int tileHeight_ = (int)src_.height() / tilesY_;
	const int histSize = CV::ContrastLimitedAdaptiveHistogram::histogramSize;
	const unsigned int shift = 0u;

	ocean_assert(validationLookupTables.size() == horizontalTiles * verticalTiles * CV::ContrastLimitedAdaptiveHistogram::histogramSize);

	const int tileSizeTotal = tileWidth_ * tileHeight_;
	const float lutScale_ = float(histSize - 1) / float(tileSizeTotal);

	int clipLimit_ = 0;
	if (clipLimit > Scalar(0))
	{
		clipLimit_ = static_cast<int>(clipLimit * Scalar(tileSizeTotal) / Scalar(histSize));
		clipLimit_ = std::max(clipLimit_, 1);
	}

	ocean_assert(rangeStart >= 0u && rangeStart <= rangeEnd);
	T* tileLut = lut_.row<T>(rangeStart);
	const size_t lut_step = lut_.strideElements();

	maxAbsError = 0.0;
	unsigned int incorrectTileLUTs = 0u;

	for (int k = int(rangeStart); k < int(rangeEnd); ++k, tileLut += lut_step)
	{
		const int ty = k / tilesX_;
		const int tx = k % tilesX_;

		// retrieve tile submatrix
		const int tileROIx = tx * tileWidth_;
		const int tileROIy = ty * tileHeight_;
		const int tileROIWidth = tileWidth_;
		const int tileROIHeight = tileHeight_;

		const Frame tile = src_.subFrame(tileROIx, tileROIy, tileROIWidth, tileROIHeight);

		// calc histogram

		int tileHist[histSize] = { 0 };

		int tileHeight = tileROIHeight;
		const size_t sstep = src_.strideElements();
		for (const T* ptr = tile.constrow<T>(0u); tileHeight--; ptr += sstep)
		{
			int x = 0;
			for (; x <= tileROIWidth - 4; x += 4)
			{
				int t0 = ptr[x], t1 = ptr[x + 1];
				tileHist[t0 >> shift]++;
				tileHist[t1 >> shift]++;
				t0 = ptr[x + 2];
				t1 = ptr[x + 3];
				tileHist[t0 >> shift]++;
				tileHist[t1 >> shift]++;
			}

			for (; x < tileROIWidth; ++x)
				tileHist[ptr[x] >> shift]++;
		}

		// clip histogram
		if (clipLimit_ > 0)
		{
			// how many pixels were clipped
			int clipped = 0;
			for (int i = 0; i < histSize; ++i)
			{
				if (tileHist[i] > clipLimit_)
				{
					clipped += tileHist[i] - clipLimit_;
					tileHist[i] = clipLimit_;
				}
			}

			// redistribute clipped pixels
			int redistBatch = clipped / histSize;
			int residual = clipped - redistBatch * histSize;

			for (int i = 0; i < histSize; ++i)
				tileHist[i] += redistBatch;

			for (int i = 0; i < residual; ++i)
				tileHist[i]++;
		}

		// calc Lut
		bool currentValidationSuccessful = true;
		int sum = 0;
		for (int i = 0; i < histSize; ++i)
		{
			sum += tileHist[i];

			const int v = NumericF::round32(float(sum) * lutScale_);
			ocean_assert(v >= 0 && v <= NumericT<T>::maxValue());
			tileLut[i] = (T)((unsigned int)v <= (unsigned)NumericT<T>::maxValue() ? v : v > 0 ? NumericT<T>::maxValue() : 0); // = saturated_cast<T>()

			const double error = std::abs((double)tileLut[i] - (double)validationLookupTables[(ty * horizontalTiles + tx) * CV::ContrastLimitedAdaptiveHistogram::histogramSize + i]);
			maxAbsError = std::max(maxAbsError, error);

			if (error > maxErrorThreshold)
			{
				currentValidationSuccessful = false;
			}
		}

		incorrectTileLUTs += currentValidationSuccessful ? 0u : 1u;
		validationSuccessful = currentValidationSuccessful && validationSuccessful;
	}

#if defined(OCEAN_DEBUG_CLAHE_LOOKUPTABLES)
	if (validationSuccessful == false)
	{
		Log::info() << "Validation failed:";
		Log::info() << "  Max. error       : " << maxError;
		Log::info() << "  Image size       : " << width << " x " << height;
		Log::info() << "  Tiles            : " << horizontalTiles << " x " << verticalTiles;
		Log::info() << "  Center image size: " << src_.width() << " x " << src_.height();
		Log::info() << " ";
	}
#endif

	return incorrectTileLUTs;
}

bool TestHistogram::testContrastLimitedHistogramEqualization(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test for CLAHE: equalization";
	Log::info() << " ";

	bool validationSuccessful = true;

	const std::vector<std::pair<unsigned int, unsigned int>> sourceImageSizes =
	{
		{ 127u, 127u },
		{ 128u, 128u },
		{ 255u, 255u },
		{ 256u, 256u },
		{ 511u, 511u },
		{ 512u, 512u },
		{ 639u, 479u },
		{ 640u, 480u },
		{ 1279u, 719u },
		{ 1280u, 720u },
		{ 1919u, 1079u },
		{ 1920u, 1080u },
		{ 0u, 0u },
	};

	for (const std::pair<unsigned int, unsigned int>& sourceImageSize : sourceImageSizes)
	{
		const unsigned int width = sourceImageSize.first;
		const unsigned int height = sourceImageSize.second;
		const bool randomize = width == 0u || height == 0u;

		const double clipLimit = randomize ? 0.0 : 40.0;
		const unsigned int horizontalTiles = randomize ? 0u : 8u;
		const unsigned int verticalTiles = randomize ? 0u : 8u;

		validationSuccessful = testContrastLimitedHistogramEqualization(sourceImageSize.first, sourceImageSize.second, Scalar(clipLimit), horizontalTiles, verticalTiles, testDuration, worker) && validationSuccessful;
	}

	Log::info() << " ";
	Log::info() << "Validation: " << (validationSuccessful ? "successful" : "failed");

	return validationSuccessful;
}

bool TestHistogram::validateEqualization8BitPerChannel(const unsigned char* const source, const unsigned int width, const unsigned height, const unsigned char* const validationTarget, const Scalar clipLimit, const unsigned int horizontalTiles, const unsigned int verticalTiles, const unsigned int sourcePaddingElements, const unsigned int validationTargetPaddingElements, double* maxError, unsigned char* groundtruth)
{
	ocean_assert(source != nullptr && validationTarget != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(clipLimit > Scalar(0));
	ocean_assert(horizontalTiles != 0u && verticalTiles != 0u);

	const bool isMultipleWidth = width % horizontalTiles == 0u;
	const bool isMultipleHeight = height % verticalTiles == 0u;
	const Scalar maxErrorThreshold = isMultipleWidth && isMultipleHeight ? Scalar(1) : Scalar(5);

	// Partition the image into tiles
	const CV::ContrastLimitedAdaptiveHistogram::TileLookupCenter2 lookupCenter2(width, height, horizontalTiles, verticalTiles);

	// Compute the lookup tables in order to equalize the tile histograms
	const unsigned int tileCount = horizontalTiles * verticalTiles;
	std::vector<unsigned char> tileLookupTablesVec(tileCount * histogramSize, 0u);

	const unsigned int sourceWidth = width;
	const unsigned int sourceStrideElements = width + sourcePaddingElements;
	unsigned char* tileLookupTables = tileLookupTablesVec.data();

	for (unsigned int tileIndex = 0u; tileIndex < tileCount; ++tileIndex)
	{
		const int binY = tileIndex / horizontalTiles;
		const int binX = tileIndex % horizontalTiles;

		const unsigned int tileYStart = (unsigned int)lookupCenter2.binTopLeftY(binY);
		const unsigned int tileYEnd = (unsigned int)lookupCenter2.binBottomRightY(binY);
		const unsigned int tileHeight = tileYEnd - tileYStart + 1u;

		const unsigned int tileXStart = (unsigned int)lookupCenter2.binTopLeftX(binX);
		const unsigned int tileXEnd = (unsigned int)lookupCenter2.binBottomRightX(binX);
		const unsigned int tileWidth = tileXEnd - tileXStart + 1u;
		ocean_assert(tileWidth <= sourceWidth);
		const unsigned int tilePaddingElements = (sourceWidth - tileWidth) + sourcePaddingElements;

		const unsigned char* tile = source + tileYStart * sourceStrideElements + tileXStart;
		unsigned char* tileLookupTable = tileLookupTables + tileIndex * histogramSize;

		const unsigned int tileArea = tileWidth * tileHeight;
		const unsigned int tileStrideElements = tileWidth + tilePaddingElements;
		const unsigned char* const sourceEnd = source + height * sourceStrideElements;

		// Histogram computation
		CV::ContrastLimitedAdaptiveHistogram::TileHistogram histogram;
		memset(histogram.data(), 0u, histogram.size() * sizeof(CV::ContrastLimitedAdaptiveHistogram::TileHistogram::value_type));
		const unsigned int tileWidthEnd = tileWidth >= 4u ? tileWidth - 4u : 0u;

		for (unsigned int y = 0u; y < tileHeight; ++y)
		{
			unsigned int x = 0u;

			while (x < tileWidthEnd)
			{
				ocean_assert_and_suppress_unused(tile + x + 3u < sourceEnd, sourceEnd);
				histogram[tile[x + 0u]]++;
				histogram[tile[x + 1u]]++;
				histogram[tile[x + 2u]]++;
				histogram[tile[x + 3u]]++;

				x += 4u;
			}

			while (x < tileWidth)
			{
				ocean_assert(source + x < sourceEnd);
				histogram[tile[x]]++;

				++x;
			}

			tile += tileStrideElements;
		}

		// Clip histogram peaks and redistribute area exceeding the clip limit
		ocean_assert(histogramSize != 0u);
		const unsigned int scaledClipLimit = std::max(1u, (unsigned int)(clipLimit * float(tileArea) / float(histogramSize)));
		unsigned int clippedArea = 0u;

		for (unsigned int i = 0u; i < histogramSize; ++i)
		{
			if (histogram[i] > scaledClipLimit)
			{
				clippedArea += histogram[i] - scaledClipLimit;
				histogram[i] = scaledClipLimit;
			}
		}

		if (clippedArea != 0u)
		{
			const unsigned int redistribution = clippedArea / histogramSize;
			const unsigned int residual = clippedArea - (redistribution * histogramSize);

			for (unsigned int i = 0u; i < residual; ++i)
			{
				histogram[i] += (redistribution + 1u);
			}

			for (unsigned int i = residual; i < histogramSize; ++i)
			{
				histogram[i] += redistribution;
			}
		}

		// Normalize histogram (CDF + normalization)
		ocean_assert(tileArea != 0u);
		const float normalizationFactor = float(histogramSize - 1u) / float(tileArea);
		unsigned int sum = 0u;

		for (unsigned int i = 0u; i < histogramSize; ++i)
		{
			sum += histogram[i];

			ocean_assert((int)(float(sum) * normalizationFactor + 0.5f) >= 0);
			ocean_assert((int)(float(sum) * normalizationFactor + 0.5f) <= (int)(NumericT<unsigned char>::maxValue()));
			tileLookupTable[i] = (unsigned char)(float(sum) * normalizationFactor + 0.5f);
		}
	}

	// Pre-compute the paramaters that are required for the horizontal part of the bilinear interpolation
	Indices32 leftBins(width);
	Indices32 rightBins(width);
	std::vector<float> leftFactors(width);
	std::vector<float> rightFactors(width);

	for (unsigned int x = 0u; x < width; ++x)
	{
		const size_t bX = lookupCenter2.binX(Scalar(x));

		const float bCenterX = (float)lookupCenter2.binCenterPositionX(bX);

		const size_t xLowBin = (float(x) >= bCenterX) ? bX : max(0, int(bX) - 1);
		const size_t xHighBin = (float(x) < bCenterX) ? bX : min(bX + 1, lookupCenter2.binsX() - 1);
		ocean_assert(((xLowBin == 0 || xLowBin == lookupCenter2.binsX() - 1) && xHighBin == xLowBin) || xLowBin + 1 == xHighBin);

		leftBins[x] = (unsigned int)xLowBin;
		rightBins[x] = (unsigned int)xHighBin;

		const float leftCenter = (float)lookupCenter2.binCenterPositionX(xLowBin);
		const float rightCenter = (float)lookupCenter2.binCenterPositionX(xHighBin);

		float xFactor = -1.0f;
		if ((float)x < leftCenter)
		{
			xFactor = 1.0f;
		}
		else if ((float)x >= rightCenter)
		{
			xFactor = 0.0f;
		}
		else
		{
			ocean_assert(leftCenter < rightCenter);
			xFactor = (rightCenter - float(x)) / (rightCenter - leftCenter);
			ocean_assert(xFactor >= 0.0f && xFactor <= 1.0f);
		}

		leftFactors[x] = xFactor;
		ocean_assert(xFactor >= 0.0f && xFactor <= 1.0f);
		rightFactors[x] = 1.0f - xFactor;
	}

	// Apply the bilinear interpolation
	bool validationSuccessful = true;
	double maxError_ = 0.0;
	maxError = maxError == nullptr ? &maxError_ : maxError;
	*maxError = 0.0;

	const unsigned int groundtruthStrideElements = width;
	const unsigned int validationTargetStrideElements = width + validationTargetPaddingElements;

	const unsigned int rowStart = 0u;
	const unsigned int rowCount = height;
	const unsigned int rowEnd = rowStart + rowCount;
	ocean_assert(rowEnd <= lookupCenter2.sizeY());

	for (unsigned int y = rowStart; y < rowEnd; ++y)
	{
		const size_t bY = lookupCenter2.binY(Scalar(y));
		const float bCenterY = (float)lookupCenter2.binCenterPositionY(bY);
		const size_t yLowBin = (float(y) >= bCenterY) ? bY : max(0, int(bY) - 1);
		const size_t yHighBin = (float(y) < bCenterY) ? bY : min(yLowBin + 1, lookupCenter2.binsY() - 1);
		ocean_assert(((yLowBin == 0 || yLowBin == lookupCenter2.binsY() - 1) && yHighBin == yLowBin) || yLowBin + 1 == yHighBin);

		const float topCenter = (float)lookupCenter2.binCenterPositionY(yLowBin);
		const float bottomCenter = (float)lookupCenter2.binCenterPositionY(yHighBin);
		ocean_assert(topCenter <= bottomCenter);
		ocean_assert(yLowBin == yHighBin || (topCenter <= float(y) && Numeric::isNotEqual(topCenter, bottomCenter)));

		const float bottomFactor = yLowBin != yHighBin ? (float(y) - topCenter) / (bottomCenter - topCenter) : 1.0f;
		const float topFactor = 1.0f - bottomFactor;

		const unsigned int binsX = (unsigned int)lookupCenter2.binsX();

		const unsigned char* const sourceRow = source + y * sourceStrideElements;
		const unsigned char* const validationTargetRow = validationTarget + y * validationTargetStrideElements;
		unsigned char* const groundtruthRow = (groundtruth ? groundtruth + y * groundtruthStrideElements : nullptr);

		for (unsigned int x = 0u; x < width; ++x)
		{
			const unsigned char sourceValue = sourceRow[x];

			const Index32 xLowBin = leftBins[x];
			const Index32 xHighBin = rightBins[x];

			const unsigned char* const topLeft = tileLookupTables + (yLowBin * binsX + xLowBin) * histogramSize;
			const unsigned char* const topRight = tileLookupTables + (yLowBin * binsX + xHighBin) * histogramSize;
			const unsigned char* const bottomLeft = tileLookupTables + (yHighBin * binsX + xLowBin) * histogramSize;
			const unsigned char* const bottomRight = tileLookupTables + (yHighBin * binsX + xHighBin) * histogramSize;

			const float factorTopLeft = leftFactors[x] * topFactor;
			const float factorTopRight = rightFactors[x] * topFactor;
			const float factorBottomLeft = leftFactors[x] * bottomFactor;
			const float factorBottomRight = rightFactors[x] * bottomFactor;
			ocean_assert(NumericF::isEqual((factorTopLeft + factorTopRight + factorBottomLeft + factorBottomRight), 1.0f));

			const float targetValueF = float(topLeft[sourceValue]) * factorTopLeft + float(topRight[sourceValue]) * factorTopRight + float(bottomLeft[sourceValue]) * factorBottomLeft + float(bottomRight[sourceValue]) * factorBottomRight;
			ocean_assert(targetValueF >= 0.0f && targetValueF + 0.5f < 256.0f);
			const unsigned char targetValue = (unsigned char)(targetValueF + 0.5f);

			const double error = std::abs((double)targetValue - (double)validationTargetRow[x]);
			*maxError = std::max(*maxError, error);

			if (error > maxErrorThreshold)
			{
				validationSuccessful = false;
			}

			if (groundtruthRow)
			{
				groundtruthRow[x] = targetValue;
			}
		}
	}

	return validationSuccessful;
}

bool TestHistogram::testContrastLimitedAdaptiveHistogramTileLookupTables(const unsigned int width0, const unsigned int height0, const Scalar clipLimit0, const unsigned int horizontalTiles0, const unsigned int verticalTiles0, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;

	bool validationSuccessful = true;

	const double maxErrorThresholdMultiple = 1.0;
	const double maxErrorThreshold = 15.0;
	double maxError = 0.0;

	const unsigned int gaussianFilterSize = 11u;

	const bool useRandomWidth = width0 == 0u;
	const bool useRandomHeight = height0 == 0u;
	const bool useRandomClipLimit = clipLimit0 <= Scalar(0);
	const bool useRandomHorizontalTiles = horizontalTiles0 == 0u;
	const bool useRandomVerticalTiles = verticalTiles0 == 0u;
	const bool hasRandomParameters = useRandomWidth || useRandomHeight || useRandomClipLimit || useRandomHorizontalTiles || useRandomVerticalTiles;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	unsigned int totalIterations = 0u;
	unsigned int incorrectTileLUTs = 0u;
	unsigned int totalTileLUTs = 0u;

	for (unsigned int workerIteration = 0u; workerIteration < (hasRandomParameters ? 1u : 2u); ++workerIteration)
	{
		HighPerformanceStatistic& performance = (workerIteration == 0u ? performanceSinglecore : performanceMulticore);
		Worker* worker_ = (workerIteration == 0u ? nullptr : &worker);

		const Timestamp startTimePerformance(true);

		do
		{
			const double clipLimit = useRandomClipLimit ? Random::scalar(randomGenerator, 1.0, 100.0) : clipLimit0;
			const unsigned int horizontalTiles = useRandomHorizontalTiles ? RandomI::random(randomGenerator, 2u, 16u) : horizontalTiles0;
			const unsigned int verticalTiles = useRandomVerticalTiles ? RandomI::random(randomGenerator, 2u, 16u) : verticalTiles0;
			ocean_assert(clipLimit > 0.0 && horizontalTiles >= 2u && verticalTiles >= 2u);
			const unsigned int tileCount = horizontalTiles * verticalTiles;

			// Random image size; make it a multiple of the tile size and subtract a few pixels
			const unsigned int randomWidth = RandomI::random(randomGenerator, 16u, 128u) * horizontalTiles - RandomI::random(randomGenerator, 5u);
			const unsigned int randomHeight = RandomI::random(randomGenerator, 16u, 128u) * verticalTiles - RandomI::random(randomGenerator, 5u);

			const unsigned int sourceImageWidth = useRandomWidth ? randomWidth : width0;
			const unsigned int sourceImageHeight = useRandomHeight ? randomHeight : height0;
			ocean_assert(sourceImageWidth != 0u && sourceImageHeight != 0u);

			const unsigned int sourceImagePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			// Generate random test images, value range: [0, 255] as in the original
			const unsigned int randomFrameWidth = sourceImageWidth + gaussianFilterSize + sourceImagePaddingElements;
			const unsigned int randomFrameHeight = sourceImageHeight + gaussianFilterSize;

			Frame randomFrame(FrameType(randomFrameWidth, randomFrameHeight, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, 1u), FrameType::ORIGIN_UPPER_LEFT));
			CV::CVUtilities::randomizeFrame(randomFrame, false, &randomGenerator);

			Frame sourceImage(FrameType(sourceImageWidth, sourceImageHeight, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, 1u), FrameType::ORIGIN_UPPER_LEFT), sourceImagePaddingElements);
			CV::FrameFilterGaussian::filter<uint8_t, uint32_t>(randomFrame.constdata<uint8_t>(), sourceImage.data<uint8_t>(), sourceImage.width(), sourceImage.height(), 1u,  randomFrame.paddingElements(), sourceImage.paddingElements(), gaussianFilterSize, gaussianFilterSize, -1.0f);

			const CV::ContrastLimitedAdaptiveHistogram::TileLookupCenter2 tileLookupCenter2(sourceImage.width(), sourceImage.height(), horizontalTiles, verticalTiles);

			std::vector<unsigned char> tileLookupTables(tileCount * CV::ContrastLimitedAdaptiveHistogram::histogramSize);

			performance.start();
			CV::ContrastLimitedAdaptiveHistogram::computeTileLookupTables(sourceImage.constdata<uint8_t>(), tileLookupCenter2, tileLookupTables, Scalar(clipLimit), sourceImage.paddingElements(), worker_);
			performance.stop();

			double error = NumericD::maxValue();
			const unsigned int currentIncorrectTileLUTs = validateTileLookupTables(sourceImage.constdata<uint8_t>(), sourceImage.width(), sourceImage.height(), tileLookupTables, horizontalTiles, verticalTiles, Scalar(clipLimit), error, sourceImage.paddingElements(), maxErrorThresholdMultiple, maxErrorThreshold);

			incorrectTileLUTs += currentIncorrectTileLUTs;
			totalTileLUTs += horizontalTiles * verticalTiles;

			maxError = std::max(maxError, error);
			totalIterations++;
		}
		while (totalIterations < 4u || startTimePerformance + testDuration > Timestamp(true));
	}

	Log::info() << " ";

	if (hasRandomParameters == false)
	{
		Log::info() << "Image size: " << width0 << " x " << height0 << " px, clip limit: " << String::toAString(clipLimit0, 2u) << ", #tiles: " << horizontalTiles0 << " x " << verticalTiles0;
		Log::info() << "Single-core performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceSinglecore.medianMseconds(), 3u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multi-core Performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceMulticore.medianMseconds(), 3u) << "ms";
			Log::info() << "Multi-core boost factor: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x, median: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 1u) << "x";
		}
	}
	else
	{
		Log::info() << "Random parameters";
	}

	ocean_assert(totalIterations != 0u);
	const double ratioIncorrectTileLUTs = (double)incorrectTileLUTs / (double)totalTileLUTs;

	validationSuccessful = ratioIncorrectTileLUTs < 0.02;
	Log::info() << "Validation: " << (validationSuccessful ? "successful" : "failed") << ", max. absolute error: " << String::toAString(maxError, 2u) << ", incorrect tile LUTs ratio: " << String::toAString(ratioIncorrectTileLUTs, 4u) << ", incorrect tile LUTs total: " << incorrectTileLUTs;

	return validationSuccessful;
}

bool TestHistogram::testContrastLimitedHistogramEqualization(const unsigned int width0, const unsigned int height0, const Scalar clipLimit0, const unsigned int horizontalTiles0, const unsigned int verticalTiles0, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;

	bool validationSuccessful = true;

	double maxError = 0.0;

	const unsigned int gaussianFilterSize = 11u;

	const bool useRandomWidth = width0 == 0u;
	const bool useRandomHeight = height0 == 0u;
	const bool useRandomClipLimit = clipLimit0 <= Scalar(0);
	const bool useRandomHorizontalTiles = horizontalTiles0 == 0u;
	const bool useRandomVerticalTiles = verticalTiles0 == 0u;
	const bool hasRandomParameters = useRandomWidth || useRandomHeight || useRandomClipLimit || useRandomHorizontalTiles || useRandomVerticalTiles;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	for (unsigned int workerIteration = 0u; workerIteration < (hasRandomParameters ? 1u : 2u); ++workerIteration)
	{
		unsigned int iterations = 0u;

		HighPerformanceStatistic& performance = (workerIteration == 0u ? performanceSinglecore : performanceMulticore);
		Worker* worker_ = (workerIteration == 0u ? nullptr : &worker);

		const Timestamp startTimePerformance(true);

		do
		{
			const double clipLimit = useRandomClipLimit ? Random::scalar(randomGenerator, 1.0, 100.0) : clipLimit0;
			const unsigned int horizontalTiles = useRandomHorizontalTiles ? RandomI::random(randomGenerator, 2u, 16u) : horizontalTiles0;
			const unsigned int verticalTiles = useRandomVerticalTiles ? RandomI::random(randomGenerator, 2u, 16u) : verticalTiles0;
			ocean_assert(clipLimit > 0.0 && horizontalTiles >= 2u && verticalTiles >= 2u);

			// Random image size; make it a multiple of the tile size ...
			const unsigned int sourceImageWidth = useRandomWidth ? RandomI::random(randomGenerator, 16u * horizontalTiles, 2000u) : width0;
			const unsigned int sourceImageHeight = useRandomHeight ? RandomI::random(randomGenerator, 16u * verticalTiles, 2000u) : height0;
			ocean_assert(sourceImageWidth != 0u && sourceImageHeight != 0u);

			const unsigned int sourceImagePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			// Generate random test images, value range: [0, 255] as in the original
			const unsigned int randomFrameWidth = sourceImageWidth + gaussianFilterSize + sourceImagePaddingElements;
			const unsigned int randomFrameHeight = sourceImageHeight + gaussianFilterSize;

			Frame randomFrame(FrameType(randomFrameWidth, randomFrameHeight, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, 1u), FrameType::ORIGIN_UPPER_LEFT));
			CV::CVUtilities::randomizeFrame(randomFrame, false, &randomGenerator);

			Frame sourceImage(FrameType(sourceImageWidth, sourceImageHeight, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, 1u), FrameType::ORIGIN_UPPER_LEFT), sourceImagePaddingElements);
			CV::FrameFilterGaussian::filter<uint8_t, uint32_t>(randomFrame.constdata<uint8_t>(), sourceImage.data<uint8_t>(), sourceImage.width(), sourceImage.height(), 1u, randomFrame.paddingElements(), sourceImage.paddingElements(), gaussianFilterSize, gaussianFilterSize, -1.0f);

			const unsigned int targetImagePadding = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			Frame targetImage(sourceImage.frameType(), targetImagePadding);

			performance.start();
			CV::ContrastLimitedAdaptiveHistogram::equalization8BitPerChannel(sourceImage.constdata<uint8_t>(), sourceImage.width(), sourceImage.height(), targetImage.data<uint8_t>(), Scalar(clipLimit), horizontalTiles, verticalTiles, sourceImage.paddingElements(), targetImage.paddingElements(), worker_);
			performance.stop();

			double error = NumericD::maxValue();
			validationSuccessful = validateEqualization8BitPerChannel(sourceImage.constdata<uint8_t>(), sourceImage.width(), sourceImage.height(), targetImage.data<uint8_t>(), Scalar(clipLimit), horizontalTiles, verticalTiles, sourceImage.paddingElements(), targetImage.paddingElements(), &error, nullptr /* groundtruth */) && validationSuccessful;

			maxError = std::max(maxError, error);
		}
		while (iterations < 2u && startTimePerformance + testDuration > Timestamp(true));
	}

	Log::info() << " ";

	if (hasRandomParameters == false)
	{
		Log::info() << "Image size: " << width0 << " x " << height0 << " px, clip limit: " << String::toAString(clipLimit0, 2u) << ", #tiles: " << horizontalTiles0 << " x " << verticalTiles0;
		Log::info() << "Single-core performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceSinglecore.medianMseconds(), 3u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multi-core Performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceMulticore.medianMseconds(), 3u) << "ms";
			Log::info() << "Multi-core boost factor: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x, median: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 1u) << "x";
		}
	}
	else
	{
		Log::info() << "Random parameters";
	}

	Log::info() << "Validation: " << (validationSuccessful ? "successful" : "failed") << ", max. absolute error: " << String::toAString(maxError, 2u);

	return validationSuccessful;
}

template <unsigned int tChannels>
bool TestHistogram::testDetermineHistogram8BitPerChannel(const double testDuration, const unsigned int width, const unsigned int height, RandomGenerator& randomGenerator, Worker& worker)
{
	static_assert(tChannels != 0u && tChannels <= 4u, "The value of 'tChannels' must be in the range: [0, 4]");
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test: determine histograms (8-bit, " << (width != 0u ? String::toAString(width) : std::string("random width")) << " x " << (height != 0u ? String::toAString(height) : std::string("random height")) << ", channels: " << tChannels << "):";

	const unsigned int gaussianFilterSize = 11u;

	// Disable performance tests if one or more image dimensions are selected randomly.
	const unsigned int totalWorkerIterations = width == 0u || height == 0u ? 1u : 2u;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	bool validationSuccessful = true;

	for (unsigned int workerIteration = 0u; workerIteration < totalWorkerIterations; ++workerIteration)
	{
		HighPerformanceStatistic& performance = (workerIteration == 0u ? performanceSinglecore : performanceMulticore);
		Worker* useWorker = (workerIteration == 0u ? nullptr : &worker);

		const Timestamp startTimePerformance(true);

		do
		{
			const unsigned int sourceImageWidth = width != 0u ? width : RandomI::random(gaussianFilterSize, 2000u);
			const unsigned int sourceImageHeight = height != 0u ? height : RandomI::random(gaussianFilterSize, 2000u);
			ocean_assert(sourceImageWidth != 0u && sourceImageHeight != 0u);
			const unsigned int sourceImagePaddingElements = RandomI::random(0u, 50u);

			Frame randomFrame(FrameType(sourceImageWidth, sourceImageHeight, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, tChannels), FrameType::ORIGIN_UPPER_LEFT), sourceImagePaddingElements);
			CV::CVUtilities::randomizeFrame(randomFrame, false, &randomGenerator);

			performance.start();
			const CV::Histogram::Histogram8BitPerChannel<tChannels> histogram = CV::Histogram::determineHistogram8BitPerChannel<tChannels>(randomFrame.constdata<uint8_t>(), randomFrame.width(), randomFrame.height(), randomFrame.paddingElements(), useWorker);
			performance.stop();

			validationSuccessful = validateDetermineHistogram8BitPerChannel<tChannels>(randomFrame.constdata<uint8_t>(), randomFrame.width(), randomFrame.height(), randomFrame.paddingElements(), 0u, 0u, randomFrame.width(), randomFrame.height(), histogram) && validationSuccessful;
		}
		while (startTimePerformance + testDuration > Timestamp(true));
	}

	if (totalWorkerIterations == 2u)
	{
		Log::info() << "Single-core performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceSinglecore.medianMseconds(), 3u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multi-core Performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceMulticore.medianMseconds(), 3u) << "ms";
			Log::info() << "Multi-core boost factor: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x, median: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 1u) << "x";
		}
	}
	else
	{
		Log::info() << "Performance tests disabled because of random image size";
	}

	Log::info() << "Validation: " << (validationSuccessful ? "successful" : "failed");
	Log::info() << " ";

	return validationSuccessful;
}

template<unsigned int tChannels>
bool TestHistogram::testDetermineHistogram8BitPerChannelSubFrame(const double testDuration, const unsigned int width, const unsigned int height, RandomGenerator& randomGenerator, Worker& worker)
{
	static_assert(tChannels != 0u && tChannels <= 4u, "The value of 'tChannels' must be in the range: [0, 4]");
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test: determine histograms on sub-frames (8-bit, " << (width != 0u ? String::toAString(width) : std::string("random width")) << " x " << (height != 0u ? String::toAString(height) : std::string("random height")) << ", channels: " << tChannels << "):";

	const unsigned int gaussianFilterSize = 11u;

	// Disable performance tests if one or more image dimensions are selected randomly.
	const unsigned int totalWorkerIterations = width == 0u || height == 0u ? 1u : 2u;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	bool validationSuccessful = true;

	for (unsigned int workerIteration = 0u; workerIteration < totalWorkerIterations; ++workerIteration)
	{
		HighPerformanceStatistic& performance = (workerIteration == 0u ? performanceSinglecore : performanceMulticore);
		Worker* worker_ = (workerIteration == 0u ? nullptr : &worker);

		const Timestamp startTimePerformance(true);

		do
		{
			const unsigned int sourceImageWidth = width != 0u ? width : RandomI::random(gaussianFilterSize, 2000u);
			const unsigned int sourceImageHeight = height != 0u ? height : RandomI::random(gaussianFilterSize, 2000u);
			ocean_assert(sourceImageWidth != 0u && sourceImageHeight != 0u);
			const unsigned int sourceImagePaddingElements = RandomI::random(0u, 50u);

			Frame randomFrame(FrameType(sourceImageWidth, sourceImageHeight, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, tChannels), FrameType::ORIGIN_UPPER_LEFT), sourceImagePaddingElements);
			CV::CVUtilities::randomizeFrame(randomFrame, false, &randomGenerator);


			const unsigned int subFrameX = RandomI::random(0u, sourceImageWidth - 1u);
			const unsigned int subFrameY = RandomI::random(0u, sourceImageHeight - 1u);

			unsigned int subFrameWidth = RandomI::random(1u, sourceImageWidth);
			unsigned int subFrameHeight = RandomI::random(1u, sourceImageHeight);

			if (subFrameX + subFrameWidth > sourceImageWidth)
			{
				subFrameWidth = sourceImageWidth - subFrameX;
			}

			if (subFrameY + subFrameHeight > sourceImageHeight)
			{
				subFrameHeight = sourceImageHeight - subFrameY;
			}

			ocean_assert(subFrameX + subFrameWidth <= sourceImageWidth);
			ocean_assert(subFrameY + subFrameHeight <= sourceImageHeight);

			performance.start();
			const CV::Histogram::Histogram8BitPerChannel<tChannels> histogram = CV::Histogram::determineHistogram8BitPerChannel<tChannels>(randomFrame.constdata<uint8_t>(), randomFrame.width(), randomFrame.height(), subFrameX, subFrameY, subFrameWidth, subFrameHeight, randomFrame.paddingElements(), worker_);
			performance.stop();

			validationSuccessful = validateDetermineHistogram8BitPerChannel<tChannels>(randomFrame.constdata<uint8_t>(), randomFrame.width(), randomFrame.height(), randomFrame.paddingElements(), subFrameX, subFrameY, subFrameWidth, subFrameHeight, histogram) && validationSuccessful;
		}
		while (startTimePerformance + testDuration > Timestamp(true));
	}

	if (totalWorkerIterations == 2u)
	{
		Log::info() << "Single-core performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceSinglecore.medianMseconds(), 3u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multi-core Performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceMulticore.medianMseconds(), 3u) << "ms";
			Log::info() << "Multi-core boost factor: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x, median: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 1u) << "x";
		}
	}
	else
	{
		Log::info() << "Performance tests disabled because of random image size";
	}

	Log::info() << "Validation: " << (validationSuccessful ? "successful" : "failed");
	Log::info() << " ";

	return validationSuccessful;
}

template<unsigned int tChannels>
bool TestHistogram::validateDetermineHistogram8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int subFrameX, const unsigned int subFrameY, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const CV::Histogram::Histogram8BitPerChannel<tChannels>& histogramToValidate)
{
	ocean_assert(frame != nullptr && width != 0u && height != 0u);
	ocean_assert(subFrameX < width && subFrameWidth != 0u && subFrameX + subFrameWidth <= width);
	ocean_assert_and_suppress_unused(subFrameY < height && subFrameWidth != 0u && subFrameY + subFrameHeight <= height, height);

	unsigned int histogram[256u * tChannels] = { 0u };

	for (unsigned int y = subFrameY; y < subFrameY + subFrameHeight; ++y)
	{
		for (unsigned int x = subFrameX; x < subFrameX + subFrameWidth; ++x)
		{
			for (unsigned int channel = 0u; channel < tChannels; ++channel)
			{
				histogram[frame[(y * width + x) * tChannels + y * framePaddingElements + channel] + channel * 256]++;
			}
		}
	}

	for (unsigned int intensity = 0u; intensity < 256u; ++intensity)
	{
		for (unsigned int channel = 0u; channel < tChannels; ++channel)
		{
			if (histogram[intensity + channel * 256] != histogramToValidate.bin(channel, uint8_t(intensity)))
			{
				return false;
			}
		}
	}

	return true;
}

} // namespace TestCV

} // namespace Test

} // namespace Ocean
