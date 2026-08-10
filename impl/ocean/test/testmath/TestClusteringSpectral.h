/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_CLUSTERING_SPECTRAL_H
#define META_OCEAN_TEST_TESTMATH_TEST_CLUSTERING_SPECTRAL_H

#include "ocean/test/testmath/TestMath.h"

#include "ocean/test/TestSelector.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/math/Matrix.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a test for ClusteringSpectral.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestClusteringSpectral
{
	public:

		/**
		 * Executes all tests.
		 * @param testDuration Number of seconds for each test
		 * @param selector The test selector
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, const TestSelector& selector);

		/**
		 * Tests the clustering of a graph which is composed of well separated groups.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSeparatedGroups(const double testDuration);

	protected:

		/**
		 * Creates the affinity matrix of a graph in which the elements of the same group are strongly connected and the elements of different groups are almost unconnected.
		 * @param groupIndices The group to which each element belongs, at least one element
		 * @param numberGroups The number of groups, with range [1, infinity)
		 * @param randomGenerator The random generator to be used
		 * @return The resulting symmetric affinity matrix with non-negative elements
		 */
		static Matrix createAffinityMatrix(const Indices32& groupIndices, const unsigned int numberGroups, RandomGenerator& randomGenerator);

		/**
		 * Returns whether the clusters are exactly the groups of the elements, in any order.
		 * @param clusters The clusters to be checked, each holding the indices of its elements
		 * @param groupIndices The group to which each element belongs, at least one element
		 * @param numberGroups The number of groups, with range [1, infinity)
		 * @return True, if so
		 */
		static bool clustersMatchGroups(const std::vector<Indices32>& clusters, const Indices32& groupIndices, const unsigned int numberGroups);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_CLUSTERING_SPECTRAL_H
