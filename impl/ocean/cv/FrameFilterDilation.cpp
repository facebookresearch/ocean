/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameFilterDilation.h"

namespace Ocean
{

namespace CV
{

void FrameFilterDilation::filter1Channel8Bit4NeighborSubset(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t maskValue, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
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
		*targetRow++ = (*maskRow1 != maskValue && *(maskRow0) != maskValue && *(maskRow1 + 1) != maskValue && *(maskRow2) != maskValue) ? nonMaskValue : maskValue;
		++maskRow0;
		++maskRow1;
		++maskRow2;

		for (unsigned int n = 0u; n < width_2; ++n)
		{
			// center pixels
			// row0:   ?
			// row1: ? X ?
			// row2:   ?
			*targetRow++ = (*maskRow1 != maskValue && *(maskRow0) != maskValue && *(maskRow1 - 1) != maskValue && *(maskRow1 + 1) != maskValue && *(maskRow2) != maskValue) ? nonMaskValue : maskValue;
			++maskRow0;
			++maskRow1;
			++maskRow2;
		}

		// right pixel
		// row0:   ? |
		// row1: ? X |
		// row2:   ? |
		*targetRow = (*maskRow1 != maskValue && *(maskRow0) != maskValue && *(maskRow1 - 1) != maskValue && *(maskRow2) != maskValue) ? nonMaskValue : maskValue;
	}
}

void FrameFilterDilation::filter1Channel8Bit8NeighborSubset(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t maskValue, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
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
		*targetRow++ = (*maskRow1 != maskValue && *(maskRow0) != maskValue && *(maskRow0 + 1) != maskValue && *(maskRow1 + 1) != maskValue && *(maskRow2) != maskValue && *(maskRow2 + 1) != maskValue) ? nonMaskValue : maskValue;
		++maskRow0;
		++maskRow1;
		++maskRow2;

		for (unsigned int n = 0u; n < width_2; ++n)
		{
			// center pixels
			// row0: ? ? ?
			// row1: ? X ?
			// row2: ? ? ?
			*targetRow++ = (*maskRow1 != maskValue && *(maskRow0) != maskValue && *(maskRow0 - 1) != maskValue && *(maskRow0 + 1) != maskValue && *(maskRow1 - 1) != maskValue && *(maskRow1 + 1) != maskValue && *(maskRow2) != maskValue && *(maskRow2 - 1) != maskValue && *(maskRow2 + 1) != maskValue) ? nonMaskValue : maskValue;
			++maskRow0;
			++maskRow1;
			++maskRow2;
		}

		// right pixel
		// row0: ? ? |
		// row1: ? X |
		// row2: ? ? |
		*targetRow = (*maskRow1 != maskValue && *(maskRow0) != maskValue && *(maskRow0 - 1) != maskValue && *(maskRow1 - 1) != maskValue && *(maskRow2) != maskValue && *(maskRow2 - 1) != maskValue) ? nonMaskValue : maskValue;
	}
}

void FrameFilterDilation::filter1Channel8Bit24NeighborSubset(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t maskValue, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
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
		*targetRow++ = (eachPixelNotEqual<3u>(maskRow0, maskValue) && eachPixelNotEqual<3u>(maskRow1, maskValue) && eachPixelNotEqual<3u>(maskRow2, maskValue) && eachPixelNotEqual<3u>(maskRow3, maskValue) && eachPixelNotEqual<3u>(maskRow4, maskValue)) ? nonMaskValue : maskValue;

		// second left pixel
		// row0: | ? ? ? ?
		// row1: | ? ? ? ?
		// row2: | ? X ? ?
		// row3: | ? ? ? ?
		// row4: | ? ? ? ?
		*targetRow++ = (eachPixelNotEqual<4u>(maskRow0, maskValue) && eachPixelNotEqual<4u>(maskRow1, maskValue) && eachPixelNotEqual<4u>(maskRow2, maskValue) && eachPixelNotEqual<4u>(maskRow3, maskValue) && eachPixelNotEqual<4u>(maskRow4, maskValue)) ? nonMaskValue : maskValue;

		for (unsigned int n = 0u; n < width_4; ++n)
		{
			// center pixels
			// row0: ? ? ? ? ?
			// row1: ? ? ? ? ?
			// row2: ? ? X ? ?
			// row3: ? ? ? ? ?
			// row4: ? ? ? ? ?
			*targetRow++ = (eachPixelNotEqual<5u>(maskRow0, maskValue) && eachPixelNotEqual<5u>(maskRow1, maskValue) && eachPixelNotEqual<5u>(maskRow2, maskValue) && eachPixelNotEqual<5u>(maskRow3, maskValue) && eachPixelNotEqual<5u>(maskRow4, maskValue)) ? nonMaskValue : maskValue;
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
		*targetRow++ = (eachPixelNotEqual<4u>(maskRow0, maskValue) && eachPixelNotEqual<4u>(maskRow1, maskValue) && eachPixelNotEqual<4u>(maskRow2, maskValue) && eachPixelNotEqual<4u>(maskRow3, maskValue) && eachPixelNotEqual<4u>(maskRow4, maskValue)) ? nonMaskValue : maskValue;
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
		*targetRow++ = (eachPixelNotEqual<3u>(maskRow0, maskValue) && eachPixelNotEqual<3u>(maskRow1, maskValue) && eachPixelNotEqual<3u>(maskRow2, maskValue) && eachPixelNotEqual<3u>(maskRow3, maskValue) && eachPixelNotEqual<3u>(maskRow4, maskValue)) ? nonMaskValue : maskValue;
	}
}

}

}
