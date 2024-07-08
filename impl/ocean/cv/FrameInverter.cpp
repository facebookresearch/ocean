/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameInverter.h"

#include "ocean/cv/NEON.h"

namespace Ocean
{

namespace CV
{

bool FrameInverter::invert(const Frame& source, Frame& target, Worker* worker)
{
	ocean_assert(source.isValid());

	if (source.numberPlanes() == 1u && source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		if (!target.set(source.frameType(), false /*forceOwner*/, true /*forceWritable*/))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		invert8BitPerChannel(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.channels(), source.paddingElements(), target.paddingElements(), worker);

		return true;
	}

	ocean_assert(false && "Not supported pixel format!");
	return false;
}

bool FrameInverter::invert(Frame& frame, Worker* worker)
{
	ocean_assert(frame);

	Frame tmpFrame;

	if (!invert(frame, tmpFrame, worker))
	{
		return false;
	}

	frame = std::move(tmpFrame);

	return true;
}

void FrameInverter::invert8BitPerChannelSubset(const uint8_t* source, uint8_t* target, const unsigned int horizontalElements, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(horizontalElements != 0u);

	const unsigned int sourceStrideElements = horizontalElements + sourcePaddingElements;
	const unsigned int targetStrideElements = horizontalElements + targetPaddingElements;

	source += sourceStrideElements * firstRow;
	target += targetStrideElements * firstRow;

	for (unsigned int y = 0u; y < numberRows; ++y)
	{
		for (unsigned int n = 0u; n < horizontalElements; ++n)
		{
			target[n] = 0xFF - source[n];
		}

		source += sourceStrideElements;
		target += targetStrideElements;
	}
}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

void FrameInverter::invert8BitPerChannelSubsetNEON(const uint8_t* source, uint8_t* target, const unsigned int horizontalElements, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(horizontalElements >= 32u);

	const unsigned int sourceStrideElements = horizontalElements + sourcePaddingElements;
	const unsigned int targetStrideElements = horizontalElements + targetPaddingElements;

	source += sourceStrideElements * firstRow;
	target += targetStrideElements * firstRow;

	const uint8x16_t constant_u_8x16_255 = vdupq_n_u8(255u);

	for (unsigned int y = 0u; y < numberRows; ++y)
	{
		for (unsigned int n = 0u; n < horizontalElements; n += 32u)
		{
			if (n + 32u > horizontalElements)
			{
				// the last iteration will not fit into the output frame,
				// so we simply shift x left by some pixels (at most 15) and we will calculate some pixels again

				ocean_assert(n >= 32u && horizontalElements > 32u);
				const unsigned int newN = horizontalElements - 32u;

				ocean_assert(n > newN);
				n = newN;

				// the for loop will stop after this iteration
				ocean_assert(!(n + 32u < horizontalElements));
			}

			const uint8x16_t sourceA_u_8x16 = vld1q_u8(source + n + 0u);
			const uint8x16_t sourceB_u_8x16 = vld1q_u8(source + n + 16u);

			const uint8x16_t invertedSourceA_u_8x16 = vsubq_u8(constant_u_8x16_255, sourceA_u_8x16);
			const uint8x16_t invertedSourceB_u_8x16 = vsubq_u8(constant_u_8x16_255, sourceB_u_8x16);

			vst1q_u8(target + n + 0u, invertedSourceA_u_8x16);
			vst1q_u8(target + n + 16u, invertedSourceB_u_8x16);
		}

		source += sourceStrideElements;
		target += targetStrideElements;
	}
}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

}

}
