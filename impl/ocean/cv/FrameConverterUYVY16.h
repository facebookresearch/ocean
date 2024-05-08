/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_CONVERTER_UYVY_16_H
#define META_OCEAN_CV_FRAME_CONVERTER_UYVY_16_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class provides functions to convert frames with UYVY16 pixel format.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameConverterUYVY16 : public FrameConverter
{
	public:

		/**
		 * Converts a UYVY16 frame to a 24 bit BGR frame into a second image buffer.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertUYVY16ToBGR24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a UYVY16 frame to a 24 bit RGB frame into a second image buffer.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertUYVY16ToRGB24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a UYVY16 frame to a 24 bit YUV frame into a second image buffer.
		 * @param source The source frame buffer
		 * @param target The target frame buffer
		 * @param width The width of the frame in pixel, must be a multiple of two
		 * @param height The height of the frame in pixel
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertUYVY16ToYUV24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a UYVY16 frame to a 24 bit YVU frame into a second image buffer.
		 * @param source The source frame buffer
		 * @param target The target frame buffer
		 * @param width The width of the frame in pixel, must be a multiple of two
		 * @param height The height of the frame in pixel
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertUYVY16ToYVU24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a UYVY16 frame to a 8 bit grayscale frame into a second image buffer.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertUYVY16ToY8(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);
};

inline void FrameConverterUYVY16::convertUYVY16ToBGR24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 1u);

	if (width < 2u || height < 1u || width % 2u != 0u)
	{
		return;
	}

	//        | B |   | 2066    1192     0     -277 * 1024 |   | U |
	// 1024 * | G | = | -400    1192   -833     135 * 1024 | * | Y |
	//        | R |   |   0     1192    1634   -223 * 1024 |   | V |
	//                                                         | 1 |

	const int options[14] =
	{
		// padding elements
		int(sourcePaddingElements), int(targetPaddingElements),

		// multiplication factors, with denominator 1024
		2066, -400, 0, 1192, 1192, 1192, 0, -833, 1634,

		// bias values, with denominator 1
		-277, 135, -223
	};

	FrameConverter::convertArbitraryPixelFormat((const void**)(&source), (void**)(&target), width, height, flag, 1u, FrameConverter::convertOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel_Precision10Bit, options, worker);
}

inline void FrameConverterUYVY16::convertUYVY16ToRGB24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 1u);

	if (width < 2u || height < 1u || width % 2u != 0u)
	{
		return;
	}

	//        | R |   |    0     1192    1634   -223 * 1024 |   | U |
	// 1024 * | G | = |  -400    1192   -833     135 * 1024 | * | Y |
	//        | B |   |  2066    1192    0      -277 * 1024 |   | V |
	//                                                          | 1 |

	const int options[14] =
	{
		// padding elements
		int(sourcePaddingElements), int(targetPaddingElements),

		// multiplication factors, with denominator 1024
		0, -400, 2066, 1192, 1192, 1192, 1634, -833, 0,

		// bias values, with denominator 1
		-223, 135, -277
	};

	FrameConverter::convertArbitraryPixelFormat((const void**)(&source), (void**)(&target), width, height, flag, 1u, FrameConverter::convertOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel_Precision10Bit, options, worker);
}

inline void FrameConverterUYVY16::convertUYVY16ToYUV24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 1u);

	if (width < 2u || height < 1u || width % 2u != 0u)
	{
		return;
	}

	const unsigned int options[2] = {sourcePaddingElements, targetPaddingElements};

	FrameConverter::convertArbitraryPixelFormat((const void**)(&source), (void**)(&target), width, height, flag, 1u, FrameConverter::mapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel<1u, 0u, 2u>, options, worker);
}

inline void FrameConverterUYVY16::convertUYVY16ToYVU24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 1u);

	if (width < 2u || height < 1u || width % 2u != 0u)
	{
		return;
	}

	const unsigned int options[2] = {sourcePaddingElements, targetPaddingElements};

	FrameConverter::convertArbitraryPixelFormat((const void**)(&source), (void**)(&target), width, height, flag, 1u, FrameConverter::mapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel<1u, 2u, 0u>, options, worker);
}

inline void FrameConverterUYVY16::convertUYVY16ToY8(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 1u);

	if (width % 2u != 0u || height == 0u)
	{
		return;
	}

	// we simply treat YUYV as e.g. AY

	// source frame U/V Y
	//               0  1
	// target frame Y
	// pattern      1
	constexpr unsigned int shufflePattern = 0x1u;

	FrameChannels::shuffleChannels<uint8_t, 2u, 1u, shufflePattern>(source, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

}

}

#endif // META_OCEAN_CV_FRAME_CONVERTER_UYVY_16_H
