/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testsynthesis/TestCreatorI1.h"
#include "ocean/test/testcv/testsynthesis/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/MaskAnalyzer.h"

#include "ocean/cv/segmentation/MaskAnalyzer.h"

#include "ocean/cv/synthesis/CreatorInpaintingContentI1.h"
#include "ocean/cv/synthesis/CreatorInformationCost4NeighborhoodI1.h"
#include "ocean/cv/synthesis/CreatorInformationSpatialCostI1.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestSynthesis
{

bool TestCreatorI1::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "CreatorI1 test:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testInpaintingContent(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testInformationSpatialCost(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testInformationCost4Neighborhood(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "CreatorI1 test succeeded.";
	}
	else
	{
		Log::info() << "CreatorI1 test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestCreatorI1, InpaintingContent_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestCreatorI1::testInpaintingContent(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestCreatorI1, InpaintingContent_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestCreatorI1::testInpaintingContent(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestCreatorI1, InpaintingContent_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestCreatorI1::testInpaintingContent(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestCreatorI1, InpaintingContent_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestCreatorI1::testInpaintingContent(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 4u, GTEST_TEST_DURATION, worker));
}


TEST(TestCreatorI1, InformationSpatialCost)
{
	Worker worker;
	EXPECT_TRUE(TestCreatorI1::testInformationSpatialCost(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker));
}


TEST(TestCreatorI1, InformationCost4Neighborhood_1Channel)
{
	Worker worker;
	EXPECT_TRUE((TestCreatorI1::testInformationCost4Neighborhood<1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestCreatorI1, InformationCost4Neighborhood_2Channels)
{
	Worker worker;
	EXPECT_TRUE((TestCreatorI1::testInformationCost4Neighborhood<2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestCreatorI1, InformationCost4Neighborhood_3Channels)
{
	Worker worker;
	EXPECT_TRUE((TestCreatorI1::testInformationCost4Neighborhood<3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

TEST(TestCreatorI1, InformationCost4Neighborhood_4Channels)
{
	Worker worker;
	EXPECT_TRUE((TestCreatorI1::testInformationCost4Neighborhood<4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker)));
}

#endif // OCEAN_USE_GTEST

bool TestCreatorI1::testInpaintingContent(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing inpainting content for " << width << "x" << height << ":";

	bool allSucceeded = true;

	for (const unsigned int channels : {1u, 2u, 3u, 4u})
	{
		Log::info() << " ";

		if (!testInpaintingContent(width, height, channels, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Inpainting content test succeeded.";
	}
	else
	{
		Log::info() << "Inpainting content test FAILED!";
	}

	return allSucceeded;
}

bool TestCreatorI1::testInpaintingContent(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(channels >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << channels << " channels:";

	bool allSucceeded = true;

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
			for (const bool performanceIteration : {true, false})
			{
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 3u, width);
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 3u, height);

				Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

				const Frame copyFrame(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				const Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

				CV::PixelBoundingBox boundingBox;
				if (RandomI::random(randomGenerator, 1u) == 0u)
				{
					boundingBox = CV::MaskAnalyzer::detectBoundingBox(mask.constdata<uint8_t>(), mask.width(), mask.height(), 0xFFu, mask.paddingElements());
					ocean_assert(boundingBox.isValid());
				}

				CV::Synthesis::LayerI1 layer(frame, mask, boundingBox);

				// we create a random mapping

				CV::Synthesis::MappingI1& mapping = layer.mappingI1();

				for (unsigned int y = 0u; y < frame.height(); ++y)
				{
					const uint8_t* rowMask = mask.constrow<uint8_t>(y);
					CV::PixelPosition* rowMapping = mapping() + y * frame.width();

					for (unsigned int x = 0u; x < frame.width(); ++x)
					{
						if (rowMask[x] == 0x00u)
						{
							while (true)
							{
								const CV::PixelPosition sourcePixel(RandomI::random(randomGenerator, frame.width() - 1u), RandomI::random(randomGenerator, frame.height() - 1u));

								if (mask.constpixel<uint8_t>(sourcePixel.x(), sourcePixel.y())[0] == 0xFFu)
								{
									rowMapping[x] = sourcePixel;
									break;
								}
							}
						}
					}
				}

				CV::Synthesis::CreatorInpaintingContentI1 creator(layer, frame);

				performance.startIf(performanceIteration);
					creator.invoke(useWorker);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, copyFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				for (unsigned int y = 0u; y < frame.height(); ++y)
				{
					const CV::PixelPosition* rowMapping = mapping() + y * frame.width();

					for (unsigned int x = 0u; x < frame.width(); ++x)
					{
						if (mask.constpixel<uint8_t>(x, y)[0] != 0xFFu)
						{
							const CV::PixelPosition& sourcePixel = rowMapping[x];

							ocean_assert(mask.constpixel<uint8_t>(sourcePixel.x(), sourcePixel.y())[0] == 0xFFu);

							if (memcmp(frame.constpixel<uint8_t>(x, y), copyFrame.constpixel<uint8_t>(sourcePixel.x(), sourcePixel.y()), sizeof(uint8_t) * channels) != 0)
							{
								allSucceeded = false;
							}
						}
						else
						{
							// we do not expect any change

							if (memcmp(frame.constpixel<uint8_t>(x, y), copyFrame.constpixel<uint8_t>(x, y), sizeof(uint8_t) * channels) != 0)
							{
								allSucceeded = false;
							}
						}
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

bool TestCreatorI1::testInformationSpatialCost(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing spatial cost information for " << width << "x" << height << ":";

	bool allSucceeded = true;

	const CV::PixelPositionsI offsets =
	{
		CV::PixelPositionI(-1, 0),
		CV::PixelPositionI(1, 0),
		CV::PixelPositionI(0, -1),
		CV::PixelPositionI(0, 1)
	};

	RandomGenerator randomGenerator;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;

		const Timestamp startTimestamp(true);

		do
		{
			for (const bool onlyCenterPixels : {false, true})
			{
				for (const unsigned int neighborhood : {1u, 2u, 3u, 4u})
				{
					const unsigned int testWidth = RandomI::random(randomGenerator, 3u, width);
					const unsigned int testHeight = RandomI::random(randomGenerator, 3u, height);

					Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
					frame.setValue(0xFFu);

					const Frame copyFrame(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					const Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

					CV::PixelBoundingBox boundingBox;
					if (RandomI::random(randomGenerator, 1u) == 0u)
					{
						boundingBox = CV::MaskAnalyzer::detectBoundingBox(mask.constdata<uint8_t>(), mask.width(), mask.height(), 0xFFu, mask.paddingElements());
						ocean_assert(boundingBox.isValid());
					}

					CV::Synthesis::LayerI1 layer(frame, mask, boundingBox);

					// we create a random mapping

					CV::Synthesis::MappingI1& mapping = layer.mappingI1();

					for (unsigned int y = 0u; y < frame.height(); ++y)
					{
						const uint8_t* rowMask = mask.constrow<uint8_t>(y);
						CV::PixelPosition* rowMapping = mapping() + y * frame.width();

						for (unsigned int x = 0u; x < frame.width(); ++x)
						{
							if (rowMask[x] == 0x00u)
							{
								while (true)
								{
									const CV::PixelPosition sourcePixel(RandomI::random(randomGenerator, frame.width() - 1u), RandomI::random(randomGenerator, frame.height() - 1u));

									if (mask.constpixel<uint8_t>(sourcePixel.x(), sourcePixel.y())[0] == 0xFFu)
									{
										rowMapping[x] = sourcePixel;
										break;
									}
								}
							}
						}
					}

					// let's create some realistic mappings with neighboring mappings mapping to neighboring pixels

					for (unsigned int n = 0u; n < mask.pixels() / 10u; ++n)
					{
						const unsigned int x = RandomI::random(randomGenerator, mask.width() - 1u);
						const unsigned int y = RandomI::random(randomGenerator, mask.height() - 1u);

						if (mask.constpixel<uint8_t>(x, y)[0] == 0x00u)
						{
							const CV::PixelPosition& centerMapping = mapping.position(x, y);

							for (int yy = -1; yy <= 1; ++yy)
							{
								const unsigned int yPosition = (unsigned int)(int(y) + yy);

								if (yPosition < mask.height())
								{
									for (int xx = -1; xx <= 1; ++xx)
									{
										const unsigned int xPosition = (unsigned int)(int(x) + xx);

										if (xPosition < mask.width())
										{
											if (xx != 0 || yy != 0)
											{
												if (mask.constpixel<uint8_t>(xPosition, yPosition)[0] == 0x00)
												{
													const unsigned int xNeighbor = (unsigned int)(int(centerMapping.x()) + xx);
													const unsigned int yNeighbor = (unsigned int)(int(centerMapping.y()) + yy);

													if (xNeighbor < mask.width() && yNeighbor < mask.height())
													{
														mapping.position(xPosition, yPosition) = CV::PixelPosition(xNeighbor, yNeighbor);
													}
												}
											}
										}
									}
								}
							}
						}
					}

					switch (neighborhood)
					{
						case 1u:
						{
							if (onlyCenterPixels)
							{
								CV::Synthesis::CreatorInformationSpatialCostI1<1u, true>(layer, frame).invoke(useWorker);
							}
							else
							{
								CV::Synthesis::CreatorInformationSpatialCostI1<1u, false>(layer, frame).invoke(useWorker);
							}
							break;
						}

						case 2u:
						{
							if (onlyCenterPixels)
							{
								CV::Synthesis::CreatorInformationSpatialCostI1<2u, true>(layer, frame).invoke(useWorker);
							}
							else
							{
								CV::Synthesis::CreatorInformationSpatialCostI1<2u, false>(layer, frame).invoke(useWorker);
							}
							break;
						}

						case 3u:
						{
							if (onlyCenterPixels)
							{
								CV::Synthesis::CreatorInformationSpatialCostI1<3u, true>(layer, frame).invoke(useWorker);
							}
							else
							{
								CV::Synthesis::CreatorInformationSpatialCostI1<3u, false>(layer, frame).invoke(useWorker);
							}
							break;
						}

						case 4u:
						{
							if (onlyCenterPixels)
							{
								CV::Synthesis::CreatorInformationSpatialCostI1<4u, true>(layer, frame).invoke(useWorker);
							}
							else
							{
								CV::Synthesis::CreatorInformationSpatialCostI1<4u, false>(layer, frame).invoke(useWorker);
							}
							break;
						}

						default:
						{
							ocean_assert(false && "Invalid neighborhood!");
							allSucceeded = false;
							break;
						}
					}

					if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, copyFrame))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					for (unsigned int y = 0u; y < frame.height(); ++y)
					{
						for (unsigned int x = 0u; x < frame.width(); ++x)
						{
							uint8_t value = 0xFFu;

							if (mask.constpixel<uint8_t>(x, y)[0] == 0x00u)
							{
								unsigned int counter = 0u;

								const CV::PixelPosition& centerPosition = mapping.position(x, y);

								for (const CV::PixelPositionI& offset : offsets)
								{
									const int xPosition = int(x) + offset.x();
									const int yPosition = int(y) + offset.y();

									bool validCandidate = false;

									if (xPosition >= 0 && xPosition < int(mask.width()) && yPosition >= 0 && yPosition < int(mask.height()))
									{
										if (mask.constpixel<uint8_t>((unsigned int)(xPosition), (unsigned int)(yPosition))[0] == 0x00u)
										{
											validCandidate = true;

											const CV::PixelPosition neighborPosition((unsigned int)(int(centerPosition.x()) + offset.x()), (unsigned int)(int(centerPosition.y() + offset.y())));

											if (neighborPosition.x() < mask.width() && neighborPosition.y() < mask.height())
											{
												if (mapping.position((unsigned int)(xPosition), (unsigned int)(yPosition)) == neighborPosition)
												{
													++counter;
												}
											}
										}
									}

									if (!validCandidate && !onlyCenterPixels)
									{
										++counter;
									}
								}

								if (counter >= neighborhood)
								{
									value = 0x80u;
								}
								else
								{
									value = 0x00u;
								}
							}

							const uint8_t result = frame.constpixel<uint8_t>(x, y)[0];

							if (result != value)
							{
								allSucceeded = false;
							}
						}
					}
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
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

bool TestCreatorI1::testInformationCost4Neighborhood(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 4-neighborhood cost information for " << width << "x" << height << ":";

	bool allSucceeded = true;

	Log::info() << " ";

	allSucceeded = testInformationCost4Neighborhood<1u>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testInformationCost4Neighborhood<2u>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testInformationCost4Neighborhood<3u>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testInformationCost4Neighborhood<4u>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "4-neighborhood cost information test succeeded.";
	}
	else
	{
		Log::info() << "4-neighborhood cost information test FAILED!";
	}

	return allSucceeded;
}

template <unsigned int tChannels>
bool TestCreatorI1::testInformationCost4Neighborhood(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << tChannels << " channels:";

	bool allSucceeded = true;

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
			for (const bool performanceIteration : {true, false})
			{
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 5u, width);
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 5u, height);

				Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

				const Frame copyFrame(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

				CV::PixelBoundingBox boundingBox;
				if (RandomI::random(randomGenerator, 1u) == 0u)
				{
					boundingBox = CV::MaskAnalyzer::detectBoundingBox(mask.constdata<uint8_t>(), mask.width(), mask.height(), 0xFFu, mask.paddingElements());
					ocean_assert(boundingBox.isValid());
				}

				constexpr unsigned int patchSize = 5u;

				CV::Segmentation::MaskAnalyzer::determineDistancesToBorder8Bit(mask.data<uint8_t>(), mask.width(), mask.height(), mask.paddingElements(), patchSize + 1u, false, CV::PixelBoundingBox(), useWorker);

				CV::Synthesis::LayerI1 layer(frame, mask, boundingBox);

				// we create a random mapping

				CV::Synthesis::MappingI1& mapping = layer.mappingI1();

				for (unsigned int y = 0u; y < frame.height(); ++y)
				{
					const uint8_t* rowMask = mask.constrow<uint8_t>(y);
					CV::PixelPosition* rowMapping = mapping() + y * frame.width();

					for (unsigned int x = 0u; x < frame.width(); ++x)
					{
						if (rowMask[x] != 0xFFu)
						{
							while (true)
							{
								const CV::PixelPosition sourcePixel(RandomI::random(randomGenerator, 2u, frame.width() - 3u), RandomI::random(randomGenerator, 2u, frame.height() - 3u));

								if (mask.constpixel<uint8_t>(sourcePixel.x(), sourcePixel.y())[0] == 0xFFu)
								{
									rowMapping[x] = sourcePixel;
									break;
								}
							}
						}
					}
				}

				// let's create some realistic mappings with neighboring mappings mapping to neighboring pixels

				for (unsigned int n = 0u; n < mask.pixels() / 10u; ++n)
				{
					const unsigned int x = RandomI::random(randomGenerator, mask.width() - 1u);
					const unsigned int y = RandomI::random(randomGenerator, mask.height() - 1u);

					if (mask.constpixel<uint8_t>(x, y)[0] != 0xFFu)
					{
						const CV::PixelPosition& centerMapping = mapping.position(x, y);

						for (int yy = -1; yy <= 1; ++yy)
						{
							const unsigned int yPosition = (unsigned int)(int(y) + yy);

							if (yPosition < mask.height())
							{
								for (int xx = -1; xx <= 1; ++xx)
								{
									const unsigned int xPosition = (unsigned int)(int(x) + xx);

									if (xPosition < mask.width())
									{
										if (xx != 0 || yy != 0)
										{
											if (mask.constpixel<uint8_t>(xPosition, yPosition)[0] != 0xFFu)
											{
												const unsigned int xNeighbor = (unsigned int)(int(centerMapping.x()) + xx);
												const unsigned int yNeighbor = (unsigned int)(int(centerMapping.y()) + yy);

												if (xNeighbor < mask.width() && yNeighbor < mask.height())
												{
													if (mask.constpixel<uint8_t>(xNeighbor, yNeighbor)[0] == 0xFFu)
													{
														mapping.position(xPosition, yPosition) = CV::PixelPosition(xNeighbor, yNeighbor);
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}

				constexpr unsigned int tWeightFactor = 5u;
				constexpr unsigned int tBorderFactor = 25u;
				constexpr unsigned int maxSpatialCost = (unsigned int)(-1);

				uint64_t cost = 0ull;

				performance.startIf(performanceIteration);
					CV::Synthesis::CreatorInformationCost4NeighborhoodI1<tWeightFactor, tBorderFactor>(layer, cost, maxSpatialCost).invoke(useWorker);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, copyFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				uint64_t testCost = 0ull;

				for (unsigned int y = 0u; y < frame.height(); ++y)
				{
					for (unsigned int x = 0u; x < frame.width(); ++x)
					{
						if (mask.constpixel<uint8_t>(x, y)[0] != 0xFFu)
						{
							const CV::PixelPosition& source = mapping.position(x, y);

							const unsigned int spatialCost = mapping.spatialCost4Neighborhood<tChannels>(x, y, source.x(), source.y(), mask.constdata<uint8_t>(), mask.paddingElements(), maxSpatialCost);
							const unsigned int appearanceCost = mapping.appearanceCost5x5<tChannels, tBorderFactor>(x, y, source.x(), source.y(), frame.constdata<uint8_t>(), mask.constdata<uint8_t>(), frame.paddingElements(), mask.paddingElements());

							testCost += uint64_t(spatialCost) * uint64_t(tWeightFactor) + appearanceCost;
						}
					}
				}

				if (cost != testCost)
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

}

}

}

}
