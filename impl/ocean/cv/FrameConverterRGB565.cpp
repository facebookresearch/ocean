/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameConverterRGB565.h"
#include "ocean/cv/NEON.h"

namespace Ocean
{

namespace CV
{

void FrameConverterRGB565::convertRowRGB565ToRGB24(const uint16_t* source, uint8_t* target, const size_t width, const void* unusedOptions)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u);

	ocean_assert_and_suppress_unused(unusedOptions == nullptr, unusedOptions);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	if (width >= size_t(8))
	{
		convertRowRGB565ToRGB24NEON(source, target, (unsigned int)width);
		return;
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	for (size_t n = 0; n < width; ++n)
	{
		const uint8_t red5 = (*source & 0x1Fu);
		const uint8_t green6 = (*source >> 5u) & 0x3Fu;
		const uint8_t blue5 = (*source >> 11u);

		// shifting with inserting
		*target++ = uint8_t((red5 << 3u) | (red5 & 0x07u));
		*target++ = uint8_t((green6 << 2u) | (green6 & 0x03u));
		*target++ = uint8_t((blue5 << 3u) | (blue5 & 0x07u));

		++source;
	}
}

void FrameConverterRGB565::convertRowRGB565ToY8(const uint16_t* source, uint8_t* target, const size_t width, const void* unusedOptions)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u);

	ocean_assert_and_suppress_unused(unusedOptions == nullptr, unusedOptions);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	if (width >= size_t(8))
	{
		convertRowRGB565ToY8NEON(source, target, (unsigned int)width);
		return;
	}

#endif // OCEAN_HARDWARE_NEON_VERSION

	for (size_t n = 0; n < width; ++n)
	{
		const uint8_t red5 = (*source & 0x1Fu);
		const uint8_t green6 = (*source >> 5u) & 0x3Fu;
		const uint8_t blue5 = (*source >> 11u);

		// shifting with inserting
		const unsigned int redPart = ((red5 << 3u) | (red5 & 0x07u)) * 306u;
		const unsigned int greenPart = ((green6 << 2u) | (green6 & 0x03u)) * 601u;
		const unsigned int bluePart = ((blue5 << 3u) | (blue5 & 0x07u)) * 117u;

		ocean_assert(redPart + greenPart + bluePart < 0x40000);
		*target++ = uint8_t((redPart + greenPart + bluePart + 512u) >> 10u);

		++source;
	}
}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

void FrameConverterRGB565::convertRowRGB565ToRGB24NEON(const uint16_t* source, uint8_t* target, const unsigned int width)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 8u);

	uint8x8x3_t red_green_blue_u_8x8x3;

	for (unsigned int x = 0u; x < width; x += 8u)
	{
		if (x + 8u > width)
		{
			// the last iteration will not fit into the output frame,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(x >= 8u && width > 8u);
			const unsigned int newX = width - 8u;

			ocean_assert(x > newX);

			x = newX;

			// the for loop will stop after this iteration
			ocean_assert(!(x + 8u < width));
		}

		uint8x16_t source_u_8x16 = vreinterpretq_u8_u16(vld1q_u16(source + x));

		// shift (with insert) red:
		// RRRRRGGG GGGBBBBB    RRRRRGGG GGGBBBBB    RRRRRGGG GGGBBBBB    RRRRRGGG GGGBBBBB
		// rrrRRRRR ????????    rrrRRRRR ????????    rrrRRRRR ????????    rrrRRRRR ????????
		const uint8x16_t red_u_8x16 = vsliq_n_u8(source_u_8x16, source_u_8x16, 3);
		red_green_blue_u_8x8x3.val[0] = vmovn_u16(vreinterpretq_u16_u8(red_u_8x16));

		// shift (with insert) green:
		// RRRRRGGG GGGBBBBB    RRRRRGGG GGGBBBBB    RRRRRGGG GGGBBBBB    RRRRRGGG GGGBBBBB
		// GGGGGGBB BBB00000    GGGGGGBB BBB00000    GGGGGGBB BBB00000    GGGGGGBB BBB00000
		// ggGGGGGG ????????    ggGGGGGG ????????    ggGGGGGG ????????    ggGGGGGG ????????
		const uint8x8_t green_intermediate_u_8x8 = vshrn_n_u16(vreinterpretq_u16_u8(source_u_8x16), 5);
		red_green_blue_u_8x8x3.val[1] = vsli_n_u8(green_intermediate_u_8x8, green_intermediate_u_8x8, 2);

		// shift (with insert) blue:
		// RRRRRGGG GGGBBBBB    RRRRRGGG GGGBBBBB    RRRRRGGG GGGBBBBB    RRRRRGGG GGGBBBBB
		// BBBBB000 00000000    BBBBB000 00000000    BBBBB000 00000000    BBBBB000 00000000
		// bbbBBBBB ????????    bbbBBBBB ????????    bbbBBBBB ????????    bbbBBBBB ????????
		const uint16x8_t blue_intermediate_u_16x8 = vshrq_n_u16(vreinterpretq_u16_u8(source_u_8x16), 11);
		const uint16x8_t blue_u_16x8 = vsliq_n_u16(blue_intermediate_u_16x8, blue_intermediate_u_16x8, 3);
		red_green_blue_u_8x8x3.val[2] = vmovn_u16(blue_u_16x8);

		vst3_u8(target + x * 3u, red_green_blue_u_8x8x3);
	}
}

void FrameConverterRGB565::convertRowRGB565ToY8NEON(const uint16_t* source, uint8_t* target, const unsigned int width)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 8u);

	const uint8x8_t constant_38_u_8x8 = vdup_n_u8(38u);
	const uint8x8_t constant_75_u_8x8 = vdup_n_u8(75u);
	const uint8x8_t constant_15_u_8x8 = vdup_n_u8(15u);

	for (unsigned int x = 0u; x < width; x += 8u)
	{
		if (x + 8u > width)
		{
			// the last iteration will not fit into the output frame,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(x >= 8u && width > 8u);
			const unsigned int newX = width - 8u;

			ocean_assert(x > newX);

			x = newX;

			// the for loop will stop after this iteration
			ocean_assert(!(x + 8u < width));
		}

		uint8x16_t source_u_8x16 = vreinterpretq_u8_u16(vld1q_u16(source + x));

		// shift (with insert) red:
		// RRRRRGGG GGGBBBBB    RRRRRGGG GGGBBBBB    RRRRRGGG GGGBBBBB    RRRRRGGG GGGBBBBB
		// rrrRRRRR ????????    rrrRRRRR ????????    rrrRRRRR ????????    rrrRRRRR ????????
		const uint8x16_t red_u_8x16 = vsliq_n_u8(source_u_8x16, source_u_8x16, 3);
		const uint8x8_t red_u_8x8 = vmovn_u16(vreinterpretq_u16_u8(red_u_8x16));

		// shift (with insert) green:
		// RRRRRGGG GGGBBBBB    RRRRRGGG GGGBBBBB    RRRRRGGG GGGBBBBB    RRRRRGGG GGGBBBBB
		// GGGGGGBB BBB00000    GGGGGGBB BBB00000    GGGGGGBB BBB00000    GGGGGGBB BBB00000
		// ggGGGGGG ????????    ggGGGGGG ????????    ggGGGGGG ????????    ggGGGGGG ????????
		const uint8x8_t green_intermediate_u_8x8 = vshrn_n_u16(vreinterpretq_u16_u8(source_u_8x16), 5);
		const uint8x8_t green_u_8x8 = vsli_n_u8(green_intermediate_u_8x8, green_intermediate_u_8x8, 2);

		// shift (with insert) blue:
		// RRRRRGGG GGGBBBBB    RRRRRGGG GGGBBBBB    RRRRRGGG GGGBBBBB    RRRRRGGG GGGBBBBB
		// BBBBB000 00000000    BBBBB000 00000000    BBBBB000 00000000    BBBBB000 00000000
		// bbbBBBBB ????????    bbbBBBBB ????????    bbbBBBBB ????????    bbbBBBBB ????????
		const uint16x8_t blue_intermediate_u_16x8 = vshrq_n_u16(vreinterpretq_u16_u8(source_u_8x16), 11);
		const uint16x8_t blue_u_16x8 = vsliq_n_u16(blue_intermediate_u_16x8, blue_intermediate_u_16x8, 3);
		const uint8x8_t blue_u_8x8 = vmovn_u16(blue_u_16x8);

		// precise color space conversion:
		// Y = 0.299 * R + 0.587 * G + 0.114 * B

		// approximation:
		// Y = (38 * R + 75 * G + 15 * B + 64) / 128

		uint16x8_t intermediateResults_u_16x8 = vmull_u8(red_u_8x8, constant_38_u_8x8);
		intermediateResults_u_16x8 = vmlal_u8(intermediateResults_u_16x8, green_u_8x8, constant_75_u_8x8);
		intermediateResults_u_16x8 = vmlal_u8(intermediateResults_u_16x8, blue_u_8x8, constant_15_u_8x8);

		// we shift the 16 bit values by 7 bits (= 128), apply rounding, and narrow the 16 bit integers to 8 bit integers within one operation
		const uint8x8_t gray_u_8x8 = vqrshrn_n_u16(intermediateResults_u_16x8, 7); // gray_u_8x8 = (intermediateResults_u_16x8 + 2^6) >> 2^7

		// and we can store the result
		vst1_u8(target + x, gray_u_8x8);
	}
}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

}

}
