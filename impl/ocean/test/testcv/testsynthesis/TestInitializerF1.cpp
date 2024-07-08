/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testsynthesis/TestInitializerF1.h"
#include "ocean/test/testcv/testsynthesis/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/MaskAnalyzer.h"

#include "ocean/cv/advanced/AdvancedSumSquareDifferences.h"

#include "ocean/cv/segmentation/MaskAnalyzer.h"

#include "ocean/cv/synthesis/InitializerAppearanceMappingF1.h"
#include "ocean/cv/synthesis/InitializerCoarserMappingAdaptionF1.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestSynthesis
{

bool TestInitializerF1::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "InitializerF1 test:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testAppearanceMapping(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCoarserMappingAdaption(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "InitializerF1 test succeeded.";
	}
	else
	{
		Log::info() << "InitializerF1 test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestInitializerF1, AppearanceMapping_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerF1::testAppearanceMapping(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerF1, AppearanceMapping_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerF1::testAppearanceMapping(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerF1, AppearanceMapping_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerF1::testAppearanceMapping(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerF1, AppearanceMapping_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerF1::testAppearanceMapping(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 4u, GTEST_TEST_DURATION, worker));
}


TEST(TestInitializerF1, CoarserMappingAdaption_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerF1::testCoarserMappingAdaption(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerF1, CoarserMappingAdaption_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerF1::testCoarserMappingAdaption(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerF1, CoarserMappingAdaption_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerF1::testCoarserMappingAdaption(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerF1, CoarserMappingAdaption_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerF1::testCoarserMappingAdaption(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 4u, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestInitializerF1::testAppearanceMapping(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing appearance mapping for " << width << "x" << height << ":";

	bool allSucceeded = true;

	for (const unsigned int channels : {1u, 2u, 3u, 4u})
	{
		Log::info() << " ";

		if (!testAppearanceMapping(width, height, channels, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Appearance mapping test succeeded.";
	}
	else
	{
		Log::info() << "Appearance mapping test FAILED!";
	}

	return allSucceeded;
}

bool TestInitializerF1::testAppearanceMapping(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
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
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 100u, width);
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 100u, height);

				Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

				const Frame copyFrame(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

				CV::PixelBoundingBox boundingBox;
				if (RandomI::random(randomGenerator, 1u) == 0u)
				{
					boundingBox = CV::MaskAnalyzer::detectBoundingBox(mask.constdata<uint8_t>(), mask.width(), mask.height(), 0xFFu, mask.paddingElements());
					ocean_assert(boundingBox.isValid());
				}

				CV::Segmentation::MaskAnalyzer::determineDistancesToBorder8Bit(mask.data<uint8_t>(), mask.width(), mask.height(), mask.paddingElements(), 4u, false /*assignFinal*/, CV::PixelBoundingBox());

				CV::Synthesis::LayerF1 layer(frame, mask, boundingBox);

				CV::Synthesis::MappingF1& mapping = layer.mappingF1();

				constexpr unsigned int patchSize = 1u;
				constexpr unsigned int iterations = 100u;

				const unsigned int randomSeed = randomGenerator.seed();

				performance.startIf(performanceIteration);
					CV::Synthesis::InitializerAppearanceMappingF1<patchSize, iterations>(layer, randomGenerator).invoke(useWorker);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, copyFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				constexpr unsigned int borderSize = 1u; // extra border for sub-pixel interpolation

				if (useWorker == nullptr)
				{
					const unsigned int patchSize_2 = patchSize / 2u;

					RandomGenerator helperGenerator(randomSeed);
					RandomGenerator localGenerator(helperGenerator);

					for (unsigned int y = 0u; y < frame.height() - borderSize; ++y)
					{
						for (unsigned int x = 0u; x < frame.width() - borderSize; ++x)
						{
							if (mask.constpixel<uint8_t>(x, y)[0] != 0xFFu)
							{
								Vector2 bestPosition(Numeric::minValue(), Numeric::minValue());
								unsigned int bestCost = (unsigned int)(-1);

								while (true)
								{
									const Scalar xCandidate = Random::scalar(localGenerator, Scalar(patchSize_2), Scalar(testWidth - patchSize_2 - 1u) - Numeric::weakEps());
									const Scalar yCandidate = Random::scalar(localGenerator, Scalar(patchSize_2), Scalar(testHeight - patchSize_2 - 1u) - Numeric::weakEps());

									if (mask.constpixel<uint8_t>((unsigned int)(Numeric::round32(xCandidate)), (unsigned int)(Numeric::round32(yCandidate)))[0] == 0xFFu)
									{
										bestPosition = Vector2(xCandidate, yCandidate);

										bestCost = CV::Advanced::AdvancedSumSquareDifferencesBase::patch8BitPerChannel(frame.constdata<uint8_t>(), frame.constdata<uint8_t>(), frame.channels(), patchSize, frame.width(), frame.width(), Scalar(x), Scalar(y), xCandidate, yCandidate, frame.paddingElements(), frame.paddingElements())
														+ (unsigned int)(sqrDistance(Scalar(x), xCandidate) + sqrDistance(Scalar(y), yCandidate)) / 2u;
										break;
									}
								}

								for (unsigned int n = 1u; n < iterations; ++n)
								{
									const Scalar xCandidate = Random::scalar(localGenerator, Scalar(patchSize_2), Scalar(testWidth - patchSize_2 - 1u) - Numeric::weakEps());
									const Scalar yCandidate = Random::scalar(localGenerator, Scalar(patchSize_2), Scalar(testHeight - patchSize_2 - 1u) - Numeric::weakEps());

									if (mask.constpixel<uint8_t>((unsigned int)(Numeric::round32(xCandidate)), (unsigned int)(Numeric::round32(yCandidate)))[0] == 0xFFu)
									{
										const unsigned int candidateCost = CV::Advanced::AdvancedSumSquareDifferencesBase::patch8BitPerChannel(frame.constdata<uint8_t>(), frame.constdata<uint8_t>(), frame.channels(), patchSize, frame.width(), frame.width(), Scalar(x), Scalar(y), xCandidate, yCandidate, frame.paddingElements(), frame.paddingElements())
																				+ (unsigned int)(sqrDistance(Scalar(x), xCandidate) + sqrDistance(Scalar(y), yCandidate)) / 2u;

										if (candidateCost < bestCost)
										{
											bestPosition = Vector2(xCandidate, yCandidate);
											bestCost = candidateCost;
										}
									}
								}

								if (mapping.position(x, y) != bestPosition)
								{
									allSucceeded = false;
								}
							}
						}
					}
				}
				else
				{
					// due to multi-core execution the result will not be deterministic - thus, ensuring that we have a valid mapping only

					for (unsigned int y = 0u; y < frame.height() - borderSize; ++y)
					{
						for (unsigned int x = 0u; x < frame.width() - borderSize; ++x)
						{
							if (mask.constpixel<uint8_t>(x, y)[0] != 0xFFu)
							{
								const Vector2& position = mapping.position(x, y);

								const int xPosition = Numeric::round32(position.x());
								const int yPosition = Numeric::round32(position.y());

								if (xPosition < 0 || xPosition >= int(mask.width()) || yPosition < 0 || yPosition >= int(mask.height()))
								{
									allSucceeded = false;
								}
								else
								{
									if (mask.constpixel<uint8_t>((unsigned int)(xPosition), (unsigned int)(yPosition))[0] != 0xFFu)
									{
										allSucceeded = false;
									}
								}
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

bool TestInitializerF1::testCoarserMappingAdaption(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing coarser mapping adaption mapping for " << width << "x" << height << ":";

	bool allSucceeded = true;

	for (const unsigned int channels : {1u, 2u, 3u, 4u})
	{
		Log::info() << " ";

		if (!testCoarserMappingAdaption(width, height, channels, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Coarser mapping adaption test succeeded.";
	}
	else
	{
		Log::info() << "Coarser mapping adaption test FAILED!";
	}

	return allSucceeded;
}

bool TestInitializerF1::testCoarserMappingAdaption(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
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
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 50u, width / 2u) * 2u;
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 50u, height / 2u) * 2u;

				Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

				const Frame copyFrame(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00, randomGenerator);

				CV::PixelBoundingBox boundingBox;
				if (RandomI::random(randomGenerator, 1u) == 0u)
				{
					boundingBox = CV::MaskAnalyzer::detectBoundingBox(mask.constdata<uint8_t>(), mask.width(), mask.height(), 0xFFu, mask.paddingElements());
					ocean_assert(boundingBox.isValid());
				}

				constexpr unsigned int factor = 2u;

				ocean_assert(testWidth % factor == 0u);
				ocean_assert(testHeight % factor == 0u);
				const unsigned int coarserTestWidth = testWidth / factor;
				const unsigned int coarserTestHeight = testHeight / factor;

				Frame coarserFrame = CV::CVUtilities::randomizedFrame(FrameType(frame, coarserTestWidth, coarserTestHeight), &randomGenerator);

				const unsigned int coarserMaskPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
					;

				// creating a coarser mask which has a mask pixel whenever the corresponding finder mask has a mask pixel

				Frame coarserMask(FrameType(coarserFrame, FrameType::FORMAT_Y8), coarserMaskPaddingElements);
				coarserMask.setValue(0xFFu);

				for (unsigned int yCoarser = 0u; yCoarser < coarserMask.height(); ++yCoarser)
				{
					const unsigned int yFinerStart = yCoarser * factor;
					const unsigned int yFinerEnd = std::min((yCoarser + 1u) * factor, mask.height()); // exclusive

					for (unsigned int xCoarser = 0u; xCoarser < coarserMask.width(); ++xCoarser)
					{
						const unsigned int xFinerStart = xCoarser * factor;
						const unsigned int xFinerEnd = std::min((xCoarser + 1u) * factor, mask.width());

						bool finerHasMaskPixel = false;

						for (unsigned int yFinder = yFinerStart; !finerHasMaskPixel && yFinder < yFinerEnd; ++yFinder)
						{
							for (unsigned int xFinder = xFinerStart; !finerHasMaskPixel && xFinder < xFinerEnd; ++xFinder)
							{
								if (mask.constpixel<uint8_t>(xFinder, yFinder)[0] != 0xFFu)
								{
									finerHasMaskPixel = true;
								}
							}
						}

						if (finerHasMaskPixel)
						{
							coarserMask.pixel<uint8_t>(xCoarser, yCoarser)[0] = 0x00u;
						}
					}
				}

				CV::Synthesis::LayerF1 layer(frame, mask, boundingBox);
				CV::Synthesis::MappingF1& mapping = layer.mappingF1();

				CV::Synthesis::LayerF1 coarserLayer(coarserFrame, coarserMask);
				CV::Synthesis::MappingF1& coarserMapping = coarserLayer.mappingF1();

				for (unsigned int y = 0u; y < coarserMask.height(); ++y)
				{
					for (unsigned int x = 0u; x < coarserMask.width(); ++x)
					{
						if (coarserMask.constpixel<uint8_t>(x, y)[0] == 0x00u)
						{
							Vector2 source;

							while (true)
							{
								source = Random::vector2(randomGenerator, Scalar(0), Scalar(coarserMask.width() - 1u), Scalar(0), Scalar(coarserMask.height() - 1u));

								const int xSource = Numeric::round32(source.x());
								const int ySource = Numeric::round32(source.y());
								ocean_assert(xSource >= 0 && ySource >= 0);

								if (coarserMask.constpixel<uint8_t>((unsigned int)(xSource), (unsigned int)(ySource))[0] == 0xFFu)
								{
									break;
								}
							}

							coarserMapping.setPosition(x, y, source);
						}
					}
				}

				CV::Segmentation::MaskAnalyzer::determineDistancesToBorder8Bit(mask.data<uint8_t>(), mask.width(), mask.height(), mask.paddingElements(), 4u, false /*assignFinal*/, CV::PixelBoundingBox());
				CV::Segmentation::MaskAnalyzer::determineDistancesToBorder8Bit(coarserMask.data<uint8_t>(), coarserMask.width(), coarserMask.height(), coarserMask.paddingElements(), 4u, false /*assignFinal*/, CV::PixelBoundingBox());

				const unsigned int randomSeed = randomGenerator.seed();

				performance.startIf(performanceIteration);
					if (!CV::Synthesis::InitializerCoarserMappingAdaptionF1<factor>(layer, randomGenerator, coarserLayer).invoke(useWorker))
					{
						allSucceeded = false;
					}
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, copyFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (useWorker == nullptr)
				{
					RandomGenerator helperGenerator(randomSeed);
					RandomGenerator localGenerator(helperGenerator);

					for (unsigned int y = 0u; y < frame.height(); ++y)
					{
						for (unsigned int x = 0u; x < frame.width(); ++x)
						{
							if (mask.constpixel<uint8_t>(x, y)[0] != 0xFFu)
							{
								const unsigned int xCoarser = std::min(x / 2u, coarserMask.width() - 1u);
								const unsigned int yCoarser = std::min(y / 2u, coarserMask.height() - 1u);

								const Vector2& coarserPosition = coarserMapping.position(xCoarser, yCoarser);

								const Vector2 coarserOffset = coarserPosition - Vector2(Scalar(xCoarser), Scalar(yCoarser));

								const Vector2 finerPosition = Vector2(Scalar(x), Scalar(y)) + coarserOffset * Scalar(factor);

								const int xFinerPosition = Numeric::round32(finerPosition.x());
								const int yFinerPosition = Numeric::round32(finerPosition.y());

								if (xFinerPosition < 0 || xFinerPosition >= int(mask.width()) || yFinerPosition < 0 || yFinerPosition >= int(mask.height()))
								{
									ocean_assert(false && "This should never happen!");
									allSucceeded = false;
								}
								else
								{
									const Vector2& position = mapping.position(x, y);

									if (mask.constpixel<uint8_t>((unsigned int)(xFinerPosition), (unsigned int)(yFinerPosition))[0] == 0xFFu)
									{
										if (position != finerPosition)
										{
											allSucceeded = false;
										}
									}
									else
									{
										while (true)
										{
											constexpr unsigned int border = 2u;

											const Scalar xFiner = Random::scalar(localGenerator, Scalar(border), Scalar(mask.width() - border - 1u));
											const Scalar yFiner = Random::scalar(localGenerator, Scalar(border), Scalar(mask.height() - border - 1u));

											if (mask.constpixel<uint8_t>((unsigned int)(Numeric::round32(xFiner)), (unsigned int)(Numeric::round32(yFiner)))[0] == 0xFFu)
											{
												if (position != Vector2(xFiner, yFiner))
												{
													allSucceeded = false;
												}

												break;
											}
										}
									}
								}
							}
						}
					}
				}
				else
				{
					// due to multi-core execution the result will not be deterministic - thus, ensuring that we have a valid mapping only

					for (unsigned int y = 0u; y < frame.height(); ++y)
					{
						for (unsigned int x = 0u; x < frame.width(); ++x)
						{
							if (mask.constpixel<uint8_t>(x, y)[0] != 0xFFu)
							{
								const Vector2& position = mapping.position(x, y);

								const int xPosition = Numeric::round32(position.x());
								const int yPosition = Numeric::round32(position.y());

								if (xPosition < 0 || xPosition >= int(mask.width()) || yPosition < 0 || yPosition >= int(mask.height()))
								{
									allSucceeded = false;
								}
								else
								{
									if (mask.constpixel<uint8_t>((unsigned int)(xPosition), (unsigned int)(yPosition))[0] != 0xFFu)
									{
										allSucceeded = false;
									}
								}
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

}

}

}

}
