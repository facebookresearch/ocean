// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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

	private:

		/**
		 * Tests the partial zero-mean ssd with two binary masks.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPartialTwoMasks(const double testDuration);

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
		 * Tests the partial zero-mean ssd with two binary masks.
		 * @param width0 The width of the first frame in pixel, with range [1, infinity)
		 * @param height0 The height of the first frame in pixel, with range [1, infinity)
		 * @param width1 The width of the second frame in pixel, with range [1, infinity)
		 * @param height1 The height of the second frame in pixel, with range [1, infinity)
		 * @param sizeX The width of the patch in pixel, with range [1, infinity)
		 * @param sizeY The height of the patch in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of data channels of the frames
		 */
		template <unsigned int tChannels>
		static bool testPartialTwoMasks(const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int sizeX, const unsigned int sizeY, const double testDuration);

		/**
		 * Validates the partial zero-mean ssd with two binary masks.
		 * @param frame0 The first frame to be used for testing
		 * @param frame1 The second frame to be used for testing
		 * @param mask0 The mask for the first frame
		 * @param mask1 The mask for the second frame
		 * @param width0 The width of the first frame in pixel, with range [1, infinity)
		 * @param height0 The height of the first frame in pixel, with range [1, infinity)
		 * @param width1 The width of the second frame in pixel, with range [1, infinity)
		 * @param height1 The height of the second frame in pixel, with range [1, infinity)
		 * @param sizeX The width of the patch in pixel, with range [1, infinity)
		 * @param sizeY The height of the patch in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of data channels of the frames
		 */
		template <unsigned int tChannels>
		static bool validatePartialTwoMasks(const unsigned char* frame0, const unsigned char* frame1, const unsigned char* mask0, const unsigned char* mask1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int sizeX, const unsigned int sizeY, const double testDuration);

		/**
		 * Tests the partial zero-mean ssd with two binary masks and two counts for the number of valid pixels.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testPartialTwoMasksTwoCounts(const double testDuration);

		/**
		 * Tests the partial zero-mean ssd with two binary masks and two counts for the number of valid pixels.
		 * @param width0 The width of the first frame in pixel, with range [1, infinity)
		 * @param height0 The height of the first frame in pixel, with range [1, infinity)
		 * @param width1 The width of the second frame in pixel, with range [1, infinity)
		 * @param height1 The height of the second frame in pixel, with range [1, infinity)
		 * @param sizeX The width of the patch in pixel, with range [1, infinity)
		 * @param sizeY The height of the patch in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of data channels of the frames
		 */
		template <unsigned int tChannels>
		static bool testPartialTwoMasksTwoCounts(const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int sizeX, const unsigned int sizeY, const double testDuration);

		/**
		 * Validates the partial zero-mean ssd with two binary masks and two counts for the number of valid pixels.
		 * @param frame0 The first frame to be used for testing
		 * @param frame1 The second frame to be used for testing
		 * @param mask0 The mask for the first frame
		 * @param mask1 The mask for the second frame
		 * @param width0 The width of the first frame in pixel, with range [1, infinity)
		 * @param height0 The height of the first frame in pixel, with range [1, infinity)
		 * @param width1 The width of the second frame in pixel, with range [1, infinity)
		 * @param height1 The height of the second frame in pixel, with range [1, infinity)
		 * @param sizeX The width of the patch in pixel, with range [1, infinity)
		 * @param sizeY The height of the patch in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of data channels of the frames
		 */
		template <unsigned int tChannels>
		static bool validatePartialTwoMasksTwoCounts(const unsigned char* frame0, const unsigned char* frame1, const unsigned char* mask0, const unsigned char* mask1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int sizeX, const unsigned int sizeY, const double testDuration);

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
