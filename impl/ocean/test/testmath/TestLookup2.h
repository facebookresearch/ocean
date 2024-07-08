/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_LOOKUP_2_H
#define META_OCEAN_TEST_TESTMATH_TEST_LOOKUP_2_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a lookup object test.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestLookup2
{
	public:

		/**
		 * Tests the lookup object.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the bin positions of the center lookup object.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testCenterLookupBinPositions(const double testDuration);

		/**
		 * Tests the clamped interpolation functions of the center lookup object.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testCenterLookupClampedValues(const double testDuration);

		/**
		 * Tests the clamped interpolation functions of the advanced center lookup object.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testAdvancedCenterLookupClampedValues(const double testDuration);

		/**
		 * Tests the nearest neighbor interpolation function of the corner lookup object.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testCornerLookupNearestNeighbor(const double testDuration);

		/**
		 * Tests the bilinear interpolation function of the corner lookup object.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testCornerLookupBilinear(const double testDuration);

		/**
		 * Tests the values bilinear interpolation function of the corner lookup object.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testCornerLookupBilinearValues(const double testDuration);

		/**
		 * Tests the values subset bilinear interpolation function of the corner lookup object.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testCornerLookupBilinearSubsetValues(const double testDuration);

		/**
		 * Tests the clamped interpolation functions of the corner lookup object.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testCornerLookupClampedValues(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_LOOKUP_2_H
