/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_RATE_CALCULATOR_H
#define META_OCEAN_TEST_TESTMATH_TEST_RATE_CALCULATOR_H

#include "ocean/test/testmath/TestMath.h"

#include "ocean/test/TestSelector.h"

#include "ocean/base/Timestamp.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements tests for the RateCalculator class.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestRateCalculator
{
	protected:

		/**
		 * Definition of a pair combining a timestamp with a quantity.
		 */
		using TimestampPair = std::pair<Timestamp, double>;

		/**
		 * Definition of a vector holding timestamp pair objects.
		 */
		using TimestampPairs = std::vector<TimestampPair>;

	public:

		/**
		 * Starts all tests for the class.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @param selector The test selector
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker, const TestSelector& selector);

		/**
		 * Tests the rate functionality.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRate(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_RATE_CALCULATOR_H
