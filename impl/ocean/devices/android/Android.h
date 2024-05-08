/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ANDROID_ANDROID_H
#define META_OCEAN_DEVICES_ANDROID_ANDROID_H

#include "ocean/devices/Devices.h"

namespace Ocean
{

namespace Devices
{

namespace Android
{

/**
 * @ingroup devices
 * @defgroup devicesandroid Ocean Devices Android Library
 * @{
 * The Ocean Devices Android Library provides device access for Android platforms.<br>
 * The library is platform dependent.
 * @}
 */

/**
 * @namespace Ocean::Devices::Android Namespace of the Devices Android library.<p>
 * The Namespace Ocean::Devices::Android is used in the entire Ocean Devices Android Library.
 */

#ifndef _ANDROID
	#error This library is available on android platforms only!
#endif // _ANDROID

// Defines OCEAN_DEVICES_ANDROID_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_DEVICES_ANDROID_EXPORT
		#define OCEAN_DEVICES_ANDROID_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_DEVICES_ANDROID_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_DEVICES_ANDROID_EXPORT
#endif

/**
 * Returns the name of this devices library.
 * @ingroup devicesandroid
 */
OCEAN_DEVICES_ANDROID_EXPORT std::string nameAndroidLibrary();

#if defined(OCEAN_RUNTIME_STATIC)

/**
 * Registers this Android device library at the global device manager.
 * This function calls AndroidFactory::registerFactory() only.
 * @return True, if succeeded
 * @ingroup devicesandroid
 */
bool registerAndroidLibrary();

/**
 * Unregisters this Android device library at the global device manager.
 * This function calls AndroidFactory::unregisterFactory() only.
 * @return True, if succeeded
 * @ingroup devicesandroid
 */
bool unregisterAndroidLibrary();

#endif // OCEAN_RUNTIME_STATIC

}

}

}

#endif // META_OCEAN_DEVICES_ANDROID_ANDROID_H
