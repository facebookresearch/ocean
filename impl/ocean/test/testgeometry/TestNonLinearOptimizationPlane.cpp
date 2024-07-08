/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestNonLinearOptimizationPlane.h"
#include "ocean/test/testgeometry/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/geometry/NonLinearOptimizationPlane.h"
#include "ocean/geometry/RANSAC.h"
#include "ocean/geometry/Utilities.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

bool TestNonLinearOptimizationPlane::test(const double testDuration, Worker* /*worker*/)
{
	Log::info() << "---   Plane non linear optimization test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testOptimizePlane(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testOptimizeOnePoseOnePlane(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testOptimizePosesOnePlane(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Plane non linear optimization test succeeded.";
	}
	else
	{
		Log::info() << "Plane non linear optimization test FAILED!";
	}

	return allSucceeded;
}

bool TestNonLinearOptimizationPlane::testOptimizePlane(const double testDuration)
{
	ocean_assert(testDuration > 0);

	Log::info() << "Optimization of 3D plane:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testOptimizePlaneIdeal(5u, testDuration, Geometry::Estimator::ET_SQUARE, 0u) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testOptimizePlaneIdeal(15u, testDuration, Geometry::Estimator::ET_SQUARE, 0u) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testOptimizePlaneIdeal(50u, testDuration, Geometry::Estimator::ET_SQUARE, 0u) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testOptimizePlaneIdeal(500u, testDuration, Geometry::Estimator::ET_SQUARE, 0u) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testOptimizePlaneIdeal(5u, testDuration, Geometry::Estimator::ET_TUKEY, 1u) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testOptimizePlaneIdeal(15u, testDuration, Geometry::Estimator::ET_TUKEY, 3u) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testOptimizePlaneIdeal(50u, testDuration, Geometry::Estimator::ET_TUKEY, 15u) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testOptimizePlaneIdeal(500u, testDuration, Geometry::Estimator::ET_TUKEY, 100u) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testOptimizePlaneNoisy(10u, testDuration, Geometry::Estimator::ET_SQUARE, Scalar(0.05), 3u) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testOptimizePlaneNoisy(20u, testDuration, Geometry::Estimator::ET_SQUARE, Scalar(0.05), 6u) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testOptimizePlaneNoisy(50u, testDuration, Geometry::Estimator::ET_SQUARE, Scalar(0.05), 10u) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testOptimizePlaneNoisy(500u, testDuration, Geometry::Estimator::ET_SQUARE, Scalar(0.05), 100u) && allSucceeded;

	return allSucceeded;
}

bool TestNonLinearOptimizationPlane::testOptimizeOnePoseOnePlane(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Optimization of one 6DOF camera pose and one 3D plane:";
	Log::info() << " ";

	bool result = true;

	for (const unsigned int numberObjectPoints : {50u, 200u})
	{
		if (numberObjectPoints != 50u)
		{
			Log::info() << " ";
		}

		Log::info() << "Samples with " << numberObjectPoints << " object points";

		for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
		{
			Log::info() << "... and " << Geometry::Estimator::translateEstimatorType(estimatorType) << ":";

			result = testOptimizeOnePoseOnePlane(numberObjectPoints, testDuration, estimatorType) && result;
		}
	}

	return result;
}

bool TestNonLinearOptimizationPlane::testOptimizePosesOnePlane(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Optimization of several 6DOF camera poses and one 3D plane:";
	Log::info() << " ";

	bool result = true;

	constexpr unsigned int numberPoses = 5u;

	for (const unsigned int numberObjectPoints : {50u, 200u})
	{
		if (numberObjectPoints != 50u)
		{
			Log::info() << " ";
		}

		Log::info() << "Samples with " << numberPoses << " poses and " << numberObjectPoints << " object points";

		for (const Geometry::Estimator::EstimatorType estimatorType : Geometry::Estimator::estimatorTypes())
		{
			Log::info() << "... and " << Geometry::Estimator::translateEstimatorType(estimatorType) << ":";

			result = testOptimizePosesOnePlane(numberPoses, numberObjectPoints, testDuration, estimatorType) && result;
		}
	}

	return result;
}

bool TestNonLinearOptimizationPlane::testOptimizePlaneIdeal(const unsigned int numberPoints, const double testDuration, const Geometry::Estimator::EstimatorType type, const unsigned int outliers)
{
	ocean_assert(numberPoints >= 3u);

	Log::info() << "Testing " << numberPoints << " ideal points with " << outliers << " outliers using estimator " << Geometry::Estimator::translateEstimatorType(type) << ":";

	Scalar averageInitialError = 0;
	Scalar averageOptimizedError = 0;

	Scalars medianInitialErrors;
	Scalars medianOptimizedErrors;

	unsigned long long iterations = 0ull;

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);
	do
	{
		const Scalar environmentRadius = Random::scalar(Scalar(0.01), 10);

		const Vector3 planePosition = Random::vector3(-environmentRadius * 10, environmentRadius * 10);
		const Quaternion planeOrientation = Random::quaternion();

		const HomogenousMatrix4 planeTransformation(planePosition, planeOrientation);
		const Plane3 plane(planeTransformation);

		Vectors3 planePoints;

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			const Vector3 point = planePosition + Random::vector3(-environmentRadius, environmentRadius);
			const Vector3 projectedPoint = plane.projectOnPlane(point);
			ocean_assert(projectedPoint.distance(planePosition) < environmentRadius * 2);

			planePoints.push_back(projectedPoint);
		}

		IndexSet32 outlierIndices;
		while (outlierIndices.size() < outliers)
			outlierIndices.insert(RandomI::random(numberPoints - 1u));

		for (IndexSet32::const_iterator i = outlierIndices.begin(); i != outlierIndices.end(); ++i)
			planePoints[*i] = plane.projectOnPlane(planePoints[*i]) + plane.normal() * environmentRadius * Random::scalar(0.5, 1000) * Random::sign();

		const Quaternion planeOrientationOffset(Random::euler(Numeric::deg2rad(5), Numeric::deg2rad(20)));
		const Vector3 planePositionOffset(Random::vector3(-environmentRadius * Scalar(0.1), environmentRadius * Scalar(0.1)));

		const HomogenousMatrix4 planeTransformationOffset(planePositionOffset, planeOrientationOffset);

		const Plane3 faultyPlane(HomogenousMatrix4(planeTransformation * planeTransformationOffset));

		performance.start();

		Plane3 optimizedPlane;
		Scalar initialError, finalError;
		if (Geometry::NonLinearOptimizationPlane::optimizePlane(faultyPlane, ConstArrayAccessor<Vector3>(planePoints), optimizedPlane, 50u, type, Scalar(0.001), Scalar(5), &initialError, &finalError))
		{
			performance.stop();

			averageInitialError += initialError;
			averageOptimizedError += finalError;

			medianInitialErrors.push_back(initialError);
			medianOptimizedErrors.push_back(finalError);
		}
		else
			performance.skip();

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	averageInitialError /= Scalar(iterations);
	averageOptimizedError /= Scalar(iterations);

	Log::info() << "Average error: " << String::toAString(averageInitialError, 1u) << " -> " << String::toAString(averageOptimizedError, 1u);
	Log::info() << "Median error: " << String::toAString(Median::constMedian(medianInitialErrors.data(), medianInitialErrors.size()), 1u) << " -> " << String::toAString(Median::constMedian(medianOptimizedErrors.data(), medianOptimizedErrors.size()), 1u);
	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 1u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 1u) << "ms, average: " << String::toAString(performance.averageMseconds(), 1u) << "ms, first: " << String::toAString(performance.firstMseconds(), 1u) << "ms";

	return true;
}

bool TestNonLinearOptimizationPlane::testOptimizePlaneNoisy(const unsigned int numberPoints, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation, const unsigned int outliers)
{
	ocean_assert(numberPoints >= 3u);
	ocean_assert(standardDeviation >= 0);

	Log::info() << "Testing " << numberPoints << " noisy points with " << outliers << " outliers and " << String::toAString(standardDeviation, 1u) << " px noise using estimator " << Geometry::Estimator::translateEstimatorType(type) << " and RANSAC:";

	Scalar averageIdealNoisyError = 0;
	Scalar averageOptimizedError = 0;

	Scalars medianIdealNoisyErrors;
	Scalars medianOptimizedErrors;

	unsigned long long iterations = 0ull;

	HighPerformanceStatistic performance;
	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);
	do
	{
		const Scalar environmentRadius = Random::scalar(Scalar(0.01), 10);

		const Vector3 planePosition = Random::vector3(-environmentRadius * 10, environmentRadius * 10);
		const Quaternion planeOrientation = Random::quaternion();

		const HomogenousMatrix4 planeTransformation(planePosition, planeOrientation);
		const Plane3 plane(planeTransformation);

		Vectors3 planePoints;

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			const Vector3 point = planePosition + Random::vector3(-environmentRadius, environmentRadius);
			const Vector3 projectedPoint = plane.projectOnPlane(point);
			ocean_assert(projectedPoint.distance(planePosition) < environmentRadius * 2);

			Vector3 planePoint = projectedPoint;

			if (standardDeviation > 0)
				planePoint += plane.normal() * Random::gaussianNoise(environmentRadius * standardDeviation);

			planePoints.push_back(planePoint);
		}

		IndexSet32 outlierIndices;
		while (outlierIndices.size() < outliers)
			outlierIndices.insert(RandomI::random(numberPoints - 1u));

		for (IndexSet32::const_iterator i = outlierIndices.begin(); i != outlierIndices.end(); ++i)
			planePoints[*i] = plane.projectOnPlane(planePoints[*i]) + plane.normal() * environmentRadius * Random::scalar(0.5, 1000) * Random::sign();

		performance.start();

		Scalar finalError;
		Plane3 optimizedPlane;
		if (Geometry::RANSAC::plane(ConstArrayAccessor<Vector3>(planePoints), randomGenerator, optimizedPlane, 50u, Scalar(0.1), (unsigned int)(planePoints.size()) / 2u, type, &finalError, nullptr))
		{
			performance.stop();

			Scalar idealNoisyError = 0;
			for (unsigned int n = 0u; n < planePoints.size(); ++n)
				if (outlierIndices.find(n) == outlierIndices.end())
					idealNoisyError += Numeric::sqr(plane.signedDistance(planePoints[n]));

			averageIdealNoisyError += idealNoisyError;
			averageOptimizedError += finalError;

			medianIdealNoisyErrors.push_back(idealNoisyError);
			medianOptimizedErrors.push_back(finalError);
		}
		else
			performance.skip();

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	averageIdealNoisyError /= Scalar(iterations);
	averageOptimizedError /= Scalar(iterations);

	Log::info() << "Average error: " << String::toAString(averageIdealNoisyError, 1u) << " -> " << String::toAString(averageOptimizedError, 1u);
	Log::info() << "Median error: " << String::toAString(Median::constMedian(medianIdealNoisyErrors.data(), medianIdealNoisyErrors.size()), 1u) << " -> " << String::toAString(Median::constMedian(medianOptimizedErrors.data(), medianOptimizedErrors.size()), 1u);
	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 1u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 1u) << "ms, average: " << String::toAString(performance.averageMseconds(), 1u) << "ms, first: " << String::toAString(performance.firstMseconds(), 1u) << "ms";

	return true;
}

bool TestNonLinearOptimizationPlane::testOptimizeOnePoseOnePlane(const unsigned int correspondences, const double testDuration, const Geometry::Estimator::EstimatorType type)
{
	ocean_assert(correspondences >= 4u);

	Scalar averageInitialSqrError = 0;
	Scalar averageInitialAngleError = 0;

	Scalar averageOptimizedSqrError = 0;
	Scalar averageOptimizedAngleError = 0;

	Scalars medianInitialPixelErrors;
	Scalars medianInitialAngleErrors;

	Scalars medianOptimizedPixelErrors;
	Scalars medianOptimizedAngleErrors;

	unsigned long long iterations = 0ull;
	unsigned long long succeeded = 0ull;

	HighPerformanceStatistic performance;

	PinholeCamera pinholeCamera(640, 480, Numeric::deg2rad(60));
	const Vector3 zeroViewingDirection(0.0, 0, -1);

	const Timestamp startTimestamp(true);

	do
	{
		const Vectors3 volumeObjectPoints(Utilities::objectPoints(Box3(Vector3(-0.5, -0.5, -0.5), Vector3(0.5, 0.5, 0.5)), correspondences));

		const Vector3 viewingDirectionLeft = Quaternion(Random::euler(Numeric::deg2rad(20))) * zeroViewingDirection;
		const HomogenousMatrix4 poseLeft(Utilities::viewPosition(pinholeCamera, volumeObjectPoints, viewingDirectionLeft));

		const Plane3 plane(Quaternion(Random::euler(Numeric::deg2rad(20))) * Vector3(0, 0, 1), Random::scalar(-5, -1));

		const Vectors2 imagePointsLeft(Utilities::imagePoints(pinholeCamera, correspondences));
		const Vectors3 planeObjectPoints(Geometry::Utilities::backProjectImagePoints(pinholeCamera, poseLeft, plane, imagePointsLeft.data(), imagePointsLeft.size(), pinholeCamera.hasDistortionParameters()));

		const Vector3 viewingDirectionRight = Quaternion(Random::euler(Numeric::deg2rad(20))) * zeroViewingDirection;
		const HomogenousMatrix4 poseRight(Utilities::viewPosition(pinholeCamera, planeObjectPoints, viewingDirectionRight));

		Vectors2 imagePointsRight(planeObjectPoints.size());
		pinholeCamera.projectToImage<true>(poseRight, planeObjectPoints.data(), planeObjectPoints.size(), pinholeCamera.hasDistortionParameters(), imagePointsRight.data());

		const Plane3 faultyPlane(Quaternion(Random::euler(Numeric::deg2rad(20))) * plane.normal(), Random::scalar(-5, -1));
		const HomogenousMatrix4 faultyPoseRight(poseRight * HomogenousMatrix4(Random::vector3() * Scalar(0.5), Random::euler(Numeric::deg2rad(30))));

		const Vectors3 faultyPlaneObjectPoints(Geometry::Utilities::backProjectImagePoints(pinholeCamera, poseLeft, faultyPlane, imagePointsLeft.data(), imagePointsLeft.size(), pinholeCamera.hasDistortionParameters()));

		// ensure that all object points are located in front of all cameras
		if (Geometry::Utilities::countFrontObjectPoints(pinholeCamera, poseLeft, planeObjectPoints.data(), planeObjectPoints.size())
			+ Geometry::Utilities::countFrontObjectPoints(pinholeCamera, faultyPoseRight, planeObjectPoints.data(), planeObjectPoints.size())
			+ Geometry::Utilities::countFrontObjectPoints(pinholeCamera, poseRight, planeObjectPoints.data(), planeObjectPoints.size()) != 3 * planeObjectPoints.size())
		{
			continue;
		}

		if (Geometry::Utilities::countFrontObjectPoints(pinholeCamera, poseLeft, faultyPlaneObjectPoints.data(), faultyPlaneObjectPoints.size())
			+ Geometry::Utilities::countFrontObjectPoints(pinholeCamera, faultyPoseRight, faultyPlaneObjectPoints.data(), faultyPlaneObjectPoints.size())
			+ Geometry::Utilities::countFrontObjectPoints(pinholeCamera, poseRight, faultyPlaneObjectPoints.data(), faultyPlaneObjectPoints.size()) != 3 * faultyPlaneObjectPoints.size())
		{
			continue;
		}

		HomogenousMatrix4 optimizedPoseRight;
		Plane3 optimizedPlane;

		performance.start();
		if (Geometry::NonLinearOptimizationPlane::optimizeOnePoseOnePlane(pinholeCamera, poseLeft, faultyPoseRight, faultyPlane, ConstArrayAccessor<Vector2>(imagePointsLeft), ConstArrayAccessor<Vector2>(imagePointsRight), pinholeCamera.hasDistortionParameters(), optimizedPoseRight, optimizedPlane, 20u, type, Scalar(0.001), Scalar(5), true))
		{
			performance.stop();

			const Scalar sqrAverageInitialError = Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, false, false>(poseLeft, pinholeCamera, ConstTemplateArrayAccessor<Vector3>(planeObjectPoints), ConstTemplateArrayAccessor<Vector2>(imagePointsLeft), pinholeCamera.hasDistortionParameters())
														+ Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, false, false>(faultyPoseRight, pinholeCamera, ConstTemplateArrayAccessor<Vector3>(planeObjectPoints), ConstTemplateArrayAccessor<Vector2>(imagePointsRight), pinholeCamera.hasDistortionParameters());

			const Scalar initialAngleError = plane.normal().angle(faultyPlane.normal());

			const Vectors3 optimizedObjectPoints(Geometry::Utilities::backProjectImagePoints(pinholeCamera, poseLeft, optimizedPlane, imagePointsLeft.data(), imagePointsLeft.size(), pinholeCamera.hasDistortionParameters()));

			const Scalar sqrAverageOptimizedError = Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, false, false>(poseLeft, pinholeCamera, ConstTemplateArrayAccessor<Vector3>(optimizedObjectPoints), ConstTemplateArrayAccessor<Vector2>(imagePointsLeft), pinholeCamera.hasDistortionParameters())
														+ Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, false, false>(optimizedPoseRight, pinholeCamera, ConstTemplateArrayAccessor<Vector3>(optimizedObjectPoints), ConstTemplateArrayAccessor<Vector2>(imagePointsRight), pinholeCamera.hasDistortionParameters());

			const Scalar optimizedAngleError = plane.normal().angle(optimizedPlane.normal());

			averageInitialSqrError += sqrAverageInitialError;
			averageInitialAngleError += initialAngleError;
			medianInitialPixelErrors.push_back(sqrAverageInitialError);
			medianInitialAngleErrors.push_back(initialAngleError);

			averageOptimizedSqrError += sqrAverageOptimizedError;
			averageOptimizedAngleError += optimizedAngleError;
			medianOptimizedPixelErrors.push_back(sqrAverageOptimizedError);
			medianOptimizedAngleErrors.push_back(optimizedAngleError);

			succeeded++;
		}
		else
			performance.skip();

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	averageInitialSqrError /= Scalar(iterations);
	averageInitialAngleError /= Scalar(iterations);
	averageOptimizedSqrError /= Scalar(iterations);
	averageOptimizedAngleError /= Scalar(iterations);

	Log::info() << "Average pixel error: " << String::toAString(averageInitialSqrError, 1u) << "px -> " << String::toAString(averageOptimizedSqrError, 1u) << "px";
	Log::info() << "Median sqr pixel error: " << String::toAString(Median::constMedian(medianInitialPixelErrors.data(), medianInitialPixelErrors.size()), 1u) << "px -> " << String::toAString(Median::constMedian(medianOptimizedPixelErrors.data(), medianOptimizedPixelErrors.size()), 1u) << "px";
	Log::info() << "Average angle error: " << String::toAString(Numeric::rad2deg(averageInitialAngleError), 1u) << "deg -> " << String::toAString(Numeric::rad2deg(averageOptimizedAngleError), 1u) << "deg";
	Log::info() << "Median angle error: " << String::toAString(Numeric::rad2deg(Median::constMedian(medianInitialAngleErrors.data(), medianInitialAngleErrors.size())), 1u) << "deg -> " << String::toAString(Numeric::rad2deg(Median::constMedian(medianOptimizedAngleErrors.data(), medianOptimizedAngleErrors.size())), 1u) << "deg";
	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 1u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 1u) << "ms, average: " << String::toAString(performance.averageMseconds(), 1u) << "ms, first: " << String::toAString(performance.firstMseconds(), 1u) << "ms";
	Log::info() << "Validation: " << String::toAString(double(succeeded) * 100.0 / double(iterations), 1u) << "% succeeded.";

	return true;
}

bool TestNonLinearOptimizationPlane::testOptimizePosesOnePlane(const unsigned int numberPoses, const unsigned int correspondences, const double testDuration, const Geometry::Estimator::EstimatorType type)
{
	ocean_assert(numberPoses >= 2u);
	ocean_assert(correspondences >= 4u);

	Scalar averageInitialSqrError = 0;
	Scalar averageInitialAngleError = 0;

	Scalar averageOptimizedSqrError = 0;
	Scalar averageOptimizedAngleError = 0;

	Scalars medianInitialPixelErrors;
	Scalars medianInitialAngleErrors;

	Scalars medianOptimizedPixelErrors;
	Scalars medianOptimizedAngleErrors;

	unsigned long long iterations = 0ull;
	unsigned long long succeeded = 0ull;

	HighPerformanceStatistic performance;

	PinholeCamera pinholeCamera(640, 480, Numeric::deg2rad(60));
	const Vector3 zeroViewingDirection(0.0, 0, -1);

	const Timestamp startTimestamp(true);

	do
	{
		const Vectors3 volumeObjectPoints(Utilities::objectPoints(Box3(Vector3(-0.5, -0.5, -0.5), Vector3(0.5, 0.5, 0.5)), correspondences));
		Vectors3 planeObjectPoints;

		Vectors2 initialImagePoints;
		std::vector<Vectors2> imagePointsSet;

		const Plane3 plane(Vector3(0, 0, 1), 0);

		HomogenousMatrix4 initialPose(Utilities::viewPosition(pinholeCamera, volumeObjectPoints, zeroViewingDirection));

		HomogenousMatrices4 poses;
		HomogenousMatrices4 faultyPoses;

		for (unsigned int n = 0u; n < numberPoses; ++n)
		{
			// the first pose is the identity transformation for that the 3D object points are created
			if (n == 0u)
			{
				Vectors2 imagePoints;
				for (Vectors3::const_iterator i = volumeObjectPoints.begin(); i != volumeObjectPoints.end(); ++i)
				{
					const Vector2 imagePoint(pinholeCamera.projectToImage<true>(initialPose, *i, pinholeCamera.hasDistortionParameters()));
					const Line3 ray(pinholeCamera.ray(pinholeCamera.projectToImage<true>(initialPose, *i, false), initialPose));

					Vector3 objectPoint(0, 0, 0);

					if (!plane.intersection(ray, objectPoint))
						ocean_assert(false);

					imagePoints.push_back(imagePoint);
					planeObjectPoints.push_back(objectPoint);
				}

				initialImagePoints = imagePoints;
			}
			else
			{
				const Vector3 viewingDirection = Quaternion(Random::euler(Numeric::deg2rad(20))) * zeroViewingDirection;
				const HomogenousMatrix4 pose(Utilities::viewPosition(pinholeCamera, volumeObjectPoints, viewingDirection));
				const HomogenousMatrix4 faultyPose(Utilities::viewPosition(pinholeCamera, volumeObjectPoints, Quaternion(Random::euler(Numeric::deg2rad(30))) * viewingDirection));

				Vectors2 imagePoints;
				for (Vectors3::const_iterator i = planeObjectPoints.begin(); i != planeObjectPoints.end(); ++i)
				{
					const Vector2 imagePoint(pinholeCamera.projectToImage<true>(pose, *i, pinholeCamera.hasDistortionParameters()));
					imagePoints.push_back(imagePoint);
				}

				imagePointsSet.push_back(imagePoints);

				poses.push_back(pose);
				faultyPoses.push_back(faultyPose);
			}
		}

		const Plane3 faultyPlane(Quaternion(Random::euler(Numeric::deg2rad(20))) * plane.normal(), plane.distance() + Random::scalar(Scalar(-5.1), Scalar(-1.1)));
		const Vectors3 faultyPlaneObjectPoints(Geometry::Utilities::backProjectImagePoints(pinholeCamera, initialPose, faultyPlane, initialImagePoints.data(), initialImagePoints.size(), pinholeCamera.hasDistortionParameters()));

		// ensure that all object points are located in front of all cameras
		if (Geometry::Utilities::countFrontObjectPoints(pinholeCamera, initialPose, planeObjectPoints.data(), planeObjectPoints.size())
				+ Geometry::Utilities::countFrontObjectPoints(pinholeCamera, initialPose, faultyPlaneObjectPoints.data(), faultyPlaneObjectPoints.size()) != 2 * planeObjectPoints.size())
				continue;

		bool validData = true;
		for (size_t n = 0; n < poses.size(); ++n)
		{
			if (Geometry::Utilities::countFrontObjectPoints(pinholeCamera, poses[n], planeObjectPoints.data(), planeObjectPoints.size())
				+ Geometry::Utilities::countFrontObjectPoints(pinholeCamera, faultyPoses[n], planeObjectPoints.data(), planeObjectPoints.size())
				+ Geometry::Utilities::countFrontObjectPoints(pinholeCamera, poses[n], faultyPlaneObjectPoints.data(), faultyPlaneObjectPoints.size())
				+ Geometry::Utilities::countFrontObjectPoints(pinholeCamera, faultyPoses[n], faultyPlaneObjectPoints.data(), faultyPlaneObjectPoints.size()) != 4 * faultyPlaneObjectPoints.size())
			{
				validData = false;
				break;
			}
		}

		if (!validData)
			continue;

		const Vectors2 totalImagePoints(concatenate(imagePointsSet));

		Plane3 optimizedPlane;
		HomogenousMatrices4 optimizedPoses(poses.size());

		performance.start();

		if (Geometry::NonLinearOptimizationPlane::optimizePosesPlane(pinholeCamera, initialPose, initialImagePoints, faultyPoses, Plane3(faultyPlane.normal(), plane.distance()), imagePointsSet, pinholeCamera.hasDistortionParameters(), optimizedPoses, optimizedPlane, 50u, type, Scalar(0.001), Scalar(5), true))
		{
			performance.stop();

			Scalar sqrAverageInitialError = Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, false, false>(initialPose, pinholeCamera, ConstTemplateArrayAccessor<Vector3>(planeObjectPoints), ConstTemplateArrayAccessor<Vector2>(initialImagePoints), pinholeCamera.hasDistortionParameters());
			for (size_t n = 0; n < poses.size(); ++n)
				sqrAverageInitialError += Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, false, false>(faultyPoses[n], pinholeCamera, ConstTemplateArrayAccessor<Vector3>(planeObjectPoints), ConstTemplateArrayAccessor<Vector2>(imagePointsSet[n]), pinholeCamera.hasDistortionParameters());

			const Scalar initialAngleError = plane.normal().angle(faultyPlane.normal());

			const Vectors3 optimizedObjectPoints(Geometry::Utilities::backProjectImagePoints(pinholeCamera, initialPose, optimizedPlane, initialImagePoints.data(), initialImagePoints.size(), pinholeCamera.hasDistortionParameters()));

			Scalar sqrAverageOptimizedError = Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, false, false>(initialPose, pinholeCamera, ConstTemplateArrayAccessor<Vector3>(optimizedObjectPoints), ConstTemplateArrayAccessor<Vector2>(initialImagePoints), pinholeCamera.hasDistortionParameters());
			for (size_t n = 0; n < poses.size(); ++n)
				sqrAverageOptimizedError += Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, false, false>(optimizedPoses[n], pinholeCamera, ConstTemplateArrayAccessor<Vector3>(optimizedObjectPoints), ConstTemplateArrayAccessor<Vector2>(imagePointsSet[n]), pinholeCamera.hasDistortionParameters());

			const Scalar optimizedAngleError = plane.normal().angle(optimizedPlane.normal());

			averageInitialSqrError += sqrAverageInitialError;
			averageInitialAngleError += initialAngleError;
			medianInitialPixelErrors.push_back(sqrAverageInitialError);
			medianInitialAngleErrors.push_back(initialAngleError);

			averageOptimizedSqrError += sqrAverageOptimizedError;
			averageOptimizedAngleError += optimizedAngleError;
			medianOptimizedPixelErrors.push_back(sqrAverageOptimizedError);
			medianOptimizedAngleErrors.push_back(optimizedAngleError);

			succeeded++;
		}
		else
			performance.skip();

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	averageInitialSqrError /= Scalar(iterations);
	averageInitialAngleError /= Scalar(iterations);
	averageOptimizedSqrError /= Scalar(iterations);
	averageOptimizedAngleError /= Scalar(iterations);

	Log::info() << "Average pixel error: " << String::toAString(averageInitialSqrError, 1u) << "px -> " << String::toAString(averageOptimizedSqrError, 1u) << "px";
	Log::info() << "Median sqr pixel error: " << String::toAString(Median::constMedian(medianInitialPixelErrors.data(), medianInitialPixelErrors.size()), 1u) << "px -> " << String::toAString(Median::constMedian(medianOptimizedPixelErrors.data(), medianOptimizedPixelErrors.size()), 1u) << "px";
	Log::info() << "Average angle error: " << String::toAString(Numeric::rad2deg(averageInitialAngleError), 1u) << "deg -> " << String::toAString(Numeric::rad2deg(averageOptimizedAngleError), 1u) << "deg";
	Log::info() << "Median angle error: " << String::toAString(Numeric::rad2deg(Median::constMedian(medianInitialAngleErrors.data(), medianInitialAngleErrors.size())), 1u) << "deg -> " << String::toAString(Numeric::rad2deg(Median::constMedian(medianOptimizedAngleErrors.data(), medianOptimizedAngleErrors.size())), 1u) << "deg";
	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 1u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 1u) << "ms, average: " << String::toAString(performance.averageMseconds(), 1u) << "ms, first: " << String::toAString(performance.firstMseconds(), 1u) << "ms";
	Log::info() << "Validation: " << String::toAString(double(succeeded) * 100.0 / double(iterations), 1u) << "% succeeded.";

	return true;
}

}

}

}
