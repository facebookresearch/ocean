/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testtracking/TestVocabularyTree.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/detector/ORBFeatureDescriptor.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestTracking
{

unsigned int TestVocabularyTree::TypeHelper<TestVocabularyTree::DT_BINARY>::determineDistance(const BinaryDescriptor& descriptorA, const BinaryDescriptor& descriptorB)
{
	static_assert(CV::Detector::ORBDescriptor::descriptorLengthInBits == sizeof(BinaryDescriptor) * 8, "Invalid descriptor!");

	return CV::Detector::ORBFeatureDescriptor::calculateHammingDistance(*(const CV::Detector::ORBDescriptor*)&descriptorA, *(const CV::Detector::ORBDescriptor*)&descriptorB);
}

void TestVocabularyTree::TypeHelper<TestVocabularyTree::DT_BINARY>::randomizeDescriptor(BinaryDescriptor& descriptor, RandomGenerator& randomGenerator)
{
	for (uint8_t& element : descriptor)
	{
		element = uint8_t(RandomI::random(randomGenerator, 255u));
	}
}

TestVocabularyTree::BinaryDescriptor TestVocabularyTree::TypeHelper<TestVocabularyTree::DT_BINARY>::modifyDescriptor(const BinaryDescriptor& descriptor, RandomGenerator& randomGenerator)
{
	constexpr unsigned int descriptorBits = binaryDescriptorElements_ * 8u;

	constexpr unsigned int maxFlippedBits = 20u;
	static_assert(maxFlippedBits <= descriptorBits, "Invalid bits!");

	const unsigned int numberFlippedBits = RandomI::random(randomGenerator, 5u, maxFlippedBits);

	UnorderedIndexSet32 flippedBits;

	while (flippedBits.size() != numberFlippedBits)
	{
		flippedBits.emplace(RandomI::random(randomGenerator, binaryDescriptorElements_ * 8u - 1u));
	}

	BinaryDescriptor modifiedDescriptor(descriptor);

	for (const Index32& flippedBit : flippedBits)
	{
		uint8_t& flippedByte = ((uint8_t*)(&modifiedDescriptor))[flippedBit / 8u];

		const unsigned int flippedBitInByte = (flippedBit % 8u);

		flippedByte ^= 1u << flippedBitInByte;
	}

	ocean_assert(determineDistance(modifiedDescriptor, descriptor) == numberFlippedBits);

	return modifiedDescriptor;
}

std::vector<unsigned int> TestVocabularyTree::TypeHelper<TestVocabularyTree::DT_BINARY>::descriptorEpsilons(const unsigned int numberEpsilons)
{
	ocean_assert(numberEpsilons > 1u);

	constexpr unsigned int descriptorBits = binaryDescriptorElements_ * 8u;

	std::vector<unsigned int> epsilons;
	epsilons.reserve(numberEpsilons);

	for (unsigned int epsIndex = 0u; epsIndex < numberEpsilons; ++epsIndex)
	{
		const unsigned int eps = descriptorBits * epsIndex / 100u;

		epsilons.emplace_back(eps);
	}

	return epsilons;
}

float TestVocabularyTree::TypeHelper<TestVocabularyTree::DT_FLOAT>::determineDistance(const FloatDescriptor& descriptorA, const FloatDescriptor& descriptorB)
{
	float sqrDistance = 0.0f;

	for (size_t n = 0; n < descriptorA.size(); ++n)
	{
		sqrDistance += NumericF::sqr(descriptorA[n] - descriptorB[n]);
	}

	return sqrDistance;
}

void TestVocabularyTree::TypeHelper<TestVocabularyTree::DT_FLOAT>::randomizeDescriptor(FloatDescriptor& descriptor, RandomGenerator& randomGenerator)
{
	for (float& element : descriptor)
	{
		element = RandomF::scalar(randomGenerator, -1.0f, 1.0f);
	}
}

TestVocabularyTree::FloatDescriptor TestVocabularyTree::TypeHelper<TestVocabularyTree::DT_FLOAT>::modifyDescriptor(const FloatDescriptor& descriptor, RandomGenerator& randomGenerator)
{
	constexpr unsigned int maxModifiedElements = 10u;
	static_assert(maxModifiedElements <= floatDescriptorElements_, "Invalid bits!");

	const unsigned int numberModifiedElements = RandomI::random(randomGenerator, 5u, maxModifiedElements);

	UnorderedIndexSet32 modifiedElements;

	while (modifiedElements.size() != numberModifiedElements)
	{
		modifiedElements.emplace(RandomI::random(randomGenerator, floatDescriptorElements_ - 1u));
	}

	FloatDescriptor modifiedDescriptor(descriptor);

	for (const Index32& modifiedElementIndex : modifiedElements)
	{
		float& modifiedElement = modifiedDescriptor[modifiedElementIndex];

		modifiedElement = -modifiedElement;
	}

	ocean_assert(determineDistance(modifiedDescriptor, descriptor) > 0.0);

	return modifiedDescriptor;
}

std::vector<float> TestVocabularyTree::TypeHelper<TestVocabularyTree::DT_FLOAT>::descriptorEpsilons(const unsigned int numberEpsilons)
{
	ocean_assert(numberEpsilons > 1u);

	std::vector<float> epsilons;
	epsilons.reserve(numberEpsilons);

	for (unsigned int epsIndex = 0u; epsIndex < numberEpsilons; ++epsIndex)
	{
		const float eps = float(epsIndex) * 0.5f / float(numberEpsilons - 1u);

		epsilons.emplace_back(eps);
	}

	return epsilons;
}

bool TestVocabularyTree::test(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   TestVocabularyTree test:   ---";
	Log::info() << " ";

	allSucceeded = TestVocabularyTree::testDetermineClustersMeanForBinaryDescriptor(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = TestVocabularyTree::testDetermineClustersMeanForFloatDescriptor(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = TestVocabularyTree::testConstructor<DT_BINARY>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = TestVocabularyTree::testConstructor<DT_FLOAT>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = TestVocabularyTree::testMatchingViaLeafs<DT_BINARY>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = TestVocabularyTree::testMatchingViaLeafs<DT_FLOAT>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = TestVocabularyTree::testMatchingDescriptors<DT_BINARY>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = TestVocabularyTree::testMatchingDescriptors<DT_FLOAT>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = TestVocabularyTree::testMatchingDescriptorsWithForest<DT_BINARY>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = TestVocabularyTree::testMatchingDescriptorsWithForest<DT_FLOAT>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Database test succeeded.";
	}
	else
	{
		Log::info() << "Database test FAILED";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestVocabularyTree, DetermineClustersMeanForBinaryDescriptor)
{
	Worker worker;
	EXPECT_TRUE(TestVocabularyTree::testDetermineClustersMeanForBinaryDescriptor(GTEST_TEST_DURATION, worker));
}

TEST(TestVocabularyTree, DetermineClustersMeanForFloatDescriptor)
{
	Worker worker;
	EXPECT_TRUE(TestVocabularyTree::testDetermineClustersMeanForFloatDescriptor(GTEST_TEST_DURATION, worker));
}

#ifndef OCEAN_DEBUG

TEST(TestVocabularyTree, Constructor_Binary)
{
	Worker worker;
	EXPECT_TRUE((TestVocabularyTree::testConstructor<TestVocabularyTree::DT_BINARY>(GTEST_TEST_DURATION, worker)));
}

TEST(TestVocabularyTree, Constructor_Float)
{
	Worker worker;
	EXPECT_TRUE((TestVocabularyTree::testConstructor<TestVocabularyTree::DT_FLOAT>(GTEST_TEST_DURATION, worker)));
}

TEST(TestVocabularyTree, MatchingViaLeafs_Binary)
{
	Worker worker;
	EXPECT_TRUE((TestVocabularyTree::testMatchingViaLeafs<TestVocabularyTree::DT_BINARY>(GTEST_TEST_DURATION, worker)));
}

TEST(TestVocabularyTree, MatchingViaLeafs_Float)
{
	Worker worker;
	EXPECT_TRUE((TestVocabularyTree::testMatchingViaLeafs<TestVocabularyTree::DT_FLOAT>(GTEST_TEST_DURATION, worker)));
}

TEST(TestVocabularyTree, MatchingDescriptors_Binary)
{
	Worker worker;
	EXPECT_TRUE((TestVocabularyTree::testMatchingDescriptors<TestVocabularyTree::DT_BINARY>(GTEST_TEST_DURATION, worker)));
}

TEST(TestVocabularyTree, MatchingDescriptors_Float)
{
	Worker worker;
	EXPECT_TRUE((TestVocabularyTree::testMatchingDescriptors<TestVocabularyTree::DT_FLOAT>(GTEST_TEST_DURATION, worker)));
}

TEST(TestVocabularyTree, MatchingDescriptorsWithForest_Binary)
{
	Worker worker;
	EXPECT_TRUE((TestVocabularyTree::testMatchingDescriptorsWithForest<TestVocabularyTree::DT_BINARY>(GTEST_TEST_DURATION, worker)));
}

TEST(TestVocabularyTree, MatchingDescriptorsWithForest_Float)
{
	Worker worker;
	EXPECT_TRUE((TestVocabularyTree::testMatchingDescriptorsWithForest<TestVocabularyTree::DT_FLOAT>(GTEST_TEST_DURATION, worker)));
}

#endif // OCEAN_DEBUG

#endif // OCEAN_USE_GTEST

bool TestVocabularyTree::testDetermineClustersMeanForBinaryDescriptor(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test determineClustersMeanForBinaryDescriptor():";

	typedef std::vector<BinaryDescriptor> BinaryDescriptors;

	constexpr unsigned int descriptorBits = sizeof(uint8_t) * binaryDescriptorElements_ * 8u;

	RandomGenerator randomGenerator;
	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int numberClusters = RandomI::random(randomGenerator, 1u, 10u);
		const unsigned int numberDescriptors = RandomI::random(randomGenerator, numberClusters, 10000u);

		BinaryDescriptors binaryDescriptors(numberDescriptors);

		for (BinaryDescriptor& binaryDescriptor : binaryDescriptors)
		{
			TypeHelper<DT_BINARY>::randomizeDescriptor(binaryDescriptor, randomGenerator);
		}

		// now, we create random indices for the descriptors (to get a random order of the descriptors)

		Indices32 straightDescriptorIndices = createIndices<Index32>(numberDescriptors, 0u);

		UnorderedIndexSet32 debugIndices;

		Indices32 descriptorIndices;
		descriptorIndices.reserve(numberDescriptors);
		while (!straightDescriptorIndices.empty())
		{
			const unsigned int index = RandomI::random(randomGenerator, (unsigned int)(straightDescriptorIndices.size()) - 1u);

			debugIndices.emplace(straightDescriptorIndices[index]);

			descriptorIndices.emplace_back(straightDescriptorIndices[index]);
			straightDescriptorIndices[index] = straightDescriptorIndices.back();
			straightDescriptorIndices.pop_back();
		}

		ocean_assert_and_suppress_unused(debugIndices.size() == numberDescriptors, debugIndices);

		// now, we assign each descriptor to a random cluster

		Indices32 clusterIndicesForDescriptor;
		clusterIndicesForDescriptor.reserve(numberDescriptors);
		for (size_t n = 0; n < numberDescriptors; ++n)
		{
			clusterIndicesForDescriptor.emplace_back(RandomI::random(randomGenerator, numberClusters - 1u));
		}

		Worker* useWorker = RandomI::random(randomGenerator, 1u) == 0u ? &worker : nullptr;

		const BinaryDescriptors meanDescriptors = Tracking::VocabularyTree<BinaryDescriptor, unsigned int, TypeHelper<DT_BINARY>::determineDistance>::determineClustersMeanForBinaryDescriptor<descriptorBits>(numberClusters, binaryDescriptors.data(), descriptorIndices.data(), clusterIndicesForDescriptor.data(), binaryDescriptors.size(), useWorker);

		ocean_assert(meanDescriptors.size() == numberClusters);
		if (meanDescriptors.size() != numberClusters)
		{
			allSucceeded = false;
			continue;
		}

		IndexGroups32 clusterDescriptorSums(numberClusters, Indices32(descriptorBits, 0u));
		Indices32 descriptorsPerCluster(numberClusters, 0u);

		for (unsigned int nDescriptor = 0u; nDescriptor < numberDescriptors; ++nDescriptor)
		{
			const Index32& descriptorIndex = descriptorIndices[nDescriptor];
			const BinaryDescriptor& binaryDescriptor = binaryDescriptors[descriptorIndex];

			const Index32& clusterIndex = clusterIndicesForDescriptor[descriptorIndex];

			Indices32& descriptorSum = clusterDescriptorSums[clusterIndex];

			const Indices32 separatedBinaryDescriptor = separateBinaryDescriptor(binaryDescriptor);
			ocean_assert(separatedBinaryDescriptor.size() == descriptorSum.size());

			for (size_t n = 0; n < separatedBinaryDescriptor.size(); ++n)
			{
				descriptorSum[n] += separatedBinaryDescriptor[n];
			}

			descriptorsPerCluster[clusterIndex]++;
		}

		for (unsigned int nCluster = 0u; nCluster < descriptorsPerCluster.size(); ++nCluster)
		{
			Indices32& descriptorSum = clusterDescriptorSums[nCluster];

			if (descriptorsPerCluster[nCluster] != 0u)
			{
				for (size_t n = 0; n < descriptorSum.size(); ++n)
				{
					descriptorSum[n] = (descriptorSum[n] + descriptorsPerCluster[nCluster] / 2u) / descriptorsPerCluster[nCluster];
				}
			}

			const Indices32 separatedMeanDescriptor = separateBinaryDescriptor(meanDescriptors[nCluster]);

			for (size_t n = 0; n < descriptorSum.size(); ++n)
			{
				if (descriptorSum[n] != separatedMeanDescriptor[n])
				{
					allSucceeded = false;
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

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

bool TestVocabularyTree::testDetermineClustersMeanForFloatDescriptor(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test determineClustersMeanForFloatDescriptor():";

	typedef std::vector<FloatDescriptor> FloatDescriptors;

	RandomGenerator randomGenerator;
	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int numberClusters = RandomI::random(randomGenerator, 1u, 10u);
		const unsigned int numberDescriptors = RandomI::random(randomGenerator, numberClusters, 10000u);

		FloatDescriptors floatDescriptors(numberDescriptors);

		for (FloatDescriptor& floatDescriptor : floatDescriptors)
		{
			TypeHelper<DT_FLOAT>::randomizeDescriptor(floatDescriptor, randomGenerator);
		}

		Indices32 straightDescriptorIndices = createIndices<Index32>(numberDescriptors, 0u);

		Indices32 descriptorIndices;
		descriptorIndices.reserve(numberDescriptors);
		while (!straightDescriptorIndices.empty())
		{
			const unsigned int index = RandomI::random(randomGenerator, (unsigned int)(straightDescriptorIndices.size()) - 1u);

			descriptorIndices.emplace_back(straightDescriptorIndices[index]);
			straightDescriptorIndices[index] = straightDescriptorIndices.back();
			straightDescriptorIndices.pop_back();
		}

		Indices32 clusterIndicesForDescriptor;
		clusterIndicesForDescriptor.reserve(numberDescriptors);
		for (size_t n = 0; n < numberDescriptors; ++n)
		{
			clusterIndicesForDescriptor.emplace_back(RandomI::random(randomGenerator, numberClusters - 1u));
		}

		Worker* useWorker = RandomI::random(randomGenerator, 1u) == 0u ? &worker : nullptr;

		const FloatDescriptors meanDescriptors = Tracking::VocabularyTree<FloatDescriptor, float, TypeHelper<DT_FLOAT>::determineDistance>::determineClustersMeanForFloatDescriptor<floatDescriptorElements_>(numberClusters, floatDescriptors.data(), descriptorIndices.data(), clusterIndicesForDescriptor.data(), floatDescriptors.size(), useWorker);

		ocean_assert(meanDescriptors.size() == numberClusters);
		if (meanDescriptors.size() != numberClusters)
		{
			allSucceeded = false;
			continue;
		}

		std::vector<std::vector<float>> clusterDescriptorSums(numberClusters, std::vector<float>(floatDescriptorElements_, 0.0f));
		Indices32 descriptorsPerCluster(numberClusters, 0u);

		for (unsigned int nDescriptor = 0u; nDescriptor < numberDescriptors; ++nDescriptor)
		{
			const Index32& descriptorIndex = descriptorIndices[nDescriptor];
			const FloatDescriptor& floatDescriptor = floatDescriptors[descriptorIndex];

			const Index32& clusterIndex = clusterIndicesForDescriptor[descriptorIndex];

			std::vector<float>& descriptorSum = clusterDescriptorSums[clusterIndex];

			for (size_t n = 0; n < descriptorSum.size(); ++n)
			{
				descriptorSum[n] += floatDescriptor[n];
			}

			descriptorsPerCluster[clusterIndex]++;
		}

		for (unsigned int nCluster = 0u; nCluster < descriptorsPerCluster.size(); ++nCluster)
		{
			std::vector<float>& descriptorSum = clusterDescriptorSums[nCluster];

			if (descriptorsPerCluster[nCluster] != 0u)
			{
				for (size_t n = 0; n < descriptorSum.size(); ++n)
				{
					descriptorSum[n] = float(descriptorSum[n]) / float(descriptorsPerCluster[nCluster]);
				}
			}

			const FloatDescriptor& meanDescriptor = meanDescriptors[nCluster];

			for (size_t n = 0; n < meanDescriptor.size(); ++n)
			{
				if (NumericF::isNotEqual(descriptorSum[n], meanDescriptor[n]))
				{
					allSucceeded = false;
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

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

template <TestVocabularyTree::DescriptorType tDescriptorType>
bool TestVocabularyTree::testConstructor(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

#ifdef OCEAN_USE_GTEST
	constexpr unsigned int benchmarkDescriptorNumber = 500u;
#else
	constexpr unsigned int benchmarkDescriptorNumber = 5000u;
#endif

	using TypeHelper = TypeHelper<tDescriptorType>;

	Log::info() << "Testing constructor of " << TypeHelper::name_ << " Tree with " << benchmarkDescriptorNumber << " descriptors:";

	using Descriptor = typename TypeHelper::Descriptor;
	using Descriptors = typename TypeHelper::Descriptors;
	using VocabularyTree = typename TypeHelper::VocabularyTree;

	RandomGenerator randomGenerator;
	bool allSucceeded = true;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool multicoreItaration : {false, true})
		{
			HighPerformanceStatistic& performance = multicoreItaration ? performanceMulticore : performanceSinglecore;

			for (const bool benchmarkIteration : {false, true})
			{
				const unsigned int numberDescriptors = benchmarkIteration ? benchmarkDescriptorNumber : RandomI::random(randomGenerator, benchmarkDescriptorNumber) + 500u;

				Descriptors descriptors(numberDescriptors);

				for (Descriptor& descriptor : descriptors)
				{
					TypeHelper::randomizeDescriptor(descriptor, randomGenerator);
				}

				Worker* useWorker = multicoreItaration ? &worker : nullptr;

				const typename VocabularyTree::Parameters parameters;

				performance.startIf(benchmarkIteration);
					const VocabularyTree vocabularyTree(descriptors.data(), descriptors.size(), TypeHelper::clusterMeanFunction_, parameters, useWorker, &randomGenerator);
				performance.stopIf(benchmarkIteration);

				UnorderedIndexSet32 descriptorIndexSet;
				descriptorIndexSet.reserve(numberDescriptors);

				typename VocabularyTree::ConstNodes nodes(1, &vocabularyTree);

				while (!nodes.empty())
				{
					const VocabularyTree* node = nodes.back();
					nodes.pop_back();

					if (node->childNodes().empty())
					{
						if (node->descriptorIndices().empty())
						{
							// a leaf node should never be empty
							allSucceeded = false;
						}

						for (const Index32& descriptorIndex : node->descriptorIndices())
						{
							if (descriptorIndexSet.find(descriptorIndex) != descriptorIndexSet.cend())
							{
								allSucceeded = false;
							}

							descriptorIndexSet.emplace(descriptorIndex);
						}
					}
					else
					{
						if (!node->descriptorIndices().empty())
						{
							// a non-leaf node should always be empty
							allSucceeded = false;
						}

						nodes.insert(nodes.cend(), node->childNodes().cbegin(), node->childNodes().cend());
					}
				}

				if (descriptorIndexSet.size() != numberDescriptors)
				{
					// not all descriptors are represented in the tree
					allSucceeded = false;
				}

				for (unsigned int nDescriptor = 0u; nDescriptor < numberDescriptors; ++nDescriptor)
				{
					const Descriptor& descriptor = descriptors[nDescriptor];

					const VocabularyTree* node = &vocabularyTree;

					while (true)
					{
						ocean_assert(node != nullptr);

						bool identicalDistance = false;

						unsigned int bestNodeIndex = (unsigned int)(-1);
						typename TypeHelper::DistanceType bestDistance = NumericT<typename TypeHelper::DistanceType>::maxValue();

						for (unsigned int nNode = 0u; nNode < node->childNodes().size(); ++nNode)
						{
							const typename TypeHelper::DistanceType distance = TypeHelper::determineDistance(descriptor, node->childNodes()[nNode]->nodeDescriptor());

							if (distance < bestDistance)
							{
								bestDistance = distance;
								bestNodeIndex = nNode;

								identicalDistance = false;
							}
							else if (distance == bestDistance)
							{
								identicalDistance = true;
							}
						}

						if (bestNodeIndex != (unsigned int)(-1))
						{
							if (identicalDistance)
							{
								// unfortunately we have more than two matching sub-nodes, therefore we cannot evaluate whether descriptor can be found
								break;
							}

							node = node->childNodes()[bestNodeIndex];
						}
						else
						{
							if (!Ocean::hasElement<Index32>(node->descriptorIndices(), nDescriptor))
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
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(performanceSinglecore.measurements() >= 1u);
	ocean_assert(performanceMulticore.measurements() >= 1u);

	Log::info() << "Performance single-core: " << String::toAString(performanceSinglecore.average(), 2u) << "s";
	Log::info() << "Performance multi-core: " << String::toAString(performanceMulticore.average(), 2u) << "s";

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

template <TestVocabularyTree::DescriptorType tDescriptorType>
bool TestVocabularyTree::testMatchingViaLeafs(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

#ifdef OCEAN_USE_GTEST
	constexpr unsigned int numberDescriptors = 500u;
	constexpr unsigned int numberQueryDescriptors = 50u;
#else
	constexpr unsigned int numberDescriptors = 5000u;
	constexpr unsigned int numberQueryDescriptors = 100u;
#endif

	using TypeHelper = TypeHelper<tDescriptorType>;

	Log::info() << "Testing matching via leafs of " << numberDescriptors << " tree features, with a " << TypeHelper::name_ << " Tree, and " << numberQueryDescriptors << " query features:";

	using Descriptor = typename TypeHelper::Descriptor;
	using Descriptors = typename TypeHelper::Descriptors;
	using VocabularyTree = typename TypeHelper::VocabularyTree;
	using DistanceType = typename TypeHelper::DistanceType;
	using DistanceTypes = std::vector<DistanceType>;

	RandomGenerator randomGenerator;
	bool allSucceeded = true;

	constexpr unsigned int numberEpsilons = 3u;

	const DistanceTypes descriptorEpsilons = TypeHelper::descriptorEpsilons(numberEpsilons);

	unsigned int sumRefindDescriptorBestLeaf = 0u;
	unsigned int sumRefindDescriptorBestLeafs = 0u;

	unsigned int sumQueryDescriptorBestLeaf = 0u;
	unsigned int sumQueryDescriptorBestLeafs[numberEpsilons] = {0u};

	HighPerformanceStatistic performanceRefindBestLeaf;
	HighPerformanceStatistic performanceRefindBestLeafs;

	HighPerformanceStatistic performanceQueryBruteForce;
	HighPerformanceStatistic performanceQueryBestLeaf;
	HighPerformanceStatistic performanceQueryBestLeafs[numberEpsilons];

	const Timestamp startTimestamp(true);

	do
	{
		Descriptors descriptors(numberDescriptors);

		for (Descriptor& descriptor : descriptors)
		{
			TypeHelper::randomizeDescriptor(descriptor, randomGenerator);
		}

		const typename VocabularyTree::Parameters parameters;
		const VocabularyTree vocabularyTree(descriptors.data(), descriptors.size(), TypeHelper::clusterMeanFunction_, parameters, &worker, &randomGenerator);

		{
			// first we check whether we can find the same descriptors again

			std::vector<const Indices32*> bestLeafResults;
			bestLeafResults.reserve(numberDescriptors);

			performanceRefindBestLeaf.start();
				for (unsigned int nDescriptor = 0u; nDescriptor < numberDescriptors; ++nDescriptor)
				{
					bestLeafResults.emplace_back(&vocabularyTree.determineBestLeaf(descriptors[nDescriptor]));
				}
			performanceRefindBestLeaf.stop();

			unsigned int localSumBestLeaf = 0u;

			for (unsigned int nDescriptor = 0u; nDescriptor < numberDescriptors; ++nDescriptor)
			{
				const Indices32& bestLeaf = *bestLeafResults[nDescriptor];

				if (hasElement(bestLeaf, nDescriptor))
				{
					++localSumBestLeaf;
				}
			}

			std::vector<std::vector<const Indices32*>> bestLeafsResults(numberDescriptors);
			for (std::vector<const Indices32*>& bestLeafsResult : bestLeafsResults)
			{
				bestLeafsResult.reserve(4);
			}

			performanceRefindBestLeafs.start();
				for (unsigned int nDescriptor = 0u; nDescriptor < numberDescriptors; ++nDescriptor)
				{
					vocabularyTree.determineBestLeafs(descriptors[nDescriptor], bestLeafsResults[nDescriptor]);
				}
			performanceRefindBestLeafs.stop();

			unsigned int localSumBestLeafs = 0u;

			for (unsigned int nDescriptor = 0u; nDescriptor < numberDescriptors; ++nDescriptor)
			{
				std::vector<const Indices32*>& bestLeafs = bestLeafsResults[nDescriptor];

				bool found = false;

				for (const Indices32* bestLeaf : bestLeafs)
				{
					if (hasElement(*bestLeaf, nDescriptor))
					{
						found = true;
						break;
					}
				}

				if (found)
				{
					++localSumBestLeafs;
				}
			}

			if (localSumBestLeaf < (unsigned int)(double(numberDescriptors) * 0.5))
			{
				allSucceeded = false;
			}
			sumRefindDescriptorBestLeaf += localSumBestLeaf;

			if (localSumBestLeafs < (unsigned int)(double(numberDescriptors) * 0.999))
			{
				// we have the guarantee that we can re-find all descriptors
				allSucceeded = false;
			}
			sumRefindDescriptorBestLeafs += localSumBestLeafs;
		}

		{
			// now we check whether we can find the query descriptors

			Descriptors queryDescriptors;
			queryDescriptors.reserve(numberQueryDescriptors);

			for (size_t n = 0; n < numberQueryDescriptors; ++n)
			{
				const Index32 index = RandomI::random(randomGenerator, numberDescriptors - 1u);

				queryDescriptors.emplace_back(TypeHelper::modifyDescriptor(descriptors[index], randomGenerator));
			}

			IndexGroups32 bruteForceResult;

			{
				// testing brute force

				bruteForceResult.reserve(numberQueryDescriptors);

				performanceQueryBruteForce.start();
					for (unsigned int nQuery = 0u; nQuery < numberQueryDescriptors; ++nQuery)
					{
						const Descriptor& queryDescriptor = queryDescriptors[nQuery];

						Indices32 bestIndices;
						bestIndices.reserve(4);

						DistanceType bestDistance = NumericT<DistanceType>::maxValue();

						for (unsigned int nDescriptor = 0u; nDescriptor < numberDescriptors; ++nDescriptor)
						{
							const DistanceType distance = TypeHelper::determineDistance(queryDescriptor, descriptors[nDescriptor]);

							if (distance < bestDistance)
							{
								bestDistance = distance;

								bestIndices.clear();
								bestIndices.emplace_back(nDescriptor);
							}
							else if (distance == bestDistance)
							{
								bestIndices.emplace_back(nDescriptor);
							}
						}

						bruteForceResult.emplace_back(std::move(bestIndices));
					}
				performanceQueryBruteForce.stop();
			}

			{
				// testing best leaf

				Indices32 bestLeafResult;
				bestLeafResult.reserve(numberQueryDescriptors);

				performanceQueryBestLeaf.start();
					for (unsigned int nQuery = 0u; nQuery < numberQueryDescriptors; ++nQuery)
					{
						const Descriptor& queryDescriptor = queryDescriptors[nQuery];

						const Indices32& candidateIndices = vocabularyTree.determineBestLeaf(queryDescriptor);

						DistanceType bestDistance = NumericT<DistanceType>::maxValue();
						unsigned int bestIndex = (unsigned int)(-1);

						for (const Index32& candidateIndex : candidateIndices)
						{
							const DistanceType distance = TypeHelper::determineDistance(queryDescriptor, descriptors[candidateIndex]);

							if (distance < bestDistance)
							{
								bestDistance = distance;
								bestIndex = candidateIndex;
							}
						}

						ocean_assert(bestIndex != (unsigned int)(-1));
						bestLeafResult.emplace_back(bestIndex);
					}
				performanceQueryBestLeaf.stop();

				for (unsigned int nQuery = 0u; nQuery < numberQueryDescriptors; ++nQuery)
				{
					if (hasElement(bruteForceResult[nQuery], bestLeafResult[nQuery]))
					{
						++sumQueryDescriptorBestLeaf;
					}
				}
			}

			{
				// testing best leafs

				for (unsigned int epsIndex = 0u; epsIndex < numberEpsilons; ++epsIndex)
				{
					Indices32 bestLeafsResult;
					bestLeafsResult.reserve(numberQueryDescriptors);

					std::vector<const Indices32*> bestLeafs;

					const DistanceType eps = descriptorEpsilons[epsIndex];

					performanceQueryBestLeafs[epsIndex].start();
						for (unsigned int nQuery = 0u; nQuery < numberQueryDescriptors; ++nQuery)
						{
							const Descriptor& queryDescriptor = queryDescriptors[nQuery];

							bestLeafs.clear();
							vocabularyTree.determineBestLeafs(queryDescriptor, bestLeafs, eps);

							DistanceType bestDistance = NumericT<DistanceType>::maxValue();
							unsigned int bestIndex = (unsigned int)(-1);

							for (const Indices32* leaf : bestLeafs)
							{
								for (const Index32& candidateIndex : *leaf)
								{
									const DistanceType distance = TypeHelper::determineDistance(queryDescriptor, descriptors[candidateIndex]);

									if (distance < bestDistance)
									{
										bestDistance = distance;
										bestIndex = candidateIndex;
									}
								}
							}

							ocean_assert(bestIndex != (unsigned int)(-1));
							bestLeafsResult.emplace_back(bestIndex);
						}
					performanceQueryBestLeafs[epsIndex].stop();

					for (unsigned int nQuery = 0u; nQuery < numberQueryDescriptors; ++nQuery)
					{
						if (hasElement(bruteForceResult[nQuery], bestLeafsResult[nQuery]))
						{
							++sumQueryDescriptorBestLeafs[epsIndex];
						}
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(performanceQueryBruteForce.measurements() >= 1u);
	Log::info() << "Brute-force Performance: " << String::toAString(performanceQueryBruteForce.average(), 2u) << "s";
	Log::info() << " ";

	ocean_assert(performanceRefindBestLeaf.measurements() >= 1u);
	ocean_assert(performanceRefindBestLeafs.measurements() >= 1u);
	const double refindDescriptorBestLeafPercent = double(sumRefindDescriptorBestLeaf) / double(numberDescriptors * performanceRefindBestLeaf.measurements());
	const double refindDescriptorBestLeafsPercent = double(sumRefindDescriptorBestLeafs) / double(numberDescriptors * performanceRefindBestLeafs.measurements());

	if (refindDescriptorBestLeafPercent < 0.95 || refindDescriptorBestLeafsPercent < 0.95)
	{
		allSucceeded = false;
	}

	Log::info() << "Re-find descriptors, best leaf: Found " << String::toAString(refindDescriptorBestLeafPercent * 100.0, 1u) << "% descriptors";
	Log::info() << "Performance: " << String::toAString(performanceRefindBestLeaf.averageMseconds(), 2u) << "ms";
	Log::info() << "Re-find descriptors, best leafs: Found " << String::toAString(refindDescriptorBestLeafsPercent * 100.0, 1u) << "% descriptors";
	Log::info() << "Performance: " << String::toAString(performanceRefindBestLeaf.averageMseconds(), 2u) << "ms";

	Log::info() << " ";

	ocean_assert(performanceQueryBestLeaf.measurements() >= 1u);
	const double queryDescriptorBestLeafPercent = double(sumQueryDescriptorBestLeaf) / double(numberQueryDescriptors * performanceQueryBestLeaf.measurements());

	if (queryDescriptorBestLeafPercent < 0.15)
	{
		allSucceeded = false;
	}

	Log::info() << "Find query descriptors, best leaf: Found " << String::toAString(queryDescriptorBestLeafPercent * 100.0, 1u) << "% descriptors";
	Log::info() << "Performance: " << String::toAString(performanceQueryBestLeaf.averageMseconds(), 2u) << "ms";

	Log::info() << " ";

	for (unsigned int epsIndex = 0u; epsIndex < numberEpsilons; ++epsIndex)
	{
		ocean_assert(performanceQueryBestLeafs[epsIndex].measurements() >= 1u);
		const double queryDescriptorBestLeafsPercent = double(sumQueryDescriptorBestLeafs[epsIndex]) / double(numberQueryDescriptors * performanceQueryBestLeafs[epsIndex].measurements());

		if (queryDescriptorBestLeafsPercent < 0.2)
		{
			allSucceeded = false;
		}

		Log::info() << "Find query descriptors, best leafs with epsilon " << descriptorEpsilons[epsIndex] << ": Found " << String::toAString(queryDescriptorBestLeafsPercent * 100.0, 1u) << "% descriptors";
		Log::info() << "Performance: " << String::toAString(performanceQueryBestLeafs[epsIndex].averageMseconds(), 2u) << "ms";
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

template <TestVocabularyTree::DescriptorType tDescriptorType>
bool TestVocabularyTree::testMatchingDescriptors(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

#ifdef OCEAN_USE_GTEST
	constexpr unsigned int numberDescriptors = 500u;
	constexpr unsigned int numberQueryDescriptors = 50u;
#else
	constexpr unsigned int numberDescriptors = 5000u;
	constexpr unsigned int numberQueryDescriptors = 100u;
#endif

	using TypeHelper = TypeHelper<tDescriptorType>;

	Log::info() << "Test matching with " << numberDescriptors << " descriptor tree features, with a " << TypeHelper::name_ << " Tree, and " << numberQueryDescriptors << " query features:";

	using Descriptor = typename TypeHelper::Descriptor;
	using Descriptors = typename TypeHelper::Descriptors;
	using VocabularyTree = typename TypeHelper::VocabularyTree;
	using DistanceType = typename TypeHelper::DistanceType;

	RandomGenerator randomGenerator;
	bool allSucceeded = true;

	constexpr unsigned int numberStages = 3u;

	unsigned int sumQueryDescriptorBestLeaf = 0u;
	unsigned int sumQueryDescriptorBestLeafs[numberStages] = {0u};

	HighPerformanceStatistic performanceQueryBruteForce;
	HighPerformanceStatistic performanceQueryBestLeaf;
	HighPerformanceStatistic performanceQueryBestLeafs[numberStages];

	const Timestamp startTimestamp(true);

	do
	{
		Descriptors descriptors(numberDescriptors);

		for (Descriptor& descriptor : descriptors)
		{
			TypeHelper::randomizeDescriptor(descriptor, randomGenerator);
		}

		const typename VocabularyTree::Parameters parameters;
		const VocabularyTree vocabularyTree(descriptors.data(), descriptors.size(), TypeHelper::clusterMeanFunction_, parameters, &worker, &randomGenerator);

		Descriptors queryDescriptors;
		queryDescriptors.reserve(numberQueryDescriptors);

		for (size_t n = 0; n < numberQueryDescriptors; ++n)
		{
			const Index32 index = RandomI::random(randomGenerator, numberDescriptors - 1u);

			queryDescriptors.emplace_back(TypeHelper::modifyDescriptor(descriptors[index], randomGenerator));
		}

		IndexGroups32 bruteForceResult;

		{
			// testing brute force

			bruteForceResult.reserve(numberQueryDescriptors);

			performanceQueryBruteForce.start();
				for (unsigned int nQuery = 0u; nQuery < numberQueryDescriptors; ++nQuery)
				{
					const Descriptor& queryDescriptor = queryDescriptors[nQuery];

					Indices32 bestIndices;
					bestIndices.reserve(4);

					DistanceType bestDistance = NumericT<DistanceType>::maxValue();

					for (unsigned int nDescriptor = 0u; nDescriptor < numberDescriptors; ++nDescriptor)
					{
						const DistanceType distance = TypeHelper::determineDistance(queryDescriptor, descriptors[nDescriptor]);

						if (distance < bestDistance)
						{
							bestDistance = distance;

							bestIndices.clear();
							bestIndices.emplace_back(nDescriptor);
						}
						else if (distance == bestDistance)
						{
							bestIndices.emplace_back(nDescriptor);
						}
					}

					bruteForceResult.emplace_back(std::move(bestIndices));
				}
			performanceQueryBruteForce.stop();
		}

		{
			// testing best leaf

			performanceQueryBestLeaf.start();
				typename VocabularyTree::Matches matches;
				vocabularyTree.template matchDescriptors<VocabularyTree::MM_FIRST_BEST_LEAF>(descriptors.data(), queryDescriptors.data(), queryDescriptors.size(), NumericT<DistanceType>::maxValue(), matches, &worker);
			performanceQueryBestLeaf.stop();

			for (const typename VocabularyTree::Match& match : matches)
			{
				if (hasElement(bruteForceResult[match.queryDescriptorIndex()], match.candidateDescriptorIndex()))
				{
					++sumQueryDescriptorBestLeaf;
				}
			}
		}

		{
			// testing best leafs

			for (unsigned int stageIndex = 0u; stageIndex < numberStages; ++stageIndex)
			{
				performanceQueryBestLeafs[stageIndex].start();

					typename VocabularyTree::Matches matches;

					switch (stageIndex)
					{
						case 0u:
							vocabularyTree.template matchDescriptors<VocabularyTree::MM_ALL_BEST_LEAFS>(descriptors.data(), queryDescriptors.data(), queryDescriptors.size(), NumericT<DistanceType>::maxValue(), matches, &worker);
							break;

						case 1u:
							vocabularyTree.template matchDescriptors<VocabularyTree::MM_ALL_GOOD_LEAFS_1>(descriptors.data(), queryDescriptors.data(), queryDescriptors.size(), NumericT<DistanceType>::maxValue(), matches, &worker);
							break;

						case 2u:
							vocabularyTree.template matchDescriptors<VocabularyTree::MM_ALL_GOOD_LEAFS_2>(descriptors.data(), queryDescriptors.data(), queryDescriptors.size(), NumericT<DistanceType>::maxValue(), matches, &worker);
							break;

						default:
							assert(false && "Invalid stage index");
					}
				performanceQueryBestLeafs[stageIndex].stop();

				for (const typename VocabularyTree::Match& match : matches)
				{
					if (hasElement(bruteForceResult[match.queryDescriptorIndex()], match.candidateDescriptorIndex()))
					{
						++sumQueryDescriptorBestLeafs[stageIndex];
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(performanceQueryBruteForce.measurements() >= 1u);
	Log::info() << "Brute-force Performance: " << String::toAString(performanceQueryBruteForce.average(), 2u) << "s";
	Log::info() << " ";

	ocean_assert(performanceQueryBestLeaf.measurements() >= 1u);
	const double queryDescriptorBestLeafPercent = double(sumQueryDescriptorBestLeaf) / double(numberQueryDescriptors * performanceQueryBestLeaf.measurements());
	if (queryDescriptorBestLeafPercent < 0.15)
	{
		allSucceeded = false;
	}

	Log::info() << "Find query descriptors, best leaf: Found " << String::toAString(queryDescriptorBestLeafPercent * 100.0, 1u) << "% descriptors";
	Log::info() << "Performance: " << String::toAString(performanceQueryBestLeaf.averageMseconds(), 2u) << "ms";

	Log::info() << " ";

	for (unsigned int stageIndex = 0u; stageIndex < numberStages; ++stageIndex)
	{
		ocean_assert(performanceQueryBestLeafs[stageIndex].measurements() >= 1u);
		const double queryDescriptorBestLeafsPercent = double(sumQueryDescriptorBestLeafs[stageIndex]) / double(numberQueryDescriptors * performanceQueryBestLeafs[stageIndex].measurements());

		if (queryDescriptorBestLeafsPercent < 0.2)
		{
			allSucceeded = false;
		}

		Log::info() << "Find query descriptors, best leafs in stage " << stageIndex << ": Found " << String::toAString(queryDescriptorBestLeafsPercent * 100.0, 1u) << "% descriptors";
		Log::info() << "Performance: " << String::toAString(performanceQueryBestLeafs[stageIndex].averageMseconds(), 2u) << "ms";
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

template <TestVocabularyTree::DescriptorType tDescriptorType>
bool TestVocabularyTree::testMatchingDescriptorsWithForest(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

#ifdef OCEAN_USE_GTEST
	constexpr unsigned int numberDescriptors = 500u;
	constexpr unsigned int numberQueryDescriptors = 50u;
	constexpr unsigned int numberTrees = 4u;
#else
	constexpr unsigned int numberDescriptors = 5000u;
	constexpr unsigned int numberQueryDescriptors = 100u;
	constexpr unsigned int numberTrees = 4u;
#endif

	using TypeHelper = TypeHelper<tDescriptorType>;

	Log::info() << "Test forest matching with " << numberTrees << " trees and " << numberDescriptors << " descriptor tree features, with a " << TypeHelper::name_ << " Tree, and " << numberQueryDescriptors << " query features:";

	using Descriptor = typename TypeHelper::Descriptor;
	using Descriptors = typename TypeHelper::Descriptors;
	using DistanceType = typename TypeHelper::DistanceType;
	using VocabularyForest = Tracking::VocabularyForest<Descriptor, DistanceType, TypeHelper::determineDistance>;

	RandomGenerator randomGenerator;
	bool allSucceeded = true;

	constexpr unsigned int numberStages = 3u;

	unsigned int sumQueryDescriptorBestLeaf = 0u;
	unsigned int sumQueryDescriptorBestLeafs[numberStages] = {0u};

	HighPerformanceStatistic performanceQueryBruteForce;
	HighPerformanceStatistic performanceQueryBestLeaf;
	HighPerformanceStatistic performanceQueryBestLeafs[numberStages];

	const Timestamp startTimestamp(true);

	do
	{
		Descriptors descriptors(numberDescriptors);

		for (Descriptor& descriptor : descriptors)
		{
			TypeHelper::randomizeDescriptor(descriptor, randomGenerator);
		}

		const typename VocabularyForest::Parameters parameters;
		const VocabularyForest vocabularyForest(numberTrees, descriptors.data(), descriptors.size(), TypeHelper::clusterMeanFunction_, parameters, &worker, &randomGenerator);

		Descriptors queryDescriptors;
		queryDescriptors.reserve(numberQueryDescriptors);

		for (size_t n = 0; n < numberQueryDescriptors; ++n)
		{
			const Index32 index = RandomI::random(randomGenerator, numberDescriptors - 1u);

			queryDescriptors.emplace_back(TypeHelper::modifyDescriptor(descriptors[index], randomGenerator));
		}

		IndexGroups32 bruteForceResult;

		{
			// testing brute force

			bruteForceResult.reserve(numberQueryDescriptors);

			performanceQueryBruteForce.start();
				for (unsigned int nQuery = 0u; nQuery < numberQueryDescriptors; ++nQuery)
				{
					const Descriptor& queryDescriptor = queryDescriptors[nQuery];

					Indices32 bestIndices;
					bestIndices.reserve(4);

					DistanceType bestDistance = NumericT<DistanceType>::maxValue();

					for (unsigned int nDescriptor = 0u; nDescriptor < numberDescriptors; ++nDescriptor)
					{
						const DistanceType distance = TypeHelper::determineDistance(queryDescriptor, descriptors[nDescriptor]);

						if (distance < bestDistance)
						{
							bestDistance = distance;

							bestIndices.clear();
							bestIndices.emplace_back(nDescriptor);
						}
						else if (distance == bestDistance)
						{
							bestIndices.emplace_back(nDescriptor);
						}
					}

					bruteForceResult.emplace_back(std::move(bestIndices));
				}
			performanceQueryBruteForce.stop();
		}

		{
			// testing best leaf

			performanceQueryBestLeaf.start();
				typename VocabularyForest::Matches matches;
				vocabularyForest.template matchDescriptors<VocabularyForest::MM_FIRST_BEST_LEAF>(descriptors.data(), queryDescriptors.data(), queryDescriptors.size(), NumericT<DistanceType>::maxValue(), matches, &worker);
			performanceQueryBestLeaf.stop();

			for (const typename VocabularyForest::Match& match : matches)
			{
				if (hasElement(bruteForceResult[match.queryDescriptorIndex()], match.candidateDescriptorIndex()))
				{
					++sumQueryDescriptorBestLeaf;
				}
			}
		}

		{
			// testing best leafs

			for (unsigned int stageIndex = 0u; stageIndex < numberStages; ++stageIndex)
			{
				performanceQueryBestLeafs[stageIndex].start();

					typename VocabularyForest::Matches matches;

					switch (stageIndex)
					{
						case 0u:
							vocabularyForest.template matchDescriptors<VocabularyForest::MM_ALL_BEST_LEAFS>(descriptors.data(), queryDescriptors.data(), queryDescriptors.size(), NumericT<DistanceType>::maxValue(), matches, &worker);
							break;

						case 1u:
							vocabularyForest.template matchDescriptors<VocabularyForest::MM_ALL_GOOD_LEAFS_1>(descriptors.data(), queryDescriptors.data(), queryDescriptors.size(), NumericT<DistanceType>::maxValue(), matches, &worker);
							break;

						case 2u:
							vocabularyForest.template matchDescriptors<VocabularyForest::MM_ALL_GOOD_LEAFS_2>(descriptors.data(), queryDescriptors.data(), queryDescriptors.size(), NumericT<DistanceType>::maxValue(), matches, &worker);
							break;

						default:
							assert(false && "Invalid stage index");
					}
				performanceQueryBestLeafs[stageIndex].stop();

				for (const typename VocabularyForest::Match& match : matches)
				{
					if (hasElement(bruteForceResult[match.queryDescriptorIndex()], match.candidateDescriptorIndex()))
					{
						++sumQueryDescriptorBestLeafs[stageIndex];
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(performanceQueryBruteForce.measurements() >= 1u);
	Log::info() << "Brute-force Performance: " << String::toAString(performanceQueryBruteForce.average(), 2u) << "s";
	Log::info() << " ";

	ocean_assert(performanceQueryBestLeaf.measurements() >= 1u);
	const double queryDescriptorBestLeafPercent = double(sumQueryDescriptorBestLeaf) / double(numberQueryDescriptors * performanceQueryBestLeaf.measurements());
	if (queryDescriptorBestLeafPercent < 0.50)
	{
		allSucceeded = false;
	}

	Log::info() << "Find query descriptors, best leaf: Found " << String::toAString(queryDescriptorBestLeafPercent * 100.0, 1u) << "% descriptors";
	Log::info() << "Performance: " << String::toAString(performanceQueryBestLeaf.averageMseconds(), 2u) << "ms";

	Log::info() << " ";

	for (unsigned int stageIndex = 0u; stageIndex < numberStages; ++stageIndex)
	{
		ocean_assert(performanceQueryBestLeafs[stageIndex].measurements() >= 1u);
		const double queryDescriptorBestLeafsPercent = double(sumQueryDescriptorBestLeafs[stageIndex]) / double(numberQueryDescriptors * performanceQueryBestLeafs[stageIndex].measurements());

		if (queryDescriptorBestLeafsPercent < 0.60)
		{
			allSucceeded = false;
		}

		Log::info() << "Find query descriptors, best leafs in stage " << stageIndex << ": Found " << String::toAString(queryDescriptorBestLeafsPercent * 100.0, 1u) << "% descriptors";
		Log::info() << "Performance: " << String::toAString(performanceQueryBestLeafs[stageIndex].averageMseconds(), 2u) << "ms";
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

Indices32 TestVocabularyTree::separateBinaryDescriptor(const BinaryDescriptor& descriptor)
{
	Indices32 result(descriptor.size() * 8, 0u);

	unsigned int index = 0u;

	for (const uint8_t& element : descriptor)
	{
		for (unsigned int n = 0u; n < 8u; ++n)
		{
			if ((element & (1u << n)))
			{
				result[index] = 1u;
			}

			++index;
		}
	}

	return result;
}

}

}

}
