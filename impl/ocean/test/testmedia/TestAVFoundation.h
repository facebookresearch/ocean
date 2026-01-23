/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMEDIA_TEST_AV_FOUNDATION_H
#define META_OCEAN_TEST_TESTMEDIA_TEST_AV_FOUNDATION_H

#include "ocean/test/testmedia/TestMedia.h"

#include "ocean/test/TestSelector.h"

#include "ocean/base/Frame.h"
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
		 * @param selector The test selector
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, const TestSelector& selector);

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

		/**
		 * Tests the video encoder and decoder by encoding frames and decoding them again.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testVideoEncoderDecoder(const double testDuration);

	protected:

		/**
		 * Creates a test frame with a unique pattern based on the frame index.
		 * @param width The width of the frame, in pixel, with range [1, infinity)
		 * @param height The height of the frame, in pixel, with range [1, infinity)
		 * @param frameIndex The index of the frame, used to create a unique pattern
		 * @return The resulting test frame
		 */
		static Frame createTestFrame(const unsigned int width, const unsigned int height, const unsigned int frameIndex);

		/**
		 * Validates that the decoded frame matches the original test frame within acceptable compression tolerances.
		 * @param testFrame The original test frame
		 * @param decodedFrame The decoded frame to validate
		 * @return True, if the frames match within tolerance
		 */
		static bool validateTestFrame(const Frame& testFrame, const Frame& decodedFrame);

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
