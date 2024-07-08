/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestScopedObject.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/ScopedObject.h"
#include "ocean/base/Timestamp.h"

#include <functional>

namespace Ocean
{

namespace Test
{

namespace TestBase
{

uint64_t TestScopedObject::Manager::uniqueId()
{
	const ScopedLock scopedLock(lock_);

	return ++uniqueIdCounter_;
}

bool TestScopedObject::Manager::hasObject(const uint64_t id) const
{
	return numberObjects(id) != 0ull;
}

uint64_t TestScopedObject::Manager::numberObjects(const uint64_t id) const
{
	const ScopedLock scopedLock(lock_);

	const IdCounterMap::const_iterator i = idCounterMap_.find(id);

	if (i != idCounterMap_.cend())
	{
		return i->second;
	}

	return 0ull;
}

void TestScopedObject::Manager::addObject(const uint64_t id)
{
	const ScopedLock scopedLock(lock_);

	idCounterMap_[id]++;
}

bool TestScopedObject::Manager::removeObject(const uint64_t id)
{
	const ScopedLock scopedLock(lock_);

	const IdCounterMap::iterator i = idCounterMap_.find(id);
	ocean_assert(i != idCounterMap_.cend());

	if (i == idCounterMap_.cend())
	{
		return false;
	}

	if (--i->second == 0ull)
	{
		idCounterMap_.erase(i);
	}

	return true;
}

bool TestScopedObject::test(const double testDuration)
{
	ocean_assert(testDuration > 0);

	Log::info() << "---   ScopedObject test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testRuntime(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCompileTime(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "ScopedObject test succeeded.";
	}
	else
	{
		Log::info() << "ScopedObject test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestScopedObject, Runtime)
{
	EXPECT_TRUE(TestScopedObject::testRuntime(GTEST_TEST_DURATION));
}

TEST(TestScopedObject, CompileTime)
{
	EXPECT_TRUE(TestScopedObject::testCompileTime(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestScopedObject::testRuntime(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing ScopedObject with runtime release function:";

	const uint64_t uniqueId = Manager::get().uniqueId();

	bool allSucceeded = true;

	{
		// testing the default release function

		using ScopedObject = ScopedObjectT<Object*>;
		using ScopedObjects = std::vector<ScopedObject>;

		ScopedObjects scopedObjects(10); // testing invalid scoped objects
		scopedObjects.clear();

		const Timestamp startTimestamp(true);

		do
		{
			const unsigned int createObjects = RandomI::random(1u, 10u);

			for (unsigned int n = 0u; n < createObjects; ++n)
			{
				if (n % 2u == 0u)
				{
					scopedObjects.emplace_back(createObject(uniqueId), &TestScopedObject::releaseObject);
				}
				else
				{
					scopedObjects.push_back(ScopedObject(createObject(uniqueId), &TestScopedObject::releaseObject));
				}
			}

			const unsigned int releaseObjects = RandomI::random((unsigned int)(scopedObjects.size()));

			for (unsigned int n = 0u; n < releaseObjects; ++n)
			{
				scopedObjects.pop_back();
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		scopedObjects.clear();

		if (Manager::get().hasObject(uniqueId))
		{
			allSucceeded = false;
		}
	}

	{
		// testing a custom release function

		using ScopedObject = ScopedObjectT<Object*, Object*, std::function<void(Object*)>>;
		using ScopedObjects = std::vector<ScopedObject>;

		ScopedObjects scopedObjects(10); // testing invalid scoped objects
		scopedObjects.clear();

		const Timestamp startTimestamp(true);

		do
		{
			const unsigned int createObjects = RandomI::random(1u, 10u);

			for (unsigned int n = 0u; n < createObjects; ++n)
			{
				if (n % 2u == 0u)
				{
					scopedObjects.emplace_back(createObject(uniqueId), std::bind(&TestScopedObject::releaseObject, std::placeholders::_1));
				}
				else
				{
					scopedObjects.push_back(ScopedObject(createObject(uniqueId), std::bind(&TestScopedObject::releaseObject, std::placeholders::_1)));
				}
			}

			const unsigned int releaseObjects = RandomI::random((unsigned int)(scopedObjects.size()));

			for (unsigned int n = 0u; n < releaseObjects; ++n)
			{
				scopedObjects.pop_back();
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		scopedObjects.clear();

		if (Manager::get().hasObject(uniqueId))
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

bool TestScopedObject::testCompileTime(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing ScopedObject with compile time release function:";

	bool allSucceeded = true;

	{
		// testing object which needs to be released

		using ScopedObject = ScopedObjectCompileTimeVoidT<Object*, &TestScopedObject::releaseObject>;
		using ScopedObjects = std::vector<ScopedObject>;

		const uint64_t uniqueId = Manager::get().uniqueId();

		ScopedObjects scopedObjects(10); // testing invalid scoped objects
		scopedObjects.clear();

		const Timestamp startTimestamp(true);

		do
		{
			const unsigned int createObjects = RandomI::random(1u, 10u);

			for (unsigned int n = 0u; n < createObjects; ++n)
			{
				if (n % 2u == 0u)
				{
					scopedObjects.emplace_back(createObject(uniqueId));
				}
				else
				{
					scopedObjects.push_back(ScopedObject(createObject(uniqueId)));
				}
			}

			const unsigned int releaseObjects = RandomI::random((unsigned int)(scopedObjects.size()));

			for (unsigned int n = 0u; n < releaseObjects; ++n)
			{
				scopedObjects.pop_back();
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		scopedObjects.clear();

		if (Manager::get().hasObject(uniqueId))
		{
			allSucceeded = false;
		}
	}

	{
		// testing object which needs to be released

		constexpr bool needsRelease = false;

		using ScopedObject = ScopedObjectCompileTimeVoidT<Object*, &TestScopedObject::releaseObject>;
		using ScopedObjects = std::vector<ScopedObject>;

		ObjectPointers objectPoints;

		const uint64_t uniqueId = Manager::get().uniqueId();

		ScopedObjects scopedObjects(10); // testing invalid scoped objects
		scopedObjects.clear();

		uint64_t createdObjects = 0ull;

		const Timestamp startTimestamp(true);

		do
		{
			const unsigned int createObjects = RandomI::random(1u, 10u);

			for (unsigned int n = 0u; n < createObjects; ++n)
			{
				Object* object = createObject(uniqueId);

				if (n % 2u == 0u)
				{
					scopedObjects.emplace_back(object, needsRelease);
				}
				else
				{
					scopedObjects.push_back(ScopedObject(object, needsRelease));
				}

				objectPoints.emplace_back(object);
			}

			createdObjects += uint64_t(createObjects);

			const unsigned int releaseObjects = RandomI::random((unsigned int)(scopedObjects.size()));

			for (unsigned int n = 0u; n < releaseObjects; ++n)
			{
				scopedObjects.pop_back();
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		scopedObjects.clear();

		if (Manager::get().numberObjects(uniqueId) != createdObjects)
		{
			allSucceeded = false;
		}

		for (Object* objectPointer : objectPoints)
		{
			delete objectPointer;
		}
	}

	{
		// testing object which custom invalid value

		constexpr int32_t invalidValue = -1;

		using ScopedObject = ScopedObjectCompileTimeT<int32_t, uint32_t, bool, TestScopedObject::increaseCounter, true, true, invalidValue>;
		using ScopedObjects = std::vector<ScopedObject>;

		const Timestamp startTimestamp(true);

		do
		{
			const size_t numberInvalidObjects = size_t(RandomI::random(10u));

			ScopedObjects scopedObjects;

			for (size_t n = 0; n < numberInvalidObjects; ++n)
			{
				scopedObjects.emplace_back(invalidValue);

				if (scopedObjects.back().isValid())
				{
					allSucceeded = false;
				}
			}

			if (counterMap()[invalidValue] != 0)
			{
				allSucceeded = false;
			}

			if (!scopedObjects.empty() && RandomI::random(1u) == 0u)
			{
				scopedObjects.pop_back();
			}

			if (counterMap()[invalidValue] != 0)
			{
				allSucceeded = false;
			}

			const int32_t validId = int32_t(RandomI::random(10u));

			scopedObjects.emplace_back(validId);

			if (!scopedObjects.back().isValid())
			{
				allSucceeded = false;
			}

			scopedObjects.clear();

			if (counterMap()[int32_t(validId)] != 1)
			{
				allSucceeded = false;
			}

			if (counterMap()[invalidValue] != 0)
			{
				allSucceeded = false;
			}

			counterMap().clear();
		}
		while (startTimestamp + testDuration > Timestamp(true));
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

TestScopedObject::Object* TestScopedObject::createObject(const uint64_t id)
{
	ocean_assert(id != 0ull);

	Manager::get().addObject(id);

	return new Object(id);
}

void TestScopedObject::releaseObject(Object* object)
{
	ocean_assert(object != nullptr);

	Manager::get().removeObject(object->id());

	delete object;
}

bool TestScopedObject::increaseCounter(const uint32_t id)
{
	ocean_assert(id != uint32_t(-1));

	counterMap()[id]++;

	return true;
}

TestScopedObject::CounterMap& TestScopedObject::counterMap()
{
	static CounterMap counterMap;
	return counterMap;
}

}

}

}
