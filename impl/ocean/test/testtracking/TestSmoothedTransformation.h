/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTTRACKING_TEST_SMOOTHED_TRANSFORMATION_H
#define META_OCEAN_TEST_TESTTRACKING_TEST_SMOOTHED_TRANSFORMATION_H

#include "ocean/test/testtracking/TestTracking.h"

namespace Ocean
{

namespace Test
{

namespace TestTracking
{

/**
 * This class implements tests for the SmoothedTransformation class.
 * @ingroup testtracking
 */
class OCEAN_TEST_TRACKING_EXPORT TestSmoothedTransformation
{
	public:

		/**
		 * Starts all tests for the class.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the transformation functionality.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testTransformation(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTTRACKING_TEST_SMOOTHED_TRANSFORMATION_H
