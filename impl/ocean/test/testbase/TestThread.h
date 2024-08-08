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
	protected:

		/**
		 * Helper class to set a value.
		 * @tparam TObject The data type of the object
		 * @tparam TValue The data type of the value
		 */
		template <typename TObject, typename TValue>
		class DelayedValueSetter : protected Thread
		{
			public:

				/**
				 * Creates a new object.
				 * @param object The object which will be set
				 * @param value The value to use
				 * @param delay The delay in seconds
				 * @param lock The lock to be used
				 */
				DelayedValueSetter(TObject& object, const TValue& value, const double delay, Lock& lock);

				/**
				 * Destructs this object.
				 */
				~DelayedValueSetter() override;

				/**
				 * Stops the setter.
				 * @return True, if the value was set; False, if the value was not set
				 */
				bool stop();

			protected:

				/**
				 * The thread run function.
				 */
				void threadRun() override;

			protected:

				/// The object to be set.
				TObject& object_;

				/// The value to use.
				const TValue& value_;

				/// The delay in seconds.
				double delay_ = -1.0;

				/// The lock to be used
				Lock& lock_;

				/// True, if the value was set.
				bool wasSet_ = false;
		};

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
};

template <typename TObject, typename TValue>
TestThread::DelayedValueSetter<TObject, TValue>::DelayedValueSetter(TObject& object, const TValue& value, const double delay, Lock& lock) :
	object_(object),
	value_(value),
	delay_(delay),
	lock_(lock)
{
	startThread();
}

template <typename TObject, typename TValue>
TestThread::DelayedValueSetter<TObject, TValue>::~DelayedValueSetter()
{
	stop();
}

template <typename TObject, typename TValue>
bool TestThread::DelayedValueSetter<TObject, TValue>::stop()
{
	stopThreadExplicitly();

	return wasSet_;
}

template <typename TObject, typename TValue>
void TestThread::DelayedValueSetter<TObject, TValue>::threadRun()
{
	const Timestamp startTimestamp(true);

	while (!shouldThreadStop())
	{
		ocean_assert(delay_ >= 0.0);
		if (startTimestamp.hasTimePassed(delay_))
		{
			const ScopedLock scopedLock(lock_);

			object_ = value_;

			wasSet_ = true;
			return;
		}

		Thread::sleep(1u);
	}
}

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_THREAD_H
