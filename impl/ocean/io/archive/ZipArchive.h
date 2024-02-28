// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_IO_ARCHIVE_ZIP_ARCHIVE_H
#define META_OCEAN_IO_ARCHIVE_ZIP_ARCHIVE_H

#include "ocean/io/archive/Archive.h"

namespace Ocean
{

namespace IO
{

namespace Archive
{

/**
 * This class implements zip archive functionalities.
 * @ingroup ioarchive
 */
class OCEAN_IO_ARCHIVE_EXPORT ZipArchive
{
	protected:

		/// Forward declaration of an internal class.
		class Internal;

	public:

		/**
		 * Extracts a zip archive to a target directory.
		 * @param zipArchiveFilename The filename of the zip archive to extract, must be valid
		 * @param targetDirectory The name of the target directory
		 * @param createTargetDirectory True, to create the target directory if not existing; False, to stop if the target directory does not exist
		 * @param overwriteFiles True, to override existing files; False, to stop immediately in case an existing file would be overwritten
		 * @param maxFileSizeBytes Optional maximum size in bytes of the uncompressed files before this function aborts, range: [1, 2^31 - 1]
		 * @return True, if succeeded
		 */
		static bool extractZipArchive(const std::string& zipArchiveFilename, const std::string& targetDirectory, const bool createTargetDirectory, const bool overwriteFiles = false, const uint64_t maxFileSizeBytes = (1ull << 31ull) - 1ull /* 2 GB - 1 Byte*/);
};

}

}

}

#endif // META_OCEAN_IO_ARCHIVE_ZIP_ARCHIVE_H
