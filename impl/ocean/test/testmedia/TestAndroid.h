/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMEDIA_TEST_ANDROID_H
#define META_OCEAN_TEST_TESTMEDIA_TEST_ANDROID_H

#include "ocean/test/testmedia/TestMedia.h"

#include "ocean/test/TestSelector.h"

#ifdef OCEAN_PLATFORM_BUILD_ANDROID

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace Test
{

namespace TestMedia
{

/**
 * This class implements tests for Android media functionality.
 * @ingroup testmedia
 */
class OCEAN_TEST_MEDIA_EXPORT TestAndroid
{
	public:

		/**
		 * Invokes all tests.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param selector The test selector
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, const TestSelector& selector);

		/**
		 * Tests the video encoder and decoder.
		 * This test encodes frames using VideoEncoder and then decodes them using VideoDecoder.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testVideoEncoderDecoder(const double testDuration);

	protected:

		/**
		 * Creates a test frame with a specific pattern.
		 * @param width The width of the frame, with range [1, infinity)
		 * @param height The height of the frame, with range [1, infinity)
		 * @param frameIndex The index of the frame, used to create a unique pattern
		 * @return The created frame
		 */
		static Frame createTestFrame(const unsigned int width, const unsigned int height, const unsigned int frameIndex);

		/**
		 * Validates that a decoded frame matches the expected test frame.
		 * @param testFrame The test frame to validate, must be valid
		 * @param decodedFrame The decoded frame to validate, must be valid
		 * @return True, if the decoded frame matches the expected test (within tolerance)
		 */
		static bool validateTestFrame(const Frame& testFrame, const Frame& decodedFrame);
};

}

}

}

#endif // OCEAN_PLATFORM_BUILD_ANDROID

#endif // META_OCEAN_TEST_TESTMEDIA_TEST_ANDROID_H
