/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/testbullseyes/TestBullseyeDetectorStereo.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/CVUtilities.h"

#include "ocean/math/Random.h"

#include "ocean/test/testgeometry/Utilities.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

namespace TestBullseyes
{

using namespace CV::Detector::Bullseyes;

bool TestBullseyeDetectorStereo::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Test for BullseyeDetectorStereo:   ---";
	Log::info() << " ";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	allSucceeded = stressTestDetectBullseyes(testDuration, randomGenerator) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "BullseyeDetectorStereo test succeeded.";
	}
	else
	{
		Log::info() << "BullseyeDetectorStereo test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

} // namespace TestBullseyes

TEST(TestBullseyeDetectorStereo, StressTestDetectBullseyes)
{
	RandomGenerator randomGenerator;
	EXPECT_TRUE(TestDetector::TestBullseyes::TestBullseyeDetectorStereo::stressTestDetectBullseyes(GTEST_TEST_DURATION, randomGenerator));
}

namespace TestBullseyes
{

#endif // OCEAN_USE_GTEST

bool TestBullseyeDetectorStereo::stressTestDetectBullseyes(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "BullseyeDetectorStereo::detectBullseyes() stress test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		constexpr std::array<AnyCameraType, 2> anyCameraTypes = {AnyCameraType::PINHOLE, AnyCameraType::FISHEYE};

		// Create two realistic cameras for stereo setup
		const SharedAnyCamera realisticCamera = TestGeometry::Utilities::realisticAnyCamera(anyCameraTypes[RandomI::random(randomGenerator, 1u)], RandomI::random(randomGenerator, 1u));
		ocean_assert(realisticCamera != nullptr && realisticCamera->isValid());

		const unsigned int widthA = RandomI::random(randomGenerator, 21u, 2048u);
		const unsigned int heightA = (realisticCamera->height() * widthA) / realisticCamera->width();

		const unsigned int widthB = RandomI::random(randomGenerator, 21u, 2048u);
		const unsigned int heightB = (realisticCamera->height() * widthB) / realisticCamera->width();

		if (heightA < 21u || heightB < 21u)
		{
			// One of the cameras/frames undercuts the minimum image height and cannot be processed.
			continue;
		}

		const SharedAnyCamera cameraA = realisticCamera->clone(widthA, heightA);
		const SharedAnyCamera cameraB = realisticCamera->clone(widthB, heightB);
		ocean_assert(cameraA != nullptr && cameraA->isValid());
		ocean_assert(cameraB != nullptr && cameraB->isValid());

		SharedAnyCameras cameras = {cameraA, cameraB};

		// Create random frames
		Frame yFrameA(FrameType(widthA, heightA, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
		Frame yFrameB(FrameType(widthB, heightB, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
		CV::CVUtilities::randomizeFrame(yFrameA, /*skipPaddingArea*/ true, &randomGenerator);
		CV::CVUtilities::randomizeFrame(yFrameB, /*skipPaddingArea*/ true, &randomGenerator);

		Frames yFrames = {std::move(yFrameA), std::move(yFrameB)};

		// Create random transformations
		const HomogenousMatrix4 world_T_device(Random::vector3(randomGenerator, -10, 10), Random::euler(randomGenerator));

		HomogenousMatrices4 device_T_cameras(2);
		device_T_cameras[0] = HomogenousMatrix4(Random::vector3(randomGenerator, -1, 1), Random::euler(randomGenerator));
		device_T_cameras[1] = HomogenousMatrix4(Random::vector3(randomGenerator, -1, 1), Random::euler(randomGenerator));

		BullseyeDetectorStereo::Parameters parameters = BullseyeDetectorStereo::Parameters::defaultParameters();
		parameters.setFramePyramidPixelThreshold((unsigned int)(Scalar(yFrames[0].pixels()) * Random::scalar(randomGenerator, 0.1, 2.0) + Scalar(0.5)));
		parameters.setFramePyramidLayers(RandomI::random(randomGenerator, 1u, 5u));
		parameters.setUseAdaptiveRowSpacing(RandomI::random(randomGenerator, 1u) == 1u);
		ocean_assert(parameters.isValid());

		const bool useWorker = RandomI::random(randomGenerator, 1u) == 1u;

		BullseyeDetectorStereo::BullseyePairs bullseyePairs;
		Vectors3 bullseyeCenters;
		if (!BullseyeDetectorStereo::detectBullseyes(cameras, yFrames, world_T_device, device_T_cameras, bullseyePairs, bullseyeCenters, parameters, (useWorker ? WorkerPool::get().scopedWorker()() : nullptr)))
		{
			allSucceeded = false;
			break;
		}

		// Validate that the output is consistent
		if (bullseyePairs.size() != bullseyeCenters.size())
		{
			allSucceeded = false;
			break;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Stress test succeeded.";
	}
	else
	{
		Log::info() << "Stress test FAILED!";
	}

	return allSucceeded;
}

} // namespace TestBullseyes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
