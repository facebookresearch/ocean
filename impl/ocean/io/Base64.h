/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_BASE_64_H
#define META_OCEAN_IO_BASE_64_H

#include "ocean/io/IO.h"

namespace Ocean
{

namespace IO
{

/**
 * This class implements function to encode binary information to text encoding and vice versa.
 */
class OCEAN_IO_EXPORT Base64
{
	public:

		/**
		 * Definition of a vector holding characters.
		 */
		typedef std::vector<uint8_t> Buffer;

	public:

		/**
		 * Encodes binary information by application of Base64 to a text encoding.
		 * @param buffer The buffer to encode
		 * @param bufferSize The size of the buffer to encode, in bytes, with range [1, infinity)
		 * @param encodedText The resulting encoded text
		 */
		static void encode(const uint8_t* buffer, const size_t bufferSize, Buffer& encodedText);

		/**
		 * Decodes a text encoding by application of an inverse Base64 to binary information.
		 * @param encodedText The encoded text to decode
		 * @param encodedTextSize The size of the text to decode, in characters, with range [4, infinity)
		 * @param buffer The resulting binary information
		 * @return True, if the given encoded text was valid
		 */
		static bool decode(const uint8_t* encodedText, const size_t encodedTextSize, Buffer& buffer);

		/**
		 * Encodes 3 bytes of binary information to 4 bytes with text encoding.
		 * @param bytes3 The three bytes to encode
		 * @param encoded4 The resulting four encoded bytes
		 */
		static inline void encode3(const uint8_t bytes3[3], uint8_t encoded4[4]);

		/**
		 * Decodes 4 bytes with text encoding to 3 bytes of binary information.
		 * @param encoded4 The four encoded byte to decode
		 * @param bytes3 The resulting three text encoded bytes
		 * @return True, if succeeded
		 */
		static inline bool decode4(const uint8_t encoded4[4], uint8_t bytes3[3]);

		/**
		 * Returns whether a given byte is text encoded.
		 * @param encodedValue The text encoded value to check
		 * @return The corresponding value with range [0, 59] if the value is text encoded, 64 otherwise
		 */
		static inline uint8_t isEncoded(const uint8_t encodedValue);

	protected:

		/// The possible encoded characters.
		static constexpr const char* encodedCharacters_ = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
};

inline void Base64::encode3(const uint8_t bytes3[3], uint8_t encoded4[4])
{
	ocean_assert(strlen(encodedCharacters_) == 64);

	uint8_t value = (bytes3[0] & 0xFCu) >> 2;
	ocean_assert(value < 64u);
	encoded4[0] = uint8_t(encodedCharacters_[value]);

	value = uint8_t(((bytes3[0] & 0x03u) << 4) + ((bytes3[1] & 0xF0u) >> 4));
	ocean_assert(value < 64u);
	encoded4[1] = uint8_t(encodedCharacters_[value]);

	value = uint8_t(((bytes3[1] & 0x0Fu) << 2) + ((bytes3[2] & 0xC0u) >> 6));
	ocean_assert(value < 64u);
	encoded4[2] = uint8_t(encodedCharacters_[value]);

	value = bytes3[2] & 0x3Fu;
	ocean_assert(value < 64u);
	encoded4[3] = uint8_t(encodedCharacters_[value]);
}

inline bool Base64::decode4(const uint8_t encoded4[4], uint8_t bytes3[3])
{
	const uint8_t value0 = isEncoded(encoded4[0]);
	const uint8_t value1 = isEncoded(encoded4[1]);
	const uint8_t value2 = isEncoded(encoded4[2]);
	const uint8_t value3 = isEncoded(encoded4[3]);

	if (value0 >= 64u || value1 >= 64u || value2 >= 64u || value3 >= 64u)
	{
		return false;
	}

	bytes3[0] = uint8_t((value0 << 2) | ((value1 & 0x30u) >> 4));
	bytes3[1] = uint8_t(((value1 & 0x0Fu) << 4) | ((value2 & 0x3Cu) >> 2));
	bytes3[2] = uint8_t(((value2 & 0x03u) << 6) | value3);

	return true;
}

inline unsigned char Base64::isEncoded(const uint8_t encodedValue)
{
	// 0                          26                          52          62 63
	// ABCDEFGHIJKLMNOPQRSTUVWXYZ  abcdefghijklmnopqrstuvwxyz  0123456789  +  /

	if (encodedValue >= 'A' && encodedValue <= 'Z')
	{
		return encodedValue - 'A';
	}
	else if (encodedValue >= 'a' && encodedValue <= 'z')
	{
		return encodedValue - 'a' + 26u;
	}
	else if (encodedValue >= '0' && encodedValue <= '9')
	{
		return encodedValue - '0' + 52u;
	}
	else if (encodedValue == '+')
	{
		return 62u;
	}
	else if (encodedValue == '/')
	{
		return 63u;
	}
	else if (encodedValue == 0)
	{
		return 0u; // special handling for end of string characters
	}

	return 64u;
}

}

}

#endif // META_OCEAN_IO_BASE_64_H
