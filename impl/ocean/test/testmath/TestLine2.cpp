/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestLine2.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/Line2.h"
#include "ocean/math/Random.h"
#include "ocean/math/Rotation.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestLine2::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Line2 test");

	Log::info() << " ";

	if (selector.shouldRun("isonline"))
	{
		testResult = testIsOnLine(testDuration);

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

	if (selector.shouldRun("decomposenormaldistance"))
	{
		testResult = testDecomposeNormalDistance(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("fitlineleastsquare"))
	{
		testResult = testFitLineLeastSquare(testDuration);

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestLine2, IsOnLine)
{
	EXPECT_TRUE(TestLine2::testIsOnLine(GTEST_TEST_DURATION));
}

TEST(TestLine2, IsLeftOfLine)
{
	EXPECT_TRUE(TestLine2::testIsLeftOfLine(GTEST_TEST_DURATION));
}

TEST(TestLine2, DecomposeNormalDistance)
{
	EXPECT_TRUE(TestLine2::testDecomposeNormalDistance(GTEST_TEST_DURATION));
}

TEST(TestLine2, FitLineLeastSquare)
{
	EXPECT_TRUE(TestLine2::testFitLineLeastSquare(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestLine2::testIsOnLine(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "isOnLine test:";

	constexpr double successThreshold = 0.99;

	RandomGenerator randomGenerator;
	ValidationPrecision validation(successThreshold, randomGenerator);

	const Scalar range = std::is_same<Scalar, float>::value ? Scalar(100) : Scalar(1000);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Vector2 point = Random::vector2(randomGenerator, -range, range);
			const Vector2 direction = Random::vector2(randomGenerator);

			const Line2 line(point, direction);
			ocean_assert(Numeric::isEqual(line.direction().length(), 1));

			Vector2 perpendicular(line.direction().perpendicular());

			if (!perpendicular.normalize())
			{
				ocean_assert(false && "This must never happen!");
			}

			ocean_assert(line.direction() != perpendicular);
			ocean_assert(Numeric::isEqual(perpendicular.length(), 1));
			ocean_assert(Numeric::isEqualEps(line.direction() * perpendicular));

			const Vector2 pointOnLine(line.point() + line.direction() * Random::scalar(randomGenerator, -range * Scalar(10), range * Scalar(10)));

			if (line.isOnLine(pointOnLine) == false)
			{
				scopedIteration.setInaccurate();
			}

			const Vector2 pointOffset(line.point() + perpendicular * Random::scalar(randomGenerator, -range, range));

			if (line.point() != pointOffset && line.isOnLine(pointOffset) == true)
			{
				scopedIteration.setInaccurate();
			}

			const Vector2 pointOffset2(pointOnLine + perpendicular * Random::scalar(randomGenerator, Scalar(0.5), range) * Random::sign(randomGenerator));

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

bool TestLine2::testIsLeftOfLine(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "isLeftOfLine test:";

	constexpr double successThreshold = 0.99;

	RandomGenerator randomGenerator;
	ValidationPrecision validation(successThreshold, randomGenerator);

	const Scalar range = std::is_same<Scalar, float>::value ? Scalar(100) : Scalar(1000);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Line2 referenceLine(Vector2(0, 0), Vector2(0, Random::scalar(randomGenerator, 1, range)));
			ocean_assert(referenceLine.direction().length() > Numeric::eps());

			const Vector2 pointLeftOfReferenceLine(Random::scalar(randomGenerator, -range, Scalar(-0.1)), Random::scalar(randomGenerator, -range, range));
			const Vector2 pointRightOfReferenceLine(-pointLeftOfReferenceLine.x(), pointLeftOfReferenceLine.y());
			const Vector2 pointOnReferenceLine(referenceLine.point() + referenceLine.direction().normalized() * pointLeftOfReferenceLine.y());

			if (referenceLine.isLeftOfLine(pointLeftOfReferenceLine) != true)
			{
				scopedIteration.setInaccurate();
			}

			if (referenceLine.isLeftOfLine(pointRightOfReferenceLine) != false)
			{
				scopedIteration.setInaccurate();
			}

			if (referenceLine.isLeftOfLine(pointOnReferenceLine) != false)
			{
				scopedIteration.setInaccurate();
			}

			const SquareMatrix3 randomRotation = SquareMatrix3(Rotation(0, 0, 1, Random::scalar(randomGenerator, -Numeric::pi(), Numeric::pi())));
			const SquareMatrix3 randomTranslation = SquareMatrix3(1, 0, 0, 0, 1, 0, Random::scalar(randomGenerator, -range, range), Random::scalar(randomGenerator, -range, range), 1);
			const SquareMatrix3 randomTransformation = randomTranslation * randomRotation;

			const Line2 transformedLine = Line2(randomTranslation * referenceLine.point(), randomRotation * referenceLine.direction());
			ocean_assert(transformedLine.direction().length() > Numeric::eps());

			const Vector2 pointLeftOfTransformedLine = randomTransformation * pointLeftOfReferenceLine;
			const Vector2 pointRightOfTransformedLine = randomTransformation * pointRightOfReferenceLine;
			const Vector2 pointOnTransformedLine = randomTransformation * pointOnReferenceLine;

			if (transformedLine.isLeftOfLine(pointLeftOfTransformedLine) != true)
			{
				scopedIteration.setInaccurate();
			}

			if (transformedLine.isLeftOfLine(pointRightOfTransformedLine) != false)
			{
				scopedIteration.setInaccurate();
			}

			if (transformedLine.isLeftOfLine(pointOnTransformedLine) != false)
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestLine2::testDecomposeNormalDistance(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "decomposeNormalDistance test:";

	constexpr double successThreshold = 0.99;

	RandomGenerator randomGenerator;
	ValidationPrecision validation(successThreshold, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		const Vector2 linePoint(Random::vector2(randomGenerator, -10, 10));
		const Vector2 lineDirection(Random::vector2(randomGenerator));
		ocean_assert(lineDirection.isUnit());

		const Line2 line(linePoint, lineDirection);
		ocean_assert(line.isValid());

		const bool forcePositiveDistanceValue = RandomI::random(randomGenerator, 1u) == 0u;

		const Vector3 implicitLine = line.decomposeNormalDistance(forcePositiveDistanceValue);

		if (Numeric::isNotEqualEps(implicitLine * Vector3(linePoint, 1)))
		{
			scopedIteration.setInaccurate();
		}

		if (Numeric::isNotEqualEps(implicitLine * Vector3(linePoint + lineDirection, 1)))
		{
			scopedIteration.setInaccurate();
		}

		if (Numeric::isNotEqualEps(implicitLine * Vector3(linePoint - lineDirection, 1)))
		{
			scopedIteration.setInaccurate();
		}

		if (forcePositiveDistanceValue && implicitLine.z() < 0)
		{
			scopedIteration.setInaccurate();
		}

		const Line2 newLine(implicitLine);
		ocean_assert(newLine.isValid());

		if (Numeric::isNotWeakEqualEps(newLine.distance(linePoint)))
		{
			scopedIteration.setInaccurate();
		}

		if (Numeric::isNotWeakEqualEps(newLine.distance(linePoint + lineDirection)))
		{
			scopedIteration.setInaccurate();
		}

		if (Numeric::isNotWeakEqualEps(newLine.distance(linePoint - lineDirection)))
		{
			scopedIteration.setInaccurate();
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestLine2::testFitLineLeastSquare(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "fitLineLeastSquare test:";

	constexpr double successThreshold = 0.99;

	RandomGenerator randomGenerator;
	ValidationPrecision validation(successThreshold, randomGenerator);

	const Scalar range = std::is_same<Scalar, float>::value ? Scalar(100) : Scalar(1000);

	const Timestamp startTimestamp(true);

	do
	{
		{
			// testing with points exactly on a line (no noise)

			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Vector2 linePoint = Random::vector2(randomGenerator, -range, range);
			const Vector2 lineDirection = Random::vector2(randomGenerator);
			ocean_assert(lineDirection.isUnit());

			const Line2 groundTruthLine(linePoint, lineDirection);
			ocean_assert(groundTruthLine.isValid());

			const size_t numberPoints = size_t(RandomI::random(randomGenerator, 2u, 100u));

			Vectors2 points;
			points.reserve(numberPoints);

			for (size_t i = 0; i < numberPoints; ++i)
			{
				const Scalar distance = Random::scalar(randomGenerator, -range, range);
				points.emplace_back(groundTruthLine.point(distance));
			}

			Line2 fittedLine;
			OCEAN_EXPECT_TRUE(validation, Line2::fitLineLeastSquare(points.data(), points.size(), fittedLine));
			ocean_assert(fittedLine.isValid());

			const Scalar maxDistanceThreshold = std::is_same<Scalar, float>::value ? Scalar(0.01) : Scalar(0.0001);

			for (const Vector2& point : points)
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

			const Vector2 linePoint = Random::vector2(randomGenerator, -range, range);
			const Vector2 lineDirection = Random::vector2(randomGenerator);
			ocean_assert(lineDirection.isUnit());

			const Line2 groundTruthLine(linePoint, lineDirection);
			ocean_assert(groundTruthLine.isValid());

			const Vector2 perpendicular = lineDirection.perpendicular();

			const size_t numberPoints = size_t(RandomI::random(randomGenerator, 2u, 100u));

			const Scalar maxNoise = std::is_same<Scalar, float>::value ? Scalar(0.01) : Scalar(0.001);

			Vectors2 points;
			points.reserve(numberPoints);

			for (size_t i = 0; i < numberPoints; ++i)
			{
				const Scalar distance = Random::scalar(randomGenerator, -range, range);
				const Scalar noise = Random::scalar(randomGenerator, -maxNoise, maxNoise);
				points.emplace_back(groundTruthLine.point(distance) + perpendicular * noise);
			}

			Line2 fittedLine;
			OCEAN_EXPECT_TRUE(validation, Line2::fitLineLeastSquare(points.data(), points.size(), fittedLine));
			ocean_assert(fittedLine.isValid());

			Scalar sqrDistanceGroundTruth = 0;
			Scalar sqrDistanceFitted = 0;

			for (const Vector2& point : points)
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
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
