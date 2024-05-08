/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_GL_WIN_GL_WIN_H
#define META_OCEAN_PLATFORM_GL_WIN_GL_WIN_H

#include "ocean/platform/gl/GL.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

namespace Win
{

/**
 * @ingroup platformgl
 * @defgroup platformglwin Ocean Platform GL Windows Library
 * @{
 * The Ocean GL Library provides specific functionalities for OpenGL using the API of Microsoft Windows.
 * The library is available on Windows platforms.
 * @}
 */

/**
 * @namespace Ocean::Platform::GL::Win Namespace of the Platform GL Windows library.<p>
 * The Namespace Ocean::Platform::GL::Win is used in the entire Ocean Platform GL Windows Library.
 */

#ifndef _WINDOWS
	#error This library is available on windows platforms only!
#endif // _WINDOWS

// Defines OCEAN_PLATFORM_GL_WIN_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_PLATFORM_GL_WIN_EXPORT
		#define OCEAN_PLATFORM_GL_WIN_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_PLATFORM_GL_WIN_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_PLATFORM_GL_WIN_EXPORT
#endif

}

}

}

}

#endif // META_OCEAN_PLATFORM_GL_WIN_GL_WIN_H
