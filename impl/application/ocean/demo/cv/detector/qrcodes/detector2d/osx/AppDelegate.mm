/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "AppDelegate.h"

#include "application/ocean/demo/cv/detector/qrcodes/detector2d/Detector2DWrapper.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomI.h"

#include "ocean/platform/apple/Resource.h"
#include "ocean/platform/apple/macos/FrameView.h"
#include "ocean/platform/apple/macos/Utilities.h"

@interface AppDelegate ()
{
	/// The platform independent implementation of the actual functionality.
	Detector2DWrapper detectorWrapper;

	/// The view displaying the current frame.
	Platform::Apple::MacOS::FrameView frameView;
}

/// The window object.
@property(weak) IBOutlet NSWindow *mainWindow;

/// The view object.
@property(weak) IBOutlet NSView *mainWindowView;

@end

@implementation AppDelegate

/**
 * The event function when the application has been launched.
 */
- (void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
	Platform::Apple::MacOS::Utilities::Commands commandLines = Platform::Apple::MacOS::Utilities::commandArguments();

	// 0. Video source
	if (commandLines.empty())
	{
		commandLines.push_back(L"LiveVideoId:1");
	}

	// 1. Video resolution
	if (commandLines.size() < 2)
	{
		commandLines.push_back(L"1280x720");
	}

	// Set where the log messages should be directed
	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	// First we create a new instance and pass the command-line arguments
	detectorWrapper = Detector2DWrapper(commandLines);

	// We create the view that can display frame medium object on its own
	frameView = Platform::Apple::MacOS::FrameView([_mainWindowView frame]);
	frameView.nsView().autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

	[_mainWindowView addSubview:frameView.nsView()];

	// We create a timer that invokes the actual functionality every 1ms
	[NSTimer scheduledTimerWithTimeInterval:0.001 target:self selector:@selector(timerTicked:) userInfo:nil repeats:YES];
}

/**
 * The timer event function.
 */
- (void)timerTicked:(NSTimer*)timer
{
	double qrCodePerformance;

	std::vector<std::string> decodedMessages;
	Vectors2 previousPoints, currentPoints;

	bool reachedLastFrame = false;

	Frame resultingFrame;
	detectorWrapper.detectAndDecode(resultingFrame, qrCodePerformance, decodedMessages, &reachedLastFrame);

	if (resultingFrame.isValid())
	{
		Platform::Apple::MacOS::Image image(resultingFrame);

		const std::string qrCodePerformanceString = (qrCodePerformance >= 0.0 ? String::toAString(qrCodePerformance * 1000.0, 2u) : "---") + std::string("ms");
		Platform::Apple::MacOS::Utilities::imageTextOutput(image, 5, 5, qrCodePerformanceString);

		for (size_t i = 0; i < decodedMessages.size(); ++i)
		{
			Platform::Apple::MacOS::Utilities::imageTextOutput(image, 5, 35 + (int(i) * 30), String::toAString(i + 1) + ": " + decodedMessages[i]);
		}

		frameView.setImage(std::move(image));
	}
	else
	{
		if (reachedLastFrame)
		{
			[_mainWindow setTitle:@"Last frame reached..."];
		}
	}
}

/**
 * The release event function.
 */
- (void)applicationWillTerminate:(NSNotification*)aNotification
{
	detectorWrapper.release();
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
