// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TEST_TESTBASE_TEST_STRING_H
#define META_OCEAN_TEST_TESTBASE_TEST_STRING_H

#include "ocean/test/testbase/TestBase.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements a test for the string class.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestString
{
	public:

		/**
		 * Tests the string functionalities.
		 * @return True, if succeeded
		 */
		static bool test();

		/**
		 * Tests the toAString() function.
		 * @return True, if succeeded
		 */
		static bool testToAString();

		/**
		 * Tests the towString() function.
		 * @return True, if succeeded
		 */
		static bool testToWString();

		/**
		 * Tests the boolean value function.
		 * @return True, if succeeded
		 */
		static bool testIsBoolean();

		/**
		 * Tests the integer value function.
		 * @return True, if succeeded
		 */
		static bool testIsInteger32();

		/**
		 * Tests the unsigned integer 64bit value function.
		 * @return True, if succeeded
		 */
		static bool testIsUnsignedInteger64();

		/**
		 * Tests the hex value function for 64 bit.
		 * @return True, if succeeded
		 */
		static bool testIsHexValue64();

		/**
		 * Tests the number value function.
		 * @return True, if succeeded
		 */
		static bool testIsNumber();

		/**
		 * Tests the trim functions.
		 * @return True, if succeeded
		 */
		static bool testTrim();

		/**
		 * Tests the replace functions.
		 * @return True, if succeeded
		 */
		static bool testReplace();

		/**
		 * Tests the to-lower function.
		 * @return True, if succeeded
		 */
		static bool testToLower();

		/**
		 * Tests the to-upper function.
		 * @return True, if succeeded
		 */
		static bool testToUpper();
};

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_STRING_H
