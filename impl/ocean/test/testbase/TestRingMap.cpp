/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestRingMap.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"
#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestRingMap::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0);

	TestResult testResult("RingMap test");
	Log::info() << " ";

	if (selector.shouldRun("insert"))
	{
		testResult = testInsert(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("changecapacity"))
	{
		testResult = testChangeCapacity(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("checkout"))
	{
		testResult = testCheckout(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("refresh"))
	{
		testResult = testRefresh(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestRingMap, Insert)
{
	EXPECT_TRUE(TestRingMap::testInsert(GTEST_TEST_DURATION));
}

TEST(TestRingMap, ChangeCapacity)
{
	EXPECT_TRUE(TestRingMap::testChangeCapacity(GTEST_TEST_DURATION));
}

TEST(TestRingMap, Checkout)
{
	EXPECT_TRUE(TestRingMap::testCheckout(GTEST_TEST_DURATION));
}

TEST(TestRingMap, Refresh)
{
	EXPECT_TRUE(TestRingMap::testRefresh(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestRingMap::testInsert(const double testDuration)
{
	Log::info() << "Insert test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int capacity = RandomI::random(randomGenerator, 20, 2000);

		StringMap stringMap(capacity);

		for (unsigned int n = 0u; n < capacity; ++n)
		{
			if (RandomI::boolean(randomGenerator))
			{
				// moving the element

				std::string element = String::toAString(n);

				OCEAN_EXPECT_TRUE(validation, stringMap.insertElement(n, std::move(element), false));
			}
			else
			{
				// copying the element

				const std::string element = String::toAString(n);

				OCEAN_EXPECT_TRUE(validation, stringMap.insertElement(n, element, false));
			}

			OCEAN_EXPECT_TRUE(validation, stringMap.hasElement(n));
		}

		OCEAN_EXPECT_EQUAL(validation, stringMap.size(), size_t(capacity));
		OCEAN_EXPECT_FALSE(validation, stringMap.isEmpty());

		for (unsigned int n = 0u; n < capacity; ++n)
		{
			std::string element;
			OCEAN_EXPECT_TRUE(validation, stringMap.element(n, element) && element == String::toAString(n));
		}

		// now, we add more elements

		for (unsigned int n = capacity; n < capacity * 2u; ++n)
		{
			if (RandomI::boolean(randomGenerator))
			{
				// moving the element

				std::string element = String::toAString(n);

				OCEAN_EXPECT_TRUE(validation, stringMap.insertElement(n, std::move(element), false));
			}
			else
			{
				// copying the element

				const std::string element = String::toAString(n);

				OCEAN_EXPECT_TRUE(validation, stringMap.insertElement(n, element, false));
			}

			std::string element;
			OCEAN_EXPECT_FALSE(validation, stringMap.element(n - capacity, element));

			OCEAN_EXPECT_TRUE(validation, stringMap.element(n, element) && element == String::toAString(n));
		}

		for (unsigned int n = capacity; n < capacity * 2u; ++n)
		{
			std::string element;
			OCEAN_EXPECT_TRUE(validation, stringMap.element(n, element) && element == String::toAString(n));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestRingMap::testChangeCapacity(const double testDuration)
{
	Log::info() << "Change capacity test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int capacity = RandomI::random(randomGenerator, 20u, 2000u);

		StringMap stringMap;

		OCEAN_EXPECT_EQUAL(validation, stringMap.capacity(), size_t(0));

		stringMap.setCapacity(capacity);

		OCEAN_EXPECT_EQUAL(validation, stringMap.capacity(), size_t(capacity));

		for (unsigned int n = 0u; n < capacity; ++n)
		{
			if (RandomI::boolean(randomGenerator))
			{
				// moving the element

				std::string element = String::toAString(n);

				OCEAN_EXPECT_TRUE(validation, stringMap.insertElement(n, std::move(element), false));
			}
			else
			{
				// copying the element

				const std::string element = String::toAString(n);

				OCEAN_EXPECT_TRUE(validation, stringMap.insertElement(n, element, false));
			}
		}

		const unsigned int smallCapacity = RandomI::random(randomGenerator, 5u, capacity - 1u);
		stringMap.setCapacity(smallCapacity);

		for (unsigned int n = 0u; n < capacity; ++n)
		{
			const bool hasElement = stringMap.hasElement(n);

			if (hasElement && n < capacity - smallCapacity)
			{
				OCEAN_SET_FAILED(validation);
			}

			if (!hasElement && n >= capacity - smallCapacity)
			{
				OCEAN_SET_FAILED(validation);
			}
		}

		const unsigned int bigCapacity = RandomI::random(randomGenerator, smallCapacity + 10u, 4000u);
		stringMap.setCapacity(bigCapacity);

		OCEAN_EXPECT_TRUE(validation, stringMap.insertElement(capacity + 1u, String::toAString(capacity + 1u), false));

		for (unsigned int n = 0u; n < capacity; ++n)
		{
			const bool hasElement = stringMap.hasElement(n);

			if (hasElement && n < capacity - smallCapacity)
			{
				OCEAN_SET_FAILED(validation);
			}

			if (!hasElement && n >= capacity - smallCapacity)
			{
				OCEAN_SET_FAILED(validation);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestRingMap::testCheckout(const double testDuration)
{
	Log::info() << "Checkout test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int capacity = RandomI::random(randomGenerator, 20, 2000);

		StringMap stringMap(capacity);

		UnorderedIndexSet32 checkedOut;

		for (unsigned int n = 0u; n < capacity; ++n)
		{
			if (RandomI::boolean(randomGenerator))
			{
				// moving the element

				std::string element = String::toAString(n);

				OCEAN_EXPECT_TRUE(validation, stringMap.insertElement(n, std::move(element), false));
			}
			else
			{
				// copying the element

				const std::string element = String::toAString(n);

				OCEAN_EXPECT_TRUE(validation, stringMap.insertElement(n, element, false));
			}

			for (unsigned int i = 0u; i <= n; ++i)
			{
				const bool hasBeenCheckedOut = checkedOut.find(i) != checkedOut.cend();

				OCEAN_EXPECT_NOT_EQUAL(validation, hasBeenCheckedOut, stringMap.hasElement(i));
			}

			if (RandomI::boolean(randomGenerator))
			{
				const unsigned int index = RandomI::random(randomGenerator, 0u, capacity + 10u);

				const bool hasBeenCheckedOutBefore = checkedOut.find(index) != checkedOut.cend();
				const bool couldBeInMap = index <= n;

				const bool expectedResult = couldBeInMap && !hasBeenCheckedOutBefore;

				std::string element;
				const bool checkoutResult = stringMap.checkoutElement(index, element);

				if (checkoutResult)
				{
					OCEAN_EXPECT_EQUAL(validation, element, String::toAString(index));

					OCEAN_EXPECT_FALSE(validation, stringMap.hasElement(index));
				}

				OCEAN_EXPECT_EQUAL(validation, checkoutResult, expectedResult);

				if (expectedResult)
				{
					checkedOut.emplace(index);
				}

				OCEAN_EXPECT_EQUAL(validation, stringMap.size(), size_t(n + 1u) - checkedOut.size());
			}

			for (unsigned int i = 0u; i <= n; ++i)
			{
				const bool hasBeenCheckedOut = checkedOut.find(i) != checkedOut.cend();

				OCEAN_EXPECT_NOT_EQUAL(validation, hasBeenCheckedOut, stringMap.hasElement(i));
			}
		}

		for (unsigned int n = 0u; n < capacity; ++n)
		{
			if (checkedOut.find(n) == checkedOut.cend())
			{
				std::string element;
				OCEAN_EXPECT_TRUE(validation, stringMap.checkoutElement(n, element) && element == String::toAString(n));

				checkedOut.emplace(n);
			}
		}

		OCEAN_EXPECT_EQUAL(validation, stringMap.size(), size_t(0));
		OCEAN_EXPECT_TRUE(validation, stringMap.isEmpty());

		OCEAN_EXPECT_EQUAL(validation, checkedOut.size(), size_t(capacity));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestRingMap::testRefresh(const double testDuration)
{
	Log::info() << "Refresh test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		{
			// refreshing one element

			const unsigned int capacity = RandomI::random(randomGenerator, 20, 2000);

			StringMap stringMap(capacity);

			for (unsigned int n = 0u; n < capacity; ++n)
			{
				OCEAN_EXPECT_TRUE(validation, stringMap.insertElement(n, String::toAString(n), false));
			}

			// now, we add more elements

			const unsigned int lastRemainingElement = RandomI::random(randomGenerator, capacity - 1u);

			OCEAN_EXPECT_TRUE(validation, stringMap.refreshElement(lastRemainingElement));

			for (unsigned int n = capacity; n < capacity * 2u; ++n)
			{
				OCEAN_EXPECT_TRUE(validation, stringMap.insertElement(n, String::toAString(n), false));

				const bool lastIteration = n + 1u == capacity * 2u;

				OCEAN_EXPECT_NOT_EQUAL(validation, stringMap.hasElement(lastRemainingElement), lastIteration);
			}
		}

		{
			// refreshing all elements

			const unsigned int capacity = RandomI::random(randomGenerator, 20, 2000);

			StringMap stringMap(capacity);

			Indices32 refreshOrder;

			for (unsigned int n = 0u; n < capacity; ++n)
			{
				OCEAN_EXPECT_TRUE(validation, stringMap.insertElement(n, String::toAString(n), false));

				refreshOrder.emplace_back(n);
			}

			for (unsigned int n = 0u; n < capacity; ++n)
			{
				std::swap(refreshOrder[RandomI::random(randomGenerator, capacity - 1u)], refreshOrder.back());
			}

			for (const Index32& index : refreshOrder)
			{
				OCEAN_EXPECT_TRUE(validation, stringMap.refreshElement(index));
			}

			// now, we add more elements

			for (unsigned int n = capacity; n < capacity * 2u; ++n)
			{
				OCEAN_EXPECT_TRUE(validation, stringMap.hasElement(refreshOrder[n - capacity]));

				OCEAN_EXPECT_TRUE(validation, stringMap.insertElement(n, String::toAString(n), false));

				OCEAN_EXPECT_FALSE(validation, stringMap.hasElement(refreshOrder[n - capacity]));
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
