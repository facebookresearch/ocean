/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameFilterErosion.h"
#include "ocean/cv/NEON.h"

namespace Ocean
{

namespace CV
{

bool FrameFilterErosion::Comfort::shrinkMask(Frame& frame, Frame& mask, const MorphologyFilter erosionFilter, const unsigned int randomNoise, const unsigned int randomSeed)
{
	ocean_assert(frame.isValid() && mask.isValid());

	if (frame.width() == mask.width() && frame.height() == mask.height() && frame.pixelOrigin() == mask.pixelOrigin() && FrameType::formatIsGeneric(mask.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u))
	{
		if (frame.numberPlanes() == 1u)
		{
			switch (frame.channels())
			{
				case 1u:
				{
					switch (erosionFilter)
					{
						case MF_CROSS_3:
							return shrinkMask8BitPerChannel4Neighbor<1u, true>(frame.data<uint8_t>(), mask.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), randomNoise, randomSeed);

						case MF_SQUARE_3:
							return shrinkMask8BitPerChannel8Neighbor<1u, true>(frame.data<uint8_t>(), mask.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), randomNoise, randomSeed);

						default:
							break;
					}

					ocean_assert(false && "Invalid erosion filter!");
					return false;
				}

				case 2u:
				{
					switch (erosionFilter)
					{
						case MF_CROSS_3:
							return shrinkMask8BitPerChannel4Neighbor<2u, true>(frame.data<uint8_t>(), mask.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), randomNoise, randomSeed);

						case MF_SQUARE_3:
							return shrinkMask8BitPerChannel8Neighbor<2u, true>(frame.data<uint8_t>(), mask.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), randomNoise, randomSeed);

						default:
							break;
					}

					ocean_assert(false && "Invalid erosion filter!");
					return false;
				}

				case 3u:
				{
					switch (erosionFilter)
					{
						case MF_CROSS_3:
							return shrinkMask8BitPerChannel4Neighbor<3u, true>(frame.data<uint8_t>(), mask.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), randomNoise, randomSeed);

						case MF_SQUARE_3:
							return shrinkMask8BitPerChannel8Neighbor<3u, true>(frame.data<uint8_t>(), mask.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), randomNoise, randomSeed);

						default:
							break;
					}

					ocean_assert(false && "Invalid erosion filter!");
					return false;
				}

				case 4u:
				{
					switch (erosionFilter)
					{
						case MF_CROSS_3:
							return shrinkMask8BitPerChannel4Neighbor<4u, true>(frame.data<uint8_t>(), mask.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), randomNoise, randomSeed);

						case MF_SQUARE_3:
							return shrinkMask8BitPerChannel8Neighbor<4u, true>(frame.data<uint8_t>(), mask.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), randomNoise, randomSeed);

						default:
							break;
					}

					ocean_assert(false && "Invalid erosion filter!");
					return false;
				}
			}

			ocean_assert(false && "Invalid frame pixel format!");
		}
	}

	ocean_assert(false && "Invalid frame dimension or pixel origin!");
	return false;
}

bool FrameFilterErosion::Comfort::shrinkMaskRandom(Frame& frame, Frame& mask, const MorphologyFilter erosionFilter, const unsigned int randomNoise, const unsigned int randomSeed)
{
	ocean_assert(frame.isValid() && mask.isValid());

	if (FrameType(frame, FrameType::FORMAT_Y8) == mask.frameType() && frame.numberPlanes() == 1u && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		switch (frame.channels())
		{
			case 1u:
			{
				switch (erosionFilter)
				{
					case MF_SQUARE_3:
						shrinkMaskRandom8BitPerChannel8Neighbor<1u>(frame.data<uint8_t>(), mask.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), randomNoise, randomSeed);
						return true;

					default:
						break;
				}

				ocean_assert(false && "Invalid erosion filter!");
				return false;
			}

			case 2u:
			{
				switch (erosionFilter)
				{
					case MF_SQUARE_3:
						shrinkMaskRandom8BitPerChannel8Neighbor<2u>(frame.data<uint8_t>(), mask.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), randomNoise, randomSeed);
						return true;

					default:
						break;
				}

				ocean_assert(false && "Invalid erosion filter!");
				return false;
			}

			case 3u:
			{
				switch (erosionFilter)
				{
					case MF_SQUARE_3:
						shrinkMaskRandom8BitPerChannel8Neighbor<3u>(frame.data<uint8_t>(), mask.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), randomNoise, randomSeed);
						return true;

					default:
						break;
				}

				ocean_assert(false && "Invalid erosion filter!");
				return false;
			}

			case 4u:
			{
				switch (erosionFilter)
				{
					case MF_SQUARE_3:
						shrinkMaskRandom8BitPerChannel8Neighbor<4u>(frame.data<uint8_t>(), mask.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), randomNoise, randomSeed);
						return true;

					default:
						break;
				}

				ocean_assert(false && "Invalid erosion filter!");
				return false;
			}
		}

		ocean_assert(false && "Invalid frame pixel format!");
	}

	ocean_assert(false && "Invalid frame dimension or pixel origin!");
	return false;
}

void FrameFilterErosion::filter1Channel8Bit4NeighborSubset(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t maskValue, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(mask != nullptr && target != nullptr);
	ocean_assert(firstRow + numberRows <= height);

	ocean_assert(width >= 2u && height >= 2u);

	const unsigned int width_2 = width - 2u;
	const unsigned int height_1 = height - 1u;

	const unsigned int maskStrideElements = width + maskPaddingElements;
	const unsigned int targetStrideElements = width + targetPaddingElements;

	const uint8_t nonMaskValue = 255u - maskValue;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		const uint8_t* maskRow0 = mask + std::max(0, int(y) - 1) * maskStrideElements; // mirroring when top row
		const uint8_t* maskRow1 = mask + y * maskStrideElements;
		const uint8_t* maskRow2 = mask + std::min(y + 1u, height_1) * maskStrideElements; // mirroring when bottom row

		uint8_t* targetRow = target + y * targetStrideElements;

		// left pixel
		// row0: | ?
		// row1: | X ?
		// row2: | ?
		*targetRow++ = (*maskRow1 != maskValue || *(maskRow0) != maskValue || *(maskRow1 + 1) != maskValue || *(maskRow2) != maskValue) ? nonMaskValue : maskValue;
		++maskRow0;
		++maskRow1;
		++maskRow2;

		for (unsigned int n = 0u; n < width_2; ++n)
		{
			// center pixels
			// row0:   ?
			// row1: ? X ?
			// row2:   ?
			*targetRow++ = (*maskRow1 != maskValue || *(maskRow0) != maskValue || *(maskRow1 - 1) != maskValue || *(maskRow1 + 1) != maskValue || *(maskRow2) != maskValue) ? nonMaskValue : maskValue;
			++maskRow0;
			++maskRow1;
			++maskRow2;
		}

		// right pixel
		// row0:   ? |
		// row1: ? X |
		// row2:   ? |
		*targetRow = (*maskRow1 != maskValue || *(maskRow0) != maskValue || *(maskRow1 - 1) != maskValue || *(maskRow2) != maskValue) ? nonMaskValue : maskValue;
	}
}

void FrameFilterErosion::filter1Channel8Bit8NeighborSubset(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t maskValue, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(mask != nullptr && target != nullptr);
	ocean_assert(firstRow + numberRows <= height);

	ocean_assert(width >= 2u && height >= 2u);

	const unsigned int width_2 = width - 2u;
	const unsigned int height_1 = height - 1u;

	const unsigned int maskStrideElements = width + maskPaddingElements;
	const unsigned int targetStrideElements = width + targetPaddingElements;

	const uint8_t nonMaskValue = 255u - maskValue;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		const uint8_t* maskRow0 = mask + std::max(0, int(y) - 1) * maskStrideElements; // mirroring when top row
		const uint8_t* maskRow1 = mask + y * maskStrideElements;
		const uint8_t* maskRow2 = mask + std::min(y + 1u, height_1) * maskStrideElements; // mirroring when bottom row

		uint8_t* targetRow = target + y * targetStrideElements;

		// left pixel
		// row0: | ? ?
		// row1: | X ?
		// row2: | ? ?
		*targetRow++ = (maskRow0[0] != maskValue || maskRow0[1] != maskValue || maskRow1[0] != maskValue || maskRow1[1] != maskValue || maskRow2[0] != maskValue || maskRow2[1] != maskValue) ? nonMaskValue : maskValue;

		unsigned int n = 0u;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		const uint8x16_t maskValue_u_8x16 = vmovq_n_u8(maskValue);
		const uint8x16_t nonMaskValue_u_8x16 = vmovq_n_u8(nonMaskValue);

		// we handle blocks of 16 pixels with NEON
		const unsigned int width_2_16 = (width_2 / 16u) * 16u;

		while (n < width_2_16)
		{
			const uint8x16_t resultA_u_8x16 = vandq_u8(vceqq_u8(maskValue_u_8x16, vld1q_u8(maskRow0 + 0)), vceqq_u8(maskValue_u_8x16, vld1q_u8(maskRow0 + 1)));
			const uint8x16_t resultB_u_8x16 = vandq_u8(vceqq_u8(maskValue_u_8x16, vld1q_u8(maskRow0 + 2)), vceqq_u8(maskValue_u_8x16, vld1q_u8(maskRow1 + 0)));
			const uint8x16_t resultC_u_8x16 = vandq_u8(vceqq_u8(maskValue_u_8x16, vld1q_u8(maskRow1 + 1)), vceqq_u8(maskValue_u_8x16, vld1q_u8(maskRow1 + 2)));
			const uint8x16_t resultD_u_8x16 = vandq_u8(vceqq_u8(maskValue_u_8x16, vld1q_u8(maskRow2 + 0)), vceqq_u8(maskValue_u_8x16, vld1q_u8(maskRow2 + 1)));
			const uint8x16_t resultAB_u_8x16 = vandq_u8(resultA_u_8x16, resultB_u_8x16);
			const uint8x16_t resultCD_u_8x16 = vandq_u8(resultC_u_8x16, resultD_u_8x16);

			const uint8x16_t resultABCD_u_8x16 = vandq_u8(resultAB_u_8x16, resultCD_u_8x16);
			const uint8x16_t result_u_8x16 = vandq_u8(resultABCD_u_8x16, vceqq_u8(maskValue_u_8x16, vld1q_u8(maskRow2 + 2)));

			const uint8x16_t resultMask_u_8x16 = vbslq_u8(result_u_8x16, maskValue_u_8x16, nonMaskValue_u_8x16);

			vst1q_u8(targetRow, resultMask_u_8x16);

			targetRow += 16;
			maskRow0 += 16;
			maskRow1 += 16;
			maskRow2 += 16;

			n += 16u;
		}

#endif

		while (n < width_2)
		{
			// center pixels
			// row0: ? ? ?
			// row1: ? X ?
			// row2: ? ? ?
			*targetRow++ = (maskRow0[0] != maskValue || maskRow0[1] != maskValue || maskRow0[2] != maskValue || maskRow1[0] != maskValue || maskRow1[1] != maskValue || maskRow1[2] != maskValue || maskRow2[0] != maskValue || maskRow2[1] != maskValue || maskRow2[2] != maskValue) ? nonMaskValue : maskValue;
			++maskRow0;
			++maskRow1;
			++maskRow2;

			++n;
		}

		// right pixel
		// row0: ? ? |
		// row1: ? X |
		// row2: ? ? |
		*targetRow = (maskRow0[0] != maskValue || maskRow0[1] != maskValue || maskRow1[0] != maskValue || maskRow1[1] != maskValue || maskRow2[0] != maskValue || maskRow2[1] != maskValue) ? nonMaskValue : maskValue;
	}
}

void FrameFilterErosion::filter1Channel8Bit24NeighborSubset(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t maskValue, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(mask != nullptr && target != nullptr);
	ocean_assert(firstRow + numberRows <= height);

	ocean_assert(width >= 4u && height >= 4u);

	const unsigned int width_4 = width - 4u;
	const unsigned int height_1 = height - 1u;

	const unsigned int maskStrideElements = width + maskPaddingElements;
	const unsigned int targetStrideElements = width + targetPaddingElements;

	const uint8_t nonMaskValue = 255u - maskValue;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		const uint8_t* maskRow0 = mask + std::max(0, int(y) - 2) * maskStrideElements; // mirroring when top row
		const uint8_t* maskRow1 = mask + std::max(0, int(y) - 1) * maskStrideElements; // mirroring when top row
		const uint8_t* maskRow2 = mask + y * maskStrideElements;
		const uint8_t* maskRow3 = mask + std::min(y + 1u, height_1) * maskStrideElements; // mirroring when bottom row
		const uint8_t* maskRow4 = mask + std::min(y + 2u, height_1) * maskStrideElements; // mirroring when bottom row

		uint8_t* targetRow = target + y * targetStrideElements;

		// left pixel
		// row0: | ? ? ?
		// row1: | ? ? ?
		// row2: | X ? ?
		// row3: | ? ? ?
		// row4: | ? ? ?
		*targetRow++ = (onePixelNotEqual<3u>(maskRow0, maskValue) || onePixelNotEqual<3u>(maskRow1, maskValue) || onePixelNotEqual<3u>(maskRow2, maskValue) || onePixelNotEqual<3u>(maskRow3, maskValue) || onePixelNotEqual<3u>(maskRow4, maskValue)) ? nonMaskValue : maskValue;

		// second left pixel
		// row0: | ? ? ? ?
		// row1: | ? ? ? ?
		// row2: | ? X ? ?
		// row3: | ? ? ? ?
		// row4: | ? ? ? ?
		*targetRow++ = (onePixelNotEqual<4u>(maskRow0, maskValue) || onePixelNotEqual<4u>(maskRow1, maskValue) || onePixelNotEqual<4u>(maskRow2, maskValue) || onePixelNotEqual<4u>(maskRow3, maskValue) || onePixelNotEqual<4u>(maskRow4, maskValue)) ? nonMaskValue : maskValue;

		for (unsigned int n = 0u; n < width_4; ++n)
		{
			// center pixels
			// row0: ? ? ? ? ?
			// row1: ? ? ? ? ?
			// row2: ? ? X ? ?
			// row3: ? ? ? ? ?
			// row4: ? ? ? ? ?
			*targetRow++ = (onePixelNotEqual<5u>(maskRow0, maskValue) || onePixelNotEqual<5u>(maskRow1, maskValue) || onePixelNotEqual<5u>(maskRow2, maskValue) || onePixelNotEqual<5u>(maskRow3, maskValue) || onePixelNotEqual<5u>(maskRow4, maskValue)) ? nonMaskValue : maskValue;
			++maskRow0;
			++maskRow1;
			++maskRow2;
			++maskRow3;
			++maskRow4;
		}

		// second right pixel
		// row0: ? ? ? ? |
		// row1: ? ? ? ? |
		// row2: ? ? X ? |
		// row3: ? ? ? ? |
		// row4: ? ? ? ? |
		*targetRow++ = (onePixelNotEqual<4u>(maskRow0, maskValue) || onePixelNotEqual<4u>(maskRow1, maskValue) || onePixelNotEqual<4u>(maskRow2, maskValue) || onePixelNotEqual<4u>(maskRow3, maskValue) || onePixelNotEqual<4u>(maskRow4, maskValue)) ? nonMaskValue : maskValue;
		++maskRow0;
		++maskRow1;
		++maskRow2;
		++maskRow3;
		++maskRow4;

		// right pixel
		// row0: ? ? ? |
		// row1: ? ? ? |
		// row2: ? ? X |
		// row3: ? ? ? |
		// row4: ? ? ? |
		*targetRow++ = (onePixelNotEqual<3u>(maskRow0, maskValue) || onePixelNotEqual<3u>(maskRow1, maskValue) || onePixelNotEqual<3u>(maskRow2, maskValue) || onePixelNotEqual<3u>(maskRow3, maskValue) || onePixelNotEqual<3u>(maskRow4, maskValue)) ? nonMaskValue : maskValue;
	}
}

}

}
