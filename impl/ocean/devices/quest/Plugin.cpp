// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/quest/Plugin.h"
#include "ocean/devices/quest/QuestFactory.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	return Ocean::Devices::Ouclus::QuestFactory::registerFactory();
}

bool pluginUnload()
{
	return Ocean::Devices::Ouclus::QuestFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
