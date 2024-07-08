/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/pointtracker/osx/AppDelegate.h"

#include "application/ocean/demo/tracking/pointtracker/PointTrackerWrapper.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomI.h"

#include "ocean/platform/apple/macos/FrameView.h"
#include "ocean/platform/apple/macos/Utilities.h"

@interface AppDelegate ()
{
	/// The platform independent wrapper for the point tracker.
	PointTrackerWrapper pointTrackerWrapper_;

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

	// first we create a new point tracker object, configurated by the command arguments

	pointTrackerWrapper_ = PointTrackerWrapper(Platform::Apple::MacOS::Utilities::commandArguments());

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

	if (pointTrackerWrapper_.trackNewFrame(resultingTrackerFrame, resultingTrackerPerformance) && resultingTrackerFrame.isValid())
	{
		Platform::Apple::MacOS::Image image(resultingTrackerFrame);

		Platform::Apple::MacOS::Utilities::imageTextOutput(image, 5, 5, String::toAString(resultingTrackerPerformance * 1000.0, 2u) + std::string("ms"));

		frameView_.setImage(std::move(image));
	}
}

/**
 * The release event function.
 */
- (void)applicationWillTerminate:(NSNotification*)aNotification
{
	pointTrackerWrapper_.release();
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
