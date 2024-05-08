// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/platform/System.h"

namespace Ocean
{

namespace Platform
{

std::string System::environmentVariable(const std::string& variable, const bool removeQuotes)
{
#ifdef _WINDOWS

	static_assert(MAX_PATH > 0, "Value of macro MAX_PATH is zero");
	std::string value(MAX_PATH, ' ');
	value.resize(GetEnvironmentVariableA(variable.c_str(), &value[0], MAX_PATH));

#else

	const char* valuePointer = getenv(variable.c_str());

	if (valuePointer == nullptr)
	{
#ifdef __APPLE__
		if (variable == "OCEAN_DEVELOPMENT_PATH")
		{
			// Workaround for Apple platforms if the OCEAN_DEVELOPMENT_PATH environment variable is not defined explicitly

			const char* homePointer = getenv("HOME");

			if (homePointer == nullptr)
			{
				return std::string();
			}

			Log::warning() << "OCEAN_DEVELOPMENT_PATH is not defined, therefore we try to link it to the correct place instead.";

			return std::string(homePointer) + std::string("/fbsource/xplat/ocean");
		}
#endif

		return std::string();
	}

	std::string value(valuePointer);

#endif

	if (removeQuotes)
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

	return value;
}

}

}
