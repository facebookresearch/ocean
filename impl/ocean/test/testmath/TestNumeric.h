/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_NUMERIC_H
#define META_OCEAN_TEST_TESTMATH_NUMERIC_H

#include "ocean/test/testmath/TestMath.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a test for the numeric class for the math library.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestNumeric
{
	public:

		/**
		 * Tests numeric math functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the eps() function.
		 * @return True, if succeeded
		 */
		static bool testEps();

		/**
		 * Tests the weakEps() function.
		 * @return True, if succeeded
		 */
		static bool testWeakEps();

		/**
		 * Tests the isEqual function with dynamic epsilon values.
		 * @return True, if succeeded
		 */
		static bool testIsEqualDynamic();

		/**
		 * Tests the round function.
		 * @return True, if succeeded
		 */
		static bool testRound();

		/**
		 * Tests the angle conversion functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type to be used
		 */
		template <typename T>
		static bool testAngleConversion(const double testDuration);

		/**
		 * Tests the angle adjust function with positive range.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAngleAdjustPositive(const double testDuration);

		/**
		 * Tests the angle adjust function with range around the zero angle.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAngleAdjustNull(const double testDuration);

		/**
		 * Tests the is equal function for angles.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAngleIsEqual(const double testDuration);

		/**
		 * Tests the below threshold function for angles.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAngleIsBelowThreshold(const double testDuration);

		/**
		 * Tests the gaussian distribution 1D function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testGaussianDistribution1(const double testDuration);

		/**
		 * Tests the Gaussian distribution 2D function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testGaussianDistribution2(const double testDuration);

		/**
		 * Tests the Gaussian distribution 3D function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testGaussianDistribution3(const double testDuration);

		/**
		 * Tests the abs function.
		 * @return True, if succeeded
		 */
		static bool testAbs();

		/**
		 * Tests the secure abs function.
		 * @return True, if succeeded
		 */
		static bool testSecureAbs();

		/**
		 * Tests the floor function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFloor(const double testDuration);

		/**
		 * Tests the ceil function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testCeil(const double testDuration);

		/**
		 * Tests the log2 function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testLog2(const double testDuration);

		/**
		 * Tests the dot product function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the individual vector elements
		 */
		template <typename T>
		static bool testDotProduct(const double testDuration);

		/**
		 * Tests the sign function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSign(const double testDuration);

		/**
		 * Tests the copySign function.
		 * @return True, if succeeded
		 */
		static bool testCopySign();

		/**
		 * Tests the invertSign function.
		 * @return True, if succeeded
		 */
		static bool testInvertSign();

		/**
		 * Tests the invertSign function.
		 * @return True, if succeeded
		 */
		static bool testFactorial();

		/**
		 * Tests the isNan function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type to be used when testing
		 */
		template <typename T>
		static bool testIsNan(const double testDuration);

		/**
		 * Tests the isInf function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type to be used when testing
		 */
		template <typename T>
		static bool testIsInf(const double testDuration);

		/**
		 * Tests the pow function.
		 * @return True, if succeeded
		 */
		static bool testPow();

		/**
		 * Tests the isInsideValueRange() function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testIsInsideValueRange(const double testDuration);

	private:

		/**
		 * Tests the dot product function with specific vector size.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the individual vector elements
		 * @tparam tSize The size of the vector, with range [1, infinity)
		 */
		template <typename T, size_t tSize>
		static bool testDotProduct(const double testDuration);

		/**
		 * Standard implementation of a dot between two vectors.
		 * @param vectorA The first vector to be used
		 * @param vectorB The second vector to be used
		 * @param size The number of elements each vector holds, with range [0, infinity)
		 * @return The resulting dot product
		 * @tparam T The data type of the elements
		 */
		template <typename T>
		static OCEAN_FORCE_INLINE T dotProduct(const T* vectorA, const T* vectorB, const size_t size);

		/**
		 * Verifies the insideValueRange() function based on a cast to a double value.
		 * @param value The value to be checked
		 * @return True, if succeeded
		 * @tparam T The data type of the value range
		 * @tparam TValue The value
		 */
		template <typename T, typename TValue>
		static bool verifyInsideValueRangeWithCast(const TValue value);
};

template <typename T>
OCEAN_FORCE_INLINE T TestNumeric::dotProduct(const T* vectorA, const T* vectorB, const size_t size)
{
	T result = T(0);

	for (size_t n = 0; n < size; ++n)
	{
		result += vectorA[n] * vectorB[n];
	}

	return result;
}

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_NUMERIC_H
