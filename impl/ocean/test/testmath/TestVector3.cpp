/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestVector3.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/Random.h"
#include "ocean/math/Vector2.h"

using namespace Ocean;

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestVector3::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Vector3 test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testWriteToMessenger() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testIsParallel(testDuration) && allSucceeded;

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
		Log::info() << "Vector3 test succeeded.";
	}
	else
	{
		Log::info() << "Vector3 test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestVector3, WriteToMessenger)
{
	EXPECT_TRUE(TestVector3::testWriteToMessenger());
}

TEST(TestVector3, IsParallel)
{
	EXPECT_TRUE(TestVector3::testIsParallel(GTEST_TEST_DURATION));
}

TEST(TestVector3, IsUnit)
{
	EXPECT_TRUE(TestVector3::testIsUnit(GTEST_TEST_DURATION));
}

TEST(TestVector3, Angle)
{
	EXPECT_TRUE(TestVector3::testAngle(GTEST_TEST_DURATION));
}

TEST(TestVector3, Perpendicular)
{
	EXPECT_TRUE(TestVector3::testPerpendicular(GTEST_TEST_DURATION));
}

TEST(TestVector3, LessOperator)
{
	EXPECT_TRUE(TestVector3::testLessOperator(GTEST_TEST_DURATION));
}

TEST(TestVector3, VectorConversion)
{
	EXPECT_TRUE(TestVector3::testVectorConversion(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestVector3::testWriteToMessenger()
{
	Log::info() << "Write to messenger test:";

	// this is mainly a check whether the code does not compile or crash

	Log::info() << " ";

	Log::info() << Vector3(0, 1, 2);
	Log::info() << "Vector: " << Vector3(0, 1, 2);
	Log::info() << Vector3(0, 1, 2) << " <- Vector";

	Log::info() << " ";
	Log::info() << "Validation succeeded.";

	return true;
}

bool TestVector3::testIsParallel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Vector3::isParallel() test:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Scalar valueRange = std::is_same<float, Scalar>::value ? 1 : 10;

	const Timestamp startTimestamp(true);
	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Vector3 vector(Random::vector3(-valueRange, valueRange));

			const Vector3 parallelVector(vector * Random::scalar(-valueRange, valueRange));

			Scalar offsetX = Random::scalar(-valueRange, valueRange);
			Scalar offsetY = Random::scalar(-valueRange, valueRange);
			Scalar offsetZ = Random::scalar(-valueRange, valueRange);

			while (Numeric::isWeakEqualEps(offsetX) && Numeric::isWeakEqualEps(offsetY) && Numeric::isWeakEqualEps(offsetZ))
			{
				offsetX = Random::scalar(-valueRange, valueRange);
				offsetY = Random::scalar(-valueRange, valueRange);
				offsetZ = Random::scalar(-valueRange, valueRange);
			}

			const Vector3 notParallelVector(vector + Vector3(offsetX, offsetY, offsetZ) * Random::scalar(-valueRange, valueRange));

			bool localSucceeded = true;

			if (vector.isNull())
			{
				if (vector.isParallel(parallelVector))
				{
					localSucceeded = false;
				}

				if (vector.isParallel(notParallelVector))
				{
					localSucceeded = false;
				}
			}
			else
			{
				if (parallelVector.isNull())
				{
					if (vector.isParallel(parallelVector))
					{
						localSucceeded = false;
					}
				}
				else
				{
					if (!vector.isParallel(parallelVector))
					{
						localSucceeded = false;
					}
				}

				if (notParallelVector.isNull())
				{
					if (vector.isParallel(notParallelVector))
					{
						localSucceeded = false;
					}
				}
				else
				{
					if (vector.isParallel(notParallelVector))
					{
						localSucceeded = false;
					}
				}
			}

			ocean_assert(vector.isParallel(Vector3(0, 0, 0)) == false);
			if (vector.isParallel(Vector3(0, 0, 0)))
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

	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "%";

	return percent >= 0.99;
}

bool TestVector3::testIsUnit(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Vector3::isUnit() test:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Scalar valueRange = std::is_same<float, Scalar>::value ? 2 : 10;

	const Timestamp startTimestamp(true);
	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Vector3 vector(Random::vector3(-valueRange, valueRange));
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
				const Vector3 normalizedVector = vector.normalized();

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

bool TestVector3::testAngle(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Vector3::angle() test:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Vector3 vectorA(Random::vector3(-10, 10));
			const Vector3 vectorB(Random::vector3(-10, 10));

			if (!vectorA.isNull() && !vectorB.isNull())
			{
				// a * b == cos(alpha) * |a| * |b|

				const Scalar lengthA = Numeric::sqrt(vectorA[0] * vectorA[0] + vectorA[1] * vectorA[1] + vectorA[2] * vectorA[2]);
				const Scalar lengthB = Numeric::sqrt(vectorB[0] * vectorB[0] + vectorB[1] * vectorB[1] + vectorB[2] * vectorB[2]);

				const Scalar dotProduct = vectorA[0] * vectorB[0] + vectorA[1] * vectorB[1] + vectorA[2] * vectorB[2];

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

		if (Numeric::isNotEqual(Vector3(1, 0, 0).angle(Vector3(1, 1, 0)), Numeric::deg2rad(45)))
		{
			localSucceeded = false;
		}
		if (Numeric::isNotEqual(Vector3(5, 0, 0).angle(Vector3(Scalar(7.4), Scalar(7.4), 0)), Numeric::deg2rad(45)))
		{
			localSucceeded = false;
		}
		if (Numeric::isNotEqual(Vector3(0, -5, 0).angle(Vector3(Scalar(-7.4), Scalar(-7.4), 0)), Numeric::deg2rad(45)))
		{
			localSucceeded = false;
		}
		if (Numeric::isNotEqual(Vector3(5, 2, 0).angle(Vector3(-10, -4, 0)), Numeric::deg2rad(180)))
		{
			localSucceeded = false;
		}
		if (Numeric::isNotEqual(Vector3(Scalar(4.2), Scalar(4.2), 0).angle(Vector3(Scalar(-7.44), Scalar(7.44), 0)), Numeric::deg2rad(90)))
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

bool TestVector3::testPerpendicular(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Perpendicular test:";

	RandomGenerator randomGenerator;

	bool succeeded = true;

	// we start to test three coordinate axis
	const Vector3 xAxis(1, 0, 0);
	const Vector3 yAxis(0, 1, 0);
	const Vector3 zAxis(0, 0, 1);

	const Vector3 p0 = xAxis.perpendicular();
	const Vector3 p1 = yAxis.perpendicular();
	const Vector3 p2 = zAxis.perpendicular();

	if (p0.isNull() || !p0.isOrthogonal(xAxis) || !p0.isUnit())
	{
		succeeded = false;
	}
	if (p1.isNull() || !p1.isOrthogonal(yAxis) || !p1.isUnit())
	{
		succeeded = false;
	}
	if (p2.isNull() || !p2.isOrthogonal(zAxis) || !p2.isUnit())
	{
		succeeded = false;
	}

	// now we test random vectors
	const Timestamp startTimestamp(true);
	do
	{
		const Vector3 vector(Random::vector3(randomGenerator));
		const Vector3 perpendicular(vector.perpendicular());

		ocean_assert(!perpendicular.isNull());

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

bool TestVector3::testLessOperator(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Vector3::operator < () test:";

	bool succeeded = true;
	unsigned int dummyValue = 0u;

	const Timestamp startTimestamp(true);
	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Vector3 first = Vector3(Random::vector3(-100, 100));
			const Vector3 second = Vector3(Random::vector3(-100, 100));

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
			if (first[0] == second[0] && first[1] == second[1] && first[2] < second[2])
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

bool TestVector3::testVectorConversion(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Vector3::vectors2vectors() test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int size = RandomI::random(1000u);

		std::vector<VectorD3> vectorsD;
		std::vector<VectorF3> vectorsF;

		for (size_t n = 0; n < size; ++n)
		{
			vectorsD.push_back(RandomD::vector3(-10, 10));
			vectorsF.push_back(RandomF::vector3(-10, 10));
		}

		const std::vector<VectorD3> convertedD2D_0(VectorD3::vectors2vectors(vectorsD));
		const std::vector<VectorD3> convertedD2D_1(VectorD3::vectors2vectors(vectorsD.data(), vectorsD.size()));

		const std::vector<VectorF3> convertedD2F_0(VectorF3::vectors2vectors(vectorsD));
		const std::vector<VectorF3> convertedD2F_1(VectorF3::vectors2vectors(vectorsD.data(), vectorsD.size()));

		const std::vector<VectorD3> convertedF2D_0(VectorD3::vectors2vectors(vectorsF));
		const std::vector<VectorD3> convertedF2D_1(VectorD3::vectors2vectors(vectorsF.data(), vectorsF.size()));

		const std::vector<VectorF3> convertedF2F_0(VectorF3::vectors2vectors(vectorsF));
		const std::vector<VectorF3> convertedF2F_1(VectorF3::vectors2vectors(vectorsF.data(), vectorsF.size()));

		for (size_t n = 0; n < size; ++n)
		{
			for (unsigned int i = 0u; i < 3u; ++i)
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
