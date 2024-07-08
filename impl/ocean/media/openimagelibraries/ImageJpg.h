/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_OL_IMAGE_JPG_H
#define META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_OL_IMAGE_JPG_H

#include "ocean/media/openimagelibraries/OpenImageLibraries.h"

#include "ocean/base/Frame.h"

#ifdef OCEAN_MEDIA_OIL_SUPPORT_JPG

namespace Ocean
{

namespace Media
{

namespace OpenImageLibraries
{

/**
 * This class implements read and write functions for JPEG images.
 * @ingroup mediaoil
 */
class OCEAN_MEDIA_OIL_EXPORT ImageJpg
{
	public:

		/**
		 * Decode a JPEG image from a given binary buffer.
		 * @param buffer The buffer from which the image will be loaded, must be valid
		 * @param size The size of the given buffer in bytes, with range [1, infinity)
		 * @return The frame containing the image information, an invalid frame if the image could not be loaded
		 */
		static Frame decodeImage(const void* buffer, const size_t size);

		/**
		 * Encode a given frame as JPEG image to a resulting buffer.
		 * @param frame The frame to be written, must be valid
		 * @param buffer The resulting buffer storing the binary information of the JPEG image
		 * @param allowConversion True, to allow an internal conversion of the frame if JPEG does not support the given frame type; False, to prevent a conversion and to stop creating the buffer
		 * @param hasBeenConverted Optional resulting statement whether the frame had to be converted to a different pixel format before it could be written; True, if so; False, if not
		 * @param quality The JPEG compression quality to be used in percent, the higher the value the better the image quality (the larger the binary size), with range [0, 100]
		 * @return True, if succeeded; False, if the frame could not be written as JPEG image
		 */
		static bool encodeImage(const Frame& frame, std::vector<unsigned char>& buffer, const bool allowConversion = true, bool* hasBeenConverted = nullptr, const int quality = 80);

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
		 * Translates a JPEG pixel format defined by the color space, the component precision and the number of components.
		 * @param jpegColorSpace The color space defined by jpeglib
		 * @param jpegPrecision The number of bit per component, with range [1, infinity)
		 * @param jpegNumberComponents The number of components per pixel, with range [1, infinity)
		 * @return The resulting Ocean-based pixel format, FORMAT_UNDEFINED if no matching pixel format exists
		 */
		static FrameType::PixelFormat translatePixelFormat(const int jpegColorSpace, const int jpegPrecision, const int jpegNumberComponents);

		/**
		 * Translates a Ocean-based pixel format to a JPEG pixel format defined by the color space, the component precision and the number of components.
		 * @param pixelFormat The Ocean-based pixel format to be translated
		 * @param jpegColorSpace Resulting color space defined by jpeglib
		 * @param jpegPrecision Resulting number of bit per component, with range [1, infinity)
		 * @param jpegNumberComponents Resulting number of components per pixel, with range [1, infinity)
		 * @return True, if the Ocean-based pixel format has a equivalent JPEG format; False, if the pixel format cannot be represented in JPEG
		 */
		static bool translatePixelFormat(const FrameType::PixelFormat pixelFormat, int& jpegColorSpace, int& jpegPrecision, int& jpegNumberComponents);
};

inline bool ImageJpg::isPixelFormatSupported(const FrameType::PixelFormat pixelFormat)
{
	return pixelFormat == FrameType::FORMAT_RGB24 || pixelFormat == FrameType::FORMAT_YUV24
				|| pixelFormat == FrameType::FORMAT_Y8;
}

inline bool ImageJpg::isPixelOriginSupported(const FrameType::PixelOrigin pixelOrigin)
{
	return pixelOrigin == FrameType::ORIGIN_UPPER_LEFT;
}

inline bool ImageJpg::isFrameTypeSupported(const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin)
{
	return isPixelFormatSupported(pixelFormat) && isPixelOriginSupported(pixelOrigin);
}

}

}

}

#endif // OCEAN_MEDIA_OIL_SUPPORT_JPG

#endif // META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_OL_IMAGE_JPG_H
