// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/test/testmath/TestHomogenousMatrix4.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Random.h"
#include "ocean/math/Rotation.h"
#include "ocean/math/SquareMatrix4.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestHomogenousMatrix4::test(const double testDuration)
{
	Log::info() << "---   HomogenousMatrix4 test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testWriteToMessenger() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConstructor(testDuration) && allSucceeded;

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

	allSucceeded = testDecomposition(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testVectorConversion(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCopyElements(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "HomogenousMatrix4 test succeeded.";
	}
	else
	{
		Log::info() << "HomogenousMatrix4 test FAILED.";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestHomogenousMatrix4, WriteToMessenger)
{
	EXPECT_TRUE(TestHomogenousMatrix4::testWriteToMessenger());
}

TEST(TestHomogenousMatrix4, Constructor)
{
	EXPECT_TRUE(TestHomogenousMatrix4::testConstructor(GTEST_TEST_DURATION));
}

TEST(TestHomogenousMatrix4, ElementConstructor)
{
	EXPECT_TRUE(TestHomogenousMatrix4::testElementConstructor(GTEST_TEST_DURATION));
}

TEST(TestHomogenousMatrix4, Invert)
{
	EXPECT_TRUE(TestHomogenousMatrix4::testInvert(GTEST_TEST_DURATION));
}

TEST(TestHomogenousMatrix4, Decomposition)
{
	EXPECT_TRUE(TestHomogenousMatrix4::testDecomposition(GTEST_TEST_DURATION));
}

TEST(TestHomogenousMatrix4, VectorConversion)
{
	EXPECT_TRUE(TestHomogenousMatrix4::testVectorConversion(GTEST_TEST_DURATION));
}

TEST(TestHomogenousMatrix4, CopyElements)
{
	EXPECT_TRUE(TestHomogenousMatrix4::testCopyElements(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestHomogenousMatrix4::testWriteToMessenger()
{
	Log::info() << "Write to messenger test:";

	// this is mainly a check whether the code does not compile or crash

	Log::info() << " ";

	Log::info() << HomogenousMatrix4(true);
	Log::info() << " ";
	Log::info() << "Matrix: " << HomogenousMatrix4(true);
	Log::info() << " ";
	Log::info() << HomogenousMatrix4(true) << " <- Matrix";

	Log::info() << " ";
	Log::info() << "Validation succeeded.";

	return true;
}

bool TestHomogenousMatrix4::testConstructor(const double testDuration)
{
	const size_t size = 1000000;

	Log::info() << "Constructor test for " << String::insertCharacter(String::toAString(size), ',', 3, false) << " matrices:";

	HomogenousMatrices4 matrices0(size);
	HomogenousMatrices4 matrices1(size);
	HomogenousMatrices4 matrices2(size);

	HighPerformanceStatistic performanceNormal, performanceOne, performanceZero;

	Timestamp startTimestamp(true);
	do
	{
		performanceNormal.start();
		for (HomogenousMatrices4::iterator i = matrices0.begin(); i != matrices0.end(); ++i)
			*i = HomogenousMatrix4();
		performanceNormal.stop();

		performanceOne.start();
		for (HomogenousMatrices4::iterator i = matrices1.begin(); i != matrices1.end(); ++i)
			*i = HomogenousMatrix4(true);
		performanceOne.stop();

		performanceZero.start();
		for (HomogenousMatrices4::iterator i = matrices2.begin(); i != matrices2.end(); ++i)
			*i = HomogenousMatrix4(false);
		performanceZero.stop();
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Uninitialized performance: " << performanceNormal.bestMseconds() << "ms - " << performanceNormal.worstMseconds() << "ms";
	Log::info() << "Identity matrix performance: " << performanceOne.bestMseconds() << "ms - " << performanceOne.worstMseconds() << "ms";
	Log::info() << "Zero matrix performance: " << performanceZero.bestMseconds() << "ms - " << performanceZero.worstMseconds() << "ms";

	bool allSucceeded = true;

	startTimestamp.toNow();
	do
	{
		const Vector3 random(Random::vector3(-1000, 1000));
		const Vector3 result(HomogenousMatrix4(true) * random);

		if (random != result)
			allSucceeded = false;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	startTimestamp.toNow();
	do
	{
		const Vector3 random(Random::vector3(-1000, 1000));
		const Vector3 result(HomogenousMatrix4(Vector3(0, 0, 0), SquareMatrix3(false)) * random);

		if (!result.isNull())
			allSucceeded = false;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

bool TestHomogenousMatrix4::testElementConstructor(const double testDuration)
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

			const HomogenousMatrixT4<float> floatMatrixA(floatValues);
			const HomogenousMatrixT4<float> floatMatrixB(floatValues, false);
			const HomogenousMatrixT4<float> floatMatrixBTransposed(floatValues, true);

			const HomogenousMatrixT4<float> floatMatrixC(doubleValues);
			const HomogenousMatrixT4<float> floatMatrixD(doubleValues, false);
			const HomogenousMatrixT4<float> floatMatrixDTransposed(doubleValues, true);


			const HomogenousMatrixT4<double> doubleMatrixA(floatValues);
			const HomogenousMatrixT4<double> doubleMatrixB(floatValues, false);
			const HomogenousMatrixT4<double> doubleMatrixBTransposed(floatValues, true);

			const HomogenousMatrixT4<double> doubleMatrixC(doubleValues);
			const HomogenousMatrixT4<double> doubleMatrixD(doubleValues, false);
			const HomogenousMatrixT4<double> doubleMatrixDTransposed(doubleValues, true);


			const HomogenousMatrix4 scalarMatrixA(floatValues);
			const HomogenousMatrix4 scalarMatrixB(floatValues, false);
			const HomogenousMatrix4 scalarMatrixBTransposed(floatValues, true);

			const HomogenousMatrix4 scalarMatrixC(doubleValues);
			const HomogenousMatrix4 scalarMatrixD(doubleValues, false);
			const HomogenousMatrix4 scalarMatrixDTransposed(doubleValues, true);


			HomogenousMatrixT4<float> floatTest, floatTestTransposed;
			HomogenousMatrixT4<double> doubleTest, doubleTestTransposed;
			HomogenousMatrix4 scalarTest, scalarTestTransposed;

			unsigned int index = 0u;
			for (unsigned int c = 0u; c < 4u; ++c)
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
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

bool TestHomogenousMatrix4::testInvert(const double testDuration)
{
	const size_t size = 1000000;

	Log::info() << "Invert test for " << String::insertCharacter(String::toAString(size), ',', 3, false) << " matrices:";

	HomogenousMatrices4 matrices(size, HomogenousMatrix4(true));

	HighPerformanceStatistic performance;

	Timestamp startTimestamp(true);
	do
	{
		performance.start();
		for (HomogenousMatrices4::iterator i = matrices.begin(); i != matrices.end(); ++i)
			i->invert();
		performance.stop();
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << performance.bestMseconds() << "ms - " << performance.firstMseconds() << "ms";

	unsigned long long iterations = 0ull;
	unsigned long long succeeded = 0ull;

	const HomogenousMatrix4 entity(true);

	startTimestamp.toNow();
	do
	{
		bool localSucceeded = true;

		const Vector3 translation(Random::vector3(-100, 100));
		const Rotation rotation(Random::rotation());
		const Vector3 scale(Random::vector3(Scalar(0.001), 100));

		const HomogenousMatrix4 transform(translation, rotation, scale);

		{
			const HomogenousMatrix4 invertedTransform(transform.inverted());

			const HomogenousMatrix4 result0(transform * invertedTransform);
			const HomogenousMatrix4 result1(invertedTransform * transform);

			for (unsigned int n = 0u; n < 16u; ++n)
				if (!result0.isValid() || !result1.isValid() || Numeric::isNotWeakEqual(entity[n], result0[n]) || Numeric::isNotWeakEqual(entity[n], result1[n]))
					localSucceeded = false;
		}

		{
			HomogenousMatrix4 invertedTransform(transform);
			invertedTransform.invert();

			const HomogenousMatrix4 result0(transform * invertedTransform);
			const HomogenousMatrix4 result1(invertedTransform * transform);

			for (unsigned int n = 0u; n < 16u; ++n)
				if (!result0.isValid() || !result1.isValid() || Numeric::isNotWeakEqual(entity[n], result0[n]) || Numeric::isNotWeakEqual(entity[n], result1[n]))
					localSucceeded = false;
		}

		{
			HomogenousMatrix4 invertedTransform;
			transform.invert(invertedTransform);

			const HomogenousMatrix4 result0(transform * invertedTransform);
			const HomogenousMatrix4 result1(invertedTransform * transform);

			for (unsigned int n = 0u; n < 16u; ++n)
				if (!result0.isValid() || !result1.isValid() || Numeric::isNotWeakEqual(entity[n], result0[n]) || Numeric::isNotWeakEqual(entity[n], result1[n]))
					localSucceeded = false;
		}

		iterations++;

		if (localSucceeded)
			succeeded++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.95;
}

bool TestHomogenousMatrix4::testDecomposition(const double testDuration)
{
	const size_t size = 1000;

	Log::info() << "Decomposition test for " << String::insertCharacter(String::toAString(size), ',', 3, false) << " matrices:";

	HighPerformanceStatistic performance;

	Timestamp startTimestamp(true);
	do
	{
		HomogenousMatrices4 matrices;
		matrices.reserve(size);

		for (size_t n = 0; n < size; ++n)
		{
			const Vector3 translation(Random::vector3() * 10);
			const Quaternion rotation(Random::quaternion());
			const Vector3 scale(Random::vector3(Scalar(0.1), Scalar(5)));
			const Vector3 shear(Random::vector3(Scalar(0), Scalar(2)));

			const HomogenousMatrix4 matrix(translation, rotation, scale, shear);
			ocean_assert(matrix.isValid());

			matrices.push_back(matrix);
		}

		Vector3 dTranslation;
		Quaternion dRotation;
		Vector3 dScale;
		Vector3 dShear;

		performance.start();

		// dummy value to ensure that the code is not optimized
		Scalar value = 0;

		for (size_t n = 0; n < size; ++n)
		{
			matrices[n].decompose(dTranslation, dRotation, dScale, dShear);
			value += dTranslation[0];
		}

		if (value == 0)
			performance.stop();
		else
			performance.stop();
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << performance.bestMseconds() << "ms - " << performance.firstMseconds() << "ms";

	unsigned long long iterations = 0ull;
	unsigned long long succeeded = 0ull;

	startTimestamp.toNow();
	do
	{
		const Vector3 translation(Random::vector3() * 10);
		const Quaternion rotation(Random::quaternion());
		const Vector3 scale(Random::vector3(Scalar(0.1), Scalar(5)));
		const Vector3 shear(Random::vector3(Scalar(0), Scalar(2)));

		const HomogenousMatrix4 matrix(translation, rotation, scale, shear);
		ocean_assert(matrix.isValid());

		Vector3 dTranslation;
		Quaternion dRotation;
		Vector3 dScale;
		Vector3 dShear;

		if (matrix.decompose(dTranslation, dRotation, dScale, dShear))
		{
			const bool bTranslation = dTranslation == translation;
			const bool bRotation = dRotation == rotation;
			const bool bScale = dScale == scale;
			const bool bShear = dShear == shear;

			if (bTranslation && bRotation && bScale && bShear)
				succeeded++;
			else
			{
				const HomogenousMatrix4 testMatrix(dTranslation, dRotation, dScale, dShear);

				bool localSucceeded = true;

				for (unsigned int n = 0u; n < 16u; ++n)
					if (Numeric::isNotWeakEqual(testMatrix[n], matrix[n]))
						localSucceeded = false;

				if (localSucceeded)
					succeeded++;
			}
		}

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.95;
}

bool TestHomogenousMatrix4::testVectorConversion(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "HomogenousMatrix4::matrices2matrices() test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int size = RandomI::random(1000u);

		std::vector<HomogenousMatrixD4> matricesD;
		std::vector<HomogenousMatrixF4> matricesF;

		for (size_t n = 0; n < size; ++n)
		{
			HomogenousMatrixD4 matrixD;
			HomogenousMatrixF4 matrixF;

			for (unsigned int i = 0u; i < 16u; ++i)
			{
				matrixD[i] = RandomD::scalar(-10, 10);
				matrixF[i] = RandomF::scalar(-10, 10);
			}

			matricesD.push_back(matrixD);
			matricesF.push_back(matrixF);
		}

		const std::vector<HomogenousMatrixD4> convertedD2D_0(HomogenousMatrixD4::matrices2matrices(matricesD));
		const std::vector<HomogenousMatrixD4> convertedD2D_1(HomogenousMatrixD4::matrices2matrices(matricesD.data(), matricesD.size()));

		const std::vector<HomogenousMatrixF4> convertedD2F_0(HomogenousMatrixF4::matrices2matrices(matricesD));
		const std::vector<HomogenousMatrixF4> convertedD2F_1(HomogenousMatrixF4::matrices2matrices(matricesD.data(), matricesD.size()));

		const std::vector<HomogenousMatrixD4> convertedF2D_0(HomogenousMatrixD4::matrices2matrices(matricesF));
		const std::vector<HomogenousMatrixD4> convertedF2D_1(HomogenousMatrixD4::matrices2matrices(matricesF.data(), matricesF.size()));

		const std::vector<HomogenousMatrixF4> convertedF2F_0(HomogenousMatrixF4::matrices2matrices(matricesF));
		const std::vector<HomogenousMatrixF4> convertedF2F_1(HomogenousMatrixF4::matrices2matrices(matricesF.data(), matricesF.size()));

		for (size_t n = 0; n < size; ++n)
		{
			for (unsigned int i = 0u; i < 16u; ++i)
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

bool TestHomogenousMatrix4::testCopyElements(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "HomogenousMatrix4::copyElements() test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		HomogenousMatrix4 matrix(true);
		HomogenousMatrixF4 matrixF(true);
		HomogenousMatrixD4 matrixD(true);

		for (unsigned int r = 0u; r < 3u; ++r)
		{
			for (unsigned int c = 0u; c < 4u; ++c)
			{
				const float value = RandomF::scalar(-10, 10);

				matrix(r, c) = Scalar(value);
				matrixF(r, c) = value;
				matrixD(r, c) = double(value);
			}
		}

		{
			// testing copyElements() of matrix (based on Scalar):

			Scalar columnAlignedData[16];
			float columnAlignedDataF[16];
			double columnAlignedDataD[16];
			matrix.copyElements(columnAlignedData);
			matrix.copyElements(columnAlignedDataF);
			matrix.copyElements(columnAlignedDataD);

			if (HomogenousMatrix4(columnAlignedData) != matrix)
			{
				return false;
			}

			if (HomogenousMatrixF4(columnAlignedDataF) != matrixF)
			{
				return false;
			}

			if (HomogenousMatrixD4(columnAlignedDataD) != matrixD)
			{
				return false;
			}

			Scalar rowAlignedData[16];
			float rowAlignedDataF[16];
			double rowAlignedDataD[16];
			matrix.copyElements(rowAlignedData, true);
			matrix.copyElements(rowAlignedDataF, true);
			matrix.copyElements(rowAlignedDataD, true);

			if (SquareMatrix4(rowAlignedData).transposed() != SquareMatrix4(matrix))
			{
				return false;
			}

			if (SquareMatrixF4(rowAlignedDataF).transposed() != SquareMatrixF4(matrixF))
			{
				return false;
			}

			if (SquareMatrixD4(rowAlignedDataD).transposed() != SquareMatrixD4(matrixD))
			{
				return false;
			}
		}

		{
			// testing copyElements() of matrixF (based on float):

			Scalar columnAlignedData[16];
			float columnAlignedDataF[16];
			double columnAlignedDataD[16];
			matrixF.copyElements(columnAlignedData);
			matrixF.copyElements(columnAlignedDataF);
			matrixF.copyElements(columnAlignedDataD);

			if (HomogenousMatrix4(columnAlignedData) != matrix)
			{
				return false;
			}

			if (HomogenousMatrixF4(columnAlignedDataF) != matrixF)
			{
				return false;
			}

			if (HomogenousMatrixD4(columnAlignedDataD) != matrixD)
			{
				return false;
			}

			Scalar rowAlignedData[16];
			float rowAlignedDataF[16];
			double rowAlignedDataD[16];
			matrixF.copyElements(rowAlignedData, true);
			matrixF.copyElements(rowAlignedDataF, true);
			matrixF.copyElements(rowAlignedDataD, true);

			if (SquareMatrix4(rowAlignedData).transposed() != SquareMatrix4(matrix))
			{
				return false;
			}

			if (SquareMatrixF4(rowAlignedDataF).transposed() != SquareMatrixF4(matrixF))
			{
				return false;
			}

			if (SquareMatrixD4(rowAlignedDataD).transposed() != SquareMatrixD4(matrixD))
			{
				return false;
			}
		}

		{
			// testing copyElements() of matrixD (based on double):

			Scalar columnAlignedData[16];
			float columnAlignedDataF[16];
			double columnAlignedDataD[16];
			matrixD.copyElements(columnAlignedData);
			matrixD.copyElements(columnAlignedDataF);
			matrixD.copyElements(columnAlignedDataD);

			if (HomogenousMatrix4(columnAlignedData) != matrix)
			{
				return false;
			}

			if (HomogenousMatrixF4(columnAlignedDataF) != matrixF)
			{
				return false;
			}

			if (HomogenousMatrixD4(columnAlignedDataD) != matrixD)
			{
				return false;
			}

			Scalar rowAlignedData[16];
			float rowAlignedDataF[16];
			double rowAlignedDataD[16];
			matrixD.copyElements(rowAlignedData, true);
			matrixD.copyElements(rowAlignedDataF, true);
			matrixD.copyElements(rowAlignedDataD, true);

			if (SquareMatrix4(rowAlignedData).transposed() != SquareMatrix4(matrix))
			{
				return false;
			}

			if (SquareMatrixF4(rowAlignedDataF).transposed() != SquareMatrixF4(matrixF))
			{
				return false;
			}

			if (SquareMatrixD4(rowAlignedDataD).transposed() != SquareMatrixD4(matrixD))
			{
				return false;
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

}

}

}
