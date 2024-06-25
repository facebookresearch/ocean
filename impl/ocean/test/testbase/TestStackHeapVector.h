/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_STACK_HEAP_VECTOR_H
#define META_OCEAN_TEST_TESTBASE_TEST_STACK_HEAP_VECTOR_H

#include "ocean/test/testbase/TestBase.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements tests for StackHeapVector.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestStackHeapVector
{
	public:

		/**
		 * Invokes all tests.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the constructor.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testConstructor(const double testDuration);

		/**
		 * Tests the assign function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAssign(const double testDuration);

		/**
		 * Tests push back function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPushBack(const double testDuration);

		/**
		 * Tests resize function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testResize(const double testDuration);

		/**
		 * Tests the performance of the stack heap vector.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPerformance(const double testDuration);

	protected:

		/**
		 * Tests the constructor
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tStackSize Size of the vector's stack memory, with range [1, infinity)
		 */
		template <size_t tStackSize>
		static bool testConstructor(double testDuration);

		/**
		 * Tests the assign function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tStackSize Size of the vector's stack memory, with range [1, infinity)
		 */
		template <size_t tStackSize>
		static bool testAssign(const double testDuration);

		/**
		 * Tests push back function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tStackSize Size of the vector's stack memory, with range [1, infinity)
		 */
		template <size_t tStackSize>
		static bool testPushBack(const double testDuration);

		/**
		 * Tests resize function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tStackSize Size of the vector's stack memory, with range [1, infinity)
		 */
		template <size_t tStackSize>
		static bool testResize(const double testDuration);

		/**
		 * Tests the performance of the stack heap vector.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tStackSize Size of the vector's stack memory, with range [1, infinity)
		 */
		template <size_t tStackSize>
		static bool testPerformance(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_STACK_HEAP_VECTOR_H
