/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_QUATERNION_H
#define META_OCEAN_TEST_TESTMATH_TEST_QUATERNION_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a quaternion test.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestQuaternion
{
	public:

		/**
		 * Executes all quaternion tests.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the messenger function.
		 * @return True, if succeeded
		 */
		static bool testWriteToMessenger();

		/**
		 * Tests the constructor.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testConstructor(const double testDuration);

		/**
		 * Tests the normalization of a quaternion.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNormalization(const double testDuration);

		/**
		 * Tests the inverting of a quaternion.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testInverting(const double testDuration);

		/**
		 * Tests the conversion from a quaternion to a rotation.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testConversionToRotation(const double testDuration);

		/**
		 * Tests the reference-offset constructor.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testReferenceOffsetConstructor(const double testDuration);

		/**
		 * Tests the calculation of the Quaternion's angle.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar value to be used
		 */
		template <typename T>
		static bool testAngle(const double testDuration);

		/**
		 * Tests the slerp function.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar value to be used
		 */
		template <typename T>
		static bool testSlerp(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_QUATERNION_H
