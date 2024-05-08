/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_APPLE_IOS_UTILITIES_H
#define META_OCEAN_PLATFORM_APPLE_IOS_UTILITIES_H

#include "ocean/platform/apple/ios/IOS.h"

#include "ocean/base/Callback.h"
#include "ocean/base/Frame.h"

namespace Ocean
{

namespace Platform
{

namespace Apple
{

namespace IOS
{

/**
 * This class implements utilitiy functions for Apple iOS platforms.
 * @ingroup platformappleios
 */
class Utilities
{
	public:

		/**
		 * Definition of a callback function for picked images.
		 */
		typedef Callback<void, void*> OnPickedImageCallback;

	public:

		/**
		 * Loads an image from the photo album of the iPhone.
		 * @param callback The callback function which will be called one the image has been loaded
		 *
		 * The following code shows how to use this function:
		 * @code
		 * void onImage(void* image)
		 * {
		 *     UIImage* uiImage = (__bridge UIImage*)image;
		 *     CGImageRef cgImage = uiImage.CGImage;
		 *
		 *     const size_t imageWidth = CGImageGetWidth(cgImage);
		 *
		 *     ...
		 * }
		 *
		 * void function()
		 * {
		 *     // let's pick an image from the photo album
		 *     Utilities::loadImageFromPhotoAlbum(Utilities::OnPickedImageCallback::createStatic(onImage));
		 * }
		 * @endcode
		 */
		static void loadImageFromPhotoAlbum(const OnPickedImageCallback& callback);

		/**
		 * Saves an image to the photos album of the iPhone.
		 * Beware: You may need to add a 'NSPhotoLibraryAddUsageDescription' key to the 'Info.plist' file of the application.
		 * @param frame The image to be saved, must be valid
		 * @return True, if succeeded
		 * @see shareImage(), shareImages().
		 */
		static bool saveImageToPhotosAlbum(const Frame& frame);

		/**
		 * Shares an image using iOS' Activity view controller.
		 * The controller allows to copy or to share the provided image.<br>
		 * By calling this function, you will see iPhone's default sharing functionality offering to share the provided image.
		 * @param frame The frame to be shared, must be valid
		 * @return True, if the frame could be shared
		 * @see shareImages(), saveImageToPhotosAlbum().
		 */
		static bool shareImage(const Frame& frame);

		/**
		 * Shares images using iOS' Activity view controller.
		 * The controller allows to copy or to share the provided images.<br>
		 * By calling this function, you will see iPhone's default sharing functionality offering to share the provided images.
		 * @param frames The frames to be shared, at least one
		 * @return True, if at least one frame could be shared
		 * @see shareImage(), saveImageToPhotosAlbum().
		 */
		static bool shareImages(const Frames& frames);

		/**
		 * Shares a text using iOS' Activity view controller.
		 * The controller allows to copy or to share the provided text.<br>
		 * By calling this function, you will see iPhone's default sharing functionality offering to share the provided text.
		 * @param text The text to share, must not be empty
		 * @return True, if the text could be shared
		 * @see shareFile(), shareImage(), saveImageToPhotosAlbum().
		 */
		static bool shareText(const std::string& text);

		/**
		 * Shares a file using iOS' Activity view controller.
		 * The controller allows to copy or to share the provided file.<br>
		 * By calling this function, you will see iPhone's default sharing functionality offering to share the provided file.
		 * @param filename The path of the file to be shared, must be valid
		 * @return True, if the file could be shared
		 * @see shareFiles(), shareText(), shareImage(), saveImageToPhotosAlbum().
		 */
		static bool shareFile(const std::string& filename);

		/**
		 * Shares several files using iOS' Activity view controller.
		 * The controller allows to copy or to share the provided files.<br>
		 * By calling this function, you will see iPhone's default sharing functionality offering to share the provided file.
		 * @param filenames The paths of the files to be shared, at least one
		 * @return True, if the files could be shared
		 * @see shareFile(), shareText(), shareImage(), saveImageToPhotosAlbum().
		 */
		static bool shareFiles(const Strings& filenames);

	protected:

		/**
		 * Casts a given frame with arbitrary pixel format to a frame with pixel format with defined channel layout.
		 * Beware: The resulting frame will not be the owner of the image content!
		 * @param frame The frame to cast, must be valid
		 * @return The resulting frame with supported pixel format, an invalid frame if now valid pixel format exists
		 */
		static Frame castToSupportedPixelFormat(const Frame& frame);
};

}

}

}

}

#endif // META_OCEAN_PLATFORM_APPLE_IOS_UTILITIES_H
