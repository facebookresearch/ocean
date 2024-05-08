/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SUM_ZERO_MEAN_SQUARE_DIFFERENCES_BASE_H
#define META_OCEAN_CV_SUM_ZERO_MEAN_SQUARE_DIFFERENCES_BASE_H

#include "ocean/cv/CV.h"
#include "ocean/cv/CVUtilities.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements several zero-mean sum square differences functions based e.g., on template parameters.
 * @ingroup cv
 */
class ZeroMeanSumSquareDifferencesBase
{
	public:

		/**
		 * Returns the zero-mean sum of square differences between two square patches.
		 * @param image0 The first image in which the first patch is located, must be valid
		 * @param image1 The second image in which the second patch is located, must be valid
		 * @param width0 Width of the first frame in pixels, with range [tPatchSize, infinity)
		 * @param width1 Width of the second frame in pixels, with range [tPatchSize, infinity)
		 * @param centerX0 Horizontal center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize / 2, width0 - tPatchSize / 2 - 1]
		 * @param centerY0 Vertical center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize / 2, height0 - tPatchSize / 2 - 1]
		 * @param centerX1 Horizontal center position of the (tPatchSize x tPatchSize) block in the second frame, with range [tPatchSize / 2, width1 - tPatchSize / 2 - 1]
		 * @param centerY1 Vertical center position of the (tPatchSize x tPatchSize) block in the second frame, with range [tPatchSize / 2, height1 - tPatchSize / 2 - 1]
		 * @param image0PaddingElements The number of padding elements at the end of each row of the first image, in elements, with range [0, infinity)
		 * @param image1PaddingElements The number of padding elements at the end of each row of the second image, in elements, with range [0, infinity
		 * @return The resulting sum of square differences for tPatchSize * tPatchSize * tChannels elements
		 * @tparam tChannels Specifies the number of channels for the given frames, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static OCEAN_FORCE_INLINE uint32_t patch8BitPerChannelTemplate(const uint8_t* const image0, const uint8_t* const image1, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements);

		/**
		 * Returns the zero-mean sum of square differences between two square patches.
		 * @param patch0 The top-left corner of the first image patch, must be valid
		 * @param patch1 The top-left corner of the second image patch, must be valid
		 * @param patch0StrideElements The number of elements between two row starts in the first patch, in elements, with range [tPatchSize * tChannels, infinity)
		 * @param patch1StrideElements The number of elements between two row starts in the second patch, in elements, with range [tPatchSize * tChannels, infinity)
		 * @return The resulting sum of square differences for tPatchSize * tPatchSize * tChannels elements
		 * @tparam tChannels Specifies the number of channels for the given frames, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static uint32_t patch8BitPerChannelTemplate(const uint8_t* patch0, const uint8_t* patch1, const unsigned int patch0StrideElements, const unsigned int patch1StrideElements);

		/**
		 * Returns the zero-mean sum of square differences between a square image patch and a buffer.
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
		static OCEAN_FORCE_INLINE uint32_t patchBuffer8BitPerChannelTemplate(const uint8_t* image0, const unsigned int width0, const unsigned int centerX0, const unsigned int centerY0, const unsigned int image0PaddingElements, const uint8_t* buffer1);

		/**
		 * Returns the zero-mean sum of square differences between a square image patch and a buffer.
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
		 * Returns the zero-mean sum of square differences between two memory buffers.
		 * @param buffer0 The first memory buffer, must be valid
		 * @param buffer1 The second memory buffer, must be valid
		 * @return The resulting sum of square differences
		 * @tparam tChannels Specifies the number of channels for the given frames
		 * @tparam tPixels The number of pixels the buffer holds, in pixels, with range [1, infinity)
		 */
		template <unsigned int tChannels, unsigned int tPixels>
		static inline uint32_t buffer8BitPerChannelTemplate(const uint8_t* buffer0, const uint8_t* buffer1);

		/**
		 * Returns the zero-mean sum of square differences between two square patches.
		 * @param image0 The first image in which the first patch is located, must be valid
		 * @param image1 The second image in which the second patch is located, must be valid
		 * @param width0 Width of the first frame in pixels, with range [patchSize, infinity)
		 * @param width1 Width of the second frame in pixels, with range [patchSize, infinity)
		 * @param patchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 * @param centerX0 Horizontal center position of the (patchSize x patchSize) block in the first frame, with range [patchSize / 2, width0 - patchSize / 2 - 1]
		 * @param centerY0 Vertical center position of the (patchSize x patchSize) block in the first frame, with range [patchSize / 2, height0 - patchSize / 2 - 1]
		 * @param centerX1 Horizontal center position of the (patchSize x patchSize) block in the second frame, with range [patchSize / 2, width1 - patchSize / 2 - 1]
		 * @param centerY1 Vertical center position of the (patchSize x patchSize) block in the second frame, with range [patchSize / 2, height1 - patchSize / 2 - 1]
		 * @param image0PaddingElements The number of padding elements at the end of each row of the first image, in elements, with range [0, infinity)
		 * @param image1PaddingElements The number of padding elements at the end of each row of the second image, in elements, with range [0, infinity
		 * @return The resulting sum of square differences for `patchSize * patchSize * channels` elements
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static OCEAN_FORCE_INLINE uint32_t patch8BitPerChannel(const uint8_t* const image0, const uint8_t* const image1, const unsigned int patchSize, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements);

		/**
		 * Returns the zero-mean sum of square differences between two square patches.
		 * @param patch0 The top-left corner of the first image patch, must be valid
		 * @param patch1 The top-left corner of the second image patch, must be valid
		 * @param patchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 * @param patch0StrideElements The number of elements between two row starts in the first patch, in elements, with range [patchSize * channels, infinity)
		 * @param patch1StrideElements The number of elements between two row starts in the second patch, in elements, with range [patchSize * channels, infinity)
		 * @return The resulting sum of square differences for `patchSize * patchSize * channels` elements
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static uint32_t patch8BitPerChannel(const uint8_t* patch0, const uint8_t* patch1, const unsigned int patchSize, const unsigned int patch0StrideElements, const unsigned int patch1StrideElements);

		/**
		 * Returns the sum of square differences between a square image patch and a buffer.
		 * @param image0 The image in which the image patch is located, must be valid
		 * @param patchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 * @param width0 Width of the first frame in pixels, with range [tPatchSize, infinity)
		 * @param centerX0 Horizontal center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize / 2, width0 - tPatchSize / 2 - 1]
		 * @param centerY0 Vertical center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize / 2, height0 - tPatchSize / 2 - 1]
		 * @param image0PaddingElements The number of padding elements at the end of each row of the first image, in elements, with range [0, infinity)
		 * @param buffer1 The memory buffer with `tChannels * tPatchSize * tPatchSize` elements, must be valid
		 * @return The resulting sum of square differences for tPatchSize * tPatchSize * tChannels elements, with range [0, infinity)
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static OCEAN_FORCE_INLINE uint32_t patchBuffer8BitPerChannel(const uint8_t* image0, unsigned int patchSize, const unsigned int width0, const unsigned int centerX0, const unsigned int centerY0, const unsigned int image0PaddingElements, const uint8_t* buffer1);

		/**
		 * Returns the sum of square differences between a square image patch and a buffer.
		 * @param patch0 The top left start position of the image patch, must be valid
		 * @param buffer1 The memory buffer, must be valid
		 * @param patchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 * @param patch0StrideElements The number of elements between two rows for the image patch, in elements, with range [tChannels, tPatchSize, infinity)
		 * @return The resulting sum of square differences for tPatchSize * tPatchSize * tChannels elements, with range [0, infinity)
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static uint32_t patchBuffer8BitPerChannel(const uint8_t* patch0, const uint8_t* buffer1, unsigned int patchSize, const unsigned int patch0StrideElements);

		/**
		 * Returns the zero-mean sum of square differences between two memory buffers.
		 * @param buffer0 The first memory buffer, must be valid
		 * @param buffer1 The second memory buffer, must be valid
		 * @param pixels The number of pixels the buffer holds, in pixels, with range [1, infinity)
		 * @return The resulting sum of square differences
		 * @tparam tChannels Specifies the number of channels for the given frames
		 */
		template <unsigned int tChannels>
		static inline unsigned int buffer8BitPerChannel(const uint8_t* buffer0, const uint8_t* buffer1, const unsigned int pixels);

		/**
		 * Returns the zero-mean sum of square differences between two patches within an image, patch pixels outside the image will be mirrored back into the image.
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
		static uint32_t patchMirroredBorder8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const unsigned int patchSize, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements);
};

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t ZeroMeanSumSquareDifferencesBase::patch8BitPerChannelTemplate(const uint8_t* const image0, const uint8_t* const image1, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid number of frame channels!");
	static_assert(tPatchSize % 2u == 1u, "Invalid patch size!");

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
uint32_t ZeroMeanSumSquareDifferencesBase::patch8BitPerChannelTemplate(const uint8_t* patch0, const uint8_t* patch1, const unsigned int patch0StrideElements, const unsigned int patch1StrideElements)
{
	static_assert(tPatchSize % 2u == 1u, "Invalid image patch size, need an odd value!");
	static_assert(tChannels > 0u, "Invalid number of frame channels!");

	ocean_assert(patch0 != nullptr && patch1 != nullptr);

	ocean_assert(patch0StrideElements >= tPatchSize * tChannels);
	ocean_assert(patch1StrideElements >= tPatchSize * tChannels);

	uint32_t sumMean0[tChannels] = {0u};
	uint32_t sumMean1[tChannels] = {0u};

	for (unsigned int y = 0u; y < tPatchSize; ++y)
	{
		for (unsigned int x = 0u; x < tPatchSize; ++x)
		{
			for (unsigned int c = 0u; c < tChannels; c++)
			{
				sumMean0[c] += patch0[c];
				sumMean1[c] += patch1[c];
			}

			patch0 += tChannels;
			patch1 += tChannels;
		}

		patch0 += patch0StrideElements - tPatchSize * tChannels;
		patch1 += patch1StrideElements - tPatchSize * tChannels;
	}

	uint8_t mean0[tChannels];
	uint8_t mean1[tChannels];

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		mean0[n] = uint8_t((sumMean0[n] + (tPatchSize * tPatchSize / 2u)) / (tPatchSize * tPatchSize));
		mean1[n] = uint8_t((sumMean1[n] + (tPatchSize * tPatchSize / 2u)) / (tPatchSize * tPatchSize));
	}

	patch0 -= patch0StrideElements * tPatchSize;
	patch1 -= patch1StrideElements * tPatchSize;

	uint32_t result = 0;
	int16_t value = 0;

	for (unsigned int y = 0u; y < tPatchSize; ++y)
	{
		for (unsigned int x = 0u; x < tPatchSize; ++x)
		{
			for (unsigned int c = 0u; c < tChannels; c++)
			{
				value = int16_t(patch0[c] - mean0[c]) - int16_t(patch1[c] - mean1[c]);
				result += value * value;
			}

			patch0 += tChannels;
			patch1 += tChannels;
		}

		patch0 += patch0StrideElements - tPatchSize * tChannels;
		patch1 += patch1StrideElements - tPatchSize * tChannels;
	}

	return result;
}

template <unsigned int tChannels, unsigned int tPatchSize>
OCEAN_FORCE_INLINE uint32_t ZeroMeanSumSquareDifferencesBase::patchBuffer8BitPerChannelTemplate(const uint8_t* image0, const unsigned int width0, const unsigned int centerX0, const unsigned int centerY0, const unsigned int image0PaddingElements, const uint8_t* buffer1)
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
uint32_t ZeroMeanSumSquareDifferencesBase::patchBuffer8BitPerChannelTemplate(const uint8_t* patch0, const uint8_t* buffer1, const unsigned int patch0StrideElements)
{
	return patch8BitPerChannelTemplate<tChannels, tPatchSize>(patch0, buffer1, patch0StrideElements, tChannels * tPatchSize);
}

template <unsigned int tChannels, unsigned int tPixels>
inline uint32_t ZeroMeanSumSquareDifferencesBase::buffer8BitPerChannelTemplate(const uint8_t* buffer0, const uint8_t* buffer1)
{
	static_assert(tChannels != 0u, "Invalid number of frame channels!");
	static_assert(tPixels != 0u, "Invalid image buffer size!");

	ocean_assert(buffer0 != nullptr && buffer1 != nullptr);

	uint32_t sumMean0[tChannels] = {0u};
	uint32_t sumMean1[tChannels] = {0u};

	for (unsigned int n = 0u; n < tPixels; ++n)
	{
		for (unsigned int c = 0u; c < tChannels; ++c)
		{
			sumMean0[c] += buffer0[c];
			sumMean1[c] += buffer1[c];
		}

		buffer0 += tChannels;
		buffer1 += tChannels;
	}

	uint8_t mean0[tChannels];
	uint8_t mean1[tChannels];

	for (unsigned int c = 0u; c < tChannels; ++c)
	{
		mean0[c] = uint8_t((sumMean0[c] + tPixels / 2u) / tPixels);
		mean1[c] = uint8_t((sumMean1[c] + tPixels / 2u) / tPixels);
	}

	buffer0 -= tChannels * tPixels;
	buffer1 -= tChannels * tPixels;

	int16_t value;
	uint32_t ssd = 0u;

	for (unsigned int n = 0u; n < tPixels; ++n)
	{
		for (unsigned int c = 0u; c < tChannels; ++c)
		{
			value = int16_t(buffer0[c] - mean0[c]) - int16_t(buffer1[c] - mean1[c]);
			ssd += uint32_t(value * value);
		}

		buffer0 += tChannels;
		buffer1 += tChannels;
	}

	return ssd;
}

template <unsigned int tChannels>
OCEAN_FORCE_INLINE uint32_t ZeroMeanSumSquareDifferencesBase::patch8BitPerChannel(const uint8_t* const image0, const uint8_t* const image1, const unsigned int patchSize, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(image0 != nullptr && image1 != nullptr);
	ocean_assert(patchSize % 2u == 1u);

	const unsigned int patchSize_2 = patchSize / 2u;

	ocean_assert(centerX0 >= patchSize_2 && centerY0 >= patchSize_2 && centerX0 < width0 - patchSize_2);
	ocean_assert(centerX1 >= patchSize_2 && centerY1 >= patchSize_2 && centerX1 < width1 - patchSize_2);

	ocean_assert(width0 >= patchSize_2);
	ocean_assert(width1 >= patchSize_2);

	const unsigned int image0StrideElements = width0 * tChannels + image0PaddingElements;
	const unsigned int image1StrideElements = width1 * tChannels + image1PaddingElements;

	return patch8BitPerChannel<tChannels>(image0 + (centerY0 - patchSize_2) * image0StrideElements + (centerX0 - patchSize_2) * tChannels, image1 + (centerY1 - patchSize_2) * image1StrideElements + (centerX1 - patchSize_2) * tChannels, patchSize, image0StrideElements, image1StrideElements);
}

template <unsigned int tChannels>
uint32_t ZeroMeanSumSquareDifferencesBase::patch8BitPerChannel(const uint8_t* patch0, const uint8_t* patch1, const unsigned int patchSize, const unsigned int patch0StrideElements, const unsigned int patch1StrideElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(patch0 != nullptr && patch1 != nullptr);
	ocean_assert(patchSize % 2u == 1u);

	ocean_assert(patch0StrideElements >= patchSize * tChannels);
	ocean_assert(patch1StrideElements >= patchSize * tChannels);

	uint32_t sumMean0[tChannels] = {0u};
	uint32_t sumMean1[tChannels] = {0u};

	for (unsigned int y = 0u; y < patchSize; ++y)
	{
		for (unsigned int x = 0u; x < patchSize; ++x)
		{
			for (unsigned int c = 0u; c < tChannels; c++)
			{
				sumMean0[c] += patch0[c];
				sumMean1[c] += patch1[c];
			}

			patch0 += tChannels;
			patch1 += tChannels;
		}

		patch0 += patch0StrideElements - patchSize * tChannels;
		patch1 += patch1StrideElements - patchSize * tChannels;
	}

	uint8_t mean0[tChannels];
	uint8_t mean1[tChannels];

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		mean0[n] = uint8_t((sumMean0[n] + (patchSize * patchSize / 2u)) / (patchSize * patchSize));
		mean1[n] = uint8_t((sumMean1[n] + (patchSize * patchSize / 2u)) / (patchSize * patchSize));
	}

	patch0 -= patch0StrideElements * patchSize;
	patch1 -= patch1StrideElements * patchSize;

	uint32_t result = 0;
	int16_t value = 0;

	for (unsigned int y = 0u; y < patchSize; ++y)
	{
		for (unsigned int x = 0u; x < patchSize; ++x)
		{
			for (unsigned int c = 0u; c < tChannels; c++)
			{
				value = int16_t(patch0[c] - mean0[c]) - int16_t(patch1[c] - mean1[c]);
				result += value * value;
			}

			patch0 += tChannels;
			patch1 += tChannels;
		}

		patch0 += patch0StrideElements - patchSize * tChannels;
		patch1 += patch1StrideElements - patchSize * tChannels;
	}

	return result;
}

template <unsigned int tChannels>
inline unsigned int ZeroMeanSumSquareDifferencesBase::buffer8BitPerChannel(const uint8_t* buffer0, const uint8_t* buffer1, const unsigned int pixels)
{
	static_assert(tChannels != 0u, "Invalid number of frame channels!");

	ocean_assert(buffer0 != nullptr && buffer1 != nullptr);
	ocean_assert(pixels >= 1u);

	uint32_t sumMean0[tChannels] = {0u};
	uint32_t sumMean1[tChannels] = {0u};

	for (unsigned int n = 0u; n < pixels; ++n)
	{
		for (unsigned int c = 0u; c < tChannels; ++c)
		{
			sumMean0[c] += buffer0[c];
			sumMean1[c] += buffer1[c];
		}

		buffer0 += tChannels;
		buffer1 += tChannels;
	}

	uint8_t mean0[tChannels];
	uint8_t mean1[tChannels];

	for (unsigned int c = 0u; c < tChannels; ++c)
	{
		mean0[c] = uint8_t((sumMean0[c] + pixels / 2u) / pixels);
		mean1[c] = uint8_t((sumMean1[c] + pixels / 2u) / pixels);
	}

	buffer0 -= tChannels * pixels;
	buffer1 -= tChannels * pixels;

	int16_t value;
	uint32_t ssd = 0u;

	for (unsigned int n = 0u; n < pixels; ++n)
	{
		for (unsigned int c = 0u; c < tChannels; ++c)
		{
			value = int16_t(buffer0[c] - mean0[c]) - int16_t(buffer1[c] - mean1[c]);
			ssd += uint32_t(value * value);
		}

		buffer0 += tChannels;
		buffer1 += tChannels;
	}

	return ssd;
}

template <unsigned int tChannels>
OCEAN_FORCE_INLINE uint32_t ZeroMeanSumSquareDifferencesBase::patchBuffer8BitPerChannel(const uint8_t* image0, unsigned int patchSize, const unsigned int width0, const unsigned int centerX0, const unsigned int centerY0, const unsigned int image0PaddingElements, const uint8_t* buffer1)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(image0 != nullptr && buffer1 != nullptr);

	ocean_assert(patchSize % 2u == 1u);

	const unsigned int patchSize_2 = patchSize / 2u;

	ocean_assert(centerX0 >= patchSize_2 && centerY0 >= patchSize_2 && centerX0 < width0 - patchSize_2);

	ocean_assert(width0 >= patchSize);

	const unsigned int image0StrideElements = width0 * tChannels + image0PaddingElements;

	return patchBuffer8BitPerChannel<tChannels>(image0 + (centerY0 - patchSize_2) * image0StrideElements + (centerX0 - patchSize_2) * tChannels, buffer1, patchSize, image0StrideElements);
}

template <unsigned int tChannels>
inline uint32_t ZeroMeanSumSquareDifferencesBase::patchBuffer8BitPerChannel(const uint8_t* patch0, const uint8_t* buffer1, unsigned int patchSize, const unsigned int patch0StrideElements)
{
	return patch8BitPerChannel<tChannels>(patch0, buffer1, patchSize, patch0StrideElements, tChannels * patchSize);
}

template <unsigned int tChannels>
uint32_t ZeroMeanSumSquareDifferencesBase::patchMirroredBorder8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const unsigned int patchSize, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
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

	const uint8_t* i0 = image0 + top0 * int(image0StrideElements) + left0 * int(tChannels);
	const uint8_t* i1 = image1 + top1 * int(image1StrideElements) + left1 * int(tChannels);

	unsigned int mean0[tChannels] = {0u};
	unsigned int mean1[tChannels] = {0u};

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
					mean0[n] += c0[n];
					mean1[n] += c1[n];
				}

				c0 += tChannels;
				c1 += tChannels;
			}
			else if (x0 < width0)
			{
				// x0 lies inside the frame

				ocean_assert(x1 >= width1);

				const uint8_t* m1 = c1 + CVUtilities::mirrorOffset(x1, width1) * int(tChannels);

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					mean0[n] += c0[n];
					mean1[n] += m1[n];
				}

				c0 += tChannels;
				c1 += tChannels;
			}
			else if (x1 < width1)
			{
				// x1 lies inside the frame

				ocean_assert(x0 >= width0);

				const uint8_t* m0 = c0 + CVUtilities::mirrorOffset(x0, width0) * int(tChannels);

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					mean0[n] += m0[n];
					mean1[n] += c1[n];
				}

				c0 += tChannels;
				c1 += tChannels;
			}
			else
			{
				// neither x0 nor x1 lie inside the frame

				ocean_assert(x0 >= width0 && x1 >= width1);

				const uint8_t* m1 = c1 + CVUtilities::mirrorOffset(x1, width1) * int(tChannels);
				const uint8_t* m0 = c0 + CVUtilities::mirrorOffset(x0, width0) * int(tChannels);

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					mean0[n] += m0[n];
					mean1[n] += m1[n];
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

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		mean0[n] = (mean0[n] + (patchSize * patchSize / 2u)) / (patchSize * patchSize);
		mean1[n] = (mean1[n] + (patchSize * patchSize / 2u)) / (patchSize * patchSize);
	}

	y0 = centerY0 - patchSize_2;
	y1 = centerY1 - patchSize_2;

	i0 = image0 + top0 * int(image0StrideElements) + left0 * int(tChannels);
	i1 = image1 + top1 * int(image1StrideElements) + left1 * int(tChannels);

	int32_t value;
	uint32_t zmssd = 0u;

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
					value = (c0[n] - mean0[n]) - (c1[n] - mean1[n]);
					zmssd += value * value;
				}

				c0 += tChannels;
				c1 += tChannels;
			}
			else if (x0 < width0)
			{
				// x0 lies inside the frame

				ocean_assert(x1 >= width1);

				const uint8_t* m1 = c1 + CVUtilities::mirrorOffset(x1, width1) * int(tChannels);

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					value = (c0[n] - mean0[n]) - (m1[n] - mean1[n]);
					zmssd += value * value;
				}

				c0 += tChannels;
				c1 += tChannels;
			}
			else if (x1 < width1)
			{
				// x1 lies inside the frame

				ocean_assert(x0 >= width0);

				const uint8_t* m0 = c0 + CVUtilities::mirrorOffset(x0, width0) * int(tChannels);

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					value = (m0[n] - mean0[n]) - (c1[n] - mean1[n]);
					zmssd += value * value;
				}

				c0 += tChannels;
				c1 += tChannels;
			}
			else
			{
				// neither x0 nor x1 lie inside the frame

				ocean_assert(x0 >= width0 && x1 >= width1);

				const uint8_t* m1 = c1 + CVUtilities::mirrorOffset(x1, width1) * int(tChannels);
				const uint8_t* m0 = c0 + CVUtilities::mirrorOffset(x0, width0) * int(tChannels);

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					value = (m0[n] - mean0[n]) - (m1[n] - mean1[n]);
					zmssd += value * value;
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

	return zmssd;
}

}

}

#endif // META_OCEAN_CV_SUM_ZERO_MEAN_SQUARE_DIFFERENCES_BASE_H
