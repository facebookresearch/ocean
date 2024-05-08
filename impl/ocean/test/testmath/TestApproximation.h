/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_APPROXIMATION_H
#define META_OCEAN_TEST_TESTMATH_APPROXIMATION_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a test of the approximation functions.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestApproximation
{
	public:

		/**
		 * Tests all approximation functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the sqrt function for 8 bit integer values.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSqrt8(const double testDuration);

		/**
		 * Tests the sqrt function for 16 bit integer values.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSqrt16(const double testDuration);

		/**
		 * Tests the sqrt function for 17 bit integer values.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSqrt17(const double testDuration);

		/**
		 * Tests the arc tangent function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAtan2(const double testDuration);

		/**
		 * Tests the exponential function exp(x) = e^x.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the value
		 * @tparam tHigherAccuracy True, to test the higer accuracy; False, to test the lower accuracy
		 */
		template <typename T, bool tHigherAccuracy>
		static bool testExp(const double testDuration);

	private:

		/**
		 * Validates the arc tangent function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool validateAtan2(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_APPROXIMATION_H
