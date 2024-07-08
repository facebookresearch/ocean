/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_GL_GL_H
#define META_OCEAN_PLATFORM_GL_GL_H

#include "ocean/platform/Platform.h"

#if defined(_WINDOWS)
	#include <gl/gl.h>
	#include <gl/glext.h>
#elif defined(__APPLE__)
	#define GL_GLEXT_PROTOTYPES
	#include <OpenGL/OpenGL.h>
	#include <OpenGL/gl.h>
#else
	#error Not supported platform.
#endif

namespace Ocean
{

namespace Platform
{

namespace GL
{

/**
 * @ingroup platform
 * @defgroup platformgl Ocean Platform GL Library
 * @{
 * The Ocean GL Library provides specific functionalities for OpenGL.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Platform::GL Namespace of the Platform GL library.<p>
 * The Namespace Ocean::Platform::GL is used in the entire Ocean Platform GL Library.
 */

// Defines OCEAN_PLATFORM_GL_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_PLATFORM_GL_EXPORT
		#define OCEAN_PLATFORM_GL_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_PLATFORM_GL_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_PLATFORM_GL_EXPORT
#endif

}

}

}

#endif // META_OCEAN_PLATFORM_GL_GL_H
