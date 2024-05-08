/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_LINUX_LINUX_H
#define META_OCEAN_PLATFORM_LINUX_LINUX_H

#include "ocean/platform/Platform.h"

namespace Ocean
{

namespace Platform
{

namespace Linux
{

/**
 * @ingroup platform
 * @defgroup platformlinux Ocean Platform Linux Library
 * @{
 * The Ocean Linux Library provides specific functionalities for Linux.
 * The library is available on Linux platforms only.
 * @}
 */

/**
 * @namespace Ocean::Platform::Linux Namespace of the Platform Linux library.<p>
 * The Namespace Ocean::Platform::Linux is used in the entire Ocean Platform Linux Library.
 */

// Defines OCEAN_PLATFORM_LINUX_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_PLATFORM_LINUX_EXPORT
		#define OCEAN_PLATFORM_LINUX_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_PLATFORM_LINUX_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_PLATFORM_LINUX_EXPORT
#endif

}

}

}

#endif // META_OCEAN_PLATFORM_LINUX_LINUX_H
