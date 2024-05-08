/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_BOUNDING_SPHERE_H
#define META_OCEAN_TEST_TESTMATH_TEST_BOUNDING_SPHERE_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a test for the bounding sphere.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestBoundingSphere
{
	public:

		/**
		 * Tests all bounding sphere functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the constructor of the bounding sphere.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testConstructor(const double testDuration);

		/**
		 * Tests the intersections function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testIntersections(const double testDuration);

		/**
		 * Tests the intersections function for a transformed sphere.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testIntersectionsTransformed(const double testDuration);

		/**
		 * Tests the positive front intersection function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPositiveFrontIntersection(const double testDuration);

		/**
		 * Tests the positive back intersection function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPositiveBackIntersection(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_BOUNDING_SPHERE_H
