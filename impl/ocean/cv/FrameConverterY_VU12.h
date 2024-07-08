/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_CONVERTER_Y_VU_12_H
#define META_OCEAN_CV_FRAME_CONVERTER_Y_VU_12_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameConverterY8.h"

namespace Ocean
{

namespace CV
{

/**
 * This class provides functions to convert frames with Y_VU12 pixel format.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameConverterY_VU12 : public FrameConverter
{
	public:

		/**
		 * Converts a Y_VU12 frame to an 8 bit gray scale frame into a second image buffer.
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param vuSource The vu source frame buffer, with (2 * width/2 + vuPaddingElements) * height/2 elements, can be invalid, as this parameter is not used
		 * @param target The target frame buffer, with (width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vuSourcePaddingElements The number of padding elements at the end of each vu-source row, in (uint8_t) elements, with range [0, infinity), actually this parameter is not used
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_VU12ToY8(const uint8_t* ySource, const uint8_t* vuSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int vuSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_VU12 frame to an 24 bit BGR frame into a second image buffer.
		 * <pre>
		 * YVU input value range:  [16, 235]x[16, 240]x[16, 240]
		 * BGR output value range: [ 0, 255]x[ 0, 255]x[ 0, 255]
		 * </pre>
		 * Note: In case the given source memory contains y and uv data without any padding elements, 'vuSource' is given by 'uvSource = ySource + width * height'.
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param vuSource The vu source frame buffer, with (2 * width/2 + vvuPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (3 * width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vuSourcePaddingElements The number of padding elements at the end of each vu-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_VU12LimitedRangeToBGR24FullRange(const uint8_t* ySource, const uint8_t* vuSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int vuSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a full range Y_VU12 frame to a full range BGRA32 frame into a second image buffer applying a transformation similar to BT.601.
		 * The actual transformation is close to the official BT.601 standard and used on Android for conversion from Y'UV420sp (NV21).<br>
		 * <pre>
		 * YVU input value range:   [0, 255]x[0, 255]x[0, 255]
		 * BGRA output value range: [0, 255]x[0, 255]x[0, 255]x[0, 255]
		 * </pre>
		 * Note: In case the given source memory contains y and uv data without any padding elements, 'vuSource' is given by 'uvSource = ySource + width * height'.
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param vuSource The vu source frame buffer, with (2 * width/2 + vvuPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (3 * width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vuSourcePaddingElements The number of padding elements at the end of each vu-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param alphaValue The value of the alpha channel to be set, with range [0, 255]
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_VU12FullRangeToBGRA32FullRangeAndroid(const uint8_t* ySource, const uint8_t* vuSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int vuSourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue = 0xFF, Worker* worker = nullptr);

		/**
		 * Converts a Y_VU12 frame to an 24 bit RGB frame into a second image buffer.
		 * <pre>
		 * YVU input value range:  [16, 235]x[16, 240]x[16, 240]
		 * RGB output value range: [ 0, 255]x[ 0, 255]x[ 0, 255]
		 * </pre>
		 * Note: In case the given source memory contains y and uv data without any padding elements, 'vuSource' is given by 'uvSource = ySource + width * height'.
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param vuSource The vu source frame buffer, with (2 * width/2 + vvuPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (3 * width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vuSourcePaddingElements The number of padding elements at the end of each vu-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_VU12LimitedRangeToRGB24FullRange(const uint8_t* ySource, const uint8_t* vuSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int vuSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_VU12 full range frame to an 24 bit RGB frame into a second image buffer.
		 * <pre>
		 * YVU input value range:  [0, 255]x[0, 255]x[0, 255]
		 * RGB output value range: [0, 255]x[0, 255]x[0, 255]
		 * </pre>
		 * Note: In case the given source memory contains y and uv data without any padding elements, 'vuSource' is given by 'uvSource = ySource + width * height'.
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param vuSource The vu source frame buffer, with (2 * width/2 + vvuPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (3 * width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vuSourcePaddingElements The number of padding elements at the end of each vu-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_VU12FullRangeToRGB24FullRangePrecision6Bit(const uint8_t* ySource, const uint8_t* vuSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int vuSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_VU12 frame to an 24 bit YUV frame into a second image buffer.
		 * Note: In case the given source memory contains y and uv data without any padding elements, 'vuSource' is given by 'uvSource = ySource + width * height'.
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param vuSource The vu source frame buffer, with (2 * width/2 + vvuPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (3 * width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vuSourcePaddingElements The number of padding elements at the end of each vu-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_VU12ToYUV24(const uint8_t* ySource, const uint8_t* vuSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int vuSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_VU12 frame to an 24 bit YVU frame into a second image buffer.
		 * Note: In case the given source memory contains y and uv data without any padding elements, 'vuSource' is given by 'uvSource = ySource + width * height'.
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param vuSource The vu source frame buffer, with (2 * width/2 + vvuPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (3 * width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vuSourcePaddingElements The number of padding elements at the end of each vu-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_VU12ToYVU24(const uint8_t* ySource, const uint8_t* vuSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int vuSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

	protected:

		/**
		 * Converts one row of an image with Y_UV12 pixel format to one row of an image with BGRA32 pixel format.
		 * This function needs one plane with the first channel and another plane/block of 2x2 sub-sampled pixels containing the second and third channels.<br>
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[0] uint32_t: sourcePlanePaddingElements
		 * options[1] uint32_t: sourceZippedPaddingElements
		 * options[2] uint32_t: targetZippedPaddingElements
		 * options[3] uint32_t: alphaValue
		 * </pre>
		 * @param sources The pointer to the first and second memory block of the source image, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height / multipleRowsPerIteration - 1]
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [multipleRowsPerIteration, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param options The 4 options parameters: 3 padding parameters, 1 alpha value parameter, must be valid
		 */
		static void convertOneRowY_VU12FullRangeToBGRA32FullRangeAndroidPrecision6Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Converts two rows of an image with Y_UV12 pixel format to two rows of an image with BGRA32 pixel format.
		 * This function needs one plane with the first channel and another plane/block of 2x2 sub-sampled pixels containing the second and third channels.<br>
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[0] uint32_t: sourcePlanePaddingElements
		 * options[1] uint32_t: sourceZippedPaddingElements
		 * options[2] uint32_t: targetZippedPaddingElements
		 * options[3] uint32_t: alphaValue
		 * </pre>
		 * @param sources The pointer to the first and second memory block of the source image, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height / multipleRowsPerIteration - 1]
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [multipleRowsPerIteration, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param options The 4 options parameters: 3 padding parameters, 1 alpha value parameter, must be valid
		 */
		static void convertTwoRowsY_VU12FullRangeToBGRA32FullRangeAndroidPrecision6Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);
};

inline void FrameConverterY_VU12::convertY_VU12ToY8(const uint8_t* ySource, const uint8_t* /* vuSource */, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int /* vuSourcePaddingElements */, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	FrameChannels::transformGeneric<uint8_t, 1u>(ySource, target, width, height, flag, ySourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterY_VU12::convertY_VU12LimitedRangeToBGR24FullRange(const uint8_t* ySource, const uint8_t* vuSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int vuSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && vuSource != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	// YVU input value range:  [16, 235]x[16, 240]x[16, 240]
	// BGR output value range: [ 0, 255]x[ 0, 255]x[ 0, 255]

	// precise color space conversion:
	// | B |   | 1.1639404296875   0.0              2.0179443359375  -276.919921875 |   | Y |
	// | G | = | 1.1639404296875  -0.81298828125   -0.3909912109375   135.486328125 | * | V |
	// | R |   | 1.1639404296875   1.595947265625   0.0              -222.904296875 |   | U |
	//                                                                                  | 1 |

#if 1

	// Approximation with 6 bit precision:
	//      | B |     | 75     0    128  |   | Y -  16 |
	// 64 * | G |  =  | 75   -52    -25  | * | V - 128 |
	//      | R |     | 75    102    0   |   | U - 128 |

	const int options[3 + 12] =
	{
		// padding parameters
		int(ySourcePaddingElements), int(vuSourcePaddingElements), int(targetPaddingElements),

		// multiplication parameters
		75, 75, 75, 0, -52, 102, 128, -25, 0,

		// bias/translation parameters
		16, 128, 128
	};

	const void* sources[2] =
	{
		ySource,
		vuSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)&target, width, height, flag, 2u, FrameConverter::convertTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit, options, worker);

#else

	// Approximation with 10 bit precision:
	// | B |   | 1192    0       2066    -277 |   | Y |
	// | G | = | 1192   -833    -400      135 | * | V |
	// | R |   | 1192    1634    0       -223 |   | U |
	//                                            | 1 |

	const int options[3 + 12] =
	{
		// padding parameters
		int(ySourcePaddingElements), int(vuSourcePaddingElements), int(targetPaddingElements),

		// multiplication parameters (with denominator 1024)
		1192, 1192, 1192, 0, -833, 1634, 2066, -400, 0,

		// bias/translation parameters (with denominator 1)
		-277, 135, -223
	};

	const void* sources[2] =
	{
		ySource,
		vuSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)&target, width, height, flag, 1u, FrameConverter::convertOneRow1PlaneAnd2ChannelsZippedDownsampled2x2ToZipped3Channels8BitPerChannelPrecision10Bit, options, worker);

#endif

}

inline void FrameConverterY_VU12::convertY_VU12FullRangeToBGRA32FullRangeAndroid(const uint8_t* ySource, const uint8_t* vuSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int vuSourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue, Worker* worker)
{
	ocean_assert(ySource != nullptr && vuSource != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	const int options[3 + 1] =
	{
		// padding parameters
		int(ySourcePaddingElements), int(vuSourcePaddingElements), int(targetPaddingElements),

		// alpha value
		int(alphaValue)
	};

	const void* sources[2] =
	{
		ySource,
		vuSource
	};

#if 1
	FrameConverter::convertArbitraryPixelFormat(sources, (void**)&target, width, height, flag, 1u, FrameConverterY_VU12::convertOneRowY_VU12FullRangeToBGRA32FullRangeAndroidPrecision6Bit, options, worker);
#else
	FrameConverter::convertArbitraryPixelFormat(sources, (void**)&target, width, height, flag, 2u, FrameConverterY_VU12::convertTwoRowsY_VU12FullRangeToBGRA32FullRangeAndroidPrecision6Bit, options, worker);
#endif

}

inline void FrameConverterY_VU12::convertY_VU12LimitedRangeToRGB24FullRange(const uint8_t* ySource, const uint8_t* vuSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int vuSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && vuSource != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	// YVU input value range:  [16, 235]x[16, 240]x[16, 240]
	// RGB output value range: [ 0, 255]x[ 0, 255]x[ 0, 255]

	// precise color space conversion:
	// | R |   | 1.1639404296875   1.595947265625   0.0              -222.904296875 |   | Y |
	// | G | = | 1.1639404296875  -0.81298828125   -0.3909912109375   135.486328125 | * | V |
	// | B |   | 1.1639404296875   0.0              2.0179443359375  -276.919921875 |   | U |
	//                                                                                  | 1 |

#if 1

	// Approximation with 6 bit precision:
	//      | R |     | 75    102    0   |   | Y -  16 |
	// 64 * | G |  =  | 75   -52    -25  | * | V - 128 |
	//      | B |     | 75     0    128  |   | U - 128 |

	const int options[3 + 12] =
	{
		// padding parameters
		int(ySourcePaddingElements), int(vuSourcePaddingElements), int(targetPaddingElements),

		// multiplication parameters
		75, 75, 75, 102, -52, 0, 0, -25, 128,

		// bias/translation parameters
		16, 128, 128
	};

	const void* sources[2] =
	{
		ySource,
		vuSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)&target, width, height, flag, 2u, FrameConverter::convertTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit, options, worker);

#else

	// Approximation with 10 bit precision:
	// | R |   | 1192    1634    0       -223 |   | Y |
	// | G | = | 1192   -833    -400      135 | * | U |
	// | B |   | 1192    0       2066    -277 |   | V |
	//                                            | 1 |

	const int options[3 + 12] =
	{
		// padding parameters
		int(ySourcePaddingElements), int(vuSourcePaddingElements), int(targetPaddingElements),

		// multiplication parameters (with denominator 1024)
		1192, 1192, 1192, 1634, -833, 0, 0, -400, 2066,

		// bias/translation parameters (with denominator 1)
		-223, 135, -277
	};

	const void* sources[2] =
	{
		ySource,
		vuSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)&target, width, height, flag, 1u, FrameConverter::convertOneRow1PlaneAnd2ChannelsZippedDownsampled2x2ToZipped3Channels8BitPerChannelPrecision10Bit, options, worker);

#endif

}

inline void FrameConverterY_VU12::convertY_VU12FullRangeToRGB24FullRangePrecision6Bit(const uint8_t* ySource, const uint8_t* vuSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int vuSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && vuSource != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	/*
	 * | R |     | 1.0     1.402      0.0        1.402     -179.456   |   | Y |
	 * | G |  =  | 1.0    -0.71414   -0.34414   -0.71414    135.45984 | * | V |
	 * | B |     | 1.0     0.0        1.772      0.0       -226.816   |   | U |
	 *                                                                    | 1 |
	 *
	 * Approximation with 6 bit precision:
	 *       | R |     | 64    90      0 |   |    Y    |
	 *  64 * | G |  =  | 64   -46    -22 | * | V - 128 |
	 *       | B |     | 64     0    113 |   | U - 128 |
	 */

	const int options[3 + 12] =
	{
		// padding parameters
		int(ySourcePaddingElements), int(vuSourcePaddingElements), int(targetPaddingElements),

		// multiplication parameters
		64, 64, 64, 90, -46, 0, 0, -22, 113,

		// bias/translation parameters
		0, 128, 128
	};

	const void* sources[2] =
	{
		ySource,
		vuSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)&target, width, height, flag, 2u, FrameConverter::convertTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit, options, worker);
}

inline void FrameConverterY_VU12::convertY_VU12ToYUV24(const uint8_t* ySource, const uint8_t* vuSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int vuSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && vuSource != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	const unsigned int options[3] = {ySourcePaddingElements, vuSourcePaddingElements, targetPaddingElements};

	const void* sources[2] =
	{
		ySource,
		vuSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)&target, width, height, flag, 2u, FrameConverter::mapTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<0u, 2u, 1u>, options, worker);
}

inline void FrameConverterY_VU12::convertY_VU12ToYVU24(const uint8_t* ySource, const uint8_t* vuSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int vuSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && vuSource != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	const unsigned int options[3] = {ySourcePaddingElements, vuSourcePaddingElements, targetPaddingElements};

	const void* sources[2] =
	{
		ySource,
		vuSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)&target, width, height, flag, 2u, FrameConverter::mapTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<0u, 1u, 2u>, options, worker);
}

}

}

#endif // META_OCEAN_CV_FRAME_CONVERTER_Y_VU_12_H
