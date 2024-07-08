/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/ffmpeg/FFMLibrary.h"
#include "ocean/media/ffmpeg/FFMMovie.h"

extern "C" {
#include <libavformat/avformat.h>
}

namespace Ocean
{

namespace Media
{

namespace FFmpeg
{

FFMLibrary::FFMLibrary() :
	Library(nameFFmpegLibrary(), 50u)
{
	Log::debug() << "FFmpeg version: " << av_version_info();

	av_log_set_callback(FFMLibrary::avLogCallback);
	av_log_set_level(AV_LOG_INFO);
}

FFMLibrary::~FFMLibrary()
{
	// nothing to do here
}

bool FFMLibrary::registerLibrary()
{
	return Manager::get().registerLibrary<FFMLibrary>(nameFFmpegLibrary());
}

bool FFMLibrary::unregisterLibrary()
{
	return Manager::get().unregisterLibrary(nameFFmpegLibrary());
}

MediumRef FFMLibrary::newMedium(const std::string& url, bool useExclusive)
{
	const ScopedLock scopedLock(lock);

	MediumRef medium(newMovie(url, useExclusive));

	return medium;
}

MediumRef FFMLibrary::newMedium(const std::string& url, const Medium::Type type, bool useExclusive)
{
	const ScopedLock scopedLock(lock);

	switch (type)
	{
		case Medium::MOVIE:
		{
			return newMovie(url, useExclusive);
		}

		case Medium::FRAME_MEDIUM:
		{
			MediumRef medium(newMovie(url, useExclusive));

			return medium;
		}

		default:
			break;
	}

	return MediumRef();
}

RecorderRef FFMLibrary::newRecorder(const Recorder::Type type)
{
	return RecorderRef();
}

Medium::Type FFMLibrary::supportedTypes() const
{
	return Medium::Type(Medium::MOVIE);
}

LibraryRef FFMLibrary::create()
{
	return LibraryRef(new FFMLibrary());
}

MediumRef FFMLibrary::newMovie(const std::string& url, bool useExclusive)
{
	if (useExclusive == false)
	{
		MediumRef mediumRef(MediumRefManager::get().medium(url, nameFFmpegLibrary(), Medium::MOVIE));

		if (mediumRef)
		{
			return mediumRef;
		}
	}

	FFMMovie* medium = new FFMMovie(url);
	ocean_assert(medium != nullptr);

	if (medium->isValid() == false)
	{
		delete medium;
		return MediumRef();
	}

	if (useExclusive)
	{
		return MediumRef(medium);
	}

	return MediumRefManager::get().registerMedium(medium);
}

void FFMLibrary::avLogCallback(void* context, int level, const char* message, va_list arguments)
{
	ocean_assert(message != nullptr);

	if (level <= AV_LOG_ERROR)
	{
		Log::error() << "Media::FFmpeg: " << message;
	}
	else if (level <= AV_LOG_WARNING)
	{
		Log::warning() << "Media::FFmpeg: " << message;
	}
}

}

}

}
