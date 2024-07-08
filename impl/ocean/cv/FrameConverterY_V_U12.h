/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_CONVERTER_Y_V_U_12_H
#define META_OCEAN_CV_FRAME_CONVERTER_Y_V_U_12_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameConverterY8.h"
#include "ocean/cv/FrameConverterY_U_V12.h"

namespace Ocean
{

namespace CV
{

/**
 * This class provides functions to convert frames with Y_V_U12 pixel format.
 * The Y_V_U12 format holds the three blocks of color channels.<br>
 * The first block covers the Y channel and holds 8 bit per pixel.<br>
 * The second block covers the V channel and holds 8 bit for four pixels in a 2x2 neighborhood.<br>
 * The third block covers the U channel and also holds 8 bit for four pixels in a 2x2 neighborhood.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameConverterY_V_U12 : public FrameConverter
{
	public:

		/**
		 * Converts a Y_V_U12 frame to an 8 bit gray scale frame into a second image buffer.
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param vSource The v source frame buffer, with (2 * width/2 + vPaddingElements) * height/2 elements, can be invalid, as this parameter is not used
		 * @param uSource The u source frame buffer, with (2 * width/2 + uPaddingElements) * height/2 elements, can be invalid, as this parameter is not used
		 * @param target The target frame buffer, with (width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vSourcePaddingElements The number of padding elements at the end of each v-source row, in (uint8_t) elements, with range [0, infinity), actually this parameter is not used
		 * @param uSourcePaddingElements The number of padding elements at the end of each u-source row, in (uint8_t) elements, with range [0, infinity), actually this parameter is not used
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_V_U12ToY8(const uint8_t* ySource, const uint8_t* vSource, const uint8_t* uSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_V_U12 frame to an 24 bit BGR frame into a second image buffer.
		 * <pre>
		 * YVU input value range:  [16, 235]x[16, 240]x[16, 240]
		 * BGR output value range: [ 0, 255]x[ 0, 255]x[ 0, 255]
		 * </pre>
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param vSource The v source frame buffer, with (2 * width/2 + vPaddingElements) * height/2 elements, must be valid
		 * @param uSource The u source frame buffer, with (2 * width/2 + uPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vSourcePaddingElements The number of padding elements at the end of each v-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uSourcePaddingElements The number of padding elements at the end of each u-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_V_U12LimitedRangeToBGR24FullRange(const uint8_t* ySource, const uint8_t* vSource, const uint8_t* uSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_V_U12 frame to an 24 bit RGB frame into a second image buffer.
		 * <pre>
		 * YVU input value range:  [16, 235]x[16, 240]x[16, 240]
		 * RGB output value range: [ 0, 255]x[ 0, 255]x[ 0, 255]
		 * </pre>
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param vSource The v source frame buffer, with (2 * width/2 + vPaddingElements) * height/2 elements, must be valid
		 * @param uSource The u source frame buffer, with (2 * width/2 + uPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vSourcePaddingElements The number of padding elements at the end of each v-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uSourcePaddingElements The number of padding elements at the end of each u-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_V_U12LimitedRangeToRGB24FullRange(const uint8_t* ySource, const uint8_t* vSource, const uint8_t* uSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_V_U12 frame to a 24 bit BGR frame into a second image buffer using BT.601 (analog).
		 * <pre>
		 * YVU input value range:  [0, 255]x[0, 255]x[0, 255]
		 * BGR output value range: [0, 255]x[0, 255]x[0, 255]
		 * </pre>
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param vSource The v source frame buffer, with (width/2 + vPaddingElements) * height/2 elements, must be valid
		 * @param uSource The u source frame buffer, with (width/2 + uPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vSourcePaddingElements The number of padding elements at the end of each v-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uSourcePaddingElements The number of padding elements at the end of each u-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_V_U12FullRangeToBGR24FullRangePrecision6Bit(const uint8_t* ySource, const uint8_t* vSource, const uint8_t* uSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_V_U12 frame to a 24 bit RGB frame into a second image buffer using BT.601 (analog).
		 * <pre>
		 * YVU input value range:  [0, 255]x[0, 255]x[0, 255]
		 * RGB output value range: [0, 255]x[0, 255]x[0, 255]
		 * </pre>
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param vSource The v source frame buffer, with (width/2 + vPaddingElements) * height/2 elements, must be valid
		 * @param uSource The u source frame buffer, with (width/2 + uPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vSourcePaddingElements The number of padding elements at the end of each v-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uSourcePaddingElements The number of padding elements at the end of each u-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_V_U12FullRangeToRGB24FullRangePrecision6Bit(const uint8_t* ySource, const uint8_t* vSource, const uint8_t* uSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_V_U12 frame to an 24 bit YVU frame into a second image buffer.
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param vSource The v source frame buffer, with (2 * width/2 + vPaddingElements) * height/2 elements, must be valid
		 * @param uSource The u source frame buffer, with (2 * width/2 + uPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vSourcePaddingElements The number of padding elements at the end of each v-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uSourcePaddingElements The number of padding elements at the end of each u-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_V_U12ToYVU24(const uint8_t* ySource, const uint8_t* vSource, const uint8_t* uSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_V_U12 frame to an 24 bit YUV frame into a second image buffer.
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param vSource The v source frame buffer, with (2 * width/2 + vPaddingElements) * height/2 elements, must be valid
		 * @param uSource The u source frame buffer, with (2 * width/2 + uPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vSourcePaddingElements The number of padding elements at the end of each v-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uSourcePaddingElements The number of padding elements at the end of each u-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_V_U12ToYUV24(const uint8_t* ySource, const uint8_t* vSource, const uint8_t* uSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);
};

inline void FrameConverterY_V_U12::convertY_V_U12ToY8(const uint8_t* ySource, const uint8_t* /* vSource */, const uint8_t* /* uSource */, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int /* vSourcePaddingElements */, const unsigned int /* uSourcePaddingElements */, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	FrameChannels::transformGeneric<uint8_t, 1u>(ySource, target, width, height, flag, ySourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterY_V_U12::convertY_V_U12LimitedRangeToBGR24FullRange(const uint8_t* ySource, const uint8_t* vSource, const uint8_t* uSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && vSource != nullptr && uSource != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	// precise color space conversion:
	// | B |   | 1.1639404296875   0.0              2.0179443359375  -276.919921875 |   | Y |
	// | G | = | 1.1639404296875  -0.81298828125   -0.3909912109375   135.486328125 | * | V |
	// | R |   | 1.1639404296875   1.595947265625   0.0              -222.904296875 |   | U |
	//                                                                                  | 1 |

	// approximation:
	// | B |   | 1192    0       2066    -277 |   | Y |
	// | G | = | 1192   -833    -400      135 | * | V |
	// | R |   | 1192    1634    0       -223 |   | U |
	//                                            | 1 |

	const int options[4 + 12] =
	{
		// padding parameters
		int(ySourcePaddingElements), int(vSourcePaddingElements), int(uSourcePaddingElements), int(targetPaddingElements),

		// multiplication parameters (with denominator 1024)
		1192, 1192, 1192, 0, -833, 1634, 2066, -400, 0,

		// bias/translation parameters (with denominator 1)
		-277, 135, -223
	};

	const void* sources[3] =
	{
		ySource,
		vSource,
		uSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)(&target), width, height, flag, 2u, FrameConverter::convertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit, options, worker);
}

inline void FrameConverterY_V_U12::convertY_V_U12LimitedRangeToRGB24FullRange(const uint8_t* ySource, const uint8_t* vSource, const uint8_t* uSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && vSource != nullptr && uSource != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	// precise color space conversion:
	// | R |   | 1.1639404296875   1.595947265625   0.0              -222.904296875 |   | Y |
	// | G | = | 1.1639404296875  -0.81298828125   -0.3909912109375   135.486328125 | * | V |
	// | B |   | 1.1639404296875   0.0              2.0179443359375  -276.919921875 |   | U |
	//                                                                                  | 1 |

	// approximation:
	// | R |   | 1192    1634    0       -223 |   | Y |
	// | G | = | 1192   -833    -400      135 | * | V |
	// | B |   | 1192    0       2066    -277 |   | U |
	//                                            | 1 |

	const int options[4 + 12] =
	{
		// padding parameters
		int(ySourcePaddingElements), int(vSourcePaddingElements), int(uSourcePaddingElements), int(targetPaddingElements),

		// multiplication parameters (with denominator 1024)
		1192, 1192, 1192, 1634, -833, 0, 0, -400, 2066,

		// bias/translation parameters (with denominator 1)
		-223, 135, -277
	};

	const void* sources[3] =
	{
		ySource,
		vSource,
		uSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)(&target), width, height, flag, 2u, FrameConverter::convertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit, options, worker);
}

inline void FrameConverterY_V_U12::convertY_V_U12FullRangeToBGR24FullRangePrecision6Bit(const uint8_t* ySource, const uint8_t* vSource, const uint8_t* uSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	FrameConverterY_U_V12::convertY_U_V12FullRangeToBGR24FullRangePrecision6Bit(ySource, uSource, vSource, target, width, height, flag, ySourcePaddingElements, uSourcePaddingElements, vSourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterY_V_U12::convertY_V_U12FullRangeToRGB24FullRangePrecision6Bit(const uint8_t* ySource, const uint8_t* vSource, const uint8_t* uSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	FrameConverterY_U_V12::convertY_U_V12FullRangeToRGB24FullRangePrecision6Bit(ySource, uSource, vSource, target, width, height, flag, ySourcePaddingElements, uSourcePaddingElements, vSourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterY_V_U12::convertY_V_U12ToYVU24(const uint8_t* ySource, const uint8_t* vSource, const uint8_t* uSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	FrameConverterY_U_V12::convertY_U_V12ToYUV24(ySource, vSource, uSource, target, width, height, flag, ySourcePaddingElements, vSourcePaddingElements, uSourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterY_V_U12::convertY_V_U12ToYUV24(const uint8_t* ySource, const uint8_t* vSource, const uint8_t* uSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && vSource != nullptr && uSource != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	const unsigned int options[4] = {ySourcePaddingElements, vSourcePaddingElements, uSourcePaddingElements, targetPaddingElements};

	const void* sources[3] =
	{
		ySource,
		vSource,
		uSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)(&target), width, height, flag, 2u, FrameConverter::mapTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<0u, 2u, 1u>, options, worker);
}

}

}

#endif // META_OCEAN_CV_FRAME_CONVERTER_Y_V_U_12_H
