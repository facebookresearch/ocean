/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_UTILITIES_H
#define META_OCEAN_PLATFORM_UTILITIES_H

#include "ocean/platform/Platform.h"

namespace Ocean
{

namespace Platform
{

/**
 * This class implements utilities and helper functions.
 * @ingroup platform
 */
class OCEAN_PLATFORM_EXPORT Utilities
{
	public:

		/**
		 * Definition of a vector holding application commands.
		 */
		typedef std::vector<std::wstring> Commands;

	public:

		/**
		 * Parses the command line and returns the individual command elements.
		 * The entire command line may contain several commands separated by space characters.<br>
		 * Command elements which contain space characters must be surrounded by quotation marks.<br>
		 * However, all surrounding quotation marks will be removed during the parsing process.<br>
		 * The first argument should not be the filename (and path) of the executable.
		 * @param commandLine Command line to be parsed
		 * @return Individual commands
		 */
		static Commands parseCommandLine(const wchar_t* commandLine);

		/**
		 * Shows a modal message box with a title, a message and an OK button.
		 * Beware: This function is supported for Desktop platforms only.
		 * @param title The title to be shown in the box
		 * @param message The message to be shown in the box
		 */
		static void showMessageBox(const std::string& title, const std::string& message);

#ifdef __APPLE__

		/**
		 * Shows a modal message box with a title, a message and an OK button on Apple platforms only.
		 * Beware: This function is supported for Desktop platforms only.
		 * @param title The title to be shown in the box
		 * @param message The message to be shown in the box
		 */
		static void showMessageBoxApple(const std::string& title, const std::string& message);

#endif

};

}

}

#endif // META_OCEAN_PLATFORM_UTILITIES_H
