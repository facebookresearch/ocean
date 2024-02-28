// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
		 * @param generator Random generator object
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDistribution32(RandomGenerator& generator, const double testDuration);

		/**
		 * Tests the random distribution of 64 bit values.
		 * @param generator Random generator object
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDistribution64(RandomGenerator& generator, const double testDuration);

		/**
		 * Tests the random distribution of values with small range.
		 * @param generator Random generator object
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDistributionSmallRange(RandomGenerator& generator, const double testDuration);

		/**
		 * Tests the random distribution of values with large range.
		 * @param generator Random generator object
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDistributionLargeRange(RandomGenerator& generator, const double testDuration);

		/**
		 * Random number generator test producing one unique value.
		 * @param generator Random generator object
		 * @return True, if succeeded
		 */
		static bool testOneParameter(RandomGenerator& generator);

		/**
		 * Random number generator test allowing to create two individual random numbers.
		 * @param generator Random generator object
		 * @return True, if succeeded
		 */
		static bool testTwoParameter(RandomGenerator& generator);

		/**
		 * Random number generator test allowing to create three individual random numbers.
		 * @param generator Random generator object
		 * @return True, if succeeded
		 */
		static bool testThreeParameter(RandomGenerator& generator);

		/**
		 * Random number generator test for several individual random numbers.
		 * @param generator Random generator object
		 * @return True, if succeeded
		 */
		static bool testSeveralParameter(RandomGenerator& generator);

		/**
		 * Random number generator test for pairs.
		 * @param generator Random generator object
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRandomPair(RandomGenerator& generator, const double testDuration);

		/**
		 * Random number generator test for triples.
		 * @param generator Random generator object
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRandomTriple(RandomGenerator& generator, const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_RANDOM_I_H
