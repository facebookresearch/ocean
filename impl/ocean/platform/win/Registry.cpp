/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/win/Registry.h"

#include "ocean/base/Exception.h"
#include "ocean/base/String.h"

namespace Ocean
{

namespace Platform
{

namespace Win
{

HKEY Registry::translateRoot(const RootType root)
{
	switch (root)
	{
		case ROOT_CLASSES_ROOT:
			return HKEY_CLASSES_ROOT;

		case ROOT_CURRENT_USER:
			return HKEY_CURRENT_USER;

		case ROOT_LOCAL_MACHINE:
			return HKEY_LOCAL_MACHINE;

		case ROOT_USERS:
			return HKEY_USERS;
	}

	return nullptr;
}

HKEY Registry::openKey(const RootType root, const std::string& path)
{
	if (path.empty())
	{
		return nullptr;
	}

	HKEY key = nullptr;
	DWORD createdOrOpened = 0;

	if (RegCreateKeyEx(translateRoot(root), String::toWString(path).c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &key, &createdOrOpened) == ERROR_SUCCESS)
	{
		return key;
	}

	return nullptr;
}

HKEY Registry::openKey(const HKEY key, const std::string& name)
{
	if (key == nullptr || name.empty())
	{
		return nullptr;
	}

	HKEY subKey = nullptr;
	DWORD createdOrOpened = 0;

	if (RegCreateKeyEx(key, String::toWString(name).c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &subKey, &createdOrOpened) == ERROR_SUCCESS)
	{
		return subKey;
	}

	return nullptr;
}

bool Registry::closeKey(const HKEY key)
{
	if (key == nullptr)
	{
		return false;
	}

	return RegCloseKey(key) == ERROR_SUCCESS;
}

bool Registry::createKey(const RootType root, const std::string& path)
{
	return closeKey(openKey(root, path));
}

Registry::ValueType Registry::valueType(const HKEY key, const std::string& name)
{
	if (key == nullptr)
	{
		return VALUE_INVALID;
	}

	DWORD type = 0;

	if (RegQueryValueEx(key, String::toWString(name).c_str(), 0, &type, nullptr, nullptr) != ERROR_SUCCESS)
	{
		return VALUE_INVALID;
	}

	switch (type)
	{
		case REG_BINARY:
			return VALUE_DATA;

		case REG_DWORD:
			return VALUE_INTEGER32;

		case REG_QWORD:
			return VALUE_INTEGER64;

		case REG_SZ:
			return VALUE_STRING;

		case REG_MULTI_SZ:
			return VALUE_STRINGS;
	}

	return VALUE_INVALID;
}

bool Registry::setValue(const RootType root, const std::string& path, const std::string& name, const int value)
{
	const HKEY key = openKey(root, path);

	const bool result = setValue(key, name, value);
	closeKey(key);

	return result;
}

bool Registry::setValue(const HKEY key, const std::string& name, const int value)
{
	if (key == nullptr || name.empty())
	{
		return false;
	}

	return RegSetValueEx(key, String::toWString(name).c_str(), 0, REG_DWORD, (BYTE*)&value, sizeof(value)) == ERROR_SUCCESS;
}

bool Registry::setValue(const RootType root, const std::string& path, const std::string& name, const long long value)
{
	const HKEY key = openKey(root, path);

	const bool result = setValue(key, name, value);
	closeKey(key);

	return result;
}

bool Registry::setValue(const HKEY key, const std::string& name, const long long value)
{
	if (key == nullptr || name.empty())
	{
		return false;
	}

	return RegSetValueEx(key, String::toWString(name).c_str(), 0, REG_QWORD, (BYTE*)&value, sizeof(value)) == ERROR_SUCCESS;
}

bool Registry::setValue(const RootType root, const std::string& path, const std::string& name, const std::string& value)
{
	const HKEY key = openKey(root, path);

	const bool result = setValue(key, name, value);
	closeKey(key);

	return result;
}

bool Registry::setValue(const HKEY key, const std::string& name, const std::string& value)
{
	if (key == nullptr || name.empty())
	{
		return false;
	}

	return RegSetValueEx(key, String::toWString(name).c_str(), 0, REG_SZ, (BYTE*)String::toWString(value).c_str(), DWORD((value.length() + 1) << 1)) == ERROR_SUCCESS;
}

bool Registry::setValue(const RootType root, const std::string& path, const std::string& name, const Names& value)
{
	const HKEY key = openKey(root, path);

	const bool result = setValue(key, name, value);
	closeKey(key);

	return result;
}

bool Registry::setValue(const HKEY key, const std::string& name, const Names& value)
{
	if (key == nullptr || name.empty())
	{
		return false;
	}

	unsigned int size = 0;

	for (Names::const_iterator i = value.begin(); i != value.end(); ++i)
	{
		size += (unsigned int)(i->length() + 1);
	}
	++size;

	if (value.empty())
	{
		++size;
	}

	wchar_t* buffer = new wchar_t[size];
	if (buffer == nullptr)
	{
		throw OutOfMemoryException();
	}

	wchar_t* pointer = buffer;
	for (Names::const_iterator i = value.begin(); i != value.end(); ++i)
	{
		ocean_assert(i->size() > i->length());
		ocean_assert((*i)[i->length()] == L'\0');
		memcpy(pointer, &((*i)[0]), (i->length() + 1) << 1);

		pointer += i->length() + 1;
	}

	pointer[0] = L'\0';
	if (value.empty())
	{
		pointer[1] = L'\0';
	}

	const bool result = RegSetValueEx(key, String::toWString(name).c_str(), 0, REG_MULTI_SZ, (BYTE*)buffer, DWORD(sizeof(wchar_t) * size)) == ERROR_SUCCESS;

	delete [] buffer;
	return result;
}

bool Registry::setValue(const RootType root, const std::string& path, const std::string& name, const unsigned char* value, const unsigned int size)
{
	const HKEY key = openKey(root, path);

	const bool result = setValue(key, name, value, size);
	closeKey(key);

	return result;
}

bool Registry::setValue(const HKEY key, const std::string& name, const unsigned char* value, const unsigned int size)
{
	if (key == nullptr || name.empty() || (value == nullptr && size != 0))
	{
		return false;
	}

	return RegSetValueEx(key, String::toWString(name).c_str(), 0, REG_BINARY, (BYTE*)value, DWORD(size)) == ERROR_SUCCESS;
}

int Registry::value(const RootType root, const std::string& path, const std::string& name, const int defaultValue)
{
	const HKEY key = openKey(root, path);

	const int result = value(key, name, defaultValue);
	closeKey(key);

	return result;
}

int Registry::value(const HKEY key, const std::string& name, const int defaultValue)
{
	if (key == nullptr)
	{
		return defaultValue;
	}

	DWORD type = 0;
	DWORD longValue = 0;
	DWORD valueSize = sizeof(longValue);

	const bool result = RegQueryValueEx(key, String::toWString(name).c_str(), 0, &type, (BYTE*)&longValue, &valueSize) == ERROR_SUCCESS;

	if (result && type == REG_DWORD)
	{
		return longValue;
	}

	return defaultValue;
}

long long Registry::value(const RootType root, const std::string& path, const std::string& name, const long long defaultValue)
{
	const HKEY key = openKey(root, path);

	const long long result = value(key, name, defaultValue);
	closeKey(key);

	return result;
}

long long Registry::value(const HKEY key, const std::string& name, const long long defaultValue)
{
	if (key == nullptr)
	{
		return defaultValue;
	}

	DWORD type = 0;
	long long longLongValue = 0;
	DWORD valueSize = sizeof(longLongValue);

	const bool result = RegQueryValueEx(key, String::toWString(name).c_str(), 0, &type, (BYTE*)&longLongValue, &valueSize) == ERROR_SUCCESS;

	if (result && type == REG_QWORD)
	{
		return longLongValue;
	}

	return defaultValue;
}

std::string Registry::value(const RootType root, const std::string& path, const std::string& name, const std::string& defaultValue)
{
	const HKEY key = openKey(root, path);

	const std::string result = value(key, name, defaultValue);
	closeKey(key);

	return result;
}

std::string Registry::value(const HKEY key, const std::string& name, const std::string& defaultValue)
{
	if (key == nullptr)
	{
		return defaultValue;
	}

	DWORD type = 0;
	DWORD size = 0;
	std::wstring resultValue(String::toWString(defaultValue));

	const bool result = RegQueryValueEx(key, String::toWString(name).c_str(), 0, &type, nullptr, &size) == ERROR_SUCCESS;

	if (result && type == REG_SZ)
	{
		if (size <= 1)
		{
			resultValue = std::wstring();
		}
		else
		{
			resultValue.resize(size);

			size <<= 1;
			RegQueryValueEx(key, String::toWString(name).c_str(), 0, nullptr, (BYTE*)resultValue.c_str(), &size);
			size >>= 1;

			resultValue.resize(size - 1);
		}
	}

	return String::toAString(resultValue);
}

Registry::Names Registry::value(const RootType root, const std::string& path, const std::string& name, const Names& defaultValue)
{
	const HKEY key = openKey(root, path);

	const Names result = value(key, name, defaultValue);
	closeKey(key);

	return result;
}

Registry::Names Registry::value(const HKEY key, const std::string& name, const Names& defaultValue)
{
	if (key == nullptr)
	{
		return defaultValue;
	}

	DWORD type = 0;
	DWORD size = 0;

	Names resultValue;

	const bool result = RegQueryValueEx(key, String::toWString(name).c_str(), 0, &type, nullptr, &size) == ERROR_SUCCESS;

	if (result && type == REG_MULTI_SZ && size > 0)
	{
		wchar_t* buffer = new wchar_t[size];
		if (buffer == nullptr)
		{
			throw OutOfMemoryException();
		}

		if (RegQueryValueEx(key, String::toWString(name).c_str(), 0, nullptr, (BYTE*)buffer, &size) == ERROR_SUCCESS)
		{
			wchar_t* pointer = buffer;

			while (true)
			{
				if (pointer[0] == L'\0')
				{
					break;
				}

				ocean_assert(wcslen(pointer) < size);

				std::wstring newValue(pointer);
				resultValue.push_back(String::toAString(newValue));

				pointer += newValue.length() + 1;
			}

		}
	}

	return resultValue;
}

Registry::Names Registry::values(const RootType root, const std::string& path)
{
	const HKEY key = openKey(root, path);
	if (key == nullptr)
	{
		return Names();
	}

	const Names names = values(key);
	closeKey(key);

	return names;
}

Registry::Names Registry::values(const HKEY key)
{
	unsigned int index = 0;
	wchar_t valueName[1024];
	DWORD valueNameSize = 1023;
	Names names;

	while (RegEnumValue(key, DWORD(index), valueName, &valueNameSize, 0, nullptr, nullptr, nullptr) == ERROR_SUCCESS)
	{
		names.push_back(String::toAString(valueName));
		valueNameSize = 1023;
		++index;
	}

	return names;
}

Registry::Names Registry::keys(const RootType root, const std::string& path)
{
	const HKEY key = openKey(root, path);
	if (key == nullptr)
	{
		return Names();
	}

	const Names names = keys(key);
	closeKey(key);

	return names;
}

Registry::Names Registry::keys(const HKEY key)
{
	unsigned int index = 0;
	wchar_t valueName[256];
	DWORD valueNameSize = 255;
	Names names;

	while (RegEnumKeyEx(key, DWORD(index), valueName, &valueNameSize, 0, nullptr, nullptr, nullptr) == ERROR_SUCCESS)
	{
		names.push_back(String::toAString(valueName));
		valueNameSize = 255;
		++index;
	}

	return names;
}

}

}

}
