/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestSingleton.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

TestSingleton::SingletonUser::SingletonUser() :
	userValue((unsigned int)(-1))
{
	// nothing to do here
}

TestSingleton::SingletonUser::~SingletonUser()
{
	// nothing to do here
}

unsigned int TestSingleton::SingletonUser::value() const
{
	return userValue;
}

bool TestSingleton::test()
{
	Log::info() << "---   Tests the singleton:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testInstance() && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Singleton test succeeded.";
	}
	else
	{
		Log::info() << "Singleton test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestSingleton, Instance)
{
	EXPECT_TRUE(TestSingleton::testInstance());
}

#endif // OCEAN_USE_GTEST

bool TestSingleton::testInstance()
{
	Log::info() << "Test instance:";

	bool allSucceeded = true;

	if (SingletonUser::get().value() != (unsigned int)(-1))
	{
		allSucceeded = false;
	}

	Log::info() << "Validation: succeeded;";
	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Singleton test succeeded.";
	}
	else
	{
		Log::info() << "Singleton test FAILED!";
	}

	return allSucceeded;
}

}

}

}
