// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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

	allSucceeded = testEigenSystemSquareMatrix3(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testEigenSystemMatrix(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSingularValueDecomposition(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testQrDecomposition(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCholeskyDecomposition(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSolve(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
		Log::info() << "Linear Algebra Test succeeded.";
	else
		Log::info() << "Linear Algebra Test FAILED!";

	return allSucceeded;
}
	
#ifdef OCEAN_USE_GTEST
	
TEST(TestLinearAlgebra, EigenSystemSquareMatrix3)
{
	EXPECT_TRUE(TestLinearAlgebra::testEigenSystemSquareMatrix3(GTEST_TEST_DURATION));
}

TEST(TestLinearAlgebra, EigenSystemMatrix)
{
	EXPECT_TRUE(TestLinearAlgebra::testEigenSystemMatrix(GTEST_TEST_DURATION));
}

TEST(TestLinearAlgebra, SingularValueDecomposition)
{
	EXPECT_TRUE(TestLinearAlgebra::testSingularValueDecomposition(GTEST_TEST_DURATION));
}

TEST(TestLinearAlgebra, QrDecomposition)
{
	EXPECT_TRUE(TestLinearAlgebra::testQrDecomposition(GTEST_TEST_DURATION));
}

TEST(TestLinearAlgebra, CholeskyDecomposition)
{
	EXPECT_TRUE(TestLinearAlgebra::testCholeskyDecomposition(GTEST_TEST_DURATION));
}

TEST(TestLinearAlgebra, Solve)
{
	EXPECT_TRUE(TestLinearAlgebra::testSolve(GTEST_TEST_DURATION));
}
	
#endif // OCEAN_USE_GTEST

bool TestLinearAlgebra::testEigenSystemSquareMatrix3(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Eigensystem of a SquareMatrix3 matrix:";

	bool allSucceeded = true;

	// Matrix:
	// |  7   0  -3 |
	// | -9  -2   3 |
	// | 18   0  -8 |
	// EigenValues: 1, -2, -2
	// EigenVectors are: (1, -1, 2); (1, 0, 3) or (1, 1, 3)
	allSucceeded = testEigenSystemSquareMatrix3Static(SquareMatrix3(7, -9, 18, 0, -2, 0, -3, 3, -8), Vector3(1, -2, -2)) && allSucceeded;

	// Matrix:
	// | -1 4 -4 |
	// | -4 7 -4 |
	// | -4 4 -1 |
	// EigenValues: 3, 3, -1
	// EigenVectors: (1 0 1); (1 0 1); (1, 1, 1)
	allSucceeded = testEigenSystemSquareMatrix3Static(SquareMatrix3(-1, -4, -4, 4, 7, 4, -4, -4, -1), Vector3(3, 3, -1)) && allSucceeded;

	// Matrix:
	// | 0 1 0 |
	// | 0 2 0 |
	// | 0 0 3 |
	// EigenValues: 3, 2, 0
	// EigenVectors: (0, 0, 1); (1, 2, 0); (1, 0, 0)
	allSucceeded = testEigenSystemSquareMatrix3Static(SquareMatrix3(0, 0, 0, 1, 2, 0, 0, 0, 3), Vector3(3, 2, 0)) && allSucceeded;

	/// No we test several random matrices
	allSucceeded = testEigenSystemSquareMatrix3Dynamic(testDuration) && allSucceeded;

	return allSucceeded;
}
	
bool TestLinearAlgebra::testEigenSystemMatrix(const double testDuration)
{
	ocean_assert(testDuration > 0.0);
	
	Log::info() << "Eigensystem of a 4x4 matrix:";
	
	bool allSucceeded = true;
	
	const static Scalar eigenMatrixValues[16] =
	{
		Scalar(0.4), -3, 6, Scalar(-3.5),
		-3, 30, Scalar(-67.5), 42,
		6, Scalar(-67.5), 162, -105,
		Scalar(-3.5), 42, -105, 70
	};
	
	const Matrix matrix(4, 4, eigenMatrixValues);
	Matrix values, vectors;
	
	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);
	
	do
	{
		performance.start();
		allSucceeded = matrix.eigenSystem(values, vectors) && allSucceeded;
		performance.stop();
		
		const Matrix diagonal(4, 4, values);
		const Matrix result = vectors * diagonal * vectors.transposed();
		
		allSucceeded = matrix.isEqual(result, Numeric::weakEps()) && allSucceeded;
	}
	while (startTimestamp + testDuration > Timestamp(true));
	
	Log::info() << "Performance: " << performance.averageMseconds() << "ms";
	
	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";
	
	return allSucceeded;
}
	
bool TestLinearAlgebra::testSingularValueDecomposition(const double testDuration)
{
	ocean_assert(testDuration > 0.0);
	
	Log::info() << "Singular value decomposition test:";
	Log::info() <<  " ";
	
	bool allSucceeded = true;
	
	allSucceeded = testSingularValueDecompositionStatic(testDuration) && allSucceeded;
	
	allSucceeded = testSingularValueDecompositionDynamic(testDuration) && allSucceeded;
	
	return allSucceeded;
}
	
bool TestLinearAlgebra::testQrDecomposition(const double testDuration)
{
	ocean_assert(testDuration > 0.0);
	
	Log::info() << "QR decomposition test:";
	Log::info() << " ";
	
	bool allSucceeded = true;
	
	allSucceeded = testQrDecompositionStatic(testDuration) && allSucceeded;
	
	allSucceeded = testQrDecompositionDynamic(testDuration) && allSucceeded;
	
	return allSucceeded;
}
	
bool TestLinearAlgebra::testCholeskyDecomposition(const double testDuration)
{
	ocean_assert(testDuration > 0.0);
	
	bool allSucceeded = true;
	
	Log::info() << "Cholesky decomposition of random matrices with different sizes:";
	
	const unsigned int dimensions[] = {5u, 10u, 20u, 50u, 100u};
	
	for (unsigned int d = 0u; d < sizeof(dimensions) / sizeof(dimensions[0]); ++d)
	{
		Log::info() << " ";
		
		unsigned long long iterations = 0ull;
		unsigned long long validIterations = 0ull;
		
		const size_t dimension = dimensions[d];
		
		Log::info() << "... with dimension " << dimension << "x" << dimension << ":";
		
		HighPerformanceStatistic performance;
		Timestamp startTimestamp(true);
		
		do
		{
			Matrix matrix(dimension, dimension);
			
			for (size_t n = 0; n < matrix.elements(); ++n)
				matrix(n) = Random::scalar(-1, 1);
			
			const Matrix squaredMatrix = matrix.transposedMultiply(matrix);
			
			Matrix matrixL;
			
			performance.start();
			const bool result = squaredMatrix.choleskyDecomposition(matrixL);
			performance.stop();
			
			const Matrix matrixValidate = matrixL * matrixL.transposed();
			
			if (result && squaredMatrix.isEqual(matrixValidate, Numeric::weakEps()))
				validIterations++;
			
			iterations++;
		}
		while (startTimestamp + testDuration > Timestamp(true));
		
		ocean_assert(iterations != 0ull);
		const double percent = double(validIterations) / double(iterations);
		
		Log::info() << "Performance: " << performance.averageMseconds() << "ms";
		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "%";
		
		if (percent < 0.95)
			allSucceeded = false;
	}
	
	Log::info() << " ";
	
	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";
	
	return allSucceeded;
}

bool TestLinearAlgebra::testEigenSystemSquareMatrix3Static(const SquareMatrix3& matrix, const Vector3& eigenValues)
{
	Scalar values[3] = {Numeric::maxValue(), Numeric::maxValue(), Numeric::maxValue()};

	if (!matrix.eigenValues(values))
		return false;

	if (values[0] < values[1] || values[1] < values[2])
		return false;

	if (Numeric::isNotEqual(values[0], eigenValues[0]) || Numeric::isNotEqual(values[1], eigenValues[1]) || Numeric::isNotEqual(values[2], eigenValues[2]))
		return false;

	Vector3 eigenVectors[3];
	values[0] = Numeric::maxValue();
	values[1] = Numeric::maxValue();
	values[2] = Numeric::maxValue();

	if (!matrix.eigenSystem(values, eigenVectors))
		return false;

	if (values[0] < values[1] || values[1] < values[2])
		return false;

	if (Numeric::isNotEqual(values[0], eigenValues[0]) || Numeric::isNotEqual(values[1], eigenValues[1]) || Numeric::isNotEqual(values[2], eigenValues[2]))
		return false;

	for (unsigned int n = 0u; n < 3u; ++n)
		if (matrix * eigenVectors[n] != eigenVectors[n] * values[n])
			return false;

	return true;
}

bool TestLinearAlgebra::testEigenSystemSquareMatrix3Dynamic(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Scalar values[3];
	Vector3 vectors[3];

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);

	do
	{
		bool localSucceeded = true;

		const Vector3 xAxis(Vector3(1, 0, 0));
		const Vector3 yAxis(Vector3(0, 1, 0));
		const Vector3 zAxis(Vector3(0, 0, 1));

		Scalar xLength = Random::scalar(Scalar(0.01), 10);
		Scalar yLength = Random::scalar(Scalar(0.01), 10);
		Scalar zLength = Random::scalar(Scalar(0.01), 10);

		const SquareMatrix3 matrix(xAxis * xLength, yAxis * yLength, zAxis * zLength);

		performance.start();

		if (!matrix.eigenSystem(values, vectors))
			localSucceeded = false;

		performance.stop();

		Utilities::sortHighestToFront3(xLength, yLength, zLength);

		if (Numeric::isNotWeakEqual(xLength, values[0]))
			localSucceeded = false;
		if (Numeric::isNotWeakEqual(yLength, values[1]))
			localSucceeded = false;
		if (Numeric::isNotWeakEqual(zLength, values[2]))
			localSucceeded = false;

		if (localSucceeded)
			validIterations++;

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Performance: " << performance.averageMseconds() * 1000.0 << "mys";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	if (std::is_same<Scalar, float>::value)
		return percent >= 0.95;

	return percent >= 0.99;
}

bool TestLinearAlgebra::testSingularValueDecompositionStatic(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	const static Scalar m[20] =
	{
		1, 0, 0, 0, 2,
		0, 0, 3, 0, 0,
		0, 0, 0, 0, 0,
		0, 2, 0, 0, 0
	};

	const static Scalar u[16] =
	{
		0, 1, 0, 0,
		1, 0, 0, 0,
		0, 0, 0, -1,
		0, 0, 1, 0
	};

	const static Scalar w[5] =
	{
		3, Numeric::sqrt(5), 2, 0, 0
	};

	const static Scalar v[25] =
	{
		 0, 0, 1, 0, 0,
		 Numeric::sqrt(Scalar(0.2)), 0, 0, 0, Numeric::sqrt(Scalar(0.8)),
		 0, 1, 0, 0, 0,
		 0, 0, 0, 1, 0,
		 -Numeric::sqrt(Scalar(0.8)), 0, 0, 0, Numeric::sqrt(Scalar(0.2))
	};

	const Scalar epsilon = std::is_same<float, Scalar>::value ? Numeric::eps() * 10 : Numeric::eps();

	const Matrix matrix = Matrix(4, 5, m);
	const Matrix matrixT(matrix.transposed());

	const Matrix uMatrix = Matrix(4, 4, u);
	const Matrix wVector = Matrix(5, 1, w);
	const Matrix vMatrix = Matrix(5, 5, v);

	allSucceeded = validateSingularValueDecomposition(matrix, uMatrix, wVector, vMatrix.transposed()) && allSucceeded;

	{
		Matrix uLocal, wLocal, vLocal;
		allSucceeded = matrix.singularValueDecomposition(uLocal, wLocal, vLocal) && allSucceeded;
		allSucceeded = wLocal.isEqual(wVector, epsilon) && allSucceeded;
		allSucceeded = validateSingularValueDecomposition(matrix, uLocal, wLocal, vLocal) && allSucceeded;
	}

	{
		Matrix uLocal, wLocal, vLocal;
		allSucceeded = matrixT.singularValueDecomposition(uLocal, wLocal, vLocal) && allSucceeded;
		allSucceeded = wLocal.isEqual(wVector, epsilon) && allSucceeded;
		allSucceeded = validateSingularValueDecomposition(matrixT, uLocal, wLocal, vLocal) && allSucceeded;
	}

	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);

	do
	{
		Matrix uLocal, wLocal, vLocal;

		performance.start();
		matrix.singularValueDecomposition(uLocal, wLocal, vLocal);
		performance.stop();

		allSucceeded = validateSingularValueDecomposition(matrix, uLocal, wLocal, vLocal) && allSucceeded;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "... with dimension 4x5:";
	Log::info() << "Performance: " << performance.averageMseconds() << "ms";

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

bool TestLinearAlgebra::testSingularValueDecompositionDynamic(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	const size_t dimensions[] = {5u, 10u, 20u, 50u, 100u};

	for (unsigned int d = 0u; d < sizeof(dimensions) / sizeof(dimensions[0]); ++d)
	{
		Log::info() << " ";

		unsigned long long iterations = 0ull;
		unsigned long long validIterations = 0ull;

		const size_t dimension = dimensions[d];

		const size_t rows = dimension;
		const size_t colums = dimension - 1;

		Log::info() << "... with dimension " << rows << "x" << colums << ":";

		HighPerformanceStatistic performance;
		Timestamp startTimestamp(true);

		do
		{
			Matrix matrix(rows, colums);

			for (size_t n = 0; n < rows * colums; ++n)
				matrix(n) = Random::scalar(-1, 1);

			Matrix uMatrix, wVector, vMatrix;

			performance.start();
			const bool result = matrix.singularValueDecomposition(uMatrix, wVector, vMatrix);
			performance.stop();

			if (result && validateSingularValueDecomposition(matrix, uMatrix, wVector, vMatrix))
				validIterations++;

			iterations++;
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);
		const double percent = double(validIterations) / double(iterations);

		Log::info() << "Performance: " << performance.averageMseconds() << "ms";
		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

		if (percent < 0.99)
			allSucceeded = false;
	}

	Log::info() << " ";

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

bool TestLinearAlgebra::validateSingularValueDecomposition(const Matrix& matrix, const Matrix& uMatrix, const Matrix& wVector, const Matrix& vMatrix)
{
	ocean_assert(uMatrix.rows() == matrix.rows());
	ocean_assert(wVector.columns() == 1);
	ocean_assert(vMatrix.rows() == matrix.columns());

	const size_t rows = matrix.rows();
	const size_t columns = matrix.columns();

	const Matrix diagonalMatrix = Matrix(rows, columns, wVector);
	const Matrix testMatrix = uMatrix * diagonalMatrix * vMatrix.transposed();

	if (!matrix.isEqual(testMatrix, Numeric::weakEps()))
		return false;

	const Matrix unitMatrixRows(rows, rows, true);
	const Matrix testMatrixRows = uMatrix * uMatrix.transposed();

	if (!unitMatrixRows.isEqual(testMatrixRows, Numeric::weakEps()))
		return false;

	const Matrix unitMatrixColumns(columns, columns, true);
	const Matrix testMatrixColumns = vMatrix * vMatrix.transposed();

	if (!unitMatrixColumns.isEqual(testMatrixColumns, Numeric::weakEps()))
		return false;

	return true;
}

bool TestLinearAlgebra::testQrDecompositionStatic(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	const static Scalar m[9] =
	{
		0, 1, 1,
		1, 1, 2,
		0, 0, 3
	};

	const static Scalar q[9] =
	{
		 0, -1, 0,
		-1,  0, 0,
		 0,  0, 1
	};

	const static Scalar r[9] =
	{
		 -1, -1, -2,
		  0, -1, -1,
		  0,  0,  3
	};

	const Matrix matrix = Matrix(3, 3, m);
	const Matrix groundTruthMatrixQ = Matrix(3, 3, q);
	const Matrix groundTruthMatrixR = Matrix(3, 3, r);

	allSucceeded = matrix.isEqual(groundTruthMatrixQ * groundTruthMatrixR, Numeric::weakEps()) && allSucceeded;

	Matrix matrixQ, matrixR;
	allSucceeded = matrix.qrDecomposition(matrixQ, &matrixR) && allSucceeded;
	allSucceeded = matrixR.isEqual(groundTruthMatrixR) && allSucceeded;
	allSucceeded = matrix.isEqual(matrixQ * matrixR, Numeric::weakEps()) && allSucceeded;

	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);

	do
	{
		Matrix matrixQLocal, matrixRLocal;

		performance.start();
		matrix.qrDecomposition(matrixQLocal, &matrixRLocal);
		performance.stop();

		allSucceeded = matrix.isEqual(matrixQLocal * matrixRLocal, Numeric::weakEps()) && allSucceeded;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "... with dimension 3x3:";
	Log::info() << "Performance: " << performance.averageMseconds() << "ms";

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

bool TestLinearAlgebra::testQrDecompositionDynamic(const double testDuration)
{
	bool allSucceeded = true;

	const unsigned int dimensions[] = {5u, 10u, 20u, 50u, 100u};

	for (unsigned int d = 0u; d < sizeof(dimensions) / sizeof(dimensions[0]); ++d)
	{
		Log::info() << " ";

		unsigned long long iterations = 0ull;
		unsigned long long validIterations = 0ull;

		const size_t dimension = dimensions[d];

		const size_t rows = dimension;
		const size_t colums = dimension;

		Log::info() << "... with dimension " << rows << "x" << colums << ":";

		HighPerformanceStatistic performance;
		Timestamp startTimestamp(true);

		do
		{
			Matrix matrix(rows, colums);

			for (size_t n = 0; n < rows * colums; ++n)
				matrix(n) = Random::scalar(-1, 1);

			Matrix matrixQ, matrixR;

			performance.start();
			const bool result = matrix.qrDecomposition(matrixQ, &matrixR);
			performance.stop();

			if (result && matrix.isEqual(matrixQ * matrixR, Numeric::weakEps()))
				validIterations++;

			iterations++;
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);
		const double percent = double(validIterations) / double(iterations);

		Log::info() << "Performance: " << performance.averageMseconds() << "ms";
		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

		if (percent < 0.99)
			allSucceeded = false;
	}

	Log::info() << " ";

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

bool TestLinearAlgebra::testSolve(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Solve test:";

	bool allSucceeded = true;

	std::vector<unsigned int> dimensions = {5u, 10u, 20u};

	if (std::is_same<Scalar, double>::value)
	{
		dimensions.push_back(50u);
		dimensions.push_back(100u);
	}

	for (unsigned int d = 0; d < dimensions.size(); ++d)
	{
		Log::info() << " ";

		unsigned long long iterations = 0ull;
		unsigned long long validIterations = 0ull;

		const size_t dimension = dimensions[d];

		Log::info() << "... with dimension " << dimension << "x" << dimension << ":";

		HighPerformanceStatistic performance;
		Timestamp startTimestamp(true);

		do
		{
			Matrix a0(dimension, dimension);
			Matrix x0(dimension, 1);

			for (unsigned int n = 0; n < a0.elements(); ++n)
				a0(n) = Random::scalar(0, 1);

			for (unsigned int n = 0; n < x0.elements(); ++n)
				x0(n) = Random::scalar(0, 1);

			const Matrix b0 = a0 * x0;

			Matrix x1;

			performance.start();
			const bool result = a0.solve(b0, x1);
			performance.stop();

			if (result && x0.isEqual(x1, Numeric::weakEps()) && b0.isEqual(a0 * x1, Numeric::weakEps()))
				validIterations++;

			iterations++;
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);
		const double percent = double(validIterations) / double(iterations);

		Log::info() << "Performance: " << performance.averageMseconds() << "ms";
		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

		if (percent < 0.95)
			allSucceeded = false;
	}

	Log::info() << " ";

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

}

}

}
