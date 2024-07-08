/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameFilterSobel.h"
#include "ocean/cv/NEON.h"

namespace Ocean
{

namespace CV
{

Frame FrameFilterSobel::Comfort::filterHorizontalVertical(const Frame& frame, const FrameType::DataType responseDataType, Worker* worker)
{
	ocean_assert(frame.isValid());
	ocean_assert(frame.numberPlanes() == 1u && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	ocean_assert(responseDataType == FrameType::DT_SIGNED_INTEGER_8 || responseDataType == FrameType::DT_SIGNED_INTEGER_16);

	if (frame.width() >= 3u && frame.height() >= 3u && frame.numberPlanes() == 1u && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		if (responseDataType == FrameType::DT_SIGNED_INTEGER_8)
		{
			Frame sobel(FrameType(frame.frameType(), FrameType::genericPixelFormat<int8_t>(frame.channels() * 2u)));

			switch (frame.channels())
			{
				case 1u:
					FrameFilterSobel::filterHorizontalVertical8BitPerChannel<int8_t, 1u>(frame.constdata<uint8_t>(), sobel.data<int8_t>(), frame.width(), frame.height(), frame.paddingElements(), sobel.paddingElements(), worker);
					break;

				case 2u:
					FrameFilterSobel::filterHorizontalVertical8BitPerChannel<int8_t, 2u>(frame.constdata<uint8_t>(), sobel.data<int8_t>(), frame.width(), frame.height(), frame.paddingElements(), sobel.paddingElements(), worker);
					break;

				case 3u:
					FrameFilterSobel::filterHorizontalVertical8BitPerChannel<int8_t, 3u>(frame.constdata<uint8_t>(), sobel.data<int8_t>(), frame.width(), frame.height(), frame.paddingElements(), sobel.paddingElements(), worker);
					break;

				case 4u:
					FrameFilterSobel::filterHorizontalVertical8BitPerChannel<int8_t, 4u>(frame.constdata<uint8_t>(), sobel.data<int8_t>(), frame.width(), frame.height(), frame.paddingElements(), sobel.paddingElements(), worker);
					break;

				default:
					ocean_assert(false && "Invalid channel number!");
					return Frame();
			}

			return sobel;
		}

		if (responseDataType == FrameType::DT_SIGNED_INTEGER_16)
		{
			Frame sobel(FrameType(frame.frameType(), FrameType::genericPixelFormat<int16_t>(frame.channels() * 2u)));

			switch (frame.channels())
			{
				case 1u:
					FrameFilterSobel::filterHorizontalVertical8BitPerChannel<int16_t, 1u>(frame.constdata<uint8_t>(), sobel.data<int16_t>(), frame.width(), frame.height(), frame.paddingElements(), sobel.paddingElements(), worker);
					break;

				case 2u:
					FrameFilterSobel::filterHorizontalVertical8BitPerChannel<int16_t, 2u>(frame.constdata<uint8_t>(), sobel.data<int16_t>(), frame.width(), frame.height(), frame.paddingElements(), sobel.paddingElements(), worker);
					break;

				case 3u:
					FrameFilterSobel::filterHorizontalVertical8BitPerChannel<int16_t, 3u>(frame.constdata<uint8_t>(), sobel.data<int16_t>(), frame.width(), frame.height(), frame.paddingElements(), sobel.paddingElements(), worker);
					break;

				case 4u:
					FrameFilterSobel::filterHorizontalVertical8BitPerChannel<int16_t, 4u>(frame.constdata<uint8_t>(), sobel.data<int16_t>(), frame.width(), frame.height(), frame.paddingElements(), sobel.paddingElements(), worker);
					break;

				default:
					ocean_assert(false && "Invalid channel number!");
					return Frame();
			}

			return sobel;
		}
	}

	ocean_assert(false && "Invalid frame!");
	return Frame();
}

Frame FrameFilterSobel::Comfort::filter(const Frame& frame, const FrameType::DataType responseDataType, Worker* worker)
{
	ocean_assert(frame.isValid());
	ocean_assert(frame.numberPlanes() == 1u && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	ocean_assert(responseDataType == FrameType::DT_SIGNED_INTEGER_8 || responseDataType == FrameType::DT_SIGNED_INTEGER_16);

	if (frame.numberPlanes() == 1u && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		if (responseDataType == FrameType::DT_SIGNED_INTEGER_8)
		{
			Frame sobel(FrameType(frame.frameType(), FrameType::genericPixelFormat<int8_t>(frame.channels() * 4u)));

			switch (frame.channels())
			{
				case 1u:
					FrameFilterSobel::filter8BitPerChannel<int8_t, 1u>(frame.constdata<uint8_t>(), sobel.data<int8_t>(), frame.width(), frame.height(), frame.paddingElements(), sobel.paddingElements(), worker);
					break;

				case 2u:
					FrameFilterSobel::filter8BitPerChannel<int8_t, 2u>(frame.constdata<uint8_t>(), sobel.data<int8_t>(), frame.width(), frame.height(), frame.paddingElements(), sobel.paddingElements(), worker);
					break;

				case 3u:
					FrameFilterSobel::filter8BitPerChannel<int8_t, 3u>(frame.constdata<uint8_t>(), sobel.data<int8_t>(), frame.width(), frame.height(), frame.paddingElements(), sobel.paddingElements(), worker);
					break;

				case 4u:
					FrameFilterSobel::filter8BitPerChannel<int8_t, 4u>(frame.constdata<uint8_t>(), sobel.data<int8_t>(), frame.width(), frame.height(), frame.paddingElements(), sobel.paddingElements(), worker);
					break;

				default:
					ocean_assert(false && "Invalid channel number!");
					return Frame();
			}

			return sobel;
		}

		if (responseDataType == FrameType::DT_SIGNED_INTEGER_16)
		{
			Frame sobel(FrameType(frame.frameType(), FrameType::genericPixelFormat<int16_t>(frame.channels() * 4u)));

			switch (frame.channels())
			{
				case 1u:
					FrameFilterSobel::filter8BitPerChannel<int16_t, 1u>(frame.constdata<uint8_t>(), sobel.data<int16_t>(), frame.width(), frame.height(), frame.paddingElements(), sobel.paddingElements(), worker);
					break;

				case 2u:
					FrameFilterSobel::filter8BitPerChannel<int16_t, 2u>(frame.constdata<uint8_t>(), sobel.data<int16_t>(), frame.width(), frame.height(), frame.paddingElements(), sobel.paddingElements(), worker);
					break;

				case 3u:
					FrameFilterSobel::filter8BitPerChannel<int16_t, 3u>(frame.constdata<uint8_t>(), sobel.data<int16_t>(), frame.width(), frame.height(), frame.paddingElements(), sobel.paddingElements(), worker);
					break;

				case 4u:
					FrameFilterSobel::filter8BitPerChannel<int16_t, 4u>(frame.constdata<uint8_t>(), sobel.data<int16_t>(), frame.width(), frame.height(), frame.paddingElements(), sobel.paddingElements(), worker);
					break;

				default:
					ocean_assert(false && "Invalid channel number!");
					return Frame();
			}

			return sobel;
		}
	}

	ocean_assert(false && "Invalid frame!");
	return Frame();
}

void FrameFilterSobel::filterHorizontalVertical3Squared1Channel8BitRow(const uint8_t* row, const unsigned int width, const unsigned int elements, const unsigned int paddingElements, int16_t* responsesXX, int16_t* responsesYY, int16_t* responsesXY)
{
	ocean_assert(row != nullptr);

	ocean_assert(width >= 10u);
	ocean_assert(elements >= 8u);

	ocean_assert(responsesXX != nullptr);
	ocean_assert(responsesYY != nullptr);
	ocean_assert(responsesXY != nullptr);

	// horizontal filter response (X)
	//      0 1 2
	// A | -1 0 1 |
	// B | -2 0 2 |
	// C | -1 0 1 |

	// vertical filter response (Y)
	//      0  1  2
	// A | -1 -2 -1 |
	// B |  0  0  0 |
	// C |  1  2  1 |

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	const unsigned int strideElements = width + paddingElements;

	const int16x8_t constant_s_two_16x8 = vdupq_n_s16(2);

	for (unsigned int x = 0u; x < elements; x += 8u)
	{
		if (x + 8u > elements)
		{
			// the last iteration will not fit into the output frame,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(x >= 8u && elements > 8u);
			const unsigned int newX = elements - 8u;

			ocean_assert(x > newX);
			const unsigned int offset = x - newX;
			ocean_assert(offset < 8u);

			row -= offset;
			responsesXX -= offset;
			responsesYY -= offset;
			responsesXY -= offset;

			// the for loop will stop after this iteration
			ocean_assert(!(x + 8u < elements));
		}

		const uint8x8_t A0_u_8x8 = vld1_u8(row - strideElements + 0);
		const uint8x8_t A1_u_8x8 = vld1_u8(row - strideElements + 1);
		const uint8x8_t A2_u_8x8 = vld1_u8(row - strideElements + 2);

		const uint8x8_t B0_u_8x8 = vld1_u8(row + 0);
		const uint8x8_t B2_u_8x8 = vld1_u8(row + 2);

		const uint8x8_t C0_u_8x8 = vld1_u8(row + strideElements + 0);
		const uint8x8_t C1_u_8x8 = vld1_u8(row + strideElements + 1);
		const uint8x8_t C2_u_8x8 = vld1_u8(row + strideElements + 2);


		// A2 - A0
		int16x8_t horizontal_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(A2_u_8x8, A0_u_8x8));

		// 2 * (B2 - B0)
		horizontal_s_16x8 = vmlaq_s16(horizontal_s_16x8, vreinterpretq_s16_u16(vsubl_u8(B2_u_8x8, B0_u_8x8)), constant_s_two_16x8);

		// C2 - C0
		horizontal_s_16x8 = vaddq_s16(horizontal_s_16x8, vreinterpretq_s16_u16(vsubl_u8(C2_u_8x8, C0_u_8x8)));

		const int8x8_t horizontal_s_8x8 = vqrshrn_n_s16(horizontal_s_16x8, 3); // as we apply a rounding shift, we have to use a saturation as (255 * 4) / 8 = 127.5 = 128 = -128 (for 8 bit signed int8_t)


		// C0 - A0
		int16x8_t vertical_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(C0_u_8x8, A0_u_8x8));

		// 2 * (C1 - A1)
		vertical_s_16x8 = vmlaq_s16(vertical_s_16x8, vreinterpretq_s16_u16(vsubl_u8(C1_u_8x8, A1_u_8x8)), constant_s_two_16x8);

		// C2 - A2
		vertical_s_16x8 = vaddq_s16(vertical_s_16x8, vreinterpretq_s16_u16(vsubl_u8(C2_u_8x8, A2_u_8x8)));

		const int8x8_t vertical_s_8x8 = vqrshrn_n_s16(vertical_s_16x8, 3);


		// horizontal * horizontal
		vst1q_s16(responsesXX, vmull_s8(horizontal_s_8x8, horizontal_s_8x8));

		// vertical * vertical
		vst1q_s16(responsesYY, vmull_s8(vertical_s_8x8, vertical_s_8x8));

		// horizontal * vertical
		vst1q_s16(responsesXY, vmull_s8(horizontal_s_8x8, vertical_s_8x8));


		row += 8;
		responsesXX += 8;
		responsesYY += 8;
		responsesXY += 8;
	}

#else

	int16_t responses_XX_YY_XY[3];

	for (unsigned int x = 0u; x < elements; ++x)
	{
		filterPixelCoreHorizontalVertical3Squared1Channel8Bit<int16_t, 8>(++row, width, responses_XX_YY_XY, paddingElements);

		*responsesXX++ = responses_XX_YY_XY[0];
		*responsesYY++ = responses_XX_YY_XY[1];
		*responsesXY++ = responses_XX_YY_XY[2];
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

}

}

}
