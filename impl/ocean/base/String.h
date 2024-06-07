/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_STRING_H
#define META_OCEAN_BASE_STRING_H

#include "ocean/base/Base.h"

#include <sstream>
#include <string>

namespace Ocean
{

/**
 * This class provides string convert functionalities.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT String
{
	public:

		/**
		 * Converts a value to a string with 8bit character.
		 * @param value The value to convert
		 * @return Converted string
		 */
		static std::string toAString(const char value);

		/**
		 * Converts a value to a string with 8bit character.
		 * @param value The value to convert
		 * @return Converted string
		 */
		static std::string toAString(const unsigned char value);

		/**
		 * Converts a value to a string with 8bit character using a fixed notation.
		 * @param value The value to convert, with range (-infinity, infinity)
		 * @param precision Optional floating-point precision field, with range [1, infinity)
		 * @return Converted string
		 */
		static std::string toAString(const double value, const unsigned int precision = 5u);

		/**
		 * Converts a value to a string with 8bit character using a fixed notation.
		 * @param value The value to convert, with range (-infinity, infinity)
		 * @param precision Optional floating-point precision field, with range [1, infinity)
		 * @return Converted string
		 */
		static std::string toAString(const float value, const unsigned int precision = 5u);

		/**
		 * Converts a value to a string with 8bit character using a scientific notation.
		 * @param value The value to convert, with range (-infinity, infinity)
		 * @param precision Optional floating-point precision field, with range [1, infinity)
		 * @return Converted string
		 */
		static std::string toAStringScientific(const double value, const unsigned int precision = 5u);

		/**
		 * Converts a value to a string with 8bit character using a scientific notation.
		 * @param value The value to convert, with range (-infinity, infinity)
		 * @param precision Optional floating-point precision field, with range [1, infinity)
		 * @return Converted string
		 */
		static std::string toAStringScientific(const float value, const unsigned int precision = 5u);

		/**
		 * Converts a value to a string with 8bit character.
		 * @param value The value to convert
		 * @return Converted string
		 */
		static std::string toAString(const int value);

		/**
		 * Converts a value to a string with 8bit character.
		 * @param value The value to convert
		 * @return Converted string
		 */
		static std::string toAString(const unsigned int value);

		/**
		 * Converts a value to a string with 8bit character.
		 * @param value The value to convert
		 * @return Converted string
		 */
		static std::string toAString(const long value);

		/**
		 * Converts a value to a string with 8bit character.
		 * @param value The value to convert
		 * @return Converted string
		 */
		static std::string toAString(const long long value);

		/**
		 * Converts a value to a string with 8bit character.
		 * @param value The value to convert
		 * @return Converted string
		 */
		static std::string toAString(const unsigned long long value);

		/**
		 * Converts a value to a string with 8bit character.
		 * @param value The value to convert
		 * @return Converted string
		 */
		static std::string toAString(const long unsigned int value);

		/**
		 * Converts a value to a string with 8bit character.
		 * @param value The value to convert
		 * @return Converted string
		 */
		static std::string toAString(const short value);

		/**
		 * Converts a value to a string with 8bit character.
		 * @param value The value to convert
		 * @return Converted string
		 */
		static std::string toAString(const unsigned short value);

		/**
		 * Converts a value to a string with a minimal number of characters.
		 * Zero (prefix) are used to fill the missing part.
		 * @param value The value to be converted
		 * @param minimalChars Number of minimal characters to be created
		 * @return Converted string
		 */
		static std::string toAString(const unsigned int value, const unsigned int minimalChars);

		/**
		 * Converts a value to a string with 8bit character as hexadecimal notation.
		 * @param value The value to convert
		 * @param upperCases True, to use upper case characters; False, to use lower case characters
		 * @return Converted string
		 */
		static std::string toAStringHex(const char value, const bool upperCases = true);

		/**
		 * Converts a value to a string with 8bit character as hexadecimal notation.
		 * @param value The value to convert
		 * @param upperCases True, to use upper case characters; False, to use lower case characters
		 * @return Converted string
		 */
		static std::string toAStringHex(const unsigned char value, const bool upperCases = true);

		/**
		 * Converts a value to a string with 8bit character as hexadecimal notation.
		 * @param value The value to convert
		 * @param upperCases True, to use upper case characters; False, to use lower case characters
		 * @return Converted string
		 */
		static std::string toAStringHex(const double value, const bool upperCases = true);

		/**
		 * Converts a value to a string with 8bit character as hexadecimal notation.
		 * @param value The value to convert
		 * @param upperCases True, to use upper case characters; False, to use lower case characters
		 * @return Converted string
		 */
		static std::string toAStringHex(const float value, const bool upperCases = true);

		/**
		 * Converts a value to a string with 8bit character as hexadecimal notation.
		 * @param value The value to convert
		 * @param upperCases True, to use upper case characters; False, to use lower case characters
		 * @return Converted string
		 */
		static std::string toAStringHex(const int value, const bool upperCases = true);

		/**
		 * Converts a value to a string with 8bit character as hexadecimal notation.
		 * @param value The value to convert
		 * @param upperCases True, to use upper case characters; False, to use lower case characters
		 * @return Converted string
		 */
		static std::string toAStringHex(const unsigned int value, const bool upperCases = true);

		/**
		 * Converts a value to a string with 8bit character as hexadecimal notation.
		 * @param value The value to convert
		 * @param upperCases True, to use upper case characters; False, to use lower case characters
		 * @return Converted string
		 */
		static std::string toAStringHex(const long long value, const bool upperCases = true);

		/**
		 * Converts a value to a string with 8bit character as hexadecimal notation.
		 * @param value The value to convert
		 * @param upperCases True, to use upper case characters; False, to use lower case characters
		 * @return Converted string
		 */
		static std::string toAStringHex(const unsigned long long value, const bool upperCases = true);

		/**
		 * Converts a value to a string with 8bit character as hexadecimal notation.
		 * @param value The value to convert
		 * @param upperCases True, to use upper case characters; False, to use lower case characters
		 * @return Converted string
		 */
		static std::string toAStringHex(const long unsigned int value, const bool upperCases = true);

		/**
		 * Converts a value to a string with 8bit character as hexadecimal notation.
		 * @param value The value to convert
		 * @param upperCases True, to use upper case characters; False, to use lower case characters
		 * @return Converted string
		 */
		static std::string toAStringHex(const short value, const bool upperCases = true);

		/**
		 * Converts a value to a string with 8bit character as hexadecimal notation.
		 * @param value The value to convert
		 * @param upperCases True, to use upper case characters; False, to use lower case characters
		 * @return Converted string
		 */
		static std::string toAStringHex(const unsigned short value, const bool upperCases = true);

		/**
		 * Converts memory to a string with hexadecimal notation.
		 * @param data The memory to convert, can be nullptr if size == 0
		 * @param size The size of the memory in bytes, with range [0, infinity)
		 * @param upperCases True, to use upper case characters; False, to use lower case characters
		 * @return Converted string
		 */
		static std::string toAStringHex(const uint8_t* data, const size_t size, const bool upperCases = true);

		/**
		 * Converts memory to a string with hexadecimal notation in a reverse order.
		 * @param data The memory to convert, can be nullptr if size == 0
		 * @param size The size of the memory in bytes, with range [0, infinity)
		 * @param upperCases True, to use upper case characters; False, to use lower case characters
		 * @return Converted string
		 */
		static std::string toAStringHexReverse(const uint8_t* data, const size_t size, const bool upperCases = true);

		/**
		 * Converts a string to a string with lower characters only.
		 * @param value String value to convert, only containing ASCII characters
		 * @return Converted string.
		 */
		static std::string toLower(const std::string& value);

		/**
		 * Converts a string to a string with upper characters only.
		 * @param value String value to convert, only containing ASCII characters
		 * @return Converted string.
		 */
		static std::string toUpper(const std::string& value);

		/**
		 * Trims the front of a string.
		 * @param value String to be trimmed at the front
		 * @param character Trimming character
		 * @return Trimmed string
		 */
		static std::string trimFront(const std::string& value, const char character = ' ');

		/**
		 * Trims the back of a string.
		 * @param value String to be trimmed at the back
		 * @param character Trimming character
		 * @return Trimmed string
		 */
		static std::string trimBack(const std::string& value, const char character = ' ');

		/**
		 * Trims the front and the back of a string.
		 * @param value String to be trimmed
		 * @param character Trimming character
		 * @return Trimmed string
		 */
		static std::string trim(const std::string& value, const char character = ' ');

		/**
		 * Replaces a sub-string inside a string with a different string.
		 * @param value The string in which the replacement will take place, can be empty
		 * @param oldString The sub-string to be replaced, empty to replace nothing
		 * @param newString The sub-string which will be placed at the location of the old string, can be empty
		 * @param onlyFirstOccurrence True, to stop after the first replacement; False, to continue with the remaining string
		 * @return The resulting string with replaced sub-string(s)
		 */
		static std::string replace(std::string&& value, const std::string& oldString, const std::string& newString, const bool onlyFirstOccurrence);

		/**
		 * Replaces characters with a specified value by a new character.
		 * @param value The string in which the character will be replaced
		 * @param oldCharacter The character which will be replaced
		 * @param newCharacter The character which will be set at the position of the old characters
		 * @return The resulting string which replaced characters
		 * @see exchangeCharacters(), removeCharacters().
		 */
		static std::string replaceCharacters(const std::string& value, const char oldCharacter, const char newCharacter);

		/**
		 * Exchanges two characters within a string.
		 * @param value The string in which the character will be exchanged
		 * @param character0 One of both characters to exchange
		 * @param character1 The second character to exchange
		 * @return The resulting string with exchanged characters
		 * @see replaceCharacters(), removeCharacters().
		 */
		static std::string exchangeCharacters(const std::string& value, const char character0, const char character1);

		/**
		 * Removes characters with a specified value from a string.
		 * @param value The string from which the characters will be removed
		 * @param character The character which will be removed
		 * @return The resulting string without the specified character
		 * @see replaceCharacters(), exchangeCharacters().
		 */
		static std::string removeCharacters(const std::string& value, const char character);

		/**
		 * Inserts a specified character into a string in regular intervals.
		 * The specified character will neither be added to the front of the string nor to the end of the string.<br>
		 * This function can be used e.g., to add (thousands) separators to integer values.
		 * @param value The string to which the character will be inserted
		 * @param character The character to be inserted
		 * @param interval The number of string characters that will stay together (between each block the specified character will be inserted), with range [0, infinity)
		 * @param startAtFront True, to start at the front of the string; False, to start at the back of the string
		 * @return The modified string
		 * @tparam TChar The data type of the character of the string
		 */
		template <typename TChar>
		static std::basic_string<TChar> insertCharacter(const std::basic_string<TChar>& value, const TChar& character, size_t interval, bool startAtFront);

		/**
		 * Returns whether a given string stores a boolean value.
		 * A boolean value must be 'true' or 'false'.<br>
		 * Any beginning or ending white space will violate the value conditions, therefore provide a trimmed string value.
		 * @param stringValue String to be checked
		 * @param matchCase True, if the sting must match the lower case, otherwise any case is accepted
		 * @param acceptInteger True, if '1' and '0' will be accepted as 'true' and 'false'
		 * @param value Optional the value is returned
		 * @return True, if so
		 */
		static bool isBoolean(const std::string& stringValue, const bool matchCase = true, const bool acceptInteger = false, bool* value = nullptr);

		/**
		 * Returns whether a given string stores a 32bit integer value.
		 * An integer value may have a prefix sign ('+' or '-') followed by digits.<br>
		 * Any beginning or ending white space will violate the value conditions, therefore provide a trimmed string value.
		 * @param stringValue String to be checked
		 * @param value Optional the value is returned
		 * @return True, if so
		 */
		static bool isInteger32(const std::string& stringValue, int32_t* value = nullptr);

		/**
		 * Returns whether a given string stores an unsigned 64bit integer value.
		 * An integer value may have a prefix sign ('+') followed by digits.<br>
		 * Any beginning or ending white space will violate the value conditions, therefore provide a trimmed string value.
		 * @param stringValue String to be checked
		 * @param value Optional the value is returned
		 * @return True, if so
		 */
		static bool isUnsignedInteger64(const std::string& stringValue, uint64_t* value = nullptr);

		/**
		 * Returns whether a given string stores an hexadecimal value with up to 64 bits.
		 * The hexadecimal value may have a prefix "0x" and may contain up to 16 characters of digits or letters ['a', 'f'] or ['A' - 'F'].
		 * Any beginning or ending white space will violate the value conditions, therefore provide a trimmed string value.
		 * @param hexString The hexadecimal value given as string, must be valid
		 * @param length The number of characters the given string contains, with range [1, 18]
		 * @param needPrefix True, to accept hexadecimal values only if they start with "0x"; False, to accept also hexadecimal values with the prefix
		 * @param value Optional resulting decimal value of the given hexadecimal value; nullptr, if the value is not of interest
		 * @return True, if so
		 */
		static bool isHexValue64(const char* hexString, size_t length, const bool needPrefix = false, unsigned long long* value = nullptr);

		/**
		 * Returns whether a given string stores a number value.
		 * Any beginning or ending white space will violate the value conditions, therefore provide a trimmed string value.
		 * @param stringValue String to be checked
		 * @param acceptInteger True, if also strict integers will be accepted
		 * @param value Optional the value is returned
		 * @return True, if so
		 */
		static bool isNumber(const std::string& stringValue, const bool acceptInteger = false, double* value = nullptr);

		/**
		 * Converts a value to a string with 8bit character.
		 * @param value The value to convert
		 * @return Converted string
		 */
		static std::string toAString(const wchar_t value);

		/**
		 * Converts a value to a string with 8bit character.
		 * @param value The value to convert, may be nullptr
		 * @return Converted string
		 */
		static std::string toAString(const char* value);

		/**
		 * Converts a value to a string with 8bit character.
		 * @param value The value to convert, may be nullptr
		 * @return Converted string
		 */
		static std::string toAString(const wchar_t* value);

		/**
		 * Converts a value to a string with 8bit character.
		 * @param value The value to convert
		 * @return Converted string
		 */
		static std::string toAString(const std::string& value);

		/**
		 * Converts a value to a string with 8bit character.
		 * @param value The value to convert
		 * @return Converted string
		 */
		static std::string toAString(const std::wstring& value);

		/**
		 * Converts a value to a wstring.
		 * @param value The value to convert
		 * @return Converted string
		 */
		static std::wstring toWString(const char value);

		/**
		 * Converts a value to a wstring.
		 * @param value The value to convert, may be nullptr
		 * @return Converted string
		 */
		static std::wstring toWString(const char* value);

		/**
		 * Converts a value to a wstring.
		 * @param value The value to convert, may be nullptr
		 * @return Converted string
		 */
		static std::wstring toWString(const wchar_t* value);

		/**
		 * Converts a value to a wstring.
		 * @param value The value to convert, may be nullptr
		 * @return Converted string
		 */
		static std::wstring toWString(const unsigned short* value);

		/**
		 * Converts a value to a wstring.
		 * @param value The value to convert
		 * @return Converted string
		 */
		static std::wstring toWString(const unsigned char value);

		/**
		 * Converts a value to a wstring using a fixed notation.
		 * @param value The value to convert, with range (-infinity, infinity)
		 * @param precision Optional floating-point precision field, with range [1, infinity)
		 * @return Converted string
		 */
		static std::wstring toWString(const double value, const unsigned int precision = 5u);

		/**
		 * Converts a value to a wstring using a fixed notation.
		 * @param value The value to convert, with range (-infinity, infinity)
		 * @param precision Optional floating-point precision field, with range [1, infinity)
		 * @return Converted string
		 */
		static std::wstring toWString(const float value, const unsigned int precision = 5u);

		/**
		 * Converts a value to a wstring using a scientific notation.
		 * @param value The value to convert, with range (-infinity, infinity)
		 * @param precision Optional floating-point precision field, with range [1, infinity)
		 * @return Converted string
		 */
		static std::wstring toWStringScientific(const double value, const unsigned int precision = 5u);

		/**
		 * Converts a value to a wstring using a scientific notation.
		 * @param value The value to convert, with range (-infinity, infinity)
		 * @param precision Optional floating-point precision field, with range [1, infinity)
		 * @return Converted string
		 */
		static std::wstring toWStringScientific(const float value, const unsigned int precision = 5u);

		/**
		 * Converts a value to a wstring.
		 * @param value The value to convert
		 * @return Converted string
		 */
		static std::wstring toWString(const int value);

		/**
		 * Converts a value to a wstring.
		 * @param value The value to convert
		 * @return Converted string
		 */
		static std::wstring toWString(const unsigned int value);

		/**
		 * Converts a value to a wstring.
		 * @param value The value to convert
		 * @return Converted string
		 */
		static std::wstring toWString(const long value);

		/**
		 * Converts a value to a wstring.
		 * @param value The value to convert
		 * @return Converted string
		 */
		static std::wstring toWString(const long long value);

		/**
		 * Converts a value to a wstring.
		 * @param value The value to convert
		 * @return Converted string
		 */
		static std::wstring toWString(const unsigned long long value);

		/**
		 * Converts a value to a wstring.
		 * @param value The value to convert
		 * @return Converted string
		 */
		static std::wstring toWString(const short value);

		/**
		 * Converts a value to a wstring.
		 * @param value The value to convert
		 * @return Converted string
		 */
		static std::wstring toWString(const unsigned short value);

		/**
		 * Converts a value to a wstring.
		 * @param value The value to convert
		 * @return Converted string
		 */
		static std::wstring toWString(const long unsigned int value);

		/**
		 * Converts a value to a wstring.
		 * @param value The value to convert
		 * @return Converted string
		 */
		static std::wstring toWString(const wchar_t value);

		/**
		 * Converts a value to a wstring with a minimal number of characters.
		 * Zero (prefix) are used to fill the missing part.
		 * @param value The value to be converted
		 * @param minimalChars Number of minimal characters to be created
		 * @return Converted string
		 */
		static std::wstring toWString(const unsigned int value, const unsigned int minimalChars);

		/**
		 * Converts a value to a wstring.
		 * @param value The value to convert
		 * @return Converted string
		 */
		static std::wstring toWString(const std::string& value);

		/**
		 * Converts a value to a wstring.
		 * @param value The value to convert
		 * @return Converted string
		 */
		static std::wstring toWString(const std::wstring& value);

		/**
		 * Trims the front of a string.
		 * @param value String to be trimmed at the front
		 * @param character Trimming character
		 * @return Trimmed string
		 */
		static std::wstring trimFront(const std::wstring& value, const wchar_t character = L' ');

		/**
		 * Trims the back of a string.
		 * @param value String to be trimmed at the back
		 * @param character Trimming character
		 * @return Trimmed string
		 */
		static std::wstring trimBack(const std::wstring& value, const wchar_t character = L' ');

		/**
		 * Trims the front and the back of a string.
		 * @param value String to be trimmed
		 * @param character Trimming character
		 * @return Trimmed string
		 */
		static std::wstring trim(const std::wstring& value, const wchar_t character = L' ');

		/**
		 * Returns the specified text with all whitespace characters removed from the left and right side.
		 * Characters considered whitespace are:
		 *     - ' '  (0x20) space (SPC)
		 *     - '\\t' (0x09) horizontal tab (TAB)
		 *     - '\\n' (0x0a) newline (LF)
		 *     - '\\v' (0x0b) vertical tab (VT)
		 *     - '\\f' (0x0c) feed (FF)
		 *     - '\\r' (0x0d) carriage return (CR)
		 *     - '\0'  (0x00) the null terminator (NUL), for every null-terminator character at position '< std::string::size()'
		 * Further, the string trimmed string will not contain a null-terminator character in the middle of the string at position '< std::string::size()',<br>
		 * the only null terminator character will be at the end of the string at position 'std::string::size()'.
		 * @param text The text string to be trimmed
		 * @return Trimmed text string
		 * @see trimWhitespaceString
		 */
		static std::string trimWhitespace(const std::string& text);

		/**
		 * Returns the specified text with all whitespace characters removed from the left and right side.
		 ** Characters considered whitespace are:
		 *     - ' '  (0x20) space (SPC)
		 *     - '\\t' (0x09) horizontal tab (TAB)
		 *     - '\\n' (0x0a) newline (LF)
		 *     - '\\v' (0x0b) vertical tab (VT)
		 *     - '\\f' (0x0c) feed (FF)
		 *     - '\\r' (0x0d) carriage return (CR)
		 *     - '\0'  (0x00) the null terminator (NUL), for every null-terminator character at position '< std::string::size()'
		 * Further, the string trimmed string will not contain a null-terminator character in the middle of the string at position '< std::string::size()',<br>
		 * the only null terminator character will be at the end of the string at position 'std::string::size()'.
		 * @param text The text string to be trimmed
		 * @return Trimmed text string
		 * @see trimWhitespaceString
		 */
		static std::wstring trimWhitespace(const std::wstring& text);

		/**
		 * Replaces characters with a specified value by a new character.
		 * @param value The string in which the character will be replaced
		 * @param oldCharacter The character which will be replaced
		 * @param newCharacter The character which will be set at the position of the old characters
		 * @return The resulting string which replaced characters
		 * @see exchangeCharacters(), removeCharacters().
		 */
		static std::wstring replaceCharacters(const std::wstring& value, const wchar_t oldCharacter, const wchar_t newCharacter);

		/**
		 * Exchanges two characters within a string.
		 * @param value The string in which the character will be exchanged
		 * @param character0 One of both characters to exchange
		 * @param character1 The second character to exchange
		 * @return The resulting string with exchanged characters
		 * @see replaceCharacters(), removeCharacters().
		 */
		static std::wstring exchangeCharacters(const std::wstring& value, const wchar_t character0, const wchar_t character1);

		/**
		 * Removes characters with a specified value from a string.
		 * @param value The string from which the characters will be removed
		 * @param character The character which will be removed
		 * @return The resulting string without the specified character
		 * @see replaceCharacters(), exchangeCharacters().
		 */
		static std::wstring removeCharacters(const std::wstring& value, const wchar_t character);

	private:

		/**
		 * Converts a value of an arbitrary primitive type into a string (supported types are: ordinal types, floating point types, bool, string, char arrays)
		 * @param value The value that is to be converted
		 * @return Text representation of value
		 * @tparam TChar Data type of the characters of the C++ String
		 * @tparam T Type of value
		 */
		template <typename TChar, typename T>
		static std::basic_string<TChar> toString(const T& value);

		/**
		 * Returns the specified text with all whitespace characters removed from the left and right side.
		 * Characters considered whitespace are:
		 *     - ' '  (0x20) space (SPC)
		 *     - '\\t' (0x09) horizontal tab (TAB)
		 *     - '\\n' (0x0a) newline (LF)
		 *     - '\\v' (0x0b) vertical tab (VT)
		 *     - '\\f' (0x0c) feed (FF)
		 *     - '\\r' (0x0d) carriage return (CR)
		 *     - '\0'  (0x00) the null terminator (NUL), for every null-terminator character at position '< std::string::size()'
		 * Further, the string trimmed string will not contain a null-terminator character in the middle of the string at position '< std::string::size()',<br>
		 * the only null terminator character will be at the end of the string at position 'std::string::size()'.
		 * @param text The text string to be trimmed
		 * @tparam TChar Character data type of specified string, either use char or wchar_t
		 * @return Trimmed text string
		 */
		template <typename TChar>
		static std::basic_string<TChar> trimWhitespaceString(const std::basic_string<TChar>& text);
};

template <>
inline std::string String::toString(const bool& value)
{
	return value ? std::string("true") : std::string("false");
}

template <>
inline std::wstring String::toString(const bool& value)
{
	return value ? std::wstring(L"true") : std::wstring(L"false");
}

template <typename TChar>
std::basic_string<TChar> String::insertCharacter(const std::basic_string<TChar>& value, const TChar& character, size_t interval, bool startAtFront)
{
	ocean_assert(interval != 0);

	std::basic_string<TChar> result(value);

	if (startAtFront)
	{
		size_t position = interval;

		while (position < result.length())
		{
			result.insert(position, 1, character);
			position += interval + 1;
		}
	}
	else
	{
		std::ptrdiff_t position = std::ptrdiff_t(result.length()) - std::ptrdiff_t(interval);

		while (position >= 1)
		{
			result.insert(position, 1, character);
			position -= std::ptrdiff_t(interval);
		}
	}

	return result;
}

template <typename TChar, typename T>
std::basic_string<TChar> String::toString(const T& value)
{
	std::basic_ostringstream<TChar> stream;
	stream << value;

	return stream.str();
}

}

#endif // META_OCEAN_BASE_STRING_H
