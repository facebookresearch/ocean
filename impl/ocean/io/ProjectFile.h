/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_PROJECT_CONFIG_H
#define META_OCEAN_IO_PROJECT_CONFIG_H

#include "ocean/io/FileConfig.h"

namespace Ocean
{

namespace IO
{

/**
 * This class implements an project file loader.
 * @ingroup io
 */
class OCEAN_IO_EXPORT ProjectFile final : protected FileConfig
{
	public:

		/**
		 * Definition of a vector holding a filenames.
		 */
		typedef std::vector<std::string> Filenames;

	public:

		/**
		 * Creates a new project file object.
		 */
		ProjectFile() = default;

		/**
		 * Creates a new project file object by a given configuration file.
		 * @param filename Configuration file
		 */
		ProjectFile(const std::string& filename);

		/**
		 * Returns the file types registered inside the project file.
		 * @return Project files
		 */
		inline const Filenames& filenames() const;

		/**
		 * Returns whether the project files exists.
		 * @return True, if so
		 */
		inline bool fileExists() const;

	protected:

		/// Parsed project files.
		Filenames filenames_;

		/// State determining whether the project file exists.
		bool fileExists_ = false;
};

inline const ProjectFile::Filenames& ProjectFile::filenames() const
{
	return filenames_;
}

inline bool ProjectFile::fileExists() const
{
	return fileExists_;
}

}

}

#endif // META_OCEAN_IO_FILE_CONFIG_H
