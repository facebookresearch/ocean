/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTIO_TEST_COMPRESSION_H
#define META_OCEAN_TEST_TESTIO_TEST_COMPRESSION_H

#include "ocean/test/testio/TestIO.h"

namespace Ocean
{

namespace Test
{

namespace TestIO
{

/**
 * This class implements a compression test.
 * @ingroup testio
 */
class OCEAN_TEST_IO_EXPORT TestCompression
{
	public:

		/**
		 * Tests all compression functions.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the gzip compression and decompression function.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testGzipCompression(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTIO_TEST_COMPRESSION_H
