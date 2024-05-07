// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/io/Signature.h"

#include <openssl/md5.h>
#include <openssl/sha.h>

#include <zlib.h>

#if defined(OCEAN_PLATFORM_BUILD_WINDOWS)
	#include <Softpub.h>
	#include <wincrypt.h>
	#include <wintrust.h>
#elif defined(__APPLE__) && TARGET_IPHONE_SIMULATOR != 1 && TARGET_OS_IPHONE != 1
	#include "ocean/base/Binary.h"
	#include "ocean/base/StringApple.h"

	#include <Foundation/Foundation.h>
	#include <Security/Security.h>
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
		innerOuterBuffer[n] = internalKey[n] ^ 0x36;
	for (size_t n = internalKeySize; n < 64; ++n)
		innerOuterBuffer[n] = 0x36;

	SHA1_Init(&context);
	SHA1_Update(&context, innerOuterBuffer, 64);
	SHA1_Update(&context, buffer, bufferSize);
	SHA1_Final(innerHash, &context);


	// outer hash

	for (size_t n = 0; n < internalKeySize; ++n)
		innerOuterBuffer[n] = internalKey[n] ^ 0x5C;
	for (size_t n = internalKeySize; n < 64; ++n)
		innerOuterBuffer[n] = 0x5C;

	SHA1_Init(&context);
	SHA1_Update(&context, innerOuterBuffer, 64);
	SHA1_Update(&context, innerHash, 20);
	SHA1_Final(hash, &context);
}

#endif

#if defined(_WINDOWS) || defined(__APPLE__) && TARGET_IPHONE_SIMULATOR != 1 && TARGET_OS_IPHONE != 1

bool Signature::evaluateCodeSignature(const std::wstring& filePath, bool& trustedCodeSignature, std::wstring* subjectName)
{
#ifdef _WINDOWS

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
		return false;

	return true;

#elif defined(__APPLE__) && TARGET_IPHONE_SIMULATOR != 1 && TARGET_OS_IPHONE != 1

	// we must differentiate between a package (.pkg) file and other files
	// package files are productsigned and other files are codesigned

	const std::wstring::size_type pos = filePath.find_last_of(L".");

	if (pos != std::wstring::npos && filePath.substr(pos + 1) == L"pkg")
	{
		// for packages we call the command line tool pkgutil

		std::string commandTool;
		HIDE_STRING_32("pkgutil --check-signature", commandTool, "\x53\x1E\xDA\xDA\xCA\x04\xAD\x43\x2C\xCC\xD2\xBF\x8B\xAD\x31\xB4\x62\xF9\x5D\x25\x46\xF8\xDB\x34\xB4", "\x42\xD9\x42\x88\x11\x89\x0F\x5F\xCC\xE7\xC4\xF4\x12\x03\x70\xDF\x88\x7E\x68\x0B\x93\xD8\x8E\x46\x39", 25 + 2);
		ocean_assert(commandTool == "pkgutil --check-signature");

		NSString* command = [NSString stringWithFormat:@"%@ \"%@\"", StringApple::toNSString(commandTool), StringApple::toNSString(filePath)];

		NSTask* task = [[NSTask alloc] init];
		NSPipe* pipe = [NSPipe pipe];

		std::string launchPath;
		HIDE_STRING_32("/bin/bash", launchPath, "\xF6\x59\x5E\xD9\xC3\x80\x26\xB4\x0F", "\x26\x37\xD4\x87\x2F\x8A\x33\x10\xDA", 9 + 2);
		ocean_assert(launchPath == "/bin/bash");

		[task setLaunchPath:StringApple::toNSString(launchPath)];
		[task setArguments:@[ @"-c", command ]];
		[task setStandardOutput:pipe];

		[task launch];
		[task waitUntilExit];

		trustedCodeSignature = [task terminationStatus] == 0;

		if (trustedCodeSignature && subjectName)
		{
			NSData* outputData = [[pipe fileHandleForReading] readDataToEndOfFile];

			if (!outputData)
				return false;

			std::wstring outputString = StringApple::toWString([[NSString alloc] initWithData:outputData encoding:NSUTF8StringEncoding]);

			// we search in the output string for the "Developer ID Installer"
			// for productsigned packages the subject identifier will always be "Developer ID Installer"
			std::string identifier;
			HIDE_STRING_32("Developer ID Installer: ", identifier, "\x69\x66\x39\xDB\x23\xD0\xAE\xEA\xD0\x29\x39\x5C\x6F\x75\x8D\x92\xEE\xD7\x28\x3A\xF5\x12\x66\x77", "\xB6\xB5\x8A\x8F\xD1\xEA\xB5\x5E\x0A\x6A\xED\x4C\x18\xE4\x68\x1A\xD7\x6C\x19\x96\x87\xEF\x24\x75", 24 + 2);
			ocean_assert(identifier == "Developer ID Installer: ");

			std::wstring::size_type pos = outputString.find(String::toWString(identifier));

			if (pos == std::wstring::npos)
				return false;

			outputString = outputString.substr(pos + 24);
			pos = outputString.find(L"\n");

			if (pos == std::wstring::npos)
				return false;

			*subjectName = outputString.substr(0, pos);
		}
	}
	else
	{
		NSURL* url = [NSURL fileURLWithPath: StringApple::toNSString(filePath)];

		SecStaticCodeRef staticCode = nullptr;
		if (SecStaticCodeCreateWithPath((__bridge CFURLRef)(url), kSecCSDefaultFlags, &staticCode) != errSecSuccess)
		{
			return false;
		}

		OSStatus status = SecStaticCodeCheckValidity(staticCode, kSecCSBasicValidateOnly, nullptr);
		trustedCodeSignature = status == errSecSuccess;

		if (trustedCodeSignature && subjectName)
		{
			CFDictionaryRef signingInformation = nullptr;

			// this function must only be called if the SecStaticCodeCheckValidity() is succeeded
			if (SecCodeCopySigningInformation(staticCode, kSecCSSigningInformation, &signingInformation) != errSecSuccess)
			{
				return false;
			}

			CFArrayRef certificates = (CFArrayRef)CFDictionaryGetValue(signingInformation, kSecCodeInfoCertificates);

			if (CFArrayGetCount(certificates) == 0)
			{
				return false;
			}

			// the first certificate will contains the subject we are looking for
			SecCertificateRef certificate = (SecCertificateRef)CFArrayGetValueAtIndex(certificates, 0);

			*subjectName = StringApple::toWString((__bridge NSString*)SecCertificateCopySubjectSummary(certificate));

			CFRelease(certificate);
			CFRelease(certificates);

			std::wstring::size_type pos = subjectName->find(L": ");

			if (pos != std::wstring::npos)
			{
				*subjectName = subjectName->substr(pos + 2);
			}
		}

		CFRelease(staticCode);
	}

	return true;

#endif
}

#endif

#ifdef _WINDOWS

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
