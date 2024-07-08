/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameFilterErosion.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterErosion.h"
#include "ocean/cv/MaskAnalyzer.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameFilterErosion::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 4u && height >= 4u && testDuration > 0.0);

	Log::info() << "---   Erosion filter test with frame size " << width << "x" << height << ":   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testShrinkMask4Neighbor(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testShrinkMask8Neighbor(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testShrinkMaskRandom8Neighbor(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = test8Bit4Neighbor(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = test8Bit8Neighbor(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = test8Bit24Neighbor(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = test8Bit(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Erosion filter test succeeded.";
	}
	else
	{
		Log::info() << "Erosion filter test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameFilterErosion, ShrinkMask4Neighbor_1Channel)
{
	EXPECT_TRUE((TestFrameFilterErosion::testShrinkMask4Neighbor<1u>(GTEST_TEST_DURATION)));
}

TEST(TestFrameFilterErosion, ShrinkMask4Neighbor_2Channels)
{
	EXPECT_TRUE((TestFrameFilterErosion::testShrinkMask4Neighbor<2u>(GTEST_TEST_DURATION)));
}

TEST(TestFrameFilterErosion, ShrinkMask4Neighbor_3Channels)
{
	EXPECT_TRUE((TestFrameFilterErosion::testShrinkMask4Neighbor<3u>(GTEST_TEST_DURATION)));
}

TEST(TestFrameFilterErosion, ShrinkMask4Neighbor_4Channels)
{
	EXPECT_TRUE((TestFrameFilterErosion::testShrinkMask4Neighbor<4u>(GTEST_TEST_DURATION)));
}


TEST(TestFrameFilterErosion, ShrinkMask8Neighbor_1Channel)
{
	EXPECT_TRUE((TestFrameFilterErosion::testShrinkMask8Neighbor<1u>(GTEST_TEST_DURATION)));
}

TEST(TestFrameFilterErosion, ShrinkMask8Neighbor_2Channels)
{
	EXPECT_TRUE((TestFrameFilterErosion::testShrinkMask8Neighbor<2u>(GTEST_TEST_DURATION)));
}

TEST(TestFrameFilterErosion, ShrinkMask8Neighbor_3Channels)
{
	EXPECT_TRUE((TestFrameFilterErosion::testShrinkMask8Neighbor<3u>(GTEST_TEST_DURATION)));
}

TEST(TestFrameFilterErosion, ShrinkMask8Neighbor_4Channels)
{
	EXPECT_TRUE((TestFrameFilterErosion::testShrinkMask8Neighbor<4u>(GTEST_TEST_DURATION)));
}


TEST(TestFrameFilterErosion, ShrinkMaskRandom8Neighbor_1Channel)
{
	EXPECT_TRUE((TestFrameFilterErosion::testShrinkMaskRandom8Neighbor<1u>(GTEST_TEST_DURATION)));
}

TEST(TestFrameFilterErosion, ShrinkMaskRandom8Neighbor_2Channels)
{
	EXPECT_TRUE((TestFrameFilterErosion::testShrinkMaskRandom8Neighbor<2u>(GTEST_TEST_DURATION)));
}

TEST(TestFrameFilterErosion, ShrinkMaskRandom8Neighbor_3Channels)
{
	EXPECT_TRUE((TestFrameFilterErosion::testShrinkMaskRandom8Neighbor<3u>(GTEST_TEST_DURATION)));
}

TEST(TestFrameFilterErosion, ShrinkMaskRandom8Neighbor_4Channels)
{
	EXPECT_TRUE((TestFrameFilterErosion::testShrinkMaskRandom8Neighbor<4u>(GTEST_TEST_DURATION)));
}


TEST(TestFrameFilterErosion, Filter8Bit4Neighbor_1920x1080u)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterErosion::test8Bit4Neighbor(1920u, 1080u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameFilterErosion, Filter8Bit8Neighbor_1920x1080u)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterErosion::test8Bit8Neighbor(1920u, 1080u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameFilterErosion, Filter8Bit24Neighbor_1920x1080u)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterErosion::test8Bit24Neighbor(1920u, 1080u, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameFilterErosion, Filter8Bit_1920x1080u)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterErosion::test8Bit(1920u, 1080u, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameFilterErosion::testShrinkMask4Neighbor(const double testDuration)
{
	Log::info() << "Testing mask shrinking with 4-neighborhood:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testShrinkMask4Neighbor<1u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testShrinkMask4Neighbor<2u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testShrinkMask4Neighbor<3u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testShrinkMask4Neighbor<4u>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Mask shrinking with 4-neighborhood succeeded.";
	}
	else
	{
		Log::info() << "Mask shrinking with 4-neighborhood FAILED!";
	}

	return allSucceeded;
}

template <unsigned int tChannels>
bool TestFrameFilterErosion::testShrinkMask4Neighbor(const double testDuration)
{
	Log::info() << "... with " << tChannels << " channels:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 2u, 320u);
		const unsigned int height = RandomI::random(randomGenerator, 2u, 240u);

		Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		constexpr uint8_t maskValue = 0x00u;
		Frame mask = CV::CVUtilities::randomizedBinaryMask(width, height, maskValue, &randomGenerator);

		Frame frameCopy(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
		Frame maskCopy(mask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		constexpr bool tUseRandomNoise = false;
		constexpr unsigned int randomNoise = 0u;

		CV::FrameFilterErosion::shrinkMask8BitPerChannel4Neighbor<tChannels, tUseRandomNoise>(frame.data<uint8_t>(), mask.data<uint8_t>(), width, height, frame.paddingElements(), mask.paddingElements(), randomNoise);

		if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, frameCopy))
		{
			ocean_assert(false && "Invalid padding memory!");
			return false;
		}

		if (!CV::CVUtilities::isPaddingMemoryIdentical(mask, maskCopy))
		{
			ocean_assert(false && "Invalid padding memory!");
			return false;
		}

		if (!validateShrinkMask4Neighbor(std::move(frameCopy), std::move(maskCopy), frame, mask))
		{
			allSucceeded = false;
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

bool TestFrameFilterErosion::testShrinkMask8Neighbor(const double testDuration)
{
	Log::info() << "Testing mask shrinking with 8-neighborhood:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testShrinkMask8Neighbor<1u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testShrinkMask8Neighbor<2u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testShrinkMask8Neighbor<3u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testShrinkMask8Neighbor<4u>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Mask shrinking with 8-neighborhood succeeded.";
	}
	else
	{
		Log::info() << "Mask shrinking with 8-neighborhood FAILED!";
	}

	return allSucceeded;
}

template <unsigned int tChannels>
bool TestFrameFilterErosion::testShrinkMask8Neighbor(const double testDuration)
{
	Log::info() << "... with " << tChannels << " channels:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 2u, 320u);
		const unsigned int height = RandomI::random(randomGenerator, 2u, 240u);

		Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		constexpr uint8_t maskValue = 0x00u;
		Frame mask = CV::CVUtilities::randomizedBinaryMask(width, height, maskValue, &randomGenerator);

		Frame frameCopy(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
		Frame maskCopy(mask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		constexpr bool tUseRandomNoise = false;
		constexpr unsigned int randomNoise = 0u;

		CV::FrameFilterErosion::shrinkMask8BitPerChannel8Neighbor<tChannels, tUseRandomNoise>(frame.data<uint8_t>(), mask.data<uint8_t>(), width, height, frame.paddingElements(), mask.paddingElements(), randomNoise);

		if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, frameCopy))
		{
			ocean_assert(false && "Invalid padding memory!");
			return false;
		}

		if (!CV::CVUtilities::isPaddingMemoryIdentical(mask, maskCopy))
		{
			ocean_assert(false && "Invalid padding memory!");
			return false;
		}

		if (!validateShrinkMask8Neighbor(std::move(frameCopy), std::move(maskCopy), frame, mask))
		{
			allSucceeded = false;
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

bool TestFrameFilterErosion::testShrinkMaskRandom8Neighbor(const double testDuration)
{
	Log::info() << "Testing random mask shrinking with 8-neighborhood:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testShrinkMaskRandom8Neighbor<1u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testShrinkMaskRandom8Neighbor<2u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testShrinkMaskRandom8Neighbor<3u>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testShrinkMaskRandom8Neighbor<4u>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Mask random shrinking with 8-neighborhood succeeded.";
	}
	else
	{
		Log::info() << "Mask random shrinking with 8-neighborhood FAILED!";
	}

	return allSucceeded;
}

template <unsigned int tChannels>
bool TestFrameFilterErosion::testShrinkMaskRandom8Neighbor(const double testDuration)
{
	Log::info() << "... with " << tChannels << " channels:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 2u, 320u);
		const unsigned int height = RandomI::random(randomGenerator, 2u, 240u);

		Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		constexpr uint8_t maskValue = 0x00u;
		Frame mask = CV::CVUtilities::randomizedBinaryMask(width, height, maskValue, &randomGenerator);

		Frame frameCopy(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
		Frame maskCopy(mask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		const unsigned int randomSeed = RandomI::random32(randomGenerator);

		constexpr unsigned int randomNoise = 3u;

		if (CV::FrameFilterErosion::Comfort::shrinkMaskRandom(frame, mask, CV::FrameFilterErosion::MF_SQUARE_3, randomNoise, randomSeed))
		{
			if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, frameCopy))
			{
				ocean_assert(false && "Invalid padding memory!");
				return false;
			}

			if (!CV::CVUtilities::isPaddingMemoryIdentical(mask, maskCopy))
			{
				ocean_assert(false && "Invalid padding memory!");
				return false;
			}

			if (!validateShrinkMaskRandom8Neighbor(std::move(frameCopy), std::move(maskCopy), frame, mask, randomNoise, randomSeed))
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
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

bool TestFrameFilterErosion::test8Bit4Neighbor(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 2u && height >= 2u);

	Log::info() << "Testing 8 bit binary erosion with cross kernel (diameter 3) for " << width << "x" << height << " image:";
	Log::info() << " ";

	bool allSucceeded = true;

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
			for (const bool performanceIteration : {true, false})
			{
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(2u, width);
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(2u, height);

				const uint8_t maskValue = uint8_t(RandomI::random(0u, 255u));

				const Frame mask = CV::CVUtilities::randomizedBinaryMask(testWidth, testHeight, maskValue);

				Frame target = CV::CVUtilities::randomizedFrame(mask.frameType());

				const Frame copyMask(mask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
				const Frame copyTarget(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				performance.startIf(performanceIteration);
					CV::FrameFilterErosion::filter1Channel8Bit4Neighbor(mask.constdata<uint8_t>(), target.data<uint8_t>(), mask.width(), mask.height(), maskValue, mask.paddingElements(), target.paddingElements(), useWorker);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(mask, copyMask) || !CV::CVUtilities::isPaddingMemoryIdentical(target, copyTarget))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (!validate8BitCrossKernel(mask.constdata<uint8_t>(), target.constdata<uint8_t>(), mask.width(), mask.height(), 3u, maskValue, mask.paddingElements(), target.paddingElements()))
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

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrameFilterErosion::test8Bit8Neighbor(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 2u && height >= 2u);

	Log::info() << "Testing 8 bit binary erosion with square kernel 3x3 for " << width << "x" << height << " image:";
	Log::info() << " ";

	bool allSucceeded = true;

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
			for (const bool performanceIteration : {true, false})
			{
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(2u, width);
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(2u, height);

				const uint8_t maskValue = uint8_t(RandomI::random(0u, 255u));

				const Frame mask = CV::CVUtilities::randomizedBinaryMask(testWidth, testHeight, maskValue);

				Frame target = CV::CVUtilities::randomizedFrame(mask.frameType());

				const Frame copyMask(mask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
				const Frame copyTarget(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				performance.startIf(performanceIteration);
					CV::FrameFilterErosion::filter1Channel8Bit8Neighbor(mask.constdata<uint8_t>(), target.data<uint8_t>(), mask.width(), mask.height(), maskValue, mask.paddingElements(), target.paddingElements(), useWorker);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(mask, copyMask) || !CV::CVUtilities::isPaddingMemoryIdentical(target, copyTarget))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (!validate8BitSquareKernel(mask.constdata<uint8_t>(), target.constdata<uint8_t>(), mask.width(), mask.height(), 3u, maskValue, mask.paddingElements(), target.paddingElements()))
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

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrameFilterErosion::test8Bit24Neighbor(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 4u && height >= 4u);

	Log::info() << "Testing 8 bit binary erosion with square kernel 5x5 for " << width << "x" << height << " image:";
	Log::info() << " ";

	bool allSucceeded = true;

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
			for (const bool performanceIteration : {true, false})
			{
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(4u, width);
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(4u, height);

				const uint8_t maskValue = uint8_t(RandomI::random(0u, 255u));

				const Frame mask = CV::CVUtilities::randomizedBinaryMask(testWidth, testHeight, maskValue);

				Frame target = CV::CVUtilities::randomizedFrame(mask.frameType());

				const Frame copyMask(mask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
				const Frame copyTarget(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				performance.startIf(performanceIteration);
					CV::FrameFilterErosion::filter1Channel8Bit24Neighbor(mask.constdata<uint8_t>(), target.data<uint8_t>(), mask.width(), mask.height(), maskValue, mask.paddingElements(), target.paddingElements(), useWorker);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(mask, copyMask) || !CV::CVUtilities::isPaddingMemoryIdentical(target, copyTarget))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (!validate8BitSquareKernel(mask.constdata<uint8_t>(), target.constdata<uint8_t>(), mask.width(), mask.height(), 5u, maskValue, mask.paddingElements(), target.paddingElements()))
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

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrameFilterErosion::test8Bit(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 2u && height >= 2u);

	Log::info() << "Testing 8 bit binary erosion with different kernels for " << width << "x" << height << " image:";

	bool allSucceeded = true;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;

		Timestamp startTimestamp(true);

		do
		{
			{
				// validating cross kernel (diameter 3) random resolutions

				const unsigned int randomWidth = RandomI::random(4u, width);
				const unsigned int randomHeight = RandomI::random(4u, height);

				const uint8_t maskValue = uint8_t(RandomI::random(0u, 255u));

				Frame mask = CV::CVUtilities::randomizedBinaryMask(randomWidth, randomHeight, maskValue);
				const Frame copyMask(mask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				const unsigned int iterations = RandomI::random(1u, 6u);

				CV::FrameFilterErosion::filter1Channel8Bit<CV::FrameFilterErosion::MF_CROSS_3>(mask.data<uint8_t>(), mask.width(), mask.height(), iterations, maskValue, mask.paddingElements(), useWorker);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(mask, copyMask))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				Frame validationMask(copyMask, Frame::ACM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA);
				Frame validationTarget(validationMask.frameType());

				for (unsigned int n = 0u; n < iterations; ++n)
				{
					CV::FrameFilterErosion::filter1Channel8Bit4Neighbor(validationMask.constdata<uint8_t>(), validationTarget.data<uint8_t>(), validationMask.width(), validationMask.height(), maskValue, validationMask.paddingElements(), validationTarget.paddingElements(), nullptr);
					std::swap(validationMask, validationTarget);
				}

				for (unsigned int y = 0u; y < mask.height(); ++y)
				{
					if (memcmp(mask.constrow<void>(y), validationMask.constrow<void>(y), mask.width()) != 0)
					{
						allSucceeded = false;
					}
				}
			}

			{
				// validating square kernel (3x3) random resolutions

				const unsigned int randomWidth = RandomI::random(4u, width);
				const unsigned int randomHeight = RandomI::random(4u, height);

				const uint8_t maskValue = uint8_t(RandomI::random(0u, 255u));

				Frame mask = CV::CVUtilities::randomizedBinaryMask(randomWidth, randomHeight, maskValue);
				const Frame copyMask(mask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				const unsigned int iterations = RandomI::random(1u, 6u);

				CV::FrameFilterErosion::filter1Channel8Bit<CV::FrameFilterErosion::MF_SQUARE_3>(mask.data<uint8_t>(), mask.width(), mask.height(), iterations, maskValue, mask.paddingElements(), useWorker);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(mask, copyMask))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				Frame validationMask(copyMask, Frame::ACM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA);
				Frame validationTarget(validationMask.frameType());

				for (unsigned int n = 0u; n < iterations; ++n)
				{
					CV::FrameFilterErosion::filter1Channel8Bit8Neighbor(validationMask.constdata<uint8_t>(), validationTarget.data<uint8_t>(), validationMask.width(), validationMask.height(), maskValue, validationMask.paddingElements(), validationTarget.paddingElements(), nullptr);
					std::swap(validationMask, validationTarget);
				}

				for (unsigned int y = 0u; y < mask.height(); ++y)
				{
					if (memcmp(mask.constrow<void>(y), validationMask.constrow<void>(y), mask.width()) != 0)
					{
						allSucceeded = false;
					}
				}
			}

			{
				// validating square kernel (5x5) random resolutions

				const unsigned int randomWidth = RandomI::random(4u, width);
				const unsigned int randomHeight = RandomI::random(4u, height);

				const uint8_t maskValue = uint8_t(RandomI::random(0u, 255u));

				Frame mask = CV::CVUtilities::randomizedBinaryMask(randomWidth, randomHeight, maskValue);
				const Frame copyMask(mask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				const unsigned int iterations = RandomI::random(1u, 6u);

				CV::FrameFilterErosion::filter1Channel8Bit<CV::FrameFilterErosion::MF_SQUARE_5>(mask.data<uint8_t>(), mask.width(), mask.height(), iterations, maskValue, mask.paddingElements(), useWorker);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(mask, copyMask))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				Frame validationMask(copyMask, Frame::ACM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA);
				Frame validationTarget(validationMask.frameType());

				for (unsigned int n = 0u; n < iterations; ++n)
				{
					CV::FrameFilterErosion::filter1Channel8Bit24Neighbor(validationMask.constdata<uint8_t>(), validationTarget.data<uint8_t>(), validationMask.width(), validationMask.height(), maskValue, validationMask.paddingElements(), validationTarget.paddingElements(), nullptr);
					std::swap(validationMask, validationTarget);
				}

				for (unsigned int y = 0u; y < mask.height(); ++y)
				{
					if (memcmp(mask.constrow<void>(y), validationMask.constrow<void>(y), mask.width()) != 0)
					{
						allSucceeded = false;
					}
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrameFilterErosion::validateShrinkMask4Neighbor(Frame&& frame, Frame&& mask, const Frame& resultFrame, const Frame& resultMask)
{
	ocean_assert(resultFrame.isValid() && resultMask.isValid() && frame.isValid() && mask.isValid());

	ocean_assert(FrameType(resultFrame, FrameType::FORMAT_Y8) == resultMask.frameType());
	ocean_assert(resultFrame.frameType() == frame.frameType());
	ocean_assert(resultMask.frameType() == mask.frameType());

	if (!frame.isValid() || !mask.isFrameTypeCompatible(FrameType(frame, FrameType::FORMAT_Y8), false))
	{
		return false;
	}

	if (!frame.isFrameTypeCompatible(resultFrame, false) || !mask.isFrameTypeCompatible(resultMask, false))
	{
		return false;
	}

	const unsigned int width = frame.width();
	const unsigned int height = frame.height();
	const unsigned int channels = frame.channels();

	for (unsigned int y = 0u; y < mask.height(); ++y)
	{
		const uint8_t* resultFrameRow = resultFrame.constrow<uint8_t>(y);
		uint8_t* frameRow = frame.row<uint8_t>(y);

		const uint8_t* maskRow = mask.constrow<uint8_t>(y);

		for (unsigned int x = 0u; x < width; ++x)
		{
			if (*maskRow == 0x00)
			{
				for (unsigned int n = 0u; n < channels; ++n)
				{
					frameRow[n] = 0u;
				}
			}
			else
			{
				for (unsigned int n = 0u; n < channels; ++n)
				{
					if (frameRow[n] != resultFrameRow[n])
					{
						return false;
					}
				}
			}
		}

		resultFrameRow += channels;
		frameRow += channels;

		++maskRow;
	}

	bool oneFurtherIteration = true;

	while (oneFurtherIteration)
	{
		oneFurtherIteration = false;

		Frame maskCopy(mask, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int x = 0u; x < width; ++x)
			{
				if (mask.constpixel<uint8_t>(x, y)[0] == 0x00)
				{
					unsigned int weight = 0u;
					std::vector<unsigned int> pixel(channels, 0u);

					// top
					if (y >= 1u && mask.constpixel<uint8_t>(x, y - 1u)[0] == 0xFF)
					{
						weight++;

						for (unsigned int n = 0u; n < channels; ++n)
						{
							pixel[n] += frame.constpixel<uint8_t>(x, y - 1u)[n];
						}
					}

					// left
					if (x >= 1u && mask.constpixel<uint8_t>(x - 1u, y)[0] == 0xFF)
					{
						weight++;

						for (unsigned int n = 0u; n < channels; ++n)
						{
							pixel[n] += frame.constpixel<uint8_t>(x - 1u, y)[n];
						}
					}

					// right
					if (x < width - 1u && mask.constpixel<uint8_t>(x + 1u, y)[0] == 0xFF)
					{
						weight++;

						for (unsigned int n = 0u; n < channels; ++n)
						{
							pixel[n] += frame.constpixel<uint8_t>(x + 1u, y)[n];
						}
					}

					// bottom
					if (y < height - 1u && mask.constpixel<uint8_t>(x, y + 1u)[0] == 0xFF)
					{
						weight++;

						for (unsigned int n = 0u; n < channels; ++n)
						{
							pixel[n] += frame.constpixel<uint8_t>(x, y + 1u)[n];
						}
					}

					if (weight != 0u)
					{
						for (unsigned int n = 0u; n < channels; ++n)
						{
							frame.pixel<uint8_t>(x, y)[n] = uint8_t(pixel[n] / weight);

							if (frame.constpixel<uint8_t>(x, y)[n] != resultFrame.constpixel<uint8_t>(x, y)[n])
							{
								return false;
							}
						}

						maskCopy.pixel<uint8_t>(x, y)[0] = 0xFF;
					}

					oneFurtherIteration = true;
				}
			}
		}

		mask.copy(0, 0, maskCopy);
	}

	for (unsigned int y = 0u; y < resultMask.height(); ++y)
	{
		const uint8_t* const resultMaskRow = resultMask.constrow<uint8_t>(y);

		for (unsigned int x = 0u; x < resultMask.width(); ++x)
		{
			if (resultMaskRow[x] != 0xFF)
			{
				return false;
			}
		}
	}

	for (unsigned int y = 0u; y < resultMask.height(); ++y)
	{
		if (memcmp(resultFrame.constrow<void>(y), frame.constrow<void>(y), frame.planeWidthBytes(0u)) != 0)
		{
			return false;
		}

		if (memcmp(resultMask.constrow<void>(y), mask.constrow<void>(y), mask.planeWidthBytes(0u)) != 0)
		{
			return false;
		}
	}

	return true;
}

bool TestFrameFilterErosion::validateShrinkMask8Neighbor(Frame&& frame, Frame&& mask, const Frame& resultFrame, const Frame& resultMask)
{
	ocean_assert(resultFrame.isValid() && resultMask.isValid() && frame.isValid() && mask.isValid());

	ocean_assert(FrameType(resultFrame, FrameType::FORMAT_Y8) == resultMask.frameType());
	ocean_assert(resultFrame.frameType() == frame.frameType());
	ocean_assert(resultMask.frameType() == mask.frameType());

	if (!frame.isValid() || !mask.isFrameTypeCompatible(FrameType(frame, FrameType::FORMAT_Y8), false))
	{
		return false;
	}

	if (!frame.isFrameTypeCompatible(resultFrame, false) || !mask.isFrameTypeCompatible(resultMask, false))
	{
		return false;
	}

	const unsigned int width = frame.width();
	const unsigned int height = frame.height();
	const unsigned int channels = frame.channels();

	for (unsigned int y = 0u; y < mask.height(); ++y)
	{
		const uint8_t* resultFrameRow = resultFrame.constrow<uint8_t>(y);
		uint8_t* frameRow = frame.row<uint8_t>(y);

		const uint8_t* maskRow = mask.constrow<uint8_t>(y);

		for (unsigned int x = 0u; x < width; ++x)
		{
			if (*maskRow == 0x00)
			{
				for (unsigned int n = 0u; n < channels; ++n)
				{
					frameRow[n] = 0u;
				}
			}
			else
			{
				for (unsigned int n = 0u; n < channels; ++n)
				{
					if (frameRow[n] != resultFrameRow[n])
					{
						return false;
					}
				}
			}
		}

		resultFrameRow += channels;
		frameRow += channels;

		++maskRow;
	}

	bool oneFurtherIteration = true;

	while (oneFurtherIteration)
	{
		oneFurtherIteration = false;

		Frame maskCopy(mask, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int x = 0u; x < width; ++x)
			{
				if (mask.constpixel<uint8_t>(x, y)[0] == 0x00)
				{
					unsigned int weight = 0u;
					std::vector<unsigned int> pixel(channels, 0u);

					for (int xx = -1; xx <= 1; ++xx)
					{
						for (int yy = -1; yy <= 1; ++yy)
						{
							if (xx == 0 && yy == 0)
							{
								// center pixel
								continue;
							}

							const unsigned int factor = (xx == 0 || yy == 0) ? 2u : 1u; // extra weight factor for 4-neighborhood

							const unsigned int xLocation = (unsigned int)(int(x) + xx);
							const unsigned int yLocation = (unsigned int)(int(y) + yy);

							if (xLocation < width && yLocation < height)
							{
								if (mask.constpixel<uint8_t>(xLocation, yLocation)[0] == 0xFF)
								{
									weight += factor;

									for (unsigned int n = 0u; n < channels; ++n)
									{
										pixel[n] += frame.constpixel<uint8_t>(xLocation, yLocation)[n] * factor;
									}
								}
							}
						}
					}

					if (weight != 0u)
					{
						for (unsigned int n = 0u; n < channels; ++n)
						{
							frame.pixel<uint8_t>(x, y)[n] = uint8_t(pixel[n] / weight);

							if (frame.constpixel<uint8_t>(x, y)[n] != resultFrame.constpixel<uint8_t>(x, y)[n])
							{
								return false;
							}
						}

						maskCopy.pixel<uint8_t>(x, y)[0] = 0xFF;
					}

					oneFurtherIteration = true;
				}
			}
		}

		mask.copy(0, 0, maskCopy);
	}

	for (unsigned int y = 0u; y < resultMask.height(); ++y)
	{
		const uint8_t* const resultMaskRow = resultMask.constrow<uint8_t>(y);

		for (unsigned int x = 0u; x < resultMask.width(); ++x)
		{
			if (resultMaskRow[x] != 0xFF)
			{
				return false;
			}
		}
	}

	for (unsigned int y = 0u; y < resultMask.height(); ++y)
	{
		if (memcmp(resultFrame.constrow<void>(y), frame.constrow<void>(y), frame.planeWidthBytes(0u)) != 0)
		{
			return false;
		}

		if (memcmp(resultMask.constrow<void>(y), mask.constrow<void>(y), mask.planeWidthBytes(0u)) != 0)
		{
			return false;
		}
	}

	return true;
}

bool TestFrameFilterErosion::validateShrinkMaskRandom8Neighbor(Frame&& frame, Frame&& mask, const Frame& resultFrame, const Frame& resultMask, const unsigned int randomNoise, const unsigned int randomSeed)
{
	ocean_assert(resultFrame.isValid() && resultMask.isValid() && frame.isValid() && mask.isValid());

	ocean_assert(FrameType(resultFrame, FrameType::FORMAT_Y8) == resultMask.frameType());
	ocean_assert(resultFrame.frameType() == frame.frameType());
	ocean_assert(resultMask.frameType() == mask.frameType());

	if (!frame.isValid() || !mask.isFrameTypeCompatible(FrameType(frame, FrameType::FORMAT_Y8), false))
	{
		return false;
	}

	if (!frame.isFrameTypeCompatible(resultFrame, false) || !mask.isFrameTypeCompatible(resultMask, false))
	{
		return false;
	}

	constexpr uint8_t maskValue = 0x00u;

	const unsigned int width = frame.width();
	const unsigned int channels = frame.channels();

	// ensuring that image content outside of mask is correct

	for (unsigned int y = 0u; y < mask.height(); ++y)
	{
		const uint8_t* resultFrameRow = resultFrame.constrow<uint8_t>(y);
		uint8_t* frameRow = frame.row<uint8_t>(y);

		const uint8_t* maskRow = mask.constrow<uint8_t>(y);

		for (unsigned int x = 0u; x < width; ++x)
		{
			if (*maskRow == maskValue)
			{
				for (unsigned int n = 0u; n < channels; ++n)
				{
					frameRow[n] = 0u;
				}
			}
			else
			{
				for (unsigned int n = 0u; n < channels; ++n)
				{
					if (frameRow[n] != resultFrameRow[n])
					{
						return false;
					}
				}
			}
		}

		resultFrameRow += channels;
		frameRow += channels;

		++maskRow;
	}

	// let's determine all mask border pixels

	CV::PixelPositions borderPixels;

	for (unsigned int y = 0u; y < mask.height(); ++y)
	{
		for (unsigned int x = 0u; x < mask.width(); ++x)
		{
			const CV::PixelPosition pixelPosition(x, y);

			if (isMaskBorderPixel(mask, pixelPosition, maskValue))
			{
				borderPixels.emplace_back(pixelPosition);
			}
		}
	}

	RandomGenerator randomGenerator(randomSeed);

	while (!borderPixels.empty())
	{
		// now, we randomly select one border pixel and erase this pixel

		const unsigned int randomIndex = RandomI::random(randomGenerator, (unsigned int)(borderPixels.size()) - 1u);

		const CV::PixelPosition pixelPosition = borderPixels[randomIndex];

		borderPixels[randomIndex] = borderPixels.back();
		borderPixels.pop_back();

		ocean_assert(mask.constpixel<uint8_t>(pixelPosition.x(), pixelPosition.y())[0] == maskValue);

		// let's determine the pixel value

		unsigned int sumWeight = 0u;
		Indices32 sumColors(channels, 0u);

		for (int yy = -1; yy <= 1; ++yy)
		{
			const unsigned int yPosition = (unsigned int)(int(pixelPosition.y()) + yy);

			if (yPosition < mask.height())
			{
				for (int xx = -1; xx <= 1; ++xx)
				{
					if (yy != 0 || xx != 0)
					{
						const unsigned int xPosition = (unsigned int)(int(pixelPosition.x()) + xx);

						if (xPosition < mask.width())
						{
							const unsigned int factor = (xx == 0 || yy == 0) ? 2u : 1u; // extra weight factor for 4-neighborhood

							const unsigned int weight = mask.constpixel<uint8_t>(xPosition, yPosition)[0] * factor;

							sumWeight += weight;

							const uint8_t* const pixel = frame.constpixel<uint8_t>(xPosition, yPosition);

							for (unsigned int n = 0u; n < channels; ++n)
							{
								sumColors[n] += weight * (unsigned int)(pixel[n]);
							}
						}
					}
				}
			}
		}

		ocean_assert(sumWeight != 0u);
		if (sumWeight == 0u)
		{
			return false;
		}

		const unsigned int sumWeight_2 = sumWeight / 2u;

		uint8_t* const pixel = frame.pixel<uint8_t>(pixelPosition.x(), pixelPosition.y());
		const uint8_t* const testPixel = resultFrame.constpixel<uint8_t>(pixelPosition.x(), pixelPosition.y());

		for (unsigned int n = 0u; n < channels; ++n)
		{
			const int value = minmax<int>(0, int((sumColors[n] + sumWeight_2) / sumWeight) + RandomI::random(randomGenerator, -int(randomNoise), int(randomNoise)), 255);

			pixel[n] = uint8_t(value);

			if (testPixel[n] != pixel[n]) // extra test here, testing the entire image below as well
			{
				return false;
			}
		}

		mask.pixel<uint8_t>(pixelPosition.x(), pixelPosition.y())[0] = 0xFFu - maskValue;

		// let's check whether we have new border pixels to be considered

		for (int yy = -1; yy <= 1; ++yy)
		{
			const unsigned int yPosition = (unsigned int)(int(pixelPosition.y()) + yy);

			if (yPosition < mask.height())
			{
				for (int xx = -1; xx <= 1; ++xx)
				{
					if (yy != 0 || xx != 0)
					{
						const unsigned int xPosition = (unsigned int)(int(pixelPosition.x()) + xx);

						if (xPosition < mask.width())
						{
							if (isMaskBorderPixel(mask, CV::PixelPosition(xPosition, yPosition), maskValue))
							{
								if (!hasElement(borderPixels, CV::PixelPosition(xPosition, yPosition)))
								{
									borderPixels.emplace_back(xPosition, yPosition);
								}
							}
						}
					}
				}
			}
		}
	}

	// the mask frame must not contain mask pixels anymore

	if (CV::MaskAnalyzer::hasValue(mask.constdata<uint8_t>(), mask.width(), mask.height(), maskValue, mask.paddingElements()))
	{
		return false;
	}

	for (unsigned int y = 0u; y < resultMask.height(); ++y)
	{
		const uint8_t* const resultMaskRow = resultMask.constrow<uint8_t>(y);

		for (unsigned int x = 0u; x < resultMask.width(); ++x)
		{
			if (resultMaskRow[x] != 0xFF)
			{
				return false;
			}
		}
	}

	for (unsigned int y = 0u; y < resultMask.height(); ++y)
	{
		if (memcmp(resultFrame.constrow<void>(y), frame.constrow<void>(y), frame.planeWidthBytes(0u)) != 0)
		{
			return false;
		}

		if (memcmp(resultMask.constrow<void>(y), mask.constrow<void>(y), mask.planeWidthBytes(0u)) != 0)
		{
			return false;
		}
	}

	return true;
}

bool TestFrameFilterErosion::validate8BitCrossKernel(const uint8_t* const mask, const uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int kernelSize, const uint8_t maskValue, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements)
{
	ocean_assert(mask != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	ocean_assert(kernelSize % 2u == 1u);
	const int kernelSize_2 = int(kernelSize / 2u);

	const uint8_t nonMaskValue = 0xFF - maskValue;

	const unsigned int maskStrideElements = width + maskPaddingElements;
	const unsigned int targetStrideElements = width + targetPaddingElements;

	for (unsigned int targetY = 0u; targetY < height; ++targetY)
	{
		for (unsigned int targetX = 0u; targetX < width; ++targetX)
		{
			bool foundNonMask = false;

			// we search for a non-mask pixel with a window with kernelSize x kernelSize

			for (int yy = -kernelSize_2; !foundNonMask && yy <= kernelSize_2; ++yy)
			{
				const unsigned int maskX = targetX;
				const unsigned int maskY = minmax(0, int(targetY) + yy, int(height) - 1);

				if (mask[maskY * maskStrideElements + maskX] != maskValue)
				{
					foundNonMask = true;
				}
			}

			for (int xx = -kernelSize_2; !foundNonMask && xx <= kernelSize_2; ++xx)
			{
				const unsigned int maskX = minmax(0, int(targetX) + xx, int(width) - 1);
				const unsigned int maskY = targetY;

				if (mask[maskY * maskStrideElements + maskX] != maskValue)
				{
					foundNonMask = true;
				}
			}

			if (foundNonMask)
			{
				if (target[targetY * targetStrideElements + targetX] != nonMaskValue)
				{
					return false;
				}
			}
			else
			{
				if (target[targetY * targetStrideElements + targetX] != maskValue)
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool TestFrameFilterErosion::validate8BitSquareKernel(const uint8_t* const mask, const uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int kernelSize, const uint8_t maskValue, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements)
{
	ocean_assert(mask != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	ocean_assert(kernelSize % 2u == 1u);
	const int kernelSize_2 = int(kernelSize / 2u);

	const uint8_t nonMaskValue = 0xFF - maskValue;

	const unsigned int maskStrideElements = width + maskPaddingElements;
	const unsigned int targetStrideElements = width + targetPaddingElements;

	for (unsigned int targetY = 0u; targetY < height; ++targetY)
	{
		for (unsigned int targetX = 0u; targetX < width; ++targetX)
		{
			bool foundNonMask = false;

			// we search for a non-mask pixel with a window with kernelSize x kernelSize

			for (int yy = -kernelSize_2; !foundNonMask && yy <= kernelSize_2; ++yy)
			{
				const unsigned int maskY = minmax(0, int(targetY) + yy, int(height) - 1);

				for (int xx = -kernelSize_2; !foundNonMask && xx <= kernelSize_2; ++xx)
				{
					const unsigned int maskX = minmax(0, int(targetX) + xx, int(width) - 1);

					if (mask[maskY * maskStrideElements + maskX] != maskValue)
					{
						foundNonMask = true;
					}
				}
			}

			if (foundNonMask)
			{
				if (target[targetY * targetStrideElements + targetX] != nonMaskValue)
				{
					return false;
				}
			}
			else
			{
				if (target[targetY * targetStrideElements + targetX] != maskValue)
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool TestFrameFilterErosion::isMaskBorderPixel(const Frame& mask, const CV::PixelPosition& pixelPosition, const uint8_t maskValue)
{
	ocean_assert(mask.isValid() && mask.isPixelFormatCompatible(FrameType::FORMAT_Y8));

	if (mask.constpixel<uint8_t>(pixelPosition.x(), pixelPosition.y())[0] == maskValue)
	{
		for (int yy = -1; yy <= 1; ++yy)
		{
			const unsigned int yPosition = (unsigned int)(int(pixelPosition.y()) + yy);

			if (yPosition < mask.height())
			{
				for (int xx = -1; xx <= 1; ++xx)
				{
					if (yy != 0 || xx != 0)
					{
						const unsigned int xPosition = (unsigned int)(int(pixelPosition.x()) + xx);

						if (xPosition < mask.width())
						{
							if (mask.constpixel<uint8_t>(xPosition, yPosition)[0] != maskValue)
							{
								return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
}

}

}

}
