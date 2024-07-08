/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/linux/Utilities.h"

#include <array>
#include <cstdlib>

namespace Ocean
{

namespace Platform
{

namespace Linux
{

bool Utilities::checkSecurityEnhancedLinuxStateIsPermissive()
{
	ScopedFILE file(popen("getenforce", "r"));

	if (!file.isValid())
	{
		Log::error() << "Failed to execute 'getenforce' command";
		return false;
	}

	std::string result;
	result.reserve(128);

	std::array<char, 64> buffer;
	while (fgets(buffer.data(), buffer.size() - 1, *file) != nullptr)
	{
		result += buffer.data();

		if (result.size() > 1024 * 1024)
		{
			return false;
		}
	}

	if (result.empty())
	{
		return false;
	}

	if (result.find("Permissive") == 0)
	{
		return true;
	}

	return false;
}

}

}

}
