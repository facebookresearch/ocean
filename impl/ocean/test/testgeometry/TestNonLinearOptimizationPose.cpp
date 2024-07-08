/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestNonLinearOptimizationPose.h"
#include "ocean/test/testgeometry/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/geometry/Error.h"
#include "ocean/geometry/NonLinearOptimizationPose.h"
#include "ocean/geometry/Utilities.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

bool TestNonLinearOptimizationPose::test(const double testDuration, Worker* /*worker*/)
{
	Log::info() << "---   Pose non linear optimization test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testNonLinearOptimizationPosePinholeCamera(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNonLinearOptimizationPoseAnyCamera(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNonLinearOptimizationPoseZoom(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Pose non linear optimization test succeeded.";
	}
	else
	{
		Log::info() << "Pose non linear optimization test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestNonLinearOptimizationPose, NonLinearOptimizationPosePinholeCamera_100Points_NoNoise)
{
	const PinholeCamera pinholeCamera(1280, 720, Numeric::deg2rad(60));

	for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
	{
		EXPECT_TRUE(TestNonLinearOptimizationPose::testNonLinearOptimizationPosePinholeCamera(pinholeCamera, 100u, GTEST_TEST_DURATION, estimatorType, Scalar(0), 0u, false));
	}
}

TEST(TestNonLinearOptimizationPose, NonLinearOptimizationPoseAnyCamera_100Points_NoNoise_NoCovariances)
{
	for (const AnyCameraType anyCameraType : Utilities::realisticCameraTypes())
	{
		const std::shared_ptr<AnyCamera> anyCamera = Utilities::realisticAnyCamera(anyCameraType, RandomI::random(1u));
		ocean_assert(anyCamera);

		for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
		{
			EXPECT_TRUE(TestNonLinearOptimizationPose::testNonLinearOptimizationPoseAnyCamera(*anyCamera, 100u, GTEST_TEST_DURATION, estimatorType, Scalar(0), 0u, false));
		}
	}
}

TEST(TestNonLinearOptimizationPose, NonLinearOptimizationPoseAnyCamera_100Points_NoNoise_Covariances)
{
	for (const AnyCameraType anyCameraType : Utilities::realisticCameraTypes())
	{
		const std::shared_ptr<AnyCamera> anyCamera = Utilities::realisticAnyCamera(anyCameraType, RandomI::random(1u));
		ocean_assert(anyCamera);

		for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
		{
			EXPECT_TRUE(TestNonLinearOptimizationPose::testNonLinearOptimizationPoseAnyCamera(*anyCamera, 100u, GTEST_TEST_DURATION, estimatorType, Scalar(0), 0u, true));
		}
	}
}

#endif // OCEAN_USE_GTEST

bool TestNonLinearOptimizationPose::testNonLinearOptimizationPosePinholeCamera(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Optimization of extrinsic pinhole camera (the 6DOF pose) parameters:";
	Log::info() << " ";

	bool result = true;

	const PinholeCamera pinholeCamera(1280, 720, Numeric::deg2rad(60));

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

			Log::info() << "Samples with Gaussian noise " << String::toAString(noise, 1u) << "px and " << outlier << "% outliers";
			Log::info() << " ";

			for (const unsigned int correspondences : {10u, 50u, 100u, 1000u})
			{
				if (correspondences != 10u)
				{
					Log::info() << " ";
				}

				Log::info() << "With " << correspondences << " correspondences";

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
						Log::info() << "... and " << Geometry::Estimator::translateEstimatorType(estimatorType) << ":";

						result = testNonLinearOptimizationPosePinholeCamera(pinholeCamera, correspondences, testDuration, estimatorType, noise, correspondences * outlier / 100u, useCovariances) && result;
					}
				}
			}
		}
	}

	return result;
}

bool TestNonLinearOptimizationPose::testNonLinearOptimizationPosePinholeCamera(const PinholeCamera& patternCamera, const unsigned int correspondences, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation, const unsigned int numberOutliers, const bool useCovariances)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(numberOutliers <= correspondences);

	uint64_t succeeded = 0ull;
	uint64_t iterations = 0ull;

	Scalar averagePixelError = 0;
	Scalar averageOptimizedPixelError = 0;

	Scalar averageTranslationError = 0;
	Scalar averageOptimizedTranslationError = 0;

	Scalar averageAngleError = 0;
	Scalar averageOptimizedAngleError = 0;

	HighPerformanceStatistic performance;

	Scalars medianPixelErrors;
	Scalars medianOptimizedPixelErrors;

	const Timestamp startTimestamp(true);

	do
	{
		// create a distorted camera
		const PinholeCamera pinholeCamera(Utilities::distortedCamera(patternCamera, true, iterations % 3u == 1u || iterations % 3u == 2u, iterations % 3u == 2u));

		const Vector3 translation(Random::vector3(-1, 1));
		const Euler euler(Random::euler(Numeric::deg2rad(10)));
		const Quaternion rotation(euler);

		const HomogenousMatrix4 world_T_camera(translation, rotation);
		const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera));

		Vectors2 imagePoints;
		Vectors2 perfectImagePoints;
		Vectors3 objectPoints;

		Matrix invertedCovariances(correspondences * 2u, 2u);

		for (unsigned int n = 0; n < correspondences; ++n)
		{
			Vector2 imagePoint(Random::scalar(40, Scalar(pinholeCamera.width() - 41)), Random::scalar(40, Scalar(pinholeCamera.height() - 41)));

			const Line3 ray(pinholeCamera.ray(imagePoint, world_T_camera));
			const Vector3 objectPoint(ray.point(Random::scalar(Scalar(0.9), Scalar(1.1))));

			imagePoint = pinholeCamera.projectToImageIF<true>(flippedCamera_T_world, objectPoint, pinholeCamera.hasDistortionParameters());

			Vector2 imagePointNoise(0, 0);
			if (standardDeviation > 0)
			{
				imagePointNoise = Vector2(Random::gaussianNoise(standardDeviation), Random::gaussianNoise(standardDeviation));

				if (useCovariances)
				{
					const SquareMatrix2 covariance(Geometry::Utilities::covarianceMatrix(imagePointNoise, standardDeviation));
					const SquareMatrix2 invertedCovariance(covariance.inverted());

					invertedCovariance.copyElements(invertedCovariances[2u * n + 0u], false);
				}
			}
			else if (useCovariances)
			{
				SquareMatrix2(true).copyElements(invertedCovariances[2u * n + 0u], false);
			}

			perfectImagePoints.push_back(imagePoint);
			imagePoints.push_back(imagePoint + imagePointNoise);
			objectPoints.push_back(objectPoint);
		}

		const IndexSet32 outlierSet(Utilities::randomIndices(correspondences - 1, numberOutliers));
		for (IndexSet32::const_iterator i = outlierSet.begin(); i != outlierSet.end(); ++i)
		{
			const Vector2 outlierNoise(Random::gaussianNoise(100), Random::gaussianNoise(100));
			imagePoints[*i] += outlierNoise;
		}

		const Vector3 errorTranslation(Random::vector3(Scalar(-0.1), Scalar(0.1)));
		const Euler errorEuler(Random::euler(Numeric::deg2rad(10)));
		const Quaternion errorRotation(errorEuler);

		const Vector3 faultyTranslation(translation + errorTranslation);
		const Quaternion faultyRotation(rotation * errorRotation);

		averageTranslationError += (translation - faultyTranslation).length();
		averageAngleError += Numeric::rad2deg(rotation.smallestAngle(faultyRotation));

		const HomogenousMatrix4 faultyPose(faultyTranslation, faultyRotation);
		HomogenousMatrix4 faultyPoseIF(PinholeCamera::standard2InvertedFlipped(faultyPose));

		Scalar totalError = 0;
		for (unsigned int n = 0; n < correspondences; ++n)
		{
			if (outlierSet.find(n) == outlierSet.end())
			{
				const Vector2& imagePoint = imagePoints[n];
				const Vector3& objectPoint = objectPoints[n];

				const Vector2 projectedPoint = pinholeCamera.projectToImageIF<true, true>(faultyPoseIF, objectPoint);
				const Scalar error = (imagePoint - projectedPoint).sqr();
				totalError += error;
			}
		}

		averagePixelError += totalError / Scalar(correspondences);
		medianPixelErrors.push_back(totalError / Scalar(correspondences));

		HomogenousMatrix4 optimizedPoseIF;

		performance.start();

		if (Geometry::NonLinearOptimizationPose::optimizePoseIF(pinholeCamera, faultyPoseIF, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), pinholeCamera.hasDistortionParameters(), optimizedPoseIF, 20, type, Scalar(0.001), Scalar(5.0), nullptr, nullptr, useCovariances ? &invertedCovariances : nullptr))
		{
			performance.stop();

			const HomogenousMatrix4 optimizedPose(PinholeCamera::invertedFlipped2Standard(optimizedPoseIF));

			const Vector3 optimizedTranslation(optimizedPose.translation());
			const Quaternion optimizedRotation(optimizedPose.rotation());

			averageOptimizedTranslationError += (translation - optimizedTranslation).length();
			averageOptimizedAngleError += Numeric::rad2deg(rotation.smallestAngle(optimizedRotation));

			const Scalar totalOptimizedError = Geometry::Error::determinePoseErrorIF<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, false, false>(optimizedPoseIF, pinholeCamera, ConstTemplateArrayAccessor<Vector3>(objectPoints.data(), objectPoints.size()), ConstTemplateArrayAccessor<Vector2>(perfectImagePoints.data(), objectPoints.size()), pinholeCamera.hasDistortionParameters());

			// if we do not have any noise or outliers, the result must be perfect
			ocean_assert(standardDeviation != 0 || numberOutliers != 0u || Numeric::isWeakEqualEps(totalOptimizedError));

			medianOptimizedPixelErrors.push_back(totalOptimizedError);

			averageOptimizedPixelError += totalOptimizedError;

			const Vector3 translationDifference(translation - optimizedTranslation);
			const Scalar angleDifference = Numeric::rad2deg(rotation.smallestAngle(optimizedRotation));

			if (translationDifference.length() < 0.1 && angleDifference < 5)
			{
				++succeeded;
			}
		}
		else
		{
			performance.skip();
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	averageTranslationError /= Scalar(iterations);
	averageAngleError /= Scalar(iterations);
	averagePixelError /= Scalar(iterations);

	averageOptimizedTranslationError /= Scalar(iterations);
	averageOptimizedAngleError /= Scalar(iterations);
	averageOptimizedPixelError /= Scalar(iterations);

	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Average translation error: " << String::toAString(averageTranslationError, 2u) << " -> " << String::toAString(averageOptimizedTranslationError, 2u);
	Log::info() << "Average angle error: " << String::toAString(averageAngleError, 1u) << "deg -> " << String::toAString(averageOptimizedAngleError, 1u) << "deg";
	Log::info() << "Average sqr pixel error: " << String::toAString(averagePixelError, 1u) << "px -> " << String::toAString(averageOptimizedPixelError, 1u) << "px";
	Log::info() << "Median sqr pixel error: " << String::toAString(Median::constMedian(medianPixelErrors.data(), medianPixelErrors.size()), 1u) << "px -> " << String::toAString(Median::constMedian(medianOptimizedPixelErrors.data(), medianOptimizedPixelErrors.size()), 1u) << "px";
	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 4u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 4u) << "ms, average: " << String::toAString(performance.averageMseconds(), 4u) << "ms, median: " << String::toAString(performance.medianMseconds(), 4u) << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	if (std::is_same<double, Scalar>::value && standardDeviation == 0 && numberOutliers == 0u)
	{
		return NumericD::isEqual(percent, 1.0);
	}

	return true;
}

bool TestNonLinearOptimizationPose::testNonLinearOptimizationPoseAnyCamera(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Optimization of 6-DOF pose with any camera:";

	bool result = true;

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

				Log::info() << "Samples with Gaussian noise " << String::toAString(noise, 1u) << "px and " << outlier << "% outliers";
				Log::info() << " ";

				for (const unsigned int correspondences : {10u, 50u, 100u, 1000u})
				{
					if (correspondences != 0u)
					{
						Log::info() << " ";
					}

					Log::info() << "With " << correspondences << " correspondences";

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
							Log::info() << "... and " << Geometry::Estimator::translateEstimatorType(estimatorType) << ":";

							result = testNonLinearOptimizationPoseAnyCamera(*anyCamera, correspondences, testDuration, estimatorType, noise, correspondences * outlier / 100u, useCovariances) && result;
						}
					}
				}
			}
		}

		Log::info() << " ";
	}

	return result;
}

bool TestNonLinearOptimizationPose::testNonLinearOptimizationPoseAnyCamera(const AnyCamera& anyCamera, const unsigned int correspondences, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation, const unsigned int numberOutliers, const bool useCovariances)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(testDuration > 0.0);
	ocean_assert(numberOutliers <= correspondences);

	uint64_t succeeded = 0ull;
	uint64_t iterations = 0ull;

	Scalar averagePixelError = 0;
	Scalar averageOptimizedPixelError = 0;

	Scalar averageTranslationError = 0;
	Scalar averageOptimizedTranslationError = 0;

	Scalar averageAngleError = 0;
	Scalar averageOptimizedAngleError = 0;

	HighPerformanceStatistic performance;

	Scalars medianPixelErrors;
	Scalars medianOptimizedPixelErrors;

	const Timestamp startTimestamp(true);

	do
	{
		const Vector3 translation(Random::vector3(-5, 5));
		const Quaternion rotation(Random::quaternion());

		const HomogenousMatrix4 world_T_camera(translation, rotation);
		const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera));

		Vectors2 distortedImagePoints;
		Vectors2 distortedNoisedImagePoints;
		Vectors3 objectPoints;

		Matrix invertedCovariances(correspondences * 2u, 2u);

		for (unsigned int n = 0; n < correspondences; ++n)
		{
			const Vector2 distortedImagePoint(Random::vector2(50, Scalar(anyCamera.width() - 50u), 50, Scalar(anyCamera.height() - 50u)));

			const Line3 ray(anyCamera.ray(distortedImagePoint, world_T_camera));
			const Vector3 objectPoint(ray.point(Random::scalar(Scalar(0.5), Scalar(5.0))));

			ocean_assert_and_suppress_unused(distortedImagePoint.sqrDistance(anyCamera.projectToImageIF(flippedCamera_T_world, objectPoint)) < Scalar(1 * 1), flippedCamera_T_world);

			Vector2 imagePointNoise(0, 0);
			if (standardDeviation > 0)
			{
				imagePointNoise = Vector2(Random::gaussianNoise(standardDeviation), Random::gaussianNoise(standardDeviation));

				if (useCovariances)
				{
					const SquareMatrix2 covariance(Geometry::Utilities::covarianceMatrix(imagePointNoise, standardDeviation));
					const SquareMatrix2 invertedCovariance(covariance.inverted());

					invertedCovariance.copyElements(invertedCovariances[2u * n + 0u], false);
				}
			}
			else if (useCovariances)
			{
				SquareMatrix2(true).copyElements(invertedCovariances[2u * n + 0u], false);
			}

			distortedImagePoints.push_back(distortedImagePoint);
			distortedNoisedImagePoints.push_back(distortedImagePoint + imagePointNoise);
			objectPoints.push_back(objectPoint);
		}

		const IndexSet32 outlierSet(Utilities::randomIndices(correspondences - 1, numberOutliers));
		for (IndexSet32::const_iterator i = outlierSet.begin(); i != outlierSet.end(); ++i)
		{
			distortedNoisedImagePoints[*i] = Random::vector2(50, Scalar(anyCamera.width() - 50u), 50, Scalar(anyCamera.height() - 50u));
		}

		const Vector3 errorTranslation(Random::vector3(Scalar(-0.1), Scalar(0.1)));
		const Euler errorEuler(Random::euler(Numeric::deg2rad(10)));
		const Quaternion errorRotation(errorEuler);

		const Vector3 faultyTranslation(translation + errorTranslation);
		const Quaternion faultyRotation(rotation * errorRotation);

		averageTranslationError += (translation - faultyTranslation).length();
		averageAngleError += Numeric::rad2deg(rotation.smallestAngle(faultyRotation));

		const HomogenousMatrix4 faultyPose(faultyTranslation, faultyRotation);
		HomogenousMatrix4 faultyPoseIF(PinholeCamera::standard2InvertedFlipped(faultyPose));

		Scalar totalError = 0;
		for (unsigned int n = 0; n < correspondences; ++n)
		{
			if (outlierSet.find(n) == outlierSet.end())
			{
				const Vector2& distortedNoisedImagePoint = distortedNoisedImagePoints[n];
				const Vector3& objectPoint = objectPoints[n];

				const Vector2 projectedPoint = anyCamera.projectToImageIF(faultyPoseIF, objectPoint);
				const Scalar error = (distortedNoisedImagePoint - projectedPoint).sqr();
				totalError += error;
			}
		}

		averagePixelError += totalError / Scalar(correspondences);
		medianPixelErrors.push_back(totalError / Scalar(correspondences));

		HomogenousMatrix4 optimizedPoseIF;

		performance.start();

		if (Geometry::NonLinearOptimizationPose::optimizePoseIF(anyCamera, faultyPoseIF, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(distortedNoisedImagePoints), optimizedPoseIF, 20u, type, Scalar(0.001), Scalar(5.0), nullptr, nullptr, useCovariances ? &invertedCovariances : nullptr))
		{
			performance.stop();

			const HomogenousMatrix4 optimizedPose(PinholeCamera::invertedFlipped2Standard(optimizedPoseIF));

			const Vector3 optimizedTranslation(optimizedPose.translation());
			const Quaternion optimizedRotation(optimizedPose.rotation());

			averageOptimizedTranslationError += (translation - optimizedTranslation).length();
			averageOptimizedAngleError += Numeric::rad2deg(rotation.smallestAngle(optimizedRotation));

			const Scalar totalOptimizedError = Geometry::Error::determinePoseErrorIF<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, false, false>(optimizedPoseIF, anyCamera, ConstTemplateArrayAccessor<Vector3>(objectPoints.data(), objectPoints.size()), ConstTemplateArrayAccessor<Vector2>(distortedImagePoints.data(), distortedImagePoints.size()));

			medianOptimizedPixelErrors.push_back(totalOptimizedError);

			averageOptimizedPixelError += totalOptimizedError;

			const Vector3 translationDifference(translation - optimizedTranslation);
			const Scalar angleDifference = Numeric::rad2deg(rotation.smallestAngle(optimizedRotation));

			if (translationDifference.length() < 0.1 && angleDifference < 5)
			{
				++succeeded;
			}
		}
		else
		{
			performance.skip();
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	averageTranslationError /= Scalar(iterations);
	averageAngleError /= Scalar(iterations);
	averagePixelError /= Scalar(iterations);

	averageOptimizedTranslationError /= Scalar(iterations);
	averageOptimizedAngleError /= Scalar(iterations);
	averageOptimizedPixelError /= Scalar(iterations);

	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Average translation error: " << String::toAString(averageTranslationError, 2u) << " -> " << String::toAString(averageOptimizedTranslationError, 2u);
	Log::info() << "Average angle error: " << String::toAString(averageAngleError, 1u) << "deg -> " << String::toAString(averageOptimizedAngleError, 1u) << "deg";
	Log::info() << "Average sqr pixel error: " << String::toAString(averagePixelError, 1u) << "px -> " << String::toAString(averageOptimizedPixelError, 1u) << "px";
	Log::info() << "Median sqr pixel error: " << String::toAString(Median::constMedian(medianPixelErrors.data(), medianPixelErrors.size()), 1u) << "px -> " << String::toAString(Median::constMedian(medianOptimizedPixelErrors.data(), medianOptimizedPixelErrors.size()), 1u) << "px";
	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 4u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 4u) << "ms, average: " << String::toAString(performance.averageMseconds(), 4u) << "ms, median: " << String::toAString(performance.medianMseconds(), 4u) << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	if (std::is_same<double, Scalar>::value && standardDeviation == 0 && numberOutliers == 0u)
	{
		return percent >= 0.95;
	}

	return true;
}

bool TestNonLinearOptimizationPose::testNonLinearOptimizationPoseZoom(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Optimization of extrinsic camera (the 6DOF pose) parameters and a zoom factor:";
	Log::info() << " ";

	bool result = true;

	const PinholeCamera pinholeCamera(640, 480, Numeric::deg2rad(60));

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

			Log::info() << "Samples with Gaussian noise " << String::toAString(noise, 1u) << "px and " << outlier << "% outliers";
			Log::info() << " ";

			for (const unsigned int correspondences : {10u, 50u, 100u, 1000u})
			{
				if (correspondences != 10u)
				{
					Log::info() << " ";
				}

				Log::info() << "With " << correspondences << " correspondences";

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
						Log::info() << "... and " << Geometry::Estimator::translateEstimatorType(estimatorType) << ":";

						result = testNonLinearOptimizationPoseZoom(pinholeCamera, correspondences, testDuration, estimatorType, noise, correspondences * outlier / 100u, useCovariances) && result;
					}
				}
			}
		}
	}

	return result;
}

bool TestNonLinearOptimizationPose::testNonLinearOptimizationPoseZoom(const PinholeCamera& patternCamera, const unsigned int correspondences, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation, const unsigned int numberOutliers, const bool useCovariances)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(numberOutliers <= correspondences);

	uint64_t succeeded = 0ull;
	uint64_t iterations = 0ull;

	Scalar averagePixelError = 0;
	Scalar averageOptimizedPixelError = 0;

	Scalar averageTranslationError = 0;
	Scalar averageOptimizedTranslationError = 0;

	Scalar averageAngleError = 0;
	Scalar averageOptimizedAngleError = 0;

	Scalar averageZoomError = 0;
	Scalar averageOptimizedZoomError = 0;

	HighPerformanceStatistic performance;

	Scalars medianPixelErrors;
	Scalars medianOptimizedPixelErrors;

	const Timestamp startTimestamp(true);

	do
	{
		// create a distorted camera
		const PinholeCamera pinholeCamera(Utilities::distortedCamera(patternCamera, true, iterations % 3u == 1u || iterations % 3u == 2u, iterations % 3u == 2u));

		const Scalar zoom = (iterations % 3u == 0u) ? Scalar(1) : Random::scalar(0.5, 10);

		const Vector3 translation(Random::vector3(-1, 1));
		const Euler euler(Random::euler(Numeric::deg2rad(10)));
		const Quaternion rotation(euler);

		const HomogenousMatrix4 pose(translation, rotation);
		const HomogenousMatrix4 poseIF(PinholeCamera::standard2InvertedFlipped(pose));

		Vectors2 imagePoints;
		Vectors2 perfectImagePoints;
		Vectors3 objectPoints;

		Matrix invertedCovariances(correspondences * 2u, 2u);

		for (unsigned int n = 0; n < correspondences; ++n)
		{
			Vector2 imagePoint(Random::scalar(40, Scalar(pinholeCamera.width() - 41)), Random::scalar(40, Scalar(pinholeCamera.height() - 41)));

			const Line3 ray(pinholeCamera.ray(imagePoint, pose, zoom));
			const Vector3 objectPoint(ray.point(Random::scalar(Scalar(0.9), Scalar(1.1))));

			imagePoint = pinholeCamera.projectToImageIF<true>(poseIF, objectPoint, pinholeCamera.hasDistortionParameters(), zoom);

			Vector2 imagePointNoise(0, 0);
			if (standardDeviation > 0)
			{
				imagePointNoise = Vector2(Random::gaussianNoise(standardDeviation), Random::gaussianNoise(standardDeviation));

				if (useCovariances)
				{
					const SquareMatrix2 covariance(Geometry::Utilities::covarianceMatrix(imagePointNoise, standardDeviation));
					const SquareMatrix2 invertedCovariance(covariance.inverted());

					invertedCovariance.copyElements(invertedCovariances[2 * n + 0u], false);
				}
			}
			else if (useCovariances)
			{
				SquareMatrix2(true).copyElements(invertedCovariances[2 * n + 0u], false);
			}

			perfectImagePoints.push_back(imagePoint);
			imagePoints.push_back(imagePoint + imagePointNoise);
			objectPoints.push_back(objectPoint);
		}

		const IndexSet32 outlierSet(Utilities::randomIndices(correspondences - 1, numberOutliers));
		for (IndexSet32::const_iterator i = outlierSet.begin(); i != outlierSet.end(); ++i)
		{
			const Vector2 outlierNoise(Random::gaussianNoise(100), Random::gaussianNoise(100));
			imagePoints[*i] += outlierNoise;
		}

		const Vector3 errorTranslation(Random::vector3(Scalar(-0.1), Scalar(0.1)));
		const Euler errorEuler(Random::euler(Numeric::deg2rad(10)));
		const Quaternion errorRotation(errorEuler);

		const Vector3 faultyTranslation(translation + errorTranslation);
		const Quaternion faultyRotation(rotation * errorRotation);

		const Scalar faultyZoom = minmax<Scalar>(Scalar(0.0001), zoom * Random::scalar(Scalar(0.1), 10), 100); // we take a quite random zoom factor

		averageTranslationError += (translation - faultyTranslation).length();
		averageAngleError += Numeric::rad2deg(rotation.smallestAngle(faultyRotation));
		averageZoomError += Numeric::abs(zoom - faultyZoom);

		const HomogenousMatrix4 faultyPose(faultyTranslation, faultyRotation);
		HomogenousMatrix4 faultyPoseIF(PinholeCamera::standard2InvertedFlipped(faultyPose));

		Scalar totalError = 0;
		for (unsigned int n = 0; n < correspondences; ++n)
		{
			if (outlierSet.find(n) == outlierSet.end())
			{
				const Vector2& imagePoint = imagePoints[n];
				const Vector3& objectPoint = objectPoints[n];

				const Vector2 projectedPoint = pinholeCamera.projectToImageIF<true, true>(faultyPoseIF, objectPoint, faultyZoom);
				const Scalar error = (imagePoint - projectedPoint).sqr();
				totalError += error;
			}
		}

		averagePixelError += totalError / Scalar(correspondences);
		medianPixelErrors.push_back(totalError / Scalar(correspondences));

		HomogenousMatrix4 optimizedPoseIF;
		Scalar optimizedZoom;

		performance.start();

		if (Geometry::NonLinearOptimizationPose::optimizePoseZoomIF(pinholeCamera, faultyPoseIF, faultyZoom, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), pinholeCamera.hasDistortionParameters(), optimizedPoseIF, optimizedZoom, 50u, type, Scalar(0.001), Scalar(5.0), nullptr, nullptr, useCovariances ? &invertedCovariances : nullptr))
		{
			performance.stop();

			const HomogenousMatrix4 optimizedPose(PinholeCamera::invertedFlipped2Standard(optimizedPoseIF));

			const Vector3 optimizedTranslation(optimizedPose.translation());
			const Quaternion optimizedRotation(optimizedPose.rotation());

			averageOptimizedTranslationError += (translation - optimizedTranslation).length();
			averageOptimizedAngleError += Numeric::rad2deg(rotation.smallestAngle(optimizedRotation));
			averageOptimizedZoomError += Numeric::abs(zoom - optimizedZoom);

			const Scalar totalOptimizedError = Geometry::Error::determinePoseErrorIF<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, false, false>(optimizedPoseIF, pinholeCamera, ConstTemplateArrayAccessor<Vector3>(objectPoints.data(), objectPoints.size()), ConstTemplateArrayAccessor<Vector2>(perfectImagePoints.data(), objectPoints.size()), pinholeCamera.hasDistortionParameters(), optimizedZoom);
			medianOptimizedPixelErrors.push_back(totalOptimizedError);

			averageOptimizedPixelError += totalOptimizedError;

			const Vector3 translationDifference(translation - optimizedTranslation);
			const Scalar angleDifference = Numeric::rad2deg(rotation.smallestAngle(optimizedRotation));
			const Scalar zoomDifference = Numeric::abs(zoom - optimizedZoom);

			if (translationDifference.length() < 0.1 && angleDifference < 5 && zoomDifference < 0.5)
			{
				++succeeded;
			}
		}
		else
		{
			performance.skip();
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	averageTranslationError /= Scalar(iterations);
	averageAngleError /= Scalar(iterations);
	averageZoomError /= Scalar(iterations);
	averagePixelError /= Scalar(iterations);

	averageOptimizedTranslationError /= Scalar(iterations);
	averageOptimizedAngleError /= Scalar(iterations);
	averageOptimizedZoomError /= Scalar(iterations);
	averageOptimizedPixelError /= Scalar(iterations);

	Log::info() << "Average translation error: " << String::toAString(averageTranslationError, 2u) << " -> " << String::toAString(averageOptimizedTranslationError, 2u);
	Log::info() << "Average angle error: " << String::toAString(averageAngleError, 1u) << "deg -> " << String::toAString(averageOptimizedAngleError, 1u) << "deg";
	Log::info() << "Average zoom error: " << String::toAString(averageZoomError, 1u) << "x -> " << String::toAString(averageOptimizedZoomError, 1u) << "x";
	Log::info() << "Average sqr pixel error: " << String::toAString(averagePixelError, 1u) << "px -> " << String::toAString(averageOptimizedPixelError, 1u) << "px";
	Log::info() << "Median sqr pixel error: " << String::toAString(Median::constMedian(medianPixelErrors.data(), medianPixelErrors.size()), 1u) << "px -> " << String::toAString(Median::constMedian(medianOptimizedPixelErrors.data(), medianOptimizedPixelErrors.size()), 1u) << "px";
	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 4u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 4u) << "ms, average: " << String::toAString(performance.averageMseconds(), 4u) << "ms, median: " << String::toAString(performance.medianMseconds(), 4u) << "ms";
	Log::info() << "Validation: " << String::toAString(double(succeeded) * 100.0 / double(iterations), 1u) << "% succeeded.";

	return true;
}

}

}

}
