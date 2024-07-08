/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SUM_SQUARE_DIFFERENCES_H
#define META_OCEAN_CV_SUM_SQUARE_DIFFERENCES_H

#include "ocean/cv/CV.h"
#include "ocean/cv/SumSquareDifferencesBase.h"
#include "ocean/cv/SumSquareDifferencesNEON.h"
#include "ocean/cv/SumSquareDifferencesSSE.h"

#include "ocean/base/Messenger.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements functions calculating the sum of square differences.
 * @ingroup cv
 */
class SumSquareDifferences : public SumSquareDifferencesBase
{
	public:

		/**
		 * Returns the sum of square differences between two square image patches.
		 * @param image0 The image in which the first patch is located, must be valid
		 * @param image1 The image in which the second patch is located, must be valid
		 * @param width0 The width of the first image, in pixels, with range [tPatchSize, infinity)
		 * @param width1 The width of the second image, in pixels, with range [tPatchSize, infinity)
		 * @param centerX0 Horizontal center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize/2, width - tPatchSize/2 - 1]
		 * @param centerY0 Vertical center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize/2, height - tPatchSize/2 - 1]
		 * @param centerX1 Horizontal center position of the (tPatchSize x tPatchSize) block in the second frame, with range [tPatchSize/2, width - tPatchSize/2 - 1]
		 * @param centerY1 Vertical center position of the (tPatchSize x tPatchSize) block in the second frame, with range [tPatchSize/2, height - tPatchSize/2 - 1]
		 * @param image0PaddingElements The number of padding elements at the end of each row of the first image, in elements, with range [0, infinity)
		 * @param image1PaddingElements The number of padding elements at the end of each row of the second image, in elements, with range [0, infinity)
		 * @return The resulting sum of square differences, with range [0, infinity)
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static inline uint32_t patch8BitPerChannel(const uint8_t* const image0, const uint8_t* const image1, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements);

		/**
		 * Returns the sum of square differences between an image patch and a memory buffer.
		 * @param image0 The image in which the image patch is located, must be valid
		 * @param width0 Width of the first frame in pixels, with range [tPatchSize, infinity)
		 * @param centerX0 Horizontal center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize / 2, width0 - tPatchSize / 2 - 1]
		 * @param centerY0 Vertical center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize / 2, height0 - tPatchSize / 2 - 1]
		 * @param image0PaddingElements The number of padding elements at the end of each row of the first image, in elements, with range [0, infinity)
		 * @param buffer1 The memory buffer with `tChannels * tPatchSize * tPatchSize` elements, must be valid
		 * @return The resulting sum of square differences for tPatchSize * tPatchSize * tChannels elements, with range [0, infinity)
		 * @tparam tChannels The number of channels for the given frames, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static inline unsigned int patchBuffer8BitPerChannel(const uint8_t* const image0, const unsigned int width0, const unsigned int centerX0, const unsigned int centerY0, const unsigned int image0PaddingElements, const uint8_t* const buffer1);

		/**
		 * Returns the sum of square differences between two memory buffers.
		 * @param buffer0 The first memory buffer, must be valid
		 * @param buffer1 The second memory buffer, must be valid
		 * @return The resulting sum of square differences
		 * @tparam tChannels The number of channels the buffers have, with range [1, infinity)
		 * @tparam tPixels The number of pixels the buffer holds, in pixels, with range [1, infinity)
		 */
		template <unsigned int tChannels, unsigned int tPixels>
		static inline uint32_t buffer8BitPerChannel(const uint8_t* const buffer0, const uint8_t* const buffer1);

		/**
		 * Returns the sum of square differences between two square image patches which can be partially outside of the images.
		 * @param image0 The image in which the first patch is located, must be valid
		 * @param image1 The image in which the second patch is located, must be valid
		 * @param width0 The width of the first image, in pixels, with range [tPatchSize/2 + 1, infinity)
		 * @param height0 The height of the first image, in pixels, with range [tPatchSize/2 + 1, infinity)
		 * @param width1 The width of the second image, in pixels, with range [tPatchSize/2 + 1, infinity)
		 * @param height1 The height of the second image, in pixels, with range [tPatchSize/2 + 1, infinity)
		 * @param centerX0 Horizontal center position of the (tPatchSize x tPatchSize) block in the first frame, with range [0, width0 - 1]
		 * @param centerY0 Vertical center position of the (tPatchSize x tPatchSize) block in the first frame, with range [0, height0 - 1]
		 * @param centerX1 Horizontal center position of the (tPatchSize x tPatchSize) block in the second frame, with range [0, width1 - 1]
		 * @param centerY1 Vertical center position of the (tPatchSize x tPatchSize) block in the second frame, with range [0, height1 - 1]
		 * @param image0PaddingElements The number of padding elements at the end of each row of the first image, in elements, with range [0, infinity)
		 * @param image1PaddingElements The number of padding elements at the end of each row of the second image, in elements, with range [0, infinity)
		 * @return Pair holding the resulting ssd and the number of pixels which contributed to the ssd, an ssd of -1 if a patch0 pixel does not have a corresponding patch1 pixel
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static inline IndexPair32 patchAtBorder8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements);

		/**
		 * Returns the sum of square differences between two patches within an image, patch pixels outside the image will be mirrored back into the image.
		 * @param image0 The image in which the first patch is located, must be valid
		 * @param image1 The image in which the second patch is located, must be valid
		 * @param width0 The width of the first image, in pixels, with range [tPatchSize, infinity)
		 * @param height0 The height of the first image, in pixels, with range [tPatchSize, infinity)
		 * @param width1 The width of the second image, in pixels, with range [tPatchSize, infinity)
		 * @param height1 The height of the second image, in pixels, with range [tPatchSize, infinity)
		 * @param centerX0 Horizontal center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize/2, width - tPatchSize/2 - 1]
		 * @param centerY0 Vertical center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize/2, height - tPatchSize/2 - 1]
		 * @param centerX1 Horizontal center position of the (tPatchSize x tPatchSize) block in the second frame, with range [tPatchSize/2, width - tPatchSize/2 - 1]
		 * @param centerY1 Vertical center position of the (tPatchSize x tPatchSize) block in the second frame, with range [tPatchSize/2, height - tPatchSize/2 - 1]
		 * @param image0PaddingElements The number of padding elements at the end of each row of the first image, in elements, with range [0, infinity)
		 * @param image1PaddingElements The number of padding elements at the end of each row of the second image, in elements, with range [0, infinity)
		 * @return The resulting sum of square differences, with range [0, infinity)
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static uint32_t patchMirroredBorder8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements);
};

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t SumSquareDifferences::patch8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize >= 1u, "Invalid patch size!");

	ocean_assert(image0 != nullptr && image1 != nullptr);

	ocean_assert(width0 >= tPatchSize);
	ocean_assert(width1 >= tPatchSize);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	ocean_assert(centerX0 >= tPatchSize_2 && centerY0 >= tPatchSize_2);
	ocean_assert(centerX1 >= tPatchSize_2 && centerY1 >= tPatchSize_2);

	ocean_assert(centerX0 < width0 - tPatchSize_2);
	ocean_assert(centerX1 < width1 - tPatchSize_2);

	const unsigned int image0StrideElements = width0 * tChannels + image0PaddingElements;
	const unsigned int image1StrideElements = width1 * tChannels + image1PaddingElements;

	const uint8_t* const patch0 = image0 + (centerY0 - tPatchSize_2) * image0StrideElements + (centerX0 - tPatchSize_2) * tChannels;
	const uint8_t* const patch1 = image1 + (centerY1 - tPatchSize_2) * image1StrideElements + (centerX1 - tPatchSize_2) * tChannels;

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	if constexpr (tPatchSize >= 5u)
	{
		return SumSquareDifferencesSSE::patch8BitPerChannel<tChannels, tPatchSize>(patch0, patch1, image0StrideElements, image1StrideElements);
	}

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	if constexpr (tPatchSize >= 5u)
	{
		return SumSquareDifferencesNEON::patch8BitPerChannel<tChannels, tPatchSize>(patch0, patch1, image0StrideElements, image1StrideElements);
	}

#endif // OCEAN_HARDWARE_SSE_VERSION, OCEAN_HARDWARE_NEON_VERSION

	return SumSquareDifferencesBase::patch8BitPerChannelTemplate<tChannels, tPatchSize>(patch0, patch1, image0StrideElements, image1StrideElements);
}

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t SumSquareDifferences::patchBuffer8BitPerChannel(const uint8_t* const image0, const unsigned int width0, const unsigned int centerX0, const unsigned int centerY0, const unsigned int image0PaddingElements, const uint8_t* const buffer1)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize % 2u == 1u, "Invalid patch size!");

	ocean_assert(image0 != nullptr && buffer1 != nullptr);

	ocean_assert(width0 >= tPatchSize);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	ocean_assert(centerX0 >= tPatchSize_2 && centerY0 >= tPatchSize_2);

	ocean_assert(centerX0 < width0 - tPatchSize_2);

	const unsigned int image0StrideElements = width0 * tChannels + image0PaddingElements;

	const uint8_t* const patch0 = image0 + (centerY0 - tPatchSize_2) * image0StrideElements + (centerX0 - tPatchSize_2) * tChannels;

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	if constexpr (tPatchSize >= 5u)
	{
		return SumSquareDifferencesSSE::patchBuffer8BitPerChannel<tChannels, tPatchSize>(patch0, buffer1, image0StrideElements);
	}

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	if constexpr (tPatchSize >= 5u)
	{
		return SumSquareDifferencesNEON::patchBuffer8BitPerChannel<tChannels, tPatchSize>(patch0, buffer1, image0StrideElements);
	}

#endif // OCEAN_HARDWARE_SSE_VERSION, OCEAN_HARDWARE_NEON_VERSION

	return SumSquareDifferencesBase::patchBuffer8BitPerChannelTemplate<tChannels, tPatchSize>(patch0, buffer1, image0StrideElements);
}

template <unsigned int tChannels, unsigned int tPixels>
inline uint32_t SumSquareDifferences::buffer8BitPerChannel(const uint8_t* const buffer0, const uint8_t* const buffer1)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPixels >= 1u, "Invalid pixel number!");

	constexpr unsigned int tElements = tChannels * tPixels;

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	if constexpr (tElements >= 15u)
	{
		return SumSquareDifferencesSSE::buffer8BitPerChannel<tElements>(buffer0, buffer1);
	}

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	if constexpr (tElements >= 8u)
	{
		return SumSquareDifferencesNEON::buffer8BitPerChannel<tElements>(buffer0, buffer1);
	}

#endif // OCEAN_HARDWARE_SSE_VERSION, OCEAN_HARDWARE_NEON_VERSION

	return SumSquareDifferencesBase::buffer8BitPerChannelTemplate<tElements>(buffer0, buffer1);
}

template <unsigned int tChannels, unsigned int tPatchSize>
inline IndexPair32 SumSquareDifferences::patchAtBorder8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize % 2u == 1u, "Invalid patch size!");

	return SumSquareDifferencesBase::patchAtBorder8BitPerChannelTemplate<tChannels, tPatchSize>(image0, image1, width0, height0, width1, height1, centerX0, centerY0, centerX1, centerY1, image0PaddingElements, image1PaddingElements);
}

template <unsigned int tChannels, unsigned int tPatchSize>
uint32_t SumSquareDifferences::patchMirroredBorder8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize % 2u == 1u, "Invalid patch size!");

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	if constexpr (tPatchSize >= 5u)
	{
		return SumSquareDifferencesNEON::patchMirroredBorder8BitPerChannel<tChannels, tPatchSize>(image0, image1, width0, height0, width1, height1, centerX0, centerY0, centerX1, centerY1, image0PaddingElements, image1PaddingElements);
	}

#endif // OCEAN_HARDWARE_SSE_VERSION, OCEAN_HARDWARE_NEON_VERSION

	return SumSquareDifferencesBase::patchMirroredBorder8BitPerChannelTemplate<tChannels>(image0, image1, tPatchSize, width0, height0, width1, height1, centerX0, centerY0, centerX1, centerY1, image0PaddingElements, image1PaddingElements);
}

}

}

#endif // META_OCEAN_CV_SUM_SQUARE_DIFFERENCES_H
