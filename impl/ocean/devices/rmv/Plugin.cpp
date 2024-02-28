// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/rmv/Plugin.h"
#include "ocean/devices/rmv/RMVFactory.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	return Ocean::Devices::RMV::RMVFactory::registerFactory();
}

bool pluginUnload()
{
	return Ocean::Devices::RMV::RMVFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
