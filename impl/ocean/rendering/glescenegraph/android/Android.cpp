// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/glescenegraph/android/Android.h"
#include "ocean/rendering/glescenegraph/android/GLESEngineAndroid.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace Android
{

#if defined(OCEAN_RUNTIME_STATIC)

void registerGLESceneGraphEngine()
{
	GLESEngineAndroid::registerEngine();
}

#endif // OCEAN_RUNTIME_STATIC

}

}

}

}
