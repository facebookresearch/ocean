/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testadvanced/TestAdvancedFrameFilterGaussian.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"

#include "ocean/cv/advanced/AdvancedFrameFilterGaussian.h"

#include "ocean/base/RandomI.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestAdvanced
{

bool TestAdvancedFrameFilterGaussian::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Advanced Gaussian blur test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testFilter<uint8_t>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFilter<float>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Advanced Gaussian blur test succeeded.";
	}
	else
	{
		Log::info() << "Advanced Gaussian blur test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestAdvancedFrameFilterGaussian, testFilter_uint8_3)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameFilterGaussian::testFilter<uint8_t, uint32_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedFrameFilterGaussian, testFilter_uint8_5)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameFilterGaussian::testFilter<uint8_t, uint32_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 5u, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedFrameFilterGaussian, testFilter_uint8_7)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameFilterGaussian::testFilter<uint8_t, uint32_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 7u, GTEST_TEST_DURATION, worker)));
}


TEST(TestAdvancedFrameFilterGaussian, testFilter_float_3)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameFilterGaussian::testFilter<float, float>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedFrameFilterGaussian, testFilter_float_5)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameFilterGaussian::testFilter<float, float>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 5u, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedFrameFilterGaussian, testFilter_float_7)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameFilterGaussian::testFilter<float, float>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 7u,  GTEST_TEST_DURATION, worker)));
}

#endif // OCEAN_USE_GTEST

template <typename T>
bool TestAdvancedFrameFilterGaussian::testFilter(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	typedef typename NextLargerTyper<T>::TypePerformance TFilter;

	Log::info() << "Testing advanced Gaussian filter with resolution " << width << "x" << height << " for element type '" << TypeNamer::name<T>() << "' with filter type '" << TypeNamer::name<TFilter>() << "':";
	Log::info() << " ";

	bool allSucceeded = true;

	for (const unsigned int filterSize : {3u, 5u, 7u})
	{
		allSucceeded = testFilter<T, TFilter>(width, height, filterSize, testDuration, worker) && allSucceeded;
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

template <typename T, typename TFilter>
bool TestAdvancedFrameFilterGaussian::testFilter(const unsigned int width, const unsigned int height, const unsigned int filterSize, const double testDuration, Worker& worker)
{
	ocean_assert(filterSize >= 1u && filterSize % 2u == 1u);

	const unsigned int filterSize_2 = filterSize / 2u;

	ocean_assert(width >= filterSize_2 && height >= filterSize_2);

	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int channels = 1u; channels <= 4u; ++channels)
	{
		Log::info() << "... with " << channels << " channels and a " << filterSize << " filter':";

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			Timestamp startTimestamp(true);

			do
			{
				for (const bool performanceIteration : {true, false})
				{
					const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, filterSize_2 + 1u, width);
					const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, filterSize_2 + 1u, height);

					const Frame source = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<T>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
					Frame target = CV::CVUtilities::randomizedFrame(source.frameType(), &randomGenerator);

					const Frame targetCopy(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					const uint8_t maskValue = uint8_t(RandomI::random(randomGenerator, 255u));

					const Frame sourceMask = CV::CVUtilities::randomizedBinaryMask(testWidth, testHeight, maskValue, &randomGenerator);

					const unsigned int targetMaskPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
					Frame targetMask(sourceMask.frameType(), targetMaskPaddingElements);

					const Frame targetMaskCopy(targetMask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.startIf(performanceIteration);
						if (!CV::Advanced::AdvancedFrameFilterGaussian::Comfort::filter(source, sourceMask, target, targetMask, filterSize, maskValue, useWorker))
						{
							allSucceeded = false;
						}
					performance.stopIf(performanceIteration);

					if (!CV::CVUtilities::isPaddingMemoryIdentical(target, targetCopy))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					if (!CV::CVUtilities::isPaddingMemoryIdentical(targetMask, targetMaskCopy))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					if (!validateFilter<T>(source, sourceMask, target, targetMask, filterSize, maskValue))
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

	return allSucceeded;
}

template <typename T>
bool TestAdvancedFrameFilterGaussian::validateFilter(const Frame& source, const Frame& sourceMask, const Frame& target, const Frame& targetMask, const unsigned int filterSize, const uint8_t maskValue)
{
	std::vector<float> floatFilters(filterSize);
	CV::Advanced::AdvancedFrameFilterGaussian::determineFilterFactors<float>(filterSize, floatFilters.data());

	return TestAdvancedFrameFilterSeparable::validateFilter<T>(source, sourceMask, target, targetMask, floatFilters, floatFilters, maskValue);
}

}

}

}

}
