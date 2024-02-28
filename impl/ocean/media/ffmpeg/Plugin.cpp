// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/ffmpeg/Plugin.h"
#include "ocean/media/ffmpeg/FFMLibrary.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	Ocean::Media::FFmpeg::FFMLibrary::registerLibrary();

	return true;
}

bool pluginUnload()
{
	return Ocean::Media::FFmpeg::FFMLibrary::unregisterLibrary();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
