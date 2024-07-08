/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestMoveBehavior.h"

#include "ocean/base/Timestamp.h"
#include "ocean/base/RandomI.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

TestMoveBehavior::OperationCounter::OperationCounter() :
	constructor_(0),
	copyConstructor_(0),
	moveConstructor_(0),
	assignOperator_(0),
	moveOperator_(0)
{
	// nothing to do here
}
	
void TestMoveBehavior::OperationCounter::increment(const bool constructor, const bool copyConstructor, const bool moveConstructor, const bool assignOperator, const bool moveOperator)
{
	ocean_assert((unsigned int)constructor + (unsigned int)copyConstructor + (unsigned int)moveConstructor + (unsigned int)assignOperator + (unsigned int)moveOperator == 1u);

	const ScopedLock scopedLock(lock_);

	if (constructor)	
	{
		++constructor_;
	}

	if (copyConstructor)	
	{
		++copyConstructor_;
	}

	if (moveConstructor)	
	{
		++moveConstructor_;
	}

	if (assignOperator)	
	{
		++assignOperator_;
	}

	if (moveOperator)	
	{
		++moveOperator_;
	}
}

size_t TestMoveBehavior::OperationCounter::constructor() const
{
	const ScopedLock scopedLock(lock_);

	return constructor_;
}

size_t TestMoveBehavior::OperationCounter::copyConstructor() const
{
	const ScopedLock scopedLock(lock_);

	return copyConstructor_;
}

size_t TestMoveBehavior::OperationCounter::moveConstructor() const
{
	const ScopedLock scopedLock(lock_);

	return moveConstructor_;
}

size_t TestMoveBehavior::OperationCounter::assignOperator() const
{
	const ScopedLock scopedLock(lock_);

	return assignOperator_;
}

size_t TestMoveBehavior::OperationCounter::moveOperator() const
{
	const ScopedLock scopedLock(lock_);

	return moveOperator_;
}

void TestMoveBehavior::OperationCounter::reset()
{
	const ScopedLock scopedLock(lock_);

	constructor_ = 0;
	copyConstructor_ = 0;
	moveConstructor_ = 0;
	assignOperator_ = 0;
	moveOperator_ = 0;
}

TestMoveBehavior::Object::Object(const int member) :
	member_(member)
{
	OperationCounter::get().increment(true, false, false, false, false);
}

TestMoveBehavior::Object::Object(const Object& object) :
	member_(object.member_)
{
	OperationCounter::get().increment(false, true, false, false, false);
}

TestMoveBehavior::Object::Object(Object&& object) :
	member_(object.member_)
{
	object.member_ = 0;

	OperationCounter::get().increment(false, false, true, false, false);
}

TestMoveBehavior::Object& TestMoveBehavior::Object::operator=(const Object& object)
{
	if (this != &object)
	{
		member_ = object.member_;
	}

	OperationCounter::get().increment(false, false, false, true, false);

	return *this;
}

TestMoveBehavior::Object& TestMoveBehavior::Object::operator=(Object&& object)
{
	if (this != &object)
	{
		member_ = object.member_;
		object.member_ = 0;
	}

	OperationCounter::get().increment(false, false, false, false, true);

	return *this;
}

TestMoveBehavior::NonExceptObject::NonExceptObject(const int member) :
	member_(member)
{
	OperationCounter::get().increment(true, false, false, false, false);
}

TestMoveBehavior::NonExceptObject::NonExceptObject(const NonExceptObject& object) :
	member_(object.member_)
{
	OperationCounter::get().increment(false, true, false, false, false);
}

TestMoveBehavior::NonExceptObject::NonExceptObject(NonExceptObject&& object) noexcept :
	member_(object.member_)
{
	object.member_ = 0;

	OperationCounter::get().increment(false, false, true, false, false);
}

TestMoveBehavior::NonExceptObject& TestMoveBehavior::NonExceptObject::operator=(const NonExceptObject& object)
{
	if (this != &object)
	{
		member_ = object.member_;
	}

	OperationCounter::get().increment(false, false, false, true, false);

	return *this;
}

TestMoveBehavior::NonExceptObject& TestMoveBehavior::NonExceptObject::operator=(NonExceptObject&& object) noexcept
{
	if (this != &object)
	{
		member_ = object.member_;
		object.member_ = 0;
	}

	OperationCounter::get().increment(false, false, false, false, true);

	return *this;
}

bool TestMoveBehavior::test(const double testDuration)
{
	Log::info() << "---   Move behavior of std implementation test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	Log::info() << " ";

	allSucceeded = testDefaultObject(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNonExceptObject(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Move behavior test succeeded.";
	}
	else
	{
		Log::info() << "Move behavior test FAILED!";
	}

	return allSucceeded;
}
	
#ifdef OCEAN_USE_GTEST

TEST(TestMoveBehavior, NonExceptObject)
{
	EXPECT_TRUE(TestMoveBehavior::testNonExceptObject(GTEST_TEST_DURATION));
}
	
#endif // OCEAN_USE_GTEST

bool TestMoveBehavior::testDefaultObject(const double testDuration)
{
	Log::info() << "Testing default object (without 'nonexcept' keyword):";

	OperationCounter::get().reset();

	bool allSucceeded = true;
	
	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int numberObjects = RandomI::random(10u, 1000u);

		{
			Objects objects;

			for (unsigned int n = 0u; n < numberObjects; ++n)
			{
				objects.emplace_back(RandomI::random(-100, 100));
			}

			if (objects.size() != numberObjects)
			{
				allSucceeded = false;
			}
		}

		{
			Objects objects;

			for (unsigned int n = 0u; n < numberObjects; ++n)
			{
				objects.push_back(Object(RandomI::random(-100, 100)));
			}

			if (objects.size() != numberObjects)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Constructor calls: " << OperationCounter::get().constructor();
	Log::info() << "Copy constructor calls: " << OperationCounter::get().copyConstructor();
	Log::info() << "Move constructor calls: " << OperationCounter::get().moveConstructor();
	Log::info() << "Assign operator calls: " << OperationCounter::get().assignOperator();
	Log::info() << "Move operator calls: " << OperationCounter::get().moveOperator();

	OperationCounter::get().reset();

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded (or expected due to the std standard).";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestMoveBehavior::testNonExceptObject(const double testDuration)
{
	Log::info() << "Testing nonexcept-object:";

	OperationCounter::get().reset();

	bool allSucceeded = true;
	
	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int numberObjects = RandomI::random(10u, 1000u);

		{
			NonExceptObjects nonExceptObjects;

			for (unsigned int n = 0u; n < numberObjects; ++n)
			{
				nonExceptObjects.emplace_back(RandomI::random(-100, 100));
			}

			if (nonExceptObjects.size() != numberObjects)
			{
				allSucceeded = false;
			}
		}

		{
			NonExceptObjects nonExceptObjects;

			for (unsigned int n = 0u; n < numberObjects; ++n)
			{
				nonExceptObjects.push_back(NonExceptObject(RandomI::random(-100, 100)));
			}

			if (nonExceptObjects.size() != numberObjects)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Constructor calls: " << OperationCounter::get().constructor();
	Log::info() << "Copy constructor calls: " << OperationCounter::get().copyConstructor();
	Log::info() << "Move constructor calls: " << OperationCounter::get().moveConstructor();
	Log::info() << "Assign operator calls: " << OperationCounter::get().assignOperator();
	Log::info() << "Move operator calls: " << OperationCounter::get().moveOperator();

	if (OperationCounter::get().copyConstructor() != 0)
	{
		allSucceeded = false;
	}

	if (OperationCounter::get().assignOperator() != 0)
	{
		allSucceeded = false;
	}

	OperationCounter::get().reset();

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
