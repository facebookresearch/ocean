/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WIN_WIN_H
#define META_OCEAN_PLATFORM_WIN_WIN_H

#include "ocean/platform/Platform.h"

#ifndef _WINDOWS
	#error This library is available on windows platforms only!
#endif // _WINDOWS

#include <winsock2.h>
#include <windows.h>

namespace Ocean
{

namespace Platform
{

namespace Win
{

/**
 * @ingroup platform
 * @defgroup platformwin Ocean Platform Win Library
 * @{
 * The Ocean Win Library provides specific functionalities for windows platforms.
 * The library is available on windows platforms only.
 * @}
 */

/**
 * @namespace Ocean::Platform::Win Namespace of the Platform Win library.<p>
 * The Namespace Ocean::Platform::Win is used in the entire Ocean Platform Win Library.
 */

// Defines OCEAN_PLATFORM_WIN_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_PLATFORM_WIN_EXPORT
		#define OCEAN_PLATFORM_WIN_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_PLATFORM_WIN_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_PLATFORM_WIN_EXPORT
#endif

}

}

}

#endif // META_OCEAN_PLATFORM_WIN_WIN_H
