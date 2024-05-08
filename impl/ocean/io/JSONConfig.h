/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_JSON_CONFIG_H
#define META_OCEAN_IO_JSON_CONFIG_H

#include "ocean/io/IO.h"
#include "ocean/io/Scanner.h"

#include "ocean/base/Config.h"

namespace Ocean
{

namespace IO
{

/**
 * This class implements a configuration toolkit based on JSON.
 * @ingroup io
 */
class OCEAN_IO_EXPORT JSONConfig final : public Config
{
	public:

		// Forward declaration
		class JSONValue;

		/**
		 * Definition of a map mapping value names to value objects.
		 */
		typedef std::unordered_map<std::string, std::vector<JSONValue>> JSONValues;

		/**
		 * Definition of a JSON value object.
		 */
		class OCEAN_IO_EXPORT JSONValue final : public Value
		{
			friend class JSONConfig;

			public:

				/**
				 * Creates a new value object.
				 */
				JSONValue();

				/**
				 * Default copy constructor.
				 * @param value The value to copy
				 */
				JSONValue(const JSONValue& value) = default;

				/**
				 * Creates a new value object by a single value as string and the real value type.
				 * @param value String value
				 * @param type Value type
				 */
				JSONValue(const std::string& value, const ValueType type);

				/**
				 * Creates a new value object holding sub values.
				 * @param values Sub values
				 */
				explicit JSONValue(const JSONValues& values);

				/**
				 * Destructs a value object.
				 */
				~JSONValue() override;

				/**
				 * Returns the number of sub values.
				 * @see Config::Value::values().
				 */
				unsigned int values() const override;

				/**
				 * Returns the number of sub values with a given name.
				 * @param name The name of the JSON node
				 * @see Config::Value::values().
				 */
				unsigned int values(const std::string& name) const override;

				/**
				 * Returns whether this value holds at least one specified sub value.
				 * @param name The name of the JSON node
				 * @see Config::Value::exist().
				 */
				bool exist(const std::string& name) const override;

				/**
				 * Returns a sub value specified by it's name and it's index if more than one value exists with the same name.
				 * @param name The name of the JSON node
				 * @param index The index of the specific node in case multiple nodes with the same name exist, range: [0, infinity)
				 * @return A reference to the node with the specified name, will be an empty node if the name doesn't exist
				 * @see Config::Value::value().
				 */
				JSONValue& value(const std::string& name, const unsigned int index) override;

				/**
				 * Returns a sub value specified by it's name and it's index if more than one value exists with the same name.
				 * @param name The name of the JSON node, must be valid
				 * @param index The index of the specific node in case multiple nodes with the same name exist, range: [0, infinity)
				 * @param value Pointer to the memory where the value of the selected node will be copied to
				 * @return True if the node with the specified name exists, otherwise false
				 * @see Config::Value::value().
				 */
				bool value(const std::string& name, const unsigned int index, Value** value) override;

				/**
				 * Returns a sub value specified by it's index.
				 * @param index The index of the specific node , range: [0, N), where N = number of nodes
				 * @param name The name of the JSON node at index @c index, will be unchanged if that node doesn't exist
				 * @return A reference to the node with the specified name, will be an empty node if the name doesn't exist
				 * @see Config::Value::value().
				 */
				JSONValue& value(const unsigned int index, std::string& name) override;

				/**
				 * Returns a sub value specified by it's index.
				 * @param index The index of the specific node , range: [0, N), where N = number of nodes
				 * @param name The name of the JSON node, must be valid
				 * @param value Pointer to the memory where the value of the selected node will be copied to
				 * @return True if the node with the specified name exists, otherwise false
				 * @see Config::Value::value().
				 */
				bool value(const unsigned int index, std::string& name, Value** value) override;

				/**
				 * Adds a new sub value specified by it's name.
				 * @param name The name of the sub value to create
				 * @return New sub value
				 */
				JSONValue& add(const std::string& name) override;

				/**
				 * Returns this value as boolean.
				 * @param value The value that is returned in case the type of the internal value doesn't match (or cannot be cast into) the type of this parameter/return type of this function
				 * @return The internal value of this node
				 * @see Config::Value::operator().
				 */
				bool operator()(const bool value) const override;

				/**
				 * Returns this value as integer.
				 * @param value The value that is returned in case the type of the internal value doesn't match (or cannot be cast into) the type of this parameter/return type of this function
				 * @return The internal value of this node
				 * @see Config::Value::operator().
				 */
				int operator()(const int value) const override;

				/**
				 * Returns this value as number.
				 * @param value The value that is returned in case the type of the internal value doesn't match (or cannot be cast into) the type of this parameter/return type of this function
				 * @return The internal value of this node
				 * @see Config::Value::operator().
				 */
				double operator()(const double value) const override;

				/**
				 * Returns this value as string.
				 * @param value The value that is returned in case the type of the internal value doesn't match (or cannot be cast into) the type of this parameter/return type of this function
				 * @return The internal value of this node
				 * @see Config::Value::operator().
				 */
				std::string operator()(const std::string& value) const override;

				/**
				 * Returns this value as multi boolean.
				 * @param value The value that is returned in case the type of the internal value doesn't match (or cannot be cast into) the type of this parameter/return type of this function
				 * @return The internal value of this node
				 * @see Config::Value::operator().
				 */
				std::vector<bool> operator()(const std::vector<bool>& value) const override;

				/**
				 * Returns this value as multi integer.
				 * @param value The value that is returned in case the type of the internal value doesn't match (or cannot be cast into) the type of this parameter/return type of this function
				 * @return The internal value of this node
				 * @see Config::Value::operator().
				 */
				std::vector<int> operator()(const std::vector<int>& value) const override;

				/**
				 * Returns this value as multi number.
				 * @param value The value that is returned in case the type of the internal value doesn't match (or cannot be cast into) the type of this parameter/return type of this function
				 * @return The internal value of this node
				 * @see Config::Value::operator().
				 */
				std::vector<double> operator()(const std::vector<double>& value) const override;

				/**
				 * Returns this value as multi string.
				 * @param value The value that is returned in case the type of the internal value doesn't match (or cannot be cast into) the type of this parameter/return type of this function
				 * @return The internal value of this node
				 * @see Config::Value::operator().
				 */
				std::vector<std::string> operator()(const std::vector<std::string>& value) const override;

				/**
				 * Sets this value as boolean.
				 * @param value Will set the internal value and value type of this node, node must be valid and not a group
				 * @return True if the value could be set, otherwise false
				 * @see Config::Value::operator=().
				 */
				bool operator=(const bool value) override;

				/**
				 * Sets this value as integer.
				 * @param value Will set the internal value and value type of this node, node must be valid and not a group
				 * @return True if the value could be set, otherwise false
				 * @see Config::Value::operator=().
				 */
				bool operator=(const int value) override;

				/**
				 * Sets this value as number.
				 * @param value Will set the internal value and value type of this node, node must be valid and not a group. Up to 10 decimals of the value will be stored.
				 * @return True if the value could be set, otherwise false
				 * @see Config::Value::operator=().
				 */
				bool operator=(const double value) override;

				/**
				 * Sets this value as string.
				 * @param value Will set the internal value and value type of this node, node must be valid and not a group
				 * @return True if the value could be set, otherwise false
				 * @see Config::Value::operator=().
				 */
				bool operator=(const std::string& value) override;

				/**
				 * Sets this value as multi boolean.
				 * @param values Will set the internal value and value type of this node, node must be valid and not a group
				 * @return True if the value could be set, otherwise false
				 * @see Config::Value::operator=().
				 */
				bool operator=(const std::vector<bool>& values) override;

				/**
				 * Sets this value as multi integer.
				 * @param values Will set the internal value and value type of this node, node must be valid and not a group
				 * @return True if the value could be set, otherwise false
				 * @see Config::Value::operator=().
				 */
				bool operator=(const std::vector<int>& values) override;

				/**
				 * Sets this value as multi number.
				 * @param values Will set the internal value and value type of this node, node must be valid and not a group
				 * @return True if the value could be set, otherwise false
				 * @see Config::Value::operator=().
				 */
				bool operator=(const std::vector<double>& values) override;

				/**
				 * Sets this value as multi string.
				 * @param values Will set the internal value and value type of this node, node must be valid and not a group
				 * @return True if the value could be set, otherwise false
				 * @see Config::Value::operator=().
				 */
				bool operator=(const std::vector<std::string>& values) override;

				/**
				 * Returns a sub value specified by it's name
				 * @param name The name of the JSON node to retrieve
				 * @return A reference to the node with the specified name; will be an empty node if the name doesn't exist
				 * @see Config::Value::operator[]().
				 */
				JSONValue& operator[](const std::string& name) override;

				/**
				 * Returns a sub value specified by it's name
				 * @param name The name of the JSON node to retrieve
				 * @return A constant reference to the node with the specified name; will be an empty node if the name doesn't exist
				 * @see Config::Value::operator[]().
				 */
				const JSONValue& operator[](const std::string& name) const;

			private:

				/**
				 * Returns the boolean value of this object.
				 * @return Parsed boolean value
				 */
				bool boolValue() const;

				/**
				 * Returns the integer value of this object.
				 * @return Parsed integer value
				 */
				int integerValue() const;

				/**
				 * Returns the number value of this object.
				 * @return Parsed number value
				 */
				double numberValue() const;

				/**
				 * Returns the boolean value of this object.
				 * @return Parsed boolean value
				 */
				std::vector<bool> boolValues() const;

				/**
				 * Returns the integer values of this object.
				 * @return Parsed integer value
				 */
				std::vector<int> integerValues() const;

				/**
				 * Returns the number values of this object.
				 * @return Parsed number value
				 */
				std::vector<double> numberValues() const;

				/**
				 * Returns the string values of this object.
				 * @return Parsed number value
				 */
				std::vector<std::string> stringValues() const;

			private:

				/// Value as string.
				std::string valueString_;

				/// Sub values if this value is a group.
				JSONValues subValues_;
		};

	protected:

		/**
		 * This class implements a scanner for JSON files.
		 */
		class JSONScanner : public Scanner
		{
			public:

				/**
				 * Definition of individual JSON symbols.
				 */
				enum JSONSymbol
				{
					/// Node begin symbol: '{'.
					JS_NODE_BEGIN,
					/// Node end symbol: '}'.
					JS_NODE_END,
					/// Array begin symbol: '['.
					JS_ARRAY_BEGIN,
					/// Node end symbol: ']'.
					JS_ARRAY_END,
					/// Colon symbol: '.'
					JS_COLON,
					/// Comma symbol: ','
					JS_COMMA
				};

				/**
				 * Definition of individual JSON keywords.
				 */
				enum JSONKeyword
				{
					/// Keyword true.
					JK_TRUE,
					/// Keyword false.
					JK_FALSE,
					/// Keyword null.
					JK_NULL
				};

			public:

				/**
				 * Creates a new scanner by a given filename.
				 * @param stream The input stream from which the JSON data is read, must be valid
				 * @param progress Optional scanner progress value
				 * @param cancel Optional scanner cancel state
				 * @see Scanner::Scanner().
				 */
				explicit JSONScanner(const std::shared_ptr<std::istream>& stream, float* progress = nullptr, bool* cancel = nullptr);
		};

	public:

		/**
		 * Creates a new config object.
		 */
		JSONConfig();

		/**
		 * Creates a new config object with a specified configuration file.
		 * @param filename The name of the file associated with the new config object
		 * @param read True, to load the data from the file directly
		 * @see read().
		 */
		explicit JSONConfig(const std::string& filename, const bool read = true);

		/**
		 * Creates a new config object with a specified input stream.
		 * @param stream The input stream from which the JSON data is read immediately, must be valid
		 */
		explicit JSONConfig(const std::shared_ptr<std::istream>& stream);

		/**
		 * Returns the config file.
		 * @return Config file
		 */
		inline const std::string& filename() const;

		/**
		 * Sets the filename of the new config objects.
		 * All old config objects will be released before.
		 * @param filename to set
		 * @param read True, to load the file directly
		 * @return True, if the file could be loaded
		 */
		bool setFilename(const std::string& filename, const bool read = true);

		/**
		 * Reads / loads all values of this configuration.
		 * @see Config::read().
		 */
		bool read() override;

		/**
		 * Writes / saves all values of this configuration.
		 * @see Config::write().
		 */
		bool write() override;

		/**
		 * Returns the number of sub values.
		 * @see Config::values().
		 */
		unsigned int values() const override;

		/**
		 * Returns the number of sub values with a given name.
		 * @see Config::values().
		 */
		unsigned int values(const std::string& name) const override;

		/**
		 * Returns whether this value holds at least one specified sub value.
		 * @see Config::exist().
		 */
		bool exist(const std::string& name) const override;

		/**
		 * Returns a sub value specified by it's index.
		 * @see Config::value().
		 */
		JSONValue& value(const unsigned int index, std::string& name) override;

		/**
		 * Returns a sub value specified by it's index.
		 * @see Config::value().
		 */
		bool value(const unsigned int index, std::string& name, Value** value) override;

		/**
		 * Returns a sub value specified by it's name and it's index if more than one value exists with the same name.
		 * @see Config::value().
		 */
		JSONValue& value(const std::string& name, const unsigned int index) override;

		/**
		 * Returns a sub value specified by it's name and it's index if more than one value exists with the same name.
		 * @see Config::value().
		 */
		bool value(const std::string& name, const unsigned int index, Value** value) override;

		/**
		 * Adds a new sub value specified by it's name.
		 * @param name The name of the sub value to create
		 * @return New sub value
		 */
		JSONValue& add(const std::string& name) override;

		/**
		 * Returns a sub value specified by it's name
		 * @see Config::operator[]().
		 */
		JSONValue& operator[](const std::string& name) override;

	protected:

		/**
		 * Parses a JSON node.
		 * @param node The node receiving the parse data
		 * @param scanner The scanner from which the information is parsed, while the node begin symbol has been popped already
		 * @return True, if succeeded
		 */
		bool parseNode(JSONValue& node, JSONScanner& scanner);

		/**
		 * Parses a JSON array.
		 * @param node The node receiving the parse data
		 * @param fieldName The name of the array
		 * @param scanner The scanner from which the information is parsed, while the array begin symbol has been popped already
		 * @return True, if succeeded
		 */
		bool parseArray(JSONValue& node, const std::string& fieldName, JSONScanner& scanner);

		/**
		 * Writes a JSON node.
		 * @param node The node to be written
		 * @param stream The output stream receiving the node's information
		 * @param indentation The indentation of the node (the number of empty space characters in front of the node), with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool writeNode(JSONValue& node, std::ostream& stream, const unsigned int indentation);

		/**
		 * Writes a line (with indentation) to an output stream.
		 * @param string The string to be written
		 * @param stream The output stream in which the string will be written
		 * @param indentation The number of empty space characters to be written in front of the string, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool writeLine(const std::string& string, std::ostream& stream, const unsigned int indentation);

		/**
		 * Returns the default value holding no data.
		 * @return The default value holding no data.
		 */
		static JSONValue& nullValue();

	protected:

		/// Filename of this config object.
		std::string filename_;

		/// The root value.
		JSONValue root_;
};

inline const std::string& JSONConfig::filename() const
{
	return filename_;
}

} // namespace IO

} // namespace Ocean

#endif // META_OCEAN_IO_JSON_CONFIG_H
