// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/test/testcv/testdetector/TestORBDetector.h"
#include "ocean/test/testcv/testdetector/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Utilities.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/cv/detector/ORBSamplingPattern.h"
#include "ocean/cv/detector/ORBFeatureOrientation.h"
#include "ocean/cv/detector/FASTFeatureDetector.h"

#include "ocean/math/Random.h"

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

	allSucceeded = testDescriptorDetermination(testDuration, worker, yFrame) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testHammingDistanceDetermination(testDuration) && allSucceeded;

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

TEST(TestORBDetector, HammingDistanceDetermination)
{
	Worker worker;
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

	bool allSucceeded = true;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

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
			yFrame = Utilities::createRandomFrameWithFeatures(1280u, 720u, 2u);
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
				const Scalar x = Random::scalar(31, Scalar(yFrame.width()) - 32);
				const Scalar y = Random::scalar(31, Scalar(yFrame.height()) - 32);

				randomFeatures.emplace_back(Vector2(x, y), CV::Detector::PointFeature::DS_UNKNOWN, Scalar(0));
			}

			performance.start();
				ocean_assert(linedIntegralFrame.isContinuous());
				CV::Detector::ORBFeatureOrientation::determineFeatureOrientation(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), randomFeatures, useWorker);
			performance.stop();

			if (validateOrientation(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), randomFeatures) < 0.99)
			{
				allSucceeded = false;
			}

			// now, we validate features determine with the FAST detector

			CV::Detector::FASTFeatures fastFeaturePoints;
			CV::Detector::FASTFeatureDetector::detectFeatures(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), 30u, false, false, fastFeaturePoints, yFrame.paddingElements(), useWorker);
			CV::Detector::ORBFeatures detectedFeatures = CV::Detector::ORBFeature::features2ORBFeatures(fastFeaturePoints, yFrame.width(), yFrame.height());

			CV::Detector::ORBFeatureOrientation::determineFeatureOrientation(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), detectedFeatures, useWorker);

			if (validateOrientation(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), detectedFeatures) < 0.99)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << String::toAString(performanceSinglecore.averageMseconds()) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore performance: " << String::toAString(performanceMulticore.averageMseconds()) << "ms";
		Log::info() << "Multicore boost factor: " << String::toAString(NumericD::ratio(performanceSinglecore.averageMseconds(), performanceMulticore.averageMseconds()), 1u) << "x";
	}

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

bool TestORBDetector::testDescriptorDetermination(const double testDuration, Worker& worker, const Frame& yFrameTest)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(!yFrameTest.isValid() || yFrameTest.isPixelFormatCompatible(FrameType::FORMAT_Y8));

	constexpr size_t numberRandomFeatures = 1000;

	Log::info() << "Testing description of " << numberRandomFeatures << " randomized feature points:";

	bool allSucceeded = true;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

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
			yFrame = Utilities::createRandomFrameWithFeatures(1280u, 720u, 2u);
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
				const Scalar x = Random::scalar(31, Scalar(yFrame.width()) - 32);
				const Scalar y = Random::scalar(31, Scalar(yFrame.height()) - 32);

				randomFeatures.emplace_back(Vector2(x, y), CV::Detector::PointFeature::DS_UNKNOWN, Scalar(0));
			}

			CV::Detector::ORBFeatureOrientation::determineFeatureOrientation(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), randomFeatures, useWorker);

			performance.start();
				ocean_assert(linedIntegralFrame.isContinuous());
				CV::Detector::ORBFeatureDescriptor::determineDescriptors(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), randomFeatures, false /*useSublayers*/, useWorker);
			performance.stop();

			if (!validateDescriptors(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), randomFeatures))
			{
				allSucceeded = false;
			}

			// now, we validate features determine with the FAST detector

			CV::Detector::FASTFeatures fastFeaturePoints;
			CV::Detector::FASTFeatureDetector::detectFeatures(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), 30u, false, false, fastFeaturePoints, yFrame.paddingElements(), useWorker);
			CV::Detector::ORBFeatures detectedFeatures = CV::Detector::ORBFeature::features2ORBFeatures(fastFeaturePoints, yFrame.width(), yFrame.height());

			CV::Detector::ORBFeatureOrientation::determineFeatureOrientation(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), detectedFeatures, useWorker);
			CV::Detector::ORBFeatureDescriptor::determineDescriptors(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), detectedFeatures, false /*useSublayers*/, useWorker);

			if (!validateDescriptors(linedIntegralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), detectedFeatures))
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << String::toAString(performanceSinglecore.averageMseconds()) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore performance: " << String::toAString(performanceMulticore.averageMseconds()) << "ms";
		Log::info() << "Multicore boost factor: " << String::toAString(NumericD::ratio(performanceSinglecore.averageMseconds(), performanceMulticore.averageMseconds()), 1u) << "x";
	}

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

bool TestORBDetector::testHammingDistanceDetermination(const double testDuration)
{
	constexpr unsigned int constIterations = 1000000u;

	Log::info() << "Test hamming distance calculation of " << String::insertCharacter(String::toAString(constIterations), ',', 3, false) << " descriptor pairs: ";

	bool allSucceeded = true;

	unsigned int dummy = 0u;

	HighPerformanceStatistic performance;
	HighPerformanceStatistic performanceBitset;

	const Timestamp startTimestamp;

	do
	{
		std::vector<CV::Detector::ORBDescriptor> descriptors1(constIterations);
		std::vector<CV::Detector::ORBDescriptor> descriptors2(constIterations);

		for (size_t i = 0; i < descriptors1.size(); i++)
		{
			for (unsigned int j = 0u; j < 4u; j++)
			{
				ocean_assert(size_t(&descriptors1[i]) % sizeof(uint64_t) == 0);
				ocean_assert(size_t(&descriptors2[i]) % sizeof(uint64_t) == 0);

				((uint64_t*)&descriptors1[i])[j] = RandomI::random64();
				((uint64_t*)&descriptors2[i])[j] = RandomI::random64();
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
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (dummy > 1u)
	{
		Log::info() << "Bitset Performance: Best: " << performanceBitset.bestMseconds() << "ms, worst: " << performanceBitset.worstMseconds() << "ms, average: " << performanceBitset.averageMseconds() << "ms";
		Log::info() << "Performance: Best: " << performance.bestMseconds() << "ms, worst: " << performance.worstMseconds() << "ms, average: " << performance.averageMseconds() << "ms";
	}
	else
	{
		Log::info() << "Bitset Performance: Best: " << performanceBitset.bestMseconds() << "ms, worst: " << performanceBitset.worstMseconds() << "ms, average: " << performanceBitset.averageMseconds() << "ms";
		Log::info() << "Performance: Best: " << performance.bestMseconds() << "ms, worst: " << performance.worstMseconds() << "ms, average: " << performance.averageMseconds() << "ms";
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

bool TestORBDetector::testDescriptorMatching(const double testDuration, Worker& worker)
{
	constexpr size_t featureSize = 1000;

	Log::info() << "Test brute force matching of " << featureSize << " randomized descriptor pairs (threshold 0.25): ";

	bool allSucceeded = true;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

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
					ocean_assert(size_t(&tmpDescriptor) % sizeof(uint64_t) == 0);

					((uint64_t*)&tmpDescriptor)[j] = RandomI::random64();
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
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << String::toAString(performanceSinglecore.averageMseconds()) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore performance: " << String::toAString(performanceMulticore.averageMseconds()) << "ms";
		Log::info() << "Multicore boost factor: " << String::toAString(NumericD::ratio(performanceSinglecore.averageMseconds(), performanceMulticore.averageMseconds()), 1u) << "x";
	}

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

double TestORBDetector::validateOrientation(const uint32_t* linedIntegralFrame, const unsigned int width, const unsigned int height, CV::Detector::ORBFeatures& features)
{
	ocean_assert(linedIntegralFrame != nullptr && !features.empty());

	constexpr unsigned int linedIntegralFramePaddingElements = 0u;

	size_t validOrientations = 0;

	for (size_t i = 0; i < features.size(); i++)
	{
		ocean_assert(width != 0u && height != 0u);

		constexpr int patchSizeHalf = 15;
		ocean_assert(features[i].observation().x() - patchSizeHalf > 0 && features[i].observation().x() + Scalar(patchSizeHalf) < Scalar(width) && "patch outside of frame");
		ocean_assert(features[i].observation().y() - patchSizeHalf > 0 && features[i].observation().y() + Scalar(patchSizeHalf) < Scalar(height) && "patch outside of frame");

		const unsigned int radiusSquare = patchSizeHalf * patchSizeHalf;

		Scalar m_01 = Scalar(0.0);
		Scalar m_10 = Scalar(0.0);

		for (int y = -patchSizeHalf; y <= patchSizeHalf; y++)
		{
			for (int x = -patchSizeHalf; x <= patchSizeHalf; x++)
			{
				if ((unsigned int)(x * x + y * y) <= radiusSquare)
				{
					Scalar pixelValue = CV::FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, features[i].observation() + Vector2(Scalar(x) + Scalar(0.5), Scalar(y) + Scalar(0.5)), CV::PC_CENTER, 1u, 1u);
					m_01 += Scalar(y) * pixelValue;
					m_10 += Scalar(x) * pixelValue;
				}
			}
		}

		const Scalar angle = Numeric::atan2(Scalar(m_01), Scalar(m_10));
		// in 5 degree increments
		const Scalar orientation = Numeric::angleAdjustPositive(angle);

		ocean_assert(orientation >= 0 && orientation < Numeric::pi2());
		const Scalar checkOrientation = features[i].orientation();
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

		const CV::Detector::ORBSamplingPattern::IntensityComparisons& lookupTable = samplingPattern.samplingPatternByAngle(features[i].orientation());
		CV::Detector::ORBDescriptor descriptor;

		for (size_t j = 0; j < numberDescriptorBitset; ++j)
		{
			const Vector2 point1(x + lookupTable[j].x1(), y + lookupTable[j].y1());
			const Vector2 point2(x + lookupTable[j].x2(), y + lookupTable[j].y2());
			ocean_assert(point1.x() - Scalar(2.0) >= Scalar(0.0) && point1.x() + Scalar(2.0) <= Scalar(width) && point1.y() - Scalar(2.0) >= Scalar(0.0) && point1.y() + Scalar(2.0) <= Scalar(height));
			ocean_assert(point2.x() - Scalar(2.0) >= Scalar(0.0) && point2.x() + Scalar(2.0) <= Scalar(width) && point2.y() - Scalar(2.0) >= Scalar(0.0) && point2.y() + Scalar(2.0) <= Scalar(height));

			const Scalar intensity1 = CV::FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, point1, CV::PC_CENTER, 5u, 5u);
			const Scalar intensity2 = CV::FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, point2, CV::PC_CENTER, 5u, 5u);

			if (intensity1 < intensity2)
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
