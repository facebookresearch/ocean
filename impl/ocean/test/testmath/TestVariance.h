/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_VARIANCE_H
#define META_OCEAN_TEST_TESTMATH_TEST_VARIANCE_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a test for the Variance class.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestVariance
{
	public:

		/**
		 * Tests all variance functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the average function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The scalar data type to be used
		 */
		template <typename T>
		static bool testAverage(const double testDuration);

		/**
		 * Tests the deviation function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The scalar data type to be used
		 */
		template <typename T>
		static bool testDeviation(const double testDuration);

		/**
		 * Tests the remove function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The scalar data type to be used
		 */
		template <typename T>
		static bool testRemove(const double testDuration);
};

} // namespace TestMath

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTMATH_TEST_VARIANCE_H
