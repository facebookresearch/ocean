/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/FileRecorder.h"

#include "ocean/base/DateTime.h"
#include "ocean/base/String.h"

#include "ocean/io/File.h"

namespace Ocean
{

namespace Media
{

FileRecorder::FileRecorder() :
	Recorder(),
	recorderFilenameSuffixed(true)
{
	recorderType = Type(recorderType | FILE_RECORDER);
}

FileRecorder::~FileRecorder()
{
	// nothing to do here
}

bool FileRecorder::setFilename(const std::string& filename)
{
	recorderFilename = filename;
	return true;
}

bool FileRecorder::setFilenameSuffixed(const bool suffixed)
{
	recorderFilenameSuffixed = suffixed;
	return true;
}

std::string FileRecorder::addOptionalSuffixToFilename(const std::string& filename, const bool addSuffix)
{
	const IO::File combinedFilename(filename);

	std::string dateTime;

	if (addSuffix)
	{
		dateTime = " " + DateTime::localStringDate('.') + " " + DateTime::localStringTime(false, '-');
	}

	return combinedFilename.base() + dateTime + std::string(".") + combinedFilename.extension();
}

std::string FileRecorder::addOptionalSuffixToFilename(const std::string& filename, const unsigned int index, const bool addSuffix)
{
	const IO::File combinedFilename(filename);

	std::string dateTime;

	if (addSuffix)
	{
		dateTime = " " + DateTime::localStringDate('.') + " " + DateTime::localStringTime(false, '-') + " ";
	}

	return combinedFilename.base() + dateTime + String::toAString(index, 5u) + std::string(".") + combinedFilename.extension();
}

}

}
