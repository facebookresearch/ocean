/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testtracking/testslam/TestFramePyramidManager.h"

#include "ocean/tracking/slam/FramePyramidManager.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FramePyramid.h"

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

bool TestFramePyramidManager::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("FramePyramidManager test");

	Log::info() << " ";

	if (selector.shouldRun("newpyramid"))
	{
		testResult = testNewPyramid(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("existingpyramid"))
	{
		testResult = testExistingPyramid(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("latestpyramid"))
	{
		testResult = testLatestPyramid(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("updatelatest"))
	{
		testResult = testUpdateLatest(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("scopedpyramid"))
	{
		testResult = testScopedPyramid(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("idealpyramidlayers"))
	{
		testResult = testIdealPyramidLayers(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestFramePyramidManager, NewPyramid)
{
	EXPECT_TRUE(TestFramePyramidManager::testNewPyramid(GTEST_TEST_DURATION));
}

TEST(TestFramePyramidManager, ExistingPyramid)
{
	EXPECT_TRUE(TestFramePyramidManager::testExistingPyramid(GTEST_TEST_DURATION));
}

TEST(TestFramePyramidManager, LatestPyramid)
{
	EXPECT_TRUE(TestFramePyramidManager::testLatestPyramid(GTEST_TEST_DURATION));
}

TEST(TestFramePyramidManager, UpdateLatest)
{
	EXPECT_TRUE(TestFramePyramidManager::testUpdateLatest(GTEST_TEST_DURATION));
}

TEST(TestFramePyramidManager, ScopedPyramid)
{
	EXPECT_TRUE(TestFramePyramidManager::testScopedPyramid(GTEST_TEST_DURATION));
}

TEST(TestFramePyramidManager, IdealPyramidLayers)
{
	EXPECT_TRUE(TestFramePyramidManager::testIdealPyramidLayers(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestFramePyramidManager::testNewPyramid(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "NewPyramid test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		Tracking::SLAM::FramePyramidManager manager;

		// initially, the manager should have no pyramids
		OCEAN_EXPECT_EQUAL(validation, manager.size(), size_t(0));

		// create a new pyramid for a random frame index
		const Index32 frameIndex = RandomI::random(randomGenerator, 0u, 1000u);

		Tracking::SLAM::FramePyramidManager::ScopedPyramid scopedPyramid = manager.newPyramid(frameIndex);

		// the scoped pyramid should be valid
		OCEAN_EXPECT_TRUE(validation, scopedPyramid.isValid());
		OCEAN_EXPECT_TRUE(validation, bool(scopedPyramid));

		// verify the frame index
		OCEAN_EXPECT_EQUAL(validation, scopedPyramid.frameIndex(), frameIndex);

		// manager should have one pyramid now
		OCEAN_EXPECT_EQUAL(validation, manager.size(), size_t(1));

		// create another pyramid with a different frame index
		const Index32 frameIndex2 = frameIndex + 1u;

		Tracking::SLAM::FramePyramidManager::ScopedPyramid scopedPyramid2 = manager.newPyramid(frameIndex2);

		OCEAN_EXPECT_TRUE(validation, scopedPyramid2.isValid());
		OCEAN_EXPECT_EQUAL(validation, scopedPyramid2.frameIndex(), frameIndex2);

		// manager should have two pyramids now
		OCEAN_EXPECT_EQUAL(validation, manager.size(), size_t(2));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestFramePyramidManager::testExistingPyramid(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "ExistingPyramid test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		Tracking::SLAM::FramePyramidManager manager;

		// create a new pyramid
		const Index32 frameIndex = RandomI::random(randomGenerator, 0u, 1000u);

		{
			Tracking::SLAM::FramePyramidManager::ScopedPyramid newPyramid = manager.newPyramid(frameIndex);
			OCEAN_EXPECT_TRUE(validation, newPyramid.isValid());

			// now try to get the existing pyramid (while the first one is still in scope)
			Tracking::SLAM::FramePyramidManager::ScopedPyramid existingPyramid = manager.existingPyramid(frameIndex);
			OCEAN_EXPECT_TRUE(validation, existingPyramid.isValid());
			OCEAN_EXPECT_EQUAL(validation, existingPyramid.frameIndex(), frameIndex);

			// both should reference the same frame index
			OCEAN_EXPECT_EQUAL(validation, newPyramid.frameIndex(), existingPyramid.frameIndex());
		}

		// create multiple pyramids and verify we can get each one
		{
			const Index32 frameIndex1 = RandomI::random(randomGenerator, 0u, 500u);
			const Index32 frameIndex2 = frameIndex1 + 1u;
			const Index32 frameIndex3 = frameIndex1 + 2u;

			Tracking::SLAM::FramePyramidManager::ScopedPyramid pyramid1 = manager.newPyramid(frameIndex1);
			Tracking::SLAM::FramePyramidManager::ScopedPyramid pyramid2 = manager.newPyramid(frameIndex2);
			Tracking::SLAM::FramePyramidManager::ScopedPyramid pyramid3 = manager.newPyramid(frameIndex3);

			OCEAN_EXPECT_TRUE(validation, pyramid1.isValid());
			OCEAN_EXPECT_TRUE(validation, pyramid2.isValid());
			OCEAN_EXPECT_TRUE(validation, pyramid3.isValid());

			// verify we can retrieve each one
			Tracking::SLAM::FramePyramidManager::ScopedPyramid existing1 = manager.existingPyramid(frameIndex1);
			Tracking::SLAM::FramePyramidManager::ScopedPyramid existing2 = manager.existingPyramid(frameIndex2);
			Tracking::SLAM::FramePyramidManager::ScopedPyramid existing3 = manager.existingPyramid(frameIndex3);

			OCEAN_EXPECT_TRUE(validation, existing1.isValid());
			OCEAN_EXPECT_TRUE(validation, existing2.isValid());
			OCEAN_EXPECT_TRUE(validation, existing3.isValid());

			OCEAN_EXPECT_EQUAL(validation, existing1.frameIndex(), frameIndex1);
			OCEAN_EXPECT_EQUAL(validation, existing2.frameIndex(), frameIndex2);
			OCEAN_EXPECT_EQUAL(validation, existing3.frameIndex(), frameIndex3);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestFramePyramidManager::testLatestPyramid(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "LatestPyramid test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		Tracking::SLAM::FramePyramidManager manager;

		// initially, there should be no latest pyramid
		{
			Tracking::SLAM::FramePyramidManager::ScopedPyramid latestPyramid = manager.latestPyramid();
			OCEAN_EXPECT_FALSE(validation, latestPyramid.isValid());
		}

		// create a pyramid and set it as latest
		const Index32 frameIndex1 = RandomI::random(randomGenerator, 0u, 1000u);

		{
			Tracking::SLAM::FramePyramidManager::ScopedPyramid pyramid1 = manager.newPyramid(frameIndex1);
			OCEAN_EXPECT_TRUE(validation, pyramid1.isValid());

			manager.updateLatest(frameIndex1);

			// verify latest pyramid
			Tracking::SLAM::FramePyramidManager::ScopedPyramid latestPyramid = manager.latestPyramid();
			OCEAN_EXPECT_TRUE(validation, latestPyramid.isValid());
			OCEAN_EXPECT_EQUAL(validation, latestPyramid.frameIndex(), frameIndex1);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestFramePyramidManager::testUpdateLatest(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "UpdateLatest test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		Tracking::SLAM::FramePyramidManager manager;

		// create multiple pyramids
		const Index32 frameIndex1 = RandomI::random(randomGenerator, 0u, 500u);
		const Index32 frameIndex2 = frameIndex1 + 1u;
		const Index32 frameIndex3 = frameIndex1 + 2u;

		Tracking::SLAM::FramePyramidManager::ScopedPyramid pyramid1 = manager.newPyramid(frameIndex1);
		Tracking::SLAM::FramePyramidManager::ScopedPyramid pyramid2 = manager.newPyramid(frameIndex2);
		Tracking::SLAM::FramePyramidManager::ScopedPyramid pyramid3 = manager.newPyramid(frameIndex3);

		OCEAN_EXPECT_TRUE(validation, pyramid1.isValid());
		OCEAN_EXPECT_TRUE(validation, pyramid2.isValid());
		OCEAN_EXPECT_TRUE(validation, pyramid3.isValid());

		// set pyramid1 as latest
		manager.updateLatest(frameIndex1);

		{
			Tracking::SLAM::FramePyramidManager::ScopedPyramid latestPyramid = manager.latestPyramid();
			OCEAN_EXPECT_TRUE(validation, latestPyramid.isValid());
			OCEAN_EXPECT_EQUAL(validation, latestPyramid.frameIndex(), frameIndex1);
		}

		// update to pyramid2
		manager.updateLatest(frameIndex2);

		{
			Tracking::SLAM::FramePyramidManager::ScopedPyramid latestPyramid = manager.latestPyramid();
			OCEAN_EXPECT_TRUE(validation, latestPyramid.isValid());
			OCEAN_EXPECT_EQUAL(validation, latestPyramid.frameIndex(), frameIndex2);
		}

		// update to pyramid3
		manager.updateLatest(frameIndex3);

		{
			Tracking::SLAM::FramePyramidManager::ScopedPyramid latestPyramid = manager.latestPyramid();
			OCEAN_EXPECT_TRUE(validation, latestPyramid.isValid());
			OCEAN_EXPECT_EQUAL(validation, latestPyramid.frameIndex(), frameIndex3);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestFramePyramidManager::testScopedPyramid(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "ScopedPyramid RAII test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		Tracking::SLAM::FramePyramidManager manager;

		const Index32 frameIndex = RandomI::random(randomGenerator, 0u, 1000u);

		// test that pyramid is released when scoped object goes out of scope
		{
			Tracking::SLAM::FramePyramidManager::ScopedPyramid scopedPyramid = manager.newPyramid(frameIndex);
			OCEAN_EXPECT_TRUE(validation, scopedPyramid.isValid());
			OCEAN_EXPECT_EQUAL(validation, manager.size(), size_t(1));

			// test bool conversion operator
			OCEAN_EXPECT_TRUE(validation, bool(scopedPyramid));

			// test pyramid access via pyramid() method
			CV::FramePyramid& pyramidA = scopedPyramid.pyramid();
			OCEAN_EXPECT_TRUE(validation, pyramidA.layers() == 0u || pyramidA.layers() >= 1u);

			// test pyramid access via dereference operator
			CV::FramePyramid& pyramidB = *scopedPyramid;
			OCEAN_EXPECT_TRUE(validation, pyramidB.layers() == 0u || pyramidB.layers() >= 1u);

			// test arrow operator
			CV::FramePyramid* pyramidC = scopedPyramid.operator->();
			OCEAN_EXPECT_TRUE(validation, pyramidC->layers() == 0u || pyramidC->layers() >= 1u);
		}

		// test move semantics
		{
			Tracking::SLAM::FramePyramidManager::ScopedPyramid scopedPyramid1 = manager.newPyramid(frameIndex);
			OCEAN_EXPECT_TRUE(validation, scopedPyramid1.isValid());

			// move to another scoped pyramid
			Tracking::SLAM::FramePyramidManager::ScopedPyramid scopedPyramid2 = std::move(scopedPyramid1);
			OCEAN_EXPECT_TRUE(validation, scopedPyramid2.isValid());
			OCEAN_EXPECT_EQUAL(validation, scopedPyramid2.frameIndex(), frameIndex);
		}

		// test explicit release
		{
			Tracking::SLAM::FramePyramidManager::ScopedPyramid scopedPyramid = manager.newPyramid(frameIndex);
			OCEAN_EXPECT_TRUE(validation, scopedPyramid.isValid());

			scopedPyramid.release();
			OCEAN_EXPECT_FALSE(validation, scopedPyramid.isValid());
		}

		// test default constructor creates invalid scoped pyramid
		{
			Tracking::SLAM::FramePyramidManager::ScopedPyramid defaultPyramid;
			OCEAN_EXPECT_FALSE(validation, defaultPyramid.isValid());
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestFramePyramidManager::testIdealPyramidLayers(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "IdealPyramidLayers test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		// Use conservative parameters that guarantee no warnings:
		// - Small maximalTrackingDistance (1-2% of diagonal)
		// - Large coarseLayerRadius (16)
		// - Small patch sizes (to allow more pyramid layers)

		const unsigned int width = RandomI::random(randomGenerator, 640u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 480u, 1080u);
		const unsigned int patchSize = RandomI::random(randomGenerator, 5u, 11u);
		const float maximalTrackingDistance = Random::scalar(randomGenerator, Scalar(0.01), Scalar(0.02));
		const unsigned int coarseLayerRadius = 16u;

		const unsigned int layers = Tracking::SLAM::FramePyramidManager::idealPyramidLayers(width, height, patchSize, maximalTrackingDistance, coarseLayerRadius);

		// Verify the result is valid
		OCEAN_EXPECT_GREATER_EQUAL(validation, layers, 1u);
		OCEAN_EXPECT_LESS_EQUAL(validation, layers, 20u);

		// Verify the algorithm correctness:
		// The actual tracking distance must be >= the requested tracking distance (no warning condition)
		const float diagonal = NumericF::sqrt(float(width * width) + float(height * height));
		const unsigned int requestedTrackingDistance = (unsigned int)(diagonal * maximalTrackingDistance + 0.5f);
		const unsigned int actualTrackingDistance = coarseLayerRadius * (1u << (layers - 1u));

		OCEAN_EXPECT_GREATER_EQUAL(validation, actualTrackingDistance, requestedTrackingDistance);

		// Verify the coarsest layer resolution is valid (>= max(patchSize * 2, 32))
		const unsigned int coarsestSizeFactor = CV::FramePyramid::sizeFactor(layers - 1u);
		const unsigned int coarsestWidth = width / coarsestSizeFactor;
		const unsigned int coarsestHeight = height / coarsestSizeFactor;
		const unsigned int invalidLayerResolution = std::max(patchSize * 2u, 32u);

		OCEAN_EXPECT_GREATER_EQUAL(validation, coarsestWidth, invalidLayerResolution);
		OCEAN_EXPECT_GREATER_EQUAL(validation, coarsestHeight, invalidLayerResolution);

		// Verify that one fewer layer would have been insufficient for tracking distance
		// (i.e., the function returns the minimal number of layers needed)
		if (layers >= 2u)
		{
			const unsigned int fewerLayersTrackingDistance = coarseLayerRadius * (1u << (layers - 2u));

			// Either fewer layers wouldn't meet the tracking distance requirement,
			// or the coarsest layer would be too small with more layers
			const unsigned int finerCoarsestSizeFactor = CV::FramePyramid::sizeFactor(layers);
			const unsigned int finerCoarsestWidth = width / finerCoarsestSizeFactor;
			const unsigned int finerCoarsestHeight = height / finerCoarsestSizeFactor;

			const bool fewerLayersInsufficientTracking = fewerLayersTrackingDistance < requestedTrackingDistance;
			const bool moreLayersInvalidResolution = finerCoarsestWidth < invalidLayerResolution || finerCoarsestHeight < invalidLayerResolution;

			OCEAN_EXPECT_TRUE(validation, fewerLayersInsufficientTracking || moreLayersInvalidResolution);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}

}
