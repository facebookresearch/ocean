/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestLock.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"
#include "ocean/test/Validation.h"

#include <thread>

namespace Ocean
{

namespace Test
{

namespace TestBase
{

Lock& TestLock::staticLockObject()
{
	static Lock lockObject;
	return lockObject;
}

bool TestLock::test(const TestSelector& selector)
{
	TestResult testResult("Lock test");
	Log::info() << " ";

	if (selector.shouldRun("lockunlock"))
	{
		testResult = testLockUnlock();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("scopedlock"))
	{
		testResult = testScopedLock();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("staticscopedlock"))
	{
		testResult = testStaticScopedLock();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("templatedlock"))
	{
		testResult = testTemplatedLock();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("dualscopedlock"))
	{
		testResult = testDualScopedLock();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestLock, LockUnlock)
{
	EXPECT_TRUE(TestLock::testLockUnlock());
}

TEST(TestLock, ScopedLock)
{
	EXPECT_TRUE(TestLock::testScopedLock());
}

TEST(TestLock, StaticScopedLock)
{
	EXPECT_TRUE(TestLock::testStaticScopedLock());
}

TEST(TestLock, TemplatedLock)
{
	EXPECT_TRUE(TestLock::testTemplatedLock());
}

TEST(TestLock, DualScopedLock)
{
	EXPECT_TRUE(TestLock::testDualScopedLock());
}

#endif // OCEAN_USE_GTEST

bool TestLock::testLockUnlock()
{
	Log::info() << "Testing lock/unlock object:";

	Lock lockObject;
	lockObject.lock();
	lockObject.unlock();

	Log::info() << "Validation: succeeded.";
	return true;
}

bool TestLock::testScopedLock()
{
	Log::info() << "Testing scoped lock object:";

	{
		Lock lockObject;
		const ScopedLock scopedLock(lockObject);
		const ScopedLock scopedLock2(staticLockObject());
	}

	Log::info() << "Validation: succeeded.";
	return true;
}

bool TestLock::testStaticScopedLock()
{
	Log::info() << "Testing static scoped lock object:";
	static Lock staticLocalLockObject;
	const ScopedLock scopedLock(staticLocalLockObject);

	Log::info() << "Validation: succeeded.";
	return true;
}

bool TestLock::testTemplatedLock()
{
	Log::info() << "Testing templated lock:";

	Validation validation;

	static_assert(sizeof(Empty) == 1, "Invalid data type!"); // empty classes always have 1 byte

	static_assert(sizeof(TemplatedLock<true>) > 1, "Invalid data type!");
	static_assert(sizeof(TemplatedLock<false>) == 1, "Invalid data type!");

	if constexpr (sizeof(TemplatedLock<true>) <= 1)
	{
		OCEAN_SET_FAILED(validation);
	}

	if constexpr (sizeof(TemplatedLock<false>) != 1)
	{
		OCEAN_SET_FAILED(validation);
	}

	static_assert(sizeof(Object<true>) > 1, "Invalid data type!");
	static_assert(sizeof(Object<false>) == 1, "Invalid data type!");

	if constexpr (sizeof(Object<true>) <= 1)
	{
		OCEAN_SET_FAILED(validation);
	}

	if constexpr (sizeof(Object<false>) != 1)
	{
		OCEAN_SET_FAILED(validation);
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

void TestLock::threadFunction(Lock& firstLock, Lock& secondLock, unsigned int& counter, const unsigned int iterations)
{
	for (unsigned int n = 0u; n < iterations; ++n)
	{
		const DualScopedLockT<ScopedLock, Lock> dualLock(firstLock, secondLock);
		counter++;
		Thread::sleep(1u);
	}
}

bool TestLock::testDualScopedLock()
{
	Log::info() << "Testing dual scoped lock object:";

	Validation validation;

	Lock lockA;
	Lock lockB;
	unsigned int counter = 0u;

	constexpr unsigned int numberThreads = 10u;
	constexpr unsigned int iterationsPerThread = 100u;

	std::vector<std::thread> threads;
	threads.reserve(numberThreads);

	for (unsigned int n = 0u; n < numberThreads; ++n)
	{
		if (n % 2u == 0u)
		{
			threads.emplace_back(threadFunction, std::ref(lockA), std::ref(lockB), std::ref(counter), iterationsPerThread);
		}
		else
		{
			threads.emplace_back(threadFunction, std::ref(lockB), std::ref(lockA), std::ref(counter), iterationsPerThread);
		}
	}

	for (std::thread& thread : threads)
	{
		thread.join();
	}

	const unsigned int expectedCount = numberThreads * iterationsPerThread;

	OCEAN_EXPECT_EQUAL(validation, counter, expectedCount);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
