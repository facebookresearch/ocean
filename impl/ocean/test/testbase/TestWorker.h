// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TEST_TESTBASE_WORKER_H
#define META_OCEAN_TEST_TESTBASE_WORKER_H

#include "ocean/test/testbase/TestBase.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements worker tests.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestWorker : protected Worker
{
	public:

		/**
		 * Tests the entire worker functionalities.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the worker delay.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to be used
		 * @return True, if succeeded
		 */
		static bool testDelay(const double testDuration, Worker& worker);

		/**
		 * Tests the worker calling a static function.
		 * @param worker The worker object to be used
		 * @return True, if succeeded
		 */
		static bool testStaticWorker(Worker& worker);

		/**
		 * Tests the validation of the worker calling a static function.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to be used
		 * @return True, if succeeded
		 */
		static bool testStaticWorkerSumOfSquares(const double testDuration, Worker& worker);

		/**
		 * Tests the validation of the worker calling a static function.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to be used
		 * @return True, if succeeded
		 */
		static bool testStaticWorkerSumOfSquareRoots(const double testDuration, Worker& worker);

		/**
		 * Tests the validation of the worker calling a static abortable function.
		 * @param worker The worker object to be used
		 * @return True, if succeeded
		 */
		static bool testAbortableFunction(Worker& worker);

		/**
		 * Tests the validation of the worker calling a static separable and abortable function.
		 * @param worker The worker object to be used
		 * @return True, if succeeded
		 */
		static bool testSeparableAndAbortableFunction(Worker& worker);
	
	private:

		/**
		 * Static worker function.
		 * @param time The time parameters
		 * @param first First element
		 * @param number Number elements
		 */
		static void staticWorkerFunctionDelay(unsigned long long* time, const unsigned int first, const unsigned int number);

		/**
		 * Static worker function.
		 * @param first First element
		 * @param number Number elements
		 */
		static void staticWorkerFunction(const unsigned int first, const unsigned int number);

		/**
		 * Static worker function to calculate the sum of square values.
		 * @param values Values to be handled
		 * @param first First element
		 * @param number Number elements
		 */
		static void staticWorkerFunctionSumOfSquares(unsigned int* values, const unsigned int first, const unsigned int number);

		/**
		 * Static worker function to calculate the sum of square root values.
		 * @param values Values to be handled
		 * @param first First element
		 * @param number Number elements
		 */
		static void staticWorkerFunctionSumOfSquareRoots(double* values, const unsigned int first, const unsigned int number);

		/**
		 * Static abortable worker function.
		 * @param result Function result
		 * @param abort Abort state
		 * @return True, if succeeded and not aborted
		 */
		static bool staticWorkerFunctionAbortable(double* result, bool* abort);

		/**
		 * Static separable and abortable worker function.
		 * @param result Function result
		 * @param first First object to be handled
		 * @param size Number of objects to be handled
		 * @param abort Abort state
		 * @return True, if succeeded and not aborted
		 */
		static bool staticWorkerFunctionSeparableAndAbortable(double* result, const unsigned int first, const unsigned int size, bool* abort);
};

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_WORKER_H
