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

	allSucceeded = testNonNegativeMatrixFactorization(testDuration) && allSucceeded;

	Log::info() << " ";

	Log::info() << " ";

	if (allSucceeded)
		Log::info() << "Sparse Matrix test suceeded.";
	else
		Log::info() << "Sparse Matrix test FAILED!";

	return allSucceeded;
}
	
#ifdef OCEAN_USE_GTEST
	
TEST(TestSparseMatrix, Rank) {
	EXPECT_TRUE(TestSparseMatrix::testRank());
}

TEST(TestSparseMatrix, NonNegativeMatrixFactorization) {
	EXPECT_TRUE(TestSparseMatrix::testNonNegativeMatrixFactorization(GTEST_TEST_DURATION));
}
	
#endif // OCEAN_USE_GTEST

bool TestSparseMatrix::testRank()
{
	Log::info() << "Rank test:";

	bool allSucceeded = true;

	{
		const SparseMatrix zeroMatrix1(1, 1, 0);
		if (zeroMatrix1.rank() != 0u)
			allSucceeded = false;

		const SparseMatrix zeroMatrix3(3, 3, 0);
		if (zeroMatrix3.rank() != 0u)
			allSucceeded = false;

		const SparseMatrix zeroMatrix7(7, 7, 0);
		if (zeroMatrix7.rank() != 0u)
			allSucceeded = false;
	}

	{
		const Scalar data[3] = { 1, 1, 1 };
		SparseMatrix identityMatrix3(3, 3, Matrix(3, 1, data));
		if (identityMatrix3.rank() != 3u)
			allSucceeded = false;
	}

	{
		const Scalar data[7] = { 1, 1, 1, 1, 1, 1, 1 };
		const SparseMatrix identityMatrix7(7, 7, Matrix(7, 1, data));
		if (identityMatrix7.rank() != 7u)
			allSucceeded = false;
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
		if (matrix.rank() != 3u)
			allSucceeded = false;
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
		if (matrix.rank() != 2u)
			allSucceeded = false;
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
		if (matrix.rank() != 2u)
			allSucceeded = false;
	}

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

bool TestSparseMatrix::testNonNegativeMatrixFactorization(const double testDuration, const unsigned int components)
{
	Log::info() << "Non-negative matrix factorization test with " << components << " components:";

	bool allSucceeded = true;

	HighPerformanceStatistic performance;

	Timestamp startTimestamp(true);

	do
	{
		SparseMatrix matrix(310, 212);
		for (size_t row = 0; row < matrix.rows(); row += RandomI::random(1u, 3u))
		{
			for (size_t col = 0; col < matrix.columns(); col += RandomI::random(1u, 3u))
			{
				matrix(row, col) = Scalar(row * col + 1u);
			}
		}

#ifdef OCEAN_DEBUG
		Matrix denseDebug = matrix.denseMatrix();
#endif

		Matrix s, w;
		performance.start();
		bool success = matrix.nonNegativeMatrixFactorization(s, w, components, 100u);
		performance.stop();
		if (!success)
		{
			allSucceeded = false;
		}
		else
		{
#ifdef OCEAN_DEBUG
			Matrix resultDebug = s * w;
#endif
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

//	Log::info() << "Performance for " << matrix.rows()*matrix.columns() << " ("<<  matrix.nonZeroElements() << " non-zero) elements: " << String::toAString(performance.averageMseconds(), 1u) << "ms";
	Log::info() << "Performance: " << String::toAString(performance.averageMseconds(), 1u) << "ms";

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;

}

}

}

}
