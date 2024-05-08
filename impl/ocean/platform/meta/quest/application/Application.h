/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_QUEST_APPLICATION_APPLICATION_H
#define META_OCEAN_PLATFORM_META_QUEST_APPLICATION_APPLICATION_H

#include "ocean/platform/meta/quest/Quest.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace Application
{

/**
 * @ingroup platformmetaquest
 * @defgroup platformmetaquestapplication Ocean Platform Meta Quest Application Library
 * @{
 * The Ocean Meta Quest Application Library provides specific functionalities for Meta applications.
 * The library is available on Quest platforms only.
 * @}
 */

/**
 * @namespace Ocean::Platform::Meta::Quest::Application Namespace of the Platform Meta Quest Application library.<p>
 * The Namespace Ocean::Platform::Meta::Quest::Application is used in the entire Ocean Platform Meta Quest Application Library.
 */

// Defines OCEAN_PLATFORM_META_QUEST_APPLICATION_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_PLATFORM_META_QUEST_APPLICATION_EXPORT
		#define OCEAN_PLATFORM_META_QUEST_APPLICATION_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_PLATFORM_META_QUEST_APPLICATION_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_PLATFORM_META_QUEST_APPLICATION_EXPORT
#endif

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_APPLICATION_APPLICATION_H
