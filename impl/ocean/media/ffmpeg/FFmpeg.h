/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_FFM_FFMPEG_H
#define META_OCEAN_MEDIA_FFM_FFMPEG_H

#include "ocean/media/Media.h"

namespace Ocean
{

namespace Media
{

namespace FFmpeg
{

/**
 * @ingroup media
 * @defgroup mediaffm Ocean Media FFmpeg Library
 * @{
 * The Ocean Media FFmpeg Library provides media functionalities based on the FFmpeg video/audio library.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Media::FFmpeg Namespace of the Media FFmpeg library.<p>
 * The Namespace Ocean::Media::FFmpeg is used in the entire Ocean Media FFmpeg Library.
 */

/**
 * Returns the name of this media FFmpeg library.
 * @ingroup mediaffm
 */
std::string nameFFmpegLibrary();

#if defined(OCEAN_RUNTIME_STATIC)

/**
 * Registers this media library at the global library manager.
 * This function calls FFMLibrary::registerLibrary() only.
 * @ingroup mediaffm
 */
void registerFFmpegLibrary();

/**
 * Unregisters this media library at the global library manager.
 * This function calls FFMLibrary::unregisterLibrary() only.
 * @return True, if succeeded
 * @ingroup mediaffm
 */
bool unregisterFFmpegLibrary();

#endif // OCEAN_RUNTIME_STATIC

}

}

}

#endif // META_OCEAN_MEDIA_FFM_FFMPEG_H
