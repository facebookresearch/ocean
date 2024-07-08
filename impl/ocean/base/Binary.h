/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_BINARY_H
#define META_OCEAN_BASE_BINARY_H

#include "ocean/base/Base.h"
#include "ocean/base/RandomI.h"

namespace Ocean
{

/**
 * This class implements functions for binaries like functions allowing to hide information in the binary code (in the release build).
 * Another possibility to hide a string in the binary code is the application of the HIDE_STRING_32 define.
 * @see HIDE_STRING_32.
 * @ingroup base
 */
class Binary
{
	public:

		/**
		 * Decrypts a given string by a simple (not very secure) encrypt/decrypt method.
		 * @param encryptedValue The string to decrypt
		 * @param size The length of the encryptedValue value, with range [1, infinity)
		 * @param key0 The first key to be used for the decryption, with same length as value
		 * @param key1 The second key to be used for the decryption, with same length as value
		 * @return The decrypted value, if succeeded
		 */
		static inline std::wstring decrypt(const wchar_t* encryptedValue, const size_t size, const wchar_t* key0, const wchar_t* key1);

		/**
		 * Encrypts a given string by a simple (not very secure)  encrypt/decrypt method.
		 * @param decryptedValue The string to encrypt
		 * @param size The length of the encrypted value, with range [1, infinity)
		 * @param key0 Resulting first key necessary for the decryption, with same length as value
		 * @param key1 Resulting second key necessary for the decryption, with same length as value
		 */
		static inline std::wstring encrypt(const wchar_t* decryptedValue, const size_t size, std::wstring& key0, std::wstring& key1);
};

inline std::wstring Binary::decrypt(const wchar_t* encryptedValue, const size_t size, const wchar_t* key0, const wchar_t* key1)
{
	ocean_assert(size != 0);

	std::wstring result;
	result.resize(size);

	for (size_t n = 0; n < size; ++n)
	{
		result[n] = wchar_t((encryptedValue[n] - key1[n]) ^ key0[n]);
	}

	return result;
}

inline std::wstring Binary::encrypt(const wchar_t* decryptedValue, const size_t size, std::wstring& key0, std::wstring& key1)
{
	ocean_assert(size != 0);

	key0.resize(size);
	key1.resize(size);

	std::wstring result;
	result.resize(size);

	for (size_t n = 0; n < size; ++n)
	{
		wchar_t value = 0;

		while (value == 0)
		{
			key0[n] = wchar_t(RandomI::random(1u, 255u));
			key1[n] = wchar_t(RandomI::random(1u, 255u));

			value = wchar_t((decryptedValue[n] ^ key0[n]) + key1[n]);
		}

		result[n] = value;
	}

	return result;
}

// This define is part of the HIDE_STRING_32 define used internally.
// @see HIDE_STRING_32.
// @ingroup base
#define ENCRYPT_STRING_INTERN_32(value, key1, key2) wchar_t(((value)[0] ^ key1[0]) + key2[0]), wchar_t(((value)[1] ^ key1[1]) + key2[1]), wchar_t(((value)[2] ^ key1[2]) + key2[2]), \
	wchar_t(((value)[3] ^ key1[3]) + key2[3]), wchar_t(((value)[4] ^ key1[4]) + key2[4]), wchar_t(((value)[5] ^ key1[5]) + key2[5]), \
	wchar_t(((value)[6] ^ key1[6]) + key2[6]), wchar_t(((value)[7] ^ key1[7]) + key2[7]), wchar_t(((value)[8] ^ key1[8]) + key2[8]), \
	wchar_t(((value)[9] ^ key1[9]) + key2[9]), wchar_t(((value)[10] ^ key1[10]) + key2[10]), wchar_t(((value)[11] ^ key1[11]) + key2[11]), \
	wchar_t(((value)[12] ^ key1[12]) + key2[12]), wchar_t(((value)[13] ^ key1[13]) + key2[13]), wchar_t(((value)[14] ^ key1[14]) + key2[14]), \
	wchar_t(((value)[15] ^ key1[15]) + key2[15]), wchar_t(((value)[16] ^ key1[16]) + key2[16]), wchar_t(((value)[17] ^ key1[17]) + key2[17]), \
	wchar_t(((value)[18] ^ key1[18]) + key2[18]), wchar_t(((value)[19] ^ key1[19]) + key2[19]), wchar_t(((value)[20] ^ key1[20]) + key2[20]), \
	wchar_t(((value)[21] ^ key1[21]) + key2[21]), wchar_t(((value)[22] ^ key1[22]) + key2[22]), wchar_t(((value)[23] ^ key1[23]) + key2[23]), \
	wchar_t(((value)[24] ^ key1[24]) + key2[24]), wchar_t(((value)[25] ^ key1[25]) + key2[25]), wchar_t(((value)[26] ^ key1[26]) + key2[26]), \
	wchar_t(((value)[27] ^ key1[27]) + key2[27]), wchar_t(((value)[28] ^ key1[28]) + key2[28]), wchar_t(((value)[29] ^ key1[29]) + key2[29]), \
	wchar_t(((value)[30] ^ key1[30]) + key2[30]), wchar_t(((value)[31] ^ key1[31]) + key2[31]), '\0'

// This define is part of the HIDE_STRING_32 define used internally.
// @see HIDE_STRING_32.
// @ingroup base
#define ENCRYPT_STRING_32(value, key1, key2) { ENCRYPT_STRING_INTERN_32(value L"\x91_#\x12\x62\0\x54tw\x44\0p==&\0\xA1\xB2O\xB3\x53\x12\x93\xF2\x31\xA5\x55x#\x63\x71\0", key1 L"\x85P>?\0\0z\0\0w!xX+$\x57uE\0b\x31\x77\x65qnn\x81\x51\xA4\x3F\0\x77", key2 L"\xD1g\0\x83~\0qwCaT//{qq7\x68\x2B\xC3\xD1#vN\xAA\xE1\xC3)C\x78\x43p") }

/**
 * This function is part of the HIDE_STRING_32 define used internally.
 * @see HIDE_STRING_32.
 * @ingroup base
 */
inline std::wstring decryptString32(const wchar_t* value, const unsigned int sizePlusTwo, const wchar_t* key1, const wchar_t* key2)
{
	ocean_assert(sizePlusTwo >= 2u && sizePlusTwo <= 34);
	std::wstring result(sizePlusTwo - 2, ' ');

	for (unsigned int n = 0u; n < sizePlusTwo - 2; ++n)
	{
		result[n] = (value[n] - key2[n]) ^ key1[n];
	}

	return result;
}

// This define allows to hide a string in the final binary file (in the release build).
// The define can be used for strings with length up to 32 characters.<br>
// Larger strings must either be separated into smaller sub-strings or must be hided by application of Binary::decrypt().<br>
// To hide a string you need random characters within the value range [1, 255].
// @code
// // the following instruction hides the string "test"
//
// // we need a standard string value as target
// std::string value;
//
// // we provided the actual string to hide, the target value, a set of random character with the same amount of characters as the length of the string to hide, a second set of random characters, and finally the length of the string to hide plus two
// HIDE_STRING_32("test", value, "\x3C\x9F\x63\x44", "\x25\xEA\x48\x7B", 4 + 2);
//
// // now the value holds the string "test"
// ocean_assert(value == "test");
// @endcode
// @see Binary.
// @ingroup base
#define HIDE_STRING_32(value, result, key1, key2, sizePlusTwo) do { ocean_assert(wcslen(value) + 2 == sizePlusTwo); const wchar_t encryptedValue[] = ENCRYPT_STRING_32(value, key1, key2); result = Ocean::decryptString32(encryptedValue, sizePlusTwo, key1, key2); ocean_assert(result == value); } while (false)

}

#endif // META_OCEAN_BASE_BINARY_H
