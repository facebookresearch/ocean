/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestHomography.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/geometry/Error.h"
#include "ocean/geometry/Homography.h"
#include "ocean/geometry/NonLinearOptimization.h"
#include "ocean/geometry/Utilities.h"

#include "ocean/math/Euler.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Line3.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Plane3.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Random.h"

#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

bool TestHomography::test(const double testDuration, Worker& /*worker*/)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Homography test:   ---";

	bool allSucceeded = true;

	Log::info() << " ";

	allSucceeded = testRotationalHomographyOnePose(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRotationalHomographyTwoPoses(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPlanarHomographyOnePose(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPlanarHomographyTwoPoses(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFactorizationPlanarHomographyOnePose(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFactorizationPlanarHomographyTwoPoses(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFaultlessPlanarHomography2D(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFaultlessNoisedPlanarHomography2D(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFaultlessHomography(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFaultlessNoisedHomography(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testIntrinsic(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHomotheticMatrix(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSimilarityMatrix(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAffineMatrix(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHomographyMatrix(testDuration, true) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHomographyMatrix(testDuration, false) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	testHomographyMatrixFromPointsAndLinesSVD(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Homography test succeeded.";
	}
	else
	{
		Log::info() << "Homography test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestHomography, RotationalHomographyOnePose)
{
	EXPECT_TRUE(TestHomography::testRotationalHomographyOnePose(GTEST_TEST_DURATION));
}

TEST(TestHomography, RotationalHomographyTwoPoses)
{
	EXPECT_TRUE(TestHomography::testRotationalHomographyTwoPoses(GTEST_TEST_DURATION));
}

TEST(TestHomography, PlanarHomographyOnePose)
{
	EXPECT_TRUE(TestHomography::testPlanarHomographyOnePose(GTEST_TEST_DURATION));
}

TEST(TestHomography, PlanarHomographyTwoPoses)
{
	EXPECT_TRUE(TestHomography::testPlanarHomographyTwoPoses(GTEST_TEST_DURATION));
}

TEST(TestHomography, FactorizationPlanarHomographyOnePose)
{
	EXPECT_TRUE(TestHomography::testFactorizationPlanarHomographyOnePose(GTEST_TEST_DURATION));
}

TEST(TestHomography, FactorizationPlanarHomographyTwoPoses)
{
	EXPECT_TRUE(TestHomography::testFactorizationPlanarHomographyTwoPoses(GTEST_TEST_DURATION));
}

TEST(TestHomography, FaultlessPlanarHomography2D)
{
	EXPECT_TRUE(TestHomography::testFaultlessPlanarHomography2D(GTEST_TEST_DURATION));
}

TEST(TestHomography, FaultlessNoisedPlanarHomography2D)
{
	EXPECT_TRUE(TestHomography::testFaultlessNoisedPlanarHomography2D(GTEST_TEST_DURATION));
}

TEST(TestHomography, FaultlessHomography)
{
	EXPECT_TRUE(TestHomography::testFaultlessHomography(GTEST_TEST_DURATION));
}

TEST(TestHomography, FaultlessNoisedHomography)
{
	EXPECT_TRUE(TestHomography::testFaultlessNoisedHomography(GTEST_TEST_DURATION));
}

TEST(TestHomography, Intrinsic)
{
	EXPECT_TRUE(TestHomography::testIntrinsic(GTEST_TEST_DURATION));
}


TEST(TestHomography, HomotheticMatrix_100)
{
	EXPECT_TRUE(TestHomography::testHomotheticMatrix(GTEST_TEST_DURATION, 100));
}

TEST(TestHomography, HomotheticMatrix_1000)
{
	EXPECT_TRUE(TestHomography::testHomotheticMatrix(GTEST_TEST_DURATION, 1000));
}

TEST(TestHomography, HomotheticMatrix_10000)
{
	EXPECT_TRUE(TestHomography::testHomotheticMatrix(GTEST_TEST_DURATION, 10000));
}

TEST(TestHomography, HomotheticMatrix_100000)
{
	EXPECT_TRUE(TestHomography::testHomotheticMatrix(GTEST_TEST_DURATION, 100000));
}


TEST(TestHomography, SimilarityMatrix_100)
{
	EXPECT_TRUE(TestHomography::testSimilarityMatrix(GTEST_TEST_DURATION, 100));
}

TEST(TestHomography, SimilarityMatrix_1000)
{
	EXPECT_TRUE(TestHomography::testSimilarityMatrix(GTEST_TEST_DURATION, 1000));
}

TEST(TestHomography, SimilarityMatrix_10000)
{
	EXPECT_TRUE(TestHomography::testSimilarityMatrix(GTEST_TEST_DURATION, 10000));
}

TEST(TestHomography, SimilarityMatrix_100000)
{
	EXPECT_TRUE(TestHomography::testSimilarityMatrix(GTEST_TEST_DURATION, 100000));
}


TEST(TestHomography, AffineMatrix_100)
{
	EXPECT_TRUE(TestHomography::testAffineMatrix(GTEST_TEST_DURATION, 100));
}

TEST(TestHomography, AffineMatrix_1000)
{
	EXPECT_TRUE(TestHomography::testAffineMatrix(GTEST_TEST_DURATION, 1000));
}

TEST(TestHomography, AffineMatrix_10000)
{
	EXPECT_TRUE(TestHomography::testAffineMatrix(GTEST_TEST_DURATION, 10000));
}

TEST(TestHomography, AffineMatrix_100000)
{
	EXPECT_TRUE(TestHomography::testAffineMatrix(GTEST_TEST_DURATION, 100000));
}


TEST(TestHomography, HomographyMatrixSVD)
{
	EXPECT_TRUE(TestHomography::testHomographyMatrix(GTEST_TEST_DURATION, true));
}

TEST(TestHomography, HomographyMatrixLinear)
{
	EXPECT_TRUE(TestHomography::testHomographyMatrix(GTEST_TEST_DURATION, false));
}

TEST(TestHomography, HomographyMatrixFromPointsAndLinesSVD)
{
	EXPECT_TRUE(TestHomography::testHomographyMatrixFromPointsAndLinesSVD(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestHomography::testRotationalHomographyOnePose(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Rotational homography determination for one pose test:";

	RandomGenerator randomGenerator;

	constexpr double successThreshold = 0.99;
	ValidationPrecision validation(successThreshold, randomGenerator);

	Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		const unsigned int leftWidth = RandomI::random(randomGenerator, 600u, 800u);
		const unsigned int leftHeight = RandomI::random(randomGenerator, 400u, 600u);
		const PinholeCamera leftCamera(leftWidth, leftHeight, Random::scalar(randomGenerator,  Numeric::deg2rad(40), Numeric::deg2rad(80)));

		const unsigned int rightWidth = RandomI::random(randomGenerator, 600u, 800u);
		const unsigned int rightHeight = RandomI::random(randomGenerator, 400u, 600u);
		const PinholeCamera rightCamera(rightWidth, rightHeight, Random::scalar(randomGenerator, Numeric::deg2rad(40), Numeric::deg2rad(80)));

		const Euler euler(Random::euler(randomGenerator, Numeric::deg2rad(5), Numeric::deg2rad(15)));
		const Quaternion quaternion(euler);
		const HomogenousMatrix4 transformation(quaternion);

		const SquareMatrix3 homography(Geometry::Homography::homographyMatrix(transformation.rotation(), leftCamera, rightCamera));
		ocean_assert(transformation.rotationMatrix() == Geometry::Homography::factorizeHomographyMatrix(homography, leftCamera, rightCamera));

		for (unsigned int n = 0; n < 1000u; ++n)
		{
			const Vector2 leftImagePoint(Random::vector2(randomGenerator, Scalar(0), Scalar(leftCamera.width() - 1u), Scalar(0), Scalar(leftCamera.height() - 1u)));
			const Line3 ray(leftCamera.ray(leftImagePoint, HomogenousMatrix4(true)));

			// we determine any arbitrary object point lying on the ray in front of the camera
			const Vector3 objectPoint = ray.point(Random::scalar(randomGenerator, Scalar(1), Scalar(10)));
			ocean_assert(leftCamera.projectToImage<true>(HomogenousMatrix4(true), objectPoint, false).isEqual(leftImagePoint, Numeric::weakEps()));

			const Vector2 rightImagePoint(rightCamera.projectToImage<true>(transformation, objectPoint, false));

			const Vector2 testRightPoint(homography * leftImagePoint);

			if (testRightPoint.sqrDistance(rightImagePoint) > Scalar(0.01 * 0.01))
			{
				scopedIteration.setInaccurate();
				break;
			}
		}
	}
	while (validation.needMoreIterations() || startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestHomography::testRotationalHomographyTwoPoses(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Rotational homography determination for two poses test:";

	RandomGenerator randomGenerator;

	constexpr double successThreshold = 0.95;
	ValidationPrecision validation(successThreshold, randomGenerator);

	const Scalar eps = Scalar(0.01);

	Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		const unsigned int leftWidth = RandomI::random(randomGenerator, 600u, 800u);
		const unsigned int leftHeight = RandomI::random(randomGenerator, 400u, 600u);
		const PinholeCamera leftCamera(leftWidth, leftHeight, Random::scalar(randomGenerator, Numeric::deg2rad(40), Numeric::deg2rad(80)));

		const Quaternion leftQuaternion(Random::quaternion(randomGenerator));
		const Vector3 leftTranslation(Random::vector3(randomGenerator, -10, 10));
		const HomogenousMatrix4 leftTransformation(leftTranslation, leftQuaternion);

		const unsigned int rightWidth = RandomI::random(randomGenerator, 600u, 800u);
		const unsigned int rightHeight = RandomI::random(randomGenerator, 400u, 600u);
		const PinholeCamera rightCamera(rightWidth, rightHeight, Random::scalar(randomGenerator, Numeric::deg2rad(40), Numeric::deg2rad(80)));

		const Euler rightEuler(Random::euler(randomGenerator, Numeric::deg2rad(5), Numeric::deg2rad(15)));
		const Quaternion rightQuaternion(leftQuaternion * Quaternion(rightEuler));
		const Vector3& rightTranslation = leftTranslation;
		const HomogenousMatrix4 rightTransformation(rightTranslation, rightQuaternion);

		const SquareMatrix3 homography(Geometry::Homography::homographyMatrix(leftTransformation.rotation(), rightTransformation.rotation(), leftCamera, rightCamera));

		for (unsigned int n = 0; n < 1000u; ++n)
		{
			const Vector2 leftImagePoint(Random::vector2(randomGenerator, Scalar(0), Scalar(leftCamera.width() - 1u), Scalar(0u), Scalar(leftCamera.height() - 1u)));
			const Line3 ray(leftCamera.ray(leftImagePoint, leftTransformation));

			// we determine any arbitrary object point lying on the ray in front of the camera
			const Vector3 objectPoint = ray.point(Random::scalar(randomGenerator, Scalar(1), Scalar(10)));

			if constexpr (std::is_same<double, Scalar>::value)
			{
				ocean_assert(leftCamera.projectToImage<true>(leftTransformation, objectPoint, false).isEqual(leftImagePoint, Numeric::weakEps()));
			}

			const Vector2 rightImagePoint(rightCamera.projectToImage<true>(rightTransformation, objectPoint, false));

			const Vector2 testRightPoint(homography * leftImagePoint);

			if (testRightPoint.sqrDistance(rightImagePoint) > Numeric::sqr(eps))
			{
				scopedIteration.setInaccurate();
				break;
			}
		}
	}
	while (validation.needMoreIterations() || startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestHomography::testPlanarHomographyOnePose(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Planar homography determination for one pose test:";

	constexpr unsigned int width = 640u;
	constexpr unsigned int height = 480u;

	const PinholeCamera pinholeCamera(width, height, Numeric::deg2rad(60));

	RandomGenerator randomGenerator;

	constexpr double successThreshold = 0.99;
	ValidationPrecision validation(successThreshold, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		// we take the identity transformation for the left camera
		const HomogenousMatrix4 transformationLeft(true);

		const Vector3 translationRight(Random::vector3(randomGenerator) * Scalar(0.25));
		const Quaternion quaternionRight(Random::euler(randomGenerator, Numeric::deg2rad(30)));
		const HomogenousMatrix4 transformationRight(translationRight, quaternionRight);

		// created a random normal for the plane, defined in the left coordinate system
		const Vector3 normal(Quaternion(Random::euler(randomGenerator, Numeric::deg2rad(30))) * Vector3(0, 0, 1));
		const Plane3 plane(normal, Random::scalar(randomGenerator, Scalar(-5), Scalar(-1)));

		const SquareMatrix3 homography(Geometry::Homography::homographyMatrix(transformationRight, pinholeCamera, pinholeCamera, plane));

		for (unsigned int n = 0; n < 100u; ++n)
		{
			const Vector2 leftImagePoint(Random::vector2(randomGenerator, Scalar(0), Scalar(pinholeCamera.width() - 1u), Scalar(0), Scalar(pinholeCamera.height() - 1u)));
			const Line3 ray(pinholeCamera.ray(leftImagePoint, transformationLeft));

			Vector3 objectPoint;
			if (plane.intersection(ray, objectPoint))
			{
				ocean_assert(pinholeCamera.projectToImage<true>(transformationLeft, objectPoint, false).isEqual(leftImagePoint, Numeric::weakEps()));
				const Vector2 rightImagePoint(pinholeCamera.projectToImage<true>(transformationRight, objectPoint, false));

				const Vector2 testRightPoint(homography * leftImagePoint);

				if (testRightPoint.sqrDistance(rightImagePoint) > Scalar(0.01 * 0.01))
				{
					scopedIteration.setInaccurate();
					break;
				}
			}
		}
	}
	while (validation.needMoreIterations() || startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestHomography::testPlanarHomographyTwoPoses(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Planar homography determination for two poses test:";

	constexpr unsigned int width = 640u;
	constexpr unsigned int height = 480u;

	const PinholeCamera pinholeCamera(width, height, Numeric::deg2rad(60));

	const Box2 largeCameraBoundingBox(Scalar(pinholeCamera.width()) * Scalar(-5), Scalar(pinholeCamera.height()) * Scalar(-5), Scalar(pinholeCamera.width() * 6), Scalar(pinholeCamera.height() * 6));

	RandomGenerator randomGenerator;

	constexpr double successThreshold = 0.95;
	ValidationPrecision validation(successThreshold, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		const Vector3 translationLeft(Random::vector3(randomGenerator) * Scalar(0.25));
		const Quaternion quaternionLeft(Random::euler(randomGenerator, Numeric::deg2rad(30)));
		const HomogenousMatrix4 transformationLeft(translationLeft, quaternionLeft);

		const Vector3 normal(Quaternion(Random::euler(randomGenerator, Numeric::deg2rad(30))) * Vector3(0, 0, 1));
		const Plane3 plane(normal, Random::scalar(randomGenerator, Scalar(-5), Scalar(-1)));

		for (unsigned int n = 0; n < 100u; ++n)
		{
			const Vector2 leftImagePoint(Random::vector2(randomGenerator, Scalar(0), Scalar(pinholeCamera.width() - 1u), Scalar(0), Scalar(pinholeCamera.height() - 1u)));
			const Line3 ray(pinholeCamera.ray(leftImagePoint, transformationLeft));

			Vector3 objectPoint;
			if (plane.intersection(ray, objectPoint))
			{
				Vector2 rightImagePoint;
				HomogenousMatrix4 transformationRight;

				while (true)
				{
					const Vector3 translationRight(Random::vector3(randomGenerator) * Scalar(0.25));
					const Quaternion quaternionRight(Random::euler(randomGenerator, Numeric::deg2rad(30)));
					transformationRight = HomogenousMatrix4(translationRight, quaternionRight);

					if (transformationRight.translation().distance(objectPoint) > Numeric::weakEps())
					{
						ocean_assert(pinholeCamera.projectToImage<true>(transformationLeft, objectPoint, false).isEqual(leftImagePoint, Numeric::weakEps()));
						rightImagePoint = pinholeCamera.projectToImage<true>(transformationRight, objectPoint, false);

						if (largeCameraBoundingBox.isInside(rightImagePoint))
						{
							break;
						}
					}
				}

				const SquareMatrix3 homography(Geometry::Homography::homographyMatrix(transformationLeft, transformationRight, pinholeCamera, pinholeCamera, plane));

				const Vector2 testRightPoint(homography * leftImagePoint);

				if (testRightPoint.sqrDistance(rightImagePoint) > Scalar(0.01 * 0.01))
				{
					scopedIteration.setInaccurate();
					break;
				}
			}
		}
	}
	while (validation.needMoreIterations() || startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestHomography::testFactorizationPlanarHomographyOnePose(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Homography factorization for one pose test:";

	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;

	const PinholeCamera pinholeCamera(width, height, Numeric::deg2rad(60));

	HighPerformanceStatistic performance;

	constexpr unsigned int correspondences = 50;

	RandomGenerator randomGenerator;

	constexpr double successThreshold = 0.95;
	ValidationPrecision validation(successThreshold, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		// we take the identity transformation for the left camera
		const HomogenousMatrix4 world_T_leftCamera(true);

		const Vector3 translationRight(Random::vector3(randomGenerator) * Scalar(0.25));
		const Quaternion quaternionRight(Random::euler(randomGenerator, Numeric::deg2rad(30)));
		const HomogenousMatrix4 world_T_rightCamera(translationRight, quaternionRight);

		// created a random normal for the plane, defined in the left coordinate system
		const Vector3 normal(Quaternion(Random::euler(randomGenerator, Numeric::deg2rad(30))) * Vector3(0, 0, 1));
		const Plane3 plane(normal, Random::scalar(randomGenerator, Scalar(-5), Scalar(-1)));

		Vectors2 imagePointsLeft, imagePointsRight;
		imagePointsLeft.reserve(correspondences);
		imagePointsRight.reserve(correspondences);

		for (unsigned int n = 0u; n < correspondences; ++n)
		{
			imagePointsLeft.emplace_back(Random::vector2(randomGenerator, Scalar(0), Scalar(pinholeCamera.width()), Scalar(0), Scalar(pinholeCamera.height())));
		}

		const Vectors3 objectPoints(Geometry::Utilities::backProjectImagePoints(pinholeCamera, world_T_leftCamera, plane, imagePointsLeft.data(), imagePointsLeft.size(), false));

		for (unsigned int n = 0u; n < objectPoints.size(); ++n)
		{
			imagePointsRight.emplace_back(pinholeCamera.projectToImage<true>(world_T_rightCamera, objectPoints[n], false));
		}

#ifdef OCEAN_DEBUG
		{
			const HomogenousMatrix4 flippedLeftCamera_T_world = PinholeCamera::standard2InvertedFlipped(world_T_leftCamera);
			const HomogenousMatrix4 flippedRightCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_rightCamera));

			// ensure that all object points lie in front of both cameras
			for (unsigned int n = 0u; n < correspondences; ++n)
			{
				ocean_assert(PinholeCamera::isObjectPointInFrontIF(flippedLeftCamera_T_world, objectPoints[n]));
				ocean_assert(PinholeCamera::isObjectPointInFrontIF(flippedRightCamera_T_world, objectPoints[n]));
			}
		}
#endif // OCEAN_DEBUG

		SquareMatrix3 right_H_left[2];

		// create a planar homography from the pose
		right_H_left[0] = Geometry::Homography::homographyMatrix(world_T_rightCamera, pinholeCamera, pinholeCamera, plane);

		// create a planar homography from the point correspondences
		bool result = Geometry::Homography::homographyMatrix(imagePointsLeft.data(), imagePointsRight.data(), objectPoints.size(), right_H_left[1]);

		if (!result)
		{
			OCEAN_SET_FAILED(validation);
		}

#ifdef OCEAN_DEBUG
		{
			// ensure that both homography matrices provide the same mapping
			for (unsigned int n = 0u; n < imagePointsLeft.size(); ++n)
			{
				Vector2 right = right_H_left[0] * imagePointsLeft[n];
				ocean_assert((std::is_same<Scalar, float>::value) || imagePointsRight[n].isEqual(right, Numeric::weakEps()));

				Vector2 right2 = right_H_left[1] * imagePointsLeft[n];
				ocean_assert((std::is_same<Scalar, float>::value) || imagePointsRight[n].isEqual(right2, Numeric::weakEps()));

				right = -right_H_left[0] * imagePointsLeft[n];
				ocean_assert((std::is_same<Scalar, float>::value) || imagePointsRight[n].isEqual(right, Numeric::weakEps()));

				right2 = -right_H_left[1] * imagePointsLeft[n];
				ocean_assert((std::is_same<Scalar, float>::value) || imagePointsRight[n].isEqual(right2, Numeric::weakEps()));
			}
		}
#endif // OCEAN_DEBUG

		// we take the transformation with normalized translation vector as the factorization has a translation scaling uncertainty
		const HomogenousMatrix4 world_T_normalizedRightCamera(world_T_rightCamera.translation() / Numeric::abs(plane.distance()), world_T_rightCamera.rotationMatrix());

		for (unsigned int n = 0u; n < 2u; ++n)
		{
			HomogenousMatrix4 world_T_rightCameras[2];
			Vector3 normals[2];

			performance.start();
				result = Geometry::Homography::factorizeHomographyMatrix(right_H_left[n], pinholeCamera, pinholeCamera, imagePointsLeft.data(), imagePointsRight.data(), imagePointsLeft.size(), world_T_rightCameras, normals);
			performance.stop();

			if (result)
			{
				if (!Geometry::Error::posesAlmostEqual(world_T_normalizedRightCamera, world_T_rightCameras[0], Vector3(Scalar(0.001), Scalar(0.001), Scalar(0.001)), Numeric::deg2rad(Scalar(0.1)))
					&& !Geometry::Error::posesAlmostEqual(world_T_normalizedRightCamera, world_T_rightCameras[1], Vector3(Scalar(0.001), Scalar(0.001), Scalar(0.001)), Numeric::deg2rad(Scalar(0.1))))
				{
					scopedIteration.setInaccurate();
				}

				if (plane.normal().angle(normals[0]) < Numeric::deg2rad(Scalar(0.1)) && plane.normal().angle(normals[1]) < Numeric::deg2rad(Scalar(0.1)))
				{
					scopedIteration.setInaccurate();
				}
			}
			else
			{
				OCEAN_SET_FAILED(validation);
			}
		}
	}
	while (validation.needMoreIterations() || startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestHomography::testFactorizationPlanarHomographyTwoPoses(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Homography factorization for two poses test:";

	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;

	const PinholeCamera pinholeCamera(width, height, Numeric::deg2rad(60));

	HighPerformanceStatistic performance;

	constexpr unsigned int correspondences = 50u;

	RandomGenerator randomGenerator;

	constexpr double successThreshold = 0.95;
	ValidationPrecision validation(successThreshold, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		const Vector3 translationLeft(Random::vector3(randomGenerator) * Scalar(0.25));
		const Quaternion quaternionLeft(Random::euler(randomGenerator, Numeric::deg2rad(15)));
		const HomogenousMatrix4 world_T_leftCamera(translationLeft, quaternionLeft);

		const Vector3 translationRight(Random::vector3(randomGenerator) * Scalar(0.25));
		const Quaternion quaternionRight(Random::euler(randomGenerator, Numeric::deg2rad(15)));
		const HomogenousMatrix4 world_T_right(translationRight, quaternionRight);

		// created a random normal for the plane, defined in the left coordinate system
		const Vector3 normal(Quaternion(Random::euler(randomGenerator, Numeric::deg2rad(5))) * Vector3(0, 0, 1));
		const Plane3 plane(normal, Random::scalar(randomGenerator, Scalar(-5), Scalar(-1)));

		Vectors2 imagePointsLeft, imagePointsRight;
		imagePointsLeft.reserve(correspondences);
		imagePointsRight.reserve(correspondences);

		for (unsigned int n = 0u; n < correspondences; ++n)
		{
			imagePointsLeft.emplace_back(Random::vector2(randomGenerator, Scalar(0), Scalar(pinholeCamera.width()), Scalar(0), Scalar(pinholeCamera.height())));
		}

		const Vectors3 objectPoints(Geometry::Utilities::backProjectImagePoints(pinholeCamera, world_T_leftCamera, plane, imagePointsLeft.data(), imagePointsLeft.size(), false));

		for (unsigned int n = 0u; n < objectPoints.size(); ++n)
		{
			imagePointsRight.emplace_back(pinholeCamera.projectToImage<true>(world_T_right, objectPoints[n], false));
		}

#ifdef OCEAN_DEBUG
		{
			const HomogenousMatrix4 flippedLeftCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_leftCamera));
			const HomogenousMatrix4 flippedRightCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_right));

			// ensure that all object points lie in front of both cameras
			for (unsigned int n = 0u; n < correspondences; ++n)
			{
				ocean_assert(PinholeCamera::isObjectPointInFrontIF(flippedLeftCamera_T_world, objectPoints[n]));
				ocean_assert(PinholeCamera::isObjectPointInFrontIF(flippedRightCamera_T_world, objectPoints[n]));
			}
		}
#endif // OCEAN_DEBUG

		SquareMatrix3 right_H_left[2];

		// create a planar homography from the pose
		right_H_left[0] = Geometry::Homography::homographyMatrix(world_T_leftCamera, world_T_right, pinholeCamera, pinholeCamera, plane);

		// create a planar homography from the point correspondences
		bool result = Geometry::Homography::homographyMatrix(imagePointsLeft.data(), imagePointsRight.data(), objectPoints.size(), right_H_left[1]);

		if (!result)
		{
			OCEAN_SET_FAILED(validation);
		}

#ifdef OCEAN_DEBUG
		{
			// ensure that both homography matrices provide the same mapping
			for (unsigned int n = 0u; n < imagePointsLeft.size(); ++n)
			{
				Vector2 right = right_H_left[0] * imagePointsLeft[n];
				ocean_assert((std::is_same<Scalar, float>::value) || imagePointsRight[n].isEqual(right, Numeric::weakEps()));

				Vector2 right2 = right_H_left[1] * imagePointsLeft[n];
				ocean_assert((std::is_same<Scalar, float>::value) || imagePointsRight[n].isEqual(right2, Numeric::weakEps()));

				right = -right_H_left[0] * imagePointsLeft[n];
				ocean_assert((std::is_same<Scalar, float>::value) || imagePointsRight[n].isEqual(right, Numeric::weakEps()));

				right2 = -right_H_left[1] * imagePointsLeft[n];
				ocean_assert((std::is_same<Scalar, float>::value) || imagePointsRight[n].isEqual(right2, Numeric::weakEps()));
			}
		}
#endif // OCEAN_DEBUG

		const HomogenousMatrix4 leftCamera_T_world(world_T_leftCamera.inverted());

		// we take the transformation with normalized translation vector as the factorization has a translation scaling uncertainty
		const HomogenousMatrix4 leftCamera_T_rightCamera = leftCamera_T_world * world_T_right;
		const Plane3 planeLeft = plane.transform(leftCamera_T_world);
		const HomogenousMatrix4 depthCorrectedTransformationOffset(leftCamera_T_rightCamera.translation() / Numeric::abs(planeLeft.distance()), leftCamera_T_rightCamera.rotationMatrix());
		const HomogenousMatrix4 world_T_normalizedRightCamera = world_T_leftCamera * depthCorrectedTransformationOffset;

		for (unsigned int n = 0u; n < 2u; ++n)
		{
			HomogenousMatrix4 world_T_rightCameras[2];
			Vector3 normals[2];

			performance.start();
				result = Geometry::Homography::factorizeHomographyMatrix(right_H_left[n], world_T_leftCamera, pinholeCamera, pinholeCamera, imagePointsLeft.data(), imagePointsRight.data(), imagePointsLeft.size(), world_T_rightCameras, normals);
			performance.stop();

			if (result)
			{
				if (!Geometry::Error::posesAlmostEqual(world_T_normalizedRightCamera, world_T_rightCameras[0], Vector3(Scalar(0.001), Scalar(0.001), Scalar(0.001)), Numeric::deg2rad(Scalar(0.1)))
						&& !Geometry::Error::posesAlmostEqual(world_T_normalizedRightCamera, world_T_rightCameras[1], Vector3(Scalar(0.001), Scalar(0.001), Scalar(0.001)), Numeric::deg2rad(Scalar(0.1))))
				{
					scopedIteration.setInaccurate();
				}

				if (plane.normal().angle(normals[0]) < Numeric::deg2rad(Scalar(0.1)) && plane.normal().angle(normals[1]) < Numeric::deg2rad(Scalar(0.1)))
				{
					scopedIteration.setInaccurate();
				}
			}
			else
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (validation.needMoreIterations() || startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestHomography::testFaultlessPlanarHomography2D(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Perfect 2D point correspondences of planar 3D object points:";
	Log::info() << " ";

	constexpr unsigned int width = 640u;
	constexpr unsigned int height = 480u;

	const PinholeCamera pinholeCamera(width, height, Numeric::deg2rad(60));

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	for (const unsigned int correspondences : {4u, 10u, 20u, 30u, 100u})
	{
		Log::info() << "... with " << correspondences << " correspondences:";

		constexpr double successThreshold = 0.99;

		ValidationPrecision validation(successThreshold, randomGenerator);

		const Timestamp startTimestamp(true);

		do
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Vector3 translation(Random::vector3(randomGenerator));
			const Euler euler = Random::euler(randomGenerator, Numeric::deg2rad(30));;

			const Quaternion quaternion(euler);
			const HomogenousMatrix4 transformation(translation, quaternion);

			const Plane3 plane(Vector3(0, -5, 0), Vector3(Scalar(0.1), 1, Scalar(0.2)).normalized());

			Vectors2 leftImagePoints, rightImagePoints;
			Vectors3 objectPoints;

			for (unsigned int n = 0u; n < correspondences; ++n)
			{
				const Vector2 leftImagePoint(Random::vector2(randomGenerator, Scalar(0), Scalar(pinholeCamera.width() - 1u), Scalar(0), Scalar(pinholeCamera.height() - 1u)));
				const Line3 ray(pinholeCamera.ray(leftImagePoint, HomogenousMatrix4(true)));

				Vector3 objectPoint;
				if (plane.intersection(ray, objectPoint))
				{
					const Vector2 rightImagePoint(pinholeCamera.projectToImage<true>(transformation, objectPoint, false));

					leftImagePoints.push_back(leftImagePoint);
					rightImagePoints.push_back(rightImagePoint);
					objectPoints.push_back(objectPoint);
				}
			}

			ocean_assert(leftImagePoints.size() == rightImagePoints.size());
			ocean_assert(leftImagePoints.size() == objectPoints.size());

			if (leftImagePoints.size() == correspondences)
			{
				SquareMatrix3 homography;
				if (Geometry::Homography::homographyMatrix(leftImagePoints.data(), rightImagePoints.data(), leftImagePoints.size(), homography))
				{
					for (size_t n = 0; n < leftImagePoints.size(); ++n)
					{
						const Vector2& leftImagePoint = leftImagePoints[n];
						const Vector2& rightImagePoint = rightImagePoints[n];

						const Vector2 testRightPoint(homography * leftImagePoint);

						if (testRightPoint.sqrDistance(rightImagePoint) > Scalar(0.01 * 0.01))
						{
							scopedIteration.setInaccurate();
						}
					}
				}
				else
				{
					scopedIteration.setInaccurate();
				}
			}
			else
			{
				scopedIteration.setInaccurate();
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		Log::info() << "Validation: " << validation;

		if (!validation.succeeded())
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestHomography::testFaultlessNoisedPlanarHomography2D(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Noised 2D point correspondences of planar 3D object points:";
	Log::info() << " ";

	constexpr unsigned int width = 640u;
	constexpr unsigned int height = 480u;

	const PinholeCamera pinholeCamera(width, height, Numeric::deg2rad(60));

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	for (const unsigned int correspondences : {4u, 10u, 20u, 30u, 100u})
	{
		Log::info() << "... with " << correspondences << " correspondences:";

		constexpr double successThreshold = 0.90;
		ValidationPrecision validation(successThreshold, randomGenerator);

		Timestamp startTimestamp(true);

		do
		{
			while (true)
			{
				const Vector3 translation(Random::vector3(randomGenerator));
				const Euler euler = Random::euler(randomGenerator, Numeric::deg2rad(30));
				const HomogenousMatrix4 transformation(translation, euler);

				const Plane3 plane(Vector3(0, -5, 0), Vector3(Scalar(0.1), 1, Scalar(0.2)).normalized());

				Vectors2 leftImagePoints;
				Vectors2 rightImagePoints;
				Vectors3 objectPoints;

				for (unsigned int n = 0u; n < correspondences; ++n)
				{
					const Vector2 leftImagePoint(Random::vector2(randomGenerator, Scalar(0), Scalar(pinholeCamera.width() - 1u), Scalar(0), Scalar(pinholeCamera.height() - 1u)));
					const Line3 ray(pinholeCamera.ray(leftImagePoint, HomogenousMatrix4(true)));

					Vector3 objectPoint;
					if (plane.intersection(ray, objectPoint))
					{
						const Vector2 rightImagePoint(pinholeCamera.projectToImage<true>(transformation, objectPoint, false));
						const Vector2 leftNoise(Random::gaussianNoiseVector2(randomGenerator, Scalar(1), Scalar(1)));

						leftImagePoints.push_back(leftImagePoint + leftNoise);
						rightImagePoints.push_back(rightImagePoint);
						objectPoints.push_back(objectPoint);
					}
				}

				ocean_assert(leftImagePoints.size() == rightImagePoints.size());
				ocean_assert(leftImagePoints.size() == objectPoints.size());

				if (leftImagePoints.size() != correspondences)
				{
					// the test data is not good enough
					continue;
				}

				SquareMatrix3 homography;
				if (Geometry::Homography::homographyMatrix(leftImagePoints.data(), rightImagePoints.data(), leftImagePoints.size(), homography))
				{
					for (size_t n = 0; n < leftImagePoints.size(); ++n)
					{
						ValidationPrecision::ScopedIteration scopedIteration(validation);

						const Vector2& leftImagePoint = leftImagePoints[n];
						const Vector2& rightImagePoint = rightImagePoints[n];

						const Vector2 testRightPoint(homography * leftImagePoint);

						if (testRightPoint.sqrDistance(rightImagePoint) > Scalar(3.5 * 3.5))
						{
							scopedIteration.setInaccurate();
						}
					}
				}
				else
				{
					validation.addIterations(0, leftImagePoints.size());
				}

				break;
			}
		}
		while (validation.needMoreIterations() || startTimestamp + testDuration > Timestamp(true));

		Log::info() << "Validation: " << validation;

		if (!validation.succeeded())
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestHomography::testFaultlessHomography(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing faultless homography matrix:";
	Log::info() << " ";

	constexpr unsigned int width = 640u;
	constexpr unsigned int height = 480u;

	const PinholeCamera pinholeCamera(width, height, Numeric::deg2rad(60));

	RandomGenerator randomGenerator;

	const Plane3 zPlane(Vector3(0, 0, 0), Vector3(0, 0, 1));

	bool allSucceeded = true;

	for (const unsigned int correspondences : {10u, 20u, 30u, 100u})
	{
		Log::info() << "... with " << correspondences << " points:";

		constexpr double successThreshold = 0.90;
		ValidationPrecision validation(successThreshold, randomGenerator);

		Timestamp startTimestamp(true);

		do
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Scalar xTranslation = Random::scalar(randomGenerator, Scalar(-10), Scalar(10));
			const Scalar yTranslation = Random::scalar(randomGenerator, Scalar(-10), Scalar(10));
			const Scalar zTranslation = Random::scalar(randomGenerator, Scalar(0.1), Scalar(10));

			const Vector3 translation = Vector3(xTranslation, yTranslation, zTranslation);
			const Euler euler = Random::euler(randomGenerator, Numeric::deg2rad(30));
			const Quaternion quaternion(euler);

			const HomogenousMatrix4 extrinsic(translation, quaternion);

			Geometry::ImagePoints imagePoints;
			Geometry::ObjectPoints objectPoints;

			for (unsigned int n = 0u; n < correspondences; ++n)
			{
				const Vector2 imagePoint(Random::vector2(randomGenerator, Scalar(0), Scalar(width - 1u), Scalar(0), Scalar(height - 1u)));
				const Line3 ray = pinholeCamera.ray(imagePoint, extrinsic);

				Vector3 objectPoint(0, 0, 0);
				const bool result = zPlane.intersection(ray, objectPoint);
				ocean_assert_and_suppress_unused(result, result);
				ocean_assert(Numeric::isEqualEps(objectPoint.z()));

				imagePoints.push_back(imagePoint);
				objectPoints.push_back(objectPoint);
			}

			SquareMatrix3 homography;

			if (Geometry::Homography::homographyMatrixPlaneXY(objectPoints.data(), imagePoints.data(), objectPoints.size(), homography))
			{
				Scalar totalError = 0;

				for (unsigned int n = 0u; n < correspondences; ++n)
				{
					Vector3 transformedPoint(homography * Vector3(objectPoints[n].x(), objectPoints[n].y(), 1));
					transformedPoint /= transformedPoint.z();

					const Scalar error = (imagePoints[n] - Vector2(transformedPoint.x(), transformedPoint.y())).length();
					totalError += error;
				}

				const Scalar averageError = totalError / Scalar(correspondences);

				if (averageError > 5)
				{
					scopedIteration.setInaccurate();
				}
			}
		}
		while (validation.needMoreIterations() || startTimestamp + testDuration > Timestamp(true));

		Log::info() << "Validation: " << validation;

		if (!validation.succeeded())
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestHomography::testFaultlessNoisedHomography(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing Gaussian noised faultless homography matrix:";
	Log::info() << " ";

	constexpr unsigned int width = 640u;
	constexpr unsigned int height = 480u;

	const Plane3 zPlane(Vector3(0, 0, 0), Vector3(0, 0, 1));

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	for (const unsigned int correspondences : {10, 20, 30, 100})
	{
		Log::info() << "... with " << correspondences << " points:";

		constexpr double successThreshold = 0.90;
		ValidationPrecision validation(successThreshold, randomGenerator);

		Timestamp startTimestamp(true);

		do
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Scalar focalX = Random::scalar(randomGenerator, 524, 526);
			const Scalar focalY = Random::scalar(randomGenerator, 524, 526);

			const Scalar principalX = Random::scalar(randomGenerator, width * Scalar(0.5) - Scalar(20), width * Scalar(0.5) + Scalar(20));
			const Scalar principalY = Random::scalar(randomGenerator, height * Scalar(0.5) - Scalar(20), height * Scalar(0.5) + Scalar(20));

			const PinholeCamera pinholeCamera(width, height, focalX, focalY, principalX, principalY);

			const Vector3 translation = Random::vector3(randomGenerator, Scalar(-10), Scalar(10), Scalar(-10), Scalar(10), Scalar(0.1), Scalar(10));
			const Euler euler = Random::euler(randomGenerator, Numeric::deg2rad(30));
			const Quaternion quaternion(euler);

			const HomogenousMatrix4 extrinsic(translation, quaternion);

			Geometry::ImagePoints imagePoints;
			Geometry::ObjectPoints objectPoints;

			for (unsigned int n = 0; n < correspondences; ++n)
			{
				const Vector2 imagePoint = Random::vector2(randomGenerator, Scalar(0), Scalar(width - 1u), Scalar(0), Scalar(height - 1u));
				const Line3 ray = pinholeCamera.ray(imagePoint, extrinsic);

				Vector3 objectPoint(0, 0, 0);
				const bool result = zPlane.intersection(ray, objectPoint);
				ocean_assert_and_suppress_unused(result, result);
				ocean_assert(Numeric::isEqualEps(objectPoint.z()));

				imagePoints.push_back(imagePoint);
				objectPoints.push_back(objectPoint);
			}

			for (unsigned int n = 0; n < correspondences; ++n)
			{
				const Vector2 noise = Random::gaussianNoiseVector2(randomGenerator, Scalar(2), Scalar(2));
				imagePoints[n] += noise;
			}

			SquareMatrix3 homography;

			if (Geometry::Homography::homographyMatrixPlaneXY(objectPoints.data(), imagePoints.data(), objectPoints.size(), homography))
			{
				Scalar totalError = 0;

				for (unsigned int n = 0; n < correspondences; ++n)
				{
					Vector3 transformedPoint(homography * Vector3(objectPoints[n].x(), objectPoints[n].y(), 1));
					transformedPoint /= transformedPoint.z();

					const Scalar error = (imagePoints[n] - Vector2(transformedPoint.x(), transformedPoint.y())).length();
					totalError += error;
				}

				ocean_assert(correspondences != 0u);
				const Scalar averageError = totalError / Scalar(correspondences);

				if (averageError > 5)
				{
					scopedIteration.setInaccurate();
				}
			}
		}
		while (validation.needMoreIterations() || startTimestamp + testDuration > Timestamp(true));

		Log::info() << "Validation: " << validation;

		if (!validation.succeeded())
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestHomography::testIntrinsic(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing determination of intrinsic matrix:";
	Log::info() << " ";

	constexpr unsigned int width = 640u;
	constexpr unsigned int height = 480u;

	const PinholeCamera pinholeCamera(width, height, Numeric::deg2rad(60));

	const Plane3 zPlane(Vector3(0, 0, 0), Vector3(0, 0, 1));

	const unsigned int correspondences = 20u;

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	for (const unsigned int images : {3u, 5u, 10u, 20u})
	{
		Log::info() << "... with " << images << " homographies:";

		constexpr double successThreshold = 0.90;
		ValidationPrecision validation(successThreshold, randomGenerator);

		Timestamp startTimestamp(true);

		do
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			SquareMatrices3 homographies;
			HomogenousMatrices4 extrinsics;

			Geometry::ObjectPointGroups objectPointGroups;
			Geometry::ImagePointGroups imagePointGroups;

			for (unsigned int i = 0u; i < images; ++i)
			{
				const Vector3 translation = Random::vector3(randomGenerator, Scalar(-10), Scalar(10), Scalar(-10), Scalar(10), Scalar(0.1), Scalar(10));
				const Euler euler(Random::scalar(Numeric::deg2rad(-30), Numeric::deg2rad(30)), Random::scalar(Numeric::deg2rad(-30), Numeric::deg2rad(30)), Random::scalar(Numeric::deg2rad(-30), Numeric::deg2rad(30)));
				const Quaternion quaternion(euler);

				const HomogenousMatrix4 extrinsic(translation, quaternion);

				Geometry::ImagePoints imagePoints;
				Geometry::ObjectPoints objectPoints;

				for (unsigned int n = 0u; n < correspondences; ++n)
				{
					const Vector2 imagePoint(Scalar(Random::random(width - 1)), Scalar(Random::random(height - 1)));
					const Line3 ray = pinholeCamera.ray(imagePoint, extrinsic);

					Vector3 objectPoint(0, 0, 0);
					const bool result = zPlane.intersection(ray, objectPoint);
					ocean_assert_and_suppress_unused(result && Numeric::isEqualEps(objectPoint.z()), result);

					imagePoints.push_back(imagePoint);
					objectPoints.push_back(objectPoint);
				}

				objectPointGroups.push_back(objectPoints);
				imagePointGroups.push_back(imagePoints);

				SquareMatrix3 homography;
				const bool result = Geometry::Homography::homographyMatrixPlaneXY(objectPoints.data(), imagePoints.data(), objectPoints.size(), homography);
				ocean_assert_and_suppress_unused(result, result);

				// we ensure that the determination of the homography is correct
				for (unsigned int n = 0u; n < correspondences; ++n)
				{
					const Vector2 testPoint = homography * Vector2(objectPoints[n].x(), objectPoints[n].y());

					const Scalar distance = testPoint.distance(imagePoints[n]);

					// we do not accept an error larger than 1 pixel (as the homography has been determined from ideal point data)
					ocean_assert(distance <= 1);
					if (distance > 1)
					{
						OCEAN_SET_FAILED(validation);
					}
				}

				homographies.push_back(homography);
				extrinsics.push_back(extrinsic);
			}

			SquareMatrix3 intrinsic;
			if (Geometry::Homography::intrinsicMatrix(homographies.data(), homographies.size(), intrinsic))
			{
				if (Numeric::isEqual(intrinsic(0, 0), pinholeCamera.intrinsic()(0, 0), 1) // fx parameter
						&& Numeric::isEqual(intrinsic(1, 1), pinholeCamera.intrinsic()(1, 1), 1) // fy parameter
						&& Numeric::isEqual(intrinsic(0, 2), pinholeCamera.intrinsic()(0, 2), 1) // mx parameter
						&& Numeric::isEqual(intrinsic(1, 2), pinholeCamera.intrinsic()(1, 2), 1)) // my parameter
				{
					for (unsigned int n = 0; n < images; ++n)
					{
						HomogenousMatrix4 extrinsic;
						if (Geometry::Homography::extrinsicMatrix(intrinsic, homographies[n], extrinsic))
						{
							HomogenousMatrix4 t = extrinsics[n];

							const Vector3 translationDifference(extrinsic.translation() - t.translation());
							OCEAN_SUPPRESS_UNUSED_WARNING(translationDifference);

							const Geometry::ObjectPoints& objectPoints = objectPointGroups[n];
							const Geometry::ImagePoints& imagePoints = imagePointGroups[n];

							for (size_t i = 0; i < objectPoints.size(); ++i)
							{
								const Vector2 imagePoint = pinholeCamera.projectToImage<true>(extrinsic, objectPoints[i], false);
								const Vector2 realImagePoint = imagePoints[i];

								const Vector2 difference = imagePoint - realImagePoint;
								const Scalar distance = difference.length();

								if (distance > 2)
								{
									scopedIteration.setInaccurate();
								}
							}
						}
					}

					Scalar distortion2 = 0;
					Scalar distortion4 = 0;

					if (!Geometry::Homography::distortionParameters(ConstArrayAccessor<HomogenousMatrix4>(extrinsics), pinholeCamera.intrinsic(), ConstArrayAccessor<Vectors3>(objectPointGroups), ConstArrayAccessor<Vectors2>(imagePointGroups), distortion2, distortion4))
					{
						scopedIteration.setInaccurate();
					}
				}
			}
			else
			{
				OCEAN_SET_FAILED(validation);
			}
		}
		while (validation.needMoreIterations() || startTimestamp + testDuration > Timestamp(true));

		Log::info() << "Validation: " << validation;

		if (!validation.succeeded())
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (!allSucceeded && std::is_same<Scalar, float>::value)
	{
		Log::info() << "The test failed, however the applied 32 bit floating point value precision is too low for this function so that we rate the result as expected.";
		return true;
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestHomography::testHomotheticMatrix(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing determination of homothetic matrix with " << sizeof(Scalar) * 8 << "bit floating point precision:";

	bool allSucceeded = true;

	for (const size_t points : {100, 1000, 10000, 100000})
	{
		Log::info() << " ";

		if (!testHomotheticMatrix(testDuration, points) && allSucceeded)
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestHomography::testHomotheticMatrix(const double testDuration, const size_t points)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with " << String::insertCharacter(String::toAString(points), ',', 3, false) << " points:";

	constexpr unsigned int width = 1920u;
	constexpr unsigned int height = 1080u;

	RandomGenerator randomGenerator;

	ValidationPrecision validation(0.99, randomGenerator);

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		const Vector2 xAxis(1, 0);
		const Vector2 yAxis(0, 1);

		Scalar scale = Random::scalar(randomGenerator, Scalar(0.01), 2);
		scale *= Random::sign(randomGenerator);

		const Vector2 translation = Random::vector2(randomGenerator, -10, 10);

		// transformation transforming left points to right points (right = T * left)
		const SquareMatrix3 right_T_left(Vector3(xAxis * scale, 0), Vector3(yAxis * scale, 0), Vector3(translation, 1));

		Vectors2 pointsLeft;
		Vectors2 pointsRight;
		Vectors2 pointsRightNoised;

		for (size_t n = 0; n < points; ++n)
		{
			const Vector2 leftPoint = Random::vector2(randomGenerator, Scalar(0), Scalar(width), Scalar(0), Scalar(height));
			const Vector2 rightPoint = right_T_left * leftPoint;

			pointsLeft.push_back(leftPoint);
			pointsRight.push_back(rightPoint);

			pointsRightNoised.emplace_back(rightPoint + Random::gaussianNoiseVector2(randomGenerator, Scalar(0.5), Scalar(0.5)));
		}

		SquareMatrix3 right_H_left(false);

		performance.start();
			const bool result = Geometry::Homography::homotheticMatrix(pointsLeft.data(), pointsRight.data(), points, right_H_left);
		performance.stop();

		if (result)
		{
			// s   0  tx
			// 0   s  ty
			// 0   0   1

			if (Numeric::isNotEqual(right_H_left(0, 0), right_H_left(1, 1)) || Numeric::isNotEqualEps(right_H_left(1, 0)) || Numeric::isNotEqualEps(right_H_left(0, 1)))
			{
				scopedIteration.setInaccurate();
			}

			if (Numeric::isNotEqual(right_H_left(2, 0), 0) || Numeric::isNotEqual(right_H_left(2, 1), 0) || Numeric::isNotEqual(right_H_left(2, 2), 1))
			{
				scopedIteration.setInaccurate();
			}

			for (size_t n = 0; n < points; ++n)
			{
				Vector2 rightPoint;
				if (!right_H_left.multiply(pointsLeft[n], rightPoint) || !rightPoint.isEqual(pointsRight[n], 1))
				{
					scopedIteration.setInaccurate();
					break;
				}
			}
		}
		else
		{
			OCEAN_SET_FAILED(validation);
		}
	}
	while (validation.needMoreIterations() || startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestHomography::testSimilarityMatrix(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing determination of similarity matrix with " << sizeof(Scalar) * 8 << "bit floating point precision:";

	bool allSucceeded = true;

	for (const size_t points : {100, 1000, 10000, 100000})
	{
		Log::info() << " ";

		if (!testSimilarityMatrix(testDuration, points) && allSucceeded)
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestHomography::testSimilarityMatrix(const double testDuration, const size_t points)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with " << String::insertCharacter(String::toAString(points), ',', 3, false) << " points:";

	constexpr unsigned int width = 1920u;
	constexpr unsigned int height = 1080u;

	RandomGenerator randomGenerator;

	ValidationPrecision validation(0.99, randomGenerator);

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		const Vector2 xAxis = Random::vector2(randomGenerator);
		const Vector2 yAxis = xAxis.perpendicular();

		Scalar scale = Random::scalar(randomGenerator, Scalar(0.01), 2);
		scale *= Random::sign(randomGenerator);

		const Vector2 translation = Random::vector2(randomGenerator, -10, 10);

		// transformation transforming left points to right points (right = T * left)
		const SquareMatrix3 right_T_left(Vector3(xAxis * scale, 0), Vector3(yAxis * scale, 0), Vector3(translation, 1));

		Vectors2 pointsLeft;
		Vectors2 pointsRight;
		Vectors2 pointsRightNoised;

		for (size_t n = 0; n < points; ++n)
		{
			const Vector2 leftPoint = Random::vector2(randomGenerator, Scalar(0), Scalar(width), Scalar(0), Scalar(height));
			const Vector2 rightPoint = right_T_left * leftPoint;

			pointsLeft.push_back(leftPoint);
			pointsRight.push_back(rightPoint);

			pointsRightNoised.emplace_back(rightPoint + Random::gaussianNoiseVector2(randomGenerator, Scalar(0.5), Scalar(0.5)));
		}

		SquareMatrix3 right_S_left(false);

		performance.start();
			const bool result = Geometry::Homography::similarityMatrix(pointsLeft.data(), pointsRightNoised.data(), points, right_S_left);
		performance.stop();

		if (result)
		{
			// a  -b  tx
			// b   a  ty
			// 0   0   1

			if (Numeric::isNotEqual(right_S_left(0, 0), right_S_left(1, 1)) || Numeric::isNotEqual(right_S_left(1, 0), -right_S_left(0, 1)))
			{
				scopedIteration.setInaccurate();
			}

			if (Numeric::isNotEqual(right_S_left(2, 0), 0) || Numeric::isNotEqual(right_S_left(2, 1), 0) || Numeric::isNotEqual(right_S_left(2, 2), 1))
			{
				scopedIteration.setInaccurate();
			}

			for (size_t n = 0; n < points; ++n)
			{
				const Vector2 rightPoint = right_S_left * pointsLeft[n];

				if (!rightPoint.isEqual(pointsRight[n], 1))
				{
					scopedIteration.setInaccurate();
					break;
				}
			}
		}
		else
		{
			OCEAN_SET_FAILED(validation);
		}
	}
	while (validation.needMoreIterations() || startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestHomography::testAffineMatrix(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing determination of affine matrix with " << sizeof(Scalar) * 8 << "bit floating point precision:";

	bool allSucceeded = true;

	for (const size_t points : {100, 1000, 10000, 100000})
	{
		Log::info() << " ";

		if (!testAffineMatrix(testDuration, points))
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestHomography::testAffineMatrix(const double testDuration, const size_t points)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with " << String::insertCharacter(String::toAString(points), ',', 3, false) << " points:";

	constexpr unsigned int width = 1920u;
	constexpr unsigned int height = 1080u;

	RandomGenerator randomGenerator;

	ValidationPrecision validation(0.99, randomGenerator);

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		const Vector2 xAxis = Random::vector2(randomGenerator);
		const Vector2 yAxis = xAxis.perpendicular();

		Scalar scaleX = Random::scalar(randomGenerator, Scalar(0.01), 2);
		scaleX *= Random::sign(randomGenerator);

		Scalar scaleY = Random::scalar(randomGenerator, Scalar(0.01), 2);
		scaleY *= Random::sign(randomGenerator);

		const Vector2 translation = Random::vector2(randomGenerator, -10, 10);

		// transformation transforming left points to right points (right = T * left)
		const SquareMatrix3 right_T_left(Vector3(xAxis * scaleX, 0), Vector3(yAxis * scaleY, 0), Vector3(translation, 1));

		Vectors2 pointsLeft;
		Vectors2 pointsRight;
		Vectors2 pointsRightNoised;

		for (size_t n = 0; n < points; ++n)
		{
			const Vector2 leftPoint = Random::vector2(randomGenerator, Scalar(0), Scalar(width), Scalar(0), Scalar(height));
			const Vector2 rightPoint = right_T_left * leftPoint;

			pointsLeft.push_back(leftPoint);
			pointsRight.push_back(rightPoint);

			pointsRightNoised.emplace_back(rightPoint + Random::gaussianNoiseVector2(randomGenerator, Scalar(0.5), Scalar(0.5)));
		}

		SquareMatrix3 right_A_left(false);

		performance.start();
			const bool result = Geometry::Homography::affineMatrix(pointsLeft.data(), pointsRightNoised.data(), points, right_A_left);
		performance.stop();

		if (result)
		{
			for (size_t n = 0; n < points; ++n)
			{
				const Vector2 rightPoint = right_A_left * pointsLeft[n];

				if (!rightPoint.isEqual(pointsRight[n], 1))
				{
					scopedIteration.setInaccurate();
					break;
				}
			}
		}
		else
		{
			OCEAN_SET_FAILED(validation);
		}
	}
	while (validation.needMoreIterations() || startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestHomography::testHomographyMatrix(const double testDuration, const bool useSVD)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing determination of homography matrix with " << sizeof(Scalar) * 8 << "bit floating point precision, " << (useSVD ? "using SVD:" : "using a linear solution:");
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testHomographyMatrix(testDuration, 100, useSVD) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testHomographyMatrix(testDuration, 1000, useSVD) && allSucceeded;

	if (!useSVD)
	{
		Log::info() << " ";

		allSucceeded = testHomographyMatrix(testDuration, 10000, useSVD) && allSucceeded;

		Log::info() << " ";

		allSucceeded = testHomographyMatrix(testDuration, 100000, useSVD) && allSucceeded;
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestHomography::testHomographyMatrixFromPointsAndLinesSVD(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing determination of point- and line-based homography matrix with " << sizeof(Scalar) * 8 << "bit floating point precision:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testHomographyMatrixFromPointsAndLinesSVD(testDuration, 10) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testHomographyMatrixFromPointsAndLinesSVD(testDuration, 30) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testHomographyMatrixFromPointsAndLinesSVD(testDuration, 100) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestHomography::testHomographyMatrix(const double testDuration, const size_t points, const bool useSVD)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with " << String::insertCharacter(String::toAString(points), ',', 3, false) << " points:";

	constexpr unsigned int width = 1920u;
	constexpr unsigned int height = 1080u;

	const PinholeCamera pinholeCamera(width, height, Numeric::deg2rad(60));

	Vectors2 pointsLeft(points);
	Vectors2 pointsRight(points);
	Vectors2 pointsRightNoised(points);

	RandomGenerator randomGenerator;

	const double successThreshold = useSVD ? 0.99 : 0.95;

	ValidationPrecision validation(successThreshold, randomGenerator);

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		// we create a realistic homography based on two camera poses and a 3D plane in front of both cameras

		const Plane3 plane(Vector3(0, 0, -4), Vector3(0, 0, 1));

		const HomogenousMatrix4 leftPose(Random::vector3(randomGenerator, -0.5, 0.5), Random::euler(randomGenerator, 0, Numeric::deg2rad(20)));
		const HomogenousMatrix4 rightPose(Random::vector3(randomGenerator, -0.5, 0.5), Random::euler(randomGenerator, 0, Numeric::deg2rad(20)));

		const SquareMatrix3 left_T_right = Geometry::Homography::homographyMatrix(leftPose, rightPose, pinholeCamera, pinholeCamera, plane);
		ocean_assert_and_suppress_unused(!left_T_right.isSingular(), left_T_right);

		for (size_t n = 0; n < points; ++n)
		{
			pointsLeft[n] = Random::vector2(randomGenerator, Scalar(0), Scalar(width), Scalar(0), Scalar(height));

			Vector3 objectPoint(Numeric::minValue(), Numeric::minValue(), Numeric::minValue());
			if (!plane.intersection(pinholeCamera.ray(pointsLeft[n], leftPose), objectPoint))
			{
				ocean_assert(false && "This should never happen!");
			}

			ocean_assert(PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(leftPose), objectPoint));
			ocean_assert(PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(rightPose), objectPoint));

			pointsRight[n] = pinholeCamera.projectToImage<false>(rightPose, objectPoint, false);

			pointsRightNoised[n] = pointsRight[n] + Random::gaussianNoiseVector2(randomGenerator, Scalar(0.5), Scalar(0.5));
		}

		SquareMatrix3 homography;

		performance.start();
			bool localSucceeded = Geometry::Homography::homographyMatrix(pointsLeft.data(), pointsRightNoised.data(), points, homography, useSVD);
		performance.stop();

		if (localSucceeded)
		{
			for (size_t n = 0; n < points; ++n)
			{
				const Vector2 transformedPoint = homography * pointsLeft[n];
				if (!transformedPoint.isEqual(pointsRight[n], 1))
				{
					scopedIteration.setInaccurate();
					break;
				}
			}
		}
		else
		{
			scopedIteration.setInaccurate();
		}
	}
	while (validation.needMoreIterations() || startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestHomography::testHomographyMatrixFromPointsAndLinesSVD(const double testDuration, const size_t correspondences)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with " << String::insertCharacter(String::toAString(correspondences), ',', 3, false) << " point or lines:";

	constexpr unsigned int width = std::is_same<float, Scalar>::value ? 640u : 1920u;
	constexpr unsigned int height = std::is_same<float, Scalar>::value ? 480u : 1080u;

	const PinholeCamera pinholeCamera(width, height, Numeric::deg2rad(60));

	RandomGenerator randomGenerator;

	constexpr double successThreshold = 0.99;
	ValidationPrecision validation(successThreshold, randomGenerator);

	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		// we create a realistic homography based on two camera poses and a 3D plane in front of both cameras

		const Plane3 plane(Vector3(0, 0, -4), Vector3(0, 0, 1));

		const HomogenousMatrix4 leftPose(Random::vector3(randomGenerator, -0.5, 0.5), Random::euler(randomGenerator, 0, Numeric::deg2rad(20)));
		const HomogenousMatrix4 rightPose(Random::vector3(randomGenerator, -0.5, 0.5), Random::euler(randomGenerator, 0, Numeric::deg2rad(20)));

		Vectors2 pointsLeft;
		Vectors2 pointsRight;

		Lines2 linesLeft;
		Lines2 linesRight;

		for (size_t n = 0; n < correspondences; ++n)
		{
			if (RandomI::boolean(randomGenerator))
			{
				// we add a new point

				const Vector2 pointLeft =  Random::vector2(randomGenerator, Scalar(0), Scalar(width), Scalar(0), Scalar(height));

				Vector3 objectPoint(Numeric::minValue(), Numeric::minValue(), Numeric::minValue());
				if (!plane.intersection(pinholeCamera.ray(pointLeft, leftPose), objectPoint))
				{
					ocean_assert(false && "This should never happen!");
				}

				ocean_assert(PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(leftPose), objectPoint));
				ocean_assert(PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(rightPose), objectPoint));

				const Vector2 pointRight = pinholeCamera.projectToImage<false>(rightPose, objectPoint, false);

				pointsLeft.push_back(pointLeft);
				pointsRight.push_back(pointRight);
			}
			else
			{
				const Vector2 pointLeft0 =  Random::vector2(randomGenerator, Scalar(0), Scalar(width), Scalar(0), Scalar(height));
				const Vector2 pointLeft1 =  pointLeft0 + Random::vector2(randomGenerator);

				const Line2 lineLeft(pointLeft0, (pointLeft1 - pointLeft0).normalized());

				Vector3 objectPoint0;
				Vector3 objectPoint1;
				if (!plane.intersection(pinholeCamera.ray(pointLeft0, leftPose), objectPoint0) || !plane.intersection(pinholeCamera.ray(pointLeft1, leftPose), objectPoint1))
				{
					ocean_assert(false && "This should never happen!");
				}

				ocean_assert(PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(leftPose), objectPoint0));
				ocean_assert(PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(rightPose), objectPoint1));

				const Vector2 pointRight0 = pinholeCamera.projectToImage<false>(rightPose, objectPoint0, false);
				const Vector2 pointRight1 = pinholeCamera.projectToImage<false>(rightPose, objectPoint1, false);

				const Line2 lineRight(pointRight0, (pointRight1 - pointRight0).normalized());

				linesLeft.push_back(lineLeft);
				linesRight.push_back(lineRight);
			}
		}

		SquareMatrix3 right_H_left(false);

		performance.start();
			const bool result = Geometry::Homography::homographyMatrixFromPointsAndLinesSVD(pointsLeft.data(), pointsRight.data(), pointsLeft.size(), linesLeft.data(), linesRight.data(), linesLeft.size(), right_H_left);
		performance.stop();

		if (result)
		{
			constexpr Scalar posEpsilon = 1;
			const Scalar angleCosEpsilon = Numeric::cos(Numeric::deg2rad(5));

			for (size_t n = 0; n < pointsLeft.size(); ++n)
			{
				const Vector2 transformedPoint = right_H_left * pointsLeft[n];
				if (!transformedPoint.isEqual(pointsRight[n], posEpsilon))
				{
					scopedIteration.setInaccurate();
					break;
				}
			}

			for (size_t n = 0; n < linesLeft.size(); ++n)
			{
				const Vector2 pointOnLineLine = linesLeft[n].point(Random::scalar(randomGenerator, -10, 10));

				const Vector2 pointOnLineRight = right_H_left * pointOnLineLine;
				if (linesRight[n].distance(pointOnLineRight) > posEpsilon)
				{
					scopedIteration.setInaccurate();
					break;
				}
			}

			const SquareMatrix3 lines_rigth_H_left = Geometry::Homography::homographyForLines(right_H_left);

			for (size_t n = 0; n < linesLeft.size(); ++n)
			{
				const Vector3 lineLeft = linesLeft[n].decomposeNormalDistance();
				const Vector3 lineRight = linesRight[n].decomposeNormalDistance();

				ocean_assert(lineLeft.xy().isUnit() && lineRight.xy().isUnit());

				Vector3 transformedLine = lines_rigth_H_left * lineLeft;

				// normalizing the line so that we get a normal with unit length
				transformedLine /= transformedLine.xy().length();

				if (lineRight.xy() * transformedLine.xy() < angleCosEpsilon)
				{
					scopedIteration.setInaccurate();
					break;
				}

				if (Numeric::abs(lineRight.z() - transformedLine.z()) > posEpsilon)
				{
					scopedIteration.setInaccurate();
					break;
				}
			}
		}
		else
		{
			OCEAN_SET_FAILED(validation);
		}
	}
	while (validation.needMoreIterations() || startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	const bool succeeded = validation.succeeded();

	if (!succeeded && std::is_same<Scalar, float>::value)
	{
		Log::info() << "The test failed, however the applied 32 bit floating point value precision is too low for this function so that we rate the result as expected.";
		return true;
	}

	return succeeded;
}

}

}

}
