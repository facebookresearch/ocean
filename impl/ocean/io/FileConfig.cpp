/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/FileConfig.h"

#include "ocean/base/Exception.h"
#include "ocean/base/String.h"

#include <fstream>
#include <iostream>

namespace Ocean
{

namespace IO
{

FileConfig::FileValue::FileValue(const std::string& value, const ValueType type) :
	Value(),
	string_(value)
{
	valueType_ = type;
}

FileConfig::FileValue::FileValue(const FileValues& values) :
	Value(),
	subValues_(values)
{
	valueType_ = TYPE_GROUP;
}

FileConfig::FileValue::~FileValue()
{
	// nothing to do here
}

unsigned int FileConfig::FileValue::values() const
{
	return (unsigned int)(subValues_.size());
}

unsigned int FileConfig::FileValue::values(const std::string& name) const
{
	return (unsigned int)(subValues_.count(name));
}

bool FileConfig::FileValue::exist(const std::string& name) const
{
	return subValues_.find(name) != subValues_.end();
}

FileConfig::FileValue& FileConfig::FileValue::value(const unsigned int index, std::string& name)
{
	FileValues::iterator i = subValues_.begin();

	unsigned int n = 0;
	while (n != index)
	{
		if (i == subValues_.end())
		{
			return nullValue();
		}

		++i;
		++n;
	}

	if (i == subValues_.end())
	{
		return nullValue();
	}

	name = i->first;
	return i->second;
}

bool FileConfig::FileValue::value(const unsigned int index, std::string& name, Value** value)
{
	ocean_assert(value);

	FileValues::iterator i = subValues_.begin();

	unsigned int n = 0;
	while (n != index)
	{
		if (i == subValues_.end())
		{
			return false;
		}

		++i;
		++n;
	}

	if (i == subValues_.end())
	{
		return false;
	}

	name = i->first;
	*value = &i->second;
	return true;
}

FileConfig::FileValue& FileConfig::FileValue::value(const std::string& name, const unsigned int index)
{
	std::pair<FileValues::iterator, FileValues::iterator> range = subValues_.equal_range(name);
	FileValues::iterator i = range.first;

	unsigned int n = 0;
	while (n != index)
	{
		if (i == range.second)
		{
			return nullValue();
		}

		++i;
		++n;
	}

	if (i == subValues_.end())
	{
		return nullValue();
	}

	return i->second;
}

bool FileConfig::FileValue::value(const std::string& name, const unsigned int index, Value** value)
{
	ocean_assert(value);

	std::pair<FileValues::iterator, FileValues::iterator> range = subValues_.equal_range(name);
	FileValues::iterator i = range.first;

	unsigned int n = 0;
	while (n != index)
	{
		if (i == range.second)
		{
			return false;
		}

		++i;
		++n;
	}

	if (i == subValues_.end())
	{
		return false;
	}

	*value = &i->second;
	return true;
}

FileConfig::FileValue& FileConfig::FileValue::add(const std::string& name)
{
	if (valueType_ == TYPE_GROUP || valueType_ == TYPE_UNDEFINED)
	{
		valueType_ = TYPE_GROUP;

		FileValues::iterator i = subValues_.insert(std::make_pair(name, FileValue()));
		return i->second;
	}

	ocean_assert(false && "Invalid value type!");
	return *this;
}

bool FileConfig::FileValue::operator()(const bool value) const
{
	if (valueType_ == TYPE_BOOL)
	{
		return boolValue();
	}

	if (valueType_ == TYPE_INT)
	{
		return integerValue() != 0;
	}

	return value;
}

int FileConfig::FileValue::operator()(const int value) const
{
	if (valueType_ == TYPE_INT)
	{
		return integerValue();
	}

	return value;
}

double FileConfig::FileValue::operator()(const double value) const
{
	if (valueType_ == TYPE_NUMBER)
	{
		return numberValue();
	}

	if (valueType_ == TYPE_INT)
	{
		return double(integerValue());
	}

	return value;
}

std::string FileConfig::FileValue::operator()(const std::string& value) const
{
	if (valueType_ == TYPE_STRING)
	{
		return string_;
	}

	return value;
}

std::vector<bool> FileConfig::FileValue::operator()(const std::vector<bool>& value) const
{
	if (valueType_ == TYPE_MULTI_BOOL)
	{
		return boolValues();
	}

	return value;
}

std::vector<int> FileConfig::FileValue::operator()(const std::vector<int>& value) const
{
	if (valueType_ == TYPE_MULTI_INT)
	{
		return integerValues();
	}

	return value;
}

std::vector<double> FileConfig::FileValue::operator()(const std::vector<double>& value) const
{
	if (valueType_ == TYPE_MULTI_NUMBER)
	{
		return numberValues();
	}

	return value;
}

std::vector<std::string> FileConfig::FileValue::operator()(const std::vector<std::string>& value) const
{
	if (valueType_ == TYPE_MULTI_STRING)
	{
		return stringValues();
	}

	return value;
}

bool FileConfig::FileValue::operator=(const bool value)
{
	if (valueType_ == TYPE_GROUP || valueType_ == TYPE_INVALID)
	{
		return false;
	}

	valueType_ = TYPE_BOOL;
	string_ = value ? "true" : "false";

	return true;
}

bool FileConfig::FileValue::operator=(const int value)
{
	if (valueType_ == TYPE_GROUP || valueType_ == TYPE_INVALID)
	{
		return false;
	}

	valueType_ = TYPE_INT;
	string_ = String::toAString(value);

	return true;
}

bool FileConfig::FileValue::operator=(const double value)
{
	if (valueType_ == TYPE_GROUP || valueType_ == TYPE_INVALID)
	{
		return false;
	}

	valueType_ = TYPE_NUMBER;
	string_ = String::toAString(value, 10);

	return true;
}

bool FileConfig::FileValue::operator=(const std::string& value)
{
	if (valueType_ == TYPE_GROUP || valueType_ == TYPE_INVALID)
	{
		return false;
	}

	valueType_ = TYPE_STRING;
	string_ = value;

	return true;
}

bool FileConfig::FileValue::operator=(const std::vector<bool>& values)
{
	if (valueType_ == TYPE_GROUP || valueType_ == TYPE_INVALID)
	{
		return false;
	}

	valueType_ = TYPE_MULTI_BOOL;

	string_.clear();
	for (unsigned int n = 0; n < values.size(); ++n)
	{
		string_ += values[n] ? "true" : "false";

		if (n + 1 < values.size())
		{
			string_ += " ";
		}
	}

	return true;
}

bool FileConfig::FileValue::operator=(const std::vector<int>& values)
{
	if (valueType_ == TYPE_GROUP || valueType_ == TYPE_INVALID)
	{
		return false;
	}

	valueType_ = TYPE_MULTI_INT;

	string_.clear();
	for (unsigned int n = 0; n < values.size(); ++n)
	{
		string_ += String::toAString(values[n]);

		if (n + 1 < values.size())
		{
			string_ += " ";
		}
	}

	return true;
}

bool FileConfig::FileValue::operator=(const std::vector<double>& values)
{
	if (valueType_ == TYPE_GROUP || valueType_ == TYPE_INVALID)
	{
		return false;
	}

	valueType_ = TYPE_MULTI_NUMBER;

	string_.clear();
	for (unsigned int n = 0; n < values.size(); ++n)
	{
		string_ += String::toAString(values[n], 10);

		if (n + 1 < values.size())
		{
			string_ += " ";
		}
	}

	return true;
}

bool FileConfig::FileValue::operator=(const std::vector<std::string>& values)
{
	if (valueType_ == TYPE_GROUP || valueType_ == TYPE_INVALID)
	{
		return false;
	}

	valueType_ = TYPE_MULTI_STRING;

	string_.clear();
	for (unsigned int n = 0; n < values.size(); ++n)
	{
		string_ += std::string("\"") + values[n] + std::string("\"");

		if (n + 1 < values.size())
		{
			string_ += " ";
		}
	}

	return true;
}

FileConfig::Value& FileConfig::FileValue::operator[](const std::string& name)
{
	if ((valueType_ != TYPE_GROUP && valueType_ != TYPE_UNDEFINED) || name.empty())
	{
		return nullValue();
	}

	valueType_ = TYPE_GROUP;

	FileValues::iterator i = subValues_.find(name);
	if (i == subValues_.end())
	{
		i = subValues_.insert(std::make_pair(name, FileValue()));
	}

	ocean_assert(i != subValues_.end());

	return i->second;
}

bool FileConfig::FileValue::boolValue() const
{
	ocean_assert(string_ == "true" || string_ == "false");
	return string_ == "true";
}

int FileConfig::FileValue::integerValue() const
{
	for (unsigned int n = 0; n < string_.length(); ++n)
	{
		ocean_assert((string_[n] >= '0' && string_[n] <= '9') || string_[n] == '-' || string_[n] == '+');
	}

	if (string_.empty())
	{
		return 0;
	}

	return atoi(string_.c_str());
}

double FileConfig::FileValue::numberValue() const
{
	if (string_.empty())
	{
		return 0.0;
	}

	return double(atof(string_.c_str()));
}

std::vector<bool> FileConfig::FileValue::boolValues() const
{
	if (string_.empty())
	{
		return std::vector<bool>();
	}

	std::string::size_type start = string_.find_first_not_of(' ');
	std::vector<bool> result;

	while (start < string_.size())
	{
		std::string::size_type stop = string_.find(' ', start + 1);

		const std::string subString = string_.substr(start, stop - start);
		if (subString.empty())
		{
			break;
		}

		ocean_assert(subString == "true" || subString == "false");

		const bool value = subString == "true";
		result.push_back(value);

		if (stop == std::string::npos)
		{
			break;
		}

		start = stop;
	}

	return result;
}

std::vector<int> FileConfig::FileValue::integerValues() const
{
	if (string_.empty())
	{
		return std::vector<int>();
	}

	std::string::size_type start = string_.find_first_not_of(' ');
	std::vector<int> result;

	while (start < string_.size())
	{
		std::string::size_type stop = string_.find(' ', start + 1);

		const std::string subString = string_.substr(start, stop - start);
		if (subString.empty())
		{
			break;
		}

		const int value = atoi(subString.c_str());
		result.push_back(value);

		if (stop == std::string::npos)
		{
			break;
		}

		start = stop;
	}

	return result;
}

std::vector<double> FileConfig::FileValue::numberValues() const
{
	if (string_.empty())
	{
		return std::vector<double>();
	}

	std::string::size_type start = string_.find_first_not_of(' ');
	std::vector<double> result;

	while (start < string_.size())
	{
		std::string::size_type stop = string_.find(' ', start + 1);

		const std::string subString = string_.substr(start, stop - start);
		if (subString.empty())
		{
			break;
		}

		const double value = atof(subString.c_str());
		result.push_back(value);

		if (stop == std::string::npos)
		{
			break;
		}

		start = stop;
	}

	return result;
}

std::vector<std::string> FileConfig::FileValue::stringValues() const
{
	if (string_.empty())
	{
		return std::vector<std::string>();
	}

	std::string::size_type start = string_.find('\"');
	std::vector<std::string> result;

	while (start < string_.size())
	{
		std::string::size_type stop = string_.find('\"', start + 1);
		if (stop == std::string::npos)
		{
			break;
		}

		const std::string subString = string_.substr(start + 1, stop - start - 1);
		if (subString.empty())
		{
			break;
		}

		result.push_back(subString);

		start = stop + 1;
	}

	return result;
}

FileConfig::ConfigScanner::ConfigScanner(const std::string& filename) :
	Scanner(filename, std::string(), nullptr, nullptr)
{
	registerSymbol("{", SYMBOL_OPEN);
	registerSymbol("}", SYMBOL_CLOSE);
	registerSymbol("[", SYMBOL_BEGIN);
	registerSymbol("]", SYMBOL_END);

	registerKeyword("true", KEYWORD_TRUE);
	registerKeyword("false", KEYWORD_FALSE);
	registerKeyword("TRUE", KEYWORD_TRUE);
	registerKeyword("FALSE", KEYWORD_FALSE);

	registerLineRemark("//");
	registerScopeRemark("/*", "*/");
}

FileConfig::FileConfig() :
	Config()
{
	// nothing to do here
}


FileConfig::FileConfig(const std::string& filename, const bool read) :
	Config(),
	filename_(filename)
{
	if (read)
	{
		FileConfig::read();
	}
}

bool FileConfig::setFilename(const std::string& filename, const bool read)
{
	filename_ = filename;

	if (read)
	{
		return FileConfig::read();
	}

	return true;
}

bool FileConfig::read()
{
	try
	{
		if (filename_.empty())
		{
			return false;
		}

		ConfigScanner configScanner(filename_);

		if (!configScanner.isValid())
		{
			Log::warning() << "Failed to load file: \"" << filename_ << "\".";
			return false;
		}

		values_.clear();
		return readSubValues(values_, configScanner, false);
	}
	catch(const Exception& exception)
	{
		Log::error() << "Failed to load config file: \"" << filename_ << "\" :" << exception.what();
	}

	return false;
}

bool FileConfig::write()
{
	if (filename_.empty())
	{
		return false;
	}

	std::ofstream stream;
	stream.open(filename_.c_str(), std::ios::binary);

	return write(values_, stream);
}

unsigned int FileConfig::values() const
{
	return (unsigned int)(values_.size());
}

unsigned int FileConfig::values(const std::string& name) const
{
	return (unsigned int)(values_.count(name));
}

bool FileConfig::exist(const std::string& name) const
{
	return values_.find(name) != values_.end();
}

FileConfig::FileValue& FileConfig::value(const unsigned int index, std::string& name)
{
	FileValues::iterator i = values_.begin();

	unsigned int n = 0;
	while (n != index)
	{
		if (i == values_.end())
		{
			return nullValue();
		}

		++i;
		++n;
	}

	if (i == values_.end())
	{
		return nullValue();
	}

	name = i->first;
	return i->second;
}

bool FileConfig::value(const unsigned int index, std::string& name, Value** value)
{
	ocean_assert(value);

	FileValues::iterator i = values_.begin();

	unsigned int n = 0;
	while (n != index)
	{
		if (i == values_.end())
		{
			return false;
		}

		++i;
		++n;
	}

	if (i == values_.end())
	{
		return false;
	}

	name = i->first;
	*value = &i->second;

	return true;
}

FileConfig::FileValue& FileConfig::value(const std::string& name, const unsigned int index)
{
	std::pair<FileValues::iterator, FileValues::iterator> range = values_.equal_range(name);
	FileValues::iterator i = range.first;

	unsigned int n = 0;
	while (n != index)
	{
		if (i == range.second)
		{
			return nullValue();
		}

		++i;
		++n;
	}

	if (i == range.second)
	{
		return nullValue();
	}

	return i->second;
}

bool FileConfig::value(const std::string& name, const unsigned int index, Value** value)
{
	ocean_assert(value);

	std::pair<FileValues::iterator, FileValues::iterator> range = values_.equal_range(name);
	FileValues::iterator i = range.first;

	unsigned int n = 0;
	while (n != index)
	{
		if (i == range.second)
		{
			return false;
		}

		++i;
		++n;
	}

	if (i == range.second)
	{
		return false;
	}

	*value = &i->second;
	return true;
}

FileConfig::FileValue& FileConfig::add(const std::string& name)
{
	FileValues::iterator i = values_.insert(std::make_pair(name, FileValue()));
	return i->second;
}

FileConfig::FileValue& FileConfig::operator[](const std::string& name)
{
	FileValues::iterator i = values_.find(name);
	if (i == values_.end())
	{
		i = values_.insert(std::make_pair(name, FileValue()));
	}

	ocean_assert(i != values_.end());

	return i->second;
}

bool FileConfig::write(const FileValues& values, std::ostream& stream)
{
	return write(values, stream, 0);
}

bool FileConfig::readSubValues(FileValues& values, Scanner& scanner, bool stopWithBrackets)
{
	while (true)
	{
		const Scanner::Token nameToken = scanner.token();
		scanner.pop();

		if (nameToken.isSymbol(ConfigScanner::SYMBOL_CLOSE))
		{
			return stopWithBrackets == true;
		}

		const Scanner::Token valueToken = scanner.token();
		scanner.pop();

		if (nameToken.isEndOfFile())
		{
			return stopWithBrackets == false;
		}

		if (nameToken.isIdentifier() == false)
		{
			return false;
		}

		if (valueToken.isEndOfFile())
		{
			return false;
		}


		if (valueToken.isNumber())
		{
			values.insert(std::make_pair(nameToken.identifier(), FileValue(valueToken.raw(), TYPE_NUMBER)));
		}
		else if (valueToken.isInteger())
		{
			values.insert(std::make_pair(nameToken.identifier(), FileValue(valueToken.raw(), TYPE_INT)));
		}
		else if (valueToken.isString())
		{
			values.insert(std::make_pair(nameToken.identifier(), FileValue(valueToken.raw(), TYPE_STRING)));
		}
		else if (valueToken.isIdentifier())
		{
			values.insert(std::make_pair(nameToken.identifier(), FileValue(valueToken.raw(), TYPE_STRING)));
		}
		else if (valueToken.isKeyword(ConfigScanner::KEYWORD_TRUE))
		{
			values.insert(std::make_pair(nameToken.identifier(), FileValue("true", TYPE_BOOL)));
		}
		else if (valueToken.isKeyword(ConfigScanner::KEYWORD_FALSE))
		{
			values.insert(std::make_pair(nameToken.identifier(), FileValue("false", TYPE_BOOL)));
		}
		else if (valueToken.isSymbol(ConfigScanner::SYMBOL_BEGIN))
		{
			ValueType multiValueType = TYPE_UNDEFINED;
			std::string valueString;

			while (true)
			{
				const Scanner::Token multiValueToken = scanner.token();
				scanner.pop();

				if (multiValueToken.isSymbol(ConfigScanner::SYMBOL_END) || multiValueToken.isEndOfFile())
				{
					break;
				}

				if (!valueString.empty())
				{
					valueString += " ";
				}

				if (multiValueToken.isNumber() && (multiValueType == TYPE_UNDEFINED || multiValueType == TYPE_MULTI_NUMBER))
				{
					valueString += multiValueToken.raw();
					multiValueType = TYPE_MULTI_NUMBER;
				}
				else if (multiValueToken.isInteger() && (multiValueType == TYPE_UNDEFINED || multiValueType == TYPE_MULTI_INT))
				{
					valueString += multiValueToken.raw();
					multiValueType = TYPE_MULTI_INT;
				}
				else if (multiValueToken.isString() && (multiValueType == TYPE_UNDEFINED || multiValueType == TYPE_MULTI_STRING))
				{
					valueString += std::string("\"") + multiValueToken.raw() + std::string("\"");
					multiValueType = TYPE_MULTI_STRING;
				}
				else if (valueToken.isKeyword(ConfigScanner::KEYWORD_TRUE) && (multiValueType == TYPE_UNDEFINED || multiValueType == TYPE_MULTI_BOOL))
				{
					valueString += "true";
					multiValueType = TYPE_MULTI_BOOL;
				}
				else if (valueToken.isKeyword(ConfigScanner::KEYWORD_FALSE) && (multiValueType == TYPE_UNDEFINED || multiValueType == TYPE_MULTI_BOOL))
				{
					valueString += "false";
					multiValueType = TYPE_MULTI_BOOL;
				}
			}

			if (multiValueType != TYPE_UNDEFINED)
			{
				values.insert(std::make_pair(nameToken.identifier(), FileValue(valueString, multiValueType)));
			}
		}
		else if (valueToken.isSymbol(ConfigScanner::SYMBOL_OPEN))
		{
			FileValues newSubValues;

			if (readSubValues(newSubValues, scanner, true))
			{
				values.insert(std::make_pair(nameToken.identifier(), FileValue(newSubValues)));
			}
			else
			{
				return false;
			}
		}
		else
		{
			ocean_assert(false && "Unhandled token.");
		}
	}
}

bool FileConfig::write(const FileValues& values, std::ostream& stream, unsigned int offset)
{
	for (FileValues::const_iterator i = values.begin(); i != values.end(); ++i)
	{
		if (i->second.type() == TYPE_GROUP)
		{
			if (!writeTabs(stream, offset))
			{
				return false;
			}

			stream << i->first.c_str() << "\n";

			if (!writeTabs(stream, offset))
			{
				return false;
			}

			stream << "{" << "\n";

			if (!write(i->second.subValues_, stream, offset + 1))
			{
				return false;
			}

			if (!writeTabs(stream, offset))
			{
				return false;
			}

			stream << "}" << "\n";
		}
		else if (i->second.type() != TYPE_INVALID && i->second.type() != TYPE_UNDEFINED)
		{
			if (!writeTabs(stream, offset))
			{
				return false;
			}

			stream << i->first.c_str() << " ";

			if (i->second.type() == TYPE_STRING)
			{
				stream << "\"";
			}
			else if (i->second.isMultiType())
			{
				stream << "[ ";
			}

			stream << i->second.string_.c_str();

			if (i->second.type() == TYPE_STRING)
			{
				stream << "\"";
			}
			else if (i->second.isMultiType())
			{
				stream << " ]";
			}

			stream << "\n";

			if (stream.fail())
			{
				return false;
			}
		}
	}

	return true;
}

bool FileConfig::writeTabs(std::ostream& stream, unsigned int offset)
{
	for (unsigned int n = 0; n < offset; n++)
	{
		stream << "\t";
	}

	return stream.fail() == false;
}

FileConfig::FileValue& FileConfig::nullValue()
{
	static FileConfig::FileValue value;
	return value;
}

} // namespace IO

} // namespace Ocean
