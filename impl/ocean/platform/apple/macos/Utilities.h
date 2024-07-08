/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_APPLE_MACOS_UTILITIES_H
#define META_OCEAN_PLATFORM_APPLE_MACOS_UTILITIES_H

#include "ocean/platform/apple/macos/MacOS.h"

#include "ocean/base/Frame.h"

#include <CoreFoundation/CoreFoundation.h>

#ifdef __OBJC__
	#include "ocean/platform/apple/macos/Image.h"

	#include <AppKit/AppKit.h>
#endif

namespace Ocean
{

namespace Platform
{

namespace Apple
{

namespace MacOS
{

/**
 * This class implements utilitiy functions for MacOS platforms.
 * @ingroup platformapplemacos
 */
class Utilities
{
	public:

		/**
		 * Definition of a vector holding application commands.
		 */
		typedef std::vector<std::wstring> Commands;

		/**
		 * Forward declaration of a manager for frame windows.
		 */
		class FrameWindowManager;

	public:

		/**
		 * Returns the command arguments that have been passed during the start of the application.
		 * The first argument is really the first argument that has been passed by the user/caller.<br>
		 * Therefore, the first argument is not the filename (and path) of the executable.
		 * @return The individual commands
		 */
		static Commands commandArguments();

#ifdef __OBJC__

		/**
		 * Prints a text into a given image.
		 * @param image The image in which the text will be printed, must be valid
		 * @param x Horizontal location of the text within the image, with range (-infinity, infinity)
		 * @param y Vertical location of the text within the image, with range (-infinity, infinity)
		 * @param text The text to be printed
		 * @param fontSize The size of the font in points, with range (0, infinity)
		 */
		static void imageTextOutput(NSImage* image, const int x, const int y, const std::string& text, const double fontSize = 14.0);

		/**
		 * Prints a text into a given image.
		 * @param image The image in which the text will be printed, must be valid
		 * @param x Horizontal location of the text within the image, with range (-infinity, infinity)
		 * @param y Vertical location of the text within the image, with range (-infinity, infinity)
		 * @param text The text to be printed
		 * @param fontSize The size of the font in points, with range (0, infinity)
		 */
		static void imageTextOutput(Image& image, const int x, const int y, const std::string& text, const double fontSize = 14.0);

#endif // __OBJC__

		/**
		 * Creates a window and paints a frame into the window.
		 * If the application is a window application, the new window will be created non-modal.<br>
		 * Instead, if the application is a console application, the new window will be created in a modal manner by default.<br>
		 * For console applications, this function must be called out of the main thread.<br>
		 * However, even in a console application the window can be created in a non modal manner - see 'modalIfSuitable',<br>
		 * e.g., to create/show two or more individual frames concurrently - so that they can be compared:
		 * @code
		 *
		 * // function creating two individual windows, the first non-modal, the second modal
		 * void functionInConsoleApplication()
		 * {
		 *    /// we visualize the first frame in a non-modal manner
		 *    Utilities::windowFrameOutput(10.0, 10.0, firstFrame, "First frame", false);
		 *
		 *    /// we can do something here ...
		 *
		 *    /// now we visualize the second frame in a modal manner,
		 *    /// so that we can interact with both frames
		 *    Utilities::windowFrameOutput(500.0, 10.0, secondFrame, "Second frame");
		 *
		 *    /// we proceed when both windows have been closed by the user ...
		 * }
		 *
		 * // function creating one windows and constantly updating the frame
		 * void functionInConsoleApplication()
		 * {
		 *    while (untilStop)
		 *    {
		 *       // a frame with updated image content
		 *       Frame updatedFrame = ...
		 *
		 *       // we update the frame in the single window
		 *       Utilities::windowFrameOUtput(10.0, 10.0, updatedFrame, "Unique name", false);
		 *    }
		 *
		 *    // we wait here until the user has close the windows (e.g., to end the application)
		 *    Utilities::waitForFrameOutput();
		 * }
		 * @endcode
		 * @param x Horizontal position of the window, in screen coordinates
		 * @param y Vertical position of the window, in screen coordinates
		 * @param frame The frame to be painted, must be valid
		 * @param title Optional title of the window displaying the frame, must be defined if a non-modal windows is supposed to be updated with new frames
		 * @param modalIfSuitable True, to create a modal window whenever suitable/necessary; False, to create a non-modal in any case while accepting that the window may not be maintainable as long as another windows in modal manner is created
		 * @param width Optional explicit width of the window painting the frame, in screen coordinates, 0 to use the width of the frame, with range [0, infinity)
		 * @param height Optional explicit height of the windows painting the frame, in screen coordinates, 0 to use the height of the frame, with range [0, infinity)
		 * @return True, if succeeded
		 * @see waitForFrameOutput().
		 */
		static bool windowFrameOutput(const double x, const double y, const Frame& frame, const std::string& title = std::string(), const bool modalIfSuitable = true, const double width = 0.0, const double height = 0.0);

		/**
		 * This function waits until all windows which have been created with `windowFrameOutput()` have been closed.
		 * @see windowFrameOutput().
		 */
		static void waitForFrameOutput();
};

}

}

}

}

#endif // META_OCEAN_PLATFORM_APPLE_MACOS_UTILITIES_H
