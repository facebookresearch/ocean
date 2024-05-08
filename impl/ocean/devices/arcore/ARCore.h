/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ARCORE_ARCORE_H
#define META_OCEAN_DEVICES_ARCORE_ARCORE_H

#include "ocean/devices/Devices.h"

namespace Ocean
{

namespace Devices
{

namespace ARCore
{

/**
 * @ingroup devices
 * @defgroup devicesarcore Ocean Devices ARCore Library
 * @{
 * The Ocean Devices ARCore Library provides access to ARCore trackers.<br>
 * The library is available on Android platforms only.
 * @}
 */

/**
 * @namespace Ocean::Devices::ARCore Namespace of the Devices ARCore library.<p>
 * The Namespace Ocean::Devices::ARCore is used in the entire Ocean Devices ARCore Library.
 */

#ifndef _ANDROID
	#error This library is available on Android platforms only!
#endif

// Defines OCEAN_DEVICES_ARCORE_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_DEVICES_ARCORE_EXPORT
		#define OCEAN_DEVICES_ARCORE_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_DEVICES_ARCORE_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_DEVICES_ARCORE_EXPORT
#endif

/**
 * Returns the name of this devices library.
 * @ingroup devicesarcore
 */
OCEAN_DEVICES_ARCORE_EXPORT const std::string& nameARCoreLibrary();

#if defined(OCEAN_RUNTIME_STATIC)

/**
 * Registers this ARCore devices library at the global device manager.
 * This function calls AKFactory::registerFactory() only.
 * @ingroup devicesarcore
 */
void registerARCoreLibrary();

/**
 * Unregisters this ARCore devices library at the global device manager.
 * This function calls AKFactory::unregisterFactory() only.
 * @return True, if succeeded
 * @ingroup devicesarcore
 */
bool unregisterARCoreLibrary();

#endif // OCEAN_RUNTIME_STATIC

}

}

}

#endif // META_OCEAN_DEVICES_ARCORE_ARCORE_H
