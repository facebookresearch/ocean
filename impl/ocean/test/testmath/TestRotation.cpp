/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestRotation.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Random.h"

#include "ocean/test/Validation.h"
#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestRotation::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Rotation test:   ---";
	Log::info() << " ";

	allSucceeded = testConversionToQuaterion<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testConversionToQuaterion<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConversionToHomogenousMatrix<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testConversionToHomogenousMatrix<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testReferenceOffsetConstructor<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testReferenceOffsetConstructor<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testLeft_R_right<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testLeft_R_right<double>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Rotation test succeeded.";
	}
	else
	{
		Log::info() << "Rotation test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

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

#endif // OCEAN_USE_GTEST

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
			rotations[n] = RandomT<T>::rotation();
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
			rotations[n] = RandomT<T>::rotation();
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
			const VectorT3<T> reference(RandomT<T>::vector3());
			const VectorT3<T> offset(RandomT<T>::vector3());

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

			if (left_R_right != identity)
			{
				OCEAN_SET_FAILED(validation);
			}

			const VectorT3<T> vector = RandomT<T>::vector3(randomGenerator);

			if (!vector.isEqual(left_R_right * vector, NumericT<T>::weakEps()))
			{
				OCEAN_SET_FAILED(validation);
			}
		}

		{
			// testing flipped

			const VectorT3<T> axis = RandomT<T>::vector3(randomGenerator);

			{
				const RotationT<T> left_R_right = RotationT<T>::left_R_right(axis, -axis);

				if (!axis.isEqual(left_R_right * -axis, NumericT<T>::weakEps()))
				{
					OCEAN_SET_FAILED(validation);
				}

				if (!(-axis).isEqual(left_R_right * axis, NumericT<T>::weakEps()))
				{
					OCEAN_SET_FAILED(validation);
				}
			}

			{
				const RotationT<T> left_R_right = RotationT<T>::left_R_right(-axis, axis);

				if (!axis.isEqual(left_R_right * -axis, NumericT<T>::weakEps()))
				{
					OCEAN_SET_FAILED(validation);
				}

				if (!(-axis).isEqual(left_R_right * axis, NumericT<T>::weakEps()))
				{
					OCEAN_SET_FAILED(validation);
				}
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

}

}

}
