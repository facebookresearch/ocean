/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testtracking/testslam/TestLocalizedObjectPoint.h"

#include "ocean/tracking/slam/LocalizedObjectPoint.h"
#include "ocean/tracking/slam/PointTrack.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Random.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestTracking
{

namespace TestSLAM
{

bool TestLocalizedObjectPoint::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("LocalizedObjectPoint test");

	Log::info() << " ";

	if (selector.shouldRun("constructor"))
	{
		testResult = testConstructor(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("constructorwithposition"))
	{
		testResult = testConstructorWithPosition(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("addobservation"))
	{
		testResult = testAddObservation(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("removeobservation"))
	{
		testResult = testRemoveObservation(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("hasobservation"))
	{
		testResult = testHasObservation(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("observation"))
	{
		testResult = testObservation(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("position"))
	{
		testResult = testPosition(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestLocalizedObjectPoint, Constructor)
{
	EXPECT_TRUE(TestLocalizedObjectPoint::testConstructor(GTEST_TEST_DURATION));
}

TEST(TestLocalizedObjectPoint, ConstructorWithPosition)
{
	EXPECT_TRUE(TestLocalizedObjectPoint::testConstructorWithPosition(GTEST_TEST_DURATION));
}

TEST(TestLocalizedObjectPoint, AddObservation)
{
	EXPECT_TRUE(TestLocalizedObjectPoint::testAddObservation(GTEST_TEST_DURATION));
}

TEST(TestLocalizedObjectPoint, RemoveObservation)
{
	EXPECT_TRUE(TestLocalizedObjectPoint::testRemoveObservation(GTEST_TEST_DURATION));
}

TEST(TestLocalizedObjectPoint, HasObservation)
{
	EXPECT_TRUE(TestLocalizedObjectPoint::testHasObservation(GTEST_TEST_DURATION));
}

TEST(TestLocalizedObjectPoint, Observation)
{
	EXPECT_TRUE(TestLocalizedObjectPoint::testObservation(GTEST_TEST_DURATION));
}

TEST(TestLocalizedObjectPoint, Position)
{
	EXPECT_TRUE(TestLocalizedObjectPoint::testPosition(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestLocalizedObjectPoint::testConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Constructor test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		// create a PointTrack with random observations
		const Index32 firstFrameIndex = RandomI::random(randomGenerator, 0u, 1000u);
		const size_t numberObservations = size_t(RandomI::random(randomGenerator, 2u, 100u));

		Vectors2 imagePoints;
		imagePoints.reserve(numberObservations);

		for (size_t n = 0; n < numberObservations; ++n)
		{
			imagePoints.emplace_back(Random::vector2(randomGenerator, 0, 1920, 0, 1080));
		}

		Tracking::SLAM::PointTrack pointTrack(firstFrameIndex, std::move(imagePoints));

		OCEAN_EXPECT_TRUE(validation, pointTrack.isValid());

		Tracking::SLAM::LocalizedObjectPoint localizedObjectPoint(pointTrack);

		// verify position is not yet valid
		OCEAN_EXPECT_EQUAL(validation, localizedObjectPoint.position(), Vector3::minValue());

		// verify localization precision is invalid initially
		OCEAN_EXPECT_EQUAL(validation, localizedObjectPoint.localizationPrecision(), Tracking::SLAM::LocalizedObjectPoint::LP_INVALID);

		// verify number of observations matches
		OCEAN_EXPECT_EQUAL(validation, localizedObjectPoint.numberObservations(), pointTrack.numberObservations());

		// verify last observation frame index
		OCEAN_EXPECT_EQUAL(validation, localizedObjectPoint.lastObservationFrameIndex(), pointTrack.lastFrameIndex());

		// verify all observations are present
		for (size_t n = 0; n < pointTrack.numberObservations(); ++n)
		{
			const Index32 frameIndex = firstFrameIndex + Index32(n);
			OCEAN_EXPECT_TRUE(validation, localizedObjectPoint.hasObservation(frameIndex));
		}

		// verify descriptors are empty
		OCEAN_EXPECT_TRUE(validation, localizedObjectPoint.descriptors().empty());
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestLocalizedObjectPoint::testConstructorWithPosition(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Constructor with position test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const Index32 firstFrameIndex = RandomI::random(randomGenerator, 0u, 1000u);

		Vectors2 imagePoints;
		imagePoints.emplace_back(Random::vector2(randomGenerator, 0, 1920, 0, 1080));
		imagePoints.emplace_back(Random::vector2(randomGenerator, 0, 1920, 0, 1080));

		Tracking::SLAM::PointTrack pointTrack(firstFrameIndex, std::move(imagePoints));

		const Vector3 position = Random::vector3(randomGenerator, -100, 100);

		const Tracking::SLAM::LocalizedObjectPoint::LocalizationPrecision precision = Tracking::SLAM::LocalizedObjectPoint::LocalizationPrecision(RandomI::random(randomGenerator, 1u, 4u));

		const bool isBundleAdjusted = RandomI::boolean(randomGenerator);

		Tracking::SLAM::LocalizedObjectPoint localizedObjectPoint(pointTrack, position, precision, isBundleAdjusted);

		OCEAN_EXPECT_EQUAL(validation, localizedObjectPoint.position(), position);

		OCEAN_EXPECT_EQUAL(validation, localizedObjectPoint.localizationPrecision(), precision);

		OCEAN_EXPECT_EQUAL(validation, localizedObjectPoint.numberObservations(), size_t(2));

		OCEAN_EXPECT_EQUAL(validation, localizedObjectPoint.isBundleAdjusted(), isBundleAdjusted);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestLocalizedObjectPoint::testAddObservation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "AddObservation test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const Index32 firstFrameIndex = RandomI::random(randomGenerator, 0u, 1000u);

		Vectors2 imagePoints;
		imagePoints.emplace_back(Random::vector2(randomGenerator, 0, 1920, 0, 1080));
		imagePoints.emplace_back(Random::vector2(randomGenerator, 0, 1920, 0, 1080));

		Tracking::SLAM::PointTrack pointTrack(firstFrameIndex, std::move(imagePoints));
		Tracking::SLAM::LocalizedObjectPoint localizedObjectPoint(pointTrack);

		const size_t initialObservations = localizedObjectPoint.numberObservations();

		// add new observations
		const unsigned int additionalObservations = RandomI::random(randomGenerator, 1u, 10u);

		Vectors2 addedImagePoints;
		addedImagePoints.reserve(additionalObservations);

		for (unsigned int n = 0u; n < additionalObservations; ++n)
		{
			const Index32 newFrameIndex = localizedObjectPoint.lastObservationFrameIndex() + 1u;
			const Vector2 newImagePoint = Random::vector2(randomGenerator, 0, 1920, 0, 1080);

			addedImagePoints.push_back(newImagePoint);

			localizedObjectPoint.addObservation(newFrameIndex, newImagePoint);

			OCEAN_EXPECT_TRUE(validation, localizedObjectPoint.hasObservation(newFrameIndex));
			OCEAN_EXPECT_EQUAL(validation, localizedObjectPoint.lastObservationFrameIndex(), newFrameIndex);
		}

		OCEAN_EXPECT_EQUAL(validation, localizedObjectPoint.numberObservations(), initialObservations + additionalObservations);

		// verify that all added 2D image points can be retrieved correctly
		const Index32 firstAddedFrameIndex = firstFrameIndex + Index32(initialObservations);

		for (unsigned int n = 0u; n < additionalObservations; ++n)
		{
			const Index32 frameIndex = firstAddedFrameIndex + n;

			Vector2 retrievedImagePoint;
			OCEAN_EXPECT_TRUE(validation, localizedObjectPoint.hasObservation(frameIndex, &retrievedImagePoint));
			OCEAN_EXPECT_EQUAL(validation, retrievedImagePoint, addedImagePoints[n]);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestLocalizedObjectPoint::testRemoveObservation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "RemoveObservation test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const Index32 firstFrameIndex = RandomI::random(randomGenerator, 0u, 1000u);

		Vectors2 imagePoints;
		for (size_t n = 0; n < 5; ++n)
		{
			imagePoints.emplace_back(Random::vector2(randomGenerator, 0, 1920, 0, 1080));
		}

		Vectors2 imagePointsCopy = imagePoints;

		Tracking::SLAM::PointTrack pointTrack(firstFrameIndex, std::move(imagePoints));
		Tracking::SLAM::LocalizedObjectPoint localizedObjectPoint(pointTrack);

		const size_t initialObservations = localizedObjectPoint.numberObservations();

		// remove an observation
		const Index32 frameToRemove = firstFrameIndex + 2u;
		OCEAN_EXPECT_TRUE(validation, localizedObjectPoint.hasObservation(frameToRemove));

		localizedObjectPoint.removeObservation(frameToRemove);

		OCEAN_EXPECT_FALSE(validation, localizedObjectPoint.hasObservation(frameToRemove));
		OCEAN_EXPECT_EQUAL(validation, localizedObjectPoint.numberObservations(), initialObservations - 1);

		// verify that all remaining observations return the correct 2D image points
		for (size_t n = 0; n < imagePointsCopy.size(); ++n)
		{
			const Index32 frameIndex = firstFrameIndex + Index32(n);

			if (frameIndex == frameToRemove)
			{
				continue;
			}

			Vector2 retrievedImagePoint;
			OCEAN_EXPECT_TRUE(validation, localizedObjectPoint.hasObservation(frameIndex, &retrievedImagePoint));
			OCEAN_EXPECT_EQUAL(validation, retrievedImagePoint, imagePointsCopy[n]);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestLocalizedObjectPoint::testHasObservation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "HasObservation test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const Index32 firstFrameIndex = RandomI::random(randomGenerator, 10u, 1000u);

		const Vector2 imagePoint0 = Random::vector2(randomGenerator, 0, 1920, 0, 1080);
		const Vector2 imagePoint1 = Random::vector2(randomGenerator, 0, 1920, 0, 1080);

		Vectors2 imagePoints;
		imagePoints.push_back(imagePoint0);
		imagePoints.push_back(imagePoint1);

		Tracking::SLAM::PointTrack pointTrack(firstFrameIndex, std::move(imagePoints));
		Tracking::SLAM::LocalizedObjectPoint localizedObjectPoint(pointTrack);

		// test hasObservation with output parameter
		Vector2 retrievedImagePoint;

		OCEAN_EXPECT_TRUE(validation, localizedObjectPoint.hasObservation(firstFrameIndex, &retrievedImagePoint));
		OCEAN_EXPECT_EQUAL(validation, retrievedImagePoint, imagePoint0);

		OCEAN_EXPECT_TRUE(validation, localizedObjectPoint.hasObservation(firstFrameIndex + 1u, &retrievedImagePoint));
		OCEAN_EXPECT_EQUAL(validation, retrievedImagePoint, imagePoint1);

		// test hasObservation for non-existing frame
		OCEAN_EXPECT_FALSE(validation, localizedObjectPoint.hasObservation(firstFrameIndex - 1u));
		OCEAN_EXPECT_FALSE(validation, localizedObjectPoint.hasObservation(firstFrameIndex + 2u));
		OCEAN_EXPECT_FALSE(validation, localizedObjectPoint.hasObservation(0u));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestLocalizedObjectPoint::testObservation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Observation test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const Index32 firstFrameIndex = RandomI::random(randomGenerator, 0u, 1000u);
		const size_t numberObservations = size_t(RandomI::random(randomGenerator, 2u, 20u));

		Vectors2 imagePoints;
		imagePoints.reserve(numberObservations);

		for (size_t n = 0; n < numberObservations; ++n)
		{
			imagePoints.emplace_back(Random::vector2(randomGenerator, 0, 1920, 0, 1080));
		}

		Vectors2 imagePointsCopy = imagePoints;
		Tracking::SLAM::PointTrack pointTrack(firstFrameIndex, std::move(imagePoints));
		Tracking::SLAM::LocalizedObjectPoint localizedObjectPoint(pointTrack);

		// test observation() returns correct image points
		for (size_t n = 0; n < numberObservations; ++n)
		{
			const Index32 frameIndex = firstFrameIndex + Index32(n);
			const Vector2 observation = localizedObjectPoint.observation(frameIndex);

			OCEAN_EXPECT_EQUAL(validation, observation, imagePointsCopy[n]);
		}

		// test lastObservation()
		const Tracking::SLAM::Observation lastObservation = localizedObjectPoint.lastObservation();
		OCEAN_EXPECT_EQUAL(validation, lastObservation.frameIndex(), firstFrameIndex + Index32(numberObservations) - 1u);
		OCEAN_EXPECT_EQUAL(validation, lastObservation.imagePoint(), imagePointsCopy.back());
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestLocalizedObjectPoint::testPosition(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Position test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const Index32 firstFrameIndex = 0u;

		Vectors2 imagePoints;
		imagePoints.emplace_back(100, 100);
		imagePoints.emplace_back(200, 200);

		Tracking::SLAM::PointTrack pointTrack(firstFrameIndex, std::move(imagePoints));
		Tracking::SLAM::LocalizedObjectPoint localizedObjectPoint(pointTrack);

		// initial position should be invalid
		OCEAN_EXPECT_EQUAL(validation, localizedObjectPoint.position(), Vector3::minValue());

		// set position and verify
		const Vector3 newPosition(Random::vector3(randomGenerator, -100, 100));
		const bool isBundleAdjusted = RandomI::boolean(randomGenerator);

		localizedObjectPoint.setPosition(newPosition, isBundleAdjusted);

		OCEAN_EXPECT_EQUAL(validation, localizedObjectPoint.position(), newPosition);
		OCEAN_EXPECT_EQUAL(validation, localizedObjectPoint.isBundleAdjusted(), isBundleAdjusted);

		// set another position
		const Vector3 anotherPosition(Random::vector3(randomGenerator, -100, 100));
		const bool anotherIsBundleAdjusted = RandomI::boolean(randomGenerator);

		localizedObjectPoint.setPosition(anotherPosition, anotherIsBundleAdjusted);

		OCEAN_EXPECT_EQUAL(validation, localizedObjectPoint.position(), anotherPosition);
		OCEAN_EXPECT_EQUAL(validation, localizedObjectPoint.isBundleAdjusted(), anotherIsBundleAdjusted);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}

}
