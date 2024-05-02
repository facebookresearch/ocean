// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/glescenegraph/quest/GLESEngineQuest.h"

#include "ocean/rendering/glescenegraph/quest/vrapi/GLESWindowFramebuffer.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace Quest
{

Framebuffer* GLESEngineQuest::internalCreateFramebuffer(const Framebuffer::FramebufferType type, const Framebuffer::FramebufferConfig& config)
{
	if (type == Framebuffer::FRAMEBUFFER_WINDOW)
	{
		return new VrApi::GLESWindowFramebuffer(config);
	}

	return nullptr;
}

}

}

}

}
