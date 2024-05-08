/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_MIN_MAX_H
#define META_OCEAN_CV_FRAME_MIN_MAX_H

#include "ocean/cv/CV.h"
#include "ocean/cv/PixelPosition.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include <limits>

namespace Ocean
{

namespace CV
{

/**
 * This class implements functions allowing to determine minimum and maximum values within frames.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameMinMax
{
	protected:

		/**
		 * This class allows to determine the extremum (the global minimum or maximum) within a given frame.
		 * The application of this helper class allows to simplify the implementation while providing fast performance,<br>
		 * as it allows to specialize the implementation for the data type 'T' independently from 'tDetermineMinimum'.
		 * @tparam T The data type of each pixel e.g., 'unsigned char', 'int', 'float'
		 */
		template <typename T>
		class ExtremumDeterminer
		{
			public:

			/**
			 * Determines the extremum minimum value (either the global minimum or the global maximum) within a given frame with one channel.
			 * In case several locations with same value exist, one of them will be returned.<br>
			 * This function supports a padding at the end of each row, in case a padding is specified the actual memory must have size: (width + paddingElements) * sizeof(T) * height.
			 * @param frame The pointer to the first pixel of the frame, must be valid
			 * @param width The width of the given frame in pixel, with range [1, infinity)
			 * @param height The height of the given frame in pixel, with range [1, infinity)
			 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
			 * @param extremumValue Optional resulting extremum value (minimal or maximal) found within the frame, 'nullptr' otherwise
			 * @param extremumLocation Optional resulting position where the extremum value (minimal or maximal) is located, with range [0, width - 1]x[0, height - 1], 'nullptr' otherwise
			 * @tparam tDetermineMinimum True, to seek for the minimum value; False, to seek for the maximum value
			 */
			template <bool tDetermineMinimum>
			static void determineExtremumValue(const T* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, T* extremumValue = nullptr, PixelPosition* extremumLocation = nullptr);
		};

	public:

		/**
		 * Determines the minimum value (the global minimum) within a given frame with one channel.
		 * In case several locations with same value exist, one of them will be returned.<br>
		 * This function supports a padding at the end of each row, in case a padding is specified the actual memory must have size: (width + paddingElements) * sizeof(T) * height.<br>
		 * Information: This function is the equivalent to OpenCV's cv::minMaxLoc() - but significantly faster.
		 * @param frame The pointer to the first pixel of the frame, must be valid
		 * @param width The width of the given frame in pixel, with range [1, infinity)
		 * @param height The height of the given frame in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param minValue Optional resulting minimal value found within the frame, 'nullptr' otherwise
		 * @param minLocation Optional resulting position where the minimal value is located, with range [0, width - 1]x[0, height - 1], 'nullptr' otherwise
		 * @tparam T The data type of each pixel e.g., 'unsigned char', 'int', 'float'
		 */
		template <typename T>
		static void determineMinValue(const T* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, T* minValue = nullptr, PixelPosition* minLocation = nullptr);

		/**
		 * Determines the maximum value (the peak value) within a given frame with one channel.
		 * In case several locations with same peak value exist, one of them will be returned.<br>
		 * This function supports a padding at the end of each row, in case a padding is specified the actual memory must have size: (width + paddingElements) * sizeof(T) * height.<br>
		 * Information: This function is the equivalent to OpenCV's cv::minMaxLoc() - but significantly faster.
		 * @param frame The pointer to the first pixel of the frame, must be valid
		 * @param width The width of the given frame in pixel, with range [1, infinity)
		 * @param height The height of the given frame in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param maxValue Optional resulting maximal value found within the frame, 'nullptr' otherwise
		 * @param maxLocation Optional resulting position where the maximal value is located, with range [0, width - 1]x[0, height - 1], 'nullptr' otherwise
		 * @tparam T The data type of each pixel e.g., 'unsigned char', 'int', 'float'
		 */
		template <typename T>
		static void determineMaxValue(const T* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, T* maxValue = nullptr, PixelPosition* maxLocation = nullptr);

		/**
		 * Determines the minimal and maximal pixel values in a given frame.
		 * In case the frame has multiple channels, minimal and maximal values are determined for each channel individually.
		 * @param frame The frame for which the minimal and maximal pixel values are determined, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param minimalValues Resulting minimal values, one for each channel, must be valid
		 * @param maximalValues Resulting maximal values, one for each channel, must be valid
		 * @param worker Optional worker object to distribute the computation
		 * @tparam T Data type of each pixel color value (per channel)
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 * @tparam tIgnoreInfinity True, to ignore +/- infinity and NaN float values; False, to consider +/- infinity float as minimum and maximum values as well, beahvior with NaN values is undefined
		 */
		template <typename T, unsigned int tChannels, bool tIgnoreInfinity = false>
		static inline void determineMinMaxValues(const T* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, T* minimalValues, T* maximalValues, Worker* worker = nullptr);

		/**
		 * Counts frame elements in a 1-channel frame that are outside a specified range of values
		 * @param frame The pointer to the first pixel of the frame, must be valid
		 * @param width The width of the given frame in pixel, with range [1, infinity)
		 * @param height The height of the given frame in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param rangeStart Start value of the range, range: [lowest<T>(), rangeEnd]
		 * @param rangeEnd Exclusive end value  of the range, range: [rangeStart, max<T>()]
		 * @param elementsBelowRange Optional resulting number of elements with values `< rangeStart`
		 * @param elementsAboveRange Optional resulting number of elements with values `>= rangeEnd`
		 * @return True on success, otherwise false
		 */
		template <typename T>
		static bool countElementsOutsideRange(const T* frame, const uint32_t width, const uint32_t height, const uint32_t framePaddingElements, const T rangeStart, const T rangeEnd, uint32_t* elementsBelowRange = nullptr, uint32_t* elementsAboveRange = nullptr);

	protected:

		/**
		 * Determines the minimal and maximal pixel values in a subset of a given frame.
		 * @param frame The frame for which the minimal and maximal pixel values are determined, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param minimalValues Resulting minimal values
		 * @param maximalValues Resulting maximal values
		 * @param lock Optional lock if this function is executed distributed within several threads
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param firstRow First row to be handled, with range [0, height - 1]
		 * @param numberRows Number of rows to be handled, with range [1, height - firstRow]
		 * @tparam T Data type of each pixel color value (per channel)
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 * @tparam tIgnoreInfinity True, to ignore +/- inf values; False, to consider +/- inf as minimum and maximum values as well
		 */
		template <typename T, unsigned int tChannels, bool tIgnoreInfinity = false>
		static void determineMinMaxValuesSubset(const T* frame, const unsigned int width, const unsigned int height, T* minimalValues, T* maximalValues, Lock* lock, const unsigned int framePaddingElements, const unsigned int firstRow, const unsigned int numberRows);
};

template <typename T>
template <bool tDetermineMinimum>
void FrameMinMax::ExtremumDeterminer<T>::determineExtremumValue(const T* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, T* extremumValue, PixelPosition* extremumLocation)
{
	ocean_assert(frame != nullptr);
	ocean_assert(width != 0u && height != 0u);

	ocean_assert_accuracy(extremumValue != nullptr || extremumLocation != nullptr); // one of the two parameters should be defined

	const unsigned int frameStrideElements = width + framePaddingElements;

	T internalExtremumValue = frame[0];
	PixelPosition internalExtremumLocation(0u, 0u);

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			if (tDetermineMinimum ? (frame[x] < internalExtremumValue) : (frame[x] > internalExtremumValue))
			{
				internalExtremumValue = frame[x];
				internalExtremumLocation = PixelPosition(x, y);
			}
		}

		frame += frameStrideElements;
	}

	if (extremumValue)
	{
		*extremumValue = internalExtremumValue;
	}

	if (extremumLocation)
	{
		*extremumLocation = internalExtremumLocation;
	}
}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

template <>
template <bool tDetermineMinimum>
void FrameMinMax::ExtremumDeterminer<unsigned char>::determineExtremumValue(const unsigned char* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, unsigned char* extremumValue, PixelPosition* extremumLocation)
{
	ocean_assert(frame != nullptr);
	ocean_assert(width != 0u && height != 0u);

	ocean_assert_accuracy(extremumValue != nullptr || extremumLocation != nullptr); // one of the two parameters should be defined

	const unsigned int frameStrideElements = width + framePaddingElements;

	unsigned char internalExtremumValue = frame[0];
	PixelPosition internalExtremumLocation(0u, 0u);

	if (width >= 16u && width < 65535u && height < 65535u)
	{
		/*
		 * We handle 16 values concurrently
		 * Strategy: we go through the provided memory and simply keep the smallest values in our NEON registers,
		 * values and coordinates are 'blended' using binary operations (here for finding the minimum value):
		 *
		 * mask[i] = candidates[i] < minValue[i] ? 0xFFFFFFFF : 0x00000000, for i = [0, 15]
		 *
		 * handling values:
		 * minValue[i] = mask[i] == 0xFFFFFFFF ? candidates[i] : minValue[i]
		 *
		 * handling coordinates:
		 * minCoordinateX[i] = mask[i] == 0xFFFFFFFF ? candidateCoordinateX[i] : minCoordinateX[i]
		 */

		// [0, 1, 2, 3, 4, 5, 6, 7]
		const uint16x8_t constant_01234567_u_16x8 = {0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u};
		// [8, 8, 8, 8, 8, 8, 8, 8]
		const uint16x8_t constant_8_u_16x8 = vdupq_n_u16(8u);
		// [16, 16, 16, 16, 16, 16, 16, 16]
		const uint16x8_t constant_16_u_16x8 = vdupq_n_u16(16u);

		// the four locations of the extremum value:
		uint16x8_t extremumLocationX_01234567_u_16x8 = constant_01234567_u_16x8;
		uint16x8_t extremumLocationX_89ABCDEF_u_16x8 = vaddq_u16(constant_01234567_u_16x8, constant_8_u_16x8);

		uint16x8_t extremumLocationY_01234567_u_16x8 = vdupq_n_u16(0u);
		uint16x8_t extremumLocationY_89ABCDEF_u_16x8 = vdupq_n_u16(0u);

		uint8x16_t extremumValue_u_8x16 = vld1q_u8(frame + 0);

		for (unsigned int y = 0u; y < height; ++y)
		{
			// [y, y, y, y, y, y, y, y]
			const uint16x8_t candidateLocation_y_u_16x8 = vdupq_n_u16(uint16_t(y));

			// [0, 1, 2, 3, 4, 5, 6, 7]
			uint16x8_t candidateLocation_01234567_x_u_16x8 = constant_01234567_u_16x8;
			uint16x8_t candidateLocation_89ABCDEF_x_u_16x8 = vaddq_u16(constant_01234567_u_16x8, constant_8_u_16x8);

			for (unsigned int x = 0u; x < width; x += 16u)
			{
				if (x + 16u > width)
				{
					// the last iteration will not fit into the output frame,
					// so we simply shift x left by some pixels (at most 15) and we will calculate some pixels again

					ocean_assert(x >= 16u && width > 16u);
					const unsigned int newX = width - 16u;

					ocean_assert(x > newX);
					const unsigned int offset = x - newX;
					ocean_assert(offset < 16u);

					x = newX;

					candidateLocation_01234567_x_u_16x8 = vsubq_u16(candidateLocation_01234567_x_u_16x8, vdupq_n_u16(uint16_t(offset)));
					candidateLocation_89ABCDEF_x_u_16x8 = vsubq_u16(candidateLocation_89ABCDEF_x_u_16x8, vdupq_n_u16(uint16_t(offset)));

					// the for loop will stop after this iteration
					ocean_assert(!(x + 16u < width));
				}

				const uint8x16_t candidates_u_8x16 = vld1q_u8(frame + x);

				// if (candidates[i] < minValue[i]) - for the example of finding the minimum
				//    mask[i] = 0xFFFFFFFF
				// else
				//    mask[i] = 0x00000000

				const uint8x16_t mask_u_8x16 = tDetermineMinimum ? vcltq_u8(candidates_u_8x16, extremumValue_u_8x16) : vcgtq_u8(candidates_u_8x16, extremumValue_u_8x16);

				// minValue[i] = mask[i] == 0xFFFFFFFF ? candidates[i] : minValue[i]
				extremumValue_u_8x16 = vbslq_u8(mask_u_8x16, candidates_u_8x16, extremumValue_u_8x16);

				// separating our 8x16 mask to two 16x8 masks
				uint16x8_t mask_01234567_u_16x8 = vmovl_u8(vget_low_u8(mask_u_8x16));
				uint16x8_t mask_89ABCDEF_u_16x8 = vmovl_u8(vget_high_u8(mask_u_8x16));
				mask_01234567_u_16x8 = vorrq_u16(mask_01234567_u_16x8, vshlq_n_u16(mask_01234567_u_16x8, 8));
				mask_89ABCDEF_u_16x8 = vorrq_u16(mask_89ABCDEF_u_16x8, vshlq_n_u16(mask_89ABCDEF_u_16x8, 8));

				// minLocationX[i] = mask[i] == 0xFFFFFFFF ? candidateLocationX[i] : minLocationX[i]
				extremumLocationX_01234567_u_16x8 = vbslq_u16(mask_01234567_u_16x8, candidateLocation_01234567_x_u_16x8, extremumLocationX_01234567_u_16x8);
				extremumLocationX_89ABCDEF_u_16x8 = vbslq_u16(mask_89ABCDEF_u_16x8, candidateLocation_89ABCDEF_x_u_16x8, extremumLocationX_89ABCDEF_u_16x8);

				// minLocationY[i] = mask[i] == 0xFFFFFFFF ? candidateLocationY[i] : minLocationY[i]
				extremumLocationY_01234567_u_16x8 = vbslq_u16(mask_01234567_u_16x8, candidateLocation_y_u_16x8, extremumLocationY_01234567_u_16x8);
				extremumLocationY_89ABCDEF_u_16x8 = vbslq_u16(mask_89ABCDEF_u_16x8, candidateLocation_y_u_16x8, extremumLocationY_89ABCDEF_u_16x8);

				// += [16, 16, 16, 16]
				candidateLocation_01234567_x_u_16x8 = vaddq_u16(candidateLocation_01234567_x_u_16x8, constant_16_u_16x8);
				candidateLocation_89ABCDEF_x_u_16x8 = vaddq_u16(candidateLocation_89ABCDEF_x_u_16x8, constant_16_u_16x8);
			}

			frame += frameStrideElements;
		}

		// we compute the best 8 results out of our best 16 results

		const uint8x8_t extremumValue_01234567_u_8x8 = vget_low_u8(extremumValue_u_8x16);
		const uint8x8_t extremumValue_89ABCDEF_u_8x8 = vget_high_u8(extremumValue_u_8x16);

		const uint8x8_t mask_u_8x8 = tDetermineMinimum ? vclt_u8(extremumValue_01234567_u_8x8, extremumValue_89ABCDEF_u_8x8) : vcgt_u8(extremumValue_01234567_u_8x8, extremumValue_89ABCDEF_u_8x8);
		const uint8x8_t extremumValue_u_8x8 = vbsl_u8(mask_u_8x8, extremumValue_01234567_u_8x8, extremumValue_89ABCDEF_u_8x8);

		uint16x8_t mask_u_16x8 = vmovl_u8(mask_u_8x8);
		mask_u_16x8 = vorrq_u16(mask_u_16x8, vshlq_n_u16(mask_u_16x8, 8));

		const uint16x8_t extremumLocationX_u_16x8 = vbslq_u16(mask_u_16x8, extremumLocationX_01234567_u_16x8, extremumLocationX_89ABCDEF_u_16x8);
		const uint16x8_t extremumLocationY_u_16x8 = vbslq_u16(mask_u_16x8, extremumLocationY_01234567_u_16x8, extremumLocationY_89ABCDEF_u_16x8);

		// we store our four best values and finally need to select the best of them

		uint16_t extremumLocationsX[8];
		vst1q_u16(extremumLocationsX, extremumLocationX_u_16x8);

		uint16_t extremumLocationsY[8];
		vst1q_u16(extremumLocationsY, extremumLocationY_u_16x8);

		unsigned char extremumValues[8];
		vst1_u8(extremumValues, extremumValue_u_8x8);

		internalExtremumValue = extremumValues[0];
		internalExtremumLocation = CV::PixelPosition((unsigned int)(extremumLocationsX[0]), (unsigned int)(extremumLocationsY[0]));

		for (unsigned int n = 1u; n < 8u; ++n)
		{
			if (tDetermineMinimum ? (extremumValues[n] < internalExtremumValue) : (extremumValues[n] > internalExtremumValue))
			{
				internalExtremumValue = extremumValues[n];
				internalExtremumLocation = CV::PixelPosition((unsigned int)(extremumLocationsX[n]), (unsigned int)(extremumLocationsY[n]));
			}
		}
	}
	else
	{
		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int x = 0u; x < width; ++x)
			{
				if (tDetermineMinimum ? (frame[x] < internalExtremumValue) : (frame[x] > internalExtremumValue))
				{
					internalExtremumValue = frame[x];
					internalExtremumLocation = PixelPosition(x, y);
				}
			}

			frame += frameStrideElements;
		}
	}

	if (extremumValue)
	{
		*extremumValue = internalExtremumValue;
	}

	if (extremumLocation)
	{
		*extremumLocation = internalExtremumLocation;
	}
}

template <>
template <bool tDetermineMinimum>
void FrameMinMax::ExtremumDeterminer<float>::determineExtremumValue(const float* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, float* extremumValue, PixelPosition* extremumLocation)
{
	ocean_assert(frame != nullptr);
	ocean_assert(width != 0u && height != 0u);

	ocean_assert_accuracy(extremumValue != nullptr || extremumLocation != nullptr); // one of the two parameters should be defined

	const unsigned int frameStrideElements = width + framePaddingElements;

	float internalExtremumValue = frame[0];
	PixelPosition internalExtremumLocation(0u, 0u);

	if (width >= 8u)
	{
		/*
		 * We handle 8 float values concurrently
		 * Strategy: we go through the provided memory and simply keep the smallest values in our NEON registers,
		 * values and coordinates are 'blended' using binary operations (here for finding the minimum value):
		 *
		 * mask[i] = candidates[i] < minValue[i] ? 0xFFFFFFFF : 0x00000000, for i = [0, 8]
		 *
		 * handling values:
		 * minValue[i] = mask[i] == 0xFFFFFFFF ? candidates[i] : minValue[i]
		 *
		 * handling coordinates:
		 * minCoordinateX[i] = mask[i] == 0xFFFFFFFF ? candidateCoordinateX[i] : minCoordinateX[i]
		 */

		// [0, 1, 2, 3]
		const uint32x4_t constant_0123_u_32x4 = {0u, 1u, 2u, 3u};
		// [4, 4, 4, 4]
		const uint32x4_t constant_4_u_32x4 = vdupq_n_u32(4u);
		// [8, 8, 8, 8]
		const uint32x4_t constant_8_u_32x4 = vdupq_n_u32(8u);

		// the four locations of the extremum value:
		uint32x4_t extremumLocationX_0123_u_32x4 = constant_0123_u_32x4;
		uint32x4_t extremumLocationX_4567_u_32x4 = vaddq_u32(constant_0123_u_32x4, constant_4_u_32x4);

		uint32x4_t extremumLocationY_0123_u_32x4 = vdupq_n_u32(0u);
		uint32x4_t extremumLocationY_4567_u_32x4 = vdupq_n_u32(0u);

		float32x4_t extremumValue_0123_f_32x4 = vld1q_f32(frame + 0);
		float32x4_t extremumValue_4567_f_32x4 = vld1q_f32(frame + 4);

		for (unsigned int y = 0u; y < height; ++y)
		{
			// [y, y, y, y]
			const uint32x4_t candidateLocation_y_u_32x4 = vdupq_n_u32(y);

			// [0, 1, 2, 3]
			uint32x4_t candidateLocation_0123_x_u_32x4 = constant_0123_u_32x4;
			uint32x4_t candidateLocation_4567_x_u_32x4 = vaddq_u32(constant_0123_u_32x4, constant_4_u_32x4);

			for (unsigned int x = 0u; x < width; x += 8u)
			{
				if (x + 8u > width)
				{
					// the last iteration will not fit into the output frame,
					// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

					ocean_assert(x >= 8u && width > 8u);
					const unsigned int newX = width - 8u;

					ocean_assert(x > newX);
					const unsigned int offset = x - newX;
					ocean_assert(offset < 8u);

					x = newX;

					candidateLocation_0123_x_u_32x4 = vsubq_u32(candidateLocation_0123_x_u_32x4, vdupq_n_u32(offset));
					candidateLocation_4567_x_u_32x4 = vsubq_u32(candidateLocation_4567_x_u_32x4, vdupq_n_u32(offset));

					// the for loop will stop after this iteration
					ocean_assert(!(x + 8u < width));
				}

				const float32x4_t candidates_0123_f_32x4 = vld1q_f32(frame + x + 0u);
				const float32x4_t candidates_4567_f_32x4 = vld1q_f32(frame + x + 4u);

				// if (candidates[i] < minValue[i]) - for the example of finding the minimum
				//    mask[i] = 0xFFFFFFFF
				// else
				//    mask[i] = 0x00000000

				const uint32x4_t mask_0123_u_32x4 = tDetermineMinimum ? vcltq_f32(candidates_0123_f_32x4, extremumValue_0123_f_32x4) : vcgtq_f32(candidates_0123_f_32x4, extremumValue_0123_f_32x4);
				const uint32x4_t mask_4567_u_32x4 = tDetermineMinimum ? vcltq_f32(candidates_4567_f_32x4, extremumValue_4567_f_32x4) : vcgtq_f32(candidates_4567_f_32x4, extremumValue_4567_f_32x4);

				// minValue[i] = mask[i] == 0xFFFFFFFF ? candidates[i] : minValue[i]
				extremumValue_0123_f_32x4 = vbslq_f32(mask_0123_u_32x4, candidates_0123_f_32x4, extremumValue_0123_f_32x4);
				extremumValue_4567_f_32x4 = vbslq_f32(mask_4567_u_32x4, candidates_4567_f_32x4, extremumValue_4567_f_32x4);

				extremumLocationX_0123_u_32x4 = vbslq_u32(mask_0123_u_32x4, candidateLocation_0123_x_u_32x4, extremumLocationX_0123_u_32x4);
				extremumLocationX_4567_u_32x4 = vbslq_u32(mask_4567_u_32x4, candidateLocation_4567_x_u_32x4, extremumLocationX_4567_u_32x4);

				extremumLocationY_0123_u_32x4 = vbslq_u32(mask_0123_u_32x4, candidateLocation_y_u_32x4, extremumLocationY_0123_u_32x4);
				extremumLocationY_4567_u_32x4 = vbslq_u32(mask_4567_u_32x4, candidateLocation_y_u_32x4, extremumLocationY_4567_u_32x4);

				// += [8, 8, 8, 8]
				candidateLocation_0123_x_u_32x4 = vaddq_u32(candidateLocation_0123_x_u_32x4, constant_8_u_32x4);
				candidateLocation_4567_x_u_32x4 = vaddq_u32(candidateLocation_4567_x_u_32x4, constant_8_u_32x4);
			}

			frame += frameStrideElements;
		}

		const uint32x4_t mask_u_32x4 = tDetermineMinimum ? vcltq_f32(extremumValue_0123_f_32x4, extremumValue_4567_f_32x4) : vcgtq_f32(extremumValue_0123_f_32x4, extremumValue_4567_f_32x4);
		extremumValue_0123_f_32x4 = vbslq_f32(mask_u_32x4, extremumValue_0123_f_32x4, extremumValue_4567_f_32x4);
		extremumLocationX_0123_u_32x4 = vbslq_u32(mask_u_32x4, extremumLocationX_0123_u_32x4, extremumLocationX_4567_u_32x4);
		extremumLocationY_0123_u_32x4 = vbslq_u32(mask_u_32x4, extremumLocationY_0123_u_32x4, extremumLocationY_4567_u_32x4);

		// we store our four best values and finally need to select the best of them

		unsigned int extremumLocationsX[4];
		vst1q_u32(extremumLocationsX, extremumLocationX_0123_u_32x4);

		unsigned int extremumLocationsY[4];
		vst1q_u32(extremumLocationsY, extremumLocationY_0123_u_32x4);

		float extremumValues[4];
		vst1q_f32(extremumValues, extremumValue_0123_f_32x4);

		internalExtremumValue = extremumValues[0];
		internalExtremumLocation = CV::PixelPosition(extremumLocationsX[0], extremumLocationsY[0]);

		for (unsigned int n = 1u; n < 4u; ++n)
		{
			if (tDetermineMinimum ? (extremumValues[n] < internalExtremumValue) : (extremumValues[n] > internalExtremumValue))
			{
				internalExtremumValue = extremumValues[n];
				internalExtremumLocation = CV::PixelPosition(extremumLocationsX[n], extremumLocationsY[n]);
			}
		}
	}
	else
	{
		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int x = 0u; x < width; ++x)
			{
				if (tDetermineMinimum ? (frame[x] < internalExtremumValue) : (frame[x] > internalExtremumValue))
				{
					internalExtremumValue = frame[x];
					internalExtremumLocation = PixelPosition(x, y);
				}
			}

			frame += frameStrideElements;
		}
	}

	if (extremumValue)
	{
		*extremumValue = internalExtremumValue;
	}

	if (extremumLocation)
	{
		*extremumLocation = internalExtremumLocation;
	}
}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

template <typename T>
void FrameMinMax::determineMinValue(const T* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, T* minValue, PixelPosition* minLocation)
{
	return ExtremumDeterminer<T>::template determineExtremumValue<true>(frame, width, height, framePaddingElements, minValue, minLocation);
}

template <typename T>
void FrameMinMax::determineMaxValue(const T* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, T* maxValue, PixelPosition* maxLocation)
{
	return ExtremumDeterminer<T>::template determineExtremumValue<false>(frame, width, height, framePaddingElements, maxValue, maxLocation);
}

template <typename T, unsigned int tChannels, bool tIgnoreInfinity>
inline void FrameMinMax::determineMinMaxValues(const T* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, T* minimalValues, T* maximalValues, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(frame != nullptr && minimalValues != nullptr && maximalValues != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		 minimalValues[n] = NumericT<T>::maxValue();
	}

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		maximalValues[n] = NumericT<T>::minValue();
	}

	if (worker)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::createStatic(determineMinMaxValuesSubset<T, tChannels, tIgnoreInfinity>, frame, width, height, minimalValues, maximalValues, &lock, framePaddingElements, 0u, 0u), 0u, height, 7u, 8u, 20u);
	}
	else
	{
		determineMinMaxValuesSubset<T, tChannels, tIgnoreInfinity>(frame, width, height, minimalValues, maximalValues, nullptr, framePaddingElements, 0u, height);
	}
}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

/// Forward declaration of template socialization
template <>
bool FrameMinMax::countElementsOutsideRange<uint8_t>(const uint8_t* frame, const uint32_t width, const uint32_t height, const uint32_t framePaddingElements, const uint8_t rangeStart, const uint8_t rangeEnd, uint32_t* elementsBelowRange, uint32_t* elementsAboveRange);

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

template <typename T>
bool FrameMinMax::countElementsOutsideRange(const T* frame, const uint32_t width, const uint32_t height, const uint32_t framePaddingElements, const T rangeStart, const T rangeEnd, uint32_t* elementsBelowRange, uint32_t* elementsAboveRange)
{
	if (frame == nullptr || width == 0u || height == 0u || rangeStart > rangeEnd || (elementsBelowRange == nullptr && elementsAboveRange == nullptr))
	{
		ocean_assert(false && "Invalid input");
		return false;
	}

	const uint32_t frameStrideElements = width + framePaddingElements;
	const T* const frameEnd = frame + height * frameStrideElements - framePaddingElements;

	uint32_t elementsBelowRangeLocal = 0u;
	uint32_t elementsAboveRangeLocal = 0u;

	if (framePaddingElements == 0u)
	{
		while (frame < frameEnd)
		{
			if (*frame < rangeStart)
			{
				++elementsBelowRangeLocal;
			}
			else if (*frame >= rangeEnd)
			{
				++elementsAboveRangeLocal;
			}

			++frame;
		}
	}
	else
	{
		for (uint32_t y = 0u; y < height; ++y)
		{
			const T* frameRowEnd = frame + width;

			while (frame < frameRowEnd)
			{
				if (*frame < rangeStart)
				{
					++elementsBelowRangeLocal;
				}
				else if (*frame >= rangeEnd)
				{
					++elementsAboveRangeLocal;
				}

				++frame;
			}

			frame += framePaddingElements;
		}
	}

	if (elementsBelowRange)
	{
		*elementsBelowRange = elementsBelowRangeLocal;
	}

	if (elementsAboveRange)
	{
		*elementsAboveRange = elementsAboveRangeLocal;
	}

	return true;
}

template <typename T, unsigned int tChannels, bool tIgnoreInfinity>
void FrameMinMax::determineMinMaxValuesSubset(const T* frame, const unsigned int width, const unsigned int height, T* minimalValues, T* maximalValues, Lock* lock, const unsigned int framePaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(frame != nullptr && minimalValues != nullptr && maximalValues != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	T localMinimal[tChannels];
	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		 localMinimal[n] = NumericT<T>::maxValue();
	}

	T localMaximal[tChannels];
	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		localMaximal[n] = NumericT<T>::minValue();
	}

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;

	frame += firstRow * frameStrideElements;

	const T* const frameEnd = frame + numberRows * frameStrideElements;
	while (frame != frameEnd)
	{
		ocean_assert(frame < frameEnd);

		const T* const frameRowEnd = frame + width * tChannels;

		while (frame != frameRowEnd)
		{
			ocean_assert(frame < frameRowEnd);
			ocean_assert(frame < frameEnd);

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				if constexpr (tIgnoreInfinity && std::is_floating_point<T>::value)
				{
					if (!NumericT<T>::isInf(frame[n]) && !NumericT<T>::isNan(frame[n]))
					{
						if (frame[n] < localMinimal[n])
						{
							localMinimal[n] = frame[n];
						}

						if (frame[n] > localMaximal[n])
						{
							localMaximal[n] = frame[n];
						}
					}
				}
				else
				{
					if (frame[n] < localMinimal[n])
					{
						localMinimal[n] = frame[n];
					}

					if (frame[n] > localMaximal[n])
					{
						localMaximal[n] = frame[n];
					}
				}
			}

			frame += tChannels;
		}

		frame += framePaddingElements;
	}

	const OptionalScopedLock scopedLock(lock);

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		minimalValues[n] = min(minimalValues[n], localMinimal[n]);
		maximalValues[n] = max(maximalValues[n], localMaximal[n]);
	}
}

}

}

#endif // META_OCEAN_CV_FRAME_MIN_MAX_H
