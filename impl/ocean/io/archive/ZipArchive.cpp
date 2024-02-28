// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/io/archive/ZipArchive.h"

#include "ocean/base/Memory.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include <safe_unzip/SafeUnzip.h>

#include <boost/filesystem.hpp>

#include <fstream>
#include <type_traits>

using namespace facebook::security::safe_unzip;

namespace Ocean
{

namespace IO
{

namespace Archive
{

/**
 * Definition of the internal class.
 */
class ZipArchive::Internal
{
	public:

		/**
		 * Writes a directory or file from a zip archive to disk.
		 * @param safeUnzipper The instance of the safe_unzip library, must be valid
		 * @param elementName The name of the element that will be unzipped, must be valid
		 * @param targetDirectory The directory into which the unzipped element will be copied, must be valid
		 * @param overwriteFiles If true, existing target files will be overwritten, otherwise this function will abort
		 * @return True, if succeeded
		 */
		static bool writeZipDirectoryOrFileToDisk(SafeUnzipper& safeUnzipper, const std::string& elementName, const IO::Directory& targetDirectory, const bool overwriteFiles);

		/**
		 * Writes a file from a zip archive to disk.
		 * @param safeUnzipper The instance of the safe_unzip library, must be valid
		 * @param elementName The name of the element that will be unzipped, must be valid
		 * @param targetFile The target file into which the unzipped element will be copied, must be valid and not exist
		 * @return True, if succeeded
		 */
		static bool writeZipFileToDisk(SafeUnzipper& safeUnzipper, const std::string& elementName, const IO::File& targetFile);
};

bool ZipArchive::Internal::writeZipDirectoryOrFileToDisk(SafeUnzipper& safeUnzipper, const std::string& elementName, const IO::Directory& targetDirectory, const bool overwriteFiles)
{
	if (elementName.empty())
	{
		Log::error() << "ZipArchive: Found empty names in zip archive - aborting to unzip!";
		return false;
	}

	std::string internalElementName = elementName;

	if (internalElementName.back() == Path::defaultSeparator())
	{
		internalElementName.pop_back();
		ocean_assert(internalElementName.back() != Path::defaultSeparator());
	}

	const std::string canonicalName = boost::filesystem::weakly_canonical(targetDirectory() + internalElementName).string();

	// Prevent archive elements from being created outside the specified target directory.
	if (canonicalName.size() < targetDirectory().size() || canonicalName.substr(0, targetDirectory().size()) != targetDirectory())
	{
		Log::error() << "ZipArchive: element from zip archive is escaping the target directory - aborting to unzip!";
		return false;
	}

	if (safeUnzipper.isDirectory(elementName))
	{
		// we have a directory

		const IO::Directory directory(canonicalName);

		return directory.create();
	}

	// we have a file

	const IO::File file(canonicalName);

	if (file.exists())
	{
		if (overwriteFiles)
		{
			if (!file.remove())
			{
				Log::error() << "ZipArchive: Could not overwrite file '" << file() << "'";
				return false;
			}
		}
		else
		{
			Log::error() << "ZipArchive: File '" << file() << "' exist already";
			return false;
		}
	}

	const IO::Directory directory(file);

	if (!directory.exists())
	{
		// The file's directory is not part of the zip archive - therefore, we skip the file

#ifdef OCEAN_DEBUG
		Log::warning() << "<debug> ZipArchive: Skipping file without corresponding directory '" << file() << "'";
#endif

		return true;
	}

	return writeZipFileToDisk(safeUnzipper, elementName, file);
}

bool ZipArchive::Internal::writeZipFileToDisk(SafeUnzipper& safeUnzipper, const std::string& elementName, const IO::File& targetFile)
{
	ocean_assert(!elementName.empty());
	ocean_assert(!targetFile.exists() && targetFile.isValid());

	std::ofstream stream(targetFile().c_str(), std::ios::binary);

	if (!stream.good())
	{
		Log::error() << "ZipArchive: Could not open the target file '" << targetFile() << "'";
		return false;
	}

	const std::unique_ptr<SafeUnzipStat> fileInfo = safeUnzipper.getFileInfo(elementName);

	if (fileInfo == nullptr)
	{
		Log::error() << "ZipArchive: No file information found for element '" << elementName << "'.";
		return false;
	}

	const std::unique_ptr<folly::IOBuf> buffer = safeUnzipper.extractFile(elementName);

	if (buffer == nullptr)
	{
		Log::error() << "ZipArchive: Failed to extract element '" << elementName << "'.";
		return false;
	}

	if (buffer->length() != size_t(fileInfo->size))
	{
		Log::error() << "ZipArchive: mismatch between buffer size and expected file size";
		return false;
	}

	stream.write((const char*)(buffer->data()), buffer->length());

	return true;
}

bool ZipArchive::extractZipArchive(const std::string& zipArchiveFilename, const std::string& targetDirectoryName, const bool createTargetDirectory, const bool overwriteFiles, const uint64_t maxFileSizeBytes)
{
	ocean_assert(maxFileSizeBytes != 0ull);

	if (zipArchiveFilename.empty() || targetDirectoryName.empty())
	{
		ocean_assert(false && "Invalid input");
		return false;
	}

	if (maxFileSizeBytes > (uint64_t)(std::numeric_limits<int>::max()))
	{
		Log::error() << "ZipArchive: max. file size exceeds value range of safe_unzip parameter - risk of integer overflow, aborting to unzip";
		return false;
	}

	std::string internalTargetDirectoryName = targetDirectoryName;

	// Remove any trailing '/' or else Boost is going to add a redundant './' to the canonical name.
	while (internalTargetDirectoryName.size() > 1 && internalTargetDirectoryName.back() == Path::defaultSeparator())
	{
		internalTargetDirectoryName.pop_back();
	}

	internalTargetDirectoryName = boost::filesystem::weakly_canonical(internalTargetDirectoryName).string();

	const IO::Directory targetDirectory(internalTargetDirectoryName);

	if (!targetDirectory.exists())
	{
		if (createTargetDirectory)
		{
			if (!targetDirectory.create())
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	try
	{
		SafeUnzipper::Options options;
		options.setExtractFileSizeLimit(int(maxFileSizeBytes));
		options.setSanitizeFileNames(false);

		SafeUnzipper safeUnzipper(boost::filesystem::path(zipArchiveFilename), options);

		std::set<std::string> elementsInArchive;
		for (const std::unique_ptr<SafeUnzipStat>& fileInfo : safeUnzipper.getFileInfos())
		{
			ocean_assert(fileInfo != nullptr);

			if (elementsInArchive.find(fileInfo->name) != elementsInArchive.end())
			{
				Log::error() << "ZipArchive: Found duplicate file names in zip archive - aborting to unzip!";
				return false;
			}

			elementsInArchive.insert(fileInfo->name);
		}

		for (const std::string& element : elementsInArchive)
		{
			if (!Internal::writeZipDirectoryOrFileToDisk(safeUnzipper, element, targetDirectory, overwriteFiles))
			{
				return false;
			}
		}
	}
	catch (SafeUnzipException& exception)
	{
		Log::error() << "ZipArchive: " << exception.what();
		return false;
	}

	return true;
}

}

}

}
