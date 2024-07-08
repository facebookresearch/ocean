/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_INTERPOLATOR_BILINEAR_NEON_H
#define META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_INTERPOLATOR_BILINEAR_NEON_H

#include "ocean/cv/advanced/Advanced.h"

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

#include "ocean/cv/NEON.h"

#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

/**
 * This class implements advanced bilinear frame interpolation functions using NEON extensions.
 * @ingroup cvadvanced
 */
class OCEAN_CV_ADVANCED_EXPORT AdvancedFrameInterpolatorBilinearNEON
{
	protected:

		/**
		 * This class allows to specialize functions for individual channels.
		 * @tparam tChannels Specifies the number of channels for the given frames, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		class SpecializedForChannels
		{
			public:

				/**
				 * Interpolates the content of a square image patch with sub-pixel accuracy inside a given image and stores the interpolated data into a buffer.
				 * The center of a pixel is expected to be located at the top-left corner of a pixel.
				 * @param imageTopLeft The pointer to the top-left position of the image, must be valid
				 * @param imageStrideElements The number of elements between two consecutive image rows (including padding), in elements, with range [tChannels * tPatchSize, infinity)
				 * @param buffer The target buffer with `tChannels * tSize * tSize` elements, must be valid
				 * @param factorRight The interpolation factor for the right pixels, with range [0, 128]
				 * @param factorBottom The interpolation factor for the bottom pixels, with range [0, 128]
				 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
				 */
				template <unsigned int tPatchSize>
				static inline void interpolateSquarePatch8BitPerChannel(const uint8_t* imageTopLeft, const unsigned int imageStrideElements, uint8_t* buffer, const unsigned int factorRight, const unsigned int factorBottom);
		};

	public:

		/**
		 * Interpolates the content of a square image patch with sub-pixel accuracy inside a given image and stores the interpolated data into a buffer.
		 * The center of a pixel is expected to be located at the top-left corner of a pixel.
		 * @param image The image in which the interpolated patch is located, must be valid
		 * @param width The width of the image, in pixel, with range [tPatchSize + 1, infinity)
		 * @param imagePaddingElements The number of padding elements at the end of each image row, in elements, with range [0, infinity)
		 * @param buffer The target buffer with `tChannels * tSize * tSize` elements, must be valid
		 * @param position The center position of the square region in the image, with range [tPatchSize/2, width - tPatchSize/2 - 1)x[tPatchSize/2, height - tPatchSize/2 - 1)
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [5, infinity), must be odd
		 * @tparam tPixelCenter The pixel center to be used during interpolation, either 'PC_TOP_LEFT' or 'PC_CENTER'
		 * @tparam TScalar The scalar data type of the sub-pixel position
		 */
		template <unsigned int tChannels, unsigned int tPatchSize, PixelCenter tPixelCenter = PC_TOP_LEFT, typename TScalar = Scalar>
		static inline void interpolateSquarePatch8BitPerChannel(const uint8_t* const image, const unsigned int width, const unsigned int imagePaddingElements, uint8_t* buffer, const VectorT2<TScalar>& position);
};

template <>
template <unsigned int tPatchSize>
inline void AdvancedFrameInterpolatorBilinearNEON::SpecializedForChannels<1u>::interpolateSquarePatch8BitPerChannel(const uint8_t* imageTopLeft, const unsigned int imageStrideElements, uint8_t* buffer, const unsigned int factorRight, const unsigned int factorBottom)
{
	ocean_assert(imageTopLeft != nullptr && buffer != nullptr);
	ocean_assert(imageStrideElements >= 1u * tPatchSize);

	ocean_assert(factorRight <= 128u && factorBottom <= 128u);

	const unsigned int factorLeft = 128u - factorRight;
	const unsigned int factorTop = 128u - factorBottom;

	constexpr unsigned int blocks15 = tPatchSize / 15u;
	constexpr unsigned int remainingAfterBlocks15 = tPatchSize % 15u;

	constexpr bool partialBlock15 = remainingAfterBlocks15 > 10u;
	constexpr unsigned int remainingAfterPartialBlock15 = partialBlock15 ? 0u : remainingAfterBlocks15;

	constexpr bool block7 = remainingAfterPartialBlock15 >= 7u;
	constexpr unsigned int remainingAfterBlock7 = remainingAfterPartialBlock15 % 7u;

	constexpr bool partialBlock7 = remainingAfterBlock7 >= 3u;
	constexpr unsigned int remainingAfterPartialBlock7 = partialBlock7 ? 0u : remainingAfterBlock7;

	constexpr unsigned int blocks1 = remainingAfterPartialBlock7;

	// L R L R L R L R
	const uint8x8_t factorsLeftRight_u_8x8 = vreinterpret_u8_u16(vdup_n_u16(uint16_t(factorLeft | (factorRight << 8u))));

	const uint32x4_t factorsTop_u_32x4 = vdupq_n_u32(factorTop);
	const uint32x4_t factorsBottom_u_32x4 = vdupq_n_u32(factorBottom);

	for (unsigned int y = 0u; y < tPatchSize; ++y)
	{
		for (unsigned int x = 0u; x < blocks15; ++x)
		{
			const uint8x16_t top_u_8x16 = vld1q_u8(imageTopLeft);
			const uint8x16_t bottom_u_8x16 = vld1q_u8(imageTopLeft + imageStrideElements);

			// top[1], top[2], top[3], ..., top[14], top[15], X
			const uint8x16_t topB_u_8x16 = vextq_u8(top_u_8x16, vreinterpretq_u8_u32(factorsTop_u_32x4), 1);
			const uint8x16_t bottomB_u_8x16 = vextq_u8(bottom_u_8x16, vreinterpretq_u8_u32(factorsTop_u_32x4), 1);


			// top[0] * L, top[1] * R, top[2] * L, ...
			const uint32x4_t topLowA_u_32x4 = vpaddlq_u16(vmull_u8(vget_low_u8(top_u_8x16), factorsLeftRight_u_8x8));
			const uint32x4_t bottomLowA_u_32x4 = vpaddlq_u16(vmull_u8(vget_low_u8(bottom_u_8x16), factorsLeftRight_u_8x8));

			// top[1] * L, top[2] * R, top[3] * L, ...
			const uint32x4_t topLowB_u_32x4 = vpaddlq_u16(vmull_u8(vget_low_u8(topB_u_8x16), factorsLeftRight_u_8x8));
			const uint32x4_t bottomLowB_u_32x4 = vpaddlq_u16(vmull_u8(vget_low_u8(bottomB_u_8x16), factorsLeftRight_u_8x8));

			// top[8] * L, top[9] * R, top[10] * L, ...
			const uint32x4_t topHighA_u_32x4 = vpaddlq_u16(vmull_u8(vget_high_u8(top_u_8x16), factorsLeftRight_u_8x8));
			const uint32x4_t bottomHighA_u_32x4 = vpaddlq_u16(vmull_u8(vget_high_u8(bottom_u_8x16), factorsLeftRight_u_8x8));

			// top[9] * L, top[10] * R, top[11] * L, ...
			const uint32x4_t topHighB_u_32x4 = vpaddlq_u16(vmull_u8(vget_high_u8(topB_u_8x16), factorsLeftRight_u_8x8));
			const uint32x4_t bottomHighB_u_32x4 = vpaddlq_u16(vmull_u8(vget_high_u8(bottomB_u_8x16), factorsLeftRight_u_8x8));


			// (top[0] * T + bottom[0] * B + 8192) / 16384, (top[1] * T + bottom[1] * B + 8192) / 16384, ...
			const uint16x4_t resultLowA_u_16x4 = vrshrn_n_u32(vmlaq_u32(vmulq_u32(topLowA_u_32x4, factorsTop_u_32x4), bottomLowA_u_32x4, factorsBottom_u_32x4), 14);
			const uint16x4_t resultHighA_u_16x4 = vrshrn_n_u32(vmlaq_u32(vmulq_u32(topHighA_u_32x4, factorsTop_u_32x4), bottomHighA_u_32x4, factorsBottom_u_32x4), 14);

			const uint16x4_t resultLowB_u_16x4 = vrshrn_n_u32(vmlaq_u32(vmulq_u32(topLowB_u_32x4, factorsTop_u_32x4), bottomLowB_u_32x4, factorsBottom_u_32x4), 14);
			const uint16x4_t resultHighB_u_16x4 = vrshrn_n_u32(vmlaq_u32(vmulq_u32(topHighB_u_32x4, factorsTop_u_32x4), bottomHighB_u_32x4, factorsBottom_u_32x4), 14);

			const uint16x8_t resultA_u_16x8 = vcombine_u16(resultLowA_u_16x4, resultHighA_u_16x4);
			const uint16x8_t resultB_u_16x8 = vcombine_u16(resultLowB_u_16x4, resultHighB_u_16x4);

			// resultA[0], resultB[0], resultA[1], resultB[1], resultA[2], ...
			const uint8x16_t result_u_8x16 = vreinterpretq_u8_u16(vsliq_n_u16(resultA_u_16x8, resultB_u_16x8, 8));


			const bool isLastBlock = (y + 1u == tPatchSize) && (x + 1u == blocks15) && (!block7 && !partialBlock7 && blocks1 == 0u);

			if (isLastBlock)
			{
				uint8_t tempBuffer[16];
				vst1q_u8(tempBuffer, result_u_8x16);

				memcpy(buffer, &tempBuffer, 15);
			}
			else
			{
				vst1q_u8(buffer, result_u_8x16);
			}

			imageTopLeft += 15;
			buffer += 15;
		}

		if constexpr (partialBlock15)
		{
			ocean_assert(!block7 && !partialBlock7 && blocks1 == 0u);

			uint8x16_t top_u_8x16;
			uint8x16_t bottom_u_8x16;

			if (y < tPatchSize - 1u)
			{
				top_u_8x16 = vld1q_u8(imageTopLeft);
				bottom_u_8x16 = vld1q_u8(imageTopLeft + imageStrideElements);
			}
			else
			{
				constexpr unsigned int overlapping = 16u - (remainingAfterBlocks15 + 1u);

				top_u_8x16 = vld1q_u8(imageTopLeft - overlapping);
				bottom_u_8x16 = vld1q_u8(imageTopLeft + imageStrideElements - overlapping);

				top_u_8x16 = vextq_u8(top_u_8x16, vreinterpretq_u8_u32(factorsTop_u_32x4), overlapping);
				bottom_u_8x16 = vextq_u8(bottom_u_8x16, vreinterpretq_u8_u32(factorsTop_u_32x4), overlapping);
			}

			// top[1], top[2], top[3], ..., top[14], top[15], top[0]
			const uint8x16_t topB_u_8x16 = vextq_u8(top_u_8x16, top_u_8x16, 1);
			const uint8x16_t bottomB_u_8x16 = vextq_u8(bottom_u_8x16, bottom_u_8x16, 1);


			// top[0] * L, top[1] * R, top[2] * L, ...
			const uint32x4_t topLowA_u_32x4 = vpaddlq_u16(vmull_u8(vget_low_u8(top_u_8x16), factorsLeftRight_u_8x8));
			const uint32x4_t bottomLowA_u_32x4 = vpaddlq_u16(vmull_u8(vget_low_u8(bottom_u_8x16), factorsLeftRight_u_8x8));

			// top[1] * L, top[2] * R, top[3] * L, ...
			const uint32x4_t topLowB_u_32x4 = vpaddlq_u16(vmull_u8(vget_low_u8(topB_u_8x16), factorsLeftRight_u_8x8));
			const uint32x4_t bottomLowB_u_32x4 = vpaddlq_u16(vmull_u8(vget_low_u8(bottomB_u_8x16), factorsLeftRight_u_8x8));

			// top[8] * L, top[9] * R, top[10] * L, ...
			const uint32x4_t topHighA_u_32x4 = vpaddlq_u16(vmull_u8(vget_high_u8(top_u_8x16), factorsLeftRight_u_8x8));
			const uint32x4_t bottomHighA_u_32x4 = vpaddlq_u16(vmull_u8(vget_high_u8(bottom_u_8x16), factorsLeftRight_u_8x8));

			// top[9] * L, top[10] * R, top[11] * L, ...
			const uint32x4_t topHighB_u_32x4 = vpaddlq_u16(vmull_u8(vget_high_u8(topB_u_8x16), factorsLeftRight_u_8x8));
			const uint32x4_t bottomHighB_u_32x4 = vpaddlq_u16(vmull_u8(vget_high_u8(bottomB_u_8x16), factorsLeftRight_u_8x8));


			// (top[0] * T + bottom[0] * B + 8192) / 16384, (top[1] * T + bottom[1] * B + 8192) / 16384, ...
			const uint16x4_t resultLowA_u_16x4 = vrshrn_n_u32(vmlaq_u32(vmulq_u32(topLowA_u_32x4, factorsTop_u_32x4), bottomLowA_u_32x4, factorsBottom_u_32x4), 14);
			const uint16x4_t resultHighA_u_16x4 = vrshrn_n_u32(vmlaq_u32(vmulq_u32(topHighA_u_32x4, factorsTop_u_32x4), bottomHighA_u_32x4, factorsBottom_u_32x4), 14);

			const uint16x4_t resultLowB_u_16x4 = vrshrn_n_u32(vmlaq_u32(vmulq_u32(topLowB_u_32x4, factorsTop_u_32x4), bottomLowB_u_32x4, factorsBottom_u_32x4), 14);
			const uint16x4_t resultHighB_u_16x4 = vrshrn_n_u32(vmlaq_u32(vmulq_u32(topHighB_u_32x4, factorsTop_u_32x4), bottomHighB_u_32x4, factorsBottom_u_32x4), 14);

			const uint16x8_t resultA_u_16x8 = vcombine_u16(resultLowA_u_16x4, resultHighA_u_16x4);
			const uint16x8_t resultB_u_16x8 = vcombine_u16(resultLowB_u_16x4, resultHighB_u_16x4);

			// resultA[0], resultB[0], resultA[1], resultB[1], resultA[2], ...
			const uint8x16_t result_u_8x16 = vreinterpretq_u8_u16(vsliq_n_u16(resultA_u_16x8, resultB_u_16x8, 8));

			ocean_assert(!block7 && !partialBlock7 && blocks1 == 0u);
			const bool isLastBlock = y + 1u == tPatchSize;

			if (isLastBlock)
			{
				uint8_t tempBuffer[16];
				vst1q_u8(tempBuffer, result_u_8x16);

				memcpy(buffer, &tempBuffer, remainingAfterBlocks15);
			}
			else
			{
				vst1q_u8(buffer, result_u_8x16);
			}

			imageTopLeft += remainingAfterBlocks15;
			buffer += remainingAfterBlocks15;
		}

		if constexpr (block7)
		{
			const uint8x8_t top_u_8x8 = vld1_u8(imageTopLeft);
			const uint8x8_t bottom_u_8x8 = vld1_u8(imageTopLeft + imageStrideElements);

			// top[1], top[2], top[3], ..., top[6], top[7], X
			const uint8x8_t topB_u_8x8 = vext_u8(top_u_8x8, factorsLeftRight_u_8x8, 1);
			const uint8x8_t bottomB_u_8x8 = vext_u8(bottom_u_8x8, factorsLeftRight_u_8x8, 1);


			// top[0] * L, top[1] * R, top[2] * L, ...
			const uint32x4_t topA_u_32x4 = vpaddlq_u16(vmull_u8(top_u_8x8, factorsLeftRight_u_8x8));
			const uint32x4_t bottomA_u_32x4 = vpaddlq_u16(vmull_u8(bottom_u_8x8, factorsLeftRight_u_8x8));

			// top[1] * L, top[2] * R, top[3] * L, ...
			const uint32x4_t topB_u_32x4 = vpaddlq_u16(vmull_u8(topB_u_8x8, factorsLeftRight_u_8x8));
			const uint32x4_t bottomB_u_32x4 = vpaddlq_u16(vmull_u8(bottomB_u_8x8, factorsLeftRight_u_8x8));


			// (top[0] * T + bottom[0] * B + 8192) / 16384, (top[1] * T + bottom[1] * B + 8192) / 16384, ...
			const uint16x4_t resultA_u_16x4 = vrshrn_n_u32(vmlaq_u32(vmulq_u32(topA_u_32x4, factorsTop_u_32x4), bottomA_u_32x4, factorsBottom_u_32x4), 14);
			const uint16x4_t resultB_u_16x4 = vrshrn_n_u32(vmlaq_u32(vmulq_u32(topB_u_32x4, factorsTop_u_32x4), bottomB_u_32x4, factorsBottom_u_32x4), 14);


			// resultA[0], resultB[0], resultA[1], resultB[1], resultA[2], ...
			const uint8x8_t result_u_8x8 = vreinterpret_u8_u16(vsli_n_u16(resultA_u_16x4, resultB_u_16x4, 8));

			const bool isLastBlock = (y + 1u == tPatchSize) && (!partialBlock7 && blocks1 == 0u);

			if (isLastBlock)
			{
				uint8_t tempBuffer[8];
				vst1_u8(tempBuffer, result_u_8x8);

				memcpy(buffer, &tempBuffer, 7);
			}
			else
			{
				vst1_u8(buffer, result_u_8x8);
			}

			imageTopLeft += 7;
			buffer += 7;
		}

		if constexpr (partialBlock7)
		{
			ocean_assert(blocks1 == 0u);

			uint8x8_t top_u_8x8;
			uint8x8_t bottom_u_8x8;

			if (y < tPatchSize - 1u)
			{
				top_u_8x8 = vld1_u8(imageTopLeft);
				bottom_u_8x8 = vld1_u8(imageTopLeft + imageStrideElements);
			}
			else
			{
				constexpr unsigned int overlapping = 8u - (remainingAfterBlock7 + 1u);

				top_u_8x8 = vld1_u8(imageTopLeft - overlapping);
				bottom_u_8x8 = vld1_u8(imageTopLeft + imageStrideElements - overlapping);

				top_u_8x8 = vext_u8(top_u_8x8, factorsLeftRight_u_8x8, overlapping);
				bottom_u_8x8 = vext_u8(bottom_u_8x8, factorsLeftRight_u_8x8, overlapping);
			}


			// top[1], top[2], top[3], ..., top[6], top[7], X
			const uint8x8_t topB_u_8x8 = vext_u8(top_u_8x8, factorsLeftRight_u_8x8, 1);
			const uint8x8_t bottomB_u_8x8 = vext_u8(bottom_u_8x8, factorsLeftRight_u_8x8, 1);


			// top[0] * L, top[1] * R, top[2] * L, ...
			const uint32x4_t topA_u_32x4 = vpaddlq_u16(vmull_u8(top_u_8x8, factorsLeftRight_u_8x8));
			const uint32x4_t bottomA_u_32x4 = vpaddlq_u16(vmull_u8(bottom_u_8x8, factorsLeftRight_u_8x8));

			// top[1] * L, top[2] * R, top[3] * L, ...
			const uint32x4_t topB_u_32x4 = vpaddlq_u16(vmull_u8(topB_u_8x8, factorsLeftRight_u_8x8));
			const uint32x4_t bottomB_u_32x4 = vpaddlq_u16(vmull_u8(bottomB_u_8x8, factorsLeftRight_u_8x8));


			// (top[0] * T + bottom[0] * B + 8192) / 16384, (top[1] * T + bottom[1] * B + 8192) / 16384, ...
			const uint16x4_t resultA_u_16x4 = vrshrn_n_u32(vmlaq_u32(vmulq_u32(topA_u_32x4, factorsTop_u_32x4), bottomA_u_32x4, factorsBottom_u_32x4), 14);
			const uint16x4_t resultB_u_16x4 = vrshrn_n_u32(vmlaq_u32(vmulq_u32(topB_u_32x4, factorsTop_u_32x4), bottomB_u_32x4, factorsBottom_u_32x4), 14);


			// resultA[0], resultB[0], resultA[1], resultB[1], resultA[2], ...
			const uint8x8_t result_u_8x8 = vreinterpret_u8_u16(vsli_n_u16(resultA_u_16x4, resultB_u_16x4, 8));

			ocean_assert(blocks1 == 0u);
			const bool isLastBlock = y + 1u == tPatchSize;

			if (isLastBlock)
			{
				uint8_t tempBuffer[8];
				vst1_u8(tempBuffer, result_u_8x8);

				memcpy(buffer, &tempBuffer, remainingAfterBlock7);
			}
			else
			{
				vst1_u8(buffer, result_u_8x8);
			}

			imageTopLeft += remainingAfterBlock7;
			buffer += remainingAfterBlock7;
		}

		if constexpr (blocks1 != 0u)
		{
			const unsigned int factorTopLeft = factorTop * factorLeft;
			const unsigned int factorTopRight = factorTop * factorRight;

			const unsigned int factorBottomLeft = factorBottom * factorLeft;
			const unsigned int factorBottomRight = factorBottom * factorRight;

			const uint8_t* const imageBottomLeft = imageTopLeft + imageStrideElements;

			for (unsigned int n = 0u; n < blocks1; ++n)
			{
				buffer[n] = uint8_t((imageTopLeft[n] * factorTopLeft + imageTopLeft[1u + n] * factorTopRight + imageBottomLeft[n] * factorBottomLeft + imageBottomLeft[1u + n] * factorBottomRight + 8192u) / 16384u);
			}

			imageTopLeft += blocks1;
			buffer += blocks1;
		}

		imageTopLeft += imageStrideElements - tPatchSize;
	}
}

template <unsigned int tChannels>
template <unsigned int tPatchSize>
inline void AdvancedFrameInterpolatorBilinearNEON::SpecializedForChannels<tChannels>::interpolateSquarePatch8BitPerChannel(const uint8_t* imageTopLeft, const unsigned int imageStrideElements, uint8_t* buffer, const unsigned int factorRight, const unsigned int factorBottom)
{
	ocean_assert(imageTopLeft != nullptr && buffer != nullptr);
	ocean_assert(imageStrideElements >= 1u * tPatchSize);

	ocean_assert(factorRight <= 128u && factorBottom <= 128u);

	const unsigned int factorLeft = 128u - factorRight;
	const unsigned int factorTop = 128u - factorBottom;

	const unsigned int factorTopLeft = factorTop * factorLeft;
	const unsigned int factorTopRight = factorTop * factorRight;

	const unsigned int factorBottomLeft = factorBottom * factorLeft;
	const unsigned int factorBottomRight = factorBottom * factorRight;

	const uint8_t* imageBottomLeft = imageTopLeft + imageStrideElements;

	for (unsigned int y = 0u; y < tPatchSize; ++y)
	{
		for (unsigned int x = 0u; x < tPatchSize; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				buffer[n] = uint8_t((imageTopLeft[n] * factorTopLeft + imageTopLeft[tChannels + n] * factorTopRight + imageBottomLeft[n] * factorBottomLeft + imageBottomLeft[tChannels + n] * factorBottomRight + 8192u) / 16384u);
			}

			imageTopLeft += tChannels;
			imageBottomLeft += tChannels;

			buffer += tChannels;
		}

		imageTopLeft += imageStrideElements - tChannels * tPatchSize;
		imageBottomLeft += imageStrideElements - tChannels * tPatchSize;
	}
}

template <unsigned int tChannels, unsigned int tPatchSize, PixelCenter tPixelCenter, typename TScalar>
inline void AdvancedFrameInterpolatorBilinearNEON::interpolateSquarePatch8BitPerChannel(const uint8_t* const image, const unsigned int width, const unsigned int imagePaddingElements, uint8_t* buffer, const VectorT2<TScalar>& position)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize % 2u == 1u, "Invalid patch size!");

	ocean_assert(image != nullptr && buffer != nullptr);
	ocean_assert(tPatchSize + 1u <= width);

	ocean_assert(tPatchSize >= 5u);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	const unsigned int imageStrideElements = width * tChannels + imagePaddingElements;

	const VectorT2<TScalar> shiftedPosition = tPixelCenter == PC_TOP_LEFT ? position : position - VectorT2<TScalar>(TScalar(0.5), TScalar(0.5));

	ocean_assert(shiftedPosition.x() >= TScalar(tPatchSize_2) && shiftedPosition.y() >= TScalar(tPatchSize_2));
	ocean_assert(shiftedPosition.x() < TScalar(width - tPatchSize_2 - 1u));

	const unsigned int left = (unsigned int)(shiftedPosition.x()) - tPatchSize_2;
	const unsigned int top = (unsigned int)(shiftedPosition.y()) - tPatchSize_2;

	ocean_assert(left + tPatchSize < width);

	const TScalar tx = shiftedPosition.x() - TScalar(int(shiftedPosition.x()));
	ocean_assert(tx >= TScalar(0) && tx <= TScalar(1));
	const unsigned int factorRight = (unsigned int)(tx * TScalar(128) + TScalar(0.5));

	const TScalar ty = shiftedPosition.y() - TScalar(int(shiftedPosition.y()));
	ocean_assert(ty >= 0 && ty <= 1);
	const unsigned int factorBottom = (unsigned int)(ty * TScalar(128) + TScalar(0.5));

	const uint8_t* const imageTopLeft = image + top * imageStrideElements + left * tChannels;

	SpecializedForChannels<tChannels>::template interpolateSquarePatch8BitPerChannel<tPatchSize>(imageTopLeft, imageStrideElements, buffer, factorRight, factorBottom);
}

}

}

}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

#endif // META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_INTERPOLATOR_BILINEAR_NEON_H
