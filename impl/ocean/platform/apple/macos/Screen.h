/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_APPLE_MACOS_SCREEN_H
#define META_OCEAN_PLATFORM_APPLE_MACOS_SCREEN_H

#include "ocean/platform/apple/macos/MacOS.h"

#ifndef __OBJC__
	#error Platform::Apple::MacOS::Screen.h needs to be included from an ObjectiveC++ file
#endif

#include <AppKit/AppKit.h>

namespace Ocean
{

namespace Platform
{

namespace Apple
{

namespace MacOS
{

/**
 * This class implements system functions on MacOS platforms.
 * @ingroup platformapplemacos
 */
class Screen
{
	public:

		/**
		 * Returns the scaling factor transforming the number of (backing) pixels defined in the (virtual) display coordinate system to the number of pixels defined in the (native/physical) screen coordinate system for a specified screen.
		 * @param screen The screen for which the scaling factor is determined, nullptr to determine the scale factor of the default screen
		 * @return The resulting scale factor with range (0, infinity)
		 */
		static double scaleFactor(NSScreen* screen);

		/**
		 * Returns the scaling factor transforming the number of (backing) pixels defined in the (virtual) display coordinate system to the number of pixels defined in the native/physical screen coordinate system for a specified window.
		 * @param window The window for which the scaling factor is determined, nullptr to determine the scale factor of the default screen
		 * @return The resulting scale factor with range (0, infinity)
		 */
		static double scaleFactor(NSWindow* window);
};

}

}

}

}

#endif // META_OCEAN_PLATFORM_APPLE_MACOS_SCREEN_H
