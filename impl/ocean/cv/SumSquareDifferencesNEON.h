/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SUM_SQUARE_DIFFERENCES_NEON_H
#define META_OCEAN_CV_SUM_SQUARE_DIFFERENCES_NEON_H

#include "ocean/cv/CV.h"

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

#include "ocean/cv/NEON.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements function to calculate sum square differences using NEON instructions.
 * @ingroup cv
 */
class SumSquareDifferencesNEON
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
		 * @return The uint8x8_t object with the loaded values
		 * @tparam tChannels The number of channels the row has, with range [1, infinity)
		 * @tparam tFront True, if the uint8_t values will be placed at the front of the resulting uint8x8_t object; False, to placed the uint8_t values at the end
		 * @tparam tSize The number of uint8_t values to be read, with range [1, 8]
		 */
		template <unsigned int tChannels, bool tFront, unsigned int tSize>
		static OCEAN_FORCE_INLINE uint8x8_t loadMirrored_u_8x8(const uint8_t* const row, const int elementIndex, const unsigned int elements, uint8_t* const intermediateBuffer);

		/**
		 * Loads up to 16 uint8_t values from a row with mirroring pixels if necessary.
		 * @param row The row from which the values will be loaded, must be valid
		 * @param elementIndex The index of the first elements to load, with range [-elements/2, elements + elements/2]
		 * @param elements The number of elements in the row, with range [8, infinity)
		 * @param intermediateBuffer An intermediate buffer with 16 elements, must be valid
		 * @return The uint8x16_t object with the loaded values
		 * @tparam tChannels The number of channels the row has, with range [1, infinity)
		 * @tparam tFront True, if the uint8_t values will be placed at the front of the resulting uint8x8_t object; False, to placed the uint8_t values at the end
		 * @tparam tSize The number of uint8_t values to be read, with range [1, 16]
		 */
		template <unsigned int tChannels, bool tFront, unsigned int tSize>
		static OCEAN_FORCE_INLINE uint8x16_t loadMirrored_u_8x16(const uint8_t* const row, const int elementIndex, const unsigned int elements, uint8_t* const intermediateBuffer);
};

template <unsigned int tSize>
inline uint32_t SumSquareDifferencesNEON::buffer8BitPerChannel(const uint8_t* buffer0, const uint8_t* buffer1)
{
	static_assert(tSize >= 1u, "Invalid buffer size!");

	uint32x4_t sumA_u_32x4 = vdupq_n_u32(0u);
	uint32x4_t sumB_u_32x4 = vdupq_n_u32(0u);

	// first, we handle blocks with 16 elements

	constexpr unsigned int blocks16 = tSize / 16u;

	for (unsigned int n = 0u; n < blocks16; ++n)
	{
		// [|buffer0[0] - buffer1[0]|, |buffer0[1] - buffer1[1]|, ..]
		const uint8x16_t absDifference_u_8x16 = vabdq_u8(vld1q_u8(buffer0), vld1q_u8(buffer1));

		const uint8x8_t absDifferenceA_u_8x8 = vget_low_u8(absDifference_u_8x16);
		const uint8x8_t absDifferenceB_u_8x8 = vget_high_u8(absDifference_u_8x16);

		// sqrDifferenceA_u_16x8 = absDifferenceA_u_8x8 ^ 2
		const uint16x8_t sqrDifferenceA_u_16x8 = vmull_u8(absDifferenceA_u_8x8, absDifferenceA_u_8x8);
		const uint16x8_t sqrDifferenceB_u_16x8 = vmull_u8(absDifferenceB_u_8x8, absDifferenceB_u_8x8);

		sumA_u_32x4 = vpadalq_u16(sumA_u_32x4, sqrDifferenceA_u_16x8);
		sumB_u_32x4 = vpadalq_u16(sumB_u_32x4, sqrDifferenceB_u_16x8);

		buffer0 += 16;
		buffer1 += 16;
	}

	// we may handle at most one block with 8 elements

	constexpr unsigned int blocks8 = (tSize % 16u) / 8u;
	static_assert(blocks8 <= 1u, "Invalid number of blocks!");

	if (blocks8 == 1u)
	{
		// [|buffer0[0] - buffer1[0]|, |buffer0[1] - buffer1[1]|, ..]
		const uint8x8_t absDifference_u_8x8 = vabd_u8(vld1_u8(buffer0), vld1_u8(buffer1));

		// sqrDifferenceA_u_16x8 = absDifferenceA_u_8x8 ^ 2
		const uint16x8_t sqrDifference_u_16x8 = vmull_u8(absDifference_u_8x8, absDifference_u_8x8);

		sumA_u_32x4 = vpadalq_u16(sumA_u_32x4, sqrDifference_u_16x8);

		buffer0 += 8;
		buffer1 += 8;
	}

	const uint32x4_t sum_u_32x4 = vaddq_u32(sumA_u_32x4, sumB_u_32x4);

	uint32_t results[4];
	vst1q_u32(results, sum_u_32x4);

	constexpr unsigned int remainingElements = tSize - blocks16 * 16u - blocks8 * 8u;
	static_assert(remainingElements < 8u, "Invalid number of remaining elements!");

	uint32_t result = results[0] + results[1] + results[2] + results[3];

	// we apply the remaining elements (at most 7)

	for (unsigned int n = 0u; n < remainingElements; ++n)
	{
		result += sqrDistance(buffer0[n], buffer1[n]);
	}

	return result;
}

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t SumSquareDifferencesNEON::patch8BitPerChannel(const uint8_t* patch0, const uint8_t* patch1, const unsigned int patch0StrideElements, const unsigned int patch1StrideElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize >= 5u, "Invalid patch size!");

	ocean_assert(patch0 != nullptr && patch1 != nullptr);

	ocean_assert(patch0StrideElements >= tChannels * tPatchSize);
	ocean_assert(patch1StrideElements >= tChannels * tPatchSize);

	constexpr unsigned int patchWidthElements = tChannels * tPatchSize;

	constexpr unsigned int blocks16 = patchWidthElements / 16u;
	constexpr unsigned int blocks8 = (patchWidthElements - blocks16 * 16u) / 8u;
	constexpr unsigned int blocks1 = patchWidthElements - blocks16 * 16u - blocks8 * 8u;

	static_assert(blocks1 <= 7u, "Invalid block size!");

	const uint8x8_t maskRight_u_8x8 = vcreate_u8(uint64_t(-1) >> (8u - blocks1) * 8u);
	const uint8x8_t maskLeft_u_8x8 = vcreate_u8(uint64_t(-1) << (8u - blocks1) * 8u);

	uint32x4_t sumA_u_32x4 = vdupq_n_u32(0u);
	uint32x4_t sumB_u_32x4 = vdupq_n_u32(0u);

	uint32_t sumIndividual = 0u;

	for (unsigned int y = 0u; y < tPatchSize; ++y)
	{
		for (unsigned int n = 0u; n < blocks16; ++n)
		{
			// [|patch0[0] - patch1[0]|, |patch0[1] - patch1[1]|, ..]
			const uint8x16_t absDifference_u_8x16 = vabdq_u8(vld1q_u8(patch0), vld1q_u8(patch1));

			const uint8x8_t absDifferenceA_u_8x8 = vget_low_u8(absDifference_u_8x16);
			const uint8x8_t absDifferenceB_u_8x8 = vget_high_u8(absDifference_u_8x16);

			// sqrDifferenceA_u_16x8 = absDifferenceA_u_8x8 ^ 2
			const uint16x8_t sqrDifferenceA_u_16x8 = vmull_u8(absDifferenceA_u_8x8, absDifferenceA_u_8x8);
			const uint16x8_t sqrDifferenceB_u_16x8 = vmull_u8(absDifferenceB_u_8x8, absDifferenceB_u_8x8);

			sumA_u_32x4 = vpadalq_u16(sumA_u_32x4, sqrDifferenceA_u_16x8);
			sumB_u_32x4 = vpadalq_u16(sumB_u_32x4, sqrDifferenceB_u_16x8);

			patch0 += 16;
			patch1 += 16;
		}

		for (unsigned int n = 0u; n < blocks8; ++n)
		{
			// [|patch0[0] - patch1[0]|, |patch0[1] - patch1[1]|, ..]
			const uint8x8_t absDifference_u_8x8 = vabd_u8(vld1_u8(patch0), vld1_u8(patch1));

			// sqrDifferenceA_u_16x8 = absDifferenceA_u_8x8 ^ 2
			const uint16x8_t sqrDifference_u_16x8 = vmull_u8(absDifference_u_8x8, absDifference_u_8x8);

			sumA_u_32x4 = vpadalq_u16(sumA_u_32x4, sqrDifference_u_16x8);

			patch0 += 8;
			patch1 += 8;
		}

		if constexpr (blocks1 != 0u)
		{
			if (blocks1 >= 3u)
			{
				// we have enough elements left so that using NEON is still faster than handling each element individually

				if (y < tPatchSize - 1u)
				{
					const uint8x8_t remaining0_u_8x8 = vand_u8(vld1_u8(patch0), maskRight_u_8x8);
					const uint8x8_t remaining1_u_8x8 = vand_u8(vld1_u8(patch1), maskRight_u_8x8);

					const uint8x8_t absDifference_u_8x8 = vabd_u8(remaining0_u_8x8, remaining1_u_8x8);

					// sqrDifferenceA_u_16x8 = absDifferenceA_u_8x8 ^ 2
					const uint16x8_t sqrDifference_u_16x8 = vmull_u8(absDifference_u_8x8, absDifference_u_8x8);

					sumA_u_32x4 = vpadalq_u16(sumA_u_32x4, sqrDifference_u_16x8);
				}
				else
				{
					constexpr unsigned int overlapElements = 8u - blocks1;
					static_assert(overlapElements >= 1u && overlapElements < 8u, "Invalid number!");

					const uint8x8_t remaining0_u_8x8 = vand_u8(vld1_u8(patch0 - overlapElements), maskLeft_u_8x8);
					const uint8x8_t remaining1_u_8x8 = vand_u8(vld1_u8(patch1 - overlapElements), maskLeft_u_8x8);

					const uint8x8_t absDifference_u_8x8 = vabd_u8(remaining0_u_8x8, remaining1_u_8x8);

					// sqrDifferenceA_u_16x8 = absDifferenceA_u_8x8 ^ 2
					const uint16x8_t sqrDifference_u_16x8 = vmull_u8(absDifference_u_8x8, absDifference_u_8x8);

					sumA_u_32x4 = vpadalq_u16(sumA_u_32x4, sqrDifference_u_16x8);
				}
			}
			else
			{
				for (unsigned int n = 0u; n < blocks1; ++n)
				{
					sumIndividual += sqrDistance(patch0[n], patch1[n]);
				}
			}

			patch0 += blocks1;
			patch1 += blocks1;
		}

		patch0 += patch0StrideElements - patchWidthElements;
		patch1 += patch1StrideElements - patchWidthElements;
	}

	const uint32x4_t sum_u_32x4 = vaddq_u32(sumA_u_32x4, sumB_u_32x4);

	uint32_t results[4];
	vst1q_u32(results, sum_u_32x4);

	return results[0] + results[1] + results[2] + results[3] + sumIndividual;
}

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t SumSquareDifferencesNEON::patchBuffer8BitPerChannel(const uint8_t* patch0, const uint8_t* buffer1, const unsigned int patch0StrideElements)
{
	return patch8BitPerChannel<tChannels, tPatchSize>(patch0, buffer1, patch0StrideElements, tChannels * tPatchSize);
}

template <unsigned int tChannels, unsigned int tPatchSize>
uint32_t SumSquareDifferencesNEON::patchMirroredBorder8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
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

	constexpr bool partialBlock16 = remainingAfterBlocks16 > 10u;
	constexpr unsigned int remainingAfterPartialBlock16 = partialBlock16 ? 0u : remainingAfterBlocks16;

	constexpr unsigned int blocks8 = remainingAfterPartialBlock16 / 8u;
	constexpr unsigned int remainingAfterBlocks8 = remainingAfterPartialBlock16 % 8u;

	constexpr bool partialBlock8 = remainingAfterBlocks8 >= 3u;
	constexpr unsigned int remainingAfterPartialBlock8 = partialBlock8 ? 0u : remainingAfterBlocks8;

	constexpr unsigned int blocks1 = remainingAfterPartialBlock8;

	static_assert(blocks1 <= 7u, "Invalid block size!");

	uint32x4_t sumA_u_32x4 = vdupq_n_u32(0u);
	uint32x4_t sumB_u_32x4 = vdupq_n_u32(0u);

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
			// [|patch0[0] - patch1[0]|, |patch0[1] - patch1[1]|, ..]
			const uint8x16_t absDifference_u_8x16 = vabdq_u8(loadMirrored_u_8x16<tChannels, true, 16u>(mirroredRow0, x0, width0Elements, intermediate), loadMirrored_u_8x16<tChannels, true, 16u>(mirroredRow1, x1, width1Elements, intermediate));

			const uint8x8_t absDifferenceA_u_8x8 = vget_low_u8(absDifference_u_8x16);
			const uint8x8_t absDifferenceB_u_8x8 = vget_high_u8(absDifference_u_8x16);

			// sqrDifferenceA_u_16x8 = absDifferenceA_u_8x8 ^ 2
			const uint16x8_t sqrDifferenceA_u_16x8 = vmull_u8(absDifferenceA_u_8x8, absDifferenceA_u_8x8);
			const uint16x8_t sqrDifferenceB_u_16x8 = vmull_u8(absDifferenceB_u_8x8, absDifferenceB_u_8x8);

			sumA_u_32x4 = vpadalq_u16(sumA_u_32x4, sqrDifferenceA_u_16x8);
			sumB_u_32x4 = vpadalq_u16(sumB_u_32x4, sqrDifferenceB_u_16x8);

			x0 += 16;
			x1 += 16;
		}

		if constexpr (partialBlock16)
		{
			if (y0 < int(centerY0) + int(tPatchSize_2))
			{
				// [|patch0[0] - patch1[0]|, |patch0[1] - patch1[1]|, ..]
				const uint8x16_t absDifference_u_8x16 = vabdq_u8(loadMirrored_u_8x16<tChannels, true, remainingAfterBlocks16>(mirroredRow0, x0, width0Elements, intermediate), loadMirrored_u_8x16<tChannels, true, remainingAfterBlocks16>(mirroredRow1, x1, width1Elements, intermediate));

				const uint8x8_t absDifferenceA_u_8x8 = vget_low_u8(absDifference_u_8x16);
				const uint8x8_t absDifferenceB_u_8x8 = vget_high_u8(absDifference_u_8x16);

				// sqrDifferenceA_u_16x8 = absDifferenceA_u_8x8 ^ 2
				const uint16x8_t sqrDifferenceA_u_16x8 = vmull_u8(absDifferenceA_u_8x8, absDifferenceA_u_8x8);
				const uint16x8_t sqrDifferenceB_u_16x8 = vmull_u8(absDifferenceB_u_8x8, absDifferenceB_u_8x8);

				sumA_u_32x4 = vpadalq_u16(sumA_u_32x4, sqrDifferenceA_u_16x8);
				sumB_u_32x4 = vpadalq_u16(sumB_u_32x4, sqrDifferenceB_u_16x8);
			}
			else
			{
				// [|patch0[0] - patch1[0]|, |patch0[1] - patch1[1]|, ..]
				const uint8x16_t absDifference_u_8x16 = vabdq_u8(loadMirrored_u_8x16<tChannels, false, remainingAfterBlocks16>(mirroredRow0, x0, width0Elements, intermediate), loadMirrored_u_8x16<tChannels, false, remainingAfterBlocks16>(mirroredRow1, x1, width1Elements, intermediate));

				const uint8x8_t absDifferenceA_u_8x8 = vget_low_u8(absDifference_u_8x16);
				const uint8x8_t absDifferenceB_u_8x8 = vget_high_u8(absDifference_u_8x16);

				// sqrDifferenceA_u_16x8 = absDifferenceA_u_8x8 ^ 2
				const uint16x8_t sqrDifferenceA_u_16x8 = vmull_u8(absDifferenceA_u_8x8, absDifferenceA_u_8x8);
				const uint16x8_t sqrDifferenceB_u_16x8 = vmull_u8(absDifferenceB_u_8x8, absDifferenceB_u_8x8);

				sumA_u_32x4 = vpadalq_u16(sumA_u_32x4, sqrDifferenceA_u_16x8);
				sumB_u_32x4 = vpadalq_u16(sumB_u_32x4, sqrDifferenceB_u_16x8);
			}

			x0 += remainingAfterBlocks16;
			x1 += remainingAfterBlocks16;
		}

		for (unsigned int n = 0u; n < blocks8; ++n)
		{
			// [|patch0[0] - patch1[0]|, |patch0[1] - patch1[1]|, ..]
			const uint8x8_t absDifference_u_8x8 = vabd_u8(loadMirrored_u_8x8<tChannels, true, 8u>(mirroredRow0, x0, width0Elements, intermediate), loadMirrored_u_8x8<tChannels, true, 8u>(mirroredRow1, x1, width1Elements, intermediate));

			// sqrDifferenceA_u_16x8 = absDifferenceA_u_8x8 ^ 2
			const uint16x8_t sqrDifference_u_16x8 = vmull_u8(absDifference_u_8x8, absDifference_u_8x8);

			sumA_u_32x4 = vpadalq_u16(sumA_u_32x4, sqrDifference_u_16x8);

			x0 += 8;
			x1 += 8;
		}

		if constexpr (partialBlock8)
		{
			// we have enough elements left so that using NEON is still faster than handling each element individually

			if (y0 < int(centerY0) + int(tPatchSize_2))
			{
				const uint8x8_t remaining0_u_8x8 = loadMirrored_u_8x8<tChannels, true, remainingAfterBlocks8>(mirroredRow0, x0, width0Elements, intermediate);
				const uint8x8_t remaining1_u_8x8 = loadMirrored_u_8x8<tChannels, true, remainingAfterBlocks8>(mirroredRow1, x1, width1Elements, intermediate);

				const uint8x8_t absDifference_u_8x8 = vabd_u8(remaining0_u_8x8, remaining1_u_8x8);

				// sqrDifferenceA_u_16x8 = absDifferenceA_u_8x8 ^ 2
				const uint16x8_t sqrDifference_u_16x8 = vmull_u8(absDifference_u_8x8, absDifference_u_8x8);

				sumA_u_32x4 = vpadalq_u16(sumA_u_32x4, sqrDifference_u_16x8);
			}
			else
			{
				const uint8x8_t remaining0_u_8x8 = loadMirrored_u_8x8<tChannels, false, remainingAfterBlocks8>(mirroredRow0, x0, width0Elements, intermediate);
				const uint8x8_t remaining1_u_8x8 = loadMirrored_u_8x8<tChannels, false, remainingAfterBlocks8>(mirroredRow1, x1, width1Elements, intermediate);

				const uint8x8_t absDifference_u_8x8 = vabd_u8(remaining0_u_8x8, remaining1_u_8x8);

				// sqrDifferenceA_u_16x8 = absDifferenceA_u_8x8 ^ 2
				const uint16x8_t sqrDifference_u_16x8 = vmull_u8(absDifference_u_8x8, absDifference_u_8x8);

				sumA_u_32x4 = vpadalq_u16(sumA_u_32x4, sqrDifference_u_16x8);
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

	const uint32x4_t sum_u_32x4 = vaddq_u32(sumA_u_32x4, sumB_u_32x4);

	uint32_t results[4];
	vst1q_u32(results, sum_u_32x4);

	return results[0] + results[1] + results[2] + results[3] + sumIndividual;
}

template <unsigned int tChannels>
inline unsigned int SumSquareDifferencesNEON::mirrorIndex(const int elementIndex, const unsigned int elements)
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
		ocean_assert(elementIndex >= elements);

		const unsigned int rightElements = elementIndex - elements;

		const unsigned int rightPixels = rightElements / tChannels;
		const unsigned int channelIndex = rightElements % tChannels;
		ocean_assert(channelIndex < tChannels);

		ocean_assert(elements - (rightPixels + 1u) * tChannels + channelIndex < elements);
		return elements - (rightPixels + 1u) * tChannels + channelIndex;
	}
}

template <unsigned int tChannels, bool tFront, unsigned int tSize>
OCEAN_FORCE_INLINE uint8x8_t SumSquareDifferencesNEON::loadMirrored_u_8x8(const uint8_t* const row, const int elementIndex, const unsigned int elements, uint8_t* const intermediateBuffer)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(tSize >= 1u && tSize <= 8u);

	ocean_assert(row != nullptr && intermediateBuffer != nullptr);

	constexpr unsigned int tOverlappingElements = 8u - tSize;

	if (elementIndex >= 0 && elementIndex <= int(elements) - int(tSize))
	{
		if constexpr (tSize == 8u)
		{
			return vld1_u8(row + elementIndex);
		}
		else
		{
			if constexpr (tFront)
			{
				constexpr uint64_t mask = tOverlappingElements < 8u ? (uint64_t(-1) >> tOverlappingElements * 8u) : uint64_t(0);
				const uint8x8_t mask_u_8x8 = vcreate_u8(mask);

				return vand_u8(vld1_u8(row + elementIndex), mask_u_8x8);
			}
			else
			{
				constexpr uint64_t mask = tOverlappingElements < 8u ? (uint64_t(-1) << tOverlappingElements * 8u) : uint64_t(0);
				const uint8x8_t mask_u_8x8 = vcreate_u8(mask);

				return vand_u8(vld1_u8(row + elementIndex - int(tOverlappingElements)), mask_u_8x8);
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

	return vld1_u8(intermediateBuffer);
}

template <unsigned int tChannels, bool tFront, unsigned int tSize>
OCEAN_FORCE_INLINE uint8x16_t SumSquareDifferencesNEON::loadMirrored_u_8x16(const uint8_t* const row, const int elementIndex, const unsigned int elements, uint8_t* const intermediateBuffer)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(tSize > 8u && tSize <= 16u);

	ocean_assert(row != nullptr && intermediateBuffer != nullptr);

	constexpr unsigned int tOverlappingElements = 16u - tSize;

	if (elementIndex >= 0 && elementIndex <= int(elements) - int(tSize))
	{
		if constexpr (tSize == 16u)
		{
			return vld1q_u8(row + elementIndex);
		}
		else
		{
			if constexpr (tFront)
			{
				constexpr uint64_t maskHigh = tOverlappingElements < 8u ? (uint64_t(-1) >> tOverlappingElements * 8u) : uint64_t(0);
				const uint8x16_t mask_u_8x16 = vcombine_u8(vdup_n_u8(uint8_t(0xFFu)), vcreate_u8(maskHigh));

				return vandq_u8(vld1q_u8(row + elementIndex), mask_u_8x16);
			}
			else
			{
				constexpr uint64_t maskLow = tOverlappingElements < 8u ? (uint64_t(-1) << tOverlappingElements * 8u) : uint64_t(0);
				const uint8x16_t mask_u_8x16 = vcombine_u8(vcreate_u8(maskLow), vdup_n_u8(uint8_t(0xFFu)));

				return vandq_u8(vld1q_u8(row + elementIndex - int(tOverlappingElements)), mask_u_8x16);
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

	return vld1q_u8(intermediateBuffer);
}

}

}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

#endif // META_OCEAN_CV_SUM_SQUARE_DIFFERENCES_NEON_H
