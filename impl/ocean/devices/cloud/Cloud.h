// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_CLOUD_CLOUD_H
#define META_OCEAN_DEVICES_CLOUD_CLOUD_H

#include "ocean/devices/Devices.h"

namespace Ocean
{

namespace Devices
{

namespace Cloud
{

/**
 * @ingroup devices
 * @defgroup devicescloud Ocean Devices Cloud Library
 * @{
 * The Ocean Devices Cloud Library provides access to tracker based on Cloud Relocalization.<br>
 * @}
 */

/**
 * @namespace Ocean::Devices::Cloud Namespace of the Devices Cloud library.<p>
 * The Namespace Ocean::Devices::Cloud is used in the entire Ocean Devices Cloud Library.
 */

// Defines OCEAN_DEVICES_CLOUD_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_DEVICES_CLOUD_EXPORT
		#define OCEAN_DEVICES_CLOUD_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_DEVICES_CLOUD_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_DEVICES_CLOUD_EXPORT
#endif

/**
 * Returns the name of this devices library.
 * @ingroup devicescloud
 */
OCEAN_DEVICES_CLOUD_EXPORT const std::string& nameCloudLibrary();

#if defined(OCEAN_RUNTIME_STATIC)

/**
 * Registers this Cloud devices library at the global device manager.
 * This function calls CloudFactory::registerFactory() only.
 * @ingroup devicescloud
 */
void registerCloudLibrary();

/**
 * Unregisters this Cloud devices library at the global device manager.
 * This function calls CloudFactory::unregisterFactory() only.
 * @return True, if succeeded
 * @ingroup devicescloud
 */
bool unregisterCloudLibrary();

#endif // OCEAN_RUNTIME_STATIC

}

}

}

#endif // META_OCEAN_DEVICES_CLOUD_CLOUD_H
