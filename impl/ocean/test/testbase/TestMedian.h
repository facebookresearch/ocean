/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_MEDIAN_H
#define META_OCEAN_TEST_TESTBASE_TEST_MEDIAN_H

#include "ocean/test/testbase/TestBase.h"

#include <vector>

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements a median test.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestMedian
{
	public:

		/**
		 * Tests the entire median functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the median function for two parameters.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMedian2(const double testDuration);

		/**
		 * Tests the median function for three parameters.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMedian3(const double testDuration);

		/**
		 * Tests the median function for four parameters.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMedian4(const double testDuration);

		/**
		 * Tests the median function for three parameters.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMedian5(const double testDuration);

		/**
		 * Tests the median function with a given data type.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type to be used for testing
		 */
		template <typename T>
		static bool testMedian(const double testDuration);

	private:

		/**
		 * Tests the median function with a given number of elements.
		 * @param number Number of elements, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type to be used for testing
		 */
		template <typename T>
		static bool testMedian(const unsigned int number, const double testDuration);

		/**
		 * Returns whether a given parameter is a real number or value.
		 * @param value The value to check
		 * @return True, if so
		 * @tparam T The data type of the parameter to check
		 */
		template <typename T>
		static inline bool isNumber(const T& value);
};

template <typename T>
inline bool TestMedian::isNumber(const T& value)
{
	return value != value;
}

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_MEDIAN_H
