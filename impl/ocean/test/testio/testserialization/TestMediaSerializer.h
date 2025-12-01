/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTIO_TESTSERIALIZATION_TEST_MEDIA_SERIALIZER_H
#define META_OCEAN_TEST_TESTIO_TESTSERIALIZATION_TEST_MEDIA_SERIALIZER_H

#include "ocean/test/testio/testserialization/TestIOSerialization.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/io/serialization/MediaSerializer.h"

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
 * This class implements a test for the MediaSerializer class.
 * @ingroup testioserialization
 */
class OCEAN_TEST_IO_SERIALIZATION_EXPORT TestMediaSerializer
{
	public:

		/**
		 * Tests all functions of the MediaSerializer class.
		 * @param testDuration The number of seconds for each test
		 * @param selector The test selector
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, const TestSelector& selector);

		/**
		 * Tests the SampleFrame constructor with frame encoding.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testSampleFrameConstructor(const double testDuration);

		/**
		 * Tests the SampleFrame read/write functions.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testSampleFrameReadWrite(const double testDuration);

		/**
		 * Tests the SampleFrame with custom encoded buffer.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testSampleFrameCustomBuffer(const double testDuration);

		/**
		 * Tests the SampleFrame with camera model.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testSampleFrameWithCamera(const double testDuration);

	protected:

		/**
		 * Returns a random frame.
		 * @param randomGenerator The random generator to be used
		 * @return The random frame
		 */
		static Frame randomFrame(RandomGenerator& randomGenerator);

		/**
		 * Checks whether two frames are equal.
		 * @param frameA The first frame to check
		 * @param frameB The second frame to check
		 * @return True, if so
		 */
		static bool isFrameContentEqual(const Frame& frameA, const Frame& frameB);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTIO_TESTSERIALIZATION_TEST_MEDIA_SERIALIZER_H
