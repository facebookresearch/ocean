/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/JSONConfig.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace IO
{

JSONConfig::JSONValue::JSONValue() :
	Value()
{
	valueType_ = TYPE_UNDEFINED;
}

JSONConfig::JSONValue::JSONValue(const std::string& value, const ValueType type) :
	Value(),
	valueString_(value)
{
	valueType_ = type;
}

JSONConfig::JSONValue::JSONValue(const JSONValues& values) :
	Value(),
	subValues_(values)
{
	valueType_ = TYPE_GROUP;
}

JSONConfig::JSONValue::~JSONValue()
{
	// nothing to do here
}

unsigned int JSONConfig::JSONValue::values() const
{
	unsigned int size = 0u;

	for (JSONValues::const_iterator i = subValues_.cbegin(); i != subValues_.cend(); ++i)
	{
		size += (unsigned int)(i->second.size());
	}

	return size;
}

unsigned int JSONConfig::JSONValue::values(const std::string& name) const
{
	const JSONValues::const_iterator i = subValues_.find(name);

	if (i == subValues_.cend())
	{
		return 0u;
	}

	return (unsigned int)(i->second.size());
}

bool JSONConfig::JSONValue::exist(const std::string& name) const
{
	return subValues_.find(name) != subValues_.end();
}

JSONConfig::JSONValue& JSONConfig::JSONValue::value(const std::string& name, const unsigned int index)
{
	JSONValues::iterator i = subValues_.find(name);

	if (i == subValues_.end() || index >= i->second.size())
	{
		return nullValue();
	}

	return i->second[index];
}

bool JSONConfig::JSONValue::value(const std::string& name, const unsigned int index, Value** value)
{
	ocean_assert(value);

	JSONValues::iterator i = subValues_.find(name);

	if (i == subValues_.end() || index >= i->second.size())
	{
		return false;
	}

	*value = &i->second[index];
	return true;
}

JSONConfig::JSONValue& JSONConfig::JSONValue::value(const unsigned int index, std::string& name)
{
	unsigned int n = 0u;

	for (JSONValues::iterator i = subValues_.begin(); i != subValues_.end(); ++i)
	{
		ocean_assert(n <= index);

		if (index < n + (unsigned int)i->second.size())
		{
			name = i->first;
			return i->second[index - n];
		}

		n += (unsigned int)i->second.size();
	}

	return nullValue();
}

bool JSONConfig::JSONValue::value(const unsigned int index, std::string& name, Value** value)
{
	ocean_assert(value != nullptr);

	unsigned int n = 0u;

	for (JSONValues::iterator i = subValues_.begin(); i != subValues_.end(); ++i)
	{
		ocean_assert(n <= index);

		if (index < n + (unsigned int)i->second.size())
		{
			name = i->first;
			*value = &(i->second[index - n]);

			return true;
		}

		n += (unsigned int)i->second.size();
	}

	return false;
}

JSONConfig::JSONValue& JSONConfig::JSONValue::add(const std::string& name)
{
	if (valueType_ == TYPE_GROUP || valueType_ == TYPE_UNDEFINED)
	{
		valueType_ = TYPE_GROUP;

		subValues_[name].push_back(JSONValue());
		return subValues_[name].back();
	}

	ocean_assert(false && "Invalid value type!");
	return *this;
}

bool JSONConfig::JSONValue::operator()(const bool value) const
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

int JSONConfig::JSONValue::operator()(const int value) const
{
	if (valueType_ == TYPE_INT)
	{
		return integerValue();
	}

	if (valueType_ == TYPE_NUMBER)
	{
		return NumericD::round32(numberValue());
	}

	return value;
}

double JSONConfig::JSONValue::operator()(const double value) const
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

std::string JSONConfig::JSONValue::operator()(const std::string& value) const
{
	if (valueType_ == TYPE_STRING)
	{
		return valueString_;
	}

	return value;
}

std::vector<bool> JSONConfig::JSONValue::operator()(const std::vector<bool>& value) const
{
	if (valueType_ == TYPE_MULTI_BOOL)
	{
		return boolValues();
	}

	return value;
}

std::vector<int> JSONConfig::JSONValue::operator()(const std::vector<int>& value) const
{
	if (valueType_ == TYPE_MULTI_INT)
	{
		return integerValues();
	}

	return value;
}

std::vector<double> JSONConfig::JSONValue::operator()(const std::vector<double>& value) const
{
	if (valueType_ == TYPE_MULTI_NUMBER)
	{
		return numberValues();
	}

	return value;
}

std::vector<std::string> JSONConfig::JSONValue::operator()(const std::vector<std::string>& value) const
{
	if (valueType_ == TYPE_MULTI_STRING)
	{
		return stringValues();
	}

	return value;
}

bool JSONConfig::JSONValue::operator=(const bool value)
{
	if (valueType_ == TYPE_GROUP || valueType_ == TYPE_INVALID)
	{
		return false;
	}

	valueType_ = TYPE_BOOL;
	valueString_ = value ? "true" : "false";

	return true;
}

bool JSONConfig::JSONValue::operator=(const int value)
{
	if (valueType_ == TYPE_GROUP || valueType_ == TYPE_INVALID)
	{
		return false;
	}

	valueType_ = TYPE_INT;
	valueString_ = String::toAString(value);

	return true;
}

bool JSONConfig::JSONValue::operator=(const double value)
{
	if (valueType_ == TYPE_GROUP || valueType_ == TYPE_INVALID)
	{
		return false;
	}

	valueType_ = TYPE_NUMBER;
	valueString_ = String::toAString(value, 16);

	return true;
}

bool JSONConfig::JSONValue::operator=(const std::string& value)
{
	if (valueType_ == TYPE_GROUP || valueType_ == TYPE_INVALID)
	{
		return false;
	}

	valueType_ = TYPE_STRING;
	valueString_ = value;

	return true;
}

bool JSONConfig::JSONValue::operator=(const std::vector<bool>& values)
{
	if (valueType_ == TYPE_GROUP || valueType_ == TYPE_INVALID)
	{
		return false;
	}

	valueType_ = TYPE_MULTI_BOOL;

	valueString_.clear();
	for (unsigned int n = 0; n < values.size(); ++n)
	{
		valueString_ += values[n] ? "true" : "false";

		if (n + 1 < values.size())
		{
			valueString_ += ",";
		}
	}

	return true;
}

bool JSONConfig::JSONValue::operator=(const std::vector<int>& values)
{
	if (valueType_ == TYPE_GROUP || valueType_ == TYPE_INVALID)
	{
		return false;
	}

	valueType_ = TYPE_MULTI_INT;

	valueString_.clear();
	for (unsigned int n = 0; n < values.size(); ++n)
	{
		valueString_ += String::toAString(values[n]);

		if (n + 1 < values.size())
		{
			valueString_ += ",";
		}
	}

	return true;
}

bool JSONConfig::JSONValue::operator=(const std::vector<double>& values)
{
	if (valueType_ == TYPE_GROUP || valueType_ == TYPE_INVALID)
	{
		return false;
	}

	valueType_ = TYPE_MULTI_NUMBER;

	valueString_.clear();
	for (unsigned int n = 0; n < values.size(); ++n)
	{
		valueString_ += String::toAString(values[n], 10);

		if (n + 1 < values.size())
		{
			valueString_ += ",";
		}
	}

	return true;
}

bool JSONConfig::JSONValue::operator=(const std::vector<std::string>& values)
{
	if (valueType_ == TYPE_GROUP || valueType_ == TYPE_INVALID)
	{
		return false;
	}

	valueType_ = TYPE_MULTI_STRING;

	valueString_.clear();
	for (unsigned int n = 0; n < values.size(); ++n)
	{
		valueString_ += std::string("\"") + values[n] + std::string("\"");

		if (n + 1 < values.size())
		{
			valueString_ += ",";
		}
	}

	return true;
}

JSONConfig::JSONValue& JSONConfig::JSONValue::operator[](const std::string& name)
{
	if ((valueType_ != TYPE_GROUP && valueType_ != TYPE_UNDEFINED) || name.empty())
	{
		return nullValue();
	}

	valueType_ = TYPE_GROUP;

	JSONValues::iterator i = subValues_.find(name);
	if (i == subValues_.end())
	{
		subValues_[name].push_back(JSONValue());
		return subValues_[name].back();
	}

	ocean_assert(i != subValues_.end());

	return i->second.front();
}

const JSONConfig::JSONValue& JSONConfig::JSONValue::operator[](const std::string& name) const
{
	if ((valueType_ != TYPE_GROUP && valueType_ != TYPE_UNDEFINED) || name.empty())
	{
		return nullValue();
	}

	JSONValues::const_iterator i = subValues_.find(name);

	if (i == subValues_.end())
	{
		return nullValue();
	}

	return i->second.front();
}

bool JSONConfig::JSONValue::boolValue() const
{
	ocean_assert(valueString_ == "true" || valueString_ == "false");
	return valueString_ == "true";
}

int JSONConfig::JSONValue::integerValue() const
{
	for (unsigned int n = 0; n < valueString_.length(); ++n)
	{
		ocean_assert((valueString_[n] >= '0' && valueString_[n] <= '9') || valueString_[n] == '-' || valueString_[n] == '+');
	}

	if (valueString_.empty())
	{
		return 0;
	}

	return atoi(valueString_.c_str());
}

double JSONConfig::JSONValue::numberValue() const
{
	if (valueString_.empty())
	{
		return 0.0;
	}

	return double(atof(valueString_.c_str()));
}

std::vector<bool> JSONConfig::JSONValue::boolValues() const
{
	if (valueString_.empty())
	{
		return std::vector<bool>();
	}

	std::string::size_type start = valueString_.find_first_not_of(' ');
	std::vector<bool> result;

	while (start < valueString_.size())
	{
		std::string::size_type stop = valueString_.find(',', start + 1);

		const std::string subString = valueString_.substr(start, stop - start);
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

std::vector<int> JSONConfig::JSONValue::integerValues() const
{
	if (valueString_.empty())
	{
		return std::vector<int>();
	}

	std::string::size_type start = valueString_.find_first_not_of(' ');
	std::vector<int> result;

	while (start < valueString_.size())
	{
		std::string::size_type stop = valueString_.find(',', start + 1);

		const std::string subString = valueString_.substr(start, stop - start);
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

std::vector<double> JSONConfig::JSONValue::numberValues() const
{
	if (valueString_.empty())
	{
		return std::vector<double>();
	}

	std::string::size_type start = valueString_.find_first_not_of(' ');
	std::vector<double> result;

	while (start < valueString_.size())
	{
		std::string::size_type stop = valueString_.find(',', start + 1);

		const std::string subString = valueString_.substr(start, stop - start);
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

std::vector<std::string> JSONConfig::JSONValue::stringValues() const
{
	if (valueString_.empty())
	{
		return std::vector<std::string>();
	}

	std::string::size_type start = valueString_.find('\"');
	std::vector<std::string> result;

	while (start < valueString_.size())
	{
		std::string::size_type stop = valueString_.find('\"', start + 1);
		if (stop == std::string::npos)
		{
			break;
		}

		const std::string subString = valueString_.substr(start + 1, stop - start - 1);
		if (subString.empty())
		{
			break;
		}

		result.push_back(subString);

		start = stop + 1;
	}

	return result;
}

JSONConfig::JSONScanner::JSONScanner(const std::shared_ptr<std::istream>& stream, float* progress, bool* cancel) :
	Scanner(stream, progress, cancel)
{
	registerSymbol("{", JS_NODE_BEGIN);
	registerSymbol("}", JS_NODE_END);
	registerSymbol("[", JS_ARRAY_BEGIN);
	registerSymbol("]", JS_ARRAY_END);
	registerSymbol(":", JS_COLON);
	registerSymbol(",", JS_COMMA);

	registerKeyword("true", JK_TRUE);
	registerKeyword("false", JK_FALSE);
	registerKeyword("null", JK_NULL);
}

JSONConfig::JSONConfig() :
	Config()
{
	// nothing to do here
}

JSONConfig::JSONConfig(const std::string& filename, const bool read) :
	Config(),
	filename_(filename)
{
	if (read)
	{
		JSONConfig::read();
	}
}

JSONConfig::JSONConfig(const std::shared_ptr<std::istream>& stream) :
	Config()
{
	ocean_assert(stream);

	JSONScanner scanner(stream);

	const JSONScanner::Token token = scanner.tokenPop();

	if (token.isSymbol(JSONScanner::JS_NODE_BEGIN))
	{
		parseNode(root_, scanner);
	}
}

bool JSONConfig::setFilename(const std::string& filename, const bool read)
{
	filename_ = filename;

	if (read)
	{
		return JSONConfig::read();
	}

	return true;
}

bool JSONConfig::read()
{
	if (filename_.empty())
	{
		return false;
	}

	JSONScanner scanner(std::shared_ptr<std::istream>(new std::ifstream(filename_, std::ios::binary)));

	if (!scanner.isValid())
	{
		return false;
	}

	const JSONScanner::Token token = scanner.tokenPop();

	if (!token.isSymbol(JSONScanner::JS_NODE_BEGIN))
	{
		return false;
	}

	return parseNode(root_, scanner);
}

bool JSONConfig::write()
{
	if (filename_.empty())
	{
		return false;
	}
	std::ofstream stream;
	stream.open(filename_.c_str(), std::ios::binary);

	if (!writeLine(std::string("{"), stream, 0u))
	{
		return false;
	}

	if (!writeNode(root_, stream, 1u))
	{
		return false;
	}

	if (!writeLine(std::string("}"), stream, 0u))
	{
		return false;
	}

	writeTimestamp_.toNow();

	return true;
}

unsigned int JSONConfig::values() const
{
	return root_.values();
}

unsigned int JSONConfig::values(const std::string& name) const
{
	return root_.values(name);
}

bool JSONConfig::exist(const std::string& name) const
{
	return root_.exist(name);
}

JSONConfig::JSONValue& JSONConfig::value(const unsigned int index, std::string& name)
{
	return root_.value(index, name);
}

bool JSONConfig::value(const unsigned int index, std::string& name, Value** value)
{
	return root_.value(index, name, value);
}

JSONConfig::JSONValue& JSONConfig::value(const std::string& name, const unsigned int index)
{
	return root_.value(name, index);
}

bool JSONConfig::value(const std::string& name, const unsigned int index, Value** value)
{
	return root_.value(name, index, value);
}

JSONConfig::JSONValue& JSONConfig::add(const std::string& name)
{
	return root_.add(name);
}

JSONConfig::JSONValue& JSONConfig::operator[](const std::string& name)
{
	return root_[name];
}

bool JSONConfig::parseNode(JSONValue& node, JSONScanner& scanner)
{
	// at this moment the node-begin symbol must have been popped from the scanner

	ocean_assert(!scanner.token().isSymbol(JSONScanner::JS_NODE_BEGIN));

	while (true)
	{
		JSONScanner::Token token = scanner.tokenPop();

		// check whether we have reached the end of this node
		if (token.isSymbol(JSONScanner::JS_NODE_END))
		{
			break;
		}

		// we simply ignore commas
		if (token.isSymbol(JSONScanner::JS_COMMA))
		{
			token = scanner.tokenPop();
		}

		// we have another field in this node beginning with an identifier

		if (!token.isString())
		{
			return false;
		}

		const std::string fieldName = token.string();

		token = scanner.tokenPop();

		// now we expect a colon before the value of the field starts

		if (!token.isSymbol(JSONScanner::JS_COLON))
		{
			return false;
		}

		token = scanner.tokenPop();

		// we either have a single value, an array or a sub-node as field

		if (token.isSymbol(JSONScanner::JS_ARRAY_BEGIN))
		{
			// arrays cannot be represented correctly so that we create multiple fields with same name

			if (!parseArray(node, fieldName, scanner))
			{
				return false;
			}

			continue;
		}

		JSONValue& field = node.add(fieldName);

		if (token.isSymbol(JSONScanner::JS_NODE_BEGIN))
		{
			if (!parseNode(field, scanner))
			{
				return false;
			}
		}
		else
		{
			// we have a single value: either "true", "false", "null" a string, or a number

			if (token.isKeyword(JSONScanner::JK_TRUE))
			{
				field = true;
			}
			else if (token.isKeyword(JSONScanner::JK_FALSE))
			{
				field = false;
			}
			else if (token.isKeyword(JSONScanner::JK_NULL))
			{
				// nothing to do here
			}
			else if (token.isString())
			{
				field = token.string();
			}
			else if (token.isIntegerOrNumber())
			{
				field = double(token.integerOrNumber());
			}
			else
			{
				// non of the above options, so it must be invalid
				return false;
			}
		}
	}

	return true;
}

bool JSONConfig::parseArray(JSONValue& node, const std::string& fieldName, JSONScanner& scanner)
{
	// at this moment the array-begin symbol must have been popped from the scanner

	if (scanner.token().isSymbol(JSONScanner::JS_ARRAY_BEGIN))
	{
		// we skip an array of arrays

		unsigned int scopes = 1u;

		while (scopes >= 1u)
		{
			const JSONScanner::Token token = scanner.tokenPop();

			if (token.isSymbol(JSONScanner::JS_ARRAY_BEGIN))
			{
				scopes++;
			}
			else if (token.isSymbol(JSONScanner::JS_ARRAY_END))
			{
				scopes--;
			}
			else if (token.isEndOfFile())
			{
				break;
			}
		}

		return true;
	}

	ocean_assert(!fieldName.empty());

	while (true)
	{
		JSONScanner::Token token = scanner.tokenPop();

		// check whether we have reached the end of this array
		if (token.isSymbol(JSONScanner::JS_ARRAY_END))
		{
			break;
		}

		// we simply ignore commas
		if (token.isSymbol(JSONScanner::JS_COMMA))
		{
			token = scanner.tokenPop();
		}

		JSONValue& field = node.add(fieldName);

		// we either have a single value, an array or a sub-node as field

		if (token.isSymbol(JSONScanner::JS_NODE_BEGIN))
		{
			if (!parseNode(field, scanner))
			{
				return false;
			}
		}
		else
		{
			// we have a single value: either "true", "false", "null" a string, or a number

			if (token.isKeyword(JSONScanner::JK_TRUE))
			{
				field = true;
			}
			else if (token.isKeyword(JSONScanner::JK_FALSE))
			{
				field = false;
			}
			else if (token.isKeyword(JSONScanner::JK_NULL))
			{
				// nothing to do here
			}
			else if (token.isString())
			{
				field = token.string();
			}
			else if (token.isIntegerOrNumber())
			{
				field = double(token.integerOrNumber());
			}
			else
			{
				// non of the above options, so it must be invalid
				return false;
			}
		}
	}

	return true;
}

bool JSONConfig::writeNode(JSONValue& node, std::ostream& stream, const unsigned int indentation)
{
	for (unsigned int n = 0u; n < node.values(); ++n)
	{
		const bool isLastValue = (n + 1u == node.values());
		const std::string lineEnd = (isLastValue ? "" : ",");

		std::string fieldName;
		JSONValue& field = node.value(n, fieldName);

		const std::string fieldNameString(std::string("\"") + fieldName + std::string("\" :"));

		if (field.type() == TYPE_GROUP)
		{
			if (!writeLine(fieldNameString + std::string(" {"), stream, indentation + 1u))
			{
				return false;
			}

			if (!writeNode(field, stream, indentation + 2u))
			{
				return false;
			}

			if (!writeLine(std::string("}") + lineEnd, stream, indentation + 1u))
			{
				return false;
			}
		}
		else if (field.type() == TYPE_MULTI_INT || field.type() == TYPE_MULTI_BOOL || field.type() == TYPE_MULTI_NUMBER || field.type() == TYPE_MULTI_STRING)
		{
			if (!writeLine(fieldNameString + std::string(" [ ") + field.valueString_ + std::string(" ]") + lineEnd, stream, indentation + 1u))
			{
				return false;
			}
		}
		else
		{
			switch (field.type())
			{
				case Config::TYPE_BOOL:
				{
					if (field(false))
					{
						if (!writeLine(fieldNameString + std::string(" true") + lineEnd, stream, indentation + 1u))
						{
							return false;
						}
					}
					else
					{
						if (!writeLine(fieldNameString + std::string(" false") + lineEnd, stream, indentation + 1u))
						{
							return false;
						}
					}

					break;
				}

				case Config::TYPE_NUMBER:
				{
					if (!writeLine(fieldNameString + std::string(" ") + String::toAString(field(0.0), 8u) + lineEnd, stream, indentation + 1u))
					{
						return false;
					}

					break;
				}

				case Config::TYPE_INT:
				{
					if (!writeLine(fieldNameString + std::string(" ") + String::toAString(field(0)) + lineEnd, stream, indentation + 1u))
					{
						return false;
					}

					break;
				}

				case Config::TYPE_STRING:
				{
					if (!writeLine(fieldNameString + std::string(" \"") + field(std::string("")) + std::string("\"") + lineEnd, stream, indentation + 1u))
					{
						return false;
					}

					break;
				}

				default:
					ocean_assert(false && "Missing implementation!");
					return false;
			}
		}
	}

	return true;
}

bool JSONConfig::writeLine(const std::string& string, std::ostream& stream, const unsigned int indentation)
{
	for (unsigned int n = 0u; n < indentation; ++n)
	{
		stream << ' ';
	}

	if (!stream.good())
	{
		return false;
	}

	stream << string << std::endl;

	return stream.good();
}

JSONConfig::JSONValue& JSONConfig::nullValue()
{
	static JSONConfig::JSONValue value;
	return value;
}

} // namespace IO

} // namespace Ocean
