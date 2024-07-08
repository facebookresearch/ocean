/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_USB_USB_MEDIUM_H
#define META_OCEAN_MEDIA_USB_USB_MEDIUM_H

#include "ocean/media/usb/USB.h"

#include "ocean/media/Medium.h"

namespace Ocean
{

namespace Media
{

namespace USB
{

/**
 * This class implements the base class for all Medium objects in the USB library.
 * @ingroup mediausb
 */
class OCEAN_MEDIA_USB_EXPORT USBMedium : virtual public Medium
{
	friend class USBLibrary;

	protected:

		/**
		 * Creates a new medium by a given url.
		 * @param url The URL of the medium
		 */
		explicit USBMedium(const std::string& url);
};

}

}

}

#endif // META_OCEAN_MEDIA_USB_USB_MEDIUM_H
