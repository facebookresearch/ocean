/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTIO_TESTSERIALIZATION_TEST_DATA_SERIALIZER_H
#define META_OCEAN_TEST_TESTIO_TESTSERIALIZATION_TEST_DATA_SERIALIZER_H

#include "ocean/test/testio/testserialization/TestIOSerialization.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/io/serialization/DataSerializer.h"

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
 * This class implements a test for the DataSerializer class.
 * @ingroup testioserialization
 */
class OCEAN_TEST_IO_SERIALIZATION_EXPORT TestDataSerializer
{
	public:

		/**
		 * Tests all functions of the DataSerializer class.
		 * @param testDuration The number of seconds for each test
		 * @param selector The test selector
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, const TestSelector& selector);

		/**
		 * Tests the ChannelConfiguration class.
		 * @return True, if succeeded
		 */
		static bool testChannelConfiguration();

		/**
		 * Tests the Channel class.
		 * @return True, if succeeded
		 */
		static bool testChannel();
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTIO_TESTSERIALIZATION_TEST_DATA_SERIALIZER_H
