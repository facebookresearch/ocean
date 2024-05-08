/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_SQUARE_MATRIX_4_H
#define META_OCEAN_TEST_TESTMATH_TEST_SQUARE_MATRIX_4_H

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
 * This class implements a test for 4x4 square matrices.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestSquareMatrix4
{
	public:

		/**
		 * Tests all square matrix 4x4 functions.
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
		 * Tests the vector multiplication operator.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object allowing to distribute computations
		 * @return True, if succeeded
		 * @tparam T The data type of a scalar value
		 */
		template <typename T>
		static bool testVectorMultiplication(const double testDuration, Worker& worker);

		/**
		 * Tests the matrix multiplication operator.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of a scalar value
		 */
		template <typename T>
		static bool testMatrixMultiplication(const double testDuration);

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
		 * Tests the projection matrix function with field of view.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of a scalar value
		 */
		template <typename T>
		static bool testProjectionMatrixFieldOfView(const double testDuration);

		/**
		 * Tests the projection matrix function with camera matrix.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of a scalar value
		 */
		template <typename T>
		static bool testProjectionMatrixCameraMatrix(const double testDuration);

	private:

		/**
		 * Standard implementation of a multiplication between a 4x4 matrix and a 4x1 vector.
		 * @param matrix The matrix to be used for multiplication
		 * @param vector The vector to be used for multiplication
		 * @return The multiplication result
		 * @tparam T The data type of the elements
		 */
		template <typename T>
		static OCEAN_FORCE_INLINE VectorT4<T> standardVectorMultiplication(const SquareMatrixT4<T>& matrix, const VectorT4<T>& vector);

		/**
		 * Standard implementation of a multiplication between a 4x4 matrix and a 4x4 matrix.
		 * @param matrix The matrix to be used for multiplication
		 * @param rightMatrix The right matrix to be used for multiplication
		 * @return The multiplication result
		 * @tparam T The data type of the elements
		 */
		template <typename T>
		static OCEAN_FORCE_INLINE SquareMatrixT4<T> standardMatrixMultiplication(const SquareMatrixT4<T>& matrix, const SquareMatrixT4<T>& rightMatrix);
};

template <typename T>
OCEAN_FORCE_INLINE VectorT4<T> TestSquareMatrix4::standardVectorMultiplication(const SquareMatrixT4<T>& matrix, const VectorT4<T>& vector)
{
	return VectorT4<T>(matrix[0] * vector[0] + matrix[4] * vector[1] + matrix[8] * vector[2] + matrix[12] * vector[3],
						matrix[1] * vector[0] + matrix[5] * vector[1] + matrix[9] * vector[2] + matrix[13] * vector[3],
						matrix[2] * vector[0] + matrix[6] * vector[1] + matrix[10] * vector[2] + matrix[14] * vector[3],
						matrix[3] * vector[0] + matrix[7] * vector[1] + matrix[11] * vector[2] + matrix[15] * vector[3]);
}

template <typename T>
OCEAN_FORCE_INLINE SquareMatrixT4<T> TestSquareMatrix4::standardMatrixMultiplication(const SquareMatrixT4<T>& matrix, const SquareMatrixT4<T>& rightMatrix)
{
	SquareMatrixT4<T> result;

	result[ 0] = matrix[0] * rightMatrix[ 0] + matrix[4] * rightMatrix[ 1] + matrix[ 8] * rightMatrix[ 2] + matrix[12] * rightMatrix[3];
	result[ 1] = matrix[1] * rightMatrix[ 0] + matrix[5] * rightMatrix[ 1] + matrix[ 9] * rightMatrix[ 2] + matrix[13] * rightMatrix[3];
	result[ 2] = matrix[2] * rightMatrix[ 0] + matrix[6] * rightMatrix[ 1] + matrix[10] * rightMatrix[ 2] + matrix[14] * rightMatrix[3];
	result[ 3] = matrix[3] * rightMatrix[ 0] + matrix[7] * rightMatrix[ 1] + matrix[11] * rightMatrix[ 2] + matrix[15] * rightMatrix[3];

	result[ 4] = matrix[0] * rightMatrix[ 4] + matrix[4] * rightMatrix[ 5] + matrix[ 8] * rightMatrix[ 6] + matrix[12] * rightMatrix[7];
	result[ 5] = matrix[1] * rightMatrix[ 4] + matrix[5] * rightMatrix[ 5] + matrix[ 9] * rightMatrix[ 6] + matrix[13] * rightMatrix[7];
	result[ 6] = matrix[2] * rightMatrix[ 4] + matrix[6] * rightMatrix[ 5] + matrix[10] * rightMatrix[ 6] + matrix[14] * rightMatrix[7];
	result[ 7] = matrix[3] * rightMatrix[ 4] + matrix[7] * rightMatrix[ 5] + matrix[11] * rightMatrix[ 6] + matrix[15] * rightMatrix[7];

	result[ 8] = matrix[0] * rightMatrix[ 8] + matrix[4] * rightMatrix[ 9] + matrix[ 8] * rightMatrix[10] + matrix[12] * rightMatrix[11];
	result[ 9] = matrix[1] * rightMatrix[ 8] + matrix[5] * rightMatrix[ 9] + matrix[ 9] * rightMatrix[10] + matrix[13] * rightMatrix[11];
	result[10] = matrix[2] * rightMatrix[ 8] + matrix[6] * rightMatrix[ 9] + matrix[10] * rightMatrix[10] + matrix[14] * rightMatrix[11];
	result[11] = matrix[3] * rightMatrix[ 8] + matrix[7] * rightMatrix[ 9] + matrix[11] * rightMatrix[10] + matrix[15] * rightMatrix[11];

	result[12] = matrix[0] * rightMatrix[12] + matrix[4] * rightMatrix[13] + matrix[ 8] * rightMatrix[14] + matrix[12] * rightMatrix[15];
	result[13] = matrix[1] * rightMatrix[12] + matrix[5] * rightMatrix[13] + matrix[ 9] * rightMatrix[14] + matrix[13] * rightMatrix[15];
	result[14] = matrix[2] * rightMatrix[12] + matrix[6] * rightMatrix[13] + matrix[10] * rightMatrix[14] + matrix[14] * rightMatrix[15];
	result[15] = matrix[3] * rightMatrix[12] + matrix[7] * rightMatrix[13] + matrix[11] * rightMatrix[14] + matrix[15] * rightMatrix[15];

	return result;
}

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_SQUARE_MATRIX_4_H
