/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_SPECIAL_IMAGE_PFM_H
#define META_OCEAN_MEDIA_SPECIAL_IMAGE_PFM_H

#include "ocean/media/special/Special.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace Media
{

namespace Special
{

/**
 * This class implements read and write functions for PFM images.
 * @ingroup mediaspecial
 */
class OCEAN_MEDIA_SPECIAL_EXPORT ImagePfm
{
	public:

		/**
		 * Decodes a PFM image from a given binary buffer.
		 * @param buffer The buffer from which the image will be loaded, must be valid
		 * @param size The size of the given buffer in bytes, with range [1, infinity)
		 * @return The frame containing the image information, an invalid frame if the image could not be loaded
		 */
		static Frame decodeImage(const void* buffer, const size_t size);

		/**
		 * Encodes a given frame as PFM image to a resulting buffer.
		 * @param frame The frame to be written, must be valid
		 * @param buffer The resulting buffer storing the binary information of the PFM image
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
		 * Reads the header of a PFM file.
		 * @param data The data buffer, will be moved if the header could be read successfully, must be valid
		 * @param size The size of the buffer in bytes, will be reduced if the header could be read successfully, must be valid
		 * @param frameType The resulting frame type
		 * @param isLittleEndian True, if the buffer provides the data with little endian encoding; False, if the buffer provides the data with big endian encoding
		 * @return True, if succeeded
		 */
		static bool readHeader(const uint8_t*& data, size_t& size, FrameType& frameType, bool& isLittleEndian);

		/**
		 * Reads one number value from a given data buffer.
		 * The value ends with a space or a new line.
		 * @param data The data buffer, will be moved if the value could be read successfully, must be valid
		 * @param size The size of the buffer in bytes, will be reduced if the value could be read successfully, must be valid
		 * @param value The resulting value
		 * @return True, if succeeded
		 */
		static bool readValue(const uint8_t*& data, size_t& size, std::string& value);
};

inline bool ImagePfm::isPixelFormatSupported(const FrameType::PixelFormat pixelFormat)
{
	return FrameType::arePixelFormatsCompatible(pixelFormat, FrameType::genericPixelFormat<float, 1u>())
			|| FrameType::arePixelFormatsCompatible(pixelFormat, FrameType::genericPixelFormat<float, 3u>());
}

inline bool ImagePfm::isPixelOriginSupported(const FrameType::PixelOrigin pixelOrigin)
{
	return pixelOrigin == FrameType::ORIGIN_LOWER_LEFT || pixelOrigin == FrameType::ORIGIN_UPPER_LEFT;
}

inline bool ImagePfm::isFrameTypeSupported(const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin)
{
	return isPixelFormatSupported(pixelFormat) && isPixelOriginSupported(pixelOrigin);
}

}

}

}

#endif // META_OCEAN_MEDIA_SPECIAL_IMAGE_PFM_H
