/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_IMAGE_PNG_H
#define META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_IMAGE_PNG_H

#include "ocean/media/openimagelibraries/OpenImageLibraries.h"

#include "ocean/base/Frame.h"
#include "ocean/base/ScopedObject.h"

#ifdef OCEAN_MEDIA_OIL_SUPPORT_PNG

namespace Ocean
{

namespace Media
{

namespace OpenImageLibraries
{

/**
 * This class implements read and write functions for PNG images.
 * @ingroup mediaoil
 */
class OCEAN_MEDIA_OIL_EXPORT ImagePng
{
	protected:

		/**
		 * Definition of a pair combining a pointer to data input buffer with the size of the buffer in bytes.
		 */
		typedef std::pair<const uint8_t**, size_t> DataInputPair;

		/**
		 * Definition of a pair combining an output buffer (with additional reserved bytes) with the number of bytes actually used in the buffer.
		 */
		typedef std::pair<std::vector<uint8_t>, size_t> OutputDataPair;

	public:

		/**
		 * Decodes a PNG image from a given binary buffer.
		 * @param buffer The buffer from which the image will be loaded, must be valid
		 * @param size The size of the given buffer in bytes, with range [1, infinity)
		 * @return The frame containing the image information, an invalid frame if the image could not be loaded
		 */
		static Frame decodeImage(const void* buffer, const size_t size);

		/**
		 * Encodes a given frame as PNG image to a resulting buffer.
		 * @param frame The frame to be written, must be valid
		 * @param buffer The resulting buffer storing the binary information of the PNG image
		 * @param allowConversion True, to allow an internal conversion of the frame if PNG does not support the given frame type; False, to prevent a conversion and to stop creating the buffer
		 * @param hasBeenConverted Optional resulting statement whether the frame had to be converted to a different pixel format before it could be written; True, if so; False, if not
		 * @return True, if succeeded; False, if the frame could not be written as PNG image
		 */
		static bool encodeImage(const Frame& frame, std::vector<uint8_t>& buffer, const bool allowConversion = true, bool* hasBeenConverted = nullptr);

		/**
		 * Returns whether a given pixel format is supported natively.
		 * @param pixelFormat The pixel format to be checked
		 * @return True, if so; False, if a conversion will be necessary
		 */
		static inline bool isPixelFormatSupported(const FrameType::PixelFormat pixelFormat);

		/**
		 * Returns whether a given pixel origin is supported natively.
		 * @param pixelOrigin The pixel origin to be checked
		 * @return True, if so; False, if a conversion will be necessary
		 */
		static inline bool isPixelOriginSupported(const FrameType::PixelOrigin pixelOrigin);

		/**
		 * Returns whether a given pixel format together with a given pixel origin is supported natively.
		 * @param pixelFormat The pixel format to be checked
		 * @param pixelOrigin The pixel origin to be checked
		 * @return True, if so; False, if a conversion will be necessary
		 */
		static inline bool isFrameTypeSupported(const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin);

	protected:

		/**
		 * Translates a PNG pixel format defined by the color type and the bit depth per channel.
		 * @param pngColorType The color space defined by libpng
		 * @param pngBitDepthPerChannel The number of bit per channel, with range [1, infinity)
		 * @return The resulting Ocean-based pixel format, FORMAT_UNDEFINED if no matching pixel format exists
		 */
		static FrameType::PixelFormat translatePixelFormat(const int pngColorType, const int pngBitDepthPerChannel);

		/**
		 * Translates a Ocean-based pixel format to a PNG pixel format defined by the color type and the bit depth per channel.
		 * @param pixelFormat Ocean-based pixel format to be converted
		 * @param pngColorType Resulting color space defined by libpng
		 * @param pngBitDepthPerChannel Resulting number of bit per channel, with range [1, infinity)
		 * @return True, if the Ocean-based pixel format has a equivalent PNG format; False, if the pixel format cannot be represented in PNG
		 */
		static bool translatePixelFormat(const FrameType::PixelFormat pixelFormat, int& pngColorType, int& pngBitDepthPerChannel);

		/**
		 * Reads a defined number of bytes from a given buffer and copies them to a given buffer provided by PNG.
		 * @param pngReadStruct The PNG read struct requesting the data, must be valid
		 * @param outBytes The buffer provided by PNG receiving the data, must be valid
		 * @param byteCountToRead The number of bytes that are requested
		 */
		static void readInputData(void* pngReadStruct, unsigned char* outBytes, const size_t byteCountToRead);

		/**
		 * Writes a defined number of bytes from a given PNG buffer to an external buffer.
		 * @param pngWriteStruct The PNG write struct providing the data, must be valid
		 * @param buffer The buffer provided by PNG holding the data, must be valid
		 * @param size The number of bytes provided in the buffer
		 */
		static void writeOutputData(void* pngWriteStruct, unsigned char* buffer, const size_t size);

		/**
		 * Event function triggered by PNG to flush the external buffer.
		 * @param pngWriteStruct  The PNG write struct triggering the event, must be valid
		 */
		static inline void flushOutputData(void* pngWriteStruct);
};

inline void ImagePng::flushOutputData(void* /*pngWriteStruct*/)
{
	// nothing to do here
}

inline bool ImagePng::isPixelFormatSupported(const FrameType::PixelFormat pixelFormat)
{
	return pixelFormat == FrameType::FORMAT_RGB24
				|| pixelFormat == FrameType::FORMAT_RGBA32
				|| pixelFormat == FrameType::FORMAT_RGBA64
				|| pixelFormat == FrameType::FORMAT_Y8
				|| pixelFormat == FrameType::FORMAT_Y16
				|| pixelFormat == FrameType::FORMAT_YA16;
}

inline bool ImagePng::isPixelOriginSupported(const FrameType::PixelOrigin pixelOrigin)
{
	return pixelOrigin == FrameType::ORIGIN_UPPER_LEFT;
}

inline bool ImagePng::isFrameTypeSupported(const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin)
{
	return isPixelFormatSupported(pixelFormat) && isPixelOriginSupported(pixelOrigin);
}

}

}

}

#endif // OCEAN_MEDIA_OIL_SUPPORT_PNG

#endif // META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_IMAGE_PNG_H
