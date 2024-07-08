/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_AUTOMATIC_DIFFERENTIATION_H
#define META_OCEAN_TEST_TESTMATH_TEST_AUTOMATIC_DIFFERENTIATION_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a test for the automatic differentiation functionalities.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestAutomaticDifferentiation
{
	public:

		/**
		 * Tests all automatic differentiation functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the automatic differentiation of simple functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type to be used
		 */
		template <typename T>
		static bool testSimple(const double testDuration);

		/**
		 * Tests the automatic differentiation of mathematic functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type to be used
		 */
		template <typename T>
		static bool testFunctions(const double testDuration);

		/**
		 * Tests the automatic differentiation of nested functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type to be used
		 */
		template <typename T>
		static bool testNested(const double testDuration);

		/**
		 * Tests the automatic differentiation for the homography function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type to be used
		 */
		template <typename T>
		static bool testHomography(const double testDuration);

		/**
		 * Tests the automatic differentiation for the 6-DOF camera pose function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type to be used
		 */
		template <typename T>
		static bool testPose(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_AUTOMATIC_DIFFERENTIATION_H
