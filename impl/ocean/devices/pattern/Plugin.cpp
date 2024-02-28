// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/pattern/Plugin.h"
#include "ocean/devices/pattern/PatternFactory.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	return Ocean::Devices::Pattern::PatternFactory::registerFactory();
}

bool pluginUnload()
{
	return Ocean::Devices::Pattern::PatternFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
