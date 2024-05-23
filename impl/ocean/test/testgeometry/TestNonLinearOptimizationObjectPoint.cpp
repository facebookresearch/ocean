/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestNonLinearOptimizationObjectPoint.h"
#include "ocean/test/testgeometry/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/String.h"
#include "ocean/base/Subset.h"
#include "ocean/base/Timestamp.h"

#include "ocean/geometry/RANSAC.h"
#include "ocean/geometry/Utilities.h"

#include "ocean/math/Random.h"

#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

bool TestNonLinearOptimizationObjectPoint::test(const double testDuration, Worker* worker)
{
	Log::info() << "---   Object point non linear optimization test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testNonLinearOptimizationObjectPointsPinholeCamera(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNonLinearOptimizationObjectPointFisheyeCamera(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNonLinearOptimizationObjectPointAnyCamera(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNonLinearOptimizationObjectPointStereoAnyCamera(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNonLinearOptimizationObjectPointAnyCameras(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNonLinearOptimizationObjectPointStereoFisheyeCamera(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNonLinearOptimizationOnePoseObjectPoints(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNonLinearOptimizationTwoPosesObjectPoints(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNonLinearOptimizationPosesObjectPoints(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNonLinearOptimizationOrientationalPosesObjectPoints(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testOptimizeObjectPointRotationalPoses(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Object point non linear optimization test succeeded.";
	}
	else
	{
		Log::info() << "Object Point non linear optimization test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestNonLinearOptimizationObjectPoint, NonLinearOptimizationObjectPointsPinholeCamera_20Poses_100Points_NoNoise)
{
	const PinholeCamera pinholeCamera(1280, 720, Numeric::deg2rad(60));
	Worker worker;

	for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
	{
		for (unsigned int workerIteration = 0u; workerIteration < 2u; ++workerIteration)
		{
			EXPECT_TRUE(TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationObjectPointsPinholeCamera(pinholeCamera, 20u, 100u, GTEST_TEST_DURATION, estimatorType, Scalar(0), 0u, workerIteration == 0u ? nullptr : &worker));
		}
	}
}

TEST(TestNonLinearOptimizationObjectPoint, NonLinearOptimizationObjectPointFisheyeCamera_100Points_NoNoise)
{
	for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
	{
		EXPECT_TRUE(TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationObjectPointFisheyeCamera(100u, GTEST_TEST_DURATION, estimatorType, Scalar(0), 0u));
	}
}

TEST(TestNonLinearOptimizationObjectPoint, NonLinearOptimizationObjectPointAnyCamera_100Points_NoNoise)
{
	for (const AnyCameraType anyCameraType : Utilities::realisticCameraTypes())
	{
		const std::shared_ptr<AnyCamera> anyCamera = Utilities::realisticAnyCamera(anyCameraType, RandomI::random(1u));
		ocean_assert(anyCamera);

		for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
		{
			EXPECT_TRUE(TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationObjectPointAnyCamera(*anyCamera, 100u, GTEST_TEST_DURATION, estimatorType, Scalar(0), 0u));
		}
	}
}

TEST(TestNonLinearOptimizationObjectPoint, NonLinearOptimizationObjectPointStereoAnyCamera_100Points_NoNoise)
{
	for (const AnyCameraType anyCameraType : Utilities::realisticCameraTypes())
	{
		const std::shared_ptr<AnyCamera> anyCameraA = Utilities::realisticAnyCamera(anyCameraType, RandomI::random(1u));
		const std::shared_ptr<AnyCamera> anyCameraB = Utilities::realisticAnyCamera(anyCameraType, RandomI::random(1u));
		ocean_assert(anyCameraA && anyCameraB);

		for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
		{
			EXPECT_TRUE(TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationObjectPointStereoAnyCamera(*anyCameraA, *anyCameraB, 100u, GTEST_TEST_DURATION, estimatorType, Scalar(0), 0u));
		}
	}
}

TEST(TestNonLinearOptimizationObjectPoint, NonLinearOptimizationObjectPointAnyCameras_100Points_NoNoise)
{
	for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
	{
		EXPECT_TRUE(TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationObjectPointAnyCameras(100u, GTEST_TEST_DURATION, estimatorType, Scalar(0), 0u));
	}
}

TEST(TestNonLinearOptimizationObjectPoint, NonLinearOptimizationObjectPointStereoFisheyeCamera_100Points_NoNoise)
{
	for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
	{
		EXPECT_TRUE(TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationObjectPointStereoFisheyeCamera(100u, GTEST_TEST_DURATION, estimatorType, Scalar(0), 0u));
	}
}

TEST(TestNonLinearOptimizationObjectPoint, NonLinearOptimizationPosesObjectPoints_20Poses_20Points_NoOutliers_NoNoise)
{
	constexpr unsigned int numberPoses = 20u;
	constexpr unsigned int numberObjectPoints = 20u;

	constexpr unsigned int percentOutliers = 0u;
	constexpr Scalar noise = 0;

	for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
	{
		EXPECT_TRUE(TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationPosesObjectPoints(numberPoses, numberObjectPoints, GTEST_TEST_DURATION, estimatorType, noise, numberPoses * percentOutliers / 100u));
	}
}

TEST(TestNonLinearOptimizationObjectPoint, NonLinearOptimizationPosesObjectPoints_50Poses_50Points_NoOutliers_NoNoise)
{
	constexpr unsigned int numberPoses = 50u;
	constexpr unsigned int numberObjectPoints = 50u;

	constexpr unsigned int percentOutliers = 0u;
	constexpr Scalar noise = 0;

	for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
	{
		EXPECT_TRUE(TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationPosesObjectPoints(numberPoses, numberObjectPoints, GTEST_TEST_DURATION, estimatorType, noise, numberPoses * percentOutliers / 100u));
	}
}

TEST(TestNonLinearOptimizationObjectPoint, NonLinearOptimizationPosesObjectPoints_20Poses_20Points_Outliers_NoNoise)
{
	constexpr unsigned int numberPoses = 20u;
	constexpr unsigned int numberObjectPoints = 20u;

	constexpr unsigned int percentOutliers = 10u;
	constexpr Scalar noise = 0;

	for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
	{
		EXPECT_TRUE(TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationPosesObjectPoints(numberPoses, numberObjectPoints, GTEST_TEST_DURATION, estimatorType, noise, numberPoses * percentOutliers / 100u));
	}
}

TEST(TestNonLinearOptimizationObjectPoint, NonLinearOptimizationPosesObjectPoints_50Poses_50Points_Outliers_NoNoise)
{
	constexpr unsigned int numberPoses = 50u;
	constexpr unsigned int numberObjectPoints = 50u;

	constexpr unsigned int percentOutliers = 10u;
	constexpr Scalar noise = 0;

	for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
	{
		EXPECT_TRUE(TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationPosesObjectPoints(numberPoses, numberObjectPoints, GTEST_TEST_DURATION, estimatorType, noise, numberPoses * percentOutliers / 100u));
	}
}

TEST(TestNonLinearOptimizationObjectPoint, NonLinearOptimizationPosesObjectPoints_20Poses_20Points_Outliers_Noise)
{
	constexpr unsigned int numberPoses = 20u;
	constexpr unsigned int numberObjectPoints = 20u;

	constexpr unsigned int percentOutliers = 10u;
	constexpr Scalar noise = Scalar(1);

	for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
	{
		EXPECT_TRUE(TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationPosesObjectPoints(numberPoses, numberObjectPoints, GTEST_TEST_DURATION, estimatorType, noise, numberPoses * percentOutliers / 100u));
	}
}

TEST(TestNonLinearOptimizationObjectPoint, NonLinearOptimizationPosesObjectPoints_50Poses_50Points_Outliers_Noise)
{
	constexpr unsigned int numberPoses = 50u;
	constexpr unsigned int numberObjectPoints = 50u;

	constexpr unsigned int percentOutliers = 10u;
	constexpr Scalar noise = Scalar(1);

	for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
	{
		EXPECT_TRUE(TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationPosesObjectPoints(numberPoses, numberObjectPoints, GTEST_TEST_DURATION, estimatorType, noise, numberPoses * percentOutliers / 100u));
	}
}


TEST(TestNonLinearOptimizationObjectPoint, NonLinearOptimizationOrientationalPosesObjectPoints_20Poses_20Points_NoOutliers_NoNoise)
{
	constexpr unsigned int numberPoses = 20u;
	constexpr unsigned int numberObjectPoints = 20u;

	constexpr unsigned int percentOutliers = 0u;
	constexpr Scalar noise = 0;

	for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
	{
		EXPECT_TRUE(TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationOrientationalPosesObjectPoints(numberPoses, numberObjectPoints, GTEST_TEST_DURATION, estimatorType, noise, numberPoses * percentOutliers / 100u));
	}
}

TEST(TestNonLinearOptimizationObjectPoint, NonLinearOptimizationOrientationalPosesObjectPoints_50Poses_50Points_NoOutliers_NoNoise)
{
	constexpr unsigned int numberPoses = 50u;
	constexpr unsigned int numberObjectPoints = 50u;

	constexpr unsigned int percentOutliers = 0u;
	constexpr Scalar noise = 0;

	for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
	{
		EXPECT_TRUE(TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationOrientationalPosesObjectPoints(numberPoses, numberObjectPoints, GTEST_TEST_DURATION, estimatorType, noise, numberPoses * percentOutliers / 100u));
	}
}

TEST(TestNonLinearOptimizationObjectPoint, NonLinearOptimizationOrientationalPosesObjectPoints_20Poses_20Points_Outliers_NoNoise)
{
	constexpr unsigned int numberPoses = 20u;
	constexpr unsigned int numberObjectPoints = 20u;

	constexpr unsigned int percentOutliers = 10u;
	constexpr Scalar noise = 0;

	for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
	{
		EXPECT_TRUE(TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationOrientationalPosesObjectPoints(numberPoses, numberObjectPoints, GTEST_TEST_DURATION, estimatorType, noise, numberPoses * percentOutliers / 100u));
	}
}

TEST(TestNonLinearOptimizationObjectPoint, NonLinearOptimizationOrientationalPosesObjectPoints_50Poses_50Points_Outliers_NoNoise)
{
	constexpr unsigned int numberPoses = 50u;
	constexpr unsigned int numberObjectPoints = 50u;

	constexpr unsigned int percentOutliers = 10u;
	constexpr Scalar noise = 0;

	for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
	{
		EXPECT_TRUE(TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationOrientationalPosesObjectPoints(numberPoses, numberObjectPoints, GTEST_TEST_DURATION, estimatorType, noise, numberPoses * percentOutliers / 100u));
	}
}

TEST(TestNonLinearOptimizationObjectPoint, NonLinearOptimizationOrientationalPosesObjectPoints_20Poses_20Points_Outliers_Noise)
{
	constexpr unsigned int numberPoses = 20u;
	constexpr unsigned int numberObjectPoints = 20u;

	constexpr unsigned int percentOutliers = 10u;
	constexpr Scalar noise = Scalar(1);

	for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
	{
		EXPECT_TRUE(TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationOrientationalPosesObjectPoints(numberPoses, numberObjectPoints, GTEST_TEST_DURATION, estimatorType, noise, numberPoses * percentOutliers / 100u));
	}
}

TEST(TestNonLinearOptimizationObjectPoint, NonLinearOptimizationOrientationalPosesObjectPoints_50Poses_50Points_Outliers_Noise)
{
	constexpr unsigned int numberPoses = 50u;
	constexpr unsigned int numberObjectPoints = 50u;

	constexpr unsigned int percentOutliers = 10u;
	constexpr Scalar noise = Scalar(1);

	for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
	{
		EXPECT_TRUE(TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationOrientationalPosesObjectPoints(numberPoses, numberObjectPoints, GTEST_TEST_DURATION, estimatorType, noise, numberPoses * percentOutliers / 100u));
	}
}


TEST(TestNonLinearOptimizationObjectPoint, OptimizeObjectPointRotationalPoses_50Observations_NoOutliers_NoNoise)
{
	const PinholeCamera pinholeCamera(1280u, 720u, Numeric::deg2rad(60));

	constexpr unsigned int numberObservations = 50u;

	constexpr Scalar noise = Scalar(0);
	constexpr unsigned int percentOutliers = 0u;

	for (const bool useRoughObjectPoint : {false, true})
	{
		for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
		{
			EXPECT_TRUE(TestNonLinearOptimizationObjectPoint::testOptimizeObjectPointRotationalPoses(pinholeCamera, numberObservations, GTEST_TEST_DURATION, estimatorType, noise, numberObservations * percentOutliers / 100u, useRoughObjectPoint));
		}
	}
}

TEST(TestNonLinearOptimizationObjectPoint, OptimizeObjectPointRotationalPoses_50Observations_Outliers_NoNoise)
{
	const PinholeCamera pinholeCamera(1280u, 720u, Numeric::deg2rad(60));

	constexpr unsigned int numberObservations = 50u;

	constexpr Scalar noise = Scalar(0);
	constexpr unsigned int percentOutliers = 10u;

	for (const bool useRoughObjectPoint : {false, true})
	{
		for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
		{
			EXPECT_TRUE(TestNonLinearOptimizationObjectPoint::testOptimizeObjectPointRotationalPoses(pinholeCamera, numberObservations, GTEST_TEST_DURATION, estimatorType, noise, numberObservations * percentOutliers / 100u, useRoughObjectPoint));
		}
	}
}

TEST(TestNonLinearOptimizationObjectPoint, OptimizeObjectPointRotationalPoses_50Observations_NoOutliers_Noise)
{
	const PinholeCamera pinholeCamera(1280u, 720u, Numeric::deg2rad(60));

	constexpr unsigned int numberObservations = 50u;

	constexpr Scalar noise = Scalar(1);
	constexpr unsigned int percentOutliers = 0u;

	for (const bool useRoughObjectPoint : {false, true})
	{
		for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
		{
			EXPECT_TRUE(TestNonLinearOptimizationObjectPoint::testOptimizeObjectPointRotationalPoses(pinholeCamera, numberObservations, GTEST_TEST_DURATION, estimatorType, noise, numberObservations * percentOutliers / 100u, useRoughObjectPoint));
		}
	}
}

TEST(TestNonLinearOptimizationObjectPoint, OptimizeObjectPointRotationalPoses_50Observations_Outliers_Noise)
{
	const PinholeCamera pinholeCamera(1280u, 720u, Numeric::deg2rad(60));

	constexpr unsigned int numberObservations = 50u;

	constexpr Scalar noise = Scalar(1);
	constexpr unsigned int percentOutliers = 10u;

	for (const bool useRoughObjectPoint : {false, true})
	{
		for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
		{
			EXPECT_TRUE(TestNonLinearOptimizationObjectPoint::testOptimizeObjectPointRotationalPoses(pinholeCamera, numberObservations, GTEST_TEST_DURATION, estimatorType, noise, numberObservations * percentOutliers / 100u, useRoughObjectPoint));
		}
	}
}

#endif // OCEAN_USE_GTEST

bool TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationObjectPointsPinholeCamera(const double testDuration, Worker* worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Optimization of 3D object points with fixed 6DOF poses for pinhole camera:";
	Log::info() << " ";

	bool result = true;

	const PinholeCamera pinholeCamera(640, 480, Numeric::deg2rad(58));

	constexpr unsigned int poses = 20u;

	for (const unsigned int outlier : {0u, 10u})
	{
		if (outlier != 0u)
		{
			Log::info() << " ";
			Log::info() << " ";
		}

		for (const Scalar noise : {Scalar(0), Scalar(1)})
		{
			if (noise != Scalar(0))
			{
				Log::info() << " ";
				Log::info() << " ";
			}

			Log::info() << "Samples with Gaussian noise " << String::toAString(noise, 1u) << "px and " << outlier << "% outliers:";
			Log::info() << " ";

			for (const unsigned int objectPoints : {10u, 50u, 100u, 1000u})
			{
				if (objectPoints != 10u)
				{
					Log::info() << " ";
				}

				Log::info() << "With " << poses << " poses and " << objectPoints << " object points";

				for (Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
				{
					Log::info() << "... and " << Geometry::Estimator::translateEstimatorType(estimatorType) << ":";

					result = testNonLinearOptimizationObjectPointsPinholeCamera(pinholeCamera, poses, objectPoints, testDuration, estimatorType, noise, poses * outlier / 100u, worker) && result;
				}
			}
		}
	}

	return result;
}

bool TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationObjectPointsPinholeCamera(const PinholeCamera& patternCamera, const unsigned int numberPoses, const unsigned int numberObjectPoints, const double testDuration, const Geometry::Estimator::EstimatorType estimatorType, const Scalar standardDeviation, const unsigned int numberOutliers, Worker* worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(numberOutliers <= numberPoses);

	uint64_t iterations = 0ull;
	uint64_t succeeded = 0ull;

	Scalar averageInitialSqrError = 0;
	Scalar averageOptimizedSqrError = 0;

	HighPerformanceStatistic performance;

	Scalars medianPixelErrors;
	Scalars medianOptimizedPixelErrors;

	const Box3 objectPointsArea(Vector3(-1, -1, -1), Vector3(1, 1, 1));

	const Timestamp startTimestamp(true);

	do
	{
		// create a distorted camera
		const PinholeCamera pinholeCamera(Utilities::distortedCamera(patternCamera, true, iterations % 3ull == 1ull || iterations % 3ull == 2ull, iterations % 3ull == 2ull));

		const Quaternion orientation0(Random::quaternion());
		const Vector3 viewDirection0(orientation0 * Vector3(0, 0, -1));

		const Vectors3 perfectObjectPoints(Utilities::objectPoints(objectPointsArea, numberObjectPoints));

		HomogenousMatrices4 poses;
		poses.push_back(Utilities::viewPosition(pinholeCamera, perfectObjectPoints, viewDirection0, true));

		while (poses.size() < numberPoses)
		{
			const Quaternion offsetRotation(Random::euler(Numeric::deg2rad(5), Numeric::deg2rad(35)));

			const Quaternion newOrientation(orientation0 * offsetRotation);
			const Vector3 newViewDirection(newOrientation * Vector3(0, 0, -1));

			const Scalar angle(Numeric::rad2deg(viewDirection0.angle(newViewDirection)));
			ocean_assert_and_suppress_unused(Numeric::isInsideRange(5, angle, 85), angle);

			poses.push_back(Utilities::viewPosition(pinholeCamera, perfectObjectPoints, newViewDirection, true));
		}

		Vectors2 imagePoints;
		Vectors2 perfectImagePoints;
		Vectors3 objectPoints;

		for (unsigned int p = 0u; p < numberPoses; ++p)
		{
			const HomogenousMatrix4& pose = poses[p];

			for (unsigned int n = 0; n < numberObjectPoints; ++n)
			{
				const Vector2 imagePoint(pinholeCamera.projectToImage<true>(pose, perfectObjectPoints[n], pinholeCamera.hasDistortionParameters()));

				Vector2 imagePointNoise(0, 0);
				if (standardDeviation > 0)
				{
					imagePointNoise = Vector2(Random::gaussianNoise(standardDeviation), Random::gaussianNoise(standardDeviation));
				}

				perfectImagePoints.push_back(imagePoint);
				imagePoints.push_back(imagePoint + imagePointNoise);
			}
		}

		Box3 objectVolume(perfectObjectPoints);
		const Scalar objectDimension = objectVolume.diagonal() * Scalar(0.01);

		const HomogenousMatrices4 posesIF(PinholeCamera::standard2InvertedFlipped(poses));
		const Box2 cameraBox(-50, -50, Scalar(pinholeCamera.width()) + Scalar(50), Scalar(pinholeCamera.height()) + Scalar(50));

		for (Vectors3::const_iterator i = perfectObjectPoints.begin(); i != perfectObjectPoints.end(); ++i)
		{
			while (true)
			{
				const Vector3 randomObjectPoint(*i + Random::vector3(-objectDimension, objectDimension));

				bool valid = true;

				for (HomogenousMatrices4::const_iterator p = posesIF.begin(); p != posesIF.end(); ++p)
				{
					if ((*p * randomObjectPoint).z() <= 0)
					{
						valid = false;
						break;
					}
				}

				if (valid)
				{
					objectPoints.push_back(randomObjectPoint);
					break;
				}
			}
		}

		ocean_assert(objectPoints.size() == perfectObjectPoints.size());

		// create outliers
		for (unsigned int n = 0u; n < numberObjectPoints; ++n)
		{
			const IndexSet32 outlierSet(Utilities::randomIndices(numberPoses, numberOutliers));
			for (IndexSet32::const_iterator i = outlierSet.begin(); i != outlierSet.end(); ++i)
			{
				const Vector2 outlierNoise(Random::gaussianNoise(100), Random::gaussianNoise(100));
				imagePoints[*i * numberObjectPoints + n] += outlierNoise;
			}
		}

		const Geometry::NonLinearOptimization::ObjectPointToPoseIndexImagePointCorrespondenceAccessor objectPointToPoseIndexImagePointCorrespondenceAccessor(objectPoints.size(), ConstTemplateArrayAccessor<Vector2>(imagePoints));

#ifdef OCEAN_DEBUG
		// ensure that the ObjectPointToPoseIndexImagePointCorrespondenceAccessor object holds the correct topology
		for (size_t g = 0; g < objectPointToPoseIndexImagePointCorrespondenceAccessor.groups(); ++g)
		{
			for (size_t p = 0; p < objectPointToPoseIndexImagePointCorrespondenceAccessor.groupElements(g); ++p)
			{
				Index32 poseIndex = Index32(-1);
				Vector2 imagePoint;

				objectPointToPoseIndexImagePointCorrespondenceAccessor.element(g, p, poseIndex, imagePoint);
				ocean_assert(imagePoints[p * objectPoints.size() + g] == imagePoint);
			}
		}
#endif

		Vectors3 optimizedObjectPoints(objectPoints.size());
		NonconstArrayAccessor<Vector3> optimizedObjectPointAccessor(optimizedObjectPoints);

		performance.start();

		const bool result = Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointsForFixedPoses(pinholeCamera, ConstArrayAccessor<HomogenousMatrix4>(poses), ConstArrayAccessor<Vector3>(objectPoints), objectPointToPoseIndexImagePointCorrespondenceAccessor, pinholeCamera.hasDistortionParameters(), optimizedObjectPointAccessor, 50u, estimatorType, Scalar(0.001), Scalar(5), true, worker);
		ocean_assert(result);

		performance.stop();

		if (result)
		{
			Scalar sqrAveragePixelErrorInitial = 0;
			Scalar sqrMinimalPixelErrorInitial = 0;
			Scalar sqrMaximalPixelErrorInitial = 0;

			OCEAN_SUPPRESS_UNUSED_WARNING(sqrMinimalPixelErrorInitial);
			OCEAN_SUPPRESS_UNUSED_WARNING(sqrMaximalPixelErrorInitial);

			for (size_t p = 0; p < poses.size(); ++p)
			{
				Scalar sqrAveragePixelError = 0, sqrMinimalPixelError = 0, sqrMaximalPixelError = 0;
				Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true>(poses[p], pinholeCamera, ConstTemplateArrayAccessor<Vector3>(objectPoints), ConstTemplateArrayAccessor<Vector2>(imagePoints.data() + p * numberObjectPoints, objectPoints.size()), pinholeCamera.hasDistortionParameters(), sqrAveragePixelError, sqrMinimalPixelError, sqrMaximalPixelError);

				sqrAveragePixelErrorInitial += sqrAveragePixelError;
				sqrMinimalPixelErrorInitial += sqrMinimalPixelError;
				sqrMaximalPixelErrorInitial += sqrMaximalPixelError;
			}

			Scalar sqrAveragePixelErrorOptimized = 0;
			Scalar sqrMinimalPixelErrorOptimized = 0;
			Scalar sqrMaximalPixelErrorOptimized = 0;

			OCEAN_SUPPRESS_UNUSED_WARNING(sqrMinimalPixelErrorOptimized);
			OCEAN_SUPPRESS_UNUSED_WARNING(sqrMaximalPixelErrorOptimized);

			for (size_t p = 0; p < poses.size(); ++p)
			{
				Scalar sqrAveragePixelError = 0, sqrMinimalPixelError = 0, sqrMaximalPixelError = 0;
				Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true>(poses[p], pinholeCamera, ConstTemplateArrayAccessor<Vector3>(optimizedObjectPoints), ConstTemplateArrayAccessor<Vector2>(perfectImagePoints.data() + p * numberObjectPoints, objectPoints.size()), pinholeCamera.hasDistortionParameters(), sqrAveragePixelError, sqrMinimalPixelError, sqrMaximalPixelError);

				sqrAveragePixelErrorOptimized += sqrAveragePixelError;
				sqrMinimalPixelErrorOptimized += sqrMinimalPixelError;
				sqrMaximalPixelErrorOptimized += sqrMaximalPixelError;
			}

			averageInitialSqrError += sqrAveragePixelErrorInitial;
			averageOptimizedSqrError += sqrAveragePixelErrorOptimized;

			medianPixelErrors.push_back(sqrAveragePixelErrorInitial);
			medianOptimizedPixelErrors.push_back(sqrAveragePixelErrorOptimized);

			succeeded++;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	averageInitialSqrError /= Scalar(iterations);
	averageOptimizedSqrError /= Scalar(iterations);

	Log::info() << "Average sqr pixel error: " << String::toAString(averageInitialSqrError, 1u) << "px -> " << String::toAString(averageOptimizedSqrError, 1u) << "px";
	Log::info() << "Median sqr pixel error: " << String::toAString(Median::constMedian(medianPixelErrors.data(), medianPixelErrors.size()), 1u) << "px -> " << String::toAString(Median::constMedian(medianOptimizedPixelErrors.data(), medianOptimizedPixelErrors.size()), 1u) << "px";
	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 1u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 1u) << "ms, average: " << String::toAString(performance.averageMseconds(), 1u) << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return true;
}

bool TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationObjectPointFisheyeCamera(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Optimization of 3D object points with fixed 6DOF poses for fisheye camera:";
	Log::info() << " ";

	bool result = true;

	const unsigned int poses = 20u;

	for (const unsigned int outlier : {0u, 10u})
	{
		if (outlier != 0u)
		{
			Log::info() << " ";
			Log::info() << " ";
		}

		for (const Scalar noise : {Scalar(0), Scalar(1)})
		{
			if (noise != Scalar(0))
			{
				Log::info() << " ";
				Log::info() << " ";
			}

			Log::info() << "Testing " << poses << " poses with Gaussian noise " << String::toAString(noise, 1u) << "px and " << outlier << "% outliers:";
			Log::info() << " ";

			for (Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
			{
				Log::info() << "... and " << Geometry::Estimator::translateEstimatorType(estimatorType) << ":";

				result = testNonLinearOptimizationObjectPointFisheyeCamera(poses, testDuration, estimatorType, noise, poses * outlier / 100u) && result;
			}
		}
	}

	return result;
}

bool TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationObjectPointFisheyeCamera(const unsigned int numberPoses, const double testDuration, const Geometry::Estimator::EstimatorType estimatorType, const Scalar standardDeviation, const unsigned int numberOutliers)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(numberOutliers <= numberPoses);

	uint64_t iterations = 0ull;
	uint64_t succeeded = 0ull;

	Scalar averageInitialSqrError = 0;
	Scalar averageOptimizedSqrError = 0;

	HighPerformanceStatistic performance;

	Scalars medianInitialPixelErrors;
	Scalars medianOptimizedPixelErrors;

	const Timestamp startTimestamp(true);

	do
	{
		const FisheyeCamera fisheyeCamera = Utilities::realisticFisheyeCamera(RandomI::random(1u));

		const Vector3 objectPoint = Random::vector3(-1, 1);
		const Vector3 faultyObjectPoint = objectPoint + Random::vector3() * Random::scalar(Scalar(0.01), Scalar(0.1));

		HomogenousMatrices4 poses_world_T_camera;

		Vectors2 imagePoints;
		Vectors2 perfectImagePoints;

		while (poses_world_T_camera.size() < size_t(numberPoses))
		{
			const Line3 objectPointRay(objectPoint, Random::vector3());
			const Vector2 imagePoint = Random::vector2(Scalar(5), Scalar(fisheyeCamera.width() - 5u), Scalar(5), Scalar(fisheyeCamera.height() - 5u));
			const Scalar distance = Random::scalar(1, 5);

			const HomogenousMatrix4 world_T_camera = Geometry::Utilities::randomCameraPose(fisheyeCamera, objectPointRay, imagePoint, distance);
			ocean_assert(fisheyeCamera.projectToImage(world_T_camera, objectPoint).sqrDistance(imagePoint) < Scalar(1));

			if (!PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(world_T_camera), faultyObjectPoint))
			{
				continue;
			}

			poses_world_T_camera.push_back(world_T_camera);
			perfectImagePoints.push_back(imagePoint);

			if (Numeric::isEqualEps(standardDeviation))
			{
				imagePoints.push_back(imagePoint);
			}
			else
			{
				imagePoints.push_back(imagePoint + Vector2(Random::gaussianNoise(standardDeviation), Random::gaussianNoise(standardDeviation)));
			}
		}

		const IndexSet32 outlierSet(Utilities::randomIndices(numberPoses, numberOutliers));
		for (IndexSet32::const_iterator i = outlierSet.begin(); i != outlierSet.end(); ++i)
		{
			imagePoints[*i] = Random::vector2(Scalar(5), Scalar(fisheyeCamera.width() - 5u), Scalar(5), Scalar(fisheyeCamera.height() - 5u));
		}

		performance.start();

		Vector3 optimizedObjectPoint;
		const bool result = Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedPoses(AnyCameraFisheye(fisheyeCamera), ConstArrayAccessor<HomogenousMatrix4>(poses_world_T_camera), faultyObjectPoint, ConstArrayAccessor<Vector2>(imagePoints), optimizedObjectPoint, 50u, estimatorType, Scalar(0.001), Scalar(5), true);
		ocean_assert(result);

		performance.stop();

		if (result)
		{
			Scalar sqrPixelErrorInitial = 0;
			Scalar sqrPixelErrorOptimized = 0;

			for (size_t n = 0; n < poses_world_T_camera.size(); ++n)
			{
				sqrPixelErrorInitial += fisheyeCamera.projectToImage(poses_world_T_camera[n], faultyObjectPoint).sqrDistance(imagePoints[n]);
				sqrPixelErrorOptimized += fisheyeCamera.projectToImage(poses_world_T_camera[n], optimizedObjectPoint).sqrDistance(perfectImagePoints[n]);
			}

			ocean_assert(poses_world_T_camera.size() > 0);
			sqrPixelErrorInitial /= Scalar(poses_world_T_camera.size());
			sqrPixelErrorOptimized /= Scalar(poses_world_T_camera.size());

			averageInitialSqrError += sqrPixelErrorInitial;
			averageOptimizedSqrError += sqrPixelErrorOptimized;

			medianInitialPixelErrors.push_back(sqrPixelErrorInitial);
			medianOptimizedPixelErrors.push_back(sqrPixelErrorOptimized);

			succeeded++;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	averageInitialSqrError /= Scalar(iterations);
	averageOptimizedSqrError /= Scalar(iterations);

	Log::info() << "Average sqr pixel error: " << String::toAString(averageInitialSqrError, 1u) << "px -> " << String::toAString(averageOptimizedSqrError, 1u) << "px";
	Log::info() << "Median sqr pixel error: " << String::toAString(Median::constMedian(medianInitialPixelErrors.data(), medianInitialPixelErrors.size()), 1u) << "px -> " << String::toAString(Median::constMedian(medianOptimizedPixelErrors.data(), medianOptimizedPixelErrors.size()), 1u) << "px";
	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 2u) << "ms, average: " << String::toAString(performance.averageMseconds(), 2u) << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	if (Numeric::isEqualEps(standardDeviation) && numberOutliers == 0u)
	{
		return percent >= 0.99 && averageOptimizedSqrError < Numeric::sqr(5);
	}

	return true;
}

bool TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationObjectPointAnyCamera(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Optimization of 3D object points with fixed 6DOF poses for any camera:";
	Log::info() << " ";

	bool allSucceeded = true;

	constexpr unsigned int poses = 20u;

	for (const AnyCameraType anyCameraType : Utilities::realisticCameraTypes())
	{
		const std::shared_ptr<AnyCamera> anyCamera = Utilities::realisticAnyCamera(anyCameraType, RandomI::random(1u));
		ocean_assert(anyCamera);

		Log::info() << " ";
		Log::info() << "Camera name: " << anyCamera->name();
		Log::info() << " ";

		for (const unsigned int outlier : {0u, 10u})
		{
			if (outlier != 0u)
			{
				Log::info() << " ";
				Log::info() << " ";
			}

			for (const Scalar noise : {Scalar(0), Scalar(1)})
			{
				if (noise != Scalar(0))
				{
					Log::info() << " ";
					Log::info() << " ";
				}

				Log::info() << "Testing " << poses << " poses with Gaussian noise " << String::toAString(noise, 1u) << "px and " << outlier << "% outliers:";
				Log::info() << " ";

				for (Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
				{
					Log::info() << "... and " << Geometry::Estimator::translateEstimatorType(estimatorType) << ":";

					allSucceeded = testNonLinearOptimizationObjectPointAnyCamera(*anyCamera, poses, testDuration, estimatorType, noise, poses * outlier / 100u) && allSucceeded;
				}
			}
		}
	}

	return allSucceeded;
}

bool TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationObjectPointAnyCamera(const AnyCamera& anyCamera, const unsigned int numberPoses, const double testDuration, const Geometry::Estimator::EstimatorType estimatorType, const Scalar standardDeviation, const unsigned int numberOutliers)
{
	ocean_assert(anyCamera.isValid());

	ocean_assert(testDuration > 0.0);
	ocean_assert(numberOutliers <= numberPoses);

	uint64_t iterations = 0ull;
	uint64_t succeeded = 0ull;

	Scalar averageInitialSqrError = 0;
	Scalar averageOptimizedSqrError = 0;

	HighPerformanceStatistic performance;

	Scalars medianInitialPixelErrors;
	Scalars medianOptimizedPixelErrors;

	const Timestamp startTimestamp(true);

	do
	{
		const Vector3 objectPoint = Random::vector3(-1, 1);
		const Vector3 faultyObjectPoint = objectPoint + Random::vector3() * Random::scalar(Scalar(0.01), Scalar(0.1));

		HomogenousMatrices4 world_T_cameras;

		Vectors2 imagePoints;
		Vectors2 perfectImagePoints;

		while (world_T_cameras.size() < size_t(numberPoses))
		{
			const Line3 objectPointRay(objectPoint, Random::vector3());
			const Vector2 imagePoint = Random::vector2(Scalar(5), Scalar(anyCamera.width() - 5u), Scalar(5), Scalar(anyCamera.height() - 5u));
			const Scalar distance = Random::scalar(1, 5);

			const HomogenousMatrix4 world_T_camera = Geometry::Utilities::randomCameraPose(anyCamera, objectPointRay, imagePoint, distance);
			ocean_assert(anyCamera.projectToImage(world_T_camera, objectPoint).sqrDistance(imagePoint) < Scalar(1));

			if (!PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(world_T_camera), faultyObjectPoint))
			{
				continue;
			}

			world_T_cameras.push_back(world_T_camera);
			perfectImagePoints.push_back(imagePoint);

			if (Numeric::isEqualEps(standardDeviation))
			{
				imagePoints.push_back(imagePoint);
			}
			else
			{
				imagePoints.push_back(imagePoint + Vector2(Random::gaussianNoise(standardDeviation), Random::gaussianNoise(standardDeviation)));
			}
		}

		const IndexSet32 outlierSet(Utilities::randomIndices(numberPoses, numberOutliers));
		for (IndexSet32::const_iterator i = outlierSet.begin(); i != outlierSet.end(); ++i)
		{
			imagePoints[*i] = Random::vector2(Scalar(5), Scalar(anyCamera.width() - 5u), Scalar(5), Scalar(anyCamera.height() - 5u));
		}

		performance.start();

		Vector3 optimizedObjectPoint;
		const bool result = Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedPoses(anyCamera, ConstArrayAccessor<HomogenousMatrix4>(world_T_cameras), faultyObjectPoint, ConstArrayAccessor<Vector2>(imagePoints), optimizedObjectPoint, 50u, estimatorType, Scalar(0.001), Scalar(5), true);
		ocean_assert(result);

		performance.stop();

		if (result)
		{
			Scalar sqrPixelErrorInitial = 0;
			Scalar sqrPixelErrorOptimized = 0;

			for (size_t n = 0; n < world_T_cameras.size(); ++n)
			{
				sqrPixelErrorInitial += anyCamera.projectToImage(world_T_cameras[n], faultyObjectPoint).sqrDistance(imagePoints[n]);
				sqrPixelErrorOptimized += anyCamera.projectToImage(world_T_cameras[n], optimizedObjectPoint).sqrDistance(perfectImagePoints[n]);
			}

			ocean_assert(world_T_cameras.size() > 0);
			sqrPixelErrorInitial /= Scalar(world_T_cameras.size());
			sqrPixelErrorOptimized /= Scalar(world_T_cameras.size());

			averageInitialSqrError += sqrPixelErrorInitial;
			averageOptimizedSqrError += sqrPixelErrorOptimized;

			medianInitialPixelErrors.push_back(sqrPixelErrorInitial);
			medianOptimizedPixelErrors.push_back(sqrPixelErrorOptimized);

			succeeded++;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	averageInitialSqrError /= Scalar(iterations);
	averageOptimizedSqrError /= Scalar(iterations);

	Log::info() << "Average sqr pixel error: " << String::toAString(averageInitialSqrError, 1u) << "px -> " << String::toAString(averageOptimizedSqrError, 1u) << "px";
	Log::info() << "Median sqr pixel error: " << String::toAString(Median::constMedian(medianInitialPixelErrors.data(), medianInitialPixelErrors.size()), 1u) << "px -> " << String::toAString(Median::constMedian(medianOptimizedPixelErrors.data(), medianOptimizedPixelErrors.size()), 1u) << "px";
	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 2u) << "ms, average: " << String::toAString(performance.averageMseconds(), 2u) << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	if (Numeric::isEqualEps(standardDeviation) && numberOutliers == 0u)
	{
		return percent >= 0.99 && averageOptimizedSqrError < Numeric::sqr(5);
	}

	return true;
}

bool TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationObjectPointStereoAnyCamera(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Optimization of 3D object points with fixed 6DOF poses for any stereo camera:";
	Log::info() << " ";

	bool result = true;

	constexpr unsigned int poses = 20u;

	for (const AnyCameraType anyCameraType : Utilities::realisticCameraTypes())
	{
		const std::shared_ptr<AnyCamera> anyCameraA = Utilities::realisticAnyCamera(anyCameraType, RandomI::random(1u));
		const std::shared_ptr<AnyCamera> anyCameraB = Utilities::realisticAnyCamera(anyCameraType, RandomI::random(1u));
		ocean_assert(anyCameraA && anyCameraB);

		Log::info() << " ";
		Log::info() << "Camera name: " << anyCameraA->name();
		Log::info() << " ";

		for (const unsigned int outlier : {0u, 10u})
		{
			if (outlier != 0u)
			{
				Log::info() << " ";
				Log::info() << " ";
			}

			for (const Scalar noise : {Scalar(0), Scalar(1)})
			{
				if (noise != Scalar(0))
				{
					Log::info() << " ";
					Log::info() << " ";
				}

				Log::info() << "Testing " << poses << " poses with Gaussian noise " << String::toAString(noise, 1u) << "px and " << outlier << "% outliers:";
				Log::info() << " ";

				for (Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
				{
					Log::info() << "... and " << Geometry::Estimator::translateEstimatorType(estimatorType) << ":";

					result = testNonLinearOptimizationObjectPointStereoAnyCamera(*anyCameraA, *anyCameraB, poses, testDuration, estimatorType, noise, poses * outlier / 100u) && result;
				}
			}
		}
	}

	return result;
}

bool TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationObjectPointStereoAnyCamera(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const unsigned int numberPoses, const double testDuration, const Geometry::Estimator::EstimatorType estimatorType, const Scalar standardDeviation, const unsigned int numberOutliers)
{
	ocean_assert(anyCameraA.isValid() && anyCameraB.isValid());
	ocean_assert(testDuration > 0.0);
	ocean_assert(numberOutliers <= numberPoses);

	uint64_t iterations = 0ull;
	uint64_t succeeded = 0ull;

	Scalar averageInitialSqrError = 0;
	Scalar averageOptimizedSqrError = 0;

	HighPerformanceStatistic performance;

	Scalars medianInitialPixelErrors;
	Scalars medianOptimizedPixelErrors;

	const Timestamp startTimestamp(true);

	do
	{
		const Vector3 objectPoint = Random::vector3(-1, 1);
		const Vector3 faultyObjectPoint = objectPoint + Random::vector3() * Random::scalar(Scalar(0.01), Scalar(0.1));

		HomogenousMatrices4 world_T_camerasA;
		HomogenousMatrices4 world_T_camerasB;

		const unsigned int numberPosesA = RandomI::random(1u, numberPoses - 1u);
		const unsigned int numberPosesB = numberPoses - numberPosesA;
		ocean_assert(numberPosesA < numberPoses && numberPosesB < numberPoses && numberPosesA + numberPosesB == numberPoses);

		// let's determine random image points and random poses for the first stereo images

		Vectors2 imagePointsA;
		Vectors2 perfectImagePointsA;

		while (world_T_camerasA.size() < size_t(numberPosesA))
		{
			const Line3 objectPointRay(objectPoint, Random::vector3());
			const Vector2 imagePoint = Random::vector2(Scalar(5), Scalar(anyCameraA.width() - 5u), Scalar(5), Scalar(anyCameraA.height() - 5u));
			const Scalar distance = Random::scalar(1, 5);

			const HomogenousMatrix4 world_T_cameraA = Geometry::Utilities::randomCameraPose(anyCameraA, objectPointRay, imagePoint, distance);
			ocean_assert(anyCameraA.projectToImage(world_T_cameraA, objectPoint).sqrDistance(imagePoint) < Scalar(1));

			if (!PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(world_T_cameraA), faultyObjectPoint))
			{
				continue;
			}

			world_T_camerasA.push_back(world_T_cameraA);
			perfectImagePointsA.push_back(imagePoint);

			if (Numeric::isEqualEps(standardDeviation))
			{
				imagePointsA.push_back(imagePoint);
			}
			else
			{
				imagePointsA.push_back(imagePoint + Vector2(Random::gaussianNoise(standardDeviation), Random::gaussianNoise(standardDeviation)));
			}
		}

		// let's determine random image points and random poses for the second stereo images

		Vectors2 imagePointsB;
		Vectors2 perfectImagePointsB;

		while (world_T_camerasB.size() < size_t(numberPosesB))
		{
			const Line3 objectPointRay(objectPoint, Random::vector3());
			const Vector2 imagePoint = Random::vector2(Scalar(5), Scalar(anyCameraB.width() - 5u), Scalar(5), Scalar(anyCameraB.height() - 5u));
			const Scalar distance = Random::scalar(1, 5);

			const HomogenousMatrix4 world_T_cameraB = Geometry::Utilities::randomCameraPose(anyCameraB, objectPointRay, imagePoint, distance);
			ocean_assert(anyCameraB.projectToImage(world_T_cameraB, objectPoint).sqrDistance(imagePoint) < Scalar(1));

			if (!PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(world_T_cameraB), faultyObjectPoint))
			{
				continue;
			}

			world_T_camerasB.push_back(world_T_cameraB);
			perfectImagePointsB.push_back(imagePoint);

			if (Numeric::isEqualEps(standardDeviation))
			{
				imagePointsB.push_back(imagePoint);
			}
			else
			{
				imagePointsB.push_back(imagePoint + Vector2(Random::gaussianNoise(standardDeviation), Random::gaussianNoise(standardDeviation)));
			}
		}

		// let's create some outliers

		unsigned int numberOutliersA = RandomI::random(0u, std::min(numberOutliers, numberPosesA));
		unsigned int numberOutliersB = numberOutliers - numberOutliersA;
		if (numberOutliersB > numberPosesB)
		{
			numberOutliersA += numberOutliersB - numberPosesB;
			numberOutliersB = numberPosesB;
		}
		ocean_assert(numberOutliersA <= numberPosesA && numberOutliersB <= numberPosesB && numberOutliersA + numberOutliersB == numberOutliers);

		const IndexSet32 outlierSetA(Utilities::randomIndices(numberPosesA, numberOutliersA));
		for (IndexSet32::const_iterator i = outlierSetA.begin(); i != outlierSetA.end(); ++i)
		{
			imagePointsA[*i] = Random::vector2(Scalar(5), Scalar(anyCameraA.width() - 5u), Scalar(5), Scalar(anyCameraA.height() - 5u));
		}

		const IndexSet32 outlierSetB(Utilities::randomIndices(numberPosesB, numberOutliersB));
		for (IndexSet32::const_iterator i = outlierSetB.begin(); i != outlierSetB.end(); ++i)
		{
			imagePointsB[*i] = Random::vector2(Scalar(5), Scalar(anyCameraB.width() - 5u), Scalar(5), Scalar(anyCameraB.height() - 5u));
		}

		performance.start();

		Vector3 optimizedObjectPoint;
		const bool result = Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedStereoPoses(anyCameraA, anyCameraB, ConstArrayAccessor<HomogenousMatrix4>(world_T_camerasA), ConstArrayAccessor<HomogenousMatrix4>(world_T_camerasB), faultyObjectPoint, ConstArrayAccessor<Vector2>(imagePointsA), ConstArrayAccessor<Vector2>(imagePointsB), optimizedObjectPoint, 50u, estimatorType, Scalar(0.001), Scalar(5), true);
		ocean_assert(result);

		performance.stop();

		if (result)
		{
			Scalar sqrPixelErrorInitial = 0;
			Scalar sqrPixelErrorOptimized = 0;

			for (size_t n = 0; n < world_T_camerasA.size(); ++n)
			{
				sqrPixelErrorInitial += anyCameraA.projectToImage(world_T_camerasA[n], faultyObjectPoint).sqrDistance(imagePointsA[n]);
				sqrPixelErrorOptimized += anyCameraA.projectToImage(world_T_camerasA[n], optimizedObjectPoint).sqrDistance(perfectImagePointsA[n]);
			}

			for (size_t n = 0; n < world_T_camerasB.size(); ++n)
			{
				sqrPixelErrorInitial += anyCameraB.projectToImage(world_T_camerasB[n], faultyObjectPoint).sqrDistance(imagePointsB[n]);
				sqrPixelErrorOptimized += anyCameraB.projectToImage(world_T_camerasB[n], optimizedObjectPoint).sqrDistance(perfectImagePointsB[n]);
			}

			ocean_assert(world_T_camerasA.size() + world_T_camerasB.size() > 0);
			sqrPixelErrorInitial /= Scalar(world_T_camerasA.size() + world_T_camerasB.size());
			sqrPixelErrorOptimized /= Scalar(world_T_camerasA.size() + world_T_camerasB.size());

			averageInitialSqrError += sqrPixelErrorInitial;
			averageOptimizedSqrError += sqrPixelErrorOptimized;

			medianInitialPixelErrors.push_back(sqrPixelErrorInitial);
			medianOptimizedPixelErrors.push_back(sqrPixelErrorOptimized);

			succeeded++;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	averageInitialSqrError /= Scalar(iterations);
	averageOptimizedSqrError /= Scalar(iterations);

	Log::info() << "Average sqr pixel error: " << String::toAString(averageInitialSqrError, 1u) << "px -> " << String::toAString(averageOptimizedSqrError, 1u) << "px";
	Log::info() << "Median sqr pixel error: " << String::toAString(Median::constMedian(medianInitialPixelErrors.data(), medianInitialPixelErrors.size()), 1u) << "px -> " << String::toAString(Median::constMedian(medianOptimizedPixelErrors.data(), medianOptimizedPixelErrors.size()), 1u) << "px";
	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 2u) << "ms, average: " << String::toAString(performance.averageMseconds(), 2u) << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	if (Numeric::isEqualEps(standardDeviation) && numberOutliers == 0u)
	{
		return percent >= 0.99 && averageOptimizedSqrError < Numeric::sqr(5);
	}

	return true;
}

bool TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationObjectPointAnyCameras(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Optimization of 3D object points with fixed 6DOF poses for any cameras:";
	Log::info() << " ";

	bool allSucceeded = true;

	constexpr unsigned int poses = 20u;

	for (const unsigned int outlier : {0u, 10u})
	{
		if (outlier != 0u)
		{
			Log::info() << " ";
			Log::info() << " ";
		}

		for (const Scalar noise : {Scalar(0), Scalar(1)})
		{
			if (noise != Scalar(0))
			{
				Log::info() << " ";
				Log::info() << " ";
			}

			Log::info() << "Testing " << poses << " poses with Gaussian noise " << String::toAString(noise, 1u) << "px and " << outlier << "% outliers:";
			Log::info() << " ";

			for (Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
			{
				Log::info() << "... and " << Geometry::Estimator::translateEstimatorType(estimatorType) << ":";

				allSucceeded = testNonLinearOptimizationObjectPointAnyCameras(poses, testDuration, estimatorType, noise, poses * outlier / 100u) && allSucceeded;
			}
		}
	}

	return allSucceeded;
}

bool TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationObjectPointAnyCameras(const unsigned int numberPoses, const double testDuration, const Geometry::Estimator::EstimatorType estimatorType, const Scalar standardDeviation, const unsigned int numberOutliers)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(numberOutliers <= numberPoses);

	uint64_t iterations = 0ull;
	uint64_t succeeded = 0ull;

	Scalar averageInitialSqrError = 0;
	Scalar averageOptimizedSqrError = 0;

	HighPerformanceStatistic performance;

	Scalars medianInitialPixelErrors;
	Scalars medianOptimizedPixelErrors;

	const Timestamp startTimestamp(true);

	const std::vector<AnyCameraType> anyCameraTypes = {AnyCameraType::PINHOLE, AnyCameraType::FISHEYE};

	do
	{
		const Vector3 objectPoint = Random::vector3(-1, 1);
		const Vector3 faultyObjectPoint = objectPoint + Random::vector3() * Random::scalar(Scalar(0.01), Scalar(0.1));

		SharedAnyCameras cameras;
		HomogenousMatrices4 world_T_cameras;

		Vectors2 imagePoints;
		Vectors2 perfectImagePoints;

		while (world_T_cameras.size() < size_t(numberPoses))
		{
			SharedAnyCamera anyCamera = Utilities::realisticAnyCamera(Random::random(anyCameraTypes), RandomI::random(1u));
			ocean_assert(anyCamera);

			const Line3 objectPointRay(objectPoint, Random::vector3());
			const Vector2 imagePoint = Random::vector2(Scalar(5), Scalar(anyCamera->width() - 5u), Scalar(5), Scalar(anyCamera->height() - 5u));
			const Scalar distance = Random::scalar(1, 5);

			const HomogenousMatrix4 world_T_camera = Geometry::Utilities::randomCameraPose(*anyCamera, objectPointRay, imagePoint, distance);
			ocean_assert(anyCamera->projectToImage(world_T_camera, objectPoint).sqrDistance(imagePoint) < Scalar(1));

			if (!PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(world_T_camera), faultyObjectPoint))
			{
				continue;
			}

			world_T_cameras.push_back(world_T_camera);
			perfectImagePoints.push_back(imagePoint);

			if (Numeric::isEqualEps(standardDeviation))
			{
				imagePoints.push_back(imagePoint);
			}
			else
			{
				imagePoints.push_back(imagePoint + Vector2(Random::gaussianNoise(standardDeviation), Random::gaussianNoise(standardDeviation)));
			}

			cameras.emplace_back(std::move(anyCamera));
		}

		ocean_assert(cameras.size() == imagePoints.size());

		const IndexSet32 outlierSet(Utilities::randomIndices(numberPoses, numberOutliers));
		for (const Index32& outlierIndex : outlierSet)
		{
			const SharedAnyCamera& camera = cameras[outlierIndex];
			imagePoints[outlierIndex] = Random::vector2(Scalar(5), Scalar(camera->width() - 5u), Scalar(5), Scalar(camera->height() - 5u));
		}

		performance.start();

		Vector3 optimizedObjectPoint;
		const bool result = Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedPoses(SharedPointerConstArrayAccessor<AnyCamera>(cameras), ConstArrayAccessor<HomogenousMatrix4>(world_T_cameras), faultyObjectPoint, ConstArrayAccessor<Vector2>(imagePoints), optimizedObjectPoint, 50u, estimatorType, Scalar(0.001), Scalar(5), true);
		ocean_assert(result);

		performance.stop();

		if (result)
		{
			Scalar sqrPixelErrorInitial = 0;
			Scalar sqrPixelErrorOptimized = 0;

			for (size_t n = 0; n < world_T_cameras.size(); ++n)
			{
				const SharedAnyCamera& anyCamera = cameras[n];

				sqrPixelErrorInitial += anyCamera->projectToImage(world_T_cameras[n], faultyObjectPoint).sqrDistance(imagePoints[n]);
				sqrPixelErrorOptimized += anyCamera->projectToImage(world_T_cameras[n], optimizedObjectPoint).sqrDistance(perfectImagePoints[n]);
			}

			ocean_assert(world_T_cameras.size() > 0);
			sqrPixelErrorInitial /= Scalar(world_T_cameras.size());
			sqrPixelErrorOptimized /= Scalar(world_T_cameras.size());

			averageInitialSqrError += sqrPixelErrorInitial;
			averageOptimizedSqrError += sqrPixelErrorOptimized;

			medianInitialPixelErrors.push_back(sqrPixelErrorInitial);
			medianOptimizedPixelErrors.push_back(sqrPixelErrorOptimized);

			succeeded++;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	averageInitialSqrError /= Scalar(iterations);
	averageOptimizedSqrError /= Scalar(iterations);

	Log::info() << "Average sqr pixel error: " << String::toAString(averageInitialSqrError, 1u) << "px -> " << String::toAString(averageOptimizedSqrError, 1u) << "px";
	Log::info() << "Median sqr pixel error: " << String::toAString(Median::constMedian(medianInitialPixelErrors.data(), medianInitialPixelErrors.size()), 1u) << "px -> " << String::toAString(Median::constMedian(medianOptimizedPixelErrors.data(), medianOptimizedPixelErrors.size()), 1u) << "px";
	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 2u) << "ms, average: " << String::toAString(performance.averageMseconds(), 2u) << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	if (Numeric::isEqualEps(standardDeviation) && numberOutliers == 0u)
	{
		return percent >= 0.99 && averageOptimizedSqrError < Numeric::sqr(5);
	}

	return true;
}

bool TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationObjectPointStereoFisheyeCamera(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Optimization of 3D object points with fixed 6DOF poses for stereo fisheye camera:";
	Log::info() << " ";

	bool result = true;

	const unsigned int poses = 20u;

	for (const unsigned int outlier : {0u, 10u})
	{
		if (outlier != 0u)
		{
			Log::info() << " ";
			Log::info() << " ";
		}

		for (const Scalar noise : {Scalar(0), Scalar(1)})
		{
			if (noise != Scalar(0))
			{
				Log::info() << " ";
				Log::info() << " ";
			}

			Log::info() << "Testing " << poses << " poses with Gaussian noise " << String::toAString(noise, 1u) << "px and " << outlier << "% outliers:";
			Log::info() << " ";

			for (Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
			{
				Log::info() << "... and " << Geometry::Estimator::translateEstimatorType(estimatorType) << ":";

				result = testNonLinearOptimizationObjectPointStereoFisheyeCamera(poses, testDuration, estimatorType, noise, poses * outlier / 100u) && result;
			}
		}
	}

	return result;
}

bool TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationObjectPointStereoFisheyeCamera(const unsigned int numberPoses, const double testDuration, const Geometry::Estimator::EstimatorType estimatorType, const Scalar standardDeviation, const unsigned int numberOutliers)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(numberOutliers <= numberPoses);

	uint64_t iterations = 0ull;
	uint64_t succeeded = 0ull;

	Scalar averageInitialSqrError = 0;
	Scalar averageOptimizedSqrError = 0;

	HighPerformanceStatistic performance;

	Scalars medianInitialPixelErrors;
	Scalars medianOptimizedPixelErrors;

	const Timestamp startTimestamp(true);

	do
	{
		unsigned int cameraIndexA = (unsigned int)(-1);
		unsigned int cameraIndexB = (unsigned int)(-1);
		RandomI::random(1u, cameraIndexA, cameraIndexB);

		const FisheyeCamera fisheyeCameraA = Utilities::realisticFisheyeCamera(cameraIndexA);
		const FisheyeCamera fisheyeCameraB = Utilities::realisticFisheyeCamera(cameraIndexB);

		const Vector3 objectPoint = Random::vector3(-1, 1);
		const Vector3 faultyObjectPoint = objectPoint + Random::vector3() * Random::scalar(Scalar(0.01), Scalar(0.1));

		HomogenousMatrices4 world_T_camerasA;
		HomogenousMatrices4 world_T_camerasB;

		const unsigned int numberPosesA = RandomI::random(1u, numberPoses - 1u);
		const unsigned int numberPosesB = numberPoses - numberPosesA;
		ocean_assert(numberPosesA < numberPoses && numberPosesB < numberPoses && numberPosesA + numberPosesB == numberPoses);

		// let's determine random image points and random poses for the first stereo images

		Vectors2 imagePointsA;
		Vectors2 perfectImagePointsA;

		while (world_T_camerasA.size() < size_t(numberPosesA))
		{
			const Line3 objectPointRay(objectPoint, Random::vector3());
			const Vector2 imagePoint = Random::vector2(Scalar(5), Scalar(fisheyeCameraA.width() - 5u), Scalar(5), Scalar(fisheyeCameraA.height() - 5u));
			const Scalar distance = Random::scalar(1, 5);

			const HomogenousMatrix4 world_T_cameraA = Geometry::Utilities::randomCameraPose(fisheyeCameraA, objectPointRay, imagePoint, distance);
			ocean_assert(fisheyeCameraA.projectToImage(world_T_cameraA, objectPoint).sqrDistance(imagePoint) < Scalar(1));

			if (!PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(world_T_cameraA), faultyObjectPoint))
			{
				continue;
			}

			world_T_camerasA.push_back(world_T_cameraA);
			perfectImagePointsA.push_back(imagePoint);

			if (Numeric::isEqualEps(standardDeviation))
			{
				imagePointsA.push_back(imagePoint);
			}
			else
			{
				imagePointsA.push_back(imagePoint + Vector2(Random::gaussianNoise(standardDeviation), Random::gaussianNoise(standardDeviation)));
			}
		}

		// let's determine random image points and random poses for the second stereo images

		Vectors2 imagePointsB;
		Vectors2 perfectImagePointsB;

		while (world_T_camerasB.size() < size_t(numberPosesB))
		{
			const Line3 objectPointRay(objectPoint, Random::vector3());
			const Vector2 imagePoint = Random::vector2(Scalar(5), Scalar(fisheyeCameraB.width() - 5u), Scalar(5), Scalar(fisheyeCameraB.height() - 5u));
			const Scalar distance = Random::scalar(1, 5);

			const HomogenousMatrix4 world_T_cameraB = Geometry::Utilities::randomCameraPose(fisheyeCameraB, objectPointRay, imagePoint, distance);
			ocean_assert(fisheyeCameraB.projectToImage(world_T_cameraB, objectPoint).sqrDistance(imagePoint) < Scalar(1));

			if (!PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(world_T_cameraB), faultyObjectPoint))
			{
				continue;
			}

			world_T_camerasB.push_back(world_T_cameraB);
			perfectImagePointsB.push_back(imagePoint);

			if (Numeric::isEqualEps(standardDeviation))
			{
				imagePointsB.push_back(imagePoint);
			}
			else
			{
				imagePointsB.push_back(imagePoint + Vector2(Random::gaussianNoise(standardDeviation), Random::gaussianNoise(standardDeviation)));
			}
		}

		// let's create some outliers

		unsigned int numberOutliersA = RandomI::random(0u, std::min(numberOutliers, numberPosesA));
		unsigned int numberOutliersB = numberOutliers - numberOutliersA;
		if (numberOutliersB > numberPosesB)
		{
			numberOutliersA += numberOutliersB - numberPosesB;
			numberOutliersB = numberPosesB;
		}
		ocean_assert(numberOutliersA <= numberPosesA && numberOutliersB <= numberPosesB && numberOutliersA + numberOutliersB == numberOutliers);

		const IndexSet32 outlierSetA(Utilities::randomIndices(numberPosesA, numberOutliersA));
		for (IndexSet32::const_iterator i = outlierSetA.begin(); i != outlierSetA.end(); ++i)
		{
			imagePointsA[*i] = Random::vector2(Scalar(5), Scalar(fisheyeCameraA.width() - 5u), Scalar(5), Scalar(fisheyeCameraA.height() - 5u));
		}

		const IndexSet32 outlierSetB(Utilities::randomIndices(numberPosesB, numberOutliersB));
		for (IndexSet32::const_iterator i = outlierSetB.begin(); i != outlierSetB.end(); ++i)
		{
			imagePointsB[*i] = Random::vector2(Scalar(5), Scalar(fisheyeCameraB.width() - 5u), Scalar(5), Scalar(fisheyeCameraB.height() - 5u));
		}

		performance.start();

		Vector3 optimizedObjectPoint;
		const bool result = Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedStereoPoses(AnyCameraFisheye(fisheyeCameraA), AnyCameraFisheye(fisheyeCameraB), ConstArrayAccessor<HomogenousMatrix4>(world_T_camerasA), ConstArrayAccessor<HomogenousMatrix4>(world_T_camerasB), faultyObjectPoint, ConstArrayAccessor<Vector2>(imagePointsA), ConstArrayAccessor<Vector2>(imagePointsB), optimizedObjectPoint, 50u, estimatorType, Scalar(0.001), Scalar(5), true);
		ocean_assert(result);

		performance.stop();

		if (result)
		{
			Scalar sqrPixelErrorInitial = 0;
			Scalar sqrPixelErrorOptimized = 0;

			for (size_t n = 0; n < world_T_camerasA.size(); ++n)
			{
				sqrPixelErrorInitial += fisheyeCameraA.projectToImage(world_T_camerasA[n], faultyObjectPoint).sqrDistance(imagePointsA[n]);
				sqrPixelErrorOptimized += fisheyeCameraA.projectToImage(world_T_camerasA[n], optimizedObjectPoint).sqrDistance(perfectImagePointsA[n]);
			}

			for (size_t n = 0; n < world_T_camerasB.size(); ++n)
			{
				sqrPixelErrorInitial += fisheyeCameraB.projectToImage(world_T_camerasB[n], faultyObjectPoint).sqrDistance(imagePointsB[n]);
				sqrPixelErrorOptimized += fisheyeCameraB.projectToImage(world_T_camerasB[n], optimizedObjectPoint).sqrDistance(perfectImagePointsB[n]);
			}

			ocean_assert(world_T_camerasA.size() + world_T_camerasB.size() > 0);
			sqrPixelErrorInitial /= Scalar(world_T_camerasA.size() + world_T_camerasB.size());
			sqrPixelErrorOptimized /= Scalar(world_T_camerasA.size() + world_T_camerasB.size());

			averageInitialSqrError += sqrPixelErrorInitial;
			averageOptimizedSqrError += sqrPixelErrorOptimized;

			medianInitialPixelErrors.push_back(sqrPixelErrorInitial);
			medianOptimizedPixelErrors.push_back(sqrPixelErrorOptimized);

			succeeded++;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	averageInitialSqrError /= Scalar(iterations);
	averageOptimizedSqrError /= Scalar(iterations);

	Log::info() << "Average sqr pixel error: " << String::toAString(averageInitialSqrError, 1u) << "px -> " << String::toAString(averageOptimizedSqrError, 1u) << "px";
	Log::info() << "Median sqr pixel error: " << String::toAString(Median::constMedian(medianInitialPixelErrors.data(), medianInitialPixelErrors.size()), 1u) << "px -> " << String::toAString(Median::constMedian(medianOptimizedPixelErrors.data(), medianOptimizedPixelErrors.size()), 1u) << "px";
	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 2u) << "ms, average: " << String::toAString(performance.averageMseconds(), 2u) << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	if (Numeric::isEqualEps(standardDeviation) && numberOutliers == 0u)
	{
		return percent >= 0.99 && averageOptimizedSqrError < Numeric::sqr(5);
	}

	return true;
}

bool TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationOnePoseObjectPoints(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Optimization of one 6DOF camera pose and several 3D object point positions:";
	Log::info() << " ";

	bool result = true;
	const PinholeCamera pinholeCamera(640, 480, Numeric::deg2rad(58));

	const unsigned int poses = 20u;

	for (const unsigned int outlier : {0u, 10u})
	{
		if (outlier != 0u)
		{
			Log::info() << " ";
			Log::info() << " ";
		}

		for (const Scalar noise : {Scalar(0), Scalar(1)})
		{
			if (noise != Scalar(0))
			{
				Log::info() << " ";
				Log::info() << " ";
			}

			Log::info() << "Samples with Gaussian noise " << String::toAString(noise, 1u) << "px and " << outlier << "% outliers:";
			Log::info() << " ";

			for (const unsigned int objectPoints : {10u, 50u, 100u, 1000u})
			{
				if (objectPoints != 10u)
				{
					Log::info() << " ";
				}

				Log::info() << "With " << poses << " poses and " << objectPoints << " object points";

				for (const bool useCovariances : {false}) // currently no covariances for this test
				{
					if (noise == 0u && useCovariances)
					{
						continue;
					}

					if (useCovariances)
					{
						Log::info() << " ";
					}

					if (useCovariances)
					{
						Log::info() << "... using covariances";
					}
					else
					{
						Log::info() << "... no covariances";
					}

					for (Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
					{
						Log::info() << "... and " << Geometry::Estimator::translateEstimatorType(estimatorType) << ":";

						result = testNonLinearOptimizationOnePoseObjectPoints(pinholeCamera, objectPoints, testDuration, estimatorType, noise, objectPoints * outlier / 100u, useCovariances) && result;
					}
				}
			}
		}
	}

	return result;
}

bool TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationOnePoseObjectPoints(const PinholeCamera& patternCamera, const unsigned int numberObjectPoints, const double testDuration, const Geometry::Estimator::EstimatorType estimatorType, const Scalar standardDeviation, const unsigned int numberOutliers, const bool useCovariances)
{
	ocean_assert(testDuration > 0.0);

	uint64_t iterations = 0ull;
	uint64_t succeeded = 0ull;

	Scalar averageInitialSqrError = 0;
	Scalar averageOptimizedSqrError = 0;

	HighPerformanceStatistic performance;

	Scalars medianPixelErrors;
	Scalars medianOptimizedPixelErrors;

	const Box3 objectPointsArea(Vector3(-1, -1, -1), Vector3(1, 1, 1));

	const Timestamp startTimestamp(true);

	do
	{
		// create a distorted camera
		const PinholeCamera pinholeCamera(Utilities::distortedCamera(patternCamera, true, iterations % 3ull == 1ull || iterations % 3ull == 2ull, iterations % 3ull == 2ull));

		const Quaternion orientation0(Random::quaternion());
		const Vector3 viewDirection0(orientation0 * Vector3(0, 0, -1));

		const Vectors3 perfectObjectPoints(Utilities::objectPoints(objectPointsArea, numberObjectPoints));

		const Quaternion offsetRotation(Random::euler(Numeric::deg2rad(5), Numeric::deg2rad(35)));
		const Quaternion orientation1(orientation0 * offsetRotation);
		const Vector3 viewDirection1(orientation1 * Vector3(0, 0, -1));
		const Scalar angle(Numeric::rad2deg(viewDirection0.angle(viewDirection1)));
		ocean_assert_and_suppress_unused(Numeric::isInsideRange(5, angle, 85), angle);

		const HomogenousMatrix4 pose0(Utilities::viewPosition(pinholeCamera, perfectObjectPoints, viewDirection0, true));
		const HomogenousMatrix4 pose1(Utilities::viewPosition(pinholeCamera, perfectObjectPoints, viewDirection1, true));

		const HomogenousMatrix4 poseIF0(PinholeCamera::standard2InvertedFlipped(pose0));

		Vectors2 imagePoints0;
		Vectors2 imagePoints1;

		Vectors2 perfectImagePoints0;
		Vectors2 perfectImagePoints1;

		Vectors3 objectPoints;

		Matrix invertedCovariances0(numberObjectPoints * 2u, 2u);
		Matrix invertedCovariances1(numberObjectPoints * 2u, 2u);

		for (unsigned int n = 0; n < numberObjectPoints; ++n)
		{
			const Vector2 imagePoint(pinholeCamera.projectToImage<true>(pose0, perfectObjectPoints[n], pinholeCamera.hasDistortionParameters()));

			Vector2 imagePointNoise(0, 0);
			if (standardDeviation > 0)
			{
				imagePointNoise = Vector2(Random::gaussianNoise(standardDeviation), Random::gaussianNoise(standardDeviation));

				if (useCovariances)
				{
					const SquareMatrix2 covariance(Geometry::Utilities::covarianceMatrix(imagePointNoise, standardDeviation));
					const SquareMatrix2 invertedCovariance(covariance.inverted());

					invertedCovariance.copyElements(invertedCovariances0[2u * n], false);
				}
			}
			else if (useCovariances)
			{
				SquareMatrix2(true).copyElements(invertedCovariances0[2u * n], false);
			}

			perfectImagePoints0.push_back(imagePoint);
			imagePoints0.push_back(imagePoint + imagePointNoise);
		}

		for (unsigned int n = 0u; n < numberObjectPoints; ++n)
		{
			const Vector2 imagePoint(pinholeCamera.projectToImage<true>(pose1, perfectObjectPoints[n], pinholeCamera.hasDistortionParameters()));

			Vector2 imagePointNoise(0, 0);
			if (standardDeviation > 0)
			{
				imagePointNoise = Vector2(Random::gaussianNoise(standardDeviation), Random::gaussianNoise(standardDeviation));

				if (useCovariances)
				{
					const SquareMatrix2 covariance(Geometry::Utilities::covarianceMatrix(imagePointNoise, standardDeviation));
					const SquareMatrix2 invertedCovariance(covariance.inverted());

					invertedCovariance.copyElements(invertedCovariances1[2u * n], false);
				}
			}
			else if (useCovariances)
			{
				SquareMatrix2(true).copyElements(invertedCovariances1[2u * n], false);
			}

			perfectImagePoints1.push_back(imagePoint);
			imagePoints1.push_back(imagePoint + imagePointNoise);
		}

		const Vector3 errorTranslation1(Random::vector3(Scalar(-0.1), Scalar(0.1)));
		const Euler errorEuler1(Random::euler(Numeric::deg2rad(10)));
		const Quaternion errorRotation1(errorEuler1);
		const Vector3 faultyTranslation1(pose1.translation() + errorTranslation1);
		const Quaternion faultyRotation1(pose1.rotation() * errorRotation1);

		const HomogenousMatrix4 faultyPose1(faultyTranslation1, faultyRotation1);
		const HomogenousMatrix4 faultyPoseIF1(PinholeCamera::standard2InvertedFlipped(faultyPose1));

		Box3 objectVolume(perfectObjectPoints);
		const Scalar objectDimension = objectVolume.diagonal() * Scalar(0.05);
		const Box2 cameraBox(-50, -50, Scalar(pinholeCamera.width()) + Scalar(50), Scalar(pinholeCamera.height()) + Scalar(50));

		for (Vectors3::const_iterator i = perfectObjectPoints.begin(); i != perfectObjectPoints.end(); ++i)
		{
			unsigned int randomSteps = 0u;

			while (randomSteps++ < 100u)
			{
				const Vector3 randomObjectPoint(*i + Random::vector3(-objectDimension, objectDimension));

				if ((poseIF0 * randomObjectPoint).z() <= 0)
					continue;

				if ((faultyPoseIF1 * randomObjectPoint).z() <= 0)
					continue;

				objectPoints.push_back(randomObjectPoint);
				break;
			}

			if (randomSteps >= 100u)
				break;
		}

		if (objectPoints.size() != perfectObjectPoints.size())
			continue;

		ocean_assert(objectPoints.size() == perfectObjectPoints.size());

		const IndexSet32 outlierSet(Utilities::randomIndices(numberObjectPoints, numberOutliers));
		for (IndexSet32::const_iterator i = outlierSet.begin(); i != outlierSet.end(); ++i)
		{
			imagePoints0[*i] += Vector2(Random::gaussianNoise(100), Random::gaussianNoise(100));
			imagePoints1[*i] += Vector2(Random::gaussianNoise(100), Random::gaussianNoise(100));
		}

		Vectors3 optimizedObjectPoints(objectPoints.size());
		HomogenousMatrix4 optimizedPose1;

		performance.start();

		NonconstArrayAccessor<Vector3> optimizedObjectPointsAccessor(optimizedObjectPoints);
		const bool result = Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointsAndOnePose(pinholeCamera, pose0, faultyPose1, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints0), ConstArrayAccessor<Vector2>(imagePoints1), pinholeCamera.hasDistortionParameters(), &optimizedPose1, &optimizedObjectPointsAccessor, 50u, estimatorType, Scalar(0.001), Scalar(5), true);
		ocean_assert(result);

		performance.stop();

		if (result)
		{
			Scalar sqrAveragePixelErrorInitial = 0;
			Scalar sqrMinimalPixelErrorInitial = 0;
			Scalar sqrMaximalPixelErrorInitial = 0;

			OCEAN_SUPPRESS_UNUSED_WARNING(sqrMinimalPixelErrorInitial);
			OCEAN_SUPPRESS_UNUSED_WARNING(sqrMaximalPixelErrorInitial);

			{
				Scalar sqrAveragePixelError = 0, sqrMinimalPixelError = 0, sqrMaximalPixelError = 0;

				Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true>(pose0, pinholeCamera, ConstTemplateArrayAccessor<Vector3>(objectPoints), ConstTemplateArrayAccessor<Vector2>(imagePoints0), pinholeCamera.hasDistortionParameters(), sqrAveragePixelError, sqrMinimalPixelError, sqrMaximalPixelError);
				sqrAveragePixelErrorInitial += sqrAveragePixelError;
				sqrMinimalPixelErrorInitial += sqrMinimalPixelError;
				sqrMaximalPixelErrorInitial += sqrMaximalPixelError;

				Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true>(pose1, pinholeCamera, ConstTemplateArrayAccessor<Vector3>(objectPoints), ConstTemplateArrayAccessor<Vector2>(imagePoints1), pinholeCamera.hasDistortionParameters(), sqrAveragePixelError, sqrMinimalPixelError, sqrMaximalPixelError);
				sqrAveragePixelErrorInitial += sqrAveragePixelError;
				sqrMinimalPixelErrorInitial += sqrMinimalPixelError;
				sqrMaximalPixelErrorInitial += sqrMaximalPixelError;
			}

			const Scalar sqrAverageObjectPointErrorInitial = Geometry::Error::determineAverageError(objectPoints, optimizedObjectPoints);
			OCEAN_SUPPRESS_UNUSED_WARNING(sqrAverageObjectPointErrorInitial);

			Scalar sqrAveragePixelErrorOptimized = 0;
			Scalar sqrMinimalPixelErrorOptimized = 0;
			Scalar sqrMaximalPixelErrorOptimized = 0;

			OCEAN_SUPPRESS_UNUSED_WARNING(sqrMinimalPixelErrorOptimized);
			OCEAN_SUPPRESS_UNUSED_WARNING(sqrMaximalPixelErrorOptimized);

			const Vectors2 outlierFreeImagePoints0(Subset::invertedSubset(perfectImagePoints0, outlierSet));
			const Vectors2 outlierFreeImagePoints1(Subset::invertedSubset(perfectImagePoints1, outlierSet));
			const Vectors3 outlierFreeObjectPoints(Subset::invertedSubset(perfectObjectPoints, outlierSet));
			const Vectors3 outlierFreeOptimizedObjectPoints(Subset::invertedSubset(optimizedObjectPoints, outlierSet));

			{
				Scalar sqrAveragePixelError = 0, sqrMinimalPixelError = 0, sqrMaximalPixelError = 0;

				Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true>(pose0, pinholeCamera, ConstTemplateArrayAccessor<Vector3>(outlierFreeOptimizedObjectPoints), ConstTemplateArrayAccessor<Vector2>(outlierFreeImagePoints0), pinholeCamera.hasDistortionParameters(), sqrAveragePixelError, sqrMinimalPixelError, sqrMaximalPixelError);
				sqrAveragePixelErrorOptimized += sqrAveragePixelError;
				sqrMinimalPixelErrorOptimized += sqrMinimalPixelError;
				sqrMaximalPixelErrorOptimized += sqrMaximalPixelError;

				Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true>(optimizedPose1, pinholeCamera, ConstTemplateArrayAccessor<Vector3>(outlierFreeOptimizedObjectPoints), ConstTemplateArrayAccessor<Vector2>(outlierFreeImagePoints1), pinholeCamera.hasDistortionParameters(), sqrAveragePixelError, sqrMinimalPixelError, sqrMaximalPixelError);
				sqrAveragePixelErrorOptimized += sqrAveragePixelError;
				sqrMinimalPixelErrorOptimized += sqrMinimalPixelError;
				sqrMaximalPixelErrorOptimized += sqrMaximalPixelError;
			}

			const Scalar sqrAverageObjectPointError = Geometry::Error::determineAverageError(outlierFreeObjectPoints, outlierFreeOptimizedObjectPoints);
			OCEAN_SUPPRESS_UNUSED_WARNING(sqrAverageObjectPointError);

			averageInitialSqrError += sqrAveragePixelErrorInitial;
			averageOptimizedSqrError += sqrAveragePixelErrorOptimized;

			medianPixelErrors.push_back(sqrAveragePixelErrorInitial);
			medianOptimizedPixelErrors.push_back(sqrAveragePixelErrorOptimized);

			succeeded++;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	averageInitialSqrError /= Scalar(iterations);
	averageOptimizedSqrError /= Scalar(iterations);

	Log::info() << "Average sqr pixel error: " << String::toAString(averageInitialSqrError, 1u) << "px -> " << String::toAString(averageOptimizedSqrError, 1u) << "px";
	Log::info() << "Median sqr pixel error: " << String::toAString(Median::constMedian(medianPixelErrors.data(), medianPixelErrors.size()), 1u) << "px -> " << String::toAString(Median::constMedian(medianOptimizedPixelErrors.data(), medianOptimizedPixelErrors.size()), 1u) << "px";
	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 2u) << "ms, average: " << String::toAString(performance.averageMseconds(), 2u) << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return true;
}

bool TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationTwoPosesObjectPoints(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Optimization of two 6DOF camera poses and several 3D object point positions:";
	Log::info() << " ";

	bool result = true;
	const PinholeCamera pinholeCamera(640, 480, Numeric::deg2rad(58));

	for (const unsigned int outlier : {0u, 10u})
	{
		if (outlier != 0u)
		{
			Log::info() << " ";
			Log::info() << " ";
		}

		for (const Scalar noise : {Scalar(0), Scalar(1)})
		{
			if (noise != 0u)
			{
				Log::info() << " ";
				Log::info() << " ";
			}

			Log::info() << "Samples with Gaussian noise " << String::toAString(noise, 1u) << "px and " << outlier << "% outliers:";
			Log::info() << " ";

			for (const unsigned int objectPoints : {10u, 50u, 100u, 1000u})
			{
				if (objectPoints != 10u)
				{
					Log::info() << " ";
				}

				Log::info() << "With " << objectPoints << " object points";

				for (const bool useCovariances : {false, true})
				{
					if (noise == 0u && useCovariances)
					{
						continue;
					}

					if (useCovariances)
					{
						Log::info() << " ";
						Log::info() << "... using covariances";
					}
					else
					{
						Log::info() << "... no covariances";
					}

					for (Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
					{
						result = testNonLinearOptimizationTwoPosesObjectPoints(pinholeCamera, objectPoints, testDuration, estimatorType, noise, objectPoints * outlier / 100u, useCovariances) && result;
					}
				}
			}
		}
	}

	return result;
}

bool TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationTwoPosesObjectPoints(const PinholeCamera& patternCamera, const unsigned int numberObjectPoints, const double testDuration, const Geometry::Estimator::EstimatorType estimatorType, const Scalar standardDeviation, const unsigned int numberOutliers, const bool useCovariances)
{
	ocean_assert(testDuration > 0.0);

	uint64_t iterations = 0ull;
	uint64_t succeeded = 0ull;

	Scalar averageInitialSqrError = 0;
	Scalar averageOptimizedSqrError = 0;

	HighPerformanceStatistic performance;

	Scalars medianPixelErrors;
	Scalars medianOptimizedPixelErrors;

	const Box3 objectPointsArea(Vector3(-1, -1, -1), Vector3(1, 1, 1));

	const Timestamp startTimestamp(true);

	do
	{
		// create a distorted camera
		const PinholeCamera pinholeCamera(Utilities::distortedCamera(patternCamera, true, iterations % 3ull == 1ull || iterations % 3ull == 2ull, iterations % 3ull == 2ull));

		const Quaternion orientation0(Random::quaternion());
		const Vector3 viewDirection0(orientation0 * Vector3(0, 0, -1));

		const Vectors3 perfectObjectPoints(Utilities::objectPoints(objectPointsArea, numberObjectPoints));

		const Quaternion offsetRotation(Random::euler(Numeric::deg2rad(5), Numeric::deg2rad(35)));
		const Quaternion orientation1(orientation0 * offsetRotation);
		const Vector3 viewDirection1(orientation1 * Vector3(0, 0, -1));
		const Scalar angle(Numeric::rad2deg(viewDirection0.angle(viewDirection1)));
		ocean_assert_and_suppress_unused(Numeric::isInsideRange(5, angle, 85), angle);

		const HomogenousMatrix4 pose0(Utilities::viewPosition(pinholeCamera, perfectObjectPoints, viewDirection0, true));
		const HomogenousMatrix4 pose1(Utilities::viewPosition(pinholeCamera, perfectObjectPoints, viewDirection1, true));

		const HomogenousMatrix4 poseIF0(PinholeCamera::standard2InvertedFlipped(pose0));

		Vectors2 imagePoints0;
		Vectors2 imagePoints1;

		Vectors2 perfectImagePoints0;
		Vectors2 perfectImagePoints1;

		Vectors3 objectPoints;

		Matrix invertedCovariances0(numberObjectPoints * 2u, 2u);
		Matrix invertedCovariances1(numberObjectPoints * 2u, 2u);

		for (unsigned int n = 0u; n < numberObjectPoints; ++n)
		{
			const Vector2 imagePoint(pinholeCamera.projectToImage<true>(pose0, perfectObjectPoints[n], pinholeCamera.hasDistortionParameters()));

			Vector2 imagePointNoise(0, 0);
			if (standardDeviation > 0)
			{
				imagePointNoise = Vector2(Random::gaussianNoise(standardDeviation), Random::gaussianNoise(standardDeviation));

				if (useCovariances)
				{
					const SquareMatrix2 covariance(Geometry::Utilities::covarianceMatrix(imagePointNoise, standardDeviation));
					const SquareMatrix2 invertedCovariance(covariance.inverted());

					invertedCovariance.copyElements(invertedCovariances0[2u * n], false);
				}
			}
			else if (useCovariances)
			{
				SquareMatrix2(true).copyElements(invertedCovariances0[2u * n], false);
			}

			perfectImagePoints0.push_back(imagePoint);
			imagePoints0.push_back(imagePoint + imagePointNoise);
		}

		for (unsigned int n = 0; n < numberObjectPoints; ++n)
		{
			const Vector2 imagePoint(pinholeCamera.projectToImage<true>(pose1, perfectObjectPoints[n], pinholeCamera.hasDistortionParameters()));

			Vector2 imagePointNoise(0, 0);
			if (standardDeviation > 0)
			{
				imagePointNoise = Vector2(Random::gaussianNoise(standardDeviation), Random::gaussianNoise(standardDeviation));

				if (useCovariances)
				{
					const SquareMatrix2 covariance(Geometry::Utilities::covarianceMatrix(imagePointNoise, standardDeviation));
					const SquareMatrix2 invertedCovariance(covariance.inverted());

					invertedCovariance.copyElements(invertedCovariances1[2u * n], false);
				}
			}
			else if (useCovariances)
			{
				SquareMatrix2(true).copyElements(invertedCovariances1[2u * n], false);
			}

			perfectImagePoints1.push_back(imagePoint);
			imagePoints1.push_back(imagePoint + imagePointNoise);
		}

		const Vector3 errorTranslation0(Random::vector3(Scalar(-0.1), Scalar(0.1)));
		const Euler errorEuler0(Random::euler(Numeric::deg2rad(10)));
		const Quaternion errorRotation0(errorEuler0);
		const Vector3 faultyTranslation0(pose0.translation() + errorTranslation0);
		const Quaternion faultyRotation0(pose0.rotation() * errorRotation0);

		const HomogenousMatrix4 faultyPose0(faultyTranslation0, faultyRotation0);

		const Vector3 errorTranslation1(Random::vector3(Scalar(-0.1), Scalar(0.1)));
		const Euler errorEuler1(Random::euler(Numeric::deg2rad(10)));
		const Quaternion errorRotation1(errorEuler1);
		const Vector3 faultyTranslation1(pose1.translation() + errorTranslation1);
		const Quaternion faultyRotation1(pose1.rotation() * errorRotation1);

		const HomogenousMatrix4 faultyPose1(faultyTranslation1, faultyRotation1);
		const HomogenousMatrix4 faultyPoseIF1(PinholeCamera::standard2InvertedFlipped(faultyPose1));

		Box3 objectVolume(perfectObjectPoints);
		const Scalar objectDimension = objectVolume.diagonal() * Scalar(0.01);
		const Box2 cameraBox(-50, -50, Scalar(pinholeCamera.width()) + Scalar(50), Scalar(pinholeCamera.height()) + Scalar(50));


		for (Vectors3::const_iterator i = perfectObjectPoints.begin(); i != perfectObjectPoints.end(); ++i)
		{
			unsigned int randomSteps = 0u;

			while (randomSteps++ < 100u)
			{
				const Vector3 randomObjectPoint(*i + Random::vector3(-objectDimension, objectDimension));

				if ((poseIF0 * randomObjectPoint).z() <= 0)
					continue;

				if ((faultyPoseIF1 * randomObjectPoint).z() <= 0)
					continue;

				objectPoints.push_back(randomObjectPoint);
				break;
			}

			if (randomSteps >= 100u)
				break;
		}

		if (objectPoints.size() != perfectObjectPoints.size())
			continue;

		ocean_assert(objectPoints.size() == perfectObjectPoints.size());

		const IndexSet32 outlierSet(Utilities::randomIndices(numberObjectPoints, numberOutliers));
		for (IndexSet32::const_iterator i = outlierSet.begin(); i != outlierSet.end(); ++i)
		{
			imagePoints0[*i] += Vector2(Random::gaussianNoise(100), Random::gaussianNoise(100));
			imagePoints1[*i] += Vector2(Random::gaussianNoise(100), Random::gaussianNoise(100));
		}

		Vectors3 optimizedObjectPoints(objectPoints.size());
		HomogenousMatrix4 optimizedPose0, optimizedPose1;

		performance.start();

		NonconstArrayAccessor<Vector3> optimizedObjectPointsAccessor(optimizedObjectPoints);
		const bool result = Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointsAndTwoPoses(pinholeCamera, faultyPose0, faultyPose1, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints0), ConstArrayAccessor<Vector2>(imagePoints1), pinholeCamera.hasDistortionParameters(), &optimizedPose0, &optimizedPose1, &optimizedObjectPointsAccessor, 50u, estimatorType, Scalar(0.001), Scalar(5), true, nullptr, nullptr, useCovariances ? &invertedCovariances0 : nullptr, useCovariances ? &invertedCovariances1 : nullptr);
		ocean_assert(result);

		performance.stop();

		if (result)
		{
			Scalar sqrAveragePixelErrorInitial = 0;
			Scalar sqrMinimalPixelErrorInitial = 0;
			Scalar sqrMaximalPixelErrorInitial = 0;

			OCEAN_SUPPRESS_UNUSED_WARNING(sqrMinimalPixelErrorInitial);
			OCEAN_SUPPRESS_UNUSED_WARNING(sqrMaximalPixelErrorInitial);

			{
				Scalar sqrAveragePixelError = 0, sqrMinimalPixelError = 0, sqrMaximalPixelError = 0;

				Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true>(pose0, pinholeCamera, ConstTemplateArrayAccessor<Vector3>(objectPoints), ConstTemplateArrayAccessor<Vector2>(imagePoints0), pinholeCamera.hasDistortionParameters(), sqrAveragePixelError, sqrMinimalPixelError, sqrMaximalPixelError);
				sqrAveragePixelErrorInitial += sqrAveragePixelError;
				sqrMinimalPixelErrorInitial += sqrMinimalPixelError;
				sqrMaximalPixelErrorInitial += sqrMaximalPixelError;

				Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true>(pose1, pinholeCamera, ConstTemplateArrayAccessor<Vector3>(objectPoints), ConstTemplateArrayAccessor<Vector2>(imagePoints1), pinholeCamera.hasDistortionParameters(), sqrAveragePixelError, sqrMinimalPixelError, sqrMaximalPixelError);
				sqrAveragePixelErrorInitial += sqrAveragePixelError;
				sqrMinimalPixelErrorInitial += sqrMinimalPixelError;
				sqrMaximalPixelErrorInitial += sqrMaximalPixelError;
			}
			const Scalar sqrAverageObjectPointErrorInitial = Geometry::Error::determineAverageError(objectPoints, optimizedObjectPoints);
			OCEAN_SUPPRESS_UNUSED_WARNING(sqrAverageObjectPointErrorInitial);

			Scalar sqrAveragePixelErrorOptimized = 0;
			Scalar sqrMinimalPixelErrorOptimized = 0;
			Scalar sqrMaximalPixelErrorOptimized = 0;

			OCEAN_SUPPRESS_UNUSED_WARNING(sqrMinimalPixelErrorOptimized);
			OCEAN_SUPPRESS_UNUSED_WARNING(sqrMaximalPixelErrorOptimized);

			const Vectors2 outlierFreeImagePoints0(Subset::invertedSubset(perfectImagePoints0, outlierSet));
			const Vectors2 outlierFreeImagePoints1(Subset::invertedSubset(perfectImagePoints1, outlierSet));
			const Vectors3 outlierFreeObjectPoints(Subset::invertedSubset(perfectObjectPoints, outlierSet));
			const Vectors3 outlierFreeOptimizedObjectPoints(Subset::invertedSubset(optimizedObjectPoints, outlierSet));

			{
				Scalar sqrAveragePixelError = 0, sqrMinimalPixelError = 0, sqrMaximalPixelError = 0;

				Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true>(optimizedPose0, pinholeCamera, ConstTemplateArrayAccessor<Vector3>(outlierFreeOptimizedObjectPoints), ConstTemplateArrayAccessor<Vector2>(outlierFreeImagePoints0), pinholeCamera.hasDistortionParameters(), sqrAveragePixelError, sqrMinimalPixelError, sqrMaximalPixelError);
				sqrAveragePixelErrorOptimized += sqrAveragePixelError;
				sqrMinimalPixelErrorOptimized += sqrMinimalPixelError;
				sqrMaximalPixelErrorOptimized += sqrMaximalPixelError;

				Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true>(optimizedPose1, pinholeCamera, ConstTemplateArrayAccessor<Vector3>(outlierFreeOptimizedObjectPoints), ConstTemplateArrayAccessor<Vector2>(outlierFreeImagePoints1), pinholeCamera.hasDistortionParameters(), sqrAveragePixelError, sqrMinimalPixelError, sqrMaximalPixelError);
				sqrAveragePixelErrorOptimized += sqrAveragePixelError;
				sqrMinimalPixelErrorOptimized += sqrMinimalPixelError;
				sqrMaximalPixelErrorOptimized += sqrMaximalPixelError;
			}

			const Scalar sqrAverageObjectPointError = Geometry::Error::determineAverageError(outlierFreeObjectPoints, outlierFreeOptimizedObjectPoints);
			OCEAN_SUPPRESS_UNUSED_WARNING(sqrAverageObjectPointError);

			averageInitialSqrError += sqrAveragePixelErrorInitial;
			averageOptimizedSqrError += sqrAveragePixelErrorOptimized;

			medianPixelErrors.push_back(sqrAveragePixelErrorInitial);
			medianOptimizedPixelErrors.push_back(sqrAveragePixelErrorOptimized);

			succeeded++;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	averageInitialSqrError /= Scalar(iterations);
	averageOptimizedSqrError /= Scalar(iterations);

	Log::info() << "Average sqr pixel error: " << String::toAString(averageInitialSqrError, 1u) << "px -> " << String::toAString(averageOptimizedSqrError, 1u) << "px";
	Log::info() << "Median sqr pixel error: " << String::toAString(Median::constMedian(medianPixelErrors.data(), medianPixelErrors.size()), 1u) << "px -> " << String::toAString(Median::constMedian(medianOptimizedPixelErrors.data(), medianOptimizedPixelErrors.size()), 1u) << "px";
	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 1u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 1u) << "ms, average: " << String::toAString(performance.averageMseconds(), 1u) << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return true;
}

bool TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationPosesObjectPoints(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Optimization of 6DOF camera poses and 3D object point positions:";
	Log::info() << " ";

	bool result = true;

	for (const bool slowImplementation : {false, true})
	{
		if (slowImplementation)
		{
			Log::info() << " ";
			Log::info() << " ";
			Log::info() << "Now we test the slow implementation for comparison purpose:";
			Log::info() << " ";
		}

		for (const unsigned int outliersPercent : {0u, 10u})
		{
			Log::info().newLine(outliersPercent != 0u);
			Log::info().newLine(outliersPercent != 0u);

			for (const Scalar noise : {Scalar(0), Scalar(1)})
			{
				Log::info().newLine(noise != Scalar(0));
				Log::info().newLine(noise != Scalar(0));

				Log::info() << "Samples with Gaussian noise " << String::toAString(noise, 1u) << "px and " << outliersPercent << "% outliers:";
				Log::info() << " ";

				for (const unsigned int numberPoses : {20u, 50u})
				{
					for (const unsigned int numberObjectPoints : {10u, 50u, 100u, 1000u})
					{
						Log::info().newLine(numberObjectPoints != 10u);

						Log::info() << "With " << numberPoses << " poses and " << numberObjectPoints << " object points";
						Log::info() << " ";

						for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
						{
							Log::info() << "... and " << Geometry::Estimator::translateEstimatorType(estimatorType) << ":";

							result = testNonLinearOptimizationPosesObjectPoints(numberPoses, numberObjectPoints, testDuration, estimatorType, noise, numberObjectPoints * outliersPercent / 100u, slowImplementation) && result;

							Log::info() << " ";
						}
					}
				}
			}
		}
	}

	return result;
}

bool TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationPosesObjectPoints(const unsigned int numberPoses, const unsigned int numberObjectPoints, const double testDuration, const Geometry::Estimator::EstimatorType estimatorType, const Scalar noiseStandardDeviation, const unsigned int numberOutliers, const bool slowImplementation)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(numberOutliers <= numberObjectPoints);

	uint64_t iterations = 0ull;
	uint64_t succeeded = 0ull;

	Scalar averageInitialSqrError = 0;
	Scalar averageOptimizedSqrError = 0;

	HighPerformanceStatistic performance;

	Scalars medianInitialPixelErrors;
	Scalars medianOptimizedPixelErrors;

	const Box3 objectPointsArea(Vector3(-1, -1, -1), Vector3(1, 1, 1));

	const Timestamp startTimestamp(true);

	do
	{
		const AnyCameraType anyCameraType = RandomI::random(1u) == 0u ? AnyCameraType::PINHOLE : AnyCameraType::FISHEYE;

		const SharedAnyCamera camera = Utilities::realisticAnyCamera(anyCameraType, RandomI::random(1u));

		const Quaternion orientation0(Random::quaternion());
		const Vector3 viewDirection0(orientation0 * Vector3(0, 0, -1));

		const Vectors3 perfectObjectPoints(Utilities::objectPoints(objectPointsArea, numberObjectPoints));

		HomogenousMatrices4 world_T_cameras;
		world_T_cameras.emplace_back(Utilities::viewPosition(*camera, perfectObjectPoints, viewDirection0));

		while (world_T_cameras.size() < numberPoses)
		{
			const Quaternion offsetRotation(Random::euler(Numeric::deg2rad(5), Numeric::deg2rad(35)));

			const Quaternion newOrientation(orientation0 * offsetRotation);
			const Vector3 newViewDirection(newOrientation * Vector3(0, 0, -1));

			const Scalar angle(Numeric::rad2deg(viewDirection0.angle(newViewDirection)));
			ocean_assert_and_suppress_unused(Numeric::isInsideRange(5, angle, 85), angle);

			world_T_cameras.emplace_back(Utilities::viewPosition(*camera, perfectObjectPoints, newViewDirection, false));
		}

		Vectors2 imagePoints;
		Vectors2 perfectImagePoints;

		for (unsigned int p = 0u; p < numberPoses; ++p)
		{
			const HomogenousMatrix4& world_T_camera = world_T_cameras[p];

			for (unsigned int n = 0; n < numberObjectPoints; ++n)
			{
				const Vector2 perfectImagePoint(camera->projectToImage(world_T_camera, perfectObjectPoints[n]));

				Vector2 imagePointNoise(0, 0);
				if (noiseStandardDeviation > 0)
				{
					imagePointNoise = Vector2(Random::gaussianNoise(noiseStandardDeviation), Random::gaussianNoise(noiseStandardDeviation));
				}

				perfectImagePoints.emplace_back(perfectImagePoint);
				imagePoints.emplace_back(perfectImagePoint + imagePointNoise);
			}
		}

		Box3 objectVolume(perfectObjectPoints);
		const Scalar objectDimension = objectVolume.diagonal() * Scalar(0.01);

		const HomogenousMatrices4 flippedCameras_T_world(PinholeCamera::standard2InvertedFlipped(world_T_cameras));

		Vectors3 faultyObjectPoints;
		faultyObjectPoints.reserve(perfectObjectPoints.size());

		for (const Vector3& perfectObjectPoint : perfectObjectPoints)
		{
			while (true)
			{
				const Vector3 randomObjectPoint(perfectObjectPoint + Random::vector3(-objectDimension, objectDimension));

				bool valid = true;

				for (const HomogenousMatrix4& flippedCamera_T_world : flippedCameras_T_world)
				{
					const Vector2 projectedRandomObjectPoint(camera->projectToImageIF(flippedCamera_T_world, randomObjectPoint));

					if (!camera->isInside(projectedRandomObjectPoint, -50) || !AnyCamera::isObjectPointInFrontIF(flippedCamera_T_world, randomObjectPoint))
					{
						valid = false;
						break;
					}
				}

				if (valid)
				{
					faultyObjectPoints.emplace_back(randomObjectPoint);
					break;
				}
			}
		}

		HomogenousMatrices4 world_T_faultyCameras(world_T_cameras);
		for (HomogenousMatrix4& world_T_faultyCamera : world_T_faultyCameras)
		{
			world_T_faultyCamera *= HomogenousMatrix4(Random::vector3(-objectDimension, objectDimension) * Scalar(0.1), Random::euler(Numeric::deg2rad(1), Numeric::deg2rad(15)));
		}

		ocean_assert(faultyObjectPoints.size() == perfectObjectPoints.size());

		for (unsigned int poseIndex = 0u; poseIndex < numberPoses; ++poseIndex)
		{
			UnorderedIndexSet32 outlierSet;
			while (outlierSet.size() < numberOutliers)
			{
				const unsigned int objectPointIndex = Random::random(numberObjectPoints - 1u);

				if (outlierSet.emplace(objectPointIndex).second)
				{
					const Vector2 outlierError = Random::vector2(50, 100, 50, 100) * Random::sign();

					imagePoints[poseIndex * numberObjectPoints + objectPointIndex] += outlierError;
				}
			}
		}

		const Geometry::NonLinearOptimization::ObjectPointToPoseIndexImagePointCorrespondenceAccessor objectPointToPoseIndexImagePointCorrespondenceAccessor(faultyObjectPoints.size(), ConstTemplateArrayAccessor<Vector2>(imagePoints));

#ifdef OCEAN_DEBUG
		// ensure that the ObjectPointToPoseIndexImagePointCorrespondenceAccessor object holds the correct topology
		for (size_t g = 0; g < objectPointToPoseIndexImagePointCorrespondenceAccessor.groups(); ++g)
		{
			for (size_t p = 0; p < objectPointToPoseIndexImagePointCorrespondenceAccessor.groupElements(g); ++p)
			{
				Index32 poseIndex = Index32(-1);
				Vector2 imagePoint;

				objectPointToPoseIndexImagePointCorrespondenceAccessor.element(g, p, poseIndex, imagePoint);
				ocean_assert(imagePoints[p * faultyObjectPoints.size() + g] == imagePoint);
			}
		}
#endif

		HomogenousMatrices4 world_T_optimizedCameras(world_T_cameras.size());
		NonconstArrayAccessor<HomogenousMatrix4> access_world_T_optimizedCameras(world_T_optimizedCameras);

		Vectors3 optimizedObjectPoints(faultyObjectPoints.size());
		NonconstArrayAccessor<Vector3> optimizedObjectPointAccessor(optimizedObjectPoints);

		Scalars intermediate;

		performance.start();

		bool result = false;

		if (slowImplementation)
		{
			result = Geometry::NonLinearOptimizationObjectPoint::slowOptimizeObjectPointsAndPoses(*camera, ConstArrayAccessor<HomogenousMatrix4>(world_T_faultyCameras), ConstArrayAccessor<Vector3>(faultyObjectPoints), objectPointToPoseIndexImagePointCorrespondenceAccessor, &access_world_T_optimizedCameras, &optimizedObjectPointAccessor, 50u, estimatorType, Scalar(0.001), Scalar(5), true, nullptr, nullptr, &intermediate);
		}
		else
		{
			result = Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointsAndPoses(*camera, ConstArrayAccessor<HomogenousMatrix4>(world_T_faultyCameras), ConstArrayAccessor<Vector3>(faultyObjectPoints), objectPointToPoseIndexImagePointCorrespondenceAccessor, &access_world_T_optimizedCameras, &optimizedObjectPointAccessor, 50u, estimatorType, Scalar(0.001), Scalar(5), true, nullptr, nullptr, &intermediate);
		}

		ocean_assert(result);

		performance.stop();

		if (result)
		{
			Scalar sqrAveragePixelErrorInitial = 0;
			Scalar sqrMinimalPixelErrorInitial = 0;
			Scalar sqrMaximalPixelErrorInitial = 0;

			OCEAN_SUPPRESS_UNUSED_WARNING(sqrMinimalPixelErrorInitial);
			OCEAN_SUPPRESS_UNUSED_WARNING(sqrMaximalPixelErrorInitial);

			for (size_t p = 0; p < world_T_cameras.size(); ++p)
			{
				Scalar sqrAveragePixelError = 0;
				Scalar sqrMinimalPixelError = 0;
				Scalar sqrMaximalPixelError = 0;
				Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>>(world_T_faultyCameras[p], *camera, ConstTemplateArrayAccessor<Vector3>(faultyObjectPoints), ConstTemplateArrayAccessor<Vector2>(imagePoints.data() + p * numberObjectPoints, faultyObjectPoints.size()), sqrAveragePixelError, sqrMinimalPixelError, sqrMaximalPixelError);

				sqrAveragePixelErrorInitial += sqrAveragePixelError;
				sqrMinimalPixelErrorInitial += sqrMinimalPixelError;
				sqrMaximalPixelErrorInitial += sqrMaximalPixelError;
			}

			sqrAveragePixelErrorInitial /= Scalar(world_T_cameras.size());
			sqrMinimalPixelErrorInitial /= Scalar(world_T_cameras.size());
			sqrMaximalPixelErrorInitial /= Scalar(world_T_cameras.size());

			Scalar sqrAveragePixelErrorOptimized = 0;
			Scalar sqrMinimalPixelErrorOptimized = 0;
			Scalar sqrMaximalPixelErrorOptimized = 0;

			OCEAN_SUPPRESS_UNUSED_WARNING(sqrMinimalPixelErrorOptimized);
			OCEAN_SUPPRESS_UNUSED_WARNING(sqrMaximalPixelErrorOptimized);

			for (size_t p = 0; p < world_T_optimizedCameras.size(); ++p)
			{
				Scalar sqrAveragePixelError = 0;
				Scalar sqrMinimalPixelError = 0;
				Scalar sqrMaximalPixelError = 0;
				Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>>(world_T_optimizedCameras[p], *camera, ConstTemplateArrayAccessor<Vector3>(optimizedObjectPoints), ConstTemplateArrayAccessor<Vector2>(perfectImagePoints.data() + p * numberObjectPoints, numberObjectPoints), sqrAveragePixelError, sqrMinimalPixelError, sqrMaximalPixelError);

				sqrAveragePixelErrorOptimized += sqrAveragePixelError;
				sqrMinimalPixelErrorOptimized += sqrMinimalPixelError;
				sqrMaximalPixelErrorOptimized += sqrMaximalPixelError;
			}

			sqrAveragePixelErrorOptimized /= Scalar(world_T_cameras.size());
			sqrMinimalPixelErrorOptimized /= Scalar(world_T_cameras.size());
			sqrMaximalPixelErrorOptimized /= Scalar(world_T_cameras.size());

			const Scalar sqrAverageObjectPointErrorInitial = Geometry::Error::determineAverageError(faultyObjectPoints, optimizedObjectPoints);
			const Scalar sqrAverageObjectPointError = Geometry::Error::determineAverageError(perfectObjectPoints, optimizedObjectPoints);

			OCEAN_SUPPRESS_UNUSED_WARNING(sqrAverageObjectPointErrorInitial);
			OCEAN_SUPPRESS_UNUSED_WARNING(sqrAverageObjectPointError);

			averageInitialSqrError += sqrAveragePixelErrorInitial;
			averageOptimizedSqrError += sqrAveragePixelErrorOptimized;

			medianInitialPixelErrors.push_back(sqrAveragePixelErrorInitial);
			medianOptimizedPixelErrors.push_back(sqrAveragePixelErrorOptimized);

			succeeded++;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	averageInitialSqrError /= Scalar(iterations);
	averageOptimizedSqrError /= Scalar(iterations);

	Log::info() << "Average pixel error: " << String::toAString(averageInitialSqrError, 1u) << "px -> " << String::toAString(averageOptimizedSqrError, 1u) << "px";
	Log::info() << "Median sqr pixel error: " << String::toAString(Median::constMedian(medianInitialPixelErrors.data(), medianInitialPixelErrors.size()), 1u) << "px -> " << String::toAString(Median::constMedian(medianOptimizedPixelErrors.data(), medianOptimizedPixelErrors.size()), 1u) << "px";
	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 1u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 1u) << "ms, average: " << String::toAString(performance.averageMseconds(), 1u) << "ms";

	bool allSucceeded = true;

	if (percent < 0.95)
	{
		allSucceeded = false;
	}

	if (averageOptimizedSqrError >= averageInitialSqrError)
	{
		// the optimized solution must be better than the initial solution

		allSucceeded = false;
	}

	if (numberPoses * numberObjectPoints > 1000u) // in case we have enough signals
	{
		if (averageOptimizedSqrError > 200)
		{
			// we always need a reasonable result

			allSucceeded = false;
		}
	}

	if (numberPoses >= 50u)
	{
		if (noiseStandardDeviation == Scalar(0))
		{
			if (numberOutliers == 0u)
			{
				// we have perfect conditions, so we expect perfect results

				if (averageOptimizedSqrError > Scalar(0.1))
				{
					allSucceeded = false;
				}
			}
			else
			{
				if (numberPoses * numberObjectPoints > 1000u) // in case we have enough signals
				{
					if (estimatorType == Geometry::Estimator::ET_LINEAR || estimatorType == Geometry::Estimator::ET_HUBER || estimatorType == Geometry::Estimator::ET_CAUCHY)
					{
						// the robust estimators need to handle outliers

						if (averageOptimizedSqrError > Scalar(10.0))
						{
							allSucceeded = false;
						}
					}
					else if (estimatorType == Geometry::Estimator::ET_TUKEY)
					{
						// Tukey may not find the optimal solution

						if (averageOptimizedSqrError > Scalar(30.0))
						{
							allSucceeded = false;
						}
					}
				}
			}
		}
		else
		{
			if (numberPoses * numberObjectPoints > 1000u) // in case we have enough signals
			{
				if (estimatorType == Geometry::Estimator::ET_LINEAR || estimatorType == Geometry::Estimator::ET_HUBER || estimatorType == Geometry::Estimator::ET_CAUCHY)
				{
					// the robust estimators cannot handle noise, but still need to handle the outliers

					if (averageOptimizedSqrError > Scalar(10.0))
					{
						allSucceeded = false;
					}
				}
			}
		}
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: Accuracy verification succeeded, " << String::toAString(percent * 100.0, 1u) << "% finished";
	}
	else
	{
		Log::info() << "Validation: Accuracy verification FAILED, " << String::toAString(percent * 100.0, 1u) << "% finished";
	}

	return allSucceeded;
}

bool TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationOrientationalPosesObjectPoints(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Optimization of 6DOF camera poses (with fixed translations) and 3D object point positions:";
	Log::info() << " ";

	bool result = true;

	for (const unsigned int outliersPercent : {0u, 10u})
	{
		Log::info().newLine(outliersPercent != 0u);
		Log::info().newLine(outliersPercent != 0u);

		for (const Scalar noise : {Scalar(0), Scalar(1)})
		{
			Log::info().newLine(noise != Scalar(0));
			Log::info().newLine(noise != Scalar(0));

			Log::info() << "Samples with Gaussian noise " << String::toAString(noise, 1u) << "px and " << outliersPercent << "% outliers:";
			Log::info() << " ";

			for (const unsigned int numberPoses : {20u, 50u})
			{
				for (const unsigned int numberObjectPoints : {10u, 50u, 100u, 1000u})
				{
					Log::info().newLine(numberObjectPoints != 10u);

					Log::info() << "With " << numberPoses << " poses and " << numberObjectPoints << " object points";
					Log::info() << " ";

					for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
					{
						Log::info() << "... and " << Geometry::Estimator::translateEstimatorType(estimatorType) << ":";

						result = testNonLinearOptimizationOrientationalPosesObjectPoints(numberPoses, numberObjectPoints, testDuration, estimatorType, noise, numberObjectPoints * outliersPercent / 100u) && result;

						Log::info() << " ";
					}
				}
			}
		}
	}

	return result;
}

bool TestNonLinearOptimizationObjectPoint::testNonLinearOptimizationOrientationalPosesObjectPoints(const unsigned int numberPoses, const unsigned int numberObjectPoints, const double testDuration, const Geometry::Estimator::EstimatorType estimatorType, const Scalar noiseStandardDeviation, const unsigned int numberOutliers)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(numberOutliers <= numberObjectPoints);

	uint64_t iterations = 0ull;
	uint64_t succeeded = 0ull;

	Scalar averageInitialSqrError = 0;
	Scalar averageOptimizedSqrError = 0;

	HighPerformanceStatistic performance;

	Scalars medianInitialPixelErrors;
	Scalars medianOptimizedPixelErrors;

	const Box3 objectPointsArea(Vector3(-1, -1, -1), Vector3(1, 1, 1));

	const std::vector<AnyCameraType> anyCameraTypes = {AnyCameraType::PINHOLE, AnyCameraType::FISHEYE};

	const Timestamp startTimestamp(true);

	RandomGenerator randomGenerator;

	do
	{
		const SharedAnyCamera firstCamera = Utilities::realisticAnyCamera(RandomI::random(randomGenerator, anyCameraTypes), RandomI::random(randomGenerator, 1u));

		const bool useSingleCamera = RandomI::random(randomGenerator, 1u) == 0u;

		SharedAnyCameras cameras;
		cameras.emplace_back(firstCamera);

		const Quaternion orientation0(Random::quaternion(randomGenerator));
		const Vector3 viewDirection0(orientation0 * Vector3(0, 0, -1));

		const Vectors3 perfectObjectPoints(Utilities::objectPoints(objectPointsArea, numberObjectPoints, &randomGenerator));

		HomogenousMatrices4 world_T_cameras;
		world_T_cameras.emplace_back(Utilities::viewPosition(*cameras.back(), perfectObjectPoints, viewDirection0));

		while (world_T_cameras.size() < numberPoses)
		{
			if (useSingleCamera)
			{
				cameras.emplace_back(firstCamera);
			}
			else
			{
				cameras.emplace_back(Utilities::realisticAnyCamera(RandomI::random(randomGenerator, anyCameraTypes), RandomI::random(randomGenerator, 1u)));
			}

			const Quaternion offsetRotation(Random::euler(randomGenerator, Numeric::deg2rad(5), Numeric::deg2rad(35)));

			const Quaternion newOrientation(orientation0 * offsetRotation);
			const Vector3 newViewDirection(newOrientation * Vector3(0, 0, -1));

			const Scalar angle(Numeric::rad2deg(viewDirection0.angle(newViewDirection)));
			ocean_assert_and_suppress_unused(Numeric::isInsideRange(5, angle, 85), angle);

			world_T_cameras.emplace_back(Utilities::viewPosition(*cameras.back(), perfectObjectPoints, newViewDirection, false));
		}

		Vectors2 imagePoints;
		Vectors2 perfectImagePoints;

		for (unsigned int p = 0u; p < numberPoses; ++p)
		{
			const HomogenousMatrix4& world_T_camera = world_T_cameras[p];

			for (unsigned int n = 0; n < numberObjectPoints; ++n)
			{
				const Vector2 perfectImagePoint(cameras[p]->projectToImage(world_T_camera, perfectObjectPoints[n]));

				Vector2 imagePointNoise(0, 0);
				if (noiseStandardDeviation > 0)
				{
					imagePointNoise = Vector2(Random::gaussianNoise(randomGenerator, noiseStandardDeviation), Random::gaussianNoise(randomGenerator, noiseStandardDeviation));
				}

				perfectImagePoints.emplace_back(perfectImagePoint);
				imagePoints.emplace_back(perfectImagePoint + imagePointNoise);
			}
		}

		Box3 objectVolume(perfectObjectPoints);
		const Scalar objectDimension = objectVolume.diagonal() * Scalar(0.01);

		const HomogenousMatrices4 flippedCameras_T_world(PinholeCamera::standard2InvertedFlipped(world_T_cameras));

		Vectors3 faultyObjectPoints;
		faultyObjectPoints.reserve(perfectObjectPoints.size());

		for (const Vector3& perfectObjectPoint : perfectObjectPoints)
		{
			while (true)
			{
				const Vector3 randomObjectPoint(perfectObjectPoint + Random::vector3(randomGenerator, -objectDimension, objectDimension));

				bool valid = true;

				for (size_t p = 0u; p < flippedCameras_T_world.size(); ++p)
				{
					const SharedAnyCamera& camera = cameras[p];
					const HomogenousMatrix4 flippedCamera_T_world = flippedCameras_T_world[p];

					if (!AnyCamera::isObjectPointInFrontIF(flippedCamera_T_world, randomObjectPoint))
					{
						valid = false;
						break;
					}

					const Vector2 projectedRandomObjectPoint(camera->projectToImageIF(flippedCameras_T_world[p], randomObjectPoint));

					if (!camera->isInside(projectedRandomObjectPoint, -50))
					{
						valid = false;
						break;
					}
				}

				if (valid)
				{
					faultyObjectPoints.emplace_back(randomObjectPoint);
					break;
				}
			}
		}

		HomogenousMatrices4 world_T_faultyCameras(world_T_cameras);
		for (HomogenousMatrix4& world_T_faultyCamera : world_T_faultyCameras)
		{
			world_T_faultyCamera *= HomogenousMatrix4(Random::euler(randomGenerator, Numeric::deg2rad(1), Numeric::deg2rad(15)));
		}

		for (size_t n = 0; n < world_T_cameras.size(); ++n)
		{
			ocean_assert(world_T_cameras[n].translation() == world_T_faultyCameras[n].translation());
		}

		ocean_assert(faultyObjectPoints.size() == perfectObjectPoints.size());

		for (unsigned int poseIndex = 0u; poseIndex < numberPoses; ++poseIndex)
		{
			UnorderedIndexSet32 outlierSet;
			while (outlierSet.size() < numberOutliers)
			{
				const unsigned int objectPointIndex = Random::random(randomGenerator, numberObjectPoints - 1u);

				if (outlierSet.emplace(objectPointIndex).second)
				{
					const Vector2 outlierError = Random::vector2(randomGenerator, 50, 100, 50, 100) * Random::sign(randomGenerator);

					imagePoints[poseIndex * numberObjectPoints + objectPointIndex] += outlierError;
				}
			}
		}

		const Geometry::NonLinearOptimization::ObjectPointToPoseIndexImagePointCorrespondenceAccessor objectPointToPoseIndexImagePointCorrespondenceAccessor(faultyObjectPoints.size(), ConstTemplateArrayAccessor<Vector2>(imagePoints));

#ifdef OCEAN_DEBUG
		// ensure that the ObjectPointToPoseIndexImagePointCorrespondenceAccessor object holds the correct topology
		for (size_t g = 0; g < objectPointToPoseIndexImagePointCorrespondenceAccessor.groups(); ++g)
		{
			for (size_t p = 0; p < objectPointToPoseIndexImagePointCorrespondenceAccessor.groupElements(g); ++p)
			{
				Index32 poseIndex = Index32(-1);
				Vector2 imagePoint;

				objectPointToPoseIndexImagePointCorrespondenceAccessor.element(g, p, poseIndex, imagePoint);
				ocean_assert(imagePoints[p * faultyObjectPoints.size() + g] == imagePoint);
			}
		}
#endif

		HomogenousMatrices4 world_T_optimizedCameras(world_T_cameras.size());
		NonconstArrayAccessor<HomogenousMatrix4> access_world_T_optimizedCameras(world_T_optimizedCameras);

		Vectors3 optimizedObjectPoints(faultyObjectPoints.size());
		NonconstArrayAccessor<Vector3> optimizedObjectPointAccessor(optimizedObjectPoints);

		Scalars intermediate;

		performance.start();

		bool result = false;

		if (useSingleCamera)
		{
			result = Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointsAndOrientationalPoses(ConstElementAccessor<const AnyCamera*>(world_T_faultyCameras.size(), firstCamera.get()), ConstArrayAccessor<HomogenousMatrix4>(world_T_faultyCameras), ConstArrayAccessor<Vector3>(faultyObjectPoints), objectPointToPoseIndexImagePointCorrespondenceAccessor, &access_world_T_optimizedCameras, &optimizedObjectPointAccessor, 50u, estimatorType, Scalar(0.001), Scalar(5), true, nullptr, nullptr, &intermediate);
		}
		else
		{
			result = Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointsAndOrientationalPoses(SharedPointerConstArrayAccessor<AnyCamera>(cameras), ConstArrayAccessor<HomogenousMatrix4>(world_T_faultyCameras), ConstArrayAccessor<Vector3>(faultyObjectPoints), objectPointToPoseIndexImagePointCorrespondenceAccessor, &access_world_T_optimizedCameras, &optimizedObjectPointAccessor, 50u, estimatorType, Scalar(0.001), Scalar(5), true, nullptr, nullptr, &intermediate);
		}

		ocean_assert(result);

		performance.stop();

		if (result)
		{
			Scalar sqrAveragePixelErrorInitial = 0;
			Scalar sqrMinimalPixelErrorInitial = 0;
			Scalar sqrMaximalPixelErrorInitial = 0;

			OCEAN_SUPPRESS_UNUSED_WARNING(sqrMinimalPixelErrorInitial);
			OCEAN_SUPPRESS_UNUSED_WARNING(sqrMaximalPixelErrorInitial);

			for (size_t p = 0; p < world_T_cameras.size(); ++p)
			{
				Scalar sqrAveragePixelError = 0;
				Scalar sqrMinimalPixelError = 0;
				Scalar sqrMaximalPixelError = 0;
				Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>>(world_T_faultyCameras[p], *cameras[p], ConstTemplateArrayAccessor<Vector3>(faultyObjectPoints), ConstTemplateArrayAccessor<Vector2>(imagePoints.data() + p * numberObjectPoints, faultyObjectPoints.size()), sqrAveragePixelError, sqrMinimalPixelError, sqrMaximalPixelError);

				sqrAveragePixelErrorInitial += sqrAveragePixelError;
				sqrMinimalPixelErrorInitial += sqrMinimalPixelError;
				sqrMaximalPixelErrorInitial += sqrMaximalPixelError;
			}

			sqrAveragePixelErrorInitial /= Scalar(world_T_cameras.size());
			sqrMinimalPixelErrorInitial /= Scalar(world_T_cameras.size());
			sqrMaximalPixelErrorInitial /= Scalar(world_T_cameras.size());

			Scalar sqrAveragePixelErrorOptimized = 0;
			Scalar sqrMinimalPixelErrorOptimized = 0;
			Scalar sqrMaximalPixelErrorOptimized = 0;

			OCEAN_SUPPRESS_UNUSED_WARNING(sqrMinimalPixelErrorOptimized);
			OCEAN_SUPPRESS_UNUSED_WARNING(sqrMaximalPixelErrorOptimized);

			for (size_t p = 0; p < world_T_optimizedCameras.size(); ++p)
			{
				ocean_assert(world_T_optimizedCameras[p].translation().isEqual(world_T_cameras[p].translation(), Numeric::weakEps()));

				const HomogenousMatrix4 world_T_optimizedCamera = world_T_optimizedCameras[p];

				Scalar sqrAveragePixelError = 0;
				Scalar sqrMinimalPixelError = 0;
				Scalar sqrMaximalPixelError = 0;
				Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>>(world_T_optimizedCamera, *cameras[p], ConstTemplateArrayAccessor<Vector3>(optimizedObjectPoints), ConstTemplateArrayAccessor<Vector2>(perfectImagePoints.data() + p * numberObjectPoints, numberObjectPoints), sqrAveragePixelError, sqrMinimalPixelError, sqrMaximalPixelError);

				sqrAveragePixelErrorOptimized += sqrAveragePixelError;
				sqrMinimalPixelErrorOptimized += sqrMinimalPixelError;
				sqrMaximalPixelErrorOptimized += sqrMaximalPixelError;
			}

			sqrAveragePixelErrorOptimized /= Scalar(world_T_cameras.size());
			sqrMinimalPixelErrorOptimized /= Scalar(world_T_cameras.size());
			sqrMaximalPixelErrorOptimized /= Scalar(world_T_cameras.size());

			const Scalar sqrAverageObjectPointErrorInitial = Geometry::Error::determineAverageError(faultyObjectPoints, optimizedObjectPoints);
			const Scalar sqrAverageObjectPointError = Geometry::Error::determineAverageError(perfectObjectPoints, optimizedObjectPoints);

			OCEAN_SUPPRESS_UNUSED_WARNING(sqrAverageObjectPointErrorInitial);
			OCEAN_SUPPRESS_UNUSED_WARNING(sqrAverageObjectPointError);

			averageInitialSqrError += sqrAveragePixelErrorInitial;
			averageOptimizedSqrError += sqrAveragePixelErrorOptimized;

			medianInitialPixelErrors.push_back(sqrAveragePixelErrorInitial);
			medianOptimizedPixelErrors.push_back(sqrAveragePixelErrorOptimized);

			succeeded++;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	averageInitialSqrError /= Scalar(iterations);
	averageOptimizedSqrError /= Scalar(iterations);

	Log::info() << "Average pixel error: " << String::toAString(averageInitialSqrError, 1u) << "px -> " << String::toAString(averageOptimizedSqrError, 1u) << "px";
	Log::info() << "Median sqr pixel error: " << String::toAString(Median::constMedian(medianInitialPixelErrors.data(), medianInitialPixelErrors.size()), 1u) << "px -> " << String::toAString(Median::constMedian(medianOptimizedPixelErrors.data(), medianOptimizedPixelErrors.size()), 1u) << "px";
	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 1u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 1u) << "ms, average: " << String::toAString(performance.averageMseconds(), 1u) << "ms";

	bool allSucceeded = true;

	if (percent < 0.95)
	{
		allSucceeded = false;
	}

	if (averageOptimizedSqrError >= averageInitialSqrError)
	{
		// the optimized solution must be better than the initial solution

		allSucceeded = false;
	}

	if (numberPoses * numberObjectPoints > 1000u) // in case we have enough signals
	{
		if (averageOptimizedSqrError > 200)
		{
			// we always need a reasonable result

			allSucceeded = false;
		}
	}

	if (numberPoses >= 50u)
	{
		if (noiseStandardDeviation == Scalar(0))
		{
			if (numberOutliers == 0u)
			{
				// we have perfect conditions, so we expect perfect results

				if (averageOptimizedSqrError > Scalar(0.1))
				{
					allSucceeded = false;
				}
			}
			else
			{
				if (numberPoses * numberObjectPoints > 1000u) // in case we have enough signals
				{
					if (estimatorType == Geometry::Estimator::ET_LINEAR || estimatorType == Geometry::Estimator::ET_HUBER || estimatorType == Geometry::Estimator::ET_CAUCHY)
					{
						// the robust estimators need to handle outliers

						if (averageOptimizedSqrError > Scalar(10.0))
						{
							allSucceeded = false;
						}
					}
					else if (estimatorType == Geometry::Estimator::ET_TUKEY)
					{
						// Tukey may not find the optimal solution

						if (averageOptimizedSqrError > Scalar(30.0))
						{
							allSucceeded = false;
						}
					}
				}
			}
		}
		else
		{
			if (numberPoses * numberObjectPoints > 1000u) // in case we have enough signals
			{
				if (estimatorType == Geometry::Estimator::ET_LINEAR || estimatorType == Geometry::Estimator::ET_HUBER || estimatorType == Geometry::Estimator::ET_CAUCHY)
				{
					// the robust estimators cannot handle noise, but still need to handle the outliers

					if (averageOptimizedSqrError > Scalar(10.0))
					{
						allSucceeded = false;
					}
				}
			}
		}
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: Accuracy verification succeeded, " << String::toAString(percent * 100.0, 1u) << "% finished";
	}
	else
	{
		Log::info() << "Validation: Accuracy verification FAILED, " << String::toAString(percent * 100.0, 1u) << "% finished";
	}

	return allSucceeded;
}

bool TestNonLinearOptimizationObjectPoint::testOptimizeObjectPointRotationalPoses(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Optimization of 3D object points for fixed 3DOF (orientational) poses:";

	bool allSucceeded = true;

	const PinholeCamera pinholeCamera(1280u, 720u, Numeric::deg2rad(60));

	for (const bool useRoughObjectPoint : {true, false})
	{
		Log::info().newLine();
		Log::info().newLine();

		if (useRoughObjectPoint)
		{
			Log::info() << "... with rough object point";
		}
		else
		{
			Log::info() << "... without rough object point";
		}

		for (const unsigned int outlier : {0u, 10u})
		{
			for (const Scalar noise : {Scalar(0), Scalar(1)})
			{
				Log::info().newLine();
				Log::info().newLine();

				Log::info() << "Samples with Gaussian noise " << String::toAString(noise, 1u) << "px and " << outlier << "% outliers:";
				Log::info() << " ";

				for (const unsigned int observations : {10u, 20u, 50u, 500u})
				{
					Log::info().newLine(observations != 10u);

					Log::info() << "With " << observations << " observations";

					for (Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
					{
						Log::info() << "... and " << Geometry::Estimator::translateEstimatorType(estimatorType) << ":";

						if (!testOptimizeObjectPointRotationalPoses(pinholeCamera, observations, testDuration, estimatorType, noise, observations * outlier / 100u, useRoughObjectPoint))
						{
							allSucceeded = false;
						}
					}
				}
			}
		}
	}

	return allSucceeded;
}

bool TestNonLinearOptimizationObjectPoint::testOptimizeObjectPointRotationalPoses(const PinholeCamera& patternCamera, const unsigned int numberObservations, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation, const unsigned int numberOutliers, const bool useRoughObjectPoint)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(numberOutliers <= numberObservations);

	Scalars initialErrors;
	Scalars optimizedErrors;

	HighPerformanceStatistic performance;

	enum DistortionType : uint32_t
	{
		DT_NO_DISTORTION = 0u,
		DT_RADIAL_DISTORTION = 1u << 0u | DT_NO_DISTORTION,
		DT_FULL_DISTORTION = (1u << 1u) | DT_RADIAL_DISTORTION
	};

	RandomGenerator randomGenerator;

	ValidationPrecision validation(0.99, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (const DistortionType distortionType : {DT_NO_DISTORTION, DT_RADIAL_DISTORTION, DT_FULL_DISTORTION})
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const AnyCameraPinhole camera(Utilities::distortedCamera(patternCamera, true, (distortionType & DT_RADIAL_DISTORTION) == DT_RADIAL_DISTORTION, (distortionType & DT_FULL_DISTORTION) == DT_FULL_DISTORTION));

			Vectors2 perfectImagePoints;
			SquareMatrices3 world_R_cameras;

			Vector3 objectPoint(Numeric::minValue(), Numeric::minValue(), Numeric::minValue());

			while (world_R_cameras.size() < numberObservations)
			{
				constexpr Scalar cameraBorder = Scalar(20);

				const SquareMatrix3 world_R_camera(Random::euler(Numeric::deg2rad(0), Numeric::deg2rad(30)));

				if (world_R_cameras.empty())
				{
					const Vector2 imagePoint(Random::vector2(randomGenerator, cameraBorder, Scalar(camera.width()) - cameraBorder, cameraBorder, Scalar(camera.height()) - cameraBorder));

					const Line3 imagePointRay = camera.ray(imagePoint, HomogenousMatrix4(world_R_camera));

					objectPoint = imagePointRay.direction() * Random::scalar(randomGenerator, 2, 5);

					world_R_cameras.push_back(world_R_camera);
					perfectImagePoints.push_back(imagePoint);
				}
				else
				{
					if (AnyCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(world_R_camera), objectPoint))
					{
						const Vector2 projectedObjectPoint(camera.projectToImage(HomogenousMatrix4(world_R_camera), objectPoint));

						if (camera.isInside(projectedObjectPoint, cameraBorder))
						{
							world_R_cameras.push_back(world_R_camera);
							perfectImagePoints.push_back(projectedObjectPoint);
						}
					}
				}
			}

			Vectors2 imagePoints(perfectImagePoints);

			if (standardDeviation > 0)
			{
				for (Vector2& imagePoint : imagePoints)
				{
					imagePoint += Random::gaussianNoiseVector2(randomGenerator, standardDeviation, standardDeviation);
				}
			}

			UnorderedIndexSet32 outlierSet;

			while (outlierSet.size() < numberOutliers)
			{
				const unsigned int index = RandomI::random(randomGenerator, numberObservations - 1u);

				if (outlierSet.emplace(index).second)
				{
					const Scalar sign = Random::sign(randomGenerator);

					imagePoints[index] += Random::vector2(randomGenerator, Scalar(10), Scalar(100)) * sign;
				}
			}

			performance.start();

			if (useRoughObjectPoint)
			{
				const Vector3 startObjectPoint = objectPoint + Random::vector3(randomGenerator, Scalar(-0.1), Scalar(0.1));

				Vector3 optimizedObjectPoint;

				Scalar initialError = Numeric::maxValue();
				Scalar finalError = Numeric::maxValue();

				if (Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedOrientations(camera, ConstArrayAccessor<SquareMatrix3>(world_R_cameras), ConstArrayAccessor<Vector2>(imagePoints), startObjectPoint, Scalar(1), optimizedObjectPoint, 20u, type, Scalar(0.001), Scalar(5), true, &initialError, &finalError))
				{
					initialErrors.push_back(initialError);
					optimizedErrors.push_back(finalError);
				}
				else
				{
					scopedIteration.setInaccurate();
				}
			}
			else
			{
				Vector3 startObjectPoint;
				Indices32 usedIndices;
				if (Geometry::RANSAC::objectPoint(camera, ConstArrayAccessor<SquareMatrix3>(world_R_cameras), ConstArrayAccessor<Vector2>(imagePoints), randomGenerator, startObjectPoint, Scalar(1), 50u, Scalar(5 * 5), 3u, true, Geometry::Estimator::ET_INVALID, nullptr, &usedIndices))
				{
					Vector3 optimizedObjectPoint;
					Scalar initialError, finalError;
					if (Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedOrientations(camera, ConstArraySubsetAccessor<SquareMatrix3, unsigned int>(world_R_cameras, usedIndices), ConstArraySubsetAccessor<Vector2, unsigned int>(imagePoints, usedIndices), startObjectPoint, Scalar(1), optimizedObjectPoint, 20u, type, Scalar(0.001), Scalar(5), true, &initialError, &finalError))
					{
						initialErrors.push_back(initialError);
						optimizedErrors.push_back(finalError);
					}
					else
					{
						scopedIteration.setInaccurate();
					}
				}
			}

			performance.stop();
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	std::sort(initialErrors.begin(), initialErrors.end());
	std::sort(optimizedErrors.begin(), optimizedErrors.end());

	ocean_assert(initialErrors.size() == optimizedErrors.size());

	if (!initialErrors.empty())
	{
		Scalar averageInitialError = 0;
		Scalar averageOptimizedError = 0;

		for (size_t n = 0; n < initialErrors.size(); ++n)
		{
			averageInitialError += initialErrors[n];
			averageOptimizedError += optimizedErrors[n];
		}

		averageInitialError /= Scalar(initialErrors.size());
		averageOptimizedError /= Scalar(initialErrors.size());

		const Scalar medianIntitialError = initialErrors[initialErrors.size() / 2];
		const Scalar medianOptimizedError = optimizedErrors[initialErrors.size() / 2];

		Log::info() << "Average error: " << String::toAString(averageInitialError, 1u) << "px -> " << String::toAString(averageOptimizedError, 1u) << "px";
		Log::info() << "Median error: " << String::toAString(medianIntitialError, 1u) << "px -> " << String::toAString(medianOptimizedError, 1u) << "px";

		if (numberOutliers == 0u)
		{
			if (standardDeviation == Scalar(0))
			{
				if (useRoughObjectPoint)
				{
					if (medianOptimizedError > Scalar(0.1))
					{
						// we have perfect conditions, so we expect perfect results
						OCEAN_SET_FAILED(validation);
					}
				}
				else
				{
					if (optimizedErrors.back() > Scalar(0.1))
					{
						OCEAN_SET_FAILED(validation);
					}
				}
			}
			else
			{
				if (numberOutliers == 0u)
				{
					if (medianOptimizedError > Scalar(10 * 10))
					{
						// we have no outliers, so we expect some noisy results
						OCEAN_SET_FAILED(validation);
					}
				}
			}
		}
	}
	else
	{
		OCEAN_SET_FAILED(validation);
	}

	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
