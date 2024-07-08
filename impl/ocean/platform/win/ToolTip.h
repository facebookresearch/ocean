/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WIN_TOOL_TIP_H
#define META_OCEAN_PLATFORM_WIN_TOOL_TIP_H

#include "ocean/platform/win/Win.h"

namespace Ocean
{

namespace Platform
{

namespace Win
{

/**
 * This class implements tool tip functionalities.
 */
class OCEAN_PLATFORM_WIN_EXPORT ToolTip
{
	public:

		/**
		 * Adds a new tool tip to a window.
		 * @param handle The handle of the window to which the tool tip will be added, must be valid
		 * @param text The text of the tool tip
		 * @return True, if succeeded
		 */
		static bool addToolTip(const HWND handle, const std::wstring& text);
};

}

}

}

#endif // META_OCEAN_PLATFORM_WIN_TOOL_TIP_H
