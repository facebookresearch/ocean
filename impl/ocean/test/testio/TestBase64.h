/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTIO_TEST_BASE64_H
#define META_OCEAN_TEST_TESTIO_TEST_BASE64_H

#include "ocean/test/testio/TestIO.h"

namespace Ocean
{

namespace Test
{

namespace TestIO
{

/**
 * This class implements a base64 test.
 * @ingroup testio
 */
class OCEAN_TEST_IO_EXPORT TestBase64
{
	public:

		/**
		 * Tests all base64 functions.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the encoding and decoding functions.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testEncodingDecoding(const double testDuration);
};

} // namespace TestIO

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTIO_TEST_BASE64_H
