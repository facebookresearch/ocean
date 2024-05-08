/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SSE_H
#define META_OCEAN_CV_SSE_H

#include "ocean/cv/CV.h"

#include "ocean/base/Utilities.h"

#include "ocean/math/Math.h"

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

// SSE2 include files
#include <emmintrin.h>
#include <immintrin.h>
#include <mmintrin.h>

// SSE3 include files
#include <pmmintrin.h>
#include <mmintrin.h>

// SSE4 include files
#include <smmintrin.h>

namespace Ocean
{

namespace CV
{

/**
 * This class implements computer vision functions using SSE extensions.
 * @ingroup cv
 */
class SSE
{
	public:

#if !defined(OCEAN_COMPILER_MSC)

		/**
		 * This union defines a wrapper for the __m128i SSE intrinsic data type.
		 */
		union M128i
		{
			/// The two 64 bit elements.
			uint64_t m128i_u64[2];

			/// The four 32 bit elements.
			uint32_t m128i_u32[4];

			/// The eight 16 bit elements.
			uint16_t m128i_u16[8];

			/// The sixteen 8 bit elements.
			uint8_t m128i_u8[16];
		};

		static_assert(sizeof(M128i) == 16, "Invalid data type!");

		/**
		 * This union defines a wrapper for the __m128 SSE intrinsic data type.
		 */
		union M128
		{
			/// The four 32 bit elements.
			float m128_f32[4];
		};

		static_assert(sizeof(M128) == 16, "Invalid data type!");

		/**
		 * This union defines a wrapper for the __m128 SSE intrinsic data type.
		 */
		union M128d
		{
			/// The two 64 bit elements.
			double m128d_f64[2];
		};

		static_assert(sizeof(M128d) == 16, "Invalid data type!");

#endif

	public:

		/**
		 * Prefetches a block of temporal memory into all cache levels.
		 * @param data Data to be prefetched
		 */
		static inline void prefetchT0(const void* const data);

		/**
		 * Prefetches a block of temporal memory in all cache levels except 0th cache level.
		 * @param data Data to be prefetched
		 */
		static inline void prefetchT1(const void* const data);

		/**
		 * Prefetches a block of temporal memory in all cache levels, except 0th and 1st cache levels.
		 * @param data Data to be prefetched
		 */
		static inline void prefetchT2(const void* const data);

		/**
		 * Prefetches a block of non-temporal memory into non-temporal cache structure.
		 * @param data Data to be prefetched
		 */
		static inline void prefetchNTA(const void* const data);

		/**
		 * Returns one specific 8 bit unsigned integer value of a m128i value object.
		 * @param value The value from which the 8 bit value will be returned
		 * @return The requested 8 bit value
		 * @tparam tIndex The index of the requested 8 bit integer value, with range [0, 15]
		 */
		template <unsigned int tIndex>
		static inline uint8_t value_u8(const __m128i& value);

		/**
		 * Returns one specific 8 bit unsigned integer value of a m128i value object.
		 * @param value The value from which the 8 bit value will be returned
		 * @param index The index of the requested 8 bit integer value, with range [0, 15]
		 * @return The requested 8 bit value
		 */
		static inline uint8_t value_u8(const __m128i& value, const unsigned int index);

		/**
		 * Returns one specific 16 bit unsigned integer value of a m128i value object.
		 * @param value The value from which the 16 bit value will be returned
		 * @return The requested 16 bit value
		 * @tparam tIndex The index of the requested 16 bit integer value, with range [0, 7]
		 */
		template <unsigned int tIndex>
		static inline uint16_t value_u16(const __m128i& value);

		/**
		 * Returns one specific 32 bit unsigned integer value of a m128i value object.
		 * @param value The value from which the 32 bit value will be returned
		 * @return The requested 32 bit value
		 * @tparam tIndex The index of the requested 32 bit integer value, with range [0, 3]
		 */
		template <unsigned int tIndex>
		static inline unsigned int value_u32(const __m128i& value);

		/**
		 * Adds the four (all four) individual 32 bit unsigned integer values of a m128i value and returns the result.
		 * @param value The value which elements will be added
		 * @return The resulting sum value
		 */
		static OCEAN_FORCE_INLINE unsigned int sum_u32_4(const __m128i& value);

		/**
		 * Adds the first two individual 32 bit unsigned integer values of a m128i value and returns the result.
		 * @param value The value which elements will be added
		 * @return The resulting sum value
		 */
		static inline unsigned int sum_u32_first_2(const __m128i& value);

		/**
		 * Adds the first and the second 32 bit unsigned integer values of a m128i value and returns the result.
		 * @param value The value which elements will be added
		 * @return The resulting sum value
		 */
		static inline unsigned int sum_u32_first_third(const __m128i& value);

		/**
		 * Adds the four (all four) individual 32 bit float of a m128 value and returns the result.
		 * @param value The value which elements will be added
		 * @return The resulting sum value
		 */
		static OCEAN_FORCE_INLINE float sum_f32_4(const __m128& value);

		/**
		 * Adds the two (all two) individual 64 bit float of a m128 value and returns the result.
		 * @param value The value which elements will be added
		 * @return The resulting sum value
		 */
		static OCEAN_FORCE_INLINE double sum_f64_2(const __m128d& value);

		/**
		 * Sum square differences determination for the last 11 elements of an 16 elements buffer with 8 bit precision.
		 * @param image0 First 11 elements to determine the ssd for, may be non aligned
		 * @param image1 Second 11 elements to determine the ssd for, may be non aligned
		 * @return SSD result distributed over four terms of the sum, thus result is (m128i_u32[0] + m128i_u32[1] + m128i_u32[2] + m128i_u32[3])
		 */
		static inline __m128i sumSquareDifferences8BitBack11Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum square difference determination for the first 12 elements of an 16 elements buffer with 8 bit precision, the remaining 4 elements are set to zero.
		 * However, the provides buffers must be at least 16 bytes large as the entire 16 bytes will be loaded to the SSE registers.<br>
		 * Thus, this function handles two buffers with this pattern (while the memory starts left and ends rights: [00 01 02 03 04 05 06 07 08 09 10 11 NA NA NA NA].
		 * @param image0 First 12 (+4) elements to determine the ssd for, with any alignment
		 * @param image1 Second 12 (+4) elements to determine the ssd for, with any alignment
		 * @return SSD result distributed over four terms of the sum, thus result is (m128i_u32[0] + m128i_u32[1] + m128i_u32[2] + m128i_u32[3])
		 */
		static inline __m128i sumSquareDifference8BitFront12Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum square difference determination for the last 12 elements of an 16 elements buffer with 8 bit precision, the beginning 4 elements are interpreted as zero.
		 * However, the provides buffers must be at least 16 bytes large as the entire 16 bytes will be loaded to the SSE registers.<br>
		 * Thus, this function handles two buffers with this pattern (while the memory starts left and ends right): [NA NA NA NA 04 05 06 07 08 09 10 11 12 13 14 15].
		 * @param image0 First (4+) 12 elements to determine the ssd for, with any alignment
		 * @param image1 Second (4+) 12 elements to determine the ssd for, with any alignment
		 * @return SSD result distributed over four terms of the sum, thus result is (m128i_u32[0] + m128i_u32[1] + m128i_u32[2] + m128i_u32[3])
		 */
		static inline __m128i sumSquareDifference8BitBack12Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum square difference determination for the first 13 elements of a buffer with 8 bit precision.
		 * This function supports to load the 13 elements from a buffer with only 13 bytes or with a buffer with at least 16 bytes.
		 * @param image0 First 13 elements to determine the ssd for, may be non aligned
		 * @param image1 Second 13 elements to determine the ssd for, may be non aligned
		 * @return SSD result distributed over four terms of the sum, thus result is (m128i_u32[0] + m128i_u32[1] + m128i_u32[2] + m128i_u32[3])
		 * @tparam tBufferHas16Bytes True, if the buffer holds at least 16 bytes; False, if the buffer holds 13 bytes only
		 */
		template <bool tBufferHas16Bytes>
		static inline __m128i sumSquareDifference8BitFront13Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum square difference determination for the last 13 elements of an 16 elements buffer with 8 bit precision, the beginning 3 elements are interpreted as zero.
		 * However, the provides buffers must be at least 16 bytes large as the entire 16 bytes will be loaded to the SSE registers.<br>
		 * Thus, this function handles two buffers with this pattern (while the memory starts left and ends rights: [NA NA NA 03 04 05 06 07 08 09 10 11 12 13 14 15].
		 * @param image0 First (3+) 13 elements to determine the ssd for, may be non aligned
		 * @param image1 Second (3+) 13 elements to determine the ssd for, may be non aligned
		 * @return SSD result distributed over four terms of the sum, thus result is (m128i_u32[0] + m128i_u32[1] + m128i_u32[2] + m128i_u32[3])
		 */
		static inline __m128i sumSquareDifference8BitBack13Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum square difference determination for the first 15 elements of a buffer with 8 bit precision.
		 * This function supports to load the 15 elements from a buffer with only 15 bytes or with a buffer with at least 16 bytes.
		 * @param image0 First 15 elements to determine the ssd for, may be non aligned
		 * @param image1 Second 15 elements to determine the ssd for, may be non aligned
		 * @return SSD result distributed over four terms of the sum, thus result is (m128i_u32[0] + m128i_u32[1] + m128i_u32[2] + m128i_u32[3])
		 * @tparam tBufferHas16Bytes True, if the buffer holds at least 16 bytes; False, if the buffer holds 15 bytes only
		 */
		template <bool tBufferHas16Bytes>
		static inline __m128i sumSquareDifference8BitFront15Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum square difference determination for 16 elements with 8 bit precision.
		 * @param image0 First 16 elements to determine the ssd for, may be non aligned
		 * @param image1 Second 16 elements to determine the ssd for, may be non aligned
		 * @return SSD result distributed over four terms of the sum, thus result is (m128i_u32[0] + m128i_u32[1] + m128i_u32[2] + m128i_u32[3])
		 */
		static inline __m128i sumSquareDifference8Bit16Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum square difference determination for 16 elements with 8 bit precision.
		 * @param image0 First 16 elements to determine the ssd for, may be non aligned
		 * @param image1 Second 16 elements to determine the ssd for, may be non aligned
		 * @return SSD result distributed over four terms of the sum, thus result is (m128i_u32[0] + m128i_u32[1] + m128i_u32[2] + m128i_u32[3])
		 */
		static inline __m128i sumSquareDifference8Bit16ElementsAligned16(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum square difference determination for 16 elements with 8 bit precision.
		 * @param row0 First 16 elements to determine the ssd for
		 * @param row1 Second 16 elements to determine the ssd for
		 * @return SSD result distributed over four terms of the sum, thus result is (m128i_u32[0] + m128i_u32[1] + m128i_u32[2] + m128i_u32[3])
		 */
		static inline __m128i sumSquareDifference8Bit16Elements(const __m128i& row0, const __m128i& row1);

		/**
		 * Averages 8 elements of 2x2 blocks for 1 channel 32 bit frames.
		 * The function takes two rows of 8 elements and returns 4 average elements (4 averaged pixels).<br>
		 * @param image0 First row of 8 elements
		 * @param image1 Second row of 8 elements
		 * @param result Resulting 4 average elements
		 */
		static inline void average8Elements1Channel32Bit2x2(const float* const image0, const float* const image1, float* const result);

		/**
		 * Averages 8 elements of 2x2 blocks for 1 channel 8 bit frames.
		 * The function takes two rows of 8 elements and returns 4 average elements (4 averaged pixels).<br>
		 * @param image0 First row of 8 elements
		 * @param image1 Second row of 8 elements
		 * @param result Resulting 4 average elements
		 */
		static inline void average8Elements1Channel8Bit2x2(const uint8_t* const image0, const uint8_t* const image1, uint8_t* const result);

		/**
		 * Averages 8 elements of 2x2 blocks for 1 binary (x00 or 0xFF) frames.
		 * The function takes two rows of 8 elements and returns 4 average elements (4 averaged pixels).<br>
		 * @param image0 First row of 8 elements, must be valid
		 * @param image1 Second row of 8 elements, must be valid
		 * @param result Resulting 4 average elementss, must be valid
		 * @param threshold The minimal sum value of four pixels to result in a mask with value 255, with range [1, 255 * 4]
		 */
		static inline void average8ElementsBinary1Channel8Bit2x2(const uint8_t* const image0, const uint8_t* const image1, uint8_t* const result, const uint16_t threshold = 776u);

		/**
		 * Averages 16 elements of 2x2 blocks for 1 channel 8 bit frames.
		 * The function takes two rows of 16 elements and returns 8 average elements (8 averaged pixels).<br>
		 * @param image0 First row of 16 elements, must be valid
		 * @param image1 Second row of 16 elements, must be valid
		 * @param result Resulting 8 average elements, must be valid
		 */
		static inline void average16Elements1Channel8Bit2x2(const uint8_t* const image0, const uint8_t* const image1, uint8_t* const result);

		/**
		 * Averages 16 elements of 2x2 blocks for 1 binary (x00 or 0xFF) frames.
		 * The function takes two rows of 16 elements and returns 8 average elements (8 averaged pixels).<br>
		 * @param image0 First row of 16 elements, must be valid
		 * @param image1 Second row of 16 elements, must be valid
		 * @param result Resulting 8 average elements, must be valid
		 * @param threshold The minimal sum value of four pixels to result in a mask with value 255, with range [1, 255 * 4]
		 */
		static inline void average16ElementsBinary1Channel8Bit2x2(const uint8_t* const image0, const uint8_t* const image1, uint8_t* const result, const uint16_t threshold = 776u);

		/**
		 * Averages 32 elements of 2x2 blocks for 1 channel 8 bit frames.
		 * The function takes two rows of 32 elements and returns 16 average elements (16 averaged pixels).<br>
		 * @param image0 First row of 32 elements
		 * @param image1 Second row of 32 elements
		 * @param result Resulting 16 average elements
		 */
		static inline void average32Elements1Channel8Bit2x2(const uint8_t* const image0, const uint8_t* const image1, uint8_t* const result);

		/**
		 * Averages 32 elements of 2x2 blocks for 1 binary (0x00 or 0xFF) frames.
		 * The function takes two rows of 32 elements and returns 16 average elements (16 averaged pixels).<br>
		 * @param image0 First row of 32 elements, must be valid
		 * @param image1 Second row of 32 elements, must be valid
		 * @param result Resulting 16 average elements, must be valid
		 * @param threshold The minimal sum value of four pixels to result in a mask with value 255, with range [1, 255 * 4]
		 */
		static inline void average32ElementsBinary1Channel8Bit2x2(const uint8_t* const image0, const uint8_t* const image1, uint8_t* const result, const uint16_t threshold = 776u);

		/**
		 * Averages 8 elements of 2x2 blocks for 2 channel 16 bit frames.
		 * The function takes two rows of 8 elements and returns 4 average elements (2 averaged pixels, each with 2 channels).<br>
		 * @param image0 First row of 8 elements
		 * @param image1 Second row of 8 elements
		 * @param result Resulting 4 average elements
		 */
		static inline void average8Elements2Channel16Bit2x2(const uint8_t* const image0, const uint8_t* const image1, uint8_t* const result);

		/**
		 * Averages 8 elements of 2x2 blocks for 2 channel 64 bit frames.
		 * The function takes two rows of 8 elements and returns 4 average elements (2 averaged pixels).<br>
		 * @param image0 First row of 8 elements
		 * @param image1 Second row of 8 elements
		 * @param result Resulting 4 average elements
		 */
		static inline void average8Elements2Channel64Bit2x2(const float* const image0, const float* const image1, float* const result);

		/**
		 * Averages 16 elements of 2x2 blocks for 2 channel 16 bit frames.
		 * The function takes two rows of 32 elements and returns 8 average elements (4 averaged pixels, each with 2 channels).<br>
		 * @param image0 First row of 16 elements
		 * @param image1 Second row of 16 elements
		 * @param result Resulting 8 average elements
		 */
		static inline void average16Elements2Channel16Bit2x2(const uint8_t* const image0, const uint8_t* const image1, uint8_t* const result);

		/**
		 * Averages 32 elements of 2x2 blocks for 2 channel 16 bit frames.
		 * The function takes two rows of 32 elements and returns 16 average elements (8 averaged pixels, each with 2 channels).<br>
		 * @param image0 First row of 32 elements
		 * @param image1 Second row of 32 elements
		 * @param result Resulting 16 average elements
		 */
		static inline void average32Elements2Channel16Bit2x2(const uint8_t* const image0, const uint8_t* const image1, uint8_t* const result);

		/**
		 * Averages 6 elements of 2x2 blocks for 3 channel 96 bit frames.
		 * The function takes two rows of 6 elements and returns 3 average elements (1 averaged pixels, each with 3 channels).<br>
		 * @param image0 First row of 6 elements
		 * @param image1 Second row of 6 elements
		 * @param result Resulting 3 average elements
		 */
		static inline void average6Elements3Channel96Bit2x2(const float* const image0, const float* const image1, float* const result);

		/**
		 * Averages 24 elements of 2x2 blocks for 3 channel 24 bit frames.
		 * The function takes two rows of 24 elements and returns 12 average elements (4 averaged pixels, each with 3 channels).<br>
		 * @param image0 First row of 24 elements
		 * @param image1 Second row of 24 elements
		 * @param result Resulting 12 average elements
		 */
		static inline void average24Elements3Channel24Bit2x2(const uint8_t* const image0, const uint8_t* const image1, uint8_t* const result);

		/**
		 * Averages 8 elements of 2x2 blocks for 4 channel 128 bit frames.
		 * The function takes two rows of 8 elements and returns 4 average elements (1 averaged pixel).<br>
		 * @param image0 First row of 8 elements
		 * @param image1 Second row of 8 elements
		 * @param result Resulting 4 average elements
		 */
		static inline void average8Elements4Channel128Bit2x2(const float* const image0, const float* const image1, float* const result);

		/**
		 * Averages 16 elements of 2x2 blocks for 4 channel 32 bit frames.
		 * The function takes two rows of 16 elements and returns 8 average elements (2 averaged pixels, each with 4 channels).<br>
		 * @param image0 First row of 16 elements
		 * @param image1 Second row of 16 elements
		 * @param result Resulting 8 average elements
		 */
		static inline void average16Elements4Channel32Bit2x2(const uint8_t* const image0, const uint8_t* const image1, uint8_t* const result);

		/**
		 * Averages 32 elements of 2x2 blocks for 4 channel 32 bit frames.
		 * The function takes two rows of 32 elements and returns 16 average elements (4 averaged pixels, each with 4 channels).<br>
		 * @param image0 First row of 32 elements
		 * @param image1 Second row of 32 elements
		 * @param result Resulting 16 average elements
		 */
		static inline void average32Elements4Channel32Bit2x2(const uint8_t* const image0, const uint8_t* const image1, uint8_t* const result);

		/**
		 * Averages 30 elements of 3x3 blocks for 1 channel 8 bit frames.
		 * The function takes two rows of 30 elements and returns 10 average elements (10 averaged pixels).<br>
		 * @param image0 First row of 30 elements
		 * @param image1 Second row of 30 elements
		 * @param image2 Third row of 30 elements
		 * @param result Resulting 10 average elements
		 */
		static inline void average30Elements1Channel8Bit3x3(const uint8_t* const image0, const uint8_t* const image1, const uint8_t* const image2, uint8_t* const result);

		/**
		 * Adds 1 to each signed 16 bit value which is both, negative and odd, so that each value can be right shifted by one bit to allow a correct division by two.
		 * This function must be invoked before the right shift is applied.
		 * @param value The eight signed 16 bit values to be handled
		 * @return The modified value for which divide (/ 2) and bit shift (>> 1) yield equal (and correct!) results
		 */
		static inline __m128i addOffsetBeforeRightShiftDivisionByTwoSigned16Bit(const __m128i& value);

		/**
		 * Adds 2^shifts - 1 to each negative signed 16 bit value, so they each value can be right shifted to allow a correct division by 2^shifts.
		 * This function must be invoked before the right shift is applied.
		 * @param value The eight signed 16 bit values to be handled
		 * @param rightShifts The number of right shifts which needs to be applied, with range [0, 15]
		 * @return The modified value for which division a shift yield equal (and correct!) results
		 */
		static inline __m128i addOffsetBeforeRightShiftDivisionSigned16Bit(const __m128i& value, const unsigned int rightShifts);

		/**
		 * Divides eight signed 16 bit values by applying a right shift.
		 * This is able to determine the correct division result for positive and negative 16 bit values.
		 * @param value The eight signed 16 bit values to be handled
		 * @param rightShifts The number of right shifts which needs to be applied, with range [0, 15]
		 * @return The divided values
		 */
		static inline __m128i divideByRightShiftSigned16Bit(const __m128i& value, const unsigned int rightShifts);

		/**
		 * Adds 1 to each signed 32 bit value which is both, negative and odd, so that each value can be right shifted by one bit to allow a correct division by two.
		 * This function must be invoked before the right shift is applied.
		 * @param value The eight signed 32 bit values to be handled
		 * @return The modified value for which divide (/ 2) and bit shift (>> 1) yield equal (and correct!) results
		 */
		static inline __m128i addOffsetBeforeRightShiftDivisionByTwoSigned32Bit(const __m128i& value);

		/**
		 * Adds 2^shifts - 1 to each negative signed 32 bit value, so they each value can be right shifted to allow a correct division by 2^shifts.
		 * This function must be invoked before the right shift is applied.
		 * @param value The eight signed 32 bit values to be handled
		 * @param rightShifts The number of right shifts which needs to be applied, with range [0, 31]
		 * @return The modified value for which division a shift yield equal (and correct!) results
		 */
		static inline __m128i addOffsetBeforeRightShiftDivisionSigned32Bit(const __m128i& value, const unsigned int rightShifts);

		/**
		 * Divides eight signed 32 bit values by applying a right shift.
		 * This is able to determine the correct division result for positive and negative 32 bit values.
		 * @param value The eight signed 32 bit values to be handled
		 * @param rightShifts The number of right shifts which needs to be applied, with range [0, 32]
		 * @return The divided values
		 */
		static inline __m128i divideByRightShiftSigned32Bit(const __m128i& value, const unsigned int rightShifts);

		/**
		 * Determines the horizontal and the vertical gradients for 16 following pixels for a given 1 channel 8 bit frame.
		 * The resulting gradients are interleaved and each response is inside the range [-127, 127] as the standard response is divided by two.
		 * @param source The source position of the first pixel to determine the gradient for, this pixel must not be a border pixel in the original frame
		 * @param response Resulting gradient responses, first the horizontal response then the vertical response (zipped) for 8 pixels
		 * @param width The width of the original frame in pixel, with range [10, infinity)
		 */
		static inline void gradientHorizontalVertical8Elements1Channel8Bit(const uint8_t* source, int8_t* response, const unsigned int width);

		/**
		 * Determines the squared horizontal and vertical gradients and the product of both gradients for 16 following pixels for a given 1 channel 8 bit frame.
		 * The resulting gradients are interleaved and each response is inside the range [-(127 * 127), 127 * 127] as the standard response is divided by two.
		 * @param source The source position of the first pixel to determine the gradient for, this pixel must not be a border pixel in the original frame
		 * @param response Resulting gradient responses, first the horizontal response then the vertical response and afterwards the product of horizontal and vertical response (zipped) for 8 pixels
		 * @param width The width of the original frame in pixel, with range [10, infinity)
		 */
		static inline void gradientHorizontalVertical8Elements3Products1Channel8Bit(const uint8_t* source, int16_t* response, const unsigned int width);

		/**
		 * Sum absolute differences determination for the last 11 elements of a 16 elements buffer with 8 bit precision.
		 * @param image0 First 11 elements to determine the sad for, may be non aligned
		 * @param image1 Second 11 elements to determine the sad for, may be non aligned
		 * @return SSD result distributed over four terms of the sum, thus result is (m128i_u32[0] + m128i_u32[1] + m128i_u32[2] + m128i_u32[3])
		 */
		static inline __m128i sumAbsoluteDifferences8BitBack11Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum absolute differences determination for the first 10 elements of a buffer with 8 bit precision.
		 * This function supports to load the 10 elements from a buffer with only 10 bytes or with a buffer with at least 16 bytes.
		 * @param image0 First 10 elements to determine the sad for, may be non aligned
		 * @param image1 Second 10 elements to determine the sad for, may be non aligned
		 * @return SSD result distributed over four terms of the sum, thus result is (m128i_u32[0] + m128i_u32[1] + m128i_u32[2] + m128i_u32[3])
		 * @tparam tBufferHas16Bytes True, if the buffer holds at least 16 bytes; False, if the buffer holds 10 bytes only
		 */
		template <bool tBufferHas16Bytes>
		static inline __m128i sumAbsoluteDifferences8BitFront10Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum absolute differences determination for the first 15 elements of a buffer with 8 bit precision.
		 * This function supports to load the 15 elements from a buffer with only 15 bytes or with a buffer with at least 16 bytes.
		 * @param image0 First 15 elements to determine the sad for, may be non aligned
		 * @param image1 Second 15 elements to determine the sad for, may be non aligned
		 * @return SSD result distributed over four terms of the sum, thus result is (m128i_u32[0] + m128i_u32[1] + m128i_u32[2] + m128i_u32[3])
		 * @tparam tBufferHas16Bytes True, if the buffer holds at least 16 bytes; False, if the buffer holds 15 bytes only
		 */
		template <bool tBufferHas16Bytes>
		static inline __m128i sumAbsoluteDifferences8BitFront15Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Interpolates 8 elements of 2x2 blocks for 1 channel 8 bit frames.
		 * The first interpolation element results from the first and second element of both rows.<br>
		 * The second interpolation element results from the second and third element of both rows.<br>
		 * ...<br>
		 * The eighth interpolation element results from the eighth and ninth.<br>
		 * The interpolation is specified by tx and ty with range [0, 128u].<br>
		 * @param values0 First row of 9 elements to be interpolated
		 * @param values1 Second row of 9 elements to be interpolated
		 * @param fx_fy_ In each unsigned 16 bit element: Product of (128u - tx) and (128u - ty)
		 * @param fxfy_ In each unsigned 16 bit element: Product of (tx) and (128u - ty)
		 * @param fx_fy In each unsigned 16 bit element: Product of (128u - tx) and (ty)
		 * @param fxfy In each unsigned 16 bit element: Product of (tx) and (ty)
		 * @return Interpolation result for 8 elements, which are 8 pixels
		 */
		static inline __m128i interpolation1Channel8Bit8Elements(const __m128i& values0, const __m128i& values1, const __m128i& fx_fy_, const __m128i& fxfy_, const __m128i& fx_fy, const __m128i& fxfy);

		/**
		 * Interpolates 8 elements of 2x2 blocks for 2 channel 16 bit frames.
		 * The first interpolation element results from the first and second element of both rows.<br>
		 * The second interpolation element results from the second and third element of both rows.<br>
		 * ...<br>
		 * The eighth interpolation element results from the eighth and ninth.<br>
		 * The interpolation is specified by tx and ty with range [0, 128u].<br>
		 * @param values0 First row of 10 elements to be interpolated
		 * @param values1 Second row of 10 elements to be interpolated
		 * @param fx_fy_ In each unsigned 16 bit element: Product of (128u - tx) and (128u - ty)
		 * @param fxfy_ In each unsigned 16 bit element: Product of (tx) and (128u - ty)
		 * @param fx_fy In each unsigned 16 bit element: Product of (128u - tx) and (ty)
		 * @param fxfy In each unsigned 16 bit element: Product of (tx) and (ty)
		 * @return Interpolation result for 8 elements, which are 4 pixels
		 */
		static inline __m128i interpolation2Channel16Bit8Elements(const __m128i& values0, const __m128i& values1, const __m128i& fx_fy_, const __m128i& fxfy_, const __m128i& fx_fy, const __m128i& fxfy);

		/**
		 * Interpolates 8 elements of 2x2 blocks for 3 channel 24 bit frames.
		 * The first interpolation element results from the first and second element of both rows.<br>
		 * The second interpolation element results from the second and third element of both rows.<br>
		 * ...<br>
		 * The eighth interpolation element results from the eighth and ninth.<br>
		 * The interpolation is specified by tx and ty with range [0, 128u].<br>
		 * @param values0 First row of 11 elements to be interpolated
		 * @param values1 Second row of 11 elements to be interpolated
		 * @param fx_fy_ In each unsigned 16 bit element: Product of (128u - tx) and (128u - ty)
		 * @param fxfy_ In each unsigned 16 bit element: Product of (tx) and (128u - ty)
		 * @param fx_fy In each unsigned 16 bit element: Product of (128u - tx) and (ty)
		 * @param fxfy In each unsigned 16 bit element: Product of (tx) and (ty)
		 * @return Interpolation result for 8 elements, which are (2 2/3 pixels)
		 */
		static inline __m128i interpolation3Channel24Bit8Elements(const __m128i& values0, const __m128i& values1, const __m128i& fx_fy_, const __m128i& fxfy_, const __m128i& fx_fy, const __m128i& fxfy);

		/**
		 * Interpolates 15 elements of 2x2 blocks for 1 channel 8 bit frames.
		 * The interpolation is specified by tx and ty with range [0, 128u].<br>
		 * @param values0 First row of 16 elements to be interpolated
		 * @param values1 Second row of 16 elements to be interpolated
		 * @param fx_fy_fxfy_ In each unsigned 16 bit element: ((128u - tx) * (128u - ty)) | (tx * (128u - ty)) << 16
		 * @param fx_fyfxfy In each unsigned 16 bit element: (128u - tx) * ty | (tx * ty) << 16
		 * @return Interpolation result for 15 elements, which are (15 pixels)
		 */
		static inline __m128i interpolation1Channel8Bit15Elements(const __m128i& values0, const __m128i& values1, const __m128i& fx_fy_fxfy_, const __m128i& fx_fyfxfy);

		/**
		 * Interpolates 12 elements of 2x2 blocks for 3 channel 24 bit frames.
		 * The interpolation is specified by tx and ty with range [0, 128u].<br>
		 * @param values0 First row of 15 elements to be interpolated
		 * @param values1 Second row of 15 elements to be interpolated
		 * @param fx_fy_fxfy_ In each unsigned 16 bit element: ((128u - tx) * (128u - ty)) | (tx * (128u - ty)) << 16
		 * @param fx_fyfxfy In each unsigned 16 bit element: (128u - tx) * ty | (tx * ty) << 16
		 * @return Interpolation result for 12 elements, which are (4 pixels)
		 */
		static inline __m128i interpolation3Channel24Bit12Elements(const __m128i& values0, const __m128i& values1, const __m128i& fx_fy_fxfy_, const __m128i& fx_fyfxfy);

		/**
		 * Interpolates 8 elements of 2x2 blocks for 4 channel 32 bit frames.
		 * The first interpolation element results from the first and second element of both rows.<br>
		 * The second interpolation element results from the second and third element of both rows.<br>
		 * ...<br>
		 * The eighth interpolation element results from the eighth and ninth.<br>
		 * The interpolation is specified by tx and ty with range [0, 128u].<br>
		 * @param values0 First row of 12 elements to be interpolated
		 * @param values1 Second row of 12 elements to be interpolated
		 * @param fx_fy_ In each unsigned 16 bit element: Product of (128u - tx) and (128u - ty)
		 * @param fxfy_ In each unsigned 16 bit element: Product of (tx) and (128u - ty)
		 * @param fx_fy In each unsigned 16 bit element: Product of (128u - tx) and (ty)
		 * @param fxfy In each unsigned 16 bit element: Product of (tx) and (ty)
		 * @return Interpolation result for 8 elements, which are (2 pixels)
		 */
		static inline __m128i interpolation4Channel32Bit8Elements(const __m128i& values0, const __m128i& values1, const __m128i& fx_fy_, const __m128i& fxfy_, const __m128i& fx_fy, const __m128i& fxfy);

		/**
		 * Interpolates 2x4 elements (two seperated blocks of 4 elements) of 2x2 blocks for 4 channel 32 bit frames.
		 * The first interpolation element results from the first and second element of both rows.<br>
		 * The second interpolation element results from the second and third element of both rows.<br>
		 * ...<br>
		 * The eighth interpolation element results from the eighth and ninth.<br>
		 * The interpolation is specified by tx and ty with range [0, 128u].<br>
		 * @param values0 First row of 16 elements to be interpolated
		 * @param values1 Second row of 16 elements to be interpolated
		 * @param fx_fy_ In each unsigned 16 bit element: Product of (128u - tx) and (128u - ty)
		 * @param fxfy_ In each unsigned 16 bit element: Product of (tx) and (128u - ty)
		 * @param fx_fy In each unsigned 16 bit element: Product of (128u - tx) and (ty)
		 * @param fxfy In each unsigned 16 bit element: Product of (tx) and (ty)
		 * @return Interpolation result for 8 elements, which are (2 2/3 pixels)
		 */
		static inline __m128i interpolation4Channel32Bit2x4Elements(const __m128i& values0, const __m128i& values1, const __m128i& fx_fy_, const __m128i& fxfy_, const __m128i& fx_fy, const __m128i& fxfy);

		/**
		 * Returns the interpolated sum of square difference for one 2 channel 16 bit pixel.
		 * @param pixel0 Uppler left pixel in the first frame
		 * @param pixel1 Uppler left pixel in the second frame
		 * @param size0 Size of one frame row in bytes
		 * @param size1 Size of one frame row in bytes
		 * @param f1x_y_ Product of the inverse fx and the inverse fy interpolation factor for the second image
		 * @param f1xy_ Product of the fx and the inverse fy interpolation factor for the second image
		 * @param f1x_y Product of the inverse fx and the fy interpolation factor for the second image
		 * @param f1xy Product of the fx and the fy interpolation factor for the second image
		 * @return Interpolated sum of square difference
		 */
		static inline unsigned int ssd2Channel16Bit1x1(const uint8_t* const pixel0, const uint8_t* const pixel1, const unsigned int size0, const unsigned int size1, const unsigned int f1x_y_, const unsigned int f1xy_, const unsigned int f1x_y, const unsigned int f1xy);

		/**
		 * Returns the interpolated sum of square difference for one 2 channel 16 bit pixel.
		 * @param pixel0 Uppler left pixel in the first frame
		 * @param pixel1 Uppler left pixel in the second frame
		 * @param size0 Size of one frame row in bytes
		 * @param size1 Size of one frame row in bytes
		 * @param f0x_y_ Product of the inverse fx and the inverse fy interpolation factor for the first image
		 * @param f0xy_ Product of the fx and the inverse fy interpolation factor for the first image
		 * @param f0x_y Product of the inverse fx and the fy interpolation factor for the first image
		 * @param f0xy Product of the fx and the fy interpolation factor for the first image
		 * @param f1x_y_ Product of the inverse fx and the inverse fy interpolation factor for the second image
		 * @param f1xy_ Product of the fx and the inverse fy interpolation factor for the second image
		 * @param f1x_y Product of the inverse fx and the fy interpolation factor for the second image
		 * @param f1xy Product of the fx and the fy interpolation factor for the second image
		 * @return Interpolated sum of square difference
		 */
		static inline unsigned int ssd2Channel16Bit1x1(const uint8_t* const pixel0, const uint8_t* const pixel1, const unsigned int size0, const unsigned int size1, const unsigned int f0x_y_, const unsigned int f0xy_, const unsigned int f0x_y, const unsigned int f0xy, const unsigned int f1x_y_, const unsigned int f1xy_, const unsigned int f1x_y, const unsigned int f1xy);

		/**
		 * Sum absolute differences determination for 16 elements of an 16 elements buffer with 8 bit precision.
		 * @param image0 First 16 elements to determine the ssd for, may be non aligned
		 * @param image1 Second 16 elements to determine the ssd for, may be non aligned
		 * @return SSD result distributed over four terms of the sum, thus result is (m128i_u32[0] + m128i_u32[1] + m128i_u32[2] + m128i_u32[3])
		 */
		static inline __m128i sumAbsoluteDifferences8Bit16Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Deinterleaves 15 elements of e.g., and image with 3 channels and 8 bit per element.
		 * This functions converts X CBA CBA CBA CBA CBA to 00000000000CCCCC 000BBBBB000AAAAA.
		 * @param interleaved The 15 elements holding the interleaved image data
		 * @param channel01 Resulting first and second channel elements, first 8 elements of the first channel, followed by 8 elements of the second channel
		 * @param channel2 Resulting third channel elements, first 8 elements of the third channel, followed by zeros
		 */
		static OCEAN_FORCE_INLINE void deInterleave3Channel8Bit15Elements(const __m128i& interleaved, __m128i& channel01, __m128i& channel2);

		/**
		 * Deinterleaves 24 elements of e.g., and image with 3 channels and 8 bit per element.
		 * This functions converts XX XXX XXX CBA CBA CB  A CBA CBA CBA CBA CBA to 00000000CCCCCCCC BBBBBBBBAAAAAAAA.
		 * @param interleavedA First 16 elements holding the interleaved image data
		 * @param interleavedB Second 16 elements holding the interleaved image data, the first 8 elements will be used only
		 * @param channel01 Resulting first and second channel elements, first 8 elements of the first channel, followed by 8 elements of the second channel
		 * @param channel2 Resulting third channel elements, first 8 elements of the third channel, followed by zeros
		 */
		static OCEAN_FORCE_INLINE void deInterleave3Channel8Bit24Elements(const __m128i& interleavedA, const __m128i& interleavedB, __m128i& channel01, __m128i& channel2);

		/**
		 * Deinterleaves 48 elements of e.g., and image with 3 channels and 8 bit per element.
		 * This functions converts CBA CBA CBA CBA CBA C  BA CBA CBA CBA CBA CB  A CBA CBA CBA CBA CBA to CCCCCCCCCCCCCCCC BBBBBBBBBBBBBBBB AAAAAAAAAAAAAAAA.
		 * @param interleavedA First 16 elements holding the interleaved image data
		 * @param interleavedB Second 16 elements holding the interleaved image data
		 * @param interleavedC Third 16 elements holding the interleaved image data
		 * @param channel0 Resulting first channel holding all elements corresponding to the first channel consecutively
		 * @param channel1 Resulting second channel holding all elements corresponding to the second channel consecutively
		 * @param channel2 Resulting third channel holding all elements corresponding to the third channel consecutively
		 */
		static OCEAN_FORCE_INLINE void deInterleave3Channel8Bit48Elements(const __m128i& interleavedA, const __m128i& interleavedB, const __m128i& interleavedC, __m128i& channel0, __m128i& channel1, __m128i& channel2);

		/**
		 * Deinterleaves 48 elements of e.g., an image with 3 channels and 8 bit per element.
		 * @param interleaved 48 elements of an image with 3 channels and 8 bit per element (48 bytes)
		 * @param channel0 Resulting first channel holding all elements corresponding to the first channel consecutively
		 * @param channel1 Resulting second channel holding all elements corresponding to the second channel consecutively
		 * @param channel2 Resulting third channel holding all elements corresponding to the third channel consecutively
		 */
		static inline void deInterleave3Channel8Bit48Elements(const uint8_t* interleaved, __m128i& channel0, __m128i& channel1, __m128i& channel2);

		/**
		 * Deinterleaves 48 elements of e.g., an image with 3 channels and 8 bit per element.
		 * @param interleaved 48 elements of an image with 3 channels and 8 bit per element (48 bytes), must be valid
		 * @param channel0 Resulting first channel holding all elements corresponding to the first channel consecutively, must be valid
		 * @param channel1 Resulting second channel holding all elements corresponding to the second channel consecutively, must be valid
		 * @param channel2 Resulting third channel holding all elements corresponding to the third channel consecutively, must be valid
		 */
		static inline void deInterleave3Channel8Bit48Elements(const uint8_t* interleaved, uint8_t* channel0, uint8_t* channel1, uint8_t* channel2);

		/**
		 * Deinterleaves 45 elements of e.g., an image with 3 channels and 8 bit per element.
		 * @param interleaved 45 elements of an image with 3 channels and 8 bit per element (45 bytes), must be valid
		 * @param channel0 Resulting first channel holding all elements corresponding to the first channel consecutively
		 * @param channel1 Resulting second channel holding all elements corresponding to the second channel consecutively
		 * @param channel2 Resulting third channel holding all elements corresponding to the third channel consecutively
		 */
		static inline void deInterleave3Channel8Bit45Elements(const uint8_t* interleaved, __m128i& channel0, __m128i& channel1, __m128i& channel2);

		/**
		 * Interleaves 48 elements of e.g., an image with 3 channels and 8 bit per element.
		 * This functions converts CCCCCCCCCCCCCCCC BBBBBBBBBBBBBBBB AAAAAAAAAAAAAAAA to CBA CBA CBA CBA CBA C  BA CBA CBA CBA CBA CB  A CBA CBA CBA CBA CBA.
		 * @param channel0 The 16 elements of the first channel to be interleaved
		 * @param channel1 The 16 elements of the second channel to be interleaved
		 * @param channel2 The 16 elements of the third channel to be interleaved
		 * @param interleavedA Resulting first 16 of the interleaved data
		 * @param interleavedB Resulting second 16 of the interleaved data
		 * @param interleavedC Resulting third 16 of the interleaved data
		 */
		OCEAN_FORCE_INLINE static void interleave3Channel8Bit48Elements(const __m128i& channel0, const __m128i& channel1, const __m128i& channel2, __m128i& interleavedA, __m128i& interleavedB, __m128i& interleavedC);

		/**
		 * Deinterleaves 48 elements of e.g., an image with 3 channels and 8 bit per element.
		 * @param channel0 The 16 elements of the first channel to be interleaved, must be valid
		 * @param channel1 The 16 elements of the second channel to be interleaved, must be valid
		 * @param channel2 The 16 elements of the third channel to be interleaved, must be valid
		 * @param interleaved The resulting 48 interleaved elements, must be valid
		 */
		static OCEAN_FORCE_INLINE void interleave3Channel8Bit48Elements(const uint8_t* const channel0, const uint8_t* const channel1, const uint8_t* const channel2, uint8_t* const interleaved);

		/**
		 * Reverses the order of the channels of 16 pixels (32 elements) of an image with 2 interleaved channels and 8 bit per element (e.g., YA16 to AY16).
		 * @param interleaved 16 elements of an image with 2 channels and 8 bit per element (32 bytes)
		 * @param reversedInterleaved Resulting 32 elements with reversed channel order
		 */
		static OCEAN_FORCE_INLINE void reverseChannelOrder2Channel8Bit32Elements(const uint8_t* interleaved, uint8_t* reversedInterleaved);

		/**
		 * Reverses the order of the first and last channel of 48 elements of an image with 3 interleaved channels and 8 bit per element.
		 * @param interleaved0 First 16 elements holding the interleaved image data
		 * @param interleaved1 Second 16 elements holding the interleaved image data
		 * @param interleaved2 Third 16 elements holding the interleaved image data
		 * @param reversedInterleaved0 Resulting first 16 elements holding the interleaved image data with reversed channel order
		 * @param reversedInterleaved1 Resulting second 16 elements holding the interleaved image data with reversed channel order
		 * @param reversedInterleaved2 Resulting third 16 elements holding the interleaved image data with reversed channel order
		 */
		static OCEAN_FORCE_INLINE void reverseChannelOrder3Channel8Bit48Elements(const __m128i& interleaved0, const __m128i& interleaved1, const __m128i& interleaved2, __m128i& reversedInterleaved0, __m128i& reversedInterleaved1, __m128i& reversedInterleaved2);

		/**
		 * Reverses the order of the first and last channel of 48 elements (16 pixels) of an image with 3 interleaved channels and 8 bit per element (e.g., RGB24 to BGR24).
		 * @param interleaved 48 elements of an image with 3 channels and 8 bit per element (48 bytes)
		 * @param reversedInterleaved Resulting 48 elements with reversed channel order
		 */
		static OCEAN_FORCE_INLINE void reverseChannelOrder3Channel8Bit48Elements(const uint8_t* interleaved, uint8_t* reversedInterleaved);

		/**
		 * Reverses the order of the channels of 16 pixels (64 elements) of an image with 4 interleaved channels and 8 bit per element (e.g., RGBA32 to ABGR24).
		 * @param interleaved 64 elements of an image with 4 channels and 8 bit per element (64 bytes)
		 * @param reversedInterleaved Resulting 64 elements with reversed channel order
		 */
		static OCEAN_FORCE_INLINE void reverseChannelOrder4Channel8Bit64Elements(const uint8_t* interleaved, uint8_t* reversedInterleaved);

		/**
		 * Reverses the order of the first and last channel of 48 elements of an image with 3 interleaved channels and 8 bit per element (in place).
		 * @param interleaved 48 elements of an image with 3 channels and 8 bit per element (48 bytes)
		 */
		static void reverseChannelOrder3Channel8Bit48Elements(uint8_t* interleaved);

		/**
		 * Reverses the order of the first and last channel of two sets of 48 elements of an image with 3 interleaved channels and 8 bit per element and further swaps both sets.
		 * @param first First 48 elements of an image with 3 channels and 8 bit per element (48 bytes)
		 * @param second Second 48 elements of an image with 3 channels and 8 bit per element (48 bytes)
		 */
		static inline void swapReversedChannelOrder3Channel8Bit48Elements(uint8_t* first, uint8_t* second);

		/**
		 * Reverses the order of 48 elements with 8 bit per element.
		 * @param elements0 First 16 elements
		 * @param elements1 Second 16 elements
		 * @param elements2 Third 16 elements
		 * @param reversedElements0 Resulting reversed first 16 elements
		 * @param reversedElements1 Resulting reversed second 16 elements
		 * @param reversedElements2 Resulting reversed third 16 elements
		 */
		static inline void reverseElements8Bit48Elements(const __m128i& elements0, const __m128i& elements1, const __m128i& elements2, __m128i& reversedElements0, __m128i& reversedElements1, __m128i& reversedElements2);

		/**
		 * Reverses the order of 48 elements with 8 bit per element.
		 * @param elements 48 elements that will be reversed
		 * @param reversedElements Resulting reversed 48 elements
		 */
		static inline void reverseElements8Bit48Elements(const uint8_t* elements, uint8_t* reversedElements);

		/**
		 * Reverses the order of 48 elements with 8 bit per element (in place).
		 * @param elements 48 elements that will be reversed
		 */
		static inline void reverseElements8Bit48Elements(uint8_t* elements);

		/**
		 * Reverses the order of two sets of 48 elements with 8 bit per element and further swaps both sets.
		 * @param first First 48 elements that will be reversed and swapped with the second 48 elements
		 * @param second Second 48 elements that will be reversed and swapped with the first 48 elements
		 */
		static inline void swapReversedElements8Bit48Elements(uint8_t* first, uint8_t* second);

		/**
		 * Shifts the channels of a 4 channel 32 bit pixels to the front and moves the front channel to the back channel.
		 * The function takes four pixels DCBA DCBA DCBA DCBA and provides ADCB ADCB ADCB ADCB.<br>
		 * @param elements 16 elements of 4 pixels to be shifted
		 * @param shiftedElements Resulting shifted elements
		 */
		static inline void shiftChannelToFront4Channel32Bit(const uint8_t* elements, uint8_t* shiftedElements);

		/**
		 * Shifts the channels of a 4 channel 32 bit pixels to the front and moves the front channel to the back channel and mirrors the four individual pixels.
		 * @param elements 16 elements of 4 pixels to be shifted and mirrored
		 * @param shiftedElements Resulting shifted and mirrored elements
		 */
		static inline void shiftAndMirrorChannelToFront4Channel32Bit(const uint8_t* elements, uint8_t* shiftedElements);

		/**
		 * Shifts the channels of a 4 channel 32 bit pixels to the back and moves the back channel to the front channel.
		 * The function takes four pixels DCBA DCBA DCBA DCBA and provides CBAD CBAD CBAD CBAD.<br>
		 * @param elements 16 elements of 4 pixels to be shifted
		 * @param shiftedElements Resulting shifted elements
		 */
		static inline void shiftChannelToBack4Channel32Bit(const uint8_t* elements, uint8_t* shiftedElements);

		/**
		 * Shifts the channels of a 4 channel 32 bit pixels to the back and moves the back channel to the front channel and mirrors the four individual pixels.
		 * @param elements 16 elements of 4 pixels to be shifted and mirrored
		 * @param shiftedElements Resulting shifted and mirrored elements
		 */
		static inline void shiftAndMirrorChannelToBack4Channel32Bit(const uint8_t* elements, uint8_t* shiftedElements);

		/**
		 * Sums 16 elements with 8 bit per element.
		 * The results are stored as first 32 bit integer value (high bits left, low bits right): ???? ???? ???? 0000.<br>
		 * @param elements 16 elements holding the image data
		 * @return Resulting sums
		 */
		static inline __m128i sum1Channel8Bit16Elements(const __m128i& elements);

		/**
		 * Sums 16 elements with 8 bit per element.
		 * The results are stored as first 32 bit integer value (high bits left, low bits right): ???? ???? ???? 0000.<br>
		 * @param elements 16 elements holding the image data
		 * @return Resulting sums
		 */
		static inline __m128i sum1Channel8Bit16Elements(const uint8_t* elements);

		/**
		 * Sums the first 15 elements of a buffer with 8 bit per element.
		 * This function supports to load the 15 elements from a buffer with only 15 bytes or with a buffer with at least 16 bytes.<br>
		 * If the provided buffer holds at least 16 bytes the load function is much faster compared to the case if the buffer is not larger than 15 bytes.<br>
		 * The results are stored as first 32 bit integer value (high bits left, low bits right): ???? ???? ???? 0000.
		 * @param elements 16 elements holding the image data
		 * @return Resulting sums
		 * @tparam tBufferHas16Bytes True, if the buffer holds at least 16 bytes; False, if the buffer holds only 15 bytes
		 */
		template <bool tBufferHas16Bytes>
		static inline __m128i sum1Channel8BitFront15Elements(const uint8_t* elements);

		/**
		 * Sums the last 15 elements of a 16 elements buffer with 8 bit per element, the beginning 1 element is interpreted as zero.
		 * However, the provided buffer must be at least 16 bytes large as the entire 16 bytes will be loaded to the SSE register.<br>
		 * Thus, this functions handles one buffer with this pattern (while the memory starts left and ends right): [NA 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15].
		 * The results are stored as first 32 bit integer value (high bits left, low bits right): ???? ???? ???? 0000.
		 * @param elements (1+) 15 elements holding the image data
		 * @return Resulting sum
		 */
		static inline __m128i sum1Channel8BitBack15Elements(const uint8_t* elements);

		/**
		 * Sums 16 elements individually for an interleaved pixel format with 3 channels and 8 bit per channel and element.
		 * The results are stored in three 32 bit integer values (high bits left, low bits right): ???? 2222 1111 0000.<br>
		 * @param interleaved0 First 16 elements holding the interleaved image data
		 * @param interleaved1 Second 16 elements holding the interleaved image data
		 * @param interleaved2 Third 16 elements holding the interleaved image data
		 * @return Resulting sums
		 */
		static inline __m128i sumInterleave3Channel8Bit48Elements(const __m128i& interleaved0, const __m128i& interleaved1, const __m128i& interleaved2);

		/**
		 * Sums 16 elements individually for an interleaved pixel format with 3 channels and 8 bit per channel and element.
		 * The results are stored in three 32 bit integer values (high bits left, low bits right): ???? 2222 1111 0000.<br>
		 * @param interleaved 48 elements holding the interleaved image data
		 * @return Resulting sums
		 */
		static inline __m128i sumInterleave3Channel8Bit48Elements(const uint8_t* interleaved);

		/**
		 * Sums 15 elements individually for an interleaved pixel format with 3 channels and 8 bit per channel and element.
		 * The results are stored in three 32 bit integer values (high bits left, low bits right): ???? 2222 1111 0000.<br>
		 * @param interleaved 45 elements holding the interleaved image data
		 * @return Resulting sums
		 */
		static inline __m128i sumInterleave3Channel8Bit45Elements(const uint8_t* interleaved);

		/**
		 * Loads the lower 64 bit of a 128i value from the memory.
		 * The upper 64 bit are zeroed.
		 * @param buffer Buffer to be loaded (does not need to be aligned on any particular boundary), ensure that the buffer has a size of at least 8 bytes
		 * @return Resulting value
		 */
		static inline __m128i load128iLower64(const void* const buffer);

		/**
		 * Loads a 128i value from the memory.
		 * @param buffer Buffer to be loaded (does not need to be aligned on any particular boundary), ensure that the buffer has a size of at least 16 bytes
		 * @return Resulting value
		 */
		static inline __m128i load128i(const void* const buffer);

		/**
		 * Loads 10 bytes from memory, which holds either at least 16 bytes or exactly 10 bytes, to a 128i value and sets the remaining bytes of the resulting 128i value to zero.
		 * The loaded memory will be stored in the upper 10 bytes of the 128i value while the lowest remaining 6 bytes will be set to zero.
		 * Thus, the resulting 128 bit value has the following byte pattern (high bits left, low bits right): [09 08 07 06 05 04 03 02 01 00 ZZ ZZ ZZ ZZ ZZ ZZ], with ZZ meaning zero.<br>
		 * @param buffer Buffer to be loaded (does not need to be aligned on any particular boundary)
		 * @return Resulting 128 bit value
		 * @tparam tBufferHas16Bytes True, if the buffer holds at least 16 bytes; False, if the buffer holds only 10 bytes
		 */
		template <bool tBufferHas16Bytes>
		static inline __m128i load_u8_10_upper_zero(const uint8_t* const buffer);

		/**
		 * Loads 15 bytes from memory, which holds either at least 16 bytes or exactly 15 bytes, to a 128i value and sets the remaining byte of the resulting 128i value to zero.
		 * The loaded memory will be stored in the upper 15 bytes of the 128i value while the lowest remaining 1 byte will be set to zero.
		 * Thus, the resulting 128 bit value has the following byte pattern (high bits left, low bits right): [14 13 12 11 10 09 08 07 06 05 04 03 02 01 00 ZZ], with ZZ meaning zero.<br>
		 * @param buffer Buffer to be loaded (does not need to be aligned on any particular boundary)
		 * @return Resulting 128 bit value
		 * @tparam tBufferHas16Bytes True, if the buffer holds at least 16 bytes; False, if the buffer holds only 15 bytes
		 */
		template <bool tBufferHas16Bytes>
		static inline __m128i load_u8_15_upper_zero(const uint8_t* const buffer);

		/**
		 * Loads 13 bytes from memory, which holds either at least 16 bytes or exactly 13 bytes, to a 128i value while the remaining byte of the resulting 128i value will be random.
		 * The loaded memory will be stored in the lower 13 bytes of the 128i value while the highest remaining 3 byte will be random.<br>
		 * Thus, the resulting 128 bit value has the following byte pattern (high bits left, low bits right): [?? ?? ?? 12 11 10 09 08 07 06 05 04 03 02 01 00], with ?? meaning a random value.<br>
		 * @param buffer Buffer to be loaded (does not need to be aligned on any particular boundary)
		 * @return Resulting 128 bit value
		 * @tparam tBufferHas16Bytes True, if the buffer holds at least 16 bytes; False, if the buffer holds only 13 bytes
		 */
		template <bool tBufferHas16Bytes>
		static inline __m128i load_u8_13_lower_random(const uint8_t* const buffer);

		/**
		 * Loads 15 bytes from memory, which holds either at least 16 bytes or exactly 15 bytes, to a 128i value and sets the remaining byte of the resulting 128i value to zero.
		 * The loaded memory will be stored in the lower 15 bytes of the 128i value while the highest remaining 1 byte will be set to zero.<br>
		 * Thus, the resulting 128 bit value has the following byte pattern (high bits left, low bits right): [-- 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00], with ZZ meaning zero.<br>
		 * @param buffer Buffer to be loaded (does not need to be aligned on any particular boundary)
		 * @return Resulting 128 bit value
		 * @tparam tBufferHas16Bytes True, if the buffer holds at least 16 bytes; False, if the buffer holds only 15 bytes
		 */
		template <bool tBufferHas16Bytes>
		static inline __m128i load_u8_15_lower_zero(const uint8_t* const buffer);

		/**
		 * Loads 15 bytes from memory, which holds either at least 16 bytes or exactly 15 bytes, to a 128i value while the remaining byte of the resulting 128i value will be random.
		 * The loaded memory will be stored in the lower 15 bytes of the 128i value while the highest remaining 1 byte will be random.<br>
		 * Thus, the resulting 128 bit value has the following byte pattern (high bits left, low bits right): [?? 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00], with ?? meaning a random value.<br>
		 * @param buffer Buffer to be loaded (does not need to be aligned on any particular boundary)
		 * @return Resulting 128 bit value
		 * @tparam tBufferHas16Bytes True, if the buffer holds at least 16 bytes; False, if the buffer holds only 15 bytes
		 */
		template <bool tBufferHas16Bytes>
		static inline __m128i load_u8_15_lower_random(const uint8_t* const buffer);

		/**
		 * Loads 16 bytes from memory which is at least 16 bytes large and shifts the 128i value by a specified number of bytes to the right (by inserting zeros).
		 * This function can be used if the remaining buffer is smaller than 16 bytes while the buffer exceeds/continues in the lower address space (from the original point of interest).<br>
		 * Thus, this function an handle a buffer with the following pattern (with lower address left and high address right):<br>
		 * | ?? ?? ?? ?? ?? ?? ?? ?? ?? V0 V1 V2 V3 V4 V5 V6 V7 V8 V9 |, where ?? represent random values in our buffer (in the lower address space), and VX represent the values of interest and V0 the location to which 'buffer' is pointing to.<br>
		 * by load_u8_16_and_shift_right<6>(buffer - 6);<br>
		 * The resulting 128i register will then be composed of (high bits left, low bits right): [00 00 00 00 00 00 V9 V8 V7 V6 V5 V4 V3 V2 V1 V0].
		 * @param buffer The actual address from which the 16 bytes will be loaded, must be valid and must be at least 16 bytes large
		 * @return The resulting 128 bit value
		 * @tparam tShiftBytes The number of bytes which will be shifted (to the right) after the memory has loaded, with range [0, 16]
		 */
		template <unsigned int tShiftBytes>
		static inline __m128i load_u8_16_and_shift_right(const uint8_t* const buffer);

		/**
		 * Stores a 128i value to the memory.
		 * @param value Value to be stored
		 * @param buffer Buffer receiving the value (does not need to be aligned on any particular boundary)
		 */
		static inline void store128i(const __m128i& value, uint8_t* const buffer);

		/**
		 * Sets a 128i value by two 64 bit values.
		 * @param high64 High 64 bits to be set
		 * @param low64 Low 64 bits to be set
		 * @return Resulting 128i value
		 */
		static inline __m128i set128i(const unsigned long long high64, const unsigned long long low64);

		/**
		 * Removes the higher 16 bits of four 32 bit elements.
		 * Given:  PONM-LKJI-HGFE-DCBA<br>
		 * Result: 00NM-00JI-00FE-00BA<br>
		 * @param value Value to remove the high bits for
		 * @return Result
		 */
		static inline __m128i removeHighBits32_16(const __m128i& value);

		/**
		 * Removes the lower 16 bits of four 32 bit elements.
		 * Given:  PONM-LKJI-HGFE-DCBA<br>
		 * Result: PO00-LK00-HG00-DC00<br>
		 * @param value Value to remove the lower bits for
		 * @return Result
		 */
		static inline __m128i removeLowBits32_16(const __m128i& value);

		/**
		 * Removes the higher 8 bits of eight 16 bit elements.
		 * Given:  PONM-LKJI-HGFE-DCBA<br>
		 * Result: 0O0M-0K0I-0G0E-0C0A<br>
		 * @param value Value to remove the high bits for
		 * @return Result
		 */
		static inline __m128i removeHighBits16_8(const __m128i& value);

		/**
		 * Removes the higher 8 bits of eight 16 bit elements and sets the upper two bytes to zero.
		 * Given:  PONM-LKJI-HGFE-DCBA<br>
		 * Result: 000M-0K0I-0G0E-0C0A<br>
		 * @param value Value to remove the high bits for
		 * @return Result
		 */
		static inline __m128i removeHighBits16_8_7_lower(const __m128i& value);

		/**
		 * Removes the higher 8 bits of eight 16 bit elements and sets the lower two bytes to zero.
		 * Given:  PONM-LKJI-HGFE-DCBA<br>
		 * Result: 0O0M-0K0I-0G0E-0C00<br>
		 * @param value Value to remove the high bits for
		 * @return Result
		 */
		static inline __m128i removeHighBits16_8_7_upper(const __m128i& value);

		/**
		 * Moves the lower 8 bits of eight 16 bit elements to the lower 64 bits and fills the high 64 bits with 0.
		 * Given:  PONM-LKJI-HGFE-DCBA<br>
		 * Result: 0000-0000-OMKI-GECA<br>
		 * @param value Value to remove the high bits for
		 * @return Result
		 */
		static inline __m128i moveLowBits16_8ToLow64(const __m128i& value);

		/**
		 * Moves the lower 8 bits of four 32 bit elements to the lower 32 bits and fills the high 96 bits with 0.
		 * Given:  PONM-LKJI-HGFE-DCBA<br>
		 * Result: 0000-0000-0000-MIEA<br>
		 * @param value Value to remove the high bits for
		 * @return Result
		 */
		static inline __m128i moveLowBits32_8ToLow32(const __m128i& value);

		/**
		 * Moves the lower 16 bits of four 32 bit elements to the lower 64 bits and fills the high 64 bits with 0.
		 * Given:  PONM-LKJI-HGFE-DCBA<br>
		 * Result: 0000-0000-NMJI-FEBA<br>
		 * @param value Value to remove the high bits for
		 * @return Result
		 */
		static inline __m128i moveLowBits32_16ToLow64(const __m128i& value);

		/**
		 * Moves the lower 8 bits of eight 16 bit elements to the higher 64 bits and fills the low 64 bits with 0.
		 * Given:  PONM-LKJI-HGFE-DCBA<br>
		 * Result: OMKI-GECA-0000-0000<br>
		 * @param value Value to remove the high bits for
		 * @return Result
		 */
		static inline __m128i moveLowBits16_8ToHigh64(const __m128i& value);

		/**
		 * Moves the higher 16 bits of four 32 bit elements to the lower 16 bits and fills the high bits with 0.
		 * Given:  PONM-LKJI-HGFE-DCBA<br>
		 * Result: 00PO-00LK-00HG-00DC<br>
		 * @param value Value to remove the high bits for
		 * @return Result
		 */
		static inline __m128i moveHighBits32_16(const __m128i& value);

		/**
		 * Moves the higher 8 bits of eight 16 bit elements to the lower 8 bits and fills the high bits with 0.
		 * Given:  PONM-LKJI-HGFE-DCBA<br>
		 * Result: 0P0N-0L0J-0H0F-0D0B<br>
		 * @param value Value to remove the high bits for
		 * @return Result
		 */
		static inline __m128i moveHighBits16_8(const __m128i& value);

		/**
		 * Moves the higher 8 bits of five 16 bit elements to the lower 8 bits and fills the high bits with 0.
		 * Given:  PONM-LKJI-HGFE-DCBA<br>
		 * Result: 0000-000J-0H0F-0D0B<br>
		 * @param value Value to remove the high bits for
		 * @return Result
		 */
		static inline __m128i moveHighBits16_8_5(const __m128i& value);

		/**
		 * Moves the higher 8 bits of six 16 bit elements to the lower 8 bits and fills the high bits with 0.
		 * Given:  PONM-LKJI-HGFE-DCBA<br>
		 * Result: 0000-0L0J-0H0F-0D0B<br>
		 * @param value Value to remove the high bits for
		 * @return Result
		 */
		static inline __m128i moveHighBits16_8_6(const __m128i& value);

		/**
		 * Moves the higher 8 bits of seven 16 bit elements to the lower 8 bits and fills the high bits with 0.
		 * Given:  PONM-LKJI-HGFE-DCBA<br>
		 * Result: 000N-0L0J-0H0F-0D0B<br>
		 * @param value Value to remove the high bits for
		 * @return Result
		 */
		static inline __m128i moveHighBits16_8_7(const __m128i& value);

		/**
		 * Shuffles the lower four 8 bits to the low 8 bits of four 32 bit elements.
		 * Given:  PONM-LKJI-HGFE-DCBA<br>
		 * Result: 000D-000C-000B-000A<br>
		 * @param value Value to be shuffled
		 * @return Result
		 */
		static inline __m128i shuffleLow32ToLow32_8(const __m128i& value);

		/**
		 * Shuffles pairs of four neighbors of the low 64 bits to the low 8 bits of eight 16 bit elements.
		 * Given:  PONM-LKJI-HGFE-DCBA<br>
		 * Result: 0H0D-0G0C-0F0B-0E0A<br>
		 * @param value Value to be shuffled
		 * @return Result
		 */
		static inline __m128i shuffleNeighbor4Low64BitsToLow16_8(const __m128i& value);

		/**
		 * Shuffles pairs of four neighbors of the high 64 bits to the low 8 bits of eight 16 bit elements.
		 * Given:  PONM-LKJI-HGFE-DCBA<br>
		 * Result: 0P0L-0O0K-0N0J-0M0I<br>
		 * @param value Value to be shuffled
		 * @return Result
		 */
		static inline __m128i shuffleNeighbor4High64BitsToLow16_8(const __m128i& value);

		/**
		 * Shuffles pairs of two neighbors of the low 64 bits to the low 8 bits of eight 16 bit elements.
		 * @param value Value to be shuffled
		 * @return Result
		 */
		static inline __m128i shuffleNeighbor2Low64BitsToLow16_8(const __m128i& value);

		/**
		 * Shuffles pairs of two neighbors of the high 64 bits to the low 8 bits of eight 16 bit elements.
		 * @param value Value to be shuffled
		 * @return Result
		 */
		static inline __m128i shuffleNeighbor2High64BitsToLow16_8(const __m128i& value);

		/**
		 * Returns the following 128 bit mask: 0x00FF00FF-00FF00FF-00FF00FF-00FF00FF.
		 * @return Bitmask
		 */
		static inline __m128i bitMaskRemoveHigh16_8();

		/**
		 * Returns the following 128 bit mask: 0x0000FFFF-0000FFFF-0000FFFF-0000FFFF.
		 * @return Bitmask
		 */
		static inline __m128i bitMaskRemoveHigh32_16();

		/**
		 * Multiplies 8 int16_t values with 8 int16_t values and returns the products as 8 int32_t results.
		 * The pseudo code of the function is as follows:
		 * <pre>
		 * products0[0] = values0[0] * values1[0]
		 * ...
		 * products0[3] = values0[3] * values1[3]
		 *
		 * products1[0] = values0[4] * values1[4]
		 * ...
		 * products1[3] = values0[7] * values1[7]
		 * </pre>
		 * @param values0 The first 8 int16_t values to be multiplied
		 * @param values1 The second 8 int16_t values to be multiplied
		 * @param products0 The resulting first 4 int32_t products
		 * @param products1 The resulting second 4 int32_t products
		 */
		static OCEAN_FORCE_INLINE void multiplyInt8x16ToInt32x8(const __m128i& values0, const __m128i& values1, __m128i& products0, __m128i& products1);

		/**
		 * Multiplies 8 int16_t values with 8 int16_t values and adds the products to 8 int32_t values.
		 * The pseudo code of the function is as follows:
		 * <pre>
		 * results0[0] += values0[0] * values1[0]
		 * ...
		 * results0[3] += values0[3] * values1[3]
		 *
		 * results1[0] += values0[4] * values1[4]
		 * ...
		 * results1[3] += values0[7] * values1[7]
		 * </pre>
		 * @param values0 The first 8 int16_t values to be multiplied
		 * @param values1 The second 8 int16_t values to be multiplied
		 * @param results0 The results to which the first 4 int32_t products will be added
		 * @param results1 The results to which the second 4 int32_t products will be added
		 */
		static OCEAN_FORCE_INLINE void multiplyInt8x16ToInt32x8AndAccumulate(const __m128i& values0, const __m128i& values1, __m128i& results0, __m128i& results1);

	private:

		/**
		 * Returns the interpolated pixel values for one 2 channel 16 bit pixel.
		 * @param pixel Upper left pixel in the frame
		 * @param size Size of one frame row in bytes
		 * @param fx_y_ Product of the inverse fx and the inverse fy interpolation factor
		 * @param fxy_ Product of the fx and the inverse fy interpolation factor
		 * @param fx_y Product of the inverse fx and the fy interpolation factor
		 * @param fxy Product of the fx and the fy interpolation factor
		 * @return Interpolated pixel values
		 */
		static inline unsigned int interpolation2Channel16Bit1x1(const uint8_t* const pixel, const unsigned int size, const unsigned int fx_y_, const unsigned int fxy_, const unsigned int fx_y, const unsigned int fxy);
};

inline void SSE::prefetchT0(const void* const data)
{
	_mm_prefetch((char*)data, _MM_HINT_T0);
}

inline void SSE::prefetchT1(const void* const data)
{
	_mm_prefetch((char*)data, _MM_HINT_T1);
}

inline void SSE::prefetchT2(const void* const data)
{
	_mm_prefetch((char*)data, _MM_HINT_T2);
}

inline void SSE::prefetchNTA(const void* const data)
{
	_mm_prefetch((char*)data, _MM_HINT_NTA);
}

template <unsigned int tIndex>
inline uint8_t SSE::value_u8(const __m128i& value)
{
	static_assert(tIndex <= 15u, "Invalid index!");

#ifdef OCEAN_COMPILER_MSC
	return value.m128i_u8[tIndex];
#else
	return ((const M128i*)(&value))->m128i_u8[tIndex];
#endif
}

inline uint8_t SSE::value_u8(const __m128i& value, const unsigned int index)
{
	ocean_assert(index <= 15u);

#ifdef OCEAN_COMPILER_MSC
	return value.m128i_u8[index];
#else
	return ((const M128i*)(&value))->m128i_u8[index];
#endif
}

template <unsigned int tIndex>
inline uint16_t SSE::value_u16(const __m128i& value)
{
	static_assert(tIndex <= 7u, "Invalid index!");

#ifdef OCEAN_COMPILER_MSC
	return value.m128i_u16[tIndex];
#else
	return ((const M128i*)(&value))->m128i_u16[tIndex];
#endif
}

template <unsigned int tIndex>
inline unsigned int SSE::value_u32(const __m128i& value)
{
	static_assert(tIndex <= 3u, "Invalid index!");

#ifdef OCEAN_COMPILER_MSC
	return value.m128i_u32[tIndex];
#else
	return ((const M128i*)(&value))->m128i_u32[tIndex];
#endif
}

OCEAN_FORCE_INLINE unsigned int SSE::sum_u32_4(const __m128i& value)
{
#ifdef OCEAN_COMPILER_MSC
	return value.m128i_u32[0] + value.m128i_u32[1] + value.m128i_u32[2] + value.m128i_u32[3];
#else
	return ((const M128i*)(&value))->m128i_u32[0] + ((const M128i*)(&value))->m128i_u32[1] + ((const M128i*)(&value))->m128i_u32[2] + ((const M128i*)(&value))->m128i_u32[3];
#endif
}

inline unsigned int SSE::sum_u32_first_2(const __m128i& value)
{
#ifdef OCEAN_COMPILER_MSC
	return value.m128i_u32[0] + value.m128i_u32[1];
#else
	return ((const M128i*)(&value))->m128i_u32[0] + ((const M128i*)(&value))->m128i_u32[1];
#endif
}

inline unsigned int SSE::sum_u32_first_third(const __m128i& value)
{
#ifdef OCEAN_COMPILER_MSC
	return value.m128i_u32[0] + value.m128i_u32[2];
#else
	return ((const M128i*)(&value))->m128i_u32[0] + ((const M128i*)(&value))->m128i_u32[2];
#endif
}

OCEAN_FORCE_INLINE float SSE::sum_f32_4(const __m128& value)
{
#ifdef OCEAN_COMPILER_MSC
	return value.m128_f32[0] + value.m128_f32[1] + value.m128_f32[2] + value.m128_f32[3];
#else
	return ((const M128*)(&value))->m128_f32[0] + ((const M128*)(&value))->m128_f32[1] + ((const M128*)(&value))->m128_f32[2] + ((const M128*)(&value))->m128_f32[3];
#endif
}

OCEAN_FORCE_INLINE double SSE::sum_f64_2(const __m128d& value)
{
#ifdef OCEAN_COMPILER_MSC
	return value.m128d_f64[0] + value.m128d_f64[1];
#else
	return ((const M128d*)(&value))->m128d_f64[0] + ((const M128d*)(&value))->m128d_f64[1];
#endif
}

inline __m128i SSE::sumSquareDifferences8BitBack11Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	return SSE::sumSquareDifference8Bit16Elements(_mm_srli_si128(SSE::load128i(image0), 5), _mm_srli_si128(SSE::load128i(image1), 5));
}

inline __m128i SSE::sumAbsoluteDifferences8BitBack11Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	return _mm_sad_epu8(_mm_srli_si128(SSE::load128i(image0), 5), _mm_srli_si128(SSE::load128i(image1), 5));
}

inline __m128i SSE::sumSquareDifference8BitFront12Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	const __m128i row0 = _mm_lddqu_si128((__m128i*)image0);
	const __m128i row1 = _mm_lddqu_si128((__m128i*)image1);

	// subtract the 16 elements (usage of saturation and bitwise or operator)
	const __m128i subtract = _mm_or_si128(_mm_subs_epu8(row0, row1), _mm_subs_epu8(row1, row0));

	// distribute the 16 elements of 8 bit values into 16 elements of 16 bit values (necessary for multiplication)

	const __m128i subtractLow = _mm_shuffle_epi8(subtract, set128i(0xA0A0A0A0A00AA008ull, 0xA006A004A002A000ull));
	const __m128i subtractHigh = _mm_shuffle_epi8(subtract, set128i(0xA0A0A0A0A00BA009ull, 0xA007A005A003A001ull));

	// square the 16 elements
	const __m128i squareLow = _mm_mullo_epi16(subtractLow, subtractLow);
	const __m128i squareHigh = _mm_mullo_epi16(subtractHigh, subtractHigh);

	// distribute the 16 elements of 16 bit values into 8 elements of 32 bit values (an itermediate add operation is used)
	const __m128i sumSquareLow = _mm_add_epi32(removeHighBits32_16(squareLow), removeHighBits32_16(squareHigh));
	const __m128i sumSquareHigh = _mm_add_epi32(moveHighBits32_16(squareLow), moveHighBits32_16(squareHigh));

	// 4 32 bit square difference values
	return _mm_add_epi32(sumSquareLow, sumSquareHigh);
}

inline __m128i SSE::sumSquareDifference8BitBack12Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	const __m128i row0 = _mm_lddqu_si128((__m128i*)image0);
	const __m128i row1 = _mm_lddqu_si128((__m128i*)image1);

	// subtract the 16 elements (usage of saturation and bitwise or operator)
	const __m128i subtract = _mm_or_si128(_mm_subs_epu8(row0, row1), _mm_subs_epu8(row1, row0));

	// distribute the 16 elements of 8 bit values into 16 elements of 16 bit values (necessary for multiplication)

	const __m128i subtractLow = _mm_shuffle_epi8(subtract, set128i(0xA0A0A0A0A00EA00Cull, 0xA00AA008A006A004ull));
	const __m128i subtractHigh = _mm_shuffle_epi8(subtract, set128i(0xA0A0A0A0A00FA00Dull, 0xA00BA009A007A005ull));

	// square the 16 elements
	const __m128i squareLow = _mm_mullo_epi16(subtractLow, subtractLow);
	const __m128i squareHigh = _mm_mullo_epi16(subtractHigh, subtractHigh);

	// distribute the 16 elements of 16 bit values into 8 elements of 32 bit values (an itermediate add operation is used)
	const __m128i sumSquareLow = _mm_add_epi32(removeHighBits32_16(squareLow), removeHighBits32_16(squareHigh));
	const __m128i sumSquareHigh = _mm_add_epi32(moveHighBits32_16(squareLow), moveHighBits32_16(squareHigh));

	// 4 32 bit square difference values
	return _mm_add_epi32(sumSquareLow, sumSquareHigh);
}

template <bool tBufferHas16Bytes>
inline __m128i SSE::sumSquareDifference8BitFront13Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	const __m128i row0 = load_u8_13_lower_random<tBufferHas16Bytes>(image0);
	const __m128i row1 = load_u8_13_lower_random<tBufferHas16Bytes>(image1);

	// subtract the 16 elements (usage of saturation and bitwise or operator)
	const __m128i subtract = _mm_or_si128(_mm_subs_epu8(row0, row1), _mm_subs_epu8(row1, row0));

	// distribute the 16 elements of 8 bit values into 16 elements of 16 bit values (necessary for multiplication)

	const __m128i subtractLow = _mm_shuffle_epi8(subtract, set128i(0xA0A0A00CA00AA008ull, 0xA006A004A002A000ull));
	const __m128i subtractHigh = _mm_shuffle_epi8(subtract, set128i(0xA0A0A0A0A00BA009ull, 0xA007A005A003A001ull));

	// square the 16 elements
	const __m128i squareLow = _mm_mullo_epi16(subtractLow, subtractLow);
	const __m128i squareHigh = _mm_mullo_epi16(subtractHigh, subtractHigh);

	// distribute the 16 elements of 16 bit values into 8 elements of 32 bit values (an itermediate add operation is used)
	const __m128i sumSquareLow = _mm_add_epi32(removeHighBits32_16(squareLow), removeHighBits32_16(squareHigh));
	const __m128i sumSquareHigh = _mm_add_epi32(moveHighBits32_16(squareLow), moveHighBits32_16(squareHigh));

	// 4 32 bit square difference values
	return _mm_add_epi32(sumSquareLow, sumSquareHigh);
}

inline __m128i SSE::sumSquareDifference8BitBack13Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	const __m128i row0 = _mm_lddqu_si128((__m128i*)image0);
	const __m128i row1 = _mm_lddqu_si128((__m128i*)image1);

	// subtract the 16 elements (usage of saturation and bitwise or operator)
	const __m128i subtract = _mm_or_si128(_mm_subs_epu8(row0, row1), _mm_subs_epu8(row1, row0));

	// distribute the 16 elements of 8 bit values into 16 elements of 16 bit values (necessary for multiplication)

	const __m128i subtractLow = _mm_shuffle_epi8(subtract, set128i(0xA0A0A00FA00DA00Bull, 0xA009A007A005A003ull));
	const __m128i subtractHigh = _mm_shuffle_epi8(subtract, set128i(0xA0A0A0A0A00EA00Cull, 0xA00AA008A006A004ull));

	// square the 16 elements
	const __m128i squareLow = _mm_mullo_epi16(subtractLow, subtractLow);
	const __m128i squareHigh = _mm_mullo_epi16(subtractHigh, subtractHigh);

	// distribute the 16 elements of 16 bit values into 8 elements of 32 bit values (an itermediate add operation is used)
	const __m128i sumSquareLow = _mm_add_epi32(removeHighBits32_16(squareLow), removeHighBits32_16(squareHigh));
	const __m128i sumSquareHigh = _mm_add_epi32(moveHighBits32_16(squareLow), moveHighBits32_16(squareHigh));

	// 4 32 bit square difference values
	return _mm_add_epi32(sumSquareLow, sumSquareHigh);
}

template <bool tBufferHas16Bytes>
inline __m128i SSE::sumSquareDifference8BitFront15Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	const __m128i row0 = load_u8_15_lower_random<tBufferHas16Bytes>(image0);
	const __m128i row1 = load_u8_15_lower_random<tBufferHas16Bytes>(image1);

	// subtract the 16 elements (usage of saturation and bitwise or operator)
	const __m128i subtract = _mm_or_si128(_mm_subs_epu8(row0, row1), _mm_subs_epu8(row1, row0));

	// distribute the 16 elements of 8 bit values into 16 elements of 16 bit values (necessary for multiplication)
	const __m128i subtractLow = removeHighBits16_8(subtract);
	const __m128i subtractHigh = moveHighBits16_8_7(subtract); // the highest high 8 bit are not used due to the only 15 elements

	// square the 16 elements
	const __m128i squareLow = _mm_mullo_epi16(subtractLow, subtractLow);
	const __m128i squareHigh = _mm_mullo_epi16(subtractHigh, subtractHigh);

	// distribute the 16 elements of 16 bit values into 8 elements of 32 bit values (an itermediate add operation is used)
	const __m128i sumSquareLow = _mm_add_epi32(removeHighBits32_16(squareLow), removeHighBits32_16(squareHigh));
	const __m128i sumSquareHigh = _mm_add_epi32(moveHighBits32_16(squareLow), moveHighBits32_16(squareHigh));

	// 4 32 bit square difference values
	return _mm_add_epi32(sumSquareLow, sumSquareHigh);
}

template <bool tBufferHas16Bytes>
inline __m128i SSE::sumAbsoluteDifferences8BitFront10Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	return _mm_sad_epu8(load_u8_10_upper_zero<tBufferHas16Bytes>(image0), load_u8_10_upper_zero<tBufferHas16Bytes>(image1));
}

template <bool tBufferHas16Bytes>
inline __m128i SSE::sumAbsoluteDifferences8BitFront15Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	return _mm_sad_epu8(load_u8_15_upper_zero<tBufferHas16Bytes>(image0), load_u8_15_upper_zero<tBufferHas16Bytes>(image1));
}

inline __m128i SSE::sumSquareDifference8Bit16Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	const __m128i row0 = _mm_lddqu_si128((__m128i*)image0);
	const __m128i row1 = _mm_lddqu_si128((__m128i*)image1);

	return sumSquareDifference8Bit16Elements(row0, row1);
}

inline __m128i SSE::sumAbsoluteDifferences8Bit16Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	return _mm_sad_epu8(SSE::load128i(image0), SSE::load128i(image1));
}

inline __m128i SSE::sumSquareDifference8Bit16ElementsAligned16(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);
	ocean_assert((unsigned long long)image0 % 16ll == 0ll);
	ocean_assert((unsigned long long)image1 % 16ll == 0ll);

	const __m128i row0 = _mm_load_si128((__m128i*)image0);
	const __m128i row1 = _mm_load_si128((__m128i*)image1);

	return sumSquareDifference8Bit16Elements(row0, row1);
}

inline __m128i SSE::sumSquareDifference8Bit16Elements(const __m128i& row0, const __m128i& row1)
{
	// subtract the 16 elements (usage of saturation and bitwise or operator)
	const __m128i subtract = _mm_or_si128(_mm_subs_epu8(row0, row1), _mm_subs_epu8(row1, row0));

	// distribute the 16 elements of 8 bit values into 16 elements of 16 bit values (necessary for multiplication)
	const __m128i subtractLow = removeHighBits16_8(subtract);
	const __m128i subtractHigh = moveHighBits16_8(subtract);

	// square the 16 elements
	const __m128i squareLow = _mm_mullo_epi16(subtractLow, subtractLow);
	const __m128i squareHigh = _mm_mullo_epi16(subtractHigh, subtractHigh);

	// distribute the 16 elements of 16 bit values into 8 elements of 32 bit values (an itermediate add operation is used)
	const __m128i sumSquareLow = _mm_add_epi32(removeHighBits32_16(squareLow), removeHighBits32_16(squareHigh));
	const __m128i sumSquareHigh = _mm_add_epi32(moveHighBits32_16(squareLow), moveHighBits32_16(squareHigh));

	// 4 32 bit square difference values
	return _mm_add_epi32(sumSquareLow, sumSquareHigh);
}

inline __m128i SSE::interpolation1Channel8Bit8Elements(const __m128i& values0, const __m128i& values1, const __m128i& fx_fy_, const __m128i& fxfy_, const __m128i& fx_fy, const __m128i& fxfy)
{
	//           F   E     D   C     B   A     9   8     7   6     5   4     3   2     1   0
	// values0: aF  yE  | yD  yC  | yB  yA  | y9  y8  | y7  y6  | y5  y4  | y3  y2  | y1  y0
	// values1: aF' yE' | yD' yC' | yB' yA' | y9' y8' | y7' y6' | y5' y4' | y3' y2' | y1' y0'

	// shuffled elements
	// row0: y7  y6  y5  y4  y3  y2  y1  y0   |  * fx_ * fy_
	// row1: y8  y7  y6  y5  y4  y3  y2  y1   |  * fx  * fy_
	// row2: y7' y6' y5' y4' y3' y2' y1' y0'  |  * fx_ * fy
	// row3: y8' y7' y6' y5' y4' y3' y2' y1'  |  * fx  * fy

#ifdef OCEAN_COMPILER_MSC

	ocean_assert(fx_fy_.m128i_u16[0] == fx_fy_.m128i_u16[1]);
	ocean_assert(fx_fy_.m128i_u16[1] == fx_fy_.m128i_u16[2]);
	ocean_assert(fx_fy_.m128i_u16[2] == fx_fy_.m128i_u16[3]);
	ocean_assert(fx_fy_.m128i_u16[3] == fx_fy_.m128i_u16[4]);
	ocean_assert(fx_fy_.m128i_u16[4] == fx_fy_.m128i_u16[5]);
	ocean_assert(fx_fy_.m128i_u16[5] == fx_fy_.m128i_u16[6]);
	ocean_assert(fx_fy_.m128i_u16[6] == fx_fy_.m128i_u16[7]);

	ocean_assert(fxfy_.m128i_u16[0] == fxfy_.m128i_u16[1]);
	ocean_assert(fxfy_.m128i_u16[1] == fxfy_.m128i_u16[2]);
	ocean_assert(fxfy_.m128i_u16[2] == fxfy_.m128i_u16[3]);
	ocean_assert(fxfy_.m128i_u16[3] == fxfy_.m128i_u16[4]);
	ocean_assert(fxfy_.m128i_u16[4] == fxfy_.m128i_u16[5]);
	ocean_assert(fxfy_.m128i_u16[5] == fxfy_.m128i_u16[6]);
	ocean_assert(fxfy_.m128i_u16[6] == fxfy_.m128i_u16[7]);

	ocean_assert(fx_fy.m128i_u16[0] == fx_fy.m128i_u16[1]);
	ocean_assert(fx_fy.m128i_u16[1] == fx_fy.m128i_u16[2]);
	ocean_assert(fx_fy.m128i_u16[2] == fx_fy.m128i_u16[3]);
	ocean_assert(fx_fy.m128i_u16[3] == fx_fy.m128i_u16[4]);
	ocean_assert(fx_fy.m128i_u16[4] == fx_fy.m128i_u16[5]);
	ocean_assert(fx_fy.m128i_u16[5] == fx_fy.m128i_u16[6]);
	ocean_assert(fx_fy.m128i_u16[6] == fx_fy.m128i_u16[7]);

	ocean_assert(fxfy.m128i_u16[0] == fxfy.m128i_u16[1]);
	ocean_assert(fxfy.m128i_u16[1] == fxfy.m128i_u16[2]);
	ocean_assert(fxfy.m128i_u16[2] == fxfy.m128i_u16[3]);
	ocean_assert(fxfy.m128i_u16[3] == fxfy.m128i_u16[4]);
	ocean_assert(fxfy.m128i_u16[4] == fxfy.m128i_u16[5]);
	ocean_assert(fxfy.m128i_u16[5] == fxfy.m128i_u16[6]);
	ocean_assert(fxfy.m128i_u16[6] == fxfy.m128i_u16[7]);

	ocean_assert(fx_fy_.m128i_u16[0] + fxfy_.m128i_u16[0] + fx_fy.m128i_u16[0] + fxfy.m128i_u16[0] == 128u * 128u);

#else

#ifdef OCEAN_DEBUG

	const M128i& debug_fx_fy_ = *(const M128i*)(&fx_fy_);
	const M128i& debug_fx_fy = *(const M128i*)(&fx_fy);
	const M128i& debug_fxfy_ = *(const M128i*)(&fxfy_);
	const M128i& debug_fxfy = *(const M128i*)(&fxfy);

#endif // OCEAN_DEBUG

	ocean_assert(debug_fx_fy_.m128i_u16[0] == debug_fx_fy_.m128i_u16[1]);
	ocean_assert(debug_fx_fy_.m128i_u16[1] == debug_fx_fy_.m128i_u16[2]);
	ocean_assert(debug_fx_fy_.m128i_u16[2] == debug_fx_fy_.m128i_u16[3]);
	ocean_assert(debug_fx_fy_.m128i_u16[3] == debug_fx_fy_.m128i_u16[4]);
	ocean_assert(debug_fx_fy_.m128i_u16[4] == debug_fx_fy_.m128i_u16[5]);
	ocean_assert(debug_fx_fy_.m128i_u16[5] == debug_fx_fy_.m128i_u16[6]);
	ocean_assert(debug_fx_fy_.m128i_u16[6] == debug_fx_fy_.m128i_u16[7]);

	ocean_assert(debug_fxfy_.m128i_u16[0] == debug_fxfy_.m128i_u16[1]);
	ocean_assert(debug_fxfy_.m128i_u16[1] == debug_fxfy_.m128i_u16[2]);
	ocean_assert(debug_fxfy_.m128i_u16[2] == debug_fxfy_.m128i_u16[3]);
	ocean_assert(debug_fxfy_.m128i_u16[3] == debug_fxfy_.m128i_u16[4]);
	ocean_assert(debug_fxfy_.m128i_u16[4] == debug_fxfy_.m128i_u16[5]);
	ocean_assert(debug_fxfy_.m128i_u16[5] == debug_fxfy_.m128i_u16[6]);
	ocean_assert(debug_fxfy_.m128i_u16[6] == debug_fxfy_.m128i_u16[7]);

	ocean_assert(debug_fx_fy.m128i_u16[0] == debug_fx_fy.m128i_u16[1]);
	ocean_assert(debug_fx_fy.m128i_u16[1] == debug_fx_fy.m128i_u16[2]);
	ocean_assert(debug_fx_fy.m128i_u16[2] == debug_fx_fy.m128i_u16[3]);
	ocean_assert(debug_fx_fy.m128i_u16[3] == debug_fx_fy.m128i_u16[4]);
	ocean_assert(debug_fx_fy.m128i_u16[4] == debug_fx_fy.m128i_u16[5]);
	ocean_assert(debug_fx_fy.m128i_u16[5] == debug_fx_fy.m128i_u16[6]);
	ocean_assert(debug_fx_fy.m128i_u16[6] == debug_fx_fy.m128i_u16[7]);

	ocean_assert(debug_fxfy.m128i_u16[0] == debug_fxfy.m128i_u16[1]);
	ocean_assert(debug_fxfy.m128i_u16[1] == debug_fxfy.m128i_u16[2]);
	ocean_assert(debug_fxfy.m128i_u16[2] == debug_fxfy.m128i_u16[3]);
	ocean_assert(debug_fxfy.m128i_u16[3] == debug_fxfy.m128i_u16[4]);
	ocean_assert(debug_fxfy.m128i_u16[4] == debug_fxfy.m128i_u16[5]);
	ocean_assert(debug_fxfy.m128i_u16[5] == debug_fxfy.m128i_u16[6]);
	ocean_assert(debug_fxfy.m128i_u16[6] == debug_fxfy.m128i_u16[7]);

	ocean_assert(debug_fx_fy_.m128i_u16[0] + debug_fxfy_.m128i_u16[0] + debug_fx_fy.m128i_u16[0] + debug_fxfy.m128i_u16[0] == 128u * 128u);

#endif

	__m128i shuffle = set128i(0xA007A006A005A004ull, 0xA003A002A001A000ull);

	// row0
	__m128i row = _mm_shuffle_epi8(values0, shuffle);

	__m128i multiLow = _mm_mullo_epi16(row, fx_fy_);
	__m128i multiHigh = _mm_mulhi_epu16(row, fx_fy_);

	__m128i resultEven = _mm_blend_epi16(multiLow, _mm_slli_si128(multiHigh, 2), 0xAA); // 0xAA = 1010 1010
	__m128i resultOdd = _mm_blend_epi16(_mm_srli_si128(multiLow, 2), multiHigh, 0xAA);

	// row2
	row = _mm_shuffle_epi8(values1, shuffle);

	multiLow = _mm_mullo_epi16(row, fx_fy);
	multiHigh = _mm_mulhi_epu16(row, fx_fy);

	resultEven = _mm_add_epi32(resultEven, _mm_blend_epi16(multiLow, _mm_slli_si128(multiHigh, 2), 0xAA));
	resultOdd = _mm_add_epi32(resultOdd, _mm_blend_epi16(_mm_srli_si128(multiLow, 2), multiHigh, 0xAA));



	shuffle = set128i(0xA008A007A006A005ull, 0xA004A003A002A001ull);

	// row1
	row = _mm_shuffle_epi8(values0, shuffle);

	multiLow = _mm_mullo_epi16(row, fxfy_);
	multiHigh = _mm_mulhi_epu16(row, fxfy_);

	resultEven = _mm_add_epi32(resultEven, _mm_blend_epi16(multiLow, _mm_slli_si128(multiHigh, 2), 0xAA));
	resultOdd = _mm_add_epi32(resultOdd, _mm_blend_epi16(_mm_srli_si128(multiLow, 2), multiHigh, 0xAA));


	// row4
	row = _mm_shuffle_epi8(values1, shuffle);

	multiLow = _mm_mullo_epi16(row, fxfy);
	multiHigh = _mm_mulhi_epu16(row, fxfy);

	resultEven = _mm_add_epi32(resultEven, _mm_blend_epi16(multiLow, _mm_slli_si128(multiHigh, 2), 0xAA));
	resultOdd = _mm_add_epi32(resultOdd, _mm_blend_epi16(_mm_srli_si128(multiLow, 2), multiHigh, 0xAA));


	// normalization ( + 128 * 128 / 2) / (128 * 128)
	resultEven = _mm_add_epi32(resultEven, _mm_set1_epi32(8192));
	resultEven = _mm_srli_epi32(resultEven, 14);

	resultOdd = _mm_add_epi32(resultOdd, _mm_set1_epi32(8192));
	resultOdd = _mm_srli_epi32(resultOdd, 14);

	// stack the 2 four 32 bit values together to eight 8 bit values
	return moveLowBits32_16ToLow64(_mm_or_si128(resultEven, _mm_slli_si128(resultOdd, 1)));
}

inline __m128i SSE::interpolation2Channel16Bit8Elements(const __m128i& values0, const __m128i& values1, const __m128i& fx_fy_, const __m128i& fxfy_, const __m128i& fx_fy, const __m128i& fxfy)
{
	//           F   E     D   C     B   A     9   8     7   6     5   4     3   2     1   0
	// values0: a7  y7  | a6  y6  | a5  y5  | a4  y4  | a3  y3  | a2  y2  | a1  y1  | a0  y0
	// values1: a7' y7' | a6' y6' | a5' y5' | a4' y4' | a3' y3' | a2' y2' | a1' y1' | a0' y0'

	// shuffled elements
	// row0: a3  y3  a2  y2  a1  y1  a0  y0   |  * fx_ * fy_
	// row1: a4  y4  a3  y3  a2  y2  a1  y1   |  * fx  * fy_
	// row2: a3' y3' a2' y2' a1' y1' a0' y0'  |  * fx_ * fy
	// row3: a4' y4' a3' y3' a2' y2' a1' y1'  |  * fx  * fy

#ifdef OCEAN_COMPILER_MSC

	ocean_assert(fx_fy_.m128i_u16[0] == fx_fy_.m128i_u16[1]);
	ocean_assert(fx_fy_.m128i_u16[1] == fx_fy_.m128i_u16[2]);
	ocean_assert(fx_fy_.m128i_u16[2] == fx_fy_.m128i_u16[3]);
	ocean_assert(fx_fy_.m128i_u16[3] == fx_fy_.m128i_u16[4]);
	ocean_assert(fx_fy_.m128i_u16[4] == fx_fy_.m128i_u16[5]);
	ocean_assert(fx_fy_.m128i_u16[5] == fx_fy_.m128i_u16[6]);
	ocean_assert(fx_fy_.m128i_u16[6] == fx_fy_.m128i_u16[7]);

	ocean_assert(fxfy_.m128i_u16[0] == fxfy_.m128i_u16[1]);
	ocean_assert(fxfy_.m128i_u16[1] == fxfy_.m128i_u16[2]);
	ocean_assert(fxfy_.m128i_u16[2] == fxfy_.m128i_u16[3]);
	ocean_assert(fxfy_.m128i_u16[3] == fxfy_.m128i_u16[4]);
	ocean_assert(fxfy_.m128i_u16[4] == fxfy_.m128i_u16[5]);
	ocean_assert(fxfy_.m128i_u16[5] == fxfy_.m128i_u16[6]);
	ocean_assert(fxfy_.m128i_u16[6] == fxfy_.m128i_u16[7]);

	ocean_assert(fx_fy.m128i_u16[0] == fx_fy.m128i_u16[1]);
	ocean_assert(fx_fy.m128i_u16[1] == fx_fy.m128i_u16[2]);
	ocean_assert(fx_fy.m128i_u16[2] == fx_fy.m128i_u16[3]);
	ocean_assert(fx_fy.m128i_u16[3] == fx_fy.m128i_u16[4]);
	ocean_assert(fx_fy.m128i_u16[4] == fx_fy.m128i_u16[5]);
	ocean_assert(fx_fy.m128i_u16[5] == fx_fy.m128i_u16[6]);
	ocean_assert(fx_fy.m128i_u16[6] == fx_fy.m128i_u16[7]);

	ocean_assert(fxfy.m128i_u16[0] == fxfy.m128i_u16[1]);
	ocean_assert(fxfy.m128i_u16[1] == fxfy.m128i_u16[2]);
	ocean_assert(fxfy.m128i_u16[2] == fxfy.m128i_u16[3]);
	ocean_assert(fxfy.m128i_u16[3] == fxfy.m128i_u16[4]);
	ocean_assert(fxfy.m128i_u16[4] == fxfy.m128i_u16[5]);
	ocean_assert(fxfy.m128i_u16[5] == fxfy.m128i_u16[6]);
	ocean_assert(fxfy.m128i_u16[6] == fxfy.m128i_u16[7]);

#else

#ifdef OCEAN_DEBUG

	const M128i& debug_fx_fy_ = *(const M128i*)(&fx_fy_);
	const M128i& debug_fx_fy = *(const M128i*)(&fx_fy);
	const M128i& debug_fxfy_ = *(const M128i*)(&fxfy_);
	const M128i& debug_fxfy = *(const M128i*)(&fxfy);

#endif // OCEAN_DEBUG

	ocean_assert(debug_fx_fy_.m128i_u16[0] == debug_fx_fy_.m128i_u16[1]);
	ocean_assert(debug_fx_fy_.m128i_u16[1] == debug_fx_fy_.m128i_u16[2]);
	ocean_assert(debug_fx_fy_.m128i_u16[2] == debug_fx_fy_.m128i_u16[3]);
	ocean_assert(debug_fx_fy_.m128i_u16[3] == debug_fx_fy_.m128i_u16[4]);
	ocean_assert(debug_fx_fy_.m128i_u16[4] == debug_fx_fy_.m128i_u16[5]);
	ocean_assert(debug_fx_fy_.m128i_u16[5] == debug_fx_fy_.m128i_u16[6]);
	ocean_assert(debug_fx_fy_.m128i_u16[6] == debug_fx_fy_.m128i_u16[7]);

	ocean_assert(debug_fxfy_.m128i_u16[0] == debug_fxfy_.m128i_u16[1]);
	ocean_assert(debug_fxfy_.m128i_u16[1] == debug_fxfy_.m128i_u16[2]);
	ocean_assert(debug_fxfy_.m128i_u16[2] == debug_fxfy_.m128i_u16[3]);
	ocean_assert(debug_fxfy_.m128i_u16[3] == debug_fxfy_.m128i_u16[4]);
	ocean_assert(debug_fxfy_.m128i_u16[4] == debug_fxfy_.m128i_u16[5]);
	ocean_assert(debug_fxfy_.m128i_u16[5] == debug_fxfy_.m128i_u16[6]);
	ocean_assert(debug_fxfy_.m128i_u16[6] == debug_fxfy_.m128i_u16[7]);

	ocean_assert(debug_fx_fy.m128i_u16[0] == debug_fx_fy.m128i_u16[1]);
	ocean_assert(debug_fx_fy.m128i_u16[1] == debug_fx_fy.m128i_u16[2]);
	ocean_assert(debug_fx_fy.m128i_u16[2] == debug_fx_fy.m128i_u16[3]);
	ocean_assert(debug_fx_fy.m128i_u16[3] == debug_fx_fy.m128i_u16[4]);
	ocean_assert(debug_fx_fy.m128i_u16[4] == debug_fx_fy.m128i_u16[5]);
	ocean_assert(debug_fx_fy.m128i_u16[5] == debug_fx_fy.m128i_u16[6]);
	ocean_assert(debug_fx_fy.m128i_u16[6] == debug_fx_fy.m128i_u16[7]);

	ocean_assert(debug_fxfy.m128i_u16[0] == debug_fxfy.m128i_u16[1]);
	ocean_assert(debug_fxfy.m128i_u16[1] == debug_fxfy.m128i_u16[2]);
	ocean_assert(debug_fxfy.m128i_u16[2] == debug_fxfy.m128i_u16[3]);
	ocean_assert(debug_fxfy.m128i_u16[3] == debug_fxfy.m128i_u16[4]);
	ocean_assert(debug_fxfy.m128i_u16[4] == debug_fxfy.m128i_u16[5]);
	ocean_assert(debug_fxfy.m128i_u16[5] == debug_fxfy.m128i_u16[6]);
	ocean_assert(debug_fxfy.m128i_u16[6] == debug_fxfy.m128i_u16[7]);

#endif

	__m128i shuffle = set128i(0xA007A006A005A004ull, 0xA003A002A001A000ull);

	// row0
	__m128i row = _mm_shuffle_epi8(values0, shuffle);

	__m128i multiLow = _mm_mullo_epi16(row, fx_fy_);
	__m128i multiHigh = _mm_mulhi_epu16(row, fx_fy_);

	__m128i resultEven = _mm_blend_epi16(multiLow, _mm_slli_si128(multiHigh, 2), 0xAA); // 0xAA = 1010 1010
	__m128i resultOdd = _mm_blend_epi16(_mm_srli_si128(multiLow, 2), multiHigh, 0xAA);

	// row2
	row = _mm_shuffle_epi8(values1, shuffle);

	multiLow = _mm_mullo_epi16(row, fx_fy);
	multiHigh = _mm_mulhi_epu16(row, fx_fy);

	resultEven = _mm_add_epi32(resultEven, _mm_blend_epi16(multiLow, _mm_slli_si128(multiHigh, 2), 0xAA));
	resultOdd = _mm_add_epi32(resultOdd, _mm_blend_epi16(_mm_srli_si128(multiLow, 2), multiHigh, 0xAA));



	shuffle = set128i(0xA009A008A007A006ull, 0xA005A004A003A002ull);

	// row1
	row = _mm_shuffle_epi8(values0, shuffle);

	multiLow = _mm_mullo_epi16(row, fxfy_);
	multiHigh = _mm_mulhi_epu16(row, fxfy_);

	resultEven = _mm_add_epi32(resultEven, _mm_blend_epi16(multiLow, _mm_slli_si128(multiHigh, 2), 0xAA));
	resultOdd = _mm_add_epi32(resultOdd, _mm_blend_epi16(_mm_srli_si128(multiLow, 2), multiHigh, 0xAA));


	// row4
	row = _mm_shuffle_epi8(values1, shuffle);

	multiLow = _mm_mullo_epi16(row, fxfy);
	multiHigh = _mm_mulhi_epu16(row, fxfy);

	resultEven = _mm_add_epi32(resultEven, _mm_blend_epi16(multiLow, _mm_slli_si128(multiHigh, 2), 0xAA));
	resultOdd = _mm_add_epi32(resultOdd, _mm_blend_epi16(_mm_srli_si128(multiLow, 2), multiHigh, 0xAA));


	// normalization ( + 128 * 128 / 2) / (128 * 128)
	resultEven = _mm_add_epi32(resultEven, _mm_set1_epi32(8192));
	resultEven = _mm_srli_epi32(resultEven, 14);

	resultOdd = _mm_add_epi32(resultOdd, _mm_set1_epi32(8192));
	resultOdd = _mm_srli_epi32(resultOdd, 14);

	// stack the 2 four 32 bit values together to eight 8 bit values
	return moveLowBits32_16ToLow64(_mm_or_si128(resultEven, _mm_slli_si128(resultOdd, 1)));
}

inline __m128i SSE::interpolation3Channel24Bit8Elements(const __m128i& values0, const __m128i& values1, const __m128i& fx_fy_, const __m128i& fxfy_, const __m128i& fx_fy, const __m128i& fxfy)
{
	//           F    E   D   C    B   A   9    8   7   6    5   4   3    2   1   0
	// values0: r5 | b4  g4  r4 | b3  g3  r3 | b2  g2  r2 | b1  g1  r1 | b0  g0  r0
	// values1: r5'| b4' g4' r4'| b3' g3' r3'| b2' g2' r2'| b1' g1' r1'| b0' g0' r0'

	// shuffled elements
	// row0: g2  r2  b1  g1  r1  b0  g0  r0   |  * fx_ * fy_
	// row1: g3  r3  b2  g2  r2  b1  g1  r1   |  * fx  * fy_
	// row2: g2' r2' b1' g1' r1' b0' g0' r0'  |  * fx_ * fy
	// row3: g3' r3' b2' g2' r2' b1' g1' r1'  |  * fx  * fy

#ifdef OCEAN_COMPILER_MSC

	ocean_assert(fx_fy_.m128i_u16[0] == fx_fy_.m128i_u16[1]);
	ocean_assert(fx_fy_.m128i_u16[1] == fx_fy_.m128i_u16[2]);
	ocean_assert(fx_fy_.m128i_u16[2] == fx_fy_.m128i_u16[3]);
	ocean_assert(fx_fy_.m128i_u16[3] == fx_fy_.m128i_u16[4]);
	ocean_assert(fx_fy_.m128i_u16[4] == fx_fy_.m128i_u16[5]);
	ocean_assert(fx_fy_.m128i_u16[5] == fx_fy_.m128i_u16[6]);
	ocean_assert(fx_fy_.m128i_u16[6] == fx_fy_.m128i_u16[7]);

	ocean_assert(fxfy_.m128i_u16[0] == fxfy_.m128i_u16[1]);
	ocean_assert(fxfy_.m128i_u16[1] == fxfy_.m128i_u16[2]);
	ocean_assert(fxfy_.m128i_u16[2] == fxfy_.m128i_u16[3]);
	ocean_assert(fxfy_.m128i_u16[3] == fxfy_.m128i_u16[4]);
	ocean_assert(fxfy_.m128i_u16[4] == fxfy_.m128i_u16[5]);
	ocean_assert(fxfy_.m128i_u16[5] == fxfy_.m128i_u16[6]);
	ocean_assert(fxfy_.m128i_u16[6] == fxfy_.m128i_u16[7]);

	ocean_assert(fx_fy.m128i_u16[0] == fx_fy.m128i_u16[1]);
	ocean_assert(fx_fy.m128i_u16[1] == fx_fy.m128i_u16[2]);
	ocean_assert(fx_fy.m128i_u16[2] == fx_fy.m128i_u16[3]);
	ocean_assert(fx_fy.m128i_u16[3] == fx_fy.m128i_u16[4]);
	ocean_assert(fx_fy.m128i_u16[4] == fx_fy.m128i_u16[5]);
	ocean_assert(fx_fy.m128i_u16[5] == fx_fy.m128i_u16[6]);
	ocean_assert(fx_fy.m128i_u16[6] == fx_fy.m128i_u16[7]);

	ocean_assert(fxfy.m128i_u16[0] == fxfy.m128i_u16[1]);
	ocean_assert(fxfy.m128i_u16[1] == fxfy.m128i_u16[2]);
	ocean_assert(fxfy.m128i_u16[2] == fxfy.m128i_u16[3]);
	ocean_assert(fxfy.m128i_u16[3] == fxfy.m128i_u16[4]);
	ocean_assert(fxfy.m128i_u16[4] == fxfy.m128i_u16[5]);
	ocean_assert(fxfy.m128i_u16[5] == fxfy.m128i_u16[6]);
	ocean_assert(fxfy.m128i_u16[6] == fxfy.m128i_u16[7]);

#else

#ifdef OCEAN_DEBUG

	const M128i& debug_fx_fy_ = *(const M128i*)(&fx_fy_);
	const M128i& debug_fx_fy = *(const M128i*)(&fx_fy);
	const M128i& debug_fxfy_ = *(const M128i*)(&fxfy_);
	const M128i& debug_fxfy = *(const M128i*)(&fxfy);

#endif // OCEAN_DEBUG

	ocean_assert(debug_fx_fy_.m128i_u16[0] == debug_fx_fy_.m128i_u16[1]);
	ocean_assert(debug_fx_fy_.m128i_u16[1] == debug_fx_fy_.m128i_u16[2]);
	ocean_assert(debug_fx_fy_.m128i_u16[2] == debug_fx_fy_.m128i_u16[3]);
	ocean_assert(debug_fx_fy_.m128i_u16[3] == debug_fx_fy_.m128i_u16[4]);
	ocean_assert(debug_fx_fy_.m128i_u16[4] == debug_fx_fy_.m128i_u16[5]);
	ocean_assert(debug_fx_fy_.m128i_u16[5] == debug_fx_fy_.m128i_u16[6]);
	ocean_assert(debug_fx_fy_.m128i_u16[6] == debug_fx_fy_.m128i_u16[7]);

	ocean_assert(debug_fxfy_.m128i_u16[0] == debug_fxfy_.m128i_u16[1]);
	ocean_assert(debug_fxfy_.m128i_u16[1] == debug_fxfy_.m128i_u16[2]);
	ocean_assert(debug_fxfy_.m128i_u16[2] == debug_fxfy_.m128i_u16[3]);
	ocean_assert(debug_fxfy_.m128i_u16[3] == debug_fxfy_.m128i_u16[4]);
	ocean_assert(debug_fxfy_.m128i_u16[4] == debug_fxfy_.m128i_u16[5]);
	ocean_assert(debug_fxfy_.m128i_u16[5] == debug_fxfy_.m128i_u16[6]);
	ocean_assert(debug_fxfy_.m128i_u16[6] == debug_fxfy_.m128i_u16[7]);

	ocean_assert(debug_fx_fy.m128i_u16[0] == debug_fx_fy.m128i_u16[1]);
	ocean_assert(debug_fx_fy.m128i_u16[1] == debug_fx_fy.m128i_u16[2]);
	ocean_assert(debug_fx_fy.m128i_u16[2] == debug_fx_fy.m128i_u16[3]);
	ocean_assert(debug_fx_fy.m128i_u16[3] == debug_fx_fy.m128i_u16[4]);
	ocean_assert(debug_fx_fy.m128i_u16[4] == debug_fx_fy.m128i_u16[5]);
	ocean_assert(debug_fx_fy.m128i_u16[5] == debug_fx_fy.m128i_u16[6]);
	ocean_assert(debug_fx_fy.m128i_u16[6] == debug_fx_fy.m128i_u16[7]);

	ocean_assert(debug_fxfy.m128i_u16[0] == debug_fxfy.m128i_u16[1]);
	ocean_assert(debug_fxfy.m128i_u16[1] == debug_fxfy.m128i_u16[2]);
	ocean_assert(debug_fxfy.m128i_u16[2] == debug_fxfy.m128i_u16[3]);
	ocean_assert(debug_fxfy.m128i_u16[3] == debug_fxfy.m128i_u16[4]);
	ocean_assert(debug_fxfy.m128i_u16[4] == debug_fxfy.m128i_u16[5]);
	ocean_assert(debug_fxfy.m128i_u16[5] == debug_fxfy.m128i_u16[6]);
	ocean_assert(debug_fxfy.m128i_u16[6] == debug_fxfy.m128i_u16[7]);

#endif

	__m128i shuffle = set128i(0xA007A006A005A004ull, 0xA003A002A001A000ull);

	// row0
	__m128i row = _mm_shuffle_epi8(values0, shuffle);

	__m128i multiLow = _mm_mullo_epi16(row, fx_fy_);
	__m128i multiHigh = _mm_mulhi_epu16(row, fx_fy_);

	__m128i resultEven = _mm_blend_epi16(multiLow, _mm_slli_si128(multiHigh, 2), 0xAA); // 0xAA = 1010 1010
	__m128i resultOdd = _mm_blend_epi16(_mm_srli_si128(multiLow, 2), multiHigh, 0xAA);

	// row2
	row = _mm_shuffle_epi8(values1, shuffle);

	multiLow = _mm_mullo_epi16(row, fx_fy);
	multiHigh = _mm_mulhi_epu16(row, fx_fy);

	resultEven = _mm_add_epi32(resultEven, _mm_blend_epi16(multiLow, _mm_slli_si128(multiHigh, 2), 0xAA));
	resultOdd = _mm_add_epi32(resultOdd, _mm_blend_epi16(_mm_srli_si128(multiLow, 2), multiHigh, 0xAA));



	shuffle = set128i(0xA00AA009A008A007ull, 0xA006A005A004A003ull);

	// row1
	row = _mm_shuffle_epi8(values0, shuffle);

	multiLow = _mm_mullo_epi16(row, fxfy_);
	multiHigh = _mm_mulhi_epu16(row, fxfy_);

	resultEven = _mm_add_epi32(resultEven, _mm_blend_epi16(multiLow, _mm_slli_si128(multiHigh, 2), 0xAA));
	resultOdd = _mm_add_epi32(resultOdd, _mm_blend_epi16(_mm_srli_si128(multiLow, 2), multiHigh, 0xAA));


	// row4
	row = _mm_shuffle_epi8(values1, shuffle);

	multiLow = _mm_mullo_epi16(row, fxfy);
	multiHigh = _mm_mulhi_epu16(row, fxfy);

	resultEven = _mm_add_epi32(resultEven, _mm_blend_epi16(multiLow, _mm_slli_si128(multiHigh, 2), 0xAA));
	resultOdd = _mm_add_epi32(resultOdd, _mm_blend_epi16(_mm_srli_si128(multiLow, 2), multiHigh, 0xAA));


	// normalization ( + 128 * 128 / 2) / (128 * 128)
	resultEven = _mm_add_epi32(resultEven, _mm_set1_epi32(8192));
	resultEven = _mm_srli_epi32(resultEven, 14);

	resultOdd = _mm_add_epi32(resultOdd, _mm_set1_epi32(8192));
	resultOdd = _mm_srli_epi32(resultOdd, 14);

	// stack the 2 four 32 bit values together to eight 8 bit values
	return moveLowBits32_16ToLow64(_mm_or_si128(resultEven, _mm_slli_si128(resultOdd, 1)));
}

inline __m128i SSE::interpolation1Channel8Bit15Elements(const __m128i& values0, const __m128i& values1, const __m128i& fx_fy_fxfy_, const __m128i& fx_fyfxfy)
{
	__m128i row0_a = _mm_shuffle_epi8(values0, set128i(0xFF04FF03FF03FF02ull, 0xFF02FF01FF01FF00ull));
	__m128i row1_a = _mm_shuffle_epi8(values1, set128i(0xFF04FF03FF03FF02ull, 0xFF02FF01FF01FF00ull));

	__m128i row0_b = _mm_shuffle_epi8(values0, set128i(0xFF08FF07FF07FF06ull, 0xFF06FF05FF05FF04ull));
	__m128i row1_b = _mm_shuffle_epi8(values1, set128i(0xFF08FF07FF07FF06ull, 0xFF06FF05FF05FF04ull));

	__m128i row0_c = _mm_shuffle_epi8(values0, set128i(0xFF0cFF0bFF0bFF0aull, 0xFF0aFF09FF09FF08ull));
	__m128i row1_c = _mm_shuffle_epi8(values1, set128i(0xFF0cFF0bFF0bFF0aull, 0xFF0aFF09FF09FF08ull));

	__m128i row0_d = _mm_shuffle_epi8(values0, set128i(0xFFFFFFFFFF0fFF0eull, 0xFF0eFF0dFF0dFF0cull));
	__m128i row1_d = _mm_shuffle_epi8(values1, set128i(0xFFFFFFFFFF0fFF0eull, 0xFF0eFF0dFF0dFF0cull));

	row0_a = _mm_madd_epi16(row0_a, fx_fy_fxfy_);
	row0_b = _mm_madd_epi16(row0_b, fx_fy_fxfy_);
	row0_c = _mm_madd_epi16(row0_c, fx_fy_fxfy_);
	row0_d = _mm_madd_epi16(row0_d, fx_fy_fxfy_);

	row1_a = _mm_madd_epi16(row1_a, fx_fyfxfy);
	row1_b = _mm_madd_epi16(row1_b, fx_fyfxfy);
	row1_c = _mm_madd_epi16(row1_c, fx_fyfxfy);
	row1_d = _mm_madd_epi16(row1_d, fx_fyfxfy);

	const __m128i rounding = _mm_set1_epi32(8192);

	__m128i row_a = _mm_add_epi32(row0_a, row1_a);
	__m128i row_b = _mm_add_epi32(row0_b, row1_b);
	__m128i row_c = _mm_add_epi32(row0_c, row1_c);
	__m128i row_d = _mm_add_epi32(row0_d, row1_d);

	row_a = _mm_add_epi32(row_a, rounding);
	row_b = _mm_add_epi32(row_b, rounding);
	row_c = _mm_add_epi32(row_c, rounding);
	row_d = _mm_add_epi32(row_d, rounding);

	row_a = _mm_srli_epi32(row_a, 14);
	row_b = _mm_srli_epi32(row_b, 14);
	row_c = _mm_srli_epi32(row_c, 14);
	row_d = _mm_srli_epi32(row_d, 14);

	row_a = _mm_shuffle_epi8(row_a, set128i(0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFF0c080400ull));
	row_b = _mm_shuffle_epi8(row_b, set128i(0xFFFFFFFFFFFFFFFFull, 0x0c080400FFFFFFFFull));
	row_c = _mm_shuffle_epi8(row_c, set128i(0xFFFFFFFF0c080400ull, 0xFFFFFFFFFFFFFFFFull));
	row_d = _mm_shuffle_epi8(row_d, set128i(0xFF080400FFFFFFFFull, 0xFFFFFFFFFFFFFFFFull));

	row_a = _mm_or_si128(row_a, row_b);
	row_c = _mm_or_si128(row_c, row_d);

	return _mm_or_si128(row_a, row_c);
}

inline __m128i SSE::interpolation3Channel24Bit12Elements(const __m128i& values0, const __m128i& values1, const __m128i& fx_fy_fxfy_, const __m128i& fx_fyfxfy)
{
	__m128i row0_a = _mm_shuffle_epi8(values0, set128i(0xFF0cFF09FF09FF06ull, 0xFF06FF03FF03FF00ull));
	__m128i row1_a = _mm_shuffle_epi8(values1, set128i(0xFF0cFF09FF09FF06ull, 0xFF06FF03FF03FF00ull));

	__m128i row0_b = _mm_shuffle_epi8(values0, set128i(0xFF0dFF0aFF0aFF07ull, 0xFF07FF04FF04FF01ull));
	__m128i row1_b = _mm_shuffle_epi8(values1, set128i(0xFF0dFF0aFF0aFF07ull, 0xFF07FF04FF04FF01ull));

	__m128i row0_c = _mm_shuffle_epi8(values0, set128i(0xFF0eFF0bFF0bFF08ull, 0xFF08FF05FF05FF02ull));
	__m128i row1_c = _mm_shuffle_epi8(values1, set128i(0xFF0eFF0bFF0bFF08ull, 0xFF08FF05FF05FF02ull));

	row0_a = _mm_madd_epi16(row0_a, fx_fy_fxfy_);
	row0_b = _mm_madd_epi16(row0_b, fx_fy_fxfy_);
	row0_c = _mm_madd_epi16(row0_c, fx_fy_fxfy_);

	row1_a = _mm_madd_epi16(row1_a, fx_fyfxfy);
	row1_b = _mm_madd_epi16(row1_b, fx_fyfxfy);
	row1_c = _mm_madd_epi16(row1_c, fx_fyfxfy);

	const __m128i rounding = _mm_set1_epi32(8192);

	__m128i row_a = _mm_add_epi32(row0_a, row1_a);
	__m128i row_b = _mm_add_epi32(row0_b, row1_b);
	__m128i row_c = _mm_add_epi32(row0_c, row1_c);

	row_a = _mm_add_epi32(row_a, rounding);
	row_b = _mm_add_epi32(row_b, rounding);
	row_c = _mm_add_epi32(row_c, rounding);

	row_a = _mm_srli_epi32(row_a, 14);
	row_b = _mm_srli_epi32(row_b, 14);
	row_c = _mm_srli_epi32(row_c, 14);

	row_a = _mm_shuffle_epi8(row_a, set128i(0xFFFFFFFFFFFF0cFFull, 0xFF08FFFF04FFFF00ull));
	row_b = _mm_shuffle_epi8(row_b, set128i(0xFFFFFFFFFF0cFFFFull, 0x08FFFF04FFFF00FFull));
	row_c = _mm_shuffle_epi8(row_c, set128i(0xFFFFFFFF0cFFFF08ull, 0xFFFF04FFFF00FFFFull));

	return _mm_or_si128(row_a, _mm_or_si128(row_b, row_c));
}

inline __m128i SSE::interpolation4Channel32Bit8Elements(const __m128i& values0, const __m128i& values1, const __m128i& fx_fy_, const __m128i& fxfy_, const __m128i& fx_fy, const __m128i& fxfy)
{
	//           F   E   D   C    B   A   9   8    7   6   5   4    3   2   1   0
	// values0: a3  b3  g3  r3 | a2  b2  g2  r2 | a1  b1  g1  r1 | a0  b0  g0  r0
	// values1: a3' b3' g3' r3'| a2' b2' g2' r2'| a1' b1' g1' r1'| a0' b0' g0' r0'

	// shuffled elements
	// row0: a1  b1  g1  r1  a0  b0  g0  r0   |  * fx_ * fy_
	// row1: a2  b2  g2  r2  a1  b1  g1  r1   |  * fx  * fy_
	// row2: a1' b1' g1' r1' a0' b0' g0' r0'  |  * fx_ * fy
	// row3: a2' b2' g2' r2' a1' b1' g1' r1'  |  * fx  * fy

#ifdef OCEAN_COMPILER_MSC

	ocean_assert(fx_fy_.m128i_u16[0] == fx_fy_.m128i_u16[1]);
	ocean_assert(fx_fy_.m128i_u16[1] == fx_fy_.m128i_u16[2]);
	ocean_assert(fx_fy_.m128i_u16[2] == fx_fy_.m128i_u16[3]);
	ocean_assert(fx_fy_.m128i_u16[3] == fx_fy_.m128i_u16[4]);
	ocean_assert(fx_fy_.m128i_u16[4] == fx_fy_.m128i_u16[5]);
	ocean_assert(fx_fy_.m128i_u16[5] == fx_fy_.m128i_u16[6]);
	ocean_assert(fx_fy_.m128i_u16[6] == fx_fy_.m128i_u16[7]);

	ocean_assert(fxfy_.m128i_u16[0] == fxfy_.m128i_u16[1]);
	ocean_assert(fxfy_.m128i_u16[1] == fxfy_.m128i_u16[2]);
	ocean_assert(fxfy_.m128i_u16[2] == fxfy_.m128i_u16[3]);
	ocean_assert(fxfy_.m128i_u16[3] == fxfy_.m128i_u16[4]);
	ocean_assert(fxfy_.m128i_u16[4] == fxfy_.m128i_u16[5]);
	ocean_assert(fxfy_.m128i_u16[5] == fxfy_.m128i_u16[6]);
	ocean_assert(fxfy_.m128i_u16[6] == fxfy_.m128i_u16[7]);

	ocean_assert(fx_fy.m128i_u16[0] == fx_fy.m128i_u16[1]);
	ocean_assert(fx_fy.m128i_u16[1] == fx_fy.m128i_u16[2]);
	ocean_assert(fx_fy.m128i_u16[2] == fx_fy.m128i_u16[3]);
	ocean_assert(fx_fy.m128i_u16[3] == fx_fy.m128i_u16[4]);
	ocean_assert(fx_fy.m128i_u16[4] == fx_fy.m128i_u16[5]);
	ocean_assert(fx_fy.m128i_u16[5] == fx_fy.m128i_u16[6]);
	ocean_assert(fx_fy.m128i_u16[6] == fx_fy.m128i_u16[7]);

	ocean_assert(fxfy.m128i_u16[0] == fxfy.m128i_u16[1]);
	ocean_assert(fxfy.m128i_u16[1] == fxfy.m128i_u16[2]);
	ocean_assert(fxfy.m128i_u16[2] == fxfy.m128i_u16[3]);
	ocean_assert(fxfy.m128i_u16[3] == fxfy.m128i_u16[4]);
	ocean_assert(fxfy.m128i_u16[4] == fxfy.m128i_u16[5]);
	ocean_assert(fxfy.m128i_u16[5] == fxfy.m128i_u16[6]);
	ocean_assert(fxfy.m128i_u16[6] == fxfy.m128i_u16[7]);

#else

#ifdef OCEAN_DEBUG

	const M128i& debug_fx_fy_ = *(const M128i*)(&fx_fy_);
	const M128i& debug_fx_fy = *(const M128i*)(&fx_fy);
	const M128i& debug_fxfy_ = *(const M128i*)(&fxfy_);
	const M128i& debug_fxfy = *(const M128i*)(&fxfy);

#endif // OCEAN_DEBUG

	ocean_assert(debug_fx_fy_.m128i_u16[0] == debug_fx_fy_.m128i_u16[1]);
	ocean_assert(debug_fx_fy_.m128i_u16[1] == debug_fx_fy_.m128i_u16[2]);
	ocean_assert(debug_fx_fy_.m128i_u16[2] == debug_fx_fy_.m128i_u16[3]);
	ocean_assert(debug_fx_fy_.m128i_u16[3] == debug_fx_fy_.m128i_u16[4]);
	ocean_assert(debug_fx_fy_.m128i_u16[4] == debug_fx_fy_.m128i_u16[5]);
	ocean_assert(debug_fx_fy_.m128i_u16[5] == debug_fx_fy_.m128i_u16[6]);
	ocean_assert(debug_fx_fy_.m128i_u16[6] == debug_fx_fy_.m128i_u16[7]);

	ocean_assert(debug_fxfy_.m128i_u16[0] == debug_fxfy_.m128i_u16[1]);
	ocean_assert(debug_fxfy_.m128i_u16[1] == debug_fxfy_.m128i_u16[2]);
	ocean_assert(debug_fxfy_.m128i_u16[2] == debug_fxfy_.m128i_u16[3]);
	ocean_assert(debug_fxfy_.m128i_u16[3] == debug_fxfy_.m128i_u16[4]);
	ocean_assert(debug_fxfy_.m128i_u16[4] == debug_fxfy_.m128i_u16[5]);
	ocean_assert(debug_fxfy_.m128i_u16[5] == debug_fxfy_.m128i_u16[6]);
	ocean_assert(debug_fxfy_.m128i_u16[6] == debug_fxfy_.m128i_u16[7]);

	ocean_assert(debug_fx_fy.m128i_u16[0] == debug_fx_fy.m128i_u16[1]);
	ocean_assert(debug_fx_fy.m128i_u16[1] == debug_fx_fy.m128i_u16[2]);
	ocean_assert(debug_fx_fy.m128i_u16[2] == debug_fx_fy.m128i_u16[3]);
	ocean_assert(debug_fx_fy.m128i_u16[3] == debug_fx_fy.m128i_u16[4]);
	ocean_assert(debug_fx_fy.m128i_u16[4] == debug_fx_fy.m128i_u16[5]);
	ocean_assert(debug_fx_fy.m128i_u16[5] == debug_fx_fy.m128i_u16[6]);
	ocean_assert(debug_fx_fy.m128i_u16[6] == debug_fx_fy.m128i_u16[7]);

	ocean_assert(debug_fxfy.m128i_u16[0] == debug_fxfy.m128i_u16[1]);
	ocean_assert(debug_fxfy.m128i_u16[1] == debug_fxfy.m128i_u16[2]);
	ocean_assert(debug_fxfy.m128i_u16[2] == debug_fxfy.m128i_u16[3]);
	ocean_assert(debug_fxfy.m128i_u16[3] == debug_fxfy.m128i_u16[4]);
	ocean_assert(debug_fxfy.m128i_u16[4] == debug_fxfy.m128i_u16[5]);
	ocean_assert(debug_fxfy.m128i_u16[5] == debug_fxfy.m128i_u16[6]);
	ocean_assert(debug_fxfy.m128i_u16[6] == debug_fxfy.m128i_u16[7]);

#endif

	__m128i shuffle = set128i(0xA007A006A005A004ull, 0xA003A002A001A000ull);

	// row0
	__m128i row = _mm_shuffle_epi8(values0, shuffle);

	__m128i multiLow = _mm_mullo_epi16(row, fx_fy_);
	__m128i multiHigh = _mm_mulhi_epu16(row, fx_fy_);

	__m128i resultEven = _mm_blend_epi16(multiLow, _mm_slli_si128(multiHigh, 2), 0xAA); // 0xAA = 1010 1010
	__m128i resultOdd = _mm_blend_epi16(_mm_srli_si128(multiLow, 2), multiHigh, 0xAA);

	// row2
	row = _mm_shuffle_epi8(values1, shuffle);

	multiLow = _mm_mullo_epi16(row, fx_fy);
	multiHigh = _mm_mulhi_epu16(row, fx_fy);

	resultEven = _mm_add_epi32(resultEven, _mm_blend_epi16(multiLow, _mm_slli_si128(multiHigh, 2), 0xAA));
	resultOdd = _mm_add_epi32(resultOdd, _mm_blend_epi16(_mm_srli_si128(multiLow, 2), multiHigh, 0xAA));



	shuffle = set128i(0xA00BA00AA009A008ull, 0xA007A006A005A004ull);

	// row1
	row = _mm_shuffle_epi8(values0, shuffle);

	multiLow = _mm_mullo_epi16(row, fxfy_);
	multiHigh = _mm_mulhi_epu16(row, fxfy_);

	resultEven = _mm_add_epi32(resultEven, _mm_blend_epi16(multiLow, _mm_slli_si128(multiHigh, 2), 0xAA));
	resultOdd = _mm_add_epi32(resultOdd, _mm_blend_epi16(_mm_srli_si128(multiLow, 2), multiHigh, 0xAA));


	// row4
	row = _mm_shuffle_epi8(values1, shuffle);

	multiLow = _mm_mullo_epi16(row, fxfy);
	multiHigh = _mm_mulhi_epu16(row, fxfy);

	resultEven = _mm_add_epi32(resultEven, _mm_blend_epi16(multiLow, _mm_slli_si128(multiHigh, 2), 0xAA));
	resultOdd = _mm_add_epi32(resultOdd, _mm_blend_epi16(_mm_srli_si128(multiLow, 2), multiHigh, 0xAA));


	// normalization ( + 128 * 128 / 2) / (128 * 128)
	resultEven = _mm_add_epi32(resultEven, _mm_set1_epi32(8192));
	resultEven = _mm_srli_epi32(resultEven, 14);

	resultOdd = _mm_add_epi32(resultOdd, _mm_set1_epi32(8192));
	resultOdd = _mm_srli_epi32(resultOdd, 14);

	// stack the 2 four 32 bit values together to eight 8 bit values
	return moveLowBits32_16ToLow64(_mm_or_si128(resultEven, _mm_slli_si128(resultOdd, 1)));
}


inline __m128i SSE::interpolation4Channel32Bit2x4Elements(const __m128i& values0, const __m128i& values1, const __m128i& fx_fy_, const __m128i& fxfy_, const __m128i& fx_fy, const __m128i& fxfy)
{
	//           F   E   D   C    B   A   9   8    7   6   5   4    3   2   1   0
	// values0: a3  b3  g3  r3 | a2  b2  g2  r2 | a1  b1  g1  r1 | a0  b0  g0  r0
	// values1: a3' b3' g3' r3'| a2' b2' g2' r2'| a1' b1' g1' r1'| a0' b0' g0' r0'

	// shuffled elements
	// row0: a2  b2  g2  r2  a0  b0  g0  r0   |  * fx_ * fy_
	// row1: a3  b3  g3  r3  a1  b1  g1  r1   |  * fx  * fy_
	// row2: a2' b2' g2' r2' a0' b0' g0' r0'  |  * fx_ * fy
	// row3: a3' b3' g3' r3' a1' b1' g1' r1'  |  * fx  * fy

#ifdef OCEAN_COMPILER_MSC

	ocean_assert(fx_fy_.m128i_u16[0] == fx_fy_.m128i_u16[1]);
	ocean_assert(fx_fy_.m128i_u16[1] == fx_fy_.m128i_u16[2]);
	ocean_assert(fx_fy_.m128i_u16[2] == fx_fy_.m128i_u16[3]);
	ocean_assert(fx_fy_.m128i_u16[3] == fx_fy_.m128i_u16[4]);
	ocean_assert(fx_fy_.m128i_u16[4] == fx_fy_.m128i_u16[5]);
	ocean_assert(fx_fy_.m128i_u16[5] == fx_fy_.m128i_u16[6]);
	ocean_assert(fx_fy_.m128i_u16[6] == fx_fy_.m128i_u16[7]);

	ocean_assert(fxfy_.m128i_u16[0] == fxfy_.m128i_u16[1]);
	ocean_assert(fxfy_.m128i_u16[1] == fxfy_.m128i_u16[2]);
	ocean_assert(fxfy_.m128i_u16[2] == fxfy_.m128i_u16[3]);
	ocean_assert(fxfy_.m128i_u16[3] == fxfy_.m128i_u16[4]);
	ocean_assert(fxfy_.m128i_u16[4] == fxfy_.m128i_u16[5]);
	ocean_assert(fxfy_.m128i_u16[5] == fxfy_.m128i_u16[6]);
	ocean_assert(fxfy_.m128i_u16[6] == fxfy_.m128i_u16[7]);

	ocean_assert(fx_fy.m128i_u16[0] == fx_fy.m128i_u16[1]);
	ocean_assert(fx_fy.m128i_u16[1] == fx_fy.m128i_u16[2]);
	ocean_assert(fx_fy.m128i_u16[2] == fx_fy.m128i_u16[3]);
	ocean_assert(fx_fy.m128i_u16[3] == fx_fy.m128i_u16[4]);
	ocean_assert(fx_fy.m128i_u16[4] == fx_fy.m128i_u16[5]);
	ocean_assert(fx_fy.m128i_u16[5] == fx_fy.m128i_u16[6]);
	ocean_assert(fx_fy.m128i_u16[6] == fx_fy.m128i_u16[7]);

	ocean_assert(fxfy.m128i_u16[0] == fxfy.m128i_u16[1]);
	ocean_assert(fxfy.m128i_u16[1] == fxfy.m128i_u16[2]);
	ocean_assert(fxfy.m128i_u16[2] == fxfy.m128i_u16[3]);
	ocean_assert(fxfy.m128i_u16[3] == fxfy.m128i_u16[4]);
	ocean_assert(fxfy.m128i_u16[4] == fxfy.m128i_u16[5]);
	ocean_assert(fxfy.m128i_u16[5] == fxfy.m128i_u16[6]);
	ocean_assert(fxfy.m128i_u16[6] == fxfy.m128i_u16[7]);

#else

#ifdef OCEAN_DEBUG

	const M128i& debug_fx_fy_ = *(const M128i*)(&fx_fy_);
	const M128i& debug_fx_fy = *(const M128i*)(&fx_fy);
	const M128i& debug_fxfy_ = *(const M128i*)(&fxfy_);
	const M128i& debug_fxfy = *(const M128i*)(&fxfy);

#endif // OCEAN_DEBUG

	ocean_assert(debug_fx_fy_.m128i_u16[0] == debug_fx_fy_.m128i_u16[1]);
	ocean_assert(debug_fx_fy_.m128i_u16[1] == debug_fx_fy_.m128i_u16[2]);
	ocean_assert(debug_fx_fy_.m128i_u16[2] == debug_fx_fy_.m128i_u16[3]);
	ocean_assert(debug_fx_fy_.m128i_u16[3] == debug_fx_fy_.m128i_u16[4]);
	ocean_assert(debug_fx_fy_.m128i_u16[4] == debug_fx_fy_.m128i_u16[5]);
	ocean_assert(debug_fx_fy_.m128i_u16[5] == debug_fx_fy_.m128i_u16[6]);
	ocean_assert(debug_fx_fy_.m128i_u16[6] == debug_fx_fy_.m128i_u16[7]);

	ocean_assert(debug_fxfy_.m128i_u16[0] == debug_fxfy_.m128i_u16[1]);
	ocean_assert(debug_fxfy_.m128i_u16[1] == debug_fxfy_.m128i_u16[2]);
	ocean_assert(debug_fxfy_.m128i_u16[2] == debug_fxfy_.m128i_u16[3]);
	ocean_assert(debug_fxfy_.m128i_u16[3] == debug_fxfy_.m128i_u16[4]);
	ocean_assert(debug_fxfy_.m128i_u16[4] == debug_fxfy_.m128i_u16[5]);
	ocean_assert(debug_fxfy_.m128i_u16[5] == debug_fxfy_.m128i_u16[6]);
	ocean_assert(debug_fxfy_.m128i_u16[6] == debug_fxfy_.m128i_u16[7]);

	ocean_assert(debug_fx_fy.m128i_u16[0] == debug_fx_fy.m128i_u16[1]);
	ocean_assert(debug_fx_fy.m128i_u16[1] == debug_fx_fy.m128i_u16[2]);
	ocean_assert(debug_fx_fy.m128i_u16[2] == debug_fx_fy.m128i_u16[3]);
	ocean_assert(debug_fx_fy.m128i_u16[3] == debug_fx_fy.m128i_u16[4]);
	ocean_assert(debug_fx_fy.m128i_u16[4] == debug_fx_fy.m128i_u16[5]);
	ocean_assert(debug_fx_fy.m128i_u16[5] == debug_fx_fy.m128i_u16[6]);
	ocean_assert(debug_fx_fy.m128i_u16[6] == debug_fx_fy.m128i_u16[7]);

	ocean_assert(debug_fxfy.m128i_u16[0] == debug_fxfy.m128i_u16[1]);
	ocean_assert(debug_fxfy.m128i_u16[1] == debug_fxfy.m128i_u16[2]);
	ocean_assert(debug_fxfy.m128i_u16[2] == debug_fxfy.m128i_u16[3]);
	ocean_assert(debug_fxfy.m128i_u16[3] == debug_fxfy.m128i_u16[4]);
	ocean_assert(debug_fxfy.m128i_u16[4] == debug_fxfy.m128i_u16[5]);
	ocean_assert(debug_fxfy.m128i_u16[5] == debug_fxfy.m128i_u16[6]);
	ocean_assert(debug_fxfy.m128i_u16[6] == debug_fxfy.m128i_u16[7]);

#endif

	__m128i shuffle = set128i(0xA00BA00AA009A008ull, 0xA003A002A001A000ull);

	// row0
	__m128i row = _mm_shuffle_epi8(values0, shuffle);

	__m128i multiLow = _mm_mullo_epi16(row, fx_fy_);
	__m128i multiHigh = _mm_mulhi_epu16(row, fx_fy_);

	__m128i resultEven = _mm_blend_epi16(multiLow, _mm_slli_si128(multiHigh, 2), 0xAA); // 0xAA = 1010 1010
	__m128i resultOdd = _mm_blend_epi16(_mm_srli_si128(multiLow, 2), multiHigh, 0xAA);

	// row2
	row = _mm_shuffle_epi8(values1, shuffle);

	multiLow = _mm_mullo_epi16(row, fx_fy);
	multiHigh = _mm_mulhi_epu16(row, fx_fy);

	resultEven = _mm_add_epi32(resultEven, _mm_blend_epi16(multiLow, _mm_slli_si128(multiHigh, 2), 0xAA));
	resultOdd = _mm_add_epi32(resultOdd, _mm_blend_epi16(_mm_srli_si128(multiLow, 2), multiHigh, 0xAA));



	shuffle = set128i(0xA00FA00EA00DA00Cull, 0xA007A006A005A004ull);

	// row1
	row = _mm_shuffle_epi8(values0, shuffle);

	multiLow = _mm_mullo_epi16(row, fxfy_);
	multiHigh = _mm_mulhi_epu16(row, fxfy_);

	resultEven = _mm_add_epi32(resultEven, _mm_blend_epi16(multiLow, _mm_slli_si128(multiHigh, 2), 0xAA));
	resultOdd = _mm_add_epi32(resultOdd, _mm_blend_epi16(_mm_srli_si128(multiLow, 2), multiHigh, 0xAA));


	// row4
	row = _mm_shuffle_epi8(values1, shuffle);

	multiLow = _mm_mullo_epi16(row, fxfy);
	multiHigh = _mm_mulhi_epu16(row, fxfy);

	resultEven = _mm_add_epi32(resultEven, _mm_blend_epi16(multiLow, _mm_slli_si128(multiHigh, 2), 0xAA));
	resultOdd = _mm_add_epi32(resultOdd, _mm_blend_epi16(_mm_srli_si128(multiLow, 2), multiHigh, 0xAA));


	// normalization ( + 128 * 128 / 2) / (128 * 128)
	resultEven = _mm_add_epi32(resultEven, _mm_set1_epi32(8192));
	resultEven = _mm_srli_epi32(resultEven, 14);

	resultOdd = _mm_add_epi32(resultOdd, _mm_set1_epi32(8192));
	resultOdd = _mm_srli_epi32(resultOdd, 14);

	// stack the 2 four 32 bit values together to eight 8 bit values
	return moveLowBits32_16ToLow64(_mm_or_si128(resultEven, _mm_slli_si128(resultOdd, 1)));
}

inline void SSE::average8Elements1Channel32Bit2x2(const float* const image0, const float* const image1, float* const result)
{
	ocean_assert(image0 && image1);

	// 4 * float = m128, input does not need to be aligned on any particular boundary.
	const __m128 row0 = _mm_loadu_ps(image0);
	const __m128 row1 = _mm_loadu_ps(image1);

	// get sum of first 4 elements
	const __m128 sumFirst = _mm_add_ps(row0, row1);

	// load next 4 elements
	const __m128 rowSecond0 = _mm_loadu_ps(image0 + 4);
	const __m128 rowSecond1 = _mm_loadu_ps(image1 + 4);

	// get sum of second 4 elements
	const __m128 sumSecond = _mm_add_ps(rowSecond0, rowSecond1);

	// get sum of adjacent summed pixels
	const __m128 sumAdjacent = _mm_hadd_ps(sumFirst, sumSecond);

	/* following variant is exactly as fast as _mm_hadd_ps(,) ~ 0.30ms / 100,000 iteration
	const unsigned int mask10001000 = 136u;
	const unsigned int mask11011101 = 221u;
	const __m128 sumAdjacent = _mm_add_ps(_mm_shuffle_ps(sumFirst, sumSecond, mask10001000), _mm_shuffle_ps(sumFirst, sumSecond, mask11011101));
	*/

	// divide by 4 --> multiply by 0.25
	const __m128 division =  _mm_mul_ps(sumAdjacent, _mm_set_ps1(0.25f));

	// store 4 elements (128 bit) to the memory, output does not need to be aligned on any particular boundary.
	_mm_storeu_ps(result, division);
}

inline void SSE::average8Elements1Channel8Bit2x2(const uint8_t* const image0, const uint8_t* const image1, uint8_t* const result)
{
	ocean_assert(image0 && image1);

	// 16 * uchar = m128i, but only the first 8 elements are set
	const __m128i row0 = _mm_loadl_epi64((__m128i*)image0);
	const __m128i row1 = _mm_loadl_epi64((__m128i*)image1);

	// distribute the 8 elements of 8 bit values into 8 elements of 16 bit values
	const __m128i sumLow = _mm_add_epi16(removeHighBits16_8(row0), removeHighBits16_8(row1));
	const __m128i sumHigh = _mm_add_epi16(moveHighBits16_8(row0), moveHighBits16_8(row1));

	// build overall sum and add 2 for rounding
	const __m128i sum = _mm_add_epi16(sumLow, _mm_add_epi16(sumHigh, _mm_set1_epi32(int(0x00020002))));

	// divide by 4 by right shifting of two bits
	const __m128i division16 = _mm_srli_epi16(sum, 2);

	// shift the lower 8 bit of the eight 16 bit values to the lower 64 bit
	const __m128i division8 = moveLowBits16_8ToLow64(division16);

	memcpy(result, &division8, sizeof(uint8_t) * 4);
}

inline void SSE::average8ElementsBinary1Channel8Bit2x2(const uint8_t* const image0, const uint8_t* const image1, uint8_t* const result, const uint16_t threshold)
{
	ocean_assert(image0 != nullptr && image1 != nullptr);
	ocean_assert(threshold >= 1u);

	// we load the first 8 elements, the uppper 8 bytes will be set to zero
	const __m128i row0_u_8x8 = _mm_loadl_epi64((__m128i*)image0);
	const __m128i row1_u_8x8 = _mm_loadl_epi64((__m128i*)image1);

	const __m128i row0_u_16x8 = _mm_cvtepu8_epi16(row0_u_8x8); // converting the lower 8 bytes to 16 byte values
	const __m128i row1_u_16x8 = _mm_cvtepu8_epi16(row1_u_8x8);

	const __m128i verticalSum_u_16x8 = _mm_adds_epu16(row0_u_16x8, row1_u_16x8);
	const __m128i sum_u_16x8 = _mm_hadd_epi16(verticalSum_u_16x8, verticalSum_u_16x8);

	const __m128i mask_u_16x8 = _mm_cmpgt_epi16(sum_u_16x8, _mm_set1_epi16(short(threshold - 1u)));

	const __m128i mask_u_8x8 = moveLowBits16_8ToLow64(mask_u_16x8);

	memcpy(result, &mask_u_8x8, sizeof(uint8_t) * 4);
}

inline void SSE::average16Elements1Channel8Bit2x2(const uint8_t* const image0, const uint8_t* const image1, uint8_t* const result)
{
	ocean_assert(image0 && image1);

	// 16 * uchar = m128i
	const __m128i row0 = _mm_lddqu_si128((__m128i*)image0);
	const __m128i row1 = _mm_lddqu_si128((__m128i*)image1);

	// distribute the 16 elements of 8 bit values into 16 elements of 16 bit values and create the sum
	const __m128i sumLow = _mm_add_epi16(removeHighBits16_8(row0), removeHighBits16_8(row1));
	const __m128i sumHigh = _mm_add_epi16(moveHighBits16_8(row0), moveHighBits16_8(row1));

	// build overall sum and add 2 for rounding
	const __m128i sum = _mm_add_epi16(sumLow, _mm_add_epi16(sumHigh, _mm_set1_epi32(int(0x00020002))));

	// divide by 4 by right shifting of two bits
	const __m128i division16 = _mm_srli_epi16(sum, 2);

	// shift the lower 8 bit of the eight 16 bit values to the lower 64 bit
	const __m128i division8 = moveLowBits16_8ToLow64(division16);

	// copy the lower 64 bit to the memory
	_mm_storel_epi64((__m128i*)result, division8);

	/* using _mm_avg_epu8 is a bit faster (~3%) but result is always rounded up
	const __m128i avgRows = _mm_avg_epu8(row0, row1);
	const __m128i avgRowsSwap =  _mm_or_si128(_mm_slli_epi16(avgRows, 8),	_mm_srli_epi16(avgRows, 8));

	const __m128i avg = _mm_avg_epu8(avgRows, avgRowsSwap); // 1 result in 2 uchar
	const __m128i avgOrdered = _mm_shuffle_epi8(avg, _mm_set_epi8(0, 0, 0, 0, 0, 0, 0, 0, 14, 12, 10, 8, 6, 4, 2, 0));

	_mm_storel_epi64((__m128i*)result, avgOrdered);
	*/
}

inline void SSE::average16ElementsBinary1Channel8Bit2x2(const uint8_t* const image0, const uint8_t* const image1, uint8_t* const result, const uint16_t threshold)
{
	ocean_assert(image0 != nullptr && image1 != nullptr);
	ocean_assert(threshold >= 1u);

	// 16 * uchar = m128i
	const __m128i row0_u_8x16 = _mm_lddqu_si128((__m128i*)image0);
	const __m128i row1_u_8x16 = _mm_lddqu_si128((__m128i*)image1);

	const __m128i horizontalSum0_u_16x8 = _mm_maddubs_epi16(row0_u_8x16, _mm_set1_epi8(1));
	const __m128i horizontalSum1_u_16x8 = _mm_maddubs_epi16(row1_u_8x16, _mm_set1_epi8(1));

	const __m128i sum_u_16x8 = _mm_add_epi16(horizontalSum0_u_16x8, horizontalSum1_u_16x8);

	const __m128i mask_u_16x8 = _mm_cmpgt_epi16(sum_u_16x8, _mm_set1_epi16(short(threshold - 1u)));

	const __m128i mask_u_8x8 = moveLowBits16_8ToLow64(mask_u_16x8);

	// copy the lower 64 bit to the memory
	_mm_storel_epi64((__m128i*)result, mask_u_8x8);
}

inline void SSE::average32Elements1Channel8Bit2x2(const uint8_t* const image0, const uint8_t* const image1, uint8_t* const result)
{
	ocean_assert(image0 && image1);

	// first 16 elements
	const __m128i firstRow0 = _mm_lddqu_si128((__m128i*)image0);
	const __m128i firstRow1 = _mm_lddqu_si128((__m128i*)image1);

	// distribute the 16 elements of 8 bit values into 16 elements of 16 bit values and create the sum
	const __m128i firstSumLow = _mm_add_epi16(removeHighBits16_8(firstRow0), removeHighBits16_8(firstRow1));
	const __m128i firstSumHigh = _mm_add_epi16(moveHighBits16_8(firstRow0), moveHighBits16_8(firstRow1));

	// build overall sum and add 2 for rounding
	const __m128i firstSum = _mm_add_epi16(firstSumLow, _mm_add_epi16(firstSumHigh, _mm_set1_epi32(int(0x00020002))));

	// divide by 4 by right shifting of two bits
	const __m128i firstDivision16 = _mm_srli_epi16(firstSum, 2);

	// shift the lower 8 bit of the eight 16 bit values to the lower 64 bit
	const __m128i firstDivision8 = moveLowBits16_8ToLow64(firstDivision16);

	// second 16 elements
	const __m128i secondRow0 = _mm_lddqu_si128((__m128i*)(image0 + 16));
	const __m128i secondRow1 = _mm_lddqu_si128((__m128i*)(image1 + 16));

	// distribute the 16 elements of 8 bit values into 16 elements of 16 bit values and create the sum
	const __m128i secondSumLow = _mm_add_epi16(removeHighBits16_8(secondRow0), removeHighBits16_8(secondRow1));
	const __m128i secondSumHigh = _mm_add_epi16(moveHighBits16_8(secondRow0), moveHighBits16_8(secondRow1));

	// build overall sum and add 2 for rounding
	const __m128i secondSum = _mm_add_epi16(secondSumLow, _mm_add_epi16(secondSumHigh, _mm_set1_epi32(int(0x00020002))));

	// divide by 4 by right shifting of two bits
	const __m128i secondDivision16 = _mm_srli_epi16(secondSum, 2);

	// shift the lower 8 bit of the eight 16 bit values to the lower 64 bit
	const __m128i secondDivision8 = moveLowBits16_8ToHigh64(secondDivision16);


	// combine both divion results
	const __m128i division8 = _mm_or_si128(firstDivision8, secondDivision8);

	// copy the 128 bit to the memory
	_mm_storeu_si128((__m128i*)result, division8);

	/* using _mm_avg_epu8 is a bit faster (~3%) but result is always rounded up
	const __m128i avgFirstRows = _mm_avg_epu8(firstRow0, firstRow1);
	const __m128i avgFirstRowsSwap =  _mm_or_si128(_mm_slli_epi16(avgFirstRows, 8),	_mm_srli_epi16(avgFirstRows, 8));

	const __m128i avgFirst = _mm_avg_epu8(avgFirstRows, avgFirstRowsSwap); // 1 result in 2 uchar
	const __m128i avgFristOrdered = _mm_shuffle_epi8(avgFirst, _mm_set_epi8(0, 0, 0, 0, 0, 0, 0, 0, 14, 12, 10, 8, 6, 4, 2, 0));

	const __m128i avgSecondRows = _mm_avg_epu8(secondRow0, secondRow1);
	const __m128i avgSecondRowsSwap =  _mm_or_si128(_mm_slli_epi16(avgSecondRows, 8),	_mm_srli_epi16(avgSecondRows, 8));

	const __m128i avgSecond = _mm_avg_epu8(avgSecondRows, avgSecondRowsSwap); // 1 result in 2 uchar
	const __m128i avgSecondOrdered = _mm_shuffle_epi8(avgSecond, _mm_set_epi8(14, 12, 10, 8, 6, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0));

	// combine both divion results
	const __m128i combinedAvg = _mm_or_si128(avgFristOrdered, avgSecondOrdered);

	// copy the 128 bit to the memory
	_mm_storeu_si128((__m128i*)result, combinedAvg);
	*/
}

inline void SSE::average32ElementsBinary1Channel8Bit2x2(const uint8_t* const image0, const uint8_t* const image1, uint8_t* const result, const uint16_t threshold)
{
	ocean_assert(image0 != nullptr && image1 != nullptr);
	ocean_assert(threshold >= 1u);

	// load first 16 uchars
	const __m128i row0A_u_8x16 = _mm_lddqu_si128((__m128i*)image0);
	const __m128i row1A_u_8x16 = _mm_lddqu_si128((__m128i*)image1);

	const __m128i horizontalSum0A_u_16x8 = _mm_maddubs_epi16(row0A_u_8x16, _mm_set1_epi8(1));
	const __m128i horizontalSum1A_u_16x8 = _mm_maddubs_epi16(row1A_u_8x16, _mm_set1_epi8(1));

	const __m128i sumA_u_16x8 = _mm_add_epi16(horizontalSum0A_u_16x8, horizontalSum1A_u_16x8);

	const __m128i maskA_u_16x8 = _mm_cmpgt_epi16(sumA_u_16x8, _mm_set1_epi16(short(threshold - 1)));

	const __m128i row0B_u_8x16 = _mm_lddqu_si128((__m128i*)(image0 + 16));
	const __m128i row1B_u_8x16 = _mm_lddqu_si128((__m128i*)(image1 + 16));

	const __m128i horizontalSum0B_u_16x8 = _mm_maddubs_epi16(row0B_u_8x16, _mm_set1_epi8(1));
	const __m128i horizontalSum1B_u_16x8 = _mm_maddubs_epi16(row1B_u_8x16, _mm_set1_epi8(1));

	const __m128i sumB_u_16x8 = _mm_add_epi16(horizontalSum0B_u_16x8, horizontalSum1B_u_16x8);

	const __m128i maskB_u_16x8 = _mm_cmpgt_epi16(sumB_u_16x8, _mm_set1_epi16(short(threshold - 1u)));

	const __m128i mask_u_8x16 = _mm_or_si128(moveLowBits16_8ToLow64(maskA_u_16x8), moveLowBits16_8ToHigh64(maskB_u_16x8));

	// copy the 128 bit to the memory
	_mm_storeu_si128((__m128i*)result, mask_u_8x16);
}

inline void SSE::average8Elements2Channel16Bit2x2(const uint8_t* const image0, const uint8_t* const image1, uint8_t* const result)
{
	ocean_assert(image0 && image1);

	// 16 * uchar = m128i, but only the first 8 elements are set
	const __m128i row0 = _mm_loadl_epi64((__m128i*)image0);
	const __m128i row1 = _mm_loadl_epi64((__m128i*)image1);

	// distribute the 8 elements of 8 bit values into 8 elements of 16 bit values
	const __m128i shuffledRow0 = shuffleNeighbor2Low64BitsToLow16_8(row0);
	const __m128i shuffledRow1 = shuffleNeighbor2Low64BitsToLow16_8(row1);

	// build sum and add 2 for rounding
	const __m128i sumLow = _mm_add_epi16(shuffledRow0, shuffledRow1);
	const __m128i sum = _mm_add_epi16(_mm_hadd_epi16(sumLow, sumLow), _mm_set1_epi32(int(0x00020002)));

	// divide by 4 by right shifting of two bits
	const __m128i division16 = _mm_srli_epi16(sum, 2);

	// shift the lower 8 bit of the eight 16 bit values to the lower 64 bit
	const __m128i division8 = moveLowBits16_8ToLow64(division16);

	memcpy(result, &division8, sizeof(uint8_t) * 4);
}

inline void SSE::average8Elements2Channel64Bit2x2(const float* const image0, const float* const image1, float* const result)
{
	ocean_assert(image0 && image1);

	// 4 * float = m128, input does not need to be aligned on any particular boundary.
	const __m128 row0 = _mm_loadu_ps(image0);
	const __m128 row1 = _mm_loadu_ps(image1);

	// get sum of first 4 elements
	const __m128 sumFirst = _mm_add_ps(row0, row1);

	// load next 4 elements
	const __m128 rowSecond0 = _mm_loadu_ps(image0 + 4);
	const __m128 rowSecond1 = _mm_loadu_ps(image1 + 4);

	// get sum of second 4 elements
	const __m128 sumSecond = _mm_add_ps(rowSecond0, rowSecond1);

	// get sum of summed pixels
	// mask01000100 = 68u
	// mask11101110 = 238u
	const __m128 sumComponents = _mm_add_ps(_mm_shuffle_ps(sumFirst, sumSecond, 68u), _mm_shuffle_ps(sumFirst, sumSecond, 238u));

	// divide by 4 --> multiply by 0.25
	const __m128 division =  _mm_mul_ps(sumComponents, _mm_set_ps1(0.25f));

	// store 4 elements (128 bit) to the memory, output does not need to be aligned on any particular boundary.
	_mm_storeu_ps(result, division);
}

inline void SSE::average16Elements2Channel16Bit2x2(const uint8_t* const image0, const uint8_t* const image1, uint8_t* const result)
{
	ocean_assert(image0 && image1);

	// 16 * uchar = m128i
	const __m128i row0 = _mm_lddqu_si128((__m128i*)image0);
	const __m128i row1 = _mm_lddqu_si128((__m128i*)image1);

	// distribute the 8 elements of 8 bit values into 8 elements of 16 bit values
	const __m128i sumLow = _mm_add_epi16(shuffleNeighbor2Low64BitsToLow16_8(row0), shuffleNeighbor2Low64BitsToLow16_8(row1));
	const __m128i sumHigh = _mm_add_epi16(shuffleNeighbor2High64BitsToLow16_8(row0), shuffleNeighbor2High64BitsToLow16_8(row1));

	// add neighboring 16 bit elements together to new 16 bit elements and add 2 for rounding to each new element
	const __m128i sum = _mm_add_epi16(_mm_hadd_epi16(sumLow, sumHigh), _mm_set1_epi32(int(0x00020002)));

	// divide by 4 by right shifting of two bits
	const __m128i division16 = _mm_srli_epi16(sum, 2);

	// shift the lower 8 bit of the eight 16 bit values to the lower 64 bit
	const __m128i division8 = moveLowBits16_8ToLow64(division16);

	// copy the lower 64 bit to the memory
	_mm_storel_epi64((__m128i*)result, division8);
}

inline void SSE::average32Elements2Channel16Bit2x2(const uint8_t* const image0, const uint8_t* const image1, uint8_t* const result)
{
	ocean_assert(image0 && image1);

	// first 16 elements: 16 * uchar = m128i
	const __m128i row0 = _mm_lddqu_si128((__m128i*)image0);
	const __m128i row1 = _mm_lddqu_si128((__m128i*)image1);

	// distribute the 8 elements of 8 bit values into 8 elements of 16 bit values
	const __m128i sumLow = _mm_add_epi16(shuffleNeighbor2Low64BitsToLow16_8(row0), shuffleNeighbor2Low64BitsToLow16_8(row1));
	const __m128i sumHigh = _mm_add_epi16(shuffleNeighbor2High64BitsToLow16_8(row0), shuffleNeighbor2High64BitsToLow16_8(row1));

	// add neighboring 16 bit elements together to new 16 bit elements and add 2 for rounding to each new element
	const __m128i sum = _mm_add_epi16(_mm_hadd_epi16(sumLow, sumHigh), _mm_set1_epi32(int(0x00020002)));

	// divide by 4 by right shifting of two bits
	const __m128i division16 = _mm_srli_epi16(sum, 2);

	// shift the lower 8 bit of the eight 16 bit values to the lower 64 bit
	const __m128i firstDivision8 = moveLowBits16_8ToLow64(division16);

	// second 16 elements
	const __m128i secondRow0 = _mm_lddqu_si128((__m128i*)(image0 + 16));
	const __m128i secondRow1 = _mm_lddqu_si128((__m128i*)(image1 + 16));

	// distribute the 8 elements of 8 bit values into 8 elements of 16 bit values
	const __m128i secondSumLow = _mm_add_epi16(shuffleNeighbor2Low64BitsToLow16_8(secondRow0), shuffleNeighbor2Low64BitsToLow16_8(secondRow1));
	const __m128i secondSumHigh = _mm_add_epi16(shuffleNeighbor2High64BitsToLow16_8(secondRow0), shuffleNeighbor2High64BitsToLow16_8(secondRow1));

	// add neighboring 16 bit elements together to new 16 bit elements and add 2 for rounding to each new element
	const __m128i secondSum = _mm_add_epi16(_mm_hadd_epi16(secondSumLow, secondSumHigh), _mm_set1_epi32(int(0x00020002)));

	// divide by 4 by right shifting of two bits
	const __m128i secondDivision16 = _mm_srli_epi16(secondSum, 2);

	// shift the lower 8 bit of the eight 16 bit values to the higher 64 bit
	const __m128i secondDivision8 = moveLowBits16_8ToHigh64(secondDivision16);


	// combine both divion results
	const __m128i division8 = _mm_or_si128(firstDivision8, secondDivision8);

	// copy the 128 bit to the memory
	_mm_storeu_si128((__m128i*)result, division8);
}

inline void SSE::average6Elements3Channel96Bit2x2(const float* const image0, const float* const image1, float* const result)
{
	ocean_assert(image0 && image1 && result);

	// 6 * float = 2 pixel: 00 01 02 03 04 05

	// load element 0 up to 3, input does not need to be aligned on any particular boundary.
	const __m128 row0 = _mm_loadu_ps(image0);
	const __m128 row1 = _mm_loadu_ps(image1);

	// get sum of first 4 elements
	const __m128 sumFirst = _mm_add_ps(row0, row1);

	// load element 2 up to 5 to prevent that we access memory out of our range
	const __m128 rowSecond0 = _mm_loadu_ps(image0 + 2);
	const __m128 rowSecond1 = _mm_loadu_ps(image1 + 2);

	// get sum of second 4 elements
	const __m128 sumSecond = _mm_add_ps(rowSecond0, rowSecond1);

	// get sum of summed pixels
	// NOTE: _mm_shuffle_ps resulting first 64bit are always from first __m128, second 64bit from second __m128
	// mask111001 = 57u; // 'i+1'th float became 'i'
	const __m128 sumComponents = _mm_add_ps(sumFirst, _mm_shuffle_ps(sumSecond, sumSecond, 57u));

	// divide by 4 --> multiply by 0.25
	const __m128 division =  _mm_mul_ps(sumComponents, _mm_set_ps1(0.25f));

	// store 3 elements (96 bit) to the memory

#ifdef OCEAN_COMPILER_MSC
	memcpy(result, &division.m128_f32[0], sizeof(float) * 3);
#else
	memcpy(result, &division, sizeof(float) * 3);
#endif
}

inline void SSE::average24Elements3Channel24Bit2x2(const uint8_t* const image0, const uint8_t* const image1, uint8_t* const result)
{
	ocean_assert(image0 && image1 && result);

	__m128i row0 = _mm_lddqu_si128((__m128i*)image0);
	__m128i row1 = _mm_lddqu_si128((__m128i*)image1);

	// distribute the first 12 elements (element 00 up to 11):
	// 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
	//
	// -- -- -- -- -- 08 -- 07 -- 06 -- 02 -- 01 -- 00
	// -- -- -- -- -- 11 -- 10 -- 09 -- 05 -- 04 -- 03

	__m128i shuffleMaskLow =  set128i(0xA0A0A0A0A008A007ull, 0xA006A002A001A000ull);
	__m128i shuffleMaskHigh = set128i(0xA0A0A0A0A00BA00Aull, 0xA009A005A004A003ull);

	__m128i sumLow = _mm_add_epi16(_mm_shuffle_epi8(row0, shuffleMaskLow), _mm_shuffle_epi8(row1, shuffleMaskLow));
	__m128i sumHigh = _mm_add_epi16(_mm_shuffle_epi8(row0, shuffleMaskHigh), _mm_shuffle_epi8(row1, shuffleMaskHigh));

	// add neighboring 16 bit elements together to new 16 bit elements and add 2 for rounding to each new element
	__m128i sum = _mm_add_epi16(_mm_add_epi16(sumLow, sumHigh), _mm_set1_epi32(int(0x00020002)));

	// divide by 4 by right shifting of two bits
	__m128i division16 = _mm_srli_epi16(sum, 2);

	// shift the lower 8 bit of the eight 16 bit values to the lower 64 bit
	__m128i division8 = _mm_shuffle_epi8(division16, set128i(0xA0A0A0A0A0A0A0A0ull, 0xA0A00A0806040200ull));


	// now we load the remaining 12 elements (however, this time we take element 04 up to 15 to prevent that we access memory out of our range)
	// 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
	//
	// -- -- -- -- -- 12 -- 11 -- 10 -- 06 -- 05 -- 04
	// -- -- -- -- -- 15 -- 14 -- 13 -- 09 -- 08 -- 07

	row0 = _mm_lddqu_si128((__m128i*)(image0 + 8));
	row1 = _mm_lddqu_si128((__m128i*)(image1 + 8));

	shuffleMaskLow =  set128i(0xA0A0A0A0A00CA00Bull, 0xA00AA006A005A004ull);
	shuffleMaskHigh = set128i(0xA0A0A0A0A00FA00Eull, 0xA00DA009A008A007ull);

	sumLow = _mm_add_epi16(_mm_shuffle_epi8(row0, shuffleMaskLow), _mm_shuffle_epi8(row1, shuffleMaskLow));
	sumHigh = _mm_add_epi16(_mm_shuffle_epi8(row0, shuffleMaskHigh), _mm_shuffle_epi8(row1, shuffleMaskHigh));

	// add neighboring 16 bit elements together to new 16 bit elements and add 2 for rounding to each new element
	sum = _mm_add_epi16(_mm_add_epi16(sumLow, sumHigh), _mm_set1_epi32(int(0x00020002)));

	// divide by 4 by right shifting of two bits
	division16 = _mm_srli_epi16(sum, 2);

	// shift the lower 8 bit of the eight 16 bit values to the lower 64 bit
	division8 = _mm_or_si128(division8, _mm_shuffle_epi8(division16, set128i(0xA0A0A0A00A080604ull, 0x0200A0A0A0A0A0A0ull)));

#ifdef OCEAN_COMPILER_MSC
	memcpy(result, &division8.m128i_u8[0], 12);
#else
	memcpy(result, &division8, 12);
#endif
}

inline void SSE::average8Elements4Channel128Bit2x2(const float* const image0, const float* const image1, float* const result)
{
	ocean_assert(image0 && image1);

	// 4 * float = m128, input does not need to be aligned on any particular boundary.
	const __m128 row0 = _mm_loadu_ps(image0);
	const __m128 row1 = _mm_loadu_ps(image1);

	// get sum of first 4 elements
	const __m128 sumFirstPixel = _mm_add_ps(row0, row1);

	// load next 4 elements
	const __m128 rowSecond0 = _mm_loadu_ps(image0 + 4);
	const __m128 rowSecond1 = _mm_loadu_ps(image1 + 4);

	// get sum of second 4 elements
	const __m128 sumSecondPixel = _mm_add_ps(rowSecond0, rowSecond1);

	// get sum of summed pixels
	const __m128 sumComponents = _mm_add_ps(sumFirstPixel, sumSecondPixel);

	// divide by 4 --> multiply by 0.25
	const __m128 division =  _mm_mul_ps(sumComponents, _mm_set_ps1(0.25f));

	// store 4 elements (128 bit) to the memory, output does not need to be aligned on any particular boundary.
	_mm_storeu_ps(result, division);
}

inline void SSE::average16Elements4Channel32Bit2x2(const uint8_t* const image0, const uint8_t* const image1, uint8_t* const result)
{
	ocean_assert(image0 && image1);

	const __m128i row0 = _mm_lddqu_si128((__m128i*)image0);
	const __m128i row1 = _mm_lddqu_si128((__m128i*)image1);

	// distribute the 8 elements of 8 bit values into 8 elements of 16 bit values
	const __m128i sumLow = _mm_add_epi16(shuffleNeighbor4Low64BitsToLow16_8(row0), shuffleNeighbor4Low64BitsToLow16_8(row1));
	const __m128i sumHigh = _mm_add_epi16(shuffleNeighbor4High64BitsToLow16_8(row0), shuffleNeighbor4High64BitsToLow16_8(row1));

	// add neighboring 16 bit elements together to new 16 bit elements and add 2 for rounding to each new element
	const __m128i sum = _mm_add_epi16(_mm_hadd_epi16(sumLow, sumHigh), _mm_set1_epi32(int(0x00020002)));

	// divide by 4 by right shifting of two bits
	const __m128i division16 = _mm_srli_epi16(sum, 2);

	// shift the lower 8 bit of the eight 16 bit values to the lower 64 bit
	const __m128i division8 = moveLowBits16_8ToLow64(division16);

	// copy the lower 64 bit to the memory
	_mm_storel_epi64((__m128i*)result, division8);
}

inline void SSE::average32Elements4Channel32Bit2x2(const uint8_t* const image0, const uint8_t* const image1, uint8_t* const result)
{
	ocean_assert(image0 && image1);

	// first 16 elements
	const __m128i firstRow0 = _mm_lddqu_si128((__m128i*)image0);
	const __m128i firstRow1 = _mm_lddqu_si128((__m128i*)image1);

	// distribute the 8 elements of 8 bit values into 8 elements of 16 bit values
	const __m128i firstSumLow = _mm_add_epi16(shuffleNeighbor4Low64BitsToLow16_8(firstRow0), shuffleNeighbor4Low64BitsToLow16_8(firstRow1));
	const __m128i firstSumHigh = _mm_add_epi16(shuffleNeighbor4High64BitsToLow16_8(firstRow0), shuffleNeighbor4High64BitsToLow16_8(firstRow1));

	// add neighboring 16 bit elements together to new 16 bit elements and add 2 for rounding to each new element
	const __m128i firstSum = _mm_add_epi16(_mm_hadd_epi16(firstSumLow, firstSumHigh), _mm_set1_epi32(int(0x00020002)));

	// divide by 4 by right shifting of two bits
	const __m128i firstDivision16 = _mm_srli_epi16(firstSum, 2);

	// shift the lower 8 bit of the eight 16 bit values to the lower 64 bit
	const __m128i firstDivision8 = moveLowBits16_8ToLow64(firstDivision16);


	// second 16 elements
	const __m128i secondRow0 = _mm_lddqu_si128((__m128i*)(image0 + 16));
	const __m128i secondRow1 = _mm_lddqu_si128((__m128i*)(image1 + 16));

	// distribute the 8 elements of 8 bit values into 8 elements of 16 bit values
	const __m128i secondSumLow = _mm_add_epi16(shuffleNeighbor4Low64BitsToLow16_8(secondRow0), shuffleNeighbor4Low64BitsToLow16_8(secondRow1));
	const __m128i secondSumHigh = _mm_add_epi16(shuffleNeighbor4High64BitsToLow16_8(secondRow0), shuffleNeighbor4High64BitsToLow16_8(secondRow1));

	// add neighboring 16 bit elements together to new 16 bit elements and add 2 for rounding to each new element
	const __m128i secondSum = _mm_add_epi16(_mm_hadd_epi16(secondSumLow, secondSumHigh), _mm_set1_epi32(int(0x00020002)));

	// divide by 4 by right shifting of two bits
	const __m128i secondDivision16 = _mm_srli_epi16(secondSum, 2);

	// shift the lower 8 bit of the eight 16 bit values to the higher 64 bit
	const __m128i secondDivision8 = moveLowBits16_8ToHigh64(secondDivision16);


	// combine both divion results
	const __m128i division8 = _mm_or_si128(firstDivision8, secondDivision8);

	// copy the 128 bit to the memory
	_mm_storeu_si128((__m128i*)result, division8);
}

inline void SSE::average30Elements1Channel8Bit3x3(const uint8_t* const image0, const uint8_t* const image1, const uint8_t* const image2, uint8_t* const result)
{
	ocean_assert(image0 && image1 && image2);

	/**
	 *		| 1 2 1 |
	 * 1/16 | 2 4 2 |
	 *		| 1 2 1 |
	 */

	// first 16 elements (actual 14 are used)
	const __m128i firstRow0 = _mm_lddqu_si128((__m128i*)image0);
	const __m128i firstRow1 = _mm_lddqu_si128((__m128i*)image1);
	const __m128i firstRow2 = _mm_lddqu_si128((__m128i*)image2);

	// distribute the 16 elements of 8 bit values into 16 elements of 16 bit values and create the sum, middle row is summed twice
	const __m128i firstSumEven = _mm_add_epi16(_mm_add_epi16(removeHighBits16_8(firstRow0), removeHighBits16_8(firstRow1)), _mm_add_epi16(removeHighBits16_8(firstRow1), removeHighBits16_8(firstRow2)));
	const __m128i firstSumOdd = _mm_add_epi16(_mm_add_epi16(moveHighBits16_8(firstRow0), moveHighBits16_8(firstRow1)), _mm_add_epi16(moveHighBits16_8(firstRow1), moveHighBits16_8(firstRow2)));

	// second 16 elements, starting from 15th element
	const __m128i secondRow0 = _mm_lddqu_si128((__m128i*)(image0 + 14));
	const __m128i secondRow1 = _mm_lddqu_si128((__m128i*)(image1 + 14));
	const __m128i secondRow2 = _mm_lddqu_si128((__m128i*)(image2 + 14));

	// distribute the 16 elements of 8 bit values into 16 elements of 16 bit values and create the sum, middle row is summed twice
	const __m128i secondSumEven = _mm_add_epi16(_mm_add_epi16(removeHighBits16_8(secondRow0), removeHighBits16_8(secondRow1)), _mm_add_epi16(removeHighBits16_8(secondRow1), removeHighBits16_8(secondRow2)));
	const __m128i secondSumOdd = _mm_add_epi16(_mm_add_epi16(moveHighBits16_8(secondRow0), moveHighBits16_8(secondRow1)), _mm_add_epi16(moveHighBits16_8(secondRow1), moveHighBits16_8(secondRow2)));

	// build overall sum and add 8 for rounding
	// positions 0, 2, 3, 5, 6 are valid, e.g. pos. 0 contains element00 + element01
	const __m128i firstSum = _mm_add_epi16(firstSumEven, _mm_add_epi16(firstSumOdd, _mm_set1_epi32(int(0x00080008))));
	// e.g. pos. 0 contains now element00 + element01 + element02
	const __m128i firstSumWithEven = _mm_add_epi16(firstSum, _mm_shuffle_epi8(firstSumEven, set128i(0xFFFF0F0E0B0AFFFFull, 0x09080504FFFF0302ull)));
	// e.g. pos. 0 contains now element00 + element01 + element02 + element01
	const __m128i firstSumWithBoth = _mm_add_epi16(firstSumWithEven, _mm_shuffle_epi8(firstSumOdd, set128i(0xFFFF0D0C0908FFFFull, 0x07060302FFFF0100ull)));

	// build overall sum and add 8 for rounding
	// positions 1, 2, 4, 5, 7 are valid
	const __m128i secondSum = _mm_add_epi16(secondSumEven, _mm_add_epi16(secondSumOdd, _mm_set1_epi32(int(0x00080008))));
	const __m128i secondSumWithEven = _mm_add_epi16(secondSum, _mm_shuffle_epi8(secondSumEven, set128i(0x0F0EFFFF0D0C0908ull, 0xFFFF07060302FFFFull)));
	const __m128i secondSumWithBoth = _mm_add_epi16(secondSumWithEven, _mm_shuffle_epi8(secondSumOdd, set128i(0x0D0CFFFF0B0A0706ull, 0xFFFF05040100FFFFull)));

	// divide by 16 by right shifting of four bits
	const __m128i firstDivision16 = _mm_srli_epi16(firstSumWithBoth, 4);
	const __m128i secondDivision16 = _mm_srli_epi16(secondSumWithBoth, 4);

	// reorder valid elements to lowest bits
	const __m128i firstDivision8 =  _mm_shuffle_epi8(firstDivision16, set128i(0xFFFFFFFFFFFFFFFFull, 0xFFFFFF0C0A060400ull));
	const __m128i secondDivision8 =  _mm_shuffle_epi8(secondDivision16, set128i(0xFFFFFFFFFFFF0E0Aull, 0x080402FFFFFFFFFFull));

	// combine both divion results
	const __m128i division8 = _mm_or_si128(firstDivision8, secondDivision8);

	// copy the lowest 10*8 bit to the memory
#ifdef OCEAN_COMPILER_MSC
	memcpy(result, &division8.m128i_u8[0], 10);
#else
	memcpy(result, &division8, 10);
#endif
}

inline __m128i SSE::addOffsetBeforeRightShiftDivisionByTwoSigned16Bit(const __m128i& value)
{
	/**
	 * SSE does not have an intrinsic for integer division, so right bit shift is used instead.
	 * Unfortunately, for negative odd integer values v: (v / 2) != (v >> 1) because a right shift rounds towards negative infinity, e.g. -5 / 2 = -2 and -5 >> 1 = -3.
	 * As a work-around, an offset of 1 is added to all values that are both, negative and odd.
	 */

	// We create a bit mask for all 16 bit odd values, an odd value will create an active lower bit in each 16 bit value
	const __m128i maskOdds = _mm_and_si128(value, CV::SSE::set128i(0x0001000100010001ull, 0x0001000100010001ull));

	// We create a bit mask for all 16 bit negative values, a negative value will create an active lower bit in each 16 bit value
	const __m128i maskNegatives = _mm_srli_epi16(_mm_and_si128(value, CV::SSE::set128i(0x8000800080008000ull, 0x8000800080008000ull)), 15);

	// We add 1 to each 16 bit value having an active 'odd-bit' and active
    // 'negative-bit'
	return _mm_add_epi16(value, _mm_and_si128(maskNegatives, maskOdds));
}

inline __m128i SSE::addOffsetBeforeRightShiftDivisionSigned16Bit(const __m128i& value, const unsigned int rightShifts)
{
	ocean_assert(rightShifts < 16u);

	// the offset for negative values: 2^shifts - 1
	const __m128i offsetForNegatives_s_16x8 = _mm_set1_epi16(short((1u << rightShifts) - 1u));

	// bit mask for all 16 bit negative values
	const __m128i maskHigh_s_16x8 = CV::SSE::set128i(0x8000800080008000ull, 0x8000800080008000ull);

	// 0x0000 for positive values, 0xFFFF for negative values
	const __m128i maskNegativeValues_s_16x8 = _mm_cmpeq_epi16(_mm_and_si128(value, maskHigh_s_16x8), maskHigh_s_16x8);

	// 0 for positive values, 2^shifts - 1 for negative values
	const __m128i offset_s_16x8 = _mm_and_si128(offsetForNegatives_s_16x8, maskNegativeValues_s_16x8);

	return _mm_add_epi16(value, offset_s_16x8);
}

inline __m128i SSE::divideByRightShiftSigned16Bit(const __m128i& value, const unsigned int rightShifts)
{
	return _mm_srai_epi16(addOffsetBeforeRightShiftDivisionSigned16Bit(value, rightShifts), int(rightShifts));
}

inline __m128i SSE::addOffsetBeforeRightShiftDivisionByTwoSigned32Bit(const __m128i& value)
{
	/**
	 * SSE does not have an intrinsic for integer division, so right bit shift is used instead.
	 * Unfortunately, for negative odd integer values v: (v / 2) != (v >> 1) because a right shift rounds towards negative infinity, e.g. -5 / 2 = -2 and -5 >> 1 = -3.
	 * As a work-around, an offset of 1 is added to all values that are both, negative and odd.
	 */

	// We create a bit mask for all 32 bit odd values, an odd value will create an active lower bit in each 32 bit value
	const __m128i maskOdds = _mm_and_si128(value, CV::SSE::set128i(0x0000000100000001ull, 0x0000000100000001ull));

	// We create a bit mask for all 32 bit negative values, a negative value will create an active lower bit in each 32 bit value
	const __m128i maskNegatives = _mm_srli_epi32(_mm_and_si128(value, CV::SSE::set128i(0x8000000080000000ull, 0x8000000080000000ull)), 31);

	// We add 1 to each 32 bit value having an active 'odd-bit' and active 'negative-bit'
	return _mm_add_epi32(value, _mm_and_si128(maskNegatives, maskOdds));
}

inline __m128i SSE::addOffsetBeforeRightShiftDivisionSigned32Bit(const __m128i& value, const unsigned int rightShifts)
{
	ocean_assert(rightShifts < 32u);

	// the offset for negative values: 2^shifts - 1
	const __m128i offsetForNegatives_s_32x4 = _mm_set1_epi32(int((1u << rightShifts) - 1u));

	// bit mask for all 32 bit negative values
	const __m128i maskHigh_s_32x4 = CV::SSE::set128i(0x8000000080000000ull, 0x8000000080000000ull);

	// 0x00000000 for positive values, 0xFFFFFFFF for negative values
	const __m128i maskNegativeValues_s_32x4 = _mm_cmpeq_epi32(_mm_and_si128(value, maskHigh_s_32x4), maskHigh_s_32x4);

	// 0 for positive values, 2^shifts - 1 for negative values
	const __m128i offset_s_32x4 = _mm_and_si128(offsetForNegatives_s_32x4, maskNegativeValues_s_32x4);

	return _mm_add_epi32(value, offset_s_32x4);
}

inline __m128i SSE::divideByRightShiftSigned32Bit(const __m128i& value, const unsigned int rightShifts)
{
	return _mm_srai_epi32(addOffsetBeforeRightShiftDivisionSigned32Bit(value, rightShifts), int(rightShifts));
}

inline void SSE::gradientHorizontalVertical8Elements1Channel8Bit(const uint8_t* source, int8_t* response, const unsigned int width)
{
	ocean_assert(source && response && width >= 10u);

	// Load 16 unsigned 8-bit values; left/right/top/bottom pixels
	const __m128i horizontalMinus = _mm_lddqu_si128((__m128i*)(source - 1));
	const __m128i horizontalPlus  = _mm_lddqu_si128((__m128i*)(source + 1));

	const __m128i verticalMinus = _mm_lddqu_si128((__m128i*)(source - width));
	const __m128i verticalPlus  = _mm_lddqu_si128((__m128i*)(source + width));

	// Convert the above values to signed 16-bit values and split them into a low and high half (shuffle). Use zero padding to fill the 16-bit result (0x80).
	const __m128i horizontalMinusLo = _mm_cvtepu8_epi16(horizontalMinus); // Specialized function since SSE 4.1; no equivalent for the upper half
	//const __m128i horizontalMinusLo = _mm_shuffle_epi8(horizontalMinus, set128i(0x8007800680058004ull, 0x8003800280018000ull));
	const __m128i horizontalMinusHi = _mm_shuffle_epi8(horizontalMinus, set128i(0x800F800E800D800Cull, 0x800B800A80098008ull));

	const __m128i horizontalPlusLo = _mm_cvtepu8_epi16(horizontalPlus); // Specialized function since SSE 4.1; no equivalent for the upper half
	//const __m128i horizontalPlusLo = _mm_shuffle_epi8(horizontalPlus, set128i(0x8007800680058004ull, 0x8003800280018000ull));
	const __m128i horizontalPlusHi = _mm_shuffle_epi8(horizontalPlus, set128i(0x800F800E800D800Cull, 0x800B800A80098008ull));

	// Take the signed difference (right - left) and divide by two to fit values into the range [-128, 127]. (Integer) division by right shifting values by one position.
	const __m128i horizontalGradientLo = _mm_srai_epi16(addOffsetBeforeRightShiftDivisionByTwoSigned16Bit(_mm_sub_epi16(horizontalPlusLo, horizontalMinusLo)), 1);
	const __m128i horizontalGradientHi = _mm_srai_epi16(addOffsetBeforeRightShiftDivisionByTwoSigned16Bit(_mm_sub_epi16(horizontalPlusHi, horizontalMinusHi)), 1);

	// Convert the low and high signed 16-bit differences to signed 8-bit and merge them into a single
	const __m128i horizontalGradient = _mm_or_si128(
	    _mm_shuffle_epi8(horizontalGradientLo, set128i(0x8080808080808080ull, 0x0E0C0A0806040200ull)),
	    _mm_shuffle_epi8(horizontalGradientHi, set128i(0x0E0C0A0806040200ull, 0x8080808080808080ull)));

	// Convert the above values to signed 16-bit values and split them into a low and high half (shuffle). Use zero padding to fill the 16-bit result (0x80).
	const __m128i verticalMinusLo = _mm_cvtepu8_epi16(verticalMinus); // Specialized function since SSE 4.1; no equivalent for the upper half
	//const __m128i verticalMinusLo = _mm_shuffle_epi8(verticalMinus, set128i(0x8007800680058004ull, 0x8003800280018000ull)); // == a[7:0]
	const __m128i verticalMinusHi = _mm_shuffle_epi8(verticalMinus, set128i(0x800F800E800D800Cull, 0x800B800A80098008ull));

	const __m128i verticalPlusLo = _mm_cvtepu8_epi16(verticalPlus); // Specialized function since SSE 4.1; no equivalent for the upper half
	//const __m128i verticalPlusLo = _mm_shuffle_epi8(verticalPlus, set128i(0x8007800680058004ull, 0x8003800280018000ull)); // == b[7:0]
	const __m128i verticalPlusHi = _mm_shuffle_epi8(verticalPlus, set128i(0x800F800E800D800Cull, 0x800B800A80098008ull));

	// Take the signed difference (bottom - top) and divide by two to fit values into the range [-128, 127]. (Integer) division by right shifting values by one position.
	const __m128i verticalGradientLo = _mm_srai_epi16(addOffsetBeforeRightShiftDivisionByTwoSigned16Bit(_mm_sub_epi16(verticalPlusLo, verticalMinusLo)), 1);
	const __m128i verticalGradientHi = _mm_srai_epi16(addOffsetBeforeRightShiftDivisionByTwoSigned16Bit(_mm_sub_epi16(verticalPlusHi, verticalMinusHi)), 1);

	// Convert the differences to signed char and merge the high and low halves
	const __m128i verticalGradient = _mm_or_si128(
	    _mm_shuffle_epi8(verticalGradientLo, set128i(0x8080808080808080ull, 0x0E0C0A0806040200ull)),
	    _mm_shuffle_epi8(verticalGradientHi, set128i(0x0E0C0A0806040200ull, 0x8080808080808080ull)));

	// Take the horizontal gradients, [dx0, dx1, dx2, ...], and the vertical gradient, [dy0, dy1, dy2, ...] and interleave them, [dx0, dy0, dx1, dy1, dx2, dy2, ...]
	const __m128i interleavedResponseLo = _mm_unpacklo_epi8(horizontalGradient, verticalGradient);
	const __m128i interleavedResponseHi = _mm_unpackhi_epi8(horizontalGradient, verticalGradient);

	ocean_assert(sizeof(char) == 1ull);
	_mm_storeu_si128((__m128i*)response, interleavedResponseLo);
	_mm_storeu_si128((__m128i*)(response + 16ull), interleavedResponseHi);
}

inline void SSE::gradientHorizontalVertical8Elements3Products1Channel8Bit(const uint8_t* source, int16_t* response, const unsigned int width)
{
	ocean_assert(source && response && width >= 10u);

	// Load 4x(16x8u) values: left/right/top/bottom pixels
	const __m128i horizontalMinus = _mm_lddqu_si128((__m128i*)(source - 1));
	const __m128i horizontalPlus  = _mm_lddqu_si128((__m128i*)(source + 1));

	const __m128i verticalMinus = _mm_lddqu_si128((__m128i*)(source - width));
	const __m128i verticalPlus  = _mm_lddqu_si128((__m128i*)(source + width));

	// Convert the above values to signed 16-bit values and split them into a low and high half (shuffle). Use zero padding to fill the 16-bit result (0x80).
	const __m128i horizontalMinusLo = _mm_cvtepu8_epi16(horizontalMinus); // Specialized function since SSE 4.1; no equivalent for the upper half
	const __m128i horizontalMinusHi = _mm_shuffle_epi8(horizontalMinus, set128i(0x800F800E800D800Cull, 0x800B800A80098008ull));

	const __m128i horizontalPlusLo = _mm_cvtepu8_epi16(horizontalPlus); // Specialized function since SSE 4.1; no equivalent for the upper half
	const __m128i horizontalPlusHi = _mm_shuffle_epi8(horizontalPlus, set128i(0x800F800E800D800Cull, 0x800B800A80098008ull));

	// Take the signed difference (right - left) and divide by two to fit values into the range [-128, 127]. (Integer) division by right shifting values by one position.
	const __m128i horizontalGradientLo = _mm_srai_epi16(addOffsetBeforeRightShiftDivisionByTwoSigned16Bit(_mm_sub_epi16(horizontalPlusLo, horizontalMinusLo)), 1);
	const __m128i horizontalGradientHi = _mm_srai_epi16(addOffsetBeforeRightShiftDivisionByTwoSigned16Bit(_mm_sub_epi16(horizontalPlusHi, horizontalMinusHi)), 1);

	// Convert the above values to signed 16-bit values and split them into a low and high half (shuffle). Use zero padding to fill the 16-bit result (0x80).
	const __m128i verticalMinusLo = _mm_cvtepu8_epi16(verticalMinus); // Specialized function since SSE 4.1; no equivalent for the upper half
	const __m128i verticalMinusHi = _mm_shuffle_epi8(verticalMinus, set128i(0x800F800E800D800Cull, 0x800B800A80098008ull));

	const __m128i verticalPlusLo = _mm_cvtepu8_epi16(verticalPlus); // Specialized function since SSE 4.1; no equivalent for the upper half
	const __m128i verticalPlusHi = _mm_shuffle_epi8(verticalPlus, set128i(0x800F800E800D800Cull, 0x800B800A80098008ull));

	// Take the signed difference (bottom - top) and divide by two to fit values into the range [-128, 127]. (Integer) division by right shifting values by one position.
	const __m128i verticalGradientLo = _mm_srai_epi16(addOffsetBeforeRightShiftDivisionByTwoSigned16Bit(_mm_sub_epi16(verticalPlusLo, verticalMinusLo)), 1);
	const __m128i verticalGradientHi = _mm_srai_epi16(addOffsetBeforeRightShiftDivisionByTwoSigned16Bit(_mm_sub_epi16(verticalPlusHi, verticalMinusHi)), 1);

	// Squared gradients: h*h, v*v, h*v
	const __m128i horizontalHorizontalLo = _mm_mullo_epi16(horizontalGradientLo, horizontalGradientLo);
	const __m128i horizontalHorizontalHi = _mm_mullo_epi16(horizontalGradientHi, horizontalGradientHi);

	const __m128i verticalVerticalLo = _mm_mullo_epi16(verticalGradientLo, verticalGradientLo);
	const __m128i verticalVerticalHi = _mm_mullo_epi16(verticalGradientHi, verticalGradientHi);

	const __m128i horzontalVerticalLo = _mm_mullo_epi16(horizontalGradientLo, verticalGradientLo);
	const __m128i horzontalVerticalHi = _mm_mullo_epi16(horizontalGradientHi, verticalGradientHi);

	// Interleave/pack the above squared gradient, 16S values
	//
	// a, b, c - Above variables ending in *Lo
	// d, e, f - Above variables ending in *Hi
	//
	// a = [a7, a6, a5, a4, a3, a2, a1, a0]
	// b = [b7, b6, b5, b4, b3, b2, b1, b0]
	// c = [c7, c6, c5, c4, c3, c2, c1, c0]
	//
	// d = [d7, d6, d5, d4, d3, d2, d1, d0]
	// e = [e7, e6, e5, e4, e3, e2, e1, e0]
	// f = [f7, f6, f5, f4, f3, f2, f1, f0]
	//
	// A = [b2, a2, c1, b1, a1, c0, b0, a0]
	// B = [a5, c4, b4, a4, c3, b3, a3, c2]
	// C = [c7, b7, a7, c6, b6, a6, c5, b5]
	//
	// D = [e2, d2, f1, e1, d1, f0, e0, d0]
	// E = [d5, f4, e4, d4, f3, e3, d3, f2]
	// F = [f7, e7, d7, f6, e6, d6, f5, e5]

    const __m128i block0Lo = _mm_or_si128(                                                                                // == [b2, a2, c1, b1, a1, c0, b0, a0]
                    _mm_or_si128(                                                                                         // == [b2, a2, 00, b1, a1, 00, b0, a0]
                        _mm_shuffle_epi8(horizontalHorizontalLo, set128i(0xFFFF0504FFFFFFFFull, 0x0302FFFFFFFF0100ull)),  // == [00, a2, 00, 00, a1, 00, 00, a0]
                        _mm_shuffle_epi8(verticalVerticalLo,     set128i(0x0504FFFFFFFF0302ull, 0xFFFFFFFF0100FFFFull))), // == [b2, 00, 00, b1, 00, 00, b0, 00]
                    _mm_shuffle_epi8(horzontalVerticalLo,        set128i(0xFFFFFFFF0302FFFFull, 0xFFFF0100FFFFFFFFull))); // == [00, 00, c1, 00, 00, c0, 00, 00]

    const __m128i block1Lo = _mm_or_si128(                                                                                // == [a5, c4, b4, a4, c3, b3, a3, c2]
                    _mm_or_si128(                                                                                         // == [a5, 00, b4, a4, 00, b3, a3, 00]
                        _mm_shuffle_epi8(horizontalHorizontalLo, set128i(0x0B0AFFFFFFFF0908ull, 0xFFFFFFFF0706FFFFull)),  // == [a5, 00, 00, a4, 00, 00, a4, 00]
                        _mm_shuffle_epi8(verticalVerticalLo,     set128i(0xFFFFFFFF0908FFFFull, 0xFFFF0706FFFFFFFFull))), // == [00, 00, b4, 00, 00, b3, 00, 00]
                    _mm_shuffle_epi8(horzontalVerticalLo,        set128i(0xFFFF0908FFFFFFFFull, 0x0706FFFFFFFF0504ull))); // == [00, c4, 00, 00, c3, 00, 00, c2]

    const __m128i block2Lo = _mm_or_si128(                                                                                // == [c7, b7, a7, c6, b6, a6, c5, b5]
                    _mm_or_si128(                                                                                         // == [00, b7, a7, 00, b6, a6, 00, b5]
                        _mm_shuffle_epi8(horizontalHorizontalLo, set128i(0xFFFFFFFF0F0EFFFFull, 0xFFFF0D0CFFFFFFFFull)),  // == [00, 00, a7, 00, 00, a6, 00, 00]
                        _mm_shuffle_epi8(verticalVerticalLo,     set128i(0xFFFF0F0EFFFFFFFFull, 0x0D0CFFFFFFFF0B0Aull))), // == [00, b7, 00, 00, b6, 00, 00, b5]
                    _mm_shuffle_epi8(horzontalVerticalLo,        set128i(0x0F0EFFFFFFFF0D0Cull, 0xFFFFFFFF0B0AFFFFull))); // == [c7, 00, 00, c6, 00, 00, c5, 00]

    const __m128i block0Hi = _mm_or_si128(                                                                                // == [e2, d2, f1, e1, d1, f0, e0, d0]
                    _mm_or_si128(                                                                                         // == [e2, d2, 00, e1, d1, 00, e0, d0]
                        _mm_shuffle_epi8(horizontalHorizontalHi, set128i(0xFFFF0504FFFFFFFFull, 0x0302FFFFFFFF0100ull)),  // == [00, d2, 00, 00, d1, 00, 00, d0]
                        _mm_shuffle_epi8(verticalVerticalHi,     set128i(0x0504FFFFFFFF0302ull, 0xFFFFFFFF0100FFFFull))), // == [e2, 00, 00, e1, 00, 00, e0, 00]
                    _mm_shuffle_epi8(horzontalVerticalHi,        set128i(0xFFFFFFFF0302FFFFull, 0xFFFF0100FFFFFFFFull))); // == [00, 00, f1, 00, 00, f0, 00, 00]

    const __m128i block1Hi = _mm_or_si128(                                                                                // == [d5, f4, e4, d4, f3, e3, d3, f2]
                    _mm_or_si128(                                                                                         // == [d5, 00, e4, d4, 00, e3, d3, 00]
                        _mm_shuffle_epi8(horizontalHorizontalHi, set128i(0x0B0AFFFFFFFF0908ull, 0xFFFFFFFF0706FFFFull)),  // == [d5, 00, 00, d4, 00, 00, d4, 00]
                        _mm_shuffle_epi8(verticalVerticalHi,     set128i(0xFFFFFFFF0908FFFFull, 0xFFFF0706FFFFFFFFull))), // == [00, 00, e4, 00, 00, e3, 00, 00]
                    _mm_shuffle_epi8(horzontalVerticalHi,        set128i(0xFFFF0908FFFFFFFFull, 0x0706FFFFFFFF0504ull))); // == [00, f4, 00, 00, f3, 00, 00, f2]

    const __m128i block2Hi = _mm_or_si128(                                                                                // == [f7, e7, d7, f6, e6, d6, f5, e5]
                    _mm_or_si128(                                                                                         // == [00, e7, d7, 00, e6, d6, 00, e5]
                        _mm_shuffle_epi8(horizontalHorizontalHi, set128i(0xFFFFFFFF0F0EFFFFull, 0xFFFF0D0CFFFFFFFFull)),  // == [00, 00, d7, 00, 00, d6, 00, 00]
                        _mm_shuffle_epi8(verticalVerticalHi,     set128i(0xFFFF0F0EFFFFFFFFull, 0x0D0CFFFFFFFF0B0Aull))), // == [00, e7, 00, 00, e6, 00, 00, e5]
                    _mm_shuffle_epi8(horzontalVerticalHi,        set128i(0x0F0EFFFFFFFF0D0Cull, 0xFFFFFFFF0B0AFFFFull))); // == [f7, 00, 00, f6, 00, 00, f5, 00]

	_mm_storeu_si128((__m128i*)response,           block0Lo);
	_mm_storeu_si128((__m128i*)(response + 8ull),  block1Lo);
	_mm_storeu_si128((__m128i*)(response + 16ull), block2Lo);
	_mm_storeu_si128((__m128i*)(response + 24ull), block0Hi);
	_mm_storeu_si128((__m128i*)(response + 32ull), block1Hi);
	_mm_storeu_si128((__m128i*)(response + 40ull), block2Hi);
}

OCEAN_FORCE_INLINE void SSE::deInterleave3Channel8Bit15Elements(const __m128i& interleaved, __m128i& channel01, __m128i& channel2)
{
	// interleaved R0 G0 B0 R1 G1 B1 R2 G2 B2 R3 G3 B3 R4 G4 B4 X

	// channel01 R0 R1 R2 R3 R4 X X X G0 G1 G2 G3 G4 X  X  X
	// channel2  B0 B1 B2 B3 B4 X X X 0  0  0  0  0  0  0  0

	channel01 = _mm_shuffle_epi8(interleaved, set128i(0xFFFFFF0d0a070401ull, 0xFFFFFF0c09060300ull));

	channel2 = _mm_shuffle_epi8(interleaved, set128i(0xFFFFFFFFFFFFFFFFull, 0xFFFFFF0e0b080502ull));
}

OCEAN_FORCE_INLINE void SSE::deInterleave3Channel8Bit24Elements(const __m128i& interleavedA, const __m128i& interleavedB, __m128i& channel01, __m128i& channel2)
{
	// interleavedA  R0 G0 B0 R1 G1 B1 R2 G2 B2 R3 G3 B3 R4 G4 B4 R5
	// interleavedB  G5 B5 R6 G6 B6 R7 G7 B7 X  X  X  X  X  X  X  X

	// channel01 R0 R1 R2 R3 R4 R5 R6 R7 G0 G1 G2 G3 G4 G5 G6 G7
	// channel2  B0 B1 B2 B3 B4 B5 B6 B7 0  0  0  0  0  0  0  0

	channel01 = _mm_or_si128(_mm_shuffle_epi8(interleavedA, set128i(0xFFFFFF0d0a070401ull, 0xFFFF0f0c09060300ull)),
								_mm_shuffle_epi8(interleavedB, set128i(0x060300FFFFFFFFFFull, 0x0502FFFFFFFFFFFFull)));

	channel2 = _mm_or_si128(_mm_shuffle_epi8(interleavedA, set128i(0xFFFFFFFFFFFFFFFFull, 0xFFFFFF0e0b080502ull)),
								_mm_shuffle_epi8(interleavedB, set128i(0xFFFFFFFFFFFFFFFFull, 0x070401FFFFFFFFFFull)));
}

OCEAN_FORCE_INLINE void SSE::deInterleave3Channel8Bit48Elements(const __m128i& interleavedA, const __m128i& interleavedB, const __m128i& interleavedC, __m128i& channel0, __m128i& channel1, __m128i& channel2)
{
	channel0 = _mm_or_si128(_mm_shuffle_epi8(interleavedA, set128i(0xFFFFFFFFFFFFFFFFull, 0xFFFF0f0c09060300ull)),
					_mm_or_si128(_mm_shuffle_epi8(interleavedB, set128i(0xFFFFFFFFFF0e0b08ull, 0x0502FFFFFFFFFFFFull)),
								 _mm_shuffle_epi8(interleavedC, set128i(0x0d0a070401FFFFFFull, 0xFFFFFFFFFFFFFFFFull))));

	channel1 = _mm_or_si128(_mm_shuffle_epi8(interleavedA, set128i(0xFFFFFFFFFFFFFFFFull, 0xFFFFFF0d0a070401ull)),
					_mm_or_si128(_mm_shuffle_epi8(interleavedB, set128i(0xFFFFFFFFFF0f0c09ull, 0x060300FFFFFFFFFFull)),
								_mm_shuffle_epi8(interleavedC, set128i(0x0e0b080502FFFFFFull, 0xFFFFFFFFFFFFFFFFull))));

	channel2 = _mm_or_si128(_mm_shuffle_epi8(interleavedA, set128i(0xFFFFFFFFFFFFFFFFull, 0xFFFFFF0e0b080502ull)),
					_mm_or_si128(_mm_shuffle_epi8(interleavedB, set128i(0xFFFFFFFFFFFF0d0aull, 0x070401FFFFFFFFFFull)),
								_mm_shuffle_epi8(interleavedC, set128i(0x0f0c09060300FFFFull, 0xFFFFFFFFFFFFFFFFull))));
}

inline void SSE::deInterleave3Channel8Bit48Elements(const uint8_t* interleaved, __m128i& channel0, __m128i& channel1, __m128i& channel2)
{
	ocean_assert(interleaved != nullptr);

	deInterleave3Channel8Bit48Elements(load128i(interleaved), load128i(interleaved + 16), load128i(interleaved + 32), channel0, channel1, channel2);
}

inline void SSE::deInterleave3Channel8Bit48Elements(const uint8_t* interleaved, uint8_t* channel0, uint8_t* channel1, uint8_t* channel2)
{
	ocean_assert(interleaved && channel0 && channel1 && channel2);

	__m128i channel0_128, channel1_128, channel2_128;
	deInterleave3Channel8Bit48Elements(load128i(interleaved), load128i(interleaved + 16), load128i(interleaved + 32), channel0_128, channel1_128, channel2_128);

	store128i(channel0_128, channel0);
	store128i(channel1_128, channel1);
	store128i(channel2_128, channel2);
}

inline void SSE::deInterleave3Channel8Bit45Elements(const uint8_t* interleaved, __m128i& channel0, __m128i& channel1, __m128i& channel2)
{
	ocean_assert(interleaved != nullptr);

	deInterleave3Channel8Bit48Elements(load128i(interleaved), load128i(interleaved + 16), _mm_srli_si128(load128i(interleaved + 29), 3), channel0, channel1, channel2);
}

OCEAN_FORCE_INLINE void SSE::interleave3Channel8Bit48Elements(const __m128i& channel0, const __m128i& channel1, const __m128i& channel2, __m128i& interleavedA, __m128i& interleavedB, __m128i& interleavedC)
{
	interleavedA = _mm_or_si128(_mm_shuffle_epi8(channel0, set128i(0x05FFFF04FFFF03FFull, 0xFF02FFFF01FFFF00ull)),
												_mm_or_si128(_mm_shuffle_epi8(channel1, set128i(0xFFFF04FFFF03FFFFull, 0x02FFFF01FFFF00FFull)),
															 _mm_shuffle_epi8(channel2, set128i(0xFF04FFFF03FFFF02ull, 0xFFFF01FFFF00FFFFull))));

	interleavedB = _mm_or_si128(_mm_shuffle_epi8(channel0, set128i(0xFF0AFFFF09FFFF08ull, 0xFFFF07FFFF06FFFFull)),
												_mm_or_si128(_mm_shuffle_epi8(channel1, set128i(0x0AFFFF09FFFF08FFull, 0xFF07FFFF06FFFF05ull)),
															 _mm_shuffle_epi8(channel2, set128i(0xFFFF09FFFF08FFFFull, 0x07FFFF06FFFF05FFull))));

	interleavedC = _mm_or_si128(_mm_shuffle_epi8(channel0, set128i(0xFFFF0FFFFF0EFFFFull, 0x0DFFFF0CFFFF0BFFull)),
												_mm_or_si128(_mm_shuffle_epi8(channel1, set128i(0xFF0FFFFF0EFFFF0Dull, 0xFFFF0CFFFF0BFFFFull)),
															 _mm_shuffle_epi8(channel2, set128i(0x0FFFFF0EFFFF0DFFull, 0xFF0CFFFF0BFFFF0Aull))));
}

OCEAN_FORCE_INLINE void SSE::interleave3Channel8Bit48Elements(const uint8_t* const channel0, const uint8_t* const channel1, const uint8_t* const channel2, uint8_t* const interleaved)
{
	ocean_assert(channel0 && channel1 && channel2 && interleaved);

	__m128i interleavedA_128, interleavedB_128, interleavedC_128;
	interleave3Channel8Bit48Elements(load128i(channel0), load128i(channel1), load128i(channel2), interleavedA_128, interleavedB_128, interleavedC_128);

	store128i(interleavedA_128, interleaved + 0);
	store128i(interleavedB_128, interleaved + 16);
	store128i(interleavedC_128, interleaved + 32);
}

OCEAN_FORCE_INLINE void SSE::reverseChannelOrder2Channel8Bit32Elements(const uint8_t* interleaved, uint8_t* reversedInterleaved)
{
	ocean_assert(interleaved != nullptr && reversedInterleaved != nullptr);

	//  input: 0 1 2 3  4 5 6 7  8 9 A B  C D E F
	//         Y A Y A  Y A Y A  Y A Y A  Y A Y A
	// output: A Y A Y  A Y A Y  A Y A Y  A Y A Y
	//         1 0 3 2  5 4 7 6  9 8 B A  D C F E

	const __m128i shuffleMask_u_16x8 = set128i(0x0E0F0C0D0A0B0809ull, 0x0607040502030001ull);

	store128i(_mm_shuffle_epi8(load128i(interleaved + 0), shuffleMask_u_16x8), reversedInterleaved + 0);
	store128i(_mm_shuffle_epi8(load128i(interleaved + 16), shuffleMask_u_16x8), reversedInterleaved + 16);
}

OCEAN_FORCE_INLINE void SSE::reverseChannelOrder3Channel8Bit48Elements(const __m128i& interleaved0, const __m128i& interleaved1, const __m128i& interleaved2, __m128i& reversedInterleaved0, __m128i& reversedInterleaved1, __m128i& reversedInterleaved2)
{
	reversedInterleaved0 = _mm_or_si128(_mm_shuffle_epi8(interleaved0, set128i(0xFF0c0d0e090a0b06ull, 0x0708030405000102ull)),
											_mm_shuffle_epi8(interleaved1, set128i(0x01FFFFFFFFFFFFFFull, 0xFFFFFFFFFFFFFFFFull)));

	reversedInterleaved1 = _mm_or_si128(_mm_shuffle_epi8(interleaved0, set128i(0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFFFFFF0fFFull)),
											_mm_or_si128(_mm_shuffle_epi8(interleaved1, set128i(0x0fFF0b0c0d08090aull, 0x050607020304FF00ull)),
											_mm_shuffle_epi8(interleaved2, set128i(0xFF00FFFFFFFFFFFFull, 0xFFFFFFFFFFFFFFFFull))));

	reversedInterleaved2 = _mm_or_si128(_mm_shuffle_epi8(interleaved1, set128i(0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFFFFFFFF0eull)),
											_mm_shuffle_epi8(interleaved2, set128i(0x0d0e0f0a0b0c0708ull, 0x09040506010203FFull)));
}

OCEAN_FORCE_INLINE void SSE::reverseChannelOrder3Channel8Bit48Elements(const uint8_t* interleaved, uint8_t* const reversedInterleaved)
{
	ocean_assert(interleaved != nullptr && reversedInterleaved != nullptr);

	__m128i reversedInterleaved0, reversedInterleaved1, reversedInterleaved2;
	reverseChannelOrder3Channel8Bit48Elements(load128i(interleaved), load128i(interleaved + 16), load128i(interleaved + 32), reversedInterleaved0, reversedInterleaved1, reversedInterleaved2);

	store128i(reversedInterleaved0, reversedInterleaved);
	store128i(reversedInterleaved1, reversedInterleaved + 16);
	store128i(reversedInterleaved2, reversedInterleaved + 32);
}

OCEAN_FORCE_INLINE void SSE::reverseChannelOrder4Channel8Bit64Elements(const uint8_t* interleaved, uint8_t* reversedInterleaved)
{
	ocean_assert(interleaved != nullptr && reversedInterleaved != nullptr);

	//  input: 0 1 2 3  4 5 6 7  8 9 A B  C D E F
	//         R G B A  R G B A  R G B A  R G B A
	// output: A B G R  A B G R  A B G R  A B G R
	//         3 2 1 0  7 6 5 4  B A 9 8  F E D C

	const __m128i shuffleMask_u_16x8 = set128i(0x0C0D0E0F08090A0Bull, 0x0405060700010203ull);

	store128i(_mm_shuffle_epi8(load128i(interleaved + 0), shuffleMask_u_16x8), reversedInterleaved + 0);
	store128i(_mm_shuffle_epi8(load128i(interleaved + 16), shuffleMask_u_16x8), reversedInterleaved + 16);
	store128i(_mm_shuffle_epi8(load128i(interleaved + 32), shuffleMask_u_16x8), reversedInterleaved + 32);
	store128i(_mm_shuffle_epi8(load128i(interleaved + 48), shuffleMask_u_16x8), reversedInterleaved + 48);
}

inline void SSE::reverseChannelOrder3Channel8Bit48Elements(uint8_t* interleaved)
{
	ocean_assert(interleaved);

	__m128i reversedInterleaved0, reversedInterleaved1, reversedInterleaved2;
	reverseChannelOrder3Channel8Bit48Elements(load128i(interleaved), load128i(interleaved + 16), load128i(interleaved + 32), reversedInterleaved0, reversedInterleaved1, reversedInterleaved2);

	store128i(reversedInterleaved0, interleaved);
	store128i(reversedInterleaved1, interleaved + 16);
	store128i(reversedInterleaved2, interleaved + 32);
}

inline void SSE::swapReversedChannelOrder3Channel8Bit48Elements(uint8_t* first, uint8_t* second)
{
	ocean_assert(first && second && first != second);

	__m128i first0, first1, first2;
	reverseChannelOrder3Channel8Bit48Elements(load128i(first), load128i(first + 16), load128i(first + 32), first0, first1, first2);

	__m128i second0, second1, second2;
	reverseChannelOrder3Channel8Bit48Elements(load128i(second), load128i(second + 16), load128i(second + 32), second0, second1, second2);

	store128i(first0, second);
	store128i(first1, second + 16);
	store128i(first2, second + 32);

	store128i(second0, first);
	store128i(second1, first + 16);
	store128i(second2, first + 32);
}

inline void SSE::reverseElements8Bit48Elements(const __m128i& elements0, const __m128i& elements1, const __m128i& elements2, __m128i& reversedElements0, __m128i& reversedElements1, __m128i& reversedElements2)
{
	const __m128i mask = set128i(0x0001020304050607ull, 0x08090a0b0c0d0e0full);

	reversedElements0 = _mm_shuffle_epi8(elements2, mask);
	reversedElements1 = _mm_shuffle_epi8(elements1, mask);
	reversedElements2 = _mm_shuffle_epi8(elements0, mask);
}

inline void SSE::reverseElements8Bit48Elements(const uint8_t* elements, uint8_t* reversedElements)
{
	ocean_assert(elements && reversedElements);

	__m128i reversedElements0, reversedElements1, reversedElements2;
	reverseElements8Bit48Elements(load128i(elements), load128i(elements + 16), load128i(elements + 32), reversedElements0, reversedElements1, reversedElements2);

	store128i(reversedElements0, reversedElements);
	store128i(reversedElements1, reversedElements + 16);
	store128i(reversedElements2, reversedElements + 32);
}

inline void SSE::reverseElements8Bit48Elements(uint8_t* elements)
{
	ocean_assert(elements);

	__m128i reversedElements0, reversedElements1, reversedElements2;
	reverseElements8Bit48Elements(load128i(elements), load128i(elements + 16), load128i(elements + 32), reversedElements0, reversedElements1, reversedElements2);

	store128i(reversedElements0, elements);
	store128i(reversedElements1, elements + 16);
	store128i(reversedElements2, elements + 32);
}

inline void SSE::swapReversedElements8Bit48Elements(uint8_t* first, uint8_t* second)
{
	ocean_assert(first && second && first != second);

	__m128i first0, first1, first2;
	reverseElements8Bit48Elements(load128i(first), load128i(first + 16), load128i(first + 32), first0, first1, first2);

	__m128i second0, second1, second2;
	reverseElements8Bit48Elements(load128i(second), load128i(second + 16), load128i(second + 32), second0, second1, second2);

	store128i(first0, second);
	store128i(first1, second + 16);
	store128i(first2, second + 32);

	store128i(second0, first);
	store128i(second1, first + 16);
	store128i(second2, first + 32);
}

inline void SSE::shiftChannelToFront4Channel32Bit(const uint8_t* elements, uint8_t* shiftedElements)
{
	ocean_assert(elements && shiftedElements);

	store128i(_mm_shuffle_epi8(load128i(elements), set128i(0x0c0f0e0d080b0a09ull, 0x0407060500030201ull)), shiftedElements);
}

inline void SSE::shiftAndMirrorChannelToFront4Channel32Bit(const uint8_t* elements, uint8_t* shiftedElements)
{
	ocean_assert(elements && shiftedElements);

	store128i(_mm_shuffle_epi8(load128i(elements), set128i(0x0003020104070605ull, 0x080b0a090c0f0e0dull)), shiftedElements);
}

inline void SSE::shiftChannelToBack4Channel32Bit(const uint8_t* elements, uint8_t* shiftedElements)
{
	ocean_assert(elements && shiftedElements);

	store128i(_mm_shuffle_epi8(load128i(elements), set128i(0x0e0d0c0f0a09080bull, 0x0605040702010003ull)), shiftedElements);
}

inline void SSE::shiftAndMirrorChannelToBack4Channel32Bit(const uint8_t* elements, uint8_t* shiftedElements)
{
	ocean_assert(elements && shiftedElements);

	store128i(_mm_shuffle_epi8(load128i(elements), set128i(0x0201000306050407ull, 0x0a09080b0e0d0c0full)), shiftedElements);
}

inline __m128i SSE::sum1Channel8Bit16Elements(const __m128i& elements)
{
	const __m128i zero = _mm_setzero_si128();
	const __m128i sum = _mm_sad_epu8(elements, zero);

	return _mm_add_epi32(_mm_srli_si128(sum, 8), sum);
}

inline __m128i SSE::sum1Channel8Bit16Elements(const uint8_t* elements)
{
	ocean_assert(elements != nullptr);

	return sum1Channel8Bit16Elements(load128i(elements));
}

template <bool tBufferHas16Bytes>
inline __m128i SSE::sum1Channel8BitFront15Elements(const uint8_t* elements)
{
	ocean_assert(elements != nullptr);
	return sum1Channel8Bit16Elements(load_u8_15_upper_zero<tBufferHas16Bytes>(elements));
}

inline __m128i SSE::sum1Channel8BitBack15Elements(const uint8_t* elements)
{
	ocean_assert(elements != nullptr);
	return sum1Channel8Bit16Elements(load_u8_16_and_shift_right<1u>(elements));
}

inline __m128i SSE::sumInterleave3Channel8Bit48Elements(const __m128i& interleaved0, const __m128i& interleaved1, const __m128i& interleaved2)
{
	// Interleaved0: R BGR BGR BGR BGR BGR
	// Interleaved1: GR BGR BGR BGR BGR BG
	// Interleaved2: BGR BGR BGR BGR BGR B

	// BBBBBBBB RRRRRRRR
	const __m128i channel0_2First = _mm_or_si128(_mm_shuffle_epi8(interleaved0, set128i(0xFFFFFF0e0b080502ull, 0xFFFF0f0c09060300ull)),
													_mm_shuffle_epi8(interleaved1, set128i(0x070401FFFFFFFFFFull, 0x0502FFFFFFFFFFFFull)));

	// BBBBBBBB RRRRRRRR
	const __m128i channel0_2Second = _mm_or_si128(_mm_shuffle_epi8(interleaved1, set128i(0xFFFFFFFFFFFF0d0aull, 0xFFFFFFFFFF0e0b08ull)),
													_mm_shuffle_epi8(interleaved2, set128i(0x0f0c09060300FFFFull, 0x0d0a070401FFFFFFull)));

	// GGGGGGGG GGGGGGGG
	const __m128i channel1 = _mm_or_si128(_mm_shuffle_epi8(interleaved0, set128i(0xFFFFFFFFFFFFFFFFull, 0xFFFFFF0d0a070401ull)),
					_mm_or_si128(_mm_shuffle_epi8(interleaved1, set128i(0xFFFFFFFFFF0f0c09ull, 0x060300FFFFFFFFFFull)),
								_mm_shuffle_epi8(interleaved2, set128i(0x0e0b080502FFFFFFull, 0xFFFFFFFFFFFFFFFFull))));

	const __m128i zero = _mm_setzero_si128();

	// 0000 BBBB 0000 RRRR
	const __m128i sum0_2 = _mm_add_epi32(_mm_sad_epu8(channel0_2First, zero), _mm_sad_epu8(channel0_2Second, zero));

	// 0000 GGGG 0000 GGGG
	const __m128i sum1 = _mm_sad_epu8(channel1, zero);

	// 0000 BBBB GGGG RRRR
	return _mm_blend_epi16(sum0_2, _mm_add_epi32(_mm_slli_si128(sum1, 4), _mm_srli_si128(sum1, 4)), int(0xC));
}

inline __m128i SSE::sumInterleave3Channel8Bit48Elements(const uint8_t* interleaved)
{
	ocean_assert(interleaved != nullptr);

	return sumInterleave3Channel8Bit48Elements(load128i(interleaved), load128i(interleaved + 16), load128i(interleaved + 32));
}

inline __m128i SSE::sumInterleave3Channel8Bit45Elements(const uint8_t* interleaved)
{
	ocean_assert(interleaved != nullptr);

	return sumInterleave3Channel8Bit48Elements(load128i(interleaved), load128i(interleaved + 16), _mm_srli_si128(load128i(interleaved + 29), 3));
}

inline __m128i SSE::load128iLower64(const void* const buffer)
{
	ocean_assert(buffer != nullptr);
	return _mm_loadl_epi64((const __m128i*)(buffer));
}

inline __m128i SSE::load128i(const void* const buffer)
{
	ocean_assert(buffer != nullptr);
	return _mm_lddqu_si128((const __m128i*)(buffer));
}

template <bool tBufferHas16Bytes>
inline __m128i SSE::load_u8_10_upper_zero(const uint8_t* const buffer)
{
	ocean_assert(buffer != nullptr);

	__m128i result;

#ifdef OCEAN_COMPILER_MSC

	result.m128i_u64[0] = uint64_t(0);
	memcpy(result.m128i_u16 + 3, buffer + 0, sizeof(uint16_t));
	memcpy(result.m128i_u64 + 1, buffer + 2, sizeof(uint64_t));

#else

	M128i& ourResult = *((M128i*)(&result));

	ourResult.m128i_u64[0] = uint64_t(0);
	memcpy(ourResult.m128i_u16 + 3, buffer + 0, sizeof(uint16_t));
	memcpy(ourResult.m128i_u64 + 1, buffer + 2, sizeof(uint64_t));

#endif

	return result;
}

template <>
inline __m128i SSE::load_u8_10_upper_zero<true>(const uint8_t* const buffer)
{
	ocean_assert(buffer != nullptr);

	// we load 16 bytes and shift the SSE register by 6 byte afterwards
	return _mm_slli_si128(SSE::load128i(buffer), 6);
}

template <bool tBufferHas16Bytes>
inline __m128i SSE::load_u8_15_upper_zero(const uint8_t* const buffer)
{
	ocean_assert(buffer != nullptr);

	__m128i intermediate;
	memcpy(&intermediate, buffer, 15);

	// we shift the SSE register by 1 byte afterwards
	return _mm_slli_si128(intermediate, 1);
}

template <>
inline __m128i SSE::load_u8_15_upper_zero<true>(const uint8_t* const buffer)
{
	ocean_assert(buffer != nullptr);

	// we load 16 bytes and shift the SSE register by 1 byte afterwards
	return _mm_slli_si128(_mm_lddqu_si128((__m128i*)(buffer)), 1);
}

template <bool tBufferHas16Bytes>
inline __m128i SSE::load_u8_13_lower_random(const uint8_t* const buffer)
{
	ocean_assert(buffer != nullptr);

	__m128i result;
	memcpy(&result, buffer, 13);

	return result;
}

template <>
inline __m128i SSE::load_u8_13_lower_random<true>(const uint8_t* const buffer)
{
	ocean_assert(buffer != nullptr);

	// we load the entire 16 bytes to the 128i value as this is the fastest way
	return _mm_lddqu_si128((__m128i*)(buffer));
}

template <bool tBufferHas16Bytes>
inline __m128i SSE::load_u8_15_lower_zero(const uint8_t* const buffer)
{
	ocean_assert(buffer != nullptr);

	__m128i result;
	memcpy(&result, buffer, 15);

#ifdef OCEAN_COMPILER_MSC
	result.m128i_u8[15] = 0u;
#else
	((M128i&)result).m128i_u8[15] = 0u;
#endif

	return result;
}

template <>
inline __m128i SSE::load_u8_15_lower_zero<true>(const uint8_t* const buffer)
{
	ocean_assert(buffer != nullptr);

	// we load the entire 16 bytes to the 128i value as this is the fastest way
	__m128i result = _mm_lddqu_si128((__m128i*)(buffer));

#ifdef OCEAN_COMPILER_MSC
	result.m128i_u8[15] = 0u;
#else
	((M128i&)result).m128i_u8[15] = 0u;
#endif

	return result;
}

template <bool tBufferHas16Bytes>
inline __m128i SSE::load_u8_15_lower_random(const uint8_t* const buffer)
{
	ocean_assert(buffer != nullptr);

	__m128i result;
	memcpy(&result, buffer, 15);

	return result;
}

template <>
inline __m128i SSE::load_u8_15_lower_random<true>(const uint8_t* const buffer)
{
	ocean_assert(buffer != nullptr);

	// we load the entire 16 bytes to the 128i value as this is the fastest way
	return _mm_lddqu_si128((__m128i*)(buffer));
}

template <unsigned int tShiftBytes>
inline __m128i SSE::load_u8_16_and_shift_right(const uint8_t* const buffer)
{
	static_assert(tShiftBytes <= 16u, "Invalid shift!");

	ocean_assert(buffer != nullptr);
	return _mm_srli_si128(_mm_lddqu_si128((__m128i*)(buffer)), tShiftBytes);
}

inline void SSE::store128i(const __m128i& value, uint8_t* const buffer)
{
	ocean_assert(buffer != nullptr);
	_mm_storeu_si128((__m128i*)(buffer), value);
}

inline __m128i SSE::set128i(const unsigned long long high64, const unsigned long long low64)
{

#ifdef _WINDOWS

	#ifdef _WIN64
		return _mm_set_epi64x(high64, low64);
	#else
		return _mm_set_epi32(*(((int*)&high64) + 1), *((int*)&high64), *(((int*)&low64) + 1), *((int*)&low64));
	#endif

#else

	return _mm_set_epi64x(high64, low64);

#endif

}

inline __m128i SSE::removeHighBits32_16(const __m128i& value)
{
	return _mm_and_si128(value, _mm_set1_epi32(int(0x0000FFFFu)));
}

inline __m128i SSE::removeLowBits32_16(const __m128i& value)
{
	return _mm_and_si128(value, _mm_set1_epi32(int(0xFFFF0000u)));
}

inline __m128i SSE::removeHighBits16_8(const __m128i& value)
{
	return _mm_and_si128(value, _mm_set1_epi32(int(0x00FF00FFu)));
}

inline __m128i SSE::removeHighBits16_8_7_lower(const __m128i& value)
{
	return _mm_and_si128(value, set128i(0x000000FF00FF00FFull, 0x00FF00FF00FF00FFull));
}

inline __m128i SSE::removeHighBits16_8_7_upper(const __m128i& value)
{
	return _mm_and_si128(value, set128i(0x00FF00FF00FF00FFull, 0x00FF00FF00FF0000ull));
}

inline __m128i SSE::moveLowBits16_8ToLow64(const __m128i& value)
{
	return _mm_shuffle_epi8(value, set128i(0xA0A0A0A0A0A0A0A0ull, 0x0E0C0A0806040200ull));
}

inline __m128i SSE::moveLowBits32_8ToLow32(const __m128i& value)
{
	return _mm_shuffle_epi8(value, set128i(0xA0A0A0A0A0A0A0A0ull, 0xA0A0A0A00C080400ull));
}

inline __m128i SSE::moveLowBits32_16ToLow64(const __m128i& value)
{
	return _mm_shuffle_epi8(value, set128i(0xA0A0A0A0A0A0A0A0ull, 0x0D0C090805040100ull));
}

inline __m128i SSE::moveLowBits16_8ToHigh64(const __m128i& value)
{
	return _mm_shuffle_epi8(value, set128i(0x0E0C0A0806040200ull, 0xA0A0A0A0A0A0A0A0ull));
}

inline __m128i SSE::moveHighBits32_16(const __m128i& value)
{
	// shift the four 32 bit integers by 16 to the right and fill by zeros
	return _mm_srli_epi32(value, 16);
}

inline __m128i SSE::moveHighBits16_8(const __m128i& value)
{
	return _mm_shuffle_epi8(value, set128i(0xA00FA00DA00BA009ull, 0xA007A005A003A001ull));
}

inline __m128i SSE::moveHighBits16_8_5(const __m128i& value)
{
	return _mm_shuffle_epi8(value, set128i(0xA0A0A0A0A0A0A009ull, 0xA007A005A003A001ull));
}

inline __m128i SSE::moveHighBits16_8_6(const __m128i& value)
{
	return _mm_shuffle_epi8(value, set128i(0xFFFFFFFFFF0bFF09ull, 0xFF07FF05FF03FF01ull));
}

inline __m128i SSE::moveHighBits16_8_7(const __m128i& value)
{
	return _mm_shuffle_epi8(value, set128i(0xA0A0A00DA00BA009ull, 0xA007A005A003A001ull));
}

inline __m128i SSE::shuffleLow32ToLow32_8(const __m128i& value)
{
	return _mm_shuffle_epi8(value, set128i(0xA0A0A003A0A0A002ull, 0xA0A0A001A0A0A000ull));
}

inline __m128i SSE::shuffleNeighbor4Low64BitsToLow16_8(const __m128i& value)
{
	// we could also use one of the following mask-defining possibility, all provide the same result
	// _mm_set_epi8(0x80, 7, 0x80, 3, 0x80, 6, 0x80, 2, 0x80, 5, 0x80, 1, 0x80, 4, 0x80, 0))
	// _mm_set_epi8(0xA0, 7, 0xA0, 3, 0xA0, 6, 0xA0, 2, 0xA0, 5, 0xA0, 1, 0xA0, 4, 0xA0, 0))
	// _mm_set_epi8(0xFF, 7, 0xFF, 3, 0xFF, 6, 0xFF, 2, 0xFF, 5, 0xFF, 1, 0xFF, 4, 0xFF, 0))

	return _mm_shuffle_epi8(value, set128i(0xA007A003A006A002ull, 0xA005A001A004A000ull));
}

inline __m128i SSE::shuffleNeighbor4High64BitsToLow16_8(const __m128i& value)
{
	return _mm_shuffle_epi8(value, set128i(0xA00FA00BA00EA00Aull, 0xA00DA009A00CA008ull));
}

inline __m128i SSE::shuffleNeighbor2Low64BitsToLow16_8(const __m128i& value)
{
	return _mm_shuffle_epi8(value, set128i(0xFF07FF05FF06FF04ull, 0xFF03FF01FF02FF00ull));
}

inline __m128i SSE::shuffleNeighbor2High64BitsToLow16_8(const __m128i& value)
{
	return _mm_shuffle_epi8(value, set128i(0xFF0FFF0DFF0EFF0Cull, 0xFF0BFF09FF0AFF08ull));
}

inline __m128i SSE::bitMaskRemoveHigh16_8()
{
	return _mm_set1_epi32(int(0x00FF00FFu));
}

inline __m128i SSE::bitMaskRemoveHigh32_16()
{
	return _mm_set1_epi32(int(0x0000FFFFu));
}

OCEAN_FORCE_INLINE void SSE::multiplyInt8x16ToInt32x8(const __m128i& values0, const __m128i& values1, __m128i& products0, __m128i& products1)
{
	const __m128i lowProducts = _mm_mullo_epi16(values0, values1);
	const __m128i highProducts = _mm_mulhi_epi16(values0, values1);

	products0 = _mm_unpacklo_epi16(lowProducts, highProducts);
	products1 = _mm_unpackhi_epi16(lowProducts, highProducts);
}

OCEAN_FORCE_INLINE void SSE::multiplyInt8x16ToInt32x8AndAccumulate(const __m128i& values0, const __m128i& values1, __m128i& results0, __m128i& results1)
{
	__m128i products0;
	__m128i products1;
	multiplyInt8x16ToInt32x8(values0, values1, products0, products1);

	results0 = _mm_add_epi32(results0, products0);
	results1 = _mm_add_epi32(results1, products1);
}

inline unsigned int SSE::interpolation2Channel16Bit1x1(const uint8_t* const pixel, const unsigned int size, const unsigned int fx_y_, const unsigned int fxy_, const unsigned int fx_y, const unsigned int fxy)
{
	ocean_assert(pixel);
	ocean_assert(fx_y_ + fxy_ + fx_y + fxy == 128u * 128u);

	return (pixel[0] * fx_y_ + pixel[2] * fxy_ + pixel[size] * fx_y + pixel[size + 2u] * fxy + 8192u) / 16384u;
}

inline unsigned int SSE::ssd2Channel16Bit1x1(const uint8_t* const pixel0, const uint8_t* const pixel1, const unsigned int /*size0*/, const unsigned int size1, const unsigned int f1x_y_, const unsigned int f1xy_, const unsigned int f1x_y, const unsigned int f1xy)
{
	ocean_assert(pixel0 && pixel1);

	ocean_assert(f1x_y_ + f1xy_ + f1x_y + f1xy == 128u * 128u);

	return sqrDistance(*pixel0, (uint8_t)interpolation2Channel16Bit1x1(pixel1, size1, f1x_y_, f1xy_, f1x_y, f1xy));
}

inline unsigned int SSE::ssd2Channel16Bit1x1(const uint8_t* const pixel0, const uint8_t* const pixel1, const unsigned int size0, const unsigned int size1, const unsigned int f0x_y_, const unsigned int f0xy_, const unsigned int f0x_y, const unsigned int f0xy, const unsigned int f1x_y_, const unsigned int f1xy_, const unsigned int f1x_y, const unsigned int f1xy)
{
	ocean_assert(pixel0 && pixel1);

	ocean_assert(f0x_y_ + f0xy_ + f0x_y + f0xy == 128u * 128u);
	ocean_assert(f1x_y_ + f1xy_ + f1x_y + f1xy == 128u * 128u);

	return sqrDistance(interpolation2Channel16Bit1x1(pixel0, size0, f0x_y_, f0xy_, f0x_y, f0xy), interpolation2Channel16Bit1x1(pixel1, size1, f1x_y_, f1xy_, f1x_y, f1xy));
}

}

}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 41

#endif // META_OCEAN_CV_SSE_H
