/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_SPARSE_MATRIX_H
#define META_OCEAN_TEST_TESTMATH_TEST_SPARSE_MATRIX_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a matrix test.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestSparseMatrix
{
	public:

		/**
		 * Tests all matrix functions.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the rank function.
		 * @return True, if succeeded
		 */
		static bool testRank();

		/**
		 * Tests the non-negative matrix factorization
		 * @return True, if succeeded
		 */
		static bool testNonNegativeMatrixFactorization(const double testDuration, const unsigned int components = 5u);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_SPARSE_MATRIX_H
