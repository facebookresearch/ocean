/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_STL_H
#define META_OCEAN_TEST_TESTBASE_TEST_STL_H

#include "ocean/test/testbase/TestBase.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements tests for c++ functions.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestSTL
{
	public:

		/**
		 * Tests all c++ functions.
		 * @return True, if succeeded
		 */
		static bool testSTL();

	private:

		/**
		 * Event function for at std::atexit events.
		 */
		static void onAtExit();
};

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_STL_H
