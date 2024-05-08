/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/apple/macos/Utilities.h"
#include "ocean/platform/apple/macos/FrameView.h"

#include "ocean/base/Singleton.h"
#include "ocean/base/StringApple.h"
#include "ocean/base/Thread.h"

#include <map>

/**
 * This class implements a window with close event.
 */
@interface OceanPlatformAppleMacOSWindow : NSWindow
{

}

@end

/**
 * This class implements a manager for frame windows.
 */
class Ocean::Platform::Apple::MacOS::Utilities::FrameWindowManager : public Ocean::Singleton<Ocean::Platform::Apple::MacOS::Utilities::FrameWindowManager>
{
	protected:

		/**
		 * Definition of a map mapping window objects to view objects.
		 */
		typedef std::map<OceanPlatformAppleMacOSWindow*, FrameView*> WindowMap;

	public:

		/**
		 * Adds a new combination of window and view to this manager.
		 * @param window The window to be added and managed, must be valid
		 * @param view The view beloning to the window and showing a frame, must be valid
		 */
		void addWindow(OceanPlatformAppleMacOSWindow* window, FrameView* view)
		{
			ocean_assert(window != nullptr && view != nullptr);

			const ScopedLock scopedLock(lock_);

			ocean_assert(windowMap_.find(window) == windowMap_.end());
			windowMap_.insert(std::make_pair(window, view));
		}

		/**
		 * Removes a window from this manager.
		 * This functino should be invoked whenever a window is closed.
		 * @param window The window to be removed, must be valid
		 */
		void removeWindow(OceanPlatformAppleMacOSWindow* window)
		{
			ocean_assert(window != nullptr);

			const ScopedLock scopedLock(lock_);

			const WindowMap::iterator i = windowMap_.find(window);
			ocean_assert(i != windowMap_.end());

			delete i->second;
			windowMap_.erase(i);
		}

		/**
		 * Counts the number of windows that are currently managed and visible.
		 * @return The number of visible windows
		 */
		unsigned int visibleWindows()
		{
			const ScopedLock scopedLock(lock_);

			unsigned int counter = 0u;

			for (WindowMap::const_iterator i = windowMap_.begin(); i != windowMap_.end(); ++i)
			{
				if ([i->first isVisible])
				{
					counter++;
				}
			}

			return counter;
		}

		/**
		 * Updates the frame of a specific window.
		 * @param title The tile of the window in which the frame will be updated
		 * @param frame The frame to update, must be valid
		 * @return True, if the frame could be updated
		 */
		bool updateFrameIfExisting(const std::string& title, const Frame& frame)
		{
			NSString* nsTitle = StringApple::toNSString(title);

			const ScopedLock scopedLock(lock_);

			for (WindowMap::iterator i = windowMap_.begin(); i != windowMap_.end(); ++i)
			{
				if ([[i->first title] isEqualToString:nsTitle])
				{
					i->second->setFrame(frame);
					return true;
				}
			}

			return false;
		}

	protected:

		/// The map mapping (currently visible) windows object to view objects.
		WindowMap windowMap_;

		/// The manager's lock.
		Lock lock_;
};

@implementation OceanPlatformAppleMacOSWindow

-(void)close
{
	[super close];

	Ocean::Platform::Apple::MacOS::Utilities::FrameWindowManager::get().removeWindow(self);
}

@end

namespace Ocean
{

namespace Platform
{

namespace Apple
{

namespace MacOS
{

Utilities::Commands Utilities::commandArguments()
{
	NSArray* commandArguments = [[NSProcessInfo processInfo] arguments];

	Commands result;
	result.reserve(commandArguments.count);

	// as the first argument is the filename (and path) of the executable we will skip this value

	for (unsigned int n = 1u; n < (unsigned int)commandArguments.count; ++n)
	{
		std::wstring argument = StringApple::toWString((NSString*)commandArguments[n]);

		// special handling for Xcode as we do not want any Xcode-default command arguments
		if (argument == L"-NSDocumentRevisionsDebugMode")
			break;

		result.push_back(std::move(argument));
	}

	return result;
}

void Utilities::imageTextOutput(NSImage* image, const int x, const int y, const std::string& text, const double fontSize)
{
	ocean_assert(image && fontSize > 0.0);

	NSString* nsText = StringApple::toNSString(text);

	// first we lock the image as current context

	[image lockFocus];

	// we need to flip the coordinate system to ensure that the text is printed top down

	NSAffineTransform* transformation = [NSAffineTransform transform];
	[transformation scaleXBy:1.0 yBy:1.0];
	[transformation concat];

	NSDictionary* attributes = @{NSFontAttributeName: [NSFont systemFontOfSize:fontSize], NSForegroundColorAttributeName: [NSColor blackColor], NSBackgroundColorAttributeName: [NSColor whiteColor]};
	[nsText drawAtPoint:NSMakePoint(x, [image size].height - y - fontSize) withAttributes:attributes];

	// we revert the applied transformation

	[transformation invert];
	[transformation concat];

	// we unlock the context

	[image unlockFocus];
}

void Utilities::imageTextOutput(Image& image, const int x, const int y, const std::string& text, const double fontSize)
{
	imageTextOutput(image.nsImage(), x, y, text, fontSize);
}

bool Utilities::windowFrameOutput(const double x, const double y, const Frame& frame, const std::string& title, const bool modalIfSuitable, const double width, const double height)
{
	ocean_assert(frame.isValid());

	if (title.empty() || modalIfSuitable || !FrameWindowManager::get().updateFrameIfExisting(title, frame))
	{
		const double windowWidth = width > 0 ? width : double(frame.width());
		const double windowHeight = height > 0 ? height : double(frame.height());

		OceanPlatformAppleMacOSWindow* window = [[OceanPlatformAppleMacOSWindow alloc] initWithContentRect:NSMakeRect(0, 0, windowWidth, windowHeight) styleMask:(NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask) backing:NSBackingStoreBuffered defer:NO];
		[window cascadeTopLeftFromPoint:NSMakePoint(x, y)];
		[window setTitle: StringApple::toNSString(title)];
		[window setReleasedWhenClosed:false];
		[window orderFrontRegardless];

		Platform::Apple::MacOS::FrameView* frameView = new Platform::Apple::MacOS::FrameView([window frame]);
		frameView->setFrame(frame);
		[window setContentView:frameView->nsView()];

		FrameWindowManager::get().addWindow(window, frameView);
	}

	if ([NSApp isRunning])
	{
		// we are a normal window application, so the application will be responsible for the event handling
		return true;
	}

	if (![NSThread isMainThread])
	{
		if (!modalIfSuitable)
		{
			return true;
		}

		ocean_assert(false && "This is a console application, so call this function from the main thread!");
		return false;
	}

	NSEvent* event = nullptr;
	do
	{
		event = [NSApp nextEventMatchingMask: NSAnyEventMask untilDate: nil inMode: NSDefaultRunLoopMode dequeue: YES];

		if (event)
		{
			[NSApp sendEvent: event];
		}
		else
		{
			if (!modalIfSuitable)
			{
				break;
			}

			Thread::sleep(1u);
		}
	}
	while (FrameWindowManager::get().visibleWindows() != 0u || event);

	return true;
}

void Utilities::waitForFrameOutput()
{
	NSEvent* event = nullptr;

	do
	{
		event = [NSApp nextEventMatchingMask: NSAnyEventMask untilDate: nil inMode: NSDefaultRunLoopMode dequeue: YES];

		if (event)
		{
			[NSApp sendEvent: event];
		}

		Thread::sleep(1u);
	}
	while (FrameWindowManager::get().visibleWindows() != 0u || event);
}

}

}

}

}
