/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GIEngine.h"
#include "ocean/rendering/globalillumination/GIBitmapFramebuffer.h"
#include "ocean/rendering/globalillumination/GIWindowFramebuffer.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

const Engine::GraphicAPI giGraphicAPI(Engine::API_RAYTRACER);

GIEngine::GIEngine(const GraphicAPI preferredGraphicAPI) :
	Engine(giGraphicAPI),
	engineLocalName(globalIlluminationEngineName())
{
	ocean_assert((preferredGraphicAPI & API_OPENGL) == API_OPENGL || (preferredGraphicAPI & API_RAYTRACER) == API_RAYTRACER || preferredGraphicAPI == API_DEFAULT);

	graphicAPI_ = API_OPENGL;

	if (preferredGraphicAPI & API_RAYTRACER)
	{
		Log::info() << "Raytracer successfully initialized.";
		graphicAPI_ = API_RAYTRACER;
	}

	ocean_assert(graphicAPI_ != API_DEFAULT);
}

GIEngine::~GIEngine()
{
	ocean_assert(!ObjectRefManager::get().hasEngineObject(engineLocalName, true));
}

void GIEngine::registerEngine()
{
	static Lock lock;
	static bool registered = false;

	const ScopedLock scopedLock(lock);

	if (registered)
	{
		ocean_assert(false && "Only one GIEngine object!");
		return;
	}

	Engine::registerEngine(globalIlluminationEngineName(), CreateCallback(&GIEngine::createEngine), giGraphicAPI, 10);
	registered = true;
}

bool GIEngine::unregisterEngine()
{
	return Engine::unregisterEngine(globalIlluminationEngineName());
}

Engine* GIEngine::createEngine(const GraphicAPI graphicAPI)
{
	Engine* newEngine = new GIEngine(graphicAPI);

	if (newEngine == nullptr)
	{
		throw OutOfMemoryException("Not enough memory to create a new engine.");
	}

	return newEngine;
}

const Factory& GIEngine::factory() const
{
	return engineFactory;
}

const std::string& GIEngine::engineName() const
{
	return engineLocalName;
}

Framebuffer* GIEngine::internalCreateFramebuffer(const Framebuffer::FramebufferType type, const Framebuffer::FramebufferConfig& /* config */)
{
	GIFramebuffer* newFramebuffer = nullptr;

	if (type == Framebuffer::FRAMEBUFFER_WINDOW)
	{
		newFramebuffer = new GIWindowFramebuffer(graphicAPI_);

		if (newFramebuffer == nullptr)
		{
			throw OutOfMemoryException("Not enough memory to create a new framebuffer.");
		}
	}
	else if (type == Framebuffer::FRAMEBUFFER_BITMAP)
	{
		newFramebuffer = new GIBitmapFramebuffer(graphicAPI_);

		if (newFramebuffer == nullptr)
		{
			throw OutOfMemoryException("Not enough memory to create a new framebuffer.");
		}
	}

	return newFramebuffer;
}

}

}

}
