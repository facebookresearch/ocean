/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_XML_CONFIG_H
#define META_OCEAN_IO_XML_CONFIG_H

#include "ocean/io/IO.h"

#include "ocean/base/Config.h"

#include <map>

// @cond
namespace tinyxml2
{
	// Forward declaration.
	class XMLDocument;
	// Forward declaration.
	class XMLNode;
	// Forward declaration.
	class XMLText;
}
// @endcond

namespace Ocean
{

namespace IO
{

/**
 * This class implements a configuration toolkit using XML encoding to store the parameters.
 * @ingroup io
 */
class OCEAN_IO_EXPORT XMLConfig final : public Config
{
	public:

		/**
		 * Definition of a file value object.
		 */
		class OCEAN_IO_EXPORT XMLValue final : public Value
		{
			friend class XMLConfig;

			public:

				/**
				 * Creates a new value object.
				 */
				XMLValue(XMLConfig* config, tinyxml2::XMLNode* node);

				/**
				 * Destructs a value object.
				 */
				~XMLValue() final;

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
				XMLValue& value(const std::string& name, const unsigned int index) override;

				/**
				 * Returns a sub value specified by it's name and it's index if more than one value exists with the same name.
				 * @see Config::Value::value().
				 */
				bool value(const std::string& name, const unsigned int index, Value** value) override;

				/**
				 * Returns a sub value specified by it's index.
				 * @see Config::Value::value().
				 */
				XMLValue& value(const unsigned int index, std::string& name) override;

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
				XMLValue& add(const std::string& name) override;

				/**
				 * Returns the value of a specified attribute of this value.
				 * @param name The name of the attribute for which the value will be returned
				 * @return The value of the attribute, an empty string if the attribute does not exist
				 */
				std::string attribute(const std::string& name) const;

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
				XMLValue& operator[](const std::string& name) override;

				/**
				 * Returns whether the config value is valid.
				 * @return True, if so
				 */
				explicit operator bool() const;

			private:

				/**
				 * Disabled copy constructor.
				 * @param object The object which would be copied
				 */
				XMLValue(const XMLValue& object) = delete;

				/**
				 * Disabled copy operator.
				 * @param object The object which would be copied
				 * @return Reference to this object
				 */
				XMLValue& operator=(const XMLValue& object) = delete;

			private:

				/// XML node.
				tinyxml2::XMLNode* xmlNode_ = nullptr;

				/// Main config object.
				XMLConfig* xmlConfig_ = nullptr;
		};

		/**
		 * Definition of a vector holding values.
		 */
		typedef std::vector<XMLValue*> XMLValues;

	public:

		/**
		 * Creates a new config object.
		 */
		XMLConfig();

		/**
		 * Destructs a xml config object.
		 */
		~XMLConfig() override;

		/**
		 * Creates a new config object by a given configuration file.
		 * @param filename Configuration file
		 * @param read True, to load the file directly
		 */
		explicit XMLConfig(const std::string& filename, const bool read = true);

		/**
		 * Creates a new config object by a given configuration input stream.
		 * @param inputStream The input stream providing the configuration information
		 * @param read True, to load the file directly
		 */
		explicit XMLConfig(std::istream& inputStream, const bool read = true);

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
		XMLValue& value(const unsigned int index, std::string& name) override;

		/**
		 * Returns a sub value specified by it's index.
		 * @see Config::value().
		 */
		bool value(const unsigned int index, std::string& name, Value** value) override;

		/**
		 * Returns a sub value specified by it's name and it's index if more than one value exists with the same name.
		 * @see Config::value().
		 */
		XMLValue& value(const std::string& name, const unsigned int index) override;

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
		XMLValue& add(const std::string& name) override;

		/**
		 * Returns a sub value specified by it's name
		 * @see Config::operator[]().
		 */
		XMLValue& operator[](const std::string& name) override;

		/**
		 * Returns whether this file configuration container does not hold any values.
		 * @return True, if so
		 */
		bool isEmpty() const;

	protected:

		/**
		 * Returns the first text node connected with a given node.
		 * @param xmlNode Node for that the connected text node is requested
		 * @return First text node
		 */
		static const tinyxml2::XMLText* firstText(const tinyxml2::XMLNode* xmlNode);

		/**
		 * Returns the first text node connected with a given node.
		 * @param xmlNode Node for that the connected text node is requested
		 * @return First text node
		 */
		static tinyxml2::XMLText* firstText(tinyxml2::XMLNode* xmlNode);

		/**
		 * Returns the default value holding no data.
		 * @return The default value holding no data.
		 */
		static XMLValue& nullValue();

	protected:

		/// XML parser.
		tinyxml2::XMLDocument* xmlDocument_ = nullptr;

		/// XML values.
		XMLValues xmlIntermediateValues_;

		/// Filename of this config object.
		std::string filename_;

		/// The input stream.
		std::istream& inputStream_;

		/// The input file stream which is used if a filename is specified.
		std::ifstream inputFileStream_;
};

inline const std::string& XMLConfig::filename() const
{
	return filename_;
}

} // namespace IO

} // namespace Ocean

#endif // META_OCEAN_IO_XML_CONFIG_H
