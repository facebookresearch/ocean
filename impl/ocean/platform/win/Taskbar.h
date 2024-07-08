/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
