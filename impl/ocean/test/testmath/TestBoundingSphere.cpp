/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
	{
		Log::info() << "Bounding sphere test succeeded.";
	}
	else
	{
		Log::info() << "Bounding sphere test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestBoundingSphere, Constructor)
{
	EXPECT_TRUE(TestBoundingSphere::testConstructor(GTEST_TEST_DURATION));
}

TEST(TestBoundingSphere, Intersections)
{
	EXPECT_TRUE(TestBoundingSphere::testIntersections(GTEST_TEST_DURATION));
}

TEST(TestBoundingSphere, IntersectionsTransformed)
{
	EXPECT_TRUE(TestBoundingSphere::testIntersectionsTransformed(GTEST_TEST_DURATION));
}

TEST(TestBoundingSphere, PositiveFrontIntersection)
{
	EXPECT_TRUE(TestBoundingSphere::testPositiveFrontIntersection(GTEST_TEST_DURATION));
}

TEST(TestBoundingSphere, PositiveBackIntersection)
{
	EXPECT_TRUE(TestBoundingSphere::testPositiveBackIntersection(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestBoundingSphere::testConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Constructor test:";

	constexpr Scalar range = std::is_same<float, Scalar>::value ? Scalar(10) : Scalar(1000);
	constexpr Scalar epsilon = std::is_same<float, Scalar>::value ? Scalar(0.001) : Numeric::eps();

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Vector3 lower(Random::vector3(-range, range));
			const Vector3 higher(lower + Random::vector3(Scalar(0.05), range));
			const Vector3 center((lower + higher) * Scalar(0.5));

			ocean_assert(Numeric::isEqual(center.distance(lower), center.distance(higher), epsilon));

			const Box3 boundingBox(lower, higher);
			const BoundingSphere sphere(boundingBox);

			bool localSucceeded = true;

			if (sphere.center() != center)
			{
				localSucceeded = false;
			}

			if (Numeric::isNotEqual(sphere.radius(), center.distance(lower), epsilon))
			{
				localSucceeded = false;
			}

			if (Numeric::isNotEqual(sphere.radius(), center.distance(higher), epsilon))
			{
				localSucceeded = false;
			}

			if (localSucceeded)
			{
				++validIterations;
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

bool TestBoundingSphere::testIntersections(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Intersections test:";

	constexpr Scalar range = std::is_same<float, Scalar>::value ? Scalar(10) : Scalar(1000);
	constexpr Scalar epsilon = std::is_same<float, Scalar>::value ? Scalar(0.001) : Numeric::eps();

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			const Vector3 center(Random::vector3(-range, range));
			const Scalar radius = Random::scalar(Scalar(0.05), range);

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
				{
					localSucceeded = false;
				}

				if (Numeric::isNotEqual(center.distance(position1), radius, epsilon))
				{
					localSucceeded = false;
				}

				const Vector3 testPosition0 = intersectingRay.point(distance0);
				if (!testPosition0.isEqual(position0, epsilon))
				{
					localSucceeded = false;
				}

				const Vector3 testPosition1 = intersectingRay.point(distance1);
				if (!testPosition1.isEqual(position1, epsilon))
				{
					localSucceeded = false;
				}

				const Vector3 testNormal0 = (position0 - center).normalizedOrZero();
				if (!testNormal0.isEqual(normal0, epsilon))
				{
					localSucceeded = false;
				}

				const Vector3 testNormal1 = (position1 - center).normalizedOrZero();
				if (!testNormal1.isEqual(normal1, epsilon))
				{
					localSucceeded = false;
				}
			}
			else
			{
				localSucceeded = false;
			}

			const Line3 arbitraryRay(Random::vector3(-range, range), Random::vector3());
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
				{
					localSucceeded = false;
				}
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
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestBoundingSphere::testIntersectionsTransformed(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Intersections test (transformed sphere):";

	constexpr Scalar range = std::is_same<float, Scalar>::value ? Scalar(10) : Scalar(1000);
	constexpr Scalar epsilon = std::is_same<float, Scalar>::value ? Scalar(0.001) : Numeric::weakEps();

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			bool localSucceeded = true;

			const Vector3 center(Random::vector3(-range, range));
			const Scalar radius = Random::scalar(Scalar(0.01), range);
			const BoundingSphere sphere(center, radius);

			// transformation transforming sphere-points to world-points
			const HomogenousMatrix4 world_T_sphere(Random::vector3(-range, range), Random::rotation(), Random::vector3(Scalar(0.05), 10));
			const HomogenousMatrix4 sphere_T_world(world_T_sphere.inverted());

			const Line3 worldRay(Random::vector3(-range, range), Random::vector3());

			const Vector3 sphereRayPoint(sphere_T_world * worldRay.point());
			const Vector3 sphereRayDirection((sphere_T_world.rotationMatrix() * worldRay.direction()).normalizedOrZero());
			ocean_assert(Numeric::isEqual(sphereRayDirection.length(), 1));

			const Line3 sRay(sphereRayPoint, sphereRayDirection);

			Vector3 position0, position1;
			Scalar distance0, distance1;
			Vector3 normal0, normal1;
			if (sphere.intersections(worldRay, world_T_sphere, sphere_T_world, position0, distance0, normal0, position1, distance1, normal1))
			{
				ocean_assert(worldRay.isOnLine(position0));
				ocean_assert(worldRay.isOnLine(position1));

				const Vector3 spherePosition0 = sphere_T_world * position0;
				const Vector3 spherePosition1 = sphere_T_world * position1;

				if (Numeric::isNotEqual(center.distance(spherePosition0), radius, epsilon))
				{
					localSucceeded = false;
				}

				if (Numeric::isNotEqual(center.distance(spherePosition1), radius, epsilon))
				{
					localSucceeded = false;
				}

				const Vector3 testPosition0 = worldRay.point(distance0);
				if (!testPosition0.isEqual(position0, epsilon))
				{
					localSucceeded = false;
				}

				const Vector3 testPosition1 = worldRay.point(distance1);
				if (!testPosition1.isEqual(position1, epsilon))
				{
					localSucceeded = false;
				}

				const Vector3 sphereTestNormal0 = (spherePosition0 - center).normalizedOrZero();
				const Vector3 worldTestNormal0 = (sphere_T_world.rotationMatrix().transposed() * sphereTestNormal0).normalizedOrZero();
				if (!worldTestNormal0.isEqual(normal0, epsilon))
				{
					localSucceeded = false;
				}

				const Vector3 sphereTestNormal1 = (spherePosition1 - center).normalizedOrZero();
				const Vector3 worldTestNormal1 = (sphere_T_world.rotationMatrix().transposed() * sphereTestNormal1).normalizedOrZero();
				if (!worldTestNormal1.isEqual(normal1, epsilon))
				{
					localSucceeded = false;
				}
			}
			else
			{
				if (sphere.intersections(sRay, position0, distance0, normal0, position1, distance1, normal1))
				{
					localSucceeded = false;
				}
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
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestBoundingSphere::testPositiveFrontIntersection(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Positive front intersection test:";

	constexpr Scalar range = std::is_same<float, Scalar>::value ? Scalar(10) : Scalar(1000);
	constexpr Scalar epsilon = std::is_same<float, Scalar>::value ? Scalar(0.001) : Numeric::eps();

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		// front test
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			const Vector3 center(Random::vector3(-range, range));
			const Scalar radius = Random::scalar(Scalar(0.01), range);

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
			const Vector3 center(Random::vector3(-range, range));
			const Scalar radius = Random::scalar(Scalar(0.01), range);

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
			const Vector3 center(Random::vector3(-range, range));
			const Scalar radius = Random::scalar(Scalar(0.01), range);

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

	constexpr Scalar range = std::is_same<float, Scalar>::value ? Scalar(10) : Scalar(1000);
	constexpr Scalar epsilon = std::is_same<float, Scalar>::value ? Scalar(0.001) : Numeric::eps();

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		// front test
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			const Vector3 center(Random::vector3(-range, range));
			const Scalar radius = Random::scalar(Scalar(0.05), range);

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
				{
					localSucceeded = false;
				}

				const Vector3 testPosition = intersectingRay.point(distance);
				if (!testPosition.isEqual(position, epsilon))
				{
					localSucceeded = false;
				}

				Vector3 position2(0, 0, 0);
				Scalar distance2;
				Vector3 normal(0, 0, 0);
				const bool localResult = sphere.positiveBackIntersection(intersectingRay, position2, distance2, normal);

				ocean_assert_and_suppress_unused(localResult, localResult);
				ocean_assert(position == position2);
				ocean_assert(distance == distance2);

				const Vector3 testNormal = (position - center).normalizedOrZero();
				if (!testNormal.isEqual(normal, epsilon))
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
				++validIterations;
			}

			iterations++;
		}

		// center test
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			const Vector3 center(Random::vector3(-range, range));
			const Scalar radius = Random::scalar(Scalar(0.01), range);

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
				{
					localSucceeded = false;
				}

				const Vector3 testPosition = intersectingRay.point(distance);
				if (!testPosition.isEqual(position, Numeric::weakEps()))
				{
					localSucceeded = false;
				}

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
				++validIterations;
			}

			++iterations;
		}

		// back test
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			const Vector3 center(Random::vector3(-range, range));
			const Scalar radius = Random::scalar(Scalar(0.01), range);

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
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

}

}

}
