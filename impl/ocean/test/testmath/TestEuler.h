/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_EULER_H
#define META_OCEAN_TEST_TESTMATH_TEST_EULER_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class tests the implementation of the Euler class.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestEuler
{
	public:

		/**
		 * Tests all euler functions.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the conversion from an euler rotation to a 3x3 rotation matrix.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testConversionToMatrix(const double testDuration);

		/**
		 * Tests the conversion from an angle-axis rotation to an euler rotation.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testConversionFromRotation(const double testDuration);

		/**
		 * Tests the conversion from a 3x3 rotation matrix to an euler rotation.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testConversionFromMatrix(const double testDuration);

		/**
		 * Tests the decomposition of a 3x3 rotation matrix to a y, x and z angle.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDecomposeRotationMatrixToYXZ(const double testDuration);

		/**
		 * Tests the decomposition of a 3x3 rotation matrix to a x, y and z angle.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDecomposeRotationMatrixToXYZ(const double testDuration);

		/**
		 * Tests the adjustAngles() function.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAdjustAngles(const double testDuration);
};

}

}

}

#endif
