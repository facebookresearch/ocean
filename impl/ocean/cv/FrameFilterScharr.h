/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_FILTER_SCHARR_H
#define META_OCEAN_CV_FRAME_FILTER_SCHARR_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameChannels.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements a Scharr filter.
 * The horizontal (0 degree) and vertical (90 degree - clockwise) 3x3 Scharr filter (not the convolution filter) are defined as:
 * <pre>
 * horizontal:         vertical (90 degree):
 * |  -3   0   3 |     | -3  -10  -3 |
 * | -10   0  10 |     |  0    0   0 |
 * |  -3   0   3 |     |  3   10   3 |
 * </pre>
 *
 * The diagonal 3x3 Scharr filters are defined as:
 * <pre>
 * 45 degree:             135 degree:
 * |  -10   -3    0 |     |  0   -3   -10 |
 * |  -3     0    3 |     |  3    0    -3 |
 * |   0     3   10 |     | 10    3     0 |
 * </pre>
 * @see FrameFilterScharrMagnitude, FrameFilterSobel.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameFilterScharr
{
	public:

		/**
		 * The following comfort class provides comfortable functions simplifying prototyping applications but also increasing binary size of the resulting applications.
		 * Best practice is to avoid using these functions if binary size matters,<br>
		 * as for every comfort function a corresponding function exists with specialized functionality not increasing binary size significantly.<br>
		 */
		class OCEAN_CV_EXPORT Comfort
		{
			public:

				/**
				 * Horizontal and vertical Scharr filer for images.
				 * The target frame must be valid, must have data type 'DT_SIGNED_INTEGER_8` or `DT_SIGNED_INTEGER_16`.
				 * @param source The source frame to filter, must be valid
				 * @param target The target frame receiving the filtered frame, must be valid
				 * @param worker Optional worker object to distribute the computational load
				 * @return True, if succeeded
				 */
				static bool filterHorizontalVertical(const Frame& source, Frame& target, Worker* worker = nullptr);

				/**
				 * Diagonal (45 and 135 degree) Scharr filer for images.
				 * The target frame must be valid, must have data type 'DT_SIGNED_INTEGER_8` or `DT_SIGNED_INTEGER_16`.
				 * @param source The source frame to filter, must be valid
				 * @param target The target frame receiving the filtered frame, must be valid
				 * @param worker Optional worker object to distribute the computational load
				 * @return True, if succeeded
				 */
				static bool filterDiagonal(const Frame& source, Frame& target, Worker* worker = nullptr);

				/**
				 * Horizontal, vertical, and diagonal Scharr filer for images.
				 * The target frame must be valid, must have data type 'DT_SIGNED_INTEGER_8` or `DT_SIGNED_INTEGER_16`.
				 * @param source The source frame to filter, must be valid
				 * @param target The target frame receiving the filtered frame, must be valid
				 * @param worker Optional worker object to distribute the computational load
				 * @return True, if succeeded
				 */
				static bool filter(const Frame& source, Frame& target, Worker* worker = nullptr);

				/**
				 * Determines the maximum of the absolute horizontal and vertical Scharr filter.
				 * The target frame must be valid, must have data type 'DT_UNSIGNED_INTEGER_8` or `DT_UNSIGNED_INTEGER_16`.
				 * @param source The source frame to filter, must be valid
				 * @param target The target frame receiving the filtered frame, must be valid
				 * @param worker Optional worker object to distribute the computational load
				 * @return True, if succeeded
				 */
				static bool filterHorizontalVerticalMaximumAbsolute(const Frame& source, Frame& target, Worker* worker = nullptr);

				/**
				 * Determines the maximum of the absolute horizontal, vertical, and diagonal Scharr filter.
				 * The target frame must be valid, must have data type 'DT_UNSIGNED_INTEGER_8` or `DT_UNSIGNED_INTEGER_16`.
				 * @param source The source frame to filter, must be valid
				 * @param target The target frame receiving the filtered frame, must be valid
				 * @param worker Optional worker object to distribute the computational load
				 * @return True, if succeeded
				 */
				static bool filterMaximumAbsolute(const Frame& source, Frame& target, Worker* worker = nullptr);
		};

	public:

		/**
		 * Horizontal and vertical Scharr filer for images.
		 * If the target response data type is selected to be `int8_t`, each filter response  is normalized by 1/32 to fit into the value range [-128, 127].<br>
		 * If the target response data type is selected to be `int16_t` no normalization will be applied.
		 * The border pixels are set to zero.
		 * @param source The source frame to which the Scharr filter will be applied, with `tSourceChannels` channels, must be valid
		 * @param target The target response frame receiving the horizontal and vertical Scharr responses, with `tSourceChannels * 2` channels, must be valid
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

		/**
		 * Diagonal (45 and 135 degree) Scharr filer for images.
		 * If the target response data type is selected to be `int8_t`, each filter response  is normalized by 1/32 to fit into the value range [-128, 127].<br>
		 * If the target response data type is selected to be `int16_t` no normalization will be applied.
		 * The border pixels are set to zero.
		 * @param source The source frame to which the Scharr filter will be applied, with `tSourceChannels` channels, must be valid
		 * @param target The target response frame receiving the 45 diagonal and 135 diagonal Scharr responses, with `tSourceChannels * 2` channels, must be valid
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param sourcePaddingElements Optional padding at the end of each source row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam TTarget The data type of the response values, either `int8_t` or `int16_t`
		 * @tparam tSourceChannels The number of channels of the source frame, with range [1, infinity)
		 */
		template <typename TTarget, unsigned int tSourceChannels>
		static inline void filterDiagonal8BitPerChannel(const uint8_t* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Horizontal, vertical, and diagonal Scharr filer for images.
		 * If the target response data type is selected to be `int8_t`, each filter response  is normalized by 1/32 to fit into the value range [-128, 127].<br>
		 * If the target response data type is selected to be `int16_t` no normalization will be applied.
		 * The border pixels are set to zero.
		 * @param source The source frame to which the Scharr filter will be applied, with `tSourceChannels` channels, must be valid
		 * @param target The target response frame receiving the horizontal, vertical, and both diagonal Scharr responses, with `tSourceChannels * 4` channels, must be valid
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param sourcePaddingElements Optional padding at the end of each source row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam TTarget The data type of the response values, either `int8_t` or `int16_t`
		 * @tparam tSourceChannels The number of channels of the source frame, with range [1, infinity)
		 */
		template <typename TTarget, unsigned int tSourceChannels>
		static inline void filter8BitPerChannel(const uint8_t* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Determines the maximum of the absolute horizontal and vertical Scharr filter.
		 * If the target response data type is selected to be `uint8_t`, each filter response  is normalized by 1/16 to fit into the value range [0, 255].<br>
		 * If the target response data type is selected to be `uint16_t` no normalization will be applied.
		 * The border pixels are set to zero.
		 * @param source The source frame to which the Scharr filter will be applied, with `tSourceChannels` channels, must be valid
		 * @param target The target response frame receiving the maximal Scharr responses, with `tSourceChannels` channels, must be valid
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param sourcePaddingElements Optional padding at the end of each source row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam TTarget The data type of the response values, either `uint8_t` or `uint16_t`
		 * @tparam tSourceChannels The number of channels of the source frame, with range [1, infinity)
		 */
		template <typename TTarget, unsigned int tSourceChannels>
		static inline void filterHorizontalVerticalMaximumAbsolute8BitPerChannel(const uint8_t* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Determines the maximum of the absolute horizontal, vertical, and diagonal Scharr filter.
		 * If the target response data type is selected to be `uint8_t`, each filter response  is normalized by 1/16 to fit into the value range [0, 255].<br>
		 * If the target response data type is selected to be `uint16_t` no normalization will be applied.
		 * The border pixels are set to zero.
		 * @param source The source frame to which the Scharr filter will be applied, with `tSourceChannels` channels, must be valid
		 * @param target The target response frame receiving the maximal Scharr responses, with `tSourceChannels` channels, must be valid
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param sourcePaddingElements Optional padding at the end of each source row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam TTarget The data type of the response values, either `uint8_t` or `uint16_t`
		 * @tparam tSourceChannels The number of channels of the source frame, with range [1, infinity)
		 */
		template <typename TTarget, unsigned int tSourceChannels>
		static inline void filterMaximumAbsolute8BitPerChannel(const uint8_t* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

	private:

		/**
		 * Applies the horizontal and vertical Scharr filter to one row of a source frame.
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

		/**
		 * Applies the diagonal Scharr filter to one row of a source frame.
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
		static void filterDiagonalRow(const TSource* sourceRow, TTarget* targetRow, const unsigned int width, const unsigned int height, unsigned int rowIndex, const unsigned int sourceStrideElements, const unsigned int targetStrideElements);

		/**
		 * Applies the horizontal, vertical, and diagonal Scharr filter to one row of a source frame.
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
		static void filterRow(const TSource* sourceRow, TTarget* targetRow, const unsigned int width, const unsigned int height, unsigned int rowIndex, const unsigned int sourceStrideElements, const unsigned int targetStrideElements);

		/**
		 * Applies the maximum of the absolute horizontal and vertical Scharr filter to one row of a source frame.
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
		static void filterHorizontalVerticalMaximumAbsoluteRow(const TSource* sourceRow, TTarget* targetRow, const unsigned int width, const unsigned int height, unsigned int rowIndex, const unsigned int sourceStrideElements, const unsigned int targetStrideElements);

		/**
		 * Applies the maximum of the absolute horizontal, vertical, and diagonal Scharr filter to one row of a source frame.
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
		static void filterMaximumAbsoluteRow(const TSource* sourceRow, TTarget* targetRow, const unsigned int width, const unsigned int height, unsigned int rowIndex, const unsigned int sourceStrideElements, const unsigned int targetStrideElements);
};

template <typename TTarget, unsigned int tSourceChannels>
inline void FrameFilterScharr::filterHorizontalVertical8BitPerChannel(const uint8_t* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value, "Invalid target data type!");
	static_assert(tSourceChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 3u && height >= 3u);

	//using TSource = uint8_t;
	constexpr unsigned int tTargetChannels = tSourceChannels * 2u;

	FrameChannels::applyRowOperator<uint8_t, TTarget, tSourceChannels, tTargetChannels>(source, target, width, height, sourcePaddingElements, targetPaddingElements, &filterHorizontalVerticalRow<uint8_t, TTarget, tSourceChannels, tTargetChannels>, worker);
}

template <typename TTarget, unsigned int tSourceChannels>
inline void FrameFilterScharr::filterDiagonal8BitPerChannel(const uint8_t* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value, "Invalid target data type!");
	static_assert(tSourceChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 3u && height >= 3u);

	//using TSource = uint8_t;
	constexpr unsigned int tTargetChannels = tSourceChannels * 2u;

	FrameChannels::applyRowOperator<uint8_t, TTarget, tSourceChannels, tTargetChannels>(source, target, width, height, sourcePaddingElements, targetPaddingElements, &filterDiagonalRow<uint8_t, TTarget, tSourceChannels, tTargetChannels>, worker);
}

template <typename TTarget, unsigned int tSourceChannels>
inline void FrameFilterScharr::filter8BitPerChannel(const uint8_t* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value, "Invalid target data type!");
	static_assert(tSourceChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 3u && height >= 3u);

	//using TSource = uint8_t;
	constexpr unsigned int tTargetChannels = tSourceChannels * 4u;

	FrameChannels::applyRowOperator<uint8_t, TTarget, tSourceChannels, tTargetChannels>(source, target, width, height, sourcePaddingElements, targetPaddingElements, &filterRow<uint8_t, TTarget, tSourceChannels, tTargetChannels>, worker);
}

template <typename TTarget, unsigned int tSourceChannels>
inline void FrameFilterScharr::filterHorizontalVerticalMaximumAbsolute8BitPerChannel(const uint8_t* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(std::is_same<TTarget, uint8_t>::value || std::is_same<TTarget, uint16_t>::value, "Invalid target data type!");
	static_assert(tSourceChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 3u && height >= 3u);

	//using TSource = uint8_t;
	constexpr unsigned int tTargetChannels = tSourceChannels;

	FrameChannels::applyRowOperator<uint8_t, TTarget, tSourceChannels, tTargetChannels>(source, target, width, height, sourcePaddingElements, targetPaddingElements, &filterHorizontalVerticalMaximumAbsoluteRow<uint8_t, TTarget, tSourceChannels, tTargetChannels>, worker);
}

template <typename TTarget, unsigned int tSourceChannels>
inline void FrameFilterScharr::filterMaximumAbsolute8BitPerChannel(const uint8_t* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(std::is_same<TTarget, uint8_t>::value || std::is_same<TTarget, uint16_t>::value, "Invalid target data type!");
	static_assert(tSourceChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 3u && height >= 3u);

	//using TSource = uint8_t;
	constexpr unsigned int tTargetChannels = tSourceChannels;

	FrameChannels::applyRowOperator<uint8_t, TTarget, tSourceChannels, tTargetChannels>(source, target, width, height, sourcePaddingElements, targetPaddingElements, &filterMaximumAbsoluteRow<uint8_t, TTarget, tSourceChannels, tTargetChannels>, worker);
}

template <typename TSource, typename TTarget, unsigned int tSourceChannels, unsigned int tTargetChannels>
void FrameFilterScharr::filterHorizontalVerticalRow(const TSource* sourceRow, TTarget* targetRow, const unsigned int width, const unsigned int height, unsigned int rowIndex, const unsigned int sourceStrideElements, const unsigned int /*targetStrideElements*/)
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

	const uint8_t* source0 = sourceRow - sourceStrideElements;
	const uint8_t* source1 = sourceRow;
	const uint8_t* source2 = sourceRow + sourceStrideElements;

	// setting first pixel to zero

	for (unsigned int n = 0u; n < tTargetChannels; ++n)
	{
		targetRow[n] = TTarget(0);
	}

	targetRow += tTargetChannels;

	for (unsigned int x = 1u; x < width - 1u; ++x)
	{
		if constexpr (std::is_same<TTarget, int8_t>::value)
		{
			for (unsigned int n = 0u; n < tSourceChannels; ++n)
			{
				// 0 degree filter
				// |  -3   0   3 |
				// | -10   0  10 | * 1/32
				// |  -3   0   3 |
				*targetRow++ = TTarget((3 * (*(source0 + tSourceChannels * 2u) - *(source0) + *(source2 + tSourceChannels * 2u) - *(source2)) + 10 * (*(source1 + tSourceChannels * 2u) - *(source1))) / 32);

				// 90 degree filter
				// | -3  -10  -3 |
				// |  0   0    0 | * 1/32
				// |  3   10   3 |
				*targetRow++ = TTarget((3 * (*(source2) + *(source2 + tSourceChannels * 2u) - *(source0) - *(source0 + tSourceChannels * 2u)) + 10 * (*(source2 + tSourceChannels) - *(source0 + tSourceChannels))) / 32);

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
				// 0 degree filter
				// |  -3   0   3 |
				// | -10   0  10 |
				// |  -3   0   3 |
				*targetRow++ = TTarget(3 * (*(source0 + tSourceChannels * 2u) - *(source0) + *(source2 + tSourceChannels * 2u) - *(source2)) + 10 * (*(source1 + tSourceChannels * 2u) - *(source1)));

				// 90 degree filter
				// | -3  -10  -3 |
				// |  0   0    0 |
				// |  3   10   3 |
				*targetRow++ = TTarget(3 * (*(source2) + *(source2 + tSourceChannels * 2u) - *(source0) - *(source0 + tSourceChannels * 2u)) + 10 * (*(source2 + tSourceChannels) - *(source0 + tSourceChannels)));

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

template <typename TSource, typename TTarget, unsigned int tSourceChannels, unsigned int tTargetChannels>
void FrameFilterScharr::filterDiagonalRow(const TSource* sourceRow, TTarget* targetRow, const unsigned int width, const unsigned int height, unsigned int rowIndex, const unsigned int sourceStrideElements, const unsigned int /*targetStrideElements*/)
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

	const uint8_t* source0 = sourceRow - sourceStrideElements;
	const uint8_t* source1 = sourceRow;
	const uint8_t* source2 = sourceRow + sourceStrideElements;

	// setting first pixel to zero

	for (unsigned int n = 0u; n < tTargetChannels; ++n)
	{
		targetRow[n] = TTarget(0);
	}

	targetRow += tTargetChannels;

	for (unsigned int x = 1u; x < width - 1u; ++x)
	{
		if constexpr (std::is_same<TTarget, int8_t>::value)
		{
			for (unsigned int n = 0u; n < tSourceChannels; ++n)
			{
				// 45 degree filter
				// |  -10   -3    0 |
				// |  -3     0    3 | * 1/32
				// |   0     3   10 |
				*targetRow++ = TTarget((3 * (*(source1 + tSourceChannels * 2u) + *(source2 + tSourceChannels) - *(source0 + tSourceChannels) - *(source1)) + 10 * (*(source2 + tSourceChannels * 2u) - *(source0))) / 32);

				// 135 degree filter
				// |  0   -3   -10 |
				// |  3    0    -3 | * 1/32
				// | 10    3     0 |
				*targetRow++ = TTarget((3 * (*(source1) + *(source2 + tSourceChannels) - *(source0 + tSourceChannels) - *(source1 + tSourceChannels * 2u)) + 10 * (*(source2) - *(source0 + tSourceChannels * 2u))) / 32);

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
				// 45 degree filter
				// |  -10   -3    0 |
				// |  -3     0    3 | * 1/32
				// |   0     3   10 |
				*targetRow++ = TTarget(3 * (*(source1 + tSourceChannels * 2u) + *(source2 + tSourceChannels) - *(source0 + tSourceChannels) - *(source1)) + 10 * (*(source2 + tSourceChannels * 2u) - *(source0)));

				// 135 degree filter
				// |  0   -3   -10 |
				// |  3    0    -3 | * 1/32
				// | 10    3     0 |
				*targetRow++ = TTarget(3 * (*(source1) + *(source2 + tSourceChannels) - *(source0 + tSourceChannels) - *(source1 + tSourceChannels * 2u)) + 10 * (*(source2) - *(source0 + tSourceChannels * 2u)));

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

template <typename TSource, typename TTarget, unsigned int tSourceChannels, unsigned int tTargetChannels>
void FrameFilterScharr::filterRow(const TSource* sourceRow, TTarget* targetRow, const unsigned int width, const unsigned int height, unsigned int rowIndex, const unsigned int sourceStrideElements, const unsigned int /*targetStrideElements*/)
{
	static_assert(std::is_same<TSource, uint8_t>::value, "Invalid source data type!");
	static_assert(std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value, "Invalid target data type!");

	static_assert(tSourceChannels >= 1u, "Invalid source channel number!");
	static_assert(tTargetChannels == tSourceChannels * 4u, "Invalid target channel number!");

	ocean_assert(width >= 3u && height >= 3u);

	if (rowIndex == 0u || rowIndex == height - 1u)
	{
		// setting the first row and last row to zero

		memset(targetRow, 0, width * tTargetChannels * sizeof(TTarget));
		return;
	}

	const uint8_t* source0 = sourceRow - sourceStrideElements;
	const uint8_t* source1 = sourceRow;
	const uint8_t* source2 = sourceRow + sourceStrideElements;

	// setting first pixel to zero

	for (unsigned int n = 0u; n < tTargetChannels; ++n)
	{
		targetRow[n] = TTarget(0);
	}

	targetRow += tTargetChannels;

	for (unsigned int x = 1u; x < width - 1u; ++x)
	{
		if constexpr (std::is_same<TTarget, int8_t>::value)
		{
			for (unsigned int n = 0u; n < tSourceChannels; ++n)
			{
				// 0 degree filter
				// |  -3   0   3 |
				// | -10   0  10 | * 1/32
				// |  -3   0   3 |
				*targetRow++ = TTarget((3 * (*(source0 + tSourceChannels * 2u) - *(source0) + *(source2 + tSourceChannels * 2u) - *(source2)) + 10 * (*(source1 + tSourceChannels * 2u) - *(source1))) / 32);

				// 90 degree filter
				// | -3  -10  -3 |
				// |  0   0    0 | * 1/32
				// |  3   10   3 |
				*targetRow++ = TTarget((3 * (*(source2) + *(source2 + tSourceChannels * 2u) - *(source0) - *(source0 + tSourceChannels * 2u)) + 10 * (*(source2 + tSourceChannels) - *(source0 + tSourceChannels))) / 32);

				// 45 degree filter
				// |  -10   -3    0 |
				// |  -3     0    3 | * 1/32
				// |   0     3   10 |
				*targetRow++ = TTarget((3 * (*(source1 + tSourceChannels * 2u) + *(source2 + tSourceChannels) - *(source0 + tSourceChannels) - *(source1)) + 10 * (*(source2 + tSourceChannels * 2u) - *(source0))) / 32);

				// 135 degree filter
				// |  0   -3   -10 |
				// |  3    0    -3 | * 1/32
				// | 10    3     0 |
				*targetRow++ = TTarget((3 * (*(source1) + *(source2 + tSourceChannels) - *(source0 + tSourceChannels) - *(source1 + tSourceChannels * 2u)) + 10 * (*(source2) - *(source0 + tSourceChannels * 2u))) / 32);

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
				// 0 degree filter
				// |  -3   0   3 |
				// | -10   0  10 |
				// |  -3   0   3 |
				*targetRow++ = TTarget(3 * (*(source0 + tSourceChannels * 2u) - *(source0) + *(source2 + tSourceChannels * 2u) - *(source2)) + 10 * (*(source1 + tSourceChannels * 2u) - *(source1)));

				// 90 degree filter
				// | -3  -10  -3 |
				// |  0   0    0 |
				// |  3   10   3 |
				*targetRow++ = TTarget(3 * (*(source2) + *(source2 + tSourceChannels * 2u) - *(source0) - *(source0 + tSourceChannels * 2u)) + 10 * (*(source2 + tSourceChannels) - *(source0 + tSourceChannels)));

				// 45 degree filter
				// |  -10   -3    0 |
				// |  -3     0    3 | * 1/32
				// |   0     3   10 |
				*targetRow++ = TTarget(3 * (*(source1 + tSourceChannels * 2u) + *(source2 + tSourceChannels) - *(source0 + tSourceChannels) - *(source1)) + 10 * (*(source2 + tSourceChannels * 2u) - *(source0)));

				// 135 degree filter
				// |  0   -3   -10 |
				// |  3    0    -3 | * 1/32
				// | 10    3     0 |
				*targetRow++ = TTarget(3 * (*(source1) + *(source2 + tSourceChannels) - *(source0 + tSourceChannels) - *(source1 + tSourceChannels * 2u)) + 10 * (*(source2) - *(source0 + tSourceChannels * 2u)));

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

template <typename TSource, typename TTarget, unsigned int tSourceChannels, unsigned int tTargetChannels>
void FrameFilterScharr::filterHorizontalVerticalMaximumAbsoluteRow(const TSource* sourceRow, TTarget* targetRow, const unsigned int width, const unsigned int height, unsigned int rowIndex, const unsigned int sourceStrideElements, const unsigned int /*targetStrideElements*/)
{
	static_assert(std::is_same<TSource, uint8_t>::value, "Invalid source data type!");
	static_assert(std::is_same<TTarget, uint8_t>::value || std::is_same<TTarget, uint16_t>::value, "Invalid target data type!");

	static_assert(tSourceChannels >= 1u, "Invalid source channel number!");
	static_assert(tTargetChannels == tSourceChannels, "Invalid target channel number!");

	ocean_assert(width >= 3u && height >= 3u);

	if (rowIndex == 0u || rowIndex == height - 1u)
	{
		// setting the first row and last row to zero

		memset(targetRow, 0, width * tTargetChannels * sizeof(TTarget));
		return;
	}

	const uint8_t* source0 = sourceRow - sourceStrideElements;
	const uint8_t* source1 = sourceRow;
	const uint8_t* source2 = sourceRow + sourceStrideElements;

	// setting first pixel to zero

	for (unsigned int n = 0u; n < tTargetChannels; ++n)
	{
		targetRow[n] = TTarget(0);
	}

	targetRow += tTargetChannels;

	for (unsigned int x = 1u; x < width - 1u; ++x)
	{
		if constexpr (std::is_same<TTarget, uint8_t>::value)
		{
			for (unsigned int n = 0u; n < tSourceChannels; ++n)
			{
				// 0 degree filter
				// |  -3   0   3 |
				// | -10   0  10 | * 1/32
				// |  -3   0   3 |
				*targetRow++ = TTarget((max(abs(int16_t(3 * (*(source0 + tSourceChannels * 2u) - *(source0) + *(source2 + tSourceChannels * 2u) - *(source2)) + 10 * (*(source1 + tSourceChannels * 2u) - *(source1)))),

				// 90 degree filter
				// | -3  -10  -3 |
				// |  0   0    0 | * 1/32
				// |  3   10   3 |
				abs(int16_t(3 * (*(source2) + *(source2 + tSourceChannels * 2u) - *(source0) - *(source0 + tSourceChannels * 2u)) + 10 * (*(source2 + tSourceChannels) - *(source0 + tSourceChannels))))) + 8u) / 16u);

				++source0;
				++source1;
				++source2;
			}
		}
		else
		{
			ocean_assert((std::is_same<TTarget, uint16_t>::value));

			for (unsigned int n = 0u; n < tSourceChannels; ++n)
			{
				// 0 degree filter
				// |  -3   0   3 |
				// | -10   0  10 | * 1/32
				// |  -3   0   3 |
				*targetRow++ = TTarget(max(abs(int16_t(3 * (*(source0 + tSourceChannels * 2u) - *(source0) + *(source2 + tSourceChannels * 2u) - *(source2)) + 10 * (*(source1 + tSourceChannels * 2u) - *(source1)))),

				// 90 degree filter
				// | -3  -10  -3 |
				// |  0   0    0 | * 1/32
				// |  3   10   3 |
				abs(int16_t(3 * (*(source2) + *(source2 + tSourceChannels * 2u) - *(source0) - *(source0 + tSourceChannels * 2u)) + 10 * (*(source2 + tSourceChannels) - *(source0 + tSourceChannels))))));

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

template <typename TSource, typename TTarget, unsigned int tSourceChannels, unsigned int tTargetChannels>
void FrameFilterScharr::filterMaximumAbsoluteRow(const TSource* sourceRow, TTarget* targetRow, const unsigned int width, const unsigned int height, unsigned int rowIndex, const unsigned int sourceStrideElements, const unsigned int /*targetStrideElements*/)
{
	static_assert(std::is_same<TSource, uint8_t>::value, "Invalid source data type!");
	static_assert(std::is_same<TTarget, uint8_t>::value || std::is_same<TTarget, uint16_t>::value, "Invalid target data type!");

	static_assert(tSourceChannels >= 1u, "Invalid source channel number!");
	static_assert(tTargetChannels == tSourceChannels, "Invalid target channel number!");

	ocean_assert(width >= 3u && height >= 3u);

	if (rowIndex == 0u || rowIndex == height - 1u)
	{
		// setting the first row and last row to zero

		memset(targetRow, 0, width * tTargetChannels * sizeof(TTarget));
		return;
	}

	const uint8_t* source0 = sourceRow - sourceStrideElements;
	const uint8_t* source1 = sourceRow;
	const uint8_t* source2 = sourceRow + sourceStrideElements;

	// setting first pixel to zero

	for (unsigned int n = 0u; n < tTargetChannels; ++n)
	{
		targetRow[n] = TTarget(0);
	}

	targetRow += tTargetChannels;

	for (unsigned int x = 1u; x < width - 1u; ++x)
	{
		if constexpr (std::is_same<TTarget, uint8_t>::value)
		{
			for (unsigned int n = 0u; n < tSourceChannels; ++n)
			{
				// 0 degree filter
				// |  -3   0   3 |
				// | -10   0  10 | * 1/32
				// |  -3   0   3 |
				*targetRow++ = TTarget((max(max(abs(int16_t(3 * (*(source0 + tSourceChannels * 2u) - *(source0) + *(source2 + tSourceChannels * 2u) - *(source2)) + 10 * (*(source1 + tSourceChannels * 2u) - *(source1)))),

				// 90 degree filter
				// | -3  -10  -3 |
				// |  0   0    0 | * 1/32
				// |  3   10   3 |
				abs(int16_t(3 * (*(source2) + *(source2 + tSourceChannels * 2u) - *(source0) - *(source0 + tSourceChannels * 2u)) + 10 * (*(source2 + tSourceChannels) - *(source0 + tSourceChannels))))),

				// 45 degree filter
				// |  -10   -3    0 |
				// |  -3     0    3 | * 1/32
				// |   0     3   10 |
				max(abs(int16_t(3 * (*(source1 + tSourceChannels * 2u) + *(source2 + tSourceChannels) - *(source0 + tSourceChannels) - *(source1)) + 10 * (*(source2 + tSourceChannels * 2u) - *(source0)))),

				// 135 degree filter
				// |  0   -3   -10 |
				// |  3    0    -3 | * 1/32
				// | 10    3     0 |
				abs(int16_t(3 * (*(source1) + *(source2 + tSourceChannels) - *(source0 + tSourceChannels) - *(source1 + tSourceChannels * 2u)) + 10 * (*(source2) - *(source0 + tSourceChannels * 2u)))))) + 8u) / 16u);

				++source0;
				++source1;
				++source2;
			}
		}
		else
		{
			ocean_assert((std::is_same<TTarget, uint16_t>::value));

			for (unsigned int n = 0u; n < tSourceChannels; ++n)
			{
				// 0 degree filter
				// |  -3   0   3 |
				// | -10   0  10 | * 1/32
				// |  -3   0   3 |
				*targetRow++ = TTarget(max(max(abs(int16_t(3 * (*(source0 + tSourceChannels * 2u) - *(source0) + *(source2 + tSourceChannels * 2u) - *(source2)) + 10 * (*(source1 + tSourceChannels * 2u) - *(source1)))),

				// 90 degree filter
				// | -3  -10  -3 |
				// |  0   0    0 | * 1/32
				// |  3   10   3 |
				abs(int16_t(3 * (*(source2) + *(source2 + tSourceChannels * 2u) - *(source0) - *(source0 + tSourceChannels * 2u)) + 10 * (*(source2 + tSourceChannels) - *(source0 + tSourceChannels))))),

				// 45 degree filter
				// |  -10   -3    0 |
				// |  -3     0    3 | * 1/32
				// |   0     3   10 |
				max(abs(int16_t(3 * (*(source1 + tSourceChannels * 2u) + *(source2 + tSourceChannels) - *(source0 + tSourceChannels) - *(source1)) + 10 * (*(source2 + tSourceChannels * 2u) - *(source0)))),

				// 135 degree filter
				// |  0   -3   -10 |
				// |  3    0    -3 | * 1/32
				// | 10    3     0 |
				abs(int16_t(3 * (*(source1) + *(source2 + tSourceChannels) - *(source0 + tSourceChannels) - *(source1 + tSourceChannels * 2u)) + 10 * (*(source2) - *(source0 + tSourceChannels * 2u)))))));

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

#endif // META_OCEAN_CV_FRAME_FILTER_SCHARR_H
