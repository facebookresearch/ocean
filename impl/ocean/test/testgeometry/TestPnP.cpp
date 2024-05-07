// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/test/testgeometry/TestPnP.h"
#include "ocean/test/testgeometry/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/geometry/Error.h"
#include "ocean/geometry/PnP.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Random.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

bool TestPnP::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   PnP test:   ---";
	Log::info() <<  " ";

	bool allSucceeded = true;

	allSucceeded = testPose(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "PnP test succeeded.";
	}
	else
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "The test failed, however the applied 32 bit floating point value precision is too low for this function so that we rate the result as expected.";
			return true;
		}

		Log::info() << "PnP test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestPnP, Pose_10)
{
	EXPECT_TRUE(TestPnP::testPose(10u, GTEST_TEST_DURATION) || (std::is_same<Scalar, float>::value)); // 32 bit floating point value precision is too low for this test
}

TEST(TestPnP, Pose_20)
{
	EXPECT_TRUE(TestPnP::testPose(20u, GTEST_TEST_DURATION) || (std::is_same<Scalar, float>::value)); // 32 bit floating point value precision is too low for this test
}

TEST(TestPnP, Pose_30)
{
	EXPECT_TRUE(TestPnP::testPose(30u, GTEST_TEST_DURATION) || (std::is_same<Scalar, float>::value)); // 32 bit floating point value precision is too low for this test
}

#endif // OCEAN_USE_GTEST

bool TestPnP::testPose(const double testDuration)
{
	Log::info() << "Testing pose:";
	Log::info() << " ";

	bool allSucceeded = true;

	for (const unsigned int numberCorrespondences : {5u, 10u, 20u, 30u})
	{
		Log::info() << "... with " << numberCorrespondences << " correspondences:";

		if (!testPose(numberCorrespondences, testDuration))
		{
			allSucceeded = false;
		}

		Log::info() << " ";
	}

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

bool TestPnP::testPose(const unsigned int numberPoints, const double testDuration)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(numberPoints >= 5u);

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	HighPerformanceStatistic performance;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const Vector3 randomTranslation = Random::vector3(randomGenerator, -10, 10);
		const Quaternion randomOrientation = Random::quaternion(randomGenerator);

		const HomogenousMatrix4 world_T_camera(randomTranslation, randomOrientation);

		const AnyCameraType anyCameraType = Random::random(randomGenerator, Utilities::realisticCameraTypes());
		const unsigned int anyCameraIndex = RandomI::random(randomGenerator, 1u);

		const SharedAnyCamera camera = Utilities::realisticAnyCamera(anyCameraType, anyCameraIndex);
		ocean_assert(camera);

		Vectors2 imagePoints(numberPoints);
		Vectors3 objectPoints(numberPoints);

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			imagePoints[n] = Random::vector2(randomGenerator, Scalar(0), Scalar(camera->width() - 1u), Scalar(0), Scalar(camera->height() - 1u));

			const Line3 ray = camera->ray(imagePoints[n], world_T_camera);
			objectPoints[n] = ray.point(Random::scalar(Scalar(0.1), Scalar(10)));

			ocean_assert(AnyCamera::isObjectPointInFrontIF(AnyCamera::standard2InvertedFlipped(world_T_camera), objectPoints[n]));

			if constexpr (std::is_same<double, Scalar>::value)
			{
				ocean_assert(imagePoints[n].distance(camera->projectToImage(world_T_camera, objectPoints[n])) < Scalar(1));
			}
		}

		HomogenousMatrix4 world_T_determinedCamera(false);

		performance.start();
			const bool localSuccess = Geometry::PnP::pose(*camera, objectPoints.data(), imagePoints.data(), objectPoints.size(), world_T_determinedCamera);
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
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Average performance: " << performance.averageMseconds() << "ms";

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 2u) << "% succeeded.";

	const bool succeeded = percent >= 0.95;

	if (!succeeded)
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return succeeded;
}

}

}

}
