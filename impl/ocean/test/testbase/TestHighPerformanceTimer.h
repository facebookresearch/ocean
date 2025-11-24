/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_HIGH_PERFORMANCE_TIMER_H
#define META_OCEAN_TEST_TESTBASE_TEST_HIGH_PERFORMANCE_TIMER_H

#include "ocean/test/testbase/TestBase.h"
#include "ocean/test/TestSelector.h"

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
		 * @param selector The test selector to filter tests
		 * @return True, if succeeded
		 */
		static bool test(const TestSelector& selector = TestSelector());
	
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
		 * @param selector The test selector to filter tests
		 * @return True, if succeeded
		 */
		static bool test(const TestSelector& selector = TestSelector());

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
