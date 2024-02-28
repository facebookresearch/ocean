// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_AVATARS_AVATARS_H
#define META_OCEAN_PLATFORM_META_AVATARS_AVATARS_H

#include "ocean/platform/meta/Meta.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Avatars
{

/**
 * @ingroup platformmeta
 * @defgroup platformmetaavatars Ocean Platform Meta Avatars Library
 * @{
 * The Ocean Meta Avatars Library provides specific functionalities for Meta avatars.
 * The library is available on Meta platforms only.
 * @}
 */

/**
 * @namespace Ocean::Platform::Meta::Avatars Namespace of the Platform Meta Avatars library.<p>
 * The Namespace Ocean::Platform::Meta::Avatars is used in the entire Ocean Platform Meta Avatars Library.
 */

// Defines OCEAN_PLATFORM_META_AVATARS_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_PLATFORM_META_AVATARS_EXPORT
		#define OCEAN_PLATFORM_META_AVATARS_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_PLATFORM_META_AVATARS_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_PLATFORM_META_AVATARS_EXPORT
#endif

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_AVATARS_AVATARS_H
