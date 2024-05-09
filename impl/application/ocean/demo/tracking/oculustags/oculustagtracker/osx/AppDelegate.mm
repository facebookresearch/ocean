/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/oculustags/oculustagtracker/osx/AppDelegate.h"

#include "application/ocean/demo/tracking/oculustags/oculustagtracker/OculusTagTrackerWrapper.h"

#include "ocean/base/RandomI.h"

#include "ocean/platform/apple/macos/FrameView.h"
#include "ocean/platform/apple/macos/Utilities.h"

@interface AppDelegate ()
{
	/// The platform independent implementation of the Oculus Tag tracker.
	OculusTagTrackerWrapper oculusTagTrackerWrapper_;

	/// The view displaying the current frame.
	Platform::Apple::MacOS::FrameView frameView_;
}

/// The window object.
@property (weak) IBOutlet NSWindow* mainWindow;

/// The view object.
@property (weak) IBOutlet NSView* mainWindowView;

@end

@implementation AppDelegate

/**
 * The event function when the application has been launched.
 */
- (void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
	// first we create a new feature tracker object, configurated by the command arguments

	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	oculusTagTrackerWrapper_ = OculusTagTrackerWrapper(Platform::Apple::MacOS::Utilities::commandArguments());

	// we create the view that can display frame medium object on its own

	frameView_ = Platform::Apple::MacOS::FrameView([_mainWindowView frame]);
	frameView_.nsView().autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

	[_mainWindowView addSubview:frameView_.nsView()];

	// we create a timer that invokes our actual image aligner every 1ms

	[NSTimer scheduledTimerWithTimeInterval:0.001 target:self selector:@selector(timerTicked:) userInfo:nil repeats:YES];
}

/**
 * The timer event function.
 */
- (void)timerTicked:(NSTimer*)timer
{
	Frame resultingFrame;

	double resultingPerformance;

	if (oculusTagTrackerWrapper_.trackNewFrame(resultingFrame, resultingPerformance) && resultingFrame.isValid())
	{
		Platform::Apple::MacOS::Image image(resultingFrame);

		if (resultingPerformance >= 0.0)
		{
			Platform::Apple::MacOS::Utilities::imageTextOutput(image, 5, 5, String::toAString(resultingPerformance * 1000.0, 2u) + std::string("ms"));
		}
		else
		{
			Platform::Apple::MacOS::Utilities::imageTextOutput(image, 5, 5, "Select a plane to track.");
		}

		frameView_.setImage(std::move(image));
	}
}

/**
 * The release event function.
 */
- (void)applicationWillTerminate:(NSNotification*)aNotification
{
	oculusTagTrackerWrapper_.release();
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
