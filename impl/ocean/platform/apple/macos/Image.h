/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_APPLE_MACOS_IMAGE_H
#define META_OCEAN_PLATFORM_APPLE_MACOS_IMAGE_H

#include "ocean/platform/apple/macos/MacOS.h"

#include "ocean/base/Frame.h"

#ifndef __OBJC__
	#error Platform::Apple::MacOS::Image.h needs to be included from an ObjectiveC++ file
#endif

#include <AppKit/AppKit.h>

namespace Ocean
{

namespace Platform
{

namespace Apple
{

namespace MacOS
{

/**
 * This class implements a wrapper for the NSImage object.
 * @ingroup platformapplemacos
 */
class Image
{
	public:

		/**
		 * Creates a new but empty image object.
		 */
		Image() = default;

		/**
		 * Move constructor.
		 * @param image The image object to be moved
		 */
		Image(Image&& image) noexcept;

		/**
		 * Not existing copy constructor.
		 * @param image The image object to be copied
		 */
		Image(const Image& image) = delete;

		/**
		 * Creates a new image object from a given frame.
		 * The frame data of the given frame will be copied.<br>
		 * Beware: Ensure that the resulting image is valid before using the wrapped object.
		 * @param frame The frame for which a corresponding NSImage object will be wrapped.
		 * @param scaleFactor The scale factor converting the number of pixels in the (virtual) display coordinate system to the number of pixels in the (native/pyhsical) screen coordinate system, with range (0, infinity)
		 * @see isValid().
		 */
		explicit Image(const Frame& frame, const double scaleFactor = 1.0);

		/**
		 * Destructs an image object.
		 */
		~Image();

		/**
		 * Returns the scale factor of this image.
		 * @return imageScaleFactor The scale factor converting the number of pixels in the (virtual) display coordinate system to the number of pixels in the (native/pyhsical) screen coordinate system, with range (0, infinity)
		 */
		inline double scaleFactor() const;

		/**
		 * Returns the wrapped NSImage object.
		 * @return The NSImage object, nullptr if no object is wrapped
		 */
		inline NSImage* nsImage();

		/**
		 * Returns whether this object is valid and holds a valid wrapped NSImage object.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns whether this object is valid and holds a valid wrapped NSImage object.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Move operator.
		 * @param image The image object to be moved
		 * @return Reference to this object
		 */
		Image& operator=(Image&& image) noexcept;

		/**
		 * Not existing copy operator.
		 * @param image The image object to be copied
		 * @return Reference to this object
		 */
		Image& operator=(const Image& image) = delete;

	protected:

		/// The wrapped MacOS specific NSImage object of this image.
		NSImage* imageObject_ = nullptr;

		/// The scale factor converting the number of pixels in the (virtual) display coordinate system to the number of pixels in the (native/pyhsical) screen coordinate system, with range (0, infinity).
		double imageScaleFactor_ = 1.0;
};

inline double Image::scaleFactor() const
{
	return imageScaleFactor_;
}

inline NSImage* Image::nsImage()
{
	return imageObject_;
}

inline bool Image::isValid() const
{
	return imageObject_ != nullptr;
}

inline Image::operator bool() const
{
	return isValid();
}

}

}

}

}

#endif // META_OCEAN_PLATFORM_APPLE_MACOS_IMAGE_H
