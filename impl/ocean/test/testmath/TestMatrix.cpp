/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestMatrix.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Matrix.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestMatrix::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Matrix test:   ---";
	Log::info() << " ";

	allSucceeded = testElementConstructor(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSubMatrixConstructor(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSelfSquareMatrix(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSelfTransposedSquareMatrixExistingResult(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSelfTransposedSquareMatrix(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testWeightedSelfTransposedSquareMatrixExistingResult(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testInvert(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPseudoInverted(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRank(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNonNegativeMatrixFactorization(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMatrixMultiplication(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Matrix test succeeded.";
	}
	else
	{
		Log::info() << "Matrix test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestMatrix, ElementConstructor)
{
	EXPECT_TRUE(TestMatrix::testElementConstructor(GTEST_TEST_DURATION));
}

TEST(TestMatrix, SubMatrixConstructor)
{
	EXPECT_TRUE(TestMatrix::testSubMatrixConstructor(GTEST_TEST_DURATION));
}

TEST(TestMatrix, SelfSquareMatrix)
{
	EXPECT_TRUE(TestMatrix::testSelfSquareMatrix(GTEST_TEST_DURATION));
}

TEST(TestMatrix, SelfTransposedSquareMatrixExistingResult)
{
	EXPECT_TRUE(TestMatrix::testSelfTransposedSquareMatrixExistingResult(GTEST_TEST_DURATION));
}

TEST(TestMatrix, SelfTransposedSquareMatrix)
{
	EXPECT_TRUE(TestMatrix::testSelfTransposedSquareMatrix(GTEST_TEST_DURATION));
}

TEST(TestMatrix, WeightedSelfTransposedSquareMatrixExistingResult)
{
	EXPECT_TRUE(TestMatrix::testWeightedSelfTransposedSquareMatrixExistingResult(GTEST_TEST_DURATION));
}

TEST(TestMatrix, Invert)
{
	EXPECT_TRUE(TestMatrix::testInvert(GTEST_TEST_DURATION));
}

TEST(TestMatrix, PseudoInverted)
{
	EXPECT_TRUE(TestMatrix::testPseudoInverted(GTEST_TEST_DURATION));
}

TEST(TestMatrix, Rank)
{
	EXPECT_TRUE(TestMatrix::testRank(GTEST_TEST_DURATION));
}

TEST(TestMatrix, NonNegativeMatrixFactorization)
{
	EXPECT_TRUE(TestMatrix::testNonNegativeMatrixFactorization(GTEST_TEST_DURATION));
}

TEST(TestMatrix, MatrixMultiplication)
{
	EXPECT_TRUE(TestMatrix::testMatrixMultiplication(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestMatrix::testElementConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Element-based constructor test:";

	bool allSucceeded = true;
	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const size_t rows = RandomI::random(randomGenerator, 1u, 20u);
		const size_t columns = RandomI::random(randomGenerator, 1u, 20u);

		Scalars values(rows * columns);

		for (size_t i = 0; i < values.size(); ++i)
		{
			values[i] = Random::scalar(randomGenerator, -100, 100);
		}

		const Matrix matrixA(rows, columns, values.data());
		const Matrix matrixB(rows, columns, values.data(), true);
		const Matrix matrixBTransposed(rows, columns, values.data(), false);

		Matrix test(rows, columns), testTransposed(rows, columns); // here the transposed matrix is not actually transposed but expects a transposed value representation

		size_t index = 0;
		for (size_t r = 0; r < rows; ++r)
		{
			for (size_t c = 0; c < columns; ++c)
			{
				test(r, c) = values[index++];
			}
		}

		ocean_assert(index == rows * columns);

		index = 0;
		for (size_t c = 0; c < columns; ++c)
		{
			for (size_t r = 0; r < rows; ++r)
			{
				testTransposed(r, c) = values[index++];
			}
		}

		ocean_assert(index == rows * columns);

		if (!matrixA.isEqual(test))
		{
			allSucceeded = false;
		}
		if (!matrixB.isEqual(test))
		{
			allSucceeded = false;
		}
		if (!matrixBTransposed.isEqual(testTransposed))
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

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

bool TestMatrix::testSubMatrixConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Sub-matrix-based constructor test:";

	bool allSucceeded = true;
	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const size_t subMatrixRows = RandomI::random(randomGenerator, 1u, 20u);
		const size_t subMatrixColumns = RandomI::random(randomGenerator, 1u, 20u);

		Matrix subMatrix(subMatrixRows, subMatrixColumns);

		for (size_t r = 0; r < subMatrix.rows(); ++r)
		{
			for (size_t c = 0; c < subMatrix.columns(); ++c)
			{
				subMatrix(r, c) = Random::scalar(randomGenerator, -100, 100);
			}
		}

		const size_t rows = RandomI::random(randomGenerator, 1u, 20u);
		const size_t columns = RandomI::random(randomGenerator, 1u, 20u);

		const size_t row = RandomI::random(randomGenerator, 0u, (unsigned int)rows - 1u);
		const size_t column = RandomI::random(randomGenerator, 0u, (unsigned int)columns - 1u);

		const Matrix matrix(rows, columns, subMatrix, row, column);

		for (size_t r = 0; r < matrix.rows(); ++r)
		{
			for (size_t c = 0; c < matrix.columns(); ++c)
			{
				if (r < row || c < column)
				{
					if (matrix(r, c) != Scalar(0))
					{
						allSucceeded = false;
					}
				}
				else if (r < row + subMatrix.rows() && c < column + subMatrix.columns())
				{
					if (matrix(r, c) != subMatrix(r - row, c - column))
					{
						allSucceeded = false;
					}
				}
				else
				{
					if (matrix(r, c) != Scalar(0))
					{
						allSucceeded = false;
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

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

bool TestMatrix::testSelfSquareMatrix(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing self-square matrix, matrix * matrix.transposed():";

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const Scalar valueRange = std::is_same<float, Scalar>::value ? 10 : 100;

	HighPerformanceStatistic performance;
	HighPerformanceStatistic performanceNaive;
	const Timestamp startTimestamp(true);

	do
	{
		Matrix matrix(RandomI::random(1u, 32u), RandomI::random(1u, 32u));

		for (unsigned int n = 0u; n < matrix.elements(); ++n)
		{
			matrix(n) = Random::scalar(-valueRange, valueRange);
		}

		performance.start();
			const Matrix squareMatrix = matrix.selfSquareMatrix();
		performance.stop();

		performanceNaive.start();
			const Matrix naiveMatrix = matrix * matrix.transposed();
		performanceNaive.stop();

		if (squareMatrix.isEqual(naiveMatrix, Numeric::eps() * 500))
		{
			validIterations++;
		}

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Standard performance: " << performanceNaive.averageMseconds() << "ms";
	Log::info() << "Ocean performance: " << performance.averageMseconds() << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestMatrix::testSelfTransposedSquareMatrixExistingResult(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing self-transposed square matrix with existing result, matrix.transposed() * matrix:";

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const Scalar valueRange = std::is_same<float, Scalar>::value ? 10 : 100;

	HighPerformanceStatistic performance;
	HighPerformanceStatistic performanceNaive;

	const Timestamp startTimestamp(true);

	Matrix result;

	do
	{
		Matrix matrix(RandomI::random(1u, 32u), RandomI::random(1u, 32u));

		for (unsigned int n = 0u; n < matrix.elements(); ++n)
		{
			matrix(n) = Random::scalar(-valueRange, valueRange);
		}

		result.resize(matrix.columns(), matrix.columns());

		performance.start();
			matrix.selfTransposedSquareMatrix(result);
		performance.stop();

		performanceNaive.start();
			const Matrix naiveMatrix = matrix.transposed() * matrix;
		performanceNaive.stop();

		if (result.isEqual(naiveMatrix, Numeric::eps() * 500))
		{
			validIterations++;
		}

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Standard performance: " << performanceNaive.averageMseconds() << "ms";
	Log::info() << "Ocean performance: " << performance.averageMseconds() << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestMatrix::testSelfTransposedSquareMatrix(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing self-transposed square matrix, matrix.transposed() * matrix:";

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const Scalar valueRange = std::is_same<float, Scalar>::value ? 10 : 100;

	HighPerformanceStatistic performance;
	HighPerformanceStatistic performanceNaive;

	const Timestamp startTimestamp(true);

	do
	{
		Matrix matrix(RandomI::random(1u, 32u), RandomI::random(1u, 32u));

		for (unsigned int n = 0u; n < matrix.elements(); ++n)
			matrix(n) = Random::scalar(-valueRange, valueRange);

		performance.start();
			const Matrix squareMatrix = matrix.selfTransposedSquareMatrix();
		performance.stop();

		performanceNaive.start();
			const Matrix naiveMatrix = matrix.transposed() * matrix;
		performanceNaive.stop();

		if (squareMatrix.isEqual(naiveMatrix, Numeric::eps() * 500))
		{
			validIterations++;
		}

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Standard performance: " << performanceNaive.averageMseconds() << "ms";
	Log::info() << "Ocean performance: " << performance.averageMseconds() << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestMatrix::testWeightedSelfTransposedSquareMatrixExistingResult(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing self-transposed weighted square matrix with existing result, matrix.transposed() * diag(weights) * matrix:";

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const Scalar valueRange = std::is_same<float, Scalar>::value ? 10 : 100;

	HighPerformanceStatistic performance;
	HighPerformanceStatistic performanceNaive;

	const Timestamp startTimestamp(true);

	Matrix result;

	do
	{
		Matrix matrix(RandomI::random(1u, 32u), RandomI::random(1u, 32u));

		for (unsigned int n = 0u; n < matrix.elements(); ++n)
		{
			matrix(n) = Random::scalar(-valueRange, valueRange);
		}

		Scalars weights(matrix.rows());
		Matrix weightMatrix(matrix.rows(), matrix.rows(), false);

		for (size_t n = 0u; n < weights.size(); ++n)
		{
			const Scalar weight = Random::scalar(-1, 1);
			weights[n] = weight;
			weightMatrix(n, n) = weight;
		}

		result.resize(matrix.columns(), matrix.columns());

		performance.start();
			matrix.weightedSelfTransposedSquareMatrix(Matrix(matrix.rows(), 1, weights.data()), result);
		performance.stop();

		performanceNaive.start();
			const Matrix naiveMatrix = matrix.transposed() * weightMatrix * matrix;
		performanceNaive.stop();

		if (result.isEqual(naiveMatrix, Numeric::eps() * 500))
		{
			validIterations++;
		}

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Standard performance: " << performanceNaive.averageMseconds() << "ms";
	Log::info() << "Ocean performance: " << performance.averageMseconds() << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestMatrix::testInvert(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Invert test:";

	const Scalar epsilon = std::is_same<float, Scalar>::value ? Scalar(0.001) : Numeric::weakEps();

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		bool localSucceeded = true;

		{
			const SquareMatrix3 matrix3(Random::quaternion());

			Matrix matrix(3, 3);
			for (unsigned int r = 0u; r < 3u; ++r)
			{
				for (unsigned int c = 0u; c < 3u; ++c)
				{
					matrix(r, c) = matrix3(r, c);
				}
			}

			const Matrix copy(matrix);
			const Matrix identity(3, 3, true);

			if (matrix.invert())
			{
				SquareMatrix3 inverted3;
				for (unsigned int r = 0u; r < 3u; ++r)
				{
					for (unsigned int c = 0u; c < 3u; ++c)
					{
						inverted3(r, c) = matrix(r, c);
					}
				}

				if (inverted3 != matrix3.inverted())
				{
					localSucceeded = false;
				}
				else
				{
					const Matrix inverted(copy.inverted());

					if (inverted * copy != identity)
					{
						localSucceeded = false;
					}
				}
			}
			else
			{
				localSucceeded = false;
			}
		}

		{
			unsigned int size = RandomI::random(5u, 20u);

			Matrix matrix(size, size);

			for (unsigned int n = 0u; n < matrix.rows() * matrix.columns(); ++n)
			{
				matrix(n) = Random::scalar(-100, 100);
			}

			const Matrix copy(matrix);
			const Matrix identity(size, size, true);

			if (matrix.invert())
			{
				if (!(matrix * copy).isEqual(identity, epsilon))
				{
					localSucceeded = false;
				}
				else
				{
					const Matrix inverted(copy.inverted());

					if (!(inverted * copy).isEqual(identity, epsilon))
					{
						localSucceeded = false;
					}
				}
			}
		}

		{
			unsigned int size = RandomI::random(5u, 20u);

			Matrix vector(size, 1);

			for (unsigned int n = 0u; n < size; ++n)
			{
				vector(n) = Random::scalar(-1, 1) * ((std::is_same<Scalar, float>::value) ? Scalar(10) : Scalar(100));
			}

			// each row/column is a linear combination of one row/column thus this matrix is singular and cannot be inverted
			Matrix matrix = vector * vector.transposed();

			const Matrix copy(matrix);
			const Matrix identity(size, size, true);

			if (matrix.invert())
			{
				localSucceeded = false;
			}
		}

		if (localSucceeded)
		{
			validIterations++;
		}

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestMatrix::testPseudoInverted(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Pseudoinverted test:";
	Log::info() << " ";

	const Scalar tolerance = Numeric::eps() * Scalar(500);
	const Scalar valueRange = std::is_same<float, Scalar>::value ? 10 : 100;

	std::vector<size_t> dimensions = {10, 20};
	if (std::is_same<Scalar, double>::value)
	{
		dimensions.push_back(50);
		dimensions.push_back(100);
	}

	for (size_t d = 0; d < dimensions.size(); ++d)
	{
		const size_t dimension = dimensions[d];

		Log::info() << "... with dimension " << dimension << "x" << dimension;

		HighPerformanceStatistic performance;

		const Timestamp startTimestamp(true);

		do
		{
			Matrix matrix(dimension, dimension);
			for (size_t n = 0; n < matrix.elements(); ++n)
			{
				matrix(n) = Random::scalar(-valueRange, valueRange);
			}

			performance.start();
				matrix.pseudoInverted(tolerance);
			performance.stop();

		}
		while (startTimestamp + testDuration > Timestamp(true));

		Log::info() << "Performance: " << performance.averageMseconds() << "ms";
	}

	Log::info() << " ";

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		bool localSucceeded = true;

		// 1. test criteria: A* = A^{-1}, for A element of SquareMatrix(m x m)
		// the pseudoinverted Matrix is equal the inverted Matrix
		{
			Matrix matrixInverted;
			Matrix matrix(3u, 3u);

			do
			{
				for (size_t i = 0; i < matrix.elements(); ++i)
				{
					matrix(i) = Random::scalar(-valueRange, valueRange);
				}

				matrixInverted = matrix;
			}
			while (!matrixInverted.invert());
			const Matrix matrixPseudoInverted = matrix.pseudoInverted();

			if (!matrixInverted.isEqual(matrixPseudoInverted, tolerance))
			{
				localSucceeded = false;
			}
		}

		Matrix matrixA(RandomI::random(3u, 8u), RandomI::random(3u, 8u));
		for (size_t i = 0; i < matrixA.elements(); ++i)
		{
			matrixA(i) = Random::scalar(-valueRange, valueRange);
		}

		const Matrix pseudoinverseA = matrixA.pseudoInverted(tolerance);

		// 2. test criteria AA*A = A
		// AA* need not be the general identity matrix, but it maps all column vectors of A to themselves
		{
			const Matrix resultA = matrixA * pseudoinverseA * matrixA;
			if (!resultA.isEqual(matrixA, tolerance))
			{
				localSucceeded = false;
			}
		}

		// 3. test criteria: A*AA* = A*
		// A+ is a weak inverse for the multiplicative semigroup
		{
			const Matrix resultA = pseudoinverseA * matrixA * pseudoinverseA;
			if (!resultA.isEqual(pseudoinverseA, tolerance))
			{
				localSucceeded = false;
			}
		}

		// 4. test criteria: (A*)* = A
		{
			const Matrix resultA = pseudoinverseA.pseudoInverted();
			if (!resultA.isEqual(matrixA, tolerance))
			{
				localSucceeded = false;
			}
		}

		// 5. test criteria: (AA*)^ = AA*
		// AA^ is Hermitian (also called conjugate transpose)
		{
			const Matrix matrixLeft = (matrixA * pseudoinverseA).transposed();
			const Matrix matrixRight = matrixA * pseudoinverseA;
			if (!matrixLeft.isEqual(matrixRight, tolerance))
			{
				localSucceeded = false;
			}
		}

		// 6. test criteria: (A*A)^ = A*A
		// AA^ is Hermitian (also called conjugate transpose)
		{
			const Matrix matrixLeft = (pseudoinverseA * matrixA).transposed();
			const Matrix matrixRight = pseudoinverseA * matrixA;
			if (!matrixLeft.isEqual(matrixRight, tolerance))
			{
				localSucceeded = false;
			}
		}

		if (localSucceeded)
		{
			validIterations++;
		}

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	const double threshold = std::is_same<Scalar, double>::value ? 0.99 : 0.90;

	return percent >= threshold;
}

bool TestMatrix::testRank(const double testDuration)
{
	Log::info() << "Rank test:";

	constexpr unsigned int size = 100;

	Timestamp startTimestamp(true);

	HighPerformanceStatistic performance;

	do
	{
		Matrix matrix(size, size);

		for (unsigned int n = 0u; n < matrix.elements(); ++n)
		{
			matrix(n) = Random::scalar(-100, 100);
		}

		performance.start();
			matrix.rank();
		performance.stop();

	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance (" << size << "^2): " << performance.averageMseconds() << "ms";

	bool allSucceeded = true;
	{
		const Matrix zeroMatrix1(1, 1, false);
		if (zeroMatrix1.rank() != 0)
		{
			allSucceeded = false;
		}

		const Matrix zeroMatrix3(3, 3, false);
		if (zeroMatrix3.rank() != 0)
		{
			allSucceeded = false;
		}

		const Matrix zeroMatrix7(7, 7, false);
		if (zeroMatrix7.rank() != 0)
		{
			allSucceeded = false;
		}
	}

	{
		const Matrix identityMatrix1(1, 1, true);
		if (identityMatrix1.rank() != 1)
		{
			allSucceeded = false;
		}

		const Matrix identityMatrix3(3, 3, true);
		if (identityMatrix3.rank() != 3)
		{
			allSucceeded = false;
		}

		const Matrix identityMatrix7(7, 7, true);
		if (identityMatrix7.rank() != 7)
		{
			allSucceeded = false;
		}
	}

	{
		const Scalar data[9] = {1, 2, 3, 0, 5, 4, 0, 10, 2};
		const Matrix matrix(3, 3, data);
		if (matrix.rank() != 3)
		{
			allSucceeded = false;
		}
	}

	{
		const Scalar data[9] = {1, 2, 3, 0, 6, 4, 0, 3, 2};
		const Matrix matrix(3, 3, data);
		if (matrix.rank() != 2)
		{
			allSucceeded = false;
		}
	}

	{
		const Scalar data[6] = {2, 3, 0, 1, 4, -1};
		const Matrix matrix(3, 2, data);
		if (matrix.rank() != 2)
		{
			allSucceeded = false;
		}
	}

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

bool TestMatrix::testNonNegativeMatrixFactorization(const double testDuration, const unsigned int maxComponents)
{
	// **TODO** **SS** use random dimension for validation, not for performance measurements

	const unsigned int rangeMin = std::max(maxComponents, std::is_same<float, Scalar>::value ? 5u : 10u);
	const unsigned int rangeMax = std::is_same<float, Scalar>::value ? 10u : 100u;

	const unsigned int rows = Random::random(rangeMin, rangeMax);
	const unsigned int columns = Random::random(rangeMin, rangeMax);

	bool allSucceeded = true;

	Log::info() << "Non-negative matrix factorization test with " << rows << " x " << columns << " matrix";

	Matrix matrix(rows, columns);

	for (size_t row = 0u; row < rows; ++row)
	{
		for (size_t col = 0u; col < columns; ++col)
		{
			matrix[row][col] = Random::scalar(Scalar(1), Scalar(row * col + 1));
		}
	}

	for (unsigned int components = 1u; components < maxComponents; ++components)
	{
		Scalar sumError = 0;

		uint64_t iterations = 0ull;
		uint64_t validIterations = 0ull;

		HighPerformanceStatistic performance;
		Timestamp startTimestamp(true);

		do
		{
			Matrix s, w;

			performance.start();
				const bool success = matrix.nonNegativeMatrixFactorization(s, w, components, 100u);
			performance.stop();

			if (success)
			{
				const Matrix result = s * w;
				const Matrix diff = matrix - result;

				ocean_assert(diff.elements() != 0);
				const Scalar error = diff.norm() / Scalar(diff.elements());

				sumError += error;
				validIterations++;
			}
			else
			{
				allSucceeded = false;
			}

			iterations++;
		}
		while (startTimestamp + (testDuration / maxComponents) > Timestamp(true));

		ocean_assert_and_suppress_unused(iterations != 0ull, iterations);

		Log::info() << "Performance: " << String::toAString(performance.averageMseconds(), 1u) << "ms";
		// show goodness of factorization
		Log::info() << "Average factorization error ||V - SW|| with " << components << " components: " << Numeric::ratio(sumError, Scalar(validIterations));
	}

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

bool TestMatrix::testMatrixMultiplication(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Matrix multiplication test:\n";

#ifdef OCEAN_USE_GTEST
	const Indices32 sizes = {10u, 20u, 50u, 97u, 203u};
#else
	const Indices32 sizes = {10u, 20u, 50u, 100u, 200u, 500u, 1000u, 2000u};
#endif

	Log::info() << "Performance test for matrix multiplication";

	for (const unsigned int size : sizes)
	{
		const Timestamp startTimestamp(true);

		HighPerformanceStatistic performance;

		do
		{
			Matrix a(size, size);
			Matrix b(size, size);
			Matrix c(size, size);

			for (unsigned int n = 0u; n < a.elements(); ++n)
			{
				a(n) = Random::scalar(-100, 100);
			}

			for (unsigned int n = 0u; n < b.elements(); ++n)
			{
				b(n) = Random::scalar(-100, 100);
			}

			performance.start();
				c = a * b;
			performance.stop();
		}
		while (startTimestamp + testDuration > Timestamp(true));

		Log::info() << "Performance (" << size << "^2): " << String::toAString(performance.averageMseconds(), 3u) << "ms";
	}

	Log::info() << " ";

	Log::info() << "Performance test for matrix multiplication and assignment";

	for (const unsigned int size : sizes)
	{
		const Timestamp startTimestamp(true);

		HighPerformanceStatistic performance;

		do
		{
			Matrix a(size, size);
			Matrix b(size, size);

			for (unsigned int n = 0u; n < a.elements(); ++n)
			{
				a(n) = Random::scalar(-100, 100);
			}

			for (unsigned int n = 0u; n < b.elements(); ++n)
			{
				b(n) = Random::scalar(-100, 100);
			}

			performance.start();
				a *= b;
			performance.stop();

		}
		while (startTimestamp + testDuration > Timestamp(true));

		Log::info() << "Performance (" << size << "^2): " << String::toAString(performance.averageMseconds(), 3u) << "ms";
	}

	Log::info() << " ";

	constexpr unsigned int size = 100u;

	bool allSucceeded = true;

	Timestamp startTimestamp(true);

	do
	{
		Matrix a(size, size);
		Matrix b(size, size);

		for (unsigned int n = 0u; n < a.elements(); ++n)
		{
			a(n) = Random::scalar(-1, 1);
		}

		for (unsigned int n = 0u; n < b.elements(); ++n)
		{
			b(n) = Random::scalar(-1, 1);
		}

		Matrix c(size, size);
		c = a * b;

		allSucceeded = validateMatrixMultiplication(a, b, c) && allSucceeded;

	}
	while (startTimestamp + testDuration > Timestamp(true));

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

bool TestMatrix::validateMatrixMultiplication(const Matrix& left, const Matrix& right, const Matrix& result)
{
	// Validation with naive matrix multiplication
	const Scalar eps = Numeric::weakEps();
	Matrix check(left.rows(), right.columns());

	for (size_t r = 0; r < result.rows(); r++)
	{
		for (size_t c = 0; c < result.columns(); c++)
		{
			Scalar element = 0;

			for (size_t i = 0; i < left.columns(); i++)
			{
				element += left(r, i) * right(i, c);
			}

			check(r, c) = element;
		}
	}

	return check.isEqual(result, eps);
}

}

}

}
