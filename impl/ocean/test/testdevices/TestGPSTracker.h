/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTDEVICES_TEST_GPS_TRACKER_H
#define META_OCEAN_TEST_TESTDEVICES_TEST_GPS_TRACKER_H

#include "ocean/test/testdevices/TestDevices.h"

namespace Ocean
{

namespace Test
{

namespace TestDevices
{

/**
 * This class implements tests for the GPSTracker class.
 * @ingroup testdevices
 */
class OCEAN_TEST_DEVICES_EXPORT TestGPSTracker
{
	public:

		/**
		 * Invokes all tests.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the parsing function of GPS locations.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testParseGPSLocation(const double testDuration);

		/**
		 * Tests the decode polyline function.
		 * @return True, if succeeded
		 */
		static bool testDecodePolyline();

		/**
		 * Tests the distance approximation between two GPS locations.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testApproximatedDistanceBetweenLocations(const double testDuration);
};

} // namespace TestDevices

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTDEVICES_TEST_GPS_TRACKER_H
