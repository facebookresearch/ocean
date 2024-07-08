/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestSquareMatrix2.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/Random.h"
#include "ocean/math/SquareMatrix2.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestSquareMatrix2::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   SquareMatrix2 test:   ---";
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

	allSucceeded = testInvert(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMatrixConversion(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testEigenConstructor(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testEigenSystem() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSolve(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "SquareMatrix2 test succeeded.";
	}
	else
	{
		Log::info() << "SquareMatrix2 test FAILED.";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestSquareMatrix2, WriteToMessenger)
{
	EXPECT_TRUE(TestSquareMatrix2::testWriteToMessenger());
}

TEST(TestSquareMatrix2, ElementConstructor)
{
	EXPECT_TRUE(TestSquareMatrix2::testElementConstructor(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix2, Invert)
{
	EXPECT_TRUE(TestSquareMatrix2::testInvert(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix2, MatrixConversion)
{
	EXPECT_TRUE(TestSquareMatrix2::testMatrixConversion(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix2, EigenConstructor)
{
	EXPECT_TRUE(TestSquareMatrix2::testEigenConstructor(GTEST_TEST_DURATION));
}

TEST(TestSquareMatrix2, EigenSystem)
{
	EXPECT_TRUE(TestSquareMatrix2::testEigenSystem());
}

TEST(TestSquareMatrix2, Solve)
{
	EXPECT_TRUE(TestSquareMatrix2::testSolve(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestSquareMatrix2::testWriteToMessenger()
{
	Log::info() << "Write to messenger test:";

	// this is mainly a check whether the code does not compile or crash

	Log::info() << " ";

	Log::info() << SquareMatrix2(true);
	Log::info() << " ";
	Log::info() << "Matrix: " << SquareMatrix2(true);
	Log::info() << " ";
	Log::info() << SquareMatrix2(true) << " <- Matrix";

	Log::info() << " ";
	Log::info() << "Validation succeeded.";

	return true;
}

bool TestSquareMatrix2::testElementConstructor(const double testDuration)
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
			float floatValues[4];
			double doubleValues[4];
			Scalar scalarValues[4];

			for (unsigned int i = 0u; i < 4u; ++i)
			{
				doubleValues[i] = RandomT<double>::scalar(randomGenerator, -100, 100);
				floatValues[i] = float(doubleValues[i]);
				scalarValues[i] = Scalar(doubleValues[i]);
			}

			const SquareMatrixT2<float> floatMatrixA(floatValues);
			const SquareMatrixT2<float> floatMatrixB(floatValues, false);
			const SquareMatrixT2<float> floatMatrixBTransposed(floatValues, true);

			const SquareMatrixT2<float> floatMatrixC(doubleValues);
			const SquareMatrixT2<float> floatMatrixD(doubleValues, false);
			const SquareMatrixT2<float> floatMatrixDTransposed(doubleValues, true);


			const SquareMatrixT2<double> doubleMatrixA(floatValues);
			const SquareMatrixT2<double> doubleMatrixB(floatValues, false);
			const SquareMatrixT2<double> doubleMatrixBTransposed(floatValues, true);

			const SquareMatrixT2<double> doubleMatrixC(doubleValues);
			const SquareMatrixT2<double> doubleMatrixD(doubleValues, false);
			const SquareMatrixT2<double> doubleMatrixDTransposed(doubleValues, true);


			const SquareMatrix2 scalarMatrixA(floatValues);
			const SquareMatrix2 scalarMatrixB(floatValues, false);
			const SquareMatrix2 scalarMatrixBTransposed(floatValues, true);

			const SquareMatrix2 scalarMatrixC(doubleValues);
			const SquareMatrix2 scalarMatrixD(doubleValues, false);
			const SquareMatrix2 scalarMatrixDTransposed(doubleValues, true);


			SquareMatrixT2<float> floatTest, floatTestTransposed;
			SquareMatrixT2<double> doubleTest, doubleTestTransposed;
			SquareMatrix2 scalarTest, scalarTestTransposed;

			unsigned int index = 0u;
			for (unsigned int c = 0u; c < 2u; ++c)
				for (unsigned int r = 0u; r < 2u; ++r)
				{
					floatTest(r, c) = floatValues[index];
					doubleTest(r, c) = doubleValues[index];
					scalarTest(r, c) = scalarValues[index];

					floatTestTransposed(c, r) = floatValues[index];
					doubleTestTransposed(c, r) = doubleValues[index];
					scalarTestTransposed(c, r) = scalarValues[index];

					index++;
				}

			ocean_assert(index == 4u);

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

bool TestSquareMatrix2::testInvert(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SquareMatrix2::invert() and SquareMatrix2::inverted() test:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Scalar epsilon = Numeric::eps() * 100;
	const SquareMatrix2 identity(true);

	const Timestamp startTimestamp(true);

	do
	{
		SquareMatrix2 matrix;

		for (unsigned int n = 0u; n < matrix.elements(); ++n)
			matrix[n] = Random::scalar(-1, 1);

		// we create a singular value each second iteration
		if (iterations % 2u == 0u)
		{
			Scalar factor = Random::scalar(-1, 1);
			while (Numeric::isWeakEqualEps(factor))
				factor = Random::scalar(-1, 1);

			if (RandomI::random(1u) == 0u)
			{
				const unsigned int rowIndex0 = RandomI::random(1u);
				const unsigned int rowIndex1 = 1u - rowIndex0;

				for (unsigned int c = 0u; c < 2u; ++c)
					matrix(rowIndex0, c) = matrix(rowIndex1, c) * factor;
			}
			else
			{
				const unsigned int columnIndex0 = RandomI::random(1u);
				const unsigned int columnIndex1 = 1u - columnIndex0;

				for (unsigned int r = 0u; r < 2u; ++r)
					matrix(r, columnIndex0) = matrix(r, columnIndex1) * factor;
			}
		}

		const bool matrixIsSingular = matrix.isSingular();
		ocean_assert((iterations % 2u) != 0u || matrixIsSingular);

		SquareMatrix2 invertedMatrix0(matrix);
		const bool matrixInverted0 = invertedMatrix0.invert();

		SquareMatrix2 invertedMatrix1;
		const bool matrixInverted1 = matrix.invert(invertedMatrix1);

		SquareMatrix2 invertedMatrix2;
		bool matrixInverted2 = false;

		if (matrixInverted0 || matrixInverted1)
		{
			invertedMatrix2 = matrix.inverted();
			matrixInverted2 = true;
		}

		bool localSucceeded = true;

		ocean_assert(matrixInverted0 == !matrixIsSingular);
		if (matrixInverted0 == matrixIsSingular)
			localSucceeded = false;

		ocean_assert(matrixInverted0 == matrixInverted1 && matrixInverted0 == matrixInverted2 && matrixInverted1 == matrixInverted2);
		if (matrixInverted0 != matrixInverted1 || matrixInverted0 != matrixInverted2 || matrixInverted1 != matrixInverted2)
			localSucceeded = false;

		if (matrixInverted0)
		{
			const SquareMatrix2 testMatrixA(matrix * invertedMatrix0);
			const SquareMatrix2 testMatrixB(invertedMatrix0 * matrix);

			if (!testMatrixA.isEqual(testMatrixB, epsilon))
				localSucceeded = false;

			if (!testMatrixA.isEqual(identity, epsilon))
				localSucceeded = false;

			if (!testMatrixB.isEqual(identity, epsilon))
				localSucceeded = false;
		}

		if (matrixInverted1)
		{
			const SquareMatrix2 testMatrixA(matrix * invertedMatrix1);
			const SquareMatrix2 testMatrixB(invertedMatrix1 * matrix);

			if (!testMatrixA.isEqual(testMatrixB, epsilon))
				localSucceeded = false;

			if (!testMatrixA.isEqual(identity, epsilon))
				localSucceeded = false;

			if (!testMatrixB.isEqual(identity, epsilon))
				localSucceeded = false;
		}

		if (matrixInverted2)
		{
			const SquareMatrix2 testMatrixA(matrix * invertedMatrix2);
			const SquareMatrix2 testMatrixB(invertedMatrix2 * matrix);

			if (!testMatrixA.isEqual(testMatrixB, epsilon))
				localSucceeded = false;

			if (!testMatrixA.isEqual(identity, epsilon))
				localSucceeded = false;

			if (!testMatrixB.isEqual(identity, epsilon))
				localSucceeded = false;
		}

		iterations++;

		if (localSucceeded)
			validIterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestSquareMatrix2::testMatrixConversion(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SquareMatrix2::matrices2matrices() test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int size = RandomI::random(1000u);

		std::vector<SquareMatrixD2> matricesD;
		std::vector<SquareMatrixF2> matricesF;

		for (size_t n = 0; n < size; ++n)
		{
			SquareMatrixD2 matrixD;
			SquareMatrixF2 matrixF;

			for (unsigned int i = 0u; i < 4u; ++i)
			{
				matrixD[i] = RandomD::scalar(-10, 10);
				matrixF[i] = RandomF::scalar(-10, 10);
			}

			matricesD.push_back(matrixD);
			matricesF.push_back(matrixF);
		}

		const std::vector<SquareMatrixD2> convertedD2D_0(SquareMatrixD2::matrices2matrices(matricesD));
		const std::vector<SquareMatrixD2> convertedD2D_1(SquareMatrixD2::matrices2matrices(matricesD.data(), matricesD.size()));

		const std::vector<SquareMatrixF2> convertedD2F_0(SquareMatrixF2::matrices2matrices(matricesD));
		const std::vector<SquareMatrixF2> convertedD2F_1(SquareMatrixF2::matrices2matrices(matricesD.data(), matricesD.size()));

		const std::vector<SquareMatrixD2> convertedF2D_0(SquareMatrixD2::matrices2matrices(matricesF));
		const std::vector<SquareMatrixD2> convertedF2D_1(SquareMatrixD2::matrices2matrices(matricesF.data(), matricesF.size()));

		const std::vector<SquareMatrixF2> convertedF2F_0(SquareMatrixF2::matrices2matrices(matricesF));
		const std::vector<SquareMatrixF2> convertedF2F_1(SquareMatrixF2::matrices2matrices(matricesF.data(), matricesF.size()));

		for (size_t n = 0; n < size; ++n)
		{
			for (unsigned int i = 0u; i < 4u; ++i)
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

bool TestSquareMatrix2::testEigenConstructor(double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SquareMatrix2 constructor from Eigen system test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		const Vector2 randomVector = Vector2(Random::gaussianNoise(1), Random::gaussianNoise(1));

		const Scalar vectorLength = randomVector.length();

		if (Numeric::isEqualEps(vectorLength))
		{
			continue;
		}

		const Vector2 eigenVector0 = randomVector / vectorLength;
		const Vector2 eigenVector1(eigenVector0.perpendicular());
		const Scalar eigenValue0 = Numeric::sqr(vectorLength);
		const Scalar eigenValue1 = Numeric::sqr(vectorLength * Random::scalar(0.0001, 0.9999));

		const SquareMatrix2 mat(eigenValue0, eigenValue1, eigenVector0, eigenVector1);

		// Check that the matrix values are all valid
		for (unsigned int i = 0u; i < 2u; ++i)
		{
			for (unsigned int j = 0u; j < 2u; ++j)
			{
				if (Numeric::isNan(mat(i,j)) || Numeric::isInf(mat(i,j)))
				{
					allSucceeded = false;
				}
			}
		}

		// Check that the matrix is symmetric
		if (Numeric::isNotEqual(mat(0,1), mat(1,0)))
		{
			allSucceeded = false;
			Log::error() << "mat(0,1): " << mat(0,1) << ", mat(1,0): " << mat(1,0);
		}

		// Ensure resulting matrix values match the results of the old algorithm except for cases where the old algorithm is known to have been numerically unstable
		const Scalar a = (eigenVector0.y() * eigenValue1 * eigenVector1.x() - eigenVector1.y() * eigenValue0 * eigenVector0.x()) / (eigenVector1.x() * eigenVector0.y() - eigenVector0.x() * eigenVector1.y());
		const Scalar c = (eigenValue1 * eigenVector1.x() - a * eigenVector1.x()) / eigenVector1.y();

		const Scalar b = (eigenValue1 * eigenVector1.y() * eigenVector0.y() - eigenValue0 * eigenVector0.y() * eigenVector1.y()) / (eigenVector1.x() * eigenVector0.y() - eigenVector0.x() * eigenVector1.y());
		const Scalar d = (eigenValue1 * eigenVector1.y() - b * eigenVector1.x()) / eigenVector1.y();

		const bool match00 = Numeric::isWeakEqual(a, mat(0, 0));
		const bool match01 = Numeric::isWeakEqual(b, mat(0, 1));
		const bool match10 = Numeric::isWeakEqual(c, mat(1, 0));
		const bool match11 = Numeric::isWeakEqual(d, mat(1, 1));

		bool match;
		if (Numeric::isWeakEqualEps(eigenVector1.y()))
		{
			// Dividing by zero or near-zero values is numerically unstable, so ignore those values
			match = match00 && match01;
		}
		else
		{
			// Allow for one off-diagonal mismatch, since the old algorithm is not always symmetric
			match = match00 && (match01 || match10) && match11;
		}

		if (!match)
		{
			allSucceeded = false;
			Log::error() << "Mismatch between old and new algorithm. New: " << mat << ", Old: " << SquareMatrix2(a, b, c, d);
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

bool TestSquareMatrix2::testEigenSystem()
{
	Log::info() << "SquareMatrix2::eigenSystem() test:";

	bool allSucceeded = true;

	Scalar eigenValue0, eigenValue1;
	Vector2 eigenVector0, eigenVector1;
	Vector2 testVector0, testVector1;
	SquareMatrix2 matrix;

	// 1 0
	// 0 1
	matrix = SquareMatrix2(1, 0, 0, 1);
	if (!matrix.eigenSystem(eigenValue0, eigenValue1, eigenVector0, eigenVector1))
		allSucceeded = false;

	testVector0 = Vector2(1, 0);
	testVector1 = Vector2(0, 1);
	if (Numeric::isNotEqual(eigenValue0, 1) || Numeric::isNotEqual(eigenValue1, 1) || (eigenVector0 != testVector0 && eigenVector0 != -testVector0) || (eigenVector1 != testVector1 && eigenVector1 != -testVector1))
		allSucceeded = false;

	//  0  1
	// -2 -3
	matrix = SquareMatrix2(0, -2, 1, -3);
	if (!matrix.eigenSystem(eigenValue0, eigenValue1, eigenVector0, eigenVector1))
		allSucceeded = false;

	testVector0 = Vector2(1 / Numeric::sqrt(2), -1 / Numeric::sqrt(2));
	testVector1 = Vector2(-1 / Numeric::sqrt(5), 2 / Numeric::sqrt(5));
	if (Numeric::isNotEqual(eigenValue0, -1) || Numeric::isNotEqual(eigenValue1, -2) || (eigenVector0 != testVector0 && eigenVector0 != -testVector0) || (eigenVector1 != testVector1 && eigenVector1 != -testVector1))
		allSucceeded = false;

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

bool TestSquareMatrix2::testSolve(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SquareMatrix2::solve() test:";

	const Scalar valueRange = std::is_same<float, Scalar>::value ? 10 : 100;

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const SquareMatrix2 matrixA(Random::scalar(-valueRange, valueRange), Random::scalar(-valueRange, valueRange), Random::scalar(-valueRange, valueRange), Random::scalar(-valueRange, valueRange));
			const Vector2 trueX = Random::vector2(-valueRange, valueRange);
			const Vector2 b = matrixA * trueX;

			Vector2 x;
			const bool solved = matrixA.solve(b, x);

			if (solved && trueX.isEqual(x, Numeric::eps() * 100))
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

}

}

}
