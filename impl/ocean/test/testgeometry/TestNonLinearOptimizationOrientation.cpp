// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/test/testgeometry/TestNonLinearOptimizationOrientation.h"
#include "ocean/test/testgeometry/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/geometry/NonLinearOptimizationOrientation.h"
#include "ocean/geometry/RANSAC.h"

#include "ocean/math/Random.h"

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

bool TestNonLinearOptimizationOrientation::testOptimizeOrientation(const double testDuration)
{
	ocean_assert(testDuration > 0);

	Log::info() << "Optimization of 3DOF camera orientation:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testOptimizeOrientation(10u, testDuration, Geometry::Estimator::ET_SQUARE, 0, 0u, true) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testOptimizeOrientation(20u, testDuration, Geometry::Estimator::ET_SQUARE, 0, 0u, true) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testOptimizeOrientation(50u, testDuration, Geometry::Estimator::ET_SQUARE, 0, 0u, true) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testOptimizeOrientation(500u, testDuration, Geometry::Estimator::ET_SQUARE, 0, 0u, true) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testOptimizeOrientation(10u, testDuration, Geometry::Estimator::ET_TUKEY, 0, 1u, true) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testOptimizeOrientation(20u, testDuration, Geometry::Estimator::ET_TUKEY, 0, 3u, true) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testOptimizeOrientation(50u, testDuration, Geometry::Estimator::ET_TUKEY, 0, 15u, true) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testOptimizeOrientation(500u, testDuration, Geometry::Estimator::ET_TUKEY, 0, 100u, true) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testOptimizeOrientation(10u, testDuration, Geometry::Estimator::ET_SQUARE, 0, 1u, false) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testOptimizeOrientation(20u, testDuration, Geometry::Estimator::ET_SQUARE, 0, 3u, false) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testOptimizeOrientation(50u, testDuration, Geometry::Estimator::ET_SQUARE, 0, 15u, false) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testOptimizeOrientation(500u, testDuration, Geometry::Estimator::ET_SQUARE, 0, 100u, false) && allSucceeded;

	return allSucceeded;
}

bool TestNonLinearOptimizationOrientation::testOptimizeOrientation(const unsigned int correspondences, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation, const unsigned int outliers, const bool roughOrientation)
{
	ocean_assert(testDuration > 0);
	ocean_assert(outliers <= correspondences);

	Log::info() << "Testing " << correspondences << " points with " << String::toAString(standardDeviation, 1u) << " px noise and " << outliers << " outliers and using estimator " << Geometry::Estimator::translateEstimatorType(type) << ":";

	Scalar averageInitialError = 0;
	Scalar averageOptimizedError = 0;

	Scalars medianInitialErrors;
	Scalars medianOptimizedErrors;

	unsigned long long iterations = 0ull;

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	const PinholeCamera patternCamera(1280, 720, Numeric::deg2rad(60));
	RandomGenerator randomGenerator;

	const Timestamp now(true);
	do
	{
		// create a distorted camera
		const PinholeCamera pinholeCamera(Utilities::distortedCamera(patternCamera, true, false, false));// iterations % 3ull == 1ull || iterations % 3ull == 2ull, iterations % 3ull == 2ull));

		const Quaternion orientation(Random::quaternion());

		const HomogenousMatrix4 pose(orientation);
		const HomogenousMatrix4 poseIF(PinholeCamera::standard2InvertedFlipped(pose));

		Geometry::ImagePoints imagePoints;
		Geometry::ImagePoints perfectImagePoints;
		Geometry::ObjectPoints objectPoints;

		for (unsigned int n = 0u; n < correspondences; ++n)
		{
			Vector2 imagePoint(Random::scalar(40, Scalar(pinholeCamera.width() - 41)), Random::scalar(40, Scalar(pinholeCamera.height() - 41)));

			const Line3 ray(pinholeCamera.ray(imagePoint, pose));
			const Vector3 objectPoint(ray.point(Random::scalar(Scalar(0.9), Scalar(1.1))));

			imagePoint = pinholeCamera.projectToImageIF<true>(poseIF, objectPoint, pinholeCamera.hasDistortionParameters());

			Vector2 imagePointNoise(0, 0);
			if (standardDeviation > 0)
				imagePointNoise = Vector2(Random::gaussianNoise(standardDeviation), Random::gaussianNoise(standardDeviation));

			perfectImagePoints.push_back(imagePoint);
			imagePoints.push_back(imagePoint + imagePointNoise);
			objectPoints.push_back(objectPoint);
		}

		const IndexSet32 outlierSet(Utilities::randomIndices(correspondences - 1, outliers));
		for (IndexSet32::const_iterator i = outlierSet.begin(); i != outlierSet.end(); ++i)
		{
			const Vector2 outlierNoise(Random::gaussianNoise(100), Random::gaussianNoise(100));
			imagePoints[*i] += outlierNoise;
		}

		performance.start();

		SquareMatrix3 startOrientation(false);

		if (roughOrientation)
		{
			const Euler faultyEuler(Random::euler(Numeric::deg2rad(20)));
			startOrientation = SquareMatrix3(orientation * Quaternion(faultyEuler));

			SquareMatrix3 optimizedOrientation;
			Scalar initialError, finalError;
			if (Geometry::NonLinearOptimizationOrientation::optimizeOrientation(pinholeCamera, startOrientation, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), pinholeCamera.hasDistortionParameters(), optimizedOrientation, 20u, type, Scalar(0.001), Scalar(5), &initialError, &finalError))
			{
				performance.stop();

				averageInitialError += initialError;
				averageOptimizedError += finalError;

				medianInitialErrors.push_back(initialError);
				medianOptimizedErrors.push_back(finalError);
			}
			else
				performance.skip();
		}
		else
		{
			Indices32 usedIndices;
			if (Geometry::RANSAC::orientation(pinholeCamera, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), randomGenerator, pinholeCamera.hasDistortionParameters(), startOrientation, 3u, 50u, Scalar(5 * 5), nullptr, &usedIndices))
			{
				SquareMatrix3 optimizedOrientation;
				Scalar initialError, finalError;
				if (Geometry::NonLinearOptimizationOrientation::optimizeOrientation(pinholeCamera, startOrientation, ConstArraySubsetAccessor<Vector3, unsigned int>(objectPoints, usedIndices), ConstArraySubsetAccessor<Vector2, unsigned int>(imagePoints, usedIndices), pinholeCamera.hasDistortionParameters(), optimizedOrientation, 20u, type, Scalar(0.001), Scalar(5), &initialError, &finalError))
				{
					performance.stop();

					averageInitialError += initialError;
					averageOptimizedError += finalError;

					medianInitialErrors.push_back(initialError);
					medianOptimizedErrors.push_back(finalError);
				}
				else
					performance.skip();
			}
		}

		++iterations;
	}
	while (now + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	averageInitialError /= Scalar(iterations);
	averageOptimizedError /= Scalar(iterations);

	Log::info() << "Average error: " << String::toAString(averageInitialError, 1u) << " -> " << String::toAString(averageOptimizedError, 1u);
	Log::info() << "Median error: " << String::toAString(Median::constMedian(medianInitialErrors.data(), medianInitialErrors.size()), 1u) << " -> " << String::toAString(Median::constMedian(medianOptimizedErrors.data(), medianOptimizedErrors.size()), 1u);
	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 1u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 1u) << "ms, average: " << String::toAString(performance.averageMseconds(), 1u) << "ms, first: " << String::toAString(performance.firstMseconds(), 1u) << "ms";

	return true;
}

}

}

}
