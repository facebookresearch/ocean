/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testsynthesis/TestMappingF1.h"
#include "ocean/test/testcv/testsynthesis/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Utilities.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/cv/advanced/AdvancedSumSquareDifferences.h"

#include "ocean/cv/segmentation/MaskAnalyzer.h"

#include "ocean/math/Random.h"

#include "ocean/test/Validation.h"
#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestSynthesis
{

bool TestMappingF1::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "MappingF1 test:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testApplyMapping(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSumSquaredDifference5x5Mask(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAppearanceCost5x5(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAppearanceReferenceCost5x5(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSpatialCost4Neighborhood(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "MappingF1 test succeeded.";
	}
	else
	{
		Log::info() << "MappingF1 test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestMappingF1, ApplyMapping_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestMappingF1::testApplyMapping(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestMappingF1, ApplyMapping_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestMappingF1::testApplyMapping(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestMappingF1, ApplyMapping_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestMappingF1::testApplyMapping(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestMappingF1, ApplyMapping_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestMappingF1::testApplyMapping(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, 4u, GTEST_TEST_DURATION, worker));
}


TEST(TestMappingF1, SumSquaredDifference5x5Mask_1Channel)
{
	EXPECT_TRUE((TestMappingF1::testSumSquaredDifference5x5Mask<1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestMappingF1, SumSquaredDifference5x5Mask_2Channels)
{
	EXPECT_TRUE((TestMappingF1::testSumSquaredDifference5x5Mask<2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestMappingF1, SumSquaredDifference5x5Mask_3Channels)
{
	EXPECT_TRUE((TestMappingF1::testSumSquaredDifference5x5Mask<3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestMappingF1, SumSquaredDifference5x5Mask_4Channels)
{
	EXPECT_TRUE((TestMappingF1::testSumSquaredDifference5x5Mask<4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}


TEST(TestMappingF1, AppearanceCost5x5_1Channel)
{
	EXPECT_TRUE((TestMappingF1::testAppearanceCost5x5<1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestMappingF1, AppearanceCost5x5_2Channels)
{
	EXPECT_TRUE((TestMappingF1::testAppearanceCost5x5<2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestMappingF1, AppearanceCost5x5_3Channels)
{
	EXPECT_TRUE((TestMappingF1::testAppearanceCost5x5<3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestMappingF1, AppearanceCost5x5_4Channels)
{
	EXPECT_TRUE((TestMappingF1::testAppearanceCost5x5<4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}


TEST(TestMappingF1, AppearanceReferenceCost5x5_1Channel)
{
	EXPECT_TRUE((TestMappingF1::testAppearanceReferenceCost5x5<1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestMappingF1, AppearanceReferenceCost5x5_2Channels)
{
	EXPECT_TRUE((TestMappingF1::testAppearanceReferenceCost5x5<2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestMappingF1, AppearanceReferenceCost5x5_3Channels)
{
	EXPECT_TRUE((TestMappingF1::testAppearanceReferenceCost5x5<3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestMappingF1, AppearanceReferenceCost5x5_4Channels)
{
	EXPECT_TRUE((TestMappingF1::testAppearanceReferenceCost5x5<4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}


TEST(TestMappingF1, SpatialCost4Neighborhood_1Channel)
{
	EXPECT_TRUE((TestMappingF1::testSpatialCost4Neighborhood<1u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestMappingF1, SpatialCost4Neighborhood_2Channels)
{
	EXPECT_TRUE((TestMappingF1::testSpatialCost4Neighborhood<2u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestMappingF1, SpatialCost4Neighborhood_3Channels)
{
	EXPECT_TRUE((TestMappingF1::testSpatialCost4Neighborhood<3u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}

TEST(TestMappingF1, SpatialCost4Neighborhood_4Channels)
{
	EXPECT_TRUE((TestMappingF1::testSpatialCost4Neighborhood<4u>(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, GTEST_TEST_DURATION)));
}


TEST(TestMappingF1, TwoPixelPatchOneSubPixelPatch8BitPerChannel)
{
	EXPECT_TRUE(TestMappingF1::testTwoPixelPatchOneSubPixelPatch8BitPerChannel(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestMappingF1::testApplyMapping(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
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

bool TestMappingF1::testApplyMapping(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(channels >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << channels << " channels:";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

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
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(3u, width);
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(3u, height);

				Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

				const Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

				CV::Synthesis::MappingF1 mapping(frame.width(), frame.height());

				const Frame frameCopy(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				for (unsigned int y = 0u; y < mask.height(); ++y)
				{
					for (unsigned int x = 0u; x < mask.width(); ++x)
					{
						if (mask.constpixel<uint8_t>(x, y)[0] == 0x00)
						{
							Scalar sourceX, sourceY;

							while (true)
							{
								sourceX = Random::scalar(Scalar(0), Scalar(mask.width() - 1u));
								sourceY = Random::scalar(Scalar(0), Scalar(mask.height() - 1u));

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
					OCEAN_SET_FAILED(validation);
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Singlecore performance: " << performanceSinglecore;

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore performance: " << performanceMulticore;
		Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 2u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 2u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 2u) << "x";
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestMappingF1::testSumSquaredDifference5x5Mask(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 5x5 SSD with mask for " << width << "x" << height << ":";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testSumSquaredDifference5x5Mask<1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testSumSquaredDifference5x5Mask<2u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testSumSquaredDifference5x5Mask<3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testSumSquaredDifference5x5Mask<4u>(width, height, testDuration) && allSucceeded;

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
bool TestMappingF1::testSumSquaredDifference5x5Mask(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	constexpr size_t iterations = 1000;

	Log::info() << "... for " << tChannels << " channels:";

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performance;

	uint64_t totalExecutions = 0ull;
	uint64_t validExecutions = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool performanceIteration : {true, false})
		{
			const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 6u, width);
			const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 6u, height);

			const Frame frame0 = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
			const Frame frame1 = CV::CVUtilities::randomizedFrame(frame0.frameType(), &randomGenerator);

			const Frame mask0 = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

			CV::PixelPositions positions0;
			positions0.reserve(iterations);

			while (positions0.size() < iterations)
			{
				const unsigned int x = RandomI::random(randomGenerator, 2u, frame1.width() - 3u);
				const unsigned int y = RandomI::random(randomGenerator, 2u, frame1.height() - 3u);

				positions0.emplace_back(x, y);
			}

			Vectors2 positions1;
			positions1.reserve(iterations);

			while (positions1.size() < iterations)
			{
				const Scalar x = Random::scalar(randomGenerator, Scalar(2), Scalar(frame0.width() - 3u) - Numeric::weakEps());
				const Scalar y = Random::scalar(randomGenerator, Scalar(2), Scalar(frame0.height() - 3u) - Numeric::weakEps());

				positions1.emplace_back(x, y);
			}

			Indices32 results(iterations);

			constexpr unsigned int borderFactor = 27u;

			performance.startIf(performanceIteration);
				for (size_t n = 0; n < iterations; ++n)
				{
					const unsigned int result = CV::Synthesis::MappingF1::ssd5x5Mask<tChannels>(frame0.constdata<uint8_t>(), frame1.constdata<uint8_t>(), mask0.constdata<uint8_t>(), frame0.width(), frame0.height(), frame1.width(), frame1.height(), positions0[n].x(), positions0[n].y(), positions1[n].x(), positions1[n].y(), frame0.paddingElements(), frame1.paddingElements(), mask0.paddingElements(), borderFactor);

					results[n] = result;
				}
			performance.stopIf(performanceIteration);

			const size_t validResults = validateSumSquaredDifference5x5Mask(frame0, frame1, mask0, positions0, positions1, results, borderFactor);

			totalExecutions += uint64_t(results.size());
			validExecutions += uint64_t(validResults);
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 3u) << "ms, average: " << String::toAString(performance.averageMseconds(), 3u) << "ms";

	ocean_assert(totalExecutions != 0ull);
	ocean_assert(validExecutions <= totalExecutions);

	const double percent = double(validExecutions) / double(totalExecutions);

	const bool succeeded = percent >= 0.99;

	if (succeeded)
	{
		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return succeeded;
}

bool TestMappingF1::testAppearanceCost5x5(const unsigned int width, const unsigned int height, const double testDuration)
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
bool TestMappingF1::testAppearanceCost5x5(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	constexpr size_t iterations = 1000;

	Log::info() << "... for " << tChannels << " channels:";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(0.99, randomGenerator);

	constexpr unsigned int patchSize = 5u;
	constexpr unsigned int borderFactor = 5u;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int testWidth = RandomI::random(randomGenerator, 6u, width);
		const unsigned int testHeight = RandomI::random(randomGenerator, 6u, height);

		const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

		CV::Segmentation::MaskAnalyzer::determineDistancesToBorder8Bit(mask.data<uint8_t>(), mask.width(), mask.height(), mask.paddingElements(), patchSize + 1u, false /*assignFinal*/, CV::PixelBoundingBox());

		const CV::Synthesis::MappingF1 mapping(testWidth, testHeight);

		const unsigned int normalizationFactor = mapping.spatialCostNormalization<tChannels>();

		for (size_t n = 0; n < iterations; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			CV::PixelPosition target;
			Vector2 source;

			while (true)
			{
				target = CV::PixelPosition(RandomI::random(randomGenerator, 2u, frame.width() - 3u), RandomI::random(randomGenerator, 2u, frame.height() - 3u));

				if (mask.constpixel<uint8_t>(target.x(), target.y())[0] != 0xFFu)
				{
					break;
				}
			}

			while (true)
			{
				source = Random::vector2(randomGenerator, Scalar(2), Scalar(frame.width() - 3u) - Numeric::weakEps(), Scalar(2), Scalar(frame.height() - 3u) - Numeric::weakEps());

				const unsigned int xRounded = (unsigned int)(Numeric::round32(source.x()));
				const unsigned int yRounded = (unsigned int)(Numeric::round32(source.y()));

				if (mask.constpixel<uint8_t>(xRounded, yRounded)[0] == 0xFFu)
				{
					break;
				}
			}

			const unsigned int cost = mapping.appearanceCost5x5<tChannels>(target.x(), target.y(), source.x(), source.y(), frame.constdata<uint8_t>(), mask.constdata<uint8_t>(), frame.paddingElements(), mask.paddingElements(), borderFactor);

			const uint64_t testCost = determineAppearanceCost<true>(frame, frame, mask, source, target, patchSize, borderFactor, normalizationFactor);

			ocean_assert((std::is_same<float, Scalar>::value) || uint64_t(cost) == testCost);

			if (uint64_t(cost) != testCost)
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (validation.needMoreIterations() || startTimestamp + testDuration > Timestamp(true));

	Log::info() << validation.iterations();
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestMappingF1::testAppearanceReferenceCost5x5(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 5x5 appearance cost with reference frame for " << width << "x" << height << ":";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testAppearanceReferenceCost5x5<1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testAppearanceReferenceCost5x5<2u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testAppearanceReferenceCost5x5<3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testAppearanceReferenceCost5x5<4u>(width, height, testDuration) && allSucceeded;

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
bool TestMappingF1::testAppearanceReferenceCost5x5(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	constexpr size_t iterations = 1000;

	Log::info() << "... for " << tChannels << " channels:";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(0.99, randomGenerator);

	constexpr unsigned int patchSize = 5u;
	constexpr unsigned int borderFactor = 5u;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int testWidth = RandomI::random(randomGenerator, 6u, width);
		const unsigned int testHeight = RandomI::random(randomGenerator, 6u, height);

		const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
		const Frame reference = CV::CVUtilities::randomizedFrame(frame.frameType(), &randomGenerator);

		Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

		CV::Segmentation::MaskAnalyzer::determineDistancesToBorder8Bit(mask.data<uint8_t>(), mask.width(), mask.height(), mask.paddingElements(), patchSize + 1u, false /*assignFinal*/, CV::PixelBoundingBox());

		const CV::Synthesis::MappingF1 mapping(testWidth, testHeight);

		const unsigned int normalizationFactor = mapping.spatialCostNormalization<tChannels>();

		for (size_t n = 0; n < iterations; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			CV::PixelPosition target;
			Vector2 source;

			while (true)
			{
				target = CV::PixelPosition(RandomI::random(randomGenerator, 2u, frame.width() - 3u), RandomI::random(randomGenerator, 2u, frame.height() - 3u));

				if (mask.constpixel<uint8_t>(target.x(), target.y())[0] != 0xFFu)
				{
					break;
				}
			}

			while (true)
			{
				source = Random::vector2(randomGenerator, Scalar(2), Scalar(frame.width() - 3u) - Numeric::weakEps(), Scalar(2), Scalar(frame.height() - 3u) - Numeric::weakEps());

				const unsigned int xRounded = (unsigned int)(Numeric::round32(source.x()));
				const unsigned int yRounded = (unsigned int)(Numeric::round32(source.y()));

				if (mask.constpixel<uint8_t>(xRounded, yRounded)[0] == 0xFFu)
				{
					break;
				}
			}

			const unsigned int cost = mapping.appearanceReferenceCost5x5<tChannels>(target.x(), target.y(), source.x(), source.y(), frame.constdata<uint8_t>(), mask.constdata<uint8_t>(), reference.constdata<uint8_t>(), frame.paddingElements(), mask.paddingElements(), reference.paddingElements(), borderFactor);

			const uint64_t testCost = determineAppearanceReferenceCost(frame, reference, mask, source, target, patchSize, borderFactor, normalizationFactor);

			ocean_assert((std::is_same<float, Scalar>::value) || uint64_t(cost) == testCost);

			if (uint64_t(cost) != testCost)
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (validation.needMoreIterations() || startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestMappingF1::testSpatialCost4Neighborhood(const unsigned int width, const unsigned int height, const double testDuration)
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
bool TestMappingF1::testSpatialCost4Neighborhood(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channels!");

	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << tChannels << " channels:";

	constexpr Scalar threshold = std::is_same<double, Scalar>::value ? Scalar(0.00001) : Scalar(0.1);

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int testWidth = RandomI::random(randomGenerator, 3u, width);
		const unsigned int testHeight = RandomI::random(randomGenerator, 3u, height);

		const Frame mask = Utilities::randomizedInpaintingMask(testWidth, testHeight, 0x00u, randomGenerator);

		CV::Synthesis::MappingF1 mapping(testWidth, testHeight);

		const unsigned int normalizationFactor = mapping.appearanceCostNormalization<tChannels>();

		for (unsigned int y = 0u; y < mask.height(); ++y)
		{
			for (unsigned int x = 0u; x < mask.width(); ++x)
			{
				mapping.setPosition(x, y, Random::vector2(randomGenerator, Scalar(0), Scalar(mask.width() - 1u), Scalar(0), Scalar(mask.height() - 1u)));
			}
		}

		const Scalar maxCost = Random::scalar(randomGenerator, Scalar(0.001), Scalar(10000000));

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

		const Scalar cost = mapping.spatialCost4Neighborhood<tChannels>(xTarget, yTarget, Scalar(xSource), Scalar(ySource), mask.constdata<uint8_t>(), mask.paddingElements(), maxCost);

		const Scalar testCost = determineSpatialCost4Neighborhood(mapping, mask, Vector2(Scalar(xSource), Scalar(ySource)), CV::PixelPosition(xTarget, yTarget), maxCost, normalizationFactor);

		if (Numeric::isNotEqual(cost, testCost, threshold))
		{
			OCEAN_SET_FAILED(validation);
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestMappingF1::testTwoPixelPatchOneSubPixelPatch8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing SSD between two pixel accurate patches and one sub-pixel accurate patch:";
	Log::info() << " ";

	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;

	bool allSucceeded = true;

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<1u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<2u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<3u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<4u, 1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<1u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<2u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<3u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<4u, 3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<1u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<2u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<3u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<4u, 5u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<1u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<2u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<3u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<4u, 7u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<1u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<2u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<3u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<4u, 9u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<1u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<2u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<3u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<4u, 15u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<1u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<2u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<3u, 31u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testTwoPixelPatchOneSubPixelPatch8BitPerChannel<4u, 31u>(width, height, testDuration) && allSucceeded;

	return allSucceeded;
}

template <unsigned int tChannels, unsigned int tPatchSize>
bool TestMappingF1::testTwoPixelPatchOneSubPixelPatch8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize >= 1u && tPatchSize % 2u == 1u, "Invalid size");

	ocean_assert(width >= tPatchSize && height >= tPatchSize);
	ocean_assert(testDuration > 0.0);

	constexpr size_t locations = 10000;

	Log::info() << "... with " << tChannels << " channels and " << tPatchSize * tPatchSize << " pixels (" << tPatchSize << "x" << tPatchSize << ") at " << locations << " locations:";

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performanceTemplate;
	HighPerformanceStatistic performanceSSE;
	HighPerformanceStatistic performanceNEON;
	HighPerformanceStatistic performanceDefault;

	Indices32 centersX0(locations);
	Indices32 centersY0(locations);
	Indices32 centersX1(locations);
	Indices32 centersY1(locations);
	Scalars centersX2(locations);
	Scalars centersY2(locations);

	Indices32 resultsNaive;
	Indices32 resultsTemplate;
	Indices32 resultsSSE;
	Indices32 resultsNEON;
	Indices32 resultsDefault;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width0 = RandomI::random(randomGenerator, width - 1u, width + 1u);
		const unsigned int height0 = RandomI::random(randomGenerator, height - 1u, height + 1u);

		const unsigned int width1 = RandomI::random(randomGenerator, width - 1u, width + 1u);
		const unsigned int height1 = RandomI::random(randomGenerator, height - 1u, height + 1u);

		const unsigned int width2 = RandomI::random(randomGenerator, width - 1u, width + 1u);
		const unsigned int height2 = RandomI::random(randomGenerator, height - 1u, height + 1u);

		const Frame frame0 = CV::CVUtilities::randomizedFrame(FrameType(width0, height0, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
		const Frame frame1 = CV::CVUtilities::randomizedFrame(FrameType(width1, height1, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
		const Frame frame2 = CV::CVUtilities::randomizedFrame(FrameType(width2, height2, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		for (unsigned int n = 0u; n < locations; ++n)
		{
			centersX0[n] = RandomI::random(randomGenerator, tPatchSize_2, width0 - tPatchSize_2 - 1u);
			centersY0[n] = RandomI::random(randomGenerator, tPatchSize_2, height0 - tPatchSize_2 - 1u);

			centersX1[n] = Random::random(randomGenerator, tPatchSize_2, width1 - tPatchSize_2 - 1u);
			centersY1[n] = Random::random(randomGenerator, tPatchSize_2, height1 - tPatchSize_2 - 1u);

			centersX2[n] = Random::scalar(randomGenerator, Scalar(tPatchSize_2), Scalar(width2 - tPatchSize_2 - 2u) - Numeric::weakEps());
			centersY2[n] = Random::scalar(randomGenerator, Scalar(tPatchSize_2), Scalar(height2 - tPatchSize_2 - 2u) - Numeric::weakEps());
		}

		const unsigned int factor02 = RandomI::random(randomGenerator, 1u, 10u);
		const unsigned int factor12 = RandomI::random(randomGenerator, 1u, 10u);

		const uint8_t* const frameData0 = frame0.constdata<uint8_t>();
		const uint8_t* const frameData1 = frame1.constdata<uint8_t>();
		const uint8_t* const frameData2 = frame2.constdata<uint8_t>();

		const unsigned int framePaddingElements0 = frame0.paddingElements();
		const unsigned int framePaddingElements1 = frame1.paddingElements();
		const unsigned int framePaddingElements2 = frame2.paddingElements();

		for (const ImplementationType implementationType : {IT_NAIVE, IT_TEMPLATE, IT_SSE, IT_NEON, IT_DEFAULT})
		{
			switch (implementationType)
			{
				case IT_NAIVE:
				{
					resultsNaive.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceNaive);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsNaive[n] = CV::Advanced::AdvancedSumSquareDifferences::patch8BitPerChannel<tChannels, tPatchSize>(frameData0, frameData2, width0, width2, centersX0[n], centersY0[n], centersX2[n], centersY2[n], framePaddingElements0, framePaddingElements2) * factor02
											+ CV::Advanced::AdvancedSumSquareDifferences::patch8BitPerChannel<tChannels, tPatchSize>(frameData1, frameData2, width1, width2, centersX1[n], centersY1[n], centersX2[n], centersY2[n], framePaddingElements1, framePaddingElements2) * factor12;
					}

					break;
				}

				case IT_TEMPLATE:
				{
					resultsTemplate.resize(locations);

					for (size_t n = 0; n < locations; ++n)
					{
						// will also run SSE as we don't have a clear separation between non-SIMD code and SIMD code

						resultsTemplate[n] = MappingF1::sumSquareDifferencesThreePatch8BitPerChannel<tChannels, tPatchSize>(frameData0, frameData1, frameData2, width0, width1, width2, framePaddingElements0, framePaddingElements1, framePaddingElements2, factor02, factor12, centersX0[n], centersY0[n], centersX1[n], centersY1[n], centersX2[n], centersY2[n]);
					}

					break;
				}

				case IT_SSE:
				{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

					if constexpr (tChannels == 3 && tPatchSize == 5u)
					{
						resultsSSE.resize(locations);

						for (size_t n = 0; n < locations; ++n)
						{
							// will also run SSE as we don't have a clear separation between non-SIMD code and SIMD code

							resultsSSE[n] = MappingF1::sumSquareDifferencesThreePatch8BitPerChannel<tChannels, tPatchSize>(frameData0, frameData1, frameData2, width0, width1, width2, framePaddingElements0, framePaddingElements1, framePaddingElements2, factor02, factor12, centersX0[n], centersY0[n], centersX1[n], centersY1[n], centersX2[n], centersY2[n]);
						}
					}
#endif // defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

					break;
				}

				case IT_NEON:
				{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
					// not yet implemented
#endif // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

					break;
				}

				case IT_DEFAULT:
				{
					resultsDefault.resize(locations);

					const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceDefault);

					for (size_t n = 0; n < locations; ++n)
					{
						resultsDefault[n] = MappingF1::sumSquareDifferencesThreePatch8BitPerChannel<tChannels, tPatchSize>(frameData0, frameData1, frameData2, width0, width1, width2, framePaddingElements0, framePaddingElements1, framePaddingElements2, factor02, factor12, centersX0[n], centersY0[n], centersX1[n], centersY1[n], centersX2[n], centersY2[n]);
					}

					break;
				}

				default:
					ocean_assert(false && "Invalid implementation type!");
					break;
			}
		}

		for (size_t n = 0; n < locations; ++n)
		{
			const uint32_t ssdTest = CV::Advanced::AdvancedSumSquareDifferences::patch8BitPerChannel<tChannels, tPatchSize>(frameData0, frameData2, width0, width2, centersX0[n], centersY0[n], centersX2[n], centersY2[n], framePaddingElements0, framePaddingElements2) * factor02
											+ CV::Advanced::AdvancedSumSquareDifferences::patch8BitPerChannel<tChannels, tPatchSize>(frameData1, frameData2, width1, width2, centersX1[n], centersY1[n], centersX2[n], centersY2[n], framePaddingElements1, framePaddingElements2) * factor12;

			if (!resultsNaive.empty())
			{
				OCEAN_EXPECT_EQUAL(validation, resultsNaive[n], ssdTest);
			}

			if (!resultsTemplate.empty())
			{
				OCEAN_EXPECT_EQUAL(validation, resultsTemplate[n], ssdTest);
			}

			if (!resultsSSE.empty())
			{
				OCEAN_EXPECT_EQUAL(validation, resultsSSE[n], ssdTest);
			}

			if (!resultsNEON.empty())
			{
				OCEAN_EXPECT_EQUAL(validation, resultsNEON[n], ssdTest);
			}

			OCEAN_EXPECT_EQUAL(validation, resultsDefault[n], ssdTest);
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	static_assert(locations != 0, "Invalid number of locations!");

	if (performanceNaive.measurements() != 0u)
	{
		Log::info() << "   Naive: [" << performanceNaive.bestMseconds() << ", " << performanceNaive.medianMseconds() << ", " << performanceNaive.worstMseconds() << "] ms";
	}

	if (performanceTemplate.measurements() != 0u)
	{
		Log::info() << "Template: [" << performanceTemplate.bestMseconds() << ", " << performanceTemplate.medianMseconds() << ", " << performanceTemplate.worstMseconds() << "] ms";
	}

	if (performanceSSE.measurements() != 0u)
	{
		Log::info() << "     SSE: [" << performanceSSE.bestMseconds() << ", " << performanceSSE.medianMseconds() << ", " << performanceSSE.worstMseconds() << "] ms";
	}

	if (performanceNEON.measurements() != 0u)
	{
		Log::info() << "    NEON: [" << performanceNEON.bestMseconds() << ", " << performanceNEON.medianMseconds() << ", " << performanceNEON.worstMseconds() << "] ms";
	}

	ocean_assert(performanceDefault.measurements() != 0u);
	Log::info() << " Default: [" << performanceDefault.bestMseconds() << ", " << performanceDefault.medianMseconds() << ", " << performanceDefault.worstMseconds() << "] ms";

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestMappingF1::validateMapping(const Frame& frame, const Frame& mask, const CV::Synthesis::MappingF1& mapping, const CV::PixelBoundingBox& boundingBox)
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

	std::vector<uint8_t> interpolatedSourcePixel(16);

	const unsigned int channels = frame.channels();

	for (unsigned int y = boundingBox.top(); y < boundingBox.bottomEnd(); ++y)
	{
		for (unsigned int x = boundingBox.left(); x < boundingBox.rightEnd(); ++x)
		{
			const uint8_t maskPixel = mask.constpixel<uint8_t>(x, y)[0];

			if (maskPixel != 0xFF)
			{
				const Vector2 sourcePosition = mapping.position(x, y);

				switch (channels)
				{
					case 1u:
						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<1u, CV::PC_TOP_LEFT>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), sourcePosition, interpolatedSourcePixel.data());
						break;

					case 2u:
						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<2u, CV::PC_TOP_LEFT>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), sourcePosition, interpolatedSourcePixel.data());
						break;

					case 3u:
						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<3u, CV::PC_TOP_LEFT>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), sourcePosition, interpolatedSourcePixel.data());
						break;

					case 4u:
						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<4u, CV::PC_TOP_LEFT>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), sourcePosition, interpolatedSourcePixel.data());
						break;

					case 6u:
						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<6u, CV::PC_TOP_LEFT>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), sourcePosition, interpolatedSourcePixel.data());
						break;

					case 8u:
						CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<8u, CV::PC_TOP_LEFT>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), sourcePosition, interpolatedSourcePixel.data());
						break;

					default:
						ocean_assert(false && "Invalid channels!");
						return false;
				}

				const uint8_t* targetPixel = frame.constpixel<uint8_t>(x, y);

				if (memcmp(interpolatedSourcePixel.data(), targetPixel, sizeof(uint8_t) * channels) != 0)
				{
					return false;
				}
			}
		}
	}

	return true;
}

size_t TestMappingF1::validateSumSquaredDifference5x5Mask(const Frame& frame0, const Frame& frame1, const Frame& mask0, const CV::PixelPositions& positions0, const Vectors2& positions1, const Indices32& results, const unsigned int borderFactor)
{
	ocean_assert(frame0.isValid() && frame1.isValid() && mask0.isValid());
	ocean_assert(!positions0.empty());
	ocean_assert(positions0.size() == positions1.size());
	ocean_assert(positions0.size() == results.size());

	if (positions0.size() != results.size())
	{
		return false;
	}

	std::vector<uint8_t> interpolatedPixel1(frame1.channels());

	size_t correctResults = 0;

	for (size_t n = 0; n < positions0.size(); ++n)
	{
		const CV::PixelPosition& position0 = positions0[n];
		const Vector2& position1 = positions1[n];

		unsigned int ssdTest = 0u;

		for (int yy = -2; yy <= 2; ++yy)
		{
			for (int xx = -2; xx <= 2; ++xx)
			{
				const unsigned int xLocation0 = (unsigned int)(int(position0.x()) + xx);
				const unsigned int yLocation0 = (unsigned int)(int(position0.y()) + yy);

				const Scalar xLocation1 = position1.x() + Scalar(xx);
				const Scalar yLocation1 = position1.y() + Scalar(yy);

				ocean_assert(xLocation0 < frame0.width() && yLocation0 < frame0.height());

				ocean_assert(xLocation1 >= Scalar(0) && xLocation1 <= Scalar(frame1.width() - 1u));
				ocean_assert(yLocation1 >= Scalar(0) && yLocation1 <= Scalar(frame1.height() - 1u));

				CV::FrameInterpolatorBilinear::Comfort::interpolatePixel8BitPerChannel(frame1.constdata<uint8_t>(), frame1.channels(), frame1.width(), frame1.height(), frame1.paddingElements(), CV::PC_TOP_LEFT, Vector2(xLocation1, yLocation1), interpolatedPixel1.data());

				const uint8_t* framePixel0 = frame0.constpixel<uint8_t>(xLocation0, yLocation0);
				const uint8_t* maskPixel0 = mask0.constpixel<uint8_t>(xLocation0, yLocation0);

				unsigned int ssdLocal = 0u;

				for (unsigned int c = 0u; c < frame0.channels(); ++c)
				{
					ssdLocal += sqrDistance(framePixel0[c], interpolatedPixel1[c]);
				}

				if (maskPixel0[0] == 0xFFu)
				{
					ssdTest += ssdLocal * borderFactor;
				}
				else
				{
					ssdTest += ssdLocal;
				}
			}
		}

		ocean_assert((std::is_same<float, Scalar>::value) || ssdTest == results[n]); // with double precision, we expect a perfect result

		if (ssdTest == results[n])
		{
			++correctResults;
		}
	}

	return correctResults;
}

template <bool tPatchSizeNormalization>
uint64_t TestMappingF1::determineAppearanceCost(const Frame& sourceFrame, const Frame& targetFrame, const Frame& mask, const Vector2& source, const CV::PixelPosition& target, const unsigned int patchSize, const unsigned int borderFactor, const unsigned int normalizationFactor)
{
	ocean_assert(sourceFrame.isValid() && mask.isValid());
	ocean_assert(sourceFrame.numberPlanes() == 1u && sourceFrame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);
	ocean_assert(sourceFrame.isFrameTypeCompatible(FrameType(mask, sourceFrame.pixelFormat()), false));
	ocean_assert(sourceFrame.isFrameTypeCompatible(targetFrame, false));

	ocean_assert(source.x() >= Scalar(2) && source.x() < Scalar(sourceFrame.width() - 3u));
	ocean_assert(source.y() >= Scalar(2) && source.y() < Scalar(sourceFrame.height() - 3u));
	ocean_assert(target.x() < targetFrame.width() && target.y() < targetFrame.height());

	ocean_assert(patchSize >= 1u && patchSize % 2u == 1u);
	ocean_assert(borderFactor >= 1u);
	ocean_assert(normalizationFactor >= 1u);

	constexpr uint64_t invalidCost = uint64_t(-1);

	const unsigned int patchSize_2 = patchSize / 2u;

	const unsigned int xSourceRounded = (unsigned int)(Numeric::round32(source.x()));
	const unsigned int ySourceRounded = (unsigned int)(Numeric::round32(source.y()));

	if (mask.constpixel<uint8_t>(xSourceRounded, ySourceRounded)[0] != 0xFFu)
	{
		ocean_assert(false && "Invalid source location!");
		return invalidCost;
	}

	if (mask.constpixel<uint8_t>(target.x(), target.y())[0] == 0xFFu)
	{
		ocean_assert(false && "Invalid target location!");
		return invalidCost;
	}

	std::vector<uint8_t> interpolatedSourcePixel(sourceFrame.channels());

	uint64_t cost = 0ull;

	for (int yy = -int(patchSize_2); yy <= int(patchSize_2); ++yy)
	{
		const unsigned int yTarget = CV::CVUtilities::mirrorIndex(int(target.y()) + yy, targetFrame.height());

		for (int xx = -int(patchSize_2); xx <= int(patchSize_2); ++xx)
		{
			const unsigned int xTarget = CV::CVUtilities::mirrorIndex(int(target.x()) + xx, targetFrame.width());

			uint32_t ssd = 0u;

			if (!CV::FrameInterpolatorBilinear::Comfort::interpolatePixel8BitPerChannel(sourceFrame.constdata<uint8_t>(), sourceFrame.channels(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), CV::PC_TOP_LEFT, source + Vector2(Scalar(xx), Scalar(yy)), interpolatedSourcePixel.data()))
			{
				return 0ull;
			}

			const uint8_t* const targetPixel = targetFrame.constpixel<uint8_t>(xTarget, yTarget);

			for (unsigned int n = 0u; n < targetFrame.channels(); ++n)
			{
				ssd += uint32_t(NumericT<int32_t>::sqr(int32_t(interpolatedSourcePixel[n]) - int32_t(targetPixel[n])));
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

	if constexpr (tPatchSizeNormalization)
	{
		cost *= uint64_t(normalizationFactor);

		cost /= uint64_t(patchSize * patchSize);
	}

	return cost;
}

uint64_t TestMappingF1::determineAppearanceReferenceCost(const Frame& frame, const Frame& reference, const Frame& mask, const Vector2& source, const CV::PixelPosition& target, const unsigned int patchSize, const unsigned int borderFactor, const unsigned int normalizationFactor)
{
	ocean_assert(frame.isValid() && reference.isValid() && mask.isValid());
	ocean_assert(frame.numberPlanes() == 1u && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);
	ocean_assert(frame.isFrameTypeCompatible(reference.frameType(), false));
	ocean_assert(frame.isFrameTypeCompatible(FrameType(mask, frame.pixelFormat()), false));

	ocean_assert(source.x() >= Scalar(2) && source.x() < Scalar(frame.width() - 3u));
	ocean_assert(source.y() >= Scalar(2) && source.y() < Scalar(frame.height() - 3u));
	ocean_assert(target.x() < frame.width() && target.y() < frame.height());

	ocean_assert(patchSize >= 1u && patchSize % 2u == 1u);
	ocean_assert(borderFactor >= 1u);
	ocean_assert(normalizationFactor >= 1u);

	const uint64_t notNormalizedFrameFrame = determineAppearanceCost<false>(frame, frame, mask, source, target, patchSize, borderFactor, 1ull);
	const uint64_t notNOrmalizedReferenceFrame = determineAppearanceCost<false>(frame, reference, mask, source, target, patchSize, borderFactor, 1ull);

	uint64_t cost = notNormalizedFrameFrame * 1ull + notNOrmalizedReferenceFrame * 5ull;

	cost *= uint64_t(normalizationFactor);

	cost /= uint64_t(patchSize * patchSize);

	return cost;
}

Scalar TestMappingF1::determineSpatialCost4Neighborhood(const CV::Synthesis::MappingF1& mapping, const Frame& mask, const Vector2& source, const CV::PixelPosition& target, const Scalar maxCost, const unsigned int normalizationFactor)
{
	ocean_assert(mapping);
	ocean_assert(mask.isValid());

	if (mask.constpixel<uint8_t>(target.x(), target.y())[0] == 0xFFu)
	{
		ocean_assert(false && "This should never happen!");
		return maxCost;
	}

	Scalar bestCost = Numeric::maxValue();

	for (const CV::PixelDirection pixelDirection : {CV::PD_NORTH, CV::PD_EAST, CV::PD_SOUTH, CV::PD_WEST})
	{
		const CV::PixelPosition targetNeighbor = target.neighbor(pixelDirection);
		const Vector2 sourceNeighbor = neighborPixel(source, pixelDirection);

		if (targetNeighbor.x() < mask.width() && targetNeighbor.y() < mask.height())
		{
			if (mask.constpixel<uint8_t>(targetNeighbor.x(), targetNeighbor.y())[0] != 0xFFu)
			{
				const Vector2& mappingPosition = mapping.position(targetNeighbor.x(), targetNeighbor.y());

				const Scalar cost = mappingPosition.sqrDistance(sourceNeighbor);

				bestCost = std::min(bestCost, cost);
			}
		}
	}

	return std::min(bestCost * Scalar(normalizationFactor), maxCost);
}

Vector2 TestMappingF1::neighborPixel(const Vector2& position, const CV::PixelDirection pixelDirection)
{
	switch (pixelDirection)
	{
		case CV::PD_NORTH:
			return position - Vector2(0, 1);

		case CV::PD_EAST:
			return position + Vector2(1, 0);

		case CV::PD_SOUTH:
			return position + Vector2(0, 1);

		case CV::PD_WEST:
			return position - Vector2(1, 0);

		default:
			break;
	}

	ocean_assert(false && "Invalid pixel direction!");
	return position;
}

}

}

}

}
