/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_FILTER_GRADIENT_BASE_H
#define META_OCEAN_CV_FRAME_FILTER_GRADIENT_BASE_H

#include "ocean/cv/CV.h"

#include "ocean/base/DataType.h"
#include "ocean/base/Utilities.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements a gradient frame filter not explicitly using SIMD instructions.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameFilterGradientBase
{
	public:

		/**
		 * Horizontal and vertical gradient filter for zipped frames with arbitrary data type and arbitrary number of channels.
		 * @see FrameFilterGradient::filterHorizontalVertical1Channel().
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels, bool tNormalizeByTwo>
		static inline void filterHorizontalVertical(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const TTarget multiplicationFactor = TTarget(1), Worker* worker = nullptr);

		/**
		 * Squared magnitude gradient filter using the horizontal and vertical gradients that can be applied to zipped frames with arbitrary data type and arbitrary number of channels.
		 * @see FrameFilterGradient::filterHorizontalVerticalMagnitudeSquared().
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels, bool tNormalizeByTwo>
		static inline void filterHorizontalVerticalMagnitudeSquared(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const TTarget multiplicationFactor = TTarget(1), Worker* worker = nullptr);

		/**
		 * Applies the horizontal and vertical gradient filter to frames with arbitrary data type and arbitrary number of channels (the channels must be zipped) and forwards the horizontal and vertical filter responses to the specified template-function allowing to compute arbitrary results.
		 * This function invokes the template-function for each pixel and for each channel individually.
		 * @param source The source frame to be handled, must be valid
		 * @param target The target frame receiving the result from the template-function, with same frame dimension and channel number as the source frame
		 * @param width The width of the source (and target) frame in pixel, with range [3, infinity)
		 * @param height The height of the source (and target) frame in pixel, with range [3, infinity)
		 * @param multiplicationFactor Optional multiplication factor that is applied to each result of the template-function before the result is assigned to the target frame
		 * @param worker Optional worker object to distribute the computation
		 * @see applyPixelModifierSubset().
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels, typename TInnerSource, TTarget (*tPixelFunction)(const TInnerSource&, const TInnerSource&)>
		static void applyPixelModifier(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const TTarget multiplicationFactor, Worker* worker = nullptr);

		/**
		 * Horizontal and vertical gradient filter for a sub-frame of zipped frames and creating a filter response frame with frame dimension equal to the dimension of the sub-frame.
		 * @see FrameFilterGradient::filterHorizontalVerticalSubFrame().
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels, bool tNormalizeByTwo>
		static inline void filterHorizontalVerticalSubFrame(const TSource* source, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourcePaddingElements, const unsigned int sourceLeft, const unsigned int sourceTop, TTarget* target, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetPaddingElemnets, const TTarget multiplicationFactor = TTarget(1), Worker* worker = nullptr);

	protected:

		/**
		 * Horizontal and vertical gradient filter for a subset of frames with arbitrary data type and arbitrary number of channels.
		 * @see FrameFilterGradient::filterHorizontalVertical().
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels, bool tNormalizeByTwo>
		static void filterHorizontalVerticalSubset(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const TTarget multiplicationFactor, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Applies the horizontal and vertical gradient filter to a subset of frames with arbitrary data type and arbitrary number of channels (the channels must be zipped) and forwards the horizontal and vertical filter responses to the specified template-function allowing to compute arbitrary results.
		 * This function invokes the template-function for each pixel and for each channel individually.
		 * @param source The source frame to be handled, must be valid
		 * @param target The target frame receiving the result from the template-function, with same frame dimension and channel number as the source frame
		 * @param width The width of the source (and target) frame in pixel, with range [3, infinity)
		 * @param height The height of the source (and target) frame in pixel, with range [3, infinity)
		 * @param multiplicationFactor Optional multiplication factor that is applied to each result of the template-function before the result is assigned to the target frame
		 * @param firstRow The first row to be handled, with range [0, height)
		 * @param numberRows The number of rows to be handled, with range [1, height - firstRow]
		 * @see applyPixelModifier().
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels, typename TInnerSource, TTarget (*tPixelFunction)(const TInnerSource&, const TInnerSource&)>
		static void applyPixelModifierSubset(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const TTarget multiplicationFactor, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Horizontal and vertical gradient filter for a sub-frame of zipped frames and creating a filter response frame with frame dimension equal to the dimension of the sub-frame.
		 * @see FrameFilterGradient::filterHorizontalVerticalSubFrame().
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels, bool tNormalizeByTwo>
		static void filterHorizontalVerticalSubFrameSubset(const TSource* source, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourcePaddingElements, const unsigned int sourceLeft, const unsigned int sourceTop, TTarget* target, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetPaddingElements, const TTarget multiplicationFactor, const unsigned int firstTargetRow, const unsigned int numberTargetRows);

		/**
		 * Determines the squared magnitude of the horizontal and vertical gradient filter for one pixel.
		 * @param horizontal The horizontal gradient filter response
		 * @param vertical The vertical gradient filter response
		 * @return The resulting squared magnitude
		 * @tparam TInnerSource The data type of the horizontal and vertical filter responses
		 * @tparam TTarget The data type of the resulting magnitude
		 * @tparam tNormalizeByTwo True, to normalize the subtraction result by two; False, to simply determine the subtraction result
		 */
		template <typename TInnerSource, typename TTarget, bool tNormalizeByTwo>
		static TTarget horizontalVerticalMagnitudeSquared(const TInnerSource& horizontal, const TInnerSource& vertical);
};

template <typename TSource, typename TTarget, unsigned int tChannels, bool tNormalizeByTwo>
inline void FrameFilterGradientBase::filterHorizontalVertical(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const TTarget multiplicationFactor, Worker* worker)
{
	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(FrameFilterGradientBase::filterHorizontalVerticalSubset<TSource, TTarget, tChannels, tNormalizeByTwo>, source, target, width, height, sourcePaddingElements, targetPaddingElements, multiplicationFactor, 0u, 0u), 0u, height);
	}
	else
	{
		filterHorizontalVerticalSubset<TSource, TTarget, tChannels, tNormalizeByTwo>(source, target, width, height, sourcePaddingElements, targetPaddingElements, multiplicationFactor, 0u, height);
	}
}

template <typename TSource, typename TTarget, unsigned int tChannels, bool tNormalizeByTwo>
inline void FrameFilterGradientBase::filterHorizontalVerticalMagnitudeSquared(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const TTarget multiplicationFactor, Worker* worker)
{
	typedef typename NextLargerTyper<TSource>::TypePerformance TSourceLarger;
	typedef typename SignedTyper<TSourceLarger>::Type TSourceLargerSigned;

	applyPixelModifier<TSource, TTarget, tChannels, TSourceLargerSigned, horizontalVerticalMagnitudeSquared<TSourceLargerSigned, TTarget, tNormalizeByTwo>>(source, target, width, height, multiplicationFactor, worker);
}

template <typename TSource, typename TTarget, unsigned int tChannels, typename TInnerSource, TTarget (*tPixelFunction)(const TInnerSource&, const TInnerSource&)>
void FrameFilterGradientBase::applyPixelModifier(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const TTarget multiplicationFactor, Worker* worker)
{
	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(FrameFilterGradientBase::applyPixelModifierSubset<TSource, TTarget, tChannels, TInnerSource, tPixelFunction>, source, target, width, height, multiplicationFactor, 0u, 0u), 0u, height);
	}
	else
	{
		applyPixelModifierSubset<TSource, TTarget, tChannels, TInnerSource, tPixelFunction>(source, target, width, height, multiplicationFactor, 0u, height);
	}
}

template <typename TSource, typename TTarget, unsigned int tChannels, bool tNormalizeByTwo>
inline void FrameFilterGradientBase::filterHorizontalVerticalSubFrame(const TSource* source, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourcePaddingElements, const unsigned int sourceLeft, const unsigned int sourceTop, TTarget* target, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetPaddingElements, const TTarget multiplicationFactor, Worker* worker)
{
	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(FrameFilterGradientBase::filterHorizontalVerticalSubFrameSubset<TSource, TTarget, tChannels, tNormalizeByTwo>, source, sourceWidth, sourceHeight, sourcePaddingElements, sourceLeft, sourceTop, target, targetWidth, targetHeight, targetPaddingElements, multiplicationFactor, 0u, 0u), 0u, targetHeight);
	}
	else
	{
		filterHorizontalVerticalSubFrameSubset<TSource, TTarget, tChannels, tNormalizeByTwo>(source, sourceWidth, sourceHeight, sourcePaddingElements, sourceLeft, sourceTop, target, targetWidth, targetHeight, targetPaddingElements, multiplicationFactor, 0u, targetHeight);
	}
}

template <typename TSource, typename TTarget, unsigned int tChannels, bool tNormalizeByTwo>
void FrameFilterGradientBase::filterHorizontalVerticalSubset(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const TTarget multiplicationFactor, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invalid number of channels!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 3u && height >= 3u);

	ocean_assert(firstRow + numberRows <= height);

	typedef typename NextLargerTyper<TTarget>::TypePerformance TTargetLarger;

	const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * tChannels * 2u + targetPaddingElements;

	source += firstRow * sourceStrideElements;
	target += firstRow * targetStrideElements;

	if (firstRow == 0u)
	{
		// setting first row to zero

		std::fill(target, target + width * tChannels * 2u, TTarget(0));

		source += sourceStrideElements;
		target += targetStrideElements;
	}

	const unsigned int firstCoreRow = firstRow == 0u ? 1u : firstRow;
	const unsigned int endCoreRow = firstRow + numberRows == height ? height - 1u : firstRow + numberRows;

	const unsigned int width_1 = width - 1u;

	const TSource* sourceTop = source - sourceStrideElements;
	const TSource* sourceBottom = source + sourceStrideElements;

	for (unsigned int y = firstCoreRow; y < endCoreRow; ++y)
	{
		// setting first pixel to zero

		for (unsigned int n = 0u; n < tChannels * 2u; ++n)
		{
			target[n] = TTarget(0);
		}

		source += tChannels;
		sourceTop += tChannels;
		sourceBottom += tChannels;
		target += tChannels * 2u;

		if (multiplicationFactor == TTarget(1))
		{
			if constexpr (tNormalizeByTwo)
			{
				for (unsigned int x = 1u; x < width_1; ++x)
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						// horizontal [-1 0 1] / 2
						*target++ = TTarget(Utilities::divideBy2<TTargetLarger>(TTargetLarger(*(source + tChannels + n) - *(source - tChannels + n))));

						//          [-1]
						// vertical [ 0] / 2
						//          [ 1]
						*target++ = TTarget(Utilities::divideBy2<TTargetLarger>(TTargetLarger(*(sourceBottom + n) - *(sourceTop + n))));
					}

					source += tChannels;
					sourceTop += tChannels;
					sourceBottom += tChannels;
				}
			}
			else
			{
				for (unsigned int x = 1u; x < width_1; ++x)
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						// horizontal [-1 0 1]
						*target++ = TTarget(*(source + tChannels + n) - *(source - tChannels + n));

						//          [-1]
						// vertical [ 0]
						//          [ 1]
						*target++ = TTarget(*(sourceBottom + n) - *(sourceTop + n));
					}

					source += tChannels;
					sourceTop += tChannels;
					sourceBottom += tChannels;
				}
			}
		}
		else
		{
			if constexpr (tNormalizeByTwo)
			{
				for (unsigned int x = 1u; x < width_1; ++x)
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						// horizontal [-1 0 1] / 2
						*target++ = TTarget(Utilities::divideBy2<TTargetLarger>(TTargetLarger(*(source + tChannels + n) - *(source - tChannels + n)))) * multiplicationFactor;

						//          [-1]
						// vertical [ 0] / 2
						//          [ 1]
						*target++ = TTarget(Utilities::divideBy2<TTargetLarger>(TTargetLarger(*(sourceBottom + n) - *(sourceTop + n)))) * multiplicationFactor;
					}

					source += tChannels;
					sourceTop += tChannels;
					sourceBottom += tChannels;
				}
			}
			else
			{
				for (unsigned int x = 1u; x < width_1; ++x)
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						// horizontal [-1 0 1]
						*target++ = TTarget(*(source + tChannels + n) - *(source - tChannels + n)) * multiplicationFactor;

						//          [-1]
						// vertical [ 0]
						//          [ 1]
						*target++ = TTarget(*(sourceBottom + n) - *(sourceTop + n)) * multiplicationFactor;
					}

					source += tChannels;
					sourceTop += tChannels;
					sourceBottom += tChannels;
				}
			}
		}

		for (unsigned int n = 0u; n < tChannels * 2u; ++n)
		{
			target[n] = TTarget(0);
		}

		source += tChannels + sourcePaddingElements;
		sourceTop += tChannels + sourcePaddingElements;
		sourceBottom += tChannels + sourcePaddingElements;

		target += tChannels * 2u + targetPaddingElements;
	}

	if (firstRow + numberRows == height)
	{
		std::fill(target, target + width * tChannels * 2u, TTarget(0));
	}
}

template <typename TSource, typename TTarget, unsigned int tChannels, typename TInnerSource, TTarget (*tPixelFunction)(const TInnerSource&, const TInnerSource&)>
void FrameFilterGradientBase::applyPixelModifierSubset(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const TTarget multiplicationFactor, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invalid number of channels!");

	ocean_assert(source && target);
	ocean_assert(width >= 3u && height >= 3u);

	ocean_assert(firstRow + numberRows <= height);

	source += tChannels * (firstRow * width);
	target += tChannels * (firstRow * width);

	const TSource* const sourceEnd = source + tChannels * (width * (numberRows - ((firstRow + numberRows == height) ? 1u : 0u)));

	if (firstRow == 0u)
	{
		const TSource* const sourceRowEnd = source + width * tChannels;

		while (source != sourceRowEnd)
		{
			ocean_assert(source < sourceRowEnd);

			for (unsigned int n = 0u; n < tChannels; ++n)
				*target++ = TTarget(0);

			source += tChannels;
		}
	}

	while (source != sourceEnd)
	{
		ocean_assert(source < sourceEnd);

		for (unsigned int n = 0u; n < tChannels; ++n)
			*target++ = TTarget(0);

		source += tChannels;

		const TSource* const sourceRowEnd = source + (width - 2u) * tChannels;

		if (multiplicationFactor == TTarget(1))
		{
			while (source != sourceRowEnd)
			{
				ocean_assert(source < sourceEnd);
				ocean_assert(source < sourceRowEnd);

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					//                               [-1]
					// horizontal [-1 0 1], vertical [ 0]
					//                               [ 1]

					*target++ = tPixelFunction(TInnerSource(*(source + tChannels + n) - *(source - tChannels + n)), TInnerSource(*(source + width * tChannels + n) - *(source - width * tChannels + n)));
				}

				source += tChannels;
			}
		}
		else
		{
			while (source != sourceRowEnd)
			{
				ocean_assert(source < sourceEnd);
				ocean_assert(source < sourceRowEnd);

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					//                               [-1]
					// horizontal [-1 0 1], vertical [ 0]
					//                               [ 1]

					*target++ = tPixelFunction(TInnerSource(*(source + tChannels + n) - *(source - tChannels + n)), TInnerSource(*(source + width * tChannels + n) - *(source - width * tChannels + n))) * multiplicationFactor;
				}

				source += tChannels;
			}
		}

		for (unsigned int n = 0u; n < tChannels; ++n)
			*target++ = TTarget(0);

		source += tChannels;
	}

	if (firstRow + numberRows == height)
	{
		const TSource* const sourceRowEnd = source + width * tChannels;

		while (source != sourceRowEnd)
		{
			ocean_assert(source < sourceRowEnd);

			for (unsigned int n = 0u; n < tChannels; ++n)
				*target++ = TTarget(0);

			source += tChannels;
		}
	}
}

template <typename TSource, typename TTarget, unsigned int tChannels, bool tNormalizeByTwo>
void FrameFilterGradientBase::filterHorizontalVerticalSubFrameSubset(const TSource* source, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourcePaddingElements, const unsigned int sourceLeft, const unsigned int sourceTop, TTarget* target, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetPaddingElements, const TTarget multiplicationFactor, const unsigned int firstTargetRow, const unsigned int numberTargetRows)
{
	static_assert(tChannels >= 1u, "Invalid number of channels!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth >= 3u && sourceHeight >= 3u);

	ocean_assert(sourceLeft + targetWidth <= sourceWidth);
	ocean_assert(sourceTop + targetHeight <= sourceHeight);

	ocean_assert_and_suppress_unused(firstTargetRow + numberTargetRows <= targetHeight, targetHeight);

	typedef typename NextLargerTyper<TTarget>::TypePerformance TTargetLarger;

	const unsigned int sourceStrideElements = sourceWidth * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * tChannels * 2u + targetPaddingElements;

	source += (firstTargetRow + sourceTop) * sourceStrideElements + sourceLeft * tChannels;
	target += firstTargetRow * targetStrideElements;

	const bool firstTargetRowAtBorder = firstTargetRow == 0u && sourceTop == 0u;
	const bool firstTargetColumnAtBorder = sourceLeft == 0u;

	const bool lastTargetRowAtBorder = sourceTop + firstTargetRow + numberTargetRows == sourceHeight;
	const bool lastTargetColumnAtBorder = sourceLeft + targetWidth == sourceWidth;

	if (firstTargetRowAtBorder)
	{
		// the sub-frame is located at the top border of the source frame, so we still need to set the first response row to zero

		std::fill(target, target + targetWidth * tChannels * 2u, TTarget(0));

		source += sourceStrideElements;
		target += targetStrideElements;
	}

	const unsigned int firstCoreTargetRow = firstTargetRowAtBorder ? 1u : firstTargetRow;
	const unsigned int endCoreTargetRow = lastTargetRowAtBorder ? targetHeight - 1u : firstTargetRow + numberTargetRows;

	const unsigned int targetCoreWidth = targetWidth - (firstTargetColumnAtBorder ? 1u : 0u) - (lastTargetColumnAtBorder ? 1u : 0u);
	const unsigned int sourceOffsetElements = sourceStrideElements - targetWidth * tChannels;

	const TSource* sourceMinus = source - sourceStrideElements;
	const TSource* sourcePlus = source + sourceStrideElements;

	for (unsigned int y = firstCoreTargetRow; y < endCoreTargetRow; ++y)
	{
		if (firstTargetColumnAtBorder)
		{
			// setting first pixel to zero

			std::fill(target, target + tChannels * 2u, TTarget(0));

			source += tChannels;
			sourceMinus += tChannels;
			sourcePlus += tChannels;

			target += tChannels * 2u;
		}

		if (multiplicationFactor == TTarget(1))
		{
			if constexpr (tNormalizeByTwo)
			{
				for (unsigned int x = 0u; x < targetCoreWidth; ++x)
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						// horizontal [-1 0 1] / 2
						*target++ = TTarget(Utilities::divideBy2<TTargetLarger>(TTargetLarger(*(source + tChannels + n) - *(source - tChannels + n))));

						//          [-1]
						// vertical [ 0] / 2
						//          [ 1]
						*target++ = TTarget(Utilities::divideBy2<TTargetLarger>(TTargetLarger(*(sourcePlus + n) - *(sourceMinus + n))));
					}

					source += tChannels;
					sourceMinus += tChannels;
					sourcePlus += tChannels;
				}
			}
			else
			{
				for (unsigned int x = 0u; x < targetCoreWidth; ++x)
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						// horizontal [-1 0 1]
						*target++ = TTarget(*(source + tChannels + n) - *(source - tChannels + n));

						//          [-1]
						// vertical [ 0]
						//          [ 1]
						*target++ = TTarget(*(sourcePlus + n) - *(sourceMinus + n));
					}

					source += tChannels;
					sourceMinus += tChannels;
					sourcePlus += tChannels;
				}
			}
		}
		else
		{
			if constexpr (tNormalizeByTwo)
			{
				for (unsigned int x = 0u; x < targetCoreWidth; ++x)
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						// horizontal [-1 0 1] / 2
						*target++ = TTarget(Utilities::divideBy2<TTargetLarger>(TTargetLarger(*(source + tChannels + n) - *(source - tChannels + n)))) * multiplicationFactor;

						//          [-1]
						// vertical [ 0] / 2
						//          [ 1]
						*target++ = TTarget(Utilities::divideBy2<TTargetLarger>(TTargetLarger(*(sourcePlus + n) - *(sourceMinus + n)))) * multiplicationFactor;
					}

					source += tChannels;
					sourceMinus += tChannels;
					sourcePlus += tChannels;
				}
			}
			else
			{
				for (unsigned int x = 0u; x < targetCoreWidth; ++x)
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						// horizontal [-1 0 1]
						*target++ = TTarget(*(source + tChannels + n) - *(source - tChannels + n)) * multiplicationFactor;

						//          [-1]
						// vertical [ 0]
						//          [ 1]
						*target++ = TTarget(*(sourcePlus + n) - *(sourceMinus + n)) * multiplicationFactor;
					}

					source += tChannels;
					sourceMinus += tChannels;
					sourcePlus += tChannels;
				}
			}
		}

		if (lastTargetColumnAtBorder)
		{
			// setting last pixel to zero

			std::fill(target, target + tChannels * 2u, TTarget(0));

			source += tChannels;
			sourceMinus += tChannels;
			sourcePlus += tChannels;

			target += tChannels * 2u;
		}

		source += sourceOffsetElements;
		sourceMinus += sourceOffsetElements;
		sourcePlus += sourceOffsetElements;

		target += targetPaddingElements;
	}

	if (lastTargetRowAtBorder)
	{
		// the sub-frame is located at the bottom border of the source frame, so we still need to set the first response row to zero

		std::fill(target, target + targetWidth * tChannels * 2u, TTarget(0));
	}
}

template <typename TInnerSource, typename TTarget, bool tNormalizeByTwo>
TTarget FrameFilterGradientBase::horizontalVerticalMagnitudeSquared(const TInnerSource& horizontal, const TInnerSource& vertical)
{
	if constexpr (tNormalizeByTwo)
	{
		const TInnerSource nHorizontal(Utilities::divideBy2<TInnerSource>(horizontal));
		const TInnerSource nVertical(Utilities::divideBy2<TInnerSource>(vertical));

		return TTarget(nHorizontal * nHorizontal + nVertical * nVertical);
	}
	else
	{
		return TTarget(horizontal * horizontal + vertical * vertical);
	}
}

}

}

#endif // META_OCEAN_CV_FRAME_FILTER_GRADIENT_BASE_H
