/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestThreadPool.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Processor.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/ThreadPool.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"
#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

void TestThreadPool::Executions::addId(const unsigned int id)
{
	const ScopedLock scopedLock(lock_);

	ids_.emplace(id);
}

UnorderedIndexSet32 TestThreadPool::Executions::ids() const
{
	const ScopedLock scopedLock(lock_);

	return ids_;
}

bool TestThreadPool::Executions::hasId() const
{
	const ScopedLock scopedLock(lock_);

	return !ids_.empty();
}

void TestThreadPool::Executions::clear()
{
	const ScopedLock scopedLock(lock_);

	ids_.clear();
}

bool TestThreadPool::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("ThreadPool test");
	Log::info() << " ";

	if (selector.shouldRun("invokefunctions"))
	{
		testResult = testInvokeFunctions(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestThreadPool, InvokeFunctions)
{
	EXPECT_TRUE(TestThreadPool::testInvokeFunctions(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestThreadPool::testInvokeFunctions(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test invoke functions:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	{
		// checking default behavior

		const ThreadPool threadPool;

		OCEAN_EXPECT_GREATER_EQUAL(validation, threadPool.capacity(), size_t(1));

		OCEAN_EXPECT_EQUAL(validation, threadPool.size(), size_t(0));
	}

	const Timestamp startTimestamp(true);

	do
	{
		const size_t numberFunctions = size_t(RandomI::random(randomGenerator, 1u, 100u));

		UnorderedIndexSet32 ids;

		while (ids.size() != numberFunctions)
		{
			ids.emplace(RandomI::random32(randomGenerator));
		}

		const size_t capacity = size_t(RandomI::random(randomGenerator, 1u, 100u));

		ThreadPool threadPool;
		OCEAN_EXPECT_TRUE(validation, threadPool.setCapacity(capacity));

		for (const Index32 id : ids)
		{
			threadPool.invoke(std::bind(&Executions::addId, &Executions::get(), id));
		}

		size_t expectedCapacity = capacity;

		size_t reducedCapacity = 0;
		if (RandomI::boolean(randomGenerator))
		{
			reducedCapacity = size_t(RandomI::random(randomGenerator, 1u, (unsigned int)(capacity)));
		}

		const Timestamp iterationTimestamp(true);

		while (iterationTimestamp + 5.0 > Timestamp(true))
		{
			if (threadPool.size() == 0 && Executions::get().hasId())
			{
				break;
			}

			Thread::sleep(1u);

			if (reducedCapacity != 0)
			{
				if (threadPool.setCapacity(reducedCapacity))
				{
					expectedCapacity = reducedCapacity;
				}
				else
				{
					OCEAN_SET_FAILED(validation);
				}
			}
		}

		const UnorderedIndexSet32 executedIds(Executions::get().ids());

		OCEAN_EXPECT_EQUAL(validation, executedIds.size(), ids.size());

		if (executedIds.size() == ids.size())
		{
			for (const Index32 id : ids)
			{
				OCEAN_EXPECT_NOT_EQUAL(validation, executedIds.find(id), executedIds.cend());
			}
		}

		OCEAN_EXPECT_EQUAL(validation, threadPool.capacity(), expectedCapacity);

		Executions::get().clear();
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
