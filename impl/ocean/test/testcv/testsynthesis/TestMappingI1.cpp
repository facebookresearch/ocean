/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testsynthesis/TestMappingI1.h"
#include "ocean/test/testcv/testsynthesis/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Utilities.h"

#include "ocean/cv/CVUtilities.h"

#include "ocean/cv/segmentation/MaskAnalyzer.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestSynthesis
{

bool TestMappingI1::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "MappingI1 test:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testApplyMapping(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSumSquaredDifference5x5MaskNoCenter(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAppearanceCost5x5(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSpatialCost4Neighborhood(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSpatialCost8Neighborhood(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "MappingI1 test succeeded.";
	}
	else
	{
		Log::info() << "MappingI1 test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestMappingI1, ApplyMapping_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestMappingI1::testApplyMapping(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestMappingI1, ApplyMapping_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestMappingI1::testApplyMapping(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestMappingI1, ApplyMapping_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestMappingI1::testApplyMapping(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestMappingI1, ApplyMapping_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestMappingI1::testApplyMapping(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 4u, GTEST_TEST_DURATION, worker));
}

TEST(TestMappingI1, ApplyMapping_6Channels)
{
	Worker worker;
	EXPECT_TRUE(TestMappingI1::testApplyMapping(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 6u, GTEST_TEST_DURATION, worker));
}

TEST(TestMappingI1, ApplyMapping_8Channels)
{
	Worker worker;
	EXPECT_TRUE(TestMappingI1::testApplyMapping(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 8u, GTEST_TEST_DURATION, worker));
}


TEST(TestMappingI1, SumSquaredDifference5x5MaskNoCenter_1Channel)
{
	EXPECT_TRUE((TestMappingI1::testSumSquaredDifference5x5MaskNoCenter<1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestMappingI1, SumSquaredDifference5x5MaskNoCenter_2Channels)
{
	EXPECT_TRUE((TestMappingI1::testSumSquaredDifference5x5MaskNoCenter<2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestMappingI1, SumSquaredDifference5x5MaskNoCenter_3Channels)
{
	EXPECT_TRUE((TestMappingI1::testSumSquaredDifference5x5MaskNoCenter<3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestMappingI1, SumSquaredDifference5x5MaskNoCenter_4Channels)
{
	EXPECT_TRUE((TestMappingI1::testSumSquaredDifference5x5MaskNoCenter<4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}


TEST(TestMappingI1, AppearanceCost5x5_1Channel)
{
	EXPECT_TRUE((TestMappingI1::testAppearanceCost5x5<1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestMappingI1, AppearanceCost5x5_2Channels)
{
	EXPECT_TRUE((TestMappingI1::testAppearanceCost5x5<2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestMappingI1, AppearanceCost5x5_3Channels)
{
	EXPECT_TRUE((TestMappingI1::testAppearanceCost5x5<3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestMappingI1, AppearanceCost5x5_4Channels)
{
	EXPECT_TRUE((TestMappingI1::testAppearanceCost5x5<4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}


TEST(TestMappingI1, SpatialCost4Neighborhood_1Channel)
{
	EXPECT_TRUE((TestMappingI1::testSpatialCost4Neighborhood<1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestMappingI1, SpatialCost4Neighborhood_2Channels)
{
	EXPECT_TRUE((TestMappingI1::testSpatialCost4Neighborhood<2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestMappingI1, SpatialCost4Neighborhood_3Channels)
{
	EXPECT_TRUE((TestMappingI1::testSpatialCost4Neighborhood<3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestMappingI1, SpatialCost4Neighborhood_4Channels)
{
	EXPECT_TRUE((TestMappingI1::testSpatialCost4Neighborhood<4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}


TEST(TestMappingI1, SpatialCost8Neighborhood_1Channel)
{
	EXPECT_TRUE((TestMappingI1::testSpatialCost8Neighborhood<1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestMappingI1, SpatialCost8Neighborhood_2Channels)
{
	EXPECT_TRUE((TestMappingI1::testSpatialCost8Neighborhood<2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestMappingI1, SpatialCost8Neighborhood_3Channels)
{
	EXPECT_TRUE((TestMappingI1::testSpatialCost8Neighborhood<3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestMappingI1, SpatialCost8Neighborhood_4Channels)
{
	EXPECT_TRUE((TestMappingI1::testSpatialCost8Neighborhood<4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

#endif // OCEAN_USE_GTEST

bool TestMappingI1::testApplyMapping(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing apply mapping for " << width << "x" << height << ":";

	bool allSucceeded = true;

	for (const unsigned int channels : {1u, 2u, 3u, 4u, 6u, 8u})
	{
		Log::info() << " ";

		if (!testApplyMapping(width, height, channels, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Apply mapping test succeeded.";
	}
	else
	{
		Log::info() << "Apply mapping test FAILED!";
	}

	return allSucceeded;
}

bool TestMappingI1::testApplyMapping(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(channels >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << channels << " channels:";

	bool allSucceeded = true;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	RandomGenerator randomGenerator;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		const Timestamp startTimestamp(true);

		do
		{
			for (const bool performanceIteration : {true, false})
			{
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 3u, width);
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 3u, height);

				Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
				const Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

				CV::Synthesis::MappingI1 mapping(frame.width(), frame.height());
				mapping.reset();

				const Frame frameCopy(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				for (unsigned int y = 0u; y < mask.height(); ++y)
				{
					const uint8_t* maskRow = mask.constrow<uint8_t>(y);

					for (unsigned int x = 0u; x < mask.width(); ++x)
					{
						if (maskRow[x] == 0x00)
						{
							unsigned int sourceX, sourceY;

							do
							{
								sourceX = RandomI::random(randomGenerator, mask.width() - 1u);
								sourceY = RandomI::random(randomGenerator, mask.height() - 1u);
							}
							while (mask.constpixel<uint8_t>(sourceX, sourceY)[0] != 0xFF);

							mapping.setPosition(x, y, CV::PixelPosition(sourceX, sourceY));
						}
					}
				}

				CV::PixelBoundingBox boundingBox(CV::PixelPosition(0u, 0u), frame.width(), frame.height());

				if (!performanceIteration)
				{
					const unsigned int left = RandomI::random(randomGenerator, frame.width() - 1u);
					const unsigned int right = RandomI::random(randomGenerator, left, frame.width() - 1u);

					const unsigned int top = RandomI::random(randomGenerator, frame.height() - 1u);
					const unsigned int bottom = RandomI::random(randomGenerator, top, frame.height() - 1u);

					boundingBox = CV::PixelBoundingBox(left, top, right, bottom);
				}

				performance.startIf(performanceIteration);
					mapping.applyMapping(frame, mask, boundingBox.left(), boundingBox.width(), boundingBox.top(), boundingBox.height(), useWorker);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, frameCopy))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (!validateMapping(frame, mask, mapping, boundingBox))
				{
					allSucceeded = false;
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms";
		Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 2u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 2u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 2u) << "x";
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

bool TestMappingI1::testSumSquaredDifference5x5MaskNoCenter(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 5x5 SSD with mask skipping center pixel for " << width << "x" << height << ":";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testSumSquaredDifference5x5MaskNoCenter<1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testSumSquaredDifference5x5MaskNoCenter<2u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testSumSquaredDifference5x5MaskNoCenter<3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testSumSquaredDifference5x5MaskNoCenter<4u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "5x5 SSD with mask test succeeded.";
	}
	else
	{
		Log::info() << "5x5 SSD with mask test FAILED!";
	}

	return allSucceeded;
}

template <unsigned int tChannels>
bool TestMappingI1::testSumSquaredDifference5x5MaskNoCenter(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	constexpr size_t iterations = 1000;

	Log::info() << "... for " << tChannels << " channels:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool performanceIteration : {true, false})
		{
			const unsigned int testWidth = performanceIteration ? width : RandomI::random(5u, width);
			const unsigned int testHeight = performanceIteration ? height : RandomI::random(5u, height);

			const Frame frame0 = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
			const Frame frame1 = CV::CVUtilities::randomizedFrame(frame0.frameType(), &randomGenerator);

			const Frame mask0 = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

			CV::PixelPositions topLeftPositions0;
			topLeftPositions0.reserve(iterations);

			while (topLeftPositions0.size() < iterations)
			{
				const unsigned int x = RandomI::random(frame0.width() - 5u);
				const unsigned int y = RandomI::random(frame0.height() - 5u);

				topLeftPositions0.emplace_back(x, y);
			}

			CV::PixelPositions topLeftPositions1;
			topLeftPositions1.reserve(iterations);

			while (topLeftPositions1.size() < iterations)
			{
				const unsigned int x = RandomI::random(frame1.width() - 5u);
				const unsigned int y = RandomI::random(frame1.height() - 5u);

				topLeftPositions1.emplace_back(x, y);
			}

			Indices32 results(iterations);

			constexpr unsigned int tBorderFactor = 27u;

			const unsigned int frame0PaddingElements = frame0.paddingElements();
			const unsigned int frame1PaddingElements = frame1.paddingElements();
			const unsigned int mask0PaddingElements = mask0.paddingElements();

			const unsigned int frame0StrideElements = frame0.strideElements();
			const unsigned int frame1StrideElements = frame1.strideElements();
			const unsigned int mask0StrideElements = mask0.strideElements();

			performance.startIf(performanceIteration);

				for (size_t n = 0; n < iterations; ++n)
				{
					const CV::PixelPosition& topLeftPosition0 = topLeftPositions0[n];
					const CV::PixelPosition& topLeftPosition1 = topLeftPositions1[n];

					const uint8_t* framePointer0 = frame0.constdata<uint8_t>() + topLeftPosition0.y() * frame0StrideElements + topLeftPosition0.x() * tChannels;
					const uint8_t* framePointer1 = frame1.constdata<uint8_t>() + topLeftPosition1.y() * frame1StrideElements + topLeftPosition1.x() * tChannels;
					const uint8_t* mask0Pointer = mask0.constdata<uint8_t>() + topLeftPosition0.y() * mask0StrideElements + topLeftPosition0.x();

					const unsigned int result = CV::Synthesis::MappingI::ssd5x5MaskNoCenter<tChannels, tBorderFactor>(framePointer0, framePointer1, mask0Pointer, frame0.width(), frame1.width(), frame0PaddingElements, frame1PaddingElements, mask0PaddingElements);

					results[n] = result;
				}

			performance.stopIf(performanceIteration);

			if (!validateSumSquaredDifference5x5MaskNoCenter(frame0, frame1, mask0, topLeftPositions0, topLeftPositions1, results, tBorderFactor))
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 3u) << "ms, average: " << String::toAString(performance.averageMseconds(), 3u) << "ms";

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

bool TestMappingI1::testAppearanceCost5x5(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 5x5 appearance cost for " << width << "x" << height << ":";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testAppearanceCost5x5<1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testAppearanceCost5x5<2u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testAppearanceCost5x5<3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testAppearanceCost5x5<4u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "5x5 appearance cost test succeeded.";
	}
	else
	{
		Log::info() << "5x5 appearance cost test FAILED!";
	}

	return allSucceeded;
}

template <unsigned int tChannels>
bool TestMappingI1::testAppearanceCost5x5(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	constexpr size_t iterations = 1000;

	Log::info() << "... for " << tChannels << " channels:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	constexpr unsigned int patchSize = 5u;
	constexpr unsigned int borderFactor = 5u;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int testWidth = RandomI::random(randomGenerator, 5u, width);
		const unsigned int testHeight = RandomI::random(randomGenerator, 5u, height);

		const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

		CV::Segmentation::MaskAnalyzer::determineDistancesToBorder8Bit(mask.data<uint8_t>(), mask.width(), mask.height(), mask.paddingElements(), patchSize + 1u, false /*assignFinal*/, CV::PixelBoundingBox());

		const CV::Synthesis::MappingI1 mapping(testWidth, testHeight);

		const unsigned int normalizationFactor = mapping.spatialCostNormalization<tChannels>();

		for (size_t n = 0; n < iterations; ++n)
		{
			CV::PixelPosition source;
			CV::PixelPosition target;

			while (true)
			{
				source = CV::PixelPosition(RandomI::random(randomGenerator, frame.width() - 1u), RandomI::random(randomGenerator, frame.height() - 1u));

				if (mask.constpixel<uint8_t>(source.x(), source.y())[0] == 0xFFu)
				{
					break;
				}
			}

			while (true)
			{
				target = CV::PixelPosition(RandomI::random(randomGenerator, frame.width() - 1u), RandomI::random(randomGenerator, frame.height() - 1u));

				if (mask.constpixel<uint8_t>(target.x(), target.y())[0] != 0xFFu)
				{
					break;
				}
			}

			const unsigned int cost = mapping.appearanceCost5x5<tChannels, borderFactor>(target.x(), target.y(), source.x(), source.y(), frame.constdata<uint8_t>(), mask.constdata<uint8_t>(), frame.paddingElements(), mask.paddingElements());

			const uint64_t testCost = determineAppearanceCost(frame, mask, source, target, patchSize, borderFactor, normalizationFactor);

			if (uint64_t(cost) != testCost)
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

bool TestMappingI1::testSpatialCost4Neighborhood(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 4-neighborhood spatial cost for " << width << "x" << height << ":";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testSpatialCost4Neighborhood<1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testSpatialCost4Neighborhood<2u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testSpatialCost4Neighborhood<3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testSpatialCost4Neighborhood<4u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "4-neighborhood spatial cost test succeeded.";
	}
	else
	{
		Log::info() << "4-neighborhood spatial cost test FAILED!";
	}

	return allSucceeded;
}

template <unsigned int tChannels>
bool TestMappingI1::testSpatialCost4Neighborhood(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channels!");

	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << tChannels << " channels:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int testWidth = RandomI::random(randomGenerator, 3u, width);
		const unsigned int testHeight = RandomI::random(randomGenerator, 3u, height);

		const Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

		CV::Synthesis::MappingI1 mapping(testWidth, testHeight);

		const unsigned int normalizationFactor = mapping.appearanceCostNormalization<tChannels>();

		for (unsigned int y = 0u; y < mask.height(); ++y)
		{
			for (unsigned int x = 0u; x < mask.width(); ++x)
			{
				if (mask.constpixel<uint8_t>(x, y)[0] == 0x00u)
				{
					unsigned int sourceX, sourceY;

					do
					{
						sourceX = RandomI::random(mask.width() - 1u);
						sourceY = RandomI::random(mask.height() - 1u);
					}
					while (mask.constpixel<uint8_t>(sourceX, sourceY)[0] != 0xFF);

					mapping.setPosition(x, y, CV::PixelPosition(sourceX, sourceY));
				}
			}
		}

		const unsigned int maxCost = std::max(1u, RandomI::random32(randomGenerator));

		unsigned int xTarget = (unsigned int)(-1);
		unsigned int yTarget = (unsigned int)(-1);

		do
		{
			xTarget = RandomI::random(randomGenerator, testWidth - 1u);
			yTarget = RandomI::random(randomGenerator, testHeight - 1u);
		}
		while (mask.constpixel<uint8_t>(xTarget, yTarget)[0] == 0xFFu);

		const unsigned int xSource = RandomI::random(randomGenerator, testWidth - 1u);
		const unsigned int ySource = RandomI::random(randomGenerator, testHeight - 1u);

		const uint32_t cost = mapping.spatialCost4Neighborhood<tChannels>(xTarget, yTarget, xSource, ySource, mask.constdata<uint8_t>(), mask.paddingElements(), maxCost);

		const uint64_t testCost = determineSpatialCost4Neighborhood(mapping, mask, CV::PixelPosition(xSource, ySource), CV::PixelPosition(xTarget, yTarget), maxCost, normalizationFactor);

		if (uint64_t(cost) != testCost)
		{
			allSucceeded = false;
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

bool TestMappingI1::testSpatialCost8Neighborhood(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 8-neighborhood spatial cost for " << width << "x" << height << ":";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testSpatialCost8Neighborhood<1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testSpatialCost8Neighborhood<2u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testSpatialCost8Neighborhood<3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testSpatialCost8Neighborhood<4u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "8-neighborhood spatial cost test succeeded.";
	}
	else
	{
		Log::info() << "8-neighborhood spatial cost test FAILED!";
	}

	return allSucceeded;
}

template <unsigned int tChannels>
bool TestMappingI1::testSpatialCost8Neighborhood(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channels!");

	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << tChannels << " channels:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int testWidth = RandomI::random(randomGenerator, 3u, width);
		const unsigned int testHeight = RandomI::random(randomGenerator, 3u, height);

		const Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

		CV::Synthesis::MappingI1 mapping(testWidth, testHeight);

		const unsigned int normalizationFactor = mapping.appearanceCostNormalization<tChannels>();

		for (unsigned int y = 0u; y < mask.height(); ++y)
		{
			for (unsigned int x = 0u; x < mask.width(); ++x)
			{
				if (mask.constpixel<uint8_t>(x, y)[0] == 0x00u)
				{
					unsigned int sourceX, sourceY;

					do
					{
						sourceX = RandomI::random(mask.width() - 1u);
						sourceY = RandomI::random(mask.height() - 1u);
					}
					while (mask.constpixel<uint8_t>(sourceX, sourceY)[0] != 0xFF);

					mapping.setPosition(x, y, CV::PixelPosition(sourceX, sourceY));
				}
			}
		}

		const unsigned int maxCost = std::max(1u, RandomI::random32(randomGenerator));

		unsigned int xTarget = (unsigned int)(-1);
		unsigned int yTarget = (unsigned int)(-1);

		do
		{
			xTarget = RandomI::random(randomGenerator, testWidth - 1u);
			yTarget = RandomI::random(randomGenerator, testHeight - 1u);
		}
		while (mask.constpixel<uint8_t>(xTarget, yTarget)[0] == 0xFFu);

		const unsigned int xSource = RandomI::random(randomGenerator, testWidth - 1u);
		const unsigned int ySource = RandomI::random(randomGenerator, testHeight - 1u);

		const uint32_t cost = mapping.spatialCost8Neighborhood<tChannels>(xTarget, yTarget, xSource, ySource, mask.constdata<uint8_t>(), mask.paddingElements(), maxCost);

		const uint64_t testCost = determineSpatialCost8Neighborhood(mapping, mask, CV::PixelPosition(xSource, ySource), CV::PixelPosition(xTarget, yTarget), maxCost, normalizationFactor);

		if (uint64_t(cost) != testCost)
		{
			allSucceeded = false;
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

bool TestMappingI1::validateMapping(const Frame& frame, const Frame& mask, const CV::Synthesis::MappingI1& mapping, const CV::PixelBoundingBox& boundingBox)
{
	ocean_assert(frame.isValid() && frame.numberPlanes() == 1u);
	ocean_assert(frame.isFrameTypeCompatible(FrameType(mask, frame.pixelFormat()), false));
	ocean_assert(boundingBox.isValid());

	if (!frame.isValid() || !frame.isFrameTypeCompatible(FrameType(mask, frame.pixelFormat()), false))
	{
		return false;
	}

	if (mapping.width() != frame.width() || mapping.height() != frame.height())
	{
		return false;
	}

	if (boundingBox.rightEnd() > frame.width() || boundingBox.bottomEnd() > frame.height())
	{
		ocean_assert(false && "Invalid bounding box!");
		return true;
	}

	const unsigned int channels = frame.channels();

	for (unsigned int y = boundingBox.top(); y < boundingBox.bottomEnd(); ++y)
	{
		for (unsigned int x = boundingBox.left(); x < boundingBox.rightEnd(); ++x)
		{
			const uint8_t maskPixel = mask.constpixel<uint8_t>(x, y)[0];

			const CV::PixelPosition& position = mapping.position(x, y);

			if (maskPixel != 0xFF)
			{
				const unsigned int sourceX = position.x();
				const unsigned int sourceY = position.y();

				if (sourceX >= frame.width() || sourceY >= frame.height())
				{
					return false;
				}

				const uint8_t* sourcePixel = frame.constpixel<uint8_t>(sourceX, sourceY);
				const uint8_t* targetPixel = frame.constpixel<uint8_t>(x, y);

				if (memcmp(sourcePixel, targetPixel, sizeof(uint8_t) * channels) != 0)
				{
					return false;
				}
			}
			else
			{
				if (position.isValid())
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool TestMappingI1::validateSumSquaredDifference5x5MaskNoCenter(const Frame& frame0, const Frame& frame1, const Frame& mask0, const CV::PixelPositions& topLeftPositions0, const CV::PixelPositions& topLeftPositions1, const Indices32& results, const unsigned int borderFactor)
{
	ocean_assert(frame0.isValid() && frame1.isValid() && mask0.isValid());
	ocean_assert(!topLeftPositions0.empty());
	ocean_assert(topLeftPositions0.size() == topLeftPositions1.size());
	ocean_assert(topLeftPositions0.size() == results.size());

	if (topLeftPositions0.size() != results.size())
	{
		return false;
	}

	for (size_t n = 0; n < topLeftPositions0.size(); ++n)
	{
		const CV::PixelPosition& topLeftPosition0 = topLeftPositions0[n];
		const CV::PixelPosition& topLeftPosition1 = topLeftPositions1[n];

		unsigned int ssdTest = 0u;

		for (unsigned int xx = 0u; xx < 5u; ++xx)
		{
			for (unsigned int yy = 0u; yy < 5u; ++yy)
			{
				if (xx != 2u || yy != 2u) // we skip the center pixel
				{
					const unsigned int xLocation0 = topLeftPosition0.x() + xx;
					const unsigned int yLocation0 = topLeftPosition0.y() + yy;

					const unsigned int xLocation1 = topLeftPosition1.x() + xx;
					const unsigned int yLocation1 = topLeftPosition1.y() + yy;

					ocean_assert(xLocation0 < frame0.width() && yLocation0 < frame0.height());
					ocean_assert(xLocation1 < frame1.width() && yLocation1 < frame1.height());

					const uint8_t* framePixel0 = frame0.constpixel<uint8_t>(xLocation0, yLocation0);
					const uint8_t* framePixel1 = frame1.constpixel<uint8_t>(xLocation1, yLocation1);
					const uint8_t* maskPixel0 = mask0.constpixel<uint8_t>(xLocation0, yLocation0);

					unsigned int ssdLocal = 0u;

					for (unsigned int c = 0u; c < frame0.channels(); ++c)
					{
						ssdLocal += sqrDistance(framePixel0[c], framePixel1[c]);
					}

					if (maskPixel0[0] == 0xFF)
					{
						ssdTest += ssdLocal * borderFactor;
					}
					else
					{
						ssdTest += ssdLocal;
					}
				}
			}
		}

		if (ssdTest != results[n])
		{
			return false;
		}
	}

	return true;
}

uint64_t TestMappingI1::determineAppearanceCost(const Frame& frame, const Frame& mask, const CV::PixelPosition& source, const CV::PixelPosition& target, const unsigned int patchSize, const unsigned int borderFactor, const unsigned int normalizationFactor)
{
	ocean_assert(frame.isValid() && mask.isValid());
	ocean_assert(frame.numberPlanes() == 1u && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);
	ocean_assert(frame.isFrameTypeCompatible(FrameType(mask, frame.pixelFormat()), false));

	ocean_assert(source.x() < frame.width() && source.y() < frame.height());
	ocean_assert(target.x() < frame.width() && target.y() < frame.height());

	ocean_assert(patchSize >= 1u && patchSize % 2u == 1u);
	ocean_assert(borderFactor >= 1u);
	ocean_assert(normalizationFactor >= 1u);

	constexpr uint64_t invalidCost = uint64_t(-1);

	const unsigned int patchSize_2 = patchSize / 2u;

	if (mask.constpixel<uint8_t>(source.x(), source.y())[0] != 0xFFu)
	{
		ocean_assert(false && "Invalid source location!");
		return invalidCost;
	}

	if (mask.constpixel<uint8_t>(target.x(), target.y())[0] == 0xFFu)
	{
		ocean_assert(false && "Invalid target location!");
		return invalidCost;
	}

	uint64_t cost = 0ull;

	for (int yy = -int(patchSize_2); yy <= int(patchSize_2); ++yy)
	{
		const unsigned int ySource = CV::CVUtilities::mirrorIndex(int(source.y()) + yy, frame.height());
		const unsigned int yTarget = CV::CVUtilities::mirrorIndex(int(target.y()) + yy, frame.height());

		for (int xx = -int(patchSize_2); xx <= int(patchSize_2); ++xx)
		{
			// we skip the center pixel

			if (yy != 0 || xx != 0)
			{
				const unsigned int xSource = CV::CVUtilities::mirrorIndex(int(source.x()) + xx, frame.width());
				const unsigned int xTarget = CV::CVUtilities::mirrorIndex(int(target.x()) + xx, frame.width());

				uint32_t ssd = 0u;

				const uint8_t* const sourcePixel = frame.constpixel<uint8_t>(xSource, ySource);
				const uint8_t* const targetPixel = frame.constpixel<uint8_t>(xTarget, yTarget);

				for (unsigned int n = 0u; n < frame.channels(); ++n)
				{
					ssd += uint32_t(NumericT<int32_t>::sqr(int32_t(sourcePixel[n]) - int32_t(targetPixel[n])));
				}

				const uint8_t maskPixelValue = mask.constpixel<uint8_t>(xTarget, yTarget)[0];

				if (maskPixelValue == 0xFFu)
				{
					cost += uint64_t(ssd) * uint64_t(borderFactor);
				}
				else
				{
					cost += uint64_t(ssd);
				}
			}
		}
	}

	cost *= uint64_t(normalizationFactor);

	cost /= uint64_t(patchSize * patchSize);

	return cost;
}

uint64_t TestMappingI1::determineSpatialCost4Neighborhood(const CV::Synthesis::MappingI1& mapping, const Frame& mask, const CV::PixelPosition& source, const CV::PixelPosition& target, const uint32_t maxCost, const unsigned int normalizationFactor)
{
	ocean_assert(mapping);
	ocean_assert(mask.isValid());

	if (mask.constpixel<uint8_t>(target.x(), target.y())[0] == 0xFFu)
	{
		ocean_assert(false && "This should never happen!");
		return maxCost;
	}

	uint32_t bestCost = uint32_t(-1);

	for (const CV::PixelDirection pixelDirection : {CV::PD_NORTH, CV::PD_EAST, CV::PD_SOUTH, CV::PD_WEST})
	{
		const CV::PixelPosition targetNeighbor = target.neighbor(pixelDirection);
		const CV::PixelPosition sourceNeighbor = source.neighbor(pixelDirection);

		if (targetNeighbor.x() < mask.width() && targetNeighbor.y() < mask.height())
		{
			if (mask.constpixel<uint8_t>(targetNeighbor.x(), targetNeighbor.y())[0] != 0xFFu)
			{
				const CV::PixelPosition& mappingPosition = mapping.position(targetNeighbor.x(), targetNeighbor.y());

				const uint32_t cost = mappingPosition.sqrDistance(sourceNeighbor);

				bestCost = std::min(bestCost, cost);
			}
		}
	}

	return std::min(uint64_t(bestCost) * uint64_t(normalizationFactor), uint64_t(maxCost));
}

uint64_t TestMappingI1::determineSpatialCost8Neighborhood(const CV::Synthesis::MappingI1& mapping, const Frame& mask, const CV::PixelPosition& source, const CV::PixelPosition& target, const uint32_t maxCost, const unsigned int normalizationFactor)
{
	ocean_assert(mapping);
	ocean_assert(mask.isValid());

	if (mask.constpixel<uint8_t>(target.x(), target.y())[0] == 0xFFu)
	{
		ocean_assert(false && "This should never happen!");
		return maxCost;
	}

	uint32_t bestCost = uint32_t(-1);

	for (int angle = 0; angle < 360; angle += 45)
	{
		const CV::PixelDirection pixelDirection = CV::PixelDirection(angle);

		const CV::PixelPosition targetNeighbor = target.neighbor(pixelDirection);
		const CV::PixelPosition sourceNeighbor = source.neighbor(pixelDirection);

		if (targetNeighbor.x() < mask.width() && targetNeighbor.y() < mask.height())
		{
			if (mask.constpixel<uint8_t>(targetNeighbor.x(), targetNeighbor.y())[0] != 0xFFu)
			{
				const CV::PixelPosition& mappingPosition = mapping.position(targetNeighbor.x(), targetNeighbor.y());

				const uint32_t cost = mappingPosition.sqrDistance(sourceNeighbor);

				bestCost = std::min(bestCost, cost);
			}
		}
	}

	return std::min(uint64_t(bestCost) * uint64_t(normalizationFactor), uint64_t(maxCost));
}

}

}

}

}
