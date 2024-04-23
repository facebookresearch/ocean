// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TEST_TESTIO_TEST_UTILITIES_H
#define META_OCEAN_TEST_TESTIO_TEST_UTILITIES_H

#include "ocean/test/testio/TestIO.h"

namespace Ocean
{

namespace Test
{

namespace TestIO
{

/**
 * This class implements a test for Utilities functions.
 * @ingroup testio
 */
class OCEAN_TEST_IO_EXPORT TestUtilities
{
	public:

		/**
		 * Tests all compression functions.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the read file function.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testReadFile(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTIO_TEST_UTILITIES_H
