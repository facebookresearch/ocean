/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestRotation.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/test/TestResult.h"

#include "ocean/math/Random.h"

#include "ocean/test/Validation.h"
#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestRotation::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Rotation test");

	Log::info() << " ";

	if (selector.shouldRun("constructors"))
	{
		testResult = testConstructors<float>(testDuration);
		Log::info() << " ";
		testResult = testConstructors<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("matrixconstructors"))
	{
		testResult = testMatrixConstructors<float>(testDuration);
		Log::info() << " ";
		testResult = testMatrixConstructors<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("arrayconstructor"))
	{
		testResult = testArrayConstructor<float>(testDuration);
		Log::info() << " ";
		testResult = testArrayConstructor<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("anglenormalization"))
	{
		testResult = testAngleNormalization<float>(testDuration);
		Log::info() << " ";
		testResult = testAngleNormalization<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("conversiontoquaterion"))
	{
		testResult = testConversionToQuaterion<float>(testDuration);
		Log::info() << " ";
		testResult = testConversionToQuaterion<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("conversiontohomogenousmatrix"))
	{
		testResult = testConversionToHomogenousMatrix<float>(testDuration);
		Log::info() << " ";
		testResult = testConversionToHomogenousMatrix<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("referenceoffsetconstructor"))
	{
		testResult = testReferenceOffsetConstructor<float>(testDuration);
		Log::info() << " ";
		testResult = testReferenceOffsetConstructor<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("left_r_right"))
	{
		testResult = testLeft_R_right<float>(testDuration);
		Log::info() << " ";
		testResult = testLeft_R_right<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("inversion"))
	{
		testResult = testInversion<float>(testDuration);
		Log::info() << " ";
		testResult = testInversion<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("comparisonoperators"))
	{
		testResult = testComparisonOperators<float>(testDuration);
		Log::info() << " ";
		testResult = testComparisonOperators<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("multiplicationoperators"))
	{
		testResult = testMultiplicationOperators<float>(testDuration);
		Log::info() << " ";
		testResult = testMultiplicationOperators<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("elementaccess"))
	{
		testResult = testElementAccess<float>(testDuration);
		Log::info() << " ";
		testResult = testElementAccess<double>(testDuration);

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestRotation, Constructors_float)
{
	EXPECT_TRUE(TestRotation::testConstructors<float>(GTEST_TEST_DURATION));
}

TEST(TestRotation, Constructors_double)
{
	EXPECT_TRUE(TestRotation::testConstructors<double>(GTEST_TEST_DURATION));
}


TEST(TestRotation, MatrixConstructors_float)
{
	EXPECT_TRUE(TestRotation::testMatrixConstructors<float>(GTEST_TEST_DURATION));
}

TEST(TestRotation, MatrixConstructors_double)
{
	EXPECT_TRUE(TestRotation::testMatrixConstructors<double>(GTEST_TEST_DURATION));
}


TEST(TestRotation, ArrayConstructor_float)
{
	EXPECT_TRUE(TestRotation::testArrayConstructor<float>(GTEST_TEST_DURATION));
}

TEST(TestRotation, ArrayConstructor_double)
{
	EXPECT_TRUE(TestRotation::testArrayConstructor<double>(GTEST_TEST_DURATION));
}


TEST(TestRotation, AngleNormalization_float)
{
	EXPECT_TRUE(TestRotation::testAngleNormalization<float>(GTEST_TEST_DURATION));
}

TEST(TestRotation, AngleNormalization_double)
{
	EXPECT_TRUE(TestRotation::testAngleNormalization<double>(GTEST_TEST_DURATION));
}


TEST(TestRotation, ConversionToQuaterion_float)
{
	EXPECT_TRUE(TestRotation::testConversionToQuaterion<float>(GTEST_TEST_DURATION));
}

TEST(TestRotation, ConversionToQuaterion_double)
{
	EXPECT_TRUE(TestRotation::testConversionToQuaterion<double>(GTEST_TEST_DURATION));
}


TEST(TestRotation, ConversionToHomogenousMatrix_float)
{
	EXPECT_TRUE(TestRotation::testConversionToHomogenousMatrix<float>(GTEST_TEST_DURATION));
}

TEST(TestRotation, ConversionToHomogenousMatrix_double)
{
	EXPECT_TRUE(TestRotation::testConversionToHomogenousMatrix<double>(GTEST_TEST_DURATION));
}


TEST(TestRotation, ReferenceOffsetConstructor_float)
{
	EXPECT_TRUE(TestRotation::testReferenceOffsetConstructor<float>(GTEST_TEST_DURATION));
}

TEST(TestRotation, ReferenceOffsetConstructor_double)
{
	EXPECT_TRUE(TestRotation::testReferenceOffsetConstructor<double>(GTEST_TEST_DURATION));
}


TEST(TestRotation, Left_R_right_float)
{
	EXPECT_TRUE(TestRotation::testLeft_R_right<float>(GTEST_TEST_DURATION));
}

TEST(TestRotation, Left_R_right_double)
{
	EXPECT_TRUE(TestRotation::testLeft_R_right<double>(GTEST_TEST_DURATION));
}


TEST(TestRotation, Inversion_float)
{
	EXPECT_TRUE(TestRotation::testInversion<float>(GTEST_TEST_DURATION));
}

TEST(TestRotation, Inversion_double)
{
	EXPECT_TRUE(TestRotation::testInversion<double>(GTEST_TEST_DURATION));
}


TEST(TestRotation, ComparisonOperators_float)
{
	EXPECT_TRUE(TestRotation::testComparisonOperators<float>(GTEST_TEST_DURATION));
}

TEST(TestRotation, ComparisonOperators_double)
{
	EXPECT_TRUE(TestRotation::testComparisonOperators<double>(GTEST_TEST_DURATION));
}


TEST(TestRotation, MultiplicationOperators_float)
{
	EXPECT_TRUE(TestRotation::testMultiplicationOperators<float>(GTEST_TEST_DURATION));
}

TEST(TestRotation, MultiplicationOperators_double)
{
	EXPECT_TRUE(TestRotation::testMultiplicationOperators<double>(GTEST_TEST_DURATION));
}


TEST(TestRotation, ElementAccess_float)
{
	EXPECT_TRUE(TestRotation::testElementAccess<float>(GTEST_TEST_DURATION));
}

TEST(TestRotation, ElementAccess_double)
{
	EXPECT_TRUE(TestRotation::testElementAccess<double>(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

template <typename T>
bool TestRotation::testConstructors(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing constructors for '" << TypeNamer::name<T>() << "':";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		{
			// default constructor

			const RotationT<T> rotation;

			OCEAN_EXPECT_TRUE(validation, rotation.isValid());
			OCEAN_EXPECT_EQUAL(validation, rotation.axis(), VectorT3<T>(0, 1, 0));
			OCEAN_EXPECT_EQUAL(validation, rotation.angle(), T(0));
		}

		{
			// constructor with x, y, z, angle

			const VectorT3<T> axis = RandomT<T>::vector3(randomGenerator);
			const T angle = RandomT<T>::scalar(randomGenerator, T(-10.0) * NumericT<T>::pi(), T(10.0) * NumericT<T>::pi());

			const RotationT<T> rotation(axis.x(), axis.y(), axis.z(), angle);

			OCEAN_EXPECT_TRUE(validation, rotation.isValid());
			OCEAN_EXPECT_EQUAL(validation, rotation.axis(), axis);

			// angle should be normalized to [0, 2*PI)
			const T normalizedAngle = rotation.angle();
			OCEAN_EXPECT_GREATER_EQUAL(validation, normalizedAngle, T(0));
			OCEAN_EXPECT_LESS(validation, normalizedAngle, NumericT<T>::pi2());
		}

		{
			// constructor with axis and angle

			const VectorT3<T> axis = RandomT<T>::vector3(randomGenerator);
			const T angle = RandomT<T>::scalar(randomGenerator, T(-10.0) * NumericT<T>::pi(), T(10.0) * NumericT<T>::pi());

			const RotationT<T> rotation(axis, angle);

			OCEAN_EXPECT_TRUE(validation, rotation.isValid());
			OCEAN_EXPECT_EQUAL(validation, rotation.axis(), axis);

			// angle should be normalized to [0, 2*PI)
			const T normalizedAngle = rotation.angle();
			OCEAN_EXPECT_GREATER_EQUAL(validation, normalizedAngle, T(0));
			OCEAN_EXPECT_LESS(validation, normalizedAngle, NumericT<T>::pi2());
		}

		{
			// Euler constructor

			const EulerT<T> euler = RandomT<T>::euler(randomGenerator);
			const RotationT<T> rotation(euler);

			OCEAN_EXPECT_TRUE(validation, rotation.isValid());

			// verify the rotation is equivalent
			const QuaternionT<T> quaternionFromEuler(euler);
			const QuaternionT<T> quaternionFromRotation(rotation);

			const VectorT3<T> testVector = RandomT<T>::vector3(randomGenerator);
			const VectorT3<T> result1 = quaternionFromEuler * testVector;
			const VectorT3<T> result2 = quaternionFromRotation * testVector;

			OCEAN_EXPECT_TRUE(validation, result1.isEqual(result2, NumericT<T>::weakEps()));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestRotation::testMatrixConstructors(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing matrix constructors for '" << TypeNamer::name<T>() << "':";

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.99, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		{
			// SquareMatrix3 constructor

			const RotationT<T> originalRotation = RandomT<T>::rotation(randomGenerator);
			const SquareMatrixT3<T> matrix(originalRotation);
			const RotationT<T> reconstructedRotation(matrix);

			OCEAN_EXPECT_TRUE(validation, reconstructedRotation.isValid());

			// verify that the rotations produce the same result

			const VectorT3<T> testVector = RandomT<T>::vector3(randomGenerator);
			const VectorT3<T> result1 = originalRotation * testVector;
			const VectorT3<T> result2 = reconstructedRotation * testVector;

			if (!result1.isEqual(result2, NumericT<T>::weakEps()))
			{
				scopedIteration.setInaccurate();
			}
		}

		{
			// HomogenousMatrix4 constructor

			const RotationT<T> originalRotation = RandomT<T>::rotation(randomGenerator);
			const HomogenousMatrixT4<T> matrix(originalRotation);
			const RotationT<T> reconstructedRotation(matrix);

			OCEAN_EXPECT_TRUE(validation, reconstructedRotation.isValid());

			// verify that the rotations produce the same result

			const VectorT3<T> testVector = RandomT<T>::vector3(randomGenerator);
			const VectorT3<T> result1 = originalRotation * testVector;
			const VectorT3<T> result2 = reconstructedRotation * testVector;

			if (!result1.isEqual(result2, NumericT<T>::weakEps()))
			{
				scopedIteration.setInaccurate();
			}
		}

		{
			// special test: 180-degree rotation around different axes

			for (unsigned int axisIndex = 0u; axisIndex < 3u; ++axisIndex)
			{
				VectorT3<T> axis(0, 0, 0);
				axis[axisIndex] = T(1);

				for (const bool tinyOffset : {false, true})
				{
					if (tinyOffset)
					{
						axis + RandomT<T>::vector3(randomGenerator, T(-0.01), T(0.01));

						axis.normalize();
					}

					const RotationT<T> originalRotation(axis, NumericT<T>::pi());

					const SquareMatrixT3<T> matrix(originalRotation);
					const RotationT<T> reconstructedRotation(matrix);

					OCEAN_EXPECT_TRUE(validation, reconstructedRotation.isValid());

					// verify that the rotations produce the same result

					const VectorT3<T> testVector = RandomT<T>::vector3(randomGenerator);
					const VectorT3<T> result1 = originalRotation * testVector;
					const VectorT3<T> result2 = reconstructedRotation * testVector;

					if (!result1.isEqual(result2, NumericT<T>::weakEps()))
					{
						scopedIteration.setInaccurate();
					}
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestRotation::testArrayConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing array constructor for '" << TypeNamer::name<T>() << "':";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		{
			// normalized angle [0, 2*PI)

			const T eps = NumericT<T>::eps() * T(100);

			const VectorT3<T> axis = RandomT<T>::vector3(randomGenerator);
			const T angle = RandomT<T>::scalar(randomGenerator, T(0), NumericT<T>::pi2() - eps);

			const T values[4] = {axis.x(), axis.y(), axis.z(), angle};

			const RotationT<T> rotation(values);

			OCEAN_EXPECT_TRUE(validation, rotation.isValid());

			OCEAN_EXPECT_EQUAL(validation, rotation.axis(), axis);
			OCEAN_EXPECT_EQUAL(validation, rotation.angle(), angle);
		}

		{
			// angle outside [0, 2*PI) - this should be normalized according to documentation

			const VectorT3<T> axis = RandomT<T>::vector3(randomGenerator);
			const T angle = RandomT<T>::scalar(randomGenerator, T(-10.0) * NumericT<T>::pi(), T(10.0) * NumericT<T>::pi());

			const T values[4] = {axis.x(), axis.y(), axis.z(), angle};

			const RotationT<T> rotation(values);

			OCEAN_EXPECT_TRUE(validation, rotation.isValid());

			const T normalizedAngle = rotation.angle();

			OCEAN_EXPECT_GREATER_EQUAL(validation, normalizedAngle, T(0));
			OCEAN_EXPECT_LESS(validation, normalizedAngle, NumericT<T>::pi2());
		}

		{
			// data() accessor consistency

			const T eps = NumericT<T>::eps() * T(100);

			const VectorT3<T> axis = RandomT<T>::vector3(randomGenerator);
			const T angle = RandomT<T>::scalar(randomGenerator, T(0), NumericT<T>::pi2() - eps);

			const T values[4] = {axis.x(), axis.y(), axis.z(), angle};
			const RotationT<T> rotation(values);

			const T* data = rotation.data();

			OCEAN_EXPECT_EQUAL(validation, data[0], axis.x());
			OCEAN_EXPECT_EQUAL(validation, data[1], axis.y());
			OCEAN_EXPECT_EQUAL(validation, data[2], axis.z());
			OCEAN_EXPECT_EQUAL(validation, data[3], angle);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestRotation::testAngleNormalization(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing angle normalization for '" << TypeNamer::name<T>() << "':";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const VectorT3<T> axis = RandomT<T>::vector3(randomGenerator);

		{
			// setAngle with various angles

			RotationT<T> rotation;
			rotation.setAxis(axis);

			{
				// normal angles

				const T eps = NumericT<T>::eps() * T(100);

				const T angle = RandomT<T>::scalar(randomGenerator, T(0), NumericT<T>::pi2() - eps);
				rotation.setAngle(angle);

				OCEAN_EXPECT_TRUE(validation, rotation.isValid());
				OCEAN_EXPECT_EQUAL(validation, rotation.angle(), angle);
			}

			{
				// negative angles

				const T angle = RandomT<T>::scalar(randomGenerator, T(-10.0) * NumericT<T>::pi(), T(0));
				rotation.setAngle(angle);

				OCEAN_EXPECT_TRUE(validation, rotation.isValid());

				const T normalizedAngle = rotation.angle();

				OCEAN_EXPECT_GREATER_EQUAL(validation, normalizedAngle, T(0));
				OCEAN_EXPECT_LESS(validation, normalizedAngle, NumericT<T>::pi2());
			}

			{
				// angles > 2*PI

				const T angle = RandomT<T>::scalar(randomGenerator, NumericT<T>::pi2(), T(10.0) * NumericT<T>::pi());
				rotation.setAngle(angle);

				OCEAN_EXPECT_TRUE(validation, rotation.isValid());

				const T normalizedAngle = rotation.angle();

				OCEAN_EXPECT_GREATER_EQUAL(validation, normalizedAngle, T(0));
				OCEAN_EXPECT_LESS(validation, normalizedAngle, NumericT<T>::pi2());
			}

			{
				// special case 2*PI

				rotation.setAngle(NumericT<T>::pi2());

				OCEAN_EXPECT_TRUE(validation, rotation.isValid());

				const T normalizedAngle = rotation.angle();

				OCEAN_EXPECT_GREATER_EQUAL(validation, normalizedAngle, T(0));
				OCEAN_EXPECT_LESS(validation, normalizedAngle, NumericT<T>::pi2());
			}
		}

		{
			// constructor angle normalization

			{
				const RotationT<T> rotation(axis, NumericT<T>::pi2());

				OCEAN_EXPECT_TRUE(validation, rotation.isValid());

				const T normalizedAngle = rotation.angle();

				OCEAN_EXPECT_GREATER_EQUAL(validation, normalizedAngle, T(0));
				OCEAN_EXPECT_LESS(validation, normalizedAngle, NumericT<T>::pi2());
			}

			{
				const RotationT<T> rotation(axis, T(2) * NumericT<T>::pi2());

				OCEAN_EXPECT_TRUE(validation, rotation.isValid());

				const T normalizedAngle = rotation.angle();

				OCEAN_EXPECT_GREATER_EQUAL(validation, normalizedAngle, T(0));
				OCEAN_EXPECT_LESS(validation, normalizedAngle, NumericT<T>::pi2());

				OCEAN_EXPECT_TRUE(validation, Numeric::isEqual(rotation.angle(), T(0)) || Numeric::isEqual(rotation.angle(), NumericT<T>::pi2()));
			}

			{
				const RotationT<T> rotation(axis, -NumericT<T>::pi2());

				const T normalizedAngle = rotation.angle();

				OCEAN_EXPECT_GREATER_EQUAL(validation, normalizedAngle, T(0));
				OCEAN_EXPECT_LESS(validation, normalizedAngle, NumericT<T>::pi2());

				OCEAN_EXPECT_TRUE(validation, Numeric::isEqual(rotation.angle(), T(0)) || Numeric::isEqual(rotation.angle(), NumericT<T>::pi2()));
			}
		}

		{
			// that rotations with angles differing by 2*PI are equivalent

			const T baseAngle = RandomT<T>::scalar(randomGenerator, T(0), NumericT<T>::pi2());

			const RotationT<T> rotation1(axis, baseAngle);
			const RotationT<T> rotation2(axis, baseAngle + NumericT<T>::pi2());
			const RotationT<T> rotation3(axis, baseAngle - NumericT<T>::pi2());

			const VectorT3<T> testVector = RandomT<T>::vector3(randomGenerator);

			const VectorT3<T> result1 = rotation1 * testVector;
			const VectorT3<T> result2 = rotation2 * testVector;
			const VectorT3<T> result3 = rotation3 * testVector;

			OCEAN_EXPECT_TRUE(validation, result1.isEqual(result2, NumericT<T>::weakEps()));
			OCEAN_EXPECT_TRUE(validation, result1.isEqual(result3, NumericT<T>::weakEps()));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestRotation::testConversionToQuaterion(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Conversion from Rotation to Quaternion (and 3x3 matrix) for '" << TypeNamer::name<T>() << "':";

	constexpr double successThreshold = 0.95;
	constexpr unsigned int constIterations = 100000u;

	RandomGenerator randomGenerator;
	ValidationPrecision validation(successThreshold, randomGenerator);

	const T epsilon = std::is_same<T, float>::value ? T(0.02) : NumericT<T>::weakEps();

	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);

	do
	{
		std::vector<RotationT<T>> rotations(constIterations);
		std::vector<QuaternionT<T>> quaternions(constIterations);

		for (unsigned int n = 0u; n < constIterations; ++n)
		{
			rotations[n] = RandomT<T>::rotation(randomGenerator);
		}

		performance.start();
			for (unsigned int n = 0u; n < constIterations; ++n)
			{
				quaternions[n] = QuaternionT<T>(rotations[n]);
			}
		performance.stop();

		for (unsigned int n = 0u; n < constIterations; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const RotationT<T>& rotation = rotations[n];
			const QuaternionT<T>& quaternion = quaternions[n];

			const SquareMatrixT3<T> matrix(rotation);

			const T angleX = NumericT<T>::rad2deg((matrix * VectorT3<T>(1, 0, 0)).angle(quaternion * VectorT3<T>(1, 0, 0)));
			const T angleY = NumericT<T>::rad2deg((matrix * VectorT3<T>(0, 1, 0)).angle(quaternion * VectorT3<T>(0, 1, 0)));
			const T angleZ = NumericT<T>::rad2deg((matrix * VectorT3<T>(0, 0, 1)).angle(quaternion * VectorT3<T>(0, 0, 1)));

			if (NumericT<T>::isNotEqual(angleX, 0, epsilon) || NumericT<T>::isNotEqual(angleY, 0, epsilon) || NumericT<T>::isNotEqual(angleZ, 0, epsilon))
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;
	Log::info() << " ";

	return validation.succeeded();
}

template <typename T>
bool TestRotation::testConversionToHomogenousMatrix(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Conversion from Rotation to Homogenous Matrix for '" << TypeNamer::name<T>() << "':";

	constexpr double successThreshold = 0.95;
	constexpr unsigned int constIterations = 100000u;

	RandomGenerator randomGenerator;
	ValidationPrecision validation(successThreshold, randomGenerator);

	std::vector<RotationT<T>> rotations(constIterations);
	std::vector<HomogenousMatrixT4<T>> matrices(constIterations);

	const T epsilon = std::is_same<T, float>::value ? T(0.02) : NumericT<T>::weakEps();

	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < constIterations; ++n)
		{
			rotations[n] = RandomT<T>::rotation(randomGenerator);
		}

		performance.start();
			for (unsigned int n = 0u; n < constIterations; ++n)
			{
				matrices[n] = HomogenousMatrixT4<T>(rotations[n]);
			}
		performance.stop();

		for (unsigned int n = 0u; n < constIterations; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const RotationT<T>& rotation = rotations[n];
			const HomogenousMatrixT4<T>& matrix = matrices[n];

			const T angleX = NumericT<T>::rad2deg((matrix * VectorT3<T>(1, 0, 0)).angle(rotation * VectorT3<T>(1, 0, 0)));
			const T angleY = NumericT<T>::rad2deg((matrix * VectorT3<T>(0, 1, 0)).angle(rotation * VectorT3<T>(0, 1, 0)));
			const T angleZ = NumericT<T>::rad2deg((matrix * VectorT3<T>(0, 0, 1)).angle(rotation * VectorT3<T>(0, 0, 1)));

			if (NumericT<T>::isNotEqual(angleX, 0, epsilon) || NumericT<T>::isNotEqual(angleY, 0, epsilon) || NumericT<T>::isNotEqual(angleZ, 0, epsilon))
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestRotation::testReferenceOffsetConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Reference offset constructor for '" << TypeNamer::name<T>() << "':";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const VectorT3<T> reference(RandomT<T>::vector3(randomGenerator));
			const VectorT3<T> offset(RandomT<T>::vector3(randomGenerator));

			// identity test
			OCEAN_EXPECT_EQUAL(validation, RotationT<T>(VectorT3<T>(1, 0, 0), VectorT3<T>(1, 0, 0)) * reference, reference);
			OCEAN_EXPECT_EQUAL(validation, RotationT<T>(VectorT3<T>(0, 1, 0), VectorT3<T>(0, 1, 0)) * reference, reference);
			OCEAN_EXPECT_EQUAL(validation, RotationT<T>(VectorT3<T>(0, 0, 1), VectorT3<T>(0, 0, 1)) * reference, reference);
			OCEAN_EXPECT_EQUAL(validation, RotationT<T>(offset, offset) * reference, reference);

			// 180 degrees test (a)
			OCEAN_EXPECT_EQUAL(validation, RotationT<T>(VectorT3<T>(1, 0, 0), VectorT3<T>(-1, 0, 0)) * VectorT3<T>(1, 0, 0), VectorT3<T>(-1, 0, 0));
			OCEAN_EXPECT_EQUAL(validation, RotationT<T>(VectorT3<T>(0, 1, 0), VectorT3<T>(0, -1, 0)) * VectorT3<T>(0, 1, 0), VectorT3<T>(0, -1, 0));
			OCEAN_EXPECT_EQUAL(validation, RotationT<T>(VectorT3<T>(0, 0, 1), VectorT3<T>(0, 0, -1)) * VectorT3<T>(0, 0, 1), VectorT3<T>(0, 0, -1));

			// 180 degrees test (b)
			OCEAN_EXPECT_EQUAL(validation, RotationT<T>(VectorT3<T>(-1, 0, 0), VectorT3<T>(1, 0, 0)) * VectorT3<T>(1, 0, 0), VectorT3<T>(-1, 0, 0));
			OCEAN_EXPECT_EQUAL(validation, RotationT<T>(VectorT3<T>(0, -1, 0), VectorT3<T>(0, 1, 0)) * VectorT3<T>(0, 1, 0), VectorT3<T>(0, -1, 0));
			OCEAN_EXPECT_EQUAL(validation, RotationT<T>(VectorT3<T>(0, 0, -1), VectorT3<T>(0, 0, 1)) * VectorT3<T>(0, 0, 1), VectorT3<T>(0, 0, -1));

			const RotationT<T> rotation0(reference, offset);
			const VectorT3<T> test0 = rotation0 * reference;

			OCEAN_EXPECT_TRUE(validation, offset.isEqual(test0, NumericT<T>::weakEps()) && offset.angle(test0) < NumericT<T>::deg2rad(T(0.1)));

			const RotationT<T> rotation1(reference, -reference);
			const VectorT3<T> test1 = rotation1 * reference;

			OCEAN_EXPECT_TRUE(validation, reference.isEqual(-test1, NumericT<T>::weakEps()) && reference.angle(test1) > NumericT<T>::deg2rad(T(179.9)));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestRotation::testLeft_R_right(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "left_R_right for '" << TypeNamer::name<T>() << "':";

	constexpr double successThreshold = std::is_same<float, T>::value ? 0.975 : 0.999;

	RandomGenerator randomGenerator;
	ValidationPrecision validation(successThreshold, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		{
			// testing identity

			const VectorT3<T> axis = RandomT<T>::vector3(randomGenerator);

			const RotationT<T> left_R_right = RotationT<T>::left_R_right(axis, axis);

			const RotationT<T> identity(VectorT3<T>(0, 1, 0), T(0));

			OCEAN_EXPECT_EQUAL(validation, left_R_right, identity);

			const VectorT3<T> vector = RandomT<T>::vector3(randomGenerator);

			OCEAN_EXPECT_TRUE(validation, vector.isEqual(left_R_right * vector, NumericT<T>::weakEps()));
		}

		{
			// testing flipped

			const VectorT3<T> axis = RandomT<T>::vector3(randomGenerator);

			{
				const RotationT<T> left_R_right = RotationT<T>::left_R_right(axis, -axis);

				OCEAN_EXPECT_TRUE(validation, axis.isEqual(left_R_right * -axis, NumericT<T>::weakEps()));

				OCEAN_EXPECT_TRUE(validation, (-axis).isEqual(left_R_right * axis, NumericT<T>::weakEps()));
			}

			{
				const RotationT<T> left_R_right = RotationT<T>::left_R_right(-axis, axis);

				OCEAN_EXPECT_TRUE(validation, axis.isEqual(left_R_right * -axis, NumericT<T>::weakEps()));

				OCEAN_EXPECT_TRUE(validation, (-axis).isEqual(left_R_right * axis, NumericT<T>::weakEps()));
			}
		}

		for (unsigned int n = 0u; n < 100u; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			// testing random vectors

			const VectorT3<T> leftVector = RandomT<T>::vector3(randomGenerator);
			const VectorT3<T> rightVector = RandomT<T>::vector3(randomGenerator);

			const RotationT<T> left_R_right = RotationT<T>::left_R_right(leftVector, rightVector);

			const VectorT3<T> testLeft = left_R_right * rightVector;

			if (!leftVector.isEqual(testLeft, NumericT<T>::eps()))
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
bool TestRotation::testInversion(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing inversion for '" << TypeNamer::name<T>() << "':";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const RotationT<T> rotation = RandomT<T>::rotation(randomGenerator);

		{
			// inverted()

			const RotationT<T> inverse = rotation.inverted();

			OCEAN_EXPECT_TRUE(validation, inverse.isValid());

			// axis should be negated, angle should be the same
			OCEAN_EXPECT_EQUAL(validation, inverse.axis(), -rotation.axis());
			OCEAN_EXPECT_EQUAL(validation, inverse.angle(), rotation.angle());

			// that rotation * inverse = identity
			const VectorT3<T> testVector = RandomT<T>::vector3(randomGenerator);
			const VectorT3<T> rotated = rotation * testVector;
			const VectorT3<T> restored = inverse * rotated;

			OCEAN_EXPECT_TRUE(validation, testVector.isEqual(restored, NumericT<T>::weakEps()));
		}

		{
			// invert()

			RotationT<T> rotationCopy = rotation;
			rotationCopy.invert();

			OCEAN_EXPECT_TRUE(validation, rotationCopy.isValid());

			// axis should be negated, angle should be the same
			OCEAN_EXPECT_EQUAL(validation, rotationCopy.axis(), -rotation.axis());
			OCEAN_EXPECT_EQUAL(validation, rotationCopy.angle(), rotation.angle());

			// Should be the same as inverted()
			OCEAN_EXPECT_EQUAL(validation, rotationCopy, rotation.inverted());
		}

		{
			// operator-() (should be the same as inverted())

			const RotationT<T> inverse = -rotation;

			OCEAN_EXPECT_TRUE(validation, inverse.isValid());
			OCEAN_EXPECT_EQUAL(validation, inverse, rotation.inverted());
		}

		{
			// double inversion

			const RotationT<T> doubleInverted = rotation.inverted().inverted();

			OCEAN_EXPECT_EQUAL(validation, doubleInverted, rotation);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestRotation::testComparisonOperators(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing comparison operators for '" << TypeNamer::name<T>() << "':";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const RotationT<T> rotation1 = RandomT<T>::rotation(randomGenerator);
		const RotationT<T> rotation2 = RandomT<T>::rotation(randomGenerator);

		{
			// operator==

			OCEAN_EXPECT_EQUAL(validation, rotation1, rotation1);

			const RotationT<T> equivalent(rotation1.axis(), rotation1.angle());
			OCEAN_EXPECT_EQUAL(validation, rotation1, equivalent);

			const RotationT<T> flipped(-rotation1.axis(), NumericT<T>::pi2() - rotation1.angle());
			OCEAN_EXPECT_EQUAL(validation, rotation1, flipped);
		}

		{
			// operator!=

			OCEAN_EXPECT_FALSE(validation, rotation1 != rotation1);

			const RotationT<T> equivalent(rotation1.axis(), rotation1.angle());
			OCEAN_EXPECT_FALSE(validation, rotation1 != equivalent);
		}

		{
			// reflexivity: a == a

			OCEAN_EXPECT_EQUAL(validation, rotation1, rotation1);
		}

		{
			// symmetry: if a == b, then b == a

			if (rotation1 == rotation2)
			{
				OCEAN_EXPECT_EQUAL(validation, rotation2, rotation1);
			}
		}

		{
			// consistency with !=: if a == b, then !(a != b)

			if (rotation1 == rotation2)
			{
				OCEAN_EXPECT_FALSE(validation, rotation1 != rotation2);
			}
			else
			{
				OCEAN_EXPECT_NOT_EQUAL(validation, rotation1, rotation2);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestRotation::testMultiplicationOperators(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing multiplication operators for '" << TypeNamer::name<T>() << "':";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const RotationT<T> rotation1 = RandomT<T>::rotation(randomGenerator);
		const RotationT<T> rotation2 = RandomT<T>::rotation(randomGenerator);

		const QuaternionT<T> quaternion = RandomT<T>::quaternion(randomGenerator);

		OCEAN_EXPECT_TRUE(validation, rotation1.isValid());
		OCEAN_EXPECT_TRUE(validation, rotation2.isValid());
		OCEAN_EXPECT_TRUE(validation, quaternion.isValid());

		{
			// operator*(Rotation)

			const RotationT<T> result = rotation1 * rotation2;

			OCEAN_EXPECT_TRUE(validation, result.isValid());

			const VectorT3<T> testVector = RandomT<T>::vector3(randomGenerator);
			const VectorT3<T> result1 = result * testVector;
			const VectorT3<T> result2 = rotation1 * (rotation2 * testVector);

			OCEAN_EXPECT_TRUE(validation, result1.isEqual(result2, NumericT<T>::weakEps()));
		}

		{
			// operator*=(Rotation)

			RotationT<T> rotationCopy = rotation1;
			rotationCopy *= rotation2;

			OCEAN_EXPECT_TRUE(validation, rotationCopy.isValid());

			const RotationT<T> rotationCopy2 = rotation1 * rotation2;

			OCEAN_EXPECT_TRUE(validation, rotationCopy.isEqual(rotationCopy2, NumericT<T>::weakEps()));
		}

		{
			// operator*(Quaternion)

			const RotationT<T> result = rotation1 * quaternion;

			OCEAN_EXPECT_TRUE(validation, result.isValid());

			const VectorT3<T> testVector = RandomT<T>::vector3(randomGenerator);
			const VectorT3<T> result1 = result * testVector;
			const VectorT3<T> result2 = rotation1 * (quaternion * testVector);

			OCEAN_EXPECT_TRUE(validation, result1.isEqual(result2, NumericT<T>::weakEps()));
		}

		{
			// operator*=(Quaternion)

			RotationT<T> rotationCopy = rotation1;
			rotationCopy *= quaternion;

			OCEAN_EXPECT_TRUE(validation, rotationCopy.isValid());

			const RotationT<T> rotationCopy2 = rotation1 * quaternion;

			OCEAN_EXPECT_EQUAL(validation, rotationCopy, rotationCopy2);
		}

		{
			// operator*(Vector3)

			const VectorT3<T> vector = RandomT<T>::vector3(randomGenerator);
			const VectorT3<T> rotated = rotation1 * vector;

			const QuaternionT<T> quat(rotation1);
			const VectorT3<T> rotated2 = quat * vector;

			OCEAN_EXPECT_TRUE(validation, rotated.isEqual(rotated2, NumericT<T>::weakEps()));
		}

		{
			// associativity: (r1 * r2) * v == r1 * (r2 * v)

			const VectorT3<T> vector = RandomT<T>::vector3(randomGenerator);
			const RotationT<T> combined = rotation1 * rotation2;

			const VectorT3<T> result1 = combined * vector;
			const VectorT3<T> result2 = rotation1 * (rotation2 * vector);

			OCEAN_EXPECT_TRUE(validation, result1.isEqual(result2, NumericT<T>::weakEps()));
		}

		{
			// identity: rotation * rotation^-1 * v == v

			const VectorT3<T> vector = RandomT<T>::vector3(randomGenerator);

			const RotationT<T> inverse = rotation1.inverted();
			const RotationT<T> identity = rotation1 * inverse;

			const VectorT3<T> result = identity * vector;

			OCEAN_EXPECT_TRUE(validation, vector.isEqual(result, NumericT<T>::weakEps()));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestRotation::testElementAccess(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing element access operators for '" << TypeNamer::name<T>() << "':";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const T eps = NumericT<T>::eps() * T(100);

		const VectorT3<T> axis = RandomT<T>::vector3(randomGenerator);
		const T angle = RandomT<T>::scalar(randomGenerator, T(0), NumericT<T>::pi2() - eps);

		const RotationT<T> rotation(axis, angle);

		{
			// operator()(index) const

			OCEAN_EXPECT_EQUAL(validation, rotation(0), axis.x());
			OCEAN_EXPECT_EQUAL(validation, rotation(1), axis.y());
			OCEAN_EXPECT_EQUAL(validation, rotation(2), axis.z());
			OCEAN_EXPECT_EQUAL(validation, rotation(3), angle);
		}

		{
			// operator[](index) const

			OCEAN_EXPECT_EQUAL(validation, rotation[0], axis.x());
			OCEAN_EXPECT_EQUAL(validation, rotation[1], axis.y());
			OCEAN_EXPECT_EQUAL(validation, rotation[2], axis.z());
			OCEAN_EXPECT_EQUAL(validation, rotation[3], angle);
		}

		{
			// operator()(index) non-const

			RotationT<T> rotationCopy = rotation;

			const T newValue = RandomT<T>::scalar(randomGenerator, T(-1), T(1));
			rotationCopy(0) = newValue;

			OCEAN_EXPECT_EQUAL(validation, rotationCopy(0), newValue);
		}

		{
			// operator[](index) non-const

			RotationT<T> rotationCopy = rotation;

			const T newValue = RandomT<T>::scalar(randomGenerator, T(-1), T(1));
			rotationCopy[1] = newValue;

			OCEAN_EXPECT_EQUAL(validation, rotationCopy[1], newValue);
		}

		{
			// operator()() const

			const T* data = rotation();

			OCEAN_EXPECT_EQUAL(validation, data[0], axis.x());
			OCEAN_EXPECT_EQUAL(validation, data[1], axis.y());
			OCEAN_EXPECT_EQUAL(validation, data[2], axis.z());
			OCEAN_EXPECT_EQUAL(validation, data[3], angle);
		}

		{
			// operator()() non-const

			RotationT<T> rotationCopy = rotation;
			T* data = rotationCopy();

			const T newValue = RandomT<T>::scalar(randomGenerator, T(-1), T(1));
			data[2] = newValue;

			OCEAN_EXPECT_EQUAL(validation, rotationCopy[2], newValue);
		}

		{
			// data() const

			const T* data = rotation.data();

			OCEAN_EXPECT_EQUAL(validation, data[0], axis.x());
			OCEAN_EXPECT_EQUAL(validation, data[1], axis.y());
			OCEAN_EXPECT_EQUAL(validation, data[2], axis.z());
			OCEAN_EXPECT_EQUAL(validation, data[3], angle);
		}

		{
			// data() non-const

			RotationT<T> rotationCopy = rotation;
			T* data = rotationCopy.data();

			const T newValue = RandomT<T>::scalar(randomGenerator, T(-1), T(1));
			data[3] = newValue;

			OCEAN_EXPECT_EQUAL(validation, rotationCopy.angle(), newValue);
		}

		{
			// consistency between different access methods

			OCEAN_EXPECT_EQUAL(validation, rotation(0), rotation[0]);
			OCEAN_EXPECT_EQUAL(validation, rotation(1), rotation[1]);
			OCEAN_EXPECT_EQUAL(validation, rotation(2), rotation[2]);
			OCEAN_EXPECT_EQUAL(validation, rotation(3), rotation[3]);

			OCEAN_EXPECT_EQUAL(validation, rotation[0], rotation.data()[0]);
			OCEAN_EXPECT_EQUAL(validation, rotation[1], rotation.data()[1]);
			OCEAN_EXPECT_EQUAL(validation, rotation[2], rotation.data()[2]);
			OCEAN_EXPECT_EQUAL(validation, rotation[3], rotation.data()[3]);

			OCEAN_EXPECT_EQUAL(validation, rotation.data()[0], rotation()[0]);
			OCEAN_EXPECT_EQUAL(validation, rotation.data()[1], rotation()[1]);
			OCEAN_EXPECT_EQUAL(validation, rotation.data()[2], rotation()[2]);
			OCEAN_EXPECT_EQUAL(validation, rotation.data()[3], rotation()[3]);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
