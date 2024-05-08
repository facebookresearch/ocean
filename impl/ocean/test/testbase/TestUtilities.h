/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_UTILITIES_H
#define META_OCEAN_TEST_TESTBASE_TEST_UTILITIES_H

#include "ocean/test/testbase/TestBase.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements utilities tests.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestUtilities
{
	public:

		/**
		 * Tests the entire utilities functionality.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the min max function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMinmax(const double testDuration);

		/**
		 * Tests the modulo function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testModulo(const double testDuration);

		/**
		 * Tests the ring distance function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRingDistance(const double testDuration);

		/**
		 * Tests the index mirror function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMirrorValue(const double testDuration);

		/**
		 * Tests the division by two function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDivisionBy2(const double testDuration);

		/**
		 * Tests the power of two function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testIsPowerOfTwo(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_UTILITIES_H
