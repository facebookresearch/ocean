/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_PATH_H
#define META_OCEAN_IO_PATH_H

#include "ocean/io/IO.h"

namespace Ocean
{

namespace IO
{

/**
 * This class holds a path.
 * A path is the base for a file or a directory.<br>
 * Each path can be absolute or relative.<br>
 * Files do not end with a separator, directories must end with a separator.<br>
 * @ingroup io
 */
class OCEAN_IO_EXPORT Path
{
	public:

		/**
		 * Definition of different path types.
		 */
		enum Type
		{
			/// Invalid path type.
			TYPE_INVALID = 0,
			/// File path.
			TYPE_FILE = 1,
			/// Directory path.
			TYPE_DIRECTORY = 2,
			/// Absolute path.
			TYPE_ABSOLUTE = 4,
			/// Relative path.
			TYPE_RELATIVE = 8,
			/// Network path.
			TYPE_NETWORK = 16
		};

		/**
		 * Definition of path separators.
		 */
		enum Separator
		{
			/// Undefined separator.
			SEPARATOR_UNDEFINED = 0,
			/// Slash as separator.
			SEPARATOR_SLASH = '/',
			/// Backslash as separator.
			SEPARATOR_BACKSLASH = '\\'
		};

	public:

		/**
		 * Destructs a path.
		 */
		virtual ~Path() = default;

		/**
		 * Returns the type of this path.
		 * @return Path type.
		 */
		inline Type type() const;

		/**
		 * Returns whether this path is absolute.
		 * If the path is invalid the result is False.
		 * @return True, if so
		 */
		inline bool isAbsolute() const;

		/**
		 * Returns whether this path is relative.
		 * If the path is invalid the result is False.
		 * @return True, if so
		 */
		inline bool isRelative() const;

		/**
		 * Returns whether the path is valid.
		 * @return True, if so
		 */
		virtual bool isValid() const = 0;

		/**
		 * Returns whether this path holds no valid.
		 * @return True, if so
		 */
		inline bool isNull() const;

		/**
		 * Returns whether the path exists already.
		 * @return True, if so
		 */
		virtual bool exists() const = 0;

		/**
		 * Returns whether two paths are equal.
		 * @param right Right path to compare
		 * @return True, if so
		 */
		bool operator==(const Path& right) const;

		/**
		 * Returns whether two paths are not equal.
		 * @param path Right path to compare
		 * @return True, if so
		 */
		inline bool operator!=(const Path& path) const;

		/**
		 * Returns the value of this path.
		 * @return Path value
		 */
		inline const std::string& operator()() const;

		/**
		 * Less operator comparing the actual strings of two paths.
		 * @param path The second path to compare
		 */
		inline bool operator<(const Path& path) const;

		/**
		 * Returns whether a given character is a separator.
		 * @param character Character to check for a separator
		 * @return True, if so
		 */
		static bool isSeparator(const char character);

		/**
		 * Returns the default separator of a path.
		 * @return The default separator for the current platform
		 */
		static char defaultSeparator();

	protected:

		/**
		 * Creates a new undefined path.
		 */
		Path() = default;

		/**
		 * Copy constructor.
		 * @param path The path to copy
		 */
		Path(const Path& path) = default;

		/**
		 * Creates a new path.
		 * @param value Path value
		 */
		explicit Path(const std::string& value);

		/**
		 * Checks the type of this path and trims it.
		 * @param expectType Expected path type
		 */
		void checkPath(const Type expectType);

		/**
		 * Trims this path.
		 */
		void trim();

		/**
		 * Default assign operator.
		 * @param path The path to assign
		 */
		Path& operator=(const Path& path) = default;

	protected:

		/// Path type.
		Type pathType_ = TYPE_INVALID;

		/// Path value.
		std::string pathValue_;
};

inline Path::Type Path::type() const
{
	return pathType_;
}

inline bool Path::isAbsolute() const
{
	return (type() & TYPE_ABSOLUTE) != 0;
}

inline bool Path::isRelative() const
{
	return (type() & TYPE_RELATIVE) != 0;
}

inline bool Path::isNull() const
{
	return pathValue_.empty();
}

inline bool Path::operator!=(const Path& path) const
{
	return !(*this == path);
}

inline const std::string& Path::operator()() const
{
	return pathValue_;
}

inline bool Path::operator<(const Path& path) const
{
	return (*this)() < path();
}

}

}

#endif // META_OCEAN_IO_PATH_H
