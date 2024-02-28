// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testtracking/TestMatchingBlob.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Random.h"

#include "ocean/tracking/blob/UnidirectionalCorrespondences.h"

namespace Ocean
{

namespace Test
{

namespace TestTracking
{

using namespace CV::Detector::Blob;
using namespace Tracking::Blob;

bool TestMatchingBlob::test(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Blob Matching test:   ---";
	Log::info() << " ";

	allSucceeded = testDetermineBijectiveFeaturesBruteForce(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetermineBijectiveFeaturesBruteForceWithQuality(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Blob Matching test succeeded.";
	}
	else
	{
		Log::info() << "Blob Matching test FAILED";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestMatchingBlob, DetermineBijectiveFeaturesBruteForce_100_500)
{
	Worker worker;
	EXPECT_TRUE(TestMatchingBlob::testDetermineBijectiveFeaturesBruteForce(100u, 500u, GTEST_TEST_DURATION, worker));
}

TEST(TestMatchingBlob, DetermineBijectiveFeaturesBruteForce_1000_1000)
{
	Worker worker;
	EXPECT_TRUE(TestMatchingBlob::testDetermineBijectiveFeaturesBruteForce(1000u, 1000u, GTEST_TEST_DURATION, worker));
}

TEST(TestMatchingBlob, DetermineBijectiveFeaturesBruteForce_1000_10000)
{
	Worker worker;
	EXPECT_TRUE(TestMatchingBlob::testDetermineBijectiveFeaturesBruteForce(1000u, 10000u, GTEST_TEST_DURATION, worker));
}

TEST(TestMatchingBlob, DetermineBijectiveFeaturesBruteForce_1000_50000)
{
	Worker worker;
	EXPECT_TRUE(TestMatchingBlob::testDetermineBijectiveFeaturesBruteForce(1000u, 50000u, GTEST_TEST_DURATION, worker));
}


TEST(TestMatchingBlob, DetermineBijectiveFeaturesBruteForceWithQuality_100_500)
{
	Worker worker;
	EXPECT_TRUE(TestMatchingBlob::testDetermineBijectiveFeaturesBruteForceWithQuality(100u, 500u, GTEST_TEST_DURATION, worker));
}

TEST(TestMatchingBlob, DetermineBijectiveFeaturesBruteForceWithQuality_1000_1000)
{
	Worker worker;
	EXPECT_TRUE(TestMatchingBlob::testDetermineBijectiveFeaturesBruteForceWithQuality(1000u, 1000u, GTEST_TEST_DURATION, worker));
}

TEST(TestMatchingBlob, DetermineBijectiveFeaturesBruteForceWithQuality_1000_10000)
{
	Worker worker;
	EXPECT_TRUE(TestMatchingBlob::testDetermineBijectiveFeaturesBruteForceWithQuality(1000u, 10000u, GTEST_TEST_DURATION, worker));
}

TEST(TestMatchingBlob, DetermineBijectiveFeaturesBruteForceWithQuality_1000_50000)
{
	Worker worker;
	EXPECT_TRUE(TestMatchingBlob::testDetermineBijectiveFeaturesBruteForceWithQuality(1000u, 50000u, GTEST_TEST_DURATION, worker));
}


#endif // OCEAN_USE_GTEST

bool TestMatchingBlob::testDetermineBijectiveFeaturesBruteForce(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test determine features via brute force search:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testDetermineBijectiveFeaturesBruteForce(100u, 500u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testDetermineBijectiveFeaturesBruteForce(1000u, 1000u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testDetermineBijectiveFeaturesBruteForce(1000u, 10000u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testDetermineBijectiveFeaturesBruteForce(1000u, 50000u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testDetermineBijectiveFeaturesBruteForce(2000u, 50000u, testDuration, worker) && allSucceeded;

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

bool TestMatchingBlob::testDetermineBijectiveFeaturesBruteForce(const unsigned int numberForwardFeatures, const unsigned int numberBackwardFeatures, const double testDuration, Worker& worker)
{
	ocean_assert(numberForwardFeatures >= 1u && numberBackwardFeatures >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with " << numberForwardFeatures << " forward and " << numberBackwardFeatures << " backward features:";

	bool allSucceeded = true;

	unsigned int iterations = 0u;

	double sumCorrectMatchesNormal = 0.0;
	double sumFalsePositiveMatchesNormal = 0.0;

	double sumCorrectMatchesEarlyReject = 0.0;
	double sumFalsePositiveMatchesEarlyReject = 0.0;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceSinglecoreNormal;
	HighPerformanceStatistic performanceSinglecoreEarlyReject;

	HighPerformanceStatistic performanceMulticoreNormal;
	HighPerformanceStatistic performanceMulticoreEarlyReject;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performanceNormal = useWorker ? performanceMulticoreNormal : performanceSinglecoreNormal;
		HighPerformanceStatistic& performanceEarlyReject = useWorker ? performanceMulticoreEarlyReject : performanceSinglecoreEarlyReject;

		const Timestamp startTimestamp(true);

		do
		{
			BlobFeatures forwardFeatures;
			BlobFeatures backwardFeatures;
			CorrespondenceMap correspondenceMap;

			createCorrespondences(numberForwardFeatures, numberBackwardFeatures, randomGenerator, forwardFeatures, backwardFeatures, correspondenceMap);
			ocean_assert(!correspondenceMap.empty());

			for (unsigned int i = 0u; i < 2u; ++i)
			{
				if (iterations % 2u == i)
				{
					performanceNormal.start();
					const IndexPairs32 correspondencePairs = UnidirectionalCorrespondences::determineFeatureCorrespondences(forwardFeatures, backwardFeatures, forwardFeatures.size(), Scalar(0.1), useWorker);
					performanceNormal.stop();

					if (correspondencePairs.size() > forwardFeatures.size())
					{
						ocean_assert(false && "This should never happen!");
						allSucceeded = false;
					}

					unsigned int correctMatch = 0u;

					for (const IndexPair32& correspondencePair : correspondencePairs)
					{
						const CorrespondenceMap::const_iterator iC = correspondenceMap.find(correspondencePair.first);

						if (iC != correspondenceMap.cend())
						{
							if (iC->second == correspondencePair.second)
							{
								correctMatch++;
							}
						}
					}

					sumCorrectMatchesNormal += NumericD::ratio(double(correctMatch), double(correspondenceMap.size()));
					sumFalsePositiveMatchesNormal += NumericD::ratio(double(correspondencePairs.size() - correctMatch), double(correspondenceMap.size()));
				}
				else
				{
					performanceEarlyReject.start();
					const IndexPairs32 correspondencePairs = UnidirectionalCorrespondences::determineFeatureCorrespondencesEarlyReject(forwardFeatures, backwardFeatures, forwardFeatures.size(), Scalar(0.1), useWorker);
					performanceEarlyReject.stop();

					if (correspondencePairs.size() > forwardFeatures.size())
					{
						ocean_assert(false && "This should never happen!");
						allSucceeded = false;
					}

					unsigned int correctMatch = 0u;

					for (const IndexPair32& correspondencePair : correspondencePairs)
					{
						const CorrespondenceMap::const_iterator iC = correspondenceMap.find(correspondencePair.first);

						if (iC != correspondenceMap.cend())
						{
							if (iC->second == correspondencePair.second)
							{
								correctMatch++;
							}
						}
					}

					sumCorrectMatchesEarlyReject += NumericD::ratio(double(correctMatch), double(correspondenceMap.size()));
					sumFalsePositiveMatchesEarlyReject += NumericD::ratio(double(correspondencePairs.size() - correctMatch), double(correspondenceMap.size()));
				}
			}

			iterations++;
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	ocean_assert(iterations != 0u);

	Log::info() << "Normal:";
	Log::info() << "Average valid matches: " << String::toAString(sumCorrectMatchesNormal * 100.0 / double(iterations), 2u) << "%";
	Log::info() << "Average false positives: " << String::toAString(sumFalsePositiveMatchesNormal * 100.0 / double(iterations), 2u) << "%";

	Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecoreNormal.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecoreNormal.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecoreNormal.averageMseconds(), 2u) << "ms";

	if (performanceMulticoreNormal.measurements() != 0u)
	{
		Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticoreNormal.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticoreNormal.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticoreNormal.averageMseconds(), 2u) << "ms";
		Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecoreNormal.best() / performanceMulticoreNormal.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecoreNormal.worst() / performanceMulticoreNormal.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecoreNormal.average() / performanceMulticoreNormal.average(), 1u) << "x";
	}

	Log::info() << " ";
	Log::info() << "With early reject:";
	Log::info() << "Average valid matches: " << String::toAString(sumCorrectMatchesEarlyReject * 100.0 / double(iterations), 2u) << "%";
	Log::info() << "Average false positives: " << String::toAString(sumFalsePositiveMatchesEarlyReject * 100.0 / double(iterations), 2u) << "%";

	Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecoreEarlyReject.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecoreEarlyReject.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecoreEarlyReject.averageMseconds(), 2u) << "ms";

	if (performanceMulticoreEarlyReject.measurements() != 0u)
	{
		Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticoreEarlyReject.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticoreEarlyReject.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticoreEarlyReject.averageMseconds(), 2u) << "ms";
		Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecoreEarlyReject.best() / performanceMulticoreEarlyReject.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecoreEarlyReject.worst() / performanceMulticoreEarlyReject.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecoreEarlyReject.average() / performanceMulticoreEarlyReject.average(), 1u) << "x";
	}

	return allSucceeded;
}

bool TestMatchingBlob::testDetermineBijectiveFeaturesBruteForceWithQuality(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test determine bijective features via brute force search with quality:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testDetermineBijectiveFeaturesBruteForceWithQuality(100u, 500u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testDetermineBijectiveFeaturesBruteForceWithQuality(1000u, 1000u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testDetermineBijectiveFeaturesBruteForceWithQuality(1000u, 10000u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testDetermineBijectiveFeaturesBruteForceWithQuality(1000u, 50000u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testDetermineBijectiveFeaturesBruteForceWithQuality(2000u, 50000u, testDuration, worker) && allSucceeded;

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

bool TestMatchingBlob::testDetermineBijectiveFeaturesBruteForceWithQuality(const unsigned int numberForwardFeatures, const unsigned int numberBackwardFeatures, const double testDuration, Worker& worker)
{
	ocean_assert(numberForwardFeatures >= 1u && numberBackwardFeatures >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with " << numberForwardFeatures << " forward and " << numberBackwardFeatures << " backward features:";

	bool allSucceeded = true;

	unsigned int iterations = 0u;

	double sumCorrectMatchesNormal = 0.0;
	double sumFalsePositiveMatchesNormal = 0.0;

	double sumCorrectMatchesEarlyReject = 0.0;
	double sumFalsePositiveMatchesEarlyReject = 0.0;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceSinglecoreNormal;
	HighPerformanceStatistic performanceSinglecoreEarlyReject;

	HighPerformanceStatistic performanceMulticoreNormal;
	HighPerformanceStatistic performanceMulticoreEarlyReject;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performanceNormal = useWorker ? performanceMulticoreNormal : performanceSinglecoreNormal;
		HighPerformanceStatistic& performanceEarlyReject = useWorker ? performanceMulticoreEarlyReject : performanceSinglecoreEarlyReject;

		const Timestamp startTimestamp(true);

		do
		{
			BlobFeatures forwardFeatures;
			BlobFeatures backwardFeatures;
			CorrespondenceMap correspondenceMap;

			createCorrespondences(numberForwardFeatures, numberBackwardFeatures, randomGenerator, forwardFeatures, backwardFeatures, correspondenceMap);
			ocean_assert(!correspondenceMap.empty());

			for (unsigned int i = 0u; i < 2u; ++i)
			{
				if (iterations % 2u == i)
				{
					performanceNormal.start();
					const Correspondences::CorrespondencePairs correspondencePairs = UnidirectionalCorrespondences::determineFeatureCorrespondencesWithQuality(forwardFeatures, backwardFeatures, forwardFeatures.size(), Scalar(0.1), Scalar(0.7), useWorker);
					performanceNormal.stop();

					if (correspondencePairs.size() > forwardFeatures.size())
					{
						ocean_assert(false && "This should never happen!");
						allSucceeded = false;
					}

					unsigned int correctMatch = 0u;

					for (const IndexPair32& correspondencePair : correspondencePairs)
					{
						const CorrespondenceMap::const_iterator iC = correspondenceMap.find(correspondencePair.first);

						if (iC != correspondenceMap.cend())
						{
							if (iC->second == correspondencePair.second)
							{
								correctMatch++;
							}
						}
					}

					sumCorrectMatchesNormal += NumericD::ratio(double(correctMatch), double(correspondenceMap.size()));
					sumFalsePositiveMatchesNormal += NumericD::ratio(double(correspondencePairs.size() - correctMatch), double(correspondenceMap.size()));
				}
				else
				{
					performanceEarlyReject.start();
					const Correspondences::CorrespondencePairs correspondencePairs = UnidirectionalCorrespondences::determineFeatureCorrespondencesWithQualityEarlyReject(forwardFeatures, backwardFeatures, forwardFeatures.size(), Scalar(0.1), Scalar(0.7), useWorker);
					performanceEarlyReject.stop();

					if (correspondencePairs.size() > forwardFeatures.size())
					{
						ocean_assert(false && "This should never happen!");
						allSucceeded = false;
					}

					unsigned int correctMatch = 0u;

					for (const IndexPair32& correspondencePair : correspondencePairs)
					{
						const CorrespondenceMap::const_iterator iC = correspondenceMap.find(correspondencePair.first);

						if (iC != correspondenceMap.cend())
						{
							if (iC->second == correspondencePair.second)
							{
								correctMatch++;
							}
						}
					}

					sumCorrectMatchesEarlyReject += NumericD::ratio(double(correctMatch), double(correspondenceMap.size()));
					sumFalsePositiveMatchesEarlyReject += NumericD::ratio(double(correspondencePairs.size() - correctMatch), double(correspondenceMap.size()));
				}
			}

			iterations++;
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Normal:";
	Log::info() << "Average valid matches: " << String::toAString(sumCorrectMatchesNormal * 100.0 / double(iterations), 2u) << "%";
	Log::info() << "Average false positives: " << String::toAString(sumFalsePositiveMatchesNormal * 100.0 / double(iterations), 2u) << "%";

	Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecoreNormal.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecoreNormal.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecoreNormal.averageMseconds(), 2u) << "ms";

	if (performanceMulticoreNormal.measurements() != 0u)
	{
		Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticoreNormal.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticoreNormal.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticoreNormal.averageMseconds(), 2u) << "ms";
		Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecoreNormal.best() / performanceMulticoreNormal.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecoreNormal.worst() / performanceMulticoreNormal.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecoreNormal.average() / performanceMulticoreNormal.average(), 1u) << "x";
	}

	Log::info() << " ";
	Log::info() << "With early reject:";
	Log::info() << "Average valid matches: " << String::toAString(sumCorrectMatchesEarlyReject * 100.0 / double(iterations), 2u) << "%";
	Log::info() << "Average false positives: " << String::toAString(sumFalsePositiveMatchesEarlyReject * 100.0 / double(iterations), 2u) << "%";

	Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecoreEarlyReject.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecoreEarlyReject.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecoreEarlyReject.averageMseconds(), 2u) << "ms";

	if (performanceMulticoreEarlyReject.measurements() != 0u)
	{
		Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticoreEarlyReject.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticoreEarlyReject.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticoreEarlyReject.averageMseconds(), 2u) << "ms";
		Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecoreEarlyReject.best() / performanceMulticoreEarlyReject.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecoreEarlyReject.worst() / performanceMulticoreEarlyReject.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecoreEarlyReject.average() / performanceMulticoreEarlyReject.average(), 1u) << "x";
	}

	return allSucceeded;
}

void TestMatchingBlob::normalizeDescriptor(CV::Detector::Blob::BlobDescriptor& descriptor)
{
	static_assert(std::is_same<float, BlobDescriptor::DescriptorSSD>::value, "Invalid descriptor type!");

	float sumSqrValues = 0.0f;

	for (unsigned int n = 0u; n < descriptor.elements(); ++n)
	{
		sumSqrValues += descriptor[n] * descriptor[n];
	}

	const float length = NumericF::sqrt(sumSqrValues);
	const float invLength = NumericF::ratio(1.0f, length);

	for (unsigned int n = 0u; n < descriptor.elements(); ++n)
	{
		descriptor[n] *= invLength;
	}
}

void TestMatchingBlob::randomizeDescriptor(CV::Detector::Blob::BlobDescriptor& descriptor, RandomGenerator& randomGenerator)
{
	static_assert(std::is_same<float, BlobDescriptor::DescriptorSSD>::value, "Invalid descriptor type!");

	for (unsigned int n = 0u; n < descriptor.elements(); ++n)
	{
		descriptor[n] = RandomF::scalar(randomGenerator, -1.0f, 1.0f);
	}

	normalizeDescriptor(descriptor);
}

void TestMatchingBlob::slightlyModifiedDescriptor(const CV::Detector::Blob::BlobDescriptor& sourceDescriptor, CV::Detector::Blob::BlobDescriptor& targetDescriptor, RandomGenerator& randomGenerator, const CV::Detector::Blob::BlobDescriptor::DescriptorElement maxElementDelta)
{
	static_assert(std::is_same<float, BlobDescriptor::DescriptorSSD>::value, "Invalid descriptor type!");

	for (unsigned int n = 0u; n < sourceDescriptor.elements(); ++n)
	{
		targetDescriptor[n] = sourceDescriptor[n] + RandomF::scalar(randomGenerator, -maxElementDelta, maxElementDelta);
	}

	normalizeDescriptor(targetDescriptor);
}

void TestMatchingBlob::createCorrespondences(const unsigned int numberForwardFeatures, const unsigned int numberBackwardFeatures, RandomGenerator& randomGenerator, BlobFeatures& forwardFeatures, BlobFeatures& backwardFeatures, CorrespondenceMap& correspondenceMap)
{
	ocean_assert(numberForwardFeatures >= 1u);
	ocean_assert(numberBackwardFeatures >= 1u);

	backwardFeatures.resize(numberBackwardFeatures);
	forwardFeatures.resize(numberForwardFeatures);

	for (BlobFeature& feature : backwardFeatures)
	{
		randomizeDescriptor(feature.descriptor(), randomGenerator);
		feature.setType(BlobFeature::DESCRIPTOR_ORIENTED, BlobFeature::ORIENTATION_SLIDING_WINDOW);
	}

	correspondenceMap.clear();

	for (unsigned int nForward = 0u; nForward < numberForwardFeatures; ++nForward)
	{
		BlobFeature& forwardFeature = forwardFeatures[nForward];

		if (RandomI::random(randomGenerator, 1u) == 0u)
		{
			randomizeDescriptor(forwardFeature.descriptor(), randomGenerator);
		}
		else
		{
			const unsigned int backwardIndex = RandomI::random(randomGenerator, numberBackwardFeatures - 1u);

			slightlyModifiedDescriptor(backwardFeatures[backwardIndex].descriptor(), forwardFeature.descriptor(), randomGenerator, 0.005f);

			correspondenceMap.insert(IndexPair32(nForward, backwardIndex));
		}

		forwardFeature.setType(BlobFeature::DESCRIPTOR_ORIENTED, BlobFeature::ORIENTATION_SLIDING_WINDOW);
	}
}

}

}

}
