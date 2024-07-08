/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_STATIC_MATRIX_H
#define META_OCEAN_TEST_TESTMATH_TEST_STATIC_MATRIX_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements the test of the StaticMatrix class.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestStaticMatrix
{
	public:

		/**
		 * Tests all matrix functions.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the constructor creating a identity or null matrix.
		 * @return True, if succeeded
		 */
		static bool testConstructorIdentity();

		/**
		 * Tests the constructor creating a matrix from a given buffer.
		 * @return True, if succeeded
		 */
		static bool testConstructorData();

		/**
		 * Tests the transpose function.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testTranspose(const double testDuration);
	
		/**
		 * Tests the add operator.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMatrixAdd(const double testDuration);

		/**
		 * Tests the matrix add function for transposed matrices.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMatrixAddTransposed(const double testDuration);

		/**
		 * Tests the scalar multiplication operator.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testScalarMultiplication(const double testDuration);

		/**
		 * Tests the vector multiplication.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of a scalar value
		 */
		template <typename T>
		static bool testVectorMultiplication(const double testDuration);

		/**
		 * Tests the matrix multiplication operator.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMatrixMultiplication(const double testDuration);

		/**
		 * Tests the matrix multiplication between the transposed and non-transposed matrix.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMatrixMultiplicationTransposedLeft(const double testDuration);

		/**
		 * Tests the matrix multiplication between the non-transposed and transposed matrix.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMatrixMultiplicationTransposedRight(const double testDuration);

		/**
		 * Tests the linear solver by application of the cholesky decomposition.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSolveCholesky(const double testDuration);
	
	protected:
	
		/**
		 * Tests the constructor creating a matrix from a given buffer.
		 * @return True, if succeeded
		 * @tparam tRows The number of matrix rows, with range [1, infinity)
		 * @tparam tColumns The number of matrix columns, with range [1, infinity)
		 */
		template <size_t tRows, size_t tColumns>
		static bool testConstructorData();
	
		/**
		 * Tests the transpose function.
		 * @return True, if succeeded
		 * @tparam tRows The number of matrix rows, with range [1, infinity)
		 * @tparam tColumns The number of matrix columns, with range [1, infinity)
		 */
		template <size_t tRows, size_t tColumns>
		static bool testTranspose();
	
		/**
		 * Tests the matrix add operator.
		 * @return True, if succeeded
		 * @tparam tRows The number of matrix rows, with range [1, infinity)
		 * @tparam tColumns The number of matrix columns, with range [1, infinity)
		 */
		template <size_t tRows, size_t tColumns>
		static bool testMatrixAdd();
	
		/**
		 * Tests the add function for transposed matrices.
		 * @return True, if succeeded
		 * @tparam tRows The number of matrix rows, with range [1, infinity)
		 * @tparam tColumns The number of matrix columns, with range [1, infinity)
		 */
		template <size_t tRows, size_t tColumns>
		static bool testMatrixAddTransposed();
	
		/**
		 * Tests the scalar multiplication operator.
		 * @return True, if succeeded
		 * @tparam tRows The number of matrix rows, with range [1, infinity)
		 * @tparam tColumns The number of matrix columns, with range [1, infinity)
		 */
		template <size_t tRows, size_t tColumns>
		static bool testScalarMultiplication();
	
		/**
		 * Tests the vector multiplication operator.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of a scalar value
		 * @tparam tSize The number of vector elements (the matrix will be a matching square matrix), with range [1, infinity)
		 */
		template <typename T, size_t tSize>
		static bool testVectorMultiplication(const double testDuration);
	
		/**
		 * Tests the matrix multiplication operator.
		 * @return True, if succeeded
		 * @tparam tRows The number of matrix rows, with range [1, infinity)
		 * @tparam tColumns The number of matrix columns, with range [1, infinity)
		 * @tparam tColumns2 The number of columns of the second matrix, with range [1, infinity)
		 */
		template <size_t tRows, size_t tColumns, size_t tColumns2>
		static bool testMatrixMultiplication();
	
		/**
		 * Tests the matrix multiplication between the transposed and non-transposed matrix.
		 * @return True, if succeeded
		 * @tparam tRows The number of matrix rows, with range [1, infinity)
		 * @tparam tColumns The number of matrix columns, with range [1, infinity)
		 */
		template <size_t tRows, size_t tColumns>
		static bool testMatrixMultiplicationTransposedLeft();
	
		/**
		 * Tests the matrix multiplication between the non-transposed and transposed matrix.
		 * @return True, if succeeded
		 * @tparam tRows The number of matrix rows, with range [1, infinity)
		 * @tparam tColumns The number of matrix columns, with range [1, infinity)
		 */
		template <size_t tRows, size_t tColumns>
		static bool testMatrixMultiplicationTransposedRight();
	
		/**
		 * Returns whether the provided matrix is a identity matrix.
		 * @param matrix The matrix to be checked
		 * @param rows The explicit number of rows to be checked, with range [0, matrix.rows()]
		 * @param columns The explicit number of columns to be checked, with range [0, matrix.columns()]
		 * @return True, if so
		 * @tparam T The data type of the matrix
		 */
		template <typename T>
		static bool isIdentityMatrix(const T& matrix, const size_t rows, const size_t columns);
	
		/**
		 * Returns whether all elements of the provided matrix are zero.
		 * @param matrix The matrix to be checked
		 * @param rows The explicit number of rows to be checked, with range [0, matrix.rows()]
		 * @param columns The explicit number of columns to be checked, with range [0, matrix.columns()]
		 * @return True, if so
		 * @tparam T The data type of the matrix
		 */
		template <typename T>
		static bool isNullMatrix(const T& matrix, const size_t rows, const size_t columns);

		/**
		 * Tests the linear solver by application of the cholesky decomposition.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tSize The number of rows and columns of the symmetric matrix, with range [1, infinity)
		 */
		template <size_t tSize>
		static bool testSolveCholeskyMatrix(double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_STATIC_MATRIX_H
