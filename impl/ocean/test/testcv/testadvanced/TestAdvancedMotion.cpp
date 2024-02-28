// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testcv/testadvanced/TestAdvancedMotion.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"

#include "ocean/cv/advanced/AdvancedMotion.h"

#include "ocean/base/RandomI.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestAdvanced
{

bool TestAdvancedMotion::test(const unsigned int /*width*/, const unsigned int /*height*/, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Advanced motion test:   ---";
	Log::info() << " ";

	allSucceeded = stressTestTrackPointsBidirectionalSubPixelMirroredBorder(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Advanced motion test succeeded.";
	}
	else
	{
		Log::info() << "Advanced motion testFAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestAdvancedMotion, StressTestTrackPointsBidirectionalSubPixelMirroredBorder_AdvancedMotionSSD_5)
{
	RandomGenerator randomGenerator;
	Worker worker;

	EXPECT_TRUE((TestAdvancedMotion::stressTestTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionSSD, 5u>(randomGenerator, worker)));
}

TEST(TestAdvancedMotion, StressTestTrackPointsBidirectionalSubPixelMirroredBorder_AdvancedMotionSSD_7)
{
	RandomGenerator randomGenerator;
	Worker worker;

	EXPECT_TRUE((TestAdvancedMotion::stressTestTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionSSD, 7u>(randomGenerator, worker)));
}

TEST(TestAdvancedMotion, StressTestTrackPointsBidirectionalSubPixelMirroredBorder_AdvancedMotionSSD_15)
{
	RandomGenerator randomGenerator;
	Worker worker;

	EXPECT_TRUE((TestAdvancedMotion::stressTestTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionSSD, 15u>(randomGenerator, worker)));
}

TEST(TestAdvancedMotion, StressTestTrackPointsBidirectionalSubPixelMirroredBorder_AdvancedMotionSSD_31)
{
	RandomGenerator randomGenerator;
	Worker worker;

	EXPECT_TRUE((TestAdvancedMotion::stressTestTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionSSD, 31u>(randomGenerator, worker)));
}


TEST(TestAdvancedMotion, StressTestTrackPointsBidirectionalSubPixelMirroredBorder_AdvancedMotionZeroMeanSSD_5)
{
	RandomGenerator randomGenerator;
	Worker worker;

	EXPECT_TRUE((TestAdvancedMotion::stressTestTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionZeroMeanSSD, 5u>(randomGenerator, worker)));
}

TEST(TestAdvancedMotion, StressTestTrackPointsBidirectionalSubPixelMirroredBorder_AdvancedMotionZeroMeanSSD_7)
{
	RandomGenerator randomGenerator;
	Worker worker;

	EXPECT_TRUE((TestAdvancedMotion::stressTestTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionZeroMeanSSD, 7u>(randomGenerator, worker)));
}

TEST(TestAdvancedMotion, StressTestTrackPointsBidirectionalSubPixelMirroredBorder_AdvancedMotionZeroMeanSSD_15)
{
	RandomGenerator randomGenerator;
	Worker worker;

	EXPECT_TRUE((TestAdvancedMotion::stressTestTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionZeroMeanSSD, 15u>(randomGenerator, worker)));
}

TEST(TestAdvancedMotion, StressTestTrackPointsBidirectionalSubPixelMirroredBorder_AdvancedMotionZeroMeanSSD_31)
{
	RandomGenerator randomGenerator;
	Worker worker;

	EXPECT_TRUE((TestAdvancedMotion::stressTestTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionZeroMeanSSD, 31u>(randomGenerator, worker)));
}

#endif // OCEAN_USE_GTEST

bool TestAdvancedMotion::stressTestTrackPointsBidirectionalSubPixelMirroredBorder(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Stress test for trackPointsBidirectionalSubPixelMirroredBorder()";
	Log::info() << " ";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	Timestamp startTimestamp(true);

	do
	{
		if (!stressTestTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionSSD>(randomGenerator, worker))
		{
			allSucceeded = false;
		}

		if (!stressTestTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionZeroMeanSSD>(randomGenerator, worker))
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: Succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestAdvancedMotion::stressTestTrackPointsBidirectionalSubPixelMirroredBorder(RandomGenerator& randomGenerator, Worker& worker)
{
	bool allSucceeded = true;

	if (!stressTestTrackPointsBidirectionalSubPixelMirroredBorder<T, 5u>(randomGenerator, worker))
	{
		allSucceeded = false;
	}

	if (!stressTestTrackPointsBidirectionalSubPixelMirroredBorder<T, 7u>(randomGenerator, worker))
	{
		allSucceeded = false;
	}

	if (!stressTestTrackPointsBidirectionalSubPixelMirroredBorder<T, 15u>(randomGenerator, worker))
	{
		allSucceeded = false;
	}

	if (!stressTestTrackPointsBidirectionalSubPixelMirroredBorder<T, 31u>(randomGenerator, worker))
	{
		allSucceeded = false;
	}

	return allSucceeded;
}

template <typename T, unsigned int tPatchSize>
bool TestAdvancedMotion::stressTestTrackPointsBidirectionalSubPixelMirroredBorder(RandomGenerator& randomGenerator, Worker& worker)
{
	static_assert(tPatchSize >= 1u && tPatchSize % 2u == 1u, "Invalid patch size!");

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;

		const unsigned int channels = RandomI::random(randomGenerator, 1u, 4u);

		const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<uint8_t>(channels);

		const unsigned int sourceWidth = RandomI::random(tPatchSize * 2u, 2000u);
		const unsigned int sourceHeight = RandomI::random(tPatchSize * 2u, 2000u);

		const unsigned int targetWidth = RandomI::random(tPatchSize * 2u, 2000u);
		const unsigned int targetHeight = RandomI::random(tPatchSize * 2u, 2000u);

		const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceWidth, sourceHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), false, &randomGenerator);
		const Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(targetWidth, targetHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), false, &randomGenerator);

		Indices32 indices;
		Indices32* useIndices = RandomI::random(randomGenerator, 1u) == 0u ? &indices : nullptr;

		const unsigned int baseline = RandomI::random(randomGenerator, 1u, std::min(sourceWidth / 4u, sourceHeight / 4u));
		const unsigned int coarsestLayerRadius = RandomI::random(randomGenerator, 2u, tPatchSize * 2u);

		const unsigned int numberFeatures = RandomI::random(randomGenerator, 1u, 200u);

		Vectors2 sourceFeatures;
		sourceFeatures.reserve(numberFeatures);

		for (unsigned int n = 0u; n < numberFeatures; ++n)
		{
			sourceFeatures.emplace_back(Random::vector2(randomGenerator, Scalar(0), Scalar(sourceWidth - 1u), Scalar(0), Scalar(sourceHeight - 1u)));
		}

		Vectors2 targetFeatures;
		if (!T::template trackPointsBidirectionalSubPixelMirroredBorder<tPatchSize>(sourceFrame, targetFrame, baseline, coarsestLayerRadius, sourceFeatures, targetFeatures, Scalar(0.9 * 0.9), CV::FramePyramid::DM_FILTER_11, useWorker, useIndices))
		{
			return false;
		}
	}

	return true;
}

}

}

}

}
