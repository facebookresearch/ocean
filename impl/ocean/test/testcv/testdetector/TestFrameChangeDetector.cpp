/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/TestFrameChangeDetector.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/CVUtilities.h"

#include "ocean/cv/detector/FrameChangeDetector.h"

#include "ocean/math/Random.h"

#include <random>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

using CV::Detector::FrameChangeDetector;

bool TestFrameChangeDetector::test(const double testDuration, Worker& worker)
{
	Log::info() << "---   Frame change detector test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	for (bool nonStaticInput : {false, true})
	{
		for (bool simulateDeviceMotion : {false, true})
		{
			for (bool forcedKeyframes : {false, true})
			{
				allSucceeded = testInput(testDuration, nonStaticInput, simulateDeviceMotion, forcedKeyframes, worker) && allSucceeded;
				Log::info() << " ";
			}
		}
	}

	if (allSucceeded)
	{
		Log::info() << "Frame change detector test succeeded.";
	}
	else
	{
		Log::info() << "Frame change detector test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameChangeDetector, StaticInputWithoutSimulatedMotionWithoutForcedKeyframes)
{
	Worker worker;
	EXPECT_TRUE(TestFrameChangeDetector::testInput(GTEST_TEST_DURATION, false, false, false, worker));
}

TEST(TestFrameChangeDetector, StaticInputWithSimulatedMotionWithoutForcedKeyframes)
{
	Worker worker;
	EXPECT_TRUE(TestFrameChangeDetector::testInput(GTEST_TEST_DURATION, false, true, false, worker));
}

TEST(TestFrameChangeDetector, StaticInputWithoutSimulatedMotionWithForcedKeyframes)
{
	Worker worker;
	EXPECT_TRUE(TestFrameChangeDetector::testInput(GTEST_TEST_DURATION, false, false, true, worker));
}

TEST(TestFrameChangeDetector, StaticInputWithSimulatedMotionWithForcedKeyframes)
{
	Worker worker;
	EXPECT_TRUE(TestFrameChangeDetector::testInput(GTEST_TEST_DURATION, false, true, true, worker));
}

TEST(TestFrameChangeDetector, NonStaticicInputWithoutSimulatedMotionWithoutForcedKeyframes)
{
	Worker worker;
	EXPECT_TRUE(TestFrameChangeDetector::testInput(GTEST_TEST_DURATION, true, false, false, worker));
}

TEST(TestFrameChangeDetector, NonStaticicInputWithSimulatedMotionWithoutForcedKeyframes)
{
	Worker worker;
	EXPECT_TRUE(TestFrameChangeDetector::testInput(GTEST_TEST_DURATION, true, true, false, worker));
}

TEST(TestFrameChangeDetector, NonStaticicInputWithoutSimulatedMotionWithForcedKeyframes)
{
	Worker worker;
	EXPECT_TRUE(TestFrameChangeDetector::testInput(GTEST_TEST_DURATION, true, false, true, worker));
}

TEST(TestFrameChangeDetector, NonStaticicInputWithSimulatedMotionWithForcedKeyframes)
{
	Worker worker;
	EXPECT_TRUE(TestFrameChangeDetector::testInput(GTEST_TEST_DURATION, true, true, true, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameChangeDetector::testInput(const double testDuration, bool nonStaticInput, bool simulateDeviceMotion, bool forcedKeyframes, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << (nonStaticInput ? "Non-static" : "Static") << " input test (with" << (simulateDeviceMotion ? "" : "out") << " device motion, with" << (forcedKeyframes ? "" : "out") << " forced keyframes):";

	RandomGenerator randomGenerator;
	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	unsigned int numberTests = 0u;
	unsigned int numberFrames = 0u;
	unsigned int numberFramesWithChange = 0u;
	unsigned int numberFramesWithLargeMotion = 0u;
	unsigned int numberForcedKeyframes = 0u;

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 4u, 2000u);
		const unsigned int height = RandomI::random(randomGenerator, 4u, 2000u);
		const unsigned int targetWidth = RandomI::random(randomGenerator, 4u, width);
		const unsigned int targetHeight = RandomI::random(randomGenerator, 4u, height);

		// Also check that a too-small spatial bin (< 4) results in an invalid detector.
		const unsigned int spatialBinSize = RandomI::random(randomGenerator, 1u, std::min(targetWidth, targetHeight));

		constexpr double secondsPerFrame = 1.0 / 30.0;

		// If we are forcing keyframes, a value in [1,2) forces every other keyframe to be skipped.
		const double minimumTimeBetweenKeyframes = forcedKeyframes ? RandomD::scalar(randomGenerator, 0.0, 5.0) * secondsPerFrame : 0.0;
		const double preferredMaximumTimeBetweenKeyframes = forcedKeyframes ? RandomD::scalar(randomGenerator, 1.0, 10.0) * secondsPerFrame : NumericD::maxValue();
		const double absoluteMaximumTimeBetweenKeyframes = forcedKeyframes ? RandomD::scalar(randomGenerator, 1.0, 20.0) * secondsPerFrame : NumericD::maxValue();

		const FrameChangeDetector::Options options =
		{
			/*.targetFrameWidth =*/ targetWidth,
			/*.targetFrameHeight =*/ targetHeight,
			/*.spatialBinSize =*/ spatialBinSize,
			/*.largeMotionAccelerationThreshold =*/ Scalar(1.0),
			/*.largeMotionRotationRateThreshold = */Scalar(1.0),
			/*.rotationThreshold =*/ Numeric::pi(), // we currently do not test this
			/*.minimumTimeBetweenKeyframes =*/ minimumTimeBetweenKeyframes,
			/*.preferredMaximumTimeBetweenKeyframes =*/ preferredMaximumTimeBetweenKeyframes,
			/*.absoluteMaximumTimeBetweenKeyframes =*/ absoluteMaximumTimeBetweenKeyframes,
			/*.minimumHistogramDistance =*/ Scalar(0.0),
			/*.histogramDistanceThreshold =*/ Numeric::weakEps(), // very sensitive -- should fire on any change
			/*.hangeDetectionThreshold =*/ Numeric::weakEps() // very sensitive -- should fire on any change
		};

		FrameChangeDetector detector(options);

		// The detector should only be invalid if the spatial bin size is invalid, or if the specified times between keyframes were invalid.
		if (!detector.isValid())
		{
			allSucceeded = (spatialBinSize < 4u || minimumTimeBetweenKeyframes >= preferredMaximumTimeBetweenKeyframes || preferredMaximumTimeBetweenKeyframes > absoluteMaximumTimeBetweenKeyframes) && allSucceeded;
			continue;
		}

		++numberTests;

		// Use multi-core processing in ~50% of tests.
		Worker* useWorker = RandomI::random(randomGenerator, 1u) == 0u ? nullptr : &worker;

		// Note that the detector will prefer not to create keyframes if large motion has occurred in the span (<timestamp of frame before the current frame>, now].
		bool priorFrameHadLargeDeviceMotion = false;

		const unsigned int numberFramesToProcess = RandomI::random(randomGenerator, 10u, 60u);
		Timestamp currentTimestamp = Timestamp(true);
		Timestamp lastKeyframeTimestamp(currentTimestamp - Timestamp(2.0 * (forcedKeyframes ? options.absoluteMaximumTimeBetweenKeyframes : secondsPerFrame))); // valid, but guaranteed to be stale

		const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		Frame yFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), paddingElements);
		CV::CVUtilities::randomizeFrame(yFrame, false, &randomGenerator);

		for (unsigned int i = 0u; i < numberFramesToProcess; ++i, currentTimestamp += Timestamp(secondsPerFrame))
		{
			++numberFrames;
			yFrame.setTimestamp(currentTimestamp);

			// Simulate accelerometer and gyroscope readings within the span of the last frame, exclusive of the last frame's timestamp.
			// Enforce a 25% chance of large motion per frame (50% over two frames).

			bool frameHasLargeDeviceMotion = false;
			if (simulateDeviceMotion)
			{
				const Timestamp accelerometerTimestamp = currentTimestamp - Timestamp(RandomD::scalar(randomGenerator, 0.0, (1.0 - Numeric::weakEps()) * secondsPerFrame));
				const Timestamp gyroscopeTimestamp = currentTimestamp - Timestamp(RandomD::scalar(randomGenerator, 0.0, (1.0 - Numeric::weakEps()) * secondsPerFrame));

				const bool largeAcceleration = RandomI::random(randomGenerator, 7u) == 0u;
				const bool largeRotation = RandomI::random(randomGenerator, 7u) == 0u;

				const Scalar minimumAcceleration = largeAcceleration ? Scalar(1.0) : Scalar(0.0);
				const Scalar maximumAcceleration = largeAcceleration ? Scalar(3.0) : Scalar(1.0) - Numeric::weakEps();
				const Scalar minimumRotation = largeRotation ? Scalar(1.0) : Scalar(0.0);
				const Scalar maximumRotation = largeRotation ? Scalar(3.0) : Scalar(1.0) - Numeric::weakEps();

				detector.addAccelerationSample(Random::vector3(randomGenerator) * Random::scalar(randomGenerator, minimumAcceleration, maximumAcceleration), accelerometerTimestamp);
				detector.addGyroSample(Random::vector3(randomGenerator) * Random::scalar(randomGenerator, minimumRotation, maximumRotation), gyroscopeTimestamp);

				// The detector will internally disregard any device motion registered before the first frame.
				if (i > 0u)
				{
					frameHasLargeDeviceMotion = largeAcceleration || largeRotation;
				}
			}

			// Actually run detection.

			const FrameChangeDetector::FrameChangeResult result = detector.detectFrameChange(yFrame, Quaternion(false), useWorker);

			// Perform logic checks.

			const double timeSinceKeyframe = double(currentTimestamp - lastKeyframeTimestamp);
			const bool minimumTimeReached = timeSinceKeyframe >= options.minimumTimeBetweenKeyframes;
			const bool preferredTimeReached = timeSinceKeyframe >= options.preferredMaximumTimeBetweenKeyframes;
			const bool absoluteTimeReached = timeSinceKeyframe >= options.absoluteMaximumTimeBetweenKeyframes;

			const bool maximumTimeReached = absoluteTimeReached || (preferredTimeReached && !frameHasLargeDeviceMotion && !priorFrameHadLargeDeviceMotion);

			// The first frame is always a keyframe.
			// Also, we have a guaranteed keyframe if (1) the absolute maximum time between keyframes was reached or (2) the preferred maximum time was reached and the last two frames did not have large motion.
			if (i == 0u || maximumTimeReached)
			{
				allSucceeded = (result == FrameChangeDetector::FrameChangeResult::CHANGE_DETECTED) && allSucceeded;
				lastKeyframeTimestamp = currentTimestamp;
				++numberFramesWithChange;
				++numberForcedKeyframes;
			}
			else if (!minimumTimeReached || !nonStaticInput || frameHasLargeDeviceMotion || priorFrameHadLargeDeviceMotion)
			{
				// Otherwise, we guarantee "not a keyframe" if the minimum time between keyframes wasn't reached, the frame is static, or large motion was recently observed.
				allSucceeded = (result == FrameChangeDetector::FrameChangeResult::NO_CHANGE_DETECTED) && allSucceeded;
			}
			else
			{
				allSucceeded = (result == FrameChangeDetector::FrameChangeResult::CHANGE_DETECTED) && allSucceeded;
				lastKeyframeTimestamp = currentTimestamp;
				++numberFramesWithChange;
			}

			// Prepare the next frame.

			if (frameHasLargeDeviceMotion || priorFrameHadLargeDeviceMotion)
			{
				++numberFramesWithLargeMotion;
			}

			priorFrameHadLargeDeviceMotion = frameHasLargeDeviceMotion;

			if (nonStaticInput)
			{
				CV::CVUtilities::randomizeFrame(yFrame, false, &randomGenerator);
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Total number of sequences tested: " << numberTests;
	Log::info() << "Total number of frames: " << numberFrames;
	Log::info() << "Total number of frames with change: " << numberFramesWithChange;
	Log::info() << "Total number of frames with large motion: " << numberFramesWithLargeMotion;
	Log::info() << "Total number of forced keyframes: " << numberForcedKeyframes;

	// Static inputs only have forced keyframes, and they only have one keyframe without forcing.
	if (!nonStaticInput)
	{
		allSucceeded = (numberFramesWithChange == numberForcedKeyframes) && allSucceeded;
		if (!forcedKeyframes)
		{
			allSucceeded = (numberFramesWithChange == numberTests) && allSucceeded;
		}
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
