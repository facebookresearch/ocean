// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_VRS_VRS_H
#define META_OCEAN_DEVICES_VRS_VRS_H

#include "ocean/devices/Devices.h"

namespace Ocean
{

namespace Devices
{

namespace VRS
{

/**
 * @ingroup devices
 * @defgroup devicesvrs Ocean Devices VRS Library
 * @{
 * The Ocean Devices VRS Library provides functionalties to record and replay device information.<br>
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Devices::VRS Namespace of the Devices VRS library.<p>
 * The Namespace Ocean::Devices::VRS is used in the entire Ocean Devices VRS Library.
 */

// Defines OCEAN_DEVICES_VRS_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_DEVICES_VRS_EXPORT
		#define OCEAN_DEVICES_VRS_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_DEVICES_VRS_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_DEVICES_VRS_EXPORT
#endif

/**
 * Returns the name of this devices library.
 * @ingroup devicesvrs
 */
OCEAN_DEVICES_VRS_EXPORT std::string nameVRSLibrary();

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

/**
 * Registers this VRS device library at the global device manager.
 * This function calls VRSFactory::registerFactory() only.
 * @ingroup devicesvrs
 */
void registerVRSLibrary();

/**
 * Unregisters this VRS device library at the global device manager.
 * This function calls VRSFactory::unregisterFactory() only.
 * @return True, if succeeded
 * @ingroup devicesvrs
 */
bool unregisterVRSLibrary();

#endif // OCEAN_RUNTIME_STATIC

}

}

}

#endif // META_OCEAN_DEVICES_VRS_VRS_H
