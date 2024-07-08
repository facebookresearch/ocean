/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameFilterMax.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameFilterMax::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 51u && height >= 51u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Max filter test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testMax<uint8_t>(width, height, 1u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMax<uint8_t>(width, height, 3u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMax<float>(width, height, 1u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMax<float>(width, height, 3u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMaxInPlace<float>(width, height, 1u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMaxInPlace<float>(width, height, 3u, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Max filter test succeeded.";
	}
	else
	{
		Log::info() << "Max filter test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameFilterMax, Max_uint8_1Channel_3)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMax::testMax<uint8_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterMax, Max_uint8_1Channel_5)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMax::testMax<uint8_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, 5u, GTEST_TEST_DURATION, worker));
}

#ifndef OCEAN_DEBUG

TEST(TestFrameFilterMax, Max_uint8_1Channel_11)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMax::testMax<uint8_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT / 2u, 1u, 11u, GTEST_TEST_DURATION, worker));
}

#endif


TEST(TestFrameFilterMax, Max_uint8_3Channels_3)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMax::testMax<uint8_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterMax, Max_uint8_3Channels_5)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMax::testMax<uint8_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, 5u, GTEST_TEST_DURATION, worker));
}

#ifndef OCEAN_DEBUG

TEST(TestFrameFilterMax, Max_uint8_3Channels_11)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMax::testMax<uint8_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT / 2u, 3u, 11u, GTEST_TEST_DURATION, worker));
}

#endif


TEST(TestFrameFilterMax, Max_float_1Channel_3)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMax::testMax<float>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterMax, Max_float_1Channel_5)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMax::testMax<float>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, 5u, GTEST_TEST_DURATION, worker));
}

#ifndef OCEAN_DEBUG

TEST(TestFrameFilterMax, Max_float_1Channel_11)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMax::testMax<float>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, 11u, GTEST_TEST_DURATION, worker));
}

#endif


TEST(TestFrameFilterMax, Max_float_3Channels_3)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMax::testMax<float>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterMax, Max_float_3Channels_5)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMax::testMax<float>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, 5u, GTEST_TEST_DURATION, worker));
}

#ifndef OCEAN_DEBUG

TEST(TestFrameFilterMax, Max_float_3Channels_11)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMax::testMax<float>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, 11u, GTEST_TEST_DURATION, worker));
}

#endif


TEST(TestFrameFilterMax, MaxInPlace_uint8_1Channel_3)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMax::testMaxInPlace<uint8_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterMax, MaxInPlace_uint8_1Channel_5)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMax::testMaxInPlace<uint8_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, 5u, GTEST_TEST_DURATION, worker));
}

#ifndef OCEAN_DEBUG

TEST(TestFrameFilterMax, MaxInPlace_uint8_1Channel_11)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMax::testMaxInPlace<uint8_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, 11u, GTEST_TEST_DURATION, worker));
}

#endif


TEST(TestFrameFilterMax, MaxInPlace_uint8_3Channels_3)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMax::testMaxInPlace<uint8_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterMax, MaxInPlace_uint8_3Channels_5)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMax::testMaxInPlace<uint8_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, 5u, GTEST_TEST_DURATION, worker));
}

#ifndef OCEAN_DEBUG

TEST(TestFrameFilterMax, MaxInPlace_uint8_3Channels_11)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMax::testMaxInPlace<uint8_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, 11u, GTEST_TEST_DURATION, worker));
}

#endif


TEST(TestFrameFilterMax, MaxInPlace_float_1Channel_3)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMax::testMaxInPlace<float>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterMax, MaxInPlace_float_1Channel_5)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMax::testMaxInPlace<float>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, 5u, GTEST_TEST_DURATION, worker));
}

#ifndef OCEAN_DEBUG

TEST(TestFrameFilterMax, MaxInPlace_float_1Channel_11)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMax::testMaxInPlace<float>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, 11u, GTEST_TEST_DURATION, worker));
}

#endif


TEST(TestFrameFilterMax, MaxInPlace_float_3Channels_3)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMax::testMaxInPlace<float>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterMax, MaxInPlace_float_3Channels_5)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMax::testMaxInPlace<float>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, 5u, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

template <typename T>
bool TestFrameFilterMax::testMax(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 51u && height >= 51u);
	ocean_assert(channels >= 1u && channels <= 4u);
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	for (const unsigned int filterSize : {3u, 5u, 11u, 25u})
	{
		Log::info().newLine(filterSize != 3u);
		Log::info().newLine(filterSize != 3u);

		if (!testMax<T>(width, height, channels, filterSize, testDuration, worker))
		{
			allSucceeded = false;
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

template <typename T>
bool TestFrameFilterMax::testMax(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int filterSize, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 51u && height >= 51u);
	ocean_assert(channels >= 1u);
	ocean_assert(filterSize >= 1u && filterSize <= std::min(width, height) && filterSize % 2u == 1u);
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	RandomGenerator randomGenerator;

	Log::info() << "Testing frame size " << width << "x" << height << " with " << channels << " channels, data type '" << TypeNamer::name<T>() << "', and with filter size " << filterSize << ":";
	Log::info() << " ";

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		const Timestamp startTimestamp(true);

		do
		{
			for (const bool performanceIteration : {true, false})
			{
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, filterSize, 1024u);
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, filterSize, 1024u);

				const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<T>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
				Frame target = CV::CVUtilities::randomizedFrame(frame.frameType(), &randomGenerator);

				const Frame copyTarget(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				performance.startIf(performanceIteration);
					CV::FrameFilterMax::Comfort::filter(frame, target, filterSize, useWorker);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(target, copyTarget))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (!validateMax<T>(frame, target, filterSize))
				{
					allSucceeded = false;
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 1u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 1u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 1u) << "ms, first: " << String::toAString(performanceSinglecore.firstMseconds(), 1u) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms";
		Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";

		allSucceeded = false;
	}

	return allSucceeded;
}

template <typename T>
bool TestFrameFilterMax::testMaxInPlace(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 51u && height >= 51u);
	ocean_assert(channels >= 1u && channels <= 4u);
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	for (const unsigned int filterSize : {3u, 5u, 11u, 25u})
	{
		Log::info().newLine(filterSize != 3u);
		Log::info().newLine(filterSize != 3u);

		if (!testMaxInPlace<T>(width, height, channels, filterSize, testDuration, worker))
		{
			allSucceeded = false;
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

template <typename T>
bool TestFrameFilterMax::testMaxInPlace(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int filterSize, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 51u && height >= 51u);
	ocean_assert(channels >= 1u);
	ocean_assert(filterSize >= 1u && filterSize <= std::min(width, height) && filterSize % 2u == 1u);
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	RandomGenerator randomGenerator;

	Log::info() << "Testing frame size " << width << "x" << height << " with " << channels << " channels, data type '" << TypeNamer::name<T>() << "', and with filter size " << filterSize << ", in place:";
	Log::info() << " ";

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		const Timestamp startTimestamp(true);

		do
		{
			for (const bool performanceIteration : {true, false})
			{
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, filterSize, 1024u);
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, filterSize, 1024u);

				Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<T>(channels), FrameType::ORIGIN_UPPER_LEFT));

				const Frame copyFrame(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				performance.startIf(performanceIteration);
					switch (frame.channels())
					{
						case 1u:
							CV::FrameFilterMax::filter<T, 1u>(frame.data<T>(), frame.width(), frame.height(), frame.paddingElements(), filterSize, useWorker);
							break;

						case 2u:
							CV::FrameFilterMax::filter<T, 2u>(frame.data<T>(), frame.width(), frame.height(), frame.paddingElements(), filterSize, useWorker);
							break;

						case 3u:
							CV::FrameFilterMax::filter<T, 3u>(frame.data<T>(), frame.width(), frame.height(), frame.paddingElements(), filterSize, useWorker);
							break;

						case 4u:
							CV::FrameFilterMax::filter<T, 4u>(frame.data<T>(), frame.width(), frame.height(),frame.paddingElements(), filterSize, useWorker);
							break;

						default:
							ocean_assert(false && "Invalid channel number!");
							allSucceeded = false;
					}
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, copyFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (!validateMax<T>(copyFrame, frame, filterSize))
				{
					allSucceeded = false;
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 1u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 1u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 1u) << "ms, first: " << String::toAString(performanceSinglecore.firstMseconds(), 1u) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms";
		Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
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

template <typename T>
bool TestFrameFilterMax::validateMax(const Frame& frame, const Frame& result, const unsigned int filterSize)
{
	ocean_assert(frame.isValid() && result.isValid());
	ocean_assert(frame.isFrameTypeCompatible(result.frameType(), false));
	ocean_assert(frame.numberPlanes() == 1u);

	if (!frame.isValid() || !result.isValid() || !frame.isFrameTypeCompatible(result.frameType(), false) || frame.numberPlanes() != 1u)
	{
		return false;
	}

	ocean_assert(frame.width() >= filterSize && frame.height() >= filterSize);

	if (frame.width() < filterSize || frame.height() < filterSize)
	{
		return false;
	}

	ocean_assert(filterSize >= 3u && filterSize <= 51u);
	ocean_assert(filterSize % 2u == 1u);

	if (filterSize < 3u || filterSize > 51u || filterSize % 2u != 1u)
	{
		return false;
	}

	const unsigned int filterSize_2 = filterSize / 2u;

	for (unsigned int y = 0u; y < frame.height(); ++y)
	{
		for (unsigned int x = 0u; x < frame.width(); ++x)
		{
			std::vector<T> maxValues(frame.channels(), NumericT<T>::minValue());

			for (int yy = -int(filterSize_2); yy <= int(filterSize_2); ++yy)
			{
				const int yPosition = int(y) + yy;

				if (yPosition >= 0 && yPosition < int(frame.height()))
				{
					for (int xx = -int(filterSize_2); xx <= int(filterSize_2); ++xx)
					{
						const int xPosition = int(x) + xx;

						if (xPosition >= 0 && xPosition < int(frame.width()))
						{
							for (unsigned int n = 0u; n < frame.channels(); ++n)
							{
								maxValues[n] = std::max(maxValues[n], frame.constpixel<T>((unsigned int)(xPosition), (unsigned int)(yPosition))[n]);
							}
						}
					}
				}
			}

			for (unsigned int n = 0u; n < frame.channels(); ++n)
			{
				const T maxValue = maxValues[n];

				const T resultValue = result.constpixel<T>(x, y)[n];

				if (maxValue != resultValue)
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
