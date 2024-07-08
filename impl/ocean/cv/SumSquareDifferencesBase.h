/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SUM_SQUARE_DIFFERENCES_BASE_H
#define META_OCEAN_CV_SUM_SQUARE_DIFFERENCES_BASE_H

#include "ocean/cv/CV.h"
#include "ocean/cv/CVUtilities.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements several sum square differences functions based e.g., on template parameters.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT SumSquareDifferencesBase
{
	public:

		/**
		 * Returns the sum of square differences between two square patches.
		 * @param image0 The first image in which the first patch is located, must be valid
		 * @param image1 The second image in which the second patch is located, must be valid
		 * @param width0 Width of the first frame in pixels, with range [tPatchSize, infinity)
		 * @param width1 Width of the second frame in pixels, with range [tPatchSize, infinity)
		 * @param centerX0 Horizontal center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize / 2, width0 - tPatchSize / 2 - 1]
		 * @param centerY0 Vertical center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize / 2, height0 - tPatchSize / 2 - 1]
		 * @param centerX1 Horizontal center position of the (tPatchSize x tPatchSize) block in the second frame, with range [tPatchSize / 2, width1 - tPatchSize / 2 - 1]
		 * @param centerY1 Vertical center position of the (tPatchSize x tPatchSize) block in the second frame, with range [tPatchSize / 2, height1 - tPatchSize / 2 - 1]
		 * @param image0PaddingElements The number of padding elements at the end of each first image, in elements, with range [0, infinity)
		 * @param image1PaddingElements The number of padding elements at the end of each second image, in elements, with range [0, infinity)
		 * @return The resulting sum of square differences for tPatchSize * tPatchSize * tChannels elements
		 * @tparam tChannels Specifies the number of channels for the given frames, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static OCEAN_FORCE_INLINE uint32_t patch8BitPerChannelTemplate(const uint8_t* const image0, const uint8_t* const image1, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements);

		/**
		 * Returns the sum of square differences between two square patches.
		 * @param patch0 The top-left corner of the first image patch, must be valid
		 * @param patch1 The top-left corner of the second image patch, must be valid
		 * @param patch0StrideElements The number of elements between two row starts in the first patch, in elements, with range [tPatchSize * tChannels, infinity)
		 * @param patch1StrideElements The number of elements between two row starts in the second patch, in elements, with range [tPatchSize * tChannels, infinity)
		 * @return The resulting sum of square differences for tPatchSize * tPatchSize * tChannels elements
		 * @tparam tChannels Specifies the number of channels for the given frames, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity)
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static uint32_t patch8BitPerChannelTemplate(const uint8_t* patch0, const uint8_t* patch1, const unsigned int patch0StrideElements, const unsigned int patch1StrideElements);

		/**
		 * Returns the sum of square differences between a square image patch and a buffer.
		 * @param image0 The image in which the image patch is located, must be valid
		 * @param width0 Width of the first frame in pixels, with range [tPatchSize, infinity)
		 * @param centerX0 Horizontal center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize / 2, width0 - tPatchSize / 2 - 1]
		 * @param centerY0 Vertical center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize / 2, height0 - tPatchSize / 2 - 1]
		 * @param image0PaddingElements The number of padding elements at the end of each first image, in elements, with range [0, infinity)
		 * @param buffer1 The memory buffer with `tChannels * tPatchSize * tPatchSize` elements, must be valid
		 * @return The resulting sum of square differences for tPatchSize * tPatchSize * tChannels elements, with range [0, infinity)
		 * @tparam tChannels The number of channels for the given frames, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static OCEAN_FORCE_INLINE uint32_t patchBuffer8BitPerChannelTemplate(const uint8_t* image0, const unsigned int width0, const unsigned int centerX0, const unsigned int centerY0, const unsigned int image0PaddingElements, const uint8_t* buffer1);

		/**
		 * Returns the sum of square differences between a square image patch and a buffer.
		 * @param patch0 The top left start position of the image patch, must be valid
		 * @param buffer1 The memory buffer, must be valid
		 * @param patch0StrideElements The number of elements between two rows for the image patch, in elements, with range [tChannels, tPatchSize, infinity)
		 * @return The resulting sum of square differences for tPatchSize * tPatchSize * tChannels elements, with range [0, infinity)
		 * @tparam tChannels The number of channels for the given frames, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static uint32_t patchBuffer8BitPerChannelTemplate(const uint8_t* patch0, const uint8_t* buffer1, const unsigned int patch0StrideElements);

		/**
		 * Returns the sum of square differences between two individual buffers.
		 * @param buffer0 The first buffer, with `tBufferSize` elements, must be valid
		 * @param buffer1 The second buffer, with `tBufferSize` elements, must be valid
		 * @return The resulting sum of square differences, with range [0, infinity)
		 * @tparam tBufferSize The number of elements in each buffer, with range [1, infinity)
		 */
		template <unsigned int tBufferSize>
		static inline uint32_t buffer8BitPerChannelTemplate(const uint8_t* const buffer0, const uint8_t* const buffer1);

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
		static inline IndexPair32 patchAtBorder8BitPerChannelTemplate(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements);

		/**
		 * Returns the sum of square differences between two patches within an image, patch pixels outside the image will be mirrored back into the image.
		 * @param image0 The image in which the first patch is located, must be valid
		 * @param image1 The image in which the second patch is located, must be valid
		 * @param patchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
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
		 * @tparam tChannels Specifies the number of channels for the given frames
		 */
		template <unsigned int tChannels>
		static uint32_t patchMirroredBorder8BitPerChannelTemplate(const uint8_t* image0, const uint8_t* image1, const unsigned int patchSize, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements);

		/**
		 * Returns the sum of square differences between two square patches.
		 * @param image0 The first image in which the first patch is located, must be valid
		 * @param image1 The second image in which the second patch is located, must be valid
		 * @param width0 Width of the first frame in pixels, with range [patchSize, infinity)
		 * @param width1 Width of the second frame in pixels, with range [patchSize, infinity)
		 * @param channels Specifies the number of channels for the given frames, with range [1, infinity)
		 * @param patchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 * @param centerX0 Horizontal center position of the (patchSize x patchSize) block in the first frame, with range [patchSize / 2, width0 - patchSize / 2 - 1]
		 * @param centerY0 Vertical center position of the (patchSize x patchSize) block in the first frame, with range [patchSize / 2, height0 - patchSize / 2 - 1]
		 * @param centerX1 Horizontal center position of the (patchSize x patchSize) block in the second frame, with range [patchSize / 2, width1 - patchSize / 2 - 1]
		 * @param centerY1 Vertical center position of the (patchSize x patchSize) block in the second frame, with range [patchSize / 2, height1 - patchSize / 2 - 1]
		 * @param image0PaddingElements The number of padding elements at the end of each first image, in elements, with range [0, infinity)
		 * @param image1PaddingElements The number of padding elements at the end of each second image, in elements, with range [0, infinity)
		 * @return The resulting sum of square differences for `patchSize * patchSize * channels` elements
		 */
		static OCEAN_FORCE_INLINE uint32_t patch8BitPerChannel(const uint8_t* const image0, const uint8_t* const image1, const unsigned int channels, const unsigned int patchSize, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements);

		/**
		 * Returns the sum of square differences between two square patches.
		 * @param patch0 The top-left corner of the first image patch, must be valid
		 * @param patch1 The top-left corner of the second image patch, must be valid
		 * @param channels Specifies the number of channels for the given frames, with range [1, infinity)
		 * @param patchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 * @param patch0StrideElements The number of elements between two row starts in the first patch, in elements, with range [patchSize * channels, infinity)
		 * @param patch1StrideElements The number of elements between two row starts in the second patch, in elements, with range [patchSize * channels, infinity)
		 * @return The resulting sum of square differences for `patchSize * patchSize * channels` elements
		 */
		static uint32_t patch8BitPerChannel(const uint8_t* patch0, const uint8_t* patch1, const unsigned int channels, const unsigned int patchSize, const unsigned int patch0StrideElements, const unsigned int patch1StrideElements);

		/**
		 * Returns the sum of square differences between a square image patch and a buffer.
		 * @param image0 The image in which the image patch is located, must be valid
		 * @param channels The number of channels for the given frames, with range [1, infinity)
		 * @param patchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 * @param width0 Width of the first frame in pixels, with range [tPatchSize, infinity)
		 * @param centerX0 Horizontal center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize / 2, width0 - tPatchSize / 2 - 1]
		 * @param centerY0 Vertical center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize / 2, height0 - tPatchSize / 2 - 1]
		 * @param image0PaddingElements The number of padding elements at the end of each first image, in elements, with range [0, infinity)
		 * @param buffer1 The memory buffer with `tChannels * tPatchSize * tPatchSize` elements, must be valid
		 * @return The resulting sum of square differences for tPatchSize * tPatchSize * tChannels elements, with range [0, infinity)
		 */
		static OCEAN_FORCE_INLINE uint32_t patchBuffer8BitPerChannel(const uint8_t* image0, unsigned int channels, unsigned int patchSize, const unsigned int width0, const unsigned int centerX0, const unsigned int centerY0, const unsigned int image0PaddingElements, const uint8_t* buffer1);

		/**
		 * Returns the sum of square differences between a square image patch and a buffer.
		 * @param patch0 The top left start position of the image patch, must be valid
		 * @param buffer1 The memory buffer, must be valid
		 * @param channels The number of channels for the given frames, with range [1, infinity)
		 * @param patchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 * @param patch0StrideElements The number of elements between two rows for the image patch, in elements, with range [tChannels, tPatchSize, infinity)
		 * @return The resulting sum of square differences for tPatchSize * tPatchSize * tChannels elements, with range [0, infinity)
		 */
		static inline uint32_t patchBuffer8BitPerChannel(const uint8_t* patch0, const uint8_t* buffer1, unsigned int channels, unsigned int patchSize, const unsigned int patch0StrideElements);

		/**
		 * Returns the sum of square differences between two individual buffers.
		 * @param buffer0 The first buffer, with `size` elements, must be valid
		 * @param buffer1 The second buffer, with `size` elements, must be valid
		 * @param bufferSize The number of elements in each buffer, with range [1, infinity)
		 * @return The sum of square differences between both buffers, with range [0, infinity)
		 */
		static uint32_t buffer8BitPerChannel(const uint8_t* const buffer0, const uint8_t* const buffer1, const unsigned int bufferSize);

		/**
		 * Returns the sum of square differences between two square image patches which can be partially outside of the images.
		 * @param image0 The image in which the first patch is located, must be valid
		 * @param image1 The image in which the second patch is located, must be valid
		 * @param channels The number of frame channels, with range [1, infinity)
		 * @param patchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
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
		 */
		static IndexPair32 patchAtBorder8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const unsigned int channels, const unsigned int patchSize, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements);
};

template <unsigned int tChannels, unsigned int tPatchSize>
OCEAN_FORCE_INLINE uint32_t SumSquareDifferencesBase::patch8BitPerChannelTemplate(const uint8_t* const image0, const uint8_t* const image1, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	static_assert(tPatchSize % 2u == 1u, "Invalid image patch size, need an odd value!");
	static_assert(tChannels > 0u, "Invalid number of frame channels!");

	ocean_assert(image0 != nullptr && image1 != nullptr);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	ocean_assert(centerX0 >= tPatchSize_2 && centerY0 >= tPatchSize_2 && centerX0 < width0 - tPatchSize_2);
	ocean_assert(centerX1 >= tPatchSize_2 && centerY1 >= tPatchSize_2 && centerX1 < width1 - tPatchSize_2);

	ocean_assert(width0 >= tPatchSize);
	ocean_assert(width1 >= tPatchSize);

	const unsigned int image0StrideElements = width0 * tChannels + image0PaddingElements;
	const unsigned int image1StrideElements = width1 * tChannels + image1PaddingElements;

	return patch8BitPerChannelTemplate<tChannels, tPatchSize>(image0 + (centerY0 - tPatchSize_2) * image0StrideElements + (centerX0 - tPatchSize_2) * tChannels, image1 + (centerY1 - tPatchSize_2) * image1StrideElements + (centerX1 - tPatchSize_2) * tChannels, image0StrideElements, image1StrideElements);
}

template <unsigned int tChannels, unsigned int tPatchSize>
uint32_t SumSquareDifferencesBase::patch8BitPerChannelTemplate(const uint8_t* patch0, const uint8_t* patch1, const unsigned int patch0StrideElements, const unsigned int patch1StrideElements)
{
	static_assert(tPatchSize != 0u, "Invalid image patch size, need an odd value!");
	static_assert(tChannels != 0u, "Invalid number of frame channels!");

	ocean_assert(patch0 != nullptr && patch1 != nullptr);

	ocean_assert(patch0StrideElements >= tPatchSize * tChannels);
	ocean_assert(patch1StrideElements >= tPatchSize * tChannels);

	uint32_t result = 0u;
	int16_t value = 0;

	for (unsigned int y = 0u; y < tPatchSize; ++y)
	{
		for (unsigned int n = 0u; n < tPatchSize * tChannels; ++n)
		{
			value = int16_t(patch0[n]) - int16_t(patch1[n]);
			result += value * value;
		}

		patch0 += patch0StrideElements;
		patch1 += patch1StrideElements;
	}

	return result;
}

template <unsigned int tChannels, unsigned int tPatchSize>
OCEAN_FORCE_INLINE uint32_t SumSquareDifferencesBase::patchBuffer8BitPerChannelTemplate(const uint8_t* image0, const unsigned int width0, const unsigned int centerX0, const unsigned int centerY0, const unsigned int image0PaddingElements, const uint8_t* buffer1)
{
	static_assert(tChannels >= 1u, "Invalid number of frame channels!");
	static_assert(tPatchSize % 2u == 1u, "Invalid patch size!");

	ocean_assert(image0 != nullptr && buffer1 != nullptr);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	ocean_assert(centerX0 >= tPatchSize_2 && centerY0 >= tPatchSize_2 && centerX0 < width0 - tPatchSize_2);

	ocean_assert(width0 >= tPatchSize);

	const unsigned int image0StrideElements = width0 * tChannels + image0PaddingElements;

	return patchBuffer8BitPerChannelTemplate<tChannels, tPatchSize>(image0 + (centerY0 - tPatchSize_2) * image0StrideElements + (centerX0 - tPatchSize_2) * tChannels, buffer1, image0StrideElements);
}

template <unsigned int tChannels, unsigned int tPatchSize>
uint32_t SumSquareDifferencesBase::patchBuffer8BitPerChannelTemplate(const uint8_t* patch0, const uint8_t* buffer1, const unsigned int patch0StrideElements)
{
	return patch8BitPerChannelTemplate<tChannels, tPatchSize>(patch0, buffer1, patch0StrideElements, tChannels * tPatchSize);
}

template <unsigned int tBufferSize>
inline uint32_t SumSquareDifferencesBase::buffer8BitPerChannelTemplate(const uint8_t* const buffer0, const uint8_t* const buffer1)
{
	static_assert(tBufferSize != 0u, "Invalid number of frame channels!");

	ocean_assert(buffer0 != nullptr && buffer1 != nullptr);

	int16_t difference;
	uint32_t ssd = 0u;

	for (unsigned int n = 0u; n < tBufferSize; ++n)
	{
		difference = int16_t(buffer0[n]) - int16_t(buffer1[n]);
		ssd += uint32_t(difference * difference);
	}

	return ssd;
}

template <unsigned int tChannels, unsigned int tPatchSize>
inline IndexPair32 SumSquareDifferencesBase::patchAtBorder8BitPerChannelTemplate(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	static_assert(tChannels != 0u, "Invalid number of frame channels!");
	static_assert(tPatchSize % 2u == 1u, "Invalid image patch size, must be odd!");

	ocean_assert(image0 != nullptr && image1 != nullptr);

	const unsigned int tPatchSize_2 = tPatchSize / 2u;

	ocean_assert(width0 >= tPatchSize_2 + 1u && height0 >= tPatchSize_2 + 1u);
	ocean_assert(width1 >= tPatchSize_2 + 1u && height1 >= tPatchSize_2 + 1u);

	ocean_assert(centerX0 < width0 && centerY0 < height0);
	ocean_assert(centerX1 < width1 && centerY1 < height1);

	ocean_assert(centerX0 - tPatchSize_2 < width0 - (tPatchSize - 1u) == (centerX0 >= tPatchSize_2 && centerX0 < width0 - tPatchSize_2));
	ocean_assert(centerY0 - tPatchSize_2 < height0 - (tPatchSize - 1u) == (centerY0 >= tPatchSize_2 && centerY0 < height0 - tPatchSize_2));
	ocean_assert(centerX1 - tPatchSize_2 < width1 - (tPatchSize - 1u) == (centerX1 >= tPatchSize_2 && centerX1 < width1 - tPatchSize_2));
	ocean_assert(centerY1 - tPatchSize_2 < height1 - (tPatchSize - 1u) == (centerY1 >= tPatchSize_2 && centerY1 < height1 - tPatchSize_2));

	if (centerX0 - tPatchSize_2 < width0 - (tPatchSize - 1u) && centerY0 - tPatchSize_2 < height0 - (tPatchSize - 1u) && centerX1 - tPatchSize_2 < width1 - (tPatchSize - 1u) && centerY1 - tPatchSize_2 < height1 - (tPatchSize - 1u))
	{
		const uint32_t ssd = patch8BitPerChannelTemplate<tChannels, tPatchSize>(image0, image1, width0, width1, centerX0, centerY0, centerX1, centerY1, image0PaddingElements, image1PaddingElements);

		return IndexPair32(ssd, tPatchSize * tPatchSize);
	}

	const unsigned int offsetLeft0 = min(centerX0, tPatchSize_2);
	const unsigned int offsetTop0 = min(centerY0, tPatchSize_2);
	const unsigned int offsetRight0 = min(width0 - centerX0 - 1u, tPatchSize_2);
	const unsigned int offsetBottom0 = min(height0 - centerY0 - 1u, tPatchSize_2);

	const unsigned int offsetLeft1 = min(centerX1, tPatchSize_2);
	const unsigned int offsetTop1 = min(centerY1, tPatchSize_2);
	const unsigned int offsetRight1 = min(width1 - centerX1 - 1u, tPatchSize_2);
	const unsigned int offsetBottom1 = min(height1 - centerY1 - 1u, tPatchSize_2);

	if (offsetLeft1 < offsetLeft0 || offsetRight1 < offsetRight0 || offsetTop1 < offsetTop0 || offsetBottom1 < offsetBottom0)
	{
		// at least one pixel in patch0 does not have a corresponding pixel in patch1 which lies inside image1s
		return IndexPair32((unsigned int)(-1), 0u);
	}

	const unsigned int image0StrideElements = width0 * tChannels + image0PaddingElements;
	const unsigned int image1StrideElements = width1 * tChannels + image1PaddingElements;

	const unsigned int columns = offsetRight0 + 1u + offsetLeft0;
	const unsigned int rows = offsetBottom0 + 1u + offsetTop0;

	const uint8_t* patch0 = image0 + (centerY0 - offsetTop0) * image0StrideElements + (centerX0 - offsetLeft0) * tChannels;
	const uint8_t* patch1 = image1 + (centerY1 - offsetTop0) * image1StrideElements + (centerX1 - offsetLeft0) * tChannels; // usage of offsetTop0 and offsetLeft0 is intended

	unsigned int ssd = 0u;

	for (unsigned int y = 0u; y < rows; ++y)
	{
		for (unsigned int x = 0u; x < columns; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				ssd += sqrDistance(patch0[x * tChannels + n], patch1[x * tChannels + n]);
			}
		}

		patch0 += image0StrideElements;
		patch1 += image1StrideElements;
	}

	return IndexPair32(ssd, rows * columns);
}

template <unsigned int tChannels>
uint32_t SumSquareDifferencesBase::patchMirroredBorder8BitPerChannelTemplate(const uint8_t* image0, const uint8_t* image1, const unsigned int patchSize, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	static_assert(tChannels != 0u, "Invalid number of data channels!");

	ocean_assert(image0 != nullptr && image1 != nullptr);
	ocean_assert(patchSize % 2u == 1u);

	const unsigned int patchSize_2 = patchSize / 2u;

	ocean_assert(width0 >= patchSize_2);
	ocean_assert(width1 >= patchSize_2);

	ocean_assert(centerX0 < width0 && centerY0 < height0);
	ocean_assert(centerX1 < width1 && centerY1 < height1);

	const unsigned int image0StrideElements = width0 * tChannels + image0PaddingElements;
	const unsigned int image1StrideElements = width1 * tChannels + image1PaddingElements;

	const int left0 = int(centerX0 - patchSize_2);
	const int top0 = int(centerY0 - patchSize_2);

	const int left1 = int(centerX1 - patchSize_2);
	const int top1 = int(centerY1 - patchSize_2);

	const uint8_t* i0 = image0 + top0 * int(image0StrideElements) + int(left0) * int(tChannels);
	const uint8_t* i1 = image1 + top1 * int(image1StrideElements) + int(left1) * int(tChannels);

	int32_t value;
	uint32_t ssd = 0u;

	unsigned int y0 = centerY0 - patchSize_2;
	unsigned int y1 = centerY1 - patchSize_2;

	const uint8_t* const i0End = i0 + patchSize * image0StrideElements;

	while (i0 != i0End)
	{
		ocean_assert(i0 < i0End);

		const uint8_t* c0 = i0 + CVUtilities::mirrorOffset(y0, height0) * int(image0StrideElements);
		const uint8_t* c1 = i1 + CVUtilities::mirrorOffset(y1, height1) * int(image1StrideElements);

		unsigned int x0 = centerX0 - patchSize_2;
		unsigned int x1 = centerX1 - patchSize_2;

		const uint8_t* const c0RowEnd = c0 + patchSize * tChannels;

		while (c0 != c0RowEnd)
		{
			ocean_assert(c0 < c0RowEnd);

			if (x0 < width0 && x1 < width1)
			{
				// both pixels lie inside the frame

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					value = *c0++ - *c1++;
					ssd += value * value;
				}
			}
			else if (x0 < width0)
			{
				// x0 lies inside the frame

				ocean_assert(x1 >= width1);

				const uint8_t* m1 = c1 + CVUtilities::mirrorOffset(x1, width1) * int(tChannels);

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					value = *c0++ - *m1++;
					ssd += value * value;
				}

				c1 += tChannels;
			}
			else if (x1 < width1)
			{
				// x1 lies inside the frame

				ocean_assert(x0 >= width0);

				const uint8_t* m0 = c0 + CVUtilities::mirrorOffset(x0, width0) * int(tChannels);

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					value = *m0++ - *c1++;
					ssd += value * value;
				}

				c0 += tChannels;
			}
			else
			{
				// neither x0 nor x1 lie inside the frame

				ocean_assert(x0 >= width0 && x1 >= width1);

				const uint8_t* m1 = c1 + CVUtilities::mirrorOffset(x1, width1) * int(tChannels);
				const uint8_t* m0 = c0 + CVUtilities::mirrorOffset(x0, width0) * int(tChannels);

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					value = *m0++ - *m1++;
					ssd += value * value;
				}

				c0 += tChannels;
				c1 += tChannels;
			}

			++x0;
			++x1;
		}

		i0 += image0StrideElements;
		i1 += image1StrideElements;

		++y0;
		++y1;
	}

	return ssd;
}

OCEAN_FORCE_INLINE uint32_t SumSquareDifferencesBase::patch8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const unsigned int channels, const unsigned int patchSize, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	ocean_assert(image0 != nullptr && image1 != nullptr);

	ocean_assert(patchSize % 2u == 1u);
	ocean_assert(channels > 0u);

	const unsigned int patchSize_2 = patchSize / 2u;

	ocean_assert(centerX0 >= patchSize_2 && centerY0 >= patchSize_2 && centerX0 < width0 - patchSize_2);
	ocean_assert(centerX1 >= patchSize_2 && centerY1 >= patchSize_2 && centerX1 < width1 - patchSize_2);

	ocean_assert(width0 >= patchSize_2);
	ocean_assert(width1 >= patchSize_2);

	const unsigned int image0StrideElements = width0 * channels + image0PaddingElements;
	const unsigned int image1StrideElements = width1 * channels + image1PaddingElements;

	return patch8BitPerChannel(image0 + (centerY0 - patchSize_2) * image0StrideElements + (centerX0 - patchSize_2) * channels, image1 + (centerY1 - patchSize_2) * image1StrideElements + (centerX1 - patchSize_2) * channels, channels, patchSize, image0StrideElements, image1StrideElements);
}

OCEAN_FORCE_INLINE uint32_t SumSquareDifferencesBase::patchBuffer8BitPerChannel(const uint8_t* image0, unsigned int channels, unsigned int patchSize, const unsigned int width0, const unsigned int centerX0, const unsigned int centerY0, const unsigned int image0PaddingElements, const uint8_t* buffer1)
{
	ocean_assert(image0 != nullptr && buffer1 != nullptr);

	ocean_assert(channels >= 1u);
	ocean_assert(patchSize % 2u == 1u);

	const unsigned int patchSize_2 = patchSize / 2u;

	ocean_assert(centerX0 >= patchSize_2 && centerY0 >= patchSize_2 && centerX0 < width0 - patchSize_2);

	ocean_assert(width0 >= patchSize);

	const unsigned int image0StrideElements = width0 * channels + image0PaddingElements;

	return patchBuffer8BitPerChannel(image0 + (centerY0 - patchSize_2) * image0StrideElements + (centerX0 - patchSize_2) * channels, buffer1, channels, patchSize, image0StrideElements);
}

inline uint32_t SumSquareDifferencesBase::patchBuffer8BitPerChannel(const uint8_t* patch0, const uint8_t* buffer1, unsigned int channels, unsigned int patchSize, const unsigned int patch0StrideElements)
{
	return patch8BitPerChannel(patch0, buffer1, channels, patchSize, patch0StrideElements, channels * patchSize);
}

}

}

#endif // META_OCEAN_CV_SUM_SQUARE_DIFFERENCES_BASE_H
