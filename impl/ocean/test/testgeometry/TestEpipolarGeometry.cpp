/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestEpipolarGeometry.h"
#include "ocean/test/testgeometry/Utilities.h"

#include "ocean/base/Timestamp.h"

#include "ocean/geometry/EpipolarGeometry.h"
#include "ocean/geometry/NonLinearOptimization.h"
#include "ocean/geometry/RANSAC.h"
#include "ocean/geometry/Utilities.h"

#include "ocean/math/Euler.h"
#include "ocean/math/Line3.h"
#include "ocean/math/Plane3.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

using namespace Ocean::Geometry;

bool TestEpipolarGeometry::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Epipolar geometry test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testFaultlessFundamentalMatrix(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFaultlessNoisedFundamentalMatrix(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFaultyFundamentalMatrix(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testTriangulateImagePoints(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Epipolar geometry test succeeded.";
	}
	else
	{
		Log::info() << "Epipolar geometry test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestEpipolarGeometry, FaultlessFundamentalMatrix)
{
	EXPECT_TRUE(TestEpipolarGeometry::testFaultlessFundamentalMatrix(GTEST_TEST_DURATION));
}

TEST(TestEpipolarGeometry, FaultlessNoisedFundamentalMatrix)
{
	EXPECT_TRUE(TestEpipolarGeometry::testFaultlessNoisedFundamentalMatrix(GTEST_TEST_DURATION));
}

TEST(TestEpipolarGeometry, FaultyFundamentalMatrix)
{
	EXPECT_TRUE(TestEpipolarGeometry::testFaultyFundamentalMatrix(GTEST_TEST_DURATION));
}

TEST(TestEpipolarGeometry, TriangulateImagePoints)
{
	EXPECT_TRUE(TestEpipolarGeometry::testTriangulateImagePoints(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestEpipolarGeometry::testFaultlessFundamentalMatrix(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing faultless fundamental matrix:";

	RandomGenerator randomGenerator;

	for (unsigned int points : {8u, 11u, 15u, 30u, 50u, 90u, 500u})
	{
		Log::info() << " ";
		Log::info() << "... with " << points << " points:";

		unsigned long long failed = 0ull;
		unsigned long long iterations = 0ull;

		const Timestamp startTimestamp(true);

		do
		{
			bool noError = true;

			const PinholeCamera pinholeCamera(Random::random(randomGenerator, 600u, 800u), Random::random(randomGenerator, 600u, 800u), Numeric::deg2rad(Random::scalar(randomGenerator, Scalar(30), Scalar(70))));

			ImagePoints leftImagePoints;
			ObjectPoints leftObjectPoints;

			for (size_t i = 0; i < points; ++i)
			{
				const ImagePoint imagePoint(Random::scalar(randomGenerator, 0, Scalar(pinholeCamera.width())), Random::scalar(randomGenerator, 0, Scalar(pinholeCamera.height())));
				const ObjectPoint objectPoint(pinholeCamera.vector(imagePoint) * Random::scalar(randomGenerator, Scalar(1), Scalar(3)));

				leftImagePoints.push_back(imagePoint);
				leftObjectPoints.push_back(objectPoint);
			}

			const Vector3 translation(Random::vector3(randomGenerator, Scalar(0.01), Scalar(0.1)));
			const Euler euler(Random::scalar(randomGenerator, Numeric::deg2rad(Scalar(-10)), Numeric::deg2rad(Scalar(10))),
							Random::scalar(randomGenerator, Numeric::deg2rad(Scalar(-10)), Numeric::deg2rad(Scalar(10))),
							Random::scalar(randomGenerator, Numeric::deg2rad(Scalar(-10)), Numeric::deg2rad(Scalar(10))));
			const Quaternion quaternion(euler);

			const HomogenousMatrix4 lTr(translation, quaternion);
			const HomogenousMatrix4 rTl(lTr.inverted());

			ImagePoints rightImagePoints;
			ObjectPoints rightObjectPoints;

			for (size_t i = 0; i < leftImagePoints.size(); ++i)
			{
				const ObjectPoint objectPoint(rTl * leftObjectPoints[i]);
				const ImagePoint imagePoint(pinholeCamera.projectToImage<true>(HomogenousMatrix4(true), objectPoint, false));

				rightImagePoints.push_back(imagePoint);
				rightObjectPoints.push_back(objectPoint);
			}

			SquareMatrix3 fundamental;
			if (Ocean::Geometry::EpipolarGeometry::fundamentalMatrix(leftImagePoints.data(), rightImagePoints.data(), leftImagePoints.size(), fundamental))
			{
				for (size_t i = 0; i < leftImagePoints.size(); ++i)
				{
					const Vector3 left(leftImagePoints[i], 1);
					const Vector3 right(rightImagePoints[i], 1);

					const Scalar scalarProduct = (fundamental * left) * right;

					if (Numeric::isNotWeakEqualEps(scalarProduct))
					{
						noError = false;
					}
				}

				Vector2 leftEpipole, rightEpipole;
				if (Geometry::EpipolarGeometry::epipoles(fundamental, leftEpipole, rightEpipole))
				{
					Vector2 trueLeftEpipole(0, 0);
					Vector2 trueRightEpipole(0, 0);
					Ocean::Geometry::EpipolarGeometry::epipoles(lTr, pinholeCamera.intrinsic(), pinholeCamera.intrinsic(), trueLeftEpipole, trueRightEpipole);

					const Scalar errorLeft = (leftEpipole - trueLeftEpipole).length();
					const Scalar errorRight = (rightEpipole - trueRightEpipole).length();

					// maximal pixel error: 0.1
					if (errorLeft > Scalar(0.1) || errorRight > Scalar(0.1))
					{
						noError = false;
					}
				}

				const SquareMatrix3 essential = Ocean::Geometry::EpipolarGeometry::fundamental2essential(fundamental, pinholeCamera.intrinsic(), pinholeCamera.intrinsic());

				for (size_t i = 0; i < leftObjectPoints.size(); ++i)
				{
					const Vector3 left = pinholeCamera.intrinsic().inverted() * Vector3(leftImagePoints[i], Scalar(1));
					const Vector3 right = pinholeCamera.intrinsic().inverted() * Vector3(rightImagePoints[i], Scalar(1));

					const Scalar scalarProduct = (essential * left) * right;

					if (Numeric::isNotWeakEqualEps(scalarProduct))
					{
						noError = false;
					}
				}

				HomogenousMatrix4 factorizedTransformation;
				if (Geometry::EpipolarGeometry::factorizeEssential(essential, pinholeCamera, pinholeCamera, leftImagePoints[0], rightImagePoints[0], factorizedTransformation))
				{
					const Vector3 factorizedTranslation(factorizedTransformation.translation());
					const Quaternion factorizedRotation(factorizedTransformation.rotation());

					const Scalar translationDifference = (translation.normalized() - factorizedTranslation).length();
					const Scalar angleDifference = Numeric::rad2deg(factorizedRotation.angle(quaternion));

					if (translationDifference >= Scalar(0.001) || angleDifference > Scalar(5))
					{
						noError = false;
					}
				}
				else
				{
					noError = false;
				}
			}
			else
			{
				noError = false;
			}

			if (!noError)
			{
				++failed;
			}

			++iterations;
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);
		Log::info() << "Validation: " << String::toAString(double(iterations - failed) * 100.00 / double(iterations), 1u) << "% succeeded.";
	}

	return true;
}

bool TestEpipolarGeometry::testFaultlessNoisedFundamentalMatrix(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing Gaussian noised fundamental matrix:";

	RandomGenerator randomGenerator;

	for (unsigned int points : {8u, 11u, 15u, 30u, 50u, 90u, 500u})
	{
		Log::info() << " ";
		Log::info() << "... with " << points << " points:";

		unsigned long long failed = 0ull;
		unsigned long long iterations = 0ull;

		Scalar totalFundamentalScalarProductError = 0;
		Scalar totalTranslationError = 0;
		Scalar totalAngleError = 0;

		const Timestamp startTimestamp(true);

		do
		{
			bool noError = true;

			const PinholeCamera pinholeCamera(640u, 480u, Numeric::deg2rad(Random::scalar(randomGenerator, Scalar(50), Scalar(70))));

			ImagePoints leftImagePoints;
			ObjectPoints leftObjectPoints;

			for (unsigned int i = 0u; i < points; ++i)
			{
				const ImagePoint imagePoint(Random::scalar(randomGenerator, 0, Scalar(pinholeCamera.width())), Random::scalar(randomGenerator, 0, Scalar(pinholeCamera.height())));
				const ObjectPoint objectPoint(pinholeCamera.vector(imagePoint) * Random::scalar(randomGenerator, Scalar(0.1), Scalar(2.0)));

				leftImagePoints.push_back(imagePoint);
				leftObjectPoints.push_back(objectPoint);
			}

			const Vector3 translation(Random::vector3(randomGenerator, Scalar(0.01), Scalar(0.1)));
			const Euler euler(Random::scalar(randomGenerator, Numeric::deg2rad(Scalar(-10)), Numeric::deg2rad(Scalar(10))),
							Random::scalar(randomGenerator, Numeric::deg2rad(Scalar(-10)), Numeric::deg2rad(Scalar(10))),
							Random::scalar(randomGenerator, Numeric::deg2rad(Scalar(-10)), Numeric::deg2rad(Scalar(10))));
			const Quaternion quaternion(euler);

			const HomogenousMatrix4 lTr(translation, quaternion);
			const HomogenousMatrix4 rTl(lTr.inverted());

			ImagePoints rightImagePoints;
			ObjectPoints rightObjectPoints;

			for (size_t i = 0; i < leftImagePoints.size(); ++i)
			{
				const ObjectPoint objectPoint(rTl * leftObjectPoints[i]);
				const ImagePoint imagePoint(pinholeCamera.projectToImage<true>(HomogenousMatrix4(true), objectPoint, false));

				rightImagePoints.push_back(imagePoint);
				rightObjectPoints.push_back(objectPoint);
			}

			for (size_t i = 0; i < points; ++i)
			{
				const Vector2 leftNoise(Random::gaussianNoise(randomGenerator, Scalar(2)), Random::gaussianNoise(randomGenerator, Scalar(2)));
				const Vector2 rightNoise(Random::gaussianNoise(randomGenerator, Scalar(2)), Random::gaussianNoise(randomGenerator, Scalar(2)));

				leftImagePoints[i] += leftNoise;
				rightImagePoints[i] += rightNoise;
			}

			SquareMatrix3 fundamental;
			if (Ocean::Geometry::EpipolarGeometry::fundamentalMatrix(leftImagePoints.data(), rightImagePoints.data(), leftImagePoints.size(), fundamental))
			{
				Scalar fundamentalScalarProductError = 0;

				for (size_t i = 0; i < leftImagePoints.size(); ++i)
				{
					const Vector3 left(leftImagePoints[i], 1);
					const Vector3 right(rightImagePoints[i], 1);

					const Scalar scalarProduct = Numeric::abs((fundamental * left) * right);

					fundamentalScalarProductError += scalarProduct;
				}

				totalFundamentalScalarProductError += fundamentalScalarProductError / Scalar(leftImagePoints.size());

				const SquareMatrix3 essential = Ocean::Geometry::EpipolarGeometry::fundamental2essential(fundamental, pinholeCamera.intrinsic(), pinholeCamera.intrinsic());

				HomogenousMatrix4 factorizedTransformation;
				if (Ocean::Geometry::EpipolarGeometry::factorizeEssential(essential, pinholeCamera, pinholeCamera, leftImagePoints[0], rightImagePoints[0], factorizedTransformation))
				{
					const Vector3 factorizedTranslation(factorizedTransformation.translation());
					const Quaternion factorizedRotation(factorizedTransformation.rotation());

					const Scalar translationDifference = (translation.normalized() - factorizedTranslation).length();
					const Scalar angleDifference = Numeric::rad2deg(factorizedRotation.angle(quaternion));

					totalTranslationError += translationDifference;
					totalAngleError += angleDifference;

					if (translationDifference >= Scalar(0.05) || angleDifference > Scalar(5))
					{
						noError = false;
					}
				}
				else
				{
					noError = false;
				}
			}
			else
			{
				noError = false;
			}

			if (!noError)
			{
				++failed;
			}

			++iterations;
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);

		Log::info() << "Average error (Pright * F * Pleft): " << totalFundamentalScalarProductError / Scalar(iterations);
		Log::info() << "Average translation error: " << totalTranslationError / Scalar(iterations);
		Log::info() << "Average angle error: " << totalAngleError / Scalar(iterations);
		Log::info() << "Validation: " << String::toAString(double(iterations - failed) * 100.0 / double(iterations), 1u) << "% succeeded.";
	}

	return true;
}

bool TestEpipolarGeometry::testFaultyFundamentalMatrix(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing faulty fundamental matrix with 20% invalid correspondences:";

	RandomGenerator randomGenerator;

	for (unsigned int points : {14u, 20u, 30u, 50u, 90u, 500u})
	{
		Log::info() << " ";
		Log::info() << "... with " << points << " points:";

		unsigned long long failed = 0ull;
		unsigned long long iterations = 0ull;

		const Timestamp startTimestamp(true);

		do
		{
			bool noError = true;

			const PinholeCamera pinholeCamera(Random::random(randomGenerator, 600u, 800u), Random::random(randomGenerator, 600u, 800u), Numeric::deg2rad(Random::scalar(randomGenerator, Scalar(30), Scalar(70))));

			ImagePoints leftImagePoints;
			ObjectPoints leftObjectPoints;

			for (unsigned int i = 0u; i < points; ++i)
			{
				const ImagePoint imagePoint(Random::scalar(randomGenerator, Scalar(0), Scalar(pinholeCamera.width() - 1)), Random::scalar(randomGenerator, Scalar(0), Scalar(pinholeCamera.height() - 1)));
				const ObjectPoint objectPoint(pinholeCamera.vector(imagePoint) * Random::scalar(randomGenerator, Scalar(1), Scalar(3)));

				leftImagePoints.push_back(imagePoint);
				leftObjectPoints.push_back(objectPoint);
			}

			const Vector3 translation(Random::vector3(randomGenerator, Scalar(0.01), Scalar(0.1)));
			const Euler euler(Random::scalar(randomGenerator, Numeric::deg2rad(Scalar(-10)), Numeric::deg2rad(Scalar(10))),
							Random::scalar(randomGenerator, Numeric::deg2rad(Scalar(-10)), Numeric::deg2rad(Scalar(10))),
							Random::scalar(randomGenerator, Numeric::deg2rad(Scalar(-10)), Numeric::deg2rad(Scalar(10))));
			const Quaternion quaternion(euler);

			const HomogenousMatrix4 lTr(translation, quaternion);
			const HomogenousMatrix4 rTl(lTr.inverted());

			ImagePoints rightImagePoints;
			ObjectPoints rightObjectPoints;

			for (size_t i = 0; i < leftImagePoints.size(); ++i)
			{
				const ObjectPoint objectPoint(rTl * leftObjectPoints[i]);
				const ImagePoint imagePoint(pinholeCamera.projectToImage<true>(HomogenousMatrix4(true), objectPoint, false));

				rightImagePoints.push_back(imagePoint);
				rightObjectPoints.push_back(objectPoint);
			}

			// randomly select some image points to disturb
			IndexSet32 indexSet;

			unsigned int numberInvalidFeatures = points * 2u / 10u;
			ocean_assert(numberInvalidFeatures < points);

			while (indexSet.size() < numberInvalidFeatures)
			{
				indexSet.insert(Random::random(randomGenerator, points - 1u));
			}

			for (IndexSet32::const_iterator i = indexSet.begin(); i != indexSet.end(); ++i)
			{
				ocean_assert(*i < points);

				leftImagePoints[*i] += Random::vector2(randomGenerator, Scalar(-15), Scalar(15));
				leftImagePoints[*i].x() = max(Scalar(0), min(Scalar(pinholeCamera.width() - 1u), leftImagePoints[*i].x()));
				leftImagePoints[*i].y() = max(Scalar(0), min(Scalar(pinholeCamera.height() - 1u), leftImagePoints[*i].y()));

				rightImagePoints[*i] += Random::vector2(randomGenerator, Scalar(-15), Scalar(15));
				rightImagePoints[*i].x() = max(Scalar(0), min(Scalar(pinholeCamera.width() - 1u), rightImagePoints[*i].x()));
				rightImagePoints[*i].y() = max(Scalar(0), min(Scalar(pinholeCamera.height() - 1u), rightImagePoints[*i].y()));
			}

			SquareMatrix3 fundamental;
			if (Ocean::Geometry::RANSAC::fundamentalMatrix(leftImagePoints.data(), rightImagePoints.data(), leftImagePoints.size(), pinholeCamera.width(), pinholeCamera.height(), fundamental))
			{
				for (size_t i = 0; i < leftImagePoints.size(); ++i)
				{
					if (indexSet.find((unsigned int)i) == indexSet.end())
					{
						const Vector3 left(leftImagePoints[i], 1);
						const Vector3 right(rightImagePoints[i], 1);

						const Scalar scalarProduct = (fundamental * left) * right;

						if (Numeric::isNotWeakEqualEps(scalarProduct))
						{
							noError = false;
						}
					}
				}

				Vector2 leftEpipole, rightEpipole;
				if (Ocean::Geometry::EpipolarGeometry::epipoles(fundamental, leftEpipole, rightEpipole))
				{
					Vector2 trueLeftEpipole(0, 0);
					Vector2 trueRightEpipole(0, 0);
					Ocean::Geometry::EpipolarGeometry::epipoles(lTr, pinholeCamera.intrinsic(), pinholeCamera.intrinsic(), trueLeftEpipole, trueRightEpipole);

					const Scalar errorLeft = (leftEpipole - trueLeftEpipole).length();
					const Scalar errorRight = (rightEpipole - trueRightEpipole).length();

					// maximal pixel error: 0.1
					if (errorLeft > Scalar(0.1) || errorRight > Scalar(0.1))
					{
						noError = false;
					}
				}

				const SquareMatrix3 essential = Ocean::Geometry::EpipolarGeometry::fundamental2essential(fundamental, pinholeCamera.intrinsic(), pinholeCamera.intrinsic());

				for (size_t i = 0; i < leftObjectPoints.size(); ++i)
				{
					if (indexSet.find((unsigned int)i) == indexSet.end())
					{
						const Vector3 left = pinholeCamera.intrinsic().inverted() * Vector3(leftImagePoints[i], 1);
						const Vector3 right = pinholeCamera.intrinsic().inverted() * Vector3(rightImagePoints[i], 1);

						const Scalar scalarProduct = (essential * left) * right;

						if (Numeric::isNotWeakEqualEps(scalarProduct))
							noError = false;
					}
				}

				HomogenousMatrix4 factorizedTransformation;
				if (Ocean::Geometry::EpipolarGeometry::factorizeEssential(essential, pinholeCamera, pinholeCamera, leftImagePoints[0], rightImagePoints[0], factorizedTransformation))
				{
					const Vector3 factorizedTranslation(factorizedTransformation.translation());
					const Quaternion factorizedRotation(factorizedTransformation.rotation());

					const Scalar translationDifference = (translation.normalized() - factorizedTranslation).length();
					const Scalar angleDifference = Numeric::rad2deg(factorizedRotation.angle(quaternion));

					if (translationDifference >= Scalar(0.05) || angleDifference > Scalar(5))
					{
						noError = false;
					}
				}
				else
				{
					noError = false;
				}
			}
			else
			{
				noError = false;
			}

			if (!noError)
			{
				++failed;
			}

			++iterations;
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);

		Log::info() << "Validation: " << String::toAString(double(iterations - failed) * 100.0 / double(iterations), 1u) << "% succeeded.";
	}

	return true;
}

bool TestEpipolarGeometry::testTriangulateImagePoints(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing triangulation of image points";
	Log::info() << " ";

	const Timestamp startTimestamp(true);

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	do
	{
		for (const AnyCameraType anyCameraType : { AnyCameraType::PINHOLE, AnyCameraType::FISHEYE })
		{
			const SharedAnyCamera anyCamera = Utilities::realisticAnyCamera(anyCameraType);

			ocean_assert(anyCamera != nullptr && anyCamera->isValid());

			const Box3 boundingBox(Vector3(0, 0, 0), Scalar(1), Scalar(1), Scalar(1));
			const Sphere3 boundingSphere(boundingBox);

			const unsigned int numberObjectPoints = RandomI::random(randomGenerator, 1u, 100u);
			const Vectors3 objectPoints = Test::TestGeometry::Utilities::objectPoints(boundingBox, size_t(numberObjectPoints), &randomGenerator);

			const HomogenousMatrix4 world_T_cameraA = Test::TestGeometry::Utilities::viewPosition(*anyCamera, boundingSphere, /* viewDirection */ Random::vector3(randomGenerator));
			const HomogenousMatrix4 world_T_cameraB = Test::TestGeometry::Utilities::viewPosition(*anyCamera, boundingSphere, /* viewDirection */ Random::vector3(randomGenerator));

			const HomogenousMatrix4 flippedCameraA_T_world = PinholeCamera::standard2InvertedFlipped(world_T_cameraA);
			const HomogenousMatrix4 flippedCameraB_T_world = PinholeCamera::standard2InvertedFlipped(world_T_cameraB);

			Vectors2 imagePointsA;
			Vectors2 imagePointsB;

			imagePointsA.reserve(objectPoints.size());
			imagePointsB.reserve(objectPoints.size());

			for (const Vector3& objectPoint : objectPoints)
			{
				ocean_assert(PinholeCamera::isObjectPointInFrontIF(flippedCameraA_T_world, objectPoint));
				ocean_assert(PinholeCamera::isObjectPointInFrontIF(flippedCameraB_T_world, objectPoint));

				imagePointsA.emplace_back(anyCamera->projectToImageIF(flippedCameraA_T_world * objectPoint));
				imagePointsB.emplace_back(anyCamera->projectToImageIF(flippedCameraB_T_world * objectPoint));
			}

			const Vectors3 triangulatedObjectPoints = EpipolarGeometry::triangulateImagePoints(world_T_cameraA, world_T_cameraB, *anyCamera, *anyCamera, imagePointsA.data(), imagePointsB.data(), imagePointsA.size());

			if (triangulatedObjectPoints.size() != objectPoints.size())
			{
				allSucceeded = false;
			}
			else
			{
				for (size_t i = 0; i < objectPoints.size(); ++i)
				{
					if (Numeric::isWeakEqualEps(objectPoints[i].sqrDistance(triangulatedObjectPoints[i])))
					{
						++validIterations;
					}
				}
			}

			iterations += uint64_t(objectPoints.size());
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations > 0ull);
	ocean_assert(validIterations <= iterations);

	const double percent = double(validIterations) / double(iterations);

	if (percent <= 0.99)
	{
		allSucceeded = false;
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

}

}

}
