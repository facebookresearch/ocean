/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_ANDROID_ANDROID_H
#define META_OCEAN_RENDERING_GLES_ANDROID_ANDROID_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace Android
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

#endif // META_OCEAN_RENDERING_GLES_ANDROID_ANDROID_H
