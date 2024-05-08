/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_MATRIX_H
#define META_OCEAN_TEST_TESTMATH_TEST_MATRIX_H

#include "ocean/test/testmath/TestMath.h"

#include "ocean/math/Matrix.h"

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
class OCEAN_TEST_MATH_EXPORT TestMatrix
{
	public:

		/**
		 * Tests all matrix functions.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the element-based constructor.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testElementConstructor(const double testDuration);

		/**
		 * Tests the sub-matrix-based constructor.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSubMatrixConstructor(const double testDuration);

		/**
		 * Tests the self square matrix function determining matrix * matrix.transposed().
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSelfSquareMatrix(const double testDuration);

		/**
		 * Tests the self transposed matrix function determining matrix.transposed() * matrix for an existing result matrix.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSelfTransposedSquareMatrixExistingResult(const double testDuration);

		/**
		 * Tests the self transposed matrix function determining matrix.transposed() * matrix.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSelfTransposedSquareMatrix(const double testDuration);

		/**
		 * Tests the self transposed matrix function determining (*this).transposed() * diag(weightDiagonal) * (*this).
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testWeightedSelfTransposedSquareMatrixExistingResult(const double testDuration);

		/**
		 * Tests the invert function.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testInvert(const double testDuration);

		/**
		 * Tests the pseudo inverted matrix function.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPseudoInverted(const double testDuration);

		/**
		 * Tests the rank function.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRank(const double testDuration);

		/**
		 * Tests the non-negative matrix factorization
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param maxComponents The number of factorization compoments to test with, with range (1, infinity)
		 * @return True, if succeeded
		 */
		static bool testNonNegativeMatrixFactorization(const double testDuration, const unsigned int maxComponents = 10u);

		/**
		 * Tests the matrix multiplication of different matrix sizes.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMatrixMultiplication(const double testDuration);

	private:

		/**
		 * Validate matrix multiplication left * right = result
		 * @param left Left matrix of the matrix multiplication
		 * @param right Right matrix of the matrix multiplication
		 * @param result Result matrix of the matrix multiplication
		 * @return True, if succeeded
		 */
		static bool validateMatrixMultiplication(const Matrix & left, const Matrix & right, const Matrix& result);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_MATRIX_H
