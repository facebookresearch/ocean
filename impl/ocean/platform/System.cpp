/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
		Log::warning() << "Failed to read the value of the environment variable \"" << variable << "\".";

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
