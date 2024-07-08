/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WXWIDGETS_IMAGE_LIST_H
#define META_OCEAN_PLATFORM_WXWIDGETS_IMAGE_LIST_H

#include "ocean/platform/wxwidgets/WxWidgets.h"

#include "ocean/base/Frame.h"

#include <wx/imaglist.h>

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

/**
 * This class implements an image list.
 * @ingroup platformwxwidgets
 */
class OCEAN_PLATFORM_WXWIDGETS_EXPORT ImageList : public wxImageList
{
	public:

		/**
		 * Creates a new image list object.
		 */
		ImageList() = default;

		/**
		 * Creates a new image list object from one large frame containing the individual images within the same row.
		 * The width of the given frame must be a multiple of the height of the given frame.
		 * @param frame The large frame containing all individual images, must be valid
		 * @param scaleFactor The scale factor converting the number of pixels in the (virtual) display coordinate system to the number of pixels in the (native/pyhsical) screen coordinate system, with range (0, infinity)
		 */
		explicit ImageList(const Frame& frame, const double scaleFactor = 1.0);
};

}

}

}

#endif // META_OCEAN_PLATFORM_WXWIDGETS_IMAGE_LIST_H
