/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_CONVERTER_RGBA_32_H
#define META_OCEAN_CV_FRAME_CONVERTER_RGBA_32_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameChannels.h"

namespace Ocean
{

namespace CV
{

/**
 * This class provides functions to convert frames with RGBA32 pixel format.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameConverterRGBA32 : public FrameConverter
{
	public:

		/**
		 * Converts an RGBA 32 bit frame to a ABGR 32 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker to distribute the computation to several CPU cores
		 */
		static inline void convertRGBA32ToABGR32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts an RGBA 32 bit frame to a ARGB 32 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker to distribute the computation to several CPU cores
		 */
		static inline void convertRGBA32ToARGB32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts an RGBA 32 bit frame to a BGR 24 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker to distribute the computation to several CPU cores
		 */
		static inline void convertRGBA32ToBGR24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker);

		/**
		 * Converts an RGBA 32 bit frame to a BGR 32 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker to distribute the computation to several CPU cores
		 * @remarks Information in alpha channel of source frame is discarded during conversion.  The unused last channel of each pixel in target frame is set to zero.
		 */
		static inline void convertRGBA32ToBGR32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker);

		/**
		 * Converts an RGBA 32 bit frame to a BGRA 32 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker to distribute the computation to several CPU cores
		 */
		static inline void convertRGBA32ToBGRA32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts an RGBA 32 bit frame to a RGBA 32 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker to distribute the computation to several CPU cores
		 */
		static inline void convertRGBA32ToRGBA32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts an RGBA 32 bit frame to a RGB 24 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker to distribute the computation to several CPU cores
		 */
		static inline void convertRGBA32ToRGB24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts an RGBA 32 bit frame to a RGB 32 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker to distribute the computation to several CPU cores
		 * @remarks Information in alpha channel of source frame is discarded during conversion.  The unused last channel of each pixel in target frame is set to zero.
		 */
	static inline void convertRGBA32ToRGB32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker);

		/**
		 * Converts a RGBA 32 bit frame to a Y 8 bit grayscale frame by the exact conversion.
		 * Gray = Red * 0.299 + Green * 0.587 + Blue * 0.114
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker to distribute the computation to several CPU cores
		 */
		static inline void convertRGBA32ToY8(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker);

		/**
		 * Converts a RGBA 32 bit frame to a YA 16 bit frame by the exact conversion.
		 * Gray = Red * 0.299 + Green * 0.587 + Blue * 0.114
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker to distribute the computation to several CPU cores
		 */
		static inline void convertRGBA32ToYA16(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a RGBA 32 bit frame to a YUV 24 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker to distribute the computation to several CPU cores
		 */
		static inline void convertRGBA32ToYUV24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker);
};

inline void FrameConverterRGBA32::convertRGBA32ToABGR32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	FrameChannels::reverseChannelOrder<uint8_t, 4u>(source, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterRGBA32::convertRGBA32ToARGB32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	// source frame R G B A
	//              0 1 2 3
	// target frame A R G B
	// pattern      3 0 1 2
	constexpr unsigned int shufflePattern = 0x2103;

	FrameChannels::shuffleChannels<uint8_t, 4u, 4u, shufflePattern>(source, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterRGBA32::convertRGBA32ToBGR24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	// source frame R G B A
	//              0 1 2 3
	// target frame B G R
	// pattern      2 1 0
	constexpr unsigned int shufflePattern = 0x012;

	FrameChannels::shuffleChannels<uint8_t, 4u, 3u, shufflePattern>(source, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterRGBA32::convertRGBA32ToBGR32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	// source frame R G B A
	//              0 1 2 3
	// target frame B G R -
	// pattern      2 1 0
	constexpr unsigned int shufflePattern = 0x012u;

	FrameChannels::shuffleChannelsAndSetLastChannelValue<uint8_t, 4u, 4u, shufflePattern>(source, 0u, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterRGBA32::convertRGBA32ToBGRA32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	// source frame R G B A
	//              0 1 2 3
	// target frame B G R A
	// pattern      2 1 0 3
	constexpr unsigned int shufflePattern = 0x3012;

	FrameChannels::shuffleChannels<uint8_t, 4u, 4u, shufflePattern>(source, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterRGBA32::convertRGBA32ToRGBA32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	FrameChannels::transformGeneric<uint8_t, 4u>(source, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterRGBA32::convertRGBA32ToRGB24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	FrameChannels::removeLastChannel<uint8_t, 4u>(source, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterRGBA32::convertRGBA32ToRGB32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	// source frame R G B A
	//              0 1 2 3
	// target frame R G B -
	// pattern      0 1 2
	constexpr unsigned int shufflePattern = 0x210u;

	FrameChannels::shuffleChannelsAndSetLastChannelValue<uint8_t, 4u, 4u, shufflePattern>(source, 0u, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterRGBA32::convertRGBA32ToY8(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width * 4u + sourcePaddingElements;
	const unsigned int targetStrideElements = width + targetPaddingElements;

	constexpr unsigned int channelFactors[4] = {38u, 75u, 15u, 0u};

	static_assert(channelFactors[0] + channelFactors[1] + channelFactors[2] + channelFactors[3] == 128u, "Invalid factors!");

	constexpr bool useFactorChannel0 = channelFactors[0] != 0u;
	constexpr bool useFactorChannel1 = channelFactors[1] != 0u;
	constexpr bool useFactorChannel2 = channelFactors[2] != 0u;
	constexpr bool useFactorChannel3 = channelFactors[3] != 0u;

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, flag, CV::FrameChannels::convertRow4ChannelsTo1Channel8BitPerChannel7BitPrecision<useFactorChannel0, useFactorChannel1, useFactorChannel2, useFactorChannel3>, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 1u>, areContinuous, channelFactors, worker);
}

inline void FrameConverterRGBA32::convertRGBA32ToYA16(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width * 4u + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 2u + targetPaddingElements;

	constexpr unsigned int channelFactors[8] = {38u, 0u, 75u, 0u, 15u, 0u, 0u, 128u};

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, flag, CV::FrameChannels::convertRow4ChannelsTo2Channels8BitPerChannel7BitPrecision, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 2u>, areContinuous, channelFactors, worker);
}

inline void FrameConverterRGBA32::convertRGBA32ToYUV24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width * 4u + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 3u + targetPaddingElements;

	// approximation:
	// Y = ( 33 * R + 64 * G + 13 * B + 0 * A) / 128 + 16
	// U = (-19 * R - 37 * G + 56 * B + 0 * A) / 128 + 128
	// V = ( 56 * R - 47 * G -  9 * B + 0 * A) / 128 + 128

	const int parameters[15] = {33, -19, 56, 64, -37, -47, 13, 56, -9, 0, 0, 0, 16, 128, 128};

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, flag, CV::FrameChannels::convertRow4ChannelsTo3Channels8BitPerChannel7BitPrecision, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>, areContinuous, parameters, worker);
}

}

}

#endif // META_OCEAN_CV_FRAME_CONVERTER_RGBA_32_H
