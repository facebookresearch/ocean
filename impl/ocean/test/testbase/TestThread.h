/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_THREAD_H
#define META_OCEAN_TEST_TESTBASE_TEST_THREAD_H

#include "ocean/test/testbase/TestBase.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Thread.h"
#include "ocean/base/Timestamp.h"

#include <atomic>

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements tests for the Thread class.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestThread
{
	public:

		/**
		 * Tests all functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests to waitForValue() function without lock object.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testWaitForValueWithoutLock(const double testDuration);

		/**
		 * Tests to waitForValue() function with lock object.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testWaitForValueWithLock(const double testDuration);

	protected:

		/**
		 * Sets a parameter to a specified value after a specified delay.
		 * @param object The object which will be set
		 * @param value The value to use
		 * @param delay The delay in seconds, with range [0, infinity)
		 * @param lock The lock to be used
		 * @param isSet Will be set to True if the value was set
		 * @tparam TObject The data type of the object
		 * @tparam TValue The data type of the value
		 */
		template <typename TObject, typename TValue>
		static void setValueDelayed(TObject& object, const TValue& value, const double delay, Lock& lock, std::atomic<bool>& isSet);
};

template <typename TObject, typename TValue>
void TestThread::setValueDelayed(TObject& object, const TValue& value, const double delay, Lock& lock, std::atomic<bool>& isSet)
{
	ocean_assert(!isSet);

	Thread::sleep((unsigned int)(Timestamp::seconds2milliseconds(delay)));

	const ScopedLock scopedLock(lock);

	object = value;

	isSet = true;
}

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_THREAD_H
