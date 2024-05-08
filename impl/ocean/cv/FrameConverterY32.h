/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_CONVERTER_Y_32_H
#define META_OCEAN_CV_FRAME_CONVERTER_Y_32_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameChannels.h"

namespace Ocean
{

namespace CV
{

/**
 * This class provides functions to convert frames with Y32 pixel format.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameConverterY32 : public FrameConverter
{
	public:

		/**
		 * Converts a Y 32 bit frame to a Y 8 bit frame by dividing by 2^24
		 * @param source The source frame buffer
		 * @param target The target frame buffer
		 * @param width The width of the frame in pixel
		 * @param height The height of the frame in pixel
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in (uint32_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertY32ToY8(const uint32_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y 32 bit frame to a Y 16 bit frame by dividing by 2^16
		 * @param source The source frame buffer
		 * @param target The target frame buffer
		 * @param width The width of the frame in pixel
		 * @param height The height of the frame in pixel
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in (uint32_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint16_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertY32ToY16(const uint32_t* source, uint16_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

	protected:

		/**
		 * Converts a Y32 row to a Y8 row.
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels receiving the converted pixel data, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [1, infinity)
		 * @param unusedParameters Unused parameters, must be nullptr
		 */
		static inline void convertRowY32ToY8(const uint32_t* source, uint8_t* target, const size_t size, const void* unusedParameters = nullptr);

		/**
		 * Converts a Y32 row to a Y16 row.
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels receiving the converted pixel data, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [1, infinity)
		 * @param unusedParameters Unused parameters, must be nullptr
		 */
		static inline void convertRowY32ToY16(const uint32_t* source, uint16_t* target, const size_t size, const void* unusedParameters = nullptr);
};

inline void FrameConverterY32::convertY32ToY8(const uint32_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width + sourcePaddingElements;
	const unsigned int targetStrideElements = width + targetPaddingElements;

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, flag, convertRowY32ToY8, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 1u>, areContinuous, nullptr, worker);
}

inline void FrameConverterY32::convertY32ToY16(const uint32_t* source, uint16_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width + sourcePaddingElements;
	const unsigned int targetStrideElements = width + targetPaddingElements;

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, flag, convertRowY32ToY16, CV::FrameChannels::reverseRowPixelOrderInPlace<uint16_t, 1u>, areContinuous, nullptr, worker);
}

inline void FrameConverterY32::convertRowY32ToY8(const uint32_t* source, uint8_t* target, const size_t size, const void* /*unusedParameters*/)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(size >= 1);

	for (size_t n = 0; n < size; ++n)
	{
		const uint32_t* const sourcePixel = source + n;
		uint8_t* const targetPixel = target + n;

		// instead of right shifhting, we simply extract the fourth byte
		*targetPixel = ((uint8_t*)sourcePixel)[3];
	}
}

inline void FrameConverterY32::convertRowY32ToY16(const uint32_t* source, uint16_t* target, const size_t size, const void* /*unusedParameters*/)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(size >= 1);

	for (size_t n = 0; n < size; ++n)
	{
		const uint32_t* const sourcePixel = source + n;
		uint16_t* const targetPixel = target + n;

		// instead of right shifhting, we simply extract the two high bytes
		*targetPixel = ((uint16_t*)sourcePixel)[1];
	}
}

}

}

#endif // META_OCEAN_CV_FRAME_CONVERTER_Y_32_H
