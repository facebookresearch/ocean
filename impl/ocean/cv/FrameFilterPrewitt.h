/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_FILTER_PREWITT_H
#define META_OCEAN_CV_FRAME_FILTER_PREWITT_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameFilter.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements a Prewitt frame filter.
 * The horizontal (0 degree) and vertical (90 degree - clockwise) 3x3 Prewitt box filter (not convolution filter) are defined as:
 * <pre>
 * horizontal:      vertical (90 degree):
 * | -1  0  1 |     | -1  -1  -1 |
 * | -1  0  1 |     |  0   0   0 |
 * | -1  0  1 |     |  1   1   1 |
 * </pre>
 *
 * The diagonal 3x3 Prewitt filters are defined as:
 * <pre>
 * 45 degree:        135 degree:
 * | -1  -1   0 |    | 0  -1  -1 |
 * | -1   0   1 |    | 1   0  -1 |
 * |  0   1   1 |    | 1   1   0 |
 * </pre>
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameFilterPrewitt : public FrameFilter
{
	public:

		/**
		 * Horizontal and vertical Prewitt filter for images.
		 * If the target response data type is selected to be `int8_t`, each filter response  is normalized by 1/6 to fit into the value range [-128, 127].<br>
		 * If the target response data type is selected to be `int16_t` no normalization will be applied.
		 * The border pixels are set to zero.
		 * @param source The source frame to which the Prewitt filter will be applied, with `tSourceChannels` channels, must be valid
		 * @param target The target response frame receiving the horizontal and vertical Prewitt responses, with `tSourceChannels * 2` channels, must be valid
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param sourcePaddingElements Optional padding at the end of each source row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam TTarget The data type of the response values, either `int8_t` or `int16_t`
		 * @tparam tSourceChannels The number of channels of the source frame, with range [1, infinity)
		 */
		template <typename TTarget, unsigned int tSourceChannels>
		static inline void filterHorizontalVertical8BitPerChannel(const uint8_t* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

	private:

		/**
		 * Applies the horizontal and vertical Prewitt filter to one row of a source frame.
		 * @param sourceRow The row of the source frame, must be valid
		 * @param targetRow The row of the target response frame, must be valid
		 * @param width The width of the source and target frame in pixel, with range [3, infinity)
		 * @param height The height of the source and target frame in pixel, with range [3, infinity)
		 * @param rowIndex The index of the row to which the filter is applied, with range [0, height - 1]
		 * @param sourceStrideElements The number of elements between the start of two consecutive source rows, with range [width * tSourceChannels, infinity)
		 * @param targetStrideElements The number of elements between the start of two consecutive target rows, with range [width * tTargetChannels, infinity)
		 * @tparam TSource The data type of the source frame, must be `uint8_t`
		 * @tparam TTarget The data type oft he target response frame, must be `int8_t` or `int16_t`
		 * @tparam tSourceChannels The number of channels the source frame has, with range [1, infinity)
		 * @tparam tTargetChannels The number of channels the target frame has, must be `tSourceChannels * 2`
		 */
		template <typename TSource, typename TTarget, unsigned int tSourceChannels, unsigned int tTargetChannels>
		static void filterHorizontalVerticalRow(const TSource* sourceRow, TTarget* targetRow, const unsigned int width, const unsigned int height, unsigned int rowIndex, const unsigned int sourceStrideElements, const unsigned int targetStrideElements);
};

template <typename TTarget, unsigned int tSourceChannels>
inline void FrameFilterPrewitt::filterHorizontalVertical8BitPerChannel(const uint8_t* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value, "Invalid target data type!");
	static_assert(tSourceChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 3u && height >= 3u);

	//using TSource = uint8_t;
	constexpr unsigned int tTargetChannels = tSourceChannels * 2u;

	FrameChannels::applyRowOperator<uint8_t, TTarget, tSourceChannels, tTargetChannels>(source, target, width, height, sourcePaddingElements, targetPaddingElements, &filterHorizontalVerticalRow<uint8_t, TTarget, tSourceChannels, tTargetChannels>, worker);
}

template <typename TSource, typename TTarget, unsigned int tSourceChannels, unsigned int tTargetChannels>
void FrameFilterPrewitt::filterHorizontalVerticalRow(const TSource* sourceRow, TTarget* targetRow, const unsigned int width, const unsigned int height, unsigned int rowIndex, const unsigned int sourceStrideElements, const unsigned int /*targetStrideElements*/)
{
	static_assert(std::is_same<TSource, uint8_t>::value, "Invalid source data type!");
	static_assert(std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value, "Invalid target data type!");

	static_assert(tSourceChannels >= 1u, "Invalid source channel number!");
	static_assert(tTargetChannels == tSourceChannels * 2u, "Invalid target channel number!");

	ocean_assert(width >= 3u && height >= 3u);

	if (rowIndex == 0u || rowIndex == height - 1u)
	{
		// setting the first row and last row to zero

		memset(targetRow, 0, width * tTargetChannels * sizeof(TTarget));
		return;
	}

	// setting first pixel to zero

	for (unsigned int n = 0u; n < tTargetChannels; ++n)
	{
		targetRow[n] = TTarget(0);
	}

	targetRow += tTargetChannels;

	const uint8_t* source0 = sourceRow - sourceStrideElements;
	const uint8_t* source1 = sourceRow;
	const uint8_t* source2 = sourceRow + sourceStrideElements;

	for (unsigned int x = 1u; x < width - 1u; ++x)
	{
		if constexpr (std::is_same<TTarget, int8_t>::value)
		{
			for (unsigned int n = 0u; n < tSourceChannels; ++n)
			{
				// | -1 0 1 |
				// | -1 0 1 |
				// | -1 0 1 |
				*targetRow++ = TTarget((*(source0 + tSourceChannels * 2u) - *(source0) + (*(source1 + tSourceChannels * 2u) - *(source1)) + *(source2 + tSourceChannels * 2u) - *(source2)) / 8);

				// | -1 -1 -1 |
				// |  0  0  0 |
				// |  1  1  1 |
				*targetRow++ = TTarget((*(source2) + (*(source2 + tSourceChannels) - *(source0 + tSourceChannels)) + *(source2 + tSourceChannels * 2u) - *(source0) - *(source0 + tSourceChannels * 2u)) / 8);

				++source0;
				++source1;
				++source2;
			}
		}
		else
		{
			ocean_assert((std::is_same<TTarget, int16_t>::value));

			for (unsigned int n = 0u; n < tSourceChannels; ++n)
			{
				// | -1 0 1 |
				// | -1 0 1 |
				// | -1 0 1 |
				*targetRow++ = TTarget(*(source0 + tSourceChannels * 2u) - *(source0) + (*(source1 + tSourceChannels * 2u) - *(source1)) + *(source2 + tSourceChannels * 2u) - *(source2));

				// | -1 -1 -1 |
				// |  0  0  0 |
				// |  1  1  1 |
				*targetRow++ = TTarget(*(source2) + (*(source2 + tSourceChannels) - *(source0 + tSourceChannels)) + *(source2 + tSourceChannels * 2u) - *(source0) - *(source0 + tSourceChannels * 2u));

				++source0;
				++source1;
				++source2;
			}
		}
	}

	// setting last pixel to zero
	for (unsigned int n = 0u; n < tTargetChannels; ++n)
	{
		targetRow[n] = TTarget(0);
	}
}

}

}

#endif // META_OCEAN_CV_FRAME_FILTER_PREWITT_H
