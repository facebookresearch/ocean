/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/detector/linedetector/osx/AppDelegate.h"

#include "application/ocean/demo/cv/detector/linedetector/LineDetectorWrapper.h"

#include "ocean/base/RandomI.h"

#include "ocean/platform/apple/macos/FrameView.h"
#include "ocean/platform/apple/macos/Utilities.h"

@interface AppDelegate ()
{
	/// The platform independent wrapper for the line detector.
	LineDetectorWrapper applicationLineDetectorWrapper;

	/// The view displaying the current frame.
	Platform::Apple::MacOS::FrameView applicationFrameView;
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

	// first we create a new wrapper object, configurated by the command arguments

	applicationLineDetectorWrapper = LineDetectorWrapper(Platform::Apple::MacOS::Utilities::commandArguments());

	// we create the view that can display frame medium object on its own

	applicationFrameView = Platform::Apple::MacOS::FrameView([_mainWindowView frame]);
	applicationFrameView.nsView().autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

	[_mainWindowView addSubview:applicationFrameView.nsView()];

	// we create a timer that invokes our detector every 10ms

	[NSTimer scheduledTimerWithTimeInterval:0.01 target:self selector:@selector(timerTicked:) userInfo:nil repeats:YES];
}

- (void)timerTicked:(NSTimer*)timer
{
	Frame resultingDetectorFrame;
	double resultingDetectorPerformance;

	if (applicationLineDetectorWrapper.detectNewFrame(resultingDetectorFrame, resultingDetectorPerformance))
	{
		Platform::Apple::MacOS::Image image(resultingDetectorFrame);

		Platform::Apple::MacOS::Utilities::imageTextOutput(image, 5, 5, String::toAString(resultingDetectorPerformance * 1000.0, 2u) + std::string("ms"));

		applicationFrameView.setImage(std::move(image));
	}
}

/**
 * The release event function.
 */
- (void)applicationWillTerminate:(NSNotification*)aNotification
{
	applicationLineDetectorWrapper.release();
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
