/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_SINGLETON_H
#define META_OCEAN_TEST_TESTBASE_TEST_SINGLETON_H

#include "ocean/test/testbase/TestBase.h"

#include "ocean/base/Singleton.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements a singleton test.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestSingleton
{
	protected:

		/**
		 * This class implements a helper class to test singleton template.
		 */
		class SingletonUser : public Ocean::Singleton<SingletonUser>
		{
			friend class Ocean::Singleton<SingletonUser>;

			public:

				/**
				 * Returns a value.
				 * @return Return value
				 */
				unsigned int value() const;

			protected:

				/**
				 * Creates a new object.
				 */
				SingletonUser();

				/**
				 * Destructs a value.
				 */
				virtual ~SingletonUser();

			protected:

				/// Object value.
				unsigned int userValue;
		};

	public:

		/**
		 * Tests all singleton functions.
		 * @return True, if succeeded
		 */
		static bool test();

		/**
		 * Tests the get function (creating the instance).
		 * @return True, if succeeded
		 */
		static bool testInstance();
};

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_SINGLETON_H
