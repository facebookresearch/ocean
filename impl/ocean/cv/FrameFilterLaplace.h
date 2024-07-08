/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_FILTER_LAPLACE_H
#define META_OCEAN_CV_FRAME_FILTER_LAPLACE_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameFilterTemplate.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements Laplace frame filter.
 * The default Laplace filter is a 3x3 box filter with (positive peak):
 * <pre>
 * |  0  -1   0 |
 * | -1   4  -1 |
 * |  0  -1   0 |
 * </pre>
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameFilterLaplace
{
	public:

		/**
		 * Filters a given 1 channel 8 bit frame using a 3x3 Laplace filter with (with positive peak).
		 * The border pixels of the target frame will be set to zero.<br>
		 * The filter response is divided by 8 (normalized by 1/8),
		 * so that each filter response has a value range between [-127, 127].
		 * @param source The source frame to be filtered, must be valid
		 * @param target the target frame receiving the filter result, with same image resolution as the source frame, must be valid
		 * @param width The width of the source frame (and target frame) in pixel, with range [3, infinity)
		 * @param height The height of the source frame (and target frame) in pixel, with range [3, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, with range [0, infinity)
		 */
		static inline void filter1Channel8Bit(const uint8_t* source, int8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Filters a given 1 channel 8 bit frame using a 3x3 Laplace filter with (with positive peak).
		 * The border pixels of the target frame will be set to zero.<br>
		 * The filter response is not normalized, so that each filter response has a value range between [-255 * 4, 255 * 4].
		 * @param source The source frame to be filtered, must be valid
		 * @param target the target frame receiving the filter result, with same image resolution as the source frame, must be valid
		 * @param width The width of the source frame (and target frame) in pixel, with range [3, infinity)
		 * @param height The height of the source frame (and target frame) in pixel, with range [3, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, with range [0, infinity)
		 */
		static inline void filter1Channel8Bit(const uint8_t* source, int16_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Filters a given 1 channel 8 bit frame using a Laplace filter and returns the magnitude.
		 * For all border pixels the corresponding subset of the Laplace filter will be applied.
		 * @param source The source frame to be filtered, must be valid
		 * @param target the target frame receiving the filter result, with same image resolution as the source frame, must be valid
		 * @param width The width of the source frame (and target frame) in pixel, with range [3, infinity)
		 * @param height The height of the source frame (and target frame) in pixel, with range [3, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, with range [0, infinity)
		 */
		static void filterMagnitude1Channel8Bit(const uint8_t* source, uint16_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Filters a given 1 channel 8 bit frame using a Laplace filter and returns the magnitude and normalizes the filter response to the range [0, 255].
		 * @param source The source frame to be filtered, must be valid
		 * @param target The target frame receiving the filter result, must be valid
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static void filterNormalizedMagnitude1Channel8Bit(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Determines the variance of all Laplace responses for a given 1-channel 8 bit frame.
		 * This function ignores all pixels at the image border.
		 * @param frame The frame to be filtered, must be valid
		 * @param width The width of the frame, in pixels, with range [3, 33026]
		 * @param height The height of the frame, in pixels, with range [3, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @return The resulting variance in the Laplace responses, with range [0, infinity)
		 */
		static double variance1Channel8Bit(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		/**
		 * Applies the Laplace filter to one row and determines the sum of responses and the sum of squared responses for a given 1-channel 8 bit row.
		 * This function ignores the first and last pixel in the row.
		 * @param row The row to which the filter will be applied (must have a row above and a row below), must be valid
		 * @param width The width of the frame, in pixels, with range [3, 33026]
		 * @param rowStrideElements The number of stride elements between the start location of two consecutive rows, in elements, with range [width, infinity)
		 * @param sumResponse_s_32x4x2 Two sum of responses, when using NEON
		 * @param sumSqrResponse_u_32x4x2 Two sum of squared responses, when using NEON
		 * @param sumResponse_s_32x1 One sum of responses, as backup when NEON cannot be applied
		 * @param sumSqrResponse_u_32x1 One sum of squared responses, as backup when NEON cannot be applied
		 */
		static void varianceRow1Channel8BitNEON(const uint8_t* row, const unsigned int width, const unsigned int rowStrideElements, int32x4x2_t& sumResponse_s_32x4x2, uint32x4x2_t& sumSqrResponse_u_32x4x2, int32_t& sumResponse_s_32x1, uint32_t& sumSqrResponse_u_32x1);

#endif // OCEAN_HARDWARE_NEON_VERSION

	private:

		/**
		 * Applies the Laplace filter to a row and stores the absolute response.
		 * @param sourceRow The row of the source frame, must be valid
		 * @param targetRow The row of the target response frame, must be valid
		 * @param width The width of the source and target frame in pixel, with range [3, infinity)
		 * @param height The height of the source and target frame in pixel, with range [3, infinity)
		 * @param rowIndex The index of the row to which the filter is applied, with range [0, height - 1]
		 * @param sourceStrideElements The number of elements between the start of two consecutive source rows, with range [width * tSourceChannels, infinity)
		 * @param targetStrideElements The number of elements between the start of two consecutive target rows, with range [width * tTargetChannels, infinity)
		 */
		static void filterAbsoluteRow(const uint8_t* sourceRow, uint16_t* targetRow, const unsigned int width, const unsigned int height, unsigned int rowIndex, const unsigned int sourceStrideElements, const unsigned int targetStrideElements);
};

inline void FrameFilterLaplace::filter1Channel8Bit(const uint8_t* source, int8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 3u && height >= 3u);

	/*
	 * |  0  -1   0 |
 	 * | -1   4  -1 | * 1/8
 	 * |  0  -1   0 |
 	 */
	typedef FrameFilterTemplate<int16_t, 0, -1, 0, -1, 4, -1, 0, -1, 0> LaplaceFilter;

	constexpr int16_t normalization = 8;
	constexpr int16_t normalizationBias = 0;

	constexpr unsigned int channels = 1u;

	LaplaceFilter::filter<uint8_t, int8_t, int16_t, normalization, normalizationBias, channels, PD_NORTH>(source, target, width, height, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameFilterLaplace::filter1Channel8Bit(const uint8_t* source, int16_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 3u && height >= 3u);

	/*
	 * |  0  -1   0 |
 	 * | -1   4  -1 |
 	 * |  0  -1   0 |
 	 */
	typedef FrameFilterTemplate<int16_t, 0, -1, 0, -1, 4, -1, 0, -1, 0> LaplaceFilter;

	constexpr short normalization = 1;
	constexpr short normalizationBias = 0;

	constexpr unsigned int channels = 1u;

	LaplaceFilter::filter<uint8_t, int16_t, int16_t, normalization, normalizationBias, channels, PD_NORTH>(source, target, width, height, sourcePaddingElements, targetPaddingElements, worker);
}

}

}

#endif // META_OCEAN_CV_FRAME_FILTER_LAPLACE_H
