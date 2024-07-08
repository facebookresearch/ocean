/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestNonLinearOptimizationOrientation.h"
#include "ocean/test/testgeometry/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/geometry/NonLinearOptimizationOrientation.h"
#include "ocean/geometry/RANSAC.h"

#include "ocean/math/Random.h"

#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

bool TestNonLinearOptimizationOrientation::test(const double testDuration, Worker* /*worker*/)
{
	Log::info() << "---   Orientation non linear optimization test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testOptimizeOrientation(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Orientation non linear optimization test succeeded.";
	}
	else
	{
		Log::info() << "Orientation non linear optimization test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestNonLinearOptimizationOrientation, OptimizeOrientation_50Correspondences_NoOutliers_NoNoise)
{
	constexpr unsigned int numberCorrespondences = 50u;

	constexpr Scalar noise = Scalar(0);
	constexpr unsigned int percentOutliers = 0u;

	for (const bool useRoughOrientation : {false, true})
	{
		for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
		{
			EXPECT_TRUE(TestNonLinearOptimizationOrientation::testOptimizeOrientation(numberCorrespondences, GTEST_TEST_DURATION, estimatorType, noise, numberCorrespondences * percentOutliers / 100u, useRoughOrientation));
		}
	}
}

TEST(TestNonLinearOptimizationOrientation, OptimizeOrientation_50Correspondences_Outliers_NoNoise)
{
	constexpr unsigned int numberCorrespondences = 50u;

	constexpr Scalar noise = Scalar(0);
	constexpr unsigned int percentOutliers = 10u;

	for (const bool useRoughOrientation : {false, true})
	{
		for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
		{
			EXPECT_TRUE(TestNonLinearOptimizationOrientation::testOptimizeOrientation(numberCorrespondences, GTEST_TEST_DURATION, estimatorType, noise, numberCorrespondences * percentOutliers / 100u, useRoughOrientation));
		}
	}
}

TEST(TestNonLinearOptimizationOrientation, OptimizeOrientation_50Correspondences_NoOutliers_Noise)
{
	constexpr unsigned int numberCorrespondences = 50u;

	constexpr Scalar noise = Scalar(1);
	constexpr unsigned int percentOutliers = 0u;

	for (const bool useRoughOrientation : {false, true})
	{
		for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
		{
			EXPECT_TRUE(TestNonLinearOptimizationOrientation::testOptimizeOrientation(numberCorrespondences, GTEST_TEST_DURATION, estimatorType, noise, numberCorrespondences * percentOutliers / 100u, useRoughOrientation));
		}
	}
}

TEST(TestNonLinearOptimizationOrientation, OptimizeOrientation_50Correspondences_Outliers_Noise)
{
	constexpr unsigned int numberCorrespondences = 50u;

	constexpr Scalar noise = Scalar(1);
	constexpr unsigned int percentOutliers = 10u;

	for (const bool useRoughOrientation : {false, true})
	{
		for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
		{
			EXPECT_TRUE(TestNonLinearOptimizationOrientation::testOptimizeOrientation(numberCorrespondences, GTEST_TEST_DURATION, estimatorType, noise, numberCorrespondences * percentOutliers / 100u, useRoughOrientation));
		}
	}
}

#endif // OCEAN_USE_GTEST

bool TestNonLinearOptimizationOrientation::testOptimizeOrientation(const double testDuration)
{
	ocean_assert(testDuration > 0);

	Log::info() << "Optimization of 3-DOF camera orientation:";
	Log::info() << " ";

	bool allSucceeded = true;

	for (const bool useRoughOrientation : {true, false})
	{
		Log::info().newLine();
		Log::info().newLine();

		if (useRoughOrientation)
		{
			Log::info() << "... with rough orientation";
		}
		else
		{
			Log::info() << "... without rough orientation";
		}

		for (const unsigned int outlier : {0u, 10u})
		{
			for (const Scalar noise : {Scalar(0), Scalar(1)})
			{
				Log::info().newLine();
				Log::info().newLine();

				Log::info() << "Samples with Gaussian noise " << String::toAString(noise, 1u) << "px and " << outlier << "% outliers:";
				Log::info() << " ";

				for (const unsigned int correspondences : {10u, 20u, 50u, 500u})
				{
					if (correspondences != 10u)
					{
						Log::info() << " ";
					}

					Log::info() << "With " << correspondences << " correspondences";

					for (Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
					{
						Log::info() << "... and " << Geometry::Estimator::translateEstimatorType(estimatorType) << ":";

						allSucceeded = testOptimizeOrientation(correspondences, testDuration, estimatorType, noise, correspondences * outlier / 100u, useRoughOrientation) && allSucceeded;
					}
				}
			}
		}
	}

	return allSucceeded;
}

bool TestNonLinearOptimizationOrientation::testOptimizeOrientation(const unsigned int correspondences, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation, const unsigned int numberOutliers, const bool useRoughOrientation)
{
	ocean_assert(testDuration > 0);
	ocean_assert(numberOutliers <= correspondences);

	Scalars initialErrors;
	Scalars optimizedErrors;

	HighPerformanceStatistic performance;

	enum DistortionType : uint32_t
	{
		DT_NO_DISTORTION = 0u,
		DT_RADIAL_DISTORTION = 1u << 0u | DT_NO_DISTORTION,
		DT_FULL_DISTORTION = (1u << 1u) | DT_RADIAL_DISTORTION
	};

	const PinholeCamera patternCamera(1280, 720, Numeric::deg2rad(60));

	RandomGenerator randomGenerator;

	ValidationPrecision validation(0.99, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (const DistortionType distortionType : {DT_NO_DISTORTION, DT_RADIAL_DISTORTION, DT_FULL_DISTORTION})
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const AnyCameraPinhole camera(Utilities::distortedCamera(patternCamera, true, (distortionType & DT_RADIAL_DISTORTION) == DT_RADIAL_DISTORTION, (distortionType & DT_FULL_DISTORTION) == DT_FULL_DISTORTION));

			const Quaternion world_R_camera(Random::quaternion(randomGenerator));

			const HomogenousMatrix4 world_T_camera(world_R_camera);

			Vectors2 perfectImagePoints;
			Vectors3 objectPoints;

			for (unsigned int n = 0u; n < correspondences; ++n)
			{
				constexpr Scalar cameraBorder = Scalar(20);

				const Vector2 imagePoint = Random::vector2(randomGenerator, cameraBorder, Scalar(camera.width()) - cameraBorder, cameraBorder, Scalar(camera.height()) - cameraBorder);

				const Line3 ray(camera.ray(imagePoint, world_T_camera));
				const Vector3 objectPoint(ray.point(Random::scalar(randomGenerator, Scalar(0.9), Scalar(1.1))));

				perfectImagePoints.push_back(imagePoint);
				objectPoints.push_back(objectPoint);
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
				const unsigned int index = RandomI::random(randomGenerator, correspondences - 1u);

				if (outlierSet.emplace(index).second)
				{
					const Scalar sign = Random::sign(randomGenerator);

					imagePoints[index] += Random::vector2(randomGenerator, Scalar(10), Scalar(100)) * sign;
				}
			}

			performance.start();

			if (useRoughOrientation)
			{
				const Euler faultyEuler(Random::euler(randomGenerator, Numeric::deg2rad(20)));
				const SquareMatrix3 world_R_roughCamera = SquareMatrix3(world_R_camera * Quaternion(faultyEuler));

				SquareMatrix3 optimizedOrientation;
				Scalar initialError, finalError;
				if (Geometry::NonLinearOptimizationOrientation::optimizeOrientation(camera, world_R_roughCamera, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), optimizedOrientation, 20u, type, Scalar(0.001), Scalar(5), &initialError, &finalError))
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
				SquareMatrix3 world_T_ransacCamera(false);

				Indices32 usedIndices;
				if (Geometry::RANSAC::orientation(camera, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), randomGenerator, world_T_ransacCamera, 3u, 50u, Scalar(5 * 5), nullptr, &usedIndices))
				{
					SquareMatrix3 optimizedOrientation;
					Scalar initialError, finalError;
					if (Geometry::NonLinearOptimizationOrientation::optimizeOrientation(camera, world_T_ransacCamera, ConstArraySubsetAccessor<Vector3, unsigned int>(objectPoints, usedIndices), ConstArraySubsetAccessor<Vector2, unsigned int>(imagePoints, usedIndices), optimizedOrientation, 20u, type, Scalar(0.001), Scalar(5), &initialError, &finalError))
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
	while (validation.needMoreIterations() || startTimestamp + testDuration > Timestamp(true));

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
				if (useRoughOrientation)
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
