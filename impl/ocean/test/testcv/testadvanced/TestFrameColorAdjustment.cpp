/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testadvanced/TestFrameColorAdjustment.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"

#include "ocean/cv/advanced/FrameColorAdjustment.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestAdvanced
{

bool TestFrameColorAdjustment::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Frame Color Adjustment test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testAdjustmentNoMask<1u>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testAdjustmentNoMask<2u>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testAdjustmentNoMask<3u>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testAdjustmentNoMask<4u>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAdjustmentWithMask<1u>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testAdjustmentWithMask<2u>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testAdjustmentWithMask<3u>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testAdjustmentWithMask<4u>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Frame Color Adjustment test succeeded.";
	}
	else
	{
		Log::info() << "Frame Color Adjustment test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

// Exact, mask, 1-4 channels

TEST(TestFrameColorAdjustment, AdjustmentNoMask_1Channels)
{
	Worker worker;
	EXPECT_TRUE((TestFrameColorAdjustment::testAdjustmentNoMask<1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameColorAdjustment, AdjustmentNoMask_2Channels)
{
	Worker worker;
	EXPECT_TRUE((TestFrameColorAdjustment::testAdjustmentNoMask<2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameColorAdjustment, AdjustmentNoMask_3Channels)
{
	Worker worker;
	EXPECT_TRUE((TestFrameColorAdjustment::testAdjustmentNoMask<3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameColorAdjustment, AdjustmentNoMask_4Channels)
{
	Worker worker;
	EXPECT_TRUE((TestFrameColorAdjustment::testAdjustmentNoMask<4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameColorAdjustment, AdjustmentWithMask_1Channels)
{
	Worker worker;
	EXPECT_TRUE((TestFrameColorAdjustment::testAdjustmentWithMask<1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameColorAdjustment, AdjustmentWithMask_2Channels)
{
	Worker worker;
	EXPECT_TRUE((TestFrameColorAdjustment::testAdjustmentWithMask<2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameColorAdjustment, AdjustmentWithMask_3Channels)
{
	Worker worker;
	EXPECT_TRUE((TestFrameColorAdjustment::testAdjustmentWithMask<3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameColorAdjustment, AdjustmentWithMask_4Channels)
{
	Worker worker;
	EXPECT_TRUE((TestFrameColorAdjustment::testAdjustmentWithMask<4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

#endif // OCEAN_USE_GTEST

template <unsigned int tChannels>
bool TestFrameColorAdjustment::testAdjustmentNoMask(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Adjustment test with " << tChannels << " channels (no mask):";

	constexpr Scalar threshold = 3;

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		Timestamp startTimestamp(true);

		do
		{
			for (const bool performanceIteration : {true, false})
			{
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 1u, 1920u);
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 1u, 1080u);

				const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

				const unsigned int modificationHorizontalBins = std::min(3u, frame.width());
				const unsigned int modificationHerticalBins = std::min(3u, frame.height());

				const ObjectLookupCenter2<tChannels> modification(modificationTable<tChannels>(frame.width(), frame.height(), modificationHorizontalBins, modificationHerticalBins, -30, 30, randomGenerator));

				Frame modifiedFrame = CV::CVUtilities::randomizedFrame(frame.frameType(), &randomGenerator);

				const Frame copyModifiedFrame(modifiedFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				modifyFrame8BitPerChannel<tChannels>(frame.constdata<uint8_t>(), frame.paddingElements(), modification, modifiedFrame.data<uint8_t>(), modifiedFrame.paddingElements());

#ifdef OCEAN_DEBUG
				const Scalar debugModifiedFrameAverageDifference = averageDifference<tChannels>(frame, Frame(), modifiedFrame, Frame(), 0xFF);
				ocean_assert(debugModifiedFrameAverageDifference > threshold);
#endif

				Frame modifiedFrameMask;

				const unsigned int horizontalBins = std::min(10u, frame.width());
				const unsigned int verticalBins = std::min(10u, frame.height());

				performance.startIf(performanceIteration);
					const bool localResult = CV::Advanced::FrameColorAdjustment::adjustFrameBilinear(frame, Frame(), modifiedFrame, modifiedFrameMask, horizontalBins, verticalBins, Scalar(40), 0xFFu, useWorker);
				performance.stopIf(performanceIteration);

				if (!localResult)
				{
					allSucceeded = false;
				}

				if (!CV::CVUtilities::isPaddingMemoryIdentical(modifiedFrame, copyModifiedFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (averageDifference<tChannels>(frame, Frame(), modifiedFrame, Frame(), 0xFFu) > threshold)
				{
					if (performanceIteration || (testWidth >= 400u && testHeight >= 400u))
					{
						allSucceeded = false;
					}
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms";
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

template <unsigned int tChannels>
bool TestFrameColorAdjustment::testAdjustmentWithMask(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Adjustment test with " << tChannels << " channels (with mask):";

	constexpr Scalar threshold = 3;

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		Timestamp startTimestamp(true);

		do
		{
			for (const bool performanceIteration : {true, false})
			{
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 1u, 1920u);
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 1u, 1080u);

				Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

				const unsigned int modificationHorizontalBins = std::min(3u, frame.width());
				const unsigned int modificationHerticalBins = std::min(3u, frame.height());

				const ObjectLookupCenter2<tChannels> modification(modificationTable<tChannels>(frame.width(), frame.height(), modificationHorizontalBins, modificationHerticalBins, -30, 30, randomGenerator));

				Frame modifiedFrame = CV::CVUtilities::randomizedFrame(frame.frameType(), &randomGenerator);

				const Frame copyModifiedFrame(modifiedFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				modifyFrame8BitPerChannel<tChannels>(frame.constdata<uint8_t>(), frame.paddingElements(), modification, modifiedFrame.data<uint8_t>(), modifiedFrame.paddingElements());

#ifdef OCEAN_DEBUG
				const Scalar debugModifiedFrameAverageDifference = averageDifference<tChannels>(frame, Frame(), modifiedFrame, Frame(), 0xFF);
				ocean_assert(debugModifiedFrameAverageDifference > threshold);
#endif

				const unsigned int maskIteration = RandomI::random(randomGenerator, 2u);

				Frame frameMask;

				if (maskIteration == 0u || maskIteration == 2u)
				{
					const unsigned int frameMaskPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

					frameMask = Frame(FrameType(frame, FrameType::FORMAT_Y8), frameMaskPaddingElements);
					frameMask.setValue(0xFFu);

					randomMask<tChannels>(frame, frameMask, frame.pixels() / 8u, randomGenerator, 0x00u);
				}

				Frame modifiedFrameMask;

				if (maskIteration == 1u || maskIteration == 2u)
				{
					const unsigned int modifiedFrameMaskPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

					modifiedFrameMask = Frame(FrameType(frame, FrameType::FORMAT_Y8), modifiedFrameMaskPaddingElements);
					modifiedFrameMask.setValue(0xFFu);

					randomMask<tChannels>(modifiedFrame, modifiedFrameMask, frame.pixels() / 8u, randomGenerator, 0x00);
				}

				const unsigned int horizontalBins = std::min(10u, frame.width());
				const unsigned int verticalBins = std::min(10u, frame.height());

				performance.startIf(performanceIteration);
					const bool localResult = CV::Advanced::FrameColorAdjustment::adjustFrameBilinear(frame, frameMask, modifiedFrame, modifiedFrameMask, horizontalBins, verticalBins, Scalar(400), 0xFFu, useWorker);
				performance.stopIf(performanceIteration);

				if (!localResult)
				{
					allSucceeded = false;
				}

				if (!CV::CVUtilities::isPaddingMemoryIdentical(modifiedFrame, copyModifiedFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (averageDifference<tChannels>(frame, frameMask, modifiedFrame, modifiedFrameMask, 0xFFu) > threshold)
				{
					if (performanceIteration || (testWidth >= 400u && testHeight >= 400u))
					{
						allSucceeded = false;
					}
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms";
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

template <unsigned int tChannels>
TestFrameColorAdjustment::ObjectLookupCenter2<tChannels> TestFrameColorAdjustment::modificationTable(const unsigned int sizeX, const unsigned int sizeY, const unsigned int binsX, const unsigned int binsY, const Scalar minimalOffset, const Scalar maximalOffset, RandomGenerator& randomGenerator)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(sizeX >= 1u && sizeY >= 1u);
	ocean_assert(binsX >= 1u && binsX <= sizeX && binsY >= 1u && binsY <= sizeY);

	ocean_assert(minimalOffset < maximalOffset);

	ObjectLookupCenter2<tChannels> result(sizeX, sizeY, binsX, binsY);

	for (unsigned int yBin = 0u; yBin < result.binsY(); ++yBin)
	{
		for (unsigned int xBin = 0u; xBin < result.binsX(); ++xBin)
		{
			Object<tChannels> offset;

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				offset[n] = Random::scalar(randomGenerator, minimalOffset, maximalOffset);
			}

			result.setBinCenterValue(xBin, yBin, offset);
		}
	}

	return result;
}

template <unsigned int tChannels>
void TestFrameColorAdjustment::modifyFrame8BitPerChannel(const uint8_t* source, const unsigned int sourcePaddingElements, const ObjectLookupCenter2<tChannels>& lookupTable, uint8_t* target, const unsigned int targetPaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(!lookupTable.isEmpty());

	for (unsigned int y = 0u; y < lookupTable.sizeY(); ++y)
	{
		for (unsigned int x = 0u; x < lookupTable.sizeX(); ++x)
		{
			const StaticBuffer<Scalar, tChannels> offset(lookupTable.bicubicValue(Scalar(x), Scalar(y)));

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				target[n] = uint8_t(minmax<int>(0, Numeric::round32(Scalar(source[n]) + offset[n]), 255));
			}

			source += tChannels;
			target += tChannels;
		}

		source += sourcePaddingElements;
		target += targetPaddingElements;
	}
}

template <unsigned int tChannels>
Scalar TestFrameColorAdjustment::averageDifference(const Frame& frame0, const Frame& mask0, const Frame& frame1, const Frame& mask1, const uint8_t maskValue)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(frame0.isValid() && frame1.isValid());
	ocean_assert(frame0.frameType() == frame1.frameType());

	Scalar difference = 0;
	unsigned int counter = 0u;

	for (unsigned int y = 0u; y < frame0.height(); ++y)
	{
		const uint8_t* frameRow0 = frame0.constrow<uint8_t>(y);
		const uint8_t* maskRow0 = mask0.isValid() ? mask0.constrow<uint8_t>(y) : nullptr;

		const uint8_t* frameRow1 = frame1.constrow<uint8_t>(y);
		const uint8_t* maskRow1 = mask1.isValid() ? mask1.constrow<uint8_t>(y) : nullptr;

		for (unsigned int x = 0u; x < frame0.width(); ++x)
		{
			if ((maskRow0 == nullptr || maskRow0[x] == maskValue) && (maskRow1 == nullptr || maskRow1[x] == maskValue))
			{
				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					difference += Numeric::abs(Scalar(frameRow0[x * tChannels + n] - frameRow1[x * tChannels + n]));
				}

				counter += tChannels;
			}
		}
	}

	if (counter == 0u)
	{
		return Scalar(0);
	}

	return difference / Scalar(counter);
}

template <unsigned int tChannels>
void TestFrameColorAdjustment::randomMask(Frame& frame, Frame& mask, const unsigned int number, RandomGenerator& randomGenerator, const uint8_t value)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(frame.isValid() && mask.isValid());
	ocean_assert(frame.width() == mask.width());
	ocean_assert(frame.height() == mask.height());

	for (unsigned int i = 0u; i < number; ++i)
	{
		const unsigned int x = RandomI::random(randomGenerator, frame.width() - 1u);
		const unsigned int y = RandomI::random(randomGenerator, frame.height() - 1u);

		mask.pixel<uint8_t>(x, y)[0] = value;

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			frame.pixel<uint8_t>(x, y)[n] = uint8_t(RandomI::random(randomGenerator, 255u));
		}
	}
}

}

}

}

}
