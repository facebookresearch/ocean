/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/android/GLESEngineAndroid.h"
#include "ocean/rendering/glescenegraph/android/GLESWindowFramebuffer.h"

#include "ocean/rendering/Manager.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace Android
{

GLESEngineAndroid::GLESEngineAndroid(const GraphicAPI graphicAPI) :
	GLESEngine(graphicAPI)
{
	// nothing to do here
}

void GLESEngineAndroid::registerEngine()
{
	Engine::registerEngine(nameGLESceneGraphEngine(), CreateCallback(&GLESEngineAndroid::createEngine), glesGraphicAPI(), 10);
}

Framebuffer* GLESEngineAndroid::internalCreateFramebuffer(const Framebuffer::FramebufferType type, const Framebuffer::FramebufferConfig& /* config */)
{
	if (type == Framebuffer::FRAMEBUFFER_WINDOW)
	{
		return new Android::GLESWindowFramebuffer();
	}

	return nullptr;
}

Engine* GLESEngineAndroid::createEngine(const GraphicAPI graphicAPI)
{
	Engine* newEngine = new GLESEngineAndroid(graphicAPI);

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
