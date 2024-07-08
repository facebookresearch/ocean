/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestVariance.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/Random.h"
#include "ocean/math/Variance.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestVariance::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Variance test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testAverage<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testAverage<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDeviation<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testDeviation<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRemove<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testRemove<double>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Variance test succeeded.";
	}
	else
	{
		Log::info() << "Variance test FAILED!";
	}

	return allSucceeded;
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

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int size = RandomI::random(1000u);

		std::vector<T> elements;
		elements.reserve(size);

		T sum = T(0);

		for (unsigned int n = 0u; n < size; ++n)
		{
			const T element = RandomT<T>::scalar(-10, 10);

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
				if (NumericT<T>::isNotWeakEqual(object.average(), average))
				{
					allSucceeded = false;
				}
			}
			else
			{
				if (object)
				{
					allSucceeded = false;
				}
			}
		}

		{
			const VarianceT<T> object(elements.data(), elements.size());

			if (size != 0)
			{
				if (NumericT<T>::isNotWeakEqual(object.average(), average))
				{
					allSucceeded = false;
				}
			}
			else
			{
				if (object)
				{
					allSucceeded = false;
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestVariance::testDeviation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Deviation test, with '" << TypeNamer::name<T>() << "':";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int size = RandomI::random(1000u);

		std::vector<T> elements;
		elements.reserve(size);

		for (unsigned int n = 0u; n < size; ++n)
		{
			const T element = RandomT<T>::scalar(-10, 10);

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

				if (NumericT<T>::isNotWeakEqual(object.variance(), variance))
				{
					allSucceeded = false;
				}

				const T deviation = NumericT<T>::sqrt(variance);

				if (NumericT<T>::isNotWeakEqual(object.deviation(), deviation))
				{
					allSucceeded = false;
				}
			}
			else
			{
				if (object)
				{
					allSucceeded = false;
				}
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

				if (NumericT<T>::isNotWeakEqual(object.variance(), variance))
				{
					allSucceeded = false;
				}

				const T deviation = NumericT<T>::sqrt(variance);

				if (NumericT<T>::isNotWeakEqual(object.deviation(), deviation))
				{
					allSucceeded = false;
				}
			}
			else
			{
				if (object)
				{
					allSucceeded = false;
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestVariance::testRemove(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Remove test, with '" << TypeNamer::name<T>() << "':";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int initialSize = RandomI::random(1u, 1000u);

		std::vector<T> elements;
		elements.reserve(initialSize);

		for (unsigned int n = 0u; n < initialSize; ++n)
		{
			const T element = RandomT<T>::scalar(-10, 10);

			elements.emplace_back(element);
		}

		VarianceT<T> object;

		for (unsigned int n = 0u; n < initialSize; ++n)
		{
			object.add(elements[n]);
		}

		const unsigned int numberRemove = RandomI::random(1u, initialSize);

		for (unsigned int n = 0u; n < numberRemove; ++n)
		{
			ocean_assert(!elements.empty());

			const unsigned int index = RandomI::random((unsigned int)(elements.size()) - 1u);

			object.remove(elements[index]);

			elements[index] = elements.back();
			elements.pop_back();
		}

		if (object.size() != elements.size())
		{
			allSucceeded = false;
		}

		if (elements.empty())
		{
			ocean_assert(initialSize == numberRemove);

			if (object)
			{
				allSucceeded = false;
			}
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

			if (NumericT<T>::isNotEqual(object.variance(), variance, eps))
			{
				allSucceeded = false;
			}

			const T deviation = NumericT<T>::sqrt(variance);

			if (NumericT<T>::isNotEqual(object.deviation(), deviation, NumericT<T>::sqrt(eps)))
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

}

}

}
