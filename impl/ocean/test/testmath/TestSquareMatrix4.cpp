/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestSquareMatrix4.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/MathUtilities.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestSquareMatrix4::test(const double testDuration, Worker& worker)
{
	Log::info() << "---   SquareMatrix4 test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testWriteToMessenger() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testElementConstructor(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testVectorMultiplication<float>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testVectorMultiplication<double>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMatrixMultiplication<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testMatrixMultiplication<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testInvert(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMatrixConversion(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testProjectionMatrixFieldOfView<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testProjectionMatrixFieldOfView<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testProjectionMatrixCameraMatrix<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testProjectionMatrixCameraMatrix<double>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "SquareMatrix4 test succeeded.";
	}
	else
	{
		Log::info() << "SquareMatrix4 test FAILED.";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestSquareMatrix4, WriteToMessenger)
{
	EXPECT_TRUE(TestSquareMatrix4::testWriteToMessenger());
}

TEST(TestSquareMatrix4, ElementConstructor)
{
	EXPECT_TRUE(TestSquareMatrix4::testElementConstructor(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix4, VectorMultiplication_Float)
{
	Worker worker;
	EXPECT_TRUE(TestSquareMatrix4::testVectorMultiplication<float>(GTEST_TEST_DURATION, worker));
}

TEST(TestSquareMatrix4, VectorMultiplication_Double)
{
	Worker worker;
	EXPECT_TRUE(TestSquareMatrix4::testVectorMultiplication<double>(GTEST_TEST_DURATION, worker));
}

TEST(TestSquareMatrix4, MatrixMultiplication_Float)
{
	EXPECT_TRUE(TestSquareMatrix4::testMatrixMultiplication<float>(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix4, MatrixMultiplication_Double)
{
	EXPECT_TRUE(TestSquareMatrix4::testMatrixMultiplication<double>(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix4, Invert)
{
	EXPECT_TRUE(TestSquareMatrix4::testInvert(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix4, MatrixConversion)
{
	EXPECT_TRUE(TestSquareMatrix4::testMatrixConversion(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix4, ProjectionMatrixFieldOfView_Float)
{
	EXPECT_TRUE(TestSquareMatrix4::testProjectionMatrixFieldOfView<float>(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix4, ProjectionMatrixFieldOfView_Double)
{
	EXPECT_TRUE(TestSquareMatrix4::testProjectionMatrixFieldOfView<double>(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix4, ProjectionMatrixCameraMatrix_Float)
{
	EXPECT_TRUE(TestSquareMatrix4::testProjectionMatrixCameraMatrix<float>(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix4, ProjectionMatrixCameraMatrix_Double)
{
	EXPECT_TRUE(TestSquareMatrix4::testProjectionMatrixCameraMatrix<double>(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestSquareMatrix4::testWriteToMessenger()
{
	Log::info() << "Write to messenger test:";

	// this is mainly a check whether the code does not compile or crash

	Log::info() << " ";

	Log::info() << SquareMatrix4(true);
	Log::info() << " ";
	Log::info() << "Matrix: " << SquareMatrix4(true);
	Log::info() << " ";
	Log::info() << SquareMatrix4(true) << " <- Matrix";

	Log::info() << " ";
	Log::info() << "Validation succeeded.";

	return true;
}

bool TestSquareMatrix4::testElementConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Element-based constructor test:";

	bool allSucceeded = true;
	RandomGenerator randomGenerator;

	const double epsilon = 0.0001;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			float floatValues[16];
			double doubleValues[16];
			Scalar scalarValues[16];

			for (unsigned int i = 0u; i < 16u; ++i)
			{
				doubleValues[i] = RandomT<double>::scalar(randomGenerator, -100, 100);
				floatValues[i] = float(doubleValues[i]);
				scalarValues[i] = Scalar(doubleValues[i]);
			}

			const SquareMatrixT4<float> floatMatrixA(floatValues);
			const SquareMatrixT4<float> floatMatrixB(floatValues, false);
			const SquareMatrixT4<float> floatMatrixBTransposed(floatValues, true);

			const SquareMatrixT4<float> floatMatrixC(doubleValues);
			const SquareMatrixT4<float> floatMatrixD(doubleValues, false);
			const SquareMatrixT4<float> floatMatrixDTransposed(doubleValues, true);


			const SquareMatrixT4<double> doubleMatrixA(floatValues);
			const SquareMatrixT4<double> doubleMatrixB(floatValues, false);
			const SquareMatrixT4<double> doubleMatrixBTransposed(floatValues, true);

			const SquareMatrixT4<double> doubleMatrixC(doubleValues);
			const SquareMatrixT4<double> doubleMatrixD(doubleValues, false);
			const SquareMatrixT4<double> doubleMatrixDTransposed(doubleValues, true);


			const SquareMatrix4 scalarMatrixA(floatValues);
			const SquareMatrix4 scalarMatrixB(floatValues, false);
			const SquareMatrix4 scalarMatrixBTransposed(floatValues, true);

			const SquareMatrix4 scalarMatrixC(doubleValues);
			const SquareMatrix4 scalarMatrixD(doubleValues, false);
			const SquareMatrix4 scalarMatrixDTransposed(doubleValues, true);


			SquareMatrixT4<float> floatTest, floatTestTransposed;
			SquareMatrixT4<double> doubleTest, doubleTestTransposed;
			SquareMatrix4 scalarTest, scalarTestTransposed;

			unsigned int index = 0u;
			for (unsigned int c = 0u; c < 4u; ++c)
			{
				for (unsigned int r = 0u; r < 4u; ++r)
				{
					floatTest(r, c) = floatValues[index];
					doubleTest(r, c) = doubleValues[index];
					scalarTest(r, c) = scalarValues[index];

					floatTestTransposed(c, r) = floatValues[index];
					doubleTestTransposed(c, r) = doubleValues[index];
					scalarTestTransposed(c, r) = scalarValues[index];

					index++;
				}
			}

			ocean_assert(index == 16u);

			if (!floatMatrixA.isEqual(floatTest, float(epsilon)))
				allSucceeded = false;
			if (!floatMatrixB.isEqual(floatTest, float(epsilon)))
				allSucceeded = false;
			if (!floatMatrixBTransposed.isEqual(floatTestTransposed, float(epsilon)))
				allSucceeded = false;

			if (!floatMatrixC.isEqual(floatTest, float(epsilon)))
				allSucceeded = false;
			if (!floatMatrixD.isEqual(floatTest, float(epsilon)))
				allSucceeded = false;
			if (!floatMatrixDTransposed.isEqual(floatTestTransposed, float(epsilon)))
				allSucceeded = false;


			if (!doubleMatrixA.isEqual(doubleTest, double(epsilon)))
				allSucceeded = false;
			if (!doubleMatrixB.isEqual(doubleTest, double(epsilon)))
				allSucceeded = false;
			if (!doubleMatrixBTransposed.isEqual(doubleTestTransposed, double(epsilon)))
				allSucceeded = false;

			if (!doubleMatrixC.isEqual(doubleTest, double(epsilon)))
				allSucceeded = false;
			if (!doubleMatrixD.isEqual(doubleTest, double(epsilon)))
				allSucceeded = false;
			if (!doubleMatrixDTransposed.isEqual(doubleTestTransposed, double(epsilon)))
				allSucceeded = false;


			if (!scalarMatrixA.isEqual(scalarTest, Scalar(epsilon)))
				allSucceeded = false;
			if (!scalarMatrixB.isEqual(scalarTest, Scalar(epsilon)))
				allSucceeded = false;
			if (!scalarMatrixBTransposed.isEqual(scalarTestTransposed, Scalar(epsilon)))
				allSucceeded = false;

			if (!scalarMatrixC.isEqual(scalarTest, Scalar(epsilon)))
				allSucceeded = false;
			if (!scalarMatrixD.isEqual(scalarTest, Scalar(epsilon)))
				allSucceeded = false;
			if (!scalarMatrixDTransposed.isEqual(scalarTestTransposed, Scalar(epsilon)))
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

template <typename T>
bool TestSquareMatrix4::testVectorMultiplication(const double testDuration, Worker& worker)
{
	const unsigned int constNumber = 100000u;

	Log::info() << "Vector multiplication test for " << sizeof(T) * 8 << "bit precision with " << String::insertCharacter(String::toAString(constNumber), ',', 3, false) << " repetitions:";

	bool allSucceeded = true;

	SquareMatrixT4<T> matrix;
	std::vector< VectorT4<T> > vectors(constNumber);
	std::vector< VectorT4<T> > results(constNumber);

	constexpr T valueRange = std::is_same<T, double>::value ? T(100) : T(10);
	constexpr T epsilon = NumericT<T>::eps() * (std::is_same<T, double>::value ? T(10) : T(1000));

	RandomGenerator randomGenerator;

	{
		// first we test the standard implementation not using any kind of explicit optimization

		HighPerformanceStatistic performanceStandard;
		const Timestamp startTimestamp(true);

		do
		{
			for (unsigned int n = 0u; n < 16u; ++n)
			{
				matrix[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
			}

			for (size_t n = 0; n < constNumber; ++n)
			{
				for (unsigned int i = 0u; i < 4u; ++i)
				{
					vectors[n][i] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
				}
			}

			{
				const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performanceStandard);

				for (unsigned int n = 0u; n < constNumber; ++n)
				{
					results[n] = standardVectorMultiplication<T>(matrix, vectors[n]);
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		Log::info() << "Standard performance as reference: " << String::toAString(performanceStandard.averageMseconds()) << "ms";
		Log::info() << " ";
	}

	{
		// now we test the default implementation of the framework using SIMD optimizations

		unsigned long long iterations = 0ull;
		unsigned long long validIterations = 0ull;

		HighPerformanceStatistic performance;
		const Timestamp startTimestamp(true);

		do
		{
			for (unsigned int n = 0u; n < 16u; ++n)
			{
				matrix[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
			}

			for (size_t n = 0; n < constNumber; ++n)
			{
				for (unsigned int i = 0u; i < 4u; ++i)
				{
					vectors[n][i] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
				}
			}

			{
				const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performance);

				for (unsigned int n = 0u; n < constNumber; ++n)
				{
					results[n] = matrix * vectors[n];
				}
			}

			for (unsigned int n = 0u; n < constNumber; ++n)
			{
				bool localSucceeded = true;

				for (unsigned int i = 0u; i < 4u; ++i)
				{
					const T rowResult = matrix(i, 0) * vectors[n][0] +  matrix(i, 1) * vectors[n][1] +  matrix(i, 2) * vectors[n][2] +  matrix(i, 3) * vectors[n][3];

					if (NumericT<T>::isNotEqual(results[n][i], rowResult, epsilon))
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

		Log::info() << "Ocean performance individual: " << String::toAString(performance.averageMseconds()) << "ms";
		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

		allSucceeded = percent >= 0.99 && allSucceeded;
	}

	{
		// now we test the utility function of the framework allowing to process an entire array

		unsigned long long iterations = 0ull;
		unsigned long long validIterations = 0ull;

		HighPerformanceStatistic performanceShared;
		const Timestamp startTimestamp(true);

		do
		{
			for (unsigned int n = 0u; n < 16u; ++n)
			{
				matrix[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
			}

			for (size_t n = 0; n < constNumber; ++n)
			{
				for (unsigned int i = 0u; i < 4u; ++i)
				{
					vectors[n][i] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
				}
			}

			performanceShared.start();
				MathUtilities::transform(matrix, vectors, results, nullptr);
			performanceShared.stop();

			for (unsigned int n = 0u; n < constNumber; ++n)
			{
				bool localSucceeded = true;

				for (unsigned int i = 0u; i < 4u; ++i)
				{
					const T rowResult = matrix(i, 0) * vectors[n][0] +  matrix(i, 1) * vectors[n][1] +  matrix(i, 2) * vectors[n][2] +  matrix(i, 3) * vectors[n][3];

					if (NumericT<T>::isNotEqual(results[n][i], rowResult, epsilon))
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

		Log::info() << "Ocean performance array (singlecore): " << String::toAString(performanceShared.averageMseconds()) << "ms";
		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

		allSucceeded = percent >= 0.99 && allSucceeded;
	}

	if (worker)
	{
		// now we test the utility function of the framework allowing to process an entire array (this time with multi-core support)

		unsigned long long iterations = 0ull;
		unsigned long long validIterations = 0ull;

		HighPerformanceStatistic performanceShared;
		const Timestamp startTimestamp(true);

		do
		{
			for (unsigned int n = 0u; n < 16u; ++n)
			{
				matrix[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
			}

			for (size_t n = 0; n < constNumber; ++n)
			{
				for (unsigned int i = 0u; i < 4u; ++i)
				{
					vectors[n][i] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
				}
			}

			performanceShared.start();
			MathUtilities::transform(matrix, vectors, results, &worker);
			performanceShared.stop();

			for (unsigned int n = 0u; n < constNumber; ++n)
			{
				bool localSucceeded = true;

				for (unsigned int i = 0u; i < 4u; ++i)
				{
					const T rowResult = matrix(i, 0) * vectors[n][0] +  matrix(i, 1) * vectors[n][1] +  matrix(i, 2) * vectors[n][2] +  matrix(i, 3) * vectors[n][3];

					if (NumericT<T>::isNotEqual(results[n][i], rowResult, epsilon))
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

		Log::info() << "Ocean performance array (multicore): " << String::toAString(performanceShared.averageMseconds()) << "ms";
		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

		allSucceeded = percent >= 0.99 && allSucceeded;
	}

	return allSucceeded;
}

template <typename T>
bool TestSquareMatrix4::testMatrixMultiplication(const double testDuration)
{
	const unsigned int constNumber = 100000u;

	Log::info() << "Matrix multiplication test for " << sizeof(T) * 8 << "bit precision with " << String::insertCharacter(String::toAString(constNumber), ',', 3, false) << " repetitions:";

	bool allSucceeded = true;

	SquareMatrixT4<T> matrix;
	std::vector< SquareMatrixT4<T> > matrices(constNumber);
	std::vector< SquareMatrixT4<T> > results(constNumber);

	const T valueRange = std::is_same<T, double>::value ? T(100) : T(10);
	RandomGenerator randomGenerator;

	{
		// first we test the standard implementation not using any kind of explicit optimization

		HighPerformanceStatistic performanceStandard;
		const Timestamp startTimestamp(true);

		do
		{
			for (unsigned int n = 0u; n < 16u; ++n)
			{
				matrix[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
			}

			for (size_t n = 0; n < constNumber; ++n)
			{
				for (unsigned int i = 0u; i < 16u; ++i)
				{
					matrices[n][i] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
				}
			}

			{
				const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performanceStandard);

				for (unsigned int n = 0u; n < constNumber; ++n)
				{
					results[n] = standardMatrixMultiplication<T>(matrix, matrices[n]);
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		Log::info() << "Standard performance as reference: " << String::toAString(performanceStandard.averageMseconds()) << "ms";
		Log::info() << " ";
	}

	{
		// now we test the default implementation of the framework using SIMD optimizations

		unsigned long long iterations = 0ull;
		unsigned long long validIterations = 0ull;

		HighPerformanceStatistic performance;
		const Timestamp startTimestamp(true);

		do
		{
			for (unsigned int n = 0u; n < 16u; ++n)
			{
				matrix[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
			}

			for (size_t n = 0; n < constNumber; ++n)
			{
				for (unsigned int i = 0u; i < 16u; ++i)
				{
					matrices[n][i] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
				}
			}

			{
				const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performance);

				for (unsigned int n = 0u; n < constNumber; ++n)
				{
					results[n] = matrix * matrices[n];
				}
			}

			for (unsigned int n = 0u; n < constNumber; ++n)
			{
				const SquareMatrixT4<T> result = standardMatrixMultiplication(matrix, matrices[n]);

				bool localSucceeded = true;

				for (unsigned int i = 0u; i < 16u; ++i)
				{
					if (NumericT<T>::isNotEqual(results[n][i], result[i], NumericT<T>::eps() * (std::is_same<T, double>::value ? T(10) : T(100))))
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

		Log::info() << "Ocean performance individual: " << String::toAString(performance.averageMseconds()) << "ms";
		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

		allSucceeded = percent >= 0.99 && allSucceeded;
	}

	return allSucceeded;
}

bool TestSquareMatrix4::testInvert(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SquareMatrix4::invert() and SquareMatrix4::inverted() test:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Scalar epsilon = Numeric::eps() * 100;
	const SquareMatrix4 identity(true);

	const Timestamp startTimestamp(true);
	do
	{
		SquareMatrix4 matrix;

		for (unsigned int n = 0u; n < matrix.elements(); ++n)
			matrix[n] = Random::scalar(-1, 1);

		// we create a singular value each second iteration
		if (iterations % 2u == 0u)
		{
			Scalar factor = Random::scalar(-1, 1);
			while (Numeric::isWeakEqualEps(factor))
			{
				factor = Random::scalar(-1, 1);
			}

			if (RandomI::random(1u) == 0u)
			{
				unsigned int rowIndex0, rowIndex1;
				RandomI::random(3u, rowIndex0, rowIndex1);

				for (unsigned int c = 0u; c < 4u; ++c)
				{
					matrix(rowIndex0, c) = matrix(rowIndex1, c) * factor;
				}
			}
			else
			{
				unsigned int columnIndex0, columnIndex1;
				RandomI::random(3u, columnIndex0, columnIndex1);

				for (unsigned int r = 0u; r < 4u; ++r)
				{
					matrix(r, columnIndex0) = matrix(r, columnIndex1) * factor;
				}
			}
		}

		const bool matrixIsSingular = matrix.isSingular();
		ocean_assert((iterations % 2u) != 0u || matrixIsSingular);

		SquareMatrix4 invertedMatrix0(matrix);
		const bool matrixInverted0 = invertedMatrix0.invert();

		SquareMatrix4 invertedMatrix1;
		const bool matrixInverted1 = matrix.invert(invertedMatrix1);

		SquareMatrix4 invertedMatrix2;
		bool matrixInverted2 = false;

		if (matrixInverted0 || matrixInverted1)
		{
			invertedMatrix2 = matrix.inverted();
			matrixInverted2 = true;
		}

		bool localSucceeded = true;

		// disabled: ocean_assert(matrixInverted0 == !matrixIsSingular); as matrix.invert() is not using the determinant we cannot be 100% sure that a singular matrix cannot be inverted
		if (matrixInverted0 == matrixIsSingular)
		{
			localSucceeded = false;
		}

		ocean_assert(matrixInverted0 == matrixInverted1 && matrixInverted0 == matrixInverted2 && matrixInverted1 == matrixInverted2);
		if (matrixInverted0 != matrixInverted1 || matrixInverted0 != matrixInverted2 || matrixInverted1 != matrixInverted2)
			localSucceeded = false;

		if (matrixInverted0)
		{
			const SquareMatrix4 testMatrixA(matrix * invertedMatrix0);
			const SquareMatrix4 testMatrixB(invertedMatrix0 * matrix);

			if (!testMatrixA.isEqual(testMatrixB, epsilon))
				localSucceeded = false;

			if (!testMatrixA.isEqual(identity, epsilon))
				localSucceeded = false;

			if (!testMatrixB.isEqual(identity, epsilon))
				localSucceeded = false;
		}

		if (matrixInverted1)
		{
			const SquareMatrix4 testMatrixA(matrix * invertedMatrix1);
			const SquareMatrix4 testMatrixB(invertedMatrix1 * matrix);

			if (!testMatrixA.isEqual(testMatrixB, epsilon))
				localSucceeded = false;

			if (!testMatrixA.isEqual(identity, epsilon))
				localSucceeded = false;

			if (!testMatrixB.isEqual(identity, epsilon))
				localSucceeded = false;
		}

		if (matrixInverted2)
		{
			const SquareMatrix4 testMatrixA(matrix * invertedMatrix2);
			const SquareMatrix4 testMatrixB(invertedMatrix2 * matrix);

			if (!testMatrixA.isEqual(testMatrixB, epsilon))
				localSucceeded = false;

			if (!testMatrixA.isEqual(identity, epsilon))
				localSucceeded = false;

			if (!testMatrixB.isEqual(identity, epsilon))
				localSucceeded = false;
		}

		iterations++;

		if (localSucceeded)
		{
			validIterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestSquareMatrix4::testMatrixConversion(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SquareMatrix4::matrices2matrices() test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int size = RandomI::random(1000u);

		std::vector<SquareMatrixD4> matricesD;
		std::vector<SquareMatrixF4> matricesF;

		for (size_t n = 0; n < size; ++n)
		{
			SquareMatrixD4 matrixD;
			SquareMatrixF4 matrixF;

			for (unsigned int i = 0u; i < 16u; ++i)
			{
				matrixD[i] = RandomD::scalar(-10, 10);
				matrixF[i] = RandomF::scalar(-10, 10);
			}

			matricesD.push_back(matrixD);
			matricesF.push_back(matrixF);
		}

		const std::vector<SquareMatrixD4> convertedD2D_0(SquareMatrixD4::matrices2matrices(matricesD));
		const std::vector<SquareMatrixD4> convertedD2D_1(SquareMatrixD4::matrices2matrices(matricesD.data(), matricesD.size()));

		const std::vector<SquareMatrixF4> convertedD2F_0(SquareMatrixF4::matrices2matrices(matricesD));
		const std::vector<SquareMatrixF4> convertedD2F_1(SquareMatrixF4::matrices2matrices(matricesD.data(), matricesD.size()));

		const std::vector<SquareMatrixD4> convertedF2D_0(SquareMatrixD4::matrices2matrices(matricesF));
		const std::vector<SquareMatrixD4> convertedF2D_1(SquareMatrixD4::matrices2matrices(matricesF.data(), matricesF.size()));

		const std::vector<SquareMatrixF4> convertedF2F_0(SquareMatrixF4::matrices2matrices(matricesF));
		const std::vector<SquareMatrixF4> convertedF2F_1(SquareMatrixF4::matrices2matrices(matricesF.data(), matricesF.size()));

		for (size_t n = 0; n < size; ++n)
		{
			for (unsigned int i = 0u; i < 16u; ++i)
			{
				if (NumericD::isNotWeakEqual(matricesD[n][i], convertedD2D_0[n][i]))
				{
					allSucceeded = false;
				}

				if (NumericD::isNotWeakEqual(matricesD[n][i], convertedD2D_1[n][i]))
				{
					allSucceeded = false;
				}

				if (NumericD::isNotWeakEqual(matricesD[n][i], double(convertedD2F_0[n][i])))
				{
					allSucceeded = false;
				}

				if (NumericD::isNotWeakEqual(matricesD[n][i], double(convertedD2F_1[n][i])))
				{
					allSucceeded = false;
				}


				if (NumericF::isNotWeakEqual(matricesF[n][i], convertedF2F_0[n][i]))
				{
					allSucceeded = false;
				}

				if (NumericF::isNotWeakEqual(matricesF[n][i], convertedF2F_1[n][i]))
				{
					allSucceeded = false;
				}

				if (NumericF::isNotWeakEqual(matricesF[n][i], float(convertedF2D_0[n][i])))
				{
					allSucceeded = false;
				}

				if (NumericF::isNotWeakEqual(matricesF[n][i], float(convertedF2D_1[n][i])))
				{
					allSucceeded = false;
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

template <typename T>
bool TestSquareMatrix4::testProjectionMatrixFieldOfView(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SquareMatrix4::projectionMatrix() with field of view test, with " << TypeNamer::name<T>() << ":";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const T fovX = RandomT<T>::scalar(NumericT<T>::deg2rad(20), NumericT<T>::deg2rad(120));

		const unsigned int width = RandomI::random(50u, 2000u);
		const unsigned int height = RandomI::random(50u, 2000u);

		const T aspectRatio = T(width) / T(height);

		const T nearDistance = RandomT<T>::scalar(T(0.1), T(1));
		const T farDistance = RandomT<T>::scalar(T(10), T(100));

		const SquareMatrixT4<T> projectionMatrix = SquareMatrixT4<T>::projectionMatrix(fovX, aspectRatio, nearDistance, farDistance);

		const AnyCameraPinholeT<T> anyCamera(PinholeCameraT<T>(width, height, fovX));

		for (unsigned int n = 0u; n < 10u; ++n)
		{
			const VectorT3<T> objectPoint(RandomT<T>::scalar(T(-10), T(10)), RandomT<T>::scalar(T(-10), T(10)), RandomT<T>::scalar(T(0.05), T(10))); // allowing to have points behind the camera

			const VectorT2<T> imagePoint = anyCamera.projectToImage(objectPoint); // with visible range [0, width]x[0, height]

			const VectorT3<T> pointInUnitCube = projectionMatrix * objectPoint;

			const T pointInClipSpaceX = (pointInUnitCube.x() + T(1)) * T(width) * T(0.5); // visible clip space range [0, width]
			const T pointInClipSpaceY = -(pointInUnitCube.y() - T(1)) * T(height) * T(0.5); // visible clip space range [0, height]

			constexpr T pointThreshold = std::is_same<float, T>::value ? T(2) : T(0.1);

			if (NumericT<T>::isNotEqual(imagePoint.x(), pointInClipSpaceX, pointThreshold))
			{
				allSucceeded = false;
			}

			if (NumericT<T>::isNotEqual(imagePoint.y(), pointInClipSpaceY, pointThreshold))
			{
				allSucceeded = false;
			}
		}

		if (!SquareMatrixT4<T>::projectionMatrix(anyCamera, nearDistance, farDistance).isEqual(projectionMatrix, NumericT<T>::weakEps()))
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

template <typename T>
bool TestSquareMatrix4::testProjectionMatrixCameraMatrix(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SquareMatrix4::projectionMatrix() with camera matrix test, with " << TypeNamer::name<T>() << ":";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const T fovX = RandomT<T>::scalar(NumericT<T>::deg2rad(20), NumericT<T>::deg2rad(120));

		const unsigned int width = RandomI::random(50u, 2000u);
		const unsigned int height = RandomI::random(50u, 2000u);

		const T principalX = RandomT<T>::scalar(T(-50), T(width + 50u));
		const T principalY = RandomT<T>::scalar(T(-50), T(height + 50u));

		const T nearDistance = RandomT<T>::scalar(T(0.1), T(1));
		const T farDistance = RandomT<T>::scalar(T(10), T(100));

		const AnyCameraPinholeT<T> anyCamera(PinholeCameraT<T>(width, height, fovX, principalX, principalY));

		const SquareMatrixT4<T> projectionMatrix = SquareMatrixT4<T>::projectionMatrix(anyCamera, nearDistance, farDistance);

		for (unsigned int n = 0u; n < 10u; ++n)
		{
			const VectorT3<T> objectPoint(RandomT<T>::scalar(T(-10), T(10)), RandomT<T>::scalar(T(-10), T(10)), RandomT<T>::scalar(T(0.05), T(10))); // allowing to have points behind the camera

			const VectorT2<T> imagePoint = anyCamera.projectToImage(objectPoint); // with visible range [0, width]x[0, height]

			const VectorT3<T> pointInUnitCube = projectionMatrix * objectPoint; // with visible range [-1, 1]x[-1, 1]x[-1, 1]

			const T pointInClipSpaceX = (pointInUnitCube.x() + T(1)) * T(width) * T(0.5); // visible clip space range [0, width]
			const T pointInClipSpaceY = -(pointInUnitCube.y() - T(1)) * T(height) * T(0.5); // visible clip space range [0, height]

			constexpr T pointThreshold = std::is_same<float, T>::value ? T(2) : T(0.1);

			if (NumericT<T>::isNotEqual(imagePoint.x(), pointInClipSpaceX, pointThreshold))
			{
				allSucceeded = false;
			}

			if (NumericT<T>::isNotEqual(imagePoint.y(), pointInClipSpaceY, pointThreshold))
			{
				allSucceeded = false;
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

}

}

}
