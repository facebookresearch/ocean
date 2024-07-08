/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_OL_IMAGE_WEBP_H
#define META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_OL_IMAGE_WEBP_H

#include "ocean/media/openimagelibraries/OpenImageLibraries.h"

#include "ocean/base/Frame.h"

#ifdef OCEAN_MEDIA_OIL_SUPPORT_WEBP

namespace Ocean
{

namespace Media
{

namespace OpenImageLibraries
{

/**
 * This class implements read and write functions for WEBP images.
 * @ingroup mediaoil
 */
class OCEAN_MEDIA_OIL_EXPORT ImageWebp
{
	public:

		/**
		 * Decode a WEBP image from a given binary buffer.
		 * @param buffer The buffer from which the image will be loaded, must be valid
		 * @param size The size of the given buffer in bytes, with range [1, infinity)
		 * @return The frame containing the image information; if the input has alpha the output will be `FORMAT_RGBA32` otherwise it will be `FORMAT_RGB24`; an invalid frame if the image could not be loaded
		 */
		static Frame decodeImage(const void* buffer, const size_t size);

		/**
		 * Encode (lossless) a given frame as WEBP image to a resulting buffer.
		 * @param frame The frame to be written, must be valid
		 * @param buffer The resulting buffer storing the binary information of the WEBP image
		 * @param allowConversion True, to allow an internal conversion of the frame if WEBP does not support the given frame type; False, to prevent a conversion and to stop creating the buffer
		 * @param hasBeenConverted Optional resulting statement whether the frame had to be converted to a different pixel format before it could be written; True, if so; False, if not
		 * @param quality The WEBP compression quality to be used in percent, the higher the value the better the image quality (but: larger binary size and longer compression times), with range [0, 100]
		 * @return True, if succeeded; False, if the frame could not be written as WEBP image
		 */
		static bool encodeImage(const Frame& frame, std::vector<unsigned char>& buffer, const bool allowConversion = true, bool* hasBeenConverted = nullptr, const float quality = 80.0f);

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
		 * Translates a WEBP status code into a human-readable string
		 * @param statusCode The WEBP status code that will be translated
		 * @return The translated, human-readable string
		 */
		static std::string translateVP8StatusCode(const int statusCode);
};

inline bool ImageWebp::isPixelFormatSupported(const FrameType::PixelFormat pixelFormat)
{
#if 0
	// Disabled RGBA and BGRA for now; for some tiny images with alpha the WEBP encoder simply drops the alpha layer even when `WebPConfig::exact == 1`.
	return pixelFormat == FrameType::FORMAT_RGB24 || pixelFormat == FrameType::FORMAT_BGR24
		|| pixelFormat == FrameType::FORMAT_RGBA32	|| pixelFormat == FrameType::FORMAT_BGRA32;
#else
	return pixelFormat == FrameType::FORMAT_RGB24 || pixelFormat == FrameType::FORMAT_BGR24;
#endif
}

inline bool ImageWebp::isPixelOriginSupported(const FrameType::PixelOrigin pixelOrigin)
{
	return pixelOrigin == FrameType::ORIGIN_UPPER_LEFT;
}

inline bool ImageWebp::isFrameTypeSupported(const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin)
{
	return isPixelFormatSupported(pixelFormat) && isPixelOriginSupported(pixelOrigin);
}

}

}

}

#endif // OCEAN_MEDIA_OIL_SUPPORT_WEBP

#endif // META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_OL_IMAGE_WEBP_H
