/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_CONVERTER_RGB_24_H
#define META_OCEAN_CV_FRAME_CONVERTER_RGB_24_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameChannels.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class provides functions to convert or to change frames with RGB pixel format.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameConverterRGB24 : public FrameConverter
{
	public:

	/**
		* Converts a RGB 24 bit frame to a ARGB 32 bit frame.
		* @param source The source frame buffer, must be valid
		* @param target The target frame buffer, must be valid
		* @param width The width of the frame in pixel, with range (0, infinity)
		* @param height The height of the frame in pixel, with range (0, infinity)
		* @param flag Determining the type of conversion
		* @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		* @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		* @param alphaValue The value of the alpha channel to be set, with range [0, 255]
		* @param worker Optional worker object to distribute the computational load
		*/
	   static inline void convertRGB24ToARGB32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue = 0xFF, Worker* worker = nullptr);

		/**
		 * Converts a RGB 24 bit frame to a BGR 24 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range (0, infinity)
		 * @param height The height of the frame in pixel, with range (0, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertRGB24ToBGR24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a RGB 24 bit frame to a BGR 32 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range (0, infinity)
		 * @param height The height of the frame in pixel, with range (0, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @remarks The unused last channel of each pixel in target frame is set to zero.
		 */
		static inline void convertRGB24ToBGR32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker);
		/**
		 * Converts a RGB 24 bit frame to a BGRA 32 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range (0, infinity)
		 * @param height The height of the frame in pixel, with range (0, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param alphaValue The value of the alpha channel to be set, with range [0, 255]
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertRGB24ToBGRA32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue = 0xFF, Worker* worker = nullptr);

		/**
		 * Changes a RGB 24 bit frame to a BGR 24 bit frame in place.
		 * @param frame The frame buffer to be changed, must be valid
		 * @param width The width of the frame in pixel, with range (0, infinity)
		 * @param height The height of the frame in pixel, with range (0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void changeRGB24ToBGR24(uint8_t* frame, const unsigned int width, const unsigned int height, Worker* worker = nullptr);

		/**
		 * Converts a RGB 24 bit frame to a RGB 24 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range (0, infinity)
		 * @param height The height of the frame in pixel, with range (0, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertRGB24ToRGB24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a RGB 24 bit frame to a RGB 32 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range (0, infinity)
		 * @param height The height of the frame in pixel, with range (0, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @remarks The unused last channel of each pixel in target frame is set to zero.
		 */
		static inline void convertRGB24ToRGB32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a RGB 24 bit frame to a RGBA 32 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range (0, infinity)
		 * @param height The height of the frame in pixel, with range (0, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param alphaValue The value of the alpha channel to be set, with range [0, 255]
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertRGB24ToRGBA32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue = 0xFF, Worker* worker = nullptr);

		/**
		 * Converts a RGB frame to a gray scale frame.
		 * Gray = Red * 0.299 + Green * 0.587 + Blue * 0.114
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range (0, infinity)
		 * @param height The height of the frame in pixel, with range (0, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertRGB24ToY8(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a RGB 24 bit frame to a YUV 24 bit frame by the exact conversion.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range (0, infinity)
		 * @param height The height of the frame in pixel, with range (0, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertRGB24ToYUV24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a full range RGB24 frame to a limited range Y_UV12 frame with 7-bit precision using BT.601.
		 * <pre>
		 * RGB input value range:  [ 0, 255]x[ 0, 255]x[ 0, 255]
		 * YUV input value range:  [16, 235]x[16, 240]x[16, 240]
		 * </pre>
		 * @param source The source frame plane, must be valid
		 * @param yTarget The y-target frame plane, must be valid
		 * @param uvTarget The uv-target frame plane, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param yTargetPaddingElements Optional number of padding elements at the end of each y-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param uvTargetPaddingElements Optional number of padding elements at the end of each uv-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertRGB24FullRangeToY_UV12LimitedRange(const uint8_t* source, uint8_t* yTarget, uint8_t* uvTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int uvTargetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a full range RGB24 frame to a limited range Y_VU12 frame with 7-bit precision using BT.601.
		 * <pre>
		 * RGB input value range:  [ 0, 255]x[ 0, 255]x[ 0, 255]
		 * YVU input value range:  [16, 235]x[16, 240]x[16, 240]
		 * </pre>
		 * @param source The source frame plane, must be valid
		 * @param yTarget The y-target frame plane, must be valid
		 * @param vuTarget The vu-target frame plane, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param yTargetPaddingElements Optional number of padding elements at the end of each y-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param vuTargetPaddingElements Optional number of padding elements at the end of each vu-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertRGB24FullRangeToY_VU12LimitedRange(const uint8_t* source, uint8_t* yTarget, uint8_t* vuTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int vuTargetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a full range RGB24 frame to a full range Y_UV12 frame with 7-bit precision using BT.601.
		 * <pre>
		 * RGB input value range:  [0, 255]x[0, 255]x[0, 255]
		 * YUV output value range: [0, 255]x[0, 255]x[0, 255]
		 * </pre>
		 * @param source The source frame plane, must be valid
		 * @param yTarget The y-target frame plane, must be valid
		 * @param uvTarget The uv-target frame plane, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param yTargetPaddingElements Optional number of padding elements at the end of each y-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param uvTargetPaddingElements Optional number of padding elements at the end of each uv-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertRGB24FullRangeToY_UV12FullRange(const uint8_t* source, uint8_t* yTarget, uint8_t* uvTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int uvTargetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a full range RGB24 frame to a full range Y_VU12 frame with 7-bit precision using BT.601.
		 * <pre>
		 * RGB input value range:  [0, 255]x[0, 255]x[0, 255]
		 * YVU output value range: [0, 255]x[0, 255]x[0, 255]
		 * </pre>
		 * @param source The source frame plane, must be valid
		 * @param yTarget The y-target frame plane, must be valid
		 * @param vuTarget The vu-target frame plane, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param height The height of the frame in pixel, with range [2, infinity), must be a multiple of 2
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param yTargetPaddingElements Optional number of padding elements at the end of each y-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param vuTargetPaddingElements Optional number of padding elements at the end of each vu-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertRGB24FullRangeToY_VU12FullRange(const uint8_t* source, uint8_t* yTarget, uint8_t* vuTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int vuTargetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a full range RGB24 frame to a limited range Y_U_V12 frame with 7-bit precision using BT.601.
		 * <pre>
		 * RGB input value range:  [ 0, 255]x[ 0, 255]x[ 0, 255]
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
		static inline void convertRGB24FullRangeToY_U_V12LimitedRange(const uint8_t* source, uint8_t* yTarget, uint8_t* uTarget, uint8_t* vTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int uTargetPaddingElements, const unsigned int vTargetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a full range RGB24 frame to a full range Y_V_U12 frame with 7-bit precision using BT.601.
		 * <pre>
		 * RGB input value range:  [ 0, 255]x[ 0, 255]x[0 , 255]
		 * YVU input value range:  [16, 235]x[16, 240]x[16, 240]
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
		static inline void convertRGB24FullRangeToY_V_U12LimitedRange(const uint8_t* source, uint8_t* yTarget, uint8_t* vTarget, uint8_t* uTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int vTargetPaddingElements, const unsigned int uTargetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a full range RGB24 frame to a full range Y_U_V12 frame with 7-bit precision using BT.601.
		 * <pre>
		 * RGB input value range:  [0, 255]x[0, 255]x[0, 255]
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
		static inline void convertRGB24FullRangeToY_U_V12FullRange(const uint8_t* source, uint8_t* yTarget, uint8_t* uTarget, uint8_t* vTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int uTargetPaddingElements, const unsigned int vTargetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a full range RGB24 frame to a full range Y_V_U12 frame with 7-bit precision using BT.601.
		 * <pre>
		 * RGB input value range:  [0, 255]x[0, 255]x[0, 255]
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
		static inline void convertRGB24FullRangeToY_V_U12FullRange(const uint8_t* source, uint8_t* yTarget, uint8_t* vTarget, uint8_t* uTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int vTargetPaddingElements, const unsigned int uTargetPaddingElements, Worker* worker = nullptr);

		/**
		 * Changes a RGB 24 bit frame to a YUV 24 bit frame by the exact conversion in place.
		 * @param frame The frame buffer to be changed, must be valid
		 * @param width The width of the frame in pixel, with range (0, infinity)
		 * @param height The height of the frame in pixel, with range (0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void changeRGB24ToYUV24(uint8_t* frame, const unsigned int width, const unsigned int height, Worker* worker = nullptr);

		/**
		 * Converts one RGB 24 bit pixel to one YUV 24 bit pixel.
		 * As this implementation uses a right shift the result may differ from the correct value by 1.
		 * @param rgb The source pixel value to convert, must be valid and must provided the three channel values
		 * @param yuv The target pixel value, must be valid and must be large enough for three channel values
		 */
		static inline void convertRGB24ToYUV24Pixel(const uint8_t* rgb, uint8_t* yuv);

	protected:

		/**
		 * Changes a subset of RGB 24 bit frame to a BGR 24 bit frame.
		 * @param frame The frame buffer to be changed, must be valid
		 * @param width The width of the frame in pixel, with range (0, infinity)
		 * @param firstRow First (including) row to convert, with range [0, height)
		 * @param numberRows Number of rows to convert, with range [1, height]
		 */
		static void changeRGB24ToBGR24Subset(uint8_t* frame, const unsigned int width, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Changes a subset of a RGB 24 bit frame to a YUV 24 bit frame by exact conversion.
		 * @param frame The frame buffer to be changed, must be valid
		 * @param width The width of the frame in pixel, with range (0, infinity)
		 * @param firstRow First (including) row to convert, with range [0, height)
		 * @param numberRows Number of rows to convert, with range [1, height]
		 */
		static void changeRGB24ToYUV24Subset(uint8_t* frame, const unsigned int width, const unsigned int firstRow, const unsigned int numberRows);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		/**
		 * Converts a RGB 24 bit row to a YUV 24 bit row by using NEON instructions.
		 * Beware: This function uses hard-coded conversion parameters which improves execution performance while also increases binary size when used.
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels receiving the converted pixel data, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [1, infinity)
		 * @param parameters Unused parameters, must be nullptr
		 */
		static void convertRGB24ToYUV24RowPrecision7BitNEON(const uint8_t* source, uint8_t* target, const size_t size, const void* parameters);

		/**
		 * Converts 16 RGB24 pixels to 16 YUV24 pixels by using NEON instructions.
		 * Beware: This function uses hard-coded conversion parameters which improves execution performance while also increases binary size when used.
		 * @param source The pointer to the 16 source pixels (with 3 channels = 48 bytes) to convert, must be valid
		 * @param target The pointer to the 16 target pixels (with 3 channels = 48 bytes) receiving the converted pixel data, must be valid
		 */
		static OCEAN_FORCE_INLINE void convert16PixelsRGB24ToYUV24Precision7BitNEON(const uint8_t* const source, uint8_t* const target);

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10
};

inline void FrameConverterRGB24::convertRGB24ToARGB32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	FrameChannels::addFirstChannelValue<uint8_t, 3u>(source, alphaValue, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterRGB24::convertRGB24ToBGR24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	FrameChannels::reverseChannelOrder<uint8_t, 3u>(source, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterRGB24::changeRGB24ToBGR24(uint8_t* frame, const unsigned int width, const unsigned int height, Worker* worker)
{
	ocean_assert(frame);
	ocean_assert(width != 0u && height != 0u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameConverterRGB24::changeRGB24ToBGR24Subset, frame, width, 0u, 0u), 0u, height, 2u, 3u, 100u);
	}
	else
	{
		changeRGB24ToBGR24Subset(frame, width, 0u, height);
	}
}

inline void FrameConverterRGB24::convertRGB24ToBGR32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	// source frame R G B
	//              0 1 2
	// target frame B G R -
	// pattern      2 1 0
	constexpr unsigned int shufflePattern = 0x012u;

	FrameChannels::shuffleChannelsAndSetLastChannelValue<uint8_t, 3u, 4u, shufflePattern>(source, 0u, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterRGB24::convertRGB24ToBGRA32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	// source frame R G B
	//              0 1 2
	// target frame B G R A
	// pattern      2 1 0
	constexpr unsigned int shufflePattern = 0x012u;

	FrameChannels::shuffleChannelsAndSetLastChannelValue<uint8_t, 3u, 4u, shufflePattern>(source, alphaValue, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterRGB24::convertRGB24ToRGB24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	FrameChannels::transformGeneric<uint8_t, 3u>(source, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterRGB24::convertRGB24ToRGB32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	FrameChannels::addLastChannelValue<uint8_t, 3u>(source, 0u, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterRGB24::convertRGB24ToRGBA32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	FrameChannels::addLastChannelValue<uint8_t, 3u>(source, alphaValue, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterRGB24::convertRGB24ToY8(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width * 3u + sourcePaddingElements;
	const unsigned int targetStrideElements = width + targetPaddingElements;

	constexpr unsigned int channelFactors[3] = {38u, 75u, 15u}; // 38/128 ~ 0.299, 75/128 ~ 0.587, 15/128 ~ 0.114

	static_assert(channelFactors[0] + channelFactors[1] + channelFactors[2] == 128u, "Invalid factors!");

	constexpr bool useFactorChannel0 = channelFactors[0] != 0u;
	constexpr bool useFactorChannel1 = channelFactors[1] != 0u;
	constexpr bool useFactorChannel2 = channelFactors[2] != 0u;

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, flag, CV::FrameChannels::convertRow3ChannelsTo1Channel8BitPerChannel7BitPrecision<useFactorChannel0, useFactorChannel1, useFactorChannel2>, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 1u>, areContinuous, channelFactors, worker);
}

inline void FrameConverterRGB24::convertRGB24ToYUV24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width * 3u + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 3u + targetPaddingElements;

#if defined(OCEAN_USE_HARDCODED_RGB24_TO_YUV24_CONVERTER) && OCEAN_HARDWARE_NEON_VERSION >= 10

	// we keep this function mainly to show the performance difference between the hard-coded implementation and a variable implementation

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, flag, FrameConverterRGB24::convertRGB24ToYUV24RowPrecision7BitNEON, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>, nullptr, worker);

#else

	// approximation:
	// Y = ( 33 * R + 64 * G + 13 * B) / 128 + 16
	// U = (-19 * R - 37 * G + 56 * B) / 128 + 128
	// V = ( 56 * R - 47 * G -  9 * B) / 128 + 128

	const int parameters[12] = {33, -19, 56, 64, -37, -47, 13, 56, -9, 16, 128, 128};

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, flag, CV::FrameChannels::convertRow3ChannelsTo3Channels8BitPerChannel7BitPrecision, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>, areContinuous, parameters, worker);

#endif
}

inline void FrameConverterRGB24::convertRGB24FullRangeToY_UV12LimitedRange(const uint8_t* source, uint8_t* yTarget, uint8_t* uvTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int uvTargetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && yTarget != nullptr && uvTarget != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	/*
	 * RGB input value range:  [ 0, 255]x[ 0, 255]x[ 0, 255]
	 * YUV output value range: [16, 235]x[16, 240]x[16, 240]
	 *
	 * | Y |   |  0.2578125   0.5039063   0.09765625   16.0 |   | R |
	 * | U | = | -0.1484375  -0.2890625   0.4375      128.0 | * | G |
	 * | V |   |  0.4375     -0.3671875  -0.0703125   128.0 |   | B |
	 *                                                          | 1 |
	 * Approximation with 7 bit precision:
	 *       | Y |     |  33     64    13     16 * 128 |   | R |
	 * 128 * | U |  =  | -19    -37    56    128 * 128 | * | G |
	 *       | V |     |  56    -47   -9     128 * 128 |   | B |
	 *                                                     | 1 |
	 */

	const int options[3 + 9 + 3] =
	{
		// padding parameters
		int(sourcePaddingElements), int(yTargetPaddingElements), int(uvTargetPaddingElements),

		// multiplication parameters
		33, -19, 56, 64, -37, -47, 13, 56, -9,

		// bias/translation parameters
		16, 128, 128
	};

	void* target[2] =
	{
		yTarget,
		uvTarget,
	};

	FrameConverter::convertArbitraryPixelFormat((const void**)(&source), target, width, height, flag, 2u, FrameConverter::convertTwoRows_1Plane3Channels_To_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_8BitPerChannel_Precision7Bit, options, worker);
}

inline void FrameConverterRGB24::convertRGB24FullRangeToY_VU12LimitedRange(const uint8_t* source, uint8_t* yTarget, uint8_t* vuTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int vuTargetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && yTarget != nullptr && vuTarget != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	/*
	 * RGB input value range:  [ 0, 255]x[ 0, 255]x[ 0, 255]
	 * YVU output value range: [16, 235]x[16, 240]x[16, 240]
	 *
	 * | Y |   |  0.2578125   0.5039063   0.09765625   16.0 |   | R |
	 * | V | = |  0.4375     -0.3671875  -0.0703125   128.0 | * | G |
	 * | U |   | -0.1484375  -0.2890625   0.4375      128.0 |   | B |
	 *                                                          | 1 |
	 * Approximation with 7 bit precision:
	 *       | Y |     |  33     64    13     16 * 128 |   | R |
	 * 128 * | V |  =  |  56    -47   -9     128 * 128 | * | G |
	 *       | U |     | -19    -37    56    128 * 128 |   | B |
	 *                                                     | 1 |
	 */

	const int options[3 + 9 + 3] =
	{
		// padding parameters
		int(sourcePaddingElements), int(yTargetPaddingElements), int(vuTargetPaddingElements),

		// multiplication parameters
		33, 56, -19, 64, -47, -37, 13, -9, 56,

		// bias/translation parameters
		16, 128, 128
	};

	void* target[2] =
	{
		yTarget,
		vuTarget
	};

	FrameConverter::convertArbitraryPixelFormat((const void**)(&source), target, width, height, flag, 2u, FrameConverter::convertTwoRows_1Plane3Channels_To_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_8BitPerChannel_Precision7Bit, options, worker);
}

inline void FrameConverterRGB24::convertRGB24FullRangeToY_UV12FullRange(const uint8_t* source, uint8_t* yTarget, uint8_t* uvTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int uvTargetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && yTarget != nullptr && uvTarget != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	/*
	 * RGB input value range:  [0, 255]x[0, 255]x[0, 255]
	 * YUV output value range: [0, 255]x[0, 255]x[0, 255]
	 *
	 * | Y |   |  0.299       0.587       0.114        0 |   | R |
	 * | U | = | -0.168736   -0.331264    0.5        128 | * | G |
	 * | V |   |  0.5        -0.418688   -0.081312   128 |   | B |
	 *                                                       | 1 |
	 * Approximation with 7 bit precision:
	 *       | Y |     |  38     75    15      0 * 128 |   | R |
	 * 128 * | U |  =  | -22    -42    64    128 * 128 | * | G |
	 *       | V |     |  64    -54   -10    128 * 128 |   | B |
	 *                                                     | 1 |
	 */

	const int options[3 + 9 + 3] =
	{
		// padding parameters
		int(sourcePaddingElements), int(yTargetPaddingElements), int(uvTargetPaddingElements),

		// multiplication parameters
		38, -22, 64, 75, -42, -54, 15, 64, -10,

		// bias/translation parameters
		0, 128, 128
	};

	void* target[2] =
	{
		yTarget,
		uvTarget
	};

	FrameConverter::convertArbitraryPixelFormat((const void**)(&source), target, width, height, flag, 2u, FrameConverter::convertTwoRows_1Plane3Channels_To_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_8BitPerChannel_Precision7Bit, options, worker);
}

inline void FrameConverterRGB24::convertRGB24FullRangeToY_VU12FullRange(const uint8_t* source, uint8_t* yTarget, uint8_t* vuTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int vuTargetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && yTarget != nullptr && vuTarget != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	/*
	 * RGB input value range:  [0, 255]x[0, 255]x[0, 255]
	 * YVU output value range: [0, 255]x[0, 255]x[0, 255]
	 *
	 * | Y |   |  0.299       0.587       0.114        0 |   | R |
	 * | V | = |  0.5        -0.418688   -0.081312   128 | * | G |
	 * | U |   | -0.168736   -0.331264    0.5        128 |   | B |
	 *                                                       | 1 |
	 * Approximation with 7 bit precision:
	 *       | Y |     |  38     75    15      0 * 128 |   | R |
	 * 128 * | V |  =  |  64    -54   -10    128 * 128 | * | G |
	 *       | U |     | -22    -42    64    128 * 128 |   | B |
	 *                                                     | 1 |
	 */

	const int options[3 + 9 + 3] =
	{
		// padding parameters
		int(sourcePaddingElements), int(yTargetPaddingElements), int(vuTargetPaddingElements),

		// multiplication parameters
		38, 64, -22, 75, -54, -42, 15, -10, 64,

		// bias/translation parameters
		0, 128, 128
	};

	void* target[2] =
	{
		yTarget,
		vuTarget
	};

	FrameConverter::convertArbitraryPixelFormat((const void**)(&source), target, width, height, flag, 2u, FrameConverter::convertTwoRows_1Plane3Channels_To_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_8BitPerChannel_Precision7Bit, options, worker);
}

inline void FrameConverterRGB24::convertRGB24FullRangeToY_U_V12LimitedRange(const uint8_t* source, uint8_t* yTarget, uint8_t* uTarget, uint8_t* vTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int uTargetPaddingElements, const unsigned int vTargetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && yTarget != nullptr && uTarget != nullptr && vTarget != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	/*
	 * RGB input value range:  [ 0, 255]x[ 0, 255]x[ 0, 255]
	 * YUV output value range: [16, 235]x[16, 240]x[16, 240]
	 *
	 * | Y |   |  0.2578125   0.5039063   0.09765625   16.0 |   | R |
	 * | U | = | -0.1484375  -0.2890625   0.4375      128.0 | * | G |
	 * | V |   |  0.4375     -0.3671875  -0.0703125   128.0 |   | B |
	 *                                                          | 1 |
	 * Approximation with 7 bit precision:
	 *       | Y |     |  33     64    13     16 * 128 |   | R |
	 * 128 * | U |  =  | -19    -37    56    128 * 128 | * | G |
	 *       | V |     |  56    -47   -9     128 * 128 |   | B |
	 *                                                     | 1 |
	 */

	const int options[4 + 9 + 3] =
	{
		// padding parameters
		int(sourcePaddingElements), int(yTargetPaddingElements), int(uTargetPaddingElements), int(vTargetPaddingElements),

		// multiplication parameters
		33, -19, 56, 64, -37, -47, 13, 56, -9,

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

inline void FrameConverterRGB24::convertRGB24FullRangeToY_V_U12LimitedRange(const uint8_t* source, uint8_t* yTarget, uint8_t* vTarget, uint8_t* uTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int vTargetPaddingElements, const unsigned int uTargetPaddingElements, Worker* worker)
{
	// just swapping u- and v-plane, and calling converter to Y_U_V12

	convertRGB24FullRangeToY_U_V12LimitedRange(source, yTarget, uTarget, vTarget, width, height, flag, sourcePaddingElements, yTargetPaddingElements, uTargetPaddingElements, vTargetPaddingElements, worker);
}

inline void FrameConverterRGB24::convertRGB24FullRangeToY_U_V12FullRange(const uint8_t* source, uint8_t* yTarget, uint8_t* uTarget, uint8_t* vTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int uTargetPaddingElements, const unsigned int vTargetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && yTarget != nullptr && uTarget != nullptr && vTarget != nullptr);

	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
	{
		return;
	}

	/*
	 * RGB input value range:  [0, 255]x[0, 255]x[0, 255]
	 * YUV output value range: [0, 255]x[0, 255]x[0, 255]
	 *
	 * | Y |   |  0.299       0.587       0.114        0 |   | R |
	 * | U | = | -0.168736   -0.331264    0.5        128 | * | G |
	 * | V |   |  0.5        -0.418688   -0.081312   128 |   | B |
	 *                                                       | 1 |
	 * Approximation with 7 bit precision:
	 *       | Y |     |  38     75    15      0 * 128 |   | R |
	 * 128 * | U |  =  | -22    -42    64    128 * 128 | * | G |
	 *       | V |     |  64    -54   -10    128 * 128 |   | B |
	 *                                                     | 1 |
	 */

	const int options[4 + 9 + 3] =
	{
		// padding parameters
		int(sourcePaddingElements), int(yTargetPaddingElements), int(uTargetPaddingElements), int(vTargetPaddingElements),

		// multiplication parameters
		38, -22, 64, 75, -42, -54, 15, 64, -10,

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

inline void FrameConverterRGB24::convertRGB24FullRangeToY_V_U12FullRange(const uint8_t* source, uint8_t* yTarget, uint8_t* vTarget, uint8_t* uTarget, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int vTargetPaddingElements, const unsigned int uTargetPaddingElements, Worker* worker)
{
	// just swapping u- and v-plane, and calling converter to Y_U_V12

	convertRGB24FullRangeToY_U_V12FullRange(source, yTarget, uTarget, vTarget, width, height, flag, sourcePaddingElements, yTargetPaddingElements, uTargetPaddingElements, vTargetPaddingElements, worker);
}

inline void FrameConverterRGB24::changeRGB24ToYUV24(uint8_t* frame, const unsigned int width, const unsigned int height, Worker* worker)
{
	ocean_assert(frame);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameConverterRGB24::changeRGB24ToYUV24Subset, frame, width, 0u, 0u), 0u, height, 2u, 3u, 100u);
	}
	else
	{
		changeRGB24ToYUV24Subset(frame, width, 0u, height);
	}
}

inline void FrameConverterRGB24::convertRGB24ToYUV24Pixel(const uint8_t* rgb, uint8_t* yuv)
{
	ocean_assert(rgb && yuv);

	yuv[0] = uint8_t(((rgb[0] * 66 + rgb[1] * 129 + rgb[2] * 25 + 128) >> 8) + 16);
	yuv[1] = uint8_t(((rgb[0] * -38 - rgb[1] * 74 + rgb[2] * 112 + 128) >> 8) + 128);
	yuv[2] = uint8_t(((rgb[0] * 112 - rgb[1] * 94 - rgb[2] * 18 + 128) >> 8) + 128);

	ocean_assert(abs(int(yuv[0]) - int(((rgb[0] * 66 + rgb[1] * 129 + rgb[2] * 25 + 128) / 256) + 16)) <= 1);
	ocean_assert(abs(int(yuv[1]) - int(((rgb[0] * -38 - rgb[1] * 74 + rgb[2] * 112 + 128) / 256) + 128)) <= 1);
	ocean_assert(abs(int(yuv[2]) - int(((rgb[0] * 112 - rgb[1] * 94 - rgb[2] * 18 + 128) / 256) + 128)) <= 1);
}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

OCEAN_FORCE_INLINE void FrameConverterRGB24::convert16PixelsRGB24ToYUV24Precision7BitNEON(const uint8_t* const source, uint8_t* const target)
{
	ocean_assert(source != nullptr && target != nullptr);

	// the documentation of this function designed for RGB24 to YUV24 conversion

	// precise color space conversion:
	// | Y |   |  0.2578125   0.5039063   0.09765625   16.0 |   | R |
	// | U | = | -0.1484375  -0.2890625   0.4375      128.0 | * | G |
	// | V |   |  0.4375     -0.3671875  -0.0703125   128.0 |   | B |
	//                                                          | 1 |

	// approximation:
	// Y = ( 33 * R + 64 * G + 13 * B) / 128 + 16
	// U = (-19 * R - 37 * G + 56 * B) / 128 + 128
	// V = ( 56 * R - 47 * G -  9 * B) / 128 + 128

	// we load 8 pixels (= 3 * 8 values) and directly deinterleave the 3 channels so that we receive the following patterns:
	// source_u_8x8x3.val[0]: R R R R R R R R
	// source_u_8x8x3.val[1]: G G G G G G G G
	// source_u_8x8x3.val[2]: B B B B B B B B

	const uint8x16x3_t source_u_8x16x3 = vld3q_u8(source);

	const uint8x8_t source0_low_u_8x8 = vget_low_u8(source_u_8x16x3.val[0]);
	const uint8x8_t source0_high_u_8x8 = vget_high_u8(source_u_8x16x3.val[0]);

	const uint8x8_t source1_low_u_8x8 = vget_low_u8(source_u_8x16x3.val[1]);
	const uint8x8_t source1_high_u_8x8 = vget_high_u8(source_u_8x16x3.val[1]);

	const uint8x8_t source2_low_u_8x8 = vget_low_u8(source_u_8x16x3.val[2]);
	const uint8x8_t source2_high_u_8x8 = vget_high_u8(source_u_8x16x3.val[2]);


	const uint8x8_t constant33_u_8x8 = vdup_n_u8(33);
	uint16x8_t intermediateResults0_low_u_16x8 = vmull_u8(source0_low_u_8x8, constant33_u_8x8);
	uint16x8_t intermediateResults0_high_u_16x8 = vmull_u8(source0_high_u_8x8, constant33_u_8x8);

	const uint8x8_t constant64_u_8x8 = vdup_n_u8(64);
	intermediateResults0_low_u_16x8 = vmlal_u8(intermediateResults0_low_u_16x8, source1_low_u_8x8, constant64_u_8x8);
	intermediateResults0_high_u_16x8 = vmlal_u8(intermediateResults0_high_u_16x8, source1_high_u_8x8, constant64_u_8x8);

	const uint8x8_t constant13_u_8x8 = vdup_n_u8(13);
	intermediateResults0_low_u_16x8 = vmlal_u8(intermediateResults0_low_u_16x8, source2_low_u_8x8, constant13_u_8x8);
	intermediateResults0_high_u_16x8 = vmlal_u8(intermediateResults0_high_u_16x8, source2_high_u_8x8, constant13_u_8x8);


	const uint8x8_t constant56_u_8x8 = vdup_n_u8(56);
	uint16x8_t intermediateResults1_low_u_16x8 = vmull_u8(source2_low_u_8x8, constant56_u_8x8);
	uint16x8_t intermediateResults1_high_u_16x8 = vmull_u8(source2_high_u_8x8, constant56_u_8x8);

	uint16x8_t intermediateResults2_low_u_16x8 = vmull_u8(source0_low_u_8x8, constant56_u_8x8);
	uint16x8_t intermediateResults2_high_u_16x8 = vmull_u8(source0_high_u_8x8, constant56_u_8x8);


	const uint8x8_t constant19_u_8x8 = vdup_n_u8(19);
	intermediateResults1_low_u_16x8 = vmlsl_u8(intermediateResults1_low_u_16x8, source0_low_u_8x8, constant19_u_8x8);
	intermediateResults1_high_u_16x8 = vmlsl_u8(intermediateResults1_high_u_16x8, source0_high_u_8x8, constant19_u_8x8);

	const uint8x8_t constant37_u_8x8 = vdup_n_u8(37);
	intermediateResults1_low_u_16x8 = vmlsl_u8(intermediateResults1_low_u_16x8, source1_low_u_8x8, constant37_u_8x8);
	intermediateResults1_high_u_16x8 = vmlsl_u8(intermediateResults1_high_u_16x8, source1_high_u_8x8, constant37_u_8x8);


	const uint8x8_t constant47_u_8x8 = vdup_n_u8(47);
	intermediateResults2_low_u_16x8 = vmlsl_u8(intermediateResults2_low_u_16x8, source1_low_u_8x8, constant47_u_8x8);
	intermediateResults2_high_u_16x8 = vmlsl_u8(intermediateResults2_high_u_16x8, source1_high_u_8x8, constant47_u_8x8);

	const uint8x8_t constant9_u_8x8 = vdup_n_u8(9);
	intermediateResults2_low_u_16x8 = vmlsl_u8(intermediateResults2_low_u_16x8, source2_low_u_8x8, constant9_u_8x8);
	intermediateResults2_high_u_16x8 = vmlsl_u8(intermediateResults2_high_u_16x8, source2_high_u_8x8, constant9_u_8x8);


	const int16x8_t constant16_s_16x8 = vdupq_n_s16(16 * 128);
	const int16x8_t constant128_s_16x8 = vdupq_n_s16(128 * 128);

	const int16x8_t intermediateResults0_low_s_16x8 = vqaddq_s16(vreinterpretq_s16_u16(intermediateResults0_low_u_16x8), constant16_s_16x8);
	const int16x8_t intermediateResults0_high_s_16x8 = vqaddq_s16(vreinterpretq_s16_u16(intermediateResults0_high_u_16x8), constant16_s_16x8);

	const int16x8_t intermediateResults1_low_s_16x8 = vqaddq_s16(vreinterpretq_s16_u16(intermediateResults1_low_u_16x8), constant128_s_16x8);
	const int16x8_t intermediateResults1_high_s_16x8 = vqaddq_s16(vreinterpretq_s16_u16(intermediateResults1_high_u_16x8), constant128_s_16x8);

	const int16x8_t intermediateResults2_low_s_16x8 = vqaddq_s16(vreinterpretq_s16_u16(intermediateResults2_low_u_16x8), constant128_s_16x8);
	const int16x8_t intermediateResults2_high_s_16x8 = vqaddq_s16(vreinterpretq_s16_u16(intermediateResults2_high_u_16x8), constant128_s_16x8);

	// saturated narrow signed to unsigned

	uint8x16x3_t results_u_8x16x3;
	results_u_8x16x3.val[0] = vcombine_u8(vqrshrun_n_s16(intermediateResults0_low_s_16x8, 7), vqrshrun_n_s16(intermediateResults0_high_s_16x8, 7));
	results_u_8x16x3.val[1] = vcombine_u8(vqrshrun_n_s16(intermediateResults1_low_s_16x8, 7), vqrshrun_n_s16(intermediateResults1_high_s_16x8, 7));
	results_u_8x16x3.val[2] = vcombine_u8(vqrshrun_n_s16(intermediateResults2_low_s_16x8, 7), vqrshrun_n_s16(intermediateResults2_high_s_16x8, 7));

	// and we can store the result
	vst3q_u8(target, results_u_8x16x3);
}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

}

}

#endif // META_OCEAN_CV_FRAME_CONVERTER_RGB_24_H
