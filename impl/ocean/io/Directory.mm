/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/Directory.h"

#include "ocean/base/StringApple.h"

#ifndef __APPLE__
	#error This files contains Apple specific functions only, see Directory.cpp
#endif

namespace Ocean
{

namespace IO
{

Directory Directory::createTemporaryDirectoryApple()
{
	NSURL* directoryURL = [NSURL fileURLWithPath:[NSTemporaryDirectory() stringByAppendingPathComponent:[[NSProcessInfo processInfo] globallyUniqueString]] isDirectory:YES];

	unsigned int counter = 0u;
	while (counter++ < 10000u)
	{
		NSError* error = nullptr;

		if ([[NSFileManager defaultManager] createDirectoryAtURL:directoryURL withIntermediateDirectories:YES attributes:nil error:&error] == YES)
		{
			NSString* urlString = [directoryURL absoluteString];

			if ([urlString length] > 0)
			{
				const std::string cString(StringApple::toUTF8(urlString));
				ocean_assert(cString.find("file://") == 0);

				return Directory(cString.substr(7));
			}
		}
	}

	return Directory();
}

bool Directory::existsApple(const std::string& directory)
{
	NSString* path = StringApple::toNSString(directory);

	BOOL isDirectory = NO;

	if ([[NSFileManager defaultManager] fileExistsAtPath:path isDirectory:&isDirectory] == YES)
	{
		return isDirectory == YES;
	}

	return false;
}

} // namespace IO

} // namespace Ocean
