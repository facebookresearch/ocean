/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestClusteringSpectral.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/ClusteringSpectral.h"
#include "ocean/math/Random.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestClusteringSpectral::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("ClusteringSpectral test");

	Log::info() << " ";

	if (selector.shouldRun("separatedgroups"))
	{
		testResult = testSeparatedGroups(testDuration);

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestClusteringSpectral, SeparatedGroups)
{
	EXPECT_TRUE(TestClusteringSpectral::testSeparatedGroups(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestClusteringSpectral::testSeparatedGroups(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Clustering of well separated groups:";

	// clusterRotation() starts from a random cluster assignment and does not restart, so a single call can end in a local optimum
	constexpr unsigned int attempts = 8u;

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int numberGroups = RandomI::random(randomGenerator, 2u, 4u);

		Indices32 groupIndices;

		for (unsigned int nGroup = 0u; nGroup < numberGroups; ++nGroup)
		{
			const unsigned int numberElements = RandomI::random(randomGenerator, 4u, 10u);

			for (unsigned int nElement = 0u; nElement < numberElements; ++nElement)
			{
				groupIndices.emplace_back(nGroup);
			}
		}

		const Matrix affinityMatrix = createAffinityMatrix(groupIndices, numberGroups, randomGenerator);

		ClusteringSpectral clusteringSpectral(affinityMatrix, numberGroups);

		bool groupsRecovered = false;

		for (unsigned int nAttempt = 0u; nAttempt < attempts && !groupsRecovered; ++nAttempt)
		{
			groupsRecovered = clustersMatchGroups(clusteringSpectral.clusterRotation(), groupIndices, numberGroups);
		}

		OCEAN_EXPECT_TRUE(validation, groupsRecovered);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

Matrix TestClusteringSpectral::createAffinityMatrix(const Indices32& groupIndices, const unsigned int numberGroups, RandomGenerator& randomGenerator)
{
	ocean_assert(!groupIndices.empty() && numberGroups >= 1u);
	ocean_assert_and_suppress_unused(numberGroups >= 1u, numberGroups);

	const size_t size = groupIndices.size();

	Matrix affinityMatrix(size, size);

	for (size_t row = 0; row < size; ++row)
	{
		for (size_t column = row; column < size; ++column)
		{
			// the affinity within a group is two orders of magnitude above the affinity between groups, the weak affinity between groups keeps the graph connected
			const Scalar affinity = groupIndices[row] == groupIndices[column] ? Random::scalar(randomGenerator, Scalar(0.9), Scalar(1)) : Random::scalar(randomGenerator, Scalar(0.001), Scalar(0.01));

			affinityMatrix[row][column] = affinity;
			affinityMatrix[column][row] = affinity;
		}
	}

	return affinityMatrix;
}

bool TestClusteringSpectral::clustersMatchGroups(const std::vector<Indices32>& clusters, const Indices32& groupIndices, const unsigned int numberGroups)
{
	if (clusters.size() != size_t(numberGroups))
	{
		return false;
	}

	// each cluster must hold exactly the elements of one group, while the order of the clusters is arbitrary

	UnorderedIndexSet32 usedGroups;
	size_t numberClusteredElements = 0;

	for (const Indices32& cluster : clusters)
	{
		if (cluster.empty())
		{
			return false;
		}

		const Index32 group = groupIndices[cluster.front()];

		if (!usedGroups.emplace(group).second)
		{
			return false;
		}

		for (const Index32 elementIndex : cluster)
		{
			if (groupIndices[elementIndex] != group)
			{
				return false;
			}
		}

		numberClusteredElements += cluster.size();
	}

	return numberClusteredElements == groupIndices.size();
}

}

}

}
