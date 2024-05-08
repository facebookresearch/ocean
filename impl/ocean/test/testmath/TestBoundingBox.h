/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_BOUNDING_BOX_H
#define META_OCEAN_TEST_TESTMATH_TEST_BOUNDING_BOX_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a bounding box test.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestBoundingBox
{
	public:

		/**
		 * Tests all bounding box functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Test the positive front intersection function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPositiveFrontIntersection(const double testDuration);

		/**
		 * Test the positive back intersection function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPositiveBackIntersection(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_BOUNDING_BOX_H
