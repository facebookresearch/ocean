// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/cloud/Plugin.h"
#include "ocean/devices/cloud/CloudFactory.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	return Ocean::Devices::Cloud::CloudFactory::registerFactory();
}

bool pluginUnload()
{
	return Ocean::Devices::Cloud::CloudFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
