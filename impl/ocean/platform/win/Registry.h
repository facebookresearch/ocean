/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WIN_REGISTRY_H
#define META_OCEAN_PLATFORM_WIN_REGISTRY_H

#include "ocean/platform/win/Win.h"

#include <vector>

namespace Ocean
{

namespace Platform
{

namespace Win
{

/**
 * This class provides windows registry functionalities.
 * @ingroup platformwin
 */
class OCEAN_PLATFORM_WIN_EXPORT Registry
{
	public:

		/**
		 * Definition of different registry root types.
		 */
		enum RootType
		{
			/// Classes root type.
			ROOT_CLASSES_ROOT,
			/// Current user root type.
			ROOT_CURRENT_USER,
			/// Local machine root type.
			ROOT_LOCAL_MACHINE,
			/// Users root type.
			ROOT_USERS
		};

		/**
		 * Definition of different registry value types.
		 */
		enum ValueType
		{
			/// Invalid value type.
			VALUE_INVALID,
			/// Data value type.
			VALUE_DATA,
			/// 32 bit integer value type.
			VALUE_INTEGER32,
			/// 64 bit integer value type.
			VALUE_INTEGER64,
			/// String value type.
			VALUE_STRING,
			/// Multi string value type.
			VALUE_STRINGS
		};

		/**
		 * Definition of a vector holding key or value names.
		 */
		typedef std::vector<std::string> Names;

	public:

		/**
		 * Translates a root type.
		 * @param root Root type to translate
		 * @return Translated root type as key handle
		 */
		static HKEY translateRoot(const RootType root);

		/**
		 * Opens a registry key and returns the received key handle.
		 * @param root Key root to open
		 * @param path Path and name of the key to open
		 * @return Received key handle
		 */
		static HKEY openKey(const RootType root, const std::string& path);

		/**
		 * Opens a registry key and returns the received key handle.
		 * @param key Base registry key
		 * @param name The name of the new sub key
		 * @return Received key handle
		 */
		static HKEY openKey(const HKEY key, const std::string& name);

		/**
		 * Closes a registry key.
		 * @param key Key to close
		 * @return True, if succeeded
		 */
		static bool closeKey(const HKEY key);

		/**
		 * Creates a new key.
		 * @param root Key root
		 * @param path Path and name of the new key
		 * @return True, if succeeded
		 */
		static bool createKey(const RootType root, const std::string& path);

		/**
		 * Returns the type of a value
		 * @param key Base registry key
		 * @param name Value name
		 * @return Value type
		 */
		static ValueType valueType(const HKEY key, const std::string& name);

		/**
		 * Sets a value of a key.
		 * @param root Key root
		 * @param path Path and name of the key to set the value for
		 * @param name The name of the value to set
		 * @param value Value to set
		 * @return True, if succeeded
		 */
		static bool setValue(const RootType root, const std::string& path, const std::string& name, const int value);

		/**
		 * Sets a value of a key.
		 * @param key Base registry key
		 * @param name The name of the value to set
		 * @param value Value to set
		 * @return True, if succeeded
		 */
		static bool setValue(const HKEY key, const std::string& name, const int value);

		/**
		 * Sets a value of a key.
		 * @param root Key root
		 * @param path Path and name of the key to set the value for
		 * @param name The name of the value to set
		 * @param value Value to set
		 * @return True, if succeeded
		 */
		static bool setValue(const RootType root, const std::string& path, const std::string& name, const long long value);

		/**
		 * Sets a value of a key.
		 * @param key Base registry key
		 * @param name The name of the value to set
		 * @param value Value to set
		 * @return True, if succeeded
		 */
		static bool setValue(const HKEY key, const std::string& name, const long long value);

		/**
		 * Sets a value of a key.
		 * @param root Key root
		 * @param path Path and name of the key to set the value for
		 * @param name The name of the value to set
		 * @param value Value to set
		 * @return True, if succeeded
		 */
		static bool setValue(const RootType root, const std::string& path, const std::string& name, const std::string& value);

		/**
		 * Sets a value of a key.
		 * @param key Base registry key
		 * @param name The name of the value to set
		 * @param value Value to set
		 * @return True, if succeeded
		 */
		static bool setValue(const HKEY key, const std::string& name, const std::string& value);

		/**
		 * Sets a value of a key.
		 * @param root Key root
		 * @param path Path and name of the key to set the value for
		 * @param name The name of the value to set
		 * @param value Value to set
		 * @return True, if succeeded
		 */
		static bool setValue(const RootType root, const std::string& path, const std::string& name, const Names& value);

		/**
		 * Sets a value of a key.
		 * @param key Base registry key
		 * @param name The name of the value to set
		 * @param value Value to set
		 * @return True, if succeeded
		 */
		static bool setValue(const HKEY key, const std::string& name, const Names& value);

		/**
		 * Sets a value of a key.
		 * @param root Key root
		 * @param path Path and name of the key to set the value for
		 * @param name The name of the value to set
		 * @param value Value buffer to set
		 * @param size Size of the value to set in bytes
		 * @return True, if succeeded
		 */
		static bool setValue(const RootType root, const std::string& path, const std::string& name, const unsigned char* value, const unsigned int size);

		/**
		 * Sets a value of a key.
		 * @param key Base registry key
		 * @param name The name of the value to set
		 * @param value Value buffer to set
		 * @param size Size of the value to set in bytes
		 * @return True, if succeeded
		 */
		static bool setValue(const HKEY key, const std::string& name, const unsigned char* value, const unsigned int size);

		/**
		 * Returns a key value.
		 * @param root Key root
		 * @param path Path and name of the key to return the value from
		 * @param name The name of the value to return
		 * @param value Default value top return if the key does not hold the specified value
		 * @return Key value or specified default value
		 */
		static int value(const RootType root, const std::string& path, const std::string& name, const int value);

		/**
		 * Returns a key value.
		 * @param key Base registry key
		 * @param name The name of the value to return
		 * @param value Default value top return if the key does not hold the specified value
		 * @return Key value or specified default value
		 */
		static int value(const HKEY key, const std::string& name, const int value);

		/**
		 * Returns a key value.
		 * @param root Key root
		 * @param path Path and name of the key to return the value from
		 * @param name The name of the value to return
		 * @param value Default value top return if the key does not hold the specified value
		 * @return Key value or specified default value
		 */
		static long long value(const RootType root, const std::string& path, const std::string& name, const long long value);

		/**
		 * Returns a key value.
		 * @param key Base registry key
		 * @param name The name of the value to return
		 * @param value Default value top return if the key does not hold the specified value
		 * @return Key value or specified default value
		 */
		static long long value(const HKEY key, const std::string& name, const long long value);

		/**
		 * Returns a key value.
		 * @param root Key root
		 * @param path Path and name of the key to return the value from
		 * @param name The name of the value to return
		 * @param value Default value top return if the key does not hold the specified value
		 * @return Key value or specified default value
		 */
		static std::string value(const RootType root, const std::string& path, const std::string& name, const std::string& value);

		/**
		 * Returns a key value.
		 * @param key Base registry key
		 * @param name The name of the value to return
		 * @param value Default value top return if the key does not hold the specified value
		 * @return Key value or specified default value
		 */
		static std::string value(const HKEY key, const std::string& name, const std::string& value);

		/**
		 * Returns a key value.
		 * @param root Key root
		 * @param path Path and name of the key to return the value from
		 * @param name The name of the value to return
		 * @param value Default value top return if the key does not hold the specified value
		 * @return Key value or specified default value
		 */
		static Names value(const RootType root, const std::string& path, const std::string& name, const Names& value);

		/**
		 * Returns a key value.
		 * @param key Base registry key
		 * @param name The name of the value to return
		 * @param value Default value top return if the key does not hold the specified value
		 * @return Key value or specified default value
		 */
		static Names value(const HKEY key, const std::string& name, const Names& value);

		/**
		 * Returns all values specified for a key.
		 * @param root Key root
		 * @param path Path and name of the key to return the values from
		 * @return Value names
		 */
		static Names values(const RootType root, const std::string& path);

		/**
		 * Returns all values specified for a key.
		 * @param key Key to return the values from
		 * @return Value names
		 */
		static Names values(const HKEY key);

		/**
		 * Returns all sub keys specified for a key.
		 * @param root Key root
		 * @param path Path and name of the key to return the keys from
		 * @return Sub key names
		 */
		static Names keys(const RootType root, const std::string& path);

		/**
		 * Returns all sub keys specified for a key.
		 * @param key Key to return the sub keys from
		 * @return Sub key names
		 */
		static Names keys(const HKEY key);
};

}

}

}

#endif // META_OCEAN_PLATFORM_WIN_REGISTRY_H
