/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
