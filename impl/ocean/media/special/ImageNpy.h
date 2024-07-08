/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_SPECIAL_IMAGE_NPY_H
#define META_OCEAN_MEDIA_SPECIAL_IMAGE_NPY_H

#include "ocean/media/special/Special.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace Media
{

namespace Special
{

/**
 * This class implements read and write functions for NPY (NumPy) images.
 * @ingroup mediaoil
 */
class OCEAN_MEDIA_SPECIAL_EXPORT ImageNpy
{
	public:

		/**
		 * Decodes a NPY image from a given binary buffer.
		 * @param buffer The buffer from which the image will be loaded, must be valid
		 * @param size The size of the given buffer in bytes, with range [1, infinity)
		 * @return The frame containing the image information, an invalid frame if the image could not be loaded
		 */
		static Frame decodeImage(const void* buffer, const size_t size);

		/**
		 * Encodes a given frame as NPY image to a resulting buffer.
		 * @param frame The frame to be written, must be valid
		 * @param buffer The resulting buffer storing the binary information of the NPY image
		 * @return True, if succeeded
		 */
		static bool encodeImage(const Frame& frame, std::vector<uint8_t>& buffer);

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
		 * Reads the header of a NPY file.
		 * @param data The data buffer, will be moved if the header could be read successfully, must be valid
		 * @param size The size of the buffer in bytes, will be reduced if the header could be read successfully, must be valid
		 * @param frameType The resulting frame type
		 * @param isRowMajor True, if the data is given in row-major order; False, if the data is given in column-major order
		 * @return True, if succeeded
		 */
		static bool readHeader(const uint8_t*& data, size_t& size, FrameType& frameType, bool& isRowMajor);

		/**
		 * Translates the pixel format.
		 * @param pixelFormat The pixel format to be translated given as string
		 * @return The resulting pixel format, FORMAT_UNDEFINED if unknown
		 */
		static FrameType::PixelFormat translatePixelFormat(const std::string& pixelFormat);

		/**
		 * Translates the pixel format.
		 * @param pixelFormat The pixel format to be translated
		 * @return The resulting pixel format as string, empty if unknown
		 */
		static std::string translatePixelFormat(const FrameType::PixelFormat pixelFormat);
};

inline bool ImageNpy::isPixelFormatSupported(const FrameType::PixelFormat pixelFormat)
{
	return !translatePixelFormat(pixelFormat).empty();
}

inline bool ImageNpy::isPixelOriginSupported(const FrameType::PixelOrigin pixelOrigin)
{
	return pixelOrigin == FrameType::ORIGIN_UPPER_LEFT || pixelOrigin == FrameType::ORIGIN_LOWER_LEFT;
}

inline bool ImageNpy::isFrameTypeSupported(const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin)
{
	return isPixelFormatSupported(pixelFormat) && isPixelOriginSupported(pixelOrigin);
}

}

}

}

#endif // META_OCEAN_MEDIA_SPECIAL_IMAGE_NPY_H
