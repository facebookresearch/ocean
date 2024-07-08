/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_CONSOLE_WINDOW_H
#define FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_CONSOLE_WINDOW_H

#include "application/ocean/demo/misc/appinspector/osx/AppDelegate.h"

#include <AppKit/AppKit.h>

/**
 * Definition of a window mainly showing a console output.
 * ingroup applicationdemomiscappinspectorosx
 */
@interface ConsoleWindow : NSWindow<NSWindowDelegate>

@end

#endif // FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_CONSOLE_WINDOW_H
