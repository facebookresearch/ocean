/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/testbullseyes/TestBullseyeDetectorMono.h"

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

bool TestBullseyeDetectorMono::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Test for BullseyeDetectorMono:   ---";
	Log::info() << " ";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	allSucceeded = stressTestDetectBullseyes(testDuration, randomGenerator) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "BullseyeDetectorMono test succeeded.";
	}
	else
	{
		Log::info() << "BullseyeDetectorMono test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

} // namespace TestBullseyes

TEST(TestBullseyeDetectorMono, StressTestDetectBullseyes)
{
	RandomGenerator randomGenerator;
	EXPECT_TRUE(TestDetector::TestBullseyes::TestBullseyeDetectorMono::stressTestDetectBullseyes(GTEST_TEST_DURATION, randomGenerator));
}

namespace TestBullseyes
{

#endif // OCEAN_USE_GTEST

bool TestBullseyeDetectorMono::stressTestDetectBullseyes(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "BullseyeDetectorMono::detectBullseyes() stress test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		constexpr std::array<AnyCameraType, 2> anyCameraTypes = {AnyCameraType::PINHOLE, AnyCameraType::FISHEYE};
		const SharedAnyCamera realisticCamera = TestGeometry::Utilities::realisticAnyCamera(anyCameraTypes[RandomI::random(randomGenerator, 1u)], RandomI::random(randomGenerator, 1u));
		ocean_assert(realisticCamera != nullptr && realisticCamera->isValid());

		const unsigned int width = RandomI::random(randomGenerator, 21u, 2048u);
		const unsigned int height = (realisticCamera->height() * width) / realisticCamera->width();

		if (height < 21u)
		{
			// The camera/frame undercuts the minimum image height and cannot be processed.
			continue;
		}

		const SharedAnyCamera camera = realisticCamera->clone(width, height);
		ocean_assert(camera != nullptr && camera->isValid());

		Frame yFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
		CV::CVUtilities::randomizeFrame(yFrame, /*skipPaddingArea*/ true, &randomGenerator);

		BullseyeDetectorMono::Parameters parameters = BullseyeDetectorMono::Parameters::defaultParameters();
		parameters.setFramePyramidPixelThreshold((unsigned int)(Scalar(yFrame.pixels()) * Random::scalar(randomGenerator, 0.1, 2.0) + Scalar(0.5)));
		parameters.setFramePyramidLayers(RandomI::random(randomGenerator, 1u, 5u));
		parameters.setUseAdaptiveRowSpacing(RandomI::random(randomGenerator, 1u) == 1u);
		ocean_assert(parameters.isValid());

		const bool useWorker = RandomI::random(randomGenerator, 1u) == 1u;


		Bullseyes bullseyes;
		if (!BullseyeDetectorMono::detectBullseyes(*camera, yFrame, bullseyes, parameters, (useWorker ? WorkerPool::get().scopedWorker()() : nullptr)))
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
