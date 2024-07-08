/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameConverterY_U_V12.h"

#include "ocean/base/Memory.h"

namespace Ocean
{

namespace CV
{

void FrameConverterY_U_V12::convertOneRowY_U_V12ToRGBA32Precision6Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 2u && width % 2u == 0u);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourcePlane0PaddingElements
	// uint32_t: sourcePlane1PaddingElements
	// uint32_t: sourcePlane2PaddingElements
	// uint32_t: targetZippedPaddingElements
	// uint32_t: alphaChannel

	const unsigned int sourcePlane0PaddingElements = (unsigned int)intOptions[0];
	const unsigned int sourcePlane1PaddingElements = (unsigned int)intOptions[1];
	const unsigned int sourcePlane2PaddingElements = (unsigned int)intOptions[2];
	const unsigned int targetZippedPaddingElements = (unsigned int)intOptions[3];

	const unsigned char* sourcePlane0 = (const unsigned char*)(sources[0]);
	const unsigned char* sourcePlane1 = (const unsigned char*)(sources[1]);
	const unsigned char* sourcePlane2 = (const unsigned char*)(sources[2]);

	unsigned char* targetZipped = (unsigned char*)(targets[0]);

	const unsigned int width_2 = width / 2u;

	const unsigned int sourcePlane0StrideElements = width + sourcePlane0PaddingElements;
	const unsigned int sourcePlane1StrideElements = width_2 + sourcePlane1PaddingElements;
	const unsigned int sourcePlane2StrideElements = width_2 + sourcePlane2PaddingElements;
	const unsigned int targetZippedStrideElements = width * 4u + targetZippedPaddingElements;

	const int valueChannel3 = intOptions[4];
	ocean_assert(valueChannel3 >= 0 && valueChannel3 <= 255);

	Memory mirroredTargetMemory;

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	const unsigned char* sPlane0 = sourcePlane0 + multipleRowIndex * sourcePlane0StrideElements;
	const unsigned char* sPlane1 = sourcePlane1 + (multipleRowIndex / 2u) * sourcePlane1StrideElements;
	const unsigned char* sPlane2 = sourcePlane2 + (multipleRowIndex / 2u) * sourcePlane2StrideElements;

	unsigned char* targetFinal = flipTarget ? (targetZipped + (height - multipleRowIndex - 1u) * targetZippedStrideElements) : targetZipped + multipleRowIndex * targetZippedStrideElements;
	unsigned char* target = nullptr;

	if (mirrorTarget)
	{
		// we create two temporary rows for mirroring

		mirroredTargetMemory = Memory::create<unsigned char>(width * 4u);

		target = mirroredTargetMemory.data<unsigned char>();
	}
	else
	{
		target = targetFinal;
	}

	const unsigned char* const sPlane0End = sPlane0 + width;

	// Approximation with 6 bit precision:
	//      | R |     | 75    0     102 |   | Y -  16 |
	// 64 * | G |  =  | 75   -25   -52  | * | U - 128 |
	//      | B |     | 75   128     0  |   | V - 128 |

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr unsigned int blockSize = 16u;
	const unsigned int blocks = width / blockSize;

	if (blocks != 0u)
	{
		const int16x8_t factorChannel00_64_s_16x8 = vdupq_n_s16(75);

		const int16x8_t factorChannel11_64_s_16x8 = vdupq_n_s16(-25);
		const int16x8_t factorChannel21_64_s_16x8 = vdupq_n_s16(128);

		const int16x8_t factorChannel02_64_s_16x8 = vdupq_n_s16(102);
		const int16x8_t factorChannel12_64_s_16x8 = vdupq_n_s16(-52);

		const uint8x8_t bias0_u_8x8 = vdup_n_u8(16u);
		const uint8x8_t bias1_u_8x8 = vdup_n_u8(128u);

		const uint8x16_t valueChannel3_u_8x16 = vdupq_n_u8((unsigned char)(valueChannel3));

		for (unsigned int n = 0u; n < blocks; ++n)
		{
			const uint8x16_t sPlane0_u_8x16 = vld1q_u8(sPlane0);
			const uint8x8_t source1_u_8x8 = vld1_u8(sPlane1);
			const uint8x8_t source2_u_8x8 = vld1_u8(sPlane2);

			// Y' = Y - bias0, U' = U - bias1, V' = V - bias2
			const int16x8_t source0_A_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(sPlane0_u_8x16), bias0_u_8x8));
			const int16x8_t source0_B_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(sPlane0_u_8x16), bias0_u_8x8));

			const int16x8_t source1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(source1_u_8x8, bias1_u_8x8));
			const int16x8_t source2_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(source2_u_8x8, bias1_u_8x8));

			const int16x8_t sourceMultiplied0_A_s_16x8 = vmulq_s16(source0_A_s_16x8, factorChannel00_64_s_16x8);
			const int16x8_t sourceMultiplied0_B_s_16x8 = vmulq_s16(source0_B_s_16x8, factorChannel00_64_s_16x8);


			// now we apply the 3x3 matrix multiplication

			int16x8_t intermediateResults1_s_16x8 = vmulq_s16(source1_s_16x8, factorChannel11_64_s_16x8);
			int16x8_t intermediateResults2_s_16x8 = vmulq_s16(source1_s_16x8, factorChannel21_64_s_16x8);

			int16x8_t intermediateResults0_s_16x8 = vmulq_s16(source2_s_16x8, factorChannel02_64_s_16x8); // downsampled channel 2
			intermediateResults1_s_16x8 = vqaddq_s16(intermediateResults1_s_16x8, vmulq_s16(source2_s_16x8, factorChannel12_64_s_16x8));

			const int16x8x2_t intermediateResults0_AB_s_16x8x2 = vzipq_s16(intermediateResults0_s_16x8, intermediateResults0_s_16x8); // upsampling channel the results
			const int16x8x2_t intermediateResults1_AB_s_16x8x2 = vzipq_s16(intermediateResults1_s_16x8, intermediateResults1_s_16x8);
			const int16x8x2_t intermediateResults2_AB_s_16x8x2 = vzipq_s16(intermediateResults2_s_16x8, intermediateResults2_s_16x8);


			const int16x8_t intermediateResults0_A_s_16x8 = vqaddq_s16(intermediateResults0_AB_s_16x8x2.val[0], sourceMultiplied0_A_s_16x8); // channel 0
			const int16x8_t intermediateResults1_A_s_16x8 = vqaddq_s16(intermediateResults1_AB_s_16x8x2.val[0], sourceMultiplied0_A_s_16x8);
			const int16x8_t intermediateResults2_A_s_16x8 = vqaddq_s16(intermediateResults2_AB_s_16x8x2.val[0], sourceMultiplied0_A_s_16x8);

			const int16x8_t intermediateResults0_B_s_16x8 = vqaddq_s16(intermediateResults0_AB_s_16x8x2.val[1], sourceMultiplied0_B_s_16x8);
			const int16x8_t intermediateResults1_B_s_16x8 = vqaddq_s16(intermediateResults1_AB_s_16x8x2.val[1], sourceMultiplied0_B_s_16x8);
			const int16x8_t intermediateResults2_B_s_16x8 = vqaddq_s16(intermediateResults2_AB_s_16x8x2.val[1], sourceMultiplied0_B_s_16x8);


			// writing results

			uint8x16x4_t results_u_8x16x4;

			// saturated narrow signed to unsigned, normalized by 2^6
			results_u_8x16x4.val[0] = vcombine_u8(vqrshrun_n_s16(intermediateResults0_A_s_16x8, 6), vqrshrun_n_s16(intermediateResults0_B_s_16x8, 6));
			results_u_8x16x4.val[1] = vcombine_u8(vqrshrun_n_s16(intermediateResults1_A_s_16x8, 6), vqrshrun_n_s16(intermediateResults1_B_s_16x8, 6));
			results_u_8x16x4.val[2] = vcombine_u8(vqrshrun_n_s16(intermediateResults2_A_s_16x8, 6), vqrshrun_n_s16(intermediateResults2_B_s_16x8, 6));
			results_u_8x16x4.val[3] = valueChannel3_u_8x16;

			// and we can store the result
			vst4q_u8(target, results_u_8x16x4);

			sPlane0 += blockSize;
			sPlane1 += blockSize / 2u;
			sPlane2 += blockSize / 2u;

			target += blockSize * 4u;
		}
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (sPlane0 != sPlane0End)
	{
		ocean_assert(sPlane0 < sPlane0End);

		const short source1 = short(sPlane1[0]) - short(128);
		const short source2 = short(sPlane2[0]) - short(128);

		const int intermediate0 = source1 * short(0) + source2 * short(102);
		const int intermediate1 = source1 * short(-25) + source2 * short(-52);
		const int intermediate2 = source1 * short(128) + source2 * short(0);

		// first and second upper pixel

		const short source0Left = (short(sPlane0[0]) - short(16)) * short(75);
		const short source0Right = (short(sPlane0[1]) - short(16)) * short(75);

		target[0] = (unsigned char)(minmax<int>(0, (source0Left + intermediate0) / 64, 255));
		target[1] = (unsigned char)(minmax<int>(0, (source0Left + intermediate1) / 64, 255));
		target[2] = (unsigned char)(minmax<int>(0, (source0Left + intermediate2) / 64, 255));
		target[3] = (unsigned char)valueChannel3;

		target[4] = (unsigned char)(minmax<int>(0, (source0Right + intermediate0) / 64, 255));
		target[5] = (unsigned char)(minmax<int>(0, (source0Right + intermediate1) / 64, 255));
		target[6] = (unsigned char)(minmax<int>(0, (source0Right + intermediate2) / 64, 255));
		target[7] = (unsigned char)valueChannel3;

		sPlane0 += 2;
		sPlane1++;
		sPlane2++;

		target += 2u * 4u;
	}

	if (mirrorTarget)
	{
		// we mirror the upper and lower rows from the temporary buffer to the actual buffer

		CV::FrameChannels::reverseRowPixelOrder<unsigned char, 4u>(target - width * 4u, targetFinal, width);
	}
}

void FrameConverterY_U_V12::mapY_U_V12ToY_UV12(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 2u && width % 2u == 0u);

	ocean_assert(options != nullptr);
	const uint32_t* intOptions = reinterpret_cast<const uint32_t*>(options);
	ocean_assert(intOptions != nullptr);

	ocean_assert_and_suppress_unused(multipleRowIndex < height / 2u, height);

	ocean_assert_and_suppress_unused(conversionFlag == CONVERT_NORMAL, conversionFlag);

	// options layout:
	// options[0] uint32_t: ySourcePaddingElements
	// options[1] uint32_t: uSourcePaddingElements
	// options[2] uint32_t: vSourcePaddingElements
	// options[3] uint32_t: yTargetPaddingElements
	// options[4] uint32_t: uvTargetPaddingElements
	// options[5] uint32_t: ySourcePixelStride
	// options[6] uint32_t: uSourcePixelStride
	// options[7] uint32_t: vSourcePixelStride

	const unsigned int ySourcePaddingElements = intOptions[0];
	const unsigned int uSourcePaddingElements = intOptions[1];
	const unsigned int vSourcePaddingElements = intOptions[2];

	const unsigned int yTargetPaddingElements = intOptions[3];
	const unsigned int uvTargetPaddingElements = intOptions[4];

	const unsigned int ySourcePixelStride = intOptions[5];
	const unsigned int uSourcePixelStride = intOptions[6];
	const unsigned int vSourcePixelStride = intOptions[7];

	const uint8_t* const ySource = (const uint8_t*)(sources[0]);
	const uint8_t* const uSource = (const uint8_t*)(sources[1]);
	const uint8_t* const vSource = (const uint8_t*)(sources[2]);

	uint8_t* const yTarget = (uint8_t*)(targets[0]);
	uint8_t* const uvTarget = (uint8_t*)(targets[1]);

	const unsigned int width_2 = width / 2u;

	const unsigned int ySourceStrideElements = width + ySourcePaddingElements;
	const unsigned int uSourceStrideElements = width_2 + uSourcePaddingElements;
	const unsigned int vSourceStrideElements = width_2 + vSourcePaddingElements;

	const unsigned int yTargetStrideElements = width + yTargetPaddingElements;
	const unsigned int uvTargetStrideElements = width + uvTargetPaddingElements;

	const unsigned int yRowStart = multipleRowIndex * 2u;
	const unsigned int& uvRow = multipleRowIndex;

	if (ySourcePixelStride == 1u)
	{
		if (ySourcePaddingElements == 0u && yTargetPaddingElements == 0u)
		{
			memcpy(yTarget + yRowStart * yTargetStrideElements, ySource + yRowStart * ySourceStrideElements, sizeof(uint8_t) * width * 2u);
		}
		else
		{
			memcpy(yTarget + (yRowStart + 0u) * yTargetStrideElements, ySource + (yRowStart + 0u) * ySourceStrideElements, sizeof(uint8_t) * width);
			memcpy(yTarget + (yRowStart + 1u) * yTargetStrideElements, ySource + (yRowStart + 1u) * ySourceStrideElements, sizeof(uint8_t) * width);
		}
	}
	else
	{
		ocean_assert(ySourcePixelStride >= 2u);

		for (unsigned int y = yRowStart; y < yRowStart + 1u; ++y)
		{
			const uint8_t* ySourceRow = ySource + y * ySourceStrideElements;
			uint8_t* yTargetRow = yTarget + y * yTargetStrideElements;

			for (unsigned int x = 0u; x < width; ++x)
			{
				yTargetRow[x] = ySourceRow[x * ySourcePixelStride];
			}
		}
	}

	const uint8_t* uSourceRow = uSource + uvRow * uSourceStrideElements;
	const uint8_t* vSourceRow = vSource + uvRow * vSourceStrideElements;

	uint8_t* uvTargetRow = uvTarget + uvRow * uvTargetStrideElements;

	if (uSourcePixelStride == 1u && vSourcePixelStride == 1u)
	{
		for (unsigned int x = 0u; x < width_2; ++x)
		{
			uvTargetRow[x * 2u + 0u] = uSourceRow[x];
			uvTargetRow[x * 2u + 1u] = vSourceRow[x];
		}
	}
	else if (uSourcePixelStride == 2u && vSourcePixelStride == 2u)
	{
		unsigned int x = 0u;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		const unsigned int blocks16 = width_2 / 16u;

		for (unsigned int n = 0u; n < blocks16; ++n)
		{
			uint8x16x2_t uvSourceRow_u_8x16x2;
			uvSourceRow_u_8x16x2.val[0] = vld2q_u8(uSourceRow + n * 16u * 2u).val[0];
			uvSourceRow_u_8x16x2.val[1] = vld2q_u8(vSourceRow + n * 16u * 2u).val[0];

			vst2q_u8(uvTargetRow + n * 16u * 2u, uvSourceRow_u_8x16x2);
		}

		x += blocks16 * 16u;

#endif // OCEAN_HARDWARE_NEON_VERSION

		while (x < width_2)
		{
			uvTargetRow[x * 2u + 0u] = uSourceRow[x * 2u];
			uvTargetRow[x * 2u + 1u] = vSourceRow[x * 2u];

			++x;
		}
	}
	else
	{
		ocean_assert(uSourcePixelStride >= 3u);
		ocean_assert(vSourcePixelStride >= 3u);

		for (unsigned int x = 0u; x < width_2; ++x)
		{
			uvTargetRow[x * 2u + 0u] = uSourceRow[x * uSourcePixelStride];
			uvTargetRow[x * 2u + 1u] = vSourceRow[x * vSourcePixelStride];
		}
	}
}

}

}
