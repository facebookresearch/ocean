/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_CONVERTER_Y_UV_12_H
#define META_OCEAN_CV_FRAME_CONVERTER_Y_UV_12_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameConverterY8.h"

namespace Ocean
{

namespace CV
{

/**
 * This class provides functions to convert frames with Y_UV12 pixel format.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameConverterY_UV12 : public FrameConverter
{
	public:

		/**
		 * Converts a Y_UV12 frame to an 8 bit gray scale frame into a second image buffer.
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param uvSource The uv source frame buffer, with (2 * width/2 + uvPaddingElements) * height/2 elements, can be invalid, as this parameter is not used
		 * @param target The target frame buffer, with (width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uvSourcePaddingElements The number of padding elements at the end of each uv-source row, in (uint8_t) elements, with range [0, infinity), actually this parameter is not used
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_UV12ToY8(const uint8_t* ySource, const uint8_t* uvSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uvSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_UV12 frame to a 24 bit BGR frame into a second image buffer.
		 * <pre>
		 * YUV input value range:  [16, 235]x[16, 240]x[16, 240]
		 * BGR output value range: [ 0, 255]x[ 0, 255]x[ 0, 255]
		 * </pre>
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param uvSource The uv source frame buffer, with (2 * width/2 + uvPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (3 * width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uvSourcePaddingElements The number of padding elements at the end of each uv-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_UV12LimitedRangeToBGR24FullRange(const uint8_t* ySource, const uint8_t* uvSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uvSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_UV12 frame to a 24 bit RGB frame into a second image buffer.
		 * <pre>
		 * YUV input value range:  [16, 235]x[16, 240]x[16, 240]
		 * RGB output value range: [ 0, 255]x[ 0, 255]x[ 0, 255]
		 * </pre>
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param uvSource The uv source frame buffer, with (2 * width/2 + uvPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (3 * width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uvSourcePaddingElements The number of padding elements at the end of each uv-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_UV12LimitedRangeToRGB24FullRange(const uint8_t* ySource, const uint8_t* uvSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uvSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a full range Y_UV12 frame to a full range 24 bit RGB frame into a second image buffer.
		 * <pre>
		 * YUV input value range:  [0, 255]x[0, 255]x[0, 255]
		 * RGB output value range: [0, 255]x[0, 255]x[0, 255]
		 * </pre>
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param uvSource The uv source frame buffer, with (2 * width/2 + uvPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (3 * width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uvSourcePaddingElements The number of padding elements at the end of each uv-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_UV12FullRangeToBGR24FullRange(const uint8_t* ySource, const uint8_t* uvSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uvSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a full range Y_UV12 frame to a full range 24 bit RGB frame into a second image buffer.
		 * <pre>
		 * YUV input value range:  [0, 255]x[0, 255]x[0, 255]
		 * RGB output value range: [0, 255]x[0, 255]x[0, 255]
		 * </pre>
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param uvSource The uv source frame buffer, with (2 * width/2 + uvPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (3 * width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uvSourcePaddingElements The number of padding elements at the end of each uv-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_UV12FullRangeToRGB24FullRange(const uint8_t* ySource, const uint8_t* uvSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uvSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_UV12 frame to a 24 bit YUV frame into a second image buffer.
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param uvSource The uv source frame buffer, with (2 * width/2 + uvPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (3 * width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uvSourcePaddingElements The number of padding elements at the end of each uv-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_UV12ToYUV24(const uint8_t* ySource, const uint8_t* uvSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uvSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_UV12 frame to a 24 bit YVU frame into a second image buffer.
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param uvSource The uv source frame buffer, with (2 * width/2 + uvPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (3 * width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uvSourcePaddingElements The number of padding elements at the end of each uv-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_UV12ToYVU24(const uint8_t* ySource, const uint8_t* uvSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uvSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_UV12 frame to a Y_U_V12 frame into a second image buffer.
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param uvSource The uv source frame buffer, with (2 * width/2 + uvPaddingElements) * height/2 elements, must be valid
		 * @param yTarget The target frame buffer, with (width + yTargetPaddingElements) * height elements, must be valid
		 * @param uTarget The target frame buffer, with (width/2 + uTargetPaddingElements) * height/2 elements, must be valid
		 * @param vTarget The target frame buffer, with (width/2 + vTargetPaddingElements) * height/2 elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uvSourcePaddingElements The number of padding elements at the end of each uv-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param yTargetPaddingElements The number of padding elements at the end of each y-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param uTargetPaddingElements The number of padding elements at the end of each u-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param vTargetPaddingElements The number of padding elements at the end of each v-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_UV12ToY_U_V12(const uint8_t* ySource, const uint8_t* uvSource, uint8_t* yTarget, uint8_t* uTarget, uint8_t* vTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uvSourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int uTargetPaddingElements, const unsigned int vTargetPaddingElements, Worker* worker = nullptr);
};

inline void FrameConverterY_UV12::convertY_UV12ToY8(const uint8_t* ySource, const uint8_t* /* uvSource */, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int /* uvSourcePaddingElements */, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	FrameChannels::transformGeneric<uint8_t, 1u>(ySource, target, width, height, flag, ySourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterY_UV12::convertY_UV12LimitedRangeToBGR24FullRange(const uint8_t* ySource, const uint8_t* uvSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uvSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && uvSource != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	// YUV input value range:  [16, 235]x[16, 240]x[16, 240]
	// BGR output value range: [ 0, 255]x[ 0, 255]x[ 0, 255]

	// precise color space conversion:
	// | B |   | 1.1639404296875   2.0179443359375  0.0             -276.919921875 |   | Y |
	// | G | = | 1.1639404296875  -0.3909912109375 -0.81298828125    135.486328125 | * | U |
	// | R |   | 1.1639404296875   0.0              1.595947265625  -222.904296875 |   | V |
	//                                                                                 | 1 |

#if 1

	// Approximation with 6 bit precision:
	//      | B |     | 75   128     0  |   | Y -  16 |
	// 64 * | G |  =  | 75   -25   -52  | * | U - 128 |
	//      | R |     | 75    0     102 |   | V - 128 |

	const int options[3 + 12] =
	{
		// padding parameters
		int(ySourcePaddingElements), int(uvSourcePaddingElements), int(targetPaddingElements),

		// multiplication parameters
		75, 75, 75, 128, -25, 0, 0, -52, 102,

		// bias/translation parameters
		16, 128, 128
	};

	const void* sources[2] =
	{
		ySource,
		uvSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)(&target), width, height, flag, 2u, FrameConverter::convertTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit, options, worker);

#else

	// Approximation with 10 bit precision:
	// | B |   | 1192    2066    0      -277 |   | Y |
	// | G | = | 1192   -400    -833     135 | * | U |
	// | R |   | 1192    0       1634   -223 |   | V |
	//                                           | 1 |

	const int options[3 + 12] =
	{
		// padding parameters
		int(ySourcePaddingElements), int(uvSourcePaddingElements), int(targetPaddingElements),

		// multiplication parameters (with denominator 1024)
		1192, 1192, 1192, 2066, -400, 0, 0, -833, 1634,

		// bias/translation parameters (with denominator 1)
		-277, 135, -223
	};

	const void* sources[2] =
	{
		ySource,
		uvSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)(&target), width, height, flag, 1u, FrameConverter::convertOneRow1PlaneAnd2ChannelsZippedDownsampled2x2ToZipped3Channels8BitPerChannelPrecision10Bit, options, worker);

#endif
}

inline void FrameConverterY_UV12::convertY_UV12LimitedRangeToRGB24FullRange(const uint8_t* ySource, const uint8_t* uvSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uvSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && uvSource != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	// YUV input value range:  [16, 235]x[16, 240]x[16, 240]
	// RGB output value range: [ 0, 255]x[ 0, 255]x[ 0, 255]

	// precise color space conversion:
	// | R |   | 1.1639404296875   0.0              1.595947265625  -222.904296875 |   | Y |
	// | G | = | 1.1639404296875  -0.3909912109375 -0.81298828125    135.486328125 | * | U |
	// | B |   | 1.1639404296875   2.0179443359375  0.0             -276.919921875 |   | V |
	//                                                                                 | 1 |

#if 1

	// Approximation with 6 bit precision:
	//      | R |     | 75    0     102 |   | Y -  16 |
	// 64 * | G |  =  | 75   -25   -52  | * | U - 128 |
	//      | B |     | 75   128     0  |   | V - 128 |

	const int options[3 + 12] =
	{
		// padding parameters
		int(ySourcePaddingElements), int(uvSourcePaddingElements), int(targetPaddingElements),

		// multiplication parameters
		75, 75, 75, 0, -25, 128, 102, -52, 0,

		// bias/translation parameters
		16, 128, 128
	};

	const void* sources[2] =
	{
		ySource,
		uvSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)(&target), width, height, flag, 2u, FrameConverter::convertTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit, options, worker);

#else

	// Approximation with 10 bit precision:
	// | R |   | 1192    0       1634   -223 |   | Y |
	// | G | = | 1192   -400    -833     135 | * | U |
	// | B |   | 1192    2066    0      -277 |   | V |
	//                                           | 1 |

	const int options[3 + 12] =
	{
		// padding parameters
		int(ySourcePaddingElements), int(uvSourcePaddingElements), int(targetPaddingElements),

		// multiplication parameters (with denominator 1024)
		1192, 1192, 1192, 0, -400, 2066, 1634, -833, 0,

		// bias/translation parameters (with denominator 1)
		-223, 135, -277
	};

	const void* sources[2] =
	{
		ySource,
		uvSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)(&target), width, height, flag, 1u, FrameConverter::convertOneRow1PlaneAnd2ChannelsZippedDownsampled2x2ToZipped3Channels8BitPerChannelPrecision10Bit, options, worker);

#endif

}

inline void FrameConverterY_UV12::convertY_UV12FullRangeToBGR24FullRange(const uint8_t* ySource, const uint8_t* uvSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uvSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && uvSource != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	/*
	 * YUV input value range:  [0, 255]x[0, 255]x[0, 255s]
	 * RGB output value range: [0, 255]x[0, 255]x[0, 255]
	 *
	 * | B |     | 1.0     1.772      0.0       -226.816   |   | Y |
	 * | G |  =  | 1.0    -0.34414   -0.71414    135.45984 | * | U |
	 * | R |     | 1.0     0.0        1.402     -179.456   |   | V |
	 *                                                         | 1 |
	 *
	 * Approximation with 6 bit precision:
	 *       | B |     | 64   113     0 |   |    Y    |
	 *  64 * | G |  =  | 64   -22   -46 | * | U - 128 |
	 *       | R |     | 64    0     90 |   | V - 128 |
	 */

	const int options[3 + 12] =
	{
		// padding parameters
		int(ySourcePaddingElements), int(uvSourcePaddingElements), int(targetPaddingElements),

		// multiplication parameters
		64, 64, 64, 113, -22, 0, 0, -46, 90,

		// bias/translation parameters
		0, 128, 128
	};

	const void* sources[2] =
	{
		ySource,
		uvSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)(&target), width, height, flag, 2u, FrameConverter::convertTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit, options, worker);
}

inline void FrameConverterY_UV12::convertY_UV12FullRangeToRGB24FullRange(const uint8_t* ySource, const uint8_t* uvSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uvSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && uvSource != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	/*
	 * YUV input value range:  [0, 255]x[0, 255]x[0, 255s]
	 * RGB output value range: [0, 255]x[0, 255]x[0, 255]
	 *
	 * | R |     | 1.0     0.0        1.402     -179.456   |   | Y |
	 * | G |  =  | 1.0    -0.34414   -0.71414    135.45984 | * | U |
	 * | B |     | 1.0     1.772      0.0       -226.816   |   | V |
	 *                                                         | 1 |
	 *
	 * Approximation with 6 bit precision:
	 *       | R |     | 64    0     90 |   |    Y    |
	 *  64 * | G |  =  | 64   -22   -46 | * | U - 128 |
	 *       | B |     | 64   113     0 |   | V - 128 |
	 */

	const int options[3 + 12] =
	{
		// padding parameters
		int(ySourcePaddingElements), int(uvSourcePaddingElements), int(targetPaddingElements),

		// multiplication parameters
		64, 64, 64, 0, -22, 113, 90, -46, 0,

		// bias/translation parameters
		0, 128, 128
	};

	const void* sources[2] =
	{
		ySource,
		uvSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)(&target), width, height, flag, 2u, FrameConverter::convertTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit, options, worker);
}

inline void FrameConverterY_UV12::convertY_UV12ToYUV24(const uint8_t* ySource, const uint8_t* uvSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uvSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && uvSource != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	const unsigned int options[3] = {ySourcePaddingElements, uvSourcePaddingElements, targetPaddingElements};

	const void* sources[2] =
	{
		ySource,
		uvSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)(&target), width, height, flag, 1u, FrameConverter::mapOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<0u, 1u, 2u>, options, worker);
}

inline void FrameConverterY_UV12::convertY_UV12ToYVU24(const uint8_t* ySource, const uint8_t* uvSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uvSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && uvSource != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	const unsigned int options[3] = {ySourcePaddingElements, uvSourcePaddingElements, targetPaddingElements};

	const void* sources[2] =
	{
		ySource,
		uvSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)(&target), width, height, flag, 1u, FrameConverter::mapOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<0u, 2u, 1u>, options, worker);
}

inline void FrameConverterY_UV12::convertY_UV12ToY_U_V12(const uint8_t* ySource, const uint8_t* uvSource, uint8_t* yTarget, uint8_t* uTarget, uint8_t* vTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uvSourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int uTargetPaddingElements, const unsigned int vTargetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && uvSource != nullptr && yTarget != nullptr && yTarget != nullptr && vTarget != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	// first, we handle the y-plane
	FrameChannels::transformGeneric<uint8_t, 1u>(ySource, yTarget, width, height, flag, ySourcePaddingElements, yTargetPaddingElements, worker);

	// now we handle the uv-plane

	const unsigned int options[3] = {uvSourcePaddingElements, uTargetPaddingElements, vTargetPaddingElements};

	void* targets[2] =
	{
		uTarget,
		vTarget
	};

	FrameConverter::convertArbitraryPixelFormat((const void**)(&uvSource), targets, width / 2u, height / 2u, flag, 1u, FrameConverter::mapOneRow_1Plane2Channels_To_2Planes1Channel_8BitPerChannel, options, worker);
}

}

}

#endif // META_OCEAN_CV_FRAME_CONVERTER_Y_UV_12_H
