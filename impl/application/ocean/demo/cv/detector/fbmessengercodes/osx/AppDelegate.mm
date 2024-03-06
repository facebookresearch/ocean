// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/cv/detector/fbmessengercodes/osx/AppDelegate.h"

#include "application/ocean/demo/cv/detector/fbmessengercodes/FBMessengerCodesWrapper.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomI.h"

#include "ocean/platform/apple/Resource.h"
#include "ocean/platform/apple/macos/FrameView.h"
#include "ocean/platform/apple/macos/Utilities.h"

@interface AppDelegate ()
{
	/// The platform independent implementation of the actual functionality.
	FBMessengerCodesWrapper applicationFBMessengerCodes;

	/// The view displaying the current frame.
	Platform::Apple::MacOS::FrameView applicationFrameView;
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
		commandLines.push_back(L"LiveVideoId:0");
	}

	// 1. Video resolution
	if (commandLines.size() < 2)
	{
		commandLines.push_back(L"1280x720");
	}

	// 2. Pixel format to be used
	if (commandLines.size() < 3)
	{
		commandLines.push_back(L"Y8");
	}

	// Set where the log messages should be directed
	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	// First we create a new instance and pass the command-line arguments
	applicationFBMessengerCodes = FBMessengerCodesWrapper(commandLines);

	// We create the view that can display frame medium object on its own
	applicationFrameView = Platform::Apple::MacOS::FrameView([_mainWindowView frame]);
	applicationFrameView.nsView().autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

	[_mainWindowView addSubview:applicationFrameView.nsView()];

	// We create a timer that invokes the actual functionality every 1ms
	[NSTimer scheduledTimerWithTimeInterval:0.001 target:self selector:@selector(timerTicked:) userInfo:nil repeats:YES];
}

/**
 * The timer event function.
 */
- (void)timerTicked:(NSTimer*)timer
{
	double fbMessengerCodesPerformance;

	std::vector<std::string> decodedMessages;
	Vectors2 previousPoints, currentPoints;

	bool reachedLastFrame = false;

	Frame resultingFrame;
	applicationFBMessengerCodes.detectAndDecode(resultingFrame, fbMessengerCodesPerformance, decodedMessages, &reachedLastFrame);

	if (resultingFrame.isValid())
	{
		Platform::Apple::MacOS::Image image(resultingFrame);

		const std::string fbMessengerCodesPerformanceString = (fbMessengerCodesPerformance >= 0.0 ? String::toAString(fbMessengerCodesPerformance * 1000.0, 2u) : "---") + std::string("ms");
		Platform::Apple::MacOS::Utilities::imageTextOutput(image, 5, 5, fbMessengerCodesPerformanceString);

		for (size_t i = 0; i < decodedMessages.size(); ++i)
		{
			Platform::Apple::MacOS::Utilities::imageTextOutput(image, 5, 35 + (i * 30), String::toAString(i + 1) + ": " + decodedMessages[i]);
		}

		applicationFrameView.setImage(std::move(image));
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
	applicationFBMessengerCodes.release();
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
