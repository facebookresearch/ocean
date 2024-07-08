/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_HASH_MAP_H
#define META_OCEAN_TEST_TESTBASE_TEST_HASH_MAP_H

#include "ocean/test/testbase/TestBase.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements a hash map test.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestHashMap
{
	public:

		/**
		 * Tests the hash map functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the hash map for single values.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSingleIntegers(const double testDuration);

		/**
		 * Tests the hash map for multiple values.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMultipleIntegers(const double testDuration);
	
	protected:

		/**
		 * Tests the hash map for a given number of elements with a given ratio of occupancy.
		 * @param number Number of elements to be tested
		 * @param occupancy Ratio of occupancy in percent
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPerformanceSingleIntegers(const unsigned int number, const unsigned int occupancy, const double testDuration);

		/**
		 * Validates the hash map with static capacity for a given number of elements with a given ratio of occupancy.
		 * @param number Number of elements to be tested
		 * @param occupancy Ratio of occupancy in percent
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool validationStaticCapacitySingleIntegers(const unsigned int number, const unsigned int occupancy, const double testDuration);

		/**
		 * Validates the hash map with dynamic capacity for a given number of elements with a given initial capacity.
		 * @param number Number of elements to be tested
		 * @param capacity The initial capacity
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool validationDynamicCapacitySingleIntegers(const unsigned int number, const unsigned int capacity, const double testDuration);

		/**
		 * Tests the hash map for a given number of elements with a given ratio of occupancy.
		 * @param number Number of elements to be tested
		 * @param occupancy Ratio of occupancy in percent
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPerformanceMultipleIntegers(const unsigned int number, const unsigned int occupancy, const double testDuration);

		/**
		 * Validates the hash map for a given number of elements with a given ratio of occupancy.
		 * @param number Number of elements to be tested
		 * @param occupancy Ratio of occupancy in percent
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool validationMultipleIntegers(const unsigned int number, const unsigned int occupancy, const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_HASH_MAP_H
