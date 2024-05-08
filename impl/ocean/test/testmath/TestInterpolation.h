/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_INTERPOLATION_H
#define META_OCEAN_TEST_TESTMATH_TEST_INTERPOLATION_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements an interpolation test.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestInterpolation
{
	public:

		/**
		 * Tests all interpolation tests.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the linear interpolation function.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testLinear(const double testDuration);

		/**
		 * Tests the bilinear interpolation function.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testBilinear(const double testDuration);

		/**
		 * Tests the bilinear subset interpolation function.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testBilinearSubset(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_INTERPOLATION_H
