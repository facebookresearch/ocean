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

	protected:

		/**
		 * Returns the mirrored element index for a given element index.
		 * The mirrored index is calculated as follows:
		 * <pre>
		 *                           |<----------------------- valid value range -------------------------->|
		 *
		 * elementIndex:   -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7,  ... elements - 3, elements - 2, elements - 1, elements + 0, elements + 1
		 * result:          2   1   0  0  1  2  3  4  5  6  7   ... elements - 3  elements - 2  elements - 1  elements - 1  elements - 2
		 * </pre>
		 * The resulting mirrored index is adjusted to support several channels.
		 * @param elementIndex The index for which the mirrored index will be returned, with range [-elements/2, elements + elements/2]
		 * @param elements The number of maximal elements, with range [1, infinity)
		 * @return The mirrored index, with range [0, elements)
		 * @tparam tChannels The number of channels the elements have, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static OCEAN_FORCE_INLINE unsigned int mirrorIndex(const int elementIndex, const unsigned int elements);

		/**
		 * Loads up to 8 uint8_t values from a row with mirroring pixels if necessary.
		 * @param row The row from which the values will be loaded, must be valid
		 * @param elementIndex The index of the first elements to load, with range [-elements/2, elements + elements/2]
		 * @param elements The number of elements in the row, with range [4, infinity)
		 * @param intermediateBuffer An intermediate buffer with 8 elements, must be valid
		 * @return The __m128i object with the loaded values (in the lower 64 bits)
		 * @tparam tChannels The number of channels the row has, with range [1, infinity)
		 * @tparam tFront True, if the uint8_t values will be placed at the front of the resulting __m128i object; False, to placed the uint8_t values at the end
		 * @tparam tSize The number of uint8_t values to be read, with range [1, 8]
		 */
		template <unsigned int tChannels, bool tFront, unsigned int tSize>
		static OCEAN_FORCE_INLINE __m128i loadMirrored_u_8x8(const uint8_t* const row, const int elementIndex, const unsigned int elements, uint8_t* const intermediateBuffer);

		/**
		 * Loads up to 16 uint8_t values from a row with mirroring pixels if necessary.
		 * @param row The row from which the values will be loaded, must be valid
		 * @param elementIndex The index of the first elements to load, with range [-elements/2, elements + elements/2]
		 * @param elements The number of elements in the row, with range [8, infinity)
		 * @param intermediateBuffer An intermediate buffer with 16 elements, must be valid
		 * @return The __m128i object with the loaded values
		 * @tparam tChannels The number of channels the row has, with range [1, infinity)
		 * @tparam tFront True, if the uint8_t values will be placed at the front of the resulting __m128i object; False, to placed the uint8_t values at the end
		 * @tparam tSize The number of uint8_t values to be read, with range [1, 16]
		 */
		template <unsigned int tChannels, bool tFront, unsigned int tSize>
		static OCEAN_FORCE_INLINE __m128i loadMirrored_u_8x16(const uint8_t* const row, const int elementIndex, const unsigned int elements, uint8_t* const intermediateBuffer);
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

template <unsigned int tChannels, unsigned int tPatchSize>
uint32_t SumSquareDifferencesSSE::patchMirroredBorder8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize % 2u == 1u, "Invalid patch size!");

	ocean_assert(image0 != nullptr && image1 != nullptr);

	ocean_assert(centerX0 < width0 && centerY0 < height0);
	ocean_assert(centerX1 < width1 && centerY1 < height1);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	const unsigned int width0Elements = width0 * tChannels;
	const unsigned int width1Elements = width1 * tChannels;

	const unsigned int image0StrideElements = width0Elements + image0PaddingElements;
	const unsigned int image1StrideElements = width1Elements + image1PaddingElements;

	constexpr unsigned int patchWidthElements = tChannels * tPatchSize;

	constexpr unsigned int blocks16 = patchWidthElements / 16u;
	constexpr unsigned int remainingAfterBlocks16 = patchWidthElements % 16u;

	constexpr bool partialBlock16 = remainingAfterBlocks16 > 8u;
	constexpr unsigned int remainingAfterPartialBlock16 = partialBlock16 ? 0u : remainingAfterBlocks16;

	constexpr unsigned int blocks8 = remainingAfterPartialBlock16 / 8u;
	constexpr unsigned int remainingAfterBlocks8 = remainingAfterPartialBlock16 % 8u;

	constexpr bool partialBlock8 = remainingAfterBlocks8 >= 3u;
	constexpr unsigned int remainingAfterPartialBlock8 = partialBlock8 ? 0u : remainingAfterBlocks8;

	constexpr unsigned int blocks1 = remainingAfterPartialBlock8;

	static_assert(blocks1 <= 7u, "Invalid block size!");

	static_assert(std::is_same<short, int16_t>::value, "Invalid data type!");

	const __m128i constant_signs_m128i = _mm_set1_epi16(short(0x1FF)); // -1, 1, -1, 1, -1, 1, -1, 1

	__m128i sumLow_128i = _mm_setzero_si128();
	__m128i sumHigh_128i = _mm_setzero_si128();

	uint32_t sumIndividual = 0u;

	uint8_t intermediate[16];

	int y1 = int(centerY1) - int(tPatchSize_2);
	for (int y0 = int(centerY0) - int(tPatchSize_2); y0 <= int(centerY0) + int(tPatchSize_2); ++y0)
	{
		const uint8_t* const mirroredRow0 = image0 + (unsigned int)(y0 + CVUtilities::mirrorOffset(y0, height0)) * image0StrideElements;
		const uint8_t* const mirroredRow1 = image1 + (unsigned int)(y1 + CVUtilities::mirrorOffset(y1, height1)) * image1StrideElements;

		int x0 = (int(centerX0) - int(tPatchSize_2)) * int(tChannels);
		int x1 = (int(centerX1) - int(tPatchSize_2)) * int(tChannels);

		for (unsigned int n = 0u; n < blocks16; ++n)
		{
			const __m128i buffer0_128i = loadMirrored_u_8x16<tChannels, true, 16u>(mirroredRow0, x0, width0Elements, intermediate);
			const __m128i buffer1_128i = loadMirrored_u_8x16<tChannels, true, 16u>(mirroredRow1, x1, width1Elements, intermediate);

			const __m128i absDifferencesLow_128i = _mm_maddubs_epi16(_mm_unpacklo_epi8(buffer0_128i, buffer1_128i), constant_signs_m128i);
			const __m128i absDifferencesHigh_128i = _mm_maddubs_epi16(_mm_unpackhi_epi8(buffer0_128i, buffer1_128i), constant_signs_m128i);

			sumLow_128i = _mm_add_epi32(sumLow_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
			sumHigh_128i = _mm_add_epi32(sumHigh_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));

			x0 += 16;
			x1 += 16;
		}

		if constexpr (partialBlock16)
		{
			if (y0 < int(centerY0) + int(tPatchSize_2))
			{
				const __m128i buffer0_128i = loadMirrored_u_8x16<tChannels, true, remainingAfterBlocks16>(mirroredRow0, x0, width0Elements, intermediate);
				const __m128i buffer1_128i = loadMirrored_u_8x16<tChannels, true, remainingAfterBlocks16>(mirroredRow1, x1, width1Elements, intermediate);

				const __m128i absDifferencesLow_128i = _mm_maddubs_epi16(_mm_unpacklo_epi8(buffer0_128i, buffer1_128i), constant_signs_m128i);
				const __m128i absDifferencesHigh_128i = _mm_maddubs_epi16(_mm_unpackhi_epi8(buffer0_128i, buffer1_128i), constant_signs_m128i);

				sumLow_128i = _mm_add_epi32(sumLow_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
				sumHigh_128i = _mm_add_epi32(sumHigh_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));
			}
			else
			{
				const __m128i buffer0_128i = loadMirrored_u_8x16<tChannels, false, remainingAfterBlocks16>(mirroredRow0, x0, width0Elements, intermediate);
				const __m128i buffer1_128i = loadMirrored_u_8x16<tChannels, false, remainingAfterBlocks16>(mirroredRow1, x1, width1Elements, intermediate);

				const __m128i absDifferencesLow_128i = _mm_maddubs_epi16(_mm_unpacklo_epi8(buffer0_128i, buffer1_128i), constant_signs_m128i);
				const __m128i absDifferencesHigh_128i = _mm_maddubs_epi16(_mm_unpackhi_epi8(buffer0_128i, buffer1_128i), constant_signs_m128i);

				sumLow_128i = _mm_add_epi32(sumLow_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
				sumHigh_128i = _mm_add_epi32(sumHigh_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));
			}

			x0 += remainingAfterBlocks16;
			x1 += remainingAfterBlocks16;
		}

		for (unsigned int n = 0u; n < blocks8; ++n)
		{
			const __m128i buffer0_128i = loadMirrored_u_8x8<tChannels, true, 8u>(mirroredRow0, x0, width0Elements, intermediate);
			const __m128i buffer1_128i = loadMirrored_u_8x8<tChannels, true, 8u>(mirroredRow1, x1, width1Elements, intermediate);

			const __m128i absDifferencesLow_128i = _mm_maddubs_epi16(_mm_unpacklo_epi8(buffer0_128i, buffer1_128i), constant_signs_m128i);

			sumLow_128i = _mm_add_epi32(sumLow_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));

			x0 += 8;
			x1 += 8;
		}

		if constexpr (partialBlock8)
		{
			// we have enough elements left so that using SSE is still faster than handling each element individually

			if (y0 < int(centerY0) + int(tPatchSize_2))
			{
				// Shift data to high bytes and use unpack_hi to ensure correct pairing for madd
				const __m128i loaded0_128i = loadMirrored_u_8x8<tChannels, true, remainingAfterBlocks8>(mirroredRow0, x0, width0Elements, intermediate);
				const __m128i loaded1_128i = loadMirrored_u_8x8<tChannels, true, remainingAfterBlocks8>(mirroredRow1, x1, width1Elements, intermediate);

				constexpr unsigned int shift = 8u + (8u - remainingAfterBlocks8);
				const __m128i remaining0_128i = _mm_slli_si128(loaded0_128i, shift);
				const __m128i remaining1_128i = _mm_slli_si128(loaded1_128i, shift);

				const __m128i absDifferencesHigh_128i = _mm_maddubs_epi16(_mm_unpackhi_epi8(remaining0_128i, remaining1_128i), constant_signs_m128i);

				sumLow_128i = _mm_add_epi32(sumLow_128i, _mm_madd_epi16(absDifferencesHigh_128i, absDifferencesHigh_128i));
			}
			else
			{
				// Data is in low bytes (after zero-padding at front), use unpack_lo
				const __m128i remaining0_128i = loadMirrored_u_8x8<tChannels, false, remainingAfterBlocks8>(mirroredRow0, x0, width0Elements, intermediate);
				const __m128i remaining1_128i = loadMirrored_u_8x8<tChannels, false, remainingAfterBlocks8>(mirroredRow1, x1, width1Elements, intermediate);

				const __m128i absDifferencesLow_128i = _mm_maddubs_epi16(_mm_unpacklo_epi8(remaining0_128i, remaining1_128i), constant_signs_m128i);

				sumLow_128i = _mm_add_epi32(sumLow_128i, _mm_madd_epi16(absDifferencesLow_128i, absDifferencesLow_128i));
			}

			x0 += remainingAfterBlocks8;
			x1 += remainingAfterBlocks8;
		}

		if constexpr (blocks1 != 0u)
		{
			for (unsigned int n = 0u; n < blocks1; ++n)
			{
				sumIndividual += sqrDistance(mirroredRow0[mirrorIndex<tChannels>(x0 + int(n), width0Elements)], mirroredRow1[mirrorIndex<tChannels>(x1 + int(n), width1Elements)]);
			}
		}

		++y1;
	}

	const __m128i sum_128i = _mm_add_epi32(sumLow_128i, sumHigh_128i);

	return SSE::sum_u32_4(sum_128i) + sumIndividual;
}

template <unsigned int tChannels>
inline unsigned int SumSquareDifferencesSSE::mirrorIndex(const int elementIndex, const unsigned int elements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	if ((unsigned int)(elementIndex) < elements)
	{
		return elementIndex;
	}

	if (elementIndex < 0)
	{
		const unsigned int leftElements = (unsigned int)(-elementIndex) - 1u;

		const unsigned int pixelIndex = leftElements / tChannels;
		const unsigned int channelIndex = tChannels - (leftElements % tChannels) - 1u;
		ocean_assert(channelIndex < tChannels);

		ocean_assert(pixelIndex * tChannels + channelIndex < elements);
		return pixelIndex * tChannels + channelIndex;
	}
	else
	{
		ocean_assert((unsigned int)(elementIndex) >= elements);

		const unsigned int rightElements = elementIndex - elements;

		const unsigned int rightPixels = rightElements / tChannels;
		const unsigned int channelIndex = rightElements % tChannels;
		ocean_assert(channelIndex < tChannels);

		ocean_assert(elements - (rightPixels + 1u) * tChannels + channelIndex < elements);
		return elements - (rightPixels + 1u) * tChannels + channelIndex;
	}
}

template <unsigned int tChannels, bool tFront, unsigned int tSize>
OCEAN_FORCE_INLINE __m128i SumSquareDifferencesSSE::loadMirrored_u_8x8(const uint8_t* const row, const int elementIndex, const unsigned int elements, uint8_t* const intermediateBuffer)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(tSize >= 1u && tSize <= 8u);

	ocean_assert(row != nullptr && intermediateBuffer != nullptr);

	constexpr unsigned int tOverlappingElements = 8u - tSize;

	if (elementIndex >= 0 && elementIndex <= int(elements) - int(tSize))
	{
		if constexpr (tSize == 8u)
		{
			return _mm_loadl_epi64((const __m128i*)(row + elementIndex));
		}
		else
		{
			if constexpr (tFront)
			{
				// For tFront=true, keep data at the front (low bytes), zero the high bytes
				// We load tSize bytes, they stay in the low bytes naturally
				for (unsigned int n = 0u; n < tSize; ++n)
				{
					intermediateBuffer[n] = row[elementIndex + n];
				}
				for (unsigned int n = tSize; n < 8u; ++n)
				{
					intermediateBuffer[n] = 0u;
				}
				return _mm_loadl_epi64((const __m128i*)intermediateBuffer);
			}
			else
			{
				// For tFront=false, put zeros at the front (low bytes), data at the back (high bytes)
				for (unsigned int n = 0u; n < tOverlappingElements; ++n)
				{
					intermediateBuffer[n] = 0u;
				}
				for (unsigned int n = 0u; n < tSize; ++n)
				{
					intermediateBuffer[tOverlappingElements + n] = row[elementIndex + n];
				}
				return _mm_loadl_epi64((const __m128i*)intermediateBuffer);
			}
		}
	}

	if constexpr (tFront)
	{
		for (unsigned int n = 0u; n < tSize; ++n)
		{
			const unsigned int index = mirrorIndex<tChannels>(elementIndex + int(n), elements);
			ocean_assert(index < elements);

			intermediateBuffer[n] = row[index];
		}

		for (unsigned int n = tSize; n < 8u; ++n)
		{
			intermediateBuffer[n] = 0u;
		}
	}
	else
	{
		for (unsigned int n = 0u; n < tOverlappingElements; ++n)
		{
			intermediateBuffer[n] = 0u;
		}

		for (unsigned int n = 0u; n < tSize; ++n)
		{
			const unsigned int index = mirrorIndex<tChannels>(elementIndex + int(n), elements);
			ocean_assert(index < elements);

			intermediateBuffer[tOverlappingElements + n] = row[index];
		}
	}

	return _mm_loadl_epi64((const __m128i*)intermediateBuffer);
}

template <unsigned int tChannels, bool tFront, unsigned int tSize>
OCEAN_FORCE_INLINE __m128i SumSquareDifferencesSSE::loadMirrored_u_8x16(const uint8_t* const row, const int elementIndex, const unsigned int elements, uint8_t* const intermediateBuffer)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(tSize > 8u && tSize <= 16u);

	ocean_assert(row != nullptr && intermediateBuffer != nullptr);

	constexpr unsigned int tOverlappingElements = 16u - tSize;

	if (elementIndex >= 0 && elementIndex <= int(elements) - int(tSize))
	{
		if constexpr (tSize == 16u)
		{
			return _mm_lddqu_si128((const __m128i*)(row + elementIndex));
		}
		else
		{
			if constexpr (tFront)
			{
				// For tFront=true, keep data at the front (low bytes), zero the high bytes
				for (unsigned int n = 0u; n < tSize; ++n)
				{
					intermediateBuffer[n] = row[elementIndex + n];
				}
				for (unsigned int n = tSize; n < 16u; ++n)
				{
					intermediateBuffer[n] = 0u;
				}
				return _mm_lddqu_si128((const __m128i*)intermediateBuffer);
			}
			else
			{
				// For tFront=false, put zeros at the front (low bytes), data at the back (high bytes)
				for (unsigned int n = 0u; n < tOverlappingElements; ++n)
				{
					intermediateBuffer[n] = 0u;
				}
				for (unsigned int n = 0u; n < tSize; ++n)
				{
					intermediateBuffer[tOverlappingElements + n] = row[elementIndex + n];
				}
				return _mm_lddqu_si128((const __m128i*)intermediateBuffer);
			}
		}
	}

	if constexpr (tFront)
	{
		for (unsigned int n = 0u; n < tSize; ++n)
		{
			const unsigned int index = mirrorIndex<tChannels>(elementIndex + int(n), elements);
			ocean_assert(index < elements);

			intermediateBuffer[n] = row[index];
		}

		for (unsigned int n = tSize; n < 16u; ++n)
		{
			intermediateBuffer[n] = 0u;
		}
	}
	else
	{
		for (unsigned int n = 0u; n < tOverlappingElements; ++n)
		{
			intermediateBuffer[n] = 0u;
		}

		for (unsigned int n = 0u; n < tSize; ++n)
		{
			const unsigned int index = mirrorIndex<tChannels>(elementIndex + int(n), elements);
			ocean_assert(index < elements);

			intermediateBuffer[tOverlappingElements + n] = row[index];
		}
	}

	return _mm_lddqu_si128((const __m128i*)intermediateBuffer);
}

}

}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 41

#endif // META_OCEAN_CV_SUM_SQUARE_DIFFERENCES_SSE_H
