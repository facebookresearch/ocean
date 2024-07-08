/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_LINEAR_ALGEBRA_H
#define META_OCEAN_TEST_TESTMATH_LINEAR_ALGEBRA_H

#include "ocean/test/testmath/TestMath.h"

#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Matrix.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements linear algebra tests.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestLinearAlgebra
{
	public:

		/**
		 * Tests all linear algebra functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the Eigen system of a 3x3 square matrix.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to use, ether 'float' or 'double'
		 */
		template <typename T>
		static bool testEigenSystemSquareMatrix3(const double testDuration);

		/**
		 * Tests the Eigen system of the matrix.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to use, ether 'float' or 'double'
		 */
		template <typename T>
		static bool testEigenSystemMatrix(const double testDuration);

		/**
		 * Tests the singular value decomposition of a matrix.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to use, ether 'float' or 'double'
		 */
		template <typename T>
		static bool testSingularValueDecomposition(const double testDuration);

		/**
		 * Tests the QR decomposition of a matrix.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to use, ether 'float' or 'double'
		 */
		template <typename T>
		static bool testQrDecomposition(const double testDuration);

		/**
		 * Tests the Cholesky decomposition of a square matrix.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to use, ether 'float' or 'double'
		 */
		template <typename T>
		static bool testCholeskyDecomposition(const double testDuration);

		/**
		 * Tests solving a system of linear equations.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to use, ether 'float' or 'double'
		 */
		template <typename T>
		static bool testSolve(const double testDuration);

	private:

		/**
		 * Tests the singular value decomposition of static and known matrix.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to use, ether 'float' or 'double'
		 */
		template <typename T>
		static bool testSingularValueDecompositionStatic(const double testDuration);

		/**
		 * Tests the singular value decomposition of dynamic random matrices.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to use, ether 'float' or 'double'
		 */
		template <typename T>
		static bool testSingularValueDecompositionDynamic(const double testDuration);

		/**
		 * Tests the QR decomposition of static and known matrix.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to use, ether 'float' or 'double'
		 */
		template <typename T>
		static bool testQrDecompositionStatic(const double testDuration);

		/**
		 * Tests the QR decomposition of dynamic random matrices.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to use, ether 'float' or 'double'
		 */
		template <typename T>
		static bool testQrDecompositionDynamic(const double testDuration);

		/**
		 * Validates the Eigen system of a static and known 3x3 square matrix.
		 * @param matrix The matrix to test
		 * @param expectedEigenValues The expected Eigen values
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to use, ether 'float' or 'double'
		 */
		template <typename T>
		static bool validateEigenSystem(const SquareMatrixT3<T>& matrix, const VectorT3<T>& expectedEigenValues);

		/**
		 * Validate singular value decomposition of a matrix M = U * diag(W) * V^T.
		 * @param matrix The matrix M for which the singular value decomposition has been computed
		 * @param uMatrix Matrix U obtained from the SVD
		 * @param wVector Vector W obtained from the SVD holding the singular values of diagonal matrix D
		 * @param vMatrix Matrix V obtained from the SVD
		 * @return True, if succeeded
		 */
		template <typename T>
		static bool validateSingularValueDecomposition(const MatrixT<T>& matrix, const  MatrixT<T>& uMatrix, const  MatrixT<T>& wVector, const  MatrixT<T>& vMatrix);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_LINEAR_ALGEBRA_H
