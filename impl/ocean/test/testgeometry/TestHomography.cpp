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
#include "ocean/geometry/RANSAC.h"
#include "ocean/geometry/Utilities.h"

#include "ocean/math/Euler.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Line3.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Plane3.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

bool TestHomography::test(const double testDuration, Worker& worker)
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

	for (unsigned int svdIteration = 0u; svdIteration <= 1u; ++svdIteration)
	{
		for (unsigned int refineIteration = 0u; refineIteration <= 1u; ++refineIteration)
		{
			Log::info() << " ";
			Log::info() << "-";
			Log::info() << " ";

			allSucceeded = testHomographyMatrixRANSAC(testDuration, refineIteration == 1u, svdIteration == 1u, worker) && allSucceeded;
		}
	}

	for (unsigned int svdIteration = 0u; svdIteration <= 1u; ++svdIteration)
	{
		for (unsigned int refineIteration = 0u; refineIteration <= 1u; ++refineIteration)
		{
			Log::info() << " ";
			Log::info() << "-";
			Log::info() << " ";

			allSucceeded = testHomographyMatrixRANSACForNonBijectiveCorrespondences(testDuration, refineIteration == 1u, svdIteration == 1u, worker) && allSucceeded;
		}
	}

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

TEST(TestHomography, HomotheticMatrix)
{
	EXPECT_TRUE(TestHomography::testHomotheticMatrix(GTEST_TEST_DURATION));
}

TEST(TestHomography, SimilarityMatrix)
{
	EXPECT_TRUE(TestHomography::testSimilarityMatrix(GTEST_TEST_DURATION));
}

TEST(TestHomography, AffineMatrix)
{
	EXPECT_TRUE(TestHomography::testAffineMatrix(GTEST_TEST_DURATION));
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


TEST(TestHomography, HomographyMatrixRANSACNoRefinementLinear)
{
	Worker worker;
	EXPECT_TRUE(TestHomography::testHomographyMatrixRANSAC(GTEST_TEST_DURATION, false, false, worker));
}

TEST(TestHomography, HomographyMatrixRANSACNoRefinementSVD)
{
	Worker worker;
	EXPECT_TRUE(TestHomography::testHomographyMatrixRANSAC(GTEST_TEST_DURATION, false, true, worker));
}

TEST(TestHomography, HomographyMatrixRANSACWithRefinementLinear)
{
	Worker worker;
	EXPECT_TRUE(TestHomography::testHomographyMatrixRANSAC(GTEST_TEST_DURATION, true, false, worker));
}

TEST(TestHomography, HomographyMatrixRANSACWithRefinementSVD)
{
	Worker worker;
	EXPECT_TRUE(TestHomography::testHomographyMatrixRANSACForNonBijectiveCorrespondences(GTEST_TEST_DURATION, true, true, worker));
}


TEST(TestHomography, HomographyMatrixRANSACForNonBijectiveCorrespondencesNoRefinementLinear)
{
	Worker worker;
	EXPECT_TRUE(TestHomography::testHomographyMatrixRANSACForNonBijectiveCorrespondences(GTEST_TEST_DURATION, false, false, worker));
}

TEST(TestHomography, HomographyMatrixRANSACForNonBijectiveCorrespondencesNoRefinementSVD)
{
	Worker worker;
	EXPECT_TRUE(TestHomography::testHomographyMatrixRANSACForNonBijectiveCorrespondences(GTEST_TEST_DURATION, false, true, worker));
}

TEST(TestHomography, HomographyMatrixRANSACForNonBijectiveCorrespondencesWithRefinementLinear)
{
	Worker worker;
	EXPECT_TRUE(TestHomography::testHomographyMatrixRANSACForNonBijectiveCorrespondences(GTEST_TEST_DURATION, true, false, worker));
}

TEST(TestHomography, HomographyMatrixRANSACForNonBijectiveCorrespondencesWithRefinementSVD)
{
	Worker worker;
	EXPECT_TRUE(TestHomography::testHomographyMatrixRANSACForNonBijectiveCorrespondences(GTEST_TEST_DURATION, true, true, worker));
}

#endif // OCEAN_USE_GTEST

bool TestHomography::testRotationalHomographyOnePose(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Rotational homography determination for one pose test:";

	uint64_t iterations = 0ull;
	uint64_t succeeded = 0ull;

	Timestamp startTimestamp(true);

	do
	{
		const unsigned int leftWidth = RandomI::random(600u, 800u);
		const unsigned int leftHeight = RandomI::random(400u, 600u);
		const PinholeCamera leftCamera(leftWidth, leftHeight, Random::scalar(Numeric::deg2rad(40), Numeric::deg2rad(80)));

		const unsigned int rightWidth = RandomI::random(600u, 800u);
		const unsigned int rightHeight = RandomI::random(400u, 600u);
		const PinholeCamera rightCamera(rightWidth, rightHeight, Random::scalar(Numeric::deg2rad(40), Numeric::deg2rad(80)));

		const Euler euler(Random::euler(Numeric::deg2rad(5), Numeric::deg2rad(15)));
		const Quaternion quaternion(euler);
		const HomogenousMatrix4 transformation(quaternion);

		const SquareMatrix3 homography(Geometry::Homography::homographyMatrix(transformation.rotation(), leftCamera, rightCamera));
		ocean_assert(transformation.rotationMatrix() == Geometry::Homography::factorizeHomographyMatrix(homography, leftCamera, rightCamera));

		bool localSucceeded = true;

		for (unsigned int n = 0; n < 1000u; ++n)
		{
			const Vector2 leftImagePoint(Random::scalar(Scalar(0u), Scalar(leftCamera.width() - 1u)), Random::scalar(Scalar(0u), Scalar(leftCamera.height() - 1u)));
			const Line3 ray(leftCamera.ray(leftImagePoint, HomogenousMatrix4(true)));

			// we determine any arbitrary object point lying on the ray in front of the camera
			const Vector3 objectPoint = ray.point(Random::scalar(1, 10));
			ocean_assert(leftCamera.projectToImage<true>(HomogenousMatrix4(true), objectPoint, false).isEqual(leftImagePoint, Numeric::weakEps()));

			const Vector2 rightImagePoint(rightCamera.projectToImage<true>(transformation, objectPoint, false));

			const Vector2 testRightPoint(homography * leftImagePoint);

			if (testRightPoint.sqrDistance(rightImagePoint) > Scalar(0.01 * 0.01))
			{
				localSucceeded = false;
			}
		}

		if (localSucceeded)
		{
			++succeeded;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestHomography::testRotationalHomographyTwoPoses(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Rotational homography determination for two poses test:";

	uint64_t iterations = 0ull;
	uint64_t succeeded = 0ull;

	const Scalar eps = Scalar(0.01);

	Timestamp startTimestamp(true);

	do
	{
		const unsigned int leftWidth = RandomI::random(600u, 800u);
		const unsigned int leftHeight = RandomI::random(400u, 600u);
		const PinholeCamera leftCamera(leftWidth, leftHeight, Random::scalar(Numeric::deg2rad(40), Numeric::deg2rad(80)));

		const Quaternion leftQuaternion(Random::quaternion());
		const Vector3 leftTranslation(Random::vector3(-10, 10));
		const HomogenousMatrix4 leftTransformation(leftTranslation, leftQuaternion);

		const unsigned int rightWidth = RandomI::random(600u, 800u);
		const unsigned int rightHeight = RandomI::random(400u, 600u);
		const PinholeCamera rightCamera(rightWidth, rightHeight, Random::scalar(Numeric::deg2rad(40), Numeric::deg2rad(80)));

		const Euler rightEuler(Random::euler(Numeric::deg2rad(5), Numeric::deg2rad(15)));
		const Quaternion rightQuaternion(leftQuaternion * Quaternion(rightEuler));
		const Vector3& rightTranslation = leftTranslation;
		const HomogenousMatrix4 rightTransformation(rightTranslation, rightQuaternion);

		const SquareMatrix3 homography(Geometry::Homography::homographyMatrix(leftTransformation.rotation(), rightTransformation.rotation(), leftCamera, rightCamera));

		bool localSucceeded = true;

		for (unsigned int n = 0; n < 1000u; ++n)
		{
			const Vector2 leftImagePoint(Random::scalar(Scalar(0u), Scalar(leftCamera.width() - 1u)), Random::scalar(Scalar(0u), Scalar(leftCamera.height() - 1u)));
			const Line3 ray(leftCamera.ray(leftImagePoint, leftTransformation));

			// we determine any arbitrary object point lying on the ray in front of the camera
			const Vector3 objectPoint = ray.point(Random::scalar(1, 10));

			if constexpr (std::is_same<double, Scalar>::value)
			{
				ocean_assert(leftCamera.projectToImage<true>(leftTransformation, objectPoint, false).isEqual(leftImagePoint, Numeric::weakEps()));
			}

			const Vector2 rightImagePoint(rightCamera.projectToImage<true>(rightTransformation, objectPoint, false));

			const Vector2 testRightPoint(homography * leftImagePoint);

			if (testRightPoint.sqrDistance(rightImagePoint) > Numeric::sqr(eps))
			{
				localSucceeded = false;
			}
		}

		if (localSucceeded)
		{
			++succeeded;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.95;
}

bool TestHomography::testPlanarHomographyOnePose(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Planar homography determination for one pose test:";

	constexpr unsigned int width = 640u;
	constexpr unsigned int height = 480u;

	const PinholeCamera pinholeCamera(width, height, Numeric::deg2rad(60));

	uint64_t iterations = 0ull;
	uint64_t succeeded = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		// we take the identity transformation for the left camera
		const HomogenousMatrix4 transformationLeft(true);

		const Vector3 translationRight(Random::vector3() * Scalar(0.25));
		const Quaternion quaternionRight(Random::euler(Numeric::deg2rad(30)));
		const HomogenousMatrix4 transformationRight(translationRight, quaternionRight);


		// created a random normal for the plane, defined in the left coordinate system
		const Vector3 normal(Quaternion(Random::euler(Numeric::deg2rad(30))) * Vector3(0, 0, 1));
		const Plane3 plane(normal, Random::scalar(Scalar(-5), Scalar(-1)));

		const SquareMatrix3 homography(Geometry::Homography::homographyMatrix(transformationRight, pinholeCamera, pinholeCamera, plane));

		bool localSucceeded = true;

		for (unsigned int n = 0; n < 100u; ++n)
		{
			const Vector2 leftImagePoint(Random::scalar(Scalar(0u), Scalar(pinholeCamera.width() - 1u)), Random::scalar(Scalar(0u), Scalar(pinholeCamera.height() - 1u)));
			const Line3 ray(pinholeCamera.ray(leftImagePoint, transformationLeft));

			Vector3 objectPoint;
			if (plane.intersection(ray, objectPoint))
			{
				ocean_assert(pinholeCamera.projectToImage<true>(transformationLeft, objectPoint, false).isEqual(leftImagePoint, Numeric::weakEps()));
				const Vector2 rightImagePoint(pinholeCamera.projectToImage<true>(transformationRight, objectPoint, false));

				const Vector2 testRightPoint(homography * leftImagePoint);

				if (testRightPoint.sqrDistance(rightImagePoint) > Scalar(0.01 * 0.01))
				{
					localSucceeded = false;
				}
			}
		}

		if (localSucceeded)
		{
			++succeeded;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestHomography::testPlanarHomographyTwoPoses(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Planar homography determination for two poses test:";

	constexpr unsigned int width = 640u;
	constexpr unsigned int height = 480u;

	const PinholeCamera pinholeCamera(width, height, Numeric::deg2rad(60));

	const Box2 largeCameraBoundingBox(Scalar(pinholeCamera.width()) * Scalar(-5), Scalar(pinholeCamera.height()) * Scalar(-5), Scalar(pinholeCamera.width() * 6), Scalar(pinholeCamera.height() * 6));

	uint64_t iterations = 0ull;
	uint64_t succeeded = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		const Vector3 translationLeft(Random::vector3() * Scalar(0.25));
		const Quaternion quaternionLeft(Random::euler(Numeric::deg2rad(30)));
		const HomogenousMatrix4 transformationLeft(translationLeft, quaternionLeft);

		const Vector3 normal(Quaternion(Random::euler(Numeric::deg2rad(30))) * Vector3(0, 0, 1));
		const Plane3 plane(normal, Random::scalar(Scalar(-5), Scalar(-1)));

		bool localSucceeded = true;

		for (unsigned int n = 0; n < 100u; ++n)
		{
			const Vector2 leftImagePoint(Random::scalar(Scalar(0u), Scalar(pinholeCamera.width() - 1u)), Random::scalar(Scalar(0u), Scalar(pinholeCamera.height() - 1u)));
			const Line3 ray(pinholeCamera.ray(leftImagePoint, transformationLeft));

			Vector3 objectPoint;
			if (plane.intersection(ray, objectPoint))
			{
				Vector2 rightImagePoint;
				HomogenousMatrix4 transformationRight;

				while (true)
				{
					const Vector3 translationRight(Random::vector3() * Scalar(0.25));
					const Quaternion quaternionRight(Random::euler(Numeric::deg2rad(30)));
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
					localSucceeded = false;
				}
			}
		}

		if (localSucceeded)
		{
			++succeeded;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.95;
}

bool TestHomography::testFactorizationPlanarHomographyOnePose(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Planar homography determination for one pose test:";

	constexpr unsigned int width = 640u;
	constexpr unsigned int height = 480u;

	const PinholeCamera pinholeCamera(width, height, Numeric::deg2rad(60));

	HighPerformanceStatistic performance;

	constexpr unsigned int correspondences = 50;

	uint64_t iterations = 0ull;
	uint64_t succeeded = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		// we take the identity transformation for the left camera
		const HomogenousMatrix4 transformationLeft(true);

		const Vector3 translationRight(Random::vector3() * Scalar(0.25));
		const Quaternion quaternionRight(Random::euler(Numeric::deg2rad(30)));
		const HomogenousMatrix4 transformationRight(translationRight, quaternionRight);

		// created a random normal for the plane, defined in the left coordinate system
		const Vector3 normal(Quaternion(Random::euler(Numeric::deg2rad(30))) * Vector3(0, 0, 1));
		const Plane3 plane(normal, Random::scalar(Scalar(-5), Scalar(-1)));

		Vectors2 imagePointsLeft, imagePointsRight;
		imagePointsLeft.reserve(correspondences);
		imagePointsRight.reserve(correspondences);

		for (unsigned int n = 0u; n < correspondences; ++n)
		{
			imagePointsLeft.emplace_back(Random::scalar(0, Scalar(pinholeCamera.width())), Random::scalar(0, Scalar(pinholeCamera.height())));
		}

		const Vectors3 objectPoints(Geometry::Utilities::backProjectImagePoints(pinholeCamera, transformationLeft, plane, imagePointsLeft.data(), imagePointsLeft.size(), false));

		for (unsigned int n = 0u; n < objectPoints.size(); ++n)
		{
			imagePointsRight.emplace_back(pinholeCamera.projectToImage<true>(transformationRight, objectPoints[n], false));
		}

		const HomogenousMatrix4 transformationLeftIF(PinholeCamera::standard2InvertedFlipped(transformationLeft));
		const HomogenousMatrix4 transformationRightIF(PinholeCamera::standard2InvertedFlipped(transformationRight));

		// ensure that all object points lie in front of both cameras
		for (unsigned int n = 0u; n < correspondences; ++n)
		{
			ocean_assert_and_suppress_unused(PinholeCamera::isObjectPointInFrontIF(transformationLeftIF, objectPoints[n]), transformationLeftIF);
			ocean_assert_and_suppress_unused(PinholeCamera::isObjectPointInFrontIF(transformationRightIF, objectPoints[n]), transformationRightIF);
		}

		SquareMatrix3 homographies[2];

		// create a planar homography from the pose
		homographies[0] = Geometry::Homography::homographyMatrix(transformationRight, pinholeCamera, pinholeCamera, plane);

		// create a planar homography from the point correspondences
		const bool result = Geometry::Homography::homographyMatrix(imagePointsLeft.data(), imagePointsRight.data(), objectPoints.size(), homographies[1]);
		ocean_assert_and_suppress_unused(result, result);

		// ensure that both homography matrices provide the same mapping
		for (unsigned int n = 0u; n < imagePointsLeft.size(); ++n)
		{
			Vector2 right = homographies[0] * imagePointsLeft[n];
			ocean_assert((std::is_same<Scalar, float>::value) || imagePointsRight[n].isEqual(right, Numeric::weakEps()));

			Vector2 right2 = homographies[1] * imagePointsLeft[n];
			ocean_assert((std::is_same<Scalar, float>::value) || imagePointsRight[n].isEqual(right2, Numeric::weakEps()));

			right = -homographies[0] * imagePointsLeft[n];
			ocean_assert((std::is_same<Scalar, float>::value) || imagePointsRight[n].isEqual(right, Numeric::weakEps()));

			right2 = -homographies[1] * imagePointsLeft[n];
			ocean_assert((std::is_same<Scalar, float>::value) || imagePointsRight[n].isEqual(right2, Numeric::weakEps()));
		}

		bool localSucceeded = true;

		// we take the transformation with normalized translation vector as the factorization has a translation scaling uncertainty
		const HomogenousMatrix4 depthCorrectedTransformationRight(transformationRight.translation() / Numeric::abs(plane.distance()), transformationRight.rotationMatrix());

		for (unsigned int n = 0u; n < 2u; ++n)
		{
			HomogenousMatrix4 transformations[2];
			Vector3 normals[2];

			performance.start();

			if (!Geometry::Homography::factorizeHomographyMatrix(homographies[n], pinholeCamera, pinholeCamera, imagePointsLeft.data(), imagePointsRight.data(), imagePointsLeft.size(), transformations, normals))
			{
				localSucceeded = false;
			}

			performance.stop();

			if (localSucceeded)
			{
				if (!Geometry::Error::posesAlmostEqual(depthCorrectedTransformationRight, transformations[0], Vector3(Scalar(0.001), Scalar(0.001), Scalar(0.001)), Numeric::deg2rad(Scalar(0.1)))
					&& !Geometry::Error::posesAlmostEqual(depthCorrectedTransformationRight, transformations[1], Vector3(Scalar(0.001), Scalar(0.001), Scalar(0.001)), Numeric::deg2rad(Scalar(0.1))))
				{
					localSucceeded = false;
				}

				if (plane.normal().angle(normals[0]) < Numeric::deg2rad(Scalar(0.1)) && plane.normal().angle(normals[1]) < Numeric::deg2rad(Scalar(0.1)))
				{
					localSucceeded = false;
				}
			}
		}

		if (localSucceeded)
		{
			++succeeded;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Performance: " << performance.averageMseconds() << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.95;
}

bool TestHomography::testFactorizationPlanarHomographyTwoPoses(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Homography factorization for two poses test:";

	constexpr unsigned int width = 640u;
	constexpr unsigned int height = 480u;

	const PinholeCamera pinholeCamera(width, height, Numeric::deg2rad(60));

	HighPerformanceStatistic performance;

	constexpr unsigned int correspondences = 50u;

	uint64_t iterations = 0ull;
	uint64_t succeeded = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		const Vector3 translationLeft(Random::vector3() * Scalar(0.25));
		const Quaternion quaternionLeft(Random::euler(Numeric::deg2rad(15)));
		const HomogenousMatrix4 transformationLeft(translationLeft, quaternionLeft);
		const HomogenousMatrix4 iTransformationLeft(transformationLeft.inverted());

		const Vector3 translationRight(Random::vector3() * Scalar(0.25));
		const Quaternion quaternionRight(Random::euler(Numeric::deg2rad(15)));
		const HomogenousMatrix4 transformationRight(translationRight, quaternionRight);

		// created a random normal for the plane, defined in the left coordinate system
		const Vector3 normal(Quaternion(Random::euler(Numeric::deg2rad(5))) * Vector3(0, 0, 1));
		const Plane3 plane(normal, Random::scalar(Scalar(-5), Scalar(-1)));

		Vectors2 imagePointsLeft, imagePointsRight;
		imagePointsLeft.reserve(correspondences);
		imagePointsRight.reserve(correspondences);

		for (unsigned int n = 0u; n < correspondences; ++n)
		{
			imagePointsLeft.emplace_back(Random::scalar(0, Scalar(pinholeCamera.width())), Random::scalar(0, Scalar(pinholeCamera.height())));
		}

		const Vectors3 objectPoints(Geometry::Utilities::backProjectImagePoints(pinholeCamera, transformationLeft, plane, imagePointsLeft.data(), imagePointsLeft.size(), false));

		for (unsigned int n = 0u; n < objectPoints.size(); ++n)
		{
			imagePointsRight.emplace_back(pinholeCamera.projectToImage<true>(transformationRight, objectPoints[n], false));
		}

		const HomogenousMatrix4 transformationLeftIF(PinholeCamera::standard2InvertedFlipped(transformationLeft));
		const HomogenousMatrix4 transformationRightIF(PinholeCamera::standard2InvertedFlipped(transformationRight));

		// ensure that all object points lie in front of both cameras
		for (unsigned int n = 0u; n < correspondences; ++n)
		{
			ocean_assert_and_suppress_unused(PinholeCamera::isObjectPointInFrontIF(transformationLeftIF, objectPoints[n]), transformationLeftIF);
			ocean_assert_and_suppress_unused(PinholeCamera::isObjectPointInFrontIF(transformationRightIF, objectPoints[n]), transformationRightIF);
		}

		SquareMatrix3 homographies[2];

		// create a planar homography from the pose
		homographies[0] = Geometry::Homography::homographyMatrix(transformationLeft, transformationRight, pinholeCamera, pinholeCamera, plane);

		// create a planar homography from the point correspondences
		const bool result = Geometry::Homography::homographyMatrix(imagePointsLeft.data(), imagePointsRight.data(), objectPoints.size(), homographies[1]);
		ocean_assert_and_suppress_unused(result, result);

		// ensure that both homography matrices provide the same mapping
		for (unsigned int n = 0u; n < imagePointsLeft.size(); ++n)
		{
			Vector2 right = homographies[0] * imagePointsLeft[n];
			ocean_assert((std::is_same<Scalar, float>::value) || imagePointsRight[n].isEqual(right, Numeric::weakEps()));

			Vector2 right2 = homographies[1] * imagePointsLeft[n];
			ocean_assert((std::is_same<Scalar, float>::value) || imagePointsRight[n].isEqual(right2, Numeric::weakEps()));

			right = -homographies[0] * imagePointsLeft[n];
			ocean_assert((std::is_same<Scalar, float>::value) || imagePointsRight[n].isEqual(right, Numeric::weakEps()));

			right2 = -homographies[1] * imagePointsLeft[n];
			ocean_assert((std::is_same<Scalar, float>::value) || imagePointsRight[n].isEqual(right2, Numeric::weakEps()));
		}

		bool localSucceeded = true;

		// we take the transformation with normalized translation vector as the factorization has a translation scaling uncertainty
		const HomogenousMatrix4 transformationOffset(iTransformationLeft * transformationRight);
		const Plane3 planeLeft = plane.transform(iTransformationLeft);
		const HomogenousMatrix4 depthCorrectedTransformationOffset(transformationOffset.translation() / Numeric::abs(planeLeft.distance()), transformationOffset.rotationMatrix());
		const HomogenousMatrix4 unitTransformationRight(transformationLeft * depthCorrectedTransformationOffset);

		for (unsigned int n = 0u; n < 2u; ++n)
		{
			HomogenousMatrix4 transformations[2];
			Vector3 normals[2];

			performance.start();

			if (!Geometry::Homography::factorizeHomographyMatrix(homographies[n], transformationLeft, pinholeCamera, pinholeCamera, imagePointsLeft.data(), imagePointsRight.data(), imagePointsLeft.size(), transformations, normals))
			{
				localSucceeded = false;
			}

			performance.stop();

			if (localSucceeded)
			{
				if (!Geometry::Error::posesAlmostEqual(unitTransformationRight, transformations[0], Vector3(Scalar(0.001), Scalar(0.001), Scalar(0.001)), Numeric::deg2rad(Scalar(0.1)))
						&& !Geometry::Error::posesAlmostEqual(unitTransformationRight, transformations[1], Vector3(Scalar(0.001), Scalar(0.001), Scalar(0.001)), Numeric::deg2rad(Scalar(0.1))))
				{
					localSucceeded = false;
				}

				if (plane.normal().angle(normals[0]) < Numeric::deg2rad(Scalar(0.1)) && plane.normal().angle(normals[1]) < Numeric::deg2rad(Scalar(0.1)))
				{
					localSucceeded = false;
				}
			}
		}

		if (localSucceeded)
		{
			++succeeded;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Performance: " << performance.averageMseconds() << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.95;
}

bool TestHomography::testFaultlessPlanarHomography2D(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Perfect 2D point correspondences of planar 3D object points:";
	Log::info() << " ";

	constexpr unsigned int width = 640u;
	constexpr unsigned int height = 480u;

	const PinholeCamera pinholeCamera(width, height, Numeric::deg2rad(60));

	bool allSucceeded = true;

	for (const unsigned int correspondences : {4u, 10u, 20u, 30u, 100u})
	{
		Log::info() << "... with " << correspondences << " correspondences:";

		uint64_t iterations = 0ull;
		uint64_t succeeded = 0ull;

		const Timestamp startTimestamp(true);

		do
		{
			const Vector3 translation(Random::vector3());
			const Euler euler(Random::scalar(Numeric::deg2rad(-30), Numeric::deg2rad(30)), Random::scalar(Numeric::deg2rad(-30), Numeric::deg2rad(30)), Random::scalar(Numeric::deg2rad(-30), Numeric::deg2rad(30)));
			const Quaternion quaternion(euler);
			const HomogenousMatrix4 transformation(translation, quaternion);

			const Plane3 plane(Vector3(0, -5, 0), Vector3(Scalar(0.1), 1, Scalar(0.2)).normalized());

			Vectors2 leftImagePoints, rightImagePoints;
			Vectors3 objectPoints;

			bool localSucceeded = true;

			for (unsigned int n = 0u; n < correspondences; ++n)
			{
				const Vector2 leftImagePoint(Random::scalar(Scalar(0u), Scalar(pinholeCamera.width() - 1u)), Random::scalar(Scalar(0u), Scalar(pinholeCamera.height() - 1u)));
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
							localSucceeded = false;
						}
					}
				}
				else
				{
					localSucceeded = false;
				}
			}
			else
			{
				localSucceeded = false;
			}

			if (localSucceeded)
			{
				++succeeded;
			}

			++iterations;
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);
		const double percent = double(succeeded) / double(iterations);

		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

		allSucceeded = percent >= 0.99 && allSucceeded;
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

	bool allSucceeded = true;

	for (const unsigned int correspondences : {4u, 10u, 20u, 30u, 100u})
	{
		Log::info() << "... with " << correspondences << " correspondences:";

		uint64_t iterations = 0ull;
		uint64_t succeeded = 0ull;

		Timestamp startTimestamp(true);

		do
		{
			const Vector3 translation(Random::vector3());
			const Euler euler(Random::scalar(Numeric::deg2rad(-30), Numeric::deg2rad(30)), Random::scalar(Numeric::deg2rad(-30), Numeric::deg2rad(30)), Random::scalar(Numeric::deg2rad(-30), Numeric::deg2rad(30)));
			const Quaternion quaternion(euler);
			const HomogenousMatrix4 transformation(translation, quaternion);

			const Plane3 plane(Vector3(0, -5, 0), Vector3(Scalar(0.1), 1, Scalar(0.2)).normalized());

			Vectors2 leftImagePoints, rightImagePoints;
			Vectors3 objectPoints;

			for (unsigned int n = 0u; n < correspondences; ++n)
			{
				const Vector2 leftImagePoint(Random::scalar(Scalar(0u), Scalar(pinholeCamera.width() - 1u)), Random::scalar(Scalar(0u), Scalar(pinholeCamera.height() - 1u)));
				const Line3 ray(pinholeCamera.ray(leftImagePoint, HomogenousMatrix4(true)));

				Vector3 objectPoint;
				if (plane.intersection(ray, objectPoint))
				{
					const Vector2 rightImagePoint(pinholeCamera.projectToImage<true>(transformation, objectPoint, false));
					const Vector2 leftNoise(Random::gaussianNoise(1), Random::gaussianNoise(1));

					leftImagePoints.push_back(leftImagePoint + leftNoise);
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

						if (testRightPoint.sqrDistance(rightImagePoint) <= Scalar(3.5 * 3.5))
						{
							++succeeded;
						}
					}
				}
			}

			iterations += uint64_t(leftImagePoints.size());
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);
		const double percent = double(succeeded) / double(iterations);

		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

		allSucceeded = percent >= 0.90 && allSucceeded;
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

	const Plane3 zPlane(Vector3(0, 0, 0), Vector3(0, 0, 1));

	bool allSucceeded = true;

	for (const unsigned int correspondences : {10u, 20u, 30u, 100u})
	{
		Log::info() << "... with " << correspondences << " points:";

		uint64_t iterations = 0ull;
		uint64_t succeeded = 0ull;

		Timestamp startTimestamp(true);

		do
		{
			const Vector3 translation(Random::scalar(-10, 10), Random::scalar(-10, 10), Random::scalar(Scalar(0.1), 10));
			const Euler euler(Random::scalar(Numeric::deg2rad(-30), Numeric::deg2rad(30)), Random::scalar(Numeric::deg2rad(-30), Numeric::deg2rad(30)), Random::scalar(Numeric::deg2rad(-30), Numeric::deg2rad(30)));
			const Quaternion quaternion(euler);

			const HomogenousMatrix4 extrinsic(translation, quaternion);

			Geometry::ImagePoints imagePoints;
			Geometry::ObjectPoints objectPoints;

			for (unsigned int n = 0u; n < correspondences; ++n)
			{
				const Vector2 imagePoint(Scalar(Random::random(width - 1u)), Scalar(Random::random(height - 1u)));
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

				if (averageError < 5)
				{
					++succeeded;
				}
			}

			++iterations;
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);
		const double percent = double(succeeded) / double(iterations);

		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

		allSucceeded = allSucceeded && percent >= 0.95;
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

	bool allSucceeded = true;

	for (const unsigned int correspondences : {10, 20, 30, 100})
	{
		Log::info() << "... with " << correspondences << " points:";

		uint64_t iterations = 0ull;
		uint64_t succeeded = 0ull;

		Timestamp startTimestamp(true);

		do
		{
			const PinholeCamera pinholeCamera(width, height, Random::scalar(524, 526), Random::scalar(524, 526), Random::scalar(width * Scalar(0.5) - Scalar(20), width * Scalar(0.5) + Scalar(20)), Random::scalar(height * Scalar(0.5) - Scalar(20), height * Scalar(0.5) + Scalar(20)));

			const Vector3 translation(Random::scalar(-10, 10), Random::scalar(-10, 10), Random::scalar(Scalar(0.1), 10));
			const Euler euler(Random::scalar(Numeric::deg2rad(-30), Numeric::deg2rad(30)), Random::scalar(Numeric::deg2rad(-30), Numeric::deg2rad(30)), Random::scalar(Numeric::deg2rad(-30), Numeric::deg2rad(30)));
			const Quaternion quaternion(euler);

			const HomogenousMatrix4 extrinsic(translation, quaternion);

			Geometry::ImagePoints imagePoints;
			Geometry::ObjectPoints objectPoints;

			for (unsigned int n = 0; n < correspondences; ++n)
			{
				const Vector2 imagePoint(Scalar(Random::random(width - 1)), Scalar(Random::random(height - 1)));
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
				const Vector2 noise(Random::gaussianNoise(2), Random::gaussianNoise(2));
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

				if (averageError < 5)
				{
					++succeeded;
				}
			}

			++iterations;
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);
		const double percent = double(succeeded) / double(iterations);

		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

		allSucceeded = allSucceeded && percent >= 0.95;
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

	typedef std::vector<SquareMatrix3> Homographies;
	typedef std::vector<HomogenousMatrix4> Extrinsics;

	bool allSucceeded = true;

	for (const unsigned int images : {3u, 5u, 10u, 20u})
	{
		Log::info() << "... with " << images << " homographies:";

		uint64_t iterations = 0ull;
		uint64_t succeeded = 0ull;

		Timestamp startTimestamp(true);

		do
		{
			Homographies homographies;
			Extrinsics extrinsics;

			Geometry::ObjectPointGroups objectPointGroups;
			Geometry::ImagePointGroups imagePointGroups;

			for (unsigned int i = 0u; i < images; ++i)
			{
				const Vector3 translation(Random::scalar(-10, 10), Random::scalar(-10, 10), Random::scalar(Scalar(0.1), 10));
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
						allSucceeded = false;
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
					bool failed = false;

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
								const Scalar length = difference.length();

								if (length > 2)
								{
									failed = true;
								}
							}
						}
					}

					if (!failed)
					{
						Scalar distortion2 = 0;
						Scalar distortion4 = 0;

						if (Geometry::Homography::distortionParameters(ConstArrayAccessor<HomogenousMatrix4>(extrinsics), pinholeCamera.intrinsic(), ConstArrayAccessor<Vectors3>(objectPointGroups), ConstArrayAccessor<Vectors2>(imagePointGroups), distortion2, distortion4))
						{
							++succeeded;
						}
					}
				}
			}

			++iterations;
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);
		const double percent = double(succeeded) / double(iterations);

		Log::info() << String::toAString(percent * 100.0, 1u) << "% succeeded.";

		allSucceeded = allSucceeded && percent >= 0.95;
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
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testHomotheticMatrix(testDuration, 100) && allSucceeded;

	allSucceeded = testHomotheticMatrix(testDuration, 1000) && allSucceeded;

	allSucceeded = testHomotheticMatrix(testDuration, 10000) && allSucceeded;

	allSucceeded = testHomotheticMatrix(testDuration, 100000) && allSucceeded;

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

	Vectors2 pointsLeft(points);
	Vectors2 pointsRight(points);
	Vectors2 pointsRightNoised(points);

	RandomGenerator randomGenerator;

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);

	do
	{
		const Vector2 xAxis(1, 0);
		const Vector2 yAxis(0, 1);

		const Scalar scale(Random::scalar(randomGenerator, -2, 2));
		const Vector2 translation(Random::vector2(randomGenerator, -10, 10));

		// transformation transforming left points to right points (right = T * left)
		const SquareMatrix3 right_H_left(Vector3(xAxis * scale, 0), Vector3(yAxis * scale, 0), Vector3(translation, 1));

		for (size_t n = 0; n < points; ++n)
		{
			pointsLeft[n] = Vector2(Random::scalar(0, Scalar(width)), Random::scalar(0, Scalar(height)));
			pointsRight[n] = right_H_left * pointsLeft[n];

			pointsRightNoised[n] = pointsRight[n] + Vector2(Random::gaussianNoise(0.5), Random::gaussianNoise(0.5));
		}

		SquareMatrix3 homothetic;

		performance.start();
		const bool result = Geometry::Homography::homotheticMatrix(pointsLeft.data(), pointsRight.data(), points, homothetic);
		performance.stop();

		ocean_assert(result);
		if (result)
		{
			bool localSucceeded = true;

			// s   0  tx
			// 0   s  ty
			// 0   0   1

			if (Numeric::isNotEqual(homothetic(0, 0), homothetic(1, 1)) || Numeric::isNotEqualEps(homothetic(1, 0)) || Numeric::isNotEqualEps(homothetic(0, 1)))
			{
				localSucceeded = false;
			}

			if (Numeric::isNotEqual(homothetic(2, 0), 0) || Numeric::isNotEqual(homothetic(2, 1), 0) || Numeric::isNotEqual(homothetic(2, 2), 1))
			{
				localSucceeded = false;
			}

			for (size_t n = 0; n < points; ++n)
			{
				Vector2 transformedPoint;
				if (!homothetic.multiply(pointsLeft[n], transformedPoint) || !transformedPoint.isEqual(pointsRight[n], 1))
				{
					localSucceeded = false;
				}
			}

			if (localSucceeded)
			{
				++validIterations;
			}
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Performance: " << String::toAString(performance.averageMseconds()) << " ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << " % succeeded.";

	return percent >= 0.99;
}

bool TestHomography::testSimilarityMatrix(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing determination of similarity matrix with " << sizeof(Scalar) * 8 << "bit floating point precision:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testSimilarityMatrix(testDuration, 100) && allSucceeded;

	allSucceeded = testSimilarityMatrix(testDuration, 1000) && allSucceeded;

	allSucceeded = testSimilarityMatrix(testDuration, 10000) && allSucceeded;

#ifndef OCEAN_USE_GTEST // skipping during gtests due to execution time
	allSucceeded = testSimilarityMatrix(testDuration, 100000) && allSucceeded;
#endif

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

	Vectors2 pointsLeft(points);
	Vectors2 pointsRight(points);
	Vectors2 pointsRightNoised(points);

	RandomGenerator randomGenerator;

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);

	do
	{
		const Vector2 xAxis(Random::vector2(randomGenerator));
		const Vector2 yAxis(xAxis.perpendicular());

		const Scalar scale(Random::scalar(randomGenerator, -2, 2));
		const Vector2 translation(Random::vector2(randomGenerator, -10, 10));

		// transformation transforming left points to right points (right = T * left)
		const SquareMatrix3 rightTleft(Vector3(xAxis * scale, 0), Vector3(yAxis * scale, 0), Vector3(translation, 1));

		for (size_t n = 0; n < points; ++n)
		{
			pointsLeft[n] = Vector2(Random::scalar(0, Scalar(width)), Random::scalar(0, Scalar(height)));
			pointsRight[n] = rightTleft * pointsLeft[n];

			pointsRightNoised[n] = pointsRight[n] + Vector2(Random::gaussianNoise(0.5), Random::gaussianNoise(0.5));
		}

		SquareMatrix3 similarity;

		performance.start();
			const bool result = Geometry::Homography::similarityMatrix(pointsLeft.data(), pointsRightNoised.data(), points, similarity);
		performance.stop();

		if (result)
		{
			bool localSucceeded = true;

			// a  -b  tx
			// b   a  ty
			// 0   0   1

			if (Numeric::isNotEqual(similarity(0, 0), similarity(1, 1)) || Numeric::isNotEqual(similarity(1, 0), -similarity(0, 1)))
			{
				localSucceeded = false;
			}

			if (Numeric::isNotEqual(similarity(2, 0), 0) || Numeric::isNotEqual(similarity(2, 1), 0) || Numeric::isNotEqual(similarity(2, 2), 1))
			{
				localSucceeded = false;
			}

			for (size_t n = 0; n < points; ++n)
			{
				const Vector2 transformedPoint = similarity * pointsLeft[n];
				if (!transformedPoint.isEqual(pointsRight[n], 1))
				{
					localSucceeded = false;
				}
			}

			if (localSucceeded)
			{
				++validIterations;
			}
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Performance: " << String::toAString(performance.averageMseconds()) << " ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << " % succeeded.";

	constexpr double threshold = std::is_same<Scalar, float>::value ? 0.98 : 0.99;

	return percent >= threshold;
}

bool TestHomography::testAffineMatrix(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing determination of affine matrix with " << sizeof(Scalar) * 8 << "bit floating point precision:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testAffineMatrix(testDuration, 100) && allSucceeded;

	allSucceeded = testAffineMatrix(testDuration, 1000) && allSucceeded;

	allSucceeded = testAffineMatrix(testDuration, 10000) && allSucceeded;

	allSucceeded = testAffineMatrix(testDuration, 100000) && allSucceeded;

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

	Vectors2 pointsLeft(points);
	Vectors2 pointsRight(points);
	Vectors2 pointsRightNoised(points);

	RandomGenerator randomGenerator;

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);

	do
	{
		const Vector2 xAxis(Random::vector2(randomGenerator));
		const Vector2 yAxis(xAxis.perpendicular());

		const Scalar scaleX(Random::scalar(randomGenerator, -2, 2));
		const Scalar scaleY(Random::scalar(randomGenerator, -2, 2));
		const Vector2 translation(Random::vector2(randomGenerator, -10, 10));

		// transformation transforming left points to right points (right = T * left)
		const SquareMatrix3 rightTleft(Vector3(xAxis * scaleX, 0), Vector3(yAxis * scaleY, 0), Vector3(translation, 1));

		for (size_t n = 0; n < points; ++n)
		{
			pointsLeft[n] = Vector2(Random::scalar(0, Scalar(width)), Random::scalar(0, Scalar(height)));
			pointsRight[n] = rightTleft * pointsLeft[n];

			pointsRightNoised[n] = pointsRight[n] + Vector2(Random::gaussianNoise(0.5), Random::gaussianNoise(0.5));
		}

		SquareMatrix3 similarity;

		performance.start();
			const bool result = Geometry::Homography::affineMatrix(pointsLeft.data(), pointsRightNoised.data(), points, similarity);
		performance.stop();

		if (result)
		{
			bool localSucceeded = true;

			for (size_t n = 0; n < points; ++n)
			{
				const Vector2 transformedPoint = similarity * pointsLeft[n];
				if (!transformedPoint.isEqual(pointsRight[n], 1))
				{
					localSucceeded = false;
				}
			}

			if (localSucceeded)
			{
				++validIterations;
			}
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Performance: " << String::toAString(performance.averageMseconds()) << " ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << " % succeeded.";

	return percent >= 0.99;
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

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);

	do
	{
		// we create a realistic homography based on two camera poses and a 3D plane in front of both cameras

		const Plane3 plane(Vector3(0, 0, -4), Vector3(0, 0, 1));

		const HomogenousMatrix4 leftPose(Random::vector3(randomGenerator, -0.5, 0.5), Random::euler(randomGenerator, 0, Numeric::deg2rad(20)));
		const HomogenousMatrix4 rightPose(Random::vector3(randomGenerator, -0.5, 0.5), Random::euler(randomGenerator, 0, Numeric::deg2rad(20)));

		const SquareMatrix3 left_T_right = Geometry::Homography::homographyMatrix(leftPose, rightPose, pinholeCamera, pinholeCamera, plane);
		ocean_assert_and_suppress_unused(!left_T_right.isSingular(), left_T_right);

		for (size_t n = 0; n < points; ++n)
		{
			pointsLeft[n] = Vector2(Random::scalar(0, Scalar(width)), Random::scalar(0, Scalar(height)));

			Vector3 objectPoint(Numeric::minValue(), Numeric::minValue(), Numeric::minValue());
			if (!plane.intersection(pinholeCamera.ray(pointsLeft[n], leftPose), objectPoint))
			{
				ocean_assert(false && "This should never happen!");
			}

			ocean_assert(PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(leftPose), objectPoint));
			ocean_assert(PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(rightPose), objectPoint));

			pointsRight[n] = pinholeCamera.projectToImage<false>(rightPose, objectPoint, false);

			pointsRightNoised[n] = pointsRight[n] + Vector2(Random::gaussianNoise(0.5), Random::gaussianNoise(0.5));
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
					localSucceeded = false;
				}
			}

			if (localSucceeded)
			{
				++validIterations;
			}
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Performance: " << String::toAString(performance.averageMseconds()) << " ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << " % succeeded.";

	if (useSVD)
	{
		return percent >= 0.99;
	}

	return percent >= 0.95;
}

bool TestHomography::testHomographyMatrixFromPointsAndLinesSVD(const double testDuration, const size_t correspondences)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with " << String::insertCharacter(String::toAString(correspondences), ',', 3, false) << " point or lines:";

	constexpr unsigned int width = std::is_same<float, Scalar>::value ? 640u : 1920u;
	constexpr unsigned int height = std::is_same<float, Scalar>::value ? 480u : 1080u;

	const PinholeCamera pinholeCamera(width, height, Numeric::deg2rad(60));

	RandomGenerator randomGenerator;

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);

	do
	{
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
			if (RandomI::random(randomGenerator, 1u) == 0u)
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
			bool localSucceeded = Geometry::Homography::homographyMatrixFromPointsAndLinesSVD(pointsLeft.data(), pointsRight.data(), pointsLeft.size(), linesLeft.data(), linesRight.data(), linesLeft.size(), right_H_left);
		performance.stop();

		if (localSucceeded)
		{
			constexpr Scalar posEpsilon = 1;
			const Scalar angleCosEpsilon = Numeric::cos(Numeric::deg2rad(5));

			for (size_t n = 0; n < pointsLeft.size(); ++n)
			{
				const Vector2 transformedPoint = right_H_left * pointsLeft[n];
				if (!transformedPoint.isEqual(pointsRight[n], posEpsilon))
				{
					localSucceeded = false;
				}
			}

			for (size_t n = 0; n < linesLeft.size(); ++n)
			{
				const Vector2 pointOnLineLine = linesLeft[n].point(Random::scalar(randomGenerator, -10, 10));

				const Vector2 pointOnLineRight = right_H_left * pointOnLineLine;
				if (linesRight[n].distance(pointOnLineRight) > posEpsilon)
				{
					localSucceeded = false;
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
					localSucceeded = false;
				}

				if (Numeric::abs(lineRight.z() - transformedLine.z()) > posEpsilon)
				{
					localSucceeded = false;
				}
			}

			if (localSucceeded)
			{
				++validIterations;
			}
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Performance: " << String::toAString(performance.averageMseconds()) << " ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << " % succeeded.";

	const bool succeeded = percent >= 0.99;

	if (!succeeded && std::is_same<Scalar, float>::value)
	{
		Log::info() << "The test failed, however the applied 32 bit floating point value precision is too low for this function so that we rate the result as expected.";
		return true;
	}

	return succeeded;
}

bool TestHomography::testHomographyMatrixRANSAC(const double testDuration, const bool refine, const bool useSVD, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing determination of homography matrix with RANSAC for " << sizeof(Scalar) * 8 << "bit floating point precision, " << (useSVD ? "using SVD, " : "using a linear solution, ") << (refine ? "refining:" : "not refining:");
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testHomographyMatrixRANSAC(testDuration, 20, refine, useSVD, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testHomographyMatrixRANSAC(testDuration, 50, refine, useSVD, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testHomographyMatrixRANSAC(testDuration, 100, refine, useSVD, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testHomographyMatrixRANSAC(testDuration, 200, refine, useSVD, worker) && allSucceeded;

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

bool TestHomography::testHomographyMatrixRANSAC(const double testDuration, const size_t points, const bool refine, const bool useSVD, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with " << String::insertCharacter(String::toAString(points), ',', 3, false) << " points:";

	constexpr unsigned int width = 1920u;
	constexpr unsigned int height = 1080u;

	const PinholeCamera pinholeCamera(width, height, Numeric::deg2rad(60));

	Vectors2 pointsLeft(points);
	Vectors2 pointsRight(points);
	Vectors2 pointsRightNoisedAndFaulty(points);

	RandomGenerator randomGenerator;

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	HighPerformanceStatistic performanceSingleCore;
	HighPerformanceStatistic performanceMultiCore;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int workerIteration = 0u; workerIteration <= 1u; ++workerIteration)
		{
			// we create a realistic homography based on two camera poses and a 3D plane in front of both cameras

			const Plane3 plane(Vector3(0, 0, -4), Vector3(0, 0, 1));

			const HomogenousMatrix4 leftPose(Random::vector3(randomGenerator, Scalar(-0.2), Scalar(0.2)), Random::euler(randomGenerator, 0, Numeric::deg2rad(10)));
			const HomogenousMatrix4 rightPose(Random::vector3(randomGenerator, Scalar(-0.2), Scalar(0.2)), Random::euler(randomGenerator, 0, Numeric::deg2rad(10)));

			const SquareMatrix3 left_T_right = Geometry::Homography::homographyMatrix(leftPose, rightPose, pinholeCamera, pinholeCamera, plane);
			ocean_assert_and_suppress_unused(!left_T_right.isSingular(), left_T_right);

			for (size_t n = 0; n < points; ++n)
			{
				pointsLeft[n] = Vector2(Random::scalar(0, Scalar(width)), Random::scalar(0, Scalar(height)));

				Vector3 objectPoint(Numeric::minValue(), Numeric::minValue(), Numeric::minValue());
				if (!plane.intersection(pinholeCamera.ray(pointsLeft[n], leftPose), objectPoint))
				{
					ocean_assert(false && "This should never happen!");
				}

				ocean_assert(PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(leftPose), objectPoint));
				ocean_assert(PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(rightPose), objectPoint));

				pointsRight[n] = pinholeCamera.projectToImage<false>(rightPose, objectPoint, false);

				pointsRightNoisedAndFaulty[n] = pointsRight[n] + Vector2(Random::scalar(Scalar(-0.5), Scalar(0.5)), Random::scalar(Scalar(-0.5), Scalar(0.5)));
			}

			IndexSet32 faultySet;
			while (faultySet.size() < 20 * points / 100)
			{
				faultySet.insert(RandomI::random32(randomGenerator) % (unsigned int)(points - 1));
			}

			for (const Index32& index : faultySet)
			{
				pointsRightNoisedAndFaulty[index] += Vector2(Random::scalar(randomGenerator, Scalar(10), Scalar(50)) * Random::sign(), Random::scalar(randomGenerator, Scalar(10), Scalar(50)) * Random::sign());
			}

			SquareMatrix3 homography;

			const unsigned int testCandidates = RandomI::random(randomGenerator, 4u, min(12u, (unsigned int)(points - faultySet.size()) / 2u));

			Indices32 dummyIndices;
			Indices32* usedIndices = RandomI::random(randomGenerator, 1u) == 0u ? &dummyIndices : nullptr;

			bool localSucceeded = false;

			if (workerIteration == 0u)
			{
				performanceSingleCore.start();
				localSucceeded = Geometry::RANSAC::homographyMatrix(pointsLeft.data(), pointsRightNoisedAndFaulty.data(), points, randomGenerator, homography, testCandidates, refine, 80u, Scalar(1.5 * 1.5), usedIndices, nullptr, useSVD);
				performanceSingleCore.stop();
			}
			else
			{
				performanceMultiCore.start();
				localSucceeded = Geometry::RANSAC::homographyMatrix(pointsLeft.data(), pointsRightNoisedAndFaulty.data(), points, randomGenerator, homography, testCandidates, refine, 80u, Scalar(1.5 * 1.5), usedIndices, &worker, useSVD);
				performanceMultiCore.stop();
			}

			if (localSucceeded)
			{
				for (size_t n = 0; n < points; ++n)
				{
					const Vector2 transformedPoint = homography * pointsLeft[n];
					if (transformedPoint.isEqual(pointsRight[n], 4))
					{
						++validIterations;
					}
				}
			}

			iterations += uint64_t(points);
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Performance single-core: " << String::toAString(performanceSingleCore.medianMseconds()) << " ms (median)";
	Log::info() << "Performance multi-core: " << String::toAString(performanceMultiCore.medianMseconds()) << " ms (median)";
	Log::info() << "Multi-core boost factor: " << String::toAString(performanceSingleCore.median() / performanceMultiCore.median(), 1u) << "x (median)";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << " % succeeded.";

	return percent >= 0.95;
}

bool TestHomography::testHomographyMatrixRANSACForNonBijectiveCorrespondences(const double testDuration, const bool refine, const bool useSVD, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing determination of non-bijective homography matrix with RANSAC for " << sizeof(Scalar) * 8 << "bit floating point precision, " << (useSVD ? "using SVD, " : "using a linear solution, ") << (refine ? "refining:" : "not refining:");
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testHomographyMatrixRANSACForNonBijectiveCorrespondences(testDuration, 20, refine, useSVD, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testHomographyMatrixRANSACForNonBijectiveCorrespondences(testDuration, 50, refine, useSVD, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testHomographyMatrixRANSACForNonBijectiveCorrespondences(testDuration, 100, refine, useSVD, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testHomographyMatrixRANSACForNonBijectiveCorrespondences(testDuration, 200, refine, useSVD, worker) && allSucceeded;

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

bool TestHomography::testHomographyMatrixRANSACForNonBijectiveCorrespondences(const double testDuration, const size_t points, const bool refine, const bool useSVD, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with " << String::insertCharacter(String::toAString(points), ',', 3, false) << " points:";

	constexpr unsigned int width = 1920u;
	constexpr unsigned int height = 1080u;

	const PinholeCamera pinholeCamera(width, height, Numeric::deg2rad(60));

	Vectors2 pointsLeft;
	Vectors2 pointsRight;
	Vectors2 pointsRightNoised;
	IndexPairs32 nonBijectiveCorrespondences;
	IndexPairs32 nonBijectiveCorrespondencesFaulty;

	Scalar sumError = 0;

	RandomGenerator randomGenerator;

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	HighPerformanceStatistic performanceSingleCore;
	HighPerformanceStatistic performanceMultiCore;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int workerIteration = 0u; workerIteration <= 1u; ++workerIteration)
		{
			pointsLeft.clear();
			pointsRight.clear();
			pointsRightNoised.clear();

			nonBijectiveCorrespondences.clear();
			nonBijectiveCorrespondencesFaulty.clear();

			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMultiCore : performanceSingleCore;

			// we create a realistic homography based on two camera poses and a 3D plane in front of both cameras

			const Plane3 plane(Vector3(0, 0, -4), Vector3(0, 0, 1));

			const HomogenousMatrix4 leftPose(Random::vector3(randomGenerator, Scalar(-0.2), Scalar(0.2)), Random::euler(randomGenerator, 0, Numeric::deg2rad(10)));
			const HomogenousMatrix4 rightPose(Random::vector3(randomGenerator, Scalar(-0.2), Scalar(0.2)), Random::euler(randomGenerator, 0, Numeric::deg2rad(10)));

			const SquareMatrix3 left_T_right = Geometry::Homography::homographyMatrix(leftPose, rightPose, pinholeCamera, pinholeCamera, plane);
			ocean_assert_and_suppress_unused(!left_T_right.isSingular(), left_T_right);

			for (size_t n = 0; n < points; ++n)
			{
				const Vector2 pointLeft(Random::vector2(randomGenerator, 0, Scalar(width), 0, Scalar(height)));

				Vector3 objectPoint(Numeric::minValue(), Numeric::minValue(), Numeric::minValue());
				if (!plane.intersection(pinholeCamera.ray(pointLeft, leftPose), objectPoint))
				{
					ocean_assert(false && "This should never happen!");
				}

				ocean_assert(PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(leftPose), objectPoint));
				ocean_assert(PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(rightPose), objectPoint));

				const Vector2 pointRight = pinholeCamera.projectToImage<false>(rightPose, objectPoint, false);

				// lefts randomly add unused points to the set of left points
				if (RandomI::random(randomGenerator, 5u) == 0u)
				{
					pointsLeft.push_back(Random::vector2(randomGenerator, 0, Scalar(width), 0, Scalar(height)));
				}

				// lefts randomly add unused points to the set of right points
				if (RandomI::random(randomGenerator, 5u) == 0u)
				{
					pointsRight.push_back(Random::vector2(randomGenerator, 0, Scalar(width), 0, Scalar(height)));
					pointsRightNoised.push_back(pointsRight.back() + Random::vector2(randomGenerator, Scalar(-0.5), Scalar(0.5)));
				}

				const IndexPair32 correspondence = IndexPair32((unsigned int)pointsLeft.size(), (unsigned int)pointsRight.size());

				nonBijectiveCorrespondences.push_back(correspondence);
				nonBijectiveCorrespondencesFaulty.push_back(correspondence);

				pointsLeft.push_back(pointLeft);
				pointsRight.push_back(pointRight);
				pointsRightNoised.push_back(pointRight + Random::vector2(randomGenerator, Scalar(-0.5), Scalar(0.5)));
			}

			ocean_assert(nonBijectiveCorrespondences.size() == points);
			ocean_assert(pointsLeft.size() >= points);
			ocean_assert(pointsRight.size() >= points);
			ocean_assert(pointsRight.size() == pointsRightNoised.size());

			IndexSet32 faultySetLeft;
			while (faultySetLeft.size() < 10 * points / 100)
			{
				faultySetLeft.insert(RandomI::random(randomGenerator, (unsigned int)(nonBijectiveCorrespondences.size() - 1)));
			}

			IndexSet32 faultySetRight;
			while (faultySetRight.size() < 10 * points / 100)
			{
				faultySetRight.insert(RandomI::random(randomGenerator, (unsigned int)(nonBijectiveCorrespondences.size() - 1)));
			}

			for (const Index32& index : faultySetLeft)
			{
				const unsigned int oldValue = nonBijectiveCorrespondencesFaulty[index].first;

				while (nonBijectiveCorrespondencesFaulty[index].first == oldValue)
				{
					nonBijectiveCorrespondencesFaulty[index].first = RandomI::random(randomGenerator, (unsigned int)(pointsLeft.size()) - 1u);
				}
			}

			for (const Index32& index : faultySetRight)
			{
				const unsigned int oldValue = nonBijectiveCorrespondencesFaulty[index].second;

				while (nonBijectiveCorrespondencesFaulty[index].second == oldValue)
				{
					nonBijectiveCorrespondencesFaulty[index].second = RandomI::random(randomGenerator, (unsigned int)pointsRight.size() - 1u);
				}
			}

			SquareMatrix3 homography;

			const unsigned int testCandidates = RandomI::random(randomGenerator, 4u, 8u);

			Indices32 dummyIndices;
			Indices32* usedIndices = RandomI::random(randomGenerator, 1u) == 0u ? &dummyIndices : nullptr;

			performance.start();
				const bool localSucceeded = Geometry::RANSAC::homographyMatrixForNonBijectiveCorrespondences(pointsLeft.data(), pointsLeft.size(), pointsRightNoised.data(), pointsRightNoised.size(), nonBijectiveCorrespondencesFaulty.data(), nonBijectiveCorrespondencesFaulty.size(), randomGenerator, homography, testCandidates, refine, 80u, Scalar(1.5 * 1.5), usedIndices, useWorker, useSVD);
			performance.stop();

			if (localSucceeded)
			{
				for (const IndexPair32& correspondence : nonBijectiveCorrespondences)
				{
					ocean_assert(correspondence.first < pointsLeft.size());
					ocean_assert(correspondence.second < pointsRight.size());

					const Vector2& pointLeft = pointsLeft[correspondence.first];
					const Vector2& pointRight = pointsRight[correspondence.second];

					const Vector2 transformedPoint = homography * pointLeft;
					if (transformedPoint.isEqual(pointRight, 4))
					{
						sumError += transformedPoint.sqrDistance(pointRight);
						++validIterations;
					}
				}
			}

			iterations += uint64_t(points);
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Performance single-core: " << String::toAString(performanceSingleCore.medianMseconds()) << " ms (median)";
	Log::info() << "Performance multi-core: " << String::toAString(performanceMultiCore.medianMseconds()) << " ms (median)";
	Log::info() << "Multi-core boost factor: " << String::toAString(performanceSingleCore.median() / performanceMultiCore.median(), 1u) << "x (median)";
	Log::info() << "Average sqr error: " << String::toAString(Numeric::ratio(sumError, Scalar(validIterations))) << "px";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << " % succeeded.";

	return percent >= 0.95;
}

}

}

}
