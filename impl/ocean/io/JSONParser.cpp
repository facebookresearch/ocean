/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/JSONParser.h"

namespace Ocean
{

namespace IO
{

JSONParser::JSONValue::JSONValue(const JSONValue& other) :
	type_(other.type_),
	data_()
{
	switch (type_)
	{
		case TYPE_INVALID:
			// data_ is already default-constructed (monostate)
			break;

		case TYPE_NULL:
			data_ = std::get<std::nullptr_t>(other.data_);
			break;

		case TYPE_BOOLEAN:
			data_ = std::get<bool>(other.data_);
			break;

		case TYPE_NUMBER:
			data_ = std::get<double>(other.data_);
			break;

		case TYPE_STRING:
			data_ = std::get<std::string>(other.data_);
			break;

		case TYPE_ARRAY:
			data_ = std::make_unique<Array>(*std::get<UniqueArray>(other.data_));
			break;

		case TYPE_OBJECT:
			data_ = std::make_unique<ObjectMap>(*std::get<UniqueObjectMap>(other.data_));
			break;
	}
}

JSONParser::JSONValue::JSONValue(std::nullptr_t value) :
	type_(TYPE_NULL),
	data_(value)
{
	// nothing to do here
}

JSONParser::JSONValue::JSONValue(bool value) :
	type_(TYPE_BOOLEAN),
	data_(value)
{
	// nothing to do here
}

JSONParser::JSONValue::JSONValue(double value) :
	type_(TYPE_NUMBER),
	data_(value)
{
	// nothing to do here
}

JSONParser::JSONValue::JSONValue(const std::string& value) :
	type_(TYPE_STRING),
	data_(value)
{
	// nothing to do here
}

JSONParser::JSONValue::JSONValue(std::string&& value) :
	type_(TYPE_STRING),
	data_(std::move(value))
{
	// nothing to do here
}

JSONParser::JSONValue::JSONValue(const Array& value) :
	type_(TYPE_ARRAY),
	data_(std::make_unique<Array>(value))
{
	// nothing to do here
}

JSONParser::JSONValue::JSONValue(Array&& value) :
	type_(TYPE_ARRAY),
	data_(std::make_unique<Array>(std::move(value)))
{
	// nothing to do here
}

JSONParser::JSONValue::JSONValue(const ObjectMap& value) :
	type_(TYPE_OBJECT),
	data_(std::make_unique<ObjectMap>(value))
{
	// nothing to do here
}

JSONParser::JSONValue::JSONValue(ObjectMap&& value) :
	type_(TYPE_OBJECT),
	data_(std::make_unique<ObjectMap>(std::move(value)))
{
	// nothing to do here
}

bool JSONParser::JSONValue::boolean() const
{
	if (type_ == TYPE_BOOLEAN)
	{
		return std::get<bool>(data_);
	}

	return false;
}

double JSONParser::JSONValue::number() const
{
	if (type_ == TYPE_NUMBER)
	{
		return std::get<double>(data_);
	}

	return 0.0;
}

const std::string& JSONParser::JSONValue::string() const
{
	static const std::string emptyString;

	if (type_ == TYPE_STRING)
	{
		return std::get<std::string>(data_);
	}

	return emptyString;
}

const JSONParser::JSONValue::Array& JSONParser::JSONValue::array() const
{
	static const Array emptyArray;

	if (type_ == TYPE_ARRAY)
	{
		return *std::get<UniqueArray>(data_);
	}

	return emptyArray;
}

const JSONParser::JSONValue::ObjectMap& JSONParser::JSONValue::object() const
{
	static const ObjectMap emptyObject;

	if (type_ == TYPE_OBJECT)
	{
		return *std::get<UniqueObjectMap>(data_);
	}

	return emptyObject;
}

const std::string* JSONParser::JSONValue::stringFromObject(const std::string& key) const
{
	if (type_ != TYPE_OBJECT)
	{
		return nullptr;
	}

	const ObjectMap& objectMap = *std::get<UniqueObjectMap>(data_);
	const ObjectMap::const_iterator iString = objectMap.find(key);

	if (iString == objectMap.end() || !iString->second.isString())
	{
		return nullptr;
	}

	return &std::get<std::string>(iString->second.data_);
}

const double* JSONParser::JSONValue::numberFromObject(const std::string& key) const
{
	if (type_ != TYPE_OBJECT)
	{
		return nullptr;
	}

	const ObjectMap& objectMap = *std::get<UniqueObjectMap>(data_);
	const ObjectMap::const_iterator iNumber = objectMap.find(key);

	if (iNumber == objectMap.end() || !iNumber->second.isNumber())
	{
		return nullptr;
	}

	return std::get_if<double>(&iNumber->second.data_);
}

const bool* JSONParser::JSONValue::booleanFromObject(const std::string& key) const
{
	if (type_ != TYPE_OBJECT)
	{
		return nullptr;
	}

	const ObjectMap& objectMap = *std::get<UniqueObjectMap>(data_);
	const ObjectMap::const_iterator iBoolean = objectMap.find(key);

	if (iBoolean == objectMap.end() || !iBoolean->second.isBoolean())
	{
		return nullptr;
	}

	return std::get_if<bool>(&iBoolean->second.data_);
}

const JSONParser::JSONValue::Array* JSONParser::JSONValue::arrayFromObject(const std::string& key) const
{
	if (type_ != TYPE_OBJECT)
	{
		return nullptr;
	}

	const ObjectMap& objectMap = *std::get<UniqueObjectMap>(data_);
	const ObjectMap::const_iterator iArray = objectMap.find(key);

	if (iArray == objectMap.end() || !iArray->second.isArray())
	{
		return nullptr;
	}

	return std::get<UniqueArray>(iArray->second.data_).get();
}

const JSONParser::JSONValue::ObjectMap* JSONParser::JSONValue::objectFromObject(const std::string& key) const
{
	if (type_ != TYPE_OBJECT)
	{
		return nullptr;
	}

	const ObjectMap& objectMap = *std::get<UniqueObjectMap>(data_);
	const ObjectMap::const_iterator iObject = objectMap.find(key);

	if (iObject == objectMap.end() || !iObject->second.isObject())
	{
		return nullptr;
	}

	return std::get<UniqueObjectMap>(iObject->second.data_).get();
}

const JSONParser::JSONValue* JSONParser::JSONValue::valueFromObject(const std::string& key) const
{
	if (type_ != TYPE_OBJECT)
	{
		return nullptr;
	}

	const ObjectMap& objectMap = *std::get<UniqueObjectMap>(data_);
	const ObjectMap::const_iterator iValue = objectMap.find(key);

	if (iValue == objectMap.end())
	{
		return nullptr;
	}

	return &iValue->second;
}

JSONParser::JSONValue& JSONParser::JSONValue::operator=(const JSONValue& other)
{
	if (this != &other)
	{
		type_ = other.type_;

		switch (type_)
		{
			case TYPE_INVALID:
				data_ = std::monostate{};
				break;

			case TYPE_NULL:
				data_ = std::get<std::nullptr_t>(other.data_);
				break;

			case TYPE_BOOLEAN:
				data_ = std::get<bool>(other.data_);
				break;

			case TYPE_NUMBER:
				data_ = std::get<double>(other.data_);
				break;

			case TYPE_STRING:
				data_ = std::get<std::string>(other.data_);
				break;

			case TYPE_ARRAY:
				data_ = std::make_unique<Array>(*std::get<UniqueArray>(other.data_));
				break;

			case TYPE_OBJECT:
				data_ = std::make_unique<ObjectMap>(*std::get<UniqueObjectMap>(other.data_));
				break;
		}
	}

	return *this;
}

JSONParser::JSONScanner::JSONScanner(const std::shared_ptr<std::istream>& stream, float* progress, bool* cancel) :
	Scanner(stream, progress, cancel)
{
	initialize();
}

JSONParser::JSONScanner::JSONScanner(const std::string& filename, const std::string& buffer, float* progress, bool* cancel) :
	Scanner(filename, buffer, progress, cancel)
{
	initialize();
}

JSONParser::JSONScanner::JSONScanner(const std::string& filename, std::string&& buffer, float* progress, bool* cancel) :
	Scanner(filename, std::move(buffer), progress, cancel)
{
	initialize();
}

void JSONParser::JSONScanner::initialize()
{
	// Register JSON symbols
	registerSymbol("{", SYMBOL_LEFT_BRACE);
	registerSymbol("}", SYMBOL_RIGHT_BRACE);
	registerSymbol("[", SYMBOL_LEFT_BRACKET);
	registerSymbol("]", SYMBOL_RIGHT_BRACKET);
	registerSymbol(":", SYMBOL_COLON);
	registerSymbol(",", SYMBOL_COMMA);

	// Register JSON keywords
	registerKeyword("true", KEYWORD_TRUE);
	registerKeyword("false", KEYWORD_FALSE);
	registerKeyword("null", KEYWORD_NULL);
}

JSONParser::JSONValue JSONParser::parse(const std::shared_ptr<std::istream>& stream, const bool strict, std::string* errorMessage)
{
	JSONScanner scanner(stream);

	if (!scanner.isValid())
	{
		if (errorMessage != nullptr)
		{
			*errorMessage = "Failed to create scanner from stream";
		}

		return JSONValue();
	}

	return parseValue(scanner, strict, errorMessage);
}

JSONParser::JSONValue JSONParser::parse(const std::string& filename, const std::string& buffer, const bool strict, std::string* errorMessage)
{
	JSONScanner scanner(filename, buffer);

	if (!scanner.isValid())
	{
		if (errorMessage != nullptr)
		{
			*errorMessage = "Failed to create scanner from " + (filename.empty() ? "buffer" : "file: " + filename);
		}

		return JSONValue();
	}

	return parseValue(scanner, strict, errorMessage);
}

JSONParser::JSONValue JSONParser::parse(const std::string& filename, std::string&& buffer, const bool strict, std::string* errorMessage)
{
	JSONScanner scanner(filename, std::move(buffer));

	if (!scanner.isValid())
	{
		if (errorMessage != nullptr)
		{
			*errorMessage = "Failed to create scanner from " + (filename.empty() ? "buffer" : "file: " + filename);
		}

		return JSONValue();
	}

	return parseValue(scanner, strict, errorMessage);
}

JSONParser::JSONValue JSONParser::parseValue(JSONScanner& scanner, const bool strict, std::string* errorMessage)
{
	const Scanner::Token& currentToken = scanner.token();

	// Check for keywords: true, false, null
	if (currentToken.isKeyword())
	{
		const uint32_t keywordId = currentToken.keyword();

		if (keywordId == JSONScanner::KEYWORD_TRUE)
		{
			scanner.pop();
			return JSONValue(true);
		}
		else if (keywordId == JSONScanner::KEYWORD_FALSE)
		{
			scanner.pop();
			return JSONValue(false);
		}
		else if (keywordId == JSONScanner::KEYWORD_NULL)
		{
			scanner.pop();
			return JSONValue(nullptr);
		}
	}

	// Check for string
	if (currentToken.isString())
	{
		std::string value = currentToken.string();
		scanner.pop();
		return JSONValue(std::move(value));
	}

	// Check for number (integer or floating point)
	if (currentToken.isIntegerOrNumber())
	{
		const double value = currentToken.integerOrNumber();
		scanner.pop();
		return JSONValue(value);
	}

	// Check for object
	if (currentToken.isSymbol(JSONScanner::SYMBOL_LEFT_BRACE))
	{
		return parseObject(scanner, strict, errorMessage);
	}

	// Check for array
	if (currentToken.isSymbol(JSONScanner::SYMBOL_LEFT_BRACKET))
	{
		return parseArray(scanner, strict, errorMessage);
	}

	// Check for end of file
	if (currentToken.isEndOfFile())
	{
		if (errorMessage != nullptr)
		{
			*errorMessage = createErrorMessage(scanner, "Unexpected end of file");
		}

		return JSONValue();
	}

	// Unexpected token
	if (errorMessage != nullptr)
	{
		*errorMessage = createErrorMessage(scanner, "Unexpected token: '" + currentToken.raw() + "'");
	}

	return JSONValue();
}

JSONParser::JSONValue JSONParser::parseObject(JSONScanner& scanner, const bool strict, std::string* errorMessage)
{
	ocean_assert(scanner.token().isSymbol(JSONScanner::SYMBOL_LEFT_BRACE));

	// Consume the '{'
	scanner.pop();

	JSONValue::ObjectMap objectMap;

	// Check for empty object
	if (scanner.token().isSymbol(JSONScanner::SYMBOL_RIGHT_BRACE))
	{
		scanner.pop();
		return JSONValue(std::move(objectMap));
	}

	while (true)
	{
		// Expect a string key
		if (!scanner.token().isString())
		{
			if (errorMessage != nullptr)
			{
				*errorMessage = createErrorMessage(scanner, "Expected string key in object, got: '" + scanner.token().raw() + "'");
			}

			return JSONValue();
		}

		std::string key = scanner.token().string();
		scanner.pop();

		// Expect a colon
		if (!scanner.token().isSymbol(JSONScanner::SYMBOL_COLON))
		{
			if (errorMessage != nullptr)
			{
				*errorMessage = createErrorMessage(scanner, "Expected ':' after object key, got: '" + scanner.token().raw() + "'");
			}

			return JSONValue();
		}

		scanner.pop();

		// Parse the value
		JSONValue value = parseValue(scanner, strict, errorMessage);

		if (!value)
		{
			return JSONValue();
		}

		// Insert the key-value pair
		objectMap[std::move(key)] = std::move(value);

		// Check for comma or closing brace
		if (scanner.token().isSymbol(JSONScanner::SYMBOL_RIGHT_BRACE))
		{
			scanner.pop();
			break;
		}

		if (scanner.token().isSymbol(JSONScanner::SYMBOL_COMMA))
		{
			scanner.pop();

			// After comma, we must have another key-value pair, no trailing commas allowed in strict mode
			if (scanner.token().isSymbol(JSONScanner::SYMBOL_RIGHT_BRACE))
			{
				if (strict)
				{
					if (errorMessage != nullptr)
					{
						*errorMessage = createErrorMessage(scanner, "Trailing comma in object");
					}

					return JSONValue();
				}
				else
				{
					// allow trailing comma

					scanner.pop();
					break;
				}
			}

			continue;
		}

		// Unexpected token
		if (errorMessage != nullptr)
		{
			*errorMessage = createErrorMessage(scanner, "Expected ',' or '}' in object, got: '" + scanner.token().raw() + "'");
		}

		return JSONValue();
	}

	return JSONValue(std::move(objectMap));
}

JSONParser::JSONValue JSONParser::parseArray(JSONScanner& scanner, const bool strict, std::string* errorMessage)
{
	ocean_assert(scanner.token().isSymbol(JSONScanner::SYMBOL_LEFT_BRACKET));

	// Consume the '['
	scanner.pop();

	JSONValue::Array array;

	// Check for empty array
	if (scanner.token().isSymbol(JSONScanner::SYMBOL_RIGHT_BRACKET))
	{
		scanner.pop();
		return JSONValue(std::move(array));
	}

	while (true)
	{
		// Parse the value
		JSONValue value = parseValue(scanner, strict, errorMessage);

		if (!value)
		{
			return JSONValue();
		}

		array.push_back(std::move(value));

		// Check for comma or closing bracket
		if (scanner.token().isSymbol(JSONScanner::SYMBOL_RIGHT_BRACKET))
		{
			scanner.pop();
			break;
		}

		if (scanner.token().isSymbol(JSONScanner::SYMBOL_COMMA))
		{
			scanner.pop();

			// After comma, we must have another value, no trailing commas allowed in strict mode
			if (scanner.token().isSymbol(JSONScanner::SYMBOL_RIGHT_BRACKET))
			{
				if (strict)
				{
					if (errorMessage != nullptr)
					{
						*errorMessage = createErrorMessage(scanner, "Trailing comma in array");
					}

					return JSONValue();
				}
				else
				{
					// allow trailing comma

					scanner.pop();
					break;
				}
			}

			continue;
		}

		// Unexpected token
		if (errorMessage != nullptr)
		{
			*errorMessage = createErrorMessage(scanner, "Expected ',' or ']' in array, got: '" + scanner.token().raw() + "'");
		}

		return JSONValue();
	}

	return JSONValue(std::move(array));
}

std::string JSONParser::createErrorMessage(const JSONScanner& scanner, const std::string& message)
{
	std::string result = "JSON parsing error";

	if (!scanner.filename().empty())
	{
		result += " in file '" + scanner.filename() + "'";
	}

	result += " at line " + std::to_string(scanner.line());
	result += ", column " + std::to_string(scanner.column());
	result += ": " + message;

	return result;
}

}

}
