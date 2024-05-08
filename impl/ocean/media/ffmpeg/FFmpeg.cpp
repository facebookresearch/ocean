/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
