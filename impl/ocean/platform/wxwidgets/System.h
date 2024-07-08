/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WXWIDGETS_SYSTEM_H
#define META_OCEAN_PLATFORM_WXWIDGETS_SYSTEM_H

#include "ocean/platform/wxwidgets/WxWidgets.h"

#include "ocean/io/Directory.h"

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

/**
 * This class provides basic system functionalities on wxwidgets platforms.
 * @ingroup platformwxw
 */
class OCEAN_PLATFORM_WXWIDGETS_EXPORT System
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

#endif // META_OCEAN_PLATFORM_WXWIDGETS_SYSTEM_H
