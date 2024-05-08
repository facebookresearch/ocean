/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_CONVERTER_RGB_565_H
#define META_OCEAN_CV_FRAME_CONVERTER_RGB_565_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameChannels.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class provides functions to convert or to change frames with RGB 565 pixel format.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameConverterRGB565 : public FrameConverter
{
	public:

		/**
		 * Converts a RGB565 (16 bit) frame to a RGB24 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range (0, infinity)
		 * @param height The height of the frame in pixel, with range (0, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in (uint16_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertRGB565ToRGB24(const uint16_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a RGB565 (16 bit) frame to a Y8 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range (0, infinity)
		 * @param height The height of the frame in pixel, with range (0, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in (uint16_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param flag Determining the type of conversion
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertRGB565ToY8(const uint16_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

	protected:

		/**
		 * Converts a row of a RGB656 frame to a row of a RGB24 frame.
		 * @param source The source row, must be valid
		 * @param target The target row, must be valid
		 * @param width The width of the row in pixel, with range [1, infinity)
		 * @param unusedOptions Unused options parameter, must be nullptr
		 */
		static void convertRowRGB565ToRGB24(const uint16_t* source, uint8_t* target, const size_t width, const void* unusedOptions);

		/**
		 * Converts a row of a RGB656 frame to a row of a Y8 frame.
		 * @param source The source row, must be valid
		 * @param target The target row, must be valid
		 * @param width The width of the row in pixel, with range [1, infinity)
		 * @param unusedOptions Unused options parameter, must be nullptr
		 */
		static void convertRowRGB565ToY8(const uint16_t* source, uint8_t* target, const size_t width, const void* unusedOptions);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		/**
		 * Converts a row of a RGB656 frame to a row of a RGB24 frame using NEON instructions.
		 * @param source The source row, must be valid
		 * @param target The target row, must be valid
		 * @param width The width of the row in pixel, with range [8, infinity)
		 */
		static void convertRowRGB565ToRGB24NEON(const uint16_t* source, uint8_t* target, const unsigned int width);

		/**
		 * Converts a row of a RGB656 frame to a row of a Y8 frame using NEON instructions.
		 * @param source The source row, must be valid
		 * @param target The target row, must be valid
		 * @param width The width of the row in pixel, with range [8, infinity)
		 */
		static void convertRowRGB565ToY8NEON(const uint16_t* source, uint8_t* target, const unsigned int width);

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10
};

inline void FrameConverterRGB565::convertRGB565ToRGB24(const uint16_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 3u + targetPaddingElements;

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat<uint16_t, uint8_t>(source, target, width, height, sourceStrideElements, targetStrideElements, flag, CV::FrameConverterRGB565::convertRowRGB565ToRGB24, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>, areContinuous, nullptr, worker);
}

inline void FrameConverterRGB565::convertRGB565ToY8(const uint16_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 1u + targetPaddingElements;

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat<uint16_t, uint8_t>(source, target, width, height, sourceStrideElements, targetStrideElements, flag, CV::FrameConverterRGB565::convertRowRGB565ToY8, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 1u>, areContinuous, nullptr, worker);
}

}

}

#endif // META_OCEAN_CV_FRAME_CONVERTER_RGB_24_H
