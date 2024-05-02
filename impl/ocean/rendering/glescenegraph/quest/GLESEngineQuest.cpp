// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/glescenegraph/quest/GLESEngineQuest.h"

#include "ocean/rendering/Manager.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace Quest
{

GLESEngineQuest::GLESEngineQuest(const GraphicAPI graphicAPI) :
	GLESEngine(graphicAPI)
{
	// nothing to do here
}

void GLESEngineQuest::registerEngine()
{
	Engine::registerEngine(nameGLESceneGraphEngine(), CreateCallback(&GLESEngineQuest::createEngine), glesGraphicAPI(), 10);
}

Engine* GLESEngineQuest::createEngine(const GraphicAPI graphicAPI)
{
	Engine* newEngine = new GLESEngineQuest(graphicAPI);

	if (newEngine == nullptr)
	{
		throw OutOfMemoryException("Not enough memory to create a new engine.");
	}

	return newEngine;
}

}

}

}

}
