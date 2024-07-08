/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestLock.h"

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

bool TestLock::test()
{
	Log::info() << "---   Lock test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testLockUnlock() && allSucceeded;

	Log::info() << " ";

	allSucceeded = testScopedLock() && allSucceeded;

	Log::info() << " ";

	allSucceeded = testStaticScopedLock() && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTemplatedLock() && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Lock test succeeded.";
	}
	else
	{
		Log::info() << "Lock test FAILED!";
	}

	return allSucceeded;
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

	bool allSucceeded = true;

	static_assert(sizeof(Empty) == 1, "Invalid data type!"); // empty classes always have 1 byte

	static_assert(sizeof(TemplatedLock<true>) > 1, "Invalid data type!");
	static_assert(sizeof(TemplatedLock<false>) == 1, "Invalid data type!");

	if constexpr (sizeof(TemplatedLock<true>) <= 1)
	{
		allSucceeded = false;
	}

	if constexpr (sizeof(TemplatedLock<false>) != 1)
	{
		allSucceeded = false;
	}

	static_assert(sizeof(Object<true>) > 1, "Invalid data type!");
	static_assert(sizeof(Object<false>) == 1, "Invalid data type!");

	if constexpr (sizeof(Object<true>) <= 1)
	{
		allSucceeded = false;
	}

	if constexpr (sizeof(Object<false>) != 1)
	{
		allSucceeded = false;
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

}

}

}
