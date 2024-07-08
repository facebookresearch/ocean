/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameFilterLaplace.h"
#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameNormalizer.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace CV
{

void FrameFilterLaplace::filterMagnitude1Channel8Bit(const uint8_t* source, uint16_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 3u && height >= 3u);

	constexpr unsigned int tSourceChannels = 1u;
	constexpr unsigned int tTargetChannels = 1u;

	FrameChannels::applyRowOperator<uint8_t, uint16_t, tSourceChannels, tTargetChannels>(source, target, width, height, sourcePaddingElements, targetPaddingElements, &filterAbsoluteRow, worker);
}

void FrameFilterLaplace::filterNormalizedMagnitude1Channel8Bit(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	Frame responseFrame(FrameType(width, height, FrameType::genericPixelFormat<uint16_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));

	filterMagnitude1Channel8Bit(source, responseFrame.data<uint16_t>(), width, height, sourcePaddingElements, responseFrame.paddingElements(), worker);

	FrameNormalizer::normalize1ChannelToUint8<uint16_t>(responseFrame.constdata<uint16_t>(), target, width, height, responseFrame.paddingElements(), targetPaddingElements, worker);
}

void FrameFilterLaplace::filterAbsoluteRow(const uint8_t* sourceRow, uint16_t* targetRow, const unsigned int width, const unsigned int height, unsigned int rowIndex, const unsigned int sourceStrideElements, const unsigned int /*targetStrideElements*/)
{
	ocean_assert(sourceRow != nullptr && targetRow != nullptr);
	ocean_assert(width >= 3u && height >= 3u);

	/**
	 *  0 -1  0
	 * -1  4 -1
	 *  0 -1  0
	 */

	const uint8_t* sourceRowUpper = rowIndex == 0u ? sourceRow : sourceRow - sourceStrideElements;
	const uint8_t* sourceRowLower = rowIndex == height - 1u ? sourceRow : sourceRow + sourceStrideElements;

	/**
	 * left pixel (works also in the first and last row)
	 * -1  0
	 *  3 -1
	 * -1  0
	 */

	*targetRow++ = uint16_t(std::abs(int16_t(*sourceRow) * int16_t(3) - int16_t(*(sourceRow + 1)) - int16_t(*sourceRowUpper) - int16_t(*sourceRowLower)));

	++sourceRowUpper;
	++sourceRow;
	++sourceRowLower;

	for (unsigned int x = 1u; x < width - 1u; ++x)
	{
		*targetRow++ = uint16_t(std::abs(int16_t(*sourceRow) * int16_t(4) - int16_t(*(sourceRow - 1)) - int16_t(*(sourceRow + 1)) - int16_t(*sourceRowUpper) - int16_t(*sourceRowLower)));

		++sourceRowUpper;
		++sourceRow;
		++sourceRowLower;
	}

	/**
	 * right pixel (works also in the first and last row)
	 *  0 -1
	 * -1  3
	 *  0 -1
	 */

	*targetRow = uint16_t(std::abs(int16_t(*sourceRow) * int16_t(3) - int16_t(*(sourceRow - 1)) - int16_t(*sourceRowUpper) - int16_t(*sourceRowLower)));
}

double FrameFilterLaplace::variance1Channel8Bit(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements)
{
	ocean_assert(frame != nullptr);
	ocean_assert(width >= 3u && height >= 3u);

	const unsigned int frameStrideElements = width + framePaddingElements;

	int64_t sum = 0ll;
	uint64_t squaredSum = 0ull;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr unsigned int maximalFilterResponse = 255u * 4u;
	constexpr unsigned int maximalSqrResponse = maximalFilterResponse * maximalFilterResponse;
	constexpr unsigned int maximalSqrResponsesPerSum = (unsigned int)(-1) / maximalSqrResponse;
	constexpr unsigned int maximalSqrResponsesPerSums = maximalSqrResponsesPerSum * 8u; // we use 'uint32x4x2_t' to accumulate the square responses per row

	constexpr unsigned int maximalWidthNEON = maximalSqrResponsesPerSums + 2u; // we skip two pixels = 33026

	ocean_assert_and_suppress_unused(width <= maximalWidthNEON, maximalWidthNEON);

	int64x2x2_t sumResponse_s_64x2x2 = {vdupq_n_s64(0ull), vdupq_n_s64(0ull)};
	uint64x2x2_t sumSqrResponse_u_64x2x2 = {vdupq_n_u64(0ull), vdupq_n_u64(0ull)};

	int64_t sumResponse_s_64x1 = 0ull;
	uint64_t sumSqrResponse_u_64x1 = 0ull;

	for (unsigned int y = 1u; y < height - 1u; ++y) // we skip the first and last row
	{
		frame += frameStrideElements;

		int32x4x2_t sumResponse_s_32x4x2 = {vdupq_n_s32(0), vdupq_n_s32(0)};
		uint32x4x2_t sumSqrResponse_u_32x4x2 = {vdupq_n_u32(0u), vdupq_n_u32(0)};

		int32_t sumResponse_s_32x1 = 0ull;
		uint32_t sumSqrResponse_u_32x1 = 0ull;

		varianceRow1Channel8BitNEON(frame, width, frameStrideElements, sumResponse_s_32x4x2, sumSqrResponse_u_32x4x2, sumResponse_s_32x1, sumSqrResponse_u_32x1);

		const int32x4_t sumResponse_s_32x4 = vaddq_s32(sumResponse_s_32x4x2.val[0], sumResponse_s_32x4x2.val[1]);

		sumResponse_s_64x2x2.val[0] = vaddw_s32(sumResponse_s_64x2x2.val[0], vget_low_s32(sumResponse_s_32x4));
		sumResponse_s_64x2x2.val[1] = vaddw_s32(sumResponse_s_64x2x2.val[1], vget_high_s32(sumResponse_s_32x4));

		const uint64x2_t sumSqrResponse0_u_64x2 = vaddl_u32(vget_low_u32(sumSqrResponse_u_32x4x2.val[0]), vget_high_u32(sumSqrResponse_u_32x4x2.val[0]));
		const uint64x2_t sumSqrResponse1_u_64x2 = vaddl_u32(vget_low_u32(sumSqrResponse_u_32x4x2.val[1]), vget_high_u32(sumSqrResponse_u_32x4x2.val[1]));

		sumSqrResponse_u_64x2x2.val[0] = vaddq_u64(sumSqrResponse_u_64x2x2.val[0], sumSqrResponse0_u_64x2);
		sumSqrResponse_u_64x2x2.val[1] = vaddq_u64(sumSqrResponse_u_64x2x2.val[1], sumSqrResponse1_u_64x2);

		sumResponse_s_64x1 += sumResponse_s_32x1;
		sumSqrResponse_u_64x1 += sumSqrResponse_u_32x1;
	}

	const int64x2_t sumResponse_s_64x2 = vaddq_s64(sumResponse_s_64x2x2.val[0], sumResponse_s_64x2x2.val[1]);
	const uint64x2_t sumResponse_u_64x2 = vaddq_u64(sumSqrResponse_u_64x2x2.val[0], sumSqrResponse_u_64x2x2.val[1]);

	sum = vgetq_lane_s64(sumResponse_s_64x2, 0) + vgetq_lane_s64(sumResponse_s_64x2, 1) + sumResponse_s_64x1;
	squaredSum = vgetq_lane_u64(sumResponse_u_64x2, 0) + vgetq_lane_u64(sumResponse_u_64x2, 1) + sumSqrResponse_u_64x1;

#else

	const uint8_t* top = frame + 1; // first row, second pixel
	const uint8_t* center = frame + frameStrideElements; // second row, first pixel
	const uint8_t* bottom = frame + frameStrideElements * 2u + 1u; // third row, second pixel

	for (unsigned int y = 1u; y < height - 1u; ++y) // we skip the first and last row
	{
		for (unsigned int x = 1u; x < width - 1u; ++x) // we skip the first and last pixel
		{
			const int16_t response = int16_t(center[1]) * int16_t(4) - int16_t(top[0]) - int16_t(center[0]) - int16_t(center[2]) - int16_t(bottom[0]);

			sum += response;
			squaredSum += int32_t(response) * int32_t(response);

			++top;
			++center;
			++bottom;
		}

		top += 2u + framePaddingElements;
		center += 2u + framePaddingElements;
		bottom += 2u + framePaddingElements;
	}

#endif // OCEAN_HARDWARE_NEON_VERSION

	const unsigned int pixels = (width - 2u) * (height - 2u);

	const double variance = double(squaredSum) / double(pixels) - double(sum) * double(sum) / (double(pixels) * double(pixels));
	ocean_assert(variance >= 0.0);

	return variance;
}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

void FrameFilterLaplace::varianceRow1Channel8BitNEON(const uint8_t* row, const unsigned int width, const unsigned int rowStrideElements, int32x4x2_t& sumResponse_s_32x4x2, uint32x4x2_t& sumSqrResponse_u_32x4x2, int32_t& sumResponse_s_32x1, uint32_t& sumSqrResponse_u_32x1)
{
	ocean_assert(row != nullptr);

	ocean_assert(width >= 3u && width <= 33026u);

	const uint8_t* top = row - rowStrideElements + 1u; // top row, second pixel
	const uint8_t* bottom = row + rowStrideElements + 1u; // bottom row, second pixel

	const unsigned int width2 = width - 2u; // we skip the first and last pixel

	const unsigned int blocks16 = width2 / 16u;
	const unsigned int remainingBlock1 = width2 % 16u;

	for (unsigned int n = 0u; n < blocks16; ++n)
	{
		const uint8x16_t top_u_8x16 = vld1q_u8(top);
		const uint8x16_t left_u_8x16 = vld1q_u8(row);
		const uint8x16_t center_u_8x16 = vld1q_u8(row + 1);
		const uint8x16_t right_u_8x16 = vld1q_u8(row + 2);
		const uint8x16_t bottom_u_8x16 = vld1q_u8(bottom);

		const uint16x8_t sumTopBottomA_u_16x8 = vaddl_u8(vget_low_u8(top_u_8x16), vget_low_u8(bottom_u_8x16)); // top + bottom
		const uint16x8_t sumTopBottomB_u_16x8 = vaddl_u8(vget_high_u8(top_u_8x16), vget_high_u8(bottom_u_8x16));

		const uint16x8_t sumLeftRightA_u_16x8 = vaddl_u8(vget_low_u8(left_u_8x16), vget_low_u8(right_u_8x16)); // left + right
		const uint16x8_t sumLeftRightB_u_16x8 = vaddl_u8(vget_high_u8(left_u_8x16), vget_high_u8(right_u_8x16));

		const uint16x8_t sumTopBottomLeftRightA_u_16x8 = vaddq_u16(sumTopBottomA_u_16x8, sumLeftRightA_u_16x8);
		const uint16x8_t sumTopBottomLeftRightB_u_16x8 = vaddq_u16(sumTopBottomB_u_16x8, sumLeftRightB_u_16x8);

		const uint16x8_t centerA_u_16x8 = vshll_n_u8(vget_low_u8(center_u_8x16), 2); // center * 4
		const uint16x8_t centerB_u_16x8 = vshll_n_u8(vget_high_u8(center_u_8x16), 2);

		const int16x8_t responseA_s_16x8 = vreinterpretq_s16_u16(vsubq_u16(centerA_u_16x8, sumTopBottomLeftRightA_u_16x8)); // center * 4 - (top + bottom + left + right)
		const int16x8_t responseB_s_16x8 = vreinterpretq_s16_u16(vsubq_u16(centerB_u_16x8, sumTopBottomLeftRightB_u_16x8));

		const int16x8_t sumResponse_s_16x8 = vaddq_s16(responseA_s_16x8, responseB_s_16x8); // sum up all responses

		sumResponse_s_32x4x2.val[0] = vaddw_s16(sumResponse_s_32x4x2.val[0], vget_low_s16(sumResponse_s_16x8));
		sumResponse_s_32x4x2.val[1] = vaddw_s16(sumResponse_s_32x4x2.val[1], vget_high_s16(sumResponse_s_16x8));

		const uint32x4_t sqrResponse0_u_32x4 = vreinterpretq_u32_s32(vmull_s16(vget_low_s16(responseA_s_16x8), vget_low_s16(responseA_s_16x8))); // sum up the squares of all responses
		const uint32x4_t sqrResponse1_u_32x4 = vreinterpretq_u32_s32(vmull_s16(vget_high_s16(responseA_s_16x8), vget_high_s16(responseA_s_16x8)));
		const uint32x4_t sqrResponse2_u_32x4 = vreinterpretq_u32_s32(vmull_s16(vget_low_s16(responseB_s_16x8), vget_low_s16(responseB_s_16x8)));
		const uint32x4_t sqrResponse3_u_32x4 = vreinterpretq_u32_s32(vmull_s16(vget_high_s16(responseB_s_16x8), vget_high_s16(responseB_s_16x8)));

		sumSqrResponse_u_32x4x2.val[0] = vaddq_u32(sumSqrResponse_u_32x4x2.val[0], vaddq_u32(sqrResponse0_u_32x4, sqrResponse1_u_32x4));
		sumSqrResponse_u_32x4x2.val[1] = vaddq_u32(sumSqrResponse_u_32x4x2.val[1], vaddq_u32(sqrResponse2_u_32x4, sqrResponse3_u_32x4));

		top += 16;
		row += 16;
		bottom += 16;
	}

	for (unsigned int n = 0u; n < remainingBlock1; ++n)
	{
		const int16_t response = int16_t(row[1]) * int16_t(4) - int16_t(top[0]) - int16_t(row[0]) - int16_t(row[2]) - int16_t(bottom[0]);

		sumResponse_s_32x1 += response;
		sumSqrResponse_u_32x1 += int32_t(response) * int32_t(response);

		++top;
		++row;
		++bottom;
	}
}

#if 0 // slower implementation for reference
void FrameFilterLaplace::varianceRow1Channel8BitNEON(const uint8_t* row, const unsigned int width, const unsigned int rowStrideElements, int64x2x2_t& sumResponse_s_64x2x2, uint64x2x2_t& sumSqrResponse_u_64x2x2, int64_t& sumResponse_s_64x1, uint64_t& sumSqrResponse_u_64x1)
{
	const uint8_t* top = row - rowStrideElements + 1u; // top row, second pixel
	const uint8_t* bottom = row + rowStrideElements + 1u; // bottom row, second pixel

	const unsigned int width2 = width - 2u; // we skip the first and last pixel

	const unsigned int blocks16 = width2 / 16u;
	const unsigned int remainingBlock1 = width2 % 16u;

	for (unsigned int n = 0u; n < blocks16; ++n)
	{
		const uint8x16_t top_u_8x16 = vld1q_u8(top);
		const uint8x16_t left_u_8x16 = vld1q_u8(row);
		const uint8x16_t center_u_8x16 = vld1q_u8(row + 1);
		const uint8x16_t right_u_8x16 = vld1q_u8(row + 2);
		const uint8x16_t bottom_u_8x16 = vld1q_u8(bottom);

		const uint16x8_t sumTopBottomA_u_16x8 = vaddl_u8(vget_low_u8(top_u_8x16), vget_low_u8(bottom_u_8x16)); // top + bottom
		const uint16x8_t sumTopBottomB_u_16x8 = vaddl_u8(vget_high_u8(top_u_8x16), vget_high_u8(bottom_u_8x16));

		const uint16x8_t sumLeftRightA_u_16x8 = vaddl_u8(vget_low_u8(left_u_8x16), vget_low_u8(right_u_8x16)); // left + right
		const uint16x8_t sumLeftRightB_u_16x8 = vaddl_u8(vget_high_u8(left_u_8x16), vget_high_u8(right_u_8x16));

		const uint16x8_t sumTopBottomLeftRightA_u_16x8 = vaddq_u16(sumTopBottomA_u_16x8, sumLeftRightA_u_16x8);
		const uint16x8_t sumTopBottomLeftRightB_u_16x8 = vaddq_u16(sumTopBottomB_u_16x8, sumLeftRightB_u_16x8);

		const uint16x8_t centerA_u_16x8 = vshll_n_u8(vget_low_u8(center_u_8x16), 2); // center * 4
		const uint16x8_t centerB_u_16x8 = vshll_n_u8(vget_high_u8(center_u_8x16), 2);

		const int16x8_t responseA_s_16x8 = vreinterpretq_s16_u16(vsubq_u16(centerA_u_16x8, sumTopBottomLeftRightA_u_16x8)); // center * 4 - (top + bottom + left + right)
		const int16x8_t responseB_s_16x8 = vreinterpretq_s16_u16(vsubq_u16(centerB_u_16x8, sumTopBottomLeftRightB_u_16x8));

		const int16x8_t sumResponse_s_16x8 = vaddq_s16(responseA_s_16x8, responseB_s_16x8); // sum up all responses
		const int32x4_t sumResponse_s_32x4 = vaddl_s16(vget_low_s16(sumResponse_s_16x8), vget_high_s16(sumResponse_s_16x8));

		sumResponse_s_64x2x2.val[0] = vaddw_s32(sumResponse_s_64x2x2.val[0], vget_low_s32(sumResponse_s_32x4));
		sumResponse_s_64x2x2.val[1] = vaddw_s32(sumResponse_s_64x2x2.val[1], vget_high_s32(sumResponse_s_32x4));

		const uint32x4_t sqrResponse0_u_32x4 = vreinterpretq_u32_s32(vmull_s16(vget_low_s16(responseA_s_16x8), vget_low_s16(responseA_s_16x8))); // sum up the squares of all responses
		const uint32x4_t sqrResponse1_u_32x4 = vreinterpretq_u32_s32(vmull_s16(vget_high_s16(responseA_s_16x8), vget_high_s16(responseA_s_16x8)));
		const uint32x4_t sqrResponse2_u_32x4 = vreinterpretq_u32_s32(vmull_s16(vget_low_s16(responseB_s_16x8), vget_low_s16(responseB_s_16x8)));
		const uint32x4_t sqrResponse3_u_32x4 = vreinterpretq_u32_s32(vmull_s16(vget_high_s16(responseB_s_16x8), vget_high_s16(responseB_s_16x8)));

		const uint64x2_t sqrResponse0_u_64x2 = vaddl_u32(vget_low_u32(sqrResponse0_u_32x4), vget_high_u32(sqrResponse0_u_32x4));
		const uint64x2_t sqrResponse1_u_64x2 = vaddl_u32(vget_low_u32(sqrResponse1_u_32x4), vget_high_u32(sqrResponse1_u_32x4));
		const uint64x2_t sqrResponse2_u_64x2 = vaddl_u32(vget_low_u32(sqrResponse2_u_32x4), vget_high_u32(sqrResponse2_u_32x4));
		const uint64x2_t sqrResponse3_u_64x2 = vaddl_u32(vget_low_u32(sqrResponse3_u_32x4), vget_high_u32(sqrResponse3_u_32x4));

		sumSqrResponse_u_64x2x2.val[0] = vaddq_u64(sumSqrResponse_u_64x2x2.val[0], vaddq_u64(sqrResponse0_u_64x2, sqrResponse1_u_64x2));
		sumSqrResponse_u_64x2x2.val[1] = vaddq_u64(sumSqrResponse_u_64x2x2.val[1], vaddq_u64(sqrResponse2_u_64x2, sqrResponse3_u_64x2));

		top += 16;
		row += 16;
		bottom += 16;
	}

	for (unsigned int n = 0u; n < remainingBlock1; ++n)
	{
		const int16_t response = int16_t(row[1]) * int16_t(4) - int16_t(top[0]) - int16_t(row[0]) - int16_t(row[2]) - int16_t(bottom[0]);

		sumResponse_s_64x1 += response;
		sumSqrResponse_u_64x1 += int32_t(response) * int32_t(response);

		++top;
		++row;
		++bottom;
	}
}

#endif

#endif // OCEAN_HARDWARE_NEON_VERSION

}

}
