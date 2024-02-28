// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/imageio/Plugin.h"
#include "ocean/media/imageio/IIOLibrary.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	Ocean::Media::ImageIO::IIOLibrary::registerLibrary();

	return true;
}

bool pluginUnload()
{
	return Ocean::Media::ImageIO::IIOLibrary::unregisterLibrary();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
