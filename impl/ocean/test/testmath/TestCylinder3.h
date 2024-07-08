/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_CYLINDER_3_H
#define META_OCEAN_TEST_TESTMATH_TEST_CYLINDER_3_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a test for the 3D cylinder.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestCylinder3
{
	public:

		/**
		 * Tests all cylinder functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the Cylinder3 class constructors.
		 * @return True, if succeeded
		 */
		static bool testConstructor();

		/**
		 * Tests the cylinder-ray intersection function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		template <typename T>
		static bool testNearestIntersection(const double testDuration);
};

} // namespace TestMath

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTMATH_TEST_CYLINDER_3_H
