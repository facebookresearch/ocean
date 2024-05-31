/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestHomogenousMatrix4.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Random.h"
#include "ocean/math/Rotation.h"
#include "ocean/math/SquareMatrix4.h"

#include "ocean/test/ValidationPrecision.h"

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

	allSucceeded = testConstructor<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testConstructor<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testElementConstructor<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testElementConstructor<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testInvert<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testInvert<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDecomposition<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testDecomposition<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testVectorConversion<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testVectorConversion<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCopyElements<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testCopyElements<double>(testDuration) && allSucceeded;

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


TEST(TestHomogenousMatrix4, Constructor_float)
{
	EXPECT_TRUE(TestHomogenousMatrix4::testConstructor<float>(GTEST_TEST_DURATION));
}

TEST(TestHomogenousMatrix4, Constructor)
{
	EXPECT_TRUE(TestHomogenousMatrix4::testConstructor<double>(GTEST_TEST_DURATION));
}


TEST(TestHomogenousMatrix4, ElementConstructor_float)
{
	EXPECT_TRUE(TestHomogenousMatrix4::testElementConstructor<float>(GTEST_TEST_DURATION));
}

TEST(TestHomogenousMatrix4, ElementConstructor_double)
{
	EXPECT_TRUE(TestHomogenousMatrix4::testElementConstructor<double>(GTEST_TEST_DURATION));
}


TEST(TestHomogenousMatrix4, Invert_float)
{
	EXPECT_TRUE(TestHomogenousMatrix4::testInvert<float>(GTEST_TEST_DURATION));
}

TEST(TestHomogenousMatrix4, Invert_double)
{
	EXPECT_TRUE(TestHomogenousMatrix4::testInvert<double>(GTEST_TEST_DURATION));
}


TEST(TestHomogenousMatrix4, Decomposition_float)
{
	EXPECT_TRUE(TestHomogenousMatrix4::testDecomposition<float>(GTEST_TEST_DURATION));
}

TEST(TestHomogenousMatrix4, Decomposition_double)
{
	EXPECT_TRUE(TestHomogenousMatrix4::testDecomposition<double>(GTEST_TEST_DURATION));
}


TEST(TestHomogenousMatrix4, VectorConversion_float)
{
	EXPECT_TRUE(TestHomogenousMatrix4::testVectorConversion<float>(GTEST_TEST_DURATION));
}

TEST(TestHomogenousMatrix4, VectorConversion_double)
{
	EXPECT_TRUE(TestHomogenousMatrix4::testVectorConversion<double>(GTEST_TEST_DURATION));
}


TEST(TestHomogenousMatrix4, CopyElements_float)
{
	EXPECT_TRUE(TestHomogenousMatrix4::testCopyElements<float>(GTEST_TEST_DURATION));
}

TEST(TestHomogenousMatrix4, CopyElements_double)
{
	EXPECT_TRUE(TestHomogenousMatrix4::testCopyElements<double>(GTEST_TEST_DURATION));
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

template <typename T>
bool TestHomogenousMatrix4::testConstructor(const double testDuration)
{
	constexpr size_t size = 1000000;

	Log::info() << "Constructor test for " << String::insertCharacter(String::toAString(size), ',', 3, false) << " matrices for " << TypeNamer::name<T>() << ":";

	HomogenousMatricesT4<T> matrices0(size);
	HomogenousMatricesT4<T> matrices1(size);
	HomogenousMatricesT4<T> matrices2(size);

	HighPerformanceStatistic performanceDefault;
	HighPerformanceStatistic performanceOne;
	HighPerformanceStatistic performanceZero;

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		performanceDefault.start();
			const HomogenousMatricesT4<T> matricesDefault(size);
		performanceDefault.stop();

		OCEAN_EXPECT_EQUAL(validation, matricesDefault.size(), size);

		performanceOne.start();
			const HomogenousMatricesT4<T> matricesOne(size, HomogenousMatrixT4<T>(true));
		performanceOne.stop();

		OCEAN_EXPECT_EQUAL(validation, matricesOne.size(), size);

		performanceZero.start();
			const HomogenousMatricesT4<T> matricesZero(size, HomogenousMatrixT4<T>(false));
		performanceZero.stop();

		OCEAN_EXPECT_EQUAL(validation, matricesZero.size(), size);

		{
			// testing identity

			const VectorT3<T> random(RandomT<T>::vector3(-1000, 1000));
			const VectorT3<T> result(HomogenousMatrixT4<T>(true) * random);

			OCEAN_EXPECT_EQUAL(validation, random, result);
		}

		{
			const VectorT3<T> random(RandomT<T>::vector3(-1000, 1000));
			const VectorT3<T> result(HomogenousMatrixT4<T>(VectorT3<T>(0, 0, 0), SquareMatrixT3<T>(false)) * random);

			OCEAN_EXPECT_TRUE(validation, result.isNull());
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Uninitialized performance: " << performanceDefault;
	Log::info() << "Identity matrix performance: " << performanceOne;
	Log::info() << "Zero matrix performance: " << performanceZero;

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestHomogenousMatrix4::testElementConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Element-based constructor test for " << TypeNamer::name<T>() << ":";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			double doubleValues[16];
			float floatValues[16];
			Scalar scalarValues[16];

			for (unsigned int i = 0u; i < 16u; ++i)
			{
				doubleValues[i] = RandomD::scalar(randomGenerator, -100, 100);
				floatValues[i] = float(doubleValues[i]);
				scalarValues[i] = Scalar(doubleValues[i]);
			}

			const HomogenousMatrixT4<T> aMatrixFromDouble(doubleValues);
			const HomogenousMatrixT4<T> bMatrixFromDouble(doubleValues, false);
			const HomogenousMatrixT4<T> bMatrixFromDoubleTransposed(doubleValues, true);

			const HomogenousMatrixT4<T> aMatrixFromFloat(floatValues);
			const HomogenousMatrixT4<T> bMatrixFromFloat(floatValues, false);
			const HomogenousMatrixT4<T> bMatrixFromFloatTransposed(floatValues, true);

			const HomogenousMatrixT4<T> aMatrixFromScalar(scalarValues);
			const HomogenousMatrixT4<T> bMatrixFromScalar(scalarValues, false);
			const HomogenousMatrixT4<T> bMatrixFromScalarTransposed(scalarValues, true);

			HomogenousMatrixF4 floatTest;
			HomogenousMatrixF4 floatTestTransposed;
			HomogenousMatrixD4 doubleTest;
			HomogenousMatrixD4 doubleTestTransposed;
			HomogenousMatrix4 scalarTest;
			HomogenousMatrix4 scalarTestTransposed;

			unsigned int index = 0u;

			for (unsigned int column = 0u; column < 4u; ++column)
			{
				for (unsigned int row = 0u; row < 4u; ++row)
				{
					floatTest(row, column) = floatValues[index];
					doubleTest(row, column) = doubleValues[index];
					scalarTest(row, column) = scalarValues[index];

					floatTestTransposed(column, row) = floatValues[index];
					doubleTestTransposed(column, row) = doubleValues[index];
					scalarTestTransposed(column, row) = scalarValues[index];

					++index;
				}
			}

			ocean_assert(index == 16u);

			constexpr T eps = T(0.0001);

			if constexpr (std::is_same<T, float>::value)
			{
				OCEAN_EXPECT_EQUAL(validation, aMatrixFromFloat, floatTest);
				OCEAN_EXPECT_EQUAL(validation, bMatrixFromFloat, floatTest);
				OCEAN_EXPECT_EQUAL(validation, bMatrixFromFloatTransposed, floatTestTransposed);
			}
			else
			{
				OCEAN_EXPECT_TRUE(validation, aMatrixFromFloat.isEqual(doubleTest, eps));
				OCEAN_EXPECT_TRUE(validation, bMatrixFromFloat.isEqual(doubleTest, eps));
				OCEAN_EXPECT_TRUE(validation, bMatrixFromFloatTransposed.isEqual(doubleTestTransposed, eps));
			}

			if constexpr (std::is_same<T, double>::value)
			{
				OCEAN_EXPECT_EQUAL(validation, aMatrixFromDouble, doubleTest);
				OCEAN_EXPECT_EQUAL(validation, bMatrixFromDouble, doubleTest);
				OCEAN_EXPECT_EQUAL(validation, bMatrixFromDoubleTransposed, doubleTestTransposed);
			}
			else
			{
				OCEAN_EXPECT_TRUE(validation, aMatrixFromDouble.isEqual(floatTest, eps));
				OCEAN_EXPECT_TRUE(validation, bMatrixFromDouble.isEqual(floatTest, eps));
				OCEAN_EXPECT_TRUE(validation, bMatrixFromDoubleTransposed.isEqual(floatTestTransposed, eps));
			}

			if constexpr (std::is_same<T, double>::value)
			{
				OCEAN_EXPECT_TRUE(validation, aMatrixFromScalar.isEqual(doubleTest, eps));
				OCEAN_EXPECT_TRUE(validation, bMatrixFromScalar.isEqual(doubleTest, eps));
				OCEAN_EXPECT_TRUE(validation, bMatrixFromScalarTransposed.isEqual(doubleTestTransposed, eps));
			}
			else
			{
				OCEAN_EXPECT_TRUE(validation, aMatrixFromScalar.isEqual(floatTest, eps));
				OCEAN_EXPECT_TRUE(validation, bMatrixFromScalar.isEqual(floatTest, eps));
				OCEAN_EXPECT_TRUE(validation, bMatrixFromScalarTransposed.isEqual(floatTestTransposed, eps));
			}

			if constexpr (std::is_same<T, Scalar>::value)
			{
				OCEAN_EXPECT_TRUE(validation, aMatrixFromScalar.isEqual(scalarTest, eps));
				OCEAN_EXPECT_TRUE(validation, bMatrixFromScalar.isEqual(scalarTest, eps));
				OCEAN_EXPECT_TRUE(validation, bMatrixFromScalarTransposed.isEqual(scalarTestTransposed, eps));
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestHomogenousMatrix4::testInvert(const double testDuration)
{
	constexpr size_t size = 1000000;

	Log::info() << "Invert test for " << String::insertCharacter(String::toAString(size), ',', 3, false) << " matrices for " << TypeNamer::name<T>() << ":";

	HighPerformanceStatistic performanceInverted;
	HighPerformanceStatistic performanceInvert;
	HighPerformanceStatistic performanceInvertTo;

	RandomGenerator randomGenerator;

	ValidationPrecision validation(0.99, randomGenerator);

	const HomogenousMatrixT4<T> identity(true);

	const Timestamp startTimestamp(true);

	constexpr T identityThreshold = std::is_same<T, float>::value ? NumericT<T>::eps() * T(100) : NumericT<T>::eps();

	do
	{
		HomogenousMatricesT4<T> matrices(size);

		for (size_t n = 0; n < size; ++n)
		{
			const VectorT3<T> translation(RandomT<T>::vector3(randomGenerator, -100, 100));
			const RotationT<T> rotation(RandomT<T>::rotation(randomGenerator));
			const VectorT3<T> scale(RandomT<T>::vector3(randomGenerator, T(0.01), 100));

			matrices[n] = HomogenousMatrixT4<T>(translation, rotation, scale);
		}

		{
			// testing inverted() function

			HomogenousMatricesT4<T> targetMatrices(size);

			performanceInverted.start();
				for (size_t n = 0; n < size; ++n)
				{
					targetMatrices[n] = matrices[n].inverted();
				}
			performanceInverted.stop();

			for (size_t n = 0; n < size; ++n)
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const HomogenousMatrixT4<T> result0(matrices[n] * targetMatrices[n]);

				if (!result0.isValid())
				{
					OCEAN_SET_FAILED(validation);
				}

				if (!result0.isEqual(identity, identityThreshold))
				{
					scopedIteration.setInaccurate();
				}

				const HomogenousMatrixT4<T> result1(targetMatrices[n] * matrices[n]);

				if (!result1.isValid())
				{
					OCEAN_SET_FAILED(validation);
				}

				if (!result1.isEqual(identity, identityThreshold))
				{
					scopedIteration.setInaccurate();
				}
			}
		}

		{
			// testing invert() function

			HomogenousMatricesT4<T> targetMatrices(size);

			performanceInvert.start();
				for (size_t n = 0; n < size; ++n)
				{
					targetMatrices[n] = matrices[n];
					targetMatrices[n].invert();
				}
			performanceInvert.stop();

			for (size_t n = 0; n < size; ++n)
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const HomogenousMatrixT4<T> result0(matrices[n] * targetMatrices[n]);

				if (!result0.isValid())
				{
					OCEAN_SET_FAILED(validation);
				}

				if (!result0.isEqual(identity, identityThreshold))
				{
					scopedIteration.setInaccurate();
				}

				const HomogenousMatrixT4<T> result1(targetMatrices[n] * matrices[n]);

				if (!result1.isValid())
				{
					OCEAN_SET_FAILED(validation);
				}

				if (!result1.isEqual(identity, identityThreshold))
				{
					scopedIteration.setInaccurate();
				}
			}
		}

		{
			// testing invert(target) function

			HomogenousMatricesT4<T> targetMatrices(size);

			performanceInvertTo.start();
				for (size_t n = 0; n < size; ++n)
				{
					matrices[n].invert(targetMatrices[n]);
				}
			performanceInvertTo.stop();

			for (size_t n = 0; n < size; ++n)
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const HomogenousMatrixT4<T> result0(matrices[n] * targetMatrices[n]);

				if (!result0.isValid())
				{
					OCEAN_SET_FAILED(validation);
				}

				if (!result0.isEqual(identity, identityThreshold))
				{
					scopedIteration.setInaccurate();
				}

				const HomogenousMatrixT4<T> result1(targetMatrices[n] * matrices[n]);

				if (!result1.isValid())
				{
					OCEAN_SET_FAILED(validation);
				}

				if (!result1.isEqual(identity, identityThreshold))
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance inverted(): " << performanceInverted;
	Log::info() << "Performance invert(): " << performanceInvert;
	Log::info() << "Performance invert(target): " << performanceInvertTo;

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestHomogenousMatrix4::testDecomposition(const double testDuration)
{
	constexpr size_t size = 1000;

	Log::info() << "Decomposition test for " << String::insertCharacter(String::toAString(size), ',', 3, false) << " matrices for " << TypeNamer::name<T>() << ":";

	HighPerformanceStatistic performance;

	RandomGenerator randomGenerator;

	ValidationPrecision validation(0.99, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		VectorsT3<T> translations;
		QuaternionsT<T> rotations;
		VectorsT3<T> scales;
		VectorsT3<T> shears;

		HomogenousMatricesT4<T> matrices;

		for (size_t n = 0; n < size; ++n)
		{
			const VectorT3<T> translation = RandomT<T>::vector3(randomGenerator) * T(10);
			const QuaternionT<T> rotation = RandomT<T>::quaternion(randomGenerator);
			const VectorT3<T> scale = RandomT<T>::vector3(randomGenerator, T(0.1), T(5));
			const VectorT3<T> shear = RandomT<T>::vector3(randomGenerator, T(0), T(2));

			matrices.emplace_back(translation, rotation, scale, shear);

			translations.push_back(translation);
			rotations.push_back(rotation);
			scales.push_back(scale);
			shears.push_back(shear);
		}

		VectorsT3<T> decomposedTranslations(size);
		QuaternionsT<T> decomposedRotations(size);
		VectorsT3<T> decomposedScales(size);
		VectorsT3<T> decomposedShears(size);

		std::vector<uint8_t> results(size, 0x00u);

		performance.start();

			for (size_t n = 0; n < size; ++n)
			{
				results[n] = matrices[n].decompose(decomposedTranslations[n], decomposedRotations[n], decomposedScales[n], decomposedShears[n]) ? 0xFFu : 0x00u;
			}

		performance.stop();

		for (size_t n = 0; n < size; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			if (results[n] != 0xFFu)
			{
				OCEAN_SET_FAILED(validation);
			}

			const VectorT3<T>& translation = translations[n];
			const QuaternionT<T>& rotation = rotations[n];
			const VectorT3<T>& scale = scales[n];
			const VectorT3<T>& shear = shears[n];

			const VectorT3<T>& decomposedTranslation = decomposedTranslations[n];
			const QuaternionT<T>& decomposedRotation = decomposedRotations[n];
			const VectorT3<T>& decomposedScale = decomposedScales[n];
			const VectorT3<T>& decomposedShear = decomposedShears[n];

			if (translation != decomposedTranslation || rotation != decomposedRotation || scale != decomposedScale || shear != decomposedShear)
			{
				const HomogenousMatrixT4<T> decomposedMatrix(decomposedTranslation, decomposedRotation, decomposedScale, decomposedShear);

				if (!matrices[n].isEqual(decomposedMatrix, NumericT<T>::weakEps()))
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestHomogenousMatrix4::testVectorConversion(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "HomogenousMatrix4::matrices2matrices() test for " << TypeNamer::name<T>() << ":";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int size = RandomI::random(randomGenerator, 1u, 1000u);

		HomogenousMatricesD4 matricesD;
		HomogenousMatricesF4 matricesF;

		for (size_t n = 0; n < size; ++n)
		{
			HomogenousMatrixD4 matrixD;
			HomogenousMatrixF4 matrixF;

			for (unsigned int i = 0u; i < 16u; ++i)
			{
				const double value = RandomD::scalar(randomGenerator, -10, 10);

				matrixD[i] = value;
				matrixF[i] = float(value);
			}

			matricesD.push_back(matrixD);
			matricesF.push_back(matrixF);
		}

		const HomogenousMatricesT4<T> convertedFromDouble0(HomogenousMatrixT4<T>::matrices2matrices(matricesD));
		const HomogenousMatricesT4<T> convertedFromDouble1(HomogenousMatrixT4<T>::matrices2matrices(matricesD.data(), matricesD.size()));

		const HomogenousMatricesT4<T> convertedFromFloat0(HomogenousMatrixT4<T>::matrices2matrices(matricesF));
		const HomogenousMatricesT4<T> convertedFromFloat1(HomogenousMatrixT4<T>::matrices2matrices(matricesF.data(), matricesF.size()));

		if constexpr (std::is_same<T, float>::value)
		{
			for (size_t n = 0; n < size; ++n)
			{
				const HomogenousMatrixF4& matrix = matricesF[n];

				for (unsigned int i = 0u; i < 16u; ++i)
				{
					OCEAN_EXPECT_TRUE(validation, convertedFromDouble0[n].isEqual(matrix, NumericF::weakEps()));
					OCEAN_EXPECT_TRUE(validation, convertedFromDouble1[n].isEqual(matrix, NumericF::weakEps()));

					OCEAN_EXPECT_EQUAL(validation, convertedFromFloat0[n], matrix);
					OCEAN_EXPECT_EQUAL(validation, convertedFromFloat1[n], matrix);
				}
			}
		}
		else
		{
			for (size_t n = 0; n < size; ++n)
			{
				const HomogenousMatrixD4& matrix = matricesD[n];

				for (unsigned int i = 0u; i < 16u; ++i)
				{
					OCEAN_EXPECT_EQUAL(validation, convertedFromDouble0[n], matrix);
					OCEAN_EXPECT_EQUAL(validation, convertedFromDouble1[n], matrix);

					OCEAN_EXPECT_TRUE(validation, convertedFromFloat0[n].isEqual(matrix, NumericD::weakEps()));
					OCEAN_EXPECT_TRUE(validation, convertedFromFloat1[n].isEqual(matrix, NumericD::weakEps()));
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestHomogenousMatrix4::testCopyElements(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "HomogenousMatrix4::copyElements() test for " << TypeNamer::name<T>() << ":";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		HomogenousMatrixT4<T> matrix;

		for (unsigned n = 0u; n < 16u; ++n)
		{
			matrix[n] = RandomT<T>::scalar(randomGenerator, -10, 10);
		}

		{
			// column aligned

			Scalar columnAlignedValues[16];
			float columnAlignedValuesF[16];
			double columnAlignedValuesD[16];

			matrix.copyElements(columnAlignedValues);
			matrix.copyElements(columnAlignedValuesF);
			matrix.copyElements(columnAlignedValuesD);

			for (unsigned int n = 0u; n < 16u; ++n)
			{
				if constexpr (std::is_same<T, float>::value)
				{
					OCEAN_EXPECT_EQUAL(validation, matrix[n], columnAlignedValuesF[n]);

					OCEAN_EXPECT_TRUE(validation, NumericT<T>::isWeakEqual(matrix[n], T(columnAlignedValuesD[n])));
				}
				else
				{
					OCEAN_EXPECT_EQUAL(validation, matrix[n], columnAlignedValuesD[n]);

					OCEAN_EXPECT_TRUE(validation, NumericT<T>::isWeakEqual(matrix[n], T(columnAlignedValuesF[n])));
				}

				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isWeakEqual(matrix[n], T(columnAlignedValues[n])));
			}

			OCEAN_EXPECT_TRUE(validation, HomogenousMatrixT4<T>(columnAlignedValues).isEqual(matrix, NumericT<T>::weakEps()));
			OCEAN_EXPECT_TRUE(validation, HomogenousMatrixT4<T>(columnAlignedValuesF).isEqual(matrix, NumericT<T>::weakEps()));
			OCEAN_EXPECT_TRUE(validation, HomogenousMatrixT4<T>(columnAlignedValuesD).isEqual(matrix, NumericT<T>::weakEps()));
		}

		{
			// row aligned

			Scalar rowAlignedValues[16];
			float rowAlignedValuesF[16];
			double rowAlignedValuesD[16];

			matrix.copyElements(rowAlignedValues, true);
			matrix.copyElements(rowAlignedValuesF, true);
			matrix.copyElements(rowAlignedValuesD, true);

			for (unsigned int n = 0u; n < 16u; ++n)
			{
				constexpr std::array<unsigned int, 16u> lookup =
				{
					0u, 4u,  8u, 12u,
					1u, 5u,  9u, 13u,
					2u, 6u, 10u, 14u,
					3u, 7u, 11u, 15u
				};

				const unsigned int nTransposed = lookup[n];

				if constexpr (std::is_same<T, float>::value)
				{
					OCEAN_EXPECT_EQUAL(validation, matrix[n], rowAlignedValuesF[nTransposed]);

					OCEAN_EXPECT_TRUE(validation, NumericT<T>::isWeakEqual(matrix[n], T(rowAlignedValuesD[nTransposed])));
				}
				else
				{
					OCEAN_EXPECT_EQUAL(validation, matrix[n], rowAlignedValuesD[nTransposed]);

					OCEAN_EXPECT_TRUE(validation, NumericT<T>::isWeakEqual(matrix[n], T(rowAlignedValuesF[nTransposed])));
				}

				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isWeakEqual(matrix[n], T(rowAlignedValues[nTransposed])));
			}

			OCEAN_EXPECT_TRUE(validation, HomogenousMatrixT4<T>(rowAlignedValues, true).isEqual(matrix, NumericT<T>::weakEps()));
			OCEAN_EXPECT_TRUE(validation, HomogenousMatrixT4<T>(rowAlignedValuesF, true).isEqual(matrix, NumericT<T>::weakEps()));
			OCEAN_EXPECT_TRUE(validation, HomogenousMatrixT4<T>(rowAlignedValuesD, true).isEqual(matrix, NumericT<T>::weakEps()));

			OCEAN_EXPECT_TRUE(validation, SquareMatrixT4<T>(rowAlignedValues).isEqual(SquareMatrixT4<T>(matrix).transposed(), NumericT<T>::weakEps()));
			OCEAN_EXPECT_TRUE(validation, SquareMatrixT4<T>(rowAlignedValuesF).isEqual(SquareMatrixT4<T>(matrix).transposed(), NumericT<T>::weakEps()));
			OCEAN_EXPECT_TRUE(validation, SquareMatrixT4<T>(rowAlignedValuesD).isEqual(SquareMatrixT4<T>(matrix).transposed(), NumericT<T>::weakEps()));

			OCEAN_EXPECT_TRUE(validation, SquareMatrixT4<T>(HomogenousMatrixT4<T>(rowAlignedValues)).transposed().isEqual(SquareMatrixT4<T>(matrix), NumericT<T>::weakEps()));
			OCEAN_EXPECT_TRUE(validation, SquareMatrixT4<T>(HomogenousMatrixT4<T>(rowAlignedValuesF)).transposed().isEqual(SquareMatrixT4<T>(matrix), NumericT<T>::weakEps()));
			OCEAN_EXPECT_TRUE(validation, SquareMatrixT4<T>(HomogenousMatrixT4<T>(rowAlignedValuesD)).transposed().isEqual(SquareMatrixT4<T>(matrix), NumericT<T>::weakEps()));
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
