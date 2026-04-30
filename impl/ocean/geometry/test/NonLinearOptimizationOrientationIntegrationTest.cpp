/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>

#include "ocean/geometry/NonLinearOptimizationOrientation.h"
#include "ocean/geometry/Error.h"

#include "ocean/base/Accessor.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Random.h"
#include "ocean/math/Rotation.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

#include <cmath>
#include <vector>

using namespace Ocean;
using namespace Ocean::Geometry;

namespace
{

// Helper: create a set of 3D object points arranged in a grid pattern in front of the camera
std::vector<Vector3> createObjectPoints()
{
	std::vector<Vector3> points;

	// Create a grid of points at various depths
	for (Scalar x = Scalar(-2); x <= Scalar(2); x += Scalar(1))
	{
		for (Scalar y = Scalar(-2); y <= Scalar(2); y += Scalar(1))
		{
			points.emplace_back(x, y, Scalar(8));
		}
	}

	// Add some points at different depths for better conditioning
	points.emplace_back(Scalar(0.5), Scalar(0.5), Scalar(5));
	points.emplace_back(Scalar(-0.5), Scalar(1.0), Scalar(6));
	points.emplace_back(Scalar(1.0), Scalar(-0.5), Scalar(10));
	points.emplace_back(Scalar(-1.5), Scalar(-1.0), Scalar(7));
	points.emplace_back(Scalar(0.0), Scalar(0.0), Scalar(12));

	return points;
}

// Helper: project 3D points to 2D image points using a pinhole camera and a flipped-inverted orientation
std::vector<Vector2> projectPointsIF(const PinholeCamera& camera, const Quaternion& flippedCamera_R_world, const std::vector<Vector3>& objectPoints)
{
	const HomogenousMatrix4 flippedCamera_T_world(flippedCamera_R_world);
	std::vector<Vector2> imagePoints;
	imagePoints.reserve(objectPoints.size());

	for (const auto& objectPoint : objectPoints)
	{
		const Vector2 imagePoint = camera.projectToImageIF<false>(flippedCamera_T_world, objectPoint, false);
		imagePoints.push_back(imagePoint);
	}

	return imagePoints;
}

// Helper: project 3D points using SquareMatrix3 orientation
std::vector<Vector2> projectPointsIF(const PinholeCamera& camera, const SquareMatrix3& flippedCamera_R_world, const std::vector<Vector3>& objectPoints)
{
	const HomogenousMatrix4 flippedCamera_T_world(flippedCamera_R_world);
	std::vector<Vector2> imagePoints;
	imagePoints.reserve(objectPoints.size());

	for (const auto& objectPoint : objectPoints)
	{
		const Vector2 imagePoint = camera.projectToImageIF<false>(flippedCamera_T_world, objectPoint, false);
		imagePoints.push_back(imagePoint);
	}

	return imagePoints;
}

// Components: NonLinearOptimizationOrientation + AnyCameraPinhole + Error + Jacobian
// Tests that optimizeOrientationIF converges to the true orientation from a perturbed initial guess
TEST(NonLinearOptimizationOrientationCameraIntegrationTest, OptimizeOrientationIFConvergesToTrueOrientation)
{
	// Arrange: create a pinhole camera (real component 1)
	const PinholeCamera pinholeCamera(640u, 480u, Scalar(500), Scalar(500), Scalar(320), Scalar(240));
	ASSERT_TRUE(pinholeCamera.isValid());

	// Define the true orientation: small rotation around y-axis
	const Quaternion trueOrientation(Rotation(Vector3(0, 1, 0), Numeric::deg2rad(Scalar(10))));
	ASSERT_TRUE(trueOrientation.isValid());

	// Create 3D object points
	const std::vector<Vector3> objectPoints = createObjectPoints();
	ASSERT_GE(objectPoints.size(), 3u);

	// Project points using the true orientation to get ground-truth image points
	const std::vector<Vector2> imagePoints = projectPointsIF(pinholeCamera, trueOrientation, objectPoints);
	ASSERT_EQ(imagePoints.size(), objectPoints.size());

	// Perturb the orientation: add a 5-degree rotation offset
	const Quaternion perturbedOrientation = trueOrientation * Quaternion(Rotation(Vector3(1, 0, 0), Numeric::deg2rad(Scalar(5))));
	ASSERT_TRUE(perturbedOrientation.isValid());

	// Verify the perturbation is meaningful (not identical to true)
	const Scalar perturbationAngle = trueOrientation.smallestAngle(perturbedOrientation);
	ASSERT_GT(perturbationAngle, Numeric::deg2rad(Scalar(1)));

	// Act: optimize the orientation using NonLinearOptimizationOrientation (real component 2)
	// interacting with AnyCameraPinhole camera model and Error/Jacobian computation
	Quaternion optimizedOrientation;
	Scalar initialError = Scalar(0);
	Scalar finalError = Scalar(0);

	const AnyCameraPinhole anyCameraPinhole(pinholeCamera);

	const bool result = NonLinearOptimizationOrientation::optimizeOrientationIF(
		anyCameraPinhole,
		perturbedOrientation,
		ConstArrayAccessor<Vector3>(objectPoints),
		ConstArrayAccessor<Vector2>(imagePoints),
		optimizedOrientation,
		50u,
		Estimator::ET_SQUARE,
		Scalar(0.001),
		Scalar(5),
		&initialError,
		&finalError);

	// Assert: optimization succeeded
	EXPECT_TRUE(result);

	// Assert: final error is significantly less than initial error
	EXPECT_LT(finalError, initialError);

	// Assert: final error is very small (near-zero reprojection error)
	EXPECT_LT(finalError, Scalar(0.1));

	// Assert: optimized orientation is close to the true orientation
	const Scalar angleDifference = optimizedOrientation.smallestAngle(trueOrientation);
	EXPECT_LT(angleDifference, Numeric::deg2rad(Scalar(1)));
}

// Components: NonLinearOptimizationOrientation + AnyCameraPinhole + Error + Jacobian
// Tests optimization with a larger perturbation to verify robustness
TEST(NonLinearOptimizationOrientationCameraIntegrationTest, OptimizeOrientationIFHandlesLargerPerturbation)
{
	// Arrange
	const PinholeCamera pinholeCamera(800u, 600u, Scalar(600), Scalar(600), Scalar(400), Scalar(300));
	ASSERT_TRUE(pinholeCamera.isValid());

	// True orientation: combined rotation around multiple axes
	const Quaternion trueOrientation(Rotation(Vector3(Scalar(0.5), Scalar(0.7), Scalar(0.5)).normalized(), Numeric::deg2rad(Scalar(15))));
	ASSERT_TRUE(trueOrientation.isValid());

	const std::vector<Vector3> objectPoints = createObjectPoints();
	const std::vector<Vector2> imagePoints = projectPointsIF(pinholeCamera, trueOrientation, objectPoints);

	// Larger perturbation: 10 degrees offset
	const Quaternion perturbedOrientation = trueOrientation * Quaternion(Rotation(Vector3(Scalar(0.3), Scalar(0.8), Scalar(0.5)).normalized(), Numeric::deg2rad(Scalar(10))));

	// Act
	Quaternion optimizedOrientation;
	Scalar initialError = Scalar(0);
	Scalar finalError = Scalar(0);

	const AnyCameraPinhole anyCameraPinhole(pinholeCamera);

	const bool result = NonLinearOptimizationOrientation::optimizeOrientationIF(
		anyCameraPinhole,
		perturbedOrientation,
		ConstArrayAccessor<Vector3>(objectPoints),
		ConstArrayAccessor<Vector2>(imagePoints),
		optimizedOrientation,
		100u,
		Estimator::ET_SQUARE,
		Scalar(0.001),
		Scalar(5),
		&initialError,
		&finalError);

	// Assert
	EXPECT_TRUE(result);
	EXPECT_LT(finalError, initialError);
	EXPECT_LT(finalError, Scalar(0.5));

	const Scalar angleDifference = optimizedOrientation.smallestAngle(trueOrientation);
	EXPECT_LT(angleDifference, Numeric::deg2rad(Scalar(2)));
}

// Components: NonLinearOptimizationOrientation + PinholeCamera (with intrinsics) + Error + Jacobian
// Tests optimizeCameraOrientationIF which jointly optimizes orientation AND camera intrinsics
TEST(NonLinearOptimizationOrientationCameraIntegrationTest, OptimizeCameraOrientationIFConverges)
{
	// Arrange: create a pinhole camera with known intrinsics (real component 1)
	const PinholeCamera truePinholeCamera(640u, 480u, Scalar(500), Scalar(500), Scalar(320), Scalar(240));
	ASSERT_TRUE(truePinholeCamera.isValid());

	// True orientation as SquareMatrix3
	const Rotation trueRotation(Vector3(0, 1, 0), Numeric::deg2rad(Scalar(8)));
	const SquareMatrix3 trueOrientationMatrix(trueRotation);
	ASSERT_FALSE(trueOrientationMatrix.isSingular());

	// Create 3D object points
	const std::vector<Vector3> objectPoints = createObjectPoints();

	// Project points using the true camera and orientation
	const std::vector<Vector2> imagePoints = projectPointsIF(truePinholeCamera, trueOrientationMatrix, objectPoints);
	ASSERT_EQ(imagePoints.size(), objectPoints.size());

	// Perturb the orientation
	const Rotation perturbedRotation(Vector3(Scalar(0.2), Scalar(0.9), Scalar(0.4)).normalized(), Numeric::deg2rad(Scalar(12)));
	const SquareMatrix3 perturbedOrientationMatrix(perturbedRotation);

	// Slightly perturb the camera intrinsics
	PinholeCamera perturbedCamera(640u, 480u, Scalar(510), Scalar(490), Scalar(322), Scalar(238));

	// Act: optimize both orientation and camera intrinsics (real component 2: NonLinearOptimizationOrientation)
	SquareMatrix3 optimizedOrientation;
	PinholeCamera optimizedCamera;
	Scalar initialError = Scalar(0);
	Scalar finalError = Scalar(0);

	const bool result = NonLinearOptimizationOrientation::optimizeCameraOrientationIF(
		perturbedCamera,
		perturbedOrientationMatrix,
		ConstArrayAccessor<Vector3>(objectPoints),
		ConstArrayAccessor<Vector2>(imagePoints),
		true, // distortImagePoints
		optimizedOrientation,
		optimizedCamera,
		100u,
		Estimator::ET_SQUARE,
		Scalar(0.001),
		Scalar(5),
		&initialError,
		&finalError);

	// Assert: optimization succeeded
	EXPECT_TRUE(result);

	// Assert: error decreased
	EXPECT_LT(finalError, initialError);

	// Assert: optimized orientation is closer to true than the perturbed one
	// Compare rotation matrices by computing the angle of the difference rotation
	const SquareMatrix3 diffOptimized = optimizedOrientation * trueOrientationMatrix.transposed();
	const SquareMatrix3 diffPerturbed = perturbedOrientationMatrix * trueOrientationMatrix.transposed();

	const Rotation rotDiffOptimized(diffOptimized);
	const Rotation rotDiffPerturbed(diffPerturbed);

	EXPECT_LT(rotDiffOptimized.angle(), rotDiffPerturbed.angle());
}

// Components: NonLinearOptimizationOrientation + AnyCameraPinhole + Error (with Tukey estimator)
// Tests that the Tukey robust estimator handles the optimization correctly
TEST(NonLinearOptimizationOrientationCameraIntegrationTest, OptimizeOrientationIFWithTukeyEstimator)
{
	// Arrange
	const PinholeCamera pinholeCamera(640u, 480u, Scalar(500), Scalar(500), Scalar(320), Scalar(240));

	const Quaternion trueOrientation(Rotation(Vector3(Scalar(0.3), Scalar(0.9), Scalar(0.3)).normalized(), Numeric::deg2rad(Scalar(12))));

	const std::vector<Vector3> objectPoints = createObjectPoints();
	const std::vector<Vector2> imagePoints = projectPointsIF(pinholeCamera, trueOrientation, objectPoints);

	// Small perturbation
	const Quaternion perturbedOrientation = trueOrientation * Quaternion(Rotation(Vector3(0, 0, 1), Numeric::deg2rad(Scalar(3))));

	// Act: use Tukey estimator instead of square estimator
	Quaternion optimizedOrientation;
	Scalar initialError = Scalar(0);
	Scalar finalError = Scalar(0);

	const AnyCameraPinhole anyCameraPinhole(pinholeCamera);

	const bool result = NonLinearOptimizationOrientation::optimizeOrientationIF(
		anyCameraPinhole,
		perturbedOrientation,
		ConstArrayAccessor<Vector3>(objectPoints),
		ConstArrayAccessor<Vector2>(imagePoints),
		optimizedOrientation,
		50u,
		Estimator::ET_TUKEY,
		Scalar(0.001),
		Scalar(5),
		&initialError,
		&finalError);

	// Assert
	EXPECT_TRUE(result);
	EXPECT_LT(finalError, initialError);

	const Scalar angleDifference = optimizedOrientation.smallestAngle(trueOrientation);
	EXPECT_LT(angleDifference, Numeric::deg2rad(Scalar(2)));
}

// Components: NonLinearOptimizationOrientation + AnyCameraPinhole + Error + Jacobian
// Tests that intermediate errors are tracked during optimization
TEST(NonLinearOptimizationOrientationCameraIntegrationTest, OptimizeOrientationIFTracksIntermediateErrors)
{
	// Arrange
	const PinholeCamera pinholeCamera(640u, 480u, Scalar(500), Scalar(500), Scalar(320), Scalar(240));

	const Quaternion trueOrientation(Rotation(Vector3(0, 1, 0), Numeric::deg2rad(Scalar(10))));

	const std::vector<Vector3> objectPoints = createObjectPoints();
	const std::vector<Vector2> imagePoints = projectPointsIF(pinholeCamera, trueOrientation, objectPoints);

	const Quaternion perturbedOrientation = trueOrientation * Quaternion(Rotation(Vector3(1, 0, 0), Numeric::deg2rad(Scalar(5))));

	// Act
	Quaternion optimizedOrientation;
	Scalar initialError = Scalar(0);
	Scalar finalError = Scalar(0);
	Scalars intermediateErrors;

	const AnyCameraPinhole anyCameraPinhole(pinholeCamera);

	const bool result = NonLinearOptimizationOrientation::optimizeOrientationIF(
		anyCameraPinhole,
		perturbedOrientation,
		ConstArrayAccessor<Vector3>(objectPoints),
		ConstArrayAccessor<Vector2>(imagePoints),
		optimizedOrientation,
		20u,
		Estimator::ET_SQUARE,
		Scalar(0.001),
		Scalar(5),
		&initialError,
		&finalError,
		nullptr, // no inverted covariances
		&intermediateErrors);

	// Assert: optimization succeeded
	EXPECT_TRUE(result);

	// Assert: intermediate errors were recorded
	ASSERT_FALSE(intermediateErrors.empty());

	// Assert: errors generally decrease (last should be less than first)
	EXPECT_LE(intermediateErrors.back(), intermediateErrors.front());

	// Assert: final error matches the last intermediate error
	EXPECT_LT(finalError, initialError);
}

} // namespace
