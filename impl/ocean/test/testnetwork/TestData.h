/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTNETWORK_TEST_DATA_H
#define META_OCEAN_TEST_TESTNETWORK_TEST_DATA_H

#include "ocean/test/testnetwork/TestNetwork.h"

#include "ocean/base/RandomGenerator.h"

namespace Ocean
{

namespace Test
{

namespace TestNetwork
{

/**
 * This class implements test for Data.
 * @ingroup testnetwork
 */
class OCEAN_TEST_NETWORK_EXPORT TestData
{
	public:

		/**
		 * Tests all Data functions.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the endian conversion.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testEndian(const double testDuration);

	protected:

		/**
		 * Tests the toBigEndian() function for a specified data type.
		 * @param randomGenerator Random generator to be used
		 * @tparam T The data type to be used
		 */
		template <typename T>
		static bool testToBigEndian(RandomGenerator& randomGenerator);

		/**
		 * Tests the fromBigEndian() function for a specified data type.
		 * @param randomGenerator Random generator to be used
		 * @tparam T The data type to be used
		 */
		template <typename T>
		static bool testFromBigEndian(RandomGenerator& randomGenerator);

		/**
		 * Flips the byte order of a value.
		 * @param value The value for which the byte order will be flipped
		 * @return The value with flipped byte order
		 * @tparam T The data type of the value
		 */
		template <typename T>
		static T flipByteOrder(const T& value);
};

}

}

}

#endif // META_OCEAN_TEST_TESTNETWORK_TEST_DATA_H
