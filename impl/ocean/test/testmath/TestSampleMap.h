// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TEST_TESTMATH_TEST_SAMPLE_MAP_H
#define META_OCEAN_TEST_TESTMATH_TEST_SAMPLE_MAP_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements tests for the SampleMap class.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestSampleMap
{
	public:

		/**
		 * Starts all tests for the SampleMap class.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the sample functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSample(const double testDuration);

		/**
		 * Applies a stress test.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testStressTest(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_SAMPLE_MAP_H
