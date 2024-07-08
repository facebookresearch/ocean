/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_SPECIAL_IMAGE_OCN_H
#define META_OCEAN_MEDIA_SPECIAL_IMAGE_OCN_H

#include "ocean/media/special/Special.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace Media
{

namespace Special
{

/**
 * This class implements read and write functions for Ocean frames.
 * @ingroup mediaspecial
 */
class OCEAN_MEDIA_SPECIAL_EXPORT ImageOcn
{
	protected:

		/// The size of the magic number header part in bytes.
		static constexpr size_t headerMagicNumberSize_ = 4;

		/// The size of the pixel format header part in bytes.
		static constexpr size_t headerPixelFormatSize_ = 40;

		/// The size of the pixel origin header part in bytes.
		static constexpr size_t headerPixelOriginSize_ = 16;

		/// The size of the data type header part in bytes.
		static constexpr size_t headerDataTypeSize_ = 32;

	public:

		/**
		 * Decodes a OCN image from a given binary buffer.
		 * @param buffer The buffer from which the image will be loaded, must be valid
		 * @param size The size of the given buffer in bytes, with range [1, infinity)
		 * @return The frame containing the image information, an invalid frame if the image could not be loaded
		 */
		static Frame decodeImage(const void* buffer, const size_t size);

		/**
		 * Encodes a given frame as OCN image to a resulting buffer.
		 * @param frame The frame to be written, must be valid
		 * @param buffer The resulting buffer storing the binary information of the OCN image
		 * @return True, if succeeded
		 */
		static bool encodeImage(const Frame& frame, std::vector<uint8_t>& buffer);

	protected:

		/**
		 * Reads a value from memory.
		 * @param data The memory from which the value will be read, will be moved, must be valid
		 * @param remainingSize The remaining size of the memory, in bytes, will be adjusted, with range [sizeof(T), infinity)
		 * @param value The resulting value
		 * @return True, if succeeded
		 * @tparam T The data type of the value
		 */
		template <typename T>
		static bool readValue(const uint8_t*& data, size_t& remainingSize, T& value);

		/**
		 * Reads a string from memory.
		 * @param data The memory from which the string will be read, will be moved, must be valid
		 * @param remainingSize The remaining size of the memory, in bytes, will be adjusted, with range [numberCharacters, infinity)
		 * @param numberCharacters The maximal size of the string to read, in bytes, with range [1, remainingSize]
		 * @param value The resulting string
		 * @return True, if succeeded
		 */
		static bool readValue(const uint8_t*& data, size_t& remainingSize, const size_t numberCharacters, std::string& value);

		/**
		 * Writes a value to memory.
		 * @param value The value to write
		 * @param data The memory receiving the value, will be moved, must be valid
		 * @param remainingSize The remaining size of the memory, in bytes, will be adjusted, with range [sizeof(T), infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the value
		 */
		template <typename T>
		static bool writeValue(const T& value, uint8_t*& data, size_t& remainingSize);

		/**
		 * Writes a string to memory and fills remaining memory with zero values.
		 * @param value The string to write
		 * @param numberCharacters The number of characters that will be written, with range [value.size(), infinity)
		 * @param data The memory receiving the value, will be moved, must be valid
		 * @param remainingSize The remaining size of the memory, in bytes, will be adjusted, with range [numberCharacters, infinity)
		 * @return True, if succeeded
		 */
		static bool writeValue(const std::string& value, const size_t numberCharacters, uint8_t*& data, size_t& remainingSize);
};

}

}

}

#endif // META_OCEAN_MEDIA_SPECIAL_IMAGE_OCN_H
