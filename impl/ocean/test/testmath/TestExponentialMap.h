// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TEST_TESTMATH_TEST_EXPONENTIAL_MAP_H
#define META_OCEAN_TEST_TESTMATH_TEST_EXPONENTIAL_MAP_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a test for ExponentialMap.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestExponentialMap
{
	public:

		/**
		 * Executes all tests.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the constructors of the ExponentialMap.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testConstructors(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_EXPONENTIAL_MAP_H
