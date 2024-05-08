/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/homographyimagealigner/osx/AppDelegate.h"

#include "application/ocean/demo/tracking/homographyimagealigner/HomographyImageAligner.h"

#include "ocean/base/RandomI.h"

#include "ocean/platform/apple/macos/FrameView.h"
#include "ocean/platform/apple/macos/Utilities.h"

@interface AppDelegate ()
{
	/// The platform independent implementation of the homography image aligner.
	HomographyImageAligner applicationHomographyImageAligner;

	/// The view displaying the current frame.
	Platform::Apple::MacOS::FrameView applicationFrameView;
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

	applicationHomographyImageAligner = HomographyImageAligner(Platform::Apple::MacOS::Utilities::commandArguments());

	// we create the view that can display frame medium object on its own

	applicationFrameView = Platform::Apple::MacOS::FrameView([_mainWindowView frame]);
	applicationFrameView.nsView().autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

	[_mainWindowView addSubview:applicationFrameView.nsView()];

	// we create a timer that invokes our actual image aligner every 1ms

	[NSTimer scheduledTimerWithTimeInterval:0.001 target:self selector:@selector(timerTicked:) userInfo:nil repeats:YES];
}

/**
 * The timer event function.
 */
- (void)timerTicked:(NSTimer*)timer
{
	Frame resultingAlignerFrame;
	double resultingAlignerPerformance;

	SquareMatrix3 currentHomographyPrevious;
	Vectors2 previousPoints, currentPoints;

	bool reachedLastFrame = false;

	if (applicationHomographyImageAligner.alignNewFrame(resultingAlignerFrame, resultingAlignerPerformance, &currentHomographyPrevious, &previousPoints, &currentPoints, nullptr, &reachedLastFrame) && resultingAlignerFrame.isValid())
	{
#if 0
		// we verify that the resulting homography matches with the entire set of resulting point correspondences

		ocean_assert(previousPoints.size() == currentPoints.size());

		for (size_t n = 0; n < previousPoints.size(); ++n)
		{
			const Vector2 transformedPreviousPoint(currentHomographyPrevious * previousPoints[n]);
			ocean_assert(currentPoints[n].isEqual(transformedPreviousPoint, 5)); // we are quite generous be using 5 pixels as threshold
		}
#endif

		Platform::Apple::MacOS::Image image(resultingAlignerFrame);

		if (resultingAlignerPerformance >= 0.0)
		{
			Platform::Apple::MacOS::Utilities::imageTextOutput(image, 5, 5, String::toAString(resultingAlignerPerformance * 1000.0, 2u) + std::string("ms"));
		}
		else
		{
			Platform::Apple::MacOS::Utilities::imageTextOutput(image, 5, 5, "Place the tracking pattern in front of the camera");
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
	applicationHomographyImageAligner.release();
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
