/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestVariance.h"

#include "ocean/base/Timestamp.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

#include "ocean/math/Random.h"
#include "ocean/math/Variance.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestVariance::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Variance test");

	Log::info() << " ";

	if (selector.shouldRun("average"))
	{
		testResult = testAverage<float>(testDuration);
		Log::info() << " ";
		testResult = testAverage<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("deviation"))
	{
		testResult = testDeviation<float>(testDuration);
		Log::info() << " ";
		testResult = testDeviation<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("remove"))
	{
		testResult = testRemove<float>(testDuration);
		Log::info() << " ";
		testResult = testRemove<double>(testDuration);

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestVariance, AverageFloat)
{
	EXPECT_TRUE((TestVariance::testAverage<float>(GTEST_TEST_DURATION)));
}

TEST(TestVariance, AverageDouble)
{
	EXPECT_TRUE((TestVariance::testAverage<double>(GTEST_TEST_DURATION)));
}


TEST(TestVariance, DeviationFloat)
{
	EXPECT_TRUE((TestVariance::testDeviation<float>(GTEST_TEST_DURATION)));
}

TEST(TestVariance, DeviationDouble)
{
	EXPECT_TRUE((TestVariance::testDeviation<double>(GTEST_TEST_DURATION)));
}


TEST(TestVariance, RemoveFloat)
{
	EXPECT_TRUE((TestVariance::testRemove<float>(GTEST_TEST_DURATION)));
}

TEST(TestVariance, RemoveDouble)
{
	EXPECT_TRUE((TestVariance::testRemove<double>(GTEST_TEST_DURATION)));
}

#endif // OCEAN_USE_GTEST

template <typename T>
bool TestVariance::testAverage(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Average test, with '" << TypeNamer::name<T>() << "':";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int size = RandomI::random(randomGenerator, 1000u);

		std::vector<T> elements;
		elements.reserve(size);

		T sum = T(0);

		for (unsigned int n = 0u; n < size; ++n)
		{
			const T element = RandomT<T>::scalar(randomGenerator, -10, 10);

			elements.emplace_back(element);

			sum += element;
		}

		const T average = NumericT<T>::ratio(sum, T(size), T(0));

		{
			VarianceT<T> object;

			for (unsigned int n = 0u; n < size; ++n)
			{
				object.add(elements[n]);
			}

			if (size != 0)
			{
				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isWeakEqual(object.average(), average));
			}
			else
			{
				OCEAN_EXPECT_TRUE(validation, !object);
			}
		}

		{
			const VarianceT<T> object(elements.data(), elements.size());

			if (size != 0)
			{
				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isWeakEqual(object.average(), average));
			}
			else
			{
				OCEAN_EXPECT_TRUE(validation, !object);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestVariance::testDeviation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Deviation test, with '" << TypeNamer::name<T>() << "':";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int size = RandomI::random(randomGenerator, 1000u);

		std::vector<T> elements;
		elements.reserve(size);

		for (unsigned int n = 0u; n < size; ++n)
		{
			const T element = RandomT<T>::scalar(randomGenerator, -10, 10);

			elements.emplace_back(element);
		}

		{
			VarianceT<T> object;

			for (unsigned int n = 0u; n < size; ++n)
			{
				object.add(elements[n]);
			}

			if (size != 0)
			{
				const T average = object.average();

				T sumSqrDifferences = T(0);

				for (unsigned int n = 0u; n < size; ++n)
				{
					sumSqrDifferences += NumericT<T>::sqr(elements[n] - average);
				}

				const T variance = sumSqrDifferences / T(size);

				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isWeakEqual(object.variance(), variance));

				const T deviation = NumericT<T>::sqrt(variance);

				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isWeakEqual(object.deviation(), deviation));
			}
			else
			{
				OCEAN_EXPECT_TRUE(validation, !object);
			}
		}

		{
			const VarianceT<T> object(elements.data(), elements.size());

			if (size != 0)
			{
				const T average = object.average();

				T sumSqrDifferences = T(0);

				for (unsigned int n = 0u; n < size; ++n)
				{
					sumSqrDifferences += NumericT<T>::sqr(elements[n] - average);
				}

				const T variance = sumSqrDifferences / T(size);

				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isWeakEqual(object.variance(), variance));

				const T deviation = NumericT<T>::sqrt(variance);

				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isWeakEqual(object.deviation(), deviation));
			}
			else
			{
				OCEAN_EXPECT_TRUE(validation, !object);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestVariance::testRemove(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Remove test, with '" << TypeNamer::name<T>() << "':";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int initialSize = RandomI::random(randomGenerator, 1u, 1000u);

		std::vector<T> elements;
		elements.reserve(initialSize);

		for (unsigned int n = 0u; n < initialSize; ++n)
		{
			const T element = RandomT<T>::scalar(randomGenerator, -10, 10);

			elements.emplace_back(element);
		}

		VarianceT<T> object;

		for (unsigned int n = 0u; n < initialSize; ++n)
		{
			object.add(elements[n]);
		}

		const unsigned int numberRemove = RandomI::random(randomGenerator, 1u, initialSize);

		for (unsigned int n = 0u; n < numberRemove; ++n)
		{
			ocean_assert(!elements.empty());

			const unsigned int index = RandomI::random(randomGenerator, (unsigned int)(elements.size()) - 1u);

			object.remove(elements[index]);

			elements[index] = elements.back();
			elements.pop_back();
		}

		OCEAN_EXPECT_EQUAL(validation, object.size(), elements.size());

		if (elements.empty())
		{
			ocean_assert(initialSize == numberRemove);

			OCEAN_EXPECT_TRUE(validation, !object);
		}
		else
		{
			T sumElements = 0;

			for (const T& element : elements)
			{
				sumElements += element;
			}

			ocean_assert(!elements.empty());
			const T average = sumElements / T(elements.size());

			T sumSqrDifferences = T(0);

			for (unsigned int n = 0u; n < elements.size(); ++n)
			{
				sumSqrDifferences += NumericT<T>::sqr(elements[n] - average);
			}

			const T variance = sumSqrDifferences / T(elements.size());

			constexpr T eps = std::is_same<T, float>::value ? T(0.1) : T(0.001);

			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(object.variance(), variance, eps));

			const T deviation = NumericT<T>::sqrt(variance);

			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(object.deviation(), deviation, NumericT<T>::sqrt(eps)));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
