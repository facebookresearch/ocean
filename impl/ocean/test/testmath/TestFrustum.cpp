/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestFrustum.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestFrustum::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Frustum test:   ---";
	Log::info() << " ";

	allSucceeded = testConstructors(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testIsInsidePoint(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Frustum test succeeded.";
	}
	else
	{
		Log::info() << "Frustum test FAILED!";
	}

	return allSucceeded;
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

	bool allSucceeded = true;

	const Frustum invalidFrustum;

	if (invalidFrustum.isValid())
	{
		allSucceeded = false;
	}

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(10u, 1920u);
		const unsigned int height = RandomI::random(10u, 1080u);

		const Scalar principalX = Random::scalar(Scalar(2), Scalar(width - 2u));
		const Scalar principalY = Random::scalar(Scalar(2), Scalar(height - 2u));

		const Scalar fovX = Random::scalar(Numeric::deg2rad(20), Numeric::deg2rad(90));

		const PinholeCamera pinholeCamera(width, height, fovX, principalX, principalY);
		ocean_assert(pinholeCamera.isValid());

		const Scalar nearDistance = Random::scalar(Scalar(0.1), Scalar(1));
		const Scalar farDistance = Random::scalar(Scalar(5), Scalar(50));

		const Frustum frustum(pinholeCamera, nearDistance, farDistance);

		const Frustum identityFrustum(HomogenousMatrix4(true), pinholeCamera, nearDistance, farDistance);

		if (!frustum.isEqual(identityFrustum))
		{
			allSucceeded = false;
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

bool TestFrustum::testIsInsidePoint(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Constructors test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(10u, 1920u);
		const unsigned int height = RandomI::random(10u, 1080u);

		const Scalar principalX = Random::scalar(Scalar(2), Scalar(width - 2u));
		const Scalar principalY = Random::scalar(Scalar(2), Scalar(height - 2u));

		const Scalar fovX = Random::scalar(Numeric::deg2rad(20), Numeric::deg2rad(90));

		const PinholeCamera pinholeCamera(width, height, fovX, principalX, principalY);
		ocean_assert(pinholeCamera.isValid());

		const Scalar nearDistance = Random::scalar(Scalar(0.1), Scalar(1));
		const Scalar farDistance = Random::scalar(Scalar(5), Scalar(50));

		const Frustum frustum(pinholeCamera, nearDistance, farDistance);

		// in front of frustum
		if (frustum.isInside(Vector3(0, 0, -nearDistance + Random::scalar(Scalar(0.1), Scalar(10)))))
		{
			allSucceeded = false;
		}

		// behind frustum
		if (frustum.isInside(Vector3(0, 0, -farDistance - Random::scalar(Scalar(0.1), Scalar(10)))))
		{
			allSucceeded = false;
		}

		// in the center of the frustum
		if (!frustum.isInside(Vector3(0, 0, -Random::scalar(nearDistance + Scalar(0.1), farDistance - Scalar(0.1)))))
		{
			allSucceeded = false;
		}

		{
			const Vector2 insideImagePoint = Vector2(Random::scalar(Scalar(0.1), Scalar(width) - Scalar(0.1)), Random::scalar(Scalar(0.1), Scalar(height) - Scalar(0.1)));
			const Vector3 insideRay = pinholeCamera.vectorToPlane(insideImagePoint, Scalar(1));

			if (frustum.isInside(insideRay * Random::scalar(-10, nearDistance - Scalar(0.1))))
			{
				allSucceeded = false;
			}

			if (frustum.isInside(insideRay * Random::scalar(farDistance + Scalar(0.1), 100)))
			{
				allSucceeded = false;
			}

			if (!frustum.isInside(insideRay * Random::scalar(nearDistance + Scalar(0.1), farDistance - Scalar(0.1))))
			{
				allSucceeded = false;
			}
		}

		{
			const Scalar xOutsideImagePoint = RandomI::random(1u) == 0u ? Random::scalar(-100, Scalar(-0.1)) : Scalar(width) + Random::scalar(Scalar(0.1), 100);
			const Scalar yOutsideImagePoint = RandomI::random(1u) == 0u ? Random::scalar(-100, Scalar(-0.1)) : Scalar(height) + Random::scalar(Scalar(0.1), 100);
			const Vector2 outsideImagePoint = Vector2(xOutsideImagePoint, yOutsideImagePoint);
			const Vector3 outsideRay = pinholeCamera.vectorToPlane(outsideImagePoint, Scalar(1));

			if (frustum.isInside(outsideRay * Random::scalar(-10, farDistance + Scalar(10))))
			{
				allSucceeded = false;
			}
		}

		const HomogenousMatrix4 world_T_camera(Random::vector3(-100, 100), Random::quaternion());
		const HomogenousMatrix4 camera_T_world = world_T_camera.inverted();

		const Frustum transformedFrustum(world_T_camera, pinholeCamera, nearDistance, farDistance);

		unsigned int invalidIterations = 0u;
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Vector3 worldObjectPoint = Random::vector3(-100, 100);

			if (transformedFrustum.isInside(worldObjectPoint) != frustum.isInside(camera_T_world * worldObjectPoint))
			{
				++invalidIterations;
			}
		}

		if (invalidIterations >= 5u)
		{
			allSucceeded = false;
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
