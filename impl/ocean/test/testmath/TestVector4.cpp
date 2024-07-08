/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestVector4.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestVector4::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Vector4 test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testWriteToMessenger() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testIsUnit(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAngle(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testLessOperator(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testVectorConversion(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Vector4 test succeeded.";
	}
	else
	{
		Log::info() << "Vector4 test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestVector4, WriteToMessenger)
{
	EXPECT_TRUE(TestVector4::testWriteToMessenger());
}

TEST(TestVector4, IsUnit)
{
	EXPECT_TRUE(TestVector4::testIsUnit(GTEST_TEST_DURATION));
}

TEST(TestVector4, Angle)
{
	EXPECT_TRUE(TestVector4::testAngle(GTEST_TEST_DURATION));
}

TEST(TestVector4, LessOperator)
{
	EXPECT_TRUE(TestVector4::testLessOperator(GTEST_TEST_DURATION));
}

TEST(TestVector4, VectorConversion)
{
	EXPECT_TRUE(TestVector4::testVectorConversion(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestVector4::testWriteToMessenger()
{
	Log::info() << "Write to messenger test:";

	// this is mainly a check whether the code does not compile or crash

	Log::info() << " ";

	Log::info() << Vector4(0, 1, 2, 3);
	Log::info() << "Vector: " << Vector4(0, 1, 2, 3);
	Log::info() << Vector4(0, 1, 2, 3) << " <- Vector";

	Log::info() << " ";
	Log::info() << "Validation succeeded.";

	return true;
}

bool TestVector4::testIsUnit(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Vector4::isUnit() test:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Scalar valueRange = std::is_same<float, Scalar>::value ? 2 : 10;

	const Timestamp startTimestamp(true);
	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Vector4 vector(Random::scalar(-valueRange, valueRange), Random::scalar(-valueRange, valueRange), Random::scalar(-valueRange, valueRange), Random::scalar(-valueRange, valueRange));
			const Scalar length = vector.length();

			if (Numeric::isEqual(length, Scalar(1)))
			{
				if (vector.isUnit())
				{
					validIterations++;
				}
			}
			else
			{
				const Vector4 normalizedVector = vector.normalized();

				if (normalizedVector.isUnit() && !vector.isUnit())
				{
					validIterations++;
				}
			}

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "%";

	return percent >= 0.99;
}

bool TestVector4::testAngle(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Vector4::angle() test:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			Vector4 vectorA(Random::vector4(-10, 10));
			Vector4 vectorB(Random::vector4(-10, 10));

			if (!vectorA.isNull() && !vectorB.isNull())
			{
				// a * b == cos(alpha) * |a| * |b|

				const Scalar lengthA = Numeric::sqrt(vectorA[0] * vectorA[0] + vectorA[1] * vectorA[1] + vectorA[2] * vectorA[2] + vectorA[3] * vectorA[3]);
				const Scalar lengthB = Numeric::sqrt(vectorB[0] * vectorB[0] + vectorB[1] * vectorB[1] + vectorB[2] * vectorB[2] + vectorB[3] * vectorB[3]);

				const Scalar dotProduct = vectorA[0] * vectorB[0] + vectorA[1] * vectorB[1] + vectorA[2] * vectorB[2] + vectorA[3] * vectorB[3];

				const Scalar cosAlpha = (dotProduct / lengthA) / lengthB;
				const Scalar alpha = Numeric::acos(cosAlpha);

				const Scalar test = vectorA.angle(vectorB);

				const Scalar alphaDegree = Numeric::rad2deg(alpha);
				const Scalar testDegree = Numeric::rad2deg(test);

				if (Numeric::isEqual(alphaDegree, testDegree, Scalar(0.01)))
					validIterations++;

				iterations++;
			}
		}

		bool localSucceeded = true;

		if (Numeric::isNotEqual(Vector4(1, 0, 0, 0).angle(Vector4(1, 1, 0, 0)), Numeric::deg2rad(45)))
			localSucceeded = false;
		if (Numeric::isNotEqual(Vector4(5, 0, 0, 0).angle(Vector4(Scalar(7.4), Scalar(7.4), 0, 0)), Numeric::deg2rad(45)))
			localSucceeded = false;
		if (Numeric::isNotEqual(Vector4(0, -5, 0, 0).angle(Vector4(Scalar(-7.4), Scalar(-7.4), 0, 0)), Numeric::deg2rad(45)))
			localSucceeded = false;
		if (Numeric::isNotEqual(Vector4(5, 2, 0, 0).angle(Vector4(-10, -4, 0, 0)), Numeric::deg2rad(180)))
			localSucceeded = false;
		if (Numeric::isNotEqual(Vector4(Scalar(4.2), Scalar(4.2), 0, 0).angle(Vector4(Scalar(-7.44), Scalar(7.44), 0, 0)), Numeric::deg2rad(90)))
			localSucceeded = false;

		if (localSucceeded)
			validIterations++;

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestVector4::testLessOperator(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Vector4::operator < () test:";

	bool succeeded = true;
	unsigned int dummyValue = 0u;

	const Timestamp startTimestamp(true);
	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Vector4 first = Vector4(Random::vector4(-100, 100));
			const Vector4 second = Vector4(Random::vector4(-100, 100));

			const bool less = first < second;

			bool testLess = false;

			if (first[0] < second[0])
				testLess = true;
			if (first[0] == second[0] && first[1] < second[1])
				testLess = true;
			if (first[0] == second[0] && first[1] == second[1] && first[2] < second[2])
				testLess = true;
			if (first[0] == second[0] && first[1] == second[1] && first[2] == second[2] && first[3] < second[3])
				testLess = true;

			if (less != testLess)
				succeeded = false;

			if (less)
				dummyValue += 1u;
			else
				dummyValue += 2u;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (dummyValue % 2u == 0u)
	{
		if (succeeded)
			Log::info() << "Validation: succeeded.";
		else
			Log::info() << "Validation: FAILED!";
	}
	else
	{
		if (succeeded)
			Log::info() << "Validation: succeeded.";
		else
			Log::info() << "Validation: FAILED!";
	}

	return succeeded;
}

bool TestVector4::testVectorConversion(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Vector4::vectors2vectors() test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int size = RandomI::random(1000u);

		std::vector<VectorD4> vectorsD;
		std::vector<VectorF4> vectorsF;

		for (size_t n = 0; n < size; ++n)
		{
			vectorsD.push_back(RandomD::vector4(-10, 10));
			vectorsF.push_back(RandomF::vector4(-10, 10));
		}

		const std::vector<VectorD4> convertedD2D_0(VectorD4::vectors2vectors(vectorsD));
		const std::vector<VectorD4> convertedD2D_1(VectorD4::vectors2vectors(vectorsD.data(), vectorsD.size()));

		const std::vector<VectorF4> convertedD2F_0(VectorF4::vectors2vectors(vectorsD));
		const std::vector<VectorF4> convertedD2F_1(VectorF4::vectors2vectors(vectorsD.data(), vectorsD.size()));

		const std::vector<VectorD4> convertedF2D_0(VectorD4::vectors2vectors(vectorsF));
		const std::vector<VectorD4> convertedF2D_1(VectorD4::vectors2vectors(vectorsF.data(), vectorsF.size()));

		const std::vector<VectorF4> convertedF2F_0(VectorF4::vectors2vectors(vectorsF));
		const std::vector<VectorF4> convertedF2F_1(VectorF4::vectors2vectors(vectorsF.data(), vectorsF.size()));

		for (size_t n = 0; n < size; ++n)
		{
			for (unsigned int i = 0u; i < 4u; ++i)
			{
				if (NumericD::isNotWeakEqual(vectorsD[n][i], convertedD2D_0[n][i]))
					allSucceeded = false;

				if (NumericD::isNotWeakEqual(vectorsD[n][i], convertedD2D_1[n][i]))
					allSucceeded = false;

				if (NumericD::isNotWeakEqual(vectorsD[n][i], double(convertedD2F_0[n][i])))
					allSucceeded = false;

				if (NumericD::isNotWeakEqual(vectorsD[n][i], double(convertedD2F_1[n][i])))
					allSucceeded = false;


				if (NumericF::isNotWeakEqual(vectorsF[n][i], convertedF2F_0[n][i]))
					allSucceeded = false;

				if (NumericF::isNotWeakEqual(vectorsF[n][i], convertedF2F_1[n][i]))
					allSucceeded = false;

				if (NumericF::isNotWeakEqual(vectorsF[n][i], float(convertedF2D_0[n][i])))
					allSucceeded = false;

				if (NumericF::isNotWeakEqual(vectorsF[n][i], float(convertedF2D_1[n][i])))
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

}

}

}
