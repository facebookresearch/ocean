/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestLine3.h"

#include "ocean/base/Timestamp.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/ValidationPrecision.h"

#include "ocean/math/Line3.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestLine3::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Line3 test");

	Log::info() << " ";

	if (selector.shouldRun("isonline"))
	{
		testResult = testIsOnLine<float>(testDuration);
		Log::info() << " ";
		testResult = testIsOnLine<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("nearestpoints"))
	{
		testResult = testNearestPoints<float>(testDuration);
		Log::info() << " ";
		testResult = testNearestPoints<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("distance"))
	{
		testResult = testDistance<float>(testDuration);
		Log::info() << " ";
		testResult = testDistance<double>(testDuration);

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestLine3, IsOnLine_float)
{
	EXPECT_TRUE((TestLine3::testIsOnLine<float>(GTEST_TEST_DURATION)));
}

TEST(TestLine3, IsOnLine_double)
{
	EXPECT_TRUE((TestLine3::testIsOnLine<double>(GTEST_TEST_DURATION)));
}


TEST(TestLine3, NearestPoints_float)
{
	EXPECT_TRUE((TestLine3::testNearestPoints<float>(GTEST_TEST_DURATION)));
}

TEST(TestLine3, NearestPoints_double)
{
	EXPECT_TRUE((TestLine3::testNearestPoints<double>(GTEST_TEST_DURATION)));
}


TEST(TestLine3, Distance_float)
{
	EXPECT_TRUE((TestLine3::testDistance<float>(GTEST_TEST_DURATION)));
}

TEST(TestLine3, Distance_double)
{
	EXPECT_TRUE((TestLine3::testDistance<double>(GTEST_TEST_DURATION)));
}

#endif // OCEAN_USE_GTEST

template <typename T>
bool TestLine3::testIsOnLine(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "isOnLine test, with " << TypeNamer::name<T>() << ":";

	constexpr double successThreshold = 0.99;

	RandomGenerator randomGenerator;
	ValidationPrecision validation(successThreshold, randomGenerator);

	const T range = std::is_same<T, float>::value ? T(10) : T(1000);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const LineT3<T> line(RandomT<T>::vector3(randomGenerator, -range, range), RandomT<T>::vector3(randomGenerator));
			ocean_assert(NumericT<T>::isEqual(line.direction().length(), 1));

			VectorT3<T> perpendicular(line.direction().perpendicular());

			if (!perpendicular.normalize())
			{
				ocean_assert(false && "This must never happen!");
			}

			ocean_assert(line.direction() != perpendicular);
			ocean_assert(NumericT<T>::isEqual(perpendicular.length(), 1));
			ocean_assert(NumericT<T>::isEqualEps(line.direction() * perpendicular));

			const VectorT3<T> pointOnLine(line.point() + line.direction() * RandomT<T>::scalar(randomGenerator, -range * T(10), range * T(10)));

			if (line.isOnLine(pointOnLine) == false)
			{
				scopedIteration.setInaccurate();
			}

			const VectorT3<T> pointOffset(line.point() + perpendicular * RandomT<T>::scalar(randomGenerator, -range, range));

			if (line.point() != pointOffset && line.isOnLine(pointOffset) == true)
			{
				scopedIteration.setInaccurate();
			}

			const VectorT3<T> pointOffset2(pointOnLine + perpendicular * RandomT<T>::scalar(randomGenerator, T(0.5), range) * RandomT<T>::sign(randomGenerator));

			if (line.isOnLine(pointOffset2) == true)
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestLine3::testNearestPoints(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Nearest points test, with " << TypeNamer::name<T>() << ":";

	constexpr double successThreshold = 0.99;

	RandomGenerator randomGenerator;
	ValidationPrecision validation(successThreshold, randomGenerator);

	const T range = std::is_same<T, float>::value ? T(10) : T(1000);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const LineT3<T> lineA(RandomT<T>::vector3(randomGenerator, -range, range), RandomT<T>::vector3(randomGenerator));
			const LineT3<T> lineB(RandomT<T>::vector3(randomGenerator, -range, range), RandomT<T>::vector3(randomGenerator));

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
						if (NumericT<T>::isWeakEqualEps(lineA.direction() * direction) && NumericT<T>::isWeakEqualEps(lineB.direction() * direction))
						{
							const T bestDistance = pointA.distance(pointB);

							for (T offset : {T(-0.01), T(0.01)})
							{
								T checkDistance = (pointA + lineA.direction() * offset).distance(pointB);

								if (checkDistance < bestDistance)
								{
									scopedIteration.setInaccurate();
								}

								checkDistance = pointA.distance(pointB + lineB.direction() * offset);

								if (checkDistance < bestDistance)
								{
									scopedIteration.setInaccurate();
								}
							}
						}
						else
						{
							scopedIteration.setInaccurate();
						}
					}
					else
					{
						if (!pointA.isEqual(pointB, NumericT<T>::weakEps()))
						{
							scopedIteration.setInaccurate();
						}
					}
				}
				else
				{
					scopedIteration.setInaccurate();
				}
			}
			else
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestLine3::testDistance(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Distance between lines test, with " << TypeNamer::name<T>() << ":";

	constexpr double successThreshold = 0.99;

	RandomGenerator randomGenerator;
	ValidationPrecision validation(successThreshold, randomGenerator);

	const T range = std::is_same<T, float>::value ? T(10) : T(1000);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const LineT3<T> lineA(RandomT<T>::vector3(randomGenerator, -range, range), RandomT<T>::vector3(randomGenerator));
			const LineT3<T> lineB(RandomT<T>::vector3(randomGenerator, -range, range), RandomT<T>::vector3(randomGenerator));

			ocean_assert(lineA.direction().isUnit() && lineB.direction().isUnit());

			const T distance = lineA.distance(lineB);

			VectorT3<T> pointA;
			VectorT3<T> pointB;
			if (lineA.nearestPoints(lineB, pointA, pointB))
			{
				const T pointDistance = pointA.distance(pointB);

				if (!NumericT<T>::isWeakEqual(distance, pointDistance))
				{
					scopedIteration.setInaccurate();
				}
			}
			else
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
