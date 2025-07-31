/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestBoundingBox.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/Random.h"
#include "ocean/math/BoundingBox.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestBoundingBox::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Bounding box test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testPositiveFrontIntersection(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPositiveBackIntersection(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
		Log::info() << "Bounding box test succeeded.";
	else
		Log::info() << "Bounding box test FAILED!";

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestBoundingBox, PositiveFrontIntersection) {
	EXPECT_TRUE(TestBoundingBox::testPositiveFrontIntersection(GTEST_TEST_DURATION));
}

TEST(TestBoundingBox, PositiveBackIntersection) {
	EXPECT_TRUE(TestBoundingBox::testPositiveBackIntersection(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestBoundingBox::testPositiveFrontIntersection(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Positive front intersection test:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Scalar epsilon = std::is_same<float, Scalar>::value ? Scalar(0.0001) : Numeric::eps();

	const Timestamp startTimestamp(true);

	do
	{
		// front test
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			const Vector3 lower(Random::vector3(-100, 100));
			const Vector3 offset(Random::vector3(Scalar(0.05), 100));
			const Vector3 higher(lower + offset);

			const BoundingBox box(lower, higher);
			const Vector3 center(box.center());
			const Scalar radius = min(box.xDimension(), min(box.yDimension(), box.zDimension())) * Scalar(0.5);
			const Scalar diagonal = box.diagonal();

			const Vector3 rayDirection(Random::vector3());
			const Vector3 offsetDirection(Random::vector3());
			const Vector3 rayPosition((center + offsetDirection * (radius * Scalar(0.5))) + rayDirection * Random::scalar(diagonal * -10, diagonal * -2));

			ocean_assert(!box.isInside(rayPosition));
			ocean_assert(!box.isOnSurface(rayPosition));

			const Line3 intersectingRay(rayPosition, rayDirection);

			Vector3 position;
			Scalar distance;

			bool localSucceeded = true;

			if (box.positiveFrontIntersection(intersectingRay, position, distance))
			{
				ocean_assert(intersectingRay.isOnLine(position));

				if (!box.isOnSurface(position, epsilon))
					localSucceeded = false;

				const Vector3 testPosition = intersectingRay.point(distance);
				if (!testPosition.isEqual(position, epsilon))
					localSucceeded = false;

				if (intersectingRay.point().distance(center) < intersectingRay.point().distance(position))
					localSucceeded = false;

				Vector3 position2(0, 0, 0);
				Scalar distance2;
				Vector3 normal2(0, 0, 0);
				const bool localResult2 = box.positiveFrontIntersection(intersectingRay, position2, distance2, normal2);

				ocean_assert_and_suppress_unused(localResult2, localResult2);
				ocean_assert(position == position2);
				ocean_assert(distance == distance2);

				const Vector3 normalOffset = position + normal2 * Scalar(0.01);
				if (box.isInside(normalOffset) || box.isInside(normalOffset, Numeric::eps()) || box.isOnSurface(normalOffset, epsilon))
					localSucceeded = false;

				if (normal2 * intersectingRay.direction() > 0)
					localSucceeded = false;

				Vector3 position3(0, 0, 0);
				Scalar distance3;
				Vector3 normal3(0, 0, 0);
				Vector2 textureCoordinate3(0, 0);
				const bool localResult = box.positiveFrontIntersection(intersectingRay, position3, distance3, normal3, textureCoordinate3);

				ocean_assert_and_suppress_unused(localResult, localResult);
				ocean_assert(position == position3);
				ocean_assert(distance == distance3);
				ocean_assert(normal2 == normal3);

				ocean_assert(textureCoordinate3.x() >= 0 && textureCoordinate3.x() <= 1 && textureCoordinate3.y() >= 0 && textureCoordinate3.y() <= 1);
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
			const Vector3 lower(Random::vector3(-100, 100));
			const Vector3 offset(Random::vector3(Scalar(0.05), 100));
			const Vector3 higher(lower + offset);

			const BoundingBox box(lower, higher);
			const Vector3 center(box.center());
			const Scalar radius = min(box.xDimension(), min(box.yDimension(), box.zDimension())) * Scalar(0.5);

			const Vector3 rayDirection(Random::vector3());
			const Vector3 offsetDirection(Random::vector3());
			const Vector3 rayPosition(center + offsetDirection * (radius * Scalar(0.9)));

			ocean_assert(box.isInside(rayPosition));
			ocean_assert(!box.isOnSurface(rayPosition, epsilon));

			const Line3 intersectingRay(rayPosition, rayDirection);

			Vector3 position;
			Scalar distance;
			Vector3 normal;
			Vector2 textureCoordinate;

			bool localSucceeded = true;

			if (box.positiveFrontIntersection(intersectingRay, position, distance)
					|| box.positiveFrontIntersection(intersectingRay, position, distance, normal)
					|| box.positiveFrontIntersection(intersectingRay, position, distance, normal, textureCoordinate))
				localSucceeded = false;

			if (localSucceeded)
				validIterations++;

			iterations++;
		}

		// back test
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			const Vector3 lower(Random::vector3(-100, 100));
			const Vector3 offset(Random::vector3(Scalar(0.05), 100));
			const Vector3 higher(lower + offset);

			const BoundingBox box(lower, higher);
			const Vector3 center(box.center());
			const Scalar radius = min(box.xDimension(), min(box.yDimension(), box.zDimension())) * Scalar(0.5);
			const Scalar diagonal = box.diagonal();

			const Vector3 rayDirection(Random::vector3());
			const Vector3 offsetDirection(Random::vector3());
			const Vector3 rayPosition((center + offsetDirection * (radius * Scalar(0.5))) + rayDirection * Random::scalar(diagonal * 2, diagonal * 10));

			ocean_assert(!box.isInside(rayPosition));
			ocean_assert(!box.isOnSurface(rayPosition, epsilon));

			const Line3 intersectingRay(rayPosition, rayDirection);

			Vector3 position;
			Scalar distance;
			Vector3 normal;
			Vector2 textureCoordinate;

			bool localSucceeded = true;

			if (box.positiveFrontIntersection(intersectingRay, position, distance)
					|| box.positiveFrontIntersection(intersectingRay, position, distance, normal)
					|| box.positiveFrontIntersection(intersectingRay, position, distance, normal, textureCoordinate))
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

bool TestBoundingBox::testPositiveBackIntersection(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Positive back intersection test:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Scalar epsilon = std::is_same<float, Scalar>::value ? Scalar(0.0001) : Numeric::eps();

	const Timestamp startTimestamp(true);

	do
	{
		// front test
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			const Vector3 lower(Random::vector3(-100, 100));
			const Vector3 offset(Random::vector3(Scalar(0.05), 100));
			const Vector3 higher(lower + offset);

			const BoundingBox box(lower, higher);
			const Vector3 center(box.center());
			const Scalar radius = min(box.xDimension(), min(box.yDimension(), box.zDimension())) * Scalar(0.5);
			const Scalar diagonal = box.diagonal();

			const Vector3 rayDirection(Random::vector3());
			const Vector3 offsetDirection(Random::vector3());
			const Vector3 rayPosition((center + offsetDirection * (radius * Scalar(0.5))) + rayDirection * Random::scalar(diagonal * -10, diagonal * -2));

			ocean_assert(!box.isInside(rayPosition));
			ocean_assert(!box.isOnSurface(rayPosition));

			const Line3 intersectingRay(rayPosition, rayDirection);

			Vector3 position;
			Scalar distance;

			bool localSucceeded = true;

			if (box.positiveBackIntersection(intersectingRay, position, distance))
			{
				ocean_assert(intersectingRay.isOnLine(position));

				if (!box.isOnSurface(position, epsilon))
					localSucceeded = false;

				const Vector3 testPosition = intersectingRay.point(distance);
				if (!testPosition.isEqual(position, epsilon))
					localSucceeded = false;

				Vector3 position2(0, 0, 0);
				Scalar distance2;
				Vector3 normal2(0, 0, 0);
				const bool localResult2 = box.positiveBackIntersection(intersectingRay, position2, distance2, normal2);

				ocean_assert_and_suppress_unused(localResult2, localResult2);
				ocean_assert(position == position2);
				ocean_assert(Numeric::isEqual(distance, distance2));

				const Vector3 normalOffset = position + normal2 * Scalar(0.01);
				if (box.isInside(normalOffset) || box.isInside(normalOffset, Numeric::eps()) || box.isOnSurface(normalOffset, epsilon))
					localSucceeded = false;

				if (normal2 * intersectingRay.direction() < 0)
					localSucceeded = false;

				Vector3 position3(0, 0, 0);
				Scalar distance3;
				Vector3 normal3(0, 0, 0);
				Vector2 textureCoordinate3(0, 0);
				const bool localResult = box.positiveBackIntersection(intersectingRay, position3, distance3, normal3, textureCoordinate3);

				ocean_assert_and_suppress_unused(localResult, localResult);
				ocean_assert(position == position3);
				ocean_assert(distance == distance3);
				ocean_assert(normal2 == normal3);

				ocean_assert(textureCoordinate3.x() >= 0 && textureCoordinate3.x() <= 1 && textureCoordinate3.y() >= 0 && textureCoordinate3.y() <= 1);
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
			const Vector3 lower(Random::vector3(-100, 100));
			const Vector3 offset(Random::vector3(Scalar(0.05), 100));
			const Vector3 higher(lower + offset);

			const BoundingBox box(lower, higher);
			const Vector3 center(box.center());
			const Scalar radius = min(box.xDimension(), min(box.yDimension(), box.zDimension())) * Scalar(0.5);

			const Vector3 rayDirection(Random::vector3());
			const Vector3 offsetDirection(Random::vector3());
			const Vector3 rayPosition(center + offsetDirection * (radius * Scalar(0.9)));

			ocean_assert(box.isInside(rayPosition));
			ocean_assert(!box.isOnSurface(rayPosition, epsilon));

			const Line3 intersectingRay(rayPosition, rayDirection);

			Vector3 position;
			Scalar distance;

			bool localSucceeded = true;

			if (box.positiveBackIntersection(intersectingRay, position, distance))
			{
				ocean_assert(intersectingRay.isOnLine(position));

				if (!box.isOnSurface(position, epsilon))
					localSucceeded = false;

				const Vector3 testPosition = intersectingRay.point(distance);
				if (!testPosition.isEqual(position, epsilon))
					localSucceeded = false;

				Vector3 position2(0, 0, 0);
				Scalar distance2;
				Vector3 normal2(0, 0, 0);
				const bool localResult2 = box.positiveBackIntersection(intersectingRay, position2, distance2, normal2);

				ocean_assert_and_suppress_unused(localResult2, localResult2);
				ocean_assert(position == position2);
				ocean_assert(Numeric::isEqual(distance, distance2));

				const Vector3 normalOffset = position + normal2 * Scalar(0.01);
				if (box.isInside(normalOffset) || box.isInside(normalOffset, Numeric::eps()) || box.isOnSurface(normalOffset, epsilon))
					localSucceeded = false;

				if (normal2 * intersectingRay.direction() < 0)
					localSucceeded = false;

				Vector3 position3(0, 0, 0);
				Scalar distance3;
				Vector3 normal3(0, 0, 0);
				Vector2 textureCoordinate3(0, 0);
				const bool localResult = box.positiveBackIntersection(intersectingRay, position3, distance3, normal3, textureCoordinate3);

				ocean_assert_and_suppress_unused(localResult, localResult);
				ocean_assert(position == position3);
				ocean_assert(distance == distance3);
				ocean_assert(normal2 == normal3);

				ocean_assert(textureCoordinate3.x() >= 0 && textureCoordinate3.x() <= 1 && textureCoordinate3.y() >= 0 && textureCoordinate3.y() <= 1);
			}
			else
				localSucceeded = false;

			if (localSucceeded)
				validIterations++;

			iterations++;
		}

		// back test
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			const Vector3 lower(Random::vector3(-100, 100));
			const Vector3 offset(Random::vector3(Scalar(0.05), 100));
			const Vector3 higher(lower + offset);

			const BoundingBox box(lower, higher);
			const Vector3 center(box.center());
			const Scalar radius = min(box.xDimension(), min(box.yDimension(), box.zDimension())) * Scalar(0.5);
			const Scalar diagonal = box.diagonal();

			const Vector3 rayDirection(Random::vector3());
			const Vector3 offsetDirection(Random::vector3());
			const Vector3 rayPosition((center + offsetDirection * (radius * Scalar(0.5))) + rayDirection * Random::scalar(diagonal * 2, diagonal * 10));

			ocean_assert(!box.isInside(rayPosition));
			ocean_assert(!box.isOnSurface(rayPosition, epsilon));

			const Line3 intersectingRay(rayPosition, rayDirection);

			Vector3 position;
			Scalar distance;
			Vector3 normal;
			Vector2 textureCoordinate;

			bool localSucceeded = true;

			if (box.positiveBackIntersection(intersectingRay, position, distance)
					|| box.positiveBackIntersection(intersectingRay, position, distance, normal)
					|| box.positiveBackIntersection(intersectingRay, position, distance, normal, textureCoordinate))
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
