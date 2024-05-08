/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_FILTER_SEPARABLE_H
#define META_OCEAN_CV_FRAME_FILTER_SEPARABLE_H

#include "ocean/cv/CV.h"
#include "ocean/cv/NEON.h"
#include "ocean/cv/SSE.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Memory.h"
#include "ocean/base/Processor.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements separable filter.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameFilterSeparable
{
	public:

		/**
		 * This class holds re-usable memory for the filtering process.
		 */
		class ReusableMemory
		{
			friend class FrameFilterSeparable;

			public:

				/**
				 * Default constructor.
				 */
				ReusableMemory() = default;

			protected:

				/// An intermediate frame which can be re-used during filtering.
				Frame intermediateFrame_;

				/// Float-based filter factors which can be re-used during filtering.
				std::vector<float> filterFactors_;

				/// Normalized horizontal filter factors which can be re-used during filtering.
				std::vector<float> normalizedHorizontalFilter_;

				/// Normalized vertical filter factors which can be re-used during filtering.
				std::vector<float> normalizedVerticalFilter_;
		};

	protected:

		/**
		 * Definition of a 128 bit SIMD data type holding four 32 bit values.
		 */
		template <typename T>
		struct SIMD32x4
		{
			typedef DataType<uint32_t, 4u>::Type Type;
		};

	public:

		/**
		 * Returns whether a given 1D filter is symmetric.
		 * @param filterValues The individual values of the 1D filter, must be valid
		 * @param size The size of the filter (the number of filter elements), with range [1, infinity), must be odd
		 * @return True, if so
		 * @tparam T The data type of each filter value, e.g., 'unsigned int', or 'float'
		 */
		template <typename T>
		static bool isFilterSymmetric(const T* filterValues, const size_t size);

		/**
		 * Determines the sum of all elements of a given 1D filter.
		 * @param filterValues The individual values of the 1D filter, must be valid
		 * @param size The size of the filter (the number of filter elements), with range [1, infinity)
		 * @return The sum of all filter values
		 * @tparam T The data type of each filter value, e.g., 'unsigned int', or 'float'
		 */
		template <typename T>
		static T sumFilterValues(const T* filterValues, const size_t size);

		/**
		 * Applies a horizontal and vertical filtering with a (separable) 2D filter kernel separated into a horizontal 1D filter and a vertical 1D filter for frames with zipped pixel format.
		 * The filter result is stored in a target frame with zipped pixel format.
		 *
		 * The provided filter values are given with integer precision, the filter responses will be normalized automatically.<br>
		 *
		 * Here is an example showing how to use this function:
		 * @code
		 * void function(const Frame& rgbFrame)
		 * {
		 *     // now let's create a simple Gaussian blur filter with kernel size 3
		 *     const std::vector<unsigned int> filter[] = {1u, 2u, 1u};
		 *
		 *     // so let's filter our frame
		 *     Frame targetFrame(rgbFrame.frameType());
		 *     FrameFilterSeparable::filter(rgbFrame, targetFrame, filter, filter);
		 * }
		 * @endcode
		 * @param source The source frame to be filtered, with zipped pixel format and with data type DT_UNSIGNED_INTEGER_8, or DT_SIGNED_FLOAT_32, must be valid
		 * @param target The target frame receiving the filtered results, will be set to the correct frame type, if invalid or if the type does not match the source frame
		 * @param horizontalFilter The horizontal filter, the number of filter elements must be odd, at least 1 element
		 * @param verticalFilter The vertical filter, the number of filter elements must be odd, at least 1 element
		 * @param worker Optional worker object to distribute the computation
		 * @param reusableMemory An optional object holding reusable memory which can be used during filtering, nullptr otherwise
		 * @param processorInstructions The set of available instructions, may be any combination of instructions
		 * @see filter<T, TFilter>()
		 */
		static bool filter(const Frame& source, Frame& target, const std::vector<unsigned int>& horizontalFilter, const std::vector<unsigned int>& verticalFilter, Worker* worker = nullptr, ReusableMemory* reusableMemory = nullptr, const ProcessorInstructions processorInstructions = Processor::get().instructions());

		/**
		 * Applies a horizontal and vertical filtering with a (separable) 2D filter kernel separated into a horizontal 1D filter and a vertical 1D filter for frames with zipped pixel format.
		 * The filter result is stored in a target frame with zipped pixel format.
		 *
		 * When providing filter values with integer precision, the filter responses will be normalized automatically.<br>
		 * In contrast, when providing filter values with floating point precision, the filter responses will not be normalized.<br>
		 * Thus, you need to provide a normalized filter already when providing floating point filters.
		 *
		 * Here is an example showing how to use this function:
		 * @code
		 * void function(const Frame& rgbFrame)
		 * {
		 *     // now let's create a simple Gaussian blur filter with kernel size 3
		 *     const unsigned int horizontalFilter[] = {1u, 2u, 1u};
		 *     const unsigned int verticalFilter[] = {1u, 2u, 1u};
		 *
		 *     // so let's filter our frame
		 *     Frame targetFrame(rgbFrame.frameType());
		 *     FrameFilterSeparable::filter<uint8_t, unsigned int>(rgbFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), rgbFrame.width(), rgbFrame.height(), rgbFrame.channels(), horizontalFilter, 3u, verticalFilter, 3u, rgbFrame.paddingElements(), targetFrame.paddingElements());
		 * }
		 * @endcode
		 * @param source The source frame to be filtered, must be valid
		 * @param target The target frame receiving the filtered results, can be the same memory pointer as 'source', must be valid
		 * @param width The width of the source (and target) frame in pixel, with range [horizontalFilterSize, infinity)
		 * @param height The height of the source (and target) frame in pixel, with range [verticalFilterSize, infinity)
		 * @param channels The number of channels the source frame (and target frame) has, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param horizontalFilter The elements of the horizontal filter, must be valid
		 * @param horizontalFilterSize The number of elements the horizontal filter has, with range [1, width], must be odd
		 * @param verticalFilter The elements of the vertical filter, must be valid
		 * @param verticalFilterSize The number of elements the vertical filter has, with range [1, height], must be odd
		 * @param worker Optional worker object to distribute the computation
		 * @param reusableMemory An optional object holding reusable memory which can be used during filtering, nullptr otherwise
		 * @param processorInstructions The set of available instructions, may be any combination of instructions
		 * @tparam T The data type of each pixel channel of the source frame (and target frame) e.g., 'uint8_t', or 'float'
		 * @tparam TFilter The data type of each filter elements e.g., 'unsigned int', or 'float'
		 * @see filterUniversal<T>()
		 */
		template <typename T, typename TFilter>
		static bool filter(const T* source, T* target, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const TFilter* horizontalFilter, const unsigned int horizontalFilterSize, const TFilter* verticalFilter, const unsigned int verticalFilterSize, Worker* worker = nullptr, ReusableMemory* reusableMemory = nullptr, const ProcessorInstructions processorInstructions = Processor::get().instructions());

		/**
		 * Applies a horizontal and vertical filtering with a (separable) 2D filter kernel separated into a horizontal 1D filter and a vertical 1D filter for frames with almost arbitrary pixel format.
		 * This function supports images with arbitrary pixel format as long as the pixel format is zipped (e.g,. FrameType::FORMAT_Y8, FrameType::FORMAT_RGB24, ...).
		 *
		 * Beware: This function is not highly optimized, you may want to check whether Ocean provided a more optimized implementation for your purpose if performance matters e.g., filer<T, TFilter>().
		 *
		 * Here is an example showing how to use this function:
		 * @code
		 * void function(const Frame& rgbFrame)
		 * {
		 *     // let's say we receive a frame with FORMAT_RGB24 pixel format
		 *     if (rgbFrame.pixelFormat() != FrameType::FORMAT_RGB24)
		 *     {
		 *         // wrong pixel format
		 *         return;
		 *     }
		 *
		 *     // let's convert this frame to a floating point frame
		 *     Frame floatFrameWith3Channels(FrameType(rgbFrame, FrameType::genericPixelFormat<float, 3u>()));
		 *
		 *     FrameConverter::cast<uint8_t, float>(rgbFrame.constdata<uint8_t>(), floatFrameWith3Channels.data<float>(), rgbFrame.width(), rgbFrame.height(), rgbFrame.channels());
		 *
		 *     // now let's create a simple Gaussian blur filter with kernel size 3
		 *     const float horizontalFilter[] = {0.25f, 0.5f, 0.25f};
		 *     const float verticalFilter[] = {0.25f, 0.5f, 0.25f};
		 *
		 *     const unsigned int channels = 3u;
		 *
		 *     // so let's filter our floating point frame
		 *     Frame floatTargetFrame(floatFrameWith3Channels.frameType());
		 *     FrameFilterSeparable::filterUniversal<float>(floatFrameWith3Channels.constdata<float>(), floatTargetFrame.data<float>(), floatTargetFrame.width(), floatTargetFrame.height(), channels, horizontalFilter, 3u, verticalFilter, 3u);
		 *
		 *     // btw: we could also apply the same filter to our RGB24 frame (with uint8_t values)
		 *     // however, this time we lose the floating point accuracy
		 *     Frame rgbTargetFrame(rgbFrame.frameType());
		 *     FrameFilterSeparable::filterUniversal<uint8_t>(rgbFrame.constdata<uint8_t>(), rgbTargetFrame.data<uint8_t>(), rgbFrame.width(), rgbFrame.height(), channels, horizontalFilter, 3u, verticalFilter, 3u, rgbFrame.paddingElements(), rgbTargetFrame.paddingElements());
		 * }
		 * @endcode
		 * @param source The source frame to which the filter will be applied, must be valid
		 * @param target The target frame receiving the filtered results, can be the same memory pointer as 'source', must be valid
		 * @param width The width of the source frame (and target frame) in pixel, with range [1, infinity)
		 * @param height The height of the source frame (and target frame) in pixel, with range [1, infinity)
		 * @param channels The number of channels the source and target frame have, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param horizontalFilter The (separable) horizontal filter to be applied, must be valid
		 * @param horizontalFilterSize The number of horizontal filter elements, with range [1, width], must be odd
		 * @param verticalFilter The (separable) vertical filter to be applied, must be valid
		 * @param verticalFilterSize The number of vertical filter elements, with range [1, height], must be odd
		 * @param worker Optional worker object to distribute the computation to several CPU cores
		 * @return True, if the filter could be applied; False, if the input parameters were wrong
		 * @tparam T The data type of each pixel channel of the source and target frame, e.g., 'uint8_t', 'int', 'float', ...
		 * @see filter<T, TFilter>()
		 */
		template <typename T>
		static bool filterUniversal(const T* source, T* target, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const float* horizontalFilter, const unsigned int horizontalFilterSize, const float* verticalFilter, const unsigned int verticalFilterSize, Worker* worker = nullptr);

	protected:

		/**
		 * Applies a horizontal and vertical filtering with a (separable) 2D filter kernel separated into a horizontal 1D filter and a vertical 1D filter for frames with zipped pixel format.
		 * The filter result is stored in a target frame with zipped pixel format.
		 * @param source The source frame to be filtered, must be valid
		 * @param target The target frame receiving the filtered results, must be valid
		 * @param width The width of the source (and target) frame in pixel, with range [horizontalFilterSize, infinity)
		 * @param height The height of the source (and target) frame in pixel, with range [verticalFilterSize, infinity)
		 * @param channels The number of channels the source frame (and target frame) has, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param horizontalFilter The elements of the horizontal filter, must be valid
		 * @param horizontalFilterSize The number of elements the horizontal filter has, with range [1, width], must be odd
		 * @param verticalFilter The elements of the vertical filter, must be valid
		 * @param verticalFilterSize The number of elements the vertical filter has, with range [1, height], must be odd
		 * @param reusableMemory An optional object holding reusable memory which can be used during filtering, nullptr otherwise
		 * @param worker Optional worker object to distribute the computation
		 * @tparam T The data type of each pixel channel of the source frame (and target frame) e.g., 'uint8_t', or 'float'
		 * @tparam TFilter The data type of each filter elements e.g., 'unsigned int', or 'float'
		 * @tparam tProcessorInstructions The processor instructions that can be used
		 * @see filterUniversal<T>()
		 */
		template <typename T, typename TFilter, ProcessorInstructions tProcessorInstructions>
		static void filter(const T* source, T* target, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const TFilter* horizontalFilter, const unsigned int horizontalFilterSize, const TFilter* verticalFilter, const unsigned int verticalFilterSize, ReusableMemory* reusableMemory = nullptr, Worker* worker = nullptr);

		/**
		 * Sets a given SIMD value to zero.
		 * @param value The SIMD value to be set
		 * @tparam T The 32 bit data type of the SIMD value
		 * @tparam tProcessorInstructions The set of available instructions, may be any combination of instructions
		 */
		template <typename T, ProcessorInstructions tProcessorInstructions>
		static OCEAN_FORCE_INLINE void setSIMDZero(typename SIMD32x4<T>::Type& value);

		/**
		 * Writes a SIMD with four 32 bit values to (not aligned) memory.
		 * @param value The SIMD value to be written
		 * @param target The buffer receiving the values
		 * @tparam T The 32 bit data type of the SIMD value
		 * @tparam tProcessorInstructions The set of available instructions, may be any combination of instructions
		 */
		template <typename T, ProcessorInstructions tProcessorInstructions>
		static OCEAN_FORCE_INLINE void writeSIMD(const typename SIMD32x4<T>::Type& value, T* target);

		/**
		 * Fills the left border area of an extended row with mirrored pixel information (from the left image region).
		 * @param source The source row providing the image information to be mirrored, must be valid
		 * @param channels The number of channels the source frame has, with range [1, infinity)
		 * @param pixels The number of pixels to be mirrored, should be filterSize / 2u, with range [1, width]
		 * @param extendedRowLeft The pointer to the left border area of the extended row to which the mirrored image content will be copied, must be valid
		 * @tparam T The data type of each pixel channel, e.g., 'uint8_t', or 'float'
		 * @see fillRightExtraBorder().
		 */
		template <typename T>
		static void fillLeftExtraBorder(const T* source, const unsigned int channels, const unsigned int pixels, T* extendedRowLeft);

		/**
		 * Fills the right border area of an extended row with mirrored pixel information (from the right image region).
		 * @param sourceEnd The end of the source row providing the image information to be mirrored (source + width * channels), must be valid
		 * @param channels The number of channels the source frame has, with range [1, infinity)
		 * @param pixels The number of pixels to be mirrored, should be filterSize / 2u, with range [1, width]
		 * @param extendedRowRight The pointer to the right border area of the extended row to which the mirrored image content will be copied, must be valid
		 * @tparam T The data type of each pixel channel, e.g., 'uint8_t', or 'float'
		 * @see fillLeftExtraBorder().
		 */
		template <typename T>
		static void fillRightExtraBorder(const T* sourceEnd, const unsigned int channels, const unsigned int pixels, T* extendedRowRight);

		/**
		 * Determines the filter responses for one filter factor of an asymmetric filter for 4 successive frame elements (4 elements are 4 successive pixels in a Y8 frame or 1 + 1/3 pixels in a RGB24 frame) and adds the individual results to given target elements.
		 * <pre>
		 * This function calculates the following:
		 * target[0] += source[0] * filterFactor
		 * ...
		 * target[3] += source[3] * filterFactor
		 * </pre>
		 * @param source The source elements for which the filter will be applied, the buffer must have at least 8 elements, must be valid
		 * @param filterFactor The filter factor to be used for multiplication
		 * @param target_32x4 The four 32 bit accumulated filter response values to which the multiplication result will be added
		 * @tparam TSource The data type of the source elements e.g., 'uint8_t', or 'float'
		 * @tparam TFilter The data type of the filter elements e.g., 'unsigned int', or 'float'
		 * @tparam tProcessorInstructions The set of available processor instructions needed
		 * @see symmetricFilterHorizontalRowMultiplyOneFilterFactor4Elements().
		 */
		template <typename TSource, typename TFilter, ProcessorInstructions tProcessorInstructions>
		static OCEAN_FORCE_INLINE void asymmetricFilterHorizontalRowMultiplyOneFilterFactor4Elements(const TSource* source, const TFilter& filterFactor, typename SIMD32x4<TFilter>::Type& target_32x4);

		/**
		 * Determines the filter responses for one filter factor of a symmetric filter for 4 successive frame elements (4 elements are 4 successive pixels in a Y8 frame or 2 + 2/3 pixels in a RGB24 frame) and adds the individual results to given target elements.
		 * This function applies a run-time known filter factor.
		 * <pre>
		 * This function calculates the following:
		 * targeta[0] += (sourceLeft[0] + sourceRight[0]) * filterFactor
		 * ...
		 * targetb[3] += (sourceLeft[3] + sourceRight[3]) * filterFactor
		 * </pre>
		 * @param sourceLeft The left source elements for which the filter will be applied, the buffer must have at least 8 elements, must be valid
		 * @param sourceRight The right source elements for which the filter will be applied, the buffer must have at least 8 elements, must be valid
		 * @param filterFactor The filter factor to be used for multiplication
		 * @param target_32x4 The four 32 bit accumulated filter response values to which the multiplication result will be added
		 * @tparam TSource The data type of the source elements e.g., 'uint8_t', or 'float'
		 * @tparam TFilter The data type of the filter elements e.g., 'unsigned int', or 'float'
		 * @tparam tProcessorInstructions The set of available processor instructions needed
		 * @see asymmetricFilterHorizontalRowMultiplyOneFilterFactor4Elements().
		 */
		template <typename TSource, typename TFilter, ProcessorInstructions tProcessorInstructions>
		static OCEAN_FORCE_INLINE void symmetricFilterHorizontalRowMultiplyOneFilterFactor4Elements(const TSource* sourceLeft, const TSource* sourceRight, const TFilter& filterFactor, typename SIMD32x4<TFilter>::Type& target_32x4);

		/**
		 * Determines the filter responses for one filter factor of an asymmetric filter for 8 successive frame elements (8 elements are 8 successive pixels in a Y8 frame or 2 + 2/3 pixels in a RGB24 frame) and adds the individual results to given target elements.
		 * <pre>
		 * This function calculates the following:
		 * targeta[0] += source[0] * filterFactor
		 * ...
		 * targeta[3] += source[3] * filterFactor
		 * targetb[4] += source[4] * filterFactor
		 * ...
		 * targetb[7] += source[7] * filterFactor
		 * </pre>
		 * @param source The source elements for which the filter will be applied, the buffer must have at least 8 elements, must be valid
		 * @param filterFactor The filter factor to be used for multiplication
		 * @param target_32x4a The first four 32 bit accumulated filter response values to which the multiplication result will be added
		 * @param target_32x4b The second four 32 bit accumulated filter response values to which the multiplication result will be added
		 * @tparam TSource The data type of the source elements e.g., 'uint8_t', or 'float'
		 * @tparam TFilter The data type of the filter elements e.g., 'unsigned int', or 'float'
		 * @tparam tProcessorInstructions The set of available processor instructions needed
		 * @see symmetricFilterHorizontalRowMultiplyOneFilterFactor8Elements().
		 */
		template <typename TSource, typename TFilter, ProcessorInstructions tProcessorInstructions>
		static OCEAN_FORCE_INLINE void asymmetricFilterHorizontalRowMultiplyOneFilterFactor8Elements(const TSource* source, const TFilter& filterFactor, typename SIMD32x4<TFilter>::Type& target_32x4a, typename SIMD32x4<TFilter>::Type& target_32x4b);

		/**
		 * Determines the filter responses for one filter factor of a symmetric filter for 8 successive frame elements (8 elements are 8 successive pixels in a Y8 frame or 2 + 2/3 pixels in a RGB24 frame) and adds the individual results to given target elements.
		 * This function applies a run-time known filter factor.
		 * <pre>
		 * This function calculates the following:
		 * targeta[0] += (sourceLeft[0] + sourceRight[0]) * filterFactor
		 * ...
		 * targeta[3] += (sourceLeft[3] + sourceRight[3]) * filterFactor
		 * targetb[4] += (sourceLeft[4] + sourceRight[4]) * filterFactor
		 * ...
		 * targetb[7] += (sourceLeft[7] + sourceRight[7]) * filterFactor
		 * </pre>
		 * @param sourceLeft The left source elements for which the filter will be applied, the buffer must have at least 8 elements, must be valid
		 * @param sourceRight The right source elements for which the filter will be applied, the buffer must have at least 8 elements, must be valid
		 * @param filterFactor The filter factor to be used for multiplication
		 * @param target_32x4a The first four 32 bit accumulated filter response values to which the multiplication result will be added
		 * @param target_32x4b The second four 32 bit accumulated filter response values to which the multiplication result will be added
		 * @tparam TSource The data type of the source elements e.g., 'uint8_t', or 'float'
		 * @tparam TFilter The data type of the filter elements e.g., 'unsigned int', or 'float'
		 * @tparam tProcessorInstructions The set of available processor instructions needed
		 * @see asymmetricFilterHorizontalRowMultiplyOneFilterFactor8Elements().
		 */
		template <typename TSource, typename TFilter, ProcessorInstructions tProcessorInstructions>
		static OCEAN_FORCE_INLINE void symmetricFilterHorizontalRowMultiplyOneFilterFactor8Elements(const TSource* sourceLeft, const TSource* sourceRight, const TFilter& filterFactor, typename SIMD32x4<TFilter>::Type& target_32x4a, typename SIMD32x4<TFilter>::Type& target_32x4b);

		/**
		 * Determines the horizontal filter responses for one block with 4 successive frame elements (4 elements are 4 successive pixels in a Y8 frame or 1 + 1/3 pixels in a RGB24 frame).
		 * @param source The first source element for which the filter will be applied, the buffer must contain at least 4 + 'filterSize' - 1 elements, must be valid
		 * @param target The first target element receiving the filter responses, the buffer must contain at least 4 elements, must be valid
		 * @param channels The number of channels the source (and target) frame has, with range [1, infinity)
		 * @param filter The filter factors of the horizontal filter, with 'filterSize' elements, must be valid
		 * @param filterSize The size of the given filter, with range [1, width], must be odd
		 * @param isSymmetric True, if the provided filter is symmetric; False, otherwise
		 * @tparam TSource The data type of the source elements e.g., 'uint8_t', or 'float'
		 * @tparam TFilter The data type of the filter elements e.g., 'unsigned int', or 'float'
		 * @tparam tProcessorInstructions The set of available processor instructions needed
		 * @see isFilterSymmetric().
		 */
		template <typename TSource, typename TFilter, ProcessorInstructions tProcessorInstructions>
		static OCEAN_FORCE_INLINE void filterHorizontalRowOneBlockWith4Elements(const TSource* const source, TFilter* const target, const unsigned int channels, const TFilter* const filter, const unsigned int filterSize, const bool isSymmetric);

		/**
		 * Determines the horizontal filter responses for one block with 8 successive frame elements (8 elements are 8 successive pixels in a Y8 frame or 2 + 2/3 pixels in a RGB24 frame).
		 * @param source The first source element for which the filter will be applied, the buffer must contain at least 8 + 'filterSize' - 1 elements, must be valid
		 * @param target The first target element receiving the filter responses, the buffer must contain at least 8 elements, must be valid
		 * @param channels The number of channels the source (and target) frame has, with range [1, infinity)
		 * @param filter The filter factors of the horizontal filter, with 'filterSize' elements, must be valid
		 * @param filterSize The size of the given filter, with range [1, width], must be odd
		 * @param isSymmetric True, if the provided filter is symmetric; False, otherwise
		 * @tparam TSource The data type of the source elements e.g., 'uint8_t', or 'float'
		 * @tparam TFilter The data type of the filter elements e.g., 'unsigned int', or 'float'
		 * @tparam tProcessorInstructions The set of available processor instructions needed
		 * @see isFilterSymmetric().
		 */
		template <typename TSource, typename TFilter, ProcessorInstructions tProcessorInstructions>
		static OCEAN_FORCE_INLINE void filterHorizontalRowOneBlockWith8Elements(const TSource* const source, TFilter* const target, const unsigned int channels, const TFilter* const filter, const unsigned int filterSize, const bool isSymmetric);

		/**
		 * Determines the vertical filter responses for the inner core of a frame for one row while processing a block of 4 elements within one iteration (4 elements are 4 successive pixels in a Y8 frame or 1 + 1/3 pixels in a RGB24 frame).
		 * The inner core lies within the frame not covering the frame border of size of filterSize/2.<br>
		 * @param source The first source element that will be used for filtering, must be valid
		 * @param target The first target elements that will receive the filtered results, must be valid
		 * @param sourceStrideElements The number of elements between two successive rows, in elements, with range [width * channels, infinity)
		 * @param filter The filter factors also containing the normalization, must be
		 * @param filterSize The size of the given filter, with range [1, width], must be odd
		 * @param isSymmetric True, if the provided filter is symmetric; False, otherwise
		 * @tparam TSource The data type of the source elements e.g., 'uint8_t', or 'float'
		 * @tparam TTarget The data type of the filter elements e.g., 'unsigned int', or 'float'
		 * @tparam tProcessorInstructions The set of available processor instructions needed
		 */
		template <typename TSource, typename TTarget, ProcessorInstructions tProcessorInstructions>
		static OCEAN_FORCE_INLINE void filterVerticalCoreRow4Elements32BitPerChannelFloat(const TSource* source, TTarget* target, const unsigned int sourceStrideElements, const float* filter, const unsigned int filterSize, const bool isSymmetric);

		/**
		 * Determines the vertical filter responses for the inner core of a frame for one row while processing a block of 8 elements within one iteration (8 elements are 8 successive pixels in a Y8 frame or 2 + 2/3 pixels in a RGB24 frame).
		 * The inner core lies within the frame not covering the frame border of size of filterSize/2.<br>
		 * @param source The first source element that will be used for filtering, must be valid
		 * @param target The first target elements that will receive the filtered results, must be valid
		 * @param sourceStrideElements The number of elements between two successive rows, in elements, with range [width * channels, infinity)
		 * @param filter The filter factors also containing the normalization, must be
		 * @param filterSize The size of the given filter, with range [1, width], must be odd
		 * @param isSymmetric True, if the provided filter is symmetric; False, otherwise
		 * @tparam TSource The data type of the source elements e.g., 'uint8_t', or 'float'
		 * @tparam TTarget The data type of the filter elements e.g., 'unsigned int', or 'float'
		 * @tparam tProcessorInstructions The set of available processor instructions needed
		 */
		template <typename TSource, typename TTarget, ProcessorInstructions tProcessorInstructions>
		static OCEAN_FORCE_INLINE void filterVerticalCoreRow8Elements32BitPerChannelFloat(const TSource* source, TTarget* target, const unsigned int sourceStrideElements, const float* filter, const unsigned int filterSize, const bool isSymmetric);

		/**
		 * Determines the vertical filter responses for the inner core of a frame for one row while processing a block of 16 elements within one iteration (16 elements are 16 successive pixels in a Y8 frame or 5 + 1/3 pixels in a RGB24 frame).
		 * The inner core lies within the frame not covering the (vertical) frame border of size of filterSize/2.<br>
		 * @param source The first source element that will be used for filtering, must be valid
		 * @param target The first target elements that will receive the filtered results, must be valid
		 * @param sourceStrideElements The stride of the frame in elements, stideElements = width * channels + paddingElements, with range [1, infinity)
		 * @param filter The filter factors also containing the normalization, must be
		 * @param filterSize The size of the given filter, with range [1, width], must be odd
		 * @param isSymmetric True, if the provided filter is symmetric; False, otherwise
		 * @tparam TSource The data type of the source elements e.g., 'uint8_t', or 'float'
		 * @tparam TTarget The data type of the filter elements e.g., 'unsigned int', or 'float'
		 * @tparam tProcessorInstructions The set of available processor instructions needed
		 */
		template <typename TSource, typename TTarget, ProcessorInstructions tProcessorInstructions>
		static OCEAN_FORCE_INLINE void filterVerticalCoreRow16Elements32BitPerChannelFloat(const TSource* source, TTarget* target, const unsigned int sourceStrideElements, const float* filter, const unsigned int filterSize, const bool isSymmetric);

		/**
		 * Determines the vertical filter responses for the inner core of a frame for one row.
		 * The inner core lies within the frame not covering the (vertical) frame border of size of filterSize/2.<br>
		 * @param source The first source element that will be used for filtering, must be valid
		 * @param target The first target elements that will receive the filtered results, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param channels The number of channels the source (and target) frame has, with range [1, infinity)
		 * @param filter The filter factors also containing the normalization, must be
		 * @param filterSize The size of the given filter, with range [1, width], must be odd
		 * @param isSymmetric True, if the provided filter is symmetric; False, otherwise
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @tparam TSource The data type of the source elements e.g., 'uint8_t', or 'float'
		 * @tparam TTarget The data type of the filter elements e.g., 'unsigned int', or 'float'
		 * @tparam tProcessorInstructions The set of available processor instructions needed
		 */
		template <typename TSource, typename TTarget, ProcessorInstructions tProcessorInstructions>
		static OCEAN_FORCE_INLINE void filterVerticalCoreRow32BitPerChannelFloat(const TSource* source, TTarget* target, const unsigned int width, const unsigned int channels, const float* filter, const unsigned int filterSize, const bool isSymmetric, const unsigned int sourcePaddingElements);

		/**
		 * Determines the vertical filter responses near the (vertical) border of a frame for one row while processing a block of 8 elements within one iteration (8 elements are 8 successive pixels in a Y8 frame or 2 + 2/3 pixels in a RGB24 frame).
		 * The border covers the upper and lower filterSize/2 rows of a frame as this area needs a special handling of filter locations lying outside the frame.
		 * @param source The first source element that will be used for filtering, must be valid
		 * @param target The first target elements that will receive the filtered results, must be valid
		 * @param sourceStrideElements The number of elements between two successive rows, in elements, with range [width * channels, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param row The row to be handled, with range [0, height - 1]
		 * @param filter The filter factors, must be 'filterSize' individual values
		 * @param filterSize The number of filter factors, with range [1, width - 1], must be odd
		 * @param isSymmetric True, if the provided filter is symmetric; False, otherwise
		 * @tparam TSource The data type of the source elements e.g., 'uint8_t', or 'float'
		 * @tparam TTarget The data type of the filter elements e.g., 'unsigned int', or 'float'
		 * @tparam tProcessorInstructions The set of available processor instructions needed
		 */
		template <typename TSource, typename TTarget, ProcessorInstructions tProcessorInstructions>
		static OCEAN_FORCE_INLINE void filterVerticalBorderRow8Elements32BitPerChannelFloat(const TSource* source, TTarget* target, const unsigned int sourceStrideElements, const unsigned int height, const unsigned int row, const float* filter, const unsigned int filterSize, const bool isSymmetric);

		/**
		 * Determines the vertical filter responses near the (vertical) border of a frame for one row while processing a block of 16 elements within one iteration (16 elements are 16 successive pixels in a Y8 frame or 5 + 1/3 pixels in a RGB24 frame).
		 * The border covers the upper and lower filterSize/2 rows of a frame as this area needs a special handling of filter locations lying outside the frame.
		 * @param source The first source element that will be used for filtering, must be valid
		 * @param target The first target elements that will receive the filtered results, must be valid
		 * @param sourceStrideElements The number of elements between two successive rows, in elements, with range [width * channels, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param row The row to be handled, with range [0, height - 1]
		 * @param filter The filter factors, must be 'filterSize' individual values
		 * @param filterSize The number of filter factors, with range [1, width - 1], must be odd
		 * @param isSymmetric True, if the provided filter is symmetric; False, otherwise
		 * @tparam TSource The data type of the source elements e.g., 'uint8_t', or 'float'
		 * @tparam TTarget The data type of the filter elements e.g., 'unsigned int', or 'float'
		 * @tparam tProcessorInstructions The set of available processor instructions needed
		 */
		template <typename TSource, typename TTarget, ProcessorInstructions tProcessorInstructions>
		static OCEAN_FORCE_INLINE void filterVerticalBorderRow16Elements32BitPerChannelFloat(const TSource* source, TTarget* target, const unsigned int sourceStrideElements, const unsigned int height, const unsigned int row, const float* filter, const unsigned int filterSize, const bool isSymmetric);

		/**
		 * Determines the vertical filter responses near the (vertical) border of a frame for one row.
		 * The border covers the upper and lower filterSize/2 rows of a frame as this area needs a special handling of filter locations lying outside the frame.
		 * @param source The first source element that will be used for filtering, must be valid
		 * @param target The first target elements that will receive the filtered results, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param channels The number of data channels both frames have, with range [1, infinity)
		 * @param row The row to be handled, with range [0, height - 1]
		 * @param filter The filter factors, must be 'filterSize' individual values
		 * @param filterSize The number of filter factors, with range [1, width - 1], must be odd
		 * @param isSymmetric True, if the provided filter is symmetric; False, otherwise
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @tparam TSource The data type of the source elements e.g., 'uint8_t', or 'float'
		 * @tparam TTarget The data type of the filter elements e.g., 'unsigned int', or 'float'
		 * @tparam tProcessorInstructions The set of available processor instructions needed
		 */
		template <typename TSource, typename TTarget, ProcessorInstructions tProcessorInstructions>
		static OCEAN_FORCE_INLINE void filterVerticalBorderRow32BitPerChannelFloat(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int row, const float* filter, const unsigned int filterSize, const bool isSymmetric, const unsigned int sourcePaddingElements);

		/**
		 * Applies the horizontal filtering in a subset of a frame with a specified 1D filter kernel for frames with zipped pixel format.
		 * The filter result is stored in a target frame with zipped pixel format and 32 bit per channel.
		 * @param source The source frame to be filtered, must be valid
		 * @param target The target frame receiving the filtered results, must be valid
		 * @param width The width of the source (and target) frame in pixel, with range [filterSize + 1, infinity)
		 * @param height The height of the source (and target) frame in pixel, with range [filterSize, infinity)
		 * @param channels The number of data channels both frames have, with range [1, infinity)
		 * @param filter The filter factors, must be 'filterSize' individual values
		 * @param filterSize The number of filter factors, with range [1, width - 1], must be odd
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstRow The first row to be handled, with range [0, height)
		 * @param numberRows The number of rows to be handled, with range [1, height - firstRow]
		 * @tparam TSource The data type of the source elements e.g., 'uint8_t', or 'float'
		 * @tparam TFilter The data type of the filter elements e.g., 'unsigned int', or 'float'
		 * @tparam tProcessorInstructions The set of available processor instructions needed
		 */
		template <typename TSource, typename TFilter, ProcessorInstructions tProcessorInstructions>
		static void filterHorizontalSubset(const TSource* source, TFilter* target, const unsigned int width, const unsigned int height, const unsigned int channels, const TFilter* filter, const unsigned int filterSize, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Applies the vertical filtering for a subset of the frame with a specified 1D filter kernel for frames with zipped pixel format and 32 bit per channel.
		 * The filter result is stored in a target frame with zipped pixel format and 8 bit per channel.<br>
		 * This function uses floating point filter factors ensuring the final result is normalized.
		 * @param source The source frame to be filtered, must be valid
		 * @param target The target frame receiving the filtered results, must be valid
		 * @param width The width of the source (and target) frame in pixel, with range [max(filterSize + 1, 16 / channels), infinity)
		 * @param height The height of the source (and target) frame in pixel, with range [filterSize, infinity)
		 * @param channels The number of data channels both frames have, with range [1, infinity)
		 * @param filter The filter factors, must be 'filterSize' individual values
		 * @param filterSize The number of filter factors, with range [1, width - 1], must be odd
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstRow The first row to be handled, with range [0, height]
		 * @param numberRows The number of rows to be handled, with range [1, height - firstRow]
		 * @tparam TSource The data type of the source elements e.g., 'uint8_t', or 'float'
		 * @tparam TTarget The data type of the filter elements e.g., 'unsigned int', or 'float'
		 * @tparam tProcessorInstructions The set of available processor instructions needed
		 */
		template <typename TSource, typename TTarget, ProcessorInstructions tProcessorInstructions>
		static void filterVerticalSubset(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int channels, const float* filter, const unsigned int filterSize, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Applies an horizontal filter to a subset of an image with almost arbitrary data type.
		 * @param source The source frame to which the filter will be applied, must be valid
		 * @param target The target frame receiving the filter response, must be valid
		 * @param width The width of the source frame (and target frame) in pixel, with range [1, infinity)
		 * @param channels The number of channels the source and target frame have, with range [1, infinity)
		 * @param horizontalFilter The (separable) horizontal filter to be applied, must be valid
		 * @param filterSize The number of filter elements, must be odd, with range [1, width]
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstRow The first row to be handled, with range [0, height)
		 * @param numberRows The number of rows to be handled, with range [1, height - firstRow]
		 * @tparam T The data type of each pixel channel of the source and target frame, e.g., 'uint8_t', 'int', 'float', ...
		 * @tparam TIntermediate The data type of the intermediate target frame, should be either 'float' or 'double'
		 */
		template <typename T, typename TIntermediate>
		static void filterUniversalHorizontalSubset(const T* source, TIntermediate* target, const unsigned int width, const unsigned int channels, const float* horizontalFilter, const unsigned int filterSize, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Applies an vertical filter to a subset of an image with almost arbitrary data type.
		 * @param source The source frame to which the filter will be applied, must be valid
		 * @param target The target frame receiving the filter response, must be valid
		 * @param width The width of the source frame (and target frame) in pixel, with range [1, infinity)
		 * @param height The height of the source frame (and target frame) in pixel, with range [1, infinity)
		 * @param channels The number of channels the source and target frame have, with range [1, infinity)
		 * @param verticalFilter The (separable) vertical filter to be applied, must be valid
		 * @param filterSize The number of filter elements, must be odd, with range [1, width]
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstRow The first row to be handled, with range [0, height)
		 * @param numberRows The number of rows to be handled, with range [1, height - firstRow]
		 * @tparam T The data type of each pixel channel of the source and target frame, e.g., 'uint8_t', 'int', 'float', ...
		 * @tparam TIntermediate The data type of the intermediate target frame, should be either 'float' or 'double'
		 */
		template <typename T, typename TIntermediate>
		static void filterUniversalVerticalSubset(const TIntermediate* source, T* target, const unsigned int width, const unsigned int height, const unsigned int channels, const float* verticalFilter, const unsigned int filterSize, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Mirrors a given value at the left border if necessary.
		 * The function provides a result as below:<br>
		 * <pre>
		 * Original: -3 -2 -1 |  0  1  2  3  4  5  6
		 *   Result:  2  1  0 |  0  1  2  3  4  5  6
		 * </pre>
		 * @param value The value to be mirrored, with range (-infinity, infinity)
		 * @return Mirrored value
		 * @ingroup base
		 */
		static inline unsigned int mirroredBorderLocationLeft(const int value);

		/**
		 * Mirrors a given value at the right border if necessary.
		 * The values is mirrored according to a given size parameter.<br>
		 * The function provides a result as below:<br>
		 * <pre>
		 * Original: 4  5  6 ... s-2  s-1 |   s  s+1  s+2
		 *   Result: 4  5  6 ... s-2  s-1 | s-1  s-2  s-3
		 * </pre>
		 * @param value The value to be mirrored, with range [0, 2*size)
		 * @param size Specified size defining the upper mirror border, with range [1, 2147483647]
		 * @return Mirrored value
		 * @ingroup base
		 */
		static inline unsigned int mirroredBorderLocationRight(const unsigned int value, const unsigned int size);
};

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 10

/**
 * Definition of a 128 bit SIMD data type holding four 32 bit values.
 */
template <>
struct FrameFilterSeparable::SIMD32x4<unsigned int>
{
	typedef __m128i Type;
};

/**
 * Definition of a 128 bit SIMD data type holding four 32 bit values.
 */
template <>
struct FrameFilterSeparable::SIMD32x4<float>
{
	typedef __m128 Type;
};

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

/**
 * Definition of a 128 bit SIMD data type holding four 32 bit values.
 */
template <>
struct FrameFilterSeparable::SIMD32x4<unsigned int>
{
	typedef uint32x4_t Type;
};

/**
 * Definition of a 128 bit SIMD data type holding four 32 bit values.
 */
template <>
struct FrameFilterSeparable::SIMD32x4<float>
{
	typedef float32x4_t Type;
};

#endif

template <typename T>
bool FrameFilterSeparable::isFilterSymmetric(const T* filterValues, const size_t size)
{
	ocean_assert(filterValues != nullptr);
	ocean_assert(size >= 1 && size % 2 == 1);

	for (size_t n = 0; n < size / 2; ++n)
	{
		if (NumericT<T>::isNotEqual(filterValues[n], filterValues[size - n - 1]))
		{
			return false;
		}
	}

	return true;
}

template <typename T>
T FrameFilterSeparable::sumFilterValues(const T* filterValues, const size_t size)
{
	ocean_assert(filterValues != nullptr);
	ocean_assert(size >= 1);

	T sum = filterValues[0];

	for (size_t n = 1; n < size; ++n)
	{
		sum += filterValues[n];
	}

	return sum;
}

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 20

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::setSIMDZero<unsigned int, PI_SSE_2>(typename SIMD32x4<unsigned int>::Type& value)
{
	// SSE2: _mm_setzero_si128

	value = _mm_setzero_si128();
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::setSIMDZero<float, PI_SSE_2>(typename SIMD32x4<float>::Type& value)
{
	// SSE: _mm_set_ps1

	value = _mm_set_ps1(0.0f);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::writeSIMD<unsigned int, PI_SSE_2>(const SIMD32x4<unsigned int>::Type& value, unsigned int* target)
{
	_mm_storeu_si128((__m128i*)target, value);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::writeSIMD<float, PI_SSE_2>(const SIMD32x4<float>::Type& value, float* target)
{
	_mm_storeu_si128((__m128i*)target, _mm_castps_si128(value));
}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 20

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::setSIMDZero<unsigned int, PI_NEON>(typename SIMD32x4<unsigned int>::Type& value)
{
	value = vdupq_n_u32(0u);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::setSIMDZero<float, PI_NEON>(typename SIMD32x4<float>::Type& value)
{
	value = vdupq_n_f32(0.0f);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::writeSIMD<unsigned int, PI_NEON>(const SIMD32x4<unsigned int>::Type& value, unsigned int* target)
{
	vst1q_u32(target, value);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::writeSIMD<float, PI_NEON>(const SIMD32x4<float>::Type& value, float* target)
{
	vst1q_f32(target, value);
}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

template <typename T>
void FrameFilterSeparable::fillLeftExtraBorder(const T* source, const unsigned int channels, const unsigned int pixels, T* extendedRow)
{
	ocean_assert(source != nullptr && extendedRow != nullptr);

	for (unsigned int n = 0u; n < pixels; ++n)
	{
		memcpy(extendedRow + n * channels, source + (pixels - n - 1u) * channels, sizeof(T) * channels);
	}
}

template <typename T>
void FrameFilterSeparable::fillRightExtraBorder(const T* sourceEnd, const unsigned int channels, const unsigned int pixels, T* extendedRow)
{
	ocean_assert(sourceEnd != nullptr && extendedRow != nullptr);

	for (unsigned int n = 0u; n < pixels; ++n)
	{
		memcpy(extendedRow + n * channels, sourceEnd - (n + 1u) * int(channels), sizeof(T) * channels);
	}
}

template <typename TSource, typename TTarget, ProcessorInstructions tProcessorInstructions>
OCEAN_FORCE_INLINE void FrameFilterSeparable::filterVerticalCoreRow32BitPerChannelFloat(const TSource* source, TTarget* target, const unsigned int width, const unsigned int channels, const float* filter, const unsigned int filterSize, const bool isSymmetric, const unsigned int sourcePaddingElements)
{
	ocean_assert(source != nullptr && target != nullptr && filter != nullptr);
	ocean_assert(channels >= 1u);
	ocean_assert(filterSize % 2u == 1u);

	const unsigned int sourceStrideElements = width * channels + sourcePaddingElements;

	unsigned int remainingElements = width * channels;

	while (remainingElements >= 16u)
	{
		filterVerticalCoreRow16Elements32BitPerChannelFloat<TSource, TTarget, tProcessorInstructions>(source, target, sourceStrideElements, filter, filterSize, isSymmetric);

		source += 16;
		target += 16;

		remainingElements -= 16u;
	}

	while (remainingElements >= 8u)
	{
		filterVerticalCoreRow8Elements32BitPerChannelFloat<TSource, TTarget, tProcessorInstructions>(source, target, sourceStrideElements, filter, filterSize, isSymmetric);

		source += 8;
		target += 8;

		remainingElements -= 8u;
	}

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 20

	while (remainingElements >= 4u)
	{
		filterVerticalCoreRow4Elements32BitPerChannelFloat<TSource, TTarget, tProcessorInstructions>(source, target, sourceStrideElements, filter, filterSize, isSymmetric);

		source += 4;
		target += 4;

		remainingElements -= 4u;
	}

	ocean_assert(width * channels >= 4u);
	ocean_assert(remainingElements < 4u);

	if (remainingElements != 0u)
	{
		const unsigned int shift = 4u - remainingElements;

		filterVerticalCoreRow4Elements32BitPerChannelFloat<TSource, TTarget, tProcessorInstructions>(source - shift, target - shift, sourceStrideElements, filter, filterSize, isSymmetric);
	}

#else

	ocean_assert(width * channels >= 8u);
	ocean_assert(remainingElements < 8u);

	if (remainingElements != 0u)
	{
		const unsigned int shift = 8u - remainingElements;

		filterVerticalCoreRow8Elements32BitPerChannelFloat<TSource, TTarget, tProcessorInstructions>(source - shift, target - shift, sourceStrideElements, filter, filterSize, isSymmetric);
	}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 20
}

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 20

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::filterVerticalCoreRow4Elements32BitPerChannelFloat<unsigned int, uint8_t, PI_SSE_2>(const unsigned int* source, uint8_t* target, const unsigned int sourceStrideElements, const float* filter, const unsigned int filterSize, const bool isSymmetric)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(filterSize >= 1u && (filterSize % 2u) == 1u);

	/**
	 * This function uses the following SSE instructions, and needs SSE2 or higher
	 *
	 * SSE1:
	 * _mm_set_ps1
	 * _mm_mul_ps
	 * _mm_add_ps
	 * _mm_loadu_ps
	 *
	 * SSE2:
	 * _mm_loadu_si128
	 * _mm_cvtepi32_ps
	 * _mm_add_epi32
	 * _mm_cvtps_epi32
	 * _mm_packs_epi32
	 * _mm_packus_epi16
	 */

	const unsigned int filterSize_2 = filterSize / 2u;

	const __m128i* sourceBlock = (const __m128i*)source;

	// we store one filter value in each of the four 32 bit integer values
	__m128 filterFactor_32x4 = _mm_set_ps1(filter[filterSize_2]);

	// now we load four input values, and multiply each of them with the center kernel value
	__m128 source128 = _mm_cvtepi32_ps(_mm_loadu_si128(sourceBlock));
	__m128 result128 = _mm_mul_ps(source128, filterFactor_32x4);

	// now we proceed with the remaining filter values
	for (unsigned int i = 1u; i <= filterSize_2; ++i)
	{
		const __m128i* sourceMinus = (const __m128i*)(source - sourceStrideElements * i);
		const __m128i* sourcePlus = (const __m128i*)(source + sourceStrideElements * i);

		if (isSymmetric)
		{
			// we have a symmetric filter, so let's do some optimizations
			filterFactor_32x4 = _mm_set_ps1(filter[filterSize_2 + i]);

			// we sum the values of the upper and the lower row (as both will be multiplied with the same filter value)

			__m128i source128i = _mm_add_epi32(_mm_loadu_si128(sourceMinus), _mm_loadu_si128(sourcePlus));

			result128 = _mm_add_ps(result128, _mm_mul_ps(_mm_cvtepi32_ps(source128i), filterFactor_32x4));
		}
		else
		{
			// we don't have a symmetric filter, so we need to handle two individual filters
			__m128 filterFactor128Minus = _mm_set_ps1(filter[filterSize_2 - i]);
			__m128 filterFactor128Plus = _mm_set_ps1(filter[filterSize_2 + i]);

			__m128i source128iMinus = _mm_loadu_si128(sourceMinus);
			__m128i source128iPlus = _mm_loadu_si128(sourcePlus);

			result128 = _mm_add_ps(result128, _mm_mul_ps(_mm_cvtepi32_ps(source128iMinus), filterFactor128Minus));
			result128 = _mm_add_ps(result128, _mm_mul_ps(_mm_cvtepi32_ps(source128iPlus), filterFactor128Plus));
		}
	}

	// now we have 8 bit values in each 32 bit register

	__m128i source128i = _mm_cvtps_epi32(result128);
	source128i = _mm_packs_epi32(source128i, source128i);
	source128i = _mm_packus_epi16(source128i, source128i);

	*((unsigned int*)target) = SSE::value_u32<0u>(source128i);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::filterVerticalCoreRow4Elements32BitPerChannelFloat<float, float, PI_SSE_2>(const float* source, float* target, const unsigned int sourceStrideElements, const float* filter, const unsigned int filterSize, const bool isSymmetric)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(filterSize >= 1u && (filterSize % 2u) == 1u);

	/**
	 * This function uses the following SSE instructions, and needs SSE2 or higher
	 *
	 * SSE:
	 * _mm_set_ps1
	 * _mm_mul_ps
	 * _mm_add_ps
	 *
	 * SSE2:
	 * _mm_loadu_si128
	 * _mm_castsi128_ps
	 */

	const unsigned int filterSize_2 = filterSize / 2u;

	const __m128i* sourceBlock = (const __m128i*)source;

	// we store one filter value in each of the four 32 bit values
	__m128 filterFactor_32x4 = _mm_set_ps1(filter[filterSize_2]);

	// now we load four input values, and multiply each of them with the center kernel value
	__m128 source128 = _mm_castsi128_ps(_mm_loadu_si128(sourceBlock + 0));
	__m128 result128 = _mm_mul_ps(source128, filterFactor_32x4);

	// now we proceed with the remaining filter values
	for (unsigned int i = 1u; i <= filterSize_2; ++i)
	{
		const __m128i* sourceMinus = (const __m128i*)(source - sourceStrideElements * i);
		const __m128i* sourcePlus = (const __m128i*)(source + sourceStrideElements * i);

		if (isSymmetric)
		{
			// we have a symmetric filter, so let's do some optimizations
			filterFactor_32x4 = _mm_set_ps1(filter[filterSize_2 + i]);

			// we sum the values of the upper and the lower row (as both will be multiplied with the same filter value)

			source128 = _mm_add_ps(_mm_castsi128_ps(_mm_loadu_si128(sourceMinus)), _mm_castsi128_ps(_mm_loadu_si128(sourcePlus)));

			result128 = _mm_add_ps(result128, _mm_mul_ps(source128, filterFactor_32x4));
		}
		else
		{
			// we don't have a symmetric filter, so we need to handle two individual filters
			__m128 filterFactor_32x4Minus = _mm_set_ps1(filter[filterSize_2 - i]);
			__m128 filterFactor_32x4Plus = _mm_set_ps1(filter[filterSize_2 + i]);

			// we sum the values of the upper and the lower row (as both will be multiplied with the same filter value)
			__m128 source128Minus = _mm_castsi128_ps(_mm_loadu_si128(sourceMinus));
			__m128 source128Plus = _mm_castsi128_ps(_mm_loadu_si128(sourcePlus));

			result128 = _mm_add_ps(result128, _mm_mul_ps(source128Minus, filterFactor_32x4Minus));
			result128 = _mm_add_ps(result128, _mm_mul_ps(source128Plus, filterFactor_32x4Plus));
		}
	}

	writeSIMD<float, PI_SSE_2>(result128, target);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::filterVerticalCoreRow8Elements32BitPerChannelFloat<unsigned int, uint8_t, PI_SSE_2>(const unsigned int* source, uint8_t* target, const unsigned int sourceStrideElements, const float* filter, const unsigned int filterSize, const bool isSymmetric)
{
	ocean_assert(source != nullptr && target != nullptr && filter != nullptr);
	ocean_assert(filterSize >= 1u && (filterSize % 2u) == 1u);

	/**
	 * This function uses the following SSE instructions, and needs SSE2 or higher
	 *
	 * SSE1:
	 * _mm_set_ps1
	 * _mm_mul_ps
	 * _mm_add_ps
	 * _mm_loadu_ps
	 *
	 * SSE2:
	 * _mm_loadu_si128
	 * _mm_cvtepi32_ps
	 * _mm_add_epi32
	 * _mm_cvtps_epi32
	 * _mm_packs_epi32
	 * _mm_packus_epi16
	 * _mm_storel_epi64
	 */

	const unsigned int filterSize_2 = filterSize / 2u;

	const __m128i* sourceBlock = (const __m128i*)source;

	// we store one filter value in each of the four 32 bit integer values
	__m128 filterFactor_32x4 = _mm_set_ps1(filter[filterSize_2]);

	// now we load four input values, and multiply each of them with the center kernel value
	__m128 source_32x4a = _mm_cvtepi32_ps(_mm_loadu_si128(sourceBlock + 0));
	__m128 result_32x4a = _mm_mul_ps(source_32x4a, filterFactor_32x4);

	// now we load the next four input values, ...
	__m128 source_32x4b = _mm_cvtepi32_ps(_mm_loadu_si128(sourceBlock + 1));
	__m128 result_32x4b = _mm_mul_ps(source_32x4b, filterFactor_32x4);

	// now we proceed with the remaining filter values
	for (unsigned int i = 1u; i <= filterSize_2; ++i)
	{
		const __m128i* sourceMinus = (const __m128i*)(source - sourceStrideElements * i);
		const __m128i* sourcePlus = (const __m128i*)(source + sourceStrideElements * i);

		if (isSymmetric)
		{
			// we have a symmetric filter, so let's do some optimizations
			filterFactor_32x4 = _mm_set_ps1(filter[filterSize_2 + i]);

			// we sum the values of the upper and the lower row (as both will be multiplied with the same filter value)
			__m128i source128ai = _mm_add_epi32(_mm_loadu_si128(sourceMinus + 0), _mm_loadu_si128(sourcePlus + 0));
			__m128i source128bi = _mm_add_epi32(_mm_loadu_si128(sourceMinus + 1), _mm_loadu_si128(sourcePlus + 1));

			result_32x4a = _mm_add_ps(result_32x4a, _mm_mul_ps(_mm_cvtepi32_ps(source128ai), filterFactor_32x4));
			result_32x4b = _mm_add_ps(result_32x4b, _mm_mul_ps(_mm_cvtepi32_ps(source128bi), filterFactor_32x4));
		}
		else
		{
			// we don't have a symmetric filter, so we need to handle two individual filters
			__m128 filterFactor128Minus = _mm_set_ps1(filter[filterSize_2 - i]);
			__m128 filterFactor128Plus = _mm_set_ps1(filter[filterSize_2 + i]);

			// we sum the values of the upper and the lower row (as both will be multiplied with the same filter value)
			__m128i source128aiMinus =_mm_loadu_si128(sourceMinus + 0);
			__m128i source128aiPlus = _mm_loadu_si128(sourcePlus + 0);
			__m128i source128biMinus = _mm_loadu_si128(sourceMinus + 1);
			__m128i source128biPlus = _mm_loadu_si128(sourcePlus + 1);

			result_32x4a = _mm_add_ps(result_32x4a, _mm_mul_ps(_mm_cvtepi32_ps(source128aiMinus), filterFactor128Minus));
			result_32x4b = _mm_add_ps(result_32x4b, _mm_mul_ps(_mm_cvtepi32_ps(source128biMinus), filterFactor128Minus));

			result_32x4a = _mm_add_ps(result_32x4a, _mm_mul_ps(_mm_cvtepi32_ps(source128aiPlus), filterFactor128Plus));
			result_32x4b = _mm_add_ps(result_32x4b, _mm_mul_ps(_mm_cvtepi32_ps(source128biPlus), filterFactor128Plus));
		}
	}

	// now we have 8 bit values in each 32 bit register
	__m128i result128 = _mm_packs_epi32(_mm_cvtps_epi32(result_32x4a), _mm_cvtps_epi32(result_32x4b));
	result128 = _mm_packus_epi16(result128, result128);

	_mm_storel_epi64((__m128i*)target, result128);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::filterVerticalCoreRow8Elements32BitPerChannelFloat<float, float, PI_SSE_2>(const float* source, float* target, const unsigned int sourceStrideElements, const float* filter, const unsigned int filterSize, const bool isSymmetric)
{
	ocean_assert(source != nullptr && target != nullptr && filter != nullptr);
	ocean_assert(filterSize >= 1u && (filterSize % 2u) == 1u);

	/**
	 * This function uses the following SSE instructions, and needs SSE2 or higher
	 *
	 * SSE:
	 * _mm_set_ps1
	 * _mm_mul_ps
	 * _mm_add_ps
	 *
	 * SSE2:
	 * _mm_loadu_si128
	 * _mm_castsi128_ps
	 */

	const unsigned int filterSize_2 = filterSize / 2u;

	const __m128i* sourceBlock = (const __m128i*)source;

	// we store one filter value in each of the four 32 bit values
	__m128 filterFactor_32x4 = _mm_set_ps1(filter[filterSize_2]);

	// now we load four input values, and multiply each of them with the center kernel value
	__m128 source_32x4a = _mm_castsi128_ps(_mm_loadu_si128(sourceBlock + 0));
	__m128 result_32x4a = _mm_mul_ps(source_32x4a, filterFactor_32x4);

	// now we load the next four input values, ...
	__m128 source_32x4b = _mm_castsi128_ps(_mm_loadu_si128(sourceBlock + 1));
	__m128 result_32x4b = _mm_mul_ps(source_32x4b, filterFactor_32x4);

	// now we proceed with the remaining filter values
	for (unsigned int i = 1u; i <= filterSize_2; ++i)
	{
		const __m128i* sourceMinus = (const __m128i*)(source - sourceStrideElements * i);
		const __m128i* sourcePlus = (const __m128i*)(source + sourceStrideElements * i);

		if (isSymmetric)
		{
			// we have a symmetric filter, so let's do some optimizations
			filterFactor_32x4 = _mm_set_ps1(filter[filterSize_2 + i]);

			// we sum the values of the upper and the lower row (as both will be multiplied with the same filter value)

			source_32x4a = _mm_add_ps(_mm_castsi128_ps(_mm_loadu_si128(sourceMinus + 0)), _mm_castsi128_ps(_mm_loadu_si128(sourcePlus + 0)));
			source_32x4b = _mm_add_ps(_mm_castsi128_ps(_mm_loadu_si128(sourceMinus + 1)), _mm_castsi128_ps(_mm_loadu_si128(sourcePlus + 1)));

			result_32x4a = _mm_add_ps(result_32x4a, _mm_mul_ps(source_32x4a, filterFactor_32x4));
			result_32x4b = _mm_add_ps(result_32x4b, _mm_mul_ps(source_32x4b, filterFactor_32x4));
		}
		else
		{
			// we don't have a symmetric filter, so we need to handle two individual filters
			__m128 filterFactor_32x4Minus = _mm_set_ps1(filter[filterSize_2 - i]);
			__m128 filterFactor_32x4Plus = _mm_set_ps1(filter[filterSize_2 + i]);

			// we sum the values of the upper and the lower row (as both will be multiplied with the same filter value)
			__m128 source128aMinus = _mm_castsi128_ps(_mm_loadu_si128(sourceMinus + 0));
			__m128 source128aPlus = _mm_castsi128_ps(_mm_loadu_si128(sourcePlus + 0));
			__m128 source128bMinus = _mm_castsi128_ps(_mm_loadu_si128(sourceMinus + 1));
			__m128 source128bPlus = _mm_castsi128_ps(_mm_loadu_si128(sourcePlus + 1));

			result_32x4a = _mm_add_ps(result_32x4a, _mm_mul_ps(source128aMinus, filterFactor_32x4Minus));
			result_32x4a = _mm_add_ps(result_32x4a, _mm_mul_ps(source128aPlus, filterFactor_32x4Plus));

			result_32x4b = _mm_add_ps(result_32x4b, _mm_mul_ps(source128bMinus, filterFactor_32x4Minus));
			result_32x4b = _mm_add_ps(result_32x4b, _mm_mul_ps(source128bPlus, filterFactor_32x4Plus));
		}
	}

	writeSIMD<float, PI_SSE_2>(result_32x4a, target +  0);
	writeSIMD<float, PI_SSE_2>(result_32x4b, target +  4);
}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 20

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::filterVerticalCoreRow8Elements32BitPerChannelFloat<unsigned int, uint8_t, PI_NEON>(const unsigned int* source, uint8_t* target, const unsigned int sourceStrideElements, const float* filter, const unsigned int filterSize, const bool isSymmetric)
{
	const unsigned int filterSize_2 = filterSize / 2u;

	// we store one filter value in each of the four 32 bit integer values
	float32x4_t filterFactor_32x4 = vdupq_n_f32(filter[filterSize_2]);

	// now we load four input values, and multiply each of them with the center kernel value
	float32x4_t source_32x4a = vcvtq_f32_u32(vld1q_u32(source + 4 * 0));
	float32x4_t result_32x4a = vmulq_f32(source_32x4a, filterFactor_32x4);

	float32x4_t source_32x4b = vcvtq_f32_u32(vld1q_u32(source + 4 * 1));
	float32x4_t result_32x4b = vmulq_f32(source_32x4b, filterFactor_32x4);

	// now we proceed with the remaining filter values
	for (unsigned int i = 1u; i <= filterSize_2; ++i)
	{
		const unsigned int* sourceMinus = source - sourceStrideElements * i;
		const unsigned int* sourcePlus = source + sourceStrideElements * i;

		if (isSymmetric)
		{
			// we have a symmetric filter, so let's do some optimizations
			filterFactor_32x4 = vdupq_n_f32(filter[filterSize_2 + i]);

			// we sum the values of the upper and the lower row (as both will be multiplied with the same filter value)
			uint32x4_t source128ai = vaddq_u32(vld1q_u32(sourceMinus + 4 * 0), vld1q_u32(sourcePlus + 4 * 0));
			uint32x4_t source128bi = vaddq_u32(vld1q_u32(sourceMinus + 4 * 1), vld1q_u32(sourcePlus + 4 * 1));

			result_32x4a = vmlaq_f32(result_32x4a, vcvtq_f32_u32(source128ai), filterFactor_32x4);
			result_32x4b = vmlaq_f32(result_32x4b, vcvtq_f32_u32(source128bi), filterFactor_32x4);
		}
		else
		{
			// we don't have a symmetric filter, so we need to handle two individual filters

			float32x4_t filterFactor128Minus = vdupq_n_f32(filter[filterSize_2 - i]);
			float32x4_t filterFactor128Plus = vdupq_n_f32(filter[filterSize_2 + i]);

			uint32x4_t source128aiMinus = vld1q_u32(sourceMinus + 4 * 0);
			result_32x4a = vmlaq_f32(result_32x4a, vcvtq_f32_u32(source128aiMinus), filterFactor128Minus);

			uint32x4_t source128biMinus = vld1q_u32(sourceMinus + 4 * 1);
			result_32x4b = vmlaq_f32(result_32x4b, vcvtq_f32_u32(source128biMinus), filterFactor128Minus);

			uint32x4_t source128aiPlus = vld1q_u32(sourcePlus + 4 * 0);
			result_32x4a = vmlaq_f32(result_32x4a, vcvtq_f32_u32(source128aiPlus), filterFactor128Plus);

			uint32x4_t source128biPlus = vld1q_u32(sourcePlus + 4 * 1);
			result_32x4b = vmlaq_f32(result_32x4b, vcvtq_f32_u32(source128biPlus), filterFactor128Plus);
		}
	}

	// now we have 8 bit values in each 32 bit register
	uint16x8_t result128ab = vcombine_u16(vqmovn_u32(vcvtq_u32_f32(result_32x4a)), vqmovn_u32(vcvtq_u32_f32(result_32x4b)));

	uint8x8_t result64 = vqmovn_u16(result128ab);

	vst1_u8(target, result64);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::filterVerticalCoreRow8Elements32BitPerChannelFloat<float, float, PI_NEON>(const float* source, float* target, const unsigned int sourceStrideElements, const float* filter, const unsigned int filterSize, const bool isSymmetric)
{
	const unsigned int filterSize_2 = filterSize / 2u;

	// we store one filter value in each of the four 32 bit integer values
	float32x4_t filterFactor_32x4 = vdupq_n_f32(filter[filterSize_2]);

	// now we load four input values, and multiply each of them with the center kernel value
	float32x4_t source_32x4a = vld1q_f32(source + 4 * 0);
	float32x4_t result_32x4a = vmulq_f32(source_32x4a, filterFactor_32x4);

	float32x4_t source_32x4b = vld1q_f32(source + 4 * 1);
	float32x4_t result_32x4b = vmulq_f32(source_32x4b, filterFactor_32x4);

	// now we proceed with the remaining filter values
	for (unsigned int i = 1u; i <= filterSize_2; ++i)
	{
		const float* sourceMinus = source - sourceStrideElements * i;
		const float* sourcePlus = source + sourceStrideElements * i;

		if (isSymmetric)
		{
			// we have a symmetric filter, so let's do some optimizations
			filterFactor_32x4 = vdupq_n_f32(filter[filterSize_2 + i]);

			// we sum the values of the upper and the lower row (as both will be multiplied with the same filter value)
			source_32x4a = vaddq_f32(vld1q_f32(sourceMinus + 4 * 0), vld1q_f32(sourcePlus + 4 * 0));
			source_32x4b = vaddq_f32(vld1q_f32(sourceMinus + 4 * 1), vld1q_f32(sourcePlus + 4 * 1));

			result_32x4a = vmlaq_f32(result_32x4a, source_32x4a, filterFactor_32x4);
			result_32x4b = vmlaq_f32(result_32x4b, source_32x4b, filterFactor_32x4);
		}
		else
		{
			// we don't have a symmetric filter, so we need to handle two individual filters

			float32x4_t filterFactor128Minus = vdupq_n_f32(filter[filterSize_2 - i]);
			float32x4_t filterFactor128Plus = vdupq_n_f32(filter[filterSize_2 + i]);

			float32x4_t source128aMinus = vld1q_f32(sourceMinus + 4 * 0);
			float32x4_t source128aPlus = vld1q_f32(sourcePlus + 4 * 0);

			float32x4_t source128bMinus = vld1q_f32(sourceMinus + 4 * 1);
			float32x4_t source128bPlus = vld1q_f32(sourcePlus + 4 * 1);

			result_32x4a = vmlaq_f32(result_32x4a, source128aMinus, filterFactor128Minus);
			result_32x4b = vmlaq_f32(result_32x4b, source128bMinus, filterFactor128Minus);

			result_32x4a = vmlaq_f32(result_32x4a, source128aPlus, filterFactor128Plus);
			result_32x4b = vmlaq_f32(result_32x4b, source128bPlus, filterFactor128Plus);
		}
	}

	vst1q_f32(target + 0, result_32x4a);
	vst1q_f32(target + 4, result_32x4b);
}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 20

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::filterVerticalCoreRow16Elements32BitPerChannelFloat<unsigned int, uint8_t, PI_SSE_2>(const unsigned int* source, uint8_t* target, const unsigned int sourceStrideElements, const float* filter, const unsigned int filterSize, const bool isSymmetric)
{
	ocean_assert(source != nullptr && target != nullptr && filter != nullptr);
	ocean_assert(filterSize >= 1u && (filterSize % 2u) == 1u);

	/**
	 * This function uses the following SSE instructions, and needs SSE2 or higher
	 *
	 * SSE1:
	 * _mm_set_ps1
	 * _mm_mul_ps
	 * _mm_add_ps
	 * _mm_loadu_ps
	 *
	 * SSE2:
	 * _mm_loadu_si128
	 * _mm_cvtepi32_ps
	 * _mm_add_epi32
	 * _mm_cvtps_epi32
	 * _mm_packs_epi32
	 * _mm_packus_epi16
	 * _mm_storeu_si128
	 */

	/**
	 * We determine 16 filter responses within one loop iteration.
	 * For a filter with size 5 for 1 channel frames we apply the following strategy:
	 *
	 * Source Data:
	 * Y
	 * Y
	 * Y 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
	 * Y 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4
	 * Y 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 <------------
	 * Y 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4
	 * Y 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
	 * Y
	 * Y
	 *
	 * Further, we use the fact that the filter kernel is symmetric so that we start at the center row (the target row) and then going to the filter's borders
	 *
	 * For frames with n channels the strategy stays the same.
	 */

	const unsigned int filterSize_2 = filterSize / 2u;

	const __m128i* sourceBlock = (const __m128i*)source;

	// we store one filter value in each of the four 32 bit integer values
	__m128 filterFactor_32x4 = _mm_set_ps1(filter[filterSize_2]);

	// now we load four input values, and multiply each of them with the center kernel value
	__m128 source_32x4a = _mm_cvtepi32_ps(_mm_loadu_si128(sourceBlock + 0));
	__m128 result_32x4a = _mm_mul_ps(source_32x4a, filterFactor_32x4);

	// now we load the next four input values, ...
	__m128 source_32x4b = _mm_cvtepi32_ps(_mm_loadu_si128(sourceBlock + 1));
	__m128 result_32x4b = _mm_mul_ps(source_32x4b, filterFactor_32x4);

	__m128 source_32x4c = _mm_cvtepi32_ps(_mm_loadu_si128(sourceBlock + 2));
	__m128 result_32x4c = _mm_mul_ps(source_32x4c, filterFactor_32x4);

	__m128 source_32x4d = _mm_cvtepi32_ps(_mm_loadu_si128(sourceBlock + 3));
	__m128 result_32x4d = _mm_mul_ps(source_32x4d, filterFactor_32x4);

	// now we proceed with the remaining filter values
	for (unsigned int i = 1u; i <= filterSize_2; ++i)
	{
		const __m128i* sourceMinus = (const __m128i*)(source - sourceStrideElements * i);
		const __m128i* sourcePlus = (const __m128i*)(source + sourceStrideElements * i);

		if (isSymmetric)
		{
			// we have a symmetric filter, so let's do some optimizations
			filterFactor_32x4 = _mm_set_ps1(filter[filterSize_2 + i]);

			// we sum the values of the upper and the lower row (as both will be multiplied with the same filter value)

			__m128i source128ai = _mm_add_epi32(_mm_loadu_si128(sourceMinus + 0), _mm_loadu_si128(sourcePlus + 0));
			__m128i source128bi = _mm_add_epi32(_mm_loadu_si128(sourceMinus + 1), _mm_loadu_si128(sourcePlus + 1));

			result_32x4a = _mm_add_ps(result_32x4a, _mm_mul_ps(_mm_cvtepi32_ps(source128ai), filterFactor_32x4));
			result_32x4b = _mm_add_ps(result_32x4b, _mm_mul_ps(_mm_cvtepi32_ps(source128bi), filterFactor_32x4));

			source128ai = _mm_add_epi32(_mm_loadu_si128(sourceMinus + 2), _mm_loadu_si128(sourcePlus + 2));
			source128bi = _mm_add_epi32(_mm_loadu_si128(sourceMinus + 3), _mm_loadu_si128(sourcePlus + 3));

			result_32x4c = _mm_add_ps(result_32x4c, _mm_mul_ps(_mm_cvtepi32_ps(source128ai), filterFactor_32x4));
			result_32x4d = _mm_add_ps(result_32x4d, _mm_mul_ps(_mm_cvtepi32_ps(source128bi), filterFactor_32x4));
		}
		else
		{
			// we don't have a symmetric filter, so we need to handle two individual filters
			__m128 filterFactor128Minus = _mm_set_ps1(filter[filterSize_2 - i]);
			__m128 filterFactor128Plus = _mm_set_ps1(filter[filterSize_2 + i]);

			__m128i source128aiMinus = _mm_loadu_si128(sourceMinus + 0);
			__m128i source128aiPlus = _mm_loadu_si128(sourcePlus + 0);

			__m128i source128biMinus = _mm_loadu_si128(sourceMinus + 1);
			__m128i source128biPlus = _mm_loadu_si128(sourcePlus + 1);

			result_32x4a = _mm_add_ps(result_32x4a, _mm_mul_ps(_mm_cvtepi32_ps(source128aiMinus), filterFactor128Minus));
			result_32x4b = _mm_add_ps(result_32x4b, _mm_mul_ps(_mm_cvtepi32_ps(source128biMinus), filterFactor128Minus));

			result_32x4a = _mm_add_ps(result_32x4a, _mm_mul_ps(_mm_cvtepi32_ps(source128aiPlus), filterFactor128Plus));
			result_32x4b = _mm_add_ps(result_32x4b, _mm_mul_ps(_mm_cvtepi32_ps(source128biPlus), filterFactor128Plus));

			__m128i source128ciMinus = _mm_loadu_si128(sourceMinus + 2);
			__m128i source128ciPlus = _mm_loadu_si128(sourcePlus + 2);

			__m128i source128diMinus = _mm_loadu_si128(sourceMinus + 3);
			__m128i source128diPlus = _mm_loadu_si128(sourcePlus + 3);

			result_32x4c = _mm_add_ps(result_32x4c, _mm_mul_ps(_mm_cvtepi32_ps(source128ciMinus), filterFactor128Minus));
			result_32x4d = _mm_add_ps(result_32x4d, _mm_mul_ps(_mm_cvtepi32_ps(source128diMinus), filterFactor128Minus));

			result_32x4c = _mm_add_ps(result_32x4c, _mm_mul_ps(_mm_cvtepi32_ps(source128ciPlus), filterFactor128Plus));
			result_32x4d = _mm_add_ps(result_32x4d, _mm_mul_ps(_mm_cvtepi32_ps(source128diPlus), filterFactor128Plus));
		}
	}

	// now we have 8 bit values in each 32 bit register
	__m128i result128ab = _mm_packs_epi32(_mm_cvtps_epi32(result_32x4a), _mm_cvtps_epi32(result_32x4b));
	__m128i result128cd = _mm_packs_epi32(_mm_cvtps_epi32(result_32x4c), _mm_cvtps_epi32(result_32x4d));
	__m128i result128 = _mm_packus_epi16(result128ab, result128cd);

	_mm_storeu_si128((__m128i*)target, result128);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::filterVerticalCoreRow16Elements32BitPerChannelFloat<float, float, PI_SSE_2>(const float* source, float* target, const unsigned int sourceStrideElements, const float* filter, const unsigned int filterSize, const bool isSymmetric)
{
	ocean_assert(source != nullptr && target != nullptr && filter != nullptr);
	ocean_assert(filterSize >= 1u && (filterSize % 2u) == 1u);

	/**
	 * This function uses the following SSE instructions, and needs SSE2 or higher
	 *
	 * SSE:
	 * _mm_set_ps1
	 * _mm_mul_ps
	 * _mm_add_ps
	 *
	 * SSE2:
	 * _mm_loadu_si128
	 * _mm_castsi128_ps
	 */

	/**
	 * We determine 16 filter responses within one loop iteration.
	 * For a filter with size 5 for 1 channel frames we apply the following strategy:
	 *
	 * Source Data:
	 * Y
	 * Y
	 * Y 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
	 * Y 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4
	 * Y 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 <------------
	 * Y 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4
	 * Y 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
	 * Y
	 * Y
	 *
	 * Further, we use the fact that the filter kernel is symmetric so that we start at the center row (the target row) and then going to the filter's borders
	 *
	 * For frames with n channels the strategy stays the same.
	 */

	const unsigned int filterSize_2 = filterSize / 2u;

	const __m128i* sourceBlock = (const __m128i*)source;

	// we store one filter value in each of the four 32 bit values
	__m128 filterFactor_32x4 = _mm_set_ps1(filter[filterSize_2]);

	// now we load four input values, and multiply each of them with the center kernel value
	__m128 source_32x4a = _mm_castsi128_ps(_mm_loadu_si128(sourceBlock + 0));
	__m128 result_32x4a = _mm_mul_ps(source_32x4a, filterFactor_32x4);

	// now we load the next four input values, ...
	__m128 source_32x4b = _mm_castsi128_ps(_mm_loadu_si128(sourceBlock + 1));
	__m128 result_32x4b = _mm_mul_ps(source_32x4b, filterFactor_32x4);

	__m128 source_32x4c = _mm_castsi128_ps(_mm_loadu_si128(sourceBlock + 2));
	__m128 result_32x4c = _mm_mul_ps(source_32x4c, filterFactor_32x4);

	__m128 source_32x4d = _mm_castsi128_ps(_mm_loadu_si128(sourceBlock + 3));
	__m128 result_32x4d = _mm_mul_ps(source_32x4d, filterFactor_32x4);

	// now we proceed with the remaining filter values
	for (unsigned int i = 1u; i <= filterSize_2; ++i)
	{
		const __m128i* sourceMinus = (const __m128i*)(source - sourceStrideElements * i);
		const __m128i* sourcePlus = (const __m128i*)(source + sourceStrideElements * i);

		if (isSymmetric)
		{
			// we have a symmetric filter, so let's do some optimizations
			filterFactor_32x4 = _mm_set_ps1(filter[filterSize_2 + i]);

			// we sum the values of the upper and the lower row (as both will be multiplied with the same filter value)

			source_32x4a = _mm_add_ps(_mm_castsi128_ps(_mm_loadu_si128(sourceMinus + 0)), _mm_castsi128_ps(_mm_loadu_si128(sourcePlus + 0)));
			source_32x4b = _mm_add_ps(_mm_castsi128_ps(_mm_loadu_si128(sourceMinus + 1)), _mm_castsi128_ps(_mm_loadu_si128(sourcePlus + 1)));

			result_32x4a = _mm_add_ps(result_32x4a, _mm_mul_ps(source_32x4a, filterFactor_32x4));
			result_32x4b = _mm_add_ps(result_32x4b, _mm_mul_ps(source_32x4b, filterFactor_32x4));

			source_32x4c = _mm_add_ps(_mm_castsi128_ps(_mm_loadu_si128(sourceMinus + 2)), _mm_castsi128_ps(_mm_loadu_si128(sourcePlus + 2)));
			source_32x4d = _mm_add_ps(_mm_castsi128_ps(_mm_loadu_si128(sourceMinus + 3)), _mm_castsi128_ps(_mm_loadu_si128(sourcePlus + 3)));

			result_32x4c = _mm_add_ps(result_32x4c, _mm_mul_ps(source_32x4c, filterFactor_32x4));
			result_32x4d = _mm_add_ps(result_32x4d, _mm_mul_ps(source_32x4d, filterFactor_32x4));
		}
		else
		{
			// we don't have a symmetric filter, so we need to handle two individual filters
			__m128 filterFactor_32x4Minus = _mm_set_ps1(filter[filterSize_2 - i]);
			__m128 filterFactor_32x4Plus = _mm_set_ps1(filter[filterSize_2 + i]);

			source_32x4a = _mm_castsi128_ps(_mm_loadu_si128(sourceMinus + 0));
			result_32x4a = _mm_add_ps(result_32x4a, _mm_mul_ps(source_32x4a, filterFactor_32x4Minus));

			source_32x4b = _mm_castsi128_ps(_mm_loadu_si128(sourceMinus + 1));
			result_32x4b = _mm_add_ps(result_32x4b, _mm_mul_ps(source_32x4b, filterFactor_32x4Minus));

			source_32x4c = _mm_castsi128_ps(_mm_loadu_si128(sourceMinus + 2));
			result_32x4c = _mm_add_ps(result_32x4c, _mm_mul_ps(source_32x4c, filterFactor_32x4Minus));

			source_32x4d = _mm_castsi128_ps(_mm_loadu_si128(sourceMinus + 3));
			result_32x4d = _mm_add_ps(result_32x4d, _mm_mul_ps(source_32x4d, filterFactor_32x4Minus));

			source_32x4a = _mm_castsi128_ps(_mm_loadu_si128(sourcePlus + 0));
			result_32x4a = _mm_add_ps(result_32x4a, _mm_mul_ps(source_32x4a, filterFactor_32x4Plus));

			source_32x4b = _mm_castsi128_ps(_mm_loadu_si128(sourcePlus + 1));
			result_32x4b = _mm_add_ps(result_32x4b, _mm_mul_ps(source_32x4b, filterFactor_32x4Plus));

			source_32x4c = _mm_castsi128_ps(_mm_loadu_si128(sourcePlus + 2));
			result_32x4c = _mm_add_ps(result_32x4c, _mm_mul_ps(source_32x4c, filterFactor_32x4Plus));

			source_32x4d = _mm_castsi128_ps(_mm_loadu_si128(sourcePlus + 3));
			result_32x4d = _mm_add_ps(result_32x4d, _mm_mul_ps(source_32x4d, filterFactor_32x4Plus));
		}
	}

	writeSIMD<float, PI_SSE_2>(result_32x4a, target +  0);
	writeSIMD<float, PI_SSE_2>(result_32x4b, target +  4);
	writeSIMD<float, PI_SSE_2>(result_32x4c, target +  8);
	writeSIMD<float, PI_SSE_2>(result_32x4d, target + 12);
}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 20

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::filterVerticalCoreRow16Elements32BitPerChannelFloat<unsigned int, uint8_t, PI_NEON>(const unsigned int* source, uint8_t* target, const unsigned int sourceStrideElements, const float* filter, const unsigned int filterSize, const bool isSymmetric)
{
	const unsigned int filterSize_2 = filterSize / 2u;

	// we store one filter value in each of the four 32 bit integer values
	float32x4_t filterFactor_32x4 = vdupq_n_f32(filter[filterSize_2]);

	// now we load four input values, and multiply each of them with the center kernel value
	float32x4_t source_32x4a = vcvtq_f32_u32(vld1q_u32(source + 4 * 0));
	float32x4_t result_32x4a = vmulq_f32(source_32x4a, filterFactor_32x4);

	float32x4_t source_32x4b = vcvtq_f32_u32(vld1q_u32(source + 4 * 1));
	float32x4_t result_32x4b = vmulq_f32(source_32x4b, filterFactor_32x4);

	float32x4_t source_32x4c = vcvtq_f32_u32(vld1q_u32(source + 4 * 2));
	float32x4_t result_32x4c = vmulq_f32(source_32x4c, filterFactor_32x4);

	float32x4_t source_32x4d = vcvtq_f32_u32(vld1q_u32(source + 4 * 3));
	float32x4_t result_32x4d = vmulq_f32(source_32x4d, filterFactor_32x4);

	// now we proceed with the remaining filter values
	for (unsigned int i = 1u; i <= filterSize_2; ++i)
	{
		const unsigned int* sourceMinus = source - sourceStrideElements * i;
		const unsigned int* sourcePlus = source + sourceStrideElements * i;

		if (isSymmetric)
		{
			// we have a symmetric filter, so let's do some optimizations
			filterFactor_32x4 = vdupq_n_f32(filter[filterSize_2 + i]);

			// we sum the values of the upper and the lower row (as both will be multiplied with the same filter value)
			uint32x4_t source128ai = vaddq_u32(vld1q_u32(sourceMinus + 4 * 0), vld1q_u32(sourcePlus + 4 * 0));
			uint32x4_t source128bi = vaddq_u32(vld1q_u32(sourceMinus + 4 * 1), vld1q_u32(sourcePlus + 4 * 1));

			result_32x4a = vmlaq_f32(result_32x4a, vcvtq_f32_u32(source128ai), filterFactor_32x4);
			result_32x4b = vmlaq_f32(result_32x4b, vcvtq_f32_u32(source128bi), filterFactor_32x4);

			source128ai = vaddq_u32(vld1q_u32(sourceMinus + 4 * 2), vld1q_u32(sourcePlus + 4 * 2));
			source128bi = vaddq_u32(vld1q_u32(sourceMinus + 4 * 3), vld1q_u32(sourcePlus + 4 * 3));

			result_32x4c = vmlaq_f32(result_32x4c, vcvtq_f32_u32(source128ai), filterFactor_32x4);
			result_32x4d = vmlaq_f32(result_32x4d, vcvtq_f32_u32(source128bi), filterFactor_32x4);
		}
		else
		{
			// we don't have a symmetric filter, so we need to handle two individual filters

			float32x4_t filterFactor128Minus = vdupq_n_f32(filter[filterSize_2 - i]);
			float32x4_t filterFactor128Plus = vdupq_n_f32(filter[filterSize_2 + i]);

			uint32x4_t source128aiMinus = vld1q_u32(sourceMinus + 4 * 0);
			result_32x4a = vmlaq_f32(result_32x4a, vcvtq_f32_u32(source128aiMinus), filterFactor128Minus);

			uint32x4_t source128biMinus = vld1q_u32(sourceMinus + 4 * 1);
			result_32x4b = vmlaq_f32(result_32x4b, vcvtq_f32_u32(source128biMinus), filterFactor128Minus);

			uint32x4_t source128aiPlus = vld1q_u32(sourcePlus + 4 * 0);
			result_32x4a = vmlaq_f32(result_32x4a, vcvtq_f32_u32(source128aiPlus), filterFactor128Plus);

			uint32x4_t source128biPlus = vld1q_u32(sourcePlus + 4 * 1);
			result_32x4b = vmlaq_f32(result_32x4b, vcvtq_f32_u32(source128biPlus), filterFactor128Plus);

			uint32x4_t source128ciMinus = vld1q_u32(sourceMinus + 4 * 2);
			result_32x4c = vmlaq_f32(result_32x4c, vcvtq_f32_u32(source128ciMinus), filterFactor128Minus);

			uint32x4_t source128diMinus = vld1q_u32(sourceMinus + 4 * 3);
			result_32x4d = vmlaq_f32(result_32x4d, vcvtq_f32_u32(source128diMinus), filterFactor128Minus);

			uint32x4_t source128ciPlus = vld1q_u32(sourcePlus + 4 * 2);
			result_32x4c = vmlaq_f32(result_32x4c, vcvtq_f32_u32(source128ciPlus), filterFactor128Plus);

			uint32x4_t source128diPlus = vld1q_u32(sourcePlus + 4 * 3);
			result_32x4d = vmlaq_f32(result_32x4d, vcvtq_f32_u32(source128diPlus), filterFactor128Plus);
		}
	}

	// now we have 8 bit values in each 32 bit register
	uint16x8_t result128ab = vcombine_u16(vqmovn_u32(vcvtq_u32_f32(result_32x4a)), vqmovn_u32(vcvtq_u32_f32(result_32x4b)));
	uint16x8_t result128cd = vcombine_u16(vqmovn_u32(vcvtq_u32_f32(result_32x4c)), vqmovn_u32(vcvtq_u32_f32(result_32x4d)));

	uint8x16_t result128 = vcombine_u8(vqmovn_u16(result128ab), vqmovn_u16(result128cd));

	vst1q_u8(target, result128);
}

template <>
	OCEAN_FORCE_INLINE void FrameFilterSeparable::filterVerticalCoreRow16Elements32BitPerChannelFloat<float, float, PI_NEON>(const float* source, float* target, const unsigned int sourceStrideElements, const float* filter, const unsigned int filterSize, const bool isSymmetric)
{
	const unsigned int filterSize_2 = filterSize / 2u;

	// we store one filter value in each of the four 32 bit integer values
	float32x4_t filterFactor_32x4 = vdupq_n_f32(filter[filterSize_2]);

	// now we load four input values, and multiply each of them with the center kernel value
	float32x4_t source_32x4a = vld1q_f32(source + 4 * 0);
	float32x4_t result_32x4a = vmulq_f32(source_32x4a, filterFactor_32x4);

	float32x4_t source_32x4b = vld1q_f32(source + 4 * 1);
	float32x4_t result_32x4b = vmulq_f32(source_32x4b, filterFactor_32x4);

	float32x4_t source_32x4c = vld1q_f32(source + 4 * 2);
	float32x4_t result_32x4c = vmulq_f32(source_32x4c, filterFactor_32x4);

	float32x4_t source_32x4d = vld1q_f32(source + 4 * 3);
	float32x4_t result_32x4d = vmulq_f32(source_32x4d, filterFactor_32x4);

	// now we proceed with the remaining filter values
	for (unsigned int i = 1u; i <= filterSize_2; ++i)
	{
		const float* sourceMinus = source - sourceStrideElements * i;
		const float* sourcePlus = source + sourceStrideElements * i;

		if (isSymmetric)
		{
			// we have a symmetric filter, so let's do some optimizations
			filterFactor_32x4 = vdupq_n_f32(filter[filterSize_2 + i]);

			// we sum the values of the upper and the lower row (as both will be multiplied with the same filter value)
			source_32x4a = vaddq_f32(vld1q_f32(sourceMinus + 4 * 0), vld1q_f32(sourcePlus + 4 * 0));
			source_32x4b = vaddq_f32(vld1q_f32(sourceMinus + 4 * 1), vld1q_f32(sourcePlus + 4 * 1));

			result_32x4a = vmlaq_f32(result_32x4a, source_32x4a, filterFactor_32x4);
			result_32x4b = vmlaq_f32(result_32x4b, source_32x4b, filterFactor_32x4);

			source_32x4c = vaddq_f32(vld1q_f32(sourceMinus + 4 * 2), vld1q_f32(sourcePlus + 4 * 2));
			source_32x4d = vaddq_f32(vld1q_f32(sourceMinus + 4 * 3), vld1q_f32(sourcePlus + 4 * 3));

			result_32x4c = vmlaq_f32(result_32x4c, source_32x4c, filterFactor_32x4);
			result_32x4d = vmlaq_f32(result_32x4d, source_32x4d, filterFactor_32x4);
		}
		else
		{
			// we don't have a symmetric filter, so we need to handle two individual filters

			float32x4_t filterFactor128Minus = vdupq_n_f32(filter[filterSize_2 - i]);
			float32x4_t filterFactor128Plus = vdupq_n_f32(filter[filterSize_2 + i]);

			float32x4_t source128aMinus = vld1q_f32(sourceMinus + 4 * 0);
			float32x4_t source128aPlus = vld1q_f32(sourcePlus + 4 * 0);

			float32x4_t source128bMinus = vld1q_f32(sourceMinus + 4 * 1);
			float32x4_t source128bPlus = vld1q_f32(sourcePlus + 4 * 1);

			result_32x4a = vmlaq_f32(result_32x4a, source128aMinus, filterFactor128Minus);
			result_32x4b = vmlaq_f32(result_32x4b, source128bMinus, filterFactor128Minus);

			result_32x4a = vmlaq_f32(result_32x4a, source128aPlus, filterFactor128Plus);
			result_32x4b = vmlaq_f32(result_32x4b, source128bPlus, filterFactor128Plus);

			source128aMinus = vld1q_f32(sourceMinus + 4 * 2);
			source128aPlus = vld1q_f32(sourcePlus + 4 * 2);

			source128bMinus = vld1q_f32(sourceMinus + 4 * 3);
			source128bPlus = vld1q_f32(sourcePlus + 4 * 3);

			result_32x4c = vmlaq_f32(result_32x4c, source128aMinus, filterFactor128Minus);
			result_32x4d = vmlaq_f32(result_32x4d, source128bMinus, filterFactor128Minus);

			result_32x4c = vmlaq_f32(result_32x4c, source128aPlus, filterFactor128Plus);
			result_32x4d = vmlaq_f32(result_32x4d, source128bPlus, filterFactor128Plus);
		}
	}

	vst1q_f32(target +  0, result_32x4a);
	vst1q_f32(target +  4, result_32x4b);
	vst1q_f32(target +  8, result_32x4c);
	vst1q_f32(target + 12, result_32x4d);
}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 20

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::filterVerticalBorderRow8Elements32BitPerChannelFloat<unsigned int, uint8_t, PI_SSE_2>(const unsigned int* source, uint8_t* target, const unsigned int sourceStrideElements, const unsigned int height, const unsigned int row, const float* filter, const unsigned int filterSize, const bool isSymmetric)
{
	ocean_assert(source != nullptr && target != nullptr && filter != nullptr);
	ocean_assert(filterSize >= 1u && (filterSize % 2u) == 1u);

	/**
	 * This function uses the following SSE instructions, and needs SSE2 or higher
	 *
	 * SSE1:
	 * _mm_set_ps1
	 * _mm_mul_ps
	 * _mm_add_ps
	 * _mm_loadu_ps
	 *
	 * SSE2:
	 * _mm_loadu_si128
	 * _mm_cvtepi32_ps
	 * _mm_add_epi32
	 * _mm_cvtps_epi32
	 * _mm_packs_epi32
	 * _mm_packus_epi16
	 * _mm_storel_epi64
	 */

	const unsigned int filterSize_2 = filterSize / 2u;

	// the border covers row ids within the range [0, filterSize_2)
	ocean_assert(row < filterSize_2 || row + filterSize_2 >= height);

	const __m128i* sourceBlock = (const __m128i*)source;

	// we store one filter value in each of the four 32 bit integer values
	__m128 filterFactor_32x4 = _mm_set_ps1(filter[filterSize_2]);

	// now we load four input values, and multiply each of them with the center kernel value
	__m128 source_32x4a = _mm_cvtepi32_ps(_mm_loadu_si128(sourceBlock + 0));
	__m128 result_32x4a = _mm_mul_ps(source_32x4a, filterFactor_32x4);

	// now we load the next four input values, ...
	__m128 source_32x4b = _mm_cvtepi32_ps(_mm_loadu_si128(sourceBlock + 1));
	__m128 result_32x4b = _mm_mul_ps(source_32x4b, filterFactor_32x4);

	__m128i source128ai, source128bi;

	// now we proceed with the remaining filter values
	for (unsigned int i = 1u; i <= filterSize_2; ++i)
	{
		// we determine the mirrored locations (and the row offset in relation to the current row)
		const int offsetMinus = int(mirroredBorderLocationLeft(int(row) - int(i))) - int(row);
		const int offsetPlus = int(mirroredBorderLocationRight(row + i, height)) - int(row);

		// depending on whether we are at the top border or at the bottom border we change the minus and plus source values
		const unsigned int* sourceMinus = source + offsetMinus * int(sourceStrideElements);
		const unsigned int* sourcePlus = source + offsetPlus * int(sourceStrideElements);

		if (isSymmetric)
		{
			// we have a symmetric filter, so let's do some optimizations
			filterFactor_32x4 = _mm_set_ps1(filter[filterSize_2 + i]);

			// we sum the values of the upper and the lower row (as both will be multiplied with the same filter value)

			source128ai = _mm_add_epi32(_mm_loadu_si128((const __m128i*)sourceMinus + 0), _mm_loadu_si128((const __m128i*)sourcePlus + 0));
			source128bi = _mm_add_epi32(_mm_loadu_si128((const __m128i*)sourceMinus + 1), _mm_loadu_si128((const __m128i*)sourcePlus + 1));

			result_32x4a = _mm_add_ps(result_32x4a, _mm_mul_ps(_mm_cvtepi32_ps(source128ai), filterFactor_32x4));
			result_32x4b = _mm_add_ps(result_32x4b, _mm_mul_ps(_mm_cvtepi32_ps(source128bi), filterFactor_32x4));
		}
		else
		{
			// we don't have a symmetric filter, so we need to handle two individual filters

			__m128 filterFactor128Minus = _mm_set_ps1(filter[filterSize_2 - i]);
			__m128 filterFactor128Plus = _mm_set_ps1(filter[filterSize_2 + i]);

			// we sum the values of the upper and the lower row (as both will be multiplied with the same filter value)

			source128ai = _mm_loadu_si128((const __m128i*)sourceMinus + 0);
			result_32x4a = _mm_add_ps(result_32x4a, _mm_mul_ps(_mm_cvtepi32_ps(source128ai), filterFactor128Minus));

			source128bi = _mm_loadu_si128((const __m128i*)sourceMinus + 1);
			result_32x4b = _mm_add_ps(result_32x4b, _mm_mul_ps(_mm_cvtepi32_ps(source128bi), filterFactor128Minus));

			source128ai = _mm_loadu_si128((const __m128i*)sourcePlus + 0);
			result_32x4a = _mm_add_ps(result_32x4a, _mm_mul_ps(_mm_cvtepi32_ps(source128ai), filterFactor128Plus));

			source128bi = _mm_loadu_si128((const __m128i*)sourcePlus + 1);
			result_32x4b = _mm_add_ps(result_32x4b, _mm_mul_ps(_mm_cvtepi32_ps(source128bi), filterFactor128Plus));
		}
	}

	// now we have 8 bit values in each 32 bit register
	__m128i result128 = _mm_packs_epi32(_mm_cvtps_epi32(result_32x4a), _mm_cvtps_epi32(result_32x4b));
	result128 = _mm_packus_epi16(result128, result128);

	_mm_storel_epi64((__m128i*)target, result128);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::filterVerticalBorderRow8Elements32BitPerChannelFloat<float, float, PI_SSE_2>(const float* source, float* target, const unsigned int sourceStrideElements, const unsigned int height, const unsigned int row, const float* filter, const unsigned int filterSize, const bool isSymmetric)
{
	ocean_assert(source != nullptr && target != nullptr && filter != nullptr);
	ocean_assert(filterSize >= 1u && (filterSize % 2u) == 1u);

	/**
	 * This function uses the following SSE instructions, and needs SSE2 or higher
	 *
	 * SSE:
	 * _mm_set_ps1
	 * _mm_mul_ps
	 * _mm_add_ps
	 *
	 * SSE2:
	 * _mm_loadu_si128
	 * _mm_castsi128_ps
	 */

	const unsigned int filterSize_2 = filterSize / 2u;

	// the border covers row ids within the range [0, filterSize_2)
	ocean_assert(row < filterSize_2 || row + filterSize_2 >= height);

	const __m128i* sourceBlock = (const __m128i*)source;

	// we store one filter value in each of the four 32 bit values
	__m128 filterFactor_32x4 = _mm_set_ps1(filter[filterSize_2]);

	// now we load four input values, and multiply each of them with the center kernel value
	__m128 source_32x4a = _mm_castsi128_ps(_mm_loadu_si128(sourceBlock + 0));
	__m128 result_32x4a = _mm_mul_ps(source_32x4a, filterFactor_32x4);

	// now we load the next four input values, ...
	__m128 source_32x4b = _mm_castsi128_ps(_mm_loadu_si128(sourceBlock + 1));
	__m128 result_32x4b = _mm_mul_ps(source_32x4b, filterFactor_32x4);

	// now we proceed with the remaining filter values
	for (unsigned int i = 1u; i <= filterSize_2; ++i)
	{
		// we determine the mirrored locations (and the row offset in relation to the current row)
		const int offsetMinus = int(mirroredBorderLocationLeft(int(row) - int(i))) - int(row);
		const int offsetPlus = int(mirroredBorderLocationRight(row + i, height)) - int(row);

		// depending on whether we are at the top border or at the bottom border we change the minus and plus source values
		const float* sourceMinus = source + offsetMinus * int(sourceStrideElements);
		const float* sourcePlus = source + offsetPlus * int(sourceStrideElements);

		if (isSymmetric)
		{
			// we have a symmetric filter, so let's do some optimizations
			filterFactor_32x4 = _mm_set_ps1(filter[filterSize_2 + i]);

			// we sum the values of the upper and the lower row (as both will be multiplied with the same filter value)

			source_32x4a = _mm_add_ps(_mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourceMinus + 0)), _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourcePlus + 0)));
			source_32x4b = _mm_add_ps(_mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourceMinus + 1)), _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourcePlus + 1)));

			result_32x4a = _mm_add_ps(result_32x4a, _mm_mul_ps(source_32x4a, filterFactor_32x4));
			result_32x4b = _mm_add_ps(result_32x4b, _mm_mul_ps(source_32x4b, filterFactor_32x4));
		}
		else
		{
			// we don't have a symmetric filter, so we need to handle two individual filters
			__m128 filterFactor_32x4Minus = _mm_set_ps1(filter[filterSize_2 - i]);
			__m128 filterFactor_32x4Plus = _mm_set_ps1(filter[filterSize_2 + i]);

			source_32x4a = _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourceMinus + 0));
			result_32x4a = _mm_add_ps(result_32x4a, _mm_mul_ps(source_32x4a, filterFactor_32x4Minus));

			source_32x4b = _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourceMinus + 1));
			result_32x4b = _mm_add_ps(result_32x4b, _mm_mul_ps(source_32x4b, filterFactor_32x4Minus));

			source_32x4a = _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourcePlus + 0));
			result_32x4a = _mm_add_ps(result_32x4a, _mm_mul_ps(source_32x4a, filterFactor_32x4Plus));

			source_32x4b = _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourcePlus + 1));
			result_32x4b = _mm_add_ps(result_32x4b, _mm_mul_ps(source_32x4b, filterFactor_32x4Plus));
		}
	}

	writeSIMD<float, PI_SSE_2>(result_32x4a, target + 0);
	writeSIMD<float, PI_SSE_2>(result_32x4b, target + 4);
}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 20

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::filterVerticalBorderRow8Elements32BitPerChannelFloat<unsigned int, uint8_t, PI_NEON>(const unsigned int* source, uint8_t* target, const unsigned int sourceStrideElements, const unsigned int height, const unsigned int row, const float* filter, const unsigned int filterSize, const bool isSymmetric)
{
	ocean_assert(source != nullptr && target != nullptr && filter != nullptr);
	ocean_assert(filterSize % 2u == 1u);

	const unsigned int filterSize_2 = filterSize / 2u;

	// the border covers row ids within the range [0, filterSize_2)
	ocean_assert(row < filterSize_2 || row + filterSize_2 >= height);

	// we store one filter value in each of the four 32 bit integer values
	float32x4_t filterFactor_32x4 = vdupq_n_f32(filter[filterSize_2]);

	// now we load four input values, and multiply each of them with the center kernel value
	float32x4_t source_32x4a = vcvtq_f32_u32(vld1q_u32(source + 4 * 0));
	float32x4_t result_32x4a = vmulq_f32(source_32x4a, filterFactor_32x4);

	float32x4_t source_32x4b = vcvtq_f32_u32(vld1q_u32(source + 4 * 1));
	float32x4_t result_32x4b = vmulq_f32(source_32x4b, filterFactor_32x4);

	// now we proceed with the remaining filter values
	for (unsigned int i = 1u; i <= filterSize_2; ++i)
	{
		// we determine the mirrored locations (and the row offset in relation to the current row)
		const int offsetMinus = int(mirroredBorderLocationLeft(int(row) - int(i))) - int(row);
		const int offsetPlus = int(mirroredBorderLocationRight(row + i, height)) - int(row);

		// depending on whether we are at the top border or at the bottom border we change the minus and plus source values
		const unsigned int* sourceMinus = source + offsetMinus * int(sourceStrideElements);
		const unsigned int* sourcePlus = source + offsetPlus * int(sourceStrideElements);

		if (isSymmetric)
		{
			// we have a symmetric filter, so let's do some optimizations
			filterFactor_32x4 = vdupq_n_f32(filter[filterSize_2 + i]);

			// we sum the values of the upper and the lower row (as both will be multiplied with the same filter value)

			uint32x4_t source128ai = vaddq_u32(vld1q_u32(sourceMinus + 4 * 0), vld1q_u32(sourcePlus + 4 * 0));
			uint32x4_t source128bi = vaddq_u32(vld1q_u32(sourceMinus + 4 * 1), vld1q_u32(sourcePlus + 4 * 1));

			result_32x4a = vmlaq_f32(result_32x4a, vcvtq_f32_u32(source128ai), filterFactor_32x4);
			result_32x4b = vmlaq_f32(result_32x4b, vcvtq_f32_u32(source128bi), filterFactor_32x4);
		}
		else
		{
			// we don't have a symmetric filter, so we need to handle two individual filters

			float32x4_t filterFactor128Minus = vdupq_n_f32(filter[filterSize_2 - i]);
			float32x4_t filterFactor128Plus = vdupq_n_f32(filter[filterSize_2 + i]);

			uint32x4_t source128aiMinus = vld1q_u32(sourceMinus + 4 * 0);
			result_32x4a = vmlaq_f32(result_32x4a, vcvtq_f32_u32(source128aiMinus), filterFactor128Minus);

			uint32x4_t source128biMinus = vld1q_u32(sourceMinus + 4 * 1);
			result_32x4b = vmlaq_f32(result_32x4b, vcvtq_f32_u32(source128biMinus), filterFactor128Minus);

			uint32x4_t source128aiPlus = vld1q_u32(sourcePlus + 4 * 0);
			result_32x4a = vmlaq_f32(result_32x4a, vcvtq_f32_u32(source128aiPlus), filterFactor128Plus);

			uint32x4_t source128biPlus = vld1q_u32(sourcePlus + 4 * 1);
			result_32x4b = vmlaq_f32(result_32x4b, vcvtq_f32_u32(source128biPlus), filterFactor128Plus);
		}
	}

	// now we have 8 bit values in each 32 bit register
	uint16x8_t result128ab = vcombine_u16(vqmovn_u32(vcvtq_u32_f32(result_32x4a)), vqmovn_u32(vcvtq_u32_f32(result_32x4b)));

	uint8x8_t result64 = vqmovn_u16(result128ab);

	vst1_u8(target, result64);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::filterVerticalBorderRow8Elements32BitPerChannelFloat<float, float, PI_NEON>(const float* source, float* target, const unsigned int sourceStrideElements, const unsigned int height, const unsigned int row, const float* filter, const unsigned int filterSize, const bool isSymmetric)
{
	ocean_assert(source != nullptr && target != nullptr && filter != nullptr);
	ocean_assert(filterSize % 2u == 1u);

	const unsigned int filterSize_2 = filterSize / 2u;

	// the border covers row ids within the range [0, filterSize_2)
	ocean_assert(row < filterSize_2 || row + filterSize_2 >= height);

	// we store one filter value in each of the four 32 bit integer values
	float32x4_t filterFactor_32x4 = vdupq_n_f32(filter[filterSize_2]);

	// now we load four input values, and multiply each of them with the center kernel value
	float32x4_t source_32x4a = vld1q_f32(source + 4 * 0);
	float32x4_t result_32x4a = vmulq_f32(source_32x4a, filterFactor_32x4);

	float32x4_t source_32x4b = vld1q_f32(source + 4 * 1);
	float32x4_t result_32x4b = vmulq_f32(source_32x4b, filterFactor_32x4);

	// now we proceed with the remaining filter values
	for (unsigned int i = 1u; i <= filterSize_2; ++i)
	{
		// we determine the mirrored locations (and the row offset in relation to the current row)
		const int offsetMinus = int(mirroredBorderLocationLeft(int(row) - int(i))) - int(row);
		const int offsetPlus = int(mirroredBorderLocationRight(row + i, height)) - int(row);

		// depending on whether we are at the top border or at the bottom border we change the minus and plus source values
		const float* sourceMinus = source + offsetMinus * int(sourceStrideElements);
		const float* sourcePlus = source + offsetPlus * int(sourceStrideElements);

		if (isSymmetric)
		{
			// we have a symmetric filter, so let's do some optimizations
			filterFactor_32x4 = vdupq_n_f32(filter[filterSize_2 + i]);

			// we sum the values of the upper and the lower row (as both will be multiplied with the same filter value)

			source_32x4a = vaddq_f32(vld1q_f32(sourceMinus + 4 * 0), vld1q_f32(sourcePlus + 4 * 0));
			source_32x4b = vaddq_f32(vld1q_f32(sourceMinus + 4 * 1), vld1q_f32(sourcePlus + 4 * 1));

			result_32x4a = vmlaq_f32(result_32x4a, source_32x4a, filterFactor_32x4);
			result_32x4b = vmlaq_f32(result_32x4b, source_32x4b, filterFactor_32x4);
		}
		else
		{
			// we don't have a symmetric filter, so we need to handle two individual filters

			float32x4_t filterFactor128Minus = vdupq_n_f32(filter[filterSize_2 - i]);
			float32x4_t filterFactor128Plus = vdupq_n_f32(filter[filterSize_2 + i]);

			float32x4_t source128aMinus = vld1q_f32(sourceMinus + 4 * 0);
			float32x4_t source128aPlus = vld1q_f32(sourcePlus + 4 * 0);

			float32x4_t source128bMinus = vld1q_f32(sourceMinus + 4 * 1);
			float32x4_t source128bPlus = vld1q_f32(sourcePlus + 4 * 1);

			result_32x4a = vmlaq_f32(result_32x4a, source128aMinus, filterFactor128Minus);
			result_32x4b = vmlaq_f32(result_32x4b, source128bMinus, filterFactor128Minus);

			result_32x4a = vmlaq_f32(result_32x4a, source128aPlus, filterFactor128Plus);
			result_32x4b = vmlaq_f32(result_32x4b, source128bPlus, filterFactor128Plus);
		}
	}

	vst1q_f32(target + 0, result_32x4a);
	vst1q_f32(target + 4, result_32x4b);
}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 20

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::asymmetricFilterHorizontalRowMultiplyOneFilterFactor4Elements<uint8_t, unsigned int, ProcessorInstructions::PI_SSE_2>(const uint8_t* source, const unsigned int& filterFactor, SIMD32x4<unsigned int>::Type& target)
{
	/**
	 * This function uses the following SSE instructions, and needs SSE2 or higher
	 *
	 * SSE2:
	 * _mm_set1_epi32
	 * _mm_unpacklo_epi8
	 * _mm_unpackhi_epi16
	 * _mm_setzero_si128
	 * _mm_madd_epi16
	 * _mm_add_epi32
	 */

	// we store one filter value in each of the four 32 bit integer values
	__m128i filterFactor_32x4 = _mm_set1_epi32(int(filterFactor));

	// we load four source values into the lower 32 bit of our 128 bit register
	__m128i source128 = _mm_set1_epi32(*((const int*)source));

	// we separate the source values to receive 16 bit integers
	source128 = _mm_unpacklo_epi8(source128, _mm_setzero_si128());

	// we separate the 16 bit values further so that we receive 32 bit integers
	source128 = _mm_unpackhi_epi16(source128, _mm_setzero_si128());

	// we multiply each value with the same filter factor, and sum the result
	source128 = _mm_madd_epi16(source128, filterFactor_32x4);

	// we add the local result to the sum parameters
	target = _mm_add_epi32(target, source128);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::asymmetricFilterHorizontalRowMultiplyOneFilterFactor4Elements<float, float, ProcessorInstructions::PI_SSE_2>(const float* source, const float& filterFactor, SIMD32x4<float>::Type& target_32x4)
{
	/**
	 * This function uses the following SSE instructions, and needs SSE2 or higher
	 *
	 * SSE:
	 * _mm_set_ps1
	 * _mm_mul_ps
	 * _mm_add_ps
	 *
	 * SSE2:
	 * _mm_loadu_si128
	 * _mm_castsi128_ps
	 */

	// we store one filter value in each of the four 32 bit values
	__m128 filterFactor_32x4 = _mm_set_ps1(filterFactor);

	// we load 8 source values into two 128 bit registers
	__m128 source_32x4 = _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)source));

	// we multiply each value with the same filter factor
	source_32x4 = _mm_mul_ps(source_32x4, filterFactor_32x4);

	// we add the local result to the sum parameters
	target_32x4 = _mm_add_ps(target_32x4, source_32x4);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::symmetricFilterHorizontalRowMultiplyOneFilterFactor4Elements<uint8_t, unsigned int, ProcessorInstructions::PI_SSE_2>(const uint8_t* sourceLeft, const uint8_t* sourceRight, const unsigned int& filterFactor, SIMD32x4<unsigned int>::Type& target)
{
	/**
	 * This function uses the following SSE instructions, and needs SSE2 or higher
	 *
	 * SSE2:
	 * _mm_set1_epi32
	 * _mm_unpacklo_epi8
	 * _mm_unpackhi_epi16
	 * _mm_setzero_si128
	 * _mm_madd_epi16
	 * _mm_add_epi32
	 */

	// we store one filter value in each of the four 32 bit integer values
	__m128i filterFactor_32x4 = _mm_set1_epi32(int(filterFactor));

	// we load 4 source values from the left side and 4 source values from the right side, we separate the values to receive 16 bit integers and add them together
	__m128i source128 = _mm_add_epi16(_mm_unpacklo_epi8(_mm_set1_epi32(*((const int*)sourceLeft)), _mm_setzero_si128()), _mm_unpacklo_epi8(_mm_set1_epi32(*((const int*)sourceRight)), _mm_setzero_si128()));

	// we separate the 16 bit values further so that we receive 32 bit integers
	source128 = _mm_unpackhi_epi16(source128, _mm_setzero_si128());

	// we multiply each value with the same filter factor, and sum the result
	source128 = _mm_madd_epi16(source128, filterFactor_32x4);

	// we add the local result to the sum parameters
	target = _mm_add_epi32(target, source128);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::symmetricFilterHorizontalRowMultiplyOneFilterFactor4Elements<float, float, ProcessorInstructions::PI_SSE_2>(const float* sourceLeft, const float* sourceRight, const float& filterFactor, SIMD32x4<float>::Type& target_32x4)
{
	/**
	 * This function uses the following SSE instructions, and needs SSE2 or higher
	 *
	 * SSE:
	 * _mm_set_ps1
	 * _mm_mul_ps
	 * _mm_add_ps
	 *
	 * SSE2:
	 * _mm_loadu_si128
	 * _mm_castsi128_ps
	 */

	// we store one filter value in each of the four 32 bit values
	__m128 filterFactor_32x4 = _mm_set_ps1(filterFactor);

	// we load 4 * 2  source values and add them together
	__m128 source_32x4 = _mm_add_ps(_mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourceLeft)), _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourceRight)));

	// we multiply each value with the same filter factor
	source_32x4 = _mm_mul_ps(source_32x4, filterFactor_32x4);

	// we add the local result to the sum parameters
	target_32x4 = _mm_add_ps(target_32x4, source_32x4);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::asymmetricFilterHorizontalRowMultiplyOneFilterFactor8Elements<uint8_t, unsigned int, ProcessorInstructions::PI_SSE_2>(const uint8_t* source, const unsigned int& filterFactor, SIMD32x4<unsigned int>::Type& target_32x4a, SIMD32x4<unsigned int>::Type& target_32x4b)
{
	/**
	 * This function uses the following SSE instructions, and needs SSE2 or higher
	 *
	 * SSE2:
	 * _mm_set1_epi32
	 * _mm_loadl_epi64
	 * _mm_unpacklo_epi8
	 * _mm_unpackhi_epi16
	 * _mm_unpacklo_epi16
	 * _mm_setzero_si128
	 * _mm_madd_epi16
	 * _mm_add_epi32
	 */

	// we store one filter value in each of the four 32 bit integer values
	__m128i filterFactor_32x4 = _mm_set1_epi32(int(filterFactor));

	// we load eight source values into the lower 64 bit of our 128 bit register
	__m128i source_32x4a = _mm_loadl_epi64((const __m128i*)source);

	// we separate the source values to receive 16 bit integers
	source_32x4a = _mm_unpacklo_epi8(source_32x4a, _mm_setzero_si128());

	// we separate the 16 bit values further so that we receive 32 bit integers
	__m128i source_32x4b = _mm_unpackhi_epi16(source_32x4a, _mm_setzero_si128());
	source_32x4a = _mm_unpacklo_epi16(source_32x4a, _mm_setzero_si128());

	// we multiply each value with the same filter factor, and sum the result
	source_32x4a = _mm_madd_epi16(source_32x4a, filterFactor_32x4);
	source_32x4b = _mm_madd_epi16(source_32x4b, filterFactor_32x4);

	// we add the local result to the sum parameters
	target_32x4a = _mm_add_epi32(target_32x4a, source_32x4a);
	target_32x4b = _mm_add_epi32(target_32x4b, source_32x4b);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::asymmetricFilterHorizontalRowMultiplyOneFilterFactor8Elements<float, float, ProcessorInstructions::PI_SSE_2>(const float* source, const float& filterFactor, SIMD32x4<float>::Type& target_32x4a, SIMD32x4<float>::Type& target_32x4b)
{
	/**
	 * This function uses the following SSE instructions, and needs SSE2 or higher
	 *
	 * SSE:
	 * _mm_set_ps1
	 * _mm_mul_ps
	 * _mm_add_ps
	 *
	 * SSE2:
	 * _mm_loadu_si128
	 * _mm_castsi128_ps
	 */

	// we store one filter value in each of the four 32 bit values
	__m128 filterFactor_32x4 = _mm_set_ps1(filterFactor);

	// we load 8 source values into two 128 bit registers
	__m128 source_32x4a = _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)source + 0));
	__m128 source_32x4b = _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)source + 1));

	// we multiply each value with the same filter factor
	source_32x4a = _mm_mul_ps(source_32x4a, filterFactor_32x4);
	source_32x4b = _mm_mul_ps(source_32x4b, filterFactor_32x4);

	// we add the local result to the sum parameters
	target_32x4a = _mm_add_ps(target_32x4a, source_32x4a);
	target_32x4b = _mm_add_ps(target_32x4b, source_32x4b);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::symmetricFilterHorizontalRowMultiplyOneFilterFactor8Elements<uint8_t, unsigned int, ProcessorInstructions::PI_SSE_2>(const uint8_t* sourceLeft, const uint8_t* sourceRight, const unsigned int& filterFactor, SIMD32x4<unsigned int>::Type& target_32x4a, SIMD32x4<unsigned int>::Type& target_32x4b)
{
	/**
	 * This function uses the following SSE instructions, and needs SSE2 or higher
	 *
	 * SSE2:
	 * _mm_set1_epi32
	 * _mm_loadl_epi64
	 * _mm_unpacklo_epi8
	 * _mm_unpackhi_epi16
	 * _mm_unpacklo_epi16
	 * _mm_setzero_si128
	 * _mm_madd_epi16
	 * _mm_add_epi32
	 */

	// we store one filter value in each of the four 32 bit integer values
	__m128i filterFactor_32x4 = _mm_set1_epi32(int(filterFactor));

	// we load 8 source values from the left side and 8 source values from the right side, we separate the values to receive 16 bit integers and add them together
	__m128i source_32x4a = _mm_add_epi16(_mm_unpacklo_epi8(_mm_loadl_epi64((const __m128i*)sourceLeft), _mm_setzero_si128()), _mm_unpacklo_epi8(_mm_loadl_epi64((const __m128i*)sourceRight), _mm_setzero_si128()));

	// we separate the 16 bit values further so that we receive 32 bit integers
	__m128i source_32x4b = _mm_unpackhi_epi16(source_32x4a, _mm_setzero_si128());
	source_32x4a = _mm_unpacklo_epi16(source_32x4a, _mm_setzero_si128());

	// we multiply each value with the same filter factor, and sum the result
	source_32x4a = _mm_madd_epi16(source_32x4a, filterFactor_32x4);
	source_32x4b = _mm_madd_epi16(source_32x4b, filterFactor_32x4);

	// we add the local result to the sum parameters
	target_32x4a = _mm_add_epi32(target_32x4a, source_32x4a);
	target_32x4b = _mm_add_epi32(target_32x4b, source_32x4b);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::symmetricFilterHorizontalRowMultiplyOneFilterFactor8Elements<float, float, ProcessorInstructions::PI_SSE_2>(const float* sourceLeft, const float* sourceRight, const float& filterFactor, SIMD32x4<float>::Type& target_32x4a, SIMD32x4<float>::Type& target_32x4b)
{
	/**
	 * This function uses the following SSE instructions, and needs SSE2 or higher
	 *
	 * SSE:
	 * _mm_set_ps1
	 * _mm_mul_ps
	 * _mm_add_ps
	 *
	 * SSE2:
	 * _mm_loadu_si128
	 * _mm_castsi128_ps
	 */

	// we store one filter value in each of the four 32 bit values
	__m128 filterFactor_32x4 = _mm_set_ps1(filterFactor);

	// we load 4 * 2  source values and add them together
	__m128 source_32x4a = _mm_add_ps(_mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourceLeft + 0)), _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourceRight + 0)));
	__m128 source_32x4b = _mm_add_ps(_mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourceLeft + 1)), _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourceRight + 1)));

	// we multiply each value with the same filter factor
	source_32x4a = _mm_mul_ps(source_32x4a, filterFactor_32x4);
	source_32x4b = _mm_mul_ps(source_32x4b, filterFactor_32x4);

	// we add the local result to the sum parameters
	target_32x4a = _mm_add_ps(target_32x4a, source_32x4a);
	target_32x4b = _mm_add_ps(target_32x4b, source_32x4b);
}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 20

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::asymmetricFilterHorizontalRowMultiplyOneFilterFactor4Elements<uint8_t, unsigned int, ProcessorInstructions::PI_NEON>(const uint8_t* source, const unsigned int& filterFactor, SIMD32x4<unsigned int>::Type& target_32x4)
{
	ocean_assert(filterFactor <= 0xFFFFu);

	// we store the same filter value in each of the four 16 bit values
	const uint16x4_t filterFactor16_4 = vdup_n_u16(uint16_t(filterFactor));

#if defined(__aarch64__)

	// we load four 8bit source values and we convert them to 16 bit values afterwards
	const uint16x8_t source16_8 = vmovl_u8(vreinterpret_u8_u32(vdup_n_u32(*((const uint32_t*)source))));

#else

	uint32_t sourceValue;
	((uint8_t*)&sourceValue)[0] = source[0];
	((uint8_t*)&sourceValue)[1] = source[1];
	((uint8_t*)&sourceValue)[2] = source[2];
	((uint8_t*)&sourceValue)[3] = source[3];

	const uint16x8_t source16_8 = vmovl_u8(vreinterpret_u8_u32(vdup_n_u32(sourceValue)));

#endif // __aarch64__

	// we multiply each 16 bit value with the same 16 bit filter factor and add the 32 bit results to the given values
	target_32x4 = vmlal_u16(target_32x4, vget_low_u16(source16_8), filterFactor16_4);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::asymmetricFilterHorizontalRowMultiplyOneFilterFactor4Elements<float, float, ProcessorInstructions::PI_NEON>(const float* source, const float& filterFactor, SIMD32x4<float>::Type& target_32x4)
{
	// we store the same filter value in each of the four 32 bit values
	const float32x4_t filterFactor_32x4 = vdupq_n_f32(filterFactor);

	// we load four 32 bit source values
	const float32x4_t source128 = vld1q_f32(source);

	// we multiply each value with the same filter factor, and sum the result
	target_32x4 = vmlaq_f32(target_32x4, source128, filterFactor_32x4);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::symmetricFilterHorizontalRowMultiplyOneFilterFactor4Elements<uint8_t, unsigned int, ProcessorInstructions::PI_NEON>(const uint8_t* sourceLeft, const uint8_t* sourceRight, const unsigned int& filterFactor, SIMD32x4<unsigned int>::Type& target_32x4)
{
	ocean_assert(filterFactor <= 0xFFFFu);

	// we store the same filter value in each of the four 16 bit values
	const uint16x4_t filterFactor16_4 = vdup_n_u16(uint16_t(filterFactor));

#if defined(__aarch64__)

	// we load eight 8bit source values and we convert them to 16 bit values afterwards
	const uint16x8_t source16_8 = vaddl_u8(vreinterpret_u8_u32(vdup_n_u32(*((const uint32_t*)sourceLeft))), vreinterpret_u8_u32(vdup_n_u32(*((const uint32_t*)sourceRight))));

#else

	uint32_t sourceValueLeft;
	((uint8_t*)&sourceValueLeft)[0] = sourceLeft[0];
	((uint8_t*)&sourceValueLeft)[1] = sourceLeft[1];
	((uint8_t*)&sourceValueLeft)[2] = sourceLeft[2];
	((uint8_t*)&sourceValueLeft)[3] = sourceLeft[3];

	uint32_t sourceValueRight;
	((uint8_t*)&sourceValueRight)[0] = sourceRight[0];
	((uint8_t*)&sourceValueRight)[1] = sourceRight[1];
	((uint8_t*)&sourceValueRight)[2] = sourceRight[2];
	((uint8_t*)&sourceValueRight)[3] = sourceRight[3];

	// we load eight 8bit source values and we convert them to 16 bit values afterwards
	const uint16x8_t source16_8 = vaddl_u8(vreinterpret_u8_u32(vdup_n_u32(sourceValueLeft)), vreinterpret_u8_u32(vdup_n_u32(sourceValueRight)));

#endif // __aarch64__

	// we multiply each 16 bit value with the same 16 bit filter factor and add the 32 bit results to the given values
	target_32x4 = vmlal_u16(target_32x4, vget_low_u16(source16_8), filterFactor16_4);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::symmetricFilterHorizontalRowMultiplyOneFilterFactor4Elements<float, float, ProcessorInstructions::PI_NEON>(const float* sourceLeft, const float* sourceRight, const float& filterFactor, SIMD32x4<float>::Type& target_32x4)
{
	// we store the same filter value in each of the four 32 bit values
	const float32x4_t filterFactor_32x4 = vdupq_n_f32(filterFactor);

	// we load eight 8bit source values and we convert them to 16 bit values afterwards
	const float32x4_t source_32x4 = vaddq_f32(vld1q_f32(sourceLeft), vld1q_f32(sourceRight));

	// we multiply each 16 bit value with the same 16 bit filter factor and add the 32 bit results to the given values
	target_32x4 = vmlaq_f32(target_32x4, source_32x4, filterFactor_32x4);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::asymmetricFilterHorizontalRowMultiplyOneFilterFactor8Elements<uint8_t, unsigned int, ProcessorInstructions::PI_NEON>(const uint8_t* source, const unsigned int& filterFactor, SIMD32x4<unsigned int>::Type& target_32x4a, SIMD32x4<unsigned int>::Type& target_32x4b)
{
	ocean_assert(filterFactor <= 0xFFFFu);

	// we store the same filter value in each of the four 16 bit values
	const uint16x4_t filterFactor16_4 = vdup_n_u16(uint16_t(filterFactor));

	// we load eight 8bit source values and we convert them to 16 bit values afterwards
	const uint16x8_t source16_8 = vmovl_u8(vld1_u8(source));

	// we multiply each 16 bit value with the same 16 bit filter factor and add the 32 bit results to the given values
	target_32x4a = vmlal_u16(target_32x4a, vget_low_u16(source16_8), filterFactor16_4);
	target_32x4b = vmlal_u16(target_32x4b, vget_high_u16(source16_8), filterFactor16_4);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::asymmetricFilterHorizontalRowMultiplyOneFilterFactor8Elements<float, float, ProcessorInstructions::PI_NEON>(const float* source, const float& filterFactor, SIMD32x4<float>::Type& target_32x4a, SIMD32x4<float>::Type& target_32x4b)
{
	// we store the same filter value in each of the four 32 bit values
	const float32x4_t filterFactor_32x4 = vdupq_n_f32(filterFactor);

	// we load eight 32 bit source values
	const float32x4_t source_32x4a = vld1q_f32(source + 0);
	const float32x4_t source_32x4b = vld1q_f32(source + 4);

	// we multiply each value with the same filter factor, and sum the result
	target_32x4a = vmlaq_f32(target_32x4a, source_32x4a, filterFactor_32x4);
	target_32x4b = vmlaq_f32(target_32x4b, source_32x4b, filterFactor_32x4);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::symmetricFilterHorizontalRowMultiplyOneFilterFactor8Elements<uint8_t, unsigned int, ProcessorInstructions::PI_NEON>(const uint8_t* sourceLeft, const uint8_t* sourceRight, const unsigned int& filterFactor, SIMD32x4<unsigned int>::Type& target_32x4a, SIMD32x4<unsigned int>::Type& target_32x4b)
{
	ocean_assert(filterFactor <= 0xFFFFu);

	// we store the same filter value in each of the four 16 bit values
	const uint16x4_t filterFactor16_4 = vdup_n_u16(uint16_t(filterFactor));

	// we load eight 8bit source values and we convert them to 16 bit values afterwards
	const uint16x8_t source16_8 = vaddl_u8(vld1_u8(sourceLeft), vld1_u8(sourceRight));

	// we multiply each 16 bit value with the same 16 bit filter factor and add the 32 bit results to the given values
	target_32x4a = vmlal_u16(target_32x4a, vget_low_u16(source16_8), filterFactor16_4);
	target_32x4b = vmlal_u16(target_32x4b, vget_high_u16(source16_8), filterFactor16_4);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::symmetricFilterHorizontalRowMultiplyOneFilterFactor8Elements<float, float, ProcessorInstructions::PI_NEON>(const float* sourceLeft, const float* sourceRight, const float& filterFactor, SIMD32x4<float>::Type& target_32x4a, SIMD32x4<float>::Type& target_32x4b)
{
	// we store the same filter value in each of the four 16 bit values
	const float32x4_t filterFactor_32x4 = vdupq_n_f32(filterFactor);

	// we load eight 32 bit source values
	const float32x4_t source_32x4a = vaddq_f32(vld1q_f32(sourceLeft + 0), vld1q_f32(sourceRight + 0));
	const float32x4_t source_32x4b = vaddq_f32(vld1q_f32(sourceLeft + 4), vld1q_f32(sourceRight + 4));

	// we multiply each value with the same filter factor, and sum the result
	target_32x4a = vmlaq_f32(target_32x4a, source_32x4a, filterFactor_32x4);
	target_32x4b = vmlaq_f32(target_32x4b, source_32x4b, filterFactor_32x4);
}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

template <typename TSource, typename TFilter, ProcessorInstructions tProcessorInstructions>
OCEAN_FORCE_INLINE void FrameFilterSeparable::filterHorizontalRowOneBlockWith4Elements(const TSource* const source, TFilter* const target, const unsigned int channels, const TFilter* const filter, const unsigned int filterSize, const bool isSymmetric)
{
	/*
	 * We determine 4 filter responses within one loop iteration.
	 * For a filter with size 5 for 1 channel frames we apply the following strategy:
	 *
	 * Source Data: Y Y Y Y Y Y Y Y Y  (if the source data has a Y8 pixel format)
	 *              1 4 6 4 1 .
	 *                1 4 6 4 1
	 *                  1 4 6 4 1
	 *                  . 1 4 6 4 1
	 *                  .     .
	 * Target Data: - - Y Y Y Y - -
	 *
	 *
	 * For a filter with size 5 for 3 channel frames we apply the following strategy:
	 *
	 * Source Data: R G B R G B R G B R G B R G B R G B R G B R G B (if the source data has a RGB24 pixel format)
	 *              1 1 1 4 4 4 6 6 6 4 4 4 1 1 1
	 *                    1 1 1 4 4 4 6 6 6 4 4 4 1 1 1
	 *                          1 1 1 4 4 4 6 6 6 4 4 4 1 1 1
	 *                          .     1 1 1 4 4 4 6 6 6 4 4 4 1 1 1
	 *                          .     .     1 1 1 4 4 4 6 6 6 4 4 4 1 1 1
	 *                          .     .
	 * Target Data: - - - - - - R G B R - - - - - - - - - - - - - - - -
	 *
	 */

	ocean_assert(source != nullptr && filter != nullptr);
	ocean_assert(channels >= 1u);
	ocean_assert(filterSize >= 1u && (filterSize % 2u) == 1u);

	typename SIMD32x4<TFilter>::Type target_32x4;

	setSIMDZero<TFilter, tProcessorInstructions>(target_32x4);

	if (isSymmetric)
	{
		const unsigned int filterSize_2 = filterSize / 2u;

		// we iterate over the first half of filter factors [0, filterSize_2)
		for (unsigned int n = 0u; n < filterSize_2; ++n)
		{
			symmetricFilterHorizontalRowMultiplyOneFilterFactor4Elements<TSource, TFilter, tProcessorInstructions>(source + n * channels, source + (filterSize - n - 1) * channels, filter[n], target_32x4);
		}

		// we handle the center filter factor at filterSize_2
		asymmetricFilterHorizontalRowMultiplyOneFilterFactor4Elements<TSource, TFilter, tProcessorInstructions>(source + filterSize_2 * channels, filter[filterSize_2], target_32x4);
	}
	else
	{
		// we iterate over the first half of filter factors [0, filterSize_2)
		for (unsigned int n = 0u; n < filterSize; ++n)
		{
			asymmetricFilterHorizontalRowMultiplyOneFilterFactor4Elements<TSource, TFilter, tProcessorInstructions>(source + n * channels, filter[n], target_32x4);
		}
	}

	writeSIMD<TFilter, tProcessorInstructions>(target_32x4, target);
}

template <typename TSource, typename TFilter, ProcessorInstructions tProcessorInstructions>
OCEAN_FORCE_INLINE void FrameFilterSeparable::filterHorizontalRowOneBlockWith8Elements(const TSource* const source, TFilter* const target, const unsigned int channels, const TFilter* const filter, const unsigned int filterSize, const bool isSymmetric)
{
	/*
	 * We determine 8 filter responses within one loop iteration.
	 * For a filter with size 5 for 1 channel frames we apply the following strategy:
	 *
	 * Source Data: Y Y Y Y Y Y Y Y Y Y Y Y  (if the source data has a Y8 pixel format)
	 *              1 4 6 4 1         .
	 *                1 4 6 4 1       .
	 *                  1 4 6 4 1     .
	 *                  . 1 4 6 4 1   .
	 *                  .   1 4 6 4 1 .
	 *                  .     1 4 6 4 1
	 *                  .       1 4 6 4 1
	 *                  .         1 4 6 4 1
	 *                  .             .
	 * Target Data: - - Y Y Y Y Y Y Y Y - -
	 *
	 *
	 * For a filter with size 5 for 3 channel frames we apply the following strategy:
	 *
	 * Source Data: R G B R G B R G B R G B R G B R G B R G B R G B (if the source data has a RGB24 pixel format)
	 *              1 1 1 4 4 4 6 6 6 4 4 4 1 1 1
	 *                    1 1 1 4 4 4 6 6 6 4 4 4 1 1 1
	 *                          1 1 1 4 4 4 6 6 6 4 4 4 1 1 1
	 *                          .     1 1 1 4 4 4 6 6 6 4 4 4 1 1 1
	 *                          .           1 1 1 4 4 4 6 6 6 4 4 4 1 1 1
	 *                          .             .   1 1 1 4 4 4 6 6 6 4 4 4 1 1 1
	 *                          .             .
	 * Target Data: - - - - - - R G B R G B R G - - - - - - - - - - - - - - - -
	 *
	 */

	ocean_assert(source != nullptr && filter != nullptr);
	ocean_assert(channels >= 1u);
	ocean_assert(filterSize >= 1u && (filterSize % 2u) == 1u);

	typename SIMD32x4<TFilter>::Type target_32x4a, target_32x4b;

	setSIMDZero<TFilter, tProcessorInstructions>(target_32x4a);
	setSIMDZero<TFilter, tProcessorInstructions>(target_32x4b);

	if (isSymmetric)
	{
		const unsigned int filterSize_2 = filterSize / 2u;

		// we iterate over the first half of filter factors [0, filterSize_2)
		for (unsigned int n = 0u; n < filterSize_2; ++n)
		{
			symmetricFilterHorizontalRowMultiplyOneFilterFactor8Elements<TSource, TFilter, tProcessorInstructions>(source + n * channels, source + (filterSize - n - 1) * channels, filter[n], target_32x4a, target_32x4b);
		}

		// we handle the center filter factor at filterSize_2
		asymmetricFilterHorizontalRowMultiplyOneFilterFactor8Elements<TSource, TFilter, tProcessorInstructions>(source + filterSize_2 * channels, filter[filterSize_2], target_32x4a, target_32x4b);
	}
	else
	{
		// we iterate over the first half of filter factors [0, filterSize_2)
		for (unsigned int n = 0u; n < filterSize; ++n)
		{
			asymmetricFilterHorizontalRowMultiplyOneFilterFactor8Elements<TSource, TFilter, tProcessorInstructions>(source + n * channels, filter[n], target_32x4a, target_32x4b);
		}
	}

	writeSIMD<TFilter, tProcessorInstructions>(target_32x4a, target + 0);
	writeSIMD<TFilter, tProcessorInstructions>(target_32x4b, target + 4);
}

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 20

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::filterVerticalBorderRow16Elements32BitPerChannelFloat<unsigned int, uint8_t, PI_SSE_2>(const unsigned int* source, uint8_t* target, const unsigned int sourceStrideElements, const unsigned int height, const unsigned int row, const float* filter, const unsigned int filterSize, const bool isSymmetric)
{
	ocean_assert(source != nullptr && target != nullptr && filter != nullptr);
	ocean_assert(filterSize >= 1u && (filterSize % 2u) == 1u);

	/*
	 * This function uses the following SSE instructions, and needs SSE2 or higher
	 *
	 * SSE1:
	 * _mm_set_ps1
	 * _mm_mul_ps
	 * _mm_add_ps
	 * _mm_loadu_ps
	 *
	 * SSE2:
	 * _mm_loadu_si128
	 * _mm_cvtepi32_ps
	 * _mm_add_epi32
	 * _mm_cvtps_epi32
	 * _mm_packs_epi32
	 * _mm_packus_epi16
	 * _mm_storeu_si128
	 */

	/*
	 * We determine 16 filter responses within one loop iteration.
	 * For a filter with size 5 for 1 channel frames, with row = 0, we apply the following mirroring strategy:
	 *
	 * Source Data:
	 *     1  Y 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
	 *     0  Y 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4
	 *        ---------------------------------
	 *     0  Y 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 <---------
	 *     1  Y 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4
	 *     2  Y 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
	 *     3  Y
	 *     4  Y
	 *
	 * For frames with n channels the strategy stays the same.
	 */

	const unsigned int filterSize_2 = filterSize / 2u;

	// the border covers row ids within the range [0, filterSize_2)
	ocean_assert(row < filterSize_2 || row + filterSize_2 >= height);

	const __m128i* sourceBlock = (const __m128i*)source;

	// we store one filter value in each of the four 32 bit integer values
	__m128 filterFactor_32x4 = _mm_set_ps1(filter[filterSize_2]);

	// now we load four input values, and multiply each of them with the center kernel value
	__m128 source_32x4a = _mm_cvtepi32_ps(_mm_loadu_si128(sourceBlock + 0));
	__m128 result_32x4a = _mm_mul_ps(source_32x4a, filterFactor_32x4);

	// now we load the next four input values, ...
	__m128 source_32x4b = _mm_cvtepi32_ps(_mm_loadu_si128(sourceBlock + 1));
	__m128 result_32x4b = _mm_mul_ps(source_32x4b, filterFactor_32x4);

	__m128 source_32x4c = _mm_cvtepi32_ps(_mm_loadu_si128(sourceBlock + 2));
	__m128 result_32x4c = _mm_mul_ps(source_32x4c, filterFactor_32x4);

	__m128 source_32x4d = _mm_cvtepi32_ps(_mm_loadu_si128(sourceBlock + 3));
	__m128 result_32x4d = _mm_mul_ps(source_32x4d, filterFactor_32x4);

	__m128i source128ai, source128bi;

	// now we proceed with the remaining filter values
	for (unsigned int i = 1u; i <= filterSize_2; ++i)
	{
		// we determine the mirrored locations (and the row offset in relation to the current row)
		const int offsetMinus = int(mirroredBorderLocationLeft(int(row) - int(i))) - int(row);
		const int offsetPlus = int(mirroredBorderLocationRight(row + i, height)) - int(row);

		// depending on whether we are at the top border or at the bottom border we change the minus and plus source values
		const unsigned int* sourceMinus = source + offsetMinus * int(sourceStrideElements);
		const unsigned int* sourcePlus = source + offsetPlus * int(sourceStrideElements);

		if (isSymmetric)
		{
			// we have a symmetric filter, so let's do some optimizations
			filterFactor_32x4 = _mm_set_ps1(filter[filterSize_2 + i]);

			// we sum the values of the upper and the lower row (as both will be multiplied with the same filter value)

			source128ai = _mm_add_epi32(_mm_loadu_si128((__m128i*)sourceMinus + 0), _mm_loadu_si128((__m128i*)sourcePlus + 0));
			source128bi = _mm_add_epi32(_mm_loadu_si128((__m128i*)sourceMinus + 1), _mm_loadu_si128((__m128i*)sourcePlus + 1));

			result_32x4a = _mm_add_ps(result_32x4a, _mm_mul_ps(_mm_cvtepi32_ps(source128ai), filterFactor_32x4));
			result_32x4b = _mm_add_ps(result_32x4b, _mm_mul_ps(_mm_cvtepi32_ps(source128bi), filterFactor_32x4));

			source128ai = _mm_add_epi32(_mm_loadu_si128((__m128i*)sourceMinus + 2), _mm_loadu_si128((__m128i*)sourcePlus + 2));
			source128bi = _mm_add_epi32(_mm_loadu_si128((__m128i*)sourceMinus + 3), _mm_loadu_si128((__m128i*)sourcePlus + 3));

			result_32x4c = _mm_add_ps(result_32x4c, _mm_mul_ps(_mm_cvtepi32_ps(source128ai), filterFactor_32x4));
			result_32x4d = _mm_add_ps(result_32x4d, _mm_mul_ps(_mm_cvtepi32_ps(source128bi), filterFactor_32x4));
		}
		else
		{
			// we don't have a symmetric filter, so we need to handle two individual filters
			__m128 filterFactor128Minus = _mm_set_ps1(filter[filterSize_2 - i]);
			__m128 filterFactor128Plus = _mm_set_ps1(filter[filterSize_2 + i]);

			__m128i source128aiMinus = _mm_loadu_si128((__m128i*)sourceMinus + 0);
			__m128i source128aiPlus = _mm_loadu_si128((__m128i*)sourcePlus + 0);

			__m128i source128biMinus = _mm_loadu_si128((__m128i*)sourceMinus + 1);
			__m128i source128biPlus = _mm_loadu_si128((__m128i*)sourcePlus + 1);

			result_32x4a = _mm_add_ps(result_32x4a, _mm_mul_ps(_mm_cvtepi32_ps(source128aiMinus), filterFactor128Minus));
			result_32x4a = _mm_add_ps(result_32x4a, _mm_mul_ps(_mm_cvtepi32_ps(source128aiPlus), filterFactor128Plus));

			result_32x4b = _mm_add_ps(result_32x4b, _mm_mul_ps(_mm_cvtepi32_ps(source128biMinus), filterFactor128Minus));
			result_32x4b = _mm_add_ps(result_32x4b, _mm_mul_ps(_mm_cvtepi32_ps(source128biPlus), filterFactor128Plus));

			__m128i source128ciMinus = _mm_loadu_si128((__m128i*)sourceMinus + 2);
			__m128i source128ciPlus = _mm_loadu_si128((__m128i*)sourcePlus + 2);

			__m128i source128diMinus = _mm_loadu_si128((__m128i*)sourceMinus + 3);
			__m128i source128diPlus = _mm_loadu_si128((__m128i*)sourcePlus + 3);

			result_32x4c = _mm_add_ps(result_32x4c, _mm_mul_ps(_mm_cvtepi32_ps(source128ciMinus), filterFactor128Minus));
			result_32x4c = _mm_add_ps(result_32x4c, _mm_mul_ps(_mm_cvtepi32_ps(source128ciPlus), filterFactor128Plus));

			result_32x4d = _mm_add_ps(result_32x4d, _mm_mul_ps(_mm_cvtepi32_ps(source128diMinus), filterFactor128Minus));
			result_32x4d = _mm_add_ps(result_32x4d, _mm_mul_ps(_mm_cvtepi32_ps(source128diPlus), filterFactor128Plus));
		}
	}

	// now we have 8 bit values in each 32 bit register
	__m128i result128ab = _mm_packs_epi32(_mm_cvtps_epi32(result_32x4a), _mm_cvtps_epi32(result_32x4b));
	__m128i result128cd = _mm_packs_epi32(_mm_cvtps_epi32(result_32x4c), _mm_cvtps_epi32(result_32x4d));
	__m128i result128 = _mm_packus_epi16(result128ab, result128cd);

	_mm_storeu_si128((__m128i*)target, result128);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::filterVerticalBorderRow16Elements32BitPerChannelFloat<float, float, PI_SSE_2>(const float* source, float* target, const unsigned int sourceStrideElements, const unsigned int height, const unsigned int row, const float* filter, const unsigned int filterSize, const bool isSymmetric)
{
	ocean_assert(source != nullptr && target != nullptr && filter != nullptr);
	ocean_assert(filterSize >= 1u && (filterSize % 2u) == 1u);

	/**
	 * This function uses the following SSE instructions, and needs SSE2 or higher
	 *
	 * SSE:
	 * _mm_set_ps1
	 * _mm_mul_ps
	 * _mm_add_ps
	 *
	 * SSE2:
	 * _mm_loadu_si128
	 * _mm_castsi128_ps
	 */

	/*
	 * We determine 16 filter responses within one loop iteration.
	 * For a filter with size 5 for 1 channel frames, with row = 0, we apply the following mirroring strategy:
	 *
	 * Source Data:
	 *     1  Y 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
	 *     0  Y 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4
	 *        ---------------------------------
	 *     0  Y 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 <---------
	 *     1  Y 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4
	 *     2  Y 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
	 *     3  Y
	 *     4  Y
	 *
	 * For frames with n channels the strategy stays the same.
	 */

	const unsigned int filterSize_2 = filterSize / 2u;

	// the border covers row ids within the range [0, filterSize_2)
	ocean_assert(row < filterSize_2 || row + filterSize_2 >= height);

	const __m128i* sourceBlock = (const __m128i*)source;

	// we store one filter value in each of the four 32 bit values
	__m128 filterFactor_32x4 = _mm_set_ps1(filter[filterSize_2]);

	// now we load four input values, and multiply each of them with the center kernel value
	__m128 source_32x4a = _mm_castsi128_ps(_mm_loadu_si128(sourceBlock + 0));
	__m128 result_32x4a = _mm_mul_ps(source_32x4a, filterFactor_32x4);

	// now we load the next four input values, ...
	__m128 source_32x4b = _mm_castsi128_ps(_mm_loadu_si128(sourceBlock + 1));
	__m128 result_32x4b = _mm_mul_ps(source_32x4b, filterFactor_32x4);

	__m128 source_32x4c = _mm_castsi128_ps(_mm_loadu_si128(sourceBlock + 2));
	__m128 result_32x4c = _mm_mul_ps(source_32x4c, filterFactor_32x4);

	__m128 source_32x4d = _mm_castsi128_ps(_mm_loadu_si128(sourceBlock + 3));
	__m128 result_32x4d = _mm_mul_ps(source_32x4d, filterFactor_32x4);

	// now we proceed with the remaining filter values
	for (unsigned int i = 1u; i <= filterSize_2; ++i)
	{
		// we determine the mirrored locations (and the row offset in relation to the current row)
		const int offsetMinus = int(mirroredBorderLocationLeft(int(row) - int(i))) - int(row);
		const int offsetPlus = int(mirroredBorderLocationRight(row + i, height)) - int(row);

		// depending on whether we are at the top border or at the bottom border we change the minus and plus source values
		const float* sourceMinus = source + offsetMinus * int(sourceStrideElements);
		const float* sourcePlus = source + offsetPlus * int(sourceStrideElements);

		if (isSymmetric)
		{
			// we have a symmetric filter, so let's do some optimizations
			filterFactor_32x4 = _mm_set_ps1(filter[filterSize_2 + i]);

			// we sum the values of the upper and the lower row (as both will be multiplied with the same filter value)

			source_32x4a = _mm_add_ps(_mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourceMinus + 0)), _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourcePlus + 0)));
			source_32x4b = _mm_add_ps(_mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourceMinus + 1)), _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourcePlus + 1)));

			result_32x4a = _mm_add_ps(result_32x4a, _mm_mul_ps(source_32x4a, filterFactor_32x4));
			result_32x4b = _mm_add_ps(result_32x4b, _mm_mul_ps(source_32x4b, filterFactor_32x4));

			source_32x4c = _mm_add_ps(_mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourceMinus + 2)), _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourcePlus + 2)));
			source_32x4d = _mm_add_ps(_mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourceMinus + 3)), _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourcePlus + 3)));

			result_32x4c = _mm_add_ps(result_32x4c, _mm_mul_ps(source_32x4c, filterFactor_32x4));
			result_32x4d = _mm_add_ps(result_32x4d, _mm_mul_ps(source_32x4d, filterFactor_32x4));
		}
		else
		{
			// we don't have a symmetric filter, so we need to handle two individual filters
			__m128 filterFactor_32x4Minus = _mm_set_ps1(filter[filterSize_2 - i]);
			__m128 filterFactor_32x4Plus = _mm_set_ps1(filter[filterSize_2 + i]);

			source_32x4a = _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourceMinus + 0));
			result_32x4a = _mm_add_ps(result_32x4a, _mm_mul_ps(source_32x4a, filterFactor_32x4Minus));

			source_32x4b = _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourceMinus + 1));
			result_32x4b = _mm_add_ps(result_32x4b, _mm_mul_ps(source_32x4b, filterFactor_32x4Minus));

			source_32x4c = _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourceMinus + 2));
			result_32x4c = _mm_add_ps(result_32x4c, _mm_mul_ps(source_32x4c, filterFactor_32x4Minus));

			source_32x4d = _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourceMinus + 3));
			result_32x4d = _mm_add_ps(result_32x4d, _mm_mul_ps(source_32x4d, filterFactor_32x4Minus));

			source_32x4a = _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourcePlus + 0));
			result_32x4a = _mm_add_ps(result_32x4a, _mm_mul_ps(source_32x4a, filterFactor_32x4Plus));

			source_32x4b = _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourcePlus + 1));
			result_32x4b = _mm_add_ps(result_32x4b, _mm_mul_ps(source_32x4b, filterFactor_32x4Plus));

			source_32x4c = _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourcePlus + 2));
			result_32x4c = _mm_add_ps(result_32x4c, _mm_mul_ps(source_32x4c, filterFactor_32x4Plus));

			source_32x4d = _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)sourcePlus + 3));
			result_32x4d = _mm_add_ps(result_32x4d, _mm_mul_ps(source_32x4d, filterFactor_32x4Plus));
		}
	}

	writeSIMD<float, PI_SSE_2>(result_32x4a, target +  0);
	writeSIMD<float, PI_SSE_2>(result_32x4b, target +  4);
	writeSIMD<float, PI_SSE_2>(result_32x4c, target +  8);
	writeSIMD<float, PI_SSE_2>(result_32x4d, target + 12);
}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 20

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::filterVerticalBorderRow16Elements32BitPerChannelFloat<unsigned int, uint8_t, PI_NEON>(const unsigned int* source, uint8_t* target, const unsigned int sourceStrideElements, const unsigned int height, const unsigned int row, const float* filter, const unsigned int filterSize, const bool isSymmetric)
{
	ocean_assert(source != nullptr && target != nullptr && filter != nullptr);
	ocean_assert(filterSize >= 1u && (filterSize % 2u) == 1u);

	/*
	 * We determine 16 filter responses within one loop iteration.
	 * For a filter with size 5 for 1 channel frames, with row = 0, we apply the following mirroring strategy:
	 *
	 * Source Data:
	 *     1  Y 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
	 *     0  Y 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4
	 *        ---------------------------------
	 *     0  Y 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 <---------
	 *     1  Y 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4
	 *     2  Y 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
	 *     3  Y
	 *     4  Y
	 *
	 * For frames with n channels the strategy stays the same.
	 */

	const unsigned int filterSize_2 = filterSize / 2u;

	// the border covers row ids within the range [0, filterSize_2)
	ocean_assert(row < filterSize_2 || row + filterSize_2 >= height);

	// we store one filter value in each of the four 32 bit integer values
	float32x4_t filterFactor_32x4 = vdupq_n_f32(filter[filterSize_2]);

	// now we load four input values, and multiply each of them with the center kernel value
	float32x4_t source_32x4a = vcvtq_f32_u32(vld1q_u32(source + 4 * 0));
	float32x4_t result_32x4a = vmulq_f32(source_32x4a, filterFactor_32x4);

	float32x4_t source_32x4b = vcvtq_f32_u32(vld1q_u32(source + 4 * 1));
	float32x4_t result_32x4b = vmulq_f32(source_32x4b, filterFactor_32x4);

	float32x4_t source_32x4c = vcvtq_f32_u32(vld1q_u32(source + 4 * 2));
	float32x4_t result_32x4c = vmulq_f32(source_32x4c, filterFactor_32x4);

	float32x4_t source_32x4d = vcvtq_f32_u32(vld1q_u32(source + 4 * 3));
	float32x4_t result_32x4d = vmulq_f32(source_32x4d, filterFactor_32x4);

	// now we proceed with the remaining filter values
	for (unsigned int i = 1u; i <= filterSize_2; ++i)
	{
		// we determine the mirrored locations (and the row offset in relation to the current row)
		const int offsetMinus = int(mirroredBorderLocationLeft(int(row) - int(i))) - int(row);
		const int offsetPlus = int(mirroredBorderLocationRight(row + i, height)) - int(row);

		// depending on whether we are at the top border or at the bottom border we change the minus and plus source values
		const unsigned int* sourceMinus = source + offsetMinus * int(sourceStrideElements);
		const unsigned int* sourcePlus = source + offsetPlus * int(sourceStrideElements);

		if (isSymmetric)
		{
			// we have a symmetric filter, so let's do some optimizations
			filterFactor_32x4 = vdupq_n_f32(filter[filterSize_2 + i]);

			// we sum the values of the upper and the lower row (as both will be multiplied with the same filter value)

			uint32x4_t source128ai = vaddq_u32(vld1q_u32(sourceMinus + 4 * 0), vld1q_u32(sourcePlus + 4 * 0));
			uint32x4_t source128bi = vaddq_u32(vld1q_u32(sourceMinus + 4 * 1), vld1q_u32(sourcePlus + 4 * 1));

			result_32x4a = vmlaq_f32(result_32x4a, vcvtq_f32_u32(source128ai), filterFactor_32x4);
			result_32x4b = vmlaq_f32(result_32x4b, vcvtq_f32_u32(source128bi), filterFactor_32x4);

			source128ai = vaddq_u32(vld1q_u32(sourceMinus + 4 * 2), vld1q_u32(sourcePlus + 4 * 2));
			source128bi = vaddq_u32(vld1q_u32(sourceMinus + 4 * 3), vld1q_u32(sourcePlus + 4 * 3));

			result_32x4c = vmlaq_f32(result_32x4c, vcvtq_f32_u32(source128ai), filterFactor_32x4);
			result_32x4d = vmlaq_f32(result_32x4d, vcvtq_f32_u32(source128bi), filterFactor_32x4);
		}
		else
		{
			// we don't have a symmetric filter, so we need to handle two individual filters

			float32x4_t filterFactor128Minus = vdupq_n_f32(filter[filterSize_2 - i]);
			float32x4_t filterFactor128Plus = vdupq_n_f32(filter[filterSize_2 + i]);

			uint32x4_t source128aiMinus = vld1q_u32(sourceMinus + 4 * 0);
			result_32x4a = vmlaq_f32(result_32x4a, vcvtq_f32_u32(source128aiMinus), filterFactor128Minus);

			uint32x4_t source128biMinus = vld1q_u32(sourceMinus + 4 * 1);
			result_32x4b = vmlaq_f32(result_32x4b, vcvtq_f32_u32(source128biMinus), filterFactor128Minus);

			uint32x4_t source128aiPlus = vld1q_u32(sourcePlus + 4 * 0);
			result_32x4a = vmlaq_f32(result_32x4a, vcvtq_f32_u32(source128aiPlus), filterFactor128Plus);

			uint32x4_t source128biPlus = vld1q_u32(sourcePlus + 4 * 1);
			result_32x4b = vmlaq_f32(result_32x4b, vcvtq_f32_u32(source128biPlus), filterFactor128Plus);

			uint32x4_t source128ciMinus = vld1q_u32(sourceMinus + 4 * 2);
			result_32x4c = vmlaq_f32(result_32x4c, vcvtq_f32_u32(source128ciMinus), filterFactor128Minus);

			uint32x4_t source128diMinus = vld1q_u32(sourceMinus + 4 * 3);
			result_32x4d = vmlaq_f32(result_32x4d, vcvtq_f32_u32(source128diMinus), filterFactor128Minus);

			uint32x4_t source128ciPlus = vld1q_u32(sourcePlus + 4 * 2);
			result_32x4c = vmlaq_f32(result_32x4c, vcvtq_f32_u32(source128ciPlus), filterFactor128Plus);

			uint32x4_t source128diPlus = vld1q_u32(sourcePlus + 4 * 3);
			result_32x4d = vmlaq_f32(result_32x4d, vcvtq_f32_u32(source128diPlus), filterFactor128Plus);
		}
	}

	// now we have 8 bit values in each 32 bit register
	uint16x8_t result128ab = vcombine_u16(vqmovn_u32(vcvtq_u32_f32(result_32x4a)), vqmovn_u32(vcvtq_u32_f32(result_32x4b)));
	uint16x8_t result128cd = vcombine_u16(vqmovn_u32(vcvtq_u32_f32(result_32x4c)), vqmovn_u32(vcvtq_u32_f32(result_32x4d)));

	uint8x16_t result128 = vcombine_u8(vqmovn_u16(result128ab), vqmovn_u16(result128cd));

	vst1q_u8(target, result128);
}

template <>
OCEAN_FORCE_INLINE void FrameFilterSeparable::filterVerticalBorderRow16Elements32BitPerChannelFloat<float, float, PI_NEON>(const float* source, float* target, const unsigned int sourceStrideElements, const unsigned int height, const unsigned int row, const float* filter, const unsigned int filterSize, const bool isSymmetric)
{
	ocean_assert(source != nullptr && target != nullptr && filter != nullptr);
	ocean_assert(filterSize >= 1u && (filterSize % 2u) == 1u);

	/*
	 * We determine 16 filter responses within one loop iteration.
	 * For a filter with size 5 for 1 channel frames, with row = 0, we apply the following mirroring strategy:
	 *
	 * Source Data:
	 *     1  Y 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
	 *     0  Y 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4
	 *        ---------------------------------
	 *     0  Y 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 <---------
	 *     1  Y 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4
	 *     2  Y 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
	 *     3  Y
	 *     4  Y
	 *
	 * For frames with n channels the strategy stays the same.
	 */

	const unsigned int filterSize_2 = filterSize / 2u;

	// the border covers row ids within the range [0, filterSize_2)
	ocean_assert(row < filterSize_2 || row + filterSize_2 >= height);

	// we store one filter value in each of the four 32 bit integer values
	float32x4_t filterFactor_32x4 = vdupq_n_f32(filter[filterSize_2]);

	// now we load four input values, and multiply each of them with the center kernel value
	float32x4_t source_32x4a = vld1q_f32(source + 4 * 0);
	float32x4_t result_32x4a = vmulq_f32(source_32x4a, filterFactor_32x4);

	float32x4_t source_32x4b = vld1q_f32(source + 4 * 1);
	float32x4_t result_32x4b = vmulq_f32(source_32x4b, filterFactor_32x4);

	float32x4_t source_32x4c = vld1q_f32(source + 4 * 2);
	float32x4_t result_32x4c = vmulq_f32(source_32x4c, filterFactor_32x4);

	float32x4_t source_32x4d = vld1q_f32(source + 4 * 3);
	float32x4_t result_32x4d = vmulq_f32(source_32x4d, filterFactor_32x4);

	// now we proceed with the remaining filter values
	for (unsigned int i = 1u; i <= filterSize_2; ++i)
	{
		// we determine the mirrored locations (and the row offset in relation to the current row)
		const int offsetMinus = int(mirroredBorderLocationLeft(int(row) - int(i))) - int(row);
		const int offsetPlus = int(mirroredBorderLocationRight(row + i, height)) - int(row);

		// depending on whether we are at the top border or at the bottom border we change the minus and plus source values
		const float* sourceMinus = source + offsetMinus * int(sourceStrideElements);
		const float* sourcePlus = source + offsetPlus * int(sourceStrideElements);

		if (isSymmetric)
		{
			// we have a symmetric filter, so let's do some optimizations
			filterFactor_32x4 = vdupq_n_f32(filter[filterSize_2 + i]);

			// we sum the values of the upper and the lower row (as both will be multiplied with the same filter value)

			source_32x4a = vaddq_f32(vld1q_f32(sourceMinus + 4 * 0), vld1q_f32(sourcePlus + 4 * 0));
			source_32x4b = vaddq_f32(vld1q_f32(sourceMinus + 4 * 1), vld1q_f32(sourcePlus + 4 * 1));

			result_32x4a = vmlaq_f32(result_32x4a, source_32x4a, filterFactor_32x4);
			result_32x4b = vmlaq_f32(result_32x4b, source_32x4b, filterFactor_32x4);

			source_32x4a = vaddq_f32(vld1q_f32(sourceMinus + 4 * 2), vld1q_f32(sourcePlus + 4 * 2));
			source_32x4b = vaddq_f32(vld1q_f32(sourceMinus + 4 * 3), vld1q_f32(sourcePlus + 4 * 3));

			result_32x4c = vmlaq_f32(result_32x4c, source_32x4a, filterFactor_32x4);
			result_32x4d = vmlaq_f32(result_32x4d, source_32x4b, filterFactor_32x4);
		}
		else
		{
			// we don't have a symmetric filter, so we need to handle two individual filters

			float32x4_t filterFactor128Minus = vdupq_n_f32(filter[filterSize_2 - i]);
			float32x4_t filterFactor128Plus = vdupq_n_f32(filter[filterSize_2 + i]);

			float32x4_t source128aMinus = vld1q_f32(sourceMinus + 4 * 0);
			float32x4_t source128aPlus = vld1q_f32(sourcePlus + 4 * 0);

			float32x4_t source128bMinus = vld1q_f32(sourceMinus + 4 * 1);
			float32x4_t source128bPlus = vld1q_f32(sourcePlus + 4 * 1);

			result_32x4a = vmlaq_f32(result_32x4a, source128aMinus, filterFactor128Minus);
			result_32x4b = vmlaq_f32(result_32x4b, source128bMinus, filterFactor128Minus);

			result_32x4a = vmlaq_f32(result_32x4a, source128aPlus, filterFactor128Plus);
			result_32x4b = vmlaq_f32(result_32x4b, source128bPlus, filterFactor128Plus);

			source128aMinus = vld1q_f32(sourceMinus + 4 * 2);
			source128aPlus = vld1q_f32(sourcePlus + 4 * 2);

			source128bMinus = vld1q_f32(sourceMinus + 4 * 3);
			source128bPlus = vld1q_f32(sourcePlus + 4 * 3);

			result_32x4c = vmlaq_f32(result_32x4c, source128aMinus, filterFactor128Minus);
			result_32x4d = vmlaq_f32(result_32x4d, source128bMinus, filterFactor128Minus);

			result_32x4c = vmlaq_f32(result_32x4c, source128aPlus, filterFactor128Plus);
			result_32x4d = vmlaq_f32(result_32x4d, source128bPlus, filterFactor128Plus);
		}
	}

	vst1q_f32(target +  0, result_32x4a);
	vst1q_f32(target +  4, result_32x4b);
	vst1q_f32(target +  8, result_32x4c);
	vst1q_f32(target + 12, result_32x4d);
}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

template <typename TSource, typename TTarget, ProcessorInstructions tProcessorInstructions>
OCEAN_FORCE_INLINE void FrameFilterSeparable::filterVerticalBorderRow32BitPerChannelFloat(const TSource* source, TTarget* target, const unsigned int width, const unsigned height, const unsigned int channels, const unsigned int row, const float* filter, const unsigned int filterSize, const bool isSymmetric, const unsigned int sourcePaddingElements)
{
	ocean_assert(source != nullptr && target != nullptr && filter != nullptr);
	ocean_assert(channels >= 1u);
	ocean_assert(filterSize <= height);
	ocean_assert(filterSize % 2u == 1u);

	const unsigned int sourceStrideElements = width * channels + sourcePaddingElements;

	unsigned int remainingElements = width * channels;

	while (remainingElements >= 16u)
	{
		filterVerticalBorderRow16Elements32BitPerChannelFloat<TSource, TTarget, tProcessorInstructions>(source, target, sourceStrideElements, height, row, filter, filterSize, isSymmetric);

		source += 16;
		target += 16;

		remainingElements -= 16u;
	}

	while (remainingElements >= 8u)
	{
		filterVerticalBorderRow8Elements32BitPerChannelFloat<TSource, TTarget, tProcessorInstructions>(source, target, sourceStrideElements, height, row, filter, filterSize, isSymmetric);

		source += 8;
		target += 8;

		remainingElements -= 8u;
	}

	ocean_assert(width * channels >= 8u);
	ocean_assert(remainingElements < 8u);

	if (remainingElements != 0u)
	{
		const unsigned int shift = 8u - remainingElements;

		filterVerticalBorderRow8Elements32BitPerChannelFloat<TSource, TTarget, tProcessorInstructions>(source - shift, target - shift, sourceStrideElements, height, row, filter, filterSize, isSymmetric);
	}
}

template <typename TSource, typename TFilter, const ProcessorInstructions tProcessorInstructions>
void FrameFilterSeparable::filterHorizontalSubset(const TSource* source, TFilter* target, const unsigned int width, const unsigned int height, const unsigned int channels, const TFilter* filter, const unsigned int filterSize, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(source != nullptr && target != nullptr && filter != nullptr);
	ocean_assert(width >= filterSize + 1u);

	ocean_assert(channels >= 1u && channels <= 8u);
	ocean_assert(filterSize >= 1u && (filterSize % 2u) == 1u);

	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	const unsigned int sourceStrideElements = width * channels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * channels + targetPaddingElements;

	const bool isSymmetric = isFilterSymmetric(filter, filterSize);

	const unsigned int filterSize_2 = filterSize / 2u;
	const unsigned int extraPixels = filterSize_2 * 2u;

	const unsigned int extendedElements = (width + extraPixels) * channels;

	Memory extendedRowMemory = Memory::create<TSource>(extendedElements);
	TSource* const extendedRow = extendedRowMemory.data<TSource>();
	ocean_assert(extendedRow != nullptr);

	source += firstRow * sourceStrideElements;
	target += firstRow * targetStrideElements;

	for (unsigned int rowsProcessed = 0u; rowsProcessed < numberRows; ++rowsProcessed)
	{
		// we create an intermediate row with extended pixels left and right
		fillLeftExtraBorder<TSource>(source, channels, filterSize_2, extendedRow);
		memcpy(extendedRow + filterSize_2 * channels, source, width * channels * sizeof(TSource));
		fillRightExtraBorder<TSource>(source + width * channels, channels, filterSize_2, extendedRow + (width + filterSize_2) * channels);

		const TSource* extendedSource = extendedRow;

		unsigned int remainingElements = width * channels;

#if (defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10) || (defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 20)

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
		const ProcessorInstructions instructions = ProcessorInstructions(PI_NEON & tProcessorInstructions);
#elif defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 20
		const ProcessorInstructions instructions = ProcessorInstructions(PI_SSE_2 & tProcessorInstructions);
#endif

		// now we apply 8-block-elements as long as they fit into the frame

		while (remainingElements >= 8u)
		{
			filterHorizontalRowOneBlockWith8Elements<TSource, TFilter, instructions>(extendedSource, target, channels, filter, filterSize, isSymmetric);

			extendedSource += 8;
			target += 8;

			remainingElements -= 8u;
		}

		// now we apply 4-block-elements as long as they fit into the frame

		while (remainingElements >= 4u)
		{
			filterHorizontalRowOneBlockWith4Elements<TSource, TFilter, instructions>(extendedSource, target, channels, filter, filterSize, isSymmetric);

			extendedSource += 4;
			target += 4;

			remainingElements -= 4u;
		}

		// finally, we check whether we have 1-3 elements left; in this case, we simply process some elements another time

		if (remainingElements != 0u)
		{
			const unsigned int shift = 4u - remainingElements;

			extendedSource -= shift;
			target -= shift;

			filterHorizontalRowOneBlockWith4Elements<TSource, TFilter, instructions>(extendedSource, target, channels, filter, filterSize, isSymmetric);

			// we do not need to shift extendedSource += 4
			target += 4u;
		}

#else

		OCEAN_SUPPRESS_UNUSED_WARNING(extendedSource);
		OCEAN_SUPPRESS_UNUSED_WARNING(remainingElements);
		OCEAN_SUPPRESS_UNUSED_WARNING(isSymmetric);

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10 || OCEAN_HARDWARE_SSE_VERSION >= 20

#ifdef OCEAN_INTENSIVE_DEBUG
		{
			const TFilter* const debugTarget = target - width * channels;

			for (unsigned int x = 0u; x < width; ++x)
			{
				for (unsigned int n = 0u; n < channels; ++n)
				{
					float result = 0.0f;

					for (int xx = -int(filterSize_2); xx <= int(filterSize_2); ++xx)
					{
						const unsigned int mirroredXX = (x < filterSize_2) ? mirroredBorderLocationLeft(int(x) + xx) : mirroredBorderLocationRight((unsigned int)(int(x) + xx), width);
						result += float(*(source + mirroredXX * channels + int(n))) * filter[xx + int(filterSize_2)];
					}

					const TFilter targetValue = debugTarget[x * channels + n];

					if (std::is_same<float, TFilter>::value)
					{
						ocean_assert(NumericT<TFilter>::isWeakEqual(result, targetValue));
					}
					else
					{
						const TFilter result8_converted = (TFilter)(result);
						const TFilter result8_rounded = (TFilter)(result + 0.51f);
						ocean_assert(result8_converted == targetValue || result8_rounded == targetValue);
					}
				}
			}
		}
#endif

		source += sourceStrideElements;
		target += targetPaddingElements;
	}
}

template <typename TSource, typename TTarget, ProcessorInstructions tProcessorInstructions>
void FrameFilterSeparable::filterVerticalSubset(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int channels, const float* filter, const unsigned int filterSize, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(filter != nullptr);
	ocean_assert(height >= filterSize / 2u + 1u);
	ocean_assert(channels >= 1u && channels <= 8u);

	ocean_assert(filterSize >= 1u && (filterSize % 2u) == 1u);

	ocean_assert(firstRow + numberRows <= height);
	ocean_assert(width * channels >= 8u * 2u);

	const unsigned int sourceStrideElements = width * channels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * channels + targetPaddingElements;

	const bool isSymmetric = isFilterSymmetric(filter, filterSize);

	const unsigned int filterSize_2 = filterSize / 2u;

#ifdef OCEAN_INTENSIVE_DEBUG
	const TSource* const debugSource = source;
#endif

	source += firstRow * sourceStrideElements;
	target += firstRow * targetStrideElements;

	unsigned int row = firstRow;

	// first we check whether we are located at the top border, whether we start within the first filterSize_2 rows

	while (row < min(firstRow + numberRows, filterSize_2))
	{
		filterVerticalBorderRow32BitPerChannelFloat<TSource, TTarget, tProcessorInstructions>(source, target, width, height, channels, row, filter, filterSize, isSymmetric, sourcePaddingElements);

#ifdef OCEAN_INTENSIVE_DEBUG
		{
			for (unsigned int x = 0u; x < width * channels; ++x)
			{
				float result = 0.0f;

				for (int y = -int(filterSize_2); y <= int(filterSize_2); ++y)
				{
					const unsigned int mirroredY = mirroredBorderLocationLeft(int(row) + y);
					result += float(*(debugSource + mirroredY * int(sourceStrideElements) + int(x))) * filter[y + int(filterSize_2)];
				}

				const TTarget targetValue = target[x];

				if (std::is_same<float, TTarget>::value)
				{
					ocean_assert(NumericT<TTarget>::isWeakEqual(result, targetValue));
				}
				else
				{
					ocean_assert(NumericT<TTarget>::isEqual((TTarget)(result), targetValue, TTarget(2)));
				}
			}
		}
#endif

		source += sourceStrideElements;
		target += targetStrideElements;

		++row;
	}

	// now we proceed the rows not located at the top or bottom border of the frame

	while (row < min(firstRow + numberRows, height - filterSize_2))
	{
		filterVerticalCoreRow32BitPerChannelFloat<TSource, TTarget, tProcessorInstructions>(source, target, width, channels, filter, filterSize, isSymmetric, sourcePaddingElements);

#ifdef OCEAN_INTENSIVE_DEBUG
		{
			for (unsigned int x = 0u; x < width * channels; ++x)
			{
				float result = 0.0f;

				for (int y = -int(filterSize_2); y <= int(filterSize_2); ++y)
					result += float(*(debugSource + (int(row) + y) * int(sourceStrideElements) + int(x))) * filter[y + int(filterSize_2)];

				const TTarget targetValue = target[x];

				ocean_assert(result >= 0.0f && result < 256.0f);

				if (std::is_same<float, TTarget>::value)
				{
					ocean_assert(NumericT<TTarget>::isWeakEqual(result, targetValue));
				}
				else
				{
					ocean_assert(NumericT<TTarget>::isEqual((TTarget)(result), targetValue, TTarget(2)));
				}
			}
		}
#endif

		source += sourceStrideElements;
		target += targetStrideElements;

		++row;
	}

	// now we check whether we are located at the bottom border, whether we start within the last filterSize_2 rows (or need to process them)

	while (row < firstRow + numberRows)
	{
		ocean_assert(row + filterSize_2 >= height);

		filterVerticalBorderRow32BitPerChannelFloat<TSource, TTarget, tProcessorInstructions>(source, target, width, height, channels, row, filter, filterSize, isSymmetric, sourcePaddingElements);

#ifdef OCEAN_INTENSIVE_DEBUG
		{
			// we do not check the left and right corner, we simply check the middle block of the upper border
			for (unsigned int x = 0u; x < width * channels; ++x)
			{
				float result = 0.0f;

				for (int y = -int(filterSize_2); y <= int(filterSize_2); ++y)
				{
					const unsigned int mirroredY = mirroredBorderLocationRight((unsigned int)(int(row) + y), height);
					result += float(*(debugSource + mirroredY * int(sourceStrideElements) + int(x))) * filter[y + int(filterSize_2)];
				}

				const TTarget targetValue = target[x];

				ocean_assert(result >= 0.0f && result < 256.0f);

				if (std::is_same<float, TTarget>::value)
				{
					ocean_assert(NumericT<TTarget>::isWeakEqual(result, targetValue));
				}
				else
				{
					ocean_assert(NumericT<TTarget>::isEqual((TTarget)(result), targetValue, TTarget(2)));
				}
			}
		}
#endif

		source += sourceStrideElements;
		target += targetStrideElements;

		++row;
	}
}

template <typename T, typename TFilter, ProcessorInstructions tProcessorInstructions>
inline void FrameFilterSeparable::filter(const T* source, T* target, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const TFilter* horizontalFilter, const unsigned int horizontalFilterSize, const TFilter* verticalFilter, const unsigned int verticalFilterSize, ReusableMemory* reusableMemory, Worker* worker)
{
	Frame localIntermediateFrame;
	Frame* intermediateFrame = &localIntermediateFrame;

	if (reusableMemory != nullptr)
	{
		intermediateFrame = &reusableMemory->intermediateFrame_;
	}

	intermediateFrame->set(FrameType(width, height, FrameType::genericPixelFormat<TFilter>(channels), FrameType::ORIGIN_UPPER_LEFT), false /*forceOwner*/, true /*forceWritable*/);

	// first we apply the horizontal filtering

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&filterHorizontalSubset<T, TFilter, tProcessorInstructions>, source, intermediateFrame->data<TFilter>(), width, height, channels, horizontalFilter, horizontalFilterSize, sourcePaddingElements, intermediateFrame->paddingElements(), 0u, 0u), 0u, height);
	}
	else
	{
		filterHorizontalSubset<T, TFilter, tProcessorInstructions>(source, intermediateFrame->data<TFilter>(), width, height, channels, horizontalFilter, horizontalFilterSize, sourcePaddingElements, intermediateFrame->paddingElements(), 0u, height);
	}

	// now we apply the vertical filtering
	// therefore, we first need to calculate the floating point filter functions (in case we use integer factors)

	std::vector<float> localFloatFilters;
	const float* verticalFloatFilter = nullptr;

	if (std::is_same<TFilter, float>::value)
	{
		verticalFloatFilter = (const float*)(verticalFilter);
	}
	else
	{
		ocean_assert((std::is_same<TFilter, unsigned int>::value));

		const TFilter sumHorizontalFilterValues = sumFilterValues(horizontalFilter, horizontalFilterSize);
		const TFilter sumVerticalFilterValues = sumFilterValues(verticalFilter, verticalFilterSize);

		const unsigned int normalizationFactor = (unsigned int)(sumHorizontalFilterValues) * (unsigned int)(sumVerticalFilterValues);
		ocean_assert(normalizationFactor != 0u);

		const float invNormalizationFactor = 1.0f / float(normalizationFactor);

		std::vector<float>& floatFilterBufferToUse = reusableMemory != nullptr ? reusableMemory->filterFactors_ : localFloatFilters;

		floatFilterBufferToUse.resize(verticalFilterSize);

		for (unsigned int n = 0u; n < verticalFilterSize; ++n)
		{
			floatFilterBufferToUse[n] = float(verticalFilter[n]) * invNormalizationFactor;
		}

		verticalFloatFilter = floatFilterBufferToUse.data();
	}

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&filterVerticalSubset<TFilter, T, tProcessorInstructions>, intermediateFrame->constdata<TFilter>(), target, width, height, channels, (const float*)(verticalFloatFilter), verticalFilterSize, intermediateFrame->paddingElements(), targetPaddingElements, 0u, 0u), 0u, height);
	}
	else
	{
		filterVerticalSubset<TFilter, T, tProcessorInstructions>(intermediateFrame->constdata<TFilter>(), target, width, height, channels, (const float*)(verticalFloatFilter), verticalFilterSize, intermediateFrame->paddingElements(), targetPaddingElements, 0u, height);
	}
}

template <typename T, typename TFilter>
bool FrameFilterSeparable::filter(const T* source, T* target, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const TFilter* horizontalFilter, const unsigned int horizontalFilterSize, const TFilter* verticalFilter, const unsigned int verticalFilterSize, Worker* worker, ReusableMemory* reusableMemory, const ProcessorInstructions processorInstructions)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= horizontalFilterSize && height >= verticalFilterSize);
	ocean_assert(channels >= 1u);

	if (source == nullptr || target == nullptr || width < horizontalFilterSize || height < verticalFilterSize || channels == 0u)
	{
		return false;
	}

	OCEAN_SUPPRESS_UNUSED_WARNING(reusableMemory);

	if (width * channels >= 16u && width >= horizontalFilterSize + 1u)
	{
		switch (Processor::bestInstructionGroup<false>(processorInstructions))
		{
			case PI_GROUP_AVX_2_SSE_4_1:
				// temporary disabled: OCEAN_APPLY_IF_AVX((filter<T, TFilter, PI_GROUP_AVX_2_SSE_4_1>(source, target, width, height, channels, horizontalFilter, horizontalFilterSize, verticalFilter, verticalFilterSize, worker)));
			case PI_GROUP_SSE_4_1:
			case PI_GROUP_AVX_2_SSE_2:
			case PI_GROUP_SSE_2:
				OCEAN_APPLY_IF_SSE((filter<T, TFilter, PI_SSE_2>(source, target, width, height, channels, sourcePaddingElements, targetPaddingElements, horizontalFilter, horizontalFilterSize, verticalFilter, verticalFilterSize, reusableMemory, worker)));
				return true;

			case PI_GROUP_NEON:
				OCEAN_APPLY_IF_NEON((filter<T, TFilter, PI_GROUP_NEON>(source, target, width, height, channels, sourcePaddingElements, targetPaddingElements, horizontalFilter, horizontalFilterSize, verticalFilter, verticalFilterSize, reusableMemory, worker)));
				return true;

			case PI_NONE:
				break;

			default:
				ocean_assert(false && "Invalid instructions!");
		}
	}

	if constexpr (std::is_same<float, TFilter>::value)
	{
		filterUniversal<T>(source, target, width, height, channels, sourcePaddingElements, targetPaddingElements, (const float*)(horizontalFilter), horizontalFilterSize, (const float*)(verticalFilter), verticalFilterSize, worker);
		return true;
	}
	else
	{
		if constexpr (std::is_same<unsigned int, TFilter>::value)
		{
			const TFilter horizontalNormalization = sumFilterValues(horizontalFilter, horizontalFilterSize);
			ocean_assert(horizontalNormalization != TFilter(0));

			std::vector<float> horizontalFloatFilter(horizontalFilterSize);
			for (size_t n = 0; n < horizontalFloatFilter.size(); ++n)
			{
				horizontalFloatFilter[n] = float(horizontalFilter[n]) / float(horizontalNormalization);
			}

			const TFilter verticalNormalization = sumFilterValues(verticalFilter, verticalFilterSize);
			ocean_assert(verticalNormalization != TFilter(0));

			std::vector<float> verticalFloatFilter(verticalFilterSize);
			for (size_t n = 0; n < verticalFloatFilter.size(); ++n)
			{
				verticalFloatFilter[n] = float(verticalFilter[n]) / float(verticalNormalization);
			}

			return filterUniversal<T>(source, target, width, height, channels, sourcePaddingElements, targetPaddingElements, horizontalFloatFilter.data(), (unsigned int)horizontalFloatFilter.size(), verticalFloatFilter.data(), (unsigned int)verticalFloatFilter.size(), worker);
		}
	}

	ocean_assert(false && "Invalid combination of parameters!");
	return false;
}

template <typename T>
bool FrameFilterSeparable::filterUniversal(const T* source, T* target, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const float* horizontalFilter, const unsigned int horizontalFilterSize, const float* verticalFilter, const unsigned int verticalFilterSize, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(channels != 0u);

	ocean_assert(horizontalFilter != nullptr && verticalFilter != nullptr);
	ocean_assert(horizontalFilterSize % 2u == 1u);
	ocean_assert(verticalFilterSize % 2u == 1u);

	if (source == nullptr || target == nullptr
		|| verticalFilter == nullptr || horizontalFilter == nullptr
		|| horizontalFilterSize > width || verticalFilterSize > height
		|| horizontalFilterSize % 2u != 1u || verticalFilterSize % 2u != 1u)
	{
		return false;
	}

	typedef typename FloatTyper<T>::Type TIntermediate;

	Frame intermediateFrame(FrameType(width, height, FrameType::genericPixelFormat<TIntermediate>(channels), FrameType::ORIGIN_UPPER_LEFT));

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&filterUniversalHorizontalSubset<T, TIntermediate>, source, intermediateFrame.data<TIntermediate>(), width, channels, horizontalFilter, horizontalFilterSize, sourcePaddingElements, intermediateFrame.paddingElements(), 0u, 0u), 0u, height);
		worker->executeFunction(Worker::Function::createStatic(&filterUniversalVerticalSubset<T, TIntermediate>, intermediateFrame.constdata<TIntermediate>(), target, width, height, channels, verticalFilter, verticalFilterSize, intermediateFrame.paddingElements(), targetPaddingElements, 0u, 0u), 0u, height);
	}
	else
	{
		filterUniversalHorizontalSubset<T, TIntermediate>(source, intermediateFrame.data<TIntermediate>(), width, channels, horizontalFilter, horizontalFilterSize, sourcePaddingElements, intermediateFrame.paddingElements(), 0u, height);
		filterUniversalVerticalSubset<T, TIntermediate>(intermediateFrame.data<TIntermediate>(), target, width, height, channels, verticalFilter, verticalFilterSize, intermediateFrame.paddingElements(), targetPaddingElements, 0u, height);
	}

	return true;
}

template <typename T, typename TIntermediate>
void FrameFilterSeparable::filterUniversalHorizontalSubset(const T* source, TIntermediate* target, const unsigned int width, unsigned int channels, const float* horizontalFilter, unsigned int horizontalFilterSize, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u);
	ocean_assert(channels != 0u);

	ocean_assert(horizontalFilterSize <= size_t(width));
	ocean_assert(horizontalFilterSize % 2u == 1u);

	const unsigned int sourceStrideElements = width * channels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * channels + targetPaddingElements;

	const unsigned int filterSize = horizontalFilterSize;
	const unsigned int filterSize_2 = filterSize / 2u;
	ocean_assert(filterSize_2 * 2u <= width);

	std::vector<TIntermediate> filterCopy;

	if (!std::is_same<TIntermediate, float>::value)
	{
		filterCopy.resize(horizontalFilterSize);
		for (size_t n = 0; n < filterCopy.size(); ++n)
		{
			filterCopy[n] = TIntermediate(horizontalFilter[n]);
		}
	}

	const TIntermediate* const filter = filterCopy.empty() ? (const TIntermediate*)horizontalFilter : filterCopy.data();

	source += firstRow * sourceStrideElements;
	target += firstRow * targetStrideElements;

	TIntermediate* const targetEnd = target + numberRows * targetStrideElements;

	while (target != targetEnd)
	{
		ocean_assert(target < targetEnd);

		// left border: [0, filterSize_2 - 1]

		for (unsigned int x = 0u; x < filterSize_2; ++x)
		{
			for (unsigned int n = 0u; n < channels; ++n)
			{
				TIntermediate response = TIntermediate(source[channels * mirroredBorderLocationLeft(-int(filterSize_2) + int(x)) + n]) * filter[0];

				for (unsigned int s = 1u; s < filterSize; ++s)
					response += TIntermediate(source[channels * mirroredBorderLocationLeft(-int(filterSize_2) + int(x + s)) + n]) * filter[s];

				target[n] = response;
			}

			target += channels;
			// we keep the location of source
		}

		// center block: [filterSize_2, width - filterSize - 2)

		for (unsigned int x = filterSize_2; x < width - filterSize_2; ++x)
		{
			for (unsigned int n = 0u; n < channels; ++n)
			{
				TIntermediate response = TIntermediate(source[channels * 0u + n]) * filter[0];

				for (unsigned int s = 1u; s < filterSize; ++s)
					response += TIntermediate(source[channels * s + n]) * filter[s];

				target[n] = response;
			}

			target += channels;
			source += channels;
		}

		// right border: [width - filterSize_2, width - 1]

		for (unsigned int x = 0u; x < filterSize_2; ++x)
		{
			for (unsigned int n = 0u; n < channels; ++n)
			{
				TIntermediate response = TIntermediate(source[channels * mirroredBorderLocationRight(x, filterSize_2 * 2u) + n]) * filter[0];

				for (unsigned int s = 1u; s < filterSize; ++s)
					response += TIntermediate(source[channels * mirroredBorderLocationRight(x + s, filterSize_2 * 2u) + n]) * filter[s];

				target[n] = response;
			}

			target += channels;
			// we keep the location of source
		}

		source += filterSize_2 * 2u * channels + sourcePaddingElements;
		target += targetPaddingElements;
	}
}

template <typename T, typename TIntermediate>
void FrameFilterSeparable::filterUniversalVerticalSubset(const TIntermediate* source, T* target, const unsigned int width, const unsigned int height, const unsigned int channels, const float* verticalFilter, const unsigned int verticalFilterSize, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(channels != 0u);

	ocean_assert(verticalFilterSize <= height);
	ocean_assert(verticalFilterSize % 2u == 1u);

	const unsigned int sourceStrideElements = width * channels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * channels + targetPaddingElements;

	const TIntermediate* const sourceStart = source;

	const unsigned int filterSize = verticalFilterSize;
	const unsigned int filterSize_2 = filterSize / 2u;
	ocean_assert(filterSize_2 * 2u <= height);

	std::vector<TIntermediate> filterCopy;

	if (!std::is_same<TIntermediate, float>::value)
	{
		filterCopy.resize(verticalFilterSize);

		for (size_t n = 0; n < filterCopy.size(); ++n)
		{
			filterCopy[n] = TIntermediate(verticalFilter[n]);
		}
	}

	const TIntermediate* const filter = filterCopy.empty() ? (const TIntermediate*)verticalFilter : filterCopy.data();

	source += max(0, int(firstRow) - int(filterSize_2)) * sourceStrideElements;
	target += firstRow * targetStrideElements;

	unsigned int y = firstRow;

	// top border: [0, filterSize_2 - 1]

	while (y < min(filterSize_2, firstRow + numberRows))
	{
		ocean_assert(source == sourceStart);
		const TIntermediate* sourceCopy = source;

		for (unsigned int x = 0u; x < width; ++x)
		{
			for (unsigned int n = 0u; n < channels; ++n)
			{
				TIntermediate response = TIntermediate(source[sourceStrideElements * mirroredBorderLocationLeft(-int(filterSize_2) + int(y)) + n]) * filter[0];

				for (unsigned int s = 1u; s < filterSize; ++s)
					response += TIntermediate(source[sourceStrideElements * mirroredBorderLocationLeft(-int(filterSize_2) + int(y + s)) + n]) * filter[s];

				target[n] = T(response);
			}

			target += channels;
			source += channels;
		}

		target += targetPaddingElements;

		// we set back the location of the source pointer
		source = sourceCopy;
		++y;
	}

	// center block: [filterSize_2, height - filterSize - 2)

	const unsigned int centerRows = (unsigned int)max(0, int(min(firstRow + numberRows, height - filterSize_2)) - int(y));

	for (unsigned int yCenter = 0u; yCenter < centerRows; ++yCenter)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			for (unsigned int c = 0u; c < channels; ++c)
			{
				TIntermediate response = TIntermediate(source[channels * 0u + c]) * filter[0];

				for (unsigned int s = 1u; s < filterSize; ++s)
					response += TIntermediate(source[sourceStrideElements * s + c]) * filter[s];

				target[c] = T(response);
			}

			source += channels;
			target += channels;
		}

		source += sourcePaddingElements;
		target += targetPaddingElements;
	}

	y += centerRows;

	// bottom border: [height - filterSize_2, height - 1]

	while (y < firstRow + numberRows)
	{
		ocean_assert(y >= height - filterSize_2 && y < height);
		source = sourceStart + (height - filterSize_2 * 2u) * sourceStrideElements;

		const unsigned int yy = y - (height - filterSize_2);
		ocean_assert(yy < filterSize_2);

		for (unsigned int x = 0u; x < width; ++x)
		{
			for (unsigned int n = 0u; n < channels; ++n)
			{
				TIntermediate response = TIntermediate(source[sourceStrideElements * mirroredBorderLocationRight(yy, filterSize_2 * 2u) + n]) * filter[0];

				for (unsigned int s = 1u; s < filterSize; ++s)
				{
					response += TIntermediate(source[sourceStrideElements * mirroredBorderLocationRight(yy + s, filterSize_2 * 2u) + n]) * filter[s];
				}

				target[n] = T(response);
			}

			target += channels;
			source += channels;
		}

		target += targetPaddingElements;

		++y;
	}
}

inline unsigned int FrameFilterSeparable::mirroredBorderLocationLeft(const int value)
{
	// Original: -3 -2 -1 |  0  1  2  3  4  5  6
	//   Result:  2  1  0 |  0  1  2  3  4  5  6

	if (value >= 0)
	{
		return value;
	}
	else
	{
		return -value - 1;
	}
}

inline unsigned int FrameFilterSeparable::mirroredBorderLocationRight(const unsigned int value, const unsigned int size)
{
	ocean_assert(value < 2u * size);

	// Original: 4  5  6 ... s-2  s-1 |   s  s+1  s+2
	//   Result: 4  5  6 ... s-2  s-1 | s-1  s-2  s-3

	if (value < size)
	{
		return value;
	}
	else
	{
		ocean_assert(size * 2u - value - 1u < size);
		return size * 2u - value - 1u;
	}
}

}

}

#endif // META_OCEAN_CV_FRAME_FILTER_BINOMIAL_H
