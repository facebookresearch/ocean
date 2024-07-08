/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WXWIDGETS_BITMAP_H
#define META_OCEAN_PLATFORM_WXWIDGETS_BITMAP_H

#include "ocean/platform/wxwidgets/WxWidgets.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

/**
 * This class implements a bitmap.
 * @ingroup platformwxwidgets
 */
class OCEAN_PLATFORM_WXWIDGETS_EXPORT Bitmap : public wxBitmap
{
	public:

		/**
		 * Creates an empty bitmap object.
		 */
		inline Bitmap();

		/**
		 * Creates bitmap object from a given image object.
		 * @param image The image with either RGB24 or RGBA32 pixel format and upper left pixel origin.
		 * @param scaleFactor The scale factor converting the number of pixels in the (virtual) display coordinate system to the number of pixels in the (native/pyhsical) screen coordinate system, with range (0, infinity)
		 */
		explicit Bitmap(const wxImage& image, const double scaleFactor = 1.0);

		/**
		 * Creates a new bitmap object by a given frame.
		 * @param frame The frame from which the bitmap will be created
		 * @param convertIfNecessary True, to convert the given frame type if the frame's type cannot be represented by the bitmap; False, to create an empty bitmap if the type is not supported
		 * @param scaleFactor The scale factor converting the number of pixels in the (virtual) display coordinate system to the number of pixels in the (native/pyhsical) screen coordinate system, with range (0, infinity)
		 */
		explicit Bitmap(const Frame& frame, const bool convertIfNecessary = true, const double scaleFactor = 1.0);

		/**
		 * Creates a new bitmap object by a given (optional transparent) frame while the new bitmap will be opaque.
		 * A transparent frame is blended with a specified background color, an opaque frame is simply copied.
		 * @param frame The frame from which the bitmap will be created
		 * @param backgroundColor The background color that is applied if the frame is transparent, ensure that the provided buffer is large enough
		 * @param convertIfNecessary True, to convert the given frame type if the frame's type cannot be represented by the bitmap; False, to create an empty bitmap if the type is not supported
		 * @param scaleFactor The scale factor converting the number of pixels in the (virtual) display coordinate system to the number of pixels in the (native/pyhsical) screen coordinate system, with range (0, infinity)
		 */
		Bitmap(const Frame& frame, const unsigned char* backgroundColor, const bool convertIfNecessary, const double scaleFactor = 1.0);

		/**
		 * Replaces this bitmap by a given frame.
		 * @param frame The frame to set, may be invalid
		 * @param convertIfNecessary True, to convert the given frame type if the frame's type cannot be represented by the bitmap; False, to create an empty bitmap if the type is not supported
		 * @param scaleFactor The scale factor converting the number of pixels in the (virtual) display coordinate system to the number of pixels in the (native/pyhsical) screen coordinate system, with range (0, infinity)
		 * @return True, if succeeded
		 */
		bool setFrame(const Frame& frame, const bool convertIfNecessary = true, const double scaleFactor = 1.0);

		/**
		 * Replaces this bitmap by a given (optional transparent) frame while the final bitmap is opaque.
		 * Transparent frames are blended with a specified background color, opaque frames are copied directly.
		 * @param frame The frame to set, may be invalid
		 * @param backgroundColor The background color that is applied if the frame is transparent, ensure that the provided buffer is large enough
		 * @param convertIfNecessary True, to convert the given frame type if the frame's type cannot be represented by the bitmap; False, to create an empty bitmap if the type is not supported
		 * @param scaleFactor The scale factor converting the number of pixels in the (virtual) display coordinate system to the number of pixels in the (native/pyhsical) screen coordinate system, with range (0, infinity)
		 * @return True, if succeeded
		 */
		bool setFrame(const Frame& frame, const unsigned char* backgroundColor, const bool convertIfNecessary = true, const double scaleFactor = 1.0);

	protected:

		/**
		 * Replaces this bitmap by a given frame without alpha channel.
		 * @param frame The frame to set, must be valid
		 * @param convertIfNecessary True, to convert the given frame type if the frame's type cannot be represented by the bitmap; False, to create an empty bitmap if the type is not supported
		 * @param scaleFactor The scale factor converting the number of pixels in the (virtual) display coordinate system to the number of pixels in the (native/pyhsical) screen coordinate system, with range (0, infinity)
		 * @return True, if succeeded
		 */
		bool setFrameWithoutAlpha(const Frame& frame, const bool convertIfNecessary, const double scaleFactor);

		/**
		 * Replaces this bitmap by a given frame with alpha channel.
		 * @param frame The frame to set, must be valid
		 * @param convertIfNecessary True, to convert the given frame type if the frame's type cannot be represented by the bitmap; False, to create an empty bitmap if the type is not supported
		 * @param scaleFactor The scale factor converting the number of pixels in the (virtual) display coordinate system to the number of pixels in the (native/pyhsical) screen coordinate system, with range (0, infinity)
		 * @return True, if succeeded
		 */
		bool setFrameWithAlpha(const Frame& frame, const bool convertIfNecessary, const double scaleFactor);
};

inline Bitmap::Bitmap() :
	wxBitmap()
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_PLATFORM_WXWIDGETS_BITMAP_H
