/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#import "RendererAppDelegate.h"

#include "ocean/base/Messenger.h"

#include "ocean/rendering/glescenegraph/apple/Apple.h"

using namespace Ocean;

@interface RendererAppDelegate ()
@end

@implementation RendererAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	Messenger::get().setOutputType(Messenger::OUTPUT_DEBUG_WINDOW);

	Rendering::GLESceneGraph::Apple::registerGLESceneGraphEngine();

	return YES;
}

@end
