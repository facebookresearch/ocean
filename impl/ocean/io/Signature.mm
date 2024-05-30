/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/Signature.h"

#ifdef OCEAN_PLATFORM_BUILD_APPLE_MACOS

#include "ocean/base/Binary.h"
#include "ocean/base/StringApple.h"

#include <Foundation/Foundation.h>
#include <Security/Security.h>

namespace Ocean
{

namespace IO
{

bool Signature::evaluateCodeSignatureAppleMacos(const std::wstring& filePath, bool& trustedCodeSignature, std::wstring* subjectName)
{
	// we must differentiate between a package (.pkg) file and other files
	// package files are productsigned and other files are codesigned

	const std::wstring::size_type dotPos = filePath.find_last_of(L".");

	if (dotPos != std::wstring::npos && filePath.substr(dotPos + 1) == L"pkg")
	{
		// for packages we call the command line tool pkgutil

		std::wstring commandTool;
		HIDE_STRING_32(L"pkgutil --check-signature", commandTool, L"\x53\x1E\xDA\xDA\xCA\x04\xAD\x43\x2C\xCC\xD2\xBF\x8B\xAD\x31\xB4\x62\xF9\x5D\x25\x46\xF8\xDB\x34\xB4", L"\x42\xD9\x42\x88\x11\x89\x0F\x5F\xCC\xE7\xC4\xF4\x12\x03\x70\xDF\x88\x7E\x68\x0B\x93\xD8\x8E\x46\x39", 25 + 2);
		ocean_assert(commandTool == L"pkgutil --check-signature");

		NSString* command = [NSString stringWithFormat:@"%@ \"%@\"", StringApple::toNSString(commandTool), StringApple::toNSString(filePath)];

		NSTask* task = [[NSTask alloc] init];
		NSPipe* pipe = [NSPipe pipe];

		std::wstring launchPath;
		HIDE_STRING_32(L"/bin/bash", launchPath, L"\xF6\x59\x5E\xD9\xC3\x80\x26\xB4\x0F", L"\x26\x37\xD4\x87\x2F\x8A\x33\x10\xDA", 9 + 2);
		ocean_assert(launchPath == L"/bin/bash");

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
			{
				return false;
			}

			std::wstring outputString = StringApple::toWString([[NSString alloc] initWithData:outputData encoding:NSUTF8StringEncoding]);

			// we search in the output string for the "Developer ID Installer"
			// for productsigned packages the subject identifier will always be "Developer ID Installer"
			std::wstring identifier;
			HIDE_STRING_32(L"Developer ID Installer: ", identifier, L"\x69\x66\x39\xDB\x23\xD0\xAE\xEA\xD0\x29\x39\x5C\x6F\x75\x8D\x92\xEE\xD7\x28\x3A\xF5\x12\x66\x77", L"\xB6\xB5\x8A\x8F\xD1\xEA\xB5\x5E\x0A\x6A\xED\x4C\x18\xE4\x68\x1A\xD7\x6C\x19\x96\x87\xEF\x24\x75", 24 + 2);
			ocean_assert(identifier == L"Developer ID Installer: ");

			std::wstring::size_type pos = outputString.find(identifier);

			if (pos == std::wstring::npos)
			{
				return false;
			}

			outputString = outputString.substr(pos + 24);
			pos = outputString.find(L"\n");

			if (pos == std::wstring::npos)
			{
				return false;
			}

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
}

}

}

#endif //OCEAN_PLATFORM_BUILD_APPLE_MACOS
