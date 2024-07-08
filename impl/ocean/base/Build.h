/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_BUILD_H
#define META_OCEAN_BASE_BUILD_H

#include "ocean/base/Base.h"

namespace Ocean
{

/**
 * This class provides compiler build support.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT Build
{
	public:

		/**
		 * Returns a string holding the platform type.
		 * Platform type can be "android", "ios", "osx" or "win".
		 * @return System type
		 */
		static std::string platformType();

		/**
		 * Returns a string holding the target architecture type.
		 * Platform type can be e.g. "arm", "arm64", "x64", "x86", or "i386"
		 * @return Target platform
		 */
		static std::string architectureType();

		/**
		 * Returns a string holding the release type.
		 * In debug mode "debug" is returned, in release mode an empty string is returned.
		 * @return Release type
		 */
		static std::string releaseType();

		/**
		 * Returns a string holding the runtime type.
		 * Possible values are "shared" or "static"
		 * @return Runtime type
		 */
		static std::string runtimeType();

		/**
		 * Returns a string holding the compiler version.
		 * Compiler version are e.g. "gc48", "gc49", "xc6", "xc8", "cl38", "vc08", "vc09", "vc10", "vc11", "vc12", or "vc14".
		 * @return Compiler version
		 */
		static std::string compilerVersion();

		/**
		 * Returns a build string composed of platform type, architecture type, compiler version and release type.
		 * @return Build string
		 */
		static std::string buildString();

		/**
		 * Returns the date of the compiler building process as string.
		 * @param date Compiler build date which must be __DATE__
		 * @return Date of build process
		 */
		static std::string buildDate(const char* date);

		/**
		 * Returns the time of the compiler building process as string.
		 * @param time Compiler build time which must be __TIME__
		 * @return Time of build process
		 */
		static std::string buildTime(const char* time);
};

}

#endif // META_OCEAN_BASE_BUILD_H
