/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_ADVANCED_SUM_SQUARE_DIFFERENCES_TEMPLATE_H
#define META_OCEAN_CV_ADVANCED_ADVANCED_SUM_SQUARE_DIFFERENCES_TEMPLATE_H

#include "ocean/cv/advanced/Advanced.h"
#include "ocean/cv/advanced/AdvancedFrameInterpolatorBilinear.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"
#include "ocean/cv/PixelPosition.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

/**
 * This class implements functions calculating the sum of square differences and omit center pixel.
 * @ingroup cvadvanced
 */
class OCEAN_CV_ADVANCED_EXPORT AdvancedSumSquareDifferencesBase
{
	public:

		/**
		 * Returns the sum of square differences for an image patch determined between two individual images.
		 * @param image0 The image in which the first patch is located, must be valid
		 * @param image1 The image in which the second patch is located, must be valid
		 * @param width0 The width of the first image, in pixels, with range [tPatchSize + 1, infinity)
		 * @param width1 The width of the second image, in pixels, with range [tPatchSize + 1, infinity)
		 * @param centerX0 Horizontal center position of the (tSize x tSize) block in the first frame, with range [tPatchSize/2, width - tPatchSize/2 - 1)
		 * @param centerY0 Vertical center position of the (tSize x tSize) block in the first frame, with range [tPatchSize/2, height - tPatchSize/2 - 1)
		 * @param centerX1 Horizontal center position of the (tSize x tSize) block in the second frame, with range [tPatchSize/2, width - tPatchSize/2 - 1)
		 * @param centerY1 Vertical center position of the (tSize x tSize) block in the second frame, with range [tPatchSize/2, height - tPatchSize/2 - 1)
		 * @param image0PaddingElements The number of padding elements at the end of each row of the first image, in elements, with range [0, infinity)
		 * @param image1PaddingElements The number of padding elements at the end of each row of the second image, in elements, with range [0, infinity)
		 * @return The resulting sum of square differences, with range [0, infinity)
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static uint32_t patch8BitPerChannelTemplate(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int width1, const Scalar centerX0, const Scalar centerY0, const Scalar centerX1, const Scalar centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements);

		/**
		 * Returns the sum of square differences for an image patch determined between two individual images.
		 * @param image0 The image in which the first patch is located, must be valid
		 * @param image1 The image in which the second patch is located, must be valid
		 * @param width0 The width of the first image, in pixels, with range [tPatchSize + 1, infinity)
		 * @param width1 The width of the second image, in pixels, with range [tPatchSize + 1, infinity)
		 * @param centerX0 Horizontal center position of the (tSize x tSize) block in the first frame, with range [tPatchSize/2, width - tPatchSize/2)
		 * @param centerY0 Vertical center position of the (tSize x tSize) block in the first frame, with range [tPatchSize/2, height - tPatchSize/2)
		 * @param centerX1 Horizontal center position of the (tSize x tSize) block in the second frame, with range [tPatchSize/2, width - tPatchSize/2 - 1)
		 * @param centerY1 Vertical center position of the (tSize x tSize) block in the second frame, with range [tPatchSize/2, height - tPatchSize/2 - 1)
		 * @param image0PaddingElements The number of padding elements at the end of each row of the first image, in elements, with range [0, infinity)
		 * @param image1PaddingElements The number of padding elements at the end of each row of the second image, in elements, with range [0, infinity)
		 * @return The resulting sum of square differences, with range [0, infinity)
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static uint32_t patch8BitPerChannelTemplate(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const Scalar centerX1, const Scalar centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements);

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
		static inline uint32_t patchBuffer8BitPerChannelTemplate(const uint8_t* image0, const unsigned int width0, const Scalar centerX0, const Scalar centerY0, const unsigned int image0PaddingElements, const uint8_t* buffer1);

		/**
		 * Returns the sum of square differences for an image patch determined for two pixel accurate positions between two individual images.
		 * Further, each image is associated with a binary mask, only valid non-mask pixels are used during SSD calculation.
		 * @param image0 The image in which the first patch is located, must be valid
		 * @param image1 The image in which the second patch is located, must be valid
		 * @param mask0 The mask associated with the first image, must be valid
		 * @param mask1 The mask associated with the second image, must be valid
		 * @param width0 The width of the first image, in pixels, with range [patchSize / 2 + 1, infinity)
		 * @param height0 The height of the first image, in pixels, with range [patchSize / 2 + 1, infinity)
		 * @param width1 The width of the second image, in pixels, with range [patchSize / 2 + 1, infinity)
		 * @param height1 The height of the second image, in pixels, with range [patchSize / 2 + 1, infinity)
		 * @param centerX0 Horizontal sub-pixel center position of the (tPatchSize x tPatchSize) block in the first frame, with range [0, width0 - 1]
		 * @param centerY0 Vertical sub-pixel center position of the (tPatchSize x tPatchSize) block in the first frame, with range [0, height0 - 1]
		 * @param centerX1 Horizontal sub-pixel center position of the (tPatchSize x tPatchSize) block in the second frame, with range [0, width1 - 1]
		 * @param centerY1 Vertical sub-pixel center position of the (tPatchSize x tPatchSize) block in the second frame, with range [0, height1 - 1]
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
		static IndexPair32 patchWithMask8BitPerChannelTemplate(const uint8_t* image0, const uint8_t* image1, const uint8_t* mask0, const uint8_t* mask1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements, const unsigned int mask0PaddingElements, const unsigned int mask1PaddingElements, const uint8_t maskValue);

		/**
		 * Returns the sum of square differences for an image patch determined for two sub-pixel accurate positions between two individual images.
		 * Further, each image is associated with a binary mask, only valid non-mask pixels in both images are used during SSD calculation.
		 * @param image0 The image in which the first patch is located, must be valid
		 * @param image1 The image in which the second patch is located, must be valid
		 * @param mask0 The mask associated with the first image, must be valid
		 * @param mask1 The mask associated with the second image, must be valid
		 * @param width0 The width of the first image, in pixels, with range [patchSize / 2 + 1, infinity)
		 * @param height0 The height of the first image, in pixels, with range [patchSize / 2 + 1, infinity)
		 * @param width1 The width of the second image, in pixels, with range [patchSize / 2 + 1, infinity)
		 * @param height1 The height of the second image, in pixels, with range [patchSize / 2 + 1, infinity)
		 * @param centerX0 Horizontal sub-pixel center position of the (tPatchSize x tPatchSize) block in the first frame, with range (-infinity, infinity)
		 * @param centerY0 Vertical sub-pixel center position of the (tPatchSize x tPatchSize) block in the first frame, with range (-infinity, infinity)
		 * @param centerX1 Horizontal sub-pixel center position of the (tPatchSize x tPatchSize) block in the second frame, with range (-infinity, infinity)
		 * @param centerY1 Vertical sub-pixel center position of the (tPatchSize x tPatchSize) block in the second frame, with range (-infinity, infinity)
		 * @param image0PaddingElements The number of padding elements at the end of each row of the first image, in elements, with range [0, infinity)
		 * @param image1PaddingElements The number of padding elements at the end of each row of the second image, in elements, with range [0, infinity)
		 * @param mask0PaddingElements The number of padding elements at the end of each row of the first mask, in elements, with range [0, infinity)
		 * @param mask1PaddingElements The number of padding elements at the end of each row of the second mask, in elements, with range [0, infinity)
		 * @param maskValue The pixel value of a mask pixel which will be excluded when calculating the SSD, with range [0, 255]
		 * @return The resulting pair holding first: sum of square differences, second: number of pixels contributed to the result
		 * @tparam tChannels The number of data channels of the frames, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static IndexPair32 patchWithMask8BitPerChannelTemplate(const uint8_t* image0, const uint8_t* image1, const uint8_t* mask0, const uint8_t* mask1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const Scalar centerX0, const Scalar centerY0, const Scalar centerX1, const Scalar centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements, const unsigned int mask0PaddingElements, const unsigned int mask1PaddingElements, const uint8_t maskValue);

		/**
		 * Returns the sum of square differences between two square patches with sub-pixel accuracy.
		 * @param image0 The first image in which the first patch is located, must be valid
		 * @param image1 The second image in which the second patch is located, must be valid
		 * @param channels Specifies the number of channels for the given frames, with range [1, infinity)
		 * @param patchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 * @param width0 Width of the first frame in pixels, with range [patchSize + 1, infinity)
		 * @param width1 Width of the second frame in pixels, with range [patchSize, infinity)
		 * @param centerX0 Horizontal center position of the (patchSize x patchSize) block in the first frame, with range [patchSize/2, width - patchSize/2 - 1)
		 * @param centerY0 Vertical center position of the (patchSize x patchSize) block in the first frame, with range [patchSize/2, height - patchSize/2 - 1)
		 * @param centerX1 Horizontal center position of the (patchSize x patchSize) block in the second frame, with range [patchSize/2, width - patchSize/2 - 1)
		 * @param centerY1 Vertical center position of the (patchSize x patchSize) block in the second frame, with range [patchSize/2, height - patchSize/2 - 1)
		 * @param image0PaddingElements The number of padding elements at the end of each first image, in elements, with range [0, infinity)
		 * @param image1PaddingElements The number of padding elements at the end of each second image, in elements, with range [0, infinity)
		 * @return The resulting sum of square differences for `patchSize * patchSize * channels` elements
		 */
		static uint32_t patch8BitPerChannel(const uint8_t* const image0, const uint8_t* const image1, const unsigned int channels, const unsigned int patchSize, const unsigned int width0, const unsigned int width1, const Scalar centerX0, const Scalar centerY0, const Scalar centerX1, const Scalar centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements);

		/**
		 * Returns the sum of square differences between two square patches, one with sub-pixel accuracy, one with pixel accuracy.
		 * @param image0 The first image in which the first patch is located, must be valid
		 * @param image1 The second image in which the second patch is located, must be valid
		 * @param channels Specifies the number of channels for the given frames, with range [1, infinity)
		 * @param patchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 * @param width0 Width of the first frame in pixels, with range [patchSize, infinity)
		 * @param width1 Width of the second frame in pixels, with range [patchSize + 1, infinity)
		 * @param centerX0 Horizontal center position of the (patchSize x patchSize) block in the first frame, with range [patchSize/2, width - patchSize/2)
		 * @param centerY0 Vertical center position of the (patchSize x patchSize) block in the first frame, with range [patchSize/2, height - patchSize/2)
		 * @param centerX1 Horizontal center position of the (patchSize x patchSize) block in the second frame, with range [patchSize/2, width - patchSize/2 - 1)
		 * @param centerY1 Vertical center position of the (patchSize x patchSize) block in the second frame, with range [patchSize/2, height - patchSize/2 - 1)
		 * @param image0PaddingElements The number of padding elements at the end of each first image, in elements, with range [0, infinity)
		 * @param image1PaddingElements The number of padding elements at the end of each second image, in elements, with range [0, infinity)
		 * @return The resulting sum of square differences for `patchSize * patchSize * channels` elements
		 */
		static uint32_t patch8BitPerChannel(const uint8_t* const image0, const uint8_t* const image1, const unsigned int channels, const unsigned int patchSize, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const Scalar centerX1, const Scalar centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements);

		/**
		 * Returns the sum of square differences between one sub-pixel image patch and a memory buffer.
		 * @param image0 The first image in which the first patch is located, must be valid
		 * @param channels Specifies the number of channels for the given frames, with range [1, infinity)
		 * @param patchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 * @param width0 Width of the first frame in pixels, with range [patchSize, infinity)
		 * @param centerX0 Horizontal center position of the (patchSize x patchSize) block in the first frame, with range [patchSize/2, width - patchSize/2 - 1)
		 * @param centerY0 Vertical center position of the (patchSize x patchSize) block in the first frame, with range [patchSize/2, height - patchSize/2 - 1)
		 * @param image0PaddingElements The number of padding elements at the end of each first image, in elements, with range [0, infinity)
		 * @param buffer1 The memory buffer with `tChannels * tPatchSize * tPatchSize` elements, must be valid
		 * @return The resulting sum of square differences for `patchSize * patchSize * channels` elements
		 */
		static uint32_t patchBuffer8BitPerChannel(const uint8_t* const image0, const unsigned int channels, const unsigned int patchSize, const unsigned int width0, const Scalar centerX0, const Scalar centerY0, const unsigned int image0PaddingElements, const uint8_t* buffer1);

		/**
		 * Returns the sum of square differences for an image patch determined for two pixel accurate positions between two individual images.
		 * Further, each image is associated with a binary mask, only valid non-mask pixels are used during SSD calculation.
		 * @param image0 The image in which the first patch is located, must be valid
		 * @param image1 The image in which the second patch is located, must be valid
		 * @param mask0 The mask associated with the first image, must be valid
		 * @param mask1 The mask associated with the second image, must be valid
		 * @param channels The number of channels for the given frames, with range [1, infinity)
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
		 */
		static IndexPair32 patchWithMask8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const uint8_t* mask0, const uint8_t* mask1, const unsigned int channels, const unsigned int patchSize, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements, const unsigned int mask0PaddingElements, const unsigned int mask1PaddingElements, const uint8_t maskValue);

		/**
		 * Returns the sum of square differences for an image patch determined for two pixel accurate positions between two individual images .
		 * Further, each image is associated with a binary mask, only valid non-mask pixels in the first image are used during SSD calculation.
		 * If a valid (mask) pixel in the first image does not have a valid (mask) pixel in the second frame, -1 is returned as SSD value.
		 * @param image0 The image in which the first patch is located, must be valid
		 * @param image1 The image in which the second patch is located, must be valid
		 * @param mask0 The mask associated with the first image, must be valid
		 * @param mask1 The mask associated with the second image, must be valid
		 * @param channels The number of channels for the given frames, with range [1, infinity)
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
		 */
		static IndexPair32 patchWithRejectingMask8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const uint8_t* mask0, const uint8_t* mask1, const unsigned int channels, const unsigned int patchSize, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements, const unsigned int mask0PaddingElements, const unsigned int mask1PaddingElements, const uint8_t maskValue);
};

template <unsigned int tChannels, unsigned int tPatchSize>
uint32_t AdvancedSumSquareDifferencesBase::patch8BitPerChannelTemplate(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int width1, const Scalar centerX0, const Scalar centerY0, const Scalar centerX1, const Scalar centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	static_assert(tChannels != 0u, "Invalid number of frame channels!");
	static_assert(tPatchSize >= 1u && tPatchSize % 2u == 1u, "Invalid image patch size, must be odd!");

	ocean_assert(image0 != nullptr && image1 != nullptr);

	ocean_assert(width0 >= tPatchSize + 1u);
	ocean_assert(width1 >= tPatchSize + 1u);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	ocean_assert(centerX0 >= Scalar(tPatchSize_2) && centerX0 < Scalar(width0 - tPatchSize_2 - 1u));
	ocean_assert_and_suppress_unused(centerY0 >= Scalar(tPatchSize_2), tPatchSize_2);

	ocean_assert(centerX1 >= Scalar(tPatchSize_2) && centerX1 < Scalar(width1 - tPatchSize_2 - 1u));
	ocean_assert(centerY1 >= Scalar(tPatchSize_2));

	constexpr unsigned int tPatchElements = tPatchSize * tPatchSize * tChannels;

	uint8_t target[tPatchElements * 2u];

	AdvancedFrameInterpolatorBilinear::interpolateSquarePatch8BitPerChannel<tChannels, tPatchSize>(image0, width0, image0PaddingElements, target, Vector2(centerX0, centerY0));
	AdvancedFrameInterpolatorBilinear::interpolateSquarePatch8BitPerChannel<tChannels, tPatchSize>(image1, width1, image1PaddingElements, target + tPatchElements, Vector2(centerX1, centerY1));

	return SumSquareDifferences::buffer8BitPerChannelTemplate<tPatchElements>(target, target + tPatchElements);
}

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t AdvancedSumSquareDifferencesBase::patch8BitPerChannelTemplate(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const Scalar centerX1, const Scalar centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	static_assert(tChannels != 0u, "Invalid number of frame channels!");
	static_assert(tPatchSize >= 1u && tPatchSize % 2u == 1u, "Invalid image patch size, must be odd!");

	ocean_assert(image0 != nullptr && image1 != nullptr);

	ocean_assert(width0 >= tPatchSize);
	ocean_assert(width1 >= tPatchSize + 1u);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	ocean_assert(centerX0 >= tPatchSize_2 && centerX0 < width0 - tPatchSize_2);
	ocean_assert_and_suppress_unused(centerY0 >= tPatchSize_2, tPatchSize_2);

	ocean_assert(centerX1 >= Scalar(tPatchSize_2) && centerX1 < Scalar(width1 - tPatchSize_2 - 1u));
	ocean_assert(centerY1 >= Scalar(tPatchSize_2));

	constexpr unsigned int tPatchPixels = tPatchSize * tPatchSize;

	constexpr unsigned int tPatchElements = tPatchPixels * tChannels;

	uint8_t target[tPatchElements * 2u];

	constexpr unsigned int targetPaddingElements = 0u;

	CV::FrameConverter::patchFrame<uint8_t>(image0, target, width0, tChannels, centerX0, centerY0, tPatchSize, image0PaddingElements, targetPaddingElements);
	AdvancedFrameInterpolatorBilinear::interpolateSquarePatch8BitPerChannel<tChannels, tPatchSize>(image1, width1, image1PaddingElements, target + tPatchElements, Vector2(centerX1, centerY1));

	return SumSquareDifferences::buffer8BitPerChannel<tChannels, tPatchPixels>(target, target + tPatchElements);
}

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t AdvancedSumSquareDifferencesBase::patchBuffer8BitPerChannelTemplate(const uint8_t* image0, const unsigned int width0, const Scalar centerX0, const Scalar centerY0, const unsigned int image0PaddingElements, const uint8_t* buffer1)
{
	static_assert(tChannels != 0u, "Invalid number of image channels!");
	static_assert(tPatchSize >= 1u && tPatchSize % 2u == 1u, "Invalid patch size!");

	ocean_assert(image0 != nullptr && buffer1 != nullptr);

	ocean_assert(width0 >= tPatchSize + 1u);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	ocean_assert_and_suppress_unused(centerX0 >= Scalar(tPatchSize_2) && centerX0 < Scalar(width0 - tPatchSize_2 - 1u), tPatchSize_2);

	constexpr unsigned int tPatchPixels = tPatchSize * tPatchSize;

	uint8_t target[tPatchPixels * tChannels];
	AdvancedFrameInterpolatorBilinear::interpolateSquarePatch8BitPerChannel<tChannels, tPatchSize>(image0, width0, image0PaddingElements, target, Vector2(centerX0, centerY0));
	return SumSquareDifferences::buffer8BitPerChannel<tChannels, tPatchPixels>(target, buffer1);
}

template <unsigned int tChannels, unsigned int tPatchSize>
IndexPair32 AdvancedSumSquareDifferencesBase::patchWithMask8BitPerChannelTemplate(const uint8_t* image0, const uint8_t* image1, const uint8_t* mask0, const uint8_t* mask1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements, const unsigned int mask0PaddingElements, const unsigned int mask1PaddingElements, const uint8_t maskValue)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize >= 1u && tPatchSize % 2u == 1u, "Invalid patch size!");

	ocean_assert(image0 != nullptr && image1 != nullptr);
	ocean_assert(mask0 != nullptr && mask1 != nullptr);

	const unsigned int patchSize_2 = tPatchSize / 2u;

	ocean_assert(width0 >= patchSize_2 + 1u && height0 >= patchSize_2 + 1u);
	ocean_assert(width1 >= patchSize_2 + 1u && height1 >= patchSize_2 + 1u);

	ocean_assert(centerX0 < width0 && centerY0 < height0);
	ocean_assert(centerX1 < width1 && centerY1 < height1);

	const unsigned int image0StrideElements = width0 * tChannels + image0PaddingElements;
	const unsigned int image1StrideElements = width1 * tChannels + image1PaddingElements;

	const unsigned int mask0StrideElements = width0 + mask0PaddingElements;
	const unsigned int mask1StrideElements = width1 + mask1PaddingElements;

	const unsigned int offsetLeft = std::min(std::min(centerX0, patchSize_2), std::min(centerX1, patchSize_2));
	const unsigned int offsetTop = std::min(std::min(centerY0, patchSize_2), std::min(centerY1, patchSize_2));

	const unsigned int offsetRight = std::min(std::min(width0 - centerX0 - 1u, patchSize_2), std::min(width1 - centerX1 - 1u, patchSize_2));
	const unsigned int offsetBottom = std::min(std::min(height0 - centerY0 - 1u, patchSize_2), std::min(height1 - centerY1 - 1u, patchSize_2));

	ocean_assert(offsetLeft <= patchSize_2 && offsetRight <= patchSize_2);
	ocean_assert(offsetTop <= patchSize_2 && offsetTop <= patchSize_2);

	ocean_assert(offsetLeft <= centerX0 && offsetTop <= centerY0);
	ocean_assert(offsetTop <= centerY1 && offsetTop <= centerY1);

	ocean_assert(centerX0 + offsetRight < width0 && centerY0 + offsetBottom < height0);
	ocean_assert(centerX1 + offsetRight < width1 && centerY1 + offsetBottom < height1);

	const unsigned int patchWidth = offsetLeft + offsetRight + 1u;
	const unsigned int patchHeight = offsetTop + offsetBottom + 1u;

	ocean_assert(patchWidth <= tPatchSize && patchHeight <= tPatchSize);

	image0 += (centerY0 - offsetTop) * image0StrideElements + (centerX0 - offsetLeft) * tChannels;
	image1 += (centerY1 - offsetTop) * image1StrideElements + (centerX1 - offsetLeft) * tChannels;

	mask0 += (centerY0 - offsetTop) * mask0StrideElements + (centerX0 - offsetLeft);
	mask1 += (centerY1 - offsetTop) * mask1StrideElements + (centerX1 - offsetLeft);

	uint32_t validPixels = 0u;
	uint32_t ssd = 0u;

	for (unsigned int y = 0u; y < patchHeight; ++y)
	{
		for (unsigned int x = 0u; x < patchWidth; ++x)
		{
			if (mask0[x] != maskValue && mask1[x] != maskValue)
			{
				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					ssd += sqr(image0[tChannels * x + n] - image1[tChannels * x + n]);
				}

				++validPixels;
			}
		}

		image0 += image0StrideElements;
		image1 += image1StrideElements;

		mask0 += mask0StrideElements;
		mask1 += mask1StrideElements;
	}

	return IndexPair32(ssd, validPixels);
}

template <unsigned int tChannels, unsigned int tPatchSize>
inline IndexPair32 AdvancedSumSquareDifferencesBase::patchWithMask8BitPerChannelTemplate(const uint8_t* image0, const uint8_t* image1, const uint8_t* mask0, const uint8_t* mask1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const Scalar centerX0, const Scalar centerY0, const Scalar centerX1, const Scalar centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements, const unsigned int mask0PaddingElements, const unsigned int mask1PaddingElements, const uint8_t maskValue)
{
	static_assert(tChannels != 0u, "Invalid number of frame channels!");

	ocean_assert(image0 != nullptr && image1 != nullptr);
	ocean_assert(mask0 != nullptr && mask1 != nullptr);

	uint8_t patchBuffers[tPatchSize * tPatchSize * tChannels * 2u];
	uint8_t maskBuffers[tPatchSize * tPatchSize * 2u];

	uint8_t* patchBuffer0 = patchBuffers;
	uint8_t* patchBuffer1 = patchBuffers + tPatchSize * tPatchSize * tChannels;

	uint8_t* maskBuffer0 = maskBuffers;
	uint8_t* maskBuffer1 = maskBuffers + tPatchSize * tPatchSize;

	constexpr unsigned int patchBuffer0PaddingElements = 0u;
	constexpr unsigned int patchBuffer1PaddingElements = 0u;

	constexpr unsigned int maskBuffer0PaddingElements = 0u;
	constexpr unsigned int maskBuffer1PaddingElements = 0u;

	const uint8_t validMaskValue = 0xFFu - maskValue;

	AdvancedFrameInterpolatorBilinear::interpolatePatchWithMask8BitPerChannel<tChannels, PC_CENTER>(image0, mask0, width0, height0, image0PaddingElements, mask0PaddingElements, Vector2(centerX0, centerY0), patchBuffer0, maskBuffer0, tPatchSize, tPatchSize, patchBuffer0PaddingElements, maskBuffer0PaddingElements, validMaskValue);
	AdvancedFrameInterpolatorBilinear::interpolatePatchWithMask8BitPerChannel<tChannels, PC_CENTER>(image1, mask1, width1, height1, image1PaddingElements, mask1PaddingElements, Vector2(centerX1, centerY1), patchBuffer1, maskBuffer1, tPatchSize, tPatchSize, patchBuffer1PaddingElements, maskBuffer1PaddingElements, validMaskValue);

	uint32_t ssd = 0u;
	uint32_t validPixels = 0u;

	for (unsigned int y = 0u; y < tPatchSize; ++y)
	{
		for (unsigned int x = 0u; x < tPatchSize; ++x)
		{
			if (*maskBuffer0 == validMaskValue && *maskBuffer1 == validMaskValue)
			{
				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					ssd += sqrDistance(patchBuffer0[n], patchBuffer1[n]);
				}

				++validPixels;
			}

			patchBuffer0 += tChannels;
			patchBuffer1 += tChannels;

			++maskBuffer0;
			++maskBuffer1;
		}
	}

	return IndexPair32(ssd, validPixels);
}

}

}

}

#endif // META_OCEAN_CV_ADVANCED_ADVANCED_SUM_SQUARE_DIFFERENCES_TEMPLATE_H
