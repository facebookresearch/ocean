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

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestLine2::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Line2 test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testIsOnLine(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testIsLeftOfLine(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = decomposeNormalDistance(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Line2 test succeeded.";
	}
	else
	{
		Log::info() << "Line2 test FAILED!";
	}

	return allSucceeded;
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

#endif // OCEAN_USE_GTEST

bool TestLine2::testIsOnLine(const double testDuration)
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
			const Line2 line(Random::vector2(-range, range), Random::vector2());
			ocean_assert(Numeric::isEqual(line.direction().length(), 1));

			Vector2 perpendicular(line.direction().perpendicular());

			if (!perpendicular.normalize())
				ocean_assert(false && "This must never happen!");

			ocean_assert(line.direction() != perpendicular);
			ocean_assert(Numeric::isEqual(perpendicular.length(), 1));
			ocean_assert(Numeric::isEqualEps(line.direction() * perpendicular));

			bool localSucceeded = true;

			const Vector2 pointOnLine(line.point() + line.direction() * Random::scalar(-range * Scalar(10), range * Scalar(10)));

			if (line.isOnLine(pointOnLine) == false)
				localSucceeded = false;

			const Vector2 pointOffset(line.point() + perpendicular * Random::scalar(-range, range));

			if (line.point() != pointOffset && line.isOnLine(pointOffset) == true)
				localSucceeded = false;

			const Vector2 pointOffset2(pointOnLine + perpendicular * Random::scalar(Scalar(0.5), range) * Random::sign());

			if (line.isOnLine(pointOffset2) == true)
				localSucceeded = false;

			if (localSucceeded)
				validIterations++;

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestLine2::testIsLeftOfLine(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "isLeftOfLine test:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Scalar range = std::is_same<Scalar, float>::value ? Scalar(100) : Scalar(1000);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			bool soFarSoGood = true;

			const Line2 referenceLine(Vector2(0, 0), Vector2(0, Random::scalar(1, range)));
			ocean_assert(referenceLine.direction().length() > Numeric::eps());

			const Vector2 pointLeftOfReferenceLine(Random::scalar(-range, Scalar(-0.1)), Random::scalar(-range, range));
			const Vector2 pointRightOfReferenceLine(-pointLeftOfReferenceLine.x(), pointLeftOfReferenceLine.y());
			const Vector2 pointOnReferenceLine(referenceLine.point() + referenceLine.direction().normalized() * pointLeftOfReferenceLine.y());

			if (referenceLine.isLeftOfLine(pointLeftOfReferenceLine) != true)
				soFarSoGood = false;

			if (referenceLine.isLeftOfLine(pointRightOfReferenceLine) != false)
				soFarSoGood = false;

			if (referenceLine.isLeftOfLine(pointOnReferenceLine) != false)
				soFarSoGood = false;

			const SquareMatrix3 randomRotation = SquareMatrix3(Rotation(0, 0, 1, Random::scalar(-Numeric::pi(), Numeric::pi())));
			const SquareMatrix3 randomTranslation = SquareMatrix3(1, 0, 0, 0, 1, 0, Random::scalar(-range, range), Random::scalar(-range, range), 1);
			const SquareMatrix3 randomTransformation = randomTranslation * randomRotation;

			const Line2 transformedLine = Line2(randomTranslation * referenceLine.point(), randomRotation * referenceLine.direction());
			ocean_assert(transformedLine.direction().length() > Numeric::eps());

			const Vector2 pointLeftOfTransformedLine = randomTransformation * pointLeftOfReferenceLine;
			const Vector2 pointRightOfTransformedLine = randomTransformation * pointRightOfReferenceLine;
			const Vector2 pointOnTransformedLine = randomTransformation * pointOnReferenceLine;

			if (transformedLine.isLeftOfLine(pointLeftOfTransformedLine) != true)
				soFarSoGood = false;

			if (transformedLine.isLeftOfLine(pointRightOfTransformedLine) != false)
				soFarSoGood = false;

			if (transformedLine.isLeftOfLine(pointOnTransformedLine) != false)
				soFarSoGood = false;

			if (soFarSoGood)
				validIterations++;

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestLine2::decomposeNormalDistance(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "decomposeNormalDistance test:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		const Vector2 linePoint(Random::vector2(-10, 10));
		const Vector2 lineDirection(Random::vector2());
		ocean_assert(lineDirection.isUnit());

		const Line2 line(linePoint, lineDirection);
		ocean_assert(line.isValid());

		const bool forcePositiveDistanceValue = RandomI::random(1u) == 0u;

		const Vector3 implicitLine = line.decomposeNormalDistance(forcePositiveDistanceValue);

		bool localSucceeded = true;

		if (Numeric::isNotEqualEps(implicitLine * Vector3(linePoint, 1)))
		{
			localSucceeded = false;
		}

		if (Numeric::isNotEqualEps(implicitLine * Vector3(linePoint + lineDirection, 1)))
		{
			localSucceeded = false;
		}

		if (Numeric::isNotEqualEps(implicitLine * Vector3(linePoint - lineDirection, 1)))
		{
			localSucceeded = false;
		}

		if (forcePositiveDistanceValue && implicitLine.z() < 0)
		{
			localSucceeded = false;
		}

		const Line2 newLine(implicitLine);
		ocean_assert(newLine.isValid());

		if (Numeric::isNotWeakEqualEps(newLine.distance(linePoint)))
		{
			localSucceeded = false;
		}

		if (Numeric::isNotWeakEqualEps(newLine.distance(linePoint + lineDirection)))
		{
			localSucceeded = false;
		}

		if (Numeric::isNotWeakEqualEps(newLine.distance(linePoint - lineDirection)))
		{
			localSucceeded = false;
		}

		if (localSucceeded)
		{
			++validIterations;
		}

		++iterations;
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
