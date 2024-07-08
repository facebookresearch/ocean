/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/SumSquareDifferencesBase.h"

namespace Ocean
{

namespace CV
{

uint32_t SumSquareDifferencesBase::patch8BitPerChannel(const uint8_t* patch0, const uint8_t* patch1, const unsigned int channels, const unsigned int patchSize, const unsigned int patch0StrideElements, const unsigned int patch1StrideElements)
{
	ocean_assert(patch0 != nullptr && patch1 != nullptr);

	ocean_assert(patchSize % 2u == 1u);
	ocean_assert(channels > 0u);

	ocean_assert(patch0StrideElements >= patchSize * channels);
	ocean_assert(patch1StrideElements >= patchSize * channels);

	uint32_t result = 0u;
	int16_t value = 0;

	for (unsigned int y = 0u; y < patchSize; ++y)
	{
		for (unsigned int n = 0u; n < patchSize * channels; ++n)
		{
			value = int16_t(patch0[n]) - int16_t(patch1[n]);
			result += value * value;
		}

		patch0 += patch0StrideElements;
		patch1 += patch1StrideElements;
	}

	return result;
}

uint32_t SumSquareDifferencesBase::buffer8BitPerChannel(const uint8_t* const buffer0, const uint8_t* const buffer1, const unsigned int bufferSize)
{
	ocean_assert(buffer0 != nullptr && buffer1 != nullptr);
	ocean_assert(bufferSize != 0u);

	int16_t difference;
	uint32_t ssd = 0u;

	for (unsigned int n = 0u; n < bufferSize; ++n)
	{
		difference = int16_t(buffer0[n]) - int16_t(buffer1[n]);
		ssd += uint32_t(difference * difference);
	}

	return ssd;
}

IndexPair32 SumSquareDifferencesBase::patchAtBorder8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const unsigned int channels, const unsigned int patchSize, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	ocean_assert(image0 != nullptr && image1 != nullptr);

	assert(channels != 0u);
	assert(patchSize % 2u == 1u);

	const unsigned int patchSize_2 = patchSize / 2u;

	ocean_assert(width0 >= patchSize_2 + 1u && height0 >= patchSize_2 + 1u);
	ocean_assert(width1 >= patchSize_2 + 1u && height1 >= patchSize_2 + 1u);

	ocean_assert(centerX0 < width0 && centerY0 < height0);
	ocean_assert(centerX1 < width1 && centerY1 < height1);

	ocean_assert(centerX0 - patchSize_2 < width0 - (patchSize - 1u) == (centerX0 >= patchSize_2 && centerX0 < width0 - patchSize_2));
	ocean_assert(centerY0 - patchSize_2 < height0 - (patchSize - 1u) == (centerY0 >= patchSize_2 && centerY0 < height0 - patchSize_2));
	ocean_assert(centerX1 - patchSize_2 < width1 - (patchSize - 1u) == (centerX1 >= patchSize_2 && centerX1 < width1 - patchSize_2));
	ocean_assert(centerY1 - patchSize_2 < height1 - (patchSize - 1u) == (centerY1 >= patchSize_2 && centerY1 < height1 - patchSize_2));

	if (centerX0 - patchSize_2 < width0 - (patchSize - 1u) && centerY0 - patchSize_2 < height0 - (patchSize - 1u) && centerX1 - patchSize_2 < width1 - (patchSize - 1u) && centerY1 - patchSize_2 < height1 - (patchSize - 1u))
	{
		const uint32_t ssd = SumSquareDifferencesBase::patch8BitPerChannel(image0, image1, channels, patchSize, width0, width1, centerX0, centerY0, centerX1, centerY1, image0PaddingElements, image1PaddingElements);

		return IndexPair32(ssd, patchSize * patchSize);
	}

	const unsigned int offsetLeft0 = min(centerX0, patchSize_2);
	const unsigned int offsetTop0 = min(centerY0, patchSize_2);
	const unsigned int offsetRight0 = min(width0 - centerX0 - 1u, patchSize_2);
	const unsigned int offsetBottom0 = min(height0 - centerY0 - 1u, patchSize_2);

	const unsigned int offsetLeft1 = min(centerX1, patchSize_2);
	const unsigned int offsetTop1 = min(centerY1, patchSize_2);
	const unsigned int offsetRight1 = min(width1 - centerX1 - 1u, patchSize_2);
	const unsigned int offsetBottom1 = min(height1 - centerY1 - 1u, patchSize_2);

	if (offsetLeft1 < offsetLeft0 || offsetRight1 < offsetRight0 || offsetTop1 < offsetTop0 || offsetBottom1 < offsetBottom0)
	{
		// at least one pixel in patch0 does not have a corresponding pixel in patch1 which lies inside image1s
		return IndexPair32((unsigned int)(-1), 0u);
	}

	const unsigned int image0StrideElements = width0 * channels + image0PaddingElements;
	const unsigned int image1StrideElements = width1 * channels + image1PaddingElements;

	const unsigned int columns = offsetRight0 + 1u + offsetLeft0;
	const unsigned int rows = offsetBottom0 + 1u + offsetTop0;

	const uint8_t* patch0 = image0 + (centerY0 - offsetTop0) * image0StrideElements + (centerX0 - offsetLeft0) * channels;
	const uint8_t* patch1 = image1 + (centerY1 - offsetTop0) * image1StrideElements + (centerX1 - offsetLeft0) * channels; // usage of offsetTop0 and offsetLeft0 is intended

	unsigned int ssd = 0u;

	for (unsigned int y = 0u; y < rows; ++y)
	{
		for (unsigned int x = 0u; x < columns; ++x)
		{
			for (unsigned int n = 0u; n < channels; ++n)
			{
				ssd += sqrDistance(patch0[x * channels + n], patch1[x * channels + n]);
			}
		}

		patch0 += image0StrideElements;
		patch1 += image1StrideElements;
	}

	return IndexPair32(ssd, rows * columns);
}

}

}
