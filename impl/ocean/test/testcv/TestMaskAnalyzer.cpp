/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestMaskAnalyzer.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/MaskAnalyzer.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestMaskAnalyzer::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u && testDuration > 0.0);

	Log::info() << "---   Mask analyzer test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testDetectBoundingBox(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectBoundingBoxWithRoughGuess(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectOpaqueBoundingBox(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHasValue(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Mask analyzer test succeeded.";
	}
	else
	{
		Log::info() << "Mask analyzer test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestMaskAnalyzer, DetectBoundingBox)
{
	EXPECT_TRUE(TestMaskAnalyzer::testDetectBoundingBox(1920u, 1080u, GTEST_TEST_DURATION));
}

TEST(TestMaskAnalyzer, DetectBoundingBoxWithRoughGuess)
{
	EXPECT_TRUE(TestMaskAnalyzer::testDetectBoundingBoxWithRoughGuess(1920u, 1080u, GTEST_TEST_DURATION));
}

TEST(TestMaskAnalyzer, DetectOpaqueBoundingBox)
{
	Worker worker;
	EXPECT_TRUE(TestMaskAnalyzer::testDetectOpaqueBoundingBox(1920u, 1080u, GTEST_TEST_DURATION, worker));
}

TEST(TestMaskAnalyzer, HasValue)
{
	EXPECT_TRUE(TestMaskAnalyzer::testHasValue(1920u, 1080u, GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestMaskAnalyzer::testDetectBoundingBox(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(width >= 1u && height >= 1u && testDuration > 0.0);

	Log::info() << "Test detect bounding box for a " << width << "x" << height << " frame:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		for (bool performanceIteration : {true, false})
		{
			const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 1u, 1920u);
			const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 1u, 1080u);

			const unsigned int framePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			Frame frame(FrameType(testWidth, testHeight, FrameType::FORMAT_Y8_FULL_RANGE, FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);

			const uint8_t nonMaskValue = uint8_t(RandomI::random(randomGenerator, 255));

			frame.setValue(nonMaskValue);

			CV::PixelBoundingBox maskBoundingBox;

			const unsigned int regions = RandomI::random(randomGenerator, 1u, 10u);

			for (unsigned int n = 0u; n < regions; ++n)
			{
				const unsigned int subFrameLeft = RandomI::random(randomGenerator, 0u, testWidth - 1u);
				const unsigned int subFrameTop = RandomI::random(randomGenerator, 0u, testHeight - 1u);
				const unsigned int subFrameWidth = RandomI::random(randomGenerator, 1u, testWidth - subFrameLeft);
				const unsigned int subFrameHeight = RandomI::random(randomGenerator, 1u, testHeight - subFrameTop);

				const uint8_t maskValue = ((unsigned int)(nonMaskValue) + RandomI::random(randomGenerator, 1u, 254u)) % 255u;
				ocean_assert(maskValue != nonMaskValue);

				frame.subFrame(subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight).setValue(maskValue);

				maskBoundingBox = maskBoundingBox || CV::PixelBoundingBox(CV::PixelPosition(subFrameLeft, subFrameTop), subFrameWidth, subFrameHeight);
			}

			ocean_assert(maskBoundingBox.isValid());

			performance.startIf(performanceIteration);
			const CV::PixelBoundingBox boundingBox = CV::MaskAnalyzer::detectBoundingBox(frame.constdata<uint8_t>(), frame.width(), frame.height(), nonMaskValue, frame.paddingElements());
			performance.stopIf(performanceIteration);

			if (boundingBox != maskBoundingBox)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance Best: " << performance.bestMseconds() << "ms, worst: " << performance.worstMseconds() << "ms, average: " << performance.averageMseconds() << "ms";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestMaskAnalyzer::testDetectBoundingBoxWithRoughGuess(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(width >= 1u && height >= 1u && testDuration > 0.0);

	Log::info() << "Test detect bounding box with rough guess for a " << width << "x" << height << " frame:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		for (bool randomIteration : {false, true})
		{
			const unsigned int testWidth = randomIteration ? RandomI::random(randomGenerator, 1u, 1920u) : width;
			const unsigned int testHeight = randomIteration ? RandomI::random(randomGenerator, 1u, 1080u) : height;

			const unsigned int framePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			Frame frame(FrameType(testWidth, testHeight, FrameType::FORMAT_Y8_FULL_RANGE, FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);

			const uint8_t nonMaskValue = uint8_t(RandomI::random(randomGenerator, 255));

			frame.setValue(nonMaskValue);

			CV::PixelBoundingBox maskBoundingBox;

			const unsigned int regions = RandomI::random(randomGenerator, 1u, 10u);

			for (unsigned int n = 0u; n < regions; ++n)
			{
				const unsigned int subFrameLeft = RandomI::random(randomGenerator, 0u, testWidth - 1u);
				const unsigned int subFrameTop = RandomI::random(randomGenerator, 0u, testHeight - 1u);
				const unsigned int subFrameWidth = RandomI::random(randomGenerator, 1u, testWidth - subFrameLeft);
				const unsigned int subFrameHeight = RandomI::random(randomGenerator, 1u, testHeight - subFrameTop);

				const uint8_t maskValue = ((unsigned int)(nonMaskValue) + RandomI::random(randomGenerator, 1u, 254u)) % 255u;
				ocean_assert(maskValue != nonMaskValue);

				frame.subFrame(subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight).setValue(maskValue);

				maskBoundingBox = maskBoundingBox || CV::PixelBoundingBox(CV::PixelPosition(subFrameLeft, subFrameTop), subFrameWidth, subFrameHeight);
			}

			ocean_assert(maskBoundingBox.isValid());

			const unsigned int maxUncertainty = RandomI::random(randomGenerator, 1u, std::max(testWidth, testHeight));

			const unsigned int roughLeft = (unsigned int)(minmax<int>(0, int(maskBoundingBox.left()) + RandomI::random(randomGenerator, -int(maxUncertainty), int(maxUncertainty)), int(frame.width()) - 1));
			const unsigned int roughTop = (unsigned int)(minmax<int>(0, int(maskBoundingBox.top()) + RandomI::random(randomGenerator, -int(maxUncertainty), int(maxUncertainty)), int(frame.height()) - 1));
			const unsigned int roughRight = std::max(roughLeft, (unsigned int)(minmax<int>(0, int(maskBoundingBox.right()) + RandomI::random(randomGenerator, -int(maxUncertainty), int(maxUncertainty)), int(frame.width()) - 1)));
			const unsigned int roughBottom = std::max(roughTop, (unsigned int)(minmax<int>(0, int(maskBoundingBox.bottom()) + RandomI::random(randomGenerator, -int(maxUncertainty), int(maxUncertainty)), int(frame.height()) - 1)));

			const CV::PixelBoundingBox roughBoundingBox(roughLeft, roughTop, roughRight, roughBottom);
			ocean_assert(roughBoundingBox.isValid());

			const CV::PixelBoundingBox boundingBox = CV::MaskAnalyzer::detectBoundingBox(frame.constdata<uint8_t>(), frame.width(), frame.height(), roughBoundingBox, maxUncertainty, nonMaskValue, frame.paddingElements());

			if (boundingBox != maskBoundingBox)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestMaskAnalyzer::testDetectOpaqueBoundingBox(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u && testDuration > 0.0);

	Log::info() << "Test detect opaque bounding box for a " << width << "x" << height << " frame:";

	bool allSucceeded = true;

	const FrameType::PixelFormats pixelFormats =
	{
		FrameType::FORMAT_ARGB32,
		FrameType::FORMAT_ABGR32,
		FrameType::FORMAT_BGRA32,
		FrameType::FORMAT_RGBA32,
		FrameType::FORMAT_YA16,
	};

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		const Timestamp startTimestamp(true);

		do
		{
			for (const FrameType::PixelFormat& pixelFormat : pixelFormats)
			{
				for (bool performanceIteration : {true, false})
				{
					const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 1u, 1920u);
					const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 1u, 1080u);

					const unsigned int framePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

					Frame frame(FrameType(testWidth, testHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);

					const uint8_t fullTransparentValue = RandomI::random(randomGenerator, 1u) == 0u ? uint8_t(0xFFu) : uint8_t(0x00u);

					std::vector<uint8_t> transparentColor(frame.channels(), uint8_t(RandomI::random(randomGenerator, 255u)));

					bool isLastChannelAlpha;
					if (FrameType::formatHasAlphaChannel(pixelFormat, &isLastChannelAlpha))
					{
						if (isLastChannelAlpha)
						{
							transparentColor.back() = fullTransparentValue;
						}
						else
						{
							transparentColor.front() = fullTransparentValue;
						}
					}
					else
					{
						ocean_assert(false && "This must never happen!");
						allSucceeded = false;
					}

					frame.setValue(transparentColor.data(), transparentColor.size());

					CV::PixelBoundingBox opaqueBoundingBox;

					const unsigned int regions = RandomI::random(randomGenerator, 1u, 10u);

					for (unsigned int n = 0u; n < regions; ++n)
					{
						const unsigned int subFrameLeft = RandomI::random(randomGenerator, 0u, testWidth - 1u);
						const unsigned int subFrameTop = RandomI::random(randomGenerator, 0u, testHeight - 1u);
						const unsigned int subFrameWidth = RandomI::random(randomGenerator, 1u, testWidth - subFrameLeft);
						const unsigned int subFrameHeight = RandomI::random(randomGenerator, 1u, testHeight - subFrameTop);

						std::vector<uint8_t> opaqueColor(frame.channels(), uint8_t(RandomI::random(randomGenerator, 255u)));

						const uint8_t opaqueValue = ((unsigned int)(fullTransparentValue) + RandomI::random(randomGenerator, 1u, 254u)) % 255u;
						ocean_assert(opaqueValue != fullTransparentValue);

						if (isLastChannelAlpha)
						{
							opaqueColor.back() = opaqueValue;
						}
						else
						{
							opaqueColor.front() = opaqueValue;
						}

						frame.subFrame(subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight).setValue(opaqueColor.data(), opaqueColor.size());

						opaqueBoundingBox = opaqueBoundingBox || CV::PixelBoundingBox(CV::PixelPosition(subFrameLeft, subFrameTop), subFrameWidth, subFrameHeight);
					}

					ocean_assert(opaqueBoundingBox.isValid());

					performance.startIf(performanceIteration);
					const CV::PixelBoundingBox boundingBox = CV::MaskAnalyzer::detectOpaqueBoundingBox(frame, fullTransparentValue, useWorker);
					performance.stopIf(performanceIteration);

					if (boundingBox != opaqueBoundingBox)
					{
						allSucceeded = false;
					}
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Singlecore Best: " << performanceSinglecore.bestMseconds() << "ms, worst: " << performanceSinglecore.worstMseconds() << "ms, average: " << performanceSinglecore.averageMseconds() << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms";
		Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestMaskAnalyzer::testHasValue(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(width >= 1u && height >= 1u && testDuration > 0.0);

	Log::info() << "Test has value for a " << width << "x" << height << " frame:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		for (bool performanceIteration : {true, false})
		{
			const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 1u, 1920u);
			const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 1u, 1080u);

			const uint8_t maskValue = uint8_t(RandomI::random(randomGenerator, 255u));

			Frame mask = CV::CVUtilities::randomizedBinaryMask(testWidth, testHeight, maskValue, &randomGenerator);

			uint8_t randomValue = uint8_t(RandomI::random(randomGenerator, 255u));
			while (randomValue == maskValue || randomValue == 255u - maskValue)
			{
				randomValue = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			const bool placeValue = RandomI::random(randomGenerator, 1u) == 0u;

			CV::PixelBoundingBox valueBoundingBox;

			if (placeValue)
			{
				const unsigned int subFrameLeft = RandomI::random(randomGenerator, 0u, testWidth - 1u);
				const unsigned int subFrameTop = RandomI::random(randomGenerator, 0u, testHeight - 1u);
				const unsigned int subFrameWidth = RandomI::random(randomGenerator, 1u, testWidth - subFrameLeft);
				const unsigned int subFrameHeight = RandomI::random(randomGenerator, 1u, testHeight - subFrameTop);

				mask.subFrame(subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight).setValue(randomValue);

				valueBoundingBox = CV::PixelBoundingBox(CV::PixelPosition(subFrameLeft, subFrameTop), subFrameWidth, subFrameHeight);
			}

			CV::PixelBoundingBox boundingBox;

			if (RandomI::random(randomGenerator, 1u) == 0u)
			{
				const unsigned int subFrameLeft = RandomI::random(randomGenerator, 0u, testWidth - 1u);
				const unsigned int subFrameTop = RandomI::random(randomGenerator, 0u, testHeight - 1u);
				const unsigned int subFrameWidth = RandomI::random(randomGenerator, 1u, testWidth - subFrameLeft);
				const unsigned int subFrameHeight = RandomI::random(randomGenerator, 1u, testHeight - subFrameTop);

				boundingBox = CV::PixelBoundingBox(CV::PixelPosition(subFrameLeft, subFrameTop), subFrameWidth, subFrameHeight);
			}

			performance.startIf(performanceIteration);
			const bool hasValue = CV::MaskAnalyzer::hasValue(mask.constdata<uint8_t>(), mask.width(), mask.height(), randomValue, mask.paddingElements(), boundingBox);
			performance.stopIf(performanceIteration);

			const bool boundingBoxDoesNotCoverValue = boundingBox.isValid() && valueBoundingBox.isValid() && !(boundingBox && valueBoundingBox).isValid();

			const bool expectedHasValue = placeValue && !boundingBoxDoesNotCoverValue;

			if (expectedHasValue != hasValue)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance Average: " << performance.averageMseconds() << "ms" << ", worst: " << performance.worstMseconds() << "ms";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
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
