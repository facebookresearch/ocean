/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/apple/System.h"

#include "ocean/base/StringApple.h"

#include <Foundation/Foundation.h>

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE==1
	#include <UIKit/UIKit.h>
#else
	#include <AppKit/AppKit.h>
#endif

namespace Ocean
{

namespace Platform
{

namespace Apple
{

std::string System::environmentVariable(const std::string& variable, const bool removeQuotes)
{
	ocean_assert(!variable.empty());
	if (variable.empty())
	{
		return std::string();
	}

	const char* valuePointer = getenv(variable.c_str());

	if (valuePointer == nullptr)
	{
		Log::warning() << "Failed to read the value of the environment variable \"" << variable << "\".";
		return std::string();
	}

	std::string value(valuePointer);

	if (removeQuotes)
	{
		while (value.length() != 0)
		{
			if (value[0] == '\"')
			{
				value = value.substr(1);
			}
			else if (value[value.length() - 1] == '\"')
			{
				value = value.substr(0, value.length() - 1);
			}
			else
			{
				break;
			}
		}
	}

	return value;
}

bool System::setEnvironmentVariable(const std::string& variable, const std::string& value)
{
	ocean_assert(!variable.empty());
	if (variable.empty())
	{
		return false;
	}

	if (value.empty())
	{
		const int result = unsetenv(variable.c_str());

		ocean_assert(result == 0);
		return result == 0;
	}
	else
	{
		const int result = setenv(variable.c_str(), value.c_str(), 1);

		ocean_assert(result == 0);
		return result == 0;
	}
}

double System::version(unsigned int* major, unsigned int* minor)
{
	NSOperatingSystemVersion version = [[[NSProcessInfo alloc] init] operatingSystemVersion];
	ocean_assert(version.majorVersion > 0 && version.minorVersion >= 0);

	if (major)
	{
		*major = (unsigned int)(version.majorVersion);
	}

	if (minor)
	{
		*minor = (unsigned int)(version.minorVersion);
	}

	ocean_assert(version.minorVersion < 100);
	return double(version.majorVersion) + double(version.minorVersion) * 0.01 + 0.0000000001; // we add an epsilon to avoid precision issues
}

bool System::isMinimalVersion(const unsigned int minimalMajor, const unsigned int minimalMinor)
{
	ocean_assert(minimalMajor >= 10u);

	NSOperatingSystemVersion version = [[[NSProcessInfo alloc] init] operatingSystemVersion];
	ocean_assert(version.majorVersion > 0 && version.minorVersion > 0);

	if ((unsigned int)(version.majorVersion) > minimalMajor)
	{
		return true;
	}

	if ((unsigned int)(version.majorVersion) < minimalMajor)
	{
		return false;
	}

	ocean_assert((unsigned int)version.majorVersion == minimalMajor);

	return (unsigned int)version.minorVersion >= minimalMinor;
}

std::string System::homeDirectory()
{
	return StringApple::toUTF8(NSHomeDirectory());
}

bool System::isBundleApp()
{
	CFBundleRef bundle = CFBundleGetMainBundle();

	if (bundle != nullptr)
	{
		const ScopedCFURLRef bundleUrl(CFBundleCopyBundleURL(bundle));

		if (bundleUrl.object() != nullptr)
		{
			CFStringRef cvStringUrl = CFURLGetString(bundleUrl.object());

			const std::string url = StringApple::toUTF8((__bridge NSString*)(cvStringUrl));

			if (url.size() >= 6 && url.substr(url.size() - 5) == ".app/")
			{
				return true;
			}
		}
	}

	return false;
}

void System::runMainLoop(const double interval)
{
	[[NSRunLoop mainRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:interval]];
}

}

}

}
