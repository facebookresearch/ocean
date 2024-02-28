// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TEST_TESTBASE_TEST_HIGH_PERFORMANCE_TIMER_H
#define META_OCEAN_TEST_TESTBASE_TEST_HIGH_PERFORMANCE_TIMER_H

#include "ocean/test/testbase/TestBase.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements a test for the HighPerformanceTimer class.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestHighPerformanceTimer
{
	public:

		/**
		 * Invokes all high performance timer tests.
		 * @return True, if succeeded
		 */
		static bool test();
	
		/**
		 * Tests the precision of the high performance timer.
		 * @return True, if succeeded
		 */
		static bool testPrecision();
};

/**
 * This class implements a test for the HighPerformanceStatistic class.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestHighPerformanceStatistic
{
	public:

		/**
		 * Tests all high performance statistic tests.
		 * @return True, if succeeded
		 */
		static bool test();

		/**
		 * Tests the reset function.
		 * @return True, if succeeded
		 */
		static bool testReset();
};

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_HIGH_PERFORMANCE_TIMER_H
