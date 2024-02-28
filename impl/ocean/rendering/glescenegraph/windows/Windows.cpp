// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
