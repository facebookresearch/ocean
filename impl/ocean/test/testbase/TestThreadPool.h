/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_THREAD_POOL_H
#define META_OCEAN_TEST_TESTBASE_TEST_THREAD_POOL_H

#include "ocean/test/testbase/TestBase.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Singleton.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements tests for the TestThreadPool class.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestThreadPool
{
	protected:

		/**
		 * This class implements a helper class allowing to keep track of function executions.
		 */
		class Executions : public Singleton<Executions>
		{
			public:

				/**
				 * Adds a new id.
				 * @param id The id to add
				 */
				void addId(const unsigned int id);

				/**
				 * Returns the added ids.
				 * @return The singleton's ids
				 */
				UnorderedIndexSet32 ids() const;

				/**
				 * Returns whether at least one id exists.
				 * @return True, if so
				 */
				bool hasId() const;

				/**
				 * Clears all ids.
				 */
				void clear();

			protected:

				/// The ids.
				UnorderedIndexSet32 ids_;

				/// The singleton's lock.
				mutable Lock lock_;
		};

	public:

		/**
		 * Tests all TestThreadPool functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests to invoke several functions.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testInvokeFunctions(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_WORKER_POOL_H
