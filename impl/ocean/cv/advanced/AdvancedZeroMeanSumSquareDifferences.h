/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_ADVANCED_ZERO_MEAN_SUM_SQUARE_DIFFERENCES_H
#define META_OCEAN_CV_ADVANCED_ADVANCED_ZERO_MEAN_SUM_SQUARE_DIFFERENCES_H

#include "ocean/cv/advanced/Advanced.h"
#include "ocean/cv/advanced/AdvancedZeroMeanSumSquareDifferencesBase.h"

#include "ocean/cv/PixelPosition.h"

#include "ocean/math/Math.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

/**
 * This class implements zero-mean sum of square difference calculation functions.
 * @ingroup cvadvanced
 */
class OCEAN_CV_ADVANCED_EXPORT AdvancedZeroMeanSumSquareDifferences : public AdvancedZeroMeanSumSquareDifferencesBase
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
};

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t AdvancedZeroMeanSumSquareDifferences::patch8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int width1, const Scalar centerX0, const Scalar centerY0, const Scalar centerX1, const Scalar centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
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

	/*if constexpr (tChannels >= 1u && tChannels <= 4u && tPatchSize == 5u) // not yet implemented
	{
		return AdvancedZeroMeanSumSquareDifferencesSSE::patch8BitPerChannel<tChannels, tPatchSize>(image0, image1, width0, width1, centerX0, centerY0, centerX1, centerY1, image0PaddingElements, image1PaddingElements);
	}*/

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	/*if constexpr (tPatchSize >= 5u) // not yet implemented
	{
		return AdvancedZeroMeanSumSquareDifferencesNEON::patch8BitPerChannel<tChannels, tPatchSize>(image0, image1, width0, width1, centeRX0, centerY0, centeRX1, centerY1, image0PaddingElements, image1PaddingElements);
	}*/

#endif // OCEAN_HARDWARE_SSE_VERSION, OCEAN_HARDWARE_NEON_VERSION

	return AdvancedZeroMeanSumSquareDifferencesBase::patch8BitPerChannelTemplate<tChannels, tPatchSize>(image0, image1, width0, width1, centerX0, centerY0, centerX1, centerY1, image0PaddingElements, image1PaddingElements);
}

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t AdvancedZeroMeanSumSquareDifferences::patch8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const Scalar centerX1, const Scalar centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
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

	/*if constexpr (tChannels >= 1u && tChannels <= 4u && tPatchSize == 5u)
	{
		 return AdvancedZeroMeanSumSquareDifferencesSSE::patch8BitPerChannel<tChannels, tPatchSize>(image0, image1, width0, width1, centerX0, centerY0, centerX1, centerY1, image0PaddingElements, image1PaddingElements);
	}*/

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	/*if constexpr (tPatchSize >= 5u)
	{
		return AdvancedZeroMeanSumSquareDifferencesNEON::patch8BitPerChannel<tChannels, tPatchSize>(image0, image1, width0, width1, centeRX0, centerY0, centeRX1, centerY1, image0PaddingElements, image1PaddingElements); // **TODO**
	}*/

#endif // OCEAN_HARDWARE_SSE_VERSION, OCEAN_HARDWARE_NEON_VERSION

	return AdvancedZeroMeanSumSquareDifferencesBase::patch8BitPerChannelTemplate<tChannels, tPatchSize>(image0, image1, width0, width1, centerX0, centerY0, centerX1, centerY1, image0PaddingElements, image1PaddingElements);
}

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t AdvancedZeroMeanSumSquareDifferences::patchBuffer8BitPerChannel(const uint8_t* image0, const unsigned int width0, const Scalar centerX0, const Scalar centerY0, const unsigned int image0PaddingElements, const uint8_t* buffer1)
{
	static_assert(tChannels != 0u, "Invalid number of image channels!");
	static_assert(tPatchSize >= 1u && tPatchSize % 2u == 1u, "Invalid patch size!");

	ocean_assert(image0 != nullptr && buffer1 != nullptr);

	ocean_assert(width0 >= tPatchSize + 1u);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	ocean_assert_and_suppress_unused(centerX0 >= Scalar(tPatchSize_2) && centerX0 < Scalar(width0 - tPatchSize_2 - 1u), tPatchSize_2);
	ocean_assert(centerY0 >= Scalar(tPatchSize_2));

	constexpr unsigned int tPatchPixels = tPatchSize * tPatchSize;

	uint8_t target[tPatchPixels * tChannels];
	AdvancedFrameInterpolatorBilinear::interpolateSquarePatch8BitPerChannel<tChannels, tPatchSize>(image0, width0, image0PaddingElements, target, Vector2(centerX0, centerY0));
	return ZeroMeanSumSquareDifferences::buffer8BitPerChannel<tChannels, tPatchPixels>(target, buffer1);
}

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t AdvancedZeroMeanSumSquareDifferences::patchMirroredBorderBuffer8BitPerChannel(const uint8_t* image0, const unsigned int width0, const unsigned int height0, const Scalar centerX0, const Scalar centerY0, const unsigned int image0PaddingElements, const uint8_t* buffer1)
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
	return ZeroMeanSumSquareDifferences::buffer8BitPerChannel<tChannels, tPatchPixels>(target, buffer1);
}

}

}

}

#endif // META_OCEAN_CV_ADVANCED_ADVANCED_ZERO_MEAN_SUM_SQUARE_DIFFERENCES_H
