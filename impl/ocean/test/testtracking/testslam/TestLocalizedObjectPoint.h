/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTTRACKING_TESTSLAM_TEST_LOCALIZED_OBJECT_POINT_H
#define META_OCEAN_TEST_TESTTRACKING_TESTSLAM_TEST_LOCALIZED_OBJECT_POINT_H

#include "ocean/test/testtracking/testslam/TestSLAM.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/math/AnyCamera.h"

#include "ocean/test/TestSelector.h"

namespace Ocean
{

namespace Test
{

namespace TestTracking
{

namespace TestSLAM
{

/**
 * This class implements LocalizedObjectPoint tests.
 * @ingroup testtrackingtestslam
 */
class OCEAN_TEST_TRACKING_SLAM_EXPORT TestLocalizedObjectPoint
{
	public:

		/**
		 * Executes all LocalizedObjectPoint tests.
		 * @param testDuration Number of seconds for each test
		 * @param selector The test selector
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, const TestSelector& selector);

		/**
		 * Tests the constructor from PointTrack.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testConstructor(const double testDuration);

		/**
		 * Tests the constructor with known position.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testConstructorWithPosition(const double testDuration);

		/**
		 * Tests the addObservation function.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testAddObservation(const double testDuration);

		/**
		 * Tests the removeObservation function.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testRemoveObservation(const double testDuration);

		/**
		 * Tests the hasObservation function.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testHasObservation(const double testDuration);

		/**
		 * Tests the observation function.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testObservation(const double testDuration);

		/**
		 * Tests the position getter and setter.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testPosition(const double testDuration);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTTRACKING_TESTSLAM_TEST_LOCALIZED_OBJECT_POINT_H
