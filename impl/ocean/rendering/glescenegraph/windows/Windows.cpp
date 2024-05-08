/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/windows/Windows.h"
#include "ocean/rendering/glescenegraph/windows/GLESEngineWindows.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace Windows
{

#if defined(OCEAN_RUNTIME_STATIC)

void registerGLESceneGraphEngine()
{
	GLESEngineWindows::registerEngine();
}

#endif // OCEAN_RUNTIME_STATIC

}

}

}

}
