/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_MEAN_FILTER_H
#define META_OCEAN_CV_FRAME_MEAN_FILTER_H

#include "ocean/cv/CV.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements a mean frame filter.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameFilterMean
{
	public:

		/**
		 * Definition of different mean filter masks.
		 */
		enum FilterMask : uint32_t
		{
			/// Invalid filter mask.
			FM_INVALID = 0u,
			/// One dimensional mean filter mask: 1/3 * [1 1 1].
			FM_KERNEL_3 = 3u,
			/// One dimensional mean filter mask: 1/5 * [1 1 1 1 1].
			FM_KERNEL_5 = 5u
		};

	public:

		/**
		 * Filters a given frame using a mean filter with arbitrary size by internally using a bordered integral image.
		 * @param source The source frame to be filtered
		 * @param target The target frame receiving the filtered frame
		 * @param window Size of the filter window in pixel, must be odd (actual size: window x window)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool filter(const Frame& source, Frame& target, const unsigned int window, Worker* worker = nullptr);

		/**
		 * Filters a given frame using a mean filter with arbitrary size by internally using a bordered integral image.
		 * @param frame The frame to be filtered
		 * @param window Size of the filter window in pixel, must be odd (actual size: window x window)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool filter(Frame& frame, const unsigned int window, Worker* worker = nullptr);

		/**
		 * Filters a given frame using a mean filter with arbitrary windows size by internally using a bordered integral image.
		 * @param source The source frame to be filtered, must be valid
		 * @param target The target frame receiving the filter result, must be valid
		 * @param width The width of source and target frame, in pixel, with range [1, infinity)
		 * @param height The height of source and target frame, in pixel, with range [1, infinity)
		 * @param window Size of the filter window in pixel, with range [1, infinity), must be odd (actual size: window x window)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void filter8BitPerChannel(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int window, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Filters a given frame using a mean filter with arbitrary windows size by internally using a bordered integral image.
		 * @param frame The frame to be filtered, must be valid
		 * @param width The width of source and target frame, in pixel, with range [1, infinity)
		 * @param height The height of source and target frame, in pixel, with range [1, infinity)
		 * @param window Size of the filter window in pixel, with range [1, infinity), must be odd (actual size: window x window)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void filter8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int window, const unsigned int framePaddingElements, Worker* worker = nullptr);

		/**
		 * Filters a frame using a mean filter with arbitrary size by using a bordered integral image.
		 * The size of the integral image's border must be `window / 2`.<br>
		 * @param borderedIntegral The bordered integral image of the original image, with border `window / 2`, must be valid
		 * @param target The target frame receiving the filter result, must be valid
		 * @param width The width of the original image in pixel, with range [1, infinity)
		 * @param height The height of the original image in pixel, with range [1, infinity)
		 * @param window Size of the filter window in pixel, with range [1, infinity), must be odd (actual size: window x window)
		 * @param borderedIntegralPaddingElements The number of padding elements at the end of each integral image row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of channels the original frame (and the bordered integral image) has, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void filterWithIntegral8BitPerChannel(const uint32_t* borderedIntegral, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int window, const unsigned int borderedIntegralPaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

	protected:

		/**
		 * Filters a subset of a frame using a mean filter with arbitrary size by using a bordered integral image.
		 * The size of the integral image's border must be `window / 2`.<br>
		 * @param borderedIntegral The bordered integral image of the original image, with border `window / 2`, must be valid
		 * @param target The target frame receiving the filter result, must be valid
		 * @param width The width of the original image in pixel, with range [1, infinity)
		 * @param height The height of the original image in pixel, with range [1, infinity)
		 * @param window Size of the filter window in pixel, with range [1, infinity), must be odd (actual size: window x window)
		 * @param borderedIntegralPaddingElements The number of padding elements at the end of each integral image row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstRow The first row to be handled, with range [0, height - 1]
		 * @param numberRows The number of rows to be handled, with range [1, height - firstRow]
		 * @tparam tChannels The number of channels the original frame (and the bordered integral image) has, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void filterWithIntegral8BitPerChannelSubset(const uint32_t* borderedIntegral, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int window, const unsigned int borderedIntegralPaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);
};

template <unsigned int tChannels>
void FrameFilterMean::filter8BitPerChannel(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int window, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(window % 2u == 1u);

	const unsigned int border = window / 2u;

	const unsigned int integralWidth = width + window;
	const unsigned int integralHeight = height + window;

	Frame integralFrame(FrameType(integralWidth, integralHeight, FrameType::genericPixelFormat<uint32_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT));
	CV::IntegralImage::createBorderedImageMirror<uint8_t, uint32_t, tChannels>(source, integralFrame.data<uint32_t>(), width, height, border, sourcePaddingElements, integralFrame.paddingElements());

	filterWithIntegral8BitPerChannel<tChannels>(integralFrame.constdata<uint32_t>(), target, width, height, window, integralFrame.paddingElements(), targetPaddingElements, worker);
}

template <unsigned int tChannels>
void FrameFilterMean::filter8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int window, const unsigned int framePaddingElements, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(frame != nullptr);
	ocean_assert(window % 2u == 1u);

	const unsigned int border = window / 2u;

	const unsigned int integralWidth = width + window;
	const unsigned int integralHeight = height + window;

	Frame integralFrame(FrameType(integralWidth, integralHeight, FrameType::genericPixelFormat<uint32_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT));
	CV::IntegralImage::createBorderedImageMirror<uint8_t, uint32_t, tChannels>(frame, integralFrame.data<uint32_t>(), width, height, border, framePaddingElements, integralFrame.paddingElements());

	filterWithIntegral8BitPerChannel<tChannels>(integralFrame.constdata<uint32_t>(), frame, width, height, window, integralFrame.paddingElements(), framePaddingElements, worker);
}

template <unsigned int tChannels>
inline void FrameFilterMean::filterWithIntegral8BitPerChannel(const uint32_t* borderedIntegral, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int window, const unsigned int borderedIntegralPaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(borderedIntegral != nullptr && target != nullptr);
	ocean_assert(window % 2u == 1u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameFilterMean::filterWithIntegral8BitPerChannelSubset<tChannels>, borderedIntegral, target, width, height, window, borderedIntegralPaddingElements, targetPaddingElements, 0u, 0u), 0u, height);
	}
	else
	{
		filterWithIntegral8BitPerChannelSubset<tChannels>(borderedIntegral, target, width, height, window, borderedIntegralPaddingElements, targetPaddingElements, 0u, height);
	}
}

template <unsigned int tChannels>
void FrameFilterMean::filterWithIntegral8BitPerChannelSubset(const uint32_t* borderedIntegral, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int window, const unsigned int borderedIntegralPaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(borderedIntegral != nullptr && target != nullptr);
	ocean_assert(window % 2u == 1u);

	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	const unsigned int borderedIntegralStrideElements = (width + window) * tChannels + borderedIntegralPaddingElements; // window as the integral image has a border of size window/2, and one extra row/column
	const unsigned int targetStrideElements = width * tChannels + targetPaddingElements;

	const unsigned int integralSkipElements = window * tChannels + borderedIntegralPaddingElements;

	const unsigned int area = window * window;
	const unsigned int area_2 = (area + 1u) / 2u;

	const uint32_t* integralTopLeft = borderedIntegral + firstRow * borderedIntegralStrideElements;
	const uint32_t* integralBottomLeft = borderedIntegral + (firstRow + window) * borderedIntegralStrideElements;

	const uint32_t* integralTopRight = integralTopLeft + window * tChannels;
	const uint32_t* integralBottomRight = integralBottomLeft + window * tChannels;

	uint8_t* targetRow = target + firstRow * targetStrideElements;

	for (unsigned int y = 0u; y < numberRows; ++y)
	{
		ocean_assert((integralTopLeft - borderedIntegral) % borderedIntegralStrideElements == 0u);
		ocean_assert((integralBottomLeft - borderedIntegral) % borderedIntegralStrideElements == 0u);

		for (unsigned int x = 0u; x < width; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				ocean_assert((*integralTopLeft - *integralTopRight - *integralBottomLeft + *integralBottomRight + area_2) / area <= 255u);

				*targetRow++ = uint8_t((*integralTopLeft++ - *integralTopRight++ - *integralBottomLeft++ + *integralBottomRight++ + area_2) / area);
			}
		}

		integralTopLeft += integralSkipElements;
		integralBottomLeft += integralSkipElements;

		integralTopRight += integralSkipElements;
		integralBottomRight += integralSkipElements;

		targetRow += targetPaddingElements;
	}
}

}

}

#endif // META_OCEAN_CV_FRAME_MEAN_FILTER_H
