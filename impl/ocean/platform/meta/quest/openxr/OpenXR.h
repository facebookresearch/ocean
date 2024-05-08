/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_QUEST_OPENXR_OPEN_XR_H
#define META_OCEAN_PLATFORM_META_QUEST_OPENXR_OPEN_XR_H

#include "ocean/platform/meta/quest/Quest.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace OpenXR
{

/**
 * @ingroup platformmetaquest
 * @defgroup platformmetaquestopenxr Ocean Platform Meta Quest OpenXR Library
 * @{
 * The Ocean Meta Quest OpenXR Library provides specific functionalities for Meta's Quest platform via OpenXR.
 * The library is available on Meta Quest OpenXR platforms only.
 * @}
 */

/**
 * @namespace Ocean::Platform::Meta::Quest::OpenXR Namespace of the Platform Meta Quest OpenXR library.<p>
 * The Namespace Ocean::Platform::Meta::Quest::OpenXR is used in the entire Ocean Platform Meta Quest OpenXR Library.
 */

// Defines OCEAN_PLATFORM_META_QUEST_OPENXR_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_PLATFORM_META_QUEST_OPENXR_EXPORT
		#define OCEAN_PLATFORM_META_QUEST_OPENXR_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_PLATFORM_META_QUEST_OPENXR_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_PLATFORM_META_QUEST_OPENXR_EXPORT
#endif

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_OPENXR_OPEN_XR_H
