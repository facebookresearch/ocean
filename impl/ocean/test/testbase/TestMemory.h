/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_MEMORY_H
#define META_OCEAN_TEST_TESTBASE_TEST_MEMORY_H

#include "ocean/test/testbase/TestBase.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements a memory test.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestMemory
{
	public:

		/**
		 * Tests some memory functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the memory object.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testObject(const double testDuration);

		/**
		 * Tests the memory allocation performance.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testAllocation(const double testDuration, Worker& worker);

		/**
		 * Tests the isInside functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testIsInside(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_MEMORY_H
