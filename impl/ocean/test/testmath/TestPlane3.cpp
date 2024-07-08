/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestPlane3.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/Line3.h"
#include "ocean/math/Plane3.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestPlane3::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);
	bool allSucceeded = true;

	Log::info() << "---   Plane3 test:   ---";
	Log::info() << " ";

	allSucceeded = testConstructorThreePoints(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testIntersectionLine(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testIntersectionPlane(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Plane3 test succeeded.";
	}
	else
	{
		Log::info() << "Plane3 test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestPlane3, ConstructorThreePoints)
{
	EXPECT_TRUE(TestPlane3::testConstructorThreePoints(GTEST_TEST_DURATION));
}

TEST(TestPlane3, IntersectionLine)
{
	EXPECT_TRUE(TestPlane3::testIntersectionLine(GTEST_TEST_DURATION));
}

TEST(TestPlane3, IntersectionPlane)
{
	EXPECT_TRUE(TestPlane3::testIntersectionPlane(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestPlane3::testConstructorThreePoints(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Three point constructor test:";

	uint64_t iterations = 0ull;
	uint64_t succeeded = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			constexpr Scalar range = std::is_same<Scalar, float>::value ? Scalar(10) : Scalar(100);

			// all points identical
			const Vector3 objectPoint(Random::vector3(-range, range));

			const Plane3 plane(objectPoint, objectPoint, objectPoint);

			if (!plane.isValid())
			{
				++succeeded;
			}

			iterations++;
		}

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			constexpr Scalar range = std::is_same<Scalar, float>::value ? Scalar(10) : Scalar(100);

			// first and second point identical
			const Vector3 objectPoint0(Random::vector3(-range, range));
			const Vector3 objectPoint1(Random::vector3(-range, range));

			const Plane3 plane(objectPoint0, objectPoint0, objectPoint1);

			if (!plane.isValid())
			{
				++succeeded;
			}

			iterations++;
		}

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			constexpr Scalar range = std::is_same<Scalar, float>::value ? Scalar(10) : Scalar(100);

			// first and third point identical
			const Vector3 objectPoint0(Random::vector3(-range, range));
			const Vector3 objectPoint1(Random::vector3(-range, range));

			const Plane3 plane(objectPoint0, objectPoint1, objectPoint0);

			if (!plane.isValid())
			{
				++succeeded;
			}

			iterations++;
		}

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			constexpr Scalar range = std::is_same<Scalar, float>::value ? Scalar(10) : Scalar(100);

			// second and third point identical
			const Vector3 objectPoint0(Random::vector3(-range, range));
			const Vector3 objectPoint1(Random::vector3(-range, range));

			const Plane3 plane(objectPoint0, objectPoint1, objectPoint1);

			if (!plane.isValid())
			{
				++succeeded;
			}

			iterations++;
		}

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			// we explicitly use a smaller region [-1, 1] or [-10, 10] to ensure that we really receive collinear points
			constexpr Scalar range = std::is_same<Scalar, float>::value ? Scalar(1) : Scalar(10);
			constexpr Scalar equalEps = std::is_same<Scalar, float>::value ? Scalar(0.01) : Numeric::weakEps();

			// non-collinear test
			const Vector3 objectPoint0(Random::vector3(-range, range));

			Vector3 objectPoint1(Random::vector3(-range, range));

			while (objectPoint0.isEqual(objectPoint1, equalEps))
			{
				objectPoint1 = Random::vector3(-range, range);
			}

			const Line3 line(objectPoint0, (objectPoint1 - objectPoint0).normalized());

			const Vector3 objectPoint2(line.point(Random::scalar(-range, range)));

			const Plane3 plane(objectPoint0, objectPoint1, objectPoint2);

			if (!plane.isValid())
			{
				++succeeded;
			}

			iterations++;
		}

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			// we explicitly use a smaller region [-5, 5] or [-100, 100] to ensure that we really receive collinear points
			constexpr Scalar range = std::is_same<Scalar, float>::value ? Scalar(5) : Scalar(100);
			constexpr Scalar equalEps = std::is_same<Scalar, float>::value ? Scalar(0.01) : Numeric::weakEps();

			const Vector3 objectPoint0(Random::vector3(-range, range));
			const Vector3 objectPoint1(Random::vector3(-range, range));
			const Vector3 objectPoint2(Random::vector3(-range, range));

			const Plane3 plane(objectPoint0, objectPoint1, objectPoint2);

			if (!objectPoint0.isEqual(objectPoint1, equalEps) && !objectPoint0.isEqual(objectPoint2, equalEps) && !objectPoint1.isEqual(objectPoint2, equalEps))
			{
				if (Numeric::isNotEqual(Numeric::abs((objectPoint1 - objectPoint0).normalized() * (objectPoint2 - objectPoint0).normalized()), Scalar(1)))
				{
					if (plane.isInPlane(objectPoint0) && plane.isInPlane(objectPoint1) && plane.isInPlane(objectPoint2))
					{
						++succeeded;
					}

					++iterations;
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.985;
}

bool TestPlane3::testIntersectionLine(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Plane-line intersection test:";

	uint64_t iterations = 0ull;
	uint64_t succeeded = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Plane3 plane(Random::vector3(), Random::scalar(-100, 100));
			const Line3 line(Random::vector3(-100, 100), Random::vector3());

			Vector3 point;
			if (plane.intersection(line, point))
			{
				const Scalar d0 = line.distance(point);
				const Scalar d1 = plane.signedDistance(point);

				if (Numeric::isWeakEqualEps(d0) && Numeric::isWeakEqualEps(d1))
				{
					++succeeded;
				}
			}
			else
			{
				// check if the line is parallel to the plane

				if (Numeric::isEqualEps(plane.normal() * line.direction()))
				{
					++succeeded;
				}
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestPlane3::testIntersectionPlane(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Plane-plane intersection test:";

	uint64_t iterations = 0ull;
	uint64_t succeeded = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Plane3 planeA(Random::vector3(), Random::scalar(-10, 10));
			const Plane3 planeB(Random::vector3(), Random::scalar(-10, 10));

			Line3 line;
			if (planeA.intersection(planeB, line))
			{
				const Scalar distanceA0 = planeA.signedDistance(line.point(-1));
				const Scalar distanceA1 = planeA.signedDistance(line.point());
				const Scalar distanceA2 = planeA.signedDistance(line.point(1));

				const Scalar distanceB0 = planeB.signedDistance(line.point(-1));
				const Scalar distanceB1 = planeB.signedDistance(line.point());
				const Scalar distanceB2 = planeB.signedDistance(line.point(1));

				if (Numeric::isWeakEqualEps(distanceA0) && Numeric::isWeakEqualEps(distanceA1) && Numeric::isWeakEqualEps(distanceA2)
						&& Numeric::isWeakEqualEps(distanceB0) && Numeric::isWeakEqualEps(distanceB1) && Numeric::isWeakEqualEps(distanceB2))
				{
					++succeeded;
				}
			}
			else
			{
				// check if both planes are parallel

				if (planeA.normal().isParallel(planeB.normal()))
				{
					++succeeded;
				}
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

}

}

}
