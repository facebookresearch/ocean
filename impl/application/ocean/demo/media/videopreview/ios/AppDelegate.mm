/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "AppDelegate.h"

#include "ocean/base/Messenger.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/StringApple.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"

#include "ocean/platform/apple/ios/OpenGLFrameMediumViewController.h"

#ifdef OCEAN_RUNTIME_STATIC
	#include "ocean/media/avfoundation/AVFoundation.h"

	#include "ocean/rendering/glescenegraph/apple/Apple.h"
#endif

@interface AppDelegate ()

@end

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	// we forward all messages to the debug window (of e.g., Xcode)
	Messenger::get().setOutputType(Messenger::OUTPUT_DEBUG_WINDOW);

#ifdef OCEAN_RUNTIME_STATIC

	// for the static runtime we explicitly register the individual plugins as desired

	Media::AVFoundation::registerAVFLibrary();
	Rendering::GLESceneGraph::Apple::registerGLESceneGraphEngine();

#else

	// for the shared runtime the plugin manager will load all available plugins in the correct order

	// we collect all plugins located in the resource path of the application
	NSBundle* bundleObject = [NSBundle mainBundle];
	PluginManager::get().collectPlugins(StringApple::toUTF8([bundleObject resourcePath]));

	// we load any plugin that has been collected
	PluginManager::get().loadPlugins(PluginManager::TYPE_ANY);

#endif


	// as the Storyboard does not define the layout of this application,
	// we simply need to set up the view controller programmatically

	self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	self.window.backgroundColor = [UIColor whiteColor];

	OpenGLFrameMediumViewController* viewController = [[OpenGLFrameMediumViewController alloc] init];

	self.window.rootViewController = viewController;
	[self.window makeKeyAndVisible];


	// we create a live video medium for our background (and e.g., for the tracker aquired by "BACKGROUND" in the TrackerTransform node)
	// as the name of the camera device may change we take the first one

	const Media::FrameMediumRef liveVideo = Media::Manager::get().newMedium("LiveVideoId:0", Media::Medium::LIVE_VIDEO);

	if (liveVideo.isNull())
	{
		ocean_assert(false && "This should never happen!");
		return YES;
	}

	// we define a preferred frame dimension (however, the camera device may provide a different dimension if this dimension is not suitable/available)
	liveVideo->setPreferredFrameDimension(1280u, 720u);

	// we start the camera device
	liveVideo->start();

	[viewController setFrameMedium:liveVideo];

	return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
	// Sent when the application is about to move from active to inactive state.
	// This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
	// Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
	// Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
	// If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
	// Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
	// Restart any tasks that were paused (or not yet started) while the application was inactive.
	// If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	// Called when the application is about to terminate.
	// Save data if appropriate.
	// See also applicationDidEnterBackground:.
}

@end
