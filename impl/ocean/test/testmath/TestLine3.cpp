// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testmath/TestLine3.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/Line3.h"
#include "ocean/math/Random.h"
#include "ocean/math/Rotation.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestLine3::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Line3 test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testIsOnLine(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNearestPoints(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDistance(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Line3 test succeeded.";
	}
	else
	{
		Log::info() << "Line3 test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestLine3, IsOnLine)
{
	EXPECT_TRUE(TestLine3::testIsOnLine(GTEST_TEST_DURATION));
}

TEST(TestLine3, NearestPoints)
{
	EXPECT_TRUE(TestLine3::testNearestPoints(GTEST_TEST_DURATION));
}

TEST(TestLine3, Distance)
{
	EXPECT_TRUE(TestLine3::testDistance(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestLine3::testIsOnLine(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "isOnLine test:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Scalar range = std::is_same<Scalar, float>::value ? Scalar(100) : Scalar(1000);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Line3 line(Random::vector3(-range, range), Random::vector3());
			ocean_assert(Numeric::isEqual(line.direction().length(), 1));

			Vector3 perpendicular(line.direction().perpendicular());

			if (!perpendicular.normalize())
			{
				ocean_assert(false && "This must never happen!");
			}

			ocean_assert(line.direction() != perpendicular);
			ocean_assert(Numeric::isEqual(perpendicular.length(), 1));
			ocean_assert(Numeric::isEqualEps(line.direction() * perpendicular));

			bool localSucceeded = true;

			const Vector3 pointOnLine(line.point() + line.direction() * Random::scalar(-range * Scalar(10), range * Scalar(10)));

			if (line.isOnLine(pointOnLine) == false)
			{
				localSucceeded = false;
			}

			const Vector3 pointOffset(line.point() + perpendicular * Random::scalar(-range, range));

			if (line.point() != pointOffset && line.isOnLine(pointOffset) == true)
			{
				localSucceeded = false;
			}

			const Vector3 pointOffset2(pointOnLine + perpendicular * Random::scalar(Scalar(0.5), range) * Random::sign());

			if (line.isOnLine(pointOffset2) == true)
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

bool TestLine3::testNearestPoints(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Nearest points test:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Scalar range = std::is_same<Scalar, float>::value ? Scalar(100) : Scalar(1000);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Line3 lineA(Random::vector3(-range, range), Random::vector3());
			const Line3 lineB(Random::vector3(-range, range), Random::vector3());

			ocean_assert(lineA.direction().isUnit() && lineB.direction().isUnit());

			Vector3 pointA;
			Vector3 pointB;
			if (lineA.nearestPoints(lineB, pointA, pointB))
			{
				if (lineA.isOnLine(pointA) && lineB.isOnLine(pointB))
				{
					Vector3 direction(pointB - pointA);

					if (direction.normalize())
					{
						// the vector between both nearest points must be perpendicular to both lines
						if (Numeric::isWeakEqualEps(lineA.direction() * direction) && Numeric::isWeakEqualEps(lineB.direction() * direction))
						{
							// ensuring that we cannot find better points

							const Scalar bestDistance = pointA.distance(pointB);

							bool allSucceeded = true;

							for (Scalar offset : {Scalar(-0.01), Scalar(0.01)})
							{
								Scalar checkDistance = (pointA + lineA.direction() * offset).distance(pointB);

								if (checkDistance < bestDistance)
								{
									allSucceeded = false;
								}

								checkDistance = pointA.distance(pointB + lineB.direction() * offset);

								if (checkDistance < bestDistance)
								{
									allSucceeded = false;
								}
							}

							if (allSucceeded)
							{
								++validIterations;
							}
						}
					}
					else
					{
						if (pointA.isEqual(pointB, Numeric::weakEps()))
						{
							++validIterations;
						}
					}
				}
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

bool TestLine3::testDistance(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Distance between lines test:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Scalar range = std::is_same<Scalar, float>::value ? Scalar(100) : Scalar(1000);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Line3 lineA(Random::vector3(-range, range), Random::vector3());
			const Line3 lineB(Random::vector3(-range, range), Random::vector3());

			ocean_assert(lineA.direction().isUnit() && lineB.direction().isUnit());

			const Scalar distance = lineA.distance(lineB);

			Vector3 pointA;
			Vector3 pointB;
			if (lineA.nearestPoints(lineB, pointA, pointB))
			{
				const Scalar pointDistance = pointA.distance(pointB);

				if (Numeric::isWeakEqual(distance, pointDistance))
				{
					++validIterations;
				}
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
