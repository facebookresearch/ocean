/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_INTERPOLATOR_BILINEAR_SSE_H
#define META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_INTERPOLATOR_BILINEAR_SSE_H

#include "ocean/cv/advanced/Advanced.h"

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

#include "ocean/cv/SSE.h"

#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

/**
 * This class implements advanced bilinear frame interpolation functions using SSE extensions.
 * @ingroup cvadvanced
 */
class OCEAN_CV_ADVANCED_EXPORT AdvancedFrameInterpolatorBilinearSSE
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
inline void AdvancedFrameInterpolatorBilinearSSE::SpecializedForChannels<1u>::interpolateSquarePatch8BitPerChannel(const uint8_t* imageTopLeft, const unsigned int imageStrideElements, uint8_t* buffer, const unsigned int factorRight, const unsigned int factorBottom)
{
	ocean_assert(tPatchSize >= 5u);

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

	const unsigned int factorTopLeft = factorTop * factorLeft;
	const unsigned int factorTopRight = factorTop * factorRight;

	// TL 0 TR 0 TL 0 TR 0 TL 0 TR 0 TL 0 TR 0
	const __m128i factorsTop_u_16x8 = _mm_set1_epi32(int(factorTopLeft) | int(factorTopRight) << 16);

	const unsigned int factorBottomLeft = factorBottom * factorLeft;
	const unsigned int factorBottomRight = factorBottom * factorRight;

	// BL 0 BR 0 BL 0 BR 0 BL 0 BR 0 BL 0 BR 0
	const __m128i factorsBottom_u_16x8 = _mm_set1_epi32(int(factorBottomLeft) | int(factorBottomRight) << 16);

	for (unsigned int y = 0u; y < tPatchSize; ++y)
	{
		SSE::prefetchT0(imageTopLeft + imageStrideElements * 2u);
		SSE::prefetchT0(imageTopLeft + imageStrideElements * 3u);

		for (unsigned int x = 0u; x < blocks15; ++x)
		{
			const __m128i top_u_8x16 = _mm_lddqu_si128((const __m128i*)(imageTopLeft));
			const __m128i bottom_u_8x16 = _mm_lddqu_si128((const __m128i*)(imageTopLeft + imageStrideElements));


			// A B C D E F G H I J K L M N O P   ->   A 0 B 0 C 0 D 0 E 0 F 0 G 0 H 0
			const __m128i topLowA_u_16x8 = _mm_unpacklo_epi8(top_u_8x16, _mm_setzero_si128());
			const __m128i bottomLowA_u_16x8 = _mm_unpacklo_epi8(bottom_u_8x16, _mm_setzero_si128());

			// A B C D E F G H I J K L M N O P   ->   B 0 C 0 D 0 E 0 F 0 G 0 H 0 I 0
			const __m128i topLowB_u_16x8 = _mm_unpacklo_epi8(_mm_srli_si128(top_u_8x16, 1), _mm_setzero_si128());
			const __m128i bottomLowB_u_16x8 = _mm_unpacklo_epi8(_mm_srli_si128(bottom_u_8x16, 1), _mm_setzero_si128());


			// A B C D E F G H I J K L M N O P   ->   I 0 J 0 K 0 L 0 M 0 N 0 O 0 P 0
			const __m128i topHighA_u_16x8 = _mm_unpackhi_epi8(top_u_8x16, _mm_setzero_si128());
			const __m128i bottomHighA_u_16x8 = _mm_unpackhi_epi8(bottom_u_8x16, _mm_setzero_si128());

			// A B C D E F G H I J K L M N O P   ->   J 0 K 0 L 0 M 0 N 0 O 0 P 0 0 0
			const __m128i topHighB_u_16x8 = _mm_unpackhi_epi8(_mm_srli_si128(top_u_8x16, 1), _mm_setzero_si128());
			const __m128i bottomHighB_u_16x8 = _mm_unpackhi_epi8(_mm_srli_si128(bottom_u_8x16, 1), _mm_setzero_si128());


			// A * TL + B * TR, C * TL + D * TR, E * TL + F * TR, G * TL + H * TR
			const __m128i topLowA_u_32x4 = _mm_madd_epi16(topLowA_u_16x8, factorsTop_u_16x8);
			const __m128i bottomLowA_u_32x4 = _mm_madd_epi16(bottomLowA_u_16x8, factorsBottom_u_16x8);

			const __m128i topLowB_u_32x4 = _mm_madd_epi16(topLowB_u_16x8, factorsTop_u_16x8);
			const __m128i bottomLowB_u_32x4 = _mm_madd_epi16(bottomLowB_u_16x8, factorsBottom_u_16x8);


			const __m128i topHighA_u_32x4 = _mm_madd_epi16(topHighA_u_16x8, factorsTop_u_16x8);
			const __m128i bottomHighA_u_32x4 = _mm_madd_epi16(bottomHighA_u_16x8, factorsBottom_u_16x8);

			const __m128i topHighB_u_32x4 = _mm_madd_epi16(topHighB_u_16x8, factorsTop_u_16x8);
			const __m128i bottomHighB_u_32x4 = _mm_madd_epi16(bottomHighB_u_16x8, factorsBottom_u_16x8);


			const __m128i resultLowA_u_32x4 = _mm_srli_epi32(_mm_add_epi32(_mm_add_epi32(topLowA_u_32x4, bottomLowA_u_32x4), _mm_set1_epi32(8192)), 14);
			const __m128i resultLowB_u_32x4 = _mm_srli_epi32(_mm_add_epi32(_mm_add_epi32(topLowB_u_32x4, bottomLowB_u_32x4), _mm_set1_epi32(8192)), 14);

			const __m128i resultLow_u_16x8 = _mm_or_si128(resultLowA_u_32x4, _mm_slli_epi32(resultLowB_u_32x4, 16));


			const __m128i resultHighA_u_32x4 = _mm_srli_epi32(_mm_add_epi32(_mm_add_epi32(topHighA_u_32x4, bottomHighA_u_32x4), _mm_set1_epi32(8192)), 14);
			const __m128i resultHighB_u_32x4 = _mm_srli_epi32(_mm_add_epi32(_mm_add_epi32(topHighB_u_32x4, bottomHighB_u_32x4), _mm_set1_epi32(8192)), 14);

			const __m128i resultHigh_u_16x8 = _mm_or_si128(resultHighA_u_32x4, _mm_slli_epi32(resultHighB_u_32x4, 16));


			// A 0 B 0 C 0 D 0 E 0 F 0 G 0 H 0
			// I 0 J 0 K 0 L 0 M 0 N 0 O 0 0 0    ->    A B C D E F G H I J K L M N O 0
			const __m128i result_u_8x16 = _mm_packus_epi16(resultLow_u_16x8, resultHigh_u_16x8);


			const bool isLastBlock = (y + 1u == tPatchSize) && (x + 1u == blocks15) && (!block7 && !partialBlock7 && blocks1 == 0u);

			if (isLastBlock)
			{
				memcpy(buffer, &result_u_8x16, 15);
			}
			else
			{
				_mm_storeu_si128((__m128i*)buffer, result_u_8x16);
			}

			imageTopLeft += 15;
			buffer += 15;
		}

		if constexpr (partialBlock15)
		{
			ocean_assert(!block7 && !partialBlock7 && blocks1 == 0u);

			__m128i top_u_8x16;
			__m128i bottom_u_8x16;

			if (y < tPatchSize - 1u)
			{
				top_u_8x16 = _mm_lddqu_si128((const __m128i*)(imageTopLeft));
				bottom_u_8x16 = _mm_lddqu_si128((const __m128i*)(imageTopLeft + imageStrideElements));
			}
			else
			{
				memcpy(&top_u_8x16, imageTopLeft, remainingAfterBlocks15 + 1u);
				memcpy(&bottom_u_8x16, imageTopLeft + imageStrideElements, remainingAfterBlocks15 + 1u);
			}


			// A B C D E F G H I J K L M N O P   ->   A 0 B 0 C 0 D 0 E 0 F 0 G 0 H 0
			const __m128i topLowA_u_16x8 = _mm_unpacklo_epi8(top_u_8x16, _mm_setzero_si128());
			const __m128i bottomLowA_u_16x8 = _mm_unpacklo_epi8(bottom_u_8x16, _mm_setzero_si128());

			// A B C D E F G H I J K L M N O P   ->   B 0 C 0 D 0 E 0 F 0 G 0 H 0 I 0
			const __m128i topLowB_u_16x8 = _mm_unpacklo_epi8(_mm_srli_si128(top_u_8x16, 1), _mm_setzero_si128());
			const __m128i bottomLowB_u_16x8 = _mm_unpacklo_epi8(_mm_srli_si128(bottom_u_8x16, 1), _mm_setzero_si128());


			// A B C D E F G H I J K L M N O P   ->   I 0 J 0 K 0 L 0 M 0 N 0 O 0 P 0
			const __m128i topHighA_u_16x8 = _mm_unpackhi_epi8(top_u_8x16, _mm_setzero_si128());
			const __m128i bottomHighA_u_16x8 = _mm_unpackhi_epi8(bottom_u_8x16, _mm_setzero_si128());

			// A B C D E F G H I J K L M N O P   ->   J 0 K 0 L 0 M 0 N 0 O 0 P 0 0 0
			const __m128i topHighB_u_16x8 = _mm_unpackhi_epi8(_mm_srli_si128(top_u_8x16, 1), _mm_setzero_si128());
			const __m128i bottomHighB_u_16x8 = _mm_unpackhi_epi8(_mm_srli_si128(bottom_u_8x16, 1), _mm_setzero_si128());


			// A * TL + B * TR, C * TL + D * TR, E * TL + F * TR, G * TL + H * TR
			const __m128i topLowA_u_32x4 = _mm_madd_epi16(topLowA_u_16x8, factorsTop_u_16x8);
			const __m128i bottomLowA_u_32x4 = _mm_madd_epi16(bottomLowA_u_16x8, factorsBottom_u_16x8);

			const __m128i topLowB_u_32x4 = _mm_madd_epi16(topLowB_u_16x8, factorsTop_u_16x8);
			const __m128i bottomLowB_u_32x4 = _mm_madd_epi16(bottomLowB_u_16x8, factorsBottom_u_16x8);


			const __m128i topHighA_u_32x4 = _mm_madd_epi16(topHighA_u_16x8, factorsTop_u_16x8);
			const __m128i bottomHighA_u_32x4 = _mm_madd_epi16(bottomHighA_u_16x8, factorsBottom_u_16x8);

			const __m128i topHighB_u_32x4 = _mm_madd_epi16(topHighB_u_16x8, factorsTop_u_16x8);
			const __m128i bottomHighB_u_32x4 = _mm_madd_epi16(bottomHighB_u_16x8, factorsBottom_u_16x8);


			const __m128i resultLowA_u_32x4 = _mm_srli_epi32(_mm_add_epi32(_mm_add_epi32(topLowA_u_32x4, bottomLowA_u_32x4), _mm_set1_epi32(8192)), 14);
			const __m128i resultLowB_u_32x4 = _mm_srli_epi32(_mm_add_epi32(_mm_add_epi32(topLowB_u_32x4, bottomLowB_u_32x4), _mm_set1_epi32(8192)), 14);

			const __m128i resultLow_u_16x8 = _mm_or_si128(resultLowA_u_32x4, _mm_slli_epi32(resultLowB_u_32x4, 16));


			const __m128i resultHighA_u_32x4 = _mm_srli_epi32(_mm_add_epi32(_mm_add_epi32(topHighA_u_32x4, bottomHighA_u_32x4), _mm_set1_epi32(8192)), 14);
			const __m128i resultHighB_u_32x4 = _mm_srli_epi32(_mm_add_epi32(_mm_add_epi32(topHighB_u_32x4, bottomHighB_u_32x4), _mm_set1_epi32(8192)), 14);

			const __m128i resultHigh_u_16x8 = _mm_or_si128(resultHighA_u_32x4, _mm_slli_epi32(resultHighB_u_32x4, 16));


			// A 0 B 0 C 0 D 0 E 0 F 0 G 0 H 0
			// I 0 J 0 K 0 L 0 M 0 N 0 O 0 0 0    ->    A B C D E F G H I J K L M N O 0
			const __m128i result_u_8x16 = _mm_packus_epi16(resultLow_u_16x8, resultHigh_u_16x8);

			ocean_assert(!block7 && !partialBlock7 && blocks1 == 0u);
			const bool isLastBlock = y + 1u == tPatchSize;

			if (isLastBlock)
			{
				memcpy(buffer, &result_u_8x16, remainingAfterBlocks15);
			}
			else
			{
				_mm_storeu_si128((__m128i*)buffer, result_u_8x16);
			}

			imageTopLeft += remainingAfterBlocks15;
			buffer += remainingAfterBlocks15;
		}

		if constexpr (block7)
		{
			const __m128i top_u_8x16 = _mm_loadl_epi64((const __m128i*)(imageTopLeft));
			const __m128i bottom_u_8x16 = _mm_loadl_epi64((const __m128i*)(imageTopLeft + imageStrideElements));


			// A B C D E F G H I J K L M N O P   ->   A 0 B 0 C 0 D 0 E 0 F 0 G 0 H 0
			const __m128i topLowA_u_16x8 = _mm_unpacklo_epi8(top_u_8x16, _mm_setzero_si128());
			const __m128i bottomLowA_u_16x8 = _mm_unpacklo_epi8(bottom_u_8x16, _mm_setzero_si128());

			// A B C D E F G H I J K L M N O P   ->   B 0 C 0 D 0 E 0 F 0 G 0 H 0 I 0
			const __m128i topLowB_u_16x8 = _mm_unpacklo_epi8(_mm_srli_si128(top_u_8x16, 1), _mm_setzero_si128());
			const __m128i bottomLowB_u_16x8 = _mm_unpacklo_epi8(_mm_srli_si128(bottom_u_8x16, 1), _mm_setzero_si128());


			// A * TL + B * TR, C * TL + D * TR, E * TL + F * TR, G * TL + H * TR
			const __m128i topLowA_u_32x4 = _mm_madd_epi16(topLowA_u_16x8, factorsTop_u_16x8);
			const __m128i bottomLowA_u_32x4 = _mm_madd_epi16(bottomLowA_u_16x8, factorsBottom_u_16x8);

			const __m128i topLowB_u_32x4 = _mm_madd_epi16(topLowB_u_16x8, factorsTop_u_16x8);
			const __m128i bottomLowB_u_32x4 = _mm_madd_epi16(bottomLowB_u_16x8, factorsBottom_u_16x8);


			const __m128i resultLowA_u_32x4 = _mm_srli_epi32(_mm_add_epi32(_mm_add_epi32(topLowA_u_32x4, bottomLowA_u_32x4), _mm_set1_epi32(8192)), 14);
			const __m128i resultLowB_u_32x4 = _mm_srli_epi32(_mm_add_epi32(_mm_add_epi32(topLowB_u_32x4, bottomLowB_u_32x4), _mm_set1_epi32(8192)), 14);

			const __m128i resultLow_u_16x8 = _mm_or_si128(resultLowA_u_32x4, _mm_slli_epi32(resultLowB_u_32x4, 16));

			// A 0 B 0 C 0 D 0 E 0 F 0 H 0 H 0   ->     A B C D E F G H X X X X X X X X
			const __m128i result_u_8x16 = _mm_packus_epi16(resultLow_u_16x8, _mm_setzero_si128());

			const bool isLastBlock = (y + 1u == tPatchSize) && (!partialBlock7 && blocks1 == 0u);

			if (isLastBlock)
			{
				memcpy(buffer, &result_u_8x16, 7);
			}
			else
			{
				_mm_storel_epi64((__m128i*)buffer, result_u_8x16);
			}

			imageTopLeft += 7;
			buffer += 7;
		}

		if constexpr (partialBlock7)
		{
			ocean_assert(blocks1 == 0u);

			__m128i top_u_8x16;
			__m128i bottom_u_8x16;

			if (y < tPatchSize - 1u)
			{
				top_u_8x16 = _mm_loadl_epi64((const __m128i*)(imageTopLeft));
				bottom_u_8x16 = _mm_loadl_epi64((const __m128i*)(imageTopLeft + imageStrideElements));
			}
			else
			{
				memcpy(&top_u_8x16, imageTopLeft, remainingAfterBlock7 + 1u);
				memcpy(&bottom_u_8x16, imageTopLeft + imageStrideElements, remainingAfterBlock7 + 1u);
			}


			// A B C D E F G H I J K L M N O P   ->   A 0 B 0 C 0 D 0 E 0 F 0 G 0 H 0
			const __m128i topLowA_u_16x8 = _mm_unpacklo_epi8(top_u_8x16, _mm_setzero_si128());
			const __m128i bottomLowA_u_16x8 = _mm_unpacklo_epi8(bottom_u_8x16, _mm_setzero_si128());

			// A B C D E F G H I J K L M N O P   ->   B 0 C 0 D 0 E 0 F 0 G 0 H 0 I 0
			const __m128i topLowB_u_16x8 = _mm_unpacklo_epi8(_mm_srli_si128(top_u_8x16, 1), _mm_setzero_si128());
			const __m128i bottomLowB_u_16x8 = _mm_unpacklo_epi8(_mm_srli_si128(bottom_u_8x16, 1), _mm_setzero_si128());


			// A * TL + B * TR, C * TL + D * TR, E * TL + F * TR, G * TL + H * TR
			const __m128i topLowA_u_32x4 = _mm_madd_epi16(topLowA_u_16x8, factorsTop_u_16x8);
			const __m128i bottomLowA_u_32x4 = _mm_madd_epi16(bottomLowA_u_16x8, factorsBottom_u_16x8);

			const __m128i topLowB_u_32x4 = _mm_madd_epi16(topLowB_u_16x8, factorsTop_u_16x8);
			const __m128i bottomLowB_u_32x4 = _mm_madd_epi16(bottomLowB_u_16x8, factorsBottom_u_16x8);


			const __m128i resultLowA_u_32x4 = _mm_srli_epi32(_mm_add_epi32(_mm_add_epi32(topLowA_u_32x4, bottomLowA_u_32x4), _mm_set1_epi32(8192)), 14);
			const __m128i resultLowB_u_32x4 = _mm_srli_epi32(_mm_add_epi32(_mm_add_epi32(topLowB_u_32x4, bottomLowB_u_32x4), _mm_set1_epi32(8192)), 14);

			const __m128i resultLow_u_16x8 = _mm_or_si128(resultLowA_u_32x4, _mm_slli_epi32(resultLowB_u_32x4, 16));

			// A 0 B 0 C 0 D 0 E 0 F 0 H 0 H 0   ->     A B C D E F G H X X X X X X X X
			const __m128i result_u_8x16 = _mm_packus_epi16(resultLow_u_16x8, _mm_setzero_si128());

			ocean_assert(blocks1 == 0u);
			const bool isLastBlock = y + 1u == tPatchSize;

			if (isLastBlock)
			{
				memcpy(buffer, &result_u_8x16, remainingAfterBlock7);
			}
			else
			{
				_mm_storel_epi64((__m128i*)buffer, result_u_8x16);
			}

			imageTopLeft += remainingAfterBlock7;
			buffer += remainingAfterBlock7;
		}

		if constexpr (blocks1 != 0u)
		{
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

template <>
template <unsigned int tPatchSize>
inline void AdvancedFrameInterpolatorBilinearSSE::SpecializedForChannels<3u>::interpolateSquarePatch8BitPerChannel(const uint8_t* imageTopLeft, const unsigned int imageStrideElements, uint8_t* buffer, const unsigned int factorRight, const unsigned int factorBottom)
{
	ocean_assert(imageTopLeft != nullptr && buffer != nullptr);
	ocean_assert(imageStrideElements >= 1u * tPatchSize);

	ocean_assert(factorRight <= 128u && factorBottom <= 128u);

	const unsigned int factorLeft = 128u - factorRight;
	const unsigned int factorTop = 128u - factorBottom;

	constexpr unsigned int blocks4 = tPatchSize / 4u;
	constexpr unsigned int remainingAfterBlocks4 = tPatchSize % 4u;

	constexpr bool partialBlock4 = remainingAfterBlocks4 >= 2u;
	constexpr unsigned int remainingAfterPartialBlock4 = partialBlock4 ? 0u : remainingAfterBlocks4;

	constexpr unsigned int blocks1 = remainingAfterPartialBlock4;

	const unsigned int factorTopLeft = factorTop * factorLeft;
	const unsigned int factorTopRight = factorTop * factorRight;

	// TL 0 TR 0 TL 0 TR 0 TL 0 TR 0 TL 0 TR 0
	const __m128i factorsTop_u_16x8 = _mm_set1_epi32(int(factorTopLeft) | int(factorTopRight) << 16);

	const unsigned int factorBottomLeft = factorBottom * factorLeft;
	const unsigned int factorBottomRight = factorBottom * factorRight;

	// BL 0 BR 0 BL 0 BR 0 BL 0 BR 0 BL 0 BR 0
	const __m128i factorsBottom_u_16x8 = _mm_set1_epi32(int(factorBottomLeft) | int(factorBottomRight) << 16);

	for (unsigned int y = 0u; y < tPatchSize; ++y)
	{
		SSE::prefetchT0(imageTopLeft + imageStrideElements * 2u);
		SSE::prefetchT0(imageTopLeft + imageStrideElements * 3u);

		for (unsigned int x = 0u; x < blocks4; ++x)
		{
			const bool canReadLastElements = y < tPatchSize - 1u || x < blocks4 - 1u;

			__m128i top_u_8x16;
			__m128i bottom_u_8x16;

			if (canReadLastElements)
			{
				top_u_8x16 = _mm_lddqu_si128((const __m128i*)(imageTopLeft));
				bottom_u_8x16 = _mm_lddqu_si128((const __m128i*)(imageTopLeft + imageStrideElements));
			}
			else
			{
				top_u_8x16 = _mm_srli_si128(_mm_lddqu_si128((const __m128i*)(imageTopLeft - 1)), 1);
				bottom_u_8x16 = _mm_srli_si128(_mm_lddqu_si128((const __m128i*)(imageTopLeft + imageStrideElements - 1u)), 1);
			}


			// de-interleaving

			// R0 G0 B0  R1 G1 B1  R2 G2 B2  R3 G3 B3  R4 G4 B4 0
			// -> R0 0 R1 0 R1 0 R2 0 R2 0 R3 0 R3 0 R4 0
			const __m128i topChannel0_u_16x8 = _mm_shuffle_epi8(top_u_8x16, SSE::set128i(0xFF0cFF09FF09FF06ull, 0xFF06FF03FF03FF00ull));
			const __m128i bottomChannel0_u_16x8 = _mm_shuffle_epi8(bottom_u_8x16, SSE::set128i(0xFF0cFF09FF09FF06ull, 0xFF06FF03FF03FF00ull));

			// -> G0 0 G1 0 G1 0 G2 0 G2 0 G3 0 G3 0 G4 0
			const __m128i topChannel1_u_16x8 = _mm_shuffle_epi8(top_u_8x16, SSE::set128i(0xFF0dFF0aFF0aFF07ull, 0xFF07FF04FF04FF01ull));
			const __m128i bottomChannel1_u_16x8 = _mm_shuffle_epi8(bottom_u_8x16, SSE::set128i(0xFF0dFF0aFF0aFF07ull, 0xFF07FF04FF04FF01ull));

			// -> B0 0 B1 0 B1 0 B2 0 B2 0 B3 0 B3 0 B4 0
			const __m128i topChannel2_u_16x8 = _mm_shuffle_epi8(top_u_8x16, SSE::set128i(0xFF0eFF0bFF0bFF08ull, 0xFF08FF05FF05FF02ull));
			const __m128i bottomChannel2_u_16x8 = _mm_shuffle_epi8(bottom_u_8x16, SSE::set128i(0xFF0eFF0bFF0bFF08ull, 0xFF08FF05FF05FF02ull));


			// R0 * TL + R1 * TR, R1 * TL + R2 * TR, R2 * TL + R3 * TR, R3 * TL + R4 * TR
			const __m128i topChannel0_u_32x4 = _mm_madd_epi16(topChannel0_u_16x8, factorsTop_u_16x8);
			const __m128i topChannel1_u_32x4 = _mm_madd_epi16(topChannel1_u_16x8, factorsTop_u_16x8);
			const __m128i topChannel2_u_32x4 = _mm_madd_epi16(topChannel2_u_16x8, factorsTop_u_16x8);

			const __m128i bottomChannel0_u_32x4 = _mm_madd_epi16(bottomChannel0_u_16x8, factorsBottom_u_16x8);
			const __m128i bottomChannel1_u_32x4 = _mm_madd_epi16(bottomChannel1_u_16x8, factorsBottom_u_16x8);
			const __m128i bottomChannel2_u_32x4 = _mm_madd_epi16(bottomChannel2_u_16x8, factorsBottom_u_16x8);


			const __m128i resultChannel0_u_32x4 = _mm_srli_epi32(_mm_add_epi32(_mm_add_epi32(topChannel0_u_32x4, bottomChannel0_u_32x4), _mm_set1_epi32(8192)), 14);
			const __m128i resultChannel1_u_32x4 = _mm_srli_epi32(_mm_add_epi32(_mm_add_epi32(topChannel1_u_32x4, bottomChannel1_u_32x4), _mm_set1_epi32(8192)), 14);
			const __m128i resultChannel2_u_32x4 = _mm_srli_epi32(_mm_add_epi32(_mm_add_epi32(topChannel2_u_32x4, bottomChannel2_u_32x4), _mm_set1_epi32(8192)), 14);


			// interleaving

			const __m128i interleavedA_u_8x16 = _mm_shuffle_epi8(resultChannel0_u_32x4, SSE::set128i(0xFFFFFFFFFFFF0cFFull, 0xFF08FFFF04FFFF00ull));
			const __m128i interleavedB_u_8x16 = _mm_shuffle_epi8(resultChannel1_u_32x4, SSE::set128i(0xFFFFFFFFFF0cFFFFull, 0x08FFFF04FFFF00FFull));
			const __m128i interleavedC_u_8x16 = _mm_shuffle_epi8(resultChannel2_u_32x4, SSE::set128i(0xFFFFFFFF0cFFFF08ull, 0xFFFF04FFFF00FFFFull));


			const __m128i result_u_8x16 = _mm_or_si128(interleavedA_u_8x16, _mm_or_si128(interleavedB_u_8x16, interleavedC_u_8x16));

			const bool isLastBlock = (y + 1u == tPatchSize) && (x + 1u == blocks4) && (!partialBlock4 && blocks1 <= 1u);

			if (isLastBlock)
			{
				uint8_t tempBuffer[16];
				_mm_storeu_si128((__m128i*)tempBuffer, result_u_8x16);

				memcpy(buffer, tempBuffer, 12);
			}
			else
			{
				_mm_storeu_si128((__m128i*)buffer, result_u_8x16);
			}

			imageTopLeft += 12;
			buffer += 12;
		}

		if (partialBlock4)
		{
			const bool canReadLastElements = y < tPatchSize - 1u;

			__m128i top_u_8x16;
			__m128i bottom_u_8x16;

			if (canReadLastElements)
			{
				top_u_8x16 = _mm_lddqu_si128((const __m128i*)(imageTopLeft));
				bottom_u_8x16 = _mm_lddqu_si128((const __m128i*)(imageTopLeft + imageStrideElements));
			}
			else
			{
				constexpr unsigned int overlappingElements = 16u - (remainingAfterBlocks4 * 3u + 3u);
				ocean_assert(overlappingElements < 16u);

				top_u_8x16 = _mm_srli_si128(_mm_lddqu_si128((const __m128i*)(imageTopLeft - overlappingElements)), overlappingElements);
				bottom_u_8x16 = _mm_srli_si128(_mm_lddqu_si128((const __m128i*)(imageTopLeft + imageStrideElements - overlappingElements)), overlappingElements);
			}


			// de-interleaving

			// R0 G0 B0  R1 G1 B1  R2 G2 B2  R3 G3 B3  R4 G4 B4 0
			// -> R0 0 R1 0 R1 0 R2 0 R2 0 R3 0 R3 0 R4 0
			const __m128i topChannel0_u_16x8 = _mm_shuffle_epi8(top_u_8x16, SSE::set128i(0xFF0cFF09FF09FF06ull, 0xFF06FF03FF03FF00ull));
			const __m128i bottomChannel0_u_16x8 = _mm_shuffle_epi8(bottom_u_8x16, SSE::set128i(0xFF0cFF09FF09FF06ull, 0xFF06FF03FF03FF00ull));

			// -> G0 0 G1 0 G1 0 G2 0 G2 0 G3 0 G3 0 G4 0
			const __m128i topChannel1_u_16x8 = _mm_shuffle_epi8(top_u_8x16, SSE::set128i(0xFF0dFF0aFF0aFF07ull, 0xFF07FF04FF04FF01ull));
			const __m128i bottomChannel1_u_16x8 = _mm_shuffle_epi8(bottom_u_8x16, SSE::set128i(0xFF0dFF0aFF0aFF07ull, 0xFF07FF04FF04FF01ull));

			// -> B0 0 B1 0 B1 0 B2 0 B2 0 B3 0 B3 0 B4 0
			const __m128i topChannel2_u_16x8 = _mm_shuffle_epi8(top_u_8x16, SSE::set128i(0xFF0eFF0bFF0bFF08ull, 0xFF08FF05FF05FF02ull));
			const __m128i bottomChannel2_u_16x8 = _mm_shuffle_epi8(bottom_u_8x16, SSE::set128i(0xFF0eFF0bFF0bFF08ull, 0xFF08FF05FF05FF02ull));


			// R0 * TL + R1 * TR, R1 * TL + R2 * TR, R2 * TL + R3 * TR, R3 * TL + R4 * TR
			const __m128i topChannel0_u_32x4 = _mm_madd_epi16(topChannel0_u_16x8, factorsTop_u_16x8);
			const __m128i topChannel1_u_32x4 = _mm_madd_epi16(topChannel1_u_16x8, factorsTop_u_16x8);
			const __m128i topChannel2_u_32x4 = _mm_madd_epi16(topChannel2_u_16x8, factorsTop_u_16x8);

			const __m128i bottomChannel0_u_32x4 = _mm_madd_epi16(bottomChannel0_u_16x8, factorsBottom_u_16x8);
			const __m128i bottomChannel1_u_32x4 = _mm_madd_epi16(bottomChannel1_u_16x8, factorsBottom_u_16x8);
			const __m128i bottomChannel2_u_32x4 = _mm_madd_epi16(bottomChannel2_u_16x8, factorsBottom_u_16x8);


			const __m128i resultChannel0_u_32x4 = _mm_srli_epi32(_mm_add_epi32(_mm_add_epi32(topChannel0_u_32x4, bottomChannel0_u_32x4), _mm_set1_epi32(8192)), 14);
			const __m128i resultChannel1_u_32x4 = _mm_srli_epi32(_mm_add_epi32(_mm_add_epi32(topChannel1_u_32x4, bottomChannel1_u_32x4), _mm_set1_epi32(8192)), 14);
			const __m128i resultChannel2_u_32x4 = _mm_srli_epi32(_mm_add_epi32(_mm_add_epi32(topChannel2_u_32x4, bottomChannel2_u_32x4), _mm_set1_epi32(8192)), 14);


			// interleaving

			const __m128i interleavedA_u_8x16 = _mm_shuffle_epi8(resultChannel0_u_32x4, SSE::set128i(0xFFFFFFFFFFFF0cFFull, 0xFF08FFFF04FFFF00ull));
			const __m128i interleavedB_u_8x16 = _mm_shuffle_epi8(resultChannel1_u_32x4, SSE::set128i(0xFFFFFFFFFF0cFFFFull, 0x08FFFF04FFFF00FFull));
			const __m128i interleavedC_u_8x16 = _mm_shuffle_epi8(resultChannel2_u_32x4, SSE::set128i(0xFFFFFFFF0cFFFF08ull, 0xFFFF04FFFF00FFFFull));


			const __m128i result_u_8x16 = _mm_or_si128(interleavedA_u_8x16, _mm_or_si128(interleavedB_u_8x16, interleavedC_u_8x16));

			ocean_assert(blocks1 == 0u);

			const bool isLastBlock = y + 1u == tPatchSize;

			if (isLastBlock)
			{
				uint8_t tempBuffer[16];
				_mm_storeu_si128((__m128i*)tempBuffer, result_u_8x16);

				memcpy(buffer, tempBuffer, remainingAfterBlocks4 * 3u);
			}
			else
			{
				_mm_storeu_si128((__m128i*)buffer, result_u_8x16);
			}

			imageTopLeft += remainingAfterBlocks4 * 3u;
			buffer += remainingAfterBlocks4 * 3u;
		}

		if constexpr (blocks1 != 0u)
		{
			const uint8_t* const imageBottomLeft = imageTopLeft + imageStrideElements;

			for (unsigned int n = 0u; n < blocks1; ++n)
			{
				for (unsigned int c = 0u; c < 3u; ++c)
				{
					buffer[n * 3u + c] = uint8_t((imageTopLeft[n * 3u + c] * factorTopLeft + imageTopLeft[n * 3u + 3u + c] * factorTopRight + imageBottomLeft[n * 3u + c] * factorBottomLeft + imageBottomLeft[n * 3u + 3u + c] * factorBottomRight + 8192u) / 16384u);
				}
			}

			imageTopLeft += blocks1 * 3u;
			buffer += blocks1 * 3u;
		}

		imageTopLeft += imageStrideElements - tPatchSize * 3u;
	}
}

template <unsigned int tChannels>
template <unsigned int tPatchSize>
inline void AdvancedFrameInterpolatorBilinearSSE::SpecializedForChannels<tChannels>::interpolateSquarePatch8BitPerChannel(const uint8_t* imageTopLeft, const unsigned int imageStrideElements, uint8_t* buffer, const unsigned int factorRight, const unsigned int factorBottom)
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
inline void AdvancedFrameInterpolatorBilinearSSE::interpolateSquarePatch8BitPerChannel(const uint8_t* const image, const unsigned int width, const unsigned int imagePaddingElements, uint8_t* buffer, const VectorT2<TScalar>& position)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize % 2u == 1u, "Invalid patch size!");

	ocean_assert(image != nullptr && buffer != nullptr);
	ocean_assert(tPatchSize + 1u <= width);

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

#endif // OCEAN_HARDWARE_SSE_VERSION >= 41

#endif // META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_INTERPOLATOR_BILINEAR_SSE_H
