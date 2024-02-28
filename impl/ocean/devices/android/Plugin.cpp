// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/android/Plugin.h"
#include "ocean/devices/android/AndroidFactory.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	return Ocean::Devices::Android::AndroidFactory::registerFactory();
}

bool pluginUnload()
{
	return Ocean::Devices::Android::AndroidFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
