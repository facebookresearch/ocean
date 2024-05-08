/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_SPECIAL_IMAGE_BMP_H
#define META_OCEAN_MEDIA_SPECIAL_IMAGE_BMP_H

#include "ocean/media/special/Special.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace Media
{

namespace Special
{

/**
 * This class implements read and write functions for BMP images.
 * @ingroup mediaspecial
 */
class OCEAN_MEDIA_SPECIAL_EXPORT ImageBmp
{
	protected:

		/**
		 * Definition of the Windows Bitmap file header.
		 */
		struct FileHeader;

		/**
		 * Definition of a Windows Bitmap data header.
		 */
		struct DataHeader;

	public:

		/**
		 * Decodes a BMP image from a given binary buffer.
		 * @param buffer The buffer from which the image will be loaded, must be valid
		 * @param size The size of the given buffer in bytes, with range [1, infinity)
		 * @return The frame containing the image information, an invalid frame if the image could not be loaded
		 */
		static Frame decodeImage(const void* buffer, const size_t size);

		/**
		 * Encodes a given frame as BMP image to a resulting buffer.
		 * @param frame The frame to be written, must be valid
		 * @param buffer The resulting buffer storing the binary information of the BMP image
		 * @param allowConversion True, to allow an internal conversion of the frame if BMP does not support the given frame type; False, to prevent a conversion and to stop creating the buffer
		 * @param hasBeenConverted Optional resulting statement whether the frame had to be converted to a different pixel format before it could be written; True, if so; False, if not
		 * @return True, if succeeded; False, if the frame could not be written as BMP image e.g., if the frame contained an alpha channel
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
};

inline bool ImageBmp::isPixelFormatSupported(const FrameType::PixelFormat pixelFormat)
{
	return pixelFormat == FrameType::FORMAT_BGR24;
}

inline bool ImageBmp::isPixelOriginSupported(const FrameType::PixelOrigin pixelOrigin)
{
	return pixelOrigin == FrameType::ORIGIN_UPPER_LEFT || pixelOrigin == FrameType::ORIGIN_LOWER_LEFT;
}

inline bool ImageBmp::isFrameTypeSupported(const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin)
{
	return isPixelFormatSupported(pixelFormat) && isPixelOriginSupported(pixelOrigin);
}

}

}

}

#endif // META_OCEAN_MEDIA_SPECIAL_IMAGE_BMP_H
