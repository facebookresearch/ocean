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
#include "ocean/cv/FrameConverter.h"

#include "ocean/cv/detector/bullseyes/Utilities.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/FisheyeCamera.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Random.h"

#include "ocean/test/Validation.h"

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

	bool allSucceeded = true;

	allSucceeded = testParametersConstructor() && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testParametersIsValid() && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testParametersFramePyramidPixelThreshold() && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testParametersSetFramePyramidPixelThreshold() && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testParametersFramePyramidLayers() && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testParametersSetFramePyramidLayers() && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testParametersUseAdaptiveRowSpacing() && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testParametersSetUseAdaptiveRowSpacing() && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testParametersDefaultParameters() && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testDetectBullseyesWithSyntheticData(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = stressTestDetectBullseyes(testDuration) && allSucceeded;

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

TEST(TestBullseyeDetectorMono, DetectBullseyesWithSyntheticData)
{
	EXPECT_TRUE(TestDetector::TestBullseyes::TestBullseyeDetectorMono::testDetectBullseyesWithSyntheticData(GTEST_TEST_DURATION));
}

TEST(TestBullseyeDetectorMono, StressTestDetectBullseyes)
{
	EXPECT_TRUE(TestDetector::TestBullseyes::TestBullseyeDetectorMono::stressTestDetectBullseyes(GTEST_TEST_DURATION));
}

namespace TestBullseyes
{

#endif // OCEAN_USE_GTEST

bool TestBullseyeDetectorMono::testParametersConstructor()
{
	Log::info() << "Testing Parameters constructor:";

	Validation validation;

	BullseyeDetectorMono::Parameters parameters;

	OCEAN_EXPECT_TRUE(validation, parameters.isValid());

	OCEAN_EXPECT_EQUAL(validation, parameters.framePyramidPixelThreshold(), 640u * 480u);

	OCEAN_EXPECT_EQUAL(validation, parameters.framePyramidLayers(), 3u);

	OCEAN_EXPECT_TRUE(validation, parameters.useAdaptiveRowSpacing());

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestBullseyeDetectorMono::testParametersIsValid()
{
	Log::info() << "Testing Parameters::isValid():";

	Validation validation;

	{
		BullseyeDetectorMono::Parameters parameters;

		OCEAN_EXPECT_TRUE(validation, parameters.isValid());
	}

	{
		BullseyeDetectorMono::Parameters parameters;
		parameters.setFramePyramidLayers(1u);

		OCEAN_EXPECT_TRUE(validation, parameters.isValid());
	}

	{
		BullseyeDetectorMono::Parameters parameters;
		parameters.setFramePyramidLayers(0u);

		OCEAN_EXPECT_FALSE(validation, parameters.isValid());
	}

	{
		BullseyeDetectorMono::Parameters parameters;
		parameters.setFramePyramidLayers(10u);

		OCEAN_EXPECT_TRUE(validation, parameters.isValid());
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestBullseyeDetectorMono::testParametersFramePyramidPixelThreshold()
{
	Log::info() << "Testing Parameters::framePyramidPixelThreshold():";

	Validation validation;

	{
		BullseyeDetectorMono::Parameters parameters;

		OCEAN_EXPECT_EQUAL(validation, parameters.framePyramidPixelThreshold(), 640u * 480u);
	}

	{
		BullseyeDetectorMono::Parameters parameters;
		parameters.setFramePyramidPixelThreshold(1000u);

		OCEAN_EXPECT_EQUAL(validation, parameters.framePyramidPixelThreshold(), 1000u);
	}

	{
		BullseyeDetectorMono::Parameters parameters;
		parameters.setFramePyramidPixelThreshold(0u);

		OCEAN_EXPECT_EQUAL(validation, parameters.framePyramidPixelThreshold(), 0u);
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestBullseyeDetectorMono::testParametersSetFramePyramidPixelThreshold()
{
	Log::info() << "Testing Parameters::setFramePyramidPixelThreshold():";

	Validation validation;

	{
		BullseyeDetectorMono::Parameters parameters;
		parameters.setFramePyramidPixelThreshold(100u);

		OCEAN_EXPECT_EQUAL(validation, parameters.framePyramidPixelThreshold(), 100u);
	}

	{
		BullseyeDetectorMono::Parameters parameters;
		parameters.setFramePyramidPixelThreshold(1920u * 1080u);

		OCEAN_EXPECT_EQUAL(validation, parameters.framePyramidPixelThreshold(), 1920u * 1080u);
	}

	{
		BullseyeDetectorMono::Parameters parameters;
		parameters.setFramePyramidPixelThreshold(0u);

		OCEAN_EXPECT_EQUAL(validation, parameters.framePyramidPixelThreshold(), 0u);
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestBullseyeDetectorMono::testParametersFramePyramidLayers()
{
	Log::info() << "Testing Parameters::framePyramidLayers():";

	Validation validation;

	{
		BullseyeDetectorMono::Parameters parameters;

		OCEAN_EXPECT_EQUAL(validation, parameters.framePyramidLayers(), 3u);
	}

	{
		BullseyeDetectorMono::Parameters parameters;
		parameters.setFramePyramidLayers(1u);

		OCEAN_EXPECT_EQUAL(validation, parameters.framePyramidLayers(), 1u);
	}

	{
		BullseyeDetectorMono::Parameters parameters;
		parameters.setFramePyramidLayers(10u);

		OCEAN_EXPECT_EQUAL(validation, parameters.framePyramidLayers(), 10u);
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestBullseyeDetectorMono::testParametersSetFramePyramidLayers()
{
	Log::info() << "Testing Parameters::setFramePyramidLayers():";

	Validation validation;

	{
		BullseyeDetectorMono::Parameters parameters;
		parameters.setFramePyramidLayers(5u);

		OCEAN_EXPECT_EQUAL(validation, parameters.framePyramidLayers(), 5u);
	}

	{
		BullseyeDetectorMono::Parameters parameters;
		parameters.setFramePyramidLayers(1u);

		OCEAN_EXPECT_EQUAL(validation, parameters.framePyramidLayers(), 1u);
	}

	{
		BullseyeDetectorMono::Parameters parameters;
		parameters.setFramePyramidLayers(0u);

		OCEAN_EXPECT_EQUAL(validation, parameters.framePyramidLayers(), 0u);
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestBullseyeDetectorMono::testParametersUseAdaptiveRowSpacing()
{
	Log::info() << "Testing Parameters::useAdaptiveRowSpacing():";

	Validation validation;

	{
		BullseyeDetectorMono::Parameters parameters;

		OCEAN_EXPECT_TRUE(validation, parameters.useAdaptiveRowSpacing());
	}

	{
		BullseyeDetectorMono::Parameters parameters;
		parameters.setUseAdaptiveRowSpacing(false);

		OCEAN_EXPECT_FALSE(validation, parameters.useAdaptiveRowSpacing());
	}

	{
		BullseyeDetectorMono::Parameters parameters;
		parameters.setUseAdaptiveRowSpacing(true);

		OCEAN_EXPECT_TRUE(validation, parameters.useAdaptiveRowSpacing());
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestBullseyeDetectorMono::testParametersSetUseAdaptiveRowSpacing()
{
	Log::info() << "Testing Parameters::setUseAdaptiveRowSpacing():";

	Validation validation;

	{
		BullseyeDetectorMono::Parameters parameters;
		parameters.setUseAdaptiveRowSpacing(false);

		OCEAN_EXPECT_FALSE(validation, parameters.useAdaptiveRowSpacing());
	}

	{
		BullseyeDetectorMono::Parameters parameters;
		parameters.setUseAdaptiveRowSpacing(true);

		OCEAN_EXPECT_TRUE(validation, parameters.useAdaptiveRowSpacing());
	}

	{
		BullseyeDetectorMono::Parameters parameters;
		parameters.setUseAdaptiveRowSpacing(false);
		parameters.setUseAdaptiveRowSpacing(true);

		OCEAN_EXPECT_TRUE(validation, parameters.useAdaptiveRowSpacing());
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestBullseyeDetectorMono::testParametersDefaultParameters()
{
	Log::info() << "Testing Parameters::defaultParameters():";

	Validation validation;

	BullseyeDetectorMono::Parameters defaultParams = BullseyeDetectorMono::Parameters::defaultParameters();

	OCEAN_EXPECT_TRUE(validation, defaultParams.isValid());

	OCEAN_EXPECT_EQUAL(validation, defaultParams.framePyramidPixelThreshold(), 640u * 480u);

	OCEAN_EXPECT_EQUAL(validation, defaultParams.framePyramidLayers(), 3u);

	OCEAN_EXPECT_TRUE(validation, defaultParams.useAdaptiveRowSpacing());

	BullseyeDetectorMono::Parameters anotherDefaultParams = BullseyeDetectorMono::Parameters::defaultParameters();

	OCEAN_EXPECT_EQUAL(validation, anotherDefaultParams.framePyramidPixelThreshold(), defaultParams.framePyramidPixelThreshold());

	OCEAN_EXPECT_EQUAL(validation, anotherDefaultParams.framePyramidLayers(), defaultParams.framePyramidLayers());

	OCEAN_EXPECT_EQUAL(validation, anotherDefaultParams.useAdaptiveRowSpacing(), defaultParams.useAdaptiveRowSpacing());

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestBullseyeDetectorMono::testDetectBullseyesWithSyntheticData(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	Log::info() << "Test for BullseyeDetectorMono::detectBullseyes() with synthetic data (random seed: " << randomGenerator.initialSeed() << "):";

	const Timestamp startTimestamp(true);

	unsigned int iterations = 0u;
	unsigned int iterationsWithDetections = 0u;

	Scalars detectionAccuracyErrors;

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 250u, 2048u);
		const unsigned int height = RandomI::random(randomGenerator, 250u, 2048u);

		const bool useFisheye = RandomI::boolean(randomGenerator);

		SharedAnyCamera camera;

		if (useFisheye)
		{
			const Scalar fovX = Random::scalar(randomGenerator, Numeric::deg2rad(65), Numeric::deg2rad(120));
			camera = std::make_shared<AnyCameraFisheye>(FisheyeCamera(width, height, fovX));
		}
		else
		{
			const Scalar fovX = Random::scalar(randomGenerator, Numeric::deg2rad(45), Numeric::deg2rad(65));
			camera = std::make_shared<AnyCameraPinhole>(PinholeCamera(width, height, fovX));
		}

		ocean_assert(camera != nullptr && camera->isValid());

		// Draw a single bullseye with random size and offset
		constexpr unsigned int minDiameter = 15u;
		const unsigned int maxDiameter = std::min(width, height) / 2u;
		ocean_assert(minDiameter < maxDiameter);

		const unsigned int diameter = RandomI::random(randomGenerator, minDiameter, maxDiameter) | 1u;
		const unsigned int emptyBorder = 50u * diameter / 100u;

		const unsigned int bullseyeSize = diameter + 2u * emptyBorder;

		if (bullseyeSize > width || bullseyeSize > height)
		{
			continue;
		}

		const unsigned int maxOffsetX = width - bullseyeSize;
		const unsigned int maxOffsetY = height - bullseyeSize;
		const unsigned int offsetX = maxOffsetX == 0u ? 0u : RandomI::random(randomGenerator, 0u, maxOffsetX);
		const unsigned int offsetY = maxOffsetY == 0u ? 0u : RandomI::random(randomGenerator, 0u, maxOffsetY);
		const CV::PixelPosition offset(offsetX, offsetY);

		Frame rgbFrame(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		CV::CVUtilities::randomizeFrame(rgbFrame, /*skipPaddingArea*/ true, &randomGenerator);

		if (!CV::Detector::Bullseyes::Utilities::drawBullseyeWithOffset(rgbFrame, offset, diameter, emptyBorder, nullptr, nullptr))
		{
			OCEAN_SET_FAILED(validation);
			break;
		}

		Frame yFrame;
		if (!CV::FrameConverter::Comfort::convert(rgbFrame, FrameType::FORMAT_Y8, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
		{
			OCEAN_SET_FAILED(validation);
			break;
		}

		iterations++;
		BullseyeDetectorMono::Parameters parameters = BullseyeDetectorMono::Parameters::defaultParameters();
		parameters.setUseAdaptiveRowSpacing(false); // Scan every row for better accuracy in this test

		Bullseyes bullseyes;
		if (!BullseyeDetectorMono::detectBullseyes(yFrame, bullseyes, parameters, WorkerPool::get().scopedWorker()()))
		{
			OCEAN_SET_FAILED(validation);
			break;
		}

		if (bullseyes.size() == 1)
		{
			iterationsWithDetections++;

			const unsigned int trueCenterX = offsetX + (bullseyeSize / 2); // pixel-accurate
			const unsigned int trueCenterY = offsetY + (bullseyeSize / 2);
			const Vector2 truePosition(trueCenterX, trueCenterY);

			const Vector2 detectedPposition = bullseyes.front().position();

			const Scalar distance = truePosition.distance(detectedPposition);

			detectionAccuracyErrors.emplace_back(distance);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	if (detectionAccuracyErrors.empty())
	{
		Log::info() << "Detection accuracy errors: no valid detections recorded";
		OCEAN_SET_FAILED(validation);
	}
	else
	{
		std::sort(detectionAccuracyErrors.begin(), detectionAccuracyErrors.end());
		const Scalar detectionAccuracyErrorP50 = detectionAccuracyErrors[detectionAccuracyErrors.size() / 2];
		const Scalar detectionAccuracyErrorP95 = detectionAccuracyErrors[95 * detectionAccuracyErrors.size() / 100];
		const Scalar detectionAccuracyErrorP99 = detectionAccuracyErrors[99 * detectionAccuracyErrors.size() / 100];
		const Scalar detectionAccuracyErrorMax = detectionAccuracyErrors.back();
		Log::info() << "Detection accuracy errors: P50: " << String::toAString(detectionAccuracyErrorP50, 1u) << "px, P95: " << String::toAString(detectionAccuracyErrorP95, 1u) << "px, P99: " << String::toAString(detectionAccuracyErrorP99, 1u) << "px, max: " << String::toAString(detectionAccuracyErrorMax, 1u) << "px";

		if (detectionAccuracyErrorMax > 1.5)
		{
			OCEAN_SET_FAILED(validation);
		}
	}

	const Scalar detectionRate = iterations != 0u ? (Scalar(iterationsWithDetections) / Scalar(iterations) * Scalar(100.0)) : Scalar(-1.0);
	Log::info() << "Detection rate: " << String::toAString(detectionRate, 1u) << "%";

	if (detectionRate < 0.99)
	{
		OCEAN_SET_FAILED(validation);
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestBullseyeDetectorMono::stressTestDetectBullseyes(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "BullseyeDetectorMono::detectBullseyes() stress test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		constexpr std::array<AnyCameraType, 2> anyCameraTypes = {AnyCameraType::PINHOLE, AnyCameraType::FISHEYE};
		const SharedAnyCamera realisticCamera = TestGeometry::Utilities::realisticAnyCamera(anyCameraTypes[RandomI::random(randomGenerator, 1u)], RandomI::random(randomGenerator, 1u));
		ocean_assert(realisticCamera != nullptr && realisticCamera->isValid());

		const unsigned int width = RandomI::random(randomGenerator, 21u, 2048u);
		const unsigned int height = (realisticCamera->height() * width + realisticCamera->width() / 2u) / realisticCamera->width();

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
		parameters.setUseAdaptiveRowSpacing(RandomI::boolean(randomGenerator));
		ocean_assert(parameters.isValid());

		const bool useWorker = RandomI::boolean(randomGenerator);


		Bullseyes bullseyes;
		if (!BullseyeDetectorMono::detectBullseyes(yFrame, bullseyes, parameters, (useWorker ? WorkerPool::get().scopedWorker()() : nullptr)))
		{
			OCEAN_SET_FAILED(validation);
			break;
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

} // namespace TestBullseyes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
