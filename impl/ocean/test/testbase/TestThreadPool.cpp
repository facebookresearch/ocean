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

bool TestThreadPool::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   ThreadPool tests:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	Log::info() << " ";

	allSucceeded = testInvokeFunctions(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "ThreadPool test succeeded.";
	}
	else
	{
		Log::info() << "ThreadPool test FAILED!";
	}

	return allSucceeded;
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

	bool allSucceeded = true;

	{
		// checking default behavior

		const ThreadPool threadPool;

		if (threadPool.capacity() < 1)
		{
			allSucceeded = false;
		}

		if (threadPool.size() != 0)
		{
			allSucceeded = false;
		}
	}

	const Timestamp startTimestamp(true);

	do
	{
		const size_t numberFunctions = size_t(RandomI::random(1u, 100u));

		UnorderedIndexSet32 ids;

		while (ids.size() != numberFunctions)
		{
			ids.emplace(RandomI::random32());
		}

		const size_t capacity = size_t(RandomI::random(1u, 100u));

		ThreadPool threadPool;
		if (!threadPool.setCapacity(capacity))
		{
			allSucceeded = false;
		}

		for (const Index32 id : ids)
		{
			threadPool.invoke(std::bind(&Executions::addId, &Executions::get(), id));
		}

		size_t expectedCapacity = capacity;

		size_t reducedCapacity = 0;
		if (RandomI::random(1u) == 0u)
		{
			reducedCapacity = size_t(RandomI::random(1u, (unsigned int)(capacity)));
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
					allSucceeded = false;
				}
			}
		}

		const UnorderedIndexSet32 executedIds(Executions::get().ids());

		if (executedIds.size() == ids.size())
		{
			for (const Index32 id : ids)
			{
				if (executedIds.find(id) == executedIds.cend())
				{
					allSucceeded = false;
					break;
				}
			}
		}
		else
		{
			allSucceeded = false;
		}

		if (threadPool.capacity() != expectedCapacity)
		{
			allSucceeded = false;
		}

		Executions::get().clear();
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
