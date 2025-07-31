/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestHashMap.h"

#include "ocean/base/HashMap.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include <vector>
#include <map>

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestHashMap::test(const double testDuration)
{
	Log::info() << "---   Hash map test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testSingleIntegers(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testMultipleIntegers(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
		Log::info() << "Hash map test succeeded.";
	else
		Log::info() << "Hash map test FAILED!";

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestHashMap, SingleIntegers) {
	EXPECT_TRUE(TestHashMap::testSingleIntegers(GTEST_TEST_DURATION));
}

TEST(TestHashMap, MultipleIntegers) {
	EXPECT_TRUE(TestHashMap::testMultipleIntegers(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestHashMap::testSingleIntegers(const double testDuration)
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

bool TestHashMap::testMultipleIntegers(const double testDuration)
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

bool TestHashMap::testPerformanceSingleIntegers(const unsigned int number, const unsigned int occupancy, const double testDuration)
{
	ocean_assert(occupancy > 0u && occupancy <= 100u);
	ocean_assert(testDuration > 0);

	Log::info() << "Test performance " << number << " (single) elements with " << occupancy << "% occupancy:";

	typedef HashMap<unsigned int, double> Table;
	typedef std::map<unsigned int, double> ElementMap;
	typedef std::vector<std::pair<unsigned int, double> > Elements;

	ElementMap tmpElementMap;
	Elements addElements;
	Elements removeElements;

	const unsigned int numberElements = number * occupancy / 100;
	while (addElements.size() < numberElements)
	{
		const unsigned int randomNumber = RandomI::random(10u * number);

		if (tmpElementMap.find(randomNumber) == tmpElementMap.end())
		{
			const double randomValue = double(RandomI::random(-500, 500)) + 0.5;

			tmpElementMap.insert(std::make_pair(randomNumber, randomValue));
			addElements.push_back(std::make_pair(randomNumber, randomValue));
		}

	}

	tmpElementMap.clear();

	while (removeElements.size() < addElements.size() / 10)
	{
		const unsigned int randomIndex = RandomI::random((unsigned int)addElements.size() - 1u);
		const unsigned int randomNumber = addElements[randomIndex].first;
		const double randomValue = addElements[randomIndex].second;

		if (tmpElementMap.find(randomNumber) == tmpElementMap.end())
		{
			tmpElementMap.insert(std::make_pair(randomNumber, randomValue));
			removeElements.push_back(std::make_pair(randomNumber, randomValue));
		}
	}

	const Timestamp startTimestamp(true);

	HighPerformanceStatistic stdPerformance, hashPerformance;

	do
	{
		{
			const HighPerformanceStatistic::ScopedStatistic performance(stdPerformance);

			ElementMap elementMap;

			for (Elements::const_iterator i = addElements.begin(); i != addElements.end(); ++i)
				elementMap.insert(*i);

			for (Elements::const_iterator i = removeElements.begin(); i != removeElements.end(); ++i)
				elementMap.erase(i->first);
		}

		{
			const HighPerformanceStatistic::ScopedStatistic performance(hashPerformance);

			Table table(number);

			for (Elements::const_iterator i = addElements.begin(); i != addElements.end(); ++i)
				table.insert(i->first, i->second);

			for (Elements::const_iterator i = removeElements.begin(); i != removeElements.end(); ++i)
				table.remove(i->first);
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	const double boostBest = hashPerformance.best() == 0 ? -1 : stdPerformance.best() / hashPerformance.best();
	const double boostWorst = hashPerformance.worst() == 0 ? -1 : stdPerformance.worst() / hashPerformance.worst();
	const double boostAverage = hashPerformance.average() == 0 ? -1 : stdPerformance.average() / hashPerformance.average();

	Log::info() << "Std performance: Best: " << stdPerformance.bestMseconds() << "ms, worst: " << stdPerformance.worstMseconds() << "ms, average: " << stdPerformance.averageMseconds() << "ms";
	Log::info() << "Hash performance: Best: " << hashPerformance.bestMseconds() << "ms, worst: " << hashPerformance.worstMseconds() << "ms, average: " << hashPerformance.averageMseconds() << "ms";
	Log::info() << "Boost factor: Best: " << String::toAString(boostBest, 1u) << "x, worst: " << String::toAString(boostWorst, 1u) << "x, average: " << String::toAString(boostAverage, 1u) << "x";

	return true;
}

bool TestHashMap::validationStaticCapacitySingleIntegers(const unsigned int number, const unsigned int occupancy, const double testDuration)
{
	ocean_assert(occupancy > 0u && occupancy <= 100u);
	ocean_assert(testDuration > 0);

	Log::info() << "Test " << number << " (single) elements with " << occupancy << "% occupancy:";

	typedef HashMap<unsigned int, double> Table;
	typedef std::vector<std::pair<unsigned int, double> > Elements;
	typedef std::map<unsigned int, double> ElementMap;

	ElementMap elementMap;
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

			const unsigned int randomValue = RandomI::random(10000u);

			if (elementMap.find(randomValue) != elementMap.end())
				continue;

			const double randomParameter = double(RandomI::random(-500, 500)) + 0.5;

			const bool insertResult = table.insert(randomValue, randomParameter, true, false);
			if (!insertResult)
			{
				succeeded = false;
				break;
			}

			elementMap.insert(std::make_pair(randomValue, randomParameter));
			elements.push_back(std::make_pair(randomValue, randomParameter));

			const double* checkValue;
			for (Elements::const_iterator i = elements.begin(); i != elements.end(); ++i)
				if (!table.find(i->first, checkValue) || i->second != *checkValue)
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

			const unsigned int randomIndex = RandomI::random((unsigned int)elements.size() - 1u);
			const unsigned int randomValue = elements[randomIndex].first;
			const double randomParameter = elements[randomIndex].second;

			ocean_assert(elementMap.find(randomValue) != elementMap.end());

			const double* checkValue;
			bool findResultBefore = table.find(randomValue, checkValue);
			if (!findResultBefore || *checkValue != randomParameter)
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

			elementMap.erase(randomValue);
			elements.erase(elements.begin() + randomIndex);

			for (Elements::const_iterator i = elements.begin(); i != elements.end(); ++i)
				if (!table.find(i->first, checkValue) || i->second != *checkValue)
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

bool TestHashMap::validationDynamicCapacitySingleIntegers(const unsigned int number, const unsigned int capacity, const double testDuration)
{
	ocean_assert(testDuration > 0);

	Log::info() << "Test " << number << " (single) elements with " << capacity << " initial capacity:";

	typedef HashMap<unsigned int, double> Table;
	typedef std::vector<std::pair<unsigned int, double> > Elements;
	typedef std::map<unsigned int, double> ElementMap;

	ElementMap elementMap;
	Elements elements;

	Table table(capacity);

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

			const unsigned int randomValue = RandomI::random(10000u);

			if (elementMap.find(randomValue) != elementMap.end())
				continue;

			const double randomParameter = double(RandomI::random(-500, 500)) + 0.5;

			const bool insertResult = table.insert(randomValue, randomParameter, true, true);
			if (!insertResult)
			{
				succeeded = false;
				break;
			}

			elementMap.insert(std::make_pair(randomValue, randomParameter));
			elements.push_back(std::make_pair(randomValue, randomParameter));

			const double* checkValue;
			for (Elements::const_iterator i = elements.begin(); i != elements.end(); ++i)
				if (!table.find(i->first, checkValue) || i->second != *checkValue)
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

			const unsigned int randomIndex = RandomI::random((unsigned int)elements.size() - 1u);
			const unsigned int randomValue = elements[randomIndex].first;
			const double randomParameter = elements[randomIndex].second;

			ocean_assert(elementMap.find(randomValue) != elementMap.end());

			const double* checkValue;
			bool findResultBefore = table.find(randomValue, checkValue);
			if (!findResultBefore || *checkValue != randomParameter)
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

			elementMap.erase(randomValue);
			elements.erase(elements.begin() + randomIndex);

			for (Elements::const_iterator i = elements.begin(); i != elements.end(); ++i)
				if (!table.find(i->first, checkValue) || i->second != *checkValue)
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

bool TestHashMap::validationMultipleIntegers(const unsigned int number, const unsigned int occupancy, const double testDuration)
{
	ocean_assert(occupancy > 0u && occupancy <= 100u);
	ocean_assert(testDuration > 0);

	Log::info() << "Test " << number << " (multiple) elements with " << occupancy << "% occupancy:";

	typedef HashMap<unsigned int, double> Table;
	typedef std::vector<std::pair<unsigned int, double> > Elements;
	typedef std::multimap<unsigned int, double> ElementMap;

	ElementMap elementMap;
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

			const unsigned int randomValue = RandomI::random(50u);
			const double randomParameter = double(RandomI::random(-500, 500)) + 0.5;

			const bool insertResult = table.insert(randomValue, randomParameter, false, false);
			if (!insertResult)
			{
				succeeded = false;
				break;
			}

			elementMap.insert(std::make_pair(randomValue, randomParameter));
			elements.push_back(std::make_pair(randomValue, randomParameter));

			for (Elements::const_iterator i = elements.begin(); i != elements.end(); ++i)
				if (!table.find(i->first))
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

			const unsigned int randomIndex = RandomI::random((unsigned int)elements.size() - 1u);
			const unsigned int randomValue = elements[randomIndex].first;

			ocean_assert(elementMap.find(randomValue) != elementMap.end());

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

			elementMap.erase(elementMap.find(randomValue));
			elements.erase(elements.begin() + randomIndex);

			const bool findResultAfter = table.find(randomValue) == (elementMap.find(randomValue) != elementMap.end());
			if (!findResultAfter)
			{
				succeeded = false;
				break;
			}

			for (Elements::const_iterator i = elements.begin(); i != elements.end(); ++i)
				if (!table.find(i->first))
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
