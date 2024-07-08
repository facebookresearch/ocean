/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESEngine.h"

#include "ocean/rendering/Manager.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESEngine::GLESEngine(const GraphicAPI graphicAPI) :
	Engine(graphicAPI),
	localName_(nameGLESceneGraphEngine())
{
	// nothing to do here
}

GLESEngine::~GLESEngine()
{
	ocean_assert(!ObjectRefManager::get().hasEngineObject(localName_, true) && "Still rendering objects existing");
}

bool GLESEngine::unregisterEngine()
{
	return Engine::unregisterEngine(nameGLESceneGraphEngine());
}

const Factory& GLESEngine::factory() const
{
	return factory_;
}

const std::string& GLESEngine::engineName() const
{
	return localName_;
}

}

}

}
