/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_NEON_H
#define META_OCEAN_CV_NEON_H

#include "ocean/cv/CV.h"

#include "ocean/base/Utilities.h"

#include "ocean/math/Math.h"

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

#if defined(__ARM_NEON__) || defined(__ARM_NEON)
	#include <arm_neon.h>
#endif // __ARM_NEON__

namespace Ocean
{

namespace CV
{

/**
 * This class implements computer vision functions using NEON extensions.
 * @ingroup cv
 */
class NEON
{
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
		 * Sum square differences determination for the last 9 elements of an 16 elements buffer with 8 bit precision.
		 * @param image0 First 9 elements to determine the ssd for, may be non aligned
		 * @param image1 Second 9 elements to determine the ssd for, may be non aligned
		 * @return SSD result distributed over four terms of the sum
		 */
		static inline uint32x4_t sumSquareDifferences8BitBack9Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum square differences determination for the last 10 elements of an 16 elements buffer with 8 bit precision.
		 * @param image0 First 10 elements to determine the ssd for, may be non aligned
		 * @param image1 Second 10 elements to determine the ssd for, may be non aligned
		 * @return SSD result distributed over four terms of the sum
		 */
		static inline uint32x4_t sumSquareDifferences8BitBack10Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum square differences determination for the last 11 elements of an 16 elements buffer with 8 bit precision.
		 * @param image0 First 11 elements to determine the ssd for, may be non aligned
		 * @param image1 Second 11 elements to determine the ssd for, may be non aligned
		 * @return SSD result distributed over four terms of the sum
		 */
		static inline uint32x4_t sumSquareDifferences8BitBack11Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum square differences determination for the last 12 elements of an 16 elements buffer with 8 bit precision.
		 * @param image0 First 12 elements to determine the ssd for, may be non aligned
		 * @param image1 Second 12 elements to determine the ssd for, may be non aligned
		 * @return SSD result distributed over four terms of the sum
		 */
		static inline uint32x4_t sumSquareDifferences8BitBack12Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum square differences determination for the last 13 elements of an 16 elements buffer with 8 bit precision.
		 * @param image0 First 13 elements to determine the ssd for, may be non aligned
		 * @param image1 Second 13 elements to determine the ssd for, may be non aligned
		 * @return SSD result distributed over four terms of the sum
		 */
		static inline uint32x4_t sumSquareDifferences8BitBack13Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum square differences determination for the last 14 elements of an 16 elements buffer with 8 bit precision.
		 * @param image0 First 14 elements to determine the ssd for, may be non aligned
		 * @param image1 Second 14 elements to determine the ssd for, may be non aligned
		 * @return SSD result distributed over four terms of the sum
		 */
		static inline uint32x4_t sumSquareDifferences8BitBack14Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum square differences determination for the last 15 elements of an 16 elements buffer with 8 bit precision.
		 * @param image0 First 15 elements to determine the ssd for, may be non aligned
		 * @param image1 Second 15 elements to determine the ssd for, may be non aligned
		 * @return SSD result distributed over four terms of the sum
		 */
		static inline uint32x4_t sumSquareDifferences8BitBack15Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum square difference determination for the first 9 elements of an 16 elements buffer with 8 bit precision.
		 * @param image0 First 9 elements to determine the ssd for, may be non aligned
		 * @param image1 Second 9 elements to determine the ssd for, may be non aligned
		 * @return SSD result distributed over four terms of the sum
		 */
		static inline uint32x4_t sumSquareDifference8BitFront9Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum square difference determination for the first 10 elements of an 16 elements buffer with 8 bit precision.
		 * @param image0 First 10 elements to determine the ssd for, may be non aligned
		 * @param image1 Second 10 elements to determine the ssd for, may be non aligned
		 * @return SSD result distributed over four terms of the sum
		 */
		static inline uint32x4_t sumSquareDifference8BitFront10Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum square difference determination for the first 11 elements of an 16 elements buffer with 8 bit precision.
		 * @param image0 First 11 elements to determine the ssd for, may be non aligned
		 * @param image1 Second 11 elements to determine the ssd for, may be non aligned
		 * @return SSD result distributed over four terms of the sum
		 */
		static inline uint32x4_t sumSquareDifference8BitFront11Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum square difference determination for the first 12 elements of an 16 elements buffer with 8 bit precision.
		 * @param image0 First 12 elements to determine the ssd for, may be non aligned
		 * @param image1 Second 12 elements to determine the ssd for, may be non aligned
		 * @return SSD result distributed over four terms of the sum
		 */
		static inline uint32x4_t sumSquareDifference8BitFront12Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum square difference determination for the first 13 elements of an 16 elements buffer with 8 bit precision.
		 * @param image0 First 13 elements to determine the ssd for, may be non aligned
		 * @param image1 Second 13 elements to determine the ssd for, may be non aligned
		 * @return SSD result distributed over four terms of the sum
		 */
		static inline uint32x4_t sumSquareDifference8BitFront13Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum square difference determination for the first 14 elements of an 16 elements buffer with 8 bit precision.
		 * @param image0 First 14 elements to determine the ssd for, may be non aligned
		 * @param image1 Second 14 elements to determine the ssd for, may be non aligned
		 * @return SSD result distributed over four terms of the sum
		 */
		static inline uint32x4_t sumSquareDifference8BitFront14Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum square difference determination for the first 15 elements of an 16 elements buffer with 8 bit precision.
		 * @param image0 First 15 elements to determine the ssd for, may be non aligned
		 * @param image1 Second 15 elements to determine the ssd for, may be non aligned
		 * @return SSD result distributed over four terms of the sum
		 */
		static inline uint32x4_t sumSquareDifference8BitFront15Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum square difference determination for 16 elements with 8 bit precision.
		 * @param image0 First 16 elements to determine the ssd for, may be non aligned
		 * @param image1 Second 16 elements to determine the ssd for, may be non aligned
		 * @return SSD result distributed over four terms of the sum
		 */
		static inline uint32x4_t sumSquareDifference8Bit16Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum square difference determination for 16 elements with 8 bit precision.
		 * @param row0 First 16 elements to determine the ssd for
		 * @param row1 Second 16 elements to determine the ssd for
		 * @return SSD result distributed over four terms of the sum
		 */
		static inline uint32x4_t sumSquareDifference8Bit16Elements(const uint8x16_t& row0, const uint8x16_t& row1);

		/**
		 * Averages 16 elements of 2x2 blocks for 1 channel 8 bit frames.
		 * The function takes two rows of 16 elements and returns 8 average elements (8 averaged pixels, each with 1 channels).
		 * @param row0 First row of 16 elements (16 pixels), must be valid
		 * @param row1 Second row of 16 elements (16 pixels), must be valid
		 * @param result Resulting 8 average elements (8 pixels), must be valid
		 */
		static OCEAN_FORCE_INLINE void average16Elements1Channel8Bit2x2(const uint8_t* const row0, const uint8_t* const row1, uint8_t* const result);

		/**
		 * Averages 32 elements of 2x2 blocks for 1 channel 8 bit frames.
		 * The function takes two rows of 32 elements and returns 16 average elements (16 averaged pixels, each with 1 channels).
		 * @param row0 First row of 32 elements (32 pixels), must be valid
		 * @param row1 Second row of 32 elements (32 pixels), must be valid
		 * @param result Resulting 16 average elements (16 pixels), must be valid
		 */
		static OCEAN_FORCE_INLINE void average32Elements1Channel8Bit2x2(const uint8_t* const row0, const uint8_t* const row1, uint8_t* const result);

		/**
		 * Averages 16 elements of 2x2 blocks for 1 binary (x00 or 0xFF) frames.
		 * The function takes two rows of 16 elements and returns 8 average elements (8 averaged pixels, each with 1 channels).
		 * @param image0 First row of 16 elements
		 * @param image1 Second row of 16 elements
		 * @param threshold Minimal threshold to result in a pixel with value 255
		 * @param result Resulting 8 average elements
		 */
		static inline void average16ElementsBinary1Channel8Bit2x2(const uint8_t* const image0, const uint8_t* const image1, uint8_t* const result, const uint8_t threshold = 192u);

		/**
		 * Averages 32 elements of 2x2 blocks for 2 channel 16 bit frames.
		 * The function takes two rows of 32 elements and returns 16 average elements (8 averaged pixels, each with 2 channels).
		 * @param row0 First row of 32 elements (16 pixels), must be valid
		 * @param row1 Second row of 32 elements (16 pixels), must be valid
		 * @param result Resulting 16 average elements (8 pixels), must be valid
		 */
		static OCEAN_FORCE_INLINE void average32Elements2Channel16Bit2x2(const uint8_t* const row0, const uint8_t* const row1, uint8_t* const result);

		/**
		 * Averages 64 elements of 2x2 blocks for 2 channel 16 bit frames.
		 * The function takes two rows of 64 elements and returns 32 average elements (16 averaged pixels, each with 2 channels).
		 * @param row0 First row of 64 elements (32 pixels), must be valid
		 * @param row1 Second row of 64 elements (32 pixels), must be valid
		 * @param result Resulting 32 average elements (16 pixels), must be valid
		 */
		static OCEAN_FORCE_INLINE void average64Elements2Channel16Bit2x2(const uint8_t* const row0, const uint8_t* const row1, uint8_t* const result);

		/**
		 * Averages 48 elements of 2x2 blocks for 3 channel 24 bit frames.
		 * The function takes two rows of 48 elements and returns 24 average elements (8 averaged pixels, each with 3 channels).
		 * @param row0 First row of 48 elements (16 pixels), must be valid
		 * @param row1 Second row of 48 elements (16 pixels), must be valid
		 * @param result Resulting 24 average elements (8 pixels), must be valid
		 */
		static OCEAN_FORCE_INLINE void average48Elements3Channel24Bit2x2(const uint8_t* const row0, const uint8_t* const row1, uint8_t* const result);

		/**
		 * Averages 64 elements of 2x2 blocks for 4 channel 32 bit frames.
		 * The function takes two rows of 64 elements and returns 32 average elements (16 averaged pixels, each with 4 channels).
		 * @param row0 First row of 64 elements (16 pixels), must be valid
		 * @param row1 Second row of 64 elements (16 pixels), must be valid
		 * @param result Resulting 32 average elements (8 pixels), must be valid
		 */
		static OCEAN_FORCE_INLINE void average64Elements4Channel32Bit2x2(const uint8_t* const row0, const uint8_t* const row1, uint8_t* const result);

		/**
		 * Averages 24 elements of 3x3 blocks for 1 channel 8 bit frames.
		 * The function takes two rows of 24 elements and returns 8 average elements (8 averaged pixels, each with 1 channels).
		 * @param image0 First row of 24 elements
		 * @param image1 Second row of 24 elements
		 * @param image2 Third row of 24 elements
		 * @param result Resulting 8 average elements
		 */
		static inline void average24Elements1Channel8Bit3x3(const uint8_t* const image0, const uint8_t* const image1, const uint8_t* const image2, uint8_t* const result);

		/**
		 * Averages 48 elements of 3x3 blocks for 1 channel 8 bit frames.
		 * The function takes two rows of 48 elements and returns 16 average elements (16 averaged pixels, each with 1 channels).<br>
		 * Beware: This function calculates an approximation only.
		 * @param image0 First row of 48 elements
		 * @param image1 Second row of 48 elements
		 * @param image2 Third row of 48 elements
		 * @param result Resulting 16 average elements
		 */
		static inline void average48Elements1Channel8Bit3x3Approximation(const uint8_t* const image0, const uint8_t* const image1, const uint8_t* const image2, uint8_t* const result);

		/**
		 * Determines the horizontal and the vertical gradients for 8 following pixels for a given 1 channel 8 bit frame.
		 * The resulting gradients are interleaved and each response is inside the range [-127, 127] as the standard response is divided by two.
		 * @param source The source position of the first pixel to determine the gradient for, this pixel must not be a border pixel in the original frame
		 * @param response Resulting gradient responses, first the horizontal response then the vertical response (zipped) for 8 pixels
		 * @param width The width of the original frame in pixel, with range [10, infinity)
		 */
		static inline void gradientHorizontalVertical8Elements1Channel8Bit(const uint8_t* source, int8_t* response, const unsigned int width);

		/**
		 * Determines the squared horizontal and vertical gradients and the product of both gradients for 8 following pixels for a given 1 channel 8 bit frame.
		 * The resulting gradients are interleaved and each response is inside the range [-(127 * 127), 127 * 127] as the standard response is divided by two.
		 * @param source The source position of the first pixel to determine the gradient for, this pixel must not be a border pixel in the original frame
		 * @param response Resulting gradient responses, first the horizontal response then the vertical response and afterwards the product of horizontal and vertical response (zipped) for 8 pixels
		 * @param width The width of the original frame in pixel, with range [10, infinity)
		 */
		static inline void gradientHorizontalVertical8Elements3Products1Channel8Bit(const uint8_t* source, int16_t* response, const unsigned int width);

		/**
		 * Sum square difference determination for 8 elements with 8 bit precision.
		 * @param image0 First 16 elements to determine the ssd for, may be non aligned
		 * @param image1 Second 16 elements to determine the ssd for, may be non aligned
		 * @return SSD result distributed over four terms of the sum
		 */
		static inline uint32x4_t sumSquareDifference8Bit8Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum square difference determination for 8 elements with 8 bit precision.
		 * @param row0 First 16 elements to determine the ssd for
		 * @param row1 Second 16 elements to determine the ssd for
		 * @return SSD result distributed over four terms of the sum
		 */
		static inline uint32x4_t sumSquareDifference8Bit8Elements(const uint8x8_t& row0, const uint8x8_t& row1);

		/**
		 * Sum absolute difference determination for 16 elements with 8 bit precision.
		 * @param image0 First 16 elements to determine the ssd for, may be non aligned
		 * @param image1 Second 16 elements to determine the ssd for, may be non aligned
		 * @return SSD result distributed over four terms of the sum
		 */
		static inline uint32x4_t sumAbsoluteDifference8Bit16Elements(const uint8_t* const image0, const uint8_t* const image1);

		/**
		 * Sum absolute difference determination for 16 elements with 8 bit precision.
		 * @param row0 First 16 elements to determine the ssd for
		 * @param row1 Second 16 elements to determine the ssd for
		 * @return SSD result distributed over four terms of the sum
		 */
		static inline uint32x4_t sumAbsoluteDifference8Bit16Elements(const uint8x16_t& row0, const uint8x16_t& row1);

		/**
		 * Sums the four 32 bit values and returns the result.
		 * Beware: This function is slow due the usage of the individual lanes, providing a large target buffer is much faster.
		 * @param value The value holding the four 32 bit values
		 * @return Sum result
		 */
		static OCEAN_FORCE_INLINE unsigned int sum32x4ByLanes(const uint32x4_t& value);

		/**
		 * Removes (sets to zero) the high 16 bits of four 32 bit elements.
		 * Given:  PONM-LKJI-HGFE-DCBA<br>
		 * Result: 00NM-00JI-00FE-00BA
		 * @param value The value to remove the high bits for
		 * @return Result
		 */
		static OCEAN_FORCE_INLINE uint32x4_t removeHighBits32_16(const uint32x4_t& value);

		/**
		 * Removes (sets to zero) the high 8 bits of four 16 bit elements.
		 * Given:  HGFE-DCBA<br>
		 * Result: 0G0E-0C0A
		 * @param value The value to remove the high bits for
		 * @return Result
		 */
		static OCEAN_FORCE_INLINE uint16x4_t removeHighBits16_8(const uint16x4_t& value);

		/**
		 * Removes (sets to zero) the high 8 bits of eight 16 bit elements.
		 * Given:  PONM-LKJI-HGFE-DCBA<br>
		 * Result: 0O0M-0K0I-0G0E-0C0A
		 * @param value The value to remove the high bits for
		 * @return Result
		 */
		static OCEAN_FORCE_INLINE uint16x8_t removeHighBits16_8(const uint16x8_t& value);

		/**
		 * Moves the high 16 bits of four 32 bit elements to the low 16 bits and fill the high bits with 0.
		 * Given:  PONM-LKJI-HGFE-DCBA<br>
		 * Result: 00PO-00LK-00HG-00DC
		 * @param value The value to remove the high bits for
		 * @return Result
		 */
		static OCEAN_FORCE_INLINE uint32x4_t moveHighBits32_16(const uint32x4_t& value);

		/**
		 * Moves the high 8 bits of four 16 bit elements to the low 8 bits and fill the high bits with 0.
		 * Given:  HGFE-DCBA<br>
		 * Result: 0H0F-0D0B
		 * @param value The value to remove the high bits for
		 * @return Result
		 */
		static OCEAN_FORCE_INLINE uint16x4_t moveHighBits16_8(const uint16x4_t& value);

		/**
		 * Moves the high 8 bits of eight 16 bit elements to the low 8 bits and fill the high bits with 0.
		 * Given:  PONM-LKJI-HGFE-DCBA<br>
		 * Result: 0P0N-0L0J-0H0F-0D0B
		 * @param value The value to remove the high bits for
		 * @return Result
		 */
		static OCEAN_FORCE_INLINE uint16x8_t moveHighBits16_8(const uint16x8_t& value);

		/**
		 * Combines eight 32 bit values (holding 16 bit information) two eight 16 bit values.
		 * Further, the combination is done with saturation (the 32 bit values will be clamped to 16 bit values before the combination is done).
		 * Given:  00DD-00CC-00BB-00AA (low)<br>
		 * Given:  00HH-00GG-00FF-00EE (high)<br>
		 * Result: HH-GG-FF-EE-DD-CC-BB-AA
		 * @param low The 128 bit register with the (resulting) lower 16 bit values
		 * @param high The 128 bit register with the (resulting) higher 16 bit values
		 * @return The resulting 128 bit register with 16 bit values
		 */
		static OCEAN_FORCE_INLINE uint16x8_t combineLowBits32x4to16x8(const uint32x4_t& low, const uint32x4_t& high);

		/**
		 * Combines sixteen 16 bit values (holding 8 bit information) two sixteen 8 bit values.
		 * Further, the combination is done with saturation (the 16 bit values will be clamped to 8 bit values before the combination is done).
		 * Given:  0H0G-0F0E-0D0C-0B0A (low)<br>
		 * Given:  0P0O-0N0M-0L0K-0J0I (high)<br>
		 * Result: P-O-N-M-L-K-J-I-H-G-F-E-D-C-B-A
		 * @param low The 128 bit register with the (resulting) lower 8 bit values
		 * @param high The 128 bit register with the (resulting) higher 8 bit values
		 * @return The resulting 128 bit register with 16 bit values
		 */
		static OCEAN_FORCE_INLINE uint8x16_t combineLowBits16x8to8x16(const uint16x8_t& low, const uint16x8_t& high);

		/**
		 * Determines the four sums of four successive (overlapping) 3x3 blocks of signed 16 bit integer values.
		 * @param rowTop The top row containing 6 short values, must be valid
		 * @param rowCenter The center row containing 6 short values, must be valid
		 * @param rowBottom The bottom row containing 6 short values, must be valid
		 * @return The resulting four sums of the four 3x3 blocks
		 */
		static OCEAN_FORCE_INLINE int32x4_t sum16Bit4Blocks3x3(const short* const rowTop, const short* const rowCenter, const short* const rowBottom);

		/**
		 * Multiplies an two uint64_t value with two uint32_t value and stores the results in two uint64_t values.
		 * This function does not check whether the multiplication results in an overflow.
		 * @param value_u_64x2 The uint64x2_t value to multiply
		 * @param value_u_32x2 The uint32x2_t value to multiply
		 * @return The resulting multiplication result
		 */
		static OCEAN_FORCE_INLINE uint64x2_t multiply(const uint64x2_t& value_u_64x2, const uint32x2_t& value_u_32x2);

		/**
		 * Copies the sign of a given value to another one.
		 * @param signReceiver First value receiving the sign from the second value
		 * @param signProvider Second value providing the sign for the first one
		 * @return First value with the sign of the second one
		 */
		static OCEAN_FORCE_INLINE int32x4_t copySign(const uint32x4_t& signReceiver, const int32x4_t& signProvider);

		/**
		 * Casts 16 float elements to 16 uint8_t elements.
		 * @param sourceA_f_32x4 The first 4 float elements
		 * @param sourceB_f_32x4 The second 4 float elements
		 * @param sourceC_f_32x4 The third 4 float elements
		 * @param sourceD_f_32x4 The fourth 4 float elements
		 * @return The resulting 16 uint8_t elements
		 */
		static OCEAN_FORCE_INLINE uint8x16_t cast16ElementsNEON(const float32x4_t& sourceA_f_32x4, const float32x4_t& sourceB_f_32x4, const float32x4_t& sourceC_f_32x4, const float32x4_t& sourceD_f_32x4);

		/**
		 * Casts 16 float elements to 16 uint8_t elements.
		 * @param source The 16 float elements, must be valid
		 * @return The resulting 16 uint8_t elements
		 */
		static OCEAN_FORCE_INLINE uint8x16_t cast16ElementsNEON(const float* const source);

		/**
		 * Casts 16 uint8_t elements to 16 float elements.
		 * @param source_u_8x16 The 16 uint8_t elements, must be valid
		 * @return The resulting 16 float elements
		 */
		static OCEAN_FORCE_INLINE float32x4x4_t cast16ElementsNEON(const uint8x16_t& source_u_8x16);

		/**
		 * Casts 16 uint8_t elements to 16 float elements.
		 * @param source The 16 uint8_t elements, must be valid
		 * @return The resulting 16 float elements
		 */
		static OCEAN_FORCE_INLINE float32x4x4_t cast16ElementsNEON(const uint8_t* const source);

	private:

		/**
		 * Returns the interpolated pixel values for one 2 channel 16 bit pixel.
		 * @param pixel Uppler left pixel in the frame
		 * @param size Size of one frame row in bytes
		 * @param fx_y_ Product of the inverse fx and the inverse fy interpolation factor
		 * @param fxy_ Product of the fx and the inverse fy interpolation factor
		 * @param fx_y Product of the inverse fx and the fy interpolation factor
		 * @param fxy Product of the fx and the fy interpolation factor
		 * @return Interpolated pixel values
		 */
		static inline unsigned int interpolation2Channel16Bit1x1(const uint8_t* const pixel, const unsigned int size, const unsigned int fx_y_, const unsigned int fxy_, const unsigned int fx_y, const unsigned int fxy);

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
};

inline void NEON::prefetchT0(const void* const data)
{
	__builtin_prefetch(data, 0, 0);
}

inline void NEON::prefetchT1(const void* const data)
{
	__builtin_prefetch(data, 0, 1);
}

inline void NEON::prefetchT2(const void* const data)
{
	__builtin_prefetch(data, 0, 2);
}

inline void NEON::prefetchNTA(const void* const data)
{
	__builtin_prefetch(data, 0, 3);
}

inline uint32x4_t NEON::sumSquareDifferences8BitBack9Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	const uint8x16_t row0 = vld1q_u8(image0);
	const uint8x16_t row1 = vld1q_u8(image1);

	const uint8x16_t mask = vcombine_u8(vcreate_u8(0xFF00000000000000ull), vdup_n_u8(0xFFu));
	return sumSquareDifference8Bit16Elements(vandq_u8(row0, mask), vandq_u8(row1, mask));
}

inline uint32x4_t NEON::sumSquareDifferences8BitBack10Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	const uint8x16_t row0 = vld1q_u8(image0);
	const uint8x16_t row1 = vld1q_u8(image1);

	const uint8x16_t mask = vcombine_u8(vcreate_u8(0xFFFF000000000000ull), vdup_n_u8(0xFFu));
	return sumSquareDifference8Bit16Elements(vandq_u8(row0, mask), vandq_u8(row1, mask));
}

inline uint32x4_t NEON::sumSquareDifferences8BitBack11Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	const uint8x16_t row0 = vld1q_u8(image0);
	const uint8x16_t row1 = vld1q_u8(image1);

	const uint8x16_t mask = vcombine_u8(vcreate_u8(0xFFFFFF0000000000ull), vdup_n_u8(0xFFu));
	return sumSquareDifference8Bit16Elements(vandq_u8(row0, mask), vandq_u8(row1, mask));
}

inline uint32x4_t NEON::sumSquareDifferences8BitBack12Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	const uint8x16_t row0 = vld1q_u8(image0);
	const uint8x16_t row1 = vld1q_u8(image1);

	const uint8x16_t mask = vcombine_u8(vcreate_u8(0xFFFFFFFF00000000ull), vdup_n_u8(0xFFu));
	return sumSquareDifference8Bit16Elements(vandq_u8(row0, mask), vandq_u8(row1, mask));
}

inline uint32x4_t NEON::sumSquareDifferences8BitBack13Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	const uint8x16_t row0 = vld1q_u8(image0);
	const uint8x16_t row1 = vld1q_u8(image1);

	const uint8x16_t mask = vcombine_u8(vcreate_u8(0xFFFFFFFFFF000000ull), vdup_n_u8(0xFFu));
	return sumSquareDifference8Bit16Elements(vandq_u8(row0, mask), vandq_u8(row1, mask));
}

inline uint32x4_t NEON::sumSquareDifferences8BitBack14Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	const uint8x16_t row0 = vld1q_u8(image0);
	const uint8x16_t row1 = vld1q_u8(image1);

	const uint8x16_t mask = vcombine_u8(vcreate_u8(0xFFFFFFFFFFFF0000ull), vdup_n_u8(0xFFu));
	return sumSquareDifference8Bit16Elements(vandq_u8(row0, mask), vandq_u8(row1, mask));
}

inline uint32x4_t NEON::sumSquareDifferences8BitBack15Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	const uint8x16_t row0 = vld1q_u8(image0);
	const uint8x16_t row1 = vld1q_u8(image1);

	const uint8x16_t mask = vcombine_u8(vcreate_u8(0xFFFFFFFFFFFFFF00ull), vdup_n_u8(0xFFu));
	return sumSquareDifference8Bit16Elements(vandq_u8(row0, mask), vandq_u8(row1, mask));
}

inline uint32x4_t NEON::sumSquareDifference8BitFront9Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	const uint8x16_t row0 = vld1q_u8(image0);
	const uint8x16_t row1 = vld1q_u8(image1);

	const uint8x16_t mask = vcombine_u8(vdup_n_u8(0xFFu), vcreate_u8(0x00000000000000FFull));
	return sumSquareDifference8Bit16Elements(vandq_u8(row0, mask), vandq_u8(row1, mask));
}

inline uint32x4_t NEON::sumSquareDifference8BitFront10Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	const uint8x16_t row0 = vld1q_u8(image0);
	const uint8x16_t row1 = vld1q_u8(image1);

	const uint8x16_t mask = vcombine_u8(vdup_n_u8(0xFFu), vcreate_u8(0x000000000000FFFFull));
	return sumSquareDifference8Bit16Elements(vandq_u8(row0, mask), vandq_u8(row1, mask));
}

inline uint32x4_t NEON::sumSquareDifference8BitFront11Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	const uint8x16_t row0 = vld1q_u8(image0);
	const uint8x16_t row1 = vld1q_u8(image1);

	const uint8x16_t mask = vcombine_u8(vdup_n_u8(0xFFu), vcreate_u8(0x0000000000FFFFFFull));
	return sumSquareDifference8Bit16Elements(vandq_u8(row0, mask), vandq_u8(row1, mask));
}

inline uint32x4_t NEON::sumSquareDifference8BitFront12Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	const uint8x16_t row0 = vld1q_u8(image0);
	const uint8x16_t row1 = vld1q_u8(image1);

	const uint8x16_t mask = vcombine_u8(vdup_n_u8(0xFFu), vcreate_u8(0x00000000FFFFFFFFull));
	return sumSquareDifference8Bit16Elements(vandq_u8(row0, mask), vandq_u8(row1, mask));
}

inline uint32x4_t NEON::sumSquareDifference8BitFront13Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	const uint8x16_t row0 = vld1q_u8(image0);
	const uint8x16_t row1 = vld1q_u8(image1);

	const uint8x16_t mask = vcombine_u8(vdup_n_u8(0xFFu), vcreate_u8(0x000000FFFFFFFFFFull));
	return sumSquareDifference8Bit16Elements(vandq_u8(row0, mask), vandq_u8(row1, mask));
}

inline uint32x4_t NEON::sumSquareDifference8BitFront14Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	const uint8x16_t row0 = vld1q_u8(image0);
	const uint8x16_t row1 = vld1q_u8(image1);

	const uint8x16_t mask = vcombine_u8(vdup_n_u8(0xFFu), vcreate_u8(0x0000FFFFFFFFFFFFull));
	return sumSquareDifference8Bit16Elements(vandq_u8(row0, mask), vandq_u8(row1, mask));
}

inline uint32x4_t NEON::sumSquareDifference8BitFront15Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	const uint8x16_t row0 = vld1q_u8(image0);
	const uint8x16_t row1 = vld1q_u8(image1);

	const uint8x16_t mask = vcombine_u8(vdup_n_u8(0xFFu), vcreate_u8(0x00FFFFFFFFFFFFFFull));
	return sumSquareDifference8Bit16Elements(vandq_u8(row0, mask), vandq_u8(row1, mask));
}

inline uint32x4_t NEON::sumSquareDifference8Bit16Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	uint8x16_t row0 = vld1q_u8(image0);
	uint8x16_t row1 = vld1q_u8(image1);

	return sumSquareDifference8Bit16Elements(row0, row1);
}

inline uint32x4_t NEON::sumSquareDifference8Bit16Elements(const uint8x16_t& row0, const uint8x16_t& row1)
{
	// Absolute difference between the arguments
	uint8x16_t subtract = vabdq_u8(row0, row1);

	uint8x8_t subtractLow = vget_low_u8(subtract);
	uint8x8_t subtractHigh = vget_high_u8(subtract);

	uint16x8_t squareLow = vmull_u8(subtractLow, subtractLow);
	uint16x8_t squareHigh = vmull_u8(subtractHigh, subtractHigh);

	return vaddq_u32(vaddl_u16(vget_low_u16(squareLow), vget_low_u16(squareHigh)), vaddl_u16(vget_high_u16(squareLow), vget_high_u16(squareHigh)));
}

inline uint32x4_t NEON::sumSquareDifference8Bit8Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	const uint8x8_t row0 = vld1_u8(image0);
	const uint8x8_t row1 = vld1_u8(image1);

	return sumSquareDifference8Bit8Elements(row0, row1);
}

inline uint32x4_t NEON::sumSquareDifference8Bit8Elements(const uint8x8_t& row0, const uint8x8_t& row1)
{
	// subtract the 8 elements (usage of saturation and bitwise or operator)
	const uint8x8_t subtract = vorr_u8(vqsub_u8(row0, row1), vqsub_u8(row1, row0));

	// distribute the 16 elements of 8 bit values into 16 elements of 16 bit values (necessary for multiplication)
	const uint16x4_t subtractLow = removeHighBits16_8(vreinterpret_u16_u8(subtract));
	const uint16x4_t subtractHigh = moveHighBits16_8(vreinterpret_u16_u8(subtract));

	const uint16x8_t subtractCombined = vcombine_u16(subtractLow, subtractHigh);

	// square the 16 elements
	const uint16x8_t square = vmulq_u16(subtractCombined, subtractCombined);

	// summing the 8 elements of 16 bit values
	return vaddq_u32(removeHighBits32_16(vreinterpretq_u32_u16(square)), moveHighBits32_16(vreinterpretq_u32_u16(square)));
}

inline uint32x4_t NEON::sumAbsoluteDifference8Bit16Elements(const uint8_t* const image0, const uint8_t* const image1)
{
	ocean_assert(image0 && image1);

	uint8x16_t row0 = vld1q_u8(image0);
	uint8x16_t row1 = vld1q_u8(image1);

	return sumAbsoluteDifference8Bit16Elements(row0, row1);
}

inline uint32x4_t NEON::sumAbsoluteDifference8Bit16Elements(const uint8x16_t& row0, const uint8x16_t& row1)
{
	// subtract the 16 elements (usage of saturation and bitwise or operator)
	uint8x16_t subtract = vabdq_u8(row0, row1);

	uint16x8_t add16 = vaddl_u8(vget_low_u8(subtract), vget_high_u8(subtract));

	return vaddl_u16(vget_low_u16(add16), vget_high_u16(add16));
}

OCEAN_FORCE_INLINE void NEON::average16Elements1Channel8Bit2x2(const uint8_t* const row0, const uint8_t* const row1, uint8_t* const result)
{
	ocean_assert(row0 != nullptr && row1 != nullptr && result != nullptr);

	// we load 16 successive pixels (= 1 * 16 = 16 values)

	const uint8x16_t m128_row0 = vld1q_u8(row0);
	const uint8x16_t m128_row1 = vld1q_u8(row1);

	// now we simply average the corresponding values of two rows by using NEON's rounding halving add function:
	// vrhaddq_u8: Vr[i] := (Va[i] + Vb[i] + 1) >> 1

	// the next step will be to add successive pairs within the merged row by using NEON's long pairwise add function:
	// vpaddlq_u8 adds two neighboring 8 bit integers and creates 16 bit integer sums

	// finally we use NEON's rounding narrowing shift function (converting 16 bit integers to 8 bit integers)
	// vrshrn_n_u16: Vr_8[i] := (Va_16[i] + 1) >> 1

	const uint8x8_t average = vrshrn_n_u16(vpaddlq_u8(vrhaddq_u8(m128_row0, m128_row1)), 1);

	// we write back the results

	vst1_u8(result, average);
}

OCEAN_FORCE_INLINE void NEON::average32Elements1Channel8Bit2x2(const uint8_t* const row0, const uint8_t* const row1, uint8_t* const result)
{
	// @see average16Elements1Channel8Bit2x2() for a detailed documentation

	ocean_assert(row0 != nullptr && row1 != nullptr && result != nullptr);

	const uint8x16_t row0A_u_8x16 = vld1q_u8(row0 + 0);
	const uint8x16_t row0B_u_8x16 = vld1q_u8(row0 + 16);

	const uint8x16_t row1A_u_8x16 = vld1q_u8(row1 + 0);
	const uint8x16_t row1B_u_8x16 = vld1q_u8(row1 + 16);

	const uint8x8_t averageA_u_8x8 = vrshrn_n_u16(vpaddlq_u8(vrhaddq_u8(row0A_u_8x16, row1A_u_8x16)), 1);
	const uint8x8_t averageB_u_8x8 = vrshrn_n_u16(vpaddlq_u8(vrhaddq_u8(row0B_u_8x16, row1B_u_8x16)), 1);

	const uint8x16_t average_u_8x16 = vcombine_u8(averageA_u_8x8, averageB_u_8x8);

	vst1q_u8(result, average_u_8x16);
}

inline void NEON::average16ElementsBinary1Channel8Bit2x2(const uint8_t* const image0, const uint8_t* const image1, uint8_t* const result, const uint8_t threshold)
{
	ocean_assert(image0 && image1 && result);

	const uint8x16_t row0 = vld1q_u8(image0);
	const uint8x16_t row1 = vld1q_u8(image1);

	// calculate normal average
	const uint8x8_t average = vmovn_u16(vshrq_n_u16(vpaddlq_u8(vhaddq_u8(row0, row1)), 1));

	// thresholding
	const uint8x8_t thresholded = vcge_u8(average, vmov_n_u8(threshold));

	vst1_u8(result, thresholded);
}

OCEAN_FORCE_INLINE void NEON::average32Elements2Channel16Bit2x2(const uint8_t* const row0, const uint8_t* const row1, uint8_t* const result)
{
	ocean_assert(row0 != nullptr && row1 != nullptr && result != nullptr);

	// we load 16 successive pixels (= 2 * 16 = 32 values) and directly deinterleave the 2 channels
	// from YA YA YA YA ... so that we receive the following patterns:
	// m2_128_row0.val[0]: Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y
	// m2_128_row0.val[1]: A A A A A A A A A A A A A A A A

	const uint8x16x2_t m2_128_row0 = vld2q_u8(row0);
	const uint8x16x2_t m2_128_row1 = vld2q_u8(row1);

	// now we simply average the corresponding values of two rows by using NEON's rounding halving add function:
	// vrhaddq_u8: Vr[i] := (Va[i] + Vb[i] + 1) >> 1

	// the next step will be to add successive pairs within the merged row by using NEON's long pairwise add function:
	// vpaddlq_u8 adds two neighboring 8 bit integers and creates 16 bit integer sums

	// finally we use NEON's rounding narrowing shift function (converting 16 bit integers to 8 bit integers)
	// vrshrn_n_u16: Vr_8[i] := (Va_16[i] + 1) >> 1

	uint8x8x2_t average;

	average.val[0] = vrshrn_n_u16(vpaddlq_u8(vrhaddq_u8(m2_128_row0.val[0], m2_128_row1.val[0])), 1);
	average.val[1] = vrshrn_n_u16(vpaddlq_u8(vrhaddq_u8(m2_128_row0.val[1], m2_128_row1.val[1])), 1);

	// we write back the results, this time we interleave the results again

	vst2_u8(result, average);
}

OCEAN_FORCE_INLINE void NEON::average64Elements2Channel16Bit2x2(const uint8_t* const row0, const uint8_t* const row1, uint8_t* const result)
{
	// @see average32Elements2Channel16Bit2x2() for a detailed documentation

	ocean_assert(row0 != nullptr && row1 != nullptr && result != nullptr);

	const uint8x16x2_t row0A_u_8x16x2 = vld2q_u8(row0 + 0);
	const uint8x16x2_t row0B_u_8x16x2 = vld2q_u8(row0 + 32);

	const uint8x16x2_t row1A_u_8x16x2 = vld2q_u8(row1 + 0);
	const uint8x16x2_t row1B_u_8x16x2 = vld2q_u8(row1 + 32);

	const uint8x8_t averageChannel0A_u_8x8 = vrshrn_n_u16(vpaddlq_u8(vrhaddq_u8(row0A_u_8x16x2.val[0], row1A_u_8x16x2.val[0])), 1);
	const uint8x8_t averageChannel1A_u_8x8 = vrshrn_n_u16(vpaddlq_u8(vrhaddq_u8(row0A_u_8x16x2.val[1], row1A_u_8x16x2.val[1])), 1);
	const uint8x8_t averageChannel0B_u_8x8 = vrshrn_n_u16(vpaddlq_u8(vrhaddq_u8(row0B_u_8x16x2.val[0], row1B_u_8x16x2.val[0])), 1);
	const uint8x8_t averageChannel1B_u_8x8 = vrshrn_n_u16(vpaddlq_u8(vrhaddq_u8(row0B_u_8x16x2.val[1], row1B_u_8x16x2.val[1])), 1);

	uint8x16x2_t average_u_8x16x2;

	average_u_8x16x2.val[0] = vcombine_u8(averageChannel0A_u_8x8, averageChannel0B_u_8x8);
	average_u_8x16x2.val[1] = vcombine_u8(averageChannel1A_u_8x8, averageChannel1B_u_8x8);

	vst2q_u8(result, average_u_8x16x2);
}

OCEAN_FORCE_INLINE void NEON::average48Elements3Channel24Bit2x2(const uint8_t* const row0, const uint8_t* const row1, uint8_t* const result)
{
	ocean_assert(row0 != nullptr && row1 != nullptr && result != nullptr);

	// we load 16 successive pixels (= 3 * 16 = 48 values) and directly deinterleave the 3 channels
	// from RGB RGB RGB RGB ... so that we receive the following patterns:
	// m3_128_row0.val[0]: R R R R R R R R R R R R R R R R
	// m3_128_row0.val[1]: G G G G G G G G G G G G G G G G
	// m3_128_row0.val[2]: B B B B B B B B B B B B B B B B

	const uint8x16x3_t m3_128_row0 = vld3q_u8(row0);
	const uint8x16x3_t m3_128_row1 = vld3q_u8(row1);

	// now we simply average the corresponding values of two rows by using NEON's rounding halving add function:
	// vrhaddq_u8: Vr[i] := (Va[i] + Vb[i] + 1) >> 1

	// the next step will be to add successive pairs within the merged row by using NEON's long pairwise add function:
	// vpaddlq_u8 adds two neighboring 8 bit integers and creates 16 bit integer sums

	// finally we use NEON's rounding narrowing shift function (converting 16 bit integers to 8 bit integers)
	// vrshrn_n_u16: Vr_8[i] := (Va_16[i] + 1) >> 1

	uint8x8x3_t average;

	average.val[0] = vrshrn_n_u16(vpaddlq_u8(vrhaddq_u8(m3_128_row0.val[0], m3_128_row1.val[0])), 1);
	average.val[1] = vrshrn_n_u16(vpaddlq_u8(vrhaddq_u8(m3_128_row0.val[1], m3_128_row1.val[1])), 1);
	average.val[2] = vrshrn_n_u16(vpaddlq_u8(vrhaddq_u8(m3_128_row0.val[2], m3_128_row1.val[2])), 1);

	// we write back the results, this time we interleave the results again

	vst3_u8(result, average);

	/* the following code would provide a more precise rounding
	uint16x8_t zero4 = vmovq_n_u16(0x0002u);

	uint16x8_t redTmp = vpadalq_u8(zero4, row0.val[0]);
	average.val[0] = vmovn_u16(vshrq_n_u16(vpadalq_u8(redTmp, row1.val[0]), 2));

	uint16x8_t greenTmp = vpadalq_u8(zero4, row0.val[1]);
	average.val[1] = vmovn_u16(vshrq_n_u16(vpadalq_u8(greenTmp, row1.val[1]), 2));

	uint16x8_t blueTmp = vpadalq_u8(zero4, row0.val[2]);
	average.val[2] = vmovn_u16(vshrq_n_u16(vpadalq_u8(blueTmp, row1.val[2]), 2));*/
}

OCEAN_FORCE_INLINE void NEON::average64Elements4Channel32Bit2x2(const uint8_t* const row0, const uint8_t* const row1, uint8_t* const result)
{
	ocean_assert(row0 != nullptr && row1 != nullptr && result != nullptr);

	// we load 16 successive pixels (= 4 * 16 = 64 values) and directly deinterleave the 4 channels
	// from RGBA RGBA RGBA RGBA ... so that we receive the following patterns:
	// m4_128_row0.val[0]: R R R R R R R R R R R R R R R R
	// m4_128_row0.val[1]: G G G G G G G G G G G G G G G G
	// m4_128_row0.val[2]: B B B B B B B B B B B B B B B B
	// m4_128_row0.val[3]: A A A A A A A A A A A A A A A A

	const uint8x16x4_t m4_128_row0 = vld4q_u8(row0);
	const uint8x16x4_t m4_128_row1 = vld4q_u8(row1);

	// now we simply average the corresponding values of two rows by using NEON's rounding halving add function:
	// vrhaddq_u8: Vr[i] := (Va[i] + Vb[i] + 1) >> 1

	// the next step will be to add successive pairs within the merged row by using NEON's long pairwise add function:
	// vpaddlq_u8 adds two neighboring 8 bit integers and creates 16 bit integer sums

	// finally we use NEON's rounding narrowing shift function (converting 16 bit integers to 8 bit integers)
	// vrshrn_n_u16: Vr_8[i] := (Va_16[i] + 1) >> 1

	uint8x8x4_t average;

	average.val[0] = vrshrn_n_u16(vpaddlq_u8(vrhaddq_u8(m4_128_row0.val[0], m4_128_row1.val[0])), 1);
	average.val[1] = vrshrn_n_u16(vpaddlq_u8(vrhaddq_u8(m4_128_row0.val[1], m4_128_row1.val[1])), 1);
	average.val[2] = vrshrn_n_u16(vpaddlq_u8(vrhaddq_u8(m4_128_row0.val[2], m4_128_row1.val[2])), 1);
	average.val[3] = vrshrn_n_u16(vpaddlq_u8(vrhaddq_u8(m4_128_row0.val[3], m4_128_row1.val[3])), 1);

	// we write back the results, this time we interleave the results again

	vst4_u8(result, average);
}

inline void NEON::average24Elements1Channel8Bit3x3(const uint8_t* const image0, const uint8_t* const image1, const uint8_t* const image2, uint8_t* const result)
{
	ocean_assert(image0 && image1 && image2 && result);

	/**
	 *      | 1 2 1 |
	 * 1/16 | 2 4 2 |
	 *		| 1 2 1 |
	 */

	// load 3 * 8 uchars
	uint8x8x3_t row0 = vld3_u8(image0);
	uint8x8x3_t row1 = vld3_u8(image1);
	uint8x8x3_t row2 = vld3_u8(image2);

	uint16x8x3_t sumPerRow;

	// create sum across rows, middle row is summed twice
	sumPerRow.val[0] = vaddq_u16(vaddq_u16(vmovl_u8(row0.val[0]), vmovl_u8(row2.val[0])), vshlq_n_u16(vmovl_u8(row1.val[0]), 1));
	sumPerRow.val[1] = vaddq_u16(vaddq_u16(vmovl_u8(row0.val[1]), vmovl_u8(row2.val[1])), vshlq_n_u16(vmovl_u8(row1.val[1]), 1));
	sumPerRow.val[2] = vaddq_u16(vaddq_u16(vmovl_u8(row0.val[2]), vmovl_u8(row2.val[2])), vshlq_n_u16(vmovl_u8(row1.val[2]), 1));

	// create sum across neighbouring pixels, second element within trio is summed twice
	const uint16x8_t sum = vaddq_u16(vaddq_u16(sumPerRow.val[0], sumPerRow.val[2]), vshlq_n_u16(sumPerRow.val[1], 1));

	// calculate the average:  (sum + 8u) >> 4
	const uint8x8_t average = vmovn_u16(vshrq_n_u16(vaddq_u16(sum, vmovq_n_u16(8u)), 4));

	vst1_u8(result, average);
}

inline void NEON::average48Elements1Channel8Bit3x3Approximation(const uint8_t* const image0, const uint8_t* const image1, const uint8_t* const image2, uint8_t* const result)
{
	ocean_assert(image0 && image1 && image2 && result);

	/**
	 *      | 1 2 1 |
	 * 1/16 | 2 4 2 |
	 *		| 1 2 1 |
	 */

	// load 3 * 16 uchars and de-interleave triples:
	//
	// row0: A0 A1 A2   A3 A4 A5   A6 A7 A8   A9 ... A44   A45 A46 A47
	// row1: B0 B1 B2   B3 B4 B5   B6 B7 B8   B9 ... B44   B45 B46 B47
	// row2: C0 C1 C2   C3 C4 C5   C6 C7 C8   C9 ... C44   C45 C46 C47

	uint8x16x3_t row0 = vld3q_u8(image0);
	uint8x16x3_t row1 = vld3q_u8(image1);
	uint8x16x3_t row2 = vld3q_u8(image2);

	// now de-interleaved:
	//
	//       val[0]                  val[1]                   valu[2]
	// row0: A0 A3 A6 A9 ... A45     A1 A4 A7 A10 ... A46     A2 A5 A8 A11 ... A47
	// row1: B0 B3 B6 B9 ... B45     B1 B4 B7 B10 ... B46     B2 B5 B8 B11 ... B47
	// row2: C0 C3 C6 C9 ... C45     C1 C4 C7 C10 ... C46     C2 C5 C8 C11 ... C47

	// now we need to 'multiply' row1 by 2 and val[1] by 2, we solve this by creating the average of the first and second row followed by the average with the middle row

	uint8x16x3_t averagePerRow;
	averagePerRow.val[0] = vhaddq_u8(vhaddq_u8(row0.val[0], row2.val[0]), row1.val[0]);
	averagePerRow.val[1] = vhaddq_u8(vhaddq_u8(row0.val[1], row2.val[1]), row1.val[1]);
	averagePerRow.val[2] = vhaddq_u8(vhaddq_u8(row0.val[2], row2.val[2]), row1.val[2]);

	// we apply the same idea as bevore in vertical direction
	const uint8x16_t average = vhaddq_u8(vhaddq_u8(averagePerRow.val[0], averagePerRow.val[2]), averagePerRow.val[1]);

	vst1q_u8(result, average);
}

inline void NEON::gradientHorizontalVertical8Elements1Channel8Bit(const uint8_t* source, int8_t* response, const unsigned int width)
{
	ocean_assert(source && response && width >= 10u);

	// we load the left 8 unsigned 8bit elements and store them has signed 16bit values
	int16x8_t horizontalMinus = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(source - 1)));
	// we load the right 8 unsigned 8bit elements and store them has signed 16bit values
	int16x8_t horizontalPlus = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(source + 1)));

	// we load the top 8 unsigned 8bit elements and store them has signed 16bit values
	int16x8_t verticalMinus = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(source - width)));
	// we load the bottom 8 unsigned 8bit elements and store them has signed 16bit values
	int16x8_t verticalPlus = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(source + width)));

	int8x8x2_t result;

	// we subtract the horizontal values (right - left), and divide the result by 2, and narrow the results to 8 bit values
	result.val[0] = vmovn_s16(vhsubq_s16(horizontalPlus, horizontalMinus));
	// we subtract the vertical values (right - left), and divide the result by 2, and narrow the results to 8 bit values
	result.val[1] = vmovn_s16(vhsubq_s16(verticalPlus, verticalMinus));

	// we store the determined results interleaved
	vst2_s8((int8_t*)response, result);
}

inline void NEON::gradientHorizontalVertical8Elements3Products1Channel8Bit(const uint8_t* source, int16_t* response, const unsigned int width)
{
	ocean_assert(source && response && width >= 10u);

	// we load the left 8 unsigned 8bit elements and store them has signed 16bit values
	int16x8_t horizontalMinus = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(source - 1)));
	// we load the right 8 unsigned 8bit elements and store them has signed 16bit values
	int16x8_t horizontalPlus = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(source + 1)));

	// we load the top 8 unsigned 8bit elements and store them has signed 16bit values
	int16x8_t verticalMinus = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(source - width)));
	// we load the bottom 8 unsigned 8bit elements and store them has signed 16bit values
	int16x8_t verticalPlus = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(source + width)));

	// we subtract the horizontal values (right - left) and divide the result by 2
	int16x8_t horizontal = vhsubq_s16(horizontalPlus, horizontalMinus);
	// we subtract the vertical values (bottom - top) and divide the result by 2
	int16x8_t vertical = vhsubq_s16(verticalPlus, verticalMinus);

	int16x8x3_t result;

	// we multiply horizontal with horizontal
	result.val[0] = vmulq_s16(horizontal, horizontal);
	// we multiply vertical with vertical
	result.val[1] = vmulq_s16(vertical, vertical);
	// we multiply horizontal with vertical
	result.val[2] = vmulq_s16(horizontal, vertical);

	// we store the determined results interleaved (h*h, v*v, h*v, h*h, v*v, h*v, ...)
	vst3q_s16(response, result);
}

OCEAN_FORCE_INLINE unsigned int NEON::sum32x4ByLanes(const uint32x4_t& value)
{
	return vgetq_lane_u32(value, 0) + vgetq_lane_u32(value, 1) + vgetq_lane_u32(value, 2) + vgetq_lane_u32(value, 3);
}

OCEAN_FORCE_INLINE uint32x4_t NEON::removeHighBits32_16(const uint32x4_t& value)
{
	return vandq_u32(value, vmovq_n_u32(0x0000FFFFu));
}

OCEAN_FORCE_INLINE uint16x4_t NEON::removeHighBits16_8(const uint16x4_t& value)
{
	return vand_u16(value, vreinterpret_u16_u32(vmov_n_u32(0x00FF00FFu)));
}

OCEAN_FORCE_INLINE uint16x8_t NEON::removeHighBits16_8(const uint16x8_t& value)
{
	return vandq_u16(value, vreinterpretq_u16_u32(vmovq_n_u32(0x00FF00FFu)));
}

OCEAN_FORCE_INLINE uint32x4_t NEON::moveHighBits32_16(const uint32x4_t& value)
{
	return vshrq_n_u32(value, 16);
}

OCEAN_FORCE_INLINE uint16x4_t NEON::moveHighBits16_8(const uint16x4_t& value)
{
	return vshr_n_u16(value, 8);
}

OCEAN_FORCE_INLINE uint16x8_t NEON::moveHighBits16_8(const uint16x8_t& value)
{
	return vshrq_n_u16(value, 8);
}

OCEAN_FORCE_INLINE uint16x8_t NEON::combineLowBits32x4to16x8(const uint32x4_t& low, const uint32x4_t& high)
{
	return vcombine_u16(vqmovn_u32(low), vqmovn_u32(high));
}

OCEAN_FORCE_INLINE uint8x16_t NEON::combineLowBits16x8to8x16(const uint16x8_t& low, const uint16x8_t& high)
{
	return vcombine_u8(vqmovn_u16(low), vqmovn_u16(high));
}

OCEAN_FORCE_INLINE int32x4_t NEON::sum16Bit4Blocks3x3(const short* const rowTop, const short* const rowCenter, const short* const rowBottom)
{
	ocean_assert(rowTop != nullptr);
	ocean_assert(rowCenter != nullptr);
	ocean_assert(rowBottom != nullptr);

	// 1 1 1
	// 1 1 1
	// 1 1 1

	//  1 1 1
	//  1 1 1
	//  1 1 1

	//   1 1 1
	//   1 1 1
	//   1 1 1

	// ...

	// load the top row
	const int16x4_t top_0_s_16x4 = vld1_s16(rowTop + 0);
	const int16x4_t top_1_s_16x4 = vld1_s16(rowTop + 1);
	const int16x4_t top_2_s_16x4 = vld1_s16(rowTop + 2);

	// load the center row
	const int16x4_t center_0_s_16x4 = vld1_s16(rowCenter + 0);
	const int16x4_t center_1_s_16x4 = vld1_s16(rowCenter + 1);
	const int16x4_t center_2_s_16x4 = vld1_s16(rowCenter + 2);

	// load the bottom row
	const int16x4_t bottom_0_s_16x4 = vld1_s16(rowBottom + 0);
	const int16x4_t bottom_1_s_16x4 = vld1_s16(rowBottom + 1);
	const int16x4_t bottom_2_s_16x4 = vld1_s16(rowBottom + 2);

	// summing up the individual elements (16 bit + 16 bit -> 32 bit)
	const int32x4_t result_A_s_32x4 = vaddl_s16(top_0_s_16x4, top_2_s_16x4);
	const int32x4_t result_B_s_32x4 = vaddl_s16(center_0_s_16x4, center_2_s_16x4);
	const int32x4_t result_C_s_32x4 = vaddl_s16(bottom_0_s_16x4, bottom_2_s_16x4);
	const int32x4_t result_D_s_32x4 = vaddl_s16(top_1_s_16x4, center_1_s_16x4);

	// summing up the intermediate results
	const int32x4_t result_E_s_32x4 = vaddq_s32(result_A_s_32x4, result_B_s_32x4);
	const int32x4_t result_F_s_32x4 = vaddq_s32(result_C_s_32x4, result_D_s_32x4);

	const int32x4_t result_G_s_32x4 = vaddq_s32(result_E_s_32x4, result_F_s_32x4);

	// adding the last missing row
	return vaddw_s16(result_G_s_32x4, bottom_1_s_16x4);
}

OCEAN_FORCE_INLINE uint64x2_t NEON::multiply(const uint64x2_t& value_u_64x2, const uint32x2_t& value_u_32x2)
{
	// uint64_t * uint32_t
	// = (high(uint64_t) + low(uint64_t)) * uint32_t
	// = (((high(uint64_t) >> 32) * uint32_t) << 32) + low(uint64_t) * uint32_t

	// [ valueA_u_64, valueB_u64 ] -> [ high(valueA_u_64), high(valueB_u64) ], [ low(valueA_u_64), low(valueB_u64) ]
	const uint32x2x2_t value64_lowHigh_32x2x2 = vtrn_u32(vget_low_u32(vreinterpretq_u32_u64(value_u_64x2)), vget_high_u32(vreinterpretq_u32_u64(value_u_64x2)));

	const uint64x2_t multiplication_low_64x2 = vmull_u32(value64_lowHigh_32x2x2.val[0], value_u_32x2);
	const uint64x2_t multiplication_high_64x2 = vmull_u32(value64_lowHigh_32x2x2.val[1], value_u_32x2);

	const uint64x2_t shiftedMultiplication_high_64x2 = vshlq_n_u64(multiplication_high_64x2, 32);

	return vaddq_u64(shiftedMultiplication_high_64x2, multiplication_low_64x2);
}

OCEAN_FORCE_INLINE int32x4_t NEON::copySign(const uint32x4_t& signReceiver_u_32x4, const int32x4_t& signProvider_s_32x4)
{
	const int32x4_t negativeSignReceiver_u_32x4 = vnegq_s32(vreinterpretq_s32_u32(signReceiver_u_32x4));

	const uint32x4_t isNegativeMask_u_32x4 = vcltq_s32(signProvider_s_32x4, vdupq_n_s32(0)); // sign < 0 ? 0xFF : 0x00;
	const uint32x4_t isPositiveMask_u_32x4 = vcgeq_s32(signProvider_s_32x4, vdupq_n_s32(0)); // sign >= 0 ? 0xFF : 0x00;

	return vreinterpretq_s32_u32(vorrq_u32(vandq_u32(vreinterpretq_u32_s32(negativeSignReceiver_u_32x4), isNegativeMask_u_32x4), vandq_u32(signReceiver_u_32x4, isPositiveMask_u_32x4)));
}

OCEAN_FORCE_INLINE uint8x16_t NEON::cast16ElementsNEON(const float32x4_t& sourceA_f_32x4, const float32x4_t& sourceB_f_32x4, const float32x4_t& sourceC_f_32x4, const float32x4_t& sourceD_f_32x4)
{
	const uint32x4_t targetA_u_32x4 = vcvtq_u32_f32(sourceA_f_32x4);
	const uint32x4_t targetB_u_32x4 = vcvtq_u32_f32(sourceB_f_32x4);
	const uint32x4_t targetC_u_32x4 = vcvtq_u32_f32(sourceC_f_32x4);
	const uint32x4_t targetD_u_32x4 = vcvtq_u32_f32(sourceD_f_32x4);

	const uint16x8_t targetA_u_16x8 = vcombine_u16(vmovn_u32(targetA_u_32x4), vmovn_u32(targetB_u_32x4));
	const uint16x8_t targetB_u_16x8 = vcombine_u16(vmovn_u32(targetC_u_32x4), vmovn_u32(targetD_u_32x4));

	return vcombine_u8(vmovn_u16(targetA_u_16x8), vmovn_u16(targetB_u_16x8));
}

OCEAN_FORCE_INLINE uint8x16_t NEON::cast16ElementsNEON(const float* const source)
{
	ocean_assert(source != nullptr);

#ifdef OCEAN_DEBUG
	for (unsigned int n = 0u; n < 16u; ++n)
	{
		ocean_assert(source[n] >= 0.0f && source[n] < 256.0f);
	}
#endif

	return cast16ElementsNEON(vld1q_f32(source + 0), vld1q_f32(source + 4), vld1q_f32(source + 8), vld1q_f32(source + 12));
}

OCEAN_FORCE_INLINE float32x4x4_t NEON::cast16ElementsNEON(const uint8x16_t& source_u_8x16)
{
	const uint16x8_t sourceA_u_16x8 = vmovl_u8(vget_low_u8(source_u_8x16));
	const uint16x8_t sourceB_u_16x8 = vmovl_u8(vget_high_u8(source_u_8x16));

	const uint32x4_t sourceA_u_32x4 = vmovl_u16(vget_low_u16(sourceA_u_16x8));
	const uint32x4_t sourceB_u_32x4 = vmovl_u16(vget_high_u16(sourceA_u_16x8));
	const uint32x4_t sourceC_u_32x4 = vmovl_u16(vget_low_u16(sourceB_u_16x8));
	const uint32x4_t sourceD_u_32x4 = vmovl_u16(vget_high_u16(sourceB_u_16x8));

	float32x4x4_t result_u_32x4x4;
	result_u_32x4x4.val[0] = vcvtq_f32_u32(sourceA_u_32x4);
	result_u_32x4x4.val[1] = vcvtq_f32_u32(sourceB_u_32x4);
	result_u_32x4x4.val[2] = vcvtq_f32_u32(sourceC_u_32x4);
	result_u_32x4x4.val[3] = vcvtq_f32_u32(sourceD_u_32x4);

	return result_u_32x4x4;
}

OCEAN_FORCE_INLINE float32x4x4_t NEON::cast16ElementsNEON(const uint8_t* const source)
{
	ocean_assert(source != nullptr);

	return cast16ElementsNEON(vld1q_u8(source));
}

inline unsigned int NEON::interpolation2Channel16Bit1x1(const uint8_t* const pixel, const unsigned int size, const unsigned int fx_y_, const unsigned int fxy_, const unsigned int fx_y, const unsigned int fxy)
{
	ocean_assert(pixel);
	ocean_assert(fx_y_ + fxy_ + fx_y + fxy == 128u * 128u);

	return (pixel[0] * fx_y_ + pixel[2] * fxy_ + pixel[size] * fx_y + pixel[size + 2u] * fxy + 8192u) / 16384u;
}

inline unsigned int NEON::ssd2Channel16Bit1x1(const uint8_t* const pixel0, const uint8_t* const pixel1, const unsigned int /*size0*/, const unsigned int size1, const unsigned int f1x_y_, const unsigned int f1xy_, const unsigned int f1x_y, const unsigned int f1xy)
{
	ocean_assert(pixel0 && pixel1);

	ocean_assert(f1x_y_ + f1xy_ + f1x_y + f1xy == 128u * 128u);

	return sqrDistance((unsigned int)*pixel0, interpolation2Channel16Bit1x1(pixel1, size1, f1x_y_, f1xy_, f1x_y, f1xy));
}

inline unsigned int NEON::ssd2Channel16Bit1x1(const uint8_t* const pixel0, const uint8_t* const pixel1, const unsigned int size0, const unsigned int size1, const unsigned int f0x_y_, const unsigned int f0xy_, const unsigned int f0x_y, const unsigned int f0xy, const unsigned int f1x_y_, const unsigned int f1xy_, const unsigned int f1x_y, const unsigned int f1xy)
{
	ocean_assert(pixel0 && pixel1);

	ocean_assert(f0x_y_ + f0xy_ + f0x_y + f0xy == 128u * 128u);
	ocean_assert(f1x_y_ + f1xy_ + f1x_y + f1xy == 128u * 128u);

	return sqrDistance(interpolation2Channel16Bit1x1(pixel0, size0, f0x_y_, f0xy_, f0x_y, f0xy), interpolation2Channel16Bit1x1(pixel1, size1, f1x_y_, f1xy_, f1x_y, f1xy));
}

}

}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

#endif // META_OCEAN_CV_NEON_H
