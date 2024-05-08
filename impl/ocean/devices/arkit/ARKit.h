/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ARKIT_ARKIT_H
#define META_OCEAN_DEVICES_ARKIT_ARKIT_H

#include "ocean/devices/Devices.h"

namespace Ocean
{

namespace Devices
{

namespace ARKit
{

/**
 * @ingroup devices
 * @defgroup devicesarkit Ocean Devices ARKit Library
 * @{
 * The Ocean Devices ARKit Library provides access to ARKit trackers.<br>
 * The library is available on iOS platforms only.
 * @}
 */

/**
 * @namespace Ocean::Devices::ARKit Namespace of the Devices ARKit library.<p>
 * The Namespace Ocean::Devices::ARKit is used in the entire Ocean Devices ARKit Library.
 */

#ifndef __APPLE__
	#error This library is available on iOS platforms only!
#endif

// Defines OCEAN_DEVICES_ARKIT_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_DEVICES_ARKIT_EXPORT
		#define OCEAN_DEVICES_ARKIT_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_DEVICES_ARKIT_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_DEVICES_ARKIT_EXPORT
#endif

/**
 * Returns the name of this devices library.
 * @ingroup devicesarkit
 */
OCEAN_DEVICES_ARKIT_EXPORT const std::string& nameARKitLibrary();

#if defined(OCEAN_RUNTIME_STATIC)

/**
 * Registers this ARKit devices library at the global device manager.
 * This function calls AKFactory::registerFactory() only.
 * @ingroup devicesarkit
 */
void registerARKitLibrary();

/**
 * Unregisters this ARKit devices library at the global device manager.
 * This function calls AKFactory::unregisterFactory() only.
 * @return True, if succeeded
 * @ingroup devicesarkit
 */
bool unregisterARKitLibrary();

#endif // OCEAN_RUNTIME_STATIC

}

}

}

#endif // META_OCEAN_DEVICES_ARKIT_ARKIT_H
