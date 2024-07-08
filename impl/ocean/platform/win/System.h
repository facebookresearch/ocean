/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WIN_SYSTEM_H
#define META_OCEAN_PLATFORM_WIN_SYSTEM_H

#include "ocean/platform/win/Win.h"

#include "ocean/io/Directory.h"

namespace Ocean
{

namespace Platform
{

namespace Win
{

/**
 * This class provides basic system functionalities on windows platforms.
 * @ingroup platformwin
 */
class OCEAN_PLATFORM_WIN_EXPORT System
{
	public:

		/**
		 * Returns the current directory used for this process.
		 * @return Current directory
		 */
		static IO::Directory currentDirectory();

		/**
		 * Returns the process directory used for this process.
		 * @return Process directory
		 */
		static IO::Directory processDirectory();

		/**
		 * Returns the handle to the calling module (a runtime library).
		 * This function returns nullptr if the calling module is an application.<br>
		 * Every valid module handle must be released by an explicit FreeLibrary() call.<br>
		 * @return The handle of the calling module library, nullptr otherwise
		 */
		static HMODULE currentLibraryModule();

		/**
		 * Returns the corresponding name (the filename) of a given module handle.
		 * @param handle The handle for that the corresponding name is returned
		 * @return Resulting handle handle, if any
		 */
		static std::wstring moduleName(const HMODULE handle);

		/**
		 * Returns a defined environment variable.
		 * @param variable Environment variable to return
		 * @param removeQuotes Determines whether all beginning and ending quotes are removed from the environment variable, if existent
		 * @return Specified environment variable
		 */
		static std::string environmentVariable(const std::string& variable, const bool removeQuotes = true);
};

}

}

}

#endif // META_OCEAN_PLATFORM_WIN_SYSTEM_H
