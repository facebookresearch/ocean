/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/TestFeatureDetector.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/cv/detector/FeatureDetector.h"

#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

bool TestFeatureDetector::test(const Frame& testFrame, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   FeatureDetector test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testDetermineHarrisPoints(testFrame, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "FeatureDetector test succeeded.";
	}
	else
	{
		Log::info() << "FeatureDetector test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFeatureDetector, GradientBasedTShapeDetector)
{
	Worker worker;
	EXPECT_TRUE(TestFeatureDetector::testDetermineHarrisPoints(Frame(), GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFeatureDetector::testDetermineHarrisPoints(const Frame& testFrame, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Harris corner detection test:";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	Timestamp start(true);

	do
	{
		Worker* useWorker = RandomI::random(randomGenerator, 1u) == 0u ? &worker : nullptr;

		const unsigned int paddingElements = RandomI::random(randomGenerator, 100u) * RandomI::random(randomGenerator, 1u);

		Frame yFrame;
		if (testFrame.isValid())
		{
			yFrame.set(FrameType(testFrame.frameType(), FrameType::FORMAT_Y8), true, true, Indices32(1, paddingElements));
			CV::FrameConverter::Comfort::convert(testFrame, FrameType::FORMAT_Y8, yFrame, CV::FrameConverter::CP_ALWAYS_COPY, useWorker);

			ocean_assert(yFrame.paddingElements() == paddingElements);
		}
		else
		{
			const unsigned int width = RandomI::random(randomGenerator, 20u, 1920u);
			const unsigned int height = RandomI::random(randomGenerator, 20u, 1080u);

			yFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
		}

		if (!yFrame.isValid())
		{
			OCEAN_SET_FAILED(validation);
			break;
		}

		CV::SubRegion subRegion;

		if (RandomI::boolean(randomGenerator))
		{
			if (RandomI::boolean(randomGenerator))
			{
				const Scalar left = Random::scalar(randomGenerator, -10, 10);
				const Scalar top = Random::scalar(randomGenerator, -10, 10);

				const Scalar width = Random::scalar(randomGenerator, Scalar(5), Scalar(yFrame.width()));
				const Scalar height = Random::scalar(randomGenerator, Scalar(5), Scalar(yFrame.height()));

				const Box2 boundingBox(width, height, Vector2(left, top));

				subRegion = CV::SubRegion(boundingBox);
			}
			else
			{
				Frame yMask = CV::CVUtilities::randomizedBinaryMask(yFrame.width(), yFrame.height(), 0x00u, &randomGenerator);

				subRegion = CV::SubRegion(std::move(yMask));
			}
		}

		unsigned int horizontalBins = 0u;
		unsigned int verticalBins = 0u;

		if (RandomI::boolean(randomGenerator))
		{
			horizontalBins = RandomI::random(randomGenerator, 1u, yFrame.width() / 3u);
			verticalBins = RandomI::random(randomGenerator, 1u, yFrame.height() / 3u);
		}

		unsigned int strength = RandomI::random(randomGenerator, 0u, 256u);

		CV::Detector::FeatureDetector::determineHarrisPoints(yFrame, subRegion, horizontalBins, verticalBins, strength, useWorker);
	}
	while (Timestamp(true) < start + testDuration);

	Log::info() << " ";

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}

}
