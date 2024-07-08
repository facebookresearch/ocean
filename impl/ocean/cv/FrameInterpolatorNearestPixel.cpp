/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameInterpolatorNearestPixel.h"

namespace Ocean
{

namespace CV
{

bool FrameInterpolatorNearestPixel::Comfort::resize(const Frame& source, Frame& target, Worker* worker)
{
	ocean_assert(source.isValid() && target.isValid());
	ocean_assert(source.haveIntersectingMemory(target) == false);

	if (source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && target.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && source.pixelFormat() == target.pixelFormat() && source.pixelOrigin() == target.pixelOrigin() && source.numberPlanes() == 1u)
	{
		switch (source.channels())
		{
			case 1u:
				FrameInterpolatorNearestPixel::resize<uint8_t, 1u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
				return true;

			case 2u:
				FrameInterpolatorNearestPixel::resize<uint8_t, 2u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
				return true;

			case 3u:
				FrameInterpolatorNearestPixel::resize<uint8_t, 3u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
				return true;

			case 4u:
				FrameInterpolatorNearestPixel::resize<uint8_t, 4u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
				return true;
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

bool FrameInterpolatorNearestPixel::Comfort::affine(const Frame& input, Frame& output, const SquareMatrix3& input_A_output, const uint8_t* borderColor, Worker* worker, const PixelPositionI& outputOrigin)
{
	ocean_assert(input.isValid() && output.isValid());

	if (input.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && output.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && input.numberPlanes() == 1u && output.isValid() && output.pixelFormat() == input.pixelFormat() && output.pixelOrigin() == input.pixelOrigin())
	{
		switch (input.channels())
		{
			case 1u:
				FrameInterpolatorNearestPixel::affine8BitPerChannel<1u>(input.constdata<uint8_t>(), input.width(), input.height(), input_A_output, borderColor, output.data<uint8_t>(), outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), worker);
				return true;

			case 2u:
				FrameInterpolatorNearestPixel::affine8BitPerChannel<2u>(input.constdata<uint8_t>(), input.width(), input.height(), input_A_output, borderColor, output.data<uint8_t>(), outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), worker);
				return true;

			case 3u:
				FrameInterpolatorNearestPixel::affine8BitPerChannel<3u>(input.constdata<uint8_t>(), input.width(), input.height(), input_A_output, borderColor, output.data<uint8_t>(), outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), worker);
				return true;

			case 4u:
				FrameInterpolatorNearestPixel::affine8BitPerChannel<4u>(input.constdata<uint8_t>(), input.width(), input.height(), input_A_output, borderColor, output.data<uint8_t>(), outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), worker);
				return true;
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

bool FrameInterpolatorNearestPixel::Comfort::homography(const Frame& input, Frame& output, const SquareMatrix3& input_H_output, const void* borderColor, Worker* worker, const PixelPositionI& outputOrigin)
{
	ocean_assert(input.isValid() && output.isValid());

	if (FrameType::arePixelFormatsCompatible(input.pixelFormat(), output.pixelFormat()) && input.numberPlanes() == 1u && input.pixelOrigin() == output.pixelOrigin())
	{
		if (input.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 || input.dataType() == FrameType::DT_SIGNED_INTEGER_8)
		{
			const uint8_t* const borderColor_u8 = (const uint8_t*)(borderColor);

			switch (input.channels())
			{
				case 1u:
					FrameInterpolatorNearestPixel::homography<uint8_t, 1u>(input.constdata<uint8_t>(), input.width(), input.height(), input_H_output, borderColor_u8, output.data<uint8_t>(), outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), worker);
					return true;

				case 2u:
					FrameInterpolatorNearestPixel::homography<uint8_t, 2u>(input.constdata<uint8_t>(), input.width(), input.height(), input_H_output, borderColor_u8, output.data<uint8_t>(), outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), worker);
					return true;

				case 3u:
					FrameInterpolatorNearestPixel::homography<uint8_t, 3u>(input.constdata<uint8_t>(), input.width(), input.height(), input_H_output, borderColor_u8, output.data<uint8_t>(), outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), worker);
					return true;

				case 4u:
					FrameInterpolatorNearestPixel::homography<uint8_t, 4u>(input.constdata<uint8_t>(), input.width(), input.height(), input_H_output, borderColor_u8, output.data<uint8_t>(), outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), worker);
					return true;
			}
		}
		else if (input.dataType() == FrameType::DT_SIGNED_FLOAT_32 || input.dataType() == FrameType::DT_UNSIGNED_INTEGER_32 || input.dataType() == FrameType::DT_SIGNED_INTEGER_32)
		{
			ocean_assert(size_t(borderColor) % sizeof(uint32_t) == 0); // the border color is most likely not provided as correct data type

			const uint32_t* const borderColor_u32 = (const uint32_t*)(borderColor);

			switch (input.channels())
			{
				case 1u:
					FrameInterpolatorNearestPixel::homography<uint32_t, 1u>(input.constdata<uint32_t>(), input.width(), input.height(), input_H_output, borderColor_u32, output.data<uint32_t>(), outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), worker);
					return true;

				case 2u:
					FrameInterpolatorNearestPixel::homography<uint32_t, 2u>(input.constdata<uint32_t>(), input.width(), input.height(), input_H_output, borderColor_u32, output.data<uint32_t>(), outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), worker);
					return true;

				case 3u:
					FrameInterpolatorNearestPixel::homography<uint32_t, 3u>(input.constdata<uint32_t>(), input.width(), input.height(), input_H_output, borderColor_u32, output.data<uint32_t>(), outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), worker);
					return true;

				case 4u:
					FrameInterpolatorNearestPixel::homography<uint32_t, 4u>(input.constdata<uint32_t>(), input.width(), input.height(), input_H_output, borderColor_u32, output.data<uint32_t>(), outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), worker);
					return true;
			}
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

bool FrameInterpolatorNearestPixel::Comfort::homographyMask(const Frame& input, Frame& output, Frame& outputMask, const SquareMatrix3& input_H_output, Worker* worker, const uint8_t maskValue, const PixelPositionI& outputOrigin)
{
	ocean_assert(input.isValid() && output.isValid() && outputMask.isValid());

	if (input.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && input.numberPlanes() == 1u && output.isValid() && output.pixelFormat() == input.pixelFormat() && output.pixelOrigin() == input.pixelOrigin())
	{
		ocean_assert(FrameType(output, FrameType::FORMAT_Y8) == outputMask.frameType());

		switch (input.channels())
		{
			case 1u:
				FrameInterpolatorNearestPixel::homographyMask8BitPerChannel<1u>(input.constdata<uint8_t>(), input.width(), input.height(), input.paddingElements(), input_H_output, output.data<uint8_t>(), output.paddingElements(), outputMask.data<uint8_t>(), outputMask.paddingElements(), outputOrigin, output.width(), output.height(), worker, maskValue);
				return true;

			case 2u:
				FrameInterpolatorNearestPixel::homographyMask8BitPerChannel<2u>(input.constdata<uint8_t>(), input.width(), input.height(), input.paddingElements(), input_H_output, output.data<uint8_t>(), output.paddingElements(),outputMask.data<uint8_t>(), outputMask.paddingElements(), outputOrigin, output.width(), output.height(), worker, maskValue);
				return true;

			case 3u:
				FrameInterpolatorNearestPixel::homographyMask8BitPerChannel<3u>(input.constdata<uint8_t>(), input.width(), input.height(), input.paddingElements(), input_H_output, output.data<uint8_t>(), output.paddingElements(),outputMask.data<uint8_t>(), outputMask.paddingElements(), outputOrigin, output.width(), output.height(), worker, maskValue);
				return true;

			case 4u:
				FrameInterpolatorNearestPixel::homographyMask8BitPerChannel<4u>(input.constdata<uint8_t>(), input.width(), input.height(), input.paddingElements(), input_H_output, output.data<uint8_t>(), output.paddingElements(),outputMask.data<uint8_t>(), outputMask.paddingElements(), outputOrigin, output.width(), output.height(), worker, maskValue);
				return true;
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

bool FrameInterpolatorNearestPixel::Comfort::transform(const Frame& input, Frame& output, const LookupTable& lookupTable, const bool offset, const uint8_t* borderColor, Worker* worker)
{
	ocean_assert(!lookupTable.isEmpty() && input.isValid());

	ocean_assert(!output.isValid() || (output.width() == (unsigned int)(lookupTable.sizeX()) && output.height() == (unsigned int)(lookupTable.sizeY())));

	if (input.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && input.numberPlanes() == 1u)
	{
		if (!output.set(FrameType(input.frameType(), (unsigned int)(lookupTable.sizeX()), (unsigned int)(lookupTable.sizeY())), false /*forceOwner*/, true /*forceWritable*/))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		switch (input.channels())
		{
			case 1u:
				FrameInterpolatorNearestPixel::transform8BitPerChannel<1u>(input.constdata<uint8_t>(), input.width(), input.height(), lookupTable, offset, borderColor, output.data<uint8_t>(), input.paddingElements(), output.paddingElements(), worker);
				return true;

			case 2u:
				FrameInterpolatorNearestPixel::transform8BitPerChannel<2u>(input.constdata<uint8_t>(), input.width(), input.height(), lookupTable, offset, borderColor, output.data<uint8_t>(), input.paddingElements(), output.paddingElements(), worker);
				return true;

			case 3u:
				FrameInterpolatorNearestPixel::transform8BitPerChannel<3u>(input.constdata<uint8_t>(), input.width(), input.height(), lookupTable, offset, borderColor, output.data<uint8_t>(), input.paddingElements(), output.paddingElements(), worker);
				return true;

			case 4u:
				FrameInterpolatorNearestPixel::transform8BitPerChannel<4u>(input.constdata<uint8_t>(), input.width(), input.height(), lookupTable, offset, borderColor, output.data<uint8_t>(), input.paddingElements(), output.paddingElements(), worker);
				return true;
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

bool FrameInterpolatorNearestPixel::Comfort::transformMask(const Frame& input, Frame& output, Frame& outputMask, const LookupTable& lookupTable, const bool offset, Worker* worker, const uint8_t maskValue)
{
	ocean_assert(!lookupTable.isEmpty() && input.isValid());

	ocean_assert(!output.isValid() || (output.width() == (unsigned int)(lookupTable.sizeX()) && output.height() == (unsigned int)(lookupTable.sizeY())));

	if (input.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && input.numberPlanes() == 1u)
	{
		if (!output.set(FrameType(input.frameType(), (unsigned int)(lookupTable.sizeX()), (unsigned int)(lookupTable.sizeY())), false /*forceOwner*/, true /*forceWritable*/))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		if (!outputMask.set(FrameType(output, FrameType::FORMAT_Y8), false /*forceOwner*/, true /*forceWritable*/))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		switch (input.channels())
		{
			case 1u:
				FrameInterpolatorNearestPixel::transformMask8BitPerChannel<1u>(input.constdata<uint8_t>(), input.width(), input.height(), input.paddingElements(), lookupTable, offset, output.data<uint8_t>(), output.paddingElements(), outputMask.data<uint8_t>(), outputMask.paddingElements(), worker, maskValue);
				return true;

			case 2u:
				FrameInterpolatorNearestPixel::transformMask8BitPerChannel<2u>(input.constdata<uint8_t>(), input.width(), input.height(), input.paddingElements(), lookupTable, offset, output.data<uint8_t>(), output.paddingElements(), outputMask.data<uint8_t>(), outputMask.paddingElements(), worker, maskValue);
				return true;

			case 3u:
				FrameInterpolatorNearestPixel::transformMask8BitPerChannel<3u>(input.constdata<uint8_t>(), input.width(), input.height(), input.paddingElements(), lookupTable, offset, output.data<uint8_t>(), output.paddingElements(), outputMask.data<uint8_t>(), outputMask.paddingElements(), worker, maskValue);
				return true;

			case 4u:
				FrameInterpolatorNearestPixel::transformMask8BitPerChannel<4u>(input.constdata<uint8_t>(), input.width(), input.height(), input.paddingElements(), lookupTable, offset, output.data<uint8_t>(), output.paddingElements(), outputMask.data<uint8_t>(), outputMask.paddingElements(), worker, maskValue);
				return true;
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

void FrameInterpolatorNearestPixel::SpecialCases::resize400x400To224x224_8BitPerChannel(const uint8_t* const source, uint8_t* const target, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements)
{
	ocean_assert(source != nullptr && target != nullptr);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10 && defined(__aarch64__)

	// every 14 elements, the lookup locations and the interpolation factors repeat; 25 pixels will be resized to 14 pixels

	/*
	 * 0   1   2   3   4   5   6   7   8   9   10   11   12   13   14   15   16   17   18   19   20   21   22   23   24
	 * X   X       X       X       X   X       X         X         X         X    X         X         X         X

	 *
	 * ^                                   ^
	 * |                                   |
	 * sourceA                             sourceB
	 *                                     0   1    2    3    4    5    6    7    8    9    10   11   12   13   14   15
	 *
	 */

	constexpr uint8_t topRowOffsets[14] = {0u, 1u, 3u, 5u, 7u, 8u, 10u, 12u, 14u, 16u, 17u, 19u, 21u, 23u};

	constexpr uint8x16_t shuffleA_u_8x16 = {255u, 255u, 255u, 255u, 255u, 255u, 255u, 0u, 1u, 3u, 5u, 7u, 8u, 10u, 12u, 14u};
	constexpr uint8x16_t shuffleB_u_8x16 = {7u, 8u, 10u, 12u, 14u, 255u, 255u, 255u, 255u, 255u, 255u, 255u, 255u, 255u, 255u, 255u};

	const unsigned int sourceStrideElements = 400u + sourcePaddingElements;
	const unsigned int targetStrideElements = 224u + targetPaddingElements;

	for (unsigned int yBlock = 0u; yBlock < 16u; ++yBlock)
	{
		for (unsigned int n = 0u; n < 14u; ++n)
		{
			const unsigned int ySource = yBlock * 25u + topRowOffsets[n];
			const unsigned int yTarget = yBlock * 14u + n;

			ocean_assert(yTarget < 224u);

			const uint8_t* sourceRow = source + ySource * sourceStrideElements;

			uint8_t* targetRow = target + yTarget * targetStrideElements;

			for (unsigned int xBlock = 0u; xBlock < 16u; ++xBlock)
			{
				uint8x16_t sourceA_u_8x16 = vld1q_u8(sourceRow + 0u);
				uint8x16_t sourceB_u_8x16 = vld1q_u8(sourceRow + 9u);

				sourceA_u_8x16 = vqtbl1q_u8(sourceA_u_8x16, shuffleA_u_8x16);
				sourceB_u_8x16 = vqtbl1q_u8(sourceB_u_8x16, shuffleB_u_8x16);

				if (xBlock != 15u)
				{
					const uint8x16_t result_u_8x16 =  vextq_u8(sourceA_u_8x16, sourceB_u_8x16, 7);

					vst1q_u8(targetRow, result_u_8x16); // overwriting 16 elements, although 14 are valid
				}
				else
				{
					const uint8_t intermediate0 = *(targetRow - 2); // storing two bytes which we will overwrite with invalid data
					const uint8_t intermediate1 = *(targetRow - 1);

					const uint8x16_t result_u_8x16 =  vextq_u8(sourceA_u_8x16, sourceB_u_8x16, 5);

					vst1q_u8(targetRow - 2, result_u_8x16); // overwriting all valid 16 elements

					*(targetRow - 2) = intermediate0; // writing the valid data back again
					*(targetRow - 1) = intermediate1;
				}

				sourceRow += 25;
				targetRow += 14;
			}
		}
	}

#else // OCEAN_HARDWARE_NEON_VERSION

	resize<uint8_t, 1u>(source, target, 400u, 400u, 224u, 224u, sourcePaddingElements, targetPaddingElements);

#endif // OCEAN_HARDWARE_NEON_VERSION
}

bool FrameInterpolatorNearestPixel::coversHomographyInputFrame(const unsigned int inputWidth, const unsigned int inputHeight, const unsigned int outputWidth, const unsigned int outputHeight, const SquareMatrix3& homography, const int outputOriginX, const int outputOriginY)
{
	ocean_assert(inputWidth >= 1u && inputHeight >= 1u);
	ocean_assert(outputWidth >= 1u && outputHeight >= 1u);

	const Vector2 inputPosition0(homography * Vector2(Scalar(outputOriginX), Scalar(outputOriginY)));
	const Vector2 inputPosition1(homography * Vector2(Scalar(outputOriginX + int(outputWidth) - 1), Scalar(outputOriginY)));
	const Vector2 inputPosition2(homography * Vector2(Scalar(outputOriginX), Scalar(outputOriginY + int(outputHeight) - 1)));
	const Vector2 inputPosition3(homography * Vector2(Scalar(outputOriginX + int(outputWidth) - 1), Scalar(outputOriginY + int(outputHeight) - 1)));

	return (unsigned int)Numeric::round32(inputPosition0.x()) < inputWidth && (unsigned int)Numeric::round32(inputPosition0.y()) < inputHeight
				&& (unsigned int)Numeric::round32(inputPosition1.x()) < inputWidth && (unsigned int)Numeric::round32(inputPosition1.y()) < inputHeight
				&& (unsigned int)Numeric::round32(inputPosition2.x()) < inputWidth && (unsigned int)Numeric::round32(inputPosition2.y()) < inputHeight
				&& (unsigned int)Numeric::round32(inputPosition3.x()) < inputWidth && (unsigned int)Numeric::round32(inputPosition3.y()) < inputHeight;
}

}

}
