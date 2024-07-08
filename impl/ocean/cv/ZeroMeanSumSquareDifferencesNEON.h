/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ZERO_MEAN_SUM_SQUARE_DIFFERENCES_NEON_H
#define META_OCEAN_CV_ZERO_MEAN_SUM_SQUARE_DIFFERENCES_NEON_H

#include "ocean/cv/CV.h"

#include "ocean/base/Utilities.h"

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

#include "ocean/cv/NEON.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements function to calculate zeao-mean sum square differences using NEON instructions.
 * @ingroup cv
 */
class ZeroMeanSumSquareDifferencesNEON
{
	protected:

		/**
		 * This class allows to specialize functions for individual channels.
		 * @tparam tChannels Specifies the number of channels for the given frames, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		class SpecializedForChannels
		{
			public:

				/**
				 * Determines the mean value for a buffer, one value for each channel.
				 * @param buffer The memory buffer to be handled, must be valid
				 * @param meanValues The resulting mean values, one for each channel
				 * @tparam tChannels Specifies the number of channels for the given frames, with range [1, infinity)
				 * @tparam tPixels The number of pixels in the buffer, in pixels, with range [8, infinity)
				 */
				template <unsigned int tPixels>
				static inline void mean8BitPerChannel(const uint8_t* const buffer, uint8_t* const meanValues);

				/**
				 * Determines the mean value for an image patch, one value for each channel.
				 * @param patch The top left start position of the image patch, must be valid
				 * @param patchStrideElements The number of elements between two rows for the image patch, in elements, with range [tChannels, tPatchSize, infinity)
				 * @param meanValues The resulting mean values, one for each channel
				 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity)
				 */
				template <unsigned int tPatchSize>
				static inline void mean8BitPerChannel(const uint8_t* patch, const unsigned int patchStrideElements, uint8_t* const meanValues);

				/**
				 * Determines the mean value for an image patch, one value for each channel, patch pixels outside the image will be mirrored back into the image.
				 * @param image The image in which the patch is located, must be valid
				 * @param width The width of the image, in pixels, with range [tPatchSize, infinity)
				 * @param height The height of the image, in pixels, with range [tPatchSize, infinity)
				 * @param centerX Horizontal center position of the (tPatchSize x tPatchSize) block in the frame, with range [tPatchSize/2, width - tPatchSize/2 - 1]
				 * @param centerY Vertical center position of the (tPatchSize x tPatchSize) block in the frame, with range [tPatchSize/2, height - tPatchSize/2 - 1]
				 * @param imagePaddingElements The number of padding elements at the end of each row of the image, in elements, with range [0, infinity)
				 * @param meanValues The resulting mean values, one for each channel
				 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
				 */
				template <unsigned int tPatchSize>
				static inline void mean8BitPerChannelMirroredBorder(const uint8_t* const image, const unsigned int width, const unsigned int height, const unsigned int centerX, const unsigned int centerY, const unsigned int imagePaddingElements, uint8_t* const meanValues);

				/**
				 * Returns the zero-mean sum of square differences between two memory buffers.
				 * @param buffer0 The first memory buffer, must be valid
				 * @param buffer1 The second memory buffer, must be valid
				 * @param meanValues0 The mean values of the first patch, one for each channel, must be valid
				 * @param meanValues1 The mean values of the second patch, one for each channel, must be valid
				 * @return The resulting sum of square differences
				 * @tparam tPixels The number of pixels in the buffer, in pixels, with range [8, infinity)
				 */
				template <unsigned int tPixels>
				static inline uint32_t buffer8BitPerChannel(const uint8_t* const buffer0, const uint8_t* const buffer1, const uint8_t* const meanValues0, const uint8_t* const meanValues1);

				/**
				 * Returns the zero-mean sum of square differences between two patches within an image.
				 * @param patch0 The top left start position of the first image patch, must be valid
				 * @param patch1 The top left start position of the second image patch, must be valid
				 * @param patch0StrideElements The number of elements between two rows for the first patch, in elements, with range [tChannels, tPatchSize, infinity)
				 * @param patch1StrideElements The number of elements between two rows for the second patch, in elements, with range [tChannels, tPatchSize, infinity)
				 * @param meanValues0 The mean values of the first patch, one for each channel, must be valid
				 * @param meanValues1 The mean values of the second patch, one for each channel, must be valid
				 * @return The resulting sum of square differences
				 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
				 */
				template <unsigned int tPatchSize>
				static inline uint32_t patch8BitPerChannel(const uint8_t* patch0, const uint8_t* patch1, const unsigned int patch0StrideElements, const unsigned int patch1StrideElements, const uint8_t* const meanValues0, const uint8_t* const meanValues1);

				/**
				 * Returns the zero-mean sum of square differences between two patches within an image, patch pixels outside the image will be mirrored back into the image.
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
				 * @param meanValues0 The mean values of the first patch, one for each channel, must be valid
				 * @param meanValues1 The mean values of the second patch, one for each channel, must be valid
				 * @return The resulting zero-mean sum of square differences, with range [0, infinity)
				 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
				 */
				template <unsigned int tPatchSize>
				static inline uint32_t patchMirroredBorder8BitPerChannel(const uint8_t* const image0, const uint8_t* const image1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements, const uint8_t* const meanValues0, const uint8_t* const meanValues1);
		};

	public:

		/**
		 * Returns the zero-mean sum of square differences between two memory buffers.
		 * @param buffer0 The first memory buffer, must be valid
		 * @param buffer1 The second memory buffer, must be valid
		 * @return The resulting sum of square differences
		 * @tparam tChannels Specifies the number of channels for the given buffers, with range [1, infinity)
		 * @tparam tPixels The number of pixels in the buffer, in pixels, with range [8, infinity)
		 */
		template <unsigned int tChannels, unsigned int tPixels>
		static inline uint32_t buffer8BitPerChannel(const uint8_t* const buffer0, const uint8_t* const buffer1);

		/**
		 * Returns the zero-mean sum of square differences between two patches within an image.
		 * @param patch0 The top left start position of the first image patch, must be valid
		 * @param patch1 The top left start position of the second image patch, must be valid
		 * @param patch0StrideElements The number of elements between two rows for the first patch, in elements, with range [tChannels, tPatchSize, infinity)
		 * @param patch1StrideElements The number of elements between two rows for the second patch, in elements, with range [tChannels, tPatchSize, infinity)
		 * @return The resulting sum of square differences
		 * @tparam tChannels Specifies the number of channels for the given frames, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static inline uint32_t patch8BitPerChannel(const uint8_t* const patch0, const uint8_t* const patch1, const unsigned int patch0StrideElements, const unsigned int patch1StrideElements);

		/**
		 * Returns the zero-mean sum of square differences between an image patch and a buffer.
		 * @param patch0 The top left start position of the image patch, must be valid
		 * @param buffer1 The memory buffer, must be valid
		 * @param patch0StrideElements The number of elements between two rows for the image patch, in elements, with range [tChannels, tPatchSize, infinity)
		 * @return The resulting sum of square differences
		 * @tparam tChannels Specifies the number of channels for the given frames, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static inline uint32_t patchBuffer8BitPerChannel(const uint8_t* patch0, const uint8_t* buffer1, const unsigned int patch0StrideElements);

		/**
		 * Returns the zero-mean sum of square differences between two patches within an image, patch pixels outside the image will be mirrored back into the image.
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
		 * @return The resulting zero-mean sum of square differences, with range [0, infinity)
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static uint32_t patchMirroredBorder8BitPerChannel(const uint8_t* const image0, const uint8_t* const image1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements);

		/**
		 * Determines the mean value for a buffer, one value for each channel.
		 * @param buffer The memory buffer to be handled, must be valid
		 * @param meanValues The resulting mean values, one for each channel
		 * @tparam tChannels Specifies the number of channels for the given frames, with range [1, infinity)
		 * @tparam tPixels The number of pixels in the buffer, in pixels, with range [8, infinity)
		 */
		template <unsigned int tChannels, unsigned int tPixels>
		static OCEAN_FORCE_INLINE void mean8BitPerChannel(const uint8_t* const buffer, uint8_t* const meanValues);

		/**
		 * Determines the mean value for an image patch, one value for each channel.
		 * @param patch The top left start position of the image patch, must be valid
		 * @param patchStrideElements The number of elements between two rows for the image patch, in elements, with range [tChannels, tPatchSize, infinity)
		 * @param meanValues The resulting mean values, one for each channel
		 * @tparam tChannels Specifies the number of channels for the given frames, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static OCEAN_FORCE_INLINE void mean8BitPerChannel(const uint8_t* patch, const unsigned int patchStrideElements, uint8_t* const meanValues);

	protected:

		/**
		 * Loads up to 8 uint8_t values from a 1-channel row with mirroring pixels if necessary.
		 * @param row The row from which the values will be loaded, must be valid
		 * @param x The index of the first pixel to load, with range [-elements/2, elements + elements/2]
		 * @param width The width of the row, in pixels, with range [4, infinity)
		 * @param intermediateBuffer An intermediate buffer with 8 elements, must be valid
		 * @return The uint8x8_t object with the loaded values
		 * @tparam tFront True, if the uint8_t values will be placed at the front of the resulting uint8x8_t object; False, to placed the uint8_t values at the end
		 * @tparam tPixels The number of uint8_t pixels to be read, with range [1, 8]
		 * @tparam tOverlappingToZero True, to set overlapping pixels to zero; False, to get overlapping pixels with random values
		 */
		template <bool tFront, unsigned int tPixels, bool tOverlappingToZero>
		static OCEAN_FORCE_INLINE uint8x8_t loadMirrored_u_8x8(const uint8_t* const row, const int x, const unsigned int width, uint8_t* const intermediateBuffer);

		/**
		 * Loads up to 16 uint8_t values from a 1-channel row with mirroring pixels if necessary.
		 * @param row The row from which the values will be loaded, must be valid
		 * @param x The index of the first pixel to load, with range [-elements/2, elements + elements/2]
		 * @param width The width of the row in pixels, with range [8, infinity)
		 * @param intermediateBuffer An intermediate buffer with 16 elements, must be valid
		 * @return The uint8x16_t object with the loaded values
		 * @tparam tFront True, if the uint8_t values will be placed at the front of the resulting uint8x8_t object; False, to placed the uint8_t values at the end
		 * @tparam tSize The number of uint8_t pixels to be read, with range [1, 16]
		 * @tparam tOverlappingToZero True, to set overlapping pixels to zero; False, to get overlapping pixels with random values
		 */
		template <bool tFront, unsigned int tPixels, bool tOverlappingToZero>
		static OCEAN_FORCE_INLINE uint8x16_t loadMirrored_u_8x16(const uint8_t* const row, const int x, const unsigned int width, uint8_t* const intermediateBuffer);
};

template <>
template <unsigned int tPixels>
inline void ZeroMeanSumSquareDifferencesNEON::SpecializedForChannels<1u>::mean8BitPerChannel(const uint8_t* buffer, uint8_t* const meanValues)
{
	static_assert(tPixels >= 8u, "Invalid pixels!");

	ocean_assert(buffer != nullptr && meanValues != nullptr);

	constexpr unsigned int blocks16 = tPixels / 16u;
	constexpr unsigned int remainingAfterBlocks16 = tPixels % 16u;

	constexpr bool partialBlock16 = remainingAfterBlocks16 > 8u && tPixels >= 16u;
	constexpr unsigned int remainingAfterPartialBlock16 = partialBlock16 ? 0u : remainingAfterBlocks16;

	constexpr unsigned int blocks8 = remainingAfterPartialBlock16 / 8u;
	constexpr unsigned int remainingAfterBlocks8 = remainingAfterPartialBlock16 % 8u;

	constexpr bool partialBlock8 = remainingAfterBlocks8 >= 3u && tPixels >= 8u;
	constexpr unsigned int remainingAfterPartialBlock8 = partialBlock8 ? 0u : remainingAfterBlocks8;

	constexpr unsigned int blocks1 = remainingAfterPartialBlock8;

	static_assert(blocks1 <= 2u, "Invalid block size!");

	uint32x4_t sum_u_32x4 = vdupq_n_u32(0u);

	uint32_t sumIndividual = 0u;

	for (unsigned int n = 0u; n < blocks16; ++n)
	{
		const uint8x16_t buffer_u_8x16 = vld1q_u8(buffer);

		sum_u_32x4 = vpadalq_u16(sum_u_32x4, vpaddlq_u8(buffer_u_8x16));

		buffer += 16;
	}

	if constexpr (partialBlock16)
	{
		static_assert(tPixels >= 16u, "We need to guarantee that loading 16 pixels of worth of data preceding the end boundary cannot cause memory access violation");

		constexpr unsigned int overlappingElements = 16u - remainingAfterBlocks16;
		ocean_assert(overlappingElements < 8u);

		// mask: |<- overlapping ->|<- remainingAfterBlocks16 ->|
		//        00 00 00 00 00 00 FF FF FF FF FF FF FF FF FF FF
		const uint8x16_t mask_u_8x16 = vcombine_u8(vcreate_u8(uint64_t(-1) << overlappingElements * 8u), vcreate_u8(uint64_t(-1)));

		const uint8x16_t buffer_u_8x16 = vandq_u8(vld1q_u8(buffer - overlappingElements), mask_u_8x16);

		sum_u_32x4 = vpadalq_u16(sum_u_32x4, vpaddlq_u8(buffer_u_8x16));

		buffer += remainingAfterBlocks16;
	}

	for (unsigned int n = 0u; n < blocks8; ++n)
	{
		const uint8x8_t buffer_u_8x8 = vld1_u8(buffer);

		sum_u_32x4 = vaddw_u16(sum_u_32x4, vpaddl_u8(buffer_u_8x8));

		buffer += 8;
	}

	if constexpr (partialBlock8)
	{
		constexpr unsigned int overlappingElements = 8u - remainingAfterBlocks8;
		ocean_assert(overlappingElements < 8u);

		const uint8x8_t mask_u_8x8 = vcreate_u8(uint64_t(-1) << overlappingElements * 8u);

		const uint8x8_t buffer_u_8x8 = vand_u8(vld1_u8(buffer - overlappingElements), mask_u_8x8);

		sum_u_32x4 = vaddw_u16(sum_u_32x4, vpaddl_u8(buffer_u_8x8));

		buffer += remainingAfterBlocks8;
	}

	if constexpr (blocks1 != 0u)
	{
		for (unsigned int n = 0u; n < blocks1; ++n)
		{
			sumIndividual += buffer[n];
		}

		buffer += blocks1;
	}

	uint32_t results[4];
	vst1q_u32(results, sum_u_32x4);

	const uint32_t sum = results[0] + results[1] + results[2] + results[3] + sumIndividual;

	meanValues[0] = uint8_t((sum + tPixels / 2u) / tPixels);
}

template <>
template <unsigned int tPixels>
inline void ZeroMeanSumSquareDifferencesNEON::SpecializedForChannels<3u>::mean8BitPerChannel(const uint8_t* buffer, uint8_t* const meanValues)
{
	static_assert(tPixels >= 8u, "Invalid pixels!");

	constexpr unsigned int tChannels = 3u;

	ocean_assert(buffer != nullptr && meanValues != nullptr);

	constexpr unsigned int blocks16 = tPixels / 16u;
	constexpr unsigned int remainingAfterBlocks16 = tPixels % 16u;

	constexpr bool partialBlock16 = remainingAfterBlocks16 > 10u && blocks16 >= 1u;
	constexpr unsigned int remainingAfterPartialBlock16 = partialBlock16 ? 0u : remainingAfterBlocks16;

	constexpr unsigned int blocks8 = remainingAfterPartialBlock16 / 8u;
	constexpr unsigned int remainingAfterBlocks8 = remainingAfterPartialBlock16 % 8u;

	constexpr bool partialBlock8 = remainingAfterBlocks8 >= 3u;
	constexpr unsigned int remainingAfterPartialBlock8 = partialBlock8 ? 0u : remainingAfterBlocks8;

	constexpr unsigned int blocks1 = remainingAfterPartialBlock8;

	static_assert(blocks1 <= 2u, "Invalid block size!");

	uint32x4_t sumChannel0_u_32x4 = vdupq_n_u32(0u);
	uint32x4_t sumChannel1_u_32x4 = vdupq_n_u32(0u);
	uint32x4_t sumChannel2_u_32x4 = vdupq_n_u32(0u);

	uint32_t sumIndividual[3] = {0u};

	for (unsigned int n = 0u; n < blocks16; ++n)
	{
		const uint8x16x3_t buffer_u_8x16x3 = vld3q_u8(buffer);

		sumChannel0_u_32x4 = vpadalq_u16(sumChannel0_u_32x4, vpaddlq_u8(buffer_u_8x16x3.val[0]));
		sumChannel1_u_32x4 = vpadalq_u16(sumChannel1_u_32x4, vpaddlq_u8(buffer_u_8x16x3.val[1]));
		sumChannel2_u_32x4 = vpadalq_u16(sumChannel2_u_32x4, vpaddlq_u8(buffer_u_8x16x3.val[2]));

		buffer += 16u * tChannels;
	}

	if constexpr (partialBlock16)
	{
		static_assert(tPixels >= 16u, "We need to guarantee that loading 16 pixels of worth of data preceding the end boundary cannot cause memory access violation");

		constexpr unsigned int overlappingElements = 16u - remainingAfterBlocks16;
		ocean_assert(overlappingElements < 8u);

		// mask: |<- overlapping ->|<- remainingAfterBlocks16 ->|
		//        00 00 00 00 00 00 FF FF FF FF FF FF FF FF FF FF
		const uint8x16_t mask_u_8x16 = vcombine_u8(vcreate_u8(uint64_t(-1) << overlappingElements * 8u), vcreate_u8(uint64_t(-1)));

		const uint8x16x3_t buffer_u_8x16x3 = vld3q_u8(buffer - overlappingElements * tChannels);

		sumChannel0_u_32x4 = vpadalq_u16(sumChannel0_u_32x4, vpaddlq_u8(vandq_u8(buffer_u_8x16x3.val[0], mask_u_8x16)));
		sumChannel1_u_32x4 = vpadalq_u16(sumChannel1_u_32x4, vpaddlq_u8(vandq_u8(buffer_u_8x16x3.val[1], mask_u_8x16)));
		sumChannel2_u_32x4 = vpadalq_u16(sumChannel2_u_32x4, vpaddlq_u8(vandq_u8(buffer_u_8x16x3.val[2], mask_u_8x16)));

		buffer += remainingAfterBlocks16 * tChannels;
	}

	for (unsigned int n = 0u; n < blocks8; ++n)
	{
		const uint8x8x3_t buffer_u_8x8x3 = vld3_u8(buffer);

		sumChannel0_u_32x4 = vaddw_u16(sumChannel0_u_32x4, vpaddl_u8(buffer_u_8x8x3.val[0]));
		sumChannel1_u_32x4 = vaddw_u16(sumChannel1_u_32x4, vpaddl_u8(buffer_u_8x8x3.val[1]));
		sumChannel2_u_32x4 = vaddw_u16(sumChannel2_u_32x4, vpaddl_u8(buffer_u_8x8x3.val[2]));

		buffer += 8u * tChannels;
	}

	if constexpr (partialBlock8)
	{
		constexpr unsigned int overlappingElements = 8u - remainingAfterBlocks8;
		ocean_assert(overlappingElements < 8u);

		const uint8x8_t mask_u_8x8 = vcreate_u8(uint64_t(-1) << overlappingElements * 8u);

		const uint8x8x3_t buffer_u_8x8x3 = vld3_u8(buffer - overlappingElements * tChannels);

		sumChannel0_u_32x4 = vaddw_u16(sumChannel0_u_32x4, vpaddl_u8(vand_u8(buffer_u_8x8x3.val[0], mask_u_8x8)));
		sumChannel1_u_32x4 = vaddw_u16(sumChannel1_u_32x4, vpaddl_u8(vand_u8(buffer_u_8x8x3.val[1], mask_u_8x8)));
		sumChannel2_u_32x4 = vaddw_u16(sumChannel2_u_32x4, vpaddl_u8(vand_u8(buffer_u_8x8x3.val[2], mask_u_8x8)));

		buffer += remainingAfterBlocks8 * tChannels;
	}

	for (unsigned int n = 0u; n < blocks1; ++n)
	{
		sumIndividual[0] += buffer[tChannels * n + 0u];
		sumIndividual[1] += buffer[tChannels * n + 1u];
		sumIndividual[2] += buffer[tChannels * n + 2u];
	}

	uint32_t results[4];
	vst1q_u32(results, sumChannel0_u_32x4);

	const uint32_t sum0 = results[0] + results[1] + results[2] + results[3] + sumIndividual[0];
	meanValues[0] = uint8_t((sum0 + tPixels / 2u) / tPixels);

	vst1q_u32(results, sumChannel1_u_32x4);

	const uint32_t sum1 = results[0] + results[1] + results[2] + results[3] + sumIndividual[1];
	meanValues[1] = uint8_t((sum1 + tPixels / 2u) / tPixels);

	vst1q_u32(results, sumChannel2_u_32x4);

	const uint32_t sum2 = results[0] + results[1] + results[2] + results[3] + sumIndividual[2];
	meanValues[2] = uint8_t((sum2 + tPixels / 2u) / tPixels);
}

template <unsigned int tChannels>
template <unsigned int tPixels>
inline void ZeroMeanSumSquareDifferencesNEON::SpecializedForChannels<tChannels>::mean8BitPerChannel(const uint8_t* const buffer, uint8_t* const meanValues)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPixels >= 1u, "Invalid buffer size!");

	ocean_assert(buffer != nullptr && meanValues != nullptr);

	uint32_t sum[tChannels] = {0u};

	for (unsigned int n = 0u; n < tPixels; ++n)
	{
		for (unsigned int c = 0u; c < tChannels; ++c)
		{
			sum[c] += buffer[n * tChannels + c];
		}
	}

	for (unsigned int c = 0u; c < tChannels; ++c)
	{
		meanValues[c] = uint8_t((sum[c] + tPixels / 2u) / tPixels);
	}
}

template <>
template <unsigned int tPatchSize>
inline void ZeroMeanSumSquareDifferencesNEON::SpecializedForChannels<1u>::mean8BitPerChannel(const uint8_t* patch, const unsigned int patchStrideElements, uint8_t* const meanValues)
{
	static_assert(tPatchSize >= 5u, "Invalid patch size!");

	ocean_assert(patch != nullptr && meanValues != nullptr);

	ocean_assert(patchStrideElements >= tPatchSize);

	constexpr unsigned int blocks16 = tPatchSize / 16u;
	constexpr unsigned int remainingAfterBlocks16 = tPatchSize % 16u;

	constexpr bool partialBlock16 = remainingAfterBlocks16 > 8u;
	constexpr unsigned int remainingAfterPartialBlock16 = partialBlock16 ? 0u : remainingAfterBlocks16;

	constexpr unsigned int blocks8 = remainingAfterPartialBlock16 / 8u;
	constexpr unsigned int remainingAfterBlocks8 = remainingAfterPartialBlock16 % 8u;

	constexpr bool partialBlock8 = remainingAfterBlocks8 >= 3u;
	constexpr unsigned int remainingAfterPartialBlock8 = partialBlock8 ? 0u : remainingAfterBlocks8;

	constexpr unsigned int blocks1 = remainingAfterPartialBlock8;

	static_assert(blocks1 <= 2u, "Invalid block size!");

	uint32x4_t sum_u_32x4 = vdupq_n_u32(0u);

	uint32_t sumIndividual = 0u;

	for (unsigned int y = 0u; y < tPatchSize; ++y)
	{
		for (unsigned int n = 0u; n < blocks16; ++n)
		{
			const uint8x16_t patch_u_8x16 = vld1q_u8(patch);

			sum_u_32x4 = vpadalq_u16(sum_u_32x4, vpaddlq_u8(patch_u_8x16));

			patch += 16;
		}

		if constexpr (partialBlock16)
		{
			constexpr unsigned int overlappingElements = 16u - remainingAfterBlocks16;
			ocean_assert(overlappingElements < 8u);

			if (y < tPatchSize - 1u)
			{
				// mask: |<- overlapping ->|<- remainingAfterBlocks16 ->|
				//        00 00 00 00 00 00 FF FF FF FF FF FF FF FF FF FF
				constexpr uint64_t maskHigh = overlappingElements >= 8u ? uint64_t(0) : (uint64_t(-1) >> overlappingElements * 8u);
				const uint8x16_t mask_u_8x16 = vcombine_u16(vcreate_u8(uint64_t(-1)), vcreate_u8(maskHigh));

				const uint8x16_t patch_u_8x16 = vandq_u8(vld1q_u8(patch), mask_u_8x16);

				sum_u_32x4 = vpadalq_u16(sum_u_32x4, vpaddlq_u8(patch_u_8x16));
			}
			else
			{
				// mask: |<- overlapping ->|<- remainingAfterBlocks16 ->|
				//        00 00 00 00 00 00 FF FF FF FF FF FF FF FF FF FF
				constexpr uint64_t maskLow = overlappingElements >= 8u ? uint64_t(0) : (uint64_t(-1) << overlappingElements * 8u);
				const uint8x16_t mask_u_8x16 = vcombine_u16(vcreate_u8(maskLow), vcreate_u8(uint64_t(-1)));

				const uint8x16_t patch_u_8x16 = vandq_u8(vld1q_u8(patch - overlappingElements), mask_u_8x16);

				sum_u_32x4 = vpadalq_u16(sum_u_32x4, vpaddlq_u8(patch_u_8x16));
			}

			patch += remainingAfterBlocks16;
		}

		for (unsigned int n = 0u; n < blocks8; ++n)
		{
			const uint8x8_t patch_u_8x8 = vld1_u8(patch);

			sum_u_32x4 = vaddw_u16(sum_u_32x4, vpaddl_u8(patch_u_8x8));

			patch += 8;
		}

		if constexpr (partialBlock8)
		{
			constexpr unsigned int overlappingElements = 8u - remainingAfterBlocks8;
			ocean_assert(overlappingElements < 8u);

			if (y < tPatchSize - 1u)
			{
				constexpr uint64_t mask = overlappingElements >= 8u ? uint64_t(0) : (uint64_t(-1) >> overlappingElements * 8u);
				const uint8x8_t mask_u_8x8 = vcreate_u8(mask);

				const uint8x8_t patch_u_8x8 = vand_u8(vld1_u8(patch), mask_u_8x8);

				sum_u_32x4 = vaddw_u16(sum_u_32x4, vpaddl_u8(patch_u_8x8));
			}
			else
			{
				constexpr uint64_t mask = overlappingElements >= 8u ? uint64_t(0) : (uint64_t(-1) << overlappingElements * 8u);
				const uint8x8_t mask_u_8x8 = vcreate_u8(mask);

				const uint8x8_t patch_u_8x8 = vand_u8(vld1_u8(patch - overlappingElements), mask_u_8x8);

				sum_u_32x4 = vaddw_u16(sum_u_32x4, vpaddl_u8(patch_u_8x8));
			}

			patch += remainingAfterBlocks8;
		}

		if constexpr (blocks1 != 0u)
		{
			for (unsigned int n = 0u; n < blocks1; ++n)
			{
				sumIndividual += patch[n];
			}

			patch += blocks1;
		}

		patch += patchStrideElements - tPatchSize;
	}

	uint32_t results[4];
	vst1q_u32(results, sum_u_32x4);

	const uint32_t sum = results[0] + results[1] + results[2] + results[3] + sumIndividual;

	meanValues[0] = uint8_t((sum + tPatchSize * tPatchSize / 2u) / (tPatchSize * tPatchSize));
}

template <>
template <unsigned int tPatchSize>
inline void ZeroMeanSumSquareDifferencesNEON::SpecializedForChannels<3u>::mean8BitPerChannel(const uint8_t* patch, const unsigned int patchStrideElements, uint8_t* const meanValues)
{
	static_assert(tPatchSize >= 5u, "Invalid patch size!");

	constexpr unsigned int tChannels = 3u;

	ocean_assert(patch != nullptr && meanValues != nullptr);

	ocean_assert(patchStrideElements >= tChannels * tPatchSize);

	constexpr unsigned int blocks16 = tPatchSize / 16u;
	constexpr unsigned int remainingAfterBlocks16 = tPatchSize % 16u;

	constexpr bool partialBlock16 = remainingAfterBlocks16 > 10u;
	constexpr unsigned int remainingAfterPartialBlock16 = partialBlock16 ? 0u : remainingAfterBlocks16;

	constexpr unsigned int blocks8 = remainingAfterPartialBlock16 / 8u;
	constexpr unsigned int remainingAfterBlocks8 = remainingAfterPartialBlock16 % 8u;

	constexpr bool partialBlock8 = remainingAfterBlocks8 >= 3u;
	constexpr unsigned int remainingAfterPartialBlock8 = partialBlock8 ? 0u : remainingAfterBlocks8;

	constexpr unsigned int blocks1 = remainingAfterPartialBlock8;

	static_assert(blocks1 <= 2u, "Invalid block size!");

	uint32x4_t sumChannel0_u_32x4 = vdupq_n_u32(0u);
	uint32x4_t sumChannel1_u_32x4 = vdupq_n_u32(0u);
	uint32x4_t sumChannel2_u_32x4 = vdupq_n_u32(0u);

	uint32_t sumIndividual[3] = {0u};

	for (unsigned int y = 0u; y < tPatchSize; ++y)
	{
		for (unsigned int n = 0u; n < blocks16; ++n)
		{
			const uint8x16x3_t patch_u_8x16x3 = vld3q_u8(patch);

			sumChannel0_u_32x4 = vpadalq_u16(sumChannel0_u_32x4, vpaddlq_u8(patch_u_8x16x3.val[0]));
			sumChannel1_u_32x4 = vpadalq_u16(sumChannel1_u_32x4, vpaddlq_u8(patch_u_8x16x3.val[1]));
			sumChannel2_u_32x4 = vpadalq_u16(sumChannel2_u_32x4, vpaddlq_u8(patch_u_8x16x3.val[2]));

			patch += 16u * tChannels;
		}

		if constexpr (partialBlock16)
		{
			constexpr unsigned int overlappingElements = 16u - remainingAfterBlocks16;
			ocean_assert(overlappingElements < 8u);

			if (y < tPatchSize - 1u)
			{
				// mask: |<- overlapping ->|<- remainingAfterBlocks16 ->|
				//        00 00 00 00 00 00 FF FF FF FF FF FF FF FF FF FF
				constexpr uint64_t maskHigh = overlappingElements >= 8u ? uint64_t(0) : (uint64_t(-1) >> overlappingElements * 8u);
				const uint8x16_t mask_u_8x16 = vcombine_u16(vcreate_u8(uint64_t(-1)), vcreate_u8(maskHigh));

				const uint8x16x3_t patch_u_8x16x3 = vld3q_u8(patch);

				sumChannel0_u_32x4 = vpadalq_u16(sumChannel0_u_32x4, vpaddlq_u8(vandq_u8(patch_u_8x16x3.val[0], mask_u_8x16)));
				sumChannel1_u_32x4 = vpadalq_u16(sumChannel1_u_32x4, vpaddlq_u8(vandq_u8(patch_u_8x16x3.val[1], mask_u_8x16)));
				sumChannel2_u_32x4 = vpadalq_u16(sumChannel2_u_32x4, vpaddlq_u8(vandq_u8(patch_u_8x16x3.val[2], mask_u_8x16)));
			}
			else
			{
				// mask: |<- overlapping ->|<- remainingAfterBlocks16 ->|
				//        00 00 00 00 00 00 FF FF FF FF FF FF FF FF FF FF
				constexpr uint64_t maskLow = overlappingElements >= 8u ? uint64_t(0) : (uint64_t(-1) << overlappingElements * 8u);
				const uint8x16_t mask_u_8x16 = vcombine_u16(vcreate_u8(maskLow), vcreate_u8(uint64_t(-1)));

				const uint8x16x3_t patch_u_8x16x3 = vld3q_u8(patch - overlappingElements * tChannels);

				sumChannel0_u_32x4 = vpadalq_u16(sumChannel0_u_32x4, vpaddlq_u8(vandq_u8(patch_u_8x16x3.val[0], mask_u_8x16)));
				sumChannel1_u_32x4 = vpadalq_u16(sumChannel1_u_32x4, vpaddlq_u8(vandq_u8(patch_u_8x16x3.val[1], mask_u_8x16)));
				sumChannel2_u_32x4 = vpadalq_u16(sumChannel2_u_32x4, vpaddlq_u8(vandq_u8(patch_u_8x16x3.val[2], mask_u_8x16)));
			}

			patch += remainingAfterBlocks16 * tChannels;
		}

		for (unsigned int n = 0u; n < blocks8; ++n)
		{
			const uint8x8x3_t patch_u_8x8x3 = vld3_u8(patch);

			sumChannel0_u_32x4 = vaddw_u16(sumChannel0_u_32x4, vpaddl_u8(patch_u_8x8x3.val[0]));
			sumChannel1_u_32x4 = vaddw_u16(sumChannel1_u_32x4, vpaddl_u8(patch_u_8x8x3.val[1]));
			sumChannel2_u_32x4 = vaddw_u16(sumChannel2_u_32x4, vpaddl_u8(patch_u_8x8x3.val[2]));

			patch += 8u * tChannels;
		}

		if constexpr (partialBlock8)
		{
			constexpr unsigned int overlappingElements = 8u - remainingAfterBlocks8;
			ocean_assert(overlappingElements < 8u);

			if (y < tPatchSize - 1u)
			{
				constexpr uint64_t mask = overlappingElements >= 8u ? uint64_t(0) : (uint64_t(-1) >> overlappingElements * 8u);
				const uint8x8_t mask_u_8x8 = vcreate_u8(mask);

				const uint8x8x3_t patch_u_8x8x3 = vld3_u8(patch);

				sumChannel0_u_32x4 = vaddw_u16(sumChannel0_u_32x4, vpaddl_u8(vand_u8(patch_u_8x8x3.val[0], mask_u_8x8)));
				sumChannel1_u_32x4 = vaddw_u16(sumChannel1_u_32x4, vpaddl_u8(vand_u8(patch_u_8x8x3.val[1], mask_u_8x8)));
				sumChannel2_u_32x4 = vaddw_u16(sumChannel2_u_32x4, vpaddl_u8(vand_u8(patch_u_8x8x3.val[2], mask_u_8x8)));
			}
			else
			{
				constexpr uint64_t mask = overlappingElements >= 8u ? uint64_t(0) : (uint64_t(-1) << overlappingElements * 8u);
				const uint8x8_t mask_u_8x8 = vcreate_u8(mask);

				const uint8x8x3_t patch_u_8x8x3 = vld3_u8(patch - overlappingElements * tChannels);

				sumChannel0_u_32x4 = vaddw_u16(sumChannel0_u_32x4, vpaddl_u8(vand_u8(patch_u_8x8x3.val[0], mask_u_8x8)));
				sumChannel1_u_32x4 = vaddw_u16(sumChannel1_u_32x4, vpaddl_u8(vand_u8(patch_u_8x8x3.val[1], mask_u_8x8)));
				sumChannel2_u_32x4 = vaddw_u16(sumChannel2_u_32x4, vpaddl_u8(vand_u8(patch_u_8x8x3.val[2], mask_u_8x8)));
			}

			patch += remainingAfterBlocks8 * tChannels;
		}

		if constexpr (blocks1 != 0u)
		{
			for (unsigned int n = 0u; n < blocks1; ++n)
			{
				sumIndividual[0] += patch[tChannels * n + 0u];
				sumIndividual[1] += patch[tChannels * n + 1u];
				sumIndividual[2] += patch[tChannels * n + 2u];
			}

			patch += blocks1 * tChannels;
		}

		patch += patchStrideElements - tChannels * tPatchSize;
	}

	uint32_t results[4];
	vst1q_u32(results, sumChannel0_u_32x4);

	const uint32_t sum0 = results[0] + results[1] + results[2] + results[3] + sumIndividual[0];
	meanValues[0] = uint8_t((sum0 + tPatchSize * tPatchSize / 2u) / (tPatchSize * tPatchSize));

	vst1q_u32(results, sumChannel1_u_32x4);

	const uint32_t sum1 = results[0] + results[1] + results[2] + results[3] + sumIndividual[1];
	meanValues[1] = uint8_t((sum1 + tPatchSize * tPatchSize / 2u) / (tPatchSize * tPatchSize));

	vst1q_u32(results, sumChannel2_u_32x4);

	const uint32_t sum2 = results[0] + results[1] + results[2] + results[3] + sumIndividual[2];
	meanValues[2] = uint8_t((sum2 + tPatchSize * tPatchSize / 2u) / (tPatchSize * tPatchSize));
}

template <unsigned int tChannels>
template <unsigned int tPatchSize>
inline void ZeroMeanSumSquareDifferencesNEON::SpecializedForChannels<tChannels>::mean8BitPerChannel(const uint8_t* patch, const unsigned int patchStrideElements, uint8_t* const meanValues)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize >= 1u, "Invalid patch size!");

	ocean_assert(patch != nullptr && meanValues != nullptr);

	ocean_assert(patchStrideElements >= tChannels * tPatchSize);

	uint32_t sum[tChannels] = {0u};

	for (unsigned int y = 0u; y < tPatchSize; ++y)
	{
		for (unsigned int x = 0u; x < tPatchSize; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				sum[n] += patch[x * tChannels + n];
			}
		}

		patch += patchStrideElements;
	}

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		meanValues[n] = uint8_t((sum[n] + tPatchSize * tPatchSize / 2u) / (tPatchSize * tPatchSize));
	}
}

template <>
template <unsigned int tPatchSize>
inline void ZeroMeanSumSquareDifferencesNEON::SpecializedForChannels<1u>::mean8BitPerChannelMirroredBorder(const uint8_t* const image, const unsigned int width, const unsigned int height, const unsigned int centerX, const unsigned int centerY, const unsigned int imagePaddingElements, uint8_t* const meanValues)
{
	static_assert(tPatchSize % 2u == 1u, "Invalid patch size!");

	ocean_assert(image != nullptr && meanValues != nullptr);
	ocean_assert(centerX < width && centerY < height);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	const unsigned int imageStrideElements = width + imagePaddingElements;

	constexpr unsigned int blocks16 = tPatchSize / 16u;
	constexpr unsigned int remainingAfterBlocks16 = tPatchSize % 16u;

	constexpr bool partialBlock16 = remainingAfterBlocks16 > 10u;
	constexpr unsigned int remainingAfterPartialBlock16 = partialBlock16 ? 0u : remainingAfterBlocks16;

	constexpr unsigned int blocks8 = remainingAfterPartialBlock16 / 8u;
	constexpr unsigned int remainingAfterBlocks8 = remainingAfterPartialBlock16 % 8u;

	constexpr bool partialBlock8 = remainingAfterBlocks8 >= 3u;
	constexpr unsigned int remainingAfterPartialBlock8 = partialBlock8 ? 0u : remainingAfterBlocks8;

	constexpr unsigned int blocks1 = remainingAfterPartialBlock8;

	static_assert(blocks1 <= 7u, "Invalid block size!");

	uint32x4_t sum_u_32x4 = vdupq_n_u32(0u);

	uint32_t sumIndividual = 0u;

	uint8_t intermediate[16];

	for (int y = int(centerY) - int(tPatchSize_2); y <= int(centerY) + int(tPatchSize_2); ++y)
	{
		const uint8_t* const mirroredRow = image + CVUtilities::mirrorIndex(y, height) * imageStrideElements;

		int x = int(centerX) - int(tPatchSize_2);

		for (unsigned int n = 0u; n < blocks16; ++n)
		{
			const uint8x16_t patch_u_8x16 = loadMirrored_u_8x16<true, 16u, true>(mirroredRow, x, width, intermediate);

			sum_u_32x4 = vpadalq_u16(sum_u_32x4, vpaddlq_u8(patch_u_8x16));

			x += 16;
		}

		if constexpr (partialBlock16)
		{
			if (y < int(centerY) + int(tPatchSize_2))
			{
				const uint8x16_t patch_u_8x16 = loadMirrored_u_8x16<true, remainingAfterBlocks16, true>(mirroredRow, x, width, intermediate);

				sum_u_32x4 = vpadalq_u16(sum_u_32x4, vpaddlq_u8(patch_u_8x16));
			}
			else
			{
				const uint8x16_t patch_u_8x16 = loadMirrored_u_8x16<false, remainingAfterBlocks16, true>(mirroredRow, x, width, intermediate);

				sum_u_32x4 = vpadalq_u16(sum_u_32x4, vpaddlq_u8(patch_u_8x16));
			}

			x += remainingAfterBlocks16;
		}

		for (unsigned int n = 0u; n < blocks8; ++n)
		{
			const uint8x8_t patch_u_8x8 = loadMirrored_u_8x8<true, 8u, true>(mirroredRow, x, width, intermediate);

			sum_u_32x4 = vaddw_u16(sum_u_32x4, vpaddl_u8(patch_u_8x8));

			x += 8;
		}

		if constexpr (partialBlock8)
		{
			if (y < int(centerY) + int(tPatchSize_2))
			{
				const uint8x8_t patch_u_8x8 = loadMirrored_u_8x8<true, remainingAfterBlocks8, true>(mirroredRow, x, width, intermediate);

				sum_u_32x4 = vaddw_u16(sum_u_32x4, vpaddl_u8(patch_u_8x8));
			}
			else
			{
				const uint8x8_t patch_u_8x8 = loadMirrored_u_8x8<false, remainingAfterBlocks8, true>(mirroredRow, x, width, intermediate);

				sum_u_32x4 = vaddw_u16(sum_u_32x4, vpaddl_u8(patch_u_8x8));
			}

			x += remainingAfterBlocks8;
		}

		if constexpr (blocks1 != 0u)
		{
			for (unsigned int n = 0u; n < blocks1; ++n)
			{
				const unsigned int index = CVUtilities::mirrorIndex(x, width);

				sumIndividual += mirroredRow[index];

				x++;
			}
		}
	}

	uint32_t results[4];
	vst1q_u32(results, sum_u_32x4);

	const uint32_t sum = results[0] + results[1] + results[2] + results[3] + sumIndividual;

	meanValues[0] = uint8_t((sum + tPatchSize * tPatchSize / 2u) / (tPatchSize * tPatchSize));
}

template <unsigned int tChannels>
template <unsigned int tPatchSize>
inline void ZeroMeanSumSquareDifferencesNEON::SpecializedForChannels<tChannels>::mean8BitPerChannelMirroredBorder(const uint8_t* const image, const unsigned int width, const unsigned int height, const unsigned int centerX, const unsigned int centerY, const unsigned int imagePaddingElements, uint8_t* const meanValues)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize % 2u == 1u, "Invalid patch size!");

	ocean_assert(image != nullptr && meanValues != nullptr);
	ocean_assert(centerX < width && centerY < height);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	const unsigned int imageStrideElements = width * tChannels + imagePaddingElements;

	uint32_t sum[tChannels] = {0u};

	for (int y = int(centerY) - int(tPatchSize_2); y <= int(centerY) + int(tPatchSize_2); ++y)
	{
		const uint8_t* const mirroredRow = image + CVUtilities::mirrorIndex(y, height) * imageStrideElements;

		for (int x = int(centerX) - int(tPatchSize_2); x <= int(centerX) + int(tPatchSize_2); ++x)
		{
			const uint8_t* const pixel = mirroredRow + CVUtilities::mirrorIndex(x, width) * tChannels;

			for (unsigned int c = 0u; c < tChannels; ++c)
			{
				sum[c] += pixel[c];
			}
		}
	}

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		meanValues[n] = uint8_t((sum[n] + tPatchSize * tPatchSize / 2u) / (tPatchSize * tPatchSize));
	}
}

template <>
template <unsigned int tPixels>
inline uint32_t ZeroMeanSumSquareDifferencesNEON::SpecializedForChannels<1u>::buffer8BitPerChannel(const uint8_t* buffer0, const uint8_t* buffer1, const uint8_t* const meanValues0, const uint8_t* const meanValues1)
{
	static_assert(tPixels >= 8u, "Invalid pixels!");

	ocean_assert(buffer0 != nullptr && buffer1 != nullptr);
	ocean_assert(meanValues0 != nullptr && meanValues1 != nullptr);

	constexpr unsigned int blocks16 = tPixels / 16u;
	constexpr unsigned int remainingAfterBlocks16 = tPixels % 16u;

	constexpr bool partialBlock16 = remainingAfterBlocks16 > 10u;
	constexpr unsigned int remainingAfterPartialBlock16 = partialBlock16 ? 0u : remainingAfterBlocks16;

	constexpr unsigned int blocks8 = remainingAfterPartialBlock16 / 8u;
	constexpr unsigned int remainingAfterBlocks8 = remainingAfterPartialBlock16 % 8u;

	constexpr bool partialBlock8 = remainingAfterBlocks8 >= 3u;
	constexpr unsigned int remainingAfterPartialBlock8 = partialBlock8 ? 0u : remainingAfterBlocks8;

	constexpr unsigned int blocks1 = remainingAfterPartialBlock8;

	static_assert(blocks1 <= 2u, "Invalid block size!");

	// [(buffer0 - mean0) - (buffer1 - mean1)]^2
	// [buffer0 - buffer1 - mean0 + mean1]^2

	const int16x8_t mean0_1_s_16x8 = vdupq_n_s16(int16_t(meanValues0[0]) - int16_t(meanValues1[0]));

	uint32x4_t sumA_u_32x4 = vdupq_n_u32(0u);
	uint32x4_t sumB_u_32x4 = vdupq_n_u32(0u);

	uint32_t sumIndividual = 0u;

	for (unsigned int n = 0u; n < blocks16; ++n)
	{
		const uint8x16_t buffer0_u_8x16 = vld1q_u8(buffer0);
		const uint8x16_t buffer1_u_8x16 = vld1q_u8(buffer1);

		const int16x8_t bufferLow0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(buffer0_u_8x16), vget_low_u8(buffer1_u_8x16))); // low 8 bytes: buffer0 - buffer1
		const int16x8_t bufferHigh0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(buffer0_u_8x16), vget_high_u8(buffer1_u_8x16))); // high 8 bytes: buffer0 - buffer1

		const uint16x8_t buffer_mean_low_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(bufferLow0_1_s_16x8, mean0_1_s_16x8)); // |(buffer0 - buffer1) - (mean1 - mean0)|, with range [0, 255 * 255]
		const uint16x8_t buffer_mean_high_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(bufferHigh0_1_s_16x8, mean0_1_s_16x8));

		sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(buffer_mean_low_u_16x8), vget_low_u16(buffer_mean_low_u_16x8));
		sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(buffer_mean_low_u_16x8), vget_high_u16(buffer_mean_low_u_16x8));

		sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(buffer_mean_high_u_16x8), vget_low_u16(buffer_mean_high_u_16x8));
		sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(buffer_mean_high_u_16x8), vget_high_u16(buffer_mean_high_u_16x8));

		buffer0 += 16;
		buffer1 += 16;
	}

	if constexpr (partialBlock16)
	{
		constexpr unsigned int overlappingElements = 16u - remainingAfterBlocks16;
		ocean_assert(overlappingElements < 8u);

		const uint8x16_t buffer0_u_8x16 = vld1q_u8(buffer0 - overlappingElements);
		const uint8x16_t buffer1_u_8x16 = vld1q_u8(buffer1 - overlappingElements);

		const int16x8_t bufferLow0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(buffer0_u_8x16), vget_low_u8(buffer1_u_8x16))); // low 8 bytes: buffer0 - buffer1
		const int16x8_t bufferHigh0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(buffer0_u_8x16), vget_high_u8(buffer1_u_8x16))); // high 8 bytes: buffer0 - buffer1

		constexpr uint64_t maskLow = overlappingElements >= 4u ? uint64_t(0) : (uint64_t(-1) << overlappingElements * 2u * 8u);
		constexpr uint64_t maskHigh = (overlappingElements <= 4u || overlappingElements >= 8u) ? uint64_t(-1) : (uint64_t(-1) << (overlappingElements - 4u) * 2u * 8u);

		const uint16x8_t mask_u_16x8 = vcombine_u16(vcreate_u16(maskLow), vcreate_u16(maskHigh));

		const uint16x8_t buffer_mean_low_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(bufferLow0_1_s_16x8, mean0_1_s_16x8)), mask_u_16x8); // |(buffer0 - buffer1) - (mean1 - mean0)|, with range [0, 255 * 255]
		const uint16x8_t buffer_mean_high_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(bufferHigh0_1_s_16x8, mean0_1_s_16x8));

		sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(buffer_mean_low_u_16x8), vget_low_u16(buffer_mean_low_u_16x8));
		sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(buffer_mean_low_u_16x8), vget_high_u16(buffer_mean_low_u_16x8));

		sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(buffer_mean_high_u_16x8), vget_low_u16(buffer_mean_high_u_16x8));
		sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(buffer_mean_high_u_16x8), vget_high_u16(buffer_mean_high_u_16x8));

		buffer0 += remainingAfterBlocks16;
		buffer1 += remainingAfterBlocks16;
	}

	for (unsigned int n = 0u; n < blocks8; ++n)
	{
		const uint8x8_t buffer0_u_8x8 = vld1_u8(buffer0);
		const uint8x8_t buffer1_u_8x8 = vld1_u8(buffer1);

		const int16x8_t buffer0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(buffer0_u_8x8, buffer1_u_8x8)); // buffer0 - buffer1

		const uint16x8_t buffer_mean_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(buffer0_1_s_16x8, mean0_1_s_16x8)); // |(buffer0 - buffer1) - (mean1 - mean0)|, with range [0, 255 * 255]

		sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(buffer_mean_u_16x8), vget_low_u16(buffer_mean_u_16x8));
		sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(buffer_mean_u_16x8), vget_high_u16(buffer_mean_u_16x8));

		buffer0 += 8;
		buffer1 += 8;
	}

	if constexpr (partialBlock8)
	{
		constexpr unsigned int overlappingElements = 8u - remainingAfterBlocks8;
		ocean_assert(overlappingElements < 8u);

		const uint8x8_t buffer0_u_8x8 = vld1_u8(buffer0 - overlappingElements);
		const uint8x8_t buffer1_u_8x8 = vld1_u8(buffer1 - overlappingElements);

		const int16x8_t buffer0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(buffer0_u_8x8, buffer1_u_8x8)); // buffer0 - buffer1

		constexpr uint64_t maskLow = overlappingElements >= 4u ? uint64_t(0) : (uint64_t(-1) << overlappingElements * 2u * 8u);
		constexpr uint64_t maskHigh = (overlappingElements <= 4u || overlappingElements >= 8u) ? uint64_t(-1) : (uint64_t(-1) << (overlappingElements - 4u) * 2u * 8u);

		const uint16x8_t mask_u_16x8 = vcombine_u16(vcreate_u16(maskLow), vcreate_u16(maskHigh));

		const uint16x8_t buffer_mean_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(buffer0_1_s_16x8, mean0_1_s_16x8)), mask_u_16x8); // |(buffer0 - buffer1) - (mean1 - mean0)|, with range [0, 255 * 255]

		sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(buffer_mean_u_16x8), vget_low_u16(buffer_mean_u_16x8));
		sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(buffer_mean_u_16x8), vget_high_u16(buffer_mean_u_16x8));

		buffer0 += remainingAfterBlocks8;
		buffer1 += remainingAfterBlocks8;
	}

	if constexpr (blocks1 != 0u)
	{
		for (unsigned int n = 0u; n < blocks1; ++n)
		{
			sumIndividual += sqrDistance(int16_t(buffer0[n] - meanValues0[0]), int16_t(buffer1[n] - meanValues1[0]));
		}
	}

	const uint32x4_t sum_u_32x4 = vaddq_u32(sumA_u_32x4, sumB_u_32x4);

	uint32_t results[4];
	vst1q_u32(results, sum_u_32x4);

	return results[0] + results[1] + results[2] + results[3] + sumIndividual;
}

template <>
template <unsigned int tPixels>
inline uint32_t ZeroMeanSumSquareDifferencesNEON::SpecializedForChannels<3u>::buffer8BitPerChannel(const uint8_t* buffer0, const uint8_t* buffer1, const uint8_t* const meanValues0, const uint8_t* const meanValues1)
{
	static_assert(tPixels >= 8u, "Invalid pixels!");

	constexpr unsigned int tChannels = 3u;

	ocean_assert(buffer0 != nullptr && buffer1 != nullptr);
	ocean_assert(meanValues0 != nullptr && meanValues1 != nullptr);

	constexpr unsigned int blocks16 = tPixels / 16u;
	constexpr unsigned int remainingAfterBlocks16 = tPixels % 16u;

	constexpr bool partialBlock16 = remainingAfterBlocks16 > 10u;
	constexpr unsigned int remainingAfterPartialBlock16 = partialBlock16 ? 0u : remainingAfterBlocks16;

	constexpr unsigned int blocks8 = remainingAfterPartialBlock16 / 8u;
	constexpr unsigned int remainingAfterBlocks8 = remainingAfterPartialBlock16 % 8u;

	constexpr bool partialBlock8 = remainingAfterBlocks8 >= 3u;
	constexpr unsigned int remainingAfterPartialBlock8 = partialBlock8 ? 0u : remainingAfterBlocks8;

	constexpr unsigned int blocks1 = remainingAfterPartialBlock8;

	static_assert(blocks1 <= 2u, "Invalid block size!");

	// [(buffer0 - mean0) - (buffer1 - mean1)]^2
	// [buffer0 - buffer1 - mean0 + mean1]^2

	const int16x8_t meanChannel0_0_1_s_16x8 = vdupq_n_s16(int16_t(meanValues0[0]) - int16_t(meanValues1[0]));
	const int16x8_t meanChannel1_0_1_s_16x8 = vdupq_n_s16(int16_t(meanValues0[1]) - int16_t(meanValues1[1]));
	const int16x8_t meanChannel2_0_1_s_16x8 = vdupq_n_s16(int16_t(meanValues0[2]) - int16_t(meanValues1[2]));

	uint32x4_t sumA_u_32x4 = vdupq_n_u32(0u);
	uint32x4_t sumB_u_32x4 = vdupq_n_u32(0u);

	uint32_t sumIndividual = 0u;

	for (unsigned int n = 0u; n < blocks16; ++n)
	{
		const uint8x16x3_t buffer0_u_8x16x3 = vld3q_u8(buffer0);
		const uint8x16x3_t buffer1_u_8x16x3 = vld3q_u8(buffer1);

		const int16x8_t bufferChannel0Low0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(buffer0_u_8x16x3.val[0]), vget_low_u8(buffer1_u_8x16x3.val[0]))); // low 8 bytes: buffer0 - buffer1
		const int16x8_t bufferChannel0High0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(buffer0_u_8x16x3.val[0]), vget_high_u8(buffer1_u_8x16x3.val[0]))); // high 8 bytes: buffer0 - buffer1

		const int16x8_t bufferChannel1Low0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(buffer0_u_8x16x3.val[1]), vget_low_u8(buffer1_u_8x16x3.val[1])));
		const int16x8_t bufferChannel1High0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(buffer0_u_8x16x3.val[1]), vget_high_u8(buffer1_u_8x16x3.val[1])));

		const int16x8_t bufferChannel2Low0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(buffer0_u_8x16x3.val[2]), vget_low_u8(buffer1_u_8x16x3.val[2])));
		const int16x8_t bufferChannel2High0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(buffer0_u_8x16x3.val[2]), vget_high_u8(buffer1_u_8x16x3.val[2])));


		const uint16x8_t bufferChannel0_mean_low_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(bufferChannel0Low0_1_s_16x8, meanChannel0_0_1_s_16x8)); // |(buffer0 - buffer1) - (mean1 - mean0)|, with range [0, 255 * 255]
		const uint16x8_t bufferChannel0_mean_high_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(bufferChannel0High0_1_s_16x8, meanChannel0_0_1_s_16x8));

		const uint16x8_t bufferChannel1_mean_low_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(bufferChannel1Low0_1_s_16x8, meanChannel1_0_1_s_16x8));
		const uint16x8_t bufferChannel1_mean_high_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(bufferChannel1High0_1_s_16x8, meanChannel1_0_1_s_16x8));

		const uint16x8_t bufferChannel2_mean_low_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(bufferChannel2Low0_1_s_16x8, meanChannel2_0_1_s_16x8));
		const uint16x8_t bufferChannel2_mean_high_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(bufferChannel2High0_1_s_16x8, meanChannel2_0_1_s_16x8));


		sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(bufferChannel0_mean_low_u_16x8), vget_low_u16(bufferChannel0_mean_low_u_16x8));
		sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(bufferChannel0_mean_low_u_16x8), vget_high_u16(bufferChannel0_mean_low_u_16x8));
		sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(bufferChannel0_mean_high_u_16x8), vget_low_u16(bufferChannel0_mean_high_u_16x8));
		sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(bufferChannel0_mean_high_u_16x8), vget_high_u16(bufferChannel0_mean_high_u_16x8));

		sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(bufferChannel1_mean_low_u_16x8), vget_low_u16(bufferChannel1_mean_low_u_16x8));
		sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(bufferChannel1_mean_low_u_16x8), vget_high_u16(bufferChannel1_mean_low_u_16x8));
		sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(bufferChannel1_mean_high_u_16x8), vget_low_u16(bufferChannel1_mean_high_u_16x8));
		sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(bufferChannel1_mean_high_u_16x8), vget_high_u16(bufferChannel1_mean_high_u_16x8));

		sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(bufferChannel2_mean_low_u_16x8), vget_low_u16(bufferChannel2_mean_low_u_16x8));
		sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(bufferChannel2_mean_low_u_16x8), vget_high_u16(bufferChannel2_mean_low_u_16x8));
		sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(bufferChannel2_mean_high_u_16x8), vget_low_u16(bufferChannel2_mean_high_u_16x8));
		sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(bufferChannel2_mean_high_u_16x8), vget_high_u16(bufferChannel2_mean_high_u_16x8));


		buffer0 += 16u * tChannels;
		buffer1 += 16u * tChannels;
	}

	if constexpr (partialBlock16)
	{
		constexpr unsigned int overlappingElements = 16u - remainingAfterBlocks16;
		ocean_assert(overlappingElements < 8u);

		const uint8x16x3_t buffer0_u_8x16x3 = vld3q_u8(buffer0 - overlappingElements * tChannels);
		const uint8x16x3_t buffer1_u_8x16x3 = vld3q_u8(buffer1 - overlappingElements * tChannels);


		const int16x8_t bufferChannel0Low0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(buffer0_u_8x16x3.val[0]), vget_low_u8(buffer1_u_8x16x3.val[0]))); // low 8 bytes: buffer0 - buffer1
		const int16x8_t bufferChannel0High0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(buffer0_u_8x16x3.val[0]), vget_high_u8(buffer1_u_8x16x3.val[0]))); // high 8 bytes: buffer0 - buffer1

		const int16x8_t bufferChannel1Low0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(buffer0_u_8x16x3.val[1]), vget_low_u8(buffer1_u_8x16x3.val[1])));
		const int16x8_t bufferChannel1High0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(buffer0_u_8x16x3.val[1]), vget_high_u8(buffer1_u_8x16x3.val[1])));

		const int16x8_t bufferChannel2Low0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(buffer0_u_8x16x3.val[2]), vget_low_u8(buffer1_u_8x16x3.val[2])));
		const int16x8_t bufferChannel2High0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(buffer0_u_8x16x3.val[2]), vget_high_u8(buffer1_u_8x16x3.val[2])));


		constexpr uint64_t maskLow = overlappingElements >= 4u ? uint64_t(0) : (uint64_t(-1) << overlappingElements * 2u * 8u);
		constexpr uint64_t maskHigh = (overlappingElements <= 4u || overlappingElements >= 8u) ? uint64_t(-1) : (uint64_t(-1) << (overlappingElements - 4u) * 2u * 8u);

		const uint16x8_t mask_u_16x8 = vcombine_u16(vcreate_u16(maskLow), vcreate_u16(maskHigh));


		const uint16x8_t bufferChannel0_mean_low_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(bufferChannel0Low0_1_s_16x8, meanChannel0_0_1_s_16x8)), mask_u_16x8); // |(buffer0 - buffer1) - (mean1 - mean0)|, with range [0, 255 * 255]
		const uint16x8_t bufferChannel0_mean_high_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(bufferChannel0High0_1_s_16x8, meanChannel0_0_1_s_16x8));

		const uint16x8_t bufferChannel1_mean_low_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(bufferChannel1Low0_1_s_16x8, meanChannel1_0_1_s_16x8)), mask_u_16x8);
		const uint16x8_t bufferChannel1_mean_high_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(bufferChannel1High0_1_s_16x8, meanChannel1_0_1_s_16x8));

		const uint16x8_t bufferChannel2_mean_low_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(bufferChannel2Low0_1_s_16x8, meanChannel2_0_1_s_16x8)), mask_u_16x8);
		const uint16x8_t bufferChannel2_mean_high_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(bufferChannel2High0_1_s_16x8, meanChannel2_0_1_s_16x8));


		sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(bufferChannel0_mean_low_u_16x8), vget_low_u16(bufferChannel0_mean_low_u_16x8));
		sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(bufferChannel0_mean_low_u_16x8), vget_high_u16(bufferChannel0_mean_low_u_16x8));
		sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(bufferChannel0_mean_high_u_16x8), vget_low_u16(bufferChannel0_mean_high_u_16x8));
		sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(bufferChannel0_mean_high_u_16x8), vget_high_u16(bufferChannel0_mean_high_u_16x8));

		sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(bufferChannel1_mean_low_u_16x8), vget_low_u16(bufferChannel1_mean_low_u_16x8));
		sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(bufferChannel1_mean_low_u_16x8), vget_high_u16(bufferChannel1_mean_low_u_16x8));
		sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(bufferChannel1_mean_high_u_16x8), vget_low_u16(bufferChannel1_mean_high_u_16x8));
		sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(bufferChannel1_mean_high_u_16x8), vget_high_u16(bufferChannel1_mean_high_u_16x8));

		sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(bufferChannel2_mean_low_u_16x8), vget_low_u16(bufferChannel2_mean_low_u_16x8));
		sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(bufferChannel2_mean_low_u_16x8), vget_high_u16(bufferChannel2_mean_low_u_16x8));
		sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(bufferChannel2_mean_high_u_16x8), vget_low_u16(bufferChannel2_mean_high_u_16x8));
		sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(bufferChannel2_mean_high_u_16x8), vget_high_u16(bufferChannel2_mean_high_u_16x8));

		buffer0 += remainingAfterBlocks16 * tChannels;
		buffer1 += remainingAfterBlocks16 * tChannels;
	}

	for (unsigned int n = 0u; n < blocks8; ++n)
	{
		const uint8x8x3_t buffer0_u_8x8x3 = vld3_u8(buffer0);
		const uint8x8x3_t buffer1_u_8x8x3 = vld3_u8(buffer1);

		const int16x8_t bufferChannel0_0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(buffer0_u_8x8x3.val[0], buffer1_u_8x8x3.val[0])); // buffer0 - buffer1
		const int16x8_t bufferChannel1_0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(buffer0_u_8x8x3.val[1], buffer1_u_8x8x3.val[1]));
		const int16x8_t bufferChannel2_0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(buffer0_u_8x8x3.val[2], buffer1_u_8x8x3.val[2]));

		const uint16x8_t bufferChannel0_mean_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(bufferChannel0_0_1_s_16x8, meanChannel0_0_1_s_16x8)); // |(buffer0 - buffer1) - (mean1 - mean0)|, with range [0, 255 * 255]
		const uint16x8_t bufferChannel1_mean_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(bufferChannel1_0_1_s_16x8, meanChannel1_0_1_s_16x8));
		const uint16x8_t bufferChannel2_mean_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(bufferChannel2_0_1_s_16x8, meanChannel2_0_1_s_16x8));

		sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(bufferChannel0_mean_u_16x8), vget_low_u16(bufferChannel0_mean_u_16x8));
		sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(bufferChannel0_mean_u_16x8), vget_high_u16(bufferChannel0_mean_u_16x8));

		sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(bufferChannel1_mean_u_16x8), vget_low_u16(bufferChannel1_mean_u_16x8));
		sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(bufferChannel1_mean_u_16x8), vget_high_u16(bufferChannel1_mean_u_16x8));

		sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(bufferChannel2_mean_u_16x8), vget_low_u16(bufferChannel2_mean_u_16x8));
		sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(bufferChannel2_mean_u_16x8), vget_high_u16(bufferChannel2_mean_u_16x8));

		buffer0 += 8u * tChannels;
		buffer1 += 8u * tChannels;
	}

	if constexpr (partialBlock8)
	{
		constexpr unsigned int overlappingElements = 8u - remainingAfterBlocks8;
		ocean_assert(overlappingElements < 8u);

		const uint8x8x3_t buffer0_u_8x8x3 = vld3_u8(buffer0 - overlappingElements * tChannels);
		const uint8x8x3_t buffer1_u_8x8x3 = vld3_u8(buffer1 - overlappingElements * tChannels);

		const int16x8_t bufferChannel0_0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(buffer0_u_8x8x3.val[0], buffer1_u_8x8x3.val[0])); // buffer0 - buffer1
		const int16x8_t bufferChannel1_0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(buffer0_u_8x8x3.val[1], buffer1_u_8x8x3.val[1]));
		const int16x8_t bufferChannel2_0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(buffer0_u_8x8x3.val[2], buffer1_u_8x8x3.val[2]));

		constexpr uint64_t maskLow = overlappingElements >= 4u ? uint64_t(0) : (uint64_t(-1) << overlappingElements * 2u * 8u);
		constexpr uint64_t maskHigh = (overlappingElements <= 4u || overlappingElements >= 8u) ? uint64_t(-1) : (uint64_t(-1) << (overlappingElements - 4u) * 2u * 8u);

		const uint16x8_t mask_u_16x8 = vcombine_u16(vcreate_u16(maskLow), vcreate_u16(maskHigh));

		const uint16x8_t bufferChannel0_mean_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(bufferChannel0_0_1_s_16x8, meanChannel0_0_1_s_16x8)), mask_u_16x8); // |(buffer0 - buffer1) - (mean1 - mean0)|, with range [0, 255 * 255]
		const uint16x8_t bufferChannel1_mean_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(bufferChannel1_0_1_s_16x8, meanChannel1_0_1_s_16x8)), mask_u_16x8);
		const uint16x8_t bufferChannel2_mean_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(bufferChannel2_0_1_s_16x8, meanChannel2_0_1_s_16x8)), mask_u_16x8);

		sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(bufferChannel0_mean_u_16x8), vget_low_u16(bufferChannel0_mean_u_16x8));
		sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(bufferChannel0_mean_u_16x8), vget_high_u16(bufferChannel0_mean_u_16x8));

		sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(bufferChannel1_mean_u_16x8), vget_low_u16(bufferChannel1_mean_u_16x8));
		sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(bufferChannel1_mean_u_16x8), vget_high_u16(bufferChannel1_mean_u_16x8));

		sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(bufferChannel2_mean_u_16x8), vget_low_u16(bufferChannel2_mean_u_16x8));
		sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(bufferChannel2_mean_u_16x8), vget_high_u16(bufferChannel2_mean_u_16x8));

		buffer0 += remainingAfterBlocks8 * tChannels;
		buffer1 += remainingAfterBlocks8 * tChannels;
	}

	if constexpr (blocks1 != 0u)
	{
		for (unsigned int n = 0u; n < blocks1; ++n)
		{
			for (unsigned int c = 0u; c < tChannels; ++c)
			{
				sumIndividual += sqrDistance(int16_t(buffer0[n * tChannels + c] - meanValues0[c]), int16_t(buffer1[n * tChannels + c] - meanValues1[c]));
			}
		}

		buffer0 += blocks1 * tChannels;
		buffer1 += blocks1 * tChannels;
	}

	const uint32x4_t sum_u_32x4 = vaddq_u32(sumA_u_32x4, sumB_u_32x4);

	uint32_t results[4];
	vst1q_u32(results, sum_u_32x4);

	return results[0] + results[1] + results[2] + results[3] + sumIndividual;
}

template <unsigned int tChannels>
template <unsigned int tPixels>
inline uint32_t ZeroMeanSumSquareDifferencesNEON::SpecializedForChannels<tChannels>::buffer8BitPerChannel(const uint8_t* buffer0, const uint8_t* buffer1, const uint8_t* const meanValues0, const uint8_t* const meanValues1)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPixels >= 1u, "Invalid pixels!");

	ocean_assert(buffer0 != nullptr && buffer1 != nullptr);
	ocean_assert(meanValues0 != nullptr && meanValues1 != nullptr);

	uint32_t zmssd = 0u;

	for (unsigned int x = 0u; x < tPixels; ++x)
	{
		for (unsigned int c = 0u; c < tChannels; ++c)
		{
			zmssd += sqrDistance(buffer0[x * tChannels + c] - meanValues0[c], buffer1[x * tChannels + c] - meanValues1[c]);
		}
	}

	return zmssd;
}

template <>
template <unsigned int tPatchSize>
inline uint32_t ZeroMeanSumSquareDifferencesNEON::SpecializedForChannels<1u>::patch8BitPerChannel(const uint8_t* patch0, const uint8_t* patch1, const unsigned int patch0StrideElements, const unsigned int patch1StrideElements, const uint8_t* const meanValues0, const uint8_t* const meanValues1)
{
	static_assert(tPatchSize >= 5u, "Invalid patch size!");

	ocean_assert(patch0 != nullptr && patch1 != nullptr);
	ocean_assert(meanValues0 != nullptr && meanValues1 != nullptr);

	ocean_assert(patch0StrideElements >= tPatchSize);
	ocean_assert(patch1StrideElements >= tPatchSize);

	constexpr unsigned int blocks16 = tPatchSize / 16u;
	constexpr unsigned int remainingAfterBlocks16 = tPatchSize % 16u;

	constexpr bool partialBlock16 = remainingAfterBlocks16 > 10u;
	constexpr unsigned int remainingAfterPartialBlock16 = partialBlock16 ? 0u : remainingAfterBlocks16;

	constexpr unsigned int blocks8 = remainingAfterPartialBlock16 / 8u;
	constexpr unsigned int remainingAfterBlocks8 = remainingAfterPartialBlock16 % 8u;

	constexpr bool partialBlock8 = remainingAfterBlocks8 >= 3u;
	constexpr unsigned int remainingAfterPartialBlock8 = partialBlock8 ? 0u : remainingAfterBlocks8;

	constexpr unsigned int blocks1 = remainingAfterPartialBlock8;

	static_assert(blocks1 <= 2u, "Invalid block size!");

	// [(patch0 - mean0) - (patch1 - mean1)]^2
	// [patch0 - patch1 - mean0 + mean1]^2

	const int16x8_t mean0_1_s_16x8 = vdupq_n_s16(int16_t(meanValues0[0]) - int16_t(meanValues1[0]));

	uint32x4_t sumA_u_32x4 = vdupq_n_u32(0u);
	uint32x4_t sumB_u_32x4 = vdupq_n_u32(0u);

	uint32_t sumIndividual = 0u;

	for (unsigned int y = 0u; y < tPatchSize; ++y)
	{
		for (unsigned int n = 0u; n < blocks16; ++n)
		{
			const uint8x16_t patch0_u_8x16 = vld1q_u8(patch0);
			const uint8x16_t patch1_u_8x16 = vld1q_u8(patch1);

			const int16x8_t patchLow0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(patch0_u_8x16), vget_low_u8(patch1_u_8x16))); // low 8 bytes: patch0 - patch1
			const int16x8_t patchHigh0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(patch0_u_8x16), vget_high_u8(patch1_u_8x16))); // high 8 bytes: patch0 - patch1

			const uint16x8_t patch_mean_low_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(patchLow0_1_s_16x8, mean0_1_s_16x8)); // |(patch0 - patch1) - (mean1 - mean0)|, with range [0, 255 * 255]
			const uint16x8_t patch_mean_high_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(patchHigh0_1_s_16x8, mean0_1_s_16x8));

			sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patch_mean_low_u_16x8), vget_low_u16(patch_mean_low_u_16x8));
			sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patch_mean_low_u_16x8), vget_high_u16(patch_mean_low_u_16x8));

			sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patch_mean_high_u_16x8), vget_low_u16(patch_mean_high_u_16x8));
			sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patch_mean_high_u_16x8), vget_high_u16(patch_mean_high_u_16x8));

			patch0 += 16;
			patch1 += 16;
		}

		if constexpr (partialBlock16)
		{
			constexpr unsigned int overlappingElements = 16u - remainingAfterBlocks16;
			ocean_assert(overlappingElements < 8u);

			if (y < tPatchSize - 1u)
			{
				const uint8x16_t patch0_u_8x16 = vld1q_u8(patch0);
				const uint8x16_t patch1_u_8x16 = vld1q_u8(patch1);

				const int16x8_t patchLow0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(patch0_u_8x16), vget_low_u8(patch1_u_8x16))); // low 8 bytes: patch0 - patch1
				const int16x8_t patchHigh0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(patch0_u_8x16), vget_high_u8(patch1_u_8x16))); // high 8 bytes: patch0 - patch1

				// mask: |<- overlapping ->|<- remainingAfterBlocks16 ->|
				//        00 00 00 00 00 00 FF FF FF FF FF FF FF FF FF FF

				constexpr uint64_t maskLow = (overlappingElements <= 4u || overlappingElements >= 8u) ? uint64_t(-1) : (uint64_t(-1) >> (overlappingElements - 4u) * 2u * 8u);
				constexpr uint64_t maskHigh = overlappingElements >= 4u ? uint64_t(0) : (uint64_t(-1) >> overlappingElements * 2u * 8u);

				const uint16x8_t mask_u_16x8 = vcombine_u16(vcreate_u16(maskLow), vcreate_u16(maskHigh));

				const uint16x8_t patch_mean_low_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(patchLow0_1_s_16x8, mean0_1_s_16x8)); // |(patch0 - patch1) - (mean1 - mean0)|, with range [0, 255 * 255]
				const uint16x8_t patch_mean_high_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(patchHigh0_1_s_16x8, mean0_1_s_16x8)), mask_u_16x8);

				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patch_mean_low_u_16x8), vget_low_u16(patch_mean_low_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patch_mean_low_u_16x8), vget_high_u16(patch_mean_low_u_16x8));

				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patch_mean_high_u_16x8), vget_low_u16(patch_mean_high_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patch_mean_high_u_16x8), vget_high_u16(patch_mean_high_u_16x8));
			}
			else
			{
				const uint8x16_t patch0_u_8x16 = vld1q_u8(patch0 - overlappingElements);
				const uint8x16_t patch1_u_8x16 = vld1q_u8(patch1 - overlappingElements);

				const int16x8_t patchLow0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(patch0_u_8x16), vget_low_u8(patch1_u_8x16))); // low 8 bytes: patch0 - patch1
				const int16x8_t patchHigh0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(patch0_u_8x16), vget_high_u8(patch1_u_8x16))); // high 8 bytes: patch0 - patch1

				constexpr uint64_t maskLow = overlappingElements >= 4u ? uint64_t(0) : (uint64_t(-1) << overlappingElements * 2u * 8u);
				constexpr uint64_t maskHigh = (overlappingElements <= 4u || overlappingElements >= 8u) ? uint64_t(-1) : (uint64_t(-1) << (overlappingElements - 4u) * 2u * 8u);

				const uint16x8_t mask_u_16x8 = vcombine_u16(vcreate_u16(maskLow), vcreate_u16(maskHigh));

				const uint16x8_t patch_mean_low_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(patchLow0_1_s_16x8, mean0_1_s_16x8)), mask_u_16x8); // |(patch0 - patch1) - (mean1 - mean0)|, with range [0, 255 * 255]
				const uint16x8_t patch_mean_high_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(patchHigh0_1_s_16x8, mean0_1_s_16x8));

				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patch_mean_low_u_16x8), vget_low_u16(patch_mean_low_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patch_mean_low_u_16x8), vget_high_u16(patch_mean_low_u_16x8));

				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patch_mean_high_u_16x8), vget_low_u16(patch_mean_high_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patch_mean_high_u_16x8), vget_high_u16(patch_mean_high_u_16x8));
			}

			patch0 += remainingAfterBlocks16;
			patch1 += remainingAfterBlocks16;
		}

		for (unsigned int n = 0u; n < blocks8; ++n)
		{
			const uint8x8_t patch0_u_8x8 = vld1_u8(patch0);
			const uint8x8_t patch1_u_8x8 = vld1_u8(patch1);

			const int16x8_t patch0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(patch0_u_8x8, patch1_u_8x8)); // patch0 - patch1

			const uint16x8_t patch_mean_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(patch0_1_s_16x8, mean0_1_s_16x8)); // |(patch0 - patch1) - (mean1 - mean0)|, with range [0, 255 * 255]

			sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patch_mean_u_16x8), vget_low_u16(patch_mean_u_16x8));
			sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patch_mean_u_16x8), vget_high_u16(patch_mean_u_16x8));

			patch0 += 8;
			patch1 += 8;
		}

		if constexpr (partialBlock8)
		{
			constexpr unsigned int overlappingElements = 8u - remainingAfterBlocks8;
			ocean_assert(overlappingElements < 8u);

			if (y < tPatchSize - 1u)
			{
				const uint8x8_t patch0_u_8x8 = vld1_u8(patch0);
				const uint8x8_t patch1_u_8x8 = vld1_u8(patch1);

				const int16x8_t patch0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(patch0_u_8x8, patch1_u_8x8)); // patch0 - patch1

				constexpr uint64_t maskLow = (overlappingElements <= 4u || overlappingElements >= 8u) ? uint64_t(-1) : (uint64_t(-1) >> (overlappingElements - 4u) * 2u * 8u);
				constexpr uint64_t maskHigh = overlappingElements >= 4u ? uint64_t(0) : (uint64_t(-1) >> overlappingElements * 2u * 8u);

				const uint16x8_t mask_u_16x8 = vcombine_u16(vcreate_u16(maskLow), vcreate_u16(maskHigh));

				const uint16x8_t patch_mean_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(patch0_1_s_16x8, mean0_1_s_16x8)), mask_u_16x8); // |(patch0 - patch1) - (mean1 - mean0)|, with range [0, 255 * 255]

				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patch_mean_u_16x8), vget_low_u16(patch_mean_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patch_mean_u_16x8), vget_high_u16(patch_mean_u_16x8));
			}
			else
			{
				const uint8x8_t patch0_u_8x8 = vld1_u8(patch0 - overlappingElements);
				const uint8x8_t patch1_u_8x8 = vld1_u8(patch1 - overlappingElements);

				const int16x8_t patch0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(patch0_u_8x8, patch1_u_8x8)); // patch0 - patch1

				constexpr uint64_t maskLow = overlappingElements >= 4u ? uint64_t(0) : (uint64_t(-1) << overlappingElements * 2u * 8u);
				constexpr uint64_t maskHigh = (overlappingElements <= 4u || overlappingElements >= 8u) ? uint64_t(-1) : (uint64_t(-1) << (overlappingElements - 4u) * 2u * 8u);

				const uint16x8_t mask_u_16x8 = vcombine_u16(vcreate_u16(maskLow), vcreate_u16(maskHigh));

				const uint16x8_t patch_mean_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(patch0_1_s_16x8, mean0_1_s_16x8)), mask_u_16x8); // |(patch0 - patch1) - (mean1 - mean0)|, with range [0, 255 * 255]

				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patch_mean_u_16x8), vget_low_u16(patch_mean_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patch_mean_u_16x8), vget_high_u16(patch_mean_u_16x8));
			}

			patch0 += remainingAfterBlocks8;
			patch1 += remainingAfterBlocks8;
		}

		if constexpr (blocks1 != 0u)
		{
			for (unsigned int n = 0u; n < blocks1; ++n)
			{
				sumIndividual += sqrDistance(int16_t(patch0[n] - meanValues0[0]), int16_t(patch1[n] - meanValues1[0]));
			}

			patch0 += blocks1;
			patch1 += blocks1;
		}

		patch0 += patch0StrideElements - tPatchSize;
		patch1 += patch1StrideElements - tPatchSize;
	}

	const uint32x4_t sum_u_32x4 = vaddq_u32(sumA_u_32x4, sumB_u_32x4);

	uint32_t results[4];
	vst1q_u32(results, sum_u_32x4);

	return results[0] + results[1] + results[2] + results[3] + sumIndividual;
}

template <>
template <unsigned int tPatchSize>
inline uint32_t ZeroMeanSumSquareDifferencesNEON::SpecializedForChannels<3u>::patch8BitPerChannel(const uint8_t* patch0, const uint8_t* patch1, const unsigned int patch0StrideElements, const unsigned int patch1StrideElements, const uint8_t* const meanValues0, const uint8_t* const meanValues1)
{
	static_assert(tPatchSize >= 5u, "Invalid patch size!");

	constexpr unsigned int tChannels = 3u;

	ocean_assert(patch0 != nullptr && patch1 != nullptr);
	ocean_assert(meanValues0 != nullptr && meanValues1 != nullptr);

	ocean_assert(patch0StrideElements >= tChannels * tPatchSize);
	ocean_assert(patch1StrideElements >= tChannels * tPatchSize);

	constexpr unsigned int blocks16 = tPatchSize / 16u;
	constexpr unsigned int remainingAfterBlocks16 = tPatchSize % 16u;

	constexpr bool partialBlock16 = remainingAfterBlocks16 > 10u;
	constexpr unsigned int remainingAfterPartialBlock16 = partialBlock16 ? 0u : remainingAfterBlocks16;

	constexpr unsigned int blocks8 = remainingAfterPartialBlock16 / 8u;
	constexpr unsigned int remainingAfterBlocks8 = remainingAfterPartialBlock16 % 8u;

	constexpr bool partialBlock8 = remainingAfterBlocks8 >= 3u;
	constexpr unsigned int remainingAfterPartialBlock8 = partialBlock8 ? 0u : remainingAfterBlocks8;

	constexpr unsigned int blocks1 = remainingAfterPartialBlock8;

	static_assert(blocks1 <= 2u, "Invalid block size!");

	// [(patch0 - mean0) - (patch1 - mean1)]^2
	// [patch0 - patch1 - mean0 + mean1]^2

	const int16x8_t meanChannel0_0_1_s_16x8 = vdupq_n_s16(int16_t(meanValues0[0]) - int16_t(meanValues1[0]));
	const int16x8_t meanChannel1_0_1_s_16x8 = vdupq_n_s16(int16_t(meanValues0[1]) - int16_t(meanValues1[1]));
	const int16x8_t meanChannel2_0_1_s_16x8 = vdupq_n_s16(int16_t(meanValues0[2]) - int16_t(meanValues1[2]));

	uint32x4_t sumA_u_32x4 = vdupq_n_u32(0u);
	uint32x4_t sumB_u_32x4 = vdupq_n_u32(0u);

	uint32_t sumIndividual = 0u;

	for (unsigned int y = 0u; y < tPatchSize; ++y)
	{
		for (unsigned int n = 0u; n < blocks16; ++n)
		{
			const uint8x16x3_t patch0_u_8x16x3 = vld3q_u8(patch0);
			const uint8x16x3_t patch1_u_8x16x3 = vld3q_u8(patch1);

			const int16x8_t patchChannel0Low0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(patch0_u_8x16x3.val[0]), vget_low_u8(patch1_u_8x16x3.val[0]))); // low 8 bytes: patch0 - patch1
			const int16x8_t patchChannel0High0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(patch0_u_8x16x3.val[0]), vget_high_u8(patch1_u_8x16x3.val[0]))); // high 8 bytes: patch0 - patch1

			const int16x8_t patchChannel1Low0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(patch0_u_8x16x3.val[1]), vget_low_u8(patch1_u_8x16x3.val[1])));
			const int16x8_t patchChannel1High0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(patch0_u_8x16x3.val[1]), vget_high_u8(patch1_u_8x16x3.val[1])));

			const int16x8_t patchChannel2Low0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(patch0_u_8x16x3.val[2]), vget_low_u8(patch1_u_8x16x3.val[2])));
			const int16x8_t patchChannel2High0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(patch0_u_8x16x3.val[2]), vget_high_u8(patch1_u_8x16x3.val[2])));


			const uint16x8_t patchChannel0_mean_low_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(patchChannel0Low0_1_s_16x8, meanChannel0_0_1_s_16x8)); // |(patch0 - patch1) - (mean1 - mean0)|, with range [0, 255 * 255]
			const uint16x8_t patchChannel0_mean_high_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(patchChannel0High0_1_s_16x8, meanChannel0_0_1_s_16x8));

			const uint16x8_t patchChannel1_mean_low_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(patchChannel1Low0_1_s_16x8, meanChannel1_0_1_s_16x8));
			const uint16x8_t patchChannel1_mean_high_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(patchChannel1High0_1_s_16x8, meanChannel1_0_1_s_16x8));

			const uint16x8_t patchChannel2_mean_low_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(patchChannel2Low0_1_s_16x8, meanChannel2_0_1_s_16x8));
			const uint16x8_t patchChannel2_mean_high_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(patchChannel2High0_1_s_16x8, meanChannel2_0_1_s_16x8));


			sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel0_mean_low_u_16x8), vget_low_u16(patchChannel0_mean_low_u_16x8));
			sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel0_mean_low_u_16x8), vget_high_u16(patchChannel0_mean_low_u_16x8));
			sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel0_mean_high_u_16x8), vget_low_u16(patchChannel0_mean_high_u_16x8));
			sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel0_mean_high_u_16x8), vget_high_u16(patchChannel0_mean_high_u_16x8));

			sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel1_mean_low_u_16x8), vget_low_u16(patchChannel1_mean_low_u_16x8));
			sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel1_mean_low_u_16x8), vget_high_u16(patchChannel1_mean_low_u_16x8));
			sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel1_mean_high_u_16x8), vget_low_u16(patchChannel1_mean_high_u_16x8));
			sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel1_mean_high_u_16x8), vget_high_u16(patchChannel1_mean_high_u_16x8));

			sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel2_mean_low_u_16x8), vget_low_u16(patchChannel2_mean_low_u_16x8));
			sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel2_mean_low_u_16x8), vget_high_u16(patchChannel2_mean_low_u_16x8));
			sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel2_mean_high_u_16x8), vget_low_u16(patchChannel2_mean_high_u_16x8));
			sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel2_mean_high_u_16x8), vget_high_u16(patchChannel2_mean_high_u_16x8));


			patch0 += 16u * tChannels;
			patch1 += 16u * tChannels;
		}

		if constexpr (partialBlock16)
		{
			constexpr unsigned int overlappingElements = 16u - remainingAfterBlocks16;
			ocean_assert(overlappingElements < 8u);

			if (y < tPatchSize - 1u)
			{
				const uint8x16x3_t patch0_u_8x16x3 = vld3q_u8(patch0);
				const uint8x16x3_t patch1_u_8x16x3 = vld3q_u8(patch1);


				const int16x8_t patchChannel0Low0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(patch0_u_8x16x3.val[0]), vget_low_u8(patch1_u_8x16x3.val[0]))); // low 8 bytes: patch0 - patch1
				const int16x8_t patchChannel0High0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(patch0_u_8x16x3.val[0]), vget_high_u8(patch1_u_8x16x3.val[0]))); // high 8 bytes: patch0 - patch1

				const int16x8_t patchChannel1Low0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(patch0_u_8x16x3.val[1]), vget_low_u8(patch1_u_8x16x3.val[1])));
				const int16x8_t patchChannel1High0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(patch0_u_8x16x3.val[1]), vget_high_u8(patch1_u_8x16x3.val[1])));

				const int16x8_t patchChannel2Low0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(patch0_u_8x16x3.val[2]), vget_low_u8(patch1_u_8x16x3.val[2])));
				const int16x8_t patchChannel2High0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(patch0_u_8x16x3.val[2]), vget_high_u8(patch1_u_8x16x3.val[2])));


				// mask: |<- overlapping ->|<- remainingAfterBlocks16 ->|
				//        00 00 00 00 00 00 FF FF FF FF FF FF FF FF FF FF

				constexpr uint64_t maskLow = (overlappingElements <= 4u || overlappingElements >= 8u) ? uint64_t(-1) : (uint64_t(-1) >> (overlappingElements - 4u) * 2u * 8u);
				constexpr uint64_t maskHigh = overlappingElements >= 4u ? uint64_t(0) : (uint64_t(-1) >> overlappingElements * 2u * 8u);

				const uint16x8_t mask_u_16x8 = vcombine_u16(vcreate_u16(maskLow), vcreate_u16(maskHigh));


				const uint16x8_t patchChannel0_mean_low_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(patchChannel0Low0_1_s_16x8, meanChannel0_0_1_s_16x8)); // |(patch0 - patch1) - (mean1 - mean0)|, with range [0, 255 * 255]
				const uint16x8_t patchChannel0_mean_high_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(patchChannel0High0_1_s_16x8, meanChannel0_0_1_s_16x8)), mask_u_16x8);

				const uint16x8_t patchChannel1_mean_low_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(patchChannel1Low0_1_s_16x8, meanChannel1_0_1_s_16x8));
				const uint16x8_t patchChannel1_mean_high_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(patchChannel1High0_1_s_16x8, meanChannel1_0_1_s_16x8)), mask_u_16x8);

				const uint16x8_t patchChannel2_mean_low_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(patchChannel2Low0_1_s_16x8, meanChannel2_0_1_s_16x8));
				const uint16x8_t patchChannel2_mean_high_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(patchChannel2High0_1_s_16x8, meanChannel2_0_1_s_16x8)), mask_u_16x8);


				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel0_mean_low_u_16x8), vget_low_u16(patchChannel0_mean_low_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel0_mean_low_u_16x8), vget_high_u16(patchChannel0_mean_low_u_16x8));
				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel0_mean_high_u_16x8), vget_low_u16(patchChannel0_mean_high_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel0_mean_high_u_16x8), vget_high_u16(patchChannel0_mean_high_u_16x8));

				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel1_mean_low_u_16x8), vget_low_u16(patchChannel1_mean_low_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel1_mean_low_u_16x8), vget_high_u16(patchChannel1_mean_low_u_16x8));
				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel1_mean_high_u_16x8), vget_low_u16(patchChannel1_mean_high_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel1_mean_high_u_16x8), vget_high_u16(patchChannel1_mean_high_u_16x8));

				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel2_mean_low_u_16x8), vget_low_u16(patchChannel2_mean_low_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel2_mean_low_u_16x8), vget_high_u16(patchChannel2_mean_low_u_16x8));
				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel2_mean_high_u_16x8), vget_low_u16(patchChannel2_mean_high_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel2_mean_high_u_16x8), vget_high_u16(patchChannel2_mean_high_u_16x8));
			}
			else
			{
				const uint8x16x3_t patch0_u_8x16x3 = vld3q_u8(patch0 - overlappingElements * tChannels);
				const uint8x16x3_t patch1_u_8x16x3 = vld3q_u8(patch1 - overlappingElements * tChannels);


				const int16x8_t patchChannel0Low0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(patch0_u_8x16x3.val[0]), vget_low_u8(patch1_u_8x16x3.val[0]))); // low 8 bytes: patch0 - patch1
				const int16x8_t patchChannel0High0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(patch0_u_8x16x3.val[0]), vget_high_u8(patch1_u_8x16x3.val[0]))); // high 8 bytes: patch0 - patch1

				const int16x8_t patchChannel1Low0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(patch0_u_8x16x3.val[1]), vget_low_u8(patch1_u_8x16x3.val[1])));
				const int16x8_t patchChannel1High0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(patch0_u_8x16x3.val[1]), vget_high_u8(patch1_u_8x16x3.val[1])));

				const int16x8_t patchChannel2Low0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(patch0_u_8x16x3.val[2]), vget_low_u8(patch1_u_8x16x3.val[2])));
				const int16x8_t patchChannel2High0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(patch0_u_8x16x3.val[2]), vget_high_u8(patch1_u_8x16x3.val[2])));


				constexpr uint64_t maskLow = overlappingElements >= 4u ? uint64_t(0) : (uint64_t(-1) << overlappingElements * 2u * 8u);
				constexpr uint64_t maskHigh = (overlappingElements <= 4u || overlappingElements >= 8u) ? uint64_t(-1) : (uint64_t(-1) << (overlappingElements - 4u) * 2u * 8u);

				const uint16x8_t mask_u_16x8 = vcombine_u16(vcreate_u16(maskLow), vcreate_u16(maskHigh));


				const uint16x8_t patchChannel0_mean_low_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(patchChannel0Low0_1_s_16x8, meanChannel0_0_1_s_16x8)), mask_u_16x8); // |(patch0 - patch1) - (mean1 - mean0)|, with range [0, 255 * 255]
				const uint16x8_t patchChannel0_mean_high_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(patchChannel0High0_1_s_16x8, meanChannel0_0_1_s_16x8));

				const uint16x8_t patchChannel1_mean_low_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(patchChannel1Low0_1_s_16x8, meanChannel1_0_1_s_16x8)), mask_u_16x8);
				const uint16x8_t patchChannel1_mean_high_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(patchChannel1High0_1_s_16x8, meanChannel1_0_1_s_16x8));

				const uint16x8_t patchChannel2_mean_low_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(patchChannel2Low0_1_s_16x8, meanChannel2_0_1_s_16x8)), mask_u_16x8);
				const uint16x8_t patchChannel2_mean_high_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(patchChannel2High0_1_s_16x8, meanChannel2_0_1_s_16x8));


				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel0_mean_low_u_16x8), vget_low_u16(patchChannel0_mean_low_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel0_mean_low_u_16x8), vget_high_u16(patchChannel0_mean_low_u_16x8));
				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel0_mean_high_u_16x8), vget_low_u16(patchChannel0_mean_high_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel0_mean_high_u_16x8), vget_high_u16(patchChannel0_mean_high_u_16x8));

				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel1_mean_low_u_16x8), vget_low_u16(patchChannel1_mean_low_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel1_mean_low_u_16x8), vget_high_u16(patchChannel1_mean_low_u_16x8));
				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel1_mean_high_u_16x8), vget_low_u16(patchChannel1_mean_high_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel1_mean_high_u_16x8), vget_high_u16(patchChannel1_mean_high_u_16x8));

				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel2_mean_low_u_16x8), vget_low_u16(patchChannel2_mean_low_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel2_mean_low_u_16x8), vget_high_u16(patchChannel2_mean_low_u_16x8));
				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel2_mean_high_u_16x8), vget_low_u16(patchChannel2_mean_high_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel2_mean_high_u_16x8), vget_high_u16(patchChannel2_mean_high_u_16x8));
			}

			patch0 += remainingAfterBlocks16 * tChannels;
			patch1 += remainingAfterBlocks16 * tChannels;
		}

		for (unsigned int n = 0u; n < blocks8; ++n)
		{
			const uint8x8x3_t patch0_u_8x8x3 = vld3_u8(patch0);
			const uint8x8x3_t patch1_u_8x8x3 = vld3_u8(patch1);

			const int16x8_t patchChannel0_0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(patch0_u_8x8x3.val[0], patch1_u_8x8x3.val[0])); // patch0 - patch1
			const int16x8_t patchChannel1_0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(patch0_u_8x8x3.val[1], patch1_u_8x8x3.val[1]));
			const int16x8_t patchChannel2_0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(patch0_u_8x8x3.val[2], patch1_u_8x8x3.val[2]));

			const uint16x8_t patchChannel0_mean_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(patchChannel0_0_1_s_16x8, meanChannel0_0_1_s_16x8)); // |(patch0 - patch1) - (mean1 - mean0)|, with range [0, 255 * 255]
			const uint16x8_t patchChannel1_mean_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(patchChannel1_0_1_s_16x8, meanChannel1_0_1_s_16x8));
			const uint16x8_t patchChannel2_mean_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(patchChannel2_0_1_s_16x8, meanChannel2_0_1_s_16x8));

			sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel0_mean_u_16x8), vget_low_u16(patchChannel0_mean_u_16x8));
			sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel0_mean_u_16x8), vget_high_u16(patchChannel0_mean_u_16x8));

			sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel1_mean_u_16x8), vget_low_u16(patchChannel1_mean_u_16x8));
			sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel1_mean_u_16x8), vget_high_u16(patchChannel1_mean_u_16x8));

			sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel2_mean_u_16x8), vget_low_u16(patchChannel2_mean_u_16x8));
			sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel2_mean_u_16x8), vget_high_u16(patchChannel2_mean_u_16x8));

			patch0 += 8u * tChannels;
			patch1 += 8u * tChannels;
		}

		if constexpr (partialBlock8)
		{
			constexpr unsigned int overlappingElements = 8u - remainingAfterBlocks8;
			ocean_assert(overlappingElements < 8u);

			if (y < tPatchSize - 1u)
			{
				const uint8x8x3_t patch0_u_8x8x3 = vld3_u8(patch0);
				const uint8x8x3_t patch1_u_8x8x3 = vld3_u8(patch1);

				const int16x8_t patchChannel0_0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(patch0_u_8x8x3.val[0], patch1_u_8x8x3.val[0])); // patch0 - patch1
				const int16x8_t patchChannel1_0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(patch0_u_8x8x3.val[1], patch1_u_8x8x3.val[1]));
				const int16x8_t patchChannel2_0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(patch0_u_8x8x3.val[2], patch1_u_8x8x3.val[2]));

				constexpr uint64_t maskLow = (overlappingElements <= 4u || overlappingElements >= 8u) ? uint64_t(-1) : (uint64_t(-1) >> (overlappingElements - 4u) * 2u * 8u);
				constexpr uint64_t maskHigh = overlappingElements >= 4u ? uint64_t(0) : (uint64_t(-1) >> overlappingElements * 2u * 8u);

				const uint16x8_t mask_u_16x8 = vcombine_u16(vcreate_u16(maskLow), vcreate_u16(maskHigh));

				const uint16x8_t patchChannel0_mean_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(patchChannel0_0_1_s_16x8, meanChannel0_0_1_s_16x8)), mask_u_16x8); // |(patch0 - patch1) - (mean1 - mean0)|, with range [0, 255 * 255]
				const uint16x8_t patchChannel1_mean_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(patchChannel1_0_1_s_16x8, meanChannel1_0_1_s_16x8)), mask_u_16x8);
				const uint16x8_t patchChannel2_mean_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(patchChannel2_0_1_s_16x8, meanChannel2_0_1_s_16x8)), mask_u_16x8);

				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel0_mean_u_16x8), vget_low_u16(patchChannel0_mean_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel0_mean_u_16x8), vget_high_u16(patchChannel0_mean_u_16x8));

				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel1_mean_u_16x8), vget_low_u16(patchChannel1_mean_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel1_mean_u_16x8), vget_high_u16(patchChannel1_mean_u_16x8));

				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel2_mean_u_16x8), vget_low_u16(patchChannel2_mean_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel2_mean_u_16x8), vget_high_u16(patchChannel2_mean_u_16x8));
			}
			else
			{
				const uint8x8x3_t patch0_u_8x8x3 = vld3_u8(patch0 - overlappingElements * tChannels);
				const uint8x8x3_t patch1_u_8x8x3 = vld3_u8(patch1 - overlappingElements * tChannels);

				const int16x8_t patchChannel0_0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(patch0_u_8x8x3.val[0], patch1_u_8x8x3.val[0])); // patch0 - patch1
				const int16x8_t patchChannel1_0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(patch0_u_8x8x3.val[1], patch1_u_8x8x3.val[1]));
				const int16x8_t patchChannel2_0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(patch0_u_8x8x3.val[2], patch1_u_8x8x3.val[2]));

				constexpr uint64_t maskLow = overlappingElements >= 4u ? uint64_t(0) : (uint64_t(-1) << overlappingElements * 2u * 8u);
				constexpr uint64_t maskHigh = (overlappingElements <= 4u || overlappingElements >= 8u) ? uint64_t(-1) : (uint64_t(-1) << (overlappingElements - 4u) * 2u * 8u);

				const uint16x8_t mask_u_16x8 = vcombine_u16(vcreate_u16(maskLow), vcreate_u16(maskHigh));

				const uint16x8_t patchChannel0_mean_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(patchChannel0_0_1_s_16x8, meanChannel0_0_1_s_16x8)), mask_u_16x8); // |(patch0 - patch1) - (mean1 - mean0)|, with range [0, 255 * 255]
				const uint16x8_t patchChannel1_mean_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(patchChannel1_0_1_s_16x8, meanChannel1_0_1_s_16x8)), mask_u_16x8);
				const uint16x8_t patchChannel2_mean_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(patchChannel2_0_1_s_16x8, meanChannel2_0_1_s_16x8)), mask_u_16x8);

				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel0_mean_u_16x8), vget_low_u16(patchChannel0_mean_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel0_mean_u_16x8), vget_high_u16(patchChannel0_mean_u_16x8));

				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel1_mean_u_16x8), vget_low_u16(patchChannel1_mean_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel1_mean_u_16x8), vget_high_u16(patchChannel1_mean_u_16x8));

				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patchChannel2_mean_u_16x8), vget_low_u16(patchChannel2_mean_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patchChannel2_mean_u_16x8), vget_high_u16(patchChannel2_mean_u_16x8));
			}

			patch0 += remainingAfterBlocks8 * tChannels;
			patch1 += remainingAfterBlocks8 * tChannels;
		}

		if constexpr (blocks1 != 0u)
		{
			for (unsigned int n = 0u; n < blocks1; ++n)
			{
				for (unsigned int c = 0u; c < tChannels; ++c)
				{
					sumIndividual += sqrDistance(int16_t(patch0[n * tChannels + c] - meanValues0[c]), int16_t(patch1[n * tChannels + c] - meanValues1[c]));
				}
			}

			patch0 += blocks1 * tChannels;
			patch1 += blocks1 * tChannels;
		}

		patch0 += patch0StrideElements - tPatchSize * tChannels;
		patch1 += patch1StrideElements - tPatchSize * tChannels;
	}

	const uint32x4_t sum_u_32x4 = vaddq_u32(sumA_u_32x4, sumB_u_32x4);

	uint32_t results[4];
	vst1q_u32(results, sum_u_32x4);

	return results[0] + results[1] + results[2] + results[3] + sumIndividual;
}

template <unsigned int tChannels>
template <unsigned int tPatchSize>
inline uint32_t ZeroMeanSumSquareDifferencesNEON::SpecializedForChannels<tChannels>::patch8BitPerChannel(const uint8_t* patch0, const uint8_t* patch1, const unsigned int patch0StrideElements, const unsigned int patch1StrideElements, const uint8_t* const meanValues0, const uint8_t* const meanValues1)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize >= 1u, "Invalid patch size!");

	ocean_assert(patch0 != nullptr && patch1 != nullptr);
	ocean_assert(meanValues0 != nullptr && meanValues1 != nullptr);

	ocean_assert(patch0StrideElements >= tChannels * tPatchSize);
	ocean_assert(patch1StrideElements >= tChannels * tPatchSize);

	uint32_t zmssd = 0u;

	for (unsigned int y = 0u; y < tPatchSize; ++y)
	{
		for (unsigned int x = 0u; x < tPatchSize; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				zmssd += sqrDistance(patch0[x * tChannels + n] - meanValues0[n], patch1[x * tChannels + n] - meanValues1[n]);
			}
		}

		patch0 += patch0StrideElements;
		patch1 += patch1StrideElements;
	}

	return zmssd;
}

template <>
template <unsigned int tPatchSize>
inline uint32_t ZeroMeanSumSquareDifferencesNEON::SpecializedForChannels<1u>::patchMirroredBorder8BitPerChannel(const uint8_t* const image0, const uint8_t* const image1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements, const uint8_t* const meanValues0, const uint8_t* const meanValues1)
{
	static_assert(tPatchSize % 2u == 1u, "Invalid patch size!");
	static_assert(tPatchSize >= 5u, "Invalid patch size!");

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	ocean_assert(image0 != nullptr && image1 != nullptr);
	ocean_assert(meanValues0 != nullptr && meanValues1 != nullptr);

	ocean_assert(centerX0 < width0 && centerY0 < height0);
	ocean_assert(centerX1 < width1 && centerY1 < height1);

	const unsigned int image0StrideElements = width0 + image0PaddingElements;
	const unsigned int image1StrideElements = width1 + image1PaddingElements;

	constexpr unsigned int blocks16 = tPatchSize / 16u;
	constexpr unsigned int remainingAfterBlocks16 = tPatchSize % 16u;

	constexpr bool partialBlock16 = remainingAfterBlocks16 > 10u;
	constexpr unsigned int remainingAfterPartialBlock16 = partialBlock16 ? 0u : remainingAfterBlocks16;

	constexpr unsigned int blocks8 = remainingAfterPartialBlock16 / 8u;
	constexpr unsigned int remainingAfterBlocks8 = remainingAfterPartialBlock16 % 8u;

	constexpr bool partialBlock8 = remainingAfterBlocks8 >= 3u;
	constexpr unsigned int remainingAfterPartialBlock8 = partialBlock8 ? 0u : remainingAfterBlocks8;

	constexpr unsigned int blocks1 = remainingAfterPartialBlock8;

	static_assert(blocks1 <= 2u, "Invalid block size!");

	// [(patch0 - mean0) - (patch1 - mean1)]^2
	// [patch0 - patch1 - mean0 + mean1]^2

	const int16x8_t mean0_1_s_16x8 = vdupq_n_s16(int16_t(meanValues0[0]) - int16_t(meanValues1[0]));

	uint32x4_t sumA_u_32x4 = vdupq_n_u32(0u);
	uint32x4_t sumB_u_32x4 = vdupq_n_u32(0u);

	uint32_t sumIndividual = 0u;

	uint8_t intermediate[16];

	int y1 = int(centerY1) - int(tPatchSize_2);
	for (int y0 = int(centerY0) - int(tPatchSize_2); y0 <= int(centerY0) + int(tPatchSize_2); ++y0)
	{
		const uint8_t* const mirroredRow0 = image0 + CVUtilities::mirrorIndex(y0, height0) * image0StrideElements;
		const uint8_t* const mirroredRow1 = image1 + CVUtilities::mirrorIndex(y1, height1) * image1StrideElements;

		int x0 = int(centerX0) - int(tPatchSize_2);
		int x1 = int(centerX1) - int(tPatchSize_2);

		for (unsigned int n = 0u; n < blocks16; ++n)
		{
			const uint8x16_t patch0_u_8x16 = loadMirrored_u_8x16<true, 16u, true>(mirroredRow0, x0, width0, intermediate);
			const uint8x16_t patch1_u_8x16 = loadMirrored_u_8x16<true, 16u, true>(mirroredRow1, x1, width1, intermediate);

			const int16x8_t patchLow0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(patch0_u_8x16), vget_low_u8(patch1_u_8x16))); // low 8 bytes: patch0 - patch1
			const int16x8_t patchHigh0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(patch0_u_8x16), vget_high_u8(patch1_u_8x16))); // high 8 bytes: patch0 - patch1

			const uint16x8_t patch_mean_low_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(patchLow0_1_s_16x8, mean0_1_s_16x8)); // |(patch0 - patch1) - (mean1 - mean0)|, with range [0, 255 * 255]
			const uint16x8_t patch_mean_high_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(patchHigh0_1_s_16x8, mean0_1_s_16x8));

			sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patch_mean_low_u_16x8), vget_low_u16(patch_mean_low_u_16x8));
			sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patch_mean_low_u_16x8), vget_high_u16(patch_mean_low_u_16x8));

			sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patch_mean_high_u_16x8), vget_low_u16(patch_mean_high_u_16x8));
			sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patch_mean_high_u_16x8), vget_high_u16(patch_mean_high_u_16x8));

			x0 += 16;
			x1 += 16;
		}

		if constexpr (partialBlock16)
		{
			constexpr unsigned int overlappingElements = 16u - remainingAfterBlocks16;
			ocean_assert(overlappingElements < 8u);

			if (y0 < int(centerY0) + int(tPatchSize_2))
			{
				const uint8x16_t patch0_u_8x16 = loadMirrored_u_8x16<true, remainingAfterBlocks16, false>(mirroredRow0, x0, width0, intermediate);
				const uint8x16_t patch1_u_8x16 = loadMirrored_u_8x16<true, remainingAfterBlocks16, false>(mirroredRow1, x1, width1, intermediate);

				const int16x8_t patchLow0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(patch0_u_8x16), vget_low_u8(patch1_u_8x16))); // low 8 bytes: patch0 - patch1
				const int16x8_t patchHigh0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(patch0_u_8x16), vget_high_u8(patch1_u_8x16))); // high 8 bytes: patch0 - patch1

				// mask: |<- overlapping ->|<- remainingAfterBlocks16 ->|
				//        00 00 00 00 00 00 FF FF FF FF FF FF FF FF FF FF

				constexpr uint64_t maskLow = (overlappingElements <= 4u || overlappingElements >= 8u) ? uint64_t(-1) : (uint64_t(-1) >> (overlappingElements - 4u) * 2u * 8u);
				constexpr uint64_t maskHigh = overlappingElements >= 4u ? uint64_t(0) : (uint64_t(-1) >> overlappingElements * 2u * 8u);

				const uint16x8_t mask_u_16x8 = vcombine_u16(vcreate_u16(maskLow), vcreate_u16(maskHigh));

				const uint16x8_t patch_mean_low_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(patchLow0_1_s_16x8, mean0_1_s_16x8)); // |(patch0 - patch1) - (mean1 - mean0)|, with range [0, 255 * 255]
				const uint16x8_t patch_mean_high_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(patchHigh0_1_s_16x8, mean0_1_s_16x8)), mask_u_16x8);

				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patch_mean_low_u_16x8), vget_low_u16(patch_mean_low_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patch_mean_low_u_16x8), vget_high_u16(patch_mean_low_u_16x8));

				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patch_mean_high_u_16x8), vget_low_u16(patch_mean_high_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patch_mean_high_u_16x8), vget_high_u16(patch_mean_high_u_16x8));
			}
			else
			{
				const uint8x16_t patch0_u_8x16 = loadMirrored_u_8x16<false, remainingAfterBlocks16, false>(mirroredRow0, x0, width0, intermediate);
				const uint8x16_t patch1_u_8x16 = loadMirrored_u_8x16<false, remainingAfterBlocks16, false>(mirroredRow1, x1, width1, intermediate);

				const int16x8_t patchLow0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(patch0_u_8x16), vget_low_u8(patch1_u_8x16))); // low 8 bytes: patch0 - patch1
				const int16x8_t patchHigh0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(patch0_u_8x16), vget_high_u8(patch1_u_8x16))); // high 8 bytes: patch0 - patch1

				constexpr uint64_t maskLow = overlappingElements >= 4u ? uint64_t(0) : (uint64_t(-1) << overlappingElements * 2u * 8u);
				constexpr uint64_t maskHigh = (overlappingElements <= 4u || overlappingElements >= 8u) ? uint64_t(-1) : (uint64_t(-1) << (overlappingElements - 4u) * 2u * 8u);

				const uint16x8_t mask_u_16x8 = vcombine_u16(vcreate_u16(maskLow), vcreate_u16(maskHigh));

				const uint16x8_t patch_mean_low_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(patchLow0_1_s_16x8, mean0_1_s_16x8)), mask_u_16x8); // |(patch0 - patch1) - (mean1 - mean0)|, with range [0, 255 * 255]
				const uint16x8_t patch_mean_high_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(patchHigh0_1_s_16x8, mean0_1_s_16x8));

				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patch_mean_low_u_16x8), vget_low_u16(patch_mean_low_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patch_mean_low_u_16x8), vget_high_u16(patch_mean_low_u_16x8));

				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patch_mean_high_u_16x8), vget_low_u16(patch_mean_high_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patch_mean_high_u_16x8), vget_high_u16(patch_mean_high_u_16x8));
			}

			x0 += remainingAfterBlocks16;
			x1 += remainingAfterBlocks16;
		}

		for (unsigned int n = 0u; n < blocks8; ++n)
		{
			const uint8x8_t patch0_u_8x8 = loadMirrored_u_8x8<true, 8u, true>(mirroredRow0, x0, width0, intermediate);
			const uint8x8_t patch1_u_8x8 = loadMirrored_u_8x8<true, 8u, true>(mirroredRow1, x1, width1, intermediate);

			const int16x8_t patch0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(patch0_u_8x8, patch1_u_8x8)); // patch0 - patch1

			const uint16x8_t patch_mean_u_16x8 = vreinterpretq_u16_s16(vabdq_s16(patch0_1_s_16x8, mean0_1_s_16x8)); // |(patch0 - patch1) - (mean1 - mean0)|, with range [0, 255 * 255]

			sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patch_mean_u_16x8), vget_low_u16(patch_mean_u_16x8));
			sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patch_mean_u_16x8), vget_high_u16(patch_mean_u_16x8));

			x0 += 8;
			x1 += 8;
		}

		if constexpr (partialBlock8)
		{
			constexpr unsigned int overlappingElements = 8u - remainingAfterBlocks8;
			ocean_assert(overlappingElements < 8u);

			if (y0 < int(centerY0) + int(tPatchSize_2))
			{
				const uint8x8_t patch0_u_8x8 = loadMirrored_u_8x8<true, remainingAfterBlocks8, false>(mirroredRow0, x0, width0, intermediate);
				const uint8x8_t patch1_u_8x8 = loadMirrored_u_8x8<true, remainingAfterBlocks8, false>(mirroredRow1, x1, width1, intermediate);

				const int16x8_t patch0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(patch0_u_8x8, patch1_u_8x8)); // patch0 - patch1

				constexpr uint64_t maskLow = (overlappingElements <= 4u || overlappingElements >= 8u) ? uint64_t(-1) : (uint64_t(-1) >> (overlappingElements - 4u) * 2u * 8u);
				constexpr uint64_t maskHigh = overlappingElements >= 4u ? uint64_t(0) : (uint64_t(-1) >> overlappingElements * 2u * 8u);

				const uint16x8_t mask_u_16x8 = vcombine_u16(vcreate_u16(maskLow), vcreate_u16(maskHigh));

				const uint16x8_t patch_mean_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(patch0_1_s_16x8, mean0_1_s_16x8)), mask_u_16x8); // |(patch0 - patch1) - (mean1 - mean0)|, with range [0, 255 * 255]

				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patch_mean_u_16x8), vget_low_u16(patch_mean_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patch_mean_u_16x8), vget_high_u16(patch_mean_u_16x8));
			}
			else
			{
				const uint8x8_t patch0_u_8x8 = loadMirrored_u_8x8<false, remainingAfterBlocks8, false>(mirroredRow0, x0, width0, intermediate);
				const uint8x8_t patch1_u_8x8 = loadMirrored_u_8x8<false, remainingAfterBlocks8, false>(mirroredRow1, x1, width1, intermediate);

				const int16x8_t patch0_1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(patch0_u_8x8, patch1_u_8x8)); // patch0 - patch1

				constexpr uint64_t maskLow = overlappingElements >= 4u ? uint64_t(0) : (uint64_t(-1) << overlappingElements * 2u * 8u);
				constexpr uint64_t maskHigh = (overlappingElements <= 4u || overlappingElements >= 8u) ? uint64_t(-1) : (uint64_t(-1) << (overlappingElements - 4u) * 2u * 8u);

				const uint16x8_t mask_u_16x8 = vcombine_u16(vcreate_u16(maskLow), vcreate_u16(maskHigh));

				const uint16x8_t patch_mean_u_16x8 = vandq_u16(vreinterpretq_u16_s16(vabdq_s16(patch0_1_s_16x8, mean0_1_s_16x8)), mask_u_16x8); // |(patch0 - patch1) - (mean1 - mean0)|, with range [0, 255 * 255]

				sumA_u_32x4 = vmlal_u16(sumA_u_32x4, vget_low_u16(patch_mean_u_16x8), vget_low_u16(patch_mean_u_16x8));
				sumB_u_32x4 = vmlal_u16(sumB_u_32x4, vget_high_u16(patch_mean_u_16x8), vget_high_u16(patch_mean_u_16x8));
			}

			x0 += remainingAfterBlocks8;
			x1 += remainingAfterBlocks8;
		}

		if constexpr (blocks1 != 0u)
		{
			for (unsigned int n = 0u; n < blocks1; ++n)
			{
				const unsigned int index0 = CVUtilities::mirrorIndex(x0 + int(n), width0);
				const unsigned int index1 = CVUtilities::mirrorIndex(x1 + int(n), width1);

				sumIndividual += sqrDistance(int16_t(mirroredRow0[index0] - meanValues0[0]), int16_t(mirroredRow1[index1] - meanValues1[0]));
			}
		}

		++y1;
	}

	const uint32x4_t sum_u_32x4 = vaddq_u32(sumA_u_32x4, sumB_u_32x4);

	uint32_t results[4];
	vst1q_u32(results, sum_u_32x4);

	return results[0] + results[1] + results[2] + results[3] + sumIndividual;
}

template <unsigned int tChannels>
template <unsigned int tPatchSize>
inline uint32_t ZeroMeanSumSquareDifferencesNEON::SpecializedForChannels<tChannels>::patchMirroredBorder8BitPerChannel(const uint8_t* const image0, const uint8_t* const image1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements, const uint8_t* const meanValues0, const uint8_t* const meanValues1)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize % 2u == 1u, "Invalid patch size!");

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	ocean_assert(image0 != nullptr && image1 != nullptr);
	ocean_assert(meanValues0 != nullptr && meanValues1 != nullptr);

	ocean_assert(centerX0 < width0 && centerY0 < height0);
	ocean_assert(centerX1 < width1 && centerY1 < height1);

	const unsigned int image0StrideElements = width0 * tChannels + image0PaddingElements;
	const unsigned int image1StrideElements = width1 * tChannels + image1PaddingElements;

	uint32_t zmssd = 0u;

	int y1 = int(centerY1) - int(tPatchSize_2);
	for (int y0 = int(centerY0) - int(tPatchSize_2); y0 <= int(centerY0) + int(tPatchSize_2); ++y0)
	{
		const uint8_t* const mirroredRow0 = image0 + CVUtilities::mirrorIndex(y0, height0) * image0StrideElements;
		const uint8_t* const mirroredRow1 = image1 + CVUtilities::mirrorIndex(y1, height1) * image1StrideElements;

		int x1 = int(centerX1) - int(tPatchSize_2);
		for (int x0 = int(centerX0) - int(tPatchSize_2); x0 <= int(centerX0) + int(tPatchSize_2); ++x0)
		{
			const uint8_t* const pixel0 = mirroredRow0 + CVUtilities::mirrorIndex(x0, width0) * tChannels;
			const uint8_t* const pixel1 = mirroredRow1 + CVUtilities::mirrorIndex(x1, width1) * tChannels;

			for (unsigned int c = 0u; c < tChannels; ++c)
			{
				zmssd += sqrDistance(pixel0[c] - meanValues0[c], pixel1[c] - meanValues1[c]);
			}

			++x1;
		}

		++y1;
	}

	return zmssd;
}

template <unsigned int tChannels, unsigned int tPixels>
inline uint32_t ZeroMeanSumSquareDifferencesNEON::buffer8BitPerChannel(const uint8_t* const buffer0, const uint8_t* const buffer1)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPixels >= 8u, "Invalid pixels!");

	ocean_assert(buffer0 != nullptr && buffer1 != nullptr);

	uint8_t meanValues0[tChannels];
	mean8BitPerChannel<tChannels, tPixels>(buffer0, meanValues0);

	uint8_t meanValues1[tChannels];
	mean8BitPerChannel<tChannels, tPixels>(buffer1, meanValues1);

	return SpecializedForChannels<tChannels>::template buffer8BitPerChannel<tPixels>(buffer0, buffer1, meanValues0, meanValues1);
}

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t ZeroMeanSumSquareDifferencesNEON::patch8BitPerChannel(const uint8_t* const patch0, const uint8_t* const patch1, const unsigned int patch0StrideElements, const unsigned int patch1StrideElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize >= 5u, "Invalid patch size!");

	ocean_assert(patch0 != nullptr && patch1 != nullptr);

	ocean_assert(patch0StrideElements >= tChannels * tPatchSize);
	ocean_assert(patch1StrideElements >= tChannels * tPatchSize);

	uint8_t meanValues0[tChannels];
	mean8BitPerChannel<tChannels, tPatchSize>(patch0, patch0StrideElements, meanValues0);

	uint8_t meanValues1[tChannels];
	mean8BitPerChannel<tChannels, tPatchSize>(patch1, patch1StrideElements, meanValues1);

	return SpecializedForChannels<tChannels>::template patch8BitPerChannel<tPatchSize>(patch0, patch1, patch0StrideElements, patch1StrideElements, meanValues0, meanValues1);
}

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t ZeroMeanSumSquareDifferencesNEON::patchBuffer8BitPerChannel(const uint8_t* patch0, const uint8_t* buffer1, const unsigned int patch0StrideElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize >= 5u, "Invalid patch size!");

	ocean_assert(patch0 != nullptr && buffer1 != nullptr);

	ocean_assert(patch0StrideElements >= tChannels * tPatchSize);

	uint8_t meanValues0[tChannels];
	mean8BitPerChannel<tChannels, tPatchSize>(patch0, patch0StrideElements, meanValues0);

	uint8_t meanValues1[tChannels];
	mean8BitPerChannel<tChannels, tPatchSize * tPatchSize>(buffer1, meanValues1);

	constexpr unsigned int patch1StrideElements = tChannels * tPatchSize;

	return SpecializedForChannels<tChannels>::template patch8BitPerChannel<tPatchSize>(patch0, buffer1, patch0StrideElements, patch1StrideElements, meanValues0, meanValues1);
}

template <unsigned int tChannels, unsigned int tPatchSize>
uint32_t ZeroMeanSumSquareDifferencesNEON::patchMirroredBorder8BitPerChannel(const uint8_t* const image0, const uint8_t* const image1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize >= 5u, "Invalid patch size!");

	ocean_assert(image0 != nullptr && image1 != nullptr);

	uint8_t meanValues0[tChannels];
	SpecializedForChannels<tChannels>::template mean8BitPerChannelMirroredBorder<tPatchSize>(image0, width0, height0, centerX0, centerY0, image0PaddingElements, meanValues0);

	uint8_t meanValues1[tChannels];
	SpecializedForChannels<tChannels>::template mean8BitPerChannelMirroredBorder<tPatchSize>(image1, width1, height1, centerX1, centerY1, image1PaddingElements, meanValues1);

	return SpecializedForChannels<tChannels>::template patchMirroredBorder8BitPerChannel<tPatchSize>(image0, image1, width0, height0, width1, height1, centerX0, centerY0, centerX1, centerY1, image0PaddingElements, image1PaddingElements, meanValues0, meanValues1);
}

template <unsigned int tChannels, unsigned int tPixels>
OCEAN_FORCE_INLINE void ZeroMeanSumSquareDifferencesNEON::mean8BitPerChannel(const uint8_t* const buffer, uint8_t* const meanValues)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPixels >= 8u, "Invalid patch size!");

	SpecializedForChannels<tChannels>::template mean8BitPerChannel<tPixels>(buffer, meanValues);
}

template <unsigned int tChannels, unsigned int tPatchSize>
OCEAN_FORCE_INLINE void ZeroMeanSumSquareDifferencesNEON::mean8BitPerChannel(const uint8_t* patch, const unsigned int patchStrideElements, uint8_t* const meanValues)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize >= 5u, "Invalid patch size!");

	SpecializedForChannels<tChannels>::template mean8BitPerChannel<tPatchSize>(patch, patchStrideElements, meanValues);
}

template <bool tFront, unsigned int tPixels, bool tOverlappingToZero>
OCEAN_FORCE_INLINE uint8x8_t ZeroMeanSumSquareDifferencesNEON::loadMirrored_u_8x8(const uint8_t* const row, const int x, const unsigned int width, uint8_t* const intermediateBuffer)
{
	ocean_assert(tPixels >= 1u && tPixels <= 8u);

	ocean_assert(row != nullptr && intermediateBuffer != nullptr);

	constexpr unsigned int tOverlappingElements = 8u - tPixels;

	if (x >= 0 && x <= int(width) - int(tPixels))
	{
		if constexpr (tPixels == 8u)
		{
			return vld1_u8(row + x);
		}
		else
		{
			if constexpr (tFront)
			{
				if constexpr (tOverlappingToZero)
				{
					constexpr uint64_t mask = tOverlappingElements < 8u ? (uint64_t(-1) >> tOverlappingElements * 8u) : uint64_t(0);
					const uint8x8_t mask_u_8x8 = vcreate_u8(mask);

					return vand_u8(vld1_u8(row + x), mask_u_8x8);
				}
				else
				{
					return vld1_u8(row + x);
				}
			}
			else
			{
				if constexpr (tOverlappingToZero)
				{
					constexpr uint64_t mask = tOverlappingElements < 8u ? (uint64_t(-1) << tOverlappingElements * 8u) : uint64_t(0);
					const uint8x8_t mask_u_8x8 = vcreate_u8(mask);

					return vand_u8(vld1_u8(row + x - int(tOverlappingElements)), mask_u_8x8);
				}
				else
				{
					return vld1_u8(row + x - int(tOverlappingElements));
				}
			}
		}
	}

	if constexpr (tFront)
	{
		for (unsigned int n = 0u; n < tPixels; ++n)
		{
			const unsigned int mirroredIndex = CVUtilities::mirrorIndex(x + int(n), width);
			ocean_assert(mirroredIndex < width);

			intermediateBuffer[n] = row[mirroredIndex];
		}

		if constexpr (tOverlappingToZero)
		{
			for (unsigned int n = tPixels; n < 8u; ++n)
			{
				intermediateBuffer[n] = 0u;
			}
		}
	}
	else
	{
		if constexpr (tOverlappingToZero)
		{
			for (unsigned int n = 0u; n < tOverlappingElements; ++n)
			{
				intermediateBuffer[n] = 0u;
			}
		}

		for (unsigned int n = 0u; n < tPixels; ++n)
		{
			const unsigned int mirroredIndex = CVUtilities::mirrorIndex(x + int(n), width);
			ocean_assert(mirroredIndex < width);

			intermediateBuffer[tOverlappingElements + n] = row[mirroredIndex];
		}
	}

	return vld1_u8(intermediateBuffer);
}

template <bool tFront, unsigned int tPixels, bool tOverlappingToZero>
OCEAN_FORCE_INLINE uint8x16_t ZeroMeanSumSquareDifferencesNEON::loadMirrored_u_8x16(const uint8_t* const row, const int x, const unsigned int width, uint8_t* const intermediateBuffer)
{
	ocean_assert(tPixels > 8u && tPixels <= 16u);

	ocean_assert(row != nullptr && intermediateBuffer != nullptr);

	constexpr unsigned int tOverlappingElements = 16u - tPixels;

	if (x >= 0 && x <= int(width) - int(tPixels))
	{
		if constexpr (tPixels == 16u)
		{
			return vld1q_u8(row + x);
		}
		else
		{
			if constexpr (tFront)
			{
				if constexpr (tOverlappingToZero)
				{
					constexpr uint64_t maskHigh = tOverlappingElements < 8u ? (uint64_t(-1) >> tOverlappingElements * 8u) : uint64_t(0);
					const uint8x16_t mask_u_8x16 = vcombine_u8(vdup_n_u8(uint8_t(0xFFu)), vcreate_u8(maskHigh));

					return vandq_u8(vld1q_u8(row + x), mask_u_8x16);
				}
				else
				{
					return vld1q_u8(row + x);
				}
			}
			else
			{
				if constexpr (tOverlappingToZero)
				{
					constexpr uint64_t maskLow = tOverlappingElements < 8u ? (uint64_t(-1) << tOverlappingElements * 8u) : uint64_t(0);
					const uint8x16_t mask_u_8x16 = vcombine_u8(vcreate_u8(maskLow), vdup_n_u8(uint8_t(0xFFu)));

					return vandq_u8(vld1q_u8(row + x - int(tOverlappingElements)), mask_u_8x16);
				}
				else
				{
					return vld1q_u8(row + x - int(tOverlappingElements));
				}
			}
		}
	}

	if constexpr (tFront)
	{
		for (unsigned int n = 0u; n < tPixels; ++n)
		{
			const unsigned int mirroredIndex = CVUtilities::mirrorIndex(x + int(n), width);
			ocean_assert(mirroredIndex < width);

			intermediateBuffer[n] = row[mirroredIndex];
		}

		if constexpr (tOverlappingToZero)
		{
			for (unsigned int n = tPixels; n < 16u; ++n)
			{
				intermediateBuffer[n] = 0u;
			}
		}
	}
	else
	{
		if constexpr (tOverlappingToZero)
		{
			for (unsigned int n = 0u; n < tOverlappingElements; ++n)
			{
				intermediateBuffer[n] = 0u;
			}
		}

		for (unsigned int n = 0u; n < tPixels; ++n)
		{
			const unsigned int mirroredIndex = CVUtilities::mirrorIndex(x + int(n), width);
			ocean_assert(mirroredIndex < width);

			intermediateBuffer[tOverlappingElements + n] = row[mirroredIndex];
		}
	}

	return vld1q_u8(intermediateBuffer);
}

}

}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

#endif // META_OCEAN_CV_ZERO_MEAN_SUM_SQUARE_DIFFERENCES_NEON_H
