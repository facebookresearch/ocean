/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_SQUARE_MATRIX_3_H
#define META_OCEAN_TEST_TESTMATH_TEST_SQUARE_MATRIX_3_H

#include "ocean/test/testmath/TestMath.h"

#include "ocean/base/Worker.h"

#include "ocean/math/SquareMatrix4.h"
#include "ocean/math/Vector4.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a test for 3x3 square matrices.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestSquareMatrix3
{
	public:

		/**
		 * Tests all square matrix 3x3 functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object allowing to distribute computations
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

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
		 * Tests the quaternion-based constructor.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of a scalar values
		 */
		template <typename T>
		static bool testQuaternionConstructor(const double testDuration);

		/**
		 * Tests the 2D vector multiplication operator and function.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of a scalar value
		 */
		template <typename T>
		static bool testVectorMultiplication2(const double testDuration);

		/**
		 * Tests the 3D vector multiplication operator.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object allowing to distribute computations
		 * @return True, if succeeded
		 * @tparam T The data type of a scalar value
		 */
		template <typename T>
		static bool testVectorMultiplication3(const double testDuration, Worker& worker);

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
		 * Test for solver of linear systems of equations.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful
		 */
		static bool testSolve(const double testDuration);
	
	protected:
	
		/**
		 * Standard implementation of a multiplication between a 4x4 matrix and a 4x1 vector.
		 * @param matrix The matrix to be used for multiplication
		 * @param vector The vector to be used for multiplication
		 * @return The multiplication result
		 * @tparam T The data type of the elements
		 */
		template <typename T>
		static OCEAN_FORCE_INLINE VectorT3<T> standardVectorMultiplication(const SquareMatrixT3<T>& matrix, const VectorT3<T>& vector);

		/**
		 * Test for solver of linear systems of equations.
		 * @param containsSignular True, to create test data containing singular matrices
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful
		 */
		static bool testSolve(const bool containsSignular, const double testDuration);

		/**
		 * Gauss algorithm for 3x3 systems of linear equations, Ax = b.
		 * @param A The coefficients matrix
		 * @param b The right-hand side vector
		 * @param x The solution vector
		 * @return True if a solution has been found; False, otherwise (e.g. when A is singular)
		 * @tparam T Data type, can be either float or double
		 */
		template <typename T>
		static bool solveGauss(const SquareMatrixT3<T>& A, const VectorT3<T>& b, VectorT3<T>& x);
};

template <typename T>
OCEAN_FORCE_INLINE VectorT3<T> TestSquareMatrix3::standardVectorMultiplication(const SquareMatrixT3<T>& matrix, const VectorT3<T>& vector)
{
	return VectorT3<T>(matrix[0] * vector[0] + matrix[3] * vector[1] + matrix[6] * vector[2],
						matrix[1] * vector[0] + matrix[4] * vector[1] + matrix[7] * vector[2],
						matrix[2] * vector[0] + matrix[5] * vector[1] + matrix[8] * vector[2]);
}

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_SQUARE_MATRIX_3_H
