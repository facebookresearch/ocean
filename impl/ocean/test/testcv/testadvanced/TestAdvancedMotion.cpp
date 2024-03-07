// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testcv/testadvanced/TestAdvancedMotion.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterGaussian.h"

#include "ocean/cv/advanced/AdvancedMotion.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestAdvanced
{

bool TestAdvancedMotion::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Advanced motion test:   ---";
	Log::info() << " ";

	allSucceeded = testTrackPointsBidirectionalSubPixelMirroredBorder(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = stressTestTrackPointsBidirectionalSubPixelMirroredBorder(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Advanced motion test succeeded.";
	}
	else
	{
		Log::info() << "Advanced motion testFAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestAdvancedMotion, TrackPointsBidirectionalSubPixelMirroredBorder_AdvancedMotionSSD_1Channel)
{
	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;
	Worker worker;

	EXPECT_TRUE((TestAdvancedMotion::testTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionSSD, 1u>(width, height, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedMotion, TrackPointsBidirectionalSubPixelMirroredBorder_AdvancedMotionSSD_2Channels)
{
	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;
	Worker worker;

	EXPECT_TRUE((TestAdvancedMotion::testTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionSSD, 2u>(width, height, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedMotion, TrackPointsBidirectionalSubPixelMirroredBorder_AdvancedMotionSSD_3Channels)
{
	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;
	Worker worker;

	EXPECT_TRUE((TestAdvancedMotion::testTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionSSD, 3u>(width, height, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedMotion, TrackPointsBidirectionalSubPixelMirroredBorder_AdvancedMotionSSD_4Channels)
{
	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;
	Worker worker;

	EXPECT_TRUE((TestAdvancedMotion::testTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionSSD, 4u>(width, height, GTEST_TEST_DURATION, worker)));
}


TEST(TestAdvancedMotion, TrackPointsBidirectionalSubPixelMirroredBorder_AdvancedMotionZeroMeanSSD_1Channel)
{
	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;
	Worker worker;

	EXPECT_TRUE((TestAdvancedMotion::testTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionZeroMeanSSD, 1u>(width, height, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedMotion, TrackPointsBidirectionalSubPixelMirroredBorder_AdvancedMotionZeroMeanSSD_2Channels)
{
	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;
	Worker worker;

	EXPECT_TRUE((TestAdvancedMotion::testTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionZeroMeanSSD, 2u>(width, height, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedMotion, TrackPointsBidirectionalSubPixelMirroredBorder_AdvancedMotionZeroMeanSSD_3Channels)
{
	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;
	Worker worker;

	EXPECT_TRUE((TestAdvancedMotion::testTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionZeroMeanSSD, 3u>(width, height, GTEST_TEST_DURATION, worker)));
}

TEST(TestAdvancedMotion, TrackPointsBidirectionalSubPixelMirroredBorder_AdvancedMotionZeroMeanSSD_4Channels)
{
	constexpr unsigned int width = 1280u;
	constexpr unsigned int height = 720u;
	Worker worker;

	EXPECT_TRUE((TestAdvancedMotion::testTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionZeroMeanSSD, 4u>(width, height, GTEST_TEST_DURATION, worker)));
}


TEST(TestAdvancedMotion, StressTestTrackPointsBidirectionalSubPixelMirroredBorder_AdvancedMotionSSD_5)
{
	RandomGenerator randomGenerator;
	Worker worker;

	EXPECT_TRUE((TestAdvancedMotion::stressTestTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionSSD, 5u>(randomGenerator, worker)));
}

TEST(TestAdvancedMotion, StressTestTrackPointsBidirectionalSubPixelMirroredBorder_AdvancedMotionSSD_7)
{
	RandomGenerator randomGenerator;
	Worker worker;

	EXPECT_TRUE((TestAdvancedMotion::stressTestTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionSSD, 7u>(randomGenerator, worker)));
}

TEST(TestAdvancedMotion, StressTestTrackPointsBidirectionalSubPixelMirroredBorder_AdvancedMotionSSD_15)
{
	RandomGenerator randomGenerator;
	Worker worker;

	EXPECT_TRUE((TestAdvancedMotion::stressTestTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionSSD, 15u>(randomGenerator, worker)));
}

TEST(TestAdvancedMotion, StressTestTrackPointsBidirectionalSubPixelMirroredBorder_AdvancedMotionSSD_31)
{
	RandomGenerator randomGenerator;
	Worker worker;

	EXPECT_TRUE((TestAdvancedMotion::stressTestTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionSSD, 31u>(randomGenerator, worker)));
}


TEST(TestAdvancedMotion, StressTestTrackPointsBidirectionalSubPixelMirroredBorder_AdvancedMotionZeroMeanSSD_5)
{
	RandomGenerator randomGenerator;
	Worker worker;

	EXPECT_TRUE((TestAdvancedMotion::stressTestTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionZeroMeanSSD, 5u>(randomGenerator, worker)));
}

TEST(TestAdvancedMotion, StressTestTrackPointsBidirectionalSubPixelMirroredBorder_AdvancedMotionZeroMeanSSD_7)
{
	RandomGenerator randomGenerator;
	Worker worker;

	EXPECT_TRUE((TestAdvancedMotion::stressTestTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionZeroMeanSSD, 7u>(randomGenerator, worker)));
}

TEST(TestAdvancedMotion, StressTestTrackPointsBidirectionalSubPixelMirroredBorder_AdvancedMotionZeroMeanSSD_15)
{
	RandomGenerator randomGenerator;
	Worker worker;

	EXPECT_TRUE((TestAdvancedMotion::stressTestTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionZeroMeanSSD, 15u>(randomGenerator, worker)));
}

TEST(TestAdvancedMotion, StressTestTrackPointsBidirectionalSubPixelMirroredBorder_AdvancedMotionZeroMeanSSD_31)
{
	RandomGenerator randomGenerator;
	Worker worker;

	EXPECT_TRUE((TestAdvancedMotion::stressTestTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionZeroMeanSSD, 31u>(randomGenerator, worker)));
}

#endif // OCEAN_USE_GTEST


bool TestAdvancedMotion::testTrackPointsBidirectionalSubPixelMirroredBorder(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test for trackPointsBidirectionalSubPixelMirroredBorder()";
	Log::info() << " ";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	Timestamp startTimestamp(true);

	do
	{
		if (!testTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionSSD>(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}

		Log::info() << " ";
		Log::info() << " ";

		if (!testTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionZeroMeanSSD>(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}

		Log::info() << " ";
		Log::info() << " ";
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: Succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestAdvancedMotion::testTrackPointsBidirectionalSubPixelMirroredBorder(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert(std::is_same<T, CV::Advanced::AdvancedMotionSSD>::value || std::is_same<T, CV::Advanced::AdvancedMotionZeroMeanSSD>::value, "Invalid metric!");

	if constexpr (std::is_same<T, CV::Advanced::AdvancedMotionSSD>::value)
	{
		Log::info() << "Using AdvancedMotionSSD with resolution " << width << "x" << height << ":";
	}
	else
	{
		Log::info() << "Using AdvancedMotionZeroMeanSSD with resolution " << width << "x" << height << ":";
	}

	Log::info() << " ";

	bool allSucceeded = true;

	if (!testTrackPointsBidirectionalSubPixelMirroredBorder<T, 1u>(width, height, testDuration, worker))
	{
		allSucceeded = false;
	}

	if (!testTrackPointsBidirectionalSubPixelMirroredBorder<T, 2u>(width, height, testDuration, worker))
	{
		allSucceeded = false;
	}

	if (!testTrackPointsBidirectionalSubPixelMirroredBorder<T, 3u>(width, height, testDuration, worker))
	{
		allSucceeded = false;
	}

	if (!testTrackPointsBidirectionalSubPixelMirroredBorder<T, 4u>(width, height, testDuration, worker))
	{
		allSucceeded = false;
	}

	return allSucceeded;
}

template <typename T, unsigned int tChannels>
bool TestAdvancedMotion::testTrackPointsBidirectionalSubPixelMirroredBorder(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(std::is_same<T, CV::Advanced::AdvancedMotionSSD>::value || std::is_same<T, CV::Advanced::AdvancedMotionZeroMeanSSD>::value, "Invalid metric!");

	Log::info() << "... with " << tChannels << " channels:";
	Log::info() << " ";

	bool allSucceeded = true;

	if (!testTrackPointsBidirectionalSubPixelMirroredBorder<T, tChannels, 5u>(width, height, testDuration, worker))
	{
		allSucceeded = false;
	}

	Log::info() << " ";

	if (!testTrackPointsBidirectionalSubPixelMirroredBorder<T, tChannels, 7u>(width, height, testDuration, worker))
	{
		allSucceeded = false;
	}

	Log::info() << " ";

	if (!testTrackPointsBidirectionalSubPixelMirroredBorder<T, tChannels, 15u>(width, height, testDuration, worker))
	{
		allSucceeded = false;
	}

	Log::info() << " ";

	if (!testTrackPointsBidirectionalSubPixelMirroredBorder<T, tChannels, 31u>(width, height, testDuration, worker))
	{
		allSucceeded = false;
	}

	Log::info() << " ";

	return allSucceeded;
}

template <typename T, unsigned int tChannels, unsigned int tPatchSize>
bool TestAdvancedMotion::testTrackPointsBidirectionalSubPixelMirroredBorder(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	Log::info() << "... with patch size " << tPatchSize;

	constexpr unsigned int numberPoints = 1000u;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	std::vector<double> percentsTrackedPoints;
	std::vector<double> percentsAccuratePoints;

	for (const bool performanceIteration : {true, false})
	{
		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			const Timestamp startTimestamp(true);

			do
			{
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, tPatchSize * 2u, width);
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, tPatchSize * 2u, height);

				const FrameType frameTypeA(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT);

				const Frame frameA = createRandomTrackableFrame(frameTypeA, randomGenerator);

				constexpr unsigned int maxBorderSize = 50u;

				const unsigned int borderLeft = RandomI::random(randomGenerator, maxBorderSize);
				const unsigned int borderTop = RandomI::random(randomGenerator, maxBorderSize);
				const unsigned int borderRight = RandomI::random(randomGenerator, maxBorderSize);
				const unsigned int borderBottom = RandomI::random(randomGenerator, maxBorderSize);

				const FrameType frameTypeB(frameTypeA, testWidth + borderLeft + borderRight, testHeight + borderTop + borderBottom);

				Frame frameB = createRandomTrackableFrame(frameTypeB, randomGenerator); // making a second frame B which is larger than frameA, but the inner core is identical to A
				frameB.copy(int(borderLeft), int(borderTop), frameA);

				Vectors2 pointsA;
				pointsA.reserve(numberPoints);

				for (unsigned int n = 0u; n < numberPoints; ++n)
				{
					pointsA.emplace_back(Random::vector2(randomGenerator, Scalar(0), Scalar(frameA.width()) - Numeric::eps(), Scalar(0), Scalar(frameA.height()) - Numeric::eps()));
				}

				constexpr unsigned int baseline = maxBorderSize * 2u;
				constexpr unsigned int coarsestLayerRadius = 8u;

				Indices32 indices;
				Indices32* useIndices = RandomI::random(randomGenerator, 1u) == 0u ? &indices : nullptr;

				Vectors2 pointsB;

				performance.startIf(performanceIteration);
					const bool trackResult = T::template trackPointsBidirectionalSubPixelMirroredBorder<tPatchSize>(frameA, frameB, baseline, coarsestLayerRadius, pointsA, pointsB, Scalar(0.9 * 0.9), CV::FramePyramid::DM_FILTER_11, useWorker, useIndices);
				performance.stopIf(performanceIteration);

				if (trackResult)
				{
					const Vector2 frameB_t_frameA = Vector2(Scalar(borderLeft), Scalar(borderTop));

					if (pointsA.size() == pointsB.size())
					{
						if (performanceIteration)
						{
							double percentTrackedPoints = NumericD::maxValue();
							double percentAccuratePoints = NumericD::maxValue();

							if (useIndices != nullptr)
							{
								percentTrackedPoints = double(useIndices->size()) / double(numberPoints);

								unsigned int accuratePoints = 0u;

								for (const Index32 validIndex : *useIndices)
								{
									const Vector2 expectedB = frameB_t_frameA + pointsA[validIndex];

									const Scalar distance = pointsB[validIndex].distance(expectedB);

									if (distance < Scalar(1))
									{
										++accuratePoints;
									}
								}

								percentAccuratePoints = double(accuratePoints) / double(useIndices->size());
							}
							else
							{
								percentTrackedPoints = double(pointsA.size()) / double(numberPoints);

								unsigned int accuratePoints = 0u;

								for (size_t n = 0; n < pointsA.size(); ++n)
								{
									const Vector2 expectedB = frameB_t_frameA + pointsA[n];

									const Scalar distance = pointsB[n].distance(expectedB);

									if (distance < Scalar(1))
									{
										++accuratePoints;
									}
								}

								percentAccuratePoints = double(accuratePoints) / double(pointsA.size());
							}

							ocean_assert(percentTrackedPoints != NumericD::maxValue());
							ocean_assert(percentAccuratePoints != NumericD::maxValue());

							percentsTrackedPoints.emplace_back(percentTrackedPoints);
							percentsAccuratePoints.emplace_back(percentAccuratePoints);
						}
					}
					else
					{
						allSucceeded = false;
					}
				}
				else
				{
					allSucceeded = false;
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}
	}

	Log::info() << "Single-core performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multi-core performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms";
		Log::info() << "Multi-core boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
	}

	std::sort(percentsTrackedPoints.begin(), percentsTrackedPoints.end());
	std::sort(percentsAccuratePoints.begin(), percentsAccuratePoints.end());

	const double minPercentTrackedPoints = percentsTrackedPoints.front();
	const double medianPercentTrackedPoints = percentsTrackedPoints[percentsTrackedPoints.size() / 2];
	const double maxPercentTrackedPoints = percentsTrackedPoints.back();

	const double minPercentAccuratePoints = percentsAccuratePoints.front();
	const double medianPercentAccuratePoints = percentsAccuratePoints[percentsAccuratePoints.size() / 2];
	const double maxPercentAccuratePoints = percentsAccuratePoints.back();

	Log::info() << "Tracked point range: [" << String::toAString(minPercentTrackedPoints * 100.0, 1u) << "%, " << String::toAString(medianPercentTrackedPoints * 100.0, 1u) << "%, " << String::toAString(maxPercentTrackedPoints * 100.0, 1u) << "%]";
	Log::info() << "Accurate point range: [" << String::toAString(minPercentAccuratePoints * 100.0, 1u) << "%, " << String::toAString(medianPercentAccuratePoints * 100.0, 1u) << "%, " << String::toAString(maxPercentAccuratePoints * 100.0, 1u) << "%]";


	ocean_assert(0.0 <= minPercentTrackedPoints && minPercentTrackedPoints <= 1.0);
	ocean_assert(0.0 <= minPercentAccuratePoints && minPercentAccuratePoints <= 1.0);

	if constexpr (tPatchSize >= 7u)
	{
		if (minPercentTrackedPoints < 0.80 || minPercentAccuratePoints < 0.90)
		{
			allSucceeded = false;
		}
	}

	if constexpr (tPatchSize >= 15u)
	{
		if (minPercentTrackedPoints < 0.90 || minPercentAccuratePoints < 0.95)
		{
			allSucceeded = false;
		}
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

bool TestAdvancedMotion::stressTestTrackPointsBidirectionalSubPixelMirroredBorder(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Stress test for trackPointsBidirectionalSubPixelMirroredBorder()";
	Log::info() << " ";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	Timestamp startTimestamp(true);

	do
	{
		if (!stressTestTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionSSD>(randomGenerator, worker))
		{
			allSucceeded = false;
		}

		if (!stressTestTrackPointsBidirectionalSubPixelMirroredBorder<CV::Advanced::AdvancedMotionZeroMeanSSD>(randomGenerator, worker))
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: Succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestAdvancedMotion::stressTestTrackPointsBidirectionalSubPixelMirroredBorder(RandomGenerator& randomGenerator, Worker& worker)
{
	bool allSucceeded = true;

	if (!stressTestTrackPointsBidirectionalSubPixelMirroredBorder<T, 5u>(randomGenerator, worker))
	{
		allSucceeded = false;
	}

	if (!stressTestTrackPointsBidirectionalSubPixelMirroredBorder<T, 7u>(randomGenerator, worker))
	{
		allSucceeded = false;
	}

	if (!stressTestTrackPointsBidirectionalSubPixelMirroredBorder<T, 15u>(randomGenerator, worker))
	{
		allSucceeded = false;
	}

	if (!stressTestTrackPointsBidirectionalSubPixelMirroredBorder<T, 31u>(randomGenerator, worker))
	{
		allSucceeded = false;
	}

	return allSucceeded;
}

template <typename T, unsigned int tPatchSize>
bool TestAdvancedMotion::stressTestTrackPointsBidirectionalSubPixelMirroredBorder(RandomGenerator& randomGenerator, Worker& worker)
{
	static_assert(tPatchSize >= 1u && tPatchSize % 2u == 1u, "Invalid patch size!");

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;

		const unsigned int channels = RandomI::random(randomGenerator, 1u, 4u);

		const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<uint8_t>(channels);

		const unsigned int sourceWidth = RandomI::random(tPatchSize * 2u, 2000u);
		const unsigned int sourceHeight = RandomI::random(tPatchSize * 2u, 2000u);

		const unsigned int targetWidth = RandomI::random(tPatchSize * 2u, 2000u);
		const unsigned int targetHeight = RandomI::random(tPatchSize * 2u, 2000u);

		const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceWidth, sourceHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), false, &randomGenerator);
		const Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(targetWidth, targetHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), false, &randomGenerator);

		Indices32 indices;
		Indices32* useIndices = RandomI::random(randomGenerator, 1u) == 0u ? &indices : nullptr;

		const unsigned int baseline = RandomI::random(randomGenerator, 1u, std::min(sourceWidth / 4u, sourceHeight / 4u));
		const unsigned int coarsestLayerRadius = RandomI::random(randomGenerator, 2u, tPatchSize * 2u);

		const unsigned int numberFeatures = RandomI::random(randomGenerator, 1u, 200u);

		Vectors2 sourceFeatures;
		sourceFeatures.reserve(numberFeatures);

		for (unsigned int n = 0u; n < numberFeatures; ++n)
		{
			sourceFeatures.emplace_back(Random::vector2(randomGenerator, Scalar(0), Scalar(sourceWidth - 1u), Scalar(0), Scalar(sourceHeight - 1u)));
		}

		Vectors2 targetFeatures;
		if (!T::template trackPointsBidirectionalSubPixelMirroredBorder<tPatchSize>(sourceFrame, targetFrame, baseline, coarsestLayerRadius, sourceFeatures, targetFeatures, Scalar(0.9 * 0.9), CV::FramePyramid::DM_FILTER_11, useWorker, useIndices))
		{
			return false;
		}
	}

	return true;
}

Frame TestAdvancedMotion::createRandomTrackableFrame(const FrameType& frameType, RandomGenerator& randomGenerator)
{
	ocean_assert(frameType.isValid() && frameType.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	Frame frame = CV::CVUtilities::randomizedFrame(frameType, false, &randomGenerator);

	const unsigned int minDimension = std::min(frameType.width(), frameType.height());

	const unsigned int maxDimentionEllipse = std::max(1u, minmax(3u, minDimension * 5u / 100u, 50u));

	const unsigned int maxDimentionRectangle = std::max(1u, minmax(3u, minDimension * 5u / 100u, 50u));
	const unsigned int maxDimentionRectangle_2 = maxDimentionRectangle / 2u;

	const unsigned int numberEllipses = std::max(1u, frameType.pixels() / 1000u);
	const unsigned int numberRectangles = std::max(1u, frameType.pixels() / 1000u);
	const unsigned int numberPoints = std::max(1u, frameType.pixels() / 100u);
	const unsigned int numberSalt = std::max(1u, frameType.pixels() / 50u);

	std::vector<uint8_t> pixelColor(frameType.channels());

	for (unsigned int n = 0u; n < numberEllipses; ++n)
	{
		const unsigned int x = RandomI::random(randomGenerator, frameType.width() - 1u);
		const unsigned int y = RandomI::random(randomGenerator, frameType.height() - 1u);

		const unsigned int width = RandomI::random(randomGenerator, 3u, maxDimentionEllipse) | 0x01u; // make it odd
		const unsigned int height = RandomI::random(randomGenerator, 3u, maxDimentionEllipse) | 0x01u;

		for (uint8_t& value : pixelColor)
		{
			value = uint8_t(RandomI::random(randomGenerator, 255u));
		}

		CV::Canvas::ellipse(frame, CV::PixelPosition(x, y), width, height, pixelColor.data());
	}

	for (unsigned int n = 0u; n < numberRectangles; ++n)
	{
		const int left = RandomI::random(randomGenerator, -int(maxDimentionRectangle_2), int(frameType.width() - maxDimentionRectangle_2));
		const int top = RandomI::random(randomGenerator, -int(maxDimentionRectangle_2), int(frameType.height() - maxDimentionRectangle_2));

		const unsigned int width = RandomI::random(randomGenerator, 1u, maxDimentionRectangle);
		const unsigned int height = RandomI::random(randomGenerator, 1u, maxDimentionRectangle);

		for (uint8_t& value : pixelColor)
		{
			value = uint8_t(RandomI::random(randomGenerator, 255u));
		}

		CV::Canvas::rectangle(frame, left, top, width, height, pixelColor.data());
	}

	for (unsigned int n = 0u; n < numberPoints; ++n)
	{
		const Scalar x = Random::scalar(randomGenerator, Scalar(0), Scalar(frameType.width()));
		const Scalar y = Random::scalar(randomGenerator, Scalar(0), Scalar(frameType.width()));

		for (uint8_t& value : pixelColor)
		{
			value = uint8_t(RandomI::random(randomGenerator, 255u));
		}

		switch (RandomI::random(randomGenerator, 3u))
		{
			case 0u:
				CV::Canvas::point<1u>(frame, Vector2(x, y), pixelColor.data());
				break;

			case 1u:
				CV::Canvas::point<3u>(frame, Vector2(x, y), pixelColor.data());
				break;

			case 2u:
				CV::Canvas::point<5u>(frame, Vector2(x, y), pixelColor.data());
				break;

			case 3u:
				CV::Canvas::point<7u>(frame, Vector2(x, y), pixelColor.data());
				break;

			default:
				ocean_assert(false && "This should never happen!");
		}
	}

	bool gaussianResult = CV::FrameFilterGaussian::filter(frame, 3u, WorkerPool::get().scopedWorker()());
	ocean_assert_and_suppress_unused(gaussianResult, gaussianResult);

	for (unsigned int n = 0u; n < numberSalt; ++n)
	{
		const unsigned int x = RandomI::random(randomGenerator, frameType.width() - 1u);
		const unsigned int y = RandomI::random(randomGenerator, frameType.height() - 1u);

		for (uint8_t& value : pixelColor)
		{
			value = uint8_t(RandomI::random(randomGenerator, 255u));
		}

		memcpy(frame.pixel<uint8_t>(x, y), pixelColor.data(), sizeof(uint8_t) * pixelColor.size());
	}

	return frame;
}

}

}

}

}
