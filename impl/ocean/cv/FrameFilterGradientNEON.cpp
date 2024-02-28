// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/cv/FrameFilterGradientNEON.h"

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

#include "ocean/cv/NEON.h"

namespace Ocean
{

namespace CV
{

void FrameFilterGradientNEON::filterHorizontalVertical3Products1Channel8BitSubset(const uint8_t* source, int16_t* target, const unsigned int width, const unsigned int height, const unsigned int firstRow, const unsigned int numberRows)
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

	const unsigned int iterations8 = (width - 2u) / 8u;
	const unsigned int iterations = (width - 2u) % 8u;

	while (source != sourceEnd)
	{
		ocean_assert(source < sourceEnd);

		*target++ = 0;
		*target++ = 0;
		*target++ = 0;
		++source;

		for (unsigned int n = 0u; n < iterations8; ++n)
		{
			NEON::gradientHorizontalVertical8Elements3Products1Channel8Bit(source, target, width);
			source += 8;
			target += 24;
		}

		for (unsigned int n = 0u; n < iterations; ++n)
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
	{
		memset(target, 0x00, sizeof(int16_t) * width * 3u);
	}
}

}

}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10
