/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/String.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <locale>

#if defined(OCEAN_PLATFORM_BUILD_WINDOWS) || defined(__APPLE__) || (defined(__linux__) && !defined(_ANDROID))
	#include <codecvt>
#endif

namespace Ocean
{

std::string String::toAString(const char value)
{
	return std::string(1, value);
}

std::string String::toAString(const unsigned char value)
{
	return std::string(1, value);
}

std::string String::toAString(const double value, const unsigned int precision)
{
	std::ostringstream stream;

	constexpr double maximalFixedValue = 1.0e30;

	if (value >= -maximalFixedValue && value <= maximalFixedValue)
	{
		stream.setf(std::ios::fixed, std::ios::floatfield);
	}

	stream.precision(std::streamsize(precision));
	stream << value;

	return stream.str();
}

std::string String::toAString(const float value, const unsigned int precision)
{
	std::ostringstream stream;

	constexpr float maximalFixedValue = 1.0e20f;

	if (value >= -maximalFixedValue && value <= maximalFixedValue)
	{
		stream.setf(std::ios::fixed, std::ios::floatfield);
	}

	stream.precision(std::streamsize(precision));
	stream << value;

	return stream.str();
}

std::string String::toAStringScientific(const double value, const unsigned int precision)
{
	std::ostringstream stream;
	stream.setf(std::ios::scientific, std::ios::floatfield);
	stream.precision(std::streamsize(precision));
	stream << value;

	return stream.str();
}

std::string String::toAStringScientific(const float value, const unsigned int precision)
{
	std::ostringstream stream;
	stream.setf(std::ios::scientific, std::ios::floatfield);
	stream.precision(std::streamsize(precision));
	stream << value;

	return stream.str();
}

std::string String::toAString(const int value)
{
	std::ostringstream stream;
	stream << value;

	return stream.str();
}

std::string String::toAString(const unsigned int value)
{
	std::ostringstream stream;
	stream << value;

	return stream.str();
}

std::string String::toAString(const long value)
{
	std::ostringstream stream;
	stream << value;

	return stream.str();
}

std::string String::toAString(const long long value)
{
	std::ostringstream stream;
	stream << value;

	return stream.str();
}

std::string String::toAString(const unsigned long long value)
{
	std::ostringstream stream;
	stream << value;

	return stream.str();
}

std::string String::toAString(const short value)
{
	std::ostringstream stream;
	stream << value;

	return stream.str();
}

std::string String::toAString(const unsigned short value)
{
	std::ostringstream stream;
	stream << value;

	return stream.str();
}

std::string String::toAString(const long unsigned int value )
{
	std::ostringstream stream;
	stream << value;
	return stream.str();
}

std::string String::toAString(const unsigned int value, const unsigned int minimalChars)
{
	const std::string numberString(String::toAString(value));
	return std::string(max(0, int(minimalChars - numberString.size())), '0') + numberString;
}

std::string String::toAStringHex(const char value, const bool upperCases)
{
	return toAStringHexReverse((unsigned char*)(&value), sizeof(value), upperCases);
}

std::string String::toAStringHex(const unsigned char value, const bool upperCases)
{
	return toAStringHexReverse((unsigned char*)(&value), sizeof(value), upperCases);
}

std::string String::toAStringHex(const double value, const bool upperCases)
{
	return toAStringHexReverse((unsigned char*)(&value), sizeof(value), upperCases);
}

std::string String::toAStringHex(const float value, const bool upperCases)
{
	return toAStringHexReverse((unsigned char*)(&value), sizeof(value), upperCases);
}

std::string String::toAStringHex(const int value, const bool upperCases)
{
	return toAStringHexReverse((unsigned char*)(&value), sizeof(value), upperCases);
}

std::string String::toAStringHex(const unsigned int value, const bool upperCases)
{
	return toAStringHexReverse((unsigned char*)(&value), sizeof(value), upperCases);
}

std::string String::toAStringHex(const long long value, const bool upperCases)
{
	return toAStringHexReverse((unsigned char*)(&value), sizeof(value), upperCases);
}

std::string String::toAStringHex(const unsigned long long value, const bool upperCases)
{
	return toAStringHexReverse((unsigned char*)(&value), sizeof(value), upperCases);
}

std::string String::toAStringHex(const long unsigned int value, const bool upperCases)
{
	return toAStringHexReverse((unsigned char*)(&value), sizeof(value), upperCases);
}

std::string String::toAStringHex(const short value, const bool upperCases)
{
	return toAStringHexReverse((unsigned char*)(&value), sizeof(value), upperCases);
}

std::string String::toAStringHex(const unsigned short value, const bool upperCases)
{
	return toAStringHexReverse((unsigned char*)(&value), sizeof(value), upperCases);
}

std::string String::toAStringHex(const uint8_t* data, const size_t size, const bool upperCases)
{
	ocean_assert(data != nullptr || size == 0);

	std::string result(size * 2, ' ');

	const char aCharacter = upperCases ? 'A' : 'a';

	for (size_t n = 0; n < size; ++n)
	{
		const uint8_t value = data[n];

		const uint8_t integer0 = value & 0x0Fu;
		const uint8_t integer1 = (value >> 4u) & 0x0Fu;

		char* const target = &result[n * 2];

		if (integer0 < 10u)
		{
			target[1] = '0' + char(integer0);
		}
		else
		{
			target[1] = aCharacter + char(integer0 - 10);
		}

		if (integer1 < 10u)
		{
			target[0] = '0' + char(integer1);
		}
		else
		{
			target[0] = aCharacter + char(integer1 - 10);
		}
	}

	return result;
}

std::string String::toAStringHexReverse(const uint8_t* data, const size_t size, const bool upperCases)
{
	ocean_assert(data != nullptr || size == 0);

	std::string result(size * 2, ' ');

	const char aCharacter = upperCases ? 'A' : 'a';

	for (size_t n = 0u; n < size; ++n)
	{
		const uint8_t value = data[n];

		const uint8_t integer0 = value & 0x0F;
		const uint8_t integer1 = (value >> 4) & 0x0F;

		char* const target = &result[(size - n - 1) * 2];

		if (integer0 < 10u)
		{
			target[1] = '0' + char(integer0);
		}
		else
		{
			target[1] = aCharacter + char(integer0 - 10);
		}

		if (integer1 < 10u)
		{
			target[0] = '0' + char(integer1);
		}
		else
		{
			target[0] = aCharacter + char(integer1 - 10);
		}
	}

	return result;
}

std::string String::toLower(const std::string& value)
{
	std::string result(value.size(), ' ');

	for (size_t n = 0; n < value.size(); ++n)
	{
		ocean_assert(value[n] <= char(127));

		result[n] = char(tolower(int(value[n])));
	}

	return result;
}

std::string String::toUpper(const std::string& value)
{
	std::string result(value.size(), ' ');

	for (size_t n = 0; n < value.size(); ++n)
	{
		ocean_assert(value[n] <= char(127));

		result[n] = char(toupper(int(value[n])));
	}

	return result;
}

std::string String::trimFront(const std::string& value, const char character)
{
	for (size_t n = 0; n < value.length(); ++n)
	{
		if (value[n] != character)
		{
			return value.substr(n);
		}
	}

	return std::string();
}

std::string String::trimBack(const std::string& value, const char character)
{
	for (size_t n = value.length() - 1; n != size_t(-1); --n)
	{
		if (value[n] != character)
		{
			return value.substr(0, n + 1);
		}
	}

	return std::string();
}

std::string String::trim(const std::string& value, const char character)
{
	for (size_t left = 0; left < value.length(); ++left)
	{
		if (value[left] != character)
		{
			for (size_t right = value.length() - 1; right != left; --right)
			{
				if (value[right] != character)
				{
					return value.substr(left, right + 1 - left);
				}
			}

			return value.substr(left, 1);
		}
	}

	for (size_t right = value.length() - 1; right != size_t(-1); --right)
	{
		if (value[right] != character)
		{
			return value.substr(0, right + 1);
		}
	}

	return std::string();
}

std::string String::replace(std::string&& value, const std::string& oldString, const std::string& newString, const bool onlyFirstOccurrence)
{
	if (value.empty() || oldString.empty() || oldString == newString)
	{
		return std::move(value);
	}

	if (onlyFirstOccurrence)
	{
		const std::string::size_type position = value.find(oldString);

		if (position == std::string::npos)
		{
			return std::move(value);
		}

		return value.substr(0, position) + newString + value.substr(position + oldString.length());
	}

	std::vector<size_t> replacementOffsets;
	replacementOffsets.reserve(16);

	std::string::size_type lastPosition = 0;
	std::string::size_type position = value.find(oldString);

	while (position != std::string::npos)
	{
		if (replacementOffsets.empty())
		{
			replacementOffsets.emplace_back(position);
		}
		else
		{
			ocean_assert(position > lastPosition);
			ocean_assert(position - lastPosition  >= oldString.size());
			replacementOffsets.emplace_back(position - lastPosition - oldString.size());
		}

		lastPosition = position;

		position = value.find(oldString, position + oldString.length());
	}

	if (replacementOffsets.empty())
	{
		return std::move(value);
	}

	const size_t finalLength = value.length() + replacementOffsets.size() * newString.size() - replacementOffsets.size() * oldString.size();

	if (finalLength == 0)
	{
		return std::string();
	}

	std::string result(finalLength, '0');

	char* const targetPointerStart = &result[0];
	char* targetPointerMoving = targetPointerStart;

	const char* sourcePointerMoving = value.c_str();

	ocean_assert(!replacementOffsets.empty());

	for (const size_t replacementOffset : replacementOffsets)
	{
		ocean_assert(sourcePointerMoving + replacementOffset <= value.c_str() + value.length());
		ocean_assert(targetPointerMoving + replacementOffset + newString.length() <= targetPointerStart + result.length());

		memcpy(targetPointerMoving, sourcePointerMoving, replacementOffset);
		memcpy(targetPointerMoving + replacementOffset, newString.c_str(), newString.length() * sizeof(char));

		targetPointerMoving += replacementOffset + newString.length();
		sourcePointerMoving += replacementOffset + oldString.length();
	}

	const size_t remainingBlock = value.c_str() + value.length() - sourcePointerMoving;

	ocean_assert(sourcePointerMoving + remainingBlock <= value.c_str() + value.length());
	ocean_assert(targetPointerMoving + remainingBlock <= targetPointerStart + result.length());
	memcpy(targetPointerMoving, sourcePointerMoving, remainingBlock);

	return result;
}

std::string String::replaceCharacters(const std::string& value, const char oldCharacter, const char newCharacter)
{
	std::string result(value);

	for (size_t n = 0; n < result.length(); ++n)
	{
		if (result[n] == oldCharacter)
		{
			result[n] = newCharacter;
		}
	}

	return result;
}

std::string String::exchangeCharacters(const std::string& value, const char character0, const char character1)
{
	std::string result(value);

	for (size_t n = 0; n < result.length(); ++n)
	{
		if (result[n] == character0)
		{
			result[n] = character1;
		}
		else if (result[n] == character1)
		{
			result[n] = character0;
		}
	}

	return result;
}

std::string String::removeCharacters(const std::string& value, const char character)
{
	std::string result;
	result.reserve(value.size());

	for (std::string::const_iterator i = value.begin(); i != value.end(); ++i)
	{
		if (*i != character)
		{
			result.push_back(*i);
		}
	}

	return result;
}

bool String::isBoolean(const std::string& stringValue, const bool matchCase, const bool acceptInteger, bool* value)
{
	if (stringValue.empty())
	{
		return false;
	}

	if (acceptInteger && stringValue.length() == 1)
	{
		if (stringValue[0] == '1')
		{
			if (value)
			{
				*value = true;
			}

			return true;
		}
		else if (stringValue[0] == '0')
		{
			if (value)
			{
				*value = false;
			}

			return true;
		}

		return false;
	}


	// 'true' or 'false'

	if (stringValue.length() == 4)
	{
		if ((matchCase && stringValue[0] == 't' && stringValue[1] == 'r' && stringValue[2] == 'u' && stringValue[3] == 'e')
				|| (!matchCase && (stringValue[0] == 't' || stringValue[0] == 'T')
						&& (stringValue[1] == 'r' || stringValue[1] == 'R')
						&& (stringValue[2] == 'u' || stringValue[2] == 'U')
						&& (stringValue[3] == 'e' || stringValue[3] == 'E')))
		{
			if (value)
			{
				*value = true;
			}

			return true;
		}
	}
	else if (stringValue.length() == 5)
	{
		if ((matchCase && stringValue[0] == 'f' && stringValue[1] == 'a' && stringValue[2] == 'l' && stringValue[3] == 's' && stringValue[4] == 'e')
				|| (!matchCase && (stringValue[0] == 'f' || stringValue[0] == 'F')
						&& (stringValue[1] == 'a' || stringValue[1] == 'A')
						&& (stringValue[2] == 'l' || stringValue[2] == 'L')
						&& (stringValue[3] == 's' || stringValue[3] == 'S')
						&& (stringValue[4] == 'e' || stringValue[4] == 'E')))
		{
			if (value)
			{
				*value = false;
			}

			return true;
		}
	}

	return false;
}

bool String::isInteger32(const std::string& stringValue, int32_t* value)
{
	// -2147483648

	if (stringValue.empty() || stringValue.length() > 11)
	{
		return false;
	}

	if (stringValue[0] == '+' || stringValue[0] == '-')
	{
		// now accept ten following digits

		if (stringValue.length() == 1)
		{
			return false;
		}

		for (size_t n = 1; n < stringValue.length(); ++n)
		{
			if (stringValue[n] < '0' || stringValue[n] > '9')
			{
				return false;
			}
		}

		// make a precise 32 bit overflow check
		if (stringValue.length() == 11)
		{
			if (stringValue[0] == '+')
			{
				const char* const reference = "2147483647";

				for (unsigned int n = 0; n < 10; ++n)
				{
					if (stringValue[n + 1] < reference[n])
					{
						break;
					}
					else if (stringValue[n + 1] > reference[n])
					{
						return false;
					}
				}
			}
			else
			{
				ocean_assert(stringValue[0] == '-');

				const char* const reference = "2147483648";

				for (unsigned int n = 0; n < 10; ++n)
				{
					if (stringValue[n + 1] < reference[n])
					{
						break;
					}
					else if (stringValue[n + 1] > reference[n])
					{
						return false;
					}
				}
			}
		}

		if (value)
		{
			*value = atoi(stringValue.c_str());
		}

		return true;
	}
	else
	{
		// accept only ten digits

		if (stringValue.length() > 10)
		{
			return false;
		}

		for (size_t n = 0; n < stringValue.length(); ++n)
		{
			if (stringValue[n] < '0' || stringValue[n] > '9')
			{
				return false;
			}
		}

		// make a precise 32 bit overflow check
		if (stringValue.length() == 10)
		{
			const char* const reference = "2147483647";

			for (unsigned int n = 0; n < 10; ++n)
			{
				if (stringValue[n] < reference[n])
				{
					break;
				}
				else if (stringValue[n] > reference[n])
				{
					return false;
				}
			}
		}

		if (value)
		{
			*value = atoi(stringValue.c_str());
		}

		return true;
	}
}

bool String::isUnsignedInteger64(const std::string& stringValue, uint64_t* value)
{
	if (stringValue.empty())
	{
		return false;
	}

	const char* characters = stringValue.c_str();
	size_t size = stringValue.size();

	ocean_assert(characters != nullptr && size != 0);

	if (characters[0] == '+')
	{
		++characters;
		--size;
	}

	if (size == 0)
	{
		return false;
	}

	// maximal value is 18446744073709551615

	constexpr size_t maximalInputLength = 20;
	ocean_assert(std::string("18446744073709551615").length() == maximalInputLength);
	ocean_assert(std::numeric_limits<uint64_t>::max() == uint64_t(18446744073709551615ull));

	if (size > maximalInputLength)
	{
		return false;
	}

	uint64_t internalValue = 0ull;

	for (size_t n = 0; n < size; ++n)
	{
		const char digit = characters[n];

		if (digit >= '0' && digit <= '9')
		{
			if (n == maximalInputLength - 1)
			{
				if (internalValue > 1844674407370955161ull || (internalValue == 1844674407370955161ull && digit > '5'))
				{
					return false;
				}
			}

			internalValue = internalValue * 10ull + uint64_t(digit - '0');
		}
		else
		{
			return false;
		}
	}

	if (value != nullptr)
	{
		*value = internalValue;
	}

	return true;
}

bool String::isHexValue64(const char* hexString, size_t length, const bool needPrefix, unsigned long long* value)
{
	ocean_assert(hexString && length > 0 && length <= 18);

	if (length >= 2 && hexString[0] == '0' && hexString[1] == 'x')
	{
		hexString += 2;
		length -= 2;
	}
	else if (needPrefix)
	{
		return false;
	}

	if (length > 16u)
	{
		return false;
	}

	if (value)
	{
		*value = 0ull;

		for (size_t n = 0; n < length; ++n)
		{
			*value <<= 4u;

			const char& character = hexString[n];

			if (character >= '0' && character <= '9')
			{
				*value |= (unsigned long long)(character - '0');
			}
			else if (character >= 'a' && character <= 'f')
			{
				*value |= (unsigned long long)(character - 'a' + 10);
			}
			else if (character >= 'A' && character <= 'F')
			{
				*value |= (unsigned long long)(character - 'A' + 10);
			}
			else
			{
				return false;
			}
		}

		return true;
	}
	else
	{
		for (size_t n = 0; n < length; ++n)
		{
			const char& character = hexString[n];

			if (!((character >= '0' && character <= '9') || (character >= 'a' && character <= 'f') || (character >= 'A' && character <= 'F')))
			{
				return false;
			}
		}

		return true;
	}
}

bool String::isNumber(const std::string& stringValue, const bool acceptInteger, double* value)
{
	if (stringValue.empty())
	{
		return false;
	}

	// 10.4, -9.0, +10.2
	// -10.23e+10, 10.5E-3
	// 1.

	bool hasDigit = false;
	bool hasExponent = false;
	bool hasDot = false;

	size_t index = 0;
	while (index < stringValue.length())
	{
		if (!hasDigit && stringValue[index] >= '0' && stringValue[index] <= '9')
		{
			hasDigit = true;
		}

		// if we have a prefix sign, a digit or a dot (before the exponent)
		if ((stringValue[index] >= '0' && stringValue[index] <= '9')
				|| (index == 0 && (stringValue[index] == '+' || stringValue[index] == '-'))
				|| (!hasDot && !hasExponent && stringValue[index] == '.'))
		{
			if (stringValue[index] == '.')
			{
				hasDot = true;
			}

			++index;
			continue;
		}

		// if we have an exponent
		if (!hasExponent && (stringValue[index] == 'e' || stringValue[index] == 'E'))
		{
			if (index + 2 < stringValue.length() && (stringValue[index + 1] == '+' || stringValue[index + 1] == '-')
				&& stringValue[index + 2] >= '0' && stringValue[index + 2] <= '9')
			{
				hasExponent = true;
				index += 3;
				continue;
			}
			else if (index + 1 < stringValue.length() && stringValue[index + 1] >= '0' && stringValue[index + 1] <= '9')
			{
				hasExponent = true;
				index += 2;
				continue;
			}
		}

		return false;
	}

	if (hasDigit && (hasDot || hasExponent || acceptInteger))
	{
		if (value)
		{
			*value = atof(stringValue.c_str());
		}

		return true;
	}

	return false;
}

std::string String::toAString(const wchar_t value)
{
	return toAString(std::wstring(1, value));
}

std::string String::toAString(const char* value)
{
	if (value == nullptr)
	{
		return std::string();
	}

	return std::string(value);
}

std::string String::toAString(const wchar_t* value)
{
	if (value == nullptr)
	{
		return std::string();
	}

	return toAString(std::wstring(value));
}

std::string String::toAString(const std::string& value)
{
	return value;
}

std::string String::toAString(const std::wstring& value)
{
	std::string result(value.length(), ' ');

	const size_t chars = wcstombs((char*)result.c_str(), value.c_str(), value.length());
	ocean_assert_and_suppress_unused(chars == value.length(), chars);

	return result;
}

std::wstring String::toWString(const char value)
{
	return String::toWString(toAString(value));
}

std::wstring String::toWString(const char* value)
{
	if (value == nullptr)
	{
		return std::wstring();
	}

	return toWString(std::string(value));
}

std::wstring String::toWString(const wchar_t* value)
{
	if (value == nullptr)
	{
		return std::wstring();
	}

	return std::wstring(value);
}

std::wstring String::toWString(const unsigned short* value)
{
	if (value == nullptr)
	{
		return std::wstring();
	}

	static_assert(sizeof(wchar_t) >= sizeof(unsigned short), "Invalid data type!");

	if constexpr (sizeof(wchar_t) == sizeof(unsigned short))
	{
		return std::wstring((wchar_t*)(value));
	}
	else
	{
		ocean_assert(sizeof(wchar_t) >= sizeof(unsigned char));

		std::wstring result;

		while (*value != 0)
		{
			result.push_back((wchar_t)*value);
			value++;
		}

		return result;
	}
}

std::wstring String::toWString(const unsigned char value)
{
	return String::toWString(toAString(value));
}

std::wstring String::toWString(const double value, const unsigned int precision)
{
	std::wostringstream stream;

	constexpr double maximalFixedValue = 1.0e30;

	if (value >= -maximalFixedValue && value <= maximalFixedValue)
	{
		stream.setf(std::ios::fixed, std::ios::floatfield);
	}

	stream.precision(std::streamsize(precision));
	stream << value;

	return stream.str();
}

std::wstring String::toWString(const float value, const unsigned int precision)
{
	std::wostringstream stream;

	constexpr float maximalFixedValue = 1.0e20f;

	if (value >= -maximalFixedValue && value <= maximalFixedValue)
	{
		stream.setf(std::ios::fixed, std::ios::floatfield);
	}

	stream.precision(std::streamsize(precision));
	stream << value;

	return stream.str();
}

std::wstring String::toWStringScientific(const double value, const unsigned int precision)
{
	std::wostringstream stream;
	stream.setf(std::ios::scientific, std::ios::floatfield);
	stream.precision(std::streamsize(precision));
	stream << value;

	return stream.str();
}

std::wstring String::toWStringScientific(const float value, const unsigned int precision)
{
	std::wostringstream stream;
	stream.setf(std::ios::scientific, std::ios::floatfield);
	stream.precision(std::streamsize(precision));
	stream << value;

	return stream.str();
}

std::wstring String::toWString(const int value)
{
	std::wostringstream stream;
	stream << value;

	return stream.str();
}

std::wstring String::toWString(const unsigned int value)
{
	std::wostringstream stream;
	stream << value;

	return stream.str();
}

std::wstring String::toWString(const long value)
{
	std::wostringstream stream;
	stream << value;

	return stream.str();
}

std::wstring String::toWString(const long long value)
{
	std::wostringstream stream;
	stream << value;

	return stream.str();
}

std::wstring String::toWString(const unsigned long long value)
{
	std::wostringstream stream;
	stream << value;

	return stream.str();
}

std::wstring String::toWString(const short value)
{
	std::wostringstream stream;
	stream << value;

	return stream.str();
}

std::wstring String::toWString(const unsigned short value)
{
	std::wostringstream stream;
	stream << value;

	return stream.str();
}

std::wstring String::toWString(const long unsigned int value)
{
	std::wostringstream stream;
	stream << value;

	return stream.str();
}

	std::wstring String::toWString(const wchar_t value)
{
	std::wostringstream stream;
	stream << value;

	return stream.str();
}

std::wstring String::toWString(const unsigned int value, const unsigned int minimalChars)
{
	const std::wstring numberString(String::toWString(value));
	return std::wstring(max(0, int(minimalChars - numberString.size())), L'0') + numberString;
}

std::wstring String::toWString(const std::string& value)
{
	std::wstring result(value.length(), L' ');

	const size_t chars = mbstowcs((wchar_t*)result.c_str(), value.c_str(), value.length());
	ocean_assert_and_suppress_unused(chars == value.length(), chars);

	return result;
}

std::wstring String::toWString(const std::wstring& value)
{
	return value;
}

std::wstring String::trimFront(const std::wstring& value, const wchar_t character)
{
	std::wstring result(value);

	while (result.find(character) == 0)
	{
		result = result.substr(1);
	}

	return result;
}

std::wstring String::trimBack(const std::wstring& value, const wchar_t character)
{
	std::wstring result(value);

	while (!result.empty() && result.rfind(character) == result.length() - 1)
	{
		result = result.substr(0, result.length() - 1);
	}

	return result;
}

std::wstring String::trim(const std::wstring& value, const wchar_t character)
{
	return trimBack(trimFront(value, character), character);
}

std::string String::trimWhitespace(const std::string& text)
{
	return trimWhitespaceString<char>(text);
}

std::wstring String::trimWhitespace(const std::wstring& text)
{
	return trimWhitespaceString<wchar_t>(text);
}

std::wstring String::replaceCharacters(const std::wstring& value, const wchar_t oldCharacter, const wchar_t newCharacter)
{
	std::wstring result(value);

	for (size_t n = 0; n < result.length(); ++n)
	{
		if (result[n] == oldCharacter)
		{
			result[n] = newCharacter;
		}
	}

	return result;
}

std::wstring String::exchangeCharacters(const std::wstring& value, const wchar_t character0, const wchar_t character1)
{
	std::wstring result(value);

	for (size_t n = 0; n < result.length(); ++n)
	{
		if (result[n] == character0)
		{
			result[n] = character1;
		}
		else if (result[n] == character1)
		{
			result[n] = character0;
		}
	}

	return result;
}

std::wstring String::removeCharacters(const std::wstring& value, const wchar_t character)
{
	std::wstring result;
	result.reserve(value.size());

	for (std::wstring::const_iterator i = value.begin(); i != value.end(); ++i)
	{
		if (*i != character)
		{
			result.push_back(*i);
		}
	}

	return result;
}

template <typename TChar>
std::basic_string<TChar> String::trimWhitespaceString(const std::basic_string<TChar>& text)
{
	if (text.empty())
	{
		return std::basic_string<TChar>();
	}

	ocean_assert(text[text.size()] == TChar(0));

	// let's determine the first non-white character

	size_t indexFirst = 0;

	while (indexFirst < text.size())
	{
		if (!iswspace(text[indexFirst]) && text[indexFirst] != TChar(0))
		{
			break;
		}

		++indexFirst;
	}

	// let's determine the last non-white character

	size_t indexLast = text.size() - 1;

	while (indexFirst < indexLast)
	{
		if (!iswspace(text[indexLast]) && text[indexLast] != TChar(0))
		{
			break;
		}

		ocean_assert(indexLast >= 1);
		--indexLast;
	}

	if (indexLast < indexFirst)
	{
		return std::basic_string<TChar>();
	}

	// let's ensure that the string does not contain any null terminator (besides the intended terminator at the very end of the string

	for (size_t index = indexFirst; index <= indexLast; ++index)
	{
		if (text[index] == TChar(0))
		{
			if (index == indexFirst)
			{
				return std::basic_string<TChar>();
			}

			indexLast = index - 1;

			break;
		}
	}

	ocean_assert(indexFirst <= indexLast);
	ocean_assert(indexLast < text.size());

	std::basic_string<TChar> result = text.substr(indexFirst, indexLast - indexFirst + 1);
	ocean_assert(result[result.size()] == TChar(0));

	return result;
}


}
