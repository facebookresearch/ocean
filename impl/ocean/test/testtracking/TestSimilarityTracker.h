/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTTRACKING_TEST_SIMILARITY_TRACKER_H
#define META_OCEAN_TEST_TESTTRACKING_TEST_SIMILARITY_TRACKER_H

#include "ocean/test/testtracking/TestTracking.h"

#include "ocean/base/RandomGenerator.h"

namespace Ocean
{

namespace Test
{

namespace TestTracking
{

/**
 * This class implements tests for the SimilarityTracker class.
 * @ingroup testtracking
 */
class OCEAN_TEST_TRACKING_EXPORT TestSimilarityTracker
{
	public:

		/**
		 * Starts all tests for the similarity tracker class.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the general tracking quality.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testTracking(const double testDuration, Worker& worker);

		/**
		 * Applies a stress test.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testStressTest(const double testDuration, Worker& worker);
};

}

}

}

#endif // META_OCEAN_TEST_TESTTRACKING_TEST_SIMILARITY_TRACKER_H
