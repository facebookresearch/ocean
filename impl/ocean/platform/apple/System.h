/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_APPLE_SYSTEM_H
#define META_OCEAN_PLATFORM_APPLE_SYSTEM_H

#include "ocean/platform/apple/Apple.h"

namespace Ocean
{

namespace Platform
{

namespace Apple
{

/**
 * This class implements system functions on Apple platforms.
 * @ingroup platformapple
 */
class System
{
	public:

		/**
		 * Returns a defined environment variable.
		 * @param variable Environment variable to return, must be valid
		 * @param removeQuotes Determines whether all beginning and ending quotes are removed from the environment variable, if existent
		 * @return Specified environment variable
		 */
		static std::string environmentVariable(const std::string& variable, const bool removeQuotes = true);

		/**
		 * Sets an environment variable.
		 * @param variable Environment variable to set, must be valid
		 * @param value The new value of the specified variable, an empty value remove the environment variable
		 * @return True, if succeeded
		 */
		static bool setEnvironmentVariable(const std::string& variable, const std::string& value);

		/**
		 * Returns the current Apple-platform version.
		 * @param major Optional resulting major version number
		 * @param minor Optional resulting minor version number
		 * @return The Apple-platform version determined by major + minor / 100
		 */
		static double version(unsigned int* major = nullptr, unsigned int* minor = nullptr);

		/**
		 * Returns whether the current Apple-platform version is equal or larger than a specified (minimal required version number).
		 * @param minimalMajor The minimal major version number the current Apple-platform must have, with range [10, infinity)
		 * @param minimalMinor THe minimal minor version number the current Apple-platform must have, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool isMinimalVersion(const unsigned int minimalMajor, const unsigned int minimalMinor);

		/**
		 * Returns the path of the home directory of the calling process.
		 * The home directory can be the application's sandbox directory or the current user's home directory (depending on the application and platform).
		 * @return The path of the home directory
		 */
		static std::string homeDirectory();

		/**
		 * Explicitly runs the main loop for a specified time interval.
		 * This function can be used to force event handling which is done in the main loop e.g., in a console application in macOS.
		 * @param interval The time interval the main loop will be executed, in seconds, with range (0, infinity)
		 */
		static void runMainLoop(const double interval);

		/**
		 * Returns whether the process ia a bundle-based app or a command line app.
		 * @return True, if the app is a bundle-based app
		 */
		static bool isBundleApp();
};

}

}

}

#endif // META_OCEAN_PLATFORM_APPLE_SYSTEM_H
