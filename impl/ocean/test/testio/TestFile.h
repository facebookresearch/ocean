/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTIO_TEST_FILE_H
#define META_OCEAN_TEST_TESTIO_TEST_FILE_H

#include "ocean/test/testio/TestIO.h"

namespace Ocean
{

namespace Test
{

namespace TestIO
{

/**
 * This class implements tests for the Directory class.
 * @ingroup testio
 */
class OCEAN_TEST_IO_EXPORT TestFile
{
	public:

		/**
		 * Invokes all tests.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the exists function.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testFileExists(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTIO_TEST_FILE_H
