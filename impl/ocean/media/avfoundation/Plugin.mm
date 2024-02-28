// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/avfoundation/Plugin.h"
#include "ocean/media/avfoundation/AVFLibrary.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	Ocean::Media::AVFoundation::AVFLibrary::registerLibrary();

	return true;
}

bool pluginUnload()
{
	return Ocean::Media::AVFoundation::AVFLibrary::unregisterLibrary();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
