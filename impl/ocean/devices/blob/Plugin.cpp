// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/blob/Plugin.h"
#include "ocean/devices/blob/BlobFactory.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	return Ocean::Devices::Blob::BlobFactory::registerFactory();
}

bool pluginUnload()
{
	return Ocean::Devices::Blob::BlobFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
