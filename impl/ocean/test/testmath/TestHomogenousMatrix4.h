// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef META_OCEAN_TEST_TESTMATH_TEST_HOMOGENOUS_MATRIX_4_H
#define META_OCEAN_TEST_TESTMATH_TEST_HOMOGENOUS_MATRIX_4_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a test for 4x4 homogenous matrices.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestHomogenousMatrix4
{
	public:

		/**
		 * Tests all homogenous matrix 4x4 functions.
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
		 * Tests the matrix constructor.
		 * @param testDuration Number of test seconds
		 * @return True, if succeeded
		 */
		static bool testConstructor(const double testDuration);

		/**
		 * Tests the element-based constructor.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testElementConstructor(const double testDuration);

		/**
		 * Tests the invert function.
		 * @param testDuration Number of test seconds
		 * @return True, if succeeded
		 */
		static bool testInvert(const double testDuration);

		/**
		 * Tests the decomposition function.
		 * @param testDuration Number of test seconds
		 * @return True, if succeeded
		 */
		static bool testDecomposition(const double testDuration);

		/**
		 * Tests the matrix conversion functions.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testVectorConversion(const double testDuration);
	
		/**
		 * Tests the copy elements functions.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testCopyElements(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_HOMOGENOUS_MATRIX_4_H
