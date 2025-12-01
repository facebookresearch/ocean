/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTIO_TESTSERIALIZATION_TEST_DATA_SAMPLE_H
#define META_OCEAN_TEST_TESTIO_TESTSERIALIZATION_TEST_DATA_SAMPLE_H

#include "ocean/test/testio/testserialization/TestIOSerialization.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/io/serialization/DataSample.h"

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
 * This class implements a test for the DataSample class.
 * @ingroup testioserialization
 */
class OCEAN_TEST_IO_SERIALIZATION_EXPORT TestDataSample
{
	public:

		/**
		 * Tests all functions of the DataSample class.
		 * @param testDuration The number of seconds for each test
		 * @param selector The test selector
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, const TestSelector& selector);

		/**
		 * Tests the DataSample construction and basic accessors.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testConstructor(const double testDuration);

		/**
		 * Tests the DataSample playback timestamp configuration.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testPlaybackTimestamp(const double testDuration);

		/**
		 * Tests the DataSample read/write operations.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testReadWrite(const double testDuration);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTIO_TESTSERIALIZATION_TEST_DATA_SAMPLE_H
