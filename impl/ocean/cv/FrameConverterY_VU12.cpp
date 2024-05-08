/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameConverterY_VU12.h"

#include "ocean/base/Memory.h"

namespace Ocean
{

namespace CV
{

void FrameConverterY_VU12::convertOneRowY_VU12FullRangeToBGRA32FullRangeAndroidPrecision6Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 2u && width % 2u == 0u);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourcePlanePaddingElements
	// uint32_t: sourceZippedPaddingElements
	// uint32_t: targetZippedPaddingElements
	// uint32_t: alphaValue

	const unsigned int ySourcePaddingElements = (unsigned int)intOptions[0];
	const unsigned int vuSourcePaddingElements = (unsigned int)intOptions[1];
	const unsigned int bgraTargetPaddingElements = (unsigned int)intOptions[2];

	const uint8_t* ySource = (const uint8_t*)sources[0];
	const uint8_t* vuSource = (const uint8_t*)sources[1];

	uint8_t* bgraTarget = (uint8_t*)targets[0];

	const unsigned int ySourceStrideElements = width + ySourcePaddingElements;
	const unsigned int vuSourceStrideElements = width + vuSourcePaddingElements; // 2x2 downsampling but 2 channels
	const unsigned int bgraTargetStrideElements = width * 4u + bgraTargetPaddingElements;

	ocean_assert(intOptions[3] >= 0 && intOptions[3] <= 255);
	const uint8_t alphaValue = uint8_t(intOptions[3]);

	Memory mirroredTargetMemory;

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	const uint8_t* yUpper = ySource + multipleRowIndex * ySourceStrideElements;
	const uint8_t* vu = vuSource + (multipleRowIndex / 2u) * vuSourceStrideElements;

	uint8_t* tUpperFinal = flipTarget ? (bgraTarget + (height - multipleRowIndex - 1u) * bgraTargetStrideElements) : bgraTarget + multipleRowIndex * bgraTargetStrideElements;
	uint8_t* tUpper = nullptr;

	if (mirrorTarget)
	{
		// we create two temporary rows for mirroring

		mirroredTargetMemory = Memory::create<uint8_t>(width * 4u);

		tUpper = mirroredTargetMemory.data<uint8_t>() + 0;
	}
	else
	{
		tUpper = tUpperFinal;
	}

	const uint8_t* const yUpperEnd = yUpper + width;

	// the documention of this function designed for YVU24 to BGR24 conversion

	// precise color space conversion:
	// | B |   |  1    0.0         1.732446   -221.753088 |   | Y |
	// | G | = |  1   -0.698001   -0.3376335   132.561152 | * | U |
	// | R |   |  1    1.370705    0.0        -175.45024  |   | V |
	//                                                        | 1 |

	// approximation:
	// B = 64 * Y   +  0 * (V - 128)   + 111 * (U - 128)
	// G = 64 * Y   - 45 * (V - 128)   -  22 * (U - 128)
	// R = 64 * Y   + 88 * (V - 128)   +   0 * (U - 128)

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	const uint8x8_t constant_128_u_8x8 = vdup_n_u8(128);
	const int16x8_t constant_22_s_16x8 = vdupq_n_s16(-22);
	const int16x8_t constant_111_s_16x8 = vdupq_n_s16(111);
	const int16x8_t constant_88_s_16x8 = vdupq_n_s16(88);
	const int16x8_t constant_45_s_16x8 = vdupq_n_s16(-45);
	const uint8x16_t alphaValue_u_8x16 = vdupq_n_u8(alphaValue);

	constexpr size_t blockSize = 16;
	const size_t blocks = width / blockSize;

	for (unsigned int n = 0u; n < blocks; ++n)
	{
		const uint8x16_t ySourceUpper_u_8x16 = vld1q_u8(yUpper);

		const int16x8_t ySourceUpperMultiplied_low_s_16x8 = vreinterpretq_s16_u16(vshll_n_u8(vget_low_u8(ySourceUpper_u_8x16), 6)); // Y * 64
		const int16x8_t ySourceUpperMultiplied_high_s_16x8 = vreinterpretq_s16_u16(vshll_n_u8(vget_high_u8(ySourceUpper_u_8x16), 6));

		const uint8x8x2_t vu_u_8x8x2 = vld2_u8(vu);

		const int16x8_t v_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vu_u_8x8x2.val[0], constant_128_u_8x8)); // V' = V - 128
		const int16x8_t u_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vu_u_8x8x2.val[1], constant_128_u_8x8)); // U' = U - 128


		const int16x8_t bIntermediateResults_s_16x8 = vmulq_s16(u_s_16x8, constant_111_s_16x8); // 111 * U'
		const int16x8_t gIntermediateResults_s_16x8 = vmlaq_s16(vmulq_s16(v_s_16x8, constant_45_s_16x8), u_s_16x8, constant_22_s_16x8); // -45 * V' - 22 * V'
		const int16x8_t rIntermediateResults_s_16x8 = vmulq_s16(v_s_16x8, constant_88_s_16x8); // 88 * V'


		// bI0 bI1 bI2 bI3 ... -> bI0 bI0 bI1 bI1 bI2 bI2 bI3 bI3 ...

		const int16x8x2_t bIntermediateResults_s_16x8x2 = vzipq_s16(bIntermediateResults_s_16x8, bIntermediateResults_s_16x8);
		const int16x8x2_t gIntermediateResults_s_16x8x2 = vzipq_s16(gIntermediateResults_s_16x8, gIntermediateResults_s_16x8);
		const int16x8x2_t rIntermediateResults_s_16x8x2 = vzipq_s16(rIntermediateResults_s_16x8, rIntermediateResults_s_16x8);

		// now combining Y and bI, gI, and rI

		const int16x8_t bResultUpper_low_s_16x8 = vaddq_s16(bIntermediateResults_s_16x8x2.val[0], ySourceUpperMultiplied_low_s_16x8);
		const int16x8_t bResultUpper_high_s_16x8 = vaddq_s16(bIntermediateResults_s_16x8x2.val[1], ySourceUpperMultiplied_high_s_16x8);

		const int16x8_t gResultUpper_low_s_16x8 = vaddq_s16(gIntermediateResults_s_16x8x2.val[0], ySourceUpperMultiplied_low_s_16x8);
		const int16x8_t gResultUpper_high_s_16x8 = vaddq_s16(gIntermediateResults_s_16x8x2.val[1], ySourceUpperMultiplied_high_s_16x8);

		const int16x8_t rResultUpper_low_s_16x8 = vaddq_s16(rIntermediateResults_s_16x8x2.val[0], ySourceUpperMultiplied_low_s_16x8);
		const int16x8_t rResultUpper_high_s_16x8 = vaddq_s16(rIntermediateResults_s_16x8x2.val[1], ySourceUpperMultiplied_high_s_16x8);


		uint8x16x4_t resultsUpper_u_8x16x4;
		resultsUpper_u_8x16x4.val[3] = alphaValue_u_8x16;

		// saturated narrow signed to unsigned
		resultsUpper_u_8x16x4.val[0] = vcombine_u8(vqrshrun_n_s16(bResultUpper_low_s_16x8, 6), vqrshrun_n_s16(bResultUpper_high_s_16x8, 6));
		resultsUpper_u_8x16x4.val[1] = vcombine_u8(vqrshrun_n_s16(gResultUpper_low_s_16x8, 6), vqrshrun_n_s16(gResultUpper_high_s_16x8, 6));
		resultsUpper_u_8x16x4.val[2] = vcombine_u8(vqrshrun_n_s16(rResultUpper_low_s_16x8, 6), vqrshrun_n_s16(rResultUpper_high_s_16x8, 6));

		// and we can store the result
		vst4q_u8(tUpper, resultsUpper_u_8x16x4);

		yUpper += blockSize;
		vu += blockSize;

		tUpper += blockSize * 4u;
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (yUpper != yUpperEnd)
	{
		ocean_assert(yUpper < yUpperEnd);

		const int16_t v_128 = vu[0] - 128;
		const int16_t u_128 = vu[1] - 128;

		const int16_t vu0 = 111 * u_128;
		const int16_t vu1 = -45 * v_128 - 22 * u_128;
		const int16_t vu2 = 88 * v_128;

		// first and second upper pixel

		const int16_t yUpperLeft64 = yUpper[0] * 64;
		const int16_t yUpperRight64 = yUpper[1] * 64;

		tUpper[0] = uint8_t(uint16_t(minmax<int16_t>(0, yUpperLeft64 + vu0, 16320)) / 64u);
		tUpper[1] = uint8_t(uint16_t(minmax<int16_t>(0, yUpperLeft64 + vu1, 16320)) / 64u);
		tUpper[2] = uint8_t(uint16_t(minmax<int16_t>(0, yUpperLeft64 + vu2, 16320)) / 64u);
		tUpper[3] = alphaValue;

		tUpper[4] = uint8_t(uint16_t(minmax<int16_t>(0, yUpperRight64 + vu0, 16320)) / 64u);
		tUpper[5] = uint8_t(uint16_t(minmax<int16_t>(0, yUpperRight64 + vu1, 16320)) / 64u);
		tUpper[6] = uint8_t(uint16_t(minmax<int16_t>(0, yUpperRight64 + vu2, 16320)) / 64u);
		tUpper[7] = alphaValue;

		yUpper += 2;
		vu += 2; // 2x2 downsampled, but two channels

		tUpper += 2u * 4u;
	}

	if (mirrorTarget)
	{
		// we mirror the upper and lower rows from the temporary buffer to the actual buffer

		CV::FrameChannels::reverseRowPixelOrder<uint8_t, 4u>(tUpper - width * 4u, tUpperFinal, width);
	}
}

void FrameConverterY_VU12::convertTwoRowsY_VU12FullRangeToBGRA32FullRangeAndroidPrecision6Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 2u && width % 2u == 0u);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourcePlanePaddingElements
	// uint32_t: sourceZippedPaddingElements
	// uint32_t: targetZippedPaddingElements
	// uint32_t: alphaValue

	const unsigned int ySourcePaddingElements = (unsigned int)intOptions[0];
	const unsigned int vuSourcePaddingElements = (unsigned int)intOptions[1];
	const unsigned int bgraTargetPaddingElements = (unsigned int)intOptions[2];

	const uint8_t* ySource = (const uint8_t*)sources[0];
	const uint8_t* vuSource = (const uint8_t*)sources[1];

	uint8_t* bgraTarget = (uint8_t*)targets[0];

	const unsigned int ySourceStrideElements = width + ySourcePaddingElements;
	const unsigned int vuSourceStrideElements = width + vuSourcePaddingElements; // 2x2 downsampling but 2 channels
	const unsigned int bgraTargetStrideElements = width * 4u + bgraTargetPaddingElements;

	ocean_assert(intOptions[3] >= 0 && intOptions[3] <= 255);
	const uint8_t alphaValue = uint8_t(intOptions[3]);

	Memory mirroredTargetMemory;

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	const uint8_t* yUpper = ySource + multipleRowIndex * 2u * ySourceStrideElements;
	const uint8_t* vu = vuSource + multipleRowIndex * vuSourceStrideElements;

	uint8_t* tUpperFinal = flipTarget ? (bgraTarget + (height - multipleRowIndex * 2u - 1u) * bgraTargetStrideElements) : bgraTarget + multipleRowIndex * 2u * bgraTargetStrideElements;
	uint8_t* tLowerFinal = flipTarget ? tUpperFinal - bgraTargetStrideElements : tUpperFinal + bgraTargetStrideElements;

	uint8_t* tUpper = nullptr;
	uint8_t* tLower = nullptr;

	if (mirrorTarget)
	{
		// we create two temporary rows for mirroring

		mirroredTargetMemory = Memory::create<uint8_t>(width * 4u * 2u);

		tUpper = mirroredTargetMemory.data<uint8_t>() + 0;
		tLower = mirroredTargetMemory.data<uint8_t>() + width * 4u;
	}
	else
	{
		tUpper = tUpperFinal;
		tLower = tLowerFinal;
	}

	const uint8_t* const yUpperEnd = yUpper + width;

	// the documention of this function designed for YVU24 to BGR24 conversion

	// precise color space conversion:
	// | B |   |  1    0.0         1.732446   -221.753088 |   | Y |
	// | G | = |  1   -0.698001   -0.3376335   132.561152 | * | U |
	// | R |   |  1    1.370705    0.0        -175.45024  |   | V |
	//                                                        | 1 |

	// approximation:
	// B = 64 * Y   +  0 * (V - 128)   + 111 * (U - 128)
	// G = 64 * Y   - 45 * (V - 128)   -  22 * (U - 128)
	// R = 64 * Y   + 88 * (V - 128)   +   0 * (U - 128)

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	const uint8x8_t constant_128_u_8x8 = vdup_n_u8(128);
	const int16x8_t constant_22_s_16x8 = vdupq_n_s16(-22);
	const int16x8_t constant_111_s_16x8 = vdupq_n_s16(111);
	const int16x8_t constant_88_s_16x8 = vdupq_n_s16(88);
	const int16x8_t constant_45_s_16x8 = vdupq_n_s16(-45);
	const uint8x16_t alphaValue_u_8x16 = vdupq_n_u8(alphaValue);

	constexpr size_t blockSize = 16;
	const size_t blocks = width / blockSize;

	for (unsigned int n = 0u; n < blocks; ++n)
	{
		const uint8x16_t ySourceUpper_u_8x16 = vld1q_u8(yUpper);
		const uint8x16_t ySourceLower_u_8x16 = vld1q_u8(yUpper + ySourceStrideElements);

		const int16x8_t ySourceUpperMultiplied_low_s_16x8 = vreinterpretq_s16_u16(vshll_n_u8(vget_low_u8(ySourceUpper_u_8x16), 6)); // Y * 64
		const int16x8_t ySourceUpperMultiplied_high_s_16x8 = vreinterpretq_s16_u16(vshll_n_u8(vget_high_u8(ySourceUpper_u_8x16), 6));

		const int16x8_t ySourceLowerMultiplied_low_s_16x8 = vreinterpretq_s16_u16(vshll_n_u8(vget_low_u8(ySourceLower_u_8x16), 6));
		const int16x8_t ySourceLowerMultiplied_high_s_16x8 = vreinterpretq_s16_u16(vshll_n_u8(vget_high_u8(ySourceLower_u_8x16), 6));

		const uint8x8x2_t vu_u_8x8x2 = vld2_u8(vu);

		const int16x8_t v_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vu_u_8x8x2.val[0], constant_128_u_8x8)); // V' = V - 128
		const int16x8_t u_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vu_u_8x8x2.val[1], constant_128_u_8x8)); // U' = U - 128


		const int16x8_t bIntermediateResults_s_16x8 = vmulq_s16(u_s_16x8, constant_111_s_16x8); // 111 * U'
		const int16x8_t gIntermediateResults_s_16x8 = vmlaq_s16(vmulq_s16(v_s_16x8, constant_45_s_16x8), u_s_16x8, constant_22_s_16x8); // -45 * V' - 22 * V'
		const int16x8_t rIntermediateResults_s_16x8 = vmulq_s16(v_s_16x8, constant_88_s_16x8); // 88 * V'


		// bI0 bI1 bI2 bI3 ... -> bI0 bI0 bI1 bI1 bI2 bI2 bI3 bI3 ...

		const int16x8x2_t bIntermediateResults_s_16x8x2 = vzipq_s16(bIntermediateResults_s_16x8, bIntermediateResults_s_16x8);
		const int16x8x2_t gIntermediateResults_s_16x8x2 = vzipq_s16(gIntermediateResults_s_16x8, gIntermediateResults_s_16x8);
		const int16x8x2_t rIntermediateResults_s_16x8x2 = vzipq_s16(rIntermediateResults_s_16x8, rIntermediateResults_s_16x8);

		// now combining Y and bI, gI, and rI

		const int16x8_t bResultUpper_low_s_16x8 = vaddq_s16(bIntermediateResults_s_16x8x2.val[0], ySourceUpperMultiplied_low_s_16x8);
		const int16x8_t bResultUpper_high_s_16x8 = vaddq_s16(bIntermediateResults_s_16x8x2.val[1], ySourceUpperMultiplied_high_s_16x8);

		const int16x8_t gResultUpper_low_s_16x8 = vaddq_s16(gIntermediateResults_s_16x8x2.val[0], ySourceUpperMultiplied_low_s_16x8);
		const int16x8_t gResultUpper_high_s_16x8 = vaddq_s16(gIntermediateResults_s_16x8x2.val[1], ySourceUpperMultiplied_high_s_16x8);

		const int16x8_t rResultUpper_low_s_16x8 = vaddq_s16(rIntermediateResults_s_16x8x2.val[0], ySourceUpperMultiplied_low_s_16x8);
		const int16x8_t rResultUpper_high_s_16x8 = vaddq_s16(rIntermediateResults_s_16x8x2.val[1], ySourceUpperMultiplied_high_s_16x8);


		uint8x16x4_t resultsUpper_u_8x16x4;
		resultsUpper_u_8x16x4.val[3] = alphaValue_u_8x16;

		// saturated narrow signed to unsigned
		resultsUpper_u_8x16x4.val[0] = vcombine_u8(vqrshrun_n_s16(bResultUpper_low_s_16x8, 6), vqrshrun_n_s16(bResultUpper_high_s_16x8, 6));
		resultsUpper_u_8x16x4.val[1] = vcombine_u8(vqrshrun_n_s16(gResultUpper_low_s_16x8, 6), vqrshrun_n_s16(gResultUpper_high_s_16x8, 6));
		resultsUpper_u_8x16x4.val[2] = vcombine_u8(vqrshrun_n_s16(rResultUpper_low_s_16x8, 6), vqrshrun_n_s16(rResultUpper_high_s_16x8, 6));

		// and we can store the result
		vst4q_u8(tUpper, resultsUpper_u_8x16x4);

		const int16x8_t bResultLower_low_s_16x8 = vaddq_s16(bIntermediateResults_s_16x8x2.val[0], ySourceLowerMultiplied_low_s_16x8);
		const int16x8_t bResultLower_high_s_16x8 = vaddq_s16(bIntermediateResults_s_16x8x2.val[1], ySourceLowerMultiplied_high_s_16x8);

		const int16x8_t gResultLower_low_s_16x8 = vaddq_s16(gIntermediateResults_s_16x8x2.val[0], ySourceLowerMultiplied_low_s_16x8);
		const int16x8_t gResultLower_high_s_16x8 = vaddq_s16(gIntermediateResults_s_16x8x2.val[1], ySourceLowerMultiplied_high_s_16x8);

		const int16x8_t rResultLower_low_s_16x8 = vaddq_s16(rIntermediateResults_s_16x8x2.val[0], ySourceLowerMultiplied_low_s_16x8);
		const int16x8_t rResultLower_high_s_16x8 = vaddq_s16(rIntermediateResults_s_16x8x2.val[1], ySourceLowerMultiplied_high_s_16x8);

		uint8x16x4_t resultsLower_u_8x16x4;
		resultsLower_u_8x16x4.val[3] = alphaValue_u_8x16;

		// saturated narrow signed to unsigned
		resultsLower_u_8x16x4.val[0] = vcombine_u8(vqrshrun_n_s16(bResultLower_low_s_16x8, 6), vqrshrun_n_s16(bResultLower_high_s_16x8, 6));
		resultsLower_u_8x16x4.val[1] = vcombine_u8(vqrshrun_n_s16(gResultLower_low_s_16x8, 6), vqrshrun_n_s16(gResultLower_high_s_16x8, 6));
		resultsLower_u_8x16x4.val[2] = vcombine_u8(vqrshrun_n_s16(rResultLower_low_s_16x8, 6), vqrshrun_n_s16(rResultLower_high_s_16x8, 6));

		// and we can store the result
		vst4q_u8(tLower, resultsLower_u_8x16x4);

		yUpper += blockSize;
		vu += blockSize;

		tUpper += blockSize * 4u;
		tLower += blockSize * 4u;
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (yUpper != yUpperEnd)
	{
		ocean_assert(yUpper < yUpperEnd);

		const int16_t v_128 = vu[0] - 128;
		const int16_t u_128 = vu[1] - 128;

		const int16_t vu0 = 111 * u_128;
		const int16_t vu1 = -45 * v_128 - 22 * u_128;
		const int16_t vu2 = 88 * v_128;

		// first and second upper pixel

		const int16_t yUpperLeft64 = yUpper[0] * 64;
		const int16_t yUpperRight64 = yUpper[1] * 64;

		tUpper[0] = uint8_t(uint16_t(minmax<int16_t>(0, yUpperLeft64 + vu0, 16320)) / 64u);
		tUpper[1] = uint8_t(uint16_t(minmax<int16_t>(0, yUpperLeft64 + vu1, 16320)) / 64u);
		tUpper[2] = uint8_t(uint16_t(minmax<int16_t>(0, yUpperLeft64 + vu2, 16320)) / 64u);
		tUpper[3] = alphaValue;

		tUpper[4] = uint8_t(uint16_t(minmax<int16_t>(0, yUpperRight64 + vu0, 16320)) / 64u);
		tUpper[5] = uint8_t(uint16_t(minmax<int16_t>(0, yUpperRight64 + vu1, 16320)) / 64u);
		tUpper[6] = uint8_t(uint16_t(minmax<int16_t>(0, yUpperRight64 + vu2, 16320)) / 64u);
		tUpper[7] = alphaValue;


		// first and second lower pixel

		const int16_t yLowerLeft64 = yUpper[ySourceStrideElements + 0u] * 64;
		const int16_t yLowerRight64 = yUpper[ySourceStrideElements + 1u] * 64;

		tLower[0] = uint8_t(uint16_t(minmax<int16_t>(0, yLowerLeft64 + vu0, 16320)) / 64u);
		tLower[1] = uint8_t(uint16_t(minmax<int16_t>(0, yLowerLeft64 + vu1, 16320)) / 64u);
		tLower[2] = uint8_t(uint16_t(minmax<int16_t>(0, yLowerLeft64 + vu2, 16320)) / 64u);
		tLower[3] = alphaValue;

		tLower[4] = uint8_t(uint16_t(minmax<int16_t>(0, yLowerRight64 + vu0, 16320)) / 64u);
		tLower[5] = uint8_t(uint16_t(minmax<int16_t>(0, yLowerRight64 + vu1, 16320)) / 64u);
		tLower[6] = uint8_t(uint16_t(minmax<int16_t>(0, yLowerRight64 + vu2, 16320)) / 64u);
		tLower[7] = alphaValue;

		yUpper += 2;
		vu += 2; // 2x2 downsampled, but two channels

		tUpper += 2u * 4u;
		tLower += 2u * 4u;
	}

	if (mirrorTarget)
	{
		// we mirror the upper and lower rows from the temporary buffer to the actual buffer

		CV::FrameChannels::reverseRowPixelOrder<uint8_t, 4u>(tUpper - width * 4u, tUpperFinal, width);
		CV::FrameChannels::reverseRowPixelOrder<uint8_t, 4u>(tLower - width * 4u, tLowerFinal, width);
	}
}

}

}
