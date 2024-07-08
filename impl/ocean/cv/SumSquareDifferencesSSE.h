/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SUM_SQUARE_DIFFERENCES_SSE_H
#define META_OCEAN_CV_SUM_SQUARE_DIFFERENCES_SSE_H

#include "ocean/cv/CV.h"

#include "ocean/base/Utilities.h"

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

#include "ocean/cv/SSE.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements function to calculate sum square differences using SSE instructions.
 * @ingroup cv
 */
class SumSquareDifferencesSSE
{
	public:

		/**
		 * Returns the sum of square differences between two memory buffers.
		 * @param buffer0 The first memory buffer, must be valid
		 * @param buffer1 The second memory buffer, must be valid
		 * @return The resulting sum of square differences
		 * @tparam tSize The size of the buffers in elements, with range [1, infinity)
		 */
		template <unsigned int tSize>
		static inline uint32_t buffer8BitPerChannel(const uint8_t* buffer0, const uint8_t* buffer1);

		/**
		 * Returns the sum of square differences between two patches within an image.
		 * @param patch0 The top left start position of the first image patch, must be valid
		 * @param patch1 The top left start position of the second image patch, must be valid
		 * @param patch0StrideElements The number of elements between two rows for the first patch, in elements, with range [tChannels, tPatchSize, infinity)
		 * @param patch1StrideElements The number of elements between two rows for the second patch, in elements, with range [tChannels, tPatchSize, infinity)
		 * @return The resulting sum of square differences
		 * @tparam tChannels The number of channels for the given frames, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static inline uint32_t patch8BitPerChannel(const uint8_t* patch0, const uint8_t* patch1, const unsigned int patch0StrideElements, const unsigned int patch1StrideElements);

		/**
		 * Returns the sum of square differences between an image patch and a buffer.
		 * @param patch0 The top left start position of the image patch, must be valid
		 * @param buffer1 The memory buffer, must be valid
		 * @param patch0StrideElements The number of elements between two rows for the image patch, in elements, with range [tChannels, tPatchSize, infinity)
		 * @return The resulting sum of square differences
		 * @tparam tChannels The number of channels for the given frames, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static inline uint32_t patchBuffer8BitPerChannel(const uint8_t* patch0, const uint8_t* buffer1, const unsigned int patch0StrideElements);
};

template <unsigned int tSize>
inline uint32_t SumSquareDifferencesSSE::buffer8BitPerChannel(const uint8_t* buffer0, const uint8_t* buffer1)
{
	static_assert(tSize >= 1u, "Invalid buffer size!");

	static_assert(std::is_same<short, int16_t>::value, "Invalid data type!");

	const __m128i constant_signs_m128i = _mm_set1_epi16(short(0x1FF)); // -1, 1, -1, 1, -1, 1, -1, 1

	__m128i sumLow_128i = _mm_setzero_si128();
	__m128i sumHigh_128i = _mm_setzero_si128();

	// first, we handle blocks with 16 elements

	constexpr unsigned int blocks16 = tSize / 16u;

	for (unsigned int n = 0u; n < blocks16; ++n)
	{
		const __m128i buffer0_128i = _mm_lddqu_si128((const __m128i*)buffer0);
		const __m128i buffer1_128i = _mm_lddqu_si128((const __m128i*)buffer1);

		const __m128i absDifferencesLow_128i = _mm_maddubs_epi16(_mm_unpacklo_epi8(buffer0_128i, buffer1_128i), constant_signs_m128i);
		const __m128i absDifferencesHigh_128i = _mm_maddubs_epi16(_mm_unpackhi_epi8(buffer0_128i, buffer1_128i), constant_signs_m128i);

		sumLow_128i = _mm_add_epi32(sumLow_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
		sumHigh_128i = _mm_add_epi32(sumHigh_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

		buffer0 += 16;
		buffer1 += 16;
	}

	if constexpr (blocks16 >= 1u && (tSize % 16u) >= 10u)
	{
		constexpr unsigned int remainingElements = tSize % 16u;
		constexpr unsigned int overlappingElements = 16u - remainingElements;

		const __m128i buffer0_128i = _mm_srli_si128(_mm_lddqu_si128((const __m128i*)(buffer0 - overlappingElements)), overlappingElements);
		const __m128i buffer1_128i = _mm_srli_si128(_mm_lddqu_si128((const __m128i*)(buffer1 - overlappingElements)), overlappingElements);

		const __m128i absDifferencesLow_128i = _mm_maddubs_epi16(_mm_unpacklo_epi8(buffer0_128i, buffer1_128i), constant_signs_m128i);
		const __m128i absDifferencesHigh_128i = _mm_maddubs_epi16(_mm_unpackhi_epi8(buffer0_128i, buffer1_128i), constant_signs_m128i);

		sumLow_128i = _mm_add_epi32(sumLow_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
		sumHigh_128i = _mm_add_epi32(sumHigh_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

		const __m128i sum_128i = _mm_add_epi32(sumLow_128i, sumHigh_128i);

		return SSE::sum_u32_4(sum_128i);
	}
	else
	{
		// we may handle at most one block with 8 elements

		constexpr unsigned int blocks8 = (tSize % 16u) / 8u;
		static_assert(blocks8 <= 1u, "Invalid number of blocks!");

		if constexpr (blocks8 == 1u)
		{
			const __m128i buffer0_128i = _mm_loadl_epi64((const __m128i*)buffer0); // load for unaligned 64 bit memory
			const __m128i buffer1_128i = _mm_loadl_epi64((const __m128i*)buffer1);

			const __m128i absDifferencesLow_128i = _mm_maddubs_epi16(_mm_unpacklo_epi8(buffer0_128i, buffer1_128i), constant_signs_m128i);

			sumLow_128i = _mm_add_epi32(sumLow_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));

			buffer0 += 8;
			buffer1 += 8;
		}

		const __m128i sum_128i = _mm_add_epi32(sumLow_128i, sumHigh_128i);

		constexpr unsigned int remainingElements = tSize - blocks16 * 16u - blocks8 * 8u;
		static_assert(remainingElements < 8u, "Invalid number of remaining elements!");

		uint32_t result = SSE::sum_u32_4(sum_128i);

		// we apply the remaining elements (at most 7)

		for (unsigned int n = 0u; n < remainingElements; ++n)
		{
			result += sqrDistance(buffer0[n], buffer1[n]);
		}

		return result;
	}
}

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t SumSquareDifferencesSSE::patch8BitPerChannel(const uint8_t* patch0, const uint8_t* patch1, const unsigned int patch0StrideElements, const unsigned int patch1StrideElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize >= 1u, "Invalid buffer size!");

	ocean_assert(patch0 != nullptr && patch1 != nullptr);

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

	__m128i sumLow_128i = _mm_setzero_si128();
	__m128i sumHigh_128i = _mm_setzero_si128();

	uint32_t sumIndividual = 0u;

	for (unsigned int y = 0u; y < tPatchSize; ++y)
	{
		SSE::prefetchT0(patch0 + patch0StrideElements);
		SSE::prefetchT0(patch1 + patch1StrideElements);

		for (unsigned int n = 0u; n < blocks16; ++n)
		{
			const __m128i buffer0_128i = _mm_lddqu_si128((const __m128i*)patch0);
			const __m128i buffer1_128i = _mm_lddqu_si128((const __m128i*)patch1);

			const __m128i absDifferencesLow_128i = _mm_maddubs_epi16(_mm_unpacklo_epi8(buffer0_128i, buffer1_128i), constant_signs_m128i);
			const __m128i absDifferencesHigh_128i = _mm_maddubs_epi16(_mm_unpackhi_epi8(buffer0_128i, buffer1_128i), constant_signs_m128i);

			sumLow_128i = _mm_add_epi32(sumLow_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
			sumHigh_128i = _mm_add_epi32(sumHigh_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

			patch0 += 16;
			patch1 += 16;
		}

		if constexpr (fullBlock8)
		{
			const __m128i buffer0_128i = _mm_loadl_epi64((const __m128i*)patch0); // load for unaligned 64 bit memory
			const __m128i buffer1_128i = _mm_loadl_epi64((const __m128i*)patch1);

			const __m128i absDifferencesLow_128i = _mm_maddubs_epi16(_mm_unpacklo_epi8(buffer0_128i, buffer1_128i), constant_signs_m128i);

			sumLow_128i = _mm_add_epi32(sumLow_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));

			patch0 += 8;
			patch1 += 8;
		}

		if constexpr (partialBlock16)
		{
			constexpr unsigned int overlapElements = partialBlock16 ? 16u - remainingAfterBlocks16 : 0u;

			static_assert(overlapElements < 8u, "Invalid value!");

			if (y < tPatchSize - 1u)
			{
				const __m128i buffer0_128i = _mm_slli_si128(_mm_lddqu_si128((const __m128i*)patch0), overlapElements); // loading 16 elements, but shifting `overlapElements` zeros to the left
				const __m128i buffer1_128i = _mm_slli_si128(_mm_lddqu_si128((const __m128i*)patch1), overlapElements);

				const __m128i absDifferencesLow_128i = _mm_maddubs_epi16(_mm_unpacklo_epi8(buffer0_128i, buffer1_128i), constant_signs_m128i);
				const __m128i absDifferencesHigh_128i = _mm_maddubs_epi16(_mm_unpackhi_epi8(buffer0_128i, buffer1_128i), constant_signs_m128i);

				sumLow_128i = _mm_add_epi32(sumLow_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
				sumHigh_128i = _mm_add_epi32(sumHigh_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));
			}
			else
			{
				const __m128i buffer0_128i = _mm_srli_si128(_mm_lddqu_si128((const __m128i*)(patch0 - overlapElements)), overlapElements); // loading 16 elements, but shifting `overlapElements` zeros to the right
				const __m128i buffer1_128i = _mm_srli_si128(_mm_lddqu_si128((const __m128i*)(patch1 - overlapElements)), overlapElements);

				const __m128i absDifferencesLow_128i = _mm_maddubs_epi16(_mm_unpacklo_epi8(buffer0_128i, buffer1_128i), constant_signs_m128i);
				const __m128i absDifferencesHigh_128i = _mm_maddubs_epi16(_mm_unpackhi_epi8(buffer0_128i, buffer1_128i), constant_signs_m128i);

				sumLow_128i = _mm_add_epi32(sumLow_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
				sumHigh_128i = _mm_add_epi32(sumHigh_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));
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
				const __m128i buffer0_128i = _mm_slli_si128(_mm_loadl_epi64((const __m128i*)patch0), overlapElements + 8); // loading 8 elements, but shifting `overlapElements` zeros to the left
				const __m128i buffer1_128i = _mm_slli_si128(_mm_loadl_epi64((const __m128i*)patch1), overlapElements + 8);

				const __m128i absDifferencesHigh_128i = _mm_maddubs_epi16(_mm_unpackhi_epi8(buffer0_128i, buffer1_128i), constant_signs_m128i);

				sumLow_128i = _mm_add_epi32(sumLow_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));
			}
			else
			{
				const __m128i buffer0_128i = _mm_srli_si128(_mm_loadl_epi64((const __m128i*)(patch0 - overlapElements)), overlapElements); // loading 8 elements, but shifting `overlapElements` zeros to the right
				const __m128i buffer1_128i = _mm_srli_si128(_mm_loadl_epi64((const __m128i*)(patch1 - overlapElements)), overlapElements);

				const __m128i absDifferencesLow_128i = _mm_maddubs_epi16(_mm_unpacklo_epi8(buffer0_128i, buffer1_128i), constant_signs_m128i);

				sumLow_128i = _mm_add_epi32(sumLow_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
			}

			patch0 += remainingAfterBlocks16;
			patch1 += remainingAfterBlocks16;
		}

		if constexpr (blocks1 != 0u)
		{
			for (unsigned int n = 0u; n < blocks1; ++n)
			{
				sumIndividual += sqrDistance(patch0[n], patch1[n]);
			}

			patch0 += blocks1;
			patch1 += blocks1;
		}

		patch0 += patch0StrideElements - patchWidthElements;
		patch1 += patch1StrideElements - patchWidthElements;
	}

	const __m128i sum_128i = _mm_add_epi32(sumLow_128i, sumHigh_128i);

	return SSE::sum_u32_4(sum_128i) + sumIndividual;
}

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t SumSquareDifferencesSSE::patchBuffer8BitPerChannel(const uint8_t* patch0, const uint8_t* buffer1, const unsigned int patch0StrideElements)
{
	return patch8BitPerChannel<tChannels, tPatchSize>(patch0, buffer1, patch0StrideElements, tChannels * tPatchSize);
}

}

}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 41

#endif // META_OCEAN_CV_SUM_SQUARE_DIFFERENCES_SSE_H
