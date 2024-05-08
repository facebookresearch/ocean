/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_KD_TREE_H
#define META_OCEAN_TEST_TESTBASE_TEST_KD_TREE_H

#include "ocean/test/testbase/TestBase.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements a k-d tree test.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestKdTree
{
	public:

		/**
		 * Tests all kd tree functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the nearest neighbor search function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T Scalar type used internally (can be `float` or `double`)
		 */
		template<typename T>
		static bool testNearestNeighborInteger(const double testDuration);

		/**
		 * Tests the radius search function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T Scalar type used internally (can be `float` or `double`)
		 */
		template<typename T>
		static bool testRadiusSearchInteger(const double testDuration);

	private:

		/**
		 * Tests the nearest neighbor search function.
		 * @param number Number of elements
		 * @param dimension Dimensions of an element
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T Scalar type used internally (can be `float` or `double`)
		 */
		template<typename T>
		static bool testNearestNeighborInteger(const unsigned int number, const unsigned int dimension, const double testDuration);

		/**
		 * Tests the radius search function.
		 * @param number Number of elements
		 * @param dimension Dimensions of an element
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T Scalar type used internally (can be `float` or `double`)
		 */
		template<typename T>
		static bool testRadiusSearchInteger(const unsigned int number, const unsigned int dimension, const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_KD_TREE_H
