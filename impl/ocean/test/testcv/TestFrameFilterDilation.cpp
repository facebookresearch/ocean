/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameFilterDilation.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterDilation.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameFilterDilation::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 4u && height >= 4u && testDuration > 0.0);

	Log::info() << "---   Dilation filter test with frame size " << width << "x" << height << ":   ---";
	Log::info() << " ";

	bool allSucceeded = true;

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
		Log::info() << "Dilation filter test succeeded.";
	}
	else
	{
		Log::info() << "Dilation filter test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameFilterDilation, Filter8Bit4Neighbor_1920x1080u)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterDilation::test8Bit4Neighbor(1920u, 1080u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterDilation, Filter8Bit8Neighbor_1920x1080u)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterDilation::test8Bit8Neighbor(1920u, 1080u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterDilation, Filter8Bit24Neighbor_1920x1080u)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterDilation::test8Bit24Neighbor(1920u, 1080u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterDilation, Filter8Bit_1920x1080u)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterDilation::test8Bit(1920u, 1080u, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameFilterDilation::test8Bit4Neighbor(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 2u && height >= 2u);

	Log::info() << "Testing 8 bit binary dilation with cross kernel (diameter 3) for " << width << "x" << height << " image:";

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
			{
				// benchmarking

				const uint8_t maskValue = uint8_t(RandomI::random(0u, 255u));

				const Frame mask = CV::CVUtilities::randomizedBinaryMask(width, height, maskValue);

				Frame target = CV::CVUtilities::randomizedFrame(mask.frameType());

				const Frame copyMask(mask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
				const Frame copyTarget(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				performance.start();
					CV::FrameFilterDilation::filter1Channel8Bit4Neighbor(mask.constdata<uint8_t>(), target.data<uint8_t>(), mask.width(), mask.height(), maskValue, mask.paddingElements(), target.paddingElements(), useWorker);
				performance.stop();

				if (!validate8BitCrossKernel(mask.constdata<uint8_t>(), target.constdata<uint8_t>(), mask.width(), mask.height(), 3u, maskValue, mask.paddingElements(), target.paddingElements()))
				{
					allSucceeded = false;
				}

				if (!CV::CVUtilities::isPaddingMemoryIdentical(mask, copyMask) || !CV::CVUtilities::isPaddingMemoryIdentical(target, copyTarget))
				{
					allSucceeded = false;
				}
			}

			{
				// validating random resolutions

				const unsigned int randomWidth = RandomI::random(2u, width);
				const unsigned int randomHeight = RandomI::random(2u, height);

				const uint8_t maskValue = uint8_t(RandomI::random(0u, 255u));

				const Frame mask = CV::CVUtilities::randomizedBinaryMask(randomWidth, randomHeight, maskValue);

				Frame target = CV::CVUtilities::randomizedFrame(mask.frameType());

				const Frame copyMask(mask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
				const Frame copyTarget(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				CV::FrameFilterDilation::filter1Channel8Bit4Neighbor(mask.constdata<uint8_t>(), target.data<uint8_t>(), mask.width(), mask.height(), maskValue, mask.paddingElements(), target.paddingElements(), useWorker);

				if (!validate8BitCrossKernel(mask.constdata<uint8_t>(), target.constdata<uint8_t>(), mask.width(), mask.height(), 3u, maskValue, mask.paddingElements(), target.paddingElements()))
				{
					allSucceeded = false;
				}

				if (!CV::CVUtilities::isPaddingMemoryIdentical(mask, copyMask) || !CV::CVUtilities::isPaddingMemoryIdentical(target, copyTarget))
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

bool TestFrameFilterDilation::test8Bit8Neighbor(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 2u && height >= 2u);

	Log::info() << "Testing 8 bit binary dilation with kernel 3x3 for " << width << "x" << height << " image:";

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
			{
				// benchmarking

				const uint8_t maskValue = uint8_t(RandomI::random(0u, 255u));

				const Frame mask = CV::CVUtilities::randomizedBinaryMask(width, height, maskValue);

				Frame target = CV::CVUtilities::randomizedFrame(mask.frameType());

				const Frame copyMask(mask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
				const Frame copyTarget(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				performance.start();
					CV::FrameFilterDilation::filter1Channel8Bit8Neighbor(mask.constdata<uint8_t>(), target.data<uint8_t>(), mask.width(), mask.height(), maskValue, mask.paddingElements(), target.paddingElements(), useWorker);
				performance.stop();

				if (!validate8BitSquareKernel(mask.constdata<uint8_t>(), target.constdata<uint8_t>(), mask.width(), mask.height(), 3u, maskValue, mask.paddingElements(), target.paddingElements()))
				{
					allSucceeded = false;
				}

				if (!CV::CVUtilities::isPaddingMemoryIdentical(mask, copyMask) || !CV::CVUtilities::isPaddingMemoryIdentical(target, copyTarget))
				{
					allSucceeded = false;
				}
			}

			{
				// validating random resolutions

				const unsigned int randomWidth = RandomI::random(2u, width);
				const unsigned int randomHeight = RandomI::random(2u, height);

				const uint8_t maskValue = uint8_t(RandomI::random(0u, 255u));

				const Frame mask = CV::CVUtilities::randomizedBinaryMask(randomWidth, randomHeight, maskValue);

				Frame target = CV::CVUtilities::randomizedFrame(mask.frameType());

				const Frame copyMask(mask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
				const Frame copyTarget(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				CV::FrameFilterDilation::filter1Channel8Bit8Neighbor(mask.constdata<uint8_t>(), target.data<uint8_t>(), mask.width(), mask.height(), maskValue, mask.paddingElements(), target.paddingElements(), useWorker);

				if (!validate8BitSquareKernel(mask.constdata<uint8_t>(), target.constdata<uint8_t>(), mask.width(), mask.height(), 3u, maskValue, mask.paddingElements(), target.paddingElements()))
				{
					allSucceeded = false;
				}

				if (!CV::CVUtilities::isPaddingMemoryIdentical(mask, copyMask) || !CV::CVUtilities::isPaddingMemoryIdentical(target, copyTarget))
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

bool TestFrameFilterDilation::test8Bit24Neighbor(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 2u && height >= 2u);

	Log::info() << "Testing 8 bit binary dilation with kernel 5x5 for " << width << "x" << height << " image:";

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
			{
				// benchmarking

				const unsigned char maskValue = (unsigned char)(RandomI::random(0u, 255u));

				const Frame mask = CV::CVUtilities::randomizedBinaryMask(width, height, maskValue);

				Frame target = CV::CVUtilities::randomizedFrame(mask.frameType());

				const Frame copyMask(mask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
				const Frame copyTarget(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				performance.start();
					CV::FrameFilterDilation::filter1Channel8Bit24Neighbor(mask.constdata<uint8_t>(), target.data<uint8_t>(), mask.width(), mask.height(), maskValue, mask.paddingElements(), target.paddingElements(), useWorker);
				performance.stop();

				if (!validate8BitSquareKernel(mask.constdata<uint8_t>(), target.constdata<uint8_t>(), mask.width(), mask.height(), 5u, maskValue, mask.paddingElements(), target.paddingElements()))
				{
					allSucceeded = false;
				}

				if (!CV::CVUtilities::isPaddingMemoryIdentical(mask, copyMask) || !CV::CVUtilities::isPaddingMemoryIdentical(target, copyTarget))
				{
					allSucceeded = false;
				}
			}

			{
				// validating random resolutions

				const unsigned int randomWidth = RandomI::random(4u, width);
				const unsigned int randomHeight = RandomI::random(4u, height);

				const uint8_t maskValue = uint8_t(RandomI::random(0u, 255u));

				const Frame mask = CV::CVUtilities::randomizedBinaryMask(randomWidth, randomHeight, maskValue);

				Frame target = CV::CVUtilities::randomizedFrame(mask.frameType());

				const Frame copyMask(mask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
				const Frame copyTarget(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				CV::FrameFilterDilation::filter1Channel8Bit24Neighbor(mask.constdata<uint8_t>(), target.data<uint8_t>(), mask.width(), mask.height(), maskValue, mask.paddingElements(), target.paddingElements(), useWorker);

				if (!validate8BitSquareKernel(mask.constdata<uint8_t>(), target.constdata<uint8_t>(), mask.width(), mask.height(), 5u, maskValue, mask.paddingElements(), target.paddingElements()))
				{
					allSucceeded = false;
				}

				if (!CV::CVUtilities::isPaddingMemoryIdentical(mask, copyMask) || !CV::CVUtilities::isPaddingMemoryIdentical(target, copyTarget))
				{
					allSucceeded = false;
				}
			}
		} while (startTimestamp + testDuration > Timestamp(true));
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

bool TestFrameFilterDilation::test8Bit(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 4u && height >= 4u);

	Log::info() << "Testing 8 bit binary dilation with different kernels for " << width << "x" << height << " image:";

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

				const unsigned int randomWidth = RandomI::random(2u, width);
				const unsigned int randomHeight = RandomI::random(2u, height);

				const uint8_t maskValue = uint8_t(RandomI::random(0u, 255u));

				Frame mask = CV::CVUtilities::randomizedBinaryMask(randomWidth, randomHeight, maskValue);
				const Frame copyMask(mask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				const unsigned int iterations = RandomI::random(1u, 6u);

				CV::FrameFilterDilation::filter1Channel8Bit<CV::FrameFilterDilation::MF_CROSS_3>(mask.data<uint8_t>(), mask.width(), mask.height(), iterations, maskValue, mask.paddingElements(), useWorker);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(mask, copyMask))
				{
					allSucceeded = false;
				}

				Frame validationMask(copyMask, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
				Frame validationTarget(validationMask.frameType());

				for (unsigned int n = 0u; n < iterations; ++n)
				{
					CV::FrameFilterDilation::filter1Channel8Bit4Neighbor(validationMask.constdata<uint8_t>(), validationTarget.data<uint8_t>(), validationMask.width(), validationMask.height(), maskValue, validationMask.paddingElements(), validationTarget.paddingElements(), nullptr);
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

				const unsigned int randomWidth = RandomI::random(2u, width);
				const unsigned int randomHeight = RandomI::random(2u, height);

				const uint8_t maskValue = uint8_t(RandomI::random(0u, 255u));

				Frame mask = CV::CVUtilities::randomizedBinaryMask(randomWidth, randomHeight, maskValue);
				const Frame copyMask(mask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				const unsigned int iterations = RandomI::random(1u, 6u);

				CV::FrameFilterDilation::filter1Channel8Bit<CV::FrameFilterDilation::MF_SQUARE_3>(mask.data<uint8_t>(), mask.width(), mask.height(), iterations, maskValue, mask.paddingElements(), useWorker);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(mask, copyMask))
				{
					allSucceeded = false;
				}

				Frame validationMask(copyMask, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
				Frame validationTarget(validationMask.frameType());

				for (unsigned int n = 0u; n < iterations; ++n)
				{
					CV::FrameFilterDilation::filter1Channel8Bit8Neighbor(validationMask.constdata<uint8_t>(), validationTarget.data<uint8_t>(), validationMask.width(), validationMask.height(), maskValue, validationMask.paddingElements(), validationTarget.paddingElements(), nullptr);
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

				CV::FrameFilterDilation::filter1Channel8Bit<CV::FrameFilterDilation::MF_SQUARE_5>(mask.data<uint8_t>(), mask.width(), mask.height(), iterations, maskValue, mask.paddingElements(), useWorker);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(mask, copyMask))
				{
					allSucceeded = false;
				}

				Frame validationMask(copyMask, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
				Frame validationTarget(validationMask.frameType());

				for (unsigned int n = 0u; n < iterations; ++n)
				{
					CV::FrameFilterDilation::filter1Channel8Bit24Neighbor(validationMask.constdata<uint8_t>(), validationTarget.data<uint8_t>(), validationMask.width(), validationMask.height(), maskValue, validationMask.paddingElements(), validationTarget.paddingElements(), nullptr);
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

bool TestFrameFilterDilation::validate8BitCrossKernel(const unsigned char* const mask, const unsigned char* target, const unsigned int width, const unsigned int height, const unsigned int kernelSize, const unsigned char maskValue, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements)
{
	ocean_assert(mask != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	ocean_assert(kernelSize % 2u == 1u);
	const int kernelSize_2 = int(kernelSize / 2u);

	const unsigned char nonMaskValue = 0xFF - maskValue;

	const unsigned int maskStrideElements = width + maskPaddingElements;
	const unsigned int targetStrideElements = width + targetPaddingElements;

	for (unsigned int targetY = 0u; targetY < height; ++targetY)
	{
		for (unsigned int targetX = 0u; targetX < width; ++targetX)
		{
			bool foundMask = false;

			// we search for a non-mask pixel with a window with kernelSize x kernelSize

			for (int yy = -kernelSize_2; !foundMask && yy <= kernelSize_2; ++yy)
			{
				const unsigned int maskX = targetX;
				const unsigned int maskY = minmax(0, int(targetY) + yy, int(height) - 1);

				if (mask[maskY * maskStrideElements + maskX] == maskValue)
				{
					foundMask = true;
				}
			}

			for (int xx = -kernelSize_2; !foundMask && xx <= kernelSize_2; ++xx)
			{
				const unsigned int maskX = minmax(0, int(targetX) + xx, int(width) - 1);
				const unsigned int maskY = targetY;

				if (mask[maskY * maskStrideElements + maskX] == maskValue)
				{
					foundMask = true;
				}
			}

			if (foundMask)
			{
				if (target[targetY * targetStrideElements + targetX] != maskValue)
				{
					return false;
				}
			}
			else
			{
				if (target[targetY * targetStrideElements + targetX] != nonMaskValue)
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool TestFrameFilterDilation::validate8BitSquareKernel(const unsigned char* const mask, const unsigned char* target, const unsigned int width, const unsigned int height, const unsigned int kernelSize, const unsigned char maskValue, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements)
{
	ocean_assert(mask != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	ocean_assert(kernelSize % 2u == 1u);
	const int kernelSize_2 = int(kernelSize / 2u);

	const unsigned char nonMaskValue = 0xFF - maskValue;

	const unsigned int maskStrideElements = width + maskPaddingElements;
	const unsigned int targetStrideElements = width + targetPaddingElements;

	for (unsigned int targetY = 0u; targetY < height; ++targetY)
	{
		for (unsigned int targetX = 0u; targetX < width; ++targetX)
		{
			bool foundMask = false;

			// we search for a non-mask pixel with a window with kernelSize x kernelSize

			for (int yy = -kernelSize_2; !foundMask && yy <= kernelSize_2; ++yy)
			{
				const unsigned int maskY = minmax(0, int(targetY) + yy, int(height) - 1);

				for (int xx = -kernelSize_2; !foundMask && xx <= kernelSize_2; ++xx)
				{
					const unsigned int maskX = minmax(0, int(targetX) + xx, int(width) - 1);

					if (mask[maskY * maskStrideElements + maskX] == maskValue)
					{
						foundMask = true;
					}
				}
			}

			if (foundMask)
			{
				if (target[targetY * targetStrideElements + targetX] != maskValue)
				{
					return false;
				}
			}
			else
			{
				if (target[targetY * targetStrideElements + targetX] != nonMaskValue)
				{
					return false;
				}
			}
		}
	}

	return true;
}

}

}

}
