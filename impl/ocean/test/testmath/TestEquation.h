/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_EQUATION_H
#define META_OCEAN_TEST_TESTMATH_EQUATION_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a math equation test.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestEquation
{
	public:

		/**
		 * Tests all math equation functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the linear equation.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testLinearEquation(const double testDuration);

		/**
		 * Tests the quadratic equation.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testQuadraticEquation(const double testDuration);

		/**
		 * Tests the qubic equation.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testCubicEquation(const double testDuration);

		/**
		 * Tests the quartic equation.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testQuarticEquation(const double testDuration);
	
	private:

		/**
		 * Validates the linear equation.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return Percent of valid root calculations, with range [0, 1]
		 */
		static double validateLinearEquation(const double testDuration);

		/**
		 * Validates the quadratic equation.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return Percent of valid root calculations, with range [0, 1]
		 */
		static double validateQuadraticEquation(const double testDuration);

		/**
		 * Validates the cubic equation.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return Percent of valid root calculations, with range [0, 1]
		 */
		static double validateCubicEquation(const double testDuration);

		/**
		 * Validates the quartic equation.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return Percent of valid root calculations, with range [0, 1]
		 */
		static double validateQuarticEquation(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_EQUATION_H
