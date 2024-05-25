/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_RGBA_COLOR_H
#define META_OCEAN_TEST_TESTMATH_TEST_RGBA_COLOR_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a RGBAColor test.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestRGBAColor
{
	public:

		/**
		 * This functions tests all RGBAColor functions.
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
		 * Tests the color temperature constructor.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testConstructorColorTemperature(const double testDuration);

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
		 * Tests the conversion between RGBA and HSVA.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testConversionHSVA(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_RGBA_COLOR_H
