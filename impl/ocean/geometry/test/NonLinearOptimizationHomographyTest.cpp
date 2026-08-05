/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>

#include "ocean/geometry/NonLinearOptimizationHomography.h"

#include "ocean/math/Matrix.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Vector2.h"

#include <array>
#include <vector>

using namespace Ocean;
using namespace Ocean::Geometry;

namespace
{

SquareMatrix3 matrixFromRows(
	const Scalar m00,
	const Scalar m01,
	const Scalar m02,
	const Scalar m10,
	const Scalar m11,
	const Scalar m12,
	const Scalar m20,
	const Scalar m21,
	const Scalar m22)
{
	const std::array<Scalar, 9> values = {
		m00, m01, m02,
		m10, m11, m12,
		m20, m21, m22};
	return SquareMatrix3(values.data(), true);
}

SquareMatrix3 homography()
{
	return matrixFromRows(
		Scalar(1.04), Scalar(0.08), Scalar(18),
		Scalar(-0.05), Scalar(0.97), Scalar(-13),
		Scalar(0.00022), Scalar(-0.00016), Scalar(1));
}

SquareMatrix3 perturbedHomography()
{
	return matrixFromRows(
		Scalar(0.99), Scalar(0.03), Scalar(12),
		Scalar(-0.01), Scalar(1.02), Scalar(-8),
		Scalar(0.00008), Scalar(-0.00005), Scalar(1));
}

SquareMatrix3 similarity(
	const Scalar scale,
	const Scalar angle,
	const Scalar translationX,
	const Scalar translationY)
{
	const Scalar c = scale * Numeric::cos(angle);
	const Scalar s = scale * Numeric::sin(angle);
	return matrixFromRows(
		c, -s, translationX,
		s, c, translationY,
		Scalar(0), Scalar(0), Scalar(1));
}

Vectors2 imagePoints()
{
	return {
		Vector2(Scalar(90), Scalar(80)),
		Vector2(Scalar(210), Scalar(70)),
		Vector2(Scalar(340), Scalar(95)),
		Vector2(Scalar(480), Scalar(140)),
		Vector2(Scalar(115), Scalar(230)),
		Vector2(Scalar(260), Scalar(220)),
		Vector2(Scalar(420), Scalar(260)),
		Vector2(Scalar(550), Scalar(310)),
		Vector2(Scalar(160), Scalar(380)),
		Vector2(Scalar(315), Scalar(405)),
		Vector2(Scalar(500), Scalar(430))};
}

Vectors2 transformPoints(const SquareMatrix3& transform, const Vectors2& points)
{
	Vectors2 transformedPoints;
	transformedPoints.reserve(points.size());

	for (const Vector2& point : points)
	{
		transformedPoints.emplace_back(transform * point);
	}

	return transformedPoints;
}

Scalar averageReprojectionError(
	const SquareMatrix3& transform,
	const Vectors2& leftPoints,
	const Vectors2& rightPoints)
{
	ocean_assert(leftPoints.size() == rightPoints.size());

	Scalar squareError = Scalar(0);
	for (size_t n = 0; n < leftPoints.size(); ++n)
	{
		squareError += (transform * leftPoints[n]).sqrDistance(rightPoints[n]);
	}

	return Numeric::sqrt(squareError / Scalar(leftPoints.size()));
}

Matrix identityInvertedCovariances(const size_t correspondences)
{
	Matrix covariances(2 * correspondences, 2, Scalar(0));
	for (size_t n = 0; n < correspondences; ++n)
	{
		covariances(2 * n, 0) = Scalar(1);
		covariances(2 * n + 1, 1) = Scalar(1);
	}

	return covariances;
}

Vector2 transformWithCamera(
	const PinholeCamera& camera,
	const SquareMatrix3& pixelHomography,
	const Vector2& leftPoint)
{
	const SquareMatrix3 normalizedHomography(camera.invertedIntrinsic() * pixelHomography * camera.intrinsic());
	const Vector2 undistortedLeft(camera.undistort<true>(leftPoint));
	const Vector2 normalizedLeft(camera.invertedIntrinsic() * undistortedLeft);
	const Vector2 normalizedRight(normalizedHomography * normalizedLeft);

	return camera.normalizedImagePoint2imagePoint<true>(normalizedRight, camera.hasDistortionParameters());
}

Vectors2 transformWithCamera(
	const PinholeCamera& camera,
	const SquareMatrix3& pixelHomography,
	const Vectors2& points)
{
	Vectors2 transformedPoints;
	transformedPoints.reserve(points.size());

	for (const Vector2& point : points)
	{
		transformedPoints.emplace_back(transformWithCamera(camera, pixelHomography, point));
	}

	return transformedPoints;
}

Scalar averageCameraReprojectionError(
	const PinholeCamera& camera,
	const SquareMatrix3& pixelHomography,
	const Vectors2& leftPoints,
	const Vectors2& rightPoints)
{
	ocean_assert(leftPoints.size() == rightPoints.size());

	Scalar squareError = Scalar(0);
	for (size_t n = 0; n < leftPoints.size(); ++n)
	{
		squareError += transformWithCamera(camera, pixelHomography, leftPoints[n]).sqrDistance(rightPoints[n]);
	}

	return Numeric::sqrt(squareError / Scalar(leftPoints.size()));
}

void expectNormalizedHomography(const SquareMatrix3& optimizedHomography)
{
	EXPECT_TRUE(optimizedHomography.isHomography());
	EXPECT_NEAR(optimizedHomography(2, 2), Scalar(1), Scalar(1e-6));
}

TEST(NonLinearOptimizationHomographyTest, OptimizeHomographyEightParameterModelReducesReprojectionError)
{
	const Vectors2 leftPoints = imagePoints();
	const Vectors2 rightPoints = transformPoints(homography(), leftPoints);
	const SquareMatrix3 initialHomography = perturbedHomography();

	SquareMatrix3 optimizedHomography;
	Scalar initialError = Scalar(0);
	Scalar finalError = Scalar(0);
	Scalars intermediateErrors;

	const bool result = NonLinearOptimizationHomography::optimizeHomography(
		initialHomography,
		leftPoints.data(),
		rightPoints.data(),
		leftPoints.size(),
		8u,
		optimizedHomography,
		50u,
		Estimator::ET_SQUARE,
		Scalar(0.001),
		Scalar(5),
		&initialError,
		&finalError,
		nullptr,
		&intermediateErrors);

	ASSERT_TRUE(result);
	EXPECT_LT(finalError, initialError);
	EXPECT_LT(averageReprojectionError(optimizedHomography, leftPoints, rightPoints), Scalar(0.05));
	ASSERT_FALSE(intermediateErrors.empty());
	EXPECT_LE(intermediateErrors.back(), intermediateErrors.front());
	expectNormalizedHomography(optimizedHomography);
}

TEST(NonLinearOptimizationHomographyTest, OptimizeHomographyNineParameterHuberModelUsesCovarianceWeights)
{
	Vectors2 leftPoints = imagePoints();
	Vectors2 rightPoints = transformPoints(homography(), leftPoints);
	rightPoints.back() += Vector2(Scalar(2.5), Scalar(-1.75));

	SquareMatrix3 optimizedHomography;
	Scalar initialError = Scalar(0);
	Scalar finalError = Scalar(0);
	const Matrix covariances = identityInvertedCovariances(leftPoints.size());

	const bool result = NonLinearOptimizationHomography::optimizeHomography<Estimator::ET_HUBER>(
		perturbedHomography(),
		leftPoints.data(),
		rightPoints.data(),
		leftPoints.size(),
		9u,
		optimizedHomography,
		60u,
		Scalar(0.001),
		Scalar(5),
		&initialError,
		&finalError,
		&covariances);

	ASSERT_TRUE(result);
	EXPECT_LT(finalError, initialError);
	EXPECT_LT(averageReprojectionError(optimizedHomography, leftPoints, rightPoints), Scalar(0.9));
	expectNormalizedHomography(optimizedHomography);
}

TEST(NonLinearOptimizationHomographyTest, OptimizeSimilarityPreservesSimilarityShape)
{
	const Vectors2 leftPoints = imagePoints();
	const SquareMatrix3 expectedSimilarity = similarity(
		Scalar(1.07),
		Numeric::deg2rad(Scalar(5)),
		Scalar(21),
		Scalar(-17));
	const Vectors2 rightPoints = transformPoints(expectedSimilarity, leftPoints);

	SquareMatrix3 optimizedSimilarity;
	Scalar initialError = Scalar(0);
	Scalar finalError = Scalar(0);

	const bool result = NonLinearOptimizationHomography::optimizeSimilarity<Estimator::ET_CAUCHY>(
		similarity(Scalar(1.01), Numeric::deg2rad(Scalar(1)), Scalar(14), Scalar(-10)),
		leftPoints.data(),
		rightPoints.data(),
		leftPoints.size(),
		optimizedSimilarity,
		50u,
		Scalar(0.001),
		Scalar(5),
		&initialError,
		&finalError);

	ASSERT_TRUE(result);
	EXPECT_LT(finalError, initialError);
	EXPECT_LT(averageReprojectionError(optimizedSimilarity, leftPoints, rightPoints), Scalar(0.05));
	EXPECT_NEAR(optimizedSimilarity(0, 0), optimizedSimilarity(1, 1), Scalar(1e-6));
	EXPECT_NEAR(optimizedSimilarity(1, 0), -optimizedSimilarity(0, 1), Scalar(1e-6));
	EXPECT_NEAR(optimizedSimilarity(2, 0), Scalar(0), Scalar(1e-6));
	EXPECT_NEAR(optimizedSimilarity(2, 1), Scalar(0), Scalar(1e-6));
	EXPECT_NEAR(optimizedSimilarity(2, 2), Scalar(1), Scalar(1e-6));
}

TEST(NonLinearOptimizationHomographyTest, OptimizePinholeHomographyReducesDistortedReprojectionError)
{
	PinholeCamera camera(640u, 480u, Scalar(520), Scalar(515), Scalar(318), Scalar(242));
	camera.setRadialDistortion(PinholeCamera::DistortionPair(Scalar(0.015), Scalar(-0.004)));
	camera.setTangentialDistortion(PinholeCamera::DistortionPair(Scalar(0.0008), Scalar(-0.0005)));
	ASSERT_TRUE(camera.isValid());

	const Vectors2 leftPoints = imagePoints();
	const Vectors2 rightPoints = transformWithCamera(camera, homography(), leftPoints);
	const Scalar unoptimizedError = averageCameraReprojectionError(camera, perturbedHomography(), leftPoints, rightPoints);

	SquareMatrix3 optimizedHomography;
	Scalar initialError = Scalar(0);
	Scalar finalError = Scalar(0);

	const bool result = NonLinearOptimizationHomography::optimizeHomography(
		camera,
		perturbedHomography(),
		leftPoints.data(),
		rightPoints.data(),
		leftPoints.size(),
		optimizedHomography,
		50u,
		Estimator::ET_SQUARE,
		Scalar(0.001),
		Scalar(5),
		&initialError,
		&finalError);

	ASSERT_TRUE(result);
	EXPECT_LT(finalError, initialError);
	EXPECT_LT(averageCameraReprojectionError(camera, optimizedHomography, leftPoints, rightPoints), unoptimizedError * Scalar(0.05));
	expectNormalizedHomography(optimizedHomography);
}

TEST(NonLinearOptimizationHomographyTest, OptimizeCameraHomographyImprovesJointCameraModel)
{
	const PinholeCamera trueCamera(640u, 480u, Scalar(525), Scalar(518), Scalar(319), Scalar(241));
	ASSERT_TRUE(trueCamera.isValid());

	const Vectors2 leftPoints = imagePoints();
	const Vectors2 rightPoints = transformWithCamera(trueCamera, homography(), leftPoints);

	const PinholeCamera initialCamera(640u, 480u, Scalar(505), Scalar(498), Scalar(323), Scalar(238));
	const Scalar unoptimizedError = averageCameraReprojectionError(initialCamera, perturbedHomography(), leftPoints, rightPoints);

	PinholeCamera optimizedCamera;
	SquareMatrix3 optimizedHomography;
	Scalar initialError = Scalar(0);
	Scalar finalError = Scalar(0);

	const bool result = NonLinearOptimizationHomography::optimizeCameraHomography(
		initialCamera,
		perturbedHomography(),
		leftPoints.data(),
		rightPoints.data(),
		leftPoints.size(),
		optimizedCamera,
		optimizedHomography,
		80u,
		Estimator::ET_SQUARE,
		Scalar(0.001),
		Scalar(5),
		&initialError,
		&finalError);

	ASSERT_TRUE(result);
	EXPECT_TRUE(optimizedCamera.isValid());
	EXPECT_LT(finalError, initialError);
	EXPECT_LT(averageCameraReprojectionError(optimizedCamera, optimizedHomography, leftPoints, rightPoints), unoptimizedError);
	expectNormalizedHomography(optimizedHomography);
}

TEST(NonLinearOptimizationHomographyTest, OptimizeCameraHomographiesReturnsOneOptimizedHomographyPerInput)
{
	const PinholeCamera trueCamera(640u, 480u, Scalar(530), Scalar(520), Scalar(320), Scalar(240));
	ASSERT_TRUE(trueCamera.isValid());

	const Vectors2 leftPoints = imagePoints();
	const SquareMatrices3 trueHomographies = {
		homography(),
		matrixFromRows(
			Scalar(0.96), Scalar(-0.06), Scalar(-14),
			Scalar(0.07), Scalar(1.03), Scalar(11),
			Scalar(-0.00012), Scalar(0.00018), Scalar(1))};
	NonLinearOptimizationHomography::ImagePointsPairs pointPairs;
	for (const SquareMatrix3& trueHomography : trueHomographies)
	{
		pointPairs.emplace_back(leftPoints, transformWithCamera(trueCamera, trueHomography, leftPoints));
	}

	const PinholeCamera initialCamera(640u, 480u, Scalar(515), Scalar(506), Scalar(323), Scalar(237));
	const SquareMatrices3 initialHomographies = {perturbedHomography(), similarity(Scalar(0.99), Numeric::deg2rad(Scalar(3)), Scalar(-10), Scalar(8))};

	PinholeCamera optimizedCamera;
	SquareMatrices3 optimizedHomographies;
	Scalar initialError = Scalar(0);
	Scalar finalError = Scalar(0);

	const bool result = NonLinearOptimizationHomography::optimizeCameraHomographies(
		initialCamera,
		initialHomographies,
		pointPairs,
		optimizedCamera,
		optimizedHomographies,
		80u,
		Estimator::ET_SQUARE,
		Scalar(0.001),
		Scalar(5),
		&initialError,
		&finalError);

	ASSERT_TRUE(result);
	EXPECT_TRUE(optimizedCamera.isValid());
	ASSERT_EQ(optimizedHomographies.size(), initialHomographies.size());
	EXPECT_LT(finalError, initialError);

	for (size_t n = 0; n < optimizedHomographies.size(); ++n)
	{
		EXPECT_LT(averageCameraReprojectionError(optimizedCamera, optimizedHomographies[n], pointPairs[n].first, pointPairs[n].second), Scalar(0.5));
		expectNormalizedHomography(optimizedHomographies[n]);
	}
}

TEST(NonLinearOptimizationHomographyTest, OptimizeDistortionCameraHomographiesImprovesDistortionOnlyModel)
{
	PinholeCamera trueCamera(640u, 480u, Scalar(520), Scalar(510), Scalar(320), Scalar(240));
	trueCamera.setRadialDistortion(PinholeCamera::DistortionPair(Scalar(0.025), Scalar(-0.006)));
	trueCamera.setTangentialDistortion(PinholeCamera::DistortionPair(Scalar(0.001), Scalar(-0.0007)));
	ASSERT_TRUE(trueCamera.isValid());

	PinholeCamera initialCamera(640u, 480u, Scalar(520), Scalar(510), Scalar(320), Scalar(240));
	initialCamera.setRadialDistortion(PinholeCamera::DistortionPair(Scalar(0.006), Scalar(0.001)));
	initialCamera.setTangentialDistortion(PinholeCamera::DistortionPair(Scalar(0), Scalar(0)));
	ASSERT_TRUE(initialCamera.isValid());

	const Vectors2 leftPoints = imagePoints();
	const SquareMatrices3 trueHomographies = {
		homography(),
		matrixFromRows(
			Scalar(1.02), Scalar(-0.04), Scalar(9),
			Scalar(0.05), Scalar(0.98), Scalar(-15),
			Scalar(0.00014), Scalar(0.00011), Scalar(1))};
	NonLinearOptimizationHomography::ImagePointsPairs pointPairs;
	for (const SquareMatrix3& trueHomography : trueHomographies)
	{
		pointPairs.emplace_back(leftPoints, transformWithCamera(trueCamera, trueHomography, leftPoints));
	}

	PinholeCamera optimizedCamera;
	SquareMatrices3 optimizedHomographies;
	Scalar initialError = Scalar(0);
	Scalar finalError = Scalar(0);

	const bool result = NonLinearOptimizationHomography::optimizeDistortionCameraHomographies(
		initialCamera,
		trueHomographies,
		pointPairs,
		optimizedCamera,
		optimizedHomographies,
		80u,
		Estimator::ET_SQUARE,
		Scalar(0.001),
		Scalar(5),
		&initialError,
		&finalError);

	ASSERT_TRUE(result);
	EXPECT_TRUE(optimizedCamera.isValid());
	ASSERT_EQ(optimizedHomographies.size(), trueHomographies.size());
	EXPECT_LT(finalError, initialError);
	EXPECT_LT(Numeric::abs(optimizedCamera.radialDistortion().first - trueCamera.radialDistortion().first), Numeric::abs(initialCamera.radialDistortion().first - trueCamera.radialDistortion().first));

	for (size_t n = 0; n < optimizedHomographies.size(); ++n)
	{
		EXPECT_LT(averageCameraReprojectionError(optimizedCamera, optimizedHomographies[n], pointPairs[n].first, pointPairs[n].second), Scalar(0.5));
		expectNormalizedHomography(optimizedHomographies[n]);
	}
}

} // namespace
