/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestDelaunay.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"
#include "ocean/test/ValidationPrecision.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Timestamp.h"

#include "ocean/geometry/Delaunay.h"
#include "ocean/geometry/SpatialDistribution.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

bool TestDelaunay::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Delaunay test");

	Log::info() << " ";

	if (selector.shouldRun("triangulation"))
	{
		testResult = testTriangulation(testDuration);
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestDelaunay, Triangulation)
{
	EXPECT_TRUE(TestDelaunay::testTriangulation(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestDelaunay::testTriangulation(const double testDuration)
{
	Log::info() << "Test triangulation:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

#ifdef OCEAN_MATH_USE_SINGLE_PRECISION
	for (const unsigned int numberPoints : {3u, 5u, 10u, 50u})
#else
	for (const unsigned int numberPoints : {3u, 5u, 10u, 50u, 100u, 1000u, 2000u})
#endif
	{
		Log::info() << " ";

		OCEAN_EXPECT_TRUE(validation, testTriangulation(numberPoints, testDuration));
	}

	Log::info() << " ";

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestDelaunay::testTriangulation(const unsigned int pointNumber, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with " << String::insertCharacter(String::toAString(pointNumber), ',', 3, false) << " points:";

	constexpr Scalar range = std::is_same<float, Scalar>::value ? Scalar(10) : Scalar(1000);

	constexpr double threshold = std::is_same<float, Scalar>::value ? 0.95 : 0.99;

	RandomGenerator randomGenerator;
	ValidationPrecision validation(threshold, randomGenerator);

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		constexpr Scalar areaSize = range * Scalar(2);
		constexpr unsigned int bins = (unsigned int)(range * 10);

		Geometry::SpatialDistribution::OccupancyArray occupancyArray(-range, -range, areaSize, areaSize, bins, bins);

		Vectors2 points;
		points.reserve(pointNumber);

		while (points.size() < pointNumber)
		{
			const Vector2 candidate = Random::vector2(-range, range);

			if (!occupancyArray.isOccupiedNeighborhood9(candidate)) // let's ensure that we have some space between all points
			{
				occupancyArray.addPoint(candidate);

				points.push_back(candidate);
			}
		}

		performance.start();
			const Geometry::Delaunay::IndexTriangles triangles = Geometry::Delaunay::triangulation(points);
		performance.stop();

		if (!Geometry::Delaunay::checkTriangulation(triangles, points))
		{
			scopedIteration.setInaccurate();
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance: " << performance.averageMseconds() << "ms";
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
