/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_TRIANGLE_2_H
#define META_OCEAN_TEST_TESTMATH_TEST_TRIANGLE_2_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a test for the 2D triangle.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestTriangle2
{
	public:

		/**
		 * Tests all triangle functions.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the intersects function.
		 * @param testDuration Number of second for each test
		 * @return True, if succeeded
		 */
		static bool testIntersects(const double testDuration);

		/**
		 * Tests the isCounterClockwise function.
		 * @param testDuration Number of second for each test
		 * @return True, if succeeded
		 */
		static bool testIsCounterClockwise(const double testDuration);

		/**
		 * Tests the padded function.
		 * @param testDuration Number of second for each test
		 * @return True, if succeeded
		 */
		static bool testPadded(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_TRIANGLE_2_H
