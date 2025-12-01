/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTIO_TESTSERIALIZATION_TEST_VECTOR_OUTPUT_STREAM_H
#define META_OCEAN_TEST_TESTIO_TESTSERIALIZATION_TEST_VECTOR_OUTPUT_STREAM_H

#include "ocean/test/testio/testserialization/TestIOSerialization.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/io/serialization/VectorOutputStream.h"

#include "ocean/test/TestSelector.h"

namespace Ocean
{

namespace Test
{

namespace TestIO
{

namespace TestSerialization
{

/**
 * This class implements a test for the VectorOutputStream class.
 * @ingroup testioserialization
 */
class OCEAN_TEST_IO_SERIALIZATION_EXPORT TestVectorOutputStream
{
	public:

		/**
		 * Tests all functions of the VectorOutputStream class.
		 * @param testDuration The number of seconds for each test
		 * @param selector The test selector
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, const TestSelector& selector);

		/**
		 * Tests basic write operations.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testBasicWrite(const double testDuration);

		/**
		 * Tests the tellp() function for position tracking.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testTellPosition(const double testDuration);

		/**
		 * Tests seeking to absolute positions.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testSeekAbsolute(const double testDuration);

		/**
		 * Tests seeking with relative offsets (beginning, current, end).
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testSeekRelative(const double testDuration);

		/**
		 * Tests combined write and seek operations.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testWriteAndSeek(const double testDuration);

		/**
		 * Tests overwriting existing data through seeking.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testOverwrite(const double testDuration);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTIO_TESTSERIALIZATION_TEST_VECTOR_OUTPUT_STREAM_H
