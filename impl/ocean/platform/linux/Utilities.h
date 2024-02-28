// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
		typedef ScopedObjectCompileTime<FILE*, FILE*, int, pclose, 0, false /*tCheckReturnValue*/> ScopedFILE;

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
