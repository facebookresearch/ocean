/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_DIRECTORY_H
#define META_OCEAN_IO_DIRECTORY_H

#include "ocean/io/IO.h"
#include "ocean/io/File.h"
#include "ocean/io/Path.h"

namespace Ocean
{

namespace IO
{

// Forward declaration.
class Directory;

/**
 * Definition of a vector holding directories.
 * @ingroup io
 */
typedef std::vector<Directory> Directories;

/**
 * This class holds a directory.
 * Valid directories will end with a separator.
 * @ingroup io
 */
class OCEAN_IO_EXPORT Directory : public Path
{
	public:

		/**
		 * Creates an empty directory.
		 */
		Directory();

		/**
		 * Creates a new directory.
		 * @param path Directory path
		 */
		explicit Directory(const std::string& path);

		/**
		 * Creates a new directory by a given file.
		 * @param file File to get the directory from
		 */
		explicit Directory(const File& file);

		/**
		 * Creates a new directory by a given path.
		 * @param path Path to create a directory from
		 */
		explicit Directory(const Path& path);

		/**
		 * Returns whether the directory is valid.
		 * @see Path::isValid().
		 */
		virtual bool isValid() const;

		/**
		 * Returns whether the directory exists already.
		 * @see Path::exists().
		 */
		virtual bool exists() const;

		/**
		 * Removes this directory from the filesystem.
		 * @param recursively True, to remove all sub-directories and their files recursively; False, to remove only this directory if empty
		 * @return True, if succeeded
		 * @see exists().
		 */
		bool remove(const bool recursively = false) const;

		/**
		 * Create all necessary sub-directories for the given path, if they do not exist.
		 * @return True, if succeeded
		 */
		bool create() const;

		/**
		 * Returns the base of this directory.
		 * The directory's base is the entire directory path without the ending separator.<br>
		 * The base of e.g. "/path/to/foo/bar/" is "/path/to/foo/bar"
		 * @return The directory's base
		 */
		std::string base() const;

		/**
		 * Returns the name of a directory.
		 * @return The name of a directory, e.g., for "/path/to/foo/bar/" it returns "bar"; will be empty if the directory doesn't exist or is invalid
		 */
		std::string name() const;

		/**
		 * Returns all files in the directory with the given file format.
		 * @param fileType File type to find files for, for example "png", "txt", or "json". Use "*" to list all files.
		 * @param recursive True, to find file in all possible sub-directories; False, to find files in this directory only
		 * @return List of existing files
		 */
		Files findFiles(const std::string& fileType = std::string("*"), const bool recursive = false) const;

		/**
		 * Returns all directories of this directory.
		 * @param recursive True, to find sub-directories as well; False, to find direct child directories only
		 * @return List of existing directories
		 */
		Directories findDirectories(const bool recursive = false) const;

		/**
		 * Returns a file composed of this directory and a relative file.
		 * @param file Relative file
		 * @return New created file
		 */
		File operator+(const File& file) const;

		/**
		 * Returns a directory composed of this and a relative directory.
		 * @param path Relative path to add
		 * @return New created directory
		 */
		Directory operator+(const Directory& path) const;

		/**
		 * Adds an relative path to this directory.
		 * @param path Relative path to add
		 * @return Reference to the new path
		 */
		Directory& operator+=(const Directory& path);

		/**
		 * Removes the last sub-directory from this directory.
		 * @return Reference to the modified path
		 */
		Directory& operator--();

		/**
		 * Removes the last sub-directory from this directory.
		 * @return A copy of this directory before the last sub-directory was removed
		 */
		Directory operator--(int);

		/**
		 * Creates a new temporary directory.
		 * The directory will be created as a sub-directory inside of a OS-specific temporary files path.
		 * @return Created temporary directory if the operation succeeded; otherwise, an empty file path is returned.
		 */
		static Directory createTemporaryDirectory();

	protected:

#if defined(__APPLE__)

		/**
		 * Creates a new temporary directory (specialization for Apple platforms).
		 * The directory will be created as a sub-directory inside of a OS-specific temporary files path.
		 * @sa createTemporaryDirectory()
		 * @return Created temporary directory if the operation succeeded; otherwise, an empty file path is returned.
		 */
		static Directory createTemporaryDirectoryApple();

		/**
		 * Returns whether a directory exists (specialization for Apple platforms).
		 * @param directory Directory to check
		 * @return True, if so
		 */
		static bool existsApple(const std::string& directory);

#endif // defined(__APPLE__)

};

/**
 * This class implements a scoped directory object which will delete the underlying directory (and sub-directories including all files) from the filesystem when the scope ends.
 * @see Directory
 * @ingroup io
 */
class OCEAN_IO_EXPORT ScopedDirectory final : public Directory
{
	public:

		/**
		 * Default constructor creating an invalid object not holding any directory.
		 */
		ScopedDirectory() = default;

		/**
		 * Move constructor.
		 * @param scopedDirectory The scoped directory to be moved
		 */
		inline ScopedDirectory(ScopedDirectory&& scopedDirectory);

		/**
		 * Create a new scoped directory.
		 * @param directory The directory for which the new scoped object will be created
		 */
		explicit inline ScopedDirectory(const Directory& directory);

		/**
		 * Create a new scoped directory.
		 * @param directory The name of the directory for which a new scoped object will be created
		 */
		explicit inline ScopedDirectory(const std::string& directory);

		/**
		 * Destructs this object and deletes the actual directory.
		 */
		~ScopedDirectory();

		/**
		 * Move operator.
		 * @param scopedDirectory The scoped directory to be moved
		 * @return Reference to this object
		 */
		ScopedDirectory& operator=(ScopedDirectory&& scopedDirectory);

	protected:

		/**
		 * Disabled copy constructor.
		 * @param scopedDirectory Directory which would be copied
		 */
		ScopedDirectory(const ScopedDirectory& scopedDirectory) = delete;

		/**
		 * Disabled copy constructor.
		 * @param scopedDirectory Directory which would be copied
		 * @return Reference to this object
		 */
		ScopedDirectory& operator=(const ScopedDirectory& scopedDirectory) = delete;
};

inline ScopedDirectory::ScopedDirectory(ScopedDirectory&& scopedDirectory) :
	Directory()
{
	*this = std::move(scopedDirectory);
}

inline ScopedDirectory::ScopedDirectory(const Directory& directory) :
	Directory(directory)
{
	// nothing to do here
}

inline ScopedDirectory::ScopedDirectory(const std::string& directory) :
	Directory(directory)
{
	// nothing to do here
}

}

}

#endif // META_OCEAN_IO_DIRECTORY_H
