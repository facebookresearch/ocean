// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_RENDERING_GLES_WINDOWS_WINDOWS_H
#define META_OCEAN_RENDERING_GLES_WINDOWS_WINDOWS_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace Windows
{

#if defined(OCEAN_RUNTIME_STATIC)

/**
 * Registers this rendering engine at the global engine manager.
 * This function calls GLESEngine::registerEngine() only.
 * @ingroup renderinggles
 */
void registerGLESceneGraphEngine();

#endif // OCEAN_RUNTIME_STATIC

}

}

}

}

#endif // META_OCEAN_RENDERING_GLES_WINDOWS_WINDOWS_H
