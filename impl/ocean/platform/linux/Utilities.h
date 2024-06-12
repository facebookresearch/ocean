/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_LINUX_UTILITIES_H
#define META_OCEAN_PLATFORM_LINUX_UTILITIES_H

#include "ocean/platform/linux/Linux.h"

#include "ocean/base/ScopedObject.h"

namespace Ocean
{

namespace Platform
{

namespace Linux
{

/**
 * This class implements utility functions for Linux platforms.
 * @ingroup platformlinux
 */
class Utilities
{
	protected:

		/**
		 * Definition of a scoped object for FILE pointers.
		 */
		using ScopedFILE = ScopedObjectCompileTimeT<FILE*, FILE*, int, pclose, 0, false /*tCheckReturnValue*/>;

	public:

		/**
		 * Checks the security enhanced linux state (SELinux).
		 * Can be toggled with "setenforce 0" or "setenforce 1".
		 * @return True, if the SELinux state is permissive; False, if the SELinux state is enforcing
		 * @return True, if the state could be determined
		 */
		static bool checkSecurityEnhancedLinuxStateIsPermissive();
};

}

}

}

#endif // META_OCEAN_PLATFORM_LINUX_UTILITIES_H
