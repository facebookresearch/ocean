/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_RANDOM_I_H
#define META_OCEAN_TEST_TESTBASE_TEST_RANDOM_I_H

#include "ocean/test/testbase/TestBase.h"

#include "ocean/base/RandomGenerator.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements a integer random test.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestRandomI
{
	public:

		/**
		 * Tests the entire integer random functionality.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the random distribution of 32 bit values.
		 * @param randomGenerator Random randomGenerator object
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDistribution32(RandomGenerator& randomGenerator, const double testDuration);

		/**
		 * Tests the random distribution of 64 bit values.
		 * @param randomGenerator Random randomGenerator object
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDistribution64(RandomGenerator& randomGenerator, const double testDuration);

		/**
		 * Tests the random distribution of values with small range.
		 * @param randomGenerator Random randomGenerator object
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDistributionSmallRange(RandomGenerator& randomGenerator, const double testDuration);

		/**
		 * Tests the random distribution of values with large range.
		 * @param randomGenerator Random randomGenerator object
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDistributionLargeRange(RandomGenerator& randomGenerator, const double testDuration);

		/**
		 * Random number generator test producing one unique value.
		 * @param randomGenerator Random randomGenerator object
		 * @return True, if succeeded
		 */
		static bool testOneParameter(RandomGenerator& randomGenerator);

		/**
		 * Random number generator test allowing to create two individual random numbers.
		 * @param randomGenerator Random randomGenerator object
		 * @return True, if succeeded
		 */
		static bool testTwoParameter(RandomGenerator& randomGenerator);

		/**
		 * Random number generator test allowing to create three individual random numbers.
		 * @param randomGenerator Random randomGenerator object
		 * @return True, if succeeded
		 */
		static bool testThreeParameter(RandomGenerator& randomGenerator);

		/**
		 * Random number generator test for several individual random numbers.
		 * @param randomGenerator Random randomGenerator object
		 * @return True, if succeeded
		 */
		static bool testSeveralParameter(RandomGenerator& randomGenerator);

		/**
		 * Random number generator test for pairs.
		 * @param randomGenerator Random randomGenerator object
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRandomPair(RandomGenerator& randomGenerator, const double testDuration);

		/**
		 * Random number generator test for triples.
		 * @param randomGenerator Random randomGenerator object
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRandomTriple(RandomGenerator& randomGenerator, const double testDuration);

		/**
		 * Random number generator test for boolean.
		 * @param randomGenerator Random randomGenerator object
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRandomBoolean(RandomGenerator& randomGenerator, const double testDuration);

		/**
		 * Random element test based on a vector.
		 * @param randomGenerator Random randomGenerator object
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRandomElementsVector(RandomGenerator& randomGenerator, const double testDuration);

		/**
		 * Random element test based on an initializer list.
		 * @param randomGenerator Random randomGenerator object
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRandomElementsInitializerList(RandomGenerator& randomGenerator, const double testDuration);

		/**
		 * Tests extreme value ranges.
		 * @param randomGenerator Random randomGenerator object
		 * @return True, if succeeded
		 */
		static bool testExtremeValueRange(RandomGenerator& randomGenerator);
};

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_RANDOM_I_H
