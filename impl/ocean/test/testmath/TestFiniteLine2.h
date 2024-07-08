/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_FINITE_LINE_2_H
#define META_OCEAN_TEST_TESTMATH_FINITE_LINE_2_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a 2D finite line test.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestFiniteLine2
{
	public:

		/**
		 * This functions tests all 2D finite line functions.
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
		 * Distance test.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDistance(const double testDuration);

		/**
		 * Test to check if a point is left of a line, i.e. left its direction vector
		 * @param testDuration Number of seconds for this test, range: (0, infinity)
		 * @return True if the test was successful, otherwise false
		 */
		static bool testIsLeftOfLine(const double testDuration);

		/**
		 * Test to check if a two line segments are collinear
		 * @param testDuration Number of seconds for this test, range: (0, infinity)
		 * @return True if the test was successful, otherwise false
		 */
		static bool testIsCollinear(const double testDuration);

		/**
		 * Test to check calculation of normal vector of a line
		 * @param testDuration Number of seconds for this test, range: (0, infinity)
		 * @return True if the test was successful, otherwise false
		 */
		static bool testNormal(const double testDuration);

		/**
		 * Tests the isEqual() function.
		 * @param testDuration Number of seconds for this test, range: (0, infinity)
		 * @return True if the test was successful, otherwise false
		 * @tparam T The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static bool testIsEqual(const double testDuration);

		/**
		 * Tests the nearestPoint() function.
		 * @param testDuration Number of seconds for this test, range: (0, infinity)
		 * @return True if the test was successful, otherwise false
		 * @tparam T The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static bool testNearestPoint(const double testDuration);

		/**
		 * Tests the intersection() function.
		 * @param testDuration Number of seconds for this test, range: (0, infinity)
		 * @return True if the test was successful, otherwise false
		 * @tparam T The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static bool testIntersection(const double testDuration);

		/**
		 * Tests the nearestPointOnInfiniteLine() function.
		 * @param testDuration Number of seconds for this test, range: (0, infinity)
		 * @return True if the test was successful, otherwise false
		 * @tparam T The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static bool testNearestPointOnInfiniteLine(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_FINITE_LINE_2_H
