/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_BOX_2_H
#define META_OCEAN_TEST_TESTMATH_TEST_BOX_2_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a test for the Box2 class.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestBox2
{
	public:

		/**
		 * Tests the entire box functionalities.
		 * @param testDuration Duration of each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the constructors.
		 * @param testDuration Duration of each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static bool testConsturctors(const double testDuration);

		/**
		 * Tests the intersects function.
		 * @param testDuration Duration of each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static bool testIntersects(const double testDuration);

		/**
		 * Tests the unsigned box2integer function.
		 * @param testDuration Duration of each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static bool testUnsignedBox2integer(const double testDuration);

		/**
		 * Tests the signed box2integer function.
		 * @param testDuration Duration of each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static bool testSignedBox2integer(const double testDuration);

		/**
		 * Tests the multiplication operators.
		 * @param testDuration Duration of each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static bool testMultiplicationOperators(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_BOX_2_H
