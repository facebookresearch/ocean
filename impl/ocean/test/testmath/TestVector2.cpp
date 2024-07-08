/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestVector2.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/Random.h"
#include "ocean/math/Vector2.h"

#include <vector>

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestVector2::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Vector2 test:   ---";
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

	allSucceeded = testPerpendicular(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testLessOperator(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testVectorConversion(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Vector2 test succeeded.";
	}
	else
	{
		Log::info() << "Vector2 test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestVector2, WriteToMessenger)
{
	EXPECT_TRUE(TestVector2::testWriteToMessenger());
}

TEST(TestVector2, IsUnit)
{
	EXPECT_TRUE(TestVector2::testIsUnit(GTEST_TEST_DURATION));
}

TEST(TestVector2, Angle)
{
	EXPECT_TRUE(TestVector2::testAngle(GTEST_TEST_DURATION));
}

TEST(TestVector2, Perpendicular)
{
	EXPECT_TRUE(TestVector2::testPerpendicular(GTEST_TEST_DURATION));
}

TEST(TestVector2, LessOperator)
{
	EXPECT_TRUE(TestVector2::testLessOperator(GTEST_TEST_DURATION));
}

TEST(TestVector2, VectorConversion)
{
	EXPECT_TRUE(TestVector2::testVectorConversion(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestVector2::testWriteToMessenger()
{
	Log::info() << "Write to messenger test:";

	// this is mainly a check whether the code does not compile or crash

	Log::info() << " ";

	Log::info() << Vector2(0, 1);
	Log::info() << "Vector: " << Vector2(0, 1);
	Log::info() << Vector2(0, 1) << " <- Vector";

	Log::info() << " ";
	Log::info() << "Validation succeeded.";

	return true;
}

bool TestVector2::testIsUnit(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Vector2::isUnit() test:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Scalar valueRange = std::is_same<float, Scalar>::value ? 2 : 10;

	const Timestamp startTimestamp(true);
	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Vector2 vector(Random::vector2(-valueRange, valueRange));
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
				const Vector2 normalizedVector = vector.normalized();

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

bool TestVector2::testAngle(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Vector2::angle() test:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			Vector2 vectorA(Random::vector2(-10, 10));
			Vector2 vectorB(Random::vector2(-10, 10));

			if (!vectorA.isNull() && !vectorB.isNull())
			{
				// a * b == cos(alpha) * |a| * |b|

				const Scalar lengthA = Numeric::sqrt(vectorA[0] * vectorA[0] + vectorA[1] * vectorA[1]);
				const Scalar lengthB = Numeric::sqrt(vectorB[0] * vectorB[0] + vectorB[1] * vectorB[1]);

				const Scalar dotProduct = vectorA[0] * vectorB[0] + vectorA[1] * vectorB[1];

				const Scalar cosAlpha = (dotProduct / lengthA) / lengthB;
				const Scalar alpha = Numeric::acos(cosAlpha);

				const Scalar test = vectorA.angle(vectorB);

				const Scalar alphaDegree = Numeric::rad2deg(alpha);
				const Scalar testDegree = Numeric::rad2deg(test);

				if (Numeric::isEqual(alphaDegree, testDegree, Scalar(0.01)))
				{
					validIterations++;
				}

				iterations++;
			}
		}

		bool localSucceeded = true;

		if (Numeric::isNotEqual(Vector2(1, 0).angle(Vector2(1, 1)), Numeric::deg2rad(45)))
		{
			localSucceeded = false;
		}
		if (Numeric::isNotEqual(Vector2(5, 0).angle(Vector2(Scalar(7.4), Scalar(7.4))), Numeric::deg2rad(45)))
		{
			localSucceeded = false;
		}
		if (Numeric::isNotEqual(Vector2(0, -5).angle(Vector2(Scalar(-7.4), Scalar(-7.4))), Numeric::deg2rad(45)))
		{
			localSucceeded = false;
		}
		if (Numeric::isNotEqual(Vector2(5, 2).angle(Vector2(-10, -4)), Numeric::deg2rad(180)))
		{
			localSucceeded = false;
		}
		if (Numeric::isNotEqual(Vector2(Scalar(4.2), Scalar(4.2)).angle(Vector2(Scalar(-7.44), Scalar(7.44))), Numeric::deg2rad(90)))
		{
			localSucceeded = false;
		}

		if (localSucceeded)
		{
			validIterations++;
		}

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestVector2::testPerpendicular(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Perpendicular test:";

	RandomGenerator randomGenerator;

	bool succeeded = true;

	// we start to test two coordinate axes
	const Vector2 xAxis(1, 0);
	const Vector2 yAxis(0, 1);

	const Vector2 p0 = xAxis.perpendicular();
	const Vector2 p1 = yAxis.perpendicular();

	if (p0.isNull() || !p0.isOrthogonal(xAxis) || !p0.isUnit())
	{
		succeeded = false;
	}

	if (p1.isNull() || !p1.isOrthogonal(yAxis) || !p1.isUnit())
	{
		succeeded = false;
	}

	// now we test random vectors
	const Timestamp startTimestamp(true);
	do
	{
		const Vector2 vector(Random::vector2(randomGenerator));
		const Vector2 perpendicular(vector.perpendicular());

		ocean_assert(!perpendicular.isNull());

		if (Numeric::isNotEqualEps(vector.length() - perpendicular.length()))
		{
			succeeded = false;
		}

		if (Numeric::isNotEqualEps(vector * perpendicular))
		{
			succeeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (succeeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return succeeded;
}

bool TestVector2::testLessOperator(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Vector2::operator < () test:";

	bool succeeded = true;
	unsigned int dummyValue = 0u;

	const Timestamp startTimestamp(true);
	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Vector2 first = Vector2(Random::vector2(-100, 100));
			const Vector2 second = Vector2(Random::vector2(-100, 100));

			const bool less = first < second;

			bool testLess = false;

			if (first[0] < second[0])
			{
				testLess = true;
			}
			if (first[0] == second[0] && first[1] < second[1])
			{
				testLess = true;
			}

			if (less != testLess)
			{
				succeeded = false;
			}

			if (less)
			{
				dummyValue += 1u;
			}
			else
			{
				dummyValue += 2u;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (dummyValue % 2u == 0u)
	{
		if (succeeded)
		{
			Log::info() << "Validation: succeeded.";
		}
		else
		{
			Log::info() << "Validation: FAILED!";
		}
	}
	else
	{
		if (succeeded)
		{
			Log::info() << "Validation: succeeded.";
		}
		else
		{
			Log::info() << "Validation: FAILED!";
		}
	}

	return succeeded;
}

bool TestVector2::testVectorConversion(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Vector2::vectors2vectors() test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int size = RandomI::random(1000u);

		std::vector<VectorD2> vectorsD;
		std::vector<VectorF2> vectorsF;

		for (size_t n = 0; n < size; ++n)
		{
			vectorsD.push_back(RandomD::vector2(-10, 10));
			vectorsF.push_back(RandomF::vector2(-10, 10));
		}

		const std::vector<VectorD2> convertedD2D_0(VectorD2::vectors2vectors(vectorsD));
		const std::vector<VectorD2> convertedD2D_1(VectorD2::vectors2vectors(vectorsD.data(), vectorsD.size()));

		const std::vector<VectorF2> convertedD2F_0(VectorF2::vectors2vectors(vectorsD));
		const std::vector<VectorF2> convertedD2F_1(VectorF2::vectors2vectors(vectorsD.data(), vectorsD.size()));

		const std::vector<VectorD2> convertedF2D_0(VectorD2::vectors2vectors(vectorsF));
		const std::vector<VectorD2> convertedF2D_1(VectorD2::vectors2vectors(vectorsF.data(), vectorsF.size()));

		const std::vector<VectorF2> convertedF2F_0(VectorF2::vectors2vectors(vectorsF));
		const std::vector<VectorF2> convertedF2F_1(VectorF2::vectors2vectors(vectorsF.data(), vectorsF.size()));

		for (size_t n = 0; n < size; ++n)
		{
			for (unsigned int i = 0u; i < 2u; ++i)
			{
				if (NumericD::isNotWeakEqual(vectorsD[n][i], convertedD2D_0[n][i]))
				{
					allSucceeded = false;
				}

				if (NumericD::isNotWeakEqual(vectorsD[n][i], convertedD2D_1[n][i]))
				{
					allSucceeded = false;
				}

				if (NumericD::isNotWeakEqual(vectorsD[n][i], double(convertedD2F_0[n][i])))
				{
					allSucceeded = false;
				}

				if (NumericD::isNotWeakEqual(vectorsD[n][i], double(convertedD2F_1[n][i])))
				{
					allSucceeded = false;
				}


				if (NumericF::isNotWeakEqual(vectorsF[n][i], convertedF2F_0[n][i]))
				{
					allSucceeded = false;
				}

				if (NumericF::isNotWeakEqual(vectorsF[n][i], convertedF2F_1[n][i]))
				{
					allSucceeded = false;
				}

				if (NumericF::isNotWeakEqual(vectorsF[n][i], float(convertedF2D_0[n][i])))
				{
					allSucceeded = false;
				}

				if (NumericF::isNotWeakEqual(vectorsF[n][i], float(convertedF2D_1[n][i])))
				{
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
