/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_CONVERTER_YUV_24_H
#define META_OCEAN_CV_FRAME_CONVERTER_YUV_24_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class provides functions to convert frames with YUV24 pixel format to other pixel formats.
 * See Frame::PixelFormat for details of the YUV24 pixel format.<br>
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameConverterYUV24 : public FrameConverter
{
	public:

		/**
		 * Converts a YUV 24 bit frame to a BGR 24 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void convertYUV24ToBGR24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a YUV 24 bit frame to a BGRA 32 bit frame with 6 bit precision.
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
		static inline void convertYUV24ToBGRA32Precision6Bit(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue = 0xFF, Worker* worker = nullptr);

		/**
		 * Converts a YUV 24 bit frame to a RGB 24 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param flag Determining the type of conversion
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void convertYUV24ToRGB24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a YUV 24 bit frame to a RGB 24 bit frame with 6 bit precision.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void convertYUV24ToRGB24Precision6Bit(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a YUV 24 bit frame to a Y 8 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void convertYUV24ToY8(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a YUV 24 bit frame to a YUV 24 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void convertYUV24ToYUV24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a YUV 24 bit frame to a YVU 24 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void convertYUV24ToYVU24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a YUV 24 bit frame to a Y_U_V 12 bit frame.
		 * @param source The source frame buffer, with (width * 3 + targetPaddingElements) * height elements, must be valid
		 * @param yTarget The y target frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param uTarget The u target frame buffer, with (2 * width/2 + vPaddingElements) * height/2 elements, must be valid
		 * @param vTarget The v target frame buffer, with (2 * width/2 + uPaddingElements) * height/2 elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in (uint8_t) elements, with range [0, infinity)
		 * @param yTargetPaddingElements The number of padding elements at the end of each y-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param vTargetPaddingElements The number of padding elements at the end of each v-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param uTargetPaddingElements The number of padding elements at the end of each u-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void convertYUV24ToY_U_V12(const uint8_t* source, uint8_t* yTarget, uint8_t* uTarget, uint8_t* vTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int uTargetPaddingElements, const unsigned int vTargetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts one YUV 24 bit pixel to a RGB 24 bit pixel.
		 * @param y Y pixel value to convert
		 * @param u U pixel value to convert
		 * @param v V pixel value to convert
		 * @param r Resulting r pixel value
		 * @param g Resulting g pixel value
		 * @param b Resulting b pixel value
		 */
		static inline void convertYUV24ToRGB24Pixel(const uint8_t y, const uint8_t u, const uint8_t v, uint8_t& r, uint8_t& g, uint8_t& b);

	protected:

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		/**
		 * Converts a YUV 24 bit row to a RGB 24 bit row by using NEON instructions.
		 * Beware: This function uses hard-coded conversion parameters which improves execution performance while also increases binary size when used.
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels receiving the converted pixel data, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [1, infinity)
		 * @param parameters Unused parameters, must be nullptr
		 */
		static void convertYUV24ToRGB24RowPrecision6BitNEON(const uint8_t* source, uint8_t* target, const size_t size, const void* parameters);

		/**
		 * Converts 16 YUV24 pixels to 16 RGB24 pixels by using NEON instructions.
		 * Beware: This function uses hardcoded conversion parameters which improves execution performance while also increases binary size when used.
		 * @param source The pointer to the 16 source pixels (with 3 channels = 48 bytes) to convert, must be valid
		 * @param target The pointer to the 16 target pixels (with 3 channels = 48 bytes) receiving the converted pixel data, must be valid
		 */
		static OCEAN_FORCE_INLINE void convert16PixelsYUV24ToRGB24Precision6BitNEON(const uint8_t* const source, uint8_t* const target);

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

};

inline void FrameConverterYUV24::convertYUV24ToBGR24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width * 3u + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 3u + targetPaddingElements;

	// precise color space conversion:
	// | B |   | 1.1639404296875   2.0179443359375  0.0             -276.919921875 |   | Y |
	// | G | = | 1.1639404296875  -0.3909912109375 -0.81298828125    135.486328125 | * | U |
	// | R |   | 1.1639404296875   0.0              1.595947265625  -222.904296875 |   | V |
	//                                                                                 | 1 |

	// approximation:
	// | B |   | 1192    2066    0      -277 |   | Y |
	// | G | = | 1192   -400    -833     135 | * | U |
	// | R |   | 1192    0       1634   -223 |   | V |
	//                                           | 1 |

	const int parameters[12] = {1192, 1192, 1192, 2066, -400, 0, 0, -833, 1634, -277, 135, -223};

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, flag, CV::FrameChannels::convertRow3ChannelsTo3Channels8BitPerChannel10BitPrecision, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>, areContinuous, parameters, worker);
}

inline void FrameConverterYUV24::convertYUV24ToBGRA32Precision6Bit(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width * 3u + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 4u + targetPaddingElements;

	// | B |   | 1.0     1.732446    0.0        -221.753088 |   | Y |
	// | G |   | 1.0    -0.337633   -0.698001    132.561152 |   | U |
	// | R | = | 1.0     0.0         1.370705   -175.45024  | * | V |

	// B = Y + 1.732446 * (U - 128);
	// G = Y - 0.337633 * (U - 128) - 0.698001 * (V - 128);
	// R = Y + 1.370705 * (V - 128);

	// | B |   | 64   111    0  | * | Y -  0  |
	// | G | = | 64   -22   -45 | * | U - 128 |
	// | R |   | 64    0     88 | * | V - 128 |

	const int parameters[13] = {64, 64, 64, 111, -22, 0, 0, -45, 88, 0, 128, 128, int(alphaValue)};

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, flag, CV::FrameChannels::convertRow3ChannelsTo4Channels8BitPerChannel6BitPrecision, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 4u>, areContinuous, parameters, worker);
}

inline void FrameConverterYUV24::convertYUV24ToRGB24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width * 3u + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 3u + targetPaddingElements;

	// precise color space conversion:
	// | R |   | 1.1639404296875   0.0              1.595947265625  -222.904296875 |   | Y |
	// | G | = | 1.1639404296875  -0.3909912109375 -0.81298828125    135.486328125 | * | U |
	// | B |   | 1.1639404296875   2.0179443359375  0.0             -276.919921875 |   | V |
	//                                                                                 | 1 |

	// approximation:
	// | R |   | 1192    0       1634   -223 |   | Y |
	// | G | = | 1192   -400    -833     135 | * | U |
	// | B |   | 1192    2066    0      -277 |   | V |
	//                                           | 1 |

	const int parameters[12] = {1192, 1192, 1192, 0, -400, 2066, 1634, -833, 0, -223, 135, -277};

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, flag, CV::FrameChannels::convertRow3ChannelsTo3Channels8BitPerChannel10BitPrecision, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>, areContinuous, parameters, worker);
}

inline void FrameConverterYUV24::convertYUV24ToRGB24Precision6Bit(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width * 3u + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 3u + targetPaddingElements;

#if defined(OCEAN_USE_HARDCODED_YUV24_TO_RGB24_CONVERTER) && OCEAN_HARDWARE_NEON_VERSION >= 10

	// we keep this function mainly to show the performance difference between the hard-coded implementation and a variable implementation

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, flag, FrameConverterYUV24::convertYUV24ToRGB24RowPrecision6BitNEON, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>, nullptr, worker);

#else

	// R = Y + 1.370705 * (V - 128);
	// G = Y - 0.337633 * (U - 128) - 0.698001 * (V - 128);
	// B = Y + 1.732446 * (U - 128);

	const int parameters[12] = {64, 64, 64, 0, -22, 111, 88, -45, 0, 0, 128, 128};

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, flag, CV::FrameChannels::convertRow3ChannelsTo3Channels8BitPerChannel6BitPrecision, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>, areContinuous, parameters, worker);

#endif
}

inline void FrameConverterYUV24::convertYUV24ToY8(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	// source frame Y U V
	//              0 1 2
	// target frame Y
	// pattern      0
	constexpr unsigned int shufflePattern = 0x0u;

	FrameChannels::shuffleChannels<uint8_t, 3u, 1u, shufflePattern>(source, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterYUV24::convertYUV24ToYUV24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	FrameChannels::transformGeneric<uint8_t, 3u>(source, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterYUV24::convertYUV24ToYVU24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	// source frame Y U V
	//              0 1 2
	// target frame Y V U
	// pattern      0 2 1

	constexpr unsigned int shufflePattern = 0x120u;

	FrameChannels::shuffleChannels<uint8_t, 3u, 3u, shufflePattern>(source, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterYUV24::convertYUV24ToY_U_V12(const uint8_t* source, uint8_t* yTarget, uint8_t* uTarget, uint8_t* vTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int uTargetPaddingElements, const unsigned int vTargetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && yTarget != nullptr && uTarget != nullptr && vTarget != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	const unsigned int options[4] = {sourcePaddingElements, yTargetPaddingElements, uTargetPaddingElements, vTargetPaddingElements};

	void* targets[3] =
	{
		yTarget,
		uTarget,
		vTarget
	};

	FrameConverter::convertArbitraryPixelFormat((const void**)(&source), targets, width, height, flag, 2u, FrameConverter::mapTwoRows_1Plane3Channels_To_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_8BitPerChannel<0u, 1u, 2u>, options, worker);
}

inline void FrameConverterYUV24::convertYUV24ToRGB24Pixel(const uint8_t y, const uint8_t u, const uint8_t v, uint8_t& r, uint8_t& g, uint8_t& b)
{
	const int y_ = (y - 16) * 298 + 128;
	const int u_ = u - 128;
	const int v_ = v - 128;

	r = (uint8_t)(minmax(0, (y_ + 409 * v_) >> 8, 255));
	g = (uint8_t)(minmax(0, (y_ - 100 * u_ - 208 * v_) >> 8, 255));
	b = (uint8_t)(minmax(0, (y_ + 516 * u_) >> 8, 255));
}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

OCEAN_FORCE_INLINE void FrameConverterYUV24::convert16PixelsYUV24ToRGB24Precision6BitNEON(const uint8_t* const source, uint8_t* const target)
{
	ocean_assert(source != nullptr && target != nullptr);

	// the documentation of this function designed for YUV24 to RGB24 conversion

	// precise color space conversion:
	// | R |   |  1    0.0          1.370705   -175.45024  |   | Y |
	// | G | = |  1   -0.3376335   -0.698001    132.561152 | * | U |
	// | B |   |  1    1.732446     0.0        -221.753088 |   | V |
	//                                                         | 1 |

	// approximation:
	// R = 64 * Y +   0 * (U - 128) + 88 * (V - 128)
	// G = 64 * Y -  22 * (U - 128) - 45 * (V - 128)
	// B = 64 * Y + 111 * (U - 128) +  0 * (V - 128)

	const uint8x8_t constant_128_u_8x8 = vdup_n_u8(128);
	const int16x8_t constant_22_s_16x8 = vdupq_n_s16(-22);
	const int16x8_t constant_111_s_16x8 = vdupq_n_s16(111);
	const int16x8_t constant_88_s_16x8 = vdupq_n_s16(88);
	const int16x8_t constant_45_s_16x8 = vdupq_n_s16(-45);

	const uint8x16x3_t source_u_8x16x3 = vld3q_u8(source);

	// Y' = Y * 64, U' = U - 128, V' = V - 128
	const int16x8_t sourceMultiplied0_low_s_16x8 = vreinterpretq_s16_u16(vshll_n_u8(vget_low_u8(source_u_8x16x3.val[0]), 6));
	const int16x8_t source1_low_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(source_u_8x16x3.val[1]), constant_128_u_8x8));
	const int16x8_t source2_low_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(source_u_8x16x3.val[2]), constant_128_u_8x8));

	const int16x8_t sourceMultiplied0_high_s_16x8 = vreinterpretq_s16_u16(vshll_n_u8(vget_high_u8(source_u_8x16x3.val[0]), 6));
	const int16x8_t source1_high_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(source_u_8x16x3.val[1]), constant_128_u_8x8));
	const int16x8_t source2_high_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(source_u_8x16x3.val[2]), constant_128_u_8x8));

	// now we apply the 3x3 matrix multiplication

	int16x8_t intermediateResults1_low_s_16x8 = vmlaq_s16(sourceMultiplied0_low_s_16x8, source1_low_s_16x8, constant_22_s_16x8);
	int16x8_t intermediateResults2_low_s_16x8 = vmlaq_s16(sourceMultiplied0_low_s_16x8, source1_low_s_16x8, constant_111_s_16x8);

	int16x8_t intermediateResults1_high_s_16x8 = vmlaq_s16(sourceMultiplied0_high_s_16x8, source1_high_s_16x8, constant_22_s_16x8);
	int16x8_t intermediateResults2_high_s_16x8 = vmlaq_s16(sourceMultiplied0_high_s_16x8, source1_high_s_16x8, constant_111_s_16x8);

	int16x8_t intermediateResults0_low_s_16x8 = vmlaq_s16(sourceMultiplied0_low_s_16x8, source2_low_s_16x8, constant_88_s_16x8);
	intermediateResults1_low_s_16x8 = vmlaq_s16(intermediateResults1_low_s_16x8, source2_low_s_16x8, constant_45_s_16x8);

	int16x8_t intermediateResults0_high_s_16x8 = vmlaq_s16(sourceMultiplied0_high_s_16x8, source2_high_s_16x8, constant_88_s_16x8);
	intermediateResults1_high_s_16x8 = vmlaq_s16(intermediateResults1_high_s_16x8, source2_high_s_16x8, constant_45_s_16x8);

	uint8x16x3_t results_u_8x16x3;

	// saturated narrow signed to unsigned, normalized by 2^6
	results_u_8x16x3.val[0] = vcombine_u8(vqrshrun_n_s16(intermediateResults0_low_s_16x8, 6), vqrshrun_n_s16(intermediateResults0_high_s_16x8, 6));
	results_u_8x16x3.val[1] = vcombine_u8(vqrshrun_n_s16(intermediateResults1_low_s_16x8, 6), vqrshrun_n_s16(intermediateResults1_high_s_16x8, 6));
	results_u_8x16x3.val[2] = vcombine_u8(vqrshrun_n_s16(intermediateResults2_low_s_16x8, 6), vqrshrun_n_s16(intermediateResults2_high_s_16x8, 6));

	// and we can store the result
	vst3q_u8(target, results_u_8x16x3);
}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

}

}

#endif // META_OCEAN_CV_FRAME_CONVERTER_RGB_24_H
