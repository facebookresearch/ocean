/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_CONVERTER_Y_U_V_24_H
#define META_OCEAN_CV_FRAME_CONVERTER_Y_U_V_24_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class provides functions to convert frames with Y_U_V24 pixel format.
 * The Y_U_V24 format holds the three planes/blocks of color channels.<br>
 * The first block covers the Y channel and holds 8 bit per pixel.<br>
 * The second block covers the U channel and holds 8 bit per pixel.<br>
 * The third block covers the V channel and also holds 8 bit per pixel.
 * The layout of a Y_U_V24 image looks like this:
 * <pre>
 *  y-plane:        u-plane:        v-plane:
 *  ---------       ---------       ---------
 * | Y Y Y Y |     | U U U U |     | V V V V |
 * | Y Y Y Y |     | U U U U |     | V V V V |
 * | Y Y Y Y |     | U U U U |     | V V V V |
 * | Y Y Y Y |     | U U U U |     | V V V V |
 *  ---------       ---------       ---------
 * </pre>
 * @ingroup cv
 */
class FrameConverterY_U_V24 : public FrameConverter
{
	public:

		/**
		 * Converts a Y_U_V24 frame to a 24 bit YUV frame into a second image buffer.
		 * @param ySource The y source frame buffer, must be valid
		 * @param uSource The u source frame buffer, must be valid
		 * @param vSource The v source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uSourcePaddingElements The number of padding elements at the end of each u-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vSourcePaddingElements The number of padding elements at the end of each v-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_U_V24ToYUV24(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_U_V24 frame to a 24 bit BGR frame into a second image buffer.
		 * <pre>
		 * YUV input value range:  [16, 235]x[16, 240]x[16, 240]
		 * BGR output value range: [ 0, 255]x[ 0, 255]x[ 0, 255]
		 * </pre>
		 * @param ySource The y source frame buffer, must be valid
		 * @param uSource The u source frame buffer, must be valid
		 * @param vSource The v source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uSourcePaddingElements The number of padding elements at the end of each u-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vSourcePaddingElements The number of padding elements at the end of each v-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_U_V24LimitedRangeToBGR24(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_U_V24 frame to a 24 bit RGB frame into a second image buffer.
		 * <pre>
		 * YUV input value range:  [16, 235]x[16, 240]x[16, 240]
		 * RGB output value range: [ 0, 255]x[ 0, 255]x[ 0, 255]
		 * </pre>
		 * @param ySource The y source frame buffer, must be valid
		 * @param uSource The u source frame buffer, must be valid
		 * @param vSource The v source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uSourcePaddingElements The number of padding elements at the end of each u-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vSourcePaddingElements The number of padding elements at the end of each v-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_U_V24LimitedRangeToRGB24(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_U_V24 frame to a 32 bit BGRA frame into a second image buffer.
		 * <pre>
		 * YUV input value range:   [16, 235]x[16, 240]x[16, 240]
		 * BGRA output value range: [ 0, 255]x[ 0, 255]x[ 0, 255]x[ 0, 255]
		 * </pre>
		 * @param ySource The y source frame buffer, must be valid
		 * @param uSource The u source frame buffer, must be valid
		 * @param vSource The v source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uSourcePaddingElements The number of padding elements at the end of each u-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vSourcePaddingElements The number of padding elements at the end of each v-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param alphaValue The value of the alpha channel to be set, with range [0, 255]
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_U_V24LimitedRangeToBGRA32(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue = 0xFF, Worker* worker = nullptr);

		/**
		 * Converts a Y_U_V24 frame to a 32 bit RGBA frame into a second image buffer.
		 * <pre>
		 * YUV input value range:   [16, 235]x[16, 240]x[16, 240]
		 * RGBA output value range: [ 0, 255]x[ 0, 255]x[ 0, 255]x[ 0, 255]
		 * </pre>
		 * @param ySource The y source frame buffer, must be valid
		 * @param uSource The u source frame buffer, must be valid
		 * @param vSource The v source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uSourcePaddingElements The number of padding elements at the end of each u-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vSourcePaddingElements The number of padding elements at the end of each v-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param alphaValue The value of the alpha channel to be set, with range [0, 255]
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_U_V24LimitedRangeToRGBA32(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue = 0xFF, Worker* worker = nullptr);

		/**
		 * Converts a full range Y_U_V24 frame to a full range BGRA32 frame into a second image buffer applying a transformation similar to BT.601.
		 * The actual transformation is close to the official BT.601 standard and used on Android for conversion from Y'UV420sp (NV21).
		 * <pre>
		 * YUV input value range:   [0, 255]x[0, 255]x[0, 255]
		 * BGRA output value range: [0, 255]x[0, 255]x[0, 255]x[0, 255]
		 * </pre>
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param uSource The u source frame buffer, with (width + uPaddingElements) * height elements, must be valid
		 * @param vSource The v source frame buffer, with (width + vPaddingElements) * height elements, must be valid
		 * @param target The target frame buffer, with (width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uSourcePaddingElements The number of padding elements at the end of each u-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vSourcePaddingElements The number of padding elements at the end of each v-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param alphaValue The alpha value to be set, with range [0, 255]
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_U_V24FullRangeToBGRA32FullRangeAndroid(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue = 0xFF, Worker* worker = nullptr);
};

inline void FrameConverterY_U_V24::convertY_U_V24ToYUV24(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && uSource != nullptr && vSource != nullptr && target != nullptr);

	const int options[4] =
	{
		// padding parameters
		int(ySourcePaddingElements), int(uSourcePaddingElements), int(vSourcePaddingElements), int(targetPaddingElements)
	};

	const void* sources[3] =
	{
		ySource,
		uSource,
		vSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)(&target), width, height, flag, 1u, FrameConverter::mapOneRow_3Plane1Channel_To_1Plane3Channels_8BitPerChannel<0u, 1u, 2u>, options, worker);
}

inline void FrameConverterY_U_V24::convertY_U_V24LimitedRangeToBGR24(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && uSource != nullptr && vSource != nullptr && target != nullptr);

	// Approximation with 6 bit precision:
	//      | B |     | 75   128    0  |   | Y -  16 |
	// 64 * | G |  =  | 75   -25   -52 | * | U - 128 |
	//      | R |     | 75    0    102 |   | V - 128 |

	const int options[4 + 12] =
	{
		// padding parameters
		int(ySourcePaddingElements), int(uSourcePaddingElements), int(vSourcePaddingElements), int(targetPaddingElements),

		// multiplication parameters (with denominator 64)
		75, 75, 75, 128, -25, 0, 0, -52, 102,

		// bias/translation parameters (with denominator 1)
		16, 128, 128
	};

	const void* sources[3] =
	{
		ySource,
		uSource,
		vSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)&target, width, height, flag, 1u, FrameConverter::convertOneRow_3Planes1Channel_To_1Plane3Channels_8BitPerChannel_Precision6Bit, options, worker);
}

inline void FrameConverterY_U_V24::convertY_U_V24LimitedRangeToRGB24(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && uSource != nullptr && vSource != nullptr && target != nullptr);

	// Approximation with 6 bit precision:
	//      | R |     | 75    0    102 |   | Y -  16 |
	// 64 * | G |  =  | 75   -25   -52 | * | U - 128 |
	//      | B |     | 75   128    0  |   | V - 128 |

	const int options[4 + 12] =
	{
		// padding parameters
		int(ySourcePaddingElements), int(uSourcePaddingElements), int(vSourcePaddingElements), int(targetPaddingElements),

		// multiplication parameters (with denominator 64)
		75, 75, 75, 0, -25, 128, 102, -52, 0,

		// bias/translation parameters (with denominator 1)
		16, 128, 128
	};

	const void* sources[3] =
	{
		ySource,
		uSource,
		vSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)&target, width, height, flag, 1u, FrameConverter::convertOneRow_3Planes1Channel_To_1Plane3Channels_8BitPerChannel_Precision6Bit, options, worker);
}

inline void FrameConverterY_U_V24::convertY_U_V24LimitedRangeToBGRA32(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue, Worker* worker)
{
	ocean_assert(ySource != nullptr && uSource != nullptr && vSource != nullptr && target != nullptr);

	// Approximation with 6 bit precision:
	//      | B |     | 75   128    0  |   | Y -  16 |
	// 64 * | G |  =  | 75   -25   -52 | * | U - 128 |
	//      | R |     | 75    0    102 |   | V - 128 |

	const int options[4 + 12 + 1] =
	{
		// padding parameters
		int(ySourcePaddingElements), int(uSourcePaddingElements), int(vSourcePaddingElements), int(targetPaddingElements),

		// multiplication parameters (with denominator 64)
		75, 75, 75, 128, -25, 0, 0, -52, 102,

		// bias/translation parameters (with denominator 1)
		16, 128, 128,

		// alpha value
		int(alphaValue)
	};

	const void* sources[3] =
	{
		ySource,
		uSource,
		vSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)&target, width, height, flag, 1u, FrameConverter::convertOneRow_3Planes1Channel_To_1Plane4Channels_8BitPerChannel_Precision6Bit, options, worker);
}

inline void FrameConverterY_U_V24::convertY_U_V24LimitedRangeToRGBA32(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue, Worker* worker)
{
	ocean_assert(ySource != nullptr && uSource != nullptr && vSource != nullptr && target != nullptr);

	// Approximation with 6 bit precision:
	//      | R |     | 75    0    102 |   | Y -  16 |
	// 64 * | G |  =  | 75   -25   -52 | * | U - 128 |
	//      | B |     | 75   128    0  |   | V - 128 |

	const int options[4 + 12 + 1] =
	{
		// padding parameters
		int(ySourcePaddingElements), int(uSourcePaddingElements), int(vSourcePaddingElements), int(targetPaddingElements),

		// multiplication parameters (with denominator 64)
		75, 75, 75, 0, -25, 128, 102, -52, 0,

		// bias/translation parameters (with denominator 1)
		16, 128, 128,

		// alpha value
		int(alphaValue)
	};

	const void* sources[3] =
	{
		ySource,
		uSource,
		vSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)&target, width, height, flag, 1u, FrameConverter::convertOneRow_3Planes1Channel_To_1Plane4Channels_8BitPerChannel_Precision6Bit, options, worker);
}

inline void FrameConverterY_U_V24::convertY_U_V24FullRangeToBGRA32FullRangeAndroid(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue, Worker* worker)
{
	ocean_assert(ySource != nullptr && uSource != nullptr && vSource != nullptr && target != nullptr);

	ocean_assert(width != 0u && height != 0u);

	// | B |   | 1.0     1.732446    0.0        -221.753088 |   | Y |
	// | G |   | 1.0    -0.337633   -0.698001    132.561152 |   | U |
	// | R | = | 1.0     0.0         1.370705   -175.45024  | * | V |
	//                                                      |   | 1 |

	// B = Y + 1.732446 * (U - 128);
	// G = Y - 0.337633 * (U - 128) - 0.698001 * (V - 128);
	// R = Y + 1.370705 * (V - 128);

	// | B |   | 64   111    0  | * | Y -  0  |
	// | G | = | 64   -22   -45 | * | U - 128 |
	// | R |   | 64    0     88 | * | V - 128 |

	const int options[4 + 12 + 1] =
	{
		// padding parameters
		int(ySourcePaddingElements), int(uSourcePaddingElements), int(vSourcePaddingElements), int(targetPaddingElements),

		// multiplication parameters (with denominator 64)
		64, 64, 64, 111, -22, 0, 0, -45, 88,

		// bias/translation parameters (with denominator 1)
		0, 128, 128,

		// alpha channel
		int(alphaValue)
	};

	const void* sources[3] =
	{
		ySource,
		uSource,
		vSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)&target, width, height, flag, 1u, FrameConverter::convertOneRow_3Planes1Channel_To_1Plane4Channels_8BitPerChannel_Precision6Bit, options, worker);
}

}

}

#endif // META_OCEAN_CV_FRAME_CONVERTER_Y_U_V_24_H
