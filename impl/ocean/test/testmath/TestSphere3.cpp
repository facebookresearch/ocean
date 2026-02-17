/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestSphere3.h"

#include "ocean/base/Timestamp.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/ValidationPrecision.h"

#include "ocean/math/Random.h"
#include "ocean/math/Sphere3.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestSphere3::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Sphere3 test");

	Log::info() << " ";

	if (selector.shouldRun("hasintersection"))
	{
		testResult = testHasIntersection<float>(testDuration);
		Log::info() << " ";
		testResult = testHasIntersection<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("hasintersectiontransformed"))
	{
		testResult = testHasIntersectionTransformed<float>(testDuration);
		Log::info() << " ";
		testResult = testHasIntersectionTransformed<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("coordinatevectorconversion"))
	{
		testResult = testCoordinateVectorConversion<float>(testDuration);
		Log::info() << " ";
		testResult = testCoordinateVectorConversion<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("shortestdistance"))
	{
		testResult = testShortestDistance<float>(testDuration);
		Log::info() << " ";
		testResult = testShortestDistance<double>(testDuration);

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestSphere3, HasIntersectionFloat)
{
	EXPECT_TRUE((TestSphere3::testHasIntersection<float>(GTEST_TEST_DURATION)));
}

TEST(TestSphere3, HasIntersectionDouble)
{
	EXPECT_TRUE((TestSphere3::testHasIntersection<double>(GTEST_TEST_DURATION)));
}


TEST(TestSphere3, HasIntersectionTransformedFloat)
{
	EXPECT_TRUE((TestSphere3::testHasIntersectionTransformed<float>(GTEST_TEST_DURATION)));
}

TEST(TestSphere3, HasIntersectionTransformedDouble)
{
	EXPECT_TRUE((TestSphere3::testHasIntersectionTransformed<double>(GTEST_TEST_DURATION)));
}


TEST(TestSphere3, CoordinateVectorConversionFloat)
{
	EXPECT_TRUE((TestSphere3::testCoordinateVectorConversion<float>(GTEST_TEST_DURATION)));
}

TEST(TestSphere3, CoordinateVectorConversionDouble)
{
	EXPECT_TRUE((TestSphere3::testCoordinateVectorConversion<double>(GTEST_TEST_DURATION)));
}


TEST(TestSphere3, ShortestDistanceFloat)
{
	EXPECT_TRUE((TestSphere3::testShortestDistance<float>(GTEST_TEST_DURATION)));
}

TEST(TestSphere3, ShortestDistanceDouble)
{
	EXPECT_TRUE((TestSphere3::testShortestDistance<double>(GTEST_TEST_DURATION)));
}

#endif // OCEAN_USE_GTEST

template <typename T>
bool TestSphere3::testHasIntersection(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Ray intersection test, with '" << TypeNamer::name<T>() << "':";

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.99, randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const VectorT3<T> center(RandomT<T>::vector3(-100, 100));
			const T radius = RandomT<T>::scalar(T(0.01), 100);
			const SphereT3<T> sphere(center, radius);

			const VectorT3<T> rayDirection(RandomT<T>::vector3());
			const VectorT3<T> offsetDirection(RandomT<T>::vector3());
			const VectorT3<T> rayPosition((center + offsetDirection * (radius * T(0.5))) + rayDirection * RandomT<T>::scalar(radius * -T(2), radius * T(2)));

			const LineT3<T> intersectingRay(rayPosition, rayDirection);

			if (!sphere.hasIntersection(intersectingRay))
			{
				scopedIteration.setInaccurate();
			}

			const LineT3<T> arbitraryRay(RandomT<T>::vector3(-100, 100), RandomT<T>::vector3());
			if (sphere.hasIntersection(arbitraryRay))
			{
				const VectorT3<T> nearestPoint(arbitraryRay.nearestPoint(center));
				if (center.distance(nearestPoint) > radius + NumericT<T>::eps())
				{
					scopedIteration.setInaccurate();
				}
			}
			else
			{
				const VectorT3<T> nearestPoint(arbitraryRay.nearestPoint(center));
				if (center.distance(nearestPoint) < radius + NumericT<T>::eps())
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

template <typename T>
bool TestSphere3::testHasIntersectionTransformed(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Ray intersection test (transformed sphere), with '" << TypeNamer::name<T>() << "':";

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.99, randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const VectorT3<T> center(RandomT<T>::vector3(-100, 100));
			const T radius = RandomT<T>::scalar(T(0.01), 100);
			const SphereT3<T> sphere(center, radius);

			// transformation transforming sphere-points to world-points
			const HomogenousMatrixT4<T> world_T_sphere(RandomT<T>::vector3(-100, 100), RandomT<T>::rotation(), RandomT<T>::vector3(T(0.01), 10));
			const HomogenousMatrixT4<T> sphere_T_world(world_T_sphere.inverted());

			const LineT3<T> worldRay(RandomT<T>::vector3(-100, 100), RandomT<T>::vector3());

			const VectorT3<T> sphereRayPoint(sphere_T_world * worldRay.point());
			const VectorT3<T> sphereRayDirection((sphere_T_world.rotationMatrix() * worldRay.direction()).normalizedOrZero());
			ocean_assert(NumericT<T>::isEqual(sphereRayDirection.length(), 1));

			const LineT3<T> sphereRay(sphereRayPoint, sphereRayDirection);

			if (sphere.hasIntersection(worldRay, sphere_T_world))
			{
				const VectorT3<T> sNearestPoint(sphereRay.nearestPoint(center));

				if (sNearestPoint.distance(center) > radius + NumericT<T>::eps())
				{
					scopedIteration.setInaccurate();
				}
			}
			else
			{
				const VectorT3<T> sphereNearestPoint(sphereRay.nearestPoint(center));

				if (sphereNearestPoint.distance(center) < radius + NumericT<T>::eps())
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

template <typename T>
bool TestSphere3::testCoordinateVectorConversion(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Coordinate/vector conversion, with '" << TypeNamer::name<T>() << "':";

	constexpr T angleThreshold = NumericT<T>::deg2rad(T(0.1));

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.99, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const T latitude = RandomT<T>::scalar(-NumericT<T>::pi_2(), NumericT<T>::pi_2());
			const T longitude = RandomT<T>::scalar(-NumericT<T>::pi(), NumericT<T>::pi());

			const VectorT3<T> coordinateVector = SphereT3<T>::coordinateToVector(latitude, longitude);

			if (!coordinateVector.isUnit())
			{
				scopedIteration.setInaccurate();
			}

			T resultingLatitude = NumericT<T>::minValue();
			T resultingLongitude = NumericT<T>::minValue();
			SphereT3<T>::vectorToCoordinate(coordinateVector, resultingLatitude, resultingLongitude);

			if (!NumericT<T>::angleIsEqual(latitude, resultingLatitude, angleThreshold))
			{
				scopedIteration.setInaccurate();
			}

			if (!NumericT<T>::angleIsEqual(longitude, resultingLongitude, angleThreshold))
			{
				if (!NumericT<T>::angleIsEqual(NumericT<T>::abs(latitude), NumericT<T>::pi_2()))
				{
					// we are not at the poles

					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestSphere3::testShortestDistance(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Shortest distance conversion, with '" << TypeNamer::name<T>() << "':";

	constexpr T angleThreshold = NumericT<T>::deg2rad(T(0.1));

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.99, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const T latitudeA = RandomT<T>::scalar(-NumericT<T>::pi_2(), NumericT<T>::pi_2());
			const T longitudeA = RandomT<T>::scalar(-NumericT<T>::pi(), NumericT<T>::pi());

			const T latitudeB = RandomT<T>::scalar(-NumericT<T>::pi_2(), NumericT<T>::pi_2());
			const T longitudeB = RandomT<T>::scalar(-NumericT<T>::pi(), NumericT<T>::pi());

			const T distance = SphereT3<T>::shortestDistance(latitudeA, longitudeA, latitudeB, longitudeB);

			const VectorT3<T> coordinateVectorA = SphereT3<T>::coordinateToVector(latitudeA, longitudeA);
			const VectorT3<T> coordinateVectorB = SphereT3<T>::coordinateToVector(latitudeB, longitudeB);

			const QuaternionT<T> quaternion(coordinateVectorA, coordinateVectorB);

			if (!NumericT<T>::angleIsEqual(distance, quaternion.angle(), angleThreshold))
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
