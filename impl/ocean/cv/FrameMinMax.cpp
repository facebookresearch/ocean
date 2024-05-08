/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameMinMax.h"

namespace Ocean
{

namespace CV
{

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

template <>
bool FrameMinMax::countElementsOutsideRange<uint8_t>(const uint8_t* frame, const uint32_t width, const uint32_t height, const uint32_t framePaddingElements, const uint8_t rangeStart, const uint8_t rangeEnd, uint32_t* elementsBelowRange, uint32_t* elementsAboveRange)
{
	if (frame == nullptr || width == 0u || height == 0u || rangeStart > rangeEnd || (elementsBelowRange == nullptr && elementsAboveRange == nullptr))
	{
		ocean_assert(false && "Invalid input");
		return false;
	}

	const uint32_t frameStrideElements = width + framePaddingElements;

	uint32_t elementsBelowRangeLocal = 0u;
	uint32_t elementsAboveRangeLocal = 0u;

	if (width >= 16u && width < 65536u * 8u /* uint16x8_t */)
	{
		// Iterate over groups of 16 elements.
		// For efficiency, elements outside the value range are counted in different stages:
		//
		//  1. for each row, count elements outside the value range as uint16x8_t (i.e. allows for a theoretical maximum image width of 65536x8 pixels).
		//  2. At the end of a row, add the row counters (uint16x8_t) to larger counters (uint32x4_t).
		//  3. At the end of the image, sum the elements of the larger counters (uint32_t) to get the final results.
		//
		// This approach minimizes the number of casts from uint8_t to uint32_t.

		// [rangeStart, ... 15x more]
		const uint8x16_t rangeStart_u_8x16 = vdupq_n_u8(rangeStart);

		// [rangeEnd, ... 15x more]
		const uint8x16_t rangeEnd_u_8x16 = vdupq_n_u8(rangeEnd);

		// large counter: [0, 0, 0, 0]
		uint32x4_t elementsBelowRange_u_32x4 = vdupq_n_u32(0u);

		// large counter: [0, 0, 0, 0]
		uint32x4_t elementsAboveRange_u_32x4 = vdupq_n_u32(0u);

		// Pre-compute a mask that's used to avoid counting elements twice in case the width isn't
		// a multiple of the vector size:
		//
		//                                                v---- right image border
		//   Image row          ...#######################|
		//   2nd last vector:      FEDCBA9876543210       |
		//   2nd last mask:        1111111111111111       |
		//   last vector:                 FEDCBA9876543210|
		//   last mask:                   0000000001111111|
		//                                |<----->| set to zero in order to avoid counting them twice
		//                                |
		//                                ^--- x-location to which the last vector will be reset (boundaryX)
		const uint32_t fullVectors = width / 16u;
		const uint32_t vectorElements = fullVectors * 16u;
		ocean_assert(vectorElements <= width);

		uint32_t boundaryX = uint32_t(-1);
		uint8_t boundaryMask[16] = { 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u };

		if (width > 16 && vectorElements < width)
		{
			const uint32_t x = vectorElements;
			ocean_assert(x >= 16u);
			boundaryX = width - 16u;

			ocean_assert(x > boundaryX);
			const uint32_t offset = x - boundaryX;
			ocean_assert(offset < 16u);

			memset(boundaryMask, 0, sizeof(uint8_t) * offset);
		}

		ocean_assert(boundaryX == width - 16u || vectorElements == width || width <= 16u);

		// [boundaryMask[0], boundaryMask[1], ..., boundaryMask[15]]
		const uint8x16_t boundaryMask_u_8x16 = vld1q_u8(boundaryMask);

		for (uint32_t y = 0u; y < height; ++y)
		{
			// [1, 1, 1, ... 13x more]
			uint8x16_t mask_u_8x16 = vdupq_n_u8(1u);

			// row-wise counters: [0, 0, 0, ... 13x more]
			uint16x8_t rowElementsBelowRange_u_16x8 = vdupq_n_u16(0u);
			uint16x8_t rowElementsAboveRange_u_16x8 = vdupq_n_u16(0u);

			for (uint32_t x = 0u; x < width; x += 16u)
			{
				if (x + 16u > width)
				{
					ocean_assert(x >= 16u && width > 16u);
					ocean_assert(boundaryX == width - 16u);

					x = boundaryX;
					mask_u_8x16 = boundaryMask_u_8x16;

					// The for loop will stop after this iteration
					ocean_assert(!(x + 16u < width));
				}

				// Load the next 16 values: frame[x + i], i = 0...15
				const uint8x16_t frame_u_8x16 = vld1q_u8(frame + x);

				// isBelowRange[i] = boundaryMask[i] & (frame[i] < rangeStart ? 0xFF : 0x00)
				const uint8x16_t isBelowRange_u_8x16 = vandq_u8(mask_u_8x16, vcltq_u8(frame_u_8x16, rangeStart_u_8x16));

				// rowElementsBelowRange[i] = rowElementsBelowRange[i] + uint16_t(isBelowRange[2*i]) + uint16_t(isBelowRange[2*i+1])
				rowElementsBelowRange_u_16x8 = vpadalq_u8(rowElementsBelowRange_u_16x8, isBelowRange_u_8x16);

				// isAboveRange[i] = boundaryMask[i] & (frame[i] >= rangeEnd ? 0xFF : 0x00)
				const uint8x16_t isAboveRange_u_8x16 = vandq_u8(mask_u_8x16, vcgeq_u8(frame_u_8x16, rangeEnd_u_8x16));

				// rowElementsAboveRange[i] = rowElementsAboveRange[i] + uint16_t(isAboveRange[2*i]) + uint16_t(isAboveRange[2*i+1])
				rowElementsAboveRange_u_16x8 = vpadalq_u8(rowElementsAboveRange_u_16x8, isAboveRange_u_8x16);
			}

			// elementsBelowRange[i] = elementsBelowRange[i] + uint32_t(rowElementsBelowRange[2*i]) + uint32_t(rowElementsBelowRange[2*i+1])
			elementsBelowRange_u_32x4  = vpadalq_u16(elementsBelowRange_u_32x4, rowElementsBelowRange_u_16x8);

			// elementsAboveRange[i] = elementsAboveRange[i] + uint32_t(rowElementsAboveRange[2*i]) + uint32_t(rowElementsAboveRange[2*i+1])
			elementsAboveRange_u_32x4  = vpadalq_u16(elementsAboveRange_u_32x4, rowElementsAboveRange_u_16x8);

			frame += frameStrideElements;
		}

		uint32_t countedElements[4];

		vst1q_u32(countedElements, elementsBelowRange_u_32x4);
		elementsBelowRangeLocal = countedElements[0] + countedElements[1] + countedElements[2] + countedElements[3];

		vst1q_u32(countedElements, elementsAboveRange_u_32x4);
		elementsAboveRangeLocal = countedElements[0] + countedElements[1] + countedElements[2] + countedElements[3];
	}
	else
	{
		for (uint32_t y = 0u; y < height; ++y)
		{
			const uint8_t* frameRowEnd = frame + width;

			while (frame < frameRowEnd)
			{
				if (*frame < rangeStart)
				{
					++elementsBelowRangeLocal;
				}
				else if (*frame >= rangeEnd)
				{
					++elementsAboveRangeLocal;
				}

				++frame;
			}

			frame += framePaddingElements;
		}
	}

	if (elementsBelowRange)
	{
		*elementsBelowRange = elementsBelowRangeLocal;
	}

	if (elementsAboveRange)
	{
		*elementsAboveRange = elementsAboveRangeLocal;
	}

	return true;
}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

} // namespace CV

} // namespace Ocean
