/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_UTILITIES_H
#define META_OCEAN_MEDIA_UTILITIES_H

#include "ocean/media/Media.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace Media
{

/**
 * This class implements utilities functions for the media library.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT Utilities
{
	public:

		/**
		 * Definition of a vector holding 8 bit elements.
		 */
		typedef std::vector<uint8_t> Buffer;

	public:

		/**
		 * Loads an image defined by the url and returns the frame.
		 * Beware: A media library supporting the image format must be loaded before.<br>
		 * @param url Url of the image to be loaded
		 * @return Resutling frame, the frame is Null if loading fails
		 * @see saveImage(), Manager, Library.
		 */
		static Frame loadImage(const std::string& url);

		/**
		 * Loads an image defined by a memory buffer, the size of the buffer and the type of the stored image information.
		 * Beware: A media library supporting the image format must be loaded before.<br>
		 * @param imageBuffer Buffer of the image to be loaded
		 * @param imageBufferSize Size of the image buffer in bytes
		 * @param imageBufferTypeIn Type of the given image that is stored in the buffer, should be specified if known (e.g. the file extension of a corresponding image file)
		 * @param imageBufferTypeOut Optional type of the given image that is stored in the buffer, as determined by the decoder (if possible)
		 * @return Resutling frame, the frame is Null if loading fails
		 * @see saveImage(), Manager, Library.
		 */
		static Frame loadImage(const void* imageBuffer, const size_t imageBufferSize, const std::string& imageBufferTypeIn = std::string(""), std::string* imageBufferTypeOut = nullptr);

		/**
		 * Saves a frame as image file.
		 * Beware: A media library supporting the image format must be loaded before.<br>
		 * @param frame The frame to be saved as image, must be valid
		 * @param url Url of the image to be saved
		 * @param addTimeSuffix If True the time and date is added to the url as suffix
		 * @return True, if succeeded
		 * @see loadImage(), Manager, Library.
		 */
		static bool saveImage(const Frame& frame, const std::string& url, const bool addTimeSuffix = true);

		/**
		 * Saves a frame into a memory buffer.
		 * Beware: A media library supporting the image format must be loaded before.<br>
		 * @param frame The frame to be saved as image
		 * @param imageType Type of the image to be created in the memory buffer e.g. the file extension of a corresponding image file (bmp, jpg, png, ...)
		 * @param buffer Resulting memory buffer holding the (compressed) image which will be extended so that already existing data stays untouched
		 * @return True, if succeeded
		 * @see loadImage(), Manager, Library.
		 */
		static bool saveImage(const Frame& frame, const std::string& imageType, Buffer& buffer);

		/**
		 * Encodes a frame to a buffer and adds further control parameters to the buffer.
		 * @param frame The frame to encode
		 * @param imageType Optional type of the image, e.g. bmp, jpg, png
		 * @param buffer The resulting buffer
		 */
		static void encodeFrame(const Frame& frame, const std::string& imageType, Buffer& buffer);

		/**
		 * Encodes a frame to a buffer and adds further control parameters to the buffer.
		 * @param frame The frame to encode
		 * @param imageType Optional type of the image, e.g. bmp, jpg, png
		 * @param reservedHeaderSize The number of bytes which will be reserved for an optional header, so that the resulting buffer has a header followed by the payload data
		 * @return The resulting buffer
		 */
		static inline Buffer encodeFrame(const Frame& frame, const std::string& imageType, const size_t reservedHeaderSize = 0);

		/**
		 * Decodes a frame from a maintenance data buffer.
		 * @param data The buffer providing the frame, will be shifted by the number of consumed bytes afterwards
		 * @param size The size of the entire buffer, will be reduced by the number of consumed bytes afterwards
		 * @param frame The resulting frame
		 * @return True, if succeeded
		 */
		static bool decodeFrame(const uint8_t*& data, size_t& size, Frame& frame);

		/**
		 * Decodes a frame from a data buffer holding the compressed image content as well as further control parameters.
		 * @param buffer The buffer from which the frame will be decoded
		 * @param frame The resulting frame
		 * @return True, if succeeded
		 */
		static inline bool decodeFrame(const Buffer& buffer, Frame& frame);
};

inline Utilities::Buffer Utilities::encodeFrame(const Frame& frame, const std::string& imageType, const size_t reservedHeaderSize)
{
	Buffer buffer(reservedHeaderSize);

	encodeFrame(frame, imageType, buffer);

	return buffer;
}

inline bool Utilities::decodeFrame(const Buffer& buffer, Frame& frame)
{
	const uint8_t* data = buffer.data();
	size_t size = buffer.size();

	return decodeFrame(data, size, frame);
}

}

}

#endif // META_OCEAN_MEDIA_UTILITIES_H
