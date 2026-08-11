/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/win/Registry.h"

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

	std::vector<std::wstring> wideValues;
	wideValues.reserve(value.size());

	size_t size = 0;

	for (const std::string& singleValue : value)
	{
		wideValues.emplace_back(String::toWString(singleValue));
		size += wideValues.back().length() + 1;
	}
	++size;

	if (value.empty())
	{
		++size;
	}

	std::vector<wchar_t> buffer(size);

	wchar_t* pointer = buffer.data();
	for (const std::wstring& wideValue : wideValues)
	{
		memcpy(pointer, wideValue.c_str(), (wideValue.length() + 1) * sizeof(wchar_t));

		pointer += wideValue.length() + 1;
	}

	pointer[0] = L'\0';
	if (value.empty())
	{
		pointer[1] = L'\0';
	}

	const bool result = RegSetValueEx(key, String::toWString(name).c_str(), 0, REG_MULTI_SZ, (BYTE*)(buffer.data()), DWORD(sizeof(wchar_t) * size)) == ERROR_SUCCESS;

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

	const std::wstring wideName(String::toWString(name));

	DWORD type = 0;
	DWORD size = 0;

	if (RegQueryValueEx(key, wideName.c_str(), 0, &type, nullptr, &size) != ERROR_SUCCESS || type != REG_SZ || size == 0)
	{
		return defaultValue;
	}

	// a REG_SZ value does not need to be null terminated, so the buffer has room for a terminator

	std::vector<wchar_t> buffer(size / sizeof(wchar_t) + 2, L'\0');

	DWORD bufferSize = DWORD((buffer.size() - 1) * sizeof(wchar_t));

	if (RegQueryValueEx(key, wideName.c_str(), 0, &type, (BYTE*)(buffer.data()), &bufferSize) != ERROR_SUCCESS || type != REG_SZ)
	{
		return defaultValue;
	}

	return String::toAString(std::wstring(buffer.data(), wcsnlen(buffer.data(), buffer.size() - 1)));
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

	const std::wstring wideName(String::toWString(name));

	DWORD type = 0;
	DWORD size = 0;

	if (RegQueryValueEx(key, wideName.c_str(), 0, &type, nullptr, &size) != ERROR_SUCCESS || type != REG_MULTI_SZ || size == 0)
	{
		return defaultValue;
	}

	std::vector<wchar_t> buffer(size / sizeof(wchar_t) + 2, L'\0');

	if (RegQueryValueEx(key, wideName.c_str(), 0, &type, (BYTE*)(buffer.data()), &size) != ERROR_SUCCESS || type != REG_MULTI_SZ)
	{
		return defaultValue;
	}

	Names resultValue;

	const wchar_t* pointer = buffer.data();
	const wchar_t* const end = buffer.data() + buffer.size() - 1;

	while (pointer < end && pointer[0] != L'\0')
	{
		const size_t length = wcsnlen(pointer, size_t(end - pointer));

		resultValue.push_back(String::toAString(std::wstring(pointer, length)));

		pointer += length + 1;
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
	if (key == nullptr)
	{
		return Names();
	}

	DWORD maximalValueNameLength = 0;

	if (RegQueryInfoKey(key, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &maximalValueNameLength, nullptr, nullptr, nullptr) != ERROR_SUCCESS)
	{
		return Names();
	}

	// the reported length does not include the terminating null character

	std::vector<wchar_t> valueName(size_t(maximalValueNameLength) + 1, L'\0');

	Names names;

	for (DWORD index = 0u; true; ++index)
	{
		DWORD valueNameSize = DWORD(valueName.size());

		if (RegEnumValue(key, index, valueName.data(), &valueNameSize, 0, nullptr, nullptr, nullptr) != ERROR_SUCCESS)
		{
			break;
		}

		names.push_back(String::toAString(std::wstring(valueName.data(), valueNameSize)));
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
	if (key == nullptr)
	{
		return Names();
	}

	DWORD maximalKeyNameLength = 0;

	if (RegQueryInfoKey(key, nullptr, nullptr, nullptr, nullptr, &maximalKeyNameLength, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS)
	{
		return Names();
	}

	// the reported length does not include the terminating null character

	std::vector<wchar_t> keyName(size_t(maximalKeyNameLength) + 1, L'\0');

	Names names;

	for (DWORD index = 0u; true; ++index)
	{
		DWORD keyNameSize = DWORD(keyName.size());

		if (RegEnumKeyEx(key, index, keyName.data(), &keyNameSize, 0, nullptr, nullptr, nullptr) != ERROR_SUCCESS)
		{
			break;
		}

		names.push_back(String::toAString(std::wstring(keyName.data(), keyNameSize)));
	}

	return names;
}

}

}

}
