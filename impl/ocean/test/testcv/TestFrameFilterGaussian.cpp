/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameFilterGaussian.h"
#include "ocean/test/testcv/TestFrameFilterSeparable.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterGaussian.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameFilterGaussian::test(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Gaussian blur test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testFilterSizeSigmaConversion() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFilterFactors() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testExtremeDimensions(worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNormalDimensions(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testReusableMemory<uint8_t, uint32_t>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testReusableMemory<float, float>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testReusableMemoryComfort<uint8_t>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testReusableMemoryComfort<float>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testInplace<uint8_t, uint32_t>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testInplace<float, float>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Gaussian filter test succeeded.";
	}
	else
	{
		Log::info() << "Gaussian filter test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameFilterGaussian, FilterSizeSigmaConversion)
{
	EXPECT_TRUE(TestFrameFilterGaussian::testFilterSizeSigmaConversion());
}

TEST(TestFrameFilterGaussian, FilterFactors)
{
	EXPECT_TRUE(TestFrameFilterGaussian::testFilterFactors());
}

TEST(TestFrameFilterGaussian, ExtremeDimensions)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterGaussian::testExtremeDimensions(worker));
}


TEST(TestFrameFilterGaussian, Filter_uint8_uint32_1920x1080_1Channel_3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterGaussian::testFilter<uint8_t, uint32_t>(1920u, 1080u, 1u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterGaussian, Filter_uint8_uint32_1920x1080_2Channel_3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterGaussian::testFilter<uint8_t, uint32_t>(1920u, 1080u, 2u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterGaussian, Filter_uint8_uint32_1920x1080_3Channel_3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterGaussian::testFilter<uint8_t, uint32_t>(1920u, 1080u, 3u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterGaussian, Filter_uint8_uint32_1920x1080_4Channel_3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterGaussian::testFilter<uint8_t, uint32_t>(1920u, 1080u, 4u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterGaussian, Filter_uint8_uint32_1920x1080_1Channel_15)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterGaussian::testFilter<uint8_t, uint32_t>(1920u, 1080u, 1u, 15u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterGaussian, Filter_uint8_uint32_1920x1080_2Channel_15)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterGaussian::testFilter<uint8_t, uint32_t>(1920u, 1080u, 2u, 15u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterGaussian, Filter_uint8_uint32_1920x1080_3Channel_15)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterGaussian::testFilter<uint8_t, uint32_t>(1920u, 1080u, 3u, 15u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterGaussian, Filter_uint8_uint32_1920x1080_4Channel_15)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterGaussian::testFilter<uint8_t, uint32_t>(1920u, 1080u, 4u, 15u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameFilterGaussian, Filter_float_float_1920x1080_1Channel_3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterGaussian::testFilter<float, float>(1920u, 1080u, 1u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterGaussian, Filter_float_float_1920x1080_2Channel_3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterGaussian::testFilter<float, float>(1920u, 1080u, 2u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterGaussian, Filter_float_float1920x1080_3Channel_3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterGaussian::testFilter<float, float>(1920u, 1080u, 3u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterGaussian, Filter_float_float_1920x1080_4Channel_3)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterGaussian::testFilter<float, float>(1920u, 1080u, 4u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterGaussian, Filter_float_float_1920x1080_1Channel_15)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterGaussian::testFilter<float, float>(1920u, 1080u, 1u, 15u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterGaussian, Filter_float_float_1920x1080_2Channel_15)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterGaussian::testFilter<float, float>(1920u, 1080u, 2u, 15u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterGaussian, Filter_float_float_1920x1080_3Channel_15)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterGaussian::testFilter<float, float>(1920u, 1080u, 3u, 15u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterGaussian, Filter_float_float_1920x1080_4Channel_15)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterGaussian::testFilter<float, float>(1920u, 1080u, 4u, 15u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameFilterGaussian, ReusableMemory_uint8)
{
	EXPECT_TRUE((TestFrameFilterGaussian::testReusableMemory<uint8_t, uint32_t>(GTEST_TEST_DURATION)));
}

TEST(TestFrameFilterGaussian, ReusableMemory_float)
{
	EXPECT_TRUE((TestFrameFilterGaussian::testReusableMemory<float, float>(GTEST_TEST_DURATION)));
}


TEST(TestFrameFilterGaussian, ReusableMemoryComfort_uint8)
{
	EXPECT_TRUE((TestFrameFilterGaussian::testReusableMemoryComfort<uint8_t>(GTEST_TEST_DURATION)));
}

TEST(TestFrameFilterGaussian, ReusableMemoryComfort_float)
{
	EXPECT_TRUE((TestFrameFilterGaussian::testReusableMemoryComfort<float>(GTEST_TEST_DURATION)));
}


TEST(TestFrameFilterGaussian, Inplace_uint8)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterGaussian::testInplace<uint8_t, uint32_t>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterGaussian, Inplace_float)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterGaussian::testInplace<float, float>(GTEST_TEST_DURATION, worker)));
}

#endif // OCEAN_USE_GTEST

bool TestFrameFilterGaussian::testFilterSizeSigmaConversion()
{
	Log::info() << "Testing conversion between filter size and sigma:";

	bool allSucceeded = true;

	for (unsigned int n = 1u; n <= 4321u; n += 2u)
	{
		const Scalar sigma = CV::FrameFilterGaussian::filterSize2sigma<Scalar>(n);

		const unsigned int filterSize = CV::FrameFilterGaussian::sigma2filterSize(sigma);

		if (filterSize != n)
		{
			allSucceeded = false;
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

bool TestFrameFilterGaussian::testFilterFactors()
{
	Log::info() << "Testing filter factors:";

	bool allSucceeded = true;

	const IndexGroups32 expectedFilterFactorGroups =
	{
		{},
		{1u},
		{},
		{1, 2u, 1u},
		{},
		{1u, 4u, 6u, 4u, 1u},
		{},
		{1u, 4u, 7u, 9u, 7u, 4u, 1u}
	};

	for (unsigned int filterSize = 1u; filterSize < 101u; filterSize += 2u)
	{
		{
			Indices32 integerFilter(filterSize + 1u);

			integerFilter.back() = RandomI::random32();
			const unsigned int lastElement = integerFilter.back();

			unsigned int normalization = RandomI::random32();
			CV::FrameFilterGaussian::determineFilterFactors(filterSize, integerFilter.data(), &normalization);

			if (lastElement != integerFilter.back())
			{
				ocean_assert(false && "Invalid padding elements!");
				return false;
			}

			unsigned int sumExpectedFilterFactors = 0u;

			for (unsigned int n = 0u; n < filterSize; ++n)
			{
				sumExpectedFilterFactors += integerFilter[n];
			}

			if (sumExpectedFilterFactors != normalization)
			{
				allSucceeded = false;
			}

			if (filterSize < expectedFilterFactorGroups.size())
			{
				const Indices32& expectedFilterFactors = expectedFilterFactorGroups[filterSize];

				if (expectedFilterFactors.size() == filterSize)
				{
					for (unsigned int n = 0u; n < filterSize; ++n)
					{
						if (expectedFilterFactors[n] != integerFilter[n])
						{
							allSucceeded = false;
						}
					}
				}
				else
				{
					allSucceeded = false;
				}
			}
			else
			{
				// just checking for a symmetric filter

				for (unsigned int n = 0u; n < filterSize / 2u; ++n)
				{
					if (integerFilter[n] != integerFilter[filterSize - n - 1u])
					{
						allSucceeded = false;
					}

					if (n >= 1u && integerFilter[n - 1u] > integerFilter[n])
					{
						allSucceeded = false;
					}
				}
			}
		}

		{
			std::vector<float> floatFilter(filterSize + 1u);

			floatFilter.back() = RandomF::scalar(-1000.0f, 1000.0f);
			const float lastElement = floatFilter.back();

			float normalization = RandomF::scalar(-1000.0f, 1000.0f);
			CV::FrameFilterGaussian::determineFilterFactors(filterSize, floatFilter.data(), &normalization);

			if (lastElement != floatFilter.back())
			{
				ocean_assert(false && "Invalid padding elements!");
				return false;
			}

			if (normalization != 1.0f)
			{
				allSucceeded = false;
			}

			float sumFilterFactors = 0.0;

			for (unsigned int n = 0u; n < filterSize; ++n)
			{
				sumFilterFactors += floatFilter[n];
			}

			if (NumericF::isNotEqual(sumFilterFactors, 1.0f))
			{
				allSucceeded = false;
			}

			// just checking for a symmetric filter

			for (unsigned int n = 0u; n < filterSize / 2u; ++n)
			{
				if (floatFilter[n] != floatFilter[filterSize - n - 1u])
				{
					allSucceeded = false;
				}

				if (n >= 1u && floatFilter[n - 1u] >= floatFilter[n])
				{
					allSucceeded = false;
				}
			}
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

bool TestFrameFilterGaussian::testExtremeDimensions(Worker& worker)
{
	Log::info() << "Testing extreme frame dimensions:";

	bool allSucceeded = true;

	for (unsigned int filterSize = 3u; filterSize <= 15u; filterSize += 2u)
	{
		for (unsigned int channels = 1u; channels <= 4u; ++channels)
		{
			allSucceeded = testExtremeDimensions<unsigned char, unsigned int>(channels, filterSize, worker) && allSucceeded;
			allSucceeded = testExtremeDimensions<float, float>(channels, filterSize, worker) && allSucceeded;
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
bool TestFrameFilterGaussian::testExtremeDimensions(const unsigned int channels, const unsigned int filterSize, Worker& worker)
{
	ocean_assert(filterSize % 2u == 1u);
	ocean_assert(channels >= 1u);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	const double averageErrorThreshold = std::is_same<TFilter, float>::value ? 0.1 : 2.0;
	const double maximalErrorThreshold = std::is_same<TFilter, float>::value ? 0.1 : 6.0;
#else
	const double averageErrorThreshold = std::is_same<TFilter, float>::value ? 0.1 : 2.0;
	const double maximalErrorThreshold = std::is_same<TFilter, float>::value ? 0.1 : 6.0;
#endif

	std::vector<float> normalizedFloatFilter(filterSize, 0.0f);
	CV::FrameFilterGaussian::determineFilterFactors(filterSize, normalizedFloatFilter.data());

	const ProcessorInstructions processorInstructions = Processor::get().instructions();

	const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<T>(channels);

	RandomGenerator randomGenerator;

	constexpr CV::FrameFilterGaussian::ReusableMemory* reusableMemory = nullptr;

	bool allSucceeded = true;

	for (unsigned int y = 0u; y < 15u; ++y)
	{
		for (unsigned int x = 0u; x < 15u; ++x)
		{
			for (const bool useWorker : {false, true})
			{
				const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(filterSize + x, filterSize + y, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
				Frame target = CV::CVUtilities::randomizedFrame(frame.frameType(), &randomGenerator);

				const Frame targetCopy(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				if (!CV::FrameFilterGaussian::filter<T, TFilter>(frame.constdata<T>(), target.data<T>(), frame.width(), frame.height(), frame.channels(), frame.paddingElements(), target.paddingElements(), filterSize, filterSize, -1.0f, useWorker ? &worker : nullptr, reusableMemory, processorInstructions))
				{
					allSucceeded = false;
				}

				if (!CV::CVUtilities::isPaddingMemoryIdentical(target, targetCopy))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				double averageAbsError = NumericD::maxValue();
				double maximalAbsError = NumericD::maxValue();
				TestFrameFilterSeparable::validateFilter<T>(frame.constdata<T>(), target.constdata<T>(), frame.width(), frame.height(), channels, normalizedFloatFilter, normalizedFloatFilter, &averageAbsError, &maximalAbsError, nullptr, frame.paddingElements(), target.paddingElements(), 0u);
				if (averageAbsError > averageErrorThreshold || maximalAbsError > maximalErrorThreshold)
				{
					allSucceeded = false;
				}
			}
		}
	}

	return allSucceeded;
}

bool TestFrameFilterGaussian::testNormalDimensions(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing normal frame dimensions:";
	Log::info() << " ";

	const Indices32 widths =  {640u, 800u, 1280u, 1281u, 1920u, 3840u};
	const Indices32 heights = {480u, 640u,  720u,  723u, 1080u, 2160u};
	ocean_assert(widths.size() == heights.size());

	bool allSucceeded = true;

	for (unsigned int n = 0u; n < widths.size(); ++n)
	{
		if (n != 0)
		{
			Log::info() << " ";
			Log::info() << "-";
			Log::info() << " ";
		}

		const unsigned int width = widths[n];
		const unsigned int height = heights[n];

		Log::info() << "Testing frame size " << width << "x" << height << ":";

		for (unsigned int filterSize = 3u; filterSize <= 15u; filterSize += 2u)
		{
			for (unsigned int channels = 1u; channels <= 4u; ++channels)
			{
				Log::info() << " ";
				Log::info() << "... with " << channels << " channels with filter kernel size " << filterSize << ":";
				Log::info() << " ";

				Log::info() << "... with data type 'unsigned char'";
				allSucceeded = testFilter<unsigned char, unsigned int>(width, height, channels, filterSize, testDuration, worker) && allSucceeded;

				Log::info() << " ";

				Log::info() << "... with data type 'float'";
				allSucceeded = testFilter<float, float>(width, height, channels, filterSize, testDuration, worker) && allSucceeded;
			}
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Normal frame dimension test succeeded.";
	}
	else
	{
		Log::info() << "Normal frame dimension test FAILED!";
	}

	return allSucceeded;
}

template <typename T, typename TFilter>
bool TestFrameFilterGaussian::testFilter(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int filterSize, const double testDuration, Worker& worker)
{
	ocean_assert(width >= filterSize && height >= filterSize);
	ocean_assert(testDuration > 0.0);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	constexpr double averageErrorThreshold = std::is_same<TFilter, float>::value ? 0.1 : 2.0;
	constexpr double maximalErrorThreshold = std::is_same<TFilter, float>::value ? 0.1 : 6.0;
#else
	constexpr double averageErrorThreshold = std::is_same<TFilter, float>::value ? 0.1 : 2.0;
	constexpr double maximalErrorThreshold = std::is_same<TFilter, float>::value ? 0.1 : 6.0;
#endif

	const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<T>(channels);

	const ProcessorInstructions processorInstructions = Processor::get().instructions();

	std::vector<float> normalizedFloatFilter(filterSize, 0.0f);
	CV::FrameFilterGaussian::determineFilterFactors(filterSize, normalizedFloatFilter.data());

	RandomGenerator randomGenerator;

	constexpr CV::FrameFilterGaussian::ReusableMemory* reusableMemory = nullptr;

	bool allSucceeded = true;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		const Timestamp startTimestamp(true);

		do
		{
			const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
			Frame target = CV::CVUtilities::randomizedFrame(frame.frameType(), &randomGenerator);

			const Frame targetCopy(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			performance.start();
				const bool localResult = CV::FrameFilterGaussian::filter<T, TFilter>(frame.constdata<T>(), target.data<T>(), frame.width(), frame.height(), frame.channels(), frame.paddingElements(), target.paddingElements(), filterSize, filterSize, -1.0f, useWorker, reusableMemory, processorInstructions);
			performance.stop();

			if (!CV::CVUtilities::isPaddingMemoryIdentical(target, targetCopy))
			{
				ocean_assert(false && "Invalid padding memory!");
				return false;
			}

			if (!localResult)
			{
				allSucceeded = false;
			}

			double averageAbsError = NumericD::maxValue();
			double maximalAbsError = NumericD::maxValue();
			TestFrameFilterSeparable::validateFilter<T>(frame.constdata<T>(), target.constdata<T>(), frame.width(), frame.height(), channels, normalizedFloatFilter, normalizedFloatFilter, &averageAbsError, &maximalAbsError, nullptr, frame.paddingElements(), target.paddingElements(), 0u);
			if (averageAbsError > averageErrorThreshold || maximalAbsError > maximalErrorThreshold)
			{
				allSucceeded = false;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Single-core performance: Best: " << performanceSinglecore.bestMseconds() << "ms, worst: " << performanceSinglecore.worstMseconds() << "ms, average: " << performanceSinglecore.averageMseconds() << ", median: " << performanceSinglecore.medianMseconds() << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multi-core performance: Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms, median: " << performanceMulticore.medianMseconds() << "ms";
		Log::info() << "Multi-core boost factor: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1) << "x, median: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 1) << "x";
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
bool TestFrameFilterGaussian::testReusableMemory(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int width = 1920u;
	constexpr unsigned int height = 1080u;

	Log::info() << "Testing reusable memory for an '" << TypeNamer::name<T>() << "' image with resolution " << width << "x" << height << ":";
	Log::info() << " ";

	const double averageErrorThreshold = std::is_same<TFilter, float>::value ? 0.1 : 2.0;
	const double maximalErrorThreshold = std::is_same<TFilter, float>::value ? 0.1 : 6.0;

	RandomGenerator randomGenerator;

	const ProcessorInstructions processorInstructions = Processor::get().instructions();

	bool allSucceeded = true;

	for (const unsigned int filterSize : {3u, 7u})
	{
		for (const unsigned int channels : {1u, 3u})
		{
			Log::info() << "... with " << channels << " channels and filter size " << filterSize << ":";

			std::vector<float> normalizedFloatFilter(filterSize, 0.0f);
			CV::FrameFilterGaussian::determineFilterFactors(filterSize, normalizedFloatFilter.data());

			HighPerformanceStatistic performanceStandard;
			HighPerformanceStatistic performanceReusableMemory;

			CV::FrameFilterGaussian::ReusableMemory reusableMemory;

			for (const bool useReusableMemory : {false, true})
			{
				HighPerformanceStatistic& performance = useReusableMemory ? performanceReusableMemory : performanceStandard;

				const Timestamp startTimestamp(true);

				do
				{
					const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<T>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
					Frame target = CV::CVUtilities::randomizedFrame(frame.frameType(), &randomGenerator);

					const Frame targetCopy(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.start();
						const bool localResult = CV::FrameFilterGaussian::filter<T, TFilter>(frame.constdata<T>(), target.data<T>(), frame.width(), frame.height(), frame.channels(), frame.paddingElements(), target.paddingElements(), filterSize, filterSize, -1.0f, nullptr, useReusableMemory ? &reusableMemory : nullptr, processorInstructions);
					performance.stop();

					if (!CV::CVUtilities::isPaddingMemoryIdentical(target, targetCopy))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					if (!localResult)
					{
						allSucceeded = false;
					}

					double averageAbsError = NumericD::maxValue();
					double maximalAbsError = NumericD::maxValue();
					TestFrameFilterSeparable::validateFilter<T>(frame.constdata<T>(), target.constdata<T>(), frame.width(), frame.height(), frame.channels(), normalizedFloatFilter, normalizedFloatFilter, &averageAbsError, &maximalAbsError, nullptr, frame.paddingElements(), target.paddingElements(), 0u);
					if (averageAbsError > averageErrorThreshold || maximalAbsError > maximalErrorThreshold)
					{
						allSucceeded = false;
					}
				}
				while (startTimestamp + testDuration > Timestamp(true));
			}

			Log::info() << "Standard performance: Best: " << performanceStandard.bestMseconds() << "ms, worst: " << performanceStandard.worstMseconds() << "ms, average: " << performanceStandard.averageMseconds() << ", median: " << performanceStandard.medianMseconds() << "ms";
			Log::info() << "Reusable memory performance: Best: " << performanceReusableMemory.bestMseconds() << "ms, worst: " << performanceReusableMemory.worstMseconds() << "ms, average: " << performanceReusableMemory.averageMseconds() << "ms, median: " << performanceReusableMemory.medianMseconds() << "ms";
			Log::info() << "Reusable memory boost factor: Best: " << String::toAString(performanceStandard.best() / performanceReusableMemory.best(), 2u) << "x, worst: " << String::toAString(performanceStandard.worst() / performanceReusableMemory.worst(), 2u) << "x, average: " << String::toAString(performanceStandard.average() / performanceReusableMemory.average(), 2u) << "x, median: " << String::toAString(performanceStandard.median() / performanceReusableMemory.median(), 2u) << "x";

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

template <typename T>
bool TestFrameFilterGaussian::testReusableMemoryComfort(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int width = 1920u;
	constexpr unsigned int height = 1080u;

	Log::info() << "Testing comfort reusable memory for 3 channels '" << TypeNamer::name<T>() << "' image with resolution " << width << "x" << height << ":";
	Log::info() << " ";

	const double averageErrorThreshold = std::is_same<T, float>::value ? 0.1 : 2.0;
	const double maximalErrorThreshold = std::is_same<T, float>::value ? 0.1 : 6.0;

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	for (const unsigned int filterSize : {3u, 7u})
	{
		for (const unsigned int channels : {1u, 3u})
		{
			Log::info() << "... with " << channels << " channels and filter size " << filterSize << ":";

			std::vector<float> normalizedFloatFilter(filterSize, 0.0f);
			CV::FrameFilterGaussian::determineFilterFactors(filterSize, normalizedFloatFilter.data());

			HighPerformanceStatistic performanceStandard;
			HighPerformanceStatistic performanceReusableMemory;

			CV::FrameFilterGaussian::ReusableMemory reusableMemory;

			for (const bool useReusableMemory : {false, true})
			{
				HighPerformanceStatistic& performance = useReusableMemory ? performanceReusableMemory : performanceStandard;

				const Timestamp startTimestamp(true);

				do
				{
					const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<T>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
					Frame target = CV::CVUtilities::randomizedFrame(frame.frameType(), &randomGenerator);

					const Frame targetCopy(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.start();
						const bool localResult = CV::FrameFilterGaussian::filter(frame, target, filterSize, nullptr, useReusableMemory ? &reusableMemory : nullptr);
					performance.stop();

					if (!CV::CVUtilities::isPaddingMemoryIdentical(target, targetCopy))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					if (!localResult)
					{
						allSucceeded = false;
					}

					double averageAbsError = NumericD::maxValue();
					double maximalAbsError = NumericD::maxValue();
					TestFrameFilterSeparable::validateFilter<T>(frame.constdata<T>(), target.constdata<T>(), frame.width(), frame.height(), frame.channels(), normalizedFloatFilter, normalizedFloatFilter, &averageAbsError, &maximalAbsError, nullptr, frame.paddingElements(), target.paddingElements(), 0u);
					if (averageAbsError > averageErrorThreshold || maximalAbsError > maximalErrorThreshold)
					{
						allSucceeded = false;
					}
				}
				while (startTimestamp + testDuration > Timestamp(true));
			}

			Log::info() << "Standard performance: Best: " << performanceStandard.bestMseconds() << "ms, worst: " << performanceStandard.worstMseconds() << "ms, average: " << performanceStandard.averageMseconds() << ", median: " << performanceStandard.medianMseconds() << "ms";
			Log::info() << "Reusable memory performance: Best: " << performanceReusableMemory.bestMseconds() << "ms, worst: " << performanceReusableMemory.worstMseconds() << "ms, average: " << performanceReusableMemory.averageMseconds() << "ms, median: " << performanceReusableMemory.medianMseconds() << "ms";
			Log::info() << "Reusable memory boost factor: Best: " << String::toAString(performanceStandard.best() / performanceReusableMemory.best(), 2u) << "x, worst: " << String::toAString(performanceStandard.worst() / performanceReusableMemory.worst(), 2u) << "x, average: " << String::toAString(performanceStandard.average() / performanceReusableMemory.average(), 2u) << "x, median: " << String::toAString(performanceStandard.median() / performanceReusableMemory.median(), 2u) << "x";

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
bool TestFrameFilterGaussian::testInplace(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing in-place filtering '" << TypeNamer::name<T>() << "':";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int filterSize = RandomI::random(randomGenerator, 3u, 15u) | 0x01u;

		const unsigned int width = RandomI::random(randomGenerator, filterSize, 200u);
		const unsigned int height = RandomI::random(randomGenerator, filterSize, 200u);

		const unsigned int channels = RandomI::random(randomGenerator, 1u, 4u);

		const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<T>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		Frame inplaceFrame(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
		Frame copyInplaceFrame(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		Frame targetFrame = CV::CVUtilities::randomizedFrame(frame.frameType(), &randomGenerator);
		const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		Worker* useWorkerA = Random::boolean(randomGenerator) ? &worker : nullptr;

		if (!CV::FrameFilterGaussian::filter<T, TFilter>(frame.constdata<T>(), targetFrame.data<T>(), frame.width(), frame.height(), frame.channels(), frame.paddingElements(), targetFrame.paddingElements(), filterSize, filterSize, -1.0f, useWorkerA))
		{
			allSucceeded = false;
		}

		if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
		{
			ocean_assert(false && "Invalid padding memory!");
			return false;
		}

		Worker* useWorkerB = Random::boolean(randomGenerator) ? &worker : nullptr;

		if (!CV::FrameFilterGaussian::filter<T, TFilter>(inplaceFrame.data<T>(), inplaceFrame.width(), inplaceFrame.height(), inplaceFrame.channels(), inplaceFrame.paddingElements(), filterSize, filterSize, -1.0f, useWorkerB))
		{
			allSucceeded = false;
		}

		if (!CV::CVUtilities::isPaddingMemoryIdentical(inplaceFrame, copyInplaceFrame))
		{
			ocean_assert(false && "Invalid padding memory!");
			return false;
		}

		for (unsigned int y = 0u; y < targetFrame.height(); ++y)
		{
			for (unsigned int x = 0u; x < targetFrame.width(); ++x)
			{
				const T* pixelA = targetFrame.constpixel<T>(x, y);
				const T* pixelB = inplaceFrame.constpixel<T>(x, y);

				for (unsigned int n = 0u; n < frame.channels(); ++n)
				{
					if (pixelA[n] != pixelB[n])
					{
						allSucceeded = false;
					}
				}
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

}

}

}
