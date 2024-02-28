// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/glescenegraph/qt/QT.h"
#include "ocean/rendering/glescenegraph/qt/GLESEngineQT.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace QT
{

#if defined(OCEAN_RUNTIME_STATIC)

void registerGLESceneGraphEngine()
{
	GLESEngineQT::registerEngine();
}

#endif // OCEAN_RUNTIME_STATIC

}

}

}

}
