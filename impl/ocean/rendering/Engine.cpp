/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Engine.h"

#include "ocean/rendering/Manager.h"

namespace Ocean
{

namespace Rendering
{

Engine::Engine(const GraphicAPI graphicAPIs) :
	graphicAPI_(API_DEFAULT),
	supportedGraphicAPIs_(graphicAPIs),
	timestamp_(false)
{
	// nothing to do here
}

Engine::~Engine()
{
	// nothing to do here
}

Engine::Framebuffers Engine::framebuffers() const
{
	const ScopedLock scopedLock(lock_);

	Framebuffers result;

	for (const ObjectId& framebufferId : framebufferIds_)
	{
		FramebufferRef framebuffer(ObjectRefManager::get().object(framebufferId));

		if (framebuffer)
		{
			result.push_back(framebuffer);
		}
	}

	return result;
}

FramebufferRef Engine::createFramebuffer(const Framebuffer::FramebufferType type, const Framebuffer::FramebufferConfig& config)
{
	Framebuffer* framebuffer = internalCreateFramebuffer(type, config);

	if (framebuffer == nullptr)
	{
		return FramebufferRef();
	}

	TemporaryScopedLock engineScopedLock(lock_);
		framebufferIds_.emplace_back(framebuffer->id());
	engineScopedLock.release();

	return ObjectRefManager::get().registerObject(framebuffer);
}

void Engine::update(const Timestamp timestamp)
{
	TemporaryScopedLock engineScopedLock(lock_);

		timestamp_ = timestamp;

		const Engine::Framebuffers framebufferObjects(framebuffers());

		ocean_assert(!framebufferObjects.empty());
		if (framebufferObjects.empty())
		{
			return;
		}

		const ViewRef view = framebufferObjects.front()->view();

		if (view.isNull())
		{
			return;
		}

	engineScopedLock.release();


	TemporaryScopedLock dynamicObjectScopedLock(objectLock_);
		const ObjectIds dynamicObjectsCopy(dynamicObjects_);
	dynamicObjectScopedLock.release();

	for (const ObjectId& dynamicObjectId : dynamicObjectsCopy)
	{
		const DynamicObjectRef dynamicObject(object(dynamicObjectId));

		if (dynamicObject)
		{
			dynamicObject->onDynamicUpdate(view, timestamp);
		}
	}
}

ObjectRef Engine::object(const ObjectId objectId) const
{
	const ObjectRef objectRef(ObjectRefManager::get().object(objectId));

	if (objectRef.isNull() || objectRef->engineName() != engineName())
	{
		return ObjectRef();
	}

	return objectRef;
}

ObjectRef Engine::object(const std::string& name) const
{
	const ObjectRefs objects(ObjectRefManager::get().objects(name));

	for (const ObjectRef& object : objects)
	{
		ocean_assert(object);

		if (object->engineName() == engineName())
		{
			return object;
		}
	}

	return ObjectRef();
}

ObjectRefs Engine::objects(const std::string& name) const
{
	ObjectRefs objects(ObjectRefManager::get().objects(name));
	ObjectRefs result;

	for (ObjectRef& object : objects)
	{
		ocean_assert(object);

		if (object->engineName() == engineName())
		{
			result.emplace_back(std::move(object));
		}
	}

	return result;
}

std::string Engine::extensions() const
{
	const ScopedLock scopedLock(lock_);

	try
	{
		const Framebuffers framebufferRefs(framebuffers());

		if (!framebufferRefs.empty())
		{
			ocean_assert(framebufferRefs.front());
			return framebufferRefs.front()->extensions();
		}
	}
	catch(...)
	{
		// nothing to do here
	}

	return std::string();
}

bool Engine::hasExtension(const std::string& extension) const
{
	const ScopedLock scopedLock(lock_);

	try
	{
		const Framebuffers framebufferRefs(framebuffers());

		for (const FramebufferRef& framebufferRef : framebufferRefs)
		{
			ocean_assert(framebufferRef);

			if (framebufferRef->hasExtension(extension))
			{
				return true;
			}
		}
	}
	catch(...)
	{
		// nothing to do here
	}

	return false;
}

void Engine::registerDynamicObject(DynamicObject* dynamicObject)
{
	if (dynamicObject == nullptr)
	{
		return;
	}

	const ScopedLock scopedLock(objectLock_);

#ifdef OCEAN_DEBUG

	for (const ObjectId& dynamicObjectId : dynamicObjects_)
	{
		ocean_assert(dynamicObjectId != dynamicObject->id());
	}

#endif

	dynamicObjects_.emplace_back(dynamicObject->id());
}

void Engine::unregisterDynamicObject(DynamicObject* dynamicObject)
{
	if (dynamicObject == nullptr)
		return;

	const ScopedLock scopedLock(objectLock_);

	for (ObjectIds::iterator i = dynamicObjects_.begin(); i != dynamicObjects_.end(); ++i)
	{
		if (*i == dynamicObject->id())
		{
			dynamicObjects_.erase(i);
			break;
		}
	}
}

void Engine::registerEngine(const std::string& engineName, const CreateCallback& callback, const GraphicAPI graphicAPI, const unsigned int priority)
{
	Manager::get().registerEngine(engineName, callback, graphicAPI, priority);
}

bool Engine::unregisterEngine(const std::string& engine)
{
	return Manager::get().unregisterEngine(engine);
}

}

}
