/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_FILTER_SCHARR_MAGNITUDE_H
#define META_OCEAN_CV_FRAME_FILTER_SCHARR_MAGNITUDE_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameFilterScharr.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements a scharr filter based on (or respecting) the magnitude of the response.
 * The horizontal 3x3 scharr filter mask is defined as:
 * <pre>
 * |  -3   0   3 |
 * | -10   0  10 |
 * |  -3   0   3 |
 * </pre>
 * @see FrameFilterScharr, FrameFilterSobelMagnitude.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameFilterScharrMagnitude
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
				 * Horizontal and vertical Scharr filter returning the response for the channel with largest magnitude.
				 * The target frame must be valid, must have data type 'DT_SIGNED_INTEGER_8` or `DT_SIGNED_INTEGER_16`, and 2 channels, one for each filter direction.
				 * @param source The source frame to filter, must be valid
				 * @param target The target frame receiving the filtered frame, will be adjusted if the frame type or pixel format is not compatible or read-only, can be invalid
				 * @param worker Optional worker object to distribute the computational load
				 * @return True, if succeeded
				 */
				static bool filterHorizontalVerticalAs1Channel(const Frame& source, Frame& target, Worker* worker = nullptr);

				/**
				 * Diagonal (45 and 135 degree) Scharr filter returning the response for the channel with largest magnitude.
				 * The target frame must be valid, must have data type 'DT_SIGNED_INTEGER_8` or `DT_SIGNED_INTEGER_16`, and 2 channels, one for each filter direction.
				 * @param source The source frame to filter, must be valid
				 * @param target The target frame receiving the filtered frame, will be adjusted if the frame type or pixel format is not compatible or read-only, can be invalid
				 * @param worker Optional worker object to distribute the computational load
				 * @return True, if succeeded
				 */
				static bool filterDiagonalAs1Channel(const Frame& source, Frame& target, Worker* worker = nullptr);

				/**
				 * Horizontal, vertical, and diagonal (45 and 135 degree) Scharr filter for returning the response for the channel with largest magnitude.
				 * The target frame must be valid, must have data type 'DT_SIGNED_INTEGER_8` or `DT_SIGNED_INTEGER_16`, and 4 channels, one for each filter direction.
				 * @param source The source frame to filter, must be valid
				 * @param target The target frame receiving the filtered frame, will be adjusted if the frame type or pixel format is not compatible or read-only, can be invalid
				 * @param worker Optional worker object to distribute the computational load
				 * @return True, if succeeded
				 */
				static bool filterAs1Channel(const Frame& source, Frame& target, Worker* worker = nullptr);
		};

	protected:

		/**
		 * This class provides functions allowing to calculate a response for one pixel.
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		class PixelResponse
		{
			public:

				/**
				 * Determines the horizontal Scharr response for one channel.
				 * @param source0 The top left location of the 3x3 box filter, must be valid
				 * @param source1 The center left location of the 3x3 box filter, must be valid
				 * @param source2 the bottom left location of the 3x3 box filter, must be valid
				 * @return The horizontal Scharr response, normalized by 32
				 * @tparam TTarget The data type oft he target response frame, must be `int8_t` or `int16_t`
				 */
				template <typename TTarget>
				static OCEAN_FORCE_INLINE TTarget response0(const uint8_t* const source0, const uint8_t* const source1, const uint8_t* const source2);

				/**
				 * Determines the vertical Scharr response for one channel.
				 * @param source0 The top left location of the 3x3 box filter, must be valid
				 * @param source1 The center left location of the 3x3 box filter, must be valid
				 * @param source2 the bottom left location of the 3x3 box filter, must be valid
				 * @return The vertical Scharr response, normalized by 32
				 * @tparam TTarget The data type oft he target response frame, must be `int8_t` or `int16_t`
				 */
				template <typename TTarget>
				static OCEAN_FORCE_INLINE TTarget response90(const uint8_t* const source0, const uint8_t* const source1, const uint8_t* const source2);

				/**
				 * Determines the diagonal Scharr response for one channel.
				 * @param source0 The top left location of the 3x3 box filter, must be valid
				 * @param source1 The center left location of the 3x3 box filter, must be valid
				 * @param source2 the bottom left location of the 3x3 box filter, must be valid
				 * @return The horizontal Scharr response, normalized by 32
				 * @tparam TTarget The data type oft he target response frame, must be `int8_t` or `int16_t`
				 * @tparam tAngle The angle of the filter, one of the following values {0, 45, 90, 135}
				 */
				template <typename TTarget>
				static OCEAN_FORCE_INLINE TTarget response45(const uint8_t* const source0, const uint8_t* const source1, const uint8_t* const source2);

				/**
				 * Determines the diagonal Scharr response for one channel.
				 * @param source0 The top left location of the 3x3 box filter, must be valid
				 * @param source1 The center left location of the 3x3 box filter, must be valid
				 * @param source2 the bottom left location of the 3x3 box filter, must be valid
				 * @return The vertical Scharr response, normalized by 32
				 * @tparam TTarget The data type oft he target response frame, must be `int8_t` or `int16_t`
				 */
				template <typename TTarget>
				static OCEAN_FORCE_INLINE TTarget response135(const uint8_t* const source0, const uint8_t* const source1, const uint8_t* const source2);

				/**
				 * Determines the maximal horizontal and vertical Scharr response across all channels.
				 * @param source0 The top left location of the 3x3 box filter, must be valid
				 * @param source1 The center left location of the 3x3 box filter, must be valid
				 * @param source2 the bottom left location of the 3x3 box filter, must be valid
				 * @param responses The maximal horizontal and vertical Scharr response across all channels based on the magnitude, normalized by 32
				 * @tparam TTarget The data type oft he target response frame, must be `int8_t` or `int16_t`
				 * @tparam TResponseA
				 * @tparam TResponseB
				 */
				template <typename TTarget, TTarget(*tResponseA)(const uint8_t* const, const uint8_t*, const uint8_t* const), TTarget(*tResponseB)(const uint8_t* const, const uint8_t*, const uint8_t* const)>
				static OCEAN_FORCE_INLINE void maxMagnitudeResponse(const uint8_t* const source0, const uint8_t* const source1, const uint8_t* const source2, TTarget* const responses);
		};

	public:

		/**
		 * Horizontal and vertical Scharr filter for a multi-channel frame, the channel response with largest magnitude is stored as filter response.
		 * If the target response data type is selected to be `int8_t`, each filter response  is normalized by 1/32 to fit into the value range [-128, 127].<br>
		 * If the target response data type is selected to be `int16_t` no normalization will be applied.
		 * The border pixels are set to zero.
		 * @param source The source frame to filter, must be valid
		 * @param target The target (response) frame receiving the filter responses, must be valid
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param sourcePaddingElements The padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam TTarget The data type of the response values, either `int8_t` or `int16_t`
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <typename TTarget, unsigned int tChannels>
		static inline void filterHorizontalVerticalAs1Channel8Bit(const uint8_t* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Diagonal (45 and 135 degree) Scharr filter for a multi-channel frame, the channel response with largest magnitude is stored as filter response.
		 * If the target response data type is selected to be `int8_t`, each filter response  is normalized by 1/32 to fit into the value range [-128, 127].<br>
		 * If the target response data type is selected to be `int16_t` no normalization will be applied.
		 * The border pixels are set to zero.
		 * @param source The source frame to filter, must be valid
		 * @param target The target (response) frame receiving the filter responses, must be valid
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param sourcePaddingElements The padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam TTarget The data type of the response values, either `int8_t` or `int16_t`
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <typename TTarget, unsigned int tChannels>
		static inline void filterDiagonalAs1Channel8Bit(const uint8_t* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Horizontal, vertical, and diagonal (45 and 135 degree) Scharr filter for a multi-channel frame, the channel response with largest magnitude is stored as filter response.
		 * If the target response data type is selected to be `int8_t`, each filter response  is normalized by 1/32 to fit into the value range [-128, 127].<br>
		 * If the target response data type is selected to be `int16_t` no normalization will be applied.
		 * The border pixels are set to zero.
		 * @param source The source frame to filter, must be valid
		 * @param target The target (response) frame receiving the filter responses, must be valid
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param sourcePaddingElements The padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam TTarget The data type of the response values, either `int8_t` or `int16_t`
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <typename TTarget, unsigned int tChannels>
		static inline void filterAs1Channel8Bit(const uint8_t* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

	private:

		/**
		 * Applies the horizontal and vertical Scharr filter for one row of a source frame, the channel response with largest magnitude is stored as filter response.
		 * @param sourceRow The row of the source frame, must be valid
		 * @param targetRow The row of the target response frame, must be valid
		 * @param width The width of the source and target frame in pixel, with range [3, infinity)
		 * @param height The height of the source and target frame in pixel, with range [3, infinity)
		 * @param rowIndex The index of the row to which the filter is applied, with range [0, height - 1]
		 * @param sourceStrideElements The number of elements between the start of two consecutive source rows, with range [width * tChannels, infinity)
		 * @param targetStrideElements The number of elements between the start of two consecutive target rows, with range [width * tChannels, infinity)
		 * @tparam TSource The data type of the source frame, must be `uint8_t`
		 * @tparam TTarget The data type oft he target response frame, must be `int8_t` or `int16_t`
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels>
		static void filterHorizontalVerticalAs1ChannelRow(const TSource* sourceRow, TTarget* targetRow, const unsigned int width, const unsigned int height, unsigned int rowIndex, const unsigned int sourceStrideElements, const unsigned int targetStrideElements);

		/**
		 * Applies the diagonal (45 and 135 degree) Scharr filter for one row of a source frame, the channel response with largest magnitude is stored as filter response.
		 * @param sourceRow The row of the source frame, must be valid
		 * @param targetRow The row of the target response frame, must be valid
		 * @param width The width of the source and target frame in pixel, with range [3, infinity)
		 * @param height The height of the source and target frame in pixel, with range [3, infinity)
		 * @param rowIndex The index of the row to which the filter is applied, with range [0, height - 1]
		 * @param sourceStrideElements The number of elements between the start of two consecutive source rows, with range [width * tChannels, infinity)
		 * @param targetStrideElements The number of elements between the start of two consecutive target rows, with range [width * tChannels, infinity)
		 * @tparam TSource The data type of the source frame, must be `uint8_t`
		 * @tparam TTarget The data type oft he target response frame, must be `int8_t` or `int16_t`
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels>
		static void filterDiagonalAs1ChannelRow(const TSource* sourceRow, TTarget* targetRow, const unsigned int width, const unsigned int height, unsigned int rowIndex, const unsigned int sourceStrideElements, const unsigned int targetStrideElements);

		/**
		 * Applies the horizontal, vertical, and diagonal (45 and 135 degree) Scharr filter for one row of a source frame, the channel response with largest magnitude is stored as filter response.
		 * @param sourceRow The row of the source frame, must be valid
		 * @param targetRow The row of the target response frame, must be valid
		 * @param width The width of the source and target frame in pixel, with range [3, infinity)
		 * @param height The height of the source and target frame in pixel, with range [3, infinity)
		 * @param rowIndex The index of the row to which the filter is applied, with range [0, height - 1]
		 * @param sourceStrideElements The number of elements between the start of two consecutive source rows, with range [width * tChannels, infinity)
		 * @param targetStrideElements The number of elements between the start of two consecutive target rows, with range [width * tChannels, infinity)
		 * @tparam TSource The data type of the source frame, must be `uint8_t`
		 * @tparam TTarget The data type oft he target response frame, must be `int8_t` or `int16_t`
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels>
		static void filterAs1ChannelRow(const TSource* sourceRow, TTarget* targetRow, const unsigned int width, const unsigned int height, unsigned int rowIndex, const unsigned int sourceStrideElements, const unsigned int targetStrideElements);
};

template <unsigned int tChannels>
template <typename TTarget>
OCEAN_FORCE_INLINE TTarget FrameFilterScharrMagnitude::PixelResponse<tChannels>::response0(const uint8_t* const source0, const uint8_t* const source1, const uint8_t* const source2)
{
	static_assert(std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value, "Invalid target data type!");
	static_assert(tChannels >= 1u, "Invalid channels!");

	ocean_assert(source0 != nullptr && source1 != nullptr && source2 != nullptr);

	// |  -3   0   3 |
	// | -10   0  10 |
	// |  -3   0   3 |

	if constexpr (std::is_same<TTarget, int8_t>::value)
	{
		return TTarget(((-int16_t(source0[0]) + int16_t(source0[tChannels * 2u]) - int16_t(source2[0]) + int16_t(source2[tChannels * 2u])) * int16_t(3) + (-int16_t(source1[0]) + int16_t(source1[tChannels * 2u])) * int16_t(10)) / int16_t(32));
	}
	else
	{
		ocean_assert((std::is_same<TTarget, int16_t>::value));

		return TTarget((-int16_t(source0[0]) + int16_t(source0[tChannels * 2u]) - int16_t(source2[0]) + int16_t(source2[tChannels * 2u])) * int16_t(3) + (-int16_t(source1[0]) + int16_t(source1[tChannels * 2u])) * int16_t(10));
	}
}

template <unsigned int tChannels>
template <typename TTarget>
OCEAN_FORCE_INLINE TTarget FrameFilterScharrMagnitude::PixelResponse<tChannels>::response90(const uint8_t* const source0, const uint8_t* const /*source1*/, const uint8_t* const source2)
{
	static_assert(std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value, "Invalid target data type!");
	static_assert(tChannels >= 1u, "Invalid channels!");

	ocean_assert(source0 != nullptr && source2 != nullptr);

	// | -3  -10  -3 |
	// |  0   0    0 |
	// |  3   10   3 |

	if constexpr (std::is_same<TTarget, int8_t>::value)
	{
		return TTarget(((-int16_t(source0[0]) - int16_t(source0[tChannels * 2u]) + int16_t(source2[0]) + int16_t(source2[tChannels * 2u])) * int16_t(3) + (-int16_t(source0[tChannels]) + int16_t(source2[tChannels])) * int16_t(10)) / int16_t(32));
	}
	else
	{
		ocean_assert((std::is_same<TTarget, int16_t>::value));

		return TTarget((-int16_t(source0[0]) - int16_t(source0[tChannels * 2u]) + int16_t(source2[0]) + int16_t(source2[tChannels * 2u])) * int16_t(3) + (-int16_t(source0[tChannels]) + int16_t(source2[tChannels])) * int16_t(10));
	}
}

template <unsigned int tChannels>
template <typename TTarget>
OCEAN_FORCE_INLINE TTarget FrameFilterScharrMagnitude::PixelResponse<tChannels>::response45(const uint8_t* const source0, const uint8_t* const source1, const uint8_t* const source2)
{
	static_assert(std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value, "Invalid target data type!");
	static_assert(tChannels >= 1u, "Invalid channels!");

	ocean_assert(source0 != nullptr && source2 != nullptr);

	// |  -10   -3     0 |
	// |  -3     0     3 |
	// |   0     3    10 |

	if constexpr (std::is_same<TTarget, int8_t>::value)
	{
		return TTarget(((int16_t(source2[tChannels * 2u]) - int16_t(source0[0])) * int16_t(10) + (int16_t(source2[tChannels]) + int16_t(source1[tChannels * 2u]) - int16_t(source1[0]) - int16_t(source0[tChannels])) * int16_t(3)) / int16_t(32));
	}
	else
	{
		ocean_assert((std::is_same<TTarget, int16_t>::value));

		return TTarget((int16_t(source2[tChannels * 2u]) - int16_t(source0[0])) * int16_t(10) + (int16_t(source2[tChannels]) + int16_t(source1[tChannels * 2u]) - int16_t(source1[0]) - int16_t(source0[tChannels])) * int16_t(3));
	}
}

template <unsigned int tChannels>
template <typename TTarget>
OCEAN_FORCE_INLINE TTarget FrameFilterScharrMagnitude::PixelResponse<tChannels>::response135(const uint8_t* const source0, const uint8_t* const source1, const uint8_t* const source2)
{
	static_assert(std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value, "Invalid target data type!");
	static_assert(tChannels >= 1u, "Invalid channels!");

	ocean_assert(source0 != nullptr && source2 != nullptr);

	// |  0   -3   -10 |
	// |  3    0    -3 |
	// | 10    3     0 |

	if constexpr (std::is_same<TTarget, int8_t>::value)
	{
		return TTarget(((int16_t(source2[0]) - int16_t(source0[tChannels * 2u])) * int16_t(10) + (int16_t(source1[0]) + int16_t(source2[tChannels]) - int16_t(source0[tChannels]) - int16_t(source1[tChannels * 2u])) * int16_t(3)) / int16_t(32));
	}
	else
	{
		ocean_assert((std::is_same<TTarget, int16_t>::value));

		return TTarget((int16_t(source2[0]) - int16_t(source0[tChannels * 2u])) * int16_t(10) + (int16_t(source1[0]) + int16_t(source2[tChannels]) - int16_t(source0[tChannels]) - int16_t(source1[tChannels * 2u])) * int16_t(3));
	}
}

template <>
template <typename TTarget, TTarget(*tResponseA)(const uint8_t* const, const uint8_t*, const uint8_t* const), TTarget(*tResponseB)(const uint8_t* const, const uint8_t*, const uint8_t* const)>
OCEAN_FORCE_INLINE void FrameFilterScharrMagnitude::PixelResponse<2u>::maxMagnitudeResponse(const uint8_t* const source0, const uint8_t* const source1, const uint8_t* const source2, TTarget* const response)
{
	ocean_assert(source0 != nullptr && source1 != nullptr && source2 != nullptr && response != nullptr);

	const TTarget a0 = tResponseA(source0 + 0, source1 + 0, source2 + 0);
	const TTarget a1 = tResponseA(source0 + 1, source1 + 1, source2 + 1);

	const TTarget b0 = tResponseB(source0 + 0, source1 + 0, source2 + 0);
	const TTarget b1 = tResponseB(source0 + 1, source1 + 1, source2 + 1);

	const uint32_t magnitude0 = uint32_t(a0 * a0 + b0 * b0);
	const uint32_t magnitude1 = uint32_t(a1 * a1 + b1 * b1);

	if (magnitude0 >= magnitude1)
	{
		response[0] = a0;
		response[1] = b0;
	}
	else
	{
		response[0] = a1;
		response[1] = b1;
	}
}

template <>
template <typename TTarget, TTarget(*tResponseA)(const uint8_t* const, const uint8_t*, const uint8_t* const), TTarget(*tResponseB)(const uint8_t* const, const uint8_t*, const uint8_t* const)>
OCEAN_FORCE_INLINE void FrameFilterScharrMagnitude::PixelResponse<3u>::maxMagnitudeResponse(const uint8_t* const source0, const uint8_t* const source1, const uint8_t* const source2, TTarget* const response)
{
	ocean_assert(source0 != nullptr && source1 != nullptr && source2 != nullptr && response != nullptr);

	const TTarget a0 = tResponseA(source0 + 0, source1 + 0, source2 + 0);
	const TTarget a1 = tResponseA(source0 + 1, source1 + 1, source2 + 1);
	const TTarget a2 = tResponseA(source0 + 2, source1 + 2, source2 + 2);

	const TTarget b0 = tResponseB(source0 + 0, source1 + 0, source2 + 0);
	const TTarget b1 = tResponseB(source0 + 1, source1 + 1, source2 + 1);
	const TTarget b2 = tResponseB(source0 + 2, source1 + 2, source2 + 2);

	const uint32_t magnitude0 = uint32_t(a0 * a0 + b0 * b0);
	const uint32_t magnitude1 = uint32_t(a1 * a1 + b1 * b1);
	const uint32_t magnitude2 = uint32_t(a2 * a2 + b2 * b2);

	if (magnitude0 >= magnitude1)
	{
		if (magnitude0 >= magnitude2)
		{
			response[0] = a0;
			response[1] = b0;
		}
		else
		{
			response[0] = a2;
			response[1] = b2;
		}
	}
	else
	{
		if (magnitude1 >= magnitude2)
		{
			response[0] = a1;
			response[1] = b1;
		}
		else
		{
			response[0] = a2;
			response[1] = b2;
		}
	}
}

template <>
template <typename TTarget, TTarget(*tResponseA)(const uint8_t* const, const uint8_t*, const uint8_t* const), TTarget(*tResponseB)(const uint8_t* const, const uint8_t*, const uint8_t* const)>
OCEAN_FORCE_INLINE void FrameFilterScharrMagnitude::PixelResponse<4u>::maxMagnitudeResponse(const uint8_t* const source0, const uint8_t* const source1, const uint8_t* const source2, TTarget* const response)
{
	ocean_assert(source0 != nullptr && source1 != nullptr && source2 != nullptr && response != nullptr);

	const TTarget a0 = tResponseA(source0 + 0, source1 + 0, source2 + 0);
	const TTarget a1 = tResponseA(source0 + 1, source1 + 1, source2 + 1);
	const TTarget a2 = tResponseA(source0 + 2, source1 + 2, source2 + 2);
	const TTarget a3 = tResponseA(source0 + 3, source1 + 3, source2 + 3);

	const TTarget b0 = tResponseB(source0 + 0, source1 + 0, source2 + 0);
	const TTarget b1 = tResponseB(source0 + 1, source1 + 1, source2 + 1);
	const TTarget b2 = tResponseB(source0 + 2, source1 + 2, source2 + 2);
	const TTarget b3 = tResponseB(source0 + 3, source1 + 3, source2 + 3);

	const uint32_t magnitude0 = uint32_t(a0 * a0 + b0 * b0);
	const uint32_t magnitude1 = uint32_t(a1 * a1 + b1 * b1);
	const uint32_t magnitude2 = uint32_t(a2 * a2 + b2 * b2);
	const uint32_t magnitude3 = uint32_t(a3 * a3 + b3 * b3);

	if (magnitude0 >= magnitude1)
	{
		if (magnitude0 >= magnitude2)
		{
			if (magnitude0 >= magnitude3)
			{
				response[0] = a0;
				response[1] = b0;
			}
			else
			{
				response[0] = a3;
				response[1] = b3;
			}
		}
		else
		{
			if (magnitude2 >= magnitude3)
			{
				response[0] = a2;
				response[1] = b2;
			}
			else
			{
				response[0] = a3;
				response[1] = b3;
			}
		}
	}
	else
	{
		if (magnitude1 >= magnitude2)
		{
			if (magnitude1 >= magnitude3)
			{
				response[0] = a1;
				response[1] = b1;
			}
			else
			{
				response[0] = a3;
				response[1] = b3;
			}
		}
		else
		{
			if (magnitude2 >= magnitude3)
			{
				response[0] = a2;
				response[1] = b2;
			}
			else
			{
				response[0] = a3;
				response[1] = b3;
			}
		}
	}
}

template <typename TTarget, unsigned int tChannels>
inline void FrameFilterScharrMagnitude::filterHorizontalVerticalAs1Channel8Bit(const uint8_t* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value, "Invalid target data type!");
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 3u && height >= 3u);

	if constexpr (tChannels == 1u)
	{
		FrameFilterScharr::template filterHorizontalVertical8BitPerChannel<TTarget, 1u>(source, target, width, height, sourcePaddingElements, targetPaddingElements, worker);
	}
	else
	{
		constexpr unsigned int tTargetChannels = 2u;

		FrameChannels::applyRowOperator<uint8_t, TTarget, tChannels, tTargetChannels>(source, target, width, height, sourcePaddingElements, targetPaddingElements, &filterHorizontalVerticalAs1ChannelRow<uint8_t, TTarget, tChannels>, worker);
	}
}

template <typename TTarget, unsigned int tChannels>
inline void FrameFilterScharrMagnitude::filterDiagonalAs1Channel8Bit(const uint8_t* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value, "Invalid target data type!");
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 3u && height >= 3u);

	if constexpr (tChannels == 1u)
	{
		FrameFilterScharr::template filterDiagonal8BitPerChannel<TTarget, 1u>(source, target, width, height, sourcePaddingElements, targetPaddingElements, worker);
	}
	else
	{
		constexpr unsigned int tTargetChannels = 2u;

		FrameChannels::applyRowOperator<uint8_t, TTarget, tChannels, tTargetChannels>(source, target, width, height, sourcePaddingElements, targetPaddingElements, &filterDiagonalAs1ChannelRow<uint8_t, TTarget, tChannels>, worker);
	}
}

template <typename TTarget, unsigned int tChannels>
inline void FrameFilterScharrMagnitude::filterAs1Channel8Bit(const uint8_t* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value, "Invalid target data type!");
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 3u && height >= 3u);

	if constexpr (tChannels == 1u)
	{
		FrameFilterScharr::template filter8BitPerChannel<TTarget, 1u>(source, target, width, height, sourcePaddingElements, targetPaddingElements, worker);
	}
	else
	{
		constexpr unsigned int tTargetChannels = 4u;

		FrameChannels::applyRowOperator<uint8_t, TTarget, tChannels, tTargetChannels>(source, target, width, height, sourcePaddingElements, targetPaddingElements, &filterAs1ChannelRow<uint8_t, TTarget, tChannels>, worker);
	}
}

template <typename TSource, typename TTarget, unsigned int tSourceChannels>
void FrameFilterScharrMagnitude::filterHorizontalVerticalAs1ChannelRow(const TSource* sourceRow, TTarget* targetRow, const unsigned int width, const unsigned int height, unsigned int rowIndex, const unsigned int sourceStrideElements, const unsigned int /*targetStrideElements*/)
{
	static_assert(std::is_same<TSource, uint8_t>::value, "Invalid source data type!");
	static_assert(std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value, "Invalid target data type!");

	static_assert(tSourceChannels >= 1u, "Invalid source channel number!");

	constexpr unsigned int tTargetChannels = 2u;

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
		PixelResponse<tSourceChannels>::template maxMagnitudeResponse<TTarget, PixelResponse<tSourceChannels>::template response0<TTarget>, PixelResponse<tSourceChannels>::template response90<TTarget>>(source0, source1, source2, targetRow);

		source0 += tSourceChannels;
		source1 += tSourceChannels;
		source2 += tSourceChannels;

		targetRow += tTargetChannels;
	}

	// setting last pixel to zero
	for (unsigned int n = 0u; n < tTargetChannels; ++n)
	{
		targetRow[n] = TTarget(0);
	}
}

template <typename TSource, typename TTarget, unsigned int tSourceChannels>
void FrameFilterScharrMagnitude::filterDiagonalAs1ChannelRow(const TSource* sourceRow, TTarget* targetRow, const unsigned int width, const unsigned int height, unsigned int rowIndex, const unsigned int sourceStrideElements, const unsigned int /*targetStrideElements*/)
{
	static_assert(std::is_same<TSource, uint8_t>::value, "Invalid source data type!");
	static_assert(std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value, "Invalid target data type!");

	static_assert(tSourceChannels >= 1u, "Invalid source channel number!");

	constexpr unsigned int tTargetChannels = 2u;

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
		PixelResponse<tSourceChannels>::template maxMagnitudeResponse<TTarget, PixelResponse<tSourceChannels>::template response45<TTarget>, PixelResponse<tSourceChannels>::template response135<TTarget>>(source0, source1, source2, targetRow);

		source0 += tSourceChannels;
		source1 += tSourceChannels;
		source2 += tSourceChannels;

		targetRow += tTargetChannels;
	}

	// setting last pixel to zero
	for (unsigned int n = 0u; n < tTargetChannels; ++n)
	{
		targetRow[n] = TTarget(0);
	}
}

template <typename TSource, typename TTarget, unsigned int tSourceChannels>
void FrameFilterScharrMagnitude::filterAs1ChannelRow(const TSource* sourceRow, TTarget* targetRow, const unsigned int width, const unsigned int height, unsigned int rowIndex, const unsigned int sourceStrideElements, const unsigned int /*targetStrideElements*/)
{
	static_assert(std::is_same<TSource, uint8_t>::value, "Invalid source data type!");
	static_assert(std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value, "Invalid target data type!");

	static_assert(tSourceChannels >= 1u, "Invalid source channel number!");

	constexpr unsigned int tTargetChannels = 4u;

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
		PixelResponse<tSourceChannels>::template maxMagnitudeResponse<TTarget, PixelResponse<tSourceChannels>::template response0<TTarget>, PixelResponse<tSourceChannels>::template response90<TTarget>>(source0, source1, source2, targetRow);
		PixelResponse<tSourceChannels>::template maxMagnitudeResponse<TTarget, PixelResponse<tSourceChannels>::template response45<TTarget>, PixelResponse<tSourceChannels>::template response135<TTarget>>(source0, source1, source2, targetRow + 2);

		source0 += tSourceChannels;
		source1 += tSourceChannels;
		source2 += tSourceChannels;

		targetRow += tTargetChannels;
	}

	// setting last pixel to zero
	for (unsigned int n = 0u; n < tTargetChannels; ++n)
	{
		targetRow[n] = TTarget(0);
	}
}

}

}

#endif // META_OCEAN_CV_FRAME_FILTER_SCHARR_MAGNITUDE_H
