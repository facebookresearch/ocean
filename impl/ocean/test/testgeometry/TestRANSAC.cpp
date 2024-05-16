/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestRANSAC.h"
#include "ocean/test/testgeometry/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/geometry/RANSAC.h"

#include "ocean/math/Random.h"

#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

bool TestRANSAC::test(const double testDuration, Worker* /*worker*/)
{
	Log::info() << "---   RANSAC test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testP3P(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testP3PPinholeCamera(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testP3PZoom(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testObjectTransformationStereoAnyCamera(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "RANSAC test succeeded.";
	}
	else
	{
		Log::info() << "RANSAC test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestRANSAC, P3P_Pinhole_10Correspondences_0Outliers)
{
	EXPECT_TRUE(TestRANSAC::testP3P(AnyCameraType::PINHOLE, 10, 0.0, GTEST_TEST_DURATION));
}

TEST(TestRANSAC, P3P_Pinhole_50Correspondences_0Outliers)
{
	EXPECT_TRUE(TestRANSAC::testP3P(AnyCameraType::PINHOLE, 50, 0.0, GTEST_TEST_DURATION));
}

TEST(TestRANSAC, P3P_Pinhole_100Correspondences_0Outliers)
{
	EXPECT_TRUE(TestRANSAC::testP3P(AnyCameraType::PINHOLE, 100, 0.0, GTEST_TEST_DURATION));
}

TEST(TestRANSAC, P3P_Pinhole_1000Correspondences_0Outliers)
{
	EXPECT_TRUE(TestRANSAC::testP3P(AnyCameraType::PINHOLE, 1000, 0.0, GTEST_TEST_DURATION));
}


TEST(TestRANSAC, P3P_Pinhole_10Correspondences_15Outliers)
{
	EXPECT_TRUE(TestRANSAC::testP3P(AnyCameraType::PINHOLE, 10, 0.15, GTEST_TEST_DURATION));
}

TEST(TestRANSAC, P3P_Pinhole_50Correspondences_15Outliers)
{
	EXPECT_TRUE(TestRANSAC::testP3P(AnyCameraType::PINHOLE, 50, 0.15, GTEST_TEST_DURATION));
}

TEST(TestRANSAC, P3P_Pinhole_100Correspondences_15Outliers)
{
	EXPECT_TRUE(TestRANSAC::testP3P(AnyCameraType::PINHOLE, 100, 0.15, GTEST_TEST_DURATION));
}

TEST(TestRANSAC, P3P_Pinhole_1000Correspondences_15Outliers)
{
	EXPECT_TRUE(TestRANSAC::testP3P(AnyCameraType::PINHOLE, 1000, 0.15, GTEST_TEST_DURATION));
}


TEST(TestRANSAC, P3P_Fisheye_10Correspondences_0Outliers)
{
	EXPECT_TRUE(TestRANSAC::testP3P(AnyCameraType::FISHEYE, 10, 0.0, GTEST_TEST_DURATION));
}

TEST(TestRANSAC, P3P_Fisheye_50Correspondences_0Outliers)
{
	EXPECT_TRUE(TestRANSAC::testP3P(AnyCameraType::FISHEYE, 50, 0.0, GTEST_TEST_DURATION));
}

TEST(TestRANSAC, P3P_Fisheye_100Correspondences_0Outliers)
{
	EXPECT_TRUE(TestRANSAC::testP3P(AnyCameraType::FISHEYE, 100, 0.0, GTEST_TEST_DURATION));
}

TEST(TestRANSAC, P3P_Fisheye_1000Correspondences_0Outliers)
{
	EXPECT_TRUE(TestRANSAC::testP3P(AnyCameraType::FISHEYE, 1000, 0.0, GTEST_TEST_DURATION));
}


TEST(TestRANSAC, P3P_Fisheye_10Correspondences_15Outliers)
{
	EXPECT_TRUE(TestRANSAC::testP3P(AnyCameraType::FISHEYE, 10, 0.15, GTEST_TEST_DURATION));
}

TEST(TestRANSAC, P3P_Fisheye_50Correspondences_15Outliers)
{
	EXPECT_TRUE(TestRANSAC::testP3P(AnyCameraType::FISHEYE, 50, 0.15, GTEST_TEST_DURATION));
}

TEST(TestRANSAC, P3P_Fisheye_100Correspondences_15Outliers)
{
	EXPECT_TRUE(TestRANSAC::testP3P(AnyCameraType::FISHEYE, 100, 0.15, GTEST_TEST_DURATION));
}

TEST(TestRANSAC, P3P_Fisheye_1000Correspondences_15Outliers)
{
	EXPECT_TRUE(TestRANSAC::testP3P(AnyCameraType::FISHEYE, 1000, 0.15, GTEST_TEST_DURATION));
}


TEST(TestRANSAC, P3PPinholeCamera)
{
	EXPECT_TRUE(TestRANSAC::testP3PPinholeCamera(GTEST_TEST_DURATION));
}

TEST(TestRANSAC, P3PZoom)
{
	EXPECT_TRUE(TestRANSAC::testP3PZoom(GTEST_TEST_DURATION));
}


TEST(TestRANSAC, ObjectTransformationStereoAnyCamera)
{
	EXPECT_TRUE(TestRANSAC::testObjectTransformationStereoAnyCamera(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestRANSAC::testP3P(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing P3P:";

	bool allSucceeded = true;

	for (const AnyCameraType anyCameraType : Utilities::realisticCameraTypes())
	{
		Log::info() << " ";

		if (anyCameraType == AnyCameraType::PINHOLE)
		{
			Log::info() << "Using a pinhole camera:";
		}
		else
		{
			ocean_assert(anyCameraType == AnyCameraType::FISHEYE);

			Log::info() << "Using a fisheye camera:";
		}

		for (const double faultyRate : {0.0, 0.15, 0.35})
		{
			Log::info() << " ";
			Log::info() << "With " << String::toAString(faultyRate * 100.0, 1u) << "% outliers:";

			for (const size_t correspondences : {10, 50, 100, 1000, 10000})
			{
				Log::info() << " ";
				Log::info() << "Using " << correspondences << " correspondences:";

				if (!testP3P(anyCameraType, correspondences, faultyRate, testDuration))
				{
					allSucceeded = false;
				}
			}
		}
	}

	return allSucceeded;
}

bool TestRANSAC::testP3P(const AnyCameraType anyCameraType, const size_t correspondences, const double faultyRate, const double testDuration)
{
	ocean_assert(anyCameraType != AnyCameraType::INVALID);
	ocean_assert(correspondences >= 4);
	ocean_assert(faultyRate >= 0.0 && faultyRate < 1.0);
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performance;

	for (const bool refine : {false, true})
	{
		if (refine)
		{
			Log::info() << "... with post refinement";
		}
		else
		{
			Log::info() << "... without refinement";
		}

		constexpr double successThreshold = std::is_same<Scalar, float>::value ? 0.35 : 0.99; // **TODO** temporary workaround

		ValidationPrecision validation(successThreshold, randomGenerator);

		const Timestamp startTimestamp(true);

		do
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const SharedAnyCamera sharedCamera = Utilities::realisticAnyCamera(anyCameraType, RandomI::random(randomGenerator, 1u));
			ocean_assert(sharedCamera);

			const AnyCamera& camera = *sharedCamera;

			const HomogenousMatrix4 world_T_camera(Random::vector3(randomGenerator, -10, 10), Random::quaternion(randomGenerator));
			const HomogenousMatrix4 flippedCamera_T_world(AnyCamera::standard2InvertedFlipped(world_T_camera));

			Vectors3 objectPoints;
			Vectors2 imagePoints;

			constexpr Scalar cameraBorder = Scalar(5);

			for (size_t n = 0; n < correspondences; ++n)
			{
				const Vector2 imagePoint = Random::vector2(randomGenerator, cameraBorder, Scalar(camera.width()) - cameraBorder, cameraBorder, Scalar(camera.height()) - cameraBorder);

				const Scalar distance = Random::scalar(randomGenerator, Scalar(0.1), Scalar(10));
				const Vector3 objectPoint = camera.ray(imagePoint, world_T_camera).point(distance);

				ocean_assert_and_suppress_unused(AnyCamera::isObjectPointInFrontIF(flippedCamera_T_world, objectPoint), flippedCamera_T_world);

				imagePoints.push_back(imagePoint);
				objectPoints.push_back(objectPoint);
			}

			const size_t faultyCorrespondences = size_t(double(correspondences) * faultyRate);
			ocean_assert(faultyCorrespondences < correspondences);

			const size_t validCorrespondences = correspondences - faultyCorrespondences;

			UnorderedIndexSet32 faultyIndices;

			while (faultyIndices.size() < faultyCorrespondences)
			{
				const unsigned int index = RandomI::random(randomGenerator, (unsigned int)(correspondences - 1));

				faultyIndices.emplace(index);

				if (RandomI::boolean(randomGenerator))
				{
					imagePoints[index] = Random::vector2(randomGenerator, cameraBorder, Scalar(camera.width()) - cameraBorder, cameraBorder, Scalar(camera.height()) - cameraBorder);
				}
				else
				{
					objectPoints[index] = Random::vector3(randomGenerator, -10, 10);
				}
			}

			const unsigned int ransacIterations = std::max(20u, Geometry::RANSAC::iterations(3u, Scalar(0.995), Scalar(faultyRate + 0.05)));

			constexpr unsigned int minimalValidCorrespondences = 4u;
			constexpr Scalar sqrPixelErrorThreshold = Scalar(5 * 5);

			Indices32 usedIndices;
			Scalar sqrAccuracy = Numeric::maxValue();

			const bool determineUseIndices = RandomI::boolean(randomGenerator);
			const bool determineSqrAccuracy = RandomI::boolean(randomGenerator);

			HomogenousMatrix4 world_T_ransacCamera;

			performance.start();
				const bool result = Geometry::RANSAC::p3p(camera, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), randomGenerator, world_T_ransacCamera, minimalValidCorrespondences, refine, ransacIterations, sqrPixelErrorThreshold, determineUseIndices ? &usedIndices : nullptr, determineSqrAccuracy ? &sqrAccuracy : nullptr);
			performance.stop();

			if (!result)
			{
				// OCEAN_SET_FAILED(validation); // **TODO** temporary workaround
				scopedIteration.setInaccurate();

				continue;
			}

			Scalar sumSqrDistances = 0;
			size_t numberPreciseCorrespondences = 0;

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const Vector2& imagePoint = imagePoints[n];
				const Vector3& objectPoint = objectPoints[n];

				const Vector2 projectedObjectPoint = camera.projectToImage(world_T_ransacCamera, objectPoint);

				const Scalar sqrDistance = imagePoint.sqrDistance(projectedObjectPoint);

				if (sqrDistance <= sqrPixelErrorThreshold)
				{
					sumSqrDistances += sqrDistance;
					++numberPreciseCorrespondences;
				}
			}

			if (numberPreciseCorrespondences >= validCorrespondences)
			{
				ocean_assert(numberPreciseCorrespondences != 0);

				const Scalar averageSqrDistance = sumSqrDistances / Scalar(numberPreciseCorrespondences);

				if (faultyCorrespondences == 0)
				{
					// no outliers, we need a perfect result

					if (averageSqrDistance > Numeric::sqr(Scalar(0.1)))
					{
						scopedIteration.setInaccurate();
					}
				}
				else
				{
					if (refine)
					{
						if (averageSqrDistance > Scalar(0.5 * 0.5))
						{
							scopedIteration.setInaccurate();
						}
					}
					else
					{
						if (averageSqrDistance > Scalar(1.5 * 1.5))
						{
							scopedIteration.setInaccurate();
						}
					}
				}
			}
			else
			{
				scopedIteration.setInaccurate();
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		Log::info() << "Performance: " << performance;

		Log::info() << "Validation: " << validation;

		if (!validation.succeeded())
		{
			allSucceeded = false;
		}
	}

	return allSucceeded;
}

bool TestRANSAC::testP3PPinholeCamera(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing RANSAC P3P with pinhole camera:";

	const PinholeCamera patternCamera(640, 480, Numeric::deg2rad(58));

	HighPerformanceStatistic performance;

	RandomGenerator randomGenerator;

	constexpr double threshold = std::is_same<float, Scalar>::value ? 0.85 : 0.95;

	ValidationPrecision validation(threshold, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (const DistortionType distortionType : {DT_NO_DISTORTION, DT_RADIAL_DISTORTION, DT_FULL_DISTORTION})
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			// determine random points inside a unit circle which are not collinear

			Vectors3 objectPoints;

			// create a distorted camera
			const PinholeCamera pinholeCamera(Utilities::distortedCamera(patternCamera, true, (distortionType & DT_RADIAL_DISTORTION) == DT_RADIAL_DISTORTION, (distortionType & DT_FULL_DISTORTION) == DT_FULL_DISTORTION, &randomGenerator));

			for (unsigned int n = 0u; n < 30u; ++n)
			{
				objectPoints.emplace_back(Random::scalar(randomGenerator, -1, 1), Random::scalar(randomGenerator, Scalar(-0.1), Scalar(0.1)), Random::scalar(randomGenerator, -1, 1));
			}

			const Euler euler(Random::euler(randomGenerator, Numeric::deg2rad(0), Numeric::deg2rad(30)));
			const Quaternion quaternion(euler);

			const HomogenousMatrix4 perfectPose(Utilities::viewPosition(pinholeCamera, objectPoints, quaternion * Vector3(0, -1, 0)));

			// determine the perfectly projected image points

			Vectors2 imagePoints(objectPoints.size());
			pinholeCamera.projectToImage<true>(perfectPose, objectPoints.data(), objectPoints.size(), true, imagePoints.data());

#ifdef OCEAN_DEBUG
			for (size_t n = 0; n < imagePoints.size(); ++n)
			{
				ocean_assert(pinholeCamera.isInside(imagePoints[n]));
			}
#endif

			IndexSet32 outlierIndices;
			while (outlierIndices.size() < 5)
			{
				outlierIndices.insert(RandomI::random(randomGenerator, (unsigned int)(objectPoints.size()) - 1u));
			}

			for (IndexSet32::const_iterator i = outlierIndices.begin(); i != outlierIndices.end(); ++i)
			{
				imagePoints[*i] = Random::vector2(randomGenerator, Scalar(0), Scalar(pinholeCamera.width()), Scalar(0), Scalar(pinholeCamera.height()));
			}

			constexpr Scalar pixelErrorThreshold = Scalar(3.5);

			performance.start();

			Indices32 validIndices;
			HomogenousMatrix4 pose;
			if (Geometry::RANSAC::p3p(pinholeCamera, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), randomGenerator, pinholeCamera.hasDistortionParameters(), pose, 5u, false, 50u, Numeric::sqr(pixelErrorThreshold), &validIndices))
			{
				performance.stop();

				Scalar maximalError = 0;
				for (Indices32::const_iterator i = validIndices.begin(); i != validIndices.end(); ++i)
				{
					maximalError = max(maximalError, imagePoints[*i].distance(pinholeCamera.projectToImage<true>(pose, objectPoints[*i], pinholeCamera.hasDistortionParameters())));
				}

				if (maximalError > pixelErrorThreshold || validIndices.size() + outlierIndices.size() < objectPoints.size())
				{
					scopedIteration.setInaccurate();
				}
			}
			else
			{
				performance.skip();

				OCEAN_SET_FAILED(validation);
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestRANSAC::testP3PZoom(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing RANSAC P3P with zoom:";

	const PinholeCamera patternCamera(640, 480, Numeric::deg2rad(58));

	HighPerformanceStatistic performance;

	RandomGenerator randomGenerator;

	ValidationPrecision validation(0.95, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (const DistortionType distortionType : {DT_NO_DISTORTION, DT_RADIAL_DISTORTION, DT_FULL_DISTORTION})
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			// determine random points inside a unit circle which are not collinear

			Vectors3 objectPoints;

			// create a distorted camera
			const PinholeCamera pinholeCamera(Utilities::distortedCamera(patternCamera, true, (distortionType & DT_RADIAL_DISTORTION) == DT_RADIAL_DISTORTION, (distortionType & DT_FULL_DISTORTION) == DT_FULL_DISTORTION));

			const Scalar perfectZoom = Random::scalar(randomGenerator, Scalar(0.1), 10);

			PinholeCamera zoomedCamera(pinholeCamera);
			zoomedCamera.applyZoomFactor(perfectZoom);

			for (unsigned int n = 0u; n < 30u; ++n)
			{
				objectPoints.emplace_back(Random::scalar(randomGenerator, -1, 1), Random::scalar(randomGenerator, Scalar(-0.1), Scalar(0.1)), Random::scalar(randomGenerator, -1, 1));
			}

			const Euler euler(Random::euler(randomGenerator, Numeric::deg2rad(0), Numeric::deg2rad(30)));
			const Quaternion quaternion(euler);

			const HomogenousMatrix4 perfectPose(Utilities::viewPosition(zoomedCamera, objectPoints, quaternion * Vector3(0, -1, 0)));

			for (unsigned int n = 0u; n < objectPoints.size(); ++n)
			{
				if (!PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(perfectPose), objectPoints[n]))
				{
					continue;
				}
			}

			// determine the perfectly projected image points

			Vectors2 imagePoints(objectPoints.size());
			zoomedCamera.projectToImage<true>(perfectPose, objectPoints.data(), objectPoints.size(), true, imagePoints.data());

#ifdef OCEAN_DEBUG
			for (size_t n = 0; n < imagePoints.size(); ++n)
			{
				ocean_assert(zoomedCamera.isInside(imagePoints[n]));
			}
#endif

			IndexSet32 outlierIndices;
			while (outlierIndices.size() < 5)
			{
				outlierIndices.insert(RandomI::random(randomGenerator, (unsigned int)objectPoints.size() - 1u));
			}

			for (IndexSet32::const_iterator i = outlierIndices.begin(); i != outlierIndices.end(); ++i)
			{
				imagePoints[*i] = Random::vector2(randomGenerator, Scalar(0), Scalar(pinholeCamera.width()), Scalar(0), Scalar(pinholeCamera.height()));
			}

			performance.start();

			Indices32 validIndices;
			HomogenousMatrix4 pose;
			Scalar zoom;
			if (Geometry::RANSAC::p3pZoom(pinholeCamera, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), randomGenerator, pinholeCamera.hasDistortionParameters(), pose, zoom, 5u, false, 50u, Scalar(3.5 * 3.5), &validIndices))
			{
				performance.stop();

				Scalar maximalError = 0;
				for (Indices32::const_iterator i = validIndices.begin(); i != validIndices.end(); ++i)
				{
					maximalError = max(maximalError, imagePoints[*i].distance(pinholeCamera.projectToImage<true>(pose, objectPoints[*i], pinholeCamera.hasDistortionParameters(), zoom)));
				}

				if (maximalError > 1.5 || validIndices.size() + outlierIndices.size() < objectPoints.size())
				{
					scopedIteration.setInaccurate();
				}
			}
			else
			{
				performance.skip();

				OCEAN_SET_FAILED(validation);
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestRANSAC::testObjectTransformationStereoAnyCamera(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Determination of 6-DOF object transformation for any stereo camera:";
	Log::info() << " ";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	for (const AnyCameraType anyCameraType : Utilities::realisticCameraTypes())
	{
		const std::shared_ptr<AnyCamera> anyCameraA = Utilities::realisticAnyCamera(anyCameraType, RandomI::random(randomGenerator, 1u));
		const std::shared_ptr<AnyCamera> anyCameraB = Utilities::realisticAnyCamera(anyCameraType, RandomI::random(randomGenerator, 1u));
		ocean_assert(anyCameraA && anyCameraB);

		Log::info() << " ";
		Log::info() << "Camera name: " << anyCameraA->name();

		ocean_assert(testDuration > 0.0);

		const double faultyRate = 0.15; // 15%

		unsigned long long iterations = 0ull;
		unsigned long long validIterations = 0ull;

		const Timestamp startTimestamp(true);

		do
		{
			const HomogenousMatrix4 world_T_object(Random::vector3(randomGenerator, -5, 5), Random::quaternion(randomGenerator));
			const HomogenousMatrix4 object_T_world = world_T_object.inverted();

			std::vector<Vectors3> objectPointGroups(2);
			std::vector<Vectors2> imagePointGroups(2);
			HomogenousMatrices4 world_T_cameras(2);

			std::vector<IndexSet32> faultyCorrespondenceGroups(2);

			for (unsigned int nCamera = 0u; nCamera < 2u; ++nCamera)
			{
				Vectors3& objectPoints = objectPointGroups[nCamera];
				Vectors2& imagePoints = imagePointGroups[nCamera];
				HomogenousMatrix4& world_T_camera = world_T_cameras[nCamera];
				IndexSet32& faultyCorrespondences = faultyCorrespondenceGroups[nCamera];

				const AnyCamera& anyCamera = nCamera == 0u ? *anyCameraA : *anyCameraB;

				world_T_camera = HomogenousMatrix4(Random::vector3(randomGenerator, -5, 5), Random::quaternion(randomGenerator));

				const unsigned int correspondences = RandomI::random(randomGenerator, 20u, 200u);

				objectPoints.reserve(correspondences);
				imagePoints.reserve(correspondences);

				for (unsigned int nCorrespondence = 0u; nCorrespondence < correspondences; ++nCorrespondence)
				{
					const Vector2 imagePoint = Random::vector2(randomGenerator, Scalar(10), Scalar(anyCamera.width() - 10u), Scalar(10), Scalar(anyCamera.height() - 10u));

					const Vector3 objectPoint = object_T_world * anyCamera.ray(imagePoint, world_T_camera).point(Random::scalar(randomGenerator, 1, 5));
					ocean_assert(anyCamera.projectToImage(world_T_camera, world_T_object * objectPoint).isEqual(imagePoint, 1));

					objectPoints.push_back(objectPoint);
					imagePoints.push_back(imagePoint);
				}

				const unsigned int numberFaultyCorrespondences = (unsigned int)(double(correspondences) * faultyRate + 0.5); // result can be zero

				while (faultyCorrespondences.size() < numberFaultyCorrespondences)
				{
					const unsigned int faultyIndex = RandomI::random(randomGenerator, correspondences - 1u);

					imagePoints[faultyIndex] = Random::vector2(randomGenerator, Scalar(10), Scalar(anyCamera.width() - 10u), Scalar(10), Scalar(anyCamera.height() - 10u));

					faultyCorrespondences.insert(faultyIndex);
				}
			}

			const unsigned int ransacIterations = Geometry::RANSAC::iterations(3u, Scalar(0.995), Scalar(faultyRate));

			HomogenousMatrix4 ransac_world_T_object;

			Indices32 usedIndicesA;
			Indices32 usedIndicesB;
			Scalar sqrAccuracy = Numeric::maxValue();

			if (Geometry::RANSAC::objectTransformationStereo(*anyCameraA, *anyCameraB, world_T_cameras[0], world_T_cameras[1], ConstArrayAccessor<Vector3>(objectPointGroups[0]), ConstArrayAccessor<Vector3>(objectPointGroups[1]), ConstArrayAccessor<Vector2>(imagePointGroups[0]), ConstArrayAccessor<Vector2>(imagePointGroups[1]), randomGenerator, ransac_world_T_object, 5u, true, ransacIterations, Scalar(5 * 5), &usedIndicesA, &usedIndicesB, &sqrAccuracy))
			{
				bool iterationIsValid = sqrAccuracy <= Scalar(5 * 5);

				for (unsigned int nCamera = 0u; nCamera < 2u; ++nCamera)
				{
					const Vectors3& objectPoints = objectPointGroups[nCamera];
					const IndexSet32& faultyCorrespondences = faultyCorrespondenceGroups[nCamera];

					const Indices32& usedIndices = nCamera == 0u ? usedIndicesA : usedIndicesB;

					const std::vector<uint8_t> faultyStatements = Subset::indices2statements<Index32, 1u>(faultyCorrespondences, objectPoints.size());
					const IndexSet32 usedIndexSet = IndexSet32(usedIndices.cbegin(), usedIndices.cend());

					ocean_assert(faultyCorrespondences.size() < objectPoints.size());
					if (usedIndices.size() < objectPoints.size() - faultyCorrespondences.size())
					{
						iterationIsValid = false;
					}
					else
					{
						for (size_t n = 0; n < faultyStatements.size(); ++n)
						{
							if (faultyStatements[n] == 0u)
							{
								// the correspondences is not faulty

								if (usedIndexSet.find(Index32(n)) == usedIndexSet.cend())
								{
									// although the correspondence is not faulty, it hasn't been used
									iterationIsValid = false;
								}
							}
						}
					}
				}

				if (iterationIsValid)
				{
					++validIterations;
				}
			}

			++iterations;
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);
		const double percent = double(validIterations) / double(iterations);

		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

		const double threshold = std::is_same<Scalar, double>::value ? 0.99 : 0.95;

		if (percent < threshold)
		{
			allSucceeded = false;
		}
	}

	return allSucceeded;
}

}

}

}
