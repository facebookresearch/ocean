/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_CONFIG_H
#define META_OCEAN_BASE_CONFIG_H

#include "ocean/base/Base.h"
#include "ocean/base/Timestamp.h"

#include <vector>

#ifdef __APPLE__
	#ifdef TYPE_BOOL
		#undef TYPE_BOOL
	#endif
#endif

namespace Ocean
{

/**
 * This class implements an application or module configuration toolkit.
 * Each configuration toolkit holds a value tree.<br>
 * Different single value types are supported like e.g. boolean, integer or strings.<br>
 * Furthermore, groups can be specified holding an arbitrary number of sub values.<br>
 * Values names does not need to be unique. Therefore, a group can hold several sub values with the same name.<br>
 * Also, values with the same name can have different value types.<br>
 * However, this is an abstract configuration object and therefore should be used as platform independent interface only.<br>
 * Use e.g. file configuration or a registry configuration object instead.
 * The entire object is not thread-safe.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT Config
{
	public:

		/**
		 * Definition of different value types.
		 */
		enum ValueType
		{
			/// Invalid value type.
			TYPE_INVALID,
			/// Undefined value type.
			TYPE_UNDEFINED,
			/// Boolean value type.
			TYPE_BOOL,
			/// Integer value type.
			TYPE_INT,
			/// Number value type.
			TYPE_NUMBER,
			/// String value type.
			TYPE_STRING,
			/// Multi boolean value type.
			TYPE_MULTI_BOOL,
			/// Multi integer value type.
			TYPE_MULTI_INT,
			/// Multi number value type.
			TYPE_MULTI_NUMBER,
			/// Multi string value type.
			TYPE_MULTI_STRING,
			/// Group value type.
			TYPE_GROUP
		};

	public:

		/**
		 * This class implements a configuration value.
		 * Each value can represent a single parameter or a grouping object.<br>
		 */
		class OCEAN_BASE_EXPORT Value
		{
			friend class Config;

			public:

				/**
				 * Returns the type of this value.
				 * @return Value type
				 */
				virtual ValueType type() const;

				/**
				 * Returns whether this value holds a multi type.
				 * @return True, if so
				 */
				virtual bool isMultiType() const;

				/**
				 * Returns the number of sub values.
				 * @return Sub value number
				 */
				virtual unsigned int values() const;

				/**
				 * Returns the number of sub values with a given name.
				 * @param name The name of the sub values to return
				 * @return Sub value number
				 */
				virtual unsigned int values(const std::string& name) const;

				/**
				 * Returns whether this value holds at least one specified sub value.
				 * @param name The name of the sub value to check
				 * @return True, if so
				 */
				virtual bool exist(const std::string& name) const;

				/**
				 * Returns a sub value specified by it's name and it's index if more than one value exists with the same name.
				 * @param name The name of the value to return
				 * @param index The index within those sub values with the specified name
				 * @return Sub value
				 */
				virtual Value& value(const std::string& name, const unsigned int index = 0u);

				/**
				 * Returns a sub value specified by it's name and it's index if more than one value exists with the same name.
				 * @param name The name of the value to return
				 * @param index The index within those sub values with the specified name
				 * @param value Resulting sub value
				 * @return True, if the value exists
				 */
				virtual bool value(const std::string& name, const unsigned int index, Value** value);

				/**
				 * Returns a sub value specified by it's index.
				 * @param index The index of the sub value to return
				 * @param name Resulting name of the returning value
				 * @return Sub value
				 */
				virtual Value& value(const unsigned int index, std::string& name);

				/**
				 * Returns a sub value specified by it's index.
				 * @param index The index of the sub value to return
				 * @param name Resulting name of the returning value
				 * @param value Resulting sub value
				 * @return True, if the value exists
				 */
				virtual bool value(const unsigned int index, std::string& name, Value** value);

				/**
				 * Adds a new sub value specified by it's name.
				 * @param name The name of the sub value to create
				 * @return New sub value
				 */
				virtual Value& add(const std::string& name);

				/**
				 * Returns this value as boolean.
				 * @param value Default value which will be returned if this value holds no boolean
				 * @return Internal value
				 */
				virtual bool operator()(const bool value) const;

				/**
				 * Returns this value as integer.
				 * @param value Default value which will be returned if this value holds no integer
				 * @return Internal value
				 */
				virtual int operator()(const int value) const;

				/**
				 * Returns this value as number.
				 * @param value Default value which will be returned if this value holds no number
				 * @return Internal value
				 */
				virtual double operator()(const double value) const;

				/**
				 * Returns this value as string.
				 * @param value Default value which will be returned if this value holds no string
				 * @return Internal value
				 */
				inline std::string operator()(const char* value) const;

				/**
				 * Returns this value as string.
				 * @param value Default value which will be returned if this value holds no string
				 * @return Internal value
				 */
				virtual std::string operator()(const std::string& value) const;

				/**
				 * Returns this value as multi boolean.
				 * @param value Default value which will be returned if this value holds no boolean
				 * @return Internal value
				 */
				virtual std::vector<bool> operator()(const std::vector<bool>& value) const;

				/**
				 * Returns this value as multi integer.
				 * @param value Default value which will be returned if this value holds no integer
				 * @return Internal value
				 */
				virtual std::vector<int> operator()(const std::vector<int>& value) const;

				/**
				 * Returns this value as multi number.
				 * @param value Default value which will be returned if this value holds no number
				 * @return Internal value
				 */
				virtual std::vector<double> operator()(const std::vector<double>& value) const;

				/**
				 * Returns this value as multi string.
				 * @param value Default value which will be returned if this value holds no string
				 * @return Internal value
				 */
				virtual std::vector<std::string> operator()(const std::vector<std::string>& value) const;

				/**
				 * Sets this value as boolean.
				 * @param value The value to set
				 * @return True, if succeeded
				 */
				virtual bool operator=(const bool value);

				/**
				 * Sets this value as integer.
				 * @param value The value to set
				 * @return True, if succeeded
				 */
				virtual bool operator=(const int value);

				/**
				 * Sets this value as number.
				 * @param value The value to set
				 * @return True, if succeeded
				 */
				virtual bool operator=(const double value);

				/**
				 * Sets this value as string.
				 * @param value The value to set
				 * @return True, if succeeded
				 */
				inline bool operator=(const char* value);

				/**
				 * Sets this value as string.
				 * @param value The value to set
				 * @return True, if succeeded
				 */
				virtual bool operator=(const std::string& value);

				/**
				 * Sets this value as multi boolean.
				 * @param values The values to set
				 * @return True, if succeeded
				 */
				virtual bool operator=(const std::vector<bool>& values);

				/**
				 * Sets this value as multi integer.
				 * @param values The values to set
				 * @return True, if succeeded
				 */
				virtual bool operator=(const std::vector<int>& values);

				/**
				 * Sets this value as multi number.
				 * @param values The values to set
				 * @return True, if succeeded
				 */
				virtual bool operator=(const std::vector<double>& values);

				/**
				 * Sets this value as multi string.
				 * @param values The values to set
				 * @return True, if succeeded
				 */
				virtual bool operator=(const std::vector<std::string>& values);

				/**
				 * Returns the first sub value specified by it's name or creates a new value if no existing.
				 * @param name The name of the sub-value to return
				 * @return Specified sub value
				 */
				virtual Value& operator[](const std::string& name);

			protected:

				/**
				 * Creates a new value object.
				 */
				Value() = default;

				/**
				 * Default copy constructor.
				 * @param value The value to copied
				 */
				Value(const Value& value) = default;

				/**
				 * Destructs a value object.
				 */
				virtual ~Value() = default;

			protected:

				/// Value type.
				ValueType valueType_ = TYPE_INVALID;
		};

	public:

		/**
		 * Destructs a configuration toolkit.
		 */
		virtual ~Config();

		/**
		 * Reads / loads all values of this configuration.
		 * @return True, if succeeded
		 */
		virtual bool read();

		/**
		 * Writes / saves all values of this configuration.
		 * @return True, if succeeded
		 */
		virtual bool write();

		/**
		 * Returns the timestamp of the last write execution.
		 * @return Most recent write timestamp
		 */
		inline Timestamp writeTimestamp() const;

		/**
		 * Returns the number of sub values.
		 * @return Sub value number
		 */
		virtual unsigned int values() const;

		/**
		 * Returns the number of sub values with a given name.
		 * @param name The name of the sub values to return
		 * @return Sub value number
		 */
		virtual unsigned int values(const std::string& name) const;

		/**
		 * Returns whether this value holds at least one specified sub value.
		 * @param name The name of the sub value to check
		 * @return True, if so
		 */
		virtual bool exist(const std::string& name) const;

		/**
		 * Returns a sub value specified by it's index.
		 * @param index The index of the sub value to return
		 * @param name Resulting name of the returning value
		 * @return Sub value
		 */
		virtual Value& value(const unsigned int index, std::string& name);

		/**
		 * Returns a sub value specified by it's index.
		 * @param index The index of the sub value to return
		 * @param name Resulting name of the returning value
		 * @param value Resulting sub value
		 * @return True, if the value exists
		 */
		virtual bool value(const unsigned int index, std::string& name, Value** value);

		/**
		 * Returns a sub value specified by it's name and it's index if more than one value exists with the same name.
		 * @param name The name of the value to return
		 * @param index The index within those sub values with the specified name
		 * @param value Resulting sub value
		 * @return True, if the value exists
		 */
		virtual bool value(const std::string& name, const unsigned int index, Value** value);

		/**
		 * Returns a sub value specified by it's name and it's index if more than one value exists with the same name.
		 * @param name The name of the value to return
		 * @param index The index within those sub values with the specified name
		 * @return Sub value
		 */
		virtual Value& value(const std::string& name, const unsigned int index = 0);

		/**
		 * Adds a new sub value specified by it's name.
		 * @param name The name of the sub value to create
		 * @return New sub value
		 */
		virtual Value& add(const std::string& name);

		/**
		 * Returns the first sub value specified by it's name or creates a new one if not existing.
		 * @param name The name of the sub value to return
		 * @return Specified sub value
		 */
		virtual Value& operator[](const std::string& name);

	protected:

		/**
		 * Creates a new configuration toolkit
		 */
		Config();

		/**
		 * Disabled copy constructor.
		 * @param config Object which would be copied
		 */
		Config(const Config& config) = delete;

		/**
		 * Disabled copy operator.
		 * @param config Object which would be copied
		 * @return Reference to this object
		 */
		Config& operator=(const Config& config) = delete;

		/**
		 * Returns the default value holding no data.
		 * @note This function is not virtual by intention. Derived classes are supposed to override it and have it return their derived value type.
		 * @return The default value holding no data.
		 */
		static Value& nullValue();

	protected:

		/// Holds the timestamp of the most recent write execution.
		Timestamp writeTimestamp_;
};

inline std::string Config::Value::operator()(const char* value) const
{
	return (*this)(std::string(value));
}

inline bool Config::Value::operator=(const char* value)
{
	return (*this) = std::string(value);
}

inline Timestamp Config::writeTimestamp() const
{
	return writeTimestamp_;
}

}

#endif // META_OCEAN_BASE_CONFIG_H
