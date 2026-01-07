/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTTRACKING_TESTSLAM_TEST_FRAME_PYRAMID_MANAGER_H
#define META_OCEAN_TEST_TESTTRACKING_TESTSLAM_TEST_FRAME_PYRAMID_MANAGER_H

#include "ocean/test/testtracking/testslam/TestSLAM.h"

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
 * This class implements FramePyramidManager tests.
 * @ingroup testtrackingtestslam
 */
class OCEAN_TEST_TRACKING_SLAM_EXPORT TestFramePyramidManager
{
	public:

		/**
		 * Executes all FramePyramidManager tests.
		 * @param testDuration Number of seconds for each test
		 * @param selector The test selector
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, const TestSelector& selector);

		/**
		 * Tests the newPyramid function.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testNewPyramid(const double testDuration);

		/**
		 * Tests the existingPyramid function.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testExistingPyramid(const double testDuration);

		/**
		 * Tests the latestPyramid function.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testLatestPyramid(const double testDuration);

		/**
		 * Tests the updateLatest function.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testUpdateLatest(const double testDuration);

		/**
		 * Tests the ScopedPyramid RAII behavior.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testScopedPyramid(const double testDuration);

		/**
		 * Tests the idealPyramidLayers static function.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testIdealPyramidLayers(const double testDuration);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTTRACKING_TESTSLAM_TEST_FRAME_PYRAMID_MANAGER_H
