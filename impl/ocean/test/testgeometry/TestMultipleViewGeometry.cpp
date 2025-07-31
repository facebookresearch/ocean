/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestMultipleViewGeometry.h"
#include "ocean/test/testgeometry/Utilities.h"

#include "ocean/base/Accessor.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/geometry/EpipolarGeometry.h"
#include "ocean/geometry/RANSAC.h"
#include "ocean/geometry/MultipleViewGeometry.h"

#include "ocean/math/Euler.h"
#include "ocean/math/Line3.h"
#include "ocean/math/Plane3.h"
#include "ocean/math/Random.h"
#include "ocean/math/Variance.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

bool TestMultipleViewGeometry::testMultipleViewGeometry(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Multiple view geometry test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testTrifocalTensorMatrix(false, testDuration) && allSucceeded;

	Log::info() << " ";

	// trifocal tensor fails (0% success rate) on noised data as expected, but nice to see
	testTrifocalTensorMatrix(true, testDuration);

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testProjectiveReconstructionFrom3Views(false, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	// projection reconstruction fails (success rate < 30%) on noised data as expected, but nice to see
	testProjectiveReconstructionFrom3Views(true, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	for (unsigned int iView = 4; iView < 11; iView += 2)
	{
		allSucceeded = testProjectiveReconstruction(iView, false, testDuration) && allSucceeded;

		Log::info() << " ";

		testProjectiveReconstruction(iView, true, testDuration) && allSucceeded;

		Log::info() << " ";

		allSucceeded = testFaultyProjectiveReconstruction(iView, testDuration) && allSucceeded;

		Log::info() << " ";
	}

	if (allSucceeded)
	{
		Log::info() << "Multiple view geometry test succeeded.";
	}
	else
	{
		Log::info() << "Multiple view geometry test FAILED!";
	}

	return allSucceeded;
}

bool TestMultipleViewGeometry::testTrifocalTensorMatrix(bool addGaussianNoise, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	if (addGaussianNoise)
	{
		Log::info() << "Testing trifocal tensor determination with Gaussian noise from three views (is expected to fail):";
	}
	else
	{
		Log::info() << "Testing trifocal tensor with perfect image points from three views:";
	}

	bool allSucceeded = true;

	const Scalar sigma = addGaussianNoise ? Scalar(1) : Scalar(0);

	for (const unsigned int points : {7u, 35u, 350u})
	{
		Log::info() << "... with " << points << " points:";

		uint64_t failedMetric = 0ull;
		uint64_t validIterations = 0ull;

		const Timestamp startTimestamp(true);
		HighPerformanceStatistic performance;
		Scalars maxProjectionErrorsMetric;

		do
		{
			std::vector<Vectors2> imagePointsPerPose;
			imagePointsPerPose.reserve(3);

			Vectors3 centerObjectPoints;
			centerObjectPoints.reserve(points);

			const PinholeCamera pinholeCamera(Random::random(600, 800), Random::random(600, 800), Numeric::deg2rad(Random::scalar(30, 70)));

			if (!generatedImagePointGroups(pinholeCamera, points, 3u, imagePointsPerPose, sigma, &centerObjectPoints))
			{
				continue;
			}

			Geometry::MultipleViewGeometry::TrifocalTensor trifocal;
			HomogenousMatrix4 projectiveMatrix0, projectiveMatrix1, projectiveMatrix2;

			performance.start();
			const bool success = Geometry::MultipleViewGeometry::trifocalTensorIF(imagePointsPerPose[0].data(), imagePointsPerPose[1].data(), imagePointsPerPose[2].data(), points, projectiveMatrix0, projectiveMatrix1, projectiveMatrix2, &trifocal);
			performance.stop();

			if (success)
			{
				/* some extra tests
				Vector3 normedEpipoleIF2, normedEpipoleIF3;
				if (Ocean::Geometry::MultipleViewGeometry::epipolesIF(trifocal, normedEpipoleIF2, normedEpipoleIF3))
				{
					// test fundamental matrix
					SquareMatrix3 fundamdental21, fundamdental31;
					if (Geometry::MultipleViewGeometry::fundamentalMatricesIF(trifocal, normedEpipoleIF2, normedEpipoleIF3, fundamdental21, fundamdental31))
					{
						for (size_t i = 0; i < imagePointsPerPose[0].size(); ++i)
						{
							const Vector3 right(imagePointsPerPose[2][i], 1);
							const Vector3 left(imagePointsPerPose[1][i], 1);
							const Vector3 center(imagePointsPerPose[0][i], 1);

							const Scalar scalarProduct21 = (fundamdental21 * center) * left;
							const Scalar scalarProduct31 = (fundamdental31 * center) * right;

							if (Numeric::isNotWeakEqualEps(scalarProduct21) || Numeric::isNotWeakEqualEps(scalarProduct31))
								Log::info() << "fundamental matrix failed";
						}
					}
				}
				*/
				HomogenousMatrices4 transformationIFs(3);
				transformationIFs[0] = projectiveMatrix0;
				transformationIFs[1] = projectiveMatrix1;
				transformationIFs[2] = projectiveMatrix2;

				SquareMatrix3 intrinsic;
				HomogenousMatrices4 posesIF(3);
				if (!Geometry::MultipleViewGeometry::calibrateFromProjectionsMatricesIF(ConstArrayAccessor<HomogenousMatrix4>(transformationIFs), pinholeCamera.width(), pinholeCamera.height(), intrinsic, posesIF.data()))
				{
					continue;
				}

				++validIterations;

				const PinholeCamera calibratedCamera(intrinsic, pinholeCamera.width(), pinholeCamera.height());

				Scalar maxSquaredMetricError = evaluateReprojectionError(imagePointsPerPose, calibratedCamera, posesIF);
				maxProjectionErrorsMetric.push_back(Numeric::sqrt(maxSquaredMetricError));

				if (maxSquaredMetricError > (addGaussianNoise ? (2.5 * 2.5) : (1.5 * 1.5)))
				{
					++failedMetric;
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		Log::info() << "Performance in ms: " << String::toAString(performance.averageMseconds());

		if (validIterations < 1)
		{
			allSucceeded = false;
			Log::info() << "No succeeded executions";
		}
		else
		{
			const Scalar percentage(Scalar(validIterations - failedMetric) / Scalar(validIterations));
			const Scalar medianMetric = maxProjectionErrorsMetric.size() > 0 ? Median::median(maxProjectionErrorsMetric.data(), maxProjectionErrorsMetric.size()) : 0;

			Log::info() << "Validation: " << String::toAString(percentage * 100.00, 1u) << "% succeeded. Median maximal re-projection error: " << String::toAString(medianMetric, 1u) << " pixel";

			allSucceeded = (addGaussianNoise ? percentage >= 0.75 : percentage >= 0.95) && allSucceeded;
		}
	}

	return allSucceeded;
}

bool TestMultipleViewGeometry::testProjectiveReconstructionFrom3Views(bool addGaussianNoise, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	if (addGaussianNoise)
		Log::info() << "Projective reconstruction with 6 Gaussian noised point correspondences from three views (is expected to fail):";
	else
		Log::info() << "Projective reconstruction with 6 faultless point correspondences from three views:";

	const size_t points = 6u;

	const Scalar sigma = addGaussianNoise ? Scalar(1) : Scalar(0);

	uint64_t failedMetric = 0ull;
	uint64_t validIterations = 0ull;

	const Timestamp startTimestamp(true);
	HighPerformanceStatistic performance;
	Scalars maxProjectionErrorsMetric;

	do
	{
		std::vector<Geometry::ImagePoints> imagePointsPerPose;
		imagePointsPerPose.reserve(3);
		Geometry::ObjectPoints centerObjectPoints;
		centerObjectPoints.reserve(points);

		const PinholeCamera pinholeCamera(Random::random(600, 800), Random::random(600, 800), Numeric::deg2rad(Random::scalar(30, 70)));

		if (!generatedImagePointGroups(pinholeCamera, points, 3u, imagePointsPerPose, sigma, &centerObjectPoints))
			continue;

		HomogenousMatrix4 projectiveMatrix0, projectiveMatrix1, projectiveMatrix2;
		HomogenousMatrices4 projectionIF;
		projectionIF.push_back(projectiveMatrix0);
		projectionIF.push_back(projectiveMatrix1);
		projectionIF.push_back(projectiveMatrix2);

		bool success;
		{
			HighPerformanceStatistic::ScopedStatistic scoped(performance);
			success = Geometry::MultipleViewGeometry::projectiveReconstructionFrom6PointsIF(imagePointsPerPose[0].data(), imagePointsPerPose[1].data(), imagePointsPerPose[2].data(), points, projectiveMatrix0, projectiveMatrix1, projectiveMatrix2, 2);
		}
		if (success)
		{

			HomogenousMatrices4 transformationIFs(3);
			transformationIFs[0] = projectiveMatrix0;
			transformationIFs[1] = projectiveMatrix1;
			transformationIFs[2] = projectiveMatrix2;

			SquareMatrix3 intrinsic;
			HomogenousMatrices4 posesIF(3);
			if (!Geometry::MultipleViewGeometry::calibrateFromProjectionsMatricesIF(ConstArrayAccessor<HomogenousMatrix4>(transformationIFs), pinholeCamera.width(), pinholeCamera.height(), intrinsic, posesIF.data()))
				continue;

			++validIterations;

			const PinholeCamera calibratedCamera(intrinsic, pinholeCamera.width(), pinholeCamera.height());

			Scalar maxSquaredMetricError = evaluateReprojectionError(imagePointsPerPose, calibratedCamera, posesIF);
			maxProjectionErrorsMetric.push_back(Numeric::sqrt(maxSquaredMetricError));

			if (maxSquaredMetricError > (addGaussianNoise ? (2.5 * 2.5) : (1.5 * 1.5)))
				++failedMetric;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance in ms: " << String::toAString(performance.averageMseconds());

	if (validIterations < 1)
	{
		return false;
	}
	else
	{
		const Scalar percentage(Scalar(validIterations - failedMetric) / Scalar(validIterations));
		const Scalar medianMetric = maxProjectionErrorsMetric.size() > 0 ? Median::median(maxProjectionErrorsMetric.data(), maxProjectionErrorsMetric.size()) : 0;

		Log::info() << "Validation: " << String::toAString(percentage * 100.00, 1u) << "% succeeded. Median maximal re-projection error: " << String::toAString(medianMetric, 1u) << " pixel";

		return addGaussianNoise? percentage > 0.25 : percentage > 0.5;
	}
}

bool TestMultipleViewGeometry::testProjectiveReconstruction(const unsigned int views, bool addGaussianNoise, const double testDuration)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(views > 2);

	if (addGaussianNoise)
		Log::info() << "Projective reconstruction with 6 Gaussian noised point correspondences from " << views << " views: (is expected to fail)";
	else
		Log::info() << "Projective reconstruction with 6 faultless point correspondences from " << views << " views:";

	const size_t points = 6u;

	const Scalar sigma = addGaussianNoise ? Scalar(1) : Scalar(0);

	uint64_t failedMetric = 0ull;
	uint64_t validIterations = 0ull;

	const Timestamp startTimestamp(true);
	HighPerformanceStatistic performance;
	Scalars maxProjectionErrorsMetric;

	do
	{
		std::vector<Geometry::ImagePoints> imagePointsPerPose;
		imagePointsPerPose.reserve(views);
		Geometry::ObjectPoints centerObjectPoints;
		centerObjectPoints.reserve(points);

		const PinholeCamera pinholeCamera(Random::random(600, 800), Random::random(600, 800), Numeric::deg2rad(Random::scalar(30, 70)));

		if (!generatedImagePointGroups(pinholeCamera, points, views, imagePointsPerPose, sigma, &centerObjectPoints))
			continue;

		HomogenousMatrices4 projectionIF(views);
		NonconstArrayAccessor<HomogenousMatrix4> projAccessor(projectionIF, views);

		bool success;
		{
			HighPerformanceStatistic::ScopedStatistic scoped(performance);
			success = Geometry::MultipleViewGeometry::projectiveReconstructionFrom6PointsIF(ConstArrayAccessor<Geometry::ImagePoints>(imagePointsPerPose), projAccessor.pointer(), 2);
		}

		if (success)
		{
			SquareMatrix3 intrinsic;
			HomogenousMatrices4 posesIF(views);
			if (!Geometry::MultipleViewGeometry::calibrateFromProjectionsMatricesIF(ConstArrayAccessor<HomogenousMatrix4>(projectionIF), pinholeCamera.width(), pinholeCamera.height(), intrinsic, posesIF.data()))
				continue;

			++validIterations;

			const PinholeCamera calibratedCamera(intrinsic, pinholeCamera.width(), pinholeCamera.height());

			Scalar maxSquaredMetricError = evaluateReprojectionError(imagePointsPerPose, calibratedCamera, posesIF);
			maxProjectionErrorsMetric.push_back(Numeric::sqrt(maxSquaredMetricError));

			if (maxSquaredMetricError > (addGaussianNoise ? (2.5 * 2.5) : (1.5 * 1.5)))
				++failedMetric;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance in ms: " << String::toAString(performance.averageMseconds());

	if (validIterations < 1)
	{
		return false;
	}
	else
	{
		const Scalar percentage(Scalar(validIterations - failedMetric) / Scalar(validIterations));
		const Scalar medianMetric = maxProjectionErrorsMetric.size() > 0 ? Median::median(maxProjectionErrorsMetric.data(), maxProjectionErrorsMetric.size()) : 0;

		Log::info() << "Validation: " << String::toAString(percentage * 100.00, 1u) << "% succeeded. Median maximal re-projection error: " << String::toAString(medianMetric, 1u) << " pixel";

		return addGaussianNoise? percentage > 0.25 : percentage > 0.75;
	}
}

bool TestMultipleViewGeometry::testFaultyProjectiveReconstruction(const unsigned int views, const double testDuration)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(views > 2);

	Log::info() << "Projective reconstruction with faulty point correspondences from " << views << " views:";

	bool allSucceeded = true;

	for (const unsigned int points : {15u, 50u, 500u})
	{
		Log::info() << "... with " << points << " points:";

		uint64_t failedMetric = 0ull;
		uint64_t validIterations = 0ull;

		const Timestamp startTimestamp(true);
		HighPerformanceStatistic performance;
		Scalars maxProjectionErrorsMetric;

		do
		{
			std::vector<Geometry::ImagePoints> imagePointsPerPose;
			imagePointsPerPose.reserve(views);
			Geometry::ObjectPoints centerObjectPoints;
			centerObjectPoints.reserve(points);

			const PinholeCamera pinholeCamera(Random::random(600, 800), Random::random(600, 800), Numeric::deg2rad(Random::scalar(30, 70)));

			if (!generatedImagePointGroups(pinholeCamera, points, views, imagePointsPerPose, 0, &centerObjectPoints))
				continue;

			// randomly select some image points to disturb
			std::vector<Geometry::ImagePoints> distortedImagePointsPerPose(views);
			for (size_t i = 0; i < views; i++)
			{
				distortedImagePointsPerPose[i] = imagePointsPerPose[i];
			}

			std::set<unsigned int> indexSetDisturbPoints;

			unsigned int numberInvalidFeatures = points * 2u / 10u;
			ocean_assert(numberInvalidFeatures < points);

			while (indexSetDisturbPoints.size() < numberInvalidFeatures)
			{
				indexSetDisturbPoints.insert(Random::random(points - 1));
			}

			for (std::set<unsigned int>::const_iterator i = indexSetDisturbPoints.begin(); i != indexSetDisturbPoints.end(); ++i)
			{
				ocean_assert(*i < points);

				for (size_t iView = 0; iView < views; iView++)
				{
					Geometry::ImagePoint& imagePoint = distortedImagePointsPerPose[iView][*i];
					imagePoint += Vector2(Random::scalar(-15, 15), Random::scalar(-15, 15));
					imagePoint.x() = max(Scalar(0), min(Scalar(pinholeCamera.width() - 1u), imagePoint.x()));
					imagePoint.y() = max(Scalar(0), min(Scalar(pinholeCamera.height() - 1u), imagePoint.y()));
				}
			}

			HomogenousMatrices4 projectionIF(views);
			NonconstArrayAccessor<HomogenousMatrix4> projAccessor(projectionIF, views);

			bool success;
			{
				HighPerformanceStatistic::ScopedStatistic scoped(performance);
				success = Geometry::RANSAC::projectiveReconstructionFrom6PointsIF(ConstArrayAccessor<Geometry::ImagePoints>(distortedImagePointsPerPose), projAccessor.pointer());
			}

			if (success)
			{
				// valid only with undistorted image points
				SquareMatrix3 intrinsic;
				HomogenousMatrices4 posesIF(views);
				if (!Geometry::MultipleViewGeometry::calibrateFromProjectionsMatricesIF(ConstArrayAccessor<HomogenousMatrix4>(projectionIF), pinholeCamera.width(), pinholeCamera.height(), intrinsic, posesIF.data()))
					continue;

				++validIterations;
				const PinholeCamera calibratedCamera(intrinsic, pinholeCamera.width(), pinholeCamera.height());

				Scalar maxSquaredMetricError = evaluateReprojectionError(imagePointsPerPose, calibratedCamera, posesIF);
				maxProjectionErrorsMetric.push_back(Numeric::sqrt(maxSquaredMetricError));

				if (maxSquaredMetricError > (1.5 * 1.5))
					++failedMetric;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		Log::info() << "Performance in ms: " << String::toAString(performance.averageMseconds());

		if (validIterations < 1)
		{
			allSucceeded = false;
			Log::info() << "No succeeded executions";

		}
		else
		{
			const Scalar percentage(Scalar(validIterations - failedMetric) / Scalar(validIterations));
			const Scalar medianMetric = maxProjectionErrorsMetric.size() > 0 ? Median::median(maxProjectionErrorsMetric.data(), maxProjectionErrorsMetric.size()) : 0;

			Log::info() << "Validation: " << String::toAString(percentage * 100.00, 1u) << "% succeeded. Median maximal re-projection error: " << String::toAString(medianMetric, 1u) << " pixel";

			allSucceeded = percentage >= 0.5 && allSucceeded;
		}
	}
	return allSucceeded;
}

bool TestMultipleViewGeometry::generatedImagePointGroups(const PinholeCamera& pinholeCamera, const size_t points, const unsigned int views, std::vector<Vectors2>& imagePointsPerPose, Scalar gaussSigma, Vectors3* objectPoints)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(views >= 2 && points != 0 && gaussSigma >= 0);

	imagePointsPerPose.resize(views);

#ifdef OCEAN_OBJ_FIRST

	/*NOTE: results in lower performance */
	const Box3 objectPointsArea(Vector3(-1, -1, -1), Vector3(1, 1, 1));

	const Quaternion orientation0(Random::quaternion());
	const Vector3 viewDirection0(orientation0 * Vector3(0, 0, -1));

	const Geometry::ObjectPoints perfectObjectPoints(Utilities::objectPoints(objectPointsArea, points));

	HomogenousMatrices4 poses;
	poses.reserve(views);
	poses.push_back(Utilities::viewPosition(pinholeCamera, perfectObjectPoints, viewDirection0, true));

	while (poses.size() < views)
	{
		const Quaternion offsetRotation(Random::euler(Numeric::deg2rad(5), Numeric::deg2rad(35)));

		const Quaternion newOrientation(orientation0 * offsetRotation);
		const Vector3 newViewDirection(newOrientation * Vector3(0, 0, -1));

		const Scalar angle(Numeric::rad2deg(viewDirection0.angle(newViewDirection)));
		ocean_assert(Numeric::isInsideRange(5, angle, 85));

		poses.push_back(Utilities::viewPosition(pinholeCamera, perfectObjectPoints, newViewDirection, true));
	}

	for (unsigned int p = 0u; p < views; ++p)
	{
		Geometry::ImagePoints& imagePoints = imagePointsPerPose[p];
		const HomogenousMatrix4& pose = poses[p];

		for (unsigned int n = 0; n < points; ++n)
		{
			const Vector2 imagePoint(pinholeCamera.projectToImage<false>(pose, perfectObjectPoints[n], false));

			Vector2 imagePointNoise(0, 0);
			if (gaussSigma > 0)
			{
				imagePointNoise = Vector2(Random::gaussianNoise(gaussSigma), Random::gaussianNoise(gaussSigma));
				imagePoints.push_back(imagePoint + imagePointNoise);
			}
			else
				imagePoints.push_back(imagePoint);
		}
	}

	if (objectPoints)
		*objectPoints = perfectObjectPoints;

#else

	Geometry::ObjectPoints centerObjectPoints;
	centerObjectPoints.reserve(points);

	HomogenousMatrices4 poses;
	poses.reserve(views);

	for (size_t iView = 0; iView < views - 1; iView++)
	{
		imagePointsPerPose[iView].reserve(points);

		const Vector3 translation(Random::vector3(Scalar(-0.1), Scalar(0.1)));
		const Euler euler(Random::scalar(Numeric::deg2rad(-10), Numeric::deg2rad(10)), Random::scalar(Numeric::deg2rad(-10), Numeric::deg2rad(10)), Random::scalar(Numeric::deg2rad(-10), Numeric::deg2rad(10)));
		const Quaternion quaternion(euler);

		poses.push_back(HomogenousMatrix4(translation, quaternion));
	}

	for (size_t iterations = 0; iterations < points; ++iterations)
	{
		const Geometry::ImagePoint imagePoint(Random::scalar(0, Scalar(pinholeCamera.width())), Random::scalar(0, Scalar(pinholeCamera.height())));
		const Geometry::ObjectPoint objectPoint(pinholeCamera.vector(imagePoint) * Random::scalar(1, 3));

		Geometry::ImagePoints candidates;
		candidates.reserve(views);
		for (size_t iView = 0; iView < views - 1; iView++)
		{
			Geometry::ImagePoint imagePointView(pinholeCamera.projectToImage<false>(poses[iView], objectPoint, false));

			if (gaussSigma > 0)
			{
				imagePointView.x() += Random::gaussianNoise(gaussSigma);
				imagePointView.y() += Random::gaussianNoise(gaussSigma);
			}

			candidates.push_back(imagePointView);
		}

		centerObjectPoints.push_back(objectPoint);
		imagePointsPerPose[0].push_back(imagePoint);

		for (size_t iView = 0; iView < views - 1; iView++)
		{
			imagePointsPerPose[iView + 1].push_back(candidates[iView]);
		}
	}

	if (objectPoints)
		*objectPoints = centerObjectPoints;

#endif

#ifdef OCEAN_INTENSIVE_DEBUG

	if (views > 3)
	{
		HomogenousMatrices4 projectionsIF;
		for (size_t i = 0; i < views - 1; i++)
		{
			projectionsIF.push_back(pinholeCamera.transformationMatrixIF(poses[i]));
		}

		SquareMatrix4 symmetricQ;
		Geometry::SelfCalibration::findAbsoluteDualQuadricLinear(ConstArrayAccessor<HomogenousMatrix4>(projectionsIF), symmetricQ, pinholeCamera.width(), pinholeCamera.height());

		SquareMatrices3 matK;
		NonconstArrayAccessor<SquareMatrix3> camAccessor(matK, views - 1);
		Geometry::SelfCalibration::getIntrinsicsFromAbsoluteDualQuadric(symmetricQ, ConstArrayAccessor<HomogenousMatrix4>(projectionsIF), camAccessor.pointer());

		SquareMatrix3 camMat;
		Geometry::SelfCalibration::findCommonIntrinsicsFromProjectionMatrices(ConstArrayAccessor<HomogenousMatrix4>(projectionsIF), camMat, &symmetricQ);
	}
#endif

	return true;
}

Scalar TestMultipleViewGeometry::evaluateReprojectionError(const std::vector<Vectors2>& imagePointsPerPose, const PinholeCamera& pinholeCamera, const HomogenousMatrices4& posesIF)
{
	const size_t views = imagePointsPerPose.size();

	if (views == 0)
		return Numeric::maxValue();

	const size_t points = imagePointsPerPose[0].size();

	const Geometry::ObjectPoints points3dMetric = Geometry::EpipolarGeometry::triangulateImagePointsIF(ConstArrayAccessor<HomogenousMatrix4>(posesIF), ConstArrayAccessor<Geometry::ImagePoints>(imagePointsPerPose), &pinholeCamera);

	Scalar maxSquaredMetricError(0);

	for (size_t i = 0; i < points; ++i)
	{
		for (size_t iView = 0; iView < views; iView++)
		{
			const Geometry::ImagePoint imagePoint = pinholeCamera.projectToImageIF<true>(posesIF[iView], points3dMetric[i], false);
			const Scalar sqrMetricDistance(imagePoint.sqrDistance(imagePointsPerPose[iView][i]));

			maxSquaredMetricError = sqrMetricDistance > maxSquaredMetricError ? sqrMetricDistance : maxSquaredMetricError;
		}
	}

	return maxSquaredMetricError;
}

}

}

}
