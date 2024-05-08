/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_LINE_2_H
#define META_OCEAN_TEST_TESTMATH_TEST_LINE_2_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a 2D line test.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestLine2
{
	public:

		/**
		 * This functions tests all 2D line functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Is on line test.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testIsOnLine(const double testDuration);

		/**
		 * Test to check if a point is left of a line, i.e. left its direction vector
		 * @param testDuration Number of seconds for this test, range: (0, infinity)
		 * @return True if the test was successful, otherwise false
		 */
		static bool testIsLeftOfLine(const double testDuration);

		/**
		 * Tests the decomposition function for normal and distance.
		 * @param testDuration Number of seconds for this test, range: (0, infinity)
		 * @return True if succeeded
		 */
		static bool decomposeNormalDistance(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_LINE_2_H
