/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_CONVERTER_BGR_24_H
#define META_OCEAN_CV_FRAME_CONVERTER_BGR_24_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameChannels.h"

namespace Ocean
{

// Forward declaration.
class Worker;

namespace CV
{

/**
 * This class provides functions to convert frames with BGR pixel format.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameConverterBGR24 : public FrameConverter
{
	public:

		/**
		 * Converts a BGR 24 bit frame to a BGR 24 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void convertBGR24ToBGR24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a BGR 24 bit frame to a BGR 24 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param alphaValue The value of the alpha channel to be set, with range [0, 255]
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void convertBGR24ToBGRA32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue = 0xFF, Worker* worker = nullptr);

		/**
		 * Converts a BGR 24 bit frame to a RGB 24 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void convertBGR24ToRGB24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a BGR 24 bit frame to a RGBA 32 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param alphaValue The value of the alpha channel to be set, with range [0, 255]
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void convertBGR24ToRGBA32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue = 0xFF, Worker* worker = nullptr);

		/**
		 * Converts a BGR 24 bit frame to a gray scale frame.
		 * Gray = Red * 0.299 + Green * 0.587 + Blue * 0.114
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void convertBGR24ToY8(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a BGR 24 bit frame to a YUV 24 bit frame by the exact conversion.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void convertBGR24ToYUV24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a full range BGR24 frame to a limited range Y_U_V12 frame with 7-bit precision using BT.601.
		 * <pre>
		 * BGR input value range:  [ 0, 255]x[ 0, 255]x[ 0, 255]
		 * YUV input value range:  [16, 235]x[16, 240]x[16, 240]
		 * </pre>
		 * @param source The source frame plane, must be valid
		 * @param yTarget The y-target frame plane, must be valid
		 * @param uTarget The u-target frame plane, must be valid
		 * @param vTarget The v-target frame plane, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param yTargetPaddingElements Optional number of padding elements at the end of each y-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param uTargetPaddingElements Optional number of padding elements at the end of each u-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param vTargetPaddingElements Optional number of padding elements at the end of each v-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertBGR24FullRangeToY_U_V12LimitedRange(const uint8_t* source, uint8_t* yTarget, uint8_t* uTarget, uint8_t* vTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int uTargetPaddingElements, const unsigned int vTargetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a full range BGR24 frame to a limited range Y_V_U12 frame with 7-bit precision using BT.601.
		 * <pre>
		 * BGR input value range:  [ 0, 255]x[ 0, 255]x[ 0, 255]
		 * YUV input value range:  [16, 235]x[16, 240]x[16, 240]
		 * </pre>
		 * @param source The source frame plane, must be valid
		 * @param yTarget The y-target frame plane, must be valid
		 * @param vTarget The v-target frame plane, must be valid
		 * @param uTarget The u-target frame plane, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param yTargetPaddingElements Optional number of padding elements at the end of each y-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param vTargetPaddingElements Optional number of padding elements at the end of each v-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param uTargetPaddingElements Optional number of padding elements at the end of each u-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertBGR24FullRangeToY_V_U12LimitedRange(const uint8_t* source, uint8_t* yTarget, uint8_t* vTarget, uint8_t* uTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int vTargetPaddingElements, const unsigned int uTargetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a full range BGR24 frame to a full range Y_U_V12 frame with 7-bit precision using BT.601.
		 * <pre>
		 * BGR input value range:  [0, 255]x[0, 255]x[0, 255]
		 * YUV output value range: [0, 255]x[0, 255]x[0, 255]
		 * </pre>
		 * @param source The source frame plane, must be valid
		 * @param yTarget The y-target frame plane, must be valid
		 * @param uTarget The u-target frame plane, must be valid
		 * @param vTarget The v-target frame plane, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param yTargetPaddingElements Optional number of padding elements at the end of each y-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param uTargetPaddingElements Optional number of padding elements at the end of each u-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param vTargetPaddingElements Optional number of padding elements at the end of each v-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertBGR24FullRangeToY_U_V12FullRange(const uint8_t* source, uint8_t* yTarget, uint8_t* uTarget, uint8_t* vTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int uTargetPaddingElements, const unsigned int vTargetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a full range BGR24 frame to a full range Y_V_U12 frame with 7-bit precision using BT.601.
		 * <pre>
		 * BGR input value range:  [0, 255]x[0, 255]x[0, 255]
		 * YVU output value range: [0, 255]x[0, 255]x[0, 255]
		 * </pre>
		 * @param source The source frame plane, must be valid
		 * @param yTarget The y-target frame plane, must be valid
		 * @param vTarget The v-target frame plane, must be valid
		 * @param uTarget The u-target frame plane, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param yTargetPaddingElements Optional number of padding elements at the end of each y-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param vTargetPaddingElements Optional number of padding elements at the end of each v-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param uTargetPaddingElements Optional number of padding elements at the end of each u-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertBGR24FullRangeToY_V_U12FullRange(const uint8_t* source, uint8_t* yTarget, uint8_t* vTarget, uint8_t* uTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int vTargetPaddingElements, const unsigned int uTargetPaddingElements, Worker* worker = nullptr);
};

inline void FrameConverterBGR24::convertBGR24ToBGR24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	FrameChannels::transformGeneric<uint8_t, 3u>(source, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterBGR24::convertBGR24ToBGRA32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	FrameChannels::addLastChannelValue<uint8_t, 3u>(source, alphaValue, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterBGR24::convertBGR24ToRGB24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	FrameChannels::reverseChannelOrder<uint8_t, 3u>(source, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterBGR24::convertBGR24ToRGBA32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	// source frame B G R
	//              0 1 2
	// target frame R G B A
	// pattern      2 1 0
	constexpr unsigned int shufflePattern = 0x012u;

	FrameChannels::shuffleChannelsAndSetLastChannelValue<uint8_t, 3u, 4u, shufflePattern>(source, alphaValue, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterBGR24::convertBGR24ToY8(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width * 3u + sourcePaddingElements;
	const unsigned int targetStrideElements = width + targetPaddingElements;

	constexpr unsigned int channelFactors[3] = {15u, 75u, 38u}; // 15/128 ~ 0.114, 75/128 ~ 0.587, 38/128 ~ 0.299

	static_assert(channelFactors[0] + channelFactors[1] + channelFactors[2] == 128u, "Invalid factors!");

	constexpr bool useFactorChannel0 = channelFactors[0] != 0u;
	constexpr bool useFactorChannel1 = channelFactors[1] != 0u;
	constexpr bool useFactorChannel2 = channelFactors[2] != 0u;

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, flag, CV::FrameChannels::convertRow3ChannelsTo1Channel8BitPerChannel7BitPrecision<useFactorChannel0, useFactorChannel1, useFactorChannel2>, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 1u>, areContinuous, channelFactors, worker);
}

inline void FrameConverterBGR24::convertBGR24ToYUV24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width * 3u + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 3u + targetPaddingElements;

	// approximation:
	// Y = ( 33 * R + 64 * G + 13 * B) / 128 + 16
	// U = (-19 * R - 37 * G + 56 * B) / 128 + 128
	// V = ( 56 * R - 47 * G -  9 * B) / 128 + 128

	const int parameters[12] = {13, 56, -9, 64, -37, -47, 33, -19, 56, 16, 128, 128};

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, flag, CV::FrameChannels::convertRow3ChannelsTo3Channels8BitPerChannel7BitPrecision, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>, areContinuous, parameters, worker);
}

inline void FrameConverterBGR24::convertBGR24FullRangeToY_U_V12LimitedRange(const uint8_t* source, uint8_t* yTarget, uint8_t* uTarget, uint8_t* vTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int uTargetPaddingElements, const unsigned int vTargetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && yTarget != nullptr && uTarget != nullptr && vTarget != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	/*
	 * BGR input value range:  [ 0, 255]x[ 0, 255]x[ 0, 255]
	 * YUV output value range: [16, 235]x[16, 240]x[16, 240]
	 *
	 * | Y |   |  0.09765625   0.5039063   0.2578125  16.0  |   | B |
	 * | U | = |  0.4375      -0.2890625  -0.1484375  128.0 | * | G |
	 * | V |   | -0.0703125   -0.3671875   0.4375     128.0 |   | R |
	 *                                                          | 1 |
	 * Approximation with 7 bit precision:
	 *       | Y |     |  13     64     33    16 * 128 |   | B |
	 * 128 * | U |  =  |  56    -37    -19   128 * 128 | * | G |
	 *       | V |     | -9     -47     56   128 * 128 |   | R |
	 *                                                     | 1 |
	 */

	const int options[4 + 9 + 3] =
	{
		// padding parameters
		int(sourcePaddingElements), int(yTargetPaddingElements), int(uTargetPaddingElements), int(vTargetPaddingElements),

		// multiplication parameters
		13, 56, -9, 64, -37, -47, 33, -19, 56,

		// bias/translation parameters
		16, 128, 128
	};

	void* target[3] =
	{
		yTarget,
		uTarget,
		vTarget
	};

	FrameConverter::convertArbitraryPixelFormat((const void**)(&source), target, width, height, flag, 2u, FrameConverter::convertTwoRows_1Plane3Channels_To_1Plane1ChannelAnd2Planes1ChannelsDownsampled2x2_8BitPerChannel_Precision7Bit, options, worker);
}

inline void FrameConverterBGR24::convertBGR24FullRangeToY_V_U12LimitedRange(const uint8_t* source, uint8_t* yTarget, uint8_t* vTarget, uint8_t* uTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int vTargetPaddingElements, const unsigned int uTargetPaddingElements, Worker* worker)
{
	// just swapping u- and v-plane, and calling converter to Y_U_V12

	convertBGR24FullRangeToY_U_V12LimitedRange(source, yTarget, uTarget, vTarget, width, height, flag, sourcePaddingElements, yTargetPaddingElements, uTargetPaddingElements, vTargetPaddingElements, worker);
}

inline void FrameConverterBGR24::convertBGR24FullRangeToY_U_V12FullRange(const uint8_t* source, uint8_t* yTarget, uint8_t* uTarget, uint8_t* vTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int uTargetPaddingElements, const unsigned int vTargetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && yTarget != nullptr && uTarget != nullptr && vTarget != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	/*
	 * BGR input value range:  [0, 255]x[0, 255]x[0, 255]
	 * YUV output value range: [0, 255]x[0, 255]x[0, 255]
	 *
	 * | Y |   |  0.114       0.587       0.299        0 |   | B |
	 * | U | = |  0.5        -0.331264   -0.168736   128 | * | G |
	 * | V |   | -0.081312   -0.418688    0.5        128 |   | R |
	 *                                                       | 1 |
	 * Approximation with 7 bit precision:
	 *       | Y |     |   15    75    38      0 * 128 |   | B |
	 * 128 * | U |  =  |   64   -42   -22    128 * 128 | * | G |
	 *       | V |     |  -10   -54    64    128 * 128 |   | R |
	 *                                                     | 1 |
	 */

	const int options[4 + 9 + 3] =
	{
		// padding parameters
		int(sourcePaddingElements), int(yTargetPaddingElements), int(uTargetPaddingElements), int(vTargetPaddingElements),

		// multiplication parameters
		15, 64, -10, 75, -42, -54, 38, -22, 64,

		// bias/translation parameters
		0, 128, 128
	};

	void* target[3] =
	{
		yTarget,
		uTarget,
		vTarget
	};

	FrameConverter::convertArbitraryPixelFormat((const void**)(&source), target, width, height, flag, 2u, FrameConverter::convertTwoRows_1Plane3Channels_To_1Plane1ChannelAnd2Planes1ChannelsDownsampled2x2_8BitPerChannel_Precision7Bit, options, worker);
}

inline void FrameConverterBGR24::convertBGR24FullRangeToY_V_U12FullRange(const uint8_t* source, uint8_t* yTarget, uint8_t* vTarget, uint8_t* uTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int vTargetPaddingElements, const unsigned int uTargetPaddingElements, Worker* worker)
{
	// just swapping u- and v-plane, and calling converter to Y_U_V12

	convertBGR24FullRangeToY_U_V12FullRange(source, yTarget, uTarget, vTarget, width, height, flag, sourcePaddingElements, yTargetPaddingElements, uTargetPaddingElements, vTargetPaddingElements, worker);
}

}

}

#endif // META_OCEAN_CV_FRAME_CONVERTER_BGR_24_H
