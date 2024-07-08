/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_STATIC_VECTOR_H
#define META_OCEAN_TEST_TESTBASE_TEST_STATIC_VECTOR_H

#include "ocean/test/testbase/TestBase.h"

#include "ocean/base/RandomGenerator.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements tests for StaticVector.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestStaticVector
{
	public:

		/**
		 * Invokes all tests.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the constructor functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testConstructor(const double testDuration);

		/**
		 * Tests the access functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAccess(const double testDuration);

		/**
		 * Tests the clear function.
		 v
		 * @return True, if succeeded
		 */
		static bool testClear(const double testDuration);

		/**
		 * Tests the resize functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testResize(const double testDuration);

		/**
		 * Tests the comparison functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testComparison(const double testDuration);

	protected:

		/**
		 * Tests the constructor functions.
		 * @param randomGenerator The random generator to be used
		 * @return True, if succeeded
		 * @tparam T The data type of the buffer
		 * @tparam tCapacity The capacity of the buffer, with range [1, infinity)
		 */
		template <typename T, size_t tCapacity>
		static bool testConstructor(RandomGenerator& randomGenerator);

		/**
		 * Tests the access functions.
		 * @param randomGenerator The random generator to be used
		 * @return True, if succeeded
		 * @tparam T The data type of the buffer
		 * @tparam tCapacity The capacity of the buffer, with range [1, infinity)
		 */
		template <typename T, size_t tCapacity>
		static bool testAccess(RandomGenerator& randomGenerator);

		/**
		 * Tests the clear function.
		 * @param randomGenerator The random generator to be used
		 * @return True, if succeeded
		 * @tparam T The data type of the buffer
		 * @tparam tCapacity The capacity of the buffer, with range [1, infinity)
		 */
		template <typename T, size_t tCapacity>
		static bool testClear(RandomGenerator& randomGenerator);

		/**
		 * Tests the resize functions.
		 * @param randomGenerator The random generator to be used
		 * @return True, if succeeded
		 * @tparam T The data type of the buffer
		 * @tparam tCapacity The capacity of the buffer, with range [1, infinity)
		 */
		template <typename T, size_t tCapacity>
		static bool testResize(RandomGenerator& randomGenerator);

		/**
		 * Tests the comparison functions.
		 * @param randomGenerator The random generator to be used
		 * @return True, if succeeded
		 * @tparam T The data type of the buffer
		 * @tparam tCapacity The capacity of the buffer, with range [1, infinity)
		 */
		template <typename T, size_t tCapacity>
		static bool testComparison(RandomGenerator& randomGenerator);
};

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_STATIC_VECTOR_H
