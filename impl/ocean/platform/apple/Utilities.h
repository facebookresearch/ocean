/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_APPLE_UTILITIES_H
#define META_OCEAN_PLATFORM_APPLE_UTILITIES_H

#include "ocean/platform/apple/Apple.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace Platform
{

namespace Apple
{

/**
 * This class implements utilitiy functions for Apple platforms.
 * @ingroup platformapple
 */
class Utilities
{
	public:

		/**
		 * Converts a given string to a CFString reference object.
		 * Beware: The resulting string must be released via CFRelease once the string is not needed anymore.
		 * @param value The string to convert, may be empty
		 * @return The resulting converted string object
		 */
		static ScopedCFStringRef toCFString(const std::string& value);

		/**
		 * Converts a given string to a CFString reference object.
		 * Beware: The resulting string must be released via CFRelease once the string is not needed anymore.
		 * @param value The string to convert, may be empty
		 * @return The resulting converted string object
		 */
		static ScopedCFStringRef toCFString(const std::wstring& value);

		/**
		 * Creates an CGImage object for a given frame object.
		 * The frame data can either be copied or used only.<br>
		 * If the frame data is not copied, the frame data must exist as long as any related object exists relying on the resulting CGImage.<br>
		 * Beware: The caller has to release the image object by calling CGImageRelease.
		 *
		 * This function can be used to e.g., debug a Frame object within Xcode by using the `Quick look` functionality.
		 * Here is a code example how to debug the an image:
		 * @code
		 * #import <UIKit/UIKit.h>
		 *
		 * ScopedCGImageRef cgImage = Platform::Apple::Utilities::toCGImage(rgbFrame);
		 *
		 * // the following image object can be visualized in Xcode (via `Quick look`)
		 * UIImage* image = [[UIImage alloc] initWithCGImage:cgImage];
		 * @endcode
		 * @param frame The frame from which the CGImage object will be created
		 * @param copyData True, to create an CGImage object owning the frame data; False, to create an CGImageo object using the frame data only
		 * @return The resulting CGImage object, nullptr if the conversion failed
		 * @see asNSImageOrUIImage().
		 */
		static ScopedCGImageRef toCGImage(const Frame& frame, bool copyData = true);

		/**
		 * This function is a simple helper function to debug a frame on iOS or macOS.
		 * Within the function, Xcode's `Quick look` debugging functionality can be used to visualize the image's content.
		 * @param frame The frame to be debugged, must be valid
		 * @return True, if the frame could be handled
		 * @see toCGImage().
		 */
		static bool asNSImageOrUIImage(const Frame& frame);

		/**
		 * Shows a modal message box with a title, a message and an OK button.
		 * Beware: This function is not supported on iOS.
		 * @param title The title to be shown in the box
		 * @param message The message to be shown in the box
		 */
		static void showMessageBox(const std::string& title, const std::string& message);

		/**
		 * Returns the path to the document directory of the application.
		 * In a non-sandboxed application (e.g., a Mac console application), path will refer to the Documents folder in home.<br>
		 * In a sandboxed application (e.g., an iOS app), the path will refer to the Document folder of the app.<br>
		 * On iOS, you can use this path to the folder to e.g., to store files in the folder,<br>
		 * and you can access the data later using Apple's file sharing/accessing capabilities.
		 * Beware: You may need/want to add a 'UIFileSharingEnabled' key and a 'LSSupportsOpeningDocumentsInPlace' to the 'Info.plist' file of the application and set both to 'True'.
		 * @return The document directory's path
		 */
		static std::string documentDirectory();

		/**
		 * Returns whether the application's main window has exactly one supported interface orientation.
		 * <pre>
		 *          device top
		 * ...........................
		 * .                         .
		 * .         ^               .
		 * .       Y |               .
		 * .         |               .
		 * .         |               .
		 * .         O --------->    .
		 * .        /        X       .
		 * .       / Z               .
		 * .      v                  .
		 * .                         .
		 * ...........................
		 * .      home button        .
		 * ...........................
		 *       device bottom
		 * </pre>
		 * @param zAxisDegree The resulting angle of the supported orientation around the z-axis, possible values {-90, 0, 90, 180}
		 * @return True, if so
		 */
		static bool hasFixedSupportedInterfaceOrientation(int& zAxisDegree);

		/**
		 * Returns the application's version number.
		 * @param bundleShortVersion The resulting release version number of the bundle
		 * @param bundleVersionKey Optional resulting version key of the bundle, nullptr if not of interest
		 * @return True, if succeeded
		 */
		static bool bundleVersion(std::string& bundleShortVersion, std::string* bundleVersionKey = nullptr);
};

}

}

}

#endif // META_OCEAN_PLATFORM_APPLE_UTILITIES_H
