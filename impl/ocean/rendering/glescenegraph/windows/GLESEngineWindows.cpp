/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/windows/GLESEngineWindows.h"
#include "ocean/rendering/glescenegraph/windows/GLESBitmapFramebuffer.h"
#include "ocean/rendering/glescenegraph/windows/GLESWindowFramebuffer.h"

#include "ocean/rendering/Manager.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace Windows
{

GLESEngineWindows::GLESEngineWindows(const GraphicAPI graphicAPI) :
	GLESEngine(graphicAPI)
{
	// nothing to do here
}

void GLESEngineWindows::registerEngine()
{
	Engine::registerEngine(nameGLESceneGraphEngine(), CreateCallback(&GLESEngineWindows::createEngine), glesGraphicAPI(), 10);
}

Framebuffer* GLESEngineWindows::internalCreateFramebuffer(const Framebuffer::FramebufferType type, const Framebuffer::FramebufferConfig& /*config*/)
{
	if (type == Framebuffer::FRAMEBUFFER_WINDOW)
	{
		return new Windows::GLESWindowFramebuffer();
	}
	else if (type == Framebuffer::FRAMEBUFFER_BITMAP)
	{
		return new Windows::GLESBitmapFramebuffer();
	}

	return nullptr;
}

Engine* GLESEngineWindows::createEngine(const GraphicAPI graphicAPI)
{
	Engine* newEngine = new GLESEngineWindows(graphicAPI);

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
