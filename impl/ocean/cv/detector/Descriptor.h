/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DETECTOR_DESCRIPTOR_H
#define META_OCEAN_CV_DETECTOR_DESCRIPTOR_H

#include "ocean/cv/detector/Detector.h"

#include "ocean/cv/NEON.h"
#include "ocean/cv/SSE.h"

#include <bitset>

namespace Ocean
{

namespace CV
{

namespace Detector
{

/**
 * This class implements the abstract base for arbitrary descriptors.
 * @ingroup cvdetector
 */
class OCEAN_CV_DETECTOR_EXPORT Descriptor
{
	public:

		/**
		 * Creates a new descriptor object.
		 */
		inline Descriptor();

		/**
		 * Determines the hamming distance between two binary descriptors.
		 * @param descriptorA The first descriptor, must be valid
		 * @param descriptorB The second descriptor, must be valid
		 * @return The hamming distance between both descriptors (the number of not identical corresponding bits), with range [0, tBits]
		 * @tparam tBits The number of bits both descriptors have, with range [128, infinity), must be a multiple of 128
		 */
		template <unsigned int tBits>
		static OCEAN_FORCE_INLINE unsigned int calculateHammingDistance(const void* descriptorA, const void* descriptorB);

	protected:

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 30

		/**
		 * Calculates a pop count of an m128i register in 8 bit groups.
		 * @param value Bit string to calculate pop count from
		 * @return Pop count
		 */
		static OCEAN_FORCE_INLINE __m128i popcount8(const __m128i value);

		/**
		 * Calculates a pop count of an m128i register in 64 bit groups.
		 * @param value Bit string to calculate pop count from
		 * @return Pop count
		 */
		static OCEAN_FORCE_INLINE __m128i popcount64(const __m128i value);

#endif

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 42

		/**
		 * Calculates a pop count of an m128i register in 64 bit groups.
		 * @param value Bit string to calculate pop count from
		 * @return Pop count
		 */
		static OCEAN_FORCE_INLINE unsigned int popcount128(const __m128i value);

#endif
};

inline Descriptor::Descriptor()
{
	// nothing to here
}

template <>
OCEAN_FORCE_INLINE unsigned int Descriptor::calculateHammingDistance<128u>(const void* descriptorA, const void* descriptorB)
{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 42

	// the following code uses the following SSE instructions, and needs SSE4.2 or higher

	// SSE2:
	// _mm_lddqu_si128
	// _mm_xor_si128

	// see also popcount128()

	const __m128i descriptorA_m128 = SSE::load128i(descriptorA);
	const __m128i descriptorB_m128 = SSE::load128i(descriptorB);

	const __m128i xor_m128 = _mm_xor_si128(descriptorA_m128, descriptorB_m128);

	return popcount128(xor_m128);

#elif defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 30

	// the following code uses the following SSE instructions, and needs SSE3 or higher

	// SSE2:
	// _mm_load1_pd
	// _mm_loadu_pd
	// _mm_mul_pd
	// _mm_add_pd
	// _mm_storeu_pd

	// see also popcount64()

	const __m128i descriptorA_m128 = SSE::load128i(descriptorA);
	const __m128i descriptorB_m128 = SSE::load128i(descriptorB);

	const __m128i xor_m128 = _mm_xor_si128(descriptorA_m128, descriptorB_m128);

	const __m128i countLowHigh_m128_64 = popcount64(xor_m128);
	const __m128i countHigh_m128_64 = _mm_unpackhi_epi64(countLowHigh_m128_64, countLowHigh_m128_64);
	const __m128i count_m128 = _mm_add_epi32(countLowHigh_m128_64, countHigh_m128_64);

	return _mm_cvtsi128_si32(count_m128);

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	const uint8x16_t descriptorA_u_8x16 = vld1q_u8((const uint8_t*)(descriptorA));
	const uint8x16_t descriptorB_u_8x16 = vld1q_u8((const uint8_t*)(descriptorB));

	const uint8x16_t xor_u_8x16 = veorq_u8(descriptorA_u_8x16, descriptorB_u_8x16);

	const uint8x16_t count_u_8x16 = vcntq_u8(xor_u_8x16);
	const uint16x8_t count_u_16x8 = vpaddlq_u8(count_u_8x16);
	const uint32x4_t count_u_32x4 = vpaddlq_u16(count_u_16x8);

	return NEON::sum32x4ByLanes(count_u_32x4);

#else

	typedef std::bitset<128> Bitset;
	static_assert(sizeof(Bitset) == 128u / 8u, "Invalid data type!");

	Bitset bitsetA, bitsetB;

	memcpy(&bitsetA, descriptorA, sizeof(Bitset));
	memcpy(&bitsetB, descriptorB, sizeof(Bitset));

	return (unsigned int)(bitsetA ^ bitsetB).count();

#endif
}

template <>
OCEAN_FORCE_INLINE unsigned int Descriptor::calculateHammingDistance<256u>(const void* descriptorA, const void* descriptorB)
{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 42

	// the following code uses the following SSE instructions, and needs SSE4.2 or higher

	// SSE2:
	// _mm_lddqu_si128
	// _mm_xor_si128

	// see also popcount128()

	const __m128i descriptorA_m128_0 = SSE::load128i(((const __m128i*)descriptorA) + 0);
	const __m128i descriptorA_m128_1 = SSE::load128i(((const __m128i*)descriptorA) + 1);

	const __m128i descriptorB_m128_0 = SSE::load128i(((const __m128i*)descriptorB) + 0);
	const __m128i descriptorB_m128_1 = SSE::load128i(((const __m128i*)descriptorB) + 1);

	const __m128i xor_m128_0 = _mm_xor_si128(descriptorA_m128_0, descriptorB_m128_0);
	const __m128i xor_m128_1 = _mm_xor_si128(descriptorA_m128_1, descriptorB_m128_1);

	return popcount128(xor_m128_0) + popcount128(xor_m128_1);

#elif defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 30

	// the following code uses the following SSE instructions, and needs SSE3 or higher

	// SSE2:
	// _mm_load1_pd
	// _mm_loadu_pd
	// _mm_mul_pd
	// _mm_add_pd
	// _mm_storeu_pd

	// see also popcount64()

	const __m128i descriptorA_m128_0 = SSE::load128i(((const __m128i*)descriptorA) + 0);
	const __m128i descriptorA_m128_1 = SSE::load128i(((const __m128i*)descriptorA) + 1);

	const __m128i descriptorB_m128_0 = SSE::load128i(((const __m128i*)descriptorB) + 0);
	const __m128i descriptorB_m128_1 = SSE::load128i(((const __m128i*)descriptorB) + 1);

	const __m128i xor_m128_0 = _mm_xor_si128(descriptorA_m128_0, descriptorB_m128_0);
	const __m128i xor_m128_1 = _mm_xor_si128(descriptorA_m128_1, descriptorB_m128_1);

	const __m128i countLowHigh_m128_64_0 = popcount64(xor_m128_0);
	const __m128i countHigh_m128_64_0 = _mm_unpackhi_epi64(countLowHigh_m128_64_0, countLowHigh_m128_64_0);
	const __m128i count_m128_0 = _mm_add_epi32(countLowHigh_m128_64_0, countHigh_m128_64_0);
	const unsigned int hammingDistance_0 = _mm_cvtsi128_si32(count_m128_0);

	const __m128i countLowHigh_m128_64_1 = popcount64(xor_m128_1);
	const __m128i countHigh_m128_64_1 = _mm_unpackhi_epi64(countLowHigh_m128_64_1, countLowHigh_m128_64_1);
	const __m128i count_m128_1 = _mm_add_epi32(countLowHigh_m128_64_1, countHigh_m128_64_1);
	const unsigned int hammingDistance_1 = _mm_cvtsi128_si32(count_m128_1);

	return hammingDistance_0 + hammingDistance_1;

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	const uint8x16_t descriptorA_u_8x16_0 = vld1q_u8((const uint8_t*)(descriptorA) + 0);
	const uint8x16_t descriptorA_u_8x16_1 = vld1q_u8((const uint8_t*)(descriptorA) + 16);

	const uint8x16_t descriptorB_u_8x16_0 = vld1q_u8((const uint8_t*)(descriptorB) + 0);
	const uint8x16_t descriptorB_u_8x16_1 = vld1q_u8((const uint8_t*)(descriptorB) + 16);

	const uint8x16_t xor_u_8x16_0 = veorq_u8(descriptorA_u_8x16_0, descriptorB_u_8x16_0);
	const uint8x16_t xor_u_8x16_1 = veorq_u8(descriptorA_u_8x16_1, descriptorB_u_8x16_1);

	const uint8x16_t count_u_8x16 = vaddq_u8(vcntq_u8(xor_u_8x16_0), vcntq_u8(xor_u_8x16_1));
	const uint16x8_t count_u_16x8 = vpaddlq_u8(count_u_8x16);
	const uint32x4_t count_u_32x4 = vpaddlq_u16(count_u_16x8);

	return NEON::sum32x4ByLanes(count_u_32x4);

#else

	typedef std::bitset<256> Bitset;
	static_assert(sizeof(Bitset) == 256u / 8u, "Invalid data type!");

	Bitset bitsetA, bitsetB;

	memcpy(&bitsetA, descriptorA, sizeof(Bitset));
	memcpy(&bitsetB, descriptorB, sizeof(Bitset));

	return (unsigned int)(bitsetA ^ bitsetB).count();

#endif
}

template <unsigned int tBits>
OCEAN_FORCE_INLINE unsigned int Descriptor::calculateHammingDistance(const void* descriptorA, const void* descriptorB)
{
	static_assert(tBits >= 128u && tBits % 128u == 0u, "Invalid bit number!");

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 42

	// the following code uses the following SSE instructions, and needs SSE4.2 or higher

	// SSE2:
	// _mm_lddqu_si128
	// _mm_xor_si128

	// see also popcount128()

	unsigned int result = 0u;

	for (unsigned int n = 0u; n < tBits / 128u; ++n)
	{
		const __m128i descriptorA_m128 = SSE::load128i(((const __m128i*)descriptorA) + n);
		const __m128i descriptorB_m128 = SSE::load128i(((const __m128i*)descriptorB) + n);

		const __m128i xor_m128 = _mm_xor_si128(descriptorA_m128, descriptorB_m128);

		result += popcount128(xor_m128);
	}

	return result;

#elif defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 30

	// the following code uses the following SSE instructions, and needs SSE3 or higher

	// SSE2:
	// _mm_load1_pd
	// _mm_loadu_pd
	// _mm_mul_pd
	// _mm_add_pd
	// _mm_storeu_pd

	// see also popcount64()

	unsigned int result = 0u;

	for (unsigned int n = 0u; n < tBits / 128u; ++n)
	{
		const __m128i descriptorA_m128 = SSE::load128i(((const __m128i*)descriptorA) + n);
		const __m128i descriptorB_m128 = SSE::load128i(((const __m128i*)descriptorB) + n);

		const __m128i xor_m128 = _mm_xor_si128(descriptorA_m128, descriptorB_m128);

		const __m128i countLowHigh_m128_64 = popcount64(xor_m128);
		const __m128i countHigh_m128_64 = _mm_unpackhi_epi64(countLowHigh_m128_64, countLowHigh_m128_64);
		const __m128i count_m128 = _mm_add_epi32(countLowHigh_m128_64, countHigh_m128_64);

		result += _mm_cvtsi128_si32(count_m128);
	}

	return result;

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	uint32x4_t result_u_32x4 = vdupq_n_u32(0u);

	for (unsigned int n = 0u; n < tBits / 128u; ++n)
	{
		const uint8x16_t descriptorA_u_8x16 = vld1q_u8((const uint8_t*)(descriptorA) + 16u * n);
		const uint8x16_t descriptorB_u_8x16 = vld1q_u8((const uint8_t*)(descriptorB) + 16u * n);

		const uint8x16_t xor_u_8x16 = veorq_u8(descriptorA_u_8x16, descriptorB_u_8x16);

		const uint8x16_t count_u_8x16 = vcntq_u8(xor_u_8x16);
		const uint16x8_t count_u_16x8 = vpaddlq_u8(count_u_8x16);
		const uint32x4_t count_u_32x4 = vpaddlq_u16(count_u_16x8);

		result_u_32x4 = vaddq_u32(result_u_32x4, count_u_32x4);
	}

	return NEON::sum32x4ByLanes(result_u_32x4);

#else

	typedef std::bitset<tBits> Bitset;
	static_assert(sizeof(Bitset) == tBits / 8u, "Invalid data type!");

	Bitset bitsetA, bitsetB;

	memcpy(&bitsetA, descriptorA, sizeof(Bitset));
	memcpy(&bitsetB, descriptorB, sizeof(Bitset));

	return (unsigned int)(bitsetA ^ bitsetB).count();

#endif
}

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 30

OCEAN_FORCE_INLINE __m128i Descriptor::popcount8(const __m128i value)
{
	// the following code uses the following SSE instructions, and needs SSE3 or higher

	// SSE2:
	// _mm_set1_epi8
	// _mm_setr_epi8
	// _mm_and_si128
	// _mm_add_epi8

	// SSE3:
	// _mm_shuffle_epi8

	const __m128i popcount_mask = _mm_set1_epi8(0x0F);
	const __m128i popcount_table = _mm_setr_epi8(0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4);
	const __m128i pcnt0 = _mm_shuffle_epi8(popcount_table, _mm_and_si128(value, popcount_mask));
	const __m128i pcnt1 = _mm_shuffle_epi8(popcount_table, _mm_and_si128(_mm_srli_epi16(value, 4), popcount_mask));
	return _mm_add_epi8(pcnt0, pcnt1);
}

OCEAN_FORCE_INLINE __m128i Descriptor::popcount64(const __m128i value)
{
	// the following code uses the following SSE instructions, and needs SSE3 or higher

	// SSE2:
	// _mm_sad_epu8
	// _mm_setzero_si128

	// see also popcount8()

	const __m128i cnt8 = popcount8(value);
	return _mm_sad_epu8(cnt8, _mm_setzero_si128());
}

#endif

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 42

OCEAN_FORCE_INLINE unsigned int Descriptor::popcount128(const __m128i value)
{
	// the following code uses the following SSE instructions, and needs SSE4.2 or higher

	// SSE2:
	// _mm_cvtsi128_si64
	// _mm_srli_si128

	// SSE4.2:
	// __popcnt64
	// __popcnt

#if defined(_WIN64) || TARGET_OS_MAC == 1

	return (unsigned int)__popcnt64(_mm_cvtsi128_si64(value)) + (unsigned int)__popcnt64(_mm_cvtsi128_si64(_mm_srli_si128(value, 8)));

#else

	return (unsigned int)__popcnt(_mm_cvtsi128_si32(value)) + (unsigned int)__popcnt(_mm_cvtsi128_si32(_mm_srli_si128(value, 4))) + (unsigned int)__popcnt(_mm_cvtsi128_si32(_mm_srli_si128(value, 4))) + (unsigned int)__popcnt(_mm_cvtsi128_si32(_mm_srli_si128(value, 4)));

#endif
}

#endif

}

}

}

#endif // META_OCEAN_CV_DETECTOR_DESCRIPTOR_H
