/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameConverterY10_Packed.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{

const uint8_t* FrameConverterY10_Packed::LookupTableManager::lookupTable(const float gamma)
{
	ocean_assert(gamma > 0.0f && gamma < 2.0f);

	const ScopedLock scopedLock(lock_);

	const LookupTables::const_iterator i = lookupTables_.find(gamma);

	if (i != lookupTables_.cend())
	{
		return i->second.constdata<uint8_t>();
	}

	Memory newMemory = Memory::create<uint8_t>(1024); // lookup with 10 bits

	uint8_t* const values = newMemory.data<uint8_t>();

	for (unsigned int n = 0u; n < 1024u; ++n)
	{
		const float value = NumericF::pow(float(n) * 0.0009775171065f, gamma) * 255.0f + 0.5f;
		ocean_assert(value < 256.0f);

		values[n] = uint8_t(value);
	}

	lookupTables_.insert(std::make_pair(gamma, std::move(newMemory)));

	return values;
}

void FrameConverterY10_Packed::convertY10_PackedToY8GammaApproximated(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const float gamma, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 4u && height >= 1u);
	ocean_assert(width % 4u == 0u);

	ocean_assert(gamma > 0.0f && gamma < 2.0f);

	const unsigned int sourceStrideElements = width * 5u / 4u + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 1u + targetPaddingElements;

	// three linear equations
	//
	// [    0, step0]:  f_0(x) = m_0 * x, with f_0(0) = 0
	// [step0, step1]:  f_1(x) = m_1 * x + c_1
	// [step1, 1    ]:  f_2(x) = m_2 * x + c_2, with f_2(1) = 1

	// m_1 = [f(t_1) - f(t_0)] / [t_1 - t_0]
	// c_1 = f(t_1) - m_1 t_1

	// https://www.desmos.com/calculator/pezgk5slux

	if (gamma <= 1.0f)
	{
		constexpr unsigned int step0 = 40u;
		constexpr unsigned int step1 = 280u;

		constexpr float fStep0 = float(step0);
		constexpr float fStep1 = float(step1);

		const float m0 = 255.0f * NumericF::pow(fStep0 / 1023.0f, gamma) / fStep0;
		const float m1 = 255.0f * (NumericF::pow(fStep1 / 1023.0f, gamma) - NumericF::pow(fStep0 / 1023.0f, gamma)) / (fStep1 - fStep0);
		const float m2 = 255.0f * (NumericF::pow(1.0f, gamma) - NumericF::pow(fStep1 / 1023.0f, gamma)) / (1023.0f - fStep1);

		const float c1 = 255.0f * NumericF::pow(fStep1 / 1023.0f, gamma) - fStep1 * m1;
		const float c2 = 255.0f - 1023.0f * m2;

		const int options[5] = {int(m0 * 256.0f), int(m1 * 256.0f), int(m2 * 256.0f), int(c1), int(c2)};

		const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

		FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, flag, convertRowY10_PackedToY8GammaApproximated<step0, step1>, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 1u>, areContinuous, options, worker);
	}
	else
	{
		constexpr unsigned int step0 = 250u;
		constexpr unsigned int step1 = 600u;

		constexpr float fStep0 = float(step0);
		constexpr float fStep1 = float(step1);

		const float m0 = 255.0f * NumericF::pow(fStep0 / 1023.0f, gamma) / fStep0;
		const float m1 = 255.0f * (NumericF::pow(fStep1 / 1023.0f, gamma) - NumericF::pow(fStep0 / 1023.0f, gamma)) / (fStep1 - fStep0);
		const float m2 = 255.0f * (NumericF::pow(1.0f, gamma) - NumericF::pow(fStep1 / 1023.0f, gamma)) / (1023.0f - fStep1);

		const float c1 = 255.0f * NumericF::pow(fStep1 / 1023.0f, gamma) - fStep1 * m1;
		const float c2 = 255.0f - 1023.0f * m2;

		const int options[5] = {int(m0 * 256.0f), int(m1 * 256.0f), int(m2 * 256.0f), int(c1), int(c2)};

		const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

		FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, flag, convertRowY10_PackedToY8GammaApproximated<step0, step1>, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 1u>, areContinuous, options, worker);
	}
}

void FrameConverterY10_Packed::convertRowY10_PackedToY8Linear(const uint8_t* source, uint8_t* target, const size_t size, const void* /*unusedParameters*/)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(size >= 4 && size % 4 == 0);

	/*
	 * Pixel format with byte order Y and 10 bits per pixel, packed so that four consecutive pixels fit into five bytes.
	 * The higher 8 bits of each pixel are stored in the first four bytes, the lower 2 bits of all four pixels are stored in the fifth byte.
	 * Here is the memory layout:
	 * <pre>
	 *   Pixel:  0        1        2        3        0 1 2 3    4        5        6        7        4 5 6 7
	 *    Byte:  0        1        2        3        4          5        6        7        8        9
	 *     Bit:  01234567 89ABCDEF 01234567 89ABCDEF 01234567   01234567 89ABCDEF 01234567 89ABCDEF 01234567
	 * Channel:  0        0        0        0        0 0 0 0    0        0        0        0        0 0 0 0
	 *   Color:  YYYYYYYY YYYYYYYY YYYYYYYY YYYYYYYY YYYYYYYY   YYYYYYYY YYYYYYYY YYYYYYYY YYYYYYYY YYYYYYYYY ........
	 * </pre>
	 */

	size_t blocks4 = size / size_t(4);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	const size_t blocks16 = size / size_t(16);

	for (size_t n = 0; n < blocks16; ++n)
	{
		convert16PixelY10_PackedToY8LinearNEON(source, target);

		target += 16;
		source += 20;
	}

	blocks4 = (size - blocks16 * size_t(16)) / size_t(4);
	ocean_assert(blocks4 <= size / size_t(4));

#endif // OCEAN_HARDWARE_NEON_VERSION

	for (size_t n = 0; n < blocks4; ++n)
	{
		for (unsigned int x = 0u; x < 4u; ++x)
		{
			target[x] = source[x]; // copying the upper 8 bits is identical to a linear conversion
		}

		target += 4;
		source += 5;
	}
}

void FrameConverterY10_Packed::convertRowY10_PackedToY8GammaLUT(const uint8_t* source, uint8_t* target, const size_t size, const void* parameters)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(size >= 4 && size % 4 == 0);
	ocean_assert(parameters != nullptr);

	/*
	 * Pixel format with byte order Y and 10 bits per pixel, packed so that four consecutive pixels fit into five bytes.
	 * The higher 8 bits of each pixel are stored in the first four bytes, the lower 2 bits of all four pixels are stored in the fifth byte.
	 * Here is the memory layout:
	 * <pre>
	 *   Pixel:  0        1        2        3        0 1 2 3    4        5        6        7        4 5 6 7
	 *    Byte:  0        1        2        3        4          5        6        7        8        9
	 *     Bit:  01234567 89ABCDEF 01234567 89ABCDEF 01234567   01234567 89ABCDEF 01234567 89ABCDEF 01234567
	 * Channel:  0        0        0        0        0 0 0 0    0        0        0        0        0 0 0 0
	 *   Color:  YYYYYYYY YYYYYYYY YYYYYYYY YYYYYYYY YYYYYYYY   YYYYYYYY YYYYYYYY YYYYYYYY YYYYYYYY YYYYYYYYY ........
	 * </pre>
	 */

	const uint8_t* const lookupValues = reinterpret_cast<const uint8_t*>(parameters);

	size_t remaining = size;

#ifdef __APPLE__

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	const size_t blocks12 = remaining / size_t(12);

	uint16_t unpacked[16];

	uint16x8_t unpackedAB_u_16x8;
	uint16x4_t unpackedC_u_16x4;

	for (size_t n = 1; n < blocks12; ++n)
	{
		FrameConverter::unpack15ElementsBayerMosaicPacked10BitNEON<true>(source, unpackedAB_u_16x8, unpackedC_u_16x4);

		vst1q_u16(unpacked + 0, unpackedAB_u_16x8);
		vst1_u16(unpacked + 8, unpackedC_u_16x4);

		target[0] = lookupValues[unpacked[0]];
		target[1] = lookupValues[unpacked[1]];
		target[2] = lookupValues[unpacked[2]];
		target[3] = lookupValues[unpacked[3]];
		target[4] = lookupValues[unpacked[4]];
		target[5] = lookupValues[unpacked[5]];
		target[6] = lookupValues[unpacked[6]];
		target[7] = lookupValues[unpacked[7]];
		target[8] = lookupValues[unpacked[8]];
		target[9] = lookupValues[unpacked[9]];
		target[10] = lookupValues[unpacked[10]];
		target[11] = lookupValues[unpacked[11]];

		target += 12;
		source += 15;
	}

	if (blocks12 != 0)
	{
		FrameConverter::unpack15ElementsBayerMosaicPacked10BitNEON<false>(source, unpackedAB_u_16x8, unpackedC_u_16x4);

		vst1q_u16(unpacked + 0, unpackedAB_u_16x8);
		vst1_u16(unpacked + 8, unpackedC_u_16x4);

		target[0] = lookupValues[unpacked[0]];
		target[1] = lookupValues[unpacked[1]];
		target[2] = lookupValues[unpacked[2]];
		target[3] = lookupValues[unpacked[3]];
		target[4] = lookupValues[unpacked[4]];
		target[5] = lookupValues[unpacked[5]];
		target[6] = lookupValues[unpacked[6]];
		target[7] = lookupValues[unpacked[7]];
		target[8] = lookupValues[unpacked[8]];
		target[9] = lookupValues[unpacked[9]];
		target[10] = lookupValues[unpacked[10]];
		target[11] = lookupValues[unpacked[11]];

		target += 12;
		source += 15;
	}

	remaining = remaining - blocks12 * size_t(12);
	ocean_assert(remaining <= size);

#endif // OCEAN_HARDWARE_NEON_VERSION

#endif // __APPLE__

	const size_t blocks8 = remaining / size_t(8);

	for (size_t n = 0; n < blocks8; ++n)
	{
		const uint16_t value0 = uint16_t(uint16_t(source[0]) << uint16_t(2) | (uint16_t(source[4]) & uint16_t(0b00000011)));
		const uint16_t value1 = uint16_t(uint16_t(source[1]) << uint16_t(2) | ((uint16_t(source[4]) & uint16_t(0b00001100)) >> uint16_t(2)));
		const uint16_t value2 = uint16_t(uint16_t(source[2]) << uint16_t(2) | ((uint16_t(source[4]) & uint16_t(0b00110000)) >> uint16_t(4)));
		const uint16_t value3 = uint16_t(uint16_t(source[3]) << uint16_t(2) | (uint16_t(source[4]) >> uint16_t(6)));

		const uint16_t value4 = uint16_t(uint16_t(source[5]) << uint16_t(2) | (uint16_t(source[9]) & uint16_t(0b00000011)));
		const uint16_t value5 = uint16_t(uint16_t(source[6]) << uint16_t(2) | ((uint16_t(source[9]) & uint16_t(0b00001100)) >> uint16_t(2)));
		const uint16_t value6 = uint16_t(uint16_t(source[7]) << uint16_t(2) | ((uint16_t(source[9]) & uint16_t(0b00110000)) >> uint16_t(4)));
		const uint16_t value7 = uint16_t(uint16_t(source[8]) << uint16_t(2) | (uint16_t(source[9]) >> uint16_t(6)));

		target[0] = lookupValues[value0];
		target[1] = lookupValues[value1];
		target[2] = lookupValues[value2];
		target[3] = lookupValues[value3];

		target[4] = lookupValues[value4];
		target[5] = lookupValues[value5];
		target[6] = lookupValues[value6];
		target[7] = lookupValues[value7];

		target += 8;
		source += 10;
	}

	remaining = remaining - blocks8 * size_t(8);
	ocean_assert(remaining <= size);

	ocean_assert(remaining == 0 || remaining == 4);

	if (remaining == 4)
	{
		const uint16_t value0 = uint16_t(uint16_t(source[0]) << uint16_t(2) | (uint16_t(source[4]) & uint16_t(0b00000011)));
		const uint16_t value1 = uint16_t(uint16_t(source[1]) << uint16_t(2) | ((uint16_t(source[4]) & uint16_t(0b00001100)) >> uint16_t(2)));
		const uint16_t value2 = uint16_t(uint16_t(source[2]) << uint16_t(2) | ((uint16_t(source[4]) & uint16_t(0b00110000)) >> uint16_t(4)));
		const uint16_t value3 = uint16_t(uint16_t(source[3]) << uint16_t(2) | (uint16_t(source[4]) >> uint16_t(6)));

		target[0] = lookupValues[value0];
		target[1] = lookupValues[value1];
		target[2] = lookupValues[value2];
		target[3] = lookupValues[value3];
	}
}

void FrameConverterY10_Packed::convertRowY10_PackedToY10(const uint8_t* source, uint16_t* target, const size_t size, const void* /*unusedParameters*/)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(size >= 4 && size % 4 == 0);

	/*
	 * Pixel format with byte order Y and 10 bits per pixel, packed so that four consecutive pixels fit into five bytes.
	 * The higher 8 bits of each pixel are stored in the first four bytes, the lower 2 bits of all four pixels are stored in the fifth byte.
	 * Here is the memory layout:
	 * <pre>
	 *   Pixel:  0        1        2        3        0 1 2 3    4        5        6        7        4 5 6 7
	 *    Byte:  0        1        2        3        4          5        6        7        8        9
	 *     Bit:  01234567 89ABCDEF 01234567 89ABCDEF 01234567   01234567 89ABCDEF 01234567 89ABCDEF 01234567
	 * Channel:  0        0        0        0        0 0 0 0    0        0        0        0        0 0 0 0
	 *   Color:  YYYYYYYY YYYYYYYY YYYYYYYY YYYYYYYY YYYYYYYY   YYYYYYYY YYYYYYYY YYYYYYYY YYYYYYYY YYYYYYYYY ........
	 * </pre>
	 */

	size_t blocks4 = size / size_t(4);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	const size_t blocks12 = size / size_t(12);

	uint16x8_t unpackedAB_u_16x8;
	uint16x4_t unpackedC_u_16x4;

	for (size_t n = 1; n < blocks12; ++n)
	{
		FrameConverter::unpack15ElementsBayerMosaicPacked10BitNEON<true>(source, unpackedAB_u_16x8, unpackedC_u_16x4);

		vst1q_u16(target + 0, unpackedAB_u_16x8);
		vst1_u16(target + 8, unpackedC_u_16x4);

		target += 12;
		source += 15;
	}

	if (blocks12 != 0)
	{
		FrameConverter::unpack15ElementsBayerMosaicPacked10BitNEON<false>(source, unpackedAB_u_16x8, unpackedC_u_16x4);

		vst1q_u16(target + 0, unpackedAB_u_16x8);
		vst1_u16(target + 8, unpackedC_u_16x4);

		target += 12;
		source += 15;
	}

	blocks4 = (size - blocks12 * size_t(12)) / size_t(4);
	ocean_assert(blocks4 <= size / size_t(4));

#endif // OCEAN_HARDWARE_NEON_VERSION

	for (size_t n = 0; n < blocks4; ++n)
	{
		FrameConverter::FrameConverter::unpack5ElementsBayerMosaicPacked10Bit(source, target);

		target += 4;
		source += 5;
	}
}

void FrameConverterY10_Packed::convertRowY10_PackedToYYY24Linear(const uint8_t* source, uint8_t* target, const size_t size, const void* /*unusedParameters*/)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(size >= 4 && size % 4 == 0);

	const size_t blocks4 = size / size_t(4);

	for (size_t n = 0; n < blocks4; ++n)
	{
		for (unsigned int x = 0u; x < 4u; ++x)
		{
			// simply setting the Y source values to each of the three target channels

			target[3u * x + 0u] = source[x];
			target[3u * x + 1u] = source[x];
			target[3u * x + 2u] = source[x];
		}

		target += 4u * 3u;
		source += 5;
	}
}


}

}
