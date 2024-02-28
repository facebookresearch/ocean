// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testcv/testsynthesis/TestInitializerF1.h"
#include "ocean/test/testcv/testsynthesis/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/CVUtilities.h"

#include "ocean/cv/advanced/AdvancedSumSquareDifferences.h"

#include "ocean/cv/segmentation/MaskAnalyzer.h"

#include "ocean/cv/synthesis/InitializerAppearanceMappingF1.h"

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

				Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), false, &randomGenerator);

				const Frame copyFrame(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				Frame mask = Utilities::randomizedInpaintingMaskWithoutPadding(testWidth, testHeight, 0x00, randomGenerator);

				CV::Segmentation::MaskAnalyzer::determineDistancesToBorder8Bit(mask.data<uint8_t>(), mask.width(), mask.height(), mask.paddingElements(), 4u, false /*assignFinal*/, CV::PixelBoundingBox());

				const LegacyFrame legacyMask(mask, LegacyFrame::FCM_USE_IF_POSSIBLE);

				CV::Synthesis::LayerF1 layer(frame, legacyMask);

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

}

}

}

}
