/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_GLESCENEGRAPH_H
#define META_OCEAN_RENDERING_GLES_GLESCENEGRAPH_H

#include "ocean/rendering/Rendering.h"

#ifdef OCEAN_PLATFORM_BUILD_MOBILE
	// we use OpenGL ES on Android and iOS, otherwise we use OpenGL
	#define OCEAN_RENDERING_GLES_USE_ES
#endif

#ifndef OCEAN_RENDERING_GLES_USE_ES
	#ifdef _WINDOWS
		// Windows does not provide libraries for OpenGL 2.0+ so that we have to load the function pointers at runtime
		#define OCEAN_RENDERING_GLES_USE_DYNAMIC_LIBRARY
	#endif
#endif

// Defines OCEAN_RENDERING_GLES_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_RENDERING_GLES_EXPORT
		#define OCEAN_RENDERING_GLES_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_RENDERING_GLES_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_RENDERING_GLES_EXPORT
#endif

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * @ingroup rendering
 * @defgroup renderinggles Ocean Rendering GLESceneGraph Library
 * @{
 * The Ocean Rendering GLESceneGraph Library is a rendering library using OpenGL ES for rendering.
 * This library is platform independent.<br>
 * However, the library is using Nativ Platform Graphic Interface (EGL) as default.<br>
 * To avoid the usage of the EGL implement an own engine and framebuffer object.<br>
 * @}
 */

/**
 * @namespace Ocean::Rendering::GLESceneGraph Namespace of the Rendering GLESceneGraph library.<p>
 * The Namespace Ocean::Rendering::GLESceneGraph is used in the entire Ocean Rendering GLESceneGraph Library.
 */

/**
 * Returns the name of this GLESceneGraph library.
 * @ingroup renderinggles
 */
OCEAN_RENDERING_GLES_EXPORT const std::string& nameGLESceneGraphEngine();

#if defined(OCEAN_RUNTIME_STATIC)

/**
 * Unregisters this rendering engine at the global engine manager.
 * This function calls GLESEngine::unregisterEngine() only.
 * @return True, if succeeded
 * @ingroup renderinggles
 */
bool unregisterGLESceneGraphEngine();

#endif // OCEAN_RUNTIME_STATIC

}

}

}

#endif // META_OCEAN_RENDERING_GLES_GLESCENEGRAPH_H
