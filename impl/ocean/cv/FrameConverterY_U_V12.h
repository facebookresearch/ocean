/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_CONVERTER_Y_U_V_12_H
#define META_OCEAN_CV_FRAME_CONVERTER_Y_U_V_12_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class provides functions to convert frames with Y_U_V12 pixel format.
 * The Y_U_V12 format holds the three blocks of color channels.<br>
 * The first block covers the Y channel and holds 8 bit per pixel.<br>
 * The second block covers the U channel and holds 8 bit for four pixels in a 2x2 neighborhood.<br>
 * The third block covers the V channel and also holds 8 bit for four pixels in a 2x2 neighborhood.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameConverterY_U_V12 : public FrameConverter
{
	public:

		/**
		 * Converts a Y_U_V12 frame to a Y_U_V12 frame into a second image buffer.
		 * @param ySource The y source plane, with (width + yPaddingElements) * height elements, must be valid
		 * @param uSource The u source plane, with (width/2 + uPaddingElements) * height/2 elements, must be valid
		 * @param vSource The v source plane, with (width/2 + vPaddingElements) * height/2 elements, must be valid
		 * @param yTarget The y target plane, with (width + yTargetPaddingElements) * height elements, must be valid
		 * @param uTarget The u target plane, with (width/2 + uTargetPaddingElements) * height/2 elements, must be valid
		 * @param vTarget The v target plane, with (width/2 + vTargetPaddingElements) * height/2 elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uSourcePaddingElements The number of padding elements at the end of each u-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vSourcePaddingElements The number of padding elements at the end of each v-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param yTargetPaddingElements The number of padding elements at the end of each y-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param uTargetPaddingElements The number of padding elements at the end of each uv-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param vTargetPaddingElements The number of padding elements at the end of each uv-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_U_V12ToY_U_V12(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* yTarget, uint8_t* uTarget, uint8_t* vTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int uTargetPaddingElements, const unsigned int vTargetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_U_V12 frame to an 8 bit gray scale frame into a second image buffer.
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param uSource The u source frame buffer, with (width/2 + uPaddingElements) * height/2 elements, can be invalid, as this parameter is not used
		 * @param vSource The v source frame buffer, with (width/2 + vPaddingElements) * height/2 elements, can be invalid, as this parameter is not used
		 * @param target The target frame buffer, with (width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uSourcePaddingElements The number of padding elements at the end of each u-source row, in (uint8_t) elements, with range [0, infinity), actually this parameter is not used
		 * @param vSourcePaddingElements The number of padding elements at the end of each v-source row, in (uint8_t) elements, with range [0, infinity), actually this parameter is not used
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_U_V12ToY8(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_U_V12 frame to a 24 bit BGR frame into a second image buffer using BT.601 (digital).
		 * <pre>
		 * YUV input value range:  [16, 235]x[16, 240]x[16, 240]
		 * BGR output value range: [ 0, 255]x[ 0, 255]x[ 0, 255]
		 * </pre>
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param uSource The u source frame buffer, with (width/2 + uPaddingElements) * height/2 elements, must be valid
		 * @param vSource The v source frame buffer, with (width/2 + vPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uSourcePaddingElements The number of padding elements at the end of each u-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vSourcePaddingElements The number of padding elements at the end of each v-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_U_V12LimitedRangeToBGR24FullRange(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_U_V12 frame to a 24 bit BGR frame into a second image buffer using BT.601 (analog).
		 * <pre>
		 * YUV input value range:  [0, 255]x[0, 255]x[0, 255]
		 * BGR output value range: [0, 255]x[0, 255]x[0, 255]
		 * </pre>
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param uSource The u source frame buffer, with (width/2 + uPaddingElements) * height/2 elements, must be valid
		 * @param vSource The v source frame buffer, with (width/2 + vPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uSourcePaddingElements The number of padding elements at the end of each u-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vSourcePaddingElements The number of padding elements at the end of each v-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_U_V12FullRangeToBGR24FullRangePrecision6Bit(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_U_V12 frame to an 32 bit BGRA frame into a second image buffer (with 6 bit precision).
		 * <pre>
		 * YUV input value range:   [16, 235]x[16, 240]x[16, 240]
		 * BGRA output value range: [ 0, 255]x[ 0, 255]x[ 0, 255]x[ 0, 255]
		 * </pre>
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param uSource The u source frame buffer, with (width/2 + uPaddingElements) * height/2 elements, must be valid
		 * @param vSource The v source frame buffer, with (width/2 + vPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uSourcePaddingElements The number of padding elements at the end of each u-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vSourcePaddingElements The number of padding elements at the end of each v-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param alphaValue The value of the alpha channel to be set, with range [0, 255]
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_U_V12LimitedRangeToBGRA32FullRangePrecision6Bit(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue = 0xFF, Worker* worker = nullptr);

		/**
		 * Converts a Y_U_V12 frame to a 24 bit RGB frame into a second image buffer.
		 * <pre>
		 * YUV input value range:  [16, 235]x[16, 240]x[16, 240]
		 * RGB output value range: [ 0, 255]x[ 0, 255]x[ 0, 255]
		 * </pre>
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param uSource The u source frame buffer, with (width/2 + uPaddingElements) * height/2 elements, must be valid
		 * @param vSource The v source frame buffer, with (width/2 + vPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uSourcePaddingElements The number of padding elements at the end of each u-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vSourcePaddingElements The number of padding elements at the end of each v-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_U_V12LimitedRangeToRGB24FullRange(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_U_V12 frame to a 24 bit RGB frame into a second image buffer (with 6 bit precision).
		 * <pre>
		 * YUV input value range:  [16, 235]x[16, 240]x[16, 240]
		 * RGB output value range: [ 0, 255]x[ 0, 255]x[ 0, 255]
		 * </pre>
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param uSource The u source frame buffer, with (width/2 + uPaddingElements) * height/2 elements, must be valid
		 * @param vSource The v source frame buffer, with (width/2 + vPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uSourcePaddingElements The number of padding elements at the end of each u-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vSourcePaddingElements The number of padding elements at the end of each v-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_U_V12LimitedRangeToRGB24FullRangePrecision6Bit(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_U_V12 frame to a 24 bit RGB frame into a second image buffer.
		 * <pre>
		 * YUV input value range:  [0, 255]x[0, 255]x[0, 255]
		 * RGB output value range: [0, 255]x[0, 255]x[0, 255]
		 * </pre>
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param uSource The u source frame buffer, with (width/2 + uPaddingElements) * height/2 elements, must be valid
		 * @param vSource The v source frame buffer, with (width/2 + vPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uSourcePaddingElements The number of padding elements at the end of each u-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vSourcePaddingElements The number of padding elements at the end of each v-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_U_V12FullRangeToRGB24FullRangePrecision6Bit(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_U_V12 frame to an 32 bit RGBA frame into a second image buffer.
		 * <pre>
		 * YUV input value range:   [16, 235]x[16, 240]x[16, 240]
		 * RGBA output value range: [ 0, 255]x[ 0, 255]x[ 0, 255]x[ 0, 255]
		 * </pre>
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param uSource The u source frame buffer, with (width/2 + uPaddingElements) * height/2 elements, must be valid
		 * @param vSource The v source frame buffer, with (width/2 + vPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uSourcePaddingElements The number of padding elements at the end of each u-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vSourcePaddingElements The number of padding elements at the end of each v-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param alphaValue The value of the alpha channel to be set, with range [0, 255]
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_U_V12LimitedRangeToRGBA32FullRangePrecision6Bit(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue = 0xFF, Worker* worker = nullptr);

		/**
		 * Converts a Y_U_V12 frame to a 24 bit YUV frame into a second image buffer.
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param uSource The u source frame buffer, with (width/2 + uPaddingElements) * height/2 elements, must be valid
		 * @param vSource The v source frame buffer, with (width/2 + vPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uSourcePaddingElements The number of padding elements at the end of each u-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vSourcePaddingElements The number of padding elements at the end of each v-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_U_V12ToYUV24(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_U_V12 frame to a 24 bit YVU frame into a second image buffer.
		 * @param ySource The y source frame buffer, with (width + yPaddingElements) * height elements, must be valid
		 * @param uSource The u source frame buffer, with (width/2 + uPaddingElements) * height/2 elements, must be valid
		 * @param vSource The v source frame buffer, with (width/2 + vPaddingElements) * height/2 elements, must be valid
		 * @param target The target frame buffer, with (width + targetPaddingElements) * height elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uSourcePaddingElements The number of padding elements at the end of each u-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vSourcePaddingElements The number of padding elements at the end of each v-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_U_V12ToYVU24(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y_U_V12 frame to a Y_UV12 frame into a second image buffer.
		 * @param ySource The y source plane, with (width + yPaddingElements) * height elements, must be valid
		 * @param uSource The u source plane, with (width/2 + uPaddingElements) * height/2 elements, must be valid
		 * @param vSource The v source plane, with (width/2 + vPaddingElements) * height/2 elements, must be valid
		 * @param yTarget The y target plane, with (width + yTargetPaddingElements) * height elements, must be valid
		 * @param uvTarget The uv target plane, with (width + uvTargetPaddingElements) * height/2 elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uSourcePaddingElements The number of padding elements at the end of each u-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vSourcePaddingElements The number of padding elements at the end of each v-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param yTargetPaddingElements The number of padding elements at the end of each y-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param uvTargetPaddingElements The number of padding elements at the end of each uv-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param ySourcePixelStride The stride between consecutive pixels in the y source plane, in elements, with range [1, infinity)
		 * @param uSourcePixelStride The stride between consecutive pixels in the u source plane, in elements, with range [1, infinity)
		 * @param vSourcePixelStride The stride between consecutive pixels in the v source plane, in elements, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertY_U_V12ToY_UV12(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* yTarget, uint8_t* uvTarget, const unsigned int width, const unsigned int height, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int uvTargetPaddingElements, const unsigned int ySourcePixelStride = 1u, const unsigned int uSourcePixelStride = 1u, const unsigned int vSourcePixelStride = 1u, Worker* worker = nullptr);

	protected:

		/**
		 * Converts one row of an image with Y_U_V12 pixel format to one row of an image with RGBA32 pixel format.
		 * This function applies hard-coded conversion parameters.<br>
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[0] uint32_t: sourcePlane0PaddingElements
		 * options[1] uint32_t: sourcePlane1PaddingElements
		 * options[2] uint32_t: sourcePlane2PaddingElements
		 * options[3] uint32_t: targetZippedPaddingElements
		 * options[4] uint32_t: channelValue3
		 * </pre>
		 * @param sources The pointer to the first, second, and third memory block of the source image, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height - 1]
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param options The 5 options parameters: 4 padding parameters, one constant channel value, must be valid
		 */
		static void convertOneRowY_U_V12ToRGBA32Precision6Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Converts two rows of an image with Y_U_V12 pixel format to two rows of an image with a Y_UV12 pixel format.
		 * The layout of the options parameters is as follows:
		 * <pre>
		 * options[0] uint32_t: ySourcePaddingElements
		 * options[1] uint32_t: uSourcePaddingElements
		 * options[2] uint32_t: vSourcePaddingElements
		 * options[3] uint32_t: yTargetPaddingElements
		 * options[4] uint32_t: uvTargetPaddingElements
		 * options[5] uint32_t: ySourcePixelStride
		 * options[6] uint32_t: uSourcePixelStride
		 * options[7] uint32_t: vSourcePixelStride
		 * </pre>
		 * @param sources The pointer to the first, second, and third plane of the source image, must be valid
		 * @param targets The one pointer to the first and second target plane, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height / 2 - 1]
		 * @param width The width of the frame in pixel, with range [2, infinity), must be even
		 * @param height The height of the frame in pixel, with range [2, infinity), must be even
		 * @param conversionFlag The conversion to be applied, must be CONVERT_NORMAL
		 * @param options The 8 options parameters: 5 padding parameters, 3 pixel stride parameters, must be valid
		 */
		static void mapY_U_V12ToY_UV12(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);
};

inline void FrameConverterY_U_V12::convertY_U_V12ToY_U_V12(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* yTarget, uint8_t* uTarget, uint8_t* vTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int uTargetPaddingElements, const unsigned int vTargetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && uSource != nullptr && vSource != nullptr);
	ocean_assert(yTarget != nullptr && uTarget != nullptr && vTarget != nullptr);
	ocean_assert(width >= 2u && height >= 2u);
	ocean_assert(width % 2u == 0u && height % 2u == 0u);

	const unsigned int width_2 = width / 2u;
	const unsigned int height_2 = height / 2u;

	FrameChannels::transformGeneric<uint8_t, 1u>(ySource, yTarget, width, height, flag, ySourcePaddingElements, yTargetPaddingElements, worker);
	FrameChannels::transformGeneric<uint8_t, 1u>(uSource, uTarget, width_2, height_2, flag, uSourcePaddingElements, uTargetPaddingElements, worker);
	FrameChannels::transformGeneric<uint8_t, 1u>(vSource, vTarget, width_2, height_2, flag, vSourcePaddingElements, vTargetPaddingElements, worker);
}

inline void FrameConverterY_U_V12::convertY_U_V12ToY8(const uint8_t* ySource, const uint8_t* /* uSource */, const uint8_t* /* vSource */, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int /* uSourcePaddingElements */, const unsigned int /* vSourcePaddingElements */, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	FrameChannels::transformGeneric<uint8_t, 1u>(ySource, target, width, height, flag, ySourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterY_U_V12::convertY_U_V12LimitedRangeToBGR24FullRange(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && uSource != nullptr && vSource != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

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

	const int options[4 + 12] =
	{
		// padding parameters
		int(ySourcePaddingElements), int(uSourcePaddingElements), int(vSourcePaddingElements), int(targetPaddingElements),

		// multiplication parameters (with denominator 1024)
		1192, 1192, 1192, 2066, -400, 0, 0, -833, 1634,

		// bias/translation parameters (with denominator 1)
		-277, 135, -223
	};

	const void* sources[3] =
	{
		ySource,
		uSource,
		vSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)(&target), width, height, flag, 2u, FrameConverter::convertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit, options, worker);
}

inline void FrameConverterY_U_V12::convertY_U_V12FullRangeToBGR24FullRangePrecision6Bit(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && uSource != nullptr && vSource != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	/*
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

	const int options[4 + 12] =
	{
		// padding parameters
		int(ySourcePaddingElements), int(uSourcePaddingElements), int(vSourcePaddingElements), int(targetPaddingElements),

		// multiplication parameters (with denominator 1024)
		64, 64, 64, 113, -22, 0, 0, -46, 90,

		// bias/translation parameters (with denominator 1)
		0, 128, 128
	};

	const void* sources[3] =
	{
		ySource,
		uSource,
		vSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)(&target), width, height, flag, 2u, FrameConverter::convertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit, options, worker);
}

inline void FrameConverterY_U_V12::convertY_U_V12LimitedRangeToBGRA32FullRangePrecision6Bit(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue, Worker* worker)
{
	ocean_assert(ySource != nullptr && uSource != nullptr && vSource != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

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

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)(&target), width, height, flag, 1u, FrameConverter::convertOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane4Channels_8BitPerChannel_Precision6Bit, options, worker);
}

inline void FrameConverterY_U_V12::convertY_U_V12LimitedRangeToRGB24FullRange(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && uSource != nullptr && vSource != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

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

	const int options[4 + 12] =
	{
		// padding parameters
		int(ySourcePaddingElements), int(uSourcePaddingElements), int(vSourcePaddingElements), int(targetPaddingElements),

		// multiplication parameters (with denominator 1024)
		1192, 1192, 1192, 0, -400, 2066, 1634, -833, 0,

		// bias/translation parameters (with denominator 1)
		-223, 135, -277
	};

	const void* sources[3] =
	{
		ySource,
		uSource,
		vSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)(&target), width, height, flag, 2u, FrameConverter::convertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit, options, worker);
}

inline void FrameConverterY_U_V12::convertY_U_V12LimitedRangeToRGB24FullRangePrecision6Bit(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && uSource != nullptr && vSource != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	/*
	 * | R |     | 1.1639404296875   0.0              1.595947265625  -222.904296875 |   | Y |
	 * | G |  =  | 1.1639404296875  -0.3909912109375 -0.81298828125    135.486328125 | * | U |
	 * | B |     | 1.1639404296875   2.0179443359375  0.0             -276.919921875 |   | V |
	 *                                                                                   | 1 |
	 *
	 * Approximation with 6 bit precision:
	 *      | R |     | 75    0    102 |   | Y -  16 |
	 * 64 * | G |  =  | 75   -25   -52 | * | U - 128 |
	 *      | B |     | 75   128    0  |   | V - 128 |
	 */

	const int options[4 + 12] =
	{
		// padding parameters
		int(ySourcePaddingElements), int(uSourcePaddingElements), int(vSourcePaddingElements), int(targetPaddingElements),

		// multiplication parameters (with denominator 1024)
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

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)(&target), width, height, flag, 2u, FrameConverter::convertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit, options, worker);
}

inline void FrameConverterY_U_V12::convertY_U_V12FullRangeToRGB24FullRangePrecision6Bit(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && uSource != nullptr && vSource != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	/*
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

	const int options[4 + 12] =
	{
		// padding parameters
		int(ySourcePaddingElements), int(uSourcePaddingElements), int(vSourcePaddingElements), int(targetPaddingElements),

		// multiplication parameters (with denominator 1024)
		64, 64, 64, 0, -22, 113, 90, -46, 0,

		// bias/translation parameters (with denominator 1)
		0, 128, 128
	};

	const void* sources[3] =
	{
		ySource,
		uSource,
		vSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)(&target), width, height, flag, 2u, FrameConverter::convertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit, options, worker);
}

inline void FrameConverterY_U_V12::convertY_U_V12LimitedRangeToRGBA32FullRangePrecision6Bit(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue, Worker* worker)
{
	ocean_assert(ySource != nullptr && uSource != nullptr && vSource != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

#if 1

	// Approximation with 6 bit precision:
	//      | R |     | 75    0     102 |   | Y -  16 |
	// 64 * | G |  =  | 75   -25   -52  | * | U - 128 |
	//      | B |     | 75   128     0  |   | V - 128 |

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

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)(&target), width, height, flag, 1u, FrameConverter::convertOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane4Channels_8BitPerChannel_Precision6Bit, options, worker);

#else

	const int options[4 + 1] =
	{
		// padding parameters
		int(ySourcePaddingElements), int(uSourcePaddingElements), int(vSourcePaddingElements), int(targetPaddingElements),

		// alpha value
		int(alphaValue)
	};

	const void* sources[3] =
	{
		ySource,
		uSource,
		vSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)(&target), width, height, flag, 1u, convertOneRowY_U_V12ToRGBA32Precision6Bit, options, worker);

#endif
}

inline void FrameConverterY_U_V12::convertY_U_V12ToYUV24(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && uSource != nullptr && vSource != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	const unsigned int options[4] = {ySourcePaddingElements, uSourcePaddingElements, vSourcePaddingElements, targetPaddingElements};

	const void* sources[3] =
	{
		ySource,
		uSource,
		vSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)(&target), width, height, flag, 2u, FrameConverter::mapTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<0u, 1u, 2u>, options, worker);
}

inline void FrameConverterY_U_V12::convertY_U_V12ToYVU24(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(ySource != nullptr && uSource != nullptr && vSource != nullptr && target != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	const unsigned int options[4] = {ySourcePaddingElements, uSourcePaddingElements, vSourcePaddingElements, targetPaddingElements};

	const void* sources[3] =
	{
		ySource,
		uSource,
		vSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)(&target), width, height, flag, 2u, FrameConverter::mapTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<0u, 2u, 1u>, options, worker);
}

inline void FrameConverterY_U_V12::convertY_U_V12ToY_UV12(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, uint8_t* yTarget, uint8_t* uvTarget, const unsigned int width, const unsigned int height, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int uvTargetPaddingElements, const unsigned int ySourcePixelStride, const unsigned int uSourcePixelStride, const unsigned int vSourcePixelStride, Worker* worker)
{
	ocean_assert(ySource != nullptr && uSource != nullptr && vSource != nullptr);
	ocean_assert(yTarget != nullptr && uvTarget != nullptr);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		ocean_assert(false && "Invalid parameters!");
		return;
	}

	const uint32_t options[8] = {ySourcePaddingElements, uSourcePaddingElements, vSourcePaddingElements, yTargetPaddingElements, uvTargetPaddingElements, ySourcePixelStride, uSourcePixelStride, vSourcePixelStride};

	const void* sources[3] =
	{
		ySource,
		uSource,
		vSource
	};

	void* targets[2] =
	{
		yTarget,
		uvTarget
	};

	FrameConverter::convertArbitraryPixelFormat(sources, targets, width, height, CONVERT_NORMAL, 2u, mapY_U_V12ToY_UV12, options, worker);
}

}

}

#endif // META_OCEAN_CV_FRAME_CONVERTER_Y_U_V_12_H
