/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_WORKER_POOL_H
#define META_OCEAN_TEST_TESTBASE_TEST_WORKER_POOL_H

#include "ocean/test/testbase/TestBase.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements tests for the WorkerPool class.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestWorkerPool
{
	public:

		/**
		 * Tests all WorkerPool functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the acquiring of a ScopedWorker object.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testScopedWorker(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_WORKER_POOL_H
