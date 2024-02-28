// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testgeometry/TestNonLinearOptimizationHomography.h"
#include "ocean/test/testgeometry/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/geometry/Homography.h"
#include "ocean/geometry/NonLinearOptimizationHomography.h"
#include "ocean/geometry/Utilities.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

const Scalar TestNonLinearOptimizationHomography::noises[] =
{
	0,
	1
};

const unsigned int TestNonLinearOptimizationHomography::outliers[] =
{
	0u,
	10u
};

bool TestNonLinearOptimizationHomography::test(const double testDuration, Worker* /*worker*/)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Non linear homography optimization test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testNonLinearOptimizationHomography(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNonLinearOptimizationSimilarity(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Non linear homography optimization test succeeded.";
	}
	else
	{
		Log::info() << "Non linear homography optimization test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestNonLinearOptimizationHomography, NonLinearOptimizationHomography_100Points_8Parameters_NoNoise)
{
	for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
	{
		EXPECT_TRUE(TestNonLinearOptimizationHomography::testNonLinearOptimizationHomography(100u, 8u, GTEST_TEST_DURATION, estimatorType, Scalar(0), 0u, false));
	}
}

TEST(TestNonLinearOptimizationHomography, NonLinearOptimizationHomography_100Points_9Parameters_NoNoise)
{
	for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
	{
		EXPECT_TRUE(TestNonLinearOptimizationHomography::testNonLinearOptimizationHomography(100u, 9u, GTEST_TEST_DURATION, estimatorType, Scalar(0), 0u, false));
	}
}

TEST(TestNonLinearOptimizationHomography, NonLinearOptimizationHomography_100Points_8Parameters_Noise)
{
	for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
	{
		for (unsigned int covarianceIteration = 0u; covarianceIteration < 2u; ++covarianceIteration)
		{
			const bool useCovariance = covarianceIteration == 1u;

			EXPECT_TRUE(TestNonLinearOptimizationHomography::testNonLinearOptimizationHomography(100u, 8u, GTEST_TEST_DURATION, estimatorType, Scalar(1), 10u, useCovariance));
		}
	}
}

TEST(TestNonLinearOptimizationHomography, NonLinearOptimizationHomography_100Points_9Parameters_Noise)
{
	for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
	{
		for (unsigned int covarianceIteration = 0u; covarianceIteration < 2u; ++covarianceIteration)
		{
			const bool useCovariance = covarianceIteration == 1u;

			EXPECT_TRUE(TestNonLinearOptimizationHomography::testNonLinearOptimizationHomography(100u, 9u, GTEST_TEST_DURATION, estimatorType, Scalar(1), 10u, useCovariance));
		}
	}
}

TEST(TestNonLinearOptimizationHomography, NonLinearOptimizationSimilarity_100Points_NoNoise)
{
	for (Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
	{
		EXPECT_TRUE(TestNonLinearOptimizationHomography::testNonLinearOptimizationSimilarity(100u, GTEST_TEST_DURATION, estimatorType, Scalar(0), 0u, false));
	}
}

TEST(TestNonLinearOptimizationHomography, NonLinearOptimizationSimilarity_100Points_Noise)
{
	for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
	{
		for (unsigned int covarianceIteration = 0u; covarianceIteration < 2u; ++covarianceIteration)
		{
			const bool useCovariance = covarianceIteration == 1u;

			EXPECT_TRUE(TestNonLinearOptimizationHomography::testNonLinearOptimizationSimilarity(100u, GTEST_TEST_DURATION, estimatorType, Scalar(1), 10u, useCovariance));
		}
	}
}

#endif // OCEAN_USE_GTEST

bool TestNonLinearOptimizationHomography::testNonLinearOptimizationHomography(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing optimization homography:";
	Log::info() << " ";

	bool allSucceeded = true;

	const unsigned int correspondenceNumbers[] = {4u, 20u, 50u, 100u, 1000u};

	const unsigned int modelParameters[] = {8u, 9u};

	for (unsigned int parameterIndex = 0u; parameterIndex < sizeof(modelParameters) / sizeof(modelParameters[0]); ++parameterIndex)
	{
		for (unsigned int o = 0u; o < sizeof(outliers) / sizeof(outliers[0]); ++o)
		{
			if (o != 0u)
			{
				Log::info() << " ";
				Log::info() << " ";
			}

			const unsigned int outlier = outliers[o];

			for (unsigned int n = 0u; n < sizeof(noises) / sizeof(noises[0]); ++n)
			{
				if (n != 0u)
				{
					Log::info() << " ";
					Log::info() << " ";
				}

				const Scalar noise = noises[n];

				Log::info() << "Samples with gaussian noise " << String::toAString(noise, 1u) << "px, " << outlier << "% outliers and " << modelParameters[parameterIndex] << " parameter for the model";
				Log::info() << " ";

				for (unsigned int c = 0u; c < sizeof(correspondenceNumbers) / sizeof(correspondenceNumbers[0]); ++c)
				{
					if (c != 0u)
					{
						Log::info() << " ";
					}

					const unsigned int correspondences = correspondenceNumbers[c];

					Log::info() << "... with " << correspondences << " correspondences";

					for (unsigned int v = 0u; v < 2u; ++v)
					{
						if (noise == 0u && v != 0u)
							continue;

						if (v != 0u)
						{
							Log::info() << " ";
						}

						const bool useCovariances = v == 1u;

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
							Log::info() << "... and " << Geometry::Estimator::translateEstimatorType(estimatorType);

							if (!testNonLinearOptimizationHomography(correspondences, modelParameters[parameterIndex], testDuration, estimatorType, noise, correspondences * outlier / 100u, useCovariances))
							{
								allSucceeded = false;
							}
						}
					}
				}
			}
		}
	}

	return allSucceeded;
}

bool TestNonLinearOptimizationHomography::testNonLinearOptimizationSimilarity(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing optimization similarity:";
	Log::info() << " ";

	bool allSucceeded = true;

	const unsigned int correspondenceNumbers[] = {4u, 20u, 50u, 100u, 1000u};

	for (unsigned int o = 0u; o < sizeof(outliers) / sizeof(outliers[0]); ++o)
	{
		if (o != 0u)
		{
			Log::info() << " ";
			Log::info() << " ";
		}

		const unsigned int outlier = outliers[o];

		for (unsigned int n = 0u; n < sizeof(noises) / sizeof(noises[0]); ++n)
		{
			if (n != 0u)
			{
				Log::info() << " ";
				Log::info() << " ";
			}

			const Scalar noise = noises[n];

			Log::info() << "Samples with gaussian noise " << String::toAString(noise, 1u) << "px, " << outlier << "% outliers";
			Log::info() << " ";

			for (unsigned int c = 0u; c < sizeof(correspondenceNumbers) / sizeof(correspondenceNumbers[0]); ++c)
			{
				if (c != 0u)
				{
					Log::info() << " ";
				}

				const unsigned int correspondences = correspondenceNumbers[c];

				Log::info() << "... with " << correspondences << " correspondences";

				for (unsigned int v = 0u; v < 2u; ++v)
				{
					if (noise == 0u && v != 0u)
						continue;

					if (v != 0u)
					{
						Log::info() << " ";
					}

					const bool useCovariances = v == 1u;

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
						Log::info() << "... and " << Geometry::Estimator::translateEstimatorType(estimatorType);

						if (!testNonLinearOptimizationSimilarity(correspondences, testDuration, estimatorType, noise, correspondences * outlier / 100u, useCovariances))
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

bool TestNonLinearOptimizationHomography::testNonLinearOptimizationHomography(const unsigned int correspondences, const unsigned int modelParameter, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation, const unsigned int numberOutliers, const bool useCovariances)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(numberOutliers <= correspondences);
	ocean_assert(modelParameter >= 8u && modelParameter <= 9u);

	unsigned long long validIterations = 0ull;
	unsigned long long iterations = 0ull;

	bool explicitError = false;

	HighPerformanceStatistic performance;

	// camera profile
	const PinholeCamera pinholeCamera(1280u, 720u, Numeric::deg2rad(45));

	RandomGenerator randomGenerator;

	Scalar averageSqrPixelErrorHomography = 0;
	Scalar averageSqrPixelErrorOpimization = 0;

	Scalars medianSqrPixelErrors;
	Scalars medianOptimizedSqrPixelErrors;

	const Timestamp startTimestamp(true);

	const Plane3 plane(Vector3(-10, -10, 0), Vector3(10, 10, 0), Vector3(-10, 10, 0));
	ocean_assert(plane.isValid());

	do
	{
		// creating the object point into the plane
		Vectors3 objectPoints;
		for (size_t i = 0; i < correspondences; i++)
		{
			objectPoints.push_back(Vector3(Random::scalar(randomGenerator, -10, 10), Random::scalar(randomGenerator, -10, 10), 0));
		}

		// viewing direction onto plane
		const Vector3 viewdirectionLeft(Quaternion(Random::euler(Numeric::deg2rad(30))) * Vector3(0, 0, -1));
		const Vector3 viewdirectionRight(Quaternion(Random::euler(Numeric::deg2rad(30))) * Vector3(0, 0, -1));

		// determine camera pose ensuring that all object points are visible
		const HomogenousMatrix4 poseLeft(Utilities::viewPosition(pinholeCamera, objectPoints, viewdirectionLeft, true));
		const HomogenousMatrix4 poseRight(Utilities::viewPosition(pinholeCamera, objectPoints, viewdirectionRight, true));

		Geometry::ImagePoints pointsLeft, pointsRightNoised;
		Geometry::ImagePoints perfectImagePointsRight;

		Matrix invertedCovariances(correspondences * 2u, 2u);

		for (unsigned int n = 0u; n < correspondences; ++n)
		{
			// projection of the object points into image
			const Vector2 imagePointRight = pinholeCamera.projectToImage<true>(poseRight, objectPoints[n], pinholeCamera.hasDistortionParameters());
			const Vector2 imagePointLeft = pinholeCamera.projectToImage<true>(poseLeft, objectPoints[n], pinholeCamera.hasDistortionParameters());

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

			perfectImagePointsRight.push_back(imagePointRight);
			pointsLeft.push_back(imagePointLeft);
			pointsRightNoised.push_back(imagePointRight + imagePointNoise);
		}

		const SquareMatrix3 homography(Geometry::Homography::normalizedHomography(Geometry::Homography::homographyMatrix(poseLeft, poseRight, pinholeCamera, pinholeCamera, plane)));
		ocean_assert_and_suppress_unused(!homography.isSingular(), homography);
		ocean_assert(Numeric::isWeakEqualEps(determineHomographyError(homography, pointsLeft, perfectImagePointsRight)));

		// add outliers to the right points
		const IndexSet32 outlierSet(Utilities::randomIndices(correspondences - 1, numberOutliers));
		for (IndexSet32::const_iterator i = outlierSet.begin(); i != outlierSet.end(); ++i)
		{
			const Vector2 outlierNoise(Random::gaussianNoise(100), Random::gaussianNoise(100));
			pointsRightNoised[*i] += outlierNoise;
		}

		// creating a faulty pose of the right camera

		const Vector3 errorTranslation(Random::vector3(Scalar(-0.1), Scalar(0.1)));
		const Euler errorEuler(Random::euler(Numeric::deg2rad(10)));
		const Quaternion errorRotation(errorEuler);

		const Vector3 faultyTranslation(poseRight.translation() + errorTranslation);
		const Quaternion faultyRotation(poseRight.rotation() * errorRotation);

		const HomogenousMatrix4 faultyPose = HomogenousMatrix4(faultyTranslation, faultyRotation);

		// faulty homography matrix based on the faulty pose
		SquareMatrix3 homographyFaulty(Geometry::Homography::homographyMatrix(poseLeft, faultyPose, pinholeCamera, pinholeCamera, plane));
		const Scalar distanceSqrErrorFaulty = determineHomographyError(homographyFaulty, pointsLeft, perfectImagePointsRight);

		averageSqrPixelErrorHomography += distanceSqrErrorFaulty;
		medianSqrPixelErrors.push_back(distanceSqrErrorFaulty);

		SquareMatrix3 optimizedHomography;

		performance.start();

		Scalar initialError = Numeric::maxValue();
		Scalar finalError = Numeric::maxValue();
		Scalars intermediateErrors;

		const bool result = Geometry::NonLinearOptimizationHomography::optimizeHomography(homographyFaulty, pointsLeft.data(), pointsRightNoised.data(), correspondences, modelParameter, optimizedHomography, 20u, type, Scalar(0.001), Scalar(5.0), &initialError, &finalError, useCovariances ? &invertedCovariances : nullptr, &intermediateErrors);
		ocean_assert(result);

		performance.stop();

		if (result)
		{
			// let's ensure that the intermediate errors are decreasing

			for (size_t i = 1; i < intermediateErrors.size(); ++i)
			{
				if (intermediateErrors[i] > intermediateErrors[i -1])
				{
					explicitError = true;
				}
			}

			if (intermediateErrors.size() >= 2)
			{
				ocean_assert(intermediateErrors.front() == initialError && intermediateErrors.back() == finalError);

				const Scalar averageSqrDistance = determineHomographyError(optimizedHomography, pointsLeft, perfectImagePointsRight);

				averageSqrPixelErrorOpimization += averageSqrDistance;
				medianOptimizedSqrPixelErrors.push_back(averageSqrDistance);

				if (standardDeviation == 0 && numberOutliers == 0u)
				{
					// perfect conditions need perfect results

					if (averageSqrDistance <= Scalar(1))
					{
						++validIterations;
					}
				}
				else
				{
					// as we do not know anything about the outliers etc. we do not check the result for accuracy
					// we just ensure that we have a couple of optimization iterations

					if (intermediateErrors.size() >= 5)
					{
						++validIterations;
					}
				}
			}
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	// output errors
	ocean_assert(iterations != 0ull);
	averageSqrPixelErrorHomography /= Scalar(iterations);
	averageSqrPixelErrorOpimization /= Scalar(iterations);

	const Scalar medianPixelErrorFaulty = Median::constMedian(medianSqrPixelErrors.data(), medianSqrPixelErrors.size());
	const Scalar medianPixelErrorOpimization = Median::constMedian(medianOptimizedSqrPixelErrors.data(), medianOptimizedSqrPixelErrors.size());
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Average sqr pixel error: " << String::toAString(averageSqrPixelErrorHomography, 1u) << "px -> " << String::toAString(averageSqrPixelErrorOpimization, 1u) << "px";
	Log::info() << "Median sqr pixel error: " << String::toAString(medianPixelErrorFaulty, 1u) << "px -> " << String::toAString(medianPixelErrorOpimization, 1u) << "px";
	Log::info() << "Performance Best: " << String::toAString(performance.bestMseconds(), 4u) << "ms worst: " << String::toAString(performance.worstMseconds(), 4u) << "ms average: " << String::toAString(performance.averageMseconds(), 4u) << "ms first: " << String::toAString(performance.firstMseconds(), 4u) << "ms";

	if (explicitError)
	{
		Log::info() << "Validation: FAILED!";
		return false;
	}

	if (correspondences >= 20u && standardDeviation == 0 && numberOutliers == 0u)
	{
		// perfect conditions need perfect results

		if (percent >= 0.99)
		{
			Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";
			return true;
		}

		Log::info() << "Validation: FAILED!";
		return false;
	}

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return true;
}

bool TestNonLinearOptimizationHomography::testNonLinearOptimizationSimilarity(const unsigned int correspondences, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation, const unsigned int numberOutliers, const bool useCovariances)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(numberOutliers <= correspondences);

	unsigned long long validIterations = 0ull;
	unsigned long long iterations = 0ull;

	bool explicitError = false;

	HighPerformanceStatistic performance;

	const unsigned int width = 1280u;
	const unsigned int height = 720u;

	RandomGenerator randomGenerator;

	Scalar averageSqrPixelErrorSimilarity = 0;
	Scalar averageSqrPixelErrorOpimization = 0;

	Scalars medianSqrPixelErrors;
	Scalars medianOptimizedSqrPixelErrors;

	const Timestamp startTimestamp(true);

	do
	{
		const Vector2 translation = Random::vector2(-100, 100);
		const Scalar rotation = Random::scalar(Numeric::deg2rad(-30), Numeric::deg2rad(30));
		const Scalar scale = Random::scalar(Scalar(0.25), Scalar(1.75));

		const Vector2 xAxis(Numeric::cos(rotation) * scale, Numeric::sin(rotation) * scale);
		const Vector2 yAxis(-xAxis.y(), xAxis.x());

		const SquareMatrix3 similarity(Vector3(xAxis, 0), Vector3(yAxis, 0), Vector3(translation, 1));

		Geometry::ImagePoints pointsLeft, pointsRightNoised;
		Geometry::ImagePoints perfectImagePointsRight;

		Matrix invertedCovariances(correspondences * 2u, 2u);

		for (unsigned int n = 0u; n < correspondences; ++n)
		{
			const Vector2 imagePointLeft = Vector2(Random::scalar(0, Scalar(width)), Random::scalar(0, Scalar(height)));
			const Vector2 imagePointRight = similarity * imagePointLeft;

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

			perfectImagePointsRight.push_back(imagePointRight);
			pointsLeft.push_back(imagePointLeft);
			pointsRightNoised.push_back(imagePointRight + imagePointNoise);
		}

		// add outliers to the right points
		const IndexSet32 outlierSet(Utilities::randomIndices(correspondences - 1, numberOutliers));
		for (IndexSet32::const_iterator i = outlierSet.begin(); i != outlierSet.end(); ++i)
		{
			const Vector2 outlierNoise(Random::gaussianNoise(100), Random::gaussianNoise(100));
			pointsRightNoised[*i] += outlierNoise;
		}

		// creating a faulty pose of the right camera

		const Vector2 errorTranslation(Vector2(-50, 50));
		const Scalar errorRotation = Random::scalar(Numeric::deg2rad(-15), Numeric::deg2rad(15));
		const Scalar errorScale = Random::scalar(Scalar(0.5), Scalar(1.5));

		const Vector2 faultyTranslation(translation + errorTranslation);
		const Scalar faultyRotation(rotation + errorRotation);
		const Scalar faultyScale(scale * errorScale);

		const Vector2 faultyXAxis(Numeric::cos(faultyRotation) * faultyScale, Numeric::sin(faultyRotation) * faultyScale);
		const Vector2 faultyYAxis(-faultyXAxis.y(), faultyXAxis.x());

		const SquareMatrix3 faultySimilarity(Vector3(faultyXAxis, 0), Vector3(faultyYAxis, 0), Vector3(faultyTranslation, 1));

		const Scalar distanceSqrErrorFaulty = determineHomographyError(faultySimilarity, pointsLeft, perfectImagePointsRight);

		averageSqrPixelErrorSimilarity += distanceSqrErrorFaulty;
		medianSqrPixelErrors.push_back(distanceSqrErrorFaulty);

		SquareMatrix3 optimizedSimilarity;

		performance.start();

		Scalar initialError = Numeric::maxValue();
		Scalar finalError = Numeric::maxValue();
		Scalars intermediateErrors;

		const bool result = Geometry::NonLinearOptimizationHomography::optimizeSimilarity(faultySimilarity, pointsLeft.data(), pointsRightNoised.data(), correspondences, optimizedSimilarity, 20u, type, Scalar(0.001), Scalar(5.0), &initialError, &finalError, useCovariances ? &invertedCovariances : nullptr, &intermediateErrors);
		ocean_assert(result);

		performance.stop();

		if (result)
		{
			// a  -b  tx
			// b   a  ty
			// 0   0   1

			if (Numeric::isNotEqual(optimizedSimilarity(0, 0), optimizedSimilarity(1, 1)) || Numeric::isNotEqual(optimizedSimilarity(1, 0), -optimizedSimilarity(0, 1)))
			{
				explicitError = true;
			}

			if (Numeric::isNotEqual(optimizedSimilarity(2, 0), 0) || Numeric::isNotEqual(optimizedSimilarity(2, 1), 0) || Numeric::isNotEqual(optimizedSimilarity(2, 2), 1))
			{
				explicitError = true;
			}

			// let's ensure that the intermediate errors are decreasing

			for (size_t i = 1; i < intermediateErrors.size(); ++i)
			{
				if (intermediateErrors[i] > intermediateErrors[i -1])
				{
					explicitError = true;
				}
			}

			if (intermediateErrors.size() >= 2)
			{
				ocean_assert(intermediateErrors.front() == initialError && intermediateErrors.back() == finalError);

				const Scalar averageSqrDistance = determineHomographyError(optimizedSimilarity, pointsLeft, perfectImagePointsRight);

				averageSqrPixelErrorOpimization += averageSqrDistance;
				medianOptimizedSqrPixelErrors.push_back(averageSqrDistance);

				if (standardDeviation == 0 && numberOutliers == 0u)
				{
					// perfect conditions need perfect results

					if (averageSqrDistance <= Scalar(1))
					{
						++validIterations;
					}
				}
				else
				{
					// as we do not know anything about the outliers etc. we do not check the result for accuracy
					// we just ensure that we have a couple of optimization iterations

					if (intermediateErrors.size() >= 5)
					{
						++validIterations;
					}
				}
			}
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	// output errors
	ocean_assert(iterations != 0ull);
	averageSqrPixelErrorSimilarity /= Scalar(iterations);
	averageSqrPixelErrorOpimization /= Scalar(iterations);

	const Scalar medianPixelErrorFaulty = Median::constMedian(medianSqrPixelErrors.data(), medianSqrPixelErrors.size());
	const Scalar medianPixelErrorOpimization = Median::constMedian(medianOptimizedSqrPixelErrors.data(), medianOptimizedSqrPixelErrors.size());
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Average sqr pixel error: " << String::toAString(averageSqrPixelErrorSimilarity, 1u) << "px -> " << String::toAString(averageSqrPixelErrorOpimization, 1u) << "px";
	Log::info() << "Median sqr pixel error: " << String::toAString(medianPixelErrorFaulty, 1u) << "px -> " << String::toAString(medianPixelErrorOpimization, 1u) << "px";
	Log::info() << "Performance Best: " << String::toAString(performance.bestMseconds(), 4u) << "ms worst: " << String::toAString(performance.worstMseconds(), 4u) << "ms average: " << String::toAString(performance.averageMseconds(), 4u) << "ms first: " << String::toAString(performance.firstMseconds(), 4u) << "ms";

	if (explicitError)
	{
		Log::info() << "Validation: FAILED!";
		return false;
	}

	if (correspondences >= 20 && standardDeviation == 0 && numberOutliers == 0u)
	{
		// perfect conditions need perfect results

		if (percent >= 0.99)
		{
			Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";
			return true;
		}

		Log::info() << "Validation: FAILED!";
		return false;
	}

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return true;
}

Scalar TestNonLinearOptimizationHomography::determineHomographyError(const SquareMatrix3& homography, const Vectors2& pointsLeft, const Vectors2& pointsRight, Scalar* maximalSqrDistance)
{
	ocean_assert(!homography.isSingular());
	ocean_assert(pointsLeft.size() == pointsRight.size());

	if (maximalSqrDistance)
	{
		*maximalSqrDistance = 0;
	}

	if (pointsLeft.empty())
	{
		return Scalar(0);
	}

	Scalar averageSqrPixelErrorHomography = 0;

	for (size_t n = 0; n < pointsLeft.size(); ++n)
	{
		const Vector2 transformedPoint = homography * pointsLeft[n];
		const Scalar sqrDistance(pointsRight[n].sqrDistance(transformedPoint));

		averageSqrPixelErrorHomography += sqrDistance;

		if (maximalSqrDistance && sqrDistance > *maximalSqrDistance)
			*maximalSqrDistance = sqrDistance;
	}

	ocean_assert(!pointsLeft.empty());
	return averageSqrPixelErrorHomography / Scalar(pointsLeft.size());
}

}

}

}
