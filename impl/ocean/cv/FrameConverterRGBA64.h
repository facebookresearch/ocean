/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_CONVERTER_RGBA_64_H
#define META_OCEAN_CV_FRAME_CONVERTER_RGBA_64_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameChannels.h"

namespace Ocean
{

namespace CV
{

/**
 * This class provides functions to convert frames with RGBA64 pixel format.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameConverterRGBA64 : public FrameConverter
{
	public:

		/**
		 * Converts an RGBA 64 bit frame to a RGB 24 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker to distribute the computation to several CPU cores
		 */
		static inline void convertRGBA64ToRGB24(const uint16_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts an RGBA 64 bit frame to a RGBA 32 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker to distribute the computation to several CPU cores
		 */
		static inline void convertRGBA64ToRGBA32(const uint16_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts an RGBA 64 bit frame to a RGBA 64 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker to distribute the computation to several CPU cores
		 */
		static inline void convertRGBA64ToRGBA64(const uint16_t* source, uint16_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

	protected:

		/**
		 * Converts a RGBA64 row to a RGB24 row applying a linear conversion.
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels receiving the converted pixel data, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [1, infinity)
		 * @param unusedParameters Unused parameters, must be nullptr
		 */
		static inline void convertRowRGBA64ToRGB24(const uint16_t* source, uint8_t* target, const size_t size, const void* unusedParameters = nullptr);

		/**
		 * Converts a RGBA64 row to a RGBA32 row applying a linear conversion.
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels receiving the converted pixel data, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [1, infinity)
		 * @param unusedParameters Unused parameters, must be nullptr
		 */
		static inline void convertRowRGBA64ToRGBA32(const uint16_t* source, uint8_t* target, const size_t size, const void* unusedParameters = nullptr);

};

inline void FrameConverterRGBA64::convertRGBA64ToRGB24(const uint16_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width * 4u + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 3u + targetPaddingElements;

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, flag, convertRowRGBA64ToRGB24, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>, areContinuous, nullptr, worker);
}

inline void FrameConverterRGBA64::convertRGBA64ToRGBA32(const uint16_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width * 4u + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 4u + targetPaddingElements;

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, flag, convertRowRGBA64ToRGBA32, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 4u>, areContinuous, nullptr, worker);
}

inline void FrameConverterRGBA64::convertRGBA64ToRGBA64(const uint16_t* source, uint16_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	FrameChannels::transformGeneric<uint16_t, 4u>(source, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterRGBA64::convertRowRGBA64ToRGB24(const uint16_t* source, uint8_t* target, const size_t size, const void* /*unusedParameters*/)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(size >= 1);

	const uint8_t* source8 = (const uint8_t*)(source);

	for (size_t n = 0; n < size; ++n)
	{
		// instead of right shifting, we simply extract the second byte
		target[n * 3 + 0] = source8[n * 8 + 1];
		target[n * 3 + 1] = source8[n * 8 + 3];
		target[n * 3 + 2] = source8[n * 8 + 5];
	}
}

inline void FrameConverterRGBA64::convertRowRGBA64ToRGBA32(const uint16_t* source, uint8_t* target, const size_t size, const void* /*unusedParameters*/)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(size >= 1);

	const uint8_t* source8 = (const uint8_t*)(source);

	for (size_t n = 0; n < size; ++n)
	{
		// instead of right shifting, we simply extract the second byte
		target[n * 4 + 0] = source8[n * 8 + 1];
		target[n * 4 + 1] = source8[n * 8 + 3];
		target[n * 4 + 2] = source8[n * 8 + 5];
		target[n * 4 + 3] = source8[n * 8 + 7];
	}
}

}

}

#endif // META_OCEAN_CV_FRAME_CONVERTER_RGBA_64_H
