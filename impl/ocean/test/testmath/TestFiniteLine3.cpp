/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestFiniteLine3.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/FiniteLine3.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestFiniteLine3::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   FiniteLine3 test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testIsOnLine(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testDistance(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "FiniteLine3 test succeeded.";
	}
	else
	{
		Log::info() << "FiniteLine3 test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFiniteLine3, IsOnLine)
{
	EXPECT_TRUE(TestFiniteLine3::testIsOnLine(GTEST_TEST_DURATION));
}

TEST(TestFiniteLine3, Distance)
{
	EXPECT_TRUE(TestFiniteLine3::testDistance(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestFiniteLine3::testIsOnLine(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "isOnLine test:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			bool localSucceeded = true;

			const Vector3 point0(Random::vector3(-100, 100));
			Vector3 point1(Random::vector3(-100, 100));

			while (point0 == point1)
				point1 = Random::vector3(-100, 100);

			const FiniteLine3 line(point0, point1);

			{
				const Scalar scalar = Random::scalar(-1, 2);
				const Vector3 point(point0 + (point1 - point0) * scalar);

				if (scalar >= 0 && scalar <= 1)
				{
					if (line.isOnLine(point) == false)
						localSucceeded = false;
				}
				else if (scalar < -Numeric::weakEps() || scalar > 1 + Numeric::weakEps())
				{
					if (line.isOnLine(point) == true)
						localSucceeded = false;
				}
				else
				{
					// we not not consider this case as the result is not uniquely defined
				}
			}

			{
				const Vector3 direction0(line.direction());
				const Vector3 direction1(direction0.perpendicular().normalized());
				ocean_assert(Numeric::isEqualEps(direction0 * direction1));

				ocean_assert(Numeric::isEqual(direction0.length(), 1));
				ocean_assert(Numeric::isEqual(direction1.length(), 1));

				const Scalar scalar0 = Random::scalar(Scalar(0.01), 100);
				const Scalar scalar1 = Random::scalar(Scalar(0.01), 100);

				const Vector3 pointA = point0 + direction0 * scalar0 + direction1 * scalar1;
				const Vector3 pointB = point0 - direction0 * scalar0 - direction1 * scalar1;

				if (line.isOnLine(pointA) || line.isOnLine(pointB))
					localSucceeded = false;
			}

			if (localSucceeded)
				validIterations++;

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestFiniteLine3::testDistance(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Distance test:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Vector3 point0(Random::vector3(-100, 100));
			Vector3 point1(Random::vector3(-100, 100));

			while (point0 == point1)
				point1 = Random::vector3(-100, 100);

			const FiniteLine3 line(point0, point1);

			const Vector3 point(Random::vector2(-100, 100));

			const Scalar result = line.distance(point);

			const Line3 infiniteLine(line.point0(), line.direction());

			const Vector3 pointOnInfiniteLine(infiniteLine.nearestPoint(point));
			ocean_assert(infiniteLine.isOnLine(pointOnInfiniteLine));

			Scalar test = Numeric::maxValue();

			if (line.isOnLine(pointOnInfiniteLine))
				test = pointOnInfiniteLine.distance(point);
			else
				test = min(point.distance(point0), point.distance(point1));

			if (Numeric::isEqual(test, result, std::is_same<float, Scalar>::value ? Numeric::eps() * 100 : Numeric::eps()))
				validIterations++;

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

}

}

}
