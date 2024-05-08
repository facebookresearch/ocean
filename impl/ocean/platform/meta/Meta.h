/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_META_H
#define META_OCEAN_PLATFORM_META_META_H

#include "ocean/platform/Platform.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

/**
 * @ingroup platform
 * @defgroup platformmeta Ocean Platform Meta Library
 * @{
 * The Ocean Meta Library provides specific functionalities for Meta.
 * The library is available on Meta platforms only.
 * @}
 */

/**
 * @namespace Ocean::Platform::Meta Namespace of the Platform Meta library.<p>
 * The Namespace Ocean::Platform::Meta is used in the entire Ocean Platform Meta Library.
 */

// Defines OCEAN_PLATFORM_META_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_PLATFORM_META_EXPORT
		#define OCEAN_PLATFORM_META_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_PLATFORM_META_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_PLATFORM_META_EXPORT
#endif

}

}

}

#endif // META_OCEAN_PLATFORM_META_META_H
