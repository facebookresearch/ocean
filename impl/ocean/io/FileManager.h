/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_FILE_MANAGER_H
#define META_OCEAN_IO_FILE_MANAGER_H

#include "ocean/io/IO.h"

#include <map>

namespace Ocean
{

namespace IO
{

/**
 * This class is the base class for all plugin manager able to load and manager files.
 * @ingroup io
 */
class OCEAN_IO_EXPORT FileManager
{
	public:

		/**
		 * Definition of a map mapping supported file extensions to file type descriptions.
		 */
		typedef std::map<std::string, std::string> FileExtensions;

	public:

		/**
		 * Returns all currently supported file extensions.
		 * @return All supported file extensions
		 */
		virtual FileExtensions supportedExtensions() = 0;

		/**
		 * Returns whether a specified file extension is supported.
		 * @param extension Extension to check
		 * @return True, if so
		 */
		virtual bool isSupported(const std::string& extension);

	protected:

		/**
		 * Creates a new file manager object.
		 */
		FileManager();

		/**
		 * Destructs a file manager object.
		 */
		virtual ~FileManager();
};

}

}

#endif // META_OCEAN_IO_FILE_MANAGER_H
