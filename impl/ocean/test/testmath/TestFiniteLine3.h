/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_FINITE_LINE_3_H
#define META_OCEAN_TEST_TESTMATH_FINITE_LINE_3_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a 3D finite line test.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestFiniteLine3
{
	public:

		/**
		 * This functions tests all 3D finite line functions.
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
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_FINITE_LINE_2_H
