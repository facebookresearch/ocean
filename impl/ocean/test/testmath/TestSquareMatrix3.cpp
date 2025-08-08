/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestSquareMatrix3.h"

#include "ocean/base/DataType.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/MathUtilities.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Random.h"

#include <algorithm>
#include <iomanip>
#include <numeric>
#include <sstream>

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestSquareMatrix3::test(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   SquareMatrix3 test:   ---";
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

	allSucceeded = testQuaternionConstructor<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testQuaternionConstructor<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testVectorMultiplication2<float>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testVectorMultiplication2<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testVectorMultiplication3<float>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testVectorMultiplication3<double>(testDuration, worker) && allSucceeded;

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

	allSucceeded = testSolve(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "SquareMatrix3 test succeeded.";
	}
	else
	{
		Log::info() << "SquareMatrix3 test FAILED.";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestSquareMatrix3, WriteToMessenger)
{
	EXPECT_TRUE(TestSquareMatrix3::testWriteToMessenger());
}

TEST(TestSquareMatrix3, ElementConstructor)
{
	EXPECT_TRUE(TestSquareMatrix3::testElementConstructor(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix3, QuaternionConstructorFloat)
{
	EXPECT_TRUE(TestSquareMatrix3::testQuaternionConstructor<float>(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix3, QuaternionConstructorDouble)
{
	EXPECT_TRUE(TestSquareMatrix3::testQuaternionConstructor<double>(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix3, Vector2DMultiplication32)
{
	EXPECT_TRUE(TestSquareMatrix3::testVectorMultiplication2<float>(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix3, Vector2DMultiplication64)
{
	EXPECT_TRUE(TestSquareMatrix3::testVectorMultiplication2<double>(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix3, Vector3DMultiplication32)
{
	Worker worker;
	EXPECT_TRUE(TestSquareMatrix3::testVectorMultiplication3<float>(GTEST_TEST_DURATION, worker));
}

TEST(TestSquareMatrix3, Vector3DMultiplication64)
{
	Worker worker;
	EXPECT_TRUE(TestSquareMatrix3::testVectorMultiplication3<double>(GTEST_TEST_DURATION, worker));
}

TEST(TestSquareMatrix3, Invert)
{
	EXPECT_TRUE(TestSquareMatrix3::testInvert(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix3, MatrixConversion)
{
	EXPECT_TRUE(TestSquareMatrix3::testMatrixConversion(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix3, Solve)
{
	EXPECT_TRUE(TestSquareMatrix3::testSolve(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestSquareMatrix3::testWriteToMessenger()
{
	Log::info() << "Write to messenger test:";

	// this is mainly a check whether the code does not compile or crash

	Log::info() << " ";

	Log::info() << SquareMatrix3(true);
	Log::info() << " ";
	Log::info() << "Matrix: " << SquareMatrix3(true);
	Log::info() << " ";
	Log::info() << SquareMatrix3(true) << " <- Matrix";

	Log::info() << " ";
	Log::info() << "Validation succeeded.";

	return true;
}

bool TestSquareMatrix3::testElementConstructor(const double testDuration)
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
			float floatValues[9];
			double doubleValues[9];
			Scalar scalarValues[9];

			for (unsigned int i = 0u; i < 9u; ++i)
			{
				doubleValues[i] = RandomT<double>::scalar(randomGenerator, -100, 100);
				floatValues[i] = float(doubleValues[i]);
				scalarValues[i] = Scalar(doubleValues[i]);
			}

			const SquareMatrixT3<float> floatMatrixA(floatValues);
			const SquareMatrixT3<float> floatMatrixB(floatValues, false);
			const SquareMatrixT3<float> floatMatrixBTransposed(floatValues, true);

			const SquareMatrixT3<float> floatMatrixC(doubleValues);
			const SquareMatrixT3<float> floatMatrixD(doubleValues, false);
			const SquareMatrixT3<float> floatMatrixDTransposed(doubleValues, true);


			const SquareMatrixT3<double> doubleMatrixA(floatValues);
			const SquareMatrixT3<double> doubleMatrixB(floatValues, false);
			const SquareMatrixT3<double> doubleMatrixBTransposed(floatValues, true);

			const SquareMatrixT3<double> doubleMatrixC(doubleValues);
			const SquareMatrixT3<double> doubleMatrixD(doubleValues, false);
			const SquareMatrixT3<double> doubleMatrixDTransposed(doubleValues, true);


			const SquareMatrix3 scalarMatrixA(floatValues);
			const SquareMatrix3 scalarMatrixB(floatValues, false);
			const SquareMatrix3 scalarMatrixBTransposed(floatValues, true);

			const SquareMatrix3 scalarMatrixC(doubleValues);
			const SquareMatrix3 scalarMatrixD(doubleValues, false);
			const SquareMatrix3 scalarMatrixDTransposed(doubleValues, true);


			SquareMatrixT3<float> floatTest, floatTestTransposed;
			SquareMatrixT3<double> doubleTest, doubleTestTransposed;
			SquareMatrix3 scalarTest, scalarTestTransposed;

			unsigned int index = 0u;
			for (unsigned int c = 0u; c < 3u; ++c)
				for (unsigned int r = 0u; r < 3u; ++r)
				{
					floatTest(r, c) = floatValues[index];
					doubleTest(r, c) = doubleValues[index];
					scalarTest(r, c) = scalarValues[index];

					floatTestTransposed(c, r) = floatValues[index];
					doubleTestTransposed(c, r) = doubleValues[index];
					scalarTestTransposed(c, r) = scalarValues[index];

					index++;
				}

			ocean_assert(index == 9u);

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
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

template <typename T>
bool TestSquareMatrix3::testQuaternionConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Quaternion-based constructor test for '" << TypeNamer::name<T>() << "':";

	bool allSucceeded = true;
	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		T w = RandomT<T>::scalar(randomGenerator, -10, 10);
		T x = RandomT<T>::scalar(randomGenerator, -10, 10);
		T y = RandomT<T>::scalar(randomGenerator, -10, 10);
		T z = RandomT<T>::scalar(randomGenerator, -10, 10);

		const T length = NumericT<T>::sqrt(w * w + x * x + y * y + z * z);

		// we explicitly use the inverse to ensure that the quaternion is not perfectly normalized
		const T invLength = T(1) / length;

		const QuaternionT<T> quanterion(w * invLength, x * invLength, y * invLength, z * invLength);

		if (quanterion.isValid() == false)
		{
			allSucceeded = false;
		}
		else
		{
			const SquareMatrixT3<T> rotationMatrix(quanterion);

			const VectorT3<T> vector(RandomT<T>::vector3(randomGenerator, -10, 10));

			const VectorT3<T> rotatedVectorMatrix = rotationMatrix * vector;
			const VectorT3<T> rotatedVectorQuaternion = quanterion * vector;

			if (rotatedVectorMatrix.isEqual(rotatedVectorQuaternion, NumericT<T>::weakEps()) == false)
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

template <typename T>
bool TestSquareMatrix3::testVectorMultiplication2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int constNumber = 1000u;

	Log::info() << "2D Vector multiplication test for " << sizeof(T) * 8 << "bit precision:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const T valueRange = std::is_same<T, double>::value ? T(100) : T(10);
	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int i = 0u; i < constNumber; ++i)
		{
			unsigned int localValidIterations = 0u;

			SquareMatrixT3<T> matrix;

			for (unsigned int n = 0u; n < 9u; ++n)
				matrix[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);

			const VectorT2<T> vector(RandomT<T>::scalar(randomGenerator, -valueRange, valueRange), RandomT<T>::scalar(randomGenerator, -valueRange, valueRange));

			VectorT3<T> testResult3;
			testResult3[0] = matrix(0, 0) * vector[0] + matrix(0, 1) * vector[1] + matrix(0, 2);
			testResult3[1] = matrix(1, 0) * vector[0] + matrix(1, 1) * vector[1] + matrix(1, 2);
			testResult3[2] = matrix(2, 0) * vector[0] + matrix(2, 1) * vector[1] + matrix(2, 2);

			const bool validNormalization = NumericT<T>::isNotEqualEps(testResult3[2]);

			// we test the multiplication operator

			if (validNormalization)
			{
				const VectorT2<T> testResult2(testResult3[0] / testResult3[2], testResult3[1] / testResult3[2]);

				const VectorT2<T> result(matrix * vector);

				if (NumericT<T>::isWeakEqual(testResult2[0], result[0]) && NumericT<T>::isWeakEqual(testResult2[1], result[1]))
				{
					localValidIterations++;
				}
			}

			// we test the multiplication function

			VectorT2<T> result;
			if (matrix.multiply(vector, result))
			{
				const VectorT2<T> testResult2(testResult3[0] / testResult3[2], testResult3[1] / testResult3[2]);

				if (validNormalization)
				{
					if (NumericT<T>::isWeakEqual(testResult2[0], result[0]) && NumericT<T>::isWeakEqual(testResult2[1], result[1]))
					{
						localValidIterations++;
					}
				}
			}
			else
			{
				if (!validNormalization)
				{
					localValidIterations++;
				}
			}

			if (localValidIterations == 2u)
			{
				validIterations++;
			}

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 2u) << "% succeeded.";

	return percent >= 0.999;
}

template <typename T>
bool TestSquareMatrix3::testVectorMultiplication3(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int constNumber = 100000u;

	Log::info() << "3D Vector multiplication test for " << sizeof(T) * 8 << "bit precision with " << String::insertCharacter(String::toAString(constNumber), ',', 3, false) << " repetitions:";

	bool allSucceeded = true;

	SquareMatrixT3<T> matrix;
	std::vector< VectorT3<T> > vectors(constNumber);
	std::vector< VectorT3<T> > results(constNumber);

	const T valueRange = std::is_same<T, double>::value ? T(100) : T(10);
	RandomGenerator randomGenerator;

	{
		// first we test the standard implementation not using any kind of explicit optimization

		HighPerformanceStatistic performanceStandard;
		const Timestamp startTimestamp(true);

		do
		{
			for (unsigned int n = 0u; n < 9u; ++n)
			{
				matrix[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
			}

			for (size_t n = 0; n < constNumber; ++n)
			{
				for (unsigned int i = 0u; i < 3u; ++i)
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

		uint64_t iterations = 0ull;
		uint64_t validIterations = 0ull;

		HighPerformanceStatistic performance;
		const Timestamp startTimestamp(true);

		do
		{
			for (unsigned int n = 0u; n < 9u; ++n)
			{
				matrix[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
			}

			for (size_t n = 0; n < constNumber; ++n)
			{
				for (unsigned int i = 0u; i < 3u; ++i)
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

				for (unsigned int i = 0u; i < 3u; ++i)
				{
					const T rowResult = matrix(i, 0) * vectors[n][0] +  matrix(i, 1) * vectors[n][1] +  matrix(i, 2) * vectors[n][2];

					if (NumericT<T>::isNotEqual(results[n][i], rowResult, NumericT<T>::eps() * (std::is_same<T, double>::value ? T(10) : T(100))))
					{
						localSucceeded = false;
					}
				}

				if (localSucceeded)
				{
					++validIterations;
				}

				++iterations;
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
			for (unsigned int n = 0u; n < 9u; ++n)
			{
				matrix[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
			}

			for (size_t n = 0; n < constNumber; ++n)
			{
				for (unsigned int i = 0u; i < 3u; ++i)
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

				for (unsigned int i = 0u; i < 3u; ++i)
				{
					const T rowResult = matrix(i, 0) * vectors[n][0] +  matrix(i, 1) * vectors[n][1] +  matrix(i, 2) * vectors[n][2];

					if (NumericT<T>::isNotEqual(results[n][i], rowResult, NumericT<T>::eps() * (std::is_same<T, double>::value ? T(10) : T(100))))
					{
						localSucceeded = false;
					}
				}

				if (localSucceeded)
				{
					++validIterations;
				}

				++iterations;
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

		uint64_t iterations = 0ull;
		uint64_t validIterations = 0ull;

		HighPerformanceStatistic performanceShared;
		const Timestamp startTimestamp(true);

		do
		{
			for (unsigned int n = 0u; n < 9u; ++n)
			{
				matrix[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
			}

			for (size_t n = 0; n < constNumber; ++n)
			{
				for (unsigned int i = 0u; i < 3u; ++i)
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

				for (unsigned int i = 0u; i < 3u; ++i)
				{
					const T rowResult = matrix(i, 0) * vectors[n][0] +  matrix(i, 1) * vectors[n][1] +  matrix(i, 2) * vectors[n][2];

					if (NumericT<T>::isNotEqual(results[n][i], rowResult, NumericT<T>::eps() * (std::is_same<T, double>::value ? T(10) : T(100))))
					{
						localSucceeded = false;
					}
				}

				if (localSucceeded)
				{
					++validIterations;
				}

				++iterations;
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

bool TestSquareMatrix3::testInvert(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SquareMatrix3::invert() and SquareMatrix3::inverted() test:";

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const Scalar epsilon = Numeric::eps() * 100;
	const SquareMatrix3 identity(true);

	const Timestamp startTimestamp(true);

	do
	{
		SquareMatrix3 matrix;

		for (unsigned int n = 0u; n < matrix.elements(); ++n)
		{
			matrix[n] = Random::scalar(-1, 1);
		}

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
				RandomI::random(2u, rowIndex0, rowIndex1);

				for (unsigned int c = 0u; c < 3u; ++c)
				{
					matrix(rowIndex0, c) = matrix(rowIndex1, c) * factor;
				}
			}
			else
			{
				unsigned int columnIndex0, columnIndex1;
				RandomI::random(2u, columnIndex0, columnIndex1);

				for (unsigned int r = 0u; r < 3u; ++r)
				{
					matrix(r, columnIndex0) = matrix(r, columnIndex1) * factor;
				}
			}
		}

		const bool matrixIsSingular = matrix.isSingular();
		ocean_assert((iterations % 2u) != 0u || matrixIsSingular);

		SquareMatrix3 invertedMatrix0(matrix);
		const bool matrixInverted0 = invertedMatrix0.invert();

		SquareMatrix3 invertedMatrix1;
		const bool matrixInverted1 = matrix.invert(invertedMatrix1);

		SquareMatrix3 invertedMatrix2;
		bool matrixInverted2 = false;

		if (matrixInverted0 || matrixInverted1)
		{
			invertedMatrix2 = matrix.inverted();
			matrixInverted2 = true;
		}

		bool localSucceeded = true;

		ocean_assert(matrixInverted0 == !matrixIsSingular);
		if (matrixInverted0 == matrixIsSingular)
		{
			localSucceeded = false;
		}

		ocean_assert(matrixInverted0 == matrixInverted1 && matrixInverted0 == matrixInverted2 && matrixInverted1 == matrixInverted2);
		if (matrixInverted0 != matrixInverted1 || matrixInverted0 != matrixInverted2 || matrixInverted1 != matrixInverted2)
		{
			localSucceeded = false;
		}

		if (matrixInverted0)
		{
			const SquareMatrix3 testMatrixA(matrix * invertedMatrix0);
			const SquareMatrix3 testMatrixB(invertedMatrix0 * matrix);

			if (!testMatrixA.isEqual(testMatrixB, epsilon))
			{
				localSucceeded = false;
			}

			if (!testMatrixA.isEqual(identity, epsilon))
			{
				localSucceeded = false;
			}

			if (!testMatrixB.isEqual(identity, epsilon))
			{
				localSucceeded = false;
			}
		}

		if (matrixInverted1)
		{
			const SquareMatrix3 testMatrixA(matrix * invertedMatrix1);
			const SquareMatrix3 testMatrixB(invertedMatrix1 * matrix);

			if (!testMatrixA.isEqual(testMatrixB, epsilon))
			{
				localSucceeded = false;
			}

			if (!testMatrixA.isEqual(identity, epsilon))
			{
				localSucceeded = false;
			}

			if (!testMatrixB.isEqual(identity, epsilon))
			{
				localSucceeded = false;
			}
		}

		if (matrixInverted2)
		{
			const SquareMatrix3 testMatrixA(matrix * invertedMatrix2);
			const SquareMatrix3 testMatrixB(invertedMatrix2 * matrix);

			if (!testMatrixA.isEqual(testMatrixB, epsilon))
			{
				localSucceeded = false;
			}

			if (!testMatrixA.isEqual(identity, epsilon))
			{
				localSucceeded = false;
			}

			if (!testMatrixB.isEqual(identity, epsilon))
			{
				localSucceeded = false;
			}
		}

		++iterations;

		if (localSucceeded)
		{
			++validIterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestSquareMatrix3::testMatrixConversion(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SquareMatrix3::matrices2matrices() test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int size = RandomI::random(1000u);

		std::vector<SquareMatrixD3> matricesD;
		std::vector<SquareMatrixF3> matricesF;

		for (size_t n = 0; n < size; ++n)
		{
			SquareMatrixD3 matrixD;
			SquareMatrixF3 matrixF;

			for (unsigned int i = 0u; i < 9u; ++i)
			{
				matrixD[i] = RandomD::scalar(-10, 10);
				matrixF[i] = RandomF::scalar(-10, 10);
			}

			matricesD.push_back(matrixD);
			matricesF.push_back(matrixF);
		}

		const std::vector<SquareMatrixD3> convertedD2D_0(SquareMatrixD3::matrices2matrices(matricesD));
		const std::vector<SquareMatrixD3> convertedD2D_1(SquareMatrixD3::matrices2matrices(matricesD.data(), matricesD.size()));

		const std::vector<SquareMatrixF3> convertedD2F_0(SquareMatrixF3::matrices2matrices(matricesD));
		const std::vector<SquareMatrixF3> convertedD2F_1(SquareMatrixF3::matrices2matrices(matricesD.data(), matricesD.size()));

		const std::vector<SquareMatrixD3> convertedF2D_0(SquareMatrixD3::matrices2matrices(matricesF));
		const std::vector<SquareMatrixD3> convertedF2D_1(SquareMatrixD3::matrices2matrices(matricesF.data(), matricesF.size()));

		const std::vector<SquareMatrixF3> convertedF2F_0(SquareMatrixF3::matrices2matrices(matricesF));
		const std::vector<SquareMatrixF3> convertedF2F_1(SquareMatrixF3::matrices2matrices(matricesF.data(), matricesF.size()));

		for (size_t n = 0; n < size; ++n)
		{
			for (unsigned int i = 0u; i < 9u; ++i)
			{
				if (NumericD::isNotWeakEqual(matricesD[n][i], convertedD2D_0[n][i]))
					allSucceeded = false;

				if (NumericD::isNotWeakEqual(matricesD[n][i], convertedD2D_1[n][i]))
					allSucceeded = false;

				if (NumericD::isNotWeakEqual(matricesD[n][i], double(convertedD2F_0[n][i])))
					allSucceeded = false;

				if (NumericD::isNotWeakEqual(matricesD[n][i], double(convertedD2F_1[n][i])))
					allSucceeded = false;


				if (NumericF::isNotWeakEqual(matricesF[n][i], convertedF2F_0[n][i]))
					allSucceeded = false;

				if (NumericF::isNotWeakEqual(matricesF[n][i], convertedF2F_1[n][i]))
					allSucceeded = false;

				if (NumericF::isNotWeakEqual(matricesF[n][i], float(convertedF2D_0[n][i])))
					allSucceeded = false;

				if (NumericF::isNotWeakEqual(matricesF[n][i], float(convertedF2D_1[n][i])))
					allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

bool TestSquareMatrix3::testSolve(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SquareMatrix3::solve() test:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testSolve(false, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testSolve(true, testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

bool TestSquareMatrix3::testSolve(const bool containsSignular, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	if (containsSignular)
		Log::info() << "... with singular matrices:";
	else
		Log::info() << "... without singular matrices:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;
	unsigned long long solvedIterations = 0ull;
	unsigned long long gaussSolvedIterations = 0ull;
	unsigned long long invalidNonSingularIterations = 0ull;

	const Scalar valueRange = std::is_same<float, Scalar>::value ? 10 : 100;
	const Scalar epsilon = Numeric::eps() * 100;

	RandomGenerator randomGenerator;
	const size_t repetitions = 1000u;

	SquareMatrices3 matrices(repetitions);
	Vectors3 xVectors(repetitions);
	Vectors3 bVectors(repetitions);
	Vectors3 trueVectors(repetitions);
	std::vector<unsigned char> solved(repetitions);

	double residual = 0.0;
	double residualGauss = 0.0;

	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);

	do
	{
		for (size_t n = 0 ; n < repetitions; ++n)
		{
			for (unsigned int i = 0u; i < matrices[n].elements(); ++i)
				matrices[n][i] = Random::scalar(randomGenerator, -valueRange, valueRange);

			if (containsSignular && (n % 50) == 0)
			{
				// we make some matrices singular: yAxis = a * xAxis + b * zAxis
				(Vector3&)*(matrices[n]() + 3) = matrices[n].xAxis() * Random::scalar(randomGenerator, -valueRange, valueRange) + matrices[n].zAxis() * Random::scalar(randomGenerator, -valueRange, valueRange);
			}

			trueVectors[n] = Vector3(Random::scalar(randomGenerator, -valueRange, valueRange), Random::scalar(randomGenerator, -valueRange, valueRange), Random::scalar(randomGenerator, -valueRange, valueRange));
			bVectors[n] = matrices[n] * trueVectors[n];
		}

		performance.start();

		for (size_t n = 0 ; n < repetitions; ++n)
			solved[n] = (unsigned char)matrices[n].solve(bVectors[n], xVectors[n]);

		performance.stop();

		for (size_t n = 0 ; n < repetitions; ++n)
		{
			if (solved[n])
			{
				if (trueVectors[n].isEqual(xVectors[n], epsilon))
					validIterations++;

				residual += double((trueVectors[n] - xVectors[n]).length());
				solvedIterations++;

				Vector3 xGauss;
				if (solveGauss(matrices[n], bVectors[n], xGauss))
				{
					residualGauss += double((trueVectors[n] - xGauss).length());
					gaussSolvedIterations++;
				}

			}
			else if (!matrices[n].isSingular())
				invalidNonSingularIterations++;

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Performance: " << performance.averageMseconds() * 1000.0 / double(repetitions) << "mys";
	Log::info() << "Average residual: " << String::toAStringScientific(NumericD::ratio(residual, double(solvedIterations))) << ", (Gauss has: " << String::toAStringScientific(NumericD::ratio(residualGauss, double(gaussSolvedIterations))) << ")";
	Log::info() << "Not solved non-singular: " << String::toAString(double(invalidNonSingularIterations) / double(iterations) * 100.0, 1u) << "%";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	if (containsSignular)
		return percent >= 0.90;
	else
		return percent >= 0.99;
}

template <typename T>
bool TestSquareMatrix3::solveGauss(const SquareMatrixT3<T>& A, const VectorT3<T>& b, VectorT3<T>& x)
{
	// Solve this system of linear equations using the good ol' Gauss algorithm
	//
	// Step 0:
	//
	// |a0 a3 a6|   |x0| = |b0|
	// |a1 a4 a7| x |x1| = |b1|
	// |a2 a5 a8|   |x2| = |b2|
	//
	//
	// Step 1:
	//
	// |a0 a1 a2|   |x0| = |b0|
	// | 0 u0 v0| x |x1| = |w0|  <- (row1 * a0) - (row0 * a1)
	// | 0 u1 v1|   |x2| = |w1|  <- (row2 * a0) - (row0 * a2)
	//
	const T u0 = (A[4] * A[0]) - (A[3] * A[1]);
	const T u1 = (A[5] * A[0]) - (A[3] * A[2]);
	const T v0 = (A[7] * A[0]) - (A[6] * A[1]);
	const T v1 = (A[8] * A[0]) - (A[6] * A[2]);
	const T w0 = (b[1] * A[0]) - (b[0] * A[1]);
	const T w1 = (b[2] * A[0]) - (b[0] * A[2]);

	// Step 2:
	//
	// |a0 a1 a2| |x0| = |b0|
	// | 0 u0 v0| |x1| = |w0|
	// | 0  0 v2| |x2| = |w2|  <- (row2 * u0) - (row1 * u1)
	//
	const T v2 = (v1 * u0) - (v0 * u1);
	const T w2 = (w1 * u0) - (w0 * u1);

	// Then solve.
	if (NumericT<T>::isNotEqualEps(v2) && NumericT<T>::isNotEqualEps(u0) && NumericT<T>::isNotEqualEps(A[0]))
	{
		x[2] = w2 / v2;
		x[1] = (w0 - (v0 * x[2])) / u0;
		x[0] = (b[0] - (A[3] * x[1]) - (A[6] * x[2])) / A[0];

		return true;
	}

	return false;
}

}

}

}
