/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_SUM_SQUARE_DIFFERENCES_H
#define META_OCEAN_TEST_TESTCV_SUM_SQUARE_DIFFERENCES_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/cv/SumSquareDifferences.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements sum square differences (SSD) tests.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestSumSquareDifferences
{
	protected:

		/**
		 * Definition of individual types of implementation.
		 */
		enum ImplementationType : uint32_t
		{
			/// The naive implementation.
			IT_NAIVE,
			/// The template-based implementation.
			IT_TEMPLATE,
			/// The SSE-based implementation.
			IT_SSE,
			/// The NEON-based implementation.
			IT_NEON,
			/// The default implementation (which is actually used by default).
			IT_DEFAULT
		};

	public:

		/**
		 * Tests the entire sum square differences functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the sum square differences function for two image patches.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPatch8BitPerChannel(const double testDuration);

		/**
		 * Tests the sum square differences function for two buffers.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testBuffer8BitPerChannel(const double testDuration);

		/**
		 * Tests the sum square differences function between an image patch and a buffer.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPatchBuffer8BitPerChannel(const double testDuration);

		/**
		 * Tests the sum square differences function for two image patches which are mirrored at the image border.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPatchMirroredBorder8BitPerChannel(const double testDuration);

		/**
		 * Tests the sum square differences function for image patches with pixel accuracy which can be partially outside of the image.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPatchAtBorder8BitPerChannel(const double testDuration);

	private:

		/**
		 * Tests the sum square differences function for two image patches.
		 * @param width The width of the test image, in pixel, with range [tSize, infinity)
		 * @param height The height of the test image, in pixel, with range [tSize, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the patch, with range [1, infinity)
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static bool testPatch8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the sum square differences function for two buffers.
		 * @param width The width of the test image, in pixel, with range [tSize, infinity)
		 * @param height The height of the test image, in pixel, with range [tSize, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of channels, with range [1, infinity)
		 * @tparam tPixels The number of pixels in the buffer, with range [1, infinity)
		 */
		template <unsigned int tChannels, unsigned int tPixels>
		static bool testBuffer8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the sum square differences function between an image patch and a buffer.
		 * @param width The width of the test image, in pixel, with range [tSize, infinity)
		 * @param height The height of the test image, in pixel, with range [tSize, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the patch, with range [1, infinity)
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static bool testPatchBuffer8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the sum square differences function for image patches with pixel accuracy which can be partially outside of the image.
		 * @param width The width of the test image, in pixel, with range [tSize, infinity)
		 * @param height The height of the test image, in pixel, with range [tSize, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the patch, with range [1, infinity)
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static bool testPatchAtBorder8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the sum square differences function for two image patches which are mirrored at the image border.
		 * @param width The width of the test image, in pixel, with range [tSize, infinity)
		 * @param height The height of the test image, in pixel, with range [tSize, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tSize The size of the patch, with range [1, infinity)
		 */
		template <unsigned int tChannels, unsigned int tSize>
		static bool testPatchMirroredBorder8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration);

	protected:

		/**
		 * Calculates the ssd value between two pixel accurate patches which can be partially outside of the frame.
		 * @param frame0 First frame to be used, must be valid, must be valid
		 * @param frame1 Second frame to be used, must be valid, must be valid
		 * @param center0 Center position in the first frame, with range [0, width0 - 1]x[0, height0 - 1]
		 * @param center1 Center position in the first frame, with range [0, width1 - 1]x[0, height1 - 1]
		 * @param patchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 * @return The resulting pair holding the ssd and the number of pixels which contributed to the ssd, and ssd of -1 if a patch0 pixel does not have a corresponding patch1 pixel
		 */
		static IndexPair32 calculateAtBorder8BitPerChannel(const Frame& frame0, const Frame& frame1, const CV::PixelPosition& center0, const CV::PixelPosition& center1, const unsigned int patchSize);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_SUM_SQUARE_DIFFERENCES_H
