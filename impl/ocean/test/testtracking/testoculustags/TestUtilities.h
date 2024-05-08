/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTTRACKING_TESTOCULUSTAG_TEST_UTILITIES_H
#define META_OCEAN_TEST_TESTTRACKING_TESTOCULUSTAG_TEST_UTILITIES_H

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
 * This class implements tests for the Utilities class.
 * @ingroup testtrackingoculustags
 */
class OCEAN_TEST_TRACKING_OCULUSTAGS_EXPORT TestUtilities
{
	public:

		/**
		 * Starts all tests for the Utilities class.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the serialization/deserialization of Oculus Tags
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSerializeDeserializeOculusTags(const double testDuration);

		/**
		 * Tests the serialization/deserialization of tag sizes
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSerializeDeserializeTagSizeMap(const double testDuration);
};

}

} // namespace OculusTag

} // namespace Tracking

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTTRACKING_TESTOCULUSTAG_TEST_UTILITIES_H
