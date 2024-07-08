/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_HSVA_COLOR_H
#define META_OCEAN_TEST_TESTMATH_TEST_HSVA_COLOR_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a HSVAColor test.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestHSVAColor
{
	public:

		/**
		 * This functions tests all HSVAColor functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the messenger function.
		 * @return True, if succeeded
		 */
		static bool testWriteToMessenger();

		/**
		 * Tests the isEqual function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testIsEqual(const double testDuration);

		/**
		 * Tests the accessor operators.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAccessors(const double testDuration);

		/**
		 * Tests the conversion to RGBA.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testConversionRGBA(const double testDuration);

		/**
		 * Tests the interpolate function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testInterpolate(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_HSVA_COLOR_H
