/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestLinearAlgebra.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Euler.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Matrix.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Random.h"
#include "ocean/math/Rotation.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestLinearAlgebra::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Linear Algebra Test:   ---";
	Log::info() << " ";

	allSucceeded = testEigenSystemSquareMatrix3<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testEigenSystemSquareMatrix3<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testEigenSystemMatrix<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testEigenSystemMatrix<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSingularValueDecomposition<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testSingularValueDecomposition<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testQrDecomposition<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testQrDecomposition<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCholeskyDecomposition<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testCholeskyDecomposition<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSolve<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testSolve<double>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Linear Algebra Test succeeded.";
	}
	else
	{
		Log::info() << "Linear Algebra Test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestLinearAlgebra, EigenSystemSquareMatrix3_float)
{
	EXPECT_TRUE(TestLinearAlgebra::testEigenSystemSquareMatrix3<float>(GTEST_TEST_DURATION));
}

TEST(TestLinearAlgebra, EigenSystemSquareMatrix3_double)
{
	EXPECT_TRUE(TestLinearAlgebra::testEigenSystemSquareMatrix3<double>(GTEST_TEST_DURATION));
}


TEST(TestLinearAlgebra, EigenSystemMatrix_float)
{
	EXPECT_TRUE(TestLinearAlgebra::testEigenSystemMatrix<float>(GTEST_TEST_DURATION));
}

TEST(TestLinearAlgebra, EigenSystemMatrix_double)
{
	EXPECT_TRUE(TestLinearAlgebra::testEigenSystemMatrix<double>(GTEST_TEST_DURATION));
}


TEST(TestLinearAlgebra, SingularValueDecomposition_float)
{
	EXPECT_TRUE(TestLinearAlgebra::testSingularValueDecomposition<float>(GTEST_TEST_DURATION));
}

TEST(TestLinearAlgebra, SingularValueDecomposition_double)
{
	EXPECT_TRUE(TestLinearAlgebra::testSingularValueDecomposition<double>(GTEST_TEST_DURATION));
}


TEST(TestLinearAlgebra, QrDecomposition_float)
{
	EXPECT_TRUE(TestLinearAlgebra::testQrDecomposition<float>(GTEST_TEST_DURATION));
}

TEST(TestLinearAlgebra, QrDecomposition_double)
{
	EXPECT_TRUE(TestLinearAlgebra::testQrDecomposition<double>(GTEST_TEST_DURATION));
}


TEST(TestLinearAlgebra, CholeskyDecomposition_float)
{
	EXPECT_TRUE(TestLinearAlgebra::testCholeskyDecomposition<float>(GTEST_TEST_DURATION));
}

TEST(TestLinearAlgebra, CholeskyDecomposition_double)
{
	EXPECT_TRUE(TestLinearAlgebra::testCholeskyDecomposition<double>(GTEST_TEST_DURATION));
}


TEST(TestLinearAlgebra, Solve_float)
{
	EXPECT_TRUE(TestLinearAlgebra::testSolve<float>(GTEST_TEST_DURATION));
}

TEST(TestLinearAlgebra, Solve_double)
{
	EXPECT_TRUE(TestLinearAlgebra::testSolve<double>(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

template <typename T>
bool TestLinearAlgebra::testEigenSystemSquareMatrix3(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Eigen system of a SquareMatrix3 matrix, with '" << TypeNamer::name<T>() << "':";

	bool allSucceeded = true;

	{
		// |  7   0  -3 |
		// | -9  -2   3 |
		// | 18   0  -8 |

		// EigenValues: 1, -2, -2
		// EigenVectors are: (1, -1, 2); (1, 0, 3) or (1, 1, 3)

		const SquareMatrixT3<T> matrix(7, -9, 18, 0, -2, 0, -3, 3, -8);
		const VectorT3<T> expectedEigenValues(1, -2, -2);

		if (!validateEigenSystem(matrix, expectedEigenValues))
		{
			allSucceeded = false;
		}
	}

	{
		// | -1 4 -4 |
		// | -4 7 -4 |
		// | -4 4 -1 |

		// EigenValues: 3, 3, -1
		// EigenVectors: (1 0 1); (1 0 1); (1, 1, 1)

		const SquareMatrixT3<T> matrix(-1, -4, -4, 4, 7, 4, -4, -4, -1);
		const VectorT3<T> expectedEigenValues(3, 3, -1);

		if (!validateEigenSystem(matrix, expectedEigenValues))
		{
			allSucceeded = false;
		}
	}

	{
		// | 0 1 0 |
		// | 0 2 0 |
		// | 0 0 3 |

		// EigenValues: 3, 2, 0
		// EigenVectors: (0, 0, 1); (1, 2, 0); (1, 0, 0)

		const SquareMatrixT3<T> matrix(0, 0, 0, 1, 2, 0, 0, 0, 3);
		const VectorT3<T> expectedEigenValues(3, 2, 0);

		if (!validateEigenSystem(matrix, expectedEigenValues))
		{
			allSucceeded = false;
		}
	}

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		const VectorT3<T> xAxis(1, 0, 0);
		const VectorT3<T> yAxis(0, 1, 0);
		const VectorT3<T> zAxis(0, 0, 1);

		T xLength = RandomT<T>::scalar(randomGenerator, T(0.01), T(10));
		T yLength = RandomT<T>::scalar(randomGenerator, T(0.01), T(10));
		T zLength = RandomT<T>::scalar(randomGenerator, T(0.01), T(10));

		const SquareMatrixT3<T> matrix(xAxis * xLength, yAxis * yLength, zAxis * zLength);

		T values[3] = {RandomT<T>::scalar(-100, 100), RandomT<T>::scalar(-100, 100), RandomT<T>::scalar(-100, 100)};
		VectorT3<T> vectors[3];

		performance.start();
			bool localResult = matrix.eigenSystem(values, vectors);
		performance.stop();

		Utilities::sortHighestToFront3(xLength, yLength, zLength);

		if (NumericT<T>::isNotWeakEqual(xLength, values[0]))
		{
			localResult = false;
		}

		if (NumericT<T>::isNotWeakEqual(yLength, values[1]))
		{
			localResult = false;
		}

		if (NumericT<T>::isNotWeakEqual(zLength, values[2]))
		{
			localResult = false;
		}

		if (localResult)
		{
			++validIterations;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Performance: " << performance.averageMseconds() * 1000.0 << "mys";

	constexpr double threshold = std::is_same<T, float>::value ? 0.95 : 0.99;

	if (percent < threshold)
	{
		allSucceeded = false;
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestLinearAlgebra::testEigenSystemMatrix(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Eigen system of a 4x4 matrix, with '" << TypeNamer::name<T>() << "':";

	bool allSucceeded = true;

	constexpr T eigenMatrixValues[16] =
	{
		T(0.4), T(-3), T(6), T(-3.5),
		T(-3), T(30), T(-67.5), T(42),
		T(6), T(-67.5), T(162), T(-105),
		T(-3.5), T(42), T(-105), T(70)
	};

	const MatrixT<T> matrix(4, 4, eigenMatrixValues);

	MatrixT<T> values;
	MatrixT<T> vectors;

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		performance.start();
			const bool localResult = matrix.eigenSystem(values, vectors);
		performance.stop();

		if (!localResult)
		{
			allSucceeded = false;
		}

		const MatrixT<T> diagonal(4, 4, values);
		const MatrixT<T> result = vectors * diagonal * vectors.transposed();

		if (!matrix.isEqual(result, NumericT<T>::weakEps()))
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << performance.averageMseconds() << "ms";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestLinearAlgebra::testSingularValueDecomposition(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Singular value decomposition test, with '" << TypeNamer::name<T>() << "':";
	Log::info() <<  " ";

	bool allSucceeded = true;

	allSucceeded = testSingularValueDecompositionStatic<T>(testDuration) && allSucceeded;
	Log::info() <<  " ";
	allSucceeded = testSingularValueDecompositionDynamic<T>(testDuration) && allSucceeded;

	return allSucceeded;
}

template <typename T>
bool TestLinearAlgebra::testQrDecomposition(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "QR decomposition test, with '" << TypeNamer::name<T>() << "':";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testQrDecompositionStatic<T>(testDuration) && allSucceeded;
	Log::info() <<  " ";
	allSucceeded = testQrDecompositionDynamic<T>(testDuration) && allSucceeded;

	return allSucceeded;
}

template <typename T>
bool TestLinearAlgebra::testCholeskyDecomposition(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "Cholesky decomposition of random matrices with different sizes, with '" << TypeNamer::name<T>() << "':";

	for (const size_t dimension : {5u, 10u, 20u, 50u, 100u})
	{
		Log::info() << " ";

		uint64_t iterations = 0ull;
		uint64_t validIterations = 0ull;

		Log::info() << "... with dimension " << dimension << "x" << dimension << ":";

		HighPerformanceStatistic performance;

		Timestamp startTimestamp(true);

		do
		{
			MatrixT<T> matrix(dimension, dimension);

			for (size_t n = 0; n < matrix.elements(); ++n)
			{
				matrix(n) = RandomT<T>::scalar(-1, 1);
			}

			const MatrixT<T> squaredMatrix = matrix.transposedMultiply(matrix);

			MatrixT<T> matrixL;

			performance.start();
				const bool result = squaredMatrix.choleskyDecomposition(matrixL);
			performance.stop();

			const MatrixT<T> matrixValidate = matrixL * matrixL.transposed();

			if (result && squaredMatrix.isEqual(matrixValidate, NumericT<T>::weakEps()))
			{
				++validIterations;
			}

			++iterations;
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);
		const double percent = double(validIterations) / double(iterations);

		Log::info() << "Performance: " << performance.averageMseconds() << "ms";
		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "%";

		constexpr double threshold = std::is_same<float, T>::value ? 0.75 : 0.95;

		if (percent < threshold)
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestLinearAlgebra::testSolve(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Solve test, with '" << TypeNamer::name<T>() << "':";

	bool allSucceeded = true;

	Indices32 dimensions = {5u, 10u, 20u};

	if (std::is_same<T, double>::value)
	{
		dimensions.push_back(50u);
		dimensions.push_back(100u);
	}

	for (const unsigned int dimension : dimensions)
	{
		Log::info() << " ";

		uint64_t iterations = 0ull;
		uint64_t validIterations = 0ull;

		Log::info() << "... with dimension " << dimension << "x" << dimension << ":";

		HighPerformanceStatistic performance;

		Timestamp startTimestamp(true);

		do
		{
			MatrixT<T> a0(dimension, dimension);
			MatrixT<T> x0(dimension, 1);

			for (unsigned int n = 0; n < a0.elements(); ++n)
			{
				a0(n) = RandomT<T>::scalar(0, 1);
			}

			for (unsigned int n = 0; n < x0.elements(); ++n)
			{
				x0(n) = RandomT<T>::scalar(0, 1);
			}

			const MatrixT<T> b0 = a0 * x0;

			MatrixT<T> x1;

			performance.start();
				const bool result = a0.solve(b0, x1);
			performance.stop();

			if (result && x0.isEqual(x1, NumericT<T>::weakEps()) && b0.isEqual(a0 * x1, NumericT<T>::weakEps()))
			{
				++validIterations;
			}

			++iterations;
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);
		const double percent = double(validIterations) / double(iterations);

		Log::info() << "Performance: " << performance.averageMseconds() << "ms";
		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

		if (percent < 0.95)
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestLinearAlgebra::testSingularValueDecompositionStatic(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with static matrix:";

	bool allSucceeded = true;

	const static T m[20] =
	{
		1, 0, 0, 0, 2,
		0, 0, 3, 0, 0,
		0, 0, 0, 0, 0,
		0, 2, 0, 0, 0
	};

	const static T u[16] =
	{
		0, 1, 0, 0,
		1, 0, 0, 0,
		0, 0, 0, -1,
		0, 0, 1, 0
	};

	const static T w[5] =
	{
		3, NumericT<T>::sqrt(5), 2, 0, 0
	};

	const static T v[25] =
	{
		 0, 0, 1, 0, 0,
		 NumericT<T>::sqrt(T(0.2)), 0, 0, 0, NumericT<T>::sqrt(T(0.8)),
		 0, 1, 0, 0, 0,
		 0, 0, 0, 1, 0,
		 -NumericT<T>::sqrt(T(0.8)), 0, 0, 0, NumericT<T>::sqrt(T(0.2))
	};

	const T epsilon = std::is_same<float, T>::value ? NumericT<T>::eps() * 10 : NumericT<T>::eps();

	const MatrixT<T> matrix(4, 5, m);
	const MatrixT<T> matrixT(matrix.transposed());

	const MatrixT<T> uMatrix(4, 4, u);
	const MatrixT<T> wVector(5, 1, w);
	const MatrixT<T> vMatrix(5, 5, v);

	if (!validateSingularValueDecomposition(matrix, uMatrix, wVector, vMatrix.transposed()))
	{
		allSucceeded = false;
	}

	{
		MatrixT<T> uLocal, wLocal, vLocal;

		if (!matrix.singularValueDecomposition(uLocal, wLocal, vLocal))
		{
			allSucceeded = false;
		}

		if (!wLocal.isEqual(wVector, epsilon))
		{
			allSucceeded = false;
		}

		if (!validateSingularValueDecomposition(matrix, uLocal, wLocal, vLocal))
		{
			allSucceeded = false;
		}
	}

	{
		MatrixT<T> uLocal, wLocal, vLocal;

		if (!matrixT.singularValueDecomposition(uLocal, wLocal, vLocal))
		{
			allSucceeded = false;
		}

		if (!wLocal.isEqual(wVector, epsilon))
		{
			allSucceeded = false;
		}

		if (!validateSingularValueDecomposition(matrixT, uLocal, wLocal, vLocal))
		{
			allSucceeded = false;
		}
	}

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		MatrixT<T> uLocal, wLocal, vLocal;

		performance.start();
			const bool result = matrix.singularValueDecomposition(uLocal, wLocal, vLocal);
		performance.stop();

		if (!result)
		{
			allSucceeded = false;
		}

		if (!validateSingularValueDecomposition(matrix, uLocal, wLocal, vLocal))
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "... with dimension 4x5:";
	Log::info() << "Performance: " << performance.averageMseconds() << "ms";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestLinearAlgebra::testSingularValueDecompositionDynamic(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with random matrix:";

	bool allSucceeded = true;

	for (const size_t dimension : {5u, 10u, 20u, 50u, 100u})
	{
		Log::info() << " ";

		uint64_t iterations = 0ull;
		uint64_t validIterations = 0ull;

		const size_t rows = dimension;
		const size_t colums = dimension - 1;

		Log::info() << "... with dimension " << rows << "x" << colums << ":";

		HighPerformanceStatistic performance;

		Timestamp startTimestamp(true);

		do
		{
			MatrixT<T> matrix(rows, colums);

			for (size_t n = 0; n < rows * colums; ++n)
			{
				matrix(n) = RandomT<T>::scalar(-1, 1);
			}

			MatrixT<T> uMatrix, wVector, vMatrix;

			performance.start();
				const bool result = matrix.singularValueDecomposition(uMatrix, wVector, vMatrix);
			performance.stop();

			if (result && validateSingularValueDecomposition(matrix, uMatrix, wVector, vMatrix))
			{
				++validIterations;
			}

			++iterations;
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);
		const double percent = double(validIterations) / double(iterations);

		Log::info() << "Performance: " << performance.averageMseconds() << "ms";
		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

		if (percent < 0.99)
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestLinearAlgebra::testQrDecompositionStatic(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	constexpr T m[9] =
	{
		0, 1, 1,
		1, 1, 2,
		0, 0, 3
	};

	constexpr T q[9] =
	{
		 0, -1, 0,
		-1,  0, 0,
		 0,  0, 1
	};

	constexpr T r[9] =
	{
		 -1, -1, -2,
		  0, -1, -1,
		  0,  0,  3
	};

	const MatrixT<T> matrix(3, 3, m);
	const MatrixT<T> groundTruthMatrixQ(3, 3, q);
	const MatrixT<T> groundTruthMatrixR(3, 3, r);

	if (!matrix.isEqual(groundTruthMatrixQ * groundTruthMatrixR, NumericT<T>::weakEps()))
	{
		allSucceeded = false;
	}

	MatrixT<T> matrixQ, matrixR;

	if (!matrix.qrDecomposition(matrixQ, &matrixR))
	{
		allSucceeded = false;
	}

	if (!matrixR.isEqual(groundTruthMatrixR))
	{
		allSucceeded = false;
	}

	if (!matrix.isEqual(matrixQ * matrixR, NumericT<T>::weakEps()))
	{
		allSucceeded = false;
	}

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		MatrixT<T> matrixQLocal, matrixRLocal;

		performance.start();
			const bool result = matrix.qrDecomposition(matrixQLocal, &matrixRLocal);
		performance.stop();

		if (!result)
		{
			allSucceeded = false;
		}

		if (!matrix.isEqual(matrixQLocal * matrixRLocal, NumericT<T>::weakEps()))
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << performance.averageMseconds() << "ms";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestLinearAlgebra::testQrDecompositionDynamic(const double testDuration)
{
	bool allSucceeded = true;

	for (const size_t dimension : {5u, 10u, 20u, 50u, 100u})
	{
		Log::info() << " ";

		uint64_t iterations = 0ull;
		uint64_t validIterations = 0ull;

		const size_t rows = dimension;
		const size_t colums = dimension;

		Log::info() << "... with dimension " << rows << "x" << colums << ":";

		HighPerformanceStatistic performance;

		Timestamp startTimestamp(true);

		do
		{
			MatrixT<T> matrix(rows, colums);

			for (size_t n = 0; n < rows * colums; ++n)
			{
				matrix(n) = RandomT<T>::scalar(-1, 1);
			}

			MatrixT<T> matrixQ, matrixR;

			performance.start();
				const bool result = matrix.qrDecomposition(matrixQ, &matrixR);
			performance.stop();

			if (result && matrix.isEqual(matrixQ * matrixR, NumericT<T>::weakEps()))
			{
				++validIterations;
			}

			++iterations;
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);
		const double percent = double(validIterations) / double(iterations);

		Log::info() << "Performance: " << performance.averageMseconds() << "ms";
		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

		if (percent < 0.99)
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestLinearAlgebra::validateEigenSystem(const SquareMatrixT3<T>& matrix, const VectorT3<T>& expectedEigenValues)
{
	T eigenValues[3] = {NumericT<T>::maxValue(), NumericT<T>::maxValue(), NumericT<T>::maxValue()};

	if (!matrix.eigenValues(eigenValues))
	{
		return false;
	}

	if (eigenValues[0] < eigenValues[1] || eigenValues[1] < eigenValues[2])
	{
		return false;
	}

	if (NumericT<T>::isNotEqual(eigenValues[0], expectedEigenValues[0]) || NumericT<T>::isNotEqual(eigenValues[1], expectedEigenValues[1]) || NumericT<T>::isNotEqual(eigenValues[2], expectedEigenValues[2]))
	{
		return false;
	}

	eigenValues[0] = NumericT<T>::maxValue();
	eigenValues[1] = NumericT<T>::maxValue();
	eigenValues[2] = NumericT<T>::maxValue();

	VectorT3<T> eigenVectors[3];

	if (!matrix.eigenSystem(eigenValues, eigenVectors))
	{
		return false;
	}

	if (eigenValues[0] < eigenValues[1] || eigenValues[1] < eigenValues[2])
	{
		return false;
	}

	if (NumericT<T>::isNotEqual(eigenValues[0], expectedEigenValues[0]) || NumericT<T>::isNotEqual(eigenValues[1], expectedEigenValues[1]) || NumericT<T>::isNotEqual(eigenValues[2], expectedEigenValues[2]))
	{
		return false;
	}

	for (unsigned int n = 0u; n < 3u; ++n)
	{
		if (matrix * eigenVectors[n] != eigenVectors[n] * eigenValues[n])
		{
			return false;
		}
	}

	return true;
}

template <typename T>
bool TestLinearAlgebra::validateSingularValueDecomposition(const MatrixT<T>& matrix, const MatrixT<T>& uMatrix, const MatrixT<T>& wVector, const MatrixT<T>& vMatrix)
{
	ocean_assert(uMatrix.rows() == matrix.rows());
	ocean_assert(wVector.columns() == 1);
	ocean_assert(vMatrix.rows() == matrix.columns());

	const size_t rows = matrix.rows();
	const size_t columns = matrix.columns();

	const MatrixT<T> diagonalMatrix = MatrixT<T>(rows, columns, wVector);
	const MatrixT<T> testMatrix = uMatrix * diagonalMatrix * vMatrix.transposed();

	if (!matrix.isEqual(testMatrix, NumericT<T>::weakEps()))
	{
		return false;
	}

	const MatrixT<T> unitMatrixRows(rows, rows, true);
	const MatrixT<T> testMatrixRows = uMatrix * uMatrix.transposed();

	if (!unitMatrixRows.isEqual(testMatrixRows, NumericT<T>::weakEps()))
	{
		return false;
	}

	const MatrixT<T> unitMatrixColumns(columns, columns, true);
	const MatrixT<T> testMatrixColumns = vMatrix * vMatrix.transposed();

	if (!unitMatrixColumns.isEqual(testMatrixColumns, NumericT<T>::weakEps()))
	{
		return false;
	}

	return true;
}

}

}

}
