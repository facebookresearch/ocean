/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_ADVANCED_SUM_SQUARE_DIFFERENCES_SSE_H
#define META_OCEAN_CV_ADVANCED_ADVANCED_SUM_SQUARE_DIFFERENCES_SSE_H

#include "ocean/cv/advanced/Advanced.h"

#include "ocean/math/Math.h"

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

#include "ocean/cv/SSE.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

/**
 * This class implements sum of square difference calculation functions allowing to determine the SSE with sub-pixel accuracy using SSD SIMD instructions.
 * @ingroup cvadvanced
 */
class OCEAN_CV_ADVANCED_EXPORT AdvancedSumSquareDifferencesSSE
{
	public:

		/**
		 * Returns the sum of square differences for an image patch determined between two individual images.
		 * @param image0 The image in which the first patch is located, must be valid
		 * @param image1 The image in which the second patch is located, must be valid
		 * @param width0 The width of the first image, in pixels, with range [tPatchSize + 1, infinity)
		 * @param width1 The width of the second image, in pixels, with range [tPatchSize + 1, infinity)
		 * @param centerX0 Horizontal center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize/2, width - tPatchSize/2 - 1)
		 * @param centerY0 Vertical center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize/2, height - tPatchSize/2 - 1)
		 * @param centerX1 Horizontal center position of the (tPatchSize x tPatchSize) block in the second frame, with range [tPatchSize/2, width - tPatchSize/2 - 1)
		 * @param centerY1 Vertical center position of the (tPatchSize x tPatchSize) block in the second frame, with range [tPatchSize/2, height - tPatchSize/2 - 1)
		 * @param image0PaddingElements The number of padding elements at the end of each row of the first image, in elements, with range [0, infinity)
		 * @param image1PaddingElements The number of padding elements at the end of each row of the second image, in elements, with range [0, infinity)
		 * @return The resulting sum of square differences, with range [0, infinity)
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static inline uint32_t patch8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int width1, const Scalar centerX0, const Scalar centerY0, const Scalar centerX1, const Scalar centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements);

		/**
		 * Returns the sum of square differences for an image patch determined between two individual images.
		 * @param image0 The image in which the first patch is located, must be valid
		 * @param image1 The image in which the second patch is located, must be valid
		 * @param width0 The width of the first image, in pixels, with range [tPatchSize + 1, infinity)
		 * @param width1 The width of the second image, in pixels, with range [tPatchSize + 1, infinity)
		 * @param centerX0 Horizontal center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize/2, width - tPatchSize/2)
		 * @param centerY0 Vertical center position of the (tPatchSize x tPatchSize) block in the first frame, with range [tPatchSize/2, height - tPatchSize/2)
		 * @param centerX1 Horizontal center position of the (tPatchSize x tPatchSize) block in the second frame, with range [tPatchSize/2, width - tPatchSize/2 - 1)
		 * @param centerY1 Vertical center position of the (tPatchSize x tPatchSize) block in the second frame, with range [tPatchSize/2, height - tPatchSize/2 - 1)
		 * @param image0PaddingElements The number of padding elements at the end of each row of the first image, in elements, with range [0, infinity)
		 * @param image1PaddingElements The number of padding elements at the end of each row of the second image, in elements, with range [0, infinity)
		 * @return The resulting sum of square differences, with range [0, infinity)
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static inline uint32_t patch8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const Scalar centerX1, const Scalar centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements);

	private:

		/**
		 * Returns the sum of square differences for an image patch determined between two individual images.
		 * @param imageTopLeft0 The top left corner of the image patch in the first image, must be valid
		 * @param imageTopLeft1 The top left corner of the image patch in the second image, must be valid
		 * @param image0StrideElements The number of elements between two rows in the first image, in elements, with range [width0 * tChannels, infinity)
		 * @param image1StrideElements The number of elements between two rows in the second image, in elements, with range [width0 * tChannels, infinity)
		 * @param fx0 Horizontal interpolation factor for the first image, with range [0, 128]
		 * @param fy0 Vertical interpolation factor for the first image, with range [0, 128]
		 * @param fx1 Horizontal interpolation factor for the second image, with range [0, 128]
		 * @param fy1 Vertical interpolation factor for the second image, with range [0, 128]
		 * @return The resulting sum of square differences, with range [0, infinity)
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static inline uint32_t patch8BitPerChannel(const uint8_t* const imageTopLeft0, const uint8_t* const imageTopLeft1, const unsigned int image0StrideElements, const unsigned int image1StrideElements, const unsigned int fx0, const unsigned int fy0, const unsigned int fx1, const unsigned int fy1);

		/**
		 * Returns the sum of square differences for an image patch determined between two individual images.
		 * @param imageTopLeft0 The top left corner of the image patch in the first image, must be valid
		 * @param imageTopLeft1 The top left corner of the image patch in the second image, must be valid
		 * @param image0StrideElements The number of elements between two rows in the first image, in elements, with range [width0 * tChannels, infinity)
		 * @param image1StrideElements The number of elements between two rows in the second image, in elements, with range [width0 * tChannels, infinity)
		 * @param fx1 Horizontal interpolation factor for the second image, with range [0, 128]
		 * @param fy1 Vertical interpolation factor for the second image, with range [0, 128]
		 * @return The resulting sum of square differences, with range [0, infinity)
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static inline uint32_t patch8BitPerChannel(const uint8_t* const imageTopLeft0, const uint8_t* const imageTopLeft1, const unsigned int image0StrideElements, const unsigned int image1StrideElements, const unsigned int fx1, const unsigned int fy1);
};

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t AdvancedSumSquareDifferencesSSE::patch8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int width1, const Scalar centerX0, const Scalar centerY0, const Scalar centerX1, const Scalar centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	ocean_assert(image0 != nullptr && image1 != nullptr);

	ocean_assert(width0 > tPatchSize);
	ocean_assert(width1 > tPatchSize);

	const unsigned int tPatchSize_2 = tPatchSize / 2u;

	ocean_assert(centerX0 >= Scalar(tPatchSize_2) && centerX0 < Scalar(width0 - tPatchSize_2 - 1u));
	ocean_assert(centerY0 >= Scalar(tPatchSize_2));

	ocean_assert(centerX1 >= Scalar(tPatchSize_2) && centerX1 < Scalar(width1 - tPatchSize_2 - 1u));
	ocean_assert(centerY1 >= Scalar(tPatchSize_2));

	const unsigned int image0StrideElements = width0 * tChannels + image0PaddingElements;
	const unsigned int image1StrideElements = width1 * tChannels + image1PaddingElements;

	const unsigned int left0 = (unsigned int)(centerX0);
	const unsigned int top0 = (unsigned int)(centerY0);

	const unsigned int left1 = (unsigned int)(centerX1);
	const unsigned int top1 = (unsigned int)(centerY1);

	const Scalar scalarFx0 = centerX0 - Scalar(left0);
	const Scalar scalarFy0 = centerY0 - Scalar(top0);

	ocean_assert(scalarFx0 >= 0 && scalarFx0 <= 1u);
	ocean_assert(scalarFy0 >= 0 && scalarFy0 <= 1u);

	const unsigned int fx0 = (unsigned int)(Scalar(128) * scalarFx0 + Scalar(0.5));
	const unsigned int fy0 = (unsigned int)(Scalar(128) * scalarFy0 + Scalar(0.5));

	const Scalar scalarFx1 = centerX1 - Scalar(left1);
	const Scalar scalarFy1 = centerY1 - Scalar(top1);

	ocean_assert(scalarFx1 >= 0 && scalarFx1 <= 1);
	ocean_assert(scalarFy1 >= 0 && scalarFy1 <= 1);

	const unsigned int fx1 = (unsigned int)(Scalar(128) * scalarFx1 + Scalar(0.5));
	const unsigned int fy1 = (unsigned int)(Scalar(128) * scalarFy1 + Scalar(0.5));

	const uint8_t* imageTopLeft0 = image0 + (top0 - tPatchSize_2) * image0StrideElements + (left0 - tPatchSize_2) * tChannels;
	const uint8_t* imageTopLeft1 = image1 + (top1 - tPatchSize_2) * image1StrideElements + (left1 - tPatchSize_2) * tChannels;

	return patch8BitPerChannel<tChannels, tPatchSize>(imageTopLeft0, imageTopLeft1, image0StrideElements, image1StrideElements, fx0, fy0, fx1, fy1);
}

template <unsigned int tChannels, unsigned int tPatchSize>
inline uint32_t AdvancedSumSquareDifferencesSSE::patch8BitPerChannel(const uint8_t* image0, const uint8_t* image1, const unsigned int width0, const unsigned int width1, const unsigned int centerX0, const unsigned int centerY0, const Scalar centerX1, const Scalar centerY1, const unsigned int image0PaddingElements, const unsigned int image1PaddingElements)
{
	ocean_assert(image0 != nullptr && image1 != nullptr);

	ocean_assert(width0 > tPatchSize);
	ocean_assert(width1 > tPatchSize);

	const unsigned int tPatchSize_2 = tPatchSize / 2u;

	ocean_assert(centerX0 >= tPatchSize_2 && centerX0 < width0 - tPatchSize_2);
	ocean_assert(centerY0 >= Scalar(tPatchSize_2));

	ocean_assert(centerX1 >= tPatchSize_2 && centerX1 < width1 - tPatchSize_2 - 1u);
	ocean_assert(centerY1 >= Scalar(tPatchSize_2));

	const unsigned int image0StrideElements = width0 * tChannels + image0PaddingElements;
	const unsigned int image1StrideElements = width1 * tChannels + image1PaddingElements;

	const unsigned int left1 = (unsigned int)(centerX1);
	const unsigned int top1 = (unsigned int)(centerY1);

	const Scalar scalarFx1 = centerX1 - Scalar(left1);
	const Scalar scalarFy1 = centerY1 - Scalar(top1);

	ocean_assert(scalarFx1 >= 0 && scalarFx1 <= 1);
	ocean_assert(scalarFy1 >= 0 && scalarFy1 <= 1);

	const unsigned int fx1 = (unsigned int)(Scalar(128) * scalarFx1 + Scalar(0.5));
	const unsigned int fy1 = (unsigned int)(Scalar(128) * scalarFy1 + Scalar(0.5));

	const uint8_t* imageTopLeft0 = image0 + (centerY0 - tPatchSize_2) * image0StrideElements + (centerX0 - tPatchSize_2) * tChannels;
	const uint8_t* imageTopLeft1 = image1 + (top1 - tPatchSize_2) * image1StrideElements + (left1 - tPatchSize_2) * tChannels;

	return patch8BitPerChannel<tChannels, tPatchSize>(imageTopLeft0, imageTopLeft1, image0StrideElements, image1StrideElements, fx1, fy1);
}

template <>
inline uint32_t AdvancedSumSquareDifferencesSSE::patch8BitPerChannel<1u, 5u>(const uint8_t* const imageTopLeft0, const uint8_t* const imageTopLeft1, const unsigned int image0StrideElements, const unsigned int image1StrideElements, const unsigned int fx0, const unsigned int fy0, const unsigned int fx1, const unsigned int fy1)
{
	ocean_assert(fx0 <= 128u && fy0 <= 128u);
	ocean_assert(fx1 <= 128u && fy1 <= 128u);

	SSE::prefetchT0(imageTopLeft0);
	SSE::prefetchT0(imageTopLeft0 + image0StrideElements);

	SSE::prefetchT0(imageTopLeft1);
	SSE::prefetchT0(imageTopLeft1 + image1StrideElements);

	const unsigned int fx0_ = 128u - fx0;
	const unsigned int fy0_ = 128u - fy0;

	const unsigned int fx1_ = 128u - fx1;
	const unsigned int fy1_ = 128u - fy1;

	const unsigned int f0x_y_ = fx0_ * fy0_;
	const unsigned int f0xy_ = fx0 * fy0_;
	const unsigned int f0x_y = fx0_ * fy0;
	const unsigned int f0xy = fx0 * fy0;

	const unsigned int f1x_y_ = fx1_ * fy1_;
	const unsigned int f1xy_ = fx1 * fy1_;
	const unsigned int f1x_y = fx1_ * fy1;
	const unsigned int f1xy = fx1 * fy1;

	const __m128i __f0x_y_ = _mm_set1_epi16(short(f0x_y_));
	const __m128i __f0xy_ = _mm_set1_epi16(short(f0xy_));
	const __m128i __f0x_y = _mm_set1_epi16(short(f0x_y));
	const __m128i __f0xy = _mm_set1_epi16(short(f0xy));

	const __m128i __f1x_y_ = _mm_set1_epi16(short(f1x_y_));
	const __m128i __f1xy_ = _mm_set1_epi16(short(f1xy_));
	const __m128i __f1x_y = _mm_set1_epi16(short(f1x_y));
	const __m128i __f1xy = _mm_set1_epi16(short(f1xy));

	SSE::prefetchT0(imageTopLeft0 + 2u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 2u * image1StrideElements);

	// row0 -> [-----------00000]
	const __m128i image0_row0 = _mm_loadu_si64(imageTopLeft0);
	const __m128i image0_row1 = _mm_loadu_si64(imageTopLeft0 + image0StrideElements);
	__m128i interpolation0 = _mm_slli_si128(SSE::interpolation1Channel8Bit8Elements(image0_row0, image0_row1, __f0x_y_, __f0xy_, __f0x_y, __f0xy), 11);

	const __m128i image1_row0 = _mm_loadu_si64(imageTopLeft1);
	const __m128i image1_row1 = _mm_loadu_si64(imageTopLeft1 + image1StrideElements);
	__m128i interpolation1 = _mm_slli_si128(SSE::interpolation1Channel8Bit8Elements(image1_row0, image1_row1, __f1x_y_, __f1xy_, __f1x_y, __f1xy), 11);

	SSE::prefetchT0(imageTopLeft0 + 3u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 3u * image1StrideElements);


	// row1 -> [------1111100000]
	const __m128i image0_row2 = _mm_loadu_si64(imageTopLeft0 + 2u * image0StrideElements);
	__m128i mask = SSE::set128i(0x0000000000FFFFFFull, 0xFFFFFFFFFFFFFFFFull);
	interpolation0 = _mm_blendv_epi8(interpolation0, _mm_slli_si128(SSE::interpolation1Channel8Bit8Elements(image0_row1, image0_row2, __f0x_y_, __f0xy_, __f0x_y, __f0xy), 6), mask);

	const __m128i image1_row2 = _mm_loadu_si64(imageTopLeft1 + 2u * image1StrideElements);
	interpolation1 = _mm_blendv_epi8(interpolation1, _mm_slli_si128(SSE::interpolation1Channel8Bit8Elements(image1_row1, image1_row2, __f1x_y_, __f1xy_, __f1x_y, __f1xy), 6), mask);

	SSE::prefetchT0(imageTopLeft0 + 4u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 4u * image1StrideElements);

#ifdef OCEAN_COMPILER_CLANG

	// workaround for Clang compiler bug - with optimizations the unused SSE bytes are not set to zero

	// row2 -> [-222221111100000]
	const __m128i image0_row3 = _mm_loadu_si64(imageTopLeft0 + 3u * image0StrideElements);
	mask = SSE::set128i(0x0000000000000000ull, 0x0000FFFFFFFFFFFFull);
	interpolation0 = _mm_blendv_epi8(interpolation0, _mm_slli_si128(SSE::interpolation1Channel8Bit8Elements(image0_row2, image0_row3, __f0x_y_, __f0xy_, __f0x_y, __f0xy), 1), mask);

	const __m128i image1_row3 = _mm_loadu_si64(imageTopLeft1 + 3u * image1StrideElements);
	interpolation1 = _mm_blendv_epi8(interpolation1, _mm_slli_si128(SSE::interpolation1Channel8Bit8Elements(image1_row2, image1_row3, __f1x_y_, __f1xy_, __f1x_y, __f1xy), 1), mask);

#else

	// row2 -> [22222-1111100000]
	const __m128i image0_row3 = _mm_loadu_si64(imageTopLeft0 + 3u * image0StrideElements);
	mask = SSE::set128i(0x0000000000000000ull, 0x000000FFFFFFFFFFull);
	interpolation0 = _mm_blendv_epi8(interpolation0, SSE::interpolation1Channel8Bit8Elements(image0_row2, image0_row3, __f0x_y_, __f0xy_, __f0x_y, __f0xy), mask);

	const __m128i image1_row3 = _mm_loadu_si64(imageTopLeft1 + 3u * image1StrideElements);
	interpolation1 = _mm_blendv_epi8(interpolation1, SSE::interpolation1Channel8Bit8Elements(image1_row2, image1_row3, __f1x_y_, __f1xy_, __f1x_y, __f1xy), mask);

#endif

	// intermediate ssd
	__m128i result = SSE::sumSquareDifference8Bit16Elements(interpolation0, interpolation1);

	SSE::prefetchT0(imageTopLeft0 + 5u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 5u * image1StrideElements);


	// row3 -> [33333-----------]
	__m128i image0_row4 = _mm_loadu_si64(imageTopLeft0 + 4u * image0StrideElements);
	interpolation0 = SSE::interpolation1Channel8Bit8Elements(image0_row3, image0_row4, __f0x_y_, __f0xy_, __f0x_y, __f0xy);

	__m128i image1_row4 = _mm_loadu_si64(imageTopLeft1 + 4u * image1StrideElements);
	interpolation1 = SSE::interpolation1Channel8Bit8Elements(image1_row3, image1_row4, __f1x_y_, __f1xy_, __f1x_y, __f1xy);

	// row3 -> [3333344444------]
	__m128i image0_row5 = _mm_loadu_si64(imageTopLeft0 + 5u * image0StrideElements);
	mask = SSE::set128i(0x000000000000FFFFull, 0xFFFFFF0000000000ull);
	interpolation0 = _mm_blendv_epi8(interpolation0, _mm_slli_si128(SSE::interpolation1Channel8Bit8Elements(image0_row4, image0_row5, __f0x_y_, __f0xy_, __f0x_y, __f0xy), 5), mask);

	__m128i image1_row5 = _mm_loadu_si64(imageTopLeft1 + 5u * image1StrideElements);
	interpolation1 = _mm_blendv_epi8(interpolation1, _mm_slli_si128(SSE::interpolation1Channel8Bit8Elements(image1_row4, image1_row5, __f1x_y_, __f1xy_, __f1x_y, __f1xy), 5), mask);

#ifdef OCEAN_COMPILER_CLANG

	// workaround for Clang compiler bug - with optimizations the unused SSE bytes are not set to zero

	interpolation0 = _mm_slli_si128(interpolation0, 6);
	interpolation1 = _mm_slli_si128(interpolation1, 6);

#endif // OCEAN_COMPILER_CLANG

	// ssd row04[0:7]
	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(interpolation0, interpolation1));

	return SSE::sum_u32_4(result);
}

template <>
inline uint32_t AdvancedSumSquareDifferencesSSE::patch8BitPerChannel<2u, 5u>(const uint8_t* const imageTopLeft0, const uint8_t* const imageTopLeft1, const unsigned int image0StrideElements, const unsigned int image1StrideElements, const unsigned int fx0, const unsigned int fy0, const unsigned int fx1, const unsigned int fy1)
{
	ocean_assert(fx0 <= 128u && fy0 <= 128u);
	ocean_assert(fx1 <= 128u && fy1 <= 128u);

	SSE::prefetchT0(imageTopLeft0);
	SSE::prefetchT0(imageTopLeft0 + image0StrideElements);

	SSE::prefetchT0(imageTopLeft1);
	SSE::prefetchT0(imageTopLeft1 + image1StrideElements);

	const unsigned int fx0_ = 128u - fx0;
	const unsigned int fy0_ = 128u - fy0;

	const unsigned int fx1_ = 128u - fx1;
	const unsigned int fy1_ = 128u - fy1;

	const unsigned int f0x_y_ = fx0_ * fy0_;
	const unsigned int f0xy_ = fx0 * fy0_;
	const unsigned int f0x_y = fx0_ * fy0;
	const unsigned int f0xy = fx0 * fy0;

	const unsigned int f1x_y_ = fx1_ * fy1_;
	const unsigned int f1xy_ = fx1 * fy1_;
	const unsigned int f1x_y = fx1_ * fy1;
	const unsigned int f1xy = fx1 * fy1;

	const __m128i __f0x_y_ = _mm_set1_epi16(short(f0x_y_));
	const __m128i __f0xy_ = _mm_set1_epi16(short(f0xy_));
	const __m128i __f0x_y = _mm_set1_epi16(short(f0x_y));
	const __m128i __f0xy = _mm_set1_epi16(short(f0xy));

	const __m128i __f1x_y_ = _mm_set1_epi16(short(f1x_y_));
	const __m128i __f1xy_ = _mm_set1_epi16(short(f1xy_));
	const __m128i __f1x_y = _mm_set1_epi16(short(f1x_y));
	const __m128i __f1xy = _mm_set1_epi16(short(f1xy));

	SSE::prefetchT0(imageTopLeft0 + 2u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 2u * image1StrideElements);

	// row0
	// image0 row0[0:7]
	__m128i image0_row0 = _mm_lddqu_si128((__m128i*)imageTopLeft0);
	__m128i image0_row1 = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + image0StrideElements));
	__m128i interpolation0 = SSE::interpolation2Channel16Bit8Elements(image0_row0, image0_row1, __f0x_y_, __f0xy_, __f0x_y, __f0xy);

	// image1 row0[0:7]
	__m128i image1_row0 = _mm_lddqu_si128((__m128i*)imageTopLeft1);
	__m128i image1_row1 = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + image1StrideElements));
	__m128i interpolation1 = SSE::interpolation2Channel16Bit8Elements(image1_row0, image1_row1, __f1x_y_, __f1xy_, __f1x_y, __f1xy);

	unsigned int localResult = SSE::ssd2Channel16Bit1x1(imageTopLeft0 + 8, imageTopLeft1 + 8, image0StrideElements, image1StrideElements, f0x_y_, f0xy_, f0x_y, f0xy, f1x_y_, f1xy_, f1x_y, f1xy)
								+ SSE::ssd2Channel16Bit1x1(imageTopLeft0 + 9, imageTopLeft1 + 9, image0StrideElements, image1StrideElements, f0x_y_, f0xy_, f0x_y, f0xy, f1x_y_, f1xy_, f1x_y, f1xy);

	SSE::prefetchT0(imageTopLeft0 + 3u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 3u * image1StrideElements);


	// row1
	// image0 row1[0:7]
	__m128i image0_row2 = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + 2u * image0StrideElements));
	interpolation0 = _mm_or_si128(_mm_slli_si128(SSE::interpolation2Channel16Bit8Elements(image0_row1, image0_row2, __f0x_y_, __f0xy_, __f0x_y, __f0xy), 8), interpolation0);

	// image1 row1[0:7]
	__m128i image1_row2 = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 2u * image1StrideElements));
	interpolation1 = _mm_or_si128(_mm_slli_si128(SSE::interpolation2Channel16Bit8Elements(image1_row1, image1_row2, __f1x_y_, __f1xy_, __f1x_y, __f1xy), 8), interpolation1);

	// ssd row01[0:7]
	__m128i result = SSE::sumSquareDifference8Bit16Elements(interpolation0, interpolation1);

	localResult += SSE::ssd2Channel16Bit1x1(imageTopLeft0 + 1u * image0StrideElements + 8u, imageTopLeft1 + 1u * image1StrideElements + 8u, image0StrideElements, image1StrideElements, f0x_y_, f0xy_, f0x_y, f0xy, f1x_y_, f1xy_, f1x_y, f1xy)
								+ SSE::ssd2Channel16Bit1x1(imageTopLeft0 + 1u * image0StrideElements + 9u, imageTopLeft1 + 1u * image1StrideElements + 9u, image0StrideElements, image1StrideElements, f0x_y_, f0xy_, f0x_y, f0xy, f1x_y_, f1xy_, f1x_y, f1xy);

	SSE::prefetchT0(imageTopLeft0 + 4u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 4u * image1StrideElements);


	// row 2
	// image0 row2[0:7]
	__m128i image0_row3 = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + 3u * image0StrideElements));
	interpolation0 = SSE::interpolation2Channel16Bit8Elements(image0_row2, image0_row3, __f0x_y_, __f0xy_, __f0x_y, __f0xy);

	// image1 row2[0:7]
	__m128i image1_row3 = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 3u * image1StrideElements));
	interpolation1 = SSE::interpolation2Channel16Bit8Elements(image1_row2, image1_row3, __f1x_y_, __f1xy_, __f1x_y, __f1xy);

	localResult += SSE::ssd2Channel16Bit1x1(imageTopLeft0 + 2u * image0StrideElements + 8u, imageTopLeft1 + 2u * image1StrideElements + 8u, image0StrideElements, image1StrideElements, f0x_y_, f0xy_, f0x_y, f0xy, f1x_y_, f1xy_, f1x_y, f1xy)
								+ SSE::ssd2Channel16Bit1x1(imageTopLeft0 + 2u * image0StrideElements + 9u, imageTopLeft1 + 2u * image1StrideElements + 9u, image0StrideElements, image1StrideElements, f0x_y_, f0xy_, f0x_y, f0xy, f1x_y_, f1xy_, f1x_y, f1xy);

	SSE::prefetchT0(imageTopLeft0 + 5u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 5u * image1StrideElements);


	// row 3
	// image0 row3[0:7]
	__m128i image0_row4 = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + 4u * image0StrideElements));
	interpolation0 = _mm_or_si128(_mm_slli_si128(SSE::interpolation2Channel16Bit8Elements(image0_row3, image0_row4, __f0x_y_, __f0xy_, __f0x_y, __f0xy), 8), interpolation0);

	// image1 row3[0:7]
	__m128i image1_row4 = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 4u * image1StrideElements));
	interpolation1 = _mm_or_si128(_mm_slli_si128(SSE::interpolation2Channel16Bit8Elements(image1_row3, image1_row4, __f1x_y_, __f1xy_, __f1x_y, __f1xy), 8), interpolation1);

	// ssd row03[0:7]
	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(interpolation0, interpolation1));

	localResult += SSE::ssd2Channel16Bit1x1(imageTopLeft0 + 3u * image0StrideElements + 8u, imageTopLeft1 + 3u * image1StrideElements + 8u, image0StrideElements, image1StrideElements, f0x_y_, f0xy_, f0x_y, f0xy, f1x_y_, f1xy_, f1x_y, f1xy)
								+ SSE::ssd2Channel16Bit1x1(imageTopLeft0 + 3u * image0StrideElements + 9u, imageTopLeft1 + 3u * image1StrideElements + 9u, image0StrideElements, image1StrideElements, f0x_y_, f0xy_, f0x_y, f0xy, f1x_y_, f1xy_, f1x_y, f1xy);


	// row 4
	// image0 row4[0:7]
	__m128i image0_row5 = _mm_srli_si128(_mm_lddqu_si128((__m128i*)(imageTopLeft0 + 5u * image0StrideElements - 2u)), 2); // here we start 2 bytes earlyer (and shift the bytes later) to avoid a segmentation fault
	interpolation0 = SSE::interpolation2Channel16Bit8Elements(image0_row4, image0_row5, __f0x_y_, __f0xy_, __f0x_y, __f0xy);

	// image0 row4[0:7]
	__m128i image1_row5 = _mm_srli_si128(_mm_lddqu_si128((__m128i*)(imageTopLeft1 + 5u * image1StrideElements - 2u)), 2); // here we start 2 bytes earlyer (and shift the bytes later) to avoid a segmentation fault
	interpolation1 = SSE::interpolation2Channel16Bit8Elements(image1_row4, image1_row5, __f1x_y_, __f1xy_, __f1x_y, __f1xy);

	// ssd row04[0:7]
	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(interpolation0, interpolation1));

	localResult += SSE::ssd2Channel16Bit1x1(imageTopLeft0 + 4u * image0StrideElements + 8u, imageTopLeft1 + 4u * image1StrideElements + 8u, image0StrideElements, image1StrideElements, f0x_y_, f0xy_, f0x_y, f0xy, f1x_y_, f1xy_, f1x_y, f1xy)
								+ SSE::ssd2Channel16Bit1x1(imageTopLeft0 + 4u * image0StrideElements + 9u, imageTopLeft1 + 4u * image1StrideElements + 9u, image0StrideElements, image1StrideElements, f0x_y_, f0xy_, f0x_y, f0xy, f1x_y_, f1xy_, f1x_y, f1xy);

	return SSE::sum_u32_4(result) + localResult;
}

template <>
inline uint32_t AdvancedSumSquareDifferencesSSE::patch8BitPerChannel<3u, 5u>(const uint8_t* const imageTopLeft0, const uint8_t* const imageTopLeft1, const unsigned int image0StrideElements, const unsigned int image1StrideElements, const unsigned int fx0, const unsigned int fy0, const unsigned int fx1, const unsigned int fy1)
{
	ocean_assert(fx0 <= 128u && fy0 <= 128u);
	ocean_assert(fx1 <= 128u && fy1 <= 128u);

	SSE::prefetchT0(imageTopLeft0);
	SSE::prefetchT0(imageTopLeft0 + image0StrideElements);

	SSE::prefetchT0(imageTopLeft1);
	SSE::prefetchT0(imageTopLeft1 + image1StrideElements);

	const unsigned int fx0_ = 128u - fx0;
	const unsigned int fy0_ = 128u - fy0;

	const unsigned int fx1_ = 128u - fx1;
	const unsigned int fy1_ = 128u - fy1;

	const __m128i f0x_y_ = _mm_set1_epi16(short(fx0_ * fy0_));
	const __m128i f0xy_ = _mm_set1_epi16(short(fx0 * fy0_));
	const __m128i f0x_y = _mm_set1_epi16(short(fx0_ * fy0));
	const __m128i f0xy = _mm_set1_epi16(short(fx0 * fy0));

	const __m128i f1x_y_ = _mm_set1_epi16(short(fx1_ * fy1_));
	const __m128i f1xy_ = _mm_set1_epi16(short(fx1 * fy1_));
	const __m128i f1x_y = _mm_set1_epi16(short(fx1_ * fy1));
	const __m128i f1xy = _mm_set1_epi16(short(fx1 * fy1));

	SSE::prefetchT0(imageTopLeft0 + 2u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 2u * image1StrideElements);

	// row 0
	__m128i image0_row0Front = _mm_lddqu_si128((__m128i*)imageTopLeft0);
	__m128i image0_row1Front = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + image0StrideElements));
	__m128i interpolation0 = SSE::interpolation3Channel24Bit8Elements(image0_row0Front, image0_row1Front, f0x_y_, f0xy_, f0x_y, f0xy);

	__m128i image0_row0Back = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + 8u));
	__m128i image0_row1Back = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + image0StrideElements + 8u));
	interpolation0 = _mm_or_si128(interpolation0, _mm_slli_si128(SSE::interpolation3Channel24Bit8Elements(image0_row0Back, image0_row1Back, f0x_y_, f0xy_, f0x_y, f0xy), 9));

	__m128i image1_row0Front = _mm_lddqu_si128((__m128i*)imageTopLeft1);
	__m128i image1_row1Front = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + image1StrideElements));
	__m128i interpolation1 = SSE::interpolation3Channel24Bit8Elements(image1_row0Front, image1_row1Front, f1x_y_, f1xy_, f1x_y, f1xy);

	__m128i image1_row0Back = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 8u));
	__m128i image1_row1Back = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + image1StrideElements + 8u));
	interpolation1 = _mm_or_si128(interpolation1, _mm_slli_si128(SSE::interpolation3Channel24Bit8Elements(image1_row0Back, image1_row1Back, f1x_y_, f1xy_, f1x_y, f1xy), 9));

	__m128i result = SSE::sumSquareDifference8Bit16Elements(interpolation0, interpolation1);


	SSE::prefetchT0(imageTopLeft0 + 3u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 3u * image1StrideElements);


	// row 1
	__m128i image0_row2Front = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + 2u * image0StrideElements));
	interpolation0 = SSE::interpolation3Channel24Bit8Elements(image0_row1Front, image0_row2Front, f0x_y_, f0xy_, f0x_y, f0xy);

	__m128i image0_row2Back = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + 2u * image0StrideElements + 8u));
	interpolation0 = _mm_or_si128(interpolation0, _mm_slli_si128(SSE::interpolation3Channel24Bit8Elements(image0_row1Back, image0_row2Back, f0x_y_, f0xy_, f0x_y, f0xy), 9));

	__m128i image1_row2Front = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 2u * image1StrideElements));
	interpolation1 = SSE::interpolation3Channel24Bit8Elements(image1_row1Front, image1_row2Front, f1x_y_, f1xy_, f1x_y, f1xy);

	__m128i image1_row2Back = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 2u * image1StrideElements + 8u));
	interpolation1 = _mm_or_si128(interpolation1, _mm_slli_si128(SSE::interpolation3Channel24Bit8Elements(image1_row1Back, image1_row2Back, f1x_y_, f1xy_, f1x_y, f1xy), 9));

	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(interpolation0, interpolation1));


	SSE::prefetchT0(imageTopLeft0 + 4u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 4u * image1StrideElements);


	// row 2
	__m128i image0_row3Front = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + 3u * image0StrideElements));
	interpolation0 = SSE::interpolation3Channel24Bit8Elements(image0_row2Front, image0_row3Front, f0x_y_, f0xy_, f0x_y, f0xy);

	__m128i image0_row3Back = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + 3u * image0StrideElements + 8u));
	interpolation0 = _mm_or_si128(interpolation0, _mm_slli_si128(SSE::interpolation3Channel24Bit8Elements(image0_row2Back, image0_row3Back, f0x_y_, f0xy_, f0x_y, f0xy), 9));

	__m128i image1_row3Front = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 3u * image1StrideElements));
	interpolation1 = SSE::interpolation3Channel24Bit8Elements(image1_row2Front, image1_row3Front, f1x_y_, f1xy_, f1x_y, f1xy);

	__m128i image1_row3Back = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 3u * image1StrideElements + 8u));
	interpolation1 = _mm_or_si128(interpolation1, _mm_slli_si128(SSE::interpolation3Channel24Bit8Elements(image1_row2Back, image1_row3Back, f1x_y_, f1xy_, f1x_y, f1xy), 9));

	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(interpolation0, interpolation1));


	SSE::prefetchT0(imageTopLeft0 + 5u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 5u * image1StrideElements);

	// row 3
	__m128i image0_row4Front = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + 4u * image0StrideElements));
	interpolation0 = SSE::interpolation3Channel24Bit8Elements(image0_row3Front, image0_row4Front, f0x_y_, f0xy_, f0x_y, f0xy);

	__m128i image0_row4Back = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + 4u * image0StrideElements + 8u));
	interpolation0 = _mm_or_si128(interpolation0, _mm_slli_si128(SSE::interpolation3Channel24Bit8Elements(image0_row3Back, image0_row4Back, f0x_y_, f0xy_, f0x_y, f0xy), 9));

	__m128i image1_row4Front = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 4u * image1StrideElements));
	interpolation1 = SSE::interpolation3Channel24Bit8Elements(image1_row3Front, image1_row4Front, f1x_y_, f1xy_, f1x_y, f1xy);

	__m128i image1_row4Back = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 4u * image1StrideElements + 8u));
	interpolation1 = _mm_or_si128(interpolation1, _mm_slli_si128(SSE::interpolation3Channel24Bit8Elements(image1_row3Back, image1_row4Back, f1x_y_, f1xy_, f1x_y, f1xy), 9));

	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(interpolation0, interpolation1));



	// row 4
	__m128i image0_row5Front = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + 5u * image0StrideElements));
	interpolation0 = SSE::interpolation3Channel24Bit8Elements(image0_row4Front, image0_row5Front, f0x_y_, f0xy_, f0x_y, f0xy);

	__m128i image0_row5Back = _mm_srli_si128(_mm_lddqu_si128((__m128i*)(imageTopLeft0 + 5u * image0StrideElements + 8u - 6u)), 6); // here we start 6 bytes earlyer (and shift the bytes later) to avoid a segmentation fault
	interpolation0 = _mm_or_si128(interpolation0, _mm_slli_si128(SSE::interpolation3Channel24Bit8Elements(image0_row4Back, image0_row5Back, f0x_y_, f0xy_, f0x_y, f0xy), 9));

	__m128i image1_row5Front = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 5u * image1StrideElements));
	interpolation1 = SSE::interpolation3Channel24Bit8Elements(image1_row4Front, image1_row5Front, f1x_y_, f1xy_, f1x_y, f1xy);

	__m128i image1_row5Back = _mm_srli_si128(_mm_lddqu_si128((__m128i*)(imageTopLeft1 + 5u * image1StrideElements + 8u - 6u)), 6); // here we start 6 bytes earlyer (and shift the bytes later) to avoid a segmentation fault
	interpolation1 = _mm_or_si128(interpolation1, _mm_slli_si128(SSE::interpolation3Channel24Bit8Elements(image1_row4Back, image1_row5Back, f1x_y_, f1xy_, f1x_y, f1xy), 9));

	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(interpolation0, interpolation1));

	return SSE::sum_u32_4(result);
}

template <>
inline uint32_t AdvancedSumSquareDifferencesSSE::patch8BitPerChannel<4u, 5u>(const uint8_t* const imageTopLeft0, const uint8_t* const imageTopLeft1, const unsigned int image0StrideElements, const unsigned int image1StrideElements, const unsigned int fx0, const unsigned int fy0, const unsigned int fx1, const unsigned int fy1)
{
	SSE::prefetchT0(imageTopLeft0);
	SSE::prefetchT0(imageTopLeft0 + image0StrideElements);

	SSE::prefetchT0(imageTopLeft1);
	SSE::prefetchT0(imageTopLeft1 + image1StrideElements);

	const unsigned int fx0_ = 128u - fx0;
	const unsigned int fy0_ = 128u - fy0;

	const unsigned int fx1_ = 128u - fx1;
	const unsigned int fy1_ = 128u - fy1;

	const __m128i f0x_y_ = _mm_set1_epi16(short(fx0_ * fy0_));
	const __m128i f0xy_ = _mm_set1_epi16(short(fx0 * fy0_));
	const __m128i f0x_y = _mm_set1_epi16(short(fx0_ * fy0));
	const __m128i f0xy = _mm_set1_epi16(short(fx0 * fy0));

	const __m128i f1x_y_ = _mm_set1_epi16(short(fx1_ * fy1_));
	const __m128i f1xy_ = _mm_set1_epi16(short(fx1 * fy1_));
	const __m128i f1x_y = _mm_set1_epi16(short(fx1_ * fy1));
	const __m128i f1xy = _mm_set1_epi16(short(fx1 * fy1));

	SSE::prefetchT0(imageTopLeft0 + 2u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 2u * image1StrideElements);

	// row0
	// image0 row0 [0:7]
	__m128i image0_row0Front = _mm_lddqu_si128((__m128i*)imageTopLeft0);
	__m128i image0_row1Front = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + image0StrideElements));
	__m128i interpolation0 = SSE::interpolation4Channel32Bit8Elements(image0_row0Front, image0_row1Front, f0x_y_, f0xy_, f0x_y, f0xy);

	// image0 row0 [8:15]
	__m128i image0_row0Back = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + 8u));
	__m128i image0_row1Back = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + image0StrideElements + 8u));
	interpolation0 = _mm_or_si128(interpolation0, _mm_slli_si128(SSE::interpolation4Channel32Bit8Elements(image0_row0Back, image0_row1Back, f0x_y_, f0xy_, f0x_y, f0xy), 8));

	// image1 row0 [0:7]
	__m128i image1_row0Front = _mm_lddqu_si128((__m128i*)imageTopLeft1);
	__m128i image1_row1Front = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + image1StrideElements));
	__m128i interpolation1 = SSE::interpolation4Channel32Bit8Elements(image1_row0Front, image1_row1Front, f1x_y_, f1xy_, f1x_y, f1xy);

	// image1 row0 [8:15]
	__m128i image1_row0Back = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 8u));
	__m128i image1_row1Back = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + image1StrideElements + 8u));
	interpolation1 = _mm_or_si128(interpolation1, _mm_slli_si128(SSE::interpolation4Channel32Bit8Elements(image1_row0Back, image1_row1Back, f1x_y_, f1xy_, f1x_y, f1xy), 8));

	// ssd row0 [0:15]
	__m128i result = SSE::sumSquareDifference8Bit16Elements(interpolation0, interpolation1);

	SSE::prefetchT0(imageTopLeft0 + 3u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 3u * image1StrideElements);



	// row1
	// image0 row1 [0:7]
	__m128i image0_row2Front = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + 2u * image0StrideElements));
	interpolation0 = SSE::interpolation4Channel32Bit8Elements(image0_row1Front, image0_row2Front, f0x_y_, f0xy_, f0x_y, f0xy);

	// image0 row1 [8:15]
	__m128i image0_row2Back = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + 2u * image0StrideElements + 8u));
	interpolation0 = _mm_or_si128(interpolation0, _mm_slli_si128(SSE::interpolation4Channel32Bit8Elements(image0_row1Back, image0_row2Back, f0x_y_, f0xy_, f0x_y, f0xy), 8));

	// image1 row1 [0:7]
	__m128i image1_row2Front = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 2u * image1StrideElements));
	interpolation1 = SSE::interpolation4Channel32Bit8Elements(image1_row1Front, image1_row2Front, f1x_y_, f1xy_, f1x_y, f1xy);

	// image1 row1 [8:15]
	__m128i image1_row2Back = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 2u * image1StrideElements + 8u));
	interpolation1 = _mm_or_si128(interpolation1, _mm_slli_si128(SSE::interpolation4Channel32Bit8Elements(image1_row1Back, image1_row2Back, f1x_y_, f1xy_, f1x_y, f1xy), 8));

	// ssd row01 [0:15]
	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(interpolation0, interpolation1));


	// image0 row0 [16:19], row1 [16:19]
	image0_row0Back = _mm_blend_epi16(_mm_srli_si128(image0_row0Back, 8), image0_row1Back, 0xF0); // 0xF0 = 1111 0000
	image0_row1Back = _mm_blend_epi16(_mm_srli_si128(image0_row1Back, 8), image0_row2Back, 0xF0); // 0xF0 = 1111 0000
	interpolation0 = SSE::interpolation4Channel32Bit2x4Elements(image0_row0Back, image0_row1Back, f0x_y_, f0xy_, f0x_y, f0xy);

	// image1 row1 [16:19], row1 [16:19]
	image1_row0Back = _mm_blend_epi16(_mm_srli_si128(image1_row0Back, 8), image1_row1Back, 0xF0); // 0xF0 = 1111 0000
	image1_row1Back = _mm_blend_epi16(_mm_srli_si128(image1_row1Back, 8), image1_row2Back, 0xF0); // 0xF0 = 1111 0000
	interpolation1 = SSE::interpolation4Channel32Bit2x4Elements(image1_row0Back, image1_row1Back, f1x_y_, f1xy_, f1x_y, f1xy);

	// ssd row01 [0:19]
	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(interpolation0, interpolation1));


	SSE::prefetchT0(imageTopLeft0 + 4u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 4u * image1StrideElements);



	// row2
	// image0 row2 [0:7]
	__m128i image0_row3Front = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + 3u * image0StrideElements));
	interpolation0 = SSE::interpolation4Channel32Bit8Elements(image0_row2Front, image0_row3Front, f0x_y_, f0xy_, f0x_y, f0xy);

	// image0 row2 [8:15]
	__m128i image0_row3Back = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + 3u * image0StrideElements + 8u));
	interpolation0 = _mm_or_si128(interpolation0, _mm_slli_si128(SSE::interpolation4Channel32Bit8Elements(image0_row2Back, image0_row3Back, f0x_y_, f0xy_, f0x_y, f0xy), 8));

	// image1 row2 [0:7]
	__m128i image1_row3Front = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 3u * image1StrideElements));
	interpolation1 = SSE::interpolation4Channel32Bit8Elements(image1_row2Front, image1_row3Front, f1x_y_, f1xy_, f1x_y, f1xy);

	// image1 row2 [8:15]
	__m128i image1_row3Back = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 3u * image1StrideElements + 8u));
	interpolation1 = _mm_or_si128(interpolation1, _mm_slli_si128(SSE::interpolation4Channel32Bit8Elements(image1_row2Back, image1_row3Back, f1x_y_, f1xy_, f1x_y, f1xy), 8));

	// ssd row01 [0:19], row2 [0:15]
	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(interpolation0, interpolation1));

	SSE::prefetchT0(imageTopLeft0 + 5u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 5u * image1StrideElements);



	// row3
	// image0 row3 [0:7]
	__m128i image0_row4Front = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + 4u * image0StrideElements));
	interpolation0 = SSE::interpolation4Channel32Bit8Elements(image0_row3Front, image0_row4Front, f0x_y_, f0xy_, f0x_y, f0xy);

	// image0 row3 [8:15]
	__m128i image0_row4Back = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + 4u * image0StrideElements + 8u));
	interpolation0 = _mm_or_si128(interpolation0, _mm_slli_si128(SSE::interpolation4Channel32Bit8Elements(image0_row3Back, image0_row4Back, f0x_y_, f0xy_, f0x_y, f0xy), 8));

	// image1 row3 [0:7]
	__m128i image1_row4Front = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 4u * image1StrideElements));
	interpolation1 = SSE::interpolation4Channel32Bit8Elements(image1_row3Front, image1_row4Front, f1x_y_, f1xy_, f1x_y, f1xy);

	// image row3 [8:15]
	__m128i image1_row4Back = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 4u * image1StrideElements + 8u));
	interpolation1 = _mm_or_si128(interpolation1, _mm_slli_si128(SSE::interpolation4Channel32Bit8Elements(image1_row3Back, image1_row4Back, f1x_y_, f1xy_, f1x_y, f1xy), 8));

	// ssd row01 [0:19], row23 [0:15]
	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(interpolation0, interpolation1));

	// image0 row2 [16:19], row3 [16:19]
	image0_row2Back = _mm_blend_epi16(_mm_srli_si128(image0_row2Back, 8), image0_row3Back, 0xF0); // 0xF0 = 1111 0000
	image0_row3Back = _mm_blend_epi16(_mm_srli_si128(image0_row3Back, 8), image0_row4Back, 0xF0); // 0xF0 = 1111 0000
	interpolation0 = SSE::interpolation4Channel32Bit2x4Elements(image0_row2Back, image0_row3Back, f0x_y_, f0xy_, f0x_y, f0xy);

	// image1 row2 [16:19], row3 [16:19]
	image1_row2Back = _mm_blend_epi16(_mm_srli_si128(image1_row2Back, 8), image1_row3Back, 0xF0); // 0xF0 = 1111 0000
	image1_row3Back = _mm_blend_epi16(_mm_srli_si128(image1_row3Back, 8), image1_row4Back, 0xF0); // 0xF0 = 1111 0000
	interpolation1 = SSE::interpolation4Channel32Bit2x4Elements(image1_row2Back, image1_row3Back, f1x_y_, f1xy_, f1x_y, f1xy);

	// ssd row03 [0:19]
	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(interpolation0, interpolation1));



	// row4
	// image0 row4 [0:7]
	__m128i image0_row5Front = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + 5u * image0StrideElements));
	interpolation0 = SSE::interpolation4Channel32Bit8Elements(image0_row4Front, image0_row5Front, f0x_y_, f0xy_, f0x_y, f0xy);

	// image0 row4 [8:15]
	__m128i image0_row5Back = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + 5u * image0StrideElements + 8u));
	interpolation0 = _mm_or_si128(interpolation0, _mm_slli_si128(SSE::interpolation4Channel32Bit8Elements(image0_row4Back, image0_row5Back, f0x_y_, f0xy_, f0x_y, f0xy), 8));

	// image1 row4 [0:7]
	__m128i image1_row5Front = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 5u * image1StrideElements));
	interpolation1 = SSE::interpolation4Channel32Bit8Elements(image1_row4Front, image1_row5Front, f1x_y_, f1xy_, f1x_y, f1xy);

	// image1 row4 [8:15]
	__m128i image1_row5Back = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 5u * image1StrideElements + 8u));
	interpolation1 = _mm_or_si128(interpolation1, _mm_slli_si128(SSE::interpolation4Channel32Bit8Elements(image1_row4Back, image1_row5Back, f1x_y_, f1xy_, f1x_y, f1xy), 8));

	// ssd row03 [0:19] row4[0:15]
	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(interpolation0, interpolation1));

	// image0 row4 [16:19]
	image0_row4Back = _mm_and_si128(image0_row4Back, SSE::set128i(0xFFFFFFFFFFFFFFFFull, 0x0000000000000000ull));
	image0_row5Back = _mm_and_si128(image0_row5Back, SSE::set128i(0xFFFFFFFFFFFFFFFFull, 0x0000000000000000ull));
	interpolation0 = SSE::interpolation4Channel32Bit2x4Elements(image0_row4Back, image0_row5Back, f0x_y_, f0xy_, f0x_y, f0xy);

	// image1 row4 [16:19]
	image1_row4Back = _mm_and_si128(image1_row4Back, SSE::set128i(0xFFFFFFFFFFFFFFFFull, 0x0000000000000000ull));
	image1_row5Back = _mm_and_si128(image1_row5Back, SSE::set128i(0xFFFFFFFFFFFFFFFFull, 0x0000000000000000ull));
	interpolation1 = SSE::interpolation4Channel32Bit2x4Elements(image1_row4Back, image1_row5Back, f1x_y_, f1xy_, f1x_y, f1xy);

	// ssd row04 [0:19]
	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(interpolation0, interpolation1));

	return SSE::sum_u32_4(result);
}

template <>
inline uint32_t AdvancedSumSquareDifferencesSSE::patch8BitPerChannel<1u, 5u>(const uint8_t* const imageTopLeft0, const uint8_t* const imageTopLeft1, const unsigned int image0StrideElements, const unsigned int image1StrideElements, const unsigned int fx1, const unsigned int fy1)
{
	ocean_assert(fx1 <= 128u && fy1 <= 128u);

	SSE::prefetchT0(imageTopLeft0);

	SSE::prefetchT0(imageTopLeft1);
	SSE::prefetchT0(imageTopLeft1 + image1StrideElements);

	const unsigned int fx1_ = 128u - fx1;
	const unsigned int fy1_ = 128u - fy1;

	const unsigned int f1x_y_ = fx1_ * fy1_;
	const unsigned int f1xy_ = fx1 * fy1_;
	const unsigned int f1x_y = fx1_ * fy1;
	const unsigned int f1xy = fx1 * fy1;

	const __m128i __f1x_y_ = _mm_set1_epi16(short(f1x_y_));
	const __m128i __f1xy_ = _mm_set1_epi16(short(f1xy_));
	const __m128i __f1x_y = _mm_set1_epi16(short(f1x_y));
	const __m128i __f1xy = _mm_set1_epi16(short(f1xy));

	SSE::prefetchT0(imageTopLeft0 + 1u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 2u * image1StrideElements);

	// row0 -> [-----------00000]
	__m128i image0_row = _mm_slli_si128(_mm_loadu_si64(imageTopLeft0), 11);

	const __m128i image1_row0 = _mm_loadu_si64(imageTopLeft1);
	const __m128i image1_row1 = _mm_loadu_si64(imageTopLeft1 + image1StrideElements);
	__m128i image1_row = _mm_slli_si128(SSE::interpolation1Channel8Bit8Elements(image1_row0, image1_row1, __f1x_y_, __f1xy_, __f1x_y, __f1xy), 11);

	SSE::prefetchT0(imageTopLeft0 + 2u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 3u * image1StrideElements);


	// row1 -> [------1111100000]
	__m128i mask = SSE::set128i(0x0000000000FFFFFFull, 0xFFFFFFFFFFFFFFFFull);
	image0_row = _mm_blendv_epi8(image0_row, _mm_slli_si128(_mm_loadu_si64(imageTopLeft0 + 1u * image0StrideElements), 6), mask);

	const __m128i image1_row2 = _mm_loadu_si64(imageTopLeft1 + 2u * image1StrideElements);
	image1_row = _mm_blendv_epi8(image1_row, _mm_slli_si128(SSE::interpolation1Channel8Bit8Elements(image1_row1, image1_row2, __f1x_y_, __f1xy_, __f1x_y, __f1xy), 6), mask);

	SSE::prefetchT0(imageTopLeft0 + 3u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 4u * image1StrideElements);

#ifdef OCEAN_COMPILER_CLANG

	// workaround for Clang compiler bug - with optimizations the unused SSE bytes are not set to zero

	// row2 -> [-222221111100000]
	mask = SSE::set128i(0x0000000000000000ull, 0x0000FFFFFFFFFFFFull);
	image0_row = _mm_blendv_epi8(image0_row, _mm_slli_si128(_mm_loadu_si64(imageTopLeft0 + 2u * image0StrideElements), 1u), mask);

	const __m128i image1_row3 = _mm_loadu_si64(imageTopLeft1 + 3u * image1StrideElements);
	image1_row = _mm_blendv_epi8(image1_row, _mm_slli_si128(SSE::interpolation1Channel8Bit8Elements(image1_row2, image1_row3, __f1x_y_, __f1xy_, __f1x_y, __f1xy), 1), mask);

#else // OCEAN_COMPILER_CLANG

	// row2 -> [22222-1111100000]
	mask = SSE::set128i(0x0000000000000000ull, 0x000000FFFFFFFFFFull);
	image0_row = _mm_blendv_epi8(image0_row, _mm_loadu_si64(imageTopLeft0 + 2u * image0StrideElements), mask);

	const __m128i image1_row3 = _mm_loadu_si64(imageTopLeft1 + 3u * image1StrideElements);
	image1_row = _mm_blendv_epi8(image1_row, SSE::interpolation1Channel8Bit8Elements(image1_row2, image1_row3, __f1x_y_, __f1xy_, __f1x_y, __f1xy), mask);

#endif // OCEAN_COMPILER_CLANG

	// intermediate ssd
	__m128i result = SSE::sumSquareDifference8Bit16Elements(image0_row, image1_row);

	SSE::prefetchT0(imageTopLeft0 + 4u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 5u * image1StrideElements);


	// row3 -> [33333-----------]
	image0_row = _mm_loadu_si64(imageTopLeft0 + 3u * image0StrideElements);

	const __m128i image1_row4 = _mm_loadu_si64(imageTopLeft1 + 4u * image1StrideElements);
	image1_row = SSE::interpolation1Channel8Bit8Elements(image1_row3, image1_row4, __f1x_y_, __f1xy_, __f1x_y, __f1xy);

	// row4 -> [3333344444------]
	mask = SSE::set128i(0x000000000000FFFFull, 0xFFFFFF0000000000ull);
	image0_row = _mm_blendv_epi8(image0_row, _mm_slli_si128(_mm_loadu_si64(imageTopLeft0 + 4u * image0StrideElements - 3), 2), mask);

	const __m128i image1_row5 = _mm_loadu_si64(imageTopLeft1 + 5u * image1StrideElements - 2);
	image1_row = _mm_blendv_epi8(image1_row, _mm_slli_si128(SSE::interpolation1Channel8Bit8Elements(_mm_slli_si128(image1_row4, 2), image1_row5, __f1x_y_, __f1xy_, __f1x_y, __f1xy), 3), mask);

#ifdef OCEAN_COMPILER_CLANG

	// workaround for Clang compiler bug - with optimizations the unused SSE bytes are not set to zero

	image0_row = _mm_slli_si128(image0_row, 6);
	image1_row = _mm_slli_si128(image1_row, 6);

#endif // OCEAN_COMPILER_CLANG

	// remaining ssd
	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(image0_row, image1_row));

	return SSE::sum_u32_4(result);
}

template <>
inline uint32_t AdvancedSumSquareDifferencesSSE::patch8BitPerChannel<2u, 5u>(const uint8_t* const imageTopLeft0, const uint8_t* const imageTopLeft1, const unsigned int image0StrideElements, const unsigned int image1StrideElements, const unsigned int fx1, const unsigned int fy1)
{
	ocean_assert(fx1 <= 128u && fy1 <= 128u);

	SSE::prefetchT0(imageTopLeft0);
	SSE::prefetchT0(imageTopLeft0 + image0StrideElements);

	SSE::prefetchT0(imageTopLeft1);
	SSE::prefetchT0(imageTopLeft1 + image1StrideElements);

	const unsigned int fx1_ = 128u - fx1;
	const unsigned int fy1_ = 128u - fy1;

	const unsigned int f1x_y_ = fx1_ * fy1_;
	const unsigned int f1xy_ = fx1 * fy1_;
	const unsigned int f1x_y = fx1_ * fy1;
	const unsigned int f1xy = fx1 * fy1;

	const __m128i __f1x_y_ = _mm_set1_epi16(short(f1x_y_));
	const __m128i __f1xy_ = _mm_set1_epi16(short(f1xy_));
	const __m128i __f1x_y = _mm_set1_epi16(short(f1x_y));
	const __m128i __f1xy = _mm_set1_epi16(short(f1xy));

	SSE::prefetchT0(imageTopLeft0 + 2u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 2u * image1StrideElements);

	// row0
	// image0 row0[0:7]
	__m128i image0_row0 = _mm_loadl_epi64((__m128i*)imageTopLeft0);

	// image1 row0[0:7]
	__m128i image1_row0 = _mm_lddqu_si128((__m128i*)imageTopLeft1);
	__m128i image1_row1 = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + image1StrideElements));
	__m128i interpolation1 = SSE::interpolation2Channel16Bit8Elements(image1_row0, image1_row1, __f1x_y_, __f1xy_, __f1x_y, __f1xy);

	unsigned int localResult = SSE::ssd2Channel16Bit1x1(imageTopLeft0 + 8, imageTopLeft1 + 8, image0StrideElements, image1StrideElements, f1x_y_, f1xy_, f1x_y, f1xy)
								+ SSE::ssd2Channel16Bit1x1(imageTopLeft0 + 9, imageTopLeft1 + 9, image0StrideElements, image1StrideElements, f1x_y_, f1xy_, f1x_y, f1xy);

	SSE::prefetchT0(imageTopLeft0 + 3u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 3u * image1StrideElements);


	// row1
	// image0 row1[0:7]
	__m128i image0_row1 = _mm_loadl_epi64((__m128i*)(imageTopLeft0 + 1u * image0StrideElements));
	image0_row0 = ::_mm_or_si128(image0_row0, _mm_slli_si128(image0_row1, 8));

	// image1 row1[0:7]
	__m128i image1_row2 = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 2u * image1StrideElements));
	interpolation1 = _mm_or_si128(_mm_slli_si128(SSE::interpolation2Channel16Bit8Elements(image1_row1, image1_row2, __f1x_y_, __f1xy_, __f1x_y, __f1xy), 8), interpolation1);

	// ssd row01[0:7]
	__m128i result = SSE::sumSquareDifference8Bit16Elements(image0_row0, interpolation1);

	localResult += SSE::ssd2Channel16Bit1x1(imageTopLeft0 + 1u * image0StrideElements + 8u, imageTopLeft1 + 1u * image1StrideElements + 8u, image0StrideElements, image1StrideElements, f1x_y_, f1xy_, f1x_y, f1xy)
								+ SSE::ssd2Channel16Bit1x1(imageTopLeft0 + 1u * image0StrideElements + 9u, imageTopLeft1 + 1u * image1StrideElements + 9u, image0StrideElements, image1StrideElements, f1x_y_, f1xy_, f1x_y, f1xy);

	SSE::prefetchT0(imageTopLeft0 + 4u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 4u * image1StrideElements);


	// row 2
	// image0 row2[0:7]
	__m128i image0_row2 = _mm_loadl_epi64((__m128i*)(imageTopLeft0 + 2u * image0StrideElements));

	// image1 row2[0:7]
	__m128i image1_row3 = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 3u * image1StrideElements));
	interpolation1 = SSE::interpolation2Channel16Bit8Elements(image1_row2, image1_row3, __f1x_y_, __f1xy_, __f1x_y, __f1xy);

	localResult += SSE::ssd2Channel16Bit1x1(imageTopLeft0 + 2u * image0StrideElements + 8u, imageTopLeft1 + 2u * image1StrideElements + 8u, image0StrideElements, image1StrideElements, f1x_y_, f1xy_, f1x_y, f1xy)
								+ SSE::ssd2Channel16Bit1x1(imageTopLeft0 + 2u * image0StrideElements + 9u, imageTopLeft1 + 2u * image1StrideElements + 9u, image0StrideElements, image1StrideElements, f1x_y_, f1xy_, f1x_y, f1xy);

	SSE::prefetchT0(imageTopLeft0 + 5u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 5u * image1StrideElements);


	// row 3
	// image0 row3[0:7]
	__m128i image0_row3 = _mm_loadl_epi64((__m128i*)(imageTopLeft0 + 3u * image0StrideElements));
	image0_row2 = _mm_or_si128(image0_row2, _mm_slli_si128(image0_row3, 8));

	// image1 row3[0:7]
	__m128i image1_row4 = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 4u * image1StrideElements));
	interpolation1 = _mm_or_si128(_mm_slli_si128(SSE::interpolation2Channel16Bit8Elements(image1_row3, image1_row4, __f1x_y_, __f1xy_, __f1x_y, __f1xy), 8), interpolation1);

	// ssd row03[0:7]
	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(image0_row2, interpolation1));

	localResult += SSE::ssd2Channel16Bit1x1(imageTopLeft0 + 3u * image0StrideElements + 8u, imageTopLeft1 + 3u * image1StrideElements + 8u, image0StrideElements, image1StrideElements, f1x_y_, f1xy_, f1x_y, f1xy)
								+ SSE::ssd2Channel16Bit1x1(imageTopLeft0 + 3u * image0StrideElements + 9u, imageTopLeft1 + 3u * image1StrideElements + 9u, image0StrideElements, image1StrideElements, f1x_y_, f1xy_, f1x_y, f1xy);

	// row 4
	// image0 row4[0:7]
	__m128i image0_row4 = _mm_loadl_epi64((__m128i*)(imageTopLeft0 + 4u * image0StrideElements));

	// image0 row4[0:7]
	__m128i image1_row5 = _mm_srli_si128(_mm_lddqu_si128((__m128i*)(imageTopLeft1 + 5u * image1StrideElements - 2u)), 2); // here we start 2 bytes earlyer (and shift the bytes later) to avoid a segmentation fault
	interpolation1 = SSE::interpolation2Channel16Bit8Elements(image1_row4, image1_row5, __f1x_y_, __f1xy_, __f1x_y, __f1xy);

	// ssd row04[0:7]
	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(image0_row4, interpolation1));

	localResult += SSE::ssd2Channel16Bit1x1(imageTopLeft0 + 4u * image0StrideElements + 8u, imageTopLeft1 + 4u * image1StrideElements + 8u, image0StrideElements, image1StrideElements, f1x_y_, f1xy_, f1x_y, f1xy)
								+ SSE::ssd2Channel16Bit1x1(imageTopLeft0 + 4u * image0StrideElements + 9u, imageTopLeft1 + 4u * image1StrideElements + 9u, image0StrideElements, image1StrideElements, f1x_y_, f1xy_, f1x_y, f1xy);

	return SSE::sum_u32_4(result) + localResult;
}

template <>
inline uint32_t AdvancedSumSquareDifferencesSSE::patch8BitPerChannel<3u, 5u>(const uint8_t* const imageTopLeft0, const uint8_t* const imageTopLeft1, const unsigned int image0StrideElements, const unsigned int image1StrideElements, const unsigned int fx1, const unsigned int fy1)
{
	SSE::prefetchT0(imageTopLeft0);
	SSE::prefetchT0(imageTopLeft0 + image0StrideElements);

	SSE::prefetchT0(imageTopLeft1);
	SSE::prefetchT0(imageTopLeft1 + image1StrideElements);

	const unsigned int fx1_ = 128u - fx1;
	const unsigned int fy1_ = 128u - fy1;

	const __m128i f1x_y_ = _mm_set1_epi16(short(fx1_ * fy1_));
	const __m128i f1xy_ = _mm_set1_epi16(short(fx1 * fy1_));
	const __m128i f1x_y = _mm_set1_epi16(short(fx1_ * fy1));
	const __m128i f1xy = _mm_set1_epi16(short(fx1 * fy1));

	SSE::prefetchT0(imageTopLeft0 + 2u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 2u * image1StrideElements);

	// row 0
	__m128i image0_row0 = _mm_shuffle_epi8(_mm_lddqu_si128((__m128i*)imageTopLeft0), SSE::set128i(0x0E0D0C0B0A0908A0ull, 0x0706050403020100ull));

	__m128i image1_row0Front = _mm_lddqu_si128((__m128i*)imageTopLeft1);
	__m128i image1_row1Front = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + image1StrideElements));
	__m128i interpolation1 = SSE::interpolation3Channel24Bit8Elements(image1_row0Front, image1_row1Front, f1x_y_, f1xy_, f1x_y, f1xy);

	__m128i image1_row0Back = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 8u));
	__m128i image1_row1Back = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + image1StrideElements + 8u));
	interpolation1 = _mm_or_si128(interpolation1, _mm_slli_si128(SSE::interpolation3Channel24Bit8Elements(image1_row0Back, image1_row1Back, f1x_y_, f1xy_, f1x_y, f1xy), 9));

	__m128i result = SSE::sumSquareDifference8Bit16Elements(image0_row0, interpolation1);


	SSE::prefetchT0(imageTopLeft0 + 3u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 3u * image1StrideElements);


	// row 1
	__m128i image0_row1 = _mm_shuffle_epi8(_mm_lddqu_si128((__m128i*)(imageTopLeft0 + 1u * image0StrideElements)), SSE::set128i(0x0E0D0C0B0A0908A0ull, 0x0706050403020100ull));

	__m128i image1_row2Front = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 2u * image1StrideElements));
	interpolation1 = SSE::interpolation3Channel24Bit8Elements(image1_row1Front, image1_row2Front, f1x_y_, f1xy_, f1x_y, f1xy);

	__m128i image1_row2Back = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 2u * image1StrideElements + 8u));
	interpolation1 = _mm_or_si128(interpolation1, _mm_slli_si128(SSE::interpolation3Channel24Bit8Elements(image1_row1Back, image1_row2Back, f1x_y_, f1xy_, f1x_y, f1xy), 9));

	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(image0_row1, interpolation1));


	SSE::prefetchT0(imageTopLeft0 + 4u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 4u * image1StrideElements);


	// row 2
	__m128i image0_row2 = _mm_shuffle_epi8(_mm_lddqu_si128((__m128i*)(imageTopLeft0 + 2u * image0StrideElements)), SSE::set128i(0x0E0D0C0B0A0908A0ull, 0x0706050403020100ull));

	__m128i image1_row3Front = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 3u * image1StrideElements));
	interpolation1 = SSE::interpolation3Channel24Bit8Elements(image1_row2Front, image1_row3Front, f1x_y_, f1xy_, f1x_y, f1xy);

	__m128i image1_row3Back = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 3u * image1StrideElements + 8u));
	interpolation1 = _mm_or_si128(interpolation1, _mm_slli_si128(SSE::interpolation3Channel24Bit8Elements(image1_row2Back, image1_row3Back, f1x_y_, f1xy_, f1x_y, f1xy), 9));

	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(image0_row2, interpolation1));


	SSE::prefetchT0(imageTopLeft0 + 5u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 5u * image1StrideElements);

	// row 3
	__m128i image0_row3 = _mm_shuffle_epi8(_mm_lddqu_si128((__m128i*)(imageTopLeft0 + 3u * image0StrideElements)), SSE::set128i(0x0E0D0C0B0A0908A0ull, 0x0706050403020100ull));

	__m128i image1_row4Front = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 4u * image1StrideElements));
	interpolation1 = SSE::interpolation3Channel24Bit8Elements(image1_row3Front, image1_row4Front, f1x_y_, f1xy_, f1x_y, f1xy);

	__m128i image1_row4Back = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 4u * image1StrideElements + 8u));
	interpolation1 = _mm_or_si128(interpolation1, _mm_slli_si128(SSE::interpolation3Channel24Bit8Elements(image1_row3Back, image1_row4Back, f1x_y_, f1xy_, f1x_y, f1xy), 9));

	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(image0_row3, interpolation1));



	// row 4
	__m128i image0_row4 = _mm_shuffle_epi8(_mm_lddqu_si128((__m128i*)(imageTopLeft0 + 4u * image0StrideElements)), SSE::set128i(0x0E0D0C0B0A0908A0ull, 0x0706050403020100ull));

	__m128i image1_row5Front = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 5u * image1StrideElements));
	interpolation1 = SSE::interpolation3Channel24Bit8Elements(image1_row4Front, image1_row5Front, f1x_y_, f1xy_, f1x_y, f1xy);

	__m128i image1_row5Back = _mm_srli_si128(_mm_lddqu_si128((__m128i*)(imageTopLeft1 + 5u * image1StrideElements + 8u - 6u)), 6); // here we start 6 bytes earlyer (and shift the bytes later) to avoid a segmentation fault
	interpolation1 = _mm_or_si128(interpolation1, _mm_slli_si128(SSE::interpolation3Channel24Bit8Elements(image1_row4Back, image1_row5Back, f1x_y_, f1xy_, f1x_y, f1xy), 9));

	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(image0_row4, interpolation1));

	return SSE::sum_u32_4(result);
}

template <>
inline uint32_t AdvancedSumSquareDifferencesSSE::patch8BitPerChannel<4u, 5u>(const uint8_t* const imageTopLeft0, const uint8_t* const imageTopLeft1, const unsigned int image0StrideElements, const unsigned int image1StrideElements, const unsigned int fx1, const unsigned int fy1)
{
	SSE::prefetchT0(imageTopLeft0);
	SSE::prefetchT0(imageTopLeft0 + image0StrideElements);

	SSE::prefetchT0(imageTopLeft1);
	SSE::prefetchT0(imageTopLeft1 + image1StrideElements);

	const unsigned int fx1_ = 128u - fx1;
	const unsigned int fy1_ = 128u - fy1;

	const __m128i f1x_y_ = _mm_set1_epi16(short(fx1_ * fy1_));
	const __m128i f1xy_ = _mm_set1_epi16(short(fx1 * fy1_));
	const __m128i f1x_y = _mm_set1_epi16(short(fx1_ * fy1));
	const __m128i f1xy = _mm_set1_epi16(short(fx1 * fy1));

	SSE::prefetchT0(imageTopLeft0 + 1u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 2u * image1StrideElements);

	// row0
	// image0 row0 [0:15]
	__m128i image0_row0Front = _mm_lddqu_si128((__m128i*)imageTopLeft0);

	// image1 row0 [0:7]
	__m128i image1_row0Front = _mm_lddqu_si128((__m128i*)imageTopLeft1);
	__m128i image1_row1Front = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + image1StrideElements));
	__m128i interpolation1 = SSE::interpolation4Channel32Bit8Elements(image1_row0Front, image1_row1Front, f1x_y_, f1xy_, f1x_y, f1xy);

	// image1 row0 [8:15]
	__m128i image1_row0Back = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 8u));
	__m128i image1_row1Back = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + image1StrideElements + 8u));
	interpolation1 = _mm_or_si128(interpolation1, _mm_slli_si128(SSE::interpolation4Channel32Bit8Elements(image1_row0Back, image1_row1Back, f1x_y_, f1xy_, f1x_y, f1xy), 8));

	// ssd row0 [0:15]
	__m128i result = SSE::sumSquareDifference8Bit16Elements(image0_row0Front, interpolation1);

	SSE::prefetchT0(imageTopLeft0 + 2u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 3u * image1StrideElements);



	// row1
	// image0 row1 [0:15]
	__m128i image0_row1Front = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + 1u * image0StrideElements));

	// image1 row1 [0:7]
	__m128i image1_row2Front = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 2u * image1StrideElements));
	interpolation1 = SSE::interpolation4Channel32Bit8Elements(image1_row1Front, image1_row2Front, f1x_y_, f1xy_, f1x_y, f1xy);

	// image1 row1 [8:15]
	__m128i image1_row2Back = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 2u * image1StrideElements + 8u));
	interpolation1 = _mm_or_si128(interpolation1, _mm_slli_si128(SSE::interpolation4Channel32Bit8Elements(image1_row1Back, image1_row2Back, f1x_y_, f1xy_, f1x_y, f1xy), 8));

	// ssd row01 [0:15]
	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(image0_row1Front, interpolation1));

	// image0 row0 [16:19], row1 [16:19]
	__m128i image0row01 = _mm_set_epi32(0, 0, *((unsigned int*)(imageTopLeft0 + image0StrideElements + 16u)), *((unsigned int*)(imageTopLeft0 + 16)));

	// image1 row1 [16:19], row1 [16:19]
	image1_row0Back = _mm_blend_epi16(_mm_srli_si128(image1_row0Back, 8), image1_row1Back, 0xF0); // 0xF0 = 1111 0000
	image1_row1Back = _mm_blend_epi16(_mm_srli_si128(image1_row1Back, 8), image1_row2Back, 0xF0); // 0xF0 = 1111 0000
	interpolation1 = SSE::interpolation4Channel32Bit2x4Elements(image1_row0Back, image1_row1Back, f1x_y_, f1xy_, f1x_y, f1xy);

	// ssd row01 [0:19]
	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(image0row01, interpolation1));


	SSE::prefetchT0(imageTopLeft0 + 3u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 4u * image1StrideElements);



	// row2
	// image0 row2 [0:7]
	__m128i image0_row2Front = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + 2u * image0StrideElements));

	// image1 row2 [0:7]
	__m128i image1_row3Front = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 3u * image1StrideElements));
	interpolation1 = SSE::interpolation4Channel32Bit8Elements(image1_row2Front, image1_row3Front, f1x_y_, f1xy_, f1x_y, f1xy);

	// image1 row2 [8:15]
	__m128i image1_row3Back = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 3u * image1StrideElements + 8u));
	interpolation1 = _mm_or_si128(interpolation1, _mm_slli_si128(SSE::interpolation4Channel32Bit8Elements(image1_row2Back, image1_row3Back, f1x_y_, f1xy_, f1x_y, f1xy), 8));

	// ssd row01 [0:19], row2 [0:15]
	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(image0_row2Front, interpolation1));

	SSE::prefetchT0(imageTopLeft0 + 4u * image0StrideElements);
	SSE::prefetchT0(imageTopLeft1 + 5u * image1StrideElements);



	// row3
	// image0 row3 [0:7]
	__m128i image0_row3Front = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + 3u * image0StrideElements));

	// image1 row3 [0:7]
	__m128i image1_row4Front = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 4u * image1StrideElements));
	interpolation1 = SSE::interpolation4Channel32Bit8Elements(image1_row3Front, image1_row4Front, f1x_y_, f1xy_, f1x_y, f1xy);

	// image row3 [8:15]
	__m128i image1_row4Back = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 4u * image1StrideElements + 8u));
	interpolation1 = _mm_or_si128(interpolation1, _mm_slli_si128(SSE::interpolation4Channel32Bit8Elements(image1_row3Back, image1_row4Back, f1x_y_, f1xy_, f1x_y, f1xy), 8));

	// ssd row01 [0:19], row23 [0:15]
	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(image0_row3Front, interpolation1));

	// image0 row2 [16:19], row3 [16:19]
	__m128i image0row23 = _mm_set_epi32(0, 0, *((unsigned int*)(imageTopLeft0 + 3u * image0StrideElements + 16u)), *((unsigned int*)(imageTopLeft0 + 2u * image0StrideElements + 16)));

	// image1 row2 [16:19], row3 [16:19]
	image1_row2Back = _mm_blend_epi16(_mm_srli_si128(image1_row2Back, 8), image1_row3Back, 0xF0); // 0xF0 = 1111 0000
	image1_row3Back = _mm_blend_epi16(_mm_srli_si128(image1_row3Back, 8), image1_row4Back, 0xF0); // 0xF0 = 1111 0000
	interpolation1 = SSE::interpolation4Channel32Bit2x4Elements(image1_row2Back, image1_row3Back, f1x_y_, f1xy_, f1x_y, f1xy);

	// ssd row03 [0:19]
	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(image0row23, interpolation1));



	// row4
	// image0 row4 [0:7]
	__m128i image0_row4Front = _mm_lddqu_si128((__m128i*)(imageTopLeft0 + 4u * image0StrideElements));

	// image1 row4 [0:7]
	__m128i image1_row5Front = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 5u * image1StrideElements));
	interpolation1 = SSE::interpolation4Channel32Bit8Elements(image1_row4Front, image1_row5Front, f1x_y_, f1xy_, f1x_y, f1xy);

	// image1 row4 [8:15]
	__m128i image1_row5Back = _mm_lddqu_si128((__m128i*)(imageTopLeft1 + 5u * image1StrideElements + 8u));
	interpolation1 = _mm_or_si128(interpolation1, _mm_slli_si128(SSE::interpolation4Channel32Bit8Elements(image1_row4Back, image1_row5Back, f1x_y_, f1xy_, f1x_y, f1xy), 8));

	// ssd row03 [0:19] row4[0:15]
	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(image0_row4Front, interpolation1));

	// image0 row4 [16:19]
	__m128i image0row4 = _mm_set_epi32(0, 0, *((unsigned int*)(imageTopLeft0 + 4u * image0StrideElements + 16u)), 0);

	// image1 row4 [16:19]
	image1_row4Back = _mm_and_si128(image1_row4Back, SSE::set128i(0xFFFFFFFFFFFFFFFFull, 0x0000000000000000ull));
	image1_row5Back = _mm_and_si128(image1_row5Back, SSE::set128i(0xFFFFFFFFFFFFFFFFull, 0x0000000000000000ull));
	interpolation1 = SSE::interpolation4Channel32Bit2x4Elements(image1_row4Back, image1_row5Back, f1x_y_, f1xy_, f1x_y, f1xy);

	// ssd row04 [0:19]
	result = _mm_add_epi32(result, SSE::sumSquareDifference8Bit16Elements(image0row4, interpolation1));

	return SSE::sum_u32_4(result);
}

}

}

}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 41

#endif // META_OCEAN_CV_ADVANCED_ADVANCED_SUM_SQUARE_DIFFERENCES_SSE_H
