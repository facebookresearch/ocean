/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/apple/macos/Screen.h"

namespace Ocean
{

namespace Platform
{

namespace Apple
{

namespace MacOS
{

double Screen::scaleFactor(NSScreen* screen)
{
	double factor = 1.0;

	if (screen)
		factor = [screen backingScaleFactor];
	else
	{
		NSScreen* mainScreen = [NSScreen mainScreen];
		ocean_assert(mainScreen);

		factor = [mainScreen backingScaleFactor];
	}

	ocean_assert(factor > 0.0);
	return factor;
}

double Screen::scaleFactor(NSWindow* window)
{
	double factor = 1.0;

	if (window)
		factor = [window backingScaleFactor];
	else
	{
		NSScreen* mainScreen = [NSScreen mainScreen];
		ocean_assert(mainScreen);

		factor = [mainScreen backingScaleFactor];
	}

	ocean_assert(factor > 0.0);
	return factor;
}

}

}

}

}
