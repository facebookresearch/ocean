/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_UNIQUE_ID_H
#define META_OCEAN_PLATFORM_UNIQUE_ID_H

#include "ocean/platform/Platform.h"

#include "ocean/base/String.h"

#ifndef _WINDOWS
	#include <uuid/uuid.h>
#endif

namespace Ocean
{

namespace Platform
{

/**
 * This class provides a 128 bit id which is unique.
 * @ingroup platform
 */
class UniqueId
{
	public:

		/**
		 * Creates a new invalid id.
		 */
		inline UniqueId();

		/**
		 * Creates a new unique id by two 64 bit values.
		 * @param a First 64 bit value
		 * @param b Second 64 bit value
		 */
		inline UniqueId(unsigned long long a, unsigned long long b);

		/**
		 * Creates a new unique id.
		 * @param createUniqueId True, to initialize this object with a valid unique id; False, to create in invalid id
		 */
		inline explicit UniqueId(const bool createUniqueId);

		/**
		 * Modifies this id and sets a new unique id value.
		 */
		inline void newUniqueId();

		/**
		 * Returns whether this object holds a valid unique id.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns this unique id as string.
		 * @return The id as string
		 */
		inline std::string string() const;

		/**
		 * Returns a very short part of this unique id as string.
		 * The short string contains four characters of the normal string (representing the first two bytes of information).<br>
		 * Beware: The short string should be used for userfriendly output information only because this string is not unique anymore!
		 * @return The id as string
		 */
		inline std::string shortString() const;

		/**
		 * Returns a pointer to two 64 bit values defining the id.
		 * @return The 64 bit values
		 */
		inline const unsigned long long* operator()() const;

		/**
		 * Compares two id objects and returns whether the left one has a smaller id than the right one.
		 * @param uniqueId The right unique id object
		 * @return True, if so
		 */
		inline bool operator<(const UniqueId& uniqueId) const;

		/**
		 * Returns whether two id objects have the same id value.
		 * @param uniqueId The second unique id object
		 * @return True, if so
		 */
		inline bool operator==(const UniqueId& uniqueId) const;

		/**
		 * Returns whether two id objects have not the same id value.
		 * @param uniqueId The second unique id object
		 * @return True, if so
		 */
		inline bool operator!=(const UniqueId& uniqueId) const;

	protected:

		/// The two 64 bit values defining the id value.
		unsigned long long value[2];
};

inline UniqueId::UniqueId()
{
	value[0] = 0ull;
	value[1] = 0ull;

	ocean_assert(!isValid());
}

inline UniqueId::UniqueId(unsigned long long a, unsigned long long b)
{
	value[0] = a;
	value[1] = b;

	ocean_assert(isValid());
}

inline UniqueId::UniqueId(const bool createUniqueId)
{
	if (createUniqueId)
	{
		newUniqueId();
		ocean_assert(isValid());
	}
	else
	{
		value[0] = 0ull;
		value[1] = 0ull;

		ocean_assert(!isValid());
	}
}

inline void UniqueId::newUniqueId()
{
#ifdef _WINDOWS
	static_assert(sizeof(UUID) == sizeof(unsigned long long) * 2, "Invalid UUID size!");
	UuidCreate((UUID*)value);
#else
	static_assert(sizeof(unsigned long long) == 8, "Invalid data type");
	static_assert(sizeof(uuid_t) == sizeof(unsigned long long) * 2, "Invalid data type");

	uuid_generate(*((uuid_t*)&value));
#endif
}

inline const unsigned long long* UniqueId::operator()() const
{
	return value;
}

inline bool UniqueId::isValid() const
{
	return value[0] != 0ull || value[1] != 0ull;
}

inline std::string UniqueId::string() const
{
	return String::toAStringHex(value[1]) + String::toAStringHex(value[0]);
}

inline std::string UniqueId::shortString() const
{
	return String::toAStringHexReverse((unsigned char*)value, 2u);
}

inline bool UniqueId::operator<(const UniqueId& uniqueId) const
{
	return value[0] < uniqueId.value[0] || (value[0] == uniqueId.value[1] && value[1] < uniqueId.value[1]);
}

inline bool UniqueId::operator==(const UniqueId& uniqueId) const
{
	return value[0] == uniqueId.value[0] && value[1] == uniqueId.value[1];
}

inline bool UniqueId::operator!=(const UniqueId& uniqueId) const
{
	return !(*this == uniqueId);
}

}

}

#endif // META_OCEAN_PLATFORM_UNIQUE_ID_H
