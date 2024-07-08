/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef __APPLE__
    #error This source file must be used on Apple platforms only!
#endif

#include "ocean/system/OperatingSystem.h"

#include "ocean/base/String.h"

#include <Foundation/Foundation.h>

namespace Ocean
{

namespace System
{

OperatingSystem::OperatingSystemId OperatingSystem::id(std::wstring* version)
{
	if (version != nullptr)
	{
		version->clear();
	}

	NSProcessInfo* processInformation = [NSProcessInfo processInfo];
	NSOperatingSystemVersion macOSVersion = [processInformation operatingSystemVersion];

	if (macOSVersion.majorVersion >= 10)
	{
		if (version != nullptr)
		{
			*version = String::toWString((long long)macOSVersion.majorVersion) + std::wstring(L".") + String::toWString((long long)macOSVersion.minorVersion) + std::wstring(L".") + String::toWString((long long)macOSVersion.patchVersion);
		}

		return OSID_MACINTOSH_OS_X;
	}

	ocean_assert(false && "Unknown Mac OS Version!");
	return OSID_MACINTOSH_OS;
}

}

}
