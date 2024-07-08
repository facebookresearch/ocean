/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_VERSION_H
#define META_OCEAN_BASE_VERSION_H

#include "ocean/base/Base.h"
#include "ocean/base/String.h"

/// The major version number of Ocean.
#define OCEAN_VERSION_MAJOR 1

/// The minor version number of Ocean.
#define OCEAN_VERSION_MINOR 0

/// The patch version number of Ocean.
#define OCEAN_VERSION_PATCH 0

/// Indicates whether this is a development or a release version.
#define OCEAN_VERSION_IS_DEVELEOPMENT_BRANCH 1

/// Combines the major, minor, patch, and development branch version numbers into a single version number.
#define OCEAN_VERSIONIZE(major, minor, patch, isDevelopmentBranch) (major * 1000000 + minor * 1000 + patch) * 2 + (isDevelopmentBranch != 0 ? 1 : 0)

/// The full version of Ocean.
#define OCEAN_VERSION OCEAN_VERSIONIZE(OCEAN_VERSION_MAJOR, OCEAN_VERSION_MINOR, OCEAN_VERSION_PATCH, OCEAN_VERSION_IS_DEVELEOPMENT_BRANCH);

namespace Ocean
{

/**
 * This class provides version numbers for Ocean.
 * @ingroup base
 */
class Version
{
	public:

		/**
		 * Returns the major version number of Ocean.
		 * @return The version
		 */
		static constexpr unsigned int major();

		/**
		 * Returns the major version number of Ocean.
		 * @return The version
		 */
		static constexpr unsigned int minor();

		/**
		 * Returns the major version number of Ocean.
		 * @return The version
		 */
		static constexpr unsigned int patch();

		/**
		 * Returns the major version number of Ocean.
		 * @return The version
		 */
		static constexpr bool isDevelopmentBranch();

		/**
		 * Returns the major version number of Ocean.
		 * @return The version
		 */
		static constexpr unsigned int version();

		/**
		 * Returns the version number of Ocean as string.
		 * @return The version
		 */
		static inline std::string versionString();
};

constexpr unsigned int Version::major()
{
	return OCEAN_VERSION_MAJOR;
}

constexpr unsigned int Version::minor()
{
	return OCEAN_VERSION_MINOR;
}

constexpr unsigned int Version::patch()
{
	return OCEAN_VERSION_PATCH;
}

constexpr bool Version::isDevelopmentBranch()
{
	return OCEAN_VERSION_IS_DEVELEOPMENT_BRANCH != 0;
}

constexpr unsigned int Version::version()
{
	return OCEAN_VERSION;
}

inline std::string Ocean::Version::versionString()
{
	return String::toAString(major()) + "." + String::toAString(minor()) + "." + String::toAString(patch()) + (isDevelopmentBranch() ? "-development" : "");
}

}

#endif // META_OCEAN_BASE_VERSION_H
