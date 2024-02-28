// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/glescenegraph/apple/Apple.h"
#include "ocean/rendering/glescenegraph/apple/GLESEngineApple.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace Apple
{

#if defined(OCEAN_RUNTIME_STATIC)

void registerGLESceneGraphEngine()
{
	GLESEngineApple::registerEngine();
}

#endif // OCEAN_RUNTIME_STATIC

}

}

}

}
