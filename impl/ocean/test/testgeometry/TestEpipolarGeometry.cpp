/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestEpipolarGeometry.h"
#include "ocean/test/testgeometry/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/geometry/EpipolarGeometry.h"
#include "ocean/geometry/RANSAC.h"

#include "ocean/math/Random.h"

#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

bool TestEpipolarGeometry::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Epipolar geometry test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testFundamentalMatrix(testDuration) && allSucceeded;

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

TEST(TestEpipolarGeometry, FundamentalMatrix)
{
	EXPECT_TRUE(TestEpipolarGeometry::testFundamentalMatrix(GTEST_TEST_DURATION));
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

bool TestEpipolarGeometry::testFundamentalMatrix(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing fundamental matrix:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	for (const size_t correspondences : {8, 11, 15, 30, 50, 90, 200})
	{
		Log::info() << " ";
		Log::info() << "... with " << correspondences << " correspondences:";

		HighPerformanceStatistic performance;

		ValidationPrecision validationFundamental(0.99, randomGenerator);
		ValidationPrecision validationEpipoles(0.99, randomGenerator);
		ValidationPrecision validationEssential(0.99, randomGenerator);
		ValidationPrecision validationFactorized(0.99, randomGenerator);

		bool needMoreIterations = false;

		const Timestamp startTimestamp(true);

		do
		{
			ValidationPrecision::ScopedIteration scopedIterationFundamental(validationFundamental);
			ValidationPrecision::ScopedIteration scopedIterationEpipoles(validationEpipoles);
			ValidationPrecision::ScopedIteration scopedIterationEssential(validationEssential);
			ValidationPrecision::ScopedIteration scopedIterationFactorized(validationFactorized);

			const unsigned int width = RandomI::random(randomGenerator, 600u, 800u);
			const unsigned int height = RandomI::random(randomGenerator, 600u, 600u);
			const Scalar fovX = Numeric::deg2rad(Random::scalar(randomGenerator, Scalar(30), Scalar(70)));

			const PinholeCamera pinholeCamera(width, height, fovX);
			const AnyCameraPinhole camera(pinholeCamera);

			const Scalar sphereRadius = Random::scalar(randomGenerator, Scalar(0.1), Scalar(1));

			Vectors3 objectPoints(correspondences);
			for (size_t n = 0; n < correspondences; ++n)
			{
				objectPoints[n] = Random::vector3(randomGenerator) * sphereRadius;
			}

			const Vector3 viewingDirectionLeft = Random::vector3(randomGenerator);
			const Vector3 viewingDirectionRight = Quaternion(Random::euler(randomGenerator, Numeric::deg2rad(50))) * viewingDirectionLeft;

			HomogenousMatrix4 world_T_leftCamera = Utilities::viewPosition(camera, Sphere3(Vector3(0, 0, 0), sphereRadius), viewingDirectionLeft);
			HomogenousMatrix4 world_T_rightCamera = Utilities::viewPosition(camera, Sphere3(Vector3(0, 0, 0), sphereRadius), viewingDirectionRight);

			// let's apply a random roll
			world_T_leftCamera *= HomogenousMatrix4(Quaternion(Vector3(0, 0, 1), Random::scalar(randomGenerator, 0, Numeric::pi2())));
			world_T_rightCamera *= HomogenousMatrix4(Quaternion(Vector3(0, 0, 1), Random::scalar(randomGenerator, 0, Numeric::pi2())));

			const HomogenousMatrix4 flippedLeftCamera_T_world(AnyCamera::standard2InvertedFlipped(world_T_leftCamera));
			const HomogenousMatrix4 flippedRightCamera_T_world(AnyCamera::standard2InvertedFlipped(world_T_rightCamera));

#ifdef OCEAN_DEBUG
			for (const Vector3& objectPoint : objectPoints)
			{
				ocean_assert(AnyCamera::isObjectPointInFrontIF(flippedLeftCamera_T_world, objectPoint));
				ocean_assert(AnyCamera::isObjectPointInFrontIF(flippedRightCamera_T_world, objectPoint));
			}
#endif

			Vectors2 leftImagePoints(correspondences);
			Vectors2 rightImagePoints(correspondences);

			for (size_t n = 0; n < correspondences; ++n)
			{
				ocean_assert(AnyCamera::isObjectPointInFrontIF(flippedLeftCamera_T_world, objectPoints[n]));
				ocean_assert(AnyCamera::isObjectPointInFrontIF(flippedRightCamera_T_world, objectPoints[n]));

				leftImagePoints[n] = camera.projectToImageIF(flippedLeftCamera_T_world, objectPoints[n]);
				rightImagePoints[n] = camera.projectToImageIF(flippedRightCamera_T_world, objectPoints[n]);
			}

			performance.start();

				SquareMatrix3 right_F_left(false);
				const bool result = Geometry::EpipolarGeometry::fundamentalMatrix(leftImagePoints.data(), rightImagePoints.data(), leftImagePoints.size(), right_F_left);

			performance.stop();

			if (result)
			{
				const HomogenousMatrix4 leftCamera_T_rightCamera = world_T_leftCamera.inverted() * world_T_rightCamera;

				{
					// verifying the fundamental matrix

					for (size_t n = 0; n < correspondences; ++n)
					{
						const Vector3 left(leftImagePoints[n], 1);
						const Vector3 right(rightImagePoints[n], 1);

						const Scalar scalarProduct = (right_F_left * left) * right;

						if (Numeric::isNotWeakEqualEps(scalarProduct))
						{
							scopedIterationFundamental.setInaccurate();
						}
					}
				}

				{
					// verifying the epipoles

					Vector2 leftEpipole;
					Vector2 rightEpipole;
					if (Geometry::EpipolarGeometry::epipoles(right_F_left, leftEpipole, rightEpipole))
					{
						Vector2 trueLeftEpipole(0, 0);
						Vector2 trueRightEpipole(0, 0);
						Geometry::EpipolarGeometry::epipoles(leftCamera_T_rightCamera, pinholeCamera.intrinsic(), pinholeCamera.intrinsic(), trueLeftEpipole, trueRightEpipole);

						const Scalar errorLeft = leftEpipole.distance(trueLeftEpipole);
						const Scalar errorRight = rightEpipole.distance(trueRightEpipole);

						constexpr Scalar maximalError = Scalar(0.1);

						if (errorLeft > maximalError || errorRight > maximalError)
						{
							scopedIterationEpipoles.setInaccurate();
						}
					}
					else
					{
						scopedIterationEpipoles.setInaccurate();
					}
				}

				{
					// verifying the essential matrix

					const SquareMatrix3 normalizedRight_E_normalizedLeft = Geometry::EpipolarGeometry::fundamental2essential(right_F_left, pinholeCamera.intrinsic(), pinholeCamera.intrinsic());

					for (size_t n = 0; n < correspondences; ++n)
					{
						const Vector3 left(leftImagePoints[n], 1);
						const Vector3 right(rightImagePoints[n], 1);

						const Vector3 normalizedLeft = pinholeCamera.invertedIntrinsic() * left;
						const Vector3 normalizedRight = pinholeCamera.invertedIntrinsic() * right;

						const Scalar scalarProduct = (normalizedRight_E_normalizedLeft * normalizedLeft) * normalizedRight;

						if (Numeric::isNotWeakEqualEps(scalarProduct))
						{
							scopedIterationEssential.setInaccurate();
						}
					}

					HomogenousMatrix4 factorizedTransformation;
					if (Geometry::EpipolarGeometry::factorizeEssential(normalizedRight_E_normalizedLeft, pinholeCamera, pinholeCamera, leftImagePoints.data(), rightImagePoints.data(), leftImagePoints.size(), factorizedTransformation))
					{
						const Vector3 factorizedTranslation(factorizedTransformation.translation());
						const Quaternion factorizedRotation(factorizedTransformation.rotation());

						const Vector3 translation = leftCamera_T_rightCamera.translation();
						const Quaternion rotation = leftCamera_T_rightCamera.rotation();

						const Scalar translationDifference = (translation.normalized() - factorizedTranslation).length();
						const Scalar angleDifference = Numeric::rad2deg(factorizedRotation.angle(rotation));

						if (translationDifference >= Scalar(0.001) || angleDifference > Scalar(5))
						{
							scopedIterationFactorized.setInaccurate();
						}
					}
					else
					{
						scopedIterationFactorized.setInaccurate();
					}
				}
			}
			else
			{
				scopedIterationFundamental.setInaccurate();
			}

			needMoreIterations = validationFundamental.needMoreIterations() || validationEpipoles.needMoreIterations() || validationEssential.needMoreIterations() || validationFactorized.needMoreIterations();
		}
		while (needMoreIterations || !startTimestamp.hasTimePassed(testDuration));

		Log::info() << "Performance: " << performance;
		Log::info() << "Validation Fundamental: " << validationFundamental;
		Log::info() << "Validation Epipoles: " << validationEpipoles;
		Log::info() << "Validation Essential: " << validationEssential;
		Log::info() << "Validation Factorized: " << validationFactorized;

		if (!validationFundamental.succeeded())
		{
			allSucceeded = false;
		}

		if (!validationEpipoles.succeeded())
		{
			allSucceeded = false;
		}

		if (!validationEssential.succeeded())
		{
			allSucceeded = false;
		}

		if (!validationFactorized.succeeded())
		{
			allSucceeded = false;
		}
	}

	return allSucceeded;
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

			Vectors2 leftImagePoints;
			Vectors3 leftObjectPoints;

			for (unsigned int i = 0u; i < points; ++i)
			{
				const Vector2 imagePoint(Random::scalar(randomGenerator, 0, Scalar(pinholeCamera.width())), Random::scalar(randomGenerator, 0, Scalar(pinholeCamera.height())));
				const Vector3 objectPoint(pinholeCamera.vector(imagePoint) * Random::scalar(randomGenerator, Scalar(0.1), Scalar(2.0)));

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

			Vectors2 rightImagePoints;
			Vectors3 rightObjectPoints;

			for (size_t i = 0; i < leftImagePoints.size(); ++i)
			{
				const Vector3 objectPoint(rTl * leftObjectPoints[i]);
				const Vector2 imagePoint(pinholeCamera.projectToImage<true>(HomogenousMatrix4(true), objectPoint, false));

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
				if (Ocean::Geometry::EpipolarGeometry::factorizeEssential(essential, pinholeCamera, pinholeCamera, leftImagePoints.data(), rightImagePoints.data(), leftImagePoints.size(), factorizedTransformation))
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

			Vectors2 leftImagePoints;
			Vectors3 leftObjectPoints;

			for (unsigned int i = 0u; i < points; ++i)
			{
				const Vector2 imagePoint(Random::scalar(randomGenerator, Scalar(0), Scalar(pinholeCamera.width() - 1)), Random::scalar(randomGenerator, Scalar(0), Scalar(pinholeCamera.height() - 1)));
				const Vector3 objectPoint(pinholeCamera.vector(imagePoint) * Random::scalar(randomGenerator, Scalar(1), Scalar(3)));

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

			Vectors2 rightImagePoints;
			Vectors3 rightObjectPoints;

			for (size_t i = 0; i < leftImagePoints.size(); ++i)
			{
				const Vector3 objectPoint(rTl * leftObjectPoints[i]);
				const Vector2 imagePoint(pinholeCamera.projectToImage<true>(HomogenousMatrix4(true), objectPoint, false));

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
			if (Geometry::RANSAC::fundamentalMatrix(leftImagePoints.data(), rightImagePoints.data(), leftImagePoints.size(), pinholeCamera.width(), pinholeCamera.height(), fundamental))
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
						{
							noError = false;
						}
					}
				}

				HomogenousMatrix4 factorizedTransformation;
				if (Ocean::Geometry::EpipolarGeometry::factorizeEssential(essential, pinholeCamera, pinholeCamera, leftImagePoints.data(), rightImagePoints.data(), leftImagePoints.size(), factorizedTransformation))
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

			const Vectors3 triangulatedObjectPoints = Geometry::EpipolarGeometry::triangulateImagePoints(world_T_cameraA, world_T_cameraB, *anyCamera, *anyCamera, imagePointsA.data(), imagePointsB.data(), imagePointsA.size());

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
