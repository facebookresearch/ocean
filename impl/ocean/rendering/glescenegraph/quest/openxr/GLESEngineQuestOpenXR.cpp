// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/glescenegraph/quest/GLESEngineQuest.h"

#include "ocean/rendering/glescenegraph/quest/openxr/GLESWindowFramebuffer.h"

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
		return new OpenXR::GLESWindowFramebuffer(config);
	}

	return nullptr;
}

}

}

}

}
