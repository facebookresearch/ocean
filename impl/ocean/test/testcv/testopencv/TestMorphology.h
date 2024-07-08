/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_MORPHOLOGY_H
#define META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_MORPHOLOGY_H

#include "ocean/test/testcv/testopencv/TestOpenCV.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestOpenCV
{

/**
 * This class tests morphology functions of OpenCV.
 * @ingroup testcvopencv
 */
class OCEAN_TEST_CV_OPENCV_EXPORT TestMorphology
{
	public:

		/**
		 * Invokes all morphology tests.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the erosion filter function with a 3x3 filter kernel.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFrameFilterErosion3x3(const double testDuration);

		/**
		 * Tests the erosion filter function with a 3x3 filter kernel.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFrameFilterErosion5x5(const double testDuration);

		/**
		 * Tests the dilation filter function with a 3x3 filter kernel.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFrameFilterDilation3x3(const double testDuration);

		/**
		 * Tests the dilation filter function with a 5x5 filter kernel.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFrameFilterDilation5x5(const double testDuration);

		/**
		 * Tests the open filter function with a 3x3 filter kernel.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFrameFilterOpen3x3(const double testDuration);

		/**
		 * Tests the open filter function with a 5x5 filter kernel.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFrameFilterOpen5x5(const double testDuration);

		/**
		 * Tests the close filter function with a 3x3 filter kernel.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFrameFilterClose3x3(const double testDuration);

		/**
		 * Tests the close filter function with a 5x5 filter kernel.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFrameFilterClose5x5(const double testDuration);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_MORPHOLOGY_H
