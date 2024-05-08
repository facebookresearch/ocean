/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Manager.h"

namespace Ocean
{

namespace Rendering
{

Manager::EngineInformation::EngineInformation(const std::string& name, const Engine::CreateCallback& callback, const Engine::GraphicAPI graphicAPI) :
	name_(name),
	createCallback_(callback),
	graphicAPI_(graphicAPI)
{
	ocean_assert(createCallback_);
	ocean_assert(graphicAPI_ != Engine::API_DEFAULT);
}

EngineRef Manager::EngineInformation::engine(const Engine::GraphicAPI preferredGraphicAPI)
{
	if (engine_.isNull())
	{
		ocean_assert(createCallback_);

		engine_ = EngineRef(createCallback_(preferredGraphicAPI), EngineRef::ReleaseCallback(Manager::get(), &Manager::onRemoveEngine));

		ocean_assert(engine_);
		return engine_;
	}

	return engine_;
}

void Manager::EngineInformation::release()
{
	engine_.release();
}

bool Manager::EngineInformation::isNull() const
{
	return engine_.isNull();
}

Manager::~Manager()
{
	release();
}

EngineRef Manager::engine(const std::string& engine, const Engine::GraphicAPI graphicAPI)
{
	const ScopedLock scopedLock(lock_);

	if (engineMap_.empty())
	{
		return EngineRef();
	}

	if (!engine.empty())
	{
		for (EngineMap::reverse_iterator i = engineMap_.rbegin(); i != engineMap_.rend(); ++i)
		{
			if (engine == i->second.name())
			{
				return i->second.engine(graphicAPI);
			}
		}
	}

	for (EngineMap::reverse_iterator i = engineMap_.rbegin(); i != engineMap_.rend(); ++i)
	{
		if (i->second.graphicAPI() & graphicAPI)
		{
			return i->second.engine(graphicAPI);
		}
	}

	return engineMap_.rbegin()->second.engine(graphicAPI);
}

Manager::EngineNames Manager::engines()
{
	const ScopedLock scopedLock(lock_);

	EngineNames names;

	for (EngineMap::const_reverse_iterator i = engineMap_.rbegin(); i != engineMap_.rend(); ++i)
	{
		names.push_back(i->second.name());
	}

	return names;
}

Engine::GraphicAPI Manager::supportedGraphicAPI(const std::string& engine)
{
	const ScopedLock scopedLock(lock_);

	for (EngineMap::const_reverse_iterator i = engineMap_.rbegin(); i != engineMap_.rend(); ++i)
	{
		if (engine == i->second.name())
		{
			return i->second.graphicAPI();
		}
	}

	return Engine::API_DEFAULT;
}

void Manager::registerEngine(const std::string& engineName, const Engine::CreateCallback& callback, const Engine::GraphicAPI graphicAPI, const unsigned int priority)
{
	ocean_assert(engineName.empty() == false && callback);

	const ScopedLock scopedLock(lock_);

#ifdef OCEAN_DEBUG
	for (EngineMap::const_iterator i = engineMap_.begin(); i != engineMap_.end(); ++i)
	{
		ocean_assert(i->second.name() != engineName);
	}
#endif

	engineMap_.insert(std::make_pair(priority, EngineInformation(engineName, callback, graphicAPI)));
}

bool Manager::unregisterEngine(const std::string& engine)
{
	const ScopedLock scopedLock(lock_);

	for (EngineMap::iterator i = engineMap_.begin(); i != engineMap_.end(); ++i)
	{
		if (i->second.name() == engine)
		{
#ifdef OCEAN_DEBUG
			if (!i->second.isNull())
			{
				ocean_assert(!ObjectRefManager::get().hasEngineObject(engine, true));
				ocean_assert(false && "The engine is still in use!");
			}
#endif

			engineMap_.erase(i);
			return true;
		}
	}

	return false;
}

void Manager::onRemoveEngine(const Engine* engine)
{
	ocean_assert(engine);

	const ScopedLock scopedLock(lock_);

	for (EngineMap::iterator i = engineMap_.begin(); i != engineMap_.end(); ++i)
	{
		if (i->second.name() == engine->engineName())
		{
			i->second.release();
			return;
		}
	}

	ocean_assert(false && "This should never happen!");
}

void Manager::release()
{
	const ScopedLock scopedLock(lock_);

	for (EngineMap::iterator i = engineMap_.begin(); i != engineMap_.end(); ++i)
	{
		ocean_assert(i->second.isNull());
	}

	engineMap_.clear();
}

}

}
