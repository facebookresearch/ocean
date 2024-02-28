// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/cv/FrameFilterGradientSSE.h"

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

#include "ocean/cv/SSE.h"

namespace Ocean
{

namespace CV
{

void FrameFilterGradientSSE::filterHorizontalVertical3Products1Channel8BitSubset(const uint8_t* source, int16_t* target, const unsigned int width, const unsigned int height, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(source && target);
	ocean_assert(width >= 3u && height >= 3u);

	ocean_assert(firstRow + numberRows <= height);

	source += firstRow * width;
	target += firstRow * width * 3u;

	const uint8_t* const sourceEnd = source + width * (numberRows - ((firstRow + numberRows == height) ? 1u : 0u));

	if (firstRow == 0u)
	{
		memset(target, 0x00, sizeof(int16_t) * width * 3u);
		source += width;
		target += width * 3u;
	}

	const unsigned int blockSize = 16u;
	const unsigned int blockSize3 = 3u * blockSize;
	const unsigned int blockIterations = (width - 2u) / blockSize;
	const unsigned int pixelIterations = (width - 2u) % blockSize;

	while (source != sourceEnd)
	{
		ocean_assert(source < sourceEnd);

		*target++ = 0;
		*target++ = 0;
		*target++ = 0;
		++source;

		for (unsigned int n = 0u; n < blockIterations; ++n)
		{
			SSE::gradientHorizontalVertical8Elements3Products1Channel8Bit(source, target, width);
			source += blockSize;
			target += blockSize3;
		}

		for (unsigned int n = 0u; n < pixelIterations; ++n)
		{
			// horizontal [-1 0 1]
			const int horizontal = (*(source + 1) - *(source - 1)) / 2;

			*target++ = int16_t(horizontal * horizontal);

			//          [-1]
			// vertical [ 0]
			//          [ 1]
			const int vertical = (*(source + width) - *(source - width)) / 2;

			*target++ = int16_t(vertical * vertical);
			*target++ = int16_t(horizontal * vertical);

			++source;
		}

		*target++ = 0;
		*target++ = 0;
		*target++ = 0;
		++source;
	}

	if (firstRow + numberRows == height)
		memset(target, 0x00, sizeof(int16_t) * width * 3u);
}

} // namespace CV

} // namespace Ocean

#endif // OCEAN_HARDWARE_SSE_VERSION >= 41
