/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTTRACKING_TESTOCULUSTAG_TEST_OCULUSTAG_H
#define META_OCEAN_TEST_TESTTRACKING_TESTOCULUSTAG_TEST_OCULUSTAG_H

#include "ocean/test/testtracking/testoculustags/TestOculusTags.h"

#include "ocean/tracking/oculustags/OculusTagTracker.h"


namespace Ocean
{

namespace Test
{

namespace TestTracking
{

namespace TestOculusTags
{

/**
 * This class implements tests for the Oculus Tag tracker class.
 * @ingroup testtrackingoculustags
 */
class OCEAN_TEST_TRACKING_OCULUSTAGS_EXPORT TestOculusTagTracker : protected Tracking::OculusTags::OculusTagTracker
{
	public:

		/**
		 * Starts all tests for the Oculus Tag tracker class.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Applies a stress test (with noisy data that never contains actual tags).
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testStressTestNegative(const double testDuration, Worker& worker);
};

}

} // namespace OculusTag

} // namespace Tracking

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTTRACKING_TESTOCULUSTAG_TEST_OCULUSTAG_H
