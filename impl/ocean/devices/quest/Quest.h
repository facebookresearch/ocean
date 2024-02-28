// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_QUEST_QUEST_H
#define META_OCEAN_DEVICES_QUEST_QUEST_H

#include "ocean/devices/Devices.h"

namespace Ocean
{

namespace Devices
{

namespace Quest
{

/**
 * @ingroup devices
 * @defgroup devicesquest Ocean Devices Quest Library
 * @{
 * The Ocean Devices Quest Library provides access to devices for Quest VR devices.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Devices::Quest Namespace of the Devices Quest library.<p>
 * The Namespace Ocean::Devices::Quest is used in the entire Ocean Devices Quest Library.
 */

// Defines OCEAN_DEVICES_QUEST_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_DEVICES_QUEST_EXPORT
		#define OCEAN_DEVICES_QUEST_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_DEVICES_QUEST_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_DEVICES_QUEST_EXPORT
#endif

/**
 * Returns the name of this devices library.
 * @ingroup devicesquest
 */
OCEAN_DEVICES_QUEST_EXPORT std::string nameQuestLibrary();

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

/**
 * Registers this Quest device library at the global device manager.
 * This function calls QuestFactory::registerFactory() only.
 * @ingroup devicesquest
 */
void registerQuestLibrary();

/**
 * Unregisters this Quest device library at the global device manager.
 * This function calls QuestFactory::unregisterFactory() only.
 * @return True, if succeeded
 * @ingroup devicesquest
 */
bool unregisterQuestLibrary();

#endif // OCEAN_RUNTIME_STATIC

}

}

}

#endif // META_OCEAN_DEVICES_QUEST_QUEST_H
