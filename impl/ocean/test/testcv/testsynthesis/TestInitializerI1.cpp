/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testsynthesis/TestInitializerI1.h"
#include "ocean/test/testcv/testsynthesis/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterErosion.h"
#include "ocean/cv/MaskAnalyzer.h"
#include "ocean/cv/SumSquareDifferences.h"

#include "ocean/cv/segmentation/MaskAnalyzer.h"

#include "ocean/cv/synthesis/InitializerAppearanceMappingAreaConstrainedI1.h"
#include "ocean/cv/synthesis/InitializerAppearanceMappingI1.h"
#include "ocean/cv/synthesis/InitializerCoarserMappingAdaptionI1.h"
#include "ocean/cv/synthesis/InitializerCoarserMappingAdaptionAreaConstrainedI1.h"
#include "ocean/cv/synthesis/InitializerCoarserMappingAdaptionSpatialCostMaskI1.h"
#include "ocean/cv/synthesis/InitializerRandomMappingAreaConstrainedI1.h"
#include "ocean/cv/synthesis/InitializerRandomMappingI1.h"
#include "ocean/cv/synthesis/InitializerShrinkingErosionI1.h"
#include "ocean/cv/synthesis/InitializerShrinkingErosionRandomizedI1.h"
#include "ocean/cv/synthesis/InitializerShrinkingPatchMatchingI1.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestSynthesis
{

TestInitializerI1::InpaintingPixel::InpaintingPixel(const CV::PixelPosition& pixelPosition, const Frame& mask, const Frame& sobel) :
	CV::PixelPosition(pixelPosition)
{
	ocean_assert(mask.isValid() && sobel.isValid());
	ocean_assert(mask.width() == sobel.width() && mask.height() == sobel.height());

	borderDirection_ = determineBorderDirection(mask);
	imageOrientation_ = determineImageOrientation(mask, sobel);

	priority_ = abs(imageOrientation_.perpendicular() * borderDirection_);
}

const VectorI2& TestInitializerI1::InpaintingPixel::borderDirection() const
{
	return borderDirection_;
}

bool TestInitializerI1::InpaintingPixel::operator<(const InpaintingPixel& inpaintingPixel) const
{
	if (priority_ <  inpaintingPixel.priority_)
	{
		return true;
	}

	if (priority_ == inpaintingPixel.priority_)
	{
		return y() * 1000u + x() < inpaintingPixel.y() * 1000u + inpaintingPixel.x();
	}

	return false;
}

VectorI2 TestInitializerI1::InpaintingPixel::determineBorderDirection(const Frame& mask) const
{
	ocean_assert(mask.isValid() && mask.isPixelFormatCompatible(FrameType::FORMAT_Y8));

	VectorI2 borderDirection = VectorI2(0, 0);

	for (int yOffsetOuter = -2; yOffsetOuter <= 2; ++yOffsetOuter)
	{
		const int yCenter = int(this->y()) + yOffsetOuter;

		if (yCenter < 0 || yCenter >= int(mask.height()))
		{
			continue;
		}

		for (int xOffsetOuter = -2; xOffsetOuter <= 2; ++xOffsetOuter)
		{
			const int xCenter = (unsigned int)(int(this->x()) + xOffsetOuter);

			if (xCenter < 0 || xCenter >= int(mask.width()))
			{
				continue;
			}

			if ((yOffsetOuter != 0 || xOffsetOuter != 0) && xCenter < int(mask.width()) && yCenter < int(mask.height()))
			{
				bool foundMaskPixel = false;

				for (int yOffsetInner = -1; !foundMaskPixel && yOffsetInner <= 1; ++yOffsetInner) // 9 neighborhood
				{
					const int yTest = yCenter + yOffsetInner;

					if (yTest < 0 || yTest >= int(mask.height()))
					{
						continue;
					}

					for (int xOffsetInner = -1; !foundMaskPixel && xOffsetInner <= 1; ++xOffsetInner)
					{
						const int xTest = xCenter + xOffsetInner;

						if (xTest < 0 || xTest >= int(mask.width()))
						{
							continue;
						}

						if (mask.constpixel<uint8_t>((unsigned int)(xTest), (unsigned int)(yTest))[0] != 0xFFu)
						{
							foundMaskPixel = true;
						}
					}
				}

				if (!foundMaskPixel) // all neighbors are valid pixels
				{
					borderDirection += VectorI2(xOffsetOuter, yOffsetOuter);
				}
			}
		}
	}

	return borderDirection;
}

VectorI2 TestInitializerI1::InpaintingPixel::determineImageOrientation(const Frame& mask, const Frame& sobel) const
{
	ocean_assert(mask.isValid() && mask.isPixelFormatCompatible(FrameType::FORMAT_Y8));

	ocean_assert(sobel.dataType() == FrameType::DT_SIGNED_INTEGER_16);
	ocean_assert(sobel.width() == mask.width() && sobel.height() == mask.height());

	VectorI2 imageOrientation = VectorI2(0, 0);

	for (int yOffsetOuter = -2; yOffsetOuter <= 2; ++yOffsetOuter)
	{
		const int yCenter = int(this->y()) + yOffsetOuter;

		if (yCenter < 0 || yCenter >= int(mask.height()))
		{
			continue;
		}

		for (int xOffsetOuter = -2; xOffsetOuter <= 2; ++xOffsetOuter)
		{
			const int xCenter = (unsigned int)(int(this->x()) + xOffsetOuter);

			if (xCenter < 0 || xCenter >= int(mask.width()))
			{
				continue;
			}

			if ((yOffsetOuter != 0 || xOffsetOuter != 0) && xCenter < int(mask.width()) && yCenter < int(mask.height()))
			{
				bool foundMaskPixel = false;

				for (int yOffsetInner = -1; !foundMaskPixel && yOffsetInner <= 1; ++yOffsetInner) // 9 neighborhood
				{
					const int yTest = yCenter + yOffsetInner;

					if (yTest < 0 || yTest >= int(mask.height()))
					{
						continue;
					}

					for (int xOffsetInner = -1; !foundMaskPixel && xOffsetInner <= 1; ++xOffsetInner)
					{
						const int xTest = xCenter + xOffsetInner;

						if (xTest < 0 || xTest >= int(mask.width()))
						{
							continue;
						}

						if (mask.constpixel<uint8_t>((unsigned int)(xTest), (unsigned int)(yTest))[0] != 0xFFu)
						{
							foundMaskPixel = true;
						}
					}
				}

				if (!foundMaskPixel) // all neighbors are valid pixels
				{
					const int16_t* sobelPixel = sobel.constpixel<int16_t>((unsigned int)(xCenter), (unsigned int)(yCenter));

					for (unsigned int n = 0u; n < sobel.channels() / 2u; ++n)
					{
						const int16_t xSobel = sobelPixel[2u * n + 0u];
						const int16_t ySobel = sobelPixel[2u * n + 1u];

						if (xSobel >= 0)
						{
							imageOrientation += VectorI2(xSobel, ySobel);
						}
						else
						{
							imageOrientation -= VectorI2(xSobel, ySobel);
						}
					}
				}
			}
		}
	}

	return imageOrientation;
}

bool TestInitializerI1::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "InitializerI1 test:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testAppearanceMappingAreaConstrained(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAppearanceMapping(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCoarserMappingAdaption(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCoarserMappingAdaptionAreaConstrained(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCoarserMappingAdaptionSpatialCostMask(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRandomMapping(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRandomMappingAreaConstrained(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testShrinkingErosion(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testShrinkingErosionRandomized(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testShrinkingPatchMatching(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "InitializerI1 test succeeded.";
	}
	else
	{
		Log::info() << "InitializerI1 test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestInitializerI1, AppearanceMappingAreaConstrained_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testAppearanceMappingAreaConstrained(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerI1, AppearanceMappingAreaConstrained_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testAppearanceMappingAreaConstrained(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerI1, AppearanceMappingAreaConstrained_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testAppearanceMappingAreaConstrained(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerI1, AppearanceMappingAreaConstrained_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testAppearanceMappingAreaConstrained(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 4u, GTEST_TEST_DURATION, worker));
}


TEST(TestInitializerI1, AppearanceMapping_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testAppearanceMapping(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerI1, AppearanceMapping_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testAppearanceMapping(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerI1, AppearanceMapping_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testAppearanceMapping(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerI1, AppearanceMapping_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testAppearanceMapping(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 4u, GTEST_TEST_DURATION, worker));
}


TEST(TestInitializerI1, CoarserMappingAdaption_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testCoarserMappingAdaption(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerI1, CoarserMappingAdaption_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testCoarserMappingAdaption(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerI1, CoarserMappingAdaption_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testCoarserMappingAdaption(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerI1, CoarserMappingAdaption_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testCoarserMappingAdaption(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 4u, GTEST_TEST_DURATION, worker));
}


TEST(TestInitializerI1, CoarserMappingAdaptionAreaConstrained_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testCoarserMappingAdaptionAreaConstrained(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerI1, CoarserMappingAdaptionAreaConstrained_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testCoarserMappingAdaptionAreaConstrained(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerI1, CoarserMappingAdaptionAreaConstrained_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testCoarserMappingAdaptionAreaConstrained(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerI1, CoarserMappingAdaptionAreaConstrained_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testCoarserMappingAdaptionAreaConstrained(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 4u, GTEST_TEST_DURATION, worker));
}


TEST(TestInitializerI1, CoarserMappingAdaptionSpatialCostMask_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testCoarserMappingAdaptionSpatialCostMask(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerI1, CoarserMappingAdaptionSpatialCostMask_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testCoarserMappingAdaptionSpatialCostMask(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerI1, CoarserMappingAdaptionSpatialCostMask_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testCoarserMappingAdaptionSpatialCostMask(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerI1, CoarserMappingAdaptionSpatialCostMask_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testCoarserMappingAdaptionSpatialCostMask(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 4u, GTEST_TEST_DURATION, worker));
}


TEST(TestInitializerI1, RandomMapping)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testRandomMapping(GTEST_TEST_DURATION, worker));
}


TEST(TestInitializerI1, RandomMappingAreaConstrained)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testRandomMappingAreaConstrained(GTEST_TEST_DURATION, worker));
}


TEST(TestInitializerI1, ShrinkingErosion_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testShrinkingErosion(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerI1, ShrinkingErosion_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testShrinkingErosion(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerI1, ShrinkingErosion_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testShrinkingErosion(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerI1, ShrinkingErosion_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testShrinkingErosion(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 4u, GTEST_TEST_DURATION, worker));
}


TEST(TestInitializerI1, ShrinkingErosionRandomized_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testShrinkingErosionRandomized(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerI1, ShrinkingErosionRandomized_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testShrinkingErosionRandomized(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerI1, ShrinkingErosionRandomized_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testShrinkingErosionRandomized(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerI1, ShrinkingErosionRandomized_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestInitializerI1::testShrinkingErosionRandomized(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 4u, GTEST_TEST_DURATION, worker));
}


TEST(TestInitializerI1, ShrinkingPatchMatching_1Channel)
{
	Worker worker;

	constexpr unsigned int width = 100u;
	constexpr unsigned int height = 100u;

	EXPECT_TRUE(TestInitializerI1::testShrinkingPatchMatching(width, height, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerI1, ShrinkingPatchMatching_2Channels)
{
	Worker worker;

	constexpr unsigned int width = 100u;
	constexpr unsigned int height = 100u;

	EXPECT_TRUE(TestInitializerI1::testShrinkingPatchMatching(width, height, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerI1, ShrinkingPatchMatching_3Channels)
{
	Worker worker;

	constexpr unsigned int width = 100u;
	constexpr unsigned int height = 100u;

	EXPECT_TRUE(TestInitializerI1::testShrinkingPatchMatching(width, height, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestInitializerI1, ShrinkingPatchMatching_4Channels)
{
	Worker worker;

	constexpr unsigned int width = 100u;
	constexpr unsigned int height = 100u;

	EXPECT_TRUE(TestInitializerI1::testShrinkingPatchMatching(width, height, 4u, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestInitializerI1::testAppearanceMappingAreaConstrained(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing constrained area appearance mapping for " << width << "x" << height << ":";

	bool allSucceeded = true;

	for (const unsigned int channels : {1u, 2u, 3u, 4u})
	{
		Log::info() << " ";

		if (!testAppearanceMappingAreaConstrained(width, height, channels, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Constrained area appearance mapping test succeeded.";
	}
	else
	{
		Log::info() << "Constrained area appearance mapping test FAILED!";
	}

	return allSucceeded;
}

bool TestInitializerI1::testAppearanceMappingAreaConstrained(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
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

				Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);;

				const Frame copyFrame(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				const Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

				CV::PixelBoundingBox boundingBox;
				if (RandomI::random(randomGenerator, 1u) == 0u)
				{
					boundingBox = CV::MaskAnalyzer::detectBoundingBox(mask.constdata<uint8_t>(), mask.width(), mask.height(), 0xFFu, mask.paddingElements());
					ocean_assert(boundingBox.isValid());
				}

				CV::Synthesis::LayerI1 layer(frame, mask, boundingBox);

				CV::Synthesis::MappingI1& mapping = layer.mappingI1();

				const Frame filterMask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

				constexpr unsigned int patchSize = 1u;
				constexpr unsigned int iterations = 100u;

				const unsigned int randomSeed = randomGenerator.seed();

				performance.startIf(performanceIteration);
					CV::Synthesis::InitializerAppearanceMappingAreaConstrainedI1<patchSize, iterations>(layer, randomGenerator, filterMask).invoke(useWorker);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, copyFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (useWorker == nullptr)
				{
					const unsigned int patchSize_2 = patchSize / 2u;

					RandomGenerator helperGenerator(randomSeed);
					RandomGenerator localGenerator(helperGenerator);

					for (unsigned int y = 0u; y < frame.height(); ++y)
					{
						for (unsigned int x = 0u; x < frame.width(); ++x)
						{
							if (mask.constpixel<uint8_t>(x, y)[0] != 0xFFu)
							{
								CV::PixelPosition bestPosition;
								unsigned int bestCost = (unsigned int)(-1);

								while (true)
								{
									const unsigned int xCandidate = RandomI::random(localGenerator, patchSize_2, testWidth - patchSize_2 - 1u);
									const unsigned int yCandidate = RandomI::random(localGenerator, patchSize_2, testHeight - patchSize_2 - 1u);

									if (mask.constpixel<uint8_t>(xCandidate, yCandidate)[0] == 0xFFu && filterMask.constpixel<uint8_t>(xCandidate, yCandidate)[0] == 0xFFu)
									{
										bestPosition = CV::PixelPosition(xCandidate, yCandidate);
										bestCost = CV::SumSquareDifferencesBase::patch8BitPerChannel(frame.constdata<uint8_t>(), frame.constdata<uint8_t>(), frame.channels(), patchSize, frame.width(), frame.width(), x, y, xCandidate, yCandidate, frame.paddingElements(), frame.paddingElements());
										break;
									}
								}

								for (unsigned int n = 1u; n < iterations; ++n)
								{
									const unsigned int xCandidate = RandomI::random(localGenerator, patchSize_2, testWidth - patchSize_2 - 1u);
									const unsigned int yCandidate = RandomI::random(localGenerator, patchSize_2, testHeight - patchSize_2 - 1u);

									if (mask.constpixel<uint8_t>(xCandidate, yCandidate)[0] == 0xFFu && filterMask.constpixel<uint8_t>(xCandidate, yCandidate)[0] == 0xFFu)
									{
										const unsigned int candidateCost = CV::SumSquareDifferencesBase::patch8BitPerChannel(frame.constdata<uint8_t>(), frame.constdata<uint8_t>(), frame.channels(), patchSize, frame.width(), frame.width(), x, y, xCandidate, yCandidate, frame.paddingElements(), frame.paddingElements());;

										if (candidateCost < bestCost)
										{
											bestPosition = CV::PixelPosition(xCandidate, yCandidate);
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

					for (unsigned int y = 0u; y < frame.height(); ++y)
					{
						for (unsigned int x = 0u; x < frame.width(); ++x)
						{
							if (mask.constpixel<uint8_t>(x, y)[0] != 0xFFu)
							{
								const CV::PixelPosition& position = mapping.position(x, y);

								if (mask.constpixel<uint8_t>(position.x(), position.y())[0] != 0xFFu || filterMask.constpixel<uint8_t>(position.x(), position.y())[0] != 0xFFu)
								{
									allSucceeded = false;
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

bool TestInitializerI1::testAppearanceMapping(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
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

bool TestInitializerI1::testAppearanceMapping(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
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

				const Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

				CV::PixelBoundingBox boundingBox;
				if (RandomI::random(randomGenerator, 1u) == 0u)
				{
					boundingBox = CV::MaskAnalyzer::detectBoundingBox(mask.constdata<uint8_t>(), mask.width(), mask.height(), 0xFFu, mask.paddingElements());
					ocean_assert(boundingBox.isValid());
				}

				CV::Synthesis::LayerI1 layer(frame, mask, boundingBox);

				CV::Synthesis::MappingI1& mapping = layer.mappingI1();

				constexpr unsigned int patchSize = 1u;
				constexpr unsigned int iterations = 100u;

				const unsigned int randomSeed = randomGenerator.seed();

				performance.startIf(performanceIteration);
					CV::Synthesis::InitializerAppearanceMappingI1<patchSize, iterations>(layer, randomGenerator).invoke(useWorker);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, copyFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (useWorker == nullptr)
				{
					const unsigned int patchSize_2 = patchSize / 2u;

					RandomGenerator helperGenerator(randomSeed);
					RandomGenerator localGenerator(helperGenerator);

					for (unsigned int y = 0u; y < frame.height(); ++y)
					{
						for (unsigned int x = 0u; x < frame.width(); ++x)
						{
							if (mask.constpixel<uint8_t>(x, y)[0] != 0xFFu)
							{
								CV::PixelPosition bestPosition;
								unsigned int bestCost = (unsigned int)(-1);

								while (true)
								{
									const unsigned int xCandidate = RandomI::random(localGenerator, patchSize_2, testWidth - patchSize_2 - 1u);
									const unsigned int yCandidate = RandomI::random(localGenerator, patchSize_2, testHeight - patchSize_2 - 1u);

									if (mask.constpixel<uint8_t>(xCandidate, yCandidate)[0] == 0xFFu)
									{
										bestPosition = CV::PixelPosition(xCandidate, yCandidate);
										bestCost = CV::SumSquareDifferencesBase::patch8BitPerChannel(frame.constdata<uint8_t>(), frame.constdata<uint8_t>(), frame.channels(), patchSize, frame.width(), frame.width(), x, y, xCandidate, yCandidate, frame.paddingElements(), frame.paddingElements());
										break;
									}
								}

								for (unsigned int n = 1u; n < iterations; ++n)
								{
									const unsigned int xCandidate = RandomI::random(localGenerator, patchSize_2, testWidth - patchSize_2 - 1u);
									const unsigned int yCandidate = RandomI::random(localGenerator, patchSize_2, testHeight - patchSize_2 - 1u);

									if (mask.constpixel<uint8_t>(xCandidate, yCandidate)[0] == 0xFFu)
									{
										const unsigned int candidateCost = CV::SumSquareDifferencesBase::patch8BitPerChannel(frame.constdata<uint8_t>(), frame.constdata<uint8_t>(), frame.channels(), patchSize, frame.width(), frame.width(), x, y, xCandidate, yCandidate, frame.paddingElements(), frame.paddingElements());;

										if (candidateCost < bestCost)
										{
											bestPosition = CV::PixelPosition(xCandidate, yCandidate);
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

					for (unsigned int y = 0u; y < frame.height(); ++y)
					{
						for (unsigned int x = 0u; x < frame.width(); ++x)
						{
							if (mask.constpixel<uint8_t>(x, y)[0] != 0xFFu)
							{
								const CV::PixelPosition& position = mapping.position(x, y);

								if (mask.constpixel<uint8_t>(position.x(), position.y())[0] != 0xFFu)
								{
									allSucceeded = false;
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

bool TestInitializerI1::testCoarserMappingAdaption(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing coarser mapping adaption for " << width << "x" << height << ":";

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

bool TestInitializerI1::testCoarserMappingAdaption(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
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

				const Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

				CV::PixelBoundingBox boundingBox;
				if (RandomI::random(randomGenerator, 1u) == 0u)
				{
					boundingBox = CV::MaskAnalyzer::detectBoundingBox(mask.constdata<uint8_t>(), mask.width(), mask.height(), 0xFFu, mask.paddingElements());
					ocean_assert(boundingBox.isValid());
				}

				CV::Synthesis::LayerI1 layer(frame, mask, boundingBox);
				CV::Synthesis::MappingI1& mapping = layer.mappingI1();

				constexpr unsigned int factor = 2u;

				ocean_assert(testWidth % factor == 0u);
				ocean_assert(testHeight % factor == 0u);
				const unsigned int coarserTestWidth = testWidth / factor;
				const unsigned int coarserTestHeight = testHeight / factor;

				Frame coarserFrame = CV::CVUtilities::randomizedFrame(FrameType(frame, coarserTestWidth, coarserTestHeight), &randomGenerator);

				const Frame coarserMask = Utilities::randomizedInpaintingMask(coarserTestWidth, coarserTestHeight, 0x00u, randomGenerator);

				CV::Synthesis::LayerI1 coarserLayer(coarserFrame, coarserMask);
				CV::Synthesis::MappingI1& coarserMapping = coarserLayer.mappingI1();

				for (unsigned int y = 0u; y < coarserMask.height(); ++y)
				{
					for (unsigned int x = 0u; x < coarserMask.width(); ++x)
					{
						if (coarserMask.constpixel<uint8_t>(x, y)[0] == 0x00u)
						{
							unsigned int sourceX, sourceY;

							do
							{
								sourceX = RandomI::random(coarserMask.width() - 1u);
								sourceY = RandomI::random(coarserMask.height() - 1u);
							}
							while (coarserMask.constpixel<uint8_t>(sourceX, sourceY)[0] != 0xFF);

							coarserMapping.setPosition(x, y, CV::PixelPosition(sourceX, sourceY));
						}
					}
				}

				const unsigned int randomSeed = randomGenerator.seed();

				performance.startIf(performanceIteration);
					CV::Synthesis::InitializerCoarserMappingAdaptionI1<factor>(layer, randomGenerator, coarserLayer).invoke(useWorker);
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

								if (coarserMask.constpixel<uint8_t>(xCoarser, yCoarser)[0] != 0xFFu)
								{
									const CV::PixelPosition& lowerPosition = coarserMapping.position(xCoarser, yCoarser);

									const int xLowerOffset = int(lowerPosition.x()) - int(xCoarser);
									const int yLowerOffset = int(lowerPosition.y()) - int(yCoarser);

									const unsigned int xPosition = (unsigned int)(minmax(0, int(x) + xLowerOffset * 2, int(mask.width() - 1)));
									const unsigned int yPosition = (unsigned int)(minmax(0, int(y) + yLowerOffset * 2, int(mask.height() - 1)));

									if (mask.constpixel<uint8_t>(xPosition, yPosition)[0] == 0xFFu)
									{
										if (mapping.position(x, y) != CV::PixelPosition(xPosition, yPosition))
										{
											allSucceeded = false;
										}

										continue;
									}
								}

								while (true)
								{
									const unsigned int xCandidate = RandomI::random(localGenerator, mask.width() - 1u);
									const unsigned int yCandidate = RandomI::random(localGenerator, mask.height() - 1u);

									if (mask.constpixel<uint8_t>(xCandidate, yCandidate)[0] == 0xFFu)
									{
										if (mapping.position(x, y) != CV::PixelPosition(xCandidate, yCandidate))
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
				else
				{
					// due to multi-core execution the result will not be deterministic - thus, ensuring that we have a valid mapping only

					for (unsigned int y = 0u; y < frame.height(); ++y)
					{
						for (unsigned int x = 0u; x < frame.width(); ++x)
						{
							if (mask.constpixel<uint8_t>(x, y)[0] != 0xFFu)
							{
								const CV::PixelPosition& position = mapping.position(x, y);

								if (mask.constpixel<uint8_t>(position.x(), position.y())[0] != 0xFFu)
								{
									allSucceeded = false;
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

bool TestInitializerI1::testCoarserMappingAdaptionAreaConstrained(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing area constrained coarser mapping adaption for " << width << "x" << height << ":";

	bool allSucceeded = true;

	for (const unsigned int channels : {1u, 2u, 3u, 4u})
	{
		Log::info() << " ";

		if (!testCoarserMappingAdaptionAreaConstrained(width, height, channels, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Area constrained coarser mapping adaption test succeeded.";
	}
	else
	{
		Log::info() << "Area constrained coarser mapping adaption test FAILED!";
	}

	return allSucceeded;
}

bool TestInitializerI1::testCoarserMappingAdaptionAreaConstrained(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
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

				const Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

				CV::PixelBoundingBox boundingBox;
				if (RandomI::random(randomGenerator, 1u) == 0u)
				{
					boundingBox = CV::MaskAnalyzer::detectBoundingBox(mask.constdata<uint8_t>(), mask.width(), mask.height(), 0xFFu, mask.paddingElements());
					ocean_assert(boundingBox.isValid());
				}

				CV::Synthesis::LayerI1 layer(frame, mask, boundingBox);
				CV::Synthesis::MappingI1& mapping = layer.mappingI1();

				constexpr unsigned int factor = 2u;

				ocean_assert(testWidth % factor == 0u);
				ocean_assert(testHeight % factor == 0u);
				const unsigned int coarserTestWidth = testWidth / factor;
				const unsigned int coarserTestHeight = testHeight / factor;

				Frame coarserFrame = CV::CVUtilities::randomizedFrame(FrameType(frame, coarserTestWidth, coarserTestHeight), &randomGenerator);

				const Frame coarserMask = Utilities::randomizedInpaintingMask(coarserTestWidth, coarserTestHeight, 0x00u, randomGenerator);

				CV::Synthesis::LayerI1 coarserLayer(coarserFrame, coarserMask);
				CV::Synthesis::MappingI1& coarserMapping = coarserLayer.mappingI1();

				for (unsigned int y = 0u; y < coarserMask.height(); ++y)
				{
					const uint8_t* maskRow = coarserMask.constrow<uint8_t>(y);

					for (unsigned int x = 0u; x < coarserMask.width(); ++x)
					{
						if (maskRow[x] == 0x00)
						{
							unsigned int sourceX, sourceY;

							do
							{
								sourceX = RandomI::random(coarserMask.width() - 1u);
								sourceY = RandomI::random(coarserMask.height() - 1u);
							}
							while (coarserMask.constpixel<uint8_t>(sourceX, sourceY)[0] != 0xFF);

							coarserMapping.setPosition(x, y, CV::PixelPosition(sourceX, sourceY));
						}
					}
				}

				const Frame filterMask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

				const unsigned int randomSeed = randomGenerator.seed();

				performance.startIf(performanceIteration);
					CV::Synthesis::InitializerCoarserMappingAdaptionAreaConstrainedI1<factor>(layer, randomGenerator, coarserLayer, filterMask).invoke(useWorker);
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

								if (coarserMask.constpixel<uint8_t>(xCoarser, yCoarser)[0] != 0xFFu)
								{
									const CV::PixelPosition& lowerPosition = coarserMapping.position(xCoarser, yCoarser);

									const int xLowerOffset = int(lowerPosition.x()) - int(xCoarser);
									const int yLowerOffset = int(lowerPosition.y()) - int(yCoarser);

									const unsigned int xPosition = (unsigned int)(minmax(0, int(x) + xLowerOffset * 2, int(mask.width() - 1)));
									const unsigned int yPosition = (unsigned int)(minmax(0, int(y) + yLowerOffset * 2, int(mask.height() - 1)));

									if (mask.constpixel<uint8_t>(xPosition, yPosition)[0] == 0xFFu && filterMask.constpixel<uint8_t>(xPosition, yPosition)[0] == 0xFFu)
									{
										if (mapping.position(x, y) != CV::PixelPosition(xPosition, yPosition))
										{
											allSucceeded = false;
										}

										continue;
									}
								}

								while (true)
								{
									const unsigned int xCandidate = RandomI::random(localGenerator, mask.width() - 1u);
									const unsigned int yCandidate = RandomI::random(localGenerator, mask.height() - 1u);

									if (mask.constpixel<uint8_t>(xCandidate, yCandidate)[0] == 0xFFu && filterMask.constpixel<uint8_t>(xCandidate, yCandidate)[0] == 0xFFu)
									{
										if (mapping.position(x, y) != CV::PixelPosition(xCandidate, yCandidate))
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
				else
				{
					// due to multi-core execution the result will not be deterministic - thus, ensuring that we have a valid mapping only

					for (unsigned int y = 0u; y < frame.height(); ++y)
					{
						for (unsigned int x = 0u; x < frame.width(); ++x)
						{
							if (mask.constpixel<uint8_t>(x, y)[0] != 0xFFu)
							{
								const CV::PixelPosition& position = mapping.position(x, y);

								if (mask.constpixel<uint8_t>(position.x(), position.y())[0] != 0xFFu || filterMask.constpixel<uint8_t>(position.x(), position.y())[0] != 0xFFu)
								{
									allSucceeded = false;
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

bool TestInitializerI1::testCoarserMappingAdaptionSpatialCostMask(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing spatial cost mask coarser mapping adaption for " << width << "x" << height << ":";

	bool allSucceeded = true;

	for (const unsigned int channels : {1u, 2u, 3u, 4u})
	{
		Log::info() << " ";

		if (!testCoarserMappingAdaptionSpatialCostMask(width, height, channels, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Spatial cost mask coarser mapping adaption test succeeded.";
	}
	else
	{
		Log::info() << "Spatial cost mask coarser mapping adaption test FAILED!";
	}

	return allSucceeded;
}

bool TestInitializerI1::testCoarserMappingAdaptionSpatialCostMask(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
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

				const Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

				CV::PixelBoundingBox boundingBox;
				if (RandomI::random(randomGenerator, 1u) == 0u)
				{
					boundingBox = CV::MaskAnalyzer::detectBoundingBox(mask.constdata<uint8_t>(), mask.width(), mask.height(), 0xFFu, mask.paddingElements());
					ocean_assert(boundingBox.isValid());
				}

				CV::Synthesis::LayerI1 layer(frame, mask, boundingBox);
				CV::Synthesis::MappingI1& mapping = layer.mappingI1();

				constexpr unsigned int factor = 2u;

				ocean_assert(testWidth % factor == 0u);
				ocean_assert(testHeight % factor == 0u);
				const unsigned int coarserTestWidth = testWidth / factor;
				const unsigned int coarserTestHeight = testHeight / factor;

				Frame coarserFrame = CV::CVUtilities::randomizedFrame(FrameType(frame, coarserTestWidth, coarserTestHeight), &randomGenerator);

				const Frame coarserMask = Utilities::randomizedInpaintingMask(coarserTestWidth, coarserTestHeight, 0x00u, randomGenerator);

				CV::Synthesis::LayerI1 coarserLayer(coarserFrame, coarserMask);
				CV::Synthesis::MappingI1& coarserMapping = coarserLayer.mappingI1();

				for (unsigned int y = 0u; y < coarserMask.height(); ++y)
				{
					const uint8_t* maskRow = coarserMask.constrow<uint8_t>(y);

					for (unsigned int x = 0u; x < coarserMask.width(); ++x)
					{
						if (maskRow[x] == 0x00)
						{
							unsigned int sourceX, sourceY;

							do
							{
								sourceX = RandomI::random(coarserMask.width() - 1u);
								sourceY = RandomI::random(coarserMask.height() - 1u);
							}
							while (coarserMask.constpixel<uint8_t>(sourceX, sourceY)[0] != 0xFF);

							coarserMapping.setPosition(x, y, CV::PixelPosition(sourceX, sourceY));
						}
					}
				}

				// make some coarser mappings neighbors

				for (unsigned int n = 0u; n < 50u; ++n)
				{
					const unsigned int x = RandomI::random(randomGenerator, coarserMask.width() - 1u);
					const unsigned int y = RandomI::random(randomGenerator, coarserMask.height() - 1u);

					if (coarserMask.constpixel<uint8_t>(x, y)[0] == 0x00)
					{
						const CV::PixelPosition& position = coarserMapping.position(x, y);

						for (int yOffset = -1; yOffset <= 1; ++yOffset)
						{
							int yy = int(y) + yOffset;

							if (yy < 0 || yy >= int(coarserMask.height()))
							{
								continue;
							}

							for (int xOffset = -1; xOffset <= 1; ++xOffset)
							{
								int xx = int(x) + xOffset;

								if (xOffset == 0 && yOffset == 0)
								{
									continue;
								}

								if (xx < 0 || xx >= int(coarserMask.width()))
								{
									continue;
								}

								if (coarserMask.constpixel<uint8_t>((unsigned int)(xx), (unsigned int)(yy))[0] == 0x00)
								{
									const int xMapping = int(position.x()) + xOffset;
									const int yMapping = int(position.y()) + yOffset;

									if (xMapping >= 0 && xMapping < int(coarserMapping.width()) && yMapping >= 0 && yMapping < int(coarserMapping.height()))
									{
										coarserMapping.setPosition((unsigned int)(xx), (unsigned int)(yy), CV::PixelPosition((unsigned int)(xMapping), (unsigned int)(yMapping)));
									}
								}
							}
						}
					}
				}

				const unsigned int neighborhood = RandomI::random(randomGenerator, 1u) == 0u ? 1u : 9u;

				const unsigned int randomSeed = randomGenerator.seed();

				Frame coarserLayerSpatialCost(coarserMask.frameType());
				if (!CV::Synthesis::CreatorInformationSpatialCostI1<4u, true>(coarserLayer, coarserLayerSpatialCost).invoke(useWorker))
				{
					allSucceeded = false;
				}

				Frame costMask;

				performance.startIf(performanceIteration);
					if (neighborhood == 1u)
					{
						if (!CV::Synthesis::InitializerCoarserMappingAdaptionSpatialCostMaskI1<factor, 1u>(layer, randomGenerator, coarserLayer, costMask).invoke(useWorker))
						{
							allSucceeded = false;
						}
					}
					else if (neighborhood == 9u)
					{
						if (!CV::Synthesis::InitializerCoarserMappingAdaptionSpatialCostMaskI1<factor, 9u>(layer, randomGenerator, coarserLayer, costMask).invoke(useWorker))
						{
							allSucceeded = false;
						}
					}
					else
					{
						ocean_assert(false && "Invalid neighborhood!");
						allSucceeded = false;
					}
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, copyFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (!costMask.isValid())
				{
					ocean_assert(false && "This should never happen!");
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

								if (coarserMask.constpixel<uint8_t>(xCoarser, yCoarser)[0] != 0xFFu)
								{
									const CV::PixelPosition& lowerPosition = coarserMapping.position(xCoarser, yCoarser);

									const int xLowerOffset = int(lowerPosition.x()) - int(xCoarser);
									const int yLowerOffset = int(lowerPosition.y()) - int(yCoarser);

									const unsigned int xPosition = (unsigned int)(minmax(0, int(x) + xLowerOffset * 2, int(mask.width() - 1)));
									const unsigned int yPosition = (unsigned int)(minmax(0, int(y) + yLowerOffset * 2, int(mask.height() - 1)));

									if (mask.constpixel<uint8_t>(xPosition, yPosition)[0] == 0xFFu)
									{
										if (mapping.position(x, y) != CV::PixelPosition(xPosition, yPosition))
										{
											allSucceeded = false;
										}

										const uint8_t costValue = costMask.constpixel<uint8_t>(x, y)[0];

										if (allValueSame(coarserLayerSpatialCost, xCoarser, yCoarser, 0x80u, neighborhood))
										{
											if (costValue != 0xFFu)
											{
												allSucceeded = false;
											}
										}
										else
										{
											if (costValue != 0x00u)
											{
												allSucceeded = false;
											}
										}

										continue;
									}
								}

								while (true)
								{
									const unsigned int xCandidate = RandomI::random(localGenerator, mask.width() - 1u);
									const unsigned int yCandidate = RandomI::random(localGenerator, mask.height() - 1u);

									if (mask.constpixel<uint8_t>(xCandidate, yCandidate)[0] == 0xFFu)
									{
										if (mapping.position(x, y) != CV::PixelPosition(xCandidate, yCandidate))
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
				else
				{
					// due to multi-core execution the result will not be deterministic - thus, ensuring that we have a valid mapping only

					for (unsigned int y = 0u; y < frame.height(); ++y)
					{
						for (unsigned int x = 0u; x < frame.width(); ++x)
						{
							if (mask.constpixel<uint8_t>(x, y)[0] != 0xFFu)
							{
								const CV::PixelPosition& position = mapping.position(x, y);

								if (mask.constpixel<uint8_t>(position.x(), position.y())[0] != 0xFFu)
								{
									allSucceeded = false;
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

bool TestInitializerI1::testRandomMapping(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing random mapping:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;

		const Timestamp startTimestamp(true);

		do
		{
			const unsigned int testWidth = RandomI::random(randomGenerator, 10u, 500u);
			const unsigned int testHeight = RandomI::random(randomGenerator, 10u, 500u);

			const unsigned int channels = RandomI::random(randomGenerator, 1u, 4u);

			Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			const Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

			CV::PixelBoundingBox boundingBox;
			if (RandomI::random(randomGenerator, 1u) == 0u)
			{
				boundingBox = CV::MaskAnalyzer::detectBoundingBox(mask.constdata<uint8_t>(), mask.width(), mask.height(), 0xFFu, mask.paddingElements());
				ocean_assert(boundingBox.isValid());
			}

			CV::Synthesis::LayerI1 layer(frame, mask, boundingBox);

			if (!CV::Synthesis::InitializerRandomMappingI1(layer, randomGenerator).invoke(useWorker))
			{
				allSucceeded = false;
			}

			for (unsigned int y = 0u; y < mask.height(); ++y)
			{
				for (unsigned int x = 0u; x < mask.width(); ++x)
				{
					if (mask.constpixel<uint8_t>(x, y)[0] != 0xFFu)
					{
						const CV::PixelPosition& sourcePosition = layer.mapping().position(x, y);

						if (sourcePosition.isValid() && sourcePosition.x() < mask.width() && sourcePosition.y() < mask.height())
						{
							if (mask.constpixel<uint8_t>(sourcePosition.x(), sourcePosition.y())[0] != 0xFFu)
							{
								allSucceeded = false;
							}
						}
						else
						{
							allSucceeded = false;
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

bool TestInitializerI1::testRandomMappingAreaConstrained(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing area constrained random mapping:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;

		const Timestamp startTimestamp(true);

		do
		{
			const unsigned int testWidth = RandomI::random(randomGenerator, 10u, 500u);
			const unsigned int testHeight = RandomI::random(randomGenerator, 10u, 500u);

			const unsigned int channels = RandomI::random(randomGenerator, 1u, 4u);

			Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			Frame mask;
			Frame filter;

			while (true)
			{
				mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);
				filter = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

				// we need to ensure that there is at least one valid mask pixel with valid filter pixel

				bool validFilter = false;

				for (unsigned int y = 0u; !validFilter && y < mask.height(); ++y)
				{
					for (unsigned int x = 0u; x < mask.width(); ++x)
					{
						if (mask.constpixel<uint8_t>(x, y)[0] == 0xFFu && filter.constpixel<uint8_t>(x, y)[0] == 0xFFu)
						{
							validFilter = true;
							break;
						}
					}
				}

				if (validFilter)
				{
					break;
				}
			}

			CV::PixelBoundingBox boundingBox;
			if (RandomI::random(randomGenerator, 1u) == 0u)
			{
				boundingBox = CV::MaskAnalyzer::detectBoundingBox(mask.constdata<uint8_t>(), mask.width(), mask.height(), 0xFFu, mask.paddingElements());
				ocean_assert(boundingBox.isValid());
			}

			CV::Synthesis::LayerI1 layer(frame, mask, boundingBox);

			if (!CV::Synthesis::InitializerRandomMappingAreaConstrainedI1(layer, randomGenerator, filter).invoke(useWorker))
			{
				allSucceeded = false;
			}

			for (unsigned int y = 0u; y < mask.height(); ++y)
			{
				for (unsigned int x = 0u; x < mask.width(); ++x)
				{
					if (mask.constpixel<uint8_t>(x, y)[0] != 0xFFu)
					{
						const CV::PixelPosition& sourcePosition = layer.mapping().position(x, y);

						if (sourcePosition.isValid() && sourcePosition.x() < mask.width() && sourcePosition.y() < mask.height())
						{
							if (mask.constpixel<uint8_t>(sourcePosition.x(), sourcePosition.y())[0] != 0xFFu)
							{
								allSucceeded = false;
							}

							if (filter.constpixel<uint8_t>(sourcePosition.x(), sourcePosition.y())[0] != 0xFFu)
							{
								allSucceeded = false;
							}
						}
						else
						{
							allSucceeded = false;
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

bool TestInitializerI1::testShrinkingErosion(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing deterministic erosion shrinking for " << width << "x" << height << ":";

	bool allSucceeded = true;

	for (const unsigned int channels : {1u, 2u, 3u, 4u})
	{
		Log::info() << " ";

		if (!testShrinkingErosion(width, height, channels, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Deterministic erosion shrinking test succeeded.";
	}
	else
	{
		Log::info() << "Deterministic erosion shrinking test FAILED!";
	}

	return allSucceeded;
}

bool TestInitializerI1::testShrinkingErosion(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
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

				const Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);
				const Frame copyMask(mask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				CV::PixelBoundingBox boundingBox;
				if (RandomI::random(randomGenerator, 1u) == 0u)
				{
					boundingBox = CV::MaskAnalyzer::detectBoundingBox(mask.constdata<uint8_t>(), mask.width(), mask.height(), 0xFFu, mask.paddingElements());
					ocean_assert(boundingBox.isValid());
				}

				CV::Synthesis::LayerI1 layer(frame, mask, boundingBox);

				CV::Synthesis::InitializerAppearanceMappingI1<1u, 100u> appearanceInitializer(layer, randomGenerator);

				const unsigned int randomSeed = randomGenerator.seed();

				performance.startIf(performanceIteration);
					CV::Synthesis::InitializerShrinkingErosionI1(layer, randomGenerator, appearanceInitializer).invoke(useWorker);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, copyFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				// even with multi-core execution, the shrinker is still deterministic, so we can verify the result with and without multi-core execution

				RandomGenerator localGenerator(randomSeed);

				Frame testFrame(copyFrame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
				Frame testMask(copyMask, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

				constexpr unsigned int randomNoise = 3u;

				if (CV::FrameFilterErosion::Comfort::shrinkMask(testFrame, testMask, CV::FrameFilterErosion::MF_SQUARE_3, randomNoise, RandomI::random32(localGenerator)))
				{
					const Frame& initializedFrame = layer.frame();

					for (unsigned int y = 0u; y < testFrame.height(); ++y)
					{
						for (unsigned int x = 0u; x < testFrame.width(); ++x)
						{
							const uint8_t* testPixel = testFrame.constpixel<uint8_t>(x, y);
							const uint8_t* initializedPixel = initializedFrame.constpixel<uint8_t>(x, y);

							for (unsigned int n = 0u; n < testFrame.channels(); ++n)
							{
								if (testPixel[n] != initializedPixel[n])
								{
									allSucceeded = false;
								}
							}

							if (testMask.constpixel<uint8_t>(x, y)[0] != 0xFFu)
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

bool TestInitializerI1::testShrinkingErosionRandomized(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing randomized erosion shrinking for " << width << "x" << height << ":";

	bool allSucceeded = true;

	for (const unsigned int channels : {1u, 2u, 3u, 4u})
	{
		Log::info() << " ";

		if (!testShrinkingErosionRandomized(width, height, channels, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Randomized erosion shrinking test succeeded.";
	}
	else
	{
		Log::info() << "Randomized erosion shrinking test FAILED!";
	}

	return allSucceeded;
}

bool TestInitializerI1::testShrinkingErosionRandomized(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
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

				const Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);
				const Frame copyMask(mask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				CV::PixelBoundingBox boundingBox;
				if (RandomI::random(randomGenerator, 1u) == 0u)
				{
					boundingBox = CV::MaskAnalyzer::detectBoundingBox(mask.constdata<uint8_t>(), mask.width(), mask.height(), 0xFFu, mask.paddingElements());
					ocean_assert(boundingBox.isValid());
				}

				CV::Synthesis::LayerI1 layer(frame, mask, boundingBox);

				CV::Synthesis::InitializerAppearanceMappingI1<1u, 100u> appearanceInitializer(layer, randomGenerator);

				const unsigned int randomSeed = randomGenerator.seed();

				performance.startIf(performanceIteration);
					CV::Synthesis::InitializerShrinkingErosionRandomizedI1(layer, randomGenerator, appearanceInitializer).invoke(useWorker);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, copyFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				// even with multi-core execution, the shrinker is still deterministic, so we can verify the result with and without multi-core execution

				RandomGenerator localGenerator(randomSeed);

				Frame testFrame(copyFrame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
				Frame testMask(copyMask, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

				constexpr unsigned int randomNoise = 3u;

				if (CV::FrameFilterErosion::Comfort::shrinkMaskRandom(testFrame, testMask, CV::FrameFilterErosion::MF_SQUARE_3, randomNoise, RandomI::random32(localGenerator)))
				{
					const Frame& initializedFrame = layer.frame();

					for (unsigned int y = 0u; y < testFrame.height(); ++y)
					{
						for (unsigned int x = 0u; x < testFrame.width(); ++x)
						{
							const uint8_t* testPixel = testFrame.constpixel<uint8_t>(x, y);
							const uint8_t* initializedPixel = initializedFrame.constpixel<uint8_t>(x, y);

							for (unsigned int n = 0u; n < testFrame.channels(); ++n)
							{
								if (testPixel[n] != initializedPixel[n])
								{
									allSucceeded = false;
								}
							}

							if (testMask.constpixel<uint8_t>(x, y)[0] != 0xFFu)
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

bool TestInitializerI1::testShrinkingPatchMatching(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int width = 100u;
	constexpr unsigned int height = 100u;

	Log::info() << "Testing patch matching shrinking for " << width << "x" << height << ":";

	bool allSucceeded = true;

	for (const unsigned int channels : {1u, 2u, 3u, 4u})
	{
		Log::info() << " ";

		if (!testShrinkingPatchMatching(width, height, channels, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Patch matching shrinking test succeeded.";
	}
	else
	{
		Log::info() << "Patch matching shrinking test FAILED!";
	}

	return allSucceeded;
}

bool TestInitializerI1::testShrinkingPatchMatching(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
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
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 10u, width / 2u);
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 10u, height / 2u);

				Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

				const Frame copyFrame(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				const Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);
				const Frame copyMask(mask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				CV::PixelBoundingBox boundingBox;
				if (RandomI::random(randomGenerator, 1u) == 0u)
				{
					boundingBox = CV::MaskAnalyzer::detectBoundingBox(mask.constdata<uint8_t>(), mask.width(), mask.height(), 0xFFu, mask.paddingElements());
					ocean_assert(boundingBox.isValid());
				}

				CV::Synthesis::LayerI1 layer(frame, mask, boundingBox);
				CV::Synthesis::MappingI1& mapping = layer.mappingI1();

				const unsigned int initializationIterations = RandomI::random(randomGenerator, 1u, 2u);
				const bool useHeuristic = RandomI::random(randomGenerator, 1u) == 0u;
				unsigned int maximalBoundingBoxOffset = (unsigned int)(-1);

				if (RandomI::random(randomGenerator, 1u) == 0u)
				{
					maximalBoundingBoxOffset = RandomI::random(randomGenerator, 1u, 100u);
				}

				const unsigned int randomSeed = randomGenerator.seed();

				performance.startIf(performanceIteration);
					CV::Synthesis::InitializerShrinkingPatchMatchingI1(layer, randomGenerator, initializationIterations, useHeuristic, maximalBoundingBoxOffset).invoke(useWorker);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, copyFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				// even with multi-core execution, the shrinker is still deterministic, so we can verify the result with and without multi-core execution

				RandomGenerator helperGenerator(randomSeed);
				RandomGenerator localGenerator(helperGenerator);

				Frame testFrame(copyFrame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

				CV::Synthesis::MappingI1 copyMapping(mapping);
				copyMapping.reset();

				ocean_assert(initializationIterations >= 1u);
				for (unsigned int i = 0u; i < initializationIterations; ++i)
				{
					const bool useMaskForSSD = i == 0u;

					Frame testMask(copyMask, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

					if (!shrinkPatchMatchingIteration(testFrame, testMask, copyMapping, useHeuristic, maximalBoundingBoxOffset, localGenerator, useMaskForSSD, worker))
					{
						allSucceeded = false;
					}
				}

				const Frame& initializedFrame = layer.frame();

				for (unsigned int y = 0u; y < testFrame.height(); ++y)
				{
					for (unsigned int x = 0u; x < testFrame.width(); ++x)
					{
						const uint8_t* testPixel = testFrame.constpixel<uint8_t>(x, y);
						const uint8_t* initializedPixel = initializedFrame.constpixel<uint8_t>(x, y);

						for (unsigned int n = 0u; n < testFrame.channels(); ++n)
						{
							if (testPixel[n] != initializedPixel[n])
							{
								allSucceeded = false;
							}
						}

						if (copyMapping.position(x, y) != layer.mapping().position(x, y))
						{
							allSucceeded = false;
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

bool TestInitializerI1::shrinkPatchMatchingIteration(Frame& frame, Frame& mask, CV::Synthesis::MappingI1& mapping, const bool useHeuristic, const unsigned int maximalRadius, RandomGenerator& randomGenerator, const bool useMaskForSSD, Worker& worker)
{
	ocean_assert(maximalRadius >= 1u);

	/**
	 * Propagation offsets to be checked with order: self, left, top-left, top, top-right, right, ...
	 *
	 * 2 3 4
	 * 1 0 5
	 * 8 7 6
	 */
	const VectorsI2 propagationOffsets =
	{
		VectorI2(0, 0), // self
		VectorI2(-1, 0), // left
		VectorI2(-1, -1), // top-left
		VectorI2(0, -1), // top
		VectorI2(1, -1),
		VectorI2(1, 0),
		VectorI2(1, 1),
		VectorI2(0, 1),
		VectorI2(-1, 1)
	};

	const Scalar frameDiagonal = Vector2(Scalar(frame.width()), Scalar(frame.height())).length();
	const Scalar diagonal_4 = min(frameDiagonal * Scalar(0.25), Scalar(10));

	for (unsigned int y = 0u; y < mapping.height(); ++y)
	{
		for (unsigned int x = 0u; x < mapping.width(); ++x)
		{
			const CV::PixelPosition& location = mapping.position(x, y);

			ocean_assert(!location.isValid() || location.x() < frame.width() && location.y() < frame.height());
			if (location.isValid() && (location.x() >= frame.width() || location.y() >= frame.height()))
			{
				return false;
			}
		}
	}

	Frame sobel = CV::FrameFilterSobel::Comfort::filterHorizontalVertical(frame, FrameType::DT_SIGNED_INTEGER_16);

	const Frame copyMask(mask, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

	CV::PixelPositions borderPixels;
	CV::Segmentation::MaskAnalyzer::findBorderPixels4(mask.constdata<uint8_t>(), mask.width(), mask.height(), mask.paddingElements(), borderPixels, CV::PixelBoundingBox(), &worker);

	InpaintingPixels inpaintingPixels;

	for (const CV::PixelPosition& borderPixel : borderPixels)
	{
		inpaintingPixels.emplace_back(borderPixel, mask, sobel);
	}

	std::sort(inpaintingPixels.begin(), inpaintingPixels.end());

	while (!inpaintingPixels.empty())
	{
		const InpaintingPixel inpaintingPixel = inpaintingPixels.back();
		inpaintingPixels.pop_back();

		uint32_t ssdBest = uint32_t(-1);
		CV::PixelPosition bestMapping;

		if (useHeuristic)
		{
			for (const VectorI2& propagationOffset : propagationOffsets)
			{
				if (int(inpaintingPixel.x()) + propagationOffset.x() < 0 || int(inpaintingPixel.x()) + propagationOffset.x() >= int(frame.width()))
				{
					continue;
				}

				if (int(inpaintingPixel.y()) + propagationOffset.y() < 0 || int(inpaintingPixel.y()) + propagationOffset.y() >= int(frame.height()))
				{
					continue;
				}

				const CV::PixelPosition mappingPosition = mapping.position((unsigned int)(int(inpaintingPixel.x()) + propagationOffset.x()), (unsigned int)(int(inpaintingPixel.y()) + propagationOffset.y()));

				if (mappingPosition.isValid())
				{
					const int xLookupPosition = int(mappingPosition.x()) - propagationOffset.x();
					const int yLookupPosition = int(mappingPosition.y()) - propagationOffset.y();

					for (int yMappingPosition = yLookupPosition - 3; yMappingPosition <= yLookupPosition + 3; ++yMappingPosition)
					{
						if (yMappingPosition >= 0 && yMappingPosition < int(frame.height()))
						{
							for (int xMappingPosition = xLookupPosition - 3; xMappingPosition <= xLookupPosition + 3; ++xMappingPosition)
							{
								if (xMappingPosition >= 0 && xMappingPosition < int(frame.width()))
								{
									if (xMappingPosition != int(inpaintingPixel.x()) || yMappingPosition != int(inpaintingPixel.y()))
									{
										if (copyMask.constpixel<uint8_t>((unsigned int)(xMappingPosition), (unsigned int)(yMappingPosition))[0] == 0xFFu)
										{
											constexpr unsigned int patchSize = 5u;

											const unsigned int ssd = determineSSD(frame, mask, patchSize, inpaintingPixel, CV::PixelPosition((unsigned int)(xMappingPosition), (unsigned int)(yMappingPosition)), useMaskForSSD);

											if (ssd < ssdBest)
											{
												ssdBest = ssd;

												bestMapping = CV::PixelPosition((unsigned int)(xMappingPosition), (unsigned int)(yMappingPosition));
											}
										}
									}
								}
							}
						}
					}
				}
			}

			// let's try to improve the matching

			Vector3 normal(Scalar(inpaintingPixel.borderDirection().x()), Scalar(inpaintingPixel.borderDirection().y()), 0);

			constexpr unsigned int guidedRandomIterations = 100u;
			constexpr unsigned int unguidedRandomIterations = 200u;

			if (normal.normalize())
			{
				for (unsigned int n = 0u; n < guidedRandomIterations; ++n)
				{
					const Quaternion quaternion(Vector3(0, 0, 1), Random::scalar(randomGenerator, -Numeric::deg2rad(90), Numeric::deg2rad(90)));

					const Scalar length = Random::scalar(randomGenerator, Scalar(1), diagonal_4);
					const Vector3 offset = quaternion * (normal * length); // random offset within +/- 90 deg of the border direction

					for (const Scalar sign : {Scalar(1), Scalar(-1)})
					{
						const int xSourcePixel = int(inpaintingPixel.x()) + Numeric::round32(offset.x() * sign);
						const int ySourcePixel = int(inpaintingPixel.y()) + Numeric::round32(offset.y() * sign);

						if (xSourcePixel >= 0 && xSourcePixel < int(frame.width()))
						{
							if (ySourcePixel >= 0 && ySourcePixel < int(frame.height()))
							{
								const CV::PixelPosition sourcePixel((unsigned int)(xSourcePixel), (unsigned int)(ySourcePixel));

								if (copyMask.constpixel<uint8_t>(sourcePixel.x(), sourcePixel.y())[0] == 0xFFu)
								{
									constexpr unsigned int patchSize = 5u;

									const unsigned int ssd = determineSSD(frame, mask, patchSize, inpaintingPixel, sourcePixel, useMaskForSSD);

									if (ssd < ssdBest)
									{
										ssdBest = ssd;

										bestMapping = sourcePixel;
									}
								}
							}
						}
					}
				}
			}

			if (bestMapping.isValid())
			{
				ocean_assert(bestMapping.x() < frame.width() && bestMapping.y() < frame.height());

				for (unsigned int n = 0u; n < unguidedRandomIterations; ++n)
				{
					const unsigned int xMaximalRadius = max(1u, (frame.width() - (frame.width() - 1u) * n / unguidedRandomIterations) / 2u);
					const unsigned int yMaximalRadius = max(1u, (frame.height() - (frame.height() - 1u) * n / unguidedRandomIterations) / 2u);

					const int offsetX = RandomI::random(randomGenerator, -int(xMaximalRadius), int(xMaximalRadius));
					const int offsetY = RandomI::random(randomGenerator, -int(yMaximalRadius), int(yMaximalRadius));

					const int xSourcePixel = int(bestMapping.x()) + offsetX;
					const int ySourcePixel = int(bestMapping.y()) + offsetY;

					if (xSourcePixel >= 0 && xSourcePixel < int(frame.width()))
					{
						if (ySourcePixel >= 0 && ySourcePixel < int(frame.height()))
						{
							if (xSourcePixel != int(inpaintingPixel.x()) || ySourcePixel != int(inpaintingPixel.y()))
							{
								const CV::PixelPosition sourcePixel((unsigned int)(xSourcePixel), (unsigned int)(ySourcePixel));

								if (copyMask.constpixel<uint8_t>(sourcePixel.x(), sourcePixel.y())[0] == 0xFFu)
								{
									constexpr unsigned int patchSize = 5u;

									const unsigned int ssd = determineSSD(frame, mask, patchSize, inpaintingPixel, sourcePixel, useMaskForSSD);

									if (ssd < ssdBest)
									{
										ssdBest = ssd;

										bestMapping = sourcePixel;
									}
								}
							}
						}
					}
				}
			}
		}

		if (ssdBest == uint32_t(-1))
		{
			// we have not found a best match yet, let's do brute force search

			const unsigned int left = (unsigned int)(std::max(int64_t(0), int64_t(inpaintingPixel.x()) - int64_t(maximalRadius)));
			const unsigned int top = (unsigned int)(std::max(int64_t(0), int64_t(inpaintingPixel.y()) - int64_t(maximalRadius)));

			const unsigned int rightEnd = (unsigned int)(std::min(uint64_t(inpaintingPixel.x()) + uint64_t(maximalRadius) + uint64_t(1), uint64_t(frame.width()))); // exclusive
			const unsigned int bottomEnd = (unsigned int)(std::min(uint64_t(inpaintingPixel.y()) + uint64_t(maximalRadius) + uint64_t(1), uint64_t(frame.height())));

			for (unsigned int y = top; y < bottomEnd; ++y)
			{
				for (unsigned int x = left; x < rightEnd; ++x)
				{
					if (y != inpaintingPixel.y() || x != inpaintingPixel.x())
					{
						if (copyMask.constpixel<uint8_t>(x, y)[0] == 0xFFu) // we accept any pixel with valid mask
						{
							constexpr unsigned int patchSize = 5u;

							const unsigned int ssd = determineSSD(frame, mask, patchSize, inpaintingPixel, CV::PixelPosition(x, y), useMaskForSSD);

							if (ssd < ssdBest)
							{
								ssdBest = ssd;

								bestMapping = CV::PixelPosition(x, y);
							}

						}
					}
				}
			}
		}

		if (ssdBest == uint32_t(-1))
		{
			// let's use the first non-mask pixel can we find

			for (unsigned int y = 0u; ssdBest == uint32_t(-1) && y < mask.height(); ++y)
			{
				for (unsigned int x = 0u; x < mask.width(); ++x)
				{
					if (mask.constpixel<uint8_t>(x, y)[0] == 0xFFu)
					{
						ssdBest = 0u;
						bestMapping = CV::PixelPosition(x, y);

						break;
					}
				}
			}
		}

		if (ssdBest == uint32_t(-1))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		// let's copy the pixel

		ocean_assert(bestMapping.x() < frame.width() && bestMapping.y() < frame.height());

		memcpy(frame.pixel<void>(inpaintingPixel.x(), inpaintingPixel.y()), frame.constpixel<void>(bestMapping.x(), bestMapping.y()), sizeof(uint8_t) * frame.channels());
		mask.pixel<uint8_t>(inpaintingPixel.x(), inpaintingPixel.y())[0] = 0xFFu;

		mapping.setPosition(inpaintingPixel.x(), inpaintingPixel.y(), bestMapping);

		// we need to update the Sobel filter response

		sobel = CV::FrameFilterSobel::Comfort::filterHorizontalVertical(frame, FrameType::DT_SIGNED_INTEGER_16, &worker);

		// let's restart over again

		borderPixels.clear();
		CV::Segmentation::MaskAnalyzer::findBorderPixels4(mask.constdata<uint8_t>(), mask.width(), mask.height(), mask.paddingElements(), borderPixels);

		inpaintingPixels.clear();

		for (const CV::PixelPosition& borderPixel : borderPixels)
		{
			inpaintingPixels.emplace_back(borderPixel, mask, sobel);
		}

		std::sort(inpaintingPixels.begin(), inpaintingPixels.end());
	}

	// let's ensure that no mask pixel is left

	for (unsigned int y = 0u; y < mask.height(); ++y)
	{
		for (unsigned int x = 0u; x < mask.width(); ++x)
		{
			if (mask.constpixel<uint8_t>(x, y)[0] != 0xFFu)
			{
				return false;
			}
		}
	}

	return true;
}

unsigned int TestInitializerI1::determineSSD(const Frame& frame, const Frame& mask, const unsigned int patchSize, const CV::PixelPosition& inpaintingPosition, const CV::PixelPosition& sourcePosition, const bool useMaskForSSD)
{
	if (useMaskForSSD)
	{
		constexpr uint8_t maskValue = 0x00u;

		return CV::Advanced::AdvancedSumSquareDifferencesBase::patchWithRejectingMask8BitPerChannel(frame.constdata<uint8_t>(), frame.constdata<uint8_t>(), mask.constdata<uint8_t>(), mask.constdata<uint8_t>(), frame.channels(), patchSize, frame.width(), frame.height(), frame.width(), frame.height(), inpaintingPosition.x(), inpaintingPosition.y(), sourcePosition.x(), sourcePosition.y(), frame.paddingElements(), frame.paddingElements(), mask.paddingElements(), mask.paddingElements(), maskValue).first;
	}

	return CV::SumSquareDifferencesBase::patchAtBorder8BitPerChannel(frame.constdata<uint8_t>(), frame.constdata<uint8_t>(), frame.channels(), patchSize, frame.width(), frame.height(), frame.width(), frame.height(), inpaintingPosition.x(), inpaintingPosition.y(), sourcePosition.x(), sourcePosition.y(), frame.paddingElements(), frame.paddingElements()).first;
}

bool TestInitializerI1::allValueSame(const Frame& mask, const unsigned int x, const unsigned int y, const uint8_t value, const unsigned int neighborhood)
{
	ocean_assert(mask.isPixelFormatCompatible(FrameType::FORMAT_Y8));
	ocean_assert(x < mask.width() && y < mask.height());

	if (neighborhood == 1u)
	{
		return mask.constpixel<uint8_t>(x, y)[0] == value;
	}

	if (neighborhood != 9u)
	{
		ocean_assert(false && "Invalid neighborhood!");
		return false;
	}

	for (int yy = int(y) - 1; yy <= int(y) + 1; ++yy)
	{
		if (yy < 0 || yy >= int(mask.height()))
		{
			continue;
		}

		for (int xx = int(x) - 1; xx <= int(x) + 1; ++xx)
		{
			if (xx < 0 || xx >= int(mask.width()))
			{
				continue;
			}

			if (mask.constpixel<uint8_t>((unsigned int)(xx), (unsigned int)(yy))[0] != value)
			{
				return false;
			}
		}
	}

	return true;
}

}

}

}

}
