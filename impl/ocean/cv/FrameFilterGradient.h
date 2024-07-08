/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_FILTER_GRADIENT_H
#define META_OCEAN_CV_FRAME_FILTER_GRADIENT_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameFilterGradientBase.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements a gradient frame filter.
 * @ingroup cv
 */
class FrameFilterGradient
{
	public:

		/**
		 * Horizontal and vertical gradient filter for a 1-plane frame with arbitrary data type and arbitrary number of channels.
		 * The horizontal and vertical filter responses are stored in a 1-plane response frame so that for each pixel and channel two corresponding filter results exist (interleaved).<br>
		 * The border response pixel results are set to zero.
		 * @param source The source frame to filter, must be valid
		 * @param target The filter response with two filter response elements per pixel and channel, must be valid
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param multiplicationFactor Multiplication factor that is applied to each filter result before the value is assigned to the target, with range (-infinity, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam TSource The data type of the source frame
		 * @tparam TTarget The data type of the target frame
		 * @tparam tChannels The number of the source frame, with range [1, infinity)
		 * @tparam tNormalizeByTwo True, to normalize the subtraction result by two; False, to simply determine the subtraction result
		 * @see filterHorizontalVerticalSubFrame(), filterHorizontalVerticalMagnitudeSquared().
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels, bool tNormalizeByTwo>
		static inline void filterHorizontalVertical(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const TTarget multiplicationFactor = TTarget(1), Worker* worker = nullptr);

		/**
		 * Squared magnitude gradient filter using the horizontal and vertical gradients that can be applied to zipped frames with arbitrary data type and arbitrary number of channels.
		 * The border pixel results are set to zero.<br>
		 * The two individual filter responses are collected, applied as a vector and the vector's squared length is stored as result.<br>
		 * Thus, for each pixel and each channel one filter response is created.
		 * @param source The source frame to filter, must be valid
		 * @param target Filter response with one filter response (the squared magnitude of the gradient) per pixel and channel
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param multiplicationFactor Multiplication factor that is applied to each filter result before the value is assigned to the target, with range (-infinity, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam TSource The data type of the source frame
		 * @tparam TTarget The data type of the target frame
		 * @tparam tChannels The number of the source frame, with range [1, infinity)
		 * @tparam tNormalizeByTwo True, to normalize the subtraction result by two; False, to simply determine the subtraction result
		 * @see filterHorizontalVertical().
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels, bool tNormalizeByTwo>
		static inline void filterHorizontalVerticalMagnitudeSquared(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const TTarget multiplicationFactor = TTarget(1), Worker* worker = nullptr);

		/**
		 * Horizontal and vertical gradient filter for sub-frame of a 1-plane frame with arbitrary data type and arbitrary number of channels.
		 * In contrast to filterHorizontalVertical(), this function does not set border response pixels to zero in case the border of the sub-frame is not located at the border of the source frame.
		 * @param source The source frame to filter, must be valid
		 * @param sourceWidth The width of the source frame in pixel, with range [3, infinity)
		 * @param sourceHeight the height of the source frame in pixel, with range [3, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param sourceLeft Horizontal start position of the sub-frame inside the source frame, in pixels, with range [0, sourceWidth)
		 * @param sourceTop Vertical start position of the sub-frame inside the source frame, in pixels, with range [0, sourceHeight)
		 * @param target The filter response with two elements per pixel and channel with dimension targetWidth x targetHeight, must be valid
		 * @param targetWidth The width of the sub-frame inside the source frame (and thus the width of the target response frame), in pixels, with range [1, sourceWidth - sourceLeft]
		 * @param targetHeight The height of the sub-frame inside the source frame (and thus the height of the target response frame), in pixels, with range [1, sourceHeight - sourceTop]
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param multiplicationFactor Multiplication factor that is applied to each filter result before the value is assigned to the target, with range (-infinity, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam TSource The data type of the source frame
		 * @tparam TTarget The data type of the target frame
		 * @tparam tChannels The number of the source frame, with range [1, infinity)
		 * @tparam tNormalizeByTwo True, to normalize the subtraction result by two; False, to simply determine the subtraction result
		 * @see filterHorizontalVertical().
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels, bool tNormalizeByTwo>
		static inline void filterHorizontalVerticalSubFrame(const TSource* source, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourcePaddingElements, const unsigned int sourceLeft, const unsigned int sourceTop, TTarget* target, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetPaddingElements, const TTarget multiplicationFactor = TTarget(1), Worker* worker = nullptr);

		/**
		 * Determines the lined integral image of the horizontal 1x2 gradient filter when applied to a source image.
		 * The function applies the following 1x2 box filter to each pixel of the source image:
		 * <pre>
		 * [-1. 1]
		 * </pre>
		 * The response value will be stored at the location of the left filter pixel (marked with a dot `.`).
		 * The intermediate (internal only) response image has resolution (width-1)x(height).
		 * The resulting (lined) integral response image has the following scheme:
		 * <pre>
		 *  ------------
		 * |000000000000|
		 * |0|----------|
		 * |0|          |
		 * |0| Integral |
		 * |0|          |
		 *  ------------
		 * </pre>
		 * The resolution of the (lined) integral image is: (width)x(height + 1).
		 * @param source The source image to which the horizontal gradient filter will be applied, with resolution (width)x(height), must be valid
		 * @param width The width of the source image in pixel, with range [2, infinity)
		 * @param height The height of the source image in pixel, with range [1, infinity)
		 * @param integral The resulting integral image, with resolution (width)x(height + 1), must be valid
		 * @param sourcePaddingElements Optional number of padding elements at the end of each row of the source frame, in elements, with range [0, infinity)
		 * @param integralPaddingElements Optional number of padding elements at the end of each row of the integral frame, in elements, with range [0, infinity)
		 * @tparam T The data type of the elements in the source image
		 * @tparam TIntegral The data type of the elements in the integral image
		 * @tparam tAbsoluteGradient True, to determine the absolute gradients; False, to determine the signed gradients
		 */
		template <typename T, typename TIntegral, bool tAbsoluteGradient = false>
		static void filterHorizontal1x2LinedIntegralImage(const T* source, const unsigned int width, const unsigned int height, TIntegral* integral, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements);

		/**
		 * Determines the lined integral image of the vertical 2x1 gradient filter when applied to a source image.
		 * The function applies the following 2x1 box filter to each pixel of the source image:
		 * <pre>
		 * [ -1. ]
		 * [  1  ]
		 * </pre>
		 * The response value will be stored at the location of the left filter pixel (marked with a dot `.`).
		 * The intermediate (internal only) response image has resolution (width)x(height - 1).
		 * The resulting (lined) integral response image has the following scheme:
		 * <pre>
		 *  ------------
		 * |000000000000|
		 * |0|----------|
		 * |0|          |
		 * |0| Integral |
		 * |0|          |
		 *  ------------
		 * </pre>
		 * The resolution of the (lined) integral image is: (width + 1)x(height).
		 * @param source The source image to which the horizontal gradient filter will be applied, with resolution (width)x(height), must be valid
		 * @param width The width of the source image in pixel, with range [1, infinity)
		 * @param height The height of the source image in pixel, with range [2, infinity)
		 * @param integral The resulting integral image, with resolution (width + 1)x(height), must be valid
		 * @param sourcePaddingElements Optional number of padding elements at the end of each row of the source frame, in elements, with range [0, infinity)
		 * @param integralPaddingElements Optional number of padding elements at the end of each row of the integral frame, in elements, with range [0, infinity)
		 * @tparam T The data type of the elements in the source image
		 * @tparam TIntegral The data type of the elements in the integral image
		 * @tparam tAbsoluteGradient True, to determine the absolute gradients; False, to determine the signed gradients
		 */
		template <typename T, typename TIntegral, bool tAbsoluteGradient = false>
		static void filterVertical2x1LinedIntegralImage(const T* source, const unsigned int width, const unsigned int height, TIntegral* integral, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements);
};

template <typename TSource, typename TTarget, unsigned int tChannels, bool tNormalizeByTwo>
inline void FrameFilterGradient::filterHorizontalVertical(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const TTarget multiplicationFactor, Worker* worker)
{
	FrameFilterGradientBase::filterHorizontalVertical<TSource, TTarget, tChannels, tNormalizeByTwo>(source, target, width, height, sourcePaddingElements, targetPaddingElements, multiplicationFactor, worker);
}

template <typename TSource, typename TTarget, unsigned int tChannels, bool tNormalizeByTwo>
inline void FrameFilterGradient::filterHorizontalVerticalMagnitudeSquared(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const TTarget multiplicationFactor, Worker* worker)
{
	FrameFilterGradientBase::filterHorizontalVerticalMagnitudeSquared<TSource, TTarget, tChannels, tNormalizeByTwo>(source, target, width, height, multiplicationFactor, worker);
}

template <typename TSource, typename TTarget, unsigned int tChannels, bool tNormalizeByTwo>
inline void FrameFilterGradient::filterHorizontalVerticalSubFrame(const TSource* source, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourcePaddingElements, const unsigned int sourceLeft, const unsigned int sourceTop, TTarget* target, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetPaddingElements, const TTarget multiplicationFactor, Worker* worker)
{
	FrameFilterGradientBase::filterHorizontalVerticalSubFrame<TSource, TTarget, tChannels, tNormalizeByTwo>(source, sourceWidth, sourceHeight, sourcePaddingElements, sourceLeft, sourceTop, target, targetWidth, targetHeight, targetPaddingElements, multiplicationFactor, worker);
}

template <typename T, typename TIntegral, bool tAbsoluteGradient>
void FrameFilterGradient::filterHorizontal1x2LinedIntegralImage(const T* source, const unsigned int width, const unsigned int height, TIntegral* integral, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements)
{
	static_assert(sizeof(T) <= sizeof(TIntegral), "Invalid integral elements!");

	ocean_assert(source != nullptr);
	ocean_assert(width >= 2u);
	ocean_assert(height >= 1u);
	ocean_assert(integral != nullptr);

	/*
	 * This is the resulting lined integral image, with resolution (width)x(height + 1):
	 *  ------------
	 * |000000000000|
	 * |0|----------|
	 * |0|          |
	 * |0| Integral |
	 * |0|          |
	 * |------------
	 */

	typedef typename DifferenceValueTyper<TIntegral>::Type TSignedIntegral;

	// entire top line will be set to zero
	memset(integral, 0x00, width * sizeof(TIntegral));

#ifdef OCEAN_DEBUG
	for (unsigned int n = 0u; n < width; ++n)
	{
		ocean_assert(integral[n] == TIntegral(0));
	}
#endif

	integral += width + integralPaddingElements;

	// we calculate the first row of the integral image

	const T* const sourceFirstRowEnd = source + width - 1u; // excluding the last pixel
	const T* const sourceEnd = source + (width + sourcePaddingElements) * height;

	const TIntegral* integralPreviousRow = integral;

	TIntegral previousIntegral = TIntegral(0);

	*integral++ = TIntegral(0);

	// the remaining pixels of the first row

	while (source != sourceFirstRowEnd)
	{
		previousIntegral += tAbsoluteGradient ? TIntegral(NumericT<TSignedIntegral>::abs(TSignedIntegral(source[1] - source[0]))) : TIntegral(source[1] - source[0]);
		++source;

		*integral++ = previousIntegral;
	}

	source += sourcePaddingElements + 1u; // including the last pixel
	integral += integralPaddingElements;

	// we calculate the remaining rows

	while (source != sourceEnd)
	{
		const T* const sourceRowEnd = source + width - 1u; // excluding the last pixel

		previousIntegral = TIntegral(0);

		// left pixel
		*integral++ = TIntegral(0);

		++integralPreviousRow;

		while (source != sourceRowEnd)
		{
			previousIntegral += tAbsoluteGradient ? TIntegral(NumericT<TSignedIntegral>::abs(TSignedIntegral(source[1] - source[0]))) : TIntegral(source[1] - source[0]);
			++source;

			*integral++ = previousIntegral + *integralPreviousRow++;
		}

		source += sourcePaddingElements + 1u; // including the last pixel
		integral += integralPaddingElements;
		integralPreviousRow += integralPaddingElements;
	}
}

template <typename T, typename TIntegral, bool tAbsoluteGradient>
void FrameFilterGradient::filterVertical2x1LinedIntegralImage(const T* source, const unsigned int width, const unsigned int height, TIntegral* integral, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements)
{
	ocean_assert(source != nullptr);
	ocean_assert(width >= 2u);
	ocean_assert(height >= 1u);
	ocean_assert(integral != nullptr);

	const unsigned int sourceStrideElements = width + sourcePaddingElements;

	/*
	 * This is the resulting lined integral image, with resolution (width + 1)x(height):
	 *  ------------
	 * |000000000000|
	 * |0|----------|
	 * |0|          |
	 * |0| Integral |
	 * |0|          |
	 * |------------
	 */

	typedef typename DifferenceValueTyper<TIntegral>::Type TSignedIntegral;

	// entire top line will be set to zero
	memset(integral, 0x00, (width + 1u) * sizeof(TIntegral));

#ifdef OCEAN_DEBUG
	for (unsigned int n = 0u; n < width + 1u; ++n)
	{
		ocean_assert(integral[n] == TIntegral(0));
	}
#endif

	integral += width + 1u + integralPaddingElements;

	// we calculate the first row of the integral image

	const T* const sourceFirstRowEnd = source + width;
	const T* const sourceEnd = source + sourceStrideElements * (height - 1u);  // excluding the last row

	const TIntegral* integralPreviousRow = integral;

	TIntegral previousIntegral = TIntegral(0);

	*integral++ = TIntegral(0);

	// the remaining pixels of the first row

	const T* sourceNextRow = source + sourceStrideElements;

	while (source != sourceFirstRowEnd)
	{
		previousIntegral += tAbsoluteGradient ? TIntegral(NumericT<TSignedIntegral>::abs(TSignedIntegral(*sourceNextRow++ - *source++))) : TIntegral(*sourceNextRow++ - *source++);

		*integral++ = previousIntegral;
	}

	source += sourcePaddingElements;
	sourceNextRow += sourcePaddingElements;
	integral += integralPaddingElements;

	// we calculate the remaining rows

	while (source != sourceEnd)
	{
		const T* const sourceRowEnd = source + width;

		previousIntegral = TIntegral(0);

		// left pixel
		*integral++ = TIntegral(0);

		++integralPreviousRow;

		while (source != sourceRowEnd)
		{
			previousIntegral += tAbsoluteGradient ? TIntegral(NumericT<TSignedIntegral>::abs(TSignedIntegral(*sourceNextRow++ - *source++))) : TIntegral(*sourceNextRow++ - *source++);

			*integral++ = previousIntegral + *integralPreviousRow++;
		}

		source += sourcePaddingElements;
		sourceNextRow += sourcePaddingElements;

		integral += integralPaddingElements;
		integralPreviousRow += integralPaddingElements;
	}
}

}

}

#endif // META_OCEAN_CV_FRAME_FILTER_GRADIENT_H
