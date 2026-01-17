/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestBresenham.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"

#include "ocean/base/Timestamp.h"

#include "ocean/cv/Bresenham.h"

#include "ocean/math/FiniteLine2.h"
#include "ocean/math/Line2.h"
#include "ocean/math/Random.h"

#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestBresenham::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Bresenham test");

	if (selector.shouldRun("integerborderintersection"))
	{
		testResult = testIntegerBorderIntersection(testDuration);

		Log::info() << " ";
	}

	if (selector.shouldRun("floatborderintersection"))
	{
		testResult = testFloatBorderIntersection(testDuration);

		Log::info() << " ";
	}

	if (selector.shouldRun("numberlinepixels"))
	{
		testResult = testNumberLinePixels(testDuration);

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestBresenham, IntegerBorderIntersection)
{
	EXPECT_TRUE(TestBresenham::testIntegerBorderIntersection(GTEST_TEST_DURATION));
}

TEST(TestBresenham, FloatBorderIntersection)
{
	EXPECT_TRUE(TestBresenham::testFloatBorderIntersection(GTEST_TEST_DURATION));
}

TEST(TestBresenham, NumberLinePixels)
{
	EXPECT_TRUE(TestBresenham::testNumberLinePixels(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestBresenham::testIntegerBorderIntersection(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Pixel border intersection test:";

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.99, randomGenerator);

	{
		// horizontal intersection interger

		const int left = RandomI::random(randomGenerator, -1000, 1000);
		const int top = RandomI::random(randomGenerator, -1000, 1000);

		const unsigned int width = RandomI::random(randomGenerator, 10u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 10u, 1080u);

		const int right = left + int(width) - 1;
		const int bottom = top + int(height) - 1;

		Vector2 lineStartPoint;
		lineStartPoint.x() = Scalar(RandomI::random(randomGenerator, -2000, 2000));
		lineStartPoint.y() = Scalar(RandomI::random(randomGenerator, -2000, 2000));

		const Vector2 lineDirection = Vector2(Random::sign(randomGenerator), 0);

		const Line2 line(lineStartPoint, lineDirection);

		int x0, y0, x1, y1;

		if (CV::Bresenham::borderIntersection(line, left, top, right, bottom, x0, y0, x1, y1))
		{
			OCEAN_EXPECT_GREATER_EQUAL(validation, lineStartPoint.y(), Scalar(top));
			OCEAN_EXPECT_LESS_EQUAL(validation, lineStartPoint.y(), Scalar(bottom));

			OCEAN_EXPECT_EQUAL(validation, x0, left);
			OCEAN_EXPECT_EQUAL(validation, x1, right);
			OCEAN_EXPECT_EQUAL(validation, Scalar(y0), lineStartPoint.y());
			OCEAN_EXPECT_EQUAL(validation, Scalar(y1), lineStartPoint.y());
		}
		else
		{
			OCEAN_EXPECT_FALSE(validation, lineStartPoint.y() >= Scalar(top) && lineStartPoint.y() <= Scalar(bottom));
		}
	}

	{
		// vertical intersection

		const int left = RandomI::random(randomGenerator, -1000, 1000);
		const int top = RandomI::random(randomGenerator, -1000, 1000);

		const unsigned int width = RandomI::random(randomGenerator, 10u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 10u, 1080u);

		const int right = left + int(width) - 1;
		const int bottom = top + int(height) - 1;

		Vector2 lineStartPoint;
		lineStartPoint.x() = Scalar(RandomI::random(randomGenerator, -2000, 2000));
		lineStartPoint.y() = Scalar(RandomI::random(randomGenerator, -2000, 2000));

		const Vector2 lineDirection = Vector2(0, Random::sign(randomGenerator));

		const Line2 line(lineStartPoint, lineDirection);

		int x0, y0, x1, y1;

		if (CV::Bresenham::borderIntersection(line, left, top, right, bottom, x0, y0, x1, y1))
		{
			OCEAN_EXPECT_GREATER_EQUAL(validation, lineStartPoint.x(), Scalar(left));
			OCEAN_EXPECT_LESS_EQUAL(validation, lineStartPoint.x(), Scalar(right));

			OCEAN_EXPECT_EQUAL(validation, y0, top);
			OCEAN_EXPECT_EQUAL(validation, y1, bottom);
			OCEAN_EXPECT_EQUAL(validation, Scalar(x0), lineStartPoint.x());
			OCEAN_EXPECT_EQUAL(validation, Scalar(x1), lineStartPoint.x());
		}
		else
		{
			OCEAN_EXPECT_FALSE(validation, lineStartPoint.x() >= Scalar(left) && lineStartPoint.x() <= Scalar(right));
		}
	}

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const int borderLeft = RandomI::random(randomGenerator, -1000, 1000);
			const int borderTop = RandomI::random(randomGenerator, -1000, 1000);

			const unsigned int width = RandomI::random(randomGenerator, 1u, 1000u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 1000u);

			const int borderRight = borderLeft + int(width);
			const int borderBottom = borderTop + int(height);

			const Vector2 point0 = Random::vector2(randomGenerator, Scalar(borderLeft - 100), Scalar(borderRight + 100), Scalar(borderTop - 100), Scalar(borderBottom + 100));
			Vector2 point1 = Random::vector2(randomGenerator, Scalar(borderLeft - 100), Scalar(borderRight + 100), Scalar(borderTop - 100), Scalar(borderBottom + 100));

			while (point0.isEqual(point1, Scalar(1)))
			{
				point1 = Random::vector2(randomGenerator, Scalar(borderLeft - 100), Scalar(borderRight + 100), Scalar(borderTop - 100), Scalar(borderBottom + 100));
			}

			const Line2 line(point0, (point1 - point0).normalized());

			const FiniteLine2 leftBorderLine = FiniteLine2(Vector2(Scalar(borderLeft), Scalar(borderTop)), Vector2(Scalar(borderLeft), Scalar(borderBottom)));
			const FiniteLine2 rightBorderLine = FiniteLine2(Vector2(Scalar(borderRight), Scalar(borderTop)), Vector2(Scalar(borderRight), Scalar(borderBottom)));
			const FiniteLine2 topBorderLine = FiniteLine2(Vector2(Scalar(borderLeft), Scalar(borderTop)), Vector2(Scalar(borderRight), Scalar(borderTop)));
			const FiniteLine2 bottomBorderLine = FiniteLine2(Vector2(Scalar(borderLeft), Scalar(borderBottom)), Vector2(Scalar(borderRight), Scalar(borderBottom)));

			Vector2 leftIntersectionPoint;
			const bool leftIntersection = leftBorderLine.intersection(line, leftIntersectionPoint);

			Vector2 rightIntersectionPoint;
			const bool rightIntersection = rightBorderLine.intersection(line, rightIntersectionPoint);

			Vector2 topIntersectionPoint;
			const bool topIntersection = topBorderLine.intersection(line, topIntersectionPoint);

			Vector2 bottomIntersectionPoint;
			const bool bottomIntersection = bottomBorderLine.intersection(line, bottomIntersectionPoint);

			using Coordinate = std::pair<int, int>;
			using CoordinateSet = std::set<Coordinate>;
			CoordinateSet testIntersectionSet;

			if (leftIntersection)
			{
				testIntersectionSet.insert(Coordinate(borderLeft, Numeric::round32(leftIntersectionPoint.y())));
			}

			if (rightIntersection)
			{
				testIntersectionSet.insert(Coordinate(borderRight, Numeric::round32(rightIntersectionPoint.y())));
			}

			if (topIntersection)
			{
				testIntersectionSet.insert(Coordinate(Numeric::round32(topIntersectionPoint.x()), borderTop));
			}

			if (bottomIntersection)
			{
				testIntersectionSet.insert(Coordinate(Numeric::round32(bottomIntersectionPoint.x()), borderBottom));
			}

			ocean_assert(testIntersectionSet.size() <= 2);

			int x0, y0, x1, y1;
			if (CV::Bresenham::borderIntersection(line, borderLeft, borderTop, borderRight, borderBottom, x0, y0, x1, y1))
			{
				ocean_assert(line.distance(Vector2(Scalar(x0), Scalar(y0))) <= 2 && line.distance(Vector2(Scalar(x1), Scalar(y1))) <= 2);

				for (CoordinateSet::const_iterator i = testIntersectionSet.begin(); i != testIntersectionSet.end(); ++i)
				{
					ocean_assert(i->first >= borderLeft && i->first <= borderRight && i->second >= borderTop && i->second <= borderBottom);
				}

				if (testIntersectionSet.size() == 0 || testIntersectionSet.size() > 2)
				{
					scopedIteration.setInaccurate();
				}
				else
				{
					CoordinateSet intersectionSet;
					intersectionSet.insert(Coordinate(x0, y0));
					intersectionSet.insert(Coordinate(x1, y1));

					if (intersectionSet != testIntersectionSet)
					{
						scopedIteration.setInaccurate();
					}
				}
			}
			else
			{
				if (leftIntersection || rightIntersection || topIntersection || bottomIntersection)
				{
					if (testIntersectionSet.size() != 1)
					{
						scopedIteration.setInaccurate();
					}
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestBresenham::testFloatBorderIntersection(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Sub-pixel border intersection test:";

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.99, randomGenerator);

	{
		// horizontal intersection interger

		const Scalar left = Random::scalar(randomGenerator, -100, 100);
		const Scalar top = Random::scalar(randomGenerator, -100, 100);

		const Scalar width = Random::scalar(randomGenerator, 10, 100);
		const Scalar height = Random::scalar(randomGenerator, 10, 100);

		const Scalar right = left + width;
		const Scalar bottom = top + height;

		const Vector2 lineStartPoint(Random::scalar(randomGenerator, -200, 200), Random::scalar(randomGenerator, -200, 200));
		const Vector2 lineDirection = Vector2(Random::sign(randomGenerator), 0);

		const Line2 line(lineStartPoint, lineDirection);

		Scalar x0, y0, x1, y1;

		if (CV::Bresenham::borderIntersection(line, left, top, right, bottom, x0, y0, x1, y1))
		{
			OCEAN_EXPECT_GREATER_EQUAL(validation, lineStartPoint.y(), top - Numeric::weakEps());
			OCEAN_EXPECT_LESS_EQUAL(validation, lineStartPoint.y(), bottom + Numeric::weakEps());

			OCEAN_EXPECT_TRUE(validation, Numeric::isWeakEqual(x0, left));
			OCEAN_EXPECT_TRUE(validation, Numeric::isWeakEqual(x1, right));
			OCEAN_EXPECT_TRUE(validation, Numeric::isWeakEqual(y0, lineStartPoint.y()));
			OCEAN_EXPECT_TRUE(validation, Numeric::isWeakEqual(y1, lineStartPoint.y()));
		}
		else
		{
			OCEAN_EXPECT_FALSE(validation, lineStartPoint.y() > top + Numeric::weakEps() && lineStartPoint.y() < bottom - Numeric::weakEps());
		}
	}

	{
		// vertical intersection

		const Scalar left = Random::scalar(randomGenerator, -100, 000);
		const Scalar top = Random::scalar(randomGenerator, -100, 100);

		const Scalar width = Random::scalar(randomGenerator, 10, 100);
		const Scalar height = Random::scalar(randomGenerator, 10, 100);

		const Scalar right = left + width;
		const Scalar bottom = top + height;

		const Vector2 lineStartPoint(Random::scalar(randomGenerator, -200, 200), Random::scalar(randomGenerator, -200, 200));
		const Vector2 lineDirection = Vector2(0, Random::sign(randomGenerator));

		const Line2 line(lineStartPoint, lineDirection);

		Scalar x0, y0, x1, y1;

		if (CV::Bresenham::borderIntersection(line, left, top, right, bottom, x0, y0, x1, y1))
		{
			OCEAN_EXPECT_GREATER_EQUAL(validation, lineStartPoint.x(), left - Numeric::weakEps());
			OCEAN_EXPECT_LESS_EQUAL(validation, lineStartPoint.x(), right + Numeric::weakEps());

			OCEAN_EXPECT_TRUE(validation, Numeric::isWeakEqual(y0, top));
			OCEAN_EXPECT_TRUE(validation, Numeric::isWeakEqual(y1, bottom));
			OCEAN_EXPECT_TRUE(validation, Numeric::isWeakEqual(x0, lineStartPoint.x()));
			OCEAN_EXPECT_TRUE(validation, Numeric::isWeakEqual(x1, lineStartPoint.x()));
		}
		else
		{
			OCEAN_EXPECT_FALSE(validation, lineStartPoint.x() > left + Numeric::weakEps() && lineStartPoint.x() < right - Numeric::weakEps());
		}
	}

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Scalar borderLeft = Random::scalar(randomGenerator, -100, 100);
			const Scalar borderTop = Random::scalar(randomGenerator, -100, 100);

			const Scalar width = Random::scalar(randomGenerator, Scalar(0.01), 100);
			const Scalar height = Random::scalar(randomGenerator, Scalar(0.01), 100);

			const Scalar borderRight = borderLeft + width;
			const Scalar borderBottom = borderTop + height;

			const Vector2 point0(Random::scalar(randomGenerator, Scalar(borderLeft - 100), Scalar(borderRight + 100)), Random::scalar(randomGenerator, Scalar(borderTop - 100), Scalar(borderBottom + 100)));
			Vector2 point1(Random::scalar(randomGenerator, Scalar(borderLeft - 100), Scalar(borderRight + 100)), Random::scalar(randomGenerator, Scalar(borderTop - 100), Scalar(borderBottom + 100)));

			while (point0.isEqual(point1, Scalar(1)))
			{
				point1 = Vector2(Random::scalar(randomGenerator, Scalar(borderLeft - 100), Scalar(borderRight + 100)), Random::scalar(randomGenerator, Scalar(borderTop - 100), Scalar(borderBottom + 100)));
			}

			const Line2 line(point0, (point1 - point0).normalized());

			const FiniteLine2 leftBorderLine = FiniteLine2(Vector2(borderLeft, borderTop), Vector2(borderLeft, borderBottom));
			const FiniteLine2 rightBorderLine = FiniteLine2(Vector2(borderRight, borderTop), Vector2(borderRight, borderBottom));
			const FiniteLine2 topBorderLine = FiniteLine2(Vector2(borderLeft, borderTop), Vector2(borderRight, borderTop));
			const FiniteLine2 bottomBorderLine = FiniteLine2(Vector2(borderLeft, borderBottom), Vector2(borderRight, borderBottom));

			Vector2 leftIntersectionPoint(Numeric::minValue(), Numeric::minValue());
			const bool leftIntersection = leftBorderLine.intersection(line, leftIntersectionPoint);

			Vector2 rightIntersectionPoint(Numeric::minValue(), Numeric::minValue());
			const bool rightIntersection = rightBorderLine.intersection(line, rightIntersectionPoint);

			Vector2 topIntersectionPoint(Numeric::minValue(), Numeric::minValue());
			const bool topIntersection = topBorderLine.intersection(line, topIntersectionPoint);

			Vector2 bottomIntersectionPoint(Numeric::minValue(), Numeric::minValue());
			const bool bottomIntersection = bottomBorderLine.intersection(line, bottomIntersectionPoint);

			Vectors2 testIntersections;

			if (leftIntersection)
			{
				testIntersections.emplace_back(Scalar(borderLeft), leftIntersectionPoint.y());
			}

			if (rightIntersection)
			{
				testIntersections.emplace_back(Scalar(borderRight), rightIntersectionPoint.y());
			}

			if (topIntersection)
			{
				testIntersections.emplace_back(topIntersectionPoint.x(), Scalar(borderTop));
			}

			if (bottomIntersection)
			{
				testIntersections.emplace_back(bottomIntersectionPoint.x(), Scalar(borderBottom));
			}

			ocean_assert(testIntersections.size() <= 2);

			Scalar x0, y0, x1, y1;
			if (CV::Bresenham::borderIntersection(line, borderLeft, borderTop, borderRight, borderBottom, x0, y0, x1, y1))
			{
				ocean_assert(line.distance(Vector2(Scalar(x0), Scalar(y0))) <= Scalar(0.001) && line.distance(Vector2(Scalar(x1), Scalar(y1))) <= Scalar(0.001));

				for (Vectors2::const_iterator i = testIntersections.begin(); i != testIntersections.end(); ++i)
				{
					ocean_assert(i->x() >= borderLeft && i->x() <= borderRight && i->y() >= borderTop && i->y() <= borderBottom);
				}

				if (testIntersections.size() != 2)
				{
					scopedIteration.setInaccurate();
				}
				else
				{
					if (!(((Vector2(x0, y0).isEqual(testIntersections[0], Scalar(0.001)) && Vector2(x1, y1).isEqual(testIntersections[1], Scalar(0.001))) || (Vector2(x0, y0).isEqual(testIntersections[1], Scalar(0.001)) && Vector2(x1, y1).isEqual(testIntersections[0], Scalar(0.001))))))
					{
						scopedIteration.setInaccurate();
					}
				}
			}
			else
			{
				if (leftIntersection || rightIntersection || topIntersection || bottomIntersection)
				{
					if (testIntersections.size() != 1)
					{
						scopedIteration.setInaccurate();
					}
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestBresenham::testNumberLinePixels(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Number pixels test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const int x0 = RandomI::random(randomGenerator, -1000, 1000);
			const int y0 = RandomI::random(randomGenerator, -1000, 1000);

			const int x1 = RandomI::random(randomGenerator, -1000, 1000);
			const int y1 = RandomI::random(randomGenerator, -1000, 1000);

			const unsigned int pixels = CV::Bresenham::numberLinePixels(x0, y0, x1, y1);

			const unsigned int sizeX = (unsigned int)std::abs(x0 - x1);
			const unsigned int sizeY = (unsigned int)std::abs(y0 - y1);
			const unsigned int size = std::max(sizeX, sizeY);

			const unsigned int size_10 = std::max(1u, (size + 5u) / 10u); // 10%

			const unsigned int minPixels = (unsigned int)std::max(1, int(size - size_10));
			const unsigned int maxPixels = size + size_10;

			ocean_assert(minPixels <= maxPixels);

			OCEAN_EXPECT_GREATER_EQUAL(validation, pixels, minPixels);
			OCEAN_EXPECT_LESS_EQUAL(validation, pixels, maxPixels);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
