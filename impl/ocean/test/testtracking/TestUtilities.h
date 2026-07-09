/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTTRACKING_TEST_UTILITIES_H
#define META_OCEAN_TEST_TESTTRACKING_TEST_UTILITIES_H

#include "ocean/test/testtracking/TestTracking.h"

#include "ocean/test/TestSelector.h"

namespace Ocean
{

namespace Test
{

namespace TestTracking
{

/**
 * This class implements tests for the Tracking::Utilities class.
 * @ingroup testtracking
 */
class OCEAN_TEST_TRACKING_EXPORT TestUtilities
{
	public:

		/**
		 * Starts all tests for the Utilities class.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @param selector The test selector
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker, const TestSelector& selector);

		/**
		 * Tests blending two frames with identical dimensions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testBlendFramesSameDimensions(const double testDuration);

		/**
		 * Tests blending two frames with different dimensions and offset computation.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testBlendFramesDifferentDimensions(const double testDuration);

		/**
		 * Tests the alignFramesHomography function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAlignFramesHomography(const double testDuration);

		/**
		 * Tests the alignFramesHomographyFullCoverage function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAlignFramesHomographyFullCoverage(const double testDuration);

		/**
		 * Tests the writeCamera and readCamera round-trip including zero-dimension detection.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testCameraSerialization(const double testDuration);

		/**
		 * Tests the readDatabase error paths: invalid magic tag and wrong version.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testReadDatabaseErrorPaths(const double testDuration);

		/**
		 * Tests the paintPoints function for radius==0 special case and bounds check.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPaintPoints(const double testDuration);

		/**
		 * Tests the visualizeDatabase function for early-return conditions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testVisualizeDatabase(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTTRACKING_TEST_UTILITIES_H
