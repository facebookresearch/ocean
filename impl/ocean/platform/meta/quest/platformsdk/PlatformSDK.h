/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_PLATFORM_SDK_H
#define META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_PLATFORM_SDK_H

#include "ocean/platform/meta/quest/Quest.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace PlatformSDK
{

/**
 * @ingroup platformmetaquest
 * @defgroup platformmetaquestplatformsdk Ocean Platform Meta Quest Platform SDK Library
 * @{
 * The Ocean Meta Quest Platform SDK Library provides specific functionalities for Meta Quest Platform SDK.
 * The library is available on Meta Quest platforms only.
 * @}
 */

/**
 * @namespace Ocean::Platform::Meta::Quest::PlatformSDK Namespace of the Platform Meta Quest Platform SDK library.<p>
 * The Namespace Ocean::Platform::Meta::Quest::PlatformSDK is used in the entire Ocean Platform Meta Quest Platform SDK Library.
 */

// Defines OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_EXPORT
		#define OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_EXPORT
#endif

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_PLATFORM_SDK_H
