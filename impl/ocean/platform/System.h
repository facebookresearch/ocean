/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_SYSTEM_H
#define META_OCEAN_PLATFORM_SYSTEM_H

#include "ocean/platform/Platform.h"

namespace Ocean
{

namespace Platform
{

/**
 * This class implements system functions for any platforms.
 * @ingroup platform
 */
class OCEAN_PLATFORM_EXPORT System
{
	public:

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

#endif // META_OCEAN_PLATFORM_SYSTEM_H
