/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_ADVANCED_ZERO_MEAN_SUM_SQUARE_DIFFERENCES_H
#define META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_ADVANCED_ZERO_MEAN_SUM_SQUARE_DIFFERENCES_H

#include "ocean/test/testcv/testadvanced/TestCVAdvanced.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/PixelPosition.h"

#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestAdvanced
{

/**
 * This class implements an advanced zero-mean sum square differences test.
 * @ingroup testcvadvanced
 */
class OCEAN_TEST_CV_ADVANCED_EXPORT TestAdvancedZeroMeanSumSquareDifferences
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
		 * Tests all advanced zero-mean sum square differences functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the sum square differences function for two sub-pixel accurate patches in two images.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testTwoSubPixelPatch8BitPerChannel(const double testDuration);

		/**
		 * Tests the sum square differences function for one sub-pixel accurate patch and one pixel-accurate patch in two images.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testOneSubPixelPatch8BitPerChannel(const double testDuration);

		/**
		 * Tests the sum square differences function between an image patch and a buffer.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPatchBuffer8BitPerChannel(const double testDuration);

		/**
		 * Tests the sum square differences function between a sub-pixel image patch and a buffer.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPatchMirroredBorderBuffer8BitPerChannel(const double testDuration);

		/**
		 * Tests the sum square differences function for two pixel accurate patches in combination with a mask.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testTwoPixelPatchWithMask8BitPerChannel(const double testDuration);

	private:

		/**
		 * Tests the sum square differences function for two sub-pixel accurate patches in two images.
		 * @param width The width of the test image, in pixel, with range [tSize, infinity)
		 * @param height The height of the test image, in pixel, with range [tSize, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the patch, with range [1, infinity)
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static bool testTwoSubPixelPatch8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the sum square differences function for one sub-pixel accurate patch and one pixel-accurate patch in two images.
		 * @param width The width of the test image, in pixel, with range [tSize, infinity)
		 * @param height The height of the test image, in pixel, with range [tSize, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the patch, with range [1, infinity)
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static bool testOneSubPixelPatch8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration);

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
		 * Tests the sum square differences function between an image patch (mirrored at the image border) and a buffer.
		 * @param width The width of the test image, in pixel, with range [tSize, infinity)
		 * @param height The height of the test image, in pixel, with range [tSize, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the patch, with range [1, infinity)
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static bool testPatchMirroredBorderBuffer8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the sum square differences function for two pixel accurate patches in combination with a mask.
		 * @param width The width of the test image, in pixel, with range [tSize, infinity)
		 * @param height The height of the test image, in pixel, with range [tSize, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the patch, with range [1, infinity)
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static bool testTwoPixelPatchWithMask8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * CCalculates the ssd value between two sub-pixel patches.
		 * @param frame0 First frame to be used, must be valid, must be valid
		 * @param frame1 Second frame to be used, must be valid, must be valid
		 * @param center0 Center position in the first frame, with range [patchSize/2, width0 - patchSize/2 - 1)x[patchSize/2, height0 - patchSize/2 - 1)
		 * @param center1 Center position in the second frame, with range [patchSize/2, width1 - patchSize/2 - 1)x[patchSize/2, height1 - patchSize/2 - 1)
		 * @param patchSize  The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 * @return The resulting ssd
		 */
		static uint32_t calculate8BitPerChannel(const Frame& frame0, const Frame& frame1, const Vector2& center0, const Vector2& center1, const unsigned int patchSize);

		/**
		 * Calculates the ssd value between one sub-pixel patch and one pixel-accurate patch.
		 * @param frame0 First frame to be used, must be valid, must be valid
		 * @param frame1 Second frame to be used, must be valid, must be valid
		 * @param center0 Center position in the first frame, with range [patchSize/2, width0 - tSize/2)x[patchSize/2, height0 - patchSize/2)
		 * @param center1 Center position in the second frame, with range [patchSize/2, width1 - patchSize/2 - 1)x[patchSize/2, height1 - patchSize/2 - 1)
		 * @param patchSize  The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 * @return The resulting ssd
		 */
		static uint32_t calculate8BitPerChannel(const Frame& frame0, const Frame& frame1, const CV::PixelPosition& center0, const Vector2& center1, const unsigned int patchSize);

		/**
		 * Calculates the ssd value between one sub-pixel patch and a buffer.
		 * @param frame0 First frame to be used, must be valid, must be valid
		 * @param center0 Center position in the first frame, with range [patchSize/2, width0 - patchSize/2 - 1)x[patchSize/2, height0 - patchSize/2 - 1)
		 * @param buffer1 The pointer to the buffer, must be valid
		 * @param patchSize  The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 * @return The resulting ssd
		 */
		static uint32_t calculate8BitPerChannel(const Frame& frame0, const Vector2& center0, const uint8_t* buffer1, const unsigned int patchSize);

		/**
		 * Calculates the ssd value between one sub-pixel patch (mirrored at the image border) and a buffer.
		 * @param frame0 First frame to be used, must be valid, must be valid
		 * @param center0 Center position in the first frame, with range [patchSize/2, width0 - patchSize/2 - 1)x[patchSize/2, height0 - patchSize/2 - 1)
		 * @param buffer1 The pointer to the buffer, must be valid
		 * @param patchSize  The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 * @return The resulting ssd
		 */
		static uint32_t calculateMirroredBorder8BitPerChannel(const Frame& frame0, const Vector2& center0, const uint8_t* buffer1, const unsigned int patchSize);

		/**
		 * Calculates the sum of squared differences value between two patches with associated masks.
		 * @param frame0 First frame to be used, must be valid, must be valid
		 * @param frame1 Second frame to be used, must be valid, must be valid
		 * @param mask0 First mask to be used, must be valid, must be valid
		 * @param mask1 Second mask to be used, must be valid, must be valid
		 * @param center0 Center position in the first frame, with range [patchSize/2, width0 - tSize/2)x[patchSize/2, height0 - patchSize/2)
		 * @param center1 Center position in the second frame, with range [patchSize/2, width1 - tSize/2 - 1)x[patchSize/2, height1 - patchSize/2 - 1)
		 * @param patchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 * @param maskValue The pixel value for mask pixels which will be excluded from SSD calculation
		 * @return The resulting ssd
		 */
		static IndexPair32 calculateWithMask8BitPerChannel(const Frame& frame0, const Frame& frame1, const Frame& mask0, const Frame& mask1, const CV::PixelPosition& center0, const CV::PixelPosition& center1, const unsigned int patchSize, const uint8_t maskValue);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_ADVANCED_ZERO_MEAN_SUM_SQUARE_DIFFERENCES_H
