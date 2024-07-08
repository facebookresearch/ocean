/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_SIGNATURE_H
#define META_OCEAN_IO_SIGNATURE_H

#include "ocean/io/IO.h"

namespace Ocean
{

namespace IO
{

/**
 * This class implements functions allowing to sign data or to provide hash values for data.
 * @ingroup io
 */
class OCEAN_IO_EXPORT Signature
{
	public:

		/**
		 * Determines the CRC32 hash for a specified data buffer.
		 * @param buffer The buffer for which the hash will be determined, must be valid
		 * @param bufferSize The size of the buffer in bytes, with range (0, 2^32 - 1)
		 * @return The resulting CRC32 hash value for the given buffer
		 */
		static unsigned int crc32(const unsigned char* buffer, const size_t bufferSize);

#if 0

		/**
		 * Determines the MD5 hash for a specified data buffer.
		 * @param buffer The buffer for which the hash will be determined, may be nullptr if bufferSize is 0
		 * @param bufferSize The size of the buffer in bytes, with range [0, infinity)
		 * @param hash The resulting MD5 hash value for the given buffer
		 */
		static void md5(const unsigned char* buffer, const size_t bufferSize, unsigned char hash[16]);

		/**
		 * Determines the SHA1 hash for a specified data buffer.
		 * @param buffer The buffer for which the hash will be determined, may be nullptr if bufferSize is 0
		 * @param bufferSize The size of the buffer in bytes, with range [0, infinity)
		 * @param hash The resulting SHA1 hash value for the given buffer
		 */
		static void sha1(const unsigned char* buffer, const size_t bufferSize, unsigned char hash[20]);

		/**
		 * Determines the keyed hash message authentication code using SHA1 for a specified data buffer and secrete key.
		 * @param buffer The buffer for which the hash will be determined, may be nullptr if bufferSize is 0
		 * @param bufferSize The size of the buffer in bytes, with range [0, infinity)
		 * @param key The key which will be used to determine the hash, may be nullptr if keySize is 0
		 * @param keySize The size of the key in bytes, with range [0, infinity)
		 * @param hash The resulting SHA1 hash value for the given buffer
		 */
		static void hmacSHA1(const unsigned char* buffer, const size_t bufferSize, const unsigned char* key, const size_t keySize, unsigned char hash[20]);

#endif

#if defined(OCEAN_PLATFORM_BUILD_APPLE_MACOS) || defined(OCEAN_PLATFORM_BUILD_WINDOWS)

		/**
		 * Evaluates if the code signature of the specified file exists and is trusted by the platform-specfic certificate store.
		 * On Apple platforms the subject name may contain a team identifier code (e.g., subject (ABCDEFG))
		 * @param filePath File path to signed code file
		 * @param trustedCodeSignature Receives true if the code signature of the file is valid and trusted; otherwise false is returned.
		 * @param subjectName Optional parameter that receives the subject name
		 * @return True if signature evaluation and subject name determination succeeded; otherwise, false is returned.
		 * @sa evaluateCodeSignatureAppleMacos(), evaluateCodeSignatureWindows()
		 */
		static inline bool evaluateCodeSignature(const std::wstring& filePath, bool& trustedCodeSignature, std::wstring* subjectName);

#endif

	private:

#ifdef OCEAN_PLATFORM_BUILD_APPLE_MACOS

		/**
		 * Evaluates if the code signature of the specified file exists and is trusted by the certificate store on macOS.
		 * The subject name may contain a team identifier code (e.g., subject (ABCDEFG))
		 * @param filePath File path to signed code file
		 * @param trustedCodeSignature Receives true if the code signature of the file is valid and trusted; otherwise false is returned.
		 * @param subjectName Optional parameter that receives the subject name
		 * @return Ture if signature evaluation and subject name determination succeeded; otherwise, false is returned.
		 * @sa evaluateCodeSignature()
		 */
		static bool evaluateCodeSignatureAppleMacos(const std::wstring& filePath, bool& trustedCodeSignature, std::wstring* subjectName);

#endif

#ifdef OCEAN_PLATFORM_BUILD_WINDOWS

		/**
		 * Evaluates if the code signature of the specified file exists and is trusted by the certificate store on Windows.
		 * On Apple platforms the subject name may contain a team identifier code (e.g., subject (ABCDEFG))
		 * @param filePath File path to signed code file
		 * @param trustedCodeSignature Receives true if the code signature of the file is valid and trusted; otherwise false is returned.
		 * @param subjectName Optional parameter that receives the subject name
		 * @return True if signature evaluation and subject name determination succeeded; otherwise, false is returned.
		 * @sa evaluateCodeSignatureAppleMacos()
		 */
		static bool evaluateCodeSignatureWindows(const std::wstring& filePath, bool& trustedCodeSignature, std::wstring* subjectName);

		/**
		 * Returns the subject name of the digital signature for the specified file.
		 * The method will fail if the specified file has no digital signature.
		 * @param filePath File path to signed code file
		 * @param subjectName Receives the subject name
		 * @return Ture if succeeded; otherwise, false is returned.
		 */
		static bool determineSignatureSubjectName(const std::wstring& filePath, std::wstring& subjectName);

#endif

};

#if defined(OCEAN_PLATFORM_BUILD_APPLE_MACOS) || defined(OCEAN_PLATFORM_BUILD_WINDOWS)

inline bool Signature::evaluateCodeSignature(const std::wstring& filePath, bool& trustedCodeSignature, std::wstring* subjectName)
{
#ifdef OCEAN_PLATFORM_BUILD_WINDOWS

	return Signature::evaluateCodeSignatureWindows(filePath, trustedCodeSignature, subjectName);

#elif defined(OCEAN_PLATFORM_BUILD_APPLE_MACOS)

	return Signature::evaluateCodeSignatureAppleMacos(filePath, trustedCodeSignature, subjectName);

#endif
}

#endif

}

}

#endif // META_OCEAN_IO_SIGNATURE_H
