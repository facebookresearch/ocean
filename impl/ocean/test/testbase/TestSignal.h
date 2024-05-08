/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_SIGNAL_H
#define META_OCEAN_TEST_TESTBASE_TEST_SIGNAL_H

#include "ocean/test/testbase/TestBase.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Signal.h"
#include "ocean/base/Thread.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements signal tests.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestSignal
{
	private:

		/**
		 * This class implements a simple helper thread allowing to invoke/pulse a signal after a specified time.
		 */
		class SignalThread : public Thread
		{
			public:

				/**
				 * Creates a new thread object.
				 * @param timer The timer providing the same time value for all threads
				 * @param timeout The timeout the thread will wait until the corresponding signal will be pulsed, in seconds with range (0, infinity)
				 * @param signal The signal to be used
				 */
				SignalThread(const HighPerformanceTimer& timer, const double timeout, Signal& signal);

			private:

				/**
				 * Thread function.
				 * @see Thread::threadRun().
				 */
				void threadRun() override;

			private:

				/// The timer providing the same time value for all threads.
				const HighPerformanceTimer& timer_;

				/// Timeout for the thread, in seconds with range (0, infinity)
				const double timeout_;

				/// Signal to be used.
				Signal& signal_;
		};

		/**
		 * This class implements a simple helper thread simulating an async function execution.
		 */
		class AsyncFunctionThread : public Thread
		{
			public:

				/**
				 * Creates a new thread object and starts the thread.
				 */
				AsyncFunctionThread();

				/**
				 * Destructs the thread object and stops any ongoing async function as fast as possible.
				 */
				~AsyncFunctionThread() override;

				/**
				 * Starts the async function.
				 */
				void startAsyncFunction();

			private:

				/**
				 * Thread function.
				 * @see Thread::threadRun().
				 */
				void threadRun() override;

				/**
				 * The async function.
				 */
				void asyncFunction();

			private:

				/// The signal for starting the async function.
				Signal signalAsyncStart_;

				/// The signal for the finished async function.
				Signal signalAsyncStopped_;
		};

	public:

		/**
		 * Tests the entire signal functionalities.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests basic signal functions.
		 * @return True, if succeeded
		 */
		static bool testSignalBasics();

		/**
		 * Tests one signal without timeout.
		 * @return True, if succeeded
		 */
		static bool testSingleSignalStandard();

		/**
		 * Tests one signal with timeout.
		 * @return True, if succeeded
		 */
		static bool testSingleSignalTimeout();

		/**
		 * Tests one signal in a loop.
		 * @return True, if succeeded
		 */
		static bool testSingleSignalLoop();

		/**
		 * Tests a pulse, wait, release cycle via an async function execution.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAsyncFunction(const double testDuration);

		/**
		 * Tests the set of multiple signals.
		 * @return True, if succeeded
		 */
		static bool testMultipleSignals();

		/**
		 * Tests the subset of multiple signals without timeout.
		 * @return True, if succeeded
		 */
		static bool testSubsetSignalsStandard();

		/**
		 * Tests the subset of multiple signals with timeout.
		 * @return True, if succeeded
		 */
		static bool testSubsetSignalsTimeout();
};

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_SIGNAL_H
