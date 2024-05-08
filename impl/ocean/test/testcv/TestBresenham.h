/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_BRESENHAM_H
#define META_OCEAN_TEST_TESTCV_TEST_BRESENHAM_H

#include "ocean/test/testcv/TestCV.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements tests of the Bresenham implementation.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestBresenham
{
	public:

		/**
		 * Tests the Bresenham functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the pixel-precise border intersection function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testIntegerBorderIntersection(const double testDuration);

		/**
		 * Tests the sub-pixel-precise border intersection function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFloatBorderIntersection(const double testDuration);

		/**
		 * Tests the function counting the number of pixels a Bresenham line needs.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNumberLinePixels(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_BRESENHAM_H
