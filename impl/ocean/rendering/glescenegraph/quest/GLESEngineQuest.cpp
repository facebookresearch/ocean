// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/glescenegraph/quest/GLESEngineQuest.h"

#ifdef OCEAN_RENDERING_GLES_QUEST_PLATFORM_OPENXR
	#include "ocean/rendering/glescenegraph/quest/openxr/GLESWindowFramebuffer.h"
#endif

#ifdef OCEAN_RENDERING_GLES_QUEST_PLATFORM_VRAPI
	#include "ocean/rendering/glescenegraph/quest/vrapi/GLESWindowFramebuffer.h"
#endif

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

Framebuffer* GLESEngineQuest::internalCreateFramebuffer(const Framebuffer::FramebufferType type, const Framebuffer::FramebufferConfig& config)
{
	if (type == Framebuffer::FRAMEBUFFER_WINDOW)
	{
#ifdef OCEAN_RENDERING_GLES_QUEST_PLATFORM_OPENXR
		return new OpenXR::GLESWindowFramebuffer(config);
#endif

#ifdef OCEAN_RENDERING_GLES_QUEST_PLATFORM_VRAPI
		return new VrApi::GLESWindowFramebuffer(config);
#endif
	}

	return nullptr;
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
