/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_INTERPOLATOR_TRILINEAR_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_INTERPOLATOR_TRILINEAR_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/test/TestSelector.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements tests for the FrameInterpolatorTrilinear class.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameInterpolatorTrilinear
{
	public:

		/**
		 * Starts all tests of the FrameInterpolatorTrilinear class.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker Worker object to distribute computational load
		 * @param selector Test selector for filtering sub-tests; default runs all tests
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker, const TestSelector& selector = TestSelector());

		/**
		 * Tests resize() with various pixel formats and shrink/enlarge factors.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @param worker Worker object
		 * @return True, if succeeded
		 */
		static bool testResizeFrame(const double testDuration, Worker& worker);

		/**
		 * Tests that resizing to the same size returns a frame matching the input
		 * (target frame type matches expected).
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @param worker Worker object
		 * @return True, if succeeded
		 */
		static bool testResizeIdentitySize(const double testDuration, Worker& worker);

		/**
		 * Tests that resizing a constant-colored frame produces a constant-colored
		 * output of approximately the same color.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @param worker Worker object
		 * @return True, if succeeded
		 */
		static bool testResizeConstantFrame(const double testDuration, Worker& worker);

		/**
		 * Tests interpolatePosition() returns sane (in-range) coordinates.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testInterpolatePosition(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_INTERPOLATOR_TRILINEAR_H
