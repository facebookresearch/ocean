// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/ffmpeg/FFmpeg.h"
#include "ocean/media/ffmpeg/FFMLibrary.h"

namespace Ocean
{

namespace Media
{

namespace FFmpeg
{

std::string nameFFmpegLibrary()
{
	return std::string("FFmpeg");
}

#if defined(OCEAN_RUNTIME_STATIC)

void registerFFmpegLibrary()
{
	FFMLibrary::registerLibrary();
}

bool unregisterFFmpegLibrary()
{
	return FFMLibrary::unregisterLibrary();
}

#endif // OCEAN_RUNTIME_STATIC

}

}

}
