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
#include "ocean/math/Matrix.h"
#include "ocean/math/Random.h"

#include "ocean/test/Validation.h"
#include "ocean/test/ValidationPrecision.h"

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
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testTranspose<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testTranspose<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDeterminant<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testDeterminant<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testTrace<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testTrace<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMatrixAddition<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testMatrixAddition<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMatrixSubtraction<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testMatrixSubtraction<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAccessor(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testEigenSystem<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testEigenSystem<double>(testDuration) && allSucceeded;

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

TEST(TestSquareMatrix4, Transpose_Float)
{
	EXPECT_TRUE(TestSquareMatrix4::testTranspose<float>(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix4, Transpose_Double)
{
	EXPECT_TRUE(TestSquareMatrix4::testTranspose<double>(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix4, Determinant_Float)
{
	EXPECT_TRUE(TestSquareMatrix4::testDeterminant<float>(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix4, Determinant_Double)
{
	EXPECT_TRUE(TestSquareMatrix4::testDeterminant<double>(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix4, Trace_Float)
{
	EXPECT_TRUE(TestSquareMatrix4::testTrace<float>(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix4, Trace_Double)
{
	EXPECT_TRUE(TestSquareMatrix4::testTrace<double>(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix4, MatrixAddition_Float)
{
	EXPECT_TRUE(TestSquareMatrix4::testMatrixAddition<float>(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix4, MatrixAddition_Double)
{
	EXPECT_TRUE(TestSquareMatrix4::testMatrixAddition<double>(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix4, MatrixSubtraction_Float)
{
	EXPECT_TRUE(TestSquareMatrix4::testMatrixSubtraction<float>(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix4, MatrixSubtraction_Double)
{
	EXPECT_TRUE(TestSquareMatrix4::testMatrixSubtraction<double>(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix4, Accessor)
{
	EXPECT_TRUE(TestSquareMatrix4::testAccessor(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix4, EigenSystem_Float)
{
	EXPECT_TRUE(TestSquareMatrix4::testEigenSystem<float>(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix4, EigenSystem_Double)
{
	EXPECT_TRUE(TestSquareMatrix4::testEigenSystem<double>(GTEST_TEST_DURATION));
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

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

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

			OCEAN_EXPECT_TRUE(validation, floatMatrixA.isEqual(floatTest, float(epsilon)));
			OCEAN_EXPECT_TRUE(validation, floatMatrixB.isEqual(floatTest, float(epsilon)));
			OCEAN_EXPECT_TRUE(validation, floatMatrixBTransposed.isEqual(floatTestTransposed, float(epsilon)));

			OCEAN_EXPECT_TRUE(validation, floatMatrixC.isEqual(floatTest, float(epsilon)));
			OCEAN_EXPECT_TRUE(validation, floatMatrixD.isEqual(floatTest, float(epsilon)));
			OCEAN_EXPECT_TRUE(validation, floatMatrixDTransposed.isEqual(floatTestTransposed, float(epsilon)));

			OCEAN_EXPECT_TRUE(validation, doubleMatrixA.isEqual(doubleTest, double(epsilon)));
			OCEAN_EXPECT_TRUE(validation, doubleMatrixB.isEqual(doubleTest, double(epsilon)));
			OCEAN_EXPECT_TRUE(validation, doubleMatrixBTransposed.isEqual(doubleTestTransposed, double(epsilon)));

			OCEAN_EXPECT_TRUE(validation, doubleMatrixC.isEqual(doubleTest, double(epsilon)));
			OCEAN_EXPECT_TRUE(validation, doubleMatrixD.isEqual(doubleTest, double(epsilon)));
			OCEAN_EXPECT_TRUE(validation, doubleMatrixDTransposed.isEqual(doubleTestTransposed, double(epsilon)));

			OCEAN_EXPECT_TRUE(validation, scalarMatrixA.isEqual(scalarTest, Scalar(epsilon)));
			OCEAN_EXPECT_TRUE(validation, scalarMatrixB.isEqual(scalarTest, Scalar(epsilon)));
			OCEAN_EXPECT_TRUE(validation, scalarMatrixBTransposed.isEqual(scalarTestTransposed, Scalar(epsilon)));

			OCEAN_EXPECT_TRUE(validation, scalarMatrixC.isEqual(scalarTest, Scalar(epsilon)));
			OCEAN_EXPECT_TRUE(validation, scalarMatrixD.isEqual(scalarTest, Scalar(epsilon)));
			OCEAN_EXPECT_TRUE(validation, scalarMatrixDTransposed.isEqual(scalarTestTransposed, Scalar(epsilon)));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
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

		ValidationPrecision validation(0.99, randomGenerator);

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
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				for (unsigned int i = 0u; i < 4u; ++i)
				{
					const T rowResult = matrix(i, 0) * vectors[n][0] +  matrix(i, 1) * vectors[n][1] +  matrix(i, 2) * vectors[n][2] +  matrix(i, 3) * vectors[n][3];

					if (NumericT<T>::isNotEqual(results[n][i], rowResult, epsilon))
					{
						scopedIteration.setInaccurate();
					}
				}
			}
		}
		while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

		Log::info() << "Ocean performance individual: " << String::toAString(performance.averageMseconds()) << "ms";
		Log::info() << "Validation: " << validation;

		if (!validation.succeeded())
		{
			allSucceeded = false;
		}
	}

	{
		// now we test the utility function of the framework allowing to process an entire array

		ValidationPrecision validation(0.99, randomGenerator);

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
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				for (unsigned int i = 0u; i < 4u; ++i)
				{
					const T rowResult = matrix(i, 0) * vectors[n][0] +  matrix(i, 1) * vectors[n][1] +  matrix(i, 2) * vectors[n][2] +  matrix(i, 3) * vectors[n][3];

					if (NumericT<T>::isNotEqual(results[n][i], rowResult, epsilon))
					{
						scopedIteration.setInaccurate();
					}
				}
			}
		}
		while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

		Log::info() << "Ocean performance array (singlecore): " << String::toAString(performanceShared.averageMseconds()) << "ms";
		Log::info() << "Validation: " << validation;

		if (!validation.succeeded())
		{
			allSucceeded = false;
		}
	}

	if (worker)
	{
		// now we test the utility function of the framework allowing to process an entire array (this time with multi-core support)

		ValidationPrecision validation(0.99, randomGenerator);

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
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				for (unsigned int i = 0u; i < 4u; ++i)
				{
					const T rowResult = matrix(i, 0) * vectors[n][0] +  matrix(i, 1) * vectors[n][1] +  matrix(i, 2) * vectors[n][2] +  matrix(i, 3) * vectors[n][3];

					if (NumericT<T>::isNotEqual(results[n][i], rowResult, epsilon))
					{
						scopedIteration.setInaccurate();
					}
				}
			}
		}
		while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

		Log::info() << "Ocean performance array (multicore): " << String::toAString(performanceShared.averageMseconds()) << "ms";
		Log::info() << "Validation: " << validation;

		if (!validation.succeeded())
		{
			allSucceeded = false;
		}
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

		ValidationPrecision validation(0.99, randomGenerator);

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
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const SquareMatrixT4<T> result = standardMatrixMultiplication(matrix, matrices[n]);

				for (unsigned int i = 0u; i < 16u; ++i)
				{
					if (NumericT<T>::isNotEqual(results[n][i], result[i], NumericT<T>::eps() * (std::is_same<T, double>::value ? T(10) : T(100))))
					{
						scopedIteration.setInaccurate();
					}
				}
			}
		}
		while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

		Log::info() << "Ocean performance individual: " << String::toAString(performance.averageMseconds()) << "ms";
		Log::info() << "Validation: " << validation;

		if (!validation.succeeded())
		{
			allSucceeded = false;
		}
	}

	return allSucceeded;
}

bool TestSquareMatrix4::testInvert(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SquareMatrix4::invert() and SquareMatrix4::inverted() test:";

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.99, randomGenerator);

	const Scalar epsilon = Numeric::eps() * Scalar(100);
	const SquareMatrix4 identity(true);

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		SquareMatrix4 matrix;

		for (unsigned int n = 0u; n < matrix.elements(); ++n)
		{
			matrix[n] = Random::scalar(randomGenerator, -1, 1);
		}

		// we create a singular value each second iteration
		if (validation.iterations() % 2u == 0u)
		{
			Scalar factor = Random::scalar(randomGenerator, -1, 1);
			while (Numeric::isWeakEqualEps(factor))
			{
				factor = Random::scalar(randomGenerator, -1, 1);
			}

			if (RandomI::random(randomGenerator, 1u) == 0u)
			{
				unsigned int rowIndex0, rowIndex1;
				RandomI::random(randomGenerator, 3u, rowIndex0, rowIndex1);

				for (unsigned int c = 0u; c < 4u; ++c)
				{
					matrix(rowIndex0, c) = matrix(rowIndex1, c) * factor;
				}
			}
			else
			{
				unsigned int columnIndex0, columnIndex1;
				RandomI::random(randomGenerator, 3u, columnIndex0, columnIndex1);

				for (unsigned int r = 0u; r < 4u; ++r)
				{
					matrix(r, columnIndex0) = matrix(r, columnIndex1) * factor;
				}
			}
		}

		const bool matrixIsSingular = matrix.isSingular();
		ocean_assert((validation.iterations() % 2u) != 0u || matrixIsSingular);

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

		// disabled: ocean_assert(matrixInverted0 == !matrixIsSingular); as matrix.invert() is not using the determinant we cannot be 100% sure that a singular matrix cannot be inverted
		if (matrixInverted0 == matrixIsSingular)
		{
			scopedIteration.setInaccurate();
		}

		ocean_assert(matrixInverted0 == matrixInverted1 && matrixInverted0 == matrixInverted2 && matrixInverted1 == matrixInverted2);
		if (matrixInverted0 != matrixInverted1 || matrixInverted0 != matrixInverted2 || matrixInverted1 != matrixInverted2)
		{
			scopedIteration.setInaccurate();
		}

		if (matrixInverted0)
		{
			const SquareMatrix4 testMatrixA(matrix * invertedMatrix0);
			const SquareMatrix4 testMatrixB(invertedMatrix0 * matrix);

			if (!testMatrixA.isEqual(testMatrixB, epsilon))
			{
				scopedIteration.setInaccurate();
			}

			if (!testMatrixA.isEqual(identity, epsilon))
			{
				scopedIteration.setInaccurate();
			}

			if (!testMatrixB.isEqual(identity, epsilon))
			{
				scopedIteration.setInaccurate();
			}
		}

		if (matrixInverted1)
		{
			const SquareMatrix4 testMatrixA(matrix * invertedMatrix1);
			const SquareMatrix4 testMatrixB(invertedMatrix1 * matrix);

			if (!testMatrixA.isEqual(testMatrixB, epsilon))
			{
				scopedIteration.setInaccurate();
			}

			if (!testMatrixA.isEqual(identity, epsilon))
			{
				scopedIteration.setInaccurate();
			}

			if (!testMatrixB.isEqual(identity, epsilon))
			{
				scopedIteration.setInaccurate();
			}
		}

		if (matrixInverted2)
		{
			const SquareMatrix4 testMatrixA(matrix * invertedMatrix2);
			const SquareMatrix4 testMatrixB(invertedMatrix2 * matrix);

			if (!testMatrixA.isEqual(testMatrixB, epsilon))
			{
				scopedIteration.setInaccurate();
			}

			if (!testMatrixA.isEqual(identity, epsilon))
			{
				scopedIteration.setInaccurate();
			}

			if (!testMatrixB.isEqual(identity, epsilon))
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestSquareMatrix4::testMatrixConversion(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SquareMatrix4::matrices2matrices() test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int size = RandomI::random(randomGenerator, 1000u);

		std::vector<SquareMatrixD4> matricesD;
		std::vector<SquareMatrixF4> matricesF;

		for (size_t n = 0; n < size; ++n)
		{
			SquareMatrixD4 matrixD;
			SquareMatrixF4 matrixF;

			for (unsigned int i = 0u; i < 16u; ++i)
			{
				matrixD[i] = RandomD::scalar(randomGenerator, -10, 10);
				matrixF[i] = RandomF::scalar(randomGenerator, -10, 10);
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
				OCEAN_EXPECT_TRUE(validation, NumericD::isWeakEqual(matricesD[n][i], convertedD2D_0[n][i]));
				OCEAN_EXPECT_TRUE(validation, NumericD::isWeakEqual(matricesD[n][i], convertedD2D_1[n][i]));
				OCEAN_EXPECT_TRUE(validation, NumericD::isWeakEqual(matricesD[n][i], double(convertedD2F_0[n][i])));
				OCEAN_EXPECT_TRUE(validation, NumericD::isWeakEqual(matricesD[n][i], double(convertedD2F_1[n][i])));

				OCEAN_EXPECT_TRUE(validation, NumericF::isWeakEqual(matricesF[n][i], convertedF2F_0[n][i]));
				OCEAN_EXPECT_TRUE(validation, NumericF::isWeakEqual(matricesF[n][i], convertedF2F_1[n][i]));
				OCEAN_EXPECT_TRUE(validation, NumericF::isWeakEqual(matricesF[n][i], float(convertedF2D_0[n][i])));
				OCEAN_EXPECT_TRUE(validation, NumericF::isWeakEqual(matricesF[n][i], float(convertedF2D_1[n][i])));
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestSquareMatrix4::testProjectionMatrixFieldOfView(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SquareMatrix4::projectionMatrix() with field of view test, with " << TypeNamer::name<T>() << ":";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const T fovX = RandomT<T>::scalar(randomGenerator, NumericT<T>::deg2rad(20), NumericT<T>::deg2rad(120));

		const unsigned int width = RandomI::random(randomGenerator, 50u, 2000u);
		const unsigned int height = RandomI::random(randomGenerator, 50u, 2000u);

		const T aspectRatio = T(width) / T(height);

		const T nearDistance = RandomT<T>::scalar(randomGenerator, T(0.1), T(1));
		const T farDistance = RandomT<T>::scalar(randomGenerator, T(10), T(100));

		const SquareMatrixT4<T> projectionMatrix = SquareMatrixT4<T>::projectionMatrix(fovX, aspectRatio, nearDistance, farDistance);

		const AnyCameraPinholeT<T> anyCamera(PinholeCameraT<T>(width, height, fovX));

		for (unsigned int n = 0u; n < 10u; ++n)
		{
			const VectorT3<T> objectPoint(RandomT<T>::scalar(randomGenerator, T(-10), T(10)), RandomT<T>::scalar(randomGenerator, T(-10), T(10)), RandomT<T>::scalar(randomGenerator, T(0.05), T(10))); // allowing to have points behind the camera

			const VectorT2<T> imagePoint = anyCamera.projectToImage(objectPoint); // with visible range [0, width]x[0, height]

			const VectorT3<T> pointInUnitCube = projectionMatrix * objectPoint;

			const T pointInClipSpaceX = (pointInUnitCube.x() + T(1)) * T(width) * T(0.5); // visible clip space range [0, width]
			const T pointInClipSpaceY = -(pointInUnitCube.y() - T(1)) * T(height) * T(0.5); // visible clip space range [0, height]

			constexpr T pointThreshold = std::is_same<float, T>::value ? T(2) : T(0.1);

			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(imagePoint.x(), pointInClipSpaceX, pointThreshold));
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(imagePoint.y(), pointInClipSpaceY, pointThreshold));
		}

		OCEAN_EXPECT_TRUE(validation, SquareMatrixT4<T>::projectionMatrix(anyCamera, nearDistance, farDistance).isEqual(projectionMatrix, NumericT<T>::weakEps()));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestSquareMatrix4::testProjectionMatrixCameraMatrix(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SquareMatrix4::projectionMatrix() with camera matrix test, with " << TypeNamer::name<T>() << ":";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const T fovX = RandomT<T>::scalar(randomGenerator, NumericT<T>::deg2rad(20), NumericT<T>::deg2rad(120));

		const unsigned int width = RandomI::random(randomGenerator, 50u, 2000u);
		const unsigned int height = RandomI::random(randomGenerator, 50u, 2000u);

		const T principalX = RandomT<T>::scalar(randomGenerator, T(-50), T(width + 50u));
		const T principalY = RandomT<T>::scalar(randomGenerator, T(-50), T(height + 50u));

		const T nearDistance = RandomT<T>::scalar(randomGenerator, T(0.1), T(1));
		const T farDistance = RandomT<T>::scalar(randomGenerator, T(10), T(100));

		const AnyCameraPinholeT<T> anyCamera(PinholeCameraT<T>(width, height, fovX, principalX, principalY));

		const SquareMatrixT4<T> projectionMatrix = SquareMatrixT4<T>::projectionMatrix(anyCamera, nearDistance, farDistance);

		for (unsigned int n = 0u; n < 10u; ++n)
		{
			const VectorT3<T> objectPoint(RandomT<T>::scalar(randomGenerator, T(-10), T(10)), RandomT<T>::scalar(randomGenerator, T(-10), T(10)), RandomT<T>::scalar(randomGenerator, T(0.05), T(10))); // allowing to have points behind the camera

			const VectorT2<T> imagePoint = anyCamera.projectToImage(objectPoint); // with visible range [0, width]x[0, height]

			const VectorT3<T> pointInUnitCube = projectionMatrix * objectPoint; // with visible range [-1, 1]x[-1, 1]x[-1, 1]

			const T pointInClipSpaceX = (pointInUnitCube.x() + T(1)) * T(width) * T(0.5); // visible clip space range [0, width]
			const T pointInClipSpaceY = -(pointInUnitCube.y() - T(1)) * T(height) * T(0.5); // visible clip space range [0, height]

			constexpr T pointThreshold = std::is_same<float, T>::value ? T(2) : T(0.1);

			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(imagePoint.x(), pointInClipSpaceX, pointThreshold));
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(imagePoint.y(), pointInClipSpaceY, pointThreshold));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestSquareMatrix4::testTranspose(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Transpose test, with " << TypeNamer::name<T>() << ":";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		SquareMatrixT4<T> matrix;
		for (unsigned int i = 0u; i < 16u; ++i)
		{
			matrix[i] = RandomT<T>::scalar(randomGenerator, T(-10), T(10));
		}

		const SquareMatrixT4<T> transposedMatrix = matrix.transposed();

		for (unsigned int r = 0u; r < 4u; ++r)
		{
			for (unsigned int c = 0u; c < 4u; ++c)
			{
				OCEAN_EXPECT_EQUAL(validation, transposedMatrix(r, c), matrix(c, r));
			}
		}

		SquareMatrixT4<T> matrix2(matrix);
		matrix2.transpose();

		OCEAN_EXPECT_TRUE(validation, transposedMatrix == matrix2);

		SquareMatrixT4<T> doubleTransposed = transposedMatrix.transposed();

		OCEAN_EXPECT_TRUE(validation, matrix == doubleTransposed);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestSquareMatrix4::testDeterminant(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Determinant test, with " << TypeNamer::name<T>() << ":";

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.95, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const SquareMatrixT4<T> identity(true);
			const T det = identity.determinant();

			if (NumericT<T>::isNotEqual(det, T(1), NumericT<T>::weakEps()))
			{
				scopedIteration.setInaccurate();
			}
		}

		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			SquareMatrixT4<T> matrixA, matrixB;
			for (unsigned int i = 0u; i < 16u; ++i)
			{
				matrixA[i] = RandomT<T>::scalar(randomGenerator, T(-10), T(10));
				matrixB[i] = RandomT<T>::scalar(randomGenerator, T(-10), T(10));
			}

			const T detA = matrixA.determinant();
			const T detB = matrixB.determinant();
			const SquareMatrixT4<T> matrixAB = matrixA * matrixB;
			const T detAB = matrixAB.determinant();

			const T expectedDetAB = detA * detB;

			const T epsilon = std::max(NumericT<T>::abs(expectedDetAB) * T(0.01), NumericT<T>::eps() * T(100));

			if (NumericT<T>::isNotEqual(detAB, expectedDetAB, epsilon))
			{
				scopedIteration.setInaccurate();
			}
		}

		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			SquareMatrixT4<T> matrix;
			for (unsigned int i = 0u; i < 16u; ++i)
			{
				matrix[i] = RandomT<T>::scalar(randomGenerator, T(-10), T(10));
			}

			const T det = matrix.determinant();
			const SquareMatrixT4<T> transposed = matrix.transposed();
			const T detTransposed = transposed.determinant();

			const T epsilon = std::max(NumericT<T>::abs(det) * T(0.01), NumericT<T>::eps() * T(100));

			if (NumericT<T>::isNotEqual(det, detTransposed, epsilon))
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestSquareMatrix4::testTrace(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Trace test, with " << TypeNamer::name<T>() << ":";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const SquareMatrixT4<T> identity(true);
		OCEAN_EXPECT_EQUAL(validation, identity.trace(), T(4));

		SquareMatrixT4<T> matrix;
		for (unsigned int i = 0u; i < 16u; ++i)
		{
			matrix[i] = RandomT<T>::scalar(randomGenerator, T(-100), T(100));
		}

		const T trace = matrix.trace();
		const T expectedTrace = matrix(0, 0) + matrix(1, 1) + matrix(2, 2) + matrix(3, 3);

		OCEAN_EXPECT_EQUAL(validation, trace, expectedTrace);

		SquareMatrixT4<T> matrixA, matrixB;
		for (unsigned int i = 0u; i < 16u; ++i)
		{
			matrixA[i] = RandomT<T>::scalar(randomGenerator, T(-100), T(100));
			matrixB[i] = RandomT<T>::scalar(randomGenerator, T(-100), T(100));
		}

		const T traceA = matrixA.trace();
		const T traceB = matrixB.trace();
		const SquareMatrixT4<T> matrixSum = matrixA + matrixB;
		const T traceSum = matrixSum.trace();

		OCEAN_EXPECT_TRUE(validation, NumericT<T>::isWeakEqual(traceSum, traceA + traceB));

		const SquareMatrixT4<T> transposed = matrixA.transposed();
		OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(matrixA.trace(), transposed.trace()));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestSquareMatrix4::testMatrixAddition(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Matrix addition test, with " << TypeNamer::name<T>() << ":";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		SquareMatrixT4<T> matrixA, matrixB;
		for (unsigned int i = 0u; i < 16u; ++i)
		{
			matrixA[i] = RandomT<T>::scalar(randomGenerator, T(-100), T(100));
			matrixB[i] = RandomT<T>::scalar(randomGenerator, T(-100), T(100));
		}

		const SquareMatrixT4<T> sum = matrixA + matrixB;
		for (unsigned int i = 0u; i < 16u; ++i)
		{
			OCEAN_EXPECT_EQUAL(validation, sum[i], matrixA[i] + matrixB[i]);
		}

		SquareMatrixT4<T> matrixC(matrixA);
		matrixC += matrixB;
		OCEAN_EXPECT_TRUE(validation, sum == matrixC);

		const SquareMatrixT4<T> sumBA = matrixB + matrixA;
		OCEAN_EXPECT_TRUE(validation, sum == sumBA);

		SquareMatrixT4<T> matrixD;
		for (unsigned int i = 0u; i < 16u; ++i)
		{
			matrixD[i] = RandomT<T>::scalar(randomGenerator, T(-100), T(100));
		}
		const SquareMatrixT4<T> sumABC1 = (matrixA + matrixB) + matrixD;
		const SquareMatrixT4<T> sumABC2 = matrixA + (matrixB + matrixD);

		OCEAN_EXPECT_TRUE(validation, sumABC1.isEqual(sumABC2, NumericT<T>::weakEps()));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestSquareMatrix4::testMatrixSubtraction(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Matrix subtraction test, with " << TypeNamer::name<T>() << ":";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		SquareMatrixT4<T> matrixA, matrixB;
		for (unsigned int i = 0u; i < 16u; ++i)
		{
			matrixA[i] = RandomT<T>::scalar(randomGenerator, T(-100), T(100));
			matrixB[i] = RandomT<T>::scalar(randomGenerator, T(-100), T(100));
		}

		const SquareMatrixT4<T> diff = matrixA - matrixB;
		for (unsigned int i = 0u; i < 16u; ++i)
		{
			OCEAN_EXPECT_EQUAL(validation, diff[i], matrixA[i] - matrixB[i]);
		}

		SquareMatrixT4<T> matrixC(matrixA);
		matrixC -= matrixB;
		OCEAN_EXPECT_TRUE(validation, diff == matrixC);

		const SquareMatrixT4<T> negA = -matrixA;
		for (unsigned int i = 0u; i < 16u; ++i)
		{
			OCEAN_EXPECT_EQUAL(validation, negA[i], -matrixA[i]);
		}

		const SquareMatrixT4<T> zero = matrixA - matrixA;
		for (unsigned int i = 0u; i < 16u; ++i)
		{
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqualEps(zero[i]));
		}

		const SquareMatrixT4<T> negB = -matrixB;
		const SquareMatrixT4<T> diffAlt = matrixA + negB;
		OCEAN_EXPECT_TRUE(validation, diff.isEqual(diffAlt));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestSquareMatrix4::testAccessor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Accessor operators test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		Scalar values[16];
		for (unsigned int i = 0u; i < 16u; ++i)
		{
			values[i] = Random::scalar(randomGenerator, Scalar(-100), Scalar(100));
		}

		SquareMatrix4 matrix(values);

		for (unsigned int i = 0u; i < 16u; ++i)
		{
			OCEAN_EXPECT_EQUAL(validation, matrix[i], values[i]);
		}

		unsigned int index = 0u;
		for (unsigned int c = 0u; c < 4u; ++c)
		{
			for (unsigned int r = 0u; r < 4u; ++r)
			{
				OCEAN_EXPECT_EQUAL(validation, matrix(r, c), values[index]);
				++index;
			}
		}

		for (unsigned int i = 0u; i < 16u; ++i)
		{
			OCEAN_EXPECT_EQUAL(validation, matrix(i), values[i]);
		}

		const Scalar* dataPtr = matrix.data();
		for (unsigned int i = 0u; i < 16u; ++i)
		{
			OCEAN_EXPECT_EQUAL(validation, dataPtr[i], values[i]);
		}

		const Scalar* directPtr = matrix();
		for (unsigned int i = 0u; i < 16u; ++i)
		{
			OCEAN_EXPECT_EQUAL(validation, directPtr[i], values[i]);
		}

		SquareMatrix4 matrix2;
		for (unsigned int i = 0u; i < 16u; ++i)
		{
			matrix2[i] = values[i];
		}
		OCEAN_EXPECT_TRUE(validation, matrix == matrix2);

		// Test modification via operator()(row, col)
		SquareMatrix4 matrix3;
		index = 0u;
		for (unsigned int c = 0u; c < 4u; ++c)
		{
			for (unsigned int r = 0u; r < 4u; ++r)
			{
				matrix3(r, c) = values[index];
				++index;
			}
		}
		OCEAN_EXPECT_TRUE(validation, matrix == matrix3);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestSquareMatrix4::testEigenSystem(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "EigenSystem test, with " << TypeNamer::name<T>() << ":";

	constexpr double successThreshold = std::is_same<T, float>::value ? 0.90 : 0.95;

	RandomGenerator randomGenerator;
	ValidationPrecision validation(successThreshold, randomGenerator);

	{
		// the identity matrix should have eigenvalues 1, 1, 1, 1

		const SquareMatrixT4<T> identity(true);

		T eigenValues[4];
		VectorT4<T> eigenVectors[4];

		if (identity.eigenSystem(eigenValues, eigenVectors))
		{
			for (unsigned int i = 0u; i < 4u; ++i)
			{
				if (NumericT<T>::isNotEqual(eigenValues[i], T(1.0), NumericT<T>::weakEps()))
				{
					OCEAN_SET_FAILED(validation);
				}
			}
		}
		else
		{
			OCEAN_SET_FAILED(validation);
		}
	}

	HighPerformanceStatistic performance;
	HighPerformanceStatistic performanceArbitrary;

	const Timestamp startTimestamp(true);

	do
	{
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			// let's test a diagonal matrix, eigenvalues should be the diagonal elements
			// eigenvectors are the standard basis vectors

			const T d0 = RandomT<T>::scalar(randomGenerator, T(-10), T(10));
			const T d1 = RandomT<T>::scalar(randomGenerator, T(-10), T(10));
			const T d2 = RandomT<T>::scalar(randomGenerator, T(-10), T(10));
			const T d3 = RandomT<T>::scalar(randomGenerator, T(-10), T(10));

			const SquareMatrixT4<T> diagonalMatrix(VectorT4<T>(d0, d1, d2, d3));

			T eigenValues[4];
			VectorT4<T> eigenVectors[4];

			if (diagonalMatrix.eigenSystem(eigenValues, eigenVectors))
			{
				std::vector<T> expectedEigenValues = {d0, d1, d2, d3};
				std::sort(expectedEigenValues.begin(), expectedEigenValues.end());

				std::vector<T> actualEigenValues = {eigenValues[0], eigenValues[1], eigenValues[2], eigenValues[3]};
				std::sort(actualEigenValues.begin(), actualEigenValues.end());

				for (unsigned int i = 0u; i < 4u; ++i)
				{
					const T expectedEigenValue = expectedEigenValues[i];
					const T actualEigenValue = actualEigenValues[i];

					constexpr T epsilon = std::is_same<T, float>::value ? T(0.1) : NumericT<T>::weakEps();

					if (NumericT<T>::isNotEqual(expectedEigenValue, actualEigenValue, epsilon))
					{
						scopedIteration.setInaccurate();
					}
				}

				for (unsigned int i = 0u; i < 4u; ++i)
				{
					const T eigenValue = eigenValues[i];
					const VectorT4<T>& eigenVector = eigenVectors[i];

					VectorT4<T> Av = diagonalMatrix * eigenVector;
					VectorT4<T> lambdaV = eigenVector * eigenValue;

					constexpr T epsilon = std::is_same<T, float>::value ? T(0.1) : NumericT<T>::weakEps();

					if (!Av.isEqual(lambdaV, epsilon))
					{
						scopedIteration.setInaccurate();
					}
				}
			}
			else
			{
				scopedIteration.setInaccurate();
			}
		}

		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			// symmetric matrix, all eigenvalues should be real and eigenvectors should be orthogonal

			SquareMatrixT4<T> matrix;
			for (unsigned int r = 0u; r < 4u; ++r)
			{
				for (unsigned int c = r; c < 4u; ++c)
				{
					const T value = RandomT<T>::scalar(randomGenerator, T(-10), T(10));

					matrix(r, c) = value;
					matrix(c, r) = value;
				}
			}

			T eigenValues[4];
			VectorT4<T> eigenVectors[4];

			performance.start();
				const bool result = matrix.eigenSystem(eigenValues, eigenVectors);
			performance.stop();

			if (result)
			{
				for (unsigned int i = 0u; i < 4u; ++i)
				{
					const T eigenValue = eigenValues[i];
					const VectorT4<T>& eigenVector = eigenVectors[i];

					VectorT4<T> Av = matrix * eigenVector;
					VectorT4<T> lambdaV = eigenVector * eigenValue;

					if (!Av.isEqual(lambdaV, NumericT<T>::eps() * T(100)))
					{
						scopedIteration.setInaccurate();
					}
				}

				// let's ensure that the eigenvectors are orthogonal

				for (unsigned int iOuter = 0u; iOuter < 4u; ++iOuter)
				{
					const VectorT4<T>& outerEigenVector = eigenVectors[iOuter];

					for (unsigned int iInner = iOuter + 1u; iInner < 4u; ++iInner)
					{
						const VectorT4<T>& innerEigenVector = eigenVectors[iInner];

						const T dotProduct = outerEigenVector * innerEigenVector;

						if (NumericT<T>::isNotWeakEqual(dotProduct, T(0)))
						{
							scopedIteration.setInaccurate();
						}
					}
				}
			}
			else
			{
				scopedIteration.setInaccurate();
			}

			{
				const MatrixT<T> matrixArbitrary(4, 4, matrix.data(), false /*valuesRowAligned*/);

				MatrixT<T> eigenValuesArbitrary;
				MatrixT<T> eigenVectorsArbitrary;

				performanceArbitrary.start();
					matrixArbitrary.eigenSystem(eigenValuesArbitrary, eigenVectorsArbitrary);
				performanceArbitrary.stop();
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance SquareMatrix4: " << performance;
	Log::info() << "Performance Matrix: " << performanceArbitrary;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
