/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTIO_TEST_BITSTREAM_H
#define META_OCEAN_TEST_TESTIO_TEST_BITSTREAM_H

#include "ocean/test/testio/TestIO.h"

#include "ocean/io/Bitstream.h"

namespace Ocean
{

namespace Test
{

namespace TestIO
{

/**
 * This class implements a test for the Bitstream class.
 * @ingroup testio
 */
class OCEAN_TEST_IO_EXPORT TestBitstream
{
	public:

		/**
		 * Tests all functions of the Bitstream class.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the input and output bitstream functions.
		 * @return True, if succeeded
		 */
		 static bool testInputOutputBitstream();

		/**
		 * Reads one value from an input stream and checks whether the value matches with a given value.
		 * @param inputStream The input stream
		 * @param expectedValue The expected value
		 * @return True, if succeeded
		 * @tparam T The data type of the value to be read
		 */
		template <typename T>
		static bool readValue(IO::InputBitstream& inputStream, const T& expectedValue);
};

template <typename T>
bool TestBitstream::readValue(IO::InputBitstream& inputStream, const T& expectedValue)
{
	ocean_assert(inputStream);

	T value;
	if (!inputStream.read<T>(value))
	{
		return false;
	}

	return value == expectedValue;
}

}

}

}

#endif // META_OCEAN_TEST_TESTIO_TEST_BITSTREAM_H
