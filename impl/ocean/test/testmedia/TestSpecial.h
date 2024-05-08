/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMEDIA_TEST_SPECIAL_H
#define META_OCEAN_TEST_TESTMEDIA_TEST_SPECIAL_H

#include "ocean/test/testmedia/TestMedia.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace Test
{

namespace TestMedia
{

/**
 * This class implements a test for the Special media library.
 * @ingroup testmedia
 */
class OCEAN_TEST_MEDIA_EXPORT TestSpecial
{
	public:

		/**
		 * Invokes all tests that are defined.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the read and write functions for BMP images.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testBmpImageEncodeDecode(const double testDuration);

		/**
		 * Tests the read and write functions for PFM images.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testPfmImageEncodeDecode(const double testDuration);

		/**
		 * Tests the read and write functions for NPY images.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testNpyImageEncodeDecode(const double testDuration);

		/**
		 * Tests the read and write functions for OCN images.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testOcnImageEncodeDecode(const double testDuration);
		/**
		 * Tests the read and write functions for any image.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testAnyImageEncodeDecode(const double testDuration);

		/**
		 * Tests the read and write functions for BMP images.
		 * @param width The width of the frame to be tested, with range [1, infinity)
		 * @param height The height of the frame to be tested, with range [1, infinity)
		 * @param pixelFormat The pixel format of the frame to be tested, must be valid
		 * @param pixelOrigin The pixel origin of the frame to be tested, must be valid
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testBmpImageEncodeDecode(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const double testDuration);

		/**
		 * Applies a stress test for the BMP decoder.
		 * @return True, if succeeded
		 */
		static bool testBmpDecodeStressTest();

		/**
		 * Tests the read and write functions for PFM images.
		 * @param width The width of the frame to be tested, with range [1, infinity)
		 * @param height The height of the frame to be tested, with range [1, infinity)
		 * @param pixelFormat The pixel format of the frame to be tested, must be valid
		 * @param pixelOrigin The pixel origin of the frame to be tested, must be valid
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testPfmImageEncodeDecode(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const double testDuration);

		/**
		 * Applies a stress test for the PFM decoder.
		 * @return True, if succeeded
		 */
		static bool testPfmDecodeStressTest();

		/**
		 * Tests the read and write functions for NPY images.
		 * @param width The width of the frame to be tested, with range [1, infinity)
		 * @param height The height of the frame to be tested, with range [1, infinity)
		 * @param pixelFormat The pixel format of the frame to be tested, must be valid
		 * @param pixelOrigin The pixel origin of the frame to be tested, must be valid
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testNpyImageEncodeDecode(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const double testDuration);

		/**
		 * Applies a stress test for the NPY decoder.
		 * @return True, if succeeded
		 */
		static bool testNpyDecodeStressTest();

		/**
		 * Applies a stress test for the OCN decoder.
		 * @return True, if succeeded
		 */
		static bool testOcnDecodeStressTest();

		/**
		 * Applies a stress test for any decoder.
		 * @return True, if succeeded
		 */
		static bool testDecodeStressTest();

	protected:

		/**
		 * Determines the minimal, the average and the maximal distance between corresponding pixel values (channel-wise) for two frames.
		 * @param firstFrame The first frame for which the similarity will be determined, must be valid
		 * @param secondFrame The second frame for which the similarity will be determined, with identical frame type as 'firstFrame' must be valid
		 * @param minDifference Resulting minimal value difference, with range [0, infinity)
		 * @param aveDifference Resulting average value difference, with range [minDifference, maxDifference]
		 * @param maxDifference Resulting maximal value difference, with range [aveDifference, infinity)
		 * @return True, if succeeded
		 */
		static bool determineSimilarity(const Frame& firstFrame, const Frame& secondFrame, double& minDifference, double& aveDifference, double& maxDifference);

		/**
		 * Determines the minimal, the average and the maximal distance between corresponding pixel values (channel-wise) for two frames.
		 * @param firstFrame The image data of the first frame for which the similarity will be determined, must be valid
		 * @param secondFrame The image data of the second frame for which the similarity will be determined, must be valid
		 * @param width The width of the frames in pixel, with range [1, infinity)
		 * @param height The height of the frames in pixel, with range [1, infinity)
		 * @param channels The number of frame channels, with range [1, infinity)
		 * @param firstFramePaddingElements The number of padding elements at the end of each first image row, in elements, with range [0, infinity)
		 * @param secondFramePaddingElements The number of padding elements at the end of each second image row, in elements, with range [0, infinity)
		 * @param minDifference Resulting minimal value difference, with range [0, infinity)
		 * @param aveDifference Resulting average value difference, with range [minDifference, maxDifference]
		 * @param maxDifference Resulting maximal value difference, with range [aveDifference, infinity)
		 */
		template <typename T>
		static void determineSimilarity(const T* firstFrame, const T* secondFrame, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int firstFramePaddingElements, const unsigned int secondFramePaddingElements, double& minDifference, double& aveDifference, double& maxDifference);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMEDIA_TEST_SPECIAL_H
