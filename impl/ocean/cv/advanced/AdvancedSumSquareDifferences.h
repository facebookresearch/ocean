/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_ADVANCED_SUM_SQUARE_DIFFERENCES_H
#define META_OCEAN_CV_ADVANCED_ADVANCED_SUM_SQUARE_DIFFERENCES_H

#include "ocean/cv/advanced/Advanced.h"
#include "ocean/cv/advanced/AdvancedSumSquareDifferencesBase.h"
#include "ocean/cv/advanced/AdvancedSumSquareDifferencesSSE.h"

#include "ocean/cv/PixelPosition.h"

#include "ocean/math/Math.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

/**
 * This class implements sum of square difference calculation functions allowing to determine the ssd with sub-pixel accuracy.
 * @ingroup cvadvanced
 */
class OCEAN_CV_ADVANCED_EXPORT AdvancedSumSquareDifferences : public AdvancedSumSquareDifferencesBase
{
	public:

		/**
		 * Returns the sum of square differences for an image patch block determined for two sub-pixel positions between two individual images.
		 * @param image0 The image in which the first patch is located, must be valid
		 * @param image1 The image in which the second patch is located, must be valid
		 * @param width0 The width of the first image, in pixels, with range [tPatchSize + 1, infinity)
		 * @param width1 The width of the second image, in pixels, with range [tPatchSize + 1, infinity)
		 * @param centerX0 Horizontal sub-pixel center position of the (tSize x tSize) block in the first frame, with range [tSize/2, width0 - tSize/2 - 1)
		 * @param centerY0 Vertical sub-pixel center position of the (tSize x tSize) block in the first frame, with range [tSize/2, height0 - tSize/2 - 1)
		 * @param centerX1 Horizontal sub-pixel center position of the (tSize x tSize) block in the second frame, with range [tSize/2, width1 - tSize/2 - 1)
		 * @param centerY1 Vertical sub-pixel center position of the (tSize x tSize) block in the second frame, with range [tSize/2, height1 - tSize/2 - 1)
		 * @param image0PaddingElements The number of padding elements at the end of each row of the first image, in elements, with range [0, infinity)
		 * @param image1PaddingElements The number of padding elements at the end of each row of the second image, in elements, with range [0, infinity)
		 * @return The resulting sum of square differences, with range [0, infinity)
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static inline uint32_t patch8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int width1, const Scalar centerX0, const Scalar centerY0, const Scalar centerX1, const Scalar centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements);

		/**
		 * Returns the sum of square differences for an image patch block determined for one pixel and one sub-pixel position between two individual images.
		 * @param image0 The image in which the first patch is located, must be valid
		 * @param image1 The image in which the second patch is located, must be valid
		 * @param width0 The width of the first image, in pixels, with range [tPatchSize + 1, infinity)
		 * @param width1 The width of the second image, in pixels, with range [tPatchSize + 1, infinity)
		 * @param centerX0 Horizontal pixel center position of the (tSize x tSize) block in the first frame, with range [tSize/2, width0 - tSize/2)
		 * @param centerY0 Vertical pixel center position of the (tSize x tSize) block in the first frame, with range [tSize/2, height0 - tSize/2)
		 * @param centerX1 Horizontal sub-pixel center position of the (tSize x tSize) block in the second frame, with range [tSize/2, width1 - tSize/2 - 1)
		 * @param centerY1 Vertical sub-pixel center position of the (tSize x tSize) block in the second frame, with range [tSize/2, height1 - tSize/2 - 1)
		 * @param image0PaddingElements The number of padding elements at the end of each row of the first image, in elements, with range [0, infinity)
		 * @param image1PaddingElements The number of padding elements at the end of each row of the second image, in elements, with range [0, infinity)
		 * @return The resulting sum of square differences, with range [0, infinity)
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static inline uint32_t patch8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const Scalar centerX1, const Scalar centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements);

		/**
		 * Determines the sum of square differences between an image patch with sub-pixel accuracy and a memory buffer.
		 * @param image0 The image in which the patch is located with sub-pixel accuracy, must be valid
		 * @param width0 The width of the image in pixel, with range [tPatchSize + 1, infinity)
		 * @param centerX0 The horizontal center position of the image patch with sub-pixel accuracy, with range [tPatchSize / 2, width0 - tPatchSize / 2 - 1]
		 * @param centerY0 The vertical center position of the image patch with sub-pixel accuracy, with range [tPatchSize / 2, height0 - tPatchSize / 2 - 1]
		 * @param image0PaddingElements The number of padding elements at the end of each image row, in elements, with range [0, infinity)
		 * @param buffer1 The memory buffer with `tChannels * tPatchSize * tPatchSize` elements, must be valid
		 * @return The resulting ssd value
		 * @tparam tChannels The number of channels for the given frames, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static inline uint32_t patchBuffer8BitPerChannel(const uint8_t* image0, const unsigned int width0, const Scalar centerX0, const Scalar centerY0, const unsigned int image0PaddingElements, const uint8_t* buffer1);

		/**
		 * Determines the sum of square differences between an image patch with sub-pixel accuracy and a memory buffer.
		 * Pixels in the square region pointing outside the frame are mirrored back into the frame.
		 * @param image0 The image in which the patch is located with sub-pixel accuracy, must be valid
		 * @param width0 The width of the image in pixel, with range [tPatchSize/2 + 1, infinity)
		 * @param height0 The height of the image in pixel, with range [tPatchSize/2 + 1, infinity)
		 * @param centerX0 The horizontal center position of the image patch with sub-pixel accuracy, with range [0, width0)
		 * @param centerY0 The vertical center position of the image patch with sub-pixel accuracy, with range [0, height0)
		 * @param image0PaddingElements The number of padding elements at the end of each image row, in elements, with range [0, infinity)
		 * @param buffer1 The memory buffer with `tChannels * tPatchSize * tPatchSize` elements, must be valid
		 * @return The resulting ssd value
		 * @tparam tChannels The number of channels for the given frames, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static inline uint32_t patchMirroredBorderBuffer8BitPerChannel(const uint8_t* image0, const unsigned int width0, const unsigned int height0, const Scalar centerX0, const Scalar centerY0, const unsigned int image0PaddingElements, const uint8_t* buffer1);

		/**
		 * Returns the sum of square differences for an image patch determined for two pixel accurate positions between two individual images .
		 * Further, each image is associated with a binary mask, only valid non-mask pixels are used during SSD calculation.
		 * @param image0 The image in which the first patch is located, must be valid
		 * @param image1 The image in which the second patch is located, must be valid
		 * @param mask0 The mask associated with the first image, must be valid
		 * @param mask1 The mask associated with the second image, must be valid
		 * @param width0 The width of the first image, in pixels, with range [patchSize / 2 + 1, infinity)
		 * @param height0 The height of the first image, in pixels, with range [patchSize / 2 + 1, infinity)
		 * @param width1 The width of the second image, in pixels, with range [patchSize / 2 + 1, infinity)
		 * @param height1 The height of the second image, in pixels, with range [patchSize / 2 + 1, infinity)
		 * @param centerX0 Horizontal sub-pixel center position of the (tSize x tSize) block in the first frame, with range [0, width0 - 1]
		 * @param centerY0 Vertical sub-pixel center position of the (tSize x tSize) block in the first frame, with range [0, height0 - 1]
		 * @param centerX1 Horizontal sub-pixel center position of the (tSize x tSize) block in the second frame, with range [0, width1 - 1]
		 * @param centerY1 Vertical sub-pixel center position of the (tSize x tSize) block in the second frame, with range [0, height1 - 1]
		 * @param image0PaddingElements The number of padding elements at the end of each row of the first image, in elements, with range [0, infinity)
		 * @param image1PaddingElements The number of padding elements at the end of each row of the second image, in elements, with range [0, infinity)
		 * @param mask0PaddingElements The number of padding elements at the end of each row of the first mask, in elements, with range [0, infinity)
		 * @param mask1PaddingElements The number of padding elements at the end of each row of the second mask, in elements, with range [0, infinity)
		 * @param maskValue The pixel value of a mask pixel which will be excluded when calculating the SSD, with range [0, 255]
		 * @return The resulting pair holding first: sum of square differences, second: number of pixels contributed to the result
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static inline IndexPair32 patchWithMask8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const uint8_t* mask0, const uint8_t* mask1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements, const unsigned int mask0PaddingElements, const unsigned int mask1PaddingElements, const uint8_t maskValue);

		/**
		 * Returns the sum of square differences for an image patch determined for two pixel accurate positions between two individual images .
		 * Further, each image is associated with a binary mask, only valid non-mask pixels in the first image are used during SSD calculation.
		 * If a valid (mask) pixel in the first image does not have a valid (mask) pixel in the second frame, -1 is returned as SSD value.
		 * @param image0 The image in which the first patch is located, must be valid
		 * @param image1 The image in which the second patch is located, must be valid
		 * @param mask0 The mask associated with the first image, must be valid
		 * @param mask1 The mask associated with the second image, must be valid
		 * @param patchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 * @param width0 The width of the first image, in pixels, with range [patchSize / 2 + 1, infinity)
		 * @param height0 The height of the first image, in pixels, with range [patchSize / 2 + 1, infinity)
		 * @param width1 The width of the second image, in pixels, with range [patchSize / 2 + 1, infinity)
		 * @param height1 The height of the second image, in pixels, with range [patchSize / 2 + 1, infinity)
		 * @param centerX0 Horizontal sub-pixel center position of the (tSize x tSize) block in the first frame, with range [0, width0 - 1]
		 * @param centerY0 Vertical sub-pixel center position of the (tSize x tSize) block in the first frame, with range [0, height0 - 1]
		 * @param centerX1 Horizontal sub-pixel center position of the (tSize x tSize) block in the second frame, with range [0, width1 - 1]
		 * @param centerY1 Vertical sub-pixel center position of the (tSize x tSize) block in the second frame, with range [0, height1 - 1]
		 * @param image0PaddingElements The number of padding elements at the end of each row of the first image, in elements, with range [0, infinity)
		 * @param image1PaddingElements The number of padding elements at the end of each row of the second image, in elements, with range [0, infinity)
		 * @param mask0PaddingElements The number of padding elements at the end of each row of the first mask, in elements, with range [0, infinity)
		 * @param mask1PaddingElements The number of padding elements at the end of each row of the second mask, in elements, with range [0, infinity)
		 * @param maskValue The pixel value of a mask pixel which will be excluded when calculating the SSD, with range [0, 255]
		 * @return The resulting pair holding first: sum of square differences, second: number of pixels contributed to the result
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline IndexPair32 patchWithRejectingMask8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const uint8_t* mask0, const uint8_t* mask1, const unsigned int patchSize, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements, const unsigned int mask0PaddingElements, const unsigned int mask1PaddingElements, const uint8_t maskValue);
};

template <unsigned int tChannels, unsigned int tPatchSize>
inline unsigned int AdvancedSumSquareDifferences::patch8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int width1, const Scalar centerX0, const Scalar centerY0, const Scalar centerX1, const Scalar centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize >= 1u && tPatchSize % 2u == 1u, "Invalid patch size!");

	ocean_assert(image0 != nullptr && image1 != nullptr);

	ocean_assert(width0 >= tPatchSize + 1u);
	ocean_assert(width1 >= tPatchSize + 1u);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	ocean_assert(centerX0 >= Scalar(tPatchSize_2) && centerX0 < Scalar(width0 - tPatchSize_2 - 1u));
	ocean_assert(centerX1 >= Scalar(tPatchSize_2) && centerX1 < Scalar(width1 - tPatchSize_2 - 1u));

	ocean_assert_and_suppress_unused(centerY0 >= Scalar(tPatchSize_2) && centerY1 >= Scalar(tPatchSize_2), tPatchSize_2);

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	if constexpr (tChannels >= 1u && tChannels <= 4u && tPatchSize == 5u)
	{
		 return AdvancedSumSquareDifferencesSSE::patch8BitPerChannel<tChannels, tPatchSize>(image0, image1, width0, width1, centerX0, centerY0, centerX1, centerY1, image0PaddingElements, image1PaddingElements);
	}

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	if constexpr (tPatchSize >= 5u)
	{
		// return AdvancedSumSquareDifferencesNEON::patch8BitPerChannel<tChannels, tPatchSize>(image0, image1, width0, width1, centeRX0, centerY0, centeRX1, centerY1, image0PaddingElements, image1PaddingElements); // **TODO**
	}

#endif // OCEAN_HARDWARE_SSE_VERSION, OCEAN_HARDWARE_NEON_VERSION

	return AdvancedSumSquareDifferencesBase::patch8BitPerChannelTemplate<tChannels, tPatchSize>(image0, image1, width0, width1, centerX0, centerY0, centerX1, centerY1, image0PaddingElements, image1PaddingElements);
}

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t AdvancedSumSquareDifferences::patch8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const Scalar centerX1, const Scalar centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize >= 1u && tPatchSize % 2u == 1u, "Invalid patch size!");

	ocean_assert(image0 != nullptr && image1 != nullptr);

	ocean_assert(width0 >= tPatchSize + 1u);
	ocean_assert(width1 >= tPatchSize + 1u);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	ocean_assert(centerX0 >= tPatchSize_2 && centerX0 < width0 - tPatchSize_2);
	ocean_assert(centerX1 >= Scalar(tPatchSize_2) && centerX1 < Scalar(width1 - tPatchSize_2 - 1u));

	ocean_assert_and_suppress_unused(centerY0 >= tPatchSize_2 && centerY1 >= Scalar(tPatchSize_2), tPatchSize_2);

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	if constexpr (tChannels >= 1u && tChannels <= 4u && tPatchSize == 5u)
	{
		 return AdvancedSumSquareDifferencesSSE::patch8BitPerChannel<tChannels, tPatchSize>(image0, image1, width0, width1, centerX0, centerY0, centerX1, centerY1, image0PaddingElements, image1PaddingElements);
	}

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	/*if constexpr (tPatchSize >= 5u) // not yet implemented
	{
		return AdvancedSumSquareDifferencesNEON::patch8BitPerChannel<tChannels, tPatchSize>(image0, image1, width0, width1, centeRX0, centerY0, centeRX1, centerY1, image0PaddingElements, image1PaddingElements);
	}*/

#endif // OCEAN_HARDWARE_SSE_VERSION, OCEAN_HARDWARE_NEON_VERSION

	return AdvancedSumSquareDifferencesBase::patch8BitPerChannelTemplate<tChannels, tPatchSize>(image0, image1, width0, width1, centerX0, centerY0, centerX1, centerY1, image0PaddingElements, image1PaddingElements);
}

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t AdvancedSumSquareDifferences::patchBuffer8BitPerChannel(const uint8_t* image0, const unsigned int width0, const Scalar centerX0, const Scalar centerY0, const unsigned int image0PaddingElements, const uint8_t* buffer1)
{
	static_assert(tChannels != 0u, "Invalid number of image channels!");
	static_assert(tPatchSize >= 1u && tPatchSize % 2u == 1u, "Invalid patch size!");

	ocean_assert(image0 != nullptr && buffer1 != nullptr);

	ocean_assert(width0 >= tPatchSize + 1u);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	ocean_assert_and_suppress_unused(centerX0 >= Scalar(tPatchSize_2) && centerX0 < Scalar(width0 - tPatchSize_2 - 1u), tPatchSize_2);

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	/*if constexpr (tChannels >= 1u && tChannels <= 4u && tPatchSize == 5u) // not yet implemented
	{
		 return AdvancedSumSquareDifferencesSSE::patchBuffer8BitPerChannel<tChannels, tPatchSize>(image0, image1, width0, width1, centerX0, centerY0, centerX1, centerY1, image0PaddingElements, image1PaddingElements);
	}*/

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	/*if constexpr (tPatchSize >= 5u) // not yet implemented
	{
		return AdvancedSumSquareDifferencesNEON::patchBuffer8BitPerChannel<tChannels, tPatchSize>(image0, image1, width0, width1, centeRX0, centerY0, centeRX1, centerY1, image0PaddingElements, image1PaddingElements);
	}*/

#endif // OCEAN_HARDWARE_SSE_VERSION, OCEAN_HARDWARE_NEON_VERSION

	return AdvancedSumSquareDifferencesBase::patchBuffer8BitPerChannelTemplate<tChannels, tPatchSize>(image0, width0, centerX0, centerY0, image0PaddingElements, buffer1);
}

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t AdvancedSumSquareDifferences::patchMirroredBorderBuffer8BitPerChannel(const uint8_t* image0, const unsigned int width0, const unsigned int height0, const Scalar centerX0, const Scalar centerY0, const unsigned int image0PaddingElements, const uint8_t* buffer1)
{
	static_assert(tChannels != 0u, "Invalid number of image channels!");
	static_assert(tPatchSize >= 1u && tPatchSize % 2u == 1u, "Invalid patch size!");

	ocean_assert(image0 != nullptr && buffer1 != nullptr);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	ocean_assert(width0 >= tPatchSize_2 + 1u);
	ocean_assert_and_suppress_unused(height0 >= tPatchSize_2 + 1u, tPatchSize_2);

	ocean_assert(centerX0 >= Scalar(0) && centerX0 < Scalar(width0));
	ocean_assert(centerY0 >= Scalar(0) && centerY0 < Scalar(height0));

	constexpr unsigned int tPatchPixels = tPatchSize * tPatchSize;

	uint8_t target[tPatchPixels * tChannels];
	AdvancedFrameInterpolatorBilinear::interpolateSquareMirroredBorder8BitPerChannel<tChannels, tPatchSize>(image0, width0, height0, image0PaddingElements, target, Vector2(centerX0, centerY0));
	return SumSquareDifferences::buffer8BitPerChannel<tChannels, tPatchPixels>(target, buffer1);
}

template <unsigned int tChannels, unsigned int tPatchSize>
inline IndexPair32 AdvancedSumSquareDifferences::patchWithMask8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const uint8_t* mask0, const uint8_t* mask1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements, const unsigned int mask0PaddingElements, const unsigned int mask1PaddingElements, const uint8_t maskValue)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize >= 1u && tPatchSize % 2u == 1u, "Invalid patch size!");

	ocean_assert(image0 != nullptr && image1 != nullptr);
	ocean_assert(mask0 != nullptr && mask1 != nullptr);

	const unsigned int patchSize_2 = tPatchSize / 2u;

	ocean_assert(width0 >= patchSize_2 + 1u && height0 >= patchSize_2 + 1u);
	ocean_assert_and_suppress_unused(width1 >= patchSize_2 + 1u && height1 >= patchSize_2 + 1u, patchSize_2);

	ocean_assert(centerX0 < width0 && centerY0 < height0);
	ocean_assert(centerX1 < width1 && centerY1 < height1);

	return AdvancedSumSquareDifferencesBase::patchWithMask8BitPerChannelTemplate<tChannels, tPatchSize>(image0, image1, mask0, mask1, width0, height0, width1, height1, centerX0, centerY0, centerX1, centerY1, image0PaddingElements, image1PaddingElements, mask0PaddingElements, mask1PaddingElements, maskValue);
}

template <unsigned int tChannels>
inline IndexPair32 AdvancedSumSquareDifferences::patchWithRejectingMask8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const uint8_t* mask0, const uint8_t* mask1, const unsigned int patchSize, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements, const unsigned int mask0PaddingElements, const unsigned int mask1PaddingElements, const uint8_t maskValue)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(image0 != nullptr && image1 != nullptr);
	ocean_assert(mask0 != nullptr && mask1 != nullptr);

	ocean_assert(patchSize >= 1u && patchSize % 2u == 1u);

	const unsigned int patchSize_2 = patchSize / 2u;

	ocean_assert(width0 >= patchSize_2 + 1u && height0 >= patchSize_2 + 1u);
	ocean_assert_and_suppress_unused(width1 >= patchSize_2 + 1u && height1 >= patchSize_2 + 1u, patchSize_2);

	ocean_assert(centerX0 < width0 && centerY0 < height0);
	ocean_assert(centerX1 < width1 && centerY1 < height1);

	return AdvancedSumSquareDifferencesBase::patchWithRejectingMask8BitPerChannel(image0, image1, mask0, mask1, tChannels, patchSize, width0, height0, width1, height1, centerX0, centerY0, centerX1, centerY1, image0PaddingElements, image1PaddingElements, mask0PaddingElements, mask1PaddingElements, maskValue);
}

}

}

}

#endif // META_OCEAN_CV_ADVANCED_ADVANCED_SUM_SQUARE_DIFFERENCES_H
