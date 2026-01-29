/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#import "SensorsAppDelegate.h"

#include "ocean/base/Messenger.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/StringApple.h"

#include "ocean/devices/ios/IOS.h"

#include "ocean/rendering/glescenegraph/apple/Apple.h"

using namespace Ocean;

@interface SensorsAppDelegate ()
@end

@implementation SensorsAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	Messenger::get().setOutputType(Messenger::OUTPUT_DEBUG_WINDOW);

	Devices::IOS::registerIOSLibrary();
	Rendering::GLESceneGraph::Apple::registerGLESceneGraphEngine();

	return YES;
}

@end
