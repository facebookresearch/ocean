/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testadvanced/TestAdvancedFrameFilterSeparable.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"

#include "ocean/cv/advanced/AdvancedFrameFilterSeparable.h"

#include "ocean/base/RandomI.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestAdvanced
{

bool TestAdvancedFrameFilterSeparable::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Advanced separable frame filter test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testFilter<uint8_t>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFilter<uint16_t>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFilter<float>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFilterInPlace<uint8_t>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFilterInPlace<uint16_t>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFilterInPlace<float>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Advanced separable frame filter test succeeded.";
	}
	else
	{
		Log::info() << "Advanced separable frame filter test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestAdvancedFrameFilterSeparable, testFilter_uint8_3x3)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameFilterSeparable::testFilter<uint8_t, uint32_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedFrameFilterSeparable, testFilter_uint8_5x5)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameFilterSeparable::testFilter<uint8_t, uint32_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 5u, 5u, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedFrameFilterSeparable, testFilter_uint8_7x7)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameFilterSeparable::testFilter<uint8_t, uint32_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 7u, 7u, GTEST_TEST_DURATION, worker)));
}


TEST(TestAdvancedFrameFilterSeparable, testFilter_uint16_3x3)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameFilterSeparable::testFilter<uint16_t, uint64_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedFrameFilterSeparable, testFilter_uint16_5x5)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameFilterSeparable::testFilter<uint16_t, uint64_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 5u, 5u, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedFrameFilterSeparable, testFilter_uint16_7x7)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameFilterSeparable::testFilter<uint16_t, uint64_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 7u, 7u, GTEST_TEST_DURATION, worker)));
}


TEST(TestAdvancedFrameFilterSeparable, testFilter_float_3x3)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameFilterSeparable::testFilter<float, float>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedFrameFilterSeparable, testFilter_float_5x5)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameFilterSeparable::testFilter<float, float>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 5u, 5u, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedFrameFilterSeparable, testFilter_float_7x7)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameFilterSeparable::testFilter<float, float>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 7u, 7u, GTEST_TEST_DURATION, worker)));
}



TEST(TestAdvancedFrameFilterSeparable, testFilterInPlace_uint8_3x3)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameFilterSeparable::testFilterInPlace<uint8_t, uint32_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedFrameFilterSeparable, testFilterInPlace_uint8_5x5)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameFilterSeparable::testFilterInPlace<uint8_t, uint32_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 5u, 5u, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedFrameFilterSeparable, testFilterInPlace_uint8_7x7)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameFilterSeparable::testFilterInPlace<uint8_t, uint32_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 7u, 7u, GTEST_TEST_DURATION, worker)));
}


TEST(TestAdvancedFrameFilterSeparable, testFilterInPlace_uint16_3x3)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameFilterSeparable::testFilterInPlace<uint16_t, uint64_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedFrameFilterSeparable, testFilterInPlace_uint16_5x5)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameFilterSeparable::testFilterInPlace<uint16_t, uint64_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 5u, 5u, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedFrameFilterSeparable, testFilterInPlace_uint16_7x7)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameFilterSeparable::testFilterInPlace<uint16_t, uint64_t>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 7u, 7u, GTEST_TEST_DURATION, worker)));
}


TEST(TestAdvancedFrameFilterSeparable, testFilterInPlace_float_3x3)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameFilterSeparable::testFilterInPlace<float, float>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedFrameFilterSeparable, testFilterInPlace_float_5x5)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameFilterSeparable::testFilterInPlace<float, float>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 5u, 5u, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedFrameFilterSeparable, testFilterInPlace_float_7x7)
{
	Worker worker;
	EXPECT_TRUE((TestAdvancedFrameFilterSeparable::testFilterInPlace<float, float>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 7u, 7u, GTEST_TEST_DURATION, worker)));
}

#endif // OCEAN_USE_GTEST

template <typename T>
bool TestAdvancedFrameFilterSeparable::testFilter(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	typedef typename NextLargerTyper<T>::TypePerformance TFilter;

	Log::info() << "Testing advanced filter with resolution " << width << "x" << height << " for element type '" << TypeNamer::name<T>() << "' with filter type '" << TypeNamer::name<TFilter>() << "':";
	Log::info() << " ";

	bool allSucceeded = true;

	for (const unsigned int horizontalFilterSize : {3u, 5u, 7u})
	{
		for (const unsigned int verticalFilterSize : {3u, 5u, 7u})
		{
			allSucceeded = testFilter<T, TFilter>(width, height, horizontalFilterSize, verticalFilterSize, testDuration, worker) && allSucceeded;
			Log::info() << " ";
		}
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
bool TestAdvancedFrameFilterSeparable::testFilter(const unsigned int width, const unsigned int height, const unsigned int horizontalFilterSize, const unsigned int verticalFilterSize, const double testDuration, Worker& worker)
{
	ocean_assert(horizontalFilterSize >= 1u && horizontalFilterSize % 2u == 1u);
	ocean_assert(verticalFilterSize >= 1u && verticalFilterSize % 2u == 1u);

	const unsigned int horizontalFilterSize_2 = horizontalFilterSize / 2u;
	const unsigned int verticalFilterSize_2 = verticalFilterSize / 2u;

	ocean_assert(width >= horizontalFilterSize_2 && height >= verticalFilterSize_2);

	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int channels = 1u; channels <= 4u; ++channels)
	{
		Log::info() << "... with " << channels << " channels and a " << horizontalFilterSize << "x" << verticalFilterSize << " filter':";

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
					const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, horizontalFilterSize_2 + 1u, width);
					const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, verticalFilterSize_2 + 1u, height);

					// we make symmetric filters

					std::vector<TFilter> horizontalFilters(horizontalFilterSize, TFilter(Random::random(randomGenerator, 16u)));

					for (unsigned int n = 0u; n < horizontalFilterSize_2; ++n)
					{
						const TFilter value = TFilter(Random::random(randomGenerator, 16u));

						horizontalFilters[n] = value;
						horizontalFilters[horizontalFilterSize - n - 1] = value;
					}

					std::vector<TFilter> verticalFilters(verticalFilterSize, TFilter(Random::random(randomGenerator, 16u)));

					for (unsigned int n = 0u; n < verticalFilterSize_2; ++n)
					{
						const TFilter value = TFilter(Random::random(randomGenerator, 16u));

						verticalFilters[n] = value;
						verticalFilters[verticalFilterSize - n - 1] = value;
					}

					const Frame source = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<T>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
					Frame target = CV::CVUtilities::randomizedFrame(source.frameType(), &randomGenerator);

					const Frame targetCopy(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					const uint8_t maskValue = uint8_t(RandomI::random(randomGenerator, 255u));

					const Frame sourceMask = CV::CVUtilities::randomizedBinaryMask(testWidth, testHeight, maskValue, &randomGenerator);

					const unsigned int targetMaskPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
					Frame targetMask(sourceMask.frameType(), targetMaskPaddingElements);

					const Frame targetMaskCopy(targetMask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.startIf(performanceIteration);
						CV::Advanced::AdvancedFrameFilterSeparable::filter<T, TFilter>(source.constdata<T>(), sourceMask.constdata<uint8_t>(), target.data<T>(), targetMask.data<uint8_t>(), source.width(), source.height(), source.channels(), source.paddingElements(), sourceMask.paddingElements(), target.paddingElements(), targetMask.paddingElements(), horizontalFilters.data(), horizontalFilterSize, verticalFilters.data(), verticalFilterSize, maskValue, useWorker);
					performance.stopIf(performanceIteration);

					if (!CV::CVUtilities::isPaddingMemoryIdentical(target, targetCopy))
					{
						ocean_assert(false && "Invalid padding memory!");
						allSucceeded = false;
						break;
					}

					if (!CV::CVUtilities::isPaddingMemoryIdentical(targetMask, targetMaskCopy))
					{
						ocean_assert(false && "Invalid padding memory!");
						allSucceeded = false;
						break;
					}

					std::vector<float> floatHorizontalFilters;
					std::vector<float> floatVerticalFilters;

					for (const TFilter& filterValue : horizontalFilters)
					{
						floatHorizontalFilters.emplace_back(float(filterValue));
					}

					for (const TFilter& filterValue : verticalFilters)
					{
						floatVerticalFilters.emplace_back(float(filterValue));
					}

					if (!validateFilter<T>(source, sourceMask, target, targetMask, floatHorizontalFilters, floatVerticalFilters, maskValue))
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
bool TestAdvancedFrameFilterSeparable::testFilterInPlace(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	typedef typename NextLargerTyper<T>::TypePerformance TFilter;

	Log::info() << "Testing advanced in-place filter with resolution " << width << "x" << height << " for element type '" << TypeNamer::name<T>() << "' with filter type '" << TypeNamer::name<TFilter>() << "':";
	Log::info() << " ";

	bool allSucceeded = true;

	for (const unsigned int horizontalFilterSize : {3u, 5u, 7u})
	{
		for (const unsigned int verticalFilterSize : {3u, 5u, 7u})
		{
			allSucceeded = testFilterInPlace<T, TFilter>(width, height, horizontalFilterSize, verticalFilterSize, testDuration, worker) && allSucceeded;
			Log::info() << " ";
		}
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
bool TestAdvancedFrameFilterSeparable::testFilterInPlace(const unsigned int width, const unsigned int height, const unsigned int horizontalFilterSize, const unsigned int verticalFilterSize, const double testDuration, Worker& worker)
{
	ocean_assert(horizontalFilterSize >= 1u && horizontalFilterSize % 2u == 1u);
	ocean_assert(verticalFilterSize >= 1u && verticalFilterSize % 2u == 1u);

	const unsigned int horizontalFilterSize_2 = horizontalFilterSize / 2u;
	const unsigned int verticalFilterSize_2 = verticalFilterSize / 2u;

	ocean_assert(width >= horizontalFilterSize_2 && height >= verticalFilterSize_2);

	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int channels = 1u; channels <= 4u; ++channels)
	{
		Log::info() << "... with " << channels << " channels and a " << horizontalFilterSize << "x" << verticalFilterSize << " filter':";

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
					const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, horizontalFilterSize_2 + 1u, width);
					const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, verticalFilterSize_2 + 1u, height);

					// we make symmetric filters

					std::vector<TFilter> horizontalFilters(horizontalFilterSize, TFilter(Random::random(randomGenerator, 16u)));

					for (unsigned int n = 0u; n < horizontalFilterSize_2; ++n)
					{
						const TFilter value = TFilter(Random::random(randomGenerator, 16u));

						horizontalFilters[n] = value;
						horizontalFilters[horizontalFilterSize - n - 1] = value;
					}

					std::vector<TFilter> verticalFilters(verticalFilterSize, TFilter(Random::random(randomGenerator, 16u)));

					for (unsigned int n = 0u; n < verticalFilterSize_2; ++n)
					{
						const TFilter value = TFilter(Random::random(randomGenerator, 16u));

						verticalFilters[n] = value;
						verticalFilters[verticalFilterSize - n - 1] = value;
					}

					Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<T>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

					const Frame frameCopy(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					const uint8_t maskValue = uint8_t(RandomI::random(randomGenerator, 255u));

					Frame mask = CV::CVUtilities::randomizedBinaryMask(testWidth, testHeight, maskValue, &randomGenerator);

					const Frame maskCopy(mask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.startIf(performanceIteration);
						CV::Advanced::AdvancedFrameFilterSeparable::filter<T, TFilter>(frame.data<T>(), mask.data<uint8_t>(), frame.width(), frame.height(), frame.channels(), frame.paddingElements(), mask.paddingElements(), horizontalFilters.data(), horizontalFilterSize, verticalFilters.data(), verticalFilterSize, maskValue, useWorker);
					performance.stopIf(performanceIteration);

					if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, frameCopy))
					{
						ocean_assert(false && "Invalid padding memory!");
						allSucceeded = false;
						break;
					}

					if (!CV::CVUtilities::isPaddingMemoryIdentical(mask, maskCopy))
					{
						ocean_assert(false && "Invalid padding memory!");
						allSucceeded = false;
						break;
					}

					std::vector<float> floatHorizontalFilters;
					std::vector<float> floatVerticalFilters;

					for (const TFilter& filterValue : horizontalFilters)
					{
						floatHorizontalFilters.emplace_back(float(filterValue));
					}

					for (const TFilter& filterValue : verticalFilters)
					{
						floatVerticalFilters.emplace_back(float(filterValue));
					}

					if (!validateFilter<T>(frameCopy, maskCopy, frame, mask, floatHorizontalFilters, floatVerticalFilters, maskValue))
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
bool TestAdvancedFrameFilterSeparable::validateFilter(const Frame& source, const Frame& sourceMask, const Frame& target, const Frame& targetMask, const std::vector<float>& horizontalFilters, const std::vector<float>& verticalFilters, const uint8_t maskValue)
{
	ocean_assert(source.isValid() && sourceMask.isValid() && target.isValid() && targetMask.isValid());

	if (!source.isFrameTypeCompatible(target, false))
	{
		return false;
	}

	if (horizontalFilters.empty() || horizontalFilters.size() % 2 != 1 || verticalFilters.empty() || verticalFilters.size() % 2 != 1)
	{
		return false;
	}

	const size_t horizontalFilters_2 = horizontalFilters.size() / 2;
	const size_t verticalFilters_2 = verticalFilters.size() / 2;

	for (unsigned int y = 0u; y < source.height(); ++y)
	{
		for (unsigned int x = 0u; x < source.width(); ++x)
		{
			std::vector<float> response(source.channels(), 0.0f);

			float factorSum = 0.0f;

			for (int yy = -int(verticalFilters_2); yy <= int(verticalFilters_2); ++yy)
			{
				const int yLocation = int(y) + yy;

				if (yLocation < 0 || yLocation >= int(source.height()))
				{
					continue;
				}

				const float verticalFactor = verticalFilters[yy + int(verticalFilters_2)];

				for (int xx = -int(horizontalFilters_2); xx <= int(horizontalFilters_2); ++xx)
				{
					const int xLocation = int(x) + xx;

					if (xLocation < 0 || xLocation >= int(source.width()))
					{
						continue;
					}

					const float horizontalFactor = horizontalFilters[xx + int(horizontalFilters_2)];

					if (sourceMask.constpixel<uint8_t>((unsigned int)(xLocation), (unsigned int)(yLocation))[0] != maskValue)
					{
						const T* sourcePixel = source.constpixel<T>((unsigned int)(xLocation), (unsigned int)(yLocation));

						const float factor = verticalFactor * horizontalFactor;

						for (unsigned int n = 0u; n < source.channels(); ++n)
						{
							response[n] += sourcePixel[n] * factor;
						}

						factorSum += factor;
					}
				}
			}

			const T* targetPixel = target.constpixel<T>(x, y);
			const uint8_t targetMaskValue = targetMask.constpixel<uint8_t>(x, y)[0];

			if (factorSum == 0.0f)
			{
				if (targetMaskValue != maskValue)
				{
					return false;
				}
			}
			else
			{
				const float invFactorSum = 1.0f / factorSum;

				for (unsigned int n = 0u; n < source.channels(); ++n)
				{
					const float responseValue = response[n] * invFactorSum;

					if (!Numeric::isEqual(float(targetPixel[n]), responseValue, 1.0f))
					{
						return false;
					}
				}

				if (targetMaskValue == maskValue)
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

}
