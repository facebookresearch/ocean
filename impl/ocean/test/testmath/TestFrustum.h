/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_FRUSTUM_H
#define META_OCEAN_TEST_TESTMATH_TEST_FRUSTUM_H

#include "ocean/test/testmath/TestMath.h"

#include "ocean/math/Frustum.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements frustum tests.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestFrustum
{
	public:

		/**
		 * Invokes all frustum tests.
		 * @param testDuration Duration of each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the constructors.
		 * @param testDuration Duration of each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testConstructors(const double testDuration);

		/**
		 * Tests isInside() for points.
		 * @param testDuration Duration of each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testIsInsidePoint(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_FRUSTUM_H
