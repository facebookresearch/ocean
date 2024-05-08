/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameFilterSorted.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameFilterSorted::test(const double testDuration, Worker& /*worker*/)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Sorted filter test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testHistogram(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSortedElements(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Sorted filter test succeeded.";
	}
	else
	{
		Log::info() << "Sorted filter test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameFilterSorted, Histogram)
{
	EXPECT_TRUE(TestFrameFilterSorted::testHistogram(GTEST_TEST_DURATION));
}

TEST(TestFrameFilterSorted, SortedElements)
{
	EXPECT_TRUE(TestFrameFilterSorted::testSortedElements(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestFrameFilterSorted::testHistogram(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing histogram:";
	Log::info() << " ";

	typedef HistogramInteger<uint8_t, uint16_t, 256> MedianHistogram;

	const bool allSucceeded = testHistogram<MedianHistogram>(testDuration);

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

bool TestFrameFilterSorted::testSortedElements(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing sorted elements:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testSortedElements<int8_t>(testDuration) && allSucceeded;
	allSucceeded = testSortedElements<uint8_t>(testDuration) && allSucceeded;
	allSucceeded = testSortedElements<int32_t>(testDuration) && allSucceeded;
	allSucceeded = testSortedElements<uint32_t>(testDuration) && allSucceeded;
	allSucceeded = testSortedElements<float>(testDuration) && allSucceeded;
	allSucceeded = testSortedElements<double>(testDuration) && allSucceeded;

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

template <typename THistogram>
bool TestFrameFilterSorted::testHistogram(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int numberIterations = RandomI::random(randomGenerator, 1u, 400u);
		const unsigned int maximalValues = RandomI::random(randomGenerator, 1u, numberIterations);

		THistogram histogram;

		typedef typename THistogram::Type ValueType;

		std::vector<ValueType> values;
		values.reserve(numberIterations);

		for (unsigned int iteration = 0u; iteration < numberIterations; ++iteration)
		{
			ValueType pushValue;

			if (std::is_floating_point<ValueType>::value)
			{
				pushValue = ValueType(RandomF::scalar(randomGenerator, 0.0f, 1.0f));
			}
			else
			{
				pushValue = ValueType(RandomI::random(randomGenerator, 0u, 255u));
			}

			values.emplace_back(pushValue);
			histogram.pushValue(pushValue);

			std::sort(values.begin(), values.end());

			ValueType minValue = values.front();
			ValueType histogramMinValue = histogram.minValue();

			ValueType medianValue = values[(values.size() - 1) / 2];
			ValueType histogramMedianValue = histogram.medianValue();

			ValueType maxValue = values.back();
			ValueType histogramMaxValue = histogram.maxValue();

			if (minValue != histogramMinValue || medianValue != histogramMedianValue || maxValue != histogramMaxValue)
			{
				allSucceeded = false;
			}

			if (histogram.values() > size_t(maximalValues))
			{
				const size_t valueIndex = size_t(RandomI::random(randomGenerator, Index32(values.size() - 1)));

				const ValueType popValue = values[valueIndex];

				values.erase(values.begin() + valueIndex);
				histogram.popValue(popValue);

				std::sort(values.begin(), values.end());

				minValue = values.front();
				histogramMinValue = histogram.minValue();

				medianValue = values[(values.size() - 1) / 2];
				histogramMedianValue = histogram.medianValue();

				maxValue = values.back();
				histogramMaxValue = histogram.maxValue();

				if (minValue != histogramMinValue || medianValue != histogramMedianValue || maxValue != histogramMaxValue)
				{
					allSucceeded = false;
				}
			}
		}

		while (!values.empty())
		{
			const size_t valueIndex = size_t(RandomI::random(randomGenerator, Index32(values.size() - 1)));

			const ValueType value = values[valueIndex];

			values.erase(values.begin() + valueIndex);
			histogram.popValue(value);

			if (!values.empty())
			{
				std::sort(values.begin(), values.end());

				const ValueType minValue = values.front();
				const ValueType histogramMinValue = histogram.minValue();

				const ValueType medianValue = values[(values.size() - 1) / 2];
				const ValueType histogramMedianValue = histogram.medianValue();

				const ValueType maxValue = values.back();
				const ValueType histogramMaxValue = histogram.maxValue();

				if (minValue != histogramMinValue || medianValue != histogramMedianValue || maxValue != histogramMaxValue)
				{
					allSucceeded = false;
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	return allSucceeded;
}

template <typename T>
bool TestFrameFilterSorted::testSortedElements(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int numberIterations = RandomI::random(randomGenerator, 1u, 400u);
		const unsigned int maximalValues = RandomI::random(randomGenerator, 1u, numberIterations);

		SortedElements<T> sortedElements;

		std::vector<T> values;
		values.reserve(numberIterations);

		for (unsigned int iteration = 0u; iteration < numberIterations; ++iteration)
		{
			T pushValue;

			if (std::is_floating_point<T>::value)
			{
				pushValue = T(RandomF::scalar(randomGenerator, 0.0f, 1.0f));
			}
			else
			{
				pushValue = T(RandomI::random(randomGenerator, 0u, 255u));
			}

			values.emplace_back(pushValue);
			sortedElements.pushValue(pushValue);

			std::sort(values.begin(), values.end());

			T minValue = values.front();
			T sortedMinValue = sortedElements.minValue();

			T medianValue = values[(values.size() - 1) / 2];
			T sortedMedianValue = sortedElements.medianValue();

			T maxValue = values.back();
			T sortedMaxValue = sortedElements.maxValue();

			if (minValue != sortedMinValue || medianValue != sortedMedianValue || maxValue != sortedMaxValue)
			{
				allSucceeded = false;
			}

			if (sortedElements.size() > size_t(maximalValues))
			{
				const size_t valueIndex = size_t(RandomI::random(randomGenerator, Index32(values.size() - 1)));

				const T popValue = values[valueIndex];

				values.erase(values.begin() + valueIndex);
				sortedElements.popValue(popValue);

				std::sort(values.begin(), values.end());

				minValue = values.front();
				sortedMinValue = sortedElements.minValue();

				medianValue = values[(values.size() - 1) / 2];
				sortedMedianValue = sortedElements.medianValue();

				maxValue = values.back();
				sortedMaxValue = sortedElements.maxValue();

				if (minValue != sortedMinValue || medianValue != sortedMedianValue || maxValue != sortedMaxValue)
				{
					allSucceeded = false;
				}
			}

			if (RandomI::random(randomGenerator, 1u) == 0u)
			{
				if (std::is_floating_point<T>::value)
				{
					pushValue = T(RandomF::scalar(randomGenerator, 0.0f, 1.0f));
				}
				else
				{
					pushValue = T(RandomI::random(randomGenerator, 0u, 255u));
				}

				const size_t popValueIndex = size_t(RandomI::random(randomGenerator, Index32(values.size() - 1)));

				const T popValue = values[popValueIndex];

				values.erase(values.begin() + popValueIndex);
				values.emplace_back(pushValue);


				sortedElements.exchange(pushValue, popValue);

				std::sort(values.begin(), values.end());

				minValue = values.front();
				sortedMinValue = sortedElements.minValue();

				medianValue = values[(values.size() - 1) / 2];
				sortedMedianValue = sortedElements.medianValue();

				maxValue = values.back();
				sortedMaxValue = sortedElements.maxValue();

				if (minValue != sortedMinValue || medianValue != sortedMedianValue || maxValue != sortedMaxValue)
				{
					allSucceeded = false;
				}
			}
		}

		while (!values.empty())
		{
			const size_t valueIndex = size_t(RandomI::random(randomGenerator, Index32(values.size() - 1)));

			const T value = values[valueIndex];

			values.erase(values.begin() + valueIndex);
			sortedElements.popValue(value);

			if (!values.empty())
			{
				std::sort(values.begin(), values.end());

				const T minValue = values.front();
				const T sortedMinValue = sortedElements.minValue();

				const T medianValue = values[(values.size() - 1) / 2];
				const T sortedMedianValue = sortedElements.medianValue();

				const T maxValue = values.back();
				const T sortedMaxValue = sortedElements.maxValue();

				if (minValue != sortedMinValue || medianValue != sortedMedianValue || maxValue != sortedMaxValue)
				{
					allSucceeded = false;
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	return allSucceeded;
}

}

}

}
