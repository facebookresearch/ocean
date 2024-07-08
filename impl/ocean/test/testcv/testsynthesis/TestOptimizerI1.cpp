/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testsynthesis/TestOptimizerI1.h"
#include "ocean/test/testcv/testsynthesis/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/CVUtilities.h"

#include "ocean/cv/segmentation/MaskAnalyzer.h"

#include "ocean/cv/synthesis/Optimizer4NeighborhoodAreaConstrainedI1.h"
#include "ocean/cv/synthesis/Optimizer4NeighborhoodHighPerformanceI1.h"
#include "ocean/cv/synthesis/Optimizer4NeighborhoodHighPerformanceSkippingI1.h"
#include "ocean/cv/synthesis/Optimizer4NeighborhoodHighPerformanceSkippingByCostMaskI1.h"
#include "ocean/cv/synthesis/Optimizer4NeighborhoodStructuralConstrainedI1.h"

#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestSynthesis
{

bool TestOptimizerI1::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "OptimizerI1 test:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testAreaConstrained4Neighborhood(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHighPerformance4Neighborhood(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHighPerformance4NeighborhoodSkipping(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHighPerformance4NeighborhoodSkippingByCostMask(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testStructuralConstrained4Neighborhood(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "OptimizerI1 test succeeded.";
	}
	else
	{
		Log::info() << "OptimizerI1 test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestOptimizerI1, AreaConstrained4Neighborhood_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerI1::testAreaConstrained4Neighborhood(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestOptimizerI1, AreaConstrained4Neighborhood_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerI1::testAreaConstrained4Neighborhood(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestOptimizerI1, AreaConstrained4Neighborhood_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerI1::testAreaConstrained4Neighborhood(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestOptimizerI1, AreaConstrained4Neighborhood_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerI1::testAreaConstrained4Neighborhood(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 4u, GTEST_TEST_DURATION, worker));
}


TEST(TestOptimizerI1, HighPerformance4Neighborhood_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerI1::testHighPerformance4Neighborhood(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestOptimizerI1, HighPerformance4Neighborhood_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerI1::testHighPerformance4Neighborhood(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestOptimizerI1, HighPerformance4Neighborhood_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerI1::testHighPerformance4Neighborhood(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestOptimizerI1, HighPerformance4Neighborhood_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerI1::testHighPerformance4Neighborhood(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 4u, GTEST_TEST_DURATION, worker));
}


TEST(TestOptimizerI1, HighPerformance4NeighborhoodSkipping_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerI1::testHighPerformance4NeighborhoodSkipping(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestOptimizerI1, HighPerformance4NeighborhoodSkipping_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerI1::testHighPerformance4NeighborhoodSkipping(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestOptimizerI1, HighPerformance4NeighborhoodSkipping_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerI1::testHighPerformance4NeighborhoodSkipping(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestOptimizerI1, HighPerformance4NeighborhoodSkipping_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerI1::testHighPerformance4NeighborhoodSkipping(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 4u, GTEST_TEST_DURATION, worker));
}


TEST(TestOptimizerI1, HighPerformance4NeighborhoodSkippingByCostMask_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerI1::testHighPerformance4NeighborhoodSkippingByCostMask(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestOptimizerI1, HighPerformance4NeighborhoodSkippingByCostMask_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerI1::testHighPerformance4NeighborhoodSkippingByCostMask(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestOptimizerI1, HighPerformance4NeighborhoodSkippingByCostMask_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerI1::testHighPerformance4NeighborhoodSkippingByCostMask(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestOptimizerI1, HighPerformance4NeighborhoodSkippingByCostMask_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerI1::testHighPerformance4NeighborhoodSkippingByCostMask(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 4u, GTEST_TEST_DURATION, worker));
}


TEST(TestOptimizerI1, StructuralConstrained4Neighborhood_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerI1::testStructuralConstrained4Neighborhood(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestOptimizerI1, StructuralConstrained4Neighborhood_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerI1::testStructuralConstrained4Neighborhood(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestOptimizerI1, StructuralConstrained4Neighborhood_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerI1::testStructuralConstrained4Neighborhood(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestOptimizerI1, StructuralConstrained4Neighborhood_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerI1::testStructuralConstrained4Neighborhood(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 4u, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestOptimizerI1::testAreaConstrained4Neighborhood(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing area constrained 4-neighborhood optimizer for " << width << "x" << height << ":";

	bool allSucceeded = true;

	for (const unsigned int channels : {1u, 2u, 3u, 4u})
	{
		Log::info() << " ";

		if (!testAreaConstrained4Neighborhood(width, height, channels, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Area constrained 4-neighborhood optimizer test succeeded.";
	}
	else
	{
		Log::info() << "Area constrained 4-neighborhood optimizer test FAILED!";
	}

	return allSucceeded;
}

bool TestOptimizerI1::testAreaConstrained4Neighborhood(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(channels >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << channels << " channels:";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

#ifdef OCEAN_DEBUG
	constexpr unsigned int maxWorkerIterations = 1u;
#else
	const unsigned int maxWorkerIterations = worker ? 2u : 1u;
#endif

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		const Timestamp startTimestamp(true);

		do
		{
			for (const bool performanceIteration : {true, false})
			{
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 50u, width / 2u) * 2u;
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 50u, height / 2u) * 2u;

				Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

				Frame copyFrame(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

				constexpr unsigned int patchSize = 5u;

				CV::Segmentation::MaskAnalyzer::determineDistancesToBorder8Bit(mask.data<uint8_t>(), mask.width(), mask.height(), mask.paddingElements(), patchSize + 1u, false, CV::PixelBoundingBox(), useWorker);

				CV::Synthesis::LayerI1 layer(frame, mask);
				CV::Synthesis::MappingI1& mapping = layer.mappingI1();

				for (unsigned int y = 0u; y < mask.height(); ++y)
				{
					const uint8_t* maskRow = mask.constrow<uint8_t>(y);

					for (unsigned int x = 0u; x < mask.width(); ++x)
					{
						if (maskRow[x] != 0xFFu)
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

				CV::Synthesis::MappingI1 copyMapping(mapping);

				const Frame filterMask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

				const unsigned int randomSeed = randomGenerator.seed();

				constexpr unsigned int weightFactor = 5u;
				constexpr unsigned int borderFactor = 25u;
				constexpr bool updateFrame = true;

				constexpr unsigned int radii = 5u;
				constexpr unsigned int iterations = 4u;
				constexpr unsigned int maxSpatialCost = (unsigned int)(-1);
				constexpr bool applyInitialMapping = true;

				performance.startIf(performanceIteration);
					CV::Synthesis::Optimizer4NeighborhoodAreaConstrainedI1<weightFactor, borderFactor, updateFrame>(layer, randomGenerator, filterMask).invoke(radii, iterations, maxSpatialCost, useWorker, applyInitialMapping);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, copyFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (useWorker == nullptr)
				{
					RandomGenerator helperGenerator(randomSeed);

					constexpr bool spatialSkipping = false;

					if (optimize4Neighborhood<borderFactor>(copyFrame, mask, filterMask, Frame(), nullptr, copyMapping, helperGenerator, applyInitialMapping, radii, iterations, weightFactor, maxSpatialCost, spatialSkipping))
					{
						for (unsigned int y = 0u; y < frame.height(); ++y)
						{
							const uint8_t* maskRow = mask.constrow<uint8_t>(y);

							for (unsigned int x = 0u; x < frame.width(); ++x)
							{
								if (memcmp(frame.constpixel<uint8_t>(x, y), copyFrame.constpixel<uint8_t>(x, y), sizeof(uint8_t) * frame.channels()) != 0)
								{
									OCEAN_SET_FAILED(validation);
								}

								if (maskRow[x] != 0xFFu)
								{
									OCEAN_EXPECT_EQUAL(validation, mapping.position(x, y), copyMapping.position(x, y));
								}
							}
						}
					}
					else
					{
						OCEAN_SET_FAILED(validation);
					}
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

	Log::info() << " ";

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestOptimizerI1::testHighPerformance4Neighborhood(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing high performance 4-neighborhood optimizer for " << width << "x" << height << ":";

	bool allSucceeded = true;

	for (const unsigned int channels : {1u, 2u, 3u, 4u})
	{
		Log::info() << " ";

		if (!testHighPerformance4Neighborhood(width, height, channels, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "High performance 4-neighborhood optimizer test succeeded.";
	}
	else
	{
		Log::info() << "High performance 4-neighborhood optimizer test FAILED!";
	}

	return allSucceeded;
}

bool TestOptimizerI1::testHighPerformance4Neighborhood(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(channels >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << channels << " channels:";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

#ifdef OCEAN_DEBUG
	constexpr unsigned int maxWorkerIterations = 1u;
#else
	const unsigned int maxWorkerIterations = worker ? 2u : 1u;
#endif

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		const Timestamp startTimestamp(true);

		do
		{
			for (const bool performanceIteration : {true, false})
			{
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 50u, width / 2u) * 2u;
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 50u, height / 2u) * 2u;

				Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

				Frame copyFrame(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

				constexpr unsigned int patchSize = 5u;

				CV::Segmentation::MaskAnalyzer::determineDistancesToBorder8Bit(mask.data<uint8_t>(), mask.width(), mask.height(), mask.paddingElements(), patchSize + 1u, false, CV::PixelBoundingBox(), useWorker);

				CV::Synthesis::LayerI1 layer(frame, mask);
				CV::Synthesis::MappingI1& mapping = layer.mappingI1();

				for (unsigned int y = 0u; y < mask.height(); ++y)
				{
					const uint8_t* maskRow = mask.constrow<uint8_t>(y);

					for (unsigned int x = 0u; x < mask.width(); ++x)
					{
						if (maskRow[x] != 0xFFu)
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

				CV::Synthesis::MappingI1 copyMapping(mapping);

				const unsigned int randomSeed = randomGenerator.seed();

				constexpr unsigned int weightFactor = 5u;
				constexpr unsigned int borderFactor = 25u;
				constexpr bool updateFrame = true;

				constexpr unsigned int radii = 5u;
				constexpr unsigned int iterations = 4u;
				constexpr unsigned int maxSpatialCost = (unsigned int)(-1);
				constexpr bool applyInitialMapping = true;

				performance.startIf(performanceIteration);
					CV::Synthesis::Optimizer4NeighborhoodHighPerformanceI1<weightFactor, borderFactor, updateFrame>(layer, randomGenerator).invoke(radii, iterations, maxSpatialCost, useWorker, applyInitialMapping);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, copyFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (useWorker == nullptr)
				{
					RandomGenerator helperGenerator(randomSeed);

					constexpr bool spatialSkipping = false;

					if (optimize4Neighborhood<borderFactor>(copyFrame, mask, Frame(), Frame(), nullptr, copyMapping, helperGenerator, applyInitialMapping, radii, iterations, weightFactor, maxSpatialCost, spatialSkipping))
					{
						for (unsigned int y = 0u; y < frame.height(); ++y)
						{
							const uint8_t* maskRow = mask.constrow<uint8_t>(y);

							for (unsigned int x = 0u; x < frame.width(); ++x)
							{
								if (memcmp(frame.constpixel<uint8_t>(x, y), copyFrame.constpixel<uint8_t>(x, y), sizeof(uint8_t) * frame.channels()) != 0)
								{
									OCEAN_SET_FAILED(validation);
								}

								if (maskRow[x] != 0xFFu)
								{
									OCEAN_EXPECT_EQUAL(validation, mapping.position(x, y), copyMapping.position(x, y));
								}
							}
						}
					}
					else
					{
						OCEAN_SET_FAILED(validation);
					}
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

	Log::info() << " ";

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestOptimizerI1::testHighPerformance4NeighborhoodSkipping(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing high performance 4-neighborhood optimizer with spatial skipping for " << width << "x" << height << ":";

	bool allSucceeded = true;

	for (const unsigned int channels : {1u, 2u, 3u, 4u})
	{
		Log::info() << " ";

		if (!testHighPerformance4NeighborhoodSkipping(width, height, channels, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "High performance 4-neighborhood optimizer with spatial skipping test succeeded.";
	}
	else
	{
		Log::info() << "High performance 4-neighborhood optimizer with spatial skipping test FAILED!";
	}

	return allSucceeded;
}

bool TestOptimizerI1::testHighPerformance4NeighborhoodSkipping(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(channels >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << channels << " channels:";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

#ifdef OCEAN_DEBUG
	constexpr unsigned int maxWorkerIterations = 1u;
#else
	const unsigned int maxWorkerIterations = worker ? 2u : 1u;
#endif

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		const Timestamp startTimestamp(true);

		do
		{
			for (const bool performanceIteration : {true, false})
			{
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 50u, width / 2u) * 2u;
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 50u, height / 2u) * 2u;

				Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

				Frame copyFrame(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

				constexpr unsigned int patchSize = 5u;

				CV::Segmentation::MaskAnalyzer::determineDistancesToBorder8Bit(mask.data<uint8_t>(), mask.width(), mask.height(), mask.paddingElements(), patchSize + 1u, false, CV::PixelBoundingBox(), useWorker);

				CV::Synthesis::LayerI1 layer(frame, mask);
				CV::Synthesis::MappingI1& mapping = layer.mappingI1();

				for (unsigned int y = 0u; y < mask.height(); ++y)
				{
					const uint8_t* maskRow = mask.constrow<uint8_t>(y);

					for (unsigned int x = 0u; x < mask.width(); ++x)
					{
						if (maskRow[x] != 0xFFu)
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

				CV::Synthesis::MappingI1 copyMapping(mapping);

				const unsigned int randomSeed = randomGenerator.seed();

				constexpr unsigned int weightFactor = 5u;
				constexpr unsigned int borderFactor = 25u;
				constexpr bool updateFrame = true;

				constexpr unsigned int radii = 5u;
				constexpr unsigned int iterations = 4u;
				constexpr unsigned int maxSpatialCost = (unsigned int)(-1);
				constexpr bool applyInitialMapping = true;

				performance.startIf(performanceIteration);
					CV::Synthesis::Optimizer4NeighborhoodHighPerformanceSkippingI1<weightFactor, borderFactor, updateFrame>(layer, randomGenerator).invoke(radii, iterations, maxSpatialCost, useWorker, applyInitialMapping);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, copyFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (useWorker == nullptr)
				{
					RandomGenerator helperGenerator(randomSeed);

					constexpr bool spatialSkipping = true;

					if (optimize4Neighborhood<borderFactor>(copyFrame, mask, Frame(), Frame(), nullptr, copyMapping, helperGenerator, applyInitialMapping, radii, iterations, weightFactor, maxSpatialCost, spatialSkipping))
					{
						for (unsigned int y = 0u; y < frame.height(); ++y)
						{
							const uint8_t* maskRow = mask.constrow<uint8_t>(y);

							for (unsigned int x = 0u; x < frame.width(); ++x)
							{
								if (memcmp(frame.constpixel<uint8_t>(x, y), copyFrame.constpixel<uint8_t>(x, y), sizeof(uint8_t) * frame.channels()) != 0)
								{
									OCEAN_SET_FAILED(validation);
								}

								if (maskRow[x] != 0xFFu)
								{
									OCEAN_EXPECT_EQUAL(validation, mapping.position(x, y), copyMapping.position(x, y));
								}
							}
						}
					}
					else
					{
						OCEAN_SET_FAILED(validation);
					}
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

	Log::info() << " ";

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestOptimizerI1::testHighPerformance4NeighborhoodSkippingByCostMask(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing high performance 4-neighborhood optimizer with skipping mask for " << width << "x" << height << ":";

	bool allSucceeded = true;

	for (const unsigned int channels : {1u, 2u, 3u, 4u})
	{
		Log::info() << " ";

		if (!testHighPerformance4NeighborhoodSkippingByCostMask(width, height, channels, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "High performance 4-neighborhood optimizer with skipping mask test succeeded.";
	}
	else
	{
		Log::info() << "High performance 4-neighborhood optimizer with skipping mask test FAILED!";
	}

	return allSucceeded;
}

bool TestOptimizerI1::testHighPerformance4NeighborhoodSkippingByCostMask(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(channels >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << channels << " channels:";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

#ifdef OCEAN_DEBUG
	constexpr unsigned int maxWorkerIterations = 1u;
#else
	const unsigned int maxWorkerIterations = worker ? 2u : 1u;
#endif

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		const Timestamp startTimestamp(true);

		do
		{
			for (const bool performanceIteration : {true, false})
			{
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 50u, width / 2u) * 2u;
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 50u, height / 2u) * 2u;

				Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

				Frame copyFrame(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

				constexpr unsigned int patchSize = 5u;

				CV::Segmentation::MaskAnalyzer::determineDistancesToBorder8Bit(mask.data<uint8_t>(), mask.width(), mask.height(), mask.paddingElements(), patchSize + 1u, false, CV::PixelBoundingBox(), useWorker);

				CV::Synthesis::LayerI1 layer(frame, mask);
				CV::Synthesis::MappingI1& mapping = layer.mappingI1();

				for (unsigned int y = 0u; y < mask.height(); ++y)
				{
					const uint8_t* maskRow = mask.constrow<uint8_t>(y);

					for (unsigned int x = 0u; x < mask.width(); ++x)
					{
						if (maskRow[x] != 0xFFu)
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

				CV::Synthesis::MappingI1 copyMapping(mapping);

				const Frame skippingMask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

				const unsigned int randomSeed = randomGenerator.seed();

				constexpr unsigned int weightFactor = 5u;
				constexpr unsigned int borderFactor = 25u;
				constexpr bool updateFrame = true;

				constexpr unsigned int radii = 5u;
				constexpr unsigned int iterations = 4u;
				constexpr unsigned int maxSpatialCost = (unsigned int)(-1);
				constexpr bool applyInitialMapping = true;

				performance.startIf(performanceIteration);
					CV::Synthesis::Optimizer4NeighborhoodHighPerformanceSkippingByCostMaskI1<weightFactor, borderFactor, updateFrame>(layer, randomGenerator, skippingMask).invoke(radii, iterations, maxSpatialCost, useWorker, applyInitialMapping);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, copyFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (useWorker == nullptr)
				{
					RandomGenerator helperGenerator(randomSeed);

					constexpr bool spatialSkipping = false;

					if (optimize4Neighborhood<borderFactor>(copyFrame, mask, Frame(), skippingMask, nullptr, copyMapping, helperGenerator, applyInitialMapping, radii, iterations, weightFactor, maxSpatialCost, spatialSkipping))
					{
						for (unsigned int y = 0u; y < frame.height(); ++y)
						{
							const uint8_t* maskRow = mask.constrow<uint8_t>(y);

							for (unsigned int x = 0u; x < frame.width(); ++x)
							{
								if (memcmp(frame.constpixel<uint8_t>(x, y), copyFrame.constpixel<uint8_t>(x, y), sizeof(uint8_t) * frame.channels()) != 0)
								{
									OCEAN_SET_FAILED(validation);
								}

								if (maskRow[x] != 0xFFu)
								{
									OCEAN_EXPECT_EQUAL(validation, mapping.position(x, y), copyMapping.position(x, y));
								}
							}
						}
					}
					else
					{
						OCEAN_SET_FAILED(validation);
					}
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

	Log::info() << " ";

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestOptimizerI1::testStructuralConstrained4Neighborhood(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing structural constrained 4-neighborhood optimizer for " << width << "x" << height << ":";

	bool allSucceeded = true;

	for (const unsigned int channels : {1u, 2u, 3u, 4u})
	{
		Log::info() << " ";

		if (!testStructuralConstrained4Neighborhood(width, height, channels, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Structural constrained 4-neighborhood optimizer test succeeded.";
	}
	else
	{
		Log::info() << "Structural constrained 4-neighborhood optimizer test FAILED!";
	}

	return allSucceeded;
}

bool TestOptimizerI1::testStructuralConstrained4Neighborhood(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(channels >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << channels << " channels:";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

#ifdef OCEAN_DEBUG
	constexpr unsigned int maxWorkerIterations = 1u;
#else
	const unsigned int maxWorkerIterations = worker ? 2u : 1u;
#endif

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		const Timestamp startTimestamp(true);

		do
		{
			for (const bool performanceIteration : {true, false})
			{
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 50u, width / 2u) * 2u;
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 50u, height / 2u) * 2u;

				Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

				Frame copyFrame(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

				constexpr unsigned int patchSize = 5u;

				CV::Segmentation::MaskAnalyzer::determineDistancesToBorder8Bit(mask.data<uint8_t>(), mask.width(), mask.height(), mask.paddingElements(), patchSize + 1u, false, CV::PixelBoundingBox(), useWorker);

				CV::Synthesis::LayerI1 layer(frame, mask);
				CV::Synthesis::MappingI1& mapping = layer.mappingI1();

				for (unsigned int y = 0u; y < mask.height(); ++y)
				{
					const uint8_t* maskRow = mask.constrow<uint8_t>(y);

					for (unsigned int x = 0u; x < mask.width(); ++x)
					{
						if (maskRow[x] != 0xFFu)
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

				CV::Synthesis::MappingI1 copyMapping(mapping);

				CV::Synthesis::Constraints constraints;

				const unsigned int numberContraints = RandomI::random(randomGenerator, 1u, 5u);

				for (unsigned int n = 0u; n < numberContraints; ++n)
				{
					const Vector2 point0 = Random::vector2(randomGenerator, Scalar(5), Scalar(frame.width() - 6u), Scalar(5), Scalar(frame.height() - 6u));
					const Vector2 point1 = Random::vector2(randomGenerator, Scalar(5), Scalar(frame.width() - 6u), Scalar(5), Scalar(frame.height() - 6u));

					constexpr Scalar impact = 200;
					constexpr Scalar radius = 40;
					constexpr Scalar penality = 500;

					constraints.addConstraint(std::make_unique<CV::Synthesis::FiniteLineConstraint>(point0, point1, impact, radius, penality, true, true));
				}

				const unsigned int randomSeed = randomGenerator.seed();

				constexpr unsigned int weightFactor = 5u;
				constexpr unsigned int borderFactor = 25u;
				constexpr bool updateFrame = true;

				constexpr unsigned int radii = 5u;
				constexpr unsigned int iterations = 4u;
				constexpr unsigned int maxSpatialCost = (unsigned int)(-1);
				constexpr bool applyInitialMapping = true;

				performance.startIf(performanceIteration);
					CV::Synthesis::Optimizer4NeighborhoodStructuralConstrainedI1<weightFactor, borderFactor, updateFrame>(layer, randomGenerator, constraints).invoke(radii, iterations, maxSpatialCost, useWorker, applyInitialMapping);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, copyFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (useWorker == nullptr)
				{
					RandomGenerator helperGenerator(randomSeed);

					constexpr bool spatialSkipping = false;

					if (optimize4Neighborhood<borderFactor>(copyFrame, mask, Frame(), Frame(), &constraints, copyMapping,helperGenerator, applyInitialMapping, radii, iterations, weightFactor, maxSpatialCost, spatialSkipping))
					{
						for (unsigned int y = 0u; y < frame.height(); ++y)
						{
							for (unsigned int x = 0u; x < frame.width(); ++x)
							{
								const uint8_t* maskRow = mask.constrow<uint8_t>(y);

								if (memcmp(frame.constpixel<uint8_t>(x, y), copyFrame.constpixel<uint8_t>(x, y), sizeof(uint8_t) * frame.channels()) != 0)
								{
									OCEAN_SET_FAILED(validation);
								}

								if (maskRow[x] != 0xFFu)
								{
									OCEAN_EXPECT_EQUAL(validation, mapping.position(x, y), copyMapping.position(x, y));
								}
							}
						}
					}
					else
					{
						OCEAN_SET_FAILED(validation);
					}
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

	Log::info() << " ";

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <unsigned int tBorderFactor>
bool TestOptimizerI1::optimize4Neighborhood(Frame& frame, const Frame& mask, const Frame& filterMask, const Frame& skippingMask, const CV::Synthesis::Constraints* constraints, CV::Synthesis::MappingI1& mapping, RandomGenerator& randomGenerator, const bool applyInitialMapping, const unsigned int radii, const unsigned int iterations, const unsigned int weightFactor, const unsigned int maxSpatialCost, const bool spatialSkipping)
{
	static_assert(tBorderFactor != 0u, "Invalid border factor!");

	if (applyInitialMapping)
	{
		for (unsigned int y = 0u; y < frame.height(); ++y)
		{
			for (unsigned int x = 0u; x < frame.width(); ++x)
			{
				if (mask.constpixel<uint8_t>(x, y)[0] != 0xFFu)
				{
					const CV::PixelPosition& position = mapping.position(x, y);

					memcpy(frame.pixel<uint8_t>(x, y), frame.constpixel<uint8_t>(position.x(), position.y()), sizeof(uint8_t) * frame.channels());
				}
			}
		}
	}

	const std::vector<int> searchRadii = CV::Synthesis::OptimizerI::calculateSearchRadii(radii, frame.width(), frame.height());

	constexpr Scalar constraintWeightFactor = 180;

	for (unsigned int iteration = 0u; iteration < iterations; ++iteration)
	{
		for (const bool down : {true, false})
		{
			const unsigned int rowOffset = RandomI::random(randomGenerator, frame.height() - 1u);

			RandomGenerator localRandomGenerator(randomGenerator);

			if (down)
			{
				for (unsigned int yIteration = 0u; yIteration < mask.height(); ++yIteration)
				{
					const unsigned int y = (yIteration + rowOffset) % mask.height(); // picking a random start row

					for (unsigned int x = 0u; x < mask.width(); ++x)
					{
						bool foundImprovement = false;

						if (mask.constpixel<uint8_t>(x, y)[0] != 0xFFu)
						{
							if (skippingMask.isValid() && skippingMask.constpixel<uint8_t>(x, y)[0] == 0xFFu)
							{
								continue;
							}

							if (spatialSkipping && x != 0u && y != 0u && mapping.position(x - 1u, y).east() == mapping.position(x, y) && (mapping.position(x, y - 1u).south() == mapping.position(x, y)))
							{
								continue;
							}

							CV::PixelPosition bestMapping = mapping.position(x, y);

							uint64_t bestCost = determineCost<tBorderFactor>(frame, mask, mapping, CV::PixelPosition(x, y), bestMapping, weightFactor, maxSpatialCost);

							if (constraints != nullptr)
							{
								bestCost += uint64_t(constraints->cost(x, y, bestMapping.x(), bestMapping.y()) * constraintWeightFactor);
							}

							// propagation

							if (x > 0u && mask.constpixel<uint8_t>(x - 1u, y)[0] != 0xFFu)
							{
								const CV::PixelPosition candidateMapping = mapping.position(x - 1u, y).east();

								if (candidateMapping.x() < mask.width() && candidateMapping.y() < mask.height() && mask.constpixel<uint8_t>(candidateMapping.x(), candidateMapping.y())[0] == 0xFFu)
								{
									if (!filterMask.isValid() || filterMask.constpixel<uint8_t>(candidateMapping.x(), candidateMapping.y())[0] == 0xFFu)
									{
										uint64_t candidateCost = determineCost<tBorderFactor>(frame, mask, mapping, CV::PixelPosition(x, y), candidateMapping, weightFactor, maxSpatialCost);

										if (constraints != nullptr)
										{
											candidateCost += uint64_t(constraints->cost(x, y, candidateMapping.x(), candidateMapping.y()) * constraintWeightFactor);
										}

										if (candidateCost < bestCost)
										{
											bestMapping = candidateMapping;
											bestCost = candidateCost;

											foundImprovement = true;
										}
									}
								}
							}

							if (y > 0u && mask.constpixel<uint8_t>(x, y - 1u)[0] != 0xFFu)
							{
								const CV::PixelPosition candidateMapping = mapping.position(x, y - 1u).south();

								if (candidateMapping.x() < mask.width() && candidateMapping.y() < mask.height() && mask.constpixel<uint8_t>(candidateMapping.x(), candidateMapping.y())[0] == 0xFFu)
								{
									if (!filterMask.isValid() || filterMask.constpixel<uint8_t>(candidateMapping.x(), candidateMapping.y())[0] == 0xFFu)
									{
										uint64_t candidateCost = determineCost<tBorderFactor>(frame, mask, mapping, CV::PixelPosition(x, y), candidateMapping, weightFactor, maxSpatialCost);

										if (constraints != nullptr)
										{
											candidateCost += uint64_t(constraints->cost(x, y, candidateMapping.x(), candidateMapping.y()) * constraintWeightFactor);
										}

										if (candidateCost < bestCost)
										{
											bestMapping = candidateMapping;
											bestCost = candidateCost;

											foundImprovement = true;
										}
									}
								}
							}

							for (unsigned int n = 0u; n < radii; ++n)
							{
								const int searchRadius = searchRadii[n];

								const unsigned int xCandidateMapping = (unsigned int)(int(bestMapping.x()) + RandomI::random(localRandomGenerator, -searchRadius, searchRadius));
								const unsigned int yCandidateMapping = (unsigned int)(int(bestMapping.y()) + RandomI::random(localRandomGenerator, -searchRadius, searchRadius));

								const CV::PixelPosition candidateMapping(xCandidateMapping, yCandidateMapping);

								if (candidateMapping == bestMapping)
								{
									continue;
								}

								if (candidateMapping.x() >= frame.width() || candidateMapping.y() >= frame.height())
								{
									continue;
								}

								if (mask.constpixel<uint8_t>(candidateMapping.x(), candidateMapping.y())[0] != 0xFFu)
								{
									continue;
								}

								if (filterMask.isValid() && filterMask.constpixel<uint8_t>(candidateMapping.x(), candidateMapping.y())[0] != 0xFFu)
								{
									continue;
								}

								uint64_t candidateCost = determineCost<tBorderFactor>(frame, mask, mapping, CV::PixelPosition(x, y), candidateMapping, weightFactor, maxSpatialCost);

								if (constraints != nullptr)
								{
									candidateCost += uint64_t(constraints->cost(x, y, candidateMapping.x(), candidateMapping.y()) * constraintWeightFactor);
								}

								if (candidateCost < bestCost)
								{
									bestMapping = candidateMapping;
									bestCost = candidateCost;

									foundImprovement = true;
								}
							}

							if (foundImprovement)
							{
								mapping.setPosition(x, y, bestMapping);

								memcpy(frame.pixel<uint8_t>(x, y), frame.constpixel<uint8_t>(bestMapping.x(), bestMapping.y()), sizeof(uint8_t) * frame.channels());
							}
						}
					}
				}
			}
			else
			{
				ocean_assert(down == false);

				for (unsigned int yIteration = mask.height() - 1u; yIteration < mask.height(); --yIteration)
				{
					const unsigned int y = (yIteration + rowOffset) % mask.height(); // picking a random start row

					for (unsigned int x = mask.width() - 1u; x < mask.width(); --x)
					{
						bool foundImprovement = false;

						if (mask.constpixel<uint8_t>(x, y)[0] != 0xFFu)
						{
							if (skippingMask.isValid() && skippingMask.constpixel<uint8_t>(x, y)[0] == 0xFFu)
							{
								continue;
							}

							if (spatialSkipping && x != frame.width() - 1u && y != frame.height() - 1u && mapping.position(x + 1u, y).west() == mapping.position(x, y) && mapping.position(x, y + 1u).north() == mapping.position(x, y))
							{
								continue;
							}

							CV::PixelPosition bestMapping = mapping.position(x, y);

							uint64_t bestCost = determineCost<tBorderFactor>(frame, mask, mapping, CV::PixelPosition(x, y), bestMapping, weightFactor, maxSpatialCost);

							if (constraints != nullptr)
							{
								bestCost += uint64_t(constraints->cost(x, y, bestMapping.x(), bestMapping.y()) * constraintWeightFactor);
							}

							// propagation

							if (x < mask.width() - 1u && mask.constpixel<uint8_t>(x + 1u, y)[0] != 0xFFu)
							{
								const CV::PixelPosition candidateMapping = mapping.position(x + 1u, y).west();

								if (candidateMapping.x() < mask.width() && candidateMapping.y() < mask.height() && mask.constpixel<uint8_t>(candidateMapping.x(), candidateMapping.y())[0] == 0xFFu)
								{
									if (!filterMask.isValid() || filterMask.constpixel<uint8_t>(candidateMapping.x(), candidateMapping.y())[0] == 0xFFu)
									{
										uint64_t candidateCost = determineCost<tBorderFactor>(frame, mask, mapping, CV::PixelPosition(x, y), candidateMapping, weightFactor, maxSpatialCost);

										if (constraints != nullptr)
										{
											candidateCost += uint64_t(constraints->cost(x, y, candidateMapping.x(), candidateMapping.y()) * constraintWeightFactor);
										}

										if (candidateCost < bestCost)
										{
											bestMapping = candidateMapping;
											bestCost = candidateCost;

											foundImprovement = true;
										}
									}
								}
							}

							if (y < mask.height() - 1u && mask.constpixel<uint8_t>(x, y + 1u)[0] != 0xFFu)
							{
								const CV::PixelPosition candidateMapping = mapping.position(x, y + 1u).north();

								if (candidateMapping.x() < mask.width() && candidateMapping.y() < mask.height() && mask.constpixel<uint8_t>(candidateMapping.x(), candidateMapping.y())[0] == 0xFFu)
								{
									if (!filterMask.isValid() || filterMask.constpixel<uint8_t>(candidateMapping.x(), candidateMapping.y())[0] == 0xFFu)
									{
										uint64_t candidateCost = determineCost<tBorderFactor>(frame, mask, mapping, CV::PixelPosition(x, y), candidateMapping, weightFactor, maxSpatialCost);

										if (constraints != nullptr)
										{
											candidateCost += uint64_t(constraints->cost(x, y, candidateMapping.x(), candidateMapping.y()) * constraintWeightFactor);
										}

										if (candidateCost < bestCost)
										{
											bestMapping = candidateMapping;
											bestCost = candidateCost;

											foundImprovement = true;
										}
									}
								}
							}

							for (unsigned int n = 0u; n < radii; ++n)
							{
								const int searchRadius = searchRadii[n];

								const unsigned int xCandidateMapping = (unsigned int)(int(bestMapping.x()) + RandomI::random(localRandomGenerator, -searchRadius, searchRadius));
								const unsigned int yCandidateMapping = (unsigned int)(int(bestMapping.y()) + RandomI::random(localRandomGenerator, -searchRadius, searchRadius));

								const CV::PixelPosition candidateMapping(xCandidateMapping, yCandidateMapping);

								if (candidateMapping == bestMapping)
								{
									continue;
								}

								if (candidateMapping.x() >= frame.width() || candidateMapping.y() >= frame.height())
								{
									continue;
								}

								if (mask.constpixel<uint8_t>(candidateMapping.x(), candidateMapping.y())[0] != 0xFFu)
								{
									continue;
								}

								if (filterMask.isValid() && filterMask.constpixel<uint8_t>(candidateMapping.x(), candidateMapping.y())[0] != 0xFFu)
								{
									continue;
								}

								uint64_t candidateCost = determineCost<tBorderFactor>(frame, mask, mapping, CV::PixelPosition(x, y), candidateMapping, weightFactor, maxSpatialCost);

								if (constraints != nullptr)
								{
									candidateCost += uint64_t(constraints->cost(x, y, candidateMapping.x(), candidateMapping.y()) * constraintWeightFactor);
								}

								if (candidateCost < bestCost)
								{
									bestMapping = candidateMapping;
									bestCost = candidateCost;

									foundImprovement = true;
								}
							}

							if (foundImprovement)
							{
								mapping.setPosition(x, y, bestMapping);

								memcpy(frame.pixel<uint8_t>(x, y), frame.constpixel<uint8_t>(bestMapping.x(), bestMapping.y()), sizeof(uint8_t) * frame.channels());
							}
						}
					}
				}
			}
		}
	}

	return true;
}

template <unsigned int tBorderFactor>
uint64_t TestOptimizerI1::determineCost(const Frame& frame, const Frame& mask, const CV::Synthesis::MappingI1& mapping, const CV::PixelPosition& targetPosition, const CV::PixelPosition& sourcePosition, const unsigned int weightFactor, const unsigned int maxSpatialCost)
{
	uint32_t spatialCost = uint32_t(-1);
	uint32_t appearanceCost = uint32_t(-1);

	switch (frame.channels())
	{
		case 1u:
			spatialCost = mapping.spatialCost4Neighborhood<1u>(targetPosition.x(), targetPosition.y(), sourcePosition.x(), sourcePosition.y(), mask.constdata<uint8_t>(), mask.paddingElements(), maxSpatialCost);
			appearanceCost = mapping.appearanceCost5x5<1u, tBorderFactor>(targetPosition.x(), targetPosition.y(), sourcePosition.x(), sourcePosition.y(), frame.constdata<uint8_t>(), mask.constdata<uint8_t>(), frame.paddingElements(), mask.paddingElements());
			break;

		case 2u:
			spatialCost = mapping.spatialCost4Neighborhood<2u>(targetPosition.x(), targetPosition.y(), sourcePosition.x(), sourcePosition.y(), mask.constdata<uint8_t>(), mask.paddingElements(), maxSpatialCost);
			appearanceCost = mapping.appearanceCost5x5<2u, tBorderFactor>(targetPosition.x(), targetPosition.y(), sourcePosition.x(), sourcePosition.y(), frame.constdata<uint8_t>(), mask.constdata<uint8_t>(), frame.paddingElements(), mask.paddingElements());
			break;

		case 3u:
			spatialCost = mapping.spatialCost4Neighborhood<3u>(targetPosition.x(), targetPosition.y(), sourcePosition.x(), sourcePosition.y(), mask.constdata<uint8_t>(), mask.paddingElements(), maxSpatialCost);
			appearanceCost = mapping.appearanceCost5x5<3u, tBorderFactor>(targetPosition.x(), targetPosition.y(), sourcePosition.x(), sourcePosition.y(), frame.constdata<uint8_t>(), mask.constdata<uint8_t>(), frame.paddingElements(), mask.paddingElements());
			break;

		case 4u:
			spatialCost = mapping.spatialCost4Neighborhood<4u>(targetPosition.x(), targetPosition.y(), sourcePosition.x(), sourcePosition.y(), mask.constdata<uint8_t>(), mask.paddingElements(), maxSpatialCost);
			appearanceCost = mapping.appearanceCost5x5<4u, tBorderFactor>(targetPosition.x(), targetPosition.y(), sourcePosition.x(), sourcePosition.y(), frame.constdata<uint8_t>(), mask.constdata<uint8_t>(), frame.paddingElements(), mask.paddingElements());
			break;

		default:
			ocean_assert(false && "This should never happen!");
	}

	return uint64_t(weightFactor) * uint64_t(spatialCost) + uint64_t(appearanceCost);
}

}

}

}

}
