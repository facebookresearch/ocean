/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_RMV_RMV_H
#define META_OCEAN_DEVICES_RMV_RMV_H

#include "ocean/devices/Devices.h"

namespace Ocean
{

namespace Devices
{

namespace RMV
{

/**
 * @ingroup devices
 * @defgroup devicesrmv Ocean Devices RMV Library
 * @{
 * The Ocean Devices RMV Library provides a RMV feature based tracking library.
 * This device plugin provides a 6DOF feature tracker with unique name <strong>"RMV Feature Based 6DOF Tracker"</strong>.<br>
 * Do not create a tracker object directory, but use the Devices::Manager object instead.<br>
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Devices::RMV Namespace of the Devices RMV library.<p>
 * The Namespace Ocean::Devices::RMV is used in the entire Ocean Devices RMV Library.
 */

// Defines OCEAN_DEVICES_RMV_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_DEVICES_RMV_EXPORT
		#define OCEAN_DEVICES_RMV_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_DEVICES_RMV_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_DEVICES_RMV_EXPORT
#endif

/**
 * Returns the name of this devices library.
 * @ingroup devicesrmv
 */
OCEAN_DEVICES_RMV_EXPORT std::string nameRMVLibrary();

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

/**
 * Registers this RMV device library at the global device manager.
 * This function calls RMVFactory::registerFactory() only.
 * @ingroup devicesrmv
 */
void registerRMVLibrary();

/**
 * Unregisters this RMV device library at the global device manager.
 * This function calls RMVFactory::unregisterFactory() only.
 * @return True, if succeeded
 * @ingroup devicesrmv
 */
bool unregisterRMVLibrary();

#endif // OCEAN_RUNTIME_STATIC

}

}

}

#endif // META_OCEAN_DEVICES_RMV_RMV_H
