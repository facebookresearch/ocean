/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/File.h"

#include "ocean/base/StringApple.h"

#ifndef __APPLE__
	#error This files contains Apple specific functions only, see Directory.cpp
#endif

namespace Ocean
{

namespace IO
{

bool File::existsApple(const std::string& file)
{
	NSString* path = StringApple::toNSString(file);

	BOOL isDirectory = YES;

	if ([[NSFileManager defaultManager] fileExistsAtPath:path isDirectory:&isDirectory] == YES)
	{
		return isDirectory == NO;
	}

	return false;
}

} // namespace IO

} // namespace Ocean
