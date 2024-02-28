// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/arcore/Plugin.h"
#include "ocean/devices/arcore/ACFactory.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	return Ocean::Devices::ARCore::ACFactory::registerFactory();
}

bool pluginUnload()
{
	return Ocean::Devices::ARCore::ACFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
