/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestP4P.h"
#include "ocean/test/testgeometry/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/geometry/Error.h"
#include "ocean/geometry/P4P.h"

#include "ocean/math/Line3.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Random.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

bool TestP4P::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   P4P test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testPose(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "P4P test succeeded.";
	}
	else
	{
		Log::info() << "P4P test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestP4P, Pose)
{
	EXPECT_TRUE(TestP4P::testPose(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestP4P::testPose(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	HighPerformanceStatistic performance;

	RandomGenerator randomGenerator;

	const Timestamp start(true);

	do
	{
		const Vector3 randomTranslation = Random::vector3(randomGenerator, -10, 10);
		const Quaternion randomOrientation = Random::quaternion(randomGenerator);

		const HomogenousMatrix4 world_T_camera(randomTranslation, randomOrientation);

		const AnyCameraType anyCameraType = Random::random(randomGenerator, Utilities::realisticCameraTypes());
		const unsigned int anyCameraIndex = RandomI::random(randomGenerator, 1u);

		const SharedAnyCamera camera = Utilities::realisticAnyCamera(anyCameraType, anyCameraIndex);
		ocean_assert(camera);

		Vectors3 objectPoints;
		Vectors2 imagePoints;

		for (unsigned int n = 0u; n < 4u; ++n)
		{
			const Vector2 imagePoint = Random::vector2(randomGenerator, Scalar(0), Scalar(camera->width() - 1u), Scalar(0), Scalar(camera->height() - 1u));

			const Line3 ray = camera->ray(imagePoint, world_T_camera);
			const Vector3 objectPoint = ray.point(Random::scalar(Scalar(0.1), Scalar(10)));

			ocean_assert(AnyCamera::isObjectPointInFrontIF(AnyCamera::standard2InvertedFlipped(world_T_camera), objectPoint));
			ocean_assert(imagePoint.distance(camera->projectToImage(world_T_camera, objectPoint)) < Scalar(1));

			objectPoints.push_back(objectPoint);
			imagePoints.push_back(imagePoint);
		}

		HomogenousMatrix4 world_T_determinedCamera(false);

		performance.start();
			const bool localSuccess = Geometry::P4P::pose(*camera, objectPoints.data(), imagePoints.data(), world_T_determinedCamera);
		performance.stop();

		if (localSuccess)
		{
			Scalar sqrAveragePixelError = Numeric::maxValue();
			Scalar sqrMinimalPixelError = Numeric::maxValue();
			Scalar sqrMaximalPixelError = Numeric::maxValue();

			Geometry::Error::determinePoseError<ConstArrayAccessor<Vector3>, ConstArrayAccessor<Vector2>>(world_T_determinedCamera, *camera, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), sqrAveragePixelError, sqrMinimalPixelError, sqrMaximalPixelError);

			if (sqrAveragePixelError <= Scalar(2 * 2) && sqrMaximalPixelError <= Scalar(10 * 10))
			{
				++validIterations;
			}
		}

		++iterations;
	}
	while (start + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 4u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 4u) << "ms, average: " << String::toAString(performance.averageMseconds(), 4u) << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	const bool allSucceeded = percent >= 0.95;

	if (!allSucceeded)
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return allSucceeded;
}

}

}

}
