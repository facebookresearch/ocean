/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_FILE_H
#define META_OCEAN_IO_FILE_H

#include "ocean/io/IO.h"
#include "ocean/io/Path.h"

#include <vector>

namespace Ocean
{

namespace IO
{

// Forward declaration.
class File;

/**
 * Definition of a vector holding files.
 * @ingroup io
 */
typedef std::vector<File> Files;

/**
 * This class holds a file.
 * @ingroup io
 */
class OCEAN_IO_EXPORT File : public Path
{
	public:

		/**
		 * Creates an empty file.
		 */
		File();

		/**
		 * Create a new file.
		 * @param filename File name
		 */
		explicit File(const std::string& filename);

		/**
		 * Returns whether the path is valid.
		 * @see Path::isValid();
		 */
		bool isValid() const override;

		/**
		 * Returns whether the file exists already.
		 * @see Path::exists(), remove().
		 */
		bool exists() const override;

		/**
		 * Removes this file from the filesystem.
		 * @return True, if succeeded
		 * @see exists().
		 */
		bool remove() const;

		/**
		 * Returns the base of this file.
		 * The file's base is the entire file path without file extension (and the character in front of the extension).<br>
		 * The base of e.g. "example.bmp" is "example"; "/first/second/example.txt" is "/first/second/example"
		 * @return File base
		 */
		std::string base() const;

		/**
		 * Returns the extension of this file.
		 * The file's extension of e.g. "example.bmp" is "bmp".
		 * @return File extension
		 */
		std::string extension() const;

		/**
		 * Returns the name of this file.
		 * The file's name is the local filename including the file extension without the prefix path.<br>
		 * The name of e.g. "example.bmp" is "example.bmp"; "/first/second/example.txt" is "example.txt"
		 * @return File name
		 */
		std::string name() const;

		/**
		 * Returns the base name of this file.
		 * The base name is the local filename without extension.<br>
		 * The base name of e.g. "example.bmp" is "example"; "/first/second/example.txt" is "example"
		 * @return File base name
		 */
		std::string baseName() const;

#if defined(__APPLE__)

		/**
		 * Returns whether a file exists (specialization for Apple platforms).
		 * @param file The file to check
		 * @return True, if so
		 */
		static bool existsApple(const std::string& file);

#endif // defined(__APPLE__)
};

/**
 * This class implements a scoped file object which will delete the underlying file from the filesystem when the scope ends.
 * @see File
 * @ingroup io
 */
class ScopedFile final : public File
{
	public:

		/**
		 * Move constructor.
		 * @param scopedFile The scoped file to be moved
		 */
		inline ScopedFile(ScopedFile&& scopedFile);

		/**
		 * Create a new scoped file.
		 * @param file The file for which the new scoped object will be created
		 */
		explicit inline ScopedFile(const File& file);

		/**
		 * Create a new scoped file.
		 * @param filename The name of the file for which a new scoped object will be created
		 */
		explicit inline ScopedFile(const std::string& filename);

		/**
		 * Destructs this object and deletes the actual file.
		 */
		~ScopedFile();

		/**
		 * Move operator.
		 * @param scopedFile The scoped file to be moved
		 * @return Reference to this object
		 */
		ScopedFile& operator=(ScopedFile&& scopedFile);

	protected:

		/**
		 * Disabled copy constructor.
		 * @param scopedFile File which would be copied
		 */
		ScopedFile(const ScopedFile& scopedFile) = delete;

		/**
		 * Disabled copy constructor.
		 * @param scopedFile File which would be copied
		 * @return Reference to this object
		 */
		ScopedFile& operator=(const ScopedFile& scopedFile) = delete;
};

inline ScopedFile::ScopedFile(ScopedFile&& scopedFile) :
	File()
{
	*this = std::move(scopedFile);
}

inline ScopedFile::ScopedFile(const File& file) :
	File(file)
{
	// nothing to do here
}

inline ScopedFile::ScopedFile(const std::string& filename) :
	File(filename)
{
	// nothing to do here
}

}

}

#endif // META_OCEAN_IO_FILE_H
