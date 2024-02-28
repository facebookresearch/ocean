// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/io/FileManager.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace IO
{

FileManager::FileManager()
{
	// nothing to do here
}

FileManager::~FileManager()
{
	// nothing to do here
}

bool FileManager::isSupported(const std::string& extension)
{
	const FileExtensions fileExtensions(supportedExtensions());
	return fileExtensions.find(String::toLower(extension)) != fileExtensions.end();
}

}

}
