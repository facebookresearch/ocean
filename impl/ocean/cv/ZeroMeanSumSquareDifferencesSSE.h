/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ZERO_MEAN_SUM_SQUARE_DIFFERENCES_SSE_H
#define META_OCEAN_CV_ZERO_MEAN_SUM_SQUARE_DIFFERENCES_SSE_H

#include "ocean/cv/CV.h"

#include "ocean/base/Utilities.h"

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

#include "ocean/cv/SSE.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements function to calculate zeao-mean sum square differences using SSE instructions.
 * @ingroup cv
 */
class ZeroMeanSumSquareDifferencesSSE
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
				 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
				 */
				template <unsigned int tPatchSize>
				static inline void mean8BitPerChannel(const uint8_t* patch, const unsigned int patchStrideElements, uint8_t* const meanValues);

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
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [5, infinity), must be odd
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
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [5, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static inline uint32_t patchBuffer8BitPerChannel(const uint8_t* const patch0, const uint8_t* const buffer1, const unsigned int patch0StrideElements);

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
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [5, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static OCEAN_FORCE_INLINE void mean8BitPerChannel(const uint8_t* const patch, const unsigned int patchStrideElements, uint8_t* const meanValues);
};

template <>
template <unsigned int tPixels>
inline void ZeroMeanSumSquareDifferencesSSE::SpecializedForChannels<1u>::mean8BitPerChannel(const uint8_t* buffer, uint8_t* const meanValues)
{
	static_assert(tPixels >= 8u, "Invalid buffer size!");

	constexpr unsigned int tChannels = 1u;

	ocean_assert(buffer != nullptr && meanValues != nullptr);

	constexpr unsigned int bufferElements = tChannels * tPixels;

	constexpr unsigned int blocks16 = bufferElements / 16u;
	constexpr unsigned int remainingAfterBlocks16 = bufferElements % 16u;

	constexpr bool partialBlock16 = remainingAfterBlocks16 > 8u;

	constexpr bool fullBlock8 = !partialBlock16 && remainingAfterBlocks16 == 8u;

	constexpr bool partialBlock8 = !partialBlock16 && !fullBlock8 && remainingAfterBlocks16 >= 3u;

	constexpr unsigned int blocks1 = (!partialBlock16 && !fullBlock8 && !partialBlock8) ? remainingAfterBlocks16 : 0u;

	static_assert(blocks1 <= 2u, "Invalid block size!");

	__m128i sum_128i = _mm_setzero_si128();

	uint32_t sumIndividual = 0u;

	for (unsigned int n = 0u; n < blocks16; ++n)
	{
		const __m128i buffer_128i = _mm_lddqu_si128((const __m128i*)buffer);

		sum_128i = _mm_add_epi32(sum_128i, _mm_sad_epu8(buffer_128i, _mm_setzero_si128()));

		buffer += 16;
	}

	if constexpr (partialBlock16)
	{
		constexpr unsigned int overlapElements = partialBlock16 ? 16u - remainingAfterBlocks16 : 0u;

		static_assert(overlapElements < 8u, "Invalid value!");

		const __m128i buffer_128i = _mm_srli_si128(_mm_lddqu_si128((const __m128i*)(buffer - overlapElements)), overlapElements); // loading 16 elements, but shifting `overlapElements` zeros to the right

		sum_128i = _mm_add_epi32(sum_128i, _mm_sad_epu8(buffer_128i, _mm_setzero_si128()));

		buffer += remainingAfterBlocks16;
	}

	if constexpr (fullBlock8)
	{
		const __m128i buffer_128i = _mm_loadl_epi64((const __m128i*)buffer); // load for unaligned 64 bit memory

		sum_128i = _mm_add_epi32(sum_128i, _mm_sad_epu8(buffer_128i, _mm_setzero_si128()));

		buffer += 8;
	}

	if constexpr (partialBlock8)
	{
		constexpr unsigned int overlapElements = partialBlock8 ? 8u - remainingAfterBlocks16 : 0u;

		static_assert(overlapElements < 8u, "Invalid value!");

		const __m128i buffer_128i = _mm_srli_si128(_mm_loadl_epi64((const __m128i*)(buffer - overlapElements)), overlapElements); // loading 8 elements, but shifting `overlapElements` zeros to the right

		sum_128i = _mm_add_epi32(sum_128i, _mm_sad_epu8(buffer_128i, _mm_setzero_si128()));

		buffer += remainingAfterBlocks16;
	}

	if constexpr (blocks1 != 0u)
	{
		for (unsigned int n = 0u; n < blocks1; ++n)
		{
			sumIndividual += buffer[n];
		}
	}

	const uint32_t sum = SSE::sum_u32_first_third(sum_128i) + sumIndividual;

	meanValues[0] = uint8_t((sum + tPixels / 2u) / tPixels);
}

template <>
template <unsigned int tPixels>
inline void ZeroMeanSumSquareDifferencesSSE::SpecializedForChannels<3u>::mean8BitPerChannel(const uint8_t* buffer, uint8_t* const meanValues)
{
	static_assert(tPixels >= 8u, "Invalid buffer size!");

	constexpr unsigned int tChannels = 3u;

	ocean_assert(buffer != nullptr && meanValues != nullptr);

	constexpr unsigned int bufferElements = tChannels * tPixels;

	constexpr unsigned int blocks48 = bufferElements / 48u;
	constexpr unsigned int remainingAfterFullBlocks48 = bufferElements % 48u;

	constexpr bool partialBlock48 = remainingAfterFullBlocks48 > 2u * 16u;

	constexpr unsigned int remainingAfterPartialBlock48 = partialBlock48 ? 0u : remainingAfterFullBlocks48;

	constexpr unsigned int blocks24 = remainingAfterPartialBlock48 / 24u;

	constexpr unsigned int remainingAfterPartialBlock24 = remainingAfterPartialBlock48 % 24u;

	constexpr unsigned int blocks21 = remainingAfterPartialBlock24 / 21u;

	constexpr unsigned int remainingAfterPartialBlock21 = remainingAfterPartialBlock24 % 21u;

	constexpr unsigned int blocks15 = remainingAfterPartialBlock21 / 15u;

	constexpr unsigned int remainingAfterPartialBlock15 = remainingAfterPartialBlock21 % 15u;

	constexpr unsigned int blocks1 = remainingAfterPartialBlock15;

	static_assert(blocks1 % 3u == 0u, "Invalid number of single blocks");

	__m128i sumChannel0_128i = _mm_setzero_si128();
	__m128i sumChannel1_128i = _mm_setzero_si128();
	__m128i sumChannel2_128i = _mm_setzero_si128();

	uint32_t sumIndividual[3] = {0u};

	for (unsigned int n = 0u; n < blocks48; ++n)
	{
		const __m128i bufferA_128i = _mm_lddqu_si128((const __m128i*)(buffer + 0));
		const __m128i bufferB_128i = _mm_lddqu_si128((const __m128i*)(buffer + 16));
		const __m128i bufferC_128i = _mm_lddqu_si128((const __m128i*)(buffer + 32));

		__m128i channel0;
		__m128i channel1;
		__m128i channel2;
		SSE::deInterleave3Channel8Bit48Elements(bufferA_128i, bufferB_128i, bufferC_128i, channel0, channel1, channel2);

		sumChannel0_128i = _mm_add_epi32(sumChannel0_128i, _mm_sad_epu8(channel0, _mm_setzero_si128()));
		sumChannel1_128i = _mm_add_epi32(sumChannel1_128i, _mm_sad_epu8(channel1, _mm_setzero_si128()));
		sumChannel2_128i = _mm_add_epi32(sumChannel2_128i, _mm_sad_epu8(channel2, _mm_setzero_si128()));

		buffer += 48;
	}

	if constexpr (partialBlock48)
	{
		constexpr int overlappingElements = int(48u - remainingAfterFullBlocks48);

		const __m128i bufferA_128i = _mm_slli_si128(_mm_lddqu_si128((const __m128i*)(buffer)), overlappingElements);
		const __m128i bufferB_128i = _mm_lddqu_si128((const __m128i*)(buffer - overlappingElements + 16));
		const __m128i bufferC_128i = _mm_lddqu_si128((const __m128i*)(buffer - overlappingElements + 32));

		__m128i channel0;
		__m128i channel1;
		__m128i channel2;
		SSE::deInterleave3Channel8Bit48Elements(bufferA_128i, bufferB_128i, bufferC_128i, channel0, channel1, channel2);

		sumChannel0_128i = _mm_add_epi32(sumChannel0_128i, _mm_sad_epu8(channel0, _mm_setzero_si128()));
		sumChannel1_128i = _mm_add_epi32(sumChannel1_128i, _mm_sad_epu8(channel1, _mm_setzero_si128()));
		sumChannel2_128i = _mm_add_epi32(sumChannel2_128i, _mm_sad_epu8(channel2, _mm_setzero_si128()));

		buffer += remainingAfterFullBlocks48;
	}

	for (unsigned int n = 0u; n < blocks24; ++n)
	{
		const __m128i bufferA_128i = _mm_lddqu_si128((const __m128i*)(buffer + 0));
		const __m128i bufferB_128i = _mm_loadl_epi64((const __m128i*)(buffer + 16)); // load for unaligned 64 bit memory

		__m128i channel01_128i;
		__m128i channel2_128i;
		SSE::deInterleave3Channel8Bit24Elements(bufferA_128i, bufferB_128i, channel01_128i, channel2_128i);

		const __m128i sumChannel01_128i = _mm_sad_epu8(channel01_128i, _mm_setzero_si128());

		sumChannel0_128i = _mm_add_epi32(sumChannel0_128i, _mm_slli_si128(sumChannel01_128i, 8));
		sumChannel1_128i = _mm_add_epi32(sumChannel1_128i, _mm_srli_si128(sumChannel01_128i, 8));
		sumChannel2_128i = _mm_add_epi32(sumChannel2_128i, _mm_sad_epu8(channel2_128i, _mm_setzero_si128()));

		buffer += 24;
	}

	for (unsigned int n = 0u; n < blocks21; ++n)
	{
		const __m128i bufferA_128i = _mm_lddqu_si128((const __m128i*)(buffer + 0));
		const __m128i bufferB_128i = _mm_srli_si128(_mm_loadl_epi64((const __m128i*)(buffer + 16 - 3)), 3); // load for unaligned 64 bit memory

		__m128i channel01_128i;
		__m128i channel2_128i;
		SSE::deInterleave3Channel8Bit24Elements(bufferA_128i, bufferB_128i, channel01_128i, channel2_128i);

		const __m128i sumChannel01_128i = _mm_sad_epu8(channel01_128i, _mm_setzero_si128());

		sumChannel0_128i = _mm_add_epi32(sumChannel0_128i, _mm_slli_si128(sumChannel01_128i, 8));
		sumChannel1_128i = _mm_add_epi32(sumChannel1_128i, _mm_srli_si128(sumChannel01_128i, 8));
		sumChannel2_128i = _mm_add_epi32(sumChannel2_128i, _mm_sad_epu8(channel2_128i, _mm_setzero_si128()));

		buffer += 21;
	}

	for (unsigned int n = 0u; n < blocks15; ++n)
	{
		const __m128i buffer_128i = _mm_srli_si128(_mm_lddqu_si128((const __m128i*)(buffer - 1)), 1);

		__m128i channel01_128i;
		__m128i channel2_128i;
		SSE::deInterleave3Channel8Bit15Elements(buffer_128i, channel01_128i, channel2_128i);

		const __m128i sumChannel01_128i = _mm_sad_epu8(channel01_128i, _mm_setzero_si128());

		sumChannel0_128i = _mm_add_epi32(sumChannel0_128i, _mm_slli_si128(sumChannel01_128i, 8));
		sumChannel1_128i = _mm_add_epi32(sumChannel1_128i, _mm_srli_si128(sumChannel01_128i, 8));
		sumChannel2_128i = _mm_add_epi32(sumChannel2_128i, _mm_sad_epu8(channel2_128i, _mm_setzero_si128()));

		buffer += 15;
	}

	if constexpr (blocks1 != 0u)
	{
		constexpr unsigned int pixels = blocks1 / 3u;

		for (unsigned int x = 0u; x < pixels; ++x)
		{
			for (unsigned int n = 0u; n < 3u; ++n)
			{
				sumIndividual[n] += buffer[x * 3u + n];
			}
		}

		buffer += blocks1;
	}

	meanValues[0] = uint8_t((SSE::sum_u32_first_third(sumChannel0_128i) + sumIndividual[0] + tPixels / 2u) / tPixels);
	meanValues[1] = uint8_t((SSE::sum_u32_first_third(sumChannel1_128i) + sumIndividual[1] + tPixels / 2u) / tPixels);
	meanValues[2] = uint8_t((SSE::sum_u32_first_third(sumChannel2_128i) + sumIndividual[2] + tPixels / 2u) / tPixels);
}

template <unsigned int tChannels>
template <unsigned int tPixels>
inline void ZeroMeanSumSquareDifferencesSSE::SpecializedForChannels<tChannels>::mean8BitPerChannel(const uint8_t* const buffer, uint8_t* const meanValues)
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
inline void ZeroMeanSumSquareDifferencesSSE::SpecializedForChannels<1u>::mean8BitPerChannel(const uint8_t* patch, const unsigned int patchStrideElements, uint8_t* const meanValues)
{
	static_assert(tPatchSize >= 5u, "Invalid patch size!");

	constexpr unsigned int tChannels = 1u;

	ocean_assert(patch != nullptr && meanValues != nullptr);

	ocean_assert(patchStrideElements >= tChannels * tPatchSize);

	constexpr unsigned int patchWidthElements = tChannels * tPatchSize;

	constexpr unsigned int blocks16 = patchWidthElements / 16u;
	constexpr unsigned int remainingAfterBlocks16 = patchWidthElements % 16u;

	constexpr bool partialBlock16 = remainingAfterBlocks16 > 8u;

	constexpr bool fullBlock8 = !partialBlock16 && remainingAfterBlocks16 == 8u;

	constexpr bool partialBlock8 = !partialBlock16 && !fullBlock8 && remainingAfterBlocks16 >= 3u;

	constexpr unsigned int blocks1 = (!partialBlock16 && !fullBlock8 && !partialBlock8) ? remainingAfterBlocks16 : 0u;

	static_assert(blocks1 <= 2u, "Invalid block size!");

	__m128i sum_128i = _mm_setzero_si128();

	uint32_t sumIndividual = 0u;

	for (unsigned int y = 0u; y < tPatchSize; ++y)
	{
		SSE::prefetchT0(patch + patchStrideElements);

		for (unsigned int n = 0u; n < blocks16; ++n)
		{
			const __m128i buffer_128i = _mm_lddqu_si128((const __m128i*)patch);

			sum_128i = _mm_add_epi32(sum_128i, _mm_sad_epu8(buffer_128i, _mm_setzero_si128()));

			patch += 16;
		}

		if constexpr (fullBlock8)
		{
			const __m128i buffer_128i = _mm_loadl_epi64((const __m128i*)patch); // load for unaligned 64 bit memory

			sum_128i = _mm_add_epi32(sum_128i, _mm_sad_epu8(buffer_128i, _mm_setzero_si128()));

			patch += 8;
		}

		if constexpr (partialBlock16)
		{
			constexpr unsigned int overlapElements = partialBlock16 ? 16u - remainingAfterBlocks16 : 0u;

			static_assert(overlapElements < 8u, "Invalid value!");

			if (y < tPatchSize - 1u)
			{
				const __m128i buffer_128i = _mm_slli_si128(_mm_lddqu_si128((const __m128i*)patch), overlapElements); // loading 16 elements, but shifting `overlapElements` zeros to the left

				sum_128i = _mm_add_epi32(sum_128i, _mm_sad_epu8(buffer_128i, _mm_setzero_si128()));
			}
			else
			{
				const __m128i buffer_128i = _mm_srli_si128(_mm_lddqu_si128((const __m128i*)(patch - overlapElements)), overlapElements); // loading 16 elements, but shifting `overlapElements` zeros to the right

				sum_128i = _mm_add_epi32(sum_128i, _mm_sad_epu8(buffer_128i, _mm_setzero_si128()));
			}

			patch += remainingAfterBlocks16;
		}

		if constexpr (partialBlock8)
		{
			constexpr unsigned int overlapElements = partialBlock8 ? 8u - remainingAfterBlocks16 : 0u;

			static_assert(overlapElements < 8u, "Invalid value!");

			if (y < tPatchSize - 1u)
			{
				const __m128i buffer_128i = _mm_slli_si128(_mm_loadl_epi64((const __m128i*)patch), overlapElements + 8); // loading 8 elements, but shifting `overlapElements` zeros to the left

				sum_128i = _mm_add_epi32(sum_128i, _mm_sad_epu8(buffer_128i, _mm_setzero_si128()));
			}
			else
			{
				const __m128i buffer_128i = _mm_srli_si128(_mm_loadl_epi64((const __m128i*)(patch - overlapElements)), overlapElements); // loading 8 elements, but shifting `overlapElements` zeros to the right

				sum_128i = _mm_add_epi32(sum_128i, _mm_sad_epu8(buffer_128i, _mm_setzero_si128()));
			}

			patch += remainingAfterBlocks16;
		}

		if constexpr (blocks1 != 0u)
		{
			for (unsigned int n = 0u; n < blocks1; ++n)
			{
				sumIndividual += patch[n];
			}

			patch += blocks1;
		}

		patch += patchStrideElements - patchWidthElements;
	}

	const uint32_t sum = SSE::sum_u32_first_third(sum_128i) + sumIndividual;

	meanValues[0] = uint8_t((sum + tPatchSize * tPatchSize / 2u) / (tPatchSize * tPatchSize));
}

template <>
template <unsigned int tPatchSize>
inline void ZeroMeanSumSquareDifferencesSSE::SpecializedForChannels<3u>::mean8BitPerChannel(const uint8_t* patch, const unsigned int patchStrideElements, uint8_t* const meanValues)
{
	static_assert(tPatchSize >= 5u, "Invalid patch size!");

	constexpr unsigned int tChannels = 3u;

	ocean_assert(patch != nullptr && meanValues != nullptr);

	ocean_assert(patchStrideElements >= tChannels * tPatchSize);

	constexpr unsigned int patchWidthElements = tChannels * tPatchSize;

	constexpr unsigned int blocks48 = patchWidthElements / 48u;
	constexpr unsigned int remainingAfterFullBlocks48 = patchWidthElements % 48u;

	constexpr bool partialBlock48 = remainingAfterFullBlocks48 > 2u * 16u;

	constexpr unsigned int remainingAfterPartialBlock48 = partialBlock48 ? 0u : remainingAfterFullBlocks48;

	constexpr unsigned int blocks24 = remainingAfterPartialBlock48 / 24u;

	constexpr unsigned int remainingAfterPartialBlock24 = remainingAfterPartialBlock48 % 24u;

	constexpr unsigned int blocks21 = remainingAfterPartialBlock24 / 21u;

	constexpr unsigned int remainingAfterPartialBlock21 = remainingAfterPartialBlock24 % 21u;

	constexpr unsigned int blocks15 = remainingAfterPartialBlock21 / 15u;

	constexpr unsigned int remainingAfterPartialBlock15 = remainingAfterPartialBlock21 % 15u;

	constexpr unsigned int blocks1 = remainingAfterPartialBlock15;

	static_assert(blocks1 % 3u == 0u, "Invalid number of single blocks");

	__m128i sumChannel0_128i = _mm_setzero_si128();
	__m128i sumChannel1_128i = _mm_setzero_si128();
	__m128i sumChannel2_128i = _mm_setzero_si128();

	uint32_t sumIndividual[3] = {0u};

	for (unsigned int y = 0u; y < tPatchSize; ++y)
	{
		SSE::prefetchT0(patch + patchStrideElements);

		for (unsigned int n = 0u; n < blocks48; ++n)
		{
			const __m128i bufferA_128i = _mm_lddqu_si128((const __m128i*)(patch + 0));
			const __m128i bufferB_128i = _mm_lddqu_si128((const __m128i*)(patch + 16));
			const __m128i bufferC_128i = _mm_lddqu_si128((const __m128i*)(patch + 32));

			__m128i channel0;
			__m128i channel1;
			__m128i channel2;
			SSE::deInterleave3Channel8Bit48Elements(bufferA_128i, bufferB_128i, bufferC_128i, channel0, channel1, channel2);

			sumChannel0_128i = _mm_add_epi32(sumChannel0_128i, _mm_sad_epu8(channel0, _mm_setzero_si128()));
			sumChannel1_128i = _mm_add_epi32(sumChannel1_128i, _mm_sad_epu8(channel1, _mm_setzero_si128()));
			sumChannel2_128i = _mm_add_epi32(sumChannel2_128i, _mm_sad_epu8(channel2, _mm_setzero_si128()));

			patch += 48;
		}

		if constexpr (partialBlock48)
		{
			constexpr int overlappingElements = int(48u - remainingAfterFullBlocks48);

			const __m128i bufferA_128i = _mm_slli_si128(_mm_lddqu_si128((const __m128i*)(patch)), overlappingElements);
			const __m128i bufferB_128i = _mm_lddqu_si128((const __m128i*)(patch - overlappingElements + 16));
			const __m128i bufferC_128i = _mm_lddqu_si128((const __m128i*)(patch - overlappingElements + 32));

			__m128i channel0;
			__m128i channel1;
			__m128i channel2;
			SSE::deInterleave3Channel8Bit48Elements(bufferA_128i, bufferB_128i, bufferC_128i, channel0, channel1, channel2);

			sumChannel0_128i = _mm_add_epi32(sumChannel0_128i, _mm_sad_epu8(channel0, _mm_setzero_si128()));
			sumChannel1_128i = _mm_add_epi32(sumChannel1_128i, _mm_sad_epu8(channel1, _mm_setzero_si128()));
			sumChannel2_128i = _mm_add_epi32(sumChannel2_128i, _mm_sad_epu8(channel2, _mm_setzero_si128()));

			patch += remainingAfterFullBlocks48;
		}

		for (unsigned int n = 0u; n < blocks24; ++n)
		{
			const __m128i bufferA_128i = _mm_lddqu_si128((const __m128i*)(patch + 0));
			const __m128i bufferB_128i = _mm_loadl_epi64((const __m128i*)(patch + 16)); // load for unaligned 64 bit memory

			__m128i channel01_128i;
			__m128i channel2_128i;
			SSE::deInterleave3Channel8Bit24Elements(bufferA_128i, bufferB_128i, channel01_128i, channel2_128i);

			const __m128i sumChannel01_128i = _mm_sad_epu8(channel01_128i, _mm_setzero_si128());

			sumChannel0_128i = _mm_add_epi32(sumChannel0_128i, _mm_slli_si128(sumChannel01_128i, 8));
			sumChannel1_128i = _mm_add_epi32(sumChannel1_128i, _mm_srli_si128(sumChannel01_128i, 8));
			sumChannel2_128i = _mm_add_epi32(sumChannel2_128i, _mm_sad_epu8(channel2_128i, _mm_setzero_si128()));

			patch += 24;
		}

		for (unsigned int n = 0u; n < blocks21; ++n)
		{
			const __m128i bufferA_128i = _mm_lddqu_si128((const __m128i*)(patch + 0));
			const __m128i bufferB_128i = _mm_srli_si128(_mm_loadl_epi64((const __m128i*)(patch + 16 - 3)), 3); // load for unaligned 64 bit memory

			__m128i channel01_128i;
			__m128i channel2_128i;
			SSE::deInterleave3Channel8Bit24Elements(bufferA_128i, bufferB_128i, channel01_128i, channel2_128i);

			const __m128i sumChannel01_128i = _mm_sad_epu8(channel01_128i, _mm_setzero_si128());

			sumChannel0_128i = _mm_add_epi32(sumChannel0_128i, _mm_slli_si128(sumChannel01_128i, 8));
			sumChannel1_128i = _mm_add_epi32(sumChannel1_128i, _mm_srli_si128(sumChannel01_128i, 8));
			sumChannel2_128i = _mm_add_epi32(sumChannel2_128i, _mm_sad_epu8(channel2_128i, _mm_setzero_si128()));

			patch += 21;
		}

		for (unsigned int n = 0u; n < blocks15; ++n)
		{
			const __m128i buffer_128i = y < tPatchSize - 1u ? _mm_lddqu_si128((const __m128i*)(patch)) : _mm_srli_si128(_mm_lddqu_si128((const __m128i*)(patch - 1)), 1);

			__m128i channel01_128i;
			__m128i channel2_128i;
			SSE::deInterleave3Channel8Bit15Elements(buffer_128i, channel01_128i, channel2_128i);

			const __m128i sumChannel01_128i = _mm_sad_epu8(channel01_128i, _mm_setzero_si128());

			sumChannel0_128i = _mm_add_epi32(sumChannel0_128i, _mm_slli_si128(sumChannel01_128i, 8));
			sumChannel1_128i = _mm_add_epi32(sumChannel1_128i, _mm_srli_si128(sumChannel01_128i, 8));
			sumChannel2_128i = _mm_add_epi32(sumChannel2_128i, _mm_sad_epu8(channel2_128i, _mm_setzero_si128()));

			patch += 15;
		}

		if constexpr (blocks1 != 0u)
		{
			constexpr unsigned int pixels = blocks1 / 3u;

			for (unsigned int x = 0u; x < pixels; ++x)
			{
				for (unsigned int n = 0u; n < 3u; ++n)
				{
					sumIndividual[n] += patch[x * 3u + n];
				}
			}

			patch += blocks1;
		}

		patch += patchStrideElements - patchWidthElements;
	}

	meanValues[0] = uint8_t((SSE::sum_u32_first_third(sumChannel0_128i) + sumIndividual[0] + tPatchSize * tPatchSize / 2u) / (tPatchSize * tPatchSize));
	meanValues[1] = uint8_t((SSE::sum_u32_first_third(sumChannel1_128i) + sumIndividual[1] + tPatchSize * tPatchSize / 2u) / (tPatchSize * tPatchSize));
	meanValues[2] = uint8_t((SSE::sum_u32_first_third(sumChannel2_128i) + sumIndividual[2] + tPatchSize * tPatchSize / 2u) / (tPatchSize * tPatchSize));
}

template <unsigned int tChannels>
template <unsigned int tPatchSize>
inline void ZeroMeanSumSquareDifferencesSSE::SpecializedForChannels<tChannels>::mean8BitPerChannel(const uint8_t* patch, const unsigned int patchStrideElements, uint8_t* const meanValues)
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
template <unsigned int tPixels>
inline uint32_t ZeroMeanSumSquareDifferencesSSE::SpecializedForChannels<1u>::buffer8BitPerChannel(const uint8_t* buffer0, const uint8_t* buffer1, const uint8_t* const meanValues0, const uint8_t* const meanValues1)
{
	static_assert(tPixels >= 8u, "Invalid pixel number!");

	constexpr unsigned int tChannels = 1u;

	ocean_assert(buffer0 != nullptr && buffer1 != nullptr);
	ocean_assert(meanValues0 != nullptr && meanValues1 != nullptr);

	constexpr unsigned int bufferElements = tChannels * tPixels;

	constexpr unsigned int blocks16 = bufferElements / 16u;
	constexpr unsigned int remainingAfterBlocks16 = bufferElements % 16u;

	constexpr bool partialBlock16 = remainingAfterBlocks16 > 8u;

	constexpr bool fullBlock8 = !partialBlock16 && remainingAfterBlocks16 == 8u;

	constexpr bool partialBlock8 = !partialBlock16 && !fullBlock8 && remainingAfterBlocks16 >= 3u;

	constexpr unsigned int blocks1 = (!partialBlock16 && !fullBlock8 && !partialBlock8) ? remainingAfterBlocks16 : 0u;

	static_assert(blocks1 <= 2u, "Invalid block size!");

	static_assert(std::is_same<short, int16_t>::value, "Invalid data type!");

	const __m128i constant_signs_m128i = _mm_set1_epi16(short(0x1FF)); // -1, 1, -1, 1, -1, 1, -1, 1

	const __m128i mean0_128i = _mm_set1_epi8(meanValues0[0]);
	const __m128i mean1_128i = _mm_set1_epi8(meanValues1[0]);

	__m128i sum0_128i = _mm_setzero_si128();
	__m128i sum1_128i = _mm_setzero_si128();

	uint32_t sumIndividual = 0u;

	for (unsigned int n = 0u; n < blocks16; ++n)
	{
		const __m128i buffer0_128i = _mm_lddqu_si128((const __m128i*)buffer0);
		const __m128i buffer1_128i = _mm_lddqu_si128((const __m128i*)buffer1);

		const __m128i absDifferencesLow_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_128i, buffer0_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_128i, buffer1_128i), constant_signs_m128i)); // (buffer0 - mean0) - (buffer1 - mean1)
		const __m128i absDifferencesHigh_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpackhi_epi8(mean0_128i, buffer0_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpackhi_epi8(mean1_128i, buffer1_128i), constant_signs_m128i));

		sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
		sum1_128i = _mm_add_epi32(sum1_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

		buffer0 += 16;
		buffer1 += 16;
	}

	if constexpr (partialBlock16)
	{
		constexpr unsigned int overlapElements = partialBlock16 ? 16u - remainingAfterBlocks16 : 0u;

		static_assert(overlapElements < 8u, "Invalid value!");

		const __m128i buffer0_128i = _mm_srli_si128(_mm_lddqu_si128((const __m128i*)(buffer0 - overlapElements)), overlapElements); // loading 16 elements, but shifting `overlapElements` zeros to the right
		const __m128i buffer1_128i = _mm_srli_si128(_mm_lddqu_si128((const __m128i*)(buffer1 - overlapElements)), overlapElements);

		const __m128i absDifferencesLow_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_128i, buffer0_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_128i, buffer1_128i), constant_signs_m128i)); // (buffer0 - mean0) - (buffer1 - mean1)
		const __m128i absDifferencesHigh_128i = _mm_slli_si128(_mm_sub_epi16(_mm_maddubs_epi16(_mm_unpackhi_epi8(mean0_128i, buffer0_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpackhi_epi8(mean1_128i, buffer1_128i), constant_signs_m128i)), overlapElements * 2);

		sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
		sum1_128i = _mm_add_epi32(sum1_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

		buffer0 += remainingAfterBlocks16;
		buffer1 += remainingAfterBlocks16;
	}

	if constexpr (fullBlock8)
	{
		const __m128i buffer0_128i = _mm_loadl_epi64((const __m128i*)buffer0); // load for unaligned 64 bit memory
		const __m128i buffer1_128i = _mm_loadl_epi64((const __m128i*)buffer1); // load for unaligned 64 bit memory

		const __m128i absDifferencesLow_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_128i, buffer0_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_128i, buffer1_128i), constant_signs_m128i)); // (buffer0 - mean0) - (buffer1 - mean1)
		const __m128i absDifferencesHigh_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpackhi_epi8(mean0_128i, buffer0_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpackhi_epi8(mean1_128i, buffer1_128i), constant_signs_m128i));

		sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
		sum1_128i = _mm_add_epi32(sum1_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

		buffer0 += 8;
		buffer1 += 8;
	}

	if constexpr (partialBlock8)
	{
		constexpr unsigned int overlapElements = partialBlock8 ? 8u - remainingAfterBlocks16 : 0u;

		static_assert(overlapElements < 8u, "Invalid value!");

		const __m128i buffer0_128i = _mm_srli_si128(_mm_loadl_epi64((const __m128i*)(buffer0 - overlapElements)), overlapElements); // loading 8 elements, but shifting `overlapElements` zeros to the right
		const __m128i buffer1_128i = _mm_srli_si128(_mm_loadl_epi64((const __m128i*)(buffer1 - overlapElements)), overlapElements);

		const __m128i absDifferencesLow_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_128i, buffer0_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_128i, buffer1_128i), constant_signs_m128i)); // (buffer0 - mean0) - (buffer1 - mean1)
		const __m128i absDifferencesHigh_128i = _mm_slli_si128(_mm_sub_epi16(_mm_maddubs_epi16(_mm_unpackhi_epi8(mean0_128i, buffer0_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpackhi_epi8(mean1_128i, buffer1_128i), constant_signs_m128i)), overlapElements * 2);

		sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
		sum1_128i = _mm_add_epi32(sum1_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

		buffer0 += remainingAfterBlocks16;
		buffer1 += remainingAfterBlocks16;
	}

	if constexpr (blocks1 != 0u)
	{
		for (unsigned int n = 0u; n < blocks1; ++n)
		{
			sumIndividual += sqrDistance(buffer0[n] - meanValues0[0], buffer1[n] - meanValues1[0]);
		}

		buffer0 += blocks1;
		buffer1 += blocks1;
	}

	return SSE::sum_u32_4(sum0_128i) + SSE::sum_u32_4(sum1_128i) + sumIndividual;
}

template <>
template <unsigned int tPixels>
inline uint32_t ZeroMeanSumSquareDifferencesSSE::SpecializedForChannels<3u>::buffer8BitPerChannel(const uint8_t* buffer0, const uint8_t* buffer1, const uint8_t* const meanValues0, const uint8_t* const meanValues1)
{
	static_assert(tPixels >= 5u, "Invalid pixel number!");

	constexpr unsigned int tChannels = 3u;

	ocean_assert(buffer0 != nullptr && buffer1 != nullptr);
	ocean_assert(meanValues0 != nullptr && meanValues1 != nullptr);

	constexpr unsigned int bufferElements = tChannels * tPixels;

	constexpr unsigned int blocks48 = bufferElements / 48u;
	constexpr unsigned int remainingAfterFullBlocks48 = bufferElements % 48u;

	constexpr bool partialBlock48 = remainingAfterFullBlocks48 > 2u * 16u;

	constexpr unsigned int remainingAfterPartialBlock48 = partialBlock48 ? 0u : remainingAfterFullBlocks48;

	constexpr unsigned int blocks24 = remainingAfterPartialBlock48 / 24u;

	constexpr unsigned int remainingAfterPartialBlock24 = remainingAfterPartialBlock48 % 24u;

	constexpr unsigned int blocks21 = remainingAfterPartialBlock24 / 21u;

	constexpr unsigned int remainingAfterPartialBlock21 = remainingAfterPartialBlock24 % 21u;

	constexpr unsigned int blocks15 = remainingAfterPartialBlock21 / 15u;

	constexpr unsigned int remainingAfterPartialBlock15 = remainingAfterPartialBlock21 % 15u;

	constexpr unsigned int blocks1 = remainingAfterPartialBlock15;

	static_assert(blocks1 % 3u == 0u, "Invalid number of single blocks");

	static_assert(std::is_same<short, int16_t>::value, "Invalid data type!");

	const __m128i constant_signs_m128i = _mm_set1_epi16(short(0x1FF)); // -1, 1, -1, 1, -1, 1, -1, 1

	const __m128i mean0_0_128i = _mm_set1_epi8(meanValues0[0]);
	const __m128i mean0_1_128i = _mm_set1_epi8(meanValues0[1]);
	const __m128i mean0_2_128i = _mm_set1_epi8(meanValues0[2]);

	const __m128i mean1_0_128i = _mm_set1_epi8(meanValues1[0]);
	const __m128i mean1_1_128i = _mm_set1_epi8(meanValues1[1]);
	const __m128i mean1_2_128i = _mm_set1_epi8(meanValues1[2]);

	__m128i sum0_128i = _mm_setzero_si128();
	__m128i sum1_128i = _mm_setzero_si128();

	uint32_t sumIndividual = 0u;

	for (unsigned int n = 0u; n < blocks48; ++n)
	{
		const __m128i buffer0A_128i = _mm_lddqu_si128((const __m128i*)(buffer0 + 0));
		const __m128i buffer0B_128i = _mm_lddqu_si128((const __m128i*)(buffer0 + 16));
		const __m128i buffer0C_128i = _mm_lddqu_si128((const __m128i*)(buffer0 + 32));

		__m128i channel0_0_128i;
		__m128i channel0_1_128i;
		__m128i channel0_2_128i;
		SSE::deInterleave3Channel8Bit48Elements(buffer0A_128i, buffer0B_128i, buffer0C_128i, channel0_0_128i, channel0_1_128i, channel0_2_128i);

		const __m128i buffer1A_128i = _mm_lddqu_si128((const __m128i*)(buffer1 + 0));
		const __m128i buffer1B_128i = _mm_lddqu_si128((const __m128i*)(buffer1 + 16));
		const __m128i buffer1C_128i = _mm_lddqu_si128((const __m128i*)(buffer1 + 32));

		__m128i channel1_0_128i;
		__m128i channel1_1_128i;
		__m128i channel1_2_128i;
		SSE::deInterleave3Channel8Bit48Elements(buffer1A_128i, buffer1B_128i, buffer1C_128i, channel1_0_128i, channel1_1_128i, channel1_2_128i);

		__m128i absDifferencesLow_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_0_128i, channel0_0_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_0_128i, channel1_0_128i), constant_signs_m128i)); // (channel0_0 - mean0) - (channel1_0 - mean1)
		__m128i absDifferencesHigh_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpackhi_epi8(mean0_0_128i, channel0_0_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpackhi_epi8(mean1_0_128i, channel1_0_128i), constant_signs_m128i));

		sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i)); // sum0_128i += absDifferencesLow_128i * absDifferencesLow_128i
		sum1_128i = _mm_add_epi32(sum1_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

		absDifferencesLow_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_1_128i, channel0_1_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_1_128i, channel1_1_128i), constant_signs_m128i));
		absDifferencesHigh_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpackhi_epi8(mean0_1_128i, channel0_1_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpackhi_epi8(mean1_1_128i, channel1_1_128i), constant_signs_m128i));

		sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
		sum1_128i = _mm_add_epi32(sum1_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

		absDifferencesLow_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_2_128i, channel0_2_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_2_128i, channel1_2_128i), constant_signs_m128i));
		absDifferencesHigh_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpackhi_epi8(mean0_2_128i, channel0_2_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpackhi_epi8(mean1_2_128i, channel1_2_128i), constant_signs_m128i));

		sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
		sum1_128i = _mm_add_epi32(sum1_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

		buffer0 += 48;
		buffer1 += 48;
	}

	if constexpr (partialBlock48)
	{
		constexpr int overlappingElements = int(48u - remainingAfterFullBlocks48);
		constexpr int overlappingPixels = overlappingElements / int(tChannels);

		const __m128i buffer0A_128i = _mm_slli_si128(_mm_lddqu_si128((const __m128i*)(buffer0)), overlappingElements);
		const __m128i buffer0B_128i = _mm_lddqu_si128((const __m128i*)(buffer0 - overlappingElements + 16));
		const __m128i buffer0C_128i = _mm_lddqu_si128((const __m128i*)(buffer0 - overlappingElements + 32));

		__m128i channel0_0_128i;
		__m128i channel0_1_128i;
		__m128i channel0_2_128i;
		SSE::deInterleave3Channel8Bit48Elements(buffer0A_128i, buffer0B_128i, buffer0C_128i, channel0_0_128i, channel0_1_128i, channel0_2_128i);

		const __m128i buffer1A_128i = _mm_slli_si128(_mm_lddqu_si128((const __m128i*)(buffer1)), overlappingElements);
		const __m128i buffer1B_128i = _mm_lddqu_si128((const __m128i*)(buffer1 - overlappingElements + 16));
		const __m128i buffer1C_128i = _mm_lddqu_si128((const __m128i*)(buffer1 - overlappingElements + 32));

		__m128i channel1_0_128i;
		__m128i channel1_1_128i;
		__m128i channel1_2_128i;
		SSE::deInterleave3Channel8Bit48Elements(buffer1A_128i, buffer1B_128i, buffer1C_128i, channel1_0_128i, channel1_1_128i, channel1_2_128i);

		__m128i absDifferencesLow_128i = _mm_srli_si128(_mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_0_128i, channel0_0_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_0_128i, channel1_0_128i), constant_signs_m128i)), overlappingPixels * 2); // (channel0_0 - mean0) - (channel1_0 - mean1)
		__m128i absDifferencesHigh_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpackhi_epi8(mean0_0_128i, channel0_0_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpackhi_epi8(mean1_0_128i, channel1_0_128i), constant_signs_m128i));

		sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i)); // sum0_128i += absDifferencesLow_128i * absDifferencesLow_128i
		sum1_128i = _mm_add_epi32(sum1_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

		absDifferencesLow_128i = _mm_srli_si128(_mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_1_128i, channel0_1_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_1_128i, channel1_1_128i), constant_signs_m128i)), overlappingPixels * 2);
		absDifferencesHigh_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpackhi_epi8(mean0_1_128i, channel0_1_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpackhi_epi8(mean1_1_128i, channel1_1_128i), constant_signs_m128i));

		sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
		sum1_128i = _mm_add_epi32(sum1_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

		absDifferencesLow_128i = _mm_srli_si128(_mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_2_128i, channel0_2_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_2_128i, channel1_2_128i), constant_signs_m128i)), overlappingPixels * 2);
		absDifferencesHigh_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpackhi_epi8(mean0_2_128i, channel0_2_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpackhi_epi8(mean1_2_128i, channel1_2_128i), constant_signs_m128i));

		sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
		sum1_128i = _mm_add_epi32(sum1_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

		buffer0 += remainingAfterFullBlocks48;
		buffer1 += remainingAfterFullBlocks48;
	}

	for (unsigned int n = 0u; n < blocks24; ++n)
	{
		const __m128i buffer0A_128i = _mm_lddqu_si128((const __m128i*)(buffer0 + 0));
		const __m128i buffer0B_128i = _mm_loadl_epi64((const __m128i*)(buffer0 + 16)); // load for unaligned 64 bit memory

		__m128i channel0_01_128i;
		__m128i channel0_2_128i;
		SSE::deInterleave3Channel8Bit24Elements(buffer0A_128i, buffer0B_128i, channel0_01_128i, channel0_2_128i);

		const __m128i buffer1A_128i = _mm_lddqu_si128((const __m128i*)(buffer1 + 0));
		const __m128i buffer1B_128i = _mm_loadl_epi64((const __m128i*)(buffer1 + 16)); // load for unaligned 64 bit memory

		__m128i channel1_01_128i;
		__m128i channel1_2_128i;
		SSE::deInterleave3Channel8Bit24Elements(buffer1A_128i, buffer1B_128i, channel1_01_128i, channel1_2_128i);

		__m128i absDifferencesLow_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_0_128i, channel0_01_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_0_128i, channel1_01_128i), constant_signs_m128i)); // (channel0_01 - mean0) - (channel1_01 - mean1)
		__m128i absDifferencesHigh_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpackhi_epi8(mean0_1_128i, channel0_01_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpackhi_epi8(mean1_1_128i, channel1_01_128i), constant_signs_m128i));

		sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i)); // sum0_128i += absDifferencesLow_128i * absDifferencesLow_128i
		sum1_128i = _mm_add_epi32(sum1_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

		absDifferencesLow_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_2_128i, channel0_2_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_2_128i, channel1_2_128i), constant_signs_m128i));

		sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i)); // sum0_128i += absDifferencesLow_128i * absDifferencesLow_128i

		buffer0 += 24;
		buffer1 += 24;
	}

	for (unsigned int n = 0u; n < blocks21; ++n)
	{
		const __m128i buffer0A_128i = _mm_lddqu_si128((const __m128i*)(buffer0 + 0));
		const __m128i buffer0B_128i = _mm_srli_si128(_mm_loadl_epi64((const __m128i*)(buffer0 + 16 - 3)), 3); // load for unaligned 64 bit memory

		__m128i channel0_01_128i;
		__m128i channel0_2_128i;
		SSE::deInterleave3Channel8Bit24Elements(buffer0A_128i, buffer0B_128i, channel0_01_128i, channel0_2_128i);

		const __m128i buffer1A_128i = _mm_lddqu_si128((const __m128i*)(buffer1 + 0));
		const __m128i buffer1B_128i = _mm_srli_si128(_mm_loadl_epi64((const __m128i*)(buffer1 + 16 - 3)), 3); // load for unaligned 64 bit memory

		__m128i channel1_01_128i;
		__m128i channel1_2_128i;
		SSE::deInterleave3Channel8Bit24Elements(buffer1A_128i, buffer1B_128i, channel1_01_128i, channel1_2_128i);

		__m128i absDifferencesLow_128i = _mm_slli_si128(_mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_0_128i, channel0_01_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_0_128i, channel1_01_128i), constant_signs_m128i)), 2); // (channel0_01 - mean0) - (channel1_01 - mean1)
		__m128i absDifferencesHigh_128i = _mm_slli_si128(_mm_sub_epi16(_mm_maddubs_epi16(_mm_unpackhi_epi8(mean0_1_128i, channel0_01_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpackhi_epi8(mean1_1_128i, channel1_01_128i), constant_signs_m128i)), 2);

		sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i)); // sum0_128i += absDifferencesLow_128i * absDifferencesLow_128i
		sum1_128i = _mm_add_epi32(sum1_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

		absDifferencesLow_128i = _mm_slli_si128(_mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_2_128i, channel0_2_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_2_128i, channel1_2_128i), constant_signs_m128i)), 2);

		sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i)); // sum0_128i += absDifferencesLow_128i * absDifferencesLow_128i

		buffer0 += 21;
		buffer1 += 21;
	}

	for (unsigned int n = 0u; n < blocks15; ++n)
	{
		const __m128i buffer0_128i = _mm_srli_si128(_mm_lddqu_si128((const __m128i*)(buffer0 - 1)), 1);

		__m128i channel0_01_128i;
		__m128i channel0_2_128i;
		SSE::deInterleave3Channel8Bit15Elements(buffer0_128i, channel0_01_128i, channel0_2_128i);

		const __m128i buffer1_128i = _mm_srli_si128(_mm_lddqu_si128((const __m128i*)(buffer1 - 1)), 1);

		__m128i channel1_01_128i;
		__m128i channel1_2_128i;
		SSE::deInterleave3Channel8Bit15Elements(buffer1_128i, channel1_01_128i, channel1_2_128i);

		__m128i absDifferencesLow_128i = _mm_slli_si128(_mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_0_128i, channel0_01_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_0_128i, channel1_01_128i), constant_signs_m128i)), 6); // (channel0_01 - mean0) - (channel1_01 - mean1)
		__m128i absDifferencesHigh_128i = _mm_slli_si128(_mm_sub_epi16(_mm_maddubs_epi16(_mm_unpackhi_epi8(mean0_1_128i, channel0_01_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpackhi_epi8(mean1_1_128i, channel1_01_128i), constant_signs_m128i)), 6);

		sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i)); // sum0_128i += absDifferencesLow_128i * absDifferencesLow_128i
		sum1_128i = _mm_add_epi32(sum1_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

		absDifferencesLow_128i = _mm_slli_si128(_mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_2_128i, channel0_2_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_2_128i, channel1_2_128i), constant_signs_m128i)), 6);

		sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i)); // sum0_128i += absDifferencesLow_128i * absDifferencesLow_128i

		buffer0 += 15;
		buffer1 += 15;
	}

	if constexpr (blocks1 != 0u)
	{
		constexpr unsigned int pixels = blocks1 / 3u;

		for (unsigned int x = 0u; x < pixels; ++x)
		{
			for (unsigned int n = 0u; n < 3u; ++n)
			{
				sumIndividual += sqrDistance(buffer0[x * 3u + n] - meanValues0[n], buffer1[x * 3u + n] - meanValues1[n]);
			}
		}

		buffer0 += blocks1;
		buffer1 += blocks1;
	}

	return SSE::sum_u32_4(sum0_128i) + SSE::sum_u32_4(sum1_128i) + sumIndividual;
}

template <unsigned int tChannels>
template <unsigned int tPixels>
inline uint32_t ZeroMeanSumSquareDifferencesSSE::SpecializedForChannels<tChannels>::buffer8BitPerChannel(const uint8_t* const buffer0, const uint8_t* const buffer1, const uint8_t* const meanValues0, const uint8_t* const meanValues1)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPixels >= 1u, "Invalid patch size!");

	ocean_assert(buffer0 != nullptr && buffer1 != nullptr);
	ocean_assert(meanValues0 != nullptr && meanValues1 != nullptr);

	uint32_t ssd = 0u;

	for (unsigned int n = 0u; n < tPixels; ++n)
	{
		for (unsigned int c = 0u; c < tChannels; ++c)
		{
			ssd += sqrDistance(buffer0[n * tChannels + c] - meanValues0[c], buffer1[n * tChannels + c] - meanValues1[c]);
		}
	}

	return ssd;
}

template <>
template <unsigned int tPatchSize>
inline uint32_t ZeroMeanSumSquareDifferencesSSE::SpecializedForChannels<1u>::patch8BitPerChannel(const uint8_t* patch0, const uint8_t* patch1, const unsigned int patch0StrideElements, const unsigned int patch1StrideElements, const uint8_t* const meanValues0, const uint8_t* const meanValues1)
{
	static_assert(tPatchSize >= 1u, "Invalid patch size!");

	constexpr unsigned int tChannels = 1u;

	ocean_assert(patch0 != nullptr && patch1 != nullptr);
	ocean_assert(meanValues0 != nullptr && meanValues1 != nullptr);

	ocean_assert(patch0StrideElements >= tChannels * tPatchSize);
	ocean_assert(patch1StrideElements >= tChannels * tPatchSize);

	constexpr unsigned int patchWidthElements = tChannels * tPatchSize;

	constexpr unsigned int blocks16 = patchWidthElements / 16u;
	constexpr unsigned int remainingAfterBlocks16 = patchWidthElements % 16u;

	constexpr bool partialBlock16 = remainingAfterBlocks16 > 8u;

	constexpr bool fullBlock8 = !partialBlock16 && remainingAfterBlocks16 == 8u;

	constexpr bool partialBlock8 = !partialBlock16 && !fullBlock8 && remainingAfterBlocks16 >= 3u;

	constexpr unsigned int blocks1 = (!partialBlock16 && !fullBlock8 && !partialBlock8) ? remainingAfterBlocks16 : 0u;

	static_assert(blocks1 <= 2u, "Invalid block size!");

	static_assert(std::is_same<short, int16_t>::value, "Invalid data type!");

	const __m128i constant_signs_m128i = _mm_set1_epi16(short(0x1FF)); // -1, 1, -1, 1, -1, 1, -1, 1

	const __m128i mean0_128i = _mm_set1_epi8(meanValues0[0]);
	const __m128i mean1_128i = _mm_set1_epi8(meanValues1[0]);

	__m128i sum0_128i = _mm_setzero_si128();
	__m128i sum1_128i = _mm_setzero_si128();

	uint32_t sumIndividual = 0u;

	for (unsigned int y = 0u; y < tPatchSize; ++y)
	{
		SSE::prefetchT0(patch0 + patch0StrideElements);
		SSE::prefetchT0(patch1 + patch1StrideElements);

		for (unsigned int n = 0u; n < blocks16; ++n)
		{
			const __m128i buffer0_128i = _mm_lddqu_si128((const __m128i*)patch0);
			const __m128i buffer1_128i = _mm_lddqu_si128((const __m128i*)patch1);

			const __m128i absDifferencesLow_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_128i, buffer0_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_128i, buffer1_128i), constant_signs_m128i)); // (buffer0 - mean0) - (buffer1 - mean1)
			const __m128i absDifferencesHigh_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpackhi_epi8(mean0_128i, buffer0_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpackhi_epi8(mean1_128i, buffer1_128i), constant_signs_m128i));

			sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
			sum1_128i = _mm_add_epi32(sum1_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

			patch0 += 16;
			patch1 += 16;
		}

		if constexpr (fullBlock8)
		{
			const __m128i buffer0_128i = _mm_loadl_epi64((const __m128i*)patch0); // load for unaligned 64 bit memory
			const __m128i buffer1_128i = _mm_loadl_epi64((const __m128i*)patch1); // load for unaligned 64 bit memory

			const __m128i absDifferencesLow_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_128i, buffer0_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_128i, buffer1_128i), constant_signs_m128i)); // (buffer0 - mean0) - (buffer1 - mean1)
			const __m128i absDifferencesHigh_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpackhi_epi8(mean0_128i, buffer0_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpackhi_epi8(mean1_128i, buffer1_128i), constant_signs_m128i));

			sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
			sum1_128i = _mm_add_epi32(sum1_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

			patch0 += 8;
			patch1 += 8;
		}

		if constexpr (partialBlock16)
		{
			constexpr unsigned int overlapElements = partialBlock16 ? 16u - remainingAfterBlocks16 : 0u;

			static_assert(overlapElements < 8u, "Invalid value!");

			if (y < tPatchSize - 1u)
			{
				const __m128i buffer0_128i = _mm_lddqu_si128((const __m128i*)patch0); // loading 16 elements
				const __m128i buffer1_128i = _mm_lddqu_si128((const __m128i*)patch1);

				const __m128i absDifferencesLow_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_128i, buffer0_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_128i, buffer1_128i), constant_signs_m128i)); // (buffer0 - mean0) - (buffer1 - mean1)
				const __m128i absDifferencesHigh_128i = _mm_slli_si128(_mm_sub_epi16(_mm_maddubs_epi16(_mm_unpackhi_epi8(mean0_128i, buffer0_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpackhi_epi8(mean1_128i, buffer1_128i), constant_signs_m128i)), overlapElements * 2);

				sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
				sum1_128i = _mm_add_epi32(sum1_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));
			}
			else
			{
				const __m128i buffer0_128i = _mm_lddqu_si128((const __m128i*)(patch0 - overlapElements)); // loading 16 elements
				const __m128i buffer1_128i = _mm_lddqu_si128((const __m128i*)(patch1 - overlapElements));

				const __m128i absDifferencesLow_128i = _mm_srli_si128(_mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_128i, buffer0_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_128i, buffer1_128i), constant_signs_m128i)), overlapElements * 2); // (buffer0 - mean0) - (buffer1 - mean1)
				const __m128i absDifferencesHigh_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpackhi_epi8(mean0_128i, buffer0_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpackhi_epi8(mean1_128i, buffer1_128i), constant_signs_m128i));

				sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
				sum1_128i = _mm_add_epi32(sum1_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));
			}

			patch0 += remainingAfterBlocks16;
			patch1 += remainingAfterBlocks16;
		}

		if constexpr (partialBlock8)
		{
			constexpr unsigned int overlapElements = partialBlock8 ? 8u - remainingAfterBlocks16 : 0u;

			static_assert(overlapElements < 8u, "Invalid value!");

			if (y < tPatchSize - 1u)
			{
				const __m128i buffer0_128i = _mm_loadl_epi64((const __m128i*)patch0);// loading 8 elements
				const __m128i buffer1_128i = _mm_loadl_epi64((const __m128i*)patch1);

				const __m128i absDifferencesLow_128i = _mm_slli_si128(_mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_128i, buffer0_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_128i, buffer1_128i), constant_signs_m128i)), overlapElements * 2); // (buffer0 - mean0) - (buffer1 - mean1)

				sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
			}
			else
			{
				const __m128i buffer0_128i = _mm_loadl_epi64((const __m128i*)(patch0 - overlapElements)); // loading 8 elements
				const __m128i buffer1_128i = _mm_loadl_epi64((const __m128i*)(patch1 - overlapElements));

				const __m128i absDifferencesLow_128i = _mm_srli_si128(_mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_128i, buffer0_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_128i, buffer1_128i), constant_signs_m128i)), overlapElements * 2); // (buffer0 - mean0) - (buffer1 - mean1)

				sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
			}

			patch0 += remainingAfterBlocks16;
			patch1 += remainingAfterBlocks16;
		}

		if constexpr (blocks1 != 0u)
		{
			for (unsigned int n = 0u; n < blocks1; ++n)
			{
				sumIndividual += sqrDistance(patch0[n] - meanValues0[0], patch1[n] - meanValues1[0]);
			}

			patch0 += blocks1;
			patch1 += blocks1;
		}

		patch0 += patch0StrideElements - patchWidthElements;
		patch1 += patch1StrideElements - patchWidthElements;
	}

	return SSE::sum_u32_4(sum0_128i) + SSE::sum_u32_4(sum1_128i) + sumIndividual;
}

template <>
template <unsigned int tPatchSize>
inline uint32_t ZeroMeanSumSquareDifferencesSSE::SpecializedForChannels<3u>::patch8BitPerChannel(const uint8_t* patch0, const uint8_t* patch1, const unsigned int patch0StrideElements, const unsigned int patch1StrideElements, const uint8_t* const meanValues0, const uint8_t* const meanValues1)
{
	static_assert(tPatchSize >= 5u, "Invalid patch size!");

	constexpr unsigned int tChannels = 3u;

	ocean_assert(patch0 != nullptr && patch1 != nullptr);
	ocean_assert(meanValues0 != nullptr && meanValues1 != nullptr);

	ocean_assert(patch0StrideElements >= tChannels * tPatchSize);
	ocean_assert(patch1StrideElements >= tChannels * tPatchSize);

	constexpr unsigned int patchWidthElements = tChannels * tPatchSize;

	constexpr unsigned int blocks48 = patchWidthElements / 48u;
	constexpr unsigned int remainingAfterFullBlocks48 = patchWidthElements % 48u;

	constexpr bool partialBlock48 = remainingAfterFullBlocks48 > 2u * 16u;

	constexpr unsigned int remainingAfterPartialBlock48 = partialBlock48 ? 0u : remainingAfterFullBlocks48;

	constexpr unsigned int blocks24 = remainingAfterPartialBlock48 / 24u;

	constexpr unsigned int remainingAfterPartialBlock24 = remainingAfterPartialBlock48 % 24u;

	constexpr unsigned int blocks21 = remainingAfterPartialBlock24 / 21u;

	constexpr unsigned int remainingAfterPartialBlock21 = remainingAfterPartialBlock24 % 21u;

	constexpr unsigned int blocks15 = remainingAfterPartialBlock21 / 15u;

	constexpr unsigned int remainingAfterPartialBlock15 = remainingAfterPartialBlock21 % 15u;

	constexpr unsigned int blocks1 = remainingAfterPartialBlock15;

	static_assert(blocks1 % 3u == 0u, "Invalid number of single blocks");

	static_assert(std::is_same<short, int16_t>::value, "Invalid data type!");

	const __m128i constant_signs_m128i = _mm_set1_epi16(short(0x1FF)); // -1, 1, -1, 1, -1, 1, -1, 1

	const __m128i mean0_0_128i = _mm_set1_epi8(meanValues0[0]);
	const __m128i mean0_1_128i = _mm_set1_epi8(meanValues0[1]);
	const __m128i mean0_2_128i = _mm_set1_epi8(meanValues0[2]);

	const __m128i mean1_0_128i = _mm_set1_epi8(meanValues1[0]);
	const __m128i mean1_1_128i = _mm_set1_epi8(meanValues1[1]);
	const __m128i mean1_2_128i = _mm_set1_epi8(meanValues1[2]);

	__m128i sum0_128i = _mm_setzero_si128();
	__m128i sum1_128i = _mm_setzero_si128();

	uint32_t sumIndividual = 0u;

	for (unsigned int y = 0u; y < tPatchSize; ++y)
	{
		SSE::prefetchT0(patch0 + patch0StrideElements);
		SSE::prefetchT0(patch1 + patch1StrideElements);

		for (unsigned int n = 0u; n < blocks48; ++n)
		{
			const __m128i buffer0A_128i = _mm_lddqu_si128((const __m128i*)(patch0 + 0));
			const __m128i buffer0B_128i = _mm_lddqu_si128((const __m128i*)(patch0 + 16));
			const __m128i buffer0C_128i = _mm_lddqu_si128((const __m128i*)(patch0 + 32));

			__m128i channel0_0_128i;
			__m128i channel0_1_128i;
			__m128i channel0_2_128i;
			SSE::deInterleave3Channel8Bit48Elements(buffer0A_128i, buffer0B_128i, buffer0C_128i, channel0_0_128i, channel0_1_128i, channel0_2_128i);

			const __m128i buffer1A_128i = _mm_lddqu_si128((const __m128i*)(patch1 + 0));
			const __m128i buffer1B_128i = _mm_lddqu_si128((const __m128i*)(patch1 + 16));
			const __m128i buffer1C_128i = _mm_lddqu_si128((const __m128i*)(patch1 + 32));

			__m128i channel1_0_128i;
			__m128i channel1_1_128i;
			__m128i channel1_2_128i;
			SSE::deInterleave3Channel8Bit48Elements(buffer1A_128i, buffer1B_128i, buffer1C_128i, channel1_0_128i, channel1_1_128i, channel1_2_128i);

			__m128i absDifferencesLow_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_0_128i, channel0_0_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_0_128i, channel1_0_128i), constant_signs_m128i)); // (channel0_0 - mean0) - (channel1_0 - mean1)
			__m128i absDifferencesHigh_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpackhi_epi8(mean0_0_128i, channel0_0_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpackhi_epi8(mean1_0_128i, channel1_0_128i), constant_signs_m128i));

			sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i)); // sum0_128i += absDifferencesLow_128i * absDifferencesLow_128i
			sum1_128i = _mm_add_epi32(sum1_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

			absDifferencesLow_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_1_128i, channel0_1_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_1_128i, channel1_1_128i), constant_signs_m128i));
			absDifferencesHigh_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpackhi_epi8(mean0_1_128i, channel0_1_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpackhi_epi8(mean1_1_128i, channel1_1_128i), constant_signs_m128i));

			sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
			sum1_128i = _mm_add_epi32(sum1_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

			absDifferencesLow_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_2_128i, channel0_2_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_2_128i, channel1_2_128i), constant_signs_m128i));
			absDifferencesHigh_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpackhi_epi8(mean0_2_128i, channel0_2_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpackhi_epi8(mean1_2_128i, channel1_2_128i), constant_signs_m128i));

			sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
			sum1_128i = _mm_add_epi32(sum1_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

			patch0 += 48;
			patch1 += 48;
		}

		if constexpr (partialBlock48)
		{
			constexpr int overlappingElements = int(48u - remainingAfterFullBlocks48);
			constexpr int overlappingPixels = overlappingElements / int(tChannels);

			const __m128i buffer0A_128i = _mm_slli_si128(_mm_lddqu_si128((const __m128i*)(patch0)), overlappingElements);
			const __m128i buffer0B_128i = _mm_lddqu_si128((const __m128i*)(patch0 - overlappingElements + 16));
			const __m128i buffer0C_128i = _mm_lddqu_si128((const __m128i*)(patch0 - overlappingElements + 32));

			__m128i channel0_0_128i;
			__m128i channel0_1_128i;
			__m128i channel0_2_128i;
			SSE::deInterleave3Channel8Bit48Elements(buffer0A_128i, buffer0B_128i, buffer0C_128i, channel0_0_128i, channel0_1_128i, channel0_2_128i);

			const __m128i buffer1A_128i = _mm_slli_si128(_mm_lddqu_si128((const __m128i*)(patch1)), overlappingElements);
			const __m128i buffer1B_128i = _mm_lddqu_si128((const __m128i*)(patch1 - overlappingElements + 16));
			const __m128i buffer1C_128i = _mm_lddqu_si128((const __m128i*)(patch1 - overlappingElements + 32));

			__m128i channel1_0_128i;
			__m128i channel1_1_128i;
			__m128i channel1_2_128i;
			SSE::deInterleave3Channel8Bit48Elements(buffer1A_128i, buffer1B_128i, buffer1C_128i, channel1_0_128i, channel1_1_128i, channel1_2_128i);

			__m128i absDifferencesLow_128i = _mm_srli_si128(_mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_0_128i, channel0_0_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_0_128i, channel1_0_128i), constant_signs_m128i)), overlappingPixels * 2); // (channel0_0 - mean0) - (channel1_0 - mean1)
			__m128i absDifferencesHigh_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpackhi_epi8(mean0_0_128i, channel0_0_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpackhi_epi8(mean1_0_128i, channel1_0_128i), constant_signs_m128i));

			sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i)); // sum0_128i += absDifferencesLow_128i * absDifferencesLow_128i
			sum1_128i = _mm_add_epi32(sum1_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

			absDifferencesLow_128i = _mm_srli_si128(_mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_1_128i, channel0_1_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_1_128i, channel1_1_128i), constant_signs_m128i)), overlappingPixels * 2);
			absDifferencesHigh_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpackhi_epi8(mean0_1_128i, channel0_1_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpackhi_epi8(mean1_1_128i, channel1_1_128i), constant_signs_m128i));

			sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
			sum1_128i = _mm_add_epi32(sum1_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

			absDifferencesLow_128i = _mm_srli_si128(_mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_2_128i, channel0_2_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_2_128i, channel1_2_128i), constant_signs_m128i)), overlappingPixels * 2);
			absDifferencesHigh_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpackhi_epi8(mean0_2_128i, channel0_2_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpackhi_epi8(mean1_2_128i, channel1_2_128i), constant_signs_m128i));

			sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
			sum1_128i = _mm_add_epi32(sum1_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

			patch0 += remainingAfterFullBlocks48;
			patch1 += remainingAfterFullBlocks48;
		}

		for (unsigned int n = 0u; n < blocks24; ++n)
		{
			const __m128i buffer0A_128i = _mm_lddqu_si128((const __m128i*)(patch0 + 0));
			const __m128i buffer0B_128i = _mm_loadl_epi64((const __m128i*)(patch0 + 16)); // load for unaligned 64 bit memory

			__m128i channel0_01_128i;
			__m128i channel0_2_128i;
			SSE::deInterleave3Channel8Bit24Elements(buffer0A_128i, buffer0B_128i, channel0_01_128i, channel0_2_128i);

			const __m128i buffer1A_128i = _mm_lddqu_si128((const __m128i*)(patch1 + 0));
			const __m128i buffer1B_128i = _mm_loadl_epi64((const __m128i*)(patch1 + 16)); // load for unaligned 64 bit memory

			__m128i channel1_01_128i;
			__m128i channel1_2_128i;
			SSE::deInterleave3Channel8Bit24Elements(buffer1A_128i, buffer1B_128i, channel1_01_128i, channel1_2_128i);

			__m128i absDifferencesLow_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_0_128i, channel0_01_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_0_128i, channel1_01_128i), constant_signs_m128i)); // (channel0_01 - mean0) - (channel1_01 - mean1)
			__m128i absDifferencesHigh_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpackhi_epi8(mean0_1_128i, channel0_01_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpackhi_epi8(mean1_1_128i, channel1_01_128i), constant_signs_m128i));

			sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i)); // sum0_128i += absDifferencesLow_128i * absDifferencesLow_128i
			sum1_128i = _mm_add_epi32(sum1_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

			absDifferencesLow_128i = _mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_2_128i, channel0_2_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_2_128i, channel1_2_128i), constant_signs_m128i));

			sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i)); // sum0_128i += absDifferencesLow_128i * absDifferencesLow_128i

			patch0 += 24;
			patch1 += 24;
		}

		for (unsigned int n = 0u; n < blocks21; ++n)
		{
			const __m128i buffer0A_128i = _mm_lddqu_si128((const __m128i*)(patch0 + 0));
			const __m128i buffer0B_128i = _mm_srli_si128(_mm_loadl_epi64((const __m128i*)(patch0 + 16 - 3)), 3); // load for unaligned 64 bit memory

			__m128i channel0_01_128i;
			__m128i channel0_2_128i;
			SSE::deInterleave3Channel8Bit24Elements(buffer0A_128i, buffer0B_128i, channel0_01_128i, channel0_2_128i);

			const __m128i buffer1A_128i = _mm_lddqu_si128((const __m128i*)(patch1 + 0));
			const __m128i buffer1B_128i = _mm_srli_si128(_mm_loadl_epi64((const __m128i*)(patch1 + 16 - 3)), 3); // load for unaligned 64 bit memory

			__m128i channel1_01_128i;
			__m128i channel1_2_128i;
			SSE::deInterleave3Channel8Bit24Elements(buffer1A_128i, buffer1B_128i, channel1_01_128i, channel1_2_128i);

			__m128i absDifferencesLow_128i = _mm_slli_si128(_mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_0_128i, channel0_01_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_0_128i, channel1_01_128i), constant_signs_m128i)), 2); // (channel0_01 - mean0) - (channel1_01 - mean1)
			__m128i absDifferencesHigh_128i = _mm_slli_si128(_mm_sub_epi16(_mm_maddubs_epi16(_mm_unpackhi_epi8(mean0_1_128i, channel0_01_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpackhi_epi8(mean1_1_128i, channel1_01_128i), constant_signs_m128i)), 2);

			sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i)); // sum0_128i += absDifferencesLow_128i * absDifferencesLow_128i
			sum1_128i = _mm_add_epi32(sum1_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

			absDifferencesLow_128i = _mm_slli_si128(_mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_2_128i, channel0_2_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_2_128i, channel1_2_128i), constant_signs_m128i)), 2);

			sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i)); // sum0_128i += absDifferencesLow_128i * absDifferencesLow_128i

			patch0 += 21;
			patch1 += 21;
		}

		for (unsigned int n = 0u; n < blocks15; ++n)
		{
			const __m128i buffer0_128i = y < tPatchSize - 1u ? _mm_lddqu_si128((const __m128i*)(patch0)) : _mm_srli_si128(_mm_lddqu_si128((const __m128i*)(patch0 - 1)), 1);

			__m128i channel0_01_128i;
			__m128i channel0_2_128i;
			SSE::deInterleave3Channel8Bit15Elements(buffer0_128i, channel0_01_128i, channel0_2_128i);

			const __m128i buffer1_128i = y < tPatchSize - 1u ? _mm_lddqu_si128((const __m128i*)(patch1)) : _mm_srli_si128(_mm_lddqu_si128((const __m128i*)(patch1 - 1)), 1);

			__m128i channel1_01_128i;
			__m128i channel1_2_128i;
			SSE::deInterleave3Channel8Bit15Elements(buffer1_128i, channel1_01_128i, channel1_2_128i);

			__m128i absDifferencesLow_128i = _mm_slli_si128(_mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_0_128i, channel0_01_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_0_128i, channel1_01_128i), constant_signs_m128i)), 6); // (channel0_01 - mean0) - (channel1_01 - mean1)
			__m128i absDifferencesHigh_128i = _mm_slli_si128(_mm_sub_epi16(_mm_maddubs_epi16(_mm_unpackhi_epi8(mean0_1_128i, channel0_01_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpackhi_epi8(mean1_1_128i, channel1_01_128i), constant_signs_m128i)), 6);

			sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i)); // sum0_128i += absDifferencesLow_128i * absDifferencesLow_128i
			sum1_128i = _mm_add_epi32(sum1_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

			absDifferencesLow_128i = _mm_slli_si128(_mm_sub_epi16(_mm_maddubs_epi16(_mm_unpacklo_epi8(mean0_2_128i, channel0_2_128i), constant_signs_m128i), _mm_maddubs_epi16(_mm_unpacklo_epi8(mean1_2_128i, channel1_2_128i), constant_signs_m128i)), 6);

			sum0_128i = _mm_add_epi32(sum0_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i)); // sum0_128i += absDifferencesLow_128i * absDifferencesLow_128i

			patch0 += 15;
			patch1 += 15;
		}

		if constexpr (blocks1 != 0u)
		{
			constexpr unsigned int pixels = blocks1 / 3u;

			for (unsigned int x = 0u; x < pixels; ++x)
			{
				for (unsigned int n = 0u; n < 3u; ++n)
				{
					sumIndividual += sqrDistance(patch0[x * 3u + n] - meanValues0[n], patch1[x * 3u + n] - meanValues1[n]);
				}
			}

			patch0 += blocks1;
			patch1 += blocks1;
		}

		patch0 += patch0StrideElements - patchWidthElements;
		patch1 += patch1StrideElements - patchWidthElements;
	}

	return SSE::sum_u32_4(sum0_128i) + SSE::sum_u32_4(sum1_128i) + sumIndividual;
}

template <unsigned int tChannels>
template <unsigned int tPatchSize>
inline uint32_t ZeroMeanSumSquareDifferencesSSE::SpecializedForChannels<tChannels>::patch8BitPerChannel(const uint8_t* patch0, const uint8_t* patch1, const unsigned int patch0StrideElements, const unsigned int patch1StrideElements, const uint8_t* const meanValues0, const uint8_t* const meanValues1)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize >= 1u, "Invalid patch size!");

	ocean_assert(patch0 != nullptr && patch1 != nullptr);
	ocean_assert(meanValues0 != nullptr && meanValues1 != nullptr);

	ocean_assert(patch0StrideElements >= tChannels * tPatchSize);
	ocean_assert(patch1StrideElements >= tChannels * tPatchSize);

	uint32_t ssd = 0u;

	for (unsigned int y = 0u; y < tPatchSize; ++y)
	{
		for (unsigned int x = 0u; x < tPatchSize; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				ssd += sqrDistance(patch0[x * tChannels + n] - meanValues0[n], patch1[x * tChannels + n] - meanValues1[n]);
			}
		}

		patch0 += patch0StrideElements;
		patch1 += patch1StrideElements;
	}

	return ssd;
}

template <unsigned int tChannels, unsigned int tPixels>
inline uint32_t ZeroMeanSumSquareDifferencesSSE::buffer8BitPerChannel(const uint8_t* const buffer0, const uint8_t* const buffer1)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPixels >= 8u, "Invalid patch size!");

	ocean_assert(buffer0 != nullptr && buffer1 != nullptr);

	uint8_t meanValues0[tChannels];
	mean8BitPerChannel<tChannels, tPixels>(buffer0, meanValues0);

	uint8_t meanValues1[tChannels];
	mean8BitPerChannel<tChannels, tPixels>(buffer1, meanValues1);

	return SpecializedForChannels<tChannels>::template buffer8BitPerChannel<tPixels>(buffer0, buffer1, meanValues0, meanValues1);
}

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t ZeroMeanSumSquareDifferencesSSE::patch8BitPerChannel(const uint8_t* const patch0, const uint8_t* const patch1, const unsigned int patch0StrideElements, const unsigned int patch1StrideElements)
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
inline uint32_t ZeroMeanSumSquareDifferencesSSE::patchBuffer8BitPerChannel(const uint8_t* const patch0, const uint8_t* const buffer1, const unsigned int patch0StrideElements)
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

template <unsigned int tChannels, unsigned int tPixels>
OCEAN_FORCE_INLINE void ZeroMeanSumSquareDifferencesSSE::mean8BitPerChannel(const uint8_t* const buffer, uint8_t* const meanValues)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPixels >= 8u, "Invalid patch size!");

	SpecializedForChannels<tChannels>::template mean8BitPerChannel<tPixels>(buffer, meanValues);
}

template <unsigned int tChannels, unsigned int tPatchSize>
OCEAN_FORCE_INLINE void ZeroMeanSumSquareDifferencesSSE::mean8BitPerChannel(const uint8_t* const patch, const unsigned int patchStrideElements, uint8_t* const meanValues)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize >= 5u, "Invalid patch size!");

	SpecializedForChannels<tChannels>::template mean8BitPerChannel<tPatchSize>(patch, patchStrideElements, meanValues);
}

}

}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 41

#endif // META_OCEAN_CV_ZERO_MEAN_SUM_SQUARE_DIFFERENCES_SSE_H
