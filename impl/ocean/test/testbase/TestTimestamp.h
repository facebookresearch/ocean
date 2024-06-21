/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_TIMESTAMP_H
#define META_OCEAN_TEST_TESTBASE_TEST_TIMESTAMP_H

#include "ocean/test/testbase/TestBase.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements a timestamp test.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestTimestamp
{
	public:

		/**
		 * Tests the accuracy of timestamp.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the precision/resolution of the timestamp implementation.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testResolution(const double testDuration);

		/**
		 * Tests the conversion to milliseconds.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMilliseconds(const double testDuration);

		/**
		 * Tests the conversion to microseconds.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMicroseconds(const double testDuration);

		/**
		 * Tests the conversion to nanoseconds.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNanoseconds(const double testDuration);

		/**
		 * Tests the hasTimePassed() function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testHasTimePassed(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_TIMESTAMP_H
