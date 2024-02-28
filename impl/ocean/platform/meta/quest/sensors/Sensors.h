// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_QUEST_SENSORS_SENSORS_H
#define META_OCEAN_PLATFORM_META_QUEST_SENSORS_SENSORS_H

#include "ocean/platform/Platform.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace Sensors
{

/**
 * @ingroup platformmetaquest
 * @defgroup platformmetaquestsensors Ocean Platform Meta Quest Sensors Library
 * @{
 * The Ocean Meta Quest Sensors Library provides access to Meta Quest's sensors interfaces.
 * The library is available on Meta's Quest platforms only.
 * @}
 */

/**
 * @namespace Ocean::Platform::Meta::Quest::Sensors Namespace of the Platform Meta Quest Sensors library.<p>
 * The Namespace Ocean::Platform::Meta::Quest::Sensors is used in the entire Ocean Platform Meta Quest Sensors Library.
 */

// Defines OCEAN_PLATFORM_META_QUEST_SENSORS_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_PLATFORM_META_QUEST_SENSORS_EXPORT
		#define OCEAN_PLATFORM_META_QUEST_SENSORS_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_PLATFORM_META_QUEST_SENSORS_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_PLATFORM_META_QUEST_SENSORS_EXPORT
#endif

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_SENSORS_SENSORS_H
