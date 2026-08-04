/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameConverterRGB24.h"

namespace Ocean
{

namespace CV
{

void FrameConverterRGB24::changeRGB24ToBGR24Subset(uint8_t* frame, const unsigned int width, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(frame != nullptr);
	ocean_assert(numberRows != 0u);

	const unsigned int width3 = width * 3u;
	const unsigned int targetBlockSize = width3 * numberRows;

	frame += firstRow * width3;
	const uint8_t* const frameEnd = frame + targetBlockSize;

	--frame;
	while (++frame != frameEnd)
	{
		const uint8_t red = *frame;
		const uint8_t blue = frame[2];

		*frame = blue;
		*(frame += 2) = red;
	}
}

void FrameConverterRGB24::changeRGB24ToYUV24Subset(uint8_t* frame, const unsigned int width, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(frame != nullptr);
	ocean_assert(width != 0u);

	const unsigned int width3 = width * 3u;
	const unsigned int targetBlockSize = width3 * numberRows;

	frame += firstRow * width3;

	--frame;
	const uint8_t* const frameEnd = frame + targetBlockSize;
	while (frame != frameEnd)
	{
		// Y = min(abs(R * 2104 + G * 4130 + B * 802 + 4096 + 131072) >> 13, 235)
		// U = min(abs(R * -1214 + G * -2384 + B * 3598 + 4096 + 1048576) >> 13, 240)
		// V = min(abs(R * 3598 + G * -3013 + B * -585 + 4096 + 1048576) >> 13, 240)

		const int red = frame[1];
		const int green = frame[2];
		const int blue = frame[3];

		*++frame = (uint8_t)(((red * 66 + green * 129 + blue * 25 + 128) >> 8) + 16);
		*++frame = (uint8_t)(((red * -38 - green * 74 + blue * 112 + 128) >> 8) + 128);
		*++frame = (uint8_t)(((red * 112 - green * 94 - blue * 18 + 128) >> 8) + 128);
	}
}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

void FrameConverterRGB24::convertRGB24ToYUV24RowPrecision7BitNEON(const uint8_t* source, uint8_t* target, const size_t size, const void* parameters)
{
	ocean_assert(source != nullptr && target != nullptr && size >= 1);
	ocean_assert_and_suppress_unused(parameters == nullptr, parameters);

	constexpr size_t blockSize = 16;
	const size_t blocks = size / blockSize;

	if (blocks >= 1)
	{
		for (size_t n = 0; n < blocks; ++n)
		{
			convert16PixelsRGB24ToYUV24Precision7BitNEON(source, target);

			source += blockSize * size_t(3);
			target += blockSize * size_t(3);
		}

		const size_t remainingPixels = size % blockSize;

		if (remainingPixels)
		{
			// we need to apply another iteration with (back shifted) pointers

			ocean_assert(remainingPixels < blockSize);

			const size_t offset = blockSize - remainingPixels;

			source -= offset * size_t(3);
			target -= offset * size_t(3);

			convert16PixelsRGB24ToYUV24Precision7BitNEON(source, target);
		}
	}
	else
	{
		const uint8_t* const targetEnd = target + size * size_t(3);

		while (target != targetEnd)
		{
			ocean_assert(target < targetEnd);

			target[0] = (uint8_t)(minmax<int16_t>(0, (int16_t(source[0]) * int16_t(33) + int16_t(source[1]) * int16_t(64) + int16_t(source[2]) * int16_t(13)) / 128 + int16_t(16), 255));
			target[1] = (uint8_t)(minmax<int16_t>(0, (int16_t(source[0]) * int16_t(-19) + int16_t(source[1]) * int16_t(-37) + int16_t(source[2]) * int16_t(56)) / 128 + int16_t(128), 255));
			target[2] = (uint8_t)(minmax<int16_t>(0, (int16_t(source[0]) * int16_t(56) + int16_t(source[1]) * int16_t(-47) + int16_t(source[2]) * int16_t(-9)) / 128 + int16_t(128), 255));

			source += 3;
			target += 3;
		}
	}
}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

}

}
