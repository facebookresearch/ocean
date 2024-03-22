// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/tracking/uvtexturemapping/osx/AppDelegate.h"

#include "application/ocean/demo/tracking/uvtexturemapping/UVTextureMappingWrapper.h"

#include "ocean/base/RandomI.h"

#include "ocean/platform/apple/macos/FrameView.h"
#include "ocean/platform/apple/macos/Utilities.h"

@interface AppDelegate ()
{
	/// The platform independent wrapper for the UV texture mapping.
	UVTextureMappingWrapper uvTextureMappingWrapper_;

	/// The view displaying the current frame.
	Platform::Apple::MacOS::FrameView frameView_;
}

/// The window object.
@property (weak) IBOutlet NSWindow* mainWindow;

/// The view object.
@property (weak) IBOutlet NSView* mainWindowView;

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
	RandomI::initialize();

	// first we create a new UV texture mapping object, configurated by the command arguments

	uvTextureMappingWrapper_ = UVTextureMappingWrapper(Platform::Apple::MacOS::Utilities::commandArguments());

	// we create the view that can display frame medium object on its own

	frameView_ = Platform::Apple::MacOS::FrameView([_mainWindowView frame]);
	frameView_.nsView().autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

	[_mainWindowView addSubview:frameView_.nsView()];

	// we create a timer that invokes our tracker every 10ms

	[NSTimer scheduledTimerWithTimeInterval:0.01 target:self selector:@selector(timerTicked:) userInfo:nil repeats:YES];
}

- (void)timerTicked:(NSTimer*)timer
{
	Frame resultingTrackerFrame;
	double resultingTrackerPerformance;

	if (uvTextureMappingWrapper_.trackNewFrame(resultingTrackerFrame, resultingTrackerPerformance))
	{
		Platform::Apple::MacOS::Image image(resultingTrackerFrame);

		if (resultingTrackerPerformance >= 0.0)
		{
			Platform::Apple::MacOS::Utilities::imageTextOutput(image, 5, 5, String::toAString(resultingTrackerPerformance * 1000.0, 2u) + std::string("ms"));
		}
		else
		{
			Platform::Apple::MacOS::Utilities::imageTextOutput(image, 5, 5, "Place the tracking pattern in front of the camera");
		}

		frameView_.setImage(std::move(image));
	}
}

/**
 * The release event function.
 */
- (void)applicationWillTerminate:(NSNotification*)aNotification
{
	uvTextureMappingWrapper_.release();
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
