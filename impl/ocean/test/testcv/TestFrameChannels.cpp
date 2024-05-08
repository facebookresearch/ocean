/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameChannels.h"
#include "ocean/test/testcv/TestFrameConverter.h"

#include "ocean/cv/CVUtilities.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameChannels::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Frame channels test:   ---";
	Log::info() << " ";

	allSucceeded = testSeparateTo1Channel(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testZipChannels(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAddFirstChannel(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAddFirstChannelValue(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAddLastChannel(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAddLastChannelValue(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRemoveFirstChannel(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRemoveLastChannel(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCopyChannel(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSetChannel(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testApplyAdvancedPixelModifier(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testApplyBivariateOperator<uint8_t, 3u>(5, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testApplyBivariateOperator<float, 3u>(5, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testTransformGeneric(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPremultipliedAlphaToStraightAlpha(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testStraightAlphaToPremultipliedAlpha(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testReverseChannelOrder(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRowPixelConversion3ChannelsTo1Channel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRowPixelConversion3ChannelsTo3Channels6BitPrecision(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRowPixelConversion3ChannelsTo3Channels7BitPrecision(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRowPixelConversion3ChannelsTo3Channels10BitPrecision(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRowPixelConversion4ChannelsTo1Channel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRowPixelConversion4ChannelsTo2Channels(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRowPixelConversion4ChannelsTo3Channels(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testReverseRowPixelOrder(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testReverseRowChannelOrder(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testShuffleRowChannels(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testShuffleRowChannelsAndSetLastChannelValue(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNarrowRow16BitPerChannels(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Frame channels test succeeded.";
	}
	else
	{
		Log::info() << "Frame channels test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameChannels, SeparateTo1Channel_1920x1080)
{
	EXPECT_TRUE(TestFrameChannels::testSeparateTo1Channel(1920u, 1080u, GTEST_TEST_DURATION));
}

TEST(TestFrameChannels, ZipChannels_1920x1080)
{
	EXPECT_TRUE(TestFrameChannels::testZipChannels(1920u, 1080u, GTEST_TEST_DURATION));
}

TEST(TestFrameChannels, AddFirstChannel_1920x1080)
{
	Worker worker;
	EXPECT_TRUE(TestFrameChannels::testAddFirstChannel(1920u, 1080u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameChannels, AddFirstChannelValue_1920x1080)
{
	Worker worker;
	EXPECT_TRUE(TestFrameChannels::testAddFirstChannelValue(1920u, 1080u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameChannels, AddLastChannel_1920x1080)
{
	Worker worker;
	EXPECT_TRUE(TestFrameChannels::testAddLastChannel(1920u, 1080u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameChannels, AddLastChannelValue_1920x1080)
{
	Worker worker;
	EXPECT_TRUE(TestFrameChannels::testAddLastChannelValue(1920u, 1080u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameChannels, RemoveFirstChannel_1920x1080)
{
	Worker worker;
	EXPECT_TRUE(TestFrameChannels::testRemoveFirstChannel(1920u, 1080u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameChannels, RemoveLastChannel_1920x1080)
{
	Worker worker;
	EXPECT_TRUE(TestFrameChannels::testRemoveLastChannel(1920u, 1080u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameChannels, CopyChannel_1920x1080)
{
	Worker worker;
	EXPECT_TRUE(TestFrameChannels::testCopyChannel(1920u, 1080u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameChannels, SetChannel_1920x1080)
{
	Worker worker;
	EXPECT_TRUE(TestFrameChannels::testSetChannel(1920u, 1080u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameChannels, ApplyAdvancedPixelModified_uint8_1channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testApplyAdvancedPixelModifier<uint8_t, 1u, TestFrameChannels::testFunctionApplyAdvancedModifier<uint8_t, uint8_t, 1u>>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ApplyAdvancedPixelModified_uint8_2channels)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testApplyAdvancedPixelModifier<uint8_t, 2u, TestFrameChannels::testFunctionApplyAdvancedModifier<uint8_t, uint8_t, 2u>>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ApplyAdvancedPixelModified_uint8_3channels)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testApplyAdvancedPixelModifier<uint8_t, 3u, TestFrameChannels::testFunctionApplyAdvancedModifier<uint8_t, uint8_t, 3u>>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ApplyAdvancedPixelModified_uint8_4channels)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testApplyAdvancedPixelModifier<uint8_t, 4u, TestFrameChannels::testFunctionApplyAdvancedModifier<uint8_t, uint8_t, 4u>>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameChannels, ApplyBivariateOperatorUint83Channels)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testApplyBivariateOperator<uint8_t, 3u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ApplyBivariateOperatorFloat3Channels)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testApplyBivariateOperator<float, 3u>(GTEST_TEST_DURATION, worker)));
}


// TransformGeneric

TEST(TestFrameChannels, TransformGeneric_uint8_1)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<uint8_t, 1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_uint8_2)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<uint8_t, 2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_uint8_3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<uint8_t, 3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_uint8_4)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<uint8_t, 4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_uint8_5)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<uint8_t, 5u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameChannels, TransformGeneric_int8_1)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<int8_t, 1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_int8_2)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<int8_t, 2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_int8_3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<int8_t, 3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_int8_4)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<int8_t, 4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_int8_5)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<int8_t, 5u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}



TEST(TestFrameChannels, TransformGeneric_uint16_1)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<uint16_t, 1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_uint16_2)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<uint16_t, 2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_uint16_3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<uint16_t, 3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_uint16_4)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<uint16_t, 4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_uint16_5)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<uint16_t, 5u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameChannels, TransformGeneric_int16_1)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<int16_t, 1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_int16_2)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<int16_t, 2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_int16_3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<int16_t, 3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_int16_4)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<int16_t, 4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_int16_5)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<int16_t, 5u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameChannels, TransformGeneric_uint32_1)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<uint32_t, 1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_uint32_2)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<uint32_t, 2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_uint32_3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<uint32_t, 3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_uint32_4)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<uint32_t, 4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_uint32_5)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<uint32_t, 5u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameChannels, TransformGeneric_int32_1)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<int32_t, 1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_int32_2)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<int32_t, 2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_int32_3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<int32_t, 3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_int32_4)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<int32_t, 4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_int32_5)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<int32_t, 5u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameChannels, TransformGeneric_float_1)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<float, 1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_float_2)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<float, 2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_float_3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<float, 3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_float_4)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<float, 4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_float_5)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<float, 5u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameChannels, TransformGeneric_double_1)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<double, 1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_double_2)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<double, 2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_double_3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<double, 3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_double_4)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<double, 4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, TransformGeneric_double_5)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testTransformGeneric<double, 5u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}


// PremultipliedAlphaToStraightAlpha

TEST(TestFrameChannels, PremultipliedAlphaToStraightAlpha)
{
	Worker worker;
	EXPECT_TRUE(TestFrameChannels::testPremultipliedAlphaToStraightAlpha(GTEST_TEST_DURATION, worker));
}


// PremultipliedAlphaToStraightAlpha

TEST(TestFrameChannels, StraightAlphaToPremultipliedAlpha)
{
	Worker worker;
	EXPECT_TRUE(TestFrameChannels::testStraightAlphaToPremultipliedAlpha(GTEST_TEST_DURATION, worker));
}


// ReverseChannelOrder

TEST(TestFrameChannels, ReverseChannelOrder_uint8_1)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<uint8_t, 1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_uint8_2)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<uint8_t, 2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_uint8_3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<uint8_t, 3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_uint8_4)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<uint8_t, 4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_uint8_5)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<uint8_t, 5u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameChannels, ReverseChannelOrder_int8_1)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<int8_t, 1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_int8_2)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<int8_t, 2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_int8_3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<int8_t, 3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_int8_4)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<int8_t, 4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_int8_5)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<int8_t, 5u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameChannels, ReverseChannelOrder_uint16_1)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<uint16_t, 1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_uint16_2)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<uint16_t, 2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_uint16_3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<uint16_t, 3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_uint16_4)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<uint16_t, 4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_uint16_5)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<int16_t, 5u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameChannels, ReverseChannelOrder_int16_1)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<int16_t, 1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_int16_2)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<int16_t, 2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_int16_3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<int16_t, 3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_int16_4)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<int16_t, 4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_int16_5)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<int16_t, 5u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameChannels, ReverseChannelOrder_uint32_1)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<uint32_t, 1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_uint32_2)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<uint32_t, 2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_uint32_3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<uint32_t, 3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_uint32_4)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<uint32_t, 4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_uint32_5)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<int32_t, 5u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameChannels, ReverseChannelOrder_int32_1)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<int32_t, 1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_int32_2)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<int32_t, 2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_int32_3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<int32_t, 3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_int32_4)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<int32_t, 4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_int32_5)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<int32_t, 5u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameChannels, ReverseChannelOrder_float_1)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<float, 1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_float_2)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<float, 2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_float_3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<float, 3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_float_4)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<float, 4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_float_5)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<float, 5u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameChannels, ReverseChannelOrder_double_1)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<double, 1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_double_2)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<double, 2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_double_3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<double, 3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_double_4)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<double, 4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameChannels, ReverseChannelOrder_double_5)
{
	Worker worker;
	EXPECT_TRUE((TestFrameChannels::testReverseChannelOrder<double, 5u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameChannels, RowPixelConversion3ChannelsTo1Channel)
{
	EXPECT_TRUE(TestFrameChannels::testRowPixelConversion3ChannelsTo1Channel(GTEST_TEST_DURATION));
}

TEST(TestFrameChannels, RowPixelConversion3ChannelsTo3Channels6BitPrecision)
{
	EXPECT_TRUE(TestFrameChannels::testRowPixelConversion3ChannelsTo3Channels6BitPrecision(GTEST_TEST_DURATION));
}

TEST(TestFrameChannels, RowPixelConversion3ChannelsTo3Channels7BitPrecision)
{
	EXPECT_TRUE(TestFrameChannels::testRowPixelConversion3ChannelsTo3Channels7BitPrecision(GTEST_TEST_DURATION));
}

TEST(TestFrameChannels, RowPixelConversion3ChannelsTo3Channels10BitPrecision)
{
	EXPECT_TRUE(TestFrameChannels::testRowPixelConversion3ChannelsTo3Channels10BitPrecision(GTEST_TEST_DURATION));
}

TEST(TestFrameChannels, RowPixelConversion4ChannelsTo1Channel)
{
	EXPECT_TRUE(TestFrameChannels::testRowPixelConversion4ChannelsTo1Channel(GTEST_TEST_DURATION));
}

TEST(TestFrameChannels, RowPixelConversion4ChannelsTo2Channels)
{
	EXPECT_TRUE(TestFrameChannels::testRowPixelConversion4ChannelsTo2Channels(GTEST_TEST_DURATION));
}

TEST(TestFrameChannels, RowPixelConversion4ChannelsTo3Channels)
{
	EXPECT_TRUE(TestFrameChannels::testRowPixelConversion4ChannelsTo3Channels(GTEST_TEST_DURATION));
}

TEST(TestFrameChannels, ReverseRowPixelOrder)
{
	EXPECT_TRUE(TestFrameChannels::testReverseRowPixelOrder(GTEST_TEST_DURATION));
}

TEST(TestFrameChannels, ReverseRowChannelOrder)
{
	EXPECT_TRUE(TestFrameChannels::testReverseRowChannelOrder(GTEST_TEST_DURATION));
}

TEST(TestFrameChannels, ShuffleRowChannels)
{
	EXPECT_TRUE(TestFrameChannels::testShuffleRowChannels(GTEST_TEST_DURATION));
}

TEST(TestFrameChannels, ShuffleRowChannelsAndSetLastChannelValue)
{
	EXPECT_TRUE(TestFrameChannels::testShuffleRowChannelsAndSetLastChannelValue(GTEST_TEST_DURATION));
}

TEST(TestFrameChannels, NarrowRow16BitPerChannels)
{
	EXPECT_TRUE(TestFrameChannels::testNarrowRow16BitPerChannels(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestFrameChannels::testSeparateTo1Channel(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing separate to 1 channel for " << width << "x" << height << " images:";
	Log::info() << " ";

	bool allSucceeded = true;

	for (unsigned int channels = 2u; channels <= 5u; ++channels)
	{
		allSucceeded = testSeparateTo1Channel<uint8_t, uint8_t>(width, height, channels, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int channels = 2u; channels <= 5u; ++channels)
	{
		allSucceeded = testSeparateTo1Channel<int16_t, int16_t>(width, height, channels, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int channels = 2u; channels <= 5u; ++channels)
	{
		allSucceeded = testSeparateTo1Channel<float, float>(width, height, channels, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int channels = 2u; channels <= 5u; ++channels)
	{
		allSucceeded = testSeparateTo1Channel<uint64_t, uint64_t>(width, height, channels, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int channels = 2u; channels <= 5u; ++channels)
	{
		allSucceeded = testSeparateTo1Channel<float, uint8_t>(width, height, channels, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int channels = 2u; channels <= 5u; ++channels)
	{
		allSucceeded = testSeparateTo1Channel<uint8_t, float>(width, height, channels, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Separate to 1 channel test succeeded.";
	}
	else
	{
		Log::info() << "Separate to 1 channel test FAILED!";
	}

	return allSucceeded;
}

bool TestFrameChannels::testZipChannels(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing zip channels for " << width << "x" << height << " images:";
	Log::info() << " ";

	bool allSucceeded = true;

	for (unsigned int channels = 2u; channels <= 5u; ++channels)
	{
		allSucceeded = testZipChannels<uint8_t, uint8_t>(width, height, channels, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int channels = 2u; channels <= 5u; ++channels)
	{
		allSucceeded = testZipChannels<int16_t, int16_t>(width, height, channels, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int channels = 2u; channels <= 5u; ++channels)
	{
		allSucceeded = testZipChannels<float, float>(width, height, channels, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int channels = 2u; channels <= 5u; ++channels)
	{
		allSucceeded = testZipChannels<uint64_t, uint64_t>(width, height, channels, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int channels = 2u; channels <= 5u; ++channels)
	{
		allSucceeded = testZipChannels<float, uint8_t>(width, height, channels, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int channels = 2u; channels <= 5u; ++channels)
	{
		allSucceeded = testZipChannels<uint8_t, float>(width, height, channels, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Zip channels test succeeded.";
	}
	else
	{
		Log::info() << "Zip channels test FAILED!";
	}

	return allSucceeded;
}

bool TestFrameChannels::testAddFirstChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing first channel add function for " << width << "x" << height << " image:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testAddFirstChannel<unsigned char, 1u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testAddFirstChannel<short, 1u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testAddFirstChannel<unsigned char, 2u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testAddFirstChannel<short, 2u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testAddFirstChannel<unsigned char, 3u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testAddFirstChannel<short, 3u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testAddFirstChannel<unsigned char, 4u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testAddFirstChannel<short, 4u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	return allSucceeded;
}

bool TestFrameChannels::testAddFirstChannelValue(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing add first channel value function for " << width << "x" << height << " image:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testAddFirstChannelValue<unsigned char, 1u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testAddFirstChannelValue<short, 1u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testAddFirstChannelValue<unsigned char, 2u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testAddFirstChannelValue<short, 2u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testAddFirstChannelValue<unsigned char, 3u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testAddFirstChannelValue<short, 3u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testAddFirstChannelValue<unsigned char, 4u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testAddFirstChannelValue<short, 4u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	return allSucceeded;
}

bool TestFrameChannels::testAddLastChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing last channel add function for " << width << "x" << height << " image:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testAddLastChannel<unsigned char, 1u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testAddLastChannel<short, 1u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testAddLastChannel<unsigned char, 2u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testAddLastChannel<short, 2u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testAddLastChannel<unsigned char, 3u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testAddLastChannel<short, 3u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testAddLastChannel<unsigned char, 4u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testAddLastChannel<short, 4u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	return allSucceeded;
}

bool TestFrameChannels::testAddLastChannelValue(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing add last channel value function for " << width << "x" << height << " image:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testAddLastChannelValue<unsigned char, 1u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testAddLastChannelValue<short, 1u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testAddLastChannelValue<unsigned char, 2u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testAddLastChannelValue<short, 2u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testAddLastChannelValue<unsigned char, 3u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testAddLastChannelValue<short, 3u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testAddLastChannelValue<unsigned char, 4u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testAddLastChannelValue<short, 4u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	return allSucceeded;
}

bool TestFrameChannels::testRemoveFirstChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing remove first channel function for " << width << "x" << height << " image:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testRemoveFirstChannel<unsigned char, 2u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testRemoveFirstChannel<short, 2u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testRemoveFirstChannel<unsigned char, 3u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testRemoveFirstChannel<short, 3u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testRemoveFirstChannel<unsigned char, 4u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testRemoveFirstChannel<short, 4u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testRemoveFirstChannel<unsigned char, 5u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testRemoveFirstChannel<short, 5u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	return allSucceeded;
}

bool TestFrameChannels::testRemoveLastChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing remove last channel function for " << width << "x" << height << " image:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testRemoveLastChannel<unsigned char, 2u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testRemoveLastChannel<short, 2u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testRemoveLastChannel<unsigned char, 3u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testRemoveLastChannel<short, 3u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testRemoveLastChannel<unsigned char, 4u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testRemoveLastChannel<short, 4u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testRemoveLastChannel<unsigned char, 5u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testRemoveLastChannel<short, 5u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	return allSucceeded;
}

bool TestFrameChannels::testCopyChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing copy channel function for " << width << "x" << height << " image:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testCopyChannel<unsigned char, 1u, 2u, 0u, 1u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testCopyChannel<short, 1u, 2u, 0u, 1u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testCopyChannel<unsigned char, 3u, 3u, 2u, 0u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testCopyChannel<short, 3u, 3u, 2u, 0u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testCopyChannel<unsigned char, 3u, 1u, 1u, 0u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testCopyChannel<short, 3u, 1u, 1u, 0u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testCopyChannel<unsigned char, 5u, 2u, 4u, 1u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testCopyChannel<short, 5u, 2u, 4u, 1u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	return allSucceeded;
}

bool TestFrameChannels::testSetChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing set channel function for " << width << "x" << height << " image:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testSetChannel<uint8_t, 0u, 1u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testSetChannel<int16_t, 0u, 1u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testSetChannel<uint8_t, 0u, 2u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testSetChannel<int16_t, 1u, 2u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testSetChannel<uint8_t, 0u, 3u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testSetChannel<int16_t, 2u, 3u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testSetChannel<uint8_t, 1u, 4u>(width, height, testDuration / 4.0, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testSetChannel<int16_t, 2u, 4u>(width, height, testDuration / 4.0, worker) && allSucceeded;

	return allSucceeded;
}

template <typename TSource, typename TTarget>
bool TestFrameChannels::testSeparateTo1Channel(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for data type " << TypeNamer::name<TSource>() << " -> " << TypeNamer::name<TTarget>() << ", with " << channels << " channels:";

	constexpr bool noComfortSupport = !std::is_same<TSource, TTarget>::value;

	bool allSucceeded = true;

	HighPerformanceStatistic performance;
	HighPerformanceStatistic performanceNaive;

	RandomGenerator randomGenerator;

	Timestamp startTimestamp(true);

	do
	{
		for (const bool performanceIteration : {true, false})
		{
			const unsigned int sourceChannels = channels;
			constexpr unsigned int targetChannels = 1u;

			const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

			const FrameType::PixelFormat sourcePixelFormat = FrameType::genericPixelFormat<TSource>(sourceChannels);
			const FrameType::PixelFormat targetPixelFormat = FrameType::genericPixelFormat<TTarget>(targetChannels);

			const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 1u, 1280u);
			const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 1u, 720u);

			const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 0u, 1u);

			Frame sourceFrame(FrameType(testWidth, testHeight, sourcePixelFormat, pixelOrigin), sourcePaddingElements);
			CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);

			Frames targetFrames(sourceChannels);
			Frames copyTargetFrames(sourceChannels);

			std::vector<TTarget*> targetPointers(sourceChannels);
			Indices32 targetsPaddingElements(sourceChannels);

			bool allTargetPaddingElementsZero = true;

			for (unsigned int c = 0u; c < sourceChannels; ++c)
			{
				const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 0u, 1u);

				targetFrames[c] = Frame(FrameType(sourceFrame, targetPixelFormat), targetPaddingElements);
				CV::CVUtilities::randomizeFrame(targetFrames[c], false, &randomGenerator);

				targetPointers[c] = targetFrames[c].data<TTarget>();
				targetsPaddingElements[c] = targetPaddingElements;

				if (targetPaddingElements != 0u)
				{
					allTargetPaddingElementsZero = false;
				}

				copyTargetFrames[c] = Frame(targetFrames[c], Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
			}

			if (allTargetPaddingElementsZero)
			{
				allTargetPaddingElementsZero = RandomI::random(randomGenerator, 0u, 1u) == 0u; // we also want to test this case
			}

			if (performanceIteration || noComfortSupport || RandomI::random(randomGenerator, 1u) == 0u)
			{
				if (RandomI::random(randomGenerator, 1u) == 0u)
				{
					CV::FrameChannels::separateTo1Channel<TSource, TTarget>(sourceFrame.constdata<TSource>(), targetPointers.data(), sourceFrame.width(), sourceFrame.height(), sourceFrame.channels(), sourcePaddingElements, allTargetPaddingElementsZero ? nullptr : targetsPaddingElements.data());
				}
				else
				{
					performance.startIf(performanceIteration);

					switch (sourceChannels)
					{
						case 1u:
							CV::FrameChannels::separateTo1Channel<TSource>(sourceFrame.constdata<TSource>(), {targetFrames[0].data<TTarget>()}, sourceFrame.width(), sourceFrame.height(), sourcePaddingElements, {targetsPaddingElements[0]});
							break;

						case 2u:
							CV::FrameChannels::separateTo1Channel<TSource>(sourceFrame.constdata<TSource>(), {targetFrames[0].data<TTarget>(), targetFrames[1].data<TTarget>()}, sourceFrame.width(), sourceFrame.height(), sourcePaddingElements, {targetsPaddingElements[0], targetsPaddingElements[1]});
							break;

						case 3u:
							CV::FrameChannels::separateTo1Channel<TSource>(sourceFrame.constdata<TSource>(), {targetFrames[0].data<TTarget>(), targetFrames[1].data<TTarget>(), targetFrames[2].data<TTarget>()}, sourceFrame.width(), sourceFrame.height(), sourcePaddingElements, {targetsPaddingElements[0], targetsPaddingElements[1], targetsPaddingElements[2]});
							break;

						case 4u:
							CV::FrameChannels::separateTo1Channel<TSource>(sourceFrame.constdata<TSource>(), {targetFrames[0].data<TTarget>(), targetFrames[1].data<TTarget>(), targetFrames[2].data<TTarget>(), targetFrames[3].data<TTarget>()}, sourceFrame.width(), sourceFrame.height(), sourcePaddingElements, {targetsPaddingElements[0], targetsPaddingElements[1], targetsPaddingElements[2], targetsPaddingElements[3]});
							break;

						case 5u:
							CV::FrameChannels::separateTo1Channel<TSource>(sourceFrame.constdata<TSource>(), {targetFrames[0].data<TTarget>(), targetFrames[1].data<TTarget>(), targetFrames[2].data<TTarget>(), targetFrames[3].data<TTarget>(), targetFrames[4].data<TTarget>()}, sourceFrame.width(), sourceFrame.height(), sourcePaddingElements, {targetsPaddingElements[0], targetsPaddingElements[1], targetsPaddingElements[2], targetsPaddingElements[3], targetsPaddingElements[4]});
							break;

						default:
							ocean_assert(false && "This should never happen!");
							allSucceeded = false;
							break;
					}

					performance.stopIf(performanceIteration);
				}
			}
			else
			{
				switch (sourceChannels)
				{
					case 1u:
						if (!CV::FrameChannels::Comfort::separateTo1Channel(sourceFrame, {&targetFrames[0]}))
						{
							allSucceeded = false;
						}
						break;

					case 2u:
						if (!CV::FrameChannels::Comfort::separateTo1Channel(sourceFrame, {&targetFrames[0], &targetFrames[1]}))
						{
							allSucceeded = false;
						}
						break;

					case 3u:
						if (!CV::FrameChannels::Comfort::separateTo1Channel(sourceFrame, {&targetFrames[0], &targetFrames[1], &targetFrames[2]}))
						{
							allSucceeded = false;
						}
						break;

					case 4u:
						if (!CV::FrameChannels::Comfort::separateTo1Channel(sourceFrame, {&targetFrames[0], &targetFrames[1], &targetFrames[2], &targetFrames[3]}))
						{
							allSucceeded = false;
						}
						break;

					case 5u:
						if (!CV::FrameChannels::Comfort::separateTo1Channel(sourceFrame, {&targetFrames[0], &targetFrames[1], &targetFrames[2], &targetFrames[3], &targetFrames[4]}))
						{
							allSucceeded = false;
						}
						break;

					default:
						ocean_assert(false && "This should never happen!");
						allSucceeded = false;
						break;
				}
			}

			for (size_t n = 0; n < targetFrames.size(); ++n)
			{
				if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrames[n], copyTargetFrames[n]))
				{
					ocean_assert(false && "Invalid padding memory!");
					allSucceeded = false;
					break;
				}
			}

			for (unsigned int c = 0u; c < sourceFrame.channels(); ++c)
			{
				const Frame& targetFrame = targetFrames[c];
				ocean_assert(targetFrame.channels() == 1u);

				for (unsigned int y = 0u; y < sourceFrame.height(); ++y)
				{
					for (unsigned int x = 0u; x < sourceFrame.width(); ++x)
					{
						if (targetFrame.constpixel<TTarget>(x, y)[0] != TTarget(sourceFrame.constpixel<TSource>(x, y)[c]))
						{
							allSucceeded = false;
						}
					}
				}
			}

			if (performanceIteration)
			{
				// testing a naive (but fair) implementation, in which the target frame is provided as one big memory block

				Frame targetFramesAsBlock(FrameType(targetFrames[0], sourceFrame.width(), sourceFrame.height() * sourceFrame.channels()));
				CV::CVUtilities::randomizeFrame(targetFramesAsBlock, false, &randomGenerator);

				const unsigned int pixels = sourceFrame.width() * sourceFrame.height();

				TTarget* const targetFrame = targetFramesAsBlock.data<TTarget>();

				const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performanceNaive);

				for (unsigned int y = 0u; y < sourceFrame.height(); ++y)
				{
					for (unsigned int x = 0u; x < sourceFrame.width(); ++x)
					{
						const unsigned int pixelIndex = y * sourceFrame.width() + x;

						const TSource* const sourcePixel = sourceFrame.constpixel<TSource>(x, y);

						for (unsigned int n = 0; n < sourceFrame.channels(); ++n)
						{
							targetFrame[pixels * n + pixelIndex] = TTarget(sourcePixel[n]);
						}
					}
				}

#ifdef OCEAN_DEBUG
				for (unsigned int c = 0u; c < sourceFrame.channels(); ++c)
				{
					for (unsigned int y = 0u; y < sourceFrame.height(); ++y)
					{
						for (unsigned int x = 0u; x < sourceFrame.width(); ++x)
						{
							if (targetFramesAsBlock.constpixel<TTarget>(x, y + c * sourceFrame.height())[0] != TTarget(sourceFrame.constpixel<TSource>(x, y)[c]))
							{
								allSucceeded = false;
							}
						}
					}
				}
#endif
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true) || performance.measurements() == 0u);

	Log::info() << "Naive: Best: " << String::toAString(performanceNaive.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceNaive.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceNaive.averageMseconds(), 3u) << "ms";
	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 3u) << "ms, average: " << String::toAString(performance.averageMseconds(), 3u) << "ms";

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

template <typename TSource, typename TTarget>
bool TestFrameChannels::testZipChannels(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for data type " << TypeNamer::name<TSource>() << " -> " << TypeNamer::name<TTarget>() << ", with " << channels << " channels:";

	constexpr bool noComfortSupport = !std::is_same<TSource, TTarget>::value;

	bool allSucceeded = true;

	HighPerformanceStatistic performance;
	HighPerformanceStatistic performanceNaive;

	RandomGenerator randomGenerator;

	Timestamp startTimestamp(true);

	do
	{
		for (const bool performanceIteration : {true, false})
		{
			constexpr unsigned int sourceChannels = 1u;
			const unsigned int targetChannels = channels;

			const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

			const FrameType::PixelFormat sourcePixelFormat = FrameType::genericPixelFormat<TSource>(sourceChannels);
			const FrameType::PixelFormat targetPixelFormat = FrameType::genericPixelFormat<TTarget>(targetChannels);

			const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 1u, 1280u);
			const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 1u, 720u);

			Frames sourceFrames;

			for (unsigned int n = 0u; n < targetChannels; ++n)
			{
				const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
				Frame sourceFrame(FrameType(testWidth, testHeight, sourcePixelFormat, pixelOrigin), paddingElements);

				CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);

				sourceFrames.emplace_back(std::move(sourceFrame));
			}

			Frame targetFrame;
			Frame copyTargetFrame;

			if (performanceIteration || noComfortSupport || RandomI::random(randomGenerator, 1u) == 0u)
			{
				const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

				targetFrame = Frame(FrameType(testWidth, testHeight, targetPixelFormat, pixelOrigin), paddingElements);

				CV::CVUtilities::randomizeFrame(targetFrame, false, &randomGenerator);

				copyTargetFrame = Frame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
			}

			const FrameType::PixelFormat targetPixelFormatToUse = RandomI::random(randomGenerator, 1u) == 0u ? targetPixelFormat : FrameType::FORMAT_UNDEFINED;

			for (const bool useInitializerList : {true, false})
			{
				if (noComfortSupport || useInitializerList)
				{
					switch (targetChannels)
					{
						case 1u:
						{
							if (performanceIteration || noComfortSupport || (RandomI::random(randomGenerator, 1u) == 0u && targetFrame.isValid()))
							{
								performance.startIf(performanceIteration);
									CV::FrameChannels::zipChannels<TSource, TTarget>({sourceFrames[0].constdata<TSource>()}, targetFrame.data<TTarget>(), targetFrame.width(), targetFrame.height(), {sourceFrames[0].paddingElements()}, targetFrame.paddingElements());
								performance.stopIf(performanceIteration);
							}
							else
							{
								if (!CV::FrameChannels::Comfort::zipChannels({sourceFrames[0]}, targetFrame, targetPixelFormatToUse))
								{
									allSucceeded = false;
								}
							}

							break;
						}

						case 2u:
						{
							if (performanceIteration || noComfortSupport || (RandomI::random(randomGenerator, 1u) == 0u && targetFrame.isValid()))
							{
								performance.startIf(performanceIteration);
									CV::FrameChannels::zipChannels<TSource, TTarget>({sourceFrames[0].constdata<TSource>(), sourceFrames[1].constdata<TSource>()}, targetFrame.data<TTarget>(), targetFrame.width(), targetFrame.height(), {sourceFrames[0].paddingElements(), sourceFrames[1].paddingElements()}, targetFrame.paddingElements());
								performance.stopIf(performanceIteration);
							}
							else
							{
								if (!CV::FrameChannels::Comfort::zipChannels({sourceFrames[0], sourceFrames[1]}, targetFrame, targetPixelFormatToUse))
								{
									allSucceeded = false;
								}
							}

							break;
						}

						case 3u:
						{
							if (performanceIteration || noComfortSupport || (RandomI::random(randomGenerator, 1u) == 0u && targetFrame.isValid()))
							{
								performance.startIf(performanceIteration);
									CV::FrameChannels::zipChannels<TSource, TTarget>({sourceFrames[0].constdata<TSource>(), sourceFrames[1].constdata<TSource>(), sourceFrames[2].constdata<TSource>()}, targetFrame.data<TTarget>(), targetFrame.width(), targetFrame.height(), {sourceFrames[0].paddingElements(), sourceFrames[1].paddingElements(), sourceFrames[2].paddingElements()}, targetFrame.paddingElements());
								performance.stopIf(performanceIteration);
							}
							else
							{
								if (!CV::FrameChannels::Comfort::zipChannels({sourceFrames[0], sourceFrames[1], sourceFrames[2]}, targetFrame, targetPixelFormatToUse))
								{
									allSucceeded = false;
								}
							}

							break;
						}

						case 4u:
						{
							if (performanceIteration || noComfortSupport || (RandomI::random(randomGenerator, 1u) == 0u && targetFrame.isValid()))
							{
								performance.startIf(performanceIteration);
									CV::FrameChannels::zipChannels<TSource, TTarget>({sourceFrames[0].constdata<TSource>(), sourceFrames[1].constdata<TSource>(), sourceFrames[2].constdata<TSource>(), sourceFrames[3].constdata<TSource>()}, targetFrame.data<TTarget>(), targetFrame.width(), targetFrame.height(), {sourceFrames[0].paddingElements(), sourceFrames[1].paddingElements(), sourceFrames[2].paddingElements(), sourceFrames[3].paddingElements()}, targetFrame.paddingElements());
								performance.stopIf(performanceIteration);
							}
							else
							{
								if (!CV::FrameChannels::Comfort::zipChannels({sourceFrames[0], sourceFrames[1], sourceFrames[2], sourceFrames[3]}, targetFrame, targetPixelFormatToUse))
								{
									allSucceeded = false;
								}
							}

							break;
						}

						case 5u:
						{
							if (performanceIteration || noComfortSupport || (RandomI::random(randomGenerator, 1u) == 0u && targetFrame.isValid()))
							{
								performance.startIf(performanceIteration);
									CV::FrameChannels::zipChannels<TSource, TTarget>({sourceFrames[0].constdata<TSource>(), sourceFrames[1].constdata<TSource>(), sourceFrames[2].constdata<TSource>(), sourceFrames[3].constdata<TSource>(), sourceFrames[4].constdata<TSource>()}, targetFrame.data<TTarget>(), targetFrame.width(), targetFrame.height(), {sourceFrames[0].paddingElements(), sourceFrames[1].paddingElements(), sourceFrames[2].paddingElements(), sourceFrames[3].paddingElements(), sourceFrames[4].paddingElements()}, targetFrame.paddingElements());
								performance.stopIf(performanceIteration);
							}
							else
							{
								if (!CV::FrameChannels::Comfort::zipChannels({sourceFrames[0], sourceFrames[1], sourceFrames[2], sourceFrames[3], sourceFrames[4]}, targetFrame, targetPixelFormatToUse))
								{
									allSucceeded = false;
								}
							}

							break;
						}

						default:
						{
							ocean_assert(false && "This should never happen!");

							allSucceeded = false;
							break;
						}
					}
				}
				else
				{
					if (!CV::FrameChannels::Comfort::zipChannels(sourceFrames, targetFrame, targetPixelFormatToUse))
					{
						allSucceeded = false;
					}
				}

				if (copyTargetFrame.isValid())
				{
					if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}
				}

				for (unsigned int c = 0u; c < targetFrame.channels(); ++c)
				{
					const Frame& sourceFrame = sourceFrames[c];
					ocean_assert(sourceFrame.channels() == 1u);

					for (unsigned int y = 0u; y < targetFrame.height(); ++y)
					{
						for (unsigned int x = 0u; x < targetFrame.width(); ++x)
						{
							if (TTarget(sourceFrame.constpixel<TSource>(x, y)[0]) != targetFrame.constpixel<TTarget>(x, y)[c])
							{
								allSucceeded = false;
							}
						}
					}
				}
			}

			if (performanceIteration)
			{
				// testing a naive (but fair) implementation, in which the source frame is provided as one big memory block

				Frame sourceFramesAsBlock(FrameType(sourceFrames[0], targetFrame.width(), targetFrame.height() * targetFrame.channels()));
				CV::CVUtilities::randomizeFrame(sourceFramesAsBlock, false, &randomGenerator);

				const unsigned int pixels = targetFrame.width() * targetFrame.height();

				const TSource* sourceFrame = sourceFramesAsBlock.constdata<TSource>();

				const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performanceNaive);

				for (unsigned int y = 0u; y < targetFrame.height(); ++y)
				{
					for (unsigned int x = 0u; x < targetFrame.width(); ++x)
					{
						const unsigned int pixelIndex = y * targetFrame.width() + x;

						TTarget* const targetPixel = targetFrame.pixel<TTarget>(x, y);

						for (unsigned int n = 0; n < targetFrame.channels(); ++n)
						{
							targetPixel[n] = TTarget(sourceFrame[pixels * n + pixelIndex]);
						}
					}
				}

#ifdef OCEAN_DEBUG
				for (unsigned int c = 0u; c < targetFrame.channels(); ++c)
				{
					for (unsigned int y = 0u; y < targetFrame.height(); ++y)
					{
						for (unsigned int x = 0u; x < targetFrame.width(); ++x)
						{
							if (TTarget(sourceFramesAsBlock.constpixel<TSource>(x, y + c * targetFrame.height())[0]) != targetFrame.constpixel<TTarget>(x, y)[c])
							{
								allSucceeded = false;
							}
						}
					}
				}
#endif
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true) || performance.measurements() == 0u);

	Log::info() << "Naive: Best: " << String::toAString(performanceNaive.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceNaive.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceNaive.averageMseconds(), 3u) << "ms";
	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 3u) << "ms, average: " << String::toAString(performance.averageMseconds(), 3u) << "ms";

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

template <typename T, unsigned int tSourceChannels>
bool TestFrameChannels::testAddFirstChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert(tSourceChannels != 0u, "Invalid channel number!");

	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	const FrameType::PixelFormat sourcePixelFormat = FrameType::genericPixelFormat<T, tSourceChannels>();
	const FrameType::PixelFormat sourceNewChannelPixelFormat = FrameType::genericPixelFormat<T, 1u>();
	const FrameType::PixelFormat targetPixelFormat = FrameType::genericPixelFormat<T, tSourceChannels + 1u>();

	const double testDuration_4 = testDuration * 0.25;

	const auto conversionFlags = CV::FrameConverter::conversionFlags();

	bool allSucceeded = true;

	for (const CV::FrameChannels::ConversionFlag& conversionFlag : conversionFlags)
	{
		Log::info() << "Testing " << tSourceChannels << " channels to " << tSourceChannels + 1u << " channels (" << TypeNamer::name<T>() << "), " << CV::FrameConverter::translateConversionFlag(conversionFlag) << ":";

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		const unsigned int maxWorkerIterations = worker ? 2u : 1u;

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			Timestamp startTimestamp(true);

			do
			{
				for (unsigned int benchmarkIteration = 0u; benchmarkIteration < 2u; ++benchmarkIteration)
				{
					const bool benchmark = benchmarkIteration == 0u;

					const unsigned int testWidth = benchmark ? width : RandomI::random(1u, 1920u);
					const unsigned int testHeight = benchmark ? height : RandomI::random(1u, 1080u);

					const unsigned int sourcePaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);
					const unsigned int sourceNewChannelPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);
					const unsigned int targetPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

					Frame sourceFrame(FrameType(testWidth, testHeight, sourcePixelFormat, FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
					Frame sourceNewChannelFrame(FrameType(sourceFrame, sourceNewChannelPixelFormat), sourceNewChannelPaddingElements);
					Frame targetFrame(FrameType(sourceFrame, targetPixelFormat), targetPaddingElements);

					CV::CVUtilities::randomizeFrame(sourceFrame, false);
					CV::CVUtilities::randomizeFrame(sourceNewChannelFrame, false);
					CV::CVUtilities::randomizeFrame(targetFrame, false);

					const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					if (benchmark)
					{
						performance.start();
					}

					CV::FrameChannels::addFirstChannel<T, tSourceChannels>(sourceFrame.constdata<T>(), sourceNewChannelFrame.constdata<T>(), targetFrame.data<T>(), sourceFrame.width(), sourceFrame.height(), conversionFlag, sourceFrame.paddingElements(), sourceNewChannelFrame.paddingElements(), targetFrame.paddingElements(), useWorker);

					if (benchmark)
					{
						performance.stop();
					}

					if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
					{
						ocean_assert(false && "Invalid padding memory!");

						allSucceeded = false;
						break;
					}

					if (!validateAddFirstChannel<T>(sourceFrame.constdata<T>(), sourceNewChannelFrame.constdata<T>(), targetFrame.constdata<T>(), tSourceChannels, sourceFrame.width(), sourceFrame.height(), conversionFlag, sourceFrame.paddingElements(), sourceNewChannelFrame.paddingElements(), targetFrame.paddingElements()))
					{
						allSucceeded = false;
					}
				}
			}
			while (startTimestamp + testDuration_4 > Timestamp(true));
		}

		Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
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

template <typename T, unsigned int tSourceChannels>
bool TestFrameChannels::testAddFirstChannelValue(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert(tSourceChannels != 0u, "Invalid channel number!");

	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	const FrameType::PixelFormat sourcePixelFormat = FrameType::genericPixelFormat<T, tSourceChannels>();
	const FrameType::PixelFormat targetPixelFormat = FrameType::genericPixelFormat<T, tSourceChannels + 1u>();

	const double testDuration_4 = testDuration * 0.25;

	const auto conversionFlags = CV::FrameConverter::conversionFlags();

	bool allSucceeded = true;

	for (const CV::FrameChannels::ConversionFlag& conversionFlag : conversionFlags)
	{
		Log::info() << "Testing " << tSourceChannels << " channels to " << tSourceChannels + 1u << " channels (" << TypeNamer::name<T>() << "), " << CV::FrameConverter::translateConversionFlag(conversionFlag) << ":";

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		const unsigned int maxWorkerIterations = worker ? 2u : 1u;

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			Timestamp startTimestamp(true);

			do
			{
				for (unsigned int benchmarkIteration = 0u; benchmarkIteration < 2u; ++benchmarkIteration)
				{
					const bool benchmark = benchmarkIteration == 0u;

					const unsigned int testWidth = benchmark ? width : RandomI::random(1u, 1920u);
					const unsigned int testHeight = benchmark ? height : RandomI::random(1u, 1080u);

					const unsigned int sourcePaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);
					const unsigned int targetPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

					Frame sourceFrame(FrameType(testWidth, testHeight, sourcePixelFormat, FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
					Frame targetFrame(FrameType(sourceFrame, targetPixelFormat), targetPaddingElements);

					CV::CVUtilities::randomizeFrame(sourceFrame, false);
					CV::CVUtilities::randomizeFrame(targetFrame, false);

					const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					const T newChannelValue = T(RandomI::random(-255, 255));

					if (benchmark)
					{
						performance.start();
					}

					CV::FrameChannels::addFirstChannelValue<T, tSourceChannels>(sourceFrame.constdata<T>(), newChannelValue, targetFrame.data<T>(), sourceFrame.width(), sourceFrame.height(), conversionFlag, sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);

					if (benchmark)
					{
						performance.stop();
					}

					if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
					{
						ocean_assert(false && "Invalid padding memory!");

						allSucceeded = false;
						break;
					}

					if (!validateAddFirstChannelValue<T>(sourceFrame.constdata<T>(), newChannelValue, targetFrame.constdata<T>(), tSourceChannels, sourceFrame.width(), sourceFrame.height(), conversionFlag, sourceFrame.paddingElements(), targetFrame.paddingElements()))
					{
						allSucceeded = false;
					}
				}
			}
			while (startTimestamp + testDuration_4 > Timestamp(true));
		}

		Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
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

template <typename T, unsigned int tSourceChannels>
bool TestFrameChannels::testAddLastChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert(tSourceChannels != 0u, "Invalid channel number!");

	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	const FrameType::PixelFormat sourcePixelFormat = FrameType::genericPixelFormat<T, tSourceChannels>();
	const FrameType::PixelFormat sourceNewChannelPixelFormat = FrameType::genericPixelFormat<T, 1u>();
	const FrameType::PixelFormat targetPixelFormat = FrameType::genericPixelFormat<T, tSourceChannels + 1u>();

	const double testDuration_4 = testDuration * 0.25;

	bool allSucceeded = true;

	for (const CV::FrameChannels::ConversionFlag& conversionFlag : CV::FrameConverter::conversionFlags())
	{
		Log::info() << "Testing " << tSourceChannels << " channels to " << tSourceChannels + 1u << " channels (" << TypeNamer::name<T>() << "), " << CV::FrameConverter::translateConversionFlag(conversionFlag) << ":";

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		const unsigned int maxWorkerIterations = worker ? 2u : 1u;

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			Timestamp startTimestamp(true);

			do
			{
				for (unsigned int benchmarkIteration = 0u; benchmarkIteration < 2u; ++benchmarkIteration)
				{
					const bool benchmark = benchmarkIteration == 0u;

					const unsigned int testWidth = benchmark ? width : RandomI::random(1u, 1920u);
					const unsigned int testHeight = benchmark ? height : RandomI::random(1u, 1080u);

					const unsigned int sourcePaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);
					const unsigned int sourceNewChannelPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);
					const unsigned int targetPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

					Frame sourceFrame(FrameType(testWidth, testHeight, sourcePixelFormat, FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
					Frame sourceNewChannelFrame(FrameType(sourceFrame, sourceNewChannelPixelFormat), sourceNewChannelPaddingElements);
					Frame targetFrame(FrameType(sourceFrame, targetPixelFormat), targetPaddingElements);

					CV::CVUtilities::randomizeFrame(sourceFrame, false);
					CV::CVUtilities::randomizeFrame(sourceNewChannelFrame, false);
					CV::CVUtilities::randomizeFrame(targetFrame, false);

					const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					if (benchmark)
					{
						performance.start();
					}

					CV::FrameChannels::addLastChannel<T, tSourceChannels>(sourceFrame.constdata<T>(), sourceNewChannelFrame.constdata<T>(), targetFrame.data<T>(), sourceFrame.width(), sourceFrame.height(), conversionFlag, sourceFrame.paddingElements(), sourceNewChannelFrame.paddingElements(), targetFrame.paddingElements(), useWorker);

					if (benchmark)
					{
						performance.stop();
					}

					if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
					{
						ocean_assert(false && "Invalid padding memory!");

						allSucceeded = false;
						break;
					}

					if (!validateAddLastChannel<T>(sourceFrame.constdata<T>(), sourceNewChannelFrame.constdata<T>(), targetFrame.constdata<T>(), tSourceChannels, sourceFrame.width(), sourceFrame.height(), conversionFlag, sourceFrame.paddingElements(), sourceNewChannelFrame.paddingElements(), targetFrame.paddingElements()))
					{
						allSucceeded = false;
					}
				}
			}
			while (startTimestamp + testDuration_4 > Timestamp(true));
		}

		Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
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

template <typename T, unsigned int tSourceChannels>
bool TestFrameChannels::testAddLastChannelValue(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert(tSourceChannels != 0u, "Invalid channel number!");

	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	const FrameType::PixelFormat sourcePixelFormat = FrameType::genericPixelFormat<T, tSourceChannels>();
	const FrameType::PixelFormat targetPixelFormat = FrameType::genericPixelFormat<T, tSourceChannels + 1u>();

	const double testDuration_4 = testDuration * 0.25;

	bool allSucceeded = true;

	for (const CV::FrameChannels::ConversionFlag conversionFlag : CV::FrameConverter::conversionFlags())
	{
		Log::info() << "Testing " << tSourceChannels << " channels to " << tSourceChannels + 1u << " channels (" << TypeNamer::name<T>() << "), " << CV::FrameConverter::translateConversionFlag(conversionFlag) << ":";

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		const unsigned int maxWorkerIterations = worker ? 2u : 1u;

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			Timestamp startTimestamp(true);

			do
			{
				for (unsigned int benchmarkIteration = 0u; benchmarkIteration < 2u; ++benchmarkIteration)
				{
					const bool benchmark = benchmarkIteration == 0u;

					const unsigned int testWidth = benchmark ? width : RandomI::random(1u, 1920u);
					const unsigned int testHeight = benchmark ? height : RandomI::random(1u, 1080u);

					const unsigned int sourcePaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);
					const unsigned int targetPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

					Frame sourceFrame(FrameType(testWidth, testHeight, sourcePixelFormat, FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
					Frame targetFrame(FrameType(sourceFrame, targetPixelFormat), targetPaddingElements);

					CV::CVUtilities::randomizeFrame(sourceFrame, false);
					CV::CVUtilities::randomizeFrame(targetFrame, false);

					const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					const T newChannelValue = T(RandomI::random(-255, 255));

					if (benchmark)
					{
						performance.start();
					}

					CV::FrameChannels::addLastChannelValue<T, tSourceChannels>(sourceFrame.constdata<T>(), newChannelValue, targetFrame.data<T>(), sourceFrame.width(), sourceFrame.height(), conversionFlag, sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);

					if (benchmark)
					{
						performance.stop();
					}

					if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
					{
						ocean_assert(false && "Invalid padding memory!");

						allSucceeded = false;
						break;
					}

					if (!validateAddLastChannelValue<T>(sourceFrame.constdata<T>(), newChannelValue, targetFrame.constdata<T>(), tSourceChannels, sourceFrame.width(), sourceFrame.height(), conversionFlag, sourceFrame.paddingElements(), targetFrame.paddingElements()))
					{
						allSucceeded = false;
					}
				}
			}
			while (startTimestamp + testDuration_4 > Timestamp(true));
		}

		Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
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

template <typename T, unsigned int tSourceChannels>
bool TestFrameChannels::testRemoveFirstChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert(tSourceChannels >= 2u, "Invalid channel number!");

	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	const FrameType::PixelFormat sourcePixelFormat = FrameType::genericPixelFormat<T, tSourceChannels>();
	const FrameType::PixelFormat targetPixelFormat = FrameType::genericPixelFormat<T, tSourceChannels - 1u>();

	const double testDuration_4 = testDuration * 0.25;

	bool allSucceeded = true;

	for (const CV::FrameChannels::ConversionFlag conversionFlag : CV::FrameConverter::conversionFlags())
	{
		Log::info() << "Testing " << tSourceChannels << " channels to " << tSourceChannels - 1u << " channels (" << TypeNamer::name<T>() << "), " << CV::FrameConverter::translateConversionFlag(conversionFlag) << ":";

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		const unsigned int maxWorkerIterations = worker ? 2u : 1u;

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			Timestamp startTimestamp(true);

			do
			{
				for (unsigned int nBenchmark = 0u; nBenchmark < 2u; ++nBenchmark)
				{
					const bool benchmarkIteration = nBenchmark == 0u;

					const unsigned int testWidth = benchmarkIteration ? width : RandomI::random(1u, width);
					const unsigned int testHeight = benchmarkIteration ? height : RandomI::random(1u, height);

					const unsigned int sourcePaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);
					const unsigned int targetPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

					Frame source(FrameType(testWidth, testHeight, sourcePixelFormat, FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
					Frame target(FrameType(source, targetPixelFormat), targetPaddingElements);

					CV::CVUtilities::randomizeFrame(source, false);
					CV::CVUtilities::randomizeFrame(target, false);

					const Frame copyTarget(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					if (benchmarkIteration)
					{
						performance.start();
					}

					CV::FrameChannels::removeFirstChannel<T, tSourceChannels>(source.constdata<T>(), target.data<T>(), source.width(), source.height(), conversionFlag, source.paddingElements(), target.paddingElements(), useWorker);

					if (benchmarkIteration)
					{
						performance.stop();
					}

					if (!CV::CVUtilities::isPaddingMemoryIdentical(target, copyTarget))
					{
						ocean_assert(false && "Invalid padding memory!");

						allSucceeded = false;
						break;
					}

					if (!validateRemoveFirstChannel<T>(source.constdata<T>(), target.constdata<T>(), source.channels(), source.width(), source.height(), conversionFlag, source.paddingElements(), target.paddingElements()))
					{
						allSucceeded = false;
					}
				}
			}
			while (startTimestamp + testDuration_4 > Timestamp(true));
		}

		Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
		}

		Log::info() << " ";
	}

	Log::info() << " ";

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

template <typename T, unsigned int tSourceChannels>
bool TestFrameChannels::testRemoveLastChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert(tSourceChannels >= 2u, "Invalid channel number!");

	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	const FrameType::PixelFormat sourcePixelFormat = FrameType::genericPixelFormat<T, tSourceChannels>();
	const FrameType::PixelFormat targetPixelFormat = FrameType::genericPixelFormat<T, tSourceChannels - 1u>();

	const double testDuration_4 = testDuration * 0.25;

	bool allSucceeded = true;

	for (const CV::FrameChannels::ConversionFlag conversionFlag : CV::FrameConverter::conversionFlags())
	{
		Log::info() << "Testing " << tSourceChannels << " channels to " << tSourceChannels - 1u << " channels (" << TypeNamer::name<T>() << "), " << CV::FrameConverter::translateConversionFlag(conversionFlag) << ":";

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		const unsigned int maxWorkerIterations = worker ? 2u : 1u;

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			Timestamp startTimestamp(true);

			do
			{
				for (unsigned int nBenchmark = 0u; nBenchmark < 2u; ++nBenchmark)
				{
					const bool benchmarkIteration = nBenchmark == 0u;

					const unsigned int testWidth = benchmarkIteration ? width : RandomI::random(1u, width);
					const unsigned int testHeight = benchmarkIteration ? height : RandomI::random(1u, height);

					const unsigned int sourcePaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);
					const unsigned int targetPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

					Frame source(FrameType(testWidth, testHeight, sourcePixelFormat, FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
					Frame target(FrameType(source, targetPixelFormat), targetPaddingElements);

					CV::CVUtilities::randomizeFrame(source, false);
					CV::CVUtilities::randomizeFrame(target, false);

					const Frame copyTarget(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					if (benchmarkIteration)
					{
						performance.start();
					}

					CV::FrameChannels::removeLastChannel<T, tSourceChannels>(source.constdata<T>(), target.data<T>(), source.width(), source.height(), conversionFlag, source.paddingElements(), target.paddingElements(), useWorker);

					if (benchmarkIteration)
					{
						performance.stop();
					}

					if (!CV::CVUtilities::isPaddingMemoryIdentical(target, copyTarget))
					{
						ocean_assert(false && "Invalid padding memory!");

						allSucceeded = false;
						break;
					}

					if (!validateRemoveLastChannel<T>(source.constdata<T>(), target.constdata<T>(), source.channels(), source.width(), source.height(), conversionFlag, source.paddingElements(), target.paddingElements()))
					{
						allSucceeded = false;
					}
				}
			}
			while (startTimestamp + testDuration_4 > Timestamp(true));
		}

		Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
		}

		Log::info() << " ";
	}

	Log::info() << " ";

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

template <typename T, unsigned int tSourceChannels, unsigned int tTargetChannels, unsigned int tSourceChannelIndex, unsigned int tTargetChannelIndex>
bool TestFrameChannels::testCopyChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert(tSourceChannels >= 1u, "Invalid channel number!");
	static_assert(tTargetChannels >= 1u, "Invalid channel number!");

	static_assert(tSourceChannelIndex < tSourceChannels, "Invalid channel number!");
	static_assert(tTargetChannelIndex < tTargetChannels, "Invalid channel number!");

	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	const FrameType::PixelFormat sourcePixelFormat = FrameType::genericPixelFormat<T, tSourceChannels>();
	const FrameType::PixelFormat targetPixelFormat = FrameType::genericPixelFormat<T, tTargetChannels>();

	bool allSucceeded = true;

	Log::info() << "Testing " << tSourceChannelIndex << " of " << tSourceChannels << " channels to " << tSourceChannelIndex << " of " << tSourceChannels << " channels (" << TypeNamer::name<T>() << "):";

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		Timestamp startTimestamp(true);

		do
		{
			for (unsigned int nBenchmark = 0u; nBenchmark < 2u; ++nBenchmark)
			{
				const bool benchmarkIteration = nBenchmark == 0u;

				const unsigned int testWidth = benchmarkIteration ? width : RandomI::random(1u, width);
				const unsigned int testHeight = benchmarkIteration ? height : RandomI::random(1u, height);

				const unsigned int sourcePaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);
				const unsigned int targetPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

				Frame source(FrameType(testWidth, testHeight, sourcePixelFormat, FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
				Frame target(FrameType(source, targetPixelFormat), targetPaddingElements);

				CV::CVUtilities::randomizeFrame(source, false);
				CV::CVUtilities::randomizeFrame(target, false);

				const Frame copyTarget(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				if (benchmarkIteration)
				{
					performance.start();
				}

				CV::FrameChannels::copyChannel<T, tSourceChannels, tTargetChannels, tSourceChannelIndex, tTargetChannelIndex>(source.constdata<T>(), target.data<T>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), useWorker);

				if (benchmarkIteration)
				{
					performance.stop();
				}

				if (!CV::CVUtilities::isPaddingMemoryIdentical(target, copyTarget))
				{
					ocean_assert(false && "Invalid padding memory!");

					allSucceeded = false;
					break;
				}

				if (!validateCopyChannel<T>(source.constdata<T>(), copyTarget.constdata<T>(), target.constdata<T>(), source.channels(), target.channels(), tSourceChannelIndex, tTargetChannelIndex, source.width(), source.height(), source.paddingElements(), target.paddingElements()))
				{
					allSucceeded = false;
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms";
		Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
	}

	Log::info() << " ";

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

template <typename T, unsigned int tChannelIndex, unsigned int tChannels>
bool TestFrameChannels::testSetChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tChannelIndex < tChannels, "Invalid channel number!");

	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<T, tChannels>();

	bool allSucceeded = true;

	Log::info() << "Testing " << tChannelIndex << " of " << tChannels << " channels (" << TypeNamer::name<T>() << "):";

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		Timestamp startTimestamp(true);

		do
		{
			for (unsigned int nBenchmark = 0u; nBenchmark < 2u; ++nBenchmark)
			{
				const bool benchmarkIteration = nBenchmark == 0u;

				const unsigned int testWidth = benchmarkIteration ? width : RandomI::random(1u, width);
				const unsigned int testHeight = benchmarkIteration ? height : RandomI::random(1u, height);

				const unsigned int framePaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

				Frame frame(FrameType(testWidth, testHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);
				CV::CVUtilities::randomizeFrame(frame, false);

				const Frame copyFrame(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				const T value = T(RandomI::random32());

				if (benchmarkIteration)
				{
					performance.start();
				}

				CV::FrameChannels::setChannel<T, tChannelIndex, tChannels>(frame.data<T>(), frame.width(), frame.height(), value, frame.paddingElements(), useWorker);

				if (benchmarkIteration)
				{
					performance.stop();
				}

				if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, copyFrame))
				{
					ocean_assert(false && "Invalid padding memory!");

					allSucceeded = false;
					break;
				}

				if (!validateSetChannel<T>(copyFrame.constdata<T>(), frame.constdata<T>(), frame.width(), frame.height(), value, tChannelIndex, tChannels, frame.paddingElements()))
				{
					allSucceeded = false;
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms";
		Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
	}

	Log::info() << " ";

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

bool TestFrameChannels::testApplyAdvancedPixelModifier(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "Test for advanced pixel modifier:";
	Log::info().newLine();

	allSucceeded = testApplyAdvancedPixelModifier<uint8_t, 1u, testFunctionApplyAdvancedModifier<uint8_t, uint8_t, 1u>>(width, height, testDuration, worker) && allSucceeded;

	Log::info().newLine();

	allSucceeded = testApplyAdvancedPixelModifier<uint8_t, 2u, testFunctionApplyAdvancedModifier<uint8_t, uint8_t, 2u>>(width, height, testDuration, worker) && allSucceeded;

	Log::info().newLine();

	allSucceeded = testApplyAdvancedPixelModifier<uint8_t, 3u, testFunctionApplyAdvancedModifier<uint8_t, uint8_t, 3u>>(width, height, testDuration, worker) && allSucceeded;

	Log::info().newLine();

	allSucceeded = testApplyAdvancedPixelModifier<uint8_t, 4u, testFunctionApplyAdvancedModifier<uint8_t, uint8_t, 4u>>(width, height, testDuration, worker) && allSucceeded;

	Log::info().newLine();

	return allSucceeded;
}

template <typename TElement, unsigned int tChannels, void (*tPixelFunction)(const TElement*, TElement*)>
bool TestFrameChannels::testApplyAdvancedPixelModifier(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert(std::is_integral<TElement>::value || std::is_arithmetic<TElement>::value, "The type must be integral or arithmetic.");
	static_assert(tChannels >= 1u && tChannels <= 4u, "The number of channels must be in the range [1, 4].");
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	for (const CV::FrameConverter::ConversionFlag conversionFlag : CV::FrameConverter::conversionFlags())
	{
		const std::string conversionName = CV::FrameConverter::translateConversionFlag(conversionFlag);

		Log::info().newLine();
		Log::info() << "Test for advanced pixel modifier with image of size " << width << "x" << height << ", " << TypeNamer::name<TElement>() << ", " << conversionName << ":";

		constexpr FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<TElement, tChannels>();

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		const unsigned int maxWorkerIterations = worker ? 2u : 1u;

		for (const bool performanceIteration : {false, true})
		{
			for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
			{
				Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
				HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

				Timestamp startTimestamp(true);

				do
				{
					const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 1u, 2048u);
					const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 1u, 2048u);

					const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 1u, 256u) * RandomI::random(randomGenerator, 1u);
					const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 1u, 256u) * RandomI::random(randomGenerator, 1u);

					Frame source(FrameType(testWidth, testHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
					Frame target(FrameType(testWidth, testHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), targetPaddingElements);

					CV::CVUtilities::randomizeFrame(source, /* skipPaddingArea */ false, &randomGenerator);
					CV::CVUtilities::randomizeFrame(target, /* skipPaddingArea */ false, &randomGenerator);

					const Frame targetClone(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.startIf(performanceIteration);
					CV::FrameChannels::applyAdvancedPixelModifier<TElement, TElement, tChannels, tChannels, tPixelFunction>(source.constdata<TElement>(), target.data<TElement>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), conversionFlag);
					performance.stopIf(performanceIteration);

					if (!CV::CVUtilities::isPaddingMemoryIdentical(target, targetClone))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					if (!validateApplyAdvancedPixelModifier<TElement, tChannels, tPixelFunction>(source, target, conversionFlag))
					{
						allSucceeded = false;
					}
				}
				while (startTimestamp + testDuration > Timestamp(true));
			}
		}

		Log::info() << "Performance (single-core): Best: " << performanceSinglecore.bestMseconds() << "ms, worst: " << performanceSinglecore.worstMseconds() << "ms, average: " << performanceSinglecore.averageMseconds() << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Performance (multi-core): Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms";
			Log::info() << "Multi-core boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;

	return false;
}

template <typename TPrecision, unsigned int tChannels>
bool TestFrameChannels::testApplyBivariateOperator(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	const std::vector<CV::FrameConverter::ConversionFlag> conversionFlags = CV::FrameConverter::conversionFlags();

	bool allSucceeded = true;

	constexpr unsigned int performanceWidth = 1920u;
	constexpr unsigned int performanceHeight = 1080u;

	RandomGenerator randomGenerator;

	for (unsigned int n = 0u; n < 4u; ++n)
	{
		const CV::FrameConverter::ConversionFlag conversionFlag = conversionFlags[n];
		const std::string conversionName = CV::FrameConverter::translateConversionFlag(conversionFlag);

		Log::info().newLine(n != 0u);
		Log::info() << "Pixel-wise subtraction of two frames, " << performanceWidth << "x" << performanceHeight << ", " << TypeNamer::name<TPrecision>() << ", " << conversionName << ":";

		constexpr FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<TPrecision, tChannels>();

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		const unsigned int maxWorkerIterations = worker ? 2u : 1u;

		for (const bool performanceIteration : {true, false})
		{
			const unsigned int width = performanceIteration ? performanceWidth : RandomI::random(randomGenerator, 1u, 2048u);
			const unsigned int height = performanceIteration ? performanceHeight : RandomI::random(randomGenerator, 1u, 2048u);

			const unsigned int source0PaddingElements = RandomI::random(randomGenerator, 1u, 256u) * RandomI::random(randomGenerator, 1u);
			const unsigned int source1PaddingElements = RandomI::random(randomGenerator, 1u, 256u) * RandomI::random(randomGenerator, 1u);
			const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 1u, 256u) * RandomI::random(randomGenerator, 1u);

			Frame source0(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), source0PaddingElements);
			Frame source1(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), source1PaddingElements);

			Frame target(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), targetPaddingElements);

			for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
			{
				Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
				HighPerformanceStatistic& performanceMatrix = useWorker ? performanceMulticore : performanceSinglecore;

				Timestamp startTimestamp(true);

				do
				{
					CV::CVUtilities::randomizeFrame(source0, /* skipPaddingArea */ true, &randomGenerator);
					CV::CVUtilities::randomizeFrame(source1, /* skipPaddingArea */ true, &randomGenerator);

					CV::CVUtilities::randomizeFrame(target, /* skipPaddingArea */ false, &randomGenerator);

					const Frame targetClone(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performanceMatrix.startIf(performanceIteration);
					CV::FrameChannels::applyBivariateOperator<TPrecision, TPrecision, TPrecision, TPrecision, tChannels, tChannels, TestOperations<tChannels>::subtract>(source0.constdata<TPrecision>(), source1.constdata<TPrecision>(), target.data<TPrecision>(), source0.width(), source0.height(), source0.paddingElements(), source1.paddingElements(), target.paddingElements(), conversionFlag, useWorker);
					performanceMatrix.stopIf(performanceIteration);

					if (!validateApplyBivariateOperatorSubtract<TPrecision, TPrecision, TPrecision>(source0, source1, target, conversionFlag))
					{
						allSucceeded = false;
					}

					if (!CV::CVUtilities::isPaddingMemoryIdentical(target, targetClone))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}
				}
				while (startTimestamp + testDuration > Timestamp(true));
			}
		}

		Log::info() << "Performance (single-core): Best: " << performanceSinglecore.bestMseconds() << "ms, worst: " << performanceSinglecore.worstMseconds() << "ms, average: " << performanceSinglecore.averageMseconds() << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Performance (multi-core): Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms";
			Log::info() << "Multi-core boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
		}
	}

	Log::info() << " ";

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

bool TestFrameChannels::testTransformGeneric(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Transform generic frame:";

	const std::vector<unsigned int> widths = {64u, 1280u, 1920u};
	const std::vector<unsigned int> heights = {64u, 720u, 1080u};

	bool allSucceeded = true;

	ocean_assert(widths.size() == heights.size());

	for (size_t i = 0; i < widths.size(); ++i)
	{
		const unsigned int width = widths[i];
		const unsigned int height = heights[i];

		allSucceeded = testTransformGeneric<uint8_t, 1u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<uint8_t, 2u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<uint8_t, 3u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<uint8_t, 4u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<uint8_t, 5u>(width, height, testDuration, worker) && allSucceeded;

		allSucceeded = testTransformGeneric<int8_t, 1u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<int8_t, 2u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<int8_t, 3u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<int8_t, 4u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<int8_t, 5u>(width, height, testDuration, worker) && allSucceeded;

		allSucceeded = testTransformGeneric<uint16_t, 1u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<uint16_t, 2u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<uint16_t, 3u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<uint16_t, 4u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<uint16_t, 5u>(width, height, testDuration, worker) && allSucceeded;

		allSucceeded = testTransformGeneric<int16_t, 1u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<int16_t, 2u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<int16_t, 3u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<int16_t, 4u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<int16_t, 5u>(width, height, testDuration, worker) && allSucceeded;

		allSucceeded = testTransformGeneric<uint32_t, 1u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<uint32_t, 2u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<uint32_t, 3u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<uint32_t, 4u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<uint32_t, 5u>(width, height, testDuration, worker) && allSucceeded;

		allSucceeded = testTransformGeneric<int32_t, 1u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<int32_t, 2u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<int32_t, 3u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<int32_t, 4u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<int32_t, 5u>(width, height, testDuration, worker) && allSucceeded;

		allSucceeded = testTransformGeneric<float, 1u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<float, 2u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<float, 3u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<float, 4u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<float, 5u>(width, height, testDuration, worker) && allSucceeded;

		allSucceeded = testTransformGeneric<double, 1u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<double, 2u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<double, 3u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<double, 4u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testTransformGeneric<double, 5u>(width, height, testDuration, worker) && allSucceeded;
	}

	return allSucceeded;
}

bool TestFrameChannels::testPremultipliedAlphaToStraightAlpha(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing premultiplied to straight alpha:";
	Log::info() << " ";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const unsigned int benchmarkWidth = 1280u;
	const unsigned int benchmarkHeight = 720u;

	HighPerformanceStatistic performanceMulticoreInplace;
	HighPerformanceStatistic performanceSinglecoreInplace;

	HighPerformanceStatistic performanceMulticoreWithTarget;
	HighPerformanceStatistic performanceSinglecoreWithTarget;

	const FrameType::PixelFormats pixelFormats =
	{
		FrameType::FORMAT_RGBA32,
		FrameType::FORMAT_ARGB32,
		FrameType::FORMAT_YA16,
	};

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (const FrameType::PixelFormat pixelFormat : pixelFormats)
	{
		Log::info() << "... with pixel format " << FrameType::translatePixelFormat(pixelFormat);

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performanceInplace = useWorker ? performanceMulticoreInplace : performanceSinglecoreInplace;
			HighPerformanceStatistic& performanceWithTarget = useWorker ? performanceMulticoreWithTarget : performanceSinglecoreWithTarget;

			Timestamp startTimestamp(true);

			do
			{
				for (const bool benchmarkIteration : {true, false})
				{
					const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
					const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

					const unsigned int width = benchmarkIteration ? benchmarkWidth : RandomI::random(randomGenerator, 1u, benchmarkWidth);
					const unsigned int height = benchmarkIteration ? benchmarkHeight : RandomI::random(randomGenerator, 1u, benchmarkHeight);

					Frame sourceFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
					Frame targetFrame(sourceFrame.frameType(), targetPaddingElements);

					CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);
					CV::CVUtilities::randomizeFrame(targetFrame, false, &randomGenerator);

					const Frame copySourceFrame(sourceFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
					const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performanceWithTarget.startIf(benchmarkIteration);
						CV::FrameChannels::Comfort::premultipliedAlphaToStraightAlpha(sourceFrame, targetFrame, useWorker);
					performanceWithTarget.stopIf(benchmarkIteration);

					performanceInplace.startIf(benchmarkIteration);
						CV::FrameChannels::Comfort::premultipliedAlphaToStraightAlpha(sourceFrame, useWorker);
					performanceInplace.stopIf(benchmarkIteration);

					if (!CV::CVUtilities::isPaddingMemoryIdentical(sourceFrame, copySourceFrame))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					const unsigned int channels = sourceFrame.channels();

					bool isLastChannel = false;
					FrameType::formatHasAlphaChannel(sourceFrame.pixelFormat(), &isLastChannel);

					const unsigned int alphaChannelIndex = isLastChannel ? channels - 1u : 0u;

					for (unsigned int y = 0u; y < height; ++y)
					{
						const uint8_t* const sourceRow = copySourceFrame.constrow<uint8_t>(y);

						const uint8_t* const targetRowA = sourceFrame.constrow<uint8_t>(y);
						const uint8_t* const targetRowB = sourceFrame.constrow<uint8_t>(y);

						if (memcmp(targetRowA, targetRowB, sourceFrame.planeWidthBytes(0)) != 0)
						{
							allSucceeded = false;
						}

						for (unsigned int x = 0u; x < width; ++x)
						{
							const uint8_t* const sourcePixel = sourceRow + x * channels;
							const uint8_t* const targetPixel = targetRowA + x * channels;

							const uint32_t alphaValue = sourcePixel[alphaChannelIndex];
							const uint32_t alphaValue_2 = alphaValue / 2u;

							for (unsigned int c = 0u; c < channels; ++c)
							{
								if (c == alphaChannelIndex)
								{
									if (sourcePixel[c] != targetPixel[c])
									{
										allSucceeded = false;
									}
								}
								else
								{
									const uint32_t targetValue = alphaValue == 0u ? uint32_t(sourcePixel[c]) : std::min((uint32_t(sourcePixel[c]) * 255u + alphaValue_2) / alphaValue, 255u);
									ocean_assert(targetValue <= 255u);

									const int error = abs(int(targetValue) - int(targetPixel[c]));

									if (error > 2)
									{
										allSucceeded = false;
									}
								}
							}
						}
					}
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		Log::info() << "In-place:";

		Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecoreInplace.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecoreInplace.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecoreInplace.averageMseconds(), 2u) << "ms";

		if (performanceMulticoreInplace.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticoreInplace.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticoreInplace.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticoreInplace.averageMseconds(), 2u) << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecoreInplace.best() / performanceMulticoreInplace.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecoreInplace.worst() / performanceMulticoreInplace.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecoreInplace.average() / performanceMulticoreInplace.average(), 1u) << "x";
		}

		Log::info() << "With additional target frame:";

		Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecoreWithTarget.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecoreWithTarget.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecoreWithTarget.averageMseconds(), 2u) << "ms";

		if (performanceMulticoreWithTarget.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticoreWithTarget.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticoreWithTarget.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticoreWithTarget.averageMseconds(), 2u) << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecoreWithTarget.best() / performanceMulticoreWithTarget.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecoreWithTarget.worst() / performanceMulticoreWithTarget.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecoreWithTarget.average() / performanceMulticoreWithTarget.average(), 1u) << "x";
		}

		Log::info() << " ";
	}

	Log::info() << " ";

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

bool TestFrameChannels::testStraightAlphaToPremultipliedAlpha(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing straight to premultiplied alpha:";
	Log::info() << " ";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const unsigned int benchmarkWidth = 1280u;
	const unsigned int benchmarkHeight = 720u;

	HighPerformanceStatistic performanceMulticoreInplace;
	HighPerformanceStatistic performanceSinglecoreInplace;

	HighPerformanceStatistic performanceMulticoreWithTarget;
	HighPerformanceStatistic performanceSinglecoreWithTarget;

	const FrameType::PixelFormats pixelFormats =
	{
		FrameType::FORMAT_RGBA32,
		FrameType::FORMAT_ARGB32,
		FrameType::FORMAT_YA16,
	};

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (const FrameType::PixelFormat pixelFormat : pixelFormats)
	{
		Log::info() << "... with pixel format " << FrameType::translatePixelFormat(pixelFormat);

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performanceInplace = useWorker ? performanceMulticoreInplace : performanceSinglecoreInplace;
			HighPerformanceStatistic& performanceWithTarget = useWorker ? performanceMulticoreWithTarget : performanceSinglecoreWithTarget;

			Timestamp startTimestamp(true);

			do
			{
				for (const bool benchmarkIteration : {true, false})
				{
					const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
					const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

					const unsigned int width = benchmarkIteration ? benchmarkWidth : RandomI::random(randomGenerator, 1u, benchmarkWidth);
					const unsigned int height = benchmarkIteration ? benchmarkHeight : RandomI::random(randomGenerator, 1u, benchmarkHeight);

					Frame sourceFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
					Frame targetFrame(sourceFrame.frameType(), targetPaddingElements);

					CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);
					CV::CVUtilities::randomizeFrame(targetFrame, false, &randomGenerator);

					const Frame copySourceFrame(sourceFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
					const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performanceWithTarget.startIf(benchmarkIteration);
						CV::FrameChannels::Comfort::straightAlphaToPremultipliedAlpha(sourceFrame, targetFrame, useWorker);
					performanceWithTarget.stopIf(benchmarkIteration);

					performanceInplace.startIf(benchmarkIteration);
						CV::FrameChannels::Comfort::straightAlphaToPremultipliedAlpha(sourceFrame, useWorker);
					performanceInplace.stopIf(benchmarkIteration);

					if (!CV::CVUtilities::isPaddingMemoryIdentical(sourceFrame, copySourceFrame))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					const unsigned int channels = sourceFrame.channels();

					bool isLastChannel = false;
					FrameType::formatHasAlphaChannel(sourceFrame.pixelFormat(), &isLastChannel);

					const unsigned int alphaChannelIndex = isLastChannel ? channels - 1u : 0u;

					for (unsigned int y = 0u; y < height; ++y)
					{
						const uint8_t* const sourceRow = copySourceFrame.constrow<uint8_t>(y);

						const uint8_t* const targetRowA = sourceFrame.constrow<uint8_t>(y);
						const uint8_t* const targetRowB = sourceFrame.constrow<uint8_t>(y);

						if (memcmp(targetRowA, targetRowB, sourceFrame.planeWidthBytes(0)) != 0)
						{
							allSucceeded = false;
						}

						for (unsigned int x = 0u; x < width; ++x)
						{
							const uint8_t* const sourcePixel = sourceRow + x * channels;
							const uint8_t* const targetPixel = targetRowA + x * channels;

							for (unsigned int c = 0u; c < channels; ++c)
							{
								const uint32_t alphaValue = sourcePixel[alphaChannelIndex];

								if (c == alphaChannelIndex)
								{
									if (sourcePixel[c] != targetPixel[c])
									{
										allSucceeded = false;
									}
								}
								else
								{
									const uint32_t targetValue = (uint32_t(sourcePixel[c]) * alphaValue + 127u) / 255u;
									ocean_assert(targetValue <= 255u);

									const int error = abs(int(targetValue) - int(targetPixel[c]));

									if (error > 2)
									{
										allSucceeded = false;
									}
								}
							}
						}
					}
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		Log::info() << "In-place:";

		Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecoreInplace.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecoreInplace.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecoreInplace.averageMseconds(), 2u) << "ms";

		if (performanceMulticoreInplace.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticoreInplace.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticoreInplace.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticoreInplace.averageMseconds(), 2u) << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecoreInplace.best() / performanceMulticoreInplace.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecoreInplace.worst() / performanceMulticoreInplace.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecoreInplace.average() / performanceMulticoreInplace.average(), 1u) << "x";
		}

		Log::info() << "With additional target frame:";

		Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecoreWithTarget.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecoreWithTarget.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecoreWithTarget.averageMseconds(), 2u) << "ms";

		if (performanceMulticoreWithTarget.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticoreWithTarget.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticoreWithTarget.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticoreWithTarget.averageMseconds(), 2u) << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecoreWithTarget.best() / performanceMulticoreWithTarget.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecoreWithTarget.worst() / performanceMulticoreWithTarget.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecoreWithTarget.average() / performanceMulticoreWithTarget.average(), 1u) << "x";
		}

		Log::info() << " ";
	}

	Log::info() << " ";

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

template <typename TElementType, unsigned int tChannels>
bool TestFrameChannels::testTransformGeneric(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert(tChannels != 0u, "Valid range for number of channels: [1, infinity)");

	ocean_assert(width != 0u && height != 0u);
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	Log::info() << " ";
	Log::info() << "... testing " << width << "x" << height << " '" << TypeNamer::name<TElementType>() << "' image with " << tChannels << " channels:";
	Log::info() << " ";

	for (const CV::FrameConverter::ConversionFlag conversionFlag : CV::FrameConverter::conversionFlags())
	{
		Log::info() << "... with " << CV::FrameConverter::translateConversionFlag(conversionFlag) << " conversion:";

		HighPerformanceStatistic performanceMulticore;
		HighPerformanceStatistic performanceSinglecore;

		const unsigned int maxWorkerIterations = worker ? 2u : 1u;

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			Timestamp startTimestamp(true);

			do
			{
				for (const bool benchmarkIteration : {true, false})
				{
					const unsigned int testWidth = benchmarkIteration ? width : RandomI::random(randomGenerator, 1u, width);
					const unsigned int testHeight = benchmarkIteration ? height : RandomI::random(randomGenerator, 1u, height);

					const FrameType frameType(testWidth, testHeight, FrameType::genericPixelFormat<TElementType, tChannels>(), FrameType::ORIGIN_UPPER_LEFT);

					const Frame sourceFrame = CV::CVUtilities::randomizedFrame(frameType, &randomGenerator);
					Frame targetFrame = CV::CVUtilities::randomizedFrame(frameType, &randomGenerator);

					const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.startIf(benchmarkIteration);
						CV::FrameChannels::transformGeneric<TElementType, tChannels>(sourceFrame.constdata<TElementType>(), targetFrame.data<TElementType>(), sourceFrame.width(), sourceFrame.height(), conversionFlag, sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
					performance.stopIf(benchmarkIteration);

					if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
					{
						ocean_assert(false && "Invalid padding memory!");

						allSucceeded = false;
						break;
					}

					if (!validateTransformGeneric<TElementType, tChannels>(sourceFrame.constdata<TElementType>(), targetFrame.constdata<TElementType>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), conversionFlag))
					{
						allSucceeded = false;
					}
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
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

bool TestFrameChannels::testReverseChannelOrder(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing reversing channel order of frame pixels:";

	const std::vector<unsigned int> widths = {64u, 1280u, 1920u};
	const std::vector<unsigned int> heights = {64u, 720u, 1080u};

	bool allSucceeded = true;

	ocean_assert(widths.size() == heights.size());

	for (size_t i = 0; i < widths.size(); ++i)
	{
		const unsigned int width = widths[i];
		const unsigned int height = heights[i];

		allSucceeded = testReverseChannelOrder<uint8_t, 1u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<uint8_t, 2u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<uint8_t, 3u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<uint8_t, 4u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<uint8_t, 5u>(width, height, testDuration, worker) && allSucceeded;

		allSucceeded = testReverseChannelOrder<int8_t, 1u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<int8_t, 2u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<int8_t, 3u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<int8_t, 4u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<int8_t, 5u>(width, height, testDuration, worker) && allSucceeded;

		allSucceeded = testReverseChannelOrder<uint16_t, 1u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<uint16_t, 2u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<uint16_t, 3u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<uint16_t, 4u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<uint16_t, 5u>(width, height, testDuration, worker) && allSucceeded;

		allSucceeded = testReverseChannelOrder<int16_t, 1u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<int16_t, 2u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<int16_t, 3u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<int16_t, 4u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<int16_t, 5u>(width, height, testDuration, worker) && allSucceeded;

		allSucceeded = testReverseChannelOrder<uint32_t, 1u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<uint32_t, 2u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<uint32_t, 3u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<uint32_t, 4u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<uint32_t, 5u>(width, height, testDuration, worker) && allSucceeded;

		allSucceeded = testReverseChannelOrder<int32_t, 1u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<int32_t, 2u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<int32_t, 3u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<int32_t, 4u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<int32_t, 5u>(width, height, testDuration, worker) && allSucceeded;

		allSucceeded = testReverseChannelOrder<float, 1u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<float, 2u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<float, 3u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<float, 4u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<float, 5u>(width, height, testDuration, worker) && allSucceeded;

		allSucceeded = testReverseChannelOrder<double, 1u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<double, 2u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<double, 3u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<double, 4u>(width, height, testDuration, worker) && allSucceeded;
		allSucceeded = testReverseChannelOrder<double, 5u>(width, height, testDuration, worker) && allSucceeded;
	}

	return allSucceeded;
}

template <typename T, unsigned int tChannels>
bool TestFrameChannels::testReverseChannelOrder(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert(tChannels != 0u, "Valid range for number of channels: [1, infinity)");

	ocean_assert(width != 0u && height != 0u);
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	Log::info() << " ";
	Log::info() << "... testing " << width << "x" << height << " '" << TypeNamer::name<T>() << "' image with " << tChannels << " channels:";
	Log::info() << " ";

	for (const CV::FrameConverter::ConversionFlag conversionFlag : CV::FrameConverter::conversionFlags())
	{
		Log::info() << "... with " << CV::FrameConverter::translateConversionFlag(conversionFlag) << " conversion:";

		HighPerformanceStatistic performanceMulticore;
		HighPerformanceStatistic performanceSinglecore;

		const unsigned int maxWorkerIterations = worker ? 2u : 1u;

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			Timestamp startTimestamp(true);

			do
			{
				for (const bool benchmarkIteration : {true, false})
				{
					const unsigned int testWidth = benchmarkIteration ? width : RandomI::random(randomGenerator, 1u, width);
					const unsigned int testHeight = benchmarkIteration ? height : RandomI::random(randomGenerator, 1u, height);

					const FrameType frameType(testWidth, testHeight, FrameType::genericPixelFormat<T, tChannels>(), FrameType::ORIGIN_UPPER_LEFT);

					const Frame sourceFrame = CV::CVUtilities::randomizedFrame(frameType, &randomGenerator);
					Frame targetFrame = CV::CVUtilities::randomizedFrame(frameType, &randomGenerator);

					const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.startIf(benchmarkIteration);
						CV::FrameChannels::reverseChannelOrder<T, tChannels>(sourceFrame.constdata<T>(), targetFrame.data<T>(), sourceFrame.width(), sourceFrame.height(), conversionFlag, sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
					performance.stopIf(benchmarkIteration);

					if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
					{
						ocean_assert(false && "Invalid padding memory!");

						allSucceeded = false;
						break;
					}

					if (!validateReverseChannelOrder<T>(sourceFrame.constdata<T>(), targetFrame.constdata<T>(), sourceFrame.width(), sourceFrame.height(), tChannels, sourceFrame.paddingElements(), targetFrame.paddingElements(), conversionFlag))
					{
						allSucceeded = false;
					}
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
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

bool TestFrameChannels::testRowPixelConversion3ChannelsTo1Channel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing row pixel conversion function 3 channels to 1 channel:";

	bool allSucceeded = true;

	typedef std::vector<unsigned char> Elements;

	RandomGenerator randomGenerator;

	Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int paddingElements = RandomI::random(randomGenerator, 0u, 100u);

		const unsigned int sourceStrideElements = 3u * width + paddingElements;
		const unsigned int targetStrideElements = width + paddingElements;

		Elements sourcePixels(sourceStrideElements);
		Elements targetPixels(targetStrideElements);

		for (unsigned char& element : sourcePixels)
		{
			element = (unsigned char)(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (unsigned char& element : targetPixels)
		{
			element = (unsigned char)(RandomI::random(randomGenerator, 0u, 255u));
		}

		const Elements copyTargetPixels(targetPixels);

		unsigned int factors[3];
		unsigned int remainingSumFactors = 128u;
		unsigned int factorFlag = 0u;

		for (unsigned int n = 0u; n < 3u; ++n)
		{
			ocean_assert(remainingSumFactors <= 128u);
			const unsigned int factor = n < 2u ? RandomI::random(randomGenerator, remainingSumFactors) : remainingSumFactors;

			factors[n] = factor;
			remainingSumFactors -= factor;

			factorFlag = factorFlag | ((factor != 0u) << n);
		}

		switch (factorFlag)
		{
			case 1u:
				CV::FrameChannels::convertRow3ChannelsTo1Channel8BitPerChannel7BitPrecision<true, false, false>(sourcePixels.data(), targetPixels.data(), width, factors);
				break;

			case 2u:
				CV::FrameChannels::convertRow3ChannelsTo1Channel8BitPerChannel7BitPrecision<false, true, false>(sourcePixels.data(), targetPixels.data(), width, factors);
				break;

			case 3u:
				CV::FrameChannels::convertRow3ChannelsTo1Channel8BitPerChannel7BitPrecision<true, true, false>(sourcePixels.data(), targetPixels.data(), width, factors);
				break;

			case 4u:
				CV::FrameChannels::convertRow3ChannelsTo1Channel8BitPerChannel7BitPrecision<false, false, true>(sourcePixels.data(), targetPixels.data(), width, factors);
				break;

			case 5u:
				CV::FrameChannels::convertRow3ChannelsTo1Channel8BitPerChannel7BitPrecision<true, false, true>(sourcePixels.data(), targetPixels.data(), width, factors);
				break;

			case 6u:
				CV::FrameChannels::convertRow3ChannelsTo1Channel8BitPerChannel7BitPrecision<false, true, true>(sourcePixels.data(), targetPixels.data(), width, factors);
				break;

			case 7u:
				CV::FrameChannels::convertRow3ChannelsTo1Channel8BitPerChannel7BitPrecision<true, true, true>(sourcePixels.data(), targetPixels.data(), width, factors);
				break;

			default:
				ocean_assert(false && "Invalid flag!");
				allSucceeded = false;
				break;
		}

		// verify that the conversion is correct
		for (unsigned int x = 0u; x < width; ++x)
		{
			const unsigned int targetValue = (sourcePixels[x * 3u + 0u] * factors[0] + sourcePixels[x * 3u + 1u] * factors[1] + sourcePixels[x * 3u + 2u] * factors[2] + 64u) / 128u;

			if (targetValue != targetPixels[x])
			{
				allSucceeded = false;
			}
		}

		// verify that the padding elements are untouched
		if (paddingElements > 0u)
		{
			if (memcmp(targetPixels.data() + width, copyTargetPixels.data() + width, paddingElements) != 0)
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

bool TestFrameChannels::testRowPixelConversion3ChannelsTo3Channels6BitPrecision(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing row pixel conversion function 3 channels to 3 channels with 6 bit precision:";

	bool allSucceeded = true;

	typedef std::vector<unsigned char> Elements;

	RandomGenerator randomGenerator;

	Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int paddingElements = RandomI::random(randomGenerator, 0u, 100u);

		const unsigned int sourceStrideElements = 3u * width + paddingElements;
		const unsigned int targetStrideElements = 3u * width + paddingElements;

		Elements sourcePixels(sourceStrideElements);
		Elements targetPixels(targetStrideElements);

		for (unsigned char& element : sourcePixels)
		{
			element = (unsigned char)(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (unsigned char& element : targetPixels)
		{
			element = (unsigned char)(RandomI::random(randomGenerator, 0u, 255u));
		}

		const Elements copyTargetPixels(targetPixels);

		int factors[12];

		// values for individual channels

		for (unsigned int n = 0u; n < 3u; ++n)
		{
			// bias value / translation value
			factors[9u + n] = RandomI::random(randomGenerator, 0, 128);
		}

		for (unsigned int n = 0u; n < 3u; ++n)
		{
			const int factor0 = RandomI::random(randomGenerator, -64 * 2, 64 * 2);
			const int factor1 = RandomI::random(randomGenerator, -64 * 2, 64 * 2);
			const int factor2 = RandomI::random(randomGenerator, -64 * 2, 64 * 2);

			if (std::abs((255 - factors[9]) * factor0) < 10922 // 10922 = (2^15 - 1) / 3
				&& std::abs((255 - factors[10]) * factor1) < 10922
				&& std::abs((255 - factors[11]) * factor2) < 10922)
			{
				factors[n + 0u] = factor0;
				factors[n + 3u] = factor1;
				factors[n + 6u] = factor2;
			}
			else
			{
				--n;
			}
		}

		CV::FrameChannels::convertRow3ChannelsTo3Channels8BitPerChannel6BitPrecision(sourcePixels.data(), targetPixels.data(), width, factors);

		// verify that the conversion is correct
		for (unsigned int x = 0u; x < width; ++x)
		{
			// layout of parameters f00, f10, f20, f01, f02, ..., f22, b0, b1, b2
			// with transformation:
			// t0 = clamp(0, f00 * (s0 - b0) + f01 * (s1 - b1) + f02 * (s2 - b2), 255)
			// t1 = clamp(0, f10 * (s0 - b0) + f11 * (s1 - b1) + f12 * (s2 - b2), 255)
			// t2 = clamp(0, f20 * (s0 - b0) + f21 * (s1 - b1) + f22 * (s2 - b2), 255)

			const int targetValue0 = minmax<int>(0, ((sourcePixels[x * 3u + 0u] - factors[9]) * factors[0] + (sourcePixels[x * 3u + 1u] - factors[10]) * factors[3] + (sourcePixels[x * 3u + 2u] - factors[11]) * factors[6]) / 64, 255);
			const int targetValue1 = minmax<int>(0, ((sourcePixels[x * 3u + 0u] - factors[9]) * factors[1] + (sourcePixels[x * 3u + 1u] - factors[10]) * factors[4] + (sourcePixels[x * 3u + 2u] - factors[11]) * factors[7]) / 64, 255);
			const int targetValue2 = minmax<int>(0, ((sourcePixels[x * 3u + 0u] - factors[9]) * factors[2] + (sourcePixels[x * 3u + 1u] - factors[10]) * factors[5] + (sourcePixels[x * 3u + 2u] - factors[11]) * factors[8]) / 64, 255);

			if (abs(targetValue0 - int(targetPixels[x * 3u + 0u])) > 1)
			{
				allSucceeded = false;
			}

			if (abs(targetValue1 - int(targetPixels[x * 3u + 1u])) > 1)
			{
				allSucceeded = false;
			}

			if (abs(targetValue2 - int(targetPixels[x * 3u + 2u])) > 1)
			{
				allSucceeded = false;
			}
		}

		// verify that the padding elements are untouched
		if (paddingElements > 0u)
		{
			if (memcmp(targetPixels.data() + width * 3u, copyTargetPixels.data() + width * 3u, paddingElements) != 0)
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

bool TestFrameChannels::testRowPixelConversion3ChannelsTo3Channels7BitPrecision(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing row pixel conversion function 3 channels to 3 channels with 7 bit precision:";

	bool allSucceeded = true;

	typedef std::vector<unsigned char> Elements;

	RandomGenerator randomGenerator;

	Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int paddingElements = RandomI::random(randomGenerator, 0u, 100u);

		const unsigned int sourceStrideElements = 3u * width + paddingElements;
		const unsigned int targetStrideElements = 3u * width + paddingElements;

		Elements sourcePixels(sourceStrideElements);
		Elements targetPixels(targetStrideElements);

		for (unsigned char& element : sourcePixels)
		{
			element = (unsigned char)(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (unsigned char& element : targetPixels)
		{
			element = (unsigned char)(RandomI::random(randomGenerator, 0u, 255u));
		}

		const Elements copyTargetPixels(targetPixels);

		int factors[12];

		// multiplication factors
		for (unsigned int n = 0u; n < 9u; ++n)
		{
			factors[n] = RandomI::random(randomGenerator, -64, 64);

			if (n == 6u)
			{
				if (abs(factors[0] + factors[3] + factors[6]) >= 128)
					--n;
			}
			else if (n == 7u)
			{
				if (abs(factors[1] + factors[4] + factors[7]) >= 128)
					--n;
			}
			if (n == 8u)
			{
				if (abs(factors[2] + factors[5] + factors[8]) >= 128)
					--n;
			}
		}

		// bias values
		for (unsigned int n = 9u; n < 12u; ++n)
		{
			factors[n] = RandomI::random(randomGenerator, -127, 127);
		}

		CV::FrameChannels::convertRow3ChannelsTo3Channels8BitPerChannel7BitPrecision(sourcePixels.data(), targetPixels.data(), width, factors);

		// verify that the conversion is correct
		for (unsigned int x = 0u; x < width; ++x)
		{
			// layout of parameters f00, f10, f20, f01, f02, ..., f22, b0, b1, b2
			// with transformation:
			// t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
			// t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
			// t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2

			const int targetValue0 = minmax<int>(0, (sourcePixels[x * 3u + 0u] * factors[0] + sourcePixels[x * 3u + 1u] * factors[3] + sourcePixels[x * 3u + 2u] * factors[6]) / 128 + factors[9], 255);
			const int targetValue1 = minmax<int>(0, (sourcePixels[x * 3u + 0u] * factors[1] + sourcePixels[x * 3u + 1u] * factors[4] + sourcePixels[x * 3u + 2u] * factors[7]) / 128 + factors[10], 255);
			const int targetValue2 = minmax<int>(0, (sourcePixels[x * 3u + 0u] * factors[2] + sourcePixels[x * 3u + 1u] * factors[5] + sourcePixels[x * 3u + 2u] * factors[8]) / 128 + factors[11], 255);

			if (abs(targetValue0 - int(targetPixels[x * 3u + 0u])) > 1)
			{
				allSucceeded = false;
			}

			if (abs(targetValue1 - int(targetPixels[x * 3u + 1u])) > 1)
			{
				allSucceeded = false;
			}

			if (abs(targetValue2 - int(targetPixels[x * 3u + 2u])) > 1)
			{
				allSucceeded = false;
			}
		}

		// verify that the padding elements are untouched
		if (paddingElements > 0u)
		{
			if (memcmp(targetPixels.data() + width * 3u, copyTargetPixels.data() + width * 3u, paddingElements) != 0)
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

bool TestFrameChannels::testRowPixelConversion3ChannelsTo3Channels10BitPrecision(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing row pixel conversion function 3 channels to 3 channels with 10 bit precision:";

	bool allSucceeded = true;

	typedef std::vector<unsigned char> Elements;

	RandomGenerator randomGenerator;

	Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int paddingElements = RandomI::random(randomGenerator, 0u, 100u);

		const unsigned int sourceStrideElements = 3u * width + paddingElements;
		const unsigned int targetStrideElements = 3u * width + paddingElements;

		Elements sourcePixels(sourceStrideElements);
		Elements targetPixels(targetStrideElements);

		for (unsigned char& element : sourcePixels)
		{
			element = (unsigned char)(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (unsigned char& element : targetPixels)
		{
			element = (unsigned char)(RandomI::random(randomGenerator, 0u, 255u));
		}

		const Elements copyTargetPixels(targetPixels);

		int factors[12];

		// 9 multiplication factors, and 3 bias factors
		for (unsigned int n = 0u; n < 12u; ++n)
		{
			const int sign = RandomI::random(randomGenerator, 1u) == 0u ? 1 : -1;

			factors[n] = int(RandomI::random(randomGenerator, 1024u * 16u)) * sign;
		}

		CV::FrameChannels::convertRow3ChannelsTo3Channels8BitPerChannel10BitPrecision(sourcePixels.data(), targetPixels.data(), width, factors);

		// verify that the conversion is correct
		for (unsigned int x = 0u; x < width; ++x)
		{
			// layout of parameters f00, f10, f20, f01, f02, ..., f22, b0, b1, b2
			// with transformation:
			// t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
			// t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
			// t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2

			const int targetValue0 = minmax<int>(0, (int(sourcePixels[x * 3u + 0u]) * factors[0] + int(sourcePixels[x * 3u + 1u]) * factors[3] + int(sourcePixels[x * 3u + 2u]) * factors[6]) / 1024 + factors[9], 255);
			const int targetValue1 = minmax<int>(0, (int(sourcePixels[x * 3u + 0u]) * factors[1] + int(sourcePixels[x * 3u + 1u]) * factors[4] + int(sourcePixels[x * 3u + 2u]) * factors[7]) / 1024 + factors[10], 255);
			const int targetValue2 = minmax<int>(0, (int(sourcePixels[x * 3u + 0u]) * factors[2] + int(sourcePixels[x * 3u + 1u]) * factors[5] + int(sourcePixels[x * 3u + 2u]) * factors[8]) / 1024 + factors[11], 255);

			if (abs(targetValue0 - int(targetPixels[x * 3u + 0u])) > 1)
			{
				allSucceeded = false;
			}

			if (abs(targetValue1 - int(targetPixels[x * 3u + 1u])) > 1)
			{
				allSucceeded = false;
			}

			if (abs(targetValue2 - int(targetPixels[x * 3u + 2u])) > 1)
			{
				allSucceeded = false;
			}
		}

		// verify that the padding elements are untouched
		if (paddingElements > 0u)
		{
			if (memcmp(targetPixels.data() + width * 3u, copyTargetPixels.data() + width * 3u, paddingElements) != 0)
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

bool TestFrameChannels::testRowPixelConversion4ChannelsTo1Channel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing row pixel conversion function 4 channels to 1 channel:";

	bool allSucceeded = true;

	typedef std::vector<unsigned char> Elements;

	RandomGenerator randomGenerator;

	Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int paddingElements = RandomI::random(randomGenerator, 0u, 100u);

		const unsigned int sourceStrideElements = 4u * width + paddingElements;
		const unsigned int targetStrideElements = width + paddingElements;

		Elements sourcePixels(sourceStrideElements);
		Elements targetPixels(targetStrideElements);

		for (unsigned char& element : sourcePixels)
		{
			element = (unsigned char)(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (unsigned char& element : targetPixels)
		{
			element = (unsigned char)(RandomI::random(randomGenerator, 0u, 255u));
		}

		const Elements copyTargetPixels(targetPixels);

		unsigned int factors[4];
		unsigned int remainingSumFactors = 128u;
		unsigned int factorFlag = 0u;

		for (unsigned int n = 0u; n < 4u; ++n)
		{
			ocean_assert(remainingSumFactors <= 128u);
			const unsigned int factor = n < 3u ? RandomI::random(randomGenerator, std::min(remainingSumFactors, 127u)) : remainingSumFactors;

			factors[n] = factor;
			remainingSumFactors -= factor;

			factorFlag = factorFlag | ((factor != 0u) << n);
		}

		switch (factorFlag)
		{
			case 1u:
				CV::FrameChannels::convertRow4ChannelsTo1Channel8BitPerChannel7BitPrecision<true, false, false, false>(sourcePixels.data(), targetPixels.data(), width, factors);
				break;

			case 2u:
				CV::FrameChannels::convertRow4ChannelsTo1Channel8BitPerChannel7BitPrecision<false, true, false, false>(sourcePixels.data(), targetPixels.data(), width, factors);
				break;

			case 3u:
				CV::FrameChannels::convertRow4ChannelsTo1Channel8BitPerChannel7BitPrecision<true, true, false, false>(sourcePixels.data(), targetPixels.data(), width, factors);
				break;

			case 4u:
				CV::FrameChannels::convertRow4ChannelsTo1Channel8BitPerChannel7BitPrecision<false, false, true, false>(sourcePixels.data(), targetPixels.data(), width, factors);
				break;

			case 5u:
				CV::FrameChannels::convertRow4ChannelsTo1Channel8BitPerChannel7BitPrecision<true, false, true, false>(sourcePixels.data(), targetPixels.data(), width, factors);
				break;

			case 6u:
				CV::FrameChannels::convertRow4ChannelsTo1Channel8BitPerChannel7BitPrecision<false, true, true, false>(sourcePixels.data(), targetPixels.data(), width, factors);
				break;

			case 7u:
				CV::FrameChannels::convertRow4ChannelsTo1Channel8BitPerChannel7BitPrecision<true, true, true, false>(sourcePixels.data(), targetPixels.data(), width, factors);
				break;

			case 8u:
				CV::FrameChannels::convertRow4ChannelsTo1Channel8BitPerChannel7BitPrecision<false, false, false, true>(sourcePixels.data(), targetPixels.data(), width, factors);
				break;

			case 9u:
				CV::FrameChannels::convertRow4ChannelsTo1Channel8BitPerChannel7BitPrecision<true, false, false, true>(sourcePixels.data(), targetPixels.data(), width, factors);
				break;

			case 10u:
				CV::FrameChannels::convertRow4ChannelsTo1Channel8BitPerChannel7BitPrecision<false, true, false, true>(sourcePixels.data(), targetPixels.data(), width, factors);
				break;

			case 11u:
				CV::FrameChannels::convertRow4ChannelsTo1Channel8BitPerChannel7BitPrecision<true, true, false, true>(sourcePixels.data(), targetPixels.data(), width, factors);
				break;

			case 12u:
				CV::FrameChannels::convertRow4ChannelsTo1Channel8BitPerChannel7BitPrecision<false, false, true, true>(sourcePixels.data(), targetPixels.data(), width, factors);
				break;

			case 13u:
				CV::FrameChannels::convertRow4ChannelsTo1Channel8BitPerChannel7BitPrecision<true, false, true, true>(sourcePixels.data(), targetPixels.data(), width, factors);
				break;

			case 14u:
				CV::FrameChannels::convertRow4ChannelsTo1Channel8BitPerChannel7BitPrecision<false, true, true, true>(sourcePixels.data(), targetPixels.data(), width, factors);
				break;

			case 15u:
				CV::FrameChannels::convertRow4ChannelsTo1Channel8BitPerChannel7BitPrecision<true, true, true, true>(sourcePixels.data(), targetPixels.data(), width, factors);
				break;

			default:
				ocean_assert(false && "Invalid flag!");
				allSucceeded = false;
				break;
		}

		// verify that the conversion is correct
		for (unsigned int x = 0u; x < width; ++x)
		{
			const unsigned int targetValue = (sourcePixels[x * 4u + 0u] * factors[0] + sourcePixels[x * 4u + 1u] * factors[1] + sourcePixels[x * 4u + 2u] * factors[2] + sourcePixels[x * 4u + 3u] * factors[3] + 64u) / 128u;

			if (targetValue != targetPixels[x])
			{
				allSucceeded = false;
			}
		}

		// verify that the padding elements are untouched
		if (paddingElements > 0u)
		{
			if (memcmp(targetPixels.data() + width, copyTargetPixels.data() + width, paddingElements) != 0)
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

bool TestFrameChannels::testRowPixelConversion4ChannelsTo2Channels(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing row pixel conversion function 4 channels to 2 channels:";

	bool allSucceeded = true;

	typedef std::vector<unsigned char> Elements;

	RandomGenerator randomGenerator;

	Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int paddingElements = RandomI::random(randomGenerator, 0u, 100u);

		const unsigned int sourceStrideElements = 4u * width + paddingElements;
		const unsigned int targetStrideElements = 2u * width + paddingElements;

		Elements sourcePixels(sourceStrideElements);
		Elements targetPixels(targetStrideElements);

		for (unsigned char& element : sourcePixels)
		{
			element = (unsigned char)(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (unsigned char& element : targetPixels)
		{
			element = (unsigned char)(RandomI::random(randomGenerator, 0u, 255u));
		}

		const Elements copyTargetPixels(targetPixels);

		unsigned int factors[8];

		unsigned int remainingSumFactorsChannel0 = 128u;
		unsigned int remainingSumFactorsChannel1 = 128u;

		for (unsigned int n = 0u; n < 4u; ++n)
		{
			ocean_assert(remainingSumFactorsChannel0 <= 128u);
			ocean_assert(remainingSumFactorsChannel1 <= 128u);

			const unsigned int factor0 = n < 3u ? RandomI::random(randomGenerator, std::min(remainingSumFactorsChannel0, 127u)) : remainingSumFactorsChannel0;
			const unsigned int factor1 = n < 3u ? RandomI::random(randomGenerator, std::min(remainingSumFactorsChannel1, 127u)) : remainingSumFactorsChannel1;

			factors[2u * n + 0u] = factor0;
			factors[2u * n + 1u] = factor1;

			remainingSumFactorsChannel0 -= factor0;
			remainingSumFactorsChannel1 -= factor1;
		}

		CV::FrameChannels::convertRow4ChannelsTo2Channels8BitPerChannel7BitPrecision(sourcePixels.data(), targetPixels.data(), width, factors);

		// verify that the conversion is correct
		for (unsigned int x = 0u; x < width; ++x)
		{
			const unsigned int targetValue0 = (sourcePixels[x * 4u + 0u] * factors[0] + sourcePixels[x * 4u + 1u] * factors[2] + sourcePixels[x * 4u + 2u] * factors[4] + sourcePixels[x * 4u + 3u] * factors[6] + 64u) / 128u;
			const unsigned int targetValue1 = (sourcePixels[x * 4u + 0u] * factors[1] + sourcePixels[x * 4u + 1u] * factors[3] + sourcePixels[x * 4u + 2u] * factors[5] + sourcePixels[x * 4u + 3u] * factors[7] + 64u) / 128u;

			if (targetValue0 != targetPixels[x * 2u + 0u])
			{
				allSucceeded = false;
			}

			if (targetValue1 != targetPixels[x * 2u + 1u])
			{
				allSucceeded = false;
			}
		}

		// verify that the padding elements are untouched
		if (paddingElements > 0u)
		{
			if (memcmp(targetPixels.data() + width * 2u, copyTargetPixels.data() + width * 2u, paddingElements) != 0)
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

bool TestFrameChannels::testRowPixelConversion4ChannelsTo3Channels(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing row pixel conversion function 4 channels to 3 channels with 7 bit precision:";

	bool allSucceeded = true;

	typedef std::vector<unsigned char> Elements;

	RandomGenerator randomGenerator;

	Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int paddingElements = RandomI::random(randomGenerator, 0u, 100u);

		const unsigned int sourceStrideElements = 4u * width + paddingElements;
		const unsigned int targetStrideElements = 3u * width + paddingElements;

		Elements sourcePixels(sourceStrideElements);
		Elements targetPixels(targetStrideElements);

		for (unsigned char& element : sourcePixels)
		{
			element = (unsigned char)(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (unsigned char& element : targetPixels)
		{
			element = (unsigned char)(RandomI::random(randomGenerator, 0u, 255u));
		}

		const Elements copyTargetPixels(targetPixels);

		int factors[15];

		// multiplication factors
		for (unsigned int n = 0u; n < 12u; ++n)
		{
			factors[n] = RandomI::random(randomGenerator, -64, 64);

			if (n == 9u)
			{
				if (abs(factors[0] + factors[3] + factors[6] + factors[9]) >= 128)
					n -= 4;
			}
			else if (n == 10u)
			{
				if (abs(factors[1] + factors[4] + factors[7] + factors[10]) >= 128)
					n -= 4;
			}
			if (n == 11u)
			{
				if (abs(factors[2] + factors[5] + factors[8] + factors[11]) >= 128)
					n -= 4;
			}
		}

		// bias values
		for (unsigned int n = 12u; n < 15u; ++n)
		{
			factors[n] = RandomI::random(randomGenerator, -127, 127);
		}

		CV::FrameChannels::convertRow4ChannelsTo3Channels8BitPerChannel7BitPrecision(sourcePixels.data(), targetPixels.data(), width, factors);

		// verify that the conversion is correct
		for (unsigned int x = 0u; x < width; ++x)
		{
			// layout of parameters f00, f10, f20, f01, f02, ..., f22, b0, b1, b2
			// with transformation:
			// t0 = f00 * s0 + f01 * s1 + f02 * s2 + f03 * s3 + b0
			// t1 = f10 * s0 + f11 * s1 + f12 * s2 + f13 * s3 + b1
			// t2 = f20 * s0 + f21 * s1 + f22 * s2 + f23 * s3 + b2

			const int targetValue0 = minmax<int>(0, (sourcePixels[x * 4u + 0u] * factors[0] + sourcePixels[x * 4u + 1u] * factors[3] + sourcePixels[x * 4u + 2u] * factors[6] + sourcePixels[x * 4u + 3u] * factors[9]) / 128 + factors[12], 255);
			const int targetValue1 = minmax<int>(0, (sourcePixels[x * 4u + 0u] * factors[1] + sourcePixels[x * 4u + 1u] * factors[4] + sourcePixels[x * 4u + 2u] * factors[7] + sourcePixels[x * 4u + 3u] * factors[10]) / 128 + factors[13], 255);
			const int targetValue2 = minmax<int>(0, (sourcePixels[x * 4u + 0u] * factors[2] + sourcePixels[x * 4u + 1u] * factors[5] + sourcePixels[x * 4u + 2u] * factors[8] + sourcePixels[x * 4u + 3u] * factors[11]) / 128 + factors[14], 255);

			if (abs(targetValue0 - int(targetPixels[x * 3u + 0u])) > 1)
			{
				allSucceeded = false;
			}

			if (abs(targetValue1 - int(targetPixels[x * 3u + 1u])) > 1)
			{
				allSucceeded = false;
			}

			if (abs(targetValue2 - int(targetPixels[x * 3u + 2u])) > 1)
			{
				allSucceeded = false;
			}
		}

		// verify that the padding elements are untouched
		if (paddingElements > 0u)
		{
			if (memcmp(targetPixels.data() + width * 3u, copyTargetPixels.data() + width * 3u, paddingElements) != 0)
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

bool TestFrameChannels::testReverseRowPixelOrder(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing reverse row pixel order function:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	Timestamp startTimestamp(true);

	do
	{
		allSucceeded = validateReverseRowPixelOrder<uint8_t, 1u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<uint8_t, 2u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<uint8_t, 3u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<uint8_t, 4u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<uint8_t, 5u>(randomGenerator) && allSucceeded;

		allSucceeded = validateReverseRowPixelOrder<int8_t, 1u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<int8_t, 2u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<int8_t, 3u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<int8_t, 4u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<int8_t, 5u>(randomGenerator) && allSucceeded;

		allSucceeded = validateReverseRowPixelOrder<uint16_t, 1u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<uint16_t, 2u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<uint16_t, 3u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<uint16_t, 4u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<uint16_t, 5u>(randomGenerator) && allSucceeded;

		allSucceeded = validateReverseRowPixelOrder<int16_t, 1u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<int16_t, 2u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<int16_t, 3u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<int16_t, 4u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<int16_t, 5u>(randomGenerator) && allSucceeded;

		allSucceeded = validateReverseRowPixelOrder<uint32_t, 1u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<uint32_t, 2u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<uint32_t, 3u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<uint32_t, 4u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<uint32_t, 5u>(randomGenerator) && allSucceeded;

		allSucceeded = validateReverseRowPixelOrder<int32_t, 1u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<int32_t, 2u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<int32_t, 3u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<int32_t, 4u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<int32_t, 5u>(randomGenerator) && allSucceeded;

		allSucceeded = validateReverseRowPixelOrder<float, 1u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<float, 2u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<float, 3u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<float, 4u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<float, 5u>(randomGenerator) && allSucceeded;

		allSucceeded = validateReverseRowPixelOrder<double, 1u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<double, 2u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<double, 3u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<double, 4u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowPixelOrder<double, 5u>(randomGenerator) && allSucceeded;
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

bool TestFrameChannels::testReverseRowChannelOrder(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing reverse row channel order function:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	Timestamp startTimestamp(true);

	do
	{
		allSucceeded = validateReverseRowChannelOrder<uint8_t, 1u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<uint8_t, 2u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<uint8_t, 3u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<uint8_t, 4u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<uint8_t, 5u>(randomGenerator) && allSucceeded;

		allSucceeded = validateReverseRowChannelOrder<int8_t, 1u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<int8_t, 2u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<int8_t, 3u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<int8_t, 4u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<int8_t, 5u>(randomGenerator) && allSucceeded;

		allSucceeded = validateReverseRowChannelOrder<uint16_t, 1u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<uint16_t, 2u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<uint16_t, 3u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<uint16_t, 4u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<uint16_t, 5u>(randomGenerator) && allSucceeded;

		allSucceeded = validateReverseRowChannelOrder<int16_t, 1u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<int16_t, 2u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<int16_t, 3u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<int16_t, 4u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<int16_t, 5u>(randomGenerator) && allSucceeded;

		allSucceeded = validateReverseRowChannelOrder<uint32_t, 1u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<uint32_t, 2u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<uint32_t, 3u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<uint32_t, 4u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<uint32_t, 5u>(randomGenerator) && allSucceeded;

		allSucceeded = validateReverseRowChannelOrder<int32_t, 1u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<int32_t, 2u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<int32_t, 3u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<int32_t, 4u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<int32_t, 5u>(randomGenerator) && allSucceeded;

		allSucceeded = validateReverseRowChannelOrder<float, 1u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<float, 2u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<float, 3u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<float, 4u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<float, 5u>(randomGenerator) && allSucceeded;

		allSucceeded = validateReverseRowChannelOrder<double, 1u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<double, 2u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<double, 3u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<double, 4u>(randomGenerator) && allSucceeded;
		allSucceeded = validateReverseRowChannelOrder<double, 5u>(randomGenerator) && allSucceeded;
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

bool TestFrameChannels::testShuffleRowChannels(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing function shuffling channels in row:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	Timestamp startTimestamp(true);

	do
	{
		// 1 channel to 3 channels, e.g., for Y to RGB
		allSucceeded = validateShuffleRowChannels<uint8_t, 1u, 3u, 0x000u>(randomGenerator) && allSucceeded;

		// 2 channel to 1 channels, e.g., for AY to Y
		allSucceeded = validateShuffleRowChannels<uint8_t, 2u, 1u, 0x1u>(randomGenerator) && allSucceeded;

		// 2 channel to 3 channels, e.g., for YA to YAA
		allSucceeded = validateShuffleRowChannels<uint8_t, 2u, 3u, 0x110u>(randomGenerator) && allSucceeded;

		// 2 channel to 4 channels, e.g., for YA to RGBA
		allSucceeded = validateShuffleRowChannels<uint8_t, 2u, 4u, 0x1000u>(randomGenerator) && allSucceeded;

		// 3 channels to 1 channels, e.g., for YUV to U
		allSucceeded = validateShuffleRowChannels<uint8_t, 3u, 1u, 0x1u>(randomGenerator) && allSucceeded;

		// 3 channels to 2 channels, e.g., for RGB to BR
		allSucceeded = validateShuffleRowChannels<uint8_t, 3u, 2u, 0x02u>(randomGenerator) && allSucceeded;

		// 3 channels to 3 channels, e.g., for RGB to BGR
		allSucceeded = validateShuffleRowChannels<uint8_t, 3u, 3u, 0x012u>(randomGenerator) && allSucceeded;

		// 4 channels to 1 channel, e.g., for ARGB to A
		allSucceeded = validateShuffleRowChannels<uint8_t, 4u, 1u, 0x0u>(randomGenerator) && allSucceeded;

		// 4 channels to 2 channels, e.g., for YUVA to YA
		allSucceeded = validateShuffleRowChannels<uint8_t, 4u, 2u, 0x30u>(randomGenerator) && allSucceeded;

		// 4 channels to 3 channels, e.g., for AYUV to YVU
		allSucceeded = validateShuffleRowChannels<uint8_t, 4u, 3u, 0x231u>(randomGenerator) && allSucceeded;

		// 4 channels to 4 channels, e.g., for YUVA to AYVU
		allSucceeded = validateShuffleRowChannels<uint8_t, 4u, 4u, 0x1203u>(randomGenerator) && allSucceeded;


		allSucceeded = validateShuffleRowChannels<uint8_t, 2u, 5u, 0x10110u>(randomGenerator) && allSucceeded;

		allSucceeded = validateShuffleRowChannels<int16_t, 3u, 3u, 0x120u>(randomGenerator) && allSucceeded;

		allSucceeded = validateShuffleRowChannels<uint32_t, 4u, 1u, 0x2u>(randomGenerator) && allSucceeded;

		allSucceeded = validateShuffleRowChannels<float, 3u, 6u, 0x120021u>(randomGenerator) && allSucceeded;
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

bool TestFrameChannels::testShuffleRowChannelsAndSetLastChannelValue(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing function shuffling channels and setting the last channel with constant value in row:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	Timestamp startTimestamp(true);

	do
	{
		// 1 channel to 4 channels, e.g., for Y to RGBA
		allSucceeded = validateShuffleRowAndSetLastChannelValueChannels<uint8_t, 1u, 4u, 0x000u>(randomGenerator) && allSucceeded;

		// 2 channels to 3 channels, e.g., for UV to UVA
		allSucceeded = validateShuffleRowAndSetLastChannelValueChannels<uint8_t, 2u, 3u, 0x10u>(randomGenerator) && allSucceeded;

		// 3 channels to 4 channels, e.g., for RGB to BGRA
		allSucceeded = validateShuffleRowAndSetLastChannelValueChannels<uint8_t, 3u, 4u, 0x012u>(randomGenerator) && allSucceeded;

		// 3 channels to 3 channels, e.g., for RGB to RGA
		allSucceeded = validateShuffleRowAndSetLastChannelValueChannels<uint8_t, 3u, 3u, 0x10u>(randomGenerator) && allSucceeded;

		// 3 channels to 2 channels, e.g., for RGB to RA
		allSucceeded = validateShuffleRowAndSetLastChannelValueChannels<uint8_t, 3u, 2u, 0x0u>(randomGenerator) && allSucceeded;

		// 4 channels to 4 channels, e.g., for BGR32 to RGBA32
		allSucceeded = validateShuffleRowAndSetLastChannelValueChannels<uint8_t, 4u, 4u, 0x012u>(randomGenerator) && allSucceeded;
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

bool TestFrameChannels::testNarrowRow16BitPerChannels(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing function narrowing 16 bit channels in row:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	Timestamp startTimestamp(true);

	do
	{
		allSucceeded = validateNarrowRow16BitPerChannels<1u>(randomGenerator) && allSucceeded;
		allSucceeded = validateNarrowRow16BitPerChannels<2u>(randomGenerator) && allSucceeded;
		allSucceeded = validateNarrowRow16BitPerChannels<3u>(randomGenerator) && allSucceeded;
		allSucceeded = validateNarrowRow16BitPerChannels<4u>(randomGenerator) && allSucceeded;
		allSucceeded = validateNarrowRow16BitPerChannels<5u>(randomGenerator) && allSucceeded;
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

template <typename TSource, typename TTarget, unsigned int tChannels>
void TestFrameChannels::testFunctionApplyAdvancedModifier(const TSource* source, TTarget* target)
{
	ocean_assert(source && target);

	for (unsigned int channel = 0u; channel < tChannels; ++channel)
	{
		// Arbitrary operation.
		target[channel] = source[channel] >> 2u;
	}
}

template <typename T>
bool TestFrameChannels::validateAddFirstChannel(const T* source, const T* sourceNewChannel, const T* target, const unsigned int sourceChannels, const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int sourceNewChannelPaddingElements, const unsigned int targetPaddingElements)
{
	ocean_assert(source != nullptr && sourceNewChannel != nullptr && target != nullptr);
	ocean_assert(sourceChannels >= 1u);
	ocean_assert(width != 0u && height != 0u);

	const unsigned int targetChannels = sourceChannels + 1u;

	const unsigned int sourceStrideElements = width * sourceChannels + sourcePaddingElements;
	const unsigned int sourceNewChannelStrideElements = width + sourceNewChannelPaddingElements;
	const unsigned int targetStrideElements = width * targetChannels + targetPaddingElements;

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			const T* const sourcePixel = source + y * sourceStrideElements + sourceChannels * x;
			const T* const newChannelPixel = sourceNewChannel + y * sourceNewChannelStrideElements + x;
			const T* targetPixel = nullptr;

			switch (flag)
			{
				case CV::FrameConverter::CONVERT_NORMAL:
					targetPixel = target + y * targetStrideElements + targetChannels * x;
					break;

				case CV::FrameConverter::CONVERT_FLIPPED:
					targetPixel = target + (height - y - 1u) * targetStrideElements + targetChannels * x;
					break;

				case CV::FrameConverter::CONVERT_MIRRORED:
					targetPixel = target + y * targetStrideElements + (width - x - 1u) * targetChannels;
					break;

				case CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED:
					targetPixel = target + (height - y - 1u) * targetStrideElements + (width - x - 1u) * targetChannels;
					break;

				default:
					ocean_assert(false && "Invalid conversion flag!");
					return false;
			}

			ocean_assert(targetPixel != nullptr);

			if (targetPixel[0] != newChannelPixel[0])
			{
				return false;
			}

			for (unsigned int n = 0u; n < sourceChannels; ++n)
			{
				if (sourcePixel[n] != targetPixel[n + 1u])
				{
					return false;
				}
			}
		}
	}

	return true;
}

template <typename T>
bool TestFrameChannels::validateAddFirstChannelValue(const T* source, const T& newChannelValue, const T* target, const unsigned int sourceChannels, const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceChannels >= 1u);
	ocean_assert(width != 0u && height != 0u);

	const unsigned int targetChannels = sourceChannels + 1u;

	const unsigned int sourceStrideElements = width * sourceChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * targetChannels + targetPaddingElements;

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			const T* const sourcePixel = source + y * sourceStrideElements + sourceChannels * x;
			const T* targetPixel = nullptr;

			switch (flag)
			{
				case CV::FrameConverter::CONVERT_NORMAL:
					targetPixel = target + y * targetStrideElements + targetChannels * x;
					break;

				case CV::FrameConverter::CONVERT_FLIPPED:
					targetPixel = target + (height - y - 1u) * targetStrideElements + targetChannels * x;
					break;

				case CV::FrameConverter::CONVERT_MIRRORED:
					targetPixel = target + y * targetStrideElements + (width - x - 1u) * targetChannels;
					break;

				case CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED:
					targetPixel = target + (height - y - 1u) * targetStrideElements + (width - x - 1u) * targetChannels;
					break;

				default:
					ocean_assert(false && "Invalid conversion flag!");
					return false;
			}

			ocean_assert(targetPixel != nullptr);

			if (targetPixel[0] != newChannelValue)
			{
				return false;
			}

			for (unsigned int n = 0u; n < sourceChannels; ++n)
			{
				if (sourcePixel[n] != targetPixel[n + 1u])
				{
					return false;
				}
			}
		}
	}

	return true;
}

template <typename T>
bool TestFrameChannels::validateAddLastChannel(const T* source, const T* sourceNewChannel, const T* target, const unsigned int sourceChannels, const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int sourceNewChannelPaddingElements, const unsigned int targetPaddingElements)
{
	ocean_assert(source != nullptr && sourceNewChannel != nullptr && target != nullptr);
	ocean_assert(sourceChannels >= 1u);
	ocean_assert(width != 0u && height != 0u);

	const unsigned int targetChannels = sourceChannels + 1u;

	const unsigned int sourceStrideElements = width * sourceChannels + sourcePaddingElements;
	const unsigned int sourceNewChannelStrideElements = width + sourceNewChannelPaddingElements;
	const unsigned int targetStrideElements = width * targetChannels + targetPaddingElements;

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			const T* const sourcePixel = source + y * sourceStrideElements + sourceChannels * x;
			const T* const newChannelPixel = sourceNewChannel + y * sourceNewChannelStrideElements + x;
			const T* targetPixel = nullptr;

			switch (flag)
			{
				case CV::FrameConverter::CONVERT_NORMAL:
					targetPixel = target + y * targetStrideElements + targetChannels * x;
					break;

				case CV::FrameConverter::CONVERT_FLIPPED:
					targetPixel = target + (height - y - 1u) * targetStrideElements + targetChannels * x;
					break;

				case CV::FrameConverter::CONVERT_MIRRORED:
					targetPixel = target + y * targetStrideElements + (width - x - 1u) * targetChannels;
					break;

				case CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED:
					targetPixel = target + (height - y - 1u) * targetStrideElements + (width - x - 1u) * targetChannels;
					break;

				default:
					ocean_assert(false && "Invalid conversion flag!");
					return false;
			}

			ocean_assert(targetPixel != nullptr);

			if (targetPixel[sourceChannels] != newChannelPixel[0])
			{
				return false;
			}

			for (unsigned int n = 0u; n < sourceChannels; ++n)
			{
				if (sourcePixel[n] != targetPixel[n])
				{
					return false;
				}
			}
		}
	}

	return true;
}

template <typename T>
bool TestFrameChannels::validateAddLastChannelValue(const T* source, const T& newChannelValue, const T* target, const unsigned int sourceChannels, const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceChannels >= 1u);
	ocean_assert(width != 0u && height != 0u);

	const unsigned int targetChannels = sourceChannels + 1u;

	const unsigned int sourceStrideElements = width * sourceChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * targetChannels + targetPaddingElements;

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			const T* const sourcePixel = source + y * sourceStrideElements + sourceChannels * x;
			const T* targetPixel = nullptr;

			switch (flag)
			{
				case CV::FrameConverter::CONVERT_NORMAL:
					targetPixel = target + y * targetStrideElements + targetChannels * x;
					break;

				case CV::FrameConverter::CONVERT_FLIPPED:
					targetPixel = target + (height - y - 1u) * targetStrideElements + targetChannels * x;
					break;

				case CV::FrameConverter::CONVERT_MIRRORED:
					targetPixel = target + y * targetStrideElements + (width - x - 1u) * targetChannels;
					break;

				case CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED:
					targetPixel = target + (height - y - 1u) * targetStrideElements + (width - x - 1u) * targetChannels;
					break;

				default:
					ocean_assert(false && "Invalid conversion flag!");
					return false;
			}

			ocean_assert(targetPixel != nullptr);

			if (targetPixel[sourceChannels] != newChannelValue)
			{
				return false;
			}

			for (unsigned int n = 0u; n < sourceChannels; ++n)
			{
				if (sourcePixel[n] != targetPixel[n])
				{
					return false;
				}
			}
		}
	}

	return true;
}

template <typename TElementType, unsigned int tChannels>
bool TestFrameChannels::validateTransformGeneric(const TElementType* source, const TElementType* testTarget, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const CV::FrameConverter::ConversionFlag flag)
{
	const TElementType* debugSourceStart = source;
	const TElementType* debugTestTargetStart = testTarget;

	const TElementType* debugSourceEnd = source + height * (width * tChannels + sourcePaddingElements);
	const TElementType* debugTestTargetEnd = testTarget + height * (width * tChannels + targetPaddingElements);

	OCEAN_SUPPRESS_UNUSED_WARNING(debugSourceEnd);
	OCEAN_SUPPRESS_UNUSED_WARNING(debugTestTargetEnd);

	if (flag == CV::FrameConverter::CONVERT_NORMAL)
	{
		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int x = 0u; x < width; ++x)
			{
				for (unsigned int c = 0u; c < tChannels; ++c)
				{
					ocean_assert_and_suppress_unused(source >= debugSourceStart && source < debugSourceEnd, debugSourceStart);
					ocean_assert_and_suppress_unused(testTarget >= debugTestTargetStart && testTarget < debugTestTargetEnd, debugTestTargetStart);

					if (*source != *testTarget)
					{
						return false;
					}

					source++;
					testTarget++;
				}
			}

			source += sourcePaddingElements;
			testTarget += targetPaddingElements;
		}

		return true;
	}
	else if (flag == CV::FrameConverter::CONVERT_FLIPPED)
	{
		for (unsigned int y = 0u; y < height; ++y)
		{
			const TElementType* sourceRow = source + y * (width * tChannels + sourcePaddingElements);
			const TElementType* testTargetRow = testTarget + (height - y - 1u) * (width * tChannels + targetPaddingElements);

			for (unsigned int x = 0u; x < width; ++x)
			{
				for (unsigned int c = 0u; c < tChannels; ++c)
				{
					ocean_assert(sourceRow >= debugSourceStart && sourceRow < debugSourceEnd);
					ocean_assert(testTargetRow >= debugTestTargetStart && testTargetRow < debugTestTargetEnd);

					if (*sourceRow != *testTargetRow)
					{
						return false;
					}

					sourceRow++;
					testTargetRow++;
				}
			}
		}

		return true;
	}
	else if (flag == CV::FrameConverter::CONVERT_MIRRORED)
	{
		for (unsigned int y = 0u; y < height; ++y)
		{
			const TElementType* sourceRow = source + y * (width * tChannels + sourcePaddingElements);
			const TElementType* testTargetRow = testTarget + y * (width * tChannels + targetPaddingElements) + (width * tChannels) - tChannels;

			for (unsigned int x = 0u; x < width; ++x)
			{
				for (unsigned int c = 0u; c < tChannels; ++c)
				{
					ocean_assert(sourceRow >= debugSourceStart && sourceRow < debugSourceEnd);
					ocean_assert(testTargetRow >= debugTestTargetStart && testTargetRow < debugTestTargetEnd);

					if (*sourceRow != *testTargetRow)
					{
						return false;
					}

					sourceRow++;
					testTargetRow++;
				}

				testTargetRow -= 2u * tChannels;
			}
		}

		return true;
	}
	else if (flag == CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED)
	{
		for (unsigned int y = 0u; y < height; ++y)
		{
			const TElementType* sourceRow = source + y * (width * tChannels + sourcePaddingElements);
			const TElementType* testTargetRow = testTarget + (height - y - 1u) * (width * tChannels + targetPaddingElements) + (width * tChannels) - tChannels;

			for (unsigned int x = 0u; x < width; ++x)
			{
				for (unsigned int c = 0u; c < tChannels; ++c)
				{
					ocean_assert(sourceRow >= debugSourceStart && sourceRow < debugSourceEnd);
					ocean_assert(testTargetRow >= debugTestTargetStart && testTargetRow < debugTestTargetEnd);

					if (*sourceRow != *testTargetRow)
					{
						return false;
					}

					sourceRow++;
					testTargetRow++;
				}

				testTargetRow -= 2u * tChannels;
			}
		}

		return true;
	}

	return false;
}

template <typename T>
bool TestFrameChannels::validateReverseChannelOrder(const T* source, const T* target, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const CV::FrameConverter::ConversionFlag flag)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width * channels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * channels + targetPaddingElements;

	for (unsigned int y = 0; y < height; ++y)
	{
		for (unsigned int x = 0; x < width; ++x)
		{
			const T* const targetPixel = target + y * targetStrideElements + x * channels;

			const T* sourcePixel = nullptr;

			switch (flag)
			{
				case CV::FrameConverter::CONVERT_NORMAL:
					sourcePixel = source + y * sourceStrideElements + channels * x;
					break;

				case CV::FrameConverter::CONVERT_FLIPPED:
					sourcePixel = source + (height - y - 1u) * sourceStrideElements + channels * x;
					break;

				case CV::FrameConverter::CONVERT_MIRRORED:
					sourcePixel = source + y * sourceStrideElements + channels * (width - x - 1u);
					break;

				case CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED:
					sourcePixel = source + (height - y - 1u) * sourceStrideElements + channels * (width - x - 1u);
					break;

				default:
					ocean_assert(false && "Not supported conversion flag.");
					return false;
			}

			for (unsigned int n = 0u; n < channels; ++n)
			{
				if (sourcePixel[n] != targetPixel[channels - n - 1u])
				{
					return false;
				}
			}
		}
	}

	return true;
}

template <typename T>
bool TestFrameChannels::validateRemoveFirstChannel(const T* source, const T* target, const unsigned int sourceChannels, const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceChannels >= 2u);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int targetChannels = sourceChannels - 1u;

	const unsigned int sourceStrideElements = width * sourceChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * targetChannels + targetPaddingElements;

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			const T* const sourcePixel = source + y * sourceStrideElements + sourceChannels * x;
			const T* targetPixel = nullptr;

			switch (flag)
			{
				case CV::FrameConverter::CONVERT_NORMAL:
					targetPixel = target + y * targetStrideElements + targetChannels * x;
					break;

				case CV::FrameConverter::CONVERT_FLIPPED:
					targetPixel = target + (height - y - 1u) * targetStrideElements + targetChannels * x;
					break;

				case CV::FrameConverter::CONVERT_MIRRORED:
					targetPixel = target + y * targetStrideElements + (width - x - 1u) * targetChannels;
					break;

				case CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED:
					targetPixel = target + (height - y - 1u) * targetStrideElements + (width - x - 1u) * targetChannels;
					break;

				default:
					ocean_assert(false && "Invalid conversion flag!");
					return false;
			}

			ocean_assert(targetPixel != nullptr);

			for (unsigned int n = 0u; n < targetChannels; ++n)
			{
				if (sourcePixel[n + 1u] != targetPixel[n])
				{
					return false;
				}
			}
		}
	}

	return true;
}

template <typename T>
bool TestFrameChannels::validateRemoveLastChannel(const T* source, const T* target, const unsigned int sourceChannels, const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceChannels >= 2u);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int targetChannels = sourceChannels - 1u;

	const unsigned int sourceStrideElements = width * sourceChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * targetChannels + targetPaddingElements;

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			const T* const sourcePixel = source + y * sourceStrideElements + sourceChannels * x;
			const T* targetPixel = nullptr;

			switch (flag)
			{
				case CV::FrameConverter::CONVERT_NORMAL:
					targetPixel = target + y * targetStrideElements + targetChannels * x;
					break;

				case CV::FrameConverter::CONVERT_FLIPPED:
					targetPixel = target + (height - y - 1u) * targetStrideElements + targetChannels * x;
					break;

				case CV::FrameConverter::CONVERT_MIRRORED:
					targetPixel = target + y * targetStrideElements + (width - x - 1u) * targetChannels;
					break;

				case CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED:
					targetPixel = target + (height - y - 1u) * targetStrideElements + (width - x - 1u) * targetChannels;
					break;

				default:
					ocean_assert(false && "Invalid conversion flag!");
					return false;
			}

			ocean_assert(targetPixel != nullptr);

			for (unsigned int n = 0u; n < targetChannels; ++n)
			{
				if (sourcePixel[n] != targetPixel[n])
				{
					return false;
				}
			}
		}
	}

	return true;
}

template <typename T>
bool TestFrameChannels::validateCopyChannel(const T* source, const T* originalTarget, const T* target, const unsigned int sourceChannels, const unsigned int targetChannels, const unsigned int sourceChannelIndex, const unsigned int targetChannelIndex, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceChannels >= 1u && targetChannels >= 1u);
	ocean_assert(sourceChannelIndex < sourceChannels && targetChannelIndex < targetChannels);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width * sourceChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * targetChannels + targetPaddingElements;

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			const T* const sourcePixel = source + y * sourceStrideElements + sourceChannels * x;
			const T* targetPixel = target + y * targetStrideElements + targetChannels * x;
			const T* originalTargetPixel = originalTarget + y * targetStrideElements + targetChannels * x;

			for (unsigned int n = 0u; n < targetChannels; ++n)
			{
				if (n != targetChannelIndex)
				{
					if (originalTargetPixel[n] != targetPixel[n])
					{
						return false;
					}
				}
				else
				{
					if (sourcePixel[sourceChannelIndex] != targetPixel[n])
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

template <typename T>
bool TestFrameChannels::validateSetChannel(const T* originalFrame, const T* frame, const unsigned int width, const unsigned int height, const T value, const unsigned int channelIndex, const unsigned int channels, const unsigned int framePaddingElements)
{
	ocean_assert(originalFrame != nullptr && frame != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(channels >= 1u && channelIndex < channels);

	const unsigned int frameStrideElements = width * channels + framePaddingElements;

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			const T* const framePixel = frame + y * frameStrideElements + channels * x;
			const T* originalFramePixel = originalFrame + y * frameStrideElements + channels * x;

			for (unsigned int n = 0u; n < channels; ++n)
			{
				if (n != channelIndex)
				{
					if (originalFramePixel[n] != framePixel[n])
					{
						return false;
					}
				}
				else
				{
					if (framePixel[n] != value)
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

template <typename TElement, unsigned int tChannels, void (*tPixelFunction)(const TElement*, TElement*)>
bool TestFrameChannels::validateApplyAdvancedPixelModifier(const Frame& source, const Frame& validationTarget, const CV::FrameConverter::ConversionFlag conversionFlag)
{
	ocean_assert(source.isValid() && validationTarget.isValid());
	ocean_assert(source.frameType() == validationTarget.frameType());

	const unsigned int width = source.width();
	const unsigned int height = source.height();

	bool allSucceeded = true;

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			const TElement* sourcePixel = source.constpixel<TElement>(x, y);
			TElement targetPixel[tChannels];

			tPixelFunction(sourcePixel, targetPixel);

			const TElement* validationTargetPixel = nullptr;
			switch (conversionFlag)
			{
				case CV::FrameConverter::CONVERT_NORMAL:
					validationTargetPixel = validationTarget.constpixel<TElement>(x, y);
					break;

				case CV::FrameConverter::CONVERT_FLIPPED:
					validationTargetPixel = validationTarget.constpixel<TElement>(x, height - y - 1u);
					break;

				case CV::FrameConverter::CONVERT_MIRRORED:
					validationTargetPixel = validationTarget.constpixel<TElement>(width - x - 1u, y);
					break;

				case CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED:
					validationTargetPixel = validationTarget.constpixel<TElement>(width - x - 1u, height - y - 1u);
					break;
			}

			ocean_assert(validationTargetPixel != nullptr);

			for (unsigned int channel = 0u; channel < tChannels; ++channel)
			{
				if (validationTargetPixel[channel] != targetPixel[channel])
				{
					allSucceeded = false;
				}
			}
		}
	}

	return allSucceeded;
}

template <typename TSource0, typename TSource1, typename TTarget>
bool TestFrameChannels::validateApplyBivariateOperatorSubtract(const Frame& source0, const Frame& source1, const Frame& target, const CV::FrameChannels::ConversionFlag flag)
{
	ocean_assert(source0.isValid() && source0.numberPlanes() == 1u && FrameType::areFrameTypesCompatible(source0, source1, /* allowDifferentPixelOrigins */ false) && FrameType::areFrameTypesCompatible(source0, target, /* allowDifferentPixelOrigins */ false));

	const unsigned int width = source0.width();
	const unsigned int height = source0.height();

	const unsigned int channels = source0.channels();

	switch (flag)
	{
		case CV::FrameChannels::CONVERT_NORMAL:

			for (unsigned int y = 0u; y < height; ++y)
			{
				for (unsigned int x = 0u; x < width; ++x)
				{
					const TSource0* source0Pixel = source0.constpixel<TSource0>(x, y);
					const TSource1* source1Pixel = source1.constpixel<TSource1>(x, y);
					const TTarget* targetPixel = target.constpixel<TTarget>(x, y);

					for (unsigned int n = 0u; n < channels; ++n)
					{
						if (NumericT<TTarget>::isNotEqualEps((TTarget)(*source0Pixel - *source1Pixel) - *targetPixel))
						{
							return false;
						}
					}
				}
			}

			return true;

		case CV::FrameChannels::CONVERT_FLIPPED:

			for (unsigned int y = 0u; y < height; ++y)
			{
				for (unsigned int x = 0u; x < width; ++x)
				{
					const TSource0* source0Pixel = source0.constpixel<TSource0>(x, y);
					const TSource1* source1Pixel = source1.constpixel<TSource1>(x, y);
					const TTarget* targetPixel = target.constpixel<TTarget>(x, height - y - 1u);

					for (unsigned int n = 0u; n < channels; ++n)
					{
						if (NumericT<TTarget>::isNotEqualEps((TTarget)(*source0Pixel - *source1Pixel) - *targetPixel))
						{
							return false;
						}
					}
				}
			}

			return true;

		case CV::FrameChannels::CONVERT_MIRRORED:

			for (unsigned int y = 0u; y < height; ++y)
			{
				for (unsigned int x = 0u; x < width; ++x)
				{
					const TSource0* source0Pixel = source0.constpixel<TSource0>(x, y);
					const TSource1* source1Pixel = source1.constpixel<TSource1>(x, y);
					const TTarget* targetPixel = target.constpixel<TTarget>(width - x - 1u, y);

					for (unsigned int n = 0u; n < channels; ++n)
					{
						if (NumericT<TTarget>::isNotEqualEps((TTarget)(*source0Pixel - *source1Pixel) - *targetPixel))
						{
							return false;
						}
					}
				}
			}

			return true;

		case CV::FrameChannels::CONVERT_FLIPPED_AND_MIRRORED:

			for (unsigned int y = 0u; y < height; ++y)
			{
				for (unsigned int x = 0u; x < width; ++x)
				{
					const TSource0* source0Pixel = source0.constpixel<TSource0>(x, y);
					const TSource1* source1Pixel = source1.constpixel<TSource1>(x, y);
					const TTarget* targetPixel = target.constpixel<TTarget>(width - x - 1u, height - y - 1u);

					for (unsigned int n = 0u; n < channels; ++n)
					{
						if (NumericT<TTarget>::isNotEqualEps((TTarget)(*source0Pixel - *source1Pixel) - *targetPixel))
						{
							return false;
						}
					}
				}
			}

			return true;

		default:
			break;
	}

	return false;
}

template <typename T, unsigned int tChannels>
bool TestFrameChannels::validateReverseRowPixelOrder(RandomGenerator& randomGenerator)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	{
		// reverse from source to target

		const unsigned int size = RandomI::random(randomGenerator, 1u, 1920u);

		std::vector<T> pixels(size * tChannels);

		for (T& element : pixels)
		{
			element = T(RandomI::random(randomGenerator, 0u, 255u));
		}

		std::vector<T> reversedPixels(pixels.size() + 1);

		const T endValue = T(RandomI::random(randomGenerator, 0u, 255u));
		reversedPixels[pixels.size()] = endValue;

		CV::FrameChannels::reverseRowPixelOrder<T, tChannels>(pixels.data(), reversedPixels.data(), size_t(size));

		ocean_assert(reversedPixels[pixels.size()] == endValue); // memory overflow check
		if (reversedPixels[pixels.size()] != endValue)
		{
			return false;
		}

		for (unsigned int x = 0u; x < size; ++x)
		{
			const T* pixel = pixels.data() + x * tChannels;
			const T* reversedPixel = reversedPixels.data() + (size - x - 1u) * tChannels;

			for (unsigned int c = 0u; c < tChannels; ++c)
			{
				if (pixel[c] != reversedPixel[c])
				{
					return false;
				}
			}
		}
	}

	{
		// reverse in-place

		const unsigned int size = RandomI::random(randomGenerator, 1u, 1920u);

		std::vector<T> pixels(size * tChannels + 1u);

		for (T& element : pixels)
		{
			element = T(RandomI::random(randomGenerator, 0u, 255u));
		}

		const T endValue = T(RandomI::random(randomGenerator, 0u, 255u));
		pixels.back() = endValue;

		const std::vector<T> pixelsCopy(pixels);

		CV::FrameChannels::reverseRowPixelOrderInPlace<T, tChannels>(pixels.data(), size_t(size));

		ocean_assert(pixels.back() == endValue); // memory overflow check
		if (pixels.back() != endValue)
		{
			return false;
		}

		for (unsigned int x = 0u; x < size; ++x)
		{
			const T* pixel = pixelsCopy.data() + x * tChannels;
			const T* reversedPixel = pixels.data() + (size - x - 1u) * tChannels;

			for (unsigned int c = 0u; c < tChannels; ++c)
			{
				if (pixel[c] != reversedPixel[c])
				{
					return false;
				}
			}
		}
	}

	return true;
}

template <typename T, unsigned int tChannels>
bool TestFrameChannels::validateReverseRowChannelOrder(RandomGenerator& randomGenerator)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	const unsigned int size = RandomI::random(randomGenerator, 1u, 1920u);

	std::vector<T> pixels(size * tChannels);

	for (T& element : pixels)
	{
		element = T(RandomI::random(randomGenerator, 0u, 255u));
	}

	std::vector<T> reversedChannelPixels(pixels.size() + 1);

	const T endValue = T(RandomI::random(randomGenerator, 0u, 255u));
	reversedChannelPixels[pixels.size()] = endValue;

	CV::FrameChannels::reverseRowChannelOrder<T, tChannels>(pixels.data(), reversedChannelPixels.data(), size_t(size));

	ocean_assert(reversedChannelPixels[pixels.size()] == endValue);
	if (reversedChannelPixels[pixels.size()] != endValue)
	{
		return false;
	}

	for (unsigned int x = 0u; x < size; ++x)
	{
		const T* pixel = pixels.data() + x * tChannels;
		const T* reversedChannelPixel = reversedChannelPixels.data() + x * tChannels;

		for (unsigned int c = 0u; c < tChannels; ++c)
		{
			if (pixel[c] != reversedChannelPixel[tChannels - c - 1u])
			{
				return false;
			}
		}
	}

	return true;
}

template <typename T, unsigned int tSourceChannels, unsigned int tTargetChannels, unsigned int tShufflePattern>
bool TestFrameChannels::validateShuffleRowChannels(RandomGenerator& randomGenerator)
{
	static_assert(tSourceChannels >= 1u && tSourceChannels <= 8u, "Invalid channel number!");
	static_assert(tTargetChannels >= 1u && tTargetChannels <= 8u, "Invalid channel number!");

	static_assert(tSourceChannels != 1u || tTargetChannels != 1u, "Invalid channel number!");

	const unsigned int size = RandomI::random(randomGenerator, 1u, 1920u);

	std::vector<T> sourcePixels(size * tSourceChannels);

	for (T& sourceElement : sourcePixels)
	{
		sourceElement = T(RandomI::random(randomGenerator, 0u, 255u));
	}

	std::vector<T> targetPixels(size * tTargetChannels + 1);

	const T endValue = T(RandomI::random(randomGenerator, 0u, 255u));
	targetPixels.back() = endValue;

	CV::FrameChannels::shuffleRowChannels<T, tSourceChannels, tTargetChannels, tShufflePattern>(sourcePixels.data(), targetPixels.data(), size_t(size), nullptr);

	ocean_assert(targetPixels.back() == endValue);
	if (targetPixels.back() != endValue)
	{
		return false;
	}

	for (unsigned int x = 0u; x < size; ++x)
	{
		const T* sourcePixel = sourcePixels.data() + x * tSourceChannels;
		const T* targetPixel = targetPixels.data() + x * tTargetChannels;

		for (unsigned int cT = 0u; cT < tTargetChannels; ++cT)
		{
			const unsigned int cS = (tShufflePattern >> (cT * 4u)) & 0x0000000Fu;

			if (targetPixel[cT] != sourcePixel[cS])
			{
				return false;
			}
		}
	}

	return true;
}

template <typename T, unsigned int tSourceChannels, unsigned int tTargetChannels, unsigned int tShufflePattern>
bool TestFrameChannels::validateShuffleRowAndSetLastChannelValueChannels(RandomGenerator& randomGenerator)
{
	static_assert(tSourceChannels >= 1u && tSourceChannels <= 8u, "Invalid channel number!");
	static_assert(tTargetChannels >= 2u && tTargetChannels <= 8u, "Invalid channel number!");

	const unsigned int size = RandomI::random(randomGenerator, 1u, 1920u);

	std::vector<T> sourcePixels(size * tSourceChannels);

	for (T& sourceElement : sourcePixels)
	{
		sourceElement = T(RandomI::random(randomGenerator, 0u, 255u));
	}

	std::vector<T> targetPixels(size * tTargetChannels + 1);

	const T endValue = T(RandomI::random(randomGenerator, 0u, 255u));
	targetPixels.back() = endValue;

	const T lastChannelValue = T(RandomI::random(randomGenerator, 0u, 255u));

	CV::FrameChannels::shuffleRowChannelsAndSetLastChannelValue<T, tSourceChannels, tTargetChannels, tShufflePattern>(sourcePixels.data(), targetPixels.data(), size_t(size), &lastChannelValue);

	ocean_assert(targetPixels.back() == endValue);
	if (targetPixels.back() != endValue)
	{
		return false;
	}

	for (unsigned int x = 0u; x < size; ++x)
	{
		const T* sourcePixel = sourcePixels.data() + x * tSourceChannels;
		const T* targetPixel = targetPixels.data() + x * tTargetChannels;

		for (unsigned int cT = 0u; cT < tTargetChannels - 1u; ++cT)
		{
			const unsigned int cS = (tShufflePattern >> (cT * 4u)) & 0x0000000Fu;

			if (targetPixel[cT] != sourcePixel[cS])
			{
				return false;
			}
		}

		if (targetPixel[tTargetChannels - 1u] != lastChannelValue)
		{
			return false;
		}
	}

	return true;
}

template <unsigned int tChannels>
bool TestFrameChannels::validateNarrowRow16BitPerChannels(RandomGenerator& randomGenerator)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	const unsigned int size = RandomI::random(randomGenerator, 1u, 1920u);

	std::vector<unsigned short> sourcePixels(size * tChannels);

	for (unsigned short& sourceElement : sourcePixels)
	{
		sourceElement = (unsigned short)(RandomI::random(randomGenerator, 0xFFFFu));
	}

	std::vector<unsigned char> targetPixels(size * tChannels + 1);

	const unsigned char endValue = (unsigned char)(RandomI::random(randomGenerator, 0u, 255u));
	targetPixels.back() = endValue;

	CV::FrameChannels::narrowRow16BitPerChannelTo8BitPerChannel<tChannels>(sourcePixels.data(), targetPixels.data(), size_t(size), nullptr);

	ocean_assert(targetPixels.back() == endValue);
	if (targetPixels.back() != endValue)
	{
		return false;
	}

	for (unsigned int x = 0u; x < size; ++x)
	{
		const unsigned short* sourcePixel = sourcePixels.data() + x * tChannels;
		const unsigned char* targetPixel = targetPixels.data() + x * tChannels;

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			const unsigned int sourceValue = sourcePixel[n];
			const int targetValue = int(sourceValue / 256u);

			if (std::abs(int(targetPixel[n]) - targetValue) > 1)
			{
				return false;
			}
		}
	}

	return true;
}

}

}

}
