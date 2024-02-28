// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/vrs/Plugin.h"
#include "ocean/devices/vrs/VRSFactory.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	return Ocean::Devices::VRS::VRSFactory::registerFactory();
}

bool pluginUnload()
{
	return Ocean::Devices::VRS::VRSFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
