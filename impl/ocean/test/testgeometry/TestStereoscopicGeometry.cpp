/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestStereoscopicGeometry.h"
#include "ocean/test/testgeometry/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/geometry/Error.h"
#include "ocean/geometry/StereoscopicGeometry.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

bool TestStereoscopicGeometry::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   StereoscopicGeometry test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testCameraPose(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "StereoscopicGeometry test succeeded.";
	}
	else
	{
		Log::info() << "StereoscopicGeometry test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestStereoscopicGeometry, CameraPose_3DOF_5)
{
	EXPECT_TRUE((TestStereoscopicGeometry::testCameraPose<true>(5u, GTEST_TEST_DURATION)));
}

TEST(TestStereoscopicGeometry, CameraPose_3DOF_10)
{
	EXPECT_TRUE((TestStereoscopicGeometry::testCameraPose<true>(10u, GTEST_TEST_DURATION)));
}

TEST(TestStereoscopicGeometry, CameraPose_3DOF_20)
{
	EXPECT_TRUE((TestStereoscopicGeometry::testCameraPose<true>(20u, GTEST_TEST_DURATION)));
}

TEST(TestStereoscopicGeometry, CameraPose_3DOF_30)
{
	EXPECT_TRUE((TestStereoscopicGeometry::testCameraPose<true>(30u, GTEST_TEST_DURATION)));
}

TEST(TestStereoscopicGeometry, CameraPose_3DOF_100)
{
	EXPECT_TRUE((TestStereoscopicGeometry::testCameraPose<true>(100u, GTEST_TEST_DURATION)));
}


TEST(TestStereoscopicGeometry, CameraPose_6DOF_5)
{
	EXPECT_TRUE((TestStereoscopicGeometry::testCameraPose<false>(5u, GTEST_TEST_DURATION)));
}

TEST(TestStereoscopicGeometry, CameraPose_6DOF_10)
{
	EXPECT_TRUE((TestStereoscopicGeometry::testCameraPose<false>(10u, GTEST_TEST_DURATION)));
}

TEST(TestStereoscopicGeometry, CameraPose_6DOF_20)
{
	EXPECT_TRUE((TestStereoscopicGeometry::testCameraPose<false>(20u, GTEST_TEST_DURATION)));
}

TEST(TestStereoscopicGeometry, CameraPose_6DOF_30)
{
	EXPECT_TRUE((TestStereoscopicGeometry::testCameraPose<false>(30u, GTEST_TEST_DURATION)));
}

TEST(TestStereoscopicGeometry, CameraPose_6DOF_100)
{
	EXPECT_TRUE((TestStereoscopicGeometry::testCameraPose<false>(100u, GTEST_TEST_DURATION)));
}

#endif // OCEAN_USE_GTEST

bool TestStereoscopicGeometry::testCameraPose(const double testDuration)
{
	Log::info() << "Testing camera pose:";
	Log::info() << " ";

	bool allSucceeded = true;

	Log::info() << "With pure rotational motion:";
	Log::info() << " ";

	for (const unsigned int numberCorrespondences : {5u, 10u, 20u, 30u, 100u})
	{
		Log::info() << "... with " << numberCorrespondences << " correspondences:";

		if (!testCameraPose<true>(numberCorrespondences, testDuration))
		{
			allSucceeded = false;
		}

		Log::info() << " ";
	}

	Log::info() << " ";
	Log::info() << "With full 6-DOF motion:";
	Log::info() << " ";

	for (const unsigned int numberCorrespondences : {5u, 10u, 20u, 30u, 100u})
	{
		Log::info() << "... with " << numberCorrespondences << " correspondences:";

		if (!testCameraPose<false>(numberCorrespondences, testDuration))
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

template <bool tPureRotation>
bool TestStereoscopicGeometry::testCameraPose(const unsigned int numberCorrespondences, const double testDuration)
{
	ocean_assert(numberCorrespondences >= 5u);
	ocean_assert(testDuration > 0.0);

	uint64_t iterations = 0ull;
	uint64_t validIerations = 0ull;

	HighPerformanceStatistic performance;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const Vector3 randomTranslation = Random::vector3(randomGenerator, -10, 10);
		const Quaternion randomOrientation = Random::quaternion(randomGenerator);

		const HomogenousMatrix4 world_T_camera(randomTranslation, randomOrientation);

		const Scalar boxDimension = Random::scalar(randomGenerator, 1, 10);

		const PinholeCamera camera = Utilities::realisticPinholeCamera(RandomI::random(randomGenerator, 1u));
		ocean_assert(camera);

		const Vectors3 objectPoints = Utilities::objectPoints(Box3(Vector3(0, 0, 0), boxDimension, boxDimension, boxDimension), size_t(numberCorrespondences), &randomGenerator);

		const Vector3 viewingDirection0 = Random::vector3(randomGenerator);

		const HomogenousMatrix4 world_T_camera0 = Utilities::viewPosition(camera, objectPoints, viewingDirection0, true);

		HomogenousMatrix4 world_T_camera1(false);

		Vectors2 imagePoints0;
		Vectors2 imagePoints1;

		while (true)
		{
			constexpr Scalar maximalRotationAngle = Numeric::deg2rad(10);

			if constexpr (tPureRotation)
			{
				world_T_camera1 = world_T_camera0 * Quaternion(Random::euler(randomGenerator, maximalRotationAngle));
			}
			else
			{
				Scalar xTranslation = Random::scalar(randomGenerator, Scalar(0.01), Scalar(0.1));
				xTranslation *= Random::sign(randomGenerator);

				Scalar yTranslation = Random::scalar(randomGenerator, Scalar(0.01), Scalar(0.1));
				yTranslation *= Random::sign(randomGenerator);

				Scalar zTranslation = Random::scalar(randomGenerator, Scalar(0), Scalar(0.01));
				zTranslation *= Random::sign(randomGenerator);

				const Vector3 translation = Vector3(xTranslation, yTranslation, zTranslation);

				const Euler euler = Random::euler(randomGenerator, maximalRotationAngle);

				world_T_camera1 = world_T_camera0 * HomogenousMatrix4(translation, euler);
			}

			imagePoints0.clear();
			imagePoints1.clear();

			bool allPointsInsideCamera = true;

			for (const Vector3& objectPoint : objectPoints)
			{
				const Vector2 imagePoint0 = camera.projectToImage<false>(world_T_camera0, objectPoint, true);
				const Vector2 imagePoint1 = camera.projectToImage<false>(world_T_camera1, objectPoint, true);

				ocean_assert(camera.isInside(imagePoint0));

				if (!camera.isInside(imagePoint1))
				{
					allPointsInsideCamera = false;
					break;
				}

				imagePoints0.push_back(imagePoint0);
				imagePoints1.push_back(imagePoint1);
			}

			if (allPointsInsideCamera)
			{
				break;
			}
		}

		HomogenousMatrix4 determinedCamera0_T_determinedCamera1(false);
		Vectors3 determinedObjectPoints;
		Indices32 validIndices;

		constexpr Scalar maxRotationalError = tPureRotation ? Scalar(1.5) : Scalar(0.1);
		constexpr Scalar maxArbitraryError = Scalar(3.5);
		constexpr Scalar rotationalMotionMinimalValidCorrespondencesPercent = tPureRotation ? Scalar(0.99) : Scalar(0.9);

		performance.start();
			bool localSuccess = Geometry::StereoscopicGeometry::cameraPose(camera, ConstArrayAccessor<Vector2>(imagePoints0), ConstArrayAccessor<Vector2>(imagePoints1), randomGenerator, determinedCamera0_T_determinedCamera1, &determinedObjectPoints, &validIndices, Numeric::sqr(maxRotationalError), Numeric::sqr(maxArbitraryError), 100u /*iterations*/, rotationalMotionMinimalValidCorrespondencesPercent);
		performance.stop();

		if (validIndices.size() != imagePoints0.size())
		{
			localSuccess = false;
		}

		if constexpr (tPureRotation)
		{
			if (localSuccess && !determinedCamera0_T_determinedCamera1.translation().isNull())
			{
				localSuccess = false;
			}
		}

		if (localSuccess)
		{
			Scalar sqrAveragePixelError = Numeric::maxValue();
			Scalar sqrMinimalPixelError = Numeric::maxValue();
			Scalar sqrMaximalPixelError = Numeric::maxValue();

			const HomogenousMatrix4 world_T_determinedCamera0(true); // the first camera is located in the origin

			bool allObjectPointsInFront = Geometry::Error::determinePoseError<ConstArrayAccessor<Vector3>, ConstArrayAccessor<Vector2>, true>(world_T_determinedCamera0, AnyCameraPinhole(camera), ConstArrayAccessor<Vector3>(determinedObjectPoints), ConstArrayAccessor<Vector2>(imagePoints0), sqrAveragePixelError, sqrMinimalPixelError, sqrMaximalPixelError);

			if (!allObjectPointsInFront || sqrAveragePixelError > Scalar(2 * 2) || sqrMaximalPixelError > Scalar(10 * 10))
			{
				localSuccess = false;
			}

			sqrAveragePixelError = Numeric::maxValue();
			sqrMinimalPixelError = Numeric::maxValue();
			sqrMaximalPixelError = Numeric::maxValue();

			const HomogenousMatrix4 world_T_determinedCamera1 = world_T_determinedCamera0 * determinedCamera0_T_determinedCamera1;

			allObjectPointsInFront = Geometry::Error::determinePoseError<ConstArrayAccessor<Vector3>, ConstArrayAccessor<Vector2>, true>(world_T_determinedCamera1, AnyCameraPinhole(camera), ConstArrayAccessor<Vector3>(determinedObjectPoints), ConstArrayAccessor<Vector2>(imagePoints1), sqrAveragePixelError, sqrMinimalPixelError, sqrMaximalPixelError);

			if (!allObjectPointsInFront || sqrAveragePixelError > Scalar(2 * 2) || sqrMaximalPixelError > Scalar(10 * 10))
			{
				localSuccess = false;
			}
		}

		if (localSuccess)
		{
			++validIerations;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Average performance: " << performance.averageMseconds() << "ms";

	ocean_assert(iterations != 0ull);
	const double percent = double(validIerations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 2u) << "% succeeded.";

	constexpr double threshold = std::is_same<double, Scalar>::value ? 0.95 : 0.85;

	return percent >= threshold;
}

}

}

}
