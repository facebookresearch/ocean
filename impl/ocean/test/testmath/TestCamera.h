/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_CAMERA_H
#define META_OCEAN_TEST_TESTMATH_CAMERA_H

#include "ocean/test/testmath/TestMath.h"

#include "ocean/test/TestSelector.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements camera tests.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestCamera
{
	public:

		/**
		 * Tests all camera functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param selector The test selector
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, const TestSelector& selector);

		/**
		 * Tests function determining whether an object point lies in front of a camera.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		template <typename T>
		static bool testObjectPointInFront(const double testDuration);

		/**
		 * Tests the conversion of a homogenous matrix from standard coordinate system to an inverted and flipped coordinate system.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		template <typename T>
		static bool testStandard2InvertedFlippedHomogenousMatrix4(const double testDuration);

		/**
		 * Tests the conversion of a 3x3 rotation matrix from standard coordinate system to an inverted and flipped coordinate system.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		template <typename T>
		static bool testStandard2InvertedFlippedSquareMatrix3(const double testDuration);

		/**
		 * Tests the conversion of a quaternion from standard coordinate system to an inverted and flipped coordinate system.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		template <typename T>
		static bool testStandard2InvertedFlippedQuaternion(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_CAMERA_H
