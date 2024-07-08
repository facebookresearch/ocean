/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMEDIA_TEST_IMAGE_IO_H
#define META_OCEAN_TEST_TESTMEDIA_TEST_IMAGE_IO_H

#include "ocean/test/testmedia/TestMedia.h"

#include "ocean/base/Frame.h"

#ifdef OCEAN_PLATFORM_BUILD_APPLE

namespace Ocean
{

namespace Test
{

namespace TestMedia
{

/**
 * This class implements a test for the IMageIO media library.
 * @ingroup testmedia
 */
class OCEAN_TEST_MEDIA_EXPORT TestImageIO
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
		 * Tests the read and write functions for JPEG images.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testJpgImageEncodeDecode(const double testDuration);

		/**
		 * Tests the read and write functions for HEIC images.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testHeicImageEncodeDecode(const double testDuration);

		/**
		 * Tests the read and write functions for PNG images.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testPngImageEncodeDecode(const double testDuration);

		/**
		 * Tests the read and write functions for PNG images.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testTifImageEncodeDecode(const double testDuration);

		/**
		 * Tests the read and write functions for any image.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testAnyImageEncodeDecode(const double testDuration);

		/**
		 * Tests the encoding and decoding between individual media libraries.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testInterchangeability(const double testDuration);

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
		 * Tests the read and write functions for JPEG images.
		 * @param width The width of the frame to be tested, with range [1, infinity)
		 * @param height The height of the frame to be tested, with range [1, infinity)
		 * @param pixelFormat The pixel format of the frame to be tested, must be valid
		 * @param pixelOrigin The pixel origin of the frame to be tested, must be valid
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testJpgImageEncodeDecode(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const double testDuration);

		/**
		 * Tests the read and write functions for HEIC images.
		 * @param width The width of the frame to be tested, with range [1, infinity)
		 * @param height The height of the frame to be tested, with range [1, infinity)
		 * @param pixelFormat The pixel format of the frame to be tested, must be valid
		 * @param pixelOrigin The pixel origin of the frame to be tested, must be valid
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testHeicImageEncodeDecode(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const double testDuration);

		/**
		 * Tests the read and write functions for PNG images.
		 * @param width The width of the frame to be tested, with range [1, infinity)
		 * @param height The height of the frame to be tested, with range [1, infinity)
		 * @param pixelFormat The pixel format of the frame to be tested, must be valid
		 * @param pixelOrigin The pixel origin of the frame to be tested, must be valid
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testPngImageEncodeDecode(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const double testDuration);

		/**
		 * Tests the read and write functions for TIFF images.
		 * @param width The width of the frame to be tested, with range [1, infinity)
		 * @param height The height of the frame to be tested, with range [1, infinity)
		 * @param pixelFormat The pixel format of the frame to be tested, must be valid
		 * @param pixelOrigin The pixel origin of the frame to be tested, must be valid
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testTifImageEncodeDecode(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const double testDuration);

		/**
		 * Tests the ImageBufferRecorder and ImageBuffer functionality of the media library.
		 * @param frameType The frame type to be used for testing, must be valid
		 * @param imageType The file extension of the image to be created (e.g. jpg, png, bmp, or tif), must be defined
		 * @param maximalAverageDifference The maximal average difference between two frames so that they still count as similar, with range [0, infinity), 0 for lossless compress, >0 for lossy compression
		 * @return True, if succeeded
		 */
		static bool testBufferImageRecorder(const FrameType& frameType, const std::string& imageType, const double maximalAverageDifference);

		/**
		 * Tests the quality property for a specified image type.
		 * @param imageType The image type for which the test will be executed
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testQualityProperty(const std::string& imageType, const double testDuration);

		/**
		 * Tests setting color profile names for a specified image type.
		 * @param imageType The image type for which the test will be executed
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testColorProfileNameProperty(const std::string& imageType, const double testDuration);

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

#endif // OCEAN_PLATFORM_BUILD_APPLE

#endif // META_OCEAN_TEST_TESTMEDIA_TEST_IMAGE_IO_H
