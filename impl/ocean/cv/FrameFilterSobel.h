/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_FILTER_SOBEL_H
#define META_OCEAN_CV_FRAME_FILTER_SOBEL_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameFilter.h"

#include "ocean/base/Worker.h"

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
	#include "ocean/cv/SSE.h"
#endif

namespace Ocean
{

namespace CV
{

/**
 * This class implements a Sobel filter.
 * The horizontal (0 degree) and vertical (90 degree - clockwise) 3x3 Sobel box filter (not convolution filter) are defined as:
 * <pre>
 * horizontal:      vertical (90 degree):
 * | -1  0  1 |     | -1  -2  -1 |
 * | -2  0  2 |     |  0   0   0 |
 * | -1  0  1 |     |  1   2   1 |
 * </pre>
 *
 * The diagonal 3x3 Sobel filters are defined as:
 * <pre>
 * 45 degree:        135 degree:
 * | -2  -1   0 |    | 0  -1  -2 |
 * | -1   0   1 |    | 1   0  -1 |
 * |  0   1   2 |    | 2   1   0 |
 * </pre>
 * @see FrameFilterSobelMagnitude, FrameFilterScharr.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameFilterSobel : public FrameFilter
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
				 * Horizontal and vertical Sobel filter for images.
				 * The resulting frame will contain interleaved horizontal and vertical Sobel responses for each individual frame channels.
				 * @param frame The 1-plane frame on which the Sobel filter will be applied, with data type 'DT_UNSIGNED_INTEGER_8', must be valid
				 * @param responseDataType The data type of the individual sobel responses, either 'DT_SIGNED_INTEGER_8' or 'DT_SIGNED_INTEGER_16'
				 * @param worker Optional worker to distribute the computation
				 * @return The resulting sobel response for the given frame, invalid in case of an error
				 */
				static Frame filterHorizontalVertical(const Frame& frame, const FrameType::DataType responseDataType = FrameType::DT_SIGNED_INTEGER_8, Worker* worker = nullptr);

				/**
				 * Horizontal, vertical, and diagonal Sobel filter for images.
				 * The resulting frame will contain interleaved horizontal, vertical, and diagonal Sobel responses for each individual frame channels.
				 * @param frame The 1-plane frame on which the Sobel filter will be applied, with data type 'DT_UNSIGNED_INTEGER_8', must be valid
				 * @param responseDataType The data type of the individual sobel responses, either 'DT_SIGNED_INTEGER_8' or 'DT_SIGNED_INTEGER_16'
				 * @param worker Optional worker to distribute the computation
				 * @return The resulting sobel response for the given frame, invalid in case of an error
				 */
				static Frame filter(const Frame& frame, const FrameType::DataType responseDataType = FrameType::DT_SIGNED_INTEGER_8, Worker* worker = nullptr);
		};

	public:

		/**
		 * Horizontal and vertical Sobel filter for images.
		 * If the target response data type is selected to be `int8_t`, each filter response  is normalized by 1/8 to fit into the value range [-128, 127].<br>
		 * If the target response data type is selected to be `int16_t` no normalization will be applied.
		 * The border pixels are set to zero.
		 * @param source The source frame to which the Sobel filter will be applied, with `tSourceChannels` channels, must be valid
		 * @param target The target response frame receiving the horizontal and vertical Sobel responses, with `tSourceChannels * 2` channels, must be valid
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
		 * Diagonal 45 and 135 degree Sobel filter for images.
		 * If the target response data type is selected to be `int8_t`, each filter response  is normalized by 1/8 to fit into the value range [-128, 127].<br>
		 * If the target response data type is selected to be `int16_t` no normalization will be applied.
		 * The border pixels are set to zero.
		 * @param source The source frame to which the Sobel filter will be applied, with `tSourceChannels` channels, must be valid
		 * @param target The target response frame receiving the diagonal Sobel responses, with `tSourceChannels * 2` channels, must be valid
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
		 * Horizontal, vertical, and diagonal Sobel filter for images.
		 * If the target response data type is selected to be `int8_t`, each filter response  is normalized by 1/8 to fit into the value range [-128, 127].<br>
		 * If the target response data type is selected to be `int16_t` no normalization will be applied.
		 * The border pixels are set to zero.
		 * @param source The source frame to which the Sobel filter will be applied, with `tSourceChannels` channels, must be valid
		 * @param target The target response frame receiving the horizontal, vertical, and both diagonal Sobel responses, with `tSourceChannels * 4` channels, must be valid
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
		 * Determines the maximum of the absolute horizontal and vertical Sobel filter.
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
		 * Determines the maximum of the absolute horizontal and vertical Sobel filter for a given pixel.
		 * If the target response data type is selected to be `uint8_t`, each filter response  is normalized by 1/4 to fit into the value range [0, 255].<br>
		 * If the target response data type is selected to be `uint16_t` no normalization will be applied.
		 * The border pixels are set to zero.
		 * @param source The source frame to filter, with `tChannels` channels, must be valid
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param x Horizontal filter position in pixel, with range [0, width - 1]
		 * @param y Vertical filter position in pixel, with range [0, height - 1]
		 * @param response The resulting filter response for the defined pixel, with `tChannels` channels, must be valid
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, with range [0, infinity)
		 * @tparam TTarget The data type of the response values, either `uint8_t` or `uint16_t`
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tIsCorePixel True, if the pixel position is known to be a core pixel (with range [1, width - 2]x[1, height - 2]; False, if the pixel position can be a border pixel as well
		 */
		template <typename TTarget, unsigned int tChannels, bool tIsCorePixel = false>
		static inline void filterPixelHorizontalVerticalMaximum8BitPerChannel(const uint8_t* const source, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, TTarget* const response, unsigned int sourcePaddingElements);

		/**
		 * Horizontal and vertical Sobel filter for a pixel.
		 * If the target response data type is selected to be `int8_t`, each filter response  is normalized by 1/8 to fit into the value range [-128, 127].<br>
		 * If the target response data type is selected to be `int16_t` no normalization will be applied.
		 * The border pixels are set to zero.
		 * @param source The source frame in which the filter will be applied, with `tSourceChannels` channels, must be valid
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param x Horizontal filter position with range [0, width - 1]
		 * @param y Vertical filter position with range [0, height -1]
		 * @param response The resulting pixel filter response, with `tSourceChannels * 2` channels, must be valid
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @tparam TTarget The data type of the response values, either `int8_t` or `int16_t`
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <typename TTarget, unsigned int tSourceChannels>
		static inline void filterPixelHorizontalVertical8BitPerChannel(const uint8_t* source, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, TTarget* response, const unsigned int sourcePaddingElements);

		/**
		 * Horizontal and vertical Sobel filter for a pixel not at the boundary of the frame (in the inner core of the frame).
		 * If the target response data type is selected to be `int8_t`, each filter response  is normalized by 1/8 to fit into the value range [-128, 127].<br>
		 * If the target response data type is selected to be `int16_t` no normalization will be applied.
		 * @param source The position in the source frame at that the filter has to be applied, the position inside the frame has to fit into the ranges [1, width - 2]x[1, height - 2], with `tChannels` channels, must be valid
		 * @param width The width of the frame in pixel, with `tSourceChannels` channels, with range [3, infinity)
		 * @param response The resulting pixel filter response, with `tSourceChannels * 2` channels, must be valid
		 * @param sourcePaddingElements Optional padding at the end of each source row in elements, with range [0, infinity)
		 * @tparam TTarget The data type of the response values, either 'int8_t' or 'int16_t'
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <typename TTarget, unsigned int tSourceChannels>
		static inline void filterPixelCoreHorizontalVertical8BitPerChannel(const uint8_t* source, const unsigned int width, TTarget* response, const unsigned int sourcePaddingElements);

		/**
		 * Determines the squared Sobel filter responses (three products) for a 1 channel 8 bit pixel based on a horizontal and on a vertical Sobel filter (Ix, Iy).
		 * The first element is the squared horizontal response (Ixx = Ix * Ix), the second the squared vertical response (Iyy = Iy * Iy) and the third is the product between horizontal and vertical response (Ixy = Ix * Iy).
		 * @param source The pointer into the source frame at which the filter has to be applied, the position inside the frame has to fit into the ranges [1, width - 2]x[1, height - 2]
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param responses The three resulting pixel filter responses, with order: Ixx, Iyy, Ixy, must be valid
		 * @param paddingElements Optional padding at the end of each frame row in elements, with range [0, infinity)
		 * @tparam TTarget The data type of the response values, either 'int8_t', 'int16_t', or 'int32_t'
		 * @tparam tNormalizationDenominator The normalization factor for each individual directional response before they will be squared, either 1, 4, or 8
		 * @tparam tRoundedNormalization True, to apply a rounded normalization; False, to apply a normalization without rounding (ignored for floating point values)
		 */
		template <typename TTarget, TTarget tNormalizationDenominator, bool tRoundedNormalization = false>
		static inline void filterPixelCoreHorizontalVertical3Squared1Channel8Bit(const uint8_t* source, const unsigned int width, TTarget* responses, const unsigned int paddingElements);

		/**
		 * Determines the squared Sobel filter responses (three products) for a 1 channel 8 bit row based on a horizontal and on a vertical Sobel filter (Ix, Iy).
		 * The tree individual response products are stored in individual buffers.<br>
		 * Each filter response is normalized by 1/8, so that the products Ixx, Iyy and Ixy fit into the range [-(128 * 128), 127 * 127].
		 * One buffer for Ixx (= Ix * Ix) responses, one buffer for Iyy (= Iy * Iy) responses, and one buffer for Ixy (= Ix * Iy) responses.<br>
		 * The first filter response will be for the second pixel within the given row.
		 * @param row The row of a frame at which the filtering starts, the row must not be the first or last row in the frame, must be valid
		 * @param width The width of the frame in pixel, with range [10, infinity)
		 * @param elements The number of elements within the row for which the filter response will be determined, with range [8, width - 2]
		 * @param paddingElements The number of padding elements at the end of each row, with range [0, infinity)
		 * @param responsesXX The resulting Ixx (= Ix * Ix) responses, the buffer must be large enough to receive 'elements' values, must be valid
		 * @param responsesYY The resulting Iyy (= Iy * Iy) responses, the buffer must be large enough to receive 'elements' values, must be valid
		 * @param responsesXY The resulting Ixy (= Ix * Iy) responses, the buffer must be large enough to receive 'elements' values, must be valid
		 */
		static void filterHorizontalVertical3Squared1Channel8BitRow(const uint8_t* row, const unsigned int width, const unsigned int elements, const unsigned int paddingElements, int16_t* responsesXX, int16_t* responsesYY, int16_t* responsesXY);

	private:

		/**
		 * Applies the horizontal and vertical Sobel filter to one row of a source frame.
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
		 * Applies the diagonal (45 and 135 degree) Sobel filter to one row of a source frame.
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
		 * Applies the horizontal, vertical, and diagonal Sobel filter to one row of a source frame.
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
		 * Applies the maximum of the absolute horizontal and vertical Sobel filter to one row of a source frame.
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

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

		/**
		 * Block based SSE implementation of horizontal and vertical Sobel filter for 8 bit pixel.
		 * This variant calculates the Sobel response of a block consisting of 3 consecutive rows each with 16 consecutive pixel, i.e. returns 14 response values for each direction.<br>
		 * Responses are returned in SSE registers and are not normalized, each response value is of type int16_t (16 bit).
		 * @param source0 First row of input frame to filter, must be valid
		 * @param source1 Second row of input frame to filter, must be valid
		 * @param source2 Third row of input frame to filter, must be valid
		 * @param response_x_low Contains first 8 horizontal response values (each size of 2 bytes)
		 * @param response_x_high Contains remaining 6 horizontal response values (each size of 2 bytes, beginning with least significant bit)
		 * @param response_y_low Contains first 8 vertical response values (each size of 2 bytes)
		 * @param response_y_high Contains remaining 6 vertical response values (each size of 2 bytes, beginning with least significant bit)
		 */
		static inline void filterHorizontalVertical8BitBlock14SSE(const uint8_t* source0, const uint8_t* source1, const uint8_t* source2, __m128i& response_x_low, __m128i& response_x_high, __m128i& response_y_low, __m128i& response_y_high);

		/**
		 * SSE block based horizontal and vertical Sobel filter for 1 channel 8 bit frame. <br>
		 * Responses of a block (3 consecutive rows, each with 16 consecutive pixel) are calculated, yielding 14 response values for each direction. <br>
		 * The two different filter responses are zipped, thus the target must provide 14x2x16 bit.
		 * @param source The source frame to filter, must be valid
		 * @param strideElements The stride of the source frame in elements, which is width of the frame plus padding elements, with range [16, infinity)
		 * @param response Pixel filter responses, must be valid
		 */
		static inline void filterHorizontalVertical1Channel8BitBlock14SSE(const uint8_t* source, const unsigned int strideElements, int16_t* response);

		/**
		 * SSE block based horizontal and vertical Sobel filter for 1 channel 8 bit frame. <br>
		 * Responses of a block (3 consecutive rows, each with 16 consecutive pixel) are calculated, yielding 14 response values for each direction. <br>
		 * Each resulting filter value is normalized by 1/8 to fit value range [-128, 127]
		 * The two different filter responses are zipped, thus the target must provide 14x2x16 bit.
		 * @param source The source frame to filter, must be valid
		 * @param strideElements The stride of the source frame in elements, which is width of the frame plus padding elements, with range [16, infinity)
		 * @param response Pixel filter responses, must be valid
		 */
		static inline void filterHorizontalVertical1Channel8BitBlock14SSE(const uint8_t* source, const unsigned int strideElements, int8_t* response);

#endif // OCEAN_HARDWARE_SSE_VERSION >= 41
};

template <typename TTarget, unsigned int tSourceChannels>
inline void FrameFilterSobel::filterHorizontalVertical8BitPerChannel(const uint8_t* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
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
inline void FrameFilterSobel::filterDiagonal8BitPerChannel(const uint8_t* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
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
inline void FrameFilterSobel::filter8BitPerChannel(const uint8_t* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
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
inline void FrameFilterSobel::filterHorizontalVerticalMaximumAbsolute8BitPerChannel(const uint8_t* const source, TTarget* const target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(std::is_same<TTarget, uint8_t>::value || std::is_same<TTarget, uint16_t>::value, "Invalid target data type!");
	static_assert(tSourceChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 3u && height >= 3u);

	//using TSource = uint8_t;
	constexpr unsigned int tTargetChannels = tSourceChannels;

	FrameChannels::applyRowOperator<uint8_t, TTarget, tSourceChannels, tTargetChannels>(source, target, width, height, sourcePaddingElements, targetPaddingElements, &filterHorizontalVerticalMaximumAbsoluteRow<uint8_t, TTarget, tSourceChannels, tTargetChannels>, worker);
}

template <typename TSource, typename TTarget, unsigned int tSourceChannels, unsigned int tTargetChannels>
void FrameFilterSobel::filterHorizontalVerticalRow(const TSource* sourceRow, TTarget* targetRow, const unsigned int width, const unsigned int height, unsigned int rowIndex, const unsigned int sourceStrideElements, const unsigned int /*targetStrideElements*/)
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

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	if constexpr (tSourceChannels == 1u)
	{
		if (width >= 16u)
		{
			const unsigned int blockNumber = (width - 2u) / 14u;

			for (unsigned int iBlock = 0u; iBlock < blockNumber; iBlock++)
			{
				filterHorizontalVertical1Channel8BitBlock14SSE(sourceRow, sourceStrideElements, targetRow);
				targetRow += 14u * 2u;
				sourceRow += 14;
			}

			// calculate remaining elements

			const unsigned int blockRest = (width - 2u) % 14u;

			if (blockRest > 0u)
			{
				// shift back pointers
				sourceRow -= (14u - blockRest);
				targetRow -= (14u - blockRest) * 2u;

				filterHorizontalVertical1Channel8BitBlock14SSE(sourceRow, sourceStrideElements, targetRow);

				// set pointers to end of line
				targetRow += 14u * 2u;
			}

			// setting last pixel to zero
			for (unsigned int n = 0u; n < tTargetChannels; ++n)
			{
				targetRow[n] = TTarget(0);
			}

			return;
		}
	}

#endif

	const uint8_t* source0 = sourceRow - sourceStrideElements;
	const uint8_t* source1 = sourceRow;
	const uint8_t* source2 = sourceRow + sourceStrideElements;

	for (unsigned int x = 1u; x < width - 1u; ++x)
	{
		if constexpr (std::is_same<TTarget, int8_t>::value)
		{
			for (unsigned int n = 0u; n < tSourceChannels; ++n)
			{
				// | -1  0  1 |
				// | -2  0  2 | / 8
				// | -1  0  1 |
				*targetRow++ = TTarget((*(source0 + tSourceChannels * 2u) - *(source0) + (*(source1 + tSourceChannels * 2u) - *(source1)) * 2 + *(source2 + tSourceChannels * 2u) - *(source2)) / 8);

				// | -1 -2 -1 |
				// |  0  0  0 | / 8
				// |  1  2  1 |
				*targetRow++ = TTarget((*(source2) + (*(source2 + tSourceChannels) - *(source0 + tSourceChannels)) * 2 + *(source2 + tSourceChannels * 2u) - *(source0) - *(source0 + tSourceChannels * 2u)) / 8);

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
				// | -1  0  1 |
				// | -2  0  2 |
				// | -1  0  1 |
				*targetRow++ = TTarget(*(source0 + tSourceChannels * 2u) - *(source0) + (*(source1 + tSourceChannels * 2u) - *(source1)) * 2 + *(source2 + tSourceChannels * 2u) - *(source2));

				// | -1 -2 -1 |
				// |  0  0  0 |
				// |  1  2  1 |
				*targetRow++ = TTarget(*(source2) + (*(source2 + tSourceChannels) - *(source0 + tSourceChannels)) * 2 + *(source2 + tSourceChannels * 2u) - *(source0) - *(source0 + tSourceChannels * 2u));

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
void FrameFilterSobel::filterDiagonalRow(const TSource* sourceRow, TTarget* targetRow, const unsigned int width, const unsigned int height, unsigned int rowIndex, const unsigned int sourceStrideElements, const unsigned int /*targetStrideElements*/)
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
				// | -2 -1  0 |
				// | -1  0  1 | / 8
				// |  0  1  2 |
				*targetRow++ = TTarget((*(source1 + tSourceChannels * 2u) + *(source2 + tSourceChannels) + (*(source2 + tSourceChannels * 2u) - *(source0)) * 2 - *(source0 + tSourceChannels) - *(source1)) / 8);

				// | 0  -1  -2 |
				// | 1   0  -1 | / 8
				// | 2   1   0 |
				*targetRow++ = TTarget((*(source1) + *(source2 + tSourceChannels) + (*(source2) - *(source0 + tSourceChannels * 2u)) * 2 - *(source0 + tSourceChannels) - *(source1 + tSourceChannels * 2u)) / 8);

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
				// | -2 -1  0 |
				// | -1  0  1 |
				// |  0  1  2 |
				*targetRow++ = TTarget(*(source1 + tSourceChannels * 2u) + *(source2 + tSourceChannels) + (*(source2 + tSourceChannels * 2u) - *(source0)) * 2 - *(source0 + tSourceChannels) - *(source1));

				// | 0  -1  -2 |
				// | 1   0  -1 |
				// | 2   1   0 |
				*targetRow++ = TTarget(*(source1) + *(source2 + tSourceChannels) + (*(source2) - *(source0 + tSourceChannels * 2u)) * 2 - *(source0 + tSourceChannels) - *(source1 + tSourceChannels * 2u));

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
void FrameFilterSobel::filterRow(const TSource* sourceRow, TTarget* targetRow, const unsigned int width, const unsigned int height, unsigned int rowIndex, const unsigned int sourceStrideElements, const unsigned int /*targetStrideElements*/)
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
				// | -1  0  1 |
				// | -2  0  2 | / 8
				// | -1  0  1 |
				*targetRow++ = TTarget((*(source0 + tSourceChannels * 2u) - *(source0) + (*(source1 + tSourceChannels * 2u) - *(source1)) * 2 + *(source2 + tSourceChannels * 2u) - *(source2)) / 8);

				// | -1 -2 -1 |
				// |  0  0  0 | / 8
				// |  1  2  1 |
				*targetRow++ = TTarget((*(source2) + (*(source2 + tSourceChannels) - *(source0 + tSourceChannels)) * 2 + *(source2 + tSourceChannels * 2u) - *(source0) - *(source0 + tSourceChannels * 2u)) / 8);

				// | -2 -1  0 |
				// | -1  0  1 | / 8
				// |  0  1  2 |
				*targetRow++ = TTarget((*(source1 + tSourceChannels * 2u) + *(source2 + tSourceChannels) + (*(source2 + tSourceChannels * 2u) - *(source0)) * 2 - *(source0 + tSourceChannels) - *(source1)) / 8);

				// | 0  -1  -2 |
				// | 1   0  -1 | / 8
				// | 2   1   0 |
				*targetRow++ = TTarget((*(source1) + *(source2 + tSourceChannels) + (*(source2) - *(source0 + tSourceChannels * 2u)) * 2 - *(source0 + tSourceChannels) - *(source1 + tSourceChannels * 2u)) / 8);

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
				// | -1  0  1 |
				// | -2  0  2 |
				// | -1  0  1 |
				*targetRow++ = TTarget(*(source0 + tSourceChannels * 2u) - *(source0) + (*(source1 + tSourceChannels * 2u) - *(source1)) * 2 + *(source2 + tSourceChannels * 2u) - *(source2));

				// | -1 -2 -1 |
				// |  0  0  0 |
				// |  1  2  1 |
				*targetRow++ = TTarget(*(source2) + (*(source2 + tSourceChannels) - *(source0 + tSourceChannels)) * 2 + *(source2 + tSourceChannels * 2u) - *(source0) - *(source0 + tSourceChannels * 2u));

				// | -2 -1  0 |
				// | -1  0  1 |
				// |  0  1  2 |
				*targetRow++ = TTarget(*(source1 + tSourceChannels * 2u) + *(source2 + tSourceChannels) + (*(source2 + tSourceChannels * 2u) - *(source0)) * 2 - *(source0 + tSourceChannels) - *(source1));

				// | 0  -1  -2 |
				// | 1   0  -1 |
				// | 2   1   0 |
				*targetRow++ = TTarget(*(source1) + *(source2 + tSourceChannels) + (*(source2) - *(source0 + tSourceChannels * 2u)) * 2 - *(source0 + tSourceChannels) - *(source1 + tSourceChannels * 2u));

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
void FrameFilterSobel::filterHorizontalVerticalMaximumAbsoluteRow(const TSource* sourceRow, TTarget* targetRow, const unsigned int width, const unsigned int height, unsigned int rowIndex, const unsigned int sourceStrideElements, const unsigned int /*targetStrideElements*/)
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
				// | -1  0  1 |
				// | -2  0  2 | / 4
				// | -1  0  1 |
				*targetRow++ = TTarget((max(abs(*(source0 + tSourceChannels * 2u) - *(source0) + (*(source1 + tSourceChannels * 2u) - *(source1)) * 2 + *(source2 + tSourceChannels * 2u) - *(source2)),

				// | -1 -2 -1 |
				// |  0  0  0 | / 4
				// |  1  2  1 |
				abs(*(source2) + (*(source2 + tSourceChannels) - *(source0 + tSourceChannels)) * 2 + *(source2 + tSourceChannels * 2u) - *(source0) - *(source0 + tSourceChannels * 2u))) + 2u) / 4u);

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
				// | -1  0  1 |
				// | -2  0  2 |
				// | -1  0  1 |
				*targetRow++ = TTarget(max(abs(*(source0 + tSourceChannels * 2u) - *(source0) + (*(source1 + tSourceChannels * 2u) - *(source1)) * 2 + *(source2 + tSourceChannels * 2u) - *(source2)),

				// | -1 -2 -1 |
				// |  0  0  0 |
				// |  1  2  1 |
				abs(*(source2) + (*(source2 + tSourceChannels) - *(source0 + tSourceChannels)) * 2 + *(source2 + tSourceChannels * 2u) - *(source0) - *(source0 + tSourceChannels * 2u))));

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

template <typename TTarget, unsigned int tSourceChannels>
inline void FrameFilterSobel::filterPixelHorizontalVertical8BitPerChannel(const uint8_t* source, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, TTarget* response, const unsigned int sourcePaddingElements)
{
	static_assert(std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value, "Invalid target data type!");
	static_assert(tSourceChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && response != nullptr);
	ocean_assert(x < width && y < height);
	ocean_assert(width >= 3u && height >= 3u);

	ocean_assert((x - 1u < width - 2u && y - 1u < height - 2u) == (x >= 1u && y >= 1u && x + 1u < width && y + 1u < height));

	if (x - 1u < width - 2u && y - 1u < height - 2u)
	{
		const unsigned int sourceStrideElements = width * tSourceChannels + sourcePaddingElements;

		filterPixelCoreHorizontalVertical8BitPerChannel<TTarget, tSourceChannels>(source + y * sourceStrideElements + x * tSourceChannels, width, response, sourcePaddingElements);
	}
	else
	{
		for (unsigned int n = 0u; n < tSourceChannels * 2u; ++n)
		{
			response[n] = TTarget(0);
		}
	}
}

template <typename TTarget, unsigned int tSourceChannels>
inline void FrameFilterSobel::filterPixelCoreHorizontalVertical8BitPerChannel(const uint8_t* source, const unsigned int width, TTarget* response, const unsigned int sourcePaddingElements)
{
	static_assert(std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value, "Invalid target data type!");
	static_assert(tSourceChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && response != nullptr);
	ocean_assert(width >= 3u);

	const unsigned int sourceStrideElements = width * tSourceChannels + sourcePaddingElements;

	if constexpr (std::is_same<TTarget, int8_t>::value)
	{
		for (unsigned int n = 0u; n < tSourceChannels; ++n)
		{
			// | -1  0  1 |
			// | -2  0  2 | / 8
			// | -1  0  1 |
			*response++ = TTarget((*(source - sourceStrideElements + tSourceChannels) - *(source - sourceStrideElements - tSourceChannels) + (*(source + tSourceChannels) - *(source - tSourceChannels)) * 2 + *(source + sourceStrideElements + tSourceChannels) - *(source + sourceStrideElements - tSourceChannels)) / 8);

			// | -1 -2 -1 |
			// |  0  0  0 | / 8
			// |  1  2  1 |
			*response++ = TTarget((*(source + sourceStrideElements - tSourceChannels) + (*(source + sourceStrideElements) - *(source - sourceStrideElements)) * 2 + *(source + sourceStrideElements + tSourceChannels) - *(source - sourceStrideElements - tSourceChannels) - *(source - sourceStrideElements + tSourceChannels)) / 8);

			++source;
		}
	}
	else
	{
		ocean_assert((std::is_same<TTarget, int16_t>::value));

		for (unsigned int n = 0u; n < tSourceChannels; ++n)
		{
			// | -1  0  1 |
			// | -2  0  2 |
			// | -1  0  1 |
			*response++ = TTarget(*(source - sourceStrideElements + tSourceChannels) - *(source - sourceStrideElements - tSourceChannels) + (*(source + tSourceChannels) - *(source - tSourceChannels)) * 2 + *(source + sourceStrideElements + tSourceChannels) - *(source + sourceStrideElements - tSourceChannels));

			// | -1 -2 -1 |
			// |  0  0  0 |
			// |  1  2  1 |
			*response++ = TTarget(*(source + sourceStrideElements - tSourceChannels) + (*(source + sourceStrideElements) - *(source - sourceStrideElements)) * 2 + *(source + sourceStrideElements + tSourceChannels) - *(source - sourceStrideElements - tSourceChannels) - *(source - sourceStrideElements + tSourceChannels));

			++source;
		}
	}
}

template <typename TTarget, TTarget tNormalizationDenominator, bool tRoundedNormalization>
inline void FrameFilterSobel::filterPixelCoreHorizontalVertical3Squared1Channel8Bit(const uint8_t* source, const unsigned int width, TTarget* responses, const unsigned int paddingElements)
{
	static_assert(tNormalizationDenominator == 1 || tNormalizationDenominator == 4 || tNormalizationDenominator == 8, "Invalid normalization factor!");

	ocean_assert(source != nullptr && responses != nullptr);
	ocean_assert(width >= 3u);

	const unsigned int strideElements = width + paddingElements;

	// | -1  0  1 |
	// | -2  0  2 |
	// | -1  0  1 |
	const TTarget horizontal = TTarget(FrameFilter::normalizeValue<int32_t, tNormalizationDenominator, tRoundedNormalization>(*(source - strideElements + 1u) - *(source - strideElements - 1u) + (*(source + 1u) - *(source - 1u)) * 2 + *(source + strideElements + 1u) - *(source + strideElements - 1u)));

	// | -1 -2 -1 |
	// |  0  0  0 |
	// |  1  2  1 |
	const TTarget vertical = TTarget(FrameFilter::normalizeValue<int32_t, tNormalizationDenominator, tRoundedNormalization>(*(source + strideElements - 1u) + (*(source + strideElements) - *(source - strideElements)) * 2 + *(source + strideElements + 1u) - *(source - strideElements - 1u) - *(source - strideElements + 1u)));

	*responses++ = horizontal * horizontal;
	*responses++ = vertical * vertical;
	*responses = horizontal * vertical;
}

template <typename TTarget, unsigned int tChannels, bool tIsCorePixel>
inline void FrameFilterSobel::filterPixelHorizontalVerticalMaximum8BitPerChannel(const uint8_t* const source, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, TTarget* const response, unsigned int sourcePaddingElements)
{
	static_assert(std::is_same<TTarget, uint8_t>::value || std::is_same<TTarget, uint16_t>::value, "Invalid target data type!");
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && response != nullptr);

	ocean_assert((x >= 1u && x < width - 1u && y >= 1u && y < height - 1u) == (x - 1u < width - 2u && y - 1u < height - 2u));

	if (tIsCorePixel || (x - 1u < width - 2u && y - 1u < height - 2u))
	{
		const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;

		const uint8_t* source0 = source + (y - 1u) * sourceStrideElements + x * tChannels;
		const uint8_t* source1 = source + (y + 0u) * sourceStrideElements + x * tChannels;
		const uint8_t* source2 = source + (y + 1u) * sourceStrideElements + x * tChannels;

		if (std::is_same<TTarget, uint8_t>::value)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
								// | -1 0 1 |
								// | -2 0 2 |
								// | -1 0 1 |
				response[n] = TTarget((max(abs(*(source0 + tChannels) - *(source0 - tChannels) + (*(source1 + tChannels) - *(source1 - tChannels)) * 2 + *(source2 + tChannels) - *(source2 - tChannels)),

								// | -1 -2 -1 |
								// |  0  0  0 |
								// |  1  2  1 |
								abs(*(source2 - tChannels) + (*(source2) - *(source0)) * 2 + *(source2 + tChannels) - *(source0 - tChannels) - *(source0 + tChannels))) + 2u) / 4u);

				++source0;
				++source1;
				++source2;
			}
		}
		else
		{
			ocean_assert((std::is_same<TTarget, uint16_t>::value));

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
								// | -1 0 1 |
								// | -2 0 2 |
								// | -1 0 1 |
				response[n] = TTarget(max(abs(*(source0 + tChannels) - *(source0 - tChannels) + (*(source1 + tChannels) - *(source1 - tChannels)) * 2 + *(source2 + tChannels) - *(source2 - tChannels)),

								// | -1 -2 -1 |
								// |  0  0  0 |
								// |  1  2  1 |
								abs(*(source2 - tChannels) + (*(source2) - *(source0)) * 2 + *(source2 + tChannels) - *(source0 - tChannels) - *(source0 + tChannels))));

				++source0;
				++source1;
				++source2;
			}
		}
	}
	else
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			response[n] = TTarget(0);
		}
	}
}

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

inline void FrameFilterSobel::filterHorizontalVertical8BitBlock14SSE(const uint8_t* source0, const uint8_t* source1, const uint8_t* source2, __m128i& response_x_low, __m128i& response_x_high, __m128i& response_y_low, __m128i& response_y_high)
{
	ocean_assert(source0 && source1 && source2);

	// load 16 byte-elements of 3 consecutive rows
	const __m128i row0 = _mm_lddqu_si128((__m128i*)source0);
	const __m128i row1 = _mm_lddqu_si128((__m128i*)source1);
	const __m128i row2 = _mm_lddqu_si128((__m128i*)source2);

	// unpack 8-bit values to 16-bit vectors
	//      row = [15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0]
	//  row_low = [ 7  6  5  4  3  2 1 0]
	// row_high = [15 14 13 12 11 10 9 8]
	const __m128i row0_low = _mm_unpacklo_epi8(row0, _mm_set1_epi8(0u));
	const __m128i row0_high = _mm_unpackhi_epi8(row0, _mm_set1_epi8(0u));
	const __m128i row1_low = _mm_unpacklo_epi8(row1, _mm_set1_epi8(0u));
	const __m128i row1_high = _mm_unpackhi_epi8(row1, _mm_set1_epi8(0u));
	const __m128i row2_low = _mm_unpacklo_epi8(row2, _mm_set1_epi8(0u));
	const __m128i row2_high = _mm_unpackhi_epi8(row2, _mm_set1_epi8(0u));

	// double row0, row1 and row2
	const __m128i row0_x2_high = _mm_slli_epi16(row0_high, 1);
	const __m128i row1_x2_high = _mm_slli_epi16(row1_high, 1);
	const __m128i row2_x2_high = _mm_slli_epi16(row2_high, 1);
	const __m128i row0_x2_low = _mm_slli_epi16(row0_low, 1);
	const __m128i row1_x2_low = _mm_slli_epi16(row1_low, 1);
	const __m128i row2_x2_low = _mm_slli_epi16(row2_low, 1);

	// vertical
	// | -1 -2 -1 |
	// |  0  0  0 |
	// |  1  2  1 |

	// subtract element wise row0 from row2
	const __m128i diff_row02_high = _mm_sub_epi16(row2_high, row0_high);
	const __m128i diff_row02_low = _mm_sub_epi16(row2_low, row0_low);

	const __m128i diff_row02_x2_high = _mm_sub_epi16(row2_x2_high, row0_x2_high);
	const __m128i diff_row02_x2_low = _mm_sub_epi16(row2_x2_low, row0_x2_low);

	// add elements
	// row_sum_1 = [7 6 5 4 3 2 1 0] + [9 8 7 6 5 4 3 2]
	// row_sum_2 = [15 14 13 12 11 10 9 8] + [x x 15 14 13 12 11 10]
	const __m128i row02_sum_1 = _mm_add_epi16(diff_row02_low, _mm_or_si128(_mm_srli_si128(diff_row02_low, 4u), _mm_slli_si128(diff_row02_high, 12u)));

	//                               [15 14 13 12 11 10  9  8] [xx xx 15 14 13 12 11 10]
	const __m128i row02_sum_2 = _mm_add_epi16(diff_row02_high, _mm_srli_si128(diff_row02_high, 4u));

	// add double weighted elements                           [x 7 6 5 4 3 2 1]                      [8 x x x x x x x]
	response_y_low = _mm_adds_epi16(row02_sum_1, _mm_or_si128(_mm_srli_si128(diff_row02_x2_low, 2u), _mm_slli_si128(diff_row02_x2_high, 14u)));

	response_y_high = _mm_adds_epi16(row02_sum_2, _mm_srli_si128(diff_row02_x2_high, 2u));

	// normalize sums (shift sums to right by 3 - division by 8)
	//response_y_low = _mm_srai_epi16(addOffsetForRightShiftDivision(sum_vert_low), 3u);
	//response_y_high = _mm_srai_epi16(addOffsetForRightShiftDivision(sum_vert_high), 3u);

	// horizontal
	// | -1 0 1 |
	// | -2 0 2 |
	// | -1 0 1 |

	//                                            [x x 7 6 5 4 3 2]             [9 8 x x x x x x]
	const __m128i row0_low_shifted = _mm_or_si128(_mm_srli_si128(row0_low, 4u), _mm_slli_si128(row0_high, 12u));

	//                                               [x x 7 6 5 4 3 2]                [9 8 x x x x x x ]
	const __m128i row1_x2_low_shifted = _mm_or_si128(_mm_srli_si128(row1_x2_low, 4u), _mm_slli_si128(row1_x2_high, 12u));

	//                                            [x x 7 6 5 4 3 2]             [9 8 x x x x x x ]
	const __m128i row2_low_shifted = _mm_or_si128(_mm_srli_si128(row2_low, 4u), _mm_slli_si128(row2_high, 12u));

	// subtract
	// diff_low = [9-7 8-6 7-5 6-4 5-3 4-2 3-1 2-0]
	// diff_high = [xxx xxx 15-13 14-12 13-11 12-10 11-9 10-8]
	const __m128i diff_cols_r0_low = _mm_sub_epi16(row0_low_shifted, row0_low);
	const __m128i diff_cols_r0_high = _mm_sub_epi16(_mm_srli_si128(row0_high, 4u), row0_high);
	const __m128i diff_cols_r2_low = _mm_sub_epi16(row2_low_shifted, row2_low);
	const __m128i diff_cols_r2_high = _mm_sub_epi16(_mm_srli_si128(row2_high, 4u), row2_high);
	const __m128i diff_cols_r1_x2_low = _mm_sub_epi16(row1_x2_low_shifted, row1_x2_low);
	const __m128i diff_cols_r1_x2_high = _mm_sub_epi16(_mm_srli_si128(row1_x2_high, 4u), row1_x2_high);

	// add
	response_x_low = _mm_adds_epi16(_mm_adds_epi16(diff_cols_r0_low, diff_cols_r2_low), diff_cols_r1_x2_low);
	response_x_high = _mm_adds_epi16(_mm_adds_epi16(diff_cols_r0_high, diff_cols_r2_high), diff_cols_r1_x2_high);

	// normalize
	//response_x_low = _mm_srai_epi16(addOffsetForRightShiftDivision(sum_horz_low), 3u);
	//response_x_high = _mm_srai_epi16(addOffsetForRightShiftDivision(sum_horz_high), 3u);
}

inline void FrameFilterSobel::filterHorizontalVertical1Channel8BitBlock14SSE(const uint8_t* source, const unsigned int strideElements, int8_t* response)
{
	ocean_assert(source != nullptr && response != nullptr);
	ocean_assert(strideElements >= 16u);

	__m128i response_y_low;
	__m128i response_y_high;
	__m128i response_x_low;
	__m128i response_x_high;

	filterHorizontalVertical8BitBlock14SSE(source - strideElements, source, source + strideElements, response_x_low, response_x_high, response_y_low, response_y_high);

	// normalize responses
	const __m128i response_x_low_norm = SSE::divideByRightShiftSigned16Bit(response_x_low, 3u);
	const __m128i response_x_high_norm = SSE::divideByRightShiftSigned16Bit(response_x_high, 3u);
	const __m128i response_y_low_norm = SSE::divideByRightShiftSigned16Bit(response_y_low, 3u);
	const __m128i response_y_high_norm = SSE::divideByRightShiftSigned16Bit(response_y_high, 3u);

	// pack into one vector
	const __m128i response_x_norm = _mm_packs_epi16(response_x_low_norm, response_x_high_norm);
	const __m128i response_y_norm = _mm_packs_epi16(response_y_low_norm, response_y_high_norm);

	// zip response values
	const __m128i response_zipped_lo = _mm_unpacklo_epi8(response_x_norm, response_y_norm);
	const __m128i response_zipped_hi = _mm_unpackhi_epi8(response_x_norm, response_y_norm);

	_mm_storeu_si128((__m128i*)response, response_zipped_lo);
	memcpy(response + 16, &response_zipped_hi, 12);
}

inline void FrameFilterSobel::filterHorizontalVertical1Channel8BitBlock14SSE(const uint8_t* source, const unsigned int strideElements, int16_t* response)
{
	ocean_assert(source != nullptr && response != nullptr);
	ocean_assert(strideElements >= 16u);

	__m128i response_y_low;
	__m128i response_y_high;
	__m128i response_x_low;
	__m128i response_x_high;

	filterHorizontalVertical8BitBlock14SSE(source - strideElements, source, source + strideElements, response_x_low, response_x_high, response_y_low, response_y_high);

	// zip response values
	const __m128i response_zipped_lo = _mm_unpacklo_epi16(response_x_low, response_y_low);
	const __m128i response_zipped_hi = _mm_unpackhi_epi16(response_x_low, response_y_low);
	const __m128i response_zipped_lo_2 = _mm_unpacklo_epi16(response_x_high, response_y_high);
	const __m128i response_zipped_hi_2 = _mm_unpackhi_epi16(response_x_high, response_y_high);

	// copy to memory. only first 2 16-bit values of response_zipped_hi_2 are relevant
	memcpy(response, &response_zipped_lo, 16);
	memcpy(response + 8, &response_zipped_hi, 16);
	memcpy(response + 16, &response_zipped_lo_2, 16);
	memcpy(response + 24, &response_zipped_hi_2, 8);
}

#endif

}

}

#endif // META_OCEAN_CV_FRAME_FILTER_SOBEL_H
