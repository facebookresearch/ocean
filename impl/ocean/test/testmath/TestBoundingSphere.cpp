// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testmath/TestBoundingSphere.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/BoundingSphere.h"
#include "ocean/math/Box3.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestBoundingSphere::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Bounding sphere test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testConstructor(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testIntersections(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testIntersectionsTransformed(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPositiveFrontIntersection(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPositiveBackIntersection(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
		Log::info() << "Bounding sphere test succeeded.";
	else
		Log::info() << "Bounding sphere test FAILED!";

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestBoundingSphere, Constructor) {
	EXPECT_TRUE(TestBoundingSphere::testConstructor(GTEST_TEST_DURATION));
}

TEST(TestBoundingSphere, Intersections) {
	EXPECT_TRUE(TestBoundingSphere::testIntersections(GTEST_TEST_DURATION));
}

TEST(TestBoundingSphere, IntersectionsTransformed) {
	EXPECT_TRUE(TestBoundingSphere::testIntersectionsTransformed(GTEST_TEST_DURATION));
}

TEST(TestBoundingSphere, PositiveFrontIntersection) {
	EXPECT_TRUE(TestBoundingSphere::testPositiveFrontIntersection(GTEST_TEST_DURATION));
}

TEST(TestBoundingSphere, PositiveBackIntersection) {
	EXPECT_TRUE(TestBoundingSphere::testPositiveBackIntersection(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestBoundingSphere::testConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Costructor test:";

	const Scalar epsilon = std::is_same<float, Scalar>::value ? Scalar(0.001) : Numeric::eps();

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Vector3 lower(Random::vector3(-1000, 1000));
			const Vector3 higher(lower + Random::vector3(Scalar(0.05), 1000));
			const Vector3 center((lower + higher) * Scalar(0.5));

			ocean_assert(Numeric::isEqual(center.distance(lower), center.distance(higher), epsilon));

			const Box3 boundingBox(lower, higher);
			const BoundingSphere sphere(boundingBox);

			bool localSucceeded = true;

			if (sphere.center() != center)
				localSucceeded = false;

			if (Numeric::isNotEqual(sphere.radius(), center.distance(lower), epsilon))
				localSucceeded = false;

			if (Numeric::isNotEqual(sphere.radius(), center.distance(higher), epsilon))
				localSucceeded = false;

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

bool TestBoundingSphere::testIntersections(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Intersections test:";

	const Scalar epsilon = std::is_same<float, Scalar>::value ? Scalar(0.001) : Numeric::eps();

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			const Vector3 center(Random::vector3(-100, 100));
			const Scalar radius = Random::scalar(Scalar(0.05), 100);

			const BoundingSphere sphere(center, radius);

			const Vector3 rayDirection(Random::vector3());
			const Vector3 offsetDirection(Random::vector3());
			const Vector3 rayPosition((center + offsetDirection * (radius * Scalar(0.5))) + rayDirection * Random::scalar(radius * -2, radius * 2));

			const Line3 intersectingRay(rayPosition, rayDirection);

			Vector3 position0, position1;
			Scalar distance0, distance1;
			Vector3 normal0, normal1;

			bool localSucceeded = true;

			if (sphere.intersections(intersectingRay, position0, distance0, normal0, position1, distance1, normal1))
			{
				ocean_assert(intersectingRay.isOnLine(position0));
				ocean_assert(intersectingRay.isOnLine(position1));

				if (Numeric::isNotEqual(center.distance(position0), radius, epsilon))
					localSucceeded = false;
				if (Numeric::isNotEqual(center.distance(position1), radius, epsilon))
					localSucceeded = false;

				const Vector3 testPosition0 = intersectingRay.point(distance0);
				if (!testPosition0.isEqual(position0, epsilon))
					localSucceeded = false;

				const Vector3 testPosition1 = intersectingRay.point(distance1);
				if (!testPosition1.isEqual(position1, epsilon))
					localSucceeded = false;

				const Vector3 testNormal0 = (position0 - center).normalizedOrZero();
				if (!testNormal0.isEqual(normal0, epsilon))
					localSucceeded = false;

				const Vector3 testNormal1 = (position1 - center).normalizedOrZero();
				if (!testNormal1.isEqual(normal1, epsilon))
					localSucceeded = false;
			}
			else
				localSucceeded = false;

			const Line3 arbitraryRay(Random::vector3(-100, 100), Random::vector3());
			if (sphere.intersections(arbitraryRay, position0, distance0, normal0, position1, distance1, normal1))
			{
				ocean_assert(arbitraryRay.isOnLine(position0));
				ocean_assert(arbitraryRay.isOnLine(position1));

				ocean_assert(Numeric::isWeakEqual(center.distance(position0), radius));
				ocean_assert(Numeric::isWeakEqual(center.distance(position1), radius));
			}
			else
			{
				const Vector3 nearestPoint(arbitraryRay.nearestPoint(center));

				if (center.distance(nearestPoint) <= radius + Numeric::eps())
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

bool TestBoundingSphere::testIntersectionsTransformed(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Intersections test (transformed sphere):";

	const Scalar epsilon = std::is_same<float, Scalar>::value ? Scalar(0.001) : Numeric::eps();

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			bool localSucceeded = true;

			const Vector3 center(Random::vector3(-100, 100));
			const Scalar radius = Random::scalar(Scalar(0.01), 100);
			const BoundingSphere sphere(center, radius);

			// transformation transforming sphere-points to world-points
			const HomogenousMatrix4 wTs(Random::vector3(-100, 100), Random::rotation(), Random::vector3(Scalar(0.05), 10));
			const HomogenousMatrix4 sTw(wTs.inverted());

			const Line3 wRay(Random::vector3(-100, 100), Random::vector3());

			const Vector3 sRayPoint(sTw * wRay.point());
			const Vector3 sRayDirection((sTw.rotationMatrix() * wRay.direction()).normalizedOrZero());
			ocean_assert(Numeric::isEqual(sRayDirection.length(), 1));

			const Line3 sRay(sRayPoint, sRayDirection);

			Vector3 position0, position1;
			Scalar distance0, distance1;
			Vector3 normal0, normal1;
			if (sphere.intersections(wRay, wTs, sTw, position0, distance0, normal0, position1, distance1, normal1))
			{
				ocean_assert(wRay.isOnLine(position0));
				ocean_assert(wRay.isOnLine(position1));

				const Vector3 sPosition0 = sTw * position0;
				const Vector3 sPosition1 = sTw * position1;

				if (Numeric::isNotEqual(center.distance(sPosition0), radius, epsilon))
					localSucceeded = false;
				if (Numeric::isNotEqual(center.distance(sPosition1), radius, epsilon))
					localSucceeded = false;

				const Vector3 testPosition0 = wRay.point(distance0);
				if (!testPosition0.isEqual(position0, epsilon))
					localSucceeded = false;

				const Vector3 testPosition1 = wRay.point(distance1);
				if (!testPosition1.isEqual(position1, epsilon))
					localSucceeded = false;

				const Vector3 sTestNormal0 = (sPosition0 - center).normalizedOrZero();
				const Vector3 wTestNormal0 = (sTw.rotationMatrix().transposed() * sTestNormal0).normalizedOrZero();
				if (wTestNormal0 != normal0)
					localSucceeded = false;

				const Vector3 sTestNormal1 = (sPosition1 - center).normalizedOrZero();
				const Vector3 wTestNormal1 = (sTw.rotationMatrix().transposed() * sTestNormal1).normalizedOrZero();
				if (wTestNormal1 != normal1)
					localSucceeded = false;
			}
			else
			{
				if (sphere.intersections(sRay, position0, distance0, normal0, position1, distance1, normal1))
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

bool TestBoundingSphere::testPositiveFrontIntersection(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Positive front intersection test:";

	const Scalar epsilon = std::is_same<float, Scalar>::value ? Scalar(0.001) : Numeric::eps();

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		// front test
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			const Vector3 center(Random::vector3(-100, 100));
			const Scalar radius = Random::scalar(Scalar(0.01), 100);

			const BoundingSphere sphere(center, radius);

			const Vector3 rayDirection(Random::vector3());
			const Vector3 offsetDirection(Random::vector3());
			const Vector3 rayPosition((center + offsetDirection * (radius * Scalar(0.5))) + rayDirection * Random::scalar(radius * -10, radius * -2));

			ocean_assert(!sphere.isInside(rayPosition));

			const Line3 intersectingRay(rayPosition, rayDirection);

			Vector3 position;
			Scalar distance;

			bool localSucceeded = true;

			if (sphere.positiveFrontIntersection(intersectingRay, position, distance))
			{
				ocean_assert(intersectingRay.isOnLine(position));

				if (Numeric::isNotWeakEqual(center.distance(position), radius))
				{
					localSucceeded = false;
				}

				const Vector3 testPosition = intersectingRay.point(distance);
				if (!testPosition.isEqual(position, epsilon))
				{
					localSucceeded = false;
				}

				if (intersectingRay.point().distance(center) < intersectingRay.point().distance(position))
				{
					localSucceeded = false;
				}

				Vector3 position2(0, 0, 0);
				Scalar distance2;
				Vector3 normal(0, 0, 0);
				const bool localResult = sphere.positiveFrontIntersection(intersectingRay, position2, distance2, normal);

				ocean_assert_and_suppress_unused(localResult, localResult);
				ocean_assert(position == position2);
				ocean_assert(distance == distance2);

				const Vector3 testNormal = (position - center).normalizedOrZero();
				if (!testNormal.isEqual(normal, epsilon))
				{
					localSucceeded = false;
				}

				if (normal * intersectingRay.direction() > 0)
				{
					localSucceeded = false;
				}
			}
			else
			{
				localSucceeded = false;
			}

			if (localSucceeded)
			{
				validIterations++;
			}

			iterations++;
		}

		// center test
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			const Vector3 center(Random::vector3(-100, 100));
			const Scalar radius = Random::scalar(Scalar(0.01), 100);

			const BoundingSphere sphere(center, radius);

			const Vector3 rayDirection(Random::vector3());
			const Vector3 offsetDirection(Random::vector3());
			const Vector3 rayPosition(center + offsetDirection * (radius * Scalar(0.9)));

			ocean_assert(sphere.isInside(rayPosition));

			const Line3 intersectingRay(rayPosition, rayDirection);

			Vector3 position;
			Scalar distance;

			bool localSucceeded = true;

			if (sphere.positiveFrontIntersection(intersectingRay, position, distance))
			{
				localSucceeded = false;
			}

			if (localSucceeded)
			{
				validIterations++;
			}

			iterations++;
		}

		// back test
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			const Vector3 center(Random::vector3(-100, 100));
			const Scalar radius = Random::scalar(Scalar(0.01), 100);

			const BoundingSphere sphere(center, radius);

			const Vector3 rayDirection(Random::vector3());
			const Vector3 offsetDirection(Random::vector3());
			const Vector3 rayPosition((center + offsetDirection * (radius * Scalar(0.5))) + rayDirection * Random::scalar(radius * 2, radius * 10));

			ocean_assert(!sphere.isInside(rayPosition));

			const Line3 intersectingRay(rayPosition, rayDirection);

			Vector3 position;
			Scalar distance;

			bool localSucceeded = true;

			if (sphere.positiveFrontIntersection(intersectingRay, position, distance))
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
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestBoundingSphere::testPositiveBackIntersection(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Positive back intersection test:";

	const Scalar epsilon = std::is_same<float, Scalar>::value ? Scalar(0.001) : Numeric::eps();

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		// front test
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			const Vector3 center(Random::vector3(-100, 100));
			const Scalar radius = Random::scalar(Scalar(0.05), 100);

			const BoundingSphere sphere(center, radius);

			const Vector3 rayDirection(Random::vector3());
			const Vector3 offsetDirection(Random::vector3());
			const Vector3 rayPosition((center + offsetDirection * (radius * Scalar(0.5))) + rayDirection * Random::scalar(radius * -10, radius * -2));

			ocean_assert(!sphere.isInside(rayPosition));

			const Line3 intersectingRay(rayPosition, rayDirection);

			Vector3 position;
			Scalar distance;

			bool localSucceeded = true;

			if (sphere.positiveBackIntersection(intersectingRay, position, distance))
			{
				ocean_assert(intersectingRay.isOnLine(position));

				if (Numeric::isNotWeakEqual(center.distance(position), radius))
					localSucceeded = false;

				const Vector3 testPosition = intersectingRay.point(distance);
				if (!testPosition.isEqual(position, epsilon))
					localSucceeded = false;

				Vector3 position2(0, 0, 0);
				Scalar distance2;
				Vector3 normal(0, 0, 0);
				const bool localResult = sphere.positiveBackIntersection(intersectingRay, position2, distance2, normal);

				ocean_assert_and_suppress_unused(localResult, localResult);
				ocean_assert(position == position2);
				ocean_assert(distance == distance2);

				const Vector3 testNormal = (position - center).normalizedOrZero();
				if (!testNormal.isEqual(normal, epsilon))
					localSucceeded = false;

				if (normal * intersectingRay.direction() < 0)
					localSucceeded = false;
			}
			else
				localSucceeded = false;

			if (localSucceeded)
				validIterations++;

			iterations++;
		}

		// center test
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			const Vector3 center(Random::vector3(-100, 100));
			const Scalar radius = Random::scalar(Scalar(0.01), 100);

			const BoundingSphere sphere(center, radius);

			const Vector3 rayDirection(Random::vector3());
			const Vector3 offsetDirection(Random::vector3());
			const Vector3 rayPosition(center + offsetDirection * (radius * Scalar(0.9)));

			ocean_assert(sphere.isInside(rayPosition));

			const Line3 intersectingRay(rayPosition, rayDirection);

			Vector3 position;
			Scalar distance;

			bool localSucceeded = true;

			if (sphere.positiveBackIntersection(intersectingRay, position, distance))
			{
				ocean_assert(intersectingRay.isOnLine(position));

				if (Numeric::isNotWeakEqual(center.distance(position), radius))
					localSucceeded = false;

				const Vector3 testPosition = intersectingRay.point(distance);
				if (!testPosition.isEqual(position, Numeric::weakEps()))
					localSucceeded = false;

				Vector3 position2(0, 0, 0);
				Scalar distance2;
				Vector3 normal(0, 0, 0);
				const bool localResult = sphere.positiveBackIntersection(intersectingRay, position2, distance2, normal);

				ocean_assert_and_suppress_unused(localResult, localResult);
				ocean_assert(position == position2);
				ocean_assert(distance == distance2);

				const Vector3 testNormal = (position - center).normalizedOrZero();
				if (!testNormal.isEqual(normal, Numeric::weakEps()))
				{
					localSucceeded = false;
				}

				if (normal * intersectingRay.direction() < 0)
				{
					localSucceeded = false;
				}
			}
			else
			{
				localSucceeded = false;
			}

			if (localSucceeded)
			{
				validIterations++;
			}

			iterations++;
		}

		// back test
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			const Vector3 center(Random::vector3(-100, 100));
			const Scalar radius = Random::scalar(Scalar(0.01), 100);

			const BoundingSphere sphere(center, radius);

			const Vector3 rayDirection(Random::vector3());
			const Vector3 offsetDirection(Random::vector3());
			const Vector3 rayPosition((center + offsetDirection * (radius * Scalar(0.5))) + rayDirection * Random::scalar(radius * 2, radius * 10));

			ocean_assert(!sphere.isInside(rayPosition));

			const Line3 intersectingRay(rayPosition, rayDirection);

			Vector3 position;
			Scalar distance;

			bool localSucceeded = true;

			if (sphere.positiveBackIntersection(intersectingRay, position, distance))
				localSucceeded = false;

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

}

}

}
