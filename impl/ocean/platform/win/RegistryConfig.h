/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WIN_REGISTRY_CONFIG_H
#define META_OCEAN_PLATFORM_WIN_REGISTRY_CONFIG_H

#include "ocean/platform/win/Win.h"
#include "ocean/platform/win/Registry.h"

#include "ocean/base/Config.h"

#include <map>

namespace Ocean
{

namespace Platform
{

namespace Win
{

/**
 * This class implements an application or module configuration toolkit using the windows registry.
 * Beware: In contrast to other implementations of Config, this class uses case-insensitive name matching for all values, due to the underlying case-insensitive key name matching of the Windows Registry.
 * @ingroup platformwin
 */
class OCEAN_PLATFORM_WIN_EXPORT RegistryConfig final : public Config
{
	protected:

		// Forward declaration
		class RegistryValue;

		/**
		 * Implements case-insensitive string comparison.
		 */
		struct CaseInsensitiveCompare
		{
			/**
			 * @param left The left string operand
			 * @param right The right string operand
			 * @return Returns true if lower case version of left operand is less than lower case version of right operand.
			 */
			 inline bool operator()(const std::string& left, const std::string& right) const;
		};

		/**
		 * Definition of a map mapping value names to value objects using case-insensitive key comparison.
		 */
		typedef std::map<std::string, RegistryValue, CaseInsensitiveCompare> Values;

		/**
		 * Definition of a registry value object.
		 */
		class OCEAN_PLATFORM_WIN_EXPORT RegistryValue final : protected Value
		{
			friend class RegistryConfig;

			public:

				/**
				 * Creates a new value object.
				 */
				RegistryValue();

				/**
				 * Creates a new value object.
				 * @param key Registry key
				 */
				explicit RegistryValue(const HKEY key);

				/**
				 * Creates a new value object.
				 * @param key Registry key
				 * @param name The name of the value
				 */
				explicit RegistryValue(const HKEY key, const std::string name);

				/**
				 * Destructs a value object.
				 */
				~RegistryValue() override;

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
				RegistryValue& value(const std::string& name, const unsigned int index) override;

				/**
				 * Returns a sub value specified by it's name and it's index if more than one value exists with the same name.
				 * @see Config::Value::value().
				 */
				bool value(const std::string& name, const unsigned int index, Value** value) override;

				/**
				 * Returns a sub value specified by it's index.
				 * @see Config::Value::value().
				 */
				RegistryValue& value(const unsigned int index, std::string& name) override;

				/**
				 * Returns a sub value specified by it's index.
				 * @see Config::Value::value().
				 */
				bool value(const unsigned int index, std::string& name, Value** value) override;

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
				 * Returns a sub value specified by it's name
				 * @see Config::Value::operator[]().
				 */
				RegistryValue& operator[](const std::string& name) override;

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
				 * Writes this value object to the windows registry.
				 * @param key Registry key
				 * @param name Windows registry value name
				 * @return True, if succeeded
				 */
				bool writeToRegistry(const HKEY key, const std::string& name) const;

			private:

				/// Value as string.
				std::string string_;

				/// Sub values this value is a group.
				Values subValues_;
		};

	public:

		/**
		 * Creates a new configuration object by the registry base path.
		 * @param path Registry base path
		 * @param rootType The root type of the path
		 */
		explicit RegistryConfig(const std::string& path, const Registry::RootType rootType = Registry::ROOT_CURRENT_USER);

		/**
		 * Destructs a configuration object.
		 */
		~RegistryConfig() override;

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
		RegistryValue& value(const unsigned int index, std::string& name) override;

		/**
		 * Returns a sub value specified by it's index.
		 * @see Config::value().
		 */
		bool value(const unsigned int index, std::string& name, Value** value) override;

		/**
		 * Returns a sub value specified by it's name and it's index if more than one value exists with the same name.
		 * @see Config::value().
		 */
		RegistryValue& value(const std::string& name, const unsigned int index) override;

		/**
		 * Returns a sub value specified by it's name and it's index if more than one value exists with the same name.
		 * @see Config::value().
		 */
		bool value(const std::string& name, const unsigned int index, Value** value) override;

		/**
		 * Returns a sub value specified by it's name
		 * @see Config::operator[]().
		 */
		RegistryValue& operator[](const std::string& name) override;

	protected:

		/**
		 * Returns the default value holding no data.
		 * @return The default value holding no data.
		 */
		static RegistryValue& nullValue();

	private:

		/// The root type of the registry path.
		const Registry::RootType rootType_;

		/// Configuration registry path.
		std::string path_;

		/// Registry values.
		Values values_;
};

inline bool RegistryConfig::CaseInsensitiveCompare::operator()(const std::string& left, const std::string& right) const
{
	return String::toLower(left) < String::toLower(right);
}

} // namespace Win

} // namespace Platform

} // namespace Ocean

#endif // META_OCEAN_PLATFORM_WIN_REGISTRY_CONFIG_H
