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

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"
#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

bool TestStereoscopicGeometry::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("StereoscopicGeometry test");

	Log::info() << " ";

	if (selector.shouldRun("camerapose"))
	{
		testResult = testCameraPose(testDuration);

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
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

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	Log::info() << "With pure rotational motion:";
	Log::info() << " ";

	for (const unsigned int numberCorrespondences : {5u, 10u, 20u, 30u, 100u})
	{
		Log::info() << "... with " << numberCorrespondences << " correspondences:";

		OCEAN_EXPECT_TRUE(validation, testCameraPose<true>(numberCorrespondences, testDuration));

		Log::info() << " ";
	}

	Log::info() << " ";
	Log::info() << "With full 6-DOF motion:";
	Log::info() << " ";

	for (const unsigned int numberCorrespondences : {5u, 10u, 20u, 30u, 100u})
	{
		Log::info() << "... with " << numberCorrespondences << " correspondences:";

		OCEAN_EXPECT_TRUE(validation, testCameraPose<false>(numberCorrespondences, testDuration));

		Log::info() << " ";
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <bool tPureRotation>
bool TestStereoscopicGeometry::testCameraPose(const unsigned int numberCorrespondences, const double testDuration)
{
	ocean_assert(numberCorrespondences >= 5u);
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	constexpr double successThreshold = std::is_same<float, Scalar>::value ? 0.85 : 0.95;

	for (const bool useGravityConstraints : {false, true})
	{
		const std::string indentation0 = "    ";

		if (useGravityConstraints)
		{
			Log::info() << indentation0 << "With gravity constraints:";
		}
		else
		{
			Log::info() << indentation0 << "No gravity constraints:";
		}

		const std::string indentation1 = indentation0 + "  ";

		ValidationPrecision validationPrecision(successThreshold, randomGenerator);

		std::vector<Scalar> gravityAngleErrors;

		HighPerformanceStatistic performance;

		const Timestamp startTimestamp(true);

		do
		{
			ValidationPrecision::ScopedIteration scopedIteration(validationPrecision);

			const Vector3 randomTranslation = Random::vector3(randomGenerator, -10, 10);
			const Quaternion randomOrientation = Random::quaternion(randomGenerator);

			const HomogenousMatrix4 world_T_camera(randomTranslation, randomOrientation);

			const Scalar boxDimension = Random::scalar(randomGenerator, 1, 10);

			const AnyCameraType anyCameraType = RandomI::random(randomGenerator, {AnyCameraType::PINHOLE, AnyCameraType::FISHEYE});
			const unsigned int cameraIndex = RandomI::random(randomGenerator, 1u);

			const SharedAnyCamera camera = Utilities::realisticAnyCamera(anyCameraType, cameraIndex);
			ocean_assert(camera);

			const Vectors3 objectPoints = Utilities::objectPoints(Box3(Vector3(0, 0, 0), boxDimension, boxDimension, boxDimension), size_t(numberCorrespondences), &randomGenerator);

			const Vector3 viewingDirection0 = Random::vector3(randomGenerator);

			const HomogenousMatrix4 world_T_camera0 = Utilities::viewPosition(*camera, objectPoints, viewingDirection0, true);

			HomogenousMatrix4 world_T_camera1(false);

			Vectors2 imagePoints0;
			Vectors2 imagePoints1;

			bool allPointsInsideCamera = true;

			for (unsigned int n = 0u; n < 1000u; ++n)
			{
				allPointsInsideCamera = true;

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

				for (const Vector3& objectPoint : objectPoints)
				{
					const Vector2 imagePoint0 = camera->projectToImage(world_T_camera0, objectPoint);
					const Vector2 imagePoint1 = camera->projectToImage(world_T_camera1, objectPoint);

					ocean_assert(camera->isInside(imagePoint0));

					if (!camera->isInside(imagePoint1))
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

			if (!allPointsInsideCamera)
			{
				scopedIteration.setInaccurate();
				continue;
			}

			Vectors3 determinedObjectPoints;
			Indices32 validIndices;

			constexpr Scalar maxRotationalError = tPureRotation ? Scalar(1.5) : Scalar(0.1);
			constexpr Scalar maxArbitraryError = Scalar(3.5);
			constexpr Scalar rotationalMotionMinimalValidCorrespondencesPercent = tPureRotation ? Scalar(0.99) : Scalar(0.9);

			const Vector3 worldGravityInWorld = Vector3(0, -1, 0);

			const Geometry::GravityConstraints gravityConstraints({world_T_camera0, world_T_camera1}, worldGravityInWorld);

			HomogenousMatrix4 world_T_determinedCamera0(false);
			HomogenousMatrix4 world_T_determinedCamera1(false);

			performance.start();
				const bool localSuccess = Geometry::StereoscopicGeometry::cameraPose(*camera, ConstArrayAccessor<Vector2>(imagePoints0), ConstArrayAccessor<Vector2>(imagePoints1), randomGenerator, world_T_determinedCamera0, world_T_determinedCamera1, gravityConstraints.conditionalPointer(useGravityConstraints), &determinedObjectPoints, &validIndices, Numeric::sqr(maxRotationalError), Numeric::sqr(maxArbitraryError), 100u /*iterations*/, rotationalMotionMinimalValidCorrespondencesPercent);
			performance.stop();

			if (localSuccess)
			{
				if (validIndices.size() != imagePoints0.size())
				{
					scopedIteration.setInaccurate();
				}
				else
				{
					const HomogenousMatrix4 determinedCamera0_T_determinedCamera1 = world_T_determinedCamera0.inverted() * world_T_determinedCamera1;

					if constexpr (tPureRotation)
					{
						if (!determinedCamera0_T_determinedCamera1.translation().isNull())
						{
							scopedIteration.setInaccurate();
						}
					}

					Scalar sqrAveragePixelError = Numeric::maxValue();
					Scalar sqrMinimalPixelError = Numeric::maxValue();
					Scalar sqrMaximalPixelError = Numeric::maxValue();

					bool allObjectPointsInFront = Geometry::Error::determinePoseError<ConstArrayAccessor<Vector3>, ConstArrayAccessor<Vector2>, true>(world_T_determinedCamera0, *camera, ConstArrayAccessor<Vector3>(determinedObjectPoints), ConstArrayAccessor<Vector2>(imagePoints0), sqrAveragePixelError, sqrMinimalPixelError, sqrMaximalPixelError);

					if (!allObjectPointsInFront || sqrAveragePixelError > Scalar(2 * 2) || sqrMaximalPixelError > Scalar(10 * 10))
					{
						scopedIteration.setInaccurate();
					}

					sqrAveragePixelError = Numeric::maxValue();
					sqrMinimalPixelError = Numeric::maxValue();
					sqrMaximalPixelError = Numeric::maxValue();

					allObjectPointsInFront = Geometry::Error::determinePoseError<ConstArrayAccessor<Vector3>, ConstArrayAccessor<Vector2>, true>(world_T_determinedCamera1, *camera, ConstArrayAccessor<Vector3>(determinedObjectPoints), ConstArrayAccessor<Vector2>(imagePoints1), sqrAveragePixelError, sqrMinimalPixelError, sqrMaximalPixelError);

					if (!allObjectPointsInFront || sqrAveragePixelError > Scalar(2 * 2) || sqrMaximalPixelError > Scalar(10 * 10))
					{
						scopedIteration.setInaccurate();
					}

					if (useGravityConstraints)
					{
						const Scalar alignmentAngle0 = gravityConstraints.alignmentAngle(world_T_determinedCamera0, 0);
						const Scalar alignmentAngle1 = gravityConstraints.alignmentAngle(world_T_determinedCamera1, 1);

						const Scalar gravityAngleError = std::max(alignmentAngle0, alignmentAngle1);

						gravityAngleErrors.push_back(Numeric::rad2deg(gravityAngleError));
					}
				}
			}
			else
			{
				scopedIteration.setInaccurate();
			}
		}
		while (validationPrecision.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

		Log::info() << indentation1 << "Performance: " << performance;

		if (useGravityConstraints)
		{
			std::sort(gravityAngleErrors.begin(), gravityAngleErrors.end());

			const Scalar gravityAngleErrorP95 = gravityAngleErrors[gravityAngleErrors.size() * 95 / 100];

			Log::info() << indentation1 << "P95 gravity error: " << String::toAString(gravityAngleErrorP95, 1u) << "deg";
		}

		Log::info() << indentation1 << "Validation: " << validationPrecision;

		OCEAN_EXPECT_TRUE(validation, validationPrecision.succeeded());
	}

	return validation.succeeded();
}

}

}

}
