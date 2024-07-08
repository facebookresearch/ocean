/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_SUBSET_H
#define META_OCEAN_TEST_TESTBASE_TEST_SUBSET_H

#include "ocean/test/testbase/TestBase.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements tests for the Subset class.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestSubset
{
	public:

		/**
		 * Tests all subset functions.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);
	
		/**
		 * Tests the normal subset function.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSubset(const double testDuration);
	
		/**
		 * Tests the inverted subset function.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testInvertedSubset(const double testDuration);

		/**
		 * Tests the hasIntersectingElement() function.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool hasIntersectingElement(const double testDuration);
	
	protected:

		/**
		 * Tests the subset function for indices.
		 * @param testDuration The number of seconds for the test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam TIndex The data type of an index
		 */
		template <typename TIndex>
		static bool testSubset(const double testDuration);

		/**
		 * Tests the inverted subset function.
		 * @param testDuration The number of seconds for the test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam TIndex The data type of an index
		 */
		template <typename TIndex>
		static bool testInvertedSubset(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_SUBSET_H
