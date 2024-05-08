/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/advanced/AdvancedSumSquareDifferencesBase.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

uint32_t AdvancedSumSquareDifferencesBase::patch8BitPerChannel(const uint8_t* const image0, const uint8_t* const image1, const unsigned int channels, const unsigned int patchSize, const unsigned int width0, const unsigned int width1, const Scalar centerX0, const Scalar centerY0, const Scalar centerX1, const Scalar centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	ocean_assert(image0 != nullptr && image1 != nullptr);

	ocean_assert(channels >= 1u);
	ocean_assert(patchSize >= 1u && patchSize % 2u == 1u);

	ocean_assert(width0 >= patchSize + 1u && width1 >= patchSize + 1u);

	const unsigned int patchSize_2 = patchSize / 2u;

	ocean_assert(centerX0 >= Scalar(patchSize_2) && centerX0 < Scalar(width0 - patchSize_2 - 1u));
	ocean_assert(centerX1 >= Scalar(patchSize_2) && centerX1 < Scalar(width1 - patchSize_2 - 1u));

	ocean_assert(centerY0 >= Scalar(patchSize_2) && centerY1 >= Scalar(patchSize_2));

	const unsigned int left0 = (unsigned int)(centerX0) - patchSize_2;
	const unsigned int top0 = (unsigned int)(centerY0) - patchSize_2;

	const unsigned int left1 = (unsigned int)(centerX1) - patchSize_2;
	const unsigned int top1 = (unsigned int)(centerY1) - patchSize_2;

	ocean_assert(left0 + patchSize < width0);
	ocean_assert(left1 + patchSize < width1);

	const unsigned int image0StrideElements = width0 * channels + image0PaddingElements;
	const unsigned int image1StrideElements = width1 * channels + image1PaddingElements;


	const Scalar tx0 = centerX0 - Scalar(int(centerX0));
	ocean_assert(tx0 >= Scalar(0) && tx0 <= Scalar(1));
	const unsigned int factorRight0 = (unsigned int)(tx0 * Scalar(128) + Scalar(0.5));
	const unsigned int factorLeft0 = 128u - factorRight0;

	const Scalar ty0 = centerY0 - Scalar(int(centerY0));
	ocean_assert(ty0 >= Scalar(0) && ty0 <= Scalar(1));
	const unsigned int factorBottom0 = (unsigned int)(ty0 * Scalar(128) + Scalar(0.5));
	const unsigned int factorTop0 = 128u - factorBottom0;

	const unsigned int factorTopLeft0 = factorLeft0 * factorTop0;
	const unsigned int factorTopRight0 = factorRight0 * factorTop0;
	const unsigned int factorBottomLeft0 = factorLeft0 * factorBottom0;
	const unsigned int factorBottomRight0 = factorRight0 * factorBottom0;


	const Scalar tx1 = centerX1 - Scalar(int(centerX1));
	ocean_assert(tx1 >= Scalar(0) && tx1 <= Scalar(1));
	const unsigned int factorRight1 = (unsigned int)(tx1 * Scalar(128) + Scalar(0.5));
	const unsigned int factorLeft1 = 128u - factorRight1;

	const Scalar ty1 = centerY1 - Scalar(int(centerY1));
	ocean_assert(ty1 >= Scalar(0) && ty1 <= Scalar(1));
	const unsigned int factorBottom1 = (unsigned int)(ty1 * Scalar(128) + Scalar(0.5));
	const unsigned int factorTop1 = 128u - factorBottom1;

	const unsigned int factorTopLeft1 = factorLeft1 * factorTop1;
	const unsigned int factorTopRight1 = factorRight1 * factorTop1;
	const unsigned int factorBottomLeft1 = factorLeft1 * factorBottom1;
	const unsigned int factorBottomRight1 = factorRight1 * factorBottom1;

	const uint8_t* imageTop0 = image0 + top0 * image0StrideElements + left0 * channels;
	const uint8_t* imageTop1 = image1 + top1 * image1StrideElements + left1 * channels;

	const uint8_t* imageBottom0 = imageTop0 + image0StrideElements;
	const uint8_t* imageBottom1 = imageTop1 + image1StrideElements;

	uint32_t result = 0u;

	for (unsigned int y = 0u; y < patchSize; ++y)
	{
		for (unsigned int x = 0u; x < patchSize; ++x)
		{
			for (unsigned int n = 0u; n < channels; ++n)
			{
				const uint8_t value0 = uint8_t((imageTop0[n] * factorTopLeft0 + imageTop0[channels + n] * factorTopRight0 + imageBottom0[n] * factorBottomLeft0 + imageBottom0[channels + n] * factorBottomRight0 + 8192u) / 16384u);
				const uint8_t value1 = uint8_t((imageTop1[n] * factorTopLeft1 + imageTop1[channels + n] * factorTopRight1 + imageBottom1[n] * factorBottomLeft1 + imageBottom1[channels + n] * factorBottomRight1 + 8192u) / 16384u);

				const int32_t difference = int32_t(value0) - int32_t(value1);

				result += difference * difference;
			}

			imageTop0 += channels;
			imageTop1 += channels;

			imageBottom0 += channels;
			imageBottom1 += channels;
		}

		imageTop0 += image0StrideElements - patchSize * channels;
		imageTop1 += image1StrideElements - patchSize * channels;

		imageBottom0 += image0StrideElements - patchSize * channels;
		imageBottom1 += image1StrideElements - patchSize * channels;
	}

	return result;
}

uint32_t AdvancedSumSquareDifferencesBase::patch8BitPerChannel(const uint8_t* const image0, const uint8_t* const image1, const unsigned int channels, const unsigned int patchSize, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const Scalar centerX1, const Scalar centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	ocean_assert(image0 != nullptr && image1 != nullptr);

	ocean_assert(channels >= 1u);
	ocean_assert(patchSize >= 1u && patchSize % 2u == 1u);

	ocean_assert(width0 >= patchSize && width1 >= patchSize + 1u);

	const unsigned int patchSize_2 = patchSize / 2u;

	ocean_assert(centerX0 >= patchSize_2 && centerX0 < width0 - patchSize_2);
	ocean_assert(centerX1 >= patchSize_2 && centerX1 < width1 - patchSize_2);

	ocean_assert(centerY0 >= Scalar(patchSize_2) && centerY1 >= Scalar(patchSize_2));

	const unsigned int left0 = centerX0 - patchSize_2;
	const unsigned int top0 = centerY0 - patchSize_2;

	const unsigned int left1 = (unsigned int)(centerX1) - patchSize_2;
	const unsigned int top1 = (unsigned int)(centerY1) - patchSize_2;

	ocean_assert(left0 + patchSize <= width0);
	ocean_assert(left1 + patchSize < width1);

	const unsigned int image0StrideElements = width0 * channels + image0PaddingElements;
	const unsigned int image1StrideElements = width1 * channels + image1PaddingElements;

	const Scalar tx1 = centerX1 - Scalar(int(centerX1));
	ocean_assert(tx1 >= Scalar(0) && tx1 <= Scalar(1));
	const unsigned int factorRight1 = (unsigned int)(tx1 * Scalar(128) + Scalar(0.5));
	const unsigned int factorLeft1 = 128u - factorRight1;

	const Scalar ty1 = centerY1 - Scalar(int(centerY1));
	ocean_assert(ty1 >= Scalar(0) && ty1 <= Scalar(1));
	const unsigned int factorBottom1 = (unsigned int)(ty1 * Scalar(128) + Scalar(0.5));
	const unsigned int factorTop1 = 128u - factorBottom1;

	const unsigned int factorTopLeft1 = factorLeft1 * factorTop1;
	const unsigned int factorTopRight1 = factorRight1 * factorTop1;
	const unsigned int factorBottomLeft1 = factorLeft1 * factorBottom1;
	const unsigned int factorBottomRight1 = factorRight1 * factorBottom1;

	const uint8_t* imageTop0 = image0 + top0 * image0StrideElements + left0 * channels;
	const uint8_t* imageTop1 = image1 + top1 * image1StrideElements + left1 * channels;

	const uint8_t* imageBottom1 = imageTop1 + image1StrideElements;

	uint32_t result = 0u;

	for (unsigned int y = 0u; y < patchSize; ++y)
	{
		for (unsigned int x = 0u; x < patchSize; ++x)
		{
			for (unsigned int n = 0u; n < channels; ++n)
			{
				const uint8_t value1 = uint8_t((imageTop1[n] * factorTopLeft1 + imageTop1[channels + n] * factorTopRight1 + imageBottom1[n] * factorBottomLeft1 + imageBottom1[channels + n] * factorBottomRight1 + 8192u) / 16384u);

				const int32_t difference = int32_t(imageTop0[n]) - int32_t(value1);

				result += difference * difference;
			}

			imageTop0 += channels;
			imageTop1 += channels;

			imageBottom1 += channels;
		}

		imageTop0 += image0StrideElements - patchSize * channels;
		imageTop1 += image1StrideElements - patchSize * channels;

		imageBottom1 += image1StrideElements - patchSize * channels;
	}

	return result;
}

uint32_t AdvancedSumSquareDifferencesBase::patchBuffer8BitPerChannel(const uint8_t* const image0, const unsigned int channels, const unsigned int patchSize, const unsigned int width0, const Scalar centerX0, const Scalar centerY0, const unsigned int image0PaddingElements, const uint8_t* buffer1)
{
	ocean_assert(image0 != nullptr && buffer1 != nullptr);

	ocean_assert(channels >= 1u);
	ocean_assert(patchSize >= 1u && patchSize % 2u == 1u);

	ocean_assert(width0 >= patchSize + 1u);

	const unsigned int patchSize_2 = patchSize / 2u;

	ocean_assert(centerX0 >= Scalar(patchSize_2) && centerX0 < Scalar(width0 - patchSize_2 - 1u));
	ocean_assert(centerY0 >= Scalar(patchSize_2));

	const unsigned int left0 = (unsigned int)(centerX0) - patchSize_2;
	const unsigned int top0 = (unsigned int)(centerY0) - patchSize_2;

	ocean_assert(left0 + patchSize + 1u < width0);

	const unsigned int image0StrideElements = width0 * channels + image0PaddingElements;


	const Scalar tx0 = centerX0 - Scalar(int(centerX0));
	ocean_assert(tx0 >= Scalar(0) && tx0 <= Scalar(1));
	const unsigned int factorRight0 = (unsigned int)(tx0 * Scalar(128) + Scalar(0.5));
	const unsigned int factorLeft0 = 128u - factorRight0;

	const Scalar ty0 = centerY0 - Scalar(int(centerY0));
	ocean_assert(ty0 >= Scalar(0) && ty0 <= Scalar(1));
	const unsigned int factorBottom0 = (unsigned int)(ty0 * Scalar(128) + Scalar(0.5));
	const unsigned int factorTop0 = 128u - factorBottom0;

	const unsigned int factorTopLeft0 = factorLeft0 * factorTop0;
	const unsigned int factorTopRight0 = factorRight0 * factorTop0;
	const unsigned int factorBottomLeft0 = factorLeft0 * factorBottom0;
	const unsigned int factorBottomRight0 = factorRight0 * factorBottom0;


	const uint8_t* imageTop0 = image0 + top0 * image0StrideElements + left0 * channels;
	const uint8_t* imageBottom0 = imageTop0 + image0StrideElements;

	uint32_t result = 0u;

	for (unsigned int y = 0u; y < patchSize; ++y)
	{
		for (unsigned int x = 0u; x < patchSize; ++x)
		{
			for (unsigned int n = 0u; n < channels; ++n)
			{
				const uint8_t value0 = uint8_t((imageTop0[n] * factorTopLeft0 + imageTop0[channels + n] * factorTopRight0 + imageBottom0[n] * factorBottomLeft0 + imageBottom0[channels + n] * factorBottomRight0 + 8192u) / 16384u);

				const int32_t difference = int32_t(value0) - int32_t(*buffer1++);

				result += difference * difference;
			}

			imageTop0 += channels;

			imageBottom0 += channels;
		}

		imageTop0 += image0StrideElements - patchSize * channels;

		imageBottom0 += image0StrideElements - patchSize * channels;
	}

	return result;
}

IndexPair32 AdvancedSumSquareDifferencesBase::patchWithMask8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const uint8_t* mask0, const uint8_t* mask1, const unsigned int channels, const unsigned int patchSize, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements, const unsigned int mask0PaddingElements, const unsigned int mask1PaddingElements, const uint8_t maskValue)
{
	ocean_assert(image0 != nullptr && image1 != nullptr);
	ocean_assert(mask0 != nullptr && mask1 != nullptr);

	ocean_assert(channels >= 1u);
	ocean_assert(patchSize >= 1u && patchSize % 2u == 1u);

	const unsigned int patchSize_2 = patchSize / 2u;

	ocean_assert(width0 >= patchSize_2 + 1u && height0 >= patchSize_2 + 1u);
	ocean_assert(width1 >= patchSize_2 + 1u && height1 >= patchSize_2 + 1u);

	ocean_assert(centerX0 < width0 && centerY0 < height0);
	ocean_assert(centerX1 < width1 && centerY1 < height1);

	const unsigned int image0StrideElements = width0 * channels + image0PaddingElements;
	const unsigned int image1StrideElements = width1 * channels + image1PaddingElements;

	const unsigned int mask0StrideElements = width0 + mask0PaddingElements;
	const unsigned int mask1StrideElements = width1 + mask1PaddingElements;

	const unsigned int offsetLeft = std::min(std::min(centerX0, patchSize_2), std::min(centerX1, patchSize_2));
	const unsigned int offsetTop = std::min(std::min(centerY0, patchSize_2), std::min(centerY1, patchSize_2));

	const unsigned int offsetRight = std::min(std::min(width0 - centerX0 - 1u, patchSize_2), std::min(width1 - centerX1 - 1u, patchSize_2));
	const unsigned int offsetBottom = std::min(std::min(height0 - centerY0 - 1u, patchSize_2), std::min(height1 - centerY1 - 1u, patchSize_2));

	ocean_assert(offsetLeft <= patchSize_2 && offsetRight <= patchSize_2);
	ocean_assert(offsetTop <= patchSize_2 && offsetTop <= patchSize_2);

	ocean_assert(offsetLeft <= centerX0 && offsetTop <= centerY0);
	ocean_assert(offsetTop <= centerY1 && offsetTop <= centerY1);

	ocean_assert(centerX0 + offsetRight < width0 && centerY0 + offsetBottom < height0);
	ocean_assert(centerX1 + offsetRight < width1 && centerY1 + offsetBottom < height1);

	const unsigned int patchWidth = offsetLeft + offsetRight + 1u;
	const unsigned int patchHeight = offsetTop + offsetBottom + 1u;

	ocean_assert(patchWidth <= patchSize && patchHeight <= patchSize);

	image0 += (centerY0 - offsetTop) * image0StrideElements + (centerX0 - offsetLeft) * channels;
	image1 += (centerY1 - offsetTop) * image1StrideElements + (centerX1 - offsetLeft) * channels;

	mask0 += (centerY0 - offsetTop) * mask0StrideElements + (centerX0 - offsetLeft);
	mask1 += (centerY1 - offsetTop) * mask1StrideElements + (centerX1 - offsetLeft);

	uint32_t validPixels = 0u;
	uint32_t ssd = 0u;

	for (unsigned int y = 0u; y < patchHeight; ++y)
	{
		for (unsigned int x = 0u; x < patchWidth; ++x)
		{
			if (mask0[x] != maskValue && mask1[x] != maskValue)
			{
				for (unsigned int n = 0u; n < channels; ++n)
				{
					ssd += sqr(image0[channels * x + n] - image1[channels * x + n]);
				}

				++validPixels;
			}
		}

		image0 += image0StrideElements;
		image1 += image1StrideElements;

		mask0 += mask0StrideElements;
		mask1 += mask1StrideElements;
	}

	return IndexPair32(ssd, validPixels);
}

IndexPair32 AdvancedSumSquareDifferencesBase::patchWithRejectingMask8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const uint8_t* mask0, const uint8_t* mask1, const unsigned int channels, const unsigned int patchSize, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int centerX0, const unsigned int centerY0, const unsigned int centerX1, const unsigned int centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements, const unsigned int mask0PaddingElements, const unsigned int mask1PaddingElements, const uint8_t maskValue)
{
	ocean_assert(image0 != nullptr && image1 != nullptr);
	ocean_assert(mask0 != nullptr && mask1 != nullptr);

	ocean_assert(channels >= 1u);
	ocean_assert(patchSize >= 1u && patchSize % 2u == 1u);

	const unsigned int patchSize_2 = patchSize / 2u;

	ocean_assert(width0 >= patchSize_2 + 1u && height0 >= patchSize_2 + 1u);
	ocean_assert(width1 >= patchSize_2 + 1u && height1 >= patchSize_2 + 1u);

	ocean_assert(centerX0 < width0 && centerY0 < height0);
	ocean_assert(centerX1 < width1 && centerY1 < height1);

	const unsigned int image0StrideElements = width0 * channels + image0PaddingElements;
	const unsigned int image1StrideElements = width1 * channels + image1PaddingElements;

	const unsigned int mask0StrideElements = width0 + mask0PaddingElements;
	const unsigned int mask1StrideElements = width1 + mask1PaddingElements;

	const unsigned int offsetLeft = std::min(centerX0, patchSize_2);
	const unsigned int offsetTop = std::min(centerY0, patchSize_2);

	const unsigned int offsetRight = std::min(width0 - centerX0 - 1u, patchSize_2);
	const unsigned int offsetBottom = std::min(height0 - centerY0 - 1u, patchSize_2);

	ocean_assert(offsetLeft <= patchSize_2 && offsetRight <= patchSize_2);
	ocean_assert(offsetTop <= patchSize_2 && offsetTop <= patchSize_2);

	ocean_assert(offsetLeft <= centerX0 && offsetTop <= centerY0);

	ocean_assert(centerX0 + offsetRight < width0 && centerY0 + offsetBottom < height0);

	const unsigned int patchWidth = offsetLeft + offsetRight + 1u;
	const unsigned int patchHeight = offsetTop + offsetBottom + 1u;

	ocean_assert(patchWidth <= patchSize && patchHeight <= patchSize);

	image0 += (centerY0 - offsetTop) * image0StrideElements + (centerX0 - offsetLeft) * channels;
	mask0 += (centerY0 - offsetTop) * mask0StrideElements + (centerX0 - offsetLeft);

	const int top1 = int(centerY1 - offsetTop);
	const int left1 = int(centerX1 - offsetLeft);

	image1 += top1 * int(image1StrideElements) + left1 * int(channels);
	mask1 += top1 * int(mask1StrideElements) + left1;

	uint32_t validPixels = 0u;
	uint32_t ssd = 0u;

	for (unsigned int y = 0u; y < patchHeight; ++y)
	{
		const unsigned int y1 = (unsigned int)(int(y) + top1);

		for (unsigned int x = 0u; x < patchWidth; ++x)
		{
			if (mask0[x] != maskValue)
			{
				const unsigned int x1 = (unsigned int)(int(x) + left1);

				if (x1 >= width1 || y1 >= height1 || mask1[x] == maskValue)
				{
					// a pixel outside of the second image or a mask pixel associated with the second image rejects the entire SSD

					return IndexPair32(uint32_t(-1), 0u);
				}

				for (unsigned int n = 0u; n < channels; ++n)
				{
					ssd += sqr(image0[channels * x + n] - image1[channels * x + n]);
				}

				++validPixels;
			}
		}

		image0 += image0StrideElements;
		image1 += image1StrideElements;

		mask0 += mask0StrideElements;
		mask1 += mask1StrideElements;
	}

	return IndexPair32(ssd, validPixels);
}

}

}

}
