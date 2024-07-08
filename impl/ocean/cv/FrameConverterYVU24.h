/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_CONVERTER_YVU_24_H
#define META_OCEAN_CV_FRAME_CONVERTER_YVU_24_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class provides functions to convert frames with YVU24 pixel format to other pixel formats.
 * See Frame::PixelFormat for details of the YVU24 pixel format.<br>
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameConverterYVU24 : public FrameConverter
{
	public:

		/**
		 * Converts a YVU 24 bit frame to a BGR 24 bit frame using the conversion.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void convertYVU24ToBGR24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a YVU 24 bit frame to a RGB 24 bit frame using the conversion.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void convertYVU24ToRGB24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a YVU 24 bit frame to a Y 8 bit frame using the conversion.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void convertYVU24ToY8(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a YVU 24 bit frame to a YUV 24 bit frame using the conversion.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void convertYVU24ToYUV24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a YVU 24 bit frame to a YVU 24 bit frame using the conversion.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void convertYVU24ToYVU24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a YVU 24 bit frame to a Y_V_U 12 bit frame using the conversion.
		 * @param source The source frame buffer, with (width * 3 + targetPaddingElements) * height elements, must be valid
		 * @param yTarget The y target frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param vTarget The v target frame buffer, with (2 * width/2 + vPaddingElements) * height/2 elements, must be valid
		 * @param uTarget The u target frame buffer, with (2 * width/2 + uPaddingElements) * height/2 elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in (uint8_t) elements, with range [0, infinity)
		 * @param yTargetPaddingElements The number of padding elements at the end of each y-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param vTargetPaddingElements The number of padding elements at the end of each v-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param uTargetPaddingElements The number of padding elements at the end of each u-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void convertYVU24ToY_V_U12(const uint8_t* source, uint8_t* yTarget, uint8_t* vTarget, uint8_t* uTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int vTargetPaddingElements, const unsigned int uTargetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts one YVU 24 bit pixel to a RGB 24 bit pixel.
		 * @param y Y pixel value to convert
		 * @param v V pixel value to convert
		 * @param u U pixel value to convert
		 * @param r Resulting r pixel value
		 * @param g Resulting g pixel value
		 * @param b Resulting b pixel value
		 */
		static inline void convertYVU24ToRGB24Pixel(const uint8_t y, const uint8_t v, const uint8_t u, uint8_t& r, uint8_t& g, uint8_t& b);
};

inline void FrameConverterYVU24::convertYVU24ToBGR24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width * 3u + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 3u + targetPaddingElements;

	// precise color space conversion:
	// | B |   | 1.1639404296875   0.0              2.0179443359375  -276.919921875 |   | Y |
	// | G | = | 1.1639404296875  -0.81298828125   -0.3909912109375   135.486328125 | * | V |
	// | R |   | 1.1639404296875   1.595947265625   0.0              -222.904296875 |   | U |
	//                                                                                  | 1 |

	// approximation:
	// | B |   | 1192    0       2066   -277 |   | Y |
	// | G | = | 1192   -833    -400     135 | * | U |
	// | R |   | 1192    1634    0      -223 |   | V |
	//                                           | 1 |

	const int parameters[12] = {1192, 1192, 1192, 0, -833, 1634, 2066, -400, 0, -277, 135, -223};

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, flag, CV::FrameChannels::convertRow3ChannelsTo3Channels8BitPerChannel10BitPrecision, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>, areContinuous, parameters, worker);
}

inline void FrameConverterYVU24::convertYVU24ToRGB24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width * 3u + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 3u + targetPaddingElements;

	// precise color space conversion:
	// | R |   | 1.1639404296875   1.595947265625   0.0              -222.904296875 |   | Y |
	// | G | = | 1.1639404296875  -0.81298828125   -0.3909912109375   135.486328125 | * | V |
	// | B |   | 1.1639404296875   0.0              2.0179443359375  -276.919921875 |   | U |
	//                                                                                  | 1 |

	// approximation:
	// | R |   | 1192    1634    0      -223 |   | Y |
	// | G | = | 1192   -833    -400     135 | * | U |
	// | B |   | 1192    0       2066   -277 |   | V |
	//                                           | 1 |

	const int parameters[12] = {1192, 1192, 1192, 1634, -833, 0, 0, -400, 2066, -223, 135, -277};

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, flag, CV::FrameChannels::convertRow3ChannelsTo3Channels8BitPerChannel10BitPrecision, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>, areContinuous, parameters, worker);
}

inline void FrameConverterYVU24::convertYVU24ToY8(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	// source frame Y V U
	//              0 1 2
	// target frame Y
	// pattern      0
	constexpr unsigned int shufflePattern = 0x0u;

	FrameChannels::shuffleChannels<uint8_t, 3u, 1u, shufflePattern>(source, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterYVU24::convertYVU24ToYUV24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	// source frame Y V U
	//              0 1 2
	// target frame Y U V
	// pattern      0 2 1
	constexpr unsigned int shufflePattern = 0x120u;

	FrameChannels::shuffleChannels<uint8_t, 3u, 3u, shufflePattern>(source, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterYVU24::convertYVU24ToYVU24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	FrameChannels::transformGeneric<uint8_t, 3u>(source, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterYVU24::convertYVU24ToY_V_U12(const uint8_t* source, uint8_t* yTarget, uint8_t* vTarget, uint8_t* uTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int vTargetPaddingElements, const unsigned int uTargetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && yTarget != nullptr && vTarget != nullptr && uTarget != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	const unsigned int options[4] = {sourcePaddingElements, yTargetPaddingElements, vTargetPaddingElements, uTargetPaddingElements};

	void* targets[3] =
	{
		yTarget,
		vTarget,
		uTarget
	};

	FrameConverter::convertArbitraryPixelFormat((const void**)(&source), targets, width, height, flag, 2u, FrameConverter::mapTwoRows_1Plane3Channels_To_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_8BitPerChannel<0u, 1u, 2u>, options, worker);
}

inline void FrameConverterYVU24::convertYVU24ToRGB24Pixel(const uint8_t y, const uint8_t v, const uint8_t u, uint8_t& r, uint8_t& g, uint8_t& b)
{
	const int y_ = (y - 16) * 298 + 128;
	const int v_ = v - 128;
	const int u_ = u - 128;

	r = (uint8_t)(minmax(0, (y_ + 409 * v_) >> 8, 255));
	g = (uint8_t)(minmax(0, (y_ - 100 * u_ - 208 * v_) >> 8, 255));
	b = (uint8_t)(minmax(0, (y_ + 516 * u_) >> 8, 255));
}

}

}

#endif // META_OCEAN_CV_FRAME_CONVERTER_YVU_24_H
