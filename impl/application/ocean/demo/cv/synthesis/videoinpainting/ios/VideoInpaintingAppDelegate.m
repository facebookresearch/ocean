// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/cv/synthesis/videoinpainting/ios/VideoInpaintingAppDelegate.h"
#include "application/ocean/demo/cv/synthesis/videoinpainting/ios/VideoInpaintingViewController.h"

#include "ocean/base/Messenger.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/StringApple.h"

#ifdef OCEAN_RUNTIME_STATIC
	#include "ocean/media/adapter/Adapter.h"
	#include "ocean/media/avfoundation/AVFoundation.h"

	#include "ocean/rendering/glescenegraph/apple/Apple.h"
#endif

@interface VideoInpaintingAppDelegate ()
@end

@implementation VideoInpaintingAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	// we forward all messages to the debug window (of e.g., Xcode)
	Ocean::Messenger::get().setOutputType(Ocean::Messenger::OUTPUT_DEBUG_WINDOW);

#ifdef OCEAN_RUNTIME_STATIC

	// for the static runtime we explicitly register the individual plugins as desired

	// we load the AVFoundation media plugins
	Ocean::Media::Adapter::registerAdapterLibrary();
	Ocean::Media::AVFoundation::registerAVFLibrary();

	// we load the GLESceneGraph rendering plugin
	Ocean::Rendering::GLESceneGraph::Apple::registerGLESceneGraphEngine();
	// TODO: Ocean:Rendering::MetalSceneGraph::registerMetalSceneGraphEngine();

#else

	// for the shared runtime the plugin manager will load all available plugins in the correct order

	// we collect all plugins located in the resource path of the application
	NSBundle* bundleObject = [NSBundle mainBundle];
	Ocean::PluginManager::get().collectPlugins(Ocean::StringApple::toUTF8([bundleObject resourcePath]));

	// we load any plugin that has been collected
	Ocean::PluginManager::get().loadPlugins(Ocean::PluginManager::TYPE_ANY);

#endif

	return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
	// Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
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
	VideoInpainting::get().reset();
	// Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	// Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
