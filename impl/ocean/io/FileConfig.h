/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_FILE_CONFIG_H
#define META_OCEAN_IO_FILE_CONFIG_H

#include "ocean/io/IO.h"
#include "ocean/io/Scanner.h"

#include "ocean/base/Config.h"

#include <map>

namespace Ocean
{

namespace IO
{

/**
 * This class implements an application or module configuration toolkit using files as input or output.
 * @ingroup io
 */
class OCEAN_IO_EXPORT FileConfig : public Config
{
	protected:

		// Forward declaration
		class FileValue;

		/**
		 * Definition of a map mapping value names to value objects.
		 */
		typedef std::multimap<std::string, FileValue> FileValues;

		/**
		 * Definition of a file value object.
		 */
		class OCEAN_IO_EXPORT FileValue final : public Value
		{
			friend class FileConfig;

			public:

				/**
				 * Creates a new value object.
				 */
				FileValue() = default;

				/**
				 * Default copy constructor.
				 * @param fileValue The value to copy
				 */
				FileValue(const FileValue& fileValue) = default;

				/**
				 * Creates a new value object by a single value as string and the real value type.
				 * @param value String value
				 * @param type Value type
				 */
				FileValue(const std::string& value, const ValueType type);

				/**
				 * Creates a new value object holding sub values.
				 * @param values Sub values
				 */
				explicit FileValue(const FileValues& values);

				/**
				 * Destructs a value object.
				 */
				~FileValue() override;

				/**
				 * Returns the number of sub values.
				 * @see Config::Value::values().
				 */
				unsigned int values() const override;

				/**
				 * Returns the number of sub values with a given name.
				 * @see Config::Value::values().
				 */
				unsigned int values(const std::string& name) const override;

				/**
				 * Returns whether this value holds at least one specified sub value.
				 * @see Config::Value::exist().
				 */
				bool exist(const std::string& name) const override;

				/**
				 * Returns a sub value specified by it's name and it's index if more than one value exists with the same name.
				 * @see Config::Value::value().
				 */
				FileValue& value(const std::string& name, const unsigned int index) override;

				/**
				 * Returns a sub value specified by it's name and it's index if more than one value exists with the same name.
				 * @see Config::Value::value().
				 */
				bool value(const std::string& name, const unsigned int index, Value** value) override;

				/**
				 * Returns a sub value specified by it's index.
				 * @see Config::Value::value().
				 */
				FileValue& value(const unsigned int index, std::string& name) override;

				/**
				 * Returns a sub value specified by it's index.
				 * @see Config::Value::value().
				 */
				bool value(const unsigned int index, std::string& name, Value** value) override;

				/**
				 * Adds a new sub value specified by it's name.
				 * @param name The name of the sub value to create
				 * @return New sub value
				 */
				FileValue& add(const std::string& name) override;

				/**
				 * Returns this value as boolean.
				 * @see Config::Value::operator().
				 */
				bool operator()(const bool value) const override;

				/**
				 * Returns this value as integer.
				 * @see Config::Value::operator().
				 */
				int operator()(const int value) const override;

				/**
				 * Returns this value as number.
				 * @see Config::Value::operator().
				 */
				double operator()(const double value) const override;

				/**
				 * Returns this value as string.
				 * @see Config::Value::operator().
				 */
				std::string operator()(const std::string& value) const override;

				/**
				 * Returns this value as multi boolean.
				 * @see Config::Value::operator().
				 */
				std::vector<bool> operator()(const std::vector<bool>& value) const override;

				/**
				 * Returns this value as multi integer.
				 * @see Config::Value::operator().
				 */
				std::vector<int> operator()(const std::vector<int>& value) const override;

				/**
				 * Returns this value as multi number.
				 * @see Config::Value::operator().
				 */
				std::vector<double> operator()(const std::vector<double>& value) const override;

				/**
				 * Returns this value as multi string.
				 * @see Config::Value::operator().
				 */
				std::vector<std::string> operator()(const std::vector<std::string>& value) const override;

				/**
				 * Sets this value as boolean.
				 * @see Config::Value::operator=().
				 */
				bool operator=(const bool value) override;

				/**
				 * Sets this value as integer.
				 * @see Config::Value::operator=().
				 */
				bool operator=(const int value) override;

				/**
				 * Sets this value as number.
				 * @see Config::Value::operator=().
				 */
				bool operator=(const double value) override;

				/**
				 * Sets this value as string.
				 * @see Config::Value::operator=().
				 */
				bool operator=(const std::string& value) override;

				/**
				 * Sets this value as multi boolean.
				 * @see Config::Value::operator=().
				 */
				bool operator=(const std::vector<bool>& values) override;

				/**
				 * Sets this value as multi integer.
				 * @see Config::Value::operator=().
				 */
				bool operator=(const std::vector<int>& values) override;

				/**
				 * Sets this value as multi number.
				 * @see Config::Value::operator=().
				 */
				bool operator=(const std::vector<double>& values) override;

				/**
				 * Sets this value as multi string.
				 * @see Config::Value::operator=().
				 */
				bool operator=(const std::vector<std::string>& values) override;

				/**
				 * Returns a sub value specified by it's name
				 * @see Config::Value::operator[]().
				 */
				Value& operator[](const std::string& name) override;

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
				std::string string_;

				/// Sub values if this value is a group.
				FileValues subValues_;
		};

		/**
		 * This scanner is able to scan the specific config file type.
		 */
		class ConfigScanner : public Scanner
		{
			public:

				/**
				 * Definition of several symbols.
				 */
				enum Symbol : uint32_t
				{
					/// Curly open brackets.
					SYMBOL_OPEN,
					/// Curly close brackets.
					SYMBOL_CLOSE,
					/// Square open brackets.
					SYMBOL_BEGIN,
					/// Square close brackets.
					SYMBOL_END
				};

				/**
				 * Definition of several keywords.
				 */
				enum Keyword : uint32_t
				{
					/// TRUE keyword
					KEYWORD_TRUE,
					/// FALSE keyword
					KEYWORD_FALSE
				};

			public:

				/**
				 * Creates a new config scanner object by a given filename.
				 * @param filename Name of the file for the scanner
				 */
				explicit ConfigScanner(const std::string& filename);
		};

	public:

		/**
		 * Creates a new config object.
		 */
		FileConfig();

		/**
		 * Creates a new config object by a given configuration file.
		 * @param filename Configuration file
		 * @param read True, to load the file directly
		 */
		explicit FileConfig(const std::string& filename, const bool read = true);

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
		FileValue& value(const unsigned int index, std::string& name) override;

		/**
		 * Returns a sub value specified by it's index.
		 * @see Config::value().
		 */
		bool value(const unsigned int index, std::string& name, Value** value) override;

		/**
		 * Returns a sub value specified by it's name and it's index if more than one value exists with the same name.
		 * @see Config::value().
		 */
		FileValue& value(const std::string& name, const unsigned int index) override;

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
		FileValue& add(const std::string& name) override;

		/**
		 * Returns a sub value specified by it's name
		 * @see Config::operator[]().
		 */
		FileValue& operator[](const std::string& name) override;

		/**
		 * Writes objects to a given output stream explicitly.
		 * @param values The values to write
		 * @param stream Target stream
		 * @return True, if succeeded
		 */
		static bool write(const FileValues& values, std::ostream& stream);

		/**
		 * Returns whether this file configuration container does not hold any values.
		 * @return True, if so
		 */
		inline bool isEmpty() const;

	protected:

		/**
		 * Reads sub values.
		 * @param values Sub values to fill
		 * @param scanner The scanner used for token scanning
		 * @param stopWithBrackets Determines whether to stop at a closing bracket
		 * @return True, if succeeded
		 */
		bool readSubValues(FileValues& values, Scanner& scanner, bool stopWithBrackets);

		/**
		 * Writes values to a stream with a given tab offset.
		 * @param values The values to write
		 * @param stream Target stream
		 * @param offset Tab offset
		 * @return True, if succeeded
		 */
		static bool write(const FileValues& values, std::ostream& stream, unsigned int offset);

		/**
		 * Writes a given number of tabs.
		 * @param stream Target stream
		 * @param offset Tab offset
		 * @return True, if succeeded
		 */
		static bool writeTabs(std::ostream& stream, unsigned int offset);

		/**
		 * Returns the default value holding no data.
		 * @return The default value holding no data.
		 */
		static FileValue& nullValue();

	protected:

		/// Roots objects.
		FileValues values_;

		/// Filename of this config object.
		std::string filename_;
};

inline const std::string& FileConfig::filename() const
{
	return filename_;
}

inline bool FileConfig::isEmpty() const
{
	return values_.empty();
}

} // namespace IO

} // namespace Ocean

#endif // META_OCEAN_IO_FILE_CONFIG_H
