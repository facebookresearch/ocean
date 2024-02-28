// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/vrs/Plugin.h"
#include "ocean/media/vrs/VRSLibrary.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	Ocean::Media::VRS::VRSLibrary::registerLibrary();

	return true;
}

bool pluginUnload()
{
	return Ocean::Media::VRS::VRSLibrary::unregisterLibrary();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
