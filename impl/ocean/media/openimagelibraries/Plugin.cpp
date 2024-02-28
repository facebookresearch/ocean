// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/openimagelibraries/Plugin.h"
#include "ocean/media/openimagelibraries/OILLibrary.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	Ocean::Media::OpenImageLibraries::OILLibrary::registerLibrary();

	return true;
}

bool pluginUnload()
{
	return Ocean::Media::OpenImageLibraries::OILLibrary::unregisterLibrary();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
