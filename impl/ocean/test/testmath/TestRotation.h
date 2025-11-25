/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_ROTATION_H
#define META_OCEAN_TEST_TESTMATH_TEST_ROTATION_H

#include "ocean/test/testmath/TestMath.h"

#include "ocean/test/TestSelector.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a rotation test.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestRotation
{
	public:

		/**
		 * Executes all rotation tests.
		 * @param testDuration Number of seconds for each test
		 * @param selector The test selector
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, const TestSelector& selector);

		/**
		 * Tests the default and basic constructors.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar value to be used
		 */
		template <typename T>
		static bool testConstructors(const double testDuration);

		/**
		 * Tests the matrix constructors (SquareMatrix3 and HomogenousMatrix4).
		 * This test is critical for finding the maximum-finding bug.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar value to be used
		 */
		template <typename T>
		static bool testMatrixConstructors(const double testDuration);

		/**
		 * Tests the array constructor.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar value to be used
		 */
		template <typename T>
		static bool testArrayConstructor(const double testDuration);

		/**
		 * Tests angle normalization (setAngle and constructors).
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar value to be used
		 */
		template <typename T>
		static bool testAngleNormalization(const double testDuration);

		/**
		 * Tests the conversion from a rotation to a quaternion.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar value to be used
		 */
		template <typename T>
		static bool testConversionToQuaterion(const double testDuration);

		/**
		 * Tests the conversion from a rotation to a homogenous matrix.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar value to be used
		 */
		template <typename T>
		static bool testConversionToHomogenousMatrix(const double testDuration);

		/**
		 * Tests the reference-offset constructor.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar value to be used
		 */
		template <typename T>
		static bool testReferenceOffsetConstructor(const double testDuration);

		/**
		 * Tests the left_R_right functions.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar value to be used
		 */
		template <typename T>
		static bool testLeft_R_right(const double testDuration);

		/**
		 * Tests the invert and inverted functions.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar value to be used
		 */
		template <typename T>
		static bool testInversion(const double testDuration);

		/**
		 * Tests the comparison operators (== and !=).
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar value to be used
		 */
		template <typename T>
		static bool testComparisonOperators(const double testDuration);

		/**
		 * Tests the multiplication operators.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar value to be used
		 */
		template <typename T>
		static bool testMultiplicationOperators(const double testDuration);

		/**
		 * Tests the element access operators.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar value to be used
		 */
		template <typename T>
		static bool testElementAccess(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_ROTATION_H
