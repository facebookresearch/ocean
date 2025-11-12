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

#include "ocean/test/Validation.h"
#include "ocean/test/ValidationPrecision.h"

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

	allSucceeded = testWriteToMessenger<float>() && allSucceeded;
	Log::info() << " ";
	allSucceeded = testWriteToMessenger<double>() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConstructor(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNormalization<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalization<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testInverting<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testInverting<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConversionToRotation<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testConversionToRotation<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testReferenceOffsetConstructor<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testReferenceOffsetConstructor<double>(testDuration) && allSucceeded;

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

TEST(TestQuaternion, WriteToMessenger_float)
{
	EXPECT_TRUE(TestQuaternion::testWriteToMessenger<float>());
}

TEST(TestQuaternion, WriteToMessenger_double)
{
	EXPECT_TRUE(TestQuaternion::testWriteToMessenger<double>());
}


TEST(TestQuaternion, Constructor)
{
	EXPECT_TRUE(TestQuaternion::testConstructor(GTEST_TEST_DURATION));
}


TEST(TestQuaternion, Normalization_float)
{
	EXPECT_TRUE(TestQuaternion::testNormalization<float>(GTEST_TEST_DURATION));
}

TEST(TestQuaternion, Normalization_double)
{
	EXPECT_TRUE(TestQuaternion::testNormalization<double>(GTEST_TEST_DURATION));
}


TEST(TestQuaternion, Inverting_float)
{
	EXPECT_TRUE(TestQuaternion::testInverting<float>(GTEST_TEST_DURATION));
}

TEST(TestQuaternion, Inverting_double)
{
	EXPECT_TRUE(TestQuaternion::testInverting<double>(GTEST_TEST_DURATION));
}


TEST(TestQuaternion, ConversionToRotation_float)
{
	EXPECT_TRUE(TestQuaternion::testConversionToRotation<float>(GTEST_TEST_DURATION));
}

TEST(TestQuaternion, ConversionToRotation_double)
{
	EXPECT_TRUE(TestQuaternion::testConversionToRotation<double>(GTEST_TEST_DURATION));
}


TEST(TestQuaternion, ReferenceOffsetConstructor_float)
{
	EXPECT_TRUE(TestQuaternion::testReferenceOffsetConstructor<float>(GTEST_TEST_DURATION));
}

TEST(TestQuaternion, ReferenceOffsetConstructor_double)
{
	EXPECT_TRUE(TestQuaternion::testReferenceOffsetConstructor<double>(GTEST_TEST_DURATION));
}


TEST(TestQuaternion, Angle_float)
{
	EXPECT_TRUE(TestQuaternion::testAngle<float>(GTEST_TEST_DURATION));
}

TEST(TestQuaternion, Angle_double)
{
	EXPECT_TRUE(TestQuaternion::testAngle<double>(GTEST_TEST_DURATION));
}


TEST(TestQuaternion, Slerp_float)
{
	EXPECT_TRUE(TestQuaternion::testSlerp<float>(GTEST_TEST_DURATION));
}

TEST(TestQuaternion, Slerp_double)
{
	EXPECT_TRUE(TestQuaternion::testSlerp<double>(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

template <typename T>
bool TestQuaternion::testWriteToMessenger()
{
	Log::info() << "Write to messenger test for '" << TypeNamer::name<T>() << "':";

	// this is mainly a check whether the code does not compile or crash

	Log::info() << " ";

	Log::info() << QuaternionT<T>(VectorT3<T>(1, 0, 0), 0);
	Log::info() << "Quaternion: " << QuaternionT<T>(VectorT3<T>(1, 0, 0), 0);
	Log::info() << QuaternionT<T>(VectorT3<T>(1, 0, 0), 0) << " <- Quaternion";

	Log::info() << " ";
	Log::info() << "Validation succeeded.";

	return true;
}

bool TestQuaternion::testConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test constructor:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	{
		// quaternion with default constructor are valid and represent a identity rotation

		const Quaternion defaultConstructedQuaternion;

		OCEAN_EXPECT_TRUE(validation, defaultConstructedQuaternion.isValid());
		OCEAN_EXPECT_EQUAL(validation, defaultConstructedQuaternion, Quaternion(Vector3(1, 0, 0), 0));
	}

	{
		// quaternion using the boolean constructor with 'true' are valid and represent a identity rotation

		const Quaternion booleanConstructedQuaternion(true);

		OCEAN_EXPECT_TRUE(validation, booleanConstructedQuaternion.isValid());
		OCEAN_EXPECT_EQUAL(validation, booleanConstructedQuaternion, Quaternion(Vector3(1, 0, 0), 0));
	}

	{
		// quaternion using the boolean constructor with 'false' are invalid

		const Quaternion booleanConstructedQuaternion(false);

		OCEAN_EXPECT_FALSE(validation, booleanConstructedQuaternion.isValid());
	}

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
			const QuaternionF quaternionF(w, x, y, z);
			const QuaternionD quaternionD = QuaternionD(double(w), double(x), double(y), double(z));

			OCEAN_EXPECT_EQUAL(validation, quaternion.w(), Scalar(w));
			OCEAN_EXPECT_EQUAL(validation, quaternion.x(), Scalar(x));
			OCEAN_EXPECT_EQUAL(validation, quaternion.y(), Scalar(y));
			OCEAN_EXPECT_EQUAL(validation, quaternion.z(), Scalar(z));

			OCEAN_EXPECT_EQUAL(validation, quaternionF.w(), w);
			OCEAN_EXPECT_EQUAL(validation, quaternionF.x(), x);
			OCEAN_EXPECT_EQUAL(validation, quaternionF.y(), y);
			OCEAN_EXPECT_EQUAL(validation, quaternionF.z(), z);

			OCEAN_EXPECT_EQUAL(validation, quaternionD.w(), double(w));
			OCEAN_EXPECT_EQUAL(validation, quaternionD.x(), double(x));
			OCEAN_EXPECT_EQUAL(validation, quaternionD.y(), double(y));
			OCEAN_EXPECT_EQUAL(validation, quaternionD.z(), double(z));

			const QuaternionD quaternionF2D(quaternionF);
			const QuaternionF quaternionD2F(quaternionD);

			OCEAN_EXPECT_TRUE(validation, NumericD::isWeakEqual(quaternionF2D.w(), double(w)));
			OCEAN_EXPECT_TRUE(validation, NumericD::isWeakEqual(quaternionF2D.x(), double(x)));
			OCEAN_EXPECT_TRUE(validation, NumericD::isWeakEqual(quaternionF2D.y(), double(y)));
			OCEAN_EXPECT_TRUE(validation, NumericD::isWeakEqual(quaternionF2D.z(), double(z)));

			OCEAN_EXPECT_TRUE(validation, NumericF::isWeakEqual(quaternionD2F.w(), w));
			OCEAN_EXPECT_TRUE(validation, NumericF::isWeakEqual(quaternionD2F.x(), x));
			OCEAN_EXPECT_TRUE(validation, NumericF::isWeakEqual(quaternionD2F.y(), y));
			OCEAN_EXPECT_TRUE(validation, NumericF::isWeakEqual(quaternionD2F.z(), z));

			const Quaternion quaternionCopy(quaternion);

			OCEAN_EXPECT_EQUAL(validation, quaternionCopy.w(), Scalar(w));
			OCEAN_EXPECT_EQUAL(validation, quaternionCopy.x(), Scalar(x));
			OCEAN_EXPECT_EQUAL(validation, quaternionCopy.y(), Scalar(y));
			OCEAN_EXPECT_EQUAL(validation, quaternionCopy.z(), Scalar(z));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestQuaternion::testNormalization(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test normalization for '" << TypeNamer::name<T>() << "':";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	// first we check a quaternion which cannot be normalized

	{
		const QuaternionT<T> quaternion(0, 0, 0, 0);
		OCEAN_EXPECT_FALSE(validation, quaternion.isValid());
	}

	{
		QuaternionT<T> quaternion(0, 0, 0, 0);
		const bool result = quaternion.normalize();

		OCEAN_EXPECT_FALSE(validation, result);
	}

	{
		const QuaternionT<T> quaternion(0, 0, 0, 0);
		QuaternionT<T> normalizedQuaternion;
		const bool result = quaternion.normalize(normalizedQuaternion);

		OCEAN_EXPECT_FALSE(validation, result);
	}

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const T w = RandomT<T>::scalar(randomGenerator, -10, 10);
			const T x = RandomT<T>::scalar(randomGenerator, -10, 10);
			const T y = RandomT<T>::scalar(randomGenerator, -10, 10);
			const T z = RandomT<T>::scalar(randomGenerator, -10, 10);

			const T length = NumericT<T>::sqrt(NumericT<T>::sqr(w) + NumericT<T>::sqr(x) + NumericT<T>::sqr(y) + NumericT<T>::sqr(z));

			if (length > NumericT<T>::weakEps())
			{
				QuaternionT<T> quaternion(w, x, y, z);
				const QuaternionT<T> normalizedQuaternion = quaternion.normalized();
				const T newLength = NumericT<T>::sqrt(NumericT<T>::sqr(normalizedQuaternion.w()) + NumericT<T>::sqr(normalizedQuaternion.x()) + NumericT<T>::sqr(normalizedQuaternion.y()) + NumericT<T>::sqr(normalizedQuaternion.z()));

				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(newLength, 1));
			}

			{
				QuaternionT<T> quaternion(w, x, y, z);
				const bool result = quaternion.normalize();

				if (result)
				{
					OCEAN_EXPECT_TRUE(validation, length > T(0));

					const T newLength = NumericT<T>::sqrt(NumericT<T>::sqr(quaternion.w()) + NumericT<T>::sqr(quaternion.x()) + NumericT<T>::sqr(quaternion.y()) + NumericT<T>::sqr(quaternion.z()));

					OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(newLength, 1));
				}
				else
				{
					OCEAN_EXPECT_FALSE(validation, length > NumericT<T>::weakEps());
				}
			}

			{
				const QuaternionT<T> quaternion(w, x, y, z);
				QuaternionT<T> normalizedQuaternion;
				const bool result = quaternion.normalize(normalizedQuaternion);

				if (result)
				{
					OCEAN_EXPECT_TRUE(validation, length > T(0));

					const T newLength = NumericT<T>::sqrt(NumericT<T>::sqr(normalizedQuaternion.w()) + NumericT<T>::sqr(normalizedQuaternion.x()) + NumericT<T>::sqr(normalizedQuaternion.y()) + NumericT<T>::sqr(normalizedQuaternion.z()));

					OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(newLength, 1));
				}
				else
				{
					OCEAN_EXPECT_FALSE(validation, length > NumericT<T>::weakEps());
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestQuaternion::testInverting(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test inverting for '" << TypeNamer::name<T>() << "':";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	// first we check a quaternion which cannot be inverted

	{
		const QuaternionT<T> quaternion(0, 0, 0, 0);
		OCEAN_EXPECT_FALSE(validation, quaternion.isValid());
	}

	{
		QuaternionT<T> quaternion(0, 0, 0, 0);
		const bool result = quaternion.invert();

		OCEAN_EXPECT_FALSE(validation, result);
	}

	{
		const QuaternionT<T> quaternion(0, 0, 0, 0);
		QuaternionT<T> invertedQuaternion;
		const bool result = quaternion.invert(invertedQuaternion);

		OCEAN_EXPECT_FALSE(validation, result);
	}

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			T w = RandomT<T>::scalar(randomGenerator, -10, 10);
			T x = RandomT<T>::scalar(randomGenerator, -10, 10);
			T y = RandomT<T>::scalar(randomGenerator, -10, 10);
			T z = RandomT<T>::scalar(randomGenerator, -10, 10);

			const T length = NumericT<T>::sqrt(NumericT<T>::sqr(w) + NumericT<T>::sqr(x) + NumericT<T>::sqr(y) + NumericT<T>::sqr(z));

			if (NumericT<T>::isNotEqualEps(length))
			{
				w /= length;
				x /= length;
				y /= length;
				z /= length;
			}

			if (length > NumericT<T>::weakEps())
			{
				const QuaternionT<T> quaternion(w, x, y, z);
				const QuaternionT<T> invertedQuaternion = quaternion.inverted();

				const QuaternionT<T> identityQuaternionA(quaternion * invertedQuaternion);
				const QuaternionT<T> identityQuaternionB(invertedQuaternion * quaternion);

				OCEAN_EXPECT_EQUAL(validation, identityQuaternionA, QuaternionT<T>());
				OCEAN_EXPECT_EQUAL(validation, identityQuaternionB, QuaternionT<T>());
			}

			{
				QuaternionT<T> quaternion(w, x, y, z);
				const bool result = quaternion.invert();

				if (result)
				{
					OCEAN_EXPECT_TRUE(validation, length > T(0));

					const QuaternionT<T> initialQuaternion(w, x, y, z);

					const QuaternionT<T> identityQuaternionA(initialQuaternion * quaternion);
					const QuaternionT<T> identityQuaternionB(quaternion * initialQuaternion);

					OCEAN_EXPECT_EQUAL(validation, identityQuaternionA, QuaternionT<T>());
					OCEAN_EXPECT_EQUAL(validation, identityQuaternionB, QuaternionT<T>());
				}
				else
				{
					OCEAN_EXPECT_FALSE(validation, length > NumericT<T>::weakEps());
				}
			}

			{
				const QuaternionT<T> quaternion(w, x, y, z);
				QuaternionT<T> invertedQuaternion;
				const bool result = quaternion.invert(invertedQuaternion);

				if (result)
				{
					OCEAN_EXPECT_TRUE(validation, length > T(0));

					const QuaternionT<T> identityQuaternionA(quaternion * invertedQuaternion);
					const QuaternionT<T> identityQuaternionB(invertedQuaternion * quaternion);

					OCEAN_EXPECT_EQUAL(validation, identityQuaternionA, QuaternionT<T>());
					OCEAN_EXPECT_EQUAL(validation, identityQuaternionB, QuaternionT<T>());
				}
				else
				{
					OCEAN_EXPECT_FALSE(validation, length > NumericT<T>::weakEps());
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestQuaternion::testConversionToRotation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Conversion from Quaternion to Rotation (and 3x3 matrix) for '" << TypeNamer::name<T>() << "':";

	constexpr double successThreshold = 0.95;
	constexpr unsigned int constIterations = 100000u;

	RandomGenerator randomGenerator;
	ValidationPrecision validation(successThreshold, randomGenerator);

	std::vector<QuaternionT<T>> quaternions(constIterations);
	std::vector<RotationT<T>> rotations(constIterations);

	const T epsilon = std::is_same<T, float>::value ? T(0.02) : NumericT<T>::weakEps();

	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < constIterations; ++n)
		{
			quaternions[n] = RandomT<T>::quaternion();
		}

		performance.start();
			for (unsigned int n = 0u; n < constIterations; ++n)
			{
				rotations[n] = RotationT<T>(quaternions[n]);
			}
		performance.stop();

		for (unsigned int n = 0u; n < constIterations; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const QuaternionT<T>& quaternion = quaternions[n];
			const RotationT<T>& rotation = rotations[n];

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

	return validation.succeeded();
}

template <typename T>
bool TestQuaternion::testReferenceOffsetConstructor(const double testDuration)
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
			OCEAN_EXPECT_EQUAL(validation, QuaternionT<T>(VectorT3<T>(1, 0, 0), VectorT3<T>(1, 0, 0)) * reference, reference);
			OCEAN_EXPECT_EQUAL(validation, QuaternionT<T>(VectorT3<T>(0, 1, 0), VectorT3<T>(0, 1, 0)) * reference, reference);
			OCEAN_EXPECT_EQUAL(validation, QuaternionT<T>(VectorT3<T>(0, 0, 1), VectorT3<T>(0, 0, 1)) * reference, reference);
			OCEAN_EXPECT_EQUAL(validation, QuaternionT<T>(offset, offset) * reference, reference);

			QuaternionT<T> q(RotationT<T>(1, 0, 0, NumericT<T>::pi_2()));
			QuaternionT<T> q2(RotationT<T>(0, 1, 0, NumericT<T>::pi_2()));
			QuaternionT<T> q3(RotationT<T>(0, 0, 1, NumericT<T>::pi_2()));

			// 180 degrees test (a)
			OCEAN_EXPECT_EQUAL(validation, QuaternionT<T>(VectorT3<T>(1, 0, 0), VectorT3<T>(-1, 0, 0)) * VectorT3<T>(1, 0, 0), VectorT3<T>(-1, 0, 0));
			OCEAN_EXPECT_EQUAL(validation, QuaternionT<T>(VectorT3<T>(0, 1, 0), VectorT3<T>(0, -1, 0)) * VectorT3<T>(0, 1, 0), VectorT3<T>(0, -1, 0));
			OCEAN_EXPECT_EQUAL(validation, QuaternionT<T>(VectorT3<T>(0, 0, 1), VectorT3<T>(0, 0, -1)) * VectorT3<T>(0, 0, 1), VectorT3<T>(0, 0, -1));

			// 180 degrees test (b)
			OCEAN_EXPECT_EQUAL(validation, QuaternionT<T>(VectorT3<T>(-1, 0, 0), VectorT3<T>(1, 0, 0)) * VectorT3<T>(1, 0, 0), VectorT3<T>(-1, 0, 0));
			OCEAN_EXPECT_EQUAL(validation, QuaternionT<T>(VectorT3<T>(0, -1, 0), VectorT3<T>(0, 1, 0)) * VectorT3<T>(0, 1, 0), VectorT3<T>(0, -1, 0));
			OCEAN_EXPECT_EQUAL(validation, QuaternionT<T>(VectorT3<T>(0, 0, -1), VectorT3<T>(0, 0, 1)) * VectorT3<T>(0, 0, 1), VectorT3<T>(0, 0, -1));

			const QuaternionT<T> quaternion0(reference, offset);
			const VectorT3<T> test0 = quaternion0 * reference;

			OCEAN_EXPECT_TRUE(validation, offset.isEqual(test0, NumericT<T>::weakEps()) && offset.angle(test0) < NumericT<T>::deg2rad(T(0.1)));

			const QuaternionT<T> quaternion1(reference, -reference);
			const VectorT3<T> test1 = quaternion1 * reference;

			OCEAN_EXPECT_TRUE(validation, reference.isEqual(-test1, NumericT<T>::weakEps()) && reference.angle(test1) > NumericT<T>::deg2rad(T(179.9)));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestQuaternion::testAngle(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Angle for '" << TypeNamer::name<T>() << "':";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	// we check some fixed rotations

	OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(QuaternionT<T>().angle(), T(0)));
	OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(QuaternionT<T>(VectorT3<T>(1, 0, 0), NumericT<T>::pi_4()).angle(), NumericT<T>::pi_4()));
	OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(QuaternionT<T>(VectorT3<T>(0, 1, 0), NumericT<T>::pi_4()).angle(), NumericT<T>::pi_4()));
	OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(QuaternionT<T>(VectorT3<T>(0, 0, 1), NumericT<T>::pi_4()).angle(), NumericT<T>::pi_4()));
	OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(QuaternionT<T>(VectorT3<T>(1, 0, 0), NumericT<T>::pi()).angle(), NumericT<T>::pi()));
	OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(QuaternionT<T>(VectorT3<T>(0, 1, 0), NumericT<T>::pi()).angle(), NumericT<T>::pi()));
	OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(QuaternionT<T>(VectorT3<T>(0, 0, 1), NumericT<T>::pi()).angle(), NumericT<T>::pi()));

	const T epsilon = std::is_same<T, float>::value ? T(0.1) : T(0.01);

	const Timestamp startTimestamp(true);

	do
	{
		const VectorT3<T> axis = RandomT<T>::vector3();
		ocean_assert(NumericT<T>::isEqual(axis.length(), 1));

		const T angle = RandomT<T>::scalar(0, NumericT<T>::pi());

		// we create a quaternion based on axis/angle and check whether the calculated angle is correct

		const QuaternionT<T> quaternion(axis, angle);

		OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(quaternion.angle(), angle, NumericT<T>::deg2rad(epsilon)));

		// we rotated a vector (perpendicular to the rotation axis) and check whether the angle between vector and rotated vector is correct

		const VectorT3<T> vector = axis.perpendicular();
		ocean_assert(NumericT<T>::isWeakEqualEps(axis * vector));

		const VectorT3<T> rotatedVector = quaternion * vector;

		const T vectorAngle = vector.angle(rotatedVector);

		OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(vectorAngle, angle, NumericT<T>::deg2rad(epsilon)));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestQuaternion::testSlerp(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Slerp for '" << TypeNamer::name<T>() << "':";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

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
					OCEAN_EXPECT_TRUE(validation, slerpVector.isEqual(vectorA, epsilonSimilarity));
				}
				else if (nFactor == 100u)
				{
					OCEAN_EXPECT_TRUE(validation, slerpVector.isEqual(vectorB, epsilonSimilarity));
				}

				const T angleA = NumericT<T>::rad2deg(vectorA.angle(slerpVector));
				const T angleB = NumericT<T>::rad2deg(vectorB.angle(slerpVector));

				const T expectedAngleA = NumericT<T>::rad2deg(angleAB * factor);
				const T expectedAngleB = NumericT<T>::rad2deg(angleAB * (T(1) - factor));

				constexpr T epsilonAngle = std::is_same<T, float>::value ? T(0.1) : T(0.01);

				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(angleA, expectedAngleA, epsilonAngle));
				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(angleB, expectedAngleB, epsilonAngle));
			}

			{
				// backward interpolation

				const QuaternionT<T> slerpQuaternion = vectorB_Q_vectorA.slerp(QuaternionT<T>(true), factor);

				const VectorT3<T> slerpVector = slerpQuaternion * vectorA;

				constexpr T epsilonSimilarity = std::is_same<T, float>::value ? NumericT<T>::weakEps() : NumericT<T>::eps();

				if (nFactor == 0u)
				{
					OCEAN_EXPECT_TRUE(validation, slerpVector.isEqual(vectorB, epsilonSimilarity));
				}
				else if (nFactor == 100u)
				{
					OCEAN_EXPECT_TRUE(validation, slerpVector.isEqual(vectorA, epsilonSimilarity));
				}

				const T angleA = NumericT<T>::rad2deg(vectorA.angle(slerpVector));
				const T angleB = NumericT<T>::rad2deg(vectorB.angle(slerpVector));

				const T expectedAngleA = NumericT<T>::rad2deg(angleAB * (T(1) - factor));
				const T expectedAngleB = NumericT<T>::rad2deg(angleAB * factor);

				constexpr T epsilonAngle = std::is_same<T, float>::value ? T(0.1) : T(0.01);

				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(angleA, expectedAngleA, epsilonAngle));
				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(angleB, expectedAngleB, epsilonAngle));
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
