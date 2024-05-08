/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
