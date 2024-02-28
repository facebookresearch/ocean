// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_WIN_TASKBAR_H
#define META_OCEAN_PLATFORM_WIN_TASKBAR_H

#include "ocean/platform/win/Win.h"

namespace Ocean
{

namespace Platform
{

namespace Win
{

/**
 * This class provides windows taskbar functionalities.
 * @ingroup platformwin
 */
class OCEAN_PLATFORM_WIN_EXPORT Taskbar
{
	public:

		/**
		 * Shows the task bar.
		 * @return True, if succeeded
		 */
		static bool show();

		/**
		 * Hides the task bar.
		 * @return True, if succeeded
		 */
		static bool hide();
};

}

}

}

#endif // META_OCEAN_PLATFORM_WIN_TASKBAR_H
