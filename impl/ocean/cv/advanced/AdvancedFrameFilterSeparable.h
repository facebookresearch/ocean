/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_FILTER_SEPARABLE_H
#define META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_FILTER_SEPARABLE_H

#include "ocean/cv/advanced/Advanced.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

/**
 * This class implements an advanced separable filter.
 * @ingroup cvadvanced
 */
class AdvancedFrameFilterSeparable
{
	public:

		/**
		 * Applies a horizontal and vertical filtering with a (separable) 2D filter kernel separated into a horizontal 1D filter and a vertical 1D filter for frames with zipped pixel format.
		 * The filter result is stored in place.
		 *
		 * Filter values will be normalized automatically.
		 *
		 * Here is an example showing how to use this function:
		 * @code
		 * void function(Frame& rgbFrame, Frame& maskFrame)
		 * {
		 *     // now let's create a simple Gaussian blur filter with kernel size 3
		 *     const unsigned int horizontalFilter[] = {1u, 2u, 1u};
		 *     const unsigned int verticalFilter[] = {1u, 2u, 1u};
		 *
		 *     FrameFilterSeparable::filter<uint8_t, unsigned int>(rgbFrame.data<uint8_t>(), maskFrame.data<uint8_t>()rgbFrame.width(), rgbFrame.height(), rgbFrame.channels(), rgbFrame.paddingElements(), maskFrame.paddingElements(), horizontalFilter, 3u, verticalFilter, 3u);
		 * }
		 * @endcode
		 * @param frame The frame to be filtered, must be valid
		 * @param mask The mask specifying valid and invalid frame pixels, must be valid
		 * @param width The width of the source (and target) frame in pixel, with range [horizontalFilterSize/2+1, infinity)
		 * @param height The height of the source (and target) frame in pixel, with range [verticalFilterSize/2+1, infinity)
		 * @param channels The number of channels the source frame (and target frame) has, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param horizontalFilter The elements of the horizontal filter, must be valid
		 * @param horizontalFilterSize The number of elements the horizontal filter has, with range [1, width], must be odd
		 * @param verticalFilter The elements of the vertical filter, must be valid
		 * @param verticalFilterSize The number of elements the vertical filter has, with range [1, height], must be odd
		 * @param maskValue The pixel value for an invalid mask pixel, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam T The data type of each pixel channel of the source frame (and target frame) e.g., 'uint8_t', or 'float'
		 * @tparam TFilter The data type of each filter elements e.g., 'unsigned int', or 'float'
		 * @see filterUniversal<T>()
		 */
		template <typename T, typename TFilter>
		static void filter(T* frame, uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int framePaddingElements, const unsigned int maskPaddingElements,const TFilter* horizontalFilter, const unsigned int horizontalFilterSize, const TFilter* verticalFilter, const unsigned int verticalFilterSize, const uint8_t maskValue, Worker* worker = nullptr);

		/**
		 * Applies a horizontal and vertical filtering with a (separable) 2D filter kernel separated into a horizontal 1D filter and a vertical 1D filter for frames with zipped pixel format.
		 * The filter result is stored in a target frame with zipped pixel format.
		 * A source and target mask is used to determine valid and invalid frame pixels.
		 *
		 * Filter values will be normalized automatically.
		 *
		 * Here is an example showing how to use this function:
		 * @code
		 * void function(const Frame& rgbFrame, const Frame& maskFrame)
		 * {
		 *     // now let's create a simple Gaussian blur filter with kernel size 3
		 *     const unsigned int horizontalFilter[] = {1u, 2u, 1u};
		 *     const unsigned int verticalFilter[] = {1u, 2u, 1u};
		 *
		 *     // so let's filter our frame
		 *     Frame targetFrame(rgbFrame.frameType());
		 *     Frame targetMaskFrame(maskFrame.frameType());
		 *     FrameFilterSeparable::filter<uint8_t, unsigned int>(rgbFrame.constdata<uint8_t>(), maskFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), targetMaskFrame.data<uint8_t>(), rgbFrame.width(), rgbFrame.height(), rgbFrame.channels(), rgbFrame.paddingElements(), maskFrame.paddingElements(), targetFrame.paddingElements(), targetMaskFrame.paddingElements(), horizontalFilter, 3u, verticalFilter, 3u);
		 * }
		 * @endcode
		 * @param source The source frame to be filtered, must be valid
		 * @param sourceMask The mask frame specifying valid and invalid source pixels, must be valid
		 * @param target The target frame receiving the filtered results, can be the same memory pointer as 'source', must be valid
		 * @param targetMask the mask frame specifying valid and invalid target pixels, must be valid
		 * @param width The width of the source (and target) frame in pixel, with range [horizontalFilterSize/2+1, infinity)
		 * @param height The height of the source (and target) frame in pixel, with range [verticalFilterSize/2+1, infinity)
		 * @param channels The number of channels the source frame (and target frame) has, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param sourceMaskPaddingElements The number of padding elements at the end of each source mask row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param targetMaskPaddingElements The number of padding elements at the end of each target mask row, in elements, with range [0, infinity)
		 * @param horizontalFilter The elements of the horizontal filter, must be valid
		 * @param horizontalFilterSize The number of elements the horizontal filter has, with range [1, width], must be odd
		 * @param verticalFilter The elements of the vertical filter, must be valid
		 * @param verticalFilterSize The number of elements the vertical filter has, with range [1, height], must be odd
		 * @param maskValue The pixel value for an invalid mask pixel, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam T The data type of each pixel channel of the source frame (and target frame) e.g., 'uint8_t', or 'float'
		 * @tparam TFilter The data type of each filter elements e.g., 'unsigned int', or 'float'
		 * @see filterUniversal<T>()
		 */
		template <typename T, typename TFilter>
		static void filter(const T* source, const uint8_t* sourceMask, T* target, uint8_t* targetMask, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int sourceMaskPaddingElements, const unsigned int targetPaddingElements, const unsigned int targetMaskPaddingElements, const TFilter* horizontalFilter, const unsigned int horizontalFilterSize, const TFilter* verticalFilter, const unsigned int verticalFilterSize, const uint8_t maskValue = 0x00u, Worker* worker = nullptr);

	protected:

		/**
		 * Applies the horizontal filtering in a subset of a frame with a specified 1D filter kernel.
		 * @param source The source frame to be filtered, must be valid
		 * @param sourceMask The mask frame specifying valid and invalid source pixels, must be valid
		 * @param target The target frame receiving the filtered results, must be valid
		 * @param width The width of the source (and target) frame in pixel, with range [filterSize/2+1, infinity)
		 * @param height The height of the source (and target) frame in pixel, with range [filterSize/2+1, infinity)
		 * @param channels The number of data channels both frames have, with range [1, infinity)
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param sourceMaskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param filter The filter factors, must be 'filterSize' individual values
		 * @param filterSize The number of filter factors, with range [1, width - 1], must be odd
		 * @param maskValue The pixel value for an invalid mask pixel, with range [0, infinity)
		 * @param firstRow The first row to be handled, with range [0, height)
		 * @param numberRows The number of rows to be handled, with range [1, height - firstRow]
		 * @tparam TSource The data type of the source elements e.g., 'uint8_t', or 'float'
		 * @tparam TFilter The data type of the filter elements e.g., 'unsigned int', or 'float'
		 */
		template <typename TSource, typename TFilter>
		static void filterHorizontalSubset(const TSource* source, const uint8_t* sourceMask, TFilter* target, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int sourceMaskPaddingElements, const unsigned int targetPaddingElements, const TFilter* filter, const unsigned int filterSize, const uint8_t maskValue, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Applies the vertical filtering in a subset of a frame with a specified 1D filter kernel.
		 * @param source The source frame to be filtered, with one extra channel holding the sum of applied filter factors, must be valid
		 * @param target The target frame receiving the filtered results, must be valid
		 * @param targetMask The mask frame specifying valid and invalid target pixels, must be valid
		 * @param width The width of the source (and target) frame in pixel, with range [filterSize/2+1, infinity)
		 * @param height The height of the source (and target) frame in pixel, with range [filterSize/2+1, infinity)
		 * @param channels The number of data channels both frames have, with range [1, infinity)
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param targetMaskPaddingElements The number of padding elements at the end of each target mask row, in elements, with range [0, infinity)
		 * @param filter The filter factors, must be 'filterSize' individual values
		 * @param filterSize The number of filter factors, with range [1, width - 1], must be odd
		 * @param maskValue The pixel value for an invalid mask pixel, with range [0, infinity)
		 * @param firstRow The first row to be handled, with range [0, height)
		 * @param numberRows The number of rows to be handled, with range [1, height - firstRow]
		 * @tparam TFilter The data type of the source/filter elements e.g., 'uint8_t', or 'float'
		 * @tparam TTarget The data type of the target elements e.g., 'unsigned int', or 'float'
		 */
		template <typename TFilter, typename TTarget>
		static void filterVerticalSubset(const TFilter* source, TTarget* target, uint8_t* targetMask, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int targetMaskPaddingElements, const TFilter* filter, const unsigned int filterSize, const uint8_t maskValue, const unsigned int firstRow, const unsigned int numberRows);
};

template <typename T, typename TFilter>
void AdvancedFrameFilterSeparable::filter(T* frame, uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const TFilter* horizontalFilter, const unsigned int horizontalFilterSize, const TFilter* verticalFilter, const unsigned int verticalFilterSize, const uint8_t maskValue, Worker* worker)
{
	ocean_assert(frame != nullptr && mask != nullptr);

	ocean_assert(horizontalFilterSize >= 1u && horizontalFilterSize % 2u == 1u);
	ocean_assert(verticalFilterSize >= 1u && verticalFilterSize % 2u == 1u);

	const unsigned int horizontalFilterSize_2 = horizontalFilterSize / 2u;
	const unsigned int verticalFilterSize_2 = verticalFilterSize / 2u;
	ocean_assert_and_suppress_unused(width >= horizontalFilterSize_2 + 1u, horizontalFilterSize_2);
	ocean_assert_and_suppress_unused(height >= verticalFilterSize_2 + 1u, verticalFilterSize_2);

	Frame intermediateFrame(FrameType(width, height, FrameType::genericPixelFormat<TFilter>(channels + 1u), FrameType::ORIGIN_UPPER_LEFT));

	// first we apply the horizontal filtering

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&filterHorizontalSubset<T, TFilter>, (const T*)(frame), (const uint8_t*)(mask), intermediateFrame.data<TFilter>(), width, height, channels, framePaddingElements, maskPaddingElements, intermediateFrame.paddingElements(), horizontalFilter, horizontalFilterSize, maskValue, 0u, 0u), 0u, height);
		worker->executeFunction(Worker::Function::createStatic(&filterVerticalSubset<TFilter, T>, intermediateFrame.constdata<TFilter>(), frame, mask, width, height, channels, intermediateFrame.paddingElements(), framePaddingElements, maskPaddingElements, verticalFilter, verticalFilterSize, maskValue, 0u, 0u), 0u, height);
	}
	else
	{
		filterHorizontalSubset<T, TFilter>(frame, mask, intermediateFrame.data<TFilter>(), width, height, channels, framePaddingElements, maskPaddingElements, intermediateFrame.paddingElements(), horizontalFilter, horizontalFilterSize, maskValue, 0u, height);
		filterVerticalSubset<TFilter, T>(intermediateFrame.data<TFilter>(), frame, mask, width, height, channels, intermediateFrame.paddingElements(), framePaddingElements, maskPaddingElements, verticalFilter, verticalFilterSize, maskValue, 0u, height);
	}
}

template <typename T, typename TFilter>
void AdvancedFrameFilterSeparable::filter(const T* source, const uint8_t* sourceMask, T* target, uint8_t* targetMask, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int sourceMaskPaddingElements, const unsigned int targetPaddingElements, const unsigned int targetMaskPaddingElements, const TFilter* horizontalFilter, const unsigned int horizontalFilterSize, const TFilter* verticalFilter, const unsigned int verticalFilterSize, const uint8_t maskValue, Worker* worker)
{
	ocean_assert(source != nullptr && sourceMask != nullptr && target != nullptr && targetMask != nullptr);

	ocean_assert(horizontalFilterSize >= 1u && horizontalFilterSize % 2u == 1u);
	ocean_assert(verticalFilterSize >= 1u && verticalFilterSize % 2u == 1u);

	const unsigned int horizontalFilterSize_2 = horizontalFilterSize / 2u;
	const unsigned int verticalFilterSize_2 = verticalFilterSize / 2u;
	ocean_assert_and_suppress_unused(width >= horizontalFilterSize_2 + 1u, horizontalFilterSize_2);
	ocean_assert_and_suppress_unused(height >= verticalFilterSize_2 + 1u, verticalFilterSize_2);

	Frame intermediateFrame(FrameType(width, height, FrameType::genericPixelFormat<TFilter>(channels + 1u), FrameType::ORIGIN_UPPER_LEFT));

	// first we apply the horizontal filtering

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&filterHorizontalSubset<T, TFilter>, source, sourceMask, intermediateFrame.data<TFilter>(), width, height, channels, sourcePaddingElements, sourceMaskPaddingElements, intermediateFrame.paddingElements(), horizontalFilter, horizontalFilterSize, maskValue, 0u, 0u), 0u, height);
		worker->executeFunction(Worker::Function::createStatic(&filterVerticalSubset<TFilter, T>, intermediateFrame.constdata<TFilter>(), target, targetMask, width, height, channels, intermediateFrame.paddingElements(), targetPaddingElements, targetMaskPaddingElements, verticalFilter, verticalFilterSize, maskValue, 0u, 0u), 0u, height);
	}
	else
	{
		filterHorizontalSubset<T, TFilter>(source, sourceMask, intermediateFrame.data<TFilter>(), width, height, channels, sourcePaddingElements, sourceMaskPaddingElements, intermediateFrame.paddingElements(), horizontalFilter, horizontalFilterSize, maskValue, 0u, height);
		filterVerticalSubset<TFilter, T>(intermediateFrame.data<TFilter>(), target, targetMask, width, height, channels, intermediateFrame.paddingElements(), targetPaddingElements, targetMaskPaddingElements, verticalFilter, verticalFilterSize, maskValue, 0u, height);
	}
}

template <typename TSource, typename TFilter>
void AdvancedFrameFilterSeparable::filterHorizontalSubset(const TSource* source, const uint8_t* sourceMask, TFilter* target, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int sourceMaskPaddingElements, const unsigned int targetPaddingElements, const TFilter* filter, const unsigned int filterSize, const uint8_t maskValue, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(filterSize >= 1u && filterSize % 2u == 1u);

	const unsigned int filterSize_2 = filterSize / 2u;

	const unsigned channels1 = channels + 1u;

	const unsigned int sourceStrideElements = width * channels + sourcePaddingElements;
	const unsigned int sourceMaskStrideElements = width + sourceMaskPaddingElements;
	const unsigned int targetStrideElements = width * channels1 + targetPaddingElements;

	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	source += sourceStrideElements * firstRow;
	sourceMask += sourceMaskStrideElements * firstRow;
	target += targetStrideElements * firstRow;

	std::vector<TFilter> response(channels1);

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			for (TFilter& value : response)
			{
				value = TFilter(0);
			}

			for (int xx = -int(filterSize_2); xx <= int(filterSize_2); ++xx)
			{
				const unsigned int xLocation = (unsigned int)(int(x) + xx);

				if (xLocation < width)
				{
					if (sourceMask[xLocation] != maskValue)
					{
						ocean_assert(xx + int(filterSize_2) < int(filterSize));
						const TFilter filterFactor = filter[xx + int(filterSize_2)];

						const TSource* sourcePixel = source + xLocation * channels;

						for (unsigned int n = 0u; n < channels; ++n)
						{
							response[n] += TFilter(sourcePixel[n]) * filterFactor;
						}

						response[channels] += filterFactor;
					}
				}
			}

			for (unsigned int n = 0u; n < channels1; ++n)
			{
				target[n] = response[n];
			}

			target += channels1;
		}

		source += sourceStrideElements;
		sourceMask += sourceMaskStrideElements;
		target += targetPaddingElements;
	}
}

template <typename TFilter, typename TTarget>
void AdvancedFrameFilterSeparable::filterVerticalSubset(const TFilter* source,  TTarget* target, uint8_t* targetMask, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int targetMaskPaddingElements, const TFilter* filter, const unsigned int filterSize, const uint8_t maskValue, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(filterSize >= 1u && filterSize % 2u == 1u);

	const unsigned int filterSize_2 = filterSize / 2u;

	const unsigned channels1 = channels + 1u;

	const unsigned int sourceStrideElements = width * channels1 + sourcePaddingElements;
	const unsigned int targetStrideElements = width * channels + targetPaddingElements;
	const unsigned int targetMaskStrideElements = width + targetMaskPaddingElements;

	source += sourceStrideElements * firstRow;
	target += targetStrideElements * firstRow;
	targetMask += targetMaskStrideElements * firstRow;

	std::vector<TFilter> response(channels1);

	const uint8_t nonMaskValue = uint8_t(0xFFu) - maskValue;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			for (TFilter& value : response)
			{
				value = TFilter(0);
			}

			const TFilter* sourcePixel = source - int(filterSize_2 * sourceStrideElements) + int(x * channels1);

			for (int yy = -int(filterSize_2); yy <= int(filterSize_2); ++yy)
			{
				const unsigned int yLocation = (unsigned int)(int(y) + yy);

				if (yLocation < height)
				{
					ocean_assert(yy + int(filterSize_2) < int(filterSize));
					const TFilter filterFactor = filter[yy + int(filterSize_2)];

					for (unsigned int n = 0u; n < channels; ++n)
					{
						response[n] += TFilter(sourcePixel[n]) * filterFactor;
					}

					response[channels] += sourcePixel[channels] * filterFactor;
				}

				sourcePixel += sourceStrideElements;
			}

			if (response[channels] == TFilter(0))
			{
				*targetMask = maskValue;
			}
			else
			{
				if constexpr (std::is_floating_point<TFilter>::value)
				{
					const TFilter invDenominator = TFilter(1) / response[channels];

					for (unsigned int n = 0u; n < channels; ++n)
					{
						target[n] = TTarget(response[n] * invDenominator);
					}
				}
				else
				{
					if constexpr (std::is_signed<TFilter>::value)
					{
						for (unsigned int n = 0u; n < channels; ++n)
						{
							target[n] = TTarget(response[n] / response[channels]);
						}
					}
					else
					{
						const TFilter response_2 = response[channels] / TFilter(2);

						for (unsigned int n = 0u; n < channels; ++n)
						{
							target[n] = TTarget((response[n] + response_2) / response[channels]);
						}
					}
				}

				*targetMask = nonMaskValue;
			}

			target += channels;
			++targetMask;
		}

		source += sourceStrideElements;
		target += targetPaddingElements;
		targetMask += targetMaskPaddingElements;
	}
}

}

}

}

#endif // META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_FILTER_SEPARABLE_H
