/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_MATH_UTILITIES_H
#define META_OCEAN_TEST_TESTMATH_TEST_MATH_UTILITIES_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a test for functions in the Utilities class.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestMathUtilities
{
	public:

		/**
		 * Tests all functions.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the encode function.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testEncodeFloatToUint8(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_MATH_UTILITIES_H
