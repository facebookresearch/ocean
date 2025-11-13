/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/system/OperatingSystem.h"

#if defined(OCEAN_PLATFORM_BUILD_APPLE)
	#include <sys/sysctl.h>
#endif

namespace Ocean
{

namespace System
{

OperatingSystem::OperatingSystemId OperatingSystem::id(std::string* version)
{
	if (version != nullptr)
	{
		version->clear();
	}

#if defined(OCEAN_PLATFORM_BUILD_WINDOWS)

	OSVERSIONINFOEXW osVersionInformationWorkstation = {sizeof(OSVERSIONINFOEXW), 0, 0, 0, 0, {0}, 0, 0, 0, VER_NT_WORKSTATION, 0};
	const bool isServer = !VerifyVersionInfoW(&osVersionInformationWorkstation, VER_PRODUCT_TYPE, VerSetConditionMask(0, VER_PRODUCT_TYPE, VER_EQUAL));

	OSVERSIONINFOEXW osVersionInformation = {sizeof(OSVERSIONINFOEXW), 0, 0, 0, 0, {0}, 0, 0, 0, 0, 0};
	const DWORDLONG conditionMask = VerSetConditionMask(VerSetConditionMask(VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL), VER_MINORVERSION, VER_GREATER_EQUAL), VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

	// Windows 10 or greater
	osVersionInformation.dwMajorVersion = HIBYTE(0x1000);
	osVersionInformation.dwMinorVersion = LOBYTE(0x1000);
	osVersionInformation.wServicePackMajor = 0;
	if (VerifyVersionInfoW(&osVersionInformation, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, conditionMask) == TRUE)
	{
		if (isServer)
		{
			return OSID_MICROSOFT_WINDOWS_SERVER_2016;
		}
		else
		{
			return OSID_MICROSOFT_WINDOWS_10;
		}
	}

	// Windows 8.1 or greater
	osVersionInformation.dwMajorVersion = HIBYTE(0x0603 /*_WIN32_WINNT_WINBLUE*/);
	osVersionInformation.dwMinorVersion = LOBYTE(0x0603 /*_WIN32_WINNT_WINBLUE*/);
	osVersionInformation.wServicePackMajor = 0;
	if (VerifyVersionInfoW(&osVersionInformation, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, conditionMask) == TRUE)
	{
		if (isServer)
		{
			return OSID_MICROSOFT_WINDOWS_SERVER_2012;
		}
		else
		{
			return OSID_MICROSOFT_WINDOWS_8_1;
		}
	}

	// Windows 8 or greater
	osVersionInformation.dwMajorVersion = HIBYTE(0x0602 /*_WIN32_WINNT_WIN8*/);
	osVersionInformation.dwMinorVersion = LOBYTE(0x0602 /*_WIN32_WINNT_WIN8*/);
	osVersionInformation.wServicePackMajor = 0;
	if (VerifyVersionInfoW(&osVersionInformation, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, conditionMask) == TRUE)
	{
		if (isServer)
		{
			return OSID_MICROSOFT_WINDOWS_SERVER_2012;
		}
		else
		{
			return OSID_MICROSOFT_WINDOWS_8;
		}
	}

	// Windows 7 or greater
	osVersionInformation.dwMajorVersion = HIBYTE(_WIN32_WINNT_WIN7);
	osVersionInformation.dwMinorVersion = LOBYTE(_WIN32_WINNT_WIN7);
	osVersionInformation.wServicePackMajor = 0;
	if (VerifyVersionInfoW(&osVersionInformation, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, conditionMask) == TRUE)
	{
		if (isServer)
		{
			return OSID_MICROSOFT_WINDOWS_SERVER_2008;
		}
		else
		{
			return OSID_MICROSOFT_WINDOWS_7;
		}
	}

	// Windows Vista or greater
	osVersionInformation.dwMajorVersion = HIBYTE(_WIN32_WINNT_VISTA);
	osVersionInformation.dwMinorVersion = LOBYTE(_WIN32_WINNT_VISTA);
	osVersionInformation.wServicePackMajor = 0;
	if (VerifyVersionInfoW(&osVersionInformation, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, conditionMask) == TRUE)
	{
		if (isServer)
		{
			return OSID_MICROSOFT_WINDOWS_SERVER_2008;
		}
		else
		{
			return OSID_MICROSOFT_WINDOWS_VISTA;
		}
	}

	// Windows XP or greater
	osVersionInformation.dwMajorVersion = HIBYTE(_WIN32_WINNT_WINXP);
	osVersionInformation.dwMinorVersion = LOBYTE(_WIN32_WINNT_WINXP);
	osVersionInformation.wServicePackMajor = 0;
	if (VerifyVersionInfoW(&osVersionInformation, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, conditionMask) == TRUE)
	{
		if (isServer)
		{
			return OSID_MICROSOFT_WINDOWS_SERVER_2003;
		}
		else
		{
			return OSID_MICROSOFT_WINDOWS_XP;
		}
	}

	// Windows 2000 or greater
	osVersionInformation.dwMajorVersion = HIBYTE(_WIN32_WINNT_WIN2K);
	osVersionInformation.dwMinorVersion = LOBYTE(_WIN32_WINNT_WIN2K);
	osVersionInformation.wServicePackMajor = 0;
	if (VerifyVersionInfoW(&osVersionInformation, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, conditionMask) == TRUE)
	{
		return OSID_MICROSOFT_WINDOWS_2000;
	}

	ocean_assert(false && "Unknown Windows Version!");
	return OSID_MICROSOFT_WINDOWS;

#elif defined(OCEAN_PLATFORM_BUILD_APPLE)

	// This works on both macOS and iOS (requires macOS 10.13.4+ / iOS 11+)
	char versionStr[256];
	size_t size = sizeof(versionStr);

	if (sysctlbyname("kern.osproductversion", versionStr, &size, nullptr, 0) == 0)
	{
		// versionStr contains the OS version like "15.1" (iOS) or "14.6.1" (macOS)
		if (version != nullptr)
		{
			*version = std::string(versionStr);
		}
	}

	#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
		return OSID_APPLE_IOS;
	#else
		return OSID_APPLE_MACOS;
	#endif

#elif defined(OCEAN_PLATFORM_BUILD_ANDROID)

	return OSID_ANDROID;

#elif defined(OCEAN_PLATFORM_BUILD_LINUX)

	return OSID_LINUX;

#else

	#error Missing implementation.

#endif
}

std::string OperatingSystem::name(const bool addVersion)
{
	std::string osVersion;
	const OperatingSystemId osid = id(&osVersion);

	std::string osString("Unknown OS");

	switch (osid)
	{
		case OSID_MICROSOFT_WINDOWS:
			osString = "Windows";
			break;

		case OSID_MICROSOFT_WINDOWS_CLIENT:
			osString = "Windows Client";
			break;

		case OSID_MICROSOFT_WINDOWS_SERVER:
			osString = "Windows Server";
			break;

		case OSID_MICROSOFT_WINDOWS_2000:
			osString = "Windows 2000";
			break;

		case OSID_MICROSOFT_WINDOWS_XP:
			osString = "Windows XP";
			break;

		case OSID_MICROSOFT_WINDOWS_VISTA:
			osString = "Windows Vista";
			break;

		case OSID_MICROSOFT_WINDOWS_7:
			osString = "Windows 7";
			break;

		case OSID_MICROSOFT_WINDOWS_8:
			osString = "Windows 8";
			break;

		case OSID_MICROSOFT_WINDOWS_8_1:
			osString = "Windows 8.1";
			break;

		case OSID_MICROSOFT_WINDOWS_10:
			osString = "Windows 10";
			break;

		case OSID_MICROSOFT_WINDOWS_SERVER_2003:
			osString = "Windows Server 2003";
			break;

		case OSID_MICROSOFT_WINDOWS_SERVER_2008:
			osString = "Windows Server 2008";
			break;

		case OSID_MICROSOFT_WINDOWS_SERVER_2012:
			osString = "Windows Server 2012";
			break;

		case OSID_MICROSOFT_WINDOWS_SERVER_2016:
			osString = "Windows Server 2016";
			break;

		case OSID_APPLE_MACOS:
			osString = "MacOS";
			break;

		case OSID_APPLE_IOS:
			osString = "iOS";
			break;

		case OSID_ANDROID:
			osString = "Android";
			break;

		case OSID_LINUX:
			osString = "Linux";
			break;

		case OSID_UNKNOWN:
		default:
			ocean_assert(false && "Invalid osid");
	}

	if (addVersion && !osVersion.empty())
	{
		osString += " " + osVersion;
	}

	return osString;
}

}

}
