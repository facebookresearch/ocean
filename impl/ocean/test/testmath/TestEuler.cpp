/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestEuler.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Euler.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestEuler::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Euler test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testConversionToMatrix(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testConversionFromRotation(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testConversionFromMatrix(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testDecomposeRotationMatrixToYXZ(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testDecomposeRotationMatrixToXYZ(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testAdjustAngles(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Euler test succeeded.";
	}
	else
	{
		Log::info() << "Euler test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestEuler, ConversionToMatrix)
{
	EXPECT_TRUE(TestEuler::testConversionToMatrix(GTEST_TEST_DURATION));
}

TEST(TestEuler, ConversionFromRotation)
{
	EXPECT_TRUE(TestEuler::testConversionFromRotation(GTEST_TEST_DURATION));
}

TEST(TestEuler, ConversionFromMatrix)
{
	EXPECT_TRUE(TestEuler::testConversionFromMatrix(GTEST_TEST_DURATION));
}

TEST(TestEuler, DecomposeRotationMatrixToYXZ)
{
	EXPECT_TRUE(TestEuler::testDecomposeRotationMatrixToYXZ(GTEST_TEST_DURATION));
}

TEST(TestEuler, DecomposeRotationMatrixToXYZ)
{
	EXPECT_TRUE(TestEuler::testDecomposeRotationMatrixToXYZ(GTEST_TEST_DURATION));
}

TEST(TestEuler, AdjustAngles)
{
	EXPECT_TRUE(TestEuler::testAdjustAngles(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestEuler::testConversionToMatrix(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Conversion from Euler to 3x3 rotation matrix:";

	const unsigned int constIterations = 100000u;

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	Rotations rotations(constIterations);
	SquareMatrices3 matrices(constIterations);

	const Scalar epsilon = std::is_same<Scalar, float>::value ? Scalar(0.02) : Numeric::weakEps();

	HighPerformanceStatistic performance;
	Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < constIterations; ++n)
		{
			rotations[n] = Random::rotation();
		}

		performance.start();

		for (unsigned int n = 0u; n < constIterations; ++n)
		{
			matrices[n] = SquareMatrix3(rotations[n]);
		}

		performance.stop();

		for (unsigned int n = 0u; n < constIterations; ++n)
		{
			const Rotation& rotation = rotations[n];
			const SquareMatrix3& matrix = matrices[n];

			const Scalar angleX = Numeric::rad2deg((matrix * Vector3(1, 0, 0)).angle(rotation * Vector3(1, 0, 0)));
			const Scalar angleY = Numeric::rad2deg((matrix * Vector3(0, 1, 0)).angle(rotation * Vector3(0, 1, 0)));
			const Scalar angleZ = Numeric::rad2deg((matrix * Vector3(0, 0, 1)).angle(rotation * Vector3(0, 0, 1)));

			if (Numeric::isEqual(angleX, 0, epsilon) && Numeric::isEqual(angleY, 0, epsilon) && Numeric::isEqual(angleZ, 0, epsilon))
			{
				validIterations++;
			}
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

bool TestEuler::testConversionFromRotation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test conversion from angle-axis rotation:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Scalar epsilon = std::is_same<Scalar, float>::value ? Scalar(0.02) : Numeric::weakEps();

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Rotation yRotation(0, 1, 0, Random::scalar(-Numeric::pi(), Numeric::pi()));
			const Rotation xRotation(1, 0, 0, Random::scalar(-Numeric::pi_2(), Numeric::pi_2()));
			const Rotation zRotation(0, 0, 1, Random::scalar(-Numeric::pi(), Numeric::pi()));

			const Rotation rotation(yRotation * xRotation * zRotation);
			const Euler euler(rotation);

			const Rotation yEuler(0, 1, 0, euler.yaw());
			const Rotation xEuler(1, 0, 0, euler.pitch());
			const Rotation zEuler(0, 0, 1, euler.roll());

			const Rotation eulerRotation(yEuler * xEuler * zEuler);

			bool localSucceeded = true;
			for (unsigned int i = 0u; i < 3u; ++i)
			{
				const Vector3 vector(Random::vector3());

				const Vector3 vectorA = rotation * vector;
				const Vector3 vectorB = eulerRotation * vector;

				const Scalar angle = Numeric::rad2deg(vectorA.angle(vectorB));

				if (Numeric::isNotEqual(angle, 0, epsilon))
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
	double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.95;
}

bool TestEuler::testConversionFromMatrix(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test conversion from 3x3 matrix:";

	const Scalar angleEpsilon = std::is_same<float, Scalar>::value ? Numeric::deg2rad(Scalar(0.02)) : Numeric::deg2rad(Scalar(0.001));

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			// here we create the rotation matrix by three angle-axis rotations with correct value ranges

			const Rotation yRotation(0, 1, 0, Random::scalar(-Numeric::pi(), Numeric::pi()));
			const Rotation xRotation(1, 0, 0, Random::scalar(-Numeric::pi_2(), Numeric::pi_2()));
			const Rotation zRotation(0, 0, 1, Random::scalar(-Numeric::pi(), Numeric::pi()));

			const SquareMatrix3 yMatrix(yRotation);
			const SquareMatrix3 xMatrix(xRotation);
			const SquareMatrix3 zMatrix(zRotation);

			const SquareMatrix3 matrix(yMatrix * xMatrix * zMatrix);

			const Euler euler(matrix);

			const SquareMatrix3 yEuler(Rotation(0, 1, 0, euler.yaw()));
			const SquareMatrix3 xEuler(Rotation(1, 0, 0, euler.pitch()));
			const SquareMatrix3 zEuler(Rotation(0, 0, 1, euler.roll()));

			const SquareMatrix3 eulerMatrix(yEuler * xEuler * zEuler);

			bool localSucceeded = true;

			for (unsigned int i = 0u; i < 3u; ++i)
			{
				const Vector3 vector(Random::vector3());

				const Vector3 vectorA = matrix * vector;
				const Vector3 vectorB = eulerMatrix * vector;

				const Scalar angle = vectorA.angle(vectorB);

				if (angle >= angleEpsilon)
				{
					localSucceeded = false;
				}
			}

			// we explicitly also checking the default axes

			if ((matrix * Vector3(1, 0, 0)).angle(eulerMatrix * Vector3(1, 0, 0)) >= angleEpsilon)
			{
				localSucceeded = false;
			}

			if ((matrix * Vector3(0, 1, 0)).angle(eulerMatrix * Vector3(0, 1, 0)) >= angleEpsilon)
			{
				localSucceeded = false;
			}

			if ((matrix * Vector3(0, 0, 1)).angle(eulerMatrix * Vector3(0, 0, 1)) >= angleEpsilon)
			{
				localSucceeded = false;
			}

			if (localSucceeded)
			{
				validIterations++;
			}

			iterations++;
		}

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			// here we create the rotation matrix by a random quaternion

			const SquareMatrix3 matrix(Random::quaternion());

			const Euler euler(matrix);

			const SquareMatrix3 yEuler(Rotation(0, 1, 0, euler.yaw()));
			const SquareMatrix3 xEuler(Rotation(1, 0, 0, euler.pitch()));
			const SquareMatrix3 zEuler(Rotation(0, 0, 1, euler.roll()));

			const SquareMatrix3 eulerMatrix(yEuler * xEuler * zEuler);

			bool localSucceeded = true;

			for (unsigned int i = 0u; i < 3u; ++i)
			{
				const Vector3 vector(Random::vector3());

				const Vector3 vectorA = matrix * vector;
				const Vector3 vectorB = eulerMatrix * vector;

				const Scalar angle = vectorA.angle(vectorB);

				if (angle >= angleEpsilon)
				{
					localSucceeded = false;
				}
			}

			// we explicitly also checking the default axes

			if ((matrix * Vector3(1, 0, 0)).angle(eulerMatrix * Vector3(1, 0, 0)) >= angleEpsilon)
			{
				localSucceeded = false;
			}

			if ((matrix * Vector3(0, 1, 0)).angle(eulerMatrix * Vector3(0, 1, 0)) >= angleEpsilon)
			{
				localSucceeded = false;
			}

			if ((matrix * Vector3(0, 0, 1)).angle(eulerMatrix * Vector3(0, 0, 1)) >= angleEpsilon)
			{
				localSucceeded = false;
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
	double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.95;
}

bool TestEuler::testDecomposeRotationMatrixToYXZ(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test decomposition of a 3x3 rotation matrix to y, x and z angle:";

	const Scalar angleEpsilon = std::is_same<float, Scalar>::value ? Numeric::deg2rad(Scalar(0.02)) : Numeric::deg2rad(Scalar(0.001));

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			// here we create the rotation matrix by three angle-axis rotations with correct value ranges

			const Rotation yRotation(1, 0, 0, Random::scalar(-Numeric::pi(), Numeric::pi()));
			const Rotation xRotation(0, 1, 0, Random::scalar(-Numeric::pi_2(), Numeric::pi_2()));
			const Rotation zRotation(0, 0, 1, Random::scalar(-Numeric::pi(), Numeric::pi()));

			const SquareMatrix3 xMatrix(xRotation);
			const SquareMatrix3 yMatrix(yRotation);
			const SquareMatrix3 zMatrix(zRotation);

			const SquareMatrix3 matrix(xMatrix * yMatrix * zMatrix);

			Scalar xAngle = Numeric::minValue();
			Scalar yAngle = Numeric::minValue();
			Scalar zAngle = Numeric::minValue();

			Euler::decomposeRotationMatrixToYXZ(matrix, yAngle, xAngle, zAngle);

			const SquareMatrix3 yDecompose(Rotation(0, 1, 0, yAngle));
			const SquareMatrix3 xDecompose(Rotation(1, 0, 0, xAngle));
			const SquareMatrix3 zDecompose(Rotation(0, 0, 1, zAngle));

			const SquareMatrix3 decomposeMatrix(yDecompose * xDecompose * zDecompose);

			bool localSucceeded = true;

			for (unsigned int i = 0u; i < 3u; ++i)
			{
				const Vector3 vector(Random::vector3());

				const Vector3 vectorA = matrix * vector;
				const Vector3 vectorB = decomposeMatrix * vector;

				const Scalar angle = vectorA.angle(vectorB);

				if (angle >= angleEpsilon)
				{
					localSucceeded = false;
				}
			}

			// we explicitly also checking the default axes

			if ((matrix * Vector3(1, 0, 0)).angle(decomposeMatrix * Vector3(1, 0, 0)) >= angleEpsilon)
			{
				localSucceeded = false;
			}

			if ((matrix * Vector3(0, 1, 0)).angle(decomposeMatrix * Vector3(0, 1, 0)) >= angleEpsilon)
			{
				localSucceeded = false;
			}

			if ((matrix * Vector3(0, 0, 1)).angle(decomposeMatrix * Vector3(0, 0, 1)) >= angleEpsilon)
			{
				localSucceeded = false;
			}

			if (localSucceeded)
			{
				validIterations++;
			}

			iterations++;
		}

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			// here we create the rotation matrix by a random quaternion

			const SquareMatrix3 matrix(Random::quaternion());

			Scalar xAngle = Numeric::minValue();
			Scalar yAngle = Numeric::minValue();
			Scalar zAngle = Numeric::minValue();

			Euler::decomposeRotationMatrixToYXZ(matrix, yAngle, xAngle, zAngle);

			const SquareMatrix3 yDecompose(Rotation(0, 1, 0, yAngle));
			const SquareMatrix3 xDecompose(Rotation(1, 0, 0, xAngle));
			const SquareMatrix3 zDecompose(Rotation(0, 0, 1, zAngle));

			const SquareMatrix3 decomposeMatrix(yDecompose * xDecompose * zDecompose);

			bool localSucceeded = true;

			for (unsigned int i = 0u; i < 3u; ++i)
			{
				const Vector3 vector(Random::vector3());

				const Vector3 vectorA = matrix * vector;
				const Vector3 vectorB = decomposeMatrix * vector;

				const Scalar angle = vectorA.angle(vectorB);

				if (angle >= angleEpsilon)
				{
					localSucceeded = false;
				}
			}

			// we explicitly also checking the default axes

			if ((matrix * Vector3(1, 0, 0)).angle(decomposeMatrix * Vector3(1, 0, 0)) >= angleEpsilon)
			{
				localSucceeded = false;
			}

			if ((matrix * Vector3(0, 1, 0)).angle(decomposeMatrix * Vector3(0, 1, 0)) >= angleEpsilon)
			{
				localSucceeded = false;
			}

			if ((matrix * Vector3(0, 0, 1)).angle(decomposeMatrix * Vector3(0, 0, 1)) >= angleEpsilon)
			{
				localSucceeded = false;
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
	double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.95;
}

bool TestEuler::testDecomposeRotationMatrixToXYZ(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test decomposition of a 3x3 rotation matrix to x, y and z angle:";

	const Scalar angleEpsilon = std::is_same<float, Scalar>::value ? Numeric::deg2rad(Scalar(0.02)) : Numeric::deg2rad(Scalar(0.001));

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			// here we create the rotation matrix by three angle-axis rotations with correct value ranges

			const Rotation xRotation(1, 0, 0, Random::scalar(-Numeric::pi(), Numeric::pi()));
			const Rotation yRotation(0, 1, 0, Random::scalar(-Numeric::pi_2(), Numeric::pi_2()));
			const Rotation zRotation(0, 0, 1, Random::scalar(-Numeric::pi(), Numeric::pi()));

			const SquareMatrix3 xMatrix(xRotation);
			const SquareMatrix3 yMatrix(yRotation);
			const SquareMatrix3 zMatrix(zRotation);

			const SquareMatrix3 matrix(xMatrix * yMatrix * zMatrix);

			Scalar xAngle = Numeric::minValue();
			Scalar yAngle = Numeric::minValue();
			Scalar zAngle = Numeric::minValue();

			Euler::decomposeRotationMatrixToXYZ(matrix, xAngle, yAngle, zAngle);

			const SquareMatrix3 xDecompose(Rotation(1, 0, 0, xAngle));
			const SquareMatrix3 yDecompose(Rotation(0, 1, 0, yAngle));
			const SquareMatrix3 zDecompose(Rotation(0, 0, 1, zAngle));

			const SquareMatrix3 decomposeMatrix(xDecompose * yDecompose * zDecompose);

			bool localSucceeded = true;

			for (unsigned int i = 0u; i < 3u; ++i)
			{
				const Vector3 vector(Random::vector3());

				const Vector3 vectorA = matrix * vector;
				const Vector3 vectorB = decomposeMatrix * vector;

				const Scalar angle = vectorA.angle(vectorB);

				if (angle >= angleEpsilon)
				{
					localSucceeded = false;
				}
			}

			// we explicitly also checking the default axes

			if ((matrix * Vector3(1, 0, 0)).angle(decomposeMatrix * Vector3(1, 0, 0)) >= angleEpsilon)
			{
				localSucceeded = false;
			}

			if ((matrix * Vector3(0, 1, 0)).angle(decomposeMatrix * Vector3(0, 1, 0)) >= angleEpsilon)
			{
				localSucceeded = false;
			}

			if ((matrix * Vector3(0, 0, 1)).angle(decomposeMatrix * Vector3(0, 0, 1)) >= angleEpsilon)
			{
				localSucceeded = false;
			}

			if (localSucceeded)
			{
				validIterations++;
			}

			iterations++;
		}

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			// here we create the rotation matrix by a random quaternion

			const SquareMatrix3 matrix(Random::quaternion());

			Scalar xAngle = Numeric::minValue();
			Scalar yAngle = Numeric::minValue();
			Scalar zAngle = Numeric::minValue();

			Euler::decomposeRotationMatrixToXYZ(matrix, xAngle, yAngle, zAngle);

			const SquareMatrix3 xDecompose(Rotation(1, 0, 0, xAngle));
			const SquareMatrix3 yDecompose(Rotation(0, 1, 0, yAngle));
			const SquareMatrix3 zDecompose(Rotation(0, 0, 1, zAngle));

			const SquareMatrix3 decomposeMatrix(xDecompose * yDecompose * zDecompose);

			bool localSucceeded = true;

			for (unsigned int i = 0u; i < 3u; ++i)
			{
				const Vector3 vector(Random::vector3());

				const Vector3 vectorA = matrix * vector;
				const Vector3 vectorB = decomposeMatrix * vector;

				const Scalar angle = vectorA.angle(vectorB);

				if (angle >= angleEpsilon)
				{
					localSucceeded = false;
				}
			}

			// we explicitly also checking the default axes

			if ((matrix * Vector3(1, 0, 0)).angle(decomposeMatrix * Vector3(1, 0, 0)) >= angleEpsilon)
			{
				localSucceeded = false;
			}

			if ((matrix * Vector3(0, 1, 0)).angle(decomposeMatrix * Vector3(0, 1, 0)) >= angleEpsilon)
			{
				localSucceeded = false;
			}

			if ((matrix * Vector3(0, 0, 1)).angle(decomposeMatrix * Vector3(0, 0, 1)) >= angleEpsilon)
			{
				localSucceeded = false;
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
	double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.95;
}

bool TestEuler::testAdjustAngles(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test adjustAngles():";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Scalar yaw = Random::scalar(randomGenerator, -10, 10);
			const Scalar pitch = Random::scalar(randomGenerator, -10, 10);
			const Scalar roll = Random::scalar(randomGenerator, -10, 10);

			/**
			 * Rotation matrix around x-axis R(x):
			 * [  1    0     0   ]
			 * [  0   cos  -sin  ]
			 * [  0   sin   cos  ]
			 */

			/**
			 * Rotation matrix around y-axis R(y):
			 * [   cos   0   sin  ]
			 * [    0    1    0   ]
			 * [  -sin   0   cos  ]
			 */

			/**
			 * Rotation matrix around z-axis R(z):
			 * [  cos   -sin   0  ]
			 * [  sin    cos   0  ]
			 * [   0      0    1  ]
			 */

			/**
			 * Combined rotation matrix for R(y)R(x)R(z)
			 * [  cy cz + sx sy sz     cz sx sy - cy sz      cx sy  ]
			 * [       cx sz                 cx cz            -sx   ]
			 * [  -cz sy + cy sx sz    cy cz sx + sy sz      cx cy  ]
			 */

			const Scalar cx = Numeric::cos(pitch);
			const Scalar sx = Numeric::sin(pitch);

			const Scalar cy = Numeric::cos(yaw);
			const Scalar sy = Numeric::sin(yaw);

			const Scalar cz = Numeric::cos(roll);
			const Scalar sz = Numeric::sin(roll);

			const Scalar rotationMatrixValues[9] =
			{
				cy * cz + sx * sy * sz,
				cx * sz,
				-cz * sy + cy * sx * sz,
				cz * sx * sy - cy * sz,
				cx * cz,
				cy * cz * sx + sy * sz,
				cx * sy,
				-sx,
				cx * cy
			};

			const SquareMatrix3 rotationMatrix(rotationMatrixValues);
			ocean_assert(Numeric::isEqual(rotationMatrix.determinant(), 1));

			Scalar adjustedYaw(yaw);
			Scalar adjustedPitch(pitch);
			Scalar adjustedRoll(roll);

			Euler::adjustAngles(adjustedYaw, adjustedPitch, adjustedRoll);
			const Euler euler(adjustedYaw, adjustedPitch, adjustedRoll);

			bool localSucceeded = true;

			if (!euler.isValid())
			{
				localSucceeded = false;
			}


			const Vector3 randomVector = Random::vector3(randomGenerator);

			const Vector3 resultVectorA = rotationMatrix * randomVector;
			const Vector3 resultVectorB = SquareMatrix3(euler) * randomVector;

			if (!resultVectorA.isEqual(resultVectorB, Numeric::weakEps()))
			{
				localSucceeded = false;
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
	double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.95;
}

}

}

}
