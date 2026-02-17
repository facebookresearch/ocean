/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestSingleton.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"
#include "ocean/test/Validation.h"

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

bool TestSingleton::test(const TestSelector& selector)
{
	TestResult testResult("Singleton test");
	Log::info() << " ";

	if (selector.shouldRun("instance"))
	{
		testResult = testInstance();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
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

	Validation validation;

	OCEAN_EXPECT_EQUAL(validation, SingletonUser::get().value(), (unsigned int)(-1));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
