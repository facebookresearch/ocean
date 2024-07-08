/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/apple/GLESEngineApple.h"
#include "ocean/rendering/glescenegraph/apple/GLESBitmapFramebuffer.h"
#include "ocean/rendering/glescenegraph/apple/GLESWindowFramebuffer.h"

#include "ocean/rendering/Manager.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace Apple
{

GLESEngineApple::GLESEngineApple(const GraphicAPI graphicAPI) :
	GLESEngine(graphicAPI)
{
	// nothing to do here
}

void GLESEngineApple::registerEngine()
{
	Engine::registerEngine(nameGLESceneGraphEngine(), CreateCallback(&GLESEngineApple::createEngine), glesGraphicAPI(), 10);
}

Framebuffer* GLESEngineApple::internalCreateFramebuffer(const Framebuffer::FramebufferType type, const Framebuffer::FramebufferConfig& /* config */)
{
	if (type == Framebuffer::FRAMEBUFFER_WINDOW)
	{
		return new Apple::GLESWindowFramebuffer();
	}
	else if (type == Framebuffer::FRAMEBUFFER_BITMAP)
	{
#ifdef OCEAN_PLATFORM_BUILD_APPLE_MACOS
		return new Apple::GLESBitmapFramebuffer();
#endif
	}

	return nullptr;
}

Engine* GLESEngineApple::createEngine(const GraphicAPI graphicAPI)
{
	Engine* newEngine = new GLESEngineApple(graphicAPI);

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
