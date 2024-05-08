/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TESTBINARY_H
#define META_OCEAN_TEST_TESTBASE_TESTBINARY_H

#include "ocean/test/testbase/TestBase.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements a test for the Binary class.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestBinary
{
	public:

		/**
		 * Invokes all tests.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the hide string function.
		 * @return True, if succeeded
		 */
		static bool testHideString();
};

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TESTBINARY_H
