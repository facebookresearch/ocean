/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestNonLinearOptimizationTransformation.h"
#include "ocean/test/testgeometry/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/geometry/Error.h"
#include "ocean/geometry/NonLinearOptimizationTransformation.h"
#include "ocean/geometry/Utilities.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

bool TestNonLinearOptimizationTransformation::test(const double testDuration, Worker* /*worker*/)
{
	Log::info() << "---   Transformation non-linear optimization test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testNonLinearOptimizationObjectTransformation(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNonLinearOptimizationObjectTransformationStereo(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Transformation non-linear optimization test succeeded.";
	}
	else
	{
		Log::info() << "Transformation non-linear optimization test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestNonLinearOptimizationTransformation, NonLinearOptimizationObjectTransformation_5_20)
{
	for (const AnyCameraType anyCameraType : Utilities::realisticCameraTypes())
	{
		const std::shared_ptr<AnyCamera> camera = Utilities::realisticAnyCamera(anyCameraType);
		ocean_assert(camera);

		EXPECT_TRUE(TestNonLinearOptimizationTransformation::testNonLinearOptimizationObjectTransformation(*camera, 5u, 20u, GTEST_TEST_DURATION));
	}
}

TEST(TestNonLinearOptimizationTransformation, NonLinearOptimizationObjectTransformation_20_50)
{
	for (const AnyCameraType anyCameraType : Utilities::realisticCameraTypes())
	{
		const std::shared_ptr<AnyCamera> camera = Utilities::realisticAnyCamera(anyCameraType);
		ocean_assert(camera);

		EXPECT_TRUE(TestNonLinearOptimizationTransformation::testNonLinearOptimizationObjectTransformation(*camera, 20u, 50u, GTEST_TEST_DURATION));
	}
}

TEST(TestNonLinearOptimizationTransformation, NonLinearOptimizationObjectTransformationStereo_5_20)
{
	for (const AnyCameraType anyCameraType : Utilities::realisticCameraTypes())
	{
		const std::shared_ptr<AnyCamera> cameraA = Utilities::realisticAnyCamera(anyCameraType, 0u);
		const std::shared_ptr<AnyCamera> cameraB = Utilities::realisticAnyCamera(anyCameraType, 1u);
		ocean_assert(cameraA && cameraB);

		EXPECT_TRUE(TestNonLinearOptimizationTransformation::testNonLinearOptimizationObjectTransformationStereo(*cameraA, *cameraB, 5u, 20u, GTEST_TEST_DURATION));
	}
}

TEST(TestNonLinearOptimizationTransformation, NonLinearOptimizationObjectTransformationStereo_20_50)
{
	for (const AnyCameraType anyCameraType : Utilities::realisticCameraTypes())
	{
		const std::shared_ptr<AnyCamera> cameraA = Utilities::realisticAnyCamera(anyCameraType, 0u);
		const std::shared_ptr<AnyCamera> cameraB = Utilities::realisticAnyCamera(anyCameraType, 1u);
		ocean_assert(cameraA && cameraB);

		EXPECT_TRUE(TestNonLinearOptimizationTransformation::testNonLinearOptimizationObjectTransformationStereo(*cameraA, *cameraB, 20u, 50u, GTEST_TEST_DURATION));
	}
}

#endif // OCEAN_USE_GTEST

bool TestNonLinearOptimizationTransformation::testNonLinearOptimizationObjectTransformation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Optimization of 6-DOF any camera transformation parameters:";

	bool result = true;

	for (const AnyCameraType anyCameraType : Utilities::realisticCameraTypes())
	{
		const std::shared_ptr<AnyCamera> camera = Utilities::realisticAnyCamera(anyCameraType, RandomI::random(1u));
		ocean_assert(camera);

		Log::info() << " ";
		Log::info() << " ";
		Log::info() << "Camera name: " << camera->name();
		Log::info() << " ";

		for (const unsigned int poses : {2u, 5u, 10u, 20u, 100u})
		{
			if (poses != 2u)
			{
				Log::info() << " ";
				Log::info() << " ";
			}

			for (const unsigned int correspondences :  {10u, 20u, 50u, 100u})
			{
				if (correspondences != 10u)
				{
					Log::info() << " ";
					Log::info() << " ";
				}

				Log::info() << "Testing " << poses << " poses and " << correspondences << " correspondences:";

				result = testNonLinearOptimizationObjectTransformation(*camera, poses, correspondences, testDuration) && result;
			}
		}
	}

	return result;
}

bool TestNonLinearOptimizationTransformation::testNonLinearOptimizationObjectTransformation(const AnyCamera& camera, const unsigned int poses, const unsigned int correspondences, const double testDuration)
{
	ocean_assert(camera.isValid());
	ocean_assert(poses >= 2u && correspondences >= 5u);
	ocean_assert(testDuration > 0.0);

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		std::vector<Vectors3> objectPointGroups(poses);
		std::vector<Vectors2> imagePointGroups(poses);
		HomogenousMatrices4 world_T_cameras(poses);

		const HomogenousMatrix4 objectTransformation(Random::vector3(-5, 5), Random::quaternion());
		const HomogenousMatrix4 iObjectTransformation = objectTransformation.inverted();

		for (unsigned int n = 0u; n < poses; ++n)
		{
			Vectors3& objectPoints = objectPointGroups[n];
			Vectors2& imagePoints = imagePointGroups[n];
			HomogenousMatrix4& world_T_camera = world_T_cameras[n];

			world_T_camera = HomogenousMatrix4(Random::vector3(-5, 5), Random::quaternion());

			imagePoints.resize(correspondences);

			for (Vector2& imagePoint : imagePoints)
			{
				imagePoint = Random::vector2(Scalar(20), Scalar(camera.width() - 20u), Scalar(20), Scalar(camera.height() - 20u));

				const Vector3 objectPoint = iObjectTransformation * camera.ray(imagePoint, world_T_camera).point(Random::scalar(1, 5));
				ocean_assert(camera.projectToImage(world_T_camera, objectTransformation * objectPoint).isEqual(imagePoint, 1));

				objectPoints.push_back(objectPoint);
			}
		}

		const Vector3 faultyTranslation = Random::vector3(Scalar(-0.05), Scalar(0.05));
		const Quaternion faultyOrientation = Quaternion(Random::euler(Numeric::deg2rad(0), Numeric::deg2rad(2)));

		const HomogenousMatrix4 faultyObjectTransformation = objectTransformation * HomogenousMatrix4(faultyTranslation, faultyOrientation);

		HomogenousMatrix4 optimizedObjectTransformation;
		Scalar initialError;
		Scalar finalError;
		Scalars intermediateErrors;

		// we explicitly use a non-square estimator to improve convergence ratio
		constexpr Geometry::Estimator::EstimatorType estimatorType = Geometry::Estimator::ET_LINEAR;

		performance.start();
		if (Geometry::NonLinearOptimizationTransformation::optimizeObjectTransformation(camera, world_T_cameras, faultyObjectTransformation, objectPointGroups, imagePointGroups, optimizedObjectTransformation, 20u, estimatorType, Scalar(0.001), Scalar(5), &initialError, &finalError, &intermediateErrors))
		{
			if (finalError < Scalar(0.1))
			{
				validIterations++;
			}
		}
		performance.stop();

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";
	Log::info() << "Performance: " << performance.averageMseconds() << "ms";

	const double threshold = std::is_same<Scalar, double>::value ? 0.99 : 0.95;

	return percent >= threshold;
}

bool TestNonLinearOptimizationTransformation::testNonLinearOptimizationObjectTransformationStereo(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Optimization of 6-DOF stereo any camera transformation parameters:";

	bool result = true;

	for (const AnyCameraType anyCameraType : Utilities::realisticCameraTypes())
	{
		const std::shared_ptr<AnyCamera> cameraA = Utilities::realisticAnyCamera(anyCameraType, RandomI::random(1u));
		const std::shared_ptr<AnyCamera> cameraB = Utilities::realisticAnyCamera(anyCameraType, RandomI::random(1u));
		ocean_assert(cameraA && cameraB);

		Log::info() << " ";
		Log::info() << " ";
		Log::info() << "Camera name: " << cameraA->name();
		Log::info() << " ";

		for (const unsigned int poses : {2u, 5u, 10u, 20u, 100u})
		{
			if (poses != 2u)
			{
				Log::info() << " ";
				Log::info() << " ";
			}

			for (const unsigned int correspondences :  {10u, 20u, 50u, 100u})
			{
				if (correspondences != 10u)
				{
					Log::info() << " ";
					Log::info() << " ";
				}

				Log::info() << "Testing " << poses << " poses and " << correspondences << " correspondences:";

				result = testNonLinearOptimizationObjectTransformationStereo(*cameraA, *cameraB, poses, correspondences, testDuration) && result;
			}
		}
	}

	return result;
}

bool TestNonLinearOptimizationTransformation::testNonLinearOptimizationObjectTransformationStereo(const AnyCamera& cameraA, const AnyCamera& cameraB, const unsigned int poses, const unsigned int correspondences, const double testDuration)
{
	ocean_assert(cameraA.isValid() && cameraB.isValid());
	ocean_assert(poses >= 2u && correspondences >= 5u);
	ocean_assert(testDuration > 0.0);

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		const HomogenousMatrix4 objectTransformation(Random::vector3(-5, 5), Random::quaternion());
		const HomogenousMatrix4 iObjectTransformation = objectTransformation.inverted();

		std::vector<Vectors3> objectPointGroupsA(poses);
		std::vector<Vectors2> imagePointGroupsA(poses);
		HomogenousMatrices4 extrinsicsA(poses);

		for (unsigned int n = 0u; n < poses; ++n)
		{
			Vectors3& objectPoints = objectPointGroupsA[n];
			Vectors2& imagePoints = imagePointGroupsA[n];
			HomogenousMatrix4& extrinsic = extrinsicsA[n];

			extrinsic = HomogenousMatrix4(Random::vector3(-5, 5), Random::quaternion());

			imagePoints.resize(correspondences);

			for (Vector2& imagePoint : imagePoints)
			{
				imagePoint = Random::vector2(Scalar(20), Scalar(cameraA.width() - 20u), Scalar(20), Scalar(cameraA.height() - 20u));

				const Vector3 objectPoint = iObjectTransformation * cameraA.ray(imagePoint, extrinsic).point(Random::scalar(1, 5));
				ocean_assert(cameraA.projectToImage(extrinsic, objectTransformation * objectPoint).isEqual(imagePoint, 1));

				objectPoints.push_back(objectPoint);
			}
		}

		std::vector<Vectors3> objectPointGroupsB(poses);
		std::vector<Vectors2> imagePointGroupsB(poses);
		HomogenousMatrices4 extrinsicsB(poses);

		for (unsigned int n = 0u; n < poses; ++n)
		{
			Vectors3& objectPoints = objectPointGroupsB[n];
			Vectors2& imagePoints = imagePointGroupsB[n];
			HomogenousMatrix4& extrinsic = extrinsicsB[n];

			extrinsic = HomogenousMatrix4(Random::vector3(-5, 5), Random::quaternion());

			imagePoints.resize(correspondences);

			for (Vector2& imagePoint : imagePoints)
			{
				imagePoint = Random::vector2(Scalar(20), Scalar(cameraB.width() - 20u), Scalar(20), Scalar(cameraB.height() - 20u));

				const Vector3 objectPoint = iObjectTransformation * cameraB.ray(imagePoint, extrinsic).point(Random::scalar(1, 5));
				ocean_assert(cameraB.projectToImage(extrinsic, objectTransformation * objectPoint).isEqual(imagePoint, 1));

				objectPoints.push_back(objectPoint);
			}
		}

		const Vector3 faultyTranslation = Random::vector3(Scalar(-0.05), Scalar(0.05));
		const Quaternion faultyOrientation = Quaternion(Random::euler(Numeric::deg2rad(0), Numeric::deg2rad(2)));

		const HomogenousMatrix4 faultyObjectTransformation = objectTransformation * HomogenousMatrix4(faultyTranslation, faultyOrientation);

		HomogenousMatrix4 optimizedObjectTransformation;
		Scalar initialError;
		Scalar finalError;
		Scalars intermediateErrors;

		// we explicitly use a non-square estimator to improve convergence ratio
		constexpr Geometry::Estimator::EstimatorType estimatorType = Geometry::Estimator::ET_LINEAR;

		performance.start();
		if (Geometry::NonLinearOptimizationTransformation::optimizeObjectTransformationStereo(cameraA, cameraB, extrinsicsA, extrinsicsB, faultyObjectTransformation, objectPointGroupsA, objectPointGroupsB, imagePointGroupsA, imagePointGroupsB, optimizedObjectTransformation, 20u, estimatorType, Scalar(0.001), Scalar(5), &initialError, &finalError, &intermediateErrors))
		{
			if (finalError < Scalar(0.1))
			{
				validIterations++;
			}
		}
		performance.stop();

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";
	Log::info() << "Performance: " << performance.averageMseconds() << "ms";

	const double threshold = std::is_same<Scalar, double>::value ? 0.99 : 0.95;

	return percent >= threshold;
}

}

}

}
