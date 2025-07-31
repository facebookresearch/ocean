/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestMedian.h"

#include "ocean/base/DataType.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Median.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include <algorithm>

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestMedian::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Median test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testMedian2(testDuration);

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMedian3(testDuration);

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMedian4(testDuration);

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMedian5(testDuration);

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMedian<char>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMedian<unsigned short>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMedian<int>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMedian<unsigned long long>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMedian<float>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMedian<double>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Median test succeeded.";
	}
	else
	{
		Log::info() << "Median test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestMedian, Median2)
{
	EXPECT_TRUE(TestMedian::testMedian2(GTEST_TEST_DURATION));
}

TEST(TestMedian, Median3)
{
	EXPECT_TRUE(TestMedian::testMedian3(GTEST_TEST_DURATION));
}

TEST(TestMedian, Median4)
{
	EXPECT_TRUE(TestMedian::testMedian4(GTEST_TEST_DURATION));
}

TEST(TestMedian, Median5)
{
	EXPECT_TRUE(TestMedian::testMedian5(GTEST_TEST_DURATION));
}

TEST(TestMedian, MedianChar)
{
	EXPECT_TRUE(TestMedian::testMedian<char>(GTEST_TEST_DURATION));
}

TEST(TestMedian, MedianUnsignedShort)
{
	EXPECT_TRUE(TestMedian::testMedian<unsigned short>(GTEST_TEST_DURATION));
}

TEST(TestMedian, MedianInt)
{
	EXPECT_TRUE(TestMedian::testMedian<int>(GTEST_TEST_DURATION));
}

TEST(TestMedian, MedianUnsignedLongLong)
{
	EXPECT_TRUE(TestMedian::testMedian<unsigned long long>(GTEST_TEST_DURATION));
}

TEST(TestMedian, MedianFloat)
{
	EXPECT_TRUE(TestMedian::testMedian<float>(GTEST_TEST_DURATION));
}

TEST(TestMedian, MedianDouble)
{
	EXPECT_TRUE(TestMedian::testMedian<double>(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestMedian::testMedian2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Median test with two parameters:";

	bool result = true;

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int i[2] =
		{
			RandomI::random(100),
			RandomI::random(100)
		};

		const unsigned int median = Median::median2(i[0], i[1]);

		std::vector<unsigned int> values(2);
		memcpy(values.data(), i, sizeof(unsigned int) * 2);

		std::sort(values.begin(), values.end());

		if (values[0] != median)
			result = false;
	}
	while (Timestamp(true) < startTimestamp + testDuration);

	if (result)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return result;
}

bool TestMedian::testMedian3(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Median test with three parameters:";

	bool result = true;

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int i[3] =
		{
			RandomI::random(100),
			RandomI::random(100),
			RandomI::random(100)
		};

		const unsigned int median = Median::median3(i[0], i[1], i[2]);

		std::vector<unsigned int> values(3);
		memcpy(values.data(), i, sizeof(unsigned int) * 3);

		std::sort(values.begin(), values.end());

		if (values[1] != median)
			result = false;
	}
	while (Timestamp(true) < startTimestamp + testDuration);

	if (result)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return result;
}

bool TestMedian::testMedian4(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Median test with four parameters:";

	bool result = true;

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int i[4] =
		{
			RandomI::random(100),
			RandomI::random(100),
			RandomI::random(100),
			RandomI::random(100)
		};

		const unsigned int median = Median::median4(i[0], i[1], i[2], i[3]);

		std::vector<unsigned int> values(4);
		memcpy(values.data(), i, sizeof(unsigned int) * 4);

		std::sort(values.begin(), values.end());

		if (values[1] != median)
			result = false;
	}
	while (Timestamp(true) < startTimestamp + testDuration);

	if (result)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return result;
}

bool TestMedian::testMedian5(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Median test with five parameters:";

	bool result = true;

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int i[5] =
		{
			RandomI::random(100),
			RandomI::random(100),
			RandomI::random(100),
			RandomI::random(100),
			RandomI::random(100)
		};

		const unsigned int median = Median::median5(i[0], i[1], i[2], i[3], i[4]);

		std::vector<unsigned int> values(5);
		memcpy(values.data(), i, sizeof(unsigned int) * 5);

		std::sort(values.begin(), values.end());

		if (values[2] != median)
			result = false;
	}
	while (Timestamp(true) < startTimestamp + testDuration);

	if (result)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return result;
}

template <typename T>
bool TestMedian::testMedian(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Median test with \"" << TypeNamer::name<T>() << "\":";
	Log::info() << " ";

	bool allSucceeded = true;

	for (const unsigned number : {1u, 10u, 101u, 1000u, 10001u, 100000u})
	{
		allSucceeded = testMedian<T>(number, testDuration) && allSucceeded;
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
bool TestMedian::testMedian(const unsigned int number, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with " << String::insertCharacter(String::toAString(number), ',', 3, false) << " elements:";

	typedef std::vector<T> Elements;
	Elements elements(number);

	bool allSucceeded = true;

	HighPerformanceStatistic performance;
	HighPerformanceStatistic stdPerformance;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0; n < number; ++n)
		{
			if constexpr (sizeof(T) == 1)
			{
				static_assert(sizeof(unsigned char) == 1, "Invalid data type!");

				const unsigned char randomValue = (unsigned char)(RandomI::random(255u));
				elements[n] = *((T*)&randomValue);
			}

			if constexpr (sizeof(T) == 2)
			{
				static_assert(sizeof(unsigned short) == 2, "Invalid data type!");

				const unsigned short randomValue = (unsigned short)(RandomI::random32());
				elements[n] = *((T*)&randomValue);
			}

			if constexpr (sizeof(T) == 4)
			{
				static_assert(sizeof(unsigned int) == 4, "Invalid data type!");

				// we explicitly create a floating point value based on random bits ensuring that the algorithm works with any value (e.g., nan, inv et.c)
				do
				{
					const unsigned int randomValue = RandomI::random32();
					elements[n] = *((T*)&randomValue);
				}
				while (elements[n] != elements[n]); // Check for NaN
			}

			if constexpr (sizeof(T) == 8)
			{
				static_assert(sizeof(unsigned long long) == 8, "Invalid data type!");

				// we explicitly create a floating point value based on random bits ensuring that the algorithm works with any value (e.g., nan, inv et.c)
				do
				{
					const unsigned long long randomValue = RandomI::random64();
					elements[n] = *((T*)&randomValue);
				}
				while (elements[n] != elements[n]); // Check for NaN
			}
		}


		Elements copy(elements);

		const T value0 = Median::constMedian(elements.data(), elements.size());

		performance.start();
		const T value1 = Median::median(copy.data(), copy.size());
		performance.stop();

		copy = elements;
		stdPerformance.start();
		std::nth_element(copy.begin(), copy.begin() + (copy.size() - 1) / 2, copy.end());
		const T testValue = copy[(copy.size() - 1) / 2];
		stdPerformance.stop();

		if (value0 != testValue || value1 != testValue)
		{
			// we may receive a different result in the case at least one value is not a number (nan)

			bool hasNan = false;
			for (size_t n = 0; !hasNan && n < elements.size(); ++n)
			{
				hasNan = !isNumber(elements[n]);
			}

			if (!hasNan)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: Best: " << performance.bestMseconds() << "ms, worst: " << performance.worstMseconds() << "ms, average: " << performance.averageMseconds() << "ms";
	Log::info() << "Standard performance: Best: " << stdPerformance.bestMseconds() << "ms, worst: " << stdPerformance.worstMseconds() << "ms, average: " << stdPerformance.averageMseconds() << "ms";

	if (performance.bestMseconds() > 0)
	{
		Log::info() << "Best boost factor: " << String::toAString(stdPerformance.bestMseconds() / performance.bestMseconds(), 1u) << "x";
	}
	else
	{
		Log::info() << "Best boost factor: ~1x";
	}

	return allSucceeded;
}

}

}

}
