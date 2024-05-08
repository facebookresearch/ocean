/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_FILE_RESOLVER_H
#define META_OCEAN_IO_FILE_RESOLVER_H

#include "ocean/io/IO.h"
#include "ocean/io/Directory.h"
#include "ocean/io/File.h"
#include "ocean/io/Path.h"

#include "ocean/base/Singleton.h"

#include <vector>

#include <vector>

namespace Ocean
{

namespace IO
{

/**
 * This class implements a file and url resolver.<br>
 * This resolver is implemented as singleton.<br>
 * However, it can be used as local object anyway.<br>
 * @ingroup io
 */
class OCEAN_IO_EXPORT FileResolver : public Singleton<FileResolver>
{
	public:

		/**
		 * Creates a new resolver object.
		 * Beware: Do not use this constructor if the global resolver object is requestet.
		 */
		FileResolver();

		/**
		 * Registers the reference path.
		 * @param reference Reference path to register
		 * @return True, if the reference path is a valid absolute path
		 */
		bool addReferencePath(const Directory& reference);

		/**
		 * Resolves the absolute paths of a file.
		 * @param file File to resolve
		 * @param checkExistence False, to return file paths even if they do not exist
		 * @return Resolved absolute file paths
		 */
		Files resolve(const File& file, const bool checkExistence = false) const;

		/**
		 * Resolves absolute paths of several files.
		 * @param files Files to resolve
		 * @param checkExistence False, to return file paths even if they do not exist
		 * @return Resolved absolute file paths
		 */
		Files resolve(const Files& files, const bool checkExistence = false) const;

		/**
		 * Resolves the absolute paths of a file by an additional reference path.
		 * @param file File to resolve
		 * @param reference Additional reference path
		 * @param checkExistence False, to return file paths even if they do not existh
		 * @return Resolved absolute file paths
		 */
		Files resolve(const File& file, const Path& reference, const bool checkExistence = false) const;

		/**
		 * Resolves the absolute paths of several file by an additional reference path.
		 * @param files Files to resolve
		 * @param reference Additional reference path
		 * @param checkExistence False, to return file paths even if they do not exist
		 * @return Resolved absolute file paths
		 */
		Files resolve(const Files& files, const Path& reference, const bool checkExistence = false) const;

	private:

		/// Registered reference paths.
		Directories referencePaths;
};

}

}

#endif // META_OCEAN_IO_FILE_RESOLVER_H
