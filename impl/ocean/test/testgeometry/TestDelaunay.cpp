// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testgeometry/TestDelaunay.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/geometry/Delaunay.h"

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
		Log::info() << "Delaunay test succeeded.";
	else
		Log::info() << "Delaunay test FAILED!";

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

	const unsigned int numberPoints[] = {3u, 5u, 10u, 50u, 100u, 1000u, 2000u};

	bool allSucceeded = true;

	Log::info() << " ";

	for (size_t n = 0; n < sizeof(numberPoints) / sizeof(numberPoints[0]); ++n)
	{
		Log::info().newLine(n != 0);

		allSucceeded = testTriangulation(numberPoints[n], testDuration) && allSucceeded;
	}

	Log::info() << " ";

	if (allSucceeded)
		Log::info() << "Triangulation test succeeded.";
	else
		Log::info() << "Triangulation test FAILED!";

	return allSucceeded;
}

bool TestDelaunay::testTriangulation(const unsigned int pointNumber, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with " << String::insertCharacter(String::toAString(pointNumber), ',', 3, false) << " points:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	Vectors2 points(pointNumber);

	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < pointNumber; ++n)
			points[n] = Vector2(Random::scalar(-100, 100), Random::scalar(-100, 100));

		performance.start();
		const Geometry::Delaunay::IndexTriangles triangles = Geometry::Delaunay::triangulation(points);
		performance.stop();

		if (Geometry::Delaunay::checkTriangulation(triangles, points))
			validIterations++;

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Performance: " << performance.averageMseconds() << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "%";

	if (std::is_same<float, Scalar>::value)
		return percent >= 0.95;

	return percent >= 0.99;
}

}

}

}
