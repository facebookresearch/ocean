/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_PIXEL_POSITION_H
#define META_OCEAN_TEST_TESTCV_TEST_PIXEL_POSITION_H

#include "ocean/test/testcv/TestCV.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a pixel position test.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestPixelPosition
{
	public:

		/**
		 * Tests the entire pixel position functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the messenger function.
		 * @return True, if succeeded
		 */
		static bool testWriteToMessenger();

		/**
		 * Tests the is neighbor 8 function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testIsNeighbor8(const double testDuration);

		/**
		 * Tests the is in area 9 function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testInArea9(const double testDuration);

		/**
		 * Tests the neighbor function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNeighbor(const double testDuration);

		/**
		 * Tests the multiplication operator.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMultiplication(const double testDuration);

		/**
		 * Tests the division operator.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDivision(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_PIXEL_POSITION_H
