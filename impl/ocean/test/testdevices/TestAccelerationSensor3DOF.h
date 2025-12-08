/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTDEVICES_TEST_ACCELERATION_SENSOR_3DOF_H
#define META_OCEAN_TEST_TESTDEVICES_TEST_ACCELERATION_SENSOR_3DOF_H

#include "ocean/test/testdevices/TestDevices.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/Vector3.h"

#include "ocean/test/TestSelector.h"

namespace Ocean
{

namespace Test
{

namespace TestDevices
{

/**
 * This class implements tests for the AccelerationSensor3DOF class.
 * @ingroup testdevices
 */
class OCEAN_TEST_DEVICES_EXPORT TestAccelerationSensor3DOF
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
		 * Computes the expected interpolated acceleration vector for a given query timestamp.
		 * Uses linear interpolation between the two bounding samples.
		 * @param queryTime The query timestamp
		 * @param timestamps The sample timestamps (must be sorted)
		 * @param accelerations The acceleration vectors
		 * @return The expected interpolated acceleration vector
		 */
		static Vector3 expectedInterpolatedAcceleration(const double queryTime, const Timestamps& timestamps, const Vectors3& accelerations);
};

} // namespace TestDevices

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTDEVICES_TEST_ACCELERATION_SENSOR_3DOF_H
