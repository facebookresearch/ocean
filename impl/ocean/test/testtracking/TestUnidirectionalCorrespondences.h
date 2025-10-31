/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTTRACKING_TEST_UNIDIRECTIONAL_CORRESPONDENCES_H
#define META_OCEAN_TEST_TESTTRACKING_TEST_UNIDIRECTIONAL_CORRESPONDENCES_H

#include "ocean/test/testtracking/TestTracking.h"

#include "ocean/base/Utilities.h"

namespace Ocean
{

namespace Test
{

namespace TestTracking
{

/**
 * This class implements tests for the UnidirectionalCorrespondences class.
 * @ingroup testtracking
 */
class OCEAN_TEST_TRACKING_EXPORT TestUnidirectionalCorrespondences
{
	protected:

		/**
		 * Definition of an unordered map mapping indices to counters.
		 */
		using IndexCounterMap = std::unordered_map<Index32, size_t>;

		/**
		 * Definition of an unordered map mapping indices to indices.
		 */
		using IndexConnectionMap = std::unordered_map<Index32, Indices32>;

		/**
		 * Definition of an unordered set holding index pairs.
		 */
		using IndexPairSet = std::unordered_set<IndexPair32, PairHash>;

	public:

		/**
		 * Starts all tests for the class.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the function counting bijective correspondences.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testCountBijectiveCorrespondences(const double testDuration);

		/**
		 * Tests the function removing non-bijective correspondences.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRemoveNonBijectiveCorrespondences(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTTRACKING_TEST_UNIDIRECTIONAL_CORRESPONDENCES_H
