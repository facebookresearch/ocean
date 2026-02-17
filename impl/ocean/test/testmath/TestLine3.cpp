/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestLine3.h"

#include "ocean/base/HighPerformanceTimer.h"
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
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("fitlineleastsquare"))
	{
		testResult = testFitLineLeastSquare<float>(testDuration);
		Log::info() << " ";
		testResult = testFitLineLeastSquare<double>(testDuration);

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

TEST(TestLine3, FitLineLeastSquare_float)
{
	EXPECT_TRUE((TestLine3::testFitLineLeastSquare<float>(GTEST_TEST_DURATION)));
}

TEST(TestLine3, FitLineLeastSquare_double)
{
	EXPECT_TRUE((TestLine3::testFitLineLeastSquare<double>(GTEST_TEST_DURATION)));
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

			const VectorT3<T> linePoint = RandomT<T>::vector3(randomGenerator, -range, range);
			const VectorT3<T> lineDirection = RandomT<T>::vector3(randomGenerator);

			const LineT3<T> line(linePoint, lineDirection);
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

			const T perpendicularDistance = RandomT<T>::scalar(randomGenerator, T(0.5), range);
			const T sign = RandomT<T>::sign(randomGenerator);

			const VectorT3<T> pointOffset2(pointOnLine + perpendicular * perpendicularDistance * sign);

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

			const VectorT3<T> linePointA = RandomT<T>::vector3(randomGenerator, -range, range);
			const VectorT3<T> lineDirectionA = RandomT<T>::vector3(randomGenerator);

			const VectorT3<T> linePointB = RandomT<T>::vector3(randomGenerator, -range, range);
			const VectorT3<T> lineDirectionB = RandomT<T>::vector3(randomGenerator);

			const LineT3<T> lineA(linePointA, lineDirectionA);
			const LineT3<T> lineB(linePointB, lineDirectionB);

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

			const VectorT3<T> linePointA = RandomT<T>::vector3(randomGenerator, -range, range);
			const VectorT3<T> lineDirectionA = RandomT<T>::vector3(randomGenerator);

			const VectorT3<T> linePointB = RandomT<T>::vector3(randomGenerator, -range, range);
			const VectorT3<T> lineDirectionB = RandomT<T>::vector3(randomGenerator);

			const LineT3<T> lineA(linePointA, lineDirectionA);
			const LineT3<T> lineB(linePointB, lineDirectionB);

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

template <typename T>
bool TestLine3::testFitLineLeastSquare(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "fitLineLeastSquare test, with " << TypeNamer::name<T>() << ":";

	constexpr double successThreshold = 0.99;

	RandomGenerator randomGenerator;
	ValidationPrecision validation(successThreshold, randomGenerator);

	HighPerformanceStatistic performance;

	const T range = std::is_same<T, float>::value ? T(100) : T(1000);

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool performanceIteration : {true, false})
		{
			{
				// testing with points exactly on a line (no noise)

				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const VectorT3<T> linePoint = RandomT<T>::vector3(randomGenerator, -range, range);
				const VectorT3<T> lineDirection = RandomT<T>::vector3(randomGenerator);
				ocean_assert(lineDirection.isUnit());

				const LineT3<T> groundTruthLine(linePoint, lineDirection);
				ocean_assert(groundTruthLine.isValid());

				const size_t numberPoints = performanceIteration ? 100 : size_t(RandomI::random(randomGenerator, 2u, 100u));

				std::vector<VectorT3<T>> points;
				points.reserve(numberPoints);

				for (size_t i = 0; i < numberPoints; ++i)
				{
					const T distance = RandomT<T>::scalar(randomGenerator, -range, range);
					points.emplace_back(groundTruthLine.point(distance));
				}

				LineT3<T> fittedLine;

				performance.startIf(performanceIteration);
					const bool result = LineT3<T>::fitLineLeastSquare(points.data(), points.size(), fittedLine);
				performance.stopIf(performanceIteration);

				if (!result)
				{
					scopedIteration.setInaccurate();
					continue;
				}

				ocean_assert(fittedLine.isValid());

				const T maxDistanceThreshold = std::is_same<T, float>::value ? T(0.01) : T(0.0001);

				for (const VectorT3<T>& point : points)
				{
					if (fittedLine.distance(point) > maxDistanceThreshold)
					{
						scopedIteration.setInaccurate();
						break;
					}
				}
			}

			{
				// testing with points with small perpendicular noise

				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const VectorT3<T> linePoint = RandomT<T>::vector3(randomGenerator, -range, range);
				const VectorT3<T> lineDirection = RandomT<T>::vector3(randomGenerator);
				ocean_assert(lineDirection.isUnit());

				const LineT3<T> groundTruthLine(linePoint, lineDirection);
				ocean_assert(groundTruthLine.isValid());

				const VectorT3<T> perpendicular0 = lineDirection.perpendicular().normalized();
				const VectorT3<T> perpendicular1 = lineDirection.cross(perpendicular0).normalized();

				const size_t numberPoints = performanceIteration ? 100 : size_t(RandomI::random(randomGenerator, 2u, 100u));

				const T maxNoise = std::is_same<T, float>::value ? T(0.01) : T(0.001);

				std::vector<VectorT3<T>> points;
				points.reserve(numberPoints);

				for (size_t i = 0; i < numberPoints; ++i)
				{
					const T distance = RandomT<T>::scalar(randomGenerator, -range, range);
					const T noise0 = RandomT<T>::scalar(randomGenerator, -maxNoise, maxNoise);
					const T noise1 = RandomT<T>::scalar(randomGenerator, -maxNoise, maxNoise);
					points.emplace_back(groundTruthLine.point(distance) + perpendicular0 * noise0 + perpendicular1 * noise1);
				}

				LineT3<T> fittedLine;

				performance.startIf(performanceIteration);
					const bool result = LineT3<T>::fitLineLeastSquare(points.data(), points.size(), fittedLine);
				performance.stopIf(performanceIteration);

				if (!result)
				{
					scopedIteration.setInaccurate();
					continue;
				}

				ocean_assert(fittedLine.isValid());

				T sqrDistanceGroundTruth = 0;
				T sqrDistanceFitted = 0;

				for (const VectorT3<T>& point : points)
				{
					sqrDistanceGroundTruth += groundTruthLine.sqrDistance(point);
					sqrDistanceFitted += fittedLine.sqrDistance(point);
				}

				if (sqrDistanceFitted > sqrDistanceGroundTruth)
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
