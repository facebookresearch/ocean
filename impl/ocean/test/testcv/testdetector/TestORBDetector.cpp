/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/TestORBDetector.h"
#include "ocean/test/testcv/testdetector/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Utilities.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/cv/detector/ORBSamplingPattern.h"
#include "ocean/cv/detector/ORBFeatureDescriptor.h"
#include "ocean/cv/detector/ORBFeatureOrientation.h"
#include "ocean/cv/detector/FASTFeatureDetector.h"

#include "ocean/math/Random.h"

#include "ocean/test/Validation.h"

#include <random>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

bool TestORBDetector::test(const Frame& frame, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   ORB detector test:   ---";
	Log::info() << " ";

	Frame yFrame;
	if (frame.isValid() && !CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
	{
		ocean_assert(false && "This must never happen!");
		Log::info() << "ORB detector test FAILED!";

		return false;
	}

	bool allSucceeded = true;

	allSucceeded = testOrientationDetermination(testDuration, worker, yFrame) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDescriptorDetermination(testDuration, worker, yFrame) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectReferenceFeaturesAndDetermineDescriptors(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHammingDistanceDetermination(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDescriptorMatching(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "ORB detector test succeeded.";
	}
	else
	{
		Log::info() << "ORB detector test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestORBDetector, OrientationDetermination)
{
	Worker worker;
	EXPECT_TRUE(TestORBDetector::testOrientationDetermination(GTEST_TEST_DURATION, worker));
}

TEST(TestORBDetector, DescriptorDetermination)
{
	Worker worker;
	EXPECT_TRUE(TestORBDetector::testDescriptorDetermination(GTEST_TEST_DURATION, worker));
}

TEST(TestORBDetector, DetectReferenceFeaturesAndDetermineDescriptors)
{
	Worker worker;
	EXPECT_TRUE(TestORBDetector::testDetectReferenceFeaturesAndDetermineDescriptors(GTEST_TEST_DURATION, worker));
}

TEST(TestORBDetector, HammingDistanceDetermination)
{
	EXPECT_TRUE(TestORBDetector::testHammingDistanceDetermination(GTEST_TEST_DURATION));
}

TEST(TestORBDetector, DescriptorMatching)
{
	Worker worker;
	EXPECT_TRUE(TestORBDetector::testDescriptorMatching(GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestORBDetector::testOrientationDetermination(const double testDuration, Worker& worker, const Frame& yFrameTest)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(!yFrameTest.isValid() || yFrameTest.isPixelFormatCompatible(FrameType::FORMAT_Y8));

	constexpr size_t numberRandomFeatures = 1000;

	Log::info() << "Testing orientation calculation of " << numberRandomFeatures << " randomized feature points:";
	Log::info() << " ";

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		Frame yFrame;

		if (yFrameTest.isValid() && yFrameTest.width() >= 65u && yFrameTest.height() >= 65u)
		{
			yFrame = Frame(yFrameTest, Frame::ACM_USE_KEEP_LAYOUT);
		}
		else
		{
			yFrame = Utilities::createRandomFrameWithFeatures(1280u, 720u, 2u, &randomGenerator);
			ocean_assert(yFrame.pixelFormat() == FrameType::FORMAT_Y8);
		}

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			Frame linedIntegralFrame = CV::IntegralImage::Comfort::createLinedImage(yFrame);
			ocean_assert(linedIntegralFrame);

			CV::Detector::ORBFeatures randomFeatures;
			randomFeatures.reserve(numberRandomFeatures);

			while (randomFeatures.size() < numberRandomFeatures)
			{
				const Vector2 position = Random::vector2(randomGenerator, Scalar(16), Scalar(yFrame.width() - 17u), Scalar(16), Scalar(yFrame.height() - 17u));

				randomFeatures.emplace_back(position, CV::Detector::PointFeature::DS_UNKNOWN, Scalar(0));
			}

			ocean_assert(linedIntegralFrame.isContinuous());

			performance.start();
				CV::Detector::ORBFeatureOrientation::determineFeatureOrientation(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), randomFeatures, useWorker);
			performance.stop();

			if (validateOrientation(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), randomFeatures) < 0.99)
			{
				OCEAN_SET_FAILED(validation);
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << performanceSinglecore;

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore performance: " << performanceMulticore;
		Log::info() << "Multicore boost factor: " << String::toAString(NumericD::ratio(performanceSinglecore.averageMseconds(), performanceMulticore.averageMseconds()), 1u) << "x";
	}

	Log::info() << "Validation " << validation;

	return validation.succeeded();
}

bool TestORBDetector::testDescriptorDetermination(const double testDuration, Worker& worker, const Frame& yFrameTest)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(!yFrameTest.isValid() || yFrameTest.isPixelFormatCompatible(FrameType::FORMAT_Y8));

	constexpr size_t numberRandomFeatures = 1000;

	Log::info() << "Testing description of " << numberRandomFeatures << " randomized feature points:";
	Log::info() << " ";

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		Frame yFrame;

		if (yFrameTest.isValid() && yFrameTest.width() >= 65u && yFrameTest.height() >= 65u)
		{
			yFrame = Frame(yFrameTest, Frame::ACM_USE_KEEP_LAYOUT);
		}
		else
		{
			yFrame = Utilities::createRandomFrameWithFeatures(1280u, 720u, 2u, &randomGenerator);
			ocean_assert(yFrame.pixelFormat() == FrameType::FORMAT_Y8);
		}

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			Frame linedIntegralFrame = CV::IntegralImage::Comfort::createLinedImage(yFrame);
			ocean_assert(linedIntegralFrame);

			CV::Detector::ORBFeatures randomFeatures;
			randomFeatures.reserve(numberRandomFeatures);

			while (randomFeatures.size() < numberRandomFeatures)
			{
				const Vector2 position = Random::vector2(randomGenerator, Scalar(21), Scalar(yFrame.width() - 22u), Scalar(21), Scalar(yFrame.height() - 22u));

				randomFeatures.emplace_back(position, CV::Detector::PointFeature::DS_UNKNOWN, Scalar(0));
			}

			CV::Detector::ORBFeatureOrientation::determineFeatureOrientation(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), randomFeatures, useWorker);

			ocean_assert(linedIntegralFrame.isContinuous());

			performance.start();
				CV::Detector::ORBFeatureDescriptor::determineDescriptors(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), randomFeatures, false /*useSublayers*/, useWorker);
			performance.stop();

			if (!validateDescriptors(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), randomFeatures))
			{
				OCEAN_SET_FAILED(validation);
			}

			// now, we validate features determine with the FAST detector

			CV::Detector::FASTFeatures fastFeaturePoints;
			CV::Detector::FASTFeatureDetector::detectFeatures(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), 30u, false, false, fastFeaturePoints, yFrame.paddingElements(), useWorker);
			CV::Detector::ORBFeatures detectedFeatures = CV::Detector::ORBFeature::features2ORBFeatures(fastFeaturePoints, yFrame.width(), yFrame.height());

			CV::Detector::ORBFeatureOrientation::determineFeatureOrientation(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), detectedFeatures, useWorker);
			CV::Detector::ORBFeatureDescriptor::determineDescriptors(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), detectedFeatures, false /*useSublayers*/, useWorker);

			if (!validateDescriptors(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), detectedFeatures))
			{
				OCEAN_SET_FAILED(validation);
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << performanceSinglecore;

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore performance: " << performanceMulticore;
		Log::info() << "Multicore boost factor: " << String::toAString(NumericD::ratio(performanceSinglecore.average(), performanceMulticore.average()), 1u) << "x";
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestORBDetector::testDetectReferenceFeaturesAndDetermineDescriptors(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing detect reference features and determine descriptors:";
	Log::info() << " ";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 64u, 2000u);
		const unsigned int height = RandomI::random(randomGenerator, 64u, 2000u);

		const Frame yFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));

		const unsigned int layers = RandomI::random(randomGenerator, 1u, 20u);

		const bool useHarrisFeatures = RandomI::boolean(randomGenerator);
		const unsigned int featureThreshold = RandomI::random(randomGenerator, 40u);
		const bool useWorker = RandomI::boolean(randomGenerator);

		CV::Detector::ORBFeatures features;
		if (!CV::Detector::ORBFeatureDescriptor::detectReferenceFeaturesAndDetermineDescriptors(yFrame, features, layers, useHarrisFeatures, featureThreshold, useWorker ? &worker : nullptr))
		{
			OCEAN_SET_FAILED(validation);
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestORBDetector::testHammingDistanceDetermination(const double testDuration)
{
	constexpr unsigned int constIterations = 1000000u;

	Log::info() << "Test hamming distance calculation of " << String::insertCharacter(String::toAString(constIterations), ',', 3, false) << " descriptor pairs:";
	Log::info() << " ";

	HighPerformanceStatistic performance;
	HighPerformanceStatistic performanceBitset;

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);;

	unsigned int dummy = 0u;

	const Timestamp startTimestamp;

	do
	{
		std::vector<CV::Detector::ORBDescriptor> descriptors1(constIterations);
		std::vector<CV::Detector::ORBDescriptor> descriptors2(constIterations);

		for (size_t i = 0; i < descriptors1.size(); i++)
		{
			for (unsigned int j = 0u; j < 4u; j++)
			{
				static_assert(sizeof(CV::Detector::ORBDescriptor) == 4 * sizeof(uint64_t), "Invalid data type!");

				const uint64_t randomValue1 = RandomI::random64(randomGenerator);
				const uint64_t randomValue2 = RandomI::random64(randomGenerator);

				memcpy((uint8_t*)(&descriptors1[i]) + sizeof(uint64_t) * j, &randomValue1, sizeof(uint64_t));
				memcpy((uint8_t*)(&descriptors2[i]) + sizeof(uint64_t) * j, &randomValue2, sizeof(uint64_t));
			}
		}

		performanceBitset.start();
			for (size_t i = 0; i < descriptors1.size(); i++)
			{
				dummy += (unsigned int)(descriptors1[i].bitset() ^ descriptors2[i].bitset()).count();
			}
		performanceBitset.stop();

		performance.start();
			for (size_t i = 0; i < descriptors1.size(); i++)
			{
				dummy += CV::Detector::ORBFeatureDescriptor::calculateHammingDistance(descriptors1[i], descriptors2[i]);
			}
		performance.stop();

		if (!validateHammingDistance(descriptors1, descriptors2))
		{
			OCEAN_SET_FAILED(validation);
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (dummy > 1u)
	{
		Log::info() << "Bitset Performance: " << performanceBitset;
		Log::info() << "Performance: " << performance;
	}
	else
	{
		Log::info() << "Bitset Performance: " << performanceBitset;
		Log::info() << "Performance: " << performance;
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestORBDetector::testDescriptorMatching(const double testDuration, Worker& worker)
{
	constexpr size_t featureSize = 1000;

	Log::info() << "Test brute force matching of " << featureSize << " randomized descriptor pairs (threshold 0.25):";
	Log::info() << " ";

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			CV::Detector::ORBFeatures referenceFeatures(featureSize);
			CV::Detector::ORBFeatures tmpFeatures(featureSize);

			std::set<CV::Detector::ORBDescriptor> descriptorSet;
			std::vector<CV::Detector::ORBDescriptor> descriptors;
			descriptors.reserve(featureSize);

			std::random_device randomDevice;
			std::mt19937 randomNumberGenerator(randomDevice());

			while(descriptorSet.size() < featureSize)
			{
				CV::Detector::ORBDescriptor tmpDescriptor;

				for (unsigned int j = 0u; j < 4u; j++)
				{
					static_assert(sizeof(CV::Detector::ORBDescriptor) == 4 * sizeof(uint64_t), "Invalid data type!");

					const uint64_t randomValue = RandomI::random64(randomGenerator);

					memcpy((uint8_t*)(&tmpDescriptor) + sizeof(uint64_t) * j, &randomValue, sizeof(uint64_t));
				}

				if (descriptorSet.insert(tmpDescriptor).second)
				{
					descriptors.push_back(tmpDescriptor);
				}
			}

			Indices32 shuffleIndex;
			shuffleIndex.reserve(featureSize);

			for (unsigned int i = 0u; i < featureSize; i++)
			{
				shuffleIndex.emplace_back(i);
			}

			std::shuffle(shuffleIndex.begin(), shuffleIndex.end(), randomNumberGenerator);

			IndexPairs32 checkMatches;
			checkMatches.reserve(featureSize);

			for (unsigned int i = 0u; i < featureSize; i++)
			{
				referenceFeatures[shuffleIndex[i]].addDescriptor(descriptors[i]);
				tmpFeatures[i].addDescriptor(descriptors[i]);
				checkMatches.push_back(IndexPair32(i, shuffleIndex[i]));
			}

			IndexPairs32 matches;

			performance.start();
				CV::Detector::ORBFeatureDescriptor::determineNonBijectiveCorrespondences(tmpFeatures, referenceFeatures, matches, 0.25f, useWorker);
			performance.stop();

			if (!validateDescriptorMatching(matches, checkMatches))
			{
				OCEAN_SET_FAILED(validation);
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << performanceSinglecore;

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore performance: " << performanceMulticore;
		Log::info() << "Multicore boost factor: " << String::toAString(NumericD::ratio(performanceSinglecore.average(), performanceMulticore.average()), 1u) << "x";
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();;
}

double TestORBDetector::validateOrientation(const uint32_t* linedIntegralFrame, const unsigned int width, const unsigned int height, CV::Detector::ORBFeatures& features)
{
	ocean_assert(linedIntegralFrame != nullptr && !features.empty());

	constexpr unsigned int linedIntegralFramePaddingElements = 0u;

	size_t validOrientations = 0;

	for (const CV::Detector::ORBFeature& feature : features)
	{
		ocean_assert(width != 0u && height != 0u);

		const Vector2& observation = feature.observation();

		constexpr int patchSize_2 = 15;

		constexpr unsigned int radiusSquare = patchSize_2 * patchSize_2;

		Scalar m_01 = Scalar(0.0);
		Scalar m_10 = Scalar(0.0);

		for (int y = -patchSize_2; y <= patchSize_2; y++)
		{
			for (int x = -patchSize_2; x <= patchSize_2; x++)
			{
				if ((unsigned int)(x * x + y * y) <= radiusSquare)
				{
					const Vector2 lookup = observation + Vector2(Scalar(x), Scalar(y));
					const Vector2 centerLookup = lookup + Vector2(Scalar(0.5), Scalar(0.5));

					ocean_assert(centerLookup.x() >= Scalar(0.5) && centerLookup.y() >= Scalar(0.5));
					ocean_assert(centerLookup.x() < Scalar(width) - Scalar(0.5) && centerLookup.y() < Scalar(height) - Scalar(0.5));

					if (centerLookup.x() < Scalar(0.5) || centerLookup.y() < Scalar(0.5) || centerLookup.x() >= Scalar(width) - Scalar(0.5) || centerLookup.y() >= Scalar(height) - Scalar(0.5))
					{
						return 0.0;
					}

					const Scalar pixelValue = CV::FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, centerLookup, CV::PC_CENTER, 1u, 1u);

					m_01 += Scalar(y) * pixelValue;
					m_10 += Scalar(x) * pixelValue;
				}
			}
		}

		const Scalar angle = Numeric::atan2(Scalar(m_01), Scalar(m_10));
		const Scalar orientation = Numeric::angleAdjustPositive(angle);

		ocean_assert(orientation >= 0 && orientation < Numeric::pi2());
		const Scalar checkOrientation = feature.orientation();
		ocean_assert(checkOrientation >= 0 && checkOrientation < Numeric::pi2());

		if (Numeric::isWeakEqual(orientation, checkOrientation))
		{
			validOrientations++;
		}
	}

	ocean_assert(features.size() != 0);
	return double(validOrientations) / double(features.size());
}

bool TestORBDetector::validateDescriptors(const uint32_t* linedIntegralFrame, const unsigned int width, const unsigned int height, CV::Detector::ORBFeatures& features)
{
	constexpr unsigned int linedIntegralFramePaddingElements = 0u;

	const CV::Detector::ORBSamplingPattern& samplingPattern = CV::Detector::ORBSamplingPattern::get();
	const size_t numberDescriptorBitset = sizeof(CV::Detector::ORBDescriptor::DescriptorBitset) * 8;

	for (size_t i = 0; i < features.size(); ++i)
	{
		const Scalar x = Scalar(features[i].observation().x() + Scalar(0.5));
		const Scalar y = Scalar(features[i].observation().y() + Scalar(0.5));
		ocean_assert(x >= Scalar(0.0) && x < Scalar(width));
		ocean_assert(y >= Scalar(0.0) && y < Scalar(height));

		const CV::Detector::ORBSamplingPattern::LookupTable& lookupTable = samplingPattern.samplingPatternForAngle(features[i].orientation());

		CV::Detector::ORBDescriptor descriptor;

		for (size_t j = 0; j < numberDescriptorBitset; ++j)
		{
			const Vector2& offset0 = lookupTable[j].point0();
			const Vector2& offset1 = lookupTable[j].point1();

			const Vector2 point0(x + offset0.x(), y + offset0.y());
			const Vector2 point1(x + offset1.x(), y + offset1.y());

			ocean_assert(point0.x() - Scalar(2.0) >= Scalar(0.0) && point0.x() + Scalar(2.0) <= Scalar(width) && point0.y() - Scalar(2.0) >= Scalar(0.0) && point0.y() + Scalar(2.0) <= Scalar(height));
			ocean_assert(point1.x() - Scalar(2.0) >= Scalar(0.0) && point1.x() + Scalar(2.0) <= Scalar(width) && point1.y() - Scalar(2.0) >= Scalar(0.0) && point1.y() + Scalar(2.0) <= Scalar(height));

			const Scalar intensity0 = CV::FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, point0, CV::PC_CENTER, 5u, 5u);
			const Scalar intensity1 = CV::FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, point1, CV::PC_CENTER, 5u, 5u);

			if (intensity0 < intensity1)
			{
				descriptor[j] = 1;
			}
		}

		CV::Detector::ORBDescriptor& checkDescriptor = features[i].firstDescriptor();

		for (size_t j = 0; j < numberDescriptorBitset; ++j)
		{
			if (checkDescriptor[j] != descriptor[j])
			{
				return false;
			}
		}
	}

	return true;
}

bool TestORBDetector::validateHammingDistance(const std::vector<CV::Detector::ORBDescriptor>& descriptors1, const std::vector<CV::Detector::ORBDescriptor>& descriptors2)
{
	ocean_assert(descriptors1.size() == descriptors2.size());

	for (unsigned int i = 0u; i < descriptors1.size(); i++)
	{
		const CV::Detector::ORBDescriptor& descriptor1 = descriptors1[i];
		const CV::Detector::ORBDescriptor& descriptor2 = descriptors2[i];

		const unsigned int hammingDistance = CV::Detector::ORBFeatureDescriptor::calculateHammingDistance(descriptor1, descriptor2);

		unsigned int checkHammingDistance = 0u;

		for (size_t j = 0; j < sizeof(CV::Detector::ORBDescriptor::DescriptorBitset) * 8; j++)
		{
			if (descriptor1[j] != descriptor2[j])
			{
				checkHammingDistance++;
			}
		}

		if (checkHammingDistance != hammingDistance)
		{
			return false;
		}
	}

	return true;
}

bool TestORBDetector::validateDescriptorMatching(const IndexPairs32& matches, const IndexPairs32& checkMatches)
{
	ocean_assert(matches.size() == checkMatches.size());

	for (const IndexPair32& match : matches)
	{
		for (const IndexPair32& checkMatch : checkMatches)
		{
			if (match.first == checkMatch.first && match.second != checkMatch.second)
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
