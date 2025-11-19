/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_JSON_PARSER_H
#define META_OCEAN_IO_JSON_PARSER_H

#include "ocean/io/IO.h"
#include "ocean/io/Scanner.h"

#include <memory>
#include <variant>

namespace Ocean
{

namespace IO
{

/**
 * This class implements a JSON parser using the Scanner.
 * @ingroup io
 */
class OCEAN_IO_EXPORT JSONParser
{
	public:

		/**
		 * This class implements a JSON value that can hold different JSON types.
		 */
		class OCEAN_IO_EXPORT JSONValue
		{
			public:

				/**
				 * Definition of different JSON value types.
				 */
				enum Type : uint32_t
				{
					/// Invalid/uninitialized value.
					TYPE_INVALID = 0u,
					/// Null value.
					TYPE_NULL,
					/// Boolean value.
					TYPE_BOOLEAN,
					/// Number value.
					TYPE_NUMBER,
					/// String value.
					TYPE_STRING,
					/// Array value.
					TYPE_ARRAY,
					/// Object value.
					TYPE_OBJECT
				};

				/**
				 * Definition of a JSON array (vector of JSONValue).
				 */
				using Array = std::vector<JSONValue>;

				/**
				 * Definition of a JSON object (map of string keys to JSONValue).
				 */
				using ObjectMap = std::unordered_map<std::string, JSONValue>;

			protected:

				/**
				 * Definition of a unique pointer holding an array.
				 */
				using UniqueArray = std::unique_ptr<Array>;

				/**
				 * Definition of a unique pointer holding an object.
				 */
				using UniqueObjectMap = std::unique_ptr<ObjectMap>;

			public:

				/**
				 * Creates an invalid JSON value.
				 */
				JSONValue() = default;

				/**
				 * Copy constructor.
				 * @param other The value to copy
				 */
				JSONValue(const JSONValue& other);

				/**
				 * Move constructor.
				 * @param other The value to move
				 */
				JSONValue(JSONValue&& other) = default;

				/**
				 * Creates a null JSON value.
				 * @param value Must be nullptr
				 */
				explicit JSONValue(std::nullptr_t value);

				/**
				 * Creates a boolean JSON value.
				 * @param value The boolean value
				 */
				explicit JSONValue(bool value);

				/**
				 * Creates a number JSON value.
				 * @param value The number value
				 */
				explicit JSONValue(double value);

				/**
				 * Creates a string JSON value.
				 * @param value The string value
				 */
				explicit JSONValue(const std::string& value);

				/**
				 * Creates a string JSON value.
				 * @param value The string value
				 */
				explicit JSONValue(std::string&& value);

				/**
				 * Creates an array JSON value.
				 * @param value The array value
				 */
				explicit JSONValue(const Array& value);

				/**
				 * Creates an array JSON value.
				 * @param value The array value
				 */
				explicit JSONValue(Array&& value);

				/**
				 * Creates an object JSON value.
				 * @param value The object value
				 */
				explicit JSONValue(const ObjectMap& value);

				/**
				 * Creates an object JSON value.
				 * @param value The object value
				 */
				explicit JSONValue(ObjectMap&& value);

				/**
				 * Returns the type of this JSON value.
				 * @return The value type
				 */
				inline Type type() const;

				/**
				 * Returns whether this value is null.
				 * @return True, if so
				 */
				inline bool isNull() const;

				/**
				 * Returns whether this value is a boolean.
				 * @return True, if so
				 */
				inline bool isBoolean() const;

				/**
				 * Returns whether this value is a number.
				 * @return True, if so
				 */
				inline bool isNumber() const;

				/**
				 * Returns whether this value is a string.
				 * @return True, if so
				 */
				inline bool isString() const;

				/**
				 * Returns whether this value is an array.
				 * @return True, if so
				 */
				inline bool isArray() const;

				/**
				 * Returns whether this value is an object.
				 * @return True, if so
				 */
				inline bool isObject() const;

				/**
				 * Returns the boolean value.
				 * @return The boolean value, false if not a boolean
				 */
				bool boolean() const;

				/**
				 * Returns the number value.
				 * @return The number value, 0.0 if not a number
				 */
				double number() const;

				/**
				 * Returns the string value.
				 * @return The string value, empty if not a string
				 */
				const std::string& string() const;

				/**
				 * Returns the array value.
				 * @return The array value, empty if not an array
				 */
				const Array& array() const;

				/**
				 * Returns the object value.
				 * @return The object value, empty if not an object
				 */
				const ObjectMap& object() const;

				/**
				 * Extracts a string value from this object by key.
				 * @param key The key to look up
				 * @return Pointer to the string value, nullptr if this is not an object, key not found, or value is not a string
				 */
				const std::string* stringFromObject(const std::string& key) const;

				/**
				 * Extracts a number value from this object by key.
				 * @param key The key to look up
				 * @return Pointer to the number value, nullptr if this is not an object, key not found, or value is not a number
				 */
				const double* numberFromObject(const std::string& key) const;

				/**
				 * Extracts a boolean value from this object by key.
				 * @param key The key to look up
				 * @return Pointer to the boolean value, nullptr if this is not an object, key not found, or value is not a boolean
				 */
				const bool* booleanFromObject(const std::string& key) const;

				/**
				 * Extracts an array value from this object by key.
				 * @param key The key to look up
				 * @return Pointer to the array value, nullptr if this is not an object, key not found, or value is not an array
				 */
				const Array* arrayFromObject(const std::string& key) const;

				/**
				 * Extracts an object value from this object by key.
				 * @param key The key to look up
				 * @return Pointer to the object value, nullptr if this is not an object, key not found, or value is not an object
				 */
				const ObjectMap* objectFromObject(const std::string& key) const;

				/**
				 * Extracts a JSONValue from an object by key.
				 * @param key The key to look up
				 * @return Pointer to the JSONValue, nullptr if this is not an object or key not found
				 */
				const JSONValue* valueFromObject(const std::string& key) const;

				/**
				 * Returns whether this value is valid.
				 * @return True, if so
				 */
				inline bool isValid() const;

				/**
				 * Returns whether this value is valid.
				 * @return True, if so
				 */
				explicit inline operator bool() const;

				/**
				 * Copy assignment operator.
				 * @param other The value to copy
				 * @return Reference to this object
				 */
				JSONValue& operator=(const JSONValue& other);

				/**
				 * Move assignment operator.
				 * @param other The value to move
				 * @return Reference to this object
				 */
				JSONValue& operator=(JSONValue&& other) = default;

			protected:

				/// The type of this JSON value.
				Type type_ = TYPE_INVALID;

				/// The value data.
				std::variant<std::monostate, std::nullptr_t, bool, double, std::string, UniqueArray, UniqueObjectMap> data_;
		};

	protected:

		/**
		 * This class implements a JSON scanner that extends the base Scanner class.
		 * It registers JSON-specific keywords and symbols.
		 */
		class OCEAN_IO_EXPORT JSONScanner : public Scanner
		{
			public:

				/**
				 * Definition of JSON symbol IDs.
				 */
				enum SymbolId : uint32_t
				{
					/// Left brace '{'.
					SYMBOL_LEFT_BRACE = 0u,
					/// Right brace '}'.
					SYMBOL_RIGHT_BRACE,
					/// Left bracket '['.
					SYMBOL_LEFT_BRACKET,
					/// Right bracket ']'.
					SYMBOL_RIGHT_BRACKET,
					/// Colon ':'.
					SYMBOL_COLON,
					/// Comma ','.
					SYMBOL_COMMA
				};

				/**
				 * Definition of JSON keyword IDs.
				 */
				enum KeywordId : uint32_t
				{
					/// Keyword 'true'.
					KEYWORD_TRUE = 0u,
					/// Keyword 'false'.
					KEYWORD_FALSE,
					/// Keyword 'null'.
					KEYWORD_NULL
				};

			public:

				/**
				 * Creates a new JSON scanner using a stream as input.
				 * @param stream The stream to be used as input
				 * @param progress Optional resulting scanner progress in percent, with range [0, 1]
				 * @param cancel Optional scanner cancel flag
				 */
				explicit JSONScanner(const std::shared_ptr<std::istream>& stream, float* progress = nullptr, bool* cancel = nullptr);

				/**
				 * Creates a new JSON scanner using a file or a memory buffer as input.
				 * @param filename The name of the file to be used as input, `buffer` must be empty
				 * @param buffer The buffer to be used as input, `filename` must be empty
				 * @param progress Optional resulting scanner progress in percent, with range [0, 1]
				 * @param cancel Optional scanner cancel flag
				 */
				JSONScanner(const std::string& filename, const std::string& buffer, float* progress = nullptr, bool* cancel = nullptr);

				/**
				 * Creates a new JSON scanner using a file or a memory buffer as input.
				 * @param filename The name of the file to be used as input, `buffer` must be empty
				 * @param buffer The buffer to be used as input, `filename` must be empty
				 * @param progress Optional resulting scanner progress in percent, with range [0, 1]
				 * @param cancel Optional scanner cancel flag
				 */
				JSONScanner(const std::string& filename, std::string&& buffer, float* progress = nullptr, bool* cancel = nullptr);

			protected:

				/**
				 * Initializes the JSON scanner by registering keywords and symbols.
				 */
				void initialize();
		};

	public:

		/**
		 * Parses JSON from a stream.
		 * @param stream The stream containing JSON data
		 * @param errorMessage Optional resulting error message
		 * @return The parsed JSON value, invalid on error
		 */
		static JSONValue parse(const std::shared_ptr<std::istream>& stream, std::string* errorMessage = nullptr);

		/**
		 * Parses JSON from a file or buffer.
		 * @param filename The name of the file to parse, `buffer` must be empty
		 * @param buffer The buffer to parse, `filename` must be empty
		 * @param errorMessage Optional resulting error message
		 * @return The parsed JSON value, invalid on error
		 */
		static JSONValue parse(const std::string& filename, const std::string& buffer, std::string* errorMessage = nullptr);

		/**
		 * Parses JSON from a file or buffer.
		 * @param filename The name of the file to parse, `buffer` must be empty
		 * @param buffer The buffer to parse, `filename` must be empty
		 * @param errorMessage Optional resulting error message
		 * @return The parsed JSON value, invalid on error
		 */
		static JSONValue parse(const std::string& filename, std::string&& buffer, std::string* errorMessage = nullptr);

	protected:

		/**
		 * Parses a JSON value.
		 * @param scanner The JSON scanner
		 * @param errorMessage Optional resulting error message
		 * @return The parsed JSON value, invalid on error
		 */
		static JSONValue parseValue(JSONScanner& scanner, std::string* errorMessage);

		/**
		 * Parses a JSON object.
		 * @param scanner The JSON scanner
		 * @param errorMessage Optional resulting error message
		 * @return The parsed JSON object value, invalid on error
		 */
		static JSONValue parseObject(JSONScanner& scanner, std::string* errorMessage);

		/**
		 * Parses a JSON array.
		 * @param scanner The JSON scanner
		 * @param errorMessage Optional resulting error message
		 * @return The parsed JSON array value, invalid on error
		 */
		static JSONValue parseArray(JSONScanner& scanner, std::string* errorMessage);

		/**
		 * Creates an error message with line and column information.
		 * @param scanner The scanner with position information
		 * @param message The error message
		 * @return The formatted error message
		 */
		static std::string createErrorMessage(const JSONScanner& scanner, const std::string& message);
};

inline JSONParser::JSONValue::Type JSONParser::JSONValue::type() const
{
	return type_;
}

inline bool JSONParser::JSONValue::isNull() const
{
	return type_ == TYPE_NULL;
}

inline bool JSONParser::JSONValue::isBoolean() const
{
	return type_ == TYPE_BOOLEAN;
}

inline bool JSONParser::JSONValue::isNumber() const
{
	return type_ == TYPE_NUMBER;
}

inline bool JSONParser::JSONValue::isString() const
{
	return type_ == TYPE_STRING;
}

inline bool JSONParser::JSONValue::isArray() const
{
	return type_ == TYPE_ARRAY;
}

inline bool JSONParser::JSONValue::isObject() const
{
	return type_ == TYPE_OBJECT;
}

inline bool JSONParser::JSONValue::isValid() const
{
	return type_ != TYPE_INVALID;
}

inline JSONParser::JSONValue::operator bool() const
{
	return isValid();
}

}

}

#endif // META_OCEAN_IO_JSON_PARSER_H
