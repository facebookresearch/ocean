// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/ios/Plugin.h"
#include "ocean/devices/ios/IOSFactory.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	return Ocean::Devices::IOS::IOSFactory::registerFactory();
}

bool pluginUnload()
{
	return Ocean::Devices::IOS::IOSFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
