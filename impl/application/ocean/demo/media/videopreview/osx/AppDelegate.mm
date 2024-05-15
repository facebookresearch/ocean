/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/media/videopreview/osx/AppDelegate.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/RandomI.h"

#include "ocean/media/Manager.h"

#include "ocean/media/avfoundation/AVFoundation.h"

#include "ocean/platform/apple/System.h"

#include "ocean/platform/apple/macos/FrameMediumView.h"

@interface AppDelegate ()
{
	/// The view displaying the medium
	Platform::Apple::MacOS::FrameMediumView frameMediumView_;
}

/// The window object.
@property (weak) IBOutlet NSWindow* mainWindow;

/// The view object.
@property (weak) IBOutlet NSView* mainWindowView;

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
	// first, we register or load the media plugin(s)
	// if we have a shared runtime we simply load all media plugins available in a specific directory
	// if we have a static runtime we explicitly need to register all plugins we want to use (at compile time)

#ifdef OCEAN_RUNTIME_SHARED
	const std::string frameworkPath(Platform::Apple::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	PluginManager::get().loadPlugins(PluginManager::TYPE_MEDIA);
#else
	Media::AVFoundation::registerAVFLibrary();
#endif

	// first we try to get a live video with id 1 (often an external web cam - not the builtin camera of a laptop)

	Media::FrameMediumRef medium = Media::Manager::get().newMedium("LiveVideoId:1");

	if (medium.isNull())
	{
		// if we could not get the device with id 1 we try to get the device with id 0
		medium = Media::Manager::get().newMedium("LiveVideoId:0");
	}

	if (medium.isNull())
	{
		// the device does not have an accessible live camera (or a necessary media plugin hasn't loaded successfully)

		return;
	}

	// we create the view that can display frame medium object on its own

	frameMediumView_ = Platform::Apple::MacOS::FrameMediumView([_mainWindowView frame]);
	frameMediumView_.nsView().autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

	// we start the medium so that medium will deliver frames

	medium->start();

	// and we tell the view which medium to display

	frameMediumView_.setFrameMedium(medium);

	[_mainWindowView addSubview:frameMediumView_.nsView()];
}

/**
 * The release event function.
 */
- (void)applicationWillTerminate:(NSNotification*)aNotification
{
	frameMediumView_ = Platform::Apple::MacOS::FrameMediumView();

	// finally we release the media plugins

#ifdef OCEAN_RUNTIME_SHARED
	PluginManager::get().release();
#else
	Media::AVFoundation::unregisterAVFLibrary();
#endif
}

/**
 * Checks whether the application should terminate when the last windows is closed.
 * @return True, if so
 */
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
	return YES;
}

@end
