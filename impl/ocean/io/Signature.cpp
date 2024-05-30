/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/Signature.h"

#include <openssl/md5.h>
#include <openssl/sha.h>

#include <zlib.h>

#if defined(OCEAN_PLATFORM_BUILD_WINDOWS)
	#include <Softpub.h>
	#include <wincrypt.h>
	#include <wintrust.h>
#endif

namespace Ocean
{

namespace IO
{

unsigned int Signature::crc32(const unsigned char* buffer, const size_t bufferSize)
{
	return (unsigned int)::crc32(0u, buffer, (unsigned int)bufferSize);
}

#if 0

void Signature::md5(const unsigned char* buffer, const size_t bufferSize, unsigned char hash[16])
{
	MD5_CTX context;

	MD5_Init(&context);
	MD5_Update(&context, buffer, bufferSize);
	MD5_Final(hash, &context);
}

void Signature::sha1(const unsigned char* buffer, const size_t bufferSize, unsigned char hash[20])
{
	SHA_CTX context;

	SHA1_Init(&context);
	SHA1_Update(&context, buffer, bufferSize);
	SHA1_Final(hash, &context);
}

void Signature::hmacSHA1(const unsigned char* buffer, const size_t bufferSize, const unsigned char* key, const size_t keySize, unsigned char hash[20])
{
	// sha hash size: 20
	// sha block size: 64

	unsigned char fallbackKey[20];

	const unsigned char* internalKey = key;
	size_t internalKeySize = keySize;

	SHA_CTX context;

	if (keySize > 64)
	{
		SHA1_Init(&context);
		SHA1_Update(&context, key, keySize);
		SHA1_Final(fallbackKey, &context);

		internalKey = fallbackKey;
		internalKeySize = 20;
	}

	unsigned char innerOuterBuffer[64];

	// inner hash

	unsigned char innerHash[20];

	for (size_t n = 0; n < internalKeySize; ++n)
	{
		innerOuterBuffer[n] = internalKey[n] ^ 0x36;
	}
	for (size_t n = internalKeySize; n < 64; ++n)
	{
		innerOuterBuffer[n] = 0x36;
	}

	SHA1_Init(&context);
	SHA1_Update(&context, innerOuterBuffer, 64);
	SHA1_Update(&context, buffer, bufferSize);
	SHA1_Final(innerHash, &context);


	// outer hash

	for (size_t n = 0; n < internalKeySize; ++n)
	{
		innerOuterBuffer[n] = internalKey[n] ^ 0x5C;
	}
	for (size_t n = internalKeySize; n < 64; ++n)
	{
		innerOuterBuffer[n] = 0x5C;
	}

	SHA1_Init(&context);
	SHA1_Update(&context, innerOuterBuffer, 64);
	SHA1_Update(&context, innerHash, 20);
	SHA1_Final(hash, &context);
}

#endif

#if defined(OCEAN_PLATFORM_BUILD_WINDOWS)

bool Signature::evaluateCodeSignatureWindows(const std::wstring& filePath, bool& trustedCodeSignature, std::wstring* subjectName)
{
	WINTRUST_FILE_INFO fileInfo;
	memset(&fileInfo, 0, sizeof(fileInfo));

	fileInfo.cbStruct = sizeof(WINTRUST_FILE_INFO);
	fileInfo.pcwszFilePath = filePath.c_str();
	fileInfo.hFile = nullptr;
	fileInfo.pgKnownSubject = nullptr;

	WINTRUST_DATA winTrustData;
	memset(&winTrustData, 0, sizeof(winTrustData));

	winTrustData.cbStruct = sizeof(winTrustData);
	winTrustData.pPolicyCallbackData = nullptr;
	winTrustData.pSIPClientData = nullptr;
	winTrustData.dwUIChoice = WTD_UI_NONE;
	winTrustData.fdwRevocationChecks = WTD_REVOKE_NONE;
	winTrustData.dwUnionChoice = WTD_CHOICE_FILE;
	winTrustData.dwStateAction = WTD_STATEACTION_VERIFY;
	winTrustData.hWVTStateData = nullptr;
	winTrustData.pwszURLReference = nullptr;
	winTrustData.dwUIContext = 0u;
	winTrustData.pFile = &fileInfo;

	// Identifies the Microsoft Authenticode Policy Provider:
	GUID policyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;

	const LONG status = WinVerifyTrust(nullptr, &policyGUID, &winTrustData);

	// Only files with explicitly trusted signatures are considered as trusted:
	trustedCodeSignature = status == ERROR_SUCCESS;

	winTrustData.dwStateAction = WTD_STATEACTION_CLOSE;
	WinVerifyTrust(nullptr, &policyGUID, &winTrustData);

	if (trustedCodeSignature && subjectName && !determineSignatureSubjectName(filePath, *subjectName))
	{
		return false;
	}

	return true;

}

bool Signature::determineSignatureSubjectName(const std::wstring& filePath, std::wstring& subjectName)
{
	bool success = false;

	HCERTSTORE storeHandle = nullptr;
	HCRYPTMSG messageHandle = nullptr;

	if (CryptQueryObject(CERT_QUERY_OBJECT_FILE, filePath.c_str(), CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED, CERT_QUERY_FORMAT_FLAG_BINARY, 0u, nullptr, nullptr, nullptr, &storeHandle, &messageHandle, nullptr) != TRUE)
	{
		return false;
	}

	ocean_assert(storeHandle && messageHandle);

	// Get signer information:
	DWORD signerInfoSize = 0u;
	if (CryptMsgGetParam(messageHandle, CMSG_SIGNER_INFO_PARAM, 0u, nullptr, &signerInfoSize) == TRUE)
	{
		PCMSG_SIGNER_INFO signerInfo = PCMSG_SIGNER_INFO(LocalAlloc(LPTR, signerInfoSize));
		ocean_assert(signerInfo);

		if (CryptMsgGetParam(messageHandle, CMSG_SIGNER_INFO_PARAM, 0u, signerInfo, &signerInfoSize) == TRUE)
		{
			CERT_INFO certInfo;
			memset(&certInfo, 0x00, sizeof(certInfo));
			certInfo.Issuer = signerInfo->Issuer;
			certInfo.SerialNumber = signerInfo->SerialNumber;

			const PCCERT_CONTEXT certificateContext = CertFindCertificateInStore(storeHandle, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0u, CERT_FIND_SUBJECT_CERT, (PVOID)&certInfo, nullptr);
			if (certificateContext)
			{
				// Get subject name:
				const DWORD length = CertGetNameStringW(certificateContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0u, nullptr, nullptr, 0u);
				if (length > 0u)
				{
					std::vector<std::wstring::value_type> nameBuffer(length);
					success = CertGetNameStringW(certificateContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0u, nullptr, nameBuffer.data(), length) != 0u;

					if (success)
					{
						subjectName = std::wstring(nameBuffer.begin(), nameBuffer.end() - 1u);
					}
				}

				CertFreeCertificateContext(certificateContext);
			}
		}

		LocalFree(signerInfo);
	}

	if (storeHandle != nullptr)
	{
		CertCloseStore(storeHandle, 0);
	}

	if (messageHandle != nullptr)
	{
		CryptMsgClose(messageHandle);
	}

	return success;
}

#endif

}

}
