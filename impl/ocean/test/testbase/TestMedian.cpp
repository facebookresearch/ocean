/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestMedian.h"

#include "ocean/base/DataType.h"
#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Median.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Numeric.h"
#include "ocean/math/Random.h"

#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestMedian::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Median test");

	Log::info() << " ";

	if (selector.shouldRun("median2"))
	{
		testResult = testMedian2(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("median3"))
	{
		testResult = testMedian3(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("median4"))
	{
		testResult = testMedian4(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("median5"))
	{
		testResult = testMedian5(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("median_int8"))
	{
		testResult = testMedian<int8_t>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("median_uint16"))
	{
		testResult = testMedian<uint16_t>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("median_int32"))
	{
		testResult = testMedian<int32_t>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("median_uint64"))
	{
		testResult = testMedian<uint64_t>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("median_float"))
	{
		testResult = testMedian<float>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("median_double"))
	{
		testResult = testMedian<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("percentile_int8"))
	{
		testResult = testPercentile<int8_t>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("percentile_uint16"))
	{
		testResult = testPercentile<uint16_t>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("percentile_int32"))
	{
		testResult = testPercentile<int32_t>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("percentile_uint64"))
	{
		testResult = testPercentile<uint64_t>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("percentile_float"))
	{
		testResult = testPercentile<float>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("percentile_double"))
	{
		testResult = testPercentile<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
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

TEST(TestMedian, Median_int8)
{
	EXPECT_TRUE(TestMedian::testMedian<int8_t>(GTEST_TEST_DURATION));
}

TEST(TestMedian, Median_uint16)
{
	EXPECT_TRUE(TestMedian::testMedian<uint16_t>(GTEST_TEST_DURATION));
}

TEST(TestMedian, Median_int32)
{
	EXPECT_TRUE(TestMedian::testMedian<int32_t>(GTEST_TEST_DURATION));
}

TEST(TestMedian, Median_uint64)
{
	EXPECT_TRUE(TestMedian::testMedian<uint64_t>(GTEST_TEST_DURATION));
}

TEST(TestMedian, Median_float)
{
	EXPECT_TRUE(TestMedian::testMedian<float>(GTEST_TEST_DURATION));
}

TEST(TestMedian, Median_double)
{
	EXPECT_TRUE(TestMedian::testMedian<double>(GTEST_TEST_DURATION));
}

TEST(TestMedian, Percentile_int8)
{
	EXPECT_TRUE(TestMedian::testPercentile<int8_t>(GTEST_TEST_DURATION));
}

TEST(TestMedian, Percentile_uint16)
{
	EXPECT_TRUE(TestMedian::testPercentile<uint16_t>(GTEST_TEST_DURATION));
}

TEST(TestMedian, Percentile_int32)
{
	EXPECT_TRUE(TestMedian::testPercentile<int32_t>(GTEST_TEST_DURATION));
}

TEST(TestMedian, Percentile_uint64)
{
	EXPECT_TRUE(TestMedian::testPercentile<uint64_t>(GTEST_TEST_DURATION));
}

TEST(TestMedian, Percentile_float)
{
	EXPECT_TRUE(TestMedian::testPercentile<float>(GTEST_TEST_DURATION));
}

TEST(TestMedian, Percentile_double)
{
	EXPECT_TRUE(TestMedian::testPercentile<double>(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestMedian::testMedian2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Median test with two parameters:";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int i[2] =
		{
			RandomI::random(randomGenerator, 100u),
			RandomI::random(randomGenerator, 100u)
		};

		const unsigned int median = Median::median2(i[0], i[1]);

		std::vector<unsigned int> values(2);
		memcpy(values.data(), i, sizeof(unsigned int) * 2);

		std::sort(values.begin(), values.end());

		OCEAN_EXPECT_EQUAL(validation, values[0], median);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestMedian::testMedian3(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Median test with three parameters:";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int i[3] =
		{
			RandomI::random(randomGenerator, 100u),
			RandomI::random(randomGenerator, 100u),
			RandomI::random(randomGenerator, 100u)
		};

		const unsigned int median = Median::median3(i[0], i[1], i[2]);

		std::vector<unsigned int> values(3);
		memcpy(values.data(), i, sizeof(unsigned int) * 3);

		std::sort(values.begin(), values.end());

		OCEAN_EXPECT_EQUAL(validation, values[1], median);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestMedian::testMedian4(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Median test with four parameters:";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int i[4] =
		{
			RandomI::random(randomGenerator, 100u),
			RandomI::random(randomGenerator, 100u),
			RandomI::random(randomGenerator, 100u),
			RandomI::random(randomGenerator, 100u)
		};

		const unsigned int median = Median::median4(i[0], i[1], i[2], i[3]);

		std::vector<unsigned int> values(4);
		memcpy(values.data(), i, sizeof(unsigned int) * 4);

		std::sort(values.begin(), values.end());

		OCEAN_EXPECT_EQUAL(validation, values[1], median);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestMedian::testMedian5(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Median test with five parameters:";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int i[5] =
		{
			RandomI::random(randomGenerator, 100u),
			RandomI::random(randomGenerator, 100u),
			RandomI::random(randomGenerator, 100u),
			RandomI::random(randomGenerator, 100u),
			RandomI::random(randomGenerator, 100u)
		};

		const unsigned int median = Median::median5(i[0], i[1], i[2], i[3], i[4]);

		std::vector<unsigned int> values(5);
		memcpy(values.data(), i, sizeof(unsigned int) * 5);

		std::sort(values.begin(), values.end());

		OCEAN_EXPECT_EQUAL(validation, values[2], median);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
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

	using Elements = std::vector<T>;
	Elements elements(number);

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	HighPerformanceStatistic performance;
	HighPerformanceStatistic stdPerformance;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0; n < number; ++n)
		{
			if constexpr (std::is_same<T, int8_t>::value)
			{
				elements[n] = int8_t(RandomI::random(randomGenerator, 255u));
			}
			else if constexpr (std::is_same<T, uint16_t>::value)
			{
				elements[n] = uint16_t(RandomI::random32(randomGenerator));
			}
			else if constexpr (std::is_same<T, int32_t>::value)
			{
				elements[n] = int32_t(RandomI::random32(randomGenerator));
			}
			else if constexpr (std::is_same<T, uint64_t>::value)
			{
				elements[n] = uint64_t(RandomI::random64(randomGenerator));
			}
			else if constexpr (std::is_same<T, float>::value)
			{
				float floatValue = NumericF::nan();

				while (NumericF::isNan(floatValue))
				{
					uint32_t value = RandomI::random32(randomGenerator);
					memcpy(&floatValue, &value, sizeof(float));
				}

				elements[n] = floatValue;
			}
			else if constexpr (std::is_same<T, double>::value)
			{
				double doubleValue = NumericD::nan();

				while (NumericD::isNan(doubleValue))
				{
					uint64_t value = RandomI::random64(randomGenerator);
					memcpy(&doubleValue, &value, sizeof(double));
				}

				elements[n] = doubleValue;
			}
			else
			{
				ocean_assert(false && "This should never happen!");
				OCEAN_SET_FAILED(validation);
			}
		}

		Elements copyOcean(elements);
		Elements copyStd(elements);

		const T value0 = Median::constMedian(elements.data(), elements.size());

		performance.start();
			const T value1 = Median::median(copyOcean.data(), copyOcean.size());
		performance.stop();

		const size_t index = (copyStd.size() - 1) / 2;
		OCEAN_EXPECT_LESS_EQUAL(validation, size_t(0), index);
		OCEAN_EXPECT_LESS_EQUAL(validation, index, copyStd.size() - 1);

		stdPerformance.start();
			std::nth_element(copyStd.begin(), copyStd.begin() + index, copyStd.end());
			const T testValue = copyStd[index];
		stdPerformance.stop();

		OCEAN_EXPECT_EQUAL(validation, value0, testValue);
		OCEAN_EXPECT_EQUAL(validation, value1, testValue);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance: Best: " << performance;
	Log::info() << "Standard performance: Best: " << stdPerformance;

	if (performance.bestMseconds() > 0.0)
	{
		Log::info() << "Median boost factor: " << String::toAString(stdPerformance.medianMseconds() / performance.medianMseconds(), 1u) << "x";
	}
	else
	{
		Log::info() << "Median boost factor: ~1x";
	}

	return validation.succeeded();
}

template <typename T>
bool TestMedian::testPercentile(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Percentile test with \"" << TypeNamer::name<T>() << "\":";

	using Elements = std::vector<T>;

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const size_t number = RandomI::random(randomGenerator, 1u, 100000u);

		Elements elements(number);

		for (unsigned int n = 0; n < number; ++n)
		{
			if constexpr (std::is_same<T, int8_t>::value)
			{
				elements[n] = int8_t(RandomI::random(randomGenerator, 255u));
			}
			else if constexpr (std::is_same<T, uint16_t>::value)
			{
				elements[n] = uint16_t(RandomI::random32(randomGenerator));
			}
			else if constexpr (std::is_same<T, int32_t>::value)
			{
				elements[n] = int32_t(RandomI::random32(randomGenerator));
			}
			else if constexpr (std::is_same<T, uint64_t>::value)
			{
				elements[n] = uint64_t(RandomI::random64(randomGenerator));
			}
			else if constexpr (std::is_same<T, float>::value)
			{
				float floatValue = NumericF::nan();

				while (NumericF::isNan(floatValue))
				{
					uint32_t value = RandomI::random32(randomGenerator);
					memcpy(&floatValue, &value, sizeof(float));
				}

				elements[n] = floatValue;
			}
			else if constexpr (std::is_same<T, double>::value)
			{
				double doubleValue = NumericD::nan();

				while (NumericD::isNan(doubleValue))
				{
					uint64_t value = RandomI::random64(randomGenerator);
					memcpy(&doubleValue, &value, sizeof(double));
				}

				elements[n] = doubleValue;
			}
			else
			{
				ocean_assert(false && "This should never happen!");
				OCEAN_SET_FAILED(validation);
			}
		}

		const double percentile = RandomD::scalar(randomGenerator, 0.0, 1.0);

		Elements copyOcean(elements);
		Elements copyConst(elements);
		Elements copyStd(elements);

		const T value0 = Median::constPercentile(copyConst.data(), copyConst.size(), percentile);
		const T value1 = Median::percentile(copyOcean.data(), copyOcean.size(), percentile);

		const size_t index = minmax<size_t>(0, size_t(double(number) * percentile), number - 1);
		OCEAN_EXPECT_LESS_EQUAL(validation, size_t(0), index);
		OCEAN_EXPECT_LESS_EQUAL(validation, index, copyStd.size() - 1);

		std::nth_element(copyStd.begin(), copyStd.begin() + index, copyStd.end());
		const T testValue = copyStd[index];

		OCEAN_EXPECT_EQUAL(validation, value0, testValue);
		OCEAN_EXPECT_EQUAL(validation, value1, testValue);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
