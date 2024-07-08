/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SYSTEM_USB_ANDROID_ANDROID_H
#define META_OCEAN_SYSTEM_USB_ANDROID_ANDROID_H

#include "ocean/system/usb/USB.h"

namespace Ocean
{

namespace System
{

namespace USB
{

namespace Android
{

/**
 * @ingroup systemusb
 * @defgroup systemusbandroid Ocean System USB Android Library
 * @{
 * The Ocean System USB Android Library provides access to devices via USB on Android platforms.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::System::USB::Android Namespace of the System library USB Android.<p>
 * The Namespace Ocean::System::USB::Android is used in the entire Ocean System USB Android Library.
 */

// Defines OCEAN_SYSTEM_USB_ANDROID_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_SYSTEM_USB_ANDROID_EXPORT
		#define OCEAN_SYSTEM_USB_ANDROID_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_SYSTEM_USB_ANDROID_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_SYSTEM_USB_ANDROID_EXPORT
#endif

}

}

}

}

#endif // META_OCEAN_SYSTEM_USB_ANDROID_ANDROID_H
