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

#include "ocean/test/TestResult.h"
#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestBoundingSphere::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Bounding sphere test");

	Log::info() << " ";

	if (selector.shouldRun("constructor"))
	{
		testResult = testConstructor(testDuration);

		Log::info() << " ";
	}

	if (selector.shouldRun("intersections"))
	{
		testResult = testIntersections(testDuration);

		Log::info() << " ";
	}

	if (selector.shouldRun("intersectionstransformed"))
	{
		testResult = testIntersectionsTransformed(testDuration);

		Log::info() << " ";
	}

	if (selector.shouldRun("positivefrontintersection"))
	{
		testResult = testPositiveFrontIntersection(testDuration);

		Log::info() << " ";
	}

	if (selector.shouldRun("positivebackintersection"))
	{
		testResult = testPositiveBackIntersection(testDuration);

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
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

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.99, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Vector3 lower(Random::vector3(randomGenerator, -range, range));
			const Vector3 higher(lower + Random::vector3(randomGenerator, Scalar(0.05), range));
			const Vector3 center((lower + higher) * Scalar(0.5));

			ocean_assert(Numeric::isEqual(center.distance(lower), center.distance(higher), epsilon));

			const Box3 boundingBox(lower, higher);
			const BoundingSphere sphere(boundingBox);

			if (sphere.center() != center)
			{
				scopedIteration.setInaccurate();
			}

			if (Numeric::isNotEqual(sphere.radius(), center.distance(lower), epsilon))
			{
				scopedIteration.setInaccurate();
			}

			if (Numeric::isNotEqual(sphere.radius(), center.distance(higher), epsilon))
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestBoundingSphere::testIntersections(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Intersections test:";

	constexpr Scalar range = std::is_same<float, Scalar>::value ? Scalar(10) : Scalar(1000);
	constexpr Scalar epsilon = std::is_same<float, Scalar>::value ? Scalar(0.001) : Numeric::eps();

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.99, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Vector3 center(Random::vector3(randomGenerator, -range, range));
			const Scalar radius = Random::scalar(randomGenerator, Scalar(0.05), range);

			const BoundingSphere sphere(center, radius);

			const Vector3 rayDirection(Random::vector3(randomGenerator));
			const Vector3 offsetDirection(Random::vector3(randomGenerator));
			const Vector3 rayPosition((center + offsetDirection * (radius * Scalar(0.5))) + rayDirection * Random::scalar(randomGenerator, radius * -2, radius * 2));

			const Line3 intersectingRay(rayPosition, rayDirection);

			Vector3 position0, position1;
			Scalar distance0, distance1;
			Vector3 normal0, normal1;

			if (sphere.intersections(intersectingRay, position0, distance0, normal0, position1, distance1, normal1))
			{
				ocean_assert(intersectingRay.isOnLine(position0));
				ocean_assert(intersectingRay.isOnLine(position1));

				if (Numeric::isNotEqual(center.distance(position0), radius, epsilon))
				{
					scopedIteration.setInaccurate();
				}

				if (Numeric::isNotEqual(center.distance(position1), radius, epsilon))
				{
					scopedIteration.setInaccurate();
				}

				const Vector3 testPosition0 = intersectingRay.point(distance0);
				if (!testPosition0.isEqual(position0, epsilon))
				{
					scopedIteration.setInaccurate();
				}

				const Vector3 testPosition1 = intersectingRay.point(distance1);
				if (!testPosition1.isEqual(position1, epsilon))
				{
					scopedIteration.setInaccurate();
				}

				const Vector3 testNormal0 = (position0 - center).normalizedOrZero();
				if (!testNormal0.isEqual(normal0, epsilon))
				{
					scopedIteration.setInaccurate();
				}

				const Vector3 testNormal1 = (position1 - center).normalizedOrZero();
				if (!testNormal1.isEqual(normal1, epsilon))
				{
					scopedIteration.setInaccurate();
				}
			}
			else
			{
				scopedIteration.setInaccurate();
			}

			const Vector3 arbitraryRayPosition(Random::vector3(randomGenerator, -range, range));
			const Vector3 arbitraryRayDirection(Random::vector3(randomGenerator));
			const Line3 arbitraryRay(arbitraryRayPosition, arbitraryRayDirection);
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
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestBoundingSphere::testIntersectionsTransformed(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Intersections test (transformed sphere):";

	constexpr Scalar range = std::is_same<float, Scalar>::value ? Scalar(10) : Scalar(1000);
	constexpr Scalar epsilon = std::is_same<float, Scalar>::value ? Scalar(0.001) : Numeric::weakEps();

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.99, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Vector3 center(Random::vector3(randomGenerator, -range, range));
			const Scalar radius = Random::scalar(randomGenerator, Scalar(0.01), range);
			const BoundingSphere sphere(center, radius);

			// transformation transforming sphere-points to world-points
			const Vector3 worldTranslation(Random::vector3(randomGenerator, -range, range));
			const Rotation worldRotation(Random::rotation(randomGenerator));
			const Vector3 worldScale(Random::vector3(randomGenerator, Scalar(0.05), 10));
			const HomogenousMatrix4 world_T_sphere(worldTranslation, worldRotation, worldScale);
			const HomogenousMatrix4 sphere_T_world(world_T_sphere.inverted());

			const Vector3 worldRayPosition(Random::vector3(randomGenerator, -range, range));
			const Vector3 worldRayDirection(Random::vector3(randomGenerator));
			const Line3 worldRay(worldRayPosition, worldRayDirection);

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
					scopedIteration.setInaccurate();
				}

				if (Numeric::isNotEqual(center.distance(spherePosition1), radius, epsilon))
				{
					scopedIteration.setInaccurate();
				}

				const Vector3 testPosition0 = worldRay.point(distance0);
				if (!testPosition0.isEqual(position0, epsilon))
				{
					scopedIteration.setInaccurate();
				}

				const Vector3 testPosition1 = worldRay.point(distance1);
				if (!testPosition1.isEqual(position1, epsilon))
				{
					scopedIteration.setInaccurate();
				}

				const Vector3 sphereTestNormal0 = (spherePosition0 - center).normalizedOrZero();
				const Vector3 worldTestNormal0 = (sphere_T_world.rotationMatrix().transposed() * sphereTestNormal0).normalizedOrZero();
				if (!worldTestNormal0.isEqual(normal0, epsilon))
				{
					scopedIteration.setInaccurate();
				}

				const Vector3 sphereTestNormal1 = (spherePosition1 - center).normalizedOrZero();
				const Vector3 worldTestNormal1 = (sphere_T_world.rotationMatrix().transposed() * sphereTestNormal1).normalizedOrZero();
				if (!worldTestNormal1.isEqual(normal1, epsilon))
				{
					scopedIteration.setInaccurate();
				}
			}
			else
			{
				if (sphere.intersections(sRay, position0, distance0, normal0, position1, distance1, normal1))
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

bool TestBoundingSphere::testPositiveFrontIntersection(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Positive front intersection test:";

	constexpr Scalar range = std::is_same<float, Scalar>::value ? Scalar(10) : Scalar(1000);
	constexpr Scalar epsilon = std::is_same<float, Scalar>::value ? Scalar(0.001) : Numeric::eps();

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.99, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		// front test
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Vector3 center(Random::vector3(randomGenerator, -range, range));
			const Scalar radius = Random::scalar(randomGenerator, Scalar(0.01), range);

			const BoundingSphere sphere(center, radius);

			const Vector3 rayDirection(Random::vector3(randomGenerator));
			const Vector3 offsetDirection(Random::vector3(randomGenerator));
			const Vector3 rayPosition((center + offsetDirection * (radius * Scalar(0.5))) + rayDirection * Random::scalar(randomGenerator, radius * -10, radius * -2));

			ocean_assert(!sphere.isInside(rayPosition));

			const Line3 intersectingRay(rayPosition, rayDirection);

			Vector3 position;
			Scalar distance;

			if (sphere.positiveFrontIntersection(intersectingRay, position, distance))
			{
				ocean_assert(intersectingRay.isOnLine(position));

				if (Numeric::isNotWeakEqual(center.distance(position), radius))
				{
					scopedIteration.setInaccurate();
				}

				const Vector3 testPosition = intersectingRay.point(distance);
				if (!testPosition.isEqual(position, epsilon))
				{
					scopedIteration.setInaccurate();
				}

				if (intersectingRay.point().distance(center) < intersectingRay.point().distance(position))
				{
					scopedIteration.setInaccurate();
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
					scopedIteration.setInaccurate();
				}

				if (normal * intersectingRay.direction() > 0)
				{
					scopedIteration.setInaccurate();
				}
			}
			else
			{
				scopedIteration.setInaccurate();
			}
		}

		// center test
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Vector3 center(Random::vector3(randomGenerator, -range, range));
			const Scalar radius = Random::scalar(randomGenerator, Scalar(0.01), range);

			const BoundingSphere sphere(center, radius);

			const Vector3 rayDirection(Random::vector3(randomGenerator));
			const Vector3 offsetDirection(Random::vector3(randomGenerator));
			const Vector3 rayPosition(center + offsetDirection * (radius * Scalar(0.9)));

			ocean_assert(sphere.isInside(rayPosition));

			const Line3 intersectingRay(rayPosition, rayDirection);

			Vector3 position;
			Scalar distance;

			if (sphere.positiveFrontIntersection(intersectingRay, position, distance))
			{
				scopedIteration.setInaccurate();
			}
		}

		// back test
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Vector3 center(Random::vector3(randomGenerator, -range, range));
			const Scalar radius = Random::scalar(randomGenerator, Scalar(0.01), range);

			const BoundingSphere sphere(center, radius);

			const Vector3 rayDirection(Random::vector3(randomGenerator));
			const Vector3 offsetDirection(Random::vector3(randomGenerator));
			const Vector3 rayPosition((center + offsetDirection * (radius * Scalar(0.5))) + rayDirection * Random::scalar(randomGenerator, radius * 2, radius * 10));

			ocean_assert(!sphere.isInside(rayPosition));

			const Line3 intersectingRay(rayPosition, rayDirection);

			Vector3 position;
			Scalar distance;

			if (sphere.positiveFrontIntersection(intersectingRay, position, distance))
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestBoundingSphere::testPositiveBackIntersection(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Positive back intersection test:";

	constexpr Scalar range = std::is_same<float, Scalar>::value ? Scalar(10) : Scalar(1000);
	constexpr Scalar epsilon = std::is_same<float, Scalar>::value ? Scalar(0.001) : Numeric::eps();

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.99, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		// front test
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Vector3 center(Random::vector3(randomGenerator, -range, range));
			const Scalar radius = Random::scalar(randomGenerator, Scalar(0.05), range);

			const BoundingSphere sphere(center, radius);

			const Vector3 rayDirection(Random::vector3(randomGenerator));
			const Vector3 offsetDirection(Random::vector3(randomGenerator));
			const Vector3 rayPosition((center + offsetDirection * (radius * Scalar(0.5))) + rayDirection * Random::scalar(randomGenerator, radius * -10, radius * -2));

			ocean_assert(!sphere.isInside(rayPosition));

			const Line3 intersectingRay(rayPosition, rayDirection);

			Vector3 position;
			Scalar distance;

			if (sphere.positiveBackIntersection(intersectingRay, position, distance))
			{
				ocean_assert(intersectingRay.isOnLine(position));

				if (Numeric::isNotWeakEqual(center.distance(position), radius))
				{
					scopedIteration.setInaccurate();
				}

				const Vector3 testPosition = intersectingRay.point(distance);
				if (!testPosition.isEqual(position, epsilon))
				{
					scopedIteration.setInaccurate();
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
					scopedIteration.setInaccurate();
				}

				if (normal * intersectingRay.direction() < 0)
				{
					scopedIteration.setInaccurate();
				}
			}
			else
			{
				scopedIteration.setInaccurate();
			}
		}

		// center test
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Vector3 center(Random::vector3(randomGenerator, -range, range));
			const Scalar radius = Random::scalar(randomGenerator, Scalar(0.01), range);

			const BoundingSphere sphere(center, radius);

			const Vector3 rayDirection(Random::vector3(randomGenerator));
			const Vector3 offsetDirection(Random::vector3(randomGenerator));
			const Vector3 rayPosition(center + offsetDirection * (radius * Scalar(0.9)));

			ocean_assert(sphere.isInside(rayPosition));

			const Line3 intersectingRay(rayPosition, rayDirection);

			Vector3 position;
			Scalar distance;

			if (sphere.positiveBackIntersection(intersectingRay, position, distance))
			{
				ocean_assert(intersectingRay.isOnLine(position));

				if (Numeric::isNotWeakEqual(center.distance(position), radius))
				{
					scopedIteration.setInaccurate();
				}

				const Vector3 testPosition = intersectingRay.point(distance);
				if (!testPosition.isEqual(position, Numeric::weakEps()))
				{
					scopedIteration.setInaccurate();
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
					scopedIteration.setInaccurate();
				}

				if (normal * intersectingRay.direction() < 0)
				{
					scopedIteration.setInaccurate();
				}
			}
			else
			{
				scopedIteration.setInaccurate();
			}
		}

		// back test
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Vector3 center(Random::vector3(randomGenerator, -range, range));
			const Scalar radius = Random::scalar(randomGenerator, Scalar(0.01), range);

			const BoundingSphere sphere(center, radius);

			const Vector3 rayDirection(Random::vector3(randomGenerator));
			const Vector3 offsetDirection(Random::vector3(randomGenerator));
			const Vector3 rayPosition((center + offsetDirection * (radius * Scalar(0.5))) + rayDirection * Random::scalar(randomGenerator, radius * 2, radius * 10));

			ocean_assert(!sphere.isInside(rayPosition));

			const Line3 intersectingRay(rayPosition, rayDirection);

			Vector3 position;
			Scalar distance;

			if (sphere.positiveBackIntersection(intersectingRay, position, distance))
			{
				scopedIteration.setInaccurate();
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
