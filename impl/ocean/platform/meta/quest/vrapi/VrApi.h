// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_QUEST_VRAPI_VR_API_H
#define META_OCEAN_PLATFORM_META_QUEST_VRAPI_VR_API_H

#include "ocean/platform/meta/quest/Quest.h"

#include <VrApi.h>

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace VrApi
{

/**
 * @ingroup platformmetaquest
 * @defgroup platformmetaquestvrapi Ocean Platform Meta Quest VrApi Library
 * @{
 * The Ocean Meta Quest VrApi Library provides specific functionalities for Meta's Quest VrApi platform.
 * The library is available on Meta Quest VrApi platforms only.
 * @}
 */

/**
 * @namespace Ocean::Platform::Meta::Quest::VrApi Namespace of the Platform Meta Quest VrApi library.<p>
 * The Namespace Ocean::Platform::Meta::Quest::VrApi is used in the entire Ocean Platform Meta Quest VrApi Library.
 */

// Defines OCEAN_PLATFORM_META_QUEST_VRAPI_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_PLATFORM_META_QUEST_VRAPI_EXPORT
		#define OCEAN_PLATFORM_META_QUEST_VRAPI_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_PLATFORM_META_QUEST_VRAPI_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_PLATFORM_META_QUEST_VRAPI_EXPORT
#endif

// Defines OCEAN_VRAPI_VERSION as VRAPI_MAJOR_VERSION * 1000 + VRAPI_MINOR_VERSION
#if defined(VRAPI_MAJOR_VERSION) && defined(VRAPI_MINOR_VERSION)
	#define OCEAN_VRAPI_VERSION (VRAPI_MAJOR_VERSION * 1000 + VRAPI_MINOR_VERSION)
#else
	#define OCEAN_VRAPI_VERSION 0
#endif

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_VRAPI_VR_API_H
