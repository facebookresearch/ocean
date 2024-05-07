// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/test/testcv/testdetector/TestBlobFeatureDetector.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Utilities.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolatorNearestPixel.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/cv/detector/blob/BlobFeatureDescriptor.h"

#include "ocean/math/Random.h"
#include "ocean/math/SquareMatrix3.h"

#include "ocean/tracking/blob/UnidirectionalCorrespondences.h"

namespace Ocean
{

using namespace CV::Detector::Blob;

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

bool TestBlobFeatureDetector::test(const Frame& frame, const double testDuration, Worker& worker)
{
	ocean_assert(frame && frame.width() >= 195u && frame.height() >= 195u && testDuration > 0.0);

	Log::info() << "---   Blob detector test:   ---";
	Log::info() << " ";

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
	{
		ocean_assert(false && "This must never happen!");
		Log::info() << "Blob detector test FAILED!";

		return false;
	}

	const Scalar threshold = Scalar(65);
	bool allSucceeded = true;

	Frame linedIntegralFrame(FrameType(yFrame.width() + 1u, yFrame.height() + 1u, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT));
	CV::IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(yFrame.constdata<uint8_t>(), linedIntegralFrame.data<uint32_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), linedIntegralFrame.paddingElements());
	const uint32_t* const linedIntegral = linedIntegralFrame.constdata<uint32_t>();

	BlobFeatures testFeatures;
	BlobFeatureDetector::detectFeatures(linedIntegral, yFrame.width(), yFrame.height(), BlobFeatureDetector::SAMPLING_NORMAL, threshold, true, testFeatures);

	if (testFeatures.empty())
	{
		Log::info() << "No features provided, thus we rate this test as FAILED!";
		return false;
	}

	allSucceeded = testResponseMaps(yFrame, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFeatureDetectionPerformance(testDuration, worker, yFrame) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMaximumSuppression(testDuration, worker, yFrame) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testOrientationCalculationPerformance(testDuration, worker, yFrame) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDescriptorCalculationPerformance(testDuration, worker, yFrame) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testOrientationAndDescriptorCalculation(linedIntegral, yFrame.width(), yFrame.height(), testFeatures, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDescriptorMatching(linedIntegral, yFrame.width(), yFrame.height(), testFeatures, Scalar(0.1), testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSSDPerformance(testFeatures.size(), Scalar(0.1), testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testUnidirectionalCorrespondences(linedIntegral, yFrame.width(), yFrame.height(), testFeatures, Scalar(0.1), testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testOverallPerformance(yFrame, threshold, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Blob detector test succeeded.";
	}
	else
	{
		Log::info() << "Blob detector test FAILED!";
	}

	return true;
}

#ifdef OCEAN_USE_GTEST

TEST(TestBlobFeatureDetector, ResponseMap_1)
{
	Worker worker;
	EXPECT_TRUE(TestBlobFeatureDetector::testResponseMap(1u, GTEST_TEST_DURATION, worker));
}

TEST(TestBlobFeatureDetector, ResponseMap_2)
{
	Worker worker;
	EXPECT_TRUE(TestBlobFeatureDetector::testResponseMap(2u, GTEST_TEST_DURATION, worker));
}

TEST(TestBlobFeatureDetector, ResponseMap_3)
{
	Worker worker;
	EXPECT_TRUE(TestBlobFeatureDetector::testResponseMap(3u, GTEST_TEST_DURATION, worker));
}

TEST(TestBlobFeatureDetector, ResponseMap_4)
{
	Worker worker;
	EXPECT_TRUE(TestBlobFeatureDetector::testResponseMap(4u, GTEST_TEST_DURATION, worker));
}

TEST(TestBlobFeatureDetector, ResponseMap_6)
{
	Worker worker;
	EXPECT_TRUE(TestBlobFeatureDetector::testResponseMap(6u, GTEST_TEST_DURATION, worker));
}

TEST(TestBlobFeatureDetector, ResponseMap_8)
{
	Worker worker;
	EXPECT_TRUE(TestBlobFeatureDetector::testResponseMap(8u, GTEST_TEST_DURATION, worker));
}

TEST(TestBlobFeatureDetector, ResponseMap_12)
{
	Worker worker;
	EXPECT_TRUE(TestBlobFeatureDetector::testResponseMap(12u, GTEST_TEST_DURATION, worker));
}

TEST(TestBlobFeatureDetector, ResponseMap_16)
{
	Worker worker;
	EXPECT_TRUE(TestBlobFeatureDetector::testResponseMap(16u, GTEST_TEST_DURATION, worker));
}

TEST(TestBlobFeatureDetector, ResponseMap_24)
{
	Worker worker;
	EXPECT_TRUE(TestBlobFeatureDetector::testResponseMap(24u, GTEST_TEST_DURATION, worker));
}

TEST(TestBlobFeatureDetector, ResponseMap_32)
{
	Worker worker;
	EXPECT_TRUE(TestBlobFeatureDetector::testResponseMap(32u, GTEST_TEST_DURATION, worker));
}

TEST(TestBlobFeatureDetector, FeatureDetectionPerformance)
{
	Worker worker;
	EXPECT_TRUE(TestBlobFeatureDetector::testFeatureDetectionPerformance(GTEST_TEST_DURATION, worker));
}

TEST(TestBlobFeatureDetector, MaximumSuppressionPerformance)
{
	Worker worker;
	EXPECT_TRUE(TestBlobFeatureDetector::testMaximumSuppression(GTEST_TEST_DURATION, worker));
}

TEST(TestBlobFeatureDetector, OrientationCalculationPerformance)
{
	Worker worker;
	EXPECT_TRUE(TestBlobFeatureDetector::testOrientationCalculationPerformance(GTEST_TEST_DURATION, worker));
}

TEST(TestBlobFeatureDetector, DescriptorCalculationPerformance)
{
	Worker worker;
	EXPECT_TRUE(TestBlobFeatureDetector::testDescriptorCalculationPerformance(GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestBlobFeatureDetector::testResponseMaps(const Frame& yFrame, const double testDuration, Worker& worker)
{
	Log::info() << "Testing Blob response maps:";
	Log::info() << " ";

	bool allSucceeded = true;

	for (const Index32 filterIndex : {1u, 2u, 3u, 4u, 6u, 8u, 12u, 16u, 24u, 32u})
	{
		allSucceeded = testResponseMap(filterIndex, testDuration, worker, yFrame) && allSucceeded;

		Log::info() << " ";
	}

	return allSucceeded;
}

bool TestBlobFeatureDetector::testResponseMap(const unsigned int filterIndex, const double testDuration, Worker& worker, const Frame& yFrameTest)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing Blob response map " << filterIndex;

	typedef std::unordered_map<Index32, Index32> IndexPairMap;

	const IndexPairMap samplingStepMap =
	{
		{1u, 2u},
		{2u, 2u},
		{3u, 2u},
		{4u, 2u},
		{6u, 4u},
		{8u, 4u},
		{12u, 8u},
		{16u, 8u},
		{24u, 16u},
		{32u, 16u}
	};

	const IndexPairMap explicitOffsetMap =
	{
		{1u, 1u},
		{2u, 0u},
		{3u, 1u},
		{4u, 0u},
		{6u, 0u},
		{8u, 2u},
		{12u, 2u},
		{16u, 6u},
		{24u, 6u},
		{32u, 14u}
	};

	const IndexPairMap::const_iterator iSamplingStep = samplingStepMap.find(filterIndex);
	const IndexPairMap::const_iterator iExplicitOffset = explicitOffsetMap.find(filterIndex);

	if (iSamplingStep == samplingStepMap.cend() || iExplicitOffset == explicitOffsetMap.cend())
	{
		ocean_assert(false && "This must never happen!");
		return false;
	}

	const unsigned int samplingStep = iSamplingStep->second;
	const unsigned int explicitOffset = iExplicitOffset->second;

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (const bool performanceIteration : {true, false})
	{
		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			const Timestamp startTimestamp(true);

			do
			{
				Frame yFrame;

				if (performanceIteration)
				{
					if (yFrameTest.isValid())
					{
						yFrame = Frame(yFrameTest, Frame::ACM_USE_KEEP_LAYOUT);
					}
					else
					{
						constexpr unsigned int width = 800u;
						constexpr unsigned int height = 640u;

						const unsigned int randomPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(1u);

						yFrame = Frame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), randomPaddingElements);
						CV::CVUtilities::randomizeFrame(yFrame, false, &randomGenerator);
					}
				}
				else
				{
					const unsigned int minSize = (9u + (filterIndex - 1u) * 6u) * 2u;

					const unsigned int randomWidth = RandomI::random(randomGenerator, minSize, 1920u);
					const unsigned int randomHeight = RandomI::random(randomGenerator, minSize, 1080u);
					const unsigned int randomPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(1u);

					yFrame = Frame(FrameType(randomWidth, randomHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), randomPaddingElements);
					CV::CVUtilities::randomizeFrame(yFrame, false, &randomGenerator);
				}

				const Frame linedIntegralFrame = CV::IntegralImage::Comfort::createLinedImage(yFrame);

				BlobFeatureDetector::ResponseMap responseMap(yFrame.width(), yFrame.height(), samplingStep, filterIndex, explicitOffset);

				if (!responseMap)
				{
					ocean_assert(false && "This should never happen!");
					allSucceeded = false;
					continue;
				}

				ocean_assert(linedIntegralFrame.isContinuous());

				performance.startIf(performanceIteration);
				responseMap.filter(linedIntegralFrame.constdata<uint32_t>(), useWorker);
				performance.stopIf(performanceIteration);

				if (!validateResponseMap(yFrame, responseMap))
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

bool TestBlobFeatureDetector::testFeatureDetectionPerformance(const double testDuration, Worker& worker, const Frame& yFrameTest)
{
	Log::info() << "Test performance of feature detection of all layers:";
	Log::info() << " ";

	RandomGenerator randomGenerator;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (const bool useSubRegion : {false, true})
	{
		if (useSubRegion)
		{
			Log::info() << " ";
			Log::info() << "Detection in large sub-region:";
		}

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		for (const bool performanceIteration : {true, false})
		{
			for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
			{
				Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
				HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

				const Timestamp startTimestamp(true);

				do
				{
					Frame yFrame;
					Scalar threshold = detectionThresholdWeak_;

					if (performanceIteration)
					{
						if (yFrameTest.isValid())
						{
							yFrame = Frame(yFrameTest, Frame::ACM_USE_KEEP_LAYOUT);
							threshold = detectionThresholdStrong_;
						}
						else
						{
							constexpr unsigned int width = 800u;
							constexpr unsigned int height = 640u;

							const unsigned int randomPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(1u);

							yFrame = Frame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), randomPaddingElements);
							CV::CVUtilities::randomizeFrame(yFrame, false, &randomGenerator);
						}
					}
					else
					{
						const unsigned int randomWidth = RandomI::random(randomGenerator, 3u, 1920u);
						const unsigned int randomHeight = RandomI::random(randomGenerator, 3u, 1080u);
						const unsigned int randomPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(1u);

						yFrame = Frame(FrameType(randomWidth, randomHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), randomPaddingElements);
						CV::CVUtilities::randomizeFrame(yFrame, false, &randomGenerator);
					}

					const Frame linedIntegralFrame = CV::IntegralImage::Comfort::createLinedImage(yFrame);

					performance.startIf(performanceIteration);
						BlobFeatures features;

						if (useSubRegion)
						{
							BlobFeatureDetector::detectFeatures(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), 1u, 1u, yFrame.width() - 2u, yFrame.height() - 2u, BlobFeatureDetector::SAMPLING_NORMAL, threshold, true, features, useWorker);
						}
						else
						{
							BlobFeatureDetector::detectFeatures(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), BlobFeatureDetector::SAMPLING_NORMAL, threshold, true, features, useWorker);
						}
					performance.stopIf(performanceIteration);
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

		Log::info() << " ";
	}

	return true;
}

bool TestBlobFeatureDetector::testMaximumSuppression(const double testDuration, Worker& worker, const Frame& yFrameTest)
{
	Log::info() << "Testing maximum suppression:";
	Log::info() << " " ;

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (const bool performanceIteration : {true, false})
	{
		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			const Timestamp startTimestamp(true);

			do
			{
				Frame yFrame;
				Scalar threshold = detectionThresholdWeak_;

				if (performanceIteration)
				{
					if (yFrameTest.isValid())
					{
						yFrame = Frame(yFrameTest, Frame::ACM_USE_KEEP_LAYOUT);
						threshold = detectionThresholdStrong_;
					}
					else
					{
						constexpr unsigned int width = 800u;
						constexpr unsigned int height = 640u;

						const unsigned int randomPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

						yFrame = Frame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), randomPaddingElements);
						CV::CVUtilities::randomizeFrame(yFrame, false, &randomGenerator);
					}
				}
				else
				{
					const unsigned int randomWidth = RandomI::random(randomGenerator, 800u, 1920u);
					const unsigned int randomHeight = RandomI::random(randomGenerator, 640u, 1080u);
					const unsigned int randomPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

					yFrame = Frame(FrameType(randomWidth, randomHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), randomPaddingElements);
					CV::CVUtilities::randomizeFrame(yFrame, false, &randomGenerator);
				}

				const Frame linedIntegralFrame = CV::IntegralImage::Comfort::createLinedImage(yFrame);

				std::vector<BlobFeatureDetector::ResponseMap> responseMaps =
				{
					BlobFeatureDetector::ResponseMap(yFrame.width(), yFrame.height(), 2u, 1u, 1u),
					BlobFeatureDetector::ResponseMap(yFrame.width(), yFrame.height(), 2u, 2u, 0u),
					BlobFeatureDetector::ResponseMap(yFrame.width(), yFrame.height(), 2u, 3u, 1u),
					BlobFeatureDetector::ResponseMap(yFrame.width(), yFrame.height(), 2u, 4u, 0u),

					BlobFeatureDetector::ResponseMap(yFrame.width(), yFrame.height(), 4u, 6u, 0u),
					BlobFeatureDetector::ResponseMap(yFrame.width(), yFrame.height(), 4u, 8u, 2u),

					BlobFeatureDetector::ResponseMap(yFrame.width(), yFrame.height(), 8u, 12u, 2u),
					BlobFeatureDetector::ResponseMap(yFrame.width(), yFrame.height(), 8u, 16u, 6u),

					BlobFeatureDetector::ResponseMap(yFrame.width(), yFrame.height(), 16u, 24u, 6u),
					BlobFeatureDetector::ResponseMap(yFrame.width(), yFrame.height(), 16u, 32u, 14u)
				};

				for (BlobFeatureDetector::ResponseMap& responseMap : responseMaps)
				{
					ocean_assert(linedIntegralFrame.isContinuous());
					responseMap.filter(linedIntegralFrame.constdata<uint32_t>());
				}

				performance.startIf(performanceIteration);

					std::vector<BlobFeatures> features(8);

					BlobFeatureDetector::findExtremes(responseMaps[0], responseMaps[1], responseMaps[2], threshold, true, features[0], useWorker);
					BlobFeatureDetector::findExtremes(responseMaps[1], responseMaps[2], responseMaps[3], threshold, true, features[1], useWorker);

					BlobFeatureDetector::findExtremes(responseMaps[1], responseMaps[3], responseMaps[4], threshold, true, features[2], useWorker);
					BlobFeatureDetector::findExtremes(responseMaps[3], responseMaps[4], responseMaps[5], threshold, true, features[3], useWorker);

					BlobFeatureDetector::findExtremes(responseMaps[3], responseMaps[5], responseMaps[6], threshold, true, features[4], useWorker);
					BlobFeatureDetector::findExtremes(responseMaps[5], responseMaps[6], responseMaps[7], threshold, true, features[5], useWorker);

					BlobFeatureDetector::findExtremes(responseMaps[5], responseMaps[7], responseMaps[8], threshold, true, features[6], useWorker);
					BlobFeatureDetector::findExtremes(responseMaps[7], responseMaps[8], responseMaps[9], threshold, true, features[7], useWorker);

				performance.stopIf(performanceIteration);

				if (!validateMaximumSuppression(responseMaps[0], responseMaps[1], responseMaps[2], threshold, features[0])
									|| !validateMaximumSuppression(responseMaps[1], responseMaps[2], responseMaps[3], threshold, features[1])
									|| !validateMaximumSuppression(responseMaps[1], responseMaps[3], responseMaps[4], threshold, features[2])
									|| !validateMaximumSuppression(responseMaps[3], responseMaps[4], responseMaps[5], threshold, features[3])
									|| !validateMaximumSuppression(responseMaps[3], responseMaps[5], responseMaps[6], threshold, features[4])
									|| !validateMaximumSuppression(responseMaps[5], responseMaps[6], responseMaps[7], threshold, features[5])
									|| !validateMaximumSuppression(responseMaps[5], responseMaps[7], responseMaps[8], threshold, features[6])
									|| !validateMaximumSuppression(responseMaps[7], responseMaps[8], responseMaps[9], threshold, features[7]))
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

bool TestBlobFeatureDetector::testOrientationCalculationPerformance(const double testDuration, Worker& worker, const Frame& yFrameTest)
{
	Frame yFrame;
	Scalar threshold = detectionThresholdStrong_;

	if (yFrameTest.isValid())
	{
		yFrame = Frame(yFrameTest, Frame::ACM_USE_KEEP_LAYOUT);
	}
	else
	{
		constexpr unsigned int width = 800u;
		constexpr unsigned int height = 640u;

		const unsigned int randomPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

		yFrame = Frame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), randomPaddingElements);
		CV::CVUtilities::randomizeFrame(yFrame, false);

		threshold = detectionThresholdWeak_;
	}

	const Frame linedIntegralFrame = CV::IntegralImage::Comfort::createLinedImage(yFrame);

	BlobFeatures testFeatures;
	BlobFeatureDetector::detectFeatures(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), BlobFeatureDetector::SAMPLING_NORMAL, threshold, true, testFeatures);

	Log::info() << "Test performance of orientation of " << testFeatures.size() << " features:";

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (const BlobFeature::OrientationType orientationType : {BlobFeature::ORIENTATION_SLIDING_WINDOW, BlobFeature::ORIENTATION_SUMMED_ORIENTATION})
	{
		Log::info() << " ";

		if (orientationType == BlobFeature::ORIENTATION_SLIDING_WINDOW)
		{
			Log::info() << "... with sliding window orientation:";
		}
		else
		{
			Log::info() << "... with summed orientation:";
		}

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			const Timestamp startTimestamp(true);

			do
			{
				BlobFeatures features(testFeatures);

				performance.start();
				ocean_assert(linedIntegralFrame.isContinuous());
				BlobFeatureDescriptor::calculateOrientations(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), FrameType::ORIGIN_UPPER_LEFT, orientationType, features, true, useWorker);
				performance.stop();
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		Log::info() << "Single-core performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multi-core performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms";
			Log::info() << "Multi-core boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
		}
	}

	Log::info() << " ";

	return true;
}

bool TestBlobFeatureDetector::testDescriptorCalculationPerformance(const double testDuration, Worker& worker, const Frame& yFrameTest)
{
	Frame yFrame;
	Scalar threshold = detectionThresholdStrong_;

	if (yFrameTest.isValid())
	{
		yFrame = Frame(yFrameTest, Frame::ACM_USE_KEEP_LAYOUT);
	}
	else
	{
		constexpr unsigned int width = 800u;
		constexpr unsigned int height = 640u;

		const unsigned int randomPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

		yFrame = Frame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), randomPaddingElements);
		CV::CVUtilities::randomizeFrame(yFrame, false);

		threshold = detectionThresholdWeak_;
	}

	const Frame linedIntegralFrame = CV::IntegralImage::Comfort::createLinedImage(yFrame);

	BlobFeatures testFeatures;
	BlobFeatureDetector::detectFeatures(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), BlobFeatureDetector::SAMPLING_NORMAL, threshold, true, testFeatures);

	Log::info() << "Test performance of description of " << testFeatures.size() << " features with " << BlobDescriptor::elements() << " elements:";

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (const bool oriented : {true, false})
	{
		Log::info() << " ";

		if (oriented)
		{
			Log::info() << "... oriented descriptor:";
		}
		else
		{
			Log::info() << "... not oriented descriptor:";
		}

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			const Timestamp startTimestamp(true);

			do
			{
				BlobFeatures features(testFeatures);

				performance.start();
				ocean_assert(linedIntegralFrame.isContinuous());

				if (oriented)
				{
					BlobFeatureDescriptor::calculateDescriptors(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), FrameType::ORIGIN_UPPER_LEFT, features, true, useWorker);
				}
				else
				{
					BlobFeatureDescriptor::calculateNotOrientedDescriptors(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), FrameType::ORIGIN_UPPER_LEFT, features, true, useWorker);
				}

				performance.stop();
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		Log::info() << "Single-core performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multi-core performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms";
			Log::info() << "Multi-core boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
		}
	}

	Log::info() << " ";

	return true;
}

bool TestBlobFeatureDetector::testOrientationAndDescriptorCalculation(const uint32_t* linedIntegral, const unsigned int width, const unsigned int height, const BlobFeatures& testFeatures, const double testDuration, Worker& worker)
{
	Log::info() << "Test orientation and description of " << testFeatures.size() << " features with " << BlobDescriptor::elements() << " elements:\n";
	Log::info() << "... with sliding window orientation:";

	BlobFeatures features(testFeatures);

	HighPerformanceStatistic performance;
	Timestamp startTimestamp(true);

	do
	{
		performance.start();
		for (BlobFeatures::iterator i = features.begin(); i != features.end(); ++i)
		{
			BlobFeatureDescriptor::calculateOrientation(linedIntegral, width, height, FrameType::ORIGIN_UPPER_LEFT, BlobFeature::ORIENTATION_SLIDING_WINDOW, *i, true);
			BlobFeatureDescriptor::calculateDescriptor(linedIntegral, width, height, FrameType::ORIGIN_UPPER_LEFT, *i, true);
		}
		performance.stop();
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: Best: " << performance.bestMseconds() << "ms, worst: " << performance.worstMseconds() << "ms, average: " << performance.averageMseconds() << "ms";

	if (worker)
	{
		Log::info() << "... with sliding window orientation (multicore):";

		HighPerformanceStatistic performanceMulticore;
		const Timestamp startTimestampMulticore(true);

		do
		{
			performanceMulticore.start();
			BlobFeatureDescriptor::calculateOrientationsAndDescriptors(linedIntegral, width, height, FrameType::ORIGIN_UPPER_LEFT, BlobFeature::ORIENTATION_SLIDING_WINDOW, features, true, &worker);
			performanceMulticore.stop();
		}
		while (startTimestampMulticore + testDuration > Timestamp(true));

		Log::info() << "Multicore performance: Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms";
		Log::info() << "Multicore boost: Best: " << String::toAString(performance.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performance.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performance.average() / performanceMulticore.average(), 1u) << "x";
	}

	Log::info() << " ";
	Log::info() << "... with summed orientation:";

	performance.reset();
	startTimestamp.toNow();

	do
	{
		performance.start();
		for (BlobFeatures::iterator i = features.begin(); i != features.end(); ++i)
		{
			BlobFeatureDescriptor::calculateOrientation(linedIntegral, width, height, FrameType::ORIGIN_UPPER_LEFT, BlobFeature::ORIENTATION_SUMMED_ORIENTATION, *i, true);
			BlobFeatureDescriptor::calculateDescriptor(linedIntegral, width, height, FrameType::ORIGIN_UPPER_LEFT, *i, true);
		}
		performance.stop();
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: Best: " << performance.bestMseconds() << "ms, worst: " << performance.worstMseconds() << "ms, average: " << performance.averageMseconds() << "ms";

	if (worker)
	{
		Log::info() << "... with summed orientation (multicore):";

		HighPerformanceStatistic performanceMulticore;
		const Timestamp startTimestampMulticore(true);

		do
		{
			performanceMulticore.start();
			BlobFeatureDescriptor::calculateOrientationsAndDescriptors(linedIntegral, width, height, FrameType::ORIGIN_UPPER_LEFT, BlobFeature::ORIENTATION_SUMMED_ORIENTATION, features, true, &worker);
			performanceMulticore.stop();
		}
		while (startTimestampMulticore + testDuration > Timestamp(true));

		Log::info() << "Multicore performance: Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms";
		Log::info() << "Multicore boost: Best: " << String::toAString(performance.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performance.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performance.average() / performanceMulticore.average(), 1u) << "x";
	}

	Log::info() << " ";
	Log::info() << "... without orientation (unoriented):";

	performance.reset();
	startTimestamp.toNow();

	do
	{
		performance.start();
		for (BlobFeatures::iterator i = features.begin(); i != features.end(); ++i)
			BlobFeatureDescriptor::calculateNotOrientedDescriptor(linedIntegral, width, height, FrameType::ORIGIN_UPPER_LEFT, *i, true);
		performance.stop();
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: Best: " << performance.bestMseconds() << "ms, worst: " << performance.worstMseconds() << "ms, average: " << performance.averageMseconds() << "ms";

	if (worker)
	{
		Log::info() << "... without orientation (unoriented) (multicore):";

		HighPerformanceStatistic performanceMulticore;
		const Timestamp startTimestampMulticore(true);

		do
		{
			performanceMulticore.start();
			BlobFeatureDescriptor::calculateNotOrientedDescriptors(linedIntegral, width, height, FrameType::ORIGIN_UPPER_LEFT, features, true, &worker);
			performanceMulticore.stop();
		}
		while (startTimestampMulticore + testDuration > Timestamp(true));

		Log::info() << "Multicore performance: Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms";
		Log::info() << "Multicore boost: Best: " << String::toAString(performance.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performance.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performance.average() / performanceMulticore.average(), 1u) << "x";
	}

	return true;
}

bool TestBlobFeatureDetector::testDescriptorMatching(const uint32_t* linedIntegral, const unsigned int width, const unsigned int height, const BlobFeatures& testFeatures, const Scalar threshold, const double testDuration, Worker& worker)
{
	ocean_assert(!testFeatures.empty() && threshold >= 0);

#ifdef OCEAN_CV_DETECTOR_BLOB_USE_INTEGER_DESCRIPTOR
	Log::info() << "Test descriptor matching of " << testFeatures.size() << " features with " << BlobDescriptor::elements() << " integer elements:";
#else
	Log::info() << "Test descriptor matching of " << testFeatures.size() << " features with " << BlobDescriptor::elements() << " float elements:";
#endif

	Log::info() << " ";

	BlobFeatures features(testFeatures);

	for (BlobFeatures::iterator i = features.begin(); i != features.end(); ++i)
	{
		BlobFeatureDescriptor::calculateOrientation(linedIntegral, width, height, FrameType::ORIGIN_UPPER_LEFT, BlobFeature::ORIENTATION_SLIDING_WINDOW, *i, true);
		BlobFeatureDescriptor::calculateDescriptor(linedIntegral, width, height, FrameType::ORIGIN_UPPER_LEFT, *i, true);
	}

	BlobFeatures referenceFeatures(features.rbegin(), features.rend());

	const BlobDescriptor::DescriptorSSD normalizedThreshold = BlobDescriptor::DescriptorSSD(threshold * BlobDescriptor::descriptorNormalizationSqr());

#ifdef OCEAN_CV_DETECTOR_BLOB_USE_INTEGER_DESCRIPTOR
	ocean_assert(normalizedThreshold > threshold);
#else
	ocean_assert(Numeric::isEqual(normalizedThreshold, threshold, Scalar(0.0001)));
#endif

	bool allSucceeded = true;

	for (unsigned int earlyRejectLoop = 0u; earlyRejectLoop <= 1u; ++earlyRejectLoop)
	{
		if (earlyRejectLoop == 0u)
			Log::info() << "... with exact matching:";
		else
			Log::info() << "... with early reject matching:";

		unsigned long long validIterations = 0ull;
		unsigned long long iterations = 0ull;

		HighPerformanceStatistic performance;
		Timestamp startTimestamp(true);

		do
		{
			std::vector<unsigned int> bestMatchingReferenceFeatures(features.size());
			std::vector<BlobDescriptor::DescriptorSSD> bestSSDs(features.size());

			performance.start();

			if (earlyRejectLoop == 0u)
				matchDescriptorsSubset<false>(features.data(), referenceFeatures.data(), referenceFeatures.size(), normalizedThreshold, bestMatchingReferenceFeatures.data(), bestSSDs.data(), 0u, (unsigned int)features.size());
			else
				matchDescriptorsSubset<true>(features.data(), referenceFeatures.data(), referenceFeatures.size(), normalizedThreshold, bestMatchingReferenceFeatures.data(), bestSSDs.data(), 0u, (unsigned int)features.size());

			performance.stop();

			for (size_t n = 0; n < features.size(); ++n)
			{
				const unsigned int bestMatchingReferenceFeature = bestMatchingReferenceFeatures[n];

				if (bestMatchingReferenceFeature == referenceFeatures.size() - n - 1)
					if (NumericT<BlobDescriptor::DescriptorSSD>::isEqual(bestSSDs[n], summedSquaredDifferences(referenceFeatures[bestMatchingReferenceFeature].descriptor()(), features[n].descriptor()(), features[n].descriptor().elements())))
						++validIterations;

				iterations++;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);
		const double percent = double(validIterations) / double(iterations);

		Log::info() << "Performance: Best: " << performance.bestMseconds() << "ms, worst: " << performance.worstMseconds() << "ms, average: " << performance.averageMseconds() << "ms";
		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

		allSucceeded = percent >= 0.98 && allSucceeded;

		if (worker)
		{
			Log::info() << " ";

			if (earlyRejectLoop == 0u)
				Log::info() << "... with exact matching (multicore):";
			else
				Log::info() << "... with early reject matching (multicore):";

			validIterations = 0ull;
			iterations = 0ull;

			HighPerformanceStatistic performanceMulticore;
			const Timestamp startTimestampMulticore(true);

			do
			{
				std::vector<unsigned int> bestMatchingReferenceFeatures(features.size());
				std::vector<BlobDescriptor::DescriptorSSD> bestSSDs(features.size());

				performanceMulticore.start();

				if (earlyRejectLoop == 0u)
					worker.executeFunction(Worker::Function::createStatic(&TestBlobFeatureDetector::matchDescriptorsSubset<false>, (const BlobFeature*)features.data(), (const BlobFeature*)referenceFeatures.data(), referenceFeatures.size(), normalizedThreshold, bestMatchingReferenceFeatures.data(), bestSSDs.data(), 0u, 0u), 0u, (unsigned int)features.size());
				else
					worker.executeFunction(Worker::Function::createStatic(&TestBlobFeatureDetector::matchDescriptorsSubset<true>, (const BlobFeature*)features.data(), (const BlobFeature*)referenceFeatures.data(), referenceFeatures.size(), normalizedThreshold, bestMatchingReferenceFeatures.data(), bestSSDs.data(), 0u, 0u), 0u, (unsigned int)features.size());

				performanceMulticore.stop();

				for (size_t n = 0; n < features.size(); ++n)
				{
					const unsigned int bestMatchingReferenceFeature = bestMatchingReferenceFeatures[n];

					if (bestMatchingReferenceFeature == referenceFeatures.size() - n - 1)
						if (NumericT<BlobDescriptor::DescriptorSSD>::isEqual(bestSSDs[n], summedSquaredDifferences(referenceFeatures[bestMatchingReferenceFeature].descriptor()(), features[n].descriptor()(), features[n].descriptor().elements())))
							++validIterations;

					iterations++;
				}
			}
			while (startTimestampMulticore + testDuration > Timestamp(true));

			ocean_assert(iterations != 0ull);
			const double percentMulticore = double(validIterations) / double(iterations);

			Log::info() << "Multicore performance: Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performance.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performance.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performance.average() / performanceMulticore.average(), 1u) << "x";
			Log::info() << "Validation: " << String::toAString(percentMulticore * 100.0, 1u) << "% succeeded.";

			allSucceeded = percentMulticore >= 0.98 && allSucceeded;
		}

		Log::info() << " ";
	}

	{
		// we make an additional test validating the SSD results

		RandomGenerator randomGenerator;

		unsigned long long validSSDs = 0ull;
		unsigned long long validEarlyRejectSSDs = 0ull;

		for (size_t n = 0; n < features.size(); ++n)
		{
			BlobDescriptor descriptorA, descriptorB;
			double doubleDescriptorA[BlobDescriptor::elements()];
			double doubleDescriptorB[BlobDescriptor::elements()];

			for (unsigned int i = 0u; i < BlobDescriptor::elements(); ++i)
			{
				doubleDescriptorA[i] = RandomD::scalar(randomGenerator, -1, 1);
				doubleDescriptorB[i] = RandomD::scalar(randomGenerator, -1, 1);
			}

			const double lengthA = NumericD::sqrt(NumericD::dot(doubleDescriptorA, doubleDescriptorA, BlobDescriptor::elements()));
			const double lengthB = NumericD::sqrt(NumericD::dot(doubleDescriptorB, doubleDescriptorB, BlobDescriptor::elements()));

			for (unsigned int i = 0u; i < BlobDescriptor::elements(); ++i)
			{
				descriptorA()[i] = BlobDescriptor::DescriptorElement(double(doubleDescriptorA[i]) / lengthA * double(BlobDescriptor::descriptorNormalization()));
				descriptorB()[i] = BlobDescriptor::DescriptorElement(double(doubleDescriptorB[i]) / lengthB * double(BlobDescriptor::descriptorNormalization()));
			}

			const BlobDescriptor::DescriptorSSD ssd = descriptorA.ssd(descriptorB);

			BlobDescriptor::DescriptorSSD ssdIsEqual;
			const bool isEqual = descriptorA.isDescriptorEqual(descriptorB, normalizedThreshold, ssdIsEqual);

			if ((ssd <= normalizedThreshold) == isEqual && NumericT<BlobDescriptor::DescriptorSSD>::isWeakEqual(ssd, ssdIsEqual))
				++validSSDs;

			BlobDescriptor::DescriptorSSD ssdIsEqualEarlyReject;
			const bool isEqualEarlyReject = descriptorA.isDescriptorEqual(descriptorB, normalizedThreshold, ssdIsEqualEarlyReject);

			if ((ssd <= normalizedThreshold) == isEqualEarlyReject && NumericT<BlobDescriptor::DescriptorSSD>::isWeakEqual(ssd, ssdIsEqualEarlyReject))
				++validEarlyRejectSSDs;
		}

		const double normalPercent = double(validSSDs) / double(features.size());
		const double earlyRejectPercent = double(validEarlyRejectSSDs) / double(features.size());

		Log::info() << "Normal SSD validation: " << String::toAString(normalPercent * 100.0, 1u) << "% succeeded.";
		Log::info() << "Early reject SSD validation: " << String::toAString(earlyRejectPercent * 100.0, 1u) << "% succeeded.";

		allSucceeded = normalPercent >= 0.99 && allSucceeded;
		allSucceeded = earlyRejectPercent >= 0.95 && allSucceeded;
	}

	return allSucceeded;
}

bool TestBlobFeatureDetector::testSSDPerformance(const size_t numberDescriptors, const Scalar threshold, const double testDuration, Worker& worker)
{
	Log::info() << "Testing SSD for individual descriptor types:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testSSDPerformance<int, 36u>(numberDescriptors, threshold, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testSSDPerformance<int, 64u>(numberDescriptors, threshold, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testSSDPerformance<float, 36u>(numberDescriptors, threshold, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testSSDPerformance<float, 64u>(numberDescriptors, threshold, testDuration, worker) && allSucceeded;

	Log::info() << " ";

#ifndef OCEAN_HARDWARE_REDUCED_PERFORMANCE

	Log::info() << " ";

	allSucceeded = testSSDPerformance<double, 36u>(numberDescriptors, threshold, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testSSDPerformance<double, 64u>(numberDescriptors, threshold, testDuration, worker) && allSucceeded;

	Log::info() << " ";

#endif

	if (allSucceeded)
		Log::info() << "SSD test succeeded.";
	else
		Log::info() << "SSD test FAILED!";

	return allSucceeded;
}

template <typename TElement, unsigned int tElements>
bool TestBlobFeatureDetector::testSSDPerformance(const size_t numberDescriptors, const Scalar threshold, const double testDuration, Worker& worker)
{
	Log::info() << "... with " << tElements << " " << TypeNamer::name<TElement>() << " elements:";

	typedef std::vector<TElement> Descriptors;

	typedef typename SquareValueTyper<TElement>::Type SSDType;

	const SSDType normalizedThreshold = SSDType(threshold * Scalar(BlobDescriptor::descriptorNormalization<TElement>() * BlobDescriptor::descriptorNormalization<TElement>()));

	if (std::is_same<TElement, int>::value)
	{
		ocean_assert(Scalar(normalizedThreshold) > threshold);
	}
	else
	{
		ocean_assert(NumericD::isWeakEqual(double(normalizedThreshold), double(threshold)));
	}

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	for (unsigned int earlyRejectLoop = 0u; earlyRejectLoop <= 1u; ++earlyRejectLoop)
	{
		Log::info() << " ";

		if (earlyRejectLoop == 0u)
			Log::info() << "... with exact matching:";
		else
			Log::info() << "... with early reject matching:";

		unsigned long long validIterations = 0ull;
		unsigned long long iterations = 0ull;

		HighPerformanceStatistic performance, performanceSIMD;
		Timestamp startTimestamp(true);

		do
		{
			Descriptors forwardDescriptors(numberDescriptors * tElements);
			Descriptors backwardDescriptors(numberDescriptors * tElements);

			for (size_t n = 0; n < numberDescriptors; ++n)
			{
				double doubleDescriptor[tElements];

				for (unsigned int i = 0u; i < tElements; ++i)
				{
					doubleDescriptor[i] = RandomD::scalar(randomGenerator, -1, 1);
				}

				const double length = NumericD::sqrt(NumericD::dot(doubleDescriptor, doubleDescriptor, tElements));

				for (unsigned int i = 0u; i < tElements; ++i)
				{
					const TElement normalizedElement = TElement(double(doubleDescriptor[i] * BlobDescriptor::descriptorNormalization<TElement>()) / length);

					forwardDescriptors[n * tElements + i] = normalizedElement;
					backwardDescriptors[(numberDescriptors - n - 1) * tElements + i] = normalizedElement;
				}
			}

			std::vector<unsigned int> bestMatchingReferenceFeatures(numberDescriptors);
			std::vector<SSDType> bestSSDs(numberDescriptors);

			for (unsigned int simdLoop = 0u; simdLoop <= 1u; ++simdLoop)
			{
				if (simdLoop == 0u)
				{
					const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performance);

					if (earlyRejectLoop == 0u)
						matchDescriptorsSubset<TElement, tElements, false, false>(forwardDescriptors.data(), backwardDescriptors.data(), numberDescriptors, normalizedThreshold, bestMatchingReferenceFeatures.data(), bestSSDs.data(), 0u, (unsigned int)numberDescriptors);
					else
						matchDescriptorsSubset<TElement, tElements, false, true>(forwardDescriptors.data(), backwardDescriptors.data(), numberDescriptors, normalizedThreshold, bestMatchingReferenceFeatures.data(), bestSSDs.data(), 0u, (unsigned int)numberDescriptors);
				}
				else
				{
					const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performanceSIMD);

					if (earlyRejectLoop == 0u)
						matchDescriptorsSubset<TElement, tElements, true, false>(forwardDescriptors.data(), backwardDescriptors.data(), numberDescriptors, normalizedThreshold, bestMatchingReferenceFeatures.data(), bestSSDs.data(), 0u, (unsigned int)numberDescriptors);
					else
						matchDescriptorsSubset<TElement, tElements, true, true>(forwardDescriptors.data(), backwardDescriptors.data(), numberDescriptors, normalizedThreshold, bestMatchingReferenceFeatures.data(), bestSSDs.data(), 0u, (unsigned int)numberDescriptors);
				}
			}

			for (size_t n = 0; n < numberDescriptors; ++n)
			{
				const unsigned int bestMatchingReferenceFeature = bestMatchingReferenceFeatures[n];

				if (bestMatchingReferenceFeature == numberDescriptors - n - 1)
					if (NumericT<TElement>::isEqual(bestSSDs[n], summedSquaredDifferences(forwardDescriptors.data() + bestMatchingReferenceFeature * tElements, backwardDescriptors.data() + n * tElements, tElements)))
						++validIterations;

				iterations++;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);
		const double percent = double(validIterations) / double(iterations);

		Log::info() << "Single core default performance: Best: " << String::toAString(performance.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 2u) << "ms, average: " << String::toAString(performance.averageMseconds(), 2u) << "ms";
		Log::info() << "Single core SIMD performance: Best: " << String::toAString(performanceSIMD.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSIMD.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSIMD.averageMseconds(), 2u) << "ms";

		allSucceeded = percent >= 0.98 && allSucceeded;

		if (worker)
		{
			validIterations = 0ull;
			iterations = 0ull;

			HighPerformanceStatistic performanceMulticoreSIMD;
			const Timestamp startTimestampMulticore(true);

			do
			{
				Descriptors forwardDescriptors(numberDescriptors * tElements);
				Descriptors backwardDescriptors(numberDescriptors * tElements);

				for (size_t n = 0; n < numberDescriptors; ++n)
				{
					double doubleDescriptor[tElements];

					for (unsigned int i = 0u; i < tElements; ++i)
					{
						doubleDescriptor[i] = RandomD::scalar(randomGenerator, -1, 1);
					}

					const double length = NumericD::sqrt(NumericD::dot(doubleDescriptor, doubleDescriptor, tElements));

					for (unsigned int i = 0u; i < tElements; ++i)
					{
						const TElement normalizedElement = TElement(double(doubleDescriptor[i] * BlobDescriptor::descriptorNormalization<TElement>()) / length);

						forwardDescriptors[n * tElements + i] = normalizedElement;
						backwardDescriptors[(numberDescriptors - n - 1) * tElements + i] = normalizedElement;
					}
				}

				std::vector<unsigned int> bestMatchingReferenceFeatures(numberDescriptors);
				std::vector<SSDType> bestSSDs(numberDescriptors);

				performanceMulticoreSIMD.start();

				if (earlyRejectLoop == 0u)
					worker.executeFunction(Worker::Function::createStatic(&TestBlobFeatureDetector::matchDescriptorsSubset<TElement, tElements, true, false>, (const TElement*)forwardDescriptors.data(), (const TElement*)backwardDescriptors.data(), numberDescriptors, normalizedThreshold, bestMatchingReferenceFeatures.data(), bestSSDs.data(), 0u, 0u), 0u, (unsigned int)numberDescriptors);
				else
					worker.executeFunction(Worker::Function::createStatic(&TestBlobFeatureDetector::matchDescriptorsSubset<TElement, tElements, true, true>, (const TElement*)forwardDescriptors.data(), (const TElement*)backwardDescriptors.data(), numberDescriptors, normalizedThreshold, bestMatchingReferenceFeatures.data(), bestSSDs.data(), 0u, 0u), 0u, (unsigned int)numberDescriptors);

				performanceMulticoreSIMD.stop();

				for (size_t n = 0; n < numberDescriptors; ++n)
				{
					const unsigned int bestMatchingReferenceFeature = bestMatchingReferenceFeatures[n];

					if (bestMatchingReferenceFeature == numberDescriptors - n - 1)
						if (NumericT<TElement>::isEqual(bestSSDs[n], summedSquaredDifferences(forwardDescriptors.data() + bestMatchingReferenceFeature * tElements, backwardDescriptors.data() + n * tElements, tElements)))
							++validIterations;

					iterations++;
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));

			ocean_assert(iterations != 0ull);
			const double percentMulticore = double(validIterations) / double(iterations);

			Log::info() << "Multi core SIMD performance: Best: " << String::toAString(performanceMulticoreSIMD.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticoreSIMD.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticoreSIMD.averageMseconds(), 2u) << "ms";
			Log::info() << "Multi core SIMD boost: Best: " << String::toAString(performanceSIMD.best() / performanceMulticoreSIMD.best(), 1u) << "x, worst: " << String::toAString(performanceSIMD.worst() / performanceMulticoreSIMD.worst(), 1u) << "x, average: " << String::toAString(performanceSIMD.average() / performanceMulticoreSIMD.average(), 1u) << "x";

			allSucceeded = percentMulticore >= 0.98 && allSucceeded;
		}
	}

	return allSucceeded;
}

bool TestBlobFeatureDetector::testUnidirectionalCorrespondences(const uint32_t* linedIntegral, const unsigned int width, const unsigned int height, const CV::Detector::Blob::BlobFeatures& testFeatures, const Scalar threshold, const double testDuration, Worker& worker)
{
	Log::info() << "Test unidirectional correspondences of " << testFeatures.size() << " features with " << BlobDescriptor::elements() << " " << TypeNamer::name<BlobDescriptor::DescriptorElement>() << " elements:";

	BlobFeatures features(testFeatures);

	for (BlobFeatures::iterator i = features.begin(); i != features.end(); ++i)
	{
		BlobFeatureDescriptor::calculateOrientation(linedIntegral, width, height, FrameType::ORIGIN_UPPER_LEFT, BlobFeature::ORIENTATION_SLIDING_WINDOW, *i, true);
		BlobFeatureDescriptor::calculateDescriptor(linedIntegral, width, height, FrameType::ORIGIN_UPPER_LEFT, *i, true);
	}

	BlobFeatures referenceFeatures(features.rbegin(), features.rend());

	bool allSucceeded = true;

	for (unsigned int qualityRatioLoop = 0u; qualityRatioLoop <= 1u; ++qualityRatioLoop)
	{
		Log::info() << " ";
		Log::info().newLine(qualityRatioLoop != 0u);

		if (qualityRatioLoop == 0u)
			Log::info() << "... without quality ratio:";
		else
			Log::info() << "... with quality ratio:";

		for (unsigned int earlyRejectLoop = 0u; earlyRejectLoop <= 1u; ++earlyRejectLoop)
		{
			Log::info() << " ";

			if (earlyRejectLoop == 0u)
				Log::info() << "... with exact matching:";
			else
				Log::info() << "... with early reject matching:";

			unsigned long long validIterations = 0ull;
			unsigned long long iterations = 0ull;

			HighPerformanceStatistic performance;
			Timestamp startTimestamp(true);

			do
			{
				const BlobFeatures& forwardFeatures = features;
				const BlobFeatures& backwardFeatures = referenceFeatures;

				Tracking::Blob::Correspondences::CorrespondencePairs correspondences;

				performance.start();

				if (qualityRatioLoop == 0u)
				{
					if (earlyRejectLoop == 0u)
						correspondences = Tracking::Blob::UnidirectionalCorrespondences::determineFeatureCorrespondences(forwardFeatures, backwardFeatures, forwardFeatures.size(), threshold);
					else
						correspondences = Tracking::Blob::UnidirectionalCorrespondences::determineFeatureCorrespondencesEarlyReject(forwardFeatures, backwardFeatures, forwardFeatures.size(), threshold);
				}
				else
				{
					if (earlyRejectLoop == 0u)
						correspondences = Tracking::Blob::UnidirectionalCorrespondences::determineFeatureCorrespondencesWithQuality(forwardFeatures, backwardFeatures, forwardFeatures.size(), threshold, Scalar(0.6));
					else
						correspondences = Tracking::Blob::UnidirectionalCorrespondences::determineFeatureCorrespondencesWithQualityEarlyReject(forwardFeatures, backwardFeatures, forwardFeatures.size(), threshold, Scalar(0.6));
				}

				performance.stop();

				for (Tracking::Blob::Correspondences::CorrespondencePairs::const_iterator i = correspondences.begin(); i != correspondences.end(); ++i)
				{
					if (i->first == (unsigned int)backwardFeatures.size() - i->second - 1u)
						++validIterations;

					iterations++;
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));

			ocean_assert(iterations != 0ull);
			const double percent = double(validIterations) / double(iterations);

			Log::info() << "Performance: Best: " << performance.bestMseconds() << "ms, worst: " << performance.worstMseconds() << "ms, average: " << performance.averageMseconds() << "ms";
			Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

			allSucceeded = percent >= 0.95 && allSucceeded;

			if (worker)
			{
				Log::info() << " ";

				if (earlyRejectLoop == 0u)
					Log::info() << "... with with exact matching (multicore):";
				else
					Log::info() << "... with early reject matching (multicore):";

				validIterations = 0ull;
				iterations = 0ull;

				HighPerformanceStatistic performanceMulticore;
				const Timestamp startTimestampMulticore(true);

				do
				{
					const BlobFeatures& forwardFeatures = features;
					const BlobFeatures& backwardFeatures = referenceFeatures;

					Tracking::Blob::Correspondences::CorrespondencePairs correspondences;

					performanceMulticore.start();

					if (qualityRatioLoop == 0u)
					{
						if (earlyRejectLoop == 0u)
							correspondences = Tracking::Blob::UnidirectionalCorrespondences::determineFeatureCorrespondences(forwardFeatures, backwardFeatures, forwardFeatures.size(), threshold, &worker);
						else
							correspondences = Tracking::Blob::UnidirectionalCorrespondences::determineFeatureCorrespondencesEarlyReject(forwardFeatures, backwardFeatures, forwardFeatures.size(), threshold, &worker);
					}
					else
					{
						if (earlyRejectLoop == 0u)
							correspondences = Tracking::Blob::UnidirectionalCorrespondences::determineFeatureCorrespondencesWithQuality(forwardFeatures, backwardFeatures, forwardFeatures.size(), threshold, Scalar(0.6), &worker);
						else
							correspondences = Tracking::Blob::UnidirectionalCorrespondences::determineFeatureCorrespondencesWithQualityEarlyReject(forwardFeatures, backwardFeatures, forwardFeatures.size(), threshold, Scalar(0.6), &worker);
					}

					performanceMulticore.stop();

					for (Tracking::Blob::Correspondences::CorrespondencePairs::const_iterator i = correspondences.begin(); i != correspondences.end(); ++i)
					{
						if (i->first == (unsigned int)backwardFeatures.size() - i->second - 1u)
							++validIterations;

						iterations++;
					}
				}
				while (startTimestampMulticore + testDuration > Timestamp(true));

				ocean_assert(iterations != 0ull);
				const double percentMulticore = double(validIterations) / double(iterations);

				Log::info() << "Multicore performance: Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms";
				Log::info() << "Multicore boost: Best: " << String::toAString(performance.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performance.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performance.average() / performanceMulticore.average(), 1u) << "x";
				Log::info() << "Validation: " << String::toAString(percentMulticore * 100.0, 1u) << "% succeeded.";

				allSucceeded = percentMulticore >= 0.95 && allSucceeded;
			}
		}
	}

	return allSucceeded;
}

bool TestBlobFeatureDetector::testOverallPerformance(const Frame& yFrame, const Scalar threshold, const double testDuration, Worker& worker)
{
	ocean_assert(yFrame.isValid());
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test overall speed: integral image, detection and description (with " << BlobDescriptor::elements() << " elements):";
	Log::info() << " ";

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	Frame integralFrame(FrameType(yFrame.width() + 1u, yFrame.height() + 1u, FrameType::genericPixelFormat<uint32_t, 1>(), FrameType::ORIGIN_UPPER_LEFT));
	ocean_assert(integralFrame.isContinuous());

	uint32_t* integralImage = integralFrame.data<uint32_t>();

	BlobFeatures features;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	for (const BlobFeature::OrientationType orientationType : {BlobFeature::ORIENTATION_SLIDING_WINDOW, BlobFeature::ORIENTATION_SUMMED_ORIENTATION, BlobFeature::ORIENTATION_NOT_ORIENTED})
	{
		Timestamp startTimestamp(true);

		if (orientationType == BlobFeature::ORIENTATION_SLIDING_WINDOW)
		{
			Log::info() << "... with sliding window orientation:";
		}
		else if (orientationType == BlobFeature::ORIENTATION_SUMMED_ORIENTATION)
		{
			Log::info() << "... with summed orientation:";
		}
		else
		{
			Log::info() << "... without orientation (unoriented):";
		}

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			do
			{
				features.clear();

				performance.start();

				CV::IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(yFrame.constdata<uint8_t>(), integralImage, yFrame.width(), yFrame.height(), yFrame.paddingElements(), integralFrame.paddingElements());

				BlobFeatureDetector::detectFeatures(integralImage, yFrame.width(), yFrame.height(), BlobFeatureDetector::SAMPLING_NORMAL, threshold, true, features, useWorker);

				if (useWorker == nullptr)
				{
					if (orientationType != BlobFeature::ORIENTATION_NOT_ORIENTED)
					{
						for (BlobFeature& feature : features)
						{
							BlobFeatureDescriptor::calculateOrientation(integralImage, yFrame.width(), yFrame.height(), FrameType::ORIGIN_UPPER_LEFT, orientationType, feature, true);
							BlobFeatureDescriptor::calculateDescriptor(integralImage, yFrame.width(), yFrame.height(), FrameType::ORIGIN_UPPER_LEFT, feature, true);
						}
					}
					else
					{
						for (BlobFeature& feature : features)
						{
							BlobFeatureDescriptor::calculateNotOrientedDescriptor(integralImage, yFrame.width(), yFrame.height(), FrameType::ORIGIN_UPPER_LEFT, feature, true);
						}
					}
				}
				else
				{
					if (orientationType != BlobFeature::ORIENTATION_NOT_ORIENTED)
					{
						BlobFeatureDescriptor::calculateOrientationsAndDescriptors(integralImage, yFrame.width(), yFrame.height(), FrameType::ORIGIN_UPPER_LEFT, orientationType, features, true, useWorker);
					}
					else
					{
						BlobFeatureDescriptor::calculateNotOrientedDescriptors(integralImage, yFrame.width(), yFrame.height(), FrameType::ORIGIN_UPPER_LEFT, features, worker);
					}
				}

				performance.stop();
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		Log::info() << "Found features: " << features.size();
		Log::info() << "Performance: Best: " << performanceSinglecore.bestMseconds() << "ms, worst: " << performanceSinglecore.worstMseconds() << "ms, average: " << performanceSinglecore.averageMseconds() << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
		}

		Log::info() << " ";
	}

	if (yFrame.width() != yFrame.height())
	{
		// finally we also ensure that the detector works in the rotated image
		// we want to check the tracker on a different aspect ratio

		Frame yFrameRotated;
		const bool rotationResult = CV::FrameInterpolatorNearestPixel::Comfort::rotate90(yFrame, yFrameRotated, true);
		ocean_assert_and_suppress_unused(rotationResult, rotationResult);

		Frame rotatedIntegralFrame = CV::IntegralImage::Comfort::createLinedImage(yFrameRotated);

		BlobFeatures rotatedFeatures;
		BlobFeatureDetector::detectFeatures(rotatedIntegralFrame.constdata<uint32_t>(), yFrameRotated.width(), yFrameRotated.height(), BlobFeatureDetector::SAMPLING_NORMAL, threshold, true, rotatedFeatures);

		for (BlobFeature& rotatedFeature : rotatedFeatures)
		{
			BlobFeatureDescriptor::calculateOrientation(rotatedIntegralFrame.constdata<uint32_t>(), yFrameRotated.width(), yFrameRotated.height(), FrameType::ORIGIN_UPPER_LEFT, BlobFeature::ORIENTATION_SLIDING_WINDOW, rotatedFeature, true);
			BlobFeatureDescriptor::calculateDescriptor(rotatedIntegralFrame.constdata<uint32_t>(), yFrameRotated.width(), yFrameRotated.height(), FrameType::ORIGIN_UPPER_LEFT, rotatedFeature, true);
		}
	}

	return true;
}

bool TestBlobFeatureDetector::validateResponseMap(const Frame& yFrame, const BlobFeatureDetector::ResponseMap& responseMap)
{
	// Check whether the response map is large enough to provide features
	if (!responseMap)
	{
		return true;
	}

	ocean_assert(yFrame.isValid() && yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8));

	const unsigned int filterSize_2 = responseMap.filterSize() / 2u;
	const unsigned int nullSizeSmall = responseMap.filterIndex();
	const unsigned int nullSizeLarge = nullSizeSmall + 1u;

	const unsigned int lobeSizeSmall = responseMap.filterSize() / 3u;
	const unsigned int lobeSizeLarge = responseMap.filterSize() - 2u * nullSizeLarge;

	ocean_assert(lobeSizeSmall * 3u == responseMap.filterSize());
	ocean_assert(lobeSizeLarge > lobeSizeSmall);

	for (unsigned int y = 0u; y < yFrame.height(); ++y)
	{
		if (y >= responseMap.responseFirstY() && y + filterSize_2 < yFrame.height())
		{
			// check whether a response value exists for this position due to the defined sampling steps
			if ((y - responseMap.responseFirstY()) % responseMap.samplingStep() == 0u)
			{
				const unsigned int responseY = (y - responseMap.responseFirstY()) / responseMap.samplingStep();
				ocean_assert(responseY < responseMap.responseHeight());

				for (unsigned int x = 0u; x < yFrame.width(); ++x)
				{
					if (x >= responseMap.responseFirstX() && x + filterSize_2 < yFrame.width())
					{
						// check whether a response value exists for this position due to the defined sampling steps
						if ((x - responseMap.responseFirstX()) % responseMap.samplingStep() == 0u)
						{
							const unsigned int copyX = x;
							const unsigned int copyY = y;

							const unsigned int responseX = (x - responseMap.responseFirstX()) / responseMap.samplingStep();
							ocean_assert(responseX < responseMap.responseWidth());

							/**
							 * Computation of Lxx
							 * The Lxx filter has the following scheme, with the filter lobe [B C B]:
							 *  -----------------
							 * |        A        |
							 * |-----------------|
							 * |     |     |     |
							 * |  B  |  C  |  B  |
							 * |     |     |     |
							 * |-----------------|
							 * |        A        |
							 *  -----------------
							 * A: Each pixel is multiplied by zero, therefore A does not need to be handled.
							 *    For a 9x9 filter each A block has a size of 2x9, a filter of 15x15 has an A block size of 3x15, 21x21 has 4x21, ...
							 *    The block size is computed by (1 + filterIndex)x(filterSizeFull).
							 *
							 * B: Each pixel is multiplied by 1.
							 *    For a 9x9 filter each B block has a size of 5x3, a filter of 15x15 has a B block size of 9x5, 21x21 has 13x7, ...
							 *    The block size is computed by (1 + filterIndex * 4)x(filterSizeFull / 3).
							 *
							 * C: Each pixel is multiplied by -2.
							 *    The filter size is identical to the B block.
							 */

							ocean_assert(int(x) - int(filterSize_2) >= 0);
							ocean_assert(int(x) - int(filterSize_2) + int(lobeSizeSmall) <= int(responseMap.frameWidth()));

							ocean_assert(int(y) - int(lobeSizeLarge / 2) >= 0);
							ocean_assert(int(y) - int(lobeSizeLarge / 2) + int(lobeSizeLarge) <= int(responseMap.frameHeight()));

							Scalar lxxLeft = 0;
							for (unsigned int xx = x - filterSize_2; xx < x - filterSize_2 + lobeSizeSmall; ++xx)
							{
								for (unsigned int yy = y - lobeSizeLarge / 2; yy < y - lobeSizeLarge / 2 + lobeSizeLarge; ++yy)
								{
									lxxLeft += Scalar(yFrame.constpixel<uint8_t>(xx, yy)[0]);
								}
							}

							ocean_assert(int(x) + int(lobeSizeSmall / 2) + 1 >= 0);
							ocean_assert(int(x) + int(filterSize_2) + 1 <= int(responseMap.frameWidth()));

							Scalar lxxRight = 0;
							for (unsigned int xx = x + lobeSizeSmall / 2 + 1; xx < x + filterSize_2 + 1; ++xx)
							{
								for (unsigned int yy = y - lobeSizeLarge / 2; yy < y - lobeSizeLarge / 2 + lobeSizeLarge; ++yy)
								{
									lxxRight += Scalar(yFrame.constpixel<uint8_t>(xx, yy)[0]);
								}
							}

							Scalar lxxMiddle = 0;
							for (unsigned int xx = x - lobeSizeSmall / 2; xx < x + lobeSizeSmall / 2 + 1; ++xx)
							{
								for (unsigned int yy = y - lobeSizeLarge / 2; yy < y - lobeSizeLarge / 2 + lobeSizeLarge; ++yy)
								{
									lxxMiddle += Scalar(yFrame.constpixel<uint8_t>(xx, yy)[0]);
								}
							}

							Scalar lxx = lxxLeft + lxxRight - 2 * lxxMiddle;
							lxx /= Scalar(responseMap.filterArea());


							/**
							 * Computation of Lyy
							 * The Lyy filter has the following scheme, with the (transposed) filter lobe [B C B]
							 *  -----------------
							 * |   |    B    |   |
							 * |   |---------|   |
							 * | A |    C    | A |
							 * |   |---------|   |
							 * |   |    B    |   |
							 *  -----------------
							 * A, B and C have the same size as for Lxx.
							 * Lxx and Lyy are identical except a 90 degree rotation.
							 */

							Scalar lyyTop = 0;
							for (unsigned int xx = x - lobeSizeLarge / 2; xx <= x + lobeSizeLarge / 2; ++xx)
							{
								for (unsigned int yy = y - filterSize_2; yy < y - lobeSizeSmall / 2; ++yy)
								{
									lyyTop += Scalar(yFrame.constpixel<uint8_t>(xx, yy)[0]);
								}
							}

							Scalar lyyBottom = 0;
							for (unsigned int xx = x - lobeSizeLarge / 2; xx <= x + lobeSizeLarge / 2; ++xx)
							{
								for (unsigned int yy = y + lobeSizeSmall / 2 + 1; yy <= y + filterSize_2; ++yy)
								{
									lyyBottom += Scalar(yFrame.constpixel<uint8_t>(xx, yy)[0]);
								}
							}

							Scalar lyyMiddle = 0;
							for (unsigned int xx = x - lobeSizeLarge / 2; xx <= x + lobeSizeLarge / 2; ++xx)
							{
								for (unsigned int yy = y - lobeSizeSmall / 2; yy <= y + lobeSizeSmall / 2; ++yy)
								{
									lyyMiddle += Scalar(yFrame.constpixel<uint8_t>(xx, yy)[0]);
								}
							}

							Scalar lyy = lyyTop + lyyBottom - 2 * lyyMiddle;
							lyy /= Scalar(responseMap.filterArea());


							/**
							 * Computation of Lxy
							 * The Lxy filter has the following scheme:
							 *  ---------------------
							 * | A                 A |
							 * |    -------------    |
							 * |   |     | |     |   |
							 * |   |  B  | |  C  |   |
							 * |   |     | |     |   |
							 * |   |-----   -----|   |
							 * |   |      D      |   |
							 * |   |-----   -----|   |
							 * |   |     | |     |   |
							 * |   |  C  | |  B  |   |
							 * |   |     | |     |   |
							 * |    -------------    |
							 * | A                 A |
							 *  ---------------------
							 * A: Each pixel is multiplied by zero, therefore A does not need to be handled.
							 *    A is a border surrounding the entire filter.
							 *    For a 9x9 filter the A border has a width of 1 pixel, a filter of 15x15 has a border of 2 pixels, 21x21 has 3 pixels, ....
							 *    The border width is identical to the filterIndex.
							 *
							 * B: Each pixel is multiplied by 1.
							 *    For a 9x9 filter each B block has a size of 3x3, a 15x15 filter has a B block size of 5x5, 21x21 has 7x7, ...
							 *    The block size is computed by (1 + filterIndex * 2)x(1 + filterIndex * 2).
							 *
							 * C: Each pixel is multiplied by -1.
							 *    The size of each C block is identical to the B blocks.
							 *
							 * D: Each pixel is multiplied by zero, therefore D does not need to be handled.
							 *    Independent of the filter size D has a simple cross shape with one pixel thickness.
							 */

							Scalar lxyLeftTop = 0;
							for (unsigned int xx = x - lobeSizeSmall; xx < x; ++xx)
							{
								for (unsigned int yy = y - lobeSizeSmall; yy < y; ++yy)
								{
									lxyLeftTop += Scalar(yFrame.constpixel<uint8_t>(xx, yy)[0]);
								}
							}

							Scalar lxyRightTop = 0;
							for (unsigned int xx = x + 1; xx <= x + lobeSizeSmall; ++xx)
							{
								for (unsigned int yy = y - lobeSizeSmall; yy < y; ++yy)
								{
									lxyRightTop += Scalar(yFrame.constpixel<uint8_t>(xx, yy)[0]);
								}
							}

							Scalar lxyLeftBottom = 0;
							for (unsigned int xx = x - lobeSizeSmall; xx < x; ++xx)
							{
								for (unsigned int yy = y + 1; yy <= y + lobeSizeSmall; ++yy)
								{
									lxyLeftBottom += Scalar(yFrame.constpixel<uint8_t>(xx, yy)[0]);
								}
							}

							Scalar lxyRightBottom = 0;
							for (unsigned int xx = x + 1; xx <= x + lobeSizeSmall; ++xx)
							{
								for (unsigned int yy = y + 1; yy <= y + lobeSizeSmall; ++yy)
								{
									lxyRightBottom += Scalar(yFrame.constpixel<uint8_t>(xx, yy)[0]);
								}
							}

							Scalar lxy = lxyLeftTop - lxyRightTop - lxyLeftBottom + lxyRightBottom;
							lxy /= Scalar(responseMap.filterArea());

							Scalar total = lxx * lyy - Scalar(0.81) * lxy * lxy;
							Scalar response = *(responseMap.filterResponse() + responseY * responseMap.responseWidth() + responseX);

							Scalar difference = Numeric::abs(total - response);

							if (Numeric::isNotWeakEqualEps(difference))
							{
								return false;
							}

							// verify that x and y has not been changed
							ocean_assert_and_suppress_unused(copyX == x, copyX);
							ocean_assert_and_suppress_unused(copyY == y, copyY);
						}
					}
				}
			}
		}
	}

	return true;
}

bool TestBlobFeatureDetector::validateDescriptor(const uint8_t* /*frame*/, const unsigned int /*width*/, const unsigned int /*height*/, const BlobFeature& /*feature*/)
{
	/**
	 * We do not use a standard haar wavelet, because the wavelet does not have a unique center pixel.
	 * We use the following horizontal filter instead:
	 *  -----------
	 * |    | |    |
	 * |    | |    |
	 * | -1 |X| +1 |
	 * |    | |    |
	 * |    | |    |
	 *  -----------
	 * The filter is quadratic and has size of 2 * 1.5s + 1 pixel resulting in an odd filter size.
	 * X defines the center position.
	 */

	/**
	 * Our vertical filter::
	 *  ---------
	 * |         |
	 * |    -1   |
	 * |---------|
	 * |    X    |
	 * |---------|
	 * |    +1   |
	 * |         |
	 *  ---------
	 * The filter is quadratic and has size of 2 * 1.5s + 1 pixel resulting in an odd filter size.
	 * X defines the center position.
	 */

	// filter size should have 2 * scale

#if 0
	const unsigned int filterHalf = (unsigned int)(feature.scale() + Scalar(0.5));
	const unsigned int filterSize = 2u * filterHalf + 1u;


	// Entire descriptor interest area: 20 * scale

	// The region is split up regularly into smaller 4x4 square sub-regions.
	// Each sub-region has size 5 * scale

	// We compute Haar wavelet response at 5x5 regularly spaced sample points.

	for (int blockY = 0; blockY < 4; ++blockY)
		for (int blockX = 0; blockX < 4; ++blockX)
		{
			for (int y = 0; y < 5; ++y)
				for (int x = 0; x < 5; ++x)
				{
					ocean_assert(false && "**TODO**");
					// **TODO**
				}
		}
#endif // 0

	return false;
}

bool TestBlobFeatureDetector::validateMaximumSuppression(const BlobFeatureDetector::ResponseMap& low, const BlobFeatureDetector::ResponseMap& middle, const BlobFeatureDetector::ResponseMap& high, const Scalar threshold, const CV::Detector::Blob::BlobFeatures& features)
{
	if (features.empty())
	{
		return true;
	}

	BlobFeatures validateFeatures;

	const Scalar* highResponse = high.filterResponse();
	const Scalar* middleResponse = middle.filterResponse();
	const Scalar* lowResponse = low.filterResponse();

	for (unsigned int yHigh = 1u; yHigh < high.responseHeight() - 1u; ++yHigh)
	{
		const unsigned int candidateFrameY = high.responseFirstY() + yHigh * high.samplingStep();

		ocean_assert((candidateFrameY - middle.responseFirstY()) % middle.samplingStep() == 0u);
		const unsigned int yCandidateMiddle = (candidateFrameY - middle.responseFirstY()) / middle.samplingStep();

		for (unsigned int xHigh = 1u; xHigh < high.responseWidth() - 1u; ++xHigh)
		{
			const unsigned int candidateFrameX = high.responseFirstX() + xHigh * high.samplingStep();

			ocean_assert((candidateFrameX - middle.responseFirstX()) % middle.samplingStep() == 0u);
			const unsigned int xCandidateMiddle = (candidateFrameX - middle.responseFirstX()) / middle.samplingStep();

			const Scalar candidate = middleResponse[yCandidateMiddle * middle.responseWidth() + xCandidateMiddle];

			if (candidate >= threshold)
			{
				bool isMaximum = true;

				for (unsigned int xx = xHigh - 1u; xx <= xHigh + 1u; ++xx)
				{
					for (unsigned int yy = yHigh - 1u; yy <= yHigh + 1u; ++yy)
					{
						// high layer
						if (highResponse[yy * high.responseWidth() + xx] > candidate)
						{
							isMaximum = false;
						}

						const unsigned int frameY = high.responseFirstY() + yy * high.samplingStep();
						const unsigned int frameX = high.responseFirstX() + xx * high.samplingStep();


						ocean_assert((frameY - middle.responseFirstY()) % middle.samplingStep() == 0u);
						const unsigned int yMiddle = (frameY - middle.responseFirstY()) / middle.samplingStep();
						ocean_assert((frameX - middle.responseFirstX()) % middle.samplingStep() == 0u);
						const unsigned int xMiddle = (frameX - middle.responseFirstX()) / middle.samplingStep();

						// middle layer (except candidate)
						if (middleResponse[yMiddle * middle.responseWidth() + xMiddle] > candidate)
						{
							if (xx != xHigh || yy != yHigh)
							{
								isMaximum = false;
							}
						}


						ocean_assert((frameY - low.responseFirstY()) % low.samplingStep() == 0u);
						const unsigned int yLow = (frameY - low.responseFirstY()) / low.samplingStep();
						ocean_assert((frameX - low.responseFirstX()) % low.samplingStep() == 0u);
						const unsigned int xLow = (frameX - low.responseFirstX()) / low.samplingStep();

						// low layer
						if (lowResponse[yLow * low.responseWidth() + xLow] > candidate)
						{
							isMaximum = false;
						}
					}
				}

				if (isMaximum)
				{
					const unsigned int frameXLow = high.responseFirstX() + (xHigh - 1u) * high.samplingStep();
					const unsigned int frameXHigh = high.responseFirstX() + (xHigh + 1u) * high.samplingStep();
					const unsigned int frameYLow = high.responseFirstY() + (yHigh - 1u) * high.samplingStep();
					const unsigned int frameYHigh = high.responseFirstY() + (yHigh + 1u) * high.samplingStep();

					ocean_assert((frameXLow - middle.responseFirstX()) % middle.samplingStep() == 0u);
					const unsigned int xMiddleLow = (frameXLow - middle.responseFirstX()) / middle.samplingStep();

					ocean_assert((frameXHigh - middle.responseFirstX()) % middle.samplingStep() == 0u);
					const unsigned int xMiddleHigh = (frameXHigh - middle.responseFirstX()) / middle.samplingStep();

					ocean_assert((frameYLow - middle.responseFirstY()) % middle.samplingStep() == 0u);
					const unsigned int yMiddleLow = (frameYLow - middle.responseFirstY()) / middle.samplingStep();

					ocean_assert((frameYHigh - middle.responseFirstY()) % middle.samplingStep() == 0u);
					const unsigned int yMiddleHigh = (frameYHigh - middle.responseFirstY()) / middle.samplingStep();

					const Scalar dxHigh = middleResponse[yCandidateMiddle * middle.responseWidth() + xMiddleHigh];
					const Scalar dxLow = middleResponse[yCandidateMiddle * middle.responseWidth() + xMiddleLow];
					const Scalar dx = (dxHigh - dxLow) / 2u;
					const Scalar dxx = dxHigh + dxLow - 2u * candidate;

					const Scalar dyHigh = middleResponse[yMiddleHigh * middle.responseWidth() + xCandidateMiddle];
					const Scalar dyLow = middleResponse[yMiddleLow * middle.responseWidth() + xCandidateMiddle];
					const Scalar dy = (dyHigh - dyLow) / 2u;
					const Scalar dyy = dyHigh + dyLow - 2u * candidate;


					ocean_assert((candidateFrameY - low.responseFirstY()) % low.samplingStep() == 0u);
					const unsigned int yCandidateLow = (candidateFrameY - low.responseFirstY()) / low.samplingStep();
					ocean_assert((candidateFrameX - low.responseFirstX()) % low.samplingStep() == 0u);
					const unsigned int xCandidateLow = (candidateFrameX - low.responseFirstX()) / low.samplingStep();

					const Scalar dsHigh = highResponse[yHigh * high.responseWidth() + xHigh];
					const Scalar dsLow = lowResponse[yCandidateLow * low.responseWidth() + xCandidateLow];
					const Scalar ds = (dsHigh - dsLow) / 2u;
					const Scalar dss = dsHigh + dsLow - 2u * candidate;


					const Scalar dxy = (middleResponse[yMiddleHigh * middle.responseWidth() + xMiddleHigh] + middleResponse[yMiddleLow * middle.responseWidth() + xMiddleLow] - middleResponse[yMiddleHigh * middle.responseWidth() + xMiddleLow] - middleResponse[yMiddleLow * middle.responseWidth() + xMiddleHigh]) / 4;

					ocean_assert((frameXLow - low.responseFirstX()) % low.samplingStep() == 0u);
					const unsigned int xLowLow = (frameXLow - low.responseFirstX()) / low.samplingStep();

					ocean_assert((frameXHigh - low.responseFirstX()) % low.samplingStep() == 0u);
					const unsigned int xLowHigh = (frameXHigh - low.responseFirstX()) / low.samplingStep();

					ocean_assert((frameYLow - low.responseFirstY()) % low.samplingStep() == 0u);
					const unsigned int yLowLow = (frameYLow - low.responseFirstY()) / low.samplingStep();

					ocean_assert((frameYHigh - low.responseFirstY()) % low.samplingStep() == 0u);
					const unsigned int yLowHigh = (frameYHigh - low.responseFirstY()) / low.samplingStep();

					const Scalar dxs = (highResponse[yHigh * high.responseWidth() + xHigh + 1] + lowResponse[yCandidateLow * low.responseWidth() + xLowLow] - highResponse[yHigh * high.responseWidth() + xHigh - 1] - lowResponse[yCandidateLow * low.responseWidth() + xLowHigh]) / Scalar(4);
					const Scalar dys = (highResponse[(yHigh + 1) * high.responseWidth() + xHigh] + lowResponse[yLowLow * low.responseWidth() + xCandidateLow] - highResponse[(yHigh - 1) * high.responseWidth() + xHigh] - lowResponse[yLowHigh * low.responseWidth() + xCandidateLow]) / Scalar(4);

					SquareMatrix3 hh(dxx, dxy, dxs, dxy, dyy, dys, dxs, dys, dss);

					if (hh.invert())
					{
						const Vector3 offset(-(hh * Vector3(dx, dy, ds)));

						if (Numeric::abs(offset.x()) < Scalar(0.6) && Numeric::abs(offset.y()) < Scalar(0.6) && Numeric::abs(offset.z()) < Scalar(0.6))
						{
							Scalar interpolatedX = Scalar(high.responseFirstX()) + (Scalar(xHigh) + offset.x()) * Scalar(high.samplingStep());
							Scalar interpolatedY = Scalar(high.responseFirstY()) + (Scalar(yHigh) + offset.y()) * Scalar(high.samplingStep());

							ocean_assert(high.filterSize() - middle.filterSize() == middle.filterSize() - low.filterSize());
							const Scalar scale = Scalar(0.1333333333333333) * (Scalar(middle.filterSize()) + Scalar(high.filterSize() - middle.filterSize()) * offset.z());
							ocean_assert(scale > 0);

							validateFeatures.push_back(BlobFeature(Vector2(interpolatedX, interpolatedY), BlobFeature::DS_UNDISTORTED, scale, candidate, false));
						}
					}
				}
			}
		}
	}

	// we expect more features than validated features (as the actual implementation provides an advanced search - which is currently not tested here)

	if (features.size() < validateFeatures.size())
	{
		return false;
	}

	size_t foundFeatures = 0;
	for (const BlobFeature& feature : features)
	{
		for (const BlobFeature& validateFeature : validateFeatures)
		{
			if (feature.position().isEqual(validateFeature.position(), Numeric::weakEps()))
			{
				++foundFeatures;
				break;
			}
		}
	}

	return features.size() == foundFeatures;
}

template <bool tEarlyReject>
void TestBlobFeatureDetector::matchDescriptorsSubset(const BlobFeature* forwardFeatures, const BlobFeature* backwardFeatures, const size_t backwardSize, const BlobDescriptor::DescriptorSSD normalizedThreshold, uint32_t* bestMatchingBackwards, BlobDescriptor::DescriptorSSD* bestSSDs, const unsigned int firstForward, const unsigned int numberForward)
{
	ocean_assert(forwardFeatures && backwardFeatures);
	ocean_assert(bestMatchingBackwards && bestSSDs);

	BlobDescriptor::DescriptorSSD ssd;
	BlobDescriptor::DescriptorSSD minSSD;
	unsigned int minIndex;

	for (unsigned int f = firstForward; f < firstForward + numberForward; ++f)
	{
		minSSD = BlobDescriptor::descriptorMaximalSSD();
		minIndex = (unsigned int)(-1);

		for (unsigned int b = 0u; b < (unsigned int)backwardSize; ++b)
		{
			ssd = NumericT<BlobDescriptor::DescriptorSSD>::maxValue();

			if constexpr (tEarlyReject)
			{
				if (forwardFeatures[f].isDescriptorEqualEarlyReject(backwardFeatures[b], normalizedThreshold, ssd) && ssd < minSSD)
				{
					minSSD = ssd;
					minIndex = b;

					// we can check the correctness of the ssd only if we have a reliable match
					ocean_assert(NumericT<BlobDescriptor::DescriptorSSD>::isEqual(ssd, summedSquaredDifferences(forwardFeatures[f].descriptor()(), backwardFeatures[b].descriptor()(), BlobDescriptor::elements())));
				}
			}
			else
			{
				if (forwardFeatures[f].isDescriptorEqual(backwardFeatures[b], normalizedThreshold, ssd) && ssd < minSSD)
				{
					minSSD = ssd;
					minIndex = b;
				}

				if (forwardFeatures[f].laplace() == backwardFeatures[b].laplace())
				{
					// we can always check the correctness of the ssd,
					// as long as both laplace responses are identical

					ocean_assert(NumericT<BlobDescriptor::DescriptorSSD>::isEqual(ssd, summedSquaredDifferences(forwardFeatures[f].descriptor()(), backwardFeatures[b].descriptor()(), BlobDescriptor::elements())));
				}
			}
		}

		bestMatchingBackwards[f] = minIndex;
		bestSSDs[f] = minSSD;
	}
}

template <typename TElement, unsigned int tElements, bool tUseSIMD, bool tEarlyReject>
void TestBlobFeatureDetector::matchDescriptorsSubset(const TElement* forwardDescriptors, const TElement* backwardDescriptors, const size_t backwardSize, const typename SquareValueTyper<TElement>::Type normalizedThreshold, uint32_t* bestMatchingBackwards, typename SquareValueTyper<TElement>::Type* bestSSDs, const unsigned int firstForward, const unsigned int numberForward)
{
	ocean_assert(forwardDescriptors != nullptr && backwardDescriptors != nullptr && bestMatchingBackwards != nullptr);

	typedef typename SquareValueTyper<TElement>::Type SSDType;

	SSDType ssd;
	SSDType minSSD;
	unsigned int minIndex;

	for (unsigned int f = firstForward; f < firstForward + numberForward; ++f)
	{
		minSSD = NumericT<TElement>::maxValue();
		minIndex = (unsigned int)(-1);

		for (unsigned int b = 0u; b < (unsigned int)backwardSize; ++b)
		{
			if constexpr (tEarlyReject)
			{
				if constexpr (tUseSIMD)
				{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
					if (BlobDescriptor::isDescriptorEqualEarlyRejectSSE<TElement, tElements>(forwardDescriptors + f * tElements, backwardDescriptors + b * tElements, normalizedThreshold, ssd) && ssd < minSSD)
#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
					if (BlobDescriptor::isDescriptorEqualEarlyRejectNEON<TElement, tElements>(forwardDescriptors + f * tElements, backwardDescriptors + b * tElements, normalizedThreshold, ssd) && ssd < minSSD)
#else
					if (BlobDescriptor::isDescriptorEqualEarlyRejectFallback<TElement, tElements>(forwardDescriptors + f * tElements, backwardDescriptors + b * tElements, normalizedThreshold, ssd) && ssd < minSSD)
#endif
					{
						minSSD = ssd;
						minIndex = b;

						// we can check the correctness of the ssd only if we have a reliable match
						ocean_assert(NumericT<TElement>::isEqual(ssd, summedSquaredDifferences(forwardDescriptors + f * tElements, backwardDescriptors + b * tElements, tElements)));
					}
				}
				else
				{
					if (BlobDescriptor::isDescriptorEqualEarlyRejectFallback<TElement, tElements>(forwardDescriptors + f * tElements, backwardDescriptors + b * tElements, normalizedThreshold, ssd) && ssd < minSSD)
					{
						minSSD = ssd;
						minIndex = b;

						// we can check the correctness of the ssd only if we have a reliable match
						const SSDType testSSD = summedSquaredDifferences(forwardDescriptors + f * tElements, backwardDescriptors + b * tElements, tElements);

						ocean_assert_and_suppress_unused(NumericT<TElement>::isWeakEqual(ssd, testSSD), testSSD);
					}

				}
			}
			else
			{
				if constexpr (tUseSIMD)
				{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
					ssd = BlobDescriptor::SummedSquareDifferencesSSE<TElement>::template ssd<tElements>(forwardDescriptors + f * tElements, backwardDescriptors + b * tElements);
#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
					ssd = BlobDescriptor::SummedSquareDifferencesNEON<TElement>::template ssd<tElements>(forwardDescriptors + f * tElements, backwardDescriptors + b * tElements);
#else
					ssd = BlobDescriptor::ssdFallback<TElement, tElements>(forwardDescriptors + f * tElements, backwardDescriptors + b * tElements);
#endif

					if (ssd < minSSD)
					{
						minSSD = ssd;
						minIndex = b;
					}
				}
				else
				{
					ssd = BlobDescriptor::ssdFallback<TElement, tElements>(forwardDescriptors + f * tElements, backwardDescriptors + b * tElements);

					if (ssd < minSSD)
					{
						minSSD = ssd;
						minIndex = b;
					}

				}

				// we can always check the correctness of the ssd
				const SSDType testSSD = summedSquaredDifferences(forwardDescriptors + f * tElements, backwardDescriptors + b * tElements, tElements);

				ocean_assert_and_suppress_unused(NumericT<TElement>::isWeakEqual(ssd, testSSD), testSSD);
			}
		}

		bestMatchingBackwards[f] = minIndex;
		bestSSDs[f] = minSSD;
	}
}

}

}

}

}
