/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/Build.h"
#include "ocean/base/String.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

namespace Ocean
{

std::string Build::platformType()
{

#if defined(OCEAN_PLATFORM_BUILD_WINDOWS)

	return std::string("win");

#elif defined(OCEAN_PLATFORM_BUILD_ANDROID)

	return std::string("android");

#elif defined(OCEAN_PLATFORM_BUILD_APPLE_IOS) || defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_SUMULATOR)

	return std::string("ios");

#elif defined(OCEAN_PLATFORM_BUILD_APPLE_MACOS)

	return std::string("osx");

#elif defined(OCEAN_PLATFORM_BUILD_LINUX)

	return std::string("linux");

#else

	#error Missing implementation

#endif

}

std::string Build::architectureType()
{

#if defined(_WINDOWS)

	#if defined(_WIN64)
		return std::string("x64");
	#elif defined(_WIN32)
		return std::string("x86");
	#else
		#error Missing implementation
	#endif

#elif defined(_ANDROID)

	#if defined(__ARM_ARCH_7__)
		return std::string("arm");
	#elif defined(__aarch64__)
		return std::string("arm64");
	#elif defined(__ARM_ARCH) && __ARM_ARCH == 7
		return std::string("arm");
	#elif defined(__x86_64__)
		return std::string("x64");
	#elif defined(__i386__)
		return std::string("i386");
	#else
		#error Missing implementation
	#endif

#elif defined(__APPLE__)

	#if defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR == 1
		return std::string("i386");
	#elif defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1
		return std::string("arm64");
	#elif defined(TARGET_OS_MAC) && TARGET_OS_MAC == 1
		return std::string("x64");
	#else
		#error Missing implmentation
	#endif
#elif defined(__EMSCRIPTEN__)
	return std::string("emscripten");
#elif defined(__linux__)

	#if defined(__x86_64__)
		return std::string("x64");
	#elif defined(__i386__)
		return std::string("x86");
	#elif defined(__ARM_ARCH_7__)
		return std::string("arm");
	#elif defined(__aarch64__)
		return std::string("arm64");
	#elif defined(__ARM_ARCH) && __ARM_ARCH == 7
		return std::string("arm");
	#else
		#error Missing implementation
	#endif

#else

	#error Missing implementation

#endif

}

std::string Build::releaseType()
{

#ifdef OCEAN_DEBUG
	return std::string("debug");
#else
	return std::string();
#endif

}

std::string Build::runtimeType()
{

#ifdef OCEAN_RUNTIME_SHARED
	return std::string("shared");
#else
	return std::string("static");
#endif

}

std::string Build::compilerVersion()
{

#if defined(OCEAN_COMPILER_MSC)

	#if OCEAN_MSC_VERSION == 1400
		// Visual Studio 2005
		return std::string("vc08");
	#elif OCEAN_MSC_VERSION == 1500
		// Visual Studio 2008
		return std::string("vc09");
	#elif OCEAN_MSC_VERSION == 1600
		// Visual Studio 2010
		return std::string("vc10");
	#elif OCEAN_MSC_VERSION == 1700
		// Visual Studio 2012
		return std::string("vc11");
	#elif OCEAN_MSC_VERSION == 1800
		// Visual Studio 2013
		return std::string("vc12");
	#elif OCEAN_MSC_VERSION == 1900
		// Visual Studio 2015
		return std::string("vc14");
	#elif OCEAN_MSC_VERSION >= 1910 && OCEAN_MSC_VERSION < 1920
		// Visual Studio 2017
		return std::string("vc141");
	#elif OCEAN_MSC_VERSION >= 1920 && OCEAN_MSC_VERSION < 1930
		// Visual Studio 2019
		return std::string("vc142");
	#elif OCEAN_MSC_VERSION >= 1930 && OCEAN_MSC_VERSION < 1940
		return std::string("vc143");
	#else
		#error Missing implementation
	#endif

#elif defined(OCEAN_COMPILER_CLANG)

	#if defined(__APPLE__) && defined(__apple_build_version__)

		// Xcode is used to compile the code, so we have the prefix 'xc'
		#if defined(__clang_major__)
			return std::string("xc") + String::toAString(__clang_major__);
		#else
			#error Unknown compiler, expected Clang
		#endif

	#else

		// the code is compiled without Xcode, so we have the prefix 'cl'
		#if defined(__clang_major__)
			return std::string("cl") + String::toAString(__clang_major__);
		#else
			#error Unknown compiler, expected Clang
		#endif

	#endif // __apple_build_version__

#elif defined(__GNUC__)

	#if defined(__clang_major__)
		return std::string("cl") + String::toAString(__clang_major__) + String::toAString(__clang_minor__);
	#else
		return std::string("gc") + String::toAString(__GNUC__) + String::toAString(__GNUC_MINOR__);
	#endif

#else

	#error Unknown compiler

#endif

}

std::string Build::buildString()
{
	std::string result(platformType());

	if (architectureType().empty() == false)
	{
		result += std::string("_") + architectureType();
	}

	if (compilerVersion().empty() == false)
	{
		result += std::string("_") + compilerVersion();
	}

	if (runtimeType().empty() == false)
	{
		result += std::string("_") + runtimeType();
	}

	if (releaseType().empty() == false)
	{
		result += std::string("_") + releaseType();
	}

	return result;
}

std::string Build::buildDate(const char* d)
{
	std::string dateStr(d);

	ocean_assert(dateStr.length() == 11);

	if (dateStr.length() != 11)
	{
		return std::string();
	}

	const std::string months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

	std::string monthStr = dateStr.substr(0, 3);
	std::string dayStr = dateStr.substr(4, 2);
	std::string yearStr = dateStr.substr(7, 4);

	unsigned int month = 0u;
	for (unsigned int n = 0u; n < 12; n++)
	{
		if (months[n] == monthStr)
		{
		  month = n + 1u;
		  break;
		}
	}

	ocean_assert(month >= 1 && month <= 12);
	if (month < 1 || month > 12)
	{
		return std::string();
	}

	unsigned int day = atoi(dayStr.c_str());
	unsigned int year = atoi(yearStr.c_str());

	ocean_assert(day >= 1 && day <= 31);
	if (day < 1 || day > 31)
	{
		return std::string();
	}

	return String::toAString(year) + std::string(".") + String::toAString(month, 2u) + std::string(".") + String::toAString(day, 2u);
}

std::string Build::buildTime(const char* t)
{
	std::string time(t);

	ocean_assert(time.length() == 8);

	if (time.length() != 8)
	{
		return std::string();
	}

	std::string hourString = time.substr(0, 2);
	std::string minuteString = time.substr(3, 2);
	std::string secondString = time.substr(6, 2);

	const unsigned int hour = atoi(hourString.c_str());
	const unsigned int minute = atoi(minuteString.c_str());
	const unsigned int second = atoi(secondString.c_str());

	return String::toAString(hour, 2u) + std::string(":") + String::toAString(minute, 2u) + std::string(":") + String::toAString(second, 2u);
}

}
