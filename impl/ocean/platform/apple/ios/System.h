/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_APPLE_IOS_SYSTEM_H
#define META_OCEAN_PLATFORM_APPLE_IOS_SYSTEM_H

#include "ocean/platform/apple/ios/IOS.h"

namespace Ocean
{

namespace Platform
{

namespace Apple
{

namespace IOS
{

/**
 * This class implements system functions on Apple iOS platforms.
 * @ingroup platformappleios
 */
class System
{
	public:

		/**
		 * Returns the name of the current iPhone model.
		 * @param name The resulting name of the current device platform
		 * @return True, if succeeded
		 */
		static bool iphoneModelName(std::string& name);

		/**
		 * Returns the version of iOS running on the current device.
		 * @param version The resulting version of the operating system
		 * @return True, if succeeded
		 */
		static bool iosVersion(std::string& version);
};

}

}

}

}

#endif // META_OCEAN_PLATFORM_APPLE_IOS_SYSTEM_H
