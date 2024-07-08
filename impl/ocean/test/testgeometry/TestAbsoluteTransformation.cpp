/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestAbsoluteTransformation.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/geometry/AbsoluteTransformation.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Random.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Vector3.h"

#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

bool TestAbsoluteTransformation::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Tests the Absolute Transformation:   ---";
	Log::info() << " ";

	allSucceeded = testAbsoluteTransformationBasedOnPoints(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAbsoluteTransformationBasedOnTransformations(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAbsoluteTransformationBasedOnTransformationsWithOutliers(testDuration) && allSucceeded;

	if (allSucceeded)
	{
		Log::info() << "Absolute Transformation test succeeded.";
	}
	else
	{
		Log::info() << "Absolute Transformation test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestAbsoluteTransformation, AbsoluteTransformationBasedOnPoints)
{
	EXPECT_TRUE(TestAbsoluteTransformation::testAbsoluteTransformationBasedOnPoints(GTEST_TEST_DURATION));
}

TEST(TestAbsoluteTransformation, AbsoluteTransformationBasedOnTransformations)
{
	EXPECT_TRUE(TestAbsoluteTransformation::testAbsoluteTransformationBasedOnTransformations(GTEST_TEST_DURATION));
}

TEST(TestAbsoluteTransformation, AbsoluteTransformationBasedOnTransformationsWithOutliers)
{
	EXPECT_TRUE(TestAbsoluteTransformation::testAbsoluteTransformationBasedOnTransformationsWithOutliers(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestAbsoluteTransformation::testAbsoluteTransformationBasedOnPoints(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const std::vector<Geometry::AbsoluteTransformation::ScaleErrorType> scaleErrorTypes =
	{
		Geometry::AbsoluteTransformation::ScaleErrorType::RightBiased,
		Geometry::AbsoluteTransformation::ScaleErrorType::LeftBiased,
		Geometry::AbsoluteTransformation::ScaleErrorType::Symmetric
	};

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	for (const bool withScale : {false, true})
	{
		if (withScale)
		{
			Log::info() << "Testing 7-DOF absolute transformation based on 3D points (with scale)";
		}
		else
		{
			Log::info() << "Testing 6-DOF absolute transformation based on 3D points (without scale)";
		}

		Log::info().newLine();

		for (const size_t numberPoints : {10u, 15u, 25u, 500u, 5000u})
		{
			Log::info() << "Testing " << numberPoints << " points:";

			ValidationPrecision validation(0.95, randomGenerator);

			HighPerformanceStatistic performance;

			const Timestamp startTimestamp(true);

			do
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const Vector3 randomTranslation(Random::vector3(randomGenerator, -100, 100));
				const Quaternion randomRotation(Random::quaternion(randomGenerator));

				HomogenousMatrix4 right_T_left(randomTranslation, randomRotation);

				Scalar randomScale = Scalar(0);

				if (withScale)
				{
					randomScale = Random::scalar(randomGenerator, Scalar(0.1), Scalar(1.9));

					right_T_left.applyScale(Vector3(randomScale, randomScale, randomScale));
				}

				Vectors3 pointsLeft(numberPoints);
				Vectors3 pointsRight(numberPoints);

				for (size_t n = 0; n < numberPoints; ++n)
				{
					pointsLeft[n] = Random::vector3(randomGenerator, -100, 100);
					pointsRight[n] = right_T_left * pointsLeft[n];
				}

				HomogenousMatrix4 result_right_T_left(false);
				Scalar resultScale(0);

				const Geometry::AbsoluteTransformation::ScaleErrorType scaleErrorType = RandomI::random(randomGenerator, scaleErrorTypes);

				performance.start();
					const bool localResult = Geometry::AbsoluteTransformation::calculateTransformation(pointsLeft.data(), pointsRight.data(), numberPoints, result_right_T_left, scaleErrorType, withScale ? &resultScale : nullptr);
				performance.stop();

				if (localResult)
				{
					// let's verify the results

					const Vector3 resultTranslation = result_right_T_left.translation();
					const Quaternion resultRotation = result_right_T_left.rotation();

					const Scalar xOffset = Numeric::abs(randomTranslation.x() - resultTranslation.x());
					const Scalar yOffset = Numeric::abs(randomTranslation.y() - resultTranslation.y());
					const Scalar zOffset = Numeric::abs(randomTranslation.z() - resultTranslation.z());
					const bool validTranslation = xOffset < Scalar(0.1) && yOffset < Scalar(0.1) && zOffset < Scalar(0.1);

					if (!validTranslation)
					{
						scopedIteration.setInaccurate();
					}

					const Scalar aOffset = Numeric::rad2deg(Numeric::angleAdjustNull(randomRotation.angle(resultRotation)));
					const bool validRotation = aOffset < 2;

					if (!validRotation)
					{
						scopedIteration.setInaccurate();
					}

					const Scalar sOffset = Numeric::abs(randomScale - resultScale);
					const bool validScale = !withScale || sOffset < (Scalar(0.1) * randomScale);

					if (!validScale)
					{
						scopedIteration.setInaccurate();
					}
				}
				else
				{
					OCEAN_SET_FAILED(validation);
				}
			}
			while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

			Log::info() << "Performance: " << performance;

			Log::info() << "Validation: " << validation;
			Log::info().newLine();

			if (!validation.succeeded())
			{
				allSucceeded = false;
			}
		}

		Log::info().newLine();
	}

	return allSucceeded;
}

bool TestAbsoluteTransformation::testAbsoluteTransformationBasedOnTransformations(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const std::vector<Geometry::AbsoluteTransformation::ScaleErrorType> scaleErrorTypes =
	{
		Geometry::AbsoluteTransformation::ScaleErrorType::RightBiased,
		Geometry::AbsoluteTransformation::ScaleErrorType::LeftBiased,
		Geometry::AbsoluteTransformation::ScaleErrorType::Symmetric
	};

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	for (const bool withScale : {false, true})
	{
		if (withScale)
		{
			Log::info() << "Testing 7-DOF absolute transformation based on 6-DOF transformations (with scale)";
		}
		else
		{
			Log::info() << "Testing 6-DOF absolute transformation based on 6-DOF transformations (without scale)";
		}

		Log::info().newLine();

		for (const size_t numberTransformations : {10u, 15u, 25u, 500u, 5000u})
		{
			Log::info() << "Testing " << numberTransformations << " transformations:";

			ValidationPrecision validation(0.95, randomGenerator);

			HighPerformanceStatistic performance;

			const Timestamp startTimestamp(true);

			do
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const Vector3 randomTranslation(Random::vector3(randomGenerator, -100, 100));
				const Quaternion randomRotation(Random::quaternion(randomGenerator));

				HomogenousMatrix4 rightWorld_T_leftWorld(randomTranslation, randomRotation);

				Scalar randomScale = Scalar(0);

				if (withScale)
				{
					randomScale = Random::scalar(randomGenerator, Scalar(0.1), Scalar(1.9));

					rightWorld_T_leftWorld.applyScale(Vector3(randomScale, randomScale, randomScale));
				}

				HomogenousMatrices4 leftWorld_T_transformations(numberTransformations);
				HomogenousMatrices4 rightWorld_T_transformations(numberTransformations);

				for (size_t n = 0; n < numberTransformations; ++n)
				{
					leftWorld_T_transformations[n] = HomogenousMatrix4(Random::vector3(randomGenerator, -100, 100), Random::quaternion(randomGenerator));
					rightWorld_T_transformations[n] = rightWorld_T_leftWorld * leftWorld_T_transformations[n];
				}

				HomogenousMatrix4 estimatedRightWorld_T_estimatedLeftWorld(false);
				Scalar resultScale(0);

				const Geometry::AbsoluteTransformation::ScaleErrorType scaleErrorType = RandomI::random(randomGenerator, scaleErrorTypes);

				performance.start();
					const bool localResult = Geometry::AbsoluteTransformation::calculateTransformation(leftWorld_T_transformations.data(), rightWorld_T_transformations.data(), numberTransformations, estimatedRightWorld_T_estimatedLeftWorld, scaleErrorType, withScale ? &resultScale : nullptr);
				performance.stop();

				if (localResult)
				{
					// let's verify the results

					const Vector3 resultTranslation = estimatedRightWorld_T_estimatedLeftWorld.translation();
					const Quaternion resultRotation = estimatedRightWorld_T_estimatedLeftWorld.rotation();

					const Scalar xOffset = Numeric::abs(randomTranslation.x() - resultTranslation.x());
					const Scalar yOffset = Numeric::abs(randomTranslation.y() - resultTranslation.y());
					const Scalar zOffset = Numeric::abs(randomTranslation.z() - resultTranslation.z());
					const bool validTranslation = xOffset < Scalar(0.1) && yOffset < Scalar(0.1) && zOffset < Scalar(0.1);

					if (!validTranslation)
					{
						scopedIteration.setInaccurate();
					}

					const Scalar aOffset = Numeric::rad2deg(Numeric::angleAdjustNull(randomRotation.angle(resultRotation)));
					const bool validRotation = aOffset < 2;

					if (!validRotation)
					{
						scopedIteration.setInaccurate();
					}

					const Scalar sOffset = Numeric::abs(randomScale - resultScale);
					const bool validScale = !withScale || sOffset < (Scalar(0.1) * randomScale);

					if (!validScale)
					{
						scopedIteration.setInaccurate();
					}
				}
				else
				{
					OCEAN_SET_FAILED(validation);
				}
			}
			while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

			Log::info() << "Performance: " << performance;

			Log::info() << "Validation: " << validation;
			Log::info().newLine();

			if (!validation.succeeded())
			{
				allSucceeded = false;
			}
		}

		Log::info().newLine();
	}

	Log::info().newLine();

	return allSucceeded;
}

bool TestAbsoluteTransformation::testAbsoluteTransformationBasedOnTransformationsWithOutliers(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const std::vector<Geometry::AbsoluteTransformation::ScaleErrorType> scaleErrorTypes =
	{
		Geometry::AbsoluteTransformation::ScaleErrorType::RightBiased,
		Geometry::AbsoluteTransformation::ScaleErrorType::LeftBiased,
		Geometry::AbsoluteTransformation::ScaleErrorType::Symmetric
	};

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	for (const bool withScale : {false, true})
	{
		if (withScale)
		{
			Log::info() << "Testing robust 7-DOF absolute transformation based on 6-DOF transformations (with scale)";
		}
		else
		{
			Log::info() << "Testing robust 6-DOF absolute transformation based on 6-DOF transformations (without scale)";
		}

		Log::info().newLine();

		for (const size_t numberTransformations : {10u, 15u, 25u, 500u, 5000u})
		{
			const size_t numberOutliers = numberTransformations * 10 / 100;
			constexpr Scalar inlierRate = Scalar(0.5);

			Log::info() << "Testing " << numberTransformations << " transformations, and " << numberOutliers << " outliers:";

			ValidationPrecision validation(0.95, randomGenerator);

			HighPerformanceStatistic performance;

			const Timestamp startTimestamp(true);

			do
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const Vector3 randomTranslation(Random::vector3(randomGenerator, -100, 100));
				const Quaternion randomRotation(Random::quaternion(randomGenerator));

				HomogenousMatrix4 rightWorld_T_leftWorld(randomTranslation, randomRotation);

				Scalar randomScale = Scalar(0);

				if (withScale)
				{
					randomScale = Random::scalar(randomGenerator, Scalar(0.1), Scalar(1.9));

					rightWorld_T_leftWorld.applyScale(Vector3(randomScale, randomScale, randomScale));
				}

				HomogenousMatrices4 leftWorld_T_transformations(numberTransformations);
				HomogenousMatrices4 rightWorld_T_transformations(numberTransformations);

				for (size_t n = 0; n < numberTransformations; ++n)
				{
					leftWorld_T_transformations[n] = HomogenousMatrix4(Random::vector3(randomGenerator, -100, 100), Random::quaternion(randomGenerator));
					rightWorld_T_transformations[n] = rightWorld_T_leftWorld * leftWorld_T_transformations[n];
				}

				UnorderedIndexSet32 outlierIndices;
				while (outlierIndices.size() < numberOutliers)
				{
					outlierIndices.emplace(RandomI::random(randomGenerator, Index32(numberTransformations) - 1u));
				}

				for (const Index32& outlierIndex : outlierIndices)
				{
					const Vector3 offset = Random::vector3(randomGenerator, Vector3(2, 2, 2));
					rightWorld_T_transformations[outlierIndex].setTranslation(rightWorld_T_transformations[outlierIndex].translation() + offset);
				}

				HomogenousMatrix4 estimatedRightWorld_T_estimatedLeftWorld(false);
				Scalar resultScale(0);

				const Geometry::AbsoluteTransformation::ScaleErrorType scaleErrorType = RandomI::random(randomGenerator, scaleErrorTypes);

				performance.start();
					const bool localResult = Geometry::AbsoluteTransformation::calculateTransformationWithOutliers(leftWorld_T_transformations.data(), rightWorld_T_transformations.data(), numberTransformations, estimatedRightWorld_T_estimatedLeftWorld, inlierRate, scaleErrorType, withScale ? &resultScale : nullptr);
				performance.stop();

				if (localResult)
				{
					// let's verify the results

					const Vector3 resultTranslation = estimatedRightWorld_T_estimatedLeftWorld.translation();
					const Quaternion resultRotation = estimatedRightWorld_T_estimatedLeftWorld.rotation();

					const Scalar xOffset = Numeric::abs(randomTranslation.x() - resultTranslation.x());
					const Scalar yOffset = Numeric::abs(randomTranslation.y() - resultTranslation.y());
					const Scalar zOffset = Numeric::abs(randomTranslation.z() - resultTranslation.z());
					const bool validTranslation = xOffset < Scalar(0.1) && yOffset < Scalar(0.1) && zOffset < Scalar(0.1);

					if (!validTranslation)
					{
						scopedIteration.setInaccurate();
					}

					const Scalar aOffset = Numeric::rad2deg(Numeric::angleAdjustNull(randomRotation.angle(resultRotation)));
					const bool validRotation = aOffset < 2;

					if (!validRotation)
					{
						scopedIteration.setInaccurate();
					}

					const Scalar sOffset = Numeric::abs(randomScale - resultScale);
					const bool validScale = !withScale || sOffset < (Scalar(0.1) * randomScale);

					if (!validScale)
					{
						scopedIteration.setInaccurate();
					}
				}
				else
				{
					OCEAN_SET_FAILED(validation);
				}
			}
			while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

			Log::info() << "Performance: " << performance;

			Log::info() << "Validation: " << validation;
			Log::info().newLine();

			if (!validation.succeeded())
			{
				allSucceeded = false;
			}
		}

		Log::info().newLine();
	}

	Log::info().newLine();

	return allSucceeded;
}

}

}

}
