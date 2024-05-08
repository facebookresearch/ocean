/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestStaticMatrix.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Matrix.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/Random.h"
#include "ocean/math/StaticMatrix.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestStaticMatrix::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Static Matrix test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testConstructorIdentity() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConstructorData() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testTranspose(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMatrixAdd(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMatrixAddTransposed(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testScalarMultiplication(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testVectorMultiplication<float>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testVectorMultiplication<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMatrixMultiplication(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMatrixMultiplicationTransposedLeft(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMatrixMultiplicationTransposedRight(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSolveCholesky(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
		Log::info() << "Static Matrix test succeeded.";
	else
		Log::info() << "Static Matrix test FAILED!";

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestStaticMatrix, ConstructorIdentity)
{
	EXPECT_TRUE(TestStaticMatrix::testConstructorIdentity());
}

TEST(TestStaticMatrix, ConstructorData)
{
	EXPECT_TRUE(TestStaticMatrix::testConstructorData());
}

TEST(TestStaticMatrix, Transpose)
{
	EXPECT_TRUE(TestStaticMatrix::testTranspose(GTEST_TEST_DURATION));
}

TEST(TestStaticMatrix, MatrixAdd)
{
	EXPECT_TRUE(TestStaticMatrix::testMatrixAdd(GTEST_TEST_DURATION));
}

TEST(TestStaticMatrix, MatrixAddTransposed)
{
	EXPECT_TRUE(TestStaticMatrix::testMatrixAddTransposed(GTEST_TEST_DURATION));
}

TEST(TestStaticMatrix, ScalarMultiplication)
{
	EXPECT_TRUE(TestStaticMatrix::testScalarMultiplication(GTEST_TEST_DURATION));
}

TEST(TestStaticMatrix, VectorMultiplication32)
{
	EXPECT_TRUE(TestStaticMatrix::testVectorMultiplication<float>(GTEST_TEST_DURATION));
}

TEST(TestStaticMatrix, VectorMultiplication64)
{
	EXPECT_TRUE(TestStaticMatrix::testVectorMultiplication<double>(GTEST_TEST_DURATION));
}

TEST(TestStaticMatrix, MatrixMultiplication)
{
	EXPECT_TRUE(TestStaticMatrix::testMatrixMultiplication(GTEST_TEST_DURATION));
}

TEST(TestStaticMatrix, MatrixMultiplicationTransposedLeft)
{
	EXPECT_TRUE(TestStaticMatrix::testMatrixMultiplicationTransposedLeft(GTEST_TEST_DURATION));
}

TEST(TestStaticMatrix, MatrixMultiplicationTransposedRight)
{
	EXPECT_TRUE(TestStaticMatrix::testMatrixMultiplicationTransposedRight(GTEST_TEST_DURATION));
}

TEST(TestStaticMatrix, SolveCholesky)
{
	EXPECT_TRUE(TestStaticMatrix::testSolveCholesky(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestStaticMatrix::testConstructorIdentity()
{
	Log::info() << "Identity Constructor test:";

	bool allSucceeded = true;

	allSucceeded = isIdentityMatrix(StaticMatrix<float, 1, 1>(true), 1, 1) && allSucceeded;
	allSucceeded = !isIdentityMatrix(StaticMatrix<float, 1, 1>(false), 1, 1) && allSucceeded;
	allSucceeded = !isNullMatrix(StaticMatrix<float, 1, 1>(true), 1, 1) && allSucceeded;
	allSucceeded = isNullMatrix(StaticMatrix<float, 1, 1>(false), 1, 1) && allSucceeded;

	allSucceeded = isIdentityMatrix(StaticMatrix<double, 1, 1>(true), 1, 1) && allSucceeded;
	allSucceeded = !isIdentityMatrix(StaticMatrix<double, 1, 1>(false), 1, 1) && allSucceeded;
	allSucceeded = !isNullMatrix(StaticMatrix<double, 1, 1>(true), 1, 1) && allSucceeded;
	allSucceeded = isNullMatrix(StaticMatrix<double, 1, 1>(false), 1, 1) && allSucceeded;


	allSucceeded = isIdentityMatrix(StaticMatrix<float, 1, 2>(true), 1, 2) && allSucceeded;
	allSucceeded = !isIdentityMatrix(StaticMatrix<float, 1, 2>(false), 1, 2) && allSucceeded;
	allSucceeded = !isNullMatrix(StaticMatrix<float, 1, 2>(true), 1, 2) && allSucceeded;
	allSucceeded = isNullMatrix(StaticMatrix<float, 1, 2>(false), 1, 2) && allSucceeded;

	allSucceeded = isIdentityMatrix(StaticMatrix<double, 1, 2>(true), 1, 2) && allSucceeded;
	allSucceeded = !isIdentityMatrix(StaticMatrix<double, 1, 2>(false), 1, 2) && allSucceeded;
	allSucceeded = !isNullMatrix(StaticMatrix<double, 1, 2>(true), 1, 2) && allSucceeded;
	allSucceeded = isNullMatrix(StaticMatrix<double, 1, 2>(false), 1, 2) && allSucceeded;


	allSucceeded = isIdentityMatrix(StaticMatrix<float, 2, 1>(true), 2, 1) && allSucceeded;
	allSucceeded = !isIdentityMatrix(StaticMatrix<float, 2, 1>(false), 2, 1) && allSucceeded;
	allSucceeded = !isNullMatrix(StaticMatrix<float, 2, 1>(true), 2, 1) && allSucceeded;
	allSucceeded = isNullMatrix(StaticMatrix<float, 2, 1>(false), 2, 1) && allSucceeded;

	allSucceeded = isIdentityMatrix(StaticMatrix<double, 2, 1>(true), 2, 1) && allSucceeded;
	allSucceeded = !isIdentityMatrix(StaticMatrix<double, 2, 1>(false), 2, 1) && allSucceeded;
	allSucceeded = !isNullMatrix(StaticMatrix<double, 2, 1>(true), 2, 1) && allSucceeded;
	allSucceeded = isNullMatrix(StaticMatrix<double, 2, 1>(false), 2, 1) && allSucceeded;


	allSucceeded = isIdentityMatrix(StaticMatrix<float, 5, 5>(true), 5, 5) && allSucceeded;
	allSucceeded = !isIdentityMatrix(StaticMatrix<float, 5, 5>(false), 5, 5) && allSucceeded;
	allSucceeded = !isNullMatrix(StaticMatrix<float, 5, 5>(true), 5, 5) && allSucceeded;
	allSucceeded = isNullMatrix(StaticMatrix<float, 5, 5>(false), 5, 5) && allSucceeded;

	allSucceeded = isIdentityMatrix(StaticMatrix<double, 5, 5>(true), 5, 5) && allSucceeded;
	allSucceeded = !isIdentityMatrix(StaticMatrix<double, 5, 5>(false), 5, 5) && allSucceeded;
	allSucceeded = !isNullMatrix(StaticMatrix<double, 5, 5>(true), 5, 5) && allSucceeded;
	allSucceeded = isNullMatrix(StaticMatrix<double, 5, 5>(false), 5, 5) && allSucceeded;


	allSucceeded = isIdentityMatrix(StaticMatrix<float, 31, 19>(true), 31, 19) && allSucceeded;
	allSucceeded = !isIdentityMatrix(StaticMatrix<float, 31, 19>(false), 31, 19) && allSucceeded;
	allSucceeded = !isNullMatrix(StaticMatrix<float, 31, 19>(true), 31, 19) && allSucceeded;
	allSucceeded = isNullMatrix(StaticMatrix<float, 31, 19>(false), 31, 19) && allSucceeded;

	allSucceeded = isIdentityMatrix(StaticMatrix<double, 31, 19>(true), 31, 19) && allSucceeded;
	allSucceeded = !isIdentityMatrix(StaticMatrix<double, 31, 19>(false), 31, 19) && allSucceeded;
	allSucceeded = !isNullMatrix(StaticMatrix<double, 31, 19>(true), 31, 19) && allSucceeded;
	allSucceeded = isNullMatrix(StaticMatrix<double, 31, 19>(false), 31, 19) && allSucceeded;


	allSucceeded = isIdentityMatrix(StaticMatrix<float, 24, 18>(true), 24, 18) && allSucceeded;
	allSucceeded = !isIdentityMatrix(StaticMatrix<float, 24, 18>(false), 24, 18) && allSucceeded;
	allSucceeded = !isNullMatrix(StaticMatrix<float, 24, 18>(true), 24, 18) && allSucceeded;
	allSucceeded = isNullMatrix(StaticMatrix<float, 24, 18>(false), 24, 18) && allSucceeded;

	allSucceeded = isIdentityMatrix(StaticMatrix<double, 24, 18>(true), 24, 18) && allSucceeded;
	allSucceeded = !isIdentityMatrix(StaticMatrix<double, 24, 18>(false), 24, 18) && allSucceeded;
	allSucceeded = !isNullMatrix(StaticMatrix<double, 24, 18>(true), 24, 18) && allSucceeded;
	allSucceeded = isNullMatrix(StaticMatrix<double, 24, 18>(false), 24, 18) && allSucceeded;


	allSucceeded = isIdentityMatrix(StaticMatrix<float, 16, 9>(true), 16, 9) && allSucceeded;
	allSucceeded = !isIdentityMatrix(StaticMatrix<float, 16, 9>(false), 16, 9) && allSucceeded;
	allSucceeded = !isNullMatrix(StaticMatrix<float, 16, 9>(true), 16, 9) && allSucceeded;
	allSucceeded = isNullMatrix(StaticMatrix<float, 16, 9>(false), 16, 9) && allSucceeded;

	allSucceeded = isIdentityMatrix(StaticMatrix<double, 16, 9>(true), 16, 9) && allSucceeded;
	allSucceeded = !isIdentityMatrix(StaticMatrix<double, 16, 9>(false), 16, 9) && allSucceeded;
	allSucceeded = !isNullMatrix(StaticMatrix<double, 16, 9>(true), 16, 9) && allSucceeded;
	allSucceeded = isNullMatrix(StaticMatrix<double, 16, 9>(false), 16, 9) && allSucceeded;


	allSucceeded = isIdentityMatrix(StaticMatrix<float, 7, 22>(true), 7, 22) && allSucceeded;
	allSucceeded = !isIdentityMatrix(StaticMatrix<float, 7, 22>(false), 7, 22) && allSucceeded;
	allSucceeded = !isNullMatrix(StaticMatrix<float, 7, 22>(true), 7, 22) && allSucceeded;
	allSucceeded = isNullMatrix(StaticMatrix<float, 7, 22>(false), 7, 22) && allSucceeded;

	allSucceeded = isIdentityMatrix(StaticMatrix<double, 7, 22>(true), 7, 22) && allSucceeded;
	allSucceeded = !isIdentityMatrix(StaticMatrix<double, 7, 22>(false), 7, 22) && allSucceeded;
	allSucceeded = !isNullMatrix(StaticMatrix<double, 7, 22>(true), 7, 22) && allSucceeded;
	allSucceeded = isNullMatrix(StaticMatrix<double, 7, 22>(false), 7, 22) && allSucceeded;

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

bool TestStaticMatrix::testConstructorData()
{
	Log::info() << "Data constructor test:";

	bool allSucceeded = true;

	allSucceeded = testConstructorData<1, 1>() && allSucceeded;
	allSucceeded = testConstructorData<1, 2>() && allSucceeded;
	allSucceeded = testConstructorData<2, 1>() && allSucceeded;
	allSucceeded = testConstructorData<2, 2>() && allSucceeded;
	allSucceeded = testConstructorData<5, 5>() && allSucceeded;
	allSucceeded = testConstructorData<12, 12>() && allSucceeded;
	allSucceeded = testConstructorData<12, 13>() && allSucceeded;
	allSucceeded = testConstructorData<13, 12>() && allSucceeded;
	allSucceeded = testConstructorData<5, 10>() && allSucceeded;
	allSucceeded = testConstructorData<31, 31>() && allSucceeded;

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

template <size_t tRows, size_t tColumns>
bool TestStaticMatrix::testConstructorData()
{
	constexpr size_t tElements = tRows * tColumns;

	static_assert(tElements >= 1, "Invalid matrix size");

	float data32[tElements];
	double data64[tElements];

	for (size_t n = 0; n < tElements; ++n)
	{
		data32[n] = RandomF::scalar(-1000.0f, 1000.0f);
		data64[n] = RandomF::scalar(-1000.0, 1000.0);
	}

	StaticMatrix<float, tRows, tColumns> matrix32(data32);
	StaticMatrix<float, tRows, tColumns> matrixAligned32(data32, true);
	StaticMatrix<float, tRows, tColumns> matrixNotAligned32(data32, false);

	StaticMatrix<double, tRows, tColumns> matrix64(data64);
	StaticMatrix<double, tRows, tColumns> matrixAligned64(data64, true);
	StaticMatrix<double, tRows, tColumns> matrixNotAligned64(data64, false);

	for (size_t r = 0; r < tRows; ++r)
	{
		for (size_t c = 0; c < tColumns; ++c)
		{
			const size_t index = r * tColumns + c;

			if (matrix32.data()[index] != data32[index] || matrix32(r, c) != data32[index])
			{
				return false;
			}

			if (matrixAligned32.data()[index] != data32[index] || matrixAligned32(r, c) != data32[index])
			{
				return false;
			}

			if (matrix64.data()[index] != data64[index] || matrix64(r, c) != data64[index])
			{
				return false;
			}

			if (matrixAligned64.data()[index] != data64[index] || matrixAligned64(r, c) != data64[index])
			{
				return false;
			}
		}
	}

	const float* pointer32 = data32;
	const double* pointer64 = data64;

	for (size_t c = 0; c < tColumns; ++c)
	{
		for (size_t r = 0; r < tRows; ++r)
		{
			const size_t index = r * tColumns + c;

			if (matrixNotAligned32.data()[index] != *pointer32 || matrixNotAligned32(r, c) != *pointer32)
			{
				return false;
			}

			if (matrixNotAligned64.data()[index] != *pointer64 || matrixNotAligned64(r, c) != *pointer64)
			{
				return false;
			}

			pointer32++;
			pointer64++;
		}
	}

	return true;
}

bool TestStaticMatrix::testTranspose(const double testDuration)
{
	Log::info() << "Transpose test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			allSucceeded = testTranspose<1, 1>() && allSucceeded;
			allSucceeded = testTranspose<1, 2>() && allSucceeded;
			allSucceeded = testTranspose<2, 1>() && allSucceeded;
			allSucceeded = testTranspose<5, 5>() && allSucceeded;
			allSucceeded = testTranspose<9, 10>() && allSucceeded;
			allSucceeded = testTranspose<10, 9>() && allSucceeded;
			allSucceeded = testTranspose<1, 11>() && allSucceeded;
			allSucceeded = testTranspose<11, 1>() && allSucceeded;
			allSucceeded = testTranspose<4, 16>() && allSucceeded;
			allSucceeded = testTranspose<25, 25>() && allSucceeded;
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

template <size_t tRows, size_t tColumns>
bool TestStaticMatrix::testTranspose()
{
	StaticMatrix<Scalar, tRows, tColumns> matrix;

	for (size_t n = 0; n < matrix.elements(); ++n)
	{
		matrix.data()[n] = Random::scalar(-1000, 1000);
	}

	StaticMatrix<Scalar, tColumns, tRows> transposed = matrix.transposed();

	StaticMatrix<Scalar, tColumns, tRows> transposedParameter;
	matrix.transposed(transposedParameter);

	for (size_t r = 0; r < tRows; ++r)
	{
		for (size_t c = 0; c < tColumns; ++c)
		{
			if (matrix.data()[r * tColumns + c] != transposed.data()[c * tRows + r] || matrix(r, c) != transposed(c, r))
			{
				return false;
			}

			if (matrix.data()[r * tColumns + c] != transposedParameter.data()[c * tRows + r] || matrix(r, c) != transposedParameter(c, r))
			{
				return false;
			}
		}
	}

	return true;
}

bool TestStaticMatrix::testMatrixAdd(const double testDuration)
{
	Log::info() << "Add operator test:";

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			if (testMatrixAdd<1, 1>())
				validIterations++;

			iterations++;

			if ( testMatrixAdd<1, 2>())
				validIterations++;

			iterations++;

			if (testMatrixAdd<2, 1>())
				validIterations++;

			iterations++;

			if (testMatrixAdd<5, 5>())
				validIterations++;

			iterations++;

			if (testMatrixAdd<9, 10>())
				validIterations++;

			iterations++;

			if (testMatrixAdd<10, 9>())
				validIterations++;

			iterations++;

			if (testMatrixAdd<1, 11>())
				validIterations++;

			iterations++;

			if (testMatrixAdd<11, 1>())
				validIterations++;

			iterations++;

			if (testMatrixAdd<4, 16>())
				validIterations++;

			iterations++;

			if (testMatrixAdd<25, 25>())
				validIterations++;

			iterations++;

		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

template <size_t tRows, size_t tColumns>
bool TestStaticMatrix::testMatrixAdd()
{
	StaticMatrix<Scalar, tRows, tColumns> matrix;

	for (size_t n = 0; n < matrix.elements(); ++n)
	{
		matrix.data()[n] = Random::scalar(-1000, 1000);
	}

	StaticMatrix<Scalar, tRows, tColumns> test0(false);
	matrix.add(test0);
	matrix.add(test0);

	StaticMatrix<Scalar, tRows, tColumns> test1(matrix + matrix);

	StaticMatrix<Scalar, tRows, tColumns> test2(false);
	test2 += matrix;
	test2 += matrix;

	for (size_t r = 0; r < tRows; ++r)
	{
		for (size_t c = 0; c < tColumns; ++c)
		{
			if (Numeric::isNotEqual(test0(r, c) * Scalar(0.5), matrix(r, c)) || Numeric::isNotEqual(test0(r, c), test1(r, c)) || Numeric::isNotEqual(test0(r, c), test2(r, c)))
			{
				return false;
			}
		}
	}

	return true;
}

bool TestStaticMatrix::testMatrixAddTransposed(const double testDuration)
{
	Log::info() << "Transposed add test:";

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			if (testMatrixAddTransposed<1, 1>())
				validIterations++;

			iterations++;

			if (testMatrixAddTransposed<1, 2>())
				validIterations++;

			iterations++;

			if (testMatrixAddTransposed<2, 1>())
				validIterations++;

			iterations++;

			if (testMatrixAddTransposed<5, 5>())
				validIterations++;

			iterations++;

			if (testMatrixAddTransposed<5, 5>())
				validIterations++;

			iterations++;

			if (testMatrixAddTransposed<1, 11>())
				validIterations++;

			iterations++;

			if (testMatrixAddTransposed<11, 1>())
				validIterations++;

			iterations++;

			if (testMatrixAddTransposed<4, 16>())
				validIterations++;

			iterations++;

			if (testMatrixAddTransposed<25, 25>())
				validIterations++;

			iterations++;

			if (testMatrixAddTransposed<2, 113>())
				validIterations++;

			iterations++;

			if (testMatrixAddTransposed<3, 82>())
				validIterations++;

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

template <size_t tRows, size_t tColumns>
bool TestStaticMatrix::testMatrixAddTransposed()
{
	StaticMatrix<Scalar, tRows, tColumns> matrix;

	for (size_t n = 0; n < matrix.elements(); ++n)
	{
		matrix.data()[n] = Random::scalar(-1000, 1000);
	}

	StaticMatrix<Scalar, tColumns, tRows> test0(false);
	matrix.addTransposed(test0);
	matrix.addTransposed(test0);

	StaticMatrix<Scalar, tColumns, tRows> test1(matrix.transposed() + matrix.transposed());

	for (size_t r = 0; r < tColumns; ++r)
	{
		for (size_t c = 0; c < tRows; ++c)
		{
			if (Numeric::isNotEqual(test0(r, c) * Scalar(0.5), matrix(c, r)) || Numeric::isNotEqual(test0(r, c), test1(r, c)))
			{
				return false;
			}
		}
	}

	return true;
}

bool TestStaticMatrix::testScalarMultiplication(const double testDuration)
{
	Log::info() << "Scalar multiplication test:";

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			if (testScalarMultiplication<1, 1>())
				validIterations++;

			iterations++;

			if (testScalarMultiplication<1, 2>())
				validIterations++;

			iterations++;

			if (testScalarMultiplication<2, 1>())
				validIterations++;

			iterations++;

			if (testScalarMultiplication<5, 5>())
				validIterations++;

			iterations++;

			if (testScalarMultiplication<1, 11>())
				validIterations++;

			iterations++;

			if (testScalarMultiplication<11, 1>())
				validIterations++;

			iterations++;

			if (testScalarMultiplication<4, 16>())
				validIterations++;

			iterations++;

			if (testScalarMultiplication<25, 25>())
				validIterations++;

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

template <size_t tRows, size_t tColumns>
bool TestStaticMatrix::testScalarMultiplication()
{
	StaticMatrix<Scalar, tRows, tColumns> matrix;

	for (size_t n = 0; n < matrix.elements(); ++n)
	{
		matrix.data()[n] = Random::scalar(-1000, 1000);
	}

	const StaticMatrix<Scalar, tRows, tColumns> copy(matrix);

	const Scalar scalar = Random::scalar(-1000, 1000);

	const StaticMatrix<Scalar, tRows, tColumns> matrix2 = matrix * scalar;
	matrix *= scalar;

	for (size_t r = 0; r < tRows; ++r)
	{
		for (size_t c = 0; c < tColumns; ++c)
		{
			if (matrix.data()[r * tColumns + c] != copy.data()[r * tColumns + c] * scalar || matrix(r, c) != copy(r, c) * scalar)
			{
				return false;
			}

			if (matrix2.data()[r * tColumns + c] != copy.data()[r * tColumns + c] * scalar || matrix2(r, c) != copy(r, c) * scalar)
			{
				return false;
			}
		}
	}

	return true;
}

template <typename T>
bool TestStaticMatrix::testVectorMultiplication(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Vector multiplication test for " << sizeof(T) * 8 << "bit floating point values:";

	bool allSucceeded = true;

	allSucceeded = testVectorMultiplication<T, 3>(testDuration) && allSucceeded;
	allSucceeded = testVectorMultiplication<T, 4>(testDuration) && allSucceeded;
	allSucceeded = testVectorMultiplication<T, 5>(testDuration) && allSucceeded;

	return allSucceeded;
}

template <typename T, size_t tSize>
bool TestStaticMatrix::testVectorMultiplication(const double testDuration)
{
	Log::info() << "... with " << tSize << "x" << tSize << " matrix:";

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const unsigned int constNumber = 100000u;

	std::vector< StaticMatrix<T, tSize, 1> > vectors(constNumber);
	std::vector< StaticMatrix<T, tSize, 1> > results(constNumber);

	StaticMatrix<T, tSize, tSize> matrix;

	HighPerformanceStatistic performance;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const T range = std::is_same<T, double>::value ? T(100) : T(10);

		for (size_t n = 0; n < matrix.elements(); ++n)
		{
			matrix[n] = RandomT<T>::scalar(randomGenerator, -range, range);
		}

		for (size_t n = 0; n < vectors.size(); ++n)
		{
			for (size_t i = 0; i < tSize; ++i)
			{
				vectors[n][i] = RandomT<T>::scalar(randomGenerator, -range, range);
			}
		}

		{
			const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performance);

			for (unsigned int n = 0u; n < constNumber; ++n)
			{
				results[n] = matrix * vectors[n];
			}
		}

		const MatrixT<T> testMatrix(tSize, tSize, matrix.data());

		for (unsigned int n = 0u; n < constNumber; ++n)
		{
			const MatrixT<T> result(testMatrix * MatrixT<T>(tSize, 1, vectors[n].data()));

			bool localSucceeded = true;

			for (size_t i = 0; i < tSize; ++i)
			{
				if (NumericT<T>::isNotEqual(results[n][i], result(i), NumericT<T>::eps() * (std::is_same<T, double>::value ? T(10) : T(100))))
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
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Performance for " << String::insertCharacter(String::toAString(constNumber), ',', 3, false) << " repetitions: " << String::toAString(performance.averageMseconds()) << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestStaticMatrix::testMatrixMultiplication(const double testDuration)
{
	Log::info() << "Matrix multiplication test:";

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			if (testMatrixMultiplication<1, 1, 1>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplication<1, 2, 1>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplication<2, 1, 1>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplication<5, 5, 2>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplication<5, 5, 5>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplication<1, 11, 7>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplication<11, 1, 11>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplication<4, 16, 9>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplication<25, 25, 4>())
				validIterations++;

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

template <size_t tRows, size_t tColumns, size_t tColumns2>
bool TestStaticMatrix::testMatrixMultiplication()
{
	StaticMatrix<Scalar, tRows, tColumns> left;
	StaticMatrix<Scalar, tColumns, tColumns2> right;

	for (size_t n = 0; n < left.elements(); ++n)
	{
		left.data()[n] = Random::scalar(-10, 10);
	}

	for (size_t n = 0; n < right.elements(); ++n)
	{
		right.data()[n] = Random::scalar(-10, 10);
	}

	StaticMatrix<Scalar, tRows, tColumns2> result0 = left * right;
	StaticMatrix<Scalar, tRows, tColumns2> result1(left * right);

	StaticMatrix<Scalar, tRows, tColumns2> result2, result3, result4;
	left.multiply(right, result2);
	left.multiply(right, result3.data());
	left.multiply(right, 0, result4.data());

	const Scalar epsilon = std::is_same<float, Scalar>::value ? Numeric::eps() * 100 : Numeric::eps();

	for (size_t r = 0; r < tRows; ++r)
	{
		for (size_t c = 0; c < tColumns2; ++c)
		{
			Scalar value = 0;

			for (size_t n = 0; n < tColumns; ++n)
				value += left(r, n) * right(n, c);

			if (Numeric::isNotEqual(value, result0(r, c), epsilon))
				return false;

			if (Numeric::isNotEqual(value, result1(r, c), epsilon))
				return false;

			if (Numeric::isNotEqual(value, result2(r, c), epsilon))
				return false;

			if (Numeric::isNotEqual(value, result3(r, c), epsilon))
				return false;

			if (Numeric::isNotEqual(value, result4(r, c), epsilon))
				return false;
		}
	}

	return true;
}

bool TestStaticMatrix::testMatrixMultiplicationTransposedLeft(const double testDuration)
{
	Log::info() << "Left transposed matrix multiplication test:";

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			if (testMatrixMultiplicationTransposedLeft<1, 1>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplicationTransposedLeft<1, 2>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplicationTransposedLeft<2, 1>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplicationTransposedLeft<5, 5>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplicationTransposedLeft<5, 5>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplicationTransposedLeft<1, 11>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplicationTransposedLeft<11, 1>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplicationTransposedLeft<4, 16>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplicationTransposedLeft<25, 25>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplicationTransposedLeft<2, 113>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplicationTransposedLeft<3, 82>())
				validIterations++;

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

template <size_t tRows, size_t tColumns>
bool TestStaticMatrix::testMatrixMultiplicationTransposedLeft()
{
	StaticMatrix<Scalar, tRows, tColumns> matrix;

	for (size_t n = 0; n < matrix.elements(); ++n)
	{
		matrix.data()[n] = Random::scalar(-10, 10);
	}

	const StaticMatrix<Scalar, tColumns, tRows> transposed(matrix.transposed());
	const StaticMatrix<Scalar, tColumns, tColumns> result = transposed * matrix;

	StaticMatrix<Scalar, tColumns, tColumns> test0;
	matrix.multiplyWithTransposedLeft(test0);

	const StaticMatrix<Scalar, tColumns, tColumns> test1 = matrix.multiplyWithTransposedLeft();

	StaticMatrix<Scalar, tColumns, tColumns> test2(false);
	matrix.multiplyWithTransposedLeftAndAdd(test2);
	matrix.multiplyWithTransposedLeftAndAdd(test2);

	const Scalar epsilon = std::is_same<float, Scalar>::value ? Numeric::eps() * 50 : Numeric::eps();

	for (size_t r = 0; r < tColumns; ++r)
	{
		for (size_t c = 0; c < tColumns; ++c)
		{
			if (Numeric::isNotEqual(test0(r, c), result(r, c), epsilon) || Numeric::isNotEqual(test1(r, c), result(r, c), epsilon))
			{
				return false;
			}

			if (Numeric::isNotEqual(test2(r, c) * Scalar(0.5), result(r, c), epsilon))
			{
				return false;
			}
		}
	}

	return true;
}

bool TestStaticMatrix::testMatrixMultiplicationTransposedRight(const double testDuration)
{
	Log::info() << "Right transposed matrix multiplication test:";

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			if (testMatrixMultiplicationTransposedLeft<1, 1>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplicationTransposedLeft<1, 2>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplicationTransposedLeft<2, 1>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplicationTransposedLeft<5, 5>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplicationTransposedLeft<5, 5>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplicationTransposedLeft<1, 11>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplicationTransposedLeft<11, 1>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplicationTransposedLeft<4, 16>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplicationTransposedLeft<25, 25>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplicationTransposedLeft<2, 113>())
				validIterations++;

			iterations++;

			if (testMatrixMultiplicationTransposedLeft<3, 82>())
				validIterations++;

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

template <size_t tRows, size_t tColumns>
bool TestStaticMatrix::testMatrixMultiplicationTransposedRight()
{
	StaticMatrix<Scalar, tRows, tColumns> matrix;

	for (size_t n = 0; n < matrix.elements(); ++n)
	{
		matrix.data()[n] = Random::scalar(-10, 10);
	}

	const StaticMatrix<Scalar, tColumns, tRows> transposed(matrix.transposed());
	const StaticMatrix<Scalar, tRows, tRows> result = matrix * transposed;

	StaticMatrix<Scalar, tRows, tRows> test0;
	matrix.multiplyWithTransposedRight(test0);

	const StaticMatrix<Scalar, tRows, tRows> test1 = matrix.multiplyWithTransposedRight();

	StaticMatrix<Scalar, tColumns, tColumns> test2(false);
	matrix.multiplyWithTransposedRightAndAdd(test2);
	matrix.multiplyWithTransposedRightAndAdd(test2);

	const Scalar epsilon = std::is_same<float, Scalar>::value ? Numeric::eps() * 50 : Numeric::eps();

	for (size_t r = 0; r < tRows; ++r)
	{
		for (size_t c = 0; c < tRows; ++c)
		{
			if (Numeric::isNotEqual(test0(r, c), result(r, c), epsilon) || Numeric::isNotEqual(test1(r, c), result(r, c), epsilon))
			{
				return false;
			}

			if (Numeric::isNotEqual(test2(r, c) * Scalar(0.5), result(r, c), epsilon))
			{
				return false;
			}
		}
	}

	return true;
}

template <typename T>
bool TestStaticMatrix::isIdentityMatrix(const T& matrix, const size_t rows, const size_t columns)
{
	ocean_assert(rows <= matrix.rows() && columns <= matrix.columns());

	for (size_t r = 0; r < rows; ++r)
	{
		for (size_t c = 0; c < columns; ++c)
		{
			const typename T::Type element = matrix.data()[r * matrix.columns() + c];

			if (r == c)
			{
				if (NumericT<typename T::Type>::isNotEqual(element, 1))
				{
					if (matrix.isIdentity())
					{
						return true;
					}

					return false;
				}
			}
			else
			{
				if (NumericT<typename T::Type>::isNotEqual(element, 0))
				{
					if (matrix.isIdentity())
					{
						return true;
					}

					return false;
				}
			}
		}
	}

	if (!matrix.isIdentity())
	{
		return false; // we return the wrong result so that we receive an error
	}

	return true;
}

template <typename T>
bool TestStaticMatrix::isNullMatrix(const T& matrix, const size_t rows, const size_t columns)
{
	ocean_assert(rows <= matrix.rows() && columns <= matrix.columns());

	for (size_t r = 0; r < rows; ++r)
	{
		for (size_t c = 0; c < columns; ++c)
		{
			const typename T::Type element = matrix.data()[r * matrix.columns() + c];

			if (NumericT<typename T::Type>::isNotEqual(element, 0))
			{
				if (matrix.isNull())
				{
					return true;
				}

				return false;
			}
		}
	}

	if (!matrix.isNull())
	{
		return false; // we return the wrong result so that we receive an error
	}

	return true;
}

bool TestStaticMatrix::testSolveCholesky(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test solving symmetric Matrix using cholesky decomposition:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testSolveCholeskyMatrix<4>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testSolveCholeskyMatrix<6>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testSolveCholeskyMatrix<9>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testSolveCholeskyMatrix<100>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
		Log::info() << "Cholesky decomposition test succeeded.";
	else
		Log::info() << "Cholesky decomposition test FAILED!";

	return allSucceeded;
}

template <size_t tSize>
bool TestStaticMatrix::testSolveCholeskyMatrix(double testDuration)
{
	Log::info() << "... with size " << tSize << "x" << tSize << ":";

	// generate random values
	RandomGenerator randomGenerator;

	Matrix matrix(10000, tSize);
	Matrix vectorX(tSize, 1u);

	const Scalar epsilon = Numeric::eps() * 100;

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);

	do
	{
		for (size_t n = 0; n < matrix.elements(); ++n)
		{
			matrix.data()[n] =  Random::scalar(randomGenerator, -10, 10);
		}

		Matrix symmetricMatrix = matrix.transposedMultiply(matrix);

		// generate groundtruth

		for (size_t n = 0; n < vectorX.elements(); ++n)
		{
			vectorX.data()[n] = Random::scalar(randomGenerator, -10, 10);
		}

		Matrix vectorY = symmetricMatrix * vectorX;

		// check
		StaticMatrix<Scalar, tSize, 1> staticMatrixY(vectorY.data());
		StaticMatrix<Scalar, tSize, 1> staticMatrixX(vectorX.data());
		StaticMatrix<Scalar, tSize, tSize> symmetricStaticMatrix(symmetricMatrix.data());

		if (symmetricStaticMatrix.isSymmetric())
		{
			StaticMatrix<Scalar, tSize, 1> staticMatrixSolve;

			performance.start();
			const bool success = symmetricStaticMatrix.solveCholesky(staticMatrixY, staticMatrixSolve);
			performance.stop();

			if (success && staticMatrixX.isEqual(staticMatrixSolve, epsilon))
			{
				++validIterations;
			}
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Performance: " << String::toAString(performance.averageMseconds()) << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

}

}

}
