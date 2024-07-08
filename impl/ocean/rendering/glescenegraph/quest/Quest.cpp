/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/quest/Quest.h"
#include "ocean/rendering/glescenegraph/quest/GLESEngineQuest.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace Quest
{

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

void registerGLESceneGraphEngine()
{
	GLESEngineQuest::registerEngine();
}

#endif // OCEAN_RUNTIME_STATIC

}

}

}

}
