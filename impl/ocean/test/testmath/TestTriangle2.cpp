/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestTriangle2.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/Quaternion.h"
#include "ocean/math/Random.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Triangle2.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestTriangle2::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Triangle2 test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testIntersects(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testIsCounterClockwise(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPadded(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Triangle2 test succeeded.";
	}
	else
	{
		Log::info() << "Triangle2 test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestTriangle2, Intersects)
{
	EXPECT_TRUE(TestTriangle2::testIntersects(GTEST_TEST_DURATION));
}

TEST(TestTriangle2, IsCounterClockwise)
{
	EXPECT_TRUE(TestTriangle2::testIsCounterClockwise(GTEST_TEST_DURATION));
}

TEST(TestTriangle2, Padded)
{
	EXPECT_TRUE(TestTriangle2::testPadded(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestTriangle2::testIntersects(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Intersects test:";

	bool someFailed = false;

	// some hard-coded tests
	{
		const Triangle2 triangleA(Vector2(0, 5), Vector2(0, 0), Vector2(5, 0));
		const Triangle2 triangleB(Vector2(Scalar(0.01), 5), Vector2(Scalar(5.01), 0), Vector2(5, 5));

		if (triangleA.intersects(triangleB))
		{
			someFailed = true;
		}
	}

	{
		const Triangle2 triangleA(Vector2(0, 5), Vector2(-2, 0), Vector2(2, 0));
		const Triangle2 triangleB(Vector2(-5, 3), Vector2(0, -2), Vector2(5, 3));

		if (!triangleA.intersects(triangleB))
		{
			someFailed = true;
		}
	}

	{
		const Triangle2 triangleA(Vector2(0, 5), Vector2(-2, 0), Vector2(2, 0));
		const Triangle2 triangleB(Vector2(0, 6), Vector2(-3, -1), Vector2(3, -1));

		if (!triangleA.intersects(triangleB))
		{
			someFailed = true;
		}
	}

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);
	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			{
				// two identical (valid) triangles have always an intersection
				const Triangle2 triangle(Random::vector2(randomGenerator, -10, 10), Random::vector2(randomGenerator, -10, 10), Random::vector2(randomGenerator, -10, 10));

				if (triangle.isValid() && !triangle.intersects(triangle))
				{
					someFailed = true;
				}
			}

			{
				// two triangles lying in different areas do not have an intersection
				const Triangle2 triangleA(Random::vector2(randomGenerator, -10, Scalar(-0.01)), Random::vector2(randomGenerator, -10, Scalar(-0.01)), Random::vector2(randomGenerator, -10, Scalar(-0.01)));
				const Triangle2 triangleB(Random::vector2(randomGenerator, Scalar(0.01), 10), Random::vector2(randomGenerator, Scalar(0.01), 10), Random::vector2(randomGenerator, Scalar(0.01), 10));

				if (triangleA.isValid() && triangleB.isValid() && triangleA.intersects(triangleB))
				{
					someFailed = true;
				}
			}

			{
				// random triangles

				const Triangle2 triangleA(Random::vector2(randomGenerator, -10, 10), Random::vector2(randomGenerator, -10, 10), Random::vector2(randomGenerator, -10, 10));
				const Triangle2 triangleB(Random::vector2(randomGenerator, -10, 10), Random::vector2(randomGenerator, -10, 10), Random::vector2(randomGenerator, -10, 10));

				if (triangleA.isValid() && triangleB.isValid())
				{
					const FiniteLine2 lineA_01(triangleA.point0(), triangleA.point1());
					const FiniteLine2 lineA_12(triangleA.point1(), triangleA.point2());
					const FiniteLine2 lineA_20(triangleA.point2(), triangleA.point0());

					const FiniteLine2 lineB_01(triangleB.point0(), triangleB.point1());
					const FiniteLine2 lineB_12(triangleB.point1(), triangleB.point2());
					const FiniteLine2 lineB_20(triangleB.point2(), triangleB.point0());

					const bool pointInA = triangleA.isInside(triangleB.point0()) || triangleA.isInside(triangleB.point1()) || triangleA.isInside(triangleB.point2());
					const bool pointInB = triangleB.isInside(triangleA.point0()) || triangleB.isInside(triangleA.point1()) || triangleB.isInside(triangleA.point2());

					const bool linesIntersect = lineA_01.intersects(lineB_01) || lineA_01.intersects(lineB_12) || lineA_01.intersects(lineB_20)
													|| lineA_12.intersects(lineB_01) || lineA_12.intersects(lineB_12) || lineA_12.intersects(lineB_20)
													|| lineA_20.intersects(lineB_01) || lineA_20.intersects(lineB_12) || lineA_20.intersects(lineB_20);

					const bool hasIntersection = pointInA || pointInB || linesIntersect;

					if (hasIntersection == triangleA.intersects(triangleB))
					{
						validIterations++;
					}

					iterations++;
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (someFailed)
	{
		Log::info() << "Validation: FAILED!";
		return false;
	}
	else
	{
		ocean_assert(iterations != 0ull);
		double percent = double(validIterations) / double(iterations);

		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

		return percent >= 0.99;
	}
}

bool TestTriangle2::testIsCounterClockwise(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "IsCounterClockwise test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);
	do
	{
		const Scalar radius = Random::scalar(randomGenerator, Scalar(0.1), Scalar(10));

		const Scalar angle0 = Random::scalar(randomGenerator, Scalar(0), Numeric::pi2());
		const Scalar angle1 = angle0 + Random::scalar(randomGenerator, Numeric::deg2rad(10), Numeric::deg2rad(135));
		const Scalar angle2 = angle1 + Random::scalar(randomGenerator, Numeric::deg2rad(10), Numeric::deg2rad(135));

		{
			// image-like coordinate system

			const bool yAxisDownwards = true;

			const Vector3 zAxis(0, 0, -1);

			const Vector2 point0 = (Quaternion(zAxis, Numeric::angleAdjustPositive(angle0)) * Vector3(radius, 0, 0)).xy();
			const Vector2 point1 = (Quaternion(zAxis, Numeric::angleAdjustPositive(angle1)) * Vector3(radius, 0, 0)).xy();
			const Vector2 point2 = (Quaternion(zAxis, Numeric::angleAdjustPositive(angle2)) * Vector3(radius, 0, 0)).xy();

			const Triangle2 counterClockwiseTriangleA(point0, point1, point2);
			const Triangle2 counterClockwiseTriangleB(point1, point2, point0);
			const Triangle2 counterClockwiseTriangleC(point2, point0, point1);

			if (!counterClockwiseTriangleA.isCounterClockwise(yAxisDownwards))
			{
				allSucceeded = false;
			}

			if (!counterClockwiseTriangleB.isCounterClockwise(yAxisDownwards))
			{
				allSucceeded = false;
			}

			if (!counterClockwiseTriangleC.isCounterClockwise(yAxisDownwards))
			{
				allSucceeded = false;
			}

			const Triangle2 clockwiseTriangleA(point2, point1, point0);
			const Triangle2 clockwiseTriangleB(point1, point0, point2);
			const Triangle2 clockwiseTriangleC(point0, point2, point1);

			if (clockwiseTriangleA.isCounterClockwise(yAxisDownwards))
			{
				allSucceeded = false;
			}

			if (clockwiseTriangleB.isCounterClockwise(yAxisDownwards))
			{
				allSucceeded = false;
			}

			if (clockwiseTriangleC.isCounterClockwise(yAxisDownwards))
			{
				allSucceeded = false;
			}
		}

		{
			// 3D world-like coordinate system (with virtual z axis pointing towards the viewer)

			const bool yAxisDownwards = false;

			const Vector3 zAxis(0, 0, 1);

			const Vector2 point0 = (Quaternion(zAxis, Numeric::angleAdjustPositive(angle0)) * Vector3(radius, 0, 0)).xy();
			const Vector2 point1 = (Quaternion(zAxis, Numeric::angleAdjustPositive(angle1)) * Vector3(radius, 0, 0)).xy();
			const Vector2 point2 = (Quaternion(zAxis, Numeric::angleAdjustPositive(angle2)) * Vector3(radius, 0, 0)).xy();

			const Triangle2 counterClockwiseTriangleA(point0, point1, point2);
			const Triangle2 counterClockwiseTriangleB(point1, point2, point0);
			const Triangle2 counterClockwiseTriangleC(point2, point0, point1);

			if (!counterClockwiseTriangleA.isCounterClockwise(yAxisDownwards))
			{
				allSucceeded = false;
			}

			if (!counterClockwiseTriangleB.isCounterClockwise(yAxisDownwards))
			{
				allSucceeded = false;
			}

			if (!counterClockwiseTriangleC.isCounterClockwise(yAxisDownwards))
			{
				allSucceeded = false;
			}

			const Triangle2 clockwiseTriangleA(point2, point1, point0);
			const Triangle2 clockwiseTriangleB(point1, point0, point2);
			const Triangle2 clockwiseTriangleC(point0, point2, point1);

			if (clockwiseTriangleA.isCounterClockwise(yAxisDownwards))
			{
				allSucceeded = false;
			}

			if (clockwiseTriangleB.isCounterClockwise(yAxisDownwards))
			{
				allSucceeded = false;
			}

			if (clockwiseTriangleC.isCounterClockwise(yAxisDownwards))
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

bool TestTriangle2::testPadded(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Pad test:";

	bool allSucceeded = true;

	constexpr unsigned int kRandomSeed = 4u;
	RandomGenerator randomGenerator(kRandomSeed);

	const Timestamp startTimestamp(true);

	// Test a few fixed cases first, then test random cases.
	// We first test an equilateral triangle centered around the origin, in which case we can simply
	// scale the triangle to achieve different paddings.
	//     2
	//    / \
	//   / . \   <- origin at the dot, equal to the triangle circumcenter
	//  0-----1
	const Scalar sqrt3 = Numeric::sqrt(3.0);
	Triangle2 equilateralTriangle(Vector2(-1, Scalar(-0.5) * sqrt3), Vector2(1, Scalar(-0.5) * sqrt3), Vector2(0, Scalar(0.5) * sqrt3));
	equilateralTriangle -= equilateralTriangle.cartesianCircumcenter(); // -= (0, -sqrt3 / 6)

	// Doubling in size.
	{
		const Scalar padAmount = sqrt3 / Scalar(3); // edge01.distance(origin) = sqrt3 / 3
		constexpr Scalar scale = 2;
		const Triangle2 groundTruthTriangle(equilateralTriangle.point0() * scale, equilateralTriangle.point1() * scale, equilateralTriangle.point2() * scale);

		const Triangle2 paddedTriangle = equilateralTriangle.padded(padAmount);
		ocean_assert(paddedTriangle.isValid());

		allSucceeded = (groundTruthTriangle.point0() == paddedTriangle.point0()) && allSucceeded;
		allSucceeded = (groundTruthTriangle.point1() == paddedTriangle.point1()) && allSucceeded;
		allSucceeded = (groundTruthTriangle.point2() == paddedTriangle.point2()) && allSucceeded;
	}

	// Halving in size.
	{
		const Scalar padAmount = -sqrt3 / Scalar(6);
		constexpr Scalar scale = Scalar(0.5);
		const Triangle2 groundTruthTriangle(equilateralTriangle.point0() * scale, equilateralTriangle.point1() * scale, equilateralTriangle.point2() * scale);

		const Triangle2 paddedTriangle = equilateralTriangle.padded(padAmount);
		ocean_assert(paddedTriangle.isValid());

		allSucceeded = (groundTruthTriangle.point0() == paddedTriangle.point0()) && allSucceeded;
		allSucceeded = (groundTruthTriangle.point1() == paddedTriangle.point1()) && allSucceeded;
		allSucceeded = (groundTruthTriangle.point2() == paddedTriangle.point2()) && allSucceeded;
	}

	// Flipping orientation (180 degree rotation plus scaling) due to the padding being greater than
	// the edge distance to the circumcenter.
	{
		const Scalar padAmount = -sqrt3;
		constexpr Scalar scale = -2.0;
		const Triangle2 groundTruthTriangle(equilateralTriangle.point0() * scale, equilateralTriangle.point1() * scale, equilateralTriangle.point2() * scale);

		const Triangle2 paddedTriangle = equilateralTriangle.padded(padAmount);
		ocean_assert(paddedTriangle.isValid());

		allSucceeded = (groundTruthTriangle.point0() == paddedTriangle.point0()) && allSucceeded;
		allSucceeded = (groundTruthTriangle.point1() == paddedTriangle.point1()) && allSucceeded;
		allSucceeded = (groundTruthTriangle.point2() == paddedTriangle.point2()) && allSucceeded;
	}

	// Degenerate case that collapses to a point.
	{
		const Scalar padAmount = -sqrt3 / Scalar(3);
		allSucceeded = !equilateralTriangle.padded(padAmount).isValid() && allSucceeded;
	}

	unsigned long long numberSuccessfulTrials = 0ull;
	unsigned long long totalNumberTrials = 0ull;

	// Random test cases.
	do
	{
		++totalNumberTrials;

		// Generate a random canonical counter-clockwise triangle from two angles.
		// The first triangle point is at (0, 0), and the second is at (1, 0).
		//
		//    Y ^
		//      |     2
		//      |    /  \
		//      |   /     \
		//      |  /        \
		//      | /           \
		//      |/ A0        A1 \     <- angles 0 and 1
		//  <---O-----------------1--->
		//      |                     X
		//      V
		//
		// We can calculate point2 as
		//   p = alpha * (cos A0, sin A0)
		//     = beta * (-cos A1, sin A1) + (1, 0) // flip across Y axis, add point 1
		// So, we can solve for alpha:
		//   p.x = alpha cos A0 = -beta cos A1 + 1
		//   p.y = alpha sin A0 = beta sin A1
		//   => beta = alpha * sin A0 / sin A1
		//   => alpha * cos A0 = -(alpha * sin A0 / sin A1) * cos A1 + 1
		//      alpha * (cos A0 + cos A1 * sin A0 / sin A1) = 1

		constexpr Scalar kMinAngle = Scalar(0.00873); // ~= 0.5 deg
		const Scalar angle0 = Random::scalar(randomGenerator, kMinAngle, Numeric::pi() - Scalar(3) * kMinAngle);
		const Scalar angle1 = Random::scalar(randomGenerator, kMinAngle, Numeric::pi() - angle0 - kMinAngle);

		ocean_assert(angle0 + angle1 < Numeric::pi());

		const Scalar cosAngle0 = Numeric::cos(angle0);
		const Scalar sinAngle0 = Numeric::sin(angle0);
		const Scalar alpha = Scalar(1) / (cosAngle0 + Numeric::cos(angle1) * sinAngle0 / Numeric::sin(angle1));

		// Create homogeneous points for subsequent transformation.
		const Vector3 hPoint0(Scalar(0), Scalar(0), Scalar(1));
		const Vector3 hPoint1(Scalar(1), Scalar(0), Scalar(1));
		const Vector3 hPoint2(alpha * cosAngle0, alpha * sinAngle0, Scalar(1));

		// Create a random 2D similarity transformation to generate a generic valid triangle.
		const SquareMatrix3 similarity = [&]()
		{
			const Scalar angle = Random::scalar(randomGenerator, -Numeric::pi(), Numeric::pi());
			const Scalar scale = Random::scalar(randomGenerator, Scalar(0.01), Scalar(10));
			const Vector2 translation = Random::vector2(randomGenerator, Scalar(-10), Scalar(10));
			return SquareMatrix3(
				scale * Numeric::cos(angle), scale * Numeric::sin(angle), Scalar(0),
				scale * -Numeric::sin(angle), scale * Numeric::cos(angle), Scalar(0),
				translation.x(), translation.y(), Scalar(1));
		}();

		const Triangle2 triangle((similarity * hPoint0).xy(), (similarity * hPoint1).xy(), (similarity * hPoint2).xy());
		ocean_assert(triangle.isValid());

		const Scalar padWidth = Random::scalar(randomGenerator, Scalar(0.001), Scalar(20)) * Random::sign(randomGenerator);

		const Triangle2 paddedTriangle = triangle.padded(padWidth);

		// Another way to compute the padded triangle t' with pad width w is to scale the original
		// triangle t around its incenter c:
		//   t' = (s + w) / s * (t - c) + c,
		// where s is the inradius of t, computed as s = area / semiperimeter.

		const Scalar d01 = Numeric::sqrt(triangle.sqrDistance01());
		const Scalar d02 = Numeric::sqrt(triangle.sqrDistance02());
		const Scalar d12 = Numeric::sqrt(triangle.sqrDistance12());

		const Scalar inradius = triangle.area() / (Scalar(0.5) * (d01 + d02 + d12));

		ocean_assert(inradius > Numeric::eps());

		const Scalar scale = Scalar(1.0) + padWidth / inradius;

		const Vector2 incenter = triangle.cartesianIncenter();
		const Triangle2 expectedTriangle(
			(triangle.point0() - incenter) * scale + incenter,
			(triangle.point1() - incenter) * scale + incenter,
			(triangle.point2() - incenter) * scale + incenter);

		bool success = true;

		for (unsigned int i = 0u; i < 3u; ++i)
		{
			// 32-bit precision is *much* worse for these tests.
			constexpr Scalar kEpsilon = std::is_same<float, Scalar>::value ? Scalar(0.01) : Numeric::weakEps();
			success = paddedTriangle[i].isEqual(expectedTriangle[i], kEpsilon) && success;
		}

		if (success)
		{
			++numberSuccessfulTrials;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	const Scalar ratioSuccessfulTrials = Scalar(numberSuccessfulTrials) / Scalar(totalNumberTrials);
	Log::info() << "Successful trial ratio: " << (ratioSuccessfulTrials * Scalar(100)) << "%.";

	constexpr Scalar kTargetSuccessfulTrialRatio = std::is_same<float, Scalar>::value ? Scalar(0.945) : Scalar(0.9999);

	allSucceeded = ratioSuccessfulTrials >= kTargetSuccessfulTrialRatio && allSucceeded;

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
