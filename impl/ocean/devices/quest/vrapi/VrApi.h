// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_QUEST_VRAPI_VR_API_H
#define META_OCEAN_DEVICES_QUEST_VRAPI_VR_API_H

#include "ocean/devices/quest/Quest.h"

namespace Ocean
{

namespace Devices
{

namespace Quest
{

namespace VrApi
{

/**
 * @ingroup devicesquest
 * @defgroup devicesquestvrapi Ocean Devices Quest VrApi Library
 * @{
 * The Ocean Devices Quest VrApi Library provides access to devices for Quest VR devices using VrApi.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Devices::Quest::VrApi Namespace of the Devices Quest VrApi library.<p>
 * The Namespace Ocean::Devices::Quest::VrApi is used in the entire Ocean Devices Quest VrApi Library.
 */

// Defines OCEAN_DEVICES_QUEST_VRAPI_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_DEVICES_QUEST_VRAPI_EXPORT
		#define OCEAN_DEVICES_QUEST_VRAPI_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_DEVICES_QUEST_VRAPI_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_DEVICES_QUEST_VRAPI_EXPORT
#endif

/**
 * Returns the name of this devices library.
 * @ingroup devicesquestvrapi
 */
OCEAN_DEVICES_QUEST_VRAPI_EXPORT std::string nameQuestVrApiLibrary();

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

/**
 * Registers this VrApi device library at the global device manager.
 * This function calls VrApiFactory::registerFactory() only.
 * @ingroup devicesquestvrapi
 */
void registerQuestVrApiLibrary();

/**
 * Unregisters this VrApi device library at the global device manager.
 * This function calls VrApiFactory::unregisterFactory() only.
 * @return True, if succeeded
 * @ingroup devicesquestvrapi
 */
bool unregisterQuestVrApiLibrary();

#endif // OCEAN_RUNTIME_STATIC

}

}

}

}

#endif // META_OCEAN_DEVICES_QUEST_VRAPI_VR_API_H
