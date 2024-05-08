/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_PLANE_3_H
#define META_OCEAN_TEST_TESTMATH_TEST_PLANE_3_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a 3D plane test.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestPlane3
{
	public:

		/**
		 * Test all plane functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the constructor based on three points.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testConstructorThreePoints(const double testDuration);

		/**
		 * Tests the intersection between lines and planes.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testIntersectionLine(const double testDuration);

		/**
		 * Tests the intersection between two planes.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testIntersectionPlane(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_PLANE_3_H
