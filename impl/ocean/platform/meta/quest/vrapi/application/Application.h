// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_APPLICATION_H
#define META_OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_APPLICATION_H

#include "ocean/platform/meta/quest/vrapi/VrApi.h"

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

namespace Application
{

/**
 * @ingroup platformmetaquestvrapi
 * @defgroup platformmetaquestvrapiapplication Ocean Platform Meta Quest VrApi Application Library
 * @{
 * The Ocean Meta Quest VrApi Application Library provides specific functionalities for Meta applications using VrApi.
 * The library is available on Quest VrApi platforms only.
 * @}
 */

/**
 * @namespace Ocean::Platform::Meta::Quest::VrApi::Application Namespace of the Platform Meta Quest VrApi Application library.<p>
 * The Namespace Ocean::Platform::Meta::Quest::VrApi::Application is used in the entire Ocean Platform Meta Quest VrApi Application Library.
 */

// Defines OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_EXPORT
		#define OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_EXPORT
#endif

}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_APPLICATION_H
