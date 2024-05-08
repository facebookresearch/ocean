/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testsynthesis/TestCreatorF1.h"
#include "ocean/test/testcv/testsynthesis/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/MaskAnalyzer.h"

#include "ocean/cv/segmentation/MaskAnalyzer.h"

#include "ocean/cv/synthesis/CreatorInpaintingContentF1.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestSynthesis
{

bool TestCreatorF1::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "CreatorF1 test:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testInpaintingContent(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "CreatorF1 test succeeded.";
	}
	else
	{
		Log::info() << "CreatorF1 test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestCreatorF1, InpaintingContent_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestCreatorF1::testInpaintingContent<1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker));
}

TEST(TestCreatorF1, InpaintingContent_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestCreatorF1::testInpaintingContent<2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker));
}

TEST(TestCreatorF1, InpaintingContent_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestCreatorF1::testInpaintingContent<3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker));
}

TEST(TestCreatorF1, InpaintingContent_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestCreatorF1::testInpaintingContent<4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestCreatorF1::testInpaintingContent(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing inpainting content for " << width << "x" << height << ":";

	bool allSucceeded = true;

	Log::info() << " ";

	allSucceeded = testInpaintingContent<1u>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testInpaintingContent<2u>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testInpaintingContent<3u>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testInpaintingContent<4u>(width, height, testDuration, worker) && allSucceeded;

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

template <unsigned int tChannels>
bool TestCreatorF1::testInpaintingContent(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
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
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 3u, width);
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 3u, height);

				Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

				const Frame copyFrame(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				const Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

				CV::PixelBoundingBox boundingBox;
				if (RandomI::random(randomGenerator, 1u) == 0u)
				{
					boundingBox = CV::MaskAnalyzer::detectBoundingBox(mask.constdata<uint8_t>(), mask.width(), mask.height(), 0xFFu, mask.paddingElements());
					ocean_assert(boundingBox.isValid());
				}

				CV::Synthesis::LayerF1 layer(frame, mask, boundingBox);

				// we create a random mapping

				CV::Synthesis::MappingF1& mapping = layer.mappingF1();

				for (unsigned int y = 0u; y < frame.height(); ++y)
				{
					const uint8_t* rowMask = mask.constrow<uint8_t>(y);
					Vector2* rowMapping = mapping() + y * frame.width();

					for (unsigned int x = 0u; x < frame.width(); ++x)
					{
						if (rowMask[x] == 0x00u)
						{
							// now, we seek a random source location with valid pixels

							while (true)
							{
								const Vector2 sourceLocation = Random::vector2(randomGenerator, Scalar(0), Scalar(frame.width() - 1u) - Numeric::eps(), Scalar(0), Scalar(frame.height() - 1u) - Numeric::eps());

								const int xSourcePixel = Numeric::round32(sourceLocation.x());
								const int ySourcePixel = Numeric::round32(sourceLocation.y());

								bool validSourceLocation = true;

								for (int yy = -1; validSourceLocation && yy <= 1; ++yy)
								{
									for (int xx = -1; validSourceLocation && xx <= 1; ++xx)
									{
										const unsigned int xPosition = (unsigned int)(xSourcePixel + xx);
										const unsigned int yPosition = (unsigned int)(ySourcePixel + yy);

										if (xPosition < mask.width() && yPosition < mask.height())
										{
											if (mask.constpixel<uint8_t>(xPosition, yPosition)[0] != 0xFFu)
											{
												validSourceLocation = false;
											}
										}
									}
								}

								if (validSourceLocation)
								{
									rowMapping[x] = sourceLocation;
									break;
								}
							}
						}
					}
				}

				CV::Synthesis::CreatorInpaintingContentF1 creator(layer, frame);

				performance.startIf(performanceIteration);
					creator.invoke(useWorker);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, copyFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				uint8_t result[tChannels];

				for (unsigned int y = 0u; y < frame.height(); ++y)
				{
					const Vector2* rowMapping = mapping() + y * frame.width();

					for (unsigned int x = 0u; x < frame.width(); ++x)
					{
						if (mask.constpixel<uint8_t>(x, y)[0] != 0xFFu)
						{
							const Vector2& sourcePixel = rowMapping[x];

							CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<tChannels, CV::PC_TOP_LEFT>(copyFrame.constdata<uint8_t>(), copyFrame.width(), copyFrame.height(), copyFrame.paddingElements(), sourcePixel, result);

							if (memcmp(frame.constpixel<uint8_t>(x, y), result, sizeof(uint8_t) * tChannels) != 0)
							{
								allSucceeded = false;
							}
						}
						else
						{
							// we do not expect any change

							if (memcmp(frame.constpixel<uint8_t>(x, y), copyFrame.constpixel<uint8_t>(x, y), sizeof(uint8_t) * tChannels) != 0)
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

}

}

}

}
