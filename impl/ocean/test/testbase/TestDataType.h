/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_DATA_TYPE_H
#define META_OCEAN_TEST_TESTBASE_TEST_DATA_TYPE_H

#include "ocean/test/testbase/TestBase.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements a basic data type test.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestDataType
{
	public:

		/**
		 * Invokes the data type test.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the signed and unsigned char data type.
		 * @return True, if succeeded
		 */
		static bool testChar();

		/**
		 * Tests the shift operator for integer values.
		 * @return True, if succeeded
		 */
		static bool testIntegerShift();

		/**
		 * Tests the arbitrary data types.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testArbitraryDataType(const double testDuration);

		/**
		 * Tests the unsigned typer class.
		 * @return True, if succeeded
		 */
		static bool testUnsignedTyper();

		/**
		 * Tests the absolute difference value typer class.
		 * @return True, if succeeded
		 */
		static bool testAbsoluteDifferenceValueTyper();

		/**
		 * Tests the Float16 data type.
		 * @return True, if succeeded
		 */
		static bool testFloat16(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_DATA_TYPE_H
