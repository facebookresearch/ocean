/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTDEVICES_TEST_ORIENTATION_TRACKER_3DOF_H
#define META_OCEAN_TEST_TESTDEVICES_TEST_ORIENTATION_TRACKER_3DOF_H

#include "ocean/test/testdevices/TestDevices.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/Quaternion.h"

#include "ocean/test/TestSelector.h"

namespace Ocean
{

namespace Test
{

namespace TestDevices
{

/**
 * This class implements tests for the OrientationTracker3DOF class.
 * @ingroup testdevices
 */
class OCEAN_TEST_DEVICES_EXPORT TestOrientationTracker3DOF
{
	public:

		/**
		 * Invokes all tests.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param selector The selector defining which tests will be executed
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, const TestSelector& selector);

		/**
		 * Tests the sample() function with random measurements and all interpolation strategies.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSampleInterpolation(const double testDuration);

	protected:

		/**
		 * Computes the expected interpolated orientation for a given query timestamp.
		 * @param queryTime The query timestamp
		 * @param timestamps The sample timestamps (must be sorted)
		 * @param orientations The orientation quaternions
		 * @return The expected interpolated orientation
		 */
		static Quaternion expectedInterpolatedOrientation(const double queryTime, const Timestamps& timestamps, const Quaternions& orientations);
};

} // namespace TestDevices

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTDEVICES_TEST_ORIENTATION_TRACKER_3DOF_H
