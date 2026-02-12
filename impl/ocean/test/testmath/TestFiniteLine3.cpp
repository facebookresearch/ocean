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

#include "ocean/test/TestResult.h"
#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestFiniteLine3::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("FiniteLine3 test");

	Log::info() << " ";

	if (selector.shouldRun("isonline"))
	{
		testResult = testIsOnLine(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("distance"))
	{
		testResult = testDistance(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("intersection"))
	{
		testResult = testIntersection(testDuration);

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
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

TEST(TestFiniteLine3, Intersection)
{
	EXPECT_TRUE(TestFiniteLine3::testIntersection(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestFiniteLine3::testIsOnLine(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "isOnLine test:";

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.99, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Vector3 point0(Random::vector3(randomGenerator, -100, 100));
			Vector3 point1(Random::vector3(randomGenerator, -100, 100));

			while (point0 == point1)
			{
				point1 = Random::vector3(randomGenerator, -100, 100);
			}

			const FiniteLine3 line(point0, point1);

			{
				const Scalar scalar = Random::scalar(randomGenerator, -1, 2);
				const Vector3 point(point0 + (point1 - point0) * scalar);

				if (scalar >= 0 && scalar <= 1)
				{
					if (line.isOnLine(point) == false)
					{
						scopedIteration.setInaccurate();
					}
				}
				else if (scalar < -Numeric::weakEps() || scalar > 1 + Numeric::weakEps())
				{
					if (line.isOnLine(point) == true)
					{
						scopedIteration.setInaccurate();
					}
				}
				else
				{
					// we do not consider this case as the result is not uniquely defined
				}
			}

			{
				const Vector3 direction0(line.direction());
				const Vector3 direction1(direction0.perpendicular().normalized());
				ocean_assert(Numeric::isEqualEps(direction0 * direction1));

				ocean_assert(Numeric::isEqual(direction0.length(), 1));
				ocean_assert(Numeric::isEqual(direction1.length(), 1));

				const Scalar scalar0 = Random::scalar(randomGenerator, Scalar(0.01), 100);
				const Scalar scalar1 = Random::scalar(randomGenerator, Scalar(0.01), 100);

				const Vector3 pointA = point0 + direction0 * scalar0 + direction1 * scalar1;
				const Vector3 pointB = point0 - direction0 * scalar0 - direction1 * scalar1;

				if (line.isOnLine(pointA) || line.isOnLine(pointB))
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestFiniteLine3::testDistance(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Distance test:";

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.99, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Vector3 point0(Random::vector3(randomGenerator, -100, 100));
			Vector3 point1(Random::vector3(randomGenerator, -100, 100));

			while (point0 == point1)
			{
				point1 = Random::vector3(randomGenerator, -100, 100);
			}

			const FiniteLine3 line(point0, point1);

			const Vector3 point(Random::vector3(randomGenerator, -100, 100));

			const Scalar result = line.distance(point);

			const Line3 infiniteLine(line.point0(), line.direction());

			const Vector3 pointOnInfiniteLine(infiniteLine.nearestPoint(point));
			ocean_assert(infiniteLine.isOnLine(pointOnInfiniteLine));

			Scalar test = Numeric::maxValue();

			if (line.isOnLine(pointOnInfiniteLine))
			{
				test = pointOnInfiniteLine.distance(point);
			}
			else
			{
				test = min(point.distance(point0), point.distance(point1));
			}

			if (!Numeric::isEqual(test, result, std::is_same<float, Scalar>::value ? Numeric::eps() * 100 : Numeric::eps()))
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestFiniteLine3::testIntersection(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Intersection test:";

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.99, randomGenerator);

	const Scalar epsilon = std::is_same<float, Scalar>::value ? Scalar(0.01) : Numeric::weakEps();

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			{
				// two lines intersecting at a known interior point must report true and return that point

				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const Vector3 expectedPoint(Random::vector3(randomGenerator, -100, 100));

				Vector3 direction0(Random::vector3(randomGenerator, -1, 1));

				while (direction0.sqr() < Numeric::eps())
				{
					direction0 = Random::vector3(randomGenerator, -1, 1);
				}

				direction0.normalize();

				Vector3 direction1(Random::vector3(randomGenerator, -1, 1));

				while (direction1.sqr() < Numeric::eps() || direction0.cross(direction1).sqr() < Numeric::eps())
				{
					direction1 = Random::vector3(randomGenerator, -1, 1);
				}

				direction1.normalize();

				const Scalar halfLength0 = Random::scalar(randomGenerator, Scalar(0.1), 10);
				const Scalar halfLength1 = Random::scalar(randomGenerator, Scalar(0.1), 10);

				const FiniteLine3 lineA(expectedPoint - direction0 * halfLength0, expectedPoint + direction0 * halfLength0);
				const FiniteLine3 lineB(expectedPoint - direction1 * halfLength1, expectedPoint + direction1 * halfLength1);

				Vector3 resultPoint;

				if (!lineA.intersection(lineB, resultPoint) || expectedPoint.distance(resultPoint) > epsilon)
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				// two parallel lines must not report an intersection

				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const Vector3 basePoint(Random::vector3(randomGenerator, -100, 100));

				Vector3 direction(Random::vector3(randomGenerator, -1, 1));

				while (direction.sqr() < Numeric::eps())
				{
					direction = Random::vector3(randomGenerator, -1, 1);
				}

				direction.normalize();

				Vector3 offset(direction.perpendicular());
				ocean_assert(Numeric::isEqualEps(offset * direction));
				offset.normalize();
				offset *= Random::scalar(randomGenerator, Scalar(0.1), 10);

				const Scalar halfLength = Random::scalar(randomGenerator, Scalar(0.1), 10);

				const FiniteLine3 lineA(basePoint - direction * halfLength, basePoint + direction * halfLength);
				const FiniteLine3 lineB(basePoint + offset - direction * halfLength, basePoint + offset + direction * halfLength);

				Vector3 resultPoint;

				if (lineA.intersection(lineB, resultPoint))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				// two skew lines (non-parallel, non-intersecting in 3D) must not report an intersection

				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const Vector3 basePoint(Random::vector3(randomGenerator, -100, 100));

				Vector3 direction0(Random::vector3(randomGenerator, -1, 1));

				while (direction0.sqr() < Numeric::eps())
				{
					direction0 = Random::vector3(randomGenerator, -1, 1);
				}

				direction0.normalize();

				Vector3 direction1(Random::vector3(randomGenerator, -1, 1));

				while (direction1.sqr() < Numeric::eps() || direction0.cross(direction1).sqr() < Numeric::eps())
				{
					direction1 = Random::vector3(randomGenerator, -1, 1);
				}

				direction1.normalize();

				// offset perpendicular to both directions guarantees the lines are skew
				const Vector3 normal = direction0.cross(direction1).normalized();
				const Vector3 offset = normal * Random::scalar(randomGenerator, Scalar(1), 10);

				const Scalar halfLength0 = Random::scalar(randomGenerator, Scalar(0.1), 10);
				const Scalar halfLength1 = Random::scalar(randomGenerator, Scalar(0.1), 10);

				const FiniteLine3 lineA(basePoint - direction0 * halfLength0, basePoint + direction0 * halfLength0);
				const FiniteLine3 lineB(basePoint + offset - direction1 * halfLength1, basePoint + offset + direction1 * halfLength1);

				Vector3 resultPoint;

				if (lineA.intersection(lineB, resultPoint))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				// two coplanar lines whose infinite extensions intersect but whose finite segments do not reach the intersection point

				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const Vector3 convergencePoint(Random::vector3(randomGenerator, -100, 100));

				Vector3 direction0(Random::vector3(randomGenerator, -1, 1));

				while (direction0.sqr() < Numeric::eps())
				{
					direction0 = Random::vector3(randomGenerator, -1, 1);
				}

				direction0.normalize();

				Vector3 direction1(Random::vector3(randomGenerator, -1, 1));

				while (direction1.sqr() < Numeric::eps() || direction0.cross(direction1).sqr() < Numeric::eps())
				{
					direction1 = Random::vector3(randomGenerator, -1, 1);
				}

				direction1.normalize();

				// both segments start well past the convergence point so they cannot reach it
				const Scalar gap = Random::scalar(randomGenerator, Scalar(1), 10);
				const Scalar length0 = Random::scalar(randomGenerator, Scalar(0.1), 5);
				const Scalar length1 = Random::scalar(randomGenerator, Scalar(0.1), 5);

				const FiniteLine3 lineA(convergencePoint + direction0 * gap, convergencePoint + direction0 * (gap + length0));
				const FiniteLine3 lineB(convergencePoint + direction1 * gap, convergencePoint + direction1 * (gap + length1));

				Vector3 resultPoint;

				if (lineA.intersection(lineB, resultPoint))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				// two lines sharing a common endpoint must report an intersection at that endpoint

				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const Vector3 sharedEndpoint(Random::vector3(randomGenerator, -100, 100));

				Vector3 direction0(Random::vector3(randomGenerator, -1, 1));

				while (direction0.sqr() < Numeric::eps())
				{
					direction0 = Random::vector3(randomGenerator, -1, 1);
				}

				direction0.normalize();

				Vector3 direction1(Random::vector3(randomGenerator, -1, 1));

				while (direction1.sqr() < Numeric::eps() || direction0.cross(direction1).sqr() < Numeric::eps())
				{
					direction1 = Random::vector3(randomGenerator, -1, 1);
				}

				direction1.normalize();

				const Scalar length0 = Random::scalar(randomGenerator, Scalar(0.1), 10);
				const Scalar length1 = Random::scalar(randomGenerator, Scalar(0.1), 10);

				const FiniteLine3 lineA(sharedEndpoint, sharedEndpoint + direction0 * length0);
				const FiniteLine3 lineB(sharedEndpoint, sharedEndpoint + direction1 * length1);

				Vector3 resultPoint;

				if (!lineA.intersection(lineB, resultPoint) || sharedEndpoint.distance(resultPoint) > epsilon)
				{
					scopedIteration.setInaccurate();
				}
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
