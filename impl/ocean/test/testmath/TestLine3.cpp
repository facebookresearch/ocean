/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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

	allSucceeded = testIsOnLine<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testIsOnLine<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNearestPoints<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNearestPoints<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDistance<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testDistance<double>(testDuration) && allSucceeded;

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

TEST(TestLine3, IsOnLine_Float)
{
	EXPECT_TRUE((TestLine3::testIsOnLine<float>(GTEST_TEST_DURATION)));
}

TEST(TestLine3, IsOnLine_Double)
{
	EXPECT_TRUE((TestLine3::testIsOnLine<double>(GTEST_TEST_DURATION)));
}


TEST(TestLine3, NearestPoints_Float)
{
	EXPECT_TRUE((TestLine3::testNearestPoints<float>(GTEST_TEST_DURATION)));
}

TEST(TestLine3, NearestPoints_Double)
{
	EXPECT_TRUE((TestLine3::testNearestPoints<double>(GTEST_TEST_DURATION)));
}


TEST(TestLine3, Distance_Float)
{
	EXPECT_TRUE((TestLine3::testDistance<float>(GTEST_TEST_DURATION)));
}

TEST(TestLine3, Distance_Double)
{
	EXPECT_TRUE((TestLine3::testDistance<double>(GTEST_TEST_DURATION)));
}

#endif // OCEAN_USE_GTEST

template <typename T>
bool TestLine3::testIsOnLine(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "isOnLine test, with " << TypeNamer::name<T>() << ":";

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const T range = std::is_same<T, float>::value ? T(10) : T(1000);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const LineT3<T> line(RandomT<T>::vector3(-range, range), RandomT<T>::vector3());
			ocean_assert(NumericT<T>::isEqual(line.direction().length(), 1));

			VectorT3<T> perpendicular(line.direction().perpendicular());

			if (!perpendicular.normalize())
			{
				ocean_assert(false && "This must never happen!");
			}

			ocean_assert(line.direction() != perpendicular);
			ocean_assert(NumericT<T>::isEqual(perpendicular.length(), 1));
			ocean_assert(NumericT<T>::isEqualEps(line.direction() * perpendicular));

			bool localSucceeded = true;

			const VectorT3<T> pointOnLine(line.point() + line.direction() * RandomT<T>::scalar(-range * T(10), range * T(10)));

			if (line.isOnLine(pointOnLine) == false)
			{
				localSucceeded = false;
			}

			const VectorT3<T> pointOffset(line.point() + perpendicular * RandomT<T>::scalar(-range, range));

			if (line.point() != pointOffset && line.isOnLine(pointOffset) == true)
			{
				localSucceeded = false;
			}

			const VectorT3<T> pointOffset2(pointOnLine + perpendicular * RandomT<T>::scalar(T(0.5), range) * RandomT<T>::sign());

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

template <typename T>
bool TestLine3::testNearestPoints(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Nearest points test, with " << TypeNamer::name<T>() << ":";

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const T range = std::is_same<T, float>::value ? T(10) : T(1000);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const LineT3<T> lineA(RandomT<T>::vector3(-range, range), RandomT<T>::vector3());
			const LineT3<T> lineB(RandomT<T>::vector3(-range, range), RandomT<T>::vector3());

			ocean_assert(lineA.direction().isUnit() && lineB.direction().isUnit());

			VectorT3<T> pointA;
			VectorT3<T> pointB;
			if (lineA.nearestPoints(lineB, pointA, pointB))
			{
				if (lineA.isOnLine(pointA) && lineB.isOnLine(pointB))
				{
					VectorT3<T> direction(pointB - pointA);

					if (direction.normalize())
					{
						// the vector between both nearest points must be perpendicular to both lines
						if (NumericT<T>::isWeakEqualEps(lineA.direction() * direction) && NumericT<T>::isWeakEqualEps(lineB.direction() * direction))
						{
							// ensuring that we cannot find better points

							const T bestDistance = pointA.distance(pointB);

							bool allSucceeded = true;

							for (T offset : {T(-0.01), T(0.01)})
							{
								T checkDistance = (pointA + lineA.direction() * offset).distance(pointB);

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
						if (pointA.isEqual(pointB, NumericT<T>::weakEps()))
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

template <typename T>
bool TestLine3::testDistance(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Distance between lines test, with " << TypeNamer::name<T>() << ":";

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const T range = std::is_same<T, float>::value ? T(10) : T(1000);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const LineT3<T> lineA(RandomT<T>::vector3(-range, range), RandomT<T>::vector3());
			const LineT3<T> lineB(RandomT<T>::vector3(-range, range), RandomT<T>::vector3());

			ocean_assert(lineA.direction().isUnit() && lineB.direction().isUnit());

			const T distance = lineA.distance(lineB);

			VectorT3<T> pointA;
			VectorT3<T> pointB;
			if (lineA.nearestPoints(lineB, pointA, pointB))
			{
				const T pointDistance = pointA.distance(pointB);

				if (NumericT<T>::isWeakEqual(distance, pointDistance))
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
