/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTIO_TESTSERIALIZATION_TEST_OUTPUT_DATA_SERIALIZER_H
#define META_OCEAN_TEST_TESTIO_TESTSERIALIZATION_TEST_OUTPUT_DATA_SERIALIZER_H

#include "ocean/test/testio/testserialization/TestIOSerialization.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/io/serialization/OutputDataSerializer.h"

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
 * This class implements a test for the OutputDataSerializer class.
 * @ingroup testioserialization
 */
class OCEAN_TEST_IO_SERIALIZATION_EXPORT TestOutputDataSerializer
{
	public:

		/**
		 * Tests all functions of the OutputDataSerializer class.
		 * @param testDuration The number of seconds for each test
		 * @param selector The test selector
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, const TestSelector& selector);

		/**
		 * Tests adding channels to the serializer.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testAddChannel(const double testDuration);

		/**
		 * Tests adding samples to the serializer.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testAddSample(const double testDuration);

		/**
		 * Tests the start/stop functionality.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testStartStop(const double testDuration);

		/**
		 * Tests writing to a file.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testFileOutput(const double testDuration);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTIO_TESTSERIALIZATION_TEST_OUTPUT_DATA_SERIALIZER_H
