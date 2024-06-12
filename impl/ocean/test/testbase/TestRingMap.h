/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_RING_MAP_H
#define META_OCEAN_TEST_TESTBASE_TEST_RING_MAP_H

#include "ocean/test/testbase/TestBase.h"

#include "ocean/base/RingMap.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implement a test for the ring map class.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestRingMap
{
	protected:

		/**
		 * Definition of a ring map holding strings.
		 */
		typedef RingMapT<unsigned int, std::string, false> StringMap;

	public:

		/**
		 * Tests the ring map class.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the insert function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testInsert(const double testDuration);

		/**
		 * Tests the change capacity function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testChangeCapacity(const double testDuration);

		/**
		 * Tests the check out function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testCheckout(const double testDuration);

			/**
		 * Tests the refresh function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRefresh(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_RING_MAP_H
