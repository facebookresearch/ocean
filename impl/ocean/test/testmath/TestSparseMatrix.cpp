/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestSparseMatrix.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/SparseMatrix.h"
#include "ocean/math/Random.h"

#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestSparseMatrix::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---  Sparse Matrix test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testRank() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNonNegativeMatrixFactorization(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testInvertDiagonal(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Sparse Matrix test succeeded.";
	}
	else
	{
		Log::info() << "Sparse Matrix test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestSparseMatrix, Rank)
{
	EXPECT_TRUE(TestSparseMatrix::testRank());
}

TEST(TestSparseMatrix, NonNegativeMatrixFactorization)
{
	EXPECT_TRUE(TestSparseMatrix::testNonNegativeMatrixFactorization(GTEST_TEST_DURATION));
}

TEST(TestSparseMatrix, InvertDiagonal)
{
	EXPECT_TRUE(TestSparseMatrix::testInvertDiagonal(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestSparseMatrix::testRank()
{
	Log::info() << "Rank test:";

	Validation validation;

	{
		const SparseMatrix zeroMatrix1(1, 1, 0);
		OCEAN_EXPECT_EQUAL(validation, zeroMatrix1.rank(), size_t(0));

		const SparseMatrix zeroMatrix3(3, 3, 0);
		OCEAN_EXPECT_EQUAL(validation, zeroMatrix3.rank(), size_t(0));

		const SparseMatrix zeroMatrix7(7, 7, 0);
		OCEAN_EXPECT_EQUAL(validation, zeroMatrix7.rank(), size_t(0));
	}

	{
		const Scalar data[3] = {1, 1, 1};

		const SparseMatrix identityMatrix3(3, 3, Matrix(3, 1, data));

		OCEAN_EXPECT_EQUAL(validation, identityMatrix3.rank(), size_t(3));
	}

	{
		const Scalar data[7] = {1, 1, 1, 1, 1, 1, 1};

		const SparseMatrix identityMatrix7(7, 7, Matrix(7, 1, data));

		OCEAN_EXPECT_EQUAL(validation, identityMatrix7.rank(), size_t(7));
	}

	{
		const Scalar data[9] = {1, 2, 3, 0, 5, 4, 0, 10, 2};

		SparseMatrix matrix(3, 3);
		for (unsigned int i = 0; i < 9; i++)
		{
			unsigned int x = i % 3;
			unsigned int y = i / 3;
			matrix(y, x) = data[i];
		}

		OCEAN_EXPECT_EQUAL(validation, matrix.rank(), size_t(3));
	}

	{
		const Scalar data[9] = {1, 2, 3, 0, 6, 4, 0, 3, 2};

		SparseMatrix matrix(3, 3);
		for (unsigned int i = 0; i < 9; i++)
		{
			unsigned int x = i % 3;
			unsigned int y = i / 3;
			matrix(y, x) = data[i];
		}

		OCEAN_EXPECT_EQUAL(validation, matrix.rank(), size_t(2));
	}

	{
		const Scalar data[6] = {2, 3, 0, 1, 4, -1};

		SparseMatrix matrix(3, 2);
		for (unsigned int i = 0; i < 6; i++)
		{
			unsigned int x = i % 2;
			unsigned int y = i / 2;
			matrix(y, x) = data[i];
		}

		OCEAN_EXPECT_EQUAL(validation, matrix.rank(), size_t(2));
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestSparseMatrix::testNonNegativeMatrixFactorization(const double testDuration, const unsigned int components)
{
	Log::info() << "Non-negative matrix factorization test with " << components << " components:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	HighPerformanceStatistic performance;

	Timestamp startTimestamp(true);

	do
	{
		SparseMatrix matrix(310, 212);
		for (size_t row = 0; row < matrix.rows(); row += RandomI::random(randomGenerator, 1u, 3u))
		{
			for (size_t col = 0; col < matrix.columns(); col += RandomI::random(randomGenerator, 1u, 3u))
			{
				matrix(row, col) = Scalar(row * col + 1u);
			}
		}

#ifdef OCEAN_DEBUG
		Matrix denseDebug = matrix.denseMatrix();
#endif

		performance.start();
			Matrix s;
			Matrix w;
			const bool success = matrix.nonNegativeMatrixFactorization(s, w, components, 100u);
		performance.stop();

		OCEAN_EXPECT_TRUE(validation, success);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestSparseMatrix::testInvertDiagonal(const double testDuration)
{
	Log::info() << "Invert diagonal test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	Timestamp startTimestamp(true);

	do
	{
		const size_t dimension = RandomI::random(1, 100);

		SparseMatrix sparseMatrix(dimension, dimension);
		Matrix matrix(dimension, dimension, false);

		for (size_t n = 0; n < dimension; ++n)
		{
			Scalar value = Random::scalar(randomGenerator, Scalar(0.001), Scalar(10));
			value *= Random::sign(randomGenerator);

			sparseMatrix(n, n) = value;
			matrix(n, n) = value;
		}

		const Matrix invertedMatrix = matrix.inverted();

		const bool result = sparseMatrix.invertDiagonal();
		OCEAN_EXPECT_TRUE(validation, result);

		for (size_t row = 0; row < matrix.rows(); ++row)
		{
			for (size_t column = 0; column < matrix.columns(); ++column)
			{
				if (sparseMatrix.isZero(row, column))
				{
					OCEAN_EXPECT_TRUE(validation, Numeric::isWeakEqualEps(invertedMatrix(row, column)));
				}
				else
				{
					const Scalar value = matrix(row, column);
					const Scalar invertedValue = Scalar(1) / value;

					const Scalar sparseValue = sparseMatrix(row, column);
					const Scalar matrixValue = invertedMatrix(row, column);

					OCEAN_EXPECT_TRUE(validation, Numeric::isWeakEqual(sparseValue, invertedValue));
					OCEAN_EXPECT_TRUE(validation, Numeric::isWeakEqual(sparseValue, matrixValue));
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
