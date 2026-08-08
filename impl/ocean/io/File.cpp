/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/File.h"

#include "ocean/base/String.h"

#ifdef _WINDOWS
	#include <winsock2.h>
	#include <windows.h>
#else
	#include <sys/stat.h>
#endif

namespace Ocean
{

namespace IO
{

File::File() :
	Path()
{
	// nothing  to do here
}

File::File(const std::string& filename) :
	Path(filename)
{
	checkPath(TYPE_FILE);
}

bool File::isValid() const
{
	return (type() & TYPE_FILE) != 0;
}

bool File::exists() const
{
	const std::string& filePath = (*this)();

#if defined(_WINDOWS)

	WIN32_FIND_DATAW data;
	HANDLE handle = FindFirstFileW(String::toWString(filePath).c_str(), &data);

	if (handle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	const bool result = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
	FindClose(handle);

	return result;

#elif defined(__APPLE__)

	return existsApple(filePath);

#else

	struct stat data;
	if (stat(filePath.c_str(), &data) == 0)
	{
		return (data.st_mode & S_IFREG) != 0;
	}

	return false;

#endif
}

bool File::remove() const
{
	ocean_assert(isValid());

	if (pathValue_.empty())
	{
		return false;
	}

	return ::remove(pathValue_.c_str()) == 0;
}

std::string File::base() const
{
	ocean_assert(isValid());

	const std::string::size_type dotPosition = extensionPosition(pathValue_);

	if (dotPosition == std::string::npos)
	{
		return pathValue_;
	}

	return pathValue_.substr(0, dotPosition);
}

std::string File::extension() const
{
	ocean_assert(isValid());

	const std::string::size_type dotPosition = extensionPosition(pathValue_);

	if (dotPosition == std::string::npos)
	{
		return std::string();
	}

	return pathValue_.substr(dotPosition + 1);
}

std::string File::name() const
{
	ocean_assert(isValid());

	for (size_t n = pathValue_.size() - 1; n != (size_t)-1; --n)
	{
		if (isSeparator(pathValue_[n]))
		{
			return pathValue_.substr(n + 1);
		}
	}

	return pathValue_;
}

std::string File::baseName() const
{
	ocean_assert(isValid());

	// the local name starts behind the last separator

	std::string::size_type nameStart = 0;

	for (size_t n = pathValue_.size() - 1; n != (size_t)(-1); --n)
	{
		if (isSeparator(pathValue_[n]))
		{
			nameStart = n + 1;
			break;
		}
	}

	const std::string::size_type dotPosition = extensionPosition(pathValue_);

	if (dotPosition == std::string::npos)
	{
		return pathValue_.substr(nameStart);
	}

	return pathValue_.substr(nameStart, dotPosition - nameStart);
}

std::string::size_type File::extensionPosition(const std::string& path)
{
	const std::string::size_type dotPosition = path.rfind('.');

	if (dotPosition == std::string::npos)
	{
		return std::string::npos;
	}

	// a dot within a directory does not separate an extension

	for (std::string::size_type n = dotPosition + 1; n < path.size(); ++n)
	{
		if (isSeparator(path[n]))
		{
			return std::string::npos;
		}
	}

	return dotPosition;
}

ScopedFile::~ScopedFile()
{
	if (exists())
	{
		remove();
	}
}

ScopedFile& ScopedFile::operator=(ScopedFile&& scopedFile)
{
	if (this != &scopedFile)
	{
		pathType_ = scopedFile.pathType_;
		scopedFile.pathType_ = TYPE_INVALID;

		pathValue_ = std::move(scopedFile.pathValue_);
	}

	return *this;
}

}

}
