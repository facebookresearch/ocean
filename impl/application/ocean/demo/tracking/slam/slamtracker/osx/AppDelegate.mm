/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "AppDelegate.h"

#include "application/ocean/demo/tracking/slam/slamtracker/SLAMTrackerWrapper.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomI.h"

#include "ocean/platform/apple/macos/FrameView.h"
#include "ocean/platform/apple/macos/Utilities.h"

/**
 * The view adding support for user interaction.
 */
class CustomView : public Platform::Apple::MacOS::FrameView
{
	public:

		/**
		 * Default constructor.
		 */
		inline CustomView();

		/**
		 * Creates a new view object with specified size and dimension.
		 * @param rect The rect defining the size and dimension
		 */
		inline explicit CustomView(const NSRect& rect);

		/**
		 * Move constructor.
		 * @param view View object to move
		 */
		inline CustomView(CustomView&& view);

		/**
		 * The function for left mouse button up events.
		 * @see FlippedView::onMouseUpLeft().
		 */
		inline void onMouseUpLeft(const Vector2& mouseLocation) override;

		/**
		 * Returns the recent mouse position and resets it afterwards.
		 * @return The recent mouse position, otherwise a negative mouse position
		 */
		inline Vector2 recentMousePositionAndReset();

		/**
		 * Move operator.
		 * @param view View object to be moved
		 * @return Reference to this object
		 */
		CustomView& operator=(CustomView&& view);

	protected:

		/// The recent location of the mouse up event.
		Vector2 recentMousePosition_;
};

inline CustomView::CustomView() :
	FrameView(),
	recentMousePosition_(Scalar(-1), Scalar(-1))
{
	// nothing to do here
}

CustomView::CustomView(const NSRect& rect) :
	FrameView(rect),
	recentMousePosition_(Scalar(-1), Scalar(-1))
{
	// nothing to do here
}

inline CustomView::CustomView(CustomView&& view)
{
	*this = std::move(view);
}

inline void CustomView::onMouseUpLeft(const Vector2& mouseLocation)
{
	recentMousePosition_ = view2image(mouseLocation);
}

inline Vector2 CustomView::recentMousePositionAndReset()
{
	const Vector2 tmpLocation(recentMousePosition_);

	recentMousePosition_ = Vector2(Scalar(-1), Scalar(-1));

	return tmpLocation;
}

CustomView& CustomView::operator=(CustomView&& view)
{
	if (this != &view)
	{
		recentMousePosition_ = view.recentMousePosition_;

		FrameView::operator=(std::move(view));
	}

	return *this;
}

@interface AppDelegate ()
{
	/// The platform independent implementation of the homography image aligner.
	SLAMTrackerWrapper applicationSLAMTrackerWrapper;

	/// The view displaying the current frame.
	CustomView applicationCustomView;
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

	applicationSLAMTrackerWrapper = SLAMTrackerWrapper(Platform::Apple::MacOS::Utilities::commandArguments());

	// we create the view that can display frame medium object on its own

	applicationCustomView = CustomView([_mainWindowView frame]);
	applicationCustomView.nsView().autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

	[_mainWindowView addSubview:applicationCustomView.nsView()];

	// we create a timer that invokes our actual image aligner every 1ms

	[NSTimer scheduledTimerWithTimeInterval:0.001 target:self selector:@selector(timerTicked:) userInfo:nil repeats:YES];
}

/**
 * The timer event function.
 */
- (void)timerTicked:(NSTimer*)timer
{
	Frame resultingFrame;
	if (applicationSLAMTrackerWrapper.trackNewFrame(resultingFrame) && resultingFrame.isValid())
	{
		Platform::Apple::MacOS::Image image(resultingFrame);

		applicationCustomView.setImage(std::move(image));
	}
}

/**
 * The release event function.
 */
- (void)applicationWillTerminate:(NSNotification*)aNotification
{
	applicationSLAMTrackerWrapper.release();
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
