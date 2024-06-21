/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/OceanManager.h"
#include "ocean/base/Messenger.h"

namespace Ocean
{

OceanManager::OceanManager()
{
	// nothing to do here
}

OceanManager::~OceanManager()
{
	shutdown();
}

OceanManager& OceanManager::get()
{
	static OceanManager* singletonManager = nullptr;

	if (!singletonManager)
	{
		static Lock lock;
		const ScopedLock scopedLock(lock);

		if (!singletonManager)
		{
			singletonManager = new OceanManager();
			std::atexit(internalRelease);
		}
	}

	return *singletonManager;
}

void OceanManager::registerSingleton(const SingletonDestroyFunction& destroySingleton)
{
	ocean_assert(destroySingleton);

	const ScopedLock scopedLock(lock_);

#ifdef OCEAN_DEBUG
	for (const SingletonDestroyFunction& object : singletonDestroyFunctions_)
	{
		ocean_assert(object != destroySingleton);
	}
#endif

	singletonDestroyFunctions_.push_back(destroySingleton);
}

void OceanManager::shutdown()
{
#ifdef OCEAN_INTENSIVE_DEBUG
	Messenger::writeToDebugOutput("OceanManager::shutdown()");
#endif

	const ScopedLock scopedLock(lock_);

	// we destroy all singleton in reverse order

	for (SingletonDestroyFunctions::const_reverse_iterator i = singletonDestroyFunctions_.rbegin(); i != singletonDestroyFunctions_.rend(); ++i)
	{
		(*i)();
	}

	singletonDestroyFunctions_.clear();
}

void OceanManager::internalRelease()
{
	delete &get();
}

}
