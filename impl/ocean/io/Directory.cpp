/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/base/String.h"

#ifdef _WINDOWS
	#include "ocean/base/RandomI.h"

	#include <winsock2.h>
	#include <windows.h>
#else
	#include <dirent.h>
	#include <errno.h>

	#include <sys/stat.h>
#endif

#ifdef _ANDROID
	#include "ocean/base/RandomI.h"
#endif

namespace Ocean
{

namespace IO
{

Directory::Directory() :
	Path()
{
	// nothing to do here
}

Directory::Directory(const std::string& path) :
	Path(path)
{
	checkPath(TYPE_DIRECTORY);
}

Directory::Directory(const File& file) :
	Path()
{
	if (file.isValid())
	{
		for (unsigned int n = (unsigned int)(file().length() - 1); n < (unsigned int)(file().length()); --n)
		{
			if (isSeparator(file()[n]))
			{
				pathValue_ = file().substr(0, n + 1);
				checkPath(TYPE_DIRECTORY);
				break;
			}
		}
	}
}

Directory::Directory(const Path& path) :
	Path()
{
	if (path.isValid())
	{
		if (path.type() & TYPE_DIRECTORY)
		{
			pathValue_ = path();
			pathType_ = path.type();
		}
		else
		{
			*this = Directory((File&)path);
		}
	}
}

bool Directory::isValid() const
{
	return (type() & TYPE_DIRECTORY) != 0;
}

bool Directory::exists() const
{
	const std::string& directoryPath = (*this)();

#if defined(_WINDOWS)

	const DWORD result = GetFileAttributesA(directoryPath.c_str());
	if (result == INVALID_FILE_ATTRIBUTES)
	{
		return false;
	}

	return (result & FILE_ATTRIBUTE_DIRECTORY) != 0;

#elif defined(__APPLE__)

	return existsApple(directoryPath);

#else

	struct stat data;
	if (stat(directoryPath.c_str(), &data) == 0)
	{
		return (data.st_mode & S_IFDIR) != 0;
	}

	return false;

#endif
}

bool Directory::create() const
{
	ocean_assert(isValid());

#ifdef _WINDOWS

	if (CreateDirectoryA(pathValue_.c_str(), nullptr) == TRUE)
	{
		return true;
	}

	const DWORD error = GetLastError();

	if (error == ERROR_ALREADY_EXISTS)
	{
		return true;
	}

	if (error == ERROR_PATH_NOT_FOUND)
	{
		Directory copy(*this);
		--copy;

		if (!copy.isValid() || !copy.create())
		{
			return false;
		}

		return CreateDirectoryA(pathValue_.c_str(), nullptr) == TRUE;
	}

#else

	if (mkdir(pathValue_.c_str(), S_IRUSR | S_IWUSR | S_IXUSR) == 0)
	{
		return true;
	}

	if (errno == EEXIST)
	{
		return true;
	}

	if (errno == ENOENT)
	{
		Directory copy(*this);
		--copy;

		if (!copy.isValid() || !copy.create())
		{
			return false;
		}

		return mkdir(pathValue_.c_str(), S_IRUSR | S_IWUSR | S_IXUSR) == 0;
	}

#endif

	return false;
}

bool Directory::remove(const bool recursively) const
{
	ocean_assert(isValid());

	if (pathValue_.empty())
	{
		return false;
	}

	bool allSucceeded = true;

	if (recursively)
	{
		const Files files = findFiles("*");

		for (const File& file : files)
		{
			if (!file.remove())
			{
				allSucceeded = false;
			}
		}

		const Directories directories = findDirectories(false /*recursively*/);

		for (const Directory& directory : directories)
		{
			if (!directory.remove(true /*recursively*/))
			{
				allSucceeded = false;
			}
		}
	}

#ifdef _WINDOWS
	if (RemoveDirectoryA(pathValue_.c_str()) != TRUE)
	{
		allSucceeded = false;
	}
#else
	if (::remove(pathValue_.c_str()) != 0)
	{

		allSucceeded = false;
	}
#endif

	return allSucceeded;
}

std::string Directory::base() const
{
	if (!isValid())
	{
		return std::string();
	}

	const std::string& directoryName = (*this)();

	ocean_assert(!directoryName.empty());
	ocean_assert(isSeparator(directoryName.back()));

	return directoryName.substr(0, directoryName.size() - 1);
}

std::string Directory::name() const
{
	if (!isValid() || !exists())
	{
		return std::string();
	}

	// Remove trailing delimiters, '/' or '\'
	std::string directoryName = (*this)();

	while (!directoryName.empty() && isSeparator(directoryName.back()))
	{
		directoryName.pop_back();
	}

	if (directoryName.empty())
	{
		return std::string();
	}

	// Extract the name of the last element
	const size_t pos = directoryName.find_last_of("/\\");

	if (pos == std::string::npos)
	{
		return std::string();
	}

	return directoryName.substr(pos + 1);
}

Files Directory::findFiles(const std::string& fileType, const bool recursive) const
{
	Files returnFiles;

#ifdef _WINDOWS

	const std::string lowerFileType(String::toLower(fileType));

	const std::string path = pathValue_ + std::string("*.") + lowerFileType;

	WIN32_FIND_DATAA data;
	HANDLE handle = FindFirstFileA(path.c_str(), &data);
	if (handle != INVALID_HANDLE_VALUE)
	{
		do
		{
			// we are interested for files (not directories)
			if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				const std::string filename(data.cFileName);

				ocean_assert(!filename.empty());
				ocean_assert(filename != std::string("."));
				ocean_assert(filename != std::string(".."));

				if (!filename.empty())
				{
					returnFiles.push_back(File(pathValue_ + filename));
					ocean_assert(String::toLower(returnFiles.back().extension()) == lowerFileType || fileType == "*");
				}
			}
		}
		while (FindNextFileA(handle, &data));

		FindClose(handle);
	}

#else

	DIR* directory = opendir(pathValue_.c_str());
	if (directory != nullptr)
	{
		dirent* entry = readdir(directory);

		while (entry != nullptr)
		{
			// we are interested for files (not directories)
			if ((entry->d_type & DT_DIR) == 0)
			{
				const std::string filename(entry->d_name);

				ocean_assert(!filename.empty());
				ocean_assert(filename != std::string("."));
				ocean_assert(filename != std::string(".."));

				if (!filename.empty())
				{
					const File file(filename);

					if (file.extension() == fileType || fileType == "*")
					{
						returnFiles.push_back(File(pathValue_ + filename));
					}
				}
			}

			entry = readdir(directory);
		}

		closedir(directory);
	}

#endif

	if (recursive)
	{
		const Directories recursiveChildDirectories = findDirectories(true /*recursive*/);

		for (const Directory& recursiveChildDirectory : recursiveChildDirectories)
		{
			const IO::Files recursiveFiles = recursiveChildDirectory.findFiles(fileType, false /*recursive*/);

			returnFiles.insert(returnFiles.cend(), recursiveFiles.cbegin(), recursiveFiles.cend());
		}
	}

#ifdef OCEAN_DEBUG
	// enusing that we do not have a file twice

	std::set<std::string> debugSet;
	for (const File& returnFile : returnFiles)
	{
		ocean_assert(debugSet.find(returnFile()) == debugSet.cend());
		debugSet.insert(returnFile());
	}
	ocean_assert(debugSet.size() == returnFiles.size());
#endif

	return returnFiles;
}

Directories Directory::findDirectories(const bool recursive) const
{
	Directories returnDirectories;

#ifdef _WINDOWS

	const std::string path = pathValue_ + std::string("*.*");

	WIN32_FIND_DATAA data;
	HANDLE handle = FindFirstFileA(path.c_str(), &data);
	if (handle != INVALID_HANDLE_VALUE)
	{
		do
		{
			// we are interested for directories (not files)
			if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
			{
				const std::string directoryName(data.cFileName);

				ocean_assert(!directoryName.empty());

				if (!directoryName.empty() && directoryName != std::string(".") && directoryName != std::string(".."))
				{
					returnDirectories.push_back(Directory(pathValue_ + directoryName));
				}
			}
		}
		while (FindNextFileA(handle, &data));

		FindClose(handle);
	}

#else

	DIR* directory = opendir(pathValue_.c_str());
	if (directory != nullptr)
	{
		dirent* entry = readdir(directory);

		while (entry != nullptr)
		{
			// we are interested for files (not directories)
			if ((entry->d_type & DT_DIR) == DT_DIR)
			{
				const std::string directoryName(entry->d_name);

				ocean_assert(!directoryName.empty());

				// Don't add the default files "." and ".." from a directory
				if (!directoryName.empty() && directoryName != std::string(".") && directoryName != std::string(".."))
				{
					returnDirectories.push_back(Directory(pathValue_ + directoryName));
				}
			}

			entry = readdir(directory);
		}

		closedir(directory);
	}

#endif

	if (recursive)
	{
		const size_t size = returnDirectories.size();

		for (size_t n = 0; n < size; ++n)
		{
			const Directories childDirectories = returnDirectories[n].findDirectories(true /*recursive*/);

			returnDirectories.insert(returnDirectories.cend(), childDirectories.cbegin(), childDirectories.cend());
		}
	}

#ifdef OCEAN_DEBUG
	// enusing that we do not have a directory twice

	std::set<std::string> debugSet;
	for (const Directory& returnDirectory : returnDirectories)
	{
		ocean_assert(debugSet.find(returnDirectory()) == debugSet.cend());
		debugSet.insert(returnDirectory());
	}
	ocean_assert(debugSet.size() == returnDirectories.size());
#endif

	return returnDirectories;
}

File Directory::operator+(const File& file) const
{
	if (isValid() == false)
	{
		return file;
	}

	if (file.isValid() == false || file.isRelative() == false)
	{
		return File();
	}

	ocean_assert(File(pathValue_ + file()).isValid());
	return File(pathValue_ + file());
}

Directory Directory::operator+(const Directory& path) const
{
	if (isValid() == false)
	{
		return path;
	}

	if (path.isValid() == false || path.isRelative() == false)
	{
		return Directory();
	}

	ocean_assert(Directory(pathValue_ + path.pathValue_).isValid());
	return Directory(pathValue_ + path.pathValue_);
}

Directory& Directory::operator+=(const Directory& path)
{
	if (isValid() == false || path.isValid() == false || path.isRelative() == false)
	{
		pathType_ = TYPE_INVALID;
		return *this;
	}

	pathValue_ += path.pathValue_;
	checkPath(TYPE_DIRECTORY);
	ocean_assert(isValid());

	return *this;
}

Directory& Directory::operator--()
{
	ocean_assert(isValid());
	ocean_assert(!pathValue_.empty() && isSeparator(pathValue_[pathValue_.size() - 1]));

	for (size_t n = pathValue_.length() - 2; n != size_t(-1); --n)
	{
		if (isSeparator(pathValue_[n]))
		{
			pathValue_ = pathValue_.substr(0, n + 1);

			if (pathValue_.size() == 2 && isSeparator(pathValue_[0]) && isSeparator(pathValue_[1]))
			{
				pathValue_.clear();
				pathType_ = TYPE_INVALID;
			}

			return *this;
		}
	}

	ocean_assert(false && "Invalid path!");
	return *this;
}

Directory Directory::operator--(int)
{
	ocean_assert(isValid());
	ocean_assert(!pathValue_.empty() && isSeparator(pathValue_[pathValue_.size() - 1]));

	const Directory copy(*this);

	for (size_t n = pathValue_.length() - 2; n != size_t(-1); --n)
	{
		if (isSeparator(pathValue_[n]))
		{
			pathValue_ = pathValue_.substr(0, n + 1);

			if (pathValue_.size() == 2 && isSeparator(pathValue_[0]) && isSeparator(pathValue_[1]))
			{
				pathValue_.clear();
				pathType_ = TYPE_INVALID;
			}

			return copy;
		}
	}

	ocean_assert(false && "Invalid path!");
	return Directory();
}

Directory Directory::createTemporaryDirectory()
{
#if defined(_WINDOWS)

	static_assert(MAX_PATH > 0, "Value of macro MAX_PATH is zero");
	char temporaryDirPath[MAX_PATH + 1u] = {0};
	const DWORD result = GetTempPathA(MAX_PATH, temporaryDirPath);

	if (result > MAX_PATH || (result == 0u))
	{
		return Directory();
	}

	const Directory parentDirectory(temporaryDirPath);

	for (unsigned int iterations = 0u; iterations < 1000u; ++iterations)
	{
		const Directory directory(parentDirectory + Directory(String::toAStringHex(RandomI::random64())));

		if (!directory.exists())
		{
			if (directory.create())
			{
				return directory;
			}
		}
	}

	return Directory();

#elif defined(__APPLE__)

	return createTemporaryDirectoryApple();

#elif defined(_ANDROID)

	const char* temporaryDirectory = getenv("TMPDIR");
	ocean_assert(temporaryDirectory != nullptr);

	if (temporaryDirectory == nullptr)
	{
		return Directory();
	}

	const std::string tempDirectoryPath = temporaryDirectory + String::toAString(RandomI::random64());
	Directory result = Directory(tempDirectoryPath);

	if (!result.create())
	{
		Log::error() << "Failed to create the temporary directory: " << result();
		result = Directory();
	}

	return result;

#elif defined(__linux__)

	char tmpl[] = "/var/tmp/XXXXXXXXXXXXXXXXXXXX";
	char* path = mkdtemp(tmpl);

	if (path == nullptr)
	{
		Log::error() << "Failed to create the temporary directory: " << path;
		return Directory();
	}
	return Directory(path);

#else

	OCEAN_WARNING_MISSING_IMPLEMENTATION;
	return Directory();

#endif

}

ScopedDirectory::~ScopedDirectory()
{
	if (exists())
	{
		const bool result = remove(true /*recursively*/);
		ocean_assert_and_suppress_unused(result, result);
	}
}

ScopedDirectory& ScopedDirectory::operator=(ScopedDirectory&& scopedDirectory)
{
	if (this != &scopedDirectory)
	{
		pathType_ = scopedDirectory.pathType_;
		scopedDirectory.pathType_ = TYPE_INVALID;

		pathValue_ = std::move(scopedDirectory.pathValue_);
	}

	return *this;
}

}

}
