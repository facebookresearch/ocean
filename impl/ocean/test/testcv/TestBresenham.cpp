/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestBresenham.h"

#include "ocean/base/Timestamp.h"

#include "ocean/cv/Bresenham.h"

#include "ocean/math/FiniteLine2.h"
#include "ocean/math/Line2.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestBresenham::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Bresenham test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	Log::info() << " ";

	allSucceeded = testIntegerBorderIntersection(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testFloatBorderIntersection(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNumberLinePixels(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Bresenham test succeeded.";
	}
	else
	{
		Log::info() << "Bresenham test FAILED!";
	}

	return allSucceeded;
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

	bool fatalError = false;

	{
		// horizontal intersection interger

		const int left = RandomI::random(-1000, 1000);
		const int top = RandomI::random(-1000, 1000);

		const unsigned int width = RandomI::random(10u, 1920u);
		const unsigned int height = RandomI::random(10u, 1080u);

		const int right = left + int(width) - 1;
		const int bottom = top + int(height) - 1;

		const Vector2 lineStartPoint(Scalar(RandomI::random(-2000, 2000)), Scalar(RandomI::random(-2000, 2000)));
		const Vector2 lineDirection = Vector2(Random::sign(), 0);

		const Line2 line(lineStartPoint, lineDirection);

		int x0, y0, x1, y1;

		if (CV::Bresenham::borderIntersection(line, left, top, right, bottom, x0, y0, x1, y1))
		{
			if (lineStartPoint.y() < Scalar(top) || lineStartPoint.y() > Scalar(bottom))
			{
				fatalError = true;
			}
			else
			{
				if (x0 != left || x1 != right || Scalar(y0) != lineStartPoint.y() || Scalar(y1) != lineStartPoint.y())
				{
					fatalError = true;
				}
			}
		}
		else
		{
			if (lineStartPoint.y() >= Scalar(top) && lineStartPoint.y() <= Scalar(bottom))
			{
				fatalError = true;
			}
		}
	}

	{
		// vertical intersection

		const int left = RandomI::random(-1000, 1000);
		const int top = RandomI::random(-1000, 1000);

		const unsigned int width = RandomI::random(10u, 1920u);
		const unsigned int height = RandomI::random(10u, 1080u);

		const int right = left + int(width) - 1;
		const int bottom = top + int(height) - 1;

		const Vector2 lineStartPoint(Scalar(RandomI::random(-2000, 2000)), Scalar(RandomI::random(-2000, 2000)));
		const Vector2 lineDirection = Vector2(0, Random::sign());

		const Line2 line(lineStartPoint, lineDirection);

		int x0, y0, x1, y1;

		if (CV::Bresenham::borderIntersection(line, left, top, right, bottom, x0, y0, x1, y1))
		{
			if (lineStartPoint.x() < Scalar(left) || lineStartPoint.x() > Scalar(right))
			{
				fatalError = true;
			}
			else
			{
				if (y0 != top || y1 != bottom || Scalar(x0) != lineStartPoint.x() || Scalar(x1) != lineStartPoint.x())
				{
					fatalError = true;
				}
			}
		}
		else
		{
			if (lineStartPoint.x() >= Scalar(left) && lineStartPoint.x() <= Scalar(right))
			{
				fatalError = true;
			}
		}
	}

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			bool localSucceeded = true;

			const int borderLeft = RandomI::random(-1000, 1000);
			const int borderTop = RandomI::random(-1000, 1000);

			const unsigned int width = RandomI::random(1u, 1000u);
			const unsigned int height = RandomI::random(1u, 1000u);

			const int borderRight = borderLeft + int(width);
			const int borderBottom = borderTop + int(height);

			const Vector2 point0(Random::scalar(Scalar(borderLeft - 100), Scalar(borderRight + 100)), Random::scalar(Scalar(borderTop - 100), Scalar(borderBottom + 100)));
			Vector2 point1(Random::scalar(Scalar(borderLeft - 100), Scalar(borderRight + 100)), Random::scalar(Scalar(borderTop - 100), Scalar(borderBottom + 100)));

			while (point0.isEqual(point1, Scalar(1)))
				point1 = Vector2(Random::scalar(Scalar(borderLeft - 100), Scalar(borderRight + 100)), Random::scalar(Scalar(borderTop - 100), Scalar(borderBottom + 100)));

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

			typedef std::pair<int, int> Coordinate;
			typedef std::set<Coordinate> CoordinateSet;
			CoordinateSet testIntersectionSet;

			if (leftIntersection)
				testIntersectionSet.insert(Coordinate(borderLeft, Numeric::round32(leftIntersectionPoint.y())));

			if (rightIntersection)
				testIntersectionSet.insert(Coordinate(borderRight, Numeric::round32(rightIntersectionPoint.y())));

			if (topIntersection)
				testIntersectionSet.insert(Coordinate(Numeric::round32(topIntersectionPoint.x()), borderTop));

			if (bottomIntersection)
				testIntersectionSet.insert(Coordinate(Numeric::round32(bottomIntersectionPoint.x()), borderBottom));

			ocean_assert(testIntersectionSet.size() <= 2);

			int x0, y0, x1, y1;
			if (CV::Bresenham::borderIntersection(line, borderLeft, borderTop, borderRight, borderBottom, x0, y0, x1, y1))
			{
				ocean_assert(line.distance(Vector2(Scalar(x0), Scalar(y0))) <= 2 && line.distance(Vector2(Scalar(x1), Scalar(y1))) <= 2);

				for (CoordinateSet::const_iterator i = testIntersectionSet.begin(); i != testIntersectionSet.end(); ++i)
					ocean_assert(i->first >= borderLeft && i->first <= borderRight && i->second >= borderTop && i->second <= borderBottom);

				if (testIntersectionSet.size() == 0 || testIntersectionSet.size() > 2)
					localSucceeded = false;
				else
				{
					CoordinateSet intersectionSet;
					intersectionSet.insert(Coordinate(x0, y0));
					intersectionSet.insert(Coordinate(x1, y1));

					if (intersectionSet != testIntersectionSet)
						localSucceeded = false;
				}
			}
			else
			{
				if (leftIntersection || rightIntersection || topIntersection || bottomIntersection)
				{
					if (testIntersectionSet.size() != 1)
						localSucceeded = false;
				}
			}

			if (localSucceeded)
				validIterations++;

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (fatalError)
	{
		Log::info() << "Validation: FAILED!";
		return false;
	}

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestBresenham::testFloatBorderIntersection(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Sub-pixel border intersection test:";

	bool fatalError = false;

	{
		// horizontal intersection interger

		const Scalar left = Random::scalar(-100, 100);
		const Scalar top = Random::scalar(-100, 100);

		const Scalar width = Random::scalar(10, 100);
		const Scalar height = Random::scalar(10, 100);

		const Scalar right = left + width;
		const Scalar bottom = top + height;

		const Vector2 lineStartPoint(Random::scalar(-200, 200), Random::scalar(-200, 200));
		const Vector2 lineDirection = Vector2(Random::sign(), 0);

		const Line2 line(lineStartPoint, lineDirection);

		Scalar x0, y0, x1, y1;

		if (CV::Bresenham::borderIntersection(line, left, top, right, bottom, x0, y0, x1, y1))
		{
			if (lineStartPoint.y() < top - Numeric::weakEps() || lineStartPoint.y() > bottom + Numeric::weakEps())
			{
				fatalError = true;
			}
			else
			{
				if (Numeric::isNotWeakEqual(x0, left) || Numeric::isNotWeakEqual(x1, right) || Numeric::isNotWeakEqual(y0, lineStartPoint.y()) || Numeric::isNotWeakEqual(y1, lineStartPoint.y()))
				{
					fatalError = true;
				}
			}
		}
		else
		{
			if (lineStartPoint.y() > top + Numeric::weakEps() && lineStartPoint.y() < bottom - Numeric::weakEps())
			{
				fatalError = true;
			}
		}
	}

	{
		// vertical intersection

		const Scalar left = Random::scalar(-100, 000);
		const Scalar top = Random::scalar(-100, 100);

		const Scalar width = Random::scalar(10, 100);
		const Scalar height = Random::scalar(10, 100);

		const Scalar right = left + width;
		const Scalar bottom = top + height;

		const Vector2 lineStartPoint(Random::scalar(-200, 200), Random::scalar(-200, 200));
		const Vector2 lineDirection = Vector2(0, Random::sign());

		const Line2 line(lineStartPoint, lineDirection);

		Scalar x0, y0, x1, y1;

		if (CV::Bresenham::borderIntersection(line, left, top, right, bottom, x0, y0, x1, y1))
		{
			if (lineStartPoint.x() < left - Numeric::weakEps() || lineStartPoint.x() > right + Numeric::weakEps())
			{
				fatalError = true;
			}
			else
			{
				if (Numeric::isNotWeakEqual(y0, top) || Numeric::isNotWeakEqual(y1, bottom) || Numeric::isNotWeakEqual(x0, lineStartPoint.x()) || Numeric::isNotWeakEqual(x1, lineStartPoint.x()))
				{
					fatalError = true;
				}
			}
		}
		else
		{
			if (lineStartPoint.x() > left + Numeric::weakEps() && lineStartPoint.x() < right - Numeric::weakEps())
			{
				fatalError = true;
			}
		}
	}

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			bool localSucceeded = true;

			const Scalar borderLeft = Random::scalar(-100, 100);
			const Scalar borderTop = Random::scalar(-100, 100);

			const Scalar width = Random::scalar(Scalar(0.01), 100);
			const Scalar height = Random::scalar(Scalar(0.01), 100);

			const Scalar borderRight = borderLeft + width;
			const Scalar borderBottom = borderTop + height;

			const Vector2 point0(Random::scalar(Scalar(borderLeft - 100), Scalar(borderRight + 100)), Random::scalar(Scalar(borderTop - 100), Scalar(borderBottom + 100)));
			Vector2 point1(Random::scalar(Scalar(borderLeft - 100), Scalar(borderRight + 100)), Random::scalar(Scalar(borderTop - 100), Scalar(borderBottom + 100)));

			while (point0.isEqual(point1, Scalar(1)))
				point1 = Vector2(Random::scalar(Scalar(borderLeft - 100), Scalar(borderRight + 100)), Random::scalar(Scalar(borderTop - 100), Scalar(borderBottom + 100)));

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
				testIntersections.push_back(Vector2(Scalar(borderLeft), leftIntersectionPoint.y()));

			if (rightIntersection)
				testIntersections.push_back(Vector2(Scalar(borderRight), rightIntersectionPoint.y()));

			if (topIntersection)
				testIntersections.push_back(Vector2(topIntersectionPoint.x(), Scalar(borderTop)));

			if (bottomIntersection)
				testIntersections.push_back(Vector2(bottomIntersectionPoint.x(), Scalar(borderBottom)));

			ocean_assert(testIntersections.size() <= 2);

			Scalar x0, y0, x1, y1;
			if (CV::Bresenham::borderIntersection(line, borderLeft, borderTop, borderRight, borderBottom, x0, y0, x1, y1))
			{
				ocean_assert(line.distance(Vector2(Scalar(x0), Scalar(y0))) <= Scalar(0.001) && line.distance(Vector2(Scalar(x1), Scalar(y1))) <= Scalar(0.001));

				for (Vectors2::const_iterator i = testIntersections.begin(); i != testIntersections.end(); ++i)
					ocean_assert(i->x() >= borderLeft && i->x() <= borderRight && i->y() >= borderTop && i->y() <= borderBottom);

				if (testIntersections.size() != 2)
					localSucceeded = false;
				else
				{
					if (!(((Vector2(x0, y0).isEqual(testIntersections[0], Scalar(0.001)) && Vector2(x1, y1).isEqual(testIntersections[1], Scalar(0.001))) || (Vector2(x0, y0).isEqual(testIntersections[1], Scalar(0.001)) && Vector2(x1, y1).isEqual(testIntersections[0], Scalar(0.001))))))
						localSucceeded = false;
				}
			}
			else
			{
				if (leftIntersection || rightIntersection || topIntersection || bottomIntersection)
				{
					if (testIntersections.size() != 1)
						localSucceeded = false;
				}
			}

			if (localSucceeded)
				validIterations++;

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (fatalError)
	{
		Log::info() << "Validation: FAILED!";
		return false;
	}

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestBresenham::testNumberLinePixels(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Number pixels test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

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

			if (pixels < minPixels || pixels > maxPixels)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

}

}

}
