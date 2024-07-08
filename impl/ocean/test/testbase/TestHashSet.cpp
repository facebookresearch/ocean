/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestHashSet.h"

#include "ocean/base/HashSet.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include <set>
#include <unordered_set>
#include <vector>

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestHashSet::test(const double testDuration)
{
	Log::info() << "---   Hash set test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testSingleIntegers(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMultipleIntegers(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
		Log::info() << "Hash set test succeeded.";
	else
		Log::info() << "Hash set test FAILED!";

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestHashSet, SingleIntegers)
{
	EXPECT_TRUE(TestHashSet::testSingleIntegers(GTEST_TEST_DURATION));
}

TEST(TestHashSet, MultipleIntegers)
{
	EXPECT_TRUE(TestHashSet::testMultipleIntegers(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestHashSet::testSingleIntegers(const double testDuration)
{
	bool allSucceeded = true;

	allSucceeded = testPerformanceSingleIntegers(100u, 10u, testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testPerformanceSingleIntegers(100u, 30u, testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testPerformanceSingleIntegers(100u, 50u, testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testPerformanceSingleIntegers(100u, 80u, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPerformanceSingleIntegers(1000u, 10u, testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testPerformanceSingleIntegers(1000u, 30u, testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testPerformanceSingleIntegers(1000u, 50u, testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testPerformanceSingleIntegers(1000u, 80u, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testPerformanceSingleIntegers(100000u, 10u, testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testPerformanceSingleIntegers(100000u, 30u, testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testPerformanceSingleIntegers(100000u, 50u, testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testPerformanceSingleIntegers(100000u, 80u, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = validationStaticCapacitySingleIntegers(100u, 10u, testDuration) && allSucceeded;
	allSucceeded = validationStaticCapacitySingleIntegers(100u, 30u, testDuration) && allSucceeded;
	allSucceeded = validationStaticCapacitySingleIntegers(100u, 50u, testDuration) && allSucceeded;
	allSucceeded = validationStaticCapacitySingleIntegers(100u, 70u, testDuration) && allSucceeded;
	allSucceeded = validationStaticCapacitySingleIntegers(100u, 90u, testDuration) && allSucceeded;
	allSucceeded = validationStaticCapacitySingleIntegers(100u, 100u, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = validationDynamicCapacitySingleIntegers(100u, 200u, testDuration) && allSucceeded;
	allSucceeded = validationDynamicCapacitySingleIntegers(100u, 100u, testDuration) && allSucceeded;
	allSucceeded = validationDynamicCapacitySingleIntegers(100u, 50u, testDuration) && allSucceeded;
	allSucceeded = validationDynamicCapacitySingleIntegers(100u, 10u, testDuration) && allSucceeded;
	allSucceeded = validationDynamicCapacitySingleIntegers(100u, 0u, testDuration) && allSucceeded;

	return allSucceeded;
}

bool TestHashSet::testMultipleIntegers(const double testDuration)
{
	bool allSucceeded = true;

	allSucceeded = validationMultipleIntegers(100u, 10u, testDuration) && allSucceeded;
	allSucceeded = validationMultipleIntegers(100u, 30u, testDuration) && allSucceeded;
	allSucceeded = validationMultipleIntegers(100u, 50u, testDuration) && allSucceeded;
	allSucceeded = validationMultipleIntegers(100u, 70u, testDuration) && allSucceeded;
	allSucceeded = validationMultipleIntegers(100u, 90u, testDuration) && allSucceeded;
	allSucceeded = validationMultipleIntegers(100u, 100u, testDuration) && allSucceeded;

	return allSucceeded;
}

bool TestHashSet::testPerformanceSingleIntegers(const unsigned int number, const unsigned int occupancy, const double testDuration)
{
	ocean_assert(occupancy > 0u && occupancy <= 100u);
	ocean_assert(testDuration > 0);

	Log::info() << "Test performance " << String::insertCharacter(String::toAString(number), ',', 3, false) << " (single) elements with " << occupancy << "% occupancy:";

	bool allSucceeded = true;

	typedef HashSet<unsigned int> Table;
	typedef std::set<unsigned int> ElementSet;
	typedef std::vector<unsigned int> Elements;

	typedef std::unordered_set<unsigned int> ElementUnorderedSet;

	ElementSet tmpElementSet;
	Elements addElements;
	Elements removeElements;

	const unsigned int numberElements = number * occupancy / 100;
	while (addElements.size() < numberElements)
	{
		const unsigned int randomNumber = RandomI::random(10u * number);

		if (tmpElementSet.find(randomNumber) == tmpElementSet.end())
		{
			tmpElementSet.insert(randomNumber);
			addElements.push_back(randomNumber);
		}

	}

	tmpElementSet.clear();

	while (removeElements.size() < addElements.size() / 10)
	{
		const unsigned int randomIndex = RandomI::random((unsigned int)(addElements.size()) - 1u);
		const unsigned int randomNumber = addElements[randomIndex];

		if (tmpElementSet.find(randomNumber) == tmpElementSet.end())
		{
			tmpElementSet.insert(randomNumber);
			removeElements.push_back(randomNumber);
		}

	}

	const Timestamp startTimestamp(true);

	HighPerformanceStatistic stdPerformance, stdUnorderedPerformance, hashPerformance;

	do
	{
		{
			const HighPerformanceStatistic::ScopedStatistic scopedStatistic(stdPerformance);

			ElementSet elementSet;

			for (Elements::const_iterator i = addElements.begin(); i != addElements.end(); ++i)
				elementSet.insert(*i);

			for (Elements::const_iterator i = addElements.begin(); i != addElements.end(); ++i)
				if (elementSet.find(*i) == elementSet.end())
					allSucceeded = false;

			for (Elements::const_iterator i = removeElements.begin(); i != removeElements.end(); ++i)
				elementSet.erase(*i);
		}

		{
			const HighPerformanceStatistic::ScopedStatistic scopedStatistic(hashPerformance);

			Table table(number);

			for (Elements::const_iterator i = addElements.begin(); i != addElements.end(); ++i)
				table.insert(*i);

			for (Elements::const_iterator i = addElements.begin(); i != addElements.end(); ++i)
				if (!table.find(*i))
					allSucceeded = false;

			for (Elements::const_iterator i = removeElements.begin(); i != removeElements.end(); ++i)
				table.remove(*i);
		}

		{
			const HighPerformanceStatistic::ScopedStatistic scopedStatistic(stdUnorderedPerformance);

			ElementUnorderedSet elementSet;
			elementSet.reserve(number);

			for (Elements::const_iterator i = addElements.begin(); i != addElements.end(); ++i)
				elementSet.insert(*i);

			for (Elements::const_iterator i = addElements.begin(); i != addElements.end(); ++i)
				if (elementSet.find(*i) == elementSet.end())
					allSucceeded = false;

			for (Elements::const_iterator i = removeElements.begin(); i != removeElements.end(); ++i)
				elementSet.erase(*i);
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	const double boostBest = hashPerformance.best() == 0 ? -1 : stdPerformance.best() / hashPerformance.best();
	const double boostWorst = hashPerformance.worst() == 0 ? -1 : stdPerformance.worst() / hashPerformance.worst();
	const double boostAverage = hashPerformance.average() == 0 ? -1 : stdPerformance.average() / hashPerformance.average();

	Log::info() << "Std performance: Best: " << stdPerformance.bestMseconds() << "ms, worst: " << stdPerformance.worstMseconds() << "ms, average: " << stdPerformance.averageMseconds() << "ms";
	Log::info() << "Hash performance: Best: " << hashPerformance.bestMseconds() << "ms, worst: " << hashPerformance.worstMseconds() << "ms, average: " << hashPerformance.averageMseconds() << "ms";
	Log::info() << "Boost factor: Best: " << String::toAString(boostBest, 1u) << "x, worst: " << String::toAString(boostWorst, 1u) << "x, average: " << String::toAString(boostAverage, 1u) << "x";

	{
		const double unorderedBoostBest = hashPerformance.best() == 0 ? -1 : stdUnorderedPerformance.best() / hashPerformance.best();
		const double unorderedBoostWorst = hashPerformance.worst() == 0 ? -1 : stdUnorderedPerformance.worst() / hashPerformance.worst();
		const double unorderedBoostAverage = hashPerformance.average() == 0 ? -1 : stdUnorderedPerformance.average() / hashPerformance.average();

		Log::info() << "Std unordered performance: Best: " << stdUnorderedPerformance.bestMseconds() << "ms, worst: " << stdUnorderedPerformance.worstMseconds() << "ms, average: " << stdUnorderedPerformance.averageMseconds() << "ms";
		Log::info() << "Boost factor: Best: " << String::toAString(unorderedBoostBest, 1u) << "x, worst: " << String::toAString(unorderedBoostWorst, 1u) << "x, average: " << String::toAString(unorderedBoostAverage, 1u) << "x";
	}

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

bool TestHashSet::validationStaticCapacitySingleIntegers(const unsigned int number, const unsigned int occupancy, const double testDuration)
{
	ocean_assert(occupancy > 0u && occupancy <= 100u);
	ocean_assert(testDuration > 0);

	Log::info() << "Test validation " << number << " (single) elements with " << occupancy << "% occupancy:";

	RandomGenerator randomGenerator;

	typedef HashSet<unsigned int> Table;
	typedef std::vector<unsigned int> Elements;
	typedef std::set<unsigned int> ElementSet;

	ElementSet elementSet;
	Elements elements;

	Table table(number);

	unsigned int tableSize = 0u;
	unsigned int tableCapacity = number;

	bool succeeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		if (!table.isEmpty())
		{
			succeeded = false;
			break;
		}

		while (tableSize * 100 / tableCapacity < occupancy)
		{
			if (tableSize != table.size())
			{
				succeeded = false;
				break;
			}

			if (tableCapacity != table.capacity())
			{
				succeeded = false;
				break;
			}

			const unsigned int randomValue = RandomI::random(randomGenerator, 10000u);

			if (elementSet.find(randomValue) != elementSet.end())
				continue;

			const bool insertResult = table.insert(randomValue, true, false);
			if (!insertResult)
			{
				succeeded = false;
				break;
			}

			elementSet.insert(randomValue);
			elements.push_back(randomValue);

			for (Elements::const_iterator i = elements.begin(); i != elements.end(); ++i)
				if (!table.find(*i))
				{
					succeeded = false;
					break;
				}

			++tableSize;
		}

		while (tableSize != 0u)
		{
			if (tableSize != table.size())
			{
				succeeded = false;
				break;
			}

			if (tableCapacity != table.capacity())
			{
				succeeded = false;
				break;
			}

			const unsigned int randomIndex = RandomI::random(randomGenerator, (unsigned int)(elements.size()) - 1u);
			const unsigned int randomValue = elements[randomIndex];

			ocean_assert(elementSet.find(randomValue) != elementSet.end());

			const bool findResultBefore = table.find(randomValue);
			if (!findResultBefore)
			{
				succeeded = false;
				break;
			}

			const bool removeResult = table.remove(randomValue);
			if (!removeResult)
			{
				succeeded = false;
				break;
			}

			const bool findResultAfter = table.find(randomValue);
			if (findResultAfter)
			{
				succeeded = false;
				break;
			}

			elementSet.erase(randomValue);
			elements.erase(elements.begin() + randomIndex);

			for (Elements::const_iterator i = elements.begin(); i != elements.end(); ++i)
				if (!table.find(*i))
				{
					succeeded = false;
					break;
				}

			ocean_assert(tableSize > 0u);
			--tableSize;
		}
	}
	while (succeeded && startTimestamp + testDuration > Timestamp(true));

	if (succeeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return succeeded;
}

bool TestHashSet::validationDynamicCapacitySingleIntegers(const unsigned int number, const unsigned int capacity, const double testDuration)
{
	ocean_assert(testDuration > 0);

	Log::info() << "Test validation " << number << " (single) elements with " << capacity << " initial capacity:";

	RandomGenerator randomGenerator;

	typedef HashSet<unsigned int> Table;
	typedef std::vector<unsigned int> Elements;
	typedef std::set<unsigned int> ElementSet;

	ElementSet elementSet;
	Elements elements;

	Table table(number);

	unsigned int tableSize = 0u;

	bool succeeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		if (!table.isEmpty())
		{
			succeeded = false;
			break;
		}

		table = Table(capacity);

		while (table.size() < number)
		{
			if (tableSize != table.size())
			{
				succeeded = false;
				break;
			}

			const unsigned int randomValue = RandomI::random(randomGenerator, 10000u);

			if (elementSet.find(randomValue) != elementSet.end())
				continue;

			const bool insertResult = table.insert(randomValue, true, true);
			if (!insertResult)
			{
				succeeded = false;
				break;
			}

			elementSet.insert(randomValue);
			elements.push_back(randomValue);

			for (Elements::const_iterator i = elements.begin(); i != elements.end(); ++i)
				if (!table.find(*i))
				{
					succeeded = false;
					break;
				}

			++tableSize;
		}

		while (tableSize != 0u)
		{
			if (tableSize != table.size())
			{
				succeeded = false;
				break;
			}

			const unsigned int randomIndex = RandomI::random(randomGenerator, (unsigned int)(elements.size()) - 1u);
			const unsigned int randomValue = elements[randomIndex];

			ocean_assert(elementSet.find(randomValue) != elementSet.end());

			const bool findResultBefore = table.find(randomValue);
			if (!findResultBefore)
			{
				succeeded = false;
				break;
			}

			const bool removeResult = table.remove(randomValue);
			if (!removeResult)
			{
				succeeded = false;
				break;
			}

			const bool findResultAfter = table.find(randomValue);
			if (findResultAfter)
			{
				succeeded = false;
				break;
			}

			elementSet.erase(randomValue);
			elements.erase(elements.begin() + randomIndex);

			for (Elements::const_iterator i = elements.begin(); i != elements.end(); ++i)
				if (!table.find(*i))
				{
					succeeded = false;
					break;
				}

			ocean_assert(tableSize > 0u);
			--tableSize;
		}
	}
	while (succeeded && startTimestamp + testDuration > Timestamp(true));

	if (succeeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return succeeded;
}

bool TestHashSet::validationMultipleIntegers(const unsigned int number, const unsigned int occupancy, const double testDuration)
{
	ocean_assert(occupancy > 0u && occupancy <= 100u);
	ocean_assert(testDuration > 0);

	Log::info() << "Test validation " << number << " (multiple) elements with " << occupancy << "% occupancy:";

	RandomGenerator randomGenerator;

	typedef HashSet<unsigned int> Table;
	typedef std::vector<unsigned int> Elements;
	typedef std::multiset<unsigned int> ElementSet;

	ElementSet elementSet;
	Elements elements;

	Table table(number);

	unsigned int tableSize = 0u;
	unsigned int tableCapacity = number;

	bool succeeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		if (!table.isEmpty())
		{
			succeeded = false;
			break;
		}

		while (tableSize * 100 / tableCapacity < occupancy)
		{
			if (tableSize != table.size())
			{
				succeeded = false;
				break;
			}

			if (tableCapacity != table.capacity())
			{
				succeeded = false;
				break;
			}

			const unsigned int randomValue = RandomI::random(randomGenerator, 50u);

			const bool insertResult = table.insert(randomValue, false, false);
			if (!insertResult)
			{
				succeeded = false;
				break;
			}

			elementSet.insert(randomValue);
			elements.push_back(randomValue);

			for (Elements::const_iterator i = elements.begin(); i != elements.end(); ++i)
				if (!table.find(*i))
				{
					succeeded = false;
					break;
				}

			++tableSize;
		}

		while (tableSize != 0u)
		{
			if (tableSize != table.size())
			{
				succeeded = false;
				break;
			}

			if (tableCapacity != table.capacity())
			{
				succeeded = false;
				break;
			}

			const unsigned int randomIndex = RandomI::random(randomGenerator, (unsigned int)(elements.size()) - 1u);
			const unsigned int randomValue = elements[randomIndex];

			ocean_assert(elementSet.find(randomValue) != elementSet.end());

			const bool findResultBefore = table.find(randomValue);
			if (!findResultBefore)
			{
				succeeded = false;
				break;
			}

			const bool removeResult = table.remove(randomValue);
			if (!removeResult)
			{
				succeeded = false;
				break;
			}

			elementSet.erase(elementSet.find(randomValue));
			elements.erase(elements.begin() + randomIndex);

			const bool findResultAfter = table.find(randomValue) == (elementSet.find(randomValue) != elementSet.end());
			if (!findResultAfter)
			{
				succeeded = false;
				break;
			}

			for (Elements::const_iterator i = elements.begin(); i != elements.end(); ++i)
				if (!table.find(*i))
				{
					succeeded = false;
					break;
				}

			ocean_assert(tableSize > 0u);
			--tableSize;
		}
	}
	while (succeeded && startTimestamp + testDuration > Timestamp(true));

	if (succeeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return succeeded;
}

}

}

}
