// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/directshow/Plugin.h"
#include "ocean/media/directshow/DSLibrary.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	Ocean::Media::DirectShow::DSLibrary::registerLibrary();

	return true;
}

bool pluginUnload()
{
	return Ocean::Media::DirectShow::DSLibrary::unregisterLibrary();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
