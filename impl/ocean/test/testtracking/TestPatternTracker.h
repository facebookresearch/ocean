/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTTACKING_TEST_PATTERN_TRACKER_H
#define META_OCEAN_TEST_TESTTACKING_TEST_PATTERN_TRACKER_H

#include "ocean/test/testtracking/TestTracking.h"

namespace Ocean
{

namespace Test
{

namespace TestTracking
{

/**
 * This class implements a test for pattern tracker.
 * @ingroup testtracking
 */
class OCEAN_TEST_TRACKING_EXPORT TestPatternTracker
{
	public:

		/**
		 * Starts all pattern tracker tests.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

	protected:

		/**
		 * Tests the power consumption of the tracker.
		 * @return True, if succeeded
		 */
		static bool testPowerConsumption();
};

}

}

}

#endif // META_OCEAN_TEST_TESTTACKING_TEST_PATTERN_TRACKER_H
