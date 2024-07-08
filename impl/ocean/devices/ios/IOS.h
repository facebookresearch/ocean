/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_IOS_IOS_H
#define META_OCEAN_DEVICES_IOS_IOS_H

#include "ocean/devices/Devices.h"

namespace Ocean
{

namespace Devices
{

namespace IOS
{

/**
 * @ingroup devices
 * @defgroup devicesios Ocean Devices IOS Library
 * @{
 * The Ocean Devices IOS Library provides device access for iOS platforms.<br>
 * The library is platform dependent.
 * @}
 */

/**
 * @namespace Ocean::Devices::IOS Namespace of the Devices IOS library.<p>
 * The Namespace Ocean::Devices::IOS is used in the entire Ocean Devices IOS Library.
 */

#ifndef __APPLE__
	#error This library is available on iOS platforms only!
#endif

// Defines OCEAN_DEVICES_IOS_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_DEVICES_IOS_EXPORT
		#define OCEAN_DEVICES_IOS_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_DEVICES_IOS_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_DEVICES_IOS_EXPORT
#endif

/**
 * Returns the name of this devices library.
 * @ingroup devicesios
 */
OCEAN_DEVICES_IOS_EXPORT const std::string& nameIOSLibrary();

#if defined(OCEAN_RUNTIME_STATIC)

/**
 * Registers this IOS device library at the global device manager.
 * This function calls IOSFactory::registerFactory() only.
 * @ingroup devicesios
 */
void registerIOSLibrary();

/**
 * Unregisters this IOS device library at the global device manager.
 * This function calls IOSFactory::unregisterFactory() only.
 * @return True, if succeeded
 * @ingroup devicesios
 */
bool unregisterIOSLibrary();

#endif // OCEAN_RUNTIME_STATIC

}

}

}

#endif // META_OCEAN_DEVICES_IOS_IOS_H
