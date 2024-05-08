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

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestRingMap::test(const double testDuration)
{
	ocean_assert(testDuration > 0);

	Log::info() << "---   RingMap test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testInsert(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testChangeCapacity(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCheckout(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRefresh(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "RingMap test succeeded.";
	}
	else
	{
		Log::info() << "RingMap test FAILED!";
	}

	return allSucceeded;
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

	bool allSucceeded = true;
	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int capacity = RandomI::random(20, 2000);

		StringMap stringMap(capacity);

		for (unsigned int n = 0u; n < capacity; ++n)
		{
			if (RandomI::random(1u) == 0u)
			{
				// moving the element

				std::string element = String::toAString(n);

				if (!stringMap.insertElement(n, std::move(element), false))
				{
					allSucceeded = false;
				}
			}
			else
			{
				// copying the element

				const std::string element = String::toAString(n);

				if (!stringMap.insertElement(n, element, false))
				{
					allSucceeded = false;
				}
			}

			if (!stringMap.hasElement(n))
			{
				allSucceeded = false;
			}
		}

		if (stringMap.size() != capacity || stringMap.isEmpty())
		{
			allSucceeded = false;
		}

		for (unsigned int n = 0u; n < capacity; ++n)
		{
			std::string element;
			if (!stringMap.element(n, element) || element != String::toAString(n))
			{
				allSucceeded = false;
			}
		}

		// now, we add more elements

		for (unsigned int n = capacity; n < capacity * 2u; ++n)
		{
			if (RandomI::random(1u) == 0u)
			{
				// moving the element

				std::string element = String::toAString(n);

				if (!stringMap.insertElement(n, std::move(element), false))
				{
					allSucceeded = false;
				}
			}
			else
			{
				// copying the element

				const std::string element = String::toAString(n);

				if (!stringMap.insertElement(n, element, false))
				{
					allSucceeded = false;
				}
			}

			std::string element;
			if (stringMap.element(n - capacity, element))
			{
				allSucceeded = false;
			}

			if (!stringMap.element(n, element) || element != String::toAString(n))
			{
				allSucceeded = false;
			}
		}

		for (unsigned int n = capacity; n < capacity * 2u; ++n)
		{
			std::string element;
			if (!stringMap.element(n, element) || element != String::toAString(n))
			{
				allSucceeded = false;
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

bool TestRingMap::testChangeCapacity(const double testDuration)
{
	Log::info() << "Change capacity test:";

	bool allSucceeded = true;
	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int capacity = RandomI::random(20u, 2000u);

		StringMap stringMap;

		if (stringMap.capacity() != 0)
		{
			allSucceeded = false;
		}

		stringMap.setCapacity(capacity);

		if (stringMap.capacity() != capacity)
		{
			allSucceeded = false;
		}

		for (unsigned int n = 0u; n < capacity; ++n)
		{
			if (RandomI::random(1u) == 0u)
			{
				// moving the element

				std::string element = String::toAString(n);

				if (!stringMap.insertElement(n, std::move(element), false))
				{
					allSucceeded = false;
				}
			}
			else
			{
				// copying the element

				const std::string element = String::toAString(n);

				if (!stringMap.insertElement(n, element, false))
				{
					allSucceeded = false;
				}
			}
		}

		const unsigned int smallCapacity = RandomI::random(5u, capacity - 1u);
		stringMap.setCapacity(smallCapacity);

		for (unsigned int n = 0u; n < capacity; ++n)
		{
			const bool hasElement = stringMap.hasElement(n);

			if (hasElement && n < capacity - smallCapacity)
			{
				allSucceeded = false;
			}

			if (!hasElement && n >= capacity - smallCapacity)
			{
				allSucceeded = false;
			}
		}

		const unsigned int bigCapacity = RandomI::random(smallCapacity + 10u, 4000u);
		stringMap.setCapacity(bigCapacity);

		if (!stringMap.insertElement(capacity + 1u, String::toAString(capacity + 1u), false))
		{
			allSucceeded = false;
		}

		for (unsigned int n = 0u; n < capacity; ++n)
		{
			const bool hasElement = stringMap.hasElement(n);

			if (hasElement && n < capacity - smallCapacity)
			{
				allSucceeded = false;
			}

			if (!hasElement && n >= capacity - smallCapacity)
			{
				allSucceeded = false;
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

bool TestRingMap::testCheckout(const double testDuration)
{
	Log::info() << "Checkout test:";

	bool allSucceeded = true;
	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int capacity = RandomI::random(20, 2000);

		StringMap stringMap(capacity);

		UnorderedIndexSet32 checkedOut;

		for (unsigned int n = 0u; n < capacity; ++n)
		{
			if (RandomI::random(1u) == 0u)
			{
				// moving the element

				std::string element = String::toAString(n);

				if (!stringMap.insertElement(n, std::move(element), false))
				{
					allSucceeded = false;
				}
			}
			else
			{
				// copying the element

				const std::string element = String::toAString(n);

				if (!stringMap.insertElement(n, element, false))
				{
					allSucceeded = false;
				}
			}

			for (unsigned int i = 0u; i <= n; ++i)
			{
				const bool hasBeenCheckedOut = checkedOut.find(i) != checkedOut.cend();

				if (hasBeenCheckedOut == stringMap.hasElement(i))
				{
					allSucceeded = false;
				}
			}

			if (RandomI::random(1u) == 0u)
			{
				const unsigned int index = RandomI::random(0u, capacity + 10u);

				const bool hasBeenCheckedOutBefore = checkedOut.find(index) != checkedOut.cend();
				const bool couldBeInMap = index <= n;

				const bool expectedResult = couldBeInMap && !hasBeenCheckedOutBefore;

				std::string element;
				const bool checkoutResult = stringMap.checkoutElement(index, element);

				if (checkoutResult)
				{
					if (element != String::toAString(index))
					{
						allSucceeded = false;
					}

					if (stringMap.hasElement(index))
					{
						allSucceeded = false;
					}
				}

				if (checkoutResult != expectedResult)
				{
					allSucceeded = false;
				}

				if (expectedResult)
				{
					checkedOut.emplace(index);
				}

				if (stringMap.size() != size_t(n + 1u) - checkedOut.size())
				{
					allSucceeded = false;
				}
			}

			for (unsigned int i = 0u; i <= n; ++i)
			{
				const bool hasBeenCheckedOut = checkedOut.find(i) != checkedOut.cend();

				if (hasBeenCheckedOut == stringMap.hasElement(i))
				{
					allSucceeded = false;
				}
			}
		}

		for (unsigned int n = 0u; n < capacity; ++n)
		{
			if (checkedOut.find(n) == checkedOut.cend())
			{
				std::string element;
				if (!stringMap.checkoutElement(n, element) || element != String::toAString(n))
				{
					allSucceeded = false;
				}

				checkedOut.emplace(n);
			}
		}

		if (stringMap.size() != 0 || !stringMap.isEmpty())
		{
			allSucceeded = false;
		}

		if (checkedOut.size() != capacity)
		{
			allSucceeded = false;
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

bool TestRingMap::testRefresh(const double testDuration)
{
	Log::info() << "Refresh test:";

	bool allSucceeded = true;
	const Timestamp startTimestamp(true);

	do
	{
		{
			// refreshing one element

			const unsigned int capacity = RandomI::random(20, 2000);

			StringMap stringMap(capacity);

			for (unsigned int n = 0u; n < capacity; ++n)
			{
				if (!stringMap.insertElement(n, String::toAString(n), false))
				{
					allSucceeded = false;
				}
			}

			// now, we add more elements

			const unsigned int lastRemainingElement = RandomI::random(capacity - 1u);

			if (!stringMap.refreshElement(lastRemainingElement))
			{
				allSucceeded = false;
			}

			for (unsigned int n = capacity; n < capacity * 2u; ++n)
			{
				if (!stringMap.insertElement(n, String::toAString(n), false))
				{
					allSucceeded = false;
				}

				const bool lastIteration = n + 1u == capacity * 2u;

				if (stringMap.hasElement(lastRemainingElement) == lastIteration)
				{
					allSucceeded = false;
				}
			}
		}

		{
			// refreshing all elements

			const unsigned int capacity = RandomI::random(20, 2000);

			StringMap stringMap(capacity);

			Indices32 refreshOrder;

			for (unsigned int n = 0u; n < capacity; ++n)
			{
				if (!stringMap.insertElement(n, String::toAString(n), false))
				{
					allSucceeded = false;
				}

				refreshOrder.emplace_back(n);
			}

			for (unsigned int n = 0u; n < capacity; ++n)
			{
				std::swap(refreshOrder[RandomI::random(capacity - 1u)], refreshOrder.back());
			}

			for (const Index32& index : refreshOrder)
			{
				if (!stringMap.refreshElement(index))
				{
					allSucceeded = false;
				}
			}

			// now, we add more elements

			for (unsigned int n = capacity; n < capacity * 2u; ++n)
			{
				if (!stringMap.hasElement(refreshOrder[n - capacity]))
				{
					allSucceeded = false;
				}

				if (!stringMap.insertElement(n, String::toAString(n), false))
				{
					allSucceeded = false;
				}

				if (stringMap.hasElement(refreshOrder[n - capacity]))
				{
					allSucceeded = false;
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
