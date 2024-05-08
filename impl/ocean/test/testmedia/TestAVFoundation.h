/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMEDIA_TEST_AV_FOUNDATION_H
#define META_OCEAN_TEST_TESTMEDIA_TEST_AV_FOUNDATION_H

#include "ocean/test/testmedia/TestMedia.h"

#include "ocean/base/RandomGenerator.h"

namespace Ocean
{

namespace Test
{

namespace TestMedia
{

/**
 * This class implements a test for the AVFoundation media library.
 * @ingroup testmedia
 */
class OCEAN_TEST_MEDIA_EXPORT TestAVFoundation
{
	public:

		/**
		 * Invokes all tests that are defined.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the pixel buffer accessor for generic pixel formats.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testPixelBufferAccessorGenericPixelFormats(const double testDuration);

		/**
		 * Tests the pixel buffer accessor for non-generic pixel formats.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testPixelBufferAccessorNonGenericPixelFormats(const double testDuration);

	protected:

		/**
		 * Fills a CVPixelBuffer object with random image data.
		 * @param pixelBuffer The pixel buffer to which the random data will be written, must be valid
		 * @param expectedPlanes The number of expected planes of the pixel buffer, with range [1, infinity)
		 * @param randomGenerator The random generator to be used
		 * @param memoryCopy The resulting copy of the random memory which has been set, memory block for each plane of the pixel buffer
		 * @param heights The resulting heights of each individual memory block
		 * @param strideBytes The resulting number of bytes per row for each individual memory block
		 * @return True, if succeeded
		 */
		static bool writeRandomDataToPixelBuffer(void* pixelBuffer, size_t expectedPlanes, RandomGenerator& randomGenerator, std::vector<std::vector<uint8_t>>& memoryCopy, Indices32& heights, Indices32& strideBytes);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMEDIA_TEST_AV_FOUNDATION_H
