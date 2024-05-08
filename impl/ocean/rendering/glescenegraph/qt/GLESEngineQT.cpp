/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/qt/GLESEngineQT.h"
#include "ocean/rendering/glescenegraph/qt/GLESWindowFramebuffer.h"

#include "ocean/rendering/Manager.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace QT
{

GLESEngineQT::GLESEngineQT(const GraphicAPI graphicAPI) :
	GLESEngine(graphicAPI)
{
	// nothing to do here
}

void GLESEngineQT::registerEngine()
{
	Engine::registerEngine(nameGLESceneGraphEngine(), CreateCallback(&GLESEngineQT::createEngine), glesGraphicAPI(), 10);
}

Framebuffer* GLESEngineQT::internalCreateFramebuffer(const Framebuffer::FramebufferType type, const Framebuffer::FramebufferConfig& config)
{
	if (type == Framebuffer::FRAMEBUFFER_WINDOW)
	{
		return new QT::GLESWindowFramebuffer();
	}

	return nullptr;
}

Engine* GLESEngineQT::createEngine(const GraphicAPI graphicAPI)
{
	Engine* newEngine = new GLESEngineQT(graphicAPI);

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
