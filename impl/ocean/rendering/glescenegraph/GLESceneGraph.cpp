/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESEngine.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

const std::string& nameGLESceneGraphEngine()
{
	static const std::string name = "GLESceneGraph";

	return name;
}

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

bool unregisterGLESceneGraphEngine()
{
	return GLESEngine::unregisterEngine();
}

#endif // OCEAN_RUNTIME_STATIC

}

}

}
