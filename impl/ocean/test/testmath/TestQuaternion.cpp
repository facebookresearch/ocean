/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestQuaternion.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestQuaternion::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Quaternion test:   ---";
	Log::info() << " ";

	allSucceeded = testWriteToMessenger() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConstructor(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNormalization(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testInverting(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConversionToRotation(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testReferenceOffsetConstructor(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAngle<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testAngle<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSlerp<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testSlerp<double>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Quaternion test succeeded.";
	}
	else
	{
		Log::info() << "Quaternion test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestQuaternion, WriteToMessenger)
{
	EXPECT_TRUE(TestQuaternion::testWriteToMessenger());
}

TEST(TestQuaternion, Constructor)
{
	EXPECT_TRUE(TestQuaternion::testConstructor(GTEST_TEST_DURATION));
}

TEST(TestQuaternion, Normalization)
{
	EXPECT_TRUE(TestQuaternion::testNormalization(GTEST_TEST_DURATION));
}

TEST(TestQuaternion, Inverting)
{
	EXPECT_TRUE(TestQuaternion::testInverting(GTEST_TEST_DURATION));
}

TEST(TestQuaternion, ConversionToRotation)
{
	EXPECT_TRUE(TestQuaternion::testConversionToRotation(GTEST_TEST_DURATION));
}

TEST(TestQuaternion, ReferenceOffsetConstructor)
{
	EXPECT_TRUE(TestQuaternion::testReferenceOffsetConstructor(GTEST_TEST_DURATION));
}


TEST(TestQuaternion, AngleFloat)
{
	EXPECT_TRUE(TestQuaternion::testAngle<float>(GTEST_TEST_DURATION));
}

TEST(TestQuaternion, AngleDouble)
{
	EXPECT_TRUE(TestQuaternion::testAngle<double>(GTEST_TEST_DURATION));
}


TEST(TestQuaternion, SlerpFloat)
{
	EXPECT_TRUE(TestQuaternion::testSlerp<float>(GTEST_TEST_DURATION));
}

TEST(TestQuaternion, SlerpDouble)
{
	EXPECT_TRUE(TestQuaternion::testSlerp<double>(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestQuaternion::testWriteToMessenger()
{
	Log::info() << "Write to messenger test:";

	// this is mainly a check whether the code does not compile or crash

	Log::info() << " ";

	Log::info() << Quaternion(Vector3(1, 0, 0), 0);
	Log::info() << "Quaternion: " << Quaternion(Vector3(1, 0, 0), 0);
	Log::info() << Quaternion(Vector3(1, 0, 0), 0) << " <- Quaternion";

	Log::info() << " ";
	Log::info() << "Validation succeeded.";

	return true;
}

bool TestQuaternion::testConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test constructor:";

	bool allSucceeded = true;

	{
		// quaternion with default constructor are valid and represent a identity rotation

		const Quaternion defaultConstructedQuaternion;

		if (defaultConstructedQuaternion.isValid() == false)
		{
			allSucceeded = false;
		}
		if (defaultConstructedQuaternion != Quaternion(Vector3(1, 0, 0), 0))
		{
			allSucceeded = false;
		}
	}

	{
		// quaternion using the boolean constructor with 'true' are valid and represent a identity rotation

		const Quaternion booleanConstructedQuaternion(true);

		if (booleanConstructedQuaternion.isValid() == false)
		{
			allSucceeded = false;
		}
		if (booleanConstructedQuaternion != Quaternion(Vector3(1, 0, 0), 0))
		{
			allSucceeded = false;
		}
	}

	{
		// quaternion using the boolean constructor with 'false' are invalid

		const Quaternion booleanConstructedQuaternion(false);

		if (booleanConstructedQuaternion.isValid() == true)
		{
			allSucceeded = false;
		}
	}

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const float w = RandomF::scalar(randomGenerator, -10.0f, 10.0f);
			const float x = RandomF::scalar(randomGenerator, -10.0f, 10.0f);
			const float y = RandomF::scalar(randomGenerator, -10.0f, 10.0f);
			const float z = RandomF::scalar(randomGenerator, -10.0f, 10.0f);

			const Quaternion quaternion = Quaternion(Scalar(w), Scalar(x), Scalar(y), Scalar(z));
			const Quaternion quaternionF(w, x, y, z);
			const QuaternionD quaternionD = QuaternionD(double(w), double(x), double(y), double(z));

			if (quaternion.w() != Scalar(w) || quaternion.x() != Scalar(x) || quaternion.y() != Scalar(y) || quaternion.z() != Scalar(z))
			{
				allSucceeded = false;
			}

			if (quaternionF.w() != w || quaternionF.x() != x || quaternionF.y() != y || quaternionF.z() != z)
			{
				allSucceeded = false;
			}

			if (quaternionD.w() != double(w) || quaternionD.x() != double(x) || quaternionD.y() != double(y) || quaternionD.z() != double(z))
			{
				allSucceeded = false;
			}

			const QuaternionD quaternionF2D(quaternionF);
			const QuaternionF quaternionD2F(quaternionD);

			if (NumericD::isNotWeakEqual(quaternionF2D.w(), double(w)) || NumericD::isNotWeakEqual(quaternionF2D.x(), double(x)) || NumericD::isNotWeakEqual(quaternionF2D.y(), double(y)) || NumericD::isNotWeakEqual(quaternionF2D.z(), double(z)))
			{
				allSucceeded = false;
			}

			if (NumericF::isNotWeakEqual(quaternionD2F.w(), w) || NumericF::isNotWeakEqual(quaternionD2F.x(), x) || NumericF::isNotWeakEqual(quaternionD2F.y(), y) || NumericF::isNotWeakEqual(quaternionD2F.z(), z))
			{
				allSucceeded = false;
			}

			const Quaternion quaternionCopy(quaternion);

			if (quaternionCopy.w() != Scalar(w) || quaternionCopy.x() != Scalar(x) || quaternionCopy.y() != Scalar(y) || quaternionCopy.z() != Scalar(z))
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

bool TestQuaternion::testNormalization(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test normalization:";

	bool allSucceeded = true;

	// first we check a quaternion which cannot be normalized

	{
		const Quaternion quaternion(0, 0, 0, 0);
		if (quaternion.isValid())
		{
			allSucceeded = false;
		}
	}

	{
		Quaternion quaternion(0, 0, 0, 0);
		const bool result = quaternion.normalize();

		if (result)
		{
			allSucceeded = false;
		}
	}

	{
		const Quaternion quaternion(0, 0, 0, 0);
		Quaternion normalizedQuaternion;
		const bool result = quaternion.normalize(normalizedQuaternion);

		if (result)
		{
			allSucceeded = false;
		}
	}

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Scalar w = Random::scalar(randomGenerator, -10, 10);
			const Scalar x = Random::scalar(randomGenerator, -10, 10);
			const Scalar y = Random::scalar(randomGenerator, -10, 10);
			const Scalar z = Random::scalar(randomGenerator, -10, 10);

			const Scalar length = Numeric::sqrt(Numeric::sqr(w) + Numeric::sqr(x) + Numeric::sqr(y) + Numeric::sqr(z));

			if (length > Numeric::weakEps())
			{
				Quaternion quaternion(w, x, y, z);
				const Quaternion normalizedQuaternion = quaternion.normalized();
				const Scalar newLength = Numeric::sqrt(Numeric::sqr(normalizedQuaternion.w()) + Numeric::sqr(normalizedQuaternion.x())
															+ Numeric::sqr(normalizedQuaternion.y()) + Numeric::sqr(normalizedQuaternion.z()));

				if (Numeric::isNotEqual(newLength, 1))
				{
					allSucceeded = false;
				}
			}

			{
				Quaternion quaternion(w, x, y, z);
				const bool result = quaternion.normalize();

				if (result)
				{
					if (length <= Scalar(0))
					{
						allSucceeded = false;
					}

					const Scalar newLength = Numeric::sqrt(Numeric::sqr(quaternion.w()) + Numeric::sqr(quaternion.x())
															+ Numeric::sqr(quaternion.y()) + Numeric::sqr(quaternion.z()));

					if (Numeric::isNotEqual(newLength, 1))
					{
						allSucceeded = false;
					}
				}
				else
				{
					if (length > Numeric::weakEps())
					{
						allSucceeded = false;
					}
				}
			}

			{
				const Quaternion quaternion(w, x, y, z);
				Quaternion normalizedQuaternion;
				const bool result = quaternion.normalize(normalizedQuaternion);

				if (result)
				{
					if (length <= Scalar(0))
					{
						allSucceeded = false;
					}

					const Scalar newLength = Numeric::sqrt(Numeric::sqr(normalizedQuaternion.w()) + Numeric::sqr(normalizedQuaternion.x())
															+ Numeric::sqr(normalizedQuaternion.y()) + Numeric::sqr(normalizedQuaternion.z()));

					if (Numeric::isNotEqual(newLength, 1))
					{
						allSucceeded = false;
					}
				}
				else
				{
					if (length > Numeric::weakEps())
					{
						allSucceeded = false;
					}
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

bool TestQuaternion::testInverting(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test inverting:";

	bool allSucceeded = true;

	// first we check a quaternion which cannot be inverted

	{
		const Quaternion quaternion(0, 0, 0, 0);
		if (quaternion.isValid())
		{
			allSucceeded = false;
		}
	}

	{
		Quaternion quaternion(0, 0, 0, 0);
		const bool result = quaternion.invert();

		if (result)
		{
			allSucceeded = false;
		}
	}

	{
		const Quaternion quaternion(0, 0, 0, 0);
		Quaternion invertedQuaternion;
		const bool result = quaternion.invert(invertedQuaternion);

		if (result)
		{
			allSucceeded = false;
		}
	}

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			Scalar w = Random::scalar(randomGenerator, -10, 10);
			Scalar x = Random::scalar(randomGenerator, -10, 10);
			Scalar y = Random::scalar(randomGenerator, -10, 10);
			Scalar z = Random::scalar(randomGenerator, -10, 10);

			const Scalar length = Numeric::sqrt(Numeric::sqr(w) + Numeric::sqr(x) + Numeric::sqr(y) + Numeric::sqr(z));

			if (Numeric::isNotEqualEps(length))
			{
				w /= length;
				x /= length;
				y /= length;
				z /= length;
			}

			if (length > Numeric::weakEps())
			{
				const Quaternion quaternion(w, x, y, z);
				const Quaternion invertedQuaternion = quaternion.inverted();

				const Quaternion identityQuaternionA(quaternion * invertedQuaternion);
				const Quaternion identityQuaternionB(invertedQuaternion * quaternion);

				if (identityQuaternionA != Quaternion())
				{
					allSucceeded = false;
				}

				if (identityQuaternionB != Quaternion())
				{
					allSucceeded = false;
				}
			}

			{
				Quaternion quaternion(w, x, y, z);
				const bool result = quaternion.invert();

				if (result)
				{
					if (length <= Scalar(0))
					{
						allSucceeded = false;
					}

					const Quaternion initialQuaternion(w, x, y, z);

					const Quaternion identityQuaternionA(initialQuaternion * quaternion);
					const Quaternion identityQuaternionB(quaternion * initialQuaternion);

					if (identityQuaternionA != Quaternion())
					{
						allSucceeded = false;
					}

					if (identityQuaternionB != Quaternion())
					{
						allSucceeded = false;
					}
				}
				else
				{
					if (length > Numeric::weakEps())
					{
						allSucceeded = false;
					}
				}
			}

			{
				const Quaternion quaternion(w, x, y, z);
				Quaternion invertedQuaternion;
				const bool result = quaternion.invert(invertedQuaternion);

				if (result)
				{
					if (length <= Scalar(0))
					{
						allSucceeded = false;
					}

					const Quaternion identityQuaternionA(quaternion * invertedQuaternion);
					const Quaternion identityQuaternionB(invertedQuaternion * quaternion);

					if (identityQuaternionA != Quaternion())
					{
						allSucceeded = false;
					}

					if (identityQuaternionB != Quaternion())
					{
						allSucceeded = false;
					}
				}
				else
				{
					if (length > Numeric::weakEps())
					{
						allSucceeded = false;
					}
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

bool TestQuaternion::testConversionToRotation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Conversion from Quaternion to Rotation (and 3x3 matrix):";

	const unsigned int constIterations = 100000u;

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	Quaternions quaternions(constIterations);
	Rotations rotations(constIterations);

	const Scalar epsilon = std::is_same<Scalar, float>::value ? Scalar(0.02) : Numeric::weakEps();

	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < constIterations; ++n)
		{
			quaternions[n] = Random::quaternion();
		}

		performance.start();
			for (unsigned int n = 0u; n < constIterations; ++n)
			{
				rotations[n] = Rotation(quaternions[n]);
			}
		performance.stop();

		for (unsigned int n = 0u; n < constIterations; ++n)
		{
			const Quaternion& quaternion = quaternions[n];
			const Rotation& rotation = rotations[n];

			const SquareMatrix3 matrix(rotation);

			const Scalar angleX = Numeric::rad2deg((matrix * Vector3(1, 0, 0)).angle(quaternion * Vector3(1, 0, 0)));
			const Scalar angleY = Numeric::rad2deg((matrix * Vector3(0, 1, 0)).angle(quaternion * Vector3(0, 1, 0)));
			const Scalar angleZ = Numeric::rad2deg((matrix * Vector3(0, 0, 1)).angle(quaternion * Vector3(0, 0, 1)));

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

bool TestQuaternion::testReferenceOffsetConstructor(const double testDuration)
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
			if (Quaternion(Vector3(1, 0, 0), Vector3(1, 0, 0)) * reference != reference)
			{
				allSucceeded = false;
			}
			if (Quaternion(Vector3(0, 1, 0), Vector3(0, 1, 0)) * reference != reference)
			{
				allSucceeded = false;
			}
			if (Quaternion(Vector3(0, 0, 1), Vector3(0, 0, 1)) * reference != reference)
			{
				allSucceeded = false;
			}
			if (Quaternion(offset, offset) * reference != reference)
			{
				allSucceeded = false;
			}

			Quaternion q(Rotation(1, 0, 0, Numeric::pi_2()));
			Quaternion q2(Rotation(0, 1, 0, Numeric::pi_2()));
			Quaternion q3(Rotation(0, 0, 1, Numeric::pi_2()));

			// 180 degrees test (a)
			if (Quaternion(Vector3(1, 0, 0), Vector3(-1, 0, 0)) * Vector3(1, 0, 0) != Vector3(-1, 0, 0))
			{
				allSucceeded = false;
			}
			if (Quaternion(Vector3(0, 1, 0), Vector3(0, -1, 0)) * Vector3(0, 1, 0) != Vector3(0, -1, 0))
			{
				allSucceeded = false;
			}
			if (Quaternion(Vector3(0, 0, 1), Vector3(0, 0, -1)) * Vector3(0, 0, 1) != Vector3(0, 0, -1))
			{
				allSucceeded = false;
			}

			// 180 degrees test (b)
			if (Quaternion(Vector3(-1, 0, 0), Vector3(1, 0, 0)) * Vector3(1, 0, 0) != Vector3(-1, 0, 0))
			{
				allSucceeded = false;
			}
			if (Quaternion(Vector3(0, -1, 0), Vector3(0, 1, 0)) * Vector3(0, 1, 0) != Vector3(0, -1, 0))
			{
				allSucceeded = false;
			}
			if (Quaternion(Vector3(0, 0, -1), Vector3(0, 0, 1)) * Vector3(0, 0, 1) != Vector3(0, 0, -1))
			{
				allSucceeded = false;
			}

			const Quaternion quaternion0(reference, offset);
			const Vector3 test0 = quaternion0 * reference;

			if (!offset.isEqual(test0, Numeric::weakEps()) || offset.angle(test0) >= Numeric::deg2rad(Scalar(0.1)))
			{
				allSucceeded = false;
			}

			const Quaternion quaternion1(reference, -reference);
			const Vector3 test1 = quaternion1 * reference;

			if (!reference.isEqual(-test1, Numeric::weakEps()) || reference.angle(test1) <= Numeric::deg2rad(Scalar(179.9)))
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
bool TestQuaternion::testAngle(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Angle for '" << TypeNamer::name<T>() << "':";

	bool allSucceeded = true;

	// we check some fixed rotations

	if (NumericT<T>::isNotEqual(QuaternionT<T>().angle(), 0))
	{
		allSucceeded = false;
	}

	if (NumericT<T>::isNotEqual(QuaternionT<T>(VectorT3<T>(1, 0, 0), NumericT<T>::pi_4()).angle(), NumericT<T>::pi_4()))
	{
		allSucceeded = false;
	}

	if (NumericT<T>::isNotEqual(QuaternionT<T>(VectorT3<T>(0, 1, 0), NumericT<T>::pi_4()).angle(), NumericT<T>::pi_4()))
	{
		allSucceeded = false;
	}

	if (NumericT<T>::isNotEqual(QuaternionT<T>(VectorT3<T>(0, 0, 1), NumericT<T>::pi_4()).angle(), NumericT<T>::pi_4()))
	{
		allSucceeded = false;
	}

	if (NumericT<T>::isNotEqual(QuaternionT<T>(VectorT3<T>(1, 0, 0), NumericT<T>::pi()).angle(), NumericT<T>::pi()))
	{
		allSucceeded = false;
	}

	if (NumericT<T>::isNotEqual(QuaternionT<T>(VectorT3<T>(0, 1, 0), NumericT<T>::pi()).angle(), NumericT<T>::pi()))
	{
		allSucceeded = false;
	}

	if (NumericT<T>::isNotEqual(QuaternionT<T>(VectorT3<T>(0, 0, 1), NumericT<T>::pi()).angle(), NumericT<T>::pi()))
	{
		allSucceeded = false;
	}

	const T epsilon = std::is_same<T, float>::value ? T(0.1) : T(0.01);

	const Timestamp startTimestamp(true);

	do
	{
		const VectorT3<T> axis = RandomT<T>::vector3();
		ocean_assert(NumericT<T>::isEqual(axis.length(), 1));

		const T angle = RandomT<T>::scalar(0, NumericT<T>::pi());

		// we create a quaternion based on axis/angle and check whether the calculated angle is correct

		const QuaternionT<T> quaternion(axis, angle);

		if (NumericT<T>::isNotEqual(quaternion.angle(), angle, NumericT<T>::deg2rad(epsilon)))
		{
			allSucceeded = false;
		}

		// we rotated a vector (perpendicular to the rotation axis) and check whether the angle between vector and rotated vector is correct

		const VectorT3<T> vector = axis.perpendicular();
		ocean_assert(NumericT<T>::isWeakEqualEps(axis * vector));

		const VectorT3<T> rotatedVector = quaternion * vector;

		const T vectorAngle = vector.angle(rotatedVector);

		if (NumericT<T>::isNotEqual(vectorAngle, angle, NumericT<T>::deg2rad(epsilon)))
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
bool TestQuaternion::testSlerp(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Slerp for '" << TypeNamer::name<T>() << "':";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const VectorT3<T> vectorA = RandomT<T>::vector3();
		const VectorT3<T> vectorB = RandomT<T>::vector3();

		const QuaternionT<T> vectorB_Q_vectorA(vectorA, vectorB);

		const T angleAB = vectorA.angle(vectorB);
		ocean_assert(angleAB >= T(0) && angleAB <= NumericT<T>::pi());

		for (unsigned int nFactor = 0u; nFactor <= 100u; ++nFactor)
		{
			const T factor = T(nFactor) / T(100);
			ocean_assert(factor >= T(0) && factor <= T(1));

			{
				// forward interpolation

				const QuaternionT<T> slerpQuaternion = QuaternionT<T>(true).slerp(vectorB_Q_vectorA, factor);

				const VectorT3<T> slerpVector = slerpQuaternion * vectorA;

				constexpr T epsilonSimilarity = std::is_same<T, float>::value ? NumericT<T>::weakEps() : NumericT<T>::eps();

				if (nFactor == 0u)
				{
					if (!slerpVector.isEqual(vectorA, epsilonSimilarity))
					{
						allSucceeded = false;
					}
				}
				else if (nFactor == 100u)
				{
					if (!slerpVector.isEqual(vectorB, epsilonSimilarity))
					{
						allSucceeded = false;
					}
				}

				const T angleA = NumericT<T>::rad2deg(vectorA.angle(slerpVector));
				const T angleB = NumericT<T>::rad2deg(vectorB.angle(slerpVector));

				const T expectedAngleA = NumericT<T>::rad2deg(angleAB * factor);
				const T expectedAngleB = NumericT<T>::rad2deg(angleAB * (T(1) - factor));

				constexpr T epsilonAngle = std::is_same<T, float>::value ? T(0.1) : T(0.01);

				if (NumericT<T>::isNotEqual(angleA, expectedAngleA, epsilonAngle))
				{
					Log::info() << angleA - expectedAngleA;

					allSucceeded = false;
				}

				if (NumericT<T>::isNotEqual(angleB, expectedAngleB, epsilonAngle))
				{
					Log::info() << angleB - expectedAngleB;

					allSucceeded = false;
				}
			}

			{
				// backward interpolation

				const QuaternionT<T> slerpQuaternion = vectorB_Q_vectorA.slerp(QuaternionT<T>(true), factor);

				const VectorT3<T> slerpVector = slerpQuaternion * vectorA;

				constexpr T epsilonSimilarity = std::is_same<T, float>::value ? NumericT<T>::weakEps() : NumericT<T>::eps();

				if (nFactor == 0u)
				{
					if (!slerpVector.isEqual(vectorB, epsilonSimilarity))
					{
						allSucceeded = false;
					}
				}
				else if (nFactor == 100u)
				{
					if (!slerpVector.isEqual(vectorA, epsilonSimilarity))
					{
						allSucceeded = false;
					}
				}

				const T angleA = NumericT<T>::rad2deg(vectorA.angle(slerpVector));
				const T angleB = NumericT<T>::rad2deg(vectorB.angle(slerpVector));

				const T expectedAngleA = NumericT<T>::rad2deg(angleAB * (T(1) - factor));
				const T expectedAngleB = NumericT<T>::rad2deg(angleAB * factor);

				constexpr T epsilonAngle = std::is_same<T, float>::value ? T(0.1) : T(0.01);

				if (NumericT<T>::isNotEqual(angleA, expectedAngleA, epsilonAngle))
				{
					Log::info() << angleA - expectedAngleA;

					allSucceeded = false;
				}

				if (NumericT<T>::isNotEqual(angleB, expectedAngleB, epsilonAngle))
				{
					Log::info() << angleB - expectedAngleB;

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

}

}

}
