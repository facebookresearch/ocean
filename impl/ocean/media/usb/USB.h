/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_USB_USB_H
#define META_OCEAN_MEDIA_USB_USB_H

#include "ocean/media/Media.h"

namespace Ocean
{

namespace Media
{

namespace USB
{

/**
 * @ingroup media
 * @defgroup mediausb Ocean Media USB Library
 * @{
 * The Ocean Media USB Library provides camera access to USB video devices.
 * The library is platform independent but is mainly intended for Android platforms, as USB device access through libusb is quite limited on other platforms.
 * @}
 */

/**
 * @namespace Ocean::Media::USB Namespace of the Media USB library.<p>
 * The Namespace Ocean::Media::USB is used in the entire Ocean Media USB Library.
 */

// Defines OCEAN_MEDIA_USB_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_MEDIA_USB_EXPORT
		#define OCEAN_MEDIA_USB_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_MEDIA_USB_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_MEDIA_USB_EXPORT
#endif

/**
 * Returns the name of this media library.
 * @ingroup mediausb
 */
OCEAN_MEDIA_USB_EXPORT std::string nameUSBLibrary();

#ifdef OCEAN_RUNTIME_STATIC

/**
 * Registers this media library at the global library manager.
 * This function calls USBLibrary::registerLibrary() only.
 * @ingroup mediausb
 */
void registerUSBLibrary();

/**
 * Unregisters this media library at the global library manager.
 * This function calls USBLibrary::unregisterLibrary() only.
 * @return True, if succeeded
 * @ingroup mediausb
 */
bool unregisterUSBLibrary();

#endif // OCEAN_RUNTIME_STATIC

}

}

}

#endif // META_OCEAN_MEDIA_USB_USB_H
