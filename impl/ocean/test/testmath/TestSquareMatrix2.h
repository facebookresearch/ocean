/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_SQUARE_MATRIX_2_H
#define META_OCEAN_TEST_TESTMATH_TEST_SQUARE_MATRIX_2_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a test for 2x2 square matrices.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestSquareMatrix2
{
	public:

		/**
		 * Tests all square matrix 2x2 functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the messenger function.
		 * @return True, if succeeded
		 */
		static bool testWriteToMessenger();

		/**
		 * Tests the element-based constructor.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testElementConstructor(const double testDuration);

		/**
		 * Tests the invert functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testInvert(const double testDuration);

		/**
		 * Tests the matrix conversion functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMatrixConversion(const double testDuration);

		/**
		 * Tests the eigen-system-based constructor.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testEigenConstructor(const double testDuration);

		/**
		 * Tests the eigen system determination function.
		 * @return True, if succeeded
		 */
		static bool testEigenSystem();

		/**
		 * Test for solver of linear systems of equations
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful
		 */
		static bool testSolve(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_SQUARE_MATRIX_2_H
