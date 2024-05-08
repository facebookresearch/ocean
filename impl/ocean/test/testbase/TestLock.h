/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_LOCK_H
#define META_OCEAN_TEST_TESTBASE_TEST_LOCK_H

#include "ocean/test/testbase/TestBase.h"

#include "ocean/base/Lock.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class tests lock functions.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestLock
{
	protected:

		/**
		 * Definition of an empty class.
		 */
		class Empty
		{

		};

		/**
		 * Definition of a helper class.
		 */
		template <bool tThreadSafe>
		class Object
		{
			public:

				/**
				 * A function.
				 * @param value A value
				 */
				void function(int& value);

			protected:

				/// The object's lock.
				TemplatedLock<tThreadSafe> lock_;
		};

	public:

		/**
		 * Tests all lock functions.
		 * @return True, if succeeded
		 */
		static bool test();

		/**
		 * Tests the lock and unlock functions.
		 * @return True, if succeeded
		 */
		static bool testLockUnlock();

		/**
		 * Tests the scoped lock object.
		 * @return True, if succeeded
		 */
		static bool testScopedLock();

		/**
		 * Tests a static scoped lock object.
		 * @return True, if succeeded
		 */
		static bool testStaticScopedLock();

		/**
		 * Tests the templated lock object.
		 * @return True, if succeeded
		 */
		static bool testTemplatedLock();

	private:

		/**
		 * Returns the static lock object.
		 * @return Static lock object
		 */
		static Lock& staticLockObject();
};

template <bool tThreadSafe>
void TestLock::Object<tThreadSafe>::function(int& value)
{
	const TemplatedScopedLock scopedLock(lock_);

	value += 1;
}

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_LOCK_H
