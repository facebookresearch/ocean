/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/system/OperatingSystem.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace System
{

#ifndef __APPLE__ // @see OperationSystemApple.mm for Apple platforms

OperatingSystem::OperatingSystemId OperatingSystem::id(std::wstring* version)
{
	if (version != nullptr)
	{
		version->clear();
	}

#if defined(_WINDOWS)

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

#elif defined(__APPLE__)

    static_assert(false, "The implementation is located in OperationSystemApple.mm for Apple platforms");

#elif defined(_ANDROID)

	return OSID_ANDROID;

#elif defined(__linux__) || defined(__EMSCRIPTEN__)

	return OSID_LINUX;

#else
	#error Missing implementation.
#endif
}

#endif // __APPLE__

std::wstring OperatingSystem::name(const bool addVersion)
{
	std::wstring osVersion;
	const OperatingSystemId osid = id(&osVersion);

	std::wstring osString(L"Unknown OS");

	switch (osid)
	{
		case OSID_MICROSOFT_WINDOWS:
			osString = L"Windows";
			break;

		case OSID_MICROSOFT_WINDOWS_CLIENT:
			osString = L"Windows Client";
			break;

		case OSID_MICROSOFT_WINDOWS_SERVER:
			osString = L"Windows Server";
			break;


		case OSID_MICROSOFT_WINDOWS_2000:
			osString = L"Windows 2000";
			break;

		case OSID_MICROSOFT_WINDOWS_XP:
			osString = L"Windows XP";
			break;

		case OSID_MICROSOFT_WINDOWS_VISTA:
			osString = L"Windows Vista";
			break;

		case OSID_MICROSOFT_WINDOWS_7:
			osString = L"Windows 7";
			break;

		case OSID_MICROSOFT_WINDOWS_8:
			osString = L"Windows 8";
			break;

		case OSID_MICROSOFT_WINDOWS_8_1:
			osString = L"Windows 8.1";
			break;

		case OSID_MICROSOFT_WINDOWS_10:
			osString = L"Windows 10";
			break;


		case OSID_MICROSOFT_WINDOWS_SERVER_2003:
			osString = L"Windows Server 2003";
			break;

		case OSID_MICROSOFT_WINDOWS_SERVER_2008:
			osString = L"Windows Server 2008";
			break;

		case OSID_MICROSOFT_WINDOWS_SERVER_2012:
			osString = L"Windows Server 2012";
			break;

		case OSID_MICROSOFT_WINDOWS_SERVER_2016:
			osString = L"Windows Server 2016";
			break;


		case OSID_MACINTOSH_OS:
			osString = L"Mac OS";
			break;

		case OSID_MACINTOSH_OS_X:
			osString = L"Mac OS X";
			break;

		case OSID_ANDROID:
			osString = L"Android";
			break;

		case OSID_LINUX:
			osString = L"Linux";
			break;

		default:
			ocean_assert(false && "Invalid osid");
	}

	if (addVersion)
	{
		osString += std::wstring(1, L' ') + osVersion;
	}

	return osString;
}

}

}
