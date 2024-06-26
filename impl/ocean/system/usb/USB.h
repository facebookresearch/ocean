/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SYSTEM_USB_USB_H
#define META_OCEAN_SYSTEM_USB_USB_H

#include "ocean/system/System.h"

#include "ocean/base/ScopedObject.h"

#include <libusb.h>

namespace Ocean
{

namespace System
{

namespace USB
{

/**
 * @ingroup system
 * @defgroup systemusb Ocean System USB Library
 * @{
 * The Ocean System USB Library provides access to USB functionalities.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::System::USB Namespace of the System library USB.<p>
 * The Namespace Ocean::System::USB is used in the entire Ocean System USB Library.
 */

// Defines OCEAN_SYSTEM_USB_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_SYSTEM_USB_EXPORT
		#define OCEAN_SYSTEM_USB_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_SYSTEM_USB_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_SYSTEM_USB_EXPORT
#endif

/**
 * Helper function to free a libusb transfer.
 * @param libusbTransfer The transfer to be release
 * @ingroup systemusb
 */
OCEAN_SYSTEM_USB_EXPORT void releaseTransfer(libusb_transfer* libusbTransfer);

/**
 * Definition of a scoped libusb transfer object.
 * @ingroup systemusb
 */
using ScopedTransfer = ScopedObjectCompileTimeVoidT<libusb_transfer*, releaseTransfer>;

/**
 * Definition of a vector holding scoped libusb transfer objects.
 * @ingroup systemusb
 */
using ScopedTransfers = std::vector<ScopedTransfer>;

/**
 * Definition of an unordered map mapping libusb transfer points to indices.
 */
using TransferIndexMap = std::unordered_map<libusb_transfer*, size_t>;

}

}

}

#endif // META_OCEAN_SYSTEM_USB_USB_H
