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

	allSucceeded = testConversionToQuaterion(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testConversionToHomogenousMatrix(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testReferenceOffsetConstructor(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
		Log::info() << "Rotation test succeeded.";
	else
		Log::info() << "Rotation test FAILED!";

	return allSucceeded;
}
	
#ifdef OCEAN_USE_GTEST
	
TEST(TestRotation, ConversionToQuaterion) {
	EXPECT_TRUE(TestRotation::testConversionToQuaterion(GTEST_TEST_DURATION));
}

TEST(TestRotation, ConversionToHomogenousMatrix) {
	EXPECT_TRUE(TestRotation::testConversionToHomogenousMatrix(GTEST_TEST_DURATION));
}

TEST(TestRotation, ReferenceOffsetConstructor) {
	EXPECT_TRUE(TestRotation::testReferenceOffsetConstructor(GTEST_TEST_DURATION));
}
	
#endif // OCEAN_USE_GTEST

bool TestRotation::testConversionToQuaterion(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Conversion from Rotation to Quaternion (and 3x3 matrix):";

	const unsigned int constIterations = 100000u;

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	Rotations rotations(constIterations);
	Quaternions quaternions(constIterations);

	const Scalar epsilon = std::is_same<Scalar, float>::value ? Scalar(0.02) : Numeric::weakEps();

	HighPerformanceStatistic performance;
	Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < constIterations; ++n)
			rotations[n] = Random::rotation();

		performance.start();

		for (unsigned int n = 0u; n < constIterations; ++n)
			quaternions[n] = Quaternion(rotations[n]);

		performance.stop();

		for (unsigned int n = 0u; n < constIterations; ++n)
		{
			const Rotation& rotation = rotations[n];
			const Quaternion& quaternion = quaternions[n];

			const SquareMatrix3 matrix(rotation);

			const Scalar angleX = Numeric::rad2deg((matrix * Vector3(1, 0, 0)).angle(quaternion * Vector3(1, 0, 0)));
			const Scalar angleY = Numeric::rad2deg((matrix * Vector3(0, 1, 0)).angle(quaternion * Vector3(0, 1, 0)));
			const Scalar angleZ = Numeric::rad2deg((matrix * Vector3(0, 0, 1)).angle(quaternion * Vector3(0, 0, 1)));

			if (Numeric::isEqual(angleX, 0, epsilon) && Numeric::isEqual(angleY, 0, epsilon) && Numeric::isEqual(angleZ, 0, epsilon))
				validIterations++;
		}

		iterations += constIterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	double percent = double(validIterations) / double(iterations);

	Log::info() << "Performance: " << performance.averageMseconds() * 1000.0 / double(constIterations) << "mys";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";
	Log::info() << " ";

	return percent >= 0.95;
}

bool TestRotation::testConversionToHomogenousMatrix(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Conversion from Rotation to Homogenous Matrix:";

	const unsigned int constIterations = 100000u;

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	Rotations rotations(constIterations);
	HomogenousMatrices4 matrices(constIterations);

	const Scalar epsilon = std::is_same<Scalar, float>::value ? Scalar(0.02) : Numeric::weakEps();

	HighPerformanceStatistic performance;
	Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < constIterations; ++n)
			rotations[n] = Random::rotation();

		performance.start();

		for (unsigned int n = 0u; n < constIterations; ++n)
			matrices[n] = HomogenousMatrix4(rotations[n]);

		performance.stop();

		for (unsigned int n = 0u; n < constIterations; ++n)
		{
			const Rotation& rotation = rotations[n];
			const HomogenousMatrix4& matrix = matrices[n];

			const Scalar angleX = Numeric::rad2deg((matrix * Vector3(1, 0, 0)).angle(rotation * Vector3(1, 0, 0)));
			const Scalar angleY = Numeric::rad2deg((matrix * Vector3(0, 1, 0)).angle(rotation * Vector3(0, 1, 0)));
			const Scalar angleZ = Numeric::rad2deg((matrix * Vector3(0, 0, 1)).angle(rotation * Vector3(0, 0, 1)));

			if (Numeric::isEqual(angleX, 0, epsilon) && Numeric::isEqual(angleY, 0, epsilon) && Numeric::isEqual(angleZ, 0, epsilon))
				validIterations++;
		}

		iterations += constIterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	double percent = double(validIterations) / double(iterations);

	Log::info() << "Performance: " << performance.averageMseconds() * 1000.0 / double(constIterations) << "mys";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.95;
}

bool TestRotation::testReferenceOffsetConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Reference offset constructor:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Vector3 reference(Random::vector3());
			const Vector3 offset(Random::vector3());

			// identity test
			if (Rotation(Vector3(1, 0, 0), Vector3(1, 0, 0)) * reference != reference)
				allSucceeded = false;
			if (Rotation(Vector3(0, 1, 0), Vector3(0, 1, 0)) * reference != reference)
				allSucceeded = false;
			if (Rotation(Vector3(0, 0, 1), Vector3(0, 0, 1)) * reference != reference)
				allSucceeded = false;
			if (Rotation(offset, offset) * reference != reference)
				allSucceeded = false;

			// 180 degrees test (a)
			if (Rotation(Vector3(1, 0, 0), Vector3(-1, 0, 0)) * Vector3(1, 0, 0) != Vector3(-1, 0, 0))
				allSucceeded = false;
			if (Rotation(Vector3(0, 1, 0), Vector3(0, -1, 0)) * Vector3(0, 1, 0) != Vector3(0, -1, 0))
				allSucceeded = false;
			if (Rotation(Vector3(0, 0, 1), Vector3(0, 0, -1)) * Vector3(0, 0, 1) != Vector3(0, 0, -1))
				allSucceeded = false;

			// 180 degrees test (b)
			if (Rotation(Vector3(-1, 0, 0), Vector3(1, 0, 0)) * Vector3(1, 0, 0) != Vector3(-1, 0, 0))
				allSucceeded = false;
			if (Rotation(Vector3(0, -1, 0), Vector3(0, 1, 0)) * Vector3(0, 1, 0) != Vector3(0, -1, 0))
				allSucceeded = false;
			if (Rotation(Vector3(0, 0, -1), Vector3(0, 0, 1)) * Vector3(0, 0, 1) != Vector3(0, 0, -1))
				allSucceeded = false;

			const Rotation rotation0(reference, offset);
			const Vector3 test0 = rotation0 * reference;

			if (!offset.isEqual(test0, Numeric::weakEps()) || offset.angle(test0) >= Numeric::deg2rad(Scalar(0.1)))
				allSucceeded = false;

			const Rotation rotation1(reference, -reference);
			const Vector3 test1 = rotation1 * reference;

			if (!reference.isEqual(-test1, Numeric::weakEps()) || reference.angle(test1) <= Numeric::deg2rad(Scalar(179.9)))
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

}

}

}
