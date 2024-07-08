/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testsynthesis/TestOptimizerF1.h"
#include "ocean/test/testcv/testsynthesis/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/CVUtilities.h"

#include "ocean/cv/segmentation/MaskAnalyzer.h"

#include "ocean/cv/synthesis/Optimizer4NeighborhoodHighPerformanceF1.h"
#include "ocean/cv/synthesis/Optimizer4NeighborhoodReferenceFrameF1.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestSynthesis
{

bool TestOptimizerF1::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "OptimizerF1 test:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testHighPerformance4Neighborhood(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testReferenceFrame4Neighborhood(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "OptimizerF1 test succeeded.";
	}
	else
	{
		Log::info() << "OptimizerF1 test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestOptimizerF1, HighPerformance4Neighborhood_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerF1::testHighPerformance4Neighborhood(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestOptimizerF1, HighPerformance4Neighborhood_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerF1::testHighPerformance4Neighborhood(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestOptimizerF1, HighPerformance4Neighborhood_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerF1::testHighPerformance4Neighborhood(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestOptimizerF1, HighPerformance4Neighborhood_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerF1::testHighPerformance4Neighborhood(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 4u, GTEST_TEST_DURATION, worker));
}


TEST(TestOptimizerF1, ReferenceFrame4Neighborhood_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerF1::testReferenceFrame4Neighborhood(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestOptimizerF1, ReferenceFrame4Neighborhood_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerF1::testReferenceFrame4Neighborhood(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestOptimizerF1, ReferenceFrame4Neighborhood_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerF1::testReferenceFrame4Neighborhood(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestOptimizerF1, ReferenceFrame4Neighborhood_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestOptimizerF1::testReferenceFrame4Neighborhood(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 4u, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestOptimizerF1::testHighPerformance4Neighborhood(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
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

bool TestOptimizerF1::testHighPerformance4Neighborhood(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(channels >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << channels << " channels:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

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

				// adding a 2-pixel border not including any mask pixel
				mask.subFrame(0u, 0u, mask.width(), 2u).setValue(0xFFu);
				mask.subFrame(0u, 0u, 2u, mask.height()).setValue(0xFFu);
				mask.subFrame(mask.width() - 2u, 0u, 2u, mask.height()).setValue(0xFFu);
				mask.subFrame(0u, mask.height() - 2u, mask.width(), 2u).setValue(0xFFu);

				constexpr unsigned int patchSize = 5u;
				constexpr unsigned int patchSize_2 = patchSize / 2u;

				CV::Segmentation::MaskAnalyzer::determineDistancesToBorder8Bit(mask.data<uint8_t>(), mask.width(), mask.height(), mask.paddingElements(), patchSize + 1u, false, CV::PixelBoundingBox(), useWorker);

				CV::Synthesis::LayerF1 layer(frame, mask);
				CV::Synthesis::MappingF1& mapping = layer.mappingF1();

				for (unsigned int y = 0u; y < mask.height(); ++y)
				{
					for (unsigned int x = 0u; x < mask.width(); ++x)
					{
						if (mask.constpixel<uint8_t>(x, y)[0] != 0xFFu)
						{
							Scalar sourceX, sourceY;

							while (true)
							{
								sourceX = Random::scalar(randomGenerator, Scalar(patchSize_2), Scalar(mask.width() - patchSize_2 - 1u) - Numeric::weakEps());
								sourceY = Random::scalar(randomGenerator, Scalar(patchSize_2), Scalar(mask.height() - patchSize_2 - 1u) - Numeric::weakEps());

								const int xInt = Numeric::round32(sourceX);
								const int yInt = Numeric::round32(sourceY);

								bool positionAccepted = true;

								for (int yy = -1; positionAccepted && yy <= 1; ++yy)
								{
									for (int xx = -1; positionAccepted && xx <= 1; ++xx)
									{
										const unsigned int xLocation = (unsigned int)(xInt + xx);
										const unsigned int yLocation = (unsigned int)(yInt + yy);

										if (xLocation < mask.width() && yLocation < mask.height())
										{
											if (mask.constpixel<uint8_t>(xLocation, yLocation)[0] != 0xFF)
											{
												positionAccepted = false;
											}
										}
									}
								}

								if (positionAccepted)
								{
									break;
								}
							}

							mapping.setPosition(x, y, Vector2(sourceX, sourceY));
						}
					}
				}

				CV::Synthesis::MappingF1 copyMapping(mapping);

				const unsigned int randomSeed = randomGenerator.seed();

				constexpr unsigned int weightFactor = 5u;
				constexpr unsigned int borderFactor = 25u;
				constexpr bool updateFrame = true;

				constexpr unsigned int radii = 5u;
				constexpr unsigned int iterations = 4u;
				constexpr unsigned int maxSpatialCost = (unsigned int)(-1);
				constexpr bool applyInitialMapping = true;

				performance.startIf(performanceIteration);
					CV::Synthesis::Optimizer4NeighborhoodHighPerformanceF1<weightFactor, borderFactor, updateFrame>(layer, randomGenerator).invoke(radii, iterations, maxSpatialCost, useWorker, applyInitialMapping);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, copyFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (useWorker == nullptr)
				{
					RandomGenerator helperGenerator(randomSeed);

					if (optimize4Neighborhood<borderFactor>(copyFrame, mask, copyMapping, helperGenerator, applyInitialMapping, radii, iterations, weightFactor, maxSpatialCost))
					{
						for (unsigned int y = 0u; y < frame.height(); ++y)
						{
							for (unsigned int x = 0u; x < frame.width(); ++x)
							{
								if (memcmp(frame.constpixel<uint8_t>(x, y), copyFrame.constpixel<uint8_t>(x, y), sizeof(uint8_t) * frame.channels()) != 0)
								{
									allSucceeded = false;
								}

								if (mapping.position(x, y) != copyMapping.position(x, y))
								{
									allSucceeded = false;
								}
							}
						}
					}
					else
					{
						allSucceeded = false;
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

bool TestOptimizerF1::testReferenceFrame4Neighborhood(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 4-neighborhood optimizer with a reference frame for " << width << "x" << height << ":";

	bool allSucceeded = true;

	for (const unsigned int channels : {1u, 2u, 3u, 4u})
	{
		Log::info() << " ";

		if (!testReferenceFrame4Neighborhood(width, height, channels, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "4-neighborhood optimizer with reference frame test succeeded.";
	}
	else
	{
		Log::info() << "4-neighborhood optimizer with reference frame test FAILED!";
	}

	return allSucceeded;
}

bool TestOptimizerF1::testReferenceFrame4Neighborhood(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(channels >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << channels << " channels:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

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

				Frame referenceFrame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

				Frame copyFrame(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

				// adding a 2-pixel border not including any mask pixel
				mask.subFrame(0u, 0u, mask.width(), 2u).setValue(0xFFu);
				mask.subFrame(0u, 0u, 2u, mask.height()).setValue(0xFFu);
				mask.subFrame(mask.width() - 2u, 0u, 2u, mask.height()).setValue(0xFFu);
				mask.subFrame(0u, mask.height() - 2u, mask.width(), 2u).setValue(0xFFu);

				constexpr unsigned int patchSize = 5u;
				constexpr unsigned int patchSize_2 = patchSize / 2u;

				CV::Segmentation::MaskAnalyzer::determineDistancesToBorder8Bit(mask.data<uint8_t>(), mask.width(), mask.height(), mask.paddingElements(), patchSize + 1u, false, CV::PixelBoundingBox(), useWorker);

				CV::Synthesis::LayerF1 layer(frame, mask);
				CV::Synthesis::MappingF1& mapping = layer.mappingF1();

				for (unsigned int y = 0u; y < mask.height(); ++y)
				{
					for (unsigned int x = 0u; x < mask.width(); ++x)
					{
						if (mask.constpixel<uint8_t>(x, y)[0] != 0xFFu)
						{
							Scalar sourceX, sourceY;

							while (true)
							{
								sourceX = Random::scalar(randomGenerator, Scalar(patchSize_2), Scalar(mask.width() - patchSize_2 - 1u) - Numeric::weakEps());
								sourceY = Random::scalar(randomGenerator, Scalar(patchSize_2), Scalar(mask.height() - patchSize_2 - 1u) - Numeric::weakEps());

								const int xInt = Numeric::round32(sourceX);
								const int yInt = Numeric::round32(sourceY);

								bool positionAccepted = true;

								for (int yy = -1; positionAccepted && yy <= 1; ++yy)
								{
									for (int xx = -1; positionAccepted && xx <= 1; ++xx)
									{
										const unsigned int xLocation = (unsigned int)(xInt + xx);
										const unsigned int yLocation = (unsigned int)(yInt + yy);

										if (xLocation < mask.width() && yLocation < mask.height())
										{
											if (mask.constpixel<uint8_t>(xLocation, yLocation)[0] != 0xFF)
											{
												positionAccepted = false;
											}
										}
									}
								}

								if (positionAccepted)
								{
									break;
								}
							}

							mapping.setPosition(x, y, Vector2(sourceX, sourceY));
						}
					}
				}

				CV::Synthesis::MappingF1 copyMapping(mapping);

				const unsigned int randomSeed = randomGenerator.seed();

				constexpr unsigned int weightFactor = 5u;
				constexpr unsigned int borderFactor = 25u;
				constexpr bool updateFrame = true;

				constexpr unsigned int radii = 5u;
				constexpr unsigned int iterations = 4u;
				constexpr unsigned int maxSpatialCost = (unsigned int)(-1);
				constexpr bool applyInitialMapping = true;

				performance.startIf(performanceIteration);
					CV::Synthesis::Optimizer4NeighborhoodReferenceFrameF1<weightFactor, borderFactor, updateFrame>(layer, randomGenerator, referenceFrame).invoke(radii, iterations, maxSpatialCost, useWorker, applyInitialMapping);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, copyFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (useWorker == nullptr)
				{
					RandomGenerator helperGenerator(randomSeed);

					if (optimizeReference4Neighborhood<borderFactor>(copyFrame, mask, referenceFrame, copyMapping, helperGenerator, applyInitialMapping, radii, iterations, weightFactor, maxSpatialCost))
					{
						for (unsigned int y = 0u; y < frame.height(); ++y)
						{
							for (unsigned int x = 0u; x < frame.width(); ++x)
							{
								if (memcmp(frame.constpixel<uint8_t>(x, y), copyFrame.constpixel<uint8_t>(x, y), sizeof(uint8_t) * frame.channels()) != 0)
								{
									allSucceeded = false;
								}

								if (mapping.position(x, y) != copyMapping.position(x, y))
								{
									allSucceeded = false;
								}
							}
						}
					}
					else
					{
						allSucceeded = false;
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

template <unsigned int tBorderFactor>
bool TestOptimizerF1::optimize4Neighborhood(Frame& frame, const Frame& mask, CV::Synthesis::MappingF1& mapping, RandomGenerator& randomGenerator, const bool applyInitialMapping, const unsigned int radii, const unsigned int iterations, const unsigned int weightFactor, const unsigned int maxSpatialCost)
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
					const Vector2& position = mapping.position(x, y);

					if (!CV::FrameInterpolatorBilinear::Comfort::interpolatePixel8BitPerChannel(frame.constdata<uint8_t>(), frame.channels(), frame.width(), frame.height(), frame.paddingElements(), CV::PC_TOP_LEFT, position, frame.pixel<uint8_t>(x, y)))
					{
						ocean_assert(false && "This should never happen!");
						return false;
					}
				}
			}
		}
	}

	const Scalars searchRadii = CV::Synthesis::OptimizerF::calculateSearchRadii(radii, frame.width(), frame.height());

	constexpr unsigned int patchSize = 5u;
	constexpr unsigned int patchSize_2 = patchSize / 2u;

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
							Vector2 bestMapping = mapping.position(x, y);

							Scalar bestCost = determineCost<tBorderFactor>(frame, mask, mapping, CV::PixelPosition(x, y), bestMapping, weightFactor, maxSpatialCost);

							// propagation

							if (x > 0u && mask.constpixel<uint8_t>(x - 1u, y)[0] != 0xFFu)
							{
								const Vector2 candidateMapping = mapping.position(x - 1u, y) + Vector2(1, 0);

								if (candidateMapping.x() >= Scalar(patchSize_2) && candidateMapping.x() < Scalar(mask.width() - patchSize_2 - 1u) && candidateMapping.y() >= Scalar(patchSize_2) && candidateMapping.y() < Scalar(mask.height() - patchSize_2 - 1u))
								{
									const unsigned int xRoundedCandidateMapping = (unsigned int)(Numeric::round32(candidateMapping.x()));
									const unsigned int yRoundedCandidateMapping = (unsigned int)(Numeric::round32(candidateMapping.y()));

									if (mask.constpixel<uint8_t>(xRoundedCandidateMapping, yRoundedCandidateMapping)[0] == 0xFFu)
									{
										const Scalar candidateCost = determineCost<tBorderFactor>(frame, mask, mapping, CV::PixelPosition(x, y), candidateMapping, weightFactor, maxSpatialCost);

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
								const Vector2 candidateMapping = mapping.position(x, y - 1u) + Vector2(0, 1);

								if (candidateMapping.x() >= Scalar(patchSize_2) && candidateMapping.x() < Scalar(mask.width() - patchSize_2 - 1u) && candidateMapping.y() >= Scalar(patchSize_2) && candidateMapping.y() < Scalar(mask.height() - patchSize_2 - 1u))
								{
									const unsigned int xRoundedCandidateMapping = (unsigned int)(Numeric::round32(candidateMapping.x()));
									const unsigned int yRoundedCandidateMapping = (unsigned int)(Numeric::round32(candidateMapping.y()));

									if (mask.constpixel<uint8_t>(xRoundedCandidateMapping, yRoundedCandidateMapping)[0] == 0xFFu)
									{
										const Scalar candidateCost = determineCost<tBorderFactor>(frame, mask, mapping, CV::PixelPosition(x, y), candidateMapping, weightFactor, maxSpatialCost);

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
								const Scalar searchRadius = searchRadii[n];

								const Vector2 candidateMapping = bestMapping + Random::vector2(localRandomGenerator, -searchRadius, searchRadius);

								if (candidateMapping == bestMapping)
								{
									continue;
								}

								if (candidateMapping.x() < Scalar(patchSize_2) || candidateMapping.x() >= Scalar(frame.width() - patchSize_2 - 1u) || candidateMapping.y() < Scalar(patchSize_2) || candidateMapping.y() >= Scalar(frame.height() - patchSize_2 - 1u))
								{
									continue;
								}

								const unsigned int xRoundedCandidateMapping = (unsigned int)(Numeric::round32(candidateMapping.x()));
								const unsigned int yRoundedCandidateMapping = (unsigned int)(Numeric::round32(candidateMapping.y()));

								if (mask.constpixel<uint8_t>(xRoundedCandidateMapping, yRoundedCandidateMapping)[0] != 0xFFu)
								{
									continue;
								}

								const Scalar candidateCost = determineCost<tBorderFactor>(frame, mask, mapping, CV::PixelPosition(x, y), candidateMapping, weightFactor, maxSpatialCost);

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

								if (!CV::FrameInterpolatorBilinear::Comfort::interpolatePixel8BitPerChannel(frame.constdata<uint8_t>(), frame.channels(), frame.width(), frame.height(), frame.paddingElements(), CV::PC_TOP_LEFT, bestMapping, frame.pixel<uint8_t>(x, y)))
								{
									ocean_assert(false && "This should never happen!");
									return false;
								}
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
							Vector2 bestMapping = mapping.position(x, y);

							Scalar bestCost = determineCost<tBorderFactor>(frame, mask, mapping, CV::PixelPosition(x, y), bestMapping, weightFactor, maxSpatialCost);

							// propagation

							if (x < mask.width() - 1u && mask.constpixel<uint8_t>(x + 1u, y)[0] != 0xFFu)
							{
								const Vector2 candidateMapping = mapping.position(x + 1u, y) - Vector2(1, 0);

								if (candidateMapping.x() >= Scalar(patchSize_2) && candidateMapping.x() < Scalar(mask.width() - patchSize_2 - 1u) && candidateMapping.y() >= Scalar(patchSize_2) && candidateMapping.y() < Scalar(mask.height() - patchSize_2 - 1u))
								{
									const unsigned int xRoundedCandidateMapping = (unsigned int)(Numeric::round32(candidateMapping.x()));
									const unsigned int yRoundedCandidateMapping = (unsigned int)(Numeric::round32(candidateMapping.y()));

									if (mask.constpixel<uint8_t>(xRoundedCandidateMapping, yRoundedCandidateMapping)[0] == 0xFFu)
									{
										const Scalar candidateCost = determineCost<tBorderFactor>(frame, mask, mapping, CV::PixelPosition(x, y), candidateMapping, weightFactor, maxSpatialCost);

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
								const Vector2 candidateMapping = mapping.position(x, y + 1u) - Vector2(0, 1);

								if (candidateMapping.x() >= Scalar(patchSize_2) && candidateMapping.x() < Scalar(mask.width() - patchSize_2 - 1u) && candidateMapping.y() >= Scalar(patchSize_2) && candidateMapping.y() < Scalar(mask.height() - patchSize_2 - 1u))
								{
									const unsigned int xRoundedCandidateMapping = (unsigned int)(Numeric::round32(candidateMapping.x()));
									const unsigned int yRoundedCandidateMapping = (unsigned int)(Numeric::round32(candidateMapping.y()));

									if (mask.constpixel<uint8_t>(xRoundedCandidateMapping, yRoundedCandidateMapping)[0] == 0xFFu)
									{
										const Scalar candidateCost = determineCost<tBorderFactor>(frame, mask, mapping, CV::PixelPosition(x, y), candidateMapping, weightFactor, maxSpatialCost);

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
								const Scalar searchRadius = searchRadii[n];

								const Vector2 candidateMapping = bestMapping + Random::vector2(localRandomGenerator, -searchRadius, searchRadius);

								if (candidateMapping == bestMapping)
								{
									continue;
								}

								if (candidateMapping.x() < Scalar(patchSize_2) || candidateMapping.x() >= Scalar(frame.width() - patchSize_2 - 1u) || candidateMapping.y() < Scalar(patchSize_2) || candidateMapping.y() >= Scalar(frame.height() - patchSize_2 - 1u))
								{
									continue;
								}

								const unsigned int xRoundedCandidateMapping = (unsigned int)(Numeric::round32(candidateMapping.x()));
								const unsigned int yRoundedCandidateMapping = (unsigned int)(Numeric::round32(candidateMapping.y()));

								if (mask.constpixel<uint8_t>(xRoundedCandidateMapping, yRoundedCandidateMapping)[0] != 0xFFu)
								{
									continue;
								}

								const Scalar candidateCost = determineCost<tBorderFactor>(frame, mask, mapping, CV::PixelPosition(x, y), candidateMapping, weightFactor, maxSpatialCost);

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

								if (!CV::FrameInterpolatorBilinear::Comfort::interpolatePixel8BitPerChannel(frame.constdata<uint8_t>(), frame.channels(), frame.width(), frame.height(), frame.paddingElements(), CV::PC_TOP_LEFT, bestMapping, frame.pixel<uint8_t>(x, y)))
								{
									ocean_assert(false && "This should never happen!");
									return false;
								}
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
bool TestOptimizerF1::optimizeReference4Neighborhood(Frame& frame, const Frame& mask, const Frame& reference, CV::Synthesis::MappingF1& mapping, RandomGenerator& randomGenerator, const bool applyInitialMapping, const unsigned int radii, const unsigned int iterations, const unsigned int weightFactor, const unsigned int maxSpatialCost)
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
					const Vector2& position = mapping.position(x, y);

					if (!CV::FrameInterpolatorBilinear::Comfort::interpolatePixel8BitPerChannel(frame.constdata<uint8_t>(), frame.channels(), frame.width(), frame.height(), frame.paddingElements(), CV::PC_TOP_LEFT, position, frame.pixel<uint8_t>(x, y)))
					{
						ocean_assert(false && "This should never happen!");
						return false;
					}
				}
			}
		}
	}

	const Scalars searchRadii = CV::Synthesis::OptimizerF::calculateSearchRadii(radii, frame.width(), frame.height());

	constexpr unsigned int patchSize = 5u;
	constexpr unsigned int patchSize_2 = patchSize / 2u;

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
							Vector2 bestMapping = mapping.position(x, y);

							Scalar bestCost = determineCost<tBorderFactor>(frame, mask, reference, mapping, CV::PixelPosition(x, y), bestMapping, weightFactor, maxSpatialCost);

							// propagation

							if (x > 0u && mask.constpixel<uint8_t>(x - 1u, y)[0] != 0xFFu)
							{
								const Vector2 candidateMapping = mapping.position(x - 1u, y) + Vector2(1, 0);

								if (candidateMapping.x() >= Scalar(patchSize_2) && candidateMapping.x() < Scalar(mask.width() - patchSize_2 - 1u) && candidateMapping.y() >= Scalar(patchSize_2) && candidateMapping.y() < Scalar(mask.height() - patchSize_2 - 1u))
								{
									const unsigned int xRoundedCandidateMapping = (unsigned int)(Numeric::round32(candidateMapping.x()));
									const unsigned int yRoundedCandidateMapping = (unsigned int)(Numeric::round32(candidateMapping.y()));

									if (mask.constpixel<uint8_t>(xRoundedCandidateMapping, yRoundedCandidateMapping)[0] == 0xFFu)
									{
										const Scalar candidateCost = determineCost<tBorderFactor>(frame, mask, reference, mapping, CV::PixelPosition(x, y), candidateMapping, weightFactor, maxSpatialCost);

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
								const Vector2 candidateMapping = mapping.position(x, y - 1u) + Vector2(0, 1);

								if (candidateMapping.x() >= Scalar(patchSize_2) && candidateMapping.x() < Scalar(mask.width() - patchSize_2 - 1u) && candidateMapping.y() >= Scalar(patchSize_2) && candidateMapping.y() < Scalar(mask.height() - patchSize_2 - 1u))
								{
									const unsigned int xRoundedCandidateMapping = (unsigned int)(Numeric::round32(candidateMapping.x()));
									const unsigned int yRoundedCandidateMapping = (unsigned int)(Numeric::round32(candidateMapping.y()));

									if (mask.constpixel<uint8_t>(xRoundedCandidateMapping, yRoundedCandidateMapping)[0] == 0xFFu)
									{
										const Scalar candidateCost = determineCost<tBorderFactor>(frame, mask, reference, mapping, CV::PixelPosition(x, y), candidateMapping, weightFactor, maxSpatialCost);

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
								const Scalar searchRadius = searchRadii[n];

								const Vector2 candidateMapping = bestMapping + Random::vector2(localRandomGenerator, -searchRadius, searchRadius);

								if (candidateMapping == bestMapping)
								{
									continue;
								}

								if (candidateMapping.x() < Scalar(patchSize_2) || candidateMapping.x() >= Scalar(frame.width() - patchSize_2 - 1u) || candidateMapping.y() < Scalar(patchSize_2) || candidateMapping.y() >= Scalar(frame.height() - patchSize_2 - 1u))
								{
									continue;
								}

								const unsigned int xRoundedCandidateMapping = (unsigned int)(Numeric::round32(candidateMapping.x()));
								const unsigned int yRoundedCandidateMapping = (unsigned int)(Numeric::round32(candidateMapping.y()));

								if (mask.constpixel<uint8_t>(xRoundedCandidateMapping, yRoundedCandidateMapping)[0] != 0xFFu)
								{
									continue;
								}

								const Scalar candidateCost = determineCost<tBorderFactor>(frame, mask, reference, mapping, CV::PixelPosition(x, y), candidateMapping, weightFactor, maxSpatialCost);

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

								if (!CV::FrameInterpolatorBilinear::Comfort::interpolatePixel8BitPerChannel(frame.constdata<uint8_t>(), frame.channels(), frame.width(), frame.height(), frame.paddingElements(), CV::PC_TOP_LEFT, bestMapping, frame.pixel<uint8_t>(x, y)))
								{
									ocean_assert(false && "This should never happen!");
									return false;
								}
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
							Vector2 bestMapping = mapping.position(x, y);

							Scalar bestCost = determineCost<tBorderFactor>(frame, mask, reference, mapping, CV::PixelPosition(x, y), bestMapping, weightFactor, maxSpatialCost);

							// propagation

							if (x < mask.width() - 1u && mask.constpixel<uint8_t>(x + 1u, y)[0] != 0xFFu)
							{
								const Vector2 candidateMapping = mapping.position(x + 1u, y) - Vector2(1, 0);

								if (candidateMapping.x() >= Scalar(patchSize_2) && candidateMapping.x() < Scalar(mask.width() - patchSize_2 - 1u) && candidateMapping.y() >= Scalar(patchSize_2) && candidateMapping.y() < Scalar(mask.height() - patchSize_2 - 1u))
								{
									const unsigned int xRoundedCandidateMapping = (unsigned int)(Numeric::round32(candidateMapping.x()));
									const unsigned int yRoundedCandidateMapping = (unsigned int)(Numeric::round32(candidateMapping.y()));

									if (mask.constpixel<uint8_t>(xRoundedCandidateMapping, yRoundedCandidateMapping)[0] == 0xFFu)
									{
										const Scalar candidateCost = determineCost<tBorderFactor>(frame, mask, reference, mapping, CV::PixelPosition(x, y), candidateMapping, weightFactor, maxSpatialCost);

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
								const Vector2 candidateMapping = mapping.position(x, y + 1u) - Vector2(0, 1);

								if (candidateMapping.x() >= Scalar(patchSize_2) && candidateMapping.x() < Scalar(mask.width() - patchSize_2 - 1u) && candidateMapping.y() >= Scalar(patchSize_2) && candidateMapping.y() < Scalar(mask.height() - patchSize_2 - 1u))
								{
									const unsigned int xRoundedCandidateMapping = (unsigned int)(Numeric::round32(candidateMapping.x()));
									const unsigned int yRoundedCandidateMapping = (unsigned int)(Numeric::round32(candidateMapping.y()));

									if (mask.constpixel<uint8_t>(xRoundedCandidateMapping, yRoundedCandidateMapping)[0] == 0xFFu)
									{
										const Scalar candidateCost = determineCost<tBorderFactor>(frame, mask, reference, mapping, CV::PixelPosition(x, y), candidateMapping, weightFactor, maxSpatialCost);

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
								const Scalar searchRadius = searchRadii[n];

								const Vector2 candidateMapping = bestMapping + Random::vector2(localRandomGenerator, -searchRadius, searchRadius);

								if (candidateMapping == bestMapping)
								{
									continue;
								}

								if (candidateMapping.x() < Scalar(patchSize_2) || candidateMapping.x() >= Scalar(frame.width() - patchSize_2 - 1u) || candidateMapping.y() < Scalar(patchSize_2) || candidateMapping.y() >= Scalar(frame.height() - patchSize_2 - 1u))
								{
									continue;
								}

								const unsigned int xRoundedCandidateMapping = (unsigned int)(Numeric::round32(candidateMapping.x()));
								const unsigned int yRoundedCandidateMapping = (unsigned int)(Numeric::round32(candidateMapping.y()));

								if (mask.constpixel<uint8_t>(xRoundedCandidateMapping, yRoundedCandidateMapping)[0] != 0xFFu)
								{
									continue;
								}

								const Scalar candidateCost = determineCost<tBorderFactor>(frame, mask, reference, mapping, CV::PixelPosition(x, y), candidateMapping, weightFactor, maxSpatialCost);

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

								if (!CV::FrameInterpolatorBilinear::Comfort::interpolatePixel8BitPerChannel(frame.constdata<uint8_t>(), frame.channels(), frame.width(), frame.height(), frame.paddingElements(), CV::PC_TOP_LEFT, bestMapping, frame.pixel<uint8_t>(x, y)))
								{
									ocean_assert(false && "This should never happen!");
									return false;
								}
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
Scalar TestOptimizerF1::determineCost(const Frame& frame, const Frame& mask, const CV::Synthesis::MappingF1& mapping, const CV::PixelPosition& targetPosition, const Vector2& sourcePosition, const unsigned int weightFactor, const unsigned int maxSpatialCost)
{
	Scalar spatialCost = Numeric::maxValue();
	uint32_t appearanceCost = uint32_t(-1);

	switch (frame.channels())
	{
		case 1u:
			spatialCost = mapping.spatialCost4Neighborhood<1u>(targetPosition.x(), targetPosition.y(), sourcePosition.x(), sourcePosition.y(), mask.constdata<uint8_t>(), mask.paddingElements(), Scalar(maxSpatialCost));
			appearanceCost = mapping.appearanceCost5x5<1u>(targetPosition.x(), targetPosition.y(), sourcePosition.x(), sourcePosition.y(), frame.constdata<uint8_t>(), mask.constdata<uint8_t>(), frame.paddingElements(), mask.paddingElements(), tBorderFactor);
			break;

		case 2u:
			spatialCost = mapping.spatialCost4Neighborhood<2u>(targetPosition.x(), targetPosition.y(), sourcePosition.x(), sourcePosition.y(), mask.constdata<uint8_t>(), mask.paddingElements(), Scalar(maxSpatialCost));
			appearanceCost = mapping.appearanceCost5x5<2u>(targetPosition.x(), targetPosition.y(), sourcePosition.x(), sourcePosition.y(), frame.constdata<uint8_t>(), mask.constdata<uint8_t>(), frame.paddingElements(), mask.paddingElements(), tBorderFactor);
			break;

		case 3u:
			spatialCost = mapping.spatialCost4Neighborhood<3u>(targetPosition.x(), targetPosition.y(), sourcePosition.x(), sourcePosition.y(), mask.constdata<uint8_t>(), mask.paddingElements(), Scalar(maxSpatialCost));
			appearanceCost = mapping.appearanceCost5x5<3u>(targetPosition.x(), targetPosition.y(), sourcePosition.x(), sourcePosition.y(), frame.constdata<uint8_t>(), mask.constdata<uint8_t>(), frame.paddingElements(), mask.paddingElements(), tBorderFactor);
			break;

		case 4u:
			spatialCost = mapping.spatialCost4Neighborhood<4u>(targetPosition.x(), targetPosition.y(), sourcePosition.x(), sourcePosition.y(), mask.constdata<uint8_t>(), mask.paddingElements(), Scalar(maxSpatialCost));
			appearanceCost = mapping.appearanceCost5x5<4u>(targetPosition.x(), targetPosition.y(), sourcePosition.x(), sourcePosition.y(), frame.constdata<uint8_t>(), mask.constdata<uint8_t>(), frame.paddingElements(), mask.paddingElements(), tBorderFactor);
			break;

		default:
			ocean_assert(false && "This should never happen!");
	}

	return Scalar(weightFactor) * spatialCost + Scalar(appearanceCost);
}

template <unsigned int tBorderFactor>
Scalar TestOptimizerF1::determineCost(const Frame& frame, const Frame& mask, const Frame& reference, const CV::Synthesis::MappingF1& mapping, const CV::PixelPosition& targetPosition, const Vector2& sourcePosition, const unsigned int weightFactor, const unsigned int maxSpatialCost)
{
	Scalar spatialCost = Numeric::maxValue();
	uint32_t appearanceCost = uint32_t(-1);

	switch (frame.channels())
	{
		case 1u:
			spatialCost = mapping.spatialCost4Neighborhood<1u>(targetPosition.x(), targetPosition.y(), sourcePosition.x(), sourcePosition.y(), mask.constdata<uint8_t>(), mask.paddingElements(), Scalar(maxSpatialCost));
			appearanceCost = mapping.appearanceReferenceCost5x5<1u>(targetPosition.x(), targetPosition.y(), sourcePosition.x(), sourcePosition.y(), frame.constdata<uint8_t>(), mask.constdata<uint8_t>(), reference.constdata<uint8_t>(), frame.paddingElements(), mask.paddingElements(), reference.paddingElements(), tBorderFactor);
			break;

		case 2u:
			spatialCost = mapping.spatialCost4Neighborhood<2u>(targetPosition.x(), targetPosition.y(), sourcePosition.x(), sourcePosition.y(), mask.constdata<uint8_t>(), mask.paddingElements(), Scalar(maxSpatialCost));
			appearanceCost = mapping.appearanceReferenceCost5x5<2u>(targetPosition.x(), targetPosition.y(), sourcePosition.x(), sourcePosition.y(), frame.constdata<uint8_t>(), mask.constdata<uint8_t>(), reference.constdata<uint8_t>(), frame.paddingElements(), mask.paddingElements(), reference.paddingElements(), tBorderFactor);
			break;

		case 3u:
			spatialCost = mapping.spatialCost4Neighborhood<3u>(targetPosition.x(), targetPosition.y(), sourcePosition.x(), sourcePosition.y(), mask.constdata<uint8_t>(), mask.paddingElements(), Scalar(maxSpatialCost));
			appearanceCost = mapping.appearanceReferenceCost5x5<3u>(targetPosition.x(), targetPosition.y(), sourcePosition.x(), sourcePosition.y(), frame.constdata<uint8_t>(), mask.constdata<uint8_t>(), reference.constdata<uint8_t>(), frame.paddingElements(), mask.paddingElements(), reference.paddingElements(), tBorderFactor);
			break;

		case 4u:
			spatialCost = mapping.spatialCost4Neighborhood<4u>(targetPosition.x(), targetPosition.y(), sourcePosition.x(), sourcePosition.y(), mask.constdata<uint8_t>(), mask.paddingElements(), Scalar(maxSpatialCost));
			appearanceCost = mapping.appearanceReferenceCost5x5<4u>(targetPosition.x(), targetPosition.y(), sourcePosition.x(), sourcePosition.y(), frame.constdata<uint8_t>(), mask.constdata<uint8_t>(), reference.constdata<uint8_t>(), frame.paddingElements(), mask.paddingElements(), reference.paddingElements(), tBorderFactor);
			break;

		default:
			ocean_assert(false && "This should never happen!");
	}

	return Scalar(weightFactor) * spatialCost + Scalar(appearanceCost);
}

}

}

}

}
