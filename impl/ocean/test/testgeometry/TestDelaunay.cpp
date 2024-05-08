/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestDelaunay.h"

#include "ocean/base/HighPerformanceTimer.h"
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

bool TestDelaunay::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Delaunay test:   ---";

	bool allSucceeded = true;

	Log::info() << " ";

	testTriangulation(testDuration);

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Delaunay test succeeded.";
	}
	else
	{
		Log::info() << "Delaunay test FAILED!";
	}

	return allSucceeded;
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

	bool allSucceeded = true;

#ifdef OCEAN_MATH_USE_SINGLE_PRECISION
	for (const unsigned int numberPoints : {3u, 5u, 10u, 50u})
#else
	for (const unsigned int numberPoints : {3u, 5u, 10u, 50u, 100u, 1000u, 2000u})
#endif
	{
		Log::info() << " ";

		if (!testTriangulation(numberPoints, testDuration))
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Triangulation test succeeded.";
	}
	else
	{
		Log::info() << "Triangulation test FAILED!";
	}

	return allSucceeded;
}

bool TestDelaunay::testTriangulation(const unsigned int pointNumber, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with " << String::insertCharacter(String::toAString(pointNumber), ',', 3, false) << " points:";

	constexpr Scalar range = std::is_same<float, Scalar>::value ? Scalar(10) : Scalar(1000);

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
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

		if (Geometry::Delaunay::checkTriangulation(triangles, points))
		{
			++validIterations;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Performance: " << performance.averageMseconds() << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "%";

	constexpr double threshold = std::is_same<float, Scalar>::value ? 0.95 : 0.99;

	return percent >= threshold;
}

}

}

}
