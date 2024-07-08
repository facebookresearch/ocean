/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/LineDetectorULF.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/PixelPosition.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

bool LineDetectorULF::EdgeDetector::invokeHorizontal(const uint8_t* /*frame*/, const unsigned int /*width*/, const unsigned int /*height*/, int16_t* /*responses*/, const unsigned int /*paddingElements*/) const
{
	// can be overwritten in derived classes
	return false;
}

bool LineDetectorULF::EdgeDetector::hasInvokeHorizontal(const unsigned int /*width*/, const unsigned int /*height*/) const
{
	// can be overwritten in derived classes
	return false;
}

unsigned int LineDetectorULF::EdgeDetector::adjustThreshold(const unsigned int threshold) const
{
	// by default the identity
	return threshold;
}

void LineDetectorULF::EdgeDetector::determineRowSums(const uint8_t* row, const unsigned int width, const unsigned int window, uint32_t* windowSums)
{
	ocean_assert(row != nullptr);
	ocean_assert(width >= 1u && window <= width);
	ocean_assert(windowSums != nullptr);

	const unsigned int sumElements = width - window + 1u;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	ocean_assert(sumElements >= 8u);

	for (unsigned int x = 0u; x < sumElements; x += 8u)
	{
		if (x + 8u > sumElements)
		{
			// the last iteration will not fit into the data,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(x >= 8u && sumElements > 8u);
			const unsigned int newX = sumElements - 8u;

			ocean_assert(x > newX);
			const unsigned int offset = x - newX;

			row -= offset;
			windowSums -= offset;

			// the for loop will stop after this iteration
			ocean_assert(!(x + 8u < sumElements));
		}

		uint16x8_t sums_u_16x8 = vmovl_u8(vld1_u8(row + 0));

		for (unsigned int n = 1u; n < window; ++n)
		{
			sums_u_16x8 = vaddw_u8(sums_u_16x8, vld1_u8(row + n));
		}

		vst1q_u32(windowSums, vmovl_u16(vget_low_u16(sums_u_16x8)));
		vst1q_u32(windowSums + 4, vmovl_u16(vget_high_u16(sums_u_16x8)));

		row += 8;
		windowSums += 8;
	}

#else

	uint32_t sum = 0u;

	// sum up (window - 1) values for the sliding window approach
	for (uint32_t i = 0u; i < window - 1u; i++)
	{
		sum += row[i];
	}

	const uint8_t* addValue = row + window - 1u;
	const uint8_t* removeValue = row;

	for (unsigned int i = 0u; i < sumElements; i++)
	{
		// add new values to sliding window
		sum += *addValue;

		*windowSums++ = sum;

		// remove old value from the sliding window
		sum -= *removeValue;

		++addValue;
		++removeValue;
	}

#endif
}

void LineDetectorULF::EdgeDetector::determineRowSums(const uint8_t* row, const unsigned int width, const unsigned int window, uint16_t* windowSums)
{
	ocean_assert(row != nullptr);
	ocean_assert(width >= 1u && window <= width);
	ocean_assert(windowSums != nullptr);

	const unsigned int sumElements = width - window + 1u;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	ocean_assert(sumElements >= 8u);

	for (unsigned int x = 0u; x < sumElements; x += 8u)
	{
		if (x + 8u > sumElements)
		{
			// the last iteration will not fit into the data,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(x >= 8u && sumElements > 8u);
			const unsigned int newX = sumElements - 8u;

			ocean_assert(x > newX);
			const unsigned int offset = x - newX;

			row -= offset;
			windowSums -= offset;

			// the for loop will stop after this iteration
			ocean_assert(!(x + 8u < sumElements));
		}

		uint16x8_t sums_u_16x8 = vmovl_u8(vld1_u8(row + 0));

		for (unsigned int n = 1u; n < window; ++n)
		{
			const uint16x8_t values_u_16x8 = vmovl_u8(vld1_u8(row + n));

			sums_u_16x8 = vaddq_u16(sums_u_16x8, values_u_16x8);
		}

		vst1q_u16(windowSums, sums_u_16x8);

		row += 8;
		windowSums += 8;
	}

#else

	uint16_t sum = 0u;

	// sum up (window - 1) values for the sliding window approach
	for (uint16_t i = 0u; i < window - 1u; i++)
	{
		sum += row[i];
	}

	const uint8_t* addValue = row + window - 1u;
	const uint8_t* removeValue = row;

	for (unsigned int i = 0u; i < sumElements; i++)
	{
		// add new values to sliding window
		sum += *addValue;

		*windowSums++ = sum;

		// remove old value from the sliding window
		sum -= *removeValue;

		++addValue;
		++removeValue;
	}

#endif
}

void LineDetectorULF::EdgeDetector::determineRowSums(const uint8_t* row, const unsigned int width, const unsigned int window, uint32_t* windowSums, uint32_t* windowSqrSums)
{
	ocean_assert(row != nullptr);
	ocean_assert(width >= 1u && window <= width);
	ocean_assert(windowSums != nullptr && windowSqrSums != nullptr);

	const unsigned int sumElements = width - window + 1u;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	ocean_assert(sumElements >= 8u);

	for (unsigned int x = 0u; x < sumElements; x += 8u)
	{
		if (x + 8u > sumElements)
		{
			// the last iteration will not fit into the data,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(x >= 8u && sumElements > 8u);
			const unsigned int newX = sumElements - 8u;

			ocean_assert(x > newX);
			const unsigned int offset = x - newX;

			row -= offset;
			windowSums -= offset;
			windowSqrSums -= offset;

			// the for loop will stop after this iteration
			ocean_assert(!(x + 8u < sumElements));
		}

		uint16x8_t sums_u_16x8 = vmovl_u8(vld1_u8(row + 0));
		uint16x8_t sqrSums_u_16x8 = vmulq_u16(sums_u_16x8, sums_u_16x8);

		uint32x4_t sqrSums0_u_32x4 = vmovl_u16(vget_low_u16(sqrSums_u_16x8));
		uint32x4_t sqrSums1_u_32x4 = vmovl_u16(vget_high_u16(sqrSums_u_16x8));

		for (unsigned int n = 1u; n < window; ++n)
		{
			const uint16x8_t values_u_16x8 = vmovl_u8(vld1_u8(row + n));

			const uint16x4_t values0_u_16x4 = vget_low_u16(values_u_16x8);
			const uint16x4_t values1_u_16x4 = vget_high_u16(values_u_16x8);

			sums_u_16x8 = vaddq_u16(sums_u_16x8, values_u_16x8);

			sqrSums0_u_32x4 = vmlal_u16(sqrSums0_u_32x4, values0_u_16x4, values0_u_16x4);
			sqrSums1_u_32x4 = vmlal_u16(sqrSums1_u_32x4, values1_u_16x4, values1_u_16x4);
		}

		vst1q_u32(windowSums, vmovl_u16(vget_low_u16(sums_u_16x8)));
		vst1q_u32(windowSums + 4, vmovl_u16(vget_high_u16(sums_u_16x8)));

		vst1q_u32(windowSqrSums, sqrSums0_u_32x4);
		vst1q_u32(windowSqrSums + 4, sqrSums1_u_32x4);

		row += 8;
		windowSums += 8;
		windowSqrSums += 8;
	}

#else

	uint32_t sum = 0u;
	uint32_t sqrSum = 0u;

	// sum up (window - 1) values for the sliding window approach
	for (unsigned int i = 0u; i < window - 1u; i++)
	{
		sum += row[i];
		sqrSum += row[i] * row[i];
	}

	const uint8_t* addValue = row + window - 1u;
	const uint8_t* removeValue = row;

	for (unsigned int i = 0u; i < sumElements; i++)
	{
		// add new values to sliding window
		sum += *addValue;
		sqrSum += *addValue * *addValue;

		*windowSums++ = sum;
		*windowSqrSums++ = sqrSum;

		// remove old value from the sliding window
		sum -= *removeValue;
		sqrSum -= *removeValue * *removeValue;

		++addValue;
		++removeValue;
	}

#endif
}

void LineDetectorULF::EdgeDetector::determineRowSums(const uint8_t* row, const unsigned int width, const unsigned int window, uint16_t* windowSums, uint32_t* windowSqrSums)
{
	ocean_assert(row != nullptr);
	ocean_assert(width >= 1u && window <= width && window <= 255u);
	ocean_assert(windowSums != nullptr && windowSqrSums != nullptr);

	const unsigned int sumElements = width - window + 1u;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	ocean_assert(sumElements >= 8u);

	for (unsigned int x = 0u; x < sumElements; x += 8u)
	{
		if (x + 8u > sumElements)
		{
			// the last iteration will not fit into the data,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(x >= 8u && sumElements > 8u);
			const unsigned int newX = sumElements - 8u;

			ocean_assert(x > newX);
			const unsigned int offset = x - newX;

			row -= offset;
			windowSums -= offset;
			windowSqrSums -= offset;

			// the for loop will stop after this iteration
			ocean_assert(!(x + 8u < sumElements));
		}

		uint16x8_t sums_u_16x8 = vmovl_u8(vld1_u8(row + 0));
		uint16x8_t sqrSums_u_16x8 = vmulq_u16(sums_u_16x8, sums_u_16x8);

		uint32x4_t sqrSums0_u_32x4 = vmovl_u16(vget_low_u16(sqrSums_u_16x8));
		uint32x4_t sqrSums1_u_32x4 = vmovl_u16(vget_high_u16(sqrSums_u_16x8));

		for (unsigned int n = 1u; n < window; ++n)
		{
			const uint16x8_t values_u_16x8 = vmovl_u8(vld1_u8(row + n));

			const uint16x4_t values0_u_16x4 = vget_low_u16(values_u_16x8);
			const uint16x4_t values1_u_16x4 = vget_high_u16(values_u_16x8);

			sums_u_16x8 = vaddq_u16(sums_u_16x8, values_u_16x8);

			sqrSums0_u_32x4 = vmlal_u16(sqrSums0_u_32x4, values0_u_16x4, values0_u_16x4);
			sqrSums1_u_32x4 = vmlal_u16(sqrSums1_u_32x4, values1_u_16x4, values1_u_16x4);
		}

		vst1q_u16(windowSums, sums_u_16x8);

		vst1q_u32(windowSqrSums, sqrSums0_u_32x4);
		vst1q_u32(windowSqrSums + 4, sqrSums1_u_32x4);

		row += 8;
		windowSums += 8;
		windowSqrSums += 8;
	}

#else

	uint16_t sum = 0u;
	uint32_t sqrSum = 0u;

	// sum up (window - 1) values for the sliding window approach
	for (unsigned int i = 0u; i < window - 1u; i++)
	{
		sum += row[i];
		sqrSum += row[i] * row[i];
	}

	const uint8_t* addValue = row + window - 1u;
	const uint8_t* removeValue = row;

	for (unsigned int i = 0u; i < sumElements; i++)
	{
		// add new values to sliding window
		sum += *addValue;
		sqrSum += *addValue * *addValue;

		*windowSums++ = sum;
		*windowSqrSums++ = sqrSum;

		// remove old value from the sliding window
		sum -= *removeValue;
		sqrSum -= *removeValue * *removeValue;

		++addValue;
		++removeValue;
	}

#endif
}

template <bool tAdd>
void LineDetectorULF::EdgeDetector::applyRowSum(const uint8_t* row, const unsigned int width, uint16_t* sum)
{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	const unsigned int remaining16 = width % 16u;

	if (remaining16 < 8u)
	{
		for (unsigned int x = 0u; x < width; x += 16u)
		{
			if (x + 16u > width)
			{
				// the last remaining block must be done without NEON

				while (x < width)
				{
					if constexpr (tAdd)
					{
						sum[x] += row[x];
					}
					else
					{
						ocean_assert(sum[x] >= row[x]);
						sum[x] -= row[x];
					}

					++x;
				}

				break;
			}

			const uint8x16_t value_u_8x16 = vld1q_u8(row + x);

			const uint8x8_t valueA_u_8x8 = vget_low_u8(value_u_8x16);
			const uint8x8_t valueB_u_8x8 = vget_high_u8(value_u_8x16);

			if constexpr (tAdd)
			{
				vst1q_u16(sum + x + 0u, vaddw_u8(vld1q_u16(sum + x + 0u), valueA_u_8x8)); // sum = sum + value
				vst1q_u16(sum + x + 8u, vaddw_u8(vld1q_u16(sum + x + 8u), valueB_u_8x8));
			}
			else
			{
				vst1q_u16(sum + x + 0u, vsubw_u8(vld1q_u16(sum + x + 0u), valueA_u_8x8)); // sum = sum - value
				vst1q_u16(sum + x + 8u, vsubw_u8(vld1q_u16(sum + x + 8u), valueB_u_8x8));
			}
		}
	}
	else
	{
		for (unsigned int x = 0u; x < width; x += 8u)
		{
			if (x + 8u > width)
			{
				// the last remaining block must be done without NEON

				while (x < width)
				{
					if constexpr (tAdd)
					{
						sum[x] += row[x];
					}
					else
					{
						ocean_assert(sum[x] >= row[x]);
						sum[x] -= row[x];
					}

					++x;
				}

				break;
			}

			const uint8x8_t value_u_8x8 = vld1_u8(row + x);

			if constexpr (tAdd)
			{
				vst1q_u16(sum + x + 0u, vaddw_u8(vld1q_u16(sum + x + 0u), value_u_8x8)); // sum = sum + value
			}
			else
			{
				vst1q_u16(sum + x + 0u, vsubw_u8(vld1q_u16(sum + x + 0u), value_u_8x8)); // sum = sum - value
			}
		}
	}

#else

	OCEAN_WARNING_MISSING_IMPLEMENTATION;
	OCEAN_SUPPRESS_UNUSED_WARNING(row);
	OCEAN_SUPPRESS_UNUSED_WARNING(width);
	OCEAN_SUPPRESS_UNUSED_WARNING(sum);

#endif // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
}

template <bool tAdd>
void LineDetectorULF::EdgeDetector::applyRowSum(const uint8_t* row, const unsigned int width, uint16_t* sum, uint32_t* sqrSum)
{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	const unsigned int remaining16 = width % 16u;

	if (remaining16 < 8u)
	{
		for (unsigned int x = 0u; x < width; x += 16u)
		{
			if (x + 16u > width)
			{
				// the last remaining block must be done without NEON

				while (x < width)
				{
					if constexpr (tAdd)
					{
						sum[x] += row[x];
						sqrSum[x] += row[x] * row[x];
					}
					else
					{
						ocean_assert(sum[x] >= row[x]);
						ocean_assert(sqrSum[x] >= row[x] * row[x]);

						sum[x] -= row[x];
						sqrSum[x] -= row[x] * row[x];
					}

					++x;
				}

				break;
			}

			const uint8x16_t value_u_8x16 = vld1q_u8(row + x);

			const uint8x8_t valueA_u_8x8 = vget_low_u8(value_u_8x16);
			const uint8x8_t valueB_u_8x8 = vget_high_u8(value_u_8x16);

			if constexpr (tAdd)
			{
				vst1q_u16(sum + x + 0u, vaddw_u8(vld1q_u16(sum + x + 0u), valueA_u_8x8)); // sum = sum + value
				vst1q_u16(sum + x + 8u, vaddw_u8(vld1q_u16(sum + x + 8u), valueB_u_8x8));
			}
			else
			{
				vst1q_u16(sum + x + 0u, vsubw_u8(vld1q_u16(sum + x + 0u), valueA_u_8x8)); // sum = sum - value
				vst1q_u16(sum + x + 8u, vsubw_u8(vld1q_u16(sum + x + 8u), valueB_u_8x8));
			}

			const uint16x8_t sqrValueA_u_16x8 = vmull_u8(valueA_u_8x8, valueA_u_8x8); // sqrValue = value * value
			const uint16x8_t sqrValueB_u_16x8 = vmull_u8(valueB_u_8x8, valueB_u_8x8);

			const uint16x4_t sqrValueA0_u_16x4 = vget_low_u16(sqrValueA_u_16x8);
			const uint16x4_t sqrValueA1_u_16x4 = vget_high_u16(sqrValueA_u_16x8);
			const uint16x4_t sqrValueB0_u_16x4 = vget_low_u16(sqrValueB_u_16x8);
			const uint16x4_t sqrValueB1_u_16x4 = vget_high_u16(sqrValueB_u_16x8);

			if constexpr (tAdd)
			{
				vst1q_u32(sqrSum + x +  0u, vaddw_u16(vld1q_u32(sqrSum + x +  0u), sqrValueA0_u_16x4)); // sqrSum = sqrSum + sqrValue
				vst1q_u32(sqrSum + x +  4u, vaddw_u16(vld1q_u32(sqrSum + x +  4u), sqrValueA1_u_16x4));
				vst1q_u32(sqrSum + x +  8u, vaddw_u16(vld1q_u32(sqrSum + x +  8u), sqrValueB0_u_16x4));
				vst1q_u32(sqrSum + x + 12u, vaddw_u16(vld1q_u32(sqrSum + x + 12u), sqrValueB1_u_16x4));
			}
			else
			{
				vst1q_u32(sqrSum + x +  0u, vsubw_u16(vld1q_u32(sqrSum + x +  0u), sqrValueA0_u_16x4)); // sqrSum = sqrSum - sqrValue
				vst1q_u32(sqrSum + x +  4u, vsubw_u16(vld1q_u32(sqrSum + x +  4u), sqrValueA1_u_16x4));
				vst1q_u32(sqrSum + x +  8u, vsubw_u16(vld1q_u32(sqrSum + x +  8u), sqrValueB0_u_16x4));
				vst1q_u32(sqrSum + x + 12u, vsubw_u16(vld1q_u32(sqrSum + x + 12u), sqrValueB1_u_16x4));
			}
		}
	}
	else
	{
		for (unsigned int x = 0u; x < width; x += 8u)
		{
			if (x + 8u > width)
			{
				// the last remaining block must be done without NEON

				while (x < width)
				{
					if constexpr (tAdd)
					{
						sum[x] += row[x];
						sqrSum[x] += row[x] * row[x];
					}
					else
					{
						ocean_assert(sum[x] >= row[x]);
						ocean_assert(sqrSum[x] >= row[x] * row[x]);

						sum[x] -= row[x];
						sqrSum[x] -= row[x] * row[x];
					}

					++x;
				}

				break;
			}

			const uint8x8_t value_u_8x8 = vld1_u8(row + x);

			if constexpr (tAdd)
			{
				vst1q_u16(sum + x + 0u, vaddw_u8(vld1q_u16(sum + x + 0u), value_u_8x8)); // sum = sum + value
			}
			else
			{
				vst1q_u16(sum + x + 0u, vsubw_u8(vld1q_u16(sum + x + 0u), value_u_8x8)); // sum = sum - value
			}

			const uint16x8_t sqrValue_u_16x8 = vmull_u8(value_u_8x8, value_u_8x8); // sqrValue = value * value

			const uint16x4_t sqrValue0_u_16x4 = vget_low_u16(sqrValue_u_16x8);
			const uint16x4_t sqrValue1_u_16x4 = vget_high_u16(sqrValue_u_16x8);

			if constexpr (tAdd)
			{
				vst1q_u32(sqrSum + x +  0u, vaddw_u16(vld1q_u32(sqrSum + x +  0u), sqrValue0_u_16x4)); // sqrSum = sqrSum + sqrValue
				vst1q_u32(sqrSum + x +  4u, vaddw_u16(vld1q_u32(sqrSum + x +  4u), sqrValue1_u_16x4));
			}
			else
			{
				vst1q_u32(sqrSum + x +  0u, vsubw_u16(vld1q_u32(sqrSum + x +  0u), sqrValue0_u_16x4)); // sqrSum = sqrSum - sqrValue
				vst1q_u32(sqrSum + x +  4u, vsubw_u16(vld1q_u32(sqrSum + x +  4u), sqrValue1_u_16x4));
			}
		}
	}

#else

	OCEAN_WARNING_MISSING_IMPLEMENTATION;
	OCEAN_SUPPRESS_UNUSED_WARNING(row);
	OCEAN_SUPPRESS_UNUSED_WARNING(width);
	OCEAN_SUPPRESS_UNUSED_WARNING(sum);
	OCEAN_SUPPRESS_UNUSED_WARNING(sqrSum);

#endif // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
}

LineDetectorULF::RMSBarEdgeDetectorI::RMSBarEdgeDetectorI(const unsigned int window, const unsigned int minimalDelta) :
	EdgeDetector(window, ET_BAR),
	minimalDelta_(minimalDelta)
{
	ocean_assert(window_ >= 1u);
	ocean_assert(minimalDelta < 256u);
}

void LineDetectorULF::RMSBarEdgeDetectorI::invokeVertical(const uint8_t* frame, const unsigned int width, const unsigned int height, int16_t* responses, const unsigned int paddingElements) const
{
	ocean_assert(frame != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(responses != nullptr);

	ocean_assert(window_ >= 1u && window_ < width && window_ <= 11u); // 2 * window_ * window_ <= 255  ->  window_ <= sqrt(255/2)

	// TODO: ensure that width - (window + barSize_2) * 2u >= 8, due to NEON

	const unsigned int frameStrideElements = width + paddingElements;

	Memory windowSumsMemory = Memory::create<uint16_t>(width - (window_ - 1u));
	Memory windowSqrSumsMemory = Memory::create<uint32_t>(width - (window_ - 1u));

	uint16_t* const windowSums = windowSumsMemory.data<uint16_t>();
	uint32_t* const windowSqrSums = windowSqrSumsMemory.data<uint32_t>();

	int16_t* rowSqrResponses = responses;

	for (unsigned int y = 0u; y < height; ++y)
	{
		determineRowSums(frame, width, window_, windowSums, windowSqrSums);

		invokeRowVertical(frame, width, window_, minimalDelta_, windowSums, windowSqrSums, rowSqrResponses);

		frame += frameStrideElements;
		rowSqrResponses += width;
	}
}

bool LineDetectorULF::RMSBarEdgeDetectorI::invokeHorizontal(const uint8_t* frame, const unsigned int width, const unsigned int height, int16_t* responses, const unsigned int framePaddingElements) const
{

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	ocean_assert(frame != nullptr);
	ocean_assert(width >= 8u);

	constexpr unsigned int barSize_2 = barSize_ / 2u;

	ocean_assert(height >= window_ * 2u + barSize_);

	const unsigned int area = window_ * 2u;
	const unsigned int minimalDeltaArea2 = minimalDelta_ * area * 2u;

	// we can set the first response rows to zero
	memset(responses, 0x00, width * (window_ + barSize_2) * sizeof(int16_t));

	Memory windowSumMemory = Memory::create<uint16_t>(width);
	memset(windowSumMemory.data(), 0x00, windowSumMemory.size());
	uint16_t* const windowSum = windowSumMemory.data<uint16_t>();

	Memory windowSqrSumMemory = Memory::create<uint32_t>(width);
	memset(windowSqrSumMemory.data(), 0x00, windowSqrSumMemory.size());
	uint32_t* const windowSqrSum = windowSqrSumMemory.data<uint32_t>();

	const unsigned int frameStrideElements = width + framePaddingElements;

	const unsigned int windowBockStride1 = frameStrideElements * (window_ - 1u);

	// handle the first sum rows

	for (unsigned int y = 0u; y < window_ - 1u; ++y)
	{
		applyRowSum<true>(frame + y * frameStrideElements, width, windowSum, windowSqrSum);
	}

	for (unsigned int y = window_ + barSize_; y < window_ * 2u + barSize_ - 1u; ++y)
	{
		applyRowSum<true>(frame + y * frameStrideElements, width, windowSum, windowSqrSum);
	}

	const int16x8_t zero_s_16x8 = vdupq_n_s16(0);

	const int16x8_t one_s_16x8 = vdupq_n_s16(1);
	const int16x8_t minusone_s_16x8 = vdupq_n_s16(-1);

	const uint32x4_t area_u_32x4 = vdupq_n_u32(area);
	const uint32x4_t area2_u_32x4 = vmulq_u32(area_u_32x4, area_u_32x4);

	const uint8x8_t area_u_8x8 = vdup_n_u8(uint8_t(area));

	const uint16x8_t minimalDeltaArea2_u_16x8 = vdupq_n_u16(uint16_t(minimalDeltaArea2));

	for (unsigned int y = window_ + barSize_2; y < height - window_ - barSize_2; ++y)
	{
		int16_t* sqrResponses = responses + y * width;

		ocean_assert(y - 2u < height);
		const uint8_t* const rowSumTop = frame + (y - 2u) * frameStrideElements; // last row of the top sum block

		const uint8_t* const rowTop = frame + (y - 1u) * frameStrideElements;
		const uint8_t* const rowCenter = frame + (y + 0u) * frameStrideElements;
		const uint8_t* const rowBottom = frame + (y + 1u) * frameStrideElements;

		ocean_assert(y + window_ + 1u < height);
		const uint8_t* const rowSumBottom = frame + (y + window_ + 1u) * frameStrideElements; // last row of the bottom sum block

		ocean_assert(width >= 8u);

		for (unsigned int x = 0u; x < width; x += 8u)
		{
			if (x + 8u > width)
			{
				// the last iteration will not fit into the data,
				// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

				ocean_assert(x >= 8u && width > 8u);
				const unsigned int newX = width - 8u;

				ocean_assert(x > newX);

				// reverting the sum calculations from the last iteration

				for (unsigned int n = newX; n < x; ++n)
				{
					windowSum[n] += (rowSumTop - windowBockStride1)[n] + (rowSumBottom - windowBockStride1)[n];
					windowSum[n] -= rowSumTop[n] + rowSumBottom[n];

					windowSqrSum[n] += sqr((rowSumTop - windowBockStride1)[n]) + sqr((rowSumBottom - windowBockStride1)[n]);
					windowSqrSum[n] -= sqr(rowSumTop[n]) + sqr(rowSumBottom[n]);
				}

				x = newX;

				// the for loop will stop after this iteration
				ocean_assert(x + 8u == width);
			}

			uint16x8_t sum_u_16x8 = vld1q_u16(windowSum + x);

			uint32x4_t sqrSumA_u_32x4 = vld1q_u32(windowSqrSum + x + 0);
			uint32x4_t sqrSumB_u_32x4 = vld1q_u32(windowSqrSum + x + 4);

			// handle the current sum row(s)

			uint8x8_t rowSumTop_u_8x8 = vld1_u8(rowSumTop + x); // load remaining row to finalize sum values
			uint8x8_t rowSumBottom_u_8x8 = vld1_u8(rowSumBottom + x);

			sum_u_16x8 = vaddq_u16(sum_u_16x8, vaddl_u8(rowSumTop_u_8x8, rowSumBottom_u_8x8)); // add next top and bottom row

			uint16x8_t rowSqrSumTop_u_16x8 = vmull_u8(rowSumTop_u_8x8, rowSumTop_u_8x8); // add next top sqr row
			sqrSumA_u_32x4 = vaddw_u16(sqrSumA_u_32x4, vget_low_u16(rowSqrSumTop_u_16x8));
			sqrSumB_u_32x4 = vaddw_u16(sqrSumB_u_32x4, vget_high_u16(rowSqrSumTop_u_16x8));

			uint16x8_t rowSqrSumBottom_u_16x8 = vmull_u8(rowSumBottom_u_8x8, rowSumBottom_u_8x8); // add next bottom sqr row
			sqrSumA_u_32x4 = vaddw_u16(sqrSumA_u_32x4, vget_low_u16(rowSqrSumBottom_u_16x8));
			sqrSumB_u_32x4 = vaddw_u16(sqrSumB_u_32x4, vget_high_u16(rowSqrSumBottom_u_16x8));


			const uint32x4_t sumSqrA_u_32x4 = vmull_u16(vget_low_u16(sum_u_16x8), vget_low_u16(sum_u_16x8)); // note: sum * sum != sqrSum
			const uint32x4_t sumSqrB_u_32x4 = vmull_u16(vget_high_u16(sum_u_16x8), vget_high_u16(sum_u_16x8));

			// normalizedSqrResidual = max(area * area, area * sqrSum - sum * sum)

			const float32x4_t normalizedSqrResidualA_f_32x4 = vcvtq_f32_u32(vmaxq_u32(area2_u_32x4, vsubq_u32(vmulq_u32(area_u_32x4, sqrSumA_u_32x4), sumSqrA_u_32x4)));
			const float32x4_t normalizedSqrResidualB_f_32x4 = vcvtq_f32_u32(vmaxq_u32(area2_u_32x4, vsubq_u32(vmulq_u32(area_u_32x4, sqrSumB_u_32x4), sumSqrB_u_32x4)));

			// we calculate the (approximated) inverse of normalizedSqrResidual: 1 / normalizedSqrResidual
			float32x4_t invNormalizedSqrResidualA_f_32x4 = vrecpeq_f32(normalizedSqrResidualA_f_32x4);
			invNormalizedSqrResidualA_f_32x4 = vmulq_f32(vrecpsq_f32(normalizedSqrResidualA_f_32x4, invNormalizedSqrResidualA_f_32x4), invNormalizedSqrResidualA_f_32x4); // improving the accuracy of the approx. inverse by Newton/Raphson

			float32x4_t invNormalizedSqrResidualB_f_32x4 = vrecpeq_f32(normalizedSqrResidualB_f_32x4);
			invNormalizedSqrResidualB_f_32x4 = vmulq_f32(vrecpsq_f32(normalizedSqrResidualB_f_32x4, invNormalizedSqrResidualB_f_32x4), invNormalizedSqrResidualB_f_32x4);


			// valueMinus = int(value[0] * area) - int(sum); // zero mean values (multiplied by area)
			const uint8x8_t valueMinus_u_8x8 = vld1_u8(rowTop + x);
			int16x8_t valueMinus_s_16x8 = vsubq_s16(vreinterpretq_s16_u16(vmull_u8(valueMinus_u_8x8, area_u_8x8)), vreinterpretq_s16_u16(sum_u_16x8));

			const uint8x8_t valueCenter_u_8x8 = vld1_u8(rowCenter + x);
			int16x8_t valueCenter_s_16x8 = vsubq_s16(vreinterpretq_s16_u16(vmull_u8(valueCenter_u_8x8, area_u_8x8)), vreinterpretq_s16_u16(sum_u_16x8));

			const uint8x8_t valuePlus_u_8x8 = vld1_u8(rowBottom + x);
			int16x8_t valuePlus_s_16x8 = vsubq_s16(vreinterpretq_s16_u16(vmull_u8(valuePlus_u_8x8, area_u_8x8)), vreinterpretq_s16_u16(sum_u_16x8));

			// valueCenterSmallerZero = valueCenter < 0 ? 0xFFFFFFFF : 0x00000000
			const uint16x8_t valueCenterSmallerZero_u_16x8 = vcltq_s16(valueCenter_s_16x8, zero_s_16x8);

			// sign = valueCenterSmallerZero == 0xFFFFFFFF ? 1 : -1
			const int16x8_t sign_s_16x8 = vbslq_s16(valueCenterSmallerZero_u_16x8, minusone_s_16x8, one_s_16x8);

			// valueMinus = sign * valueMinus
			valueMinus_s_16x8 = vmulq_s16(valueMinus_s_16x8, sign_s_16x8);

			// valueCenter = sign * valueCenter
			valueCenter_s_16x8 = vmulq_s16(valueCenter_s_16x8, sign_s_16x8);

			// valuePlus = sign * valuePlus
			valuePlus_s_16x8 = vmulq_s16(valuePlus_s_16x8, sign_s_16x8);

			// maxMinusPlus = max(valueMinus, valuePlus)
			const int16x8_t maxMinusPlus_s_16x8 = vmaxq_s16(valueMinus_s_16x8, valuePlus_s_16x8);

			// peakValue = valueCenter + maxMinusPlus
			const int16x8_t peakValue_s_16x8 = vaddq_s16(valueCenter_s_16x8, maxMinusPlus_s_16x8);

			// valueMinus <= valueCenter && valuePlus < valueCenter && minimalDeltaArea2 < abs(peakValue)
			const uint16x8_t validResponse_u_16x8 = vandq_u16(vandq_u16(vcleq_s16(valueMinus_s_16x8, valueCenter_s_16x8), vcltq_s16(valuePlus_s_16x8, valueCenter_s_16x8)), vcltq_u16(minimalDeltaArea2_u_16x8, vreinterpretq_u16_s16(vabsq_s16(peakValue_s_16x8))));

			const int16x4_t peakValueA_s_16x4 = vget_low_s16(peakValue_s_16x8);
			const int16x4_t peakValueB_s_16x4 = vget_high_s16(peakValue_s_16x8);

			// peakValue^2 * 64
			const uint32x4_t sqrPeakValueA_u_32x4 = vqshlq_n_u32(vreinterpretq_u32_s32(vmull_s16(peakValueA_s_16x4, peakValueA_s_16x4)), 6);
			const uint32x4_t sqrPeakValueB_u_32x4 = vqshlq_n_u32(vreinterpretq_u32_s32(vmull_s16(peakValueB_s_16x4, peakValueB_s_16x4)), 6);

			const int32x4_t signA_s_32x4 = vmovl_s16(vget_low_s16(sign_s_16x8));
			const int32x4_t signB_s_32x4 = vmovl_s16(vget_high_s16(sign_s_16x8));

			// peakValue^2 * 64 / normalizedSqrResidual
			const int32x4_t responseA_s_32x4 = vmulq_s32(signA_s_32x4, vcvtq_s32_f32(vmulq_f32(vcvtq_f32_u32(sqrPeakValueA_u_32x4), invNormalizedSqrResidualA_f_32x4)));
			const int32x4_t responseB_s_32x4 = vmulq_s32(signB_s_32x4, vcvtq_s32_f32(vmulq_f32(vcvtq_f32_u32(sqrPeakValueB_u_32x4), invNormalizedSqrResidualB_f_32x4)));

			// saturated cast
			const int16x8_t response_s_16x8 = vcombine_s16(vqmovn_s32(responseA_s_32x4), vqmovn_s32(responseB_s_32x4));

			const int16x8_t validResponse_s_16x8 = vreinterpretq_s16_u16(vandq_u16(vreinterpretq_u16_s16(response_s_16x8), validResponse_u_16x8));

			vst1q_s16(sqrResponses + x, validResponse_s_16x8);


			// handle the last sum row(s)

			rowSumTop_u_8x8 = vld1_u8(rowSumTop - windowBockStride1 + x); // load remaining row to finalize sum values
			rowSumBottom_u_8x8 = vld1_u8(rowSumBottom - windowBockStride1 + x);

			sum_u_16x8 = vsubq_u16(sum_u_16x8, vaddl_u8(rowSumTop_u_8x8, rowSumBottom_u_8x8)); // subtract next top and bottom row

			rowSqrSumTop_u_16x8 = vmull_u8(rowSumTop_u_8x8, rowSumTop_u_8x8); // subtract next top sqr row
			sqrSumA_u_32x4 = vsubw_u16(sqrSumA_u_32x4, vget_low_u16(rowSqrSumTop_u_16x8));
			sqrSumB_u_32x4 = vsubw_u16(sqrSumB_u_32x4, vget_high_u16(rowSqrSumTop_u_16x8));

			rowSqrSumBottom_u_16x8 = vmull_u8(rowSumBottom_u_8x8, rowSumBottom_u_8x8); // subtract next bottom sqr row
			sqrSumA_u_32x4 = vsubw_u16(sqrSumA_u_32x4, vget_low_u16(rowSqrSumBottom_u_16x8));
			sqrSumB_u_32x4 = vsubw_u16(sqrSumB_u_32x4, vget_high_u16(rowSqrSumBottom_u_16x8));

			vst1q_u16(windowSum + x, sum_u_16x8);

			vst1q_u32(windowSqrSum + x + 0, sqrSumA_u_32x4);
			vst1q_u32(windowSqrSum + x + 4, sqrSumB_u_32x4);
		}
	}

	// we set the last response rows to zero
	memset(responses + width * (height - window_ - barSize_2), 0x00, width * (window_ + barSize_2) * sizeof(int16_t));

	return true;

#else // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	OCEAN_SUPPRESS_UNUSED_WARNING(frame);
	OCEAN_SUPPRESS_UNUSED_WARNING(width);
	OCEAN_SUPPRESS_UNUSED_WARNING(height);
	OCEAN_SUPPRESS_UNUSED_WARNING(responses);
	OCEAN_SUPPRESS_UNUSED_WARNING(framePaddingElements);

	return false;

#endif

}

bool LineDetectorULF::RMSBarEdgeDetectorI::hasInvokeHorizontal(const unsigned int width, const unsigned int height) const
{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	return width >= 8u && height >= window_ * 2u + barSize_;

#else

	OCEAN_SUPPRESS_UNUSED_WARNING(width);
	OCEAN_SUPPRESS_UNUSED_WARNING(height);

	return false;

#endif
}

unsigned int LineDetectorULF::RMSBarEdgeDetectorI::adjustThreshold(const unsigned int threshold) const
{
	return staticAdjustThreshold(threshold);
}

#ifdef WE_KEEP_THIS_IMPLEMENTATION_FOR_COMPARISON // 32bit sum values

void LineDetectorULF::RMSBarEdgeDetectorI::invokeRowVertical(const uint8_t* const row, const unsigned int width, const unsigned int window, const unsigned int minimalDelta, const uint32_t* const windowSums, const uint32_t* const windowSqrSums, int16_t* sqrResponses)
{
	constexpr unsigned int barSize_2 = barSize_ / 2u;

	const unsigned int area = window * 2u;
	const unsigned int minimalDeltaArea2 = minimalDelta * area * 2u;

	const unsigned int elements = width - (window + barSize_2) * 2u;

	// clear left border
	memset(sqrResponses, 0, (window + barSize_2) * sizeof(int16_t));

	const uint32_t* windowSumsL = windowSums;
	const uint32_t* windowSumsR = windowSums + window + barSize_;

	const uint32_t* windowSqrSumsL = windowSqrSums;
	const uint32_t* windowSqrSumsR = windowSqrSums + window + barSize_;

	const uint8_t* value = row + window;

	sqrResponses += window + barSize_2;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	ocean_assert(elements >= 8u);

	const int32x4_t zero_s_32x4 = vdupq_n_s32(0);

	int32x4_t one_s_32x4 = vdupq_n_s32(1);
	int32x4_t minusone_s_32x4 = vdupq_n_s32(-1);

	const uint32x4_t area_u_32x4 = vdupq_n_u32(area);
	const uint32x4_t area2_u_32x4 = vmulq_u32(area_u_32x4, area_u_32x4);

	const uint32x4_t minimalDeltaArea2_u_32x4 = vdupq_n_u32(minimalDeltaArea2);

	for (unsigned int x = 0u; x < elements; x += 8u)
	{
		if (x + 8u > elements)
		{
			// the last iteration will not fit into the data,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(x >= 8u && elements > 8u);
			const unsigned int newX = elements - 8u;

			ocean_assert(x > newX);
			const unsigned int offset = x - newX;

			value -= offset;

			windowSumsL -= offset;
			windowSumsR -= offset;
			windowSqrSumsL -= offset;
			windowSqrSumsR -= offset;

			sqrResponses -= offset;

			// the for loop will stop after this iteration
			ocean_assert(!(x + 8u < elements));
		}

		const uint32x4_t sumA_u_32x4 = vaddq_u32(vld1q_u32(windowSumsL + 0), vld1q_u32(windowSumsR + 0));
		const uint32x4_t sumB_u_32x4 = vaddq_u32(vld1q_u32(windowSumsL + 4), vld1q_u32(windowSumsR + 4));

		const uint32x4_t sqrSumA_u_32x4 = vaddq_u32(vld1q_u32(windowSqrSumsL + 0), vld1q_u32(windowSqrSumsR + 0));
		const uint32x4_t sqrSumB_u_32x4 = vaddq_u32(vld1q_u32(windowSqrSumsL + 4), vld1q_u32(windowSqrSumsR + 4));

		// normalizedSqrResidual = max(area * area, area * sqrSum - sum * sum)

		const float32x4_t normalizedSqrResidualA_f_32x4 = vcvtq_f32_u32(vmaxq_u32(area2_u_32x4, vsubq_u32(vmulq_u32(area_u_32x4, sqrSumA_u_32x4), vmulq_u32(sumA_u_32x4, sumA_u_32x4))));
		const float32x4_t normalizedSqrResidualB_f_32x4 = vcvtq_f32_u32(vmaxq_u32(area2_u_32x4, vsubq_u32(vmulq_u32(area_u_32x4, sqrSumB_u_32x4), vmulq_u32(sumB_u_32x4, sumB_u_32x4))));

		// we calculate the (approximated) inverse of normalizedSqrResidual: 1 / normalizedSqrResidual
		float32x4_t invNormalizedSqrResidualA_f_32x4 = vrecpeq_f32(normalizedSqrResidualA_f_32x4);
		invNormalizedSqrResidualA_f_32x4 = vmulq_f32(vrecpsq_f32(normalizedSqrResidualA_f_32x4, invNormalizedSqrResidualA_f_32x4), invNormalizedSqrResidualA_f_32x4); // improving the accuracy of the approx. inverse by Newton/Raphson

		float32x4_t invNormalizedSqrResidualB_f_32x4 = vrecpeq_f32(normalizedSqrResidualB_f_32x4);
		invNormalizedSqrResidualB_f_32x4 = vmulq_f32(vrecpsq_f32(normalizedSqrResidualB_f_32x4, invNormalizedSqrResidualB_f_32x4), invNormalizedSqrResidualB_f_32x4);

		// valueMinus = int(value[0] * area) - int(sum); // zero mean values (multiplied by area)
		const uint16x8_t valueMinus_u_16x8 = vmovl_u8(vld1_u8(value));
		int32x4_t valueMinusA_s_32x4 = vsubq_s32(vreinterpretq_s32_u32(vmulq_u32(vmovl_u16(vget_low_u16(valueMinus_u_16x8)), area_u_32x4)), vreinterpretq_s32_u32(sumA_u_32x4));
		int32x4_t valueMinusB_s_32x4 = vsubq_s32(vreinterpretq_s32_u32(vmulq_u32(vmovl_u16(vget_high_u16(valueMinus_u_16x8)), area_u_32x4)), vreinterpretq_s32_u32(sumB_u_32x4));

		const uint16x8_t valueCenter_u_16x8 = vmovl_u8(vld1_u8(value + 1));
		int32x4_t valueCenterA_s_32x4 = vsubq_s32(vreinterpretq_s32_u32(vmulq_u32(vmovl_u16(vget_low_u16(valueCenter_u_16x8)), area_u_32x4)), vreinterpretq_s32_u32(sumA_u_32x4));
		int32x4_t valueCenterB_s_32x4 = vsubq_s32(vreinterpretq_s32_u32(vmulq_u32(vmovl_u16(vget_high_u16(valueCenter_u_16x8)), area_u_32x4)), vreinterpretq_s32_u32(sumB_u_32x4));

		const uint16x8_t valuePlus_u_16x8 = vmovl_u8(vld1_u8(value + 2));
		int32x4_t valuePlusA_s_32x4 = vsubq_s32(vreinterpretq_s32_u32(vmulq_u32(vmovl_u16(vget_low_u16(valuePlus_u_16x8)), area_u_32x4)), vreinterpretq_s32_u32(sumA_u_32x4));
		int32x4_t valuePlusB_s_32x4 = vsubq_s32(vreinterpretq_s32_u32(vmulq_u32(vmovl_u16(vget_high_u16(valuePlus_u_16x8)), area_u_32x4)), vreinterpretq_s32_u32(sumB_u_32x4));

		// valueCenterSmallerZero = valueCenter < 0 ? 0xFFFFFFFF : 0x00000000
		const uint32x4_t valueCenterSmallerZeroA_u_32x4 = vcltq_s32(valueCenterA_s_32x4, zero_s_32x4);
		const uint32x4_t valueCenterSmallerZeroB_u_32x4 = vcltq_s32(valueCenterB_s_32x4, zero_s_32x4);

		// sign = valueCenterSmallerZero == 0xFFFFFFFF ? 1 : -1
		const int32x4_t signA_s_32x4 = vbslq_u32(valueCenterSmallerZeroA_u_32x4, minusone_s_32x4, one_s_32x4);
		const int32x4_t signB_s_32x4 = vbslq_u32(valueCenterSmallerZeroB_u_32x4, minusone_s_32x4, one_s_32x4);

		// valueMinus = sign * valueMinus
		valueMinusA_s_32x4 = vmulq_s32(valueMinusA_s_32x4, signA_s_32x4);
		valueMinusB_s_32x4 = vmulq_s32(valueMinusB_s_32x4, signB_s_32x4);

		// valueCenter = sign * valueCenter
		valueCenterA_s_32x4 = vmulq_s32(valueCenterA_s_32x4, signA_s_32x4);
		valueCenterB_s_32x4 = vmulq_s32(valueCenterB_s_32x4, signB_s_32x4);

		// valuePlus = sign * valuePlus
		valuePlusA_s_32x4 = vmulq_s32(valuePlusA_s_32x4, signA_s_32x4);
		valuePlusB_s_32x4 = vmulq_s32(valuePlusB_s_32x4, signB_s_32x4);

		// maxMinusPlus = max(valueMinus, valuePlus)
		const int32x4_t maxMinusPlusA_s_32x4 = vmaxq_s32(valueMinusA_s_32x4, valuePlusA_s_32x4);
		const int32x4_t maxMinusPlusB_s_32x4 = vmaxq_s32(valueMinusB_s_32x4, valuePlusB_s_32x4);

		// peakValue = valueCenter + maxMinusPlus
		const int32x4_t peakValueA_s_32x4 = vaddq_s32(valueCenterA_s_32x4, maxMinusPlusA_s_32x4);
		const int32x4_t peakValueB_s_32x4 = vaddq_s32(valueCenterB_s_32x4, maxMinusPlusB_s_32x4);

		// valueMinus <= valueCenter && valuePlus < valueCenter && minimalDeltaArea2 < abs(peakValue)
		const uint32x4_t validResponseA_u_32x4 = vandq_u32(vandq_u32(vcleq_s32(valueMinusA_s_32x4, valueCenterA_s_32x4), vcltq_s32(valuePlusA_s_32x4, valueCenterA_s_32x4)), vcltq_u32(minimalDeltaArea2_u_32x4, vreinterpretq_u32_s32(vabsq_s32(peakValueA_s_32x4))));
		const uint32x4_t validResponseB_u_32x4 = vandq_u32(vandq_u32(vcleq_s32(valueMinusB_s_32x4, valueCenterB_s_32x4), vcltq_s32(valuePlusB_s_32x4, valueCenterB_s_32x4)), vcltq_u32(minimalDeltaArea2_u_32x4, vreinterpretq_u32_s32(vabsq_s32(peakValueB_s_32x4))));

		const uint16x8_t validResponse_u_16x8 = vcombine_u16(vmovn_u32(validResponseA_u_32x4), vmovn_u32(validResponseB_u_32x4));

		// peakValue^2 * 64
		const int32x4_t sqrPeakValueA_u_32x4 = vqshlq_n_u32(vreinterpretq_u32_s32(vmulq_s32(peakValueA_s_32x4, peakValueA_s_32x4)), 6);
		const int32x4_t sqrPeakValueB_u_32x4 = vqshlq_n_u32(vreinterpretq_u32_s32(vmulq_s32(peakValueB_s_32x4, peakValueB_s_32x4)), 6);

		// peakValue^2 * 64 / normalizedSqrResidual
		const int32x4_t responseA_s_32x4 = vmulq_s32(signA_s_32x4, vcvtq_s32_f32(vmulq_f32(vcvtq_f32_u32(sqrPeakValueA_u_32x4), invNormalizedSqrResidualA_f_32x4)));
		const int32x4_t responseB_s_32x4 = vmulq_s32(signB_s_32x4, vcvtq_s32_f32(vmulq_f32(vcvtq_f32_u32(sqrPeakValueB_u_32x4), invNormalizedSqrResidualB_f_32x4)));

		// saturated cast
		const int16x8_t response_s_16x8 = vcombine_s16(vqmovn_s32(responseA_s_32x4), vqmovn_s32(responseB_s_32x4));

		const int16x8_t validResponse_s_16x8 = vreinterpretq_s16_u16(vandq_u16(vreinterpretq_u16_s16(response_s_16x8), validResponse_u_16x8));

		vst1q_s16(sqrResponses, validResponse_s_16x8);

		value += 8;

		windowSumsL += 8;
		windowSumsR += 8;
		windowSqrSumsL += 8;
		windowSqrSumsR += 8;

		sqrResponses += 8;
	}

#else // OCEAN_HARDWARE_NEON_VERSION >= 10

	const int16_t* const responsesEnd = sqrResponses + elements;

	while (sqrResponses != responsesEnd)
	{
		ocean_assert(sqrResponses < responsesEnd);

		ocean_assert(windowSumsL < windowSums + width - (window - 1u));
		ocean_assert(windowSumsR < windowSums + width - (window - 1u));
		ocean_assert(windowSqrSumsL < windowSqrSums + width - (window - 1u));
		ocean_assert(windowSqrSumsR < windowSqrSums + width - (window - 1u));

		// first we ensure that the center value is a peak value (positive or negative)
		const uint32_t value_window = value[1] * window;

		if ((value_window < *windowSumsL && value_window < *windowSumsR) || (value_window > *windowSumsL && value_window > *windowSumsR))
		{
			const uint32_t sum = *windowSumsL + *windowSumsR; // sum of left and right values within window
			const uint32_t sqrSum = *windowSqrSumsL + *windowSqrSumsR; // sum of left and right squared values within window

			// we determine the root mean square residual:

			// mean = 1/n * sum(yi)

			// rms = sqrt(1/n * sum[(mean - yi)^2])
			//     = sqrt(1/n * sum[mean^2 - 2 * mean * yi + yi^2])
			//     = sqrt(1/n * (n * mean^2 - 2 * n * mean^2 + sum[yi^2]))
			//     = sqrt(mean^2 - 2 * mean^2 + 1/n * sum[yi^2])
			//     = sqrt(1/n * sum[yi^2] - mean^2)
			//     = sqrt(1/n * sum[yi^2] - 1/n^2 * sum[yi]^2)
			//     = 1/n * sqrt(n * sum[yi^2] - sum[yi]^2)

			// due to performance reasons we avoid sqrt and keep the squared residual result

			//       rms^2 = 1/n^2 * (n * sum[yi^2] - sum[yi]^2)
			// n^2 * rms^2 =          n * sum[yi^2] - sum[yi]^2

			// normalized residual (multiplied with area)
			ocean_assert(area * sqrSum >= sum);
			uint32_t normalizedSqrResidual = area * sqrSum - sum * sum;

			// assume minimum residual of 1.0 (* area) to avoid large responses in saturated areas, and due to jpg artifacts
			normalizedSqrResidual = max(area * area, normalizedSqrResidual);
			ocean_assert(normalizedSqrResidual >= area * area);

			// value = row[x] - sum / area = row[x] - average

			const int valueMinus = int(value[0] * area) - int(sum); // zero mean values (multiplied by area)
			const int valueCenter = int(value[1] * area) - int(sum);
			const int valuePlus = int(value[2] * area) - int(sum);

			if (valueCenter < 0)
			{
				// we have a dark edge

				if (valueMinus >= valueCenter && valueCenter < valuePlus)
				{
					const int peakValue = valueCenter + std::min(valueMinus, valuePlus); // normalization by 2 in next row

					if ((uint32_t)std::abs(peakValue) >= minimalDeltaArea2)
					{
						ocean_assert(double(peakValue * peakValue) * double(64u) + double(normalizedSqrResidual / 2u) < double(NumericT<uint32_t>::maxValue()));

						/**
						 * sqrResponse = [64 * (2 * area) * (peakValue - average)]^2 / [area * residual]^2
						 *             = [64 * 2 * (peakValue - average) / residual]^2
						 *             = 16^ * [(peakValue - average) / residual]^2
						 */
						const int sqrResponse = -int((sqr(peakValue) * 64u + normalizedSqrResidual / 2u) / normalizedSqrResidual);
						ocean_assert(sqrResponse <= 0);

						*sqrResponses = minmax<int>(NumericT<int16_t>::minValue(), sqrResponse, NumericT<int16_t>::maxValue());
					}
					else
					{
						*sqrResponses = 0;
					}
				}
				else
				{
					*sqrResponses = 0;
				}
			}
			else
			{
				// we have a bright edge

				if (valueMinus <= valueCenter && valueCenter > valuePlus)
				{
					const int peakValue = valueCenter + std::max(valueMinus, valuePlus); // normalization by 2 in next row

					if ((uint32_t)std::abs(peakValue) >= minimalDeltaArea2)
					{
						ocean_assert(double(peakValue * peakValue) * double(64u) + double(normalizedSqrResidual / 2u) < double(NumericT<uint32_t>::maxValue()));

						/**
						 * sqrResponse = [64 * (2 * area) * (peakValue - average)]^2 / [area * residual]^2
						 *             = [64 * 2 * (peakValue - average) / residual]^2
						 *             = 16^ * [(peakValue - average) / residual]^2
						 */
						const int sqrResponse = int((sqr(peakValue) * 64u + normalizedSqrResidual / 2u) / normalizedSqrResidual);
						ocean_assert(sqrResponse >= 0);

						*sqrResponses = minmax<int>(NumericT<int16_t>::minValue(), sqrResponse, NumericT<int16_t>::maxValue());
					}
					else
					{
						*sqrResponses = 0;
					}
				}
				else
				{
					*sqrResponses = 0;
				}
			}
		}
		else
		{
			*sqrResponses = 0;
		}

		++windowSumsL;
		++windowSumsR;

		++windowSqrSumsL;
		++windowSqrSumsR;

		++value;
		++sqrResponses;
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	// clear right border
	memset(sqrResponses, 0, (window + barSize_2) * sizeof(int16_t));
}

#endif // WE_KEEP_THIS_IMPLEMENTATION_FOR_COMPARISON

void LineDetectorULF::RMSBarEdgeDetectorI::invokeRowVertical(const uint8_t* const row, const unsigned int width, const unsigned int window, const unsigned int minimalDelta, const uint16_t* const windowSums, const uint32_t* const windowSqrSums, int16_t* sqrResponses)
{
	constexpr unsigned int barSize_2 = barSize_ / 2u;

	const unsigned int area = window * 2u;
	const unsigned int minimalDeltaArea2 = minimalDelta * area * 2u;

	const unsigned int elements = width - (window + barSize_2) * 2u;

	// clear left border
	memset(sqrResponses, 0, (window + barSize_2) * sizeof(int16_t));

	const uint16_t* windowSumsL = windowSums;
	const uint16_t* windowSumsR = windowSums + window + barSize_;

	const uint32_t* windowSqrSumsL = windowSqrSums;
	const uint32_t* windowSqrSumsR = windowSqrSums + window + barSize_;

	const uint8_t* value = row + window;

	sqrResponses += window + barSize_2;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	ocean_assert(elements >= 8u);

	const int16x8_t zero_s_16x8 = vdupq_n_s16(0);

	int16x8_t one_s_16x8 = vdupq_n_s16(1);
	int16x8_t minusone_s_16x8 = vdupq_n_s16(-1);

	const uint32x4_t area_u_32x4 = vdupq_n_u32(area);
	const uint32x4_t area2_u_32x4 = vmulq_u32(area_u_32x4, area_u_32x4);

	const uint8x8_t area_u_8x8 = vdup_n_u8(uint8_t(area));

	const uint16x8_t minimalDeltaArea2_u_16x8 = vdupq_n_u16(uint16_t(minimalDeltaArea2));

	for (unsigned int x = 0u; x < elements; x += 8u)
	{
		if (x + 8u > elements)
		{
			// the last iteration will not fit into the data,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(x >= 8u && elements > 8u);
			const unsigned int newX = elements - 8u;

			ocean_assert(x > newX);
			const unsigned int offset = x - newX;

			value -= offset;

			windowSumsL -= offset;
			windowSumsR -= offset;
			windowSqrSumsL -= offset;
			windowSqrSumsR -= offset;

			sqrResponses -= offset;

			// the for loop will stop after this iteration
			ocean_assert(!(x + 8u < elements));
		}

		const uint16x8_t sum_u_16x8 = vaddq_u16(vld1q_u16(windowSumsL), vld1q_u16(windowSumsR));

		const uint32x4_t sqrSumA_u_32x4 = vaddq_u32(vld1q_u32(windowSqrSumsL + 0), vld1q_u32(windowSqrSumsR + 0));
		const uint32x4_t sqrSumB_u_32x4 = vaddq_u32(vld1q_u32(windowSqrSumsL + 4), vld1q_u32(windowSqrSumsR + 4));

		const uint32x4_t sumSqrA_u_32x4 = vmull_u16(vget_low_u16(sum_u_16x8), vget_low_u16(sum_u_16x8)); // note: sum * sum != sqrSum
		const uint32x4_t sumSqrB_u_32x4 = vmull_u16(vget_high_u16(sum_u_16x8), vget_high_u16(sum_u_16x8));

		// normalizedSqrResidual = max(area * area, area * sqrSum - sum * sum)

		const float32x4_t normalizedSqrResidualA_f_32x4 = vcvtq_f32_u32(vmaxq_u32(area2_u_32x4, vsubq_u32(vmulq_u32(area_u_32x4, sqrSumA_u_32x4), sumSqrA_u_32x4)));
		const float32x4_t normalizedSqrResidualB_f_32x4 = vcvtq_f32_u32(vmaxq_u32(area2_u_32x4, vsubq_u32(vmulq_u32(area_u_32x4, sqrSumB_u_32x4), sumSqrB_u_32x4)));

		// we calculate the (approximated) inverse of normalizedSqrResidual: 1 / normalizedSqrResidual
		float32x4_t invNormalizedSqrResidualA_f_32x4 = vrecpeq_f32(normalizedSqrResidualA_f_32x4);
		invNormalizedSqrResidualA_f_32x4 = vmulq_f32(vrecpsq_f32(normalizedSqrResidualA_f_32x4, invNormalizedSqrResidualA_f_32x4), invNormalizedSqrResidualA_f_32x4); // improving the accuracy of the approx. inverse by Newton/Raphson

		float32x4_t invNormalizedSqrResidualB_f_32x4 = vrecpeq_f32(normalizedSqrResidualB_f_32x4);
		invNormalizedSqrResidualB_f_32x4 = vmulq_f32(vrecpsq_f32(normalizedSqrResidualB_f_32x4, invNormalizedSqrResidualB_f_32x4), invNormalizedSqrResidualB_f_32x4);

		// valueMinus = int(value[0] * area) - int(sum); // zero mean values (multiplied by area)
		const uint8x8_t valueMinus_u_8x8 = vld1_u8(value);
		int16x8_t valueMinus_s_16x8 = vsubq_s16(vreinterpretq_s16_u16(vmull_u8(valueMinus_u_8x8, area_u_8x8)), vreinterpretq_s16_u16(sum_u_16x8));

		const uint8x8_t valueCenter_u_8x8 = vld1_u8(value + 1);
		int16x8_t valueCenter_s_16x8 = vsubq_s16(vreinterpretq_s16_u16(vmull_u8(valueCenter_u_8x8, area_u_8x8)), vreinterpretq_s16_u16(sum_u_16x8));

		const uint8x8_t valuePlus_u_8x8 = vld1_u8(value + 2);
		int16x8_t valuePlus_s_16x8 = vsubq_s16(vreinterpretq_s16_u16(vmull_u8(valuePlus_u_8x8, area_u_8x8)), vreinterpretq_s16_u16(sum_u_16x8));

		// valueCenterSmallerZero = valueCenter < 0 ? 0xFFFFFFFF : 0x00000000
		const uint16x8_t valueCenterSmallerZero_u_16x8 = vcltq_s16(valueCenter_s_16x8, zero_s_16x8);

		// sign = valueCenterSmallerZero == 0xFFFFFFFF ? 1 : -1
		const int16x8_t sign_s_16x8 = vbslq_s16(valueCenterSmallerZero_u_16x8, minusone_s_16x8, one_s_16x8);

		// valueMinus = sign * valueMinus
		valueMinus_s_16x8 = vmulq_s16(valueMinus_s_16x8, sign_s_16x8);

		// valueCenter = sign * valueCenter
		valueCenter_s_16x8 = vmulq_s16(valueCenter_s_16x8, sign_s_16x8);

		// valuePlus = sign * valuePlus
		valuePlus_s_16x8 = vmulq_s16(valuePlus_s_16x8, sign_s_16x8);

		// maxMinusPlus = max(valueMinus, valuePlus)
		const int16x8_t maxMinusPlus_s_16x8 = vmaxq_s16(valueMinus_s_16x8, valuePlus_s_16x8);

		// peakValue = valueCenter + maxMinusPlus
		const int16x8_t peakValue_s_16x8 = vaddq_s16(valueCenter_s_16x8, maxMinusPlus_s_16x8);

		// valueMinus <= valueCenter && valuePlus < valueCenter && minimalDeltaArea2 < abs(peakValue)
		const uint16x8_t validResponse_u_16x8 = vandq_u16(vandq_u16(vcleq_s16(valueMinus_s_16x8, valueCenter_s_16x8), vcltq_s16(valuePlus_s_16x8, valueCenter_s_16x8)), vcltq_u16(minimalDeltaArea2_u_16x8, vreinterpretq_u16_s16(vabsq_s16(peakValue_s_16x8))));

		const int16x4_t peakValueA_s_16x4 = vget_low_s16(peakValue_s_16x8);
		const int16x4_t peakValueB_s_16x4 = vget_high_s16(peakValue_s_16x8);

		// peakValue^2 * 64
		const uint32x4_t sqrPeakValueA_u_32x4 = vqshlq_n_u32(vreinterpretq_u32_s32(vmull_s16(peakValueA_s_16x4, peakValueA_s_16x4)), 6);
		const uint32x4_t sqrPeakValueB_u_32x4 = vqshlq_n_u32(vreinterpretq_u32_s32(vmull_s16(peakValueB_s_16x4, peakValueB_s_16x4)), 6);

		const int32x4_t signA_s_32x4 = vmovl_s16(vget_low_s16(sign_s_16x8));
		const int32x4_t signB_s_32x4 = vmovl_s16(vget_high_s16(sign_s_16x8));

		// peakValue^2 * 64 / normalizedSqrResidual
		const int32x4_t responseA_s_32x4 = vmulq_s32(signA_s_32x4, vcvtq_s32_f32(vmulq_f32(vcvtq_f32_u32(sqrPeakValueA_u_32x4), invNormalizedSqrResidualA_f_32x4)));
		const int32x4_t responseB_s_32x4 = vmulq_s32(signB_s_32x4, vcvtq_s32_f32(vmulq_f32(vcvtq_f32_u32(sqrPeakValueB_u_32x4), invNormalizedSqrResidualB_f_32x4)));

		// saturated cast
		const int16x8_t response_s_16x8 = vcombine_s16(vqmovn_s32(responseA_s_32x4), vqmovn_s32(responseB_s_32x4));

		const int16x8_t validResponse_s_16x8 = vreinterpretq_s16_u16(vandq_u16(vreinterpretq_u16_s16(response_s_16x8), validResponse_u_16x8));

		vst1q_s16(sqrResponses, validResponse_s_16x8);

		value += 8;

		windowSumsL += 8;
		windowSumsR += 8;
		windowSqrSumsL += 8;
		windowSqrSumsR += 8;

		sqrResponses += 8;
	}

#else // OCEAN_HARDWARE_NEON_VERSION >= 10

	const int16_t* const responsesEnd = sqrResponses + elements;

	while (sqrResponses != responsesEnd)
	{
		ocean_assert(sqrResponses < responsesEnd);

		ocean_assert(windowSumsL < windowSums + width - (window - 1u));
		ocean_assert(windowSumsR < windowSums + width - (window - 1u));
		ocean_assert(windowSqrSumsL < windowSqrSums + width - (window - 1u));
		ocean_assert(windowSqrSumsR < windowSqrSums + width - (window - 1u));

		// first we ensure that the center value is a peak value (positive or negative)
		const uint32_t value_window = value[1] * window;

		if ((value_window < *windowSumsL && value_window < *windowSumsR) || (value_window > *windowSumsL && value_window > *windowSumsR))
		{
			const uint32_t sum = *windowSumsL + *windowSumsR; // sum of left and right values within window
			const uint32_t sqrSum = *windowSqrSumsL + *windowSqrSumsR; // sum of left and right squared values within window

			// we determine the root mean square residual:

			// mean = 1/n * sum(yi)

			// rms = sqrt(1/n * sum[(mean - yi)^2])
			//     = sqrt(1/n * sum[mean^2 - 2 * mean * yi + yi^2])
			//     = sqrt(1/n * (n * mean^2 - 2 * n * mean^2 + sum[yi^2]))
			//     = sqrt(mean^2 - 2 * mean^2 + 1/n * sum[yi^2])
			//     = sqrt(1/n * sum[yi^2] - mean^2)
			//     = sqrt(1/n * sum[yi^2] - 1/n^2 * sum[yi]^2)
			//     = 1/n * sqrt(n * sum[yi^2] - sum[yi]^2)

			// due to performance reasons we avoid sqrt and keep the squared residual result

			//       rms^2 = 1/n^2 * (n * sum[yi^2] - sum[yi]^2)
			// n^2 * rms^2 =          n * sum[yi^2] - sum[yi]^2

			// normalized residual (multiplied with area)
			ocean_assert(area * sqrSum >= sum);
			uint32_t normalizedSqrResidual = area * sqrSum - sum * sum;

			// assume minimum residual of 1.0 (* area) to avoid large responses in saturated areas, and due to jpg artifacts
			normalizedSqrResidual = max(area * area, normalizedSqrResidual);
			ocean_assert(normalizedSqrResidual >= area * area);

			// value = row[x] - sum / area = row[x] - average

			const int valueMinus = int(value[0] * area) - int(sum); // zero mean values (multiplied by area)
			const int valueCenter = int(value[1] * area) - int(sum);
			const int valuePlus = int(value[2] * area) - int(sum);

			if (valueCenter < 0)
			{
				// we have a dark edge

				if (valueMinus >= valueCenter && valueCenter < valuePlus)
				{
					const int peakValue = valueCenter + std::min(valueMinus, valuePlus); // normalization by 2 in next row

					if ((uint32_t)std::abs(peakValue) >= minimalDeltaArea2)
					{
						ocean_assert(double(peakValue * peakValue) * double(64u) + double(normalizedSqrResidual / 2u) < double(NumericT<uint32_t>::maxValue()));

						/**
						 * sqrResponse = [64 * (2 * area) * (peakValue - average)]^2 / [area * residual]^2
						 *             = [64 * 2 * (peakValue - average) / residual]^2
						 *             = 16^ * [(peakValue - average) / residual]^2
						 */
						const int sqrResponse = -int((sqr(peakValue) * 64u + normalizedSqrResidual / 2u) / normalizedSqrResidual);
						ocean_assert(sqrResponse <= 0);

						*sqrResponses = int16_t(minmax<int>(NumericT<int16_t>::minValue(), sqrResponse, NumericT<int16_t>::maxValue()));
					}
					else
					{
						*sqrResponses = 0;
					}
				}
				else
				{
					*sqrResponses = 0;
				}
			}
			else
			{
				// we have a bright edge

				if (valueMinus <= valueCenter && valueCenter > valuePlus)
				{
					const int peakValue = valueCenter + std::max(valueMinus, valuePlus); // normalization by 2 in next row

					if ((uint32_t)std::abs(peakValue) >= minimalDeltaArea2)
					{
						ocean_assert(double(peakValue * peakValue) * double(64u) + double(normalizedSqrResidual / 2u) < double(NumericT<uint32_t>::maxValue()));

						/**
						 * sqrResponse = [64 * (2 * area) * (peakValue - average)]^2 / [area * residual]^2
						 *             = [64 * 2 * (peakValue - average) / residual]^2
						 *             = 16^ * [(peakValue - average) / residual]^2
						 */
						const int sqrResponse = int((sqr(peakValue) * 64u + normalizedSqrResidual / 2u) / normalizedSqrResidual);
						ocean_assert(sqrResponse >= 0);

						*sqrResponses = int16_t(minmax<int>(NumericT<int16_t>::minValue(), sqrResponse, NumericT<int16_t>::maxValue()));
					}
					else
					{
						*sqrResponses = 0;
					}
				}
				else
				{
					*sqrResponses = 0;
				}
			}
		}
		else
		{
			*sqrResponses = 0;
		}

		++windowSumsL;
		++windowSumsR;

		++windowSqrSumsL;
		++windowSqrSumsR;

		++value;
		++sqrResponses;
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	// clear right border
	memset(sqrResponses, 0, (window + barSize_2) * sizeof(int16_t));
}

LineDetectorULF::RMSStepEdgeDetectorI::RMSStepEdgeDetectorI(const unsigned int window) :
	EdgeDetector(window, ET_STEP)
{
	ocean_assert(window_ >= 1u);
}

void LineDetectorULF::RMSStepEdgeDetectorI::invokeVertical(const uint8_t* frame, const unsigned int width, const unsigned int height, int16_t* response, const unsigned int paddingElements) const
{
	ocean_assert(frame != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(response != nullptr);

	ocean_assert(window_ >= 1u && window_ < width);

	// **TODO** ensure that width - (window + stepSize_2) * 2u >= 8 due to NEON

	const unsigned int frameStrideElements = width + paddingElements;

	Memory windowSumsMemory = Memory::create<uint16_t>(width - (window_ - 1u));
	Memory windowSqrSumsMemory = Memory::create<uint32_t>(width - (window_ - 1u));

	uint16_t* const windowSums = windowSumsMemory.data<uint16_t>();
	uint32_t* const windowSqrSums = windowSqrSumsMemory.data<uint32_t>();

	int16_t* rowSqrResponses = response;

	for (unsigned int y = 0u; y < height; ++y)
	{
		determineRowSums(frame, width, window_, windowSums, windowSqrSums);

		invokeRowVertical(frame, width, window_, windowSums, windowSqrSums, rowSqrResponses);

		frame += frameStrideElements;
		rowSqrResponses += width;
	}
}

bool LineDetectorULF::RMSStepEdgeDetectorI::invokeHorizontal(const uint8_t* frame, const unsigned int width, const unsigned int height, int16_t* responses, const unsigned int paddingElements) const
{

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	ocean_assert(frame != nullptr);
	ocean_assert(width >= 8u);

	constexpr unsigned int stepSize_2 = stepSize_ / 2u; // 0 in case 'stepSize == 1'

	const unsigned int sqrWindow2 = window_ * window_ * 2u;

	// we can set the first response rows to zero
	memset(responses, 0x00, width * (window_ + stepSize_2) * sizeof(int16_t));

	// we use an intermediate ring buffer with 3 response rows in which we store the responses,
	// followed by a non-maximum supression which will be written to the actual responses frame
	Memory responseRingBufferMemory = Memory::create<int16_t>(width * 3u);
	memset(responseRingBufferMemory.data(), 0x00, responseRingBufferMemory.size());

	Memory windowSumMemory = Memory::create<uint16_t>(width * 2u);
	memset(windowSumMemory.data(), 0x00, windowSumMemory.size());
	uint16_t* const windowSumTop = windowSumMemory.data<uint16_t>();
	uint16_t* const windowSumBottom = windowSumMemory.data<uint16_t>() + width;

	Memory windowSqrSumMemory = Memory::create<uint32_t>(width * 2u);
	memset(windowSqrSumMemory.data(), 0x00, windowSqrSumMemory.size());
	uint32_t* const windowSqrSumTop = windowSqrSumMemory.data<uint32_t>();
	uint32_t* const windowSqrSumBottom = windowSqrSumMemory.data<uint32_t>() + width;

	const unsigned int frameStrideElements = width + paddingElements;

	const unsigned int windowBockStride1 = frameStrideElements * (window_ - 1u);

	// handle the first sum rows

	for (unsigned int y = 0u; y < window_ - 1u; ++y)
	{
		applyRowSum<true>(frame + y * frameStrideElements, width, windowSumTop, windowSqrSumTop);
	}

	for (unsigned int y = window_ + stepSize_; y < window_ * 2u + stepSize_ - 1u; ++y)
	{
		applyRowSum<true>(frame + y * frameStrideElements, width, windowSumBottom, windowSqrSumBottom);
	}

	const uint32x4_t window_u_32x4 = vdupq_n_u32(window_);
	const uint32x4_t sqrWindow2_u_32x4 = vdupq_n_u32(sqrWindow2);

	const int16x8_t zero_s_16x8 = vdupq_n_s16(0);

	const int16x8_t one_s_16x8 = vdupq_n_s16(1);
	const int16x8_t minusone_s_16x8 = vdupq_n_s16(-1);

	unsigned int responseRingBufferIndex = 0u;

	for (unsigned int y = window_ + stepSize_2; y < height - window_ - stepSize_2 + 1u; ++y)
	{
		int16_t* sqrResponsesIntermediate = responseRingBufferMemory.data<int16_t>() + (responseRingBufferIndex % 3u) * width; // the current intermediate ring buffer row

		if (y < height - window_ - stepSize_2)
		{
			ocean_assert(y -  window_ - stepSize_2 < height);
			const uint8_t* const rowSumTop = frame + (y -  window_ - stepSize_2) * frameStrideElements; // first row of top sum-block

			ocean_assert(y + stepSize_2 + 1u < height);
			const uint8_t* const rowSumBottom = frame + (y + stepSize_2 + 1u) * frameStrideElements; // first row of bottom sum-block

			ocean_assert(width >= 8u);

			for (unsigned int x = 0u; x < width; x += 8u)
			{
				if (x + 8u > width)
				{
					// the last iteration will not fit into the data,
					// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

					ocean_assert(x >= 8u && width > 8u);
					const unsigned int newX = width - 8u;

					ocean_assert(x > newX);

					// reverting the sum calculations from the last iteration

					for (unsigned int n = newX; n < x; ++n)
					{
						windowSumTop[n] += (rowSumTop)[n];
						windowSumTop[n] -= (rowSumTop + windowBockStride1)[n];

						windowSumBottom[n] += (rowSumBottom)[n];
						windowSumBottom[n] -= (rowSumBottom + windowBockStride1)[n];

						windowSqrSumTop[n] += sqr((rowSumTop)[n]);
						windowSqrSumTop[n] -= sqr((rowSumTop + windowBockStride1)[n]);

						windowSqrSumBottom[n] += sqr((rowSumBottom)[n]);
						windowSqrSumBottom[n] -= sqr((rowSumBottom + windowBockStride1)[n]);
					}

					x = newX;

					// the for loop will stop after this iteration
					ocean_assert(x + 8u == width);
				}

				uint16x8_t sumTop_u_16x8 = vld1q_u16(windowSumTop + x);
				uint16x8_t sumBottom_u_16x8 = vld1q_u16(windowSumBottom + x);

				uint32x4_t sqrSumTopA_u_32x4 = vld1q_u32(windowSqrSumTop + x + 0);
				uint32x4_t sqrSumTopB_u_32x4 = vld1q_u32(windowSqrSumTop + x + 4);

				uint32x4_t sqrSumBottomA_u_32x4 = vld1q_u32(windowSqrSumBottom + x + 0);
				uint32x4_t sqrSumBottomB_u_32x4 = vld1q_u32(windowSqrSumBottom + x + 4);


				// handle the current sum row(s)

				uint8x8_t rowSumTop_u_8x8 = vld1_u8(rowSumTop + windowBockStride1 + x); // load remaining row to finalize sum values
				uint8x8_t rowSumBottom_u_8x8 = vld1_u8(rowSumBottom + windowBockStride1 + x);

				sumTop_u_16x8 = vaddw_u8(sumTop_u_16x8, rowSumTop_u_8x8); // add next top row
				sumBottom_u_16x8 = vaddw_u8(sumBottom_u_16x8, rowSumBottom_u_8x8); // add next bottom row

				uint16x8_t rowSqrSumTop_u_16x8 = vmull_u8(rowSumTop_u_8x8, rowSumTop_u_8x8); // add next top sqr row
				sqrSumTopA_u_32x4 = vaddw_u16(sqrSumTopA_u_32x4, vget_low_u16(rowSqrSumTop_u_16x8));
				sqrSumTopB_u_32x4 = vaddw_u16(sqrSumTopB_u_32x4, vget_high_u16(rowSqrSumTop_u_16x8));

				uint16x8_t rowSqrSumBottom_u_16x8 = vmull_u8(rowSumBottom_u_8x8, rowSumBottom_u_8x8); // add next bottom sqr row
				sqrSumBottomA_u_32x4 = vaddw_u16(sqrSumBottomA_u_32x4, vget_low_u16(rowSqrSumBottom_u_16x8));
				sqrSumBottomB_u_32x4 = vaddw_u16(sqrSumBottomB_u_32x4, vget_high_u16(rowSqrSumBottom_u_16x8));


				// normalizedSqrResidualL = window * *windowSqrSumsL - sqr(*windowSumsL);
				const uint32x4_t normalizedSqrResidualTopA_u_32x4 = vsubq_u32(vmulq_u32(window_u_32x4, sqrSumTopA_u_32x4), vmull_u16(vget_low_u16(sumTop_u_16x8), vget_low_u16(sumTop_u_16x8)));
				const uint32x4_t normalizedSqrResidualTopB_u_32x4 = vsubq_u32(vmulq_u32(window_u_32x4, sqrSumTopB_u_32x4), vmull_u16(vget_high_u16(sumTop_u_16x8), vget_high_u16(sumTop_u_16x8)));

				const uint32x4_t normalizedSqrResidualBottomA_u_32x4 = vsubq_u32(vmulq_u32(window_u_32x4, sqrSumBottomA_u_32x4), vmull_u16(vget_low_u16(sumBottom_u_16x8), vget_low_u16(sumBottom_u_16x8)));
				const uint32x4_t normalizedSqrResidualBottomB_u_32x4 = vsubq_u32(vmulq_u32(window_u_32x4, sqrSumBottomB_u_32x4), vmull_u16(vget_high_u16(sumBottom_u_16x8), vget_high_u16(sumBottom_u_16x8)));

				const float32x4_t normalizedSqrResidualA_f_32x4 = vcvtq_f32_u32(vmaxq_u32(sqrWindow2_u_32x4, vaddq_u32(normalizedSqrResidualTopA_u_32x4, normalizedSqrResidualBottomA_u_32x4)));
				const float32x4_t normalizedSqrResidualB_f_32x4 = vcvtq_f32_u32(vmaxq_u32(sqrWindow2_u_32x4, vaddq_u32(normalizedSqrResidualTopB_u_32x4, normalizedSqrResidualBottomB_u_32x4)));

				// we calculate the (approximated) inverse of normalizedSqrResidual: 1 / normalizedSqrResidual
				float32x4_t invNormalizedSqrResidualA_f_32x4 = vrecpeq_f32(normalizedSqrResidualA_f_32x4);
				invNormalizedSqrResidualA_f_32x4 = vmulq_f32(vrecpsq_f32(normalizedSqrResidualA_f_32x4, invNormalizedSqrResidualA_f_32x4), invNormalizedSqrResidualA_f_32x4); // improving the accuracy of the approx. inverse by Newton/Raphson

				float32x4_t invNormalizedSqrResidualB_f_32x4 = vrecpeq_f32(normalizedSqrResidualB_f_32x4);
				invNormalizedSqrResidualB_f_32x4 = vmulq_f32(vrecpsq_f32(normalizedSqrResidualB_f_32x4, invNormalizedSqrResidualB_f_32x4), invNormalizedSqrResidualB_f_32x4);

				// delta = *windowSumsL - *windowSumsR;
				const int16x8_t delta_s_16x8 = vsubq_s16(vreinterpretq_s16_u16(sumTop_u_16x8), vreinterpretq_s16_u16(sumBottom_u_16x8));

				// valueCenterSmallerZero = valueCenter < 0 ? 0xFFFFFFFF : 0x00000000
				const uint16x8_t deltaSmallerZero_u_16x8 = vcltq_s16(delta_s_16x8, zero_s_16x8);

				// sign = valueCenterMinusZero == 0xFFFFFFFF ? -1 : 1
				const int16x8_t sign_s_16x8 = vbslq_s16(deltaSmallerZero_u_16x8, minusone_s_16x8, one_s_16x8);

				const int32x4_t signA_s_32x4 = vmovl_s16(vget_low_s16(sign_s_16x8));
				const int32x4_t signB_s_32x4 = vmovl_s16(vget_high_s16(sign_s_16x8));

				const int32x4_t sqrDeltaA_s_32x4 = vmull_s16(vget_low_s16(delta_s_16x8), vget_low_s16(delta_s_16x8));
				const int32x4_t sqrDeltaB_s_32x4 = vmull_s16(vget_high_s16(delta_s_16x8), vget_high_s16(delta_s_16x8));

				// sign * delta^2 * 32 / normalizedSqrResidual
				const int32x4_t sqrResponseA_s_32x4 = vcvtq_s32_f32(vmulq_f32(vcvtq_f32_s32(vmulq_s32(signA_s_32x4, vshlq_n_s32(sqrDeltaA_s_32x4, 5))), invNormalizedSqrResidualA_f_32x4));
				const int32x4_t sqrResponseB_s_32x4 = vcvtq_s32_f32(vmulq_f32(vcvtq_f32_s32(vmulq_s32(signB_s_32x4, vshlq_n_s32(sqrDeltaB_s_32x4, 5))), invNormalizedSqrResidualB_f_32x4));

				const int16x8_t sqrResponse_s_16x8 = vcombine_s16(vqmovn_s32(sqrResponseA_s_32x4), vqmovn_s32(sqrResponseB_s_32x4));

				vst1q_s16(sqrResponsesIntermediate + x, sqrResponse_s_16x8); // we store the result in the intermediate ring buffer


				// handle the last sum row(s)

				rowSumTop_u_8x8 = vld1_u8(rowSumTop + x); // load remaining row to finalize sum values
				rowSumBottom_u_8x8 = vld1_u8(rowSumBottom + x);

				sumTop_u_16x8 = vsubw_u8(sumTop_u_16x8, rowSumTop_u_8x8); // add next top row
				sumBottom_u_16x8 = vsubw_u8(sumBottom_u_16x8, rowSumBottom_u_8x8); // add next bottom row

				rowSqrSumTop_u_16x8 = vmull_u8(rowSumTop_u_8x8, rowSumTop_u_8x8); // subtract next top sqr row
				sqrSumTopA_u_32x4 = vsubw_u16(sqrSumTopA_u_32x4, vget_low_u16(rowSqrSumTop_u_16x8));
				sqrSumTopB_u_32x4 = vsubw_u16(sqrSumTopB_u_32x4, vget_high_u16(rowSqrSumTop_u_16x8));

				rowSqrSumBottom_u_16x8 = vmull_u8(rowSumBottom_u_8x8, rowSumBottom_u_8x8); // subtract next bottom sqr row
				sqrSumBottomA_u_32x4 = vsubw_u16(sqrSumBottomA_u_32x4, vget_low_u16(rowSqrSumBottom_u_16x8));
				sqrSumBottomB_u_32x4 = vsubw_u16(sqrSumBottomB_u_32x4, vget_high_u16(rowSqrSumBottom_u_16x8));

				vst1q_u16(windowSumTop + x, sumTop_u_16x8);
				vst1q_u16(windowSumBottom + x, sumBottom_u_16x8);

				vst1q_u32(windowSqrSumTop + x + 0, sqrSumTopA_u_32x4);
				vst1q_u32(windowSqrSumTop + x + 4, sqrSumTopB_u_32x4);

				vst1q_u32(windowSqrSumBottom + x + 0, sqrSumBottomA_u_32x4);
				vst1q_u32(windowSqrSumBottom + x + 4, sqrSumBottomB_u_32x4);
			}
		}
		else
		{
			// this is the last row in which we do not determine valid responses anymore
			memset(sqrResponsesIntermediate, 0x00, sizeof(int16_t) * width);
		}

		if (responseRingBufferIndex >= 1u)
		{
			// we have at seen two rows (and a zero response row), so that we can apply the non maximum supression

			int16_t* sqrResponses = responses + (y - 1u) * width; // the response will be for the previous row

			const int16_t* responseTop = responseRingBufferMemory.data<int16_t>() + ((responseRingBufferIndex + 1u) % 3u) * width;
			const int16_t* responseCenter = responseRingBufferMemory.data<int16_t>() + ((responseRingBufferIndex + 2u) % 3u) * width;
			const int16_t* responseBottom = responseRingBufferMemory.data<int16_t>() + (responseRingBufferIndex % 3u) * width;

			for (unsigned int x = 0u; x < width; x += 8u)
			{
				if (x + 8u > width)
				{
					// the last iteration will not fit into the data,
					// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

					ocean_assert(x >= 8u && width > 8u);
					const unsigned int newX = width - 8u;

					ocean_assert(x > newX);

					x = newX;

					// the for loop will stop after this iteration
					ocean_assert(x + 8u == width);
				}

				const int16x8_t responsesTop_s_16x8 = vld1q_s16(responseTop + x);
				const int16x8_t responsesCenter_s_16x8 = vld1q_s16(responseCenter + x);
				const int16x8_t responsesBottom_s_16x8 = vld1q_s16(responseBottom + x);

				// mask = (center > 0 && (center <= top || center < bottom)) || (center < 0 && (center >= top || center > bottom) ? 0xFFFF : 0x0000

				const uint16x8_t centerGreaterZero_u_16x8 = vandq_u16(vcgtq_s16(responsesCenter_s_16x8, zero_s_16x8), vorrq_u16(vcleq_s16(responsesCenter_s_16x8, responsesTop_s_16x8), vcltq_s16(responsesCenter_s_16x8, responsesBottom_s_16x8)));
				const uint16x8_t centerLessZero_u_16x8 = vandq_u16(vcltq_s16(responsesCenter_s_16x8, zero_s_16x8), vorrq_u16(vcgeq_s16(responsesCenter_s_16x8, responsesTop_s_16x8), vcgtq_s16(responsesCenter_s_16x8, responsesBottom_s_16x8)));
				const uint16x8_t mask_u_16x8 = vorrq_u16(centerGreaterZero_u_16x8, centerLessZero_u_16x8);

				const int16x8_t nonMaximumSuppressionResponse_s_16x8 = vandq_s16(responsesCenter_s_16x8, vreinterpretq_s16_u16(vmvnq_u16(mask_u_16x8))); // vmvnq_u16: 0xFFFF <-> 0x0000

				vst1q_s16(sqrResponses + x, nonMaximumSuppressionResponse_s_16x8);
			}
		}

		++responseRingBufferIndex;
	}

	// we set the last response rows to zero
	memset(responses + width * (height - window_ - stepSize_2), 0x00, width * (window_ + stepSize_2) * sizeof(int16_t));

	return true;

#else // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	OCEAN_SUPPRESS_UNUSED_WARNING(frame);
	OCEAN_SUPPRESS_UNUSED_WARNING(width);
	OCEAN_SUPPRESS_UNUSED_WARNING(height);
	OCEAN_SUPPRESS_UNUSED_WARNING(responses);
	OCEAN_SUPPRESS_UNUSED_WARNING(paddingElements);

	return false;

#endif

}

bool LineDetectorULF::RMSStepEdgeDetectorI::hasInvokeHorizontal(const unsigned int width, const unsigned int height) const
{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	return width >= 8u && height >= window_ * 2u + stepSize_;

#else

	OCEAN_SUPPRESS_UNUSED_WARNING(width);
	OCEAN_SUPPRESS_UNUSED_WARNING(height);

	return false;

#endif
}

unsigned int LineDetectorULF::RMSStepEdgeDetectorI::adjustThreshold(const unsigned int threshold) const
{
	return staticAdjustThreshold(threshold);
}

#ifdef WE_KEEP_THIS_IMPLEMENTATION_FOR_COMPARISON // 32bit sum values

void LineDetectorULF::RMSStepEdgeDetectorI::invokeRowVertical(const uint8_t* const row, const unsigned int width, const unsigned int window, const uint32_t* const windowSums, const uint32_t* const windowSqrSums, int16_t* sqrResponses)
{
	ocean_assert(row != nullptr);
	ocean_assert(width != 0u);
	ocean_assert(window >= 1u && window * 2u < width);

	ocean_assert(windowSums != nullptr && windowSqrSums != nullptr);
	ocean_assert(sqrResponses != nullptr);

	constexpr unsigned int stepSize = 1u;
	constexpr unsigned int stepSize_2 = stepSize / 2u; // 0 in case 'stepSize == 1'

	const unsigned int sqrWindow2 = window * window * 2u;

	const unsigned int elements = width - (window + stepSize_2) * 2u;

	const uint32_t* windowSumsL = windowSums;
	const uint32_t* windowSumsR = windowSums + window + stepSize;

	const uint32_t* windowSqrSumsL = windowSqrSums;
	const uint32_t* windowSqrSumsR = windowSqrSums + window + stepSize;

	// clear left border
	memset(sqrResponses, 0, (window + stepSize_2) * sizeof(int16_t));

	sqrResponses += window + stepSize_2;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	ocean_assert(elements >= 8u);

	const uint32x4_t window_u_32x4 = vdupq_n_u32(window);
	const uint32x4_t sqrWindow2_u_32x4 = vdupq_n_u32(sqrWindow2);

	const int32x4_t zero_s_32x4 = vdupq_n_s32(0);
	const int32x4_t one_s_32x4 = vdupq_n_s32(1);
	const int32x4_t minusone_s_32x4 = vdupq_n_s32(-1);

	for (unsigned int nElement = 0u; nElement < elements; nElement += 8u)
	{
		if (nElement + 8u > elements)
		{
			// the last iteration will not fit into the data,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(nElement >= 8u && elements > 8u);
			const unsigned int newNElement = elements - 8u;

			ocean_assert(nElement > newNElement);
			const unsigned int offset = nElement - newNElement;

			windowSumsL -= offset;
			windowSumsR -= offset;

			windowSqrSumsL -= offset;
			windowSqrSumsR -= offset;

			sqrResponses -= offset;

			// the for loop will stop after this iteration
			ocean_assert(!(nElement + 8u < elements));

			/* nElement = newNElement; update not necessary */
		}

		const uint32x4_t sumLA_u_32x4 = vld1q_u32(windowSumsL + 0);
		const uint32x4_t sumLB_u_32x4 = vld1q_u32(windowSumsL + 4);

		const uint32x4_t sumRA_u_32x4 = vld1q_u32(windowSumsR + 0);
		const uint32x4_t sumRB_u_32x4 = vld1q_u32(windowSumsR + 4);

		const uint32x4_t sqrSumLA_u_32x4 = vld1q_u32(windowSqrSumsL + 0);
		const uint32x4_t sqrSumLB_u_32x4 = vld1q_u32(windowSqrSumsL + 4);

		const uint32x4_t sqrSumRA_u_32x4 = vld1q_u32(windowSqrSumsR + 0);
		const uint32x4_t sqrSumRB_u_32x4 = vld1q_u32(windowSqrSumsR + 4);

		// normalizedSqrResidualL = window * *windowSqrSumsL - sqr(*windowSumsL);
		const uint32x4_t normalizedSqrResidualLA_u_32x4 = vsubq_u32(vmulq_u32(window_u_32x4, sqrSumLA_u_32x4), vmulq_u32(sumLA_u_32x4, sumLA_u_32x4));
		const uint32x4_t normalizedSqrResidualLB_u_32x4 = vsubq_u32(vmulq_u32(window_u_32x4, sqrSumLB_u_32x4), vmulq_u32(sumLB_u_32x4, sumLB_u_32x4));

		const uint32x4_t normalizedSqrResidualRA_u_32x4 = vsubq_u32(vmulq_u32(window_u_32x4, sqrSumRA_u_32x4), vmulq_u32(sumRA_u_32x4, sumRA_u_32x4));
		const uint32x4_t normalizedSqrResidualRB_u_32x4 = vsubq_u32(vmulq_u32(window_u_32x4, sqrSumRB_u_32x4), vmulq_u32(sumRB_u_32x4, sumRB_u_32x4));

		const float32x4_t normalizedSqrResidualA_f_32x4 = vcvtq_f32_u32(vmaxq_u32(sqrWindow2_u_32x4, vaddq_u32(normalizedSqrResidualLA_u_32x4, normalizedSqrResidualRA_u_32x4)));
		const float32x4_t normalizedSqrResidualB_f_32x4 = vcvtq_f32_u32(vmaxq_u32(sqrWindow2_u_32x4, vaddq_u32(normalizedSqrResidualLB_u_32x4, normalizedSqrResidualRB_u_32x4)));

		// we calculate the (approximated) inverse of normalizedSqrResidual: 1 / normalizedSqrResidual
		float32x4_t invNormalizedSqrResidualA_f_32x4 = vrecpeq_f32(normalizedSqrResidualA_f_32x4);
		invNormalizedSqrResidualA_f_32x4 = vmulq_f32(vrecpsq_f32(normalizedSqrResidualA_f_32x4, invNormalizedSqrResidualA_f_32x4), invNormalizedSqrResidualA_f_32x4); // improving the accuracy of the approx. inverse by Newton/Raphson

		float32x4_t invNormalizedSqrResidualB_f_32x4 = vrecpeq_f32(normalizedSqrResidualB_f_32x4);
		invNormalizedSqrResidualB_f_32x4 = vmulq_f32(vrecpsq_f32(normalizedSqrResidualB_f_32x4, invNormalizedSqrResidualB_f_32x4), invNormalizedSqrResidualB_f_32x4);

		// delta = *windowSumsL - *windowSumsR;
		const int32x4_t deltaA_s_32x4 = vsubq_s32(vreinterpretq_s32_u32(sumLA_u_32x4), vreinterpretq_s32_u32(sumRA_u_32x4));
		const int32x4_t deltaB_s_32x4 = vsubq_s32(vreinterpretq_s32_u32(sumLB_u_32x4), vreinterpretq_s32_u32(sumRB_u_32x4));

		// valueCenterSmallerZero = valueCenter < 0 ? 0xFFFFFFFF : 0x00000000
		const uint32x4_t deltaSmallerZeroA_u_32x4 = vcltq_s32(deltaA_s_32x4, zero_s_32x4);
		const uint32x4_t deltaSmallerZeroB_u_32x4 = vcltq_s32(deltaB_s_32x4, zero_s_32x4);

		// sign = valueCenterMinusZero == 0xFFFFFFFF ? -1 : 1
		const int32x4_t signA_s_32x4 = vbslq_u32(deltaSmallerZeroA_u_32x4, minusone_s_32x4, one_s_32x4);
		const int32x4_t signB_s_32x4 = vbslq_u32(deltaSmallerZeroB_u_32x4, minusone_s_32x4, one_s_32x4);

		// sign * delta^2 * 32 / normalizedSqrResidual
		const int32x4_t sqrResponseA_s_32x4 = vcvtq_s32_f32(vmulq_f32(vcvtq_f32_s32(vmulq_s32(signA_s_32x4, vshlq_n_s32(vmulq_s32(deltaA_s_32x4, deltaA_s_32x4), 5))), invNormalizedSqrResidualA_f_32x4));
		const int32x4_t sqrResponseB_s_32x4 = vcvtq_s32_f32(vmulq_f32(vcvtq_f32_s32(vmulq_s32(signB_s_32x4, vshlq_n_s32(vmulq_s32(deltaB_s_32x4, deltaB_s_32x4), 5))), invNormalizedSqrResidualB_f_32x4));

		const int16x8_t sqrResponse_s_16x8 = vcombine_s16(vqmovn_s32(sqrResponseA_s_32x4), vqmovn_s32(sqrResponseB_s_32x4));

		vst1q_s16(sqrResponses, sqrResponse_s_16x8);

		windowSumsL += 8;
		windowSumsR += 8;

		windowSqrSumsL += 8;
		windowSqrSumsR += 8;

		sqrResponses += 8;
	}

	// clear right border
	memset(sqrResponses, 0, (window + stepSize_2) * sizeof(int16_t));

	const int16x8_t zero_s_16x8 = vdupq_n_s16(0);

	sqrResponses -= elements;

	ocean_assert(elements >= 8u);

	// the non maximum suppression must not set a response immediately, as this result can have an impact on the following/neighboring suppression iteration
	// therefore, we store an intermediate response value which we will update one iteration later
	int16x8_t newPreviousNonMaximumSuppressionResponse_s_16x8 = vld1q_s16(sqrResponses);
	int16_t* previousSqrResponse = sqrResponses;

	for (unsigned int nElement = 0u; nElement < elements; nElement += 8u)
	{
		if (nElement + 8u > elements)
		{
			// the last iteration will not fit into the data,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(nElement >= 8u && elements > 8u);
			const unsigned int newNElement = elements - 8u;

			ocean_assert(nElement > newNElement);
			const unsigned int offset = nElement - newNElement;

			sqrResponses -= offset;

			// the for loop will stop after this iteration
			ocean_assert(!(nElement + 8u < elements));

			/* nElement = newNElement; update not necessary */
		}

		const int16x8_t responsesLeft_s_16x8 = vld1q_s16(sqrResponses - 1);
		const int16x8_t responsesCenter_s_16x8 = vld1q_s16(sqrResponses + 0);
		const int16x8_t responsesRight_s_16x8 = vld1q_s16(sqrResponses + 1);

		// mask = (center > 0 && (center <= left || center < right)) || (center < 0 && (center >= left || center > right) ? 0xFFFF : 0x0000

		const uint16x8_t centerGreaterZero_u_16x8 = vandq_u16(vcgtq_s16(responsesCenter_s_16x8, zero_s_16x8), vorrq_u16(vcleq_s16(responsesCenter_s_16x8, responsesLeft_s_16x8), vcltq_s16(responsesCenter_s_16x8, responsesRight_s_16x8)));
		const uint16x8_t centerLessZero_u_16x8 = vandq_u16(vcltq_s16(responsesCenter_s_16x8, zero_s_16x8), vorrq_u16(vcgeq_s16(responsesCenter_s_16x8, responsesLeft_s_16x8), vcgtq_s16(responsesCenter_s_16x8, responsesRight_s_16x8)));
		const uint16x8_t mask_u_16x8 = vorrq_u16(centerGreaterZero_u_16x8, centerLessZero_u_16x8);

		vst1q_s16(previousSqrResponse, newPreviousNonMaximumSuppressionResponse_s_16x8);

		newPreviousNonMaximumSuppressionResponse_s_16x8 = vandq_s16(responsesCenter_s_16x8, vreinterpretq_s16_u16(vmvnq_u16(mask_u_16x8))); // vmvnq_u16: 0xFFFF <-> 0x0000

		previousSqrResponse = sqrResponses;
		sqrResponses += 8;
	}

	vst1q_s16(previousSqrResponse, newPreviousNonMaximumSuppressionResponse_s_16x8);

#else // OCEAN_HARDWARE_NEON_VERSION >= 10

	const int16_t* const responsesEnd = sqrResponses + elements;

	while (sqrResponses != responsesEnd)
	{
		ocean_assert(sqrResponses < responsesEnd);

		ocean_assert(windowSumsL < windowSums + width - (window - 1u));
		ocean_assert(windowSumsR < windowSums + width - (window - 1u));
		ocean_assert(windowSqrSumsL < windowSqrSums + width - (window - 1u));
		ocean_assert(windowSqrSumsR < windowSqrSums + width - (window - 1u));

		// we determine the root mean square residual:

		// mean = 1/n * sum(yi)

		// rms = sqrt(1/n * sum[(mean - yi)^2])
		//     = sqrt(1/n * sum[mean^2 - 2 * mean * yi + yi^2])
		//     = sqrt(1/n * (n * mean^2 - 2 * n * mean^2 + sum[yi^2]))
		//     = sqrt(mean^2 - 2 * mean^2 + 1/n * sum[yi^2])
		//     = sqrt(1/n * sum[yi^2] - mean^2)
		//     = sqrt(1/n * sum[yi^2] - 1/n^2 * sum[yi]^2)
		//     = 1/n * sqrt(n * sum[yi^2] - sum[yi]^2)

		// due to performance reasons we avoid sqrt and keep the squared residual result

		//       rms^2 = 1/n^2 * (n * sum[yi^2] - sum[yi]^2)
		// n^2 * rms^2 =          n * sum[yi^2] - sum[yi]^2

		ocean_assert(window * *windowSqrSumsL >= *windowSumsL);
		ocean_assert(window * *windowSqrSumsR >= *windowSumsR);

		// normalized residual (each multiplied with window)
		const uint32_t normalizedSqrResidualL = window * *windowSqrSumsL - sqr(*windowSumsL);
		const uint32_t normalizedSqrResidualR = window * *windowSqrSumsR - sqr(*windowSumsR);

		// assume minimum residual of 1.0 (* window) to avoid large responses in saturated areas, and due to jpg artifacts
		// as we have 2 * residual^2, we assume a minimum value of 2 * window^2
		const uint32_t normalizedSqrResidual = max(sqrWindow2, normalizedSqrResidualL + normalizedSqrResidualR);

		const int normalizedDelta = *windowSumsL - *windowSumsR; // delta * window

		const int response = NumericT<int>::sign(normalizedDelta) * int((sqr(normalizedDelta) * 32u + normalizedSqrResidual / 2u) / normalizedSqrResidual); // 32 = 2u * 4u * 4u, 2 for `normalizedSqrResidual`, 4 as scaling parameter to get reasonable integer values

		*sqrResponses = minmax<int>(NumericT<int16_t>::minValue(), response, NumericT<int16_t>::maxValue());

		++sqrResponses;

		++windowSumsL;
		++windowSumsR;

		++windowSqrSumsL;
		++windowSqrSumsR;
	}

	// clear right border
	memset(sqrResponses, 0, (window + stepSize_2) * sizeof(int16_t));

	sqrResponses -= elements + window + stepSize_2;

	// separate pass for non-max suppression -- slow, how to speed up?

	// the non maximum suppression must not set a response immediately, as this result can have an impact on the following/neighboring suppression iteration
	// therefore, we store an intermediate response value which we will update one iteration later
	int16_t newPreviousSqrResponsesValue = 0;
	ocean_assert(sqrResponses[window + stepSize_2 - 1] == 0);

	for (unsigned int x = window + stepSize_2; x < width - window - stepSize_2; ++x)
	{
		const int16_t left = sqrResponses[x - 1];
		const int16_t center = sqrResponses[x];
		const int16_t right = sqrResponses[x + 1];

		sqrResponses[x - 1] = newPreviousSqrResponsesValue;

		if ((center > 0 && (center <= left || center < right)) || (center < 0 && (center >= left || center > right)))
		{
			newPreviousSqrResponsesValue = 0;
		}
		else
		{
			newPreviousSqrResponsesValue = sqrResponses[x];
		}
	}

	sqrResponses[width - window - stepSize_2 - 1u] = newPreviousSqrResponsesValue;

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

}

#endif // WE_KEEP_THIS_IMPLEMENTATION_FOR_COMPARISON

void LineDetectorULF::RMSStepEdgeDetectorI::invokeRowVertical(const uint8_t* const row, const unsigned int width, const unsigned int window, const uint16_t* const windowSums, const uint32_t* const windowSqrSums, int16_t* sqrResponses)
{
	ocean_assert_and_suppress_unused(row != nullptr, row);
	ocean_assert(width != 0u);
	ocean_assert(window >= 1u && window * 2u < width && window <= 8u);

	ocean_assert(windowSums != nullptr && windowSqrSums != nullptr);
	ocean_assert(sqrResponses != nullptr);

	constexpr unsigned int stepSize_2 = stepSize_ / 2u; // 0 in case 'stepSize == 1'

	const unsigned int sqrWindow2 = window * window * 2u;

	const unsigned int elements = width - (window + stepSize_2) * 2u;

	const uint16_t* windowSumsL = windowSums;
	const uint16_t* windowSumsR = windowSums + window + stepSize_;

	const uint32_t* windowSqrSumsL = windowSqrSums;
	const uint32_t* windowSqrSumsR = windowSqrSums + window + stepSize_;

	// clear left border
	memset(sqrResponses, 0, (window + stepSize_2) * sizeof(int16_t));

	sqrResponses += window + stepSize_2;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	ocean_assert(elements >= 8u);

	const uint32x4_t window_u_32x4 = vdupq_n_u32(window);
	const uint32x4_t sqrWindow2_u_32x4 = vdupq_n_u32(sqrWindow2);

	const int16x8_t zero_s_16x8 = vdupq_n_s16(0);
	const int16x8_t one_s_16x8 = vdupq_n_s16(1);
	const int16x8_t minusone_s_16x8 = vdupq_n_s16(-1);

	for (unsigned int nElement = 0u; nElement < elements; nElement += 8u)
	{
		if (nElement + 8u > elements)
		{
			// the last iteration will not fit into the data,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(nElement >= 8u && elements > 8u);
			const unsigned int newNElement = elements - 8u;

			ocean_assert(nElement > newNElement);
			const unsigned int offset = nElement - newNElement;

			windowSumsL -= offset;
			windowSumsR -= offset;

			windowSqrSumsL -= offset;
			windowSqrSumsR -= offset;

			sqrResponses -= offset;

			// the for loop will stop after this iteration
			ocean_assert(!(nElement + 8u < elements));

			/* nElement = newNElement; update not necessary */
		}

		const uint16x8_t sumL_u_16x8 = vld1q_u16(windowSumsL);
		const uint16x8_t sumR_u_16x8 = vld1q_u16(windowSumsR);

		const uint32x4_t sqrSumLA_u_32x4 = vld1q_u32(windowSqrSumsL + 0);
		const uint32x4_t sqrSumLB_u_32x4 = vld1q_u32(windowSqrSumsL + 4);

		const uint32x4_t sqrSumRA_u_32x4 = vld1q_u32(windowSqrSumsR + 0);
		const uint32x4_t sqrSumRB_u_32x4 = vld1q_u32(windowSqrSumsR + 4);

		// normalizedSqrResidualL = window * *windowSqrSumsL - sqr(*windowSumsL);
		const uint32x4_t normalizedSqrResidualLA_u_32x4 = vsubq_u32(vmulq_u32(window_u_32x4, sqrSumLA_u_32x4), vmull_u16(vget_low_u16(sumL_u_16x8), vget_low_u16(sumL_u_16x8)));
		const uint32x4_t normalizedSqrResidualLB_u_32x4 = vsubq_u32(vmulq_u32(window_u_32x4, sqrSumLB_u_32x4), vmull_u16(vget_high_u16(sumL_u_16x8), vget_high_u16(sumL_u_16x8)));

		const uint32x4_t normalizedSqrResidualRA_u_32x4 = vsubq_u32(vmulq_u32(window_u_32x4, sqrSumRA_u_32x4), vmull_u16(vget_low_u16(sumR_u_16x8), vget_low_u16(sumR_u_16x8)));
		const uint32x4_t normalizedSqrResidualRB_u_32x4 = vsubq_u32(vmulq_u32(window_u_32x4, sqrSumRB_u_32x4), vmull_u16(vget_high_u16(sumR_u_16x8), vget_high_u16(sumR_u_16x8)));

		const float32x4_t normalizedSqrResidualA_f_32x4 = vcvtq_f32_u32(vmaxq_u32(sqrWindow2_u_32x4, vaddq_u32(normalizedSqrResidualLA_u_32x4, normalizedSqrResidualRA_u_32x4)));
		const float32x4_t normalizedSqrResidualB_f_32x4 = vcvtq_f32_u32(vmaxq_u32(sqrWindow2_u_32x4, vaddq_u32(normalizedSqrResidualLB_u_32x4, normalizedSqrResidualRB_u_32x4)));

		// we calculate the (approximated) inverse of normalizedSqrResidual: 1 / normalizedSqrResidual
		float32x4_t invNormalizedSqrResidualA_f_32x4 = vrecpeq_f32(normalizedSqrResidualA_f_32x4);
		invNormalizedSqrResidualA_f_32x4 = vmulq_f32(vrecpsq_f32(normalizedSqrResidualA_f_32x4, invNormalizedSqrResidualA_f_32x4), invNormalizedSqrResidualA_f_32x4); // improving the accuracy of the approx. inverse by Newton/Raphson

		float32x4_t invNormalizedSqrResidualB_f_32x4 = vrecpeq_f32(normalizedSqrResidualB_f_32x4);
		invNormalizedSqrResidualB_f_32x4 = vmulq_f32(vrecpsq_f32(normalizedSqrResidualB_f_32x4, invNormalizedSqrResidualB_f_32x4), invNormalizedSqrResidualB_f_32x4);

		// delta = *windowSumsL - *windowSumsR;
		const int16x8_t delta_s_16x8 = vsubq_s16(vreinterpretq_s16_u16(sumL_u_16x8), vreinterpretq_s16_u16(sumR_u_16x8));

		// valueCenterSmallerZero = valueCenter < 0 ? 0xFFFFFFFF : 0x00000000
		const uint16x8_t deltaSmallerZero_u_16x8 = vcltq_s16(delta_s_16x8, zero_s_16x8);

		// sign = valueCenterMinusZero == 0xFFFFFFFF ? -1 : 1
		const int16x8_t sign_s_16x8 = vbslq_s16(deltaSmallerZero_u_16x8, minusone_s_16x8, one_s_16x8);

		const int32x4_t signA_s_32x4 = vmovl_s16(vget_low_s16(sign_s_16x8));
		const int32x4_t signB_s_32x4 = vmovl_s16(vget_high_s16(sign_s_16x8));

		const int32x4_t sqrDeltaA_s_32x4 = vmull_s16(vget_low_s16(delta_s_16x8), vget_low_s16(delta_s_16x8));
		const int32x4_t sqrDeltaB_s_32x4 = vmull_s16(vget_high_s16(delta_s_16x8), vget_high_s16(delta_s_16x8));

		// sign * delta^2 * 32 / normalizedSqrResidual
		const int32x4_t sqrResponseA_s_32x4 = vcvtq_s32_f32(vmulq_f32(vcvtq_f32_s32(vmulq_s32(signA_s_32x4, vshlq_n_s32(sqrDeltaA_s_32x4, 5))), invNormalizedSqrResidualA_f_32x4));
		const int32x4_t sqrResponseB_s_32x4 = vcvtq_s32_f32(vmulq_f32(vcvtq_f32_s32(vmulq_s32(signB_s_32x4, vshlq_n_s32(sqrDeltaB_s_32x4, 5))), invNormalizedSqrResidualB_f_32x4));

		const int16x8_t sqrResponse_s_16x8 = vcombine_s16(vqmovn_s32(sqrResponseA_s_32x4), vqmovn_s32(sqrResponseB_s_32x4));

		vst1q_s16(sqrResponses, sqrResponse_s_16x8);

		windowSumsL += 8;
		windowSumsR += 8;

		windowSqrSumsL += 8;
		windowSqrSumsR += 8;

		sqrResponses += 8;
	}

	// clear right border
	memset(sqrResponses, 0, (window + stepSize_2) * sizeof(int16_t));

	sqrResponses -= elements;

	ocean_assert(elements >= 8u);

	// the non maximum suppression must not set a response immediately, as this result can have an impact on the following/neighboring suppression iteration
	// therefore, we store an intermediate response value which we will update one iteration later
	int16x8_t newPreviousNonMaximumSuppressionResponse_s_16x8 = vld1q_s16(sqrResponses);
	int16_t* previousSqrResponse = sqrResponses;

	for (unsigned int nElement = 0u; nElement < elements; nElement += 8u)
	{
		if (nElement + 8u > elements)
		{
			// the last iteration will not fit into the data,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(nElement >= 8u && elements > 8u);
			const unsigned int newNElement = elements - 8u;

			ocean_assert(nElement > newNElement);
			const unsigned int offset = nElement - newNElement;

			sqrResponses -= offset;

			// the for loop will stop after this iteration
			ocean_assert(!(nElement + 8u < elements));

			/* nElement = newNElement; update not necessary */
		}

		const int16x8_t responsesLeft_s_16x8 = vld1q_s16(sqrResponses - 1);
		const int16x8_t responsesCenter_s_16x8 = vld1q_s16(sqrResponses + 0);
		const int16x8_t responsesRight_s_16x8 = vld1q_s16(sqrResponses + 1);

		// mask = (center > 0 && (center <= left || center < right)) || (center < 0 && (center >= left || center > right) ? 0xFFFF : 0x0000

		const uint16x8_t centerGreaterZero_u_16x8 = vandq_u16(vcgtq_s16(responsesCenter_s_16x8, zero_s_16x8), vorrq_u16(vcleq_s16(responsesCenter_s_16x8, responsesLeft_s_16x8), vcltq_s16(responsesCenter_s_16x8, responsesRight_s_16x8)));
		const uint16x8_t centerLessZero_u_16x8 = vandq_u16(vcltq_s16(responsesCenter_s_16x8, zero_s_16x8), vorrq_u16(vcgeq_s16(responsesCenter_s_16x8, responsesLeft_s_16x8), vcgtq_s16(responsesCenter_s_16x8, responsesRight_s_16x8)));
		const uint16x8_t mask_u_16x8 = vorrq_u16(centerGreaterZero_u_16x8, centerLessZero_u_16x8);

		vst1q_s16(previousSqrResponse, newPreviousNonMaximumSuppressionResponse_s_16x8);

		newPreviousNonMaximumSuppressionResponse_s_16x8 = vandq_s16(responsesCenter_s_16x8, vreinterpretq_s16_u16(vmvnq_u16(mask_u_16x8))); // vmvnq_u16: 0xFFFF <-> 0x0000

		previousSqrResponse = sqrResponses;
		sqrResponses += 8;
	}

	vst1q_s16(previousSqrResponse, newPreviousNonMaximumSuppressionResponse_s_16x8);

#else // OCEAN_HARDWARE_NEON_VERSION >= 10

	const int16_t* const responsesEnd = sqrResponses + elements;

	while (sqrResponses != responsesEnd)
	{
		ocean_assert(sqrResponses < responsesEnd);

		ocean_assert(windowSumsL < windowSums + width - (window - 1u));
		ocean_assert(windowSumsR < windowSums + width - (window - 1u));
		ocean_assert(windowSqrSumsL < windowSqrSums + width - (window - 1u));
		ocean_assert(windowSqrSumsR < windowSqrSums + width - (window - 1u));

		// we determine the root mean square residual:

		// mean = 1/n * sum(yi)

		// rms = sqrt(1/n * sum[(mean - yi)^2])
		//     = sqrt(1/n * sum[mean^2 - 2 * mean * yi + yi^2])
		//     = sqrt(1/n * (n * mean^2 - 2 * n * mean^2 + sum[yi^2]))
		//     = sqrt(mean^2 - 2 * mean^2 + 1/n * sum[yi^2])
		//     = sqrt(1/n * sum[yi^2] - mean^2)
		//     = sqrt(1/n * sum[yi^2] - 1/n^2 * sum[yi]^2)
		//     = 1/n * sqrt(n * sum[yi^2] - sum[yi]^2)

		// due to performance reasons we avoid sqrt and keep the squared residual result

		//       rms^2 = 1/n^2 * (n * sum[yi^2] - sum[yi]^2)
		// n^2 * rms^2 =          n * sum[yi^2] - sum[yi]^2

		ocean_assert(window * *windowSqrSumsL >= *windowSumsL);
		ocean_assert(window * *windowSqrSumsR >= *windowSumsR);

		// normalized residual (each multiplied with window)
		const uint32_t normalizedSqrResidualL = window * *windowSqrSumsL - sqr(*windowSumsL);
		const uint32_t normalizedSqrResidualR = window * *windowSqrSumsR - sqr(*windowSumsR);

		// assume minimum residual of 1.0 (* window) to avoid large responses in saturated areas, and due to jpg artifacts
		// as we have 2 * residual^2, we assume a minimum value of 2 * window^2
		const uint32_t normalizedSqrResidual = max(sqrWindow2, normalizedSqrResidualL + normalizedSqrResidualR);

		const int normalizedDelta = *windowSumsL - *windowSumsR; // delta * window

		const int response = NumericT<int>::sign(normalizedDelta) * int((sqr(normalizedDelta) * 32u + normalizedSqrResidual / 2u) / normalizedSqrResidual); // 32 = 2u * 4u * 4u, 2 for `normalizedSqrResidual`, 4 as scaling parameter to get reasonable integer values

		*sqrResponses = int16_t(minmax<int>(NumericT<int16_t>::minValue(), response, NumericT<int16_t>::maxValue()));

		++sqrResponses;

		++windowSumsL;
		++windowSumsR;

		++windowSqrSumsL;
		++windowSqrSumsR;
	}

	// clear right border
	memset(sqrResponses, 0, (window + stepSize_2) * sizeof(int16_t));

	sqrResponses -= elements + window + stepSize_2;

	// separate pass for non-max suppression -- slow, how to speed up?

	// the non maximum suppression must not set a response immediately, as this result can have an impact on the following/neighboring suppression iteration
	// therefore, we store an intermediate response value which we will update one iteration later
	int16_t newPreviousSqrResponsesValue = 0;
	ocean_assert(sqrResponses[window + stepSize_2 - 1] == 0);

	for (unsigned int x = window + stepSize_2; x < width - window - stepSize_2; ++x)
	{
		const int16_t left = sqrResponses[x - 1];
		const int16_t center = sqrResponses[x];
		const int16_t right = sqrResponses[x + 1];

		sqrResponses[x - 1] = newPreviousSqrResponsesValue;

		if ((center > 0 && (center <= left || center < right)) || (center < 0 && (center >= left || center > right)))
		{
			newPreviousSqrResponsesValue = 0;
		}
		else
		{
			newPreviousSqrResponsesValue = sqrResponses[x];
		}
	}

	sqrResponses[width - window - stepSize_2 - 1u] = newPreviousSqrResponsesValue;

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

}

LineDetectorULF::RMSBarEdgeDetectorF::RMSBarEdgeDetectorF(const unsigned int window, const unsigned int minimalDelta) :
	EdgeDetector(window, ET_BAR),
	minimalDelta_(minimalDelta)
{
	ocean_assert(window_ >= 1u);
	ocean_assert(minimalDelta < 256u);
}

void LineDetectorULF::RMSBarEdgeDetectorF::invokeVertical(const uint8_t* frame, const unsigned int width, const unsigned int height, int16_t* responses, const unsigned int paddingElements) const
{
	ocean_assert(frame != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(responses != nullptr);

	ocean_assert(window_ >= 1u && window_ < width);

	const unsigned int frameStrideElements = width + paddingElements;

	Memory windowSumsMemory = Memory::create<uint32_t>(width - (window_ - 1u));
	Memory windowSqrSumsMemory = Memory::create<uint32_t>(width - (window_ - 1u));

	uint32_t* const windowSums = windowSumsMemory.data<uint32_t>();
	uint32_t* const windowSqrSums = windowSqrSumsMemory.data<uint32_t>();

	int16_t* rowResponses = responses;

	for (unsigned int y = 0u; y < height; ++y)
	{
		determineRowSums(frame, width, window_, windowSums, windowSqrSums);

		invokeRowVertical(frame, width, window_, minimalDelta_, windowSums, windowSqrSums, rowResponses);

		frame += frameStrideElements;
		rowResponses += width;
	}
}

unsigned int LineDetectorULF::RMSBarEdgeDetectorF::adjustThreshold(const unsigned int threshold) const
{
	return staticAdjustThreshold(threshold);
}

void LineDetectorULF::RMSBarEdgeDetectorF::invokeRowVertical(const uint8_t* const row, const unsigned int width, const unsigned int window, const unsigned int minimalDelta, const uint32_t* const windowSums, const uint32_t* const windowSqrSums, int16_t* responses)
{
	const unsigned int w1 = window + 1;
	const double ww = 2 * window;

	for (unsigned int x = w1; x < width - w1; x++)
	{
		int sumL = windowSums[x - w1]; // sum of rowi[x-w-1]...rowi[x-2]
		int sumR = windowSums[x + 2];  // sum of rowi[x+2]...rowi[x+w+1]
		int sumSqL = windowSqrSums[x - w1];
		int sumSqR = windowSqrSums[x + 2];

		const double avgL = double(sumL) / double(window);
		const double avgR = double(sumR) / double(window);

		// compute average "background"" intensity
		double avg = (avgL + avgR) * 0.5;
		ocean_assert(NumericD::isWeakEqual(avg, double(sumL + sumR) / double(ww)));

		// we explicitly check whether both window values are higher or lesser than the center value
		const bool isValidBarEdge = (double(row[x]) < avgL && double(row[x]) < avgR) || (double(row[x]) > avgL && double(row[x]) > avgR);

		if (!isValidBarEdge)
		{
			responses[x] = 0;
			continue;
		}

		// compute avg abs residual / variance over side windows
		double res = 0;

		double sx = sumL + sumR;
		double sxx = sumSqL + sumSqR;
		res = std::sqrt((float)(ww * sxx - sx * sx)) / ww;

		double minres = 1; // (up.oldnew ? 2 : 1);
		res = max(minres, res); // assume minimum residual of 1.0 to avoid large responses in saturated areas, and due to jpg artifacts

		// now look at center 3 pixels
		// do NMS on center value
		double cen = row[x] - avg;
		double cenp = row[x + 1] - avg;
		double cenm = row[x - 1] - avg;
		int sign = 1;

		if (cen < 0)
		{
			// dark line
			sign = -1;
			cen = -cen;
			cenp = -cenp;
			cenm = -cenm;
		}

		cenp += 1e-4; // break symmetry
		// (if two neighboring values have the same response, pick the right one)

		cenp = max(cenm, cenp); // larger of the 2 neighbors
		if (cen < cenp)
		{
			// non-max suppression
			cen = 0;
		}
		else
		{
			cen = (cen + cenp) / 2.0; // average with larger neighbor
			//to be robust to aliasing
		}

		// final measure is ratio, retaining sign

		int r = NumericD::round32(sign * 16.0 * fabs(cen) / res);

		// ensuring that we have a meaningful delta
		// (intensity difference between surrounding and center pixel)
		if (std::abs(cen) < double(minimalDelta))
		{
			r = 0;
		}

		responses[x] = int16_t(minmax<int>(NumericT<int16_t>::minValue(), r, NumericT<int16_t>::maxValue()));
	}

	// clear borders
	for (unsigned int x = 0u; x < w1; x++)
	{
		responses[x] = 0;
		responses[width - 1u - x] = 0;
	}
}

LineDetectorULF::RMSStepEdgeDetectorF::RMSStepEdgeDetectorF(const unsigned int window) :
	EdgeDetector(window, ET_STEP)
{
	ocean_assert(window_ >= 1u);
}

void LineDetectorULF::RMSStepEdgeDetectorF::invokeVertical(const uint8_t* frame, const unsigned int width, const unsigned int height, int16_t* responses, const unsigned int paddingElements) const
{
	ocean_assert(frame != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(responses != nullptr);

	ocean_assert(window_ >= 1u && window_ < width);

	const unsigned int frameStrideElements = width + paddingElements;

	Memory windowSumsMemory = Memory::create<uint32_t>(width - (window_ - 1u));
	Memory windowSqrSumsMemory = Memory::create<uint32_t>(width - (window_ - 1u));

	uint32_t* const windowSums = windowSumsMemory.data<uint32_t>();
	uint32_t* const windowSqrSums = windowSqrSumsMemory.data<uint32_t>();

	int16_t* rowResponses = responses;

	for (unsigned int y = 0u; y < height; ++y)
	{
		determineRowSums(frame, width, window_, windowSums, windowSqrSums);

		invokeRowVertical(frame, width, window_, windowSums, windowSqrSums, rowResponses);

		frame += frameStrideElements;
		rowResponses += width;
	}
}

unsigned int LineDetectorULF::RMSStepEdgeDetectorF::adjustThreshold(const unsigned int threshold) const
{
	return staticAdjustThreshold(threshold);
}

void LineDetectorULF::RMSStepEdgeDetectorF::invokeRowVertical(const uint8_t* const /*row*/, const unsigned int width, const unsigned int window, const uint32_t* const windowSums, const uint32_t* const windowSqrSums, int16_t* responses)
{
	const double ww = window;

	Memory intermediateResponsesMemory = Memory::create<int16_t>(width);
	int16_t* intermediateResponses = intermediateResponsesMemory.data<int16_t>();

	for (unsigned int x = window; x < width - window; ++x)
	{
		const int sumL = windowSums[x - window]; // sum of rowi[x-w]...rowi[x-1]
		const int sumR = windowSums[x + 1u]; // sum of rowi[x+1]...rowi[x+w]
		const int sumSqL = windowSqrSums[x - window];
		const int sumSqR = windowSqrSums[x + 1u];

		// compute two averages
		const double avgL = (double)sumL / ww;
		const double avgR = (double)sumR / ww;

		// compute avg residuals
		double resL = 0;
		double resR = 0;
		double res = 0;

		resL = std::sqrt((float)(window * sumSqL - sumL * sumL));
		resR = std::sqrt((float)(window * sumSqR - sumR * sumR));

		res = (resL + resR)/ (2.0 * ww);

		// final measure is ratio of difference of the averages to avg residual
		double minres = 1;//(up.oldnew ? 2 : 1);
		res = max(minres, res); // assume minimum residual of 1.0 to avoid large
		// responses in saturated areas, and due to jpg artifacts
		int r = int((avgL - avgR) * 4.0 / res);

		intermediateResponses[x] = int16_t(min(181, max(-181, r))); // maximal possible sqrt response: 181 = sqrt(2^15)
	}

	// separate pass for non-max suppression -- slow, how to speed up?
	ocean_assert(window  >= 1);

	for (unsigned int x = window + 1u; x < width - window - 1u; ++x)
	{
		int16_t rx = intermediateResponses[x];
		int16_t rl = intermediateResponses[x - 1];
		int16_t rr = intermediateResponses[x + 1];

		if ((rx > 0 && (rx <= rl || rx < rr)) || (rx < 0 && (rx >= rl || rx > rr)))
		{
			responses[x] = 0;
		}
		else
		{
			responses[x] = rx;
		}
	}

	// clear borders
	for (unsigned int x = 0u; x < window + 1u; ++x)
	{
		responses[x] = 0;
		responses[width - 1u - x] = 0;
	}
}

LineDetectorULF::ADBarEdgeDetectorI::ADBarEdgeDetectorI(const unsigned int window) :
	EdgeDetector(window, ET_BAR)
{
	ocean_assert(window_ >= 1u);
}

void LineDetectorULF::ADBarEdgeDetectorI::invokeVertical(const uint8_t* frame, const unsigned int width, const unsigned int height, int16_t* responses, const unsigned int paddingElements) const
{
	ocean_assert(frame != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(responses != nullptr);

	ocean_assert(window_ >= 1u && window_ < width);

	// **TODO** ensure width - (window + barSize_2) * 2u >= 8 due to NEON

	const unsigned int frameStrideElements = width + paddingElements;

	Memory windowSumsMemory = Memory::create<uint32_t>(width - (window_ - 1u));
	uint32_t* const windowSums = windowSumsMemory.data<uint32_t>();

	int16_t* rowResponses = responses;

	for (unsigned int y = 0u; y < height; ++y)
	{
		determineRowSums(frame, width, window_, windowSums);

		invokeRowVertical(frame, width, window_, windowSums, rowResponses);

		frame += frameStrideElements;
		rowResponses += width;
	}
}

unsigned int LineDetectorULF::ADBarEdgeDetectorI::adjustThreshold(const unsigned int threshold) const
{
	return staticAdjustThreshold(threshold);
}

void LineDetectorULF::ADBarEdgeDetectorI::invokeRowVertical(const uint8_t* const row, const unsigned int width, const unsigned int window, const uint32_t* const windowSums, int16_t* responses)
{
	constexpr unsigned int barSize = 3u;
	constexpr unsigned int barSize_2 = barSize / 2u;

	const unsigned int elements = width - (window + barSize_2) * 2u;

	// clear left border
	memset(responses, 0, (window + barSize_2) * sizeof(int16_t));

	const uint32_t* windowSumsL = windowSums;
	const uint32_t* windowSumsR = windowSums + window + barSize;

	const uint8_t* value = row + window;

	responses += window + barSize_2;

	const int threshold = 10 * window * barSize;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	ocean_assert(elements >= 8u);

	const uint16x4_t window_u_16x4 = vdup_n_u16(uint16_t(window));
	const uint32x4_t barSize_u_32x4 = vdupq_n_u32(barSize);

	const int32x4_t thresholdPositive_s_32x4 = vdupq_n_s32(threshold);
	const int32x4_t thresholdNegative_s_32x4 = vdupq_n_s32(-threshold);

	for (unsigned int x = 0u; x < elements; x += 8u)
	{
		if (x + 8u > elements)
		{
			// the last iteration will not fit into the data,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(x >= 8u && elements > 8u);
			const unsigned int newX = elements - 8u;

			ocean_assert(x > newX);
			const unsigned int offset = x - newX;

			value -= offset;

			windowSumsL -= offset;
			windowSumsR -= offset;

			responses -= offset;

			// the for loop will stop after this iteration
			ocean_assert(!(x + 8u < elements));
		}

		// sumL * barSize
		const uint32x4_t sumLA_u_32x4 = vmulq_u32(vld1q_u32(windowSumsL + 0), barSize_u_32x4);
		const uint32x4_t sumLB_u_32x4 = vmulq_u32(vld1q_u32(windowSumsL + 4), barSize_u_32x4);

		// sumR * barSize
		const uint32x4_t sumRA_u_32x4 = vmulq_u32(vld1q_u32(windowSumsR + 0), barSize_u_32x4);
		const uint32x4_t sumRB_u_32x4 = vmulq_u32(vld1q_u32(windowSumsR + 4), barSize_u_32x4);

		// bar = value[0] + value[1] + value[2]
		uint16x8_t bar_u_16x8 = vmovl_u8(vld1_u8(value + 0));
		bar_u_16x8 = vaddw_u8(bar_u_16x8, vld1_u8(value + 1));
		bar_u_16x8 = vaddw_u8(bar_u_16x8, vld1_u8(value + 2));

		// bar *= window
		const uint32x4_t barA_u_32x4 = vmull_u16(vget_low_u16(bar_u_16x8), window_u_16x4);
		const uint32x4_t barB_u_32x4 = vmull_u16(vget_high_u16(bar_u_16x8), window_u_16x4);

		const int32x4_t deltaLA_s_32x4 = vsubq_s32(vreinterpretq_s32_u32(barA_u_32x4), vreinterpretq_s32_u32(sumLA_u_32x4));
		const int32x4_t deltaLB_s_32x4 = vsubq_s32(vreinterpretq_s32_u32(barB_u_32x4), vreinterpretq_s32_u32(sumLB_u_32x4));
		const int32x4_t deltaRA_s_32x4 = vsubq_s32(vreinterpretq_s32_u32(barA_u_32x4), vreinterpretq_s32_u32(sumRA_u_32x4));
		const int32x4_t deltaRB_s_32x4 = vsubq_s32(vreinterpretq_s32_u32(barB_u_32x4), vreinterpretq_s32_u32(sumRB_u_32x4));

		// (leftDelta >= threshold && rightDelta >= threshold) || (leftDelta <= -threshold && rightDelta <= -threshold)
		const uint32x4_t maskA_u_32x4 = vorrq_u32(vandq_u32(vcgeq_s32(deltaLA_s_32x4, thresholdPositive_s_32x4), vcgeq_s32(deltaRA_s_32x4, thresholdPositive_s_32x4)),
													vandq_u32(vcleq_s32(deltaLA_s_32x4, thresholdNegative_s_32x4), vcleq_s32(deltaRA_s_32x4, thresholdNegative_s_32x4)));

		const uint32x4_t maskB_u_32x4 = vorrq_u32(vandq_u32(vcgeq_s32(deltaLB_s_32x4, thresholdPositive_s_32x4), vcgeq_s32(deltaRB_s_32x4, thresholdPositive_s_32x4)),
													vandq_u32(vcleq_s32(deltaLB_s_32x4, thresholdNegative_s_32x4), vcleq_s32(deltaRB_s_32x4, thresholdNegative_s_32x4)));

		const uint16x8_t mask_u_16x8 = vcombine_u16(vmovn_u32(maskA_u_32x4), vmovn_u32(maskB_u_32x4));

		// response = (leftDelta + rightDelta) / 2;
		const int16x4_t responseA_s_16x4 = vrshrn_n_s32(vaddq_s32(deltaLA_s_32x4, deltaRA_s_32x4), 1);
		const int16x4_t responseB_s_16x4 = vrshrn_n_s32(vaddq_s32(deltaLB_s_32x4, deltaRB_s_32x4), 1);

		// minmax(NumericT<int16_t>::minValue(), response, NumericT<int16_t>::maxValue())
		const int16x8_t response_s_16x8 = vandq_s16(vcombine_s16(responseA_s_16x4, responseB_s_16x4), vreinterpretq_s16_u16(mask_u_16x8));

		vst1q_s16(responses, response_s_16x8);

		value += 8;

		windowSumsL += 8;
		windowSumsR += 8;

		responses += 8;
	}

#else // OCEAN_HARDWARE_NEON_VERSION >= 10

	const int16_t* const responsesEnd = responses + elements;

	while (responses != responsesEnd)
	{
		ocean_assert(responses < responsesEnd);

		ocean_assert(windowSumsL < windowSums + width - (window - 1u));
		ocean_assert(windowSumsR < windowSums + width - (window - 1u));

		const int bar = (value[0] + value[1] + value[2]) * window;

		const int leftDelta = bar - *windowSumsL++ * barSize;
		const int rightDelta = bar - *windowSumsR++ * barSize;

		*responses = 0;

		if ((leftDelta >= threshold && rightDelta >= threshold) || (leftDelta <= -threshold && rightDelta <= -threshold))
		{
			const int response = (leftDelta + rightDelta) / 2;

			ocean_assert(response >= NumericT<int16_t>::minValue() && response <= NumericT<int16_t>::maxValue());

			*responses = int16_t(response);
		}

		++value;
		++responses;
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	// clear right border
	memset(responses, 0, (window + barSize_2) * sizeof(int16_t));
}

#ifdef TODO_NOT_YET_IMPLEMENTED

#if 1

// find zigzag edges, using windows size of 4*w, threshold t
// looks for low, high, low, high image intensities
static void process_row_zigzag(const uint8_t* rowi, int16_t* rowb, int n, int w, int t)
{
  uint32_t blocks[4] = {0u, 0u, 0u, 0u};

  for (int x = 0; x < n - w * 4; ++x)
  {
    blocks[0] = 0u;
    blocks[1] = 0u;
    blocks[2] = 0u;
    blocks[3] = 0u;

    for (int b = 0; b < w; ++b)
    {
      blocks[0] += rowi[x + 0 * w + b];
      blocks[1] += rowi[x + 1 * w + b];
      blocks[2] += rowi[x + 2 * w + b];
      blocks[3] += rowi[x + 3 * w + b];
    }

    const int diff01 = int(blocks[0] - blocks[1]) / w;
    const int diff12 = int(blocks[1] - blocks[2]) / w;
    const int diff23 = int(blocks[2] - blocks[3]) / w;

    if (diff01 >= t && diff12 <= -t && diff23 >= t)
    {
      rowb[x + 3 * w / 2] = min((diff01 - diff12) * 2, 127);
    }
    else if (diff01 <= -t && diff12 >= t && diff23 <= -t)
    {
      rowb[x + 5 * w / 2] = min((diff12 - diff23) * 2, 127);
    }
  }
}

#else

// find zigzag edges, using windows size of 4*w, threshold t
// looks for low, high, low, high image intensities
void process_row_zigzag(uchar* rowi, int16_t* rowb, int n, int w, int t)
{
  for (int x = 0; x < n - w * 4; ++x)
  {
    const uint8_t blocks0 = rowi[x + (1 * w) / 2];
    const uint8_t blocks1 = rowi[x + (3 * w) / 2];
    const uint8_t blocks2 = rowi[x + (5 * w) / 2];
    const uint8_t blocks3 = rowi[x + (7 * w) / 2];

    const int diff01 = int(blocks0 - blocks1);
    const int diff12 = int(blocks1 - blocks2);
    const int diff23 = int(blocks2 - blocks3);

    if (diff01 >= t && diff12 <= -t && diff23 >= t)
    {
      rowb[x + 3 * w / 2] = min((diff01 - diff12) * 2, 127);
    }
    else if (diff01 <= -t && diff12 >= t && diff23 <= -t)
    {
      rowb[x + 5 * w / 2] = min((diff12 - diff23) * 2, 127);
    }
  }
}

#endif

#endif // TODO_NOT_YET_IMPLEMENTED

LineDetectorULF::SDStepEdgeDetectorI::SDStepEdgeDetectorI(const unsigned int window, const unsigned int stepSize) :
	EdgeDetector(window, ET_STEP),
	stepSize_(stepSize)
{
	ocean_assert(window_ >= 1u && window_ <= 127u);
}

void LineDetectorULF::SDStepEdgeDetectorI::invokeVertical(const uint8_t* frame, const unsigned int width, const unsigned int height, int16_t* response, const unsigned int paddingElements) const
{
	ocean_assert(frame != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(response != nullptr);

	ocean_assert(window_ >= 1u && window_ < width);

	// **TODO** ensure that width - (window + stepSize_2) * 2u >= 8 due to NEON

	const unsigned int frameStrideElements = width + paddingElements;

	Memory windowSumsMemory = window_ >= 3u ? Memory::create<uint16_t>(width - (window_ - 1u)) : Memory();
	uint16_t* windowSums = window_ >= 3u ? windowSumsMemory.data<uint16_t>() : nullptr;

	int16_t* responseRow = response;

	for (unsigned int y = 0u; y < height; ++y)
	{
		if (windowSums != nullptr)
		{
			determineRowSums(frame, width, window_, windowSums);
		}

		invokeRowVertical(frame, width, stepSize_, window_, windowSums, responseRow);

		frame += frameStrideElements;
		responseRow += width;
	}
}

bool LineDetectorULF::SDStepEdgeDetectorI::invokeHorizontal(const uint8_t* frame, const unsigned int width, const unsigned int height, int16_t* responses, const unsigned int paddingElements) const
{

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	ocean_assert(frame != nullptr);
	ocean_assert((window_ >= 3u && width >= 8u) || width >= 16u);

	const unsigned int stepSize_2 = stepSize_ / 2u; // 0 in case 'stepSize == 1'

	const unsigned int frameStrideElements = width + paddingElements;

	// we can set the first response rows to zero
	memset(responses, 0x00, width * (window_ + stepSize_2) * sizeof(int16_t));

	// we use an intermediate ring buffer with 3 response rows in which we store the responses,
	// followed by a non-maximum supression which will be written to the actual responses frame
	Memory responseRingBufferMemory = Memory::create<int16_t>(width * 3u);
	memset(responseRingBufferMemory.data(), 0x00, responseRingBufferMemory.size());

	Memory windowSumMemory;
	uint16_t* windowSumTop = nullptr;
	uint16_t* windowSumBottom = nullptr;

	if (window_ >= 3u)
	{
		windowSumMemory = Memory::create<uint16_t>(width * 2u);
		memset(windowSumMemory.data(), 0x00, windowSumMemory.size());
		windowSumTop = windowSumMemory.data<uint16_t>();
		windowSumBottom = windowSumMemory.data<uint16_t>() + width;

		// handle the first sum rows

		for (unsigned int y = 0u; y < window_ - 1u; ++y)
		{
			applyRowSum<true>(frame + y * frameStrideElements, width, windowSumTop);
		}

		for (unsigned int y = window_ + stepSize_; y < window_ * 2u + stepSize_ - 1u; ++y)
		{
			applyRowSum<true>(frame + y * frameStrideElements, width, windowSumBottom);
		}
	}

	const int16x8_t zero_s_16x8 = vdupq_n_s16(0);

	unsigned int responseRingBufferIndex = 0u;

	for (unsigned int y = window_ + stepSize_2; y < height - window_ - stepSize_2 + 1u; ++y)
	{
		int16_t* responsesIntermediate = responseRingBufferMemory.data<int16_t>() + (responseRingBufferIndex % 3u) * width; // the current intermediate ring buffer row

		if (y < height - window_ - stepSize_2)
		{
			ocean_assert(y - window_ - stepSize_2 < height);
			const uint8_t* const rowSumTop = frame + (y -  window_ - stepSize_2) * frameStrideElements; // first row of top sum-block

			ocean_assert(y + stepSize_2 + 1u < height);
			const uint8_t* const rowSumBottom = rowSumTop + (window_ + stepSize_) * frameStrideElements; // first row of bottom sum-block

			if (window_ == 1u)
			{
				ocean_assert(width >= 16u);

				for (unsigned int x = 0u; x < width; x += 16u)
				{
					if (x + 16u > width)
					{
						// the last iteration will not fit into the data,
						// so we simply shift x left by some pixels (at most 15) and we will calculate some pixels again

						ocean_assert(x >= 16u && width > 16u);
						const unsigned int newX = width - 16u;

						ocean_assert(x > newX);

						x = newX;

						// the for loop will stop after this iteration
						ocean_assert(x + 16u == width);
					}

					const uint8x16_t top_u_8x16 = vld1q_u8(rowSumTop + x);
					const uint8x16_t bottom_u_8x16 = vld1q_u8(rowSumBottom + x);

					// we store the result in the intermediate ring buffer
					vst1q_s16(responsesIntermediate + x + 0, vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(top_u_8x16), vget_low_u8(bottom_u_8x16))));
					vst1q_s16(responsesIntermediate + x + 8, vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(top_u_8x16), vget_high_u8(bottom_u_8x16))));
				}
			}
			else if (window_ == 2u)
			{
				ocean_assert(width >= 16u);

				for (unsigned int x = 0u; x < width; x += 16u)
				{
					if (x + 16u > width)
					{
						// the last iteration will not fit into the data,
						// so we simply shift x left by some pixels (at most 15) and we will calculate some pixels again

						ocean_assert(x >= 16u && width > 16u);
						const unsigned int newX = width - 16u;

						ocean_assert(x > newX);

						x = newX;

						// the for loop will stop after this iteration
						ocean_assert(x + 16u == width);
					}

					const uint8x16_t top0_u_8x16 = vld1q_u8(rowSumTop + x);
					const uint8x16_t top1_u_8x16 = vld1q_u8(rowSumTop + frameStrideElements + x);
					const uint16x8_t topA_u_16x8 = vaddl_u8(vget_low_u8(top0_u_8x16), vget_low_u8(top1_u_8x16));
					const uint16x8_t topB_u_16x8 = vaddl_u8(vget_high_u8(top0_u_8x16), vget_high_u8(top1_u_8x16));

					const uint8x16_t bottom0_u_8x16 = vld1q_u8(rowSumBottom + x);
					const uint8x16_t bottom1_u_8x16 = vld1q_u8(rowSumBottom + frameStrideElements + x);
					const uint16x8_t bottomA_u_16x8 = vaddl_u8(vget_low_u8(bottom0_u_8x16), vget_low_u8(bottom1_u_8x16));
					const uint16x8_t bottomB_u_16x8 = vaddl_u8(vget_high_u8(bottom0_u_8x16), vget_high_u8(bottom1_u_8x16));

					// we store the result in the intermediate ring buffer
					vst1q_s16(responsesIntermediate + x + 0, vsubq_s16(vreinterpretq_s16_u16(topA_u_16x8), vreinterpretq_s16_u16(bottomA_u_16x8)));
					vst1q_s16(responsesIntermediate + x + 8, vsubq_s16(vreinterpretq_s16_u16(topB_u_16x8), vreinterpretq_s16_u16(bottomB_u_16x8)));
				}
			}
			else
			{
				ocean_assert(window_ >= 3u);
				ocean_assert(width >= 8u);

				ocean_assert(windowSumTop != nullptr);
				ocean_assert(windowSumBottom != nullptr);

				const unsigned int windowBockStride1 = frameStrideElements * (window_ - 1u);

				for (unsigned int x = 0u; x < width; x += 8u)
				{
					if (x + 8u > width)
					{
						// the last iteration will not fit into the data,
						// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

						ocean_assert(x >= 8u && width > 8u);
						const unsigned int newX = width - 8u;

						ocean_assert(x > newX);

						// reverting the sum calculations from the last iteration

						for (unsigned int n = newX; n < x; ++n)
						{
							windowSumTop[n] += (rowSumTop)[n];
							windowSumTop[n] -= (rowSumTop + windowBockStride1)[n];

							windowSumBottom[n] += (rowSumBottom)[n];
							windowSumBottom[n] -= (rowSumBottom + windowBockStride1)[n];
						}

						x = newX;

						// the for loop will stop after this iteration
						ocean_assert(x + 8u == width);
					}

					uint16x8_t sumTop_u_16x8 = vld1q_u16(windowSumTop + x);
					uint16x8_t sumBottom_u_16x8 = vld1q_u16(windowSumBottom + x);

					// handle the current sum row(s)

					uint8x8_t rowSumTop_u_8x8 = vld1_u8(rowSumTop + windowBockStride1 + x); // load remaining row to finalize sum values
					uint8x8_t rowSumBottom_u_8x8 = vld1_u8(rowSumBottom + windowBockStride1 + x);

					sumTop_u_16x8 = vaddw_u8(sumTop_u_16x8, rowSumTop_u_8x8); // add next top row
					sumBottom_u_16x8 = vaddw_u8(sumBottom_u_16x8, rowSumBottom_u_8x8); // add next bottom row

					const int16x8_t response_s_16x8 = vsubq_s16(vreinterpretq_s16_u16(sumTop_u_16x8), vreinterpretq_s16_u16(sumBottom_u_16x8));

					// we store the result in the intermediate ring buffer
					vst1q_s16(responsesIntermediate + x, response_s_16x8);

					// handle the last sum row(s)

					rowSumTop_u_8x8 = vld1_u8(rowSumTop + x); // load remaining row to finalize sum values
					rowSumBottom_u_8x8 = vld1_u8(rowSumBottom + x);

					sumTop_u_16x8 = vsubw_u8(sumTop_u_16x8, rowSumTop_u_8x8); // add next top row
					sumBottom_u_16x8 = vsubw_u8(sumBottom_u_16x8, rowSumBottom_u_8x8); // add next bottom row

					vst1q_u16(windowSumTop + x, sumTop_u_16x8);
					vst1q_u16(windowSumBottom + x, sumBottom_u_16x8);
				}
			}
		}
		else
		{
			// this is the last row in which we do not determine valid responses anymore
			memset(responsesIntermediate, 0x00, sizeof(int16_t) * width);
		}

		if (responseRingBufferIndex >= 1u)
		{
			// we have at seen two rows (and a zero response row), so that we can apply the non maximum supression

			int16_t* sqrResponses = responses + (y - 1u) * width; // the response will be for the previous row

			const int16_t* responseTop = responseRingBufferMemory.data<int16_t>() + ((responseRingBufferIndex + 1u) % 3u) * width;
			const int16_t* responseCenter = responseRingBufferMemory.data<int16_t>() + ((responseRingBufferIndex + 2u) % 3u) * width;
			const int16_t* responseBottom = responseRingBufferMemory.data<int16_t>() + (responseRingBufferIndex % 3u) * width;

			for (unsigned int x = 0u; x < width; x += 8u)
			{
				if (x + 8u > width)
				{
					// the last iteration will not fit into the data,
					// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

					ocean_assert(x >= 8u && width > 8u);
					const unsigned int newX = width - 8u;

					ocean_assert(x > newX);

					x = newX;

					// the for loop will stop after this iteration
					ocean_assert(x + 8u == width);
				}

				const int16x8_t responsesTop_s_16x8 = vld1q_s16(responseTop + x);
				const int16x8_t responsesCenter_s_16x8 = vld1q_s16(responseCenter + x);
				const int16x8_t responsesBottom_s_16x8 = vld1q_s16(responseBottom + x);

				// mask = (center > 0 && (center <= top || center < bottom)) || (center < 0 && (center >= top || center > bottom) ? 0xFFFF : 0x0000

				const uint16x8_t centerGreaterZero_u_16x8 = vandq_u16(vcgtq_s16(responsesCenter_s_16x8, zero_s_16x8), vorrq_u16(vcleq_s16(responsesCenter_s_16x8, responsesTop_s_16x8), vcltq_s16(responsesCenter_s_16x8, responsesBottom_s_16x8)));
				const uint16x8_t centerLessZero_u_16x8 = vandq_u16(vcltq_s16(responsesCenter_s_16x8, zero_s_16x8), vorrq_u16(vcgeq_s16(responsesCenter_s_16x8, responsesTop_s_16x8), vcgtq_s16(responsesCenter_s_16x8, responsesBottom_s_16x8)));
				const uint16x8_t mask_u_16x8 = vorrq_u16(centerGreaterZero_u_16x8, centerLessZero_u_16x8);

				const int16x8_t nonMaximumSuppressionResponse_s_16x8 = vandq_s16(responsesCenter_s_16x8, vreinterpretq_s16_u16(vmvnq_u16(mask_u_16x8))); // vmvnq_u16: 0xFFFF <-> 0x0000

				vst1q_s16(sqrResponses + x, nonMaximumSuppressionResponse_s_16x8);
			}
		}

		++responseRingBufferIndex;
	}

	// we set the last response rows to zero
	memset(responses + width * (height - window_ - stepSize_2), 0x00, width * (window_ + stepSize_2) * sizeof(int16_t));

	return true;

#else // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	OCEAN_SUPPRESS_UNUSED_WARNING(frame);
	OCEAN_SUPPRESS_UNUSED_WARNING(width);
	OCEAN_SUPPRESS_UNUSED_WARNING(height);
	OCEAN_SUPPRESS_UNUSED_WARNING(responses);
	OCEAN_SUPPRESS_UNUSED_WARNING(paddingElements);

	return false;

#endif

}

bool LineDetectorULF::SDStepEdgeDetectorI::hasInvokeHorizontal(const unsigned int width, const unsigned int height) const
{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	if (window_ >= 3u)
	{
		return width >= 8u && height >= window_ * 2u + stepSize_;
	}
	else
	{
		return width >= 16u && height >= window_ * 2u + stepSize_;
	}

#else

	OCEAN_SUPPRESS_UNUSED_WARNING(width);
	OCEAN_SUPPRESS_UNUSED_WARNING(height);

	return false;

#endif
}

unsigned int LineDetectorULF::SDStepEdgeDetectorI::adjustThreshold(const unsigned int threshold) const
{
	return threshold;
}

void LineDetectorULF::SDStepEdgeDetectorI::invokeRowVertical(const uint8_t* const row, const unsigned int width, const unsigned int stepSize, const unsigned int window, const uint16_t* const windowSums, int16_t* responses)
{
	ocean_assert(row != nullptr);
	ocean_assert(width != 0u);
	ocean_assert(window >= 1u && window <= std::min(width, 127u));
	ocean_assert(responses != nullptr);
	ocean_assert((windowSums == nullptr && window < 3u) || (windowSums != nullptr && window >= 3u));

	const unsigned int stepSize_2 = stepSize / 2u; // 0 in case 'stepSize == 1'

	const unsigned int elements = width - (window + stepSize_2) * 2u;

	int16_t* const responsesStart = responses;
	int16_t* const responsesEnd = responses + width;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	ocean_assert(elements >= 8u);

	// Clear left border
	memset(responses, 0, (window + stepSize_2) * sizeof(int16_t));

	responses += window + stepSize_2;

	const uint8_t* left = row;
	const uint8_t* right = left + window + stepSize;

	if (window == 1u)
	{
		for (unsigned int x = 0u; x < elements; x += 8u)
		{
			if (x + 8u > elements)
			{
				// The last iteration will not fit into the data,
				// Shift x left by up to 7 pixels and continue the computation

				ocean_assert(x >= 8u && elements > 8u);
				const unsigned int newX = elements - 8u;

				ocean_assert(x > newX);
				const unsigned int offset = x - newX;

				left -= offset;
				right -= offset;
				responses -= offset;

				// Ensure that the for loop will stop after this iteration
				ocean_assert(!(x + 8u < elements));
			}

			// responses = leftSum - rightSum
			const int16x8_t responses_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vld1_u8(left), vld1_u8(right)));

			vst1q_s16(responses, responses_s_16x8);

			left += 8;
			right += 8;
			responses += 8;
		}
	}
	else if (window == 2u)
	{
		for (unsigned int x = 0u; x < elements; x += 8u)
		{
			if (x + 8u > elements)
			{
				// The last iteration will not fit into the data,
				// Shift x left by up to 7 pixels and continue the computation

				ocean_assert(x >= 8u && elements > 8u);
				const unsigned int newX = elements - 8u;

				ocean_assert(x > newX);
				const unsigned int offset = x - newX;

				left -= offset;
				right -= offset;
				responses -= offset;

				// Ensure that the for loop will stop after this iteration
				ocean_assert(!(x + 8u < elements));
			}

			// leftSum = left0 + left1
			const uint16x8_t leftSum_u_16x8 = vaddl_u8(vld1_u8(left + 0), vld1_u8(left + 1));

			// rightSum = right0 + right1
			const uint16x8_t rightSum_u_16x8 = vaddl_u8(vld1_u8(right + 0), vld1_u8(right + 1));

			// responses = leftSum - rightSum
			const int16x8_t responses_s_16x8 = vsubq_s16(vreinterpretq_s16_u16(leftSum_u_16x8), vreinterpretq_s16_u16(rightSum_u_16x8));

			vst1q_s16(responses, responses_s_16x8);

			left += 8;
			right += 8;
			responses += 8;
		}
	}
	else
	{
		ocean_assert(window >= 3u);

		const uint16_t* leftSum = windowSums;
		const uint16_t* rightSum = windowSums + window + stepSize;

		for (unsigned int x = 0u; x < elements; x += 8u)
		{
			if (x + 8u > elements)
			{
				// The last iteration will not fit into the data,
				// Shift x left by up to 7 pixels and continue the computation

				ocean_assert(x >= 8u && elements > 8u);
				const unsigned int newX = elements - 8u;

				ocean_assert(x > newX);
				const unsigned int offset = x - newX;

				leftSum -= offset;
				rightSum -= offset;
				responses -= offset;

				// Ensure that the for loop will stop after this iteration
				ocean_assert(!(x + 8u < elements));
			}

			// responses = leftSum - rightSum
			const int16x8_t responses_s_16x8 = vsubq_s16(vreinterpretq_s16_u16(vld1q_u16(leftSum)), vreinterpretq_s16_u16(vld1q_u16(rightSum)));

			vst1q_s16(responses, responses_s_16x8);

			leftSum += 8;
			rightSum += 8;
			responses += 8;
		}
	}

	// Clear right border
	ocean_assert(responsesEnd > responses);
	const size_t remaining = responsesEnd - responses;
	memset(responses, 0, remaining * sizeof(int16_t));

	// Nonmaximum suppression

	const int16x8_t zero_s_16x8 = vdupq_n_s16(0);

	responses = responsesStart + window + stepSize_2;

	// The non-maximum suppression must not set its result immediately because that may influence the outcome of the next
	// iteration of non-maximum suppression. This is avoided by delayed storing of the results (storing is always one
	// iteration behind)
	int16x8_t delayedNonMaximumSuppressionResponse_s_16x8 = vld1q_s16(responses);
	int16_t* previousResponse = responses;

	for (unsigned int x = 0u; x < elements; x += 8u)
	{
		if (x + 8u > elements)
		{
			// the last iteration will not fit into the data,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(x >= 8u && elements > 8u);
			const unsigned int newX = elements - 8u;

			ocean_assert(x > newX);
			const unsigned int offset = x - newX;

			responses -= offset;

			// The for loop will stop after this iteration
			ocean_assert(!(x + 8u < elements));
		}

		const int16x8_t responsesLeft_s_16x8 = vld1q_s16(responses - 1);
		const int16x8_t responsesCenter_s_16x8 = vld1q_s16(responses + 0);
		const int16x8_t responsesRight_s_16x8 = vld1q_s16(responses + 1);

		// Delayed storing of results
		vst1q_s16(previousResponse, delayedNonMaximumSuppressionResponse_s_16x8);

		// mask = (center > 0 && center > left && center >= right) || (center < 0 && center < left && center <= right) ? 0xFFFF : 0x0000
		const uint16x8_t mask_u_16x8 = vorrq_u16(
			vandq_u16(vcgtq_s16(responsesCenter_s_16x8, zero_s_16x8), vandq_u16(vcgtq_s16(responsesCenter_s_16x8, responsesLeft_s_16x8), vcgeq_s16(responsesCenter_s_16x8, responsesRight_s_16x8))),
			vandq_u16(vcltq_s16(responsesCenter_s_16x8, zero_s_16x8), vandq_u16(vcltq_s16(responsesCenter_s_16x8, responsesLeft_s_16x8), vcleq_s16(responsesCenter_s_16x8, responsesRight_s_16x8))));

		delayedNonMaximumSuppressionResponse_s_16x8 = vandq_s16(responsesCenter_s_16x8, vreinterpretq_s16_u16(mask_u_16x8));

		previousResponse = responses;
		responses += 8;
	}

	// Delayed storing of results
	vst1q_s16(previousResponse, delayedNonMaximumSuppressionResponse_s_16x8);

#else

	// Clear left border
	memset(responses, 0, (window + stepSize_2) * sizeof(int16_t));

	const uint8_t* left = row;
	const uint8_t* right = row + window + stepSize;

	responses += window + stepSize_2;
	const int16_t* const responsesElementsEnd = responses + elements;

	if (window == 1u)
	{
		while (responses != responsesElementsEnd)
		{
			*responses++ = int16_t(*left++) - int16_t(*right++);
		}
	}
	else if (window == 2u)
	{
		while (responses != responsesElementsEnd)
		{
			*responses = int16_t(left[0] + left[1]) - int16_t(right[0] + right[1]);

			++responses;
			++left;
			++right;
		}
	}
	else
	{
		ocean_assert(window >= 3u);

		const uint16_t* leftSum = windowSums;
		const uint16_t* rightSum = windowSums + window + stepSize;

		while (responses != responsesElementsEnd)
		{
			*responses = int16_t(*leftSum) - int16_t(*rightSum);

			++responses;
			++leftSum;
			++rightSum;
		}
	}

	// Clear right border
	ocean_assert(responsesEnd > responses);
	const size_t remaining = responsesEnd - responses;
	memset(responses, 0, remaining * sizeof(int16_t));

	// Non-maximum suppression

	responses = responsesStart;

	// The non-maximum suppression must not set its result immediately because that may influence the outcome of the next
	// iteration of non-maximum suppression. This is avoided by delayed storing of the results (storing is always one
	// iteration behind).
	int16_t delayedNonMaximumSuppressionResponse = 0;
	ocean_assert(responses[window + stepSize_2 - 1u] == 0);

	for (unsigned int x = window + stepSize_2; x < width - window - stepSize_2; ++x)
	{
		const int16_t leftResponse = responses[x - 1];
		const int16_t centerResponse = responses[x];
		const int16_t rightResponse = responses[x + 1];

		// Delayed storing of results
		responses[x - 1] = delayedNonMaximumSuppressionResponse;

		if ((centerResponse > 0 && (centerResponse <= leftResponse || centerResponse < rightResponse)) || (centerResponse < 0 && (centerResponse >= leftResponse || centerResponse > rightResponse)))
		{
			delayedNonMaximumSuppressionResponse = 0;
		}
		else
		{
			delayedNonMaximumSuppressionResponse = responses[x];
		}
	}

	// Delayed storing of results
	responses[width - window - stepSize_2 - 1u] = delayedNonMaximumSuppressionResponse;

#endif
}

FiniteLines2 LineDetectorULF::detectLines(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const EdgeDetectors& edgeDetectors, const unsigned int threshold, const unsigned int minimalLength, const float maximalStraightLineDistance, EdgeTypes* types, const ScanDirection scaneDirection)
{
	ocean_assert(!edgeDetectors.empty());

	ocean_assert(yFrame != nullptr);
	ocean_assert(width >= 20u && height >= 20u);

	if (yFrame == nullptr || width < 20u || height < 20u || edgeDetectors.empty())
	{
		return FiniteLines2();
	}

	Frame reusableResponseBuffer(FrameType(width, height, FrameType::genericPixelFormat<int16_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));

	FiniteLines2 detectedLines;

	if (types)
	{
		ocean_assert(types->empty());
		types->clear();
	}

	Memory yFrameTransposedMemory;

	unsigned int yFrameTransposedMemoryPaddingElements = 0u;

	for (const std::shared_ptr<EdgeDetector>& edgeDetector : edgeDetectors)
	{
		ocean_assert(edgeDetector);

		if (edgeDetector)
		{
			detectLines(yFrame, yFrameTransposedMemory, width, height, framePaddingElements, yFrameTransposedMemoryPaddingElements, *edgeDetector, detectedLines, scaneDirection, threshold, reusableResponseBuffer.data<int16_t>(), minimalLength, maximalStraightLineDistance, types);
		}
	}

	ocean_assert(types == nullptr || types->size() == detectedLines.size());

	return detectedLines;
}

void LineDetectorULF::separateStraightLines(const unsigned int* pixelPositionsMajor, const unsigned int firstPositionIndex, const unsigned int lastPositionIndex, FiniteLines2& lines, const unsigned int minimalLength, const float maximalOffset, const bool majorIsY, const bool refine)
{
	ocean_assert(pixelPositionsMajor != nullptr);
	ocean_assert(firstPositionIndex <= lastPositionIndex);
	ocean_assert(minimalLength >= 2u);
	ocean_assert(maximalOffset >= 0.0f);

	if (lastPositionIndex - firstPositionIndex + 1u < minimalLength)
	{
		return;
	}

	const unsigned int& startPointMajor = pixelPositionsMajor[firstPositionIndex];
	const unsigned int& endPointMajor = pixelPositionsMajor[lastPositionIndex];

	const unsigned int& startPointMinor = firstPositionIndex;
	const unsigned int& endPointMinor = lastPositionIndex;

	const float mMajor = float(int(endPointMajor - startPointMajor));
	const float mMinor = float(int(endPointMinor - startPointMinor));

	ocean_assert(NumericF::isNotEqualEps(mMinor));
	const float mMajor_Minor = mMajor / mMinor; // inverted slope based on the end points

	float worstOffset = 0.0f;
	unsigned int worstOffsetMinor = (unsigned int)(-1);

	for (unsigned int minor = firstPositionIndex + 1u; minor < lastPositionIndex /* skipping end point */; ++minor)
	{
		const int dMinor = int(minor - firstPositionIndex);

		// a perfect line would follow: mx/my == dx/dy

		const float idealMajor = float(startPointMajor) + mMajor_Minor * float(dMinor);

		const float offset = NumericF::abs(float(pixelPositionsMajor[minor]) - idealMajor);

		if (offset > worstOffset)
		{
			worstOffset = offset;
			worstOffsetMinor = minor;
		}
	}

	if (worstOffset <= maximalOffset)
	{
		if (refine)
		{
			// least square fitting (via swapping x and y)

			float sumX = 0.0f;
			float sumY = 0.0f;

			float sumXX = 0.0f;
			float sumXY = 0.0f;

			for (unsigned int i = firstPositionIndex; i <= lastPositionIndex; ++i)
			{
				const float x = float(int(i - firstPositionIndex)); // original y coordinate relative to start point
				const float y = float(int(pixelPositionsMajor[i] - startPointMajor));

				sumX += x;
				sumY += y;

				sumXX += x * x;
				sumXY += x * y;
			}

			const float size =  float(lastPositionIndex - firstPositionIndex + 1u);

			const float determinant = sumX * sumX - size * sumXX;
			ocean_assert(NumericF::isNotEqualEps(determinant));

			const float invDeterminant = 1.0f / determinant;
			const float slope = (sumX * sumY - size * sumXY) * invDeterminant;
			const float interception = (sumX * sumXY - sumXX * sumY) * invDeterminant;

			const float refinedStartPointMajor = float(startPointMajor) + interception;
			const float refinedEndPointMajor = float(startPointMajor) + slope * float(lastPositionIndex - firstPositionIndex);

			if (majorIsY)
			{
				lines.emplace_back(Vector2(Scalar(startPointMinor), Scalar(refinedStartPointMajor)), Vector2(Scalar(endPointMinor), Scalar(refinedEndPointMajor)));
			}
			else
			{
				lines.emplace_back(Vector2(Scalar(refinedStartPointMajor), Scalar(startPointMinor)), Vector2(Scalar(refinedEndPointMajor), Scalar(endPointMinor)));
			}
		}
		else
		{
			if (majorIsY)
			{
				lines.emplace_back(Vector2(Scalar(startPointMinor), Scalar(startPointMajor)), Vector2(Scalar(endPointMinor), Scalar(endPointMajor)));
			}
			else
			{
				lines.emplace_back(Vector2(Scalar(startPointMajor), Scalar(startPointMinor)), Vector2(Scalar(endPointMajor), Scalar(endPointMinor)));
			}
		}
	}
	else
	{
		// divide the line into two lines and check again

		separateStraightLines(pixelPositionsMajor, firstPositionIndex, worstOffsetMinor, lines, minimalLength, maximalOffset, majorIsY, refine);
		separateStraightLines(pixelPositionsMajor, worstOffsetMinor, lastPositionIndex, lines, minimalLength, maximalOffset, majorIsY, refine);
	}
}

bool LineDetectorULF::detectLines(const uint8_t* yFrame, Memory& yFrameTransposedMemory, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, unsigned int& yFrameTransposedMemoryPaddingElements, const EdgeDetector& edgeDetector, FiniteLines2& detectedLines, const ScanDirection scanDirection, const unsigned int threshold, int16_t* reusableResponseBuffer, const unsigned int minimalLength, const float maximalStraightLineDistance, EdgeTypes* types)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(width != 0u && height != 0u);

	ocean_assert(types == nullptr || types->size() == detectedLines.size());

	Frame ownResponseFrame;

	if (reusableResponseBuffer == nullptr)
	{
		// we have to create our own response buffer;

		if (!ownResponseFrame.set(FrameType(width, height, FrameType::genericPixelFormat<int16_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT), true /*forceOwner*/, true /*forceWritable*/))
		{
			return false;
		}

		reusableResponseBuffer = ownResponseFrame.data<int16_t>();
	}

	ocean_assert(reusableResponseBuffer != nullptr);

	const unsigned int startThreshold = edgeDetector.adjustThreshold(threshold);
	const unsigned int intermediateThreshold = edgeDetector.adjustThreshold((threshold + 1u) / 2u);

	constexpr unsigned int responsePaddingElements = 0u;

	const size_t initialLinesSize = detectedLines.size();

	if (scanDirection & SD_VERTICAL)
	{
		// we need to detect vertical lines, so we simply invoke the vertical edge detector (without the need of transposing anything)

		edgeDetector.invokeVertical(yFrame, width, height, reusableResponseBuffer, yFramePaddingElements);
		extractVerticalLines(reusableResponseBuffer, width, height, responsePaddingElements, false /* not transposed */, detectedLines, startThreshold, intermediateThreshold, minimalLength, maximalStraightLineDistance, types);
	}

	if (scanDirection & SD_HORIZONTAL)
	{
		// we need to detect horizontal lines

		if (edgeDetector.hasInvokeHorizontal(width, height))
		{
			// we have an implementation for horizontal edge detector, so we simply invoke the horizontal edge detector (without the need of transposing anything)

			edgeDetector.invokeHorizontal(yFrame, width, height, reusableResponseBuffer, yFramePaddingElements);
			extractHorizontalLines(reusableResponseBuffer, width, height, responsePaddingElements, detectedLines, startThreshold, intermediateThreshold, minimalLength, maximalStraightLineDistance, types);
		}
		else
		{
			// we do not have an implementation for horizontal edge detector, so we need a transposed version of the input image, and we call the vertical edge detector

			if (yFrameTransposedMemory.isNull())
			{
				yFrameTransposedMemory = Memory::create<uint8_t>(width * height);
				yFrameTransposedMemoryPaddingElements = 0u;

				FrameTransposer::transpose<uint8_t, 1u>(yFrame, yFrameTransposedMemory.data<uint8_t>(), width, height, yFramePaddingElements, yFrameTransposedMemoryPaddingElements, nullptr);
			}

			ocean_assert(yFrameTransposedMemory);

			edgeDetector.invokeVertical(yFrameTransposedMemory.data<uint8_t>(), height, width, reusableResponseBuffer, yFrameTransposedMemoryPaddingElements);
			extractVerticalLines(reusableResponseBuffer, height, width, responsePaddingElements, true /* transposed */, detectedLines, startThreshold, intermediateThreshold, minimalLength, maximalStraightLineDistance, types);
		}
	}

	if (types)
	{
		// as extractVerticalLines() assigned the sign but not the type of the edges, we need to add the type here

		ocean_assert(types->size() == detectedLines.size());

		for (size_t n = initialLinesSize; n < detectedLines.size(); ++n)
		{
			(*types)[n] = EdgeType((*types)[n] | edgeDetector.edgeType());
		}
	}

	return true;
}

}

}

}
