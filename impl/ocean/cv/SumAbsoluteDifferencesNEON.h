/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SUM_ABSOLUTE_DIFFERENCES_NEON_H
#define META_OCEAN_CV_SUM_ABSOLUTE_DIFFERENCES_NEON_H

#include "ocean/cv/CV.h"

#include "ocean/base/Utilities.h"

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

#include "ocean/cv/NEON.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements functions calculation the sum of absolute differences with NEON instructions.
 * @ingroup cv
 */
class SumAbsoluteDifferencesNEON
{
	public:

		/**
		 * Returns the sum of absolute differences between two memory buffers.
		 * @param buffer0 The first memory buffer, must be valid
		 * @param buffer1 The second memory buffer, must be valid
		 * @return The resulting sum of square differences
		 * @tparam tSize The size of the buffers in elements, with range [1, infinity)
		 */
		template <unsigned int tSize>
		static inline uint32_t buffer8BitPerChannel(const uint8_t* buffer0, const uint8_t* buffer1);

		/**
		 * Returns the sum of absolute differences between two patches within an image.
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
		 * Returns the sum of absolute differences between an image patch and a buffer.
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
inline uint32_t SumAbsoluteDifferencesNEON::buffer8BitPerChannel(const uint8_t* buffer0, const uint8_t* buffer1)
{
	static_assert(tSize >= 1u, "Invalid buffer size!");

	uint32x4_t sum_u_32x4 = vdupq_n_u32(0u);

	// first, we handle blocks with 16 elements

	constexpr unsigned int blocks16 = tSize / 16u;

	for (unsigned int n = 0u; n < blocks16; ++n)
	{
		// [|buffer0[0] - buffer1[0]|, |buffer0[1] - buffer1[1]|, ..]
		const uint8x16_t absDifference_u_8x16 = vabdq_u8(vld1q_u8(buffer0), vld1q_u8(buffer1));

		const uint16x8_t absDifference_u_16x8 = vaddl_u8(vget_low_u8(absDifference_u_8x16), vget_high_u8(absDifference_u_8x16));

		sum_u_32x4 = vpadalq_u16(sum_u_32x4, absDifference_u_16x8);

		buffer0 += 16;
		buffer1 += 16;
	}

	// we may handle at most one block with 8 elements

	constexpr unsigned int blocks8 = (tSize % 16u) / 8u;
	static_assert(blocks8 <= 1u, "Invalid number of blocks!");

	if (blocks8 == 1u)
	{
		// [|buffer0[0] - buffer1[0]|, |buffer0[1] - buffer1[1]|, ..]
		const uint16x8_t absDifference_u_16x8 = vabdl_u8(vld1_u8(buffer0), vld1_u8(buffer1));

		sum_u_32x4 = vpadalq_u16(sum_u_32x4, absDifference_u_16x8);

		buffer0 += 8;
		buffer1 += 8;
	}

	uint32_t results[4];
	vst1q_u32(results, sum_u_32x4);

	constexpr unsigned int remainingElements = tSize - blocks16 * 16u - blocks8 * 8u;
	static_assert(remainingElements < 8u, "Invalid number of remaining elements!");

	uint32_t result = results[0] + results[1] + results[2] + results[3];

	// we apply the remaining elements (at most 7)

	for (unsigned int n = 0u; n < remainingElements; ++n)
	{
		result += uint32_t(abs(int32_t(buffer0[n]) - int32_t(buffer1[n])));
	}

	return result;
}

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t SumAbsoluteDifferencesNEON::patch8BitPerChannel(const uint8_t* patch0, const uint8_t* patch1, const unsigned int patch0StrideElements, const unsigned int patch1StrideElements)
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

	uint32x4_t sum_u_32x4 = vdupq_n_u32(0u);

	uint32_t sumIndividual = 0u;

	for (unsigned int y = 0u; y < tPatchSize; ++y)
	{
		for (unsigned int n = 0u; n < blocks16; ++n)
		{
			// [|patch0[0] - patch1[0]|, |patch0[1] - patch1[1]|, ..]
			const uint8x16_t absDifference_u_8x16 = vabdq_u8(vld1q_u8(patch0), vld1q_u8(patch1));

			const uint16x8_t absDifference_u_16x8 = vaddl_u8(vget_low_u8(absDifference_u_8x16), vget_high_u8(absDifference_u_8x16));

			sum_u_32x4 = vpadalq_u16(sum_u_32x4, absDifference_u_16x8);

			patch0 += 16;
			patch1 += 16;
		}

		for (unsigned int n = 0u; n < blocks8; ++n)
		{
			// [|patch0[0] - patch1[0]|, |patch0[1] - patch1[1]|, ..]
			const uint16x8_t absDifference_u_16x8 = vabdl_u8(vld1_u8(patch0), vld1_u8(patch1));

			sum_u_32x4 = vpadalq_u16(sum_u_32x4, absDifference_u_16x8);

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

					const uint16x8_t absDifference_u_16x8 = vabdl_u8(remaining0_u_8x8, remaining1_u_8x8);

					sum_u_32x4 = vpadalq_u16(sum_u_32x4, absDifference_u_16x8);
				}
				else
				{
					constexpr unsigned int overlapElements = 8u - blocks1;
					static_assert(overlapElements >= 1u && overlapElements < 8u, "Invalid number!");

					const uint8x8_t remaining0_u_8x8 = vand_u8(vld1_u8(patch0 - overlapElements), maskLeft_u_8x8);
					const uint8x8_t remaining1_u_8x8 = vand_u8(vld1_u8(patch1 - overlapElements), maskLeft_u_8x8);

					const uint16x8_t absDifference_u_16x8 = vabdl_u8(remaining0_u_8x8, remaining1_u_8x8);

					sum_u_32x4 = vpadalq_u16(sum_u_32x4, absDifference_u_16x8);
				}
			}
			else
			{
				for (unsigned int n = 0u; n < blocks1; ++n)
				{
					sumIndividual += uint32_t(abs(int32_t(patch0[n]) - int32_t(patch1[n])));
				}
			}

			patch0 += blocks1;
			patch1 += blocks1;
		}

		patch0 += patch0StrideElements - patchWidthElements;
		patch1 += patch1StrideElements - patchWidthElements;
	}

	uint32_t results[4];
	vst1q_u32(results, sum_u_32x4);

	return results[0] + results[1] + results[2] + results[3] + sumIndividual;
}

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t SumAbsoluteDifferencesNEON::patchBuffer8BitPerChannel(const uint8_t* patch0, const uint8_t* buffer1, const unsigned int patch0StrideElements)
{
	return patch8BitPerChannel<tChannels, tPatchSize>(patch0, buffer1, patch0StrideElements, tChannels * tPatchSize);
}

}

}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

#endif // META_OCEAN_CV_SUM_ABSOLUTE_DIFFERENCES_3_CHANNEL_24_BIT_NEON_H
