/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestFiniteLine2.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/FiniteLine2.h"
#include "ocean/math/Random.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestFiniteLine2::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("FiniteLine2 test");

	Log::info() << " ";

	if (selector.shouldRun("isonline"))
	{
		testResult = testIsOnLine(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("distance"))
	{
		testResult = testDistance(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("isleftofline"))
	{
		testResult = testIsLeftOfLine(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("iscollinear"))
	{
		testResult = testIsCollinear(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("normal"))
	{
		testResult = testNormal(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("isequal"))
	{
		testResult = testIsEqual<float>(testDuration);
		Log::info() << " ";
		testResult = testIsEqual<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("nearestpoint"))
	{
		testResult = testNearestPoint<float>(testDuration);
		Log::info() << " ";
		testResult = testNearestPoint<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("intersection"))
	{
		testResult = testIntersection<float>(testDuration);
		Log::info() << " ";
		testResult = testIntersection<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("nearestpointoninfiniteline"))
	{
		testResult = testNearestPointOnInfiniteLine<float>(testDuration);
		Log::info() << " ";
		testResult = testNearestPointOnInfiniteLine<double>(testDuration);

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestFiniteLine2, IsOnLine)
{
	EXPECT_TRUE(TestFiniteLine2::testIsOnLine(GTEST_TEST_DURATION));
}

TEST(TestFiniteLine2, Distance)
{
	EXPECT_TRUE(TestFiniteLine2::testDistance(GTEST_TEST_DURATION));
}

TEST(TestFiniteLine2, IsLeftOfLine)
{
	EXPECT_TRUE(TestFiniteLine2::testIsLeftOfLine(GTEST_TEST_DURATION));
}

TEST(TestFiniteLine2, IsCollinear)
{
	EXPECT_TRUE(TestFiniteLine2::testIsCollinear(GTEST_TEST_DURATION));
}

TEST(TestFiniteLine2, Normal)
{
	EXPECT_TRUE(TestFiniteLine2::testNormal(GTEST_TEST_DURATION));
}

TEST(TestFiniteLine2, IsEqualFloat)
{
	EXPECT_TRUE(TestFiniteLine2::testIsEqual<float>(GTEST_TEST_DURATION));
}

TEST(TestFiniteLine2, IsEqualDouble)
{
	EXPECT_TRUE(TestFiniteLine2::testIsEqual<double>(GTEST_TEST_DURATION));
}

TEST(TestFiniteLine2, NearestPointFloat)
{
	EXPECT_TRUE(TestFiniteLine2::testNearestPoint<float>(GTEST_TEST_DURATION));
}

TEST(TestFiniteLine2, NearestPointDouble)
{
	EXPECT_TRUE(TestFiniteLine2::testNearestPoint<double>(GTEST_TEST_DURATION));
}

TEST(TestFiniteLine2, IntersectionFloat)
{
	EXPECT_TRUE(TestFiniteLine2::testIntersection<float>(GTEST_TEST_DURATION));
}

TEST(TestFiniteLine2, IntersectionDouble)
{
	EXPECT_TRUE(TestFiniteLine2::testIntersection<double>(GTEST_TEST_DURATION));
}

TEST(TestFiniteLine2, NearestPointOnInfiniteLineFloat)
{
	EXPECT_TRUE(TestFiniteLine2::testNearestPointOnInfiniteLine<float>(GTEST_TEST_DURATION));
}

TEST(TestFiniteLine2, NearestPointOnInfiniteLineDouble)
{
	EXPECT_TRUE(TestFiniteLine2::testNearestPointOnInfiniteLine<double>(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestFiniteLine2::testIsOnLine(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "isOnLine test:";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(0.99, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Vector2 point0(Random::vector2(randomGenerator, -100, 100));
			Vector2 point1(Random::vector2(randomGenerator, -100, 100));

			while (point0 == point1)
			{
				point1 = Random::vector2(randomGenerator, -100, 100);
			}

			const FiniteLine2 line(point0, point1);

			{
				const Scalar scalar = Random::scalar(randomGenerator, -1, 2);
				const Vector2 point(point0 + (point1 - point0) * scalar);

				if (scalar >= 0 && scalar <= 1)
				{
					if (line.isOnLine(point) == false)
					{
						scopedIteration.setInaccurate();
					}
				}
				else if (scalar < -Numeric::weakEps() || scalar > 1 + Numeric::weakEps())
				{
					if (line.isOnLine(point) == true)
					{
						scopedIteration.setInaccurate();
					}
				}
				else
				{
					// we not not consider this case as the result is not uniquely defined
				}
			}

			{
				const Vector2 direction0(line.direction());
				const Vector2 direction1(direction0.perpendicular());

				ocean_assert(Numeric::isEqual(direction0.length(), 1));
				ocean_assert(Numeric::isEqual(direction1.length(), 1));

				const Scalar scalar0 = Random::scalar(randomGenerator, Scalar(0.01), 100);
				const Scalar scalar1 = Random::scalar(randomGenerator, Scalar(0.01), 100);

				const Vector2 pointA = point0 + direction0 * scalar0 + direction1 * scalar1;
				const Vector2 pointB = point0 - direction0 * scalar0 - direction1 * scalar1;

				if (line.isOnLine(pointA) || line.isOnLine(pointB))
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestFiniteLine2::testDistance(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Distance test:";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(0.99, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Vector2 point0(Random::vector2(randomGenerator, -100, 100));
			Vector2 point1(Random::vector2(randomGenerator, -100, 100));

			while (point0 == point1)
			{
				point1 = Random::vector2(randomGenerator, -100, 100);
			}

			const FiniteLine2 line(point0, point1);

			const Vector2 point(Random::vector2(randomGenerator, -100, 100));

			const Scalar result = line.distance(point);

			const Line2 infiniteLine(line.point0(), line.direction());

			const Vector2 pointOnInfiniteLine(infiniteLine.nearestPoint(point));
			ocean_assert(infiniteLine.isOnLine(pointOnInfiniteLine));

			Scalar test = Numeric::maxValue();

			if (line.isOnLine(pointOnInfiniteLine))
			{
				test = pointOnInfiniteLine.distance(point);
			}
			else
			{
				test = min(point.distance(point0), point.distance(point1));
			}

			if (!Numeric::isEqual(test, result, std::is_same<float, Scalar>::value ? Numeric::eps() * 100 : Numeric::eps()))
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestFiniteLine2::testIsLeftOfLine(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "IsLeftOfLine test:";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(0.99, randomGenerator);

	const Scalar range = std::is_same<Scalar, float>::value ? Scalar(100) : Scalar(1000);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Vector2 point0 = Vector2(Random::scalar(randomGenerator, -range, range), Random::scalar(randomGenerator, -range, range));
			const Vector2 point1 = Vector2(Random::scalar(randomGenerator, -range, range), Random::scalar(randomGenerator, -range, range));

			if ((point0 - point1).length() < Numeric::eps())
			{
				continue;
			}

			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const FiniteLine2 finiteLine(point0, point1);
			const Line2 line(finiteLine.point0(), finiteLine.direction());
			const Vector2 testPoint = Vector2(Random::scalar(randomGenerator, -range, range), Random::scalar(randomGenerator, -range, range));

			if (finiteLine.isLeftOfLine(testPoint) != line.isLeftOfLine(testPoint))
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestFiniteLine2::testIsCollinear(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "IsCollinear test:";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(0.99, randomGenerator);

	const Scalar range = std::is_same<Scalar, float>::value ? Scalar(10) : Scalar(1000);
	const Scalar distanceEpsilon = std::is_same<Scalar, float>::value ? Numeric::weakEps() : Numeric::eps();
	const Scalar directionEpsilon = std::is_same<Scalar, float>::value ? Numeric::weakEps() : Numeric::eps();

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Vector2 point0 = Vector2(Random::scalar(randomGenerator, -range, range), Random::scalar(randomGenerator, -range, range));
			const Vector2 point1 = Vector2(Random::scalar(randomGenerator, -range, range), Random::scalar(randomGenerator, -range, range));

			if ((point0 - point1).length() < distanceEpsilon)
			{
				continue;
			}

			const FiniteLine2 randomLine(point0, point1);

			ValidationPrecision::ScopedIteration scopedIteration(validation);

			// Case 1: every line segment must be collinear to itself.
			if (randomLine.isCollinear(randomLine, distanceEpsilon, directionEpsilon) == false)
			{
				scopedIteration.setInaccurate();
			}

			// Case 2: lines where the distance of the endpoints to the random line offsets < epsilon should be considered as collinear
			//
			// +epsilon     - - - - - - - - - - - - - - -
			//
			// random line --> --------------
			//                                  --------- <-- other line still considered as collinear since it is within the distance threshold, epsilon
			// -epsilon     - - - - - - - - - - - - - - -
			//                                  --------- <-- other line still NOT considered as collinear anymore
			//
			{
				const Scalar distanceOffset = Random::scalar(randomGenerator, Scalar(0.0), Scalar(range));
				const Vector2 endpoint0 = point0 + randomLine.direction() * Random::scalar(randomGenerator, -range, range) + Vector2(Random::scalar(randomGenerator, -distanceOffset, distanceOffset), Random::scalar(randomGenerator, -distanceOffset, distanceOffset));
				const Vector2 endpoint1 = point1 + randomLine.direction() * Random::scalar(randomGenerator, -range, range) + Vector2(Random::scalar(randomGenerator, -distanceOffset, distanceOffset), Random::scalar(randomGenerator, -distanceOffset, distanceOffset));

				if ((endpoint0 - endpoint1).length() < Numeric::eps())
				{
					continue;
				}

				const FiniteLine2 currentLine(endpoint0, endpoint1);
				const Scalar distanceCurrentLineToRandomLine = std::max(randomLine.distance(currentLine.point0()), randomLine.distance(currentLine.point1()));
				const Scalar distanceRandomLineToCurrentLine = std::max(currentLine.distance(randomLine.point0()), currentLine.distance(randomLine.point1()));
				const bool alignedDirections = Numeric::isEqual(currentLine.direction() * randomLine.direction(), Scalar(1), directionEpsilon);
				const bool isCollinearGroundtruth = alignedDirections && distanceCurrentLineToRandomLine < distanceEpsilon && distanceRandomLineToCurrentLine < distanceEpsilon;

				const bool isCollinear = randomLine.isCollinear(currentLine, distanceEpsilon, directionEpsilon);

				if (isCollinear != isCollinearGroundtruth)
				{
					scopedIteration.setInaccurate();
				}
			}

			// Case 3: Line has a different orientation (worst case: is orthogonal) but its endpoints within distance threshold, e.g.,
			//
			// +epsilon   - - - - - - - - - - - - - - -
			//
			//              -------------|-------- <-- random line
			//                           | <-- line orthogonal to the orthogonal line but within distance threshold, epsilon
			// -epsilon   - - - - - - - - - - - - - - -
			//
			{
				const Vector2 crossPoint = randomLine.point0() + randomLine.direction() * (randomLine.length() * Random::scalar(randomGenerator, Scalar(0), Scalar(1)));
				const Vector2 endpoint0 = crossPoint + randomLine.normal() * Random::scalar(randomGenerator, Scalar(0), Scalar(0.499));
				const Vector2 endpoint1 = crossPoint - randomLine.normal() * Random::scalar(randomGenerator, Scalar(0), Scalar(0.499));

				if ((endpoint0 - endpoint1).length() < Numeric::eps())
				{
					continue;
				}

				const FiniteLine2 currentLine(endpoint0, endpoint1);
				const Scalar distanceCurrentLineToRandomLine = std::max(randomLine.distance(currentLine.point0()), randomLine.distance(currentLine.point1()));
				const Scalar distanceRandomLineToCurrentLine = std::max(currentLine.distance(randomLine.point0()), currentLine.distance(randomLine.point1()));
				const bool alignedDirections = Numeric::isEqual(currentLine.direction() * randomLine.direction(), Scalar(1), directionEpsilon);
				const bool isCollinearGroundtruth = alignedDirections && distanceCurrentLineToRandomLine < distanceEpsilon && distanceRandomLineToCurrentLine < distanceEpsilon;

				const bool isCollinear = randomLine.isCollinear(currentLine, distanceEpsilon, directionEpsilon);

				if (isCollinear != isCollinearGroundtruth)
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestFiniteLine2::testNormal(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Normal test:";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(0.99, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Vector2 point0(Random::vector2(randomGenerator, -100, 100));
			Vector2 point1(Random::vector2(randomGenerator, -100, 100));

			while ((point0 - point1).isNull())
			{
				point1 = Random::vector2(randomGenerator, -100, 100);
			}

			const FiniteLine2 line(point0, point1);

			const Vector2 direction = line.direction();
			const Vector2 normal = line.normal();
			const Scalar dotProduct = direction * normal;
			const Scalar crossProduct = normal.cross(direction);

			if (!Numeric::isEqual(normal.length(), Scalar(1)) || !Numeric::isEqual(crossProduct, Scalar(1)) || !Numeric::isEqualEps(dotProduct))
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestFiniteLine2::testIsEqual(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "IsEqual test for '" << TypeNamer::name<T>() << "' precision:";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const VectorT2<T> point0(RandomT<T>::vector2(randomGenerator, -100, 100));
			VectorT2<T> point1(RandomT<T>::vector2(randomGenerator, -100, 100));

			while (point0.sqrDistance(point1) <= T(0.01))
			{
				point1 = RandomT<T>::vector2(randomGenerator, -100, 100);
			}

			const FiniteLineT2<T> line(point0, point1);
			ocean_assert(line.isValid());

			const T epsilon = RandomT<T>::scalar(randomGenerator, NumericT<T>::eps() * 100, 10);

			// we crate a similar line to ensure that isEqual() is identifying the line as equal

			const VectorT2<T> similarOffset0 = RandomT<T>::vector2(randomGenerator) * (RandomT<T>::scalar(randomGenerator, 0, epsilon - NumericT<T>::eps() * 10) * RandomT<T>::sign(randomGenerator));
			const VectorT2<T> similarOffset1 = RandomT<T>::vector2(randomGenerator) * (RandomT<T>::scalar(randomGenerator, 0, epsilon - NumericT<T>::eps() * 10) * RandomT<T>::sign(randomGenerator));

			const VectorT2<T> similarPoint0 = point0 + similarOffset0;
			const VectorT2<T> similarPoint1 = point1 + similarOffset1;

			const FiniteLineT2<T> similarLineA(similarPoint0, similarPoint1);
			const FiniteLineT2<T> similarLineB(similarPoint1, similarPoint0);

			OCEAN_EXPECT_TRUE(validation, line.isEqual(similarLineA, epsilon));
			OCEAN_EXPECT_TRUE(validation, line.isEqual(similarLineB, epsilon));

			// now we create a different line to ensure that isEqual() is identifying the line as not equal

			const VectorT2<T> differentOffset0 = RandomT<T>::vector2(randomGenerator) * (RandomT<T>::scalar(randomGenerator, epsilon + NumericT<T>::eps() * 10, 100) * RandomT<T>::sign(randomGenerator));
			const VectorT2<T> differentOffset1 = RandomT<T>::vector2(randomGenerator) * (RandomT<T>::scalar(randomGenerator, epsilon + NumericT<T>::eps() * 10, 100) * RandomT<T>::sign(randomGenerator));

			const VectorT2<T> differentPoint0 = point0 + differentOffset0;
			const VectorT2<T> differentPoint1 = point1 + differentOffset1;

			const FiniteLineT2<T> differentLineA(differentPoint0, point1);
			const FiniteLineT2<T> differentLineB(point0, differentPoint1);

			if (line.point0().distance(line.point1()) >= epsilon + NumericT<T>::eps() * 100)
			{
				OCEAN_EXPECT_FALSE(validation, line.isEqual(differentLineA, epsilon));
				OCEAN_EXPECT_FALSE(validation, line.isEqual(differentLineB, epsilon));
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestFiniteLine2::testNearestPoint(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "NearestPoint test for '" << TypeNamer::name<T>() << "' precision:";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const VectorT2<T> linePoint0(RandomT<T>::vector2(randomGenerator, -100, 100));
			VectorT2<T> linePoint1(RandomT<T>::vector2(randomGenerator, -100, 100));

			while (linePoint0.sqrDistance(linePoint1) <= T(0.01))
			{
				linePoint1 = RandomT<T>::vector2(randomGenerator, -100, 100);
			}

			const FiniteLineT2<T> line(linePoint0, linePoint1);
			ocean_assert(line.isValid());

			// testing some fixed points

			OCEAN_EXPECT_TRUE(validation, linePoint0.isEqual(line.nearestPoint(linePoint0), NumericT<T>::weakEps()));
			OCEAN_EXPECT_TRUE(validation, linePoint1.isEqual(line.nearestPoint(linePoint1), NumericT<T>::weakEps()));

			const VectorT2<T> lineCenterPoint = (linePoint0 + linePoint1) * T(0.5);
			OCEAN_EXPECT_TRUE(validation, lineCenterPoint.isEqual(line.nearestPoint(lineCenterPoint), NumericT<T>::weakEps()));

			const VectorT2<T> anyPointOnLine = linePoint0 + line.direction() * RandomT<T>::scalar(randomGenerator, 0, line.length());
			OCEAN_EXPECT_TRUE(validation, anyPointOnLine.isEqual(line.nearestPoint(anyPointOnLine + line.normal() * RandomT<T>::scalar(randomGenerator, 0, 1)), NumericT<T>::weakEps()));

			// testing some random points

			const VectorT2<T> point(RandomT<T>::vector2(randomGenerator, -100, 100));
			const VectorT2<T> direction = point - line.point0();

			ocean_assert(NumericT<T>::isEqual(line.direction().length(), 1));
			const T length = direction * line.direction();

			const VectorT2<T> nearestPoint = line.nearestPoint(point);

			if (length <= 0)
			{
				// the projected point lies outside the finite line

				OCEAN_EXPECT_TRUE(validation, nearestPoint.isEqual(line.point0(), NumericT<T>::weakEps()));
			}
			else if (length >= line.length())
			{
				// the projected point lies outside the finite line

				OCEAN_EXPECT_TRUE(validation, nearestPoint.isEqual(line.point1(), NumericT<T>::weakEps()));
			}
			else
			{
				// the projected point lies within the finite line

				const VectorT2<T> pointOnInfiniteLine = line.point0() + line.direction() * length;

				OCEAN_EXPECT_TRUE(validation, nearestPoint.isEqual(pointOnInfiniteLine, NumericT<T>::weakEps()));
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestFiniteLine2::testIntersection(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Intersection test for '" << TypeNamer::name<T>() << "' precision:";

	const T area = std::is_same<double, T>::value ? 100.0 : T(10);

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			{
				// ensure that two parallel lines do not have an intersection

				const VectorT2<T> lineAPoint0(RandomT<T>::vector2(randomGenerator, -area, area));
				VectorT2<T> lineAPoint1(RandomT<T>::vector2(randomGenerator, -area, area));

				while (lineAPoint0.sqrDistance(lineAPoint1) <= T(0.01))
				{
					lineAPoint1 = RandomT<T>::vector2(randomGenerator, -area, area);
				}

				const FiniteLineT2<T> lineA(lineAPoint0, lineAPoint1);
				ocean_assert(lineA.isValid());

				const VectorT2<T> lineDirection = (lineAPoint1 - lineAPoint0).normalized();

				const VectorT2<T> pointOnLineA = lineAPoint0 + lineDirection * RandomT<T>::scalar(randomGenerator, -area, area);
				ocean_assert(lineA.isOnInfiniteLine(pointOnLineA));

				const VectorT2<T> pointNotOnLineA = pointOnLineA + lineDirection.perpendicular() * RandomT<T>::scalar(randomGenerator, T(0.01), area) * RandomT<T>::sign(randomGenerator);
				ocean_assert(!lineA.isOnInfiniteLine(pointNotOnLineA));

				const VectorT2<T>& lineBPoint0 = pointNotOnLineA;
				const VectorT2<T> lineBPoint1 = lineBPoint0 + lineDirection * RandomT<T>::scalar(randomGenerator, T(0.01), area) * RandomT<T>::sign(randomGenerator);

				// testing intersection with finite line

				const FiniteLineT2<T> lineB(lineBPoint0, lineBPoint1);

				OCEAN_EXPECT_FALSE(validation, lineA.intersects(lineB));

				VectorT2<T> intersectionPoint;
				OCEAN_EXPECT_FALSE(validation, lineA.intersection(lineB, intersectionPoint));

				// testing intersection with infinite line

				const LineT2<T> infiniteLineB(lineB.point0(), lineB.direction());

				OCEAN_EXPECT_FALSE(validation, lineA.intersects(infiniteLineB));
				OCEAN_EXPECT_FALSE(validation, lineA.intersection(infiniteLineB, intersectionPoint));
			}

			{
				// ensure that two lines crossing each other have an intersection

				const VectorT2<T> lineAPoint0(RandomT<T>::vector2(randomGenerator, -area, area));
				VectorT2<T> lineAPoint1(RandomT<T>::vector2(randomGenerator, -area, area));

				while (lineAPoint0.sqrDistance(lineAPoint1) <= T(0.01))
				{
					lineAPoint1 = RandomT<T>::vector2(randomGenerator, -area, area);
				}

				const FiniteLineT2<T> lineA(lineAPoint0, lineAPoint1);
				ocean_assert(lineA.isValid());


				VectorT2<T> lineBDirection(RandomT<T>::vector2(randomGenerator));
				while (NumericT<T>::abs(lineBDirection * lineA.direction()) >= NumericT<T>::deg2rad(T(0.5)))
				{
					lineBDirection = RandomT<T>::vector2(randomGenerator);
				}
				ocean_assert(!lineA.direction().isParallel(lineBDirection));

				// defining a crossing point
				const T locationOnInfiniteLine = RandomT<T>::scalar(randomGenerator, -1, 2);
				const VectorT2<T> pointOnInfiniteLine = lineA.point0() + (lineA.point1() - lineA.point0()) * locationOnInfiniteLine;

				const VectorT2<T> lineBPoint0 = pointOnInfiniteLine + lineBDirection * RandomT<T>::scalar(randomGenerator, -1, T(-0.01));
				const VectorT2<T> lineBPoint1 = lineBPoint0 + lineBDirection * RandomT<T>::scalar(randomGenerator, T(1.01), 2);

				const FiniteLineT2<T> lineB(lineBPoint0, lineBPoint1);
				ocean_assert(!lineA.isParallel(lineB));


				if (locationOnInfiniteLine >= T(0) && locationOnInfiniteLine <= T(1))
				{
					// we actually have an intersection, as the intersection point lies between point0 and point1

					OCEAN_EXPECT_TRUE(validation, lineA.intersects(lineB));

					VectorT2<T> intersectionPoint;
					if (lineA.intersection(lineB, intersectionPoint))
					{
						OCEAN_EXPECT_TRUE(validation, intersectionPoint.isEqual(pointOnInfiniteLine, T(0.001)));
					}
					else
					{
						OCEAN_SET_FAILED(validation);
					}

					// testing intersection with infinite line

					const LineT2<T> infiniteLineB(lineB.point0(), lineB.direction());

					OCEAN_EXPECT_TRUE(validation, lineA.intersects(infiniteLineB));

					if (lineA.intersection(infiniteLineB, intersectionPoint))
					{
						OCEAN_EXPECT_TRUE(validation, intersectionPoint.isEqual(pointOnInfiniteLine, T(0.001)));
					}
					else
					{
						OCEAN_SET_FAILED(validation);
					}
				}
				else
				{
					// we do not have an intersection, as the intersection point lies outside point0 or point1

					OCEAN_EXPECT_FALSE(validation, lineA.intersects(lineB));

					VectorT2<T> intersectionPoint;
					OCEAN_EXPECT_FALSE(validation, lineA.intersection(lineB, intersectionPoint));

					// testing intersection with infinite line

					const LineT2<T> infiniteLineB(lineB.point0(), lineB.direction());

					OCEAN_EXPECT_FALSE(validation, lineA.intersects(infiniteLineB));
					OCEAN_EXPECT_FALSE(validation, lineA.intersection(infiniteLineB, intersectionPoint));
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestFiniteLine2::testNearestPointOnInfiniteLine(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "NearestPointOnInfiniteLine test for '" << TypeNamer::name<T>() << "' precision:";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const VectorT2<T> linePoint0(RandomT<T>::vector2(randomGenerator, -100, 100));
			VectorT2<T> linePoint1(RandomT<T>::vector2(randomGenerator, -100, 100));

			while (linePoint0.sqrDistance(linePoint1) <= T(0.01))
			{
				linePoint1 = RandomT<T>::vector2(randomGenerator, -100, 100);
			}

			const FiniteLineT2<T> line(linePoint0, linePoint1);
			ocean_assert(line.isValid());

			T outOfBoundaryDistance = T(0);
			T* useOutOfBoundaryDistance = RandomI::random(randomGenerator, 1u) % 2u == 0u ? &outOfBoundaryDistance : nullptr;

			T finiteLineLocation = T(0);
			T* useFiniteLineLocation = RandomI::random(randomGenerator, 1u) % 2u == 0u ? &finiteLineLocation : nullptr;

			// testing some fixed points

			// line.point0()
			OCEAN_EXPECT_TRUE(validation, linePoint0.isEqual(line.nearestPointOnInfiniteLine(linePoint0, useOutOfBoundaryDistance, useFiniteLineLocation), NumericT<T>::weakEps()));

			if (useOutOfBoundaryDistance)
			{
				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isWeakEqualEps(*useOutOfBoundaryDistance));
			}
			if (useFiniteLineLocation)
			{
				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(*useFiniteLineLocation, 0, NumericT<T>::weakEps()));
			}

			// line.point1()
			OCEAN_EXPECT_TRUE(validation, linePoint1.isEqual(line.nearestPointOnInfiniteLine(linePoint1, useOutOfBoundaryDistance, useFiniteLineLocation), NumericT<T>::weakEps()));

			if (useOutOfBoundaryDistance)
			{
				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isWeakEqualEps(*useOutOfBoundaryDistance));
			}
			if (useFiniteLineLocation)
			{
				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(*useFiniteLineLocation, line.length(), NumericT<T>::weakEps()));
			}

			// (line.point0() + line.point1()) / 2
			const VectorT2<T> lineCenterPoint = (linePoint0 + linePoint1) * T(0.5);
			OCEAN_EXPECT_TRUE(validation, lineCenterPoint.isEqual(line.nearestPointOnInfiniteLine(lineCenterPoint, useOutOfBoundaryDistance, useFiniteLineLocation), NumericT<T>::weakEps()));

			if (useOutOfBoundaryDistance)
			{
				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isWeakEqualEps(*useOutOfBoundaryDistance));
			}
			if (useFiniteLineLocation)
			{
				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(*useFiniteLineLocation, line.length() * T(0.5), NumericT<T>::weakEps()));
			}

			// any point between line.point0() and line.point1()
			const T distance = RandomT<T>::scalar(randomGenerator, 0, line.length());
			const VectorT2<T> anyPointOnLine = linePoint0 + line.direction() * distance;
			OCEAN_EXPECT_TRUE(validation, anyPointOnLine.isEqual(line.nearestPointOnInfiniteLine(anyPointOnLine + line.normal() * RandomT<T>::scalar(randomGenerator, 0, 1), useOutOfBoundaryDistance, useFiniteLineLocation), NumericT<T>::weakEps()));

			if (useOutOfBoundaryDistance)
			{
				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isWeakEqualEps(*useOutOfBoundaryDistance));
			}
			if (useFiniteLineLocation)
			{
				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(*useFiniteLineLocation, distance, NumericT<T>::weakEps()));
			}

			// testing some random points

			const VectorT2<T> point(RandomT<T>::vector2(randomGenerator, -100, 100));
			const VectorT2<T> direction = point - line.point0();

			ocean_assert(NumericT<T>::isEqual(line.direction().length(), 1));
			const T length = direction * line.direction();

			const VectorT2<T> nearestPoint = line.nearestPointOnInfiniteLine(point, useOutOfBoundaryDistance, useFiniteLineLocation);

			if (length <= 0)
			{
				// the projected point lies outside the finite line

				const VectorT2<T> directionToNearestPoint(nearestPoint - line.point1());

				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(line.direction().angle(directionToNearestPoint), NumericT<T>::pi(), NumericT<T>::deg2rad(T(0.5))));

				if (useOutOfBoundaryDistance)
				{
					OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(length, *useOutOfBoundaryDistance, NumericT<T>::weakEps()));
				}
			}
			else if (length >= line.length())
			{
				// the projected point lies outside the finite line

				const VectorT2<T> directionToNearestPoint(nearestPoint - line.point0());

				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(line.direction().angle(directionToNearestPoint), 0, NumericT<T>::deg2rad(T(0.5))));

				if (useOutOfBoundaryDistance)
				{
					OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(length - line.length(), *useOutOfBoundaryDistance, NumericT<T>::weakEps()));
				}
			}
			else
			{
				if (useOutOfBoundaryDistance)
				{
					OCEAN_EXPECT_TRUE(validation, NumericT<T>::isWeakEqualEps(*useOutOfBoundaryDistance));
				}
			}

			if (useFiniteLineLocation)
			{
				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(*useFiniteLineLocation, length, NumericT<T>::weakEps()));
			}

			const VectorT2<T> pointOnInfiniteLine = line.point0() + line.direction() * length;

			OCEAN_EXPECT_TRUE(validation, nearestPoint.isEqual(pointOnInfiniteLine, NumericT<T>::weakEps()));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
