/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/win/System.h"

#include "ocean/base/String.h"

#include "ocean/io/File.h"

namespace Ocean
{

namespace Platform
{

namespace Win
{

IO::Directory System::currentDirectory()
{
	static_assert(MAX_PATH > 0, "Value of macro MAX_PATH is zero");
	wchar_t buffer[MAX_PATH + 1];

	if (GetCurrentDirectoryW(MAX_PATH, buffer) == 0)
	{
		return IO::Directory();
	}

	return IO::Directory(String::toAString(buffer));
}

IO::Directory System::processDirectory()
{
	static_assert(MAX_PATH > 0, "Value of macro MAX_PATH is zero");
	wchar_t buffer[MAX_PATH + 1];

	if (GetModuleFileNameW(nullptr, buffer, MAX_PATH) == 0)
	{
		return IO::Directory();
	}

	const IO::File applicationFile(String::toAString(buffer));
	return IO::Directory(applicationFile);
}

HMODULE System::currentLibraryModule()
{
	HMODULE handle = nullptr;

	if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)System::currentLibraryModule, &handle) == TRUE)
	{
		// now we check whether the calling process is the application or a library
		if (GetModuleHandle(nullptr) == handle)
		{
			// the msdn documentation does not state whether a module handle of a process has to be released
			// however, several tests showed that a FreeLibrary() call does not have any negative effect
			const BOOL result = FreeLibrary(handle);
			ocean_assert_and_suppress_unused(result == TRUE, result);

			return nullptr;
		}

		return handle;
	}

	return nullptr;
}

std::wstring System::moduleName(const HMODULE handle)
{
	if (handle == nullptr)
	{
		return std::wstring();
	}

	wchar_t name[4096 + 2];
	const DWORD result = GetModuleFileName(handle, name, 4096);
	ocean_assert(result != 4096);

	if (result == 0)
	{
		return std::wstring();
	}

	// check whether the name is a zero-terminated string (issue on Windows XP)
	if (name[result - 1] != L'\0')
	{
		name[result] = L'\0';
	}

	return std::wstring(name);
}

std::string System::environmentVariable(const std::string& variable, const bool removeQuotes)
{
	static_assert(MAX_PATH > 0, "Value of macro MAX_PATH is zero");

	std::string value(MAX_PATH, ' ');
	value.resize(GetEnvironmentVariableA(variable.c_str(), &value[0], MAX_PATH));

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

}

}

}
