/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
	protected:

		/**
		 * Definition of a pair combining two strings.
		 * @tparam The data type of the character to be used
		 */
		template <typename T>
		using StringPair = std::pair<std::basic_string<T>, std::basic_string<T>>;

		/**
		 * Definition of a vector holding string pairs.
		 * @tparam The data type of the character to be used
		 */
		template <typename T>
		using StringPairs = std::vector<StringPair<T>>;

	public:

		/**
		 * Tests the string functionalities.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

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
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testTrim(const double testDuration);

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
