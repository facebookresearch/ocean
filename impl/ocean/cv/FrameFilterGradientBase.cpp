// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/cv/FrameFilterGradientBase.h"

namespace Ocean
{

namespace CV
{

void FrameFilterGradientBase::filterHorizontalVertical3Products1Channel8BitLinedIntegralImage(const uint8_t* source, int32_t* target, const unsigned int width, const unsigned int height)
{

#ifdef OCEAN_DEBUG
	const uint8_t* const sourceDebug = source;
	int32_t* const targetDebug = target;
#endif // OCEAN_DEBUG

	ocean_assert(source && target);
	ocean_assert(width >= 3u && height >= 3u);

	/// one zero line for the integral border
	/// one zero line as standard filter border
	memset(target, 0x00, sizeof(int32_t) * (width + 1u) * 3u * 2u);
	source += width;
	target += width * 3u + 3u;

	const int32_t* targetLastRow = target + 6;
	int32_t lastTarget0 = 0;
	int32_t lastTarget1 = 0;
	int32_t lastTarget2 = 0;

	target += width * 3u + 3u;

	const uint8_t* const sourceEnd = source + width * (height - 2u);

	while (source != sourceEnd)
	{
		ocean_assert(source < sourceEnd);

		ocean_assert((source - sourceDebug) % width == 0u);
		ocean_assert((target - targetDebug) % ((width + 1) * 3u) == 0u);
		ocean_assert((targetLastRow - targetDebug) % ((width + 1) * 3u) == 6u);

		/// one zero pixel for the integral border
		*target++ = 0;
		*target++ = 0;
		*target++ = 0;

		/// one zero pixel as standard filter border
		*target++ = 0;
		*target++ = 0;
		*target++ = 0;
		++source;

		lastTarget0 = 0;
		lastTarget1 = 0;
		lastTarget2 = 0;

		const uint8_t* const sourceRowEnd = source + width - 2u;
		while (source != sourceRowEnd)
		{
			ocean_assert(source < sourceEnd);
			ocean_assert(source < sourceRowEnd);

			// horizontal [-1 0 1]
			const int32_t horizontal = (*(source + 1) - *(source - 1)) >> 1;

			lastTarget0 += horizontal * horizontal;

			//          [-1]
			// vertical [ 0]
			//          [ 1]
			const int32_t vertical = (*(source + width) - *(source - width)) >> 1;

			lastTarget1 += vertical * vertical;
			lastTarget2 += horizontal * vertical;

			*target++ = lastTarget0 + *targetLastRow++;
			*target++ = lastTarget1 + *targetLastRow++;
			*target++ = lastTarget2 + *targetLastRow++;

			++source;
		}

		*target++ = lastTarget0 + *targetLastRow++;
		*target++ = lastTarget1 + *targetLastRow++;
		*target++ = lastTarget2 + *targetLastRow++;

		targetLastRow += 6;
		++source;
	}

	ocean_assert(target + 3u * (width + 1u) == targetDebug + (width + 1u) * (height + 1u) * 3u);

	memcpy(target, target - 3u * (width + 1u), 3u * (width + 1u) * sizeof(int32_t));
}

void FrameFilterGradientBase::filterHorizontalVertical3Products1Channel8BitSubset(const uint8_t* source, int16_t* target, const unsigned int width, const unsigned int height, const unsigned int firstRow, const unsigned int numberRows)
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

	while (source != sourceEnd)
	{
		ocean_assert(source < sourceEnd);

		*target++ = 0;
		*target++ = 0;
		*target++ = 0;
		++source;

		const uint8_t* const sourceRowEnd = source + width - 2u;
		while (source != sourceRowEnd)
		{
			ocean_assert(source < sourceEnd);
			ocean_assert(source < sourceRowEnd);

			// horizontal [-1 0 1]
			const int32_t horizontal = (*(source + 1) - *(source - 1)) / 2;

			*target++ = int16_t(horizontal * horizontal);

			//          [-1]
			// vertical [ 0]
			//          [ 1]
			const int32_t vertical = (*(source + width) - *(source - width)) / 2;

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

}

}
