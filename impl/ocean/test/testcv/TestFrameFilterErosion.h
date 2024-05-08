/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_EROSION_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_EROSION_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements an erosion filter test.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameFilterErosion
{
	public:

		/**
		 * Tests the entire erosion filter.
		 * @param width The width of the test frame in pixel, with range [4, infinity)
		 * @param height The height of the test frame in pixel, with range [4, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the mask shrinking for a 4-neighborhood.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testShrinkMask4Neighbor(const double testDuration);

		/**
		 * Tests the mask shrinking for a 4-neighborhood.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static bool testShrinkMask4Neighbor(const double testDuration);

		/**
		 * Tests the mask shrinking for a 8-neighborhood.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testShrinkMask8Neighbor(const double testDuration);

		/**
		 * Tests the mask shrinking for a 8-neighborhood.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static bool testShrinkMask8Neighbor(const double testDuration);

		/**
		 * Tests the random mask shrinking for a 8-neighborhood.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testShrinkMaskRandom8Neighbor(const double testDuration);

		/**
		 * Tests the random mask shrinking for a 8-neighborhood.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static bool testShrinkMaskRandom8Neighbor(const double testDuration);

		/**
		 * Tests the binary 8 bit erosion filter with a cross filter with diameter 3.
		 * @param width The width of the test frame in pixel, with range [2, infinity)
		 * @param height The height of the test frame in pixel, with range [2, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test8Bit4Neighbor(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the binary 8 bit erosion filter with a 3x3 square filter.
		 * @param width The width of the test frame in pixel, with range [2, infinity)
		 * @param height The height of the test frame in pixel, with range [2, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test8Bit8Neighbor(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		* Tests the binary 8 bit erosion filter with a 5x5 square filter.
		* @param width The width of the test frame in pixel, with range [4, infinity)
		* @param height The height of the test frame in pixel, with range [4, infinity)
		* @param testDuration Number of seconds for each test, with range (0, infinity)
		* @param worker The worker object
		* @return True, if succeeded
		*/
		static bool test8Bit24Neighbor(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the binary 8 bit erosion filter for square filters and cross filters.
		 * @param width The width of the test frame in pixel, with range [4, infinity)
		 * @param height The height of the test frame in pixel, with range [4, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test8Bit(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

	private:

		/**
		 * Validates the mask shrinking result for a 4-neighborhood.
		 * @param frame The frame to be handled, must be valid
		 * @param mask The mask to be handled, with same dimension and pixel origin as the frame
		 * @param resultFrame The resulting frame, must be valid
		 * @param resultMask The resulting mask, with same dimension and pixel origin as the frame
		 * @return True, if succeeded
		 */
		static bool validateShrinkMask4Neighbor(Frame&& frame, Frame&& mask, const Frame& resultFrame, const Frame& resultMask);

		/**
		 * Validates the mask shrinking result for an 8-neighborhood.
		 * @param frame The frame to be handled, must be valid
		 * @param mask The mask to be handled, with same dimension and pixel origin as the frame
		 * @param resultFrame The resulting frame, must be valid
		 * @param resultMask The resulting mask, with same dimension and pixel origin as the frame
		 * @return True, if succeeded
		 */
		static bool validateShrinkMask8Neighbor(Frame&& frame, Frame&& mask, const Frame& resultFrame, const Frame& resultMask);

		/**
		 * Validates the random mask shrinking result for an 8-neighborhood.
		 * @param frame The frame to be handled, must be valid
		 * @param mask The mask to be handled, with same dimension and pixel origin as the frame
		 * @param resultFrame The resulting frame, must be valid
		 * @param resultMask The resulting mask, with same dimension and pixel origin as the frame
		 * @param randomNoise The maximal amplitude of the random noise to be applied to each color value, with range [0, 255]
		 * @param randomSeed The random seed value to be used
		 * @return True, if succeeded
		 */
		static bool validateShrinkMaskRandom8Neighbor(Frame&& frame, Frame&& mask, const Frame& resultFrame, const Frame& resultMask, const unsigned int randomNoise, const unsigned int randomSeed);

		/**
		 * Validates the binary 8 bit erosion filter for a cross kernel.
		 * @param mask The binary mask for which the result will be validated, must be valid
		 * @param target The filtered mask to validate, must be valid
		 * @param width The width of the mask in pixel, with range [2, infinity)
		 * @param height The height of the mask in pixel, with range [2, infinity)
		 * @param kernelSize The size of the erosion kernel in pixel, with range [3, infinity), must be odd
		 * @param maskValue The value of a mask pixel, all other pixels are treated as non-mask pixels, with range [0, 255]
		 * @param maskPaddingElements Optional number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool validate8BitCrossKernel(const uint8_t* const mask, const uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int kernelSize, const uint8_t maskValue, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements);

		/**
		 * Validates the binary 8 bit erosion filter for a square kernel.
		 * @param mask The binary mask for which the result will be validated, must be valid
		 * @param target The filtered mask to validate, must be valid
		 * @param width The width of the mask in pixel, with range [2, infinity)
		 * @param height The height of the mask in pixel, with range [2, infinity)
		 * @param kernelSize The size of the erosion kernel in pixel, with range [3, infinity), must be odd
		 * @param maskValue The value of a mask pixel, all other pixels are treated as non-mask pixels, with range [0, 255]
		 * @param maskPaddingElements Optional number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool validate8BitSquareKernel(const uint8_t* const mask, const uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int kernelSize, const uint8_t maskValue, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements);

		/**
		 * Returns whether a given pixel is a mask pixel while at least one pixel in the 8-neighborhood is a non-mask pixel.
		 * @param mask The mask to be used, must have pixel format FORMAT_Y8, must be valid
		 * @param pixelPosition The position of the pixel to be check, with range [0, mask.width() - 1]x[0, mask.height() -1]
		 * @param maskValue The value of a mask pixel, with range [0, 255]
		 * @return True, if so
		 */
		static bool isMaskBorderPixel(const Frame& mask, const CV::PixelPosition& pixelPosition, const uint8_t maskValue);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_EROSION_H
