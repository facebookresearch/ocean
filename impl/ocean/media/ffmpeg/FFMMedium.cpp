/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/ffmpeg/FFMMedium.h"
#include "ocean/media/ffmpeg/FFMLibrary.h"

extern "C" {
#include <libavformat/avformat.h>
}

namespace Ocean
{

namespace Media
{

namespace FFmpeg
{

FFMMedium::FFMMedium(const std::string& url) :
	Medium(url)
{
	libraryName_ = nameFFmpegLibrary();

	isValid_ = createContextAndOpenFile(url);
}

FFMMedium::~FFMMedium()
{
	releaseContext();
}

bool FFMMedium::isStarted() const
{
	const ScopedLock scopedLock(lock_);

	return startTimestamp_.isValid();
}

Timestamp FFMMedium::startTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	return startTimestamp_;
}

Timestamp FFMMedium::pauseTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	return pauseTimestamp_;
}

Timestamp FFMMedium::stopTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	return stopTimestamp_;
}

bool FFMMedium::start()
{
	if (!isValid_)
	{
		ocean_assert(false && "Invalid medium");
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (startTimestamp_.isValid())
	{
		return true;
	}

	startTimestamp_.toNow();
	pauseTimestamp_.toInvalid();
	stopTimestamp_.toInvalid();

	if (!internalStart())
	{
		startTimestamp_.toInvalid();

		return false;
	}

	return true;
}

bool FFMMedium::pause()
{
	if (!isValid_)
	{
		ocean_assert(false && "Invalid medium");
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (pauseTimestamp_.isValid())
	{
		return true;
	}

	startTimestamp_.toInvalid();
	pauseTimestamp_.toNow();
	stopTimestamp_.toInvalid();

	if (!internalPause())
	{
		pauseTimestamp_.toInvalid();

		return false;
	}

	return true;
}

bool FFMMedium::stop()
{
	if (!isValid_)
	{
		ocean_assert(false && "Invalid medium");
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (stopTimestamp_.isValid())
	{
		return true;
	}

	startTimestamp_.toInvalid();
	pauseTimestamp_.toInvalid();
	stopTimestamp_.toNow();

	if (!internalStop())
	{
		stopTimestamp_.toInvalid();

		return false;
	}

	return true;
}

bool FFMMedium::createContextAndOpenFile(const std::string& filename)
{
	ocean_assert(avFormatContext_ == nullptr);
	ocean_assert(!filename.empty());

	int result = avformat_open_input(&avFormatContext_, filename.c_str(), nullptr /*format*/, nullptr /*options*/);

	if (result < 0)
	{
		Log::error() << "FFmpeg: Failed to open '" << filename << "': " << av_err2str(result);

		return false;
	}

	ocean_assert(avFormatContext_ != nullptr);

	result = avformat_find_stream_info(avFormatContext_, nullptr);

	if (result < 0)
	{
		Log::error() << "FFmpeg: Failed to find media streams in '" << filename << "': " << av_err2str(result);

		return false;
	}

	return true;
}

void FFMMedium::releaseContext()
{
	if (avFormatContext_ != nullptr)
	{
		avformat_close_input(&avFormatContext_);
		avFormatContext_ = nullptr;
	}
}

}

}

}
