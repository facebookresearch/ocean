/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
