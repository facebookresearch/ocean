/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestFrustum.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/Random.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestFrustum::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Frustum test");

	Log::info() << " ";

	if (selector.shouldRun("constructors"))
	{
		testResult = testConstructors(testDuration);

		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
	}

	if (selector.shouldRun("isinsidepoint"))
	{
		testResult = testIsInsidePoint(testDuration);

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrustum, Constructors)
{
	EXPECT_TRUE(TestFrustum::testConstructors(GTEST_TEST_DURATION));
}

TEST(TestFrustum, IsInsidePoint)
{
	EXPECT_TRUE(TestFrustum::testIsInsidePoint(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestFrustum::testConstructors(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Constructors test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Frustum invalidFrustum;

	OCEAN_EXPECT_FALSE(validation, invalidFrustum.isValid());

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 10u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 10u, 1080u);

		const Scalar principalX = Random::scalar(randomGenerator, Scalar(2), Scalar(width - 2u));
		const Scalar principalY = Random::scalar(randomGenerator, Scalar(2), Scalar(height - 2u));

		const Scalar fovX = Random::scalar(randomGenerator, Numeric::deg2rad(20), Numeric::deg2rad(90));

		const PinholeCamera pinholeCamera(width, height, fovX, principalX, principalY);
		ocean_assert(pinholeCamera.isValid());

		const Scalar nearDistance = Random::scalar(randomGenerator, Scalar(0.1), Scalar(1));
		const Scalar farDistance = Random::scalar(randomGenerator, Scalar(5), Scalar(50));

		const Frustum frustum(pinholeCamera, nearDistance, farDistance);

		const Frustum identityFrustum(HomogenousMatrix4(true), pinholeCamera, nearDistance, farDistance);

		OCEAN_EXPECT_TRUE(validation, frustum.isEqual(identityFrustum));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestFrustum::testIsInsidePoint(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Constructors test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 10u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 10u, 1080u);

		const Scalar principalX = Random::scalar(randomGenerator, Scalar(2), Scalar(width - 2u));
		const Scalar principalY = Random::scalar(randomGenerator, Scalar(2), Scalar(height - 2u));

		const Scalar fovX = Random::scalar(randomGenerator, Numeric::deg2rad(20), Numeric::deg2rad(90));

		const PinholeCamera pinholeCamera(width, height, fovX, principalX, principalY);
		ocean_assert(pinholeCamera.isValid());

		const Scalar nearDistance = Random::scalar(randomGenerator, Scalar(0.1), Scalar(1));
		const Scalar farDistance = Random::scalar(randomGenerator, Scalar(5), Scalar(50));

		const Frustum frustum(pinholeCamera, nearDistance, farDistance);

		// in front of frustum
		OCEAN_EXPECT_FALSE(validation, frustum.isInside(Vector3(0, 0, -nearDistance + Random::scalar(randomGenerator, Scalar(0.1), Scalar(10)))));

		// behind frustum
		OCEAN_EXPECT_FALSE(validation, frustum.isInside(Vector3(0, 0, -farDistance - Random::scalar(randomGenerator, Scalar(0.1), Scalar(10)))));

		// in the center of the frustum
		OCEAN_EXPECT_TRUE(validation, frustum.isInside(Vector3(0, 0, -Random::scalar(randomGenerator, nearDistance + Scalar(0.1), farDistance - Scalar(0.1)))));

		{
			const Scalar insideImagePointX = Random::scalar(randomGenerator, Scalar(0.1), Scalar(width) - Scalar(0.1));
			const Scalar insideImagePointY = Random::scalar(randomGenerator, Scalar(0.1), Scalar(height) - Scalar(0.1));
			const Vector2 insideImagePoint = Vector2(insideImagePointX, insideImagePointY);
			const Vector3 insideRay = pinholeCamera.vectorToPlane(insideImagePoint, Scalar(1));

			OCEAN_EXPECT_FALSE(validation, frustum.isInside(insideRay * Random::scalar(randomGenerator, -10, nearDistance - Scalar(0.1))));

			OCEAN_EXPECT_FALSE(validation, frustum.isInside(insideRay * Random::scalar(randomGenerator, farDistance + Scalar(0.1), 100)));

			OCEAN_EXPECT_TRUE(validation, frustum.isInside(insideRay * Random::scalar(randomGenerator, nearDistance + Scalar(0.1), farDistance - Scalar(0.1))));
		}

		{
			const bool xIsNegative = RandomI::boolean(randomGenerator);
			const Scalar xOutsideImagePoint = xIsNegative ? Random::scalar(randomGenerator, -100, Scalar(-0.1)) : Scalar(width) + Random::scalar(randomGenerator, Scalar(0.1), 100);

			const bool yIsNegative = RandomI::boolean(randomGenerator);
			const Scalar yOutsideImagePoint = yIsNegative ? Random::scalar(randomGenerator, -100, Scalar(-0.1)) : Scalar(height) + Random::scalar(randomGenerator, Scalar(0.1), 100);
			const Vector2 outsideImagePoint = Vector2(xOutsideImagePoint, yOutsideImagePoint);
			const Vector3 outsideRay = pinholeCamera.vectorToPlane(outsideImagePoint, Scalar(1));

			OCEAN_EXPECT_FALSE(validation, frustum.isInside(outsideRay * Random::scalar(randomGenerator, -10, farDistance + Scalar(10))));
		}

		const Vector3 randomTranslation = Random::vector3(randomGenerator, -100, 100);
		const Quaternion randomRotation = Random::quaternion(randomGenerator);
		const HomogenousMatrix4 world_T_camera(randomTranslation, randomRotation);
		const HomogenousMatrix4 camera_T_world = world_T_camera.inverted();

		const Frustum transformedFrustum(world_T_camera, pinholeCamera, nearDistance, farDistance);

		unsigned int invalidIterations = 0u;
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Vector3 worldObjectPoint = Random::vector3(randomGenerator, -100, 100);

			if (transformedFrustum.isInside(worldObjectPoint) != frustum.isInside(camera_T_world * worldObjectPoint))
			{
				++invalidIterations;
			}
		}

		OCEAN_EXPECT_LESS(validation, invalidIterations, 5u);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
