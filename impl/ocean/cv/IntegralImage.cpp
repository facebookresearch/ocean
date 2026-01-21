/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/IntegralImage.h"
#include "ocean/cv/NEON.h"

namespace Ocean
{

namespace CV
{

Frame IntegralImage::Comfort::createLinedImage(const Frame& frame)
{
	ocean_assert(frame.isValid());
	ocean_assert(frame.numberPlanes() == 1u);
	ocean_assert(frame.channels() <= 4u);

	if (!frame.isValid() || frame.numberPlanes() != 1u || frame.channels() > 4u)
	{
		return Frame();
	}

	switch (frame.dataType())
	{
		case FrameType::DT_UNSIGNED_INTEGER_8:
			return createLinedImage<uint8_t, uint32_t>(frame);

		case FrameType::DT_SIGNED_INTEGER_8:
			return createLinedImage<int8_t, int32_t>(frame);

		case FrameType::DT_SIGNED_FLOAT_64:
			return createLinedImage<double, double>(frame);

		default:
			ocean_assert(false && "Not supported pixel format!");
			break;
	}

	return Frame();
}

Frame IntegralImage::Comfort::createBorderedImage(const Frame& frame, const unsigned int border)
{
	ocean_assert(frame.isValid());
	ocean_assert(frame.numberPlanes() == 1u);
	ocean_assert(frame.channels() <= 4u);
	ocean_assert(border >= 1u);

	if (!frame.isValid() || frame.numberPlanes() != 1u || frame.channels() > 4u)
	{
		return Frame();
	}

	switch (frame.dataType())
	{
		case FrameType::DT_UNSIGNED_INTEGER_8:
			return createBorderedImage<uint8_t, uint32_t>(frame, border);

		case FrameType::DT_SIGNED_INTEGER_8:
			return createBorderedImage<int8_t, int32_t>(frame, border);

		case FrameType::DT_SIGNED_FLOAT_64:
			return createBorderedImage<double, double>(frame, border);

		default:
			ocean_assert(false && "Not supported pixel format!");
			break;
	}

	return Frame();
}

template <typename T, typename TIntegral>
Frame IntegralImage::Comfort::createLinedImage(const Frame& frame)
{
	static_assert(sizeof(T) <= sizeof(TIntegral), "Invalid data type!");

	ocean_assert(frame.isValid());

	const unsigned int channels = frame.channels();
	ocean_assert(frame.isPixelFormatCompatible(FrameType::genericPixelFormat<T>(channels)));

	Frame linedIntegralFrame(FrameType(frame.width() + 1u, frame.height() + 1u, FrameType::genericPixelFormat<TIntegral>(channels), frame.pixelOrigin()));

	switch (channels)
	{
		case 1u:
			IntegralImage::createLinedImage<T, TIntegral, 1u>(frame.constdata<T>(), linedIntegralFrame.data<TIntegral>(), frame.width(), frame.height(), frame.paddingElements(), linedIntegralFrame.paddingElements());
			break;

		case 2u:
			IntegralImage::createLinedImage<T, TIntegral, 2u>(frame.constdata<T>(), linedIntegralFrame.data<TIntegral>(), frame.width(), frame.height(), frame.paddingElements(), linedIntegralFrame.paddingElements());
			break;

		case 3u:
			IntegralImage::createLinedImage<T, TIntegral, 3u>(frame.constdata<T>(), linedIntegralFrame.data<TIntegral>(), frame.width(), frame.height(), frame.paddingElements(), linedIntegralFrame.paddingElements());
			break;

		case 4u:
			IntegralImage::createLinedImage<T, TIntegral, 4u>(frame.constdata<T>(), linedIntegralFrame.data<TIntegral>(), frame.width(), frame.height(), frame.paddingElements(), linedIntegralFrame.paddingElements());
			break;

		default:
			ocean_assert(false && "Not supported channel number!");
			return Frame();
	}

	return linedIntegralFrame;
}

template <typename T, typename TIntegral>
Frame IntegralImage::Comfort::createBorderedImage(const Frame& frame, const unsigned int border)
{
	static_assert(sizeof(T) <= sizeof(TIntegral), "Invalid data type!");

	ocean_assert(frame.isValid());
	ocean_assert(border >= 1u);

	const unsigned int channels = frame.channels();
	ocean_assert(frame.isPixelFormatCompatible(FrameType::genericPixelFormat<T>(channels)));

	Frame linedIntegralFrame(FrameType(frame.width() + 1u + 2u * border, frame.height() + 1u + 2u * border, FrameType::genericPixelFormat<TIntegral>(channels), frame.pixelOrigin()));

	switch (channels)
	{
		case 1u:
			IntegralImage::createBorderedImage<T, TIntegral, 1u>(frame.constdata<T>(), linedIntegralFrame.data<TIntegral>(), frame.width(), frame.height(), border, frame.paddingElements(), linedIntegralFrame.paddingElements());
			break;

		case 2u:
			IntegralImage::createBorderedImage<T, TIntegral, 2u>(frame.constdata<T>(), linedIntegralFrame.data<TIntegral>(), frame.width(), frame.height(), border, frame.paddingElements(), linedIntegralFrame.paddingElements());
			break;

		case 3u:
			IntegralImage::createBorderedImage<T, TIntegral, 3u>(frame.constdata<T>(), linedIntegralFrame.data<TIntegral>(), frame.width(), frame.height(), border, frame.paddingElements(), linedIntegralFrame.paddingElements());
			break;

		case 4u:
			IntegralImage::createBorderedImage<T, TIntegral, 4u>(frame.constdata<T>(), linedIntegralFrame.data<TIntegral>(), frame.width(), frame.height(), border, frame.paddingElements(), linedIntegralFrame.paddingElements());
			break;

		default:
			ocean_assert(false && "Not supported channel number!");
			return Frame();
	}

	return linedIntegralFrame;
}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

#if defined(__aarch64__)

// the code within this scoped seems to be faster on ARM64 devices but slower on ARMv7 devices

void IntegralImage::createLinedImage1Channel8BitNEON(const uint8_t* source, uint32_t* integral, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements)
{
	ocean_assert(source != nullptr);
	ocean_assert(integral != nullptr);
	ocean_assert(width >= 8u && height != 0u);
	ocean_assert(width * height <= 16777216u);

	/*
	 * This is the resulting lined integral image:
	 *  ------------
	 * |000000000000|
	 * |0|----------|
	 * |0|          |
	 * |0| Integral |
	 * |0|          |
	 * |------------
	 *
	 * NEON-based implementation with scalar running sum optimization:
	 *
	 * For each block of 4 pixels we compute:
	 *
	 *   previous row:    T0 T1 T2 T3
	 *   source row:      C0 C1 C2 C3
	 *   running sum:     R
	 *
	 *   X0 = T0 + R + C0
	 *   X1 = T1 + R + C0 + C1
	 *   X2 = T2 + R + C0 + C1 + C2
	 *   X3 = T3 + R + C0 + C1 + C2 + C3
	 *
	 * Which can be written as:
	 *
	 *   [X0 X1 X2 X3] = [T0 T1 T2 T3] + [R R R R] + prefixSum([C0 C1 C2 C3])
	 *
	 * Where prefixSum is computed via parallel prefix pattern:
	 *   step 1: [C0, C1, C2, C3] + [0, C0, C1, C2] = [C0, C0+C1, C1+C2, C2+C3]
	 *   step 2: [C0, C0+C1, C1+C2, C2+C3] + [0, 0, C0, C0+C1] = [C0, C0+C1, C0+C1+C2, C0+C1+C2+C3]
	 */

	constexpr unsigned int blockSize = 8u;

	const uint32x4_t constant_zero_u_32x4 = vdupq_n_u32(0u);

	// entire top line will be set to zero
	memset(integral, 0x00, sizeof(uint32_t) * (width + 1u));

	uint32_t* integralLastRow = integral;

	integral += width + 1u + integralPaddingElements;

	for (unsigned int y = 0u; y < height; ++y)
	{
		*integral++ = 0u;
		integralLastRow++;

		// keep running sum as scalar - avoids memory round-trip
		uint32_t rowSum = 0u;

		unsigned int x = 0u;

		// main loop: process blockSize pixels at a time
		while (x + blockSize <= width)
		{
			const uint16x8_t source_16x8 = vmovl_u8(vld1_u8(source));

			const uint32x4_t lastRow_a_32x4 = vld1q_u32(integralLastRow + 0);
			const uint32x4_t lastRow_b_32x4 = vld1q_u32(integralLastRow + 4);

			// widen source to 32-bit
			uint32x4_t source_a_32x4 = vmovl_u16(vget_low_u16(source_16x8));
			uint32x4_t source_b_32x4 = vmovl_u16(vget_high_u16(source_16x8));

			// compute prefix sums for first 4 elements (parallel prefix pattern)
			uint32x4_t prefix_a_32x4 = vaddq_u32(source_a_32x4, vextq_u32(constant_zero_u_32x4, source_a_32x4, 3));
			prefix_a_32x4 = vaddq_u32(prefix_a_32x4, vextq_u32(constant_zero_u_32x4, prefix_a_32x4, 2));

			// add previous row and rowSum
			uint32x4_t result_a_32x4 = vaddq_u32(prefix_a_32x4, vaddq_u32(lastRow_a_32x4, vdupq_n_u32(rowSum)));

			// update rowSum with sum of first 4 pixels
			rowSum += vgetq_lane_u32(prefix_a_32x4, 3);

			// compute prefix sums for second 4 elements
			uint32x4_t prefix_b_32x4 = vaddq_u32(source_b_32x4, vextq_u32(constant_zero_u_32x4, source_b_32x4, 3));
			prefix_b_32x4 = vaddq_u32(prefix_b_32x4, vextq_u32(constant_zero_u_32x4, prefix_b_32x4, 2));

			// add previous row and rowSum
			uint32x4_t result_b_32x4 = vaddq_u32(prefix_b_32x4, vaddq_u32(lastRow_b_32x4, vdupq_n_u32(rowSum)));

			// update rowSum with sum of second 4 pixels
			rowSum += vgetq_lane_u32(prefix_b_32x4, 3);

			// store results
			vst1q_u32(integral + 0, result_a_32x4);
			vst1q_u32(integral + 4, result_b_32x4);

			source += blockSize;
			integral += blockSize;
			integralLastRow += blockSize;
			x += blockSize;
		}

		// process remaining 0-7 pixels with scalar code

		while (x < width)
		{
			rowSum += *source++;
			*integral++ = *integralLastRow++ + rowSum;
			++x;
		}

		source += sourcePaddingElements;
		integral += integralPaddingElements;
		integralLastRow += integralPaddingElements;
	}
}

#endif // defined(__aarch64__)

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

}

}
