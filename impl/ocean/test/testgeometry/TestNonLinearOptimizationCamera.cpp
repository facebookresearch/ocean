/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestNonLinearOptimizationCamera.h"
#include "ocean/test/testgeometry/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Subset.h"
#include "ocean/base/Timestamp.h"

#include "ocean/geometry/Error.h"
#include "ocean/geometry/NonLinearOptimizationCamera.h"
#include "ocean/geometry/Utilities.h"

#include "ocean/math/Euler.h"
#include "ocean/math/Line3.h"
#include "ocean/math/Plane3.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

bool TestNonLinearOptimizationCamera::test(const double testDuration, Worker* /*worker*/)
{
	Log::info() << "---   Camera non linear optimization test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testNonLinearOptimizationCamera(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNonLinearOptimizationCameraPoses(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Camera non linear optimization test succeeded.";
	}
	else
	{
		Log::info() << "Camera non linear optimization test FAILED!";
	}

	return allSucceeded;
}

bool TestNonLinearOptimizationCamera::testNonLinearOptimizationCamera(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Camera profile optimization test:";
	Log::info() << " ";

	bool result = true;

	for (const unsigned int outliersPercent : {0u, 10u})
	{
		if (outliersPercent != 0u)
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

			Log::info() << "Samples with Gaussian noise " << String::toAString(noise, 1u) << "px and " << outliersPercent << "% outliers";
			Log::info() << " ";

			for (const unsigned int numberCorrespondences : {50u, 500u, 5000u})
			{
				if (numberCorrespondences != 50u)
				{
					Log::info() << " ";
				}

				Log::info() << "With " << numberCorrespondences << " correspondences";

				for (Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
				{
					Log::info() << "... and " << Geometry::Estimator::translateEstimatorType(estimatorType) << ":";

					result = testNonLinearOptimizationCamera(numberCorrespondences, testDuration, estimatorType, noise, numberCorrespondences * outliersPercent / 100u) && result;
				}
			}
		}
	}

	return result;
}

bool TestNonLinearOptimizationCamera::testNonLinearOptimizationCameraPoses(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Camera profile and camera poses optimization test:";
	Log::info() << " ";

	bool result = true;

	constexpr unsigned int poses = 10u;

	for (const Scalar noise : {Scalar(0), Scalar(1)})
	{
		if (noise != Scalar(0))
		{
			Log::info() << " ";
			Log::info() << " ";
		}

		Log::info() << "Samples with Gaussian noise " << String::toAString(noise, 1u) << "px:";
		Log::info() << " ";

		for (const unsigned int numberObjectPoints : {50u, 200u})
		{
			if (numberObjectPoints != 50u)
			{
				Log::info() << " ";
			}

			Log::info() << "With " << poses << " poses and " << numberObjectPoints << " object points";

			for (Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
			{
				Log::info() << "... and " << Geometry::Estimator::translateEstimatorType(estimatorType) << ":";

				result = testNonLinearOptimizationCameraPoses(poses, numberObjectPoints, testDuration, estimatorType, noise) && result;
			}
		}
	}

	return result;
}

bool TestNonLinearOptimizationCamera::testNonLinearOptimizationCamera(const unsigned int correspondences, const double testDuration, const Geometry::Estimator::EstimatorType estimatorType, const Scalar standardDeviation, const unsigned int numberOutliers)
{
	ocean_assert(correspondences >= 3u && testDuration > 0);

	constexpr unsigned int width = 640u;
	constexpr unsigned int height = 480u;

	uint64_t iterations = 0ull;
	uint64_t succeeded = 0ull;

	HighPerformanceStatistic performance;

	const Box3 objectPointBoundingBox(Vector3(-1, -1, Scalar(-0.1)), Vector3(1, 1, Scalar(0.1)));

	Scalar averagePixelError = 0;
	Scalar averageOptimizedPixelError = 0;

	Scalars medianPixelErrors;
	Scalars medianOptimizedPixelErrors;

	constexpr unsigned int setSize = 50u;

	const Timestamp startTimestamp(true);

	do
	{
		const Scalar focalX = Random::scalar(500, 600);
		const Scalar focalY = Random::scalar(500, 600);

		const Scalar principalPointX = Random::scalar(Scalar(width) * Scalar(0.5) - 50, Scalar(width) * Scalar(0.5) + 50);
		const Scalar principalPointY = Random::scalar(Scalar(height) * Scalar(0.5) - 50, Scalar(height) * Scalar(0.5) + 50);

		const PinholeCamera::DistortionPair radialDistortionPair = PinholeCamera::DistortionPair(Random::scalar(Scalar(-0.1), Scalar(0.1)), Random::scalar(Scalar(-0.1), Scalar(0.1)));
		const PinholeCamera::DistortionPair tangentialDistortionPair = PinholeCamera::DistortionPair(Random::scalar(Scalar(-0.1), Scalar(0.1)), Random::scalar(Scalar(-0.1), Scalar(0.1)));

		const PinholeCamera pinholeCamera(width, height, focalX, focalY, principalPointX, principalPointY, radialDistortionPair, tangentialDistortionPair);

		const Vectors3 objectPoints(Utilities::objectPoints(objectPointBoundingBox, setSize));

		Vectors2 imagePoints;
		Vectors2 normalizedObjectPoints;
		Vectors2 perfectImagePoints;

		for (unsigned int i = 0u; i < max(1u, correspondences / setSize); ++i)
		{
			const Euler cameraRotation(Random::euler(Numeric::deg2rad(40)));
			const Vector3 viewingDirection(Quaternion(cameraRotation) * Vector3(0, 0, -1));

			const HomogenousMatrix4 extrinsic(Utilities::viewPosition(pinholeCamera, objectPoints, viewingDirection));

			Vectors2 localImagePoints(setSize);
			pinholeCamera.projectToImage<true>(extrinsic, objectPoints.data(), objectPoints.size(), pinholeCamera.hasDistortionParameters(), localImagePoints.data());

			perfectImagePoints.insert(perfectImagePoints.end(), localImagePoints.begin(), localImagePoints.end());

			if (standardDeviation > Scalar(0))
			{
				for (unsigned int n = 0; n < setSize; ++n)
				{
					const Vector2 noise(Random::gaussianNoise(standardDeviation), Random::gaussianNoise(standardDeviation));

					localImagePoints[n] += noise;
				}
			}

			Vectors2 localNormalizedObjectPoints(setSize);
			PinholeCamera::objectPoints2normalizedImagePoints(extrinsic, objectPoints.data(), objectPoints.size(), localNormalizedObjectPoints.data());

			imagePoints.insert(imagePoints.end(), localImagePoints.begin(), localImagePoints.end());
			normalizedObjectPoints.insert(normalizedObjectPoints.end(), localNormalizedObjectPoints.begin(), localNormalizedObjectPoints.end());
		}

		const IndexSet32 outlierSet(Utilities::randomIndices(correspondences - 1u, numberOutliers));
		for (IndexSet32::const_iterator i = outlierSet.begin(); i != outlierSet.end(); ++i)
		{
			const Vector2 outlierNoise(Random::gaussianNoise(100), Random::gaussianNoise(100));
			imagePoints[*i] += outlierNoise;
		}

		PinholeCamera initialCamera(pinholeCamera.width(), pinholeCamera.height(), Numeric::deg2rad(60));
		PinholeCamera optimizedCamera;

		performance.start();
			const bool result = Geometry::NonLinearOptimizationCamera::optimizeCamera(initialCamera, ConstArrayAccessor<Vector2>(normalizedObjectPoints), ConstArrayAccessor<Vector2>(imagePoints), PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS, optimizedCamera, 50u, estimatorType);
		performance.stop();
		ocean_assert(result);

		if (result)
		{
			Scalar initialAverage = 0, initialMinimal = 0, initialMaximal = 0;
			Geometry::Error::determineCameraError(initialCamera, normalizedObjectPoints.data(), perfectImagePoints.data(), normalizedObjectPoints.size(), true, initialAverage, initialMinimal, initialMaximal);

			Scalar optimizedAverage = 0, optimizedMinimal = 0, optimizedMaximal = 0;
			Geometry::Error::determineCameraError(optimizedCamera, normalizedObjectPoints.data(), perfectImagePoints.data(), normalizedObjectPoints.size(), true, optimizedAverage, optimizedMinimal, optimizedMaximal);

			averagePixelError += initialAverage;
			medianPixelErrors.push_back(initialAverage);

			averageOptimizedPixelError += optimizedAverage;
			medianOptimizedPixelErrors.push_back(optimizedAverage);

			// we do not investigate the actual difference between our ideal and the determine camera profile, as we are mainly interested that the error for all point correspondences is small
			// i.e., there may be different camera profiles (especially the distortion parameters) which provide the same result
			/*const Scalar fxError = Numeric::abs(pinholeCamera.focalLengthX() - optimizedCamera.focalLengthX());
			const Scalar fyError = Numeric::abs(pinholeCamera.focalLengthY() - optimizedCamera.focalLengthY());
			const Scalar mxError = Numeric::abs(pinholeCamera.principalPointX() - optimizedCamera.principalPointX());
			const Scalar myError = Numeric::abs(pinholeCamera.principalPointY() - optimizedCamera.principalPointY());
			const Scalar k1Error = Numeric::abs(pinholeCamera.radialDistortion().first - optimizedCamera.radialDistortion().first);
			const Scalar k2Error = Numeric::abs(pinholeCamera.radialDistortion().second - optimizedCamera.radialDistortion().second);

			const Scalar p1Error = Numeric::abs(pinholeCamera.tangentialDistortion().first - optimizedCamera.tangentialDistortion().first);
			const Scalar p2Error = Numeric::abs(pinholeCamera.tangentialDistortion().second - optimizedCamera.tangentialDistortion().second);*/

			succeeded++;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	averagePixelError /= Scalar(iterations);
	averageOptimizedPixelError /= Scalar(iterations);

	Log::info() << "Average sqr pixel error: " << String::toAString(averagePixelError, 1u) << "px -> " << String::toAString(averageOptimizedPixelError, 1u) << "px";
	Log::info() << "Median sqr pixel error: " << String::toAString(Median::constMedian(medianPixelErrors.data(), medianPixelErrors.size()), 1u) << "px -> " << String::toAString(Median::constMedian(medianOptimizedPixelErrors.data(), medianOptimizedPixelErrors.size()), 1u) << "px";
	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 1u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 1u) << "ms, average: " << String::toAString(performance.averageMseconds(), 1u) << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return true;
}

bool TestNonLinearOptimizationCamera::testNonLinearOptimizationCameraPoses(const unsigned int numberPoses, const unsigned int correspondences, const double testDuration, const Geometry::Estimator::EstimatorType estimatorType, const Scalar standardDeviation)
{
	ocean_assert(correspondences >= 3 && numberPoses >= 1 && testDuration > 0);

	const Plane3 yPlane(Vector3(0, 0, 0), Vector3(0, 1, 0));

	uint64_t succeeded = 0ull;
	uint64_t iterations = 0ull;

	Scalar averageInitialSqrError = 0;
	Scalar averageOptimizedSqrError = 0;

	Scalars medianPixelErrors;
	Scalars medianOptimizedPixelErrors;

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		constexpr unsigned int width = 640;
		constexpr unsigned int height = 480;

		const Scalar Fx = Random::scalar(500, 600);
		const Scalar Fy = Random::scalar(500, 600);
		const Scalar mx = Random::scalar(Scalar(width) * Scalar(0.5) - 50, Scalar(width) * Scalar(0.5) + 50);
		const Scalar my = Random::scalar(Scalar(height) * Scalar(0.5) - 50, Scalar(height) * Scalar(0.5) + 50);

		const Scalar k1 = Random::scalar(Scalar(-0.1), Scalar(0.1));
		const Scalar k2 = Random::scalar(Scalar(-0.1), Scalar(0.1));

		const Scalar p1 = Random::scalar(Scalar(-0.01), Scalar(0.01));
		const Scalar p2 = Random::scalar(Scalar(-0.01), Scalar(0.01));

		const SquareMatrix3 intrinsic(Fx, 0, 0, 0, Fy, 0, mx, my, 1);
		const PinholeCamera pinholeCamera(intrinsic, width, height, PinholeCamera::DistortionPair(k1, k2), PinholeCamera::DistortionPair(p1, p2));

		const Vectors3 objectPoints(Utilities::objectPoints(Box3(Vector3(-1, -1, -1), Vector3(1, 1, 1)), correspondences));

		std::vector<Vectors2> perfectImagePointGroups;
		std::vector<Vectors2> imagePointGroups;

		HomogenousMatrices4 poses;
		HomogenousMatrices4 inaccuratePoses;

		for (unsigned int i = 0u; i < numberPoses; ++i)
		{
			const Vector3 viewingDirection(Random::vector3());
			const HomogenousMatrix4 pose(Utilities::viewPosition(pinholeCamera, objectPoints, viewingDirection));

			Vectors2 imagePoints(objectPoints.size());
			pinholeCamera.projectToImage<true>(pose, objectPoints.data(), objectPoints.size(), true, imagePoints.data());

			perfectImagePointGroups.push_back(imagePoints);

			if (standardDeviation > 0)
			{
				for (Vector2& imagePoint : imagePoints)
				{
					imagePoint += Vector2(Random::gaussianNoise(standardDeviation), Random::gaussianNoise(standardDeviation));
				}
			}

			imagePointGroups.push_back(imagePoints);
			poses.push_back(pose);

			const Euler inaccurateViewingOffset(Random::euler(Numeric::deg2rad(2.5)));
			const Vector3 inaccurateViewingDirection(SquareMatrix3(inaccurateViewingOffset) * viewingDirection);

			const HomogenousMatrix4 inaccuratePose(Utilities::viewPosition(pinholeCamera, objectPoints, inaccurateViewingDirection));
			inaccuratePoses.push_back(inaccuratePose);
		}

		const SquareMatrix3 inaccurateIntrinsic(Fx + Random::scalar(-100, 100), 0, 0, 0, Fy + Random::scalar(-100, 100), 0, mx + Random::scalar(-50, 50), my + Random::scalar(-50, 50), 1);
		const PinholeCamera inaccurateCamera(inaccurateIntrinsic, width, height, PinholeCamera::DistortionPair(Scalar(0), Scalar(0)), PinholeCamera::DistortionPair(Scalar(0), Scalar(0)));

		HomogenousMatrices4 optimizedPoses(inaccuratePoses.size());
		NonconstArrayAccessor<HomogenousMatrix4> optimizedPosesAccessor(optimizedPoses);

		PinholeCamera optimizedCamera;

		performance.start();
		if (Geometry::NonLinearOptimizationCamera::optimizeCameraPoses(inaccurateCamera, ConstArrayAccessor<HomogenousMatrix4>(inaccuratePoses), ConstElementAccessor<Vectors3>(inaccuratePoses.size(), objectPoints), ConstArrayAccessor<Vectors2>(imagePointGroups), optimizedCamera, &optimizedPosesAccessor, 50u, estimatorType, Scalar(0.001), Scalar(5)))
		{
			performance.stop();

			Scalar sqrAveragePixelErrorInitial = 0;
			Scalar sqrMinimalPixelErrorInitial = 0;
			Scalar sqrMaximalPixelErrorInitial = 0;

			OCEAN_SUPPRESS_UNUSED_WARNING(sqrMinimalPixelErrorInitial);
			OCEAN_SUPPRESS_UNUSED_WARNING(sqrMaximalPixelErrorInitial);

			for (size_t p = 0; p < inaccuratePoses.size(); ++p)
			{
				Scalar sqrAveragePixelError = 0, sqrMinimalPixelError = 0, sqrMaximalPixelError = 0;
				Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true>(inaccuratePoses[p], inaccurateCamera, ConstTemplateArrayAccessor<Vector3>(objectPoints), ConstTemplateArrayAccessor<Vector2>(imagePointGroups[p]), inaccurateCamera.hasDistortionParameters(), sqrAveragePixelError, sqrMinimalPixelError, sqrMaximalPixelError);

				sqrAveragePixelErrorInitial += sqrAveragePixelError;
				sqrMinimalPixelErrorInitial += sqrMinimalPixelError;
				sqrMaximalPixelErrorInitial += sqrMaximalPixelError;
			}

			Scalar sqrAveragePixelErrorOptimized = 0;
			Scalar sqrMinimalPixelErrorOptimized = 0;
			Scalar sqrMaximalPixelErrorOptimized = 0;

			OCEAN_SUPPRESS_UNUSED_WARNING(sqrMinimalPixelErrorOptimized);
			OCEAN_SUPPRESS_UNUSED_WARNING(sqrMaximalPixelErrorOptimized);

			for (size_t p = 0; p < optimizedPoses.size(); ++p)
			{
				Scalar sqrAveragePixelError = 0, sqrMinimalPixelError = 0, sqrMaximalPixelError = 0;
				Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true>(optimizedPoses[p], optimizedCamera, ConstTemplateArrayAccessor<Vector3>(objectPoints), ConstTemplateArrayAccessor<Vector2>(perfectImagePointGroups[p]), optimizedCamera.hasDistortionParameters(), sqrAveragePixelError, sqrMinimalPixelError, sqrMaximalPixelError);

				sqrAveragePixelErrorOptimized += sqrAveragePixelError;
				sqrMinimalPixelErrorOptimized += sqrMinimalPixelError;
				sqrMaximalPixelErrorOptimized += sqrMaximalPixelError;
			}

			averageInitialSqrError += sqrAveragePixelErrorInitial;
			averageOptimizedSqrError += sqrAveragePixelErrorOptimized;

			medianPixelErrors.push_back(sqrAveragePixelErrorInitial);
			medianOptimizedPixelErrors.push_back(sqrAveragePixelErrorOptimized);

			const Scalar fxError = Numeric::abs(pinholeCamera.focalLengthX() - optimizedCamera.focalLengthX());
			const Scalar fyError = Numeric::abs(pinholeCamera.focalLengthY() - optimizedCamera.focalLengthY());
			const Scalar mxError = Numeric::abs(pinholeCamera.principalPointX() - optimizedCamera.principalPointX());
			const Scalar myError = Numeric::abs(pinholeCamera.principalPointY() - optimizedCamera.principalPointY());
			const Scalar k1Error = Numeric::abs(pinholeCamera.radialDistortion().first - optimizedCamera.radialDistortion().first);
			const Scalar k2Error = Numeric::abs(pinholeCamera.radialDistortion().second - optimizedCamera.radialDistortion().second);

			OCEAN_SUPPRESS_UNUSED_WARNING(fxError);
			OCEAN_SUPPRESS_UNUSED_WARNING(fyError);
			OCEAN_SUPPRESS_UNUSED_WARNING(mxError);
			OCEAN_SUPPRESS_UNUSED_WARNING(myError);
			OCEAN_SUPPRESS_UNUSED_WARNING(k1Error);
			OCEAN_SUPPRESS_UNUSED_WARNING(k2Error);

			++succeeded;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);

	Log::info() << "Average pixel error: " << String::toAString(averageInitialSqrError, 1u) << "px -> " << String::toAString(averageOptimizedSqrError, 1u) << "px";
	Log::info() << "Median sqr pixel error: " << String::toAString(Median::constMedian(medianPixelErrors.data(), medianPixelErrors.size()), 1u) << "px -> " << String::toAString(Median::constMedian(medianOptimizedPixelErrors.data(), medianOptimizedPixelErrors.size()), 1u) << "px";
	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 1u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 1u) << "ms, average: " << String::toAString(performance.averageMseconds(), 1u) << "ms";
	Log::info() << "Validation: " << String::toAString(double(succeeded) * 100.0 / double(iterations), 1u) << "% succeeded.";

	return true;
}

}

}

}
