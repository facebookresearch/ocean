/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
